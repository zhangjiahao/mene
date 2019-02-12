#ifndef C_QUOTE_MY_H
#define C_QUOTE_MY_H

struct my_marketdata 
{
	long time;
	char contract[64];
	//char direction;

	int curr_volume;
	int b_total_volume;
	int s_total_volume;
	double last_price;
	double b_open_interest;
	double s_open_interest;

	double buy_price[30];
	int buy_vol[30];
	double sell_price[30];
	int sell_vol[30];
};

#endif