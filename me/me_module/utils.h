/*
* utils.h
*
* Provide utils functions
*
* Copyright(C) by MY Capital Inc. 2007-2999
*/
#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdint>

//------------------Data type related---------------------

struct Parameter
{
	double min;
	double max;
	double increment;
	int increment_times;
};

using SplitVec = std::vector<std::string>;

//------------------System related---------------------
#define TOKEN_BUFFER_SIZE 1024
#define PRECISION         0.0001
#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)>(b)?(b):(a))

#ifdef WIN32
#define snprintf sprintf_s
#else
inline void
__localtime_s_(struct tm *result, const time_t *time){
	*result = *localtime(time);
}

inline void
__fopen_s_(FILE **pFile, const char *filename, const char *mode){
	*pFile = fopen(filename, mode);
}

#define strcpy_s(dest, size, src)       memcpy(dest, src, size)
#define localtime_s(result, time)       __localtime_s_(result, time)
#define fopen_s(pFile, filename, mode)  __fopen_s_(pFile, filename, mode)
#define strtok_s(str, delim, saveptr)   strtok_r(str, delim, saveptr)

/* High Precision Time Compare */
#define HP_TIMING_NOW(Var) \
 ({ unsigned long long _hi, _lo; \
  asm volatile ("rdtsc" : "=a" (_lo), "=d" (_hi)); \
  (Var) = _hi << 32 | _lo; })

/*  Example: Compare clock time
unsigned long start, end;
HP_TIMING_NOW(start);
Do something...
HP_TIMING_NOW(end);

printf("%ld\n", end - start);
*/


#include <time.h>
timespec diff(timespec start, timespec end);

/*  Example: Compare clock time
timespec ts0,ts1;
clock_gettime(CLOCK_MONOTONIC_RAW, &ts0);
Do something...
clock_gettime(CLOCK_MONOTONIC_RAW, &ts1);
cout<<"============TimeDiff: "<<diff(ts0,ts1).tv_sec<<":"<<diff(ts0,ts1).tv_nsec<<endl;
*/

#endif   /* WIN32 */


//------------------Conversion related---------------------
void convert_to_rss_symbol(const char* a_symbol, char* a_out_rss_symbol);
void my_strncpy(char *dest, int size, const char *src);
void get_product_by_symbol(const char *symbol, const char *product);
char get_exch_by_symbol(const char *a_symbol);

int my_strcmp(const char *s1, const char *s2);
unsigned long my_hash_value(const char *str_key);

void split_string(const char *line, char delims[], SplitVec &elems);
bool split_string(std::string tmpStr, char delims[], std::string & tmpName, double & tmpNum);
void split_string(const std::string &line, char delims[], SplitVec &elems);

bool is_endswith(char *large, char *sub);

std::string& my_trim(std::string &ss);
int str2int(char *str);
double str2double(char * str);

int calc_increment_count(Parameter *a_parm, double epsilon = PRECISION);
std::vector<double> get_parmeter_value(std::vector< Parameter*> *a_para, int a_parm_index);

