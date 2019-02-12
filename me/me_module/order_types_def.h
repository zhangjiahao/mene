#ifndef ORDER_TYPES_DEF_H
#define ORDER_TYPES_DEF_H

#include <stdint.h>

#pragma pack(push)
#pragma pack(8)

#define MAX_NAME_SIZE  64
#define MAX_PATH_SIZE  256

enum MatchEngineMode{
	NORMAL_MATCH = 1,
	IDEAL_MATCH = 2,
	LINEAR_IMPACT_MATCH = 3,
	DOUBLE_GAUSSIAN_MATCH = 4,
	SPLIT_TWO_PRICE_MATCH = 5
};

enum OrderStatus{
    STAT_REJECT  = 'e',                     // order invalid identification
    STAT_NEW     = 'a',                     // new a order then we return order_rsp
    STAT_PARTIAL = 'p',                     // order partial traded
    STAT_FULL    = 'c',           			// order totally finished
    STAT_CANCEL  = 'd',                     // order terminated by cancel signal already
	STAT_CANCEL_REJECT = 'z'                // cancel order signal invalid(will not as a status of hang order)
};


// place order struct
struct place_order_struct
{
	uint64_t serial_no;                     // A global id that can identify the strategy
	char 	contr[MAX_NAME_SIZE];           // IF1501, rb1501
	char 	direction;			            // '0' : BUY, '1' : SELL
	char	open_close;                     // '0' : open, '1' : close
	double 	limit_price;
	long 	volume;

	char	order_kind;     	            // '0' limit price, '1' market price, '2' best price, 0x33 5-level price
	char	order_type;     	            // '0' normal, '1' fak(fill and kill), '2' fok(fill or kill)
	char	speculator;     	            // invest type, '0' speculate, '1' hedge, '2' arbitrage
	char	xchg_code;      	            // exchange code, A-shfe, B-dce, C-czce, G-cffex for future
	int     max_pos;        	            // no of use now
};

// cancel order struct
struct cancel_order_struct
{
	uint64_t serial_no;
	uint64_t org_serial_no;

	char 	contr[MAX_NAME_SIZE];
	char 	direction;
	char	open_close;
	char	speculator;                     // invest type
	char	xchg_code;
};

// order return struct
struct order_return_struct
{
	uint64_t serial_no;
	char    contr[MAX_NAME_SIZE];
	char    direction;
	char	open_close;

	long    entrust_no;                     // entrust number generate by exchange
	double  limit_price;
	long    volume;
	long	volume_remain;                  // volume waiting match

	char	xchg_code;
	char    entrust_status;                 // same type with node->status
};

// trade return struct
struct trade_return_struct
{
	uint64_t serial_no;
	char	contr[MAX_NAME_SIZE];
	char 	direction;
	char	open_close;

	long    entrust_no;
	double  business_price;                 // transaction price
	int     business_volume;                // transaction volumn

	char	xchg_code;
	char    entrust_status;
};

/* information of a product */
struct product_info
{
	char    name[MAX_NAME_SIZE]; 	        // IF, dla, dli, shag, shcu, zzsr
	char    prefix[32];			 	        // IF, a, i, ag, cu, SR

	int     fee_fixed;				        // 0 calculate by capital, 1 calculate by volume
	double  rate;					        // 0.00002625 for IF, fee_rate
	double  unit;					        // minimal price change, 0.2 for IF, 1/kg for au
	int     multiple;				        // 300 for IF, 15 kg/lot for ag
	char    xchg_code;                      // exchange code like 'A','B','C','G'

	double  acc_transfer_fee; 		        // account transfer fees(change the brokerage)
	double  stamp_tax;        		        // stamp tax
};

struct mode_config{
	int      mode;                          // mode of match_engine
	char     exch;                          // exchange
	char     product[8];                    // product name

	int      param_num;                     // parameter numbers
	double   trade_ratio;                   // trade ration (next trade success ratio)
	double   params[16];                    // a list of parameters
};

typedef struct {
	int  task_id;                           // task identification
	int  task_type; 				        // backtesting, prototype, post-trade
	int  task_date;                         // exchange date, format like: 20170412
	char user[MAX_NAME_SIZE];               // the user name
	char user_home_path[MAX_PATH_SIZE];     // user name home path, may used to store logs
} task_conf_t;

struct match_engine_config {
	task_conf_t    task;
	product_info   pi;
	mode_config    mode_cfg;

	char    contract[MAX_NAME_SIZE];        // contract name
	char    bin_res_log[MAX_NAME_SIZE];     // original result log position
	int     detailed_res; 			        // detailed result or not
	int     day_night; 				        // day --> 0 , night--->1
};

#pragma pack(pop)

#endif
