#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealBossBtlEvent.h"
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
#include "../logic/BossBtlCfg.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealBossBtlEvent ::DealBossBtlEvent ()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealBossBtlEvent ::~DealBossBtlEvent ()
{

}

void DealBossBtlEvent ::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceBossOpt, ((ProcessRoutine) DealBossBtlEvent ::handle_));
}

void DealBossBtlEvent ::handle(Event* e)
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
	case C2S_RceBossOpt:
		{
			HandleRceBossBtlOpt(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealBossBtlEvent::HandleRceBossBtlOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if ( pRDH == NULL )
	{
		return;
	}
	e->set_state(Status_Country_To_Game);
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	pRseOpt->set_ret(0);

	int nType = pRceOpt->type();

	switch( nType )
	{
	case BOT_Open:
		{
			HandleOpen(e,pRDH,nGameID);
		}
		break;
	case BOT_OpenBtl:
	case BOT_UpdateBtl:
		{
			HandleBossBtlOpenBtl(e,pRDH,nGameID);
		}
		break;

	case BOT_Up:
		{
			HandleBossBtlUp(e,pRDH,nGameID);
		}
		break;

	case BOT_SeeUp:
		{
			HandleBossBtlSeeUp(e,pRDH,nGameID);
		}
		break;

	case BOT_SeeRank:
	case BOT_SeeHighRank:
	case BOT_SeeUpRank:
		{
			HandleBossBtlSeeRank(e,pRDH,nGameID,nType);
		}
		break;

	case BOT_GetReward:
		{
			HandleBossBtlGetReward(e,pRDH,nGameID);
		}
		break;

	case BOT_SetWords:
		{
			HandleBossBtlSetWords(e,pRDH,nGameID);
		}
		break;

	case BOT_ReportWords:
		{
			HandleBossBtlReportWords(e,pRDH,nGameID);
		}
		break;

	case BOT_Update:
		{
			HandleBossBtlUpdate(e,pRDH,nGameID);
		}
		break;

	case BOT_AtkOver:
		{
			HandleBossBtlAtk(e,pRDH,nGameID);
		}
		break;

	default: 
		break;
	}
}

void DealBossBtlEvent::HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	time_t tNow = time(NULL);

	////////////////////////////////////test

	//for ( int i = 0; i< 3; i++ )
	//{
	//	BossBattlePlayerUnit* pBattleUnit = pRseOpt->add_playerrankunit();
	//	pBattleUnit->set_name("123");
	//	pBattleUnit->set_declaration("234");
	//}

	//BossBattlePlayerUnit* pBattleUnit = pRseOpt->mutable_killerunit();
	//pBattleUnit->set_name("345");
	//pBattleUnit->set_declaration("456");
	//
	//for ( int i = 0; i< 2; i++ )
	//{
	//	BossBattlePlayerUnit* pBattleUnit = pRseOpt->add_luckyunit();
	//	pBattleUnit->set_name("567");
	//	pBattleUnit->set_declaration("789");
	//}

	//pRseOpt->set_bossid(1);

	//pRseOpt->set_selfrank(1);

	//////////////////////////////////

	pRDH->FillBossBtlMsg(pRseOpt);
	
	pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid()) );
	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlOpenBtl(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	time_t tNow = time(NULL);

	pRDH->FillBossBtlFightMsg(pRseOpt);

	pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid()) );
	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlUp(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}

	if ( pRceOpt->count() < 0 || pRceOpt->count() > 3 )
	{
		//错误的玩家,不能点赞
		return;
	}
	DB_C_BossBtlMsg* pDBBossMsg = pRDH->GetBossBtlMsg();

	if ( pDBBossMsg == NULL )
	{ 
		return;
	}

	
	DB_C_BossBtlRankUnit* pDBUnit = NULL;
	if ( pRceOpt->count() == 0 )
	{
		pDBUnit = pDBBossMsg->mutable_killer();
	}
	else
	{
		for ( int i = 0; i < pDBBossMsg->ranklist_size(); i++ )
		{
			if ( i + 1 == pRceOpt->count() )
			{
				pDBUnit = pDBBossMsg->mutable_ranklist(i);
				break;
			}
		}
	}

	if ( pDBUnit == NULL )
	{
		//没有玩家
		pRseOpt->set_ret(45);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
    pDBUnit->set_count( pDBUnit->count() + 1 );

	pRDH->BossBtlPlayerUp( pRceOpt->count(), pDBCUser, pDBUnit);

	pRDH->FillBossBtlMsg(pRseOpt);
	eh_->sendEventToGamed(e,nGameID);

    pRDH->markBossBtlInfoDirty();
}

