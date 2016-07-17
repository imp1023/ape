#define _CRT_SECURE_NO_WARNINGS

#include "GameEventHandler.h"

#include <sys/types.h>
#include "../common/SysLog.h"
#include "../common/string-util.h"
#include "event/AutoEventHead.h"
#include "../event/event.pb.h"
#include "../event/EventDefine.h"
#include "GameNetHandler.h"
#include "GameDataHandler.h"

extern int g_processingCmd;

GameEventHandler::GameEventHandler(EventQueue *eq, GameDataHandler* dh, GameNetHandler *nh, int nid)
: eq_(eq), dh_(dh), nh_(nh), nid_(nid)
{
    logger_ = log4cxx::Logger::getLogger("GameEventHandler");
    eh_ = new EventHandler(eq, dh, nh, nid);
    initialEventProcessors();
    serverIp = ServerConfig::Instance().gamedIp(nid);
    serverPort = ServerConfig::Instance().gamedPort(nid);

    dh->init();
}

GameEventHandler::~GameEventHandler(void)
{
}

void GameEventHandler::start()
{
    eh_->start();
}

void GameEventHandler::quit()
{
	if (eh_ != NULL)
	{
		eh_->quit();
	}
}

void GameEventHandler::createUserFdMap(int fd, int64 uid)
{
    nh_->addUser(fd, uid);
}

void GameEventHandler::removeUserFdMap(int fd, int64 uid)
{
	nh_->kickUser(fd, uid);
}

void GameEventHandler::sendEventToWorld(Event *e)
{
    LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to world.");
    int fd = 0;
    if (e->has_forward() && e->forward().worldfd() > 0)
    {
        fd = (int)e->forward().worldfd();
    }

    string text;
    e->SerializeToString(&text);
    text = "ev," + text;
    nh_->sendToWorld(fd, text);
}

void GameEventHandler::sendEventToCenter( Event *e, int wid )
{
	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to center.");
	string text;
	e->SerializeToString(&text);
	text = "ev," + text;
	nh_->sendToWorldByWid(wid, text);
}

bool GameEventHandler::sendDataToUser(int fd, int cmd, const string &text)
{
    LOG4CXX_DEBUG(logger_, "send to sock:" << fd << " cmd:" << cmd << " len:" << text.length());
    return nh_->sendDataToClient(fd, cmd, text);
}

void GameEventHandler::sendFdString(int fd, const char* str, size_t size)
{
    LOG4CXX_DEBUG(logger_, "send to sock:" << fd << " len:" << size);
    nh_->sendFdString(fd, str, size);
}

#include "../logic/User.h"

void GameEventHandler::SendToAllOnlineUser(int cmd, const string& text)
{
	GameDataHandler* pUserManager = getDataHandler();
	const hash_map<int64, User*>& usermap = pUserManager->getUsers();
	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter ++)
	{
		User* pUser = iter->second;
		if (pUser != NULL && pUser->Online())
		{
			sendDataToUser(pUser->fd(), cmd, text);
		}
	}
}
// void GameEventHandler::SendToOnlineCampUser(int cmd, const string& text, int camp)
// {
// 	GameDataHandler* pUserManager = getDataHandler();
// 	const hash_map<int64, User*>& usermap = pUserManager->getUsers();
// 	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter ++)
// 	{
// 		User* pUser = iter->second;
// 		if (pUser != NULL && pUser->Online() && pUser->getCamp() == camp)
// 		{
// 			sendDataToUser(pUser->fd(), cmd, text);
// 		}
// 	}
// }

void GameEventHandler::SendBroadCast( int type, const string& content )
{
	string text;
	RseBroadcast data;
	data.set_type(type);
	data.set_content(content);
	data.SerializeToString(&text);
	SendToAllOnlineUser(S2C_RseBroadcast, text);
}

void GameEventHandler::SendBroadCast( int type, int id, vector<string> param, const string& content )
{
	string text;
	RseBroadcast data;
	data.set_type(type);
	data.set_content(content);
	data.set_id(id);
	for(size_t i = 0; i < param.size(); i++)
		data.add_param(param[i]);
	data.SerializeToString(&text);
	SendToAllOnlineUser(S2C_RseBroadcast, text);
}

#include "event/UpdateWorkingStatus.h"
#include "event/GMHandler.h"
#include "event/UserLogin.h"
#include "event/UserAuth.h"
#include "event/TickEventHandler.h"

void GameEventHandler::initialEventProcessors()
{
    // add Event Processors here
    UpdateWorkingStatus::createInstance(this);
	//GMHandler::createInstance(this);
    UserLogin::createInstance(this);
    UserAuth::createInstance(this);
	TickEventHandler::createInstance(this);

    //initAutoEventProcessors函数为工具自动生成
    initAutoEventProcessors();
}
