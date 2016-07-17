#ifndef _POLICYHANDLER_H_
#define _POLICYHANDLER_H_

#pragma once

#include <string>
#include <vector>
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"

using namespace std;

class GameNetHandler;

class PolicyHandler : public ProtocolHandler
{
public:
	PolicyHandler(GameNetHandler *nh, int fd);
	~PolicyHandler(void);
	void handle(int64 uid, string &req);
	int handlerType();
	void leave(int64 uid);


private:
	GameNetHandler *nh;
	int fd;
	static string policy_content;

	void processPolicy();
};

#endif