void DealBossBtlEvent::HandleBossBtlSeeUp(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	int64 nUserID	= e->uid();

	if ( pRceOpt->count() <= 0 || pRceOpt->count() > 3 )
	{
		//错误的玩家,不能点赞
		return;
	}
	DB_C_BossBtlMsg* pDBBossMsg = pRDH->GetBossBtlMsg();

	if ( pDBBossMsg == NULL )
	{ 
		return;
	}

	DB_C_BossBtlRankUnit* pDBUnit = NULL;
	if ( pRceOpt->count() == 0 )
	{
		pDBUnit = pDBBossMsg->mutable_killer();
	}
	else
	{
		for ( int i = 0; i < pDBBossMsg->ranklist_size(); i++ )
		{
			if ( i + 1 == pRceOpt->count() )
			{
				pDBUnit = pDBBossMsg->mutable_ranklist(i);
				break;
			}
		}
	}

	if ( pDBUnit == NULL )
	{
		//没有玩家
		return;
	}

	int nRank = pRceOpt->count();

	if ( nRank >= BOSSBTL_UP_CNT || nRank < 0 )
	{
		return;
	}

	for ( list<string>::iterator iter = pRDH->m_lstUpList[nRank].begin(); iter != pRDH->m_lstUpList[nRank].end(); iter++ )
	{
		string strName = *iter;

		pRseOpt->add_uplist(strName);
	}
	
// 	for ( int i = 0; i < pDBUnit->namelist_size(); i++ )
// 	{
// 		pRseOpt->add_uplist(pDBUnit->namelist(i));
// 
	pRDH->FillBossBtlMsg(pRseOpt);
	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlSeeRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID,int nType)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();

	if ( nType == BOT_SeeRank )
	{
		pRDH->FillBossBtlRankList(pRseOpt,BOSSBTL_RANK_CNT_ALL);

		pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid()) );
	}

	else if ( nType == BOT_SeeHighRank )
	{
		pRDH->FillBossBtlRankList(pRseOpt,BOSSBTL_RANK_CNT_ALL,BOT_SeeHighRank);

		pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid(),BOT_SeeHighRank) );
	}

	else if ( nType == BOT_SeeUpRank )
	{
		pRDH->FillBossBtlRankList(pRseOpt,BOSSBTL_RANK_CNT_ALL,BOT_SeeUpRank);

		pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid(),BOT_SeeUpRank) );
	}

	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlGetReward(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	time_t tNow = time(NULL);

	pRDH->FillBossBtlMsg(pRseOpt);

	pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid()) );
	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlSetWords(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();

	DB_C_BossBtlMsg* pDBBossMsg = pRDH->GetBossBtlMsg();

	if ( pDBBossMsg == NULL )
	{ 
		return;
	}

	DB_C_BossBtlRankUnit* pDBUnit = NULL;

	bool bFound = false;

	for ( int i = 0; i < pDBBossMsg->ranklist_size() && i < 3 ; i++ )
	{
		pDBUnit = pDBBossMsg->mutable_ranklist(i);

		if ( pDBUnit->uid() == e->uid() )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		//没有玩家
		return;
	}

	pDBUnit->set_words(pRceOpt->words());

	pRDH->FillBossBtlMsg(pRseOpt);
	eh_->sendEventToGamed(e,nGameID);

    pRDH->markBossBtlInfoDirty();
}

void DealBossBtlEvent::HandleBossBtlReportWords(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	if ( pRceOpt->count() < 0 || pRceOpt->count() > 3 )
	{
		//错误的玩家,不能点赞
		return;
	}
	DB_C_BossBtlMsg* pDBBossMsg = pRDH->GetBossBtlMsg();

	if ( pDBBossMsg == NULL )
	{ 
		return;
	}

	DB_C_BossBtlRankUnit* pDBUnit = NULL;
	if ( pRceOpt->count() == 0 )
	{
		pDBUnit = pDBBossMsg->mutable_killer();
	}
	else
	{
		for ( int i = 0; i < pDBBossMsg->ranklist_size(); i++ )
		{
			if ( i + 1 == pRceOpt->count() )
			{
				pDBUnit = pDBBossMsg->mutable_ranklist(i);
				break;
			}
		}
	}

	if ( pDBUnit == NULL )
	{
		//没有玩家
		return;
	}

	DB_C_Report* pDBReport = NULL;
	for ( int i = 0; i < pDBUnit->reports_size(); i++ )
	{
		pDBReport = pDBUnit->mutable_reports(i);
		if ( pDBReport->type() == pRceOpt->reason() )
		{
			break;
		}
		pDBReport = NULL;
	}

	if ( pDBReport == NULL )
	{
		pDBReport = pDBUnit->add_reports();
		pDBReport->set_type( pRceOpt->reason());
	}

	pDBReport->set_count(pDBReport->count() + 1);

	pRDH->FillBossBtlMsg(pRseOpt);
	eh_->sendEventToGamed(e,nGameID);

    pRDH->markBossBtlInfoDirty();
}

