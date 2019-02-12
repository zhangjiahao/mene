#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "quote_struct.h"
#include "order_types_def.h"
#include "quote_convert.h"
#include "my_trade.h"

using namespace std;
#define DCE_BEST_TYPE  1

#define QUOTE_SIZE  10
static dce_my_best_deep_quote g_dec_quote[QUOTE_SIZE] = {
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1566018, 2909, 605536, 17616460000, 3076, 2784, 2922, 2891, 0, 0, 2908, 204, 40, 2907, 213, 0, 2906, 214, 3, 2905, 118, 0, 2904, 245, 0, 2910, 164, 0, 2911, 184, 10, 2912, 159, 0, 2913, 217, 1, 2914, 467, 0, "90024768", 60, 21120, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1566018, 2908, 605546, 17616750000, 3076, 2784, 2922, 2891, 0, 0, 2908, 245, 87, 2907, 214, 0, 2906, 214, 3, 2905, 118, 0, 2904, 245, 0, 2910, 178, 11, 2911, 174, 0, 2912, 159, 0, 2913, 217, 1, 2914, 467, 0, "90025014", 10, 21120, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1566018, 2908, 605546, 17616750000, 3076, 2784, 2922, 2891, 0, 0, 2909, 1, 0, 2908, 256, 88, 2907, 214, 0, 2906, 214, 3, 2905, 118, 0, 2910, 184, 12, 2911, 173, 0, 2912, 159, 0, 2913, 217, 1, 2914, 467, 0, "90025275", 10, 21120, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565990, 2909, 605576, 17617620000, 3076, 2784, 2922, 2891, 0, 0, 2909, 10, 0, 2908, 256, 88, 2907, 220, 0, 2906, 212, 1, 2905, 122, 0, 2910, 186, 13, 2911, 173, 0, 2912, 159, 0, 2913, 217, 1, 2914, 467, 0, "90025525", 10, 21092, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565980, 2910, 605588, 17617970000, 3076, 2784, 2922, 2891, 0, 0, 2909, 5, 0, 2908, 266, 98, 2907, 220, 0, 2906, 213, 1, 2905, 103, 0, 2910, 184, 12, 2911, 123, 0, 2912, 159, 0, 2913, 217, 1, 2914, 467, 0, "90025772", 2, 21082, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565998, 2910, 605690, 17620940000, 3076, 2784, 2922, 2891, 0, 0, 2909, 20, 0, 2908, 275, 112, 2907, 220, 0, 2906, 213, 1, 2905, 104, 0, 2910, 123, 2, 2911, 123, 0, 2912, 159, 0, 2913, 223, 1, 2914, 467, 0, "90026022", 50, 21100, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565998, 2910, 605690, 17620940000, 3076, 2784, 2922, 2891, 0, 0, 2909, 75, 0, 2908, 279, 116, 2907, 220, 0, 2906, 213, 1, 2905, 104, 0, 2910, 124, 0, 2911, 133, 10, 2912, 159, 0, 2913, 223, 1, 2914, 467, 0, "90026246", 50, 21100, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565998, 2910, 605690, 17620940000, 3076, 2784, 2922, 2891, 0, 0, 2909, 115, 0, 2908, 281, 118, 2907, 226, 0, 2906, 214, 2, 2905, 104, 0, 2910, 125, 1, 2911, 122, 0, 2912, 160, 0, 2913, 223, 1, 2914, 467, 0, "90026520", 50, 21100, 2360, 3475, 0, 0, 0, 0, 0, },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565940, 2910, 605804, 17624250000, 3076, 2784, 2922, 2891, 0, 0, 2909, 80, 6, 2908, 206, 43, 2907, 227, 0, 2906, 215, 2, 2905, 104, 0, 2910, 113, 0, 2911, 139, 12, 2912, 161, 0, 2913, 222, 0, 2914, 467, 0, "90026751", 2, 21042, 2360, 3475, 0, 0, 0, 0, 0 },
	{ MI_DCE_BEST_DEEP, 388, 0, 0, "B", "m1701", 0, 2930, 0, 2909, 2910, 0, 2905, 1544898, 1565976, 2909, 605848, 17615530000, 3076, 2784, 2922, 2891, 0, 0, 2909, 91, 6, 2908, 206, 43, 2907, 247, 18, 2906, 216, 0, 2905, 104, 0, 2910, 112, 0, 2911, 137, 10, 2912, 163, 0, 2913, 222, 0, 2914, 467, 0, "90026985", 40, 21078, 2360, 3475, 0, 0, 0, 0, 0 }
};

static product_info g_product[1] = {
    /*
     * symbol, product, fee_mode(0 by amount capital, 1 by trade volumes), fee_rate,
     * tick size（bid or ask's minimal change like 0.001/kg）,
     * multiple of basic unit(like 15kg/lot),
     * exchange prefix, account transfer fee, stamp tax
     * */
	{ "m1701", "m", 1, 0.0003, 1, 10, 'B', 0, 0 },
};

