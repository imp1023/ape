#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceUpdateGameUnitsHandle.h"
#include "../../logic/SkuIdTbl.h"


RceUpdateGameUnitsHandle::RceUpdateGameUnitsHandle()
:CBaseEvent()
{

}
RceUpdateGameUnitsHandle::~RceUpdateGameUnitsHandle()
{

}
void RceUpdateGameUnitsHandle::SendRet2User(User* pUser,int nRet,RseUpdateGameUnits& rseUpdateGameUnits)
{
	rseUpdateGameUnits.set_ret(nRet);
	string text;
	rseUpdateGameUnits.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateGameUnits,text);
}

void RceUpdateGameUnitsHandle::handle(Event* e)
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

	RceUpdateGameUnits* req = e->mutable_ce_rceupdategameunits();
	if( !req )
	{
		return;
	}

	RseUpdateGameUnits rse;
	rse.set_ret(RceUpdateGameUnitsRet_Success);

	int nPlanetId = req->planetid();
	DB_Planet *pDBPlanet = pPlayer->GetPlanet(1);
	if(!pDBPlanet){
		SendRet2User(pUser,RceUpdateGameUnitsRet_CantFindPlanet,rse);
		return;	
	}
	string strAction = req->action();
	int Sku = SkuIDTblInst::instance().GetSku(req->sku());
	if(strAction == "unlockStart")
	{	
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Mineral,MT->minerals()))
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_ResNotEnough,rse);
		}
		bool bFind = false;
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				pDBGU->set_timeleft(req->timeleft());
				pDBGU->set_updateat(time(NULL));
				pDBGU->set_upgradeid(0);
				pDBGU->set_unlock(0);
				bFind = true;
			}
		}
		if(!bFind)
		{
			DB_GameUnit* pDBGU = pDBPlanet->add_units();
			pDBGU->set_sku(Sku);
			pDBGU->set_timeleft(req->timeleft());
			pDBGU->set_updateat(time(NULL));
			pDBGU->set_upgradeid(0);
			pDBGU->set_unlock(0);
		}
	}
	else if(strAction == "unlockCancel")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Mineral,MT->minerals()))
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_ResNotEnough,rse);
		}
		bool bFind = false;
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				pDBGU->set_timeleft(-1);
				pDBGU->set_updateat(time(NULL));
			}
		}
		if(!bFind)
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_UnitNotFind,rse);
		}


	}
	else if(strAction == "unlockCompleted")
	{
		MsgTransaction* MT = req->mutable_transaction();//秒科技消耗现金
		if(!pPlayer->CostRes(RC_Cash,MT->cash()))
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_ResNotEnough,rse);
		}
		bool bFind = false;
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				pDBGU->set_timeleft(-1);
				pDBGU->set_unlock(1);
				pDBGU->set_updateat(time(NULL));
				pDBGU->set_upgradeid(0);
				bFind = true;
			}
		}
		if(!bFind)
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_UnitNotFind,rse);
		}

	}
	else if(strAction == "upgradeStart")
	{
		MsgTransaction* MT = req->mutable_transaction();//秒科技消耗现金
		if(!pPlayer->CostRes(RC_Mineral,MT->minerals()))
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_ResNotEnough,rse);
		}
		bool bFind = false;
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				pDBGU->set_timeleft(req->timeleft());
				pDBGU->set_updateat(time(NULL));
				bFind = true;
			}
		}
		if(!bFind)
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_UnitNotFind,rse);
		}
	}
	else if(strAction == "upgradeCancel")
	{
		bool bFind = false;
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				pDBGU->set_timeleft(-1);
				pDBGU->set_updateat(time(NULL));
					bFind = true;
			}
		}
		if(!bFind)
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_UnitNotFind,rse);
		}
	}
	else if(strAction == "upgradeCompleted")
	{
				bool bFind = false;
		MsgTransaction* MT = req->mutable_transaction();//秒科技消耗现金
		if(!pPlayer->CostRes(RC_Cash,MT->cash()))
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_ResNotEnough,rse);
		}
		for(int i = 0;i<pDBPlanet->units_size();i++)
		{
			DB_GameUnit* pDBGU = pDBPlanet->mutable_units(i);
			if(pDBGU->sku() == Sku)
			{
				time_t now = time(NULL);
				pDBGU->set_timeleft(-1);
				pDBGU->set_updateat(now);
				pDBGU->set_upgradeid(pDBGU->upgradeid() + 1);
				bFind = true;
			}
		}
		if(!bFind)
		{
			SendRet2User(pUser,RceUpdateGameUnitsRet_UnitNotFind,rse);
		}
	}
	eh_->getDataHandler()->markUserDirty(pUser);

}
