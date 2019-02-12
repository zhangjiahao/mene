//match_engine implementation
#include "match_engine.h"

#define  MAX_PRICE_SIZE   (1024 * 64) 
#define  MAX_TICK_SIZE    (1024 * 256) 
#define  MAX_ORDER_SIZE   (1024 * 128) 
#define  ARRAY_SIZE(a)    (sizeof(a)/sizeof(a[0]))

static better_price_func  compare_price[2] = {better_buy, better_sell};
extern process_quote_func quote_process[MI_SIZE];

double get_midprice(double p1, double p2)
{
	return (compare(p1, 0) > 0 && compare(p2, 0) > 0) ? (p1 + p2) / 2 : p1 + p2;
}

//-----------MatchEngine external interface----------

MatchEngine::MatchEngine(char *symbol)
{
	m_split_data  = new MyArray(sizeof(pv_pair), MAX_TRD_PV_CNT);
	m_trade_rtn   = new MyArray(sizeof(trade_return_struct), MAX_PLACE_SIZE);
	m_place_orders= new MyArray(sizeof(order_node), MAX_ORDER_SIZE);

	m_price_data = new MyArray(sizeof(price_node), MAX_PRICE_SIZE);
	m_ticks_data = new MyArray(sizeof(tick_node), MAX_TICK_SIZE);
	m_place_hash = new PlaceOrderMap();
	clear();

#ifdef TestDebug
	char log_name[TOKEN_BUFFER_SIZE] = "";
	snprintf(log_name, TOKEN_BUFFER_SIZE, "logs/match_engine_%s.log", symbol);
	m_write_log.open(log_name, std::ios_base::trunc);
	m_write_log << "Current Match Engine Version: V1.1.0\n";
#endif
}

MatchEngine::~MatchEngine()
{
	delete  m_split_data;
	delete  m_trade_rtn;
	delete  m_place_orders;

	delete  m_price_data;
	delete  m_ticks_data;
	delete  m_place_hash;
#ifdef TestDebug
	m_write_log.close();
#endif
}

void 
MatchEngine::clear()
{
	m_entrust_num = 0;
	m_tick_counter = 0;
	m_place_num = 0;
	m_last_empty = false;
	INIT_LIST_HEAD(&m_buy_prices);         /* prev, next set to self */
	INIT_LIST_HEAD(&m_sell_prices);        /* prev, next set to self */
	memset(&m_quote, 0, sizeof(quote_info));

	m_price_data->reset();
	m_ticks_data->reset();
	m_place_orders->reset();
	m_place_hash->clear();
}

void
MatchEngine::set_config(match_engine_config* cfg)
{
	memcpy(&m_cfg, cfg, sizeof(match_engine_config));
}

void  MatchEngine::round_finish()
{
#ifdef TestDebug
	m_write_log << "Param exch=" << m_cfg.mode_cfg.exch << " product=" << m_cfg.mode_cfg.product
		        << " trade_ratio=" << m_cfg.mode_cfg.trade_ratio << std::endl;
#endif
}

bool 
MatchEngine::is_empty_order()
{
	return list_empty(&m_buy_prices) && list_empty(&m_sell_prices);
}

void
MatchEngine::place_order(place_order_struct* pl_ord, order_return_struct *ord_rtn)
{
	// we didn't pass the order to m_place_hash structure
	order_node* node = NULL;
	if (pl_ord->volume <= 0 ) {
        // invalid order, return reject directly
		ord_rtn->entrust_status = STAT_REJECT;
		goto place_end;
	}
    // get next available buffer
	node = (order_node *)m_place_orders->next();
	if (node == NULL){
		printf("Error: Can't get available order_node\n");
		assert(0);
	}
	memcpy(&node->pl_ord, pl_ord, sizeof(place_order_struct));
    // entrust_no is a identification that broker send to client to identify a order operation
	node->entrust_no = (++m_entrust_num);
	ord_rtn->entrust_status = STAT_NEW;
	// this can be another copy of data to local array
	m_place_array[m_place_num++] = node;
	node->data_flag = m_is_filter;

place_end:
    // sync the basic information that passed to this module
	ord_rtn->serial_no = pl_ord->serial_no;
	memcpy(ord_rtn->contr, pl_ord->contr, MAX_NAME_SIZE);
	ord_rtn->direction = pl_ord->direction;
	ord_rtn->open_close = pl_ord->open_close;
	ord_rtn->entrust_no = m_entrust_num;
	ord_rtn->limit_price = pl_ord->limit_price;
	ord_rtn->volume = pl_ord->volume;
	ord_rtn->volume_remain = pl_ord->volume;
	ord_rtn->xchg_code = pl_ord->xchg_code;
}

