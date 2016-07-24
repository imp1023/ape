#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
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
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../logic/GvgCfg.h"
#include "../logic/GuildInfoCfg.h"
#include "../GuildBtlDBHandler.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealGvgEvent ::DealGvgEvent ()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealGvgEvent ::~DealGvgEvent ()
{

}

void DealGvgEvent ::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceGvgOpt, ((ProcessRoutine) DealGvgEvent ::handle_));
	eh_->getEventHandler()->registHandler(EVENT_GVGBTL_CACU, ((ProcessRoutine) DealGvgEvent ::handle_));
	eh_->getEventHandler()->registHandler(EVENT_GVGBTL_SEEREPORT, ((ProcessRoutine) DealGvgEvent ::handle_));
}

void DealGvgEvent ::handle(Event* e)
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
	case C2S_RceGvgOpt:
		{
			HandleRceGvgOpt(e,pRDH,nGameID);
		}
		break;
	case EVENT_GVGBTL_CACU:
		{
			HandleGvgBtlCacu(e,pRDH,nGameID);
		}
		break;
	case EVENT_GVGBTL_SEEREPORT:
		{
			HandleGvgSeeReport(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealGvgEvent::HandleRceGvgOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if ( pRDH == NULL )
	{
		return;
	}
	e->set_state(Status_Country_To_Game);
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	pRseOpt->set_ret(0);

	int nType		= pRceOpt->type();

	pRseOpt->set_type(nType);

	int nGuildID = pRceOpt->guildid();
	CGuild* pGuild = pRDH->GetGuild(nGuildID);
	if(pGuild==NULL)
	{
		pRseOpt->set_ret(2);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	switch( nType )
	{
	case Gvg_Open:
		{
			HandleGvgOpen(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Click:
		{
			HandleGvgClick(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Report:
		{
			HandleGvgReport(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Rank:
		{
			HandleGvgRank(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Sign:
		{
			HandleGvgSign(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Enter:
		{
			HandleGvgEnter(e,pRDH,nGameID,pGuild);
		}
		break;

	case Gvg_AddProp:
	case Gvg_AddPropCredit:
		{
			HandleGvgAddProp(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_BuyStreak:
		{
			HandleGvgBuyStreak(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_BrokeCity:
		{
			HandleGvgBrokeCity(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_DefCity:
		{
			HandleGvgDefCity(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_GetReward:
		{
			HandleGvgGetReward(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Update:
		{
			HandleGvgUpdate(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_Leave:
		{
			HandleGvgLeave(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_SeeCityPlayer:
		{
			HandleGvgSeeCityPlayer(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_GvgInterface:
		{
			HandleGvgInterface(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_RewardRecord:
		{
			HandleGvgRewardRecord(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_OffLineEnter:
		{
			HandleGvgOffLineEnter(e,pRDH,nGameID,pGuild);
		}
		break;
	case Gvg_OffLineLeave:
		{
			HandleGvgOffLineLeave(e,pRDH,nGameID,pGuild);
		}
		break;
	default: 
		break;
	}
}


void DealGvgEvent::HandleGvgOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	DB_C_PubGvgBtlInfo* pPublicInfo = pRDH->GetGvgEarthInfo();
	if ( pPublicInfo == NULL )
	{
		//不会走到这里
		return;
	}

	int nFlag;
	const GvgActiveTbl* pTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pTbl == NULL )
	{
		return;
	}

	nFlag = pTbl->m_nID;

	if ( nFlag <= pPublicInfo->flag() )
	{
		//确保在城市信息之后更新
		pGuild->CheckGvg(pPublicInfo->flag());
	}
	
	pRDH->FillGvgAllCityData( pRseOpt );
	pGuild->FillGvgGuildData( pRseOpt );

	if ( pRseOpt->controlcityid() != 0 )
	{
		pRDH->FillGvgCityData( pRseOpt,pRseOpt->controlcityid() );
	}
	
	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgClick(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	GCG_GuildOpt* pOpt = e->mutable_guildopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}
	
	pRDH->FillGvgCityData( pRseOpt,pRceOpt->cityid(),pOpt,pGuild->GetGuildID() );

	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 || pRceOpt->session() == 0 )
	{
		return;
	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgBtlRet* pRet = pCity->GetDBRet( pRceOpt->session() );
	if ( pRet == NULL )
	{
		return;
	}

	if ( pRet->winguildid() == -1 )
	{
		//没出结果
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	
	GuildBtlDBHandler::Instance().SafeLoadGvgBtlResult(e->uid(),pRceOpt->cityid(),pRDH->GetRegion(),pRceOpt->session(),nGameID);
}

void DealGvgEvent::HandleGvgRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	if ( pCity == NULL )
	{
		return;
	}
	
	pRDH->FillGvgRank(pRseOpt,pCity);

	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgSign(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	int nCityID = pRceOpt->cityid();

	if ( nCityID == 0 )
	{
		return;
	}
	
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);

	if(pDBCUser==NULL)
	{
		return;
	}

	int nPosition = pDBCUser->position();
	if ( nPosition != GP_Leader && nPosition != GP_SecLeader )
	{
		pRseOpt->set_ret( 3 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	if ( !pGuild->CanGvgSign() )
	{
		pRseOpt->set_ret( 4 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	CGvgCity* pGvgCity = pRDH->GetGvgCity(nCityID);
	if ( pGvgCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pGvgCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

	const GvgCityTbl* pTbl = GvgCfg::Instance().GetCityTbl(nCityID);
	if ( pTbl == NULL )
	{
		return;
	}

	for ( int i = 0; i < pDBGvgCity->signguildlist_size(); i++ )
	{
		if ( pDBGvgCity->signguildlist(i).guildid() == pGuild->GetGuildID() )
		{
			pRseOpt->set_ret( 10 );
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}

	if ( pDBGvgCity->guildid() == pGuild->GetGuildID() )
	{
		pRseOpt->set_ret( 10 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	if ( pDBGvgCity->signguildlist_size() >= pTbl->m_nCnt )
	{
		pRseOpt->set_ret( 5 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	if ( pGuild->GetGuildLV() < pTbl->m_nLv )
	{
		pRseOpt->set_ret( 6 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	//可以报名
	pGuild->GvgSign( nCityID );
	
	DB_C_GvgSignMsg* pDBSignMsg = pDBGvgCity->add_signguildlist();
	pDBSignMsg->set_guildid(pGuild->GetGuildID());

	SYS_LOG(pGuild->GetGuildID(),LT_CNTGVG_SIGN,0,0,nCityID<<e->uid());

	pRDH->CheckGvgRank( pGvgCity );

	pRDH->FillGvgRank( pRseOpt,pGvgCity );

	pGuild->FillGvgGuildData( pRseOpt );


	pRDH->markGvgCityDirty(pGvgCity);
	
	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgEnter(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}
	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
	if ( gvgStep != GvgBtlType_LineUp0 && gvgStep != GvgBtlType_LineUp1 )
	{
		pRseOpt->set_ret( 11 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	if ( pCity == NULL )
	{
		return;
	}

	int nSession = 0;
	if ( gvgStep == GvgBtlType_LineUp0 )
	{
		nSession = 1;
	}

	if ( gvgStep == GvgBtlType_LineUp1 )
	{
		nSession = 2;
	}

	if ( !pRDH->CheckCanEnterCity( pCity, pGuild, nSession ) )
	{
		pRseOpt->set_ret( 12 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	pGuild->EnterCity( e->uid(), pRceOpt->cityid(), nSession );

	//更新数据
	GCG_GuildOpt* pGuildOpt = e->mutable_guildopt();
	pRDH->SetPlayerProp( pGuild, e->uid(), pGuildOpt );
	
	Country_FightSendData* pFightData = e->mutable_guildfightsenddata();
	SaveGvgBtlSendData(e->uid(),pFightData);

	pRDH->FillGvgPlayerData( pRseOpt, pRceOpt->cityid(), nSession );
	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgAddProp(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	GCG_GuildOpt* pOpt = e->mutable_guildopt();

	pRDH->SetPlayerProp( pGuild, e->uid(), pOpt );
}

void DealGvgEvent::HandleGvgBuyStreak(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	GCG_GuildOpt* pOpt = e->mutable_guildopt();

	pRDH->SetPlayerProp( pGuild, e->uid(), pOpt );
}

void DealGvgEvent::HandleGvgBrokeCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	int nCityID = pRceOpt->cityid();

	CGvgCity* pCity = pRDH->GetGvgCity(nCityID);
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBCity = pCity->GetDBGvgCity();
	if ( pDBCity == NULL )
	{
		return;
	}

	GCG_GuildOpt* pOpt = e->mutable_guildopt();
	int64 nUserID = pOpt->taruserid();
	if ( nUserID > 0 && pCity->m_lstDefUser.size() > 0 )
	{
		//消除玩家防御记录
		list<int64>::iterator iter;
		iter = pCity->m_lstDefUser.begin();
		if ( nUserID == *iter )
		{
			pCity->m_lstDefUser.pop_front();
		}
	}

	if ( pCity->m_lstAtkUser.size() < MAX_GVG_PLAYER_RECORD_NUM && pRceOpt->citypoint() > 0 )
	{
		//城防点数等于零时攻击失败
		pCity->m_lstAtkUser.push_back(e->uid());
	}

	if ( pRceOpt->citypoint() > 0 )
	{
		pCity->AddGvgCityPoint ( -pRceOpt->citypoint() );
	}
	pRDH->markGvgCityDirty(pCity);
}

void DealGvgEvent::HandleGvgDefCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	int nCityID = pRceOpt->cityid();

	CGvgCity* pCity = pRDH->GetGvgCity(nCityID);
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBCity = pCity->GetDBGvgCity();
	if ( pDBCity == NULL )
	{
		return;
	}

	GCG_GuildOpt* pOpt = e->mutable_guildopt();
	int64 nUserID = pOpt->taruserid();
	if ( nUserID > 0 && pCity->m_lstAtkUser.size() > 0 )
	{
		list<int64>::iterator iter;
		iter = pCity->m_lstAtkUser.begin();
		if ( nUserID == *iter )
		{
			pCity->m_lstAtkUser.pop_front();
		}
	}

	if ( pCity->m_lstDefUser.size() < MAX_GVG_PLAYER_RECORD_NUM && pRceOpt->citypoint() > 0 )
	{
		//城防点数等于零时防御失败
		pCity->m_lstDefUser.push_back(e->uid());
	}

	if ( pRceOpt->citypoint() > 0 )
	{
		pDBCity->set_citypoint( pDBCity->citypoint() + pRceOpt->citypoint() );
	}
	pRDH->markGvgCityDirty(pCity);
}

void DealGvgEvent::HandleGvgGetReward(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);

	const GvgActiveTbl* pTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pTbl == NULL )
	{
		return;
	}

	if(pDBCUser==NULL)
	{
		return;
	}

	int nPosition = pDBCUser->position();

	DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	DB_C_GVG_Reward_Record* pDBRecord = NULL;

	for ( int i = 0; i < pDBGvgMsg->rewardrecord_size(); i++ )
	{
		if ( pDBGvgMsg->rewardrecord(i).pos() == nPosition )
		{
			pDBRecord = pDBGvgMsg->mutable_rewardrecord(i);
			break;
		}
	}

	if ( pDBRecord == NULL )
	{
		pDBRecord = pDBGvgMsg->add_rewardrecord();
		pDBRecord->set_pos( nPosition );
	}

// 	bool bFind = false;
// 	for ( int i = 0; i < pDBGvgMsg->playerid_size(); i++ )
// 	{
// 		if ( pDBGvgMsg->playerid(i) == e->uid() )
// 		{
// 			bFind = true;
// 			break;
// 		}
// 	}
// 
// 	if ( !bFind )
// 	{
// 		pRseOpt->set_ret(23);
// 		eh_->sendEventToGamed(e,nGameID);
// 		return;
// 	}

	int nLimit = GuildInfoCfg::Instance().GetPositionMaxCnt(pGuild->GetGuildLV(),(GuildPositionType)nPosition);

	if ( pDBRecord->cnt() >= nLimit )
	{
		//超过领取数量限制
		pRseOpt->set_ret(22);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	//可以领取

	pRDH->FillGvgAllCityData( pRseOpt );
	pGuild->FillGvgGuildData( pRseOpt );

	//////////////////////////////////////////////////////////
	//20141209活动问题补偿
	
	int nLv = 0;
	int nGetCityID = 0;
	if ( pTbl->m_nID == 2 )
	{
		for ( int i = 0; i < pDBGvgMsg->signcity_size(); i++ )
		{
			int nCityID = pDBGvgMsg->signcity(i);

			const GvgCityTbl* pTmpTbl = GvgCfg::Instance().GetCityTbl(nCityID);
			if ( pTmpTbl == NULL )
			{
				continue;
			}

			CGvgCity* pCity = pRDH->GetGvgCity(nCityID);
			if ( pCity == NULL )
			{
				continue;
			}
			
			bool bCanEnter = false;
			for ( int j = 1; j <= 2; j++ )
			{
				if ( pRDH->CheckCanEnterCity( pCity, pGuild, j ) )
				{
					bCanEnter = true;
				}
			}
			if ( bCanEnter )
			{
				//可以领取，判断优先城市
				if ( nCityID == 0 )
				{
					nGetCityID = pTmpTbl->m_nID;
					nLv = pTmpTbl->m_nLv;
				}
				else if ( pTmpTbl->m_nLv > nLv)
				{
					nGetCityID = pTmpTbl->m_nID;
					nLv = pTmpTbl->m_nLv;
				}
			}
		}


		pRseOpt->set_controlcityid( nGetCityID );
	}
	else if ( pTbl->m_nID == 3 )
	{
		return;
	}


	///////////////////////////////////////////

	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	GCG_GuildOpt* pOpt = e->mutable_guildopt();

	pRDH->SetPlayerProp( pGuild, e->uid(), pOpt );

	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}
	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
	if ( gvgStep != GvgBtlType_LineUp0 && gvgStep != GvgBtlType_LineUp1 )
	{
		pRseOpt->set_ret( 11 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	int nSession = 0;
	if ( gvgStep == GvgBtlType_LineUp0 )
	{
		nSession = 1;
	}

	if ( gvgStep == GvgBtlType_LineUp1 )
	{
		nSession = 2;
	}

	pGuild->LeaveCity( e->uid(), pRceOpt->cityid(), nSession );

	pRDH->FillGvgPlayerData( pRseOpt, pRceOpt->cityid(), nSession );

	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::SaveGvgBtlSendData(int64 nUid,Country_FightSendData* pFightData)
{
	if(nUid==0||pFightData==NULL)
		return;
	DB_C_Gvg_FightSendData dbFight;
	DB_C_GuildBtlUser* pDBBtlUser = dbFight.mutable_btluser();
	PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
	pDBBtlUser->set_userid(toString(nUid));
	pDBBtlUser->set_name(pAtkUser->name());
	pDBBtlUser->set_pic(pAtkUser->pic());
	pDBBtlUser->set_level(pAtkUser->level());
	pDBBtlUser->set_combatvalue(pAtkUser->combatvalue());

	for(int i=0;i<pFightData->fightsenddata_size();i++)
	{
		Pve_FightSendDataUnit* pDataUnit = pFightData->mutable_fightsenddata(i);
		DB_C_GuildBtlSendDataUnit* pDBDataUnit = dbFight.add_fightsenddata();
        for (int j = 0; j < pDataUnit->pvespeedreduce_self_size(); j++)
        {
            pDBDataUnit->add_nreducespeed(pDataUnit->pvespeedreduce_self(j));
        }
		//兵种属性
		for(int j=0;j<pDataUnit->pvecarmyatkprop_self_size();j++)
		{
			Pve_CArmyAtkProp* pProp = pDataUnit->mutable_pvecarmyatkprop_self(j);
			DB_C_GuildBtlArmyAtkProp* pDBProp = pDBDataUnit->add_pvecarmyatkprop();
			pDBProp->set_narmytype(pProp->narmytype());
			pDBProp->set_nhp(pProp->nhp());
			pDBProp->set_ndamage(pProp->ndamage());
			pDBProp->set_ndamagebase(pProp->ndamagebase());
			pDBProp->set_ndamagepec(pProp->ndamagepec());
			pDBProp->set_nrange(pProp->nrange());
			pDBProp->set_ncrt(pProp->ncrt());
			pDBProp->set_nspeed(pProp->nspeed());
			pDBProp->set_nturnspeed(pProp->nturnspeed());
			//pDBProp->set_nrestraint(pProp->nrestraint());
			pDBProp->set_ncrtdamage(pProp->ncrtdamage());
			pDBProp->set_ncrtdef(pProp->ncrtdef());
			pDBProp->set_ndamagedef(pProp->ndamagedef());
			for(int k=0;k<pProp->nrestraint_size();k++)
			{
				pDBProp->add_nrestraint(pProp->nrestraint(k));
			}
			for(int k=0;k<pProp->nrestraintdef_size();k++)
			{
				pDBProp->add_nrestraintdef(pProp->nrestraintdef(k));
			}
			pDBProp->set_ntotaldamagepct(pProp->ntotaldamagepct());
			pDBProp->set_nbuffdamagepct(pProp->nbuffdamagepct());
			pDBProp->set_nrangebase(pProp->nrangebase());
			pDBProp->set_nspeedbase(pProp->nspeedbase());
			pDBProp->set_npropertydamagepec(pProp->npropertydamagepec());
			pDBProp->set_npropertydamagedefpec(pProp->npropertydamagedefpec());

            for (int k = 0; k < pProp->ndefatkpec_size();k++)
            {
                pDBProp->add_ndefatkpec(pProp->ndefatkpec(k));
                pDBProp->add_ndefcritrange(pProp->ndefcritrange(k));
                pDBProp->add_ndefcritrate(pProp->ndefcritrate(k));
            }
		}
		//战斗单元
		for(int j=0;j<pDataUnit->pvefightunit_self_size();j++)
		{
			Pve_FightUnit* pFUnit = pDataUnit->mutable_pvefightunit_self(j);
			DB_C_GuildBtlFightUnit* pDBUnit = pDBDataUnit->add_pvefightunit();
			pDBUnit->set_nid(pFUnit->nid());
			pDBUnit->set_narmytype(pFUnit->narmytype());
			pDBUnit->set_nhp(pFUnit->nhp());
			pDBUnit->set_nmaxhp(pFUnit->nmaxhp());
			pDBUnit->set_ngrade(pFUnit->ngrade());
			pDBUnit->set_narmygroup(pFUnit->narmygroup());
			pDBUnit->set_nrestframe(pFUnit->nrestframe());
			DB_C_GuildBtlPosition* pDBPos = pDBUnit->mutable_pos();
			pDBPos->set_nposx(pFUnit->mutable_pos()->nposx());
			pDBPos->set_nposy(pFUnit->mutable_pos()->nposy());
			pDBUnit->set_nherograde(pFUnit->nherograde());
			pDBUnit->set_nherolv(pFUnit->nherolv());
		}
		//英雄技能
		Pve_HeroSkill* pHeroSkill = pDataUnit->mutable_pveheroskill_self();
		if(pHeroSkill&&pHeroSkill->heroid()>0)
		{
			Pve_GuildBtlHeroSkill* pDBHeroSkill = pDBDataUnit->mutable_pvefightheroskill();
			pDBHeroSkill->set_heroid(pHeroSkill->heroid());
			pDBHeroSkill->set_level(pHeroSkill->level());
			pDBHeroSkill->set_skillid1(pHeroSkill->skillid1());
			pDBHeroSkill->set_skillid2(pHeroSkill->skillid2());
			pDBHeroSkill->set_skill1grade(pHeroSkill->skill1grade());
			pDBHeroSkill->set_skill2grade(pHeroSkill->skill2grade());
			pDBHeroSkill->set_skill1quality(pHeroSkill->skill1quality());
			pDBHeroSkill->set_skill2quality(pHeroSkill->skill2quality());
		}
		//将领
		DB_C_GuildBtlAdmiralAdd* pDBAdmiralAdd = pDBDataUnit->mutable_pvefightadmiral();
		Pve_AdmiralAdd* pAdmiralAdd = pDataUnit->mutable_pvefightadmiral_self();
		for(int j=0;j<pAdmiralAdd->naddpctfoot_size();j++)
		{
			pDBAdmiralAdd->add_naddpctfoot(pAdmiralAdd->naddpctfoot(j));
		}
		for(int j=0;j<pAdmiralAdd->naddpcttank_size();j++)
		{
			pDBAdmiralAdd->add_naddpcttank(pAdmiralAdd->naddpcttank(j));
		}
		for(int j=0;j<pAdmiralAdd->naddpctplane_size();j++)
		{
			pDBAdmiralAdd->add_naddpctplane(pAdmiralAdd->naddpctplane(j));
		}
		for(int j=0;j<pAdmiralAdd->naddpctbuild_size();j++)
		{
			pDBAdmiralAdd->add_naddpctbuild(pAdmiralAdd->naddpctbuild(j));
		}
		DB_C_GuildBtlGeneral* pDBGeneral = pDBAdmiralAdd->mutable_fightgeneral();
		PVEFightGeneral* pGeneral = pAdmiralAdd->mutable_fightgeneral();
		pDBGeneral->set_ngeneralid(pGeneral->ngeneralid());
		pDBGeneral->set_ngenerallvl(pGeneral->ngenerallvl());
		pDBGeneral->set_ngeneraltrainlvl(pGeneral->ngeneraltrainlvl());
		pDBGeneral->set_ngeneralatk(pGeneral->ngeneralatk());
		pDBGeneral->set_ngeneraldef(pGeneral->ngeneraldef());
		pDBGeneral->set_ngeneralhp(pGeneral->ngeneralhp());
		pDBGeneral->set_ngeneralcrt(pGeneral->ngeneralcrt());
		pDBGeneral->set_ngeneralscore(pGeneral->ngeneralscore());
        for (int mhIdx = 0; mhIdx < pGeneral->ngeneralmh_size(); mhIdx++)
        {
            PVEFightMH *pMH = pGeneral->mutable_ngeneralmh(mhIdx);
            DB_C_PVEFightMH *pDBMH = pDBGeneral->add_listgeneralmh();
            pDBMH->set_nmhid(pMH->nmhid());
            pDBMH->set_nmhlv(pMH->nmhlv());
        }
	}
	GuildBtlDBHandler::Instance().SafePushGuildGvgUpdate(nUid,GuildGvgDB_USER,&dbFight);
}

void DealGvgEvent::CheckGvgBtlStep(CountryRgnDatHandler* pRDH)
{
	if ( pRDH == NULL )
	{
		return;
	}

	time_t ltNow = time(NULL);

	const GvgActiveTbl* pTbl = GvgCfg::Instance().GetActiveTbl(ltNow);
	if ( pTbl == NULL )
	{
		return;
	}

	GvgBtlStepType gvgStep = pTbl->GetGvgStep(ltNow);

	int nSession = 0;

	if ( gvgStep == GvgBtlType_Btl0 )
	{
		nSession = 1;
	}

	if ( gvgStep == GvgBtlType_Btl1 )
	{
		nSession = 2;
	}

	if ( nSession == 0 )
	{
		return;
	}

	for(int i=0;i<2&&true;i++)
	{
		DB_C_GvgBtlRet* pGvgBtl = pRDH->GetGvgFightCity( nSession );

		if ( pGvgBtl == NULL )
		{
			//结束
			continue;
		}
		
		CGuild* pGuild1 = pRDH->GetGuild(pGvgBtl->guildid1());
		CGuild* pGuild2 = pRDH->GetGuild(pGvgBtl->guildid2());
		if ( pGuild1 == NULL || pGuild2 == NULL )
		{
			return;
		}

		int nStartNum = 0;

		for(int j=0;j<GUILD_BTL_DAY_CNT;j++)
		{
			GuildBtlDBHandler::Instance().DealGvgBtl(pGuild1,pGuild2,pRDH->GetRegion(),nSession,j,pGvgBtl->cityid());
		}
	}
}


void DealGvgEvent::HandleGvgBtlCacu(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}
	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
	if ( gvgStep != GvgBtlType_Btl0 && gvgStep != GvgBtlType_Btl1 )
	{
		return;
	}
	int nSession = 0;
	if ( gvgStep == GvgBtlType_Btl0 )
	{
		nSession = 1;
	}
	if ( gvgStep == GvgBtlType_Btl1 )
	{
		nSession = 2;
	}

	
	if(e->state() == Status_Normal_Game)
	{//在此取包，将双方的工会加成塞进去
		GuildBtl_FightCacuData* pGBCacuData = e->mutable_guildbtlcacudata();
		int nGuildID1 = pGBCacuData->guildid1();
		int nGuildID2 = pGBCacuData->guildid2();

		const GvgBaseTbl* pBaseTbl = GvgCfg::Instance().GetBaseTbl();
		if ( pBaseTbl == NULL )
		{
			return;
		}

		const GvgCityTbl* pTbl = GvgCfg::Instance().GetCityTbl(pGBCacuData->cityid());
		if ( pTbl == NULL )
		{
			return;
		}
		CGvgCity* pGvgCity = pRDH->GetGvgCity(pGBCacuData->cityid());
		if ( pGvgCity == NULL )
		{
			return;
		}
		DB_C_GvgCity* pDBGvgCity = pGvgCity->GetDBGvgCity();
		if ( pDBGvgCity == NULL )
		{
			return;
		}
		pGvgCity->GetDBRet(nSession);
		DB_C_GvgBtlRet* pRet = pGvgCity->GetDBRet(nSession);
		if ( pRet == NULL )
		{
			return;
		}
		if ( pRet->winguildid() != -1 )
		{
			return;
		}

		CGuild* pGuild1 = pRDH->GetGuild(nGuildID1);
		if(pGuild1)
		{
			int nTurns = pGBCacuData->session();
			int nCityPoint = 100;

			if ( pDBGvgCity->guildid() == nGuildID1 )
			{
				if ( pTbl->m_nPoint != 0 )
				{
					nCityPoint = pDBGvgCity->citypoint()/(pTbl->m_nPoint/100);
				}
				if ( nCityPoint < pBaseTbl->m_nMinCityPoint )
				{
					nCityPoint = pBaseTbl->m_nMinCityPoint;
				}
				if ( nCityPoint > pBaseTbl->m_nMaxCityPoint )
				{
					nCityPoint = pBaseTbl->m_nMaxCityPoint;
				}
			}

			DB_C_GVG_Msg* pDBGvgGuild = pGuild1->GetDBGvgMsg();
			if ( pDBGvgGuild != NULL )
			{
				for(int i=0;i<pDBGvgGuild->addprop_size();i++)
				{
					DB_C_GvgBtlAddProp* pDBProp = pDBGvgGuild->mutable_addprop(i);
					GuildBtl_AddProp* pAddProp = pGBCacuData->add_addprop();
					pAddProp->set_userid(pDBProp->userid());
					pAddProp->set_addatkpct(pDBProp->addatkpct());
					pAddProp->set_addbloodpct(pDBProp->addbloodpct());
					pAddProp->set_maxwintime(pDBProp->wintime());
					pAddProp->set_citypointpct(nCityPoint);
				}
			}

		}
		CGuild* pGuild2 = pRDH->GetGuild(nGuildID2);
		if(pGuild2)
		{
			int nTurns = pGBCacuData->session();
			int nCityPoint = 100;

			if ( pDBGvgCity->guildid() == nGuildID2 )
			{
				if ( pTbl->m_nPoint != 0 )
				{
					nCityPoint = pDBGvgCity->citypoint()/(pTbl->m_nPoint/100);
				}
				if ( nCityPoint < pBaseTbl->m_nMinCityPoint )
				{
					nCityPoint = pBaseTbl->m_nMinCityPoint;
				}
				if ( nCityPoint > pBaseTbl->m_nMaxCityPoint )
				{
					nCityPoint = pBaseTbl->m_nMaxCityPoint;
				}
			}

			DB_C_GVG_Msg* pDBGvgGuild = pGuild2->GetDBGvgMsg();
			if ( pDBGvgGuild != NULL )
			{
				for(int i=0;i<pDBGvgGuild->addprop_size();i++)
				{
					DB_C_GvgBtlAddProp* pDBProp = pDBGvgGuild->mutable_addprop(i);
					GuildBtl_AddProp* pAddProp = pGBCacuData->add_addprop();
					pAddProp->set_userid(pDBProp->userid());
					pAddProp->set_addatkpct(pDBProp->addatkpct());
					pAddProp->set_addbloodpct(pDBProp->addbloodpct());
					pAddProp->set_maxwintime(pDBProp->wintime());
					pAddProp->set_citypointpct(nCityPoint);
				}
			}
		}
		SYS_LOG(pGBCacuData->cityid(),LT_CNTGVG_BTL_SEND,0,0,pGBCacuData->cityid()<<pRDH->GetRegion()<<nGuildID1<<nGuildID2<<nSession);
		eh_->sendEventToGamed(e,nGameID);
	}
	else if(e->state() == Status_Game_To_Country)
	{//game返回
		GuildBtlRet* pRseRet = e->mutable_guildbtlret();
		int nGuild1 = pRseRet->guildid1();
		int nGuild2 = pRseRet->guildid2();
		int nRetSession = pRseRet->session();
		int nCityID = pRseRet->cityid();
		CGuild* pGuild1 = pRDH->GetGuild(nGuild1);
		CGuild* pGuild2 = pRDH->GetGuild(nGuild2);

		CGvgCity* pCity = pRDH->GetGvgCity(nCityID);
		if ( pCity == NULL )
		{
			return;
		}

		if ( nRetSession != nSession )
		{
			//过期消息
			return;
		}

		DB_C_GvgBtlRet* pDBRet = pCity->GetDBRet(nSession);
		if ( pDBRet == NULL )
		{
			return;
		}

		GuildBtlDBHandler::Instance().SafePushGuildGvgResult(pRseRet,nGuild1,nGuild2,pRseRet->region(),nCityID,nRetSession);

		pDBRet->set_winguildid( pRseRet->winid() );

		if ( nSession == 1 )
		{
			//记录第二战公会
			DB_C_GvgBtlRet* pDBRet2 = pCity->GetDBRet(nSession + 1);
			if ( pDBRet2 != NULL )
			{
				pDBRet2->set_guildid1( pRseRet->winid() );
			}	
		}
		SYS_LOG(0,LT_CNTGVG_BTL_GET,0,0,nCityID<<pRDH->GetRegion()<<nGuild1<<nGuild2<<nSession);
		pRDH->markGvgCityDirty(pCity);
	}
}

void DealGvgEvent::HandleGvgSeeReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	e->set_cmd(C2S_RceGvgOpt);
	e->set_state(Status_Country_To_Game);
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	pRceOpt->set_type(Gvg_Report);
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();

	if ( pRseOpt->bltret_size() < 0 )
	{
		return;
	}

	GuildBtlRet* pGBtlRet = pRseOpt->mutable_bltret(0);
	CGuild* pGuild1 = pRDH->GetGuild(pGBtlRet->guildid1());
	CGuild* pGuild2 = pRDH->GetGuild(pGBtlRet->guildid2());
	if(pGuild1)
	{
		GuildData* pData1 = pGBtlRet->mutable_guild1();
		pData1->set_guildid(pGuild1->GetGuildID());
		pData1->set_guildlv(pGuild1->GetGuildLV());
		pData1->set_guildname(pGuild1->GetGuildName());
	}
	if(pGuild2)
	{
		GuildData* pData2 = pGBtlRet->mutable_guild2();
		pData2->set_guildid(pGuild2->GetGuildID());
		pData2->set_guildlv(pGuild2->GetGuildLV());
		pData2->set_guildname(pGuild2->GetGuildName());
	}
	pRseOpt->set_ret(0);
	eh_->sendEventToGamed(e,nGameID);
}


void DealGvgEvent::HandleGvgSeeCityPlayer(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}
	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
	if ( gvgStep != GvgBtlType_LineUp0 && gvgStep != GvgBtlType_LineUp1 )
	{
		pRseOpt->set_ret( 11 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	if ( pCity == NULL )
	{
		return;
	}

	int nSession = 0;
	if ( gvgStep == GvgBtlType_LineUp0 )
	{
		nSession = 1;
	}

	if ( gvgStep == GvgBtlType_LineUp1 )
	{
		nSession = 2;
	}

	if ( !pRDH->CheckCanEnterCity( pCity, pGuild, nSession ) )
	{
		pRseOpt->set_ret( 12 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	//pGuild->EnterCity( e->uid(), pRceOpt->cityid(), nSession );

	//更新数据
	GCG_GuildOpt* pGuildOpt = e->mutable_guildopt();
	pRDH->SetPlayerProp( pGuild, e->uid(), pGuildOpt );

	Country_FightSendData* pFightData = e->mutable_guildfightsenddata();
	SaveGvgBtlSendData(e->uid(),pFightData);

	pRDH->FillGvgPlayerData( pRseOpt, pRceOpt->cityid(), nSession );

	pRDH->FillGvgCityData( pRseOpt,pRceOpt->cityid());
	eh_->sendEventToGamed(e,nGameID);
}

void DealGvgEvent::HandleGvgInterface(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);

	DB_C_PubGvgBtlInfo* pPublicInfo = pRDH->GetGvgEarthInfo();
	if ( pPublicInfo == NULL )
	{
		//不会走到这里
		return;
	}

	int nFlag;
	const GvgActiveTbl* pTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pTbl == NULL )
	{
		return;
	}

	nFlag = pTbl->m_nID;

	if ( nFlag <= pPublicInfo->flag() )
	{
		//确保在城市信息之后更新
		pGuild->CheckGvg(pPublicInfo->flag());
	}

	DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	CGvgCity* pGvgCity = pRDH->GetGvgCity( pDBGvgMsg->controlcity() );
	if ( pGvgCity == NULL )
	{
		
	}
	else 
	{
		DB_C_GvgCity* pDBCity = pGvgCity->GetDBGvgCity();
		if ( pDBCity == NULL )
		{
			return;
		}
		if ( pDBCity->guildid() != pGuild->GetGuildID() )
		{
			pDBGvgMsg->set_controlcity(0);
		}
	}
		
	pGuild->FillGvgGuildData( pRseOpt );

	if ( pRseOpt->controlcityid() != 0 )
	{
		pRDH->FillGvgCityData( pRseOpt,pRseOpt->controlcityid() );
	}


	//////////////////////////////////////////////////////////
	//20141209活动问题补偿

	int nLv = 0;
	int nGetCityID = 0;
	if ( pTbl->m_nID == 2 )
	{
		for ( int i = 0; i < pDBGvgMsg->signcity_size(); i++ )
		{
			int nCityID = pDBGvgMsg->signcity(i);

			const GvgCityTbl* pTmpTbl = GvgCfg::Instance().GetCityTbl(nCityID);
			if ( pTmpTbl == NULL )
			{
				continue;
			}

			CGvgCity* pCity = pRDH->GetGvgCity(nCityID);
			if ( pCity == NULL )
			{
				continue;
			}

			bool bCanEnter = false;
			for ( int j = 1; j <= 2; j++ )
			{
				if ( pRDH->CheckCanEnterCity( pCity, pGuild, j ) )
				{
					bCanEnter = true;
				}
			}
			if ( bCanEnter )
			{
				//可以领取，判断优先城市
				if ( nCityID == 0 )
				{
					nGetCityID = pTmpTbl->m_nID;
					nLv = pTmpTbl->m_nLv;
				}
				else if ( pTmpTbl->m_nLv > nLv)
				{
					nGetCityID = pTmpTbl->m_nID;
					nLv = pTmpTbl->m_nLv;
				}
			}
		}
		pRseOpt->set_controlcityid( nGetCityID );
	}

	/////////////////////////////////////

	for ( int i = GP_Normal; i < GP_Cnt; i++ )
	{
		int nPosition = i;
		int nCnt = 0;


		DB_C_GVG_Reward_Record* pDBRecord = NULL;

		for ( int i = 0; i < pDBGvgMsg->rewardrecord_size(); i++ )
		{
			if ( pDBGvgMsg->rewardrecord(i).pos() == nPosition )
			{
				pDBRecord = pDBGvgMsg->mutable_rewardrecord(i);
				break;
			}
		}

		if ( pDBRecord == NULL )
		{
			nCnt = 0;
		}
		else
		{
			nCnt = pDBRecord->cnt();
		}

		int nLimit = GuildInfoCfg::Instance().GetPositionMaxCnt(pGuild->GetGuildLV(),(GuildPositionType)nPosition);

		pRseOpt->add_getrewardpos(nPosition);
		pRseOpt->add_getrewardcnt(nLimit - nCnt);
	}


	eh_->sendEventToGamed(e,nGameID);
}


void DealGvgEvent::HandleGvgRewardRecord(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);

	if(pDBCUser==NULL)
	{
		return;
	}

	int nPosition = pDBCUser->position();

	DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	DB_C_GVG_Reward_Record* pDBRecord = NULL;

	for ( int i = 0; i < pDBGvgMsg->rewardrecord_size(); i++ )
	{
		if ( pDBGvgMsg->rewardrecord(i).pos() == nPosition )
		{
			pDBRecord = pDBGvgMsg->mutable_rewardrecord(i);
			break;
		}
	}

	if ( pDBRecord == NULL )
	{
		pDBRecord = pDBGvgMsg->add_rewardrecord();
		pDBRecord->set_pos( nPosition );
	}

	pDBRecord->set_cnt( pDBRecord->cnt() + 1 );

	pRDH->AddGuildLog_Pos(pGuild,G_Log_GvgReward,nUserID,pDBCUser->name(),nPosition);
}

void DealGvgEvent::HandleGvgOffLineEnter(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);

	if(pDBCUser==NULL)
	{
		return;
	}

	DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}

	const GvgBaseTbl* pBaseTbl = GvgCfg::Instance().GetBaseTbl();
	if ( pBaseTbl == NULL )
	{
		return;
	}

	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
// 	if ( gvgStep != GvgBtlType_LineUp0 && gvgStep != GvgBtlType_LineUp1 )
// 	{
// 		pRseOpt->set_ret( 11 );
// 		eh_->sendEventToGamed(e,nGameID);
// 		return;
// 	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	if ( pCity == NULL )
	{
		return;
	}

	if ( !pRDH->CheckCanOffLineEnterCity( pCity, pGuild ) )
	{
		pRseOpt->set_ret( 12 );
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	GCG_GuildOpt* pGuildOpt = e->mutable_guildopt();


	for ( int i = 1; i <= MAX_FIGHT_TIME; i++ )
	{
		int nSession = i;
		pGuild->EnterCity( e->uid(), pRceOpt->cityid(), nSession );

		DB_C_GVG_Msg* pDBGvgGuild = pGuild->GetDBGvgMsg();
		if ( pDBGvgGuild == NULL )
		{
			continue;
		}

		DB_C_GvgBtlAddProp* pAddProp = pGuild->GetDBGvgAddProp( nUserID, i );
		if ( pAddProp == NULL )
		{
			pAddProp = pDBGvgGuild->add_addprop();
			pAddProp->set_userid( nUserID );
			pAddProp->set_session( i ); 

			pAddProp->set_name( pGuildOpt->selfname() );
			pAddProp->set_pic( pGuildOpt->selfpic() );

			pAddProp->set_maxwintime(pBaseTbl->m_nBaseStreak);
		}
		if ( pAddProp == NULL )
		{
			continue;
		}

		pAddProp->set_isoffline( true );
	}

	eh_->sendEventToGamed(e,nGameID);
}


void DealGvgEvent::HandleGvgOffLineLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild)
{
	RceGvgOpt* pRceOpt = e->mutable_ce_rcegvgopt();
	CountryDataHandler* pDH = eh_->getDataHandler();
	RseGvgOpt* pRseOpt = e->mutable_se_rsegvgopt();
	time_t tNow = time(NULL);
	int64 nUserID	= e->uid();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);

	if(pDBCUser==NULL)
	{
		return;
	}

	DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	if ( pRceOpt->cityid() == 0 )
	{
		return;
	}

	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}
	GvgBtlStepType gvgStep = pActiveTbl->GetGvgStep();
	// 	if ( gvgStep != GvgBtlType_LineUp0 && gvgStep != GvgBtlType_LineUp1 )
	// 	{
	// 		pRseOpt->set_ret( 11 );
	// 		eh_->sendEventToGamed(e,nGameID);
	// 		return;
	// 	}

	CGvgCity* pCity = pRDH->GetGvgCity( pRceOpt->cityid() );
	for ( int i = 1; i <= MAX_FIGHT_TIME; i++ )
	{
		int nSession = i;
		pGuild->LeaveCity( e->uid(), pRceOpt->cityid(), nSession );

		DB_C_GVG_Msg* pDBGvgGuild = pGuild->GetDBGvgMsg();
		if ( pDBGvgGuild == NULL )
		{
			continue;
		}

		DB_C_GvgBtlAddProp* pAddProp = pGuild->GetDBGvgAddProp( nUserID, i );
		if ( pAddProp == NULL )
		{
			continue;
		}
		else
		{
			pAddProp->set_isoffline( false );
		}
	}

	eh_->sendEventToGamed(e,nGameID);
}