/*
* utils.cpp
* 
* Copyright(C) by MY Capital Inc. 2007-2999
*/

#include "utils.h"
#include "mytrader_defines.h"
#ifdef WIN32
	#include <windows.h> 
#else
	#include <time.h>
	timespec diff(timespec start, timespec end)
	{
		timespec temp;
		if ((end.tv_nsec - start.tv_nsec) < 0) {
			temp.tv_sec = end.tv_sec - start.tv_sec - 1;
			temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
		}
		else {
			temp.tv_sec = end.tv_sec - start.tv_sec;
			temp.tv_nsec = end.tv_nsec - start.tv_nsec;
		}
		return temp;
	}

#endif

//------------------Data type related---------------------
using SymbolMap = std::map<std::string, std::string>;
const static SymbolMap symbol_map{ { "if", "IF" }, { "ic", "IC" }, { "ih", "IH" }, { "tf", "TF" }, { "t", "T" },
					{ "ag", "shag" }, { "au", "shau" }, { "al", "shal" }, { "bu", "shbu" }, { "cu", "shcu" }, { "rb", "shrb" }, 
					{ "ru", "shru" }, { "zn", "shzn" }, { "ni", "shni" }, { "fu", "shfu" }, { "hc", "shhc" }, { "pb", "shpb" }, 
					{ "wr", "shwr" }, { "sn", "shsn" },{ "a", "dla" }, { "bb", "dlbb" }, { "c", "dlc" }, { "cs", "dlcs" }, { "fb", "dlfb" },
					{ "i", "dli" }, { "j", "dlj" }, { "jd", "dljd" }, { "jm", "dljm" }, { "l", "dll" }, { "m", "dlm" }, { "p", "dlp" },
					{ "pp", "dlpp" }, { "y", "dly" }, { "b", "dlb" }, { "v", "dlv" }, { "rm", "zzrm" }, { "cf", "zzcf" }, 
					{ "sr", "zzsr" }, { "ta", "zzta" }, { "ma", "zzma" }, { "fg", "zzfg" }, { "oi", "zzoi" }, { "zc", "zzzc" }, 
					{ "jr", "zzjr" }, { "li", "zzli" }, { "me", "zzme" }, { "pm", "zzpm" }, { "ri", "zzri" }, { "rs", "zzrs" }, 
					{ "sf", "zzsf" }, { "sm", "zzsm" }, { "tc", "zztc" }, { "wh", "zzwh" }, {"au(t+d)","Au(T+D)"} };


//------------------System related---------------------
//add here

//------------------Conversion related---------------------
void
convert_to_rss_symbol(const char* a_symbol, char* a_out_rss_symbol)
{
	char szName[MAX_SYMBOL_SIZE] = "";
	char *pt = (char *)a_symbol;
	char *ps = (char *)szName;
	while (*pt != '\0'){
		*ps++ = tolower(*pt++);
	}

	if (symbol_map.find(szName) != symbol_map.end()) {
		my_strncpy(a_out_rss_symbol, 7, symbol_map.find(szName)->second.c_str());
	}
	else{
		my_strncpy(a_out_rss_symbol, MAX_SYMBOL_SIZE, a_symbol);
	}
}

void
my_strncpy(char *s1, int n, const char *s2)
{
	s1[0] = s2[0];
	if (s2[0] == 0)
		goto  end;

	s1[1] = s2[1];
	if (s2[1] == 0)
		goto  end;

	s1[2] = s2[2];
	if (s2[2] == 0)
		goto  end;

	s1[3] = s2[3];
	if (s2[3] == 0)
		goto  end;

	s1[4] = s2[4];
	if (s2[4] == 0)
		goto  end;

	s1[5] = s2[5];
	if (s2[5] == 0)
		goto  end;

	s1[6] = s2[6];
	if (s2[6] == 0)
		goto  end;

	s1[7] = s2[7];
	if (s2[7] == 0)
		goto  end;

	s1[8] = s2[8];
	if (s2[8] == 0)
		goto  end;

	s1[9] = s2[9];
	if (s2[9] == 0)
		goto  end;

	s1[10] = s2[10];
	if (s2[10] == 0)
		goto  end;

	s1[11] = s2[11];
	if (s2[11] == 0)
		goto  end;

	strncpy(s1 + 12, s2 + 12, n - 12);
end:
	return;
}

