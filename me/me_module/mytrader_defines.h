/*
* mytrader_defines.h
*
* Common Function, used in generating SO/DLL
*
* Copyright(C) by MY Capital Inc. 2007-2999
*/
#pragma once
#ifndef __MYTRADER_DEFINES_H__
#define __MYTRADER_DEFINES_H__

#include "quote_define/mi_type.h"
#include "quote_define/c_quote_cfex.h"
#include "quote_define/c_quote_czce_my.h"
#include "quote_define/c_quote_dce_my.h"
#include "quote_define/c_quote_ib.h"
#include "quote_define/c_quote_my.h"
#include "quote_define/c_quote_my_each_datasource.h"
#include "quote_define/c_quote_my_level1.h"
#include "quote_define/c_quote_my_stock.h"
#include "quote_define/c_quote_shfe_my.h"
#include "quote_define/c_quote_sungard.h"
#include "quote_define/c_quote_tap_my.h"

#define MY_SHFE_QUOTE_PRICE_POS_COUNT 30
#define iSIGNAL_LEN 1000
#define iOUT_LEN 100
#define iSTRATEGY_MK1 100
#define iMK101 101
#define MAX_SIGNAL_SIZE 1000
#define MAX_INNER_QUOTE_FORMAT 10

#define MAX_DATA_SIZE   9
#define MAX_SYMBOL_SIZE 64
#define MAX_PATH_SIZE   256
#define MAX_STRATEGY_NAME_SIZE 128

//for ContractHash 
#define HASH_TABLE_SIZE	  (8192)
#define MAX_ITEM_SIZE	  (4096)


enum PRODUCT_RANK{
	UNDEFINED_RANK = 0,
	main_product = 1,
	second_product = 2,
	ALL_RANK = 100
};

/* Combination of all the available quote formats */
union quote_union
{
    cffex_deep_quote cfex;
	dce_my_best_deep_quote bst_deep;
	dce_my_march_pri_quote march_pri;
	dce_my_ord_stat_quote ord_stat;
	dce_my_rt_pri_quote rt_pri;
	dce_my_ten_entr_quote ten_ent;
	shfe_my_quote shfe;
	czce_my_deep_quote czce_deep;
	czce_my_snapshot_quote czce_snapshot;
	my_stock_lv2_quote stock_lv2;
	my_stock_index_quote stock_index;
	my_marketdata market_data;
	my_level1_quote level1;
	sgd_option_quote option;
	sgd_order_queue order_queue;
	sgd_per_entrust entrust;
	sgd_per_bargain bargain;
	my_each_datasource datasource;
	ib_deep_quote deep_quote;
};

//st_order_info.status
enum ORDER_STATUS{
	SIG_STATUS_INIT = -1,			 //new order
	SIG_STATUS_SUCCESS, 	 	     /* ����ȫ���ɽ� */
	SIG_STATUS_ENTRUSTED, 	 	     /* ����ί�гɹ� */
	SIG_STATUS_PARTED, 	 	 	     /* �������ֳɽ� */
	SIG_STATUS_CANCEL, 	 	 	     /* ���������� */
	SIG_STATUS_REJECTED, 	 	     /* �������ܾ� */
	SIG_STATUS_CANCEL_REJECTED,		 /* �������ܾ� */
	UNDEFINED_STATUS
};

/* Doesn't belong to MyTrader */
enum POSITION_FLAG{
	LONG_OPEN = 0,
	LONG_CLOSE = 1,
	SHORT_OPEN = 2,
	SHORT_CLOSE = 3
};

//st_order_info.kaiping
enum OPEN_CLOSE_FLAG{
	SIG_ACTION_OPEN = 1,
	SIG_ACTION_CLOSE = 2,
	UNDEFINED_OPEN_CLOSE_FLAG = 3
};

//st_order_info.direct
enum ORDER_DIRECTION{
	SIG_DIRECTION_BUY = 1,
	SIG_DIRECTION_SELL = 2,
	SIG_DIRECTION_CANCEL = 4,
	UNDEFINED_SIDE = 5
};

