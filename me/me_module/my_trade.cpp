#include  <assert.h>
#include  <fstream>
#include  "match_engine.h"
#include  "extend_match.h"
#include  "my_trade.h"
#include  "list.h"
#include  "utils.h"

#define  HASH_TABLE_SIZE      (8192)
#define  MAX_ITEM_SIZE        (4096)
#define  THRESHOLD_SIZE       (2)
#define  MODE_CONF_PATH       "config/match_engine.config"

struct match_node{
	char           symbol[MAX_NAME_SIZE];       // trade symbol
	MatchEngine   *match;                       // match engine entity
	Recorder      *record;                      // records internal data, do nothing now
	list_head      hs_link;
};

struct hash_bucket{
	list_head      head;
};

//---------------------EngineHash implementation-----------------------

EngineHash::EngineHash()
{
	m_use_count = 0;
	int size = MAX_ITEM_SIZE * sizeof(match_node) + HASH_TABLE_SIZE * sizeof(hash_bucket);
	// filled with zero
	void * all_mem = calloc(1, size);
	assert(all_mem);

	char *table = (char *)all_mem;
	m_match_head = (match_node *)(all_mem);
	m_bucket = (hash_bucket *)(table + MAX_ITEM_SIZE * sizeof(match_node));
	clear();
}

EngineHash::~EngineHash()
{
	for (int i = 0; i < MAX_ITEM_SIZE; i++){
		match_node *node = m_match_head + i;
		if (node->match != NULL){
			delete node->match;
			node->match = NULL;
		}
	}
	if (m_match_head != NULL){
		free(m_match_head);
		m_match_head = NULL;
	}
}

void
EngineHash::clear()
{
	for (int i = 0; i < HASH_TABLE_SIZE; i++){
		INIT_LIST_HEAD(&m_bucket[i].head);
	}
	for (int i = 0; i < m_use_count; i++){
		match_node *node = m_match_head + i;
		memset(node->symbol, 0, MAX_NAME_SIZE);
		INIT_LIST_HEAD(&node->hs_link);
		node->match->clear();
		//delete node->record;
	}
	m_use_count = 0;
}

void EngineHash::round_finish()
{
	for (int i = 0; i < m_use_count; i++){
		match_node *node = m_match_head + i;
		node->match->round_finish();
	}
}

match_node *
EngineHash::add(char* symbol, product_info *product, mode_config *mode, task_conf_t *task)
{
	if (g_unlikely(m_use_count >= MAX_ITEM_SIZE)) 
		return NULL;

	match_engine_config me_cfg = { 0 };
	memcpy(me_cfg.contract, symbol, MAX_NAME_SIZE);
	memcpy(&me_cfg.task, task, sizeof(task_conf_t));
	memcpy(&me_cfg.pi, product, sizeof(product_info));
	memcpy(&me_cfg.mode_cfg, mode, sizeof(mode_config));

	match_node *node = &m_match_head[m_use_count];
	memcpy(node->symbol, symbol, MAX_NAME_SIZE);
	if (node->match == NULL){
		if (mode->mode == NORMAL_MATCH){
			node->match = new MatchEngine(symbol);
		}else if (mode->mode == IDEAL_MATCH){
			node->match = new IdealMatch(symbol);
		}else if (mode->mode == LINEAR_IMPACT_MATCH){
			node->match = new LinearMatch(symbol);
		}else if (mode->mode == DOUBLE_GAUSSIAN_MATCH){
			node->match = new GaussianMatch(symbol);
		}else if (mode->mode == SPLIT_TWO_PRICE_MATCH){
			node->match = new OriginMatch(symbol);
		}else{
			printf("Match engine mode error!\n");
			assert(node->match);
		}
	}
	//node->record = new Recorder(&me_cfg);
	node->match->set_config(&me_cfg);
	unsigned long index = hash_value(symbol);
	list_add_after(&node->hs_link, &m_bucket[index].head);
	m_use_count++;
	return node;
}

match_node *
EngineHash::find(char* symbol)
{
	if (m_use_count > THRESHOLD_SIZE){
		return hash_find(symbol);
	}else{
		return traverse_find(symbol);
	}
}

match_node *
EngineHash::find(int index)
{
	return m_match_head + index;
}

match_node *
EngineHash::hash_find(char* symbol)
{
	list_head *pos, *next;
	unsigned long index = hash_value(symbol);
	list_for_each_safe(pos, next, &m_bucket[index].head){
		match_node *node = list_entry(pos, match_node, hs_link);
		if (my_strcmp(node->symbol, symbol) == 0) {
			return node;
		}
	}
	return NULL;
}

match_node *
EngineHash::traverse_find(char* symbol)
{
	for (int i = 0; i < m_use_count; i++){
		match_node *node = m_match_head + i;
		if (g_likely(my_strcmp(node->symbol, symbol) == 0))
			return node;
	}
	return NULL;
}

