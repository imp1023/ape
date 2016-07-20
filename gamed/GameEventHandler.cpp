#define _CRT_SECURE_NO_WARNINGS
#include "../event/EventDefine.h"
#include "../event/event.pb.h"
#include "GameNetHandler.h"
#include "GameEventHandler.h"
#include "GameDataHandler.h"
#include "EventQueue.h"

#include "event/AutoEventHead.h"
#include "../common/SysLog.h"
#include "../logic/User.h"
#include "../logic/Player.h"

#include "event/UpdateWorkingStatus.h"
#include "event/UserLogin.h"
#include "event/UserAuth.h"

#include <sys/types.h>

extern int g_processingCmd;

const int _EVERYTIME_SEND_COUNT = 50;

void GameEventHandler::initialEventProcessors()
{
	// add Event Processors here
	UpdateWorkingStatus::createInstance(this);
	UserLogin::createInstance(this);
	UserAuth::createInstance(this);
	//initAutoEventProcessors函数为工具自动生成
	initAutoEventProcessors();
}

GameEventHandler::GameEventHandler(EventQueue *eq, GameDataHandler* dh, GameNetHandler *nh, int nid)
: eq_(eq), dh_(dh), nh_(nh), nid_(nid)
{
    logger_ = log4cxx::Logger::getLogger("GameEventHandler");
    eh_ = new EventHandler(eq, dh, nh, nid);
    initialEventProcessors();
    serverIp = serverConfig.gamedIp(nid);
    serverPort = serverConfig.gamedPort(nid);
	serverPort1 = serverConfig.gamedPort1(nid);

	char name[128];
	sprintf(name, "game%d_ec.txt", nid);
	eh_->m_xEventCounter.Open(name);
	eh_->m_xEventCounter.m_nWriteMod = 100;
}

GameEventHandler::~GameEventHandler(void)
{

}

void GameEventHandler::start()
{
    eh_->start(Srv_Game,1000);
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
	Event* e=getEventQueue()->allocateEvent();
	e->set_cmd(EVENT_USER_LOGOUT);
	e->set_time(0);
	e->set_state(Status_Normal_Game);
	e->set_uid(uid);
	WG_UserLeave* pUserLeave = e->mutable_wg_userleave();
	pUserLeave->set_fd(fd);
	getEventQueue()->safePushEvent(e);

	nh_->kickUser(fd, uid);
}

int GameEventHandler::GetPlatSrvID(const string& pid)
{
	int hash = getPlatidHash(pid);
	return (hash % serverConfig.plat_num() + 1);
}

int GameEventHandler::GetFightSrvID(const string& pid)
{
	int hash = getPlatidHash(pid);
	return (hash % serverConfig.fightd_num() + 1);
}

int	 GameEventHandler::GetFightSrvID(int64 uid)
{
	int hash = getUidHash(uid);
	return (hash % serverConfig.fightd_num() + 1);
}

void GameEventHandler::SendUserInfo(User* pUser)
{
	if(pUser==NULL)
		return;
	string text;
	//RseUserInfo rseInfo;
	//pUser->FillUserInfo(rseInfo);
	//rseInfo.SerializeToString(&text);
	//sendDataToUser(pUser->fd(), S2C_RseUserInfo, text);
}

void GameEventHandler::sendEventToPlat(Event *e,int nPlatID)
{
	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to plat.");
	e->set_platsrvid(nPlatID);

	string text;
	e->SerializeToString(&text);
	text = "ev," + text;
	nh_->sendToPlat(nPlatID, text);
}

void GameEventHandler::sendEventToAllPlat(Event* e)
{
	for (int i=0;i<serverConfig.plat_num();i++)
	{
		sendEventToPlat(e,i+1);
	}
}

void GameEventHandler::sendEventToWorld(Event *e)
{
    LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to world.");
    //int fd = 0;
    //if (e->has_fromworld_fd() && e->fromworld_fd() > 0)
    //{
    //    fd = (int) e->fromworld_fd();
    //}
	int nWorldSrvID = e->worldsrvid();
    string text;
    e->SerializeToString(&text);
    text = "ev," + text;
   // nh_->sendToWorld(fd, text);
	nh_->sendToWorld(nWorldSrvID,text);
}

bool GameEventHandler::sendDataToUser(int fd, int cmd, const string &text,bool bUseRc4/* =true */)
{
    LOG4CXX_DEBUG(logger_, "send to sock:" << fd << " cmd:" << cmd << " len:" << text.length());
    return nh_->sendDataToClient(fd, cmd, text,bUseRc4);
}

void GameEventHandler::sendFdString(int fd, const char* str, size_t size)
{
    LOG4CXX_DEBUG(logger_, "send to sock:" << fd << " len:" << size);
    nh_->sendFdString(fd, str, size);
}