int my_strcmp(const char *s1, const char *s2)
{
	int ret = 1;
	if (s1[0] != s2[0]) goto  end;
	if (s1[0] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[1] != s2[1]) goto  end;
	if (s1[1] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[2] != s2[2])goto  end;
	if (s1[2] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[3] != s2[3]) goto  end;
	if (s1[3] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[4] != s2[4]) goto  end;
	if (s1[4] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[5] != s2[5]) goto  end;
	if (s1[5] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[6] != s2[6]) goto  end;
	if (s1[6] == 0) { 
		ret = 0; goto  end; 
	}

	if (s1[7] != s2[7]) goto  end;
	if (s1[7] == 0) {
		ret = 0; goto  end; 
	}

	ret = strcmp(s1 + 8, s2 + 8);
end:
	return ret;
}

unsigned long my_hash_value(const char *str_key)
{
	char *pt = (char *)str_key;
	unsigned int hash = 1315423911;

//	if (pt[0] == 0) goto end;
//	hash ^= ((hash << 5) + pt[0] + (hash >> 2));
//	if (pt[1] == 0) goto end;
//	hash ^= ((hash << 5) + pt[1] + (hash >> 2));
//	if (pt[2] == 0) goto end;
//	hash ^= ((hash << 5) + pt[2] + (hash >> 2));
//	if (pt[3] == 0) goto end;
//	hash ^= ((hash << 5) + pt[3] + (hash >> 2));
//
//	if (pt[4] == 0) goto end;
//	hash ^= ((hash << 5) + pt[4] + (hash >> 2));
//	if (pt[5] == 0) goto end;
//	hash ^= ((hash << 5) + pt[5] + (hash >> 2));
//	if (pt[6] == 0) goto end;
//	hash ^= ((hash << 5) + pt[6] + (hash >> 2));
//	if (pt[7] == 0) goto end;
//	hash ^= ((hash << 5) + pt[7] + (hash >> 2));

//	pt = (char *)(str_key + 8);
	while (*pt != 0){
		hash ^= ((hash << 5) + *pt + (hash >> 2));
		pt++;
	}

end:
	return hash;
}

void
get_product_by_symbol(const char *symbol, const char *product)
{
	char *ps = (char *)symbol;
	char *pd = (char *)product;
	while (*ps != '\0'){
		if (isalpha(*ps)){
			*pd = tolower(*ps);
			pd++;
		}
		ps++;
	}
	*pd = '\0';
}

