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
#include "../logic/CountryNameTbl.h"
#include "../logic/StarLogic.h"

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
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_LITE, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_USER_LOGIN, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceQueryStarInfo, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceQueryGalaxyWindow, ((ProcessRoutine) DealCountryEvent::handle_));
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
	case EVENT_USER_LOGIN:
		{
			HandleNewPlayerLogin(e, pRDH, nGameID);
		}
		break;
	case C2S_RceQueryStarInfo:
		{
			HandleQueryStarInfo(e, pRDH, nGameID);
		}
		break;
	case C2S_RceQueryGalaxyWindow:
		{
			HandleQueryGalaxyWindow(e, pRDH, nGameID);
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

void DealCountryEvent::HandleNewPlayerLogin(Event* e, CountryRgnDatHandler *pRDH, int nGameID)
{
	NewStar_Req *pReq = e->mutable_newstarreq();
	int64 uid = pReq->accountid();
	int nPlanetId = pReq->planetid();
	string platId = pReq->name();
	string url = pReq->url();

	NewStar_Rsp *pRsp = e->mutable_newstarrsp();
	string sku;
	int name, type, nStarId;
	pRDH->GetNewPlayerStar(uid, nPlanetId, platId, url, sku, name, type, nStarId);
	pRsp->set_id(nStarId);
	pRsp->set_sku(sku);
	pRsp->set_name(name);
	pRsp->set_type(type);
	e->set_state(UserLogin_CoG_Rsp);
	eh_->sendEventToGamed(e, nGameID);
}

void DealCountryEvent::HandleQueryStarInfo(Event* e, CountryRgnDatHandler *pRDH, int nGameID)
{
	RceQueryStarInfo *pReq = e->mutable_ce_rcequerystarinfo();
	int nStarId = pReq->starid();

	RseQueryStarInfo *pRsp = e->mutable_se_rsequerystarinfo();
	pRDH->FillinStarInfo(nStarId, pRsp);
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e, nGameID);
}

void DealCountryEvent::HandleQueryGalaxyWindow(Event* e, CountryRgnDatHandler *pRDH, int nGameID)
{
	RceQueryGalaxyWindow *pReq = e->mutable_ce_rcequerygalaxywindow();
	int topLeftx = pReq->topleftcoordx();
	int topLefty = pReq->topleftcoordy();
	int bottomRihgtx = pReq->bottomrightcoordx();
	int bottomRihgty = pReq->bottomrightcoordy();
	
	RseQueryGalaxyWindow *pRsp = e->mutable_se_rsequerygalaxywindow();
	pRsp->set_ret(0);
	pRsp->set_topleftcoorx(topLeftx);
	pRsp->set_topleftcoory(topLefty);
	pRsp->set_bottomrightcoorx(bottomRihgtx);
	pRsp->set_bottomrightcoory(bottomRihgty);
	
	map<int, CCity*>& mapCitys = pRDH->GetAllCity();
	for (map<int, CCity*>::iterator iter = mapCitys.begin(); iter != mapCitys.end(); iter++){
		CCity *pCity = iter->second;
		int x = pCity->GetX();
		int y = pCity->GetY();
		if(x >= topLeftx && x <= bottomRihgtx && y >= topLefty && y <= bottomRihgty){
			GalaxyWindow *pMsg = pRsp->add_galaxywindow();
			char sku[128] = {0};
			sprintf(sku, "%d:%d", x, y);
			pMsg->set_sku(sku);
			pMsg->set_starid(pCity->GetCityID());
			pMsg->set_type(pCity->GetType());
			pMsg->set_name(pCity->GetName());
			pMsg->set_planetsoccupied(pCity->GetPlanetSize());
			pMsg->set_planetsfree(StarLogicInst::instance().STAR_MAX_COUNTRY - pCity->GetPlanetSize());
		}
	}

	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e, nGameID);
}
