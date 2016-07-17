#ifndef _WEBSERVERHANDLER_H_
#define _WEBSERVERHANDLER_H_

#pragma once

#include <string>
#include <log4cxx/logger.h>
#include "../net/ProtocolHandler.h"
#include "../common/const_def.h"
using namespace std;

class EventQueue;

class WebServerHandler : public ProtocolHandler
{
public:
    WebServerHandler(EventQueue *eq, int fd, int nid);
    ~WebServerHandler(void);

    void handle(int64 uid, string &req);
    int handlerType();

protected:
    int getServerId(const string& query);
	
private:
    EventQueue *eq_;
    int fd_;
    int nid_;
    log4cxx::LoggerPtr logger_;
} ;

#endif