char
get_exch_by_symbol(const char *a_symbol)
{
	char exch = 'u'; // undefined
	/* convert to lower case */
	char char0 = (a_symbol[0] >= 'A' && a_symbol[0] <= 'Z') ? a_symbol[0] + 32 : a_symbol[0];
	char char1 = (a_symbol[1] >= 'A' && a_symbol[1] <= 'Z') ? a_symbol[1] + 32 : a_symbol[1];

	switch (char0) {
	case 'a':
		switch (char1) {
		case 'g': case 'l': case 'u': // ag al au
			exch = 'A'; break;
		default:  // a
			exch = 'B'; break;
		} break;
	case 'b':
		switch (char1) {
		case 'u': // bu
			exch = 'A'; break;
		default:  // b, bb
			exch = 'B'; break;
		} break;
	case 'c':
		switch (char1) {
		case 'f': // cf
			exch = 'C'; break;
		case 'u': // cu
			exch = 'A'; break;
		default: // c cs
			exch = 'B'; break;
		} break;
	case 'f':
		switch (char1) {
		case 'b': // fb
			exch = 'B'; break;
		case 'g': // fg
			exch = 'C'; break;
		case 'u': // fu
			exch = 'A'; break;
		} break;
	case 'h': // hc
		exch = 'A';
		break;
	case 'i':
		switch (char1) {
		case 'c': case 'f': case 'h': // ic if ih
			exch = 'G'; break;
		default: // i
			exch = 'B'; break;
		} break;
	case 'j':
		switch (char1) {
		case 'r': // jr
			exch = 'C'; break;
		default: // j jd jm
			exch = 'B'; break;
		} break;
	case 'l':
		switch (char1) {
		case 'i': // li
			exch = 'C'; break;
		default: // l
			exch = 'B'; break;
		} break;
	case 'm':
		switch (char1) {
		case 'a': case 'e': // ma me
			exch = 'C'; break;
		default: // m
			exch = 'B'; break;
		} break;
	case 'n': // ni
		exch = 'A';
		break;
	case 'o': // oi
		exch = 'C';
		break;
	case 'p':
		switch (char1) {
		case 'b': // pb
			exch = 'A'; break;
		case 'm': // pm
			exch = 'C'; break;
		default: // p pp
			exch = 'B'; break;
		} break;
	case 'r':
		switch (char1) {
		case 'b': case 'u': // rb ru
			exch = 'A'; break;
		case 'i': case 'm': case 's': // ri rm rs
			exch = 'C'; break;
		} break;
	case 's': // sf sm sr
		exch = 'C';
		break;
	case 't':
		switch (char1) {
		case 'a': case 'c': // ta tc
			exch = 'C'; break;
		default: // t tf
			exch = 'G'; break;
		} break;
	case 'v': // v
		exch = 'B';
		break;
	case 'w':
		switch (char1) {
		case 'h': // wh
			exch = 'C'; break;
		case 'r': // wr
			exch = 'A'; break;
		} break;
	case 'y': // y
		exch = 'B';
		break;
	case 'z':
		switch (char1) {
		case 'c': // zc
			exch = 'C'; break;
		case 'n': // zn
			exch = 'A'; break;
		} break;
	default:
		break;
	}

	return exch;
}

bool
is_match(char ch, char delims[])
{
	for (size_t i = 0; i < strlen(delims); i++){
		if (ch == delims[i])
			return true;
	}
	return false;
}

bool
split_string(std::string tmpStr, char delims[], std::string & tmpName, double & tmpNum)
{
	if (tmpStr == "")
		return false;
	SplitVec tmpVec;
	split_string(tmpStr.c_str(), delims, tmpVec);
	tmpName = tmpVec[0];
	tmpNum = atof(tmpVec[1].c_str());
	return true;
}

void
split_string(const char *line, char delims[], SplitVec &elems)
{
	if (line == NULL || line[0] == '\0')
		return;
	char token[TOKEN_BUFFER_SIZE];
	char *pStart = (char *)line, *pNext = pStart;
	while (*pNext != '\0' && *pNext != '\r' && *pNext != '\n')
	{
		if (is_match(*pNext, delims)){
			memset(token, 0x0, TOKEN_BUFFER_SIZE);
			memcpy(token, pStart, pNext - pStart);
			elems.emplace_back(token);
			pStart = pNext + 1;
		}
		pNext++;
	}
	memset(token, 0x0, TOKEN_BUFFER_SIZE);
	memcpy(token, pStart, pNext - pStart);
	elems.emplace_back(token);
}

void
split_string(const std::string &line, char delims[], SplitVec &elems)
{
	split_string(line.c_str(), delims, elems);
}

bool is_endswith(char *large, char *sub)
{
	size_t one_len = strlen(large);
	size_t two_len = strlen(sub);
	if (two_len > one_len)  return false;

	for (size_t i = 1; i <= two_len; i++){
		if (large[one_len - i] != sub[two_len - i])
			return false;
	}

	return true;
}

std::string&
my_trim(std::string &ss)
{
	const char * drop = " \r\n\t";
	if (ss.empty()){
		return ss;
	}
	ss.erase(0, ss.find_first_not_of(drop));
	ss.erase(ss.find_last_not_of(drop) + 1);
	return ss;
}

int str2int(char *str)
{
	int result = 0;
	std::string temp(str);
	str2num<int>(my_trim(temp), result);
	return result;
}

double str2double(char * str)
{
	double result = 0.0;
	std::string temp(str);
	str2num<double>(my_trim(temp), result);
	return result;
}