/* hash the symbol into bucket */
unsigned long 
EngineHash::hash_value(char *symbol)
{
	unsigned int hash = my_hash_value(symbol);
	return hash & (HASH_TABLE_SIZE - 1);
}

//---------------------Recorder implementation-----------------------

Recorder::Recorder(match_engine_config *cfg)
{
}

Recorder::~Recorder()
{
}

//---------------------MyTrader implementation-----------------------

MyTrader::MyTrader(int match_mode)
{
	m_pi_cnt = 0;
	m_match_mode = match_mode;
	m_pi_data = (product_info *)malloc(MAX_ITEM_SIZE * sizeof(product_info));  // this can contains many product info
	m_stock_pi = (product_info *)malloc(sizeof(product_info));                 // like above, this is for stock(one)
	m_engine = new EngineHash();                                               // match_engine manager
	m_mode_cfg = new MyArray(sizeof(mode_config), MAX_NAME_SIZE);              // contains all product->mode, ratio
	//read_mode_config();
}

MyTrader::~MyTrader()
{
	free(m_pi_data);
	free(m_stock_pi);
	delete m_engine;
	delete m_mode_cfg;
}

void MyTrader::clear()
{
	if (m_engine->size() > 0){
		m_engine->clear();
	}
}

void MyTrader::round_finish()
{
	if (m_engine->size() > 0){
		m_engine->round_finish();
	}
}

void MyTrader::set_task_conf(task_conf_t *task)
{
	memcpy(&m_task_cfg, task, sizeof(task_conf_t));
}

int
MyTrader::load_product(product_info* pi, int count)
{
	if (g_unlikely(NULL == pi || count <= 0)) {
		return -1;
	}
    assert(count < MAX_ITEM_SIZE);
	memcpy(m_pi_data, pi, count * sizeof(product_info));
	m_pi_cnt = count;
	return 0;
}

int  
MyTrader::load_config(mode_config * m_cfg, int count)
{
	m_mode_cfg->push((void *)m_cfg, count);
	return 0;
}

product_info* 
MyTrader::stock_product(char* symbol, char *prd)
{
#if 0
	//Check the legitimacy of stock code
	for (int i = 0; i < 6; i++){
		if (symbol[i] < '0' || symbol[i] > '9'){
			return NULL;
		}
	}
#endif
	if (my_strcmp(prd, "SH") == 0){
		m_stock_pi->xchg_code = '1'; //SSE;
		m_stock_pi->acc_transfer_fee = 0.00002; //�����ѣ����Ͻ�����ȡ
		memcpy(m_stock_pi->name, symbol, MAX_NAME_SIZE);
		memcpy(m_stock_pi->prefix, prd, 32);
		m_stock_pi->rate = 0.0003;     //Ӷ��
		m_stock_pi->stamp_tax = 0.001; //������ӡ��˰
		m_stock_pi->unit = 0.01;       //��С�䶯��λ
		m_stock_pi->multiple = 1;    //���׵Ļ�����
		m_stock_pi->fee_fixed = 1;     //�Ƿ�̶�����
	}
	else if (my_strcmp(prd, "SZ") == 0){
		m_stock_pi->xchg_code = '0'; //SZSE;
		memcpy(m_stock_pi->name, symbol, MAX_NAME_SIZE);
		memcpy(m_stock_pi->prefix, prd, 32);
		m_stock_pi->rate = 0.0003;     
		m_stock_pi->stamp_tax = 0.001; 
		m_stock_pi->unit = 0.01;      
		m_stock_pi->multiple = 1;    
		m_stock_pi->fee_fixed = 1;     
	}
	else if (my_strcmp(prd, "AuG") == 0){
		m_stock_pi->unit = 0.01;
		m_stock_pi->multiple = 1;
	}
	else if (my_strcmp(prd, "AgG") == 0){
		m_stock_pi->unit = 1;
		m_stock_pi->multiple = 1;
	}
	
	return  m_stock_pi;
}

product_info*
MyTrader::find_product(char* symbol)
{
	char product[8] = { 0 };
	char* pos = symbol;
	int idx = 0;

	while (*pos != 0){
		if ((*pos >= '0' && *pos <= '9')|| (*pos == '.')){
			pos++; continue;
		}
		if (*pos == '('){
			product[idx++] = 'G';
			break;
		}
		product[idx++] = *pos;
		pos++;
	}

	product[idx] = 0;
	for (idx = 0; idx < m_pi_cnt; idx++){
		product_info* pi = m_pi_data + idx;
#ifdef WIN32
		if (_stricmp(product, pi->prefix) == 0)
#else
		if (strcasecmp(product, pi->prefix) == 0)
#endif
		    return pi;
	}

	return stock_product(symbol, product);
}

