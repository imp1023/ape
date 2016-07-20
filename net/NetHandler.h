#ifndef _NETHANDLER_H_
#define _NETHANDLER_H_

#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#endif
#include <log4cxx/logger.h>

#include <map>
#include <string>
#include <pthread.h>
#include "../common/const_def.h"

using namespace std;

class NetCache;

class NetHandler
{
public:
	NetCache *getCacheFromFd(int fd);
	void closeConnection(int fd);
	void start();
	void quit();

	bool sendFdString(int fd, const char* str, size_t size);
	bool sendSizedFdString(int fd, const string& str);
	bool sendIntSizedFdString(int fd, const string& str);
	virtual bool sendSizedTypeFdString(int fd, int type, const string& str){return false;}

	virtual bool SetRc4Keys(int fd,string& strSend,string& strReceive){return true;}

protected:
	NetHandler();
	virtual ~NetHandler();

	static const int MAX_CONNECTIONS = 100000;
	static const int MAX_EVENTS = 256;
	static const int MAX_QUEUE_LENGTH = 32;
	static const unsigned long ZLIB_BUFFER_SIZE = 262144;

	log4cxx::LoggerPtr logger_;
	map<int,NetCache*> fdCache;

	/*
	 * epfd is for Linux, master is for Windows
	 */
	int createListenSocket(string socket_name,string port, string address);
	int createConnectSocket(string socket_name,string address,string port,struct addrinfo &sa);

	NetCache *addConnection(int fd, struct sockaddr_in addr, size_t rsize);
	char *getRemoteAddress(int fd);
	void doCloseConnection(int fd);

	virtual void removeConnection(int fd);
	virtual int initSockets() = 0;
	virtual bool isListenSocket(int fd) = 0;
	virtual bool isConnectSocket(int fd) = 0;
	virtual int readCacheSize(int listenFd) = 0;
	virtual void createProtocolHandler(NetCache *cache, int listenFd) = 0;
	virtual int connectFailed(int connectFd) = 0;
	virtual bool preNetEvent(time_t now);
	virtual bool isConnecting(int fd) = 0; 
	virtual bool connectSuccess(int fd) = 0;

protected:
	pthread_mutex_t send_mutex;
	pthread_mutex_t map_mutex;
	fd_set	master;
	int		fdmax;
	int		epfd;
	bool	m_bRunning;

	inline void lockSend() { pthread_mutex_lock(&send_mutex); }
	inline void unlockSend() { pthread_mutex_unlock(&send_mutex); }
	inline void lockMap() { pthread_mutex_lock(&map_mutex); }
	inline void unlockMap() { pthread_mutex_unlock(&map_mutex); }
};

#endif
