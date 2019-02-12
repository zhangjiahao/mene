
#include <string.h>
#include "quote_convert.h"
#include "utils.h"

const double g_stock_scale = 10000.0;
process_quote_func quote_process[MI_SIZE] = {
	process_cfex, 			// QUOTE_CFEX_DEEP
	process_dce,			// QUOTE_DCE_BEST_DEEP
	NULL,					// QUOTE_MATCH_PRICE_QTY
	NULL,					// QUOTE_DCE_ORDER_STATISTIC
	NULL,					// QUOTE_DCE_REALTIME_PRICE
	NULL,					// QUOTE_DCE_TEN_ENTRUST
	process_shfe,			// QUOTE_SHFE_MY
	process_czce_deep,      // QUOTE_CZCE_DEEP 
	NULL,
	process_stock_lv2,
	NULL,
	NULL,
	process_level1,         //==12, New add level1 quote
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	process_sge,
	NULL,
	NULL,
	process_esunny,
	process_shfe,  //new add shfe quote
};

struct tmp_node{
	double b_price; int b_vol;
	double s_price; int s_vol;
};

void process_sge(void* origin, quote_info* info)
{
	struct sge_deep_quote* quote;
	quote = (struct sge_deep_quote*)origin;

	info->high_limit = quote->UpperLimitPrice;
	info->low_limit = quote->LowerLimitPrice;
	memcpy(info->curr_quote.time, quote->UpdateTime, 9);

	info->curr_quote.amount = quote->Turnover / 2;
	info->curr_quote.total_volume = quote->Volume / 2;
	info->curr_quote.high_price = quote->HighestPrice;
	info->curr_quote.low_price = quote->LowestPrice;
	info->curr_quote.last_price = quote->LastPrice;
	info->curr_quote.last_vol = MIN(1, quote->Volume);

	tmp_node* pos = (tmp_node *)&quote->BidPrice1;
	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = pos->b_price;
		b_pv_ar[i].vol = pos->b_vol;
		s_pv_ar[i].price = pos->s_price;
		s_pv_ar[i].vol = pos->s_vol;
		pos += 1;
	}
}

void process_esunny(void* origin, quote_info* info)
{
	struct my_esunny_frn_quote* quote;
	quote = (struct my_esunny_frn_quote*)origin;

	info->high_limit = (double)quote->LimitUp;
	info->low_limit = (double)quote->LimitDown;
	sprintf(info->curr_quote.time, "%d", (int)(quote->updatetime) * 1000);

	info->curr_quote.amount = (double)quote->CJJE;
	info->curr_quote.total_volume = (int)quote->Volume;
	info->curr_quote.high_price = (double)quote->High;
	info->curr_quote.low_price = (double)quote->Low;
	info->curr_quote.last_price = (double)quote->New;
	info->curr_quote.last_vol = (int)quote->Lastvol;

	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = (double)quote->Bid[i];
		b_pv_ar[i].vol = (int)quote->BidVol[i];
		s_pv_ar[i].price = (double)quote->Ask[i];
		s_pv_ar[i].vol = (int)quote->AskVol[i];
	}
}

void process_cfex(void* origin, quote_info* info)
{
	struct cffex_deep_quote* quote;
	quote = (struct cffex_deep_quote*)origin;

	info->high_limit = quote->dUpperLimitPrice;
	info->low_limit = quote->dLowerLimitPrice;
	memcpy(info->curr_quote.time, quote->szUpdateTime, 9);

	info->curr_quote.amount = quote->dTurnover;
	info->curr_quote.total_volume = quote->nVolume;
	info->curr_quote.high_price = quote->dHighestPrice;
	info->curr_quote.low_price = quote->dLowestPrice;
	info->curr_quote.last_price = quote->dLastPrice;
	info->curr_quote.last_vol = MIN(1, quote->nVolume);

	tmp_node* pos = (tmp_node *)&quote->dBidPrice1;
	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = pos->b_price;
		b_pv_ar[i].vol = pos->b_vol;
		s_pv_ar[i].price = pos->s_price;
		s_pv_ar[i].vol = pos->s_vol;
		pos += 1;
	}
}


