#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealCountryEvent.h"
#include "DealGuildEvent.h"
#include "DealGvgEvent.h"
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
#include "../logic/GuildInfoCfg.h"
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../logic/OfficerInfoCfg.h"
#include "../logic/WeaponRankActiveCfg.h"
#include "../logic/LeagueBattleCfg.h"
#include "../logic/NewRegionActivityCfg.h"

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
	eh_->getEventHandler()->registHandler(C2S_RceCountryOpt, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_FORCECHGCITY, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_SENDCOUNTRY_REPORT, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_ADMIN_OPT,((ProcessRoutine)DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_TIMER, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_BLOODBTL_OVER, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_UPDATE_COUNTRY_WORLDARENA, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceOfficerOpen, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceOfficerOpt, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(C2S_RceOfficerRightInfo, ((ProcessRoutine) DealCountryEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_USER_LITE, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_COUNTRY_UPDATEUSERNAMEDATA, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceWPCBaseOpen, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_UPDATE_COUNTRY_WPCRANK, ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceCountryChatMsg, ((ProcessRoutine) DealCountryEvent::handle_));
    eh_->getEventHandler()->registHandler(EVENT_ADMIN_RESETGUILDPKSCORE,((ProcessRoutine) DealCountryEvent::handle_));
    eh_->getEventHandler()->registHandler(EVENT_ADMIN_OUTPUTGUILDUSERLIST,((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceAttack , ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceNPCActive , ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceArmRace , ((ProcessRoutine) DealCountryEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_ARM_RACE_RANK , ((ProcessRoutine) DealCountryEvent::handle_));
	//eh_->getEventHandler()->registHandler(C2S_RceOneYuanPurchase , ((ProcessRoutine) DealCountryEvent::handle_));
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
	case EVENT_COUNTRY_TIMER:
		{//国家计时器，1秒钟接受一次消息
			HandleCountryTimer(e);
			return;
		}
		break;
	case EVENT_COUNTRY_BLOODBTL_OVER:
		{
			HandleountryBloodBtlOver(e);
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
	case EVENT_UPDATE_COUNTRY_USER:
		{
			HandleUpdateCountryUser(e,pRDH,nGameID);
		}		
		break;
	case EVENT_COUNTRY_UPDATEUSERNAMEDATA:
		{
			HandleCountryUpdateUserNameData(e,pRDH,nGameID);
		}
		break;
	case C2S_RceCountryOpt:
		{
			HandleRceCountryOpt(e,pRDH,nGameID);
		}		
		break;
	case EVENT_COUNTRY_FORCECHGCITY:
		{
			HandleFouceChgCity(e,pRDH,nGameID);
		}		
		break;
	case EVENT_ADMIN_OPT:
		{
			HandleAdminOpt(pRDH,e->mutable_adminopt_req());
		}
		break;
	case EVENT_UPDATE_COUNTRY_WORLDARENA:
		{
			HandleUpdateWorldArenaRank(e,pRDH,nGameID);
		}		
		break;
	case C2S_RceOfficerOpen:
		{
			HandleRceOfficerOpen(e,pRDH,nGameID);
		}
		break;
	case C2S_RceOfficerOpt:
		{
			HandleRceOfficerOpt(e,pRDH,nGameID);
		}
		break;
	case C2S_RceOfficerRightInfo:
		{
			HandleRceOfficerRightInfo(e,pRDH,nGameID);
		}
		break;
	case EVENT_COUNTRY_USER_LITE:
		{
			HandleCountryUserLite(e,pRDH,nGameID);
		}		
		break;
	case C2S_RceWPCBaseOpen:
		{
			HandleRceWPCBaseOpen(e,pRDH,nGameID);
		}		
		break;
	case EVENT_UPDATE_COUNTRY_WPCRANK:
		{
			HandleUpdateCountryWpcRank(e,pRDH,nGameID);
		}
		break;
	case C2S_RceCountryChatMsg:
		{
			HandleCountryChatOpt(e , pRDH , nGameID);
			//HandleCountryChat(e,pRDH,nGameID);
		}
		break;
    case EVENT_ADMIN_RESETGUILDPKSCORE:
        {
            HandleResetGuildPKScore(e, pRDH, nRegion);
        }
        break;
    case EVENT_ADMIN_OUTPUTGUILDUSERLIST:
        {
            HandleOutputGuildUserList(e, pRDH, nRegion);
        }
        break;
	case C2S_RceAttack:
		{
			HandleUpdataNpcActiveRecord(e , pRDH , nGameID);
		}
		break;
	case C2S_RceNPCActive:
		{
			HandleGetNpcActiveRank(e , pRDH , nGameID);
		}
		break;
	case C2S_RceArmRace:
		{
			HandGetAllArmRaceRank(e , pRDH , nGameID);
		}
		break;
	case EVENT_ARM_RACE_RANK:
		{
			HandGetSelfArmRaceRank(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealCountryEvent::HandleCountryTimer(Event* e)
{//1秒钟到一次
	
	CheckCountryBloodBtl();
	CountryDataHandler* pDH= eh_->getDataHandler();
	if(pDH)
	{
		const vector<int>& lstRgn = pDH->GetRegionLst();
		for(int i=0;i<(int)lstRgn.size();i++)
		{
			int nRegion = lstRgn[i];
			CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
			if(pRDH==NULL)
				continue;
			CheckAutoBloodBtlStart(pRDH);

			pRDH->CheckGuildRankSort();
			pRDH->GuildStat(eh_);
			pRDH->CheckCountryUserUpdateFlag(eh_);
			pRDH->CheckGveRank();
			pRDH->CheckGvg();
			pRDH->CheckWpcRank(eh_);
			pRDH->GvgStat(eh_);
			pRDH->CheckLeagueBattle(eh_);
			pRDH->CheckBossBtlMsg(eh_, nRegion);
			pRDH->CheckNpcActiveRank();
            pRDH->CheckRedEnvelopeInfo(eh_);
			pRDH->CheckWpcLabSearchRank();
			pRDH->CheckCityBuffActive(eh_);
			DealGuildEvent::getInstance()->CheckGuildBtlStep(pRDH);
			DealGvgEvent::getInstance()->CheckGvgBtlStep(pRDH);
		}
	}
}

void DealCountryEvent::processReloadConfig(Event* e)
{
	CountryConfigMgr::GetInst()->LoadCountryConfigInfo(eh_->GetCountrySrvID(),false);

	CountryDataHandler* pDH= eh_->getDataHandler();
	if(pDH != NULL)
	{
		pDH->CheckClearArenaData();
	}
}

void DealCountryEvent::HandleCountryLite(Event* e)
{
	eh_->SendCountryLiteInfo(e->mutable_countryneed()->gamesrvid());
}


void DealCountryEvent::HandleFouceChgCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	e->set_state(Status_Country_To_Game);

	int64 nUserID	= e->uid();
	int nOldCity = pSrvOpt->selfcity();
	int nNewCity = pSrvOpt->tarcity();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	CCity* pOldCity = pRDH->GetCity(nOldCity);
	CCity* pNewCity = pRDH->GetCity(nNewCity);

	//判断数据是否一致
	if(nOldCity == 0||pNewCity==NULL||pDBCUser==NULL)
		return;
	int nCountryID = pNewCity->GetCountry();
	if(nCountryID!=pSrvOpt->tarcountry())
		return;

	//更改玩家所在城市，此处先改country后改game，可能会有bug
	bool bSuc = pRDH->UserChgCity(pDBCUser,nOldCity,nNewCity);
	int nRet = bSuc?0:1;
	pSrvOpt->set_ret(nRet);
	if(nRet == 0)
	{
		pRDH->markUserLstDirty(nUserID);
		eh_->sendEventToGamed(e,nGameID);
		if(pDBCUser->officerposition()>0)
		{
			//发送给该国家的所有玩家更新官员信息
			//eh_->SendOfficerRightInfo2CountryUser(pDBCUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
		}
		//发送该城市的所有官员使用的权限
		eh_->SendOfficerRightUsed(pRDH,pNewCity,nUserID);
	}
	return;	
}

void DealCountryEvent::AddCountryReport(PVPFightUser* pMsgAtk,PVPFightUser* pMsgDef,CountryRoportType emType,CountryRgnDatHandler* pRDH,int nGameID)
{
	CountryDataHandler* pDH= eh_->getDataHandler();
	CCountry* pEarth = pRDH->GetEarthCountry();
	DB_C_Country* pDBCountry = pEarth->GetDBCountry();
	DB_C_BtlRecord* pDBReport = NULL;
	if(pDBCountry->btlrecord_size()<MAX_COUNTRY_REPORT_CNT)
	{
		pDBReport = pDBCountry->add_btlrecord();
	}
	else
	{
		pDBReport = pDBCountry->mutable_btlrecord(0);
		for(int i=1;i<pDBCountry->btlrecord_size();i++)
		{
			DB_C_BtlRecord* pTmpReport = pDBCountry->mutable_btlrecord(i);
			if(pTmpReport->btltime()<pDBReport->btltime())
			{
				pDBReport = pTmpReport;
			}
		}
	}
	if(pDBReport==NULL)
		return;
	pDBReport->set_type(emType);
	pDBReport->set_btltime((int)time(NULL));
	pDBReport->set_atkid(pMsgAtk->userid());
	pDBReport->set_atkname(pMsgAtk->name());
	pDBReport->set_atkcountry(pMsgAtk->country());
	pDBReport->set_defid(pMsgDef->userid());
	pDBReport->set_defname(pMsgDef->name());
	pDBReport->set_defcountry(pMsgDef->country());
	pDBReport->set_cityid(pMsgDef->city());
	pDBReport->set_winningcnt(0);

	pRDH->markCountryDirty(pEarth);

	int64 nAtkID;
	safe_atoll(pMsgAtk->userid(),nAtkID);
	SYS_LOG(nAtkID,LT_CNTR_CaptureCity,pRDH->GetRegion(),0,pMsgAtk->country()<<pMsgDef->city()<<pMsgDef->country());		

}

void DealCountryEvent::AddCountryReport(DB_C_BloodBtl* pDBBloodBtl,CountryRgnDatHandler* pRDH)
{
	if(pDBBloodBtl==NULL)
		return;
	CountryDataHandler* pDH= eh_->getDataHandler();
	CCountry* pEarth = pRDH->GetEarthCountry();
	DB_C_Country* pDBCountry = pEarth->GetDBCountry();
	DB_C_BtlRecord* pDBReport = NULL;

	if(pDBCountry->btlrecord_size()<MAX_COUNTRY_REPORT_CNT)
	{
		pDBReport = pDBCountry->add_btlrecord();
	}
	else
	{
		pDBReport = pDBCountry->mutable_btlrecord(0);
		for(int i=1;i<pDBCountry->btlrecord_size();i++)
		{
			DB_C_BtlRecord* pTmpReport = pDBCountry->mutable_btlrecord(i);
			if(pTmpReport->btltime()<pDBReport->btltime())
			{
				pDBReport = pTmpReport;
			}
		}
	}
	if(pDBReport==NULL)
		return;
	pDBReport->set_type(Cntr_Report_BloodBtl);
	pDBReport->set_btltime(pDBBloodBtl->overtime());
	pDBReport->set_atkid(toString(pDBBloodBtl->launchuserid()));
	pDBReport->set_atkname(pDBBloodBtl->launchname());
	pDBReport->set_atkcountry(pDBBloodBtl->atkcountry());
	pDBReport->set_defid("");
	pDBReport->set_defname("");
	pDBReport->set_defcountry(pDBBloodBtl->defcountry());
	pDBReport->set_cityid(pDBBloodBtl->defcity());
	pDBReport->set_winningcnt(0);

	pDBReport->set_bloodatkcity(pDBBloodBtl->atkcity());
	pDBReport->set_blooddefcity(pDBBloodBtl->defcity());
	pDBReport->set_bloodwincity(pDBBloodBtl->wincity());

	pRDH->markCountryDirty(pEarth);

	if(pDBBloodBtl->wincity() == pDBBloodBtl->atkcity())
	{
		SYS_LOG((int64)pDBBloodBtl->launchuserid(),LT_CNTR_CaptureCity,pRDH->GetRegion(),0,pDBBloodBtl->atkcountry()<<pDBBloodBtl->defcity()<<pDBBloodBtl->defcountry());		
	}
	else
	{
		SYS_LOG((int64)pDBBloodBtl->launchuserid(),LT_CNTR_CaptureCity,pRDH->GetRegion(),0,pDBBloodBtl->defcountry()<<pDBBloodBtl->atkcity()<<pDBBloodBtl->atkcountry());		
	}

}

void DealCountryEvent::AddCountryReport(CountryReport* pReport,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(pReport==NULL||pReport->btltime()<=0)
		return;
	CountryDataHandler* pDH= eh_->getDataHandler();
	CCountry* pEarth = pRDH->GetEarthCountry();
	DB_C_Country* pDBCountry = pEarth->GetDBCountry();
	DB_C_BtlRecord* pDBReport = NULL;
	if(pDBCountry->btlrecord_size()<MAX_COUNTRY_REPORT_CNT)
	{
		pDBReport = pDBCountry->add_btlrecord();
	}
	else
	{
		pDBReport = pDBCountry->mutable_btlrecord(0);
		for(int i=1;i<pDBCountry->btlrecord_size();i++)
		{
			DB_C_BtlRecord* pTmpReport = pDBCountry->mutable_btlrecord(i);
			if(pTmpReport->btltime()<pDBReport->btltime())
			{
				pDBReport = pTmpReport;
			}
		}
	}
	if(pDBReport==NULL)
		return;
	pDBReport->set_type(pReport->type());
	pDBReport->set_btltime(pReport->btltime());
	pDBReport->set_atkid(pReport->atkid());
	pDBReport->set_atkname(pReport->atkname());
	pDBReport->set_atkcountry(pReport->atkcountry());
	pDBReport->set_defid(pReport->defid());
	pDBReport->set_defname(pReport->defname());
	pDBReport->set_defcountry(pReport->defcountry());
	pDBReport->set_cityid(pReport->cityid());
	pDBReport->set_winningcnt(pReport->winningcnt());

	pRDH->markCountryDirty(pEarth);

	int64 nAtkID;
	safe_atoll(pReport->atkid(),nAtkID);
	SYS_LOG(nAtkID,LT_CNTR_CaptureCity,pRDH->GetRegion(),0,pReport->atkcountry()<<pReport->cityid()<<pReport->defcountry());		

}

bool DealCountryEvent::CheckCaptureCity(CountryRgnDatHandler* pRDH,DB_C_User* pDBCUser,CCity* pSelfCity,CCity* pTarCity)
{
	if(/*pDBCUser==NULL||*/pSelfCity==NULL||pTarCity==NULL)
		return false;

	if(!CountryInfoCfg::Instance().CanCapure(pTarCity->GetCityID()))
		return false;
	if(CountryInfoCfg::Instance().IsUserCountry(pTarCity->GetCountry()))
	{//玩家城市
		if(pTarCity->GetUserCnt()<=10)
		{
			if(pTarCity->CheckEmptyCity())
			{//城市被抢	
				if(pDBCUser)
				{
					int nOldCityCountry = pTarCity->GetCountry();
					pRDH->CityChgCountry(pDBCUser->userid(),pTarCity,pSelfCity->GetCountry());
					pRDH->UserChgCity(pDBCUser,pSelfCity->GetCityID(),pTarCity->GetCityID());
					pRDH->markUserLstDirty(pDBCUser->userid());
					if(pTarCity->HasOfficerRightBuff())
					{
						//发送被抢占国家的所有玩家更新城市官员使用的权限信息
						pTarCity->ClearOfficerRightBuff();
						eh_->SendOfficerRightInfo2CountryUser(nOldCityCountry,pRDH->GetRegion(),-1,E_Officer_Fill_Right);

					}
					if(pDBCUser->officerposition()>0)
					{
						//发送给抢占国家的所有玩家更新官员信息
						//eh_->SendOfficerRightInfo2CountryUser(pDBCUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
					}
				}
				else
				{
					pRDH->CityChgCountry(0,pTarCity,pSelfCity->GetCountry());
				}
				return true;
			}
		}
	}
	else
	{//NPC城市
		if(pTarCity->GetUserCnt()<=0)
		{
			if(pTarCity->CheckEmptyCity())
			{
				if(pDBCUser)
				{
					pRDH->CityChgCountry(pDBCUser->userid(),pTarCity,pSelfCity->GetCountry());
					pRDH->UserChgCity(pDBCUser,pSelfCity->GetCityID(),pTarCity->GetCityID());
					pRDH->markUserLstDirty(pDBCUser->userid());
					if(pDBCUser->officerposition()>0)
					{
						//发送给该国家的所有玩家更新官员信息
						//eh_->SendOfficerRightInfo2CountryUser(pDBCUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
					}
				}
				else
				{
					pRDH->CityChgCountry(0,pTarCity,pSelfCity->GetCountry());
				}

			}
			return true;
		}
	}
	return false;
}

void DealCountryEvent::HandleCountryUpdateUserNameData(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseCountryUserLst* pRseLst = e->mutable_se_rsecountryuserlst();

	for(int i=0;i<pRseLst->user_size();i++)
	{
		CountryUser* pCUser = pRseLst->mutable_user(i);
		int64 nID = 0;
		safe_atoll(pCUser->baseid(),nID);
		DB_C_User* pDBCUser = pRDH->GetDBCityUser(nID);
		if(pDBCUser==NULL)
			continue;
		pDBCUser->set_name(pCUser->first_name());
		pDBCUser->set_pic(pCUser->pic());
		pDBCUser->set_level(pCUser->lvl());
		pDBCUser->set_viplevel(pCUser->viplevel());
		pDBCUser->set_viptype(pCUser->viptype());
	
		pRDH->markUserLstDirty(nID);
	}
}

void DealCountryEvent::HandleUpdateCountryUser(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int64 nUserID	= e->uid();
	int nSelfCity	= pSrvOpt->selfcity();
	int nSelfCountry= pSrvOpt->selfcountry();
	CCountry* pCountry  = pRDH->GetCountry(nSelfCountry);
	if(pCountry==NULL)
	{//没有国家信息，直接return
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_UserNoCountry<<nSelfCity<<nSelfCountry);
		return;
	}

	int nRightCity  = nSelfCity;
	bool bNeedSave = false;
	pRDH->CheckUserInfo(nUserID,pCountry,nSelfCity,nRightCity,bNeedSave);
	if(nRightCity != nSelfCity)
	{
		pSrvOpt->set_selfcity(nRightCity);
		pSrvOpt->set_selfcountry(nSelfCountry);
		eh_->sendEventToGamed(e,nGameID);		
	}
	
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser) 
	{
		if(pDBCUser->name()!=pSrvOpt->selfname()||pDBCUser->pic()!=pSrvOpt->selfpic())
		{
			bNeedSave = true;
			pDBCUser->set_name(pSrvOpt->selfname());
			pDBCUser->set_pic(pSrvOpt->selfpic());
			bNeedSave = true;
		}
		if(pDBCUser->level() != pSrvOpt->selflv())
		{
			pDBCUser->set_level(pSrvOpt->selflv());
			bNeedSave = true;
		}
		if(pDBCUser->viplevel()!=pSrvOpt->viplevel()||pDBCUser->viptype()!=pSrvOpt->viptype())
		{
			pDBCUser->set_viplevel(pSrvOpt->viplevel());
			pDBCUser->set_viptype(pSrvOpt->viptype());
			bNeedSave = true;
		}
	}

	if(bNeedSave)
	{
		pRDH->markUserLstDirty(nUserID);
	}
}

void DealCountryEvent::HandleRceCountryOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceCountryOpt* pRceOpt = e->mutable_ce_rcecountryopt();
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nType		= pRceOpt->type();
	int64 nUserID	= e->uid();
	e->set_state(Status_Country_To_Game);

	switch (nType)
	{
	case COT_SelCountry:
		{
			int nSelCountry = pSrvOpt->tarcountry();
			int nUserCnt = 0;
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			
			if(pCUser)
			{
				//数据不一致
				SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_CityError<<nUserID<<pCUser->cityid()<<pCUser->countryid()<<nSelCountry);
				int nErrCity = pCUser->cityid();
				//这是开新服活动的实验室探索次数，这里是为了防止新加入国家的数据被清
				int nCnt=pCUser->wpclabsearchcnt();
				vector<DB_One_Yuan> vecOenYuan;
				for(int i=0; i<pCUser->oneyuan_size();i++)
				{
					//DB_One_Yuan DBOneYuan = pCUser->oneyuan(i);
					vecOenYuan.push_back(pCUser->oneyuan(i));
				}
				pCUser->Clear();
				pCUser->set_userid(nUserID);
				CCity* pCity = pRDH->GetCity(nErrCity);
				if(pCity)
				{
					bool bIsChampion =pRDH->IsChampion(pCUser->userid());
					pCity->UserLeaveCity(pCUser,bIsChampion);
				}
				//把以前存在的user数据再添加进去
				if(nCnt>0)
					pCUser->set_wpclabsearchcnt(nCnt);
				for(int i=0; i<vecOenYuan.size();i++)
				{
					DB_One_Yuan TempDB = vecOenYuan[i];
					DB_One_Yuan*pDB = pCUser->add_oneyuan();
					pDB->set_flag(TempDB.flag());
					for(int j=0;j<TempDB.rewardkey_size();j++)
					{
						DB_One_Yuan_Reward_key*pTempKey = TempDB.mutable_rewardkey(j);
						DB_One_Yuan_Reward_key*pDBKey =pDB->add_rewardkey();
						pDBKey->set_redwardkey(pTempKey->redwardkey());
						pDBKey->set_nindex(pTempKey->nindex());
					}
				}
			}
			else
			{
				pCUser = pRDH->AllocDBUser(nUserID);
			}
			int nRet = pRDH->UserSelCountry(pCUser,nSelCountry,nUserCnt);
			pRDH->markUserLstDirty(nUserID);

			

			pSrvOpt->set_ret(nRet);
			pSrvOpt->set_usercnt(nUserCnt);
			eh_->sendEventToGamed(e,nGameID);

			//发送该城市的所有官员使用的权限
			eh_->SendOfficerRightUsed(pRDH,pRDH->GetCity(pCUser->cityid()),nUserID);
		}
		break;
	case COT_ChgCountry:
		{//没有同步到geme国家人数，没必要
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				pCUser = pRDH->AllocDBUser(nUserID);
			}
			int nSelCountry = pSrvOpt->tarcountry();
			int nOldCountry = pSrvOpt->selfcountry();
			int nOldCityID  = pSrvOpt->selfcity();

			//官员身份取消
			if(pCUser->officerposition()>0)
			{
				E_Officer_Fill_Type fillType = E_Officer_Fill_Officer;
				pRDH->ClearUserOfficerPosition(pCUser,E_Officer_Type_Arena);
				pRDH->ClearUserOfficerPosition(pCUser,E_Officer_Type_Guild);
				eh_->SendCountryUserLiteInfo(pRDH,pCUser);
				//if(pRDH->ClearUserCityOfficerRight(pCUser,pRDH->GetCountry(nOldCountry)))
				//{
				//	fillType = E_Officer_Fill_ALL;
				//}
				//eh_->SendOfficerRightInfo2CountryUser(nOldCountry,pRDH->GetRegion(),-1,fillType);
			}

			pRDH->UserLeaveCountry(pCUser,nOldCountry,nOldCityID);

			int nUserCnt = 0;
			pRDH->UserSelCountry(pCUser,nSelCountry,nUserCnt);

			//竞技场数据修改
			pRDH->CheckClearCountryArenaData();
			CCountry* pCountry = pRDH->GetCountry(nOldCountry);
			pRDH->UserLeaveArena(pCountry,pCUser,pRceOpt->chgnpcid());

			pDH->markCountryDirty(pCountry);
			pCountry = pRDH->GetCountry(nSelCountry);
			pRceOpt->set_arenaindex(pRDH->UserAddArena(pCountry,pCUser));

			pRDH->markUserLstDirty(nUserID);

			eh_->sendEventToGamed(e,nGameID);
			//发送该城市的所有官员使用的权限
			CCity* pCity = pRDH->GetCity(pCUser->cityid());
			if(pCity)
			{
				eh_->SendOfficerRightUsed(pRDH,pRDH->GetCity(pCUser->cityid()),nUserID);
				eh_->SendOfficerRightInfo2User(pRDH,pCUser->countryid(),nUserID,E_Officer_Fill_ALL);
			}
		}
		break;
	case COT_ClickMap:
		{
			int nSelfCountry	= pSrvOpt->selfcountry();
			int nSelfCity		= pSrvOpt->selfcity();
			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			CCountry* pCountry  = pRDH->GetCountry(nSelfCountry);
			if(pCountry==NULL)
			{
				pRseRet->set_ret(2);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			int nRightCity  = nSelfCity;
			bool bNeedSave = false;
			pRDH->CheckUserInfo(nUserID,pCountry,nSelfCity,nRightCity,bNeedSave);
			if(nRightCity != nSelfCity)
			{//点击地图时做一次校验	
				pSrvOpt->set_selfcity(nRightCity);
				nSelfCity = nRightCity;
			}
			if(bNeedSave)
			{
				pRDH->markUserLstDirty(nUserID);
			}
			vector<int>::iterator iter;
			const vector<int> lstCityID = CountryInfoCfg::Instance().GetCityLst();
			for(int i=0;i<(int)lstCityID.size();i++)
			{
				int nCityID = lstCityID[i];
				CCity* pCity = pRDH->GetCity(nCityID);
				if(pCity==NULL)
					continue;
				CityData* pRetCity = pRseRet->add_citylist();
				pRetCity->set_cityid(nCityID);
				pRetCity->set_countryid(pCity->GetCountry());
				pRetCity->set_usercnt(pCity->GetUserCnt());
				if(pCity->UpdateOfficerRightBuff())
				{
					pRDH->markCityDirty(pCity);
				}
				if(pCity->HasOfficerRightBuff())
				{
					CityRightInfo* pCityRightinfo = pRseRet->add_cityrightinfo();
					pRDH->FillOfficerRightUsed(pCity->GetDBCity(),pCityRightinfo);
				}
				else
				{
					pCity->ClearOfficerRightBuff();
				}

				pRetCity->set_lastbattletime( pCity->m_nLastBattleTime );
				if ( pRetCity->lastbattletime() < 0 )
				{
					pRetCity->set_lastbattletime( 0 );
				}
				pRetCity->set_champion(pCity->m_bChampionIn);
			}
			const map<int,DB_C_BloodBtl*>& mapBloobBtl = pRDH->GetBloobBtlLst();
			map<int,DB_C_BloodBtl*>::const_iterator itBB;
			for(itBB = mapBloobBtl.begin();itBB!=mapBloobBtl.end();itBB++)
			{
				const DB_C_BloodBtl* pDBBtl = itBB->second;
				if(pDBBtl==NULL||pDBBtl->launchtime()<=0||pDBBtl->wincity()>0)
					continue;
				CityBloodBattle* pFillBtl = pRseRet->add_bloodbtllite();
				pRDH->FillBloodBtl(pDBBtl,pFillBtl,false);
			}
			pRseRet->set_wpcrankuid(pRDH->GetChampoinUid());
			
			/*buff城市信息*/
			DB_C_CityBuffData* buffs = pRDH->GetCityBuffData();
			RseCityBuffInfo* bi = pRseRet->mutable_citybuffs();
			if(buffs && bi){
				bi->set_flag(buffs->flag());
				bi->set_state(buffs->state());
				for (int i = 0; i < buffs->randombuffs_size(); i++){
					RseRandomBuff* buf = bi->add_randombuffs();
					if(buf){
						buf->set_cityid(buffs->randombuffs(i).cityid());
						buf->set_buffid(buffs->randombuffs(i).buffid());
						buf->set_activetm(buffs->randombuffs(i).activetm());
					}
				}
			}

			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser != NULL)
			{
				LeagueMsg* pLeagueMsg = pRseRet->mutable_leaguemsg();
				pLeagueMsg->set_leagueid( pRDH->GetLeagueID( pCUser->countryid() ) );

				int nLeagueID = pRDH->GetLeagueID( pCUser->countryid() );
				CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
				if ( pCountryLeague != NULL )
				{
					time_t tNow = time(NULL);
					const LeagueBattleActiveTbl* pTbl = LeagueBattleCfg::Instance().GetActiveTbl( tNow );
					if ( pTbl != NULL )
					{
						if ( tNow < pTbl->m_nEndTm && tNow > pTbl->m_nAllStartTm )
						{
							pCountryLeague->FillLastRankList(pLeagueMsg,LEAGUERWDTYPEPERSON,3);
						}
						else
						{
							pCountryLeague->FillRankList(pLeagueMsg,LEAGUERWDTYPEPERSON,3);
						}
						pLeagueMsg->set_selfrank( pCountryLeague->GetPlayerRank(pCUser) );
					}
				}
			}

			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case COT_UpdateUser:
	case COT_ClickCity:
		{
			int nCityID = pSrvOpt->tarcity();
			int nSelfCityID = pSrvOpt->selfcity();
			int nSelfCountry= pSrvOpt->selfcountry();
			CCity* pTarCity = pRDH->GetCity(nCityID);
			CCity* pSelfCity = pRDH->GetCity(nSelfCityID);
			if(pTarCity==NULL||pSelfCity==NULL)
				return;
			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			int nPage = pRceOpt->pageinx();
			if(nPage<0)
				nPage = 0;
			pRseRet->set_pageinx(nPage);
			pRseRet->set_usercnt(pTarCity->GetUserCnt());

			pSrvOpt->set_usercnt(pTarCity->GetUserCnt());
			pSrvOpt->set_tarcountry(pTarCity->GetCountry());
			
			if(pTarCity->GetCountry() == nSelfCountry)
			{//同一个国家
				int nNeedThew = 0;
				int nRet = pRDH->CheckHasWay(nUserID,pSelfCity,pTarCity,nNeedThew);
				pSrvOpt->set_needthew(nNeedThew);
			}
			else
			{
				if(CountryInfoCfg::Instance().CanCapure(nCityID)&&pTarCity->GetUserCnt()<=0)
				{//空城
					int nNeedThew = 0;
					int nRet = pRDH->CheckHasWay(nUserID,pSelfCity,pTarCity,nNeedThew);
					pSrvOpt->set_needthew(nNeedThew);
				}
				else 
				{
					pSrvOpt->set_needthew(0);
				}
			}

			if(CountryInfoCfg::Instance().IsUserCountry(pTarCity->GetCountry()))
			{
				RseCountryUserLst* pRseUserLst = e->mutable_se_rsecountryuserlst();
				pRseUserLst->set_ncityid(pTarCity->GetCityID());
				pRDH->FillUserIDLst(pTarCity,nPage,pSrvOpt,false,pRseUserLst);
			}
			else
			{
				RseCountryUserLst* pRseUserLst = e->mutable_se_rsecountryuserlst();
				pRDH->FillNPCLst(pTarCity,nPage,pRseUserLst);
			}
			const map<int,DB_C_BloodBtl*>& mapBloobBtl = pRDH->GetBloobBtlLst();
			map<int,DB_C_BloodBtl*>::const_iterator itBB;
			for(itBB = mapBloobBtl.begin();itBB!=mapBloobBtl.end();itBB++)
			{
				const DB_C_BloodBtl* pDBBtl = itBB->second;
				if(pDBBtl==NULL||pDBBtl->launchtime()<=0||pDBBtl->wincity()>0)
					continue;
				CityBloodBattle* pFillBtl = pRseRet->add_bloodbtllite();
				pRDH->FillBloodBtl(pDBBtl,pFillBtl,false);
			}

			CityData* pRetCity = pRseRet->mutable_citydata();
			pRetCity->set_lastbattletime( pTarCity->m_nLastBattleTime );
			if ( pRetCity->lastbattletime() < 0 )
			{
				pRetCity->set_lastbattletime( 0 );
			}
			pRetCity->set_champion(pTarCity->m_bChampionIn);

			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser != NULL)
			{
				bool bSameLeague = pRDH->CheckSameLeague(pCUser,pTarCity);

				pSrvOpt->set_bsameleague(bSameLeague);
			}

			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case COT_GotoCity:
		{
			OptGotoCity(e,nUserID,pRDH,nGameID);
		}
		break;
	case COT_AttackOver:
	case COT_ReAttackOver:
	case COP_AtkSweepOver:
    case COT_AttackBomb:
    case COT_AttackBombOver:
		{
			Country_FightBackData* pFightBackData = e->mutable_countryfightbackdata();
			if(pFightBackData->attacknpc())
			{
				AttackNPCOver(e,nUserID,pRDH,nGameID);
			}
			else
			{
				AttackUserOver(e,nUserID,pRDH);
			}
		}
		break;
	case COT_ShowReport:
		{	
			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			DB_C_Country* pDBEarth = pRDH->GetEarthCountry()->GetDBCountry();
			for(int i=0;i<pDBEarth->btlrecord_size();i++)
			{
				DB_C_BtlRecord* pDBReport = pDBEarth->mutable_btlrecord(i);
				if(pDBReport->btltime()<=0)
					continue;
				CountryReport* pReport = pRseRet->add_report();
				pReport->set_type(pDBReport->type());
				pReport->set_btltime(pDBReport->btltime());
				pReport->set_atkid(pDBReport->atkid());
				pReport->set_atkname(pDBReport->atkname());
				pReport->set_atkcountry(pDBReport->atkcountry());
				pReport->set_defid(pDBReport->defid());
				pReport->set_defname(pDBReport->defname());
				pReport->set_defcountry(pDBReport->defcountry());
				pReport->set_cityid(pDBReport->cityid());
				pReport->set_winningcnt(pDBReport->winningcnt());
				pReport->set_bloodatkcity(pDBReport->bloodatkcity());
				pReport->set_blooddefcity(pDBReport->blooddefcity());
				pReport->set_bloodwincity(pDBReport->bloodwincity());
			}
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case COP_SetTarget:
		{
			CCountry* pCountry = pRDH->GetCountry(pRceOpt->countryid());
			if(pCountry==NULL)
				return;
			int nAtkCity = pRceOpt->atktargetcity();
			int nDefCity = pRceOpt->deftargetcity();
			pCountry->GetDBCountry()->set_atktargetcity(nAtkCity);
			pCountry->GetDBCountry()->set_deftargetcity(nDefCity);
			pRDH->markCountryDirty(pCountry);
			eh_->SendCountryLiteInfo();
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case COP_LaunchBloodBtl:
		{
			OptLaunchBloodBtl(e,nUserID,pRDH,nGameID);
		}
		break;
	//case COP_SeeBloodBtl:
	//	{
	//		RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
	//		DB_C_BloodBtl* pDBBloodBtl1 = pRDH->GetLastDBBloodBtl(pSrvOpt->selfcountry(),true);
	//		DB_C_BloodBtl* pDBBloodBtl2 = pRDH->GetLastDBBloodBtl(pSrvOpt->selfcountry(),false);
	//		CityBloodBattle* pBldBtl = pRseRet->mutable_bloodbtl();
	//		bool bNoChg = false;
	//		time_t ltLast = 0;
	//		if(pDBBloodBtl1&&pDBBloodBtl1->launchtime()>0)
	//		{//有血战
	//			if(pDBBloodBtl1->wincity()<=0)
	//			{//尚未结束，一直发
	//				bNoChg = true;
	//				pRDH->FillBloodBtl(pDBBloodBtl1,pBldBtl,true);
	//			}
	//			else 
	//			{
	//				CityBloodBattle* pBldBtl = pRseRet->mutable_bloodbtl();
	//				pRDH->FillBloodBtl(pDBBloodBtl1,pBldBtl,true);
	//				ltLast = pBldBtl->overtime();
	//			}
	//		}
	//		if(!bNoChg&&pDBBloodBtl2&&pDBBloodBtl2->launchtime()>0)
	//		{//有血战
	//			if(pDBBloodBtl2->wincity()<=0)
	//			{//尚未结束，一直发
	//				pRDH->FillBloodBtl(pDBBloodBtl2,pBldBtl,true);
	//			}
	//			else if(pDBBloodBtl2->overtime()>ltLast)
	//			{
	//				pRDH->FillBloodBtl(pDBBloodBtl2,pBldBtl,true);
	//			}
	//		}
	//		eh_->sendEventToGamed(e,nGameID);
	//	}
	//	break;
	case COT_Attack:
	case COT_ReAttack:
	case COP_AtkSweep:
		{
			CCity* pTarCity		= pRDH->GetCity(pRceOpt->cityid());
			if(pTarCity==NULL)
				return;
			pTarCity->m_nLastBattleTime = time(NULL);
		}
		break;

	//联盟战相关
	case COT_LeagueBtlMsg:
		{
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				return;
			}


			DB_C_CountryLeagueBattle* pDBResult = pRDH->GetEarthCountry()->GetDBCountryLeagueBattle();
			if ( pDBResult == NULL )
			{
				//数据错误
				return;
			}

			bool bOver = false;
			if ( pDBResult->activestate() >= LeagueBtlType_Over )
			{
				bOver = true;
			}

			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			LeagueMsg* pLeagueMsg = pRseRet->mutable_leaguemsg();
			pLeagueMsg->set_leagueid( pRDH->GetLeagueID( pCUser->countryid() ) );
			
			for ( int i = LEAGUEBTLCNTSTART; i <= LEAGUEBTLCNTEND; i++ )
			{
				DB_C_CountryLeagueMsg* pDBLeague = pRDH->GetDBCountryLeague(i);
				if ( pDBLeague != NULL )
				{
					for ( int j = 0; j < pDBLeague->countryid_size(); j++ )
					{
						CountryRank* pCountryRank = pLeagueMsg->add_countryrank();
						pCountryRank->set_countryid( pDBLeague->countryid(j) );

						if ( !bOver )
						{
							//未结束,取实时繁荣度
							CCountry* pCountry = pRDH->GetCountry( pDBLeague->countryid(j) );
							if ( pCountry != NULL )
							{
								pCountryRank->set_boom( pCountry->m_nBoom );
							}
						}

						else
						{
							if ( pDBLeague->boomrecord_size() < j + 1 )
							{
								pCountryRank->set_boom( 0 );
								continue;
							}

							pCountryRank->set_boom( pDBLeague->boomrecord( j ) );
						}

					}
				}
			}

			int nLeagueID = pRDH->GetLeagueID( pCUser->countryid() );
			CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
			if ( pCountryLeague != NULL )
			{
				pCountryLeague->FillRankList(pLeagueMsg,LEAGUERWDTYPEPERSON,3);
				pLeagueMsg->set_selfrank( pCountryLeague->GetPlayerRank(pCUser) );

				CGuild* pGuild = pRDH->GetGuild( pCUser->guildid() );
				if ( pGuild != NULL )
				{
					pLeagueMsg->set_selfguildrank( pCountryLeague->GetGuildRank(pGuild) );

					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					if ( pDBGuild != NULL )
					{
						pLeagueMsg->set_selfguildpoint(pDBGuild->leaguepoint());
					}
				}
			}

			if ( pDBResult->winid() != LEAGUEBTLDRAW )
			{
				if ( pDBResult->winid() == pLeagueMsg->leagueid() )
				{
					pLeagueMsg->set_winid( 1 );
				}
				else
				{
					pLeagueMsg->set_winid( 2 );
				}
			}
			else
			{
				pLeagueMsg->set_winid( 3 );
			}
			
			eh_->sendEventToGamed(e,nGameID);
		}
		break;

	case COT_LeagueBtlPlayerRank:
		{
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				return;
			}

			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			LeagueMsg* pLeagueMsg = pRseRet->mutable_leaguemsg();
			pLeagueMsg->set_leagueid( pRDH->GetLeagueID( pCUser->countryid() ) );

			int nLeagueID = pRDH->GetLeagueID( pCUser->countryid() );
			CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
			if ( pCountryLeague != NULL )
			{
				pCountryLeague->FillRankList(pLeagueMsg,LEAGUERWDTYPEPERSON);
				pLeagueMsg->set_selfrank( pCountryLeague->GetPlayerRank(pCUser) );

				CGuild* pGuild = pRDH->GetGuild( pCUser->guildid() );
				if ( pGuild != NULL )
				{
					pLeagueMsg->set_selfguildrank( pCountryLeague->GetGuildRank(pGuild) );

					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					if ( pDBGuild != NULL )
					{
						pLeagueMsg->set_selfguildpoint(pDBGuild->leaguepoint());
					}
				}
			}
	
			eh_->sendEventToGamed(e,nGameID);
		}
		break;

	case COT_LeagueBtlGuildRank:
		{
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				return;
			}

			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			LeagueMsg* pLeagueMsg = pRseRet->mutable_leaguemsg();
			pLeagueMsg->set_leagueid( pRDH->GetLeagueID( pCUser->countryid() ) );

			int nLeagueID = pRDH->GetLeagueID( pCUser->countryid() );
			CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
			if ( pCountryLeague != NULL )
			{
				pCountryLeague->FillRankList(pLeagueMsg,LEAGUERWDTYPEGUILD);
				pLeagueMsg->set_selfrank( pCountryLeague->GetPlayerRank(pCUser) );

				CGuild* pGuild = pRDH->GetGuild( pCUser->guildid() );
				if ( pGuild != NULL )
				{
					pLeagueMsg->set_selfguildrank( pCountryLeague->GetGuildRank(pGuild) );

					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					if ( pDBGuild != NULL )
					{
						pLeagueMsg->set_selfguildpoint(pDBGuild->leaguepoint());
					}
				}
			}

			eh_->sendEventToGamed(e,nGameID);
		}
		break;

	case COT_LeagueGetReward:
	case COT_LeagueRequestReward:
		{
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				return;
			}

			RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();
			LeagueMsg* pLeagueMsg = pRseRet->mutable_leaguemsg();
			pLeagueMsg->set_leagueid( pRDH->GetLeagueID( pCUser->countryid() ) );

			int nLeagueID = pRDH->GetLeagueID( pCUser->countryid() );
			CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
			if ( pCountryLeague != NULL )
			{
				pCountryLeague->FillRankList(pLeagueMsg,LEAGUERWDTYPEGUILD);
				pLeagueMsg->set_selfrank( pCountryLeague->GetPlayerRank(pCUser) );
				pLeagueMsg->set_playerrankreward( pCountryLeague->GetPlayerRank(pCUser) );

				CGuild* pGuild = pRDH->GetGuild( pCUser->guildid() );
				if ( pGuild != NULL )
				{
					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					if ( pDBGuild != NULL )
					{
						pLeagueMsg->set_selfguildrank( pCountryLeague->GetGuildRank(pGuild) );
						if ( pCUser->position() == GP_Leader && ( pCUser->userid() == pDBGuild->leagueleaderuid() || pDBGuild->leagueleaderuid() == 0 ) )
						{
							//判断该工会会长奖励是否被领取
							//会长奖励
							pLeagueMsg->set_guildrankreward( pCountryLeague->GetGuildRank(pGuild) );
						}

						pLeagueMsg->set_selfguildpoint(pDBGuild->leaguepoint());
					}
				}
			}

			DB_C_CountryLeagueBattle* pDBResult = pRDH->GetEarthCountry()->GetDBCountryLeagueBattle();
			if ( pDBResult == NULL )
			{
				//数据错误
				return;
			}

			if ( pDBResult->winid() != LEAGUEBTLDRAW )
			{
				if ( pDBResult->winid() == pLeagueMsg->leagueid() )
				{
					pLeagueMsg->set_winid( 1 );
				}
				else
				{
					pLeagueMsg->set_winid( 2 );
				}
			}
			else
			{
				pLeagueMsg->set_winid( 3 );
			}
			
			pLeagueMsg->set_bwin( pDBResult->winid() == pLeagueMsg->leagueid() );
			eh_->sendEventToGamed(e,nGameID);
		}
		break;

	case COT_LeagueRewardRecord:
		{
			DB_C_User* pCUser = pRDH->GetDBCityUser(nUserID);
			if(pCUser == NULL)
			{
				return;
			}

			CGuild* pGuild = pRDH->GetGuild( pCUser->guildid() );
			if ( pGuild != NULL )
			{
				DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
				if ( pDBGuild != NULL )
				{
					pDBGuild->set_leagueleaderuid( pCUser->userid() );
				}
			}
		}
		break;

	case COT_NormalSpy:
	case COT_SeniorSpy:
	case COT_AddThew:
	case COT_AddMorale:
	case COT_PickCountryBox:
	case COT_OpenCountryBox:
	case COT_ZhanGongChg:
		break;
    case COT_TeleportCity:
        {
            OptTeleportCity(e, nUserID, pRDH, nGameID);
        }
        break;

	default:
		break;
	}
}

void DealCountryEvent::AttackUserOver(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH)
{
	Country_FightBackData* pFightBackData = e->mutable_countryfightbackdata();
	PVPFightUser* pMsgAtk = pFightBackData->mutable_atkuser();
	PVPFightUser* pMsgDef = pFightBackData->mutable_defuser();

	//nUserID为攻击方玩家
	GCG_CountryOpt* pArgs	= e->mutable_countryopt();
	int nGameID = pArgs->atkgamesrvid();
	DB_C_User* pDBAtkUser	= pRDH->GetDBCityUser(nUserID);
	CCity* pAtkCity			= pRDH->GetCity(pMsgAtk->city());
	CCity* pDefCity			= pRDH->GetCity(pMsgDef->city());
	CCountry* pDefCountry	= pRDH->GetCountry(pMsgDef->country());
	int64 nDefID = 0;
	safe_atoll(pMsgDef->userid(),nDefID);
	DB_C_User* pDBDefUser = pRDH->GetDBCityUser(nDefID);

	if(pDBAtkUser==NULL||pDBDefUser==NULL||pAtkCity==NULL||pDefCity==NULL||pDefCountry==NULL)
		return;

	//判断玩家是否是不同阵营
	bool bSameLeague = pRDH->CheckSameLeague(pDBAtkUser,pDBDefUser);

	pArgs->set_bsameleague(bSameLeague);
	//被攻击玩家回主城
	CCity* pDefOldCity = pDefCity;
	CCity* pDefNewCity = pDefCountry->GetCapital();
	bool bSuc = pRDH->UserChgCity(pDBDefUser,pDefOldCity->GetCityID(),pDefNewCity->GetCityID());
	if(bSuc)
	{
		pRDH->markUserLstDirty(nDefID);
		if(pDBDefUser->officerposition()>0)
		{
			//发送给该国家的所有玩家更新官员信息
			//eh_->SendOfficerRightInfo2CountryUser(pDBDefUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
		}
		//发送该城市的所有官员使用的权限
		eh_->SendOfficerRightUsed(pRDH,pDefNewCity,nUserID);
	}
	pFightBackData->set_enemyback(true);

	DB_C_BloodBtl* pDBBloodBtl = pRDH->GetLastDBBloodBtl(pMsgAtk->city());
	if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0&&pDBBloodBtl->overtime()<=0)
	{//正在进行血战,保存战报
		DB_C_BloodBtlStep* pStep = NULL;
		if(pDBBloodBtl->btlstep_size() < 30)
		{
			pStep = pDBBloodBtl->add_btlstep();
		}
		else
		{
			pStep = pDBBloodBtl->mutable_btlstep(0);
			for(int i=1;i<pDBBloodBtl->btlstep_size();i++)
			{
				if(pDBBloodBtl->mutable_btlstep(i)->time()<pStep->time())
				{
					pStep = pDBBloodBtl->mutable_btlstep(i);
				}
			}
		}
		if(pStep)
		{
			pStep->set_time(time(NULL));
			pStep->set_atkid(pMsgAtk->userid());
			pStep->set_atkname(pMsgAtk->name());
			pStep->set_atkcountry(pMsgAtk->country());
			pStep->set_defid(pMsgDef->userid());
			pStep->set_defname(pMsgDef->name());
			pStep->set_defcountry(pMsgDef->country());

			pStep->set_winid(pMsgAtk->userid());
			CCity* pSaveCity = pRDH->GetCity(pDBBloodBtl->atkcity());
			if(pSaveCity)
			{
				pRDH->markCityDirty(pSaveCity);
			}
		}
	
	}

	bool bSendBloodOver = false;
	if(pMsgAtk->morale()>pMsgAtk->costmorale())
	{
		bSuc = CheckCaptureCity(pRDH,pDBAtkUser,pAtkCity,pDefCity);
		if(bSuc)
		{
			eh_->SendCountryLiteInfo();
			AddCountryReport(pMsgAtk,pMsgDef,Cntr_Report_Capture,pRDH,nGameID);
			if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0&&pDBBloodBtl->overtime()<=0)
			{//该城市在发生血战，并且血战结束
				pDBBloodBtl->set_wincity(pAtkCity->GetCityID());
				pDBBloodBtl->set_overtime(time(NULL));
				if(pAtkCity->GetCityID() == pDBBloodBtl->atkcity())
				{
					pDBBloodBtl->set_atkleftcnt(pAtkCity->GetUserCnt());
					pDBBloodBtl->set_defleftcnt(0);
				}
				else
				{
					pDBBloodBtl->set_atkleftcnt(0);
					pDBBloodBtl->set_defleftcnt(pAtkCity->GetUserCnt());
				}
				pRDH->SetBloodBtlStep(pDBBloodBtl->atkcity(),CBloodBtlStep_Over);

				AddCountryReport(pDBBloodBtl,pRDH);
				bSendBloodOver = true;
				SYS_LOG((int64)pDBBloodBtl->launchuserid(),LT_CNTR_BloodBtlEnd,pRDH->GetRegion(),0,pDBBloodBtl->atkcountry()<<pDBBloodBtl->atkcity()<<pDBBloodBtl->defcountry()<<pDBBloodBtl->defcity()<<pDBBloodBtl->wincity());
			}
			//如果成功占领buff城市，需要重新设置城防开启时间
			if(CountryInfoCfg::Instance().IsBuffCity(pDefCity->GetCityID())){
				DB_C_CityBuffData *pBuffData = pRDH->GetCityBuffData();
				if(pBuffData){
					for(int i = 0; i < pBuffData->randombuffs_size(); i++){
						DB_C_RandomBuff *pBuff = pBuffData->mutable_randombuffs(i);
						if(pBuff && pBuff->cityid() == pDefCity->GetCityID()){
							pBuff->set_activetm(int(time(NULL)));
							eh_->SendCityBuffInfo();
							break;
						}
					}
				}
			}
		}
		pFightBackData->set_capturecity(bSuc);
	}
	eh_->sendEventToGamed(e,nGameID);
	if(bSendBloodOver&&pDBBloodBtl)
	{
		eh_->SendBloodBtlInfo(pRDH->GetRegion(),-1,pDBBloodBtl->atkcity(),pDBBloodBtl->defcity(),true);
		SendBloodWinRedward(pRDH,pAtkCity,pDBBloodBtl,pDBAtkUser->userid());
	}
}

