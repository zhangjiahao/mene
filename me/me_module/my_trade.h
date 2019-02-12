/*
* The match engine external caller
* Copyright(C) by MY Capital Inc. 2007-2999
*/

#pragma once
#ifndef __MY_TRADE_H__
#define __MY_TRADE_H__

#include  "order_types_def.h"

struct match_node;
struct hash_bucket;
struct rss_recorder;
class  MatchEngine;
class  MyArray;

class EngineHash
{
public:
	EngineHash();
	~EngineHash();
	void clear();
	void round_finish();

	match_node *add(char* symbol, product_info *product, 
		            mode_config *mode, task_conf_t *task);
	match_node *find(char* symbol);
	match_node *find(int index);

	inline int size(){
		return m_use_count;
	}

private:
	match_node *hash_find(char* symbol);
	match_node *traverse_find(char* symbol);
	unsigned long hash_value(char *symbol);

private:
	int               m_use_count;  	// this is match_engine size
	hash_bucket      *m_bucket;
	match_node       *m_match_head; 	// match_engine array
};

//Provide calculation PNL for BSS, abandoned
class Recorder{
public:
	Recorder(match_engine_config *cfg);
	~Recorder();

private:
	rss_recorder *m_rss_rec;
};

class MyTrader{
public:
	MyTrader(int match_mode=NORMAL_MATCH);
	~MyTrader();
	void clear();
	void round_finish();
	void set_task_conf(task_conf_t *task);

	bool is_empty_order();
	void place_order(place_order_struct* pl_ord, order_return_struct *ord_rtn);
	void cancel_order(cancel_order_struct* cl_ord, order_return_struct *ord_rtn);

	void feed_quote(void* quote, int q_type, trade_return_struct** trd_rtn_ar, int* rtn_cnt, bool is_first = false);
	void feed_internal_book(void *book, trade_return_struct** trd_rtn_ar, int* rtn_cnt, bool is_first=false);

	int  load_product(product_info* pi, int count);
	int  load_config(mode_config * m_cfg, int count);

private:
	product_info* stock_product(char* symbol, char *prd);
	product_info* find_product(char* symbol);
	match_node*   find_engine(char* symbol);
	match_node*   add_match_node(char* old_sym, char *new_sym);

	void  read_mode_config();
	mode_config * find_mode_config(product_info* prd);

private:
	int               m_pi_cnt;
	int               m_match_mode;
	task_conf_t       m_task_cfg;
	mode_config       m_default_cfg;
	MyArray          *m_mode_cfg;

	product_info     *m_pi_data;  		// future product information
	product_info     *m_stock_pi;       // stock product information
	EngineHash       *m_engine;         // hash of engines
};

#endif
