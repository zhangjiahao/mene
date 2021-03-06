/*
* The extend match engine define
*/

#pragma once
#ifndef __EXTEND_MATCH_H__
#define __EXTEND_MATCH_H__

#include "match_engine.h"


//------------IdealMatch definition-------------
class IdealMatch: public MatchEngine{
public:
	IdealMatch(char *symbol): MatchEngine(symbol){};

	void execute_match(trade_return_struct** trd_rtn_ar, int* rtn_cnt);
	void execute_ideal_match();
};


//------------LinerMatch definition-------------
#define  MAX_PARAM_SIZE   (100)

struct line_mode_param{
	int       volBuyLst[MAX_PARAM_SIZE];
	double    buyMQPchgLst[MAX_PARAM_SIZE];
	int       volSellLst[MAX_PARAM_SIZE];
	double    sellMQPchgLst[MAX_PARAM_SIZE];
	int       totalImpactVol;
	double    totalImpactCost;
};

class LinearMatch: public MatchEngine{
public:
	LinearMatch(char *symbol): MatchEngine(symbol){
		m_param_index = 0;
	};

	void  clear();
	void  round_finish();
	void  execute_match(trade_return_struct** trd_rtn_ar, int* rtn_cnt);

	void   do_match_on_price(price_node *pn, char direct);
	void   line_mode_calc_trade_volume();
	double get_impact_cost(int cost_vol, char dir);
	void   line_mode_do_opposite(price_node *pn, int exe_vol);

private:
	long             m_param_index;
	line_mode_param  m_param;
};


//------------GaussianMatch definition-------------
class GaussianMatch: public MatchEngine{
public:
	GaussianMatch(char *symbol): MatchEngine(symbol){
		m_price_from = 0.0;
		m_price_to = 0.0;
	};

	void  update_market_book(price_node* pn, char direct, double price);
	void  do_match_on_price(price_node *pn, char direct);

	void  find_trade_volume(double price, char dir);
	int   find_trade_volume_Gaussian(double price, char dir);

private:
	double       m_price_from;
	double       m_price_to;
};


//------------OriginMatch definition-------------
class OriginMatch : public MatchEngine{
public:
	OriginMatch(char *symbol);
	~OriginMatch();

	void  clear();
	void  set_config(match_engine_config* cfg);
	void  place_order(place_order_struct* pl_ord, order_return_struct *ord_rtn);
	void  cancel_order(cancel_order_struct* cl_ord, order_return_struct *ord_rtn);
	void  feed_quote(int type, void* quote, bool is_first);
	void  feed_internal_book(void* quote, bool is_first);
	void  execute_match(trade_return_struct** trd_rtn_ar, int* rtn_cnt);

	bool  ogn_update_order_book(place_order_struct *pl_ord);
	price_node *ogn_find_price_node(double price, char direct);
	price_node *ogn_add_price_node(double price, char direct);
	tick_node  *ogn_find_tick_node(price_node *pn, long tick, int owner);
	tick_node  *ogn_add_tick_node(price_node *pn, long tick, int owner);
	strat_node *ogn_find_strat_node(tick_node *tn, int st_id);
	strat_node *ogn_add_strat_node(tick_node *tn, int st_id);
	order_node *ogn_add_order_node(strat_node *sn, place_order_struct *ord);

	void  ogn_match_on_dir(char direct);
	void  ogn_match_on_price(price_node *pn, int exe_vol, int flag);
	int   ogn_match_on_tick(price_node *pn, tick_node *tn, int exe_vol);
	int   ogn_match_on_strat(tick_node *tn, strat_node *sn, int exe_vol);
	void  ogn_del_tick_node(price_node *pn, tick_node *tn);

	void  update_tradeable_vol(pv_pair *pv_ar, int cnt, double price, char direct, int exe_vol);
	void  update_opposite_vol(char direct, int exe_vol);
	void  update_mkt_tick(price_node *pn, char direct);

	int   calc_tradeable_vol(pv_pair *pv_ar, int cnt, double price, char direct);
	int   calc_opp_trade_vol(double price, char direct);
	void  calc_trade_pv(pv_pair *pv_ar, int *pv_ar_cnt);

	bool  if_czce_is_changed();
	int   if_equal_trade(double mkt_price, double my_price, char dir);
	void  divide_price(double amount, int volume, double &p1, double &p2);
	int   get_vol_on_price(double price, int volume, char direct);
	int   get_volume_prior(place_order_struct *pl_ord);

private:
	int      m_factor;
	int      m_trd_pv_cnt;
	pv_pair  pv_ar_data[MAX_TRD_PV_CNT];
	pv_pair  pv_array_buy[MAX_TRD_PV_CNT];
	pv_pair  pv_array_sell[MAX_TRD_PV_CNT];
	MyArray  *m_strat_data;
};

#endif
