#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_
#pragma once

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"

using namespace std;

class GameNetHandler;

class ClientHandler : public ProtocolHandler
{
public:
	ClientHandler(GameNetHandler *nh, int fd, int nid_, NetCache *cache);
	~ClientHandler(void);
	void	handle(int64 uid, string &req);
	void	HandleAutoEvent(int64 uid,int nType,string& req);
	int		handlerType();
	void	leave(int64 uid);

private:
	void	processPolicy();
	void	processState();

private:
	GameNetHandler *nh;
	int				fd;
	int				nid_;
	static string policy_content;
	log4cxx::LoggerPtr logger_;
};

#endif
