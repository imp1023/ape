#define _CRT_SECURE_NO_WARNINGS
#include "AdminHandler.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../common/string-util.h"
#include "GameNetHandler.h"
#include "../net/NetCache.h"
#include "../event/EventQueue.h"

using namespace std;

AdminHandler::AdminHandler(GameNetHandler *nh, int fd, int nid)
{
	this->nh = nh;
	this->fd = fd;
	this->nid_ = nid;
	welcome();
}

AdminHandler::~AdminHandler(void)
{
}

int AdminHandler::handlerType()
{
	return ProtocolHandler::ADMIN;
}

void AdminHandler::print(bool prompt, const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	if(prompt)
	{
		strcat(buf, "\r\nEmpire# ");
	}

	nh->sendFdString(fd, buf, strlen(buf));
}

void AdminHandler::welcome()
{
	print(true, "Welcome!\r\n");
}

void AdminHandler::invalid()
{
	print(true, "Invalid command!\r\n");
}

void AdminHandler::handle(int64 uid, string &req)
{
	if (req[0]==(char)255) // process telnet commands
	{
		if (req[1]==(char)0xfb && req[2]==(char)0x22) // WILL LINEMODE
		{
			req[1] = (char)0xfd;
			print(true, req.c_str());
		}
	}
	else
	{
		vector<string> tokens;
		static string delims = ",";
		tokenize(req, tokens, delims);
		if (tokens.size()<1)
		{
			invalid();
			return;
		}
		string cmd = tokens[0];
		tokens.erase(tokens.begin());

        //handle by type
	}
}