void DealCountryEvent::AttackNPCOver(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID)
{

	Country_FightBackData* pFightBackData = e->mutable_countryfightbackdata();
	PVPFightUser* pMsgAtk = pFightBackData->mutable_atkuser();
	PVPFightUser* pMsgDef = pFightBackData->mutable_defuser();
	GCG_CountryOpt* pArgs = e->mutable_countryopt();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	CCity* pSelfCity    = pRDH->GetCity(pMsgAtk->city());
	CCity* pTarCity		= pRDH->GetCity(pMsgDef->city());
	if(pDBCUser==NULL||pSelfCity==NULL||pTarCity==NULL)
		return;
	if(!CountryInfoCfg::Instance().IsNearCity(pMsgAtk->city(),pMsgDef->city()))
	{//game端已做客户端判断
		return;
	}
	int64 nDefID = 0;
	safe_atoll(pMsgDef->userid(),nDefID);
	int nIdx = nDefID%COUNTRY_NPC_RATE;
	int nNPCID = (int)(nDefID/COUNTRY_NPC_RATE);
	
	int nError = 0;
	if(pTarCity->GetCountry()!=pMsgDef->country()||
		nIdx >= pTarCity->GetDBCity()->npc_size())
	{//城市已经被占领
		pArgs->set_ret(15);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	DB_C_NPC* pDBNPC = pTarCity->GetDBCity()->mutable_npc(nIdx);
	if(pDBNPC==NULL||pDBNPC->npcid()<=0||pDBNPC->morale()<=0)
	{//该NPC已经被挂掉
		pArgs->set_ret(15);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	pArgs->set_ret(0);
	pDBNPC->set_morale(pDBNPC->morale()-pMsgDef->costmorale());
	if(pDBNPC->morale()<=0)
	{//NPC被击败
		//从列表中删除NPC
		pDBNPC->Clear();
		vector<DB_C_NPC*>::iterator it1;
		for(it1 = pTarCity->m_lstNPC.begin();it1!=pTarCity->m_lstNPC.end();)
		{
			DB_C_NPC* pFindDBNPC = *it1;
			if(pFindDBNPC->npcid()<=0)
			{
				it1 = pTarCity->m_lstNPC.erase(it1);
				continue;
			}
			it1++;
		}
		pFightBackData->set_enemyback(true);
	}
	pMsgDef->set_morale(pDBNPC->morale());
	
	if(pTarCity->GetUserCnt()<=0&&
		pMsgAtk->morale()>pMsgAtk->costmorale())
	{
		bool bSuc = CheckCaptureCity(pRDH,pDBCUser,pSelfCity,pTarCity);
		if(bSuc)
		{
			eh_->SendCountryLiteInfo();
			AddCountryReport(pMsgAtk,pMsgDef,Cntr_Report_Capture,pRDH,nGameID);
			pFightBackData->set_capturecity(true);
			//如果成功占领buff城市，需要重新设置城防开启时间
			if(CountryInfoCfg::Instance().IsBuffCity(pTarCity->GetCityID())){
				DB_C_CityBuffData *pBuffData = pRDH->GetCityBuffData();
				if(pBuffData){
					for(int i = 0; i < pBuffData->randombuffs_size(); i++){
						DB_C_RandomBuff *pBuff = pBuffData->mutable_randombuffs(i);
						if(pBuff && pBuff->cityid() == pTarCity->GetCityID()){
							pBuff->set_activetm(int(time(NULL)));
							eh_->SendCityBuffInfo();
							break;
						}
					}
				}
			}
		}
	}

	eh_->sendEventToGamed(e,nGameID);

	pRDH->markCityDirty(pTarCity);

}

void DealCountryEvent::OptLaunchBloodBtl(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceCountryOpt* pRceOpt = e->mutable_ce_rcecountryopt();
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();

	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	CCountry* pAtkCountry = pRDH->GetCountry(pDBCUser->countryid());
	CCity* pAtkCity    = pRDH->GetCity(pRceOpt->atktargetcity());	//发起城市不一定是玩家所在城市
	CCity* pTarCity		= pRDH->GetCity( pSrvOpt->tarcity());
	if(pDBCUser==NULL||pTarCity==NULL||pTarCity==NULL||pAtkCountry==NULL)
		return;
	if(pAtkCity->GetCountry()!=pAtkCountry->GetCountryID())
	{
		pSrvOpt->set_ret(6);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	CCountry* pTarCountry = pRDH->GetCountry(pTarCity->GetCountry());
	if(pTarCountry==NULL||!CountryInfoCfg::Instance().IsUserCountry(pTarCity->GetCountry()))
		return;

	CountryDataHandler* pDH= eh_->getDataHandler();
	e->set_state(Status_Country_To_Game);
	
	if(pAtkCity->GetCountry()==pTarCity->GetCountry())
	{//是同一个国家的城市
		pSrvOpt->set_ret(34);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(pRDH->HasLaunchBloodBtl(pAtkCountry->GetCountryID()))
	{//game country 数据不一致
		eh_->SendBloodBtlInfo(-1,nGameID);
		pSrvOpt->set_ret(32);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	time_t ltNow = time(NULL);
	DB_C_BloodBtl* pDBBloodBtlFrom = pRDH->GetLastDBBloodBtl(pAtkCity->GetCityID());
	if(pDBBloodBtlFrom&&pDBBloodBtlFrom->launchtime()>0)
	{
		if(pDBBloodBtlFrom->overtime()<=0)
		{
			eh_->SendBloodBtlInfo(-1,nGameID);
			pSrvOpt->set_ret(39);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		else if(pDBBloodBtlFrom->overtime()+CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pDBBloodBtlFrom->type())>ltNow)
		{
			eh_->SendBloodBtlInfo(-1,nGameID);
			pSrvOpt->set_ret(36);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}
	DB_C_BloodBtl* pDBBloodBtlTar = pRDH->GetLastDBBloodBtl(pTarCity->GetCityID());
	if(pDBBloodBtlTar&&pDBBloodBtlTar->launchtime()>0)
	{
		if(pDBBloodBtlTar->overtime()<=0)
		{
			eh_->SendBloodBtlInfo(-1,nGameID);
			pSrvOpt->set_ret(39);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		else if(pDBBloodBtlTar->overtime()+CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pDBBloodBtlTar->type())>ltNow)
		{
			eh_->SendBloodBtlInfo(-1,nGameID);
			pSrvOpt->set_ret(36);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}
	//血战开启
	DB_C_BloodBtl* pDBBloodBtl1 = pRDH->GetDBBloodBtl(pAtkCity,true);
	pDBBloodBtl1->Clear();
	pDBBloodBtl1->set_launchtime(time(NULL));
	pDBBloodBtl1->set_launchuserid(nUserID);
	pDBBloodBtl1->set_launchname(pSrvOpt->selfname());

	pDBBloodBtl1->set_atkcountry(pAtkCountry->GetCountryID());
	pDBBloodBtl1->set_atkcity(pAtkCity->GetCityID());

	pDBBloodBtl1->set_defcountry(pTarCity->GetCountry());
	pDBBloodBtl1->set_defcity(pTarCity->GetCityID());
	pRDH->SetBloodBtlStep(pAtkCity->GetCityID(),CBloodBtlStep_Start);
	//分发到所有game
	map<int,bool> mapCity;
	mapCity[pDBBloodBtl1->atkcity()] = true;
	mapCity[pDBBloodBtl1->defcity()] = true;
	eh_->SendBloodBtlInfo(pRDH->GetRegion(),-1,0,0,false,&mapCity);
	//返回game端,在上个分发命令中，game已接到数据
	pSrvOpt->set_ret(0);
	eh_->sendEventToGamed(e,nGameID);

	pRDH->markCityDirty(pAtkCity);

	SYS_LOG(nUserID,LT_CNTR_BloodBtlStart,pRDH->GetRegion(),0,pDBBloodBtl1->atkcountry()<<pDBBloodBtl1->atkcity()<<pDBBloodBtl1->defcountry()<<pDBBloodBtl1->defcity());

	//清空一次过期血战列表
	pRDH->ClearUnuseBloodBtl();
	
}


void DealCountryEvent::CheckAutoBloodBtlStart(CountryRgnDatHandler* pRDH)
{
	//if(!CountryInfoCfg::Instance().GetAutoBloodBtlTbl()->m_bCanUse)
	//	return;
	time_t ltNow = time(NULL);
	if(!CountryInfoCfg::Instance().IsAutoBloodBtlTime(ltNow))
	{
		if(pRDH->HasLaunchAutoBloodBtl())
		{
			pRDH->SetHasLaunchAutoBloodBtl(false);
		}
		return;
	}
	if(pRDH->HasLaunchAutoBloodBtl())
		return;
#ifdef WIN32
	if(pRDH->GetRegion()!=0)
		return;
#endif
	//发起自动血战
	int nKey[C_UserCoutryEnd];
	memset(nKey,0,C_UserCoutryEnd*sizeof(int));
	
	//将国家随机
	vector<int> lstCountryID;
	for(int i=C_Start;i<C_UserCoutryEnd;i++)
	{
		lstCountryID.push_back(i);
	}
	for(int i=0;i<lstCountryID.size();i++)
	{
		int nIdx = rand()%lstCountryID.size();
		int nTmp = lstCountryID[i];
		lstCountryID[i]		= lstCountryID[nIdx];
		lstCountryID[nIdx]	= nTmp;
	}
	map<int,bool> mapCity;
	for(int i=0;i<lstCountryID.size();i++)
	{
		int nCountryID = lstCountryID[i];
		if(nKey[nCountryID]>0)//已经有血战了
			continue;

		int nMinKey = 999; 
		vector<SelCity4BloodBtl*> lstSel;
		lstSel.clear();
		
		
		CCountry* pCountry = pRDH->GetCountry(lstCountryID[i]);
		if(pCountry==NULL)
			continue;
		for(int j=0;j<pCountry->m_lstCity.size();j++)
		{
			//己方城市
			CCity* pCity = pCountry->m_lstCity[j];
			const CityDataTbl* pCityTbl = CountryInfoCfg::Instance().GetCityTbl(pCity->GetCityID());
			if(pCityTbl==NULL)
				continue;
			if(!pCityTbl->m_bCanCapure)
				continue;
			//正在发生血战的城市剔除
			DB_C_BloodBtl* pTmpBtl = pRDH->GetLastDBBloodBtl(pCity->GetCityID());
			if(pTmpBtl && pTmpBtl->wincity() <= 0)
				continue;

			for(int k=0;k<pCityTbl->m_lstCity.size();k++)
			{//敌方城市
				int nNearID = pCityTbl->m_lstCity[k];
				//if(nNearID >= pCity->GetCityID())
				//	continue;
				if(mapCity.find(nNearID)!=mapCity.end())
					continue;
				CCity* pNearCity = pRDH->GetCity(nNearID);
				if(pNearCity==NULL)
					continue;
				const CityDataTbl* pNearCityTbl = CountryInfoCfg::Instance().GetCityTbl(pNearCity->GetCityID());
				if(pNearCityTbl==NULL)
					continue; 
				if(!pNearCityTbl->m_bCanCapure)
					continue;
				if(pNearCity->GetCountry()==pCity->GetCountry()||pNearCity->GetCountry()>=C_NPCStart)
					continue;
				if(nMinKey < nKey[pNearCity->GetCountry()]) //已经有了权值更低的选项
					continue;
				//正在发生血战的城市剔除
				DB_C_BloodBtl* pTmpBtl = pRDH->GetLastDBBloodBtl(pNearCity->GetCityID());
				if(pTmpBtl && pTmpBtl->wincity() <= 0)
					continue;
				//放入列表
				SelCity4BloodBtl* pSelData = new SelCity4BloodBtl();
				pSelData->pCity1	= pCity;
				pSelData->pCity2	= pNearCity;
				pSelData->nKey		= nKey[pNearCity->GetCountry()];
				lstSel.push_back(pSelData);
				if(nMinKey > nKey[pNearCity->GetCountry()])
				{
					nMinKey = nKey[pNearCity->GetCountry()];
				}
			}
		}
		if(lstSel.size()<=0)	//无符合条件城市
			continue;		
		vector<SelCity4BloodBtl*> lstUse;
		for(int j=0;j<lstSel.size();j++)
		{//只取权值最低的列表
			SelCity4BloodBtl* pData = lstSel[j];
			if(pData->nKey > nMinKey)
				continue;
			lstUse.push_back(pData);
		}
		if(lstUse.size()<=0)
			continue;
		int nIdx = rand()%lstUse.size();
		SelCity4BloodBtl* pSelData = lstUse[nIdx];
		CCity* pAtkCity = pSelData->pCity1;
		CCity* pTarCity = pSelData->pCity2;
		nKey[pAtkCity->GetCountry()]++;
		nKey[pTarCity->GetCountry()]++;

		//清空列表，释放内存
		vector<SelCity4BloodBtl*>::iterator iter;
		for(iter = lstSel.begin();iter!=lstSel.end();iter++)
		{
			delete *iter;
		}
		lstUse.clear();
		lstSel.clear();
		pSelData = NULL;

		//血战开启
		DB_C_BloodBtl* pDBBloodBtl1 = pRDH->GetDBBloodBtl(pAtkCity,true);
		pDBBloodBtl1->Clear();
		pDBBloodBtl1->set_launchtime(time(NULL));
		pDBBloodBtl1->set_launchuserid(0);
		pDBBloodBtl1->set_launchname("");
 
		pDBBloodBtl1->set_atkcountry(pAtkCity->GetCountry());
		pDBBloodBtl1->set_atkcity(pAtkCity->GetCityID());

		pDBBloodBtl1->set_defcountry(pTarCity->GetCountry());
		pDBBloodBtl1->set_defcity(pTarCity->GetCityID());
		
		pDBBloodBtl1->set_wincity(0);
		pDBBloodBtl1->set_overtime(0);

		pDBBloodBtl1->set_type(CBT_SYSLaunch);

		pRDH->SetBloodBtlStep(pAtkCity->GetCityID(),CBloodBtlStep_Start);
		////分发到所有game
		//eh_->SendBloodBtlInfo(pRDH->GetRegion(),-1,pDBBloodBtl1->atkcity(),pDBBloodBtl1->defcity());
		mapCity[pDBBloodBtl1->atkcity()] = true;
		mapCity[pDBBloodBtl1->defcity()] = true;

		pRDH->markCityDirty(pAtkCity);

		SYS_LOG(0,LT_CNTR_BloodBtlStart,pRDH->GetRegion(),0,pDBBloodBtl1->atkcountry()<<pDBBloodBtl1->atkcity()<<pDBBloodBtl1->defcountry()<<pDBBloodBtl1->defcity());
	}
	//分发到所有game
	eh_->SendBloodBtlInfo(pRDH->GetRegion(),-1,0,0,false,&mapCity);

	pRDH->SetHasLaunchAutoBloodBtl(true);
	//清空一次过期血战列表
	pRDH->ClearUnuseBloodBtl();
}

void DealCountryEvent::CheckCountryBloodBtl()
{
	CountryDataHandler* pDH= eh_->getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();

	time_t ltNow = time(NULL);
	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(pRDH==NULL)
			continue;
		map<int,DB_C_BloodBtl*>& mapBtlLst = pRDH->GetBloobBtlLst();
		map<int,DB_C_BloodBtl*>::iterator iter;
		for(iter = mapBtlLst.begin();iter!=mapBtlLst.end();iter++)
		{
			DB_C_BloodBtl* pDBBloodBtl = iter->second;
			if(pDBBloodBtl==NULL||pDBBloodBtl->launchtime()<=0||pDBBloodBtl->wincity()>0||pDBBloodBtl->overtime()>0)
				continue;
			//正在进行中的血战
			int emStep = pRDH->GetBloodBtlStep(pDBBloodBtl->atkcity());
			if(emStep == CBloodBtlStep_Start)
			{
				int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
				
				if(pDBBloodBtl->launchtime()+nBanAtkTime <= ltNow)
				{//进入PK阶段
					pRDH->SetBloodBtlStep(pDBBloodBtl->atkcity(),CBloodBtlStep_PK);
					CCountry*	pAtkCountry = pRDH->GetCountry(pDBBloodBtl->atkcountry());
					CCity*		pAtkCity	= pRDH->GetCity(pDBBloodBtl->atkcity());
					CCountry*	pDefCountry = pRDH->GetCountry(pDBBloodBtl->defcountry());
					CCity*		pDefCity	= pRDH->GetCity(pDBBloodBtl->defcity());
					if(pAtkCountry&&pAtkCity&&pDBBloodBtl->atktotalcnt()<=0)
					{	
						pDBBloodBtl->set_atktotalcnt(pAtkCity->GetUserCnt());
					}
					if(pDefCountry&&pDefCity&&pDBBloodBtl->deftotalcnt()<=0)
					{
						pDBBloodBtl->set_deftotalcnt(pDefCity->GetUserCnt());
					}
				}
			}
			else if(emStep == CBloodBtlStep_PK)
			{	
				int nTotalAtkTime = CountryInfoCfg::Instance().GetBloodBtlTotalAtkTime(pDBBloodBtl->type());
				if(pDBBloodBtl->wincity()<=0&&pDBBloodBtl->launchtime()+nTotalAtkTime + 30 <= ltNow)
				{//结算
					OptBalanceBloodBtl(pRDH,pDBBloodBtl->atkcity());
					pRDH->SetBloodBtlCacuTime(pDBBloodBtl->atkcity());
				}
			}
			else if(emStep == CBloodBtlStep_Balance)
			{
				int nLastCacu = pRDH->GetBloodBtlCacuTime(pDBBloodBtl->atkcity());
				if( pDBBloodBtl->wincity()<=0&& nLastCacu + 60 <= ltNow)
				{//超过1分钟还没结算完，重新结算，用于game重启等情况
					OptBalanceBloodBtl(pRDH,pDBBloodBtl->atkcity());
					pRDH->SetBloodBtlCacuTime(pDBBloodBtl->atkcity());
				}	
				
			}
		}
	}
}

void DealCountryEvent::OptBalanceBloodBtl(CountryRgnDatHandler* pRDH,int nCityID)
{
	CountryDataHandler* pDH= eh_->getDataHandler();
	CCity*		pAtkCity	= pRDH->GetCity(nCityID);
	if(pAtkCity==NULL)
		return;
	DB_C_BloodBtl* pDBBloodBtl = pRDH->GetDBBloodBtl(pAtkCity);
	if(pDBBloodBtl==NULL)
		return;
	CCountry*	pAtkCountry = pRDH->GetCountry(pDBBloodBtl->atkcountry());

	CCountry*	pDefCountry = pRDH->GetCountry(pDBBloodBtl->defcountry());
	CCity*		pDefCity	= pRDH->GetCity(pDBBloodBtl->defcity());

	if(pAtkCountry&&pAtkCity&&pDefCountry&&pDefCity)
	{
		pRDH->SetBloodBtlStep(nCityID,CBloodBtlStep_Balance);
		Event evn;
		evn.set_cmd(EVENT_COUNTRY_BLOODBTL_BALLANCE);
		evn.set_uid(pDBBloodBtl->launchuserid());
		evn.set_time(-1);
		evn.set_state(Status_Country_To_Game);
		GCG_CountryBloodBtlCacu* pCacu	= evn.mutable_bloodbtlcacu();
		GCG_CountryOpt* pSrvOpt			= evn.mutable_countryopt();
		pCacu->set_atkcity(pDBBloodBtl->atkcity());
		pCacu->set_defcity(pDBBloodBtl->defcity());
		//去发起血战的玩家game结算

		int nCnt = 0;
		string strUserLst = "";
		pRDH->FillUserIDLst(pAtkCity,-1,pSrvOpt,true);
		nCnt += pSrvOpt->userid_size();
		for(int i=0;i<pSrvOpt->userid_size();i++)
		{
			int64 nGetID = pSrvOpt->userid(i);
			strUserLst += toString(nGetID)+ "," ;
			//pSrvOpt->add_atkidlst(pSrvOpt->userid(i));
		}
		pSrvOpt->clear_userid();
		strUserLst += "-1";
		strUserLst += ",";
		pRDH->FillUserIDLst(pDefCity,-1,pSrvOpt,true);
		nCnt += pSrvOpt->userid_size();
		for(int i=0;i<pSrvOpt->userid_size();i++)
		{
			int64 nGetID = pSrvOpt->userid(i);
			strUserLst += toString(nGetID)+ "," ;
			//pSrvOpt->add_defidlst(pSrvOpt->userid(i));
		}
		pSrvOpt->clear_userid();
		pCacu->set_usercnt(nCnt);
		pCacu->set_region(pRDH->GetRegion());
		pCacu->set_blooduserlst(strUserLst);
		int gid = pDH->getGamedIdByUserId(evn.uid());
		eh_->sendEventToGamed(&evn,gid);


	}
}

void DealCountryEvent::HandleountryBloodBtlOver(Event* e)
{
	CountryDataHandler* pDH= eh_->getDataHandler();
	GCG_CountryBloodBtlCacu* pCacu = e->mutable_bloodbtlcacu();
	int nRegion = pCacu->region();
	CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
	if(pDH==NULL)
		return;
	CCity*		pAtkCity	= pRDH->GetCity(pCacu->atkcity());	
	if(pAtkCity==NULL)
		return;
	DB_C_BloodBtl* pDBBloodBtl = pRDH->GetDBBloodBtl(pAtkCity);
	if(pDBBloodBtl==NULL||pDBBloodBtl->wincity()>0)
		return;
	CCountry*	pAtkCountry = pRDH->GetCountry(pDBBloodBtl->atkcountry());
	
	CCountry*	pDefCountry = pRDH->GetCountry(pDBBloodBtl->defcountry());
	CCity*		pDefCity	= pRDH->GetCity(pDBBloodBtl->defcity());
	if(pAtkCountry&&pAtkCity&&pDefCountry&&pDefCity)
	{//结算
		if(pAtkCountry->GetCapital()==NULL || pDefCountry->GetCapital() == NULL )
			return;
		pRDH->SetBloodBtlStep(pDBBloodBtl->atkcity(),CBloodBtlStep_Over);
		
		Event evn;
		evn.set_cmd(EVENT_COUNTRY_BLOODBTL_LOSE);
		evn.set_state(Status_Game_To_Country);
		evn.set_time(0);
		evn.set_uid(0);
		

		CCity* pWinCity = NULL;
		CCity* pLoseCity = NULL;
		CCountry* pWinCountry = NULL;
		CCountry* pLoseCountry = NULL;
		if(pCacu->wincity()==pDBBloodBtl->atkcity())
		{//攻方胜利,守方城市所有人回老家
			pWinCountry = pAtkCountry;
			pWinCity	= pAtkCity;
			pLoseCountry= pDefCountry;
			pLoseCity	= pDefCity;
		}
		else
		{//守方胜利
			pWinCountry = pDefCountry;
			pWinCity	= pDefCity;
			pLoseCountry= pAtkCountry;
			pLoseCity	= pAtkCity;
		}
		GCG_CountryOpt* pArgs = evn.mutable_countryopt();
		pArgs->set_selfcity(pLoseCity->GetCityID());
		pArgs->set_tarcity(pLoseCountry->GetCapital()->GetCityID());
		pArgs->set_tarcountry(pLoseCountry->GetCountryID());

		pDBBloodBtl->set_wincity(pWinCity->GetCityID());
		pDBBloodBtl->set_overtime(time(NULL));
		if(pWinCity == pAtkCity)
		{
			pDBBloodBtl->set_atkleftcnt(pWinCity->GetUserCnt());
			pDBBloodBtl->set_defleftcnt(0);
		}
		else
		{
			pDBBloodBtl->set_atkleftcnt(0);
			pDBBloodBtl->set_defleftcnt(pWinCity->GetUserCnt());
		}
		
		CityBloodBattle* pBld = evn.mutable_bloodbtlredward();
		pRDH->FillBloodBtl(pDBBloodBtl,pBld,false);

		//eh_->sendEventToCountry(&evn,nRegion);
		bool bHasOfficer = false;
		for(int i=0;i<CITY_USER_GROUP;i++)
		{
			for(int j=0;j<(int)pLoseCity->m_lstUser[i].size();j++)
			{
				DB_C_User* pDBCUser = pLoseCity->m_lstUser[i][j];
				if(pDBCUser==NULL)
				{
					continue;
				}
				if(pDBCUser->cityid()!=pLoseCity->GetCityID())
				{
					continue;
				}
				//是否是官员
				if(pDBCUser->officerposition()>0)
				{
					bHasOfficer = true;
				}
				//所有人回首都，这里没有调用换城市函数
				bool bIsChampion = pRDH->IsChampion(pDBCUser->userid());
				pLoseCountry->GetCapital()->UserEnterCity(pDBCUser,bIsChampion);
				pRDH->markUserLstDirty(pDBCUser->userid());

				evn.set_uid(pDBCUser->userid());
				eh_->sendEventToGamed(&evn,pDH->getGamedIdByUserId(pDBCUser->userid()));

				//发送该城市的所有官员使用的权限
				eh_->SendOfficerRightUsed(pRDH,pRDH->GetCity(pDBCUser->cityid()),pDBCUser->userid());
			}
			pLoseCity->m_lstUser[i].clear();
		}
		if(bHasOfficer)
		{
			//发送给该国家的所有玩家更新官员信息
			//eh_->SendOfficerRightInfo2CountryUser(pLoseCountry->GetCountryID(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
		}
		if(CheckCaptureCity(pRDH,NULL,pWinCity,pLoseCity))
		{
			eh_->SendCountryLiteInfo();
			AddCountryReport(pDBBloodBtl,pRDH);
			//SendBloodWinRedward(pRDH,pAtkCity,pDBBloodBtl);
		}

		pRDH->markCityDirty(pAtkCity);
		pRDH->markCityDirty(pDefCity);
		SYS_LOG((int64)pDBBloodBtl->launchuserid(),LT_CNTR_BloodBtlEnd,pRDH->GetRegion(),0,pDBBloodBtl->atkcountry()<<pDBBloodBtl->atkcity()<<pDBBloodBtl->defcountry()<<pDBBloodBtl->defcity()<<pDBBloodBtl->wincity());
	
		eh_->SendBloodBtlInfo(nRegion,-1,pDBBloodBtl->atkcity(),pDBBloodBtl->defcity(),true);
		SendBloodWinRedward(pRDH,pWinCity,pDBBloodBtl);
	}
}

void DealCountryEvent::SendBloodWinRedward(CountryRgnDatHandler* pRDH,CCity* pWinCity,DB_C_BloodBtl* pDBBloodBtl,int64 nCaputreUserID/* =0 */)
{
	if(pRDH==NULL||pWinCity==NULL||pDBBloodBtl==NULL)
		return;
	CountryDataHandler* pDH= eh_->getDataHandler();

	Event evn;
	evn.set_cmd(EVENT_COUNTRY_BLOODBTL_REWARD);
	evn.set_state(Status_Game_To_Country);
	evn.set_time(0);
	evn.set_uid(0);
	CityBloodBattle* pBld = evn.mutable_bloodbtlredward();
	pRDH->FillBloodBtl(pDBBloodBtl,pBld,false);
	for(int i=0;i<CITY_USER_GROUP;i++)
	{
		for(int j=0;j<(int)pWinCity->m_lstUser[i].size();j++)
		{
			DB_C_User* pDBCUser = pWinCity->m_lstUser[i][j];
			if(pDBCUser==NULL)
			{
				continue;
			}
			if(pDBCUser->cityid()!=pWinCity->GetCityID())
			{
				continue;
			}
			evn.set_uid(pDBCUser->userid());
			eh_->sendEventToGamed(&evn,pDH->getGamedIdByUserId(pDBCUser->userid()));
		}
	}

	if(nCaputreUserID>0)
	{
		evn.set_uid(nCaputreUserID);
		eh_->sendEventToGamed(&evn,pDH->getGamedIdByUserId(nCaputreUserID));
	}

	if(pWinCity->GetCityID() == pDBBloodBtl->wincity())
	{//攻方胜利
		evn.set_cmd(EVENT_COUNTRY_BLOODBTL_LAUNCHREWARD);
		evn.set_uid(pDBBloodBtl->launchuserid());
		eh_->sendEventToGamed(&evn,pDH->getGamedIdByUserId(pDBBloodBtl->launchuserid()));
	}
}


void DealCountryEvent::OptGotoCity(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceCountryOpt* pRceOpt = e->mutable_ce_rcecountryopt();
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();

	CountryDataHandler* pDH= eh_->getDataHandler();

	e->set_state(Status_Country_To_Game);

	int nOldCity = pSrvOpt->selfcity();
	int nNewCity = pSrvOpt->tarcity();
	CCity* pOldCity = pRDH->GetCity(nOldCity);
	CCity* pNewCity = pRDH->GetCity(nNewCity);
	DB_C_User* pDBUser=pRDH->GetDBCityUser(nUserID);
	//判断数据是否一致
	if(nOldCity == 0||pNewCity==NULL||pDBUser==NULL)
		return;
	//判断是否有通路
	int nNeedThew = 0;
	int nRet = pRDH->CheckHasWay(nUserID,pOldCity,pNewCity,nNeedThew);
	if(nRet != 0)
	{
		pSrvOpt->set_ret(nRet);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(nNeedThew>pSrvOpt->hasthew())
	{
		pSrvOpt->set_ret(9);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	time_t ltNow = time(NULL);
	//判断血战信息
	DB_C_BloodBtl* pDBBloodBtl = pRDH->GetLastDBBloodBtl(nOldCity);
	if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0)
	{//移出城市血战信息
		if(pDBBloodBtl->wincity()<=0)
		{//该国家血战未结束,血战进行中，不可进入或者离开
			int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
			if(pDBBloodBtl->launchtime()+nBanAtkTime<ltNow)
			{
				bool bCanMove = false;
				if(pDBBloodBtl->atkcity() == nNewCity && pDBBloodBtl->defcity() == nOldCity)
				{
					bCanMove = true;
				}
				else if(pDBBloodBtl->defcity() == nNewCity && pDBBloodBtl->atkcity() == nOldCity)
				{
					bCanMove = true;
				}
				if(!bCanMove)
				{
					pSrvOpt->set_ret(38);
					eh_->sendEventToGamed(e,nGameID);
					return;
				}
			}
		}
	}
	pDBBloodBtl = pRDH->GetLastDBBloodBtl(nNewCity);
	if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0)
	{//移入城市血战信息
		if(pDBBloodBtl->wincity()<=0)
		{//该国家血战未结束,血战进行中，不可进入或者离开
			int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
			if(pDBBloodBtl->launchtime()+nBanAtkTime<ltNow)
			{
				bool bCanMove = false;
				if(pDBBloodBtl->atkcity() == nNewCity && pDBBloodBtl->defcity() == nOldCity)
				{
					bCanMove = true;
				}
				else if(pDBBloodBtl->defcity() == nNewCity && pDBBloodBtl->atkcity() == nOldCity)
				{
					bCanMove = true;
				}
				if(!bCanMove)
				{
					pSrvOpt->set_ret(38);
					eh_->sendEventToGamed(e,nGameID);
					return;
				}
			}
			else if(pNewCity->GetCountry()!=pOldCity->GetCountry())
			{
				int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
				if(pDBBloodBtl->launchtime()+nBanAtkTime>=ltNow)
				{//血战准备期，他国玩家不可进入
					pSrvOpt->set_ret(38);
					eh_->sendEventToGamed(e,nGameID);
					return;
				}
			}
		}
		else if(pNewCity->GetCountry()!=pOldCity->GetCountry())
		{//不是一个国家
			int nCityProtectTm = CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pDBBloodBtl->type());
			if(pDBBloodBtl->overtime()+nCityProtectTm>ltNow)
			{
				pSrvOpt->set_ret(36);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
		}
	}

	bool bSuc = false;
	if(pNewCity->GetCountry()!=pOldCity->GetCountry())
	{//不是同一个国家

		int nTarCountryID = pNewCity->GetCountry();
		bSuc = CheckCaptureCity(pRDH,pDBUser,pOldCity,pNewCity);
		if(bSuc)
		{//成功占城
			eh_->SendCountryLiteInfo();
			//如果是buff城市，重新计算激活时间，并同步给game
			if(pRDH->OnBuffCityChangeCountry(pNewCity->GetCityID())){
				eh_->SendCityBuffInfo();
			}

			CountryReport* pReport = pSrvOpt->mutable_report();
			pReport->set_defcountry(nTarCountryID);
			AddCountryReport(pReport,pRDH,nGameID); //加入战报
			CountryCapCity* pCap = pRseRet->mutable_capturecity();
			pCap->set_capturecity(pNewCity->GetCityID());
			pCap->set_oldcountryid(nTarCountryID);
			pCap->set_newcountryid(pOldCity->GetCountry());

			pDBBloodBtl = pRDH->GetLastDBBloodBtl(nNewCity);
			if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0&&pDBBloodBtl->overtime()<=0)
			{//该城市在发生血战，并且血战结束
				CCity* pWinCity = pOldCity;
				CCity* pLoseCity = pNewCity;
				pDBBloodBtl->set_wincity(pWinCity->GetCityID());
				pDBBloodBtl->set_overtime(time(NULL));
				if(pWinCity->GetCityID() == pDBBloodBtl->atkcity())
				{
					pDBBloodBtl->set_atkleftcnt(pOldCity->GetUserCnt());
					pDBBloodBtl->set_defleftcnt(0);
				}
				else
				{
					pDBBloodBtl->set_atkleftcnt(0);
					pDBBloodBtl->set_defleftcnt(pOldCity->GetUserCnt());
				}
				
				pRDH->SetBloodBtlStep(pDBBloodBtl->atkcity(),CBloodBtlStep_Over);
				pRDH->markCityDirty(pWinCity);
				AddCountryReport(pDBBloodBtl,pRDH);
				SYS_LOG((int64)pDBBloodBtl->launchuserid(),LT_CNTR_BloodBtlEnd,pRDH->GetRegion(),0,pDBBloodBtl->atkcountry()<<pDBBloodBtl->atkcity()<<pDBBloodBtl->defcountry()<<pDBBloodBtl->defcity()<<pDBBloodBtl->wincity());
				eh_->SendBloodBtlInfo(pRDH->GetRegion(),-1,pDBBloodBtl->atkcity(),pDBBloodBtl->defcity(),true);
				SendBloodWinRedward(pRDH,pWinCity,pDBBloodBtl,nUserID);
			}

		}
	}
	else
	{//同一个国家
		//更改玩家所在城市，此处先改country后改game，可能会有bug
		bSuc = pRDH->UserChgCity(pDBUser,nOldCity,nNewCity);
	}
	if(bSuc)
	{
		pRDH->markUserLstDirty(nUserID);
	}

	nRet = bSuc?0:1;
	pSrvOpt->set_ret(nRet);
	pSrvOpt->set_needthew(nNeedThew);
	CityData* pCityData = pRseRet->mutable_citydata();
	pCityData->set_cityid(pNewCity->GetCityID());
	pCityData->set_countryid(pNewCity->GetCountry());
	pCityData->set_usercnt(pNewCity->GetUserCnt());

	//发送该城市的所有官员使用的权限
	eh_->SendOfficerRightUsed(pRDH,pNewCity,nUserID);
	if(pDBUser->officerposition()>0)
	{
		//发送给该国家的所有玩家更新官员信息
		//eh_->SendOfficerRightInfo2CountryUser(pDBUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
	}
	eh_->sendEventToGamed(e,nGameID);
	return;	
}

void DealCountryEvent::HandleAdminOpt(CountryRgnDatHandler* pRDH,Admin_AdminOpt_Req* req)
{
	if(pRDH == NULL || req == NULL)
	{
		return;
	}

	switch(req->opttype())
	{
	case AOT_ADMIN_ARENAFIGHTSET:
		{
			int nType	=	req->itemtype();
			int nValue1 =	req->bldtype();
			int nValue2 =	req->armytype();

			if(nType == 9)
			{
				int nCountryId = nValue1;
				if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
				{
					CCountry* pCountry = pRDH->GetCountry(nCountryId);
					if(pCountry)
					{
						int64 nUid = req->uid();
						DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
						if(pDBUser && pDBUser->countryid() == nCountryId && pDBUser->arenaindex() > -1)
						{
							pCountry->ExChangePos(pRDH,nUid,pDBUser->arenaindex(),pCountry->GetRankUidByIndex(nValue2),nValue2);
							pRDH->markUserLstDirty(pDBUser->userid());
						}
					}
				}
			}

			if(nType == 10)
			{
				int nCountryId = nValue1;
				if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
				{
					CCountry* pCountry = pRDH->GetCountry(nCountryId);
					if(pCountry)
					{
						int64 nUid = req->uid();
						DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
						if(pDBUser && pDBUser->countryid() == nCountryId && pDBUser->arenaindex() > -1)
						{
							pCountry->ExChangePos(pDBUser,nValue2);
							pRDH->markUserLstDirty(pDBUser->userid());
						}
					}
				}
			}
			if(nType == 14)
			{
				int nCountryId = nValue1;
				if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
				{
					CCountry* pCountry = pRDH->GetCountry(nCountryId);
					if(pCountry)
					{
						int64 nUid = req->uid();
						DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
						if(pDBUser && pDBUser->countryid() == nCountryId)
						{
							pDBUser->set_lastarenaindex(nValue2);
							pRDH->markUserLstDirty(pDBUser->userid());
						}
					}
				}
			}
		}
		break;
	case AOT_ADMIN_WORLDARENAFIGHTSET:
		{
			int nType	=	req->itemtype();
			int nValue1 =	req->bldtype();

			if(nType == 6)
			{
				CCountry* pEarth = pRDH->GetEarthCountry();
				if(pEarth)
				{
					int64 nUid = req->uid();
					DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
					if(pDBUser && pDBUser->worldarenaindex() > -1)
					{
						pEarth->ChangeWorldArenaPos(pRDH,nUid,pDBUser->worldarenaindex(),pEarth->GetRankUidByIndex(nValue1),nValue1);
						pRDH->markUserLstDirty(pDBUser->userid());
					}
				}
			}
		}
		break;
	case AOT_ADMIN_HEROARENAFIGHTSET:
		{
			int nType	=	req->itemtype();
			int nValue1 =	req->bldtype();

			if(nType == 6)
			{
				CCountry* pEarth = pRDH->GetEarthCountry();
				if(pEarth)
				{
					int64 nUid = req->uid();
					DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
					if(pDBUser && pDBUser->heroarenaindex() > -1)
					{
						pEarth->ChangeHeroArenaPos(pRDH,nUid,pDBUser->heroarenaindex(),pEarth->GetHeroRankUidByIndex(nValue1),nValue1);
						pRDH->markUserLstDirty(pDBUser->userid());
					}
				}
			}
		}
		break;
	case AOT_ADMIN_SET_GUILDBLDLV:
		{//设置工会建筑等级
			int nGuildID= req->guildid();
			int nBldID	= req->bldid();
			int nLV		= req->level();
			CGuild* pGuild = pRDH->GetGuild(nGuildID);
			if(pGuild==NULL)
				return;
			int64 nUserID = req->uid();
			const GuildBldTbl* pGBTbl = GuildInfoCfg::Instance().GetGuildBuildingTbl(nBldID);
			if(pGBTbl==NULL||nLV<=0||nLV>pGBTbl->GetMaxLV())
				return;//game端已经判断
			DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
			for(int i=pDBGuild->build_size();i<GB_Cnt;i++)
			{
				DB_C_GuildBld* pDBBld = pDBGuild->add_build();
				pDBBld->set_id(i);
				pDBBld->set_level(0);
				pDBBld->set_exp(0);
			}
			if(pDBGuild->mutable_build(GB_Hall)->level()<1)
			{
				pDBGuild->mutable_build(GB_Hall)->set_level(1);
			}
			DB_C_GuildBld* pDBBld = pDBGuild->mutable_build(nBldID);
			if(pDBBld->level()==nLV)
				return;
			int nOldLV = pDBBld->level();
			pDBBld->set_exp(pGBTbl->m_lstCostValue[nLV]);
			pDBBld->set_level(nLV);
			if(nBldID == GB_Hall)
			{
				pGuild->GetDBGuild()->set_guildlv(pDBGuild->mutable_build(GB_Hall)->level());
				pRDH->SortGuild();
				if(nOldLV < GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinGuildLV && pGuild->GetGuildLV() >= GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinGuildLV)
				{
					pRDH->AddPKGuild(pGuild);
				}
				else
				{
					pRDH->SortPKGuild();
				}
			}
			SYS_LOG(nUserID,LT_ADMIN_SETGUILDBLDLV,pRDH->GetRegion(),0,nBldID<<nOldLV<<nLV);
			pRDH->markGuildLstDirty(pGuild);

		}
		break;
	case AOT_ADMIN_ADD_GUILDBLDVALUE:
		{//增加工会建设值
			int nGuildID= req->guildid();
			int nCnt	= req->count();
			CGuild* pGuild = pRDH->GetGuild(nGuildID);
			if(pGuild==NULL)
				return;
			DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
			int64 nUserID = req->uid();
			pDBGuild->set_bldvalue(pDBGuild->bldvalue()+nCnt);
			pDBGuild->set_totalbldvalue(pDBGuild->totalbldvalue()+nCnt);
			if(pDBGuild->bldvalue()<0)
			{
				pDBGuild->set_bldvalue(0);
			}
			if(pDBGuild->totalbldvalue()<0)
			{
				pDBGuild->set_totalbldvalue(0);
			}
			SYS_LOG(nUserID,LT_ADMIN_ADDGUILDBLDVALUE,pRDH->GetRegion(),0,nGuildID<<nCnt<<pDBGuild->bldvalue()<<pDBGuild->totalbldvalue());
			pRDH->markGuildLstDirty(pGuild);

		}
		break;
	case AOT_ADMIN_SET_OFFICERPOSITION:
		{//设置官员职位
			int nPosition= req->itemtype();
			int nFlag = req->bldtype();
			int64 nUid = req->uid();
			DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
			if(pDBUser==NULL)
			{
				break;
			}
			int nCountryId = pDBUser->countryid();
			if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
			{
				CCountry* pCountry = pRDH->GetCountry(nCountryId);
				if(pCountry)
				{
					int nOldPosition = pDBUser->officerposition();
					bool bUpdate = false;
					if(nFlag>0)
					{
						bUpdate = pCountry->SetUserOfficerPosition(pDBUser,(E_Officer_Position)nPosition);
					}
					else
					{
						bUpdate = pCountry->DelUserOfficerPosition(pDBUser,(E_Officer_Position)nPosition);
					}
					if(bUpdate)
					{
						int nNewPosition = pDBUser->officerposition();
						pRDH->markCountryDirty(pCountry);
						pRDH->markUserLstDirty(pDBUser->userid());
						eh_->SendCountryUserLiteInfo(pRDH,pDBUser);
						eh_->SendOfficerRightInfo2CountryUser(nCountryId,pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
						SYS_LOG(0,LT_OFFICER_POSITION_UPDATE,pRDH->GetRegion(),0,0<<(int64)pDBUser->userid()<<pDBUser->countryid()<<nPosition<<nOldPosition<<nNewPosition);
					}
				}
				eh_->SendCountryLiteInfo();
			}

		}
		break;
	case AOT_ADMIN_NPCACTIVERANK:
		pRDH->SortNpcActiveRank(time(NULL));
		break;
	default:
		break;
	}
}


void DealCountryEvent::HandleUpdateWorldArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e==NULL||pRDH==NULL)
	{
		return;
	}
	GCG_WorldArenaUpdate* pSrvOpt= e->mutable_worldarenaupdate();
	CountryDataHandler* pDH= eh_->getDataHandler();
	int64 nUserID	= e->uid();
	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	DB_C_User* pDBUser = pRDH->GetDBCityUser(nUserID);
	if(pDBUser == NULL)
	{
		return;
	}
	//国家竞技场数据更新
	pRDH->CheckClearCountryArenaData();
	//世界竞技场数据更新
	pRDH->UpdateWorldArenaData();
	//英雄竞技场数据更新
	pRDH->UpdateHeroArenaData();
	int nRankIndex = pEarth->CheckPlayerWorldRankIndex(pRDH,e->uid());
	pSrvOpt->set_selfrank(nRankIndex);
	pSrvOpt->set_lastworldrank(pDBUser->lastworldarenaindex());
	pSrvOpt->set_heroarenarank(pDBUser->heroarenaindex());
	//发送
	eh_->sendEventToGamed(e,nGameID);
}

void DealCountryEvent::HandleRceOfficerOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CountryDataHandler* pDH= eh_->getDataHandler();
	if(pDH==NULL)
	{
		return;
	}
	int64 nUserID = e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}
	int nCityID = pDBCUser->cityid();
	CCity* pCity = pRDH->GetCity(nCityID);
	if(NULL == pCity)
	{
		return;
	}
	CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
	if(NULL == pCountry)
	{
		return;
	}
	pCountry->UpdateUserOfficerPerDay();
	RceOfficerOpt *pRceOpt = e->mutable_ce_rceofficeropt();
	RseOfficerOpt *pRseOpt = e->mutable_se_rseofficeropt();
	RightInfo* pRightInfo   = pRseOpt->mutable_rightinfo();

	const OfficerRightTbl* pRightTbl = OfficerInfoCfg::Instance().GetOfficerRightTbl(pRightInfo->rightid());
	if(NULL == pRightTbl)
	{
		return ;
	}

	int ret = pRseOpt->ret();
	if(E_Officer_Ret_Sucess==ret)
	{
		if(!pCountry->IsOfficerRightUsed((E_Officer_Position)pRightInfo->position()))
		{
			if(pCity->UpdateOfficerRightBuff())
			{
				pRDH->markCityDirty(pCity);
			}
			int nLastTime = pRightTbl->m_nDelayTime;
			pRightInfo->set_endtime((int)time(NULL)+nLastTime);
			if(!pRDH->AddOfficerRightUsed(pCity,pRightInfo))
			{
				ret = E_Officer_Ret_Fail;
			}
		}
		else
		{
			ret = E_Officer_Ret_HasUsed;
		}
	}
	if(E_Officer_Ret_Sucess==ret)
	{
		pCountry->SetOfficerRightUsed((E_Officer_Position)pRightInfo->position());
		pRDH->markCountryDirty(pCountry);
		pRDH->markCityDirty(pCity);
		//发送给该城市的所有玩家
		eh_->SendOfficerRightUsed(pRDH,pCity);
		//发送给该国家的所有玩家
		eh_->SendOfficerRightInfo2CountryUser(pDBCUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Right);
	}
	pRseOpt->set_ret(ret);
	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);


}

void DealCountryEvent::HandleRceOfficerOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CountryDataHandler* pDH= eh_->getDataHandler();
	if(pDH==NULL)
	{
		return;
	}
	int64 nUserID = e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}

	RceOfficerOpen *pRceOfficerOpen = e->mutable_ce_rceofficeropen();
	RseOfficerOpen *pRseofficeropen = e->mutable_se_rseofficeropen();
	int opType = pRceOfficerOpen->optype();
	pRseofficeropen->set_optype(opType);
	if(opType==1)
	{
		for(int i=C_Start;i<C_UserCoutryEnd;i++)
		{
			CCountry* pCountry = pRDH->GetCountry(i);
			if(pCountry)
			{
				CoutryOfficer* pCountryOfficer = pRseofficeropen->add_officers();
				pRDH->FillCountrOfficerInfo(pCountry,pCountryOfficer,E_Officer_Fill_Officer);
			}
		}
	}
	else
	{
		CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
		if(pCountry == NULL)
		{
			return;					
		}
		CoutryOfficer* pCountryOfficer = pRseofficeropen->add_officers();
		pRDH->FillCountrOfficerInfo(pCountry,pCountryOfficer,E_Officer_Fill_Officer);
	}

	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}