int
calc_increment_count(Parameter *a_parm, double epsilon)
{
	return (int)((a_parm->max - a_parm->min + epsilon) / a_parm->increment) + 1;
}

std::vector<double>
get_parmeter_value(std::vector< Parameter*> *a_para, int a_parm_index)
{
	std::vector<double> l_values;
	int l_current_index = a_parm_index;

	for (int i = 0; i < (*a_para).size(); i++){
		Parameter *l_parm = (*a_para)[i];
		int l_tmp = l_current_index % l_parm->increment_times;
		double l_value = l_parm->min + (double)l_tmp * l_parm->increment;

		/* Reset current_index for the next loop */
		int l_tmp1 = l_current_index / l_parm->increment_times;
		l_current_index = l_tmp1;
		l_values.push_back(l_value);
	}

	return l_values;
}

int
compare(double a, double b, double epsilon)
{	
	if (a - b > epsilon)
		return 1;
	else if (a - b < -epsilon)
		return -1;
	else
		return 0;
}

bool 
case_compare(const char *one, const char *two)
{
	char *pOne = (char *)one;
	char *pTwo = (char *)two;
	if (strlen(one) != strlen(two))
		return false;
	while (*pOne != '\0'){
		if (tolower(*pOne) != tolower(*pTwo))
			return false;
		pOne++;
		pTwo++;
	}
	return true;
}


//------------------ Time & date related---------------------
int get_now_time()
{
	int result = 0;
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	result = sys.wHour * 10000 + sys.wMinute * 100 + sys.wSecond;
#else
	time_t timep;
	struct tm *p_tm;
	time(&timep);
	p_tm = localtime(&timep);
	result = p_tm->tm_hour * 10000 + p_tm->tm_min * 100 + p_tm->tm_sec;
#endif
	return result;
}

int get_now_date()
{
	int result = 0;
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	result = sys.wYear * 10000 + sys.wMonth * 100 + sys.wDay;
#else
	time_t timep;
	struct tm *p_tm;
	time(&timep);
	p_tm = localtime(&timep);
	result = (p_tm->tm_year + 1900) * 10000 + (p_tm->tm_mon + 1) * 100 + p_tm->tm_mday;
#endif
	return result;

}

/* Convert char date string, 2016-11-01 */
int str_date_to_int(const char *str_date)
{
	int result = 0;
	if (strlen(str_date) >= 10){
		int year = (str_date[0] - 48) * 1000 + (str_date[1] - 48) * 100
			+ (str_date[2] - 48) * 10 + (str_date[3] - 48);
		int month = (str_date[5] - 48) * 10 + (str_date[6] - 48);
		int day = (str_date[8] - 48) * 10 + (str_date[9] - 48);
		result = year * 10000 + month * 100 + day;
	}
	return result;
}

int get_time_diff(int time1, int time2)
{
	int time1_hour = time1 / 10000000;
	int time1_min = (time1 / 100000) % 100;
	int time1_sec = ((time1 / 1000) % 100);
	int time2_hour = time2 / 10000000;
	int time2_min = (time2 / 100000) % 100;
	int time2_sec = ((time2 / 1000) % 100);

	int hour_diff = time2_hour - time1_hour;
	int min_diff = time2_min - time1_min;
	int sec_diff = time2_sec - time1_sec;
	return (hour_diff * 3600 + min_diff * 60 + sec_diff);
}

int add_time(int int_time, int seconds)
{
	int time_hour = int_time / 10000000;
	int time_min = (int_time / 100000) % 100;
	int time_sec = ((int_time / 1000) % 100);

	int current_sec = time_hour * 3600 + time_min * 60 + time_sec;
	int sum_sec = current_sec + seconds;

	int time1_hour = sum_sec / 3600;
	int time1_min = (sum_sec % 3600) / 60;
	int time1_sec = ((sum_sec % 3600) % 60);
	return (time1_hour * 10000 + time1_min * 100 + time1_sec) * 1000;
}