void
MatchEngine::cancel_order(cancel_order_struct* cl_ord, order_return_struct *ord_rtn)
{
	// cancel order by the serial_no (maybe use entrust_no is better while exchange use entrust_no do this)
	// try to remove order from the hang order list
	auto need = m_place_hash->end();
	order_node* node = remove_order_from_pool(cl_ord);
	if (node != NULL){
		// mark the node status is STAT_CANCEL
		node->status = STAT_CANCEL;
		ord_rtn->entrust_status = node->status;
		goto canl_end;
	}

	// if order not found locally
	need = m_place_hash->find(cl_ord->org_serial_no);
	if (need == m_place_hash->end()){
		ord_rtn->serial_no = cl_ord->org_serial_no;
		ord_rtn->entrust_status = STAT_CANCEL_REJECT;
		memcpy(ord_rtn->contr, cl_ord->contr, MAX_NAME_SIZE);
		ord_rtn->direction = cl_ord->direction;
		ord_rtn->open_close = cl_ord->open_close;
		ord_rtn->xchg_code = cl_ord->xchg_code;
		ord_rtn->volume = 0;
		ord_rtn->volume_remain = 0;
		return;
	}
	// pointed pl_order
	node = need->second;
	if (node->status == STAT_FULL 
	 || node->status == STAT_CANCEL){
		ord_rtn->entrust_status = STAT_CANCEL_REJECT;
		goto canl_end;
	}
	node->status = STAT_CANCEL;
	// delete order from local hash map
	m_place_hash->erase(need);
	update_order_node(node, (int)node->volume);
	ord_rtn->entrust_status = node->status;

canl_end:
	ord_rtn->serial_no = node->pl_ord.serial_no;
	memcpy(ord_rtn->contr, node->pl_ord.contr, MAX_NAME_SIZE);
	ord_rtn->direction = node->pl_ord.direction;
	ord_rtn->open_close = node->pl_ord.open_close;
	ord_rtn->entrust_no = node->entrust_no;
	ord_rtn->xchg_code = node->pl_ord.xchg_code;
	ord_rtn->volume = node->pl_ord.volume;
	// this can be error, because it could happened partial volume trade success, but we dont hava a correct remain
	// volume here, it should be total_volume - trade_volume
	ord_rtn->volume_remain = node->volume;
}

order_node* 
MatchEngine::remove_order_from_pool(cancel_order_struct* cl_ord)
{
	for (int i = 0; i < m_place_num; i++){
		order_node* node = m_place_array[i];
		if (node->pl_ord.serial_no == cl_ord->org_serial_no){
			// if cancel a lot orders this can task a long time O(n), this copy a lot of things
			for (int start = i; start < m_place_num - 1; start++) {
				m_place_array[start] = m_place_array[start+1];
			}
			m_place_num--;
			return node;
		}
	}
	return NULL;
}

void
MatchEngine::feed_quote(int type, void* quote, bool is_first)
{
	m_mi_type = type;
	// default quote is mark as filtered
	m_is_filter = Filter;
    // check mi_type
	if (g_unlikely(type >= ARRAY_SIZE(quote_process) || NULL == quote_process[type])) {
		return;
	}
    // tick index, this will always add, so index with strategy can be the same
	m_tick_counter++;
	if (m_quote.mult_way > 1){
		if (type == MI_MY_LEVEL1){
            // multichannel quote source only considering the mi_my_level1
			goto Convert;
		} else {
			// not process the multi-level quote
			return;
		}
	} else {
        /* at most take into two kinds of mi_type now, update the quote style in m_quote */
		add_mult_quote_way(type);
	}

    /* level3 quote (lv1 part) is not considering */
	if (type == MI_SHFE_MY_LVL3 && extract_data_flag(quote) < 3){
		return;
	}

Convert:
	// not exist any customer's order in match_engine
	if (!update_first_quote(type, quote)){
		return;
	}

	memcpy(&m_quote.last_quote, &m_quote.curr_quote, sizeof(mat_quote));
	// the first piece of quote (last_quote is same with curr_quote)
	quote_process[type](quote, &m_quote);
    // make opposite same as curr_quote, this will be changed later
	memcpy(&m_quote.opposite, &m_quote.curr_quote, sizeof(mat_quote));
	// current quote mark as not filter
	m_is_filter = Process;
}

void  
MatchEngine::feed_internal_book(void* quote, bool is_first)
{
	m_tick_counter++;
	if (m_place_num == 0 && is_empty_order()){
		m_is_filter = Filter;
		m_last_empty = true;
		m_last_quote = quote;
		return;
	}else{
		if (m_last_empty){
			process_internal_book(m_last_quote, &m_quote);
		}
		m_last_empty = false;
		m_last_quote = quote;
	}

	memcpy(&m_quote.last_quote, &m_quote.curr_quote, sizeof(mat_quote));
	process_internal_book(quote, &m_quote);
    // curr_quote has been update in previous sentence
	memcpy(&m_quote.opposite, &m_quote.curr_quote, sizeof(mat_quote));
	m_is_filter = Process;
}

void 
MatchEngine::execute_match(trade_return_struct** trd_rtn_ar, int* rtn_cnt)
{
    // clear my_trade_rtn size
	if (g_unlikely(m_trade_rtn->size() > 0)){
		m_trade_rtn->reset();
	}
    // quote not fill into engine so needn't to process the match, there is no new quote come to here
	if (m_is_filter == Filter){
		goto proc_end;
	}

	/* clear the m_split_data in each execute_match */
	m_split_data->clear();
    /* do two price split */
	calc_trade_volume();

	/* processed with new orders (parse from m_place_array) */
	update_order_book();

#ifdef TestDebug
	show_price_list();
#endif
    /* this check is not necessary */
	if (m_is_filter == Process) {
		do_match_on_direct(DIR_BUY);
		do_match_on_direct(DIR_SELL);
	}

#ifdef TestDebug
	show_price_list(false);
#endif

proc_end:
	*rtn_cnt = m_trade_rtn->size();
	*trd_rtn_ar = (trade_return_struct *)m_trade_rtn->head();
}

