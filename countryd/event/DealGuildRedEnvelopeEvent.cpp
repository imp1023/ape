#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealGuildRedEnvelopeEvent.h"
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
#include "../logic/GuildRedEnvelopeCfg.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealGuildRedEnvelopeEvent ::DealGuildRedEnvelopeEvent ()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealGuildRedEnvelopeEvent ::~DealGuildRedEnvelopeEvent ()
{

}

void DealGuildRedEnvelopeEvent ::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceGuildRedEnvelopeOpt, ((ProcessRoutine) DealGuildRedEnvelopeEvent ::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceOneYuanPurchase , ((ProcessRoutine) DealGuildRedEnvelopeEvent::handle_));
}

void DealGuildRedEnvelopeEvent ::handle(Event* e)
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
	case C2S_RceGuildRedEnvelopeOpt:
		{
			HandleRceGuildRedEnvelopeOpt(e,pRDH,nGameID);
		}
		break;
	case C2S_RceOneYuanPurchase:
		{
			HandYiYuanPurchaseOpt(e,pRDH,nGameID);
		}
	default:
		break;
	}
}

void DealGuildRedEnvelopeEvent::HandYiYuanPurchaseOpt(Event* e,CountryRgnDatHandler* pRDH, int nGameID)
{
	//SYS_LOG((int64)6666, LT_Guild_Dismiss,1000,1000,1);
	if (nGameID <= 0)
	{
		return;
	}
	//

	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	DB_C_UserList* pUserList = pRDH->GetMainUserList();
	if(pUserList==NULL)
	{
		return ;
	}
	//SYS_LOG((int64)6666, LT_Guild_Dismiss,1000,1000,2);
	//检测是否是本次的活动，如果不是就清空活动相关的数据
	pRDH->CheckClearOneYuanPurchaseData();

	bool bSave = false;
	DB_Region_One_Yuan*pRegionDB_One_Yuan = pUserList->mutable_regiononeyuan();

	GameActivityTbl*pTimeTbl = GameActivityTimeCfg::Instance().GetTbl(GAT_One_Yuan_Purchase);
	if(0==pRegionDB_One_Yuan->oneyuanpurchasecleartime())
	{
		pRegionDB_One_Yuan->set_oneyuanpurchasecleartime(pTimeTbl->m_nClearTime);
	}
	
	bSave = true;
	

	RceOneYuanPurchase*pRce = e->mutable_ce_rceoneyuanpurchase();
	RseOneYuanPurchase*pRse = e->mutable_se_rseoneyuanpurchase();

	DB_C_User*pUser = pRDH->GetDBCityUser(e->uid());
	if(pUser == NULL)
	{
		pUser = pRDH->AllocDBUser(e->uid());
		//pUser->set_name(pArgs->region());
		pUser->set_name(pRse->name());
		bSave = true;
	}
	//SYS_LOG((int64)6666, LT_Guild_Dismiss,1000,1000,3);

	int nType = pRce->type();
	vector<int>	lstNewKey;
	lstNewKey.clear();

	switch(nType)
	{
	case 0:  //打开
		{
			map<int, OneYuanPurchaseTbl*>::iterator itTbl = OneYuanPurchaseCfg::instance().GetMapTbl().begin();
			for(;itTbl!=OneYuanPurchaseCfg::instance().GetMapTbl().end();itTbl++)
			{
				OneYuanPurchaseTbl*pTbl = itTbl->second;
				int nFlag = pTbl->m_nID;
				OneYuan*pOY = pRse->add_oneyuan();
				pOY->set_flag(nFlag);
				pOY->set_total(pTbl->m_nTotalCopies);
				pOY->set_participation(pRDH->GetParticitionCntByFlag(nFlag));
				pOY->set_bsendprize(pTbl->m_bSendPrize);


				int nWinKey = pTbl->m_nRewardKey;
				pRse->set_rewardkey(nWinKey);
				pRse->set_num1(pTbl->m_nNum1);
				pRse->set_num2(pTbl->m_nNum2);
				//已经抽奖了
				if(pTbl->m_bSendPrize)
				{
					if(pTimeTbl!=NULL && pTimeTbl->isCurrentTime(AT_Fetch_Time))
					{
						DB_C_User*pWinner = pRDH->GetWinner(nFlag,nWinKey);
						if(pWinner!=NULL)
						{
							Winner*pWin = pOY->mutable_win();
							pWin->set_userid(pWinner->userid());
							pWin->set_region(pRDH->GetRegion());
							pWin->set_name(pWinner->name());

							DB_One_Yuan*pDBWin = pRDH->GetOneYuanDataByFlag(pWinner,nFlag);
							if(pDBWin!=NULL)
							{

								pWin->set_buycnt(pDBWin->rewardkey_size());
							}
						}
					}
						
				}
				for(int i=0;i<pRegionDB_One_Yuan->win_size();i++)
				{
					DB_One_Yuan_Purchase_Winner*pRegionWin = pRegionDB_One_Yuan->mutable_win(i);
					if(pRegionWin->flag()==nFlag)
					{
						pOY->set_bfetch(pRegionWin->bfetch());
					}
				}
			}

			if(bSave)
			{
				pRDH->markUserLstDirty(pUserList);
			}

			e->set_state(Status_Country_To_Game);
			eh_->sendEventToGamed(e,nGameID);
			//SYS_LOG((int64)6666, LT_Guild_Dismiss,1000,1000,4);
			return;
		}
		break; //购买
	case 1:
		{
			int nCnt = pRce->buycnt();
			int nFlag = pRce->giftid();
			OneYuanPurchaseTbl*pBuyTbl = OneYuanPurchaseCfg::instance().GetOneYuanTbl(nFlag);
			if(pBuyTbl==NULL)
			{
				pRse->set_ret(ER_Table_Error);
				e->set_state(Status_Region_To_Game);
				eh_->sendEventToGamed(e,nGameID);
				return ;

			}
			int nLeft = pBuyTbl->m_nTotalCopies-pRDH->GetParticitionCntByFlag(nFlag);
			if(nLeft<nCnt)
			{
				pRse->set_ret(ER_BuyCnt_Error);
			}
			else
			{

				for(int i=0;i<nCnt;i++)
				{
					pRDH->GetRewardKey(e->uid(),nFlag,lstNewKey);
				}
				bSave = true;
			}
		}
		break;
	case 2:  //领取奖励
		{
			int nFlag = pRce->giftid();
			OneYuanPurchaseTbl*pBuyTbl = OneYuanPurchaseCfg::instance().GetOneYuanTbl(nFlag);
			if(pRDH->GetWinner(nFlag,pBuyTbl->m_nRewardKey)->userid()!=e->uid())
			{
				pRse->set_ret(ER_Fetcher_Error);
				e->set_state(Status_Region_To_Game);
				eh_->sendEventToGamed(e,nGameID);
				return ;
			}

			for(int i=0;i<pRegionDB_One_Yuan->win_size();i++)
			{
				DB_One_Yuan_Purchase_Winner*pRegionWin = pRegionDB_One_Yuan->mutable_win(i);
				if(pRegionWin->flag()==nFlag)
				{
					if(pRegionWin->bfetch())
					{
						pRse->set_ret(ER_Repeat_Fetch);
						e->set_state(Status_Region_To_Game);
						eh_->sendEventToGamed(e,nGameID);
						return ;
					}
					else
					{
						pRegionWin->set_bfetch(true);
						bSave = true;
					}

				}
			}
		}
		break;
	}


	int nFlag = pRce->giftid();
	OneYuanPurchaseTbl*pTbl = OneYuanPurchaseCfg::instance().GetOneYuanTbl(nFlag);
	OneYuan*pOY = pRse->add_oneyuan();
	pOY->set_flag(nFlag);
	pOY->set_total(pTbl->m_nTotalCopies);
	pOY->set_participation(pRDH->GetParticitionCntByFlag(nFlag));
	pOY->set_bsendprize(pTbl->m_bSendPrize);

	for(int i=0; i<pUser->oneyuan_size();i++)
	{
		DB_One_Yuan*pDB = pUser->mutable_oneyuan(i);
		if(pDB->flag()==nFlag)
		{
			//把购买新获得的获奖码同步到game里
			for(int i=0;i<lstNewKey.size();i++)
			{
				string strKey = pRDH->IntTo4String(lstNewKey[i]);
				pOY->add_newkey(strKey);
			}
		}

	}
	int nWinKey = pTbl->m_nRewardKey;
	pRse->set_rewardkey(nWinKey);
	pRse->set_num1(pTbl->m_nNum1);
	pRse->set_num2(pTbl->m_nNum2);
	//已经抽奖了
	if(pTbl->m_bSendPrize)
	{
		
		if(pTimeTbl!=NULL && pTimeTbl->isCurrentTime(AT_Fetch_Time))
		{
			DB_C_User*pWinner = pRDH->GetWinner(nFlag,nWinKey);
			if(pWinner!=NULL)
			{
				Winner*pWin = pOY->mutable_win();
				pWin->set_userid(pWinner->userid());
				pWin->set_region(pRDH->GetRegion());
				pWin->set_name(pWinner->name());

				DB_One_Yuan*pDBWin = pRDH->GetOneYuanDataByFlag(pWinner,nFlag);
				if(pDBWin!=NULL)
				{

					pWin->set_buycnt(pDBWin->rewardkey_size());
				}
			}
		}
	}
	for(int i=0;i<pRegionDB_One_Yuan->win_size();i++)
	{
		DB_One_Yuan_Purchase_Winner*pRegionWin = pRegionDB_One_Yuan->mutable_win(i);
		if(pRegionWin->flag()==nFlag)
		{
			pOY->set_bfetch(pRegionWin->bfetch());
		}
	}


	if(bSave)
	{
		pRDH->markUserLstDirty(pUserList);
		pRDH->markUserLstDirty(e->uid());
	}

	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildRedEnvelopeEvent::HandleRceGuildRedEnvelopeOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if ( pRDH == NULL )
	{
		return;
	}
	e->set_state(Status_Country_To_Game);
	RceGuildRedEnvelopeOpt* pRceOpt = e->mutable_ce_rceguildredenvelopeopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildRedEnvelopeOpt* pRseOpt = e->mutable_se_rseguildredenvelopeopt();

	int nType = pRceOpt->opttype();

	switch( nType )
	{
    case E_GUILD_REDENVELOPE_OPEN:
        HandleOpen(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_ISSUE:
        HandleIssueRedEnvelope(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_SNAPPE:
        HandleSnappeRedEnvelope(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_REDENVELOPEINFO:
        HandleCheckRedEnvelopeInfo(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_ALLREDENVELOPE:
        HandleGetRedEnvelopeList(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_GUILDREDENVELOPE:
        HandleGetRedEnvelopeList(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_PERSONALLOG:
        HandleGetRedEnvelopeList(e, pRDH, nGameID);
        break;
    case E_GUILD_REDENVELOPE_CHECKRANK:
        HandleCheckRank(e, pRDH, nGameID);
        break;
	default: 
		break;
	}
}

void DealGuildRedEnvelopeEvent::HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    //
}

void DealGuildRedEnvelopeEvent::HandleCheckRedEnvelopeInfo(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RseGuildRedEnvelopeOpt *pRse = e->mutable_se_rseguildredenvelopeopt();
    RceGuildRedEnvelopeOpt *pRce = e->mutable_ce_rceguildredenvelopeopt();

    CGuildRedEnvelope * pGuildRedEnvelope = pRDH->GetGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        eh_->sendEventToGamed(e, nGameID);
        return;
    }

    if (pGuildRedEnvelope->GetFlag() <= 0)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_ACTOVERDUE);
        eh_->sendEventToGamed(e, nGameID);
        return;
    }

    pGuildRedEnvelope->FillRedEnveLopeInfo(pRse, e->uid(), pRce->nguildid(), pRce->nid());
    eh_->sendEventToGamed(e, nGameID);
}

void DealGuildRedEnvelopeEvent::HandleIssueRedEnvelope(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RseGuildRedEnvelopeOpt *pRse = e->mutable_se_rseguildredenvelopeopt();
    RceGuildRedEnvelopeOpt *pRce = e->mutable_ce_rceguildredenvelopeopt();

    CGuildRedEnvelope * pGuildRedEnvelope = pRDH->GetGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        eh_->sendEventToGamed(e, nGameID);
        return;
    }
    int nResult = pGuildRedEnvelope->AddRedEnvelope(e->uid(), pRce->redtype(), pRce->ntotalcredit(), pRce->ncopies(), pRse);
    pRse->set_nresult(nResult);
    eh_->sendEventToGamed(e, nGameID);
}
void DealGuildRedEnvelopeEvent::HandleCheckRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    //
}
void DealGuildRedEnvelopeEvent::HandleSnappeRedEnvelope(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RseGuildRedEnvelopeOpt *pRse = e->mutable_se_rseguildredenvelopeopt();
    RceGuildRedEnvelopeOpt *pRce = e->mutable_ce_rceguildredenvelopeopt();

    CGuildRedEnvelope * pGuildRedEnvelope = pRDH->GetGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        eh_->sendEventToGamed(e, nGameID);
        return;
    }
    if (pRse == NULL || eh_ == NULL)
    {
        return;
    }

    pGuildRedEnvelope->SnappeRedEnvelope(pRse, e->uid(), pRce->nguildid(), pRce->nid(), eh_);
    eh_->sendEventToGamed(e, nGameID);
}
void DealGuildRedEnvelopeEvent::HandleGetRedEnvelopeList(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
    RseGuildRedEnvelopeOpt *pRse = e->mutable_se_rseguildredenvelopeopt();
    RceGuildRedEnvelopeOpt *pRce = e->mutable_ce_rceguildredenvelopeopt();

    CGuildRedEnvelope * pGuildRedEnvelope = pRDH->GetGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        eh_->sendEventToGamed(e, nGameID);
        return;
    }

    pGuildRedEnvelope->FillGuildRedEnvelopeInfo(pRse, e->uid());
    eh_->sendEventToGamed(e, nGameID);
}