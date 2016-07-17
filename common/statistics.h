#ifndef _STATISTICS_H_
#define _STATISTICS_H_
#pragma once

#include <time.h>
#include <string>
#include <vector>
#include <deque>
#include <map>
using namespace std;

class StatItem {
public:
	StatItem();
	~StatItem();
	void sample(double value);
	void sample(double value, time_t now);
	vector<double> recentOneHour(time_t now);
	vector<double> recentOneDay(time_t now);

protected:
	void roll_time(int idx, int time_value);

private:
	// index 0 - minite,  index 1 - hour
	double accumulator_[2];
	int   counter_[2];
	int   indicator_[2];
	vector<double> record_[2];
};

class Statistics {
public:
	Statistics();
	~Statistics();

	void capture(const string &name, double value);
	void capture(const string &name, double value, time_t now);
	vector<string> names();
	void clear();

	vector<double> recentOneHour(const string &name);
	vector<double> recentOneDay(const string &name);
	// 1 min, 5 min, 20 min, 60 min, 2 hour, 6 hour, 24 hour
	vector<double> recentDigest(const string &name);

private:
	map<string, StatItem*> items_;
	typedef map<string, StatItem*>::iterator items_iter;
};

class Profile {
public:
	Profile();
	void hitEvent(int id, double time);
	void clear();
	void report();
private:
	map<int, double> time_elapsed_;
	map<int, long long> event_hit_;
};

#endif