enum report_types
{
	new_ord_rpt = '0',
	cancel_rpt  = '1',
	/*
	* market maker, request quote
	*/
	req_quote_rpt = '9',
};

enum side_options
{
	dir_buy            = '0',
	dir_sell           = '1',
	undefined_side     = 'u',
};

/**
��1�����ɲ���״̬�ǲ���Ҫ���ڵģ�
��2��rejected״̬��last_qty��ֵ��
*/
enum EXCH_ORDER_STATUS
{
	// δ��(n:�ȴ�����; s:�����걨)
	pending_new = '0',
	// �Ѿ�����
	new_state = 'a',
	// ���ֳɽ�
	partial_filled = 'p',
	// ȫ���ɽ�
	filled = 'c',
	// �ȴ�����
	pending_cancel = 'f',
	// ���ھܾ�(e:����ί��)
	//rejected = 'q',
	rejected = 'e',
	// �Ѿ�����(b:���ɲ���)
	cancelled = 'd',
	cancel_rejected = 'z', /* Add by Wen, need to make sure it's consist with live trading*/
	UNDEFINED_STATE = 'u'
};

enum position_effect_options
{
	//	����
	open_pos = '0',
	// ƽ��
	close_pos = '1',
	// ƽ���
	rolled = '2',
	// 	ƽ���
	close_yesterday = '3',
	undefined_pos_effect = 'u'
};

enum price_options
{
	/* �޼� */
	limit = '0',
	/* �м� */
	market = '1'
};

enum request_types
{
	place_order  = '0',
	cancel_order = '1',
	req_quote    = '2',
	quote        = '3',
};

enum ord_types
{
	general   = '0',
	fak       = '1',
	fok       = '2',
};

enum investor_type
{
	speculate  = '0',
	hedge      = '1',
	arbitrage  = '2',
};

/*
��ö�ٶ��彻��ͨ����صĴ�����Ϣ
*/
enum channel_errors
{
	// ִ�й���ʧ��
	RESULT_FAIL = -1,
	// 	ִ�й��ܳɹ�
	RESULT_SUCCESS = 0,
	// �޴˹��ܺ�
	UNSUPPORTED_FUNCTION_NO = 1,
	// ����ͨ��δ�����ڻ���
	NO_VALID_CONNECT_AVAILABLE = 2,
	// ���������ָ��
	ERROR_REQUEST = 3,
	// ��ָ�ڻ��ۼƿ��ֳ����������
	CFFEX_EXCEED_LIMIT = 4,
	// ��֧�ֵĹ���
	UNSUPPORTED_FUNCTION = 100,
	// �޴�Ȩ��
	NO_PRIVILEGE = 101,
	// û�б�������Ȩ��
	NO_TRADING_RIGHT = 102,
	// �ý���ϯλδ���ӵ�������
	NO_VALID_TRADER_AVAILABLE = 103,
	// ��ϯλĿǰû�д��ڿ���״̬
	MARKET_NOT_OPEN = 104,
	// ������δ�������󳬹������
	CFFEX_OVER_REQUEST = 105,
	// ������ÿ�뷢�����������������
	CFFEX_OVER_REQUEST_PER_SECOND = 106,
	// ������δȷ��
	SETTLEMENT_INFO_NOT_CONFIRMED = 107,
	// �Ҳ�����Լ
	INSTRUMENT_NOT_FOUND = 200,
	// ��Լ���ܽ���
	INSTRUMENT_NOT_TRADING = 201,
	// �����ֶ�����
	BAD_FIELD = 202,
	// ����ı��������ֶ�
	BAD_ORDER_ACTION_FIELD = 203,
	// �������ظ�����
	DUPLICATE_ORDER_REF = 204,
	// �������ظ�����
	DUPLICATE_ORDER_ACTION_REF = 205,
	// �����Ҳ�����Ӧ����
	ORDER_NOT_FOUND = 206,
	// ��ǰ����״̬��������
	UNSUITABLE_ORDER_STATUS = 207,
	// ֻ��ƽ��
	CLOSE_ONLY = 208,
	// ƽ���������ֲ���
	OVER_CLOSE_POSITION = 209,
	// �ʽ���
	INSUFFICIENT_MONEY = 210,
	// �ֻ����ײ�������
	SHORT_SELL = 211,
	// ƽ���λ����
	OVER_CLOSETODAY_POSITION = 212,
	// ƽ���λ����
	OVER_CLOSEYESTERDAY_POSITION = 213,
	// ί�м۸񳬳��ǵ�������
	PRICE_OVER_LIMIT = 214,
};