template<class TP>
std::string num2str(TP num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

template<class TP>
TP str2num(std::string str, TP &ret)
{
	TP num;
	std::stringstream ss(str);
	ss >> num;
	ret = num;
	return num;
}



/* Compare two doubles, if less return -1, if more return 1, or return 0 when equal */
int compare(double a, double b, double epsilon = PRECISION);

bool case_compare(const char *one, const char *two);

//------------------ Time & date related---------------------
int get_now_time();
int get_now_date();

int get_time_diff(int time1, int time2);
int add_time(int int_time, int seconds);

//"2016-03-14" --> int(20160314)
int str_date_to_int(const char *str_date);

int convert_itime_2_sec(int itime);

int get_int_time(char *str_time);

int get_timestamp(char *src, int millisec);

void merge_timestamp(char *update, int millisec, char *result);

int get_timestamp(char *timestamp, bool is_all);

int get_timestamp_sge(char *src, int millisec);

int get_date(const char *str_date);

//------------------ Algorithm  related---------------------
template<typename TP> double
sum(TP data[], int len)
{
	if (len <= 0) return 0.0;

	double std_sum = 0.0;
	for (int i = 0; i < len; i++){
		std_sum += data[i];
	}
	return std_sum;
}

template<typename TP> double
mean(TP data[], int len)
{
	if (len <= 0) return 0.0;

	double std_sum = 0.0;
	for (int i = 0; i < len; i++){
		std_sum += data[i];
	}
	return std_sum / len;
}

template<typename TP> double
stdev(TP data[], int len)
{
	if (len <= 0) return 0.0;

	double pow_sum = 0.0;
	double l_mean = mean(data, len);
	for (int i = 0; i < len; i++){
		double diff = data[i] - l_mean;
#ifdef __APPLE__
        pow_sum += diff * diff;
#else
		pow_sum += std::pow(diff, 2);
#endif
	}
#ifdef __APPLE__
    return sqrt(pow_sum / len);
#else
	return std::sqrt(pow_sum / len);
#endif
}

template<typename TP> double
stdev_deltas(TP data[], int len)
{
	if (len <= 0) return 0.0;

	for (int i = len-1; i > 0; i--){
		data[i] -= data[i-1];
	}

	return stdev(data, len);
}


/* Convert cumulative values to deltas (replacing in-place) */
/* Example: {0, 5, 8, 15, 9} -> {0, 5, 3, 7, -6} */
template<typename T> inline std::vector<T>&
vector_to_deltas(std::vector<T>& vec)
{
	if (vec.size() > 1) {
		/* iterate backward, and subtract the previous one */
		std::transform(std::next(vec.rbegin()), vec.rend(), vec.rbegin(), vec.rbegin(), [](T a, T b){ return b - a; });
	}
	return vec;
}

template<typename T> inline double
vector_mean(std::vector<T> const& vec)
{
	if (vec.size() > 0) {
		double res = std::accumulate(vec.begin(), vec.end(), (T)0);
		return res / vec.size();
	}
	else {
		return 0.0;
	}
}

template<typename T> inline double
vector_sum(std::vector<T> const& vec)
{
	if (vec.size() > 0) {
		double res = std::accumulate(vec.begin(), vec.end(), (T)0);
		return res;
	}
	else {
		return 0.0;
	}
}

template<typename T> inline double
vector_stdev(std::vector<T> const& v)
{
	if (v.size() > 0) {
		double mean = vector_mean(v);
		std::vector<double> diff(v.size());
		std::transform(v.begin(), v.end(), diff.begin(), [mean](double x) { return x - mean; });
		double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
#ifdef __APPLE__
		return sqrt(sq_sum / v.size());
#else
		return std::sqrt(sq_sum / v.size());
#endif
	}
	else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

template<typename T> inline bool
sort_cmp(const T &a, const T &b)
{
	return a > b;
}

template<typename T> void
sort_array(T arr[], int size, bool ascending = true)
{
	if (ascending)
		std::sort(arr, arr + size);
	else
		std::sort(arr, arr + size, sort_cmp<T>);
}

template<typename T> int
search_array(T arr[], int size, T target)
{
	for (int i = 0; i < size; i++){
		if (arr[i] == target)
			return i;
	}
	return -1;
}

int search_double_array(double arr[], int size, double target);

template<typename T>  void
search_all_array(T arr[], int size, T target, std::vector<int>& result)
{
	result.clear();
	for (int i = 0; i < size; i++){
		if (arr[i] == target)
			result.push_back(i);
	}
}

//---------------------- strategy status --------------------------
#define STRAT_STATUS_BIT_SIZE   4      /* each strategy has this number of bits in m_strat_status */
#define STRAT_STATUS_BIT_BLOCK  15ULL  /* 0000...00001111 (15) if STRAT_STATUS_BIT_SIZE == 4 */

/* Get strategy's status by index */
uint64_t inline
get_strat_status(uint64_t a_all_status, unsigned int a_idx)
{
	return (a_all_status >> (a_idx * STRAT_STATUS_BIT_SIZE)) & STRAT_STATUS_BIT_BLOCK;
}

/* Set strategy's status by index */
uint64_t inline
set_strat_status(uint64_t *a_all_status, unsigned int a_idx, int a_status)
{
	unsigned int l_offset = a_idx * STRAT_STATUS_BIT_SIZE;
	(*a_all_status) &= ~(STRAT_STATUS_BIT_BLOCK << l_offset); /* clear status */
	return (*a_all_status) |= (((uint64_t)a_status) & STRAT_STATUS_BIT_BLOCK) << l_offset; /* set status */
}

#endif

