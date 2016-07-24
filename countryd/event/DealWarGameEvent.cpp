#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealWarGameEvent.h"
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
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../logic/WarGameCfg.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealWarGameEvent ::DealWarGameEvent ()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealWarGameEvent ::~DealWarGameEvent ()
{

}

void DealWarGameEvent ::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceWarGameOpt, ((ProcessRoutine) DealWarGameEvent ::handle_));
}

void DealWarGameEvent ::handle(Event* e)
{	
	GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
	int nGameID = pNeedArgs->gamesrvid();
	int nRegion = pNeedArgs->region();

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
	case C2S_RceWarGameOpt:
		{
			HandleRceWarGameOpt(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealWarGameEvent::HandleRceWarGameOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    if ( pRDH == NULL )
    {
        return;
    }
    e->set_state(Status_Country_To_Game);
    CountryDataHandler* pDH= eh_->getDataHandler();
    RceWarGameOpt* pRce = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();
    pRseOpt->set_ret(WGOR_Success);
    pRseOpt->set_type(pRce->type());
    //pRDH->FillAllWarGameSiteInfo(pRseOpt);  ///所有的操作都需要返回全部列表

    switch (pRce->type())
    {
    case WGOT_Open:
    case WGOT_ReFresh:
        HandleOpen(e, pRDH, nGameID);
        break;
    case WGOT_Occupy:
        HandleOccupy(e, pRDH, nGameID);
        break;
    case WGOT_LootOver:
    case WGOT_CounterAttackOver:
        HandleLootOver(e, pRDH, nGameID);
        break;
    case WGOT_Leave:
        HandleLeave(e, pRDH, nGameID);
        break;
    case WGOT_DelaySite:
        HandleDelay(e, pRDH, nGameID);
        break;
    case WGOT_GetSiteList:
        HandleGetSiteList(e, pRDH, nGameID);
        break;
    case WGOT_Truce:
        HandleTruce(e, pRDH, nGameID);
        break;
    default:
        return;
    }
}


void DealWarGameEvent::HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();
    GCG_WarGameOpt *pGCGOpt = e->mutable_wargameopt();

    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(pGCGOpt->atkuid());
    if (pWarGameSite != NULL)
    {
        FillSiteInfoToRse(pRseOpt, pWarGameSite);

        pWarGameSite->UpdatePower(pGCGOpt->atkpower());
        pWarGameSite->UpdateCountry(pGCGOpt->atkcountry());
        pWarGameSite->UpdateName(pGCGOpt->atkname());
    }
    else
    {//没有占领演习场则需要获取所有演习场信息
        pRseOpt->set_siteid(0);
        pRDH->FillAllWarGameSiteInfo(pRseOpt);
    }

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::HandleOccupy(Event*e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();
    GCG_WarGameOpt *pGCGOpt = e->mutable_wargameopt();

    string strUid = pGCGOpt->atkuid();
    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(strUid);
    if (pWarGameSite != NULL)
    {
        pRseOpt->set_ret(WGOR_HasOccupy);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);

        pWarGameSite->UpdatePower(pGCGOpt->atkpower());
        pWarGameSite->UpdateCountry(pGCGOpt->atkcountry());
    }
    else
    {
        const WarGameSiteTbl *pTbl = WarGameCfg::Instance().GetSiteTbl(pRceOpt->siteid());
        pWarGameSite = pRDH->GetWarGameSite(pRceOpt->siteid());
        if (pWarGameSite == NULL || pTbl == NULL)
        {
            pRseOpt->set_ret(WGOR_WrongData);
        }
        else if (pWarGameSite->GetUid() != "")
        {
            pRseOpt->set_ret(WGOR_OccupyByOther);
            pRDH->FillAllWarGameSiteInfo(pRseOpt);
        }
        else
        {
            int nStart = time(NULL);
            int nEnd = nStart + pTbl->m_nOccupyDuration;
            vector<CWarGameAdmiral> tempVec;
            for (int i = 0; i < pGCGOpt->admiralinfo_size(); i++)
            {
                WarGameAdmiralInfo *pAdmiral = pGCGOpt->mutable_admiralinfo(i);
                if (pAdmiral != NULL)
                {
                    tempVec.push_back(CWarGameAdmiral(pAdmiral->admiralid(), pAdmiral->admirallv(), pAdmiral->admiraltrainlv()));
                }
            }
            pWarGameSite->ChangeOccupant(strUid, pGCGOpt->atkname(), pGCGOpt->atkcountry(),nStart, nEnd, pGCGOpt->atkpower(), tempVec);
            pRseOpt->set_ret(WGOR_Success);
            FillSiteInfoToRse(pRseOpt, pWarGameSite);

            pRDH->markWarGameDataDirty();
        }
    }

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::HandleLootOver(Event*e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    int nType = pRceOpt->type();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();

    GCG_WarGameOpt *pGCGOpt = e->mutable_wargameopt();
    pRceOpt->set_type(WGOT_AttackOverCountryAtck);

    string strUid = pGCGOpt->atkuid();
    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(strUid);
    if (pWarGameSite != NULL)
    {
        pGCGOpt->set_nresult(WGOR_HasOccupy);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);

        pWarGameSite->UpdatePower(pGCGOpt->atkpower());
        pWarGameSite->UpdateCountry(pGCGOpt->atkcountry());

        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    pWarGameSite = pRDH->GetWarGameSite(pGCGOpt->siteid());
    if (pWarGameSite == NULL)
    {
        pGCGOpt->set_nresult(WGOR_EnemyNoOccpySite);
        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    if (pWarGameSite->GetUid() != pGCGOpt->deftuid())
    {
        pGCGOpt->set_nresult(WGOR_OccupantChange);

        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    if (nType != WGOT_CounterAttackOver && pWarGameSite->IsSiteOnProtect())
    {//复仇没有保护时间
        pGCGOpt->set_nresult(WGOR_OccupyOnProtect);

        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    if (pWarGameSite->IsSiteOnTruce() && !pGCGOpt->bforce())
    {//免战期间
        pGCGOpt->set_nresult(WGOR_OccupyOnProtect);

        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    if (pGCGOpt->nresult() == WGOR_FightWin)
    {
        vector<CWarGameAdmiral> vecAdmiral;
        for (int i = 0; i < pGCGOpt->admiralinfo_size(); i++)
        {
            WarGameAdmiralInfo *pAdmiral = pGCGOpt->mutable_admiralinfo(i);
            if (pAdmiral != NULL)
            {
                CWarGameAdmiral tempAdmiral;
                tempAdmiral.m_nAdmID = pAdmiral->admiralid();
                tempAdmiral.m_nAdmLv = pAdmiral->admirallv();
                tempAdmiral.m_nAdmTrainLv = pAdmiral->admiraltrainlv();
                vecAdmiral.push_back(tempAdmiral);
            }
        }
        const WarGameSiteTbl *pTbl = WarGameCfg::Instance().GetSiteTbl(pGCGOpt->siteid());
        int tNow = time(NULL);
        int tEnd = tNow + pTbl->m_nOccupyDuration;
        pWarGameSite->ChangeOccupant(pGCGOpt->atkuid(), pGCGOpt->atkname(), pGCGOpt->atkcountry(), tNow, tEnd, pGCGOpt->atkpower(), vecAdmiral);
        pRseOpt->set_ret(WGOR_Success);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);
        pRseOpt->set_delaytime(0);
    }

    eh_->sendEventToGamed(e,nGameID);

    //发送给被攻击者
    int64 enemyUid = 0;
    safe_atoll(pGCGOpt->deftuid(),enemyUid);

    pRceOpt->set_type(WGOT_AttackOverCountryEnemy);
    e->set_state(Status_Normal_Logic_Game);
    e->set_uid(enemyUid);

    pRDH->markWarGameDataDirty();
    eh_->sendEventToGamed(e,eh_->getGamedIdByUserId(enemyUid));
}

void DealWarGameEvent::HandleLeave(Event*e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();

    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(toString(e->uid()));
    if (pWarGameSite == NULL)
    {
        pRseOpt->set_ret(WGOR_NoOccupySite);
    }
    else if (pRceOpt->siteid() != pWarGameSite->GetSiteID())
    {
        pRseOpt->set_ret(WGOR_WrongData);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);
    }
    else
    {
        if (!pWarGameSite->OccupantLeave())
        {
            pRseOpt->set_ret(WGOR_OccupyOnProtect);
            FillSiteInfoToRse(pRseOpt, pWarGameSite);
        }
        else
        {
            pRseOpt->set_ret(WGOR_Success);
            pRDH->FillAllWarGameSiteInfo(pRseOpt);
            pRDH->markWarGameDataDirty();
        }
    }

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::HandleDelay(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();

    string strUid = toString(e->uid());
    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(strUid);
    if (pWarGameSite == NULL)
    {
        pRseOpt->set_ret(WGOR_NoOccupySite);
        pRseOpt->clear_siteid();
    }
    else if (pWarGameSite->GetSiteID() != pRceOpt->siteid())
    {
        pRseOpt->set_ret(WGOR_WrongData);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);
    }
    else
    {
        pWarGameSite->OccupantDelay()?pRseOpt->set_ret(WGOR_Success) : pRseOpt->set_ret(WGOR_OverLimitDelayNum);
        FillSiteInfoToRse(pRseOpt, pWarGameSite);

        pRDH->markWarGameDataDirty();
    }

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::HandleGetSiteList(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();
    pRDH->FillAllWarGameSiteInfo(pRseOpt);

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::HandleTruce(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RceWarGameOpt* pRceOpt = e->mutable_ce_rcewargameopt();
    RseWarGameOpt* pRseOpt = e->mutable_se_rsewargameopt();
    GCG_WarGameOpt *pGCGOpt = e->mutable_wargameopt();

    string strUid = pGCGOpt->atkuid();
    CWarGameSite* pWarGameSite = pRDH->GetWarGameSiteByUid(strUid);
    if (pWarGameSite == NULL)
    {
        pRseOpt->set_ret(WGOR_WrongData);
    }
    else
    {
        pRseOpt->set_ret(WGOR_Success);
        pWarGameSite->AddTruceTime();
        FillSiteInfoToRse(pRseOpt, pWarGameSite);
    }

    eh_->sendEventToGamed(e,nGameID);
}

void DealWarGameEvent::FillSiteInfoToRse(RseWarGameOpt *pRseOpt, CWarGameSite *pWarGameSite)
{
    pRseOpt->set_siteid(pWarGameSite->GetSiteID());
    pRseOpt->set_sitelv( pWarGameSite->GetSiteLv() );
    pRseOpt->set_starttime(pWarGameSite->GetStartTime());
    pRseOpt->set_siteendtime( pWarGameSite->GetEndTime());
    pRseOpt->set_delaytime(pWarGameSite->GetDelayNum());
    pRseOpt->set_truceendtime(pWarGameSite->GetTruceEndTime());
    pRseOpt->set_trucestarttime(pWarGameSite->GetTruceStartTime());
}