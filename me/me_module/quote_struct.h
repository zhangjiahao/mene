#ifndef QUOTE_STRUCT_H
#define QUOTE_STRUCT_H

#include "quote_define/mi_type.h"
#include "quote_define/c_quote_cfex.h"
#include "quote_define/c_quote_dce_my.h"
#include "quote_define/c_quote_czce_my.h"
#include "quote_define/c_quote_shfe_my.h"
#include "quote_define/c_quote_my_stock.h"
#include "quote_define/c_quote_my_level1.h"
#include "quote_define/c_quote_sge_deep.h"
#include "quote_define/c_quote_esunny_foreign.h"

#pragma pack(push)
#pragma pack(8)

struct quote_struct
{
	enum mi_type type;
	union{
		char data;
		struct cffex_deep_quote cfex;
		struct dce_my_best_deep_quote dce_bst_dp;
		struct shfe_my_quote shfe;
		struct czce_my_deep_quote czce;
	};
};

static int
s_quote_len_ar[MI_SIZE] = 
{
	sizeof(struct cffex_deep_quote),
	sizeof(struct dce_my_best_deep_quote),
	sizeof(struct dce_my_march_pri_quote),
	sizeof(struct dce_my_ord_stat_quote),
	sizeof(struct dce_my_rt_pri_quote),
	sizeof(struct dce_my_ten_entr_quote),
	sizeof(struct shfe_my_quote),
	sizeof(struct czce_my_deep_quote),
	0
};

static const char*
extract_cfex(void* quote)
{
	return ((struct cffex_deep_quote*)quote)->szInstrumentID;
}

static const char *
extract_shge(void *quote)
{
	return ((struct sge_deep_quote *)quote)->InstrumentID;
}

static const char *
extract_esunny_foreign(void *quote)
{
	return ((struct my_esunny_frn_quote *)quote)->Code;
}

static const char*
extract_dce_my_best_deep(void* quote)
{
	return ((struct dce_my_best_deep_quote*)quote)->Contract;
}

static const char*
extract_dce_my_march_pri(void* quote)
{
	return ((struct dce_my_march_pri_quote*)quote)->ContractID;
}

static const char*
extract_dce_my_ord_stat(void* quote)
{
	return ((struct dce_my_ord_stat_quote*)quote)->ContractID;
}

static const char*
extract_dce_my_rt_pri(void* quote)
{
	return ((struct dce_my_rt_pri_quote*)quote)->ContractID;
}

static const char*
extract_dce_my_ten_entr(void* quote)
{
	return ((struct dce_my_ten_entr_quote*)quote)->Contract;
}

static const char*
extract_shfe(void* quote)
{
	return ((struct shfe_my_quote*)quote)->InstrumentID;
}

static const char*
extract_czce(void* quote)
{
	return ((struct czce_my_deep_quote*)quote)->ContractID;
}

static const char*
extract_stock_lv2(void* quote)
{
    return ((struct my_stock_lv2_quote*)quote)->szWindCode;
}

static const char*
extract_my_level1(void* quote)
{
	return ((struct my_level1_quote*)quote)->InstrumentID;
}

typedef const char* (*extract_symbol_func_t)(void*);

static extract_symbol_func_t
s_extract_sym_func_ar[MI_SIZE] = 
{
	extract_cfex,
	extract_dce_my_best_deep,
	extract_dce_my_march_pri,
	extract_dce_my_ord_stat,
	extract_dce_my_rt_pri,
	extract_dce_my_ten_entr,
	extract_shfe,
	extract_czce,
	NULL,
	extract_stock_lv2,
	NULL,
	NULL,
	extract_my_level1,  // MI_MY_LEVEL1 is identical to CFFEX
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	extract_shge,
	NULL,
	NULL,
	extract_esunny_foreign,
	extract_shfe,
};

static const char*
extract_symbol(enum mi_type type, void* q)
{
	return s_extract_sym_func_ar[type](q);
}

/*ADD BY hwg*/
/* functions for extracting quote date */
static const char *
extract_cfex_date(void *quote)
{
        return ((struct cffex_deep_quote *)quote)->szTradingDay;
}

static const char *
extract_dce_bestdeep_date(void *quote)
{
        return ((struct dce_my_best_deep_quote *)quote)->TradeDate;
}

static const char *
extract_shfe_date(void *quote)
{
        return ((struct shfe_my_quote *)quote)->TradingDay;
}

static const char *
extract_czce_date(void *quote)
{
        return ((struct czce_my_deep_quote *)quote)->TimeStamp;
}

typedef const char *(*extract_date_fn_t)(void *);

static extract_date_fn_t extract_date_fn_ar[MI_SIZE] = {
        extract_cfex_date,
        extract_dce_bestdeep_date,	/* Best and deep */
        NULL,						/* March price */
        NULL,						/* Order statistics */
        NULL,						/* Realtime price */
        NULL,						/* Ten entrust */
        extract_shfe_date,
        extract_czce_date
};
static double
extract_cfex_last_price(void *quote)
{
        return (((struct cffex_deep_quote *)quote)->dLastPrice);
}

static double
extract_dce_bestdeep_last_price(void *quote)
{
        return (((struct dce_my_best_deep_quote *)quote)->LastPrice);
}

static double
extract_dce_ord_stat_last_price(void *quote)
{
        return (((struct dce_my_ord_stat_quote *)quote)->WeightedAverageBuyOrderPrice);
}

static double
extract_shfe_last_price(void *quote)
{
        if(((struct shfe_my_quote *)quote)->data_flag != 3) {
                return 0.0;
        }
        return (((struct shfe_my_quote *)quote)->LastPrice);
}

static double
extract_czce_last_price(void *quote)
{
        return (((struct czce_my_deep_quote *)quote)->LastPrice);
}


typedef  double(*extract_last_price_fn_t)(void *);
static extract_last_price_fn_t extract_last_price_fn_ar[MI_SIZE] = {
        extract_cfex_last_price,
        extract_dce_bestdeep_last_price,	/* Best and deep */
        NULL,						/* March price */
        extract_dce_ord_stat_last_price,						/* Order statistics */
        NULL,						/* Realtime price */
        NULL,						/* Ten entrust */
        extract_shfe_last_price,
        extract_czce_last_price
};

/*ADD END*/
#pragma pack(pop)

#endif