void DealCountryEvent::HandleRceOfficerRightInfo(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CountryDataHandler* pDH= eh_->getDataHandler();
	if(pDH==NULL)
	{
		return;
	}
	int64 nUserID = e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}
	CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
	if(pCountry == NULL)
	{
		return;					
	}
	RseOfficerRightInfo *pRseInfo = e->mutable_se_rseofficerrightinfo();
	CoutryOfficer* pOfficerInfo = pRseInfo->mutable_officerinfo();
	pRDH->FillCountrOfficerInfo(pCountry,pOfficerInfo,(E_Officer_Fill_Type)pRseInfo->type());
	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}

void DealCountryEvent::HandleCountryUserLite(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e==NULL||pRDH==NULL)
	{
		return;
	}
	CountryDataHandler* pDH= eh_->getDataHandler();
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser == NULL)
	{
		return;
	}
	eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);
}

void DealCountryEvent::HandleRceWPCBaseOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e==NULL||pRDH==NULL)
	{
		return;
	}

	RceWPCBaseOpen* pRceRet = e->mutable_ce_rcewpcbaseopen();
	RseWPCBaseOpen* pRseRet = e->mutable_se_rsewpcbaseopen();
	if ( pRseRet == NULL || pRceRet == NULL )
	{
		return;
	}

	if ( pRceRet->type() == 2 )
	{
		pRDH->FillWpcRank( pRseRet );
	}
	else if ( pRceRet->type() == 3 )
	{
		pRDH->FillWpcRankChampion( pRseRet );
	}

	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}