//-----------MatchEngine internal interface----------
void 
MatchEngine::update_order_book()
{
	price_node* pn = NULL;
	tick_node * tn = NULL;
	place_order_struct* pl_ord = NULL;

	for (int i = 0; i < m_place_num; i++){
		order_node* node = m_place_array[i];
		pl_ord = &node->pl_ord;
		/**
		 * records all place order linked structure with same price
		 **/
		pn = add_price_node(pl_ord->direction, pl_ord->limit_price);
		/**
		 * create the market order which will complete with us firstly
		 **/
		update_market_book(pn, pl_ord->direction, pl_ord->limit_price);
		/**
		 * find tick node possess by our company
		 */
		tn = find_tick_node(pn, m_tick_counter, OWNER_MY);
		if (tn == NULL){
			tn = add_tick_node(pn, m_tick_counter, OWNER_MY);
		}
		add_order_node(tn, node);
		/* pass the place_order volume to the node->volume */
		node->volume = pl_ord->volume;
		/* tick node records all volume trigger same price and same direction same tick */
		tn->volume += pl_ord->volume;
		/* price node records all volume of same price */
		pn->volume += pl_ord->volume;
	}
	/* update those message into local hang orders */
	m_place_num = 0;
}

void  
MatchEngine::update_market_book(price_node* pn, char direct, double price)
{
	tick_node * tn = find_tick_node(pn, m_tick_counter, OWNER_MKT);
	/**
	 * if it is the first order triggered by this tick of quote in current price_node
	 * this means we create a new price_node here
	 **/
	if (tn == NULL){
		/**
		 * get specified direct volume in current and previous quote
		 * get the volume diff in current tick
		 */
		pn->last_vol = find_market_volume(direct, price, false);
		pn->curr_vol = find_market_volume(direct, price, true);

		int	split_vol = find_trade_volume(price);
		/**
		 * generated volume in current and previous tick of quote, this means market volume can compete with us part
		 */
		int	rto_vol = int(m_cfg.mode_cfg.trade_ratio * (pn->curr_vol - pn->last_vol + split_vol));
		/**
		 * if market exist volume more than history mkt_vol, it means more market tick_node should hang in price_node
		 */
		int mkt_vol = (int)pn->last_vol + rto_vol - (int)pn->mkt_vol;
		if (g_unlikely(mkt_vol > 0)){
			/* mkt tick_node before my tick_node, this means this part will trade before my order */
			tn = add_tick_node(pn, m_tick_counter, OWNER_MKT);
			tn->volume = mkt_vol;
			/* this is pn->last_vol + rto_vol, means compete volume with pn->volume */
			pn->mkt_vol += mkt_vol;
		}
	}
}

price_node* 
MatchEngine::add_price_node(char direct, double price)
{
	price_node* node = NULL;
	list_head* pos, *insert_pos; 
	list_head* head = get_price_handle(direct);
	int cmp_vol, bad_ret = BAD_RESULT(direct);

	insert_pos = head;
	list_for_each(pos, head){
		/* this means list in price_node pointed by price_node_hdl */
		node = list_entry(pos, price_node, list);
		cmp_vol = compare(price, node->price);
		if (cmp_vol == 0)  return node;
		if (cmp_vol == bad_ret) {
			insert_pos = pos;
			goto not_find;
		}else{
			insert_pos = pos->next;
		}
	}

not_find:
	node = (price_node *)m_price_data->next();
	if (node == NULL){
		printf("Error: Can't get available price_node\n");
		assert(0);
	}else{
		/* mark price_node in using */
		node->used = 1;
		node->price = price;
		node->direct = direct;
		node->volume = 0;
		node->mkt_vol = 0;
		node->last_vol = 0;
		node->curr_vol = 0;
		INIT_LIST_HEAD(&node->list);
		INIT_LIST_HEAD(&node->tick_list);
		// insert this node to specified insert_pos in price_node
		list_add_before(&node->list, insert_pos);
	}
	return node;
}

tick_node*  
MatchEngine::find_tick_node(price_node* pn, long tick, int owner)
{
	tick_node *node = NULL;
	list_head* head, *pos;
	head = &pn->tick_list;

	list_for_each(pos, head){
		/* tick_node is order by tick_count number */
		node = list_entry(pos, tick_node, list);
		if (tick < node->tick){
			return NULL;
		}
		if (node->tick == tick && node->owner == owner){
			return node;
		}
	}
	return NULL;
}

tick_node*  
MatchEngine::add_tick_node(price_node* pn, long tick, int owner)
{
	list_head* head = &pn->tick_list;
	tick_node *node = (tick_node *)m_ticks_data->next();
	if (node == NULL){
		printf("Error: Can't get available tick_node\n");
		assert(0);
	}else{
		node->used = 1;
		node->tick = tick;
		node->owner = owner;
		/* point the price node */
		node->parent = pn;
		node->volume = 0;
		/* it could appear more than one order there */
		INIT_LIST_HEAD(&node->ord_list);
		/* insert the node to price_node->tick_list front */
		list_add_before(&node->list, head);
	}
	return node;
}

