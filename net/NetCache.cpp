#define _CRT_SECURE_NO_WARNINGS
#include "NetCache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../net/ProtocolHandler.h"
//#include "AdminHandler.h"

//const size_t NetCache::DEFAULT_READ_SIZE = 409600;
//const size_t NetCache::INIT_WRITE_SIZE = 16384;
//const size_t NetCache::WEBSERVER_READ_SIZE = 2097152;

NetCache::NetCache(int fd, struct sockaddr_in addr, size_t rsize)
{
	rpos = 0;
	wpos = 0;
	this->fd = fd;
	this->addr = addr;
	uid = -1;
	ph = NULL;
	remove = false;
	aborted = false;
	idle = false;
	pthread_mutex_init(&write_mutex, NULL);
	logger_ = log4cxx::Logger::getLogger("NetCache");
	wsize = DEFAULT_WRITE_SIZE;
	wbuf = new (nothrow) char[DEFAULT_WRITE_SIZE];
	if (wbuf==NULL)
	{
		LOG4CXX_FATAL(logger_, "Cannot allocate memory for write buffer!!");
		exit(1);
	}
	this->rsize = rsize;
	rbuf = new (nothrow) char[rsize];
	//cmdbuf = new (nothrow) char[rsize+1];
	if (rbuf==NULL)// || cmdbuf==NULL)
	{
		LOG4CXX_FATAL(logger_, "Cannot allocate memory for read buffer!!");
		exit(1);
	}
	index_ = 0;
}

NetCache::~NetCache(void)
{
	pthread_mutex_destroy(&write_mutex);
	if (ph!=NULL)
	{
		delete ph;
	}
	if (wbuf!=NULL)
	{
		delete []wbuf;
	}
	if (rbuf!=NULL)
	{
		delete []rbuf;
	}
	//if (cmdbuf!=NULL)
	//{
	//	delete []cmdbuf;
	//}
}

char *NetCache::addrstr()
{
	return inet_ntoa(addr.sin_addr);
}

bool NetCache::read()
{
	if ((size_t)rpos+1==rsize)
	{
		LOG4CXX_ERROR(logger_, "Read cache overflow for uid=" << uid << " and fd=" << fd);
		return false;
	}
	int size = recv(fd, rbuf+rpos, rsize-rpos-1, 0);
	if (size<=0)
	{
		return false;
	}
	rpos += size;
	rbuf[rpos] = '\0';
	idle = false;
	return true;
}

