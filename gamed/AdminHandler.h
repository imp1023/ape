#ifndef _ADMINHANDLER_H_
#define _ADMINHANDLER_H_
#pragma once

#include <string>
#include "../net/ProtocolHandler.h"
using namespace std;

class GameNetHandler;


class AdminHandler : public ProtocolHandler
{
public:
	AdminHandler(GameNetHandler *nh, int fd, int nid);
	~AdminHandler(void);

	void handle(int64 uid, string &req);
	int handlerType();

private:
	GameNetHandler *nh;
	int fd;
	int nid_;

	void welcome();
	void invalid();
	void print(bool prompt, const char *format, ...);
};

#endif
