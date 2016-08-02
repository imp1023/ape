#pragma once

#include <string>
#include <sstream>
#include "json/json.h"

using namespace std;
using namespace Json;

inline string getJsonStr(Json::Value &json)
{
	StyledWriter writer;  
	return writer.write(json);  
}
inline Json::Value parseJsonStr(const string &str,bool& bSuc) {
	Reader reader;
	Json::Value json(arrayValue);
	bSuc = reader.parse(str, json);
	if (!bSuc) json.clear();
	return json;
}