enum SecurityType
{
	FUTURE  = 0,
	STOCK   = 1,
	OPTION  = 2,
	FX      = 3,
	ETF     = 4
};

/* MyTrader Enums and Structs */
enum Exchanges
{
	// SHENZHEN STOCK EXCHANGE
	SZSE = '0',
	// SHANGHAI STOCK EXCHANGE
	SSE = '1',
	HKEX = '2', // FIXME make this consistent with trader
	// SHANGHAI FUTURES EXCHANGE
	SHFE = 'A',//XSGE = 'A',
	// CHINA FINANCIAL FUTURES EXCHANGE
	CFFEX = 'G',//CCFX = 'G',
	// DALIAN COMMODITY EXCHANGE
	DCE = 'B', //XDCE = 'B',
	// ZHENGZHOU COMMODITY EXCHANGE
	CZCE = 'C', //XZCE = 'C',
	FUT_EXCH = 'F',
	SGX = 'S', // FIXME make this consistent with trader
	BLANK_EXCH = '\0',
	UNDEFINED_EXCH = 'u'
};

enum  FEED_STATUS{
	DEBUT = '0',		//��������
	REISSUE = '1',		//�����¹�
	ONLINE_SET_PRICE = '2', //2 �������۷���
	ONLINE_BID_PRICE = '3',	//3 �������۷���
	CLOSE = 'A',		//A ���׽�����
	HALT_FOR_TODAY = 'B',			//B ����ͣ��
	END_OF_DAY = 'C',   //C ȫ������
	PAUSE_TRADE = 'D',	    //D ��ͣ����	
	START = 'E',		//����������
	PRE_ENTER = 'F',	//��ǰ����
	HOLIDAY = 'H',		//�ż�
	OPEN_AUCTION = 'I', //���м��Ͼ���
	IN_MARKET_AUCTION = 'J', //���м��Ͼ���
	OPEN_PRE_ORDER_BOOK = 'K',	//���ж�����ƽ��ǰ��
	IN_MARKET_PRE_ORDER_BOOK = 'L', //���ж�����ƽ��ǰ��	
	OPEN_ORDER_BOOK = 'M',	//���ж�����ƽ��
	IN_MARKET_ORDER_BOOK = 'N',	//���ж�����ƽ��
	TRADE = 'O', //�������
	BREAK = 'P', //����
	VOL_BREAK = 'Q', //�������ж�
	BETWEEN_TRADE = 'R', //���׼�
	NO_TRADE_SUPPORT = 'S', //�ǽ��׷���֧��
	FIX_PRICE_AUCTION = 'T', //�̶��۸񼯺Ͼ���
	POST_TRADE = 'U', //�̺���
	END_TRADE = 'V', //��������
	PAUSE = 'W',  //��ͣ
	SUSPEND = 'X', //ͣ��
	ADD = 'Y', //������Ʒ
	DEL = 'Z' //��ɾ���Ĳ�Ʒ
};

struct comm_out_data {
	char     cTicker[8];
	int      iTime, iRecordNo, iV;
	double   rP, rBP1, rSP1, rVal, rOI, rBuyPrice, rSelPrice;
	int      iBV1, iSV1, iHaveOrder, iTS, iCancel, iOpenSize, iCloseSize;
	int      iToTradeSize, iLongPos, iShortPos,  
		     iMaxPos, iTotalOrderSize, iTotalCancelSize;
};

