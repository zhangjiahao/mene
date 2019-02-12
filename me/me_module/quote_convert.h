/*
* The implementation of the general data structure
* Copyright(C) by MY Capital Inc. 2007-2999
*/
#pragma once
#ifndef __QUOTE_CONVERT_H__
#define __QUOTE_CONVERT_H__

#include "quote_struct.h"

#define  QUOTE_LVL_CNT  5

enum Direction{
    DIR_BUY  = '0',
    DIR_SELL = '1'
};

enum Owner{
    OWNER_MY  = 0,                                       // order trigger by ours strategy
    OWNER_MKT = 1                                        // order market exist which not contains our orders
};

struct quote_t {
	enum mi_type q_type;                                 // mi_type see mi_type.h
	void *quote;                                         // quote data
};

struct pv_pair {
	double price;                                        // price and volume pair
	int vol;
};

struct mat_quote {
	// buy & sell quote
	struct pv_pair bs_pv[2][QUOTE_LVL_CNT];              // at most 5 level

	char   time[32];                                     // timestamp of current quote
	double last_price;                                   // latest price
	int    last_vol;                                     // latest trade volume

	double high_price;                                   // highest price
	double low_price;                                    // lowest price
	double amount;                                       // turnover(trade capital amount, single side)
	int    total_volume;                                 // number of volume (trade volume, single side)

	/* for czce only */
	int tot_b_vol;                                       // entrust buy volume
	int tot_s_vol;                                       // entrust sell volume
};

struct quote_info {
	int        mult_way;                                  // contains multichannel of quote source
	int        mi_types[QUOTE_LVL_CNT];                   // mi_type of ordered quote
	double     high_limit;                                // high limit at rise limit
	double     low_limit;                                 // low limit at fall limit
	int        data_flag;                                 // when shfe lv3 works

	mat_quote  last_quote;  							  // previous quote
	mat_quote  curr_quote;  							  // current quote
	mat_quote  opposite;    							  // TODO: what this used for
};

//The price comparison function
typedef int(*better_price_func)(double, double);

inline int
better_buy(double left, double right){
	return (left > right);
}

inline int
better_sell(double left, double right){
	return (left < right);
}


//quote conversion function
typedef void (*process_quote_func)(void* origin, quote_info* info);

void process_sge(void* origin, quote_info* info);

void process_cfex(void* origin, quote_info* info);

void process_level1(void* origin, quote_info* info);

void process_dce(void* origin, quote_info* info);

void process_shfe(void* origin, quote_info* info);

void process_czce_deep(void* origin, quote_info* info);

void process_stock_lv2(void* origin, quote_info* info);

void process_esunny(void* origin, quote_info* info);


int  extract_data_flag(void* origin);

char* extract_internal_book(void *origin);

void process_internal_book(void *origin, quote_info* info);

#endif
