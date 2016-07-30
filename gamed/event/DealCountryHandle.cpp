#include "MessageDef.h"
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../common/SysLog.h"
#include "DealCountryHandle.h"
#include "../../event/EventQueue.h"
#include "../../logic/GameConstantSetCfg.h"
#include <math.h>
#include "../event/RseQueryStarInfo.pb.h"

DealCountryHandle* DealCountryHandle::instance_ = NULL;

void DealCountryHandle::createInstance(GameEventHandler* eh)
{
	instance_ = new DealCountryHandle();
	instance_->eh_ = eh;
	eh->getEventHandler()->registHandler(EVENT_COUNTRY_LITE, (ProcessRoutine) DealCountryHandle::handle_);
	eh->getEventHandler()->registHandler(C2S_RceQueryStarInfo, (ProcessRoutine) DealCountryHandle::handle_);
}

void DealCountryHandle::handle(Event* e)
{
	switch (e->cmd())
	{
	case EVENT_COUNTRY_LITE:
		{
			HandleCountryLite(e);
		}
		break;
	case C2S_RceQueryStarInfo:
		{
			HandleQueyrStarInfo(e);
		}
		break;
	default:
		break;
	}
}

void DealCountryHandle::HandleCountryLite(Event* e)
{
#if 0
	GameDataHandler* pUserManager = eh_->getDataHandler();
	map<int,GameRgnDataHandler*> mapRDH;
	for(int i=0;i<e->countrylite_size();i++)
	{
		GCG_CountryLite* pLite = e->mutable_countrylite(i);
		GameRgnDataHandler* pRDH = pUserManager->GetRgnDataHandler(pLite->region());
		if(pRDH==NULL)
			continue;
		mapRDH[pLite->region()] = pRDH;
		CLiteCountry* pCntr = pRDH->GetLiteCountryInfo(0,true);
		if(pCntr==NULL)
			continue;
		for (int k = 0; k < pLite->city_size(); k++){
			GCG_City *pCity = pLite->mutable_city(k);
			if(pCity){
				
			}
		}
	}
#endif
}

void DealCountryHandle::HandleQueyrStarInfo(Event *e)
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

	RseQueryStarInfo rsp;
	string text;
	rsp.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryStarInfo,text);
}