order_node*
MatchEngine::add_order_node(tick_node * tn, order_node* node)
{
	list_head* head = &tn->ord_list;
	node->parent = tn;
	node->status = STAT_NEW;
	list_add_before(&node->list, head);
	/*add the order into local hash map*/
	m_place_hash->insert(std::make_pair(node->pl_ord.serial_no, node));
	return node;
}

list_head *
MatchEngine::get_price_handle(char direct){
	if (direct == DIR_BUY){
		return &m_buy_prices;
	}else{
		return &m_sell_prices;
	}
}

void
MatchEngine::update_tick_node(tick_node* tn, int trad_vol)
{
	/* update the volume in tick_node and father price_node */
	price_node* pn = tn->parent;
	list_head* pos, *p, *head;
	tn->volume -= trad_vol;	
	if (tn->owner == OWNER_MKT){
		// market wanted to buy volume
		pn->mkt_vol -= trad_vol;
	}else{
		// our wanted to buy volume
		pn->volume -= trad_vol;
	}
	if (tn->volume <= 0){
		// if tick volume equal 0 then ignore this node and remove from our tick node list
		list_del(&tn->list);
		tn->used = 0;
	}
	if (pn->volume <= 0){
		// if price node volume equal 0 then remove from price node list
		list_del(&pn->list);
		// update the flag, mark this item is not usable now
		pn->used = 0;
		// a price node contains may contains many tick node, remove each tick node in this price node
		head = &pn->tick_list;
		// delete all tick_node belong to this price_node
		list_for_each_safe(pos, p, head){
			tick_node *node = list_entry(pos, tick_node, list);
			/* order_node in this list has been release when it finished */
			list_del(&node->list);
			node->used = 0;
		}
	}
}

void
MatchEngine::update_order_node(order_node* on, int trad_vol)
{
	on->volume -= trad_vol;
	if (on->volume <= 0){
		// remove this order_node from the m_place_order list
		list_del(&on->list);
	}
	tick_node* tn = on->parent;
	update_tick_node(tn, trad_vol);
}

/* fill trade_rtn info into return list */
void 
MatchEngine::set_order_result(order_node* order)
{
	trade_return_struct *rtn = NULL;
	rtn = (trade_return_struct *)m_trade_rtn->next();
	rtn->serial_no = order->pl_ord.serial_no;
	memcpy(rtn->contr, order->pl_ord.contr, MAX_NAME_SIZE);
	rtn->direction = order->pl_ord.direction;
	rtn->open_close = order->pl_ord.open_close;
	rtn->entrust_no = order->entrust_no;
	rtn->business_price = order->trade_price;
	rtn->business_volume = order->trade_vol;
	rtn->xchg_code = order->pl_ord.xchg_code;
	rtn->entrust_status = order->status;
}

void  
MatchEngine::do_match_on_direct(char direct)
{
	price_node* node = NULL;
	list_head* pos, *p, *head;

	/* head is the price_node list */
	head = get_price_handle(direct);
    /* enumerate all price_node and do match */
	list_for_each_safe(pos, p, head) {
		node = list_entry(pos, price_node, list);
		int over = is_overstep(node->direct, node->price);
		/* over means trade use a stupid price make orders, it will buy all possible volume in level quote*/
		if (g_unlikely(over)){
			do_match_on_overstep(node, direct);
		}else{
			do_match_on_price(node, direct);
		}
	}
}

void  
MatchEngine::do_match_on_price(price_node *pn, char direct)
{
	int self_trade = 0;
	pn->last_vol = pn->curr_vol;
	pn->curr_vol = find_market_volume(direct, pn->price, true);
	int	split_vol = find_trade_volume(pn->price);

	/* market canceled traded volume, volume generaged tick range, assure only cancel order in this time range */
	int canl_vol = (int)pn->last_vol - (int)pn->curr_vol - split_vol;
    /* only match the market order, decrease the hang volume of the market, this may not right */
	do_match_on_tick(pn, canl_vol, false, self_trade);

	double avg_price = 0.0;
	/* use this price to match then make it a avg_price to trade and get the total volume */
	int opp_vol = find_opposite_volume(direct, pn->price, avg_price);

	do_match_on_opposite(pn, opp_vol, avg_price);
	/* do match pair in m_split_data */
	do_match_can_trade(pn);
}

void  
MatchEngine::do_match_on_overstep(price_node *pn, char direct)
{
	tick_node *node = NULL;
	list_head* pos, *p, *head;
	head = &pn->tick_list;
	double exe_price = 0.0;

	list_for_each_safe(pos, p, head){
		node = list_entry(pos, tick_node, list);
		/* trade_vol return equal node->volume in most case */
		int trade_vol = calc_overstep_data(direct, (int)node->volume, exe_price);
		/* this will always be true at first */
		if (trade_vol > 0){
			trade_vol -= do_match_on_order(node, trade_vol, exe_price);
			// the real trade_vol has been updated here
			update_tick_node(node, trade_vol);
		}
	}
}

