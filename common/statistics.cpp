#define _CRT_SECURE_NO_WARNINGS
#include "statistics.h"
#include <sstream>
#include <fstream>

using namespace std;

StatItem::StatItem() {
	accumulator_[0] = accumulator_[1] = 0.f;
	counter_[0] = counter_[1] = 0;
	indicator_[0] = indicator_[1] = 0;
	record_[0].resize(60, 0.f);
	record_[1].resize(24, 0.f);
}

StatItem::~StatItem() {
}

void StatItem::sample(double value) {
	time_t now = time(NULL);
	sample(value, now);
}

void StatItem::sample(double value, time_t now) {
	struct tm *nowtm = localtime(&now);
	int sample_time[2] = {nowtm->tm_min, nowtm->tm_hour};
	for (int i=0; i<2; i++) {
		roll_time(i, sample_time[i]);
		accumulator_[i] += value;
		counter_[i] ++;
		record_[i][indicator_[i]] = (counter_[i]==0)? 0.f : accumulator_[i] / counter_[i];
	}
}

vector<double> StatItem::recentOneHour(time_t now) {
	vector<double> rec;
	struct tm * nowtm = localtime(&now);
	roll_time(0, nowtm->tm_min);
	for (int i=0; i<60; i++) {
		rec.push_back(record_[0][(indicator_[0]-i+60)%60]);
	}
	return rec;
}

vector<double> StatItem::recentOneDay(time_t now) {
	vector<double> rec;
	struct tm * nowtm = localtime(&now);
	roll_time(1, nowtm->tm_hour);
	for (int i=0; i<24; i++) {
		rec.push_back(record_[1][(indicator_[1]-i+24)%24]);
	}
	return rec;
}

void StatItem::roll_time(int i, int time_value) {
	while (indicator_[i] != time_value) {
		record_[i][indicator_[i]] = (counter_[i]==0)? 0.f : accumulator_[i] / counter_[i];
		indicator_[i] = (indicator_[i]+1)%record_[i].size();
		accumulator_[i] = 0.f;
		counter_[i] = 0;
	}
}

Statistics::Statistics() {}

Statistics::~Statistics() {
	for (items_iter iter = items_.begin(); iter!=items_.end(); ++iter)
		delete iter->second;
}

vector<string> Statistics::names() {
	vector<string> name;
	for (items_iter iter = items_.begin(); iter!=items_.end(); ++iter)
		name.push_back(iter->first);
	return name;
}

void Statistics::clear() {
	items_.clear();
}

void Statistics::capture(const string &name, double value) {
	time_t now = time(NULL);
	capture(name, value, now);
}

void Statistics::capture(const std::string &name, double value, time_t now) {
	items_iter iter = items_.find(name);
	if (iter==items_.end()) {
		items_.insert(make_pair(name, new StatItem()));
		iter = items_.find(name);
	}
	iter->second->sample(value, now);
}

vector<double> Statistics::recentOneHour(const string &name) {
	items_iter iter = items_.find(name);
	if (iter==items_.end()) return vector<double>(60, 0);
	time_t now = time(NULL);
	return iter->second->recentOneHour(now);
}

vector<double> Statistics::recentOneDay(const string &name) {
	items_iter iter = items_.find(name);
	if (iter==items_.end()) return vector<double>(24, 0);
	time_t now = time(NULL);
	return iter->second->recentOneDay(now);
}

vector<double> Statistics::recentDigest(const string &name) {
	// 1 min, 5 min, 20 min, 1 hour, 2 hour, 6 hour, 24 hour
	items_iter iter = items_.find(name);
	if (iter==items_.end()) return vector<double>(7, 0);
	time_t now = time(NULL);
	vector<double> minute = iter->second->recentOneHour(now);
	vector<double> hour = iter->second->recentOneDay(now);
	vector<double> digest;
	double acc = 0.f;
	for (int i=0; i<20; i++) {
		acc += minute[i];
		if (i==0) {
			digest.push_back(acc);
		} else if (i==4) {
			digest.push_back(acc/5);
		} else if (i==19) {
			digest.push_back(acc/20);
		}
	}
	acc = 0.f;
	for (int i=0; i<24; i++) {
		acc += hour[i];
		if (i==0) {
			digest.push_back(acc);
		} else if (i==1) {
			digest.push_back(acc/2);
		} else if (i==5) {
			digest.push_back(acc/6);
		} else if (i==23) {
			digest.push_back(acc/24);
		}
	}
	return digest;
}

Profile::Profile() {
	clear();
}

void Profile::clear() {
	time_elapsed_.clear();
	event_hit_.clear();
}

void Profile::hitEvent(int id, double time) {
	if (time_elapsed_.find(id)==time_elapsed_.end()) {
		time_elapsed_[id] = 0;
		event_hit_[id] = 0;
	}
	time_elapsed_[id] += time;
	event_hit_[id] += 1;
}

void Profile::report() {
	ofstream outfile ("profile.log", ofstream::binary);
	double totalTime = 0;
	long long totalHit = 0;
	for (map<int,double>::iterator iter = time_elapsed_.begin(); iter!=time_elapsed_.end(); ++iter) {
		totalHit += event_hit_[iter->first];
		totalTime += iter->second;
	}
	outfile<<"Event\t\tHit\t\tTime\t\tTime%\t\tTime/Hit"<<endl;
	for (map<int,double>::iterator iter = time_elapsed_.begin(); iter!=time_elapsed_.end(); ++iter) {
		outfile<<iter->first<<"\t\t"<<event_hit_[iter->first]<<"\t\t"<<iter->second<<"\t\t"<<iter->second/totalTime*100<<"%\t\t"<<
			iter->second/event_hit_[iter->first]<<endl;
	}
	outfile<<"SUM\t\t"<<totalHit<<"\t\t"<<totalTime<<"\t\t100%\t\t"<<totalTime/totalHit<<endl;
	outfile.close();
}