void process_level1(void* origin, quote_info* info)
{
	struct my_level1_quote* quote;
	quote = (struct my_level1_quote*)origin;

	info->high_limit = quote->UpperLimitPrice;
	info->low_limit = quote->LowerLimitPrice;
	memcpy(info->curr_quote.time, quote->UpdateTime, 9);

	char exch = get_exch_by_symbol(quote->InstrumentID);
	if (exch == 'G'){
		info->curr_quote.amount = quote->Turnover;
		info->curr_quote.total_volume = quote->Volume;
	}else{
		info->curr_quote.amount = quote->Turnover / 2;
		info->curr_quote.total_volume = quote->Volume / 2;
	}

	info->curr_quote.high_price = quote->HighestPrice;
	info->curr_quote.low_price = quote->LowestPrice;
	info->curr_quote.last_price = quote->LastPrice;
	info->curr_quote.last_vol = MIN(1, quote->Volume);

	// it's a special process for quote lv1
	memset(info->curr_quote.bs_pv, 0, sizeof(struct pv_pair) * 10);
	info->curr_quote.bs_pv[0][0].price = quote->BidPrice1;
	info->curr_quote.bs_pv[0][0].vol = quote->BidVolume1;
	info->curr_quote.bs_pv[1][0].price = quote->AskPrice1;
	info->curr_quote.bs_pv[1][0].vol = quote->AskVolume1;
}


void process_dce(void* origin, quote_info* info)
{
	struct tmp_node2{
		float price;
		int vol;
		int imply;
	};

	struct dce_my_best_deep_quote* quote;
	quote = (struct dce_my_best_deep_quote*)origin;

	info->high_limit = quote->RiseLimit;
	info->low_limit = quote->FallLimit;

	memcpy(info->curr_quote.time, quote->GenTime, 13);
	info->curr_quote.amount = quote->Turnover / 2;
	info->curr_quote.total_volume = quote->MatchTotQty / 2;

	info->curr_quote.high_price = quote->HighPrice;
	info->curr_quote.low_price = quote->LowPrice;
	info->curr_quote.last_price = quote->LastPrice;
	info->curr_quote.last_vol = quote->LastMatchQty;

	tmp_node2* b_pos = (tmp_node2 *)&quote->BuyPriceOne;
	tmp_node2* s_pos = (tmp_node2 *)&quote->SellPriceOne;
	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = b_pos->price;
		b_pv_ar[i].vol = b_pos->vol;
		s_pv_ar[i].price = s_pos->price;
		s_pv_ar[i].vol = s_pos->vol;
		b_pos += 1; s_pos += 1;
	}
}

void process_shfe(void* origin, quote_info* info)
{
	struct shfe_my_quote* quote;
	quote = (struct shfe_my_quote*)origin;

	info->high_limit = quote->UpperLimitPrice;
	info->low_limit = quote->LowerLimitPrice;
	info->data_flag = quote->data_flag;

	memcpy(info->curr_quote.time, quote->UpdateTime, 9);
	info->curr_quote.amount = quote->Turnover / 2;
	info->curr_quote.total_volume = quote->Volume / 2;

	info->curr_quote.high_price = quote->HighestPrice;
	info->curr_quote.low_price = quote->LowestPrice;
	info->curr_quote.last_price = quote->LastPrice;
	info->curr_quote.last_vol = 1;

	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = quote->buy_price[i];
		b_pv_ar[i].vol = quote->buy_volume[i];
		s_pv_ar[i].price = quote->sell_price[i];
		s_pv_ar[i].vol = quote->sell_volume[i];
	}
}

void process_czce_deep(void* origin, quote_info* info)
{
	struct czce_my_deep_quote* quote;
	quote = (struct czce_my_deep_quote*)origin;

	info->high_limit = quote->HighLimit;
	info->low_limit = quote->LowLimit;
	memcpy(info->curr_quote.time, quote->TimeStamp, 24);

	/* trade capital amount, not appear in quote */
	info->curr_quote.amount = 0;
	info->curr_quote.total_volume = quote->TotalVolume / 2;
	info->curr_quote.high_price = quote->HighPrice;
	info->curr_quote.low_price = quote->LowPrice;

	info->curr_quote.last_price = quote->LastPrice;
	info->curr_quote.last_vol = 1;
	/* only work for czce */
	info->curr_quote.tot_b_vol = quote->TotalBidLot;
	info->curr_quote.tot_s_vol = quote->TotalAskLot;

	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = quote->BidPrice[i];
		b_pv_ar[i].vol = quote->BidLot[i];
		s_pv_ar[i].price = quote->AskPrice[i];
		s_pv_ar[i].vol = quote->AskLot[i];
	}
}