void 
MatchEngine::do_match_can_trade(price_node *pn)
{
	int self_trade = 0;
	/* get the can trade part in m_split_data */
	find_trade_volume(pn->price, pn->direct);
	for (int i = 0; i < m_can_trade_num; i++){
		pv_pair *trade_pair = m_can_trade_list[i];
		trade_pair->vol = do_match_on_tick(pn, trade_pair->vol, true, self_trade);
	}
}

int
MatchEngine::do_match_on_tick(price_node *pn, int exe_vol, bool is_all, int &self_trade)
{
	self_trade = 0;
	if (g_unlikely(exe_vol <= 0)) return 0;
	tick_node *node = NULL;
	list_head* pos, *p, *head;
	int trade_vol, remain_vol = exe_vol;

	head = &pn->tick_list;
	list_for_each_safe(pos, p, head) {
		node = list_entry(pos, tick_node, list);
		if (is_all || node->owner == OWNER_MKT) {
			trade_vol = (int)MIN(node->volume, remain_vol);
			if (node->owner == OWNER_MY){
				int real_vol = do_match_on_order(node, trade_vol);
				trade_vol -= real_vol;
				self_trade += trade_vol;
			}
			remain_vol -= trade_vol;
			update_tick_node(node, trade_vol);
			if (remain_vol <= 0) break;
		}
	}
	return remain_vol;
}

int  
MatchEngine::do_match_on_opposite(price_node *pn, int exe_vol, double exe_price)
{
	if (g_unlikely(exe_vol <= 0)) return 0;
	tick_node *node = NULL;
	list_head* pos, *p, *head;
	int remain_vol = exe_vol;

	head = &pn->tick_list;
	list_for_each_safe(pos, p, head){
		node = list_entry(pos, tick_node, list);
		/* if we can't consume a tick_node then exit directly */
		if (node->volume > remain_vol) break;
		if (node->owner == OWNER_MY){
            // this can trigger real trade_rtn, only the MY contains the order_node list
			do_match_on_order(node, (int)node->volume, exe_price);
		}
		remain_vol -= node->volume;
		update_tick_node(node, (int)node->volume);
		if (remain_vol <= 0) break;
	}
	return remain_vol;
}

#if 0
int  
MatchEngine::do_match_on_order(tick_node *tn, int exe_vol, double exe_price)
{
	order_node * node = NULL;
	list_head* pos, *p, *head;
	int trade_vol = 0;
	int remain_vol = exe_vol;
	head = &tn->ord_list;

	list_for_each_safe(pos, p, head){
		node = list_entry(pos, order_node, list);
		if (is_skip_order(node)) continue;
		trade_vol = MIN(node->volume, remain_vol);
		if (g_unlikely(trade_vol <= 0)) continue;

		node->volume -= trade_vol;
		remain_vol -= trade_vol;
		node->trade_vol = trade_vol;
		if (g_unlikely(exe_price > 0)){
			node->trade_price = exe_price;
		}else{
			node->trade_price = node->pl_ord.limit_price;
		}
		if (node->volume <= 0){
			node->status = STAT_FULL;
			list_del(&node->list);
		}else{
			node->status = STAT_PARTIAL;
		}

		set_order_result(node);
		if (remain_vol <= 0) break;
	}
	return remain_vol;
}
#endif

void 
MatchEngine::statis_order(tick_node *tn, int &count, int &total)
{
	order_node * node = NULL;
	list_head* head, *pos;
	count = total = 0;
	head = &tn->ord_list;

	list_for_each(pos, head){
		node = list_entry(pos, order_node, list);
		if (is_skip_order(node)) continue;
		total += node->pl_ord.volume;
		count++;
	}
}

/* return the remain_volume after match of a tick_node */
int
MatchEngine::do_match_on_order(tick_node *tn, int exe_vol, double exe_price)
{
	order_node * node = NULL;
	list_head* pos, *p, *head;
	int trade_vol = 0, proc_index = 0;
	int remain_vol = exe_vol;
	int order_count, total_pos;
	/* calc statistical order volume and count bind some tick_node */
	statis_order(tn, order_count, total_pos);

	/* nothing wait to trade in current tick_node */
	if (total_pos <= 0) return remain_vol;

	head = &tn->ord_list;
	list_for_each_safe(pos, p, head){
		node = list_entry(pos, order_node, list);
		/* order triggered by ctp part will not match here*/
		if (is_skip_order(node)) continue;

		if (proc_index >= order_count - 1){ //last order
			trade_vol = (int)MIN(node->volume, remain_vol);
		}else {
			// each order hash same opportunity to trade some volume
			int avg_vol = (int) ceil(remain_vol * node->pl_ord.volume / (double) total_pos);
			trade_vol = (int) MIN(node->volume, avg_vol);
		}
		if (g_unlikely(trade_vol <= 0)) continue;

		node->volume -= trade_vol;
		/* update the remain_vol to match */
		remain_vol -= trade_vol;
		/* store current trade_vol in order_node */
		node->trade_vol = trade_vol;
		/* later order have less opportunity to trade */
		total_pos -= node->pl_ord.volume;
		/* unlikey means have less probability to happen */
		if (g_unlikely(exe_price > 0)){
			node->trade_price = exe_price;
		}else{
			node->trade_price = node->pl_ord.limit_price;
		}
		/* update the status of order_node */
		if (node->volume <= 0){
			/*if a order_node finished then remove from order_list */
			node->status = STAT_FULL;
			list_del(&node->list);
		}else{
			node->status = STAT_PARTIAL;
		}

		proc_index++;
		set_order_result(node);
		if (remain_vol <= 0) break;
	}
	return remain_vol;
}

