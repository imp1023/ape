#include "rand_util.h"
#include <string>
#ifndef WIN32
#include <stdlib.h>
#endif

//·µ»Ø[0,1)Ëæ»úÊı
double get_rand()
{
	return rand() / (RAND_MAX + 1.0);
}

bool rand_by_percent(double percent)
{
	if (get_rand() >= percent)
	{
		return false;
	}
	return true;
}

int rand_by_weight( const std::vector<int>& val, const std::vector<int>& rnd )
{
	if (val.size() != rnd.size())
		return -1;
	int sum = 0;
	std::vector<int>::const_iterator iter = rnd.begin();
	for (; iter!=rnd.end(); iter++)
		sum += *iter;
	int randnum = (int)(sum * get_rand());
	int k = 0;
	for (iter = rnd.begin(); iter!=rnd.end(); iter++, k++)
	{
		if (*iter > randnum) break;
		randnum -= *iter;
	}
	return val[k];
}