void GameEventHandler::sendToGlobalUser(int64 uid, int cmd, const string &text)
{
    Event e;
    e.set_cmd(EVENT_SEND_REMOTE_USER);
    e.set_state(SendRemoteUser_GW_Req);
    e.set_time(0);
    e.set_uid(uid);
    SendRemoteUser_Req* req = e.mutable_sendremoteuser_req();
    req->set_cmd(cmd);
    req->set_text(base64_encode(text));
    req->set_uid(uid);
    sendEventToWorld(&e);
}

void GameEventHandler::SendToAllOnlineUser(int cmd, const string& text,int nRegion,bool bUseRc4/* =true */)
{
	if(nRegion>=0)
	{
		nRegion = serverConfig.getmapregion(nRegion);
	}
	GameDataHandler* pUserManager = getDataHandler();
	const hash_map<int64, User*>& usermap = pUserManager->getUsers();
	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter ++)
	{
		User* pUser = iter->second;
		if (pUser != NULL && pUser->Online())
		{
			if(nRegion>=0&&pUser->GetUserRegion(true)!=nRegion)
				continue;
			sendDataToUser(pUser->fd(), cmd, text,bUseRc4);
		}
	}
}

// void GameEventHandler::sendEventToStar(Event* e,int nStarID)
// {
// 	e->set_starsrvid(nStarID);
// 
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to hall.");
// 	//int nHallSrvID = e->starsrvid();
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToStar(nStarID, text);
// }
// 
// void GameEventHandler::sendEventToAllStar(Event* e)
// {
// 	for (int i=0;i<serverConfig.star_num();i++)
// 	{
// 		sendEventToStar(e,i+1);
// 	}
// }

// void GameEventHandler::sendEventToFight(Event *e,int nFightID)
// {
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to fight.");
// 	e->set_fightsrvid(nFightID);
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToFight(nFightID, text);
// }
// 
// void GameEventHandler::sendEventToAllFight(Event* e)
// {
// 	for (int i=0;i<serverConfig.fightd_num();i++)
// 	{
// 		sendEventToFight(e,i+1);
// 	}
// }