/* predict trade price and volume in time between previous tick and current tick */
void
MatchEngine::calc_trade_volume()
{
	pv_pair *high = (pv_pair *)m_split_data->next();
	pv_pair *low = (pv_pair *)m_split_data->next();

	mat_quote *q_curr = &m_quote.curr_quote;
	mat_quote *q_last = &m_quote.last_quote;

	pv_pair* b_last = q_last->bs_pv[0]; // it is quote data buy price and volume
	pv_pair* s_last = q_last->bs_pv[1]; // it is quote data sell price and volume

	double tickSize = m_cfg.pi.unit;
	int trade_unit = m_cfg.pi.multiple;

    // total trade amount in previous tick (convert to standard unit, like  money * lot / kg)
	// this can be zero because of the earlier not fill right amount value
	double diff_amount = (q_curr->amount - q_last->amount) / trade_unit;
	int diff_vol = q_curr->total_volume - q_last->total_volume;              // lot number

	// this is normal and standard entrance
	if (diff_amount > 0 && diff_vol > 0){
        // call the average trade price convert it to be a valid price(this less one)
		double p0 = int(diff_amount / diff_vol / tickSize) * tickSize;
		high->vol = (int)((diff_amount - diff_vol * p0 + PRECISION) / tickSize);
		low->vol = diff_vol - high->vol;
		low->price = p0;
		high->price = p0 + tickSize;
        // better price trade at price: p0 and volume: diff_vol - high->vol
        // vice-better price trade at price: p0 + tickSize, volume: high->vol
	}
	/* why this happen, diff_vol > 0 while diff_amount == 0? 233 because we don't have the amount item in quote
 	 * while the diff_vol exist, which means we know total trade volume between previous and current tick
 	 * */
	else if (diff_amount == 0 && diff_vol > 0){
        // get the trade average price of current bid/ask (handicap price) buy1 and sell1
		double midp1 = get_midprice(q_curr->bs_pv[0][0].price, q_curr->bs_pv[1][0].price);
        // get the trade average price of previous bid/ask (handicap price)
		double midp0 = get_midprice(b_last[0].price, s_last[0].price);

		//////////////////normal////////////////////
		/* this is standard two price split method */
		//low->price = int((midp0 + q_curr->last_price) / 2.0 / tickSize)*tickSize;
		//high->price = low->price + tickSize;
		//if (fabs(midp1 - midp0) < tickSize / 2){
		//	high->vol = diff_vol / 2;
		//}
		//else if (midp1 - midp0 > tickSize / 2){
		//	high->vol = diff_vol;
		//}
		//else{
		//	high->vol = 0;
		//}
		//low->vol = diff_vol - high->vol;

		//////////Here By Zhang.jialiang modify////////
		/* the handicap price keep still */
		if (fabs(midp1 - midp0) < PRECISION) {
            // the price not change in current tick0
			low->price = int(midp1 / tickSize)*tickSize;
            // less than the fall limit price, this trigger limit move
			if (low->price < m_quote.low_limit - PRECISION){
				high->price = int(midp1 / tickSize + 0.5)*tickSize;
				low->vol = 0; // not possible trade at low_limit price
				high->vol = diff_vol;
			}else{
				high->price = low->price + tickSize;
                // low price predict perfect
				if (fabs(midp1 - low->price) < PRECISION || high->price > m_quote.high_limit + PRECISION){
					low->vol = diff_vol;
					high->vol = 0;
				} else {
                    // half of volume into each side
					high->vol = diff_vol / 2;
					low->vol = diff_vol - high->vol;
				}
			}
		}
		else if (midp1 - midp0 >= PRECISION){
            // the handicap price increased
			low->price = int(q_curr->bs_pv[0][0].price / tickSize)*tickSize; // current buy1 price
			if (fabs(low->price) < 2 * tickSize || low->price < m_quote.low_limit - PRECISION){
                // if low->price error or reach fall limit then ceil of current sell price
				high->price = int(q_curr->bs_pv[1][0].price / tickSize + 0.5)*tickSize; // sell1 price
				low->vol = 0;
				high->vol = diff_vol;
			}else{
				high->price = low->price + tickSize;
                // if current buy price predict perfect or reach raise limit
				if (fabs(q_curr->bs_pv[0][0].price - low->price) < PRECISION || high->price > m_quote.high_limit + PRECISION){
					low->vol = diff_vol;
					high->vol = 0;
				}else{
					high->vol = diff_vol / 2;
					low->vol = diff_vol - high->vol;
				}
			}
		}
		else{
            // the handicap price decreased
			low->price = int(q_curr->bs_pv[1][0].price / tickSize)*tickSize; // current sell price
			if (low->price < m_quote.low_limit - PRECISION){
                // if price price is int fall limit
				high->price = int(q_curr->bs_pv[1][0].price / tickSize + 0.5)*tickSize;
				low->vol = 0;
				high->vol = diff_vol;
			}else{
				high->price = low->price + tickSize;
                // current sell price predict perfect or high price reach the rise price
				if (fabs(q_curr->bs_pv[1][0].price - low->price) < PRECISION || high->price > m_quote.high_limit + PRECISION){
					low->vol = diff_vol;
					high->vol = 0;
				}else{
					high->vol = diff_vol / 2;
					low->vol = diff_vol - high->vol;
				}
			}
		}
	}
}

