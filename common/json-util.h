#pragma once

#include <string>
#include <sstream>
#include "json/json.h"

using namespace std;
using namespace Json;

inline string getJsonStr(Value &json)
{
	StyledWriter writer;  
	return writer.write(json);  
}
inline Value parseJsonStr(const string &str) {
	Reader reader;
	Value json(arrayValue);
	bool succ = reader.parse(str, json);
	if (!succ) json.clear();
	return json;
}