struct	symbol_t
{
	enum Exchanges	exch;
	char ticker[MAX_SYMBOL_SIZE];
	int	 order_lmt_per_second;
	int	 order_cancel_lmt_per_day;
	int	 max_pos;
	int	 ticker_log_id;
	char ticker_log_name[MAX_PATH_SIZE];
};

struct ContractInfo_t
{
	char    contract_code[MAX_SYMBOL_SIZE];
	char    TradeDate[9];
	char    ExpireDate[9];
};

/* Config file used in st_init() */
struct st_config_t
{
	int		strategy_id;                     //strategy id
	char	strategy_name[MAX_SYMBOL_SIZE];  //strategy name;
	int	    log_id;                          // model log file id in Fortran
	char	log_name[MAX_PATH_SIZE];         /* model log file path,not including file name */
	int		intvariable_id;                  /* the file id which is used to record intermediate variables by model. */
	char	intvariable_name[MAX_PATH_SIZE]; /* the file name,including path,which is used to record intermediate variables by model.*/
	int		extvariable_id;                  /* the file id which is used to record external variables by model.*/
	char	extvariable_name[MAX_PATH_SIZE]; /* the file name,including path,which is used to record external variables by model. */
	int		order_lmt_per_second;            /* order limit per second */
	int		order_cancel_lmt_per_day;        /* order cancel limit per day*/
	int		ticker_count;                    /* number of contracts, configed */
	symbol_t   ticker_detail[2000];          /* fixed length array and specific contract number is specified by ticker_count*/
	int		log_flags;
	int		contract_info_count;	         /* Requested from Exchange */
	ContractInfo_t contracts[1000];
};

/* Used for Stock */
struct	symbol_pos_t
{
	char     ticker[MAX_SYMBOL_SIZE];
	int      long_volume;       // Current Position, might include yesterday's pos.
	double   long_price;	    // Future 
	int      short_volume;
	double   short_price;
	int      pos_change;        //1: pos changed; 0: position not change

	int      today_buy_volume;	// Only today's position, no previous position.
	double   today_aver_price_buy;	// Stock
	int      today_sell_volume;
	double   today_aver_price_sell;
	unsigned char  exchg_code;
};

struct position_t
{
	int             count;
	symbol_pos_t    pos[10000];
};

struct acc_volume_t
{
	char           ticker[MAX_SYMBOL_SIZE];
	int            buy_volume;
	double         buy_price;
	int            sell_volume;
	double         sell_price;
	unsigned char  exchg_code;
};

struct	strategy_init_pos_t
{
	char          st_name[MAX_SYMBOL_SIZE];
	position_t    yest_pos;
	position_t    today_pos;
	int           today_acc_count;
	acc_volume_t  today_acc_volume[10000];
};

enum cancel_pattern_t {
	SIG_CANCEL_MODE_SYNC  = 0,
	SIG_CANCEL_MODE_ASYNC = 1,
};

struct signal_t
{
	uint64_t         sig_id;
	/* When cancel order, we use sig_id */
	uint64_t         cancel_sig_id;
	/*
	* 0:synchronously execute canceling order signal
	* 1:asynchronously execute canceling order signal
	*/
	cancel_pattern_t cancel_mode;
	int	             st_id;
	Exchanges	     exch;
	char	         ticker[MAX_SYMBOL_SIZE];
	int              open_volume;
	double           buy_price;
	int              close_volume;
	double           sell_price;
	unsigned short   sig_action;     //buy sell, cancel   ORDER_DIRECTION
	unsigned short   sig_openclose;  // 1-Open, 2-Close.  OPEN_CLOSE_FLAG
	unsigned short   instruction;	 // Not supported for now.
	unsigned short   internal_match;  // Not supported for now.
	char             mm_order_id[21];
};