void DealCountryEvent::HandGetAllArmRaceRank(Event* e,CountryRgnDatHandler* pRDH, int nGameID)
{
	if(e==NULL||pRDH==NULL)
	{
		return;
	}

	int64 nUserID = e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}
	RseArmRace*pRse = e->mutable_se_rsearmrace();
	if(pRse==NULL)
		return;

	if(NewRegionActivityCfg::Instance().InRightTimeByRegionAndType(pRDH->GetRegion(),ET_ACTIVITY_ARM_RACE,NG_ACTIVITY_ACTIVE_TIME))
	{
		pRse->set_labsearchcnt(pDBCUser->wpclabsearchcnt());
		pRDH->FillWpcLabSearchRank( pRse );

		e->set_state(Status_Country_To_Game);
		eh_->sendEventToGamed(e,nGameID);
	}
}

void DealCountryEvent::HandGetSelfArmRaceRank(Event* e,CountryRgnDatHandler* pRDH, int nGameID)
{
	if(e==NULL||pRDH==NULL)
	{
		return;
	}

	int64 nUserID = e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}

	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	pSrvOpt->set_wpcarmracerank(pRDH->GetSelfLabSearchRank(nUserID));

	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}

void DealCountryEvent::HandleCountryChatOpt(Event* e , CountryRgnDatHandler* pRDH , int nGameID)
{
	if (NULL == pRDH)
	{
		return;
	}
	e->set_state(Status_Country_To_Game);
	RceCountryChatMsg* pRce = e->mutable_ce_rcecountrychatmsg();
	int nType = pRce->type();
	switch(nType)
	{
	case CCOT_CHATUI:
		HandleCountryChatUI(e , pRDH , nGameID);
		return;
	case CCOT_CHATCOUNTRY:
		HandleCountryChat(e , pRDH , nGameID);
		return;
	case CCOT_CHATWORLD:
		HandleCountryChatWorld(e , pRDH , nGameID);
		return;
	}
}

