#include "Proto2Json.h"
#include "string-util.h"
#include "const_def.h"
#include "../event/event.pb.h"

void
Proto2Json(Value& json, const google::protobuf::Message& msg)
{
	const google::protobuf::Reflection* pRef = msg.GetReflection();
	std::vector<const google::protobuf::FieldDescriptor*> fields;
	pRef->ListFields(msg, &fields);
	for (size_t i = 0; i < fields.size(); i++)
	{
		if (fields[i]->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			if (fields[i]->is_repeated())
			{
				Value subarr(arrayValue);
				int cnt = pRef->FieldSize(msg, fields[i]);
				for (int c = 0; c < cnt; c++)
				{
					Value sub(objectValue);
					Proto2Json(sub, pRef->GetRepeatedMessage(msg, fields[i], c));
					subarr.append(sub);
					std::string db = sub.toSingleString();
				}
				json[fields[i]->name()] = subarr;
			}
			else
			{
				Value sub(objectValue);
				Proto2Json(sub, pRef->GetMessage(msg, fields[i]));
				json[fields[i]->name()] = sub;
			}
		}
		// Json doesn't support int64 .so we cast it into string;
		if (fields[i]->type() == google::protobuf::FieldDescriptor::TYPE_INT64)
		{
			if (fields[i]->is_repeated())
			{
				Value subarr(arrayValue);
				int cnt = pRef->FieldSize(msg, fields[i]);
				for (int c = 0; c < cnt; c++)
					subarr.append( toString<int64 > (pRef->GetRepeatedInt64(msg, fields[i], c))) ;
				json[fields[i]->name()]  = subarr;
			}
			else
			{
				json[fields[i]->name()] = toString<int64 > (pRef->GetInt64(msg, fields[i]));
			}
		}
		//enum value
		if (fields[i]->type() == google::protobuf::FieldDescriptor::TYPE_ENUM)
		{
			if (fields[i]->is_repeated())
			{
				Value subarr(arrayValue);
				int cnt = pRef->FieldSize(msg, fields[i]);
				for (int c = 0; c < cnt; c++)
					subarr.append(pRef->GetRepeatedEnum(msg, fields[i], c)->name()) ;
				json[fields[i]->name()]  = subarr;
			}
			else
			{
				json[fields[i]->name()] = pRef->GetEnum(msg, fields[i])->name();
			}
		}
#define FILLJSON(CPPTYPE,PTYPE,index) \
	if (fields[index]->type() == google::protobuf::FieldDescriptor::TYPE_##CPPTYPE){\
	if (fields[index]->is_repeated()){\
	Value subarr(arrayValue);\
	int cnt = pRef->FieldSize(msg, fields[index]);\
	for (int c = 0; c < cnt; c++)\
	subarr.append( pRef->GetRepeated##PTYPE(msg, fields[index], c)) ;\
	json[fields[index]->name()]  = subarr;\
	}\
		else\
		{\
		json[fields[index]->name()] = pRef->Get##PTYPE(msg, fields[i]);\
		}\
	}
		FILLJSON(INT32,     Int32,  i);
		FILLJSON(FLOAT,     Float,  i);
		FILLJSON(DOUBLE,    Double, i);
		FILLJSON(BOOL,      Bool,   i);
		FILLJSON(STRING,    String, i);


#undef FILLJSON
	}

}

::google::protobuf::Message*
GetMessageByPath(google::protobuf::Message* pMsg, const std::string& path, bool bWhenErrReturnParent)
{
	typedef ::google::protobuf::Message Msg;

	if (pMsg == NULL)
	{
		return NULL;
	}
	if (path == "" || path == "/" || path == "\\")
	{
		return pMsg;
	}

	size_t pos = path.find("/");
	pos = pos == path.npos ? path.find("\\") : pos;
	bool bEndOfPath = path.npos == pos;

	std::string p = bEndOfPath ? path : path.substr(0, pos);


	Msg* pResult = NULL;
	const google::protobuf::Reflection* pRef = pMsg->GetReflection();
	std::vector<const google::protobuf::FieldDescriptor*> fields;
	pRef->ListFields(*pMsg, &fields);


	for (size_t i = 0; i < fields.size(); i++)
	{
		const google::protobuf::FieldDescriptor* pField = fields[i];

		if (pField->name() == p)
		{
			if (pField->is_repeated() && ! bEndOfPath)
			{
				std::string sub = path.substr(pos + 1);
				size_t sub_pos = sub.find("/");
				sub_pos = sub_pos == sub.npos ? sub.find("\\") : sub_pos;
				if (sub_pos != sub.npos)
				{
					sub = sub.substr(0, sub_pos);
				}
				int index = 0;
				safe_atoi(sub, index);
				if (index < 0)
				{
					index = 0;
				}
				if (index < pRef->FieldSize(*pMsg, pField))
				{
					pResult = GetMessageByPath(pRef->MutableRepeatedMessage(pMsg, pField, index), path.substr(pos + sub_pos + 1 + 1), bWhenErrReturnParent);
				}
			}
			else if (bEndOfPath)
			{
				pResult =
					(pField->is_repeated() || pField->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE) ? pMsg : pRef->MutableMessage(pMsg, pField);
			}
			else
			{
				pResult = GetMessageByPath(pRef->MutableMessage(pMsg, pField), path.substr(pos + 1), bWhenErrReturnParent);
			}
		}
	}

	if (pResult == NULL && bWhenErrReturnParent)
	{
		pResult = pMsg;
	}
	return pResult;
}

Value*
JsonPathSearch(Value* pRoot, const std::string& path)
{
	//Value* pResult = NULL;
	if (pRoot == NULL)
	{
		return NULL;
	}
	if (path == "" || path == "/" || path == "\\")
	{
		return pRoot;
	}

	size_t pos = path.find("/");
	pos = pos == path.npos ? path.find("\\") : pos;
	bool bEndOfPath = path.npos == pos;

	std::string p = bEndOfPath ? path : path.substr(0, pos);

	if (pRoot->isArray())
	{
		int index  = 0;
		safe_atoi(p, index);
		if (index < 0 || index >= (int)pRoot->size())
		{
			return NULL;
		}
		return JsonPathSearch( & ((*pRoot)[index]) , bEndOfPath?"":path.substr(pos + 1));
	}
	if ((pRoot->isNull()|| pRoot->isObject()) && pRoot->isMember(p))
	{
		return JsonPathSearch( & ((*pRoot)[p]), bEndOfPath?"":path.substr(pos + 1));
	}
	return NULL;
}
