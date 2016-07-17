#include <stdlib.h>
#include "../net/NetCache.h"
#include "ClientHandler.h"
#include "GameNetHandler.h"
#include "../event/EventDefine.h"
#include "../common/string-util.h"
#include "../event/EventQueue.h"
#include "event/MessageDef.h"
#include "event/UserAuth.h"
#include "event/UserLogin.h"
#include "../common/Clock.h"
string ClientHandler::policy_content;

#pragma warning(disable : 4065)

ClientHandler::ClientHandler(GameNetHandler *nh, int fd, int nid, NetCache *cache)
{
	this->nh = nh;
	this->fd = fd;
	this->nid_ = nid;
	ClientHandler::policy_content = \
		string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
			   <cross-domain-policy>\
			   <allow-access-from domain=\"*\" to-ports=\"*\" secure=\"false\" />\
			   </cross-domain-policy>");
	logger_ = log4cxx::Logger::getLogger("ClientHandler");
}

ClientHandler::~ClientHandler(void)
{
}

int ClientHandler::handlerType()
{
	return ProtocolHandler::CLIENT;
}


#define DEFINE_AUTO_EVENT(EventID,SubEvent,SubEventFunc)\
case EventID:\
{\
	SubEvent* pSubEvn = e->SubEventFunc();\
	if (pSubEvn->ParseFromString(req)) bRight = true;\
		break;\
}

void ClientHandler::HandleAutoEvent(int64 uid,int nType,string& req)
{
	if(nType<C2S_EVENT_BASE)
		return;
	bool bRight = false;
	Event* e=nh->eq->allocateEvent();
	e->set_cmd(nType);
	e->set_state(Client_to_Game);
	e->set_time(0);
	e->set_uid(uid);

	switch(nType)
	{
#include "ClientHandler_Auto.h"
	default:
		break;
	}
	if(bRight)
		nh->eq->safePushEvent(e);	
	else
	{
		nh->eq->freeEvent(e);
		nh->closeConnection(fd);
	}
}

void ClientHandler::handle(int64 uid, string &req)
{
	if (req.substr(0, 22) == "<policy-file-request/>")
	{
		processPolicy();
	}
	else if (req.substr(0, 22) == "<policy-stat-request/>")
	{
		processState();
	}
	else if(req.c_str()[0] == 'a')
	{
		vector<string> tokens;
		static string delims = ",";
		tokenize(req, tokens, delims);
		int64 uid, secret;
		if(safe_atoll(tokens[1].c_str(), uid) &&
			safe_atoll(tokens[2].c_str(), secret))
		{
			UserAuth::addAuthEvent(nh->eq, uid, secret, fd);
		}
	}
	else if(req.substr(0,3) == "tgw")
	{
		return;
	}
	else
	{
		unsigned short nType=0;
		if(req.size()<sizeof(unsigned short))
			return;
		nType = ntohs(*((unsigned short*)req.c_str()));
		LOG4CXX_DEBUG(logger_, "Receive Command Number: "<<nType << " length:"<< req.size() << " uid:" <<uid);
		try
		{
			string subreq = req.substr(sizeof(unsigned short) + sizeof(unsigned int));
			HandleAutoEvent(uid,nType,subreq);
		}
		catch (...)
		{	
		}
	}
}

void ClientHandler::processPolicy()
{
	nh->sendFdString(fd, policy_content.c_str(), policy_content.size());
	nh->closeConnection(fd);
}

void ClientHandler::processState()
{
	nh->sendFdString(fd, "<policy-stat-response/>", 23);
	nh->closeConnection(fd);
}

void ClientHandler::leave(int64 uid)
{
	Event* e=nh->eq->allocateEvent();
	e->set_cmd(EVENT_USER_DISCONNECT);
	e->set_time(Clock::getCurrentSystemTime() + 1000);
	e->set_state(Game_to_Self);
	e->set_uid(uid);
	UserLeave* pLeave = e->mutable_userleave();
	pLeave->set_fd(fd);
	nh->eq->safePushEvent(e);	
}