struct cStruct_timeval
{
	unsigned long tv_sec;
	unsigned long tv_usec;
};

/* Statistic info should be removed */
struct signal_resp_t
{
	cStruct_timeval     exchg_timestamp;           /* �ر�ʱ�� */
	cStruct_timeval     local_timestamp;           /* ����ʱ�� */
	uint64_t            sig_id;                    /* �ź�id */
	char                symbol[MAX_SYMBOL_SIZE];   /* ��Լ�� */
	unsigned short      sig_act;                   /* buy = 1, sell = 2, cancel = 4, quote = 8, �����Ĳ������ */   
	int                 order_volume;              /* ���źŵ���ί���� */
	double              order_price;               /* ���źŵ�ί�м۸� */
	double              exe_price;                 /* ��ǰ�ɽ��۸� */
	int                 exe_volume;                /* ��ǰ�ɽ��� */
	double              avg_price;                 /* �ܵ�ƽ���ɽ��۸񣬽��鲻Ҫʹ�� */
	int                 cum_volume;                /* �ۼƳɽ��������鲻Ҫʹ��  */
	ORDER_STATUS        status;                    /* �ɽ�״̬ */
	int                 ack;                       /* �ύ���������鲻Ҫʹ�� */
	int                 cancelled;                 /* �������������鲻Ҫʹ�� */
	int                 rejected;                  /* �ܾ����������鲻Ҫʹ�� */
	int                 error_no;                  /* ����ţ����鲻Ҫʹ�� */
};

struct	order_request_t
{
	char               ticker[MAX_SYMBOL_SIZE];
	double             price;
	int                volume;
	int                side;
};

struct pending_order_t
{
	int                count;
	order_request_t    pending_vol[10000];
};

struct cStruct_local_quote
{
	long long          time;
	char               contract[MAX_SYMBOL_SIZE];
	int                curr_volume;
	int                b_total_volume;
	int                s_total_volume;
	double             last_price;
	double             b_open_interest;
	double             s_open_interest;
	double             buy_price[30];
	int                buy_vol[30];
	double             sell_price[30];
	int                sel_vol[30];
};

struct	cStruct_quote_data
{
	char      ticker[MAX_SYMBOL_SIZE];
	char      serial_no[21];
	char      exch_tv[9];
};

/* Stock specific definitions */
enum STOCK_INDEX_ENUM {
	STOCK_INDEX_HS300,
	STOCK_INDEX_ZZ500,
	STOCK_INDEX_SH50,
	STOCK_INDEX_A50,

	STOCK_INDEX_COUNT
};

/* Stock pool definitions */
enum STOCK_POOL_ENUM {
	STOCK_POOL_A50,
	STOCK_POOL_SH50,
	STOCK_POOL_HS300,
	STOCK_POOL_ZZ500,
	STOCK_POOL_ZZ800,
	STOCK_POOL_ZZ1000,
	STOCK_POOL_ASSETS_PROFIT_RATE_INCREASE3,
	STOCK_POOL_CASHFLOW_NETINCOME,
	STOCK_POOL_DUPONT_ROA,
	STOCK_POOL_INTDEBT_TO_TOTAL_CAP,
	STOCK_POOL_LONG_DEBT_ODEBT,
	STOCK_POOL_NET_PROFIT_RATE_INCREASE3,
	STOCK_POOL_OCF_TO_DEBT,
	STOCK_POOL_OCF_TO_OR,
	STOCK_POOL_OPER_REVINCREASE3,
	STOCK_POOL_ROE_AVG,
	STOCK_POOL_SHRHLDR_PROFIT_RATE_INCREASE3,
	STOCK_POOL_YOYBPS,
	STOCK_POOL_GROSS_PROFIT_RATE_INCREASE3,
	STOCK_POOL_COUNT
};

/**
 * Turing status code for individual strategy
 */
