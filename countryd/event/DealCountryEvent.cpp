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
#include "../event/RseUpdateAlliances.pb.h"

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
	eh_->getEventHandler()->registHandler(C2S_RceUpdateAlliances, ((ProcessRoutine) DealCountryEvent::handle_));//hansoul
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
		return;//分区错误
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
	case C2S_RceUpdateAlliances:
		{
			HandleUpdateAlliances(e, pRDH, nGameID);//hansoul
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

void DealCountryEvent::HandleUpdateAlliances(Event* e, CountryRgnDatHandler *pRDH, int nGameID)//hansoul
{
	RceUpdateAlliances* rce = e->mutable_ce_rceupdatealliances();
	RseUpdateAlliances* rse = e->mutable_se_rseupdatealliances();
	rse->set_action(rce->action());
	string strAction = rce->action();
	int alnId = 0;
	if(strAction == "createAlliance")
	{
		Members* mb = new Members;
		Alliance* aln = rse->mutable_alliance(0);
		Member* m = aln->mutable_member(0);
		mb->id = m->id();
		mb->name = m->name();
		mb->pictureUrl = m->pictureurl();
		mb->role = m->role();
		mb->score = m->score();
		mb->currentWarScore = m->currentwarscore();
		
		alnId = pRDH->CreateAlliance(rce->name(),rce->logo(),rce->description(),rce->publicrecruit(),rce->guid(), mb);
		if(0 == alnId)
		{
			//不成功退钱！
		}
		rse->Clear();
		rse->set_action(rce->action());
		pRDH->FillinAln(alnId, rse,true);
	}
	else if(strAction == "editAlliance")
	{
		if(!pRDH->editAlliance(rce->allianceid(), rce->logo(), rce->description(),rse))
		{
			
		}
	}
	else if(strAction == "updatePublic")
	{
		if(!pRDH->updatePublic(rce->allianceid(),rce->publicrecruit(),rse))
		{
			
		}
	}
	else if(strAction == "getMyAlliance")
	{
		int64 playerId = 0;
		safe_atoll(rce->playerid(), playerId);
		if(!pRDH->getMyAlliance(rce->allianceid(),rce->attackscore(), rce->warscore(), rce->role(), playerId, rse))
		{
			
		}
	}
	else if(strAction == "joinAlliance")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		MsgAllianceNews* news = rse->mutable_alliancenews(0);
		if(!pRDH->joinAlliance(rce->allianceid(),guid, rce->level(), news->name(), news->pictureurl() ,rse->totalsize(),rse))
		{
			
		}
	}
	else if(strAction == "refuseJoinAlliance")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		int64 playerId = 0;
		safe_atoll(rce->playerid(), playerId);
		if(!pRDH->refuseJoinAlliance(rce->allianceid(), playerId, guid, rse))
		{
			
		}
	}
	else if(strAction == "agreeJoinAlliance")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		int64 playerId = 0;
		safe_atoll(rce->playerid(), playerId);
		if(!pRDH->agreeJoinAlliance(rce->allianceid(), playerId, guid, rse))
		{

		}
	}
	else if(strAction == "leaveAlliance")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->leaveAlliance(rce->allianceid(), guid, rse))
		{

		}
	}
	else if(strAction == "kickMember")
	{
		int64 adminId = 0;
		safe_atoll(rce->adminid(), adminId);
		int64 memberId = 0;
		safe_atoll(rce->memberid(), memberId);
		if(!pRDH->kickMember(adminId, memberId, rse))
		{
			
		}
	}
	else if(strAction == "warHistory")
	{
		if(!pRDH->warHistory(rce->allianceid(), rce->count(), rce->startindex(), rse))
		{

		}
	}
	else if(strAction == "declareWar")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->declareWar(guid, rce->allianceid(),rse))
		{

		}
	}
	else if(strAction == "sendMessage")
	{
		if(!pRDH->sendMessage(rse))
		{

		}
	}
	else if(strAction == "getNews")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->getNews(guid, rce->count(), rce->fromindex(), rse))
		{

		}
	}
	else if(strAction =="getReward")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->getReward(guid, rce->alliancesbattleswon(),rce->warpoints(), rse))
		{
			
		}
	}
	else if(strAction == "getSuggestedAlliances")
	{
		if(!pRDH->getSuggestedAlliances(rce->count(),rse))
		{

		}
	}
	else if(strAction == "getAlliances")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->getAlliances(guid, rce->count(), rce->from(), rce->userpage(), rce->searchkey(),rse))
		{

		}
	}
	else if(strAction == "getAllianceById" || strAction == "getAllianceByUserId")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->getAlliance(rce->allianceid(),guid, rce->includemembers(), rse))
		{
			
		}
	}
	
	else if(strAction == "grantMember")
	{
		int64 memberId = 0;
		safe_atoll(rce->memberid(), memberId);
		if(!pRDH->grantMember(memberId, rce->role(), rse))
		{

		}
	}
	else if(strAction == "getAlliancesNotInWar")
	{
		int64 guid = 0;
		safe_atoll(rce->guid(), guid);
		if(!pRDH->getAlliancesNotInWar(guid, rce->count(), rce->from(), rce->userpage(), rce->searchkey(),rse))
		{

		}
	}
	else
	{

	}
	
	
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
