/* 
 * File:   Proto2Json.h
 * Author: Kidd
 *
 * Created on 2014年1月24日, 上午10:15
 */

#ifndef PROTO2JSON_H
#define	PROTO2JSON_H
#include "json-util.h"
#include <google/protobuf/descriptor.h>
void Proto2Json(Value& json, const google::protobuf::Message& msg);
google::protobuf::Message* GetMessageByPath(google::protobuf::Message* pMsg, const std::string& path, bool bWhenErrReturnParent = false);
Value* JsonPathSearch(Value* pRoot, const std::string& path);
#endif	/* PROTO2JSON_H */