enum ST_STATUS_ENUM {
	ST_STATUS_NORMAL = 0,
	ST_STATUS_INIT_FAILED,
	ST_STATUS_ORDER_REJECTED,
	ST_STATUS_ORDER_TOO_FREQUENT,
	ST_STATUS_INVALID_QUOTE,
	ST_STATUS_COUNT
};

struct bss_config {
	double       *params;
	int           param_len;
	int           day_night_flag;  /* 0 - DAY; 1 - NIGHT*/
};

/************************************************************************/
/* Turing MY Trader Definitions
/************************************************************************/
#define TURING_DCE_MAX_ORDER_COUNT 64

typedef struct _turing_symbol_pos {
	unsigned char exchg_code;       /* ������������ */
	int           long_volume;      /* ��ֲ�λ */
	double        long_price;       /* ��ֳɽ�ƽ���۸� */
	int           short_volume;     /* �ղֲ�λ */
	double        short_price;      /* �ղֳɽ�ƽ���۸� */
} turing_symbol_pos_t;

typedef int(*turing_dce_st_log)(char *account, char *symbol, char *log, unsigned long len);
typedef int(*turing_shfe_st_log)(char *log, unsigned long len);

typedef struct _turing_st_config {
	char               symbol[MAX_SYMBOL_SIZE];    /* ��Լ���� */
	char               ev_name[MAX_PATH_SIZE];     /* �ⲿ������¼�ļ�������Ŀ¼�� */
	int                max_pos;                    /* ���ֲ�(��Ӫ���ã���ʾ����ÿ���µ�������������뿪��������޹�) */

	turing_symbol_pos_t yesterday_pos;      /* ���ճֲ� */
	turing_symbol_pos_t today_pos;          /* ����ֲ� */

	/* ������־���� */
	union {
		turing_dce_st_log  dce_st_log;       // 1 GB log space per account
		turing_shfe_st_log shfe_st_log;
	};

	char                 account[16];         /* �˻�ID */
	int                  st_sub_id;           /* Strategy sub ID */
	char                 strat_name[MAX_STRATEGY_NAME_SIZE]; /* Used in Turing strategy setup */

	int                  single_side_max_pos;  /* ��󵥱߲�λ���ޣ����Զ�ο�ƽ�֣�һ�������λ�������� max_pos���� */
	int                  max_accum_open_vol;   /* ���˫���ۼƿ������ޣ��������涨��һ���ﵽ�������޷��ٿ��� */
	int                  expiration_date;      /* ��Լ�������ڣ����磺20161125  */
	/* For future use */
	int                  single_side_max_pos_limit; /* ��󵥱߲�λ���ߣ���λ���Բ�������������� */
	int                  future_int_param2;
	int                  future_int_param3;
	uint64_t             future_uint64_param1;
	double               future_double_param1;
	double               future_double_param2;
	double               future_double_param3;
	char                 future_char64_param1[64];
	char                 future_char64_param2[64];
	char                 future_char64_param3[64];
}  turing_st_config_t;

/* Not passed by Turing, just for SDP convenience */
typedef struct _turing_extra_info {
	int                  single_side_max_pos;
	int                  max_accum_open_vol;
	int                  single_side_max_pos_limit;
} turing_extra_info;

/* trade���͸����Իر������ݽṹ */
typedef struct _turing_signal_resp {
	char exchg_timestamp[13];         /* �ر�ʱ�� */
	char local_timestamp[13];         /* ����ʱ�� */

	uint64_t       local_id;          /* ����ί�к� */
	char           symbol[MAX_SYMBOL_SIZE];        /* ��Լ�� */
	unsigned short direction;         /* DCE ��-'0'/��-'1' */  /* SHFE ��-0/��-1 */
	unsigned short openclose;         /* DCE ��ƽ��ʶ��'0'��ʾ����'1'��ʾƽ */  /* SHFE ��-0/ƽ-1 */

	double exec_price;                /* ��ǰ�ɽ��۸� */
	int    exec_volume;               /* ��ǰ�ɽ��� */
	ORDER_STATUS   status;            /* �ɽ�״̬ */
	int    error_no;                  /* �����  - huanghong */
	char   error_info[512];           /* ����ԭ�� */
} turing_signal_rsp_t;