static struct mode_config mc[1] = {
	{NORMAL_MATCH, 'B', "m", 0, 0.5},
};

static place_order_struct g_pld_ord = {
    /**
     * serial_no, a global number identify this order
     * symbol,
     * direction, '0' buy, '1' sell
     * open_close, '0' open, '1' close
     * limit price, a double number
     * volume, a long int number
     * order_kind: @see place_order_struct
     * order_type: @see place_order_struct
     * speculator: @see place_order_struct
     * xchg_code: @see place_order_struct
     */
	2345, "m1701", '0', '0', 2985, 20, '0', '0', '0','B'
};
static cancel_order_struct g_can_ord = {
    /**
     * serial_no global id
     * original serial_no
     * symbol/contract
     * direction
     * open_close
     * speculator
     * xchg_code
     */
	8765, 2345, "m1701", '0', '0', '0', 'B'
};

void test_case1()
{
	MyTrader *trade = new MyTrader(NORMAL_MATCH);
//	MyTrader *trade = new MyTrader(IDEAL_MATCH);

	trade->load_config(mc, 1);
	trade->load_product(g_product, 1);
	for (int i = 0; i < QUOTE_SIZE; i++){
		int rtn_cnt = 0;
		trade_return_struct* trad_acks = NULL;
		trade->feed_quote(&g_dec_quote[i], MI_DCE_BEST_DEEP, &trad_acks, &rtn_cnt);
		if (i == 1) {
			g_pld_ord.direction = DIR_BUY;
			g_pld_ord.limit_price = 2910;
			g_pld_ord.volume = 10;
			order_return_struct order_rtn;
			trade->place_order(&g_pld_ord, &order_rtn);
			cout << "i=" << i << " status=" << order_rtn.entrust_status<<endl;
		}
		for (int n = 0; n < rtn_cnt; n++){
			cout << "i=" << i << " status=" << trad_acks[n].entrust_status
					<< " price=" << trad_acks[n].business_price
				<< " volume=" << trad_acks[n].business_volume << endl;
		}
	}
	trade->round_finish();
	delete trade;
}

void test_case2()
{
	MyTrader *trade = new MyTrader(NORMAL_MATCH);
//	MyTrader *trade = new MyTrader(IDEAL_MATCH);
	trade->load_config(mc, 1);
	trade->load_product(g_product, 1);
	for (int i = 0; i < QUOTE_SIZE; i++){
		int rtn_cnt = 0;
		trade_return_struct* trad_acks = NULL;
		trade->feed_quote(&g_dec_quote[i], MI_DCE_BEST_DEEP, &trad_acks, &rtn_cnt);
		if (i == 1) {
			g_pld_ord.direction = DIR_SELL;
			g_pld_ord.limit_price = 2906;
			g_pld_ord.volume = 10;
			order_return_struct order_rtn;
			trade->place_order(&g_pld_ord, &order_rtn);
			cout << "i=" << i << " status=" << order_rtn.entrust_status<<endl;
		}
		for (int n = 0; n < rtn_cnt; n++){
			cout << "i=" << i << " status=" << trad_acks[n].entrust_status
					<< " price=" << trad_acks[n].business_price
				<< " volume=" << trad_acks[n].business_volume << endl;
		}
	}
	trade->round_finish();
	delete trade;
}

void test_case3()
{
	MyTrader *trade = new MyTrader(LINEAR_IMPACT_MATCH);
//	MyTrader *trade = new MyTrader(IDEAL_MATCH);

	trade->load_config(mc, 1);
	trade->load_product(g_product, 1);
	for (int i = 0; i < QUOTE_SIZE; i++){
		int rtn_cnt = 0;
		trade_return_struct* trad_acks = NULL;
		trade->feed_quote(&g_dec_quote[i], MI_DCE_BEST_DEEP, &trad_acks, &rtn_cnt);
		if (i == 1) {
			g_pld_ord.direction = DIR_SELL;
			g_pld_ord.limit_price = 2908;
			g_pld_ord.volume = 200;
			order_return_struct order_rtn;
			trade->place_order(&g_pld_ord, &order_rtn);
			cout << "i=" << i << " status=" << order_rtn.entrust_status<<endl;
		}
		if (i == 2){
			order_return_struct order_rtn;
			trade->cancel_order(&g_can_ord, &order_rtn);
			cout << "i=" << i << " status=" << order_rtn.entrust_status<<endl;
		}
		if (i == 3){
			order_return_struct order_rtn;
			g_can_ord.org_serial_no = 123; //error serial_no
			trade->cancel_order(&g_can_ord, &order_rtn);
			cout << "i=" << i << " status=" << order_rtn.entrust_status<<endl;
		}
	}
	trade->round_finish();
	delete trade;
}

int main()
{
	test_case1();
	
//	test_case2();
//
//	test_case3();

	return 0;
}