/* this is really a get_or_create method */
match_node *  
MyTrader::add_match_node(char* old_sym, char *new_sym)
{
	match_node *node = m_engine->find(new_sym);
	if (g_unlikely(node == NULL)){
		product_info *product = find_product(old_sym);
		assert(product);
		mode_config *mode = find_mode_config(product);
		node = m_engine->add(new_sym, product, mode, &m_task_cfg);
		assert(node);
	}
	return node;
}

match_node *
MyTrader::find_engine(char* symbol)
{
	match_node *node = NULL;
	if (strlen(symbol) > 8){
        // process symbol exception(symbol from quote may appear this)
		char new_symbol[MAX_NAME_SIZE] = "";
		memcpy(new_symbol, symbol, 6);
		node = add_match_node(symbol, new_symbol);
	}else{
		node = add_match_node(symbol, symbol);
	}
	return node;
}

void  
MyTrader::read_mode_config()
{
	std::ifstream read_fin;
	read_fin.open(MODE_CONF_PATH);
	if (!read_fin.is_open()){
		printf("Open [%s] file fail!\n", MODE_CONF_PATH);
		return;
	}

	SplitVec elems;
	char temp[TOKEN_BUFFER_SIZE];
	while (!read_fin.eof()){
		memset(temp, 0x0, TOKEN_BUFFER_SIZE);
		read_fin.getline(temp, TOKEN_BUFFER_SIZE);
		if (temp[0] == '[' || temp[0] == '#' || temp[0] == '/'){
			continue;
		}

		elems.clear();
		split_string(temp, (char *)",", elems);
		if (elems.size() > 3){
			mode_config *node = (mode_config *)m_mode_cfg->next();
			node->mode = atoi(my_trim(elems[0]).c_str());
			node->exch = my_trim(elems[1]).at(0);
			memcpy(node->product, my_trim(elems[2]).c_str(), 8);
			node->trade_ratio = atof(my_trim(elems[3]).c_str());
			if (node->mode == LINEAR_IMPACT_MATCH){
				node->param_num = int(elems.size() - 4);
				for (int i = 0; i < node->param_num; i++){
					node->params[i] = atof(my_trim(elems[i+4]).c_str());
				}
			}
		}
	}
	read_fin.close();
}

mode_config * 
MyTrader::find_mode_config(product_info* prd)
{
	mode_config *node = NULL, *any_node = NULL;
	for (unsigned int i = 0; i < m_mode_cfg->size(); i++) {
		node = (mode_config *)m_mode_cfg->at(i);
		if (node->mode == m_match_mode && node->exch == prd->xchg_code){
			if (my_strcmp(node->product, prd->prefix) == 0){
				return node;
			}else if (my_strcmp(node->product, "any") == 0){
				any_node = node;
			}
		}
	}
	
	if (any_node != NULL){
		return any_node;
	}else{
		if(m_match_mode != IDEAL_MATCH){
			printf("Can't find mode config\n");
			assert(any_node);
		}
		m_default_cfg.mode = m_match_mode;
		m_default_cfg.trade_ratio = 0.2;
		m_default_cfg.param_num = 3;
		m_default_cfg.params[0] = 1000;
		m_default_cfg.params[1] = 1;
		m_default_cfg.params[2] = 80;
		return &m_default_cfg;
	}
}

bool MyTrader::is_empty_order()
{
	bool is_empty = true;
	match_node *node = NULL;
	for (int i = 0; i < m_engine->size(); i++){
		node = m_engine->find(i);
		is_empty = node->match->is_empty_order();
		if (!is_empty)  return false;
	}
	return true;
}

void 
MyTrader::place_order(place_order_struct* pl_ord, order_return_struct *ord_rtn)
{
    // each contract only contains one match_engine
	match_node *node = find_engine(pl_ord->contr);
    // entrance of the place order operation
	node->match->place_order(pl_ord, ord_rtn);
}

void 
MyTrader::cancel_order(cancel_order_struct* cl_ord, order_return_struct *ord_rtn)
{
	match_node *node = find_engine(cl_ord->contr);
	node->match->cancel_order(cl_ord, ord_rtn);
}

void 
MyTrader::feed_quote(void* quote, int q_type,
          trade_return_struct** trd_rtn_ar, int* rtn_cnt, bool is_first)
{
	char* symbol = (char *)s_extract_sym_func_ar[q_type](quote);
	match_node *node = find_engine(symbol);
	node->match->feed_quote(q_type, quote, is_first);   // feed quote to strategy and match_engine
	node->match->execute_match(trd_rtn_ar, rtn_cnt);    // do buy & sell match job
}

void MyTrader::feed_internal_book(void *book, trade_return_struct** trd_rtn_ar, int* rtn_cnt, bool is_first)
{
	char* symbol = extract_internal_book(book);
	match_node *node = find_engine(symbol);
	node->match->feed_internal_book(book, is_first);
	node->match->execute_match(trd_rtn_ar, rtn_cnt);
}

