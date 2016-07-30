#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/MissionsTbl.h"
#include "RceUpdateMissionsHandle.h"


RceUpdateMissionsHandle::RceUpdateMissionsHandle()
:CBaseEvent()
{

}
RceUpdateMissionsHandle::~RceUpdateMissionsHandle()
{

}
void RceUpdateMissionsHandle::SendRet2User(User* pUser,int nRet,RseUpdateMissions& rseUpdateMissions)
{
	rseUpdateMissions.set_ret(nRet);
	string text;
	rseUpdateMissions.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateMissions,text);
}

void RceUpdateMissionsHandle::handle(Event* e)
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

	RceUpdateMissions* req = e->mutable_ce_rceupdatemissions();
	if( !req )
	{
		return;
	}

	RseUpdateMissions rse;
	rse.set_ret(0);
	string strAction = req->acton();
	int plantId = 0;
	safe_atoi(req->planetid(),plantId);
	DB_Planet* pDBPlanet = pPlayer->GetPlanet(plantId);
	DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
	if(!pDBPlanet || !pDBPlayer)
	{
		return;
	}
	if(strAction == "newState")
	{
		DB_Mission* pDBMission = pDBPlayer->mutable_missions();
		DB_MissionParam* pDBMissionParam = NULL;
		for(int i = 0;i<pDBMission->params_size();i++)
		{
			pDBMissionParam = pDBMission->mutable_params(i);
			if(pDBMissionParam->sku() == req->sku())
			{
				break;
			}
		}
		switch (req->state())
		{
		case 1://ready to start
			{
				if(!pDBMissionParam)
				{
					pDBMissionParam = pDBMission->add_params();
				}
				string strMission = "";
				if(pDBMission->readytostart() == "")
				{
					strMission = req->sku();
				}
				else
				{
					strMission = pDBMission->readytostart() + "," + req->sku();
				}
				pDBMission->set_readytostart(strMission);
				pDBMissionParam->set_sku(req->sku());
				pDBMissionParam->set_state(req->state());
				CFG_Missions* Tbl = MissionsTblInst::instance().GetMission(req->sku());
				int endTM = Tbl->finishTime;
				int size = Tbl->size;
				pDBMissionParam->set_target(getStringBySize(size));
				if(endTM > 0)
					pDBMissionParam->set_endtime(time(NULL) + endTM/1000);
				else
					pDBMissionParam->set_endtime(0);
			}
			
			break;
		case 2://available
			{
				if(!pDBMissionParam)
				{
					pDBMissionParam = pDBMission->add_params();
				}
				string strMission = "";
				if(pDBMission->available() == "")
				{
					strMission = req->sku();
				}
				else
				{
					strMission = pDBMission->available() + "," + req->sku();
				}
				pDBMission->set_available(strMission);
				string findStr = req->sku();
				string readyToStart = pDBMission->readytostart();
				string ::size_type position;
				position = readyToStart.find(findStr);
				if(position != readyToStart.npos)//找到了
				{
					if(readyToStart[position+findStr.size()] == ',')
						readyToStart.erase(position,findStr.size()+1);
					else
						readyToStart.erase(position,findStr.size());
					if(readyToStart.size() > 0 && readyToStart[readyToStart.size() - 1] == ',')
					{
						readyToStart.erase(readyToStart.size() - 1,1);
					}
				}
				pDBMission->set_readytostart(readyToStart);

				pDBMissionParam->set_sku(req->sku());
				pDBMissionParam->set_state(req->state());
				CFG_Missions* Tbl = MissionsTblInst::instance().GetMission(req->sku());
				int endTM = Tbl->finishTime;
				int size = Tbl->size;
				pDBMissionParam->set_target(getStringBySize(size));
				if(endTM > 0)
					pDBMissionParam->set_endtime(time(NULL) + endTM/1000);
				else
					pDBMissionParam->set_endtime(0);
			}
			
			break;
		case 3://rewarded
			{
				MsgTransaction* MT = req->mutable_transaction();
				if (!pPlayer->CostRes(RC_Cash, MT->cash()) ||
					!pPlayer->CostRes(RC_Coin, MT->coins()) ||
					!pPlayer->CostRes(RC_Mineral, MT->minerals()) ||
					!pPlayer->CostRes(RC_Exp, MT->exp()) ||
					!pPlayer->CostRes(RC_Score, MT->score()))
				{
					SendRet2User(pUser,1,rse);
					return;	
				}
				string strMission = "";
				if(pDBMission->rewarded() == "")
				{
					strMission = req->sku();
				}
				else
				{
					strMission = pDBMission->rewarded() + "," + req->sku();
				}
				pDBMission->set_rewarded(strMission);

				string findStr = req->sku();
				string available = pDBMission->available();
				string ::size_type position;
				position = available.find(findStr);
				if(position != available.npos)//找到了
				{
					if(available[position+findStr.size()] == ',')
						available.erase(position,findStr.size()+1);
					else
						available.erase(position,findStr.size());
					if(available.size() > 0 && available[available.size() - 1] == ',')
					{
						available.erase(available.size() - 1,1);
					}
				}
				
				pDBMission->set_available(available);

				if(!pDBMissionParam)
				{
					pDBMissionParam = pDBMission->add_params();
				}
				pDBMissionParam->set_sku(req->sku());
				pDBMissionParam->set_state(req->state());
			}
			break;
		case 4://completed
			{
				string findStr = req->sku();
				string rewarded = pDBMission->rewarded();
				string ::size_type position;
				position = rewarded.find(findStr);
				if(position != rewarded.npos)//找到了
				{
					if(rewarded[position+findStr.size()] == ',')
						rewarded.erase(position,findStr.size()+1);
					else
						rewarded.erase(position,findStr.size());
					if(rewarded.size() > 0 && rewarded[rewarded.size() - 1] == ',')
					{
						rewarded.erase(rewarded.size() - 1,1);
					}
				}

				pDBMission->set_rewarded(rewarded);
				if(!pDBMissionParam)
				{
					pDBMissionParam = pDBMission->add_params();
					pDBMissionParam->set_sku(req->sku());
				}
				string strMission = "";
				if(pDBMission->completed() == "")
				{
					strMission = req->sku();
				}
				else
				{
					strMission = pDBMission->completed() + "," + req->sku();
				}
				pDBMission->set_completed(strMission);
				pDBMissionParam->set_state(req->state());
			}
			
			break;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);

	}
}

string RceUpdateMissionsHandle::progressToString(string progress, int index, int amount)
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
string RceUpdateMissionsHandle::getStringBySize(int size)
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