bool NetCache::assemble(string &str)
{
	if (ph!=NULL)
	{
		int type = ph->handlerType();
		if (type==ProtocolHandler::POLICY)
		{
			if (rpos<2)
			{
				return false;
			}
			str = string(rbuf);
			rpos = 0;
			return true;
		}
		else if (type==ProtocolHandler::CLIENT || type==ProtocolHandler::GAMED 
				|| type==ProtocolHandler::WORLD) 
		{
			if (rpos<4)
			{
				return false;
			}

			if (rbuf[0] == '<' && rbuf[1] == 'p'
				&& rbuf[2] == 'o' && rbuf[3] == 'l'
				&& type != ProtocolHandler::WORLD) // <pol
			{
				if(rpos < 22)
				{
					return false;
				}
				if(strncmp(rbuf, "<policy-file-request/>", 22) == 0
					|| strncmp(rbuf, "<policy-stat-request/>", 22) == 0)
				{
					// 直接做明文处理
					str = string(rbuf);
					rpos = 0;
					return true;
				}
			}
			if (rpos >= 3 && rbuf[0] == 'G' && rbuf[1] == 'E' && rbuf[2] == 'T')
			{
				int newline_cnt  = 0;
				int newline_cnt2 = 0;
				int http_length = 0;
				for (int i = 0; newline_cnt < 3 && i < rpos; i++)
				{
					if (rbuf[i] == '\r')
					{
						newline_cnt2 ++;
					}
					if (rbuf[i] == '\n')
					{
						newline_cnt++;
						if (newline_cnt == 3 && newline_cnt2 == 3)
						{
							http_length = i + 1;
							break;
						}
					}
				}
				if (http_length > 0 && http_length <= rpos)
				{
					str = string(rbuf, http_length);
					//INFO("assemble, str:" << str);
					memmove(rbuf, rbuf + http_length, rpos - http_length);
					rpos = rpos - http_length;
					rbuf[rpos] = '\0';
					return true;
				}
				else
				{
					return false;
				}
			}
			if (rpos >= 4 && rbuf[0] == 't' && rbuf[1] == 'g' && rbuf[2] == 'w')
			{
				int http_length = 0;
 				for (int i = 3; http_length == 0 && i < rpos; i++)
				{
					if (rbuf[i - 3] == '\r' && rbuf[i - 2] == '\n' && rbuf[i - 1] == '\r' && rbuf[i] == '\n')
					{
						http_length = i + 1;
					}
				}
				if (http_length > 0 && http_length <= rpos)
				{
					str = string(rbuf, http_length);
					memmove(rbuf, rbuf + http_length, rpos - http_length);
					rpos = rpos - http_length;
					rbuf[rpos] = '\0';
					return true;
				}
				else
				{
					return false;
				}
			}

			int length = 0;
			int lsize = 2;
			if (type==ProtocolHandler::CLIENT)
			{
				length = ntohs(*((unsigned short*)rbuf));
			}
			else
			{
				length = *((unsigned int*)rbuf);
				lsize = sizeof(unsigned int);
			}

			if (length<=0 || length>(int)rsize || length>rpos-lsize)
			{
				if( length<=0 || length>(int)rsize )
				{
					remove = true; 
				}
				//LOG4CXX_ERROR(logger_, "Net assemble error:fd=" << fd << " uid=" << uid << " length=" << length << " rpos-lsize:"<< rpos-lsize<< " content:"<< (int)*(rbuf)<<" " << (int)*(rbuf+1)<<" "<<(int) *(rbuf+2)<<" "<< (int)*(rbuf+3));
				return false;
			}
			//memcpy(cmdbuf, rbuf+lsize, length);
			str = string(rbuf+lsize, length);
			//cmdbuf[length] = '\0';
			memmove(rbuf, rbuf+length+lsize, rpos-length-lsize);
			rpos -= length+lsize;
			rbuf[rpos] = '\0';
			//str = string(cmdbuf, length);
			return true;
		}
		else if (type==ProtocolHandler::ADMIN || type==ProtocolHandler::WEBSERVER)
		{
			// admin client or web server
			int len = 0;
			if (type==ProtocolHandler::ADMIN && rbuf[0]==(char)255)
			{
				// telnet command
				len = 3;
			}
			else
			{
				char *p = strchr(rbuf, '\n'), *q = strchr(rbuf, '\r');
				if (p==NULL&&q==NULL)
				{
					return false;
				}
				len = max(p,q)-rbuf+1;
			}
			//strncpy(cmdbuf, rbuf, len);
			//cmdbuf[len] = '\0';
			int n = len-1;
			//while (n>=0 && (cmdbuf[n]=='\n'||cmdbuf[n]=='\r'))
			//{
			//	cmdbuf[n] = '\0';
			//	n--;
			//}
			int nStrLen = len;
			while (n>=0 && (rbuf[n]=='\n'||rbuf[n]=='\r'))
			{
				nStrLen = n;
				n--;
			}
			str = string(rbuf,nStrLen);
			memmove(rbuf, rbuf+len, rpos-len);
			rpos -= len;
			rbuf[rpos] = '\0';
			//str = string(cmdbuf);
			return true;
		}
	}
	else // ph==NULL
	{
		LOG4CXX_ERROR(logger_, "Protocol handler is NULL for fd:"<<fd);
	}
	return false;
}

bool NetCache::waitToWrite()
{
	return wpos>0;
}

bool NetCache::prepareWrite(const char *str, size_t size)
{
	lockWrite();
	if (size>wsize-1-wpos) // buffer overflow, allocate new buffer
	{
		size_t nwsize = max(wsize + DEFAULT_WRITE_SIZE , wpos + size + 1024);
		size_t nwsize_mb = nwsize>>20;
		if (nwsize_mb >= 100) {
			LOG4CXX_INFO(logger_, "Resizing write buffer for fd " << fd << " to " << nwsize_mb << " MB.");
		} else {
			LOG4CXX_DEBUG(logger_, "Resizing write buffer for fd " << fd << " to " << nwsize_mb << " MB.");
		}
		char *nwbuf = new (nothrow) char[nwsize];
		if (nwbuf==NULL)
		{
			LOG4CXX_ERROR(logger_, "Cannot allocate memory for write buffer!!");
			unlockWrite();
			return false;
		}
		else
		{
			if (wpos>0)
			{
				memcpy(nwbuf, wbuf, wpos);
			}
			delete[] wbuf;
			wsize = nwsize;
			wbuf = nwbuf;
		}
	}
	memcpy(wbuf+wpos, str, size);
	wpos += size;
	unlockWrite();
	return true;
}

#ifdef _WIN32

bool NetCache::write(bool block) {
	return true;
}

#else

bool NetCache::write(bool block)
{
	lockWrite();
	if (wpos==0)
	{
		unlockWrite();
		return true;
	}
	int size = send(fd, wbuf, wpos, block ? 0 : MSG_DONTWAIT);
	if (size<0)
	{
		unlockWrite();
		LOG4CXX_ERROR(logger_, "net send failed!");
		return false;
	}
	else
	{
		memmove(wbuf, wbuf+size, wpos-size);
		wpos -= size;
		bool finished = (wpos==0);
		unlockWrite();
		if (!finished) {
			LOG4CXX_DEBUG(logger_, "send, but not finished, wpos=" << wpos);
		}
		return finished;
	}
}

#endif
