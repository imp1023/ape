#include "counter.h"

void Counter::clearAll() {
	counter_.clear();
}
void Counter::clear(const string &name) {
	counter_iter iter = counter_.find(name);
	if (iter==counter_.end()) return;
	iter->second = 0;
}
int Counter::set(const string &name, int value) {
	counter_iter iter = counter_.find(name);
	if (iter==counter_.end()) {
		counter_.insert(make_pair(name, value));
	} else iter->second = value;
	return counter_[name];
}
int Counter::increase(const string &name) {
	counter_iter iter = counter_.find(name);
	if (iter==counter_.end()) {
		counter_.insert(make_pair(name, 0));
		iter = counter_.find(name);
	}
	iter->second ++;
	return iter->second;
}
int Counter::decrease(const string &name) {
	counter_iter iter = counter_.find(name);
	if (iter==counter_.end()) {
		counter_.insert(make_pair(name, 0));
		iter = counter_.find(name);
	}
	iter->second --;
	return iter->second;
}
int Counter::count(const string &name) {
	counter_iter iter = counter_.find(name);
	if (iter==counter_.end()) return 0;
	return iter->second;
}