void DealBossBtlEvent::HandleBossBtlUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceBossOpt* pRceOpt = e->mutable_ce_rcebossopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	time_t tNow = time(NULL);

	pRDH->FillBossBtlFightMsg(pRseOpt);

	pRseOpt->set_selfrank( pRDH->GetBossBtlRank(e->uid()) );
	eh_->sendEventToGamed(e,nGameID);
}

void DealBossBtlEvent::HandleBossBtlAtk(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	Country_FightBackData* pFightBackData = e->mutable_countryfightbackdata();
	PVPFightUser* pMsgAtk = pFightBackData->mutable_atkuser();
	PVPFightUser* pMsgDef = pFightBackData->mutable_defuser();
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		return;
	}

	RseBossOpt* pRseOpt = e->mutable_se_rsebossopt();
	pRseOpt->set_ret(0);

    DB_C_BossBtlMsg *pDBBossBtlMsg = pRDH->GetBossBtlMsg();
    DB_C_BossMsg* pDBBoss = pRDH->GetDBBossMsg();
    if (pDBBossBtlMsg == NULL || pDBBossBtlMsg->state() != E_BOSSBTLSTATE_BATTLE || pDBBoss == NULL)
    {
        pRseOpt->set_ret(5);
        eh_->sendEventToGamed(e,nGameID);
        return;
    }

    if ( pDBBoss->bossid() != 0 )
    {
        DB_C_BossInfo* pDBBossLVInfo = NULL;
        pDBBossLVInfo = pRDH->GetDBBossInfo(pDBBoss->bossid());
        if ( pDBBossLVInfo != NULL )
        {
            pRseOpt->set_bosslevel( pDBBossLVInfo->bosslv() );
        }
    }
	
	if ( pDBBoss->hp() <= 0 )
	{
		pRseOpt->set_ret(2);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

    int64 nDmg = pFightBackData->alldmg();
    //伤害需要缩小的倍数
    nDmg /= BossBtlCfg::Instance().GetBaseTbl(pDBBossBtlMsg->nbossbtlflag())->GetDamageZoomRate();

    if ( nDmg <= 0 )
    {
        //数据错误 伤害为零时强制为1
        nDmg = 1;
    }
    pFightBackData->set_alldmg(nDmg);

	int64 nOldHp = pDBBoss->hp();
	int64 nNewHp = nOldHp - nDmg;

	/////////////////////////////////////////////
	 

	bool bKill = false;
	if ( nNewHp < 0 )
	{
		nNewHp = 0;
		bKill = true;
	}
	pDBBoss->set_hp(nNewHp);

	pRseOpt->set_oldhp( toString(nOldHp) );
	pRseOpt->set_atkdmg( toString(nDmg) );

	pRseOpt->set_perhp( pDBBoss->hp()*100/pDBBoss->maxhp());
	pRseOpt->set_bkill(bKill);

	//////////////////////////////////////////////////测试添加日志
// #ifdef _WIN32
// 	for ( int i = 0 ; i < 25; i++ )
// 	{
// 		pRDH->AddBossBtlRecord(pDBCUser,nDmg,pDBBoss->hp(),bKill,pRseOpt->perhp());
// 	}
// #else
// 	pRDH->AddBossBtlRecord(pDBCUser,nDmg,pDBBoss->hp(),bKill,pRseOpt->perhp());
// #endif

	pRDH->AddBossBtlRecord(pDBCUser,nDmg,pDBBoss->hp(),bKill,pRseOpt->perhp());

    pRDH->markBossBtlInfoDirty();

    if (bKill)
    {
        SYS_LOG(nUserID,LT_BOSSBATTLE_COUNTRY_BOSSDEAD, pRDH->GetRegion(), 0, nOldHp<<nNewHp<<nDmg<<pRseOpt->perhp())
    }
    else
    {
        SYS_LOG(nUserID,LT_BOSSBATTLE_COUNTRY_DMGRECORD, pRDH->GetRegion(), 0, nOldHp<<nNewHp<<nDmg<<pRseOpt->perhp())
    }
	
	eh_->sendEventToGamed(e,nGameID);
}
