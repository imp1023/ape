#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/MissionsTbl.h"
#include "RceUpdateTargetsHandle.h"


RceUpdateTargetsHandle::RceUpdateTargetsHandle()
:CBaseEvent()
{

}
RceUpdateTargetsHandle::~RceUpdateTargetsHandle()
{

}
void RceUpdateTargetsHandle::SendRet2User(User* pUser,int nRet,RseUpdateTargets& rseUpdateTargets)
{
	rseUpdateTargets.set_ret(nRet);
	string text;
	rseUpdateTargets.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateTargets,text);
}

void RceUpdateTargetsHandle::handle(Event* e)
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

	RceUpdateTargets* req = e->mutable_ce_rceupdatetargets();
	if( !req )
	{
		return;
	}

	RseUpdateTargets rse;
	rse.set_ret(0);
	string strAction = req->action();
	int plantId = 0;
	safe_atoi(req->planetid(),plantId);
	DB_Planet* pDBPlanet = pPlayer->GetPlanet(plantId);
	DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
	if(!pDBPlanet || !pDBPlayer)
	{
		return;
	}
	if(strAction == "addProgress")
	{
		DB_Mission* pDBMission = pDBPlayer->mutable_missions();
		bool bFind = false;
		for(int i = 0;i<pDBMission->params_size();i++)
		{
			DB_MissionParam* pDBMP = pDBMission->mutable_params(i);
			if(req->sku() == pDBMP->sku())
			{
				pDBMP->set_target(progressToString(pDBMP->target(),req->subtargetindex(),req->amount()));
				break;
			}
		}
		if(!bFind)
		{
			DB_MissionParam* pDBMP = pDBMission->add_params();
			pDBMP->set_sku(req->sku());
			pDBMP->set_state(2);
			CFG_Missions* Tbl = MissionsTblInst::instance().GetMission(req->sku());
			int endTM = Tbl->finishTime;
			int size = Tbl->size;
			pDBMP->set_target(progressToString(getStringBySize(size),req->subtargetindex(),req->amount()));
			if(endTM > 0)
				pDBMP->set_endtime(time(NULL) + endTM/1000);
			else
				pDBMP->set_endtime(0);
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
	}
	else
	{

	}


}
string RceUpdateTargetsHandle::progressToString(string progress, int index, int amount)
{
	int position = 1;
	for(int i = 0;i<progress.size();i++)
	{
		if(progress.at(i) == ',')
			continue;
		else
		{
			if(position == index)
			{
				int tmp =  (int)progress[i];
				tmp += amount;
				progress[i] = (char)tmp;
				break;
			}
			else
				position++;
		}
	}
	return progress;
}
string RceUpdateTargetsHandle::getStringBySize(int size)
{
	string sb = "";
	for (int i = 0; i < size; i++) {
		if (i == 0) {
			sb.append("0");
		} else {
			sb.append(",0");
		}
	}
	return sb;
}