int convert_itime_2_sec(int itime)
{
	int isec;
	int it1, it2, it3;
	it1 = itime / 10000000;
	it2 = (itime - it1 * 10000000) / 100000;
	it3 = (itime % 100000) / 1000;
	isec = it3 + it2 * 60 + it1 * 3600;

	return isec;
}

int get_int_time(char *str_time)
{
	if (str_time == NULL) return 0;
	int result = atoi(str_time);
	if (result < 30000000)
		result = result + 240000000;
	return result;
}


/* Convert char date string, 2016-11-01 */
int get_date(const char *str_date)
{
	int result = 0;
	if (strlen(str_date) >= 8){
		int year = (str_date[0] - 48) * 1000 + (str_date[1] - 48) * 100
			+ (str_date[2] - 48) * 10 + (str_date[3] - 48);
		int month = (str_date[4] - 48) * 10 + (str_date[5] - 48);
		int day = (str_date[6] - 48) * 10 + (str_date[7] - 48);
		result = year * 10000 + month * 100 + day;
	}
	return result;
}

int get_timestamp(char *src, int millisec)  // char = "13.01.01"
{
	int t;
	t = src[0] - 48;
	t = t * 10;
	t = t + (src[1] - 48);
	t = t * 10;
	t = t + (src[3] - 48);
	t = t * 10;
	t = t + (src[4] - 48);
	t = t * 10;
	t = t + (src[6] - 48);
	t = t * 10;
	t = t + (src[7] - 48);

	t = t * 1000;
	t = t + millisec;

	if (t < 30000000)
		t = t + 240000000;

	return t;
}

int get_timestamp_sge(char *src, int millisec)  //char = "130101" or "20101"
{
	int t;
	if (strlen(src) > 8){
		t = src[0] - 48;
		t = t * 10;
		t = t + (src[1] - 48);
		t = t * 10;
		t = t + (src[2] - 48);
		t = t * 10;
		t = t + (src[3] - 48);
		t = t * 10;
		t = t + (src[4] - 48);
		t = t * 10;
		t = t + (src[5] - 48);
	}
	else {
		t = src[0] - 48;
		t = t * 10;
		t = t + (src[1] - 48);
		t = t * 10;
		t = t + (src[2] - 48);
		t = t * 10;
		t = t + (src[3] - 48);
		t = t * 10;
		t = t + (src[4] - 48);
	}

	t = t * 1000;
	t = t + millisec;

	if (t < 30000000)
		t = t + 240000000;

	return t;
}

void merge_timestamp(char *update, int millisec, char *result)
{
	char temp[16] = "";
	result[0] = update[0]; result[1] = update[1];
	result[2] = update[3]; result[3] = update[4];
	result[4] = update[6]; result[5] = update[7];

	snprintf(temp, 4, "%03d", millisec);
	result[6] = temp[0]; result[7] = temp[1];
	result[8] = temp[2]; result[9] = '\0';
}

int get_timestamp(char *timestamp, bool is_all)
{
	int nret = 0;
	char *update = NULL;
	if (is_all){
		update = timestamp;
	}
	else{
		update = timestamp + 11;
	}

	nret = *update - 48;
	nret = nret * 10;
	nret = nret + (*(update + 1) - 48);
	nret = nret * 10;
	nret = nret + (*(update + 3) - 48);
	nret = nret * 10;
	nret = nret + (*(update + 4) - 48);
	nret = nret * 10;
	nret = nret + (*(update + 6) - 48);
	nret = nret * 10;
	nret = nret + (*(update + 7) - 48);

	if (*(update + 8) == '.'){
		nret = nret * 10;
		nret = nret + (*(update + 9) - 48);
		nret = nret * 10;
		nret = nret + (*(update + 10) - 48);
		nret = nret * 10;
		nret = nret + (*(update + 11) - 48);
	}else{
		nret = nret * 1000;
	}

	if (nret < 30000000)
		nret = nret + 240000000;
	return nret;
}

//------------------ Algorithm  related---------------------
int
search_double_array(double arr[], int size, double target)
{
	for (int i = 0; i < size; i++){
		if (compare(arr[i], target, PRECISION) == 0)
			return i;
	}
	return -1;
}
