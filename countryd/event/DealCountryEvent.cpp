#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealCountryEvent.h"
#include "../CountryRgnDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../common/const_def.h"
#include "../../event/EventDefine.h"
#include <math.h>
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../CountryConfigMgr.h"
#include "../CountryDefine.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealCountryEvent::DealCountryEvent()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealCountryEvent::~DealCountryEvent()
{

}

void DealCountryEvent::registHandler()
{
	eh_->getEventHandler()->registHandler(EVENT_ADMIN_RELOAD, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_UPDATE_COUNTRY_USER, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_LITE, ((ProcessRoutine) DealCountryEvent::handle_));	
}

void DealCountryEvent::handle(Event* e)
{	
	GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
	int nGameID = pNeedArgs->gamesrvid();
	int nRegion = pNeedArgs->region();

	switch(e->cmd())
	{
	case EVENT_ADMIN_RELOAD:
		{
			processReloadConfig(e);
			return;
		}
		break;
	case EVENT_COUNTRY_LITE:
		{
			HandleCountryLite(e);
			return;
		}
		break;
	default:
		break;
	
	}

	CountryDataHandler* pDH= eh_->getDataHandler();
	if(!pDH->IsRightRegion(nRegion))
	{
		return;//·ÖÇø´íÎó
	}
	
	CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
	if(pRDH==NULL)
	{
		return;
	}

	switch(e->cmd())
	{
	case EVENT_UPDATE_COUNTRY_USER:
		{
			HandleUpdateCountryUser(e,pRDH,nGameID);
		}		
		break;
	default:
		break;
	}
}

void DealCountryEvent::processReloadConfig(Event* e)
{
	CountryConfigMgr::GetInst()->LoadCountryConfigInfo(eh_->GetCountrySrvID(),false);
}

void DealCountryEvent::HandleCountryLite(Event* e)
{
	eh_->SendCountryLiteInfo(e->mutable_countryneed()->gamesrvid());
}