// void GameEventHandler::sendEventToCountry(Event* e,int nRegion)
// {
// 	int nNum = serverConfig.country_num();
// 	if(nNum<=0)
// 		return;
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to country.");
// 	int nSrvID = nRegion%nNum+1;
// 	GCG_CountryNeed* pArgs = e->mutable_countryneed();
// 	pArgs->set_countrysrvid(nSrvID);
// 	pArgs->set_gamesrvid(nid_);
// 	pArgs->set_region(nRegion);
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToCountry(nSrvID, text);
// }
// 
// void GameEventHandler::sendEventToAllCountry(Event* e)
// {
// 	for (int i=0;i<serverConfig.country_num();i++)
// 	{
// 		LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to country.");
// 		
// 		int nSrvID = i+1;
// 		GCG_CountryNeed* pArgs = e->mutable_countryneed();
// 		pArgs->set_countrysrvid(nSrvID);
// 		pArgs->set_gamesrvid(nid_);
// 		pArgs->set_region(-1);
// 
// 		string text;
// 		e->SerializeToString(&text);
// 		text = "ev," + text;
// 		nh_->sendToCountry(nSrvID, text);
// 	}
// }
// 
// void GameEventHandler::sendEventToRegion(Event* e,int nRegion)
// {
// 	int nNum = serverConfig.regionsrv_num();
// 	if(nNum<=0)
// 		return;
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to region.");
// 
// 	int nSrvID = nRegion%nNum+1;
// 	GRG_RegionNeed* pArgs = e->mutable_regionneed();
// 	pArgs->set_regionsrvid(nSrvID);
// 	pArgs->set_gamesrvid(nid_);
// 	pArgs->set_region(nRegion);
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToRegion(nSrvID, text);
// }
// 
// void GameEventHandler::sendEventToAllRegion(Event* e)
// {
// 	for (int i=0;i<serverConfig.regionsrv_num();i++)
// 	{
// 		LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to region.");
// 
// 		int nSrvID = i+1;
// 
// 		GRG_RegionNeed* pArgs = e->mutable_regionneed();
// 		pArgs->set_regionsrvid(nSrvID);
// 		pArgs->set_gamesrvid(nid_);
// 		pArgs->set_region(-1);
// 
// 		string text;
// 		e->SerializeToString(&text);
// 		text = "ev," + text;
// 		nh_->sendToRegion(nSrvID, text);
// 	}
// }
// void GameEventHandler::sendEventToRank(Event* e,int nRegion)
// {
// 	int nNum = serverConfig.ranksrv_num();
// 	if(nNum<=0)
// 		return;
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to rank.");
// 
// 	int nSrvID = nRegion%nNum+1;
// 	GRG_RankNeed* pArgs = e->mutable_rankneed();
// 	pArgs->set_ranksrvid(nSrvID);
// 	pArgs->set_gamesrvid(nid_);
// 	pArgs->set_region(nRegion);
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToRank(nSrvID, text);
// }
// 
// void GameEventHandler::sendEventToAllRank(Event* e)
// {
// 	for (int i=0;i<serverConfig.ranksrv_num();i++)
// 	{
// 		LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to rank.");
// 
// 		int nSrvID = i+1;
// 
// 		GRG_RankNeed* pArgs = e->mutable_rankneed();
// 		pArgs->set_ranksrvid(nSrvID);
// 		pArgs->set_gamesrvid(nid_);
// 		pArgs->set_region(-1);
// 
// 		string text;
// 		e->SerializeToString(&text);
// 		text = "ev," + text;
// 		nh_->sendToRank(nSrvID, text);
// 	}
// }
// 
// void GameEventHandler::sendEventToStat(Event* e,int nRegion)
// {
// 	int nNum = serverConfig.statsrv_num();
// 	if(nNum<=0)
// 		return;
// 	LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to stat.");
// 
// 	int nSrvID = nRegion%nNum+1;
// 	GSG_StatNeed* pArgs = e->mutable_statneed();
// 	pArgs->set_statsrvid(nSrvID);
// 	pArgs->set_gamesrvid(nid_);
// 	pArgs->set_region(nRegion);
// 
// 	string text;
// 	e->SerializeToString(&text);
// 	text = "ev," + text;
// 	nh_->sendToStat(nSrvID, text);
// }
// 
// void GameEventHandler::sendEventToAllStat(Event* e)
// {
// 	for (int i=0;i<serverConfig.statsrv_num();i++)
// 	{
// 		LOG4CXX_DEBUG(logger_, "send event " << e->cmd() << ", state " << e->state() << " to stat.");
// 
// 		int nSrvID = i+1;
// 
// 		GSG_StatNeed* pArgs = e->mutable_statneed();
// 		pArgs->set_statsrvid(nSrvID);
// 		pArgs->set_gamesrvid(nid_);
// 		pArgs->set_region(-1);
// 
// 		string text;
// 		e->SerializeToString(&text);
// 		text = "ev," + text;
// 		nh_->sendToStat(nSrvID, text);
// 	}
// }
// 
// void GameEventHandler::SendCountryNumQuery()
// {
// 	Event evn;
// 	evn.set_uid(0);
// 	evn.set_cmd(EVENT_COUNTRY_LITE);
// 	evn.set_time(0);
// 	evn.set_state(Status_Game_To_Country);
// 	
// 	sendEventToAllCountry(&evn);
// }

void GameEventHandler::SendToAllOnLineUserByCountry(int cmd, const string& text, int nCountry1, int nCountry2,int nRegion,bool bUseRc4/* =true */)
{
	if(nRegion>=0)
	{
		nRegion = serverConfig.getmapregion(nRegion);
	}
	GameDataHandler* pUserManager = getDataHandler();
	const hash_map<int64, User*>& usermap = pUserManager->getUsers();
	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter ++)
	{
		User* pUser = iter->second;
		if (pUser != NULL && pUser->Online())
		{
			if(nRegion>=0&&pUser->GetUserRegion(true)!=nRegion)
				continue;
			if(pUser->GetPlayer()==NULL)
				continue;
			int nUserCountry = 0;//pUser->GetPlayer()->GetCountryManager()->GetDBCountry()->countryid();
			if(nUserCountry == nCountry1 || nUserCountry == nCountry2)
			{
				sendDataToUser(pUser->fd(), cmd, text,bUseRc4);
			}
		}
	}
}

void GameEventHandler::SendToAllOnlineUserByLevel(int cmd, const string& text, int iMinLvl, int iMaxLvl,int nRegion,bool bUseRc4/* =true */)
{
	if(nRegion>=0)
	{
		nRegion = serverConfig.getmapregion(nRegion);
	}
	GameDataHandler* pUserManager = getDataHandler();
	const hash_map<int64, User*>& usermap = pUserManager->getUsers();
	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter ++)
	{
		User* pUser = iter->second;
		if (pUser != NULL && pUser->Online())
		{
			if(nRegion>=0&&pUser->GetUserRegion(true)!=nRegion)
				continue;
			int iLvl = pUser->GetUserLevel();
			if ( iLvl >= iMinLvl && iLvl <= iMaxLvl )
			{
				sendDataToUser(pUser->fd(), cmd, text,bUseRc4);
			}
			else if ( iMinLvl == -1 && iMaxLvl == -1 )
			{
				sendDataToUser(pUser->fd(), cmd, text,bUseRc4);
			}
		}
	}
}
