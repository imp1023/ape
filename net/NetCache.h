#ifndef _NETCACHE_H_
#define _NETCACHE_H_
#pragma once

#ifdef _WIN32
	#include <WinSock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif
#include <log4cxx/logger.h>

#include <pthread.h>
#include <string>
#include "../common/const_def.h"
#include "RC4Engine.h"
using namespace std;

#define DEFAULT_CLIENT_SIZE	204800
#define DEFAULT_SERVER_SIZE 2097152
#define DEFAULT_WRITE_SIZE	204800

class ProtocolHandler;

class NetCache
{
public:
	int		fd;
	int64	uid;
	bool	remove;
	bool	aborted; // remote disconnected
	bool	idle; // if idle, should be kicked out
	ProtocolHandler *ph;
	//static const size_t DEFAULT_READ_SIZE;// = 4096;
	//static const size_t INIT_WRITE_SIZE;// = 16384;
	//static const size_t WEBSERVER_READ_SIZE;// = 131072;

	RC4Engine m_rc4Send,m_rc4Receive;

	NetCache(int fd, struct sockaddr_in addr, size_t rsize);
	~NetCache(void);

	bool read(void);
	bool write(bool block=false);
	bool prepareWrite(const char *str, size_t size);
	bool assemble(string &str);
	char *addrstr();
	bool waitToWrite();

	inline int getNextIndex()
	{
		return ++index_;
	}

private:
	char *rbuf;
	//char *cmdbuf;
	char *wbuf;
	size_t wsize; // write buffer size
	size_t rsize; // read buffer size
	struct sockaddr_in addr;
	int rpos, wpos;
	pthread_mutex_t write_mutex;
	log4cxx::LoggerPtr logger_;
	int index_;

	inline void lockWrite()
	{
		pthread_mutex_lock(&write_mutex);
	}
	inline void unlockWrite()
	{
		pthread_mutex_unlock(&write_mutex);
	}	
};

#endif