typedef enum _turing_my_info {
	MY_INFO_QUOTE,                 /* ���� */
	MY_INFO_BAR_RTN_ORDER,         /* ��̨�����µ��ɹ��ر� */
	MY_INFO_BAR_RTN_ORDER_FAIL,    /* ��̨�����µ�ʧ�ܻر� */
	MY_INFO_BAR_RTN_CANCEL,        /* ��̨���س����ɹ��ر� */
	MY_INFO_BAR_RTN_CANCEL_FAIL,   /* ��̨���س���ʧ�ܻر� */
	MY_INFO_XCHG_RTN_ORDER,        /* �����������µ��ɹ��ر� */
	MY_INFO_XCHG_RTN_CANCEL,       /* ���������س����ɹ��ر� */
	MY_INFO_DEAL_RTN,              /* �ɽ��ر� */
	MY_INFO_OPEN_VOL_CHK,          /* ��������鲻���ر� */
} turing_my_info_t;

/* trade��������/�ر������Ե����ݽṹ */
struct turing_feed_st_info {
	turing_my_info_t type;    /* ���͸����Ե���Ϣ���� */

	/* trade�����������Ϊ���������/�Գɽ���麯������ */
	void *__strategy;

	/* trade�����������Ϊ��ȡDMA�ڴ��׵�ַ�������� */
	void *__session;

	/* ����/�ر� */
	void *info;

	union {
		/* SHFE ����+�Ϲ��� */
		int(*send_order)(void *__strategy, /* place the received __strategy */
			char *symbol,
			int volume,
			double price,
			unsigned int direction,
			unsigned int open_close_flag,
			unsigned int st_sub_id,  /* strategy sub id */
			uint64_t *ord_id,
			unsigned int flag_close_yes_pos); /* ord_id is returned by trader */

		/* DCE FPGA ����Լ�Ϲ��� */
		int(*compliant)(void *__strategy, void *order);
	};

	union {
		/* SHFE ����+�Ϲ��� */
		int(*cancel_order)(void *__strategy,
			char *symbol,
			unsigned int st_sub_id,  /* strategy sub id */
			uint64_t ord_id);

		/* DCE FPGA ��ȡ����/�µ�DMA�ڴ����׵�ַ */
		/* sig_idx��ʾ���������źŵ���ţ�ȡֵ0-63 */
		/* ��follow fpga������ڴ�ṹ�Լ�fpga������ʽ����Ҫ���Դ���sig_idx�����������ʾ�������ɵĵڼ����ź� */
		void *(*get_dma_zone)(void *__session, unsigned char sig_idx);
	};

	/*
	 * Strategy status defined in ST_STATUS_ENUM.
	 * Each sub-strategy owns 4 bits starting from the least significant bit
	 * 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	 * |--|     ...                                              |--| |--||--|
	 *  16      ... strategy                                      3    2   1
	 */
	uint64_t status;
};

enum TURING_ACTION_TYPE {
	TURING_ACTION_TYPE_PLACE_ORDER = '0',
	TURING_ACTION_TYPE_CANCEL_ORDER = '1'
};

enum TURING_STRATEGY_TYPE {
	TURING_STRATEGY_TYPE_UNDEFINED = 0,
	TURING_STRATEGY_TYPE_SPECULATE = 0x30,
	TURING_STRATEGY_TYPE_HEDGE = 0x31,
	TURING_STRATEGY_TYPE_ARBITRAGE = 0x32
};

enum TURING_ORDER_INSTRUCTION {
	TURING_ORDER_INSTRUCTION_UNDEFINED = 0,
	TURING_ORDER_INSTRUCTION_NORMAL = 0x30,
	TURING_ORDER_INSTRUCTION_FAK = 0x31,
	TURING_ORDER_INSTRUCTION_FOK = 0x32
};