int
MatchEngine::find_trade_volume(double price)
{
	/**
	 * we know the size of m_split_data is 1 by far, it only records the split pair_pv (high one and low one)
	 * in the future we may split the trade items into several price and it's corresponding volume
	 */
	for (unsigned int i = 0; i < m_split_data->size(); i++){
		pv_pair* pair = (pv_pair *)m_split_data->at(i);
		if (compare(price, pair->price) == 0) {
			return pair->vol;
		}
	}
	return 0;
}

void 
MatchEngine::find_trade_volume(double price, char dir)
{
	pv_pair* pair = NULL;
	m_can_trade_num = 0;
	//Buy: price >= split_price  add pair
	//Sell: price <= split_price  add pair
	if (dir == DIR_BUY){
		for (unsigned int i = 0; i < m_split_data->size(); i++){
			pair = (pv_pair *)m_split_data->at(i);
			if (is_can_trade(pair->price, price, dir)
				|| compare(price, pair->price) == 0){
				m_can_trade_list[m_can_trade_num++] = pair;
			}
		}
	}else{
		for (int i = (int)m_split_data->size() - 1; i >= 0; i--){
			pair = (pv_pair *)m_split_data->at(i);
			if (is_can_trade(pair->price, price, dir)
				|| compare(price, pair->price) == 0){
				m_can_trade_list[m_can_trade_num++] = pair;
			}
		}
	}
}

int  
MatchEngine::find_market_volume(char dir, double price, bool is_curr)
{
	pv_pair* pv_ar = NULL;
	if (is_curr){
		pv_ar = m_quote.curr_quote.bs_pv[dir-DIR_BUY]; // 0-buy, 1-sell
	}else{
		pv_ar = m_quote.last_quote.bs_pv[dir-DIR_BUY];
	}

	int cmp_vol, bad_ret = BAD_RESULT(dir);
    // exact price the return the volume
	for (int i = 0; i < QUOTE_LVL_CNT; i++) {
		/* illeagle price data or data not exist */
		if (pv_ar[i].price <= 0)  return 0;
		cmp_vol = compare(price, pv_ar[i].price);
		/* our buy price < market sell price or our sell price > market buy price */
		if (cmp_vol == bad_ret)  return 0;
		/* buy-sell corredponding parter */
		if (cmp_vol == 0) return  pv_ar[i].vol;
	}
	return 0;
}

int 
MatchEngine::find_opposite_volume(char dir, double price, double &exe_price)
{
	pv_pair* pv_ar = m_quote.opposite.bs_pv[SWITCH_DIR(dir)];
	int cmp_vol, bad_ret = BAD_RESULT(dir);

	int total_vol = 0;
	double total_amount = 0.0;
	for (int i = 0; i < QUOTE_LVL_CNT; i++) {
		/* some opposite price not right then return 0 */
		if (pv_ar[i].price <= 0)  return 0;
		cmp_vol = compare(price, pv_ar[i].price);
		if (cmp_vol == bad_ret){
			/* price better than the opposite side */
			total_amount += pv_ar[i].price * pv_ar[i].vol;
			total_vol += pv_ar[i].vol;
			continue;
		}else if (cmp_vol == 0) {
			/* price exactlly equal to the opposite side */
			exe_price = pv_ar[i].price;
			return pv_ar[i].vol;
		}else{
			exe_price = total_vol > 0 ? total_amount / total_vol : 0;
			return (i > 0 ? pv_ar[i - 1].vol : 0);
		}
	}
	return 0;
}

int   
MatchEngine::is_overstep(char dir, double price)
{
	/* reverse direction and compare to the level5 price */
	pv_pair* pv_ar = m_quote.curr_quote.bs_pv[SWITCH_DIR(dir)];
	/* last level price of quote */
	if (pv_ar[QUOTE_LVL_CNT - 1].price > 0){
		/* the price is in normal price order list */
		return (compare(price, pv_ar[QUOTE_LVL_CNT - 1].price) == (int)BAD_RESULT(dir));
	}else{
		/* time when start trading */
		return 0;
	}
}

bool  
MatchEngine::is_skip_order(order_node* node)
{
	/* in m_mi_type equal lv3 and data_flag = filter means that is ctp partial of quote */
	if (m_mi_type == MI_SHFE_MY_LVL3 &&
		m_tick_counter == node->parent->tick &&
		node->data_flag == Filter) {
		return true;
	}
	return false;
}