void DealCountryEvent::HandleCountryChatUI(Event* e , CountryRgnDatHandler* pRDH , int nGameID)
{
	//打开界面,取出本国所有聊天记录
	RseCountryChatMsg* pRse = e->mutable_se_rsecountrychatmsg();
	int64 uid = e->uid();
	DB_C_User* pdbUser = pRDH->GetDBCityUser(uid);
	if (NULL == pdbUser)
	{
		pRse->set_ret(CCOR_REGIONERR);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	int nCountry = pdbUser->countryid();
	CCountry* pCountry = pRDH->GetCountry(nCountry);
	if (NULL == pCountry)
	{
		pRse->set_ret(CCOR_REGIONCOUNTRY);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	pCountry->GetChatMsg(pRse);
	eh_->sendEventToGamed(e , nGameID);
}

void DealCountryEvent::HandleCountryChat(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//会长在国家或世界发言
	RceCountryChatMsg* pRce = e->mutable_ce_rcecountrychatmsg();
	if (NULL == pRce)
	{
		return;
	}
	RseCountryChatMsg* pRse = e->mutable_se_rsecountrychatmsg();
	if (NULL == pRse)
	{
		return;
	}

	CountryChatInfo* pSelfInfo = e->mutable_countrychatinfo();
	if (NULL == pSelfInfo)
	{
		return;
	}
	string strName = pSelfInfo->name();

	CountryDataHandler* pDH= eh_->getDataHandler();
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(!pDBCUser) 
	{
		pRse->set_ret(CCOR_REGIONERR);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if (!pDBCUser->has_guildid())
	{
		pRse->set_ret(CCOR_NOTLEADER);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	CGuild* pGuild = pRDH->GetGuild(pDBCUser->guildid());
	if (NULL == pGuild)
	{
		pRse->set_ret(CCOR_NOTLEADER);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	//if (pGuild->GetGuildLV() < 2)
	if (pGuild->GetGuildLV() < 4)//新限制为4级公会
	{
		pRse->set_ret(CCOR_GUILDLV);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	//存数据库

	Event env;
	env.set_uid(nUserID);
	env.set_cmd(EVENT_COUNTRYCHAT_BROAD);
	env.set_state(Status_Country_To_Game);
	env.set_time(0);
	RseCountryChatMsg* pNewRse = env.mutable_se_rsecountrychatmsg();
	pNewRse->set_type(CCOT_ONE);
	MsgData* pNewMsg = pNewRse->add_chatmsg();
	pNewMsg->set_countryid(pDBCUser->countryid());
	pNewMsg->set_name(strName);
	pNewMsg->set_msg(pRce->chatmsg());
	pNewMsg->set_type(pRce->type());
	stringstream tmp;
	tmp<<nUserID;
	pNewMsg->set_uid(tmp.str());
	CountryChatInfo* pInfo = env.mutable_countrychatinfo();
	//pInfo->set_country(pDBCUser->countryid());
	pInfo->set_region(pRDH->GetRegion());

	bool bLeague = false;
	int nLeagueID = pRDH->GetLeagueID( pDBCUser->countryid() );
	CCountryLeague* pCountryLeague = pRDH->GetCountryLeague(nLeagueID);
	if ( pCountryLeague != NULL && pRce->type() == CCOT_CHATCOUNTRY)
	{
		time_t tNow = time(NULL);
		const LeagueBattleActiveTbl* pTbl = LeagueBattleCfg::Instance().GetActiveTbl( tNow );
		if ( pTbl != NULL )
		{
			if ( tNow < pTbl->m_nEndTm && tNow > pTbl->m_nAllStartTm )
			{
				//联盟广播
				bLeague = true;
				::google::protobuf::RepeatedField< ::google::protobuf::int32 >* pLeague = pCountryLeague->GetLeagueCountry();
				for (int i = 0 ; i < pLeague->size() ; i++)
				{
					CCountry* pCountry = pRDH->GetCountry(pLeague->Get(i));
					if (pCountry != NULL)
					{
						pCountry->AddChatMsg(nUserID , pDBCUser->countryid() , strName , pRce->chatmsg() , pRce->type());
						pRDH->markCountryDirty(pCountry);
						pInfo->add_country(pCountry->GetCountryID());
					}
				}
				eh_->sendEventToAllGamed(&env);
			}
		}
	}
	if(!bLeague)
	{
		//全国广播
		pInfo->add_country(pDBCUser->countryid());
		eh_->sendEventToAllGamed(&env);
		CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
		if (NULL == pCountry)
		{
			pRse->set_ret(CCOR_REGIONCOUNTRY);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		pCountry->AddChatMsg(nUserID , pDBCUser->countryid() , strName , pRce->chatmsg() , pRce->type());
		pRDH->markCountryDirty(pCountry);
	}
	
	{
		//前端需要给自己返回一条
		MsgData* pSelfMsg = pRse->add_chatmsg();
		pSelfMsg->set_countryid(pDBCUser->countryid());
		pSelfMsg->set_name(strName);
		pSelfMsg->set_msg(pRce->chatmsg());
		pSelfMsg->set_type(pRce->type());
		stringstream tmpself;
		tmpself<<nUserID;
		pSelfMsg->set_uid(tmpself.str());
		eh_->sendEventToGamed(e , nGameID);
	}

	//世界广播
	/*if (pRce->type() == CCOT_CHATWORLD)
	{
		for (int i = C_Start ; i < C_UserCoutryEnd ; i++)
		{
			CCountry* pOCountry = pRDH->GetCountry(i);
			if (NULL == pOCountry)
			{
				continue;
			}
			if (pOCountry->GetCountryID() == pDBCUser->countryid())
			{
				continue;
			}
			pOCountry->AddChatMsg(nUserID , pDBCUser->countryid() , strName , pRce->chatmsg() , pRce->type());
			pRDH->markCountryDirty(pOCountry);
		}
	}*/
	//不用发了,在gamed上处理吧
}

void DealCountryEvent::HandleCountryChatWorld(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//世界发言,所有人有道具都能发
	RceCountryChatMsg* pRce = e->mutable_ce_rcecountrychatmsg();
	if (NULL == pRce)
	{
		return;
	}
	RseCountryChatMsg* pRse = e->mutable_se_rsecountrychatmsg();
	if (NULL == pRse)
	{
		return;
	}

	CountryChatInfo* pSelfInfo = e->mutable_countrychatinfo();
	if (NULL == pSelfInfo)
	{
		return;
	}
	string strName = pSelfInfo->name();

	CountryDataHandler* pDH= eh_->getDataHandler();
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(!pDBCUser) 
	{
		pRse->set_ret(CCOR_REGIONERR);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	Event env;
	env.set_uid(nUserID);
	env.set_cmd(EVENT_COUNTRYCHAT_BROAD);
	env.set_state(Status_Country_To_Game);
	env.set_time(0);
	RseCountryChatMsg* pNewRse = env.mutable_se_rsecountrychatmsg();
	pNewRse->set_type(CCOT_ONE);
	MsgData* pNewMsg = pNewRse->add_chatmsg();
	pNewMsg->set_countryid(pDBCUser->countryid());
	pNewMsg->set_name(strName);
	pNewMsg->set_msg(pRce->chatmsg());
	pNewMsg->set_type(pRce->type());
	stringstream tmp;
	tmp<<nUserID;
	pNewMsg->set_uid(tmp.str());
	CountryChatInfo* pInfo = env.mutable_countrychatinfo();
	//pInfo->set_country(pDBCUser->countryid());
	pInfo->set_region(pRDH->GetRegion());
	//pInfo->set_bleader(pDBCUser->position() == GP_Leader);
	if (pDBCUser->position() != GP_Leader)
	{
		pNewMsg->set_type(CCOT_CHATCOUNTRY);//非会长发报机只能发本国了,前端需要把类型改了.正好gameevehthandler里就不用改了
	}
	eh_->sendEventToAllGamed(&env);

	{
		//前端需要给自己返回一条
		MsgData* pSelfMsg = pRse->add_chatmsg();
		pSelfMsg->set_countryid(pDBCUser->countryid());
		pSelfMsg->set_name(strName);
		pSelfMsg->set_msg(pRce->chatmsg());
		//pSelfMsg->set_type(pRce->type());
		pSelfMsg->set_type(pDBCUser->position() == GP_Leader ?  pRce->type() : CCOT_CHATCOUNTRY);
		stringstream tmpself;
		tmpself<<nUserID;
		pSelfMsg->set_uid(tmpself.str());
		eh_->sendEventToGamed(e , nGameID);
	}

	//世界广播
	//if (pRce->type() == CCOT_CHATWORLD)
	if(pDBCUser->position() == GP_Leader)
	{
		for (int i = C_Start ; i < C_UserCoutryEnd ; i++)
		{
			CCountry* pOCountry = pRDH->GetCountry(i);
			if (NULL == pOCountry)
			{
				continue;
			}
			/*if (pOCountry->GetCountryID() == pDBCUser->countryid())
			{
				continue;
			}*/
			pOCountry->AddChatMsg(nUserID , pDBCUser->countryid() , strName , pRce->chatmsg() , pRce->type());
			pRDH->markCountryDirty(pOCountry);
		}
	}
	else
	{
		CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
		if (NULL == pCountry)
		{
			pRse->set_ret(CCOR_REGIONCOUNTRY);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		pCountry->AddChatMsg(nUserID , pDBCUser->countryid() , strName , pRce->chatmsg() , CCOT_CHATCOUNTRY);
		pRDH->markCountryDirty(pCountry);
	}
}

void DealCountryEvent::HandleResetGuildPKScore(Event* e,CountryRgnDatHandler* pRDH, int nRegionID)
{
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    if (req == NULL)
    {
        return;
    }
    int nScore = req->count();
    if (nScore < 0)
    {
        nScore = 0;
    }
    pRDH->ResetGuildPKScore(nScore);
}

void DealCountryEvent::HandleUpdateCountryWpcRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nType = pSrvOpt->updatetype();

	if ( nType == 0 )
	{
		int64 nUserID	= e->uid();
		int nSelfCity	= pSrvOpt->selfcity();
		int nSelfCountry= pSrvOpt->selfcountry();
		int nActiveID = WeaponRankActiveCfg::Instance().GetWpcRankActiveID();
		if ( WeaponRankActiveCfg::Instance().IsRankActiveStart( nActiveID ) )
		{
			return;
		}

		bool bNeedSave = false;

		DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
		if(pDBCUser==NULL)
		{
			pDBCUser = pRDH->AllocDBUser(nUserID);
            pDBCUser->set_cityid(0);
            pDBCUser->set_countryid(0);
            pDBCUser->set_guildid(0);
            pDBCUser->set_name(pSrvOpt->selfname());
		}
		if(pDBCUser) 
		{
			// 		if(pDBCUser->name()!=pSrvOpt->selfname()||pDBCUser->pic()!=pSrvOpt->selfpic())
			// 		{
			// 			bNeedSave = true;
			// 			pDBCUser->set_name(pSrvOpt->selfname());
			// 			pDBCUser->set_pic(pSrvOpt->selfpic());
			// 			bNeedSave = true;
			// 		}
			// 		if(pDBCUser->level() != pSrvOpt->selflv())
			// 		{
			// 			pDBCUser->set_level(pSrvOpt->selflv());
			// 			bNeedSave = true;
			// 		}
			// 		if(pDBCUser->viplevel()!=pSrvOpt->viplevel()||pDBCUser->viptype()!=pSrvOpt->viptype())
			// 		{
			// 			pDBCUser->set_viplevel(pSrvOpt->viplevel());
			// 			pDBCUser->set_viptype(pSrvOpt->viptype());
			// 			bNeedSave = true;
			// 		}

            if (nSelfCountry > 0)
            {
                pDBCUser->set_wpcsearchtime(pSrvOpt->wpcsearchtime());

                if ( pSrvOpt->wpcsearchtime() > 0 )
                {
                    pRDH->CheckWpcMsg(pDBCUser);
                }

                string sUid = toString( nUserID );
                pRDH->SetWpcFightPoint(sUid,pSrvOpt->fightpoint());
            }
			pDBCUser->set_name(pSrvOpt->selfname());
			//判断活动的时间
			if(NewRegionActivityCfg::Instance().InRightTimeByRegionAndType(pRDH->GetRegion(),ET_ACTIVITY_ARM_RACE,NG_ACTIVITY_IN_TIME))
			{
				pDBCUser->set_wpclabsearchcnt(pDBCUser->wpclabsearchcnt()+pSrvOpt->wpclabeventcnt());
				bNeedSave = true;
			}
			
		}

		if(bNeedSave)
		{
			pRDH->markUserLstDirty(nUserID);
		}
	}
	else if ( nType == 1 )
	{
		//更新联盟战
		int64 nUserID	= e->uid();
		int nSelfCity	= pSrvOpt->selfcity();
		int nSelfCountry= pSrvOpt->selfcountry();

		bool bNeedSave = false;

		DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
		if(pDBCUser) 
		{
			pDBCUser->set_leaguepoint(pSrvOpt->leaguepoint());

			if ( pSrvOpt->leaguepoint() > 0 )
			{
				pRDH->CheckLeagueMsg(pDBCUser);
			}

			string sUid = toString( nUserID );
			pRDH->SetWpcFightPoint(sUid,pSrvOpt->fightpoint());

			if ( pDBCUser->has_guildid() && pSrvOpt->addleaguepoint() > 0 )
			{
				CGuild* pGuild = pRDH->GetGuild(pDBCUser->guildid());
				if ( pGuild != NULL )
				{
					bool bSave = pGuild->AddLeaguePoint( pSrvOpt->addleaguepoint() );
					pRDH->CheckLeagueMsg(pGuild);

					if ( bSave )
					{
						pRDH->markGuildLstDirty(pGuild);
					}	
				}
			}
		}

		if(bNeedSave)
		{
			pRDH->markUserLstDirty(nUserID);
		}
	}
	else if ( nType == 2 )
	{
		//更新boss战
		int64 nUserID	= e->uid();
		int nSelfCity	= pSrvOpt->selfcity();
		int nSelfCountry= pSrvOpt->selfcountry();

		bool bNeedSave = true;

		DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
		if(pDBCUser) 
		{
			pDBCUser->set_bossbtldmg(pSrvOpt->bossbtldmg());

			if ( pSrvOpt->bossbtldmg() > 0 )
			{
				pRDH->CheckBossBtlDmg(pDBCUser);
			}
		}

		string sUid = toString( nUserID );
		pRDH->SetWpcFightPoint(sUid,pSrvOpt->fightpoint());

		if(bNeedSave)
		{
			pRDH->markUserLstDirty(nUserID);
		}
	}
}

void DealCountryEvent::HandleUpdataNpcActiveRecord(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//通缉令打完后发给countryd记录,做排行榜
	e->set_state(Status_Country_To_Game);

	GCG_CountryNpcActive* pActiveData = e->mutable_countrynpcactive();
	if (NULL == pActiveData)
	{
		return;
	}

	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if (NULL == pDBCUser)
	{
		pActiveData->set_ret(-1);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
	if (NULL == pCountry)
	{
		pActiveData->set_ret(-1);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}

	int ret = pCountry->AddNpcActiveRecord(pActiveData->winlv());
	if (ret < 0)
	{
		pActiveData->set_ret(ret);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	pRDH->markCountryDirty(pCountry);
	eh_->sendEventToGamed(e , nGameID);
}

void DealCountryEvent::HandleGetNpcActiveRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//玩家查看军团通缉令排行榜
	e->set_state(Status_Country_To_Game);
	GCG_CountryNpcActive* pActData = e->mutable_countrynpcactive();
	if (NULL == pActData)
	{
		return;
	}
	RseNPCActive* pRse = e->mutable_se_rsenpcactive();
	if (NULL == pRse)
	{
		return;
	}
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if (NULL == pDBCUser)
	{
		pRse->set_ret(-1);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	//排的数据放在earthcountry中,直接从earth中取吧
	CCountry* pEarth = pRDH->GetEarthCountry();
	if (NULL == pEarth)
	{
		pRse->set_ret(-1);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	DB_C_Country* pDbEarth = pEarth->GetDBCountry();
	if (NULL == pDbEarth)
	{
		pRse->set_ret(-1);
		eh_->sendEventToGamed(e , nGameID);
		return;
	}
	if (pDbEarth->has_npcactiverank())
	{
		DB_C_NpcActiveRank* pdbrank = pDbEarth->mutable_npcactiverank();
		if (NULL == pdbrank)
		{
			return;
		}
		GroupCountryRank* pgrank = pRse->mutable_crank();
		if (NULL == pgrank)
		{
			pRse->set_ret(-1);
			eh_->sendEventToGamed(e , nGameID);
			return;
		}
		pgrank->set_topcountry(pDBCUser->countryid());
		for (int i = 0 ; i < pdbrank->rank_size() ; i++)
		{
			GroupCountryData* pgdata = pgrank->add_cdata();
			if (NULL == pgdata)
			{
				pRse->set_ret(-1);
				eh_->sendEventToGamed(e , nGameID);
				return;
			}
			DB_C_NpcActiveRankItem* pDBItem = pdbrank->mutable_rank(i);
			if (NULL == pDBItem)
			{
				return;
			}
			pgdata->set_countryid(pDBItem->has_country() ? pDBItem->country() : 0);
			for (int j = 0 ; j < pDBItem->wincnt_size() ; j++)
			{
				pgdata->add_wintimes(pDBItem->wincnt(j));
			}
		}
	}
	eh_->sendEventToGamed(e , nGameID);
	return;
}

void DealCountryEvent::HandleOutputGuildUserList(Event* e,CountryRgnDatHandler* pRDH, int nRegionID)
{
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    if (req == NULL)
    {
        return;
    }
    int nGuildID = req->count();
    if (nGuildID <= 0)
    {
        return;
    }
    CGuild *pGuild = pRDH->GetGuild(nGuildID);
    if (pGuild == NULL)
    {
        return;
    }
    for (map<int64,DB_C_User*>::const_iterator cIter = pGuild->m_mapGuildUser.begin(); cIter != pGuild->m_mapGuildUser.end(); cIter++)
    {
        if (cIter->second != NULL)
        {
            SYS_LOG((int64)cIter->first, LT_Guild_Dismiss,nRegionID,0,nGuildID);
        }
    }
}

void DealCountryEvent::OptTeleportCity(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceCountryOpt* pRceOpt = e->mutable_ce_rcecountryopt();
    GCG_CountryOpt* pSrvOpt= e->mutable_countryopt();
    RseCountryOpt* pRseRet = e->mutable_se_rsecountryopt();

    CountryDataHandler* pDH= eh_->getDataHandler();

    e->set_state(Status_Country_To_Game);

    int nOldCity = pSrvOpt->selfcity();
    int nNewCity = pSrvOpt->tarcity();
    CCity* pOldCity = pRDH->GetCity(nOldCity);
    CCity* pNewCity = pRDH->GetCity(nNewCity);
    DB_C_User* pDBUser=pRDH->GetDBCityUser(nUserID);
    //判断数据是否一致
    if(nOldCity == 0||pNewCity==NULL||pDBUser==NULL)
        return;

    time_t ltNow = time(NULL);
    //判断血战信息
    DB_C_BloodBtl* pDBBloodBtl = pRDH->GetLastDBBloodBtl(nOldCity);
    if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0)
    {//移出城市血战信息
        if(pDBBloodBtl->wincity()<=0)
        {//该国家血战未结束,血战进行中，不可进入或者离开
            int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
            if(pDBBloodBtl->launchtime()+nBanAtkTime<ltNow)
            {
                bool bCanMove = false;
                if(pDBBloodBtl->atkcity() == nNewCity && pDBBloodBtl->defcity() == nOldCity)
                {
                    bCanMove = true;
                }
                else if(pDBBloodBtl->defcity() == nNewCity && pDBBloodBtl->atkcity() == nOldCity)
                {
                    bCanMove = true;
                }
                if(!bCanMove)
                {
                    pSrvOpt->set_ret(38);
                    eh_->sendEventToGamed(e,nGameID);
                    return;
                }
            }
        }
    }
    pDBBloodBtl = pRDH->GetLastDBBloodBtl(nNewCity);
    if(pDBBloodBtl&&pDBBloodBtl->launchtime()>0)
    {//移入城市血战信息
        if(pDBBloodBtl->wincity()<=0)
        {//该国家血战未结束,血战进行中，不可进入或者离开
            int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
            if(pDBBloodBtl->launchtime()+nBanAtkTime<ltNow)
            {
                bool bCanMove = false;
                if(pDBBloodBtl->atkcity() == nNewCity && pDBBloodBtl->defcity() == nOldCity)
                {
                    bCanMove = true;
                }
                else if(pDBBloodBtl->defcity() == nNewCity && pDBBloodBtl->atkcity() == nOldCity)
                {
                    bCanMove = true;
                }
                if(!bCanMove)
                {
                    pSrvOpt->set_ret(38);
                    eh_->sendEventToGamed(e,nGameID);
                    return;
                }
            }
            else if(pNewCity->GetCountry()!=pOldCity->GetCountry())
            {
                int nBanAtkTime = CountryInfoCfg::Instance().GetBloodBtlBanAtkTime(pDBBloodBtl->type());
                if(pDBBloodBtl->launchtime()+nBanAtkTime>=ltNow)
                {//血战准备期，他国玩家不可进入
                    pSrvOpt->set_ret(38);
                    eh_->sendEventToGamed(e,nGameID);
                    return;
                }
            }
        }
        else if(pNewCity->GetCountry()!=pOldCity->GetCountry())
        {//不是一个国家
            int nCityProtectTm = CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pDBBloodBtl->type());
            if(pDBBloodBtl->overtime()+nCityProtectTm>ltNow)
            {
                pSrvOpt->set_ret(36);
                eh_->sendEventToGamed(e,nGameID);
                return;
            }
        }
    }

    int nRet = E_CORT_OtherCountry;
    if(pNewCity->GetCountry() == pOldCity->GetCountry() && pRDH->UserChgCity(pDBUser,nOldCity,nNewCity))
    {//同一个国家才能传送
        //更改玩家所在城市，此处先改country后改game，可能会有bug
        pRDH->markUserLstDirty(nUserID);
        nRet = 0;
    }

    pSrvOpt->set_ret(nRet);
    CityData* pCityData = pRseRet->mutable_citydata();
    pCityData->set_cityid(pNewCity->GetCityID());
    pCityData->set_countryid(pNewCity->GetCountry());
    pCityData->set_usercnt(pNewCity->GetUserCnt());

    //发送该城市的所有官员使用的权限
    eh_->SendOfficerRightUsed(pRDH,pNewCity,nUserID);
    if(pDBUser->officerposition()>0)
    {
        //发送给该国家的所有玩家更新官员信息
        //eh_->SendOfficerRightInfo2CountryUser(pDBUser->countryid(),pRDH->GetRegion(),-1,E_Officer_Fill_Officer);
    }
    eh_->sendEventToGamed(e,nGameID);
}