enum TURING_ORDER_TYPE {
	TURING_ORDER_TYPE_UNDEFINED = 0,
	TURING_ORDER_TYPE_LIMIT_ORDER = 0x30,
	TURING_ORDER_TYPE_MARKET_ORDER = 0x31,
	TURING_ORDER_TYPE_BEST_PRICE = 0x32,
	TURING_ORDER_TYPE_FIVE_LEVEL = 0x33
};

/* �������ɵ��µ�/�������ݽṹ */
struct fpga_common_head {
	unsigned char __toe_id;
	unsigned char __connect_id;    // toe session id
	unsigned char xchg_code;       // ����������
	unsigned char direction;       // ����  '0' buy  / '1' sell
	unsigned char open_close;      // ��ƽ  '0' open / '1' close
	unsigned char __st_id;         // ����id
	/* defined in TURING_STRATEGY_TYPE */
	unsigned char speculator;      // Ŀǰֻ��0x30Ͷ����0x31�ױ���0x32������ʱδ��
	/* defined in TURING_ACTION_TYPE */
	unsigned char action;          // '0' �µ��� '1' ����
};

/* �������ɵĳ������ݽṹ */
struct turing_st_cancel_order {
	struct fpga_common_head head;

	uint64_t      local_id;        // ����ί�к�
	unsigned char __li_pad[12];

	uint64_t      orig_local_id;   // ����������ί�к�
	unsigned char __oli_pad[12];

	unsigned char account[16];     // �˻���
	unsigned char symbol[MAX_SYMBOL_SIZE];      // ��Լ��

	uint64_t      spd_id;          // ��̨ί�к�(-1)
	unsigned char __si_pad[12];
};

/* Not Used, ֻ�ڴ�����FPGA �汾ͼ��ʹ�á��������ɵ��µ����ݽṹ */
struct turing_st_place_order {
	struct fpga_common_head head;

	uint64_t      volume;          // �µ�����
	unsigned char __vol_pad[12];

	double        price;           // �µ��۸�
	unsigned char __p_pad[12];

	uint64_t      local_id;        // ����ί�к�
	unsigned char __li_pad[12];

	/* defined in TURING_ORDER_INSTRUCTION */
	unsigned char order_type;      // �������ͣ�0x30��ͨ | 0x31 FAK | 0x32 FOK
	/* defined in TURING_ORDER_TYPE */
	unsigned char order_kind;      // �������࣬0x30�޼� | 0x31�м� | 0x32���ż� | 0x33�嵵��
	unsigned char __resv2;
	unsigned char __resv3;
	unsigned char account[16];     // �˻���
	unsigned char symbol[MAX_SYMBOL_SIZE];      // ��Լ��
};

/************************************************************************/
/* End of Turing MY Trader Definitions
/************************************************************************/

/* Trader environment - live or simulation */
enum TRADER_ENV {
	TRADER_ENV_LIVE_TRADING,
	TRADER_ENV_LOCAL_SIMULATION,
	TRADER_ENV_BSS_SIMULATION
};

/* Trader type - Normal, Turing, etc. */
enum TRADER_TYPE {
	TRADER_TYPE_NORMAL,
	TRADER_TYPE_SOFTWARE_TURING,
	TRADER_TYPE_FPGA_TURING_DCE
};

/* Internal structure to track current session. */
/* Each quote/response updates the session. */
struct Internal_Session {
	TRADER_TYPE       trader_type;
	Exchanges         exch;                    /* Exchange info for Turing MY Trader */
	char              turing_account[16];      /* Turing DCE */

	int               *sig_count;              /* Normal + Turing */
	signal_t          *sig_out;                /* Normal */
	union {
		turing_dce_st_log   turing_dce_log;    /* Turing */
		turing_shfe_st_log  turing_shfe_log;
	};
	turing_feed_st_info     *turing_info;      /* Turing */
};


#endif