void process_stock_lv2(void* origin, quote_info* info)
{
	struct my_stock_lv2_quote* quote;
	quote = (struct my_stock_lv2_quote*)origin;
	info->high_limit = quote->nHighLimited / g_stock_scale;
	info->low_limit = quote->nLowLimited / g_stock_scale;

	info->curr_quote.amount = (double)quote->iTurnover;
	info->curr_quote.total_volume = (int)quote->iVolume;
	info->curr_quote.high_price = quote->nHigh / g_stock_scale;
	info->curr_quote.low_price = quote->nLow / g_stock_scale;

	info->curr_quote.last_price = quote->nMatch / g_stock_scale;
	info->curr_quote.last_vol = 1;
	info->curr_quote.tot_b_vol = (int)quote->nTotalBidVol;
	info->curr_quote.tot_s_vol = (int)quote->nTotalAskVol;

	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = quote->nBidPrice[i] / g_stock_scale;
		b_pv_ar[i].vol = quote->nBidVol[i];
		s_pv_ar[i].price = quote->nAskPrice[i] / g_stock_scale;
		s_pv_ar[i].vol = quote->nAskVol[i];
	}
}

struct InternalBook {
	int    contract_id;	    /* Contract Id */
	char   symbol[64];      /* Changed from char cTicker[8] */
	char   Wind_equity_code[32];
	int    exchange;
	char   char_time[10];
	int    int_time;        /* 090059000, HourMintueSecondMilliSeconds 1125*/
	int    feed_status;
	double pre_settle_px;
	double last_px;
	int    last_trade_size;
	double open_interest;
	unsigned long long total_vol;
	double total_notional;   /* Trade Notional, Turnover */
	double upper_limit_px;
	double lower_limit_px;
	double open_px;
	double high_px;
	double low_px;
	double pre_close_px;
	double close_px;	/* Today's close price */
	double pre_open_interest;
	double avg_px;
	double settle_px;
	double bp1;         /* Best Bid */
	double ap1;         /* Best Ask */
	int    bv1;
	int	   av1;
	double bp_array[30];
	double ap_array[30];
	int    bv_array[30];
	int    av_array[30];
	int    interval_trade_vol;	/* TradeVolume between each Snapshot Updates */
	double interval_notional;   /* Trade Notional between Snapshots */
	double interval_avg_px;  	/* Avg Price between Snapshot updates */
	double interval_oi_change;	/* Open Interest Change between Snapshot updates */

	int    shfe_data_flag;			/* SHFE 1-Valid Market Data, 2-Valid Entrust, 3-Both Market/Entrust are valid*/
	int    implied_bid_size[30];	/*DCE implied Bid Ask Size */
	int    implied_ask_size[30];
	int    total_buy_ordsize;     /* Total Open Buy Order Size */
	int    total_sell_ordsize;    /* Total Open Sell Order Size */
	double weighted_buy_px;       /* Weighted Buy Order Price */
	double weighted_sell_px;      /* Weighted Sell Order Price */
	double life_high;             /* LifeTime High */
	double life_low;              /* LifeTime Low */

	unsigned int num_trades;    /* Stock Lv2 Number of trades */
	long long    total_bid_vol;
	long long    total_ask_vol;
	mi_type      book_type; /* type of the data source - mainly used to identify level 1 quote */
};

char* extract_internal_book(void *origin)
{
	InternalBook *quote = (InternalBook *)origin;
	return quote->symbol;
}

void process_internal_book(void *origin, quote_info* info)
{
	InternalBook *quote = (InternalBook *)origin;
	info->high_limit = quote->upper_limit_px;
	info->low_limit = quote->lower_limit_px;
	
	if (quote->exchange == 'G'){
		info->curr_quote.amount = quote->total_notional;
		info->curr_quote.total_volume = (int)quote->total_vol;
	}else{
		// bilateral side count into the value
		info->curr_quote.amount = quote->total_notional / 2;
		info->curr_quote.total_volume = (int)quote->total_vol / 2;
	}
	info->curr_quote.high_price = quote->high_px;
	info->curr_quote.low_price = quote->low_px;

	// latest average trade price (buy vs sell)
	info->curr_quote.last_price = quote->last_px;
	info->curr_quote.last_vol = quote->last_trade_size;
	// bid volume
	info->curr_quote.tot_b_vol = (int)quote->total_bid_vol;
	// ask volume
	info->curr_quote.tot_s_vol = (int)quote->total_ask_vol;
	// get the reference
	pv_pair* b_pv_ar = info->curr_quote.bs_pv[0];
	pv_pair* s_pv_ar = info->curr_quote.bs_pv[1];
	// level 5
	for (int i = 0; i < QUOTE_LVL_CNT; i++){
		b_pv_ar[i].price = quote->bp_array[i];
		b_pv_ar[i].vol = quote->bv_array[i];
		s_pv_ar[i].price = quote->ap_array[i];
		s_pv_ar[i].vol = quote->av_array[i];
	}
}


int  extract_data_flag(void* origin)
{
	struct shfe_my_quote* quote = (shfe_my_quote*)origin;
	return quote->data_flag;
}