int  
MatchEngine::is_can_trade(double market, double self, char dir)
{
	return compare_price[dir-DIR_BUY](self, market);
}

int 
MatchEngine::calc_overstep_data(char dir, int volume, double &exe_price)
{
	/* order price not in the level quote calculate a execute price and volume */
	double total_amount = 0.0;
	int total_vol = 0, real_vol = 0, trade_vol[QUOTE_LVL_CNT];
	pv_pair* pv_ar = m_quote.opposite.bs_pv[SWITCH_DIR(dir)];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		trade_vol[i] = int(pv_ar[i].vol * m_cfg.mode_cfg.trade_ratio);
		total_vol += trade_vol[i];
		total_amount += pv_ar[i].price * trade_vol[i];
	}
	/* amount capital we can make it trade */
	exe_price = total_amount / total_vol;
	/* market provided volume more than we wanted to buy */
	if (total_vol > volume){
		for (int i = 0; i < QUOTE_LVL_CNT-1; i++){
			/* TODO: why we should buy/sell with ratio different level quote */
			trade_vol[i] = int(volume * trade_vol[i] / total_vol);
			real_vol += trade_vol[i];
		}
		int remain_vol = pv_ar[4].vol > (volume-real_vol) ? (volume-real_vol) : pv_ar[4].vol;
		trade_vol[4] = remain_vol;
		total_vol = volume;
	}

	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		/*update the data in opposite pv_pair */
		pv_ar[i].vol -= trade_vol[i];
	}
	return total_vol;
}

void  
MatchEngine::add_mult_quote_way(int m_type)
{
	for (int i = 0; i < m_quote.mult_way; i++){
        /* check if this exist or not */
		if (m_quote.mi_types[i] == m_type) return;
	}
	m_quote.mi_types[m_quote.mult_way++] = m_type;
}

bool  
MatchEngine::update_first_quote(int type, void *quote)
{
    /* update the previous quote */
	bool result = true;
    // no place order is waiting to process, so we need't tranverse the orders
	if (m_place_num == 0 && is_empty_order()){
		m_last_empty = true;
		result = false;
		goto proc_end;
	} else {
		// this branch means that someone place_order between previous and current tick of quote
		if (m_last_empty){ //update last quote
            /* m_last_type is previous quote type, records the information we care about into m_quote */
			/* records those quote from the second one (useful one)*/
			quote_process[m_last_type](m_last_quote, &m_quote);
		}
		m_last_empty = false;
		result = true;
		goto proc_end;
	}

proc_end:
	m_last_type = type;
	m_last_quote = quote;   // this means we can't free previous quote when it's cycle finished
	return result;
}

#ifdef TestDebug
void MatchEngine::show_price_list(bool flag)
{
	price_node* node = NULL;
	list_head* pos, *p, *head;
	if (m_tick_counter > 0){
		m_write_log << "tick=" << m_tick_counter << " split:";
		for (unsigned int i = 0; i < m_split_data->size(); i++){
			pv_pair *tmp = (pv_pair *)m_split_data->at(i);
			m_write_log << " price=" << tmp->price << " vol=" << tmp->vol;
		}
		m_write_log << std::endl;
		if (flag){
			pv_pair* pv_buy = m_quote.curr_quote.bs_pv[0];
			pv_pair* pv_sel = m_quote.curr_quote.bs_pv[1];
			m_write_log << "Time " << m_quote.curr_quote.time << " buy " << pv_buy[0].price << "#" << pv_buy[0].vol << "  "
				<< pv_buy[1].price << "#" << pv_buy[1].vol << "  " << pv_buy[2].price << "#" << pv_buy[2].vol
				<< " | sell " << pv_sel[0].price << "#" << pv_sel[0].vol << "  "
				<< pv_sel[1].price << "#" << pv_sel[1].vol << "  " << pv_sel[2].price << "#" << pv_sel[2].vol << std::endl;
		}
		head = get_price_handle(DIR_SELL);
		list_for_each_safe(pos, p, head){
			node = list_entry(pos, price_node, list);
			if (node->volume > 0){
				m_write_log << "Sell List:";
				show_tick_list(node);
				m_write_log << std::endl;
			}
		}
		head = get_price_handle(DIR_BUY);
		list_for_each_safe(pos, p, head){
			node = list_entry(pos, price_node, list);
			if (node->volume > 0){
				m_write_log << "Buy List:";
				show_tick_list(node);
				m_write_log << std::endl;
			}
		}
	}
}

void MatchEngine::show_tick_list(price_node* pn)
{
	list_head* head, *pos;
	head = &pn->tick_list;
	m_write_log << "price=" << pn->price << " volume=" << pn->volume << " mkt_vol=" << pn->mkt_vol;
	list_for_each(pos, head){
		tick_node *node = list_entry(pos, tick_node, list);
		m_write_log << " tick=" << node->tick << " vol=" << node->volume << " owner=" << node->owner;
		show_order_list(node);
	}
}

void MatchEngine::show_order_list(tick_node *tn)
{
	list_head* head, *pos;
	head = &tn->ord_list;
	list_for_each(pos, head){
		order_node * node = list_entry(pos, order_node, list);
		m_write_log << " order=" << node->volume << " status=" << node->status;
	}
}
#endif