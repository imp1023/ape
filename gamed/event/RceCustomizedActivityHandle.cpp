#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceCustomizedActivityHandle.h"


RceCustomizedActivityHandle::RceCustomizedActivityHandle()
:CBaseEvent()
{

}
RceCustomizedActivityHandle::~RceCustomizedActivityHandle()
{

}

void RceCustomizedActivityHandle::SendRet2User(User* pUser,int nRet,RseCustomizedActivity& rseCustomizedActivity)
{
	//rseCustomizedActivity.set_ret(nRet);
	string text;
	rseCustomizedActivity.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseCustomizedActivity,text);
}

void RceCustomizedActivityHandle::handle(Event* e)
{
	int64 uid = e->uid();
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}
	User *pUser = pUserManager->getUser(uid);
	if ( !pUser)
	{
		return;
	}

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer || !pPlayer->CanUse())
	{
		return ;
	}

	RceCustomizedActivity* req = e->mutable_ce_rcecustomizedactivity();
	if( !req )
	{
		return;
	}

	RseCustomizedActivity rse;
	//rse.set_ret(0);
	string strAction = req->action();
	int plantId = 0;
	safe_atoi(req->planetid(),plantId);
	DB_Planet* pDBPlanet = pPlayer->GetPlanet(plantId);
	DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
	if(!pDBPlanet || !pDBPlayer)
	{
		return;
	}
	if(strAction == "dailyBonus")//每日奖励
	{
		//int nday = req->nday();
		//int errorType = 0;// 1:领取失败 2:已经领取过

	}
	else if(strAction == "callOldFriend")
	{

	}
	else if(strAction == "leaveMessage")
	{

	}
	else if(strAction == "getLeaveMessage")
	{

	}
	else if(strAction == "removeLeaveMessage")
	{

	}
	else if(strAction == "initCA")
	{

	}
	else if(strAction == "getFriendArmy")
	{

	}
	else if(strAction == "firewireSupportProgress")
	{

	}
	else if(strAction == "firewireSupportReward")
	{

	}
	else if(strAction == "halloweenReward")
	{

	}
	else if(strAction == "getReward")
	{

	}
	else
	{

	}
}
