#ifndef _WEBSERVERHANDLER_H_
#define _WEBSERVERHANDLER_H_

#pragma once

#include <string>
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
    // command parsers
    void processUserLogin(vector<string> &tokens);
	
private:
    EventQueue *eq_;
    int fd_;
    int nid_;
private:
    static const int USER_LOGIN_ARG_NUM = 16;
    static const int INVITE_ARG_NUM = 7;
	static const int URL_INVITE_ARG_NUM = 7;
    static const int BUY_ARG_NUM = 8;
    static const int CHECK_BANDCOIN_NUM = 3;
    static const int LENGTHEN_ARG_NUM = 8;
    static const int WEB_BET_ARG_NUM = 16;
} ;

#endif
