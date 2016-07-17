#ifndef _COUNTER_H_
#define _COUNTER_H_

#pragma once

#include <map>
#include <string>

using namespace std;

class Counter 
{
public:
	void	clearAll();
	void	clear(const string &name);
	int		set(const string &name, int value);
	int		increase(const string &name);
	int		decrease(const string &name);
	int		count(const string &name);
private:
	map<string, int> counter_;
	typedef map<string, int>::iterator counter_iter;
};

#endif
