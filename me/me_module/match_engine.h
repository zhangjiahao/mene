/*
* The synchronous match engine for simulation
* Copyright(C) by MY Capital Inc. 2007-2999
* Author:Luan.Feng & Chen.xiaowen
* Version:V1.0.2  modify:20161121
*/

#pragma once
#ifndef __MATCH_ENGINE_H__
#define __MATCH_ENGINE_H__

#include <stdio.h>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include "quote_convert.h"
#include "order_types_def.h"
#include "tools.h"
#include "utils.h"
#include "list.h"

#define  TestDebug        11
#define  MAX_TRD_PV_CNT   (16)
#define  MAX_PLACE_SIZE   (256)
#define  SWITCH_DIR(dir)  ((dir-DIR_BUY+1) & 0x1)
#define  BAD_RESULT(dir)  (dir==DIR_BUY ? 1 : -1)

#ifdef   _WIN32
#define  g_likely(x)     x
#define  g_unlikely(x)   x
#else
#define  g_likely(x)     __builtin_expect(!!(x), 1) // predict judgement to be true
#define  g_unlikely(x)   __builtin_expect(!!(x), 0) // predict judgement to be false
#endif

//-----------Global structure & data definition----------
struct price_node {
	int        used;                              // mark the price_node in use
	double     price;                             // the buy/sell price
	long       volume;                            // gathered order place order volume with same price
	long       mkt_vol;                           // total mkt_vol of this price at lastest tick
	long       curr_vol;                          // current quote volume of this price
	long       last_vol;                          // previous quote volume of this price
	long       opp_match_tick;
	char       direct;                            // the direction of price_node group
	list_head  list;                              // next and previous self(construct a list)
	list_head  tick_list;                         // a list contains tick_node
};

struct tick_node {
	int         used;                             // tick_node in use
	long        tick;                             // the tick that order triggered by
	long        volume;                           // total volume of this tick_node
	int         owner;                            // market_volume & mycap_volume
	price_node* parent;                           // a price node of same price
	list_head   list;                             // next and previous self(construct a list)
	list_head   ord_list;                         // a list of orders
	list_head   st_list;                          // a list of strategies
};

struct strat_node {
	int               used;                       // strategy node in use
	int               st_id;                      // the strategy id
	int               vol;                        // strategy volume(buy/sell)
	struct tick_node* parent;                     // tick_node parent
	struct list_head  ord_list;                   // a list of order placed by this strategy
	struct list_head  list;                       // next and previous self(construct a list)
};

struct order_node {
	place_order_struct pl_ord;
	int                data_flag;                 // set with is_filter, now i don't its mean
	long               volume;                    // total place order volume
	double             trade_price;               // latest trade price (assume)
	long               trade_vol;                 // latest trade vol (assume)
	long               entrust_no;                // when we init a order we just need update entrust_no and pl_ord & data_flag
	char               status;                    // like complete or partial or cancel status
	tick_node*         parent;                    // tick node of the order node
	strat_node*        the_st;                    // point the strategy placed this order
	list_head          list;                      // next and previous self(construct a list)
};

enum Special { Filter = 0, Update = 1, Process = 2 };
typedef std::unordered_map<uint64_t, order_node *> PlaceOrderMap;
double get_midprice(double p1, double p2);


class  MatchEngine{

public:
	MatchEngine(char *symbol);
	virtual ~MatchEngine();

	virtual void  clear();
	virtual void  round_finish();
	virtual void  set_config(match_engine_config* cfg);

	virtual void  place_order(place_order_struct* pl_ord, order_return_struct *ord_rtn);
	virtual void  cancel_order(cancel_order_struct* cl_ord, order_return_struct *ord_rtn);
	virtual void  feed_quote(int type, void* quote, bool is_first);
	virtual void  feed_internal_book(void* quote, bool is_first);
	virtual void  execute_match(trade_return_struct** trd_rtn_ar, int* rtn_cnt);

	virtual void  update_order_book();
	virtual void  update_market_book(price_node* pn, char direct, double price);
	virtual void  do_match_on_price(price_node *pn, char direct);
	virtual void  find_trade_volume(double price, char dir);


public:
	bool  is_empty_order();
	order_node* remove_order_from_pool(cancel_order_struct* cl_ord);
	list_head*  get_price_handle(char direct);
	price_node* add_price_node(char direct, double price);
	tick_node*  find_tick_node(price_node* pn, long tick, int owner);
	tick_node*  add_tick_node(price_node* pn, long tick, int owner);
	order_node* add_order_node(tick_node * tn, order_node* node);

	void  update_tick_node(tick_node* tn, int trad_vol);
	void  update_order_node(order_node* on, int trad_vol);
	void  set_order_result(order_node* order);

	void  do_match_on_direct(char direct);
	void  do_match_on_overstep(price_node *pn, char direct);

	void  do_match_can_trade(price_node *pn);
	int   do_match_on_tick(price_node *pn, int exe_vol, bool is_all, int &self_trade);
	int   do_match_on_opposite(price_node *pn, int exe_vol, double exe_price);
	int   do_match_on_order(tick_node *tn, int exe_vol, double exe_price=0);

	void  calc_trade_volume();
	int   find_trade_volume(double price);

	bool  is_skip_order(order_node* node);
	int   is_overstep(char dir, double price);
	void  statis_order(tick_node *tn, int &count, int &total);
	int   calc_overstep_data(char dir, int volume, double &exe_price);
	int   is_can_trade(double market, double self, char dir);
	int   find_market_volume(char dir, double price, bool is_curr);
	int   find_opposite_volume(char dir, double price, double &exe_price);
	void  add_mult_quote_way(int m_type);
	bool  update_first_quote(int type, void *quote);


#ifdef TestDebug
	void show_price_list(bool flag=true);
	void show_tick_list(price_node* pn);
	void show_order_list(tick_node *tn);
#endif

public:
	long         m_entrust_num;                 // global identification of entrust number return by match_engine
	long         m_tick_counter;                // quote tick counter

	bool         m_last_empty;                  // flag identify previous quote not processed
	int          m_last_type;                   // previous mi_type
	void        *m_last_quote;                  // previous piece of quote
	int          m_is_filter;                   // identify previous quote if been filter or processed
	int          m_mi_type;                     // current quote mi_type
	quote_info   m_quote;                       // abstructed information from the quote
	match_engine_config m_cfg;                  // all match engine configuration

#ifdef TestDebug
	std::ofstream  m_write_log;                 // ofstream to write match engine log
#endif

	MyArray     *m_split_data;            		// only used for current tick pv_pair list(now size 16)
	int          m_can_trade_num;               // can trade items in m_split_data count
	pv_pair     *m_can_trade_list[MAX_TRD_PV_CNT]; // can trade items in m_split_data

	MyArray     *m_trade_rtn;            		// store the latest trade rtn info(temporary data)
	MyArray     *m_place_orders;         		// store all user's order
	MyArray     *m_price_data;                  // store all price node
	MyArray     *m_ticks_data;                  // records each tick triggered strategy order signal(time records)

	int          m_place_num;            		// valid placed order number
	order_node  *m_place_array[MAX_PLACE_SIZE]; // point each place order node, new order will append to this array
	list_head    m_buy_prices;           		// buy price list in desc order, it's a price_node list
	list_head    m_sell_prices;          		// sell price list in asc order, it's a price_node list
	PlaceOrderMap   *m_place_hash;              // user's serial_no vs order
};

#endif
