#ifndef _PROTOCOLHANDLER_H_
#define _PROTOCOLHANDLER_H_
#pragma once

#include <string>
#include <vector>
#include "../common/const_def.h"

using namespace std;

struct event_header;

class ProtocolHandler
{
public:
	static const int CLIENT		= 0;
	static const int ADMIN		= 1;
	static const int WEBSERVER	= 2;
	static const int POLICY		= 3;
	static const int WORLD		= 4;
	static const int GAMED		= 5;


	virtual ~ProtocolHandler() {};
	virtual void	handle(int64 uid, string &cmd) = 0;
	virtual int		handlerType() = 0;
	virtual void	leave(int64 uid);
	
protected:
	inline bool checkCmd(string &cmd, const char *cmdstr, vector<string> &tokens, int count) 
	{
		return cmd==cmdstr && tokens.size()==(unsigned int)count;
	}
	// bool parseCommandHeader(const vector<string> &tokens, event_header &eh);
};

#endif
