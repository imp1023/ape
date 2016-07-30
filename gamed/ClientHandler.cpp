#include <stdlib.h>
#include "../net/NetCache.h"
#include "ClientHandler.h"
#include "GameNetHandler.h"
#include "../event/EventDefine.h"
#include "../common/string-util.h"
#include "../common/json-util.h"
#include "../event/EventQueue.h"
#include "../common/Msg2QQ.h"
#include "event/MessageDef.h"
#include "event/UserAuth.h"
#include "event/UserLogin.h"
string ClientHandler::policy_content;

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
	this->m_Parent = cache;
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
	if(nType<C2S_EVENT_BASE || nType >= C2S_EVENT_MAX)
	{
		return;
	}
	bool bRight = false;
	Event* e=nh->eq->allocateEvent();
	e->set_cmd(nType);
	e->set_state(Status_Normal_Game);
	e->set_time(0);
	e->set_uid(uid);

	if(m_Parent && req.length() > 0)
	{
		//char m_charrc4[_MAX_RC4BUFF_SIZE];
		//RC4Engine m_rc4Send,m_rc4Receive;
		//m_rc4Send.Setup((const unsigned char *)req.c_str(),req.length());
		//m_rc4Receive.Setup((const unsigned char *)req.c_str(),req.length());
		//string strTest="sskylin夜下听风夜下听风夜下听风夜下听风1982";
		//
		//memset(m_charrc4,0,_MAX_RC4BUFF_SIZE);
		//m_rc4Send.Process((const unsigned char *)strTest.c_str(),(unsigned char *)m_charrc4,strTest.length());
		//string strRet(m_charrc4,strTest.length());
		//m_rc4Receive.ReadProcess(strRet);
		switch(nType)
		{
 		case C2S_RceHeartbeat:
// 		case C2S_RceInit:
// 		case C2S_RceLoad:
// 		case C2S_RceDebug:
 			break;
		default:
			{
				m_Parent->m_rc4Receive.ReadProcess(req);
			}
			break;
		}	
	}

	switch(nType)
	{
#include "ClientHandler_Auto.h"
	default:
		break;
	}
	if(bRight)
		nh->eq->safePushEvent(e);	
	else
		nh->eq->freeEvent(e);
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
		if(tokens.size() >= 3 &&
			safe_atoll(tokens[1].c_str(), uid) &&
			safe_atoll(tokens[2].c_str(), secret))
		{
			UserAuth::addEvent(nh->eq, uid, secret, fd);
		}
	}
	else if(req.substr(0,3) == "tgw")
	{
		return;
	}
#ifdef _WIN32
	else if (req.substr(0,7) == "maskrc4")
	{
		m_Parent->m_rc4Receive.ResetInitialized();
	}
#endif
	else
	{
		unsigned short nType=0;
		if(req.size()<sizeof(unsigned short))
			return;
		nType = ntohs(*((unsigned short*)req.c_str()));
		LOG4CXX_INFO(logger_, "Receive Command Number: "<<nType);
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
	e->set_cmd(EVENT_USER_LOGOUT);
	e->set_time(0);
	e->set_state(Status_Normal_Game);
	e->set_uid(uid);
	WG_UserLeave* pUserLeave = e->mutable_wg_userleave();
	pUserLeave->set_fd(fd);
	nh->eq->safePushEvent(e);
}
