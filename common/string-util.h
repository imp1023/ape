#pragma once

#include <vector>
#include <sstream>
using namespace std;

bool safe_atoi(const char* str, int &num);
bool safe_atoi(const string &str, int &num);
bool safe_atoi(const string &str, time_t &num);
#ifndef _WIN32
bool safe_atoi(const string &str, size_t &num);
#endif
bool safe_atoi(const string &str, unsigned int &num);
bool safe_atoi(const string &str, unsigned char &num);
bool safe_atoll(const char* str, long long &num);
bool safe_atoll(const string &str, long long &num);
bool safe_atof(const string &str, float &num);
bool tokenize(const string& str, vector<string>& tokens, string &delims);
bool tokenize_as_int(const string& str, vector<int>& tokens, const string &delims);
bool tokenize_as_longlong(const string& str, vector<long long>& tokens, const string &delims);

std::string base64_encode(std::string const& input_str);
std::string base64_decode(std::string const& encoded_string);

int gb2312ToUtf8(std::string const& input_str,std::string& output_str);
int gb2312ToUnicode(std::string const& input_str,std::wstring& output_str);

template<class T> 
string toString(T data) {
	stringstream ssm;
	ssm <<data;
	return ssm.str();
}

template<class T> 
string toString(vector<T> &data) {
	stringstream ssm;
	for (int i=0; i<int(data.size())-1; i++) ssm<<data[i]<<",";
	if (data.size()>0) ssm << *data.rbegin();
	return ssm.str();
}

template<class T>
string toString(int len, T* data) {
	stringstream ssm;
	for (int i=0; i<len-1; i++) ssm<<data[i]<<",";
	if (len>0) ssm << data[len-1];
	return ssm.str();
}
