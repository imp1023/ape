#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealGuildEvent.h"
#include "../CountryRgnDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../common/const_def.h"
#include "../../event/EventDefine.h"
#include "../../logic/StoreItemCfg.h"

#include <math.h>
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../CountryConfigMgr.h"
#include "../logic/GuildInfoCfg.h"
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../GuildBtlDBHandler.h"

extern ServerConfig serverConfig;
extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealGuildEvent::DealGuildEvent()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealGuildEvent::~DealGuildEvent()
{

}

void DealGuildEvent::registHandler()
{
	eh_->getEventHandler()->registHandler(C2S_RceGuildOpt, ((ProcessRoutine) DealGuildEvent::handle_));	
	eh_->getEventHandler()->registHandler(C2S_RcePurchase, ((ProcessRoutine) DealGuildEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_GUILDBTL_CACU, ((ProcessRoutine) DealGuildEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_GUILDBTL_SEEREPORT, ((ProcessRoutine) DealGuildEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_ADMIN_ADDGUILDBTLREWARD, ((ProcessRoutine) DealGuildEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_ADMIN_GVELIST, ((ProcessRoutine) DealGuildEvent::handle_));
}

void DealGuildEvent::handle(Event* e)
{	
	GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
	int nGameID = pNeedArgs->gamesrvid();
	int nRegion = pNeedArgs->region();

	CountryDataHandler* pDH= eh_->getDataHandler();
	if(!pDH->IsRightRegion(nRegion))
		return;//分区错误
	CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
	if(pRDH==NULL)
		return;

	switch(e->cmd())
	{
	case C2S_RceGuildOpt:
		HandleRceGuildOpt(e,pRDH,nGameID);
		break;
	case C2S_RcePurchase:
		HandleRcePurchase(e,pRDH,nGameID);
		break;
	case EVENT_GUILDBTL_CACU:
		HandleGuildBtlCacu(e,pRDH,nGameID);
		break;
	case EVENT_GUILDBTL_SEEREPORT:
		HandleGuildBtlSeeReport(e,pRDH,nGameID);
		break;
	case EVENT_ADMIN_ADDGUILDBTLREWARD:
		HandleAdminAddGuildBtlRedward(e,pRDH,nGameID);
		break;
	case EVENT_ADMIN_GVELIST:
		HandleAdminGveList(e,pRDH,nGameID);
		break;
	default:
		break;
	}
} 

void DealGuildEvent::HandleAdminAddGuildBtlRedward(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	Admin_AddGuildBtlReward_Req* req = e->mutable_adminaddguildbtlreward_req();
	Admin_AddGuildBtlReward_Rsp* rsp = e->mutable_adminaddguildbtlreward_rsp();
	int nGuildID	= req->guildid();
	int nRank		= req->guildrank();
	rsp->set_gid(req->gid());
	rsp->set_adminfd(req->adminfd());
	rsp->set_sid(req->sid());
	rsp->set_region(req->region());
	rsp->set_guildid(nGuildID);
	rsp->set_guildrank(req->guildrank());
	rsp->set_ret(0);

	CGuild* pGuild = pRDH->GetGuild(nGuildID);
	if(pGuild == NULL)
	{
		rsp->set_ret(1);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	pRDH->AddGuildBtlReward(pGuild,nRank);
	e->set_state(Admin_CG_Rsq);
	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::HandleAdminGveList(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	int nGuildID	= req->guildid();
	int nType		= req->level();
	int nGveCity	= req->bldid();

	CGuild* pGuild = pRDH->GetGuild(nGuildID);
	if(pGuild == NULL)
	{
		return;
	}

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}
	
	string sResult;
	sResult.clear();

	if ( nType == 1 )
	{
		//gve上次进度
		
		sResult += pDBCity->lastmsg().gvestep();
		sResult += "|";

		for(int i=0;i<(int)pDBCity->npc_size();i++)
		{
			DB_C_NPC* pUser = pDBCity->mutable_npc(i);
			if(pUser->npcid()<=0)
				continue;
			int nNPCTblID = pUser->npcid()/COUNTRY_NPC_RATE;
			
			sResult += toString(pUser->npcid());
			sResult += ",";
			sResult += toString(pUser->morale());
			sResult += ";";
		}
	}
	else if ( nType == 2 )
	{
		for ( int i = 0; i < pDBCity->lastlist_size(); i++ )
		{
			DB_GVE_List* pDBList = pDBCity->mutable_lastlist(i);
			if ( pDBList == NULL )
			{
				continue;
			}

			sResult += toString(pDBList->uid());
			sResult += ",";
			sResult += toString(pDBList->gvepoint());
			sResult += ";";
		}
	}
	else if ( nType == 3 )
	{
		map<int64,DB_C_User*>::iterator iter,iterTmp;
		iter = pGuild->m_mapGuildUser.begin();
		while(iter != pGuild->m_mapGuildUser.end())
		{
			DB_C_User* pMember = iter->second;
			if(pMember==NULL||pMember->guildid()!=pGuild->GetGuildID())
			{
				iter++;
				continue;
			}
			sResult += toString(pMember->userid());
			sResult += ",";
			sResult += toString(nGveCity);
			sResult += ";";
			iter++;
		}	

	}
	else if ( nType == 4 )
	{
		DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
		pDBGvgMsg->clear_signcity();
		pDBGvgMsg->add_signcity( nGveCity );
	}
	SYS_LOG(req->guildid(),LT_GVE_COUNGRY_GVELIST,0,0,nType<<sResult.c_str()<<nGveCity);
}

void DealGuildEvent::HandleRcePurchase(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	e->set_state(Status_Country_To_Game);

	CountryDataHandler* pDH= eh_->getDataHandler();
	RcePurchase* pRcePurchase = e->mutable_ce_rcepurchase();
	RsePurchase* pRsePurchase = e->mutable_se_rsepurchase();
	int64 nUserID = e->uid();
	int nShopID = pRcePurchase->shopid();
	const ShopItemTbl* pShopTbl = StoreItemCfg::Instance().GetShopItemTbl(nShopID);
	if(pShopTbl==NULL)
	{
		return;
	}
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL||pDBCUser->guildid()<=0)
	{
		pRsePurchase->set_error(OR_NoGuild);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	CGuild* pGuild = pRDH->GetGuild(pDBCUser->guildid());
	if(pGuild==NULL)
	{
		pRsePurchase->set_error(OR_NoGuild);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	

	bool bNeedSave = pRDH->CheckGuildShopRecord(pGuild);
	DB_C_Guild*	pDBCGuild	= pGuild->GetDBGuild();
	DB_C_GuildShop* pDBShop	= pDBCGuild->mutable_guildshop(); 
	GuildPositionType emPmt = GP_Normal;
	for(int i=0;i<pDBShop->shoppmt_size();i++)
	{
		if(pDBShop->mutable_shoppmt(i)->shopid()==nShopID)
		{
			emPmt = (GuildPositionType)pDBShop->mutable_shoppmt(i)->position();
			break;
		}
	}
	if(emPmt != GP_Normal)
	{
		int nPositon = pDBCUser->position();
		if(nPositon==GP_Normal || nPositon > emPmt)
		{
			pRsePurchase->set_error(OR_NoPermit);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}
	if(pShopTbl->m_nShopType == SHOP_TYPE_GUILDMYSTIC)
	{//神秘商城，先判断次物品是否在卖
		bool bFind = false;
		for(int i=0;i<pDBShop->mysticshopsaleid_size();i++)
		{
			if(pDBShop->mysticshopsaleid(i)==nShopID)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			pRsePurchase->set_error(OR_OutTime);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}
	if(pShopTbl->m_mapGuildSaleCnt.size()>0)
	{//有限购
		int nGuildLV = pGuild->GetDBGuild()->mutable_build(GB_Shop)->level();
		int nMaxCnt = 0;
		map<int,int>::const_iterator iter = pShopTbl->m_mapGuildSaleCnt.find(nGuildLV);
		if(iter != pShopTbl->m_mapGuildSaleCnt.end())
		{
			nMaxCnt = iter->second;
		}
		
		int nSaleCnt = 0;
		int nIdx = 0;
		bool bFnid = false;
		for(nIdx=0;nIdx<pDBShop->buyrecord_size();nIdx++)
		{
			if(pDBShop->mutable_buyrecord(nIdx)->daybuyshopid()==nShopID)
			{
				nSaleCnt = pDBShop->buyrecord(nIdx).daybuyshopcnt();
				bFnid = true;
				break;
			}
		}
		if(nSaleCnt+pRcePurchase->buynum() > nMaxCnt)
		{//超出最大上限
			pRsePurchase->set_error(OR_GuildShopUpLimit);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		if(bFnid)
		{
			pDBShop->mutable_buyrecord(nIdx)->set_daybuyshopcnt(nSaleCnt+pRcePurchase->buynum());
		}
		else
		{
			DB_C_GuildBuyRecord* pDBRecord = pDBShop->add_buyrecord();
			pDBRecord->set_daybuyshopid(nShopID);
			pDBRecord->set_daybuyshopcnt(pRcePurchase->buynum());
		}
		bNeedSave = true;
	}
	if(bNeedSave)
	{
		pRDH->markGuildLstDirty(pGuild);
	}
	pRsePurchase->set_error(OR_Success);
	//添加商城信息

	for(int i=0;i<pDBShop->buyrecord_size();i++)
	{
		DB_C_GuildBuyRecord* pDBRecord = pDBShop->mutable_buyrecord(i);
		if(pDBRecord->daybuyshopid()<=0)
			continue;
		pRsePurchase->add_guildshopsaleid(pDBRecord->daybuyshopid());
		pRsePurchase->add_guildshopsalecnt(pDBRecord->daybuyshopcnt());
	}

	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::CheckGuildUser(int64 nUserID,GCG_GuildOpt* pSrvOpt,CountryRgnDatHandler* pRDH)
{
	CountryDataHandler* pDH= eh_->getDataHandler();
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		//此处一定出现了错误
		pDBCUser = pRDH->AllocDBUser(nUserID);
		pDBCUser->set_countryid(pSrvOpt->country());
		pDBCUser->set_cityid(CountryInfoCfg::Instance().GetCapital(pSrvOpt->country()));

		CGuild* pNewGuild = pRDH->GetGuild(pSrvOpt->guildid());
		if(pNewGuild&&pNewGuild->GetCountry()==pSrvOpt->country())
		{
			pDBCUser->set_guildid(pSrvOpt->guildid());
			
			if(pNewGuild->GetDBGuild()->leaderid()==nUserID)
			{//遗失的会长~~~~~
				pDBCUser->set_position(GP_Leader);
			}
			else
			{
				pDBCUser->set_position(GP_Normal);
			}
			pNewGuild->AddUser(pDBCUser);
			//pRDH->markGuildLstDirty(pNewGuild);
		}
		pRDH->markUserLstDirty(nUserID);
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_UserNotFind4Guild<<0<<pSrvOpt->country()<<pSrvOpt->country());
	}
	if(pDBCUser->guildid()!=pSrvOpt->guildid())
	{//工会ID不一致，以国家服务器为准
		if(pSrvOpt->country()!=pDBCUser->countryid())
		{//国家ID不一致，可能出现的原因是玩家叛国时服务器当机
			CGuild* pGuild = pRDH->GetGuild(pDBCUser->guildid());
			SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_GuildCountryError<<0<<pDBCUser->countryid()<<pSrvOpt->country()<<pDBCUser->guildid()<<pDBCUser->guildid());
			if(pGuild)
			{
				pGuild->EraseUser(nUserID);
				pDBCUser->set_guildid(0);
				pSrvOpt->set_guildid(0);
			}
		}
		else
		{
			SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_GuildIDError<<0<<pDBCUser->countryid()<<pSrvOpt->country()<<pDBCUser->guildid()<<pSrvOpt->guildid());

			int nDBUserGuildID = pDBCUser->guildid();
			CGuild* pGuild = pRDH->GetGuild(pSrvOpt->guildid());
			if(pGuild)
			{
				pGuild->EraseUser(nUserID);
				pDBCUser->set_guildid(0);
				pSrvOpt->set_guildid(0);
			}
			CGuild* pGuild1 = pRDH->GetGuild(nDBUserGuildID);
			if(pGuild1)
			{
				pDBCUser->set_guildid(nDBUserGuildID);
				pGuild1->AddUser(pDBCUser);
			}
			pSrvOpt->set_guildid(pDBCUser->guildid());
			
			//pSrvOpt->set_guildid(pDBCUser->guildid());
		}
	}

	CGuild* pGuild = pRDH->GetGuild(pSrvOpt->guildid());
	if ( pGuild != NULL )
	{
		pGuild->CheckGve();
	}
}

void DealGuildEvent::HandleGuildBtlSeeReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//此处将包改为S2C_RseGuildOpt
	e->set_cmd(C2S_RceGuildOpt);
	e->set_state(Status_Country_To_Game);
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	pRceOpt->set_type(GOT_GBtlSeeLastBtl);
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	GuildBtlRet* pGBtlRet = pRseOpt->mutable_bltret();
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

void DealGuildEvent::HandleGuildBtlCacu(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(!GuildInfoCfg::Instance().IsGuildBtlEnable(pRDH->GetRegion()))
		return;

	if(e->state() == Status_Normal_Game)
	{//在此取包，将双方的工会加成塞进去
		GuildBtl_FightCacuData* pGBCacuData = e->mutable_guildbtlcacudata();
		int nGuildID1 = pGBCacuData->guildid1();
		int nGuildID2 = pGBCacuData->guildid2();
		CGuild* pGuild1 = pRDH->GetGuild(nGuildID1);
		if(pGuild1)
		{
			int nTurns = pGBCacuData->session();
			DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pGuild1,nTurns);
			if(pDBResult->result()!=GBR_Unknow)
			{//次工会PK已经结束
				return;
			}
			DB_C_GuildBtl* pDBGBtl = pGuild1->GetDBGuild()->mutable_btlinfo();
			for(int i=0;i<pDBGBtl->addprop_size();i++)
			{
				DB_C_GuildBtlAddProp* pDBProp = pDBGBtl->mutable_addprop(i);
				GuildBtl_AddProp* pAddProp = pGBCacuData->add_addprop();
				pAddProp->set_userid(pDBProp->userid());
				pAddProp->set_addatkpct(pDBProp->addatkpct());
				pAddProp->set_addbloodpct(pDBProp->addbloodpct());
			}
		}
		CGuild* pGuild2 = pRDH->GetGuild(nGuildID2);
		if(pGuild2)
		{
			int nTurns = pGBCacuData->session();
			DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pGuild2,nTurns);
			if(pDBResult->result()!=GBR_Unknow)
			{//次工会PK已经结束
				return;
			}

			DB_C_GuildBtl* pDBGBtl = pGuild2->GetDBGuild()->mutable_btlinfo();
			for(int i=0;i<pDBGBtl->addprop_size();i++)
			{
				DB_C_GuildBtlAddProp* pDBProp = pDBGBtl->mutable_addprop(i);
				GuildBtl_AddProp* pAddProp = pGBCacuData->add_addprop();
				pAddProp->set_userid(pDBProp->userid());
				pAddProp->set_addatkpct(pDBProp->addatkpct());
				pAddProp->set_addbloodpct(pDBProp->addbloodpct());
			}
		}
		eh_->sendEventToGamed(e,nGameID);
		SYS_LOG(pGBCacuData->cityid(),LT_CNTGUILD_BTL_SEND,0,0,pRDH->GetRegion()<<nGuildID1<<nGuildID2<<pGBCacuData->session()<<pGBCacuData->way()<<pGBCacuData->cityid()<<pGBCacuData->maxplayercnt());
	}
	else if(e->state() == Status_Game_To_Country)
	{//game返回
		GuildBtlRet* pRseRet = e->mutable_guildbtlret();
		int nGuild1 = pRseRet->guildid1();
		int nGuild2 = pRseRet->guildid2();
		int nTurns = pRseRet->session();
		CGuild* pGuild1 = pRDH->GetGuild(nGuild1);
		CGuild* pGuild2 = pRDH->GetGuild(nGuild2);

		if(pGuild1)
		{
			DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pGuild1,nTurns);
			if(pRseRet->mutable_way0()->winguildid()!=0 && pDBResult->winway0()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay0 = pRseRet->mutable_way0();
				pDBResult->set_winway0((pRetWay0->winguildid()==pGuild1->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
				//存入数据库
				GuildBtlDBHandler::Instance().SafePushGuildBtlResult(pRetWay0,nGuild1,nGuild2,pRDH->GetRegion(),nTurns,0);
			}
			if(pRseRet->mutable_way1()->winguildid()!=0 && pDBResult->winway1()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay1 = pRseRet->mutable_way1();
				pDBResult->set_winway1((pRetWay1->winguildid()==pGuild1->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
				//存入数据库
				GuildBtlDBHandler::Instance().SafePushGuildBtlResult(pRetWay1,nGuild1,nGuild2,pRDH->GetRegion(),nTurns,1);
			}
			if(pRseRet->mutable_way2()->winguildid()!=0 && pDBResult->winway2()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay2 = pRseRet->mutable_way2();
				pDBResult->set_winway2((pRetWay2->winguildid()==pGuild1->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
				//存入数据库
				GuildBtlDBHandler::Instance().SafePushGuildBtlResult(pRetWay2,nGuild1,nGuild2,pRDH->GetRegion(),nTurns,2);
			}

			if(pDBResult->result()==GBR_Unknow&&
				pDBResult->winway0()!=GBOResult_Unknow&&
				pDBResult->winway1()!=GBOResult_Unknow&&
				pDBResult->winway2()!=GBOResult_Unknow)
			{//三路都计算完毕
				pRDH->CacuGuildBtlScore(pGuild1,nTurns);
			}
			pRDH->markGuildLstDirty(pGuild1);
		}
		if(pGuild2)
		{
			DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pGuild2,nTurns);
			if(pRseRet->mutable_way0()->winguildid()!=0 && pDBResult->winway0()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay0 = pRseRet->mutable_way0();
				pDBResult->set_winway0((pRetWay0->winguildid()==pGuild2->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
			}
			if(pRseRet->mutable_way1()->winguildid()!=0 && pDBResult->winway1()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay1 = pRseRet->mutable_way1();
				pDBResult->set_winway1((pRetWay1->winguildid()==pGuild2->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
			}
			if(pRseRet->mutable_way2()->winguildid()!=0 && pDBResult->winway2()==GBOResult_Unknow)
			{
				GuidlBtlRetWay* pRetWay2 = pRseRet->mutable_way2();
				pDBResult->set_winway2((pRetWay2->winguildid()==pGuild2->GetGuildID())?GBOResult_Win:GBOResult_Lose);	
			}
			if(pDBResult->result()==GBR_Unknow&&
				pDBResult->winway0()!=GBOResult_Unknow&&
				pDBResult->winway1()!=GBOResult_Unknow&&
				pDBResult->winway2()!=GBOResult_Unknow)
			{//三路都计算完毕
				pRDH->CacuGuildBtlScore(pGuild2,nTurns);
			}
			pRDH->markGuildLstDirty(pGuild2);
		}

		bool bRet = true;
		CCountry* pECountry = pRDH->GetEarthCountry();
		DB_C_PubGuildBtlInfo* pDBPGB = pECountry->GetDBCountry()->mutable_pubguildbtlinfo();
		for(int i = pRDH->m_GuildBtlReceive[nTurns]; i < pDBPGB->pklst_size(); i++)
		{
			DB_C_PubGuildPKGroup* pDBGroup = pDBPGB->mutable_pklst(i);
			int nGuildID1 = pDBGroup->guildid1();
			int nGuildID2 = pDBGroup->guildid2();
			if(nGuildID1<=0&&nGuildID2<=0){
				pRDH->m_GuildBtlReceive[nTurns]++;
				continue;
			}

			CGuild* pGuild1 = pRDH->GetGuild(pDBGroup->guildid1());
			CGuild* pGuild2 = pRDH->GetGuild(pDBGroup->guildid2());
			if( pGuild1 == NULL || pGuild2 == NULL){
				pRDH->m_GuildBtlReceive[nTurns]++;
				continue;
			}
			DB_C_GuildBtlResult* pDBRet1 = pRDH->GetDBGuildBtlResult(pGuild1,nTurns);
			DB_C_GuildBtlResult* pDBRet2 = pRDH->GetDBGuildBtlResult(pGuild2,nTurns);
			if(pDBRet1->winway0()!=0 && pDBRet1->winway1()!=0 && pDBRet1->winway2()!=0 && 
				pDBRet2->winway0()!=0 && pDBRet2->winway1()!=0 && pDBRet2->winway2()!=0){
				//该组战斗已经结束
				pRDH->m_GuildBtlReceive[nTurns]++;
			}else{
				bRet = false;
				break;
			}
		}
		if(bRet){//本轮工会战结束
			pRDH->m_GuildBtlReceive[nTurns] = 0;
			pRDH->SetCurTurnsPKOver(nTurns,true);
			SYS_LOG(0,LT_GBtl_TurnsOver,pRDH->GetRegion(),0,nTurns<<pDBPGB->pklst_size());
			pDBPGB->set_pklstidx(0);
			pDBPGB->clear_pklst();
			pRDH->SortPKGuild();
			pRDH->markCountryDirty(pECountry);
		}
	}
}

void DealGuildEvent::HandleRceGuildOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	int nType		= pRceOpt->type();
	int64 nUserID	= e->uid();
	e->set_state(Status_Country_To_Game);
	
	if(nType == GOT_Open)
	{
		CheckGuildUser(nUserID,pSrvOpt,pRDH);
	}
	DB_C_User* pDBCUser = pRDH->GetDBCityUser(nUserID);
	if(pDBCUser==NULL)
	{
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_UserNotFind4Guild<<0<<pSrvOpt->country()<<pSrvOpt->country());
		return;
	}
	if(pDBCUser->guildid()!=pSrvOpt->guildid())
	{
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_GuildIDError<<0<<pDBCUser->countryid()<<pSrvOpt->country()<<pDBCUser->guildid()<<pSrvOpt->guildid());
		return;
	}
	
	switch (nType)
	{
	case GOT_Open:
		{
			Handle_GuildPageOpen(pDBCUser,e,pRDH,nGameID);
		}
		break;
	case GOT_ChgShowGuild:
		{
			pRseOpt->set_type(pRceOpt->type());
			GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
			pRDH->FillRandGuildLst(pRseOpt,pSrvOpt,pDBCUser->countryid());
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case GOT_FindGuild:
		{
			int nTarGuildID = pRceOpt->guildid();
			CGuild* pGuild = pRDH->GetGuild(nTarGuildID);
			if(pGuild==NULL)
			{
				pSrvOpt->set_ret(3);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			if(pDBCUser->guildid()<=0&&pGuild->GetCountry()!=pDBCUser->countryid())
			{
				pSrvOpt->set_ret(37);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			GuildData* pGData = pRseOpt->add_guildlst();
			pRDH->FillGuildData(pGuild,pGData);	
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case GOT_AllGuildLst:
		{
			pRDH->FillGuildLst(pRseOpt,pRceOpt->tarcountry(),pRceOpt->page());
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	//case GOT_CountryGuildLst:
	//	{
	//		pRDH->FillGuildLst(pRseOpt,pSrvOpt->country(),pRceOpt->page(),pSrvOpt->plattype());
	//		eh_->sendEventToGamed(e,nGameID);
	//	}
	//	break;
	case GOT_CltCreateGuild:
		{//此处判断工会能否创建，并不创建工会
			string strName = pRceOpt->guildname();
			if(pDBCUser->guildid()>0)
			{
				pSrvOpt->set_ret(4);
			}
			else if(!pRDH->CanGuildNameUse(strName))
			{
				pSrvOpt->set_ret(8);
			}
			else
			{
				pSrvOpt->set_ret(0);
			}
			eh_->sendEventToGamed(e,nGameID);
		}
		break;
	case GOT_SrvCreateGuild:
		{
			Handle_SrvCreateGuild(pDBCUser,e,pRDH,nGameID);
		}
		break;
	case GOT_ApplyGuild:
		{
			Handle_ApplyGuild(pDBCUser,e,pRDH,nGameID);
		}
		break;
	case GOT_CancelApply:
		{
			int nGuildID = pRceOpt->guildid();
			CGuild* pGuild = pRDH->GetGuild(nGuildID);
			if(pGuild==NULL)
			{
				return;
			}
			bool bFind = false;
			DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
			for(int i=0;i<pDBGuild->applyuserid_size();i++)
			{
				if(pDBGuild->applyuserid(i)==nUserID)
				{
					pDBGuild->set_applyuserid(i,0);
					bFind = true;
					break;
				}
			}
			if(bFind)
			{
				pRDH->markGuildLstDirty(pGuild);
			}
		}
		break;
	case GOT_ChgLeaderByDelate:
		{
			HandleChgLdeaderByDelate(pDBCUser,e,pRDH,nGameID);
		}
		break;
	default:
		{//下列命令都需要有工会才能执行
			int nGuildID = pSrvOpt->guildid();
			CGuild* pGuild = pRDH->GetGuild(nGuildID);
			if(pGuild==NULL)
			{
				pSrvOpt->set_ret(3);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			switch(nType)
			{
			case GOT_ChgBulletin:
				{
					pGuild->GetDBGuild()->set_guildbulletin(pRceOpt->guildbulletin());
					pRDH->markGuildLstDirty(pGuild);
					pSrvOpt->set_ret(0);

					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_ChgSlogan:
				{
					pGuild->GetDBGuild()->set_guildslogan(pRceOpt->guildslogan());
					pRDH->markGuildLstDirty(pGuild);
					pSrvOpt->set_ret(0);
					
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_QuitGuild:
				{//退出工会
					int nPosition = pDBCUser->position();
					//如果已经报名参加夺城战不能退会
					if(pGuild->IsUserInGvgBtl(nUserID)){
						pSrvOpt->set_ret(216);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pGuild->IsUserSignUpBtl(nUserID))
					{//如果已报名工会战，则不能退出工会
						pSrvOpt->set_ret(57);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(nPosition == GP_Leader && pGuild->GetUserCnt()>1)
					{//会长只有剩下一个人的时候才可以退出工会
						pSrvOpt->set_ret(30);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					pGuild->EraseUser(nUserID);
					pDBCUser->set_guildid(0);
					pDBCUser->set_position(0);
					pDBCUser->set_gvepoint(0);
                    pDBCUser->set_tlastleaveguildtime((int)time(NULL));

					pRDH->AddGuildLog(pGuild,G_Log_Quit,nUserID,pSrvOpt->selfname());

					if(nPosition == GP_Leader)
					{//工会解散
						pRDH->DismissGuild(pGuild);	
					}
					//官员身份取消
					if(pDBCUser->officerposition()>0)
					{
						pRDH->ClearUserOfficerPosition(pDBCUser,E_Officer_Type_Guild);
						eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);
					}
					pRDH->markUserLstDirty(nUserID);
					pRDH->markGuildLstDirty(pGuild);
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_DelateLeader:
				{//弹劾会长
					int64 nLeaderID = 0;
					safe_atoll(pRceOpt->taruserid(),nLeaderID);
					if(pGuild->GetDBGuild()->leaderid() != nLeaderID)
					{//不会走到这里
						return;
					}
					time_t ltNow = time(NULL);
					time_t ltLast = pGuild->GetDBGuild()->leaderlasttm();
					if(ltNow - ltLast < GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nDelateLeaderTime)
					{
						pSrvOpt->set_ret(35);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					pGuild->GetDBGuild()->set_delatetm(time(NULL));

					//存一个本次第一个弹劾会长的会员
					if (!pGuild->GetDBGuild()->has_delateid() || pGuild->GetDBGuild()->delateid() <= 0)
					{
						pGuild->GetDBGuild()->set_delateid(nUserID);
					}
					pRDH->markGuildLstDirty(pGuild);

					SYS_LOG(nUserID,LT_Guild_DelateLeader,pRDH->GetRegion(),0,pGuild->GetGuildID()<<nLeaderID);

					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_GuildLog:
				{
					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					int nIdx = 0;
					for(int i=0;i<pDBGuild->log_size();i++)
					{
						DB_C_GuildLog* pDBLog = pDBGuild->mutable_log(i);
						GuildLog* pLog = pRseOpt->add_log();
						pLog->set_logtype(pDBLog->logtype());
						pLog->set_time(pDBLog->time());
						pLog->set_userid(pDBLog->userid());
						pLog->set_username(pDBLog->username());
						pLog->set_userpos(pDBLog->userpos());
						pLog->set_bldid(pDBLog->bldid());
						pLog->set_bldlv(pDBLog->bldlv());
						pLog->set_paytype(pDBLog->paytype());
						pLog->set_contribute(pDBLog->gscore());
						pLog->set_itemid(pDBLog->itemid());
						pLog->set_itemcnt(pDBLog->itemcnt());
						pLog->set_bldvalue(pDBLog->bldvalue());
						pLog->set_pkrank(pDBLog->pkrank());
						pLog->set_pkrank1(pDBLog->pkrank1());
					}
					pSrvOpt->set_ret(0);
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_GuildCoinLog:
				{
					static int nHour = 3600;
					if(pGuild->GetCheckLogTM()<=0||pGuild->GetCheckLogTM() - time(NULL) > nHour )
					{
						pRDH->ResetGuildLog(pGuild);
						pRDH->markGuildLstDirty(pGuild);
					}
					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					int nIdx = 0;
					for(int i=0;i<pDBGuild->coinlog_size();i++)
					{
						DB_C_GuildLog* pDBLog = pDBGuild->mutable_coinlog(i);
						GuildLog* pLog = pRseOpt->add_log();
						pLog->set_logtype(pDBLog->logtype());
						pLog->set_time(pDBLog->time());
						pLog->set_userid(pDBLog->userid());
						pLog->set_username(pDBLog->username());
						pLog->set_userpos(pDBLog->userpos());
						pLog->set_bldid(pDBLog->bldid());
						pLog->set_bldlv(pDBLog->bldlv());
						pLog->set_paytype(pDBLog->paytype());
						pLog->set_contribute(pDBLog->gscore());
					}
					pSrvOpt->set_ret(0);
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_PickWelfare:
				break;
			case GOT_Contribute:
				{//工会捐献
					int nContributeID = pRceOpt->contributeid();
					const GuildContributeTbl* pTbl = GuildInfoCfg::Instance().GetGuildContributeTbl(nContributeID);
					if(pTbl==NULL)
					{
						return;
					}
					pGuild->GetDBGuild()->set_bldvalue(pGuild->GetDBGuild()->bldvalue()+pTbl->m_nBuildValue);
					pGuild->GetDBGuild()->set_totalbldvalue(pGuild->GetDBGuild()->totalbldvalue()+pTbl->m_nBuildValue);

					DB_C_GVG_Msg* pDBGvg = pGuild->GetDBGvgMsg();
					if ( pDBGvg != NULL )
					{
						time_t ltNow = time( NULL );
						const GvgActiveTbl* pGvgTbl = GvgCfg::Instance().GetActiveTbl( ltNow );
						if ( pGvgTbl != NULL )
						{
							if ( pGvgTbl->GetGvgStep( ltNow ) >= GvgBtlType_PreTime )
							{
								pDBGvg->set_contributerecorcd( pDBGvg->contributerecorcd() + pTbl->m_nContribute );
							}
							else
							{
								pDBGvg->set_contribute( pDBGvg->contribute() + pTbl->m_nContribute );
							}
						}
					}
		
					SYS_LOG(nUserID,LT_Guild_Contribute,pRDH->GetRegion(),0,pTbl->m_emCostType<<pTbl->m_nCostCoinCnt<<pTbl->m_nBuildValue<<pGuild->GetDBGuild()->bldvalue()<<pGuild->GetDBGuild()->totalbldvalue());

					pRDH->AddGuildLog_Pay(pGuild,G_Log_Contribute,nUserID,pSrvOpt->selfname(),pTbl->m_emCostType,pTbl->m_nContribute);

					pRDH->markGuildLstDirty(pGuild);
					pSrvOpt->set_ret(0);
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);

					//同步GVG贡献数据
					e->set_cmd( C2S_RceGvgOpt );
					RceGvgOpt* pRceGvgOpt = e->mutable_ce_rcegvgopt();
					RseGvgOpt* pRseGvgOpt = e->mutable_se_rsegvgopt(); 
					pRceGvgOpt->set_type( Gvg_ContributeData );
					pGuild->FillGvgGuildData( pRseGvgOpt );
					if ( pRseGvgOpt->controlcityid() != 0 )
					{
						pRDH->FillGvgCityData( pRseGvgOpt,pRseGvgOpt->controlcityid() );
					}

					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_OpenMemberPage:
				{//发送成员列表
					Handle_OpenMemberPage(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_ChgLeader:
				{
					Handle_ChgLeader(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_UpPos:
			case GOT_DownPos:
				{
					int64 nTID = pSrvOpt->taruserid();
					DB_C_User* pTarUser = pGuild->GetMember(nTID);
					if(pTarUser==NULL)
					{
						pSrvOpt->set_ret(16);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					int emPos = pTarUser->position();
					int nTarPos = pRceOpt->tarposition();
					if(emPos==GP_Leader||nTarPos==GP_Leader)
					{//会长不能被降职或者升职
						return;
					}
					if(nType == GOT_UpPos)
					{
						if(emPos==GP_Leader||emPos==GP_SecLeader)
						{
							pSrvOpt->set_ret(17);
							eh_->sendEventToGamed(e,nGameID);
							return;
						}
						if(emPos!=GP_Normal&&nTarPos>=emPos)
						{
							pSrvOpt->set_ret(33);
							eh_->sendEventToGamed(e,nGameID);
							return;
						}
						
						if(pGuild->GetPositionCnt((GuildPositionType)nTarPos)>=GuildInfoCfg::Instance().GetPositionMaxCnt(pGuild->GetGuildLV(),(GuildPositionType)nTarPos))
						{
							pSrvOpt->set_ret(17);
							eh_->sendEventToGamed(e,nGameID);
							return;
						}
					}
					else
					{
						if(nTarPos != GP_Normal)
						{
							if(nTarPos<=emPos)
							{
								pSrvOpt->set_ret(33);
								eh_->sendEventToGamed(e,nGameID);
								return;
							}
							if(pGuild->GetPositionCnt((GuildPositionType)nTarPos)>=GuildInfoCfg::Instance().GetPositionMaxCnt(pGuild->GetGuildLV(),(GuildPositionType)nTarPos))
							{
								pSrvOpt->set_ret(17);
								eh_->sendEventToGamed(e,nGameID);
								return;
							}
						}
					}
					
					pTarUser->set_position(nTarPos);
					pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pTarUser->userid(),pRceOpt->tarusername(),nTarPos);

					pRDH->markUserLstDirty(nTID);
					pRDH->markGuildLstDirty(pGuild);

					pSrvOpt->set_ret(0);
					pSrvOpt->set_taruserpos(nTarPos);

					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	

					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_AgreeApply:
				{
					int64 nTID = pSrvOpt->taruserid();
					int nGuildID = pSrvOpt->guildid();
					
					pGuild->CheckNewDayAndClear();
					if(pGuild->GetDBGuild()->dayentercnt()>=GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nDayMaxEnterCnt)
					{
						pSrvOpt->set_ret(39);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}

					DB_C_User* pTarUser  = pRDH->GetDBCityUser(nTID);
					if(pTarUser==NULL)
					{
						pSrvOpt->set_ret(19);
						pGuild->EraseApplyUser(nTID);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pTarUser->guildid()>0||pTarUser->countryid()!=pSrvOpt->country())
					{
						pSrvOpt->set_ret(18);
						pGuild->EraseApplyUser(nTID);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
                    if (pTarUser->tlastleaveguildtime() +GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nChgGuildTime > time(NULL))
                    {
                        pSrvOpt->set_ret(5);
                        pGuild->EraseApplyUser(nTID);
                        eh_->sendEventToGamed(e,nGameID);
                        return;
                    }
					if(pGuild->GetUserCnt()>=GuildInfoCfg::Instance().GetMemberMaxCnt(pGuild->GetGuildLV()))
					{
						pSrvOpt->set_ret(20);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					
					if(!pGuild->IsInApplyLst(nTID))
					{
						pSrvOpt->set_ret(19);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					CCountry* pCountry = pRDH->GetCountry(pTarUser->countryid());
					if(pCountry==NULL)
					{
						pSrvOpt->set_ret(3);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					//玩家加入工会
					pTarUser->set_guildid(nGuildID);
					pTarUser->set_position(GP_Normal);
					pSrvOpt->set_taruserpos(GP_Normal);
					pGuild->AddUser(pTarUser);
					pGuild->EraseApplyUser(nTID);

					//设置玩家官员职位
					DB_C_User* pDBCUserLeader = pGuild->GetMember(pGuild->GetLeaderID());
					if(pDBCUserLeader!=NULL)
					{
						if(pCountry->HasUserOfficerPosition(pDBCUserLeader,Officer_Chairman)||pCountry->HasUserOfficerPosition(pDBCUserLeader,Officer_President))
						{
							if(pCountry->SetUserOfficerPosition(pTarUser,Officer_PresidentGuard))
							{
								eh_->SendCountryUserLiteInfo(pRDH,pTarUser);
								pRDH->markCountryDirty(pCountry);
							}
						}
					}
					pRDH->markUserLstDirty(nTID);
					pGuild->GetDBGuild()->set_dayentercnt(pGuild->GetDBGuild()->dayentercnt()+1);
					pRDH->markGuildLstDirty(pGuild);
					pRseOpt->set_usercnt(pGuild->GetDBGuild()->dayentercnt());
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_DisAgreeApply:
				{
					int64 nTID = pSrvOpt->taruserid();
					int nGuildID = pSrvOpt->guildid();
					CGuild* pGuild = pRDH->GetGuild(nGuildID);
					if(pGuild==NULL)
					{
						pSrvOpt->set_ret(3);
						eh_->sendEventToGamed(e,nGameID);
						return;
					} 
					pGuild->EraseApplyUser(nTID);
					pRDH->markGuildLstDirty(pGuild);
				}
				break;
			case GOT_KickMember:
				{
					int64 nTID = pSrvOpt->taruserid();
					int nGuildID = pSrvOpt->guildid();

					//DB_C_User* pTarUser = pGuild->GetMember(nTID);
					DB_C_User* pTarUser  = pRDH->GetDBCityUser(nTID);
					if(pTarUser==NULL)
					{
						pSrvOpt->set_ret(19);
						pGuild->EraseApplyUser(nTID);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pTarUser->guildid()!=pGuild->GetGuildID())
					{
						pSrvOpt->set_ret(15);
						pGuild->EraseApplyUser(nTID);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pGuild->IsUserInGvgBtl(nTID)){
						//如果已报名夺城战，则不能踢出公会
						pSrvOpt->set_ret(217);
						eh_->sendEventToGamed(e,nGameID);
						return;	
					}
					if(pGuild->IsUserSignUpBtl(nTID))
					{//如果已报名工会战，则不能退出工会
						pSrvOpt->set_ret(57);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					pGuild->CheckNewDayAndClear();
					if(pGuild->GetDBGuild()->daykickusercnt()>=GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nDayMaxKickCnt)
					{
						pSrvOpt->set_ret(38);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pTarUser->position()==GP_Leader||!GuildInfoCfg::Instance().HasPermit((GuildPositionType)pDBCUser->position(),G_Pmt_KickUser))
					{
						pSrvOpt->set_ret(13);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					if(pTarUser->position()!=GP_Normal && pDBCUser->position() >= pTarUser->position())
					{
						pSrvOpt->set_ret(13);
						eh_->sendEventToGamed(e,nGameID);
						return;
					}
					pGuild->EraseUser(nTID);
					pTarUser->set_guildid(0);
					pTarUser->set_position(0);

					//官员身份取消
					if(pTarUser->officerposition()>0)
					{
						pRDH->ClearUserOfficerPosition(pTarUser,E_Officer_Type_Guild);
						eh_->SendCountryUserLiteInfo(pRDH,pTarUser);
					}

					pRDH->markUserLstDirty(nTID);

					pRDH->AddGuildLog(pGuild,G_Log_Kick,nTID,pRceOpt->tarusername());
					pRDH->markGuildLstDirty(pGuild);

					pGuild->GetDBGuild()->set_daykickusercnt(pGuild->GetDBGuild()->daykickusercnt()+1);
					pRDH->markGuildLstDirty(pGuild);

					pRseOpt->set_usercnt(pGuild->GetDBGuild()->daykickusercnt());
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_ApplyList:
				{
					Handle_ApplyList(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_BldUpgrade:
				{
					Handle_GuildBldUpgrade(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_OpenGuildShop:
				{			
					Handle_OpenGuildShop(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_SetGuildShopPmt:
				{
					Handle_SetGuildShopPmt(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlOpenPage:
				{
					Handle_GBtlOpenPage(pDBCUser,pGuild,e,pRDH,nGameID);
				}	
				break;
			case GOT_GBtlGuildLst:
				{
					HandleGBtlGuildLst(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlAllRankTotal:
				{
					HandleGBtlAllRankTotal(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlAllRank:
				{
					HandleGBtlAllRank(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSinUp:
				{
					Handle_GBtlSinUp(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlCreditEmbrave:
			case GOT_GBtlHonorEmbrave:
				{
					DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();
					bool bFind = false;
					for(int i=0;i<pDBGBInfo->addprop_size();i++)
					{
						DB_C_GuildBtlAddProp* pDBProp = pDBGBInfo->mutable_addprop(i);
						if(pDBProp->userid() == nUserID)
						{
							bFind = true;
							pDBProp->set_addatkpct(pSrvOpt->addatkpct());
							pDBProp->set_addbloodpct(pSrvOpt->addbloodpct());
							break;
						}
					}
					if(!bFind)
					{
						DB_C_GuildBtlAddProp* pDBProp = pDBGBInfo->add_addprop();
						pDBProp->set_userid(nUserID);
						pDBProp->set_addatkpct(pSrvOpt->addatkpct());
						pDBProp->set_addbloodpct(pSrvOpt->addbloodpct());
					}

					pRDH->markGuildLstDirty(pGuild);
				}
				break;
			case GOT_GBtlSeeLineup:
				{
					DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();
					const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();
					
					int nCnt = pGBtlTbl->GetWayUserCnt(pGuild->GetGuildLV());
					for(int i=pDBGBInfo->userway0_size();i<nCnt;i++)
					{
						pDBGBInfo->add_userway0(0);
					}
					for(int i=pDBGBInfo->userway1_size();i<nCnt;i++)
					{
						pDBGBInfo->add_userway1(0);
					}
					for(int i=pDBGBInfo->userway2_size();i<nCnt;i++)
					{
						pDBGBInfo->add_userway2(0);
					}

					for(int i=0;i<pDBGBInfo->userway0_size();i++)
					{
						pRseOpt->add_userway0(toString(pDBGBInfo->userway0(i)));
					}
					for(int i=0;i<pDBGBInfo->userway1_size();i++)
					{
						pRseOpt->add_userway1(toString(pDBGBInfo->userway1(i)));
					}
					for(int i=0;i<pDBGBInfo->userway2_size();i++)
					{
						pRseOpt->add_userway2(toString(pDBGBInfo->userway2(i)));
					}
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break; 
			case GOT_GBtlSetLineup:
				{
					Handle_GbtlSetLineup(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSeeLastBtl:
				{
					Handle_GBtlSeeLastBtl(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSeeReward:
			case GOT_GBtlGetReward:
				{
					Handle_GBtlSeeReward(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSeeStorage:
				{
					Handle_GbtlSeeStorage(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSendStorageItem:
				{
					Handle_GBtlSendStorageItem(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GBtlSendItemLog:
				{
					DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
					int nIdx = 0;
					for(int i=0;i<pDBGuild->rewardlog_size();i++)
					{
						DB_C_GuildLog* pDBLog = pDBGuild->mutable_rewardlog(i);
						GuildLog* pLog = pRseOpt->add_log();
						pLog->set_logtype(pDBLog->logtype());
						pLog->set_time(pDBLog->time());
						pLog->set_userid(pDBLog->userid());
						pLog->set_username(pDBLog->username());
						pLog->set_userpos(pDBLog->userpos());
						pLog->set_bldid(pDBLog->bldid());
						pLog->set_bldlv(pDBLog->bldlv());
						pLog->set_paytype(pDBLog->paytype());
						pLog->set_contribute(pDBLog->gscore());
						pLog->set_itemid(pDBLog->itemid());
						pLog->set_itemcnt(pDBLog->itemcnt());
						pLog->set_bldvalue(pDBLog->bldvalue());
						pLog->set_pkrank(pDBLog->pkrank());
					}
					pSrvOpt->set_ret(0);
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			//GVE
			case GOT_GVE_Open:
				{
					Handle_GveOpen(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_NpcList:
				{
					Handle_GveNpcList(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_SpNpcList:
				{
					Handle_GveSpNpcList(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_GetBox:
				{
					Handle_GveGetBox(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_GetGuildRank:
				{
					Handle_GveRankBox(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_UseBuff:
				{
					Handle_GveUseBuff(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_Map:
				{
					Handle_GveMap(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_Rank:
			case GOT_GVE_GuildRank:
			case GOT_GVE_LastRank:
			case GOT_GVE_LastGuildRank:
			case GOT_GVE_HistoryRank:
				{
					Handle_GveRank(pDBCUser,pGuild,e,pRDH,nGameID,nType,pRceOpt->listcountryid());
				}
				break;

			case GOT_GVE_AttackOver:
			case GOT_GVE_AtkSweepOver:
				{
					Handle_GveAttackOver(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_GVE_GetAllGuildRank:
				{
					Handle_GveAllRankBox(pDBCUser,pGuild,e,pRDH,nGameID);
				}
				break;
			case GOT_TASK_AWARD:
				{
					/*int nContributeID = pRceOpt->contributeid();
					const GuildContributeTbl* pTbl = GuildInfoCfg::Instance().GetGuildContributeTbl(nContributeID);
					if(pTbl==NULL)
					{
						return;
					}*/
					int gtaskid = pRseOpt->guildtaskid();
					const GuildTaskTbl* pTbl = GuildInfoCfg::Instance().GetGuildTaskTbl(gtaskid);
					if (pTbl == NULL)
					{
						return;
					}
					pGuild->GetDBGuild()->set_bldvalue(pGuild->GetDBGuild()->bldvalue()+pTbl->m_nBld);
					pGuild->GetDBGuild()->set_totalbldvalue(pGuild->GetDBGuild()->totalbldvalue()+pTbl->m_nBld);

					pRDH->markGuildLstDirty(pGuild);
					pSrvOpt->set_ret(0);
					GuildData* pGData = pSrvOpt->mutable_guild();
					pRDH->FillGuildData(pGuild,pGData);	
					eh_->sendEventToGamed(e,nGameID);
				}
				break;
			case GOT_SET_DELATETM:
				{
					//指令测试,时间直接置1以便check删除
					pGuild->GetDBGuild()->set_delatetm(1);
					pGuild->GetDBGuild()->set_delateid(nUserID);
				}
				break;
			case GOT_SET_LEADER:
				Handle_SetLeader(pDBCUser , pGuild , e , pRDH , nGameID);
				break;
			default: 
				break;
			}
		}
	}
}

void DealGuildEvent::SaveGuildBtlSendData(DB_C_User* pDBCUser,Country_FightSendData* pFightData)
{
	if(pDBCUser==NULL||pFightData==NULL)
		return;
	DB_C_Guild_FightSendData dbFight;
	DB_C_GuildBtlUser* pDBBtlUser = dbFight.mutable_btluser();
	PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
	pDBBtlUser->set_userid(toString(pDBCUser->userid()));
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
	GuildBtlDBHandler::Instance().SafePushGuildBtlUpdate(pDBCUser->userid(),GuildBtlDB_USER,&dbFight);
}


void DealGuildEvent::Handle_GuildPageOpen(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{

	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	int nGuildID = pSrvOpt->guildid();
	if(nGuildID<=0)
	{//无工会
		pRseOpt->set_type(pRceOpt->type());
		GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
		pRDH->FillRandGuildLst(pRseOpt,pSrvOpt,pDBCUser->countryid());
	}
	else
	{//有工会
		CGuild* pGuild = pRDH->GetGuild(nGuildID);
		if(pGuild==NULL)
		{
			pSrvOpt->set_ret(2);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
		if(pDBCUser->userid()==pDBGuild->leaderid())
		{//校验
			if(pDBCUser->position() != GP_Leader)
			{
				pDBCUser->set_position(GP_Leader);
				pRDH->markUserLstDirty(pDBCUser->userid());
			}
		}
		else
		{//校验		
			if(pDBCUser->position() == GP_Leader)
			{
				pDBCUser->set_position(GP_Normal);
				pRDH->markUserLstDirty(pDBCUser->userid());
			}
		}
		if(pDBCUser->position() == GP_Leader)
		{
			if(pSrvOpt->leadername().size()>0)
			{//更新会长名字
				pDBGuild->set_leadername(pSrvOpt->leadername());
			}
			pDBGuild->set_delatetm(0);
			pDBGuild->set_leaderlasttm(time(NULL));
		}
		//校验工会
		for(int i=pDBGuild->build_size();i<GB_Cnt;i++)
		{//重复校验
			DB_C_GuildBld* pBld = pDBGuild->add_build();
			pBld->set_id(i);
			pBld->set_level(0);
			pBld->set_exp(0);
		}
		if(pDBGuild->mutable_build(GB_Hall)->level()<1)
		{
			pDBGuild->mutable_build(GB_Hall)->set_level(1);
		}		
		GuildData* pGData = pSrvOpt->mutable_guild();
		pSrvOpt->set_position(pDBCUser->position());
		pRDH->FillGuildData(pGuild,pGData);	

		if(pDBGuild->delatetm()>0)
		{//有人发起弹劾会长
			CheckDelateLeader(pRDH,pGuild);
		}

		//校验工会战
		time_t ltNow = time(NULL);
		tm tmNow = *localtime(&ltNow);
		int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
		bool bNewDay = pRDH->CheckNewDayGuildPK(nYearDay,pGuild);
		if(bNewDay)
		{
			pRDH->markGuildLstDirty(pGuild);
		}

		//发送GVG城市ID

		DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
		if ( pDBGvgMsg != NULL )
		{
			pSrvOpt->set_gvgcityid( pDBGvgMsg->controlcity() );
		}

		pRseOpt->set_lastallcountryrank( pRDH->GetLastRank(pGuild) );
	}
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::CheckDelateLeader(CountryRgnDatHandler* pRDH,CGuild* pGuild)
{
	if(pGuild==NULL||pRDH==NULL)
		return ;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	time_t ltNow = time(NULL);
	if(pDBGuild->delatetm()<=0)
		return;
	if(ltNow - pDBGuild->delatetm() <= GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nDelateLeftTime)
		return;
	//更换会长
	/*int nMaxPos = -1;
	int64 nTarID = 0;
	map<int64,DB_C_User*>::iterator iter;
	for(iter = pGuild->m_mapGuildUser.begin();iter!=pGuild->m_mapGuildUser.end();iter++)
	{
		DB_C_User* pDBCUser = iter->second;

		int nUserPos = pDBCUser->position();
		if(nUserPos == GP_Leader||pDBCUser->userid()<100000000)
			continue;
		if(pDBCUser->guildid()!=pGuild->GetGuildID())
			continue;
		if(nMaxPos < 0)
		{
			nTarID = pDBCUser->userid();
			nMaxPos = nUserPos;
		}
		else if(nMaxPos == GP_Normal)
		{
			if(nUserPos!=GP_Normal)
			{
				nTarID = pDBCUser->userid();
				nMaxPos = nUserPos;
			}
		}
		else 
		{
			if(nUserPos != GP_Normal&&nMaxPos>nUserPos)
			{
				nTarID = pDBCUser->userid();
				nMaxPos = nUserPos;
			}
		}
	}*/
	//去目标玩家的game取名字
	CountryDataHandler* pDH= eh_->getDataHandler();
	Event evn;
	evn.set_cmd(C2S_RceGuildOpt);
	//evn.set_uid(nTarID);
	evn.set_uid(pDBGuild->leaderid());//去会长的gamed,从lite遍历所有会员,取一周内贡献值最高的
	evn.set_time(-1);
	evn.set_state(Status_Country_To_Game);
	RceGuildOpt* pRceOpt = evn.mutable_ce_rceguildopt();
	pRceOpt->set_type(GOT_ChgLeaderByDelate);
	GCG_GuildOpt* pOpt = evn.mutable_guildopt();
	pOpt->set_guildid(pGuild->GetGuildID());
	pOpt->set_country(pGuild->GetCountry());
	pOpt->set_taruserid(pDBGuild->leaderid());//gamed remateload use
	//传所有成员id以遍历
	map<int64,DB_C_User*>::iterator iter;
	for(iter = pGuild->m_mapGuildUser.begin();iter!=pGuild->m_mapGuildUser.end();iter++)
	{
		DB_C_User* pDBCUser = iter->second;
		if (pDBCUser->userid()<100000000)
		{
			continue;
		}
		pOpt->add_memberid(pDBCUser->userid());
	}

	int gid = pDH->getGamedIdByUserId(evn.uid());

	eh_->sendEventToGamed(&evn,gid);
}

void DealGuildEvent::Handle_SrvCreateGuild(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	string strName = pRceOpt->guildname();
	if(pDBCUser->guildid()>0)
	{//已有工会,前面有判断，不会走到这里
		SYS_LOG((int64)pDBCUser->userid(),LT_Guild_Create,pRDH->GetRegion(),0,3<<0<<0<<0<<0);
		pSrvOpt->set_ret(4);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(!pRDH->CanGuildNameUse(strName))
	{//再判断一次名字
		SYS_LOG((int64)pDBCUser->userid(),LT_Guild_Create,pRDH->GetRegion(),0,1<<0<<0<<0<<0);
		pSrvOpt->set_ret(8);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	CGuild* pNewGuild = pRDH->AllocEmptyGuild(pDBCUser->countryid());
	if(pNewGuild==NULL)
	{//创建失败
		SYS_LOG((int64)pDBCUser->userid(),LT_Guild_Create,pRDH->GetRegion(),0,2<<0<<0<<0<<0);
		pSrvOpt->set_ret(9);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	pSrvOpt->set_ret(0);
	DB_C_Guild* pDBGuild = pNewGuild->GetDBGuild();
	pDBGuild->set_guildname(strName);
	pDBGuild->set_guildbulletin(pRceOpt->guildbulletin());
	pDBGuild->set_guildslogan(pRceOpt->guildslogan());
	pDBGuild->set_leaderid(pDBCUser->userid());
	pDBGuild->set_countryid(pDBCUser->countryid());
	pDBGuild->set_leaderlasttm(time(NULL));
	pDBGuild->set_leadername(pSrvOpt->leadername());
	pDBGuild->set_delatetm(0);
	pNewGuild->SetGuildName(strName.c_str());
	pRDH->AddGuildName(strName);
	

	pDBCUser->set_position(GP_Leader);
	pDBCUser->set_guildid(pDBGuild->guildid());
	pNewGuild->AddUser(pDBCUser);

	pRDH->markGuildLstDirty(pNewGuild);
	pRDH->markUserLstDirty(pDBCUser->userid());

	int64 nUserID = pDBCUser->userid();
	SYS_LOG(nUserID,LT_Guild_Create,pRDH->GetRegion(),0,0<<pNewGuild->GetGuildID()<<pNewGuild->GetCountry()<<pRDH->GetGuildCnt()<<pRDH->GetGuildCnt(pNewGuild->GetCountry()));

	pRDH->SortGuild();

	//填充返回值
	pSrvOpt->set_guildid(pNewGuild->GetGuildID());
	pSrvOpt->set_position(GP_Leader);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pNewGuild,pGData);	

	eh_->sendEventToGamed(e,nGameID);

}

void DealGuildEvent::Handle_OpenMemberPage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	map<int64,DB_C_User*>::iterator iter,iterTmp;
	iter = pGuild->m_mapGuildUser.begin();
	while(iter != pGuild->m_mapGuildUser.end())
	{
		DB_C_User* pMember = iter->second;
		if(pMember==NULL||pMember->countryid()!=pDBCUser->countryid()||pMember->guildid()!=pGuild->GetGuildID())
		{
			iterTmp = iter;
			iter++;
			pGuild->m_mapGuildUser.erase(iterTmp);
			continue;
		}
		pSrvOpt->add_memberid(pMember->userid());
		iter++;
	}	

	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::HandleChgLdeaderByDelate(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
//此函数为国家服务器发起
	//现在的pdbcuser和ngameid都是会长的,重新处理
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nGuildID = pSrvOpt->guildid();
	CGuild* pGuild = pRDH->GetGuild(nGuildID);
	if(pGuild==NULL)
	{
		return;
	}
	if(pSrvOpt->ret()!=0)
	{//弹劾会长失败，下次有玩家登陆时继续弹劾
		pGuild->EraseUser(pDBCUser->userid());
		return;
	}
	
	//将pdbcuser和ngameid重新赋值为传过来的新会长的
	int64 nTarUid = pSrvOpt->taruserid();
	if (nTarUid <= 0)
	{
		nTarUid = pGuild->GetDBGuild()->delateid();
	}
	pDBCUser = pRDH->GetDBCityUser(nTarUid);
	if (NULL == pDBCUser)
	{
		return;
	}
	nGameID = pDH->getGamedIdByUserId(nTarUid);
	
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	int64 nOldID = pDBGuild->leaderid();
	string strOldName = pGuild->GetLeaderName();

	pDBGuild->set_leaderid(pDBCUser->userid());
	//pDBGuild->set_leadername(pSrvOpt->leadername());
	if (pSrvOpt->has_leadername())
	{
		pDBGuild->set_leadername(pSrvOpt->leadername());
	}
	else
	{
		pDBGuild->set_leadername(pDBCUser->name());
	}
	//pDBGuild->clear_leadername();
	//for(int i=0;i<pSrvOpt->name_size();i++)
	//{
	//	pDBGuild->add_leadername(pSrvOpt->name(i));
	//}
	pDBGuild->set_delatetm(0);
	pDBGuild->set_leaderlasttm(time(NULL));

	pDBGuild->clear_delateid();

	pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pDBCUser->userid(),pGuild->GetLeaderName(),GP_Leader);
	
	pDBCUser->set_position(GP_Leader);


	DB_C_User* pOldLeader = pRDH->GetDBCityUser(nOldID);
	if(pOldLeader&&nOldID!=pDBCUser->userid())
	{
		//转让官员身份
		CCountry*pCountry = pRDH->GetCountry(pOldLeader->countryid());
		if(pCountry&&pDBCUser->officerposition()>0)
		{
			if(pCountry->ChangeOfficerPosition(pOldLeader,pDBCUser,E_Officer_Type_Guild))
			{
				pRDH->markCountryDirty(pCountry);
				eh_->SendCountryUserLiteInfo(pRDH,pOldLeader);
				eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);
			}
		}

		pOldLeader->set_position(GP_Normal);
		pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,nOldID,strOldName,GP_Normal);
		pRDH->markUserLstDirty(nOldID);
	}

	pRDH->markUserLstDirty(pDBCUser->userid());
	pRDH->markGuildLstDirty(pGuild);

	//告知原会长和新会长
	//去目标玩家的game取名字
	Event evn;
	evn.set_cmd(C2S_RceGuildOpt);
	evn.set_uid(pDBCUser->userid());
	evn.set_time(-1);
	evn.set_state(Status_Country_To_Game);
	RceGuildOpt* pRceOpt1 = evn.mutable_ce_rceguildopt(); 
	pRceOpt1->set_type(GOT_ChgLeaderByDelate2Game);
	GCG_GuildOpt* pOpt1 = evn.mutable_guildopt();
	pOpt1->set_guildid(pGuild->GetGuildID());
	pOpt1->set_country(pGuild->GetCountry());
	pOpt1->set_taruserid(pDBCUser->userid());

	GuildData* pGData = pOpt1->mutable_guild();
	pOpt1->set_position(pDBCUser->position());
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(&evn,nGameID);

	if(pOldLeader) 
	{
		evn.set_uid(nOldID);
		pOpt1->set_position(pOldLeader->position());
		pOpt1->set_taruserid(nOldID);
		int gid = pDH->getGamedIdByUserId(evn.uid());
		eh_->sendEventToGamed(&evn,gid);
	}
	SYS_LOG(pDBCUser->userid() , LT_GUILD_CHG_LEADER,0,0,nTarUid<<nOldID);
}

void DealGuildEvent::Handle_ChgLeader(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	//此处判断一次是否有新会长的数据，如果没有，创建
	int64 nTarID = pSrvOpt->taruserid();
	DB_C_User* pDBTarUser = pRDH->GetDBCityUser(nTarID);
	if(pDBTarUser==NULL)
	{//出错！！！！
		pDBTarUser = pRDH->AllocDBUser(nTarID);
		pDBTarUser->set_countryid(pSrvOpt->country());
		pDBTarUser->set_cityid(CountryInfoCfg::Instance().GetCapital(pSrvOpt->country()));
		pDBTarUser->set_guildid(pSrvOpt->guildid());
		pRDH->markUserLstDirty(nTarID);
		SYS_LOG(nTarID,LT_CNTR_ERROR,0,0,CntrError_UserNotFind4ChgGuildLeader<<0<<pSrvOpt->country()<<pSrvOpt->country());
	}
	if(pDBTarUser->guildid()!=pDBCUser->guildid()||pDBTarUser->countryid()!=pDBCUser->countryid())
	{//数据错误	
		pSrvOpt->set_ret(15);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	pDBGuild->set_leaderid(pSrvOpt->taruserid());
	pDBGuild->set_leadername(pSrvOpt->leadername());
	pDBGuild->set_chgleadertm(time(NULL));

	pDBTarUser->set_position(GP_Leader);
	pDBCUser->set_position(GP_Normal);

	pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pDBCUser->userid(),pSrvOpt->selfname(),GP_Normal);
	pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pDBTarUser->userid(),pGuild->GetLeaderName(),GP_Leader);

	//转让官员身份
	CCountry*pCountry = pRDH->GetCountry(pDBCUser->countryid());
	if(pCountry&&pDBCUser->officerposition()>0)
	{
		if(pCountry->ChangeOfficerPosition(pDBCUser,pDBTarUser,E_Officer_Type_Guild))
		{
			pRDH->markCountryDirty(pCountry);
			eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);
			eh_->SendCountryUserLiteInfo(pRDH,pDBTarUser);
		}
	}

	pRDH->markUserLstDirty(nTarID);
	pRDH->markUserLstDirty(pDBCUser->userid());
	pRDH->markGuildLstDirty(pGuild);

	int64 nFromID	= pDBCUser->userid();
	int64 nToID		= nTarID;

	SYS_LOG(nFromID,LT_Guild_CheLeader,pRDH->GetRegion(),0,pGuild->GetCountry()<<pGuild->GetGuildID()<<nTarID);
	pSrvOpt->set_ret(0);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);
} 

void DealGuildEvent::Handle_ApplyGuild(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nGuildID = pRceOpt->guildid();
	CGuild* pGuild = pRDH->GetGuild(nGuildID);
	if(pGuild==NULL)
	{
		pSrvOpt->set_ret(2);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	bool bSuc = false;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	if(pDBGuild->applyuserid_size()<GuildInfoCfg::Instance().GetGuildBaseTbl()->m_nMaxAppUserCnt)
	{
		bSuc = true;
		pDBGuild->add_applyuserid(pDBCUser->userid());
	}
	else
	{
		for(int i=0;i<pDBGuild->applyuserid_size();i++)
		{
			if(pDBGuild->applyuserid(i)==0)
			{
				bSuc = true;
				pDBGuild->set_applyuserid(i,pDBCUser->userid());
				break;
			}
		}
	}
	if(!bSuc)
	{
		pSrvOpt->set_ret(11);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	else
	{
		pRDH->markGuildLstDirty(pGuild);
		pSrvOpt->set_targuildid(nGuildID);
		pSrvOpt->set_ret(0);
	}
	eh_->sendEventToGamed(e,nGameID);

}

void DealGuildEvent::Handle_ApplyList(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nGuildID = pRceOpt->guildid();
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	bool bNeedSave = false;
	for(int i=0;i<pDBGuild->applyuserid_size();i++)
	{
		int64 nID = pDBGuild->applyuserid(i);
		if(nID<=0)
			continue;
		DB_C_User* pMember = pRDH->GetDBCityUser(nID);
		if(pMember==NULL||pMember->countryid()!=pDBCUser->countryid()||pMember->guildid()>0)
		{
			bNeedSave = true;
			pDBGuild->set_applyuserid(i,0);
			continue;
		}
		pSrvOpt->add_memberid(pMember->userid());
	}	
	if(bNeedSave)
	{
		pRDH->markGuildLstDirty(pGuild);
	}
	eh_->sendEventToGamed(e,nGameID);

}
void DealGuildEvent::Handle_GuildBldUpgrade(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nGuildID = pRceOpt->guildid();
	
	if(pDBCUser->position()!=GP_Leader&&pDBCUser->position()!=GP_SecLeader)
	{
		pSrvOpt->set_ret(14);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	int nBldID = pRceOpt->guildbuidlid();
	int nTarLV = pRceOpt->guildbuildtarlv();
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();

	const GuildBldTbl* pGBTbl = GuildInfoCfg::Instance().GetGuildBuildingTbl(nBldID);
	if(pGBTbl==NULL||nTarLV<=0||nTarLV>pGBTbl->GetMaxLV()||pDBGuild->build_size()<GB_Cnt)
		return;//game端已经判断

	if(pDBGuild->mutable_build(GB_Hall)->level()<1)
	{
		pDBGuild->mutable_build(GB_Hall)->set_level(1);
	}

	DB_C_GuildBld* pDBBld = pDBGuild->mutable_build(nBldID);
	if(pDBBld->level()+1!=nTarLV)
	{
		pSrvOpt->set_ret(25);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	//判断前置建筑
	if(pGBTbl->m_lstPreBldID.size()>pDBBld->level())
	{
		int nPreBldID = pGBTbl->m_lstPreBldID[pDBBld->level()];
		if(nPreBldID>=GB_Hall&&nPreBldID<GB_Cnt)
		{
			int nPreLV = pDBGuild->mutable_build(nPreBldID)->level();
			if(nPreLV < pGBTbl->m_lstPreBldLV[pDBBld->level()])
			{
				pSrvOpt->set_ret(25);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
		}
	}
	//int nNeedValue = pGBTbl->m_lstCostValue[pDBBld->level()];
	//nNeedValue -= pDBBld->exp();
	int nNeedValue = pGBTbl->m_lstCostValue[nTarLV] - pGBTbl->m_lstCostValue[pDBBld->level()];
	int nNeedValue1 = pGBTbl->m_lstCostValue[nTarLV] - pDBBld->exp();
	nNeedValue = max(nNeedValue,nNeedValue1);

	if(nNeedValue<0)
	{
		pSrvOpt->set_ret(1);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(pDBGuild->bldvalue()<nNeedValue)
	{
		pSrvOpt->set_ret(26);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	//建筑升级
	pDBGuild->set_bldvalue(pDBGuild->bldvalue() - nNeedValue);
	pDBBld->set_exp(pDBBld->exp()+nNeedValue);
	pDBBld->set_level(pDBBld->level()+1);
	pRDH->AddGuildLog_Bld(pGuild,G_Log_BldUpgrade,pDBCUser->userid(),pSrvOpt->selfname(),pDBBld->id(),pDBBld->level());
	pRDH->markGuildLstDirty(pGuild);

	if(pGuild->GetGuildLV()!=pDBGuild->mutable_build(GB_Hall)->level())
	{
		int nOldLV = pGuild->GetGuildLV();
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

	SYS_LOG(int64(pDBCUser->userid()),LT_Guild_BldUpgrade,pRDH->GetRegion(),0,nGuildID<<nBldID<<pDBBld->level()<<pDBBld->exp()<<nNeedValue<<pDBGuild->bldvalue());
	//填充返回值
	pSrvOpt->set_ret(0);
	pSrvOpt->set_guildid(pGuild->GetGuildID());
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	

	eh_->sendEventToGamed(e,nGameID);

	//建筑升级，发送给所有工会成员
	pRceOpt->set_type(GOT_SendBldUpgrade);
	map<int64,DB_C_User*>::iterator iter;
	for(iter = pGuild->m_mapGuildUser.begin();iter!=pGuild->m_mapGuildUser.end();iter++)
	{
		DB_C_User* pMember = iter->second;
		if(pMember==NULL||pMember->countryid()!=pDBCUser->countryid()||pMember->guildid()!=pGuild->GetGuildID())
			continue;
		if(pMember->userid()==pDBCUser->userid())
			continue;
		int nGameID = eh_->getGamedIdByUserId(pMember->userid());
		pSrvOpt->set_taruserid(pMember->userid());
		eh_->sendEventToGamed(e,nGameID);
	}		


}

void DealGuildEvent::Handle_SetGuildShopPmt(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	int nGuildID = pRceOpt->guildid();
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildShop* pDBShop = pDBGuild->mutable_guildshop();

	int nShopID		= pRceOpt->shopid();
	int nPosition	= pRceOpt->tarposition();

	const ShopItemTbl* pShopTbl = StoreItemCfg::Instance().GetShopItemTbl(nShopID);
	if(pShopTbl==NULL)
	{
		return;
	}
	if(pDBCUser->position() != GP_Leader)
	{
		pSrvOpt->set_ret(13);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	bool bFind = false;
	for(int i=0;i<pDBShop->shoppmt_size();i++)
	{
		DB_C_GuildShopPmt* pDBPmt = pDBShop->mutable_shoppmt(i);
		if(pDBPmt->shopid() == nShopID)
		{
			bFind = true;
			pDBPmt->set_position(nPosition);
			break;
		}
	}
	if(!bFind)
	{
		DB_C_GuildShopPmt* pDBPmt = pDBShop->add_shoppmt();
		pDBPmt->set_shopid(nShopID);
		pDBPmt->set_position(nPosition);
	}
	pRDH->markGuildLstDirty(pGuild);

	pSrvOpt->set_ret(0);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);

}

void DealGuildEvent::Handle_OpenGuildShop(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	int nGuildID = pRceOpt->guildid();
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildShop* pDBShop = pDBGuild->mutable_guildshop();
	time_t ltNow = time(NULL);
	bool bUpdate = false;
	const GuildMysticShopTbl* pMshopTbl = GuildInfoCfg::Instance().GetGuildMysticShopTbl();
	if(ltNow<pMshopTbl->m_nStartDay || ltNow > pMshopTbl->m_nEndDay)
	{
		pDBShop->clear_mysticshopupdatetm();
		pDBShop->clear_mysticshopsaleid();
		pRDH->markGuildLstDirty(pGuild);
		bUpdate = false;
	}
	else if(pDBShop->mysticshopupdatetm()<=0)
	{
		bUpdate = true;
	}
	else
	{
		time_t ltLast = pDBShop->mysticshopupdatetm();
		tm tmLast = *localtime(&ltLast);
		tm tmNow  = *localtime(&ltNow);
		int nLast = tmLast.tm_sec + tmLast.tm_min * 30 + tmLast.tm_hour*3600;
		int nNow = tmNow.tm_sec + tmNow.tm_min * 30 + tmNow.tm_hour*3600;
		for(int i=1;i<pMshopTbl->m_lstDayUpdateTm.size();i++)
		{
			if(nLast>=pMshopTbl->m_lstDayUpdateTm[i-1]&&nLast<pMshopTbl->m_lstDayUpdateTm[i])
			{
				if(nNow>=pMshopTbl->m_lstDayUpdateTm[i-1]&&nNow<pMshopTbl->m_lstDayUpdateTm[i])
				{//在同一个时间段
					bUpdate = false;
				}
				else
				{
					bUpdate = true;
				}
				break;
			}
		}
	}

	if(bUpdate)
	{
		pDBShop->set_mysticshopupdatetm(ltNow);
		pDBShop->clear_mysticshopsaleid();
		vector<int> lstID;
		if(pDBGuild->build_size()<GB_Cnt)
		{
			pSrvOpt->set_ret(24);
			eh_->sendEventToGamed(e,nGameID);
			return;

		}
		DB_C_GuildBld* pDBBld = pDBGuild->mutable_build(GB_Shop);
		GuildInfoCfg::Instance().GetMysticsShopSaleID(pDBBld->level(),lstID);
		for(int i=0;i<lstID.size();i++)
		{
			pDBShop->add_mysticshopsaleid(lstID[i]);
		}
	}
	bool bNeedSave = pRDH->CheckGuildShopRecord(pGuild);
	if(bUpdate||bNeedSave)
	{
		pRDH->markGuildLstDirty(pGuild);

	}

	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	GuildShop* pGuildShop = pRseOpt->mutable_shop();
	for(int i=0;i<pDBShop->mysticshopsaleid_size();i++)
	{
		pGuildShop->add_mysticshopsaleid(pDBShop->mysticshopsaleid(i));
	}
	for(int i=0;i<pDBShop->buyrecord_size();i++)
	{
		DB_C_GuildBuyRecord* pDBRecord = pDBShop->mutable_buyrecord(i);
		pGuildShop->add_shopid(pDBRecord->daybuyshopid());
		pGuildShop->add_salecnt(pDBRecord->daybuyshopcnt());
	}
	for(int i=0;i<pDBShop->shoppmt_size();i++)
	{
		DB_C_GuildShopPmt* pDBPmt = pDBShop->mutable_shoppmt(i);
		GuildShopPmt* pPmt = pGuildShop->add_shoppmt();
		pPmt->set_shopid(pDBPmt->shopid());
		pPmt->set_position(pDBPmt->position());
	}	
	pSrvOpt->set_ret(0);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);
	return;
}

void DealGuildEvent::HandleGBtlAllRankTotal(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	CCountry* pCountry = pRDH->GetEarthCountry();
	if(pCountry==NULL)
	{
		pSrvOpt->set_ret(53);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	for(int i=0;i<pDBCountry->gbtlrecord_size();i++)
	{
		DB_C_GuildBtl_Record* pDBRecored = pDBCountry->mutable_gbtlrecord(i);
		if(pDBRecored->guilddata_size()>0)
		{
			DB_C_GuildBtl_Top_Guild* pDBGData = pDBRecored->mutable_guilddata(0);
			GuildData* pGData = pRseOpt->add_guildlst();
			pGData->set_session(pDBRecored->seasons());
			pGData->set_guildid(pDBGData->guildid());
			pGData->set_countryid(pDBGData->countryid());
			pGData->set_guildlv(pDBGData->guildlv());
			pGData->set_guildname(pDBGData->guildname());
			pGData->set_leadername(pDBGData->leadername());
			pGData->set_guildbtlscore(pDBGData->btlscore());
			pGData->set_leadername(pDBGData->leadername());
		}
		
	}
	
	int nSession=0;
	int nStep = 0;
	time_t ltNow = time(NULL);
	GuildInfoCfg::Instance().GetGuildPKSession(ltNow,nSession,nStep);
	DB_C_GuildBtl* pDBGBtl = pRDH->GetDBGuildBtl(pGuild);
	if(pDBGBtl)
	{
		if(pDBGBtl->lastbtlsession()+1 == nSession||pDBGBtl->lastbtlsession() == nSession)
		{
			pRseOpt->set_lastbtlrank(pDBGBtl->lastbtlrank());
		}
	}
	 


	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::HandleGBtlAllRank(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	int nSession = pRceOpt->session();
	if(nSession<=0)
		nSession = 1;
	int nCountry = pRceOpt->tarcountry();
	if(nCountry<C_Start||nCountry>=C_UserCoutryEnd)
		nCountry=C_AllCountry; 
	CCountry* pCountry = pRDH->GetCountry(nCountry);
	if(pCountry==NULL)
	{
		pSrvOpt->set_ret(53);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	DB_C_GuildBtl_Record* pDBRecord = pRDH->GetGuildBtlRecord(pCountry,nSession);
	if(pDBRecord==NULL)
	{
		pSrvOpt->set_ret(53);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);
	for(int i=0;i<pDBRecord->guilddata_size();i++)
	{
		DB_C_GuildBtl_Top_Guild* pDBGData = pDBRecord->mutable_guilddata(i);
		GuildData* pGData = pRseOpt->add_guildlst();
		pGData->set_guildid(pDBGData->guildid());
		pGData->set_countryid(pDBGData->countryid());
		pGData->set_guildlv(pDBGData->guildlv());
		pGData->set_guildname(pDBGData->guildname());
		pGData->set_guildpkrank(i+1);
		pGData->set_totalpkrank(i+1);
		pGData->set_leadername(pDBGData->leadername());
		pGData->set_guildbtlscore(pDBGData->btlscore());
		pGData->set_leadername(pDBGData->leadername());
		pGData->set_guilduesrcnt(pDBGData->usercnt());
	}

	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::HandleGBtlGuildLst(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);
	pRDH->FillPKGuildLst(pRseOpt,pRceOpt->tarcountry(),pRceOpt->page());
	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::Handle_GBtlOpenPage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//打开工会战的第一个包
	
	//检验是否为新一期
	pRDH->CheckGuildBtlData();
	//pRDH->SortPKGuild();  

	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);
	pRDH->FillPKGuildLst(pRseOpt,pRceOpt->tarcountry(),pRceOpt->page());
	
	int nPKGuildID = pGuild->GetPKGuildID(pRDH->GetGuildPKTurns());
	CGuild* pPKGuild = pRDH->GetGuild(nPKGuildID);
	if(pPKGuild)
	{
		GuildData* pEData = pRseOpt->mutable_pkguild();
		pEData->set_guildid(pPKGuild->GetGuildID());
		pEData->set_guildlv(pPKGuild->GetGuildLV());
		pEData->set_guildname(pPKGuild->GetGuildName());
		pEData->set_totalpkrank(pPKGuild->m_nAllGuildPKRank);
	}
	const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();

	DB_C_GuildBtl* pDBGBtl = pRDH->GetDBGuildBtl(pGuild);
	pRseOpt->set_dayhaspk(pGuild->HasPKToday());
	
	if(pRDH->GetGuildBtlStep()==GBtlType_Over)
	{
		CCountry* pEarth = pRDH->GetEarthCountry();
		DB_C_Country* pDBCountry = pEarth->GetDBCountry();
		DB_C_PubGuildBtlInfo* pDBGPInfo = pDBCountry->mutable_pubguildbtlinfo();
		pRseOpt->set_cursesionbtlover(pDBGPInfo->cursesionbtlover());
	}

	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	

	eh_->sendEventToGamed(e,nGameID);

//#ss#测试代码
//#ifdef WIN32
//	const GuildRewardTbl* pRewardTbl = GuildInfoCfg::Instance().GetGuildBtlRedwardTbl(1);
//	if(pRewardTbl)
//	{
//		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
//		bool bFind = false;
//		for(int j=0;j<pDBGuild->storage_size();j++)
//		{
//			DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(j);
//			if(pDBStorage->itemid() == pRewardTbl->m_nItemID)
//			{
//				pDBStorage->set_itemcnt(pDBStorage->itemcnt()+pRewardTbl->m_nItemCnt);
//				if(pDBStorage->itemcnt()<0)
//				{
//					pDBStorage->set_itemcnt(0);
//				}
//				bFind = true;
//				break;
//			}
//		}
//		if(!bFind)
//		{
//			DB_C_GuildStorage* pDBStorage = pDBGuild->add_storage();
//			pDBStorage->set_itemid(pRewardTbl->m_nItemID);
//			pDBStorage->set_itemcnt(pRewardTbl->m_nItemCnt);
//		}
//	}
//#endif
}

void DealGuildEvent::Handle_GBtlSinUp(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//工会战报名，此处不判断报名时间
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	time_t ltNow = time(NULL);
	tm tmNow = *localtime(&ltNow);
	int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
	bool bNewDay = pRDH->CheckNewDayGuildPK(nYearDay,pGuild);
	
	DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();
	//此处加一个校验
	const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();
	int nCnt = pGBtlTbl->GetWayUserCnt(pGuild->GetGuildLV());
	for(int i=pDBGBInfo->userway0_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway0(0);
	}
	for(int i=pDBGBInfo->userway1_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway1(0);
	}
	for(int i=pDBGBInfo->userway2_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway2(0);
	}


	int nIdx = 0;
	int64 nUserID = pDBCUser->userid();
	bool bFind=false;
	int nWayCnt0=0,nWayCnt1=0,nWayCnt2=0;

	for(int i=0;i<pDBGBInfo->userway0_size();i++)
	{
		int64 nID = pDBGBInfo->userway0(i);
		if(nID<=0)
			continue;
		nWayCnt0++;
		if(nID==nUserID)
		{
			bFind = true;
			break;
		}
	}
	for(int i=0;i<pDBGBInfo->userway1_size();i++)
	{
		int64 nID = pDBGBInfo->userway1(i);
		if(nID<=0)
			continue;
		nWayCnt1++;
		if(nID==nUserID)
		{
			bFind = true;
			break;
		}
	}
	for(int i=0;i<pDBGBInfo->userway2_size();i++)
	{
		int64 nID = pDBGBInfo->userway2(i);
		if(nID<=0)
			continue;
		nWayCnt2++;
		if(nID==nUserID)
		{
			bFind = true;
			break;
		}
	}
	if(bFind)
	{//已经报过名了
		pSrvOpt->set_ret(43);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	else
	{
		//#ss#临时代码，方便测试
//#ifdef WIN32
//		for(int j=0;j<30;j++)
//#endif
		{
			int nSignUpWay = -1;
			if(nWayCnt2<nWayCnt0&&nWayCnt2<nWayCnt1)
			{//放在第三路
				for(int i=0;i<pDBGBInfo->userway2_size();i++)
				{
					if(pDBGBInfo->userway2(i)==0)
					{
						pDBGBInfo->set_userway2(i,nUserID);
						nWayCnt2++;
						nSignUpWay = 2;
						break;
					}
				}
			}
			else if(nWayCnt1<nWayCnt0)
			{//放在第二路
				for(int i=0;i<pDBGBInfo->userway1_size();i++)
				{
					if(pDBGBInfo->userway1(i)==0)
					{
						pDBGBInfo->set_userway1(i,nUserID);
						nWayCnt1++;
						nSignUpWay = 1;
						break;
					}
				}
			}
			else
			{//放在第一路
				for(int i=0;i<pDBGBInfo->userway0_size();i++)
				{
					if(pDBGBInfo->userway0(i)==0)
					{
						pDBGBInfo->set_userway0(i,nUserID);
						nWayCnt0++;
						nSignUpWay = 0;
						break;
					}
				}
			}

			pDBGBInfo->set_signupcnt(pDBGBInfo->signupcnt()+1);

			SYS_LOG(nUserID,LT_GBtl_SignUp,pRDH->GetRegion(),0,pGuild->GetGuildID()<<nSignUpWay
				<<(int)pDBGBInfo->userway0_size()<<(int)pDBGBInfo->userway1_size()<<(int)pDBGBInfo->userway2_size()
				<<nWayCnt0<<nWayCnt1<<nWayCnt2
				<<pDBGBInfo->signupcnt()<<pDBGBInfo->btlday());
		}
		
	}
	pRDH->markGuildLstDirty(pGuild);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);

	//#ss#测试数据
	//if(pRDH->GetRegion()==2)
	//{
	//	if(pRDH->GetGuildCnt()<1000)
	//	{
	//		for(int i=0;i<1000;i++)
	//		{
	//			CGuild* pTestGuild = pRDH->AllocEmptyGuild(rand()%6+C_Start);
	//			DB_C_Guild* pDBTmpGuild = pTestGuild->GetDBGuild();
	//			pDBTmpGuild->set_guildlv(2);
	//			pDBTmpGuild->mutable_build(GB_Hall)->set_level(2);
	//			char szTmp[256];
	//			sprintf(szTmp,"TestGuild_%d",i);
	//			pDBTmpGuild->set_guildname(szTmp);
	//			pDBTmpGuild->set_leaderid(123);
	//			pDBTmpGuild->set_leadername(szTmp);

	//			DB_C_GuildBtl* pTmpDBGBInfo = pDBTmpGuild->mutable_btlinfo();
	//			pTmpDBGBInfo->CopyFrom(*pDBGBInfo);

	//			pRDH->AddPKGuild(pTestGuild);
	//		}
	//	}
	//}

	//将玩家战斗数据放入存储线程
	Country_FightSendData* pFightData = e->mutable_guildfightsenddata();
	SaveGuildBtlSendData(pDBCUser,pFightData);
}

void DealGuildEvent::Handle_GBtlSeeReward(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	GuildBtlStepType emType = pRDH->GetGuildBtlStep();
	if(emType == GBtlType_SignUp || emType == GBtlType_Over)
	{
		RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
		GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
		CountryDataHandler* pDH= eh_->getDataHandler();
		RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
		GuildBtlRetTotal* pRseTotal = pRseOpt->mutable_rettotal();
		if(!pRDH->FillGuildBtlRetTotal(pDBCUser->userid(),pGuild,pRseTotal))
		{
            pSrvOpt->set_ret(52);
            eh_->sendEventToGamed(e,nGameID);
			return;
		}

		GuildData* pGData = pSrvOpt->mutable_guild();
		pRDH->FillGuildData(pGuild,pGData);	
		eh_->sendEventToGamed(e,nGameID);
	}
}

void DealGuildEvent::Handle_GbtlSeeStorage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	map<int64,DB_C_User*>::iterator iter,iterTmp;
	iter = pGuild->m_mapGuildUser.begin();
	while(iter != pGuild->m_mapGuildUser.end())
	{
		DB_C_User* pMember = iter->second;
		if(pMember==NULL||pMember->countryid()!=pDBCUser->countryid()||pMember->guildid()!=pGuild->GetGuildID())
		{
			iterTmp = iter;
			iter++;
			pGuild->m_mapGuildUser.erase(iterTmp);
			continue;
		}
		pSrvOpt->add_memberid(pMember->userid());
		iter++;
	}	
	GuildStorage* pStorage = pRseOpt->mutable_storage();
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	for(int i=0;i<pDBGuild->storage_size();i++)
	{
		DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(i);
		if(pDBStorage->itemid()<=0)
			continue;
		pStorage->add_itemid(pDBStorage->itemid());
		pStorage->add_itemcnt(pDBStorage->itemcnt());
	}
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::Handle_GBtlSendStorageItem(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	
	if(!GuildInfoCfg::Instance().HasPermit(GuildPositionType(pDBCUser->position()),G_Pmt_SendReward))
	{
		pSrvOpt->set_ret(13);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	DB_C_User* pDBTUser = pRDH->GetDBCityUser(pSrvOpt->taruserid());
	if(pDBTUser==NULL||pDBTUser->guildid()!=pGuild->GetGuildID())
	{
		pSrvOpt->set_ret(16);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	
	//查看物品数量是否符合
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();

	bool bSuc = true;
	bool bFind = false;
	GuildStorage* pSendItem = pRceOpt->mutable_senditem();
	for(int i=0;i<pSendItem->itemid_size()&&i<pSendItem->itemcnt_size();i++)
	{
		bFind = false;
		int nID = pSendItem->itemid(i);
		int nCnt = pSendItem->itemcnt(i);
		for(int j=0;j<pDBGuild->storage_size();j++)
		{
			DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(j);
			if(pDBStorage->itemid()==nID)
			{
				bFind = true;
				if(pDBStorage->itemcnt()<nCnt)
				{
					bSuc = false;
				}
				break;
			}
		}
		if(!bFind||!bSuc)
			break;
	}
	if(!bFind||!bSuc)
	{//道具数量不足
		pSrvOpt->set_ret(58);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	//扣除物品
	for(int i=0;i<pSendItem->itemid_size()&&i<pSendItem->itemcnt_size();i++)
	{
		int nID = pSendItem->itemid(i);
		int nCnt = pSendItem->itemcnt(i);
		for(int j=0;j<pDBGuild->storage_size();j++)
		{
			DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(j);
			if(pDBStorage->itemid()==nID)
			{
				pDBStorage->set_itemcnt(pDBStorage->itemcnt()-nCnt);
				if(pDBStorage->itemcnt()<=0)
				{//删除物品
					pDBStorage->set_itemid(0);
				}
				pRDH->AddGuildLog_Storage(pGuild,G_Log_SendReward,pDBTUser->userid(),pRceOpt->tarusername(),nID,nCnt);
				
				SYS_LOG((int64)pDBCUser->userid(),LT_GBtl_LeaderSendItem,pRDH->GetRegion(),0,pGuild->GetGuildID()<<(int64)pDBTUser->userid()<<nID<<nCnt);

				break;
			}
		}
	}
	//添加工会日志
	pRDH->markGuildLstDirty(pGuild);
	
	//填充返回值
	GuildStorage* pRetStorage = pRseOpt->mutable_storage();
	for(int i=0;i<pDBGuild->storage_size();i++)
	{
		DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(i);
		if(pDBStorage->itemid()<=0)
			continue;
		pRetStorage->add_itemid(pDBStorage->itemid());
		pRetStorage->add_itemcnt(pDBStorage->itemcnt());
	}
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);
}

void DealGuildEvent::Handle_GBtlSeeLastBtl(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();
	time_t ltNow = time(NULL);
	tm tmNow = *localtime(&ltNow);
	int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
	if(nYearDay!=pDBGBInfo->btlday())
	{//今天没人报名工会战
		pSrvOpt->set_ret(59);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	int nTurns = pRceOpt->turns();
	DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pGuild,nTurns);
	if(pDBResult==NULL||pDBResult->result() == GBR_Unknow)
	{
		pSrvOpt->set_ret(52);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(pDBResult->result() == GBR_WinNoEnemy)
	{
		pSrvOpt->set_ret(60);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	GuildBtlDBHandler::Instance().SafeLoadGuildBtlResult(pDBCUser->userid(),pGuild->GetGuildID(),pRDH->GetRegion(),pRceOpt->turns(),nGameID);
}

void DealGuildEvent::Handle_GbtlSetLineup(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();

	DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();

	//if(pDBCUser->position() != GP_Leader)
	if(!GuildInfoCfg::Instance().HasPermit(GuildPositionType(pDBCUser->position()),G_Pmt_SendReward))
	{
		pSrvOpt->set_ret(13);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	if(pDBGBInfo->userway0_size() + pDBGBInfo->userway1_size() + pDBGBInfo->userway2_size() != 
		pRceOpt->userway0_size() + pRceOpt->userway1_size() + pRceOpt->userway2_size())
	{
		pSrvOpt->set_ret(48);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}

	map<int64,bool> mapOldUser;
//#ifndef WIN32
	//临时代码，方便测试

	//校验合理性
	for(int i=0;i<pDBGBInfo->userway0_size();i++)
	{
		int64 nID = pDBGBInfo->userway0(i);
		if(nID<GBTool_Cnt)
			continue;
		mapOldUser[nID] = true;
	}
	for(int i=0;i<pDBGBInfo->userway1_size();i++)
	{
		int64 nID = pDBGBInfo->userway1(i);
		if(nID<GBTool_Cnt)
			continue;
		mapOldUser[nID] = true;
	}
	for(int i=0;i<pDBGBInfo->userway2_size();i++)
	{
		int64 nID = pDBGBInfo->userway2(i);
		if(nID<GBTool_Cnt)
			continue;
		mapOldUser[nID] = true;
	}

	int nTotalToolCnt = 0;
	map<int64,bool>::iterator iter;
	for(int i=0;i<pRceOpt->userway0_size();i++)
	{
		int nToolCnt = 0;
		int64 nMemID;
		safe_atoll(pRceOpt->userway0(i),nMemID);
		if(nMemID<=0)
			continue;
		if(nMemID>GBTool_Empty&&nMemID<GBTool_Cnt)
		{
			nTotalToolCnt ++;
			nToolCnt++;
			if(nToolCnt > 3)
			{
				pSrvOpt->set_ret(50);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			continue;
		}
		
		iter= mapOldUser.find(nMemID);
		if(iter == mapOldUser.end() || iter->second == false)
		{
			pSrvOpt->set_ret(49);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		mapOldUser[nMemID] = false;
	}
	for(int i=0;i<pRceOpt->userway1_size();i++)
	{
		int nToolCnt = 0;
		int64 nMemID;
		safe_atoll(pRceOpt->userway1(i),nMemID);
		if(nMemID<=0)
			continue;
		if(nMemID>GBTool_Empty&&nMemID<GBTool_Cnt)
		{
			nTotalToolCnt ++;
			nToolCnt++;
			if(nToolCnt > 3)
			{
				pSrvOpt->set_ret(50);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			continue;
		}
		iter= mapOldUser.find(nMemID);
		if(iter == mapOldUser.end() || iter->second == false)
		{
			pSrvOpt->set_ret(49);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		mapOldUser[nMemID] = false;
	}
	for(int i=0;i<pRceOpt->userway2_size();i++)
	{
		int nToolCnt = 0;
		int64 nMemID;
		safe_atoll(pRceOpt->userway2(i),nMemID);
		if(nMemID<=0)
			continue;
		if(nMemID>GBTool_Empty&&nMemID<GBTool_Cnt)
		{
			nTotalToolCnt ++;
			nToolCnt++;
			if(nToolCnt > 3)
			{
				pSrvOpt->set_ret(50);
				eh_->sendEventToGamed(e,nGameID);
				return;
			}
			continue;
		}
		iter= mapOldUser.find(nMemID);
		if(iter == mapOldUser.end() || iter->second == false)
		{
			pSrvOpt->set_ret(49);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		mapOldUser[nMemID] = false;
	}
	if(nTotalToolCnt>6)
	{
		pSrvOpt->set_ret(51);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	for(iter = mapOldUser.begin();iter!=mapOldUser.end();iter++)
	{
		if(iter->second == true)
		{//有玩家被客户端丢了
			pSrvOpt->set_ret(48);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
	}
//#endif
	//校验结束
	pDBGBInfo->clear_userway0();
	pDBGBInfo->clear_userway1();
	pDBGBInfo->clear_userway2();
	for(int i=0;i<pRceOpt->userway0_size();i++)
	{
		int64 nMemID;
		safe_atoll(pRceOpt->userway0(i),nMemID);
		pDBGBInfo->add_userway0(nMemID);
	}
	for(int i=0;i<pRceOpt->userway1_size();i++)
	{
		int64 nMemID;
		safe_atoll(pRceOpt->userway1(i),nMemID);
		pDBGBInfo->add_userway1(nMemID);
	}
	for(int i=0;i<pRceOpt->userway2_size();i++)
	{
		int64 nMemID;
		safe_atoll(pRceOpt->userway2(i),nMemID);
		pDBGBInfo->add_userway2(nMemID);
	}
	const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();
	int nCnt = pGBtlTbl->GetWayUserCnt(pGuild->GetGuildLV());
	for(int i=pRceOpt->userway0_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway0(0);
	}
	for(int i=pRceOpt->userway1_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway1(0);
	}
	for(int i=pRceOpt->userway2_size();i<nCnt;i++)
	{
		pDBGBInfo->add_userway2(0);
	}
	pRDH->markGuildLstDirty(pGuild);
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	eh_->sendEventToGamed(e,nGameID);

}

void DealGuildEvent::CheckGuildBtlStep(CountryRgnDatHandler* pRDH)
{
/*===================================================================================
工会战方案：by sskylin
	1.工会战开启时，进行排序
	2.每次轮询时，发送一系列工会数据到读取线程，并将guild数据放入队尾
	3.数据读出后，组包发往game
	4.game无脑发往fight
	5.fight计算后发往game
	6.game无脑发回country
	7.country端记录本次工会战结果
=====================================================================================*/
	if(pRDH == NULL)
		return;
	if(!GuildInfoCfg::Instance().IsGuildBtlEnable(pRDH->GetRegion()))
		return;
#ifdef WIN32  
	//测试代码,临时代码
	if(pRDH->GetRegion()!=0)
		return;
#endif
	GuildBtlStepType emType = pRDH->GetGuildBtlStep();
	GuildBtlStepType emTarType = GBtlType_UnKnow;
	time_t ltNow= time(NULL);
	tm tmNow	= *localtime(&ltNow);
	int nSecNow	= tmNow.tm_hour*3600 + tmNow.tm_min*60 + tmNow.tm_sec;
	const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();

	if(nSecNow >= pGBtlTbl->m_nSignUpStartTM && nSecNow<pGBtlTbl->m_nLineUpStartTM[0])
	{
		emTarType = GBtlType_SignUp;
	}
	else if(nSecNow < pGBtlTbl->m_nBtlStartTM[0])
	{
		emTarType = GBtlType_LineUp0;
	}
	else if(nSecNow < pGBtlTbl->m_nLineUpStartTM[1])
	{
		if(!pRDH->IsCurTurnsPKOver(0))
		{
			emTarType = GBtlType_Btl0;
		}
	}
	else if(nSecNow < pGBtlTbl->m_nBtlStartTM[1])
	{
		if(pRDH->IsCurTurnsPKOver(0))
		{
			emTarType = GBtlType_LineUp1;
		}
	}
	else if(nSecNow < pGBtlTbl->m_nLineUpStartTM[2])
	{
		if(!pRDH->IsCurTurnsPKOver(1))
		{
			emTarType = GBtlType_Btl1;
		}
	}
	else if(nSecNow < pGBtlTbl->m_nBtlStartTM[2])
	{
		if(pRDH->IsCurTurnsPKOver(1))
		{
			emTarType = GBtlType_LineUp2;
		}
	}
	else if(nSecNow < pGBtlTbl->m_nBalanceTM)
	{
		if(!pRDH->IsCurTurnsPKOver(2))
		{
			emTarType = GBtlType_Btl2;
		}
	}
	else
	{
		emTarType = GBtlType_Over;
	}
	//系统纠错，防止计算时间过程或服务器当机造成工会战未结束
	if(emTarType >= GBtlType_LineUp1 &&!pRDH->IsCurTurnsPKOver(0))
	{
		emTarType = GBtlType_Btl0;
	}
	else if(emTarType >= GBtlType_LineUp2 &&!pRDH->IsCurTurnsPKOver(1))
	{
		emTarType = GBtlType_Btl1;
	}
	else if(emTarType >= GBtlType_Over &&!pRDH->IsCurTurnsPKOver(2))
	{
		emTarType = GBtlType_Btl2;
	}

	if(emTarType!=GBtlType_UnKnow&&emTarType != emType)
	{//状态改变
		pRDH->CheckGuildBtlData();

		pRDH->SetGuildBtlSetp(emTarType);
		int nTurns = pRDH->GetGuildPKTurns();	
		switch(emTarType)
		{
		case GBtlType_LineUp0:
		case GBtlType_LineUp1:
		case GBtlType_LineUp2:
			{//在工会战开启式，根据规则对工会进行分组，如果此时服务器重启，则剩余的工会重新分组
				
				pRDH->SetCurTurnsPKOver(nTurns,false);
				//配表时间结束后，结算积分
				if(nTurns>0)
				{
					pRDH->BalanceGuildBtlTurns(nTurns-1,eh_);
				}

				int nSession=0;
				int nStep = 0;
				time_t ltNow = time(NULL);
				GuildInfoCfg::Instance().GetGuildPKSession(ltNow,nSession,nStep);
				if(nStep==0)
				{
					pRDH->MatchGuildBtlGroup(nTurns,false);
				}
				else
				{
					pRDH->MatchGuildBtlGroup(nTurns,true);
				}
			}
			break;
		default:
			break;
		}
	}

	emType = pRDH->GetGuildBtlStep();
	switch(emType)
	{//团战计算
	case GBtlType_Over:
		{
			if(nSecNow >= pGBtlTbl->m_nBalanceTM)
			{
				pRDH->BalanceGuildBtlTurns(2,eh_);
			}
		}
		break;
	case GBtlType_Btl0:
	case GBtlType_Btl1:
	case GBtlType_Btl2:
		{
			int nTurns = pRDH->GetGuildPKTurns();
			if(pRDH->IsCurTurnsPKOver(nTurns))
				break;
			//增加判断，如果没有匹配，重新匹配，用于防止服务器当机等状况
			{
				if(nTurns>0)
				{
					pRDH->BalanceGuildBtlTurns(nTurns-1,eh_);
				}

				int nSession=0;
				int nStep = 0;
				time_t ltNow = time(NULL);
				GuildInfoCfg::Instance().GetGuildPKSession(ltNow,nSession,nStep);
				if(nStep==0)
				{
					pRDH->MatchGuildBtlGroup(pRDH->GetGuildPKTurns(),false);
				}
				else
				{
					pRDH->MatchGuildBtlGroup(pRDH->GetGuildPKTurns(),true);
				}
			}

			bool bRet = true;
			for(int i = 0; i < 20; i++){
				CGuild* pGuild1=NULL;
				CGuild* pGuild2=NULL;
				int nSize=0;
				bRet = pRDH->GetMatchGuild(&pGuild1,&pGuild2,nTurns,nSize);
				if(!bRet)
					break;
				if(pGuild1==NULL&&pGuild2==NULL)
					continue;
				if(pGuild1==NULL||pGuild2==NULL)
				{//轮空
					CGuild* pWinGuild = (pGuild1!=NULL)?pGuild1:pGuild2;
					DB_C_GuildBtl* pDBGBtl = pWinGuild->GetDBGuild()->mutable_btlinfo();
					DB_C_GuildBtlResult* pDBResult = pRDH->GetDBGuildBtlResult(pWinGuild,nTurns);
					pDBResult->set_winway0(GBOResult_WinNoEnemy);
					pDBResult->set_winway1(GBOResult_WinNoEnemy);
					pDBResult->set_winway2(GBOResult_WinNoEnemy);
					pRDH->CacuGuildBtlScore(pWinGuild,nTurns);
					pRDH->markGuildLstDirty(pWinGuild);

					if(nSize != 1){
						continue;
					}
					//本轮工会战结束
					CCountry* pECountry = pRDH->GetEarthCountry();
					DB_C_PubGuildBtlInfo* pDBPGB = pECountry->GetDBCountry()->mutable_pubguildbtlinfo();
					pRDH->m_GuildBtlReceive[nTurns] = 0;
					pRDH->SetCurTurnsPKOver(nTurns,true);
					SYS_LOG(0,LT_GBtl_TurnsOver,pRDH->GetRegion(),0,nTurns<<pDBPGB->pklst_size());
					pDBPGB->set_pklstidx(0);
					pDBPGB->clear_pklst();
					pRDH->SortPKGuild();
					pRDH->markCountryDirty(pECountry);
					break;
				}
				//双方都有数据
				for(int nWay=0;nWay<GUILD_BTL_DAY_CNT;nWay++)
				{
					GuildBtlDBHandler::Instance().DealGuildBtl(pGuild1,pGuild2,pRDH->GetRegion(),nTurns,nWay);
					/*DB_C_GuildBtlResult* pDBRet1 = pRDH->GetDBGuildBtlResult(pGuild1,nTurns);
					if(pDBRet1->winway0() == GBOResult_Unknow)
					{
						GuildBtlDBHandler::Instance().DealGuildBtl(pGuild1,pGuild2,pRDH->GetRegion(),nTurns,nWay);
					}
					if(pDBRet1->winway1() == GBOResult_Unknow)
					{
						GuildBtlDBHandler::Instance().DealGuildBtl(pGuild1,pGuild2,pRDH->GetRegion(),nTurns,nWay);
					}
					if(pDBRet1->winway1() == GBOResult_Unknow)
					{
						GuildBtlDBHandler::Instance().DealGuildBtl(pGuild1,pGuild2,pRDH->GetRegion(),nTurns,nWay);
					}*/
				}
				/*
				if(i>=nSize)
					break;*/
			}
		}
	}
}


void DealGuildEvent::Handle_GveOpen(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	if ( pDBCUser == NULL )
	{
		return;
	}

	if ( pRseOpt == NULL )
	{
		return;
	}

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	int nGvePoint = pDBCUser->gvepoint();
	pDBCUser->set_gvepoint(pSrvOpt->gvepoint());
	if ( nGvePoint != pSrvOpt->gvepoint() )
	{
		pRDH->markUserLstDirty(pDBCUser->userid());
	}

	pGuild->ResetRank();
	pRDH->CheckGuildGveMsg(pGuild);

	CopyGveMsg( pRseOpt->mutable_nowmsg(), pDBCity->mutable_nowmsg() );
	CopyGveMsg( pRseOpt->mutable_lastmsg(), pDBCity->mutable_lastmsg() );
	CopyGveMsg( pRseOpt->mutable_highestmsg(), pDBCity->mutable_highestmsg() );
	CopyGveMsg( pRseOpt->mutable_lastmsgnew(), pDBCity->mutable_lastmsg() );
	CopyGveMsg( pRseOpt->mutable_highestmsgnew(), pDBCity->mutable_highestmsg() );

	pRseOpt->mutable_lastmsg()->set_rank( pRDH->GetLastRank(pGuild) );
	pRseOpt->mutable_highestmsg()->set_rank( pRDH->GetHighestRank(pGuild) );

	pRseOpt->mutable_lastmsgnew()->set_rank( pRDH->GetLastRankNew(pGuild) );
	pRseOpt->mutable_highestmsgnew()->set_rank( pRDH->GetHighestRankNew(pGuild) );

	FillGveBuffMsg(*pRseOpt,pDBCity);

	pRseOpt->mutable_nowpersonmsg()->set_rank(pGuild->GetRank(pDBCUser));
	pRseOpt->mutable_lastpersonmsg()->set_rank(pGuild->GetLastRank(pDBCUser));
	pRseOpt->set_lastallcountryrank( pRDH->GetLastRank(pGuild) );

	pRDH->markGuildLstDirty(pGuild);

	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::CopyGveMsg( GVEMsg* pMsg, DB_GVE_Msg* pDBMsg )
{
	if ( pMsg == NULL || pDBMsg == NULL )
	{
		return;
	}
	
	pMsg->set_gvestep(pDBMsg->gvestep());
	pMsg->set_point(pDBMsg->point());
	pMsg->set_rank(pDBMsg->rank());
	pMsg->set_playercnt(pDBMsg->playercnt());
	pMsg->set_overguildstep(pDBMsg->overguildstep());
	pMsg->set_overguildpoint(pDBMsg->overguildpoint());
	pMsg->set_overguildmem(pDBMsg->overguildmem());
}

void DealGuildEvent::Handle_GveNpcList(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	int nPage = pRceOpt->page();
	if(nPage<0)
		nPage = 0;

	int nIdx = nPage * SEND_CITY_USER_CNT;
	int nCnt = 0;
	int i = 0;
	for ( i = 0; i < (int)pDBCity->npc_size() && nCnt < nIdx; i++ )
	{
		DB_C_NPC* pUser = pDBCity->mutable_npc(i);
		if(pUser->npcid()<=0)
			continue;

		nCnt++;
	}
	nIdx = i;
	nCnt = 0;

	for(int i=nIdx;i<(int)pDBCity->npc_size()&&nCnt<SEND_CITY_USER_CNT;i++)
	{
		DB_C_NPC* pUser = pDBCity->mutable_npc(i);
		if(pUser->npcid()<=0)
			continue;
		int nNPCTblID = pUser->npcid()/COUNTRY_NPC_RATE;
		const GveNPCTbl* pNPC = GVEInfoCfg::Instance().GetNpcTbl(nNPCTblID);
		if(pNPC==NULL)
			continue;
		CountryUser* pSendUser = pRseOpt->add_npclist();
		pSendUser->set_baseid(toString(pUser->npcid()));
		pSendUser->set_morale(pUser->morale());
		pSendUser->set_first_name("");
		pSendUser->set_combatpowervalue(pNPC->m_nCombatValue);
		pSendUser->set_lvl(pNPC->m_nLevel);

		nCnt++;
	}
	CopyGveMsg( pRseOpt->mutable_nowmsg(), pDBCity->mutable_nowmsg() );
	pRseOpt->set_citycnt( pRDH->GetOverCnt(pGuild) );
	pRseOpt->set_allnpccnt( pGuild->GetNpcCnt() );
	pRseOpt->set_page(nPage);
	pRDH->FillFirstDownMsg(pRseOpt);
	FillGveBuffMsg(*pRseOpt,pDBCity);
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::Handle_GveSpNpcList(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	int nPage = pRceOpt->page();
	if(nPage<0)
		nPage = 0;

	int nIdx = nPage * SEND_CITY_USER_CNT;
	int nCnt = 0;
	int i = 0;
	for ( i = 0; i < (int)pDBCity->spnpc_size() && nCnt < nIdx; i++ )
	{
		DB_C_NPC* pUser = pDBCity->mutable_spnpc(i);
		if(pUser->npcid()<=0)
			continue;

		nCnt++;
	}
	nIdx = i;
	nCnt = 0;

	for(int i=nIdx;i<(int)pDBCity->spnpc_size()&&nCnt<SEND_CITY_USER_CNT;i++)
	{
		DB_C_NPC* pUser = pDBCity->mutable_spnpc(i);
		if(pUser->npcid()<=0)
			continue;
		int nNPCTblID = pUser->npcid()/COUNTRY_NPC_RATE;
		const GveNPCTbl* pNPC = GVEInfoCfg::Instance().GetNpcTbl(nNPCTblID);
		if(pNPC==NULL)
			continue;
		CountryUser* pSendUser = pRseOpt->add_npclist();
		pSendUser->set_baseid(toString(pUser->npcid()));
		pSendUser->set_morale(pUser->morale());
		pSendUser->set_first_name("");
		pSendUser->set_combatpowervalue(pNPC->m_nCombatValue);
		pSendUser->set_lvl(pNPC->m_nLevel);

		nCnt++;
	}
	CopyGveMsg( pRseOpt->mutable_nowmsg(), pDBCity->mutable_nowmsg() );
	pRseOpt->set_citycnt( pRDH->GetOverCnt(pGuild) );
	pRseOpt->set_allnpccnt( pGuild->GetSpNpcCnt() );
	pRseOpt->set_page(nPage);
	pRDH->FillFirstDownMsg(pRseOpt);
	FillGveBuffMsg(*pRseOpt,pDBCity);
	eh_->sendEventToGamed(e,nGameID); 
}


void DealGuildEvent::Handle_GveAttackOver(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	Country_FightBackData* pFightBackData = e->mutable_countryfightbackdata();
	PVPFightUser* pMsgAtk = pFightBackData->mutable_atkuser();
	PVPFightUser* pMsgDef = pFightBackData->mutable_defuser();
	GCG_CountryOpt* pArgs = e->mutable_countryopt();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	if(pDBCUser==NULL)
		return;
// 	if(!CountryInfoCfg::Instance().IsNearCity(pMsgAtk->city(),pMsgDef->city()))
// 	{//game端已做客户端判断
// 		return;
// 	}
	int64 nDefID = 0;
	safe_atoll(pMsgDef->userid(),nDefID);
	int nIdx = nDefID%COUNTRY_NPC_RATE;
	int nStep = (int)(nDefID/COUNTRY_NPC_RATE);
	bool bSpNpc = false;
	if ( nIdx >= GVE_SP_NPC_ADD )
	{
		bSpNpc = true;
		nIdx -= GVE_SP_NPC_ADD;
	}

	int nError = 0;
	if(nIdx >= pDBCity->npc_size()&&!bSpNpc)
	{
		pArgs->set_ret(208);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	if(nIdx >= pDBCity->spnpc_size()&&bSpNpc)
	{
		pArgs->set_ret(208);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	DB_C_NPC* pDBNPC = NULL;
	if ( bSpNpc )
	{
		pDBNPC = pDBCity->mutable_spnpc(nIdx);
	}
	else
	{
		pDBNPC = pDBCity->mutable_npc(nIdx);
	}
	if(pDBNPC==NULL||pDBNPC->npcid()<=0||pDBNPC->morale()<=0||pDBNPC->npcid()!=nDefID)
	{//该NPC已经被挂掉
		pArgs->set_ret(208);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}
	pArgs->set_ret(0);
	pDBNPC->set_morale(pDBNPC->morale()-pMsgDef->costmorale());
	int nMorale = pDBNPC->morale();
	if(pDBNPC->morale()<=0)
	{//NPC被击败
		//从列表中删除NPC
		if (  bSpNpc )
		{
			pFightBackData->set_enemyback(true);
			pDBNPC->set_morale(CountryInfoCfg::Instance().GetCountryBaseTbl()->m_nInitMorale);
		}
		else
		{
			pDBNPC->Clear();
			pFightBackData->set_enemyback(true);
			pGuild->m_nNpcCnt -= 1;
		}
	}
	pMsgDef->set_morale(nMorale);

// 	if(pDBCity->npc_size()<=0&&
// 		pMsgAtk->morale()>pMsgAtk->costmorale())
	{
		bool bSuc = CheckCaptureCity(pDBCity);
		if(bSuc)
		{
			pFightBackData->set_capturecity(true);

			int nStep = pDBCity->mutable_nowmsg()->gvestep();
			pArgs->set_tarcity(nStep);
			pDBCity->mutable_nowmsg()->set_gvestep(nStep+1);
			pGuild->ResetNpc();
			SYS_LOG(pGuild->GetGuildID(),LT_GVE_COUNGRY_Step,0,0,nStep+1);

			SYS_STAT(0,"gvestep",0,0,false,0,0,0,0,0,pGuild->m_nRegion,0,0,false,pGuild->GetGuildID(),nStep+1,0,0,0);

			pRDH->SetFirstDown(pGuild,nStep);
		}
	}

	bool bFind = false;
	for ( int i = 0; i < pDBCity->playerlist_size(); i++ )
	{
		if ( pDBCity->playerlist(i) == pDBCUser->userid() )
		{
			bFind = true;
			break;
		}
	}
	if ( !bFind )
	{
		pDBCity->add_playerlist( pDBCUser->userid() );
	}
	
	pDBCity->mutable_nowmsg()->set_playercnt( pDBCity->playerlist_size() );

	CopyGveMsg( pRseOpt->mutable_nowmsg(), pDBCity->mutable_nowmsg() );
	CopyGveMsg( pRseOpt->mutable_lastmsg(), pDBCity->mutable_lastmsg() );
	CopyGveMsg( pRseOpt->mutable_highestmsg(), pDBCity->mutable_highestmsg() );

	pRDH->markGuildLstDirty(pGuild);

	eh_->sendEventToGamed(e,nGameID);

}

void DealGuildEvent::Handle_GveGetBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	DB_GVE_Msg* pMsg = pDBCity->mutable_nowmsg();
	if ( pMsg == NULL )
	{
		return;
	}

	if ( pRceOpt->boxid() >= pMsg->gvestep() )
	{
		//不能领取
		pSrvOpt->set_ret( 202 );
		eh_->sendEventToGamed(e,nGameID); 
		return;
	}
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::Handle_GveRankBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	GveBaseTbl* pTbl = GVEInfoCfg::Instance().GetGveBaseTbl();

	if ( pTbl == NULL )
	{
		return;
	}
	
	if ( pTbl->m_bOld )
	{
		pSrvOpt->set_lastrank( pRDH->GetLastRank(pGuild) );
	}
	
	else
	{
		pSrvOpt->set_lastrank( pRDH->GetLastRankNew(pGuild) );
	}
	
	if ( pGuild->GetLastRank(pDBCUser) <= 0 )
	{
		pSrvOpt->set_ret( 203 );
	}
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::Handle_GveUseBuff(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}
	
	GveBuffTbl* pTbl = GVEInfoCfg::Instance().GetBuffTbl( pRceOpt->buffid() );
	if ( pTbl == NULL )
	{
		return;
	}

	time_t tNow = time(NULL);

	if ( pTbl->m_nType == GveBuffType_Master )
	{
		if ( pDBCUser->position() != GP_Leader )
		{
			//不是会长
			pSrvOpt->set_ret( 200 );
			eh_->sendEventToGamed(e,nGameID); 
			return;
		}

		for ( int i = 0; i < pDBCity->bufflist_size(); i++ )
		{
			if ( pDBCity->bufflist(i).buffid() == pRceOpt->buffid() )
			{
				pSrvOpt->set_ret( 201 );
				eh_->sendEventToGamed(e,nGameID); 
				return;
			}
		}

		DB_GVE_Buff* pBuff = pDBCity->add_bufflist();
		pBuff->set_buffid( pRceOpt->buffid() );
		pBuff->set_buffendtime( tNow + pTbl->m_nTime );
	}

	else if ( pTbl->m_nType == GveBuffType_First )
	{
		if ( pDBCity->lastlist_size() == 0 && pDBCUser->position() != GP_Leader )
		{
			//不是会长
			pSrvOpt->set_ret( 200 );
			eh_->sendEventToGamed(e,nGameID); 
			return;
		}
		else if ( pGuild->GetLastRank(pDBCUser) != 1 )
		{
			//不是第一名
			pSrvOpt->set_ret( 206 );
			eh_->sendEventToGamed(e,nGameID); 
			return;
		}

		for ( int i = 0; i < pDBCity->bufflist_size(); i++ )
		{
			if ( pDBCity->bufflist(i).buffid() == pRceOpt->buffid() )
			{
				pSrvOpt->set_ret( 201 );
				eh_->sendEventToGamed(e,nGameID); 
				return;
			}
		}

		DB_GVE_Buff* pBuff = pDBCity->add_bufflist();
		pBuff->set_buffid( pRceOpt->buffid() );
		pBuff->set_buffendtime( tNow + pTbl->m_nTime );
	}

	FillGveBuffMsg(*pRseOpt,pDBCity);

	pRDH->markGuildLstDirty(pGuild);
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::Handle_GveRank(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID,int nType,int nCountry)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);
	pRseOpt->set_listcountryid(nCountry);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	if ( nType == GOT_GVE_Rank )
	{
		for ( int i = 0; i < pDBCity->nowlist_size(); i++ )
		{
			DB_GVE_List* pDBList = pDBCity->mutable_nowlist(i);
			if ( pDBList == NULL )
			{
				continue;
			}
			if ( pDBList->gvepoint() <= 0 )
			{
				continue;
			}
			GVEList* pList = pRseOpt->add_nowlist();

			FillGveRankList(pList,pDBList);
		}
	}

	else if ( nType == GOT_GVE_LastRank )
	{
		for ( int i = 0; i < pDBCity->lastlist_size(); i++ )
		{
			DB_GVE_List* pDBList = pDBCity->mutable_lastlist(i);
			if ( pDBList == NULL )
			{
				continue;
			}
			if ( pDBList->gvepoint() <= 0 )
			{
				continue;
			}
			GVEList* pList = pRseOpt->add_lastlist();

			FillGveRankList(pList,pDBList);
		}
	}

	else if ( nType == GOT_GVE_GuildRank )
	{
		vector<CGuild*> lstGuildGve;
		if ( nCountry == 0 )
		{
			lstGuildGve = pRDH->m_lstGuildGveNow;
		}
		else if ( nCountry >= C_Start && nCountry < C_UserCoutryEnd )
		{
			lstGuildGve = pRDH->m_lstGuildGveNowNew[nCountry];
		}
		for ( int i = 0; i < lstGuildGve.size() && i < GVE_MAX_RANK; i++ )
		{
			CGuild* pGuild = lstGuildGve[i];
			if ( pGuild == NULL )
			{
				continue;
			}

			if (pGuild->m_nPoint <= 0)
			{
				continue;
			}

			DB_GVE_City* pDBCity = pGuild->GetDBGve();
			if ( pDBCity == NULL )
			{
				continue;
			}

			GVEGuildList* pList = pRseOpt->add_guildlist();

			pList->set_guildid(pGuild->GetGuildID());
			pList->set_guildlv(pGuild->GetGuildLV());
			pList->set_mastername(pGuild->GetLeaderName());
			pList->set_guildname(pGuild->GetGuildName());
			pList->set_guildpoint( pDBCity->nowmsg().point() );
			pList->set_guildrate(pDBCity->nowmsg().gvestep());
			pList->set_guildcountry(pGuild->GetCountry());

		}
	}

	else if ( nType == GOT_GVE_LastGuildRank )
	{
		vector<CGuild*> lstGuildGve;
		if ( nCountry == 0 )
		{
			lstGuildGve = pRDH->m_lstGuildGveLast;
		}
		else if ( nCountry >= C_Start && nCountry < C_UserCoutryEnd )
		{
			lstGuildGve = pRDH->m_lstGuildGveLastNew[nCountry];
		}
		for ( int i = 0; i < lstGuildGve.size() && i < GVE_MAX_RANK; i++ )
		{
			CGuild* pGuild = lstGuildGve[i];
			if ( pGuild == NULL )
			{
				continue;
			}

			DB_GVE_City* pDBCity = pGuild->GetDBGve();
			if ( pDBCity == NULL )
			{
				continue;
			}

			if (pDBCity->lastmsg().point() <= 0)
			{
				continue;
			}

			GVEGuildList* pList = pRseOpt->add_guildlist();

			pList->set_guildid(pGuild->GetGuildID());
			pList->set_guildlv(pGuild->GetGuildLV());
			pList->set_mastername(pGuild->GetLeaderName());
			pList->set_guildname(pGuild->GetGuildName());
			pList->set_guildpoint(pDBCity->lastmsg().point());
			pList->set_guildrate(pDBCity->lastmsg().gvestep());
			pList->set_guildcountry(pGuild->GetCountry());
		}
	}

	else if ( nType == GOT_GVE_HistoryRank )
	{
		vector<CGuild*> lstGuildGve;
		if ( nCountry == 0 )
		{
			lstGuildGve = pRDH->m_lstGuildGveHighest;
		}
		else if ( nCountry >= C_Start && nCountry < C_UserCoutryEnd )
		{
			lstGuildGve = pRDH->m_lstGuildGveHighestNew[nCountry];
		}
		for ( int i = 0; i < lstGuildGve.size() && i < GVE_MAX_RANK; i++ )
		{
			CGuild* pGuild = lstGuildGve[i];
			if ( pGuild == NULL )
			{
				continue;
			}

			DB_GVE_City* pDBCity = pGuild->GetDBGve();
			if ( pDBCity == NULL )
			{
				continue;
			}

			if (pDBCity->highestmsg().point() <= 0)
			{
				continue;
			}

			GVEGuildList* pList = pRseOpt->add_guildlist();

			pList->set_guildid(pGuild->GetGuildID());
			pList->set_guildlv(pGuild->GetGuildLV());
			pList->set_mastername(pGuild->GetLeaderName());
			pList->set_guildname(pGuild->GetGuildName());
			pList->set_guildpoint(pDBCity->highestmsg().point());
			pList->set_guildrate(pDBCity->highestmsg().gvestep());
			pList->set_guildcountry(pGuild->GetCountry());
		}
	}

	eh_->sendEventToGamed(e,nGameID); 
}


bool DealGuildEvent::CheckCaptureCity( DB_GVE_City* pDBCity)
{
	if ( pDBCity == NULL )
	{
		return false;
	}

	for ( int i = 0; i < pDBCity->npc_size(); i++ )
	{
		DB_C_NPC* pNpc = pDBCity->mutable_npc(i);
		if ( pNpc->morale() > 0 )
		{
			return false;
		}
	}
	return true;
}

void DealGuildEvent::Handle_GveMap(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	if ( pRseOpt == NULL )
	{
		return;
	}

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	int nGvePoint = pDBCUser->gvepoint();
	pDBCUser->set_gvepoint(pSrvOpt->gvepoint());
	if ( nGvePoint != pSrvOpt->gvepoint() )
	{
		pRDH->markUserLstDirty(pDBCUser->userid());
	}

	CopyGveMsg( pRseOpt->mutable_nowmsg(), pDBCity->mutable_nowmsg() );

	for ( int i = 0; i < pDBCity->bufflist_size(); i++ )
	{
		GVEBuff* pBuff = pRseOpt->add_gvebuff();
		pBuff->set_buffid(pDBCity->bufflist(i).buffid());
		pBuff->set_endtime(pDBCity->bufflist(i).buffendtime());
	}
	FillGveBuffMsg(*pRseOpt,pDBCity);
	
	pRDH->FillFirstDownMsg(pRseOpt);
	pRseOpt->mutable_nowpersonmsg()->set_rank(pGuild->GetRank(pDBCUser));
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::Handle_GveAllRankBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();
	RseGuildOpt* pRseOpt = e->mutable_se_rseguildopt();
	pRseOpt->set_ret(0);

	DB_GVE_City* pDBCity = pGuild->GetDBGve();
	if ( pDBCity == NULL )
	{
		return;
	}

	GveBaseTbl* pTbl = GVEInfoCfg::Instance().GetGveBaseTbl();

	if ( pTbl == NULL )
	{
		return;
	}

	pSrvOpt->set_lastrank( pRDH->GetLastRank(pGuild) );

	if ( pGuild->GetLastRank(pDBCUser) <= 0 )
	{
		pSrvOpt->set_ret( 203 );
	}
	eh_->sendEventToGamed(e,nGameID); 
}

void DealGuildEvent::FillGveBuffMsg( RseGuildOpt& rep, DB_GVE_City* pDBCity )
{
	for ( int i = 0; i < GVEInfoCfg::Instance().GetBuffTblNum(); i++ )
	{
		GveBuffTbl* pTbl = GVEInfoCfg::Instance().GetBuffTblByNum(i);
		if ( pTbl == NULL )
		{
			continue;
		}
		
		DB_GVE_Buff* pDBBuff = NULL;
		for ( int j = 0; j < pDBCity->bufflist_size(); j++ )
		{
			if ( pDBCity->bufflist(j).buffid() == pTbl->m_nID )
			{
				pDBBuff = pDBCity->mutable_bufflist(j);
			}
		}
		
		GVEBuff* pMsg = rep.add_gvebuff();
		pMsg->set_buffid( pTbl->m_nID );
		if ( pDBBuff == NULL )
		{
			pMsg->set_endtime(0);
		}
		else
		{
			pMsg->set_endtime( pDBBuff->buffendtime() );
		}
	}
}

void DealGuildEvent::FillGveRankList( GVEList* pList, DB_GVE_List* pDBList )
{
	if ( pList == NULL || pDBList == NULL )
	{
		return;
	}

	pList->set_uid(pDBList->uid());
	pList->set_name(pDBList->name());
	pList->set_guildpoint(pDBList->gvepoint());
	pList->set_level(pDBList->level());
	pList->set_officerposition(pDBList->officerposition());
	pList->set_viptype(pDBList->viptype());
	pList->set_viplevel(pDBList->viplevel());

}

void DealGuildEvent::Handle_SetLeader(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	//指令设置新会长,从chgleader copy的,这里的taruser为会长,dbcuser为新会长
	RceGuildOpt* pRceOpt = e->mutable_ce_rceguildopt();
	GCG_GuildOpt* pSrvOpt= e->mutable_guildopt();
	CountryDataHandler* pDH= eh_->getDataHandler();

	//int64 nTarID = pSrvOpt->taruserid();
	int64 nTarID = pGuild->GetLeaderID();
	DB_C_User* pDBTarUser = pRDH->GetDBCityUser(nTarID);

	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	//pDBGuild->set_leaderid(pSrvOpt->taruserid());
	//pDBGuild->set_leadername(pSrvOpt->leadername());
	pDBGuild->set_leaderid(pDBCUser->userid());
	pDBGuild->set_leadername(pDBCUser->name());
	//pDBGuild->set_leaderlasttm((int)time(NULL));
	pDBCUser->set_position(GP_Leader);

	int64 nFromID	= 0;
	if (NULL != pDBTarUser)
	{
		if(pDBTarUser->guildid()!=pDBCUser->guildid()||pDBTarUser->countryid()!=pDBCUser->countryid())
		{//数据错误	
			pSrvOpt->set_ret(15);
			eh_->sendEventToGamed(e,nGameID);
			return;
		}
		pDBTarUser->set_position(GP_Normal);
		pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pDBTarUser->userid(),pDBTarUser->name(),GP_Normal);
		//转让官员身份
		CCountry*pCountry = pRDH->GetCountry(pDBTarUser->countryid());
		if(pCountry&&pDBTarUser->officerposition()>0)
		{
			if(pCountry->ChangeOfficerPosition(pDBTarUser,pDBCUser,E_Officer_Type_Guild))
			{
				pRDH->markCountryDirty(pCountry);
				//eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);
				eh_->SendCountryUserLiteInfo(pRDH,pDBTarUser);
			}
		}
		pRDH->markUserLstDirty(nTarID);
		nFromID = pDBTarUser->userid();

		Event evn;
		evn.set_cmd(C2S_RceGuildOpt);
		evn.set_uid(nFromID);
		evn.set_time(-1);
		evn.set_state(Status_Country_To_Game);
		RceGuildOpt* pRceOpt1 = evn.mutable_ce_rceguildopt(); 
		pRceOpt1->set_type(GOT_SET_LEADER);
		GCG_GuildOpt* pOpt1 = evn.mutable_guildopt();
		pOpt1->set_guildid(pGuild->GetGuildID());
		pOpt1->set_country(pGuild->GetCountry());
		pOpt1->set_position(pDBTarUser->position());
		pOpt1->set_taruserid(nFromID);
		GuildData* pGData = pOpt1->mutable_guild();
		pRDH->FillGuildData(pGuild,pGData);	
		eh_->sendEventToGamed(&evn,nGameID);
	}

	pRDH->AddGuildLog_Pos(pGuild,G_Log_ChgPos,pDBCUser->userid(),pDBCUser->name(),GP_Leader);
	
	pRDH->markUserLstDirty(pDBCUser->userid());
	pRDH->markGuildLstDirty(pGuild);
	eh_->SendCountryUserLiteInfo(pRDH,pDBCUser);

	//int64 nFromID	= pDBTarUser->userid();
	//int64 nToID		= nTarID;

	SYS_LOG(nFromID,LT_Guild_CheLeader,pRDH->GetRegion(),0,pGuild->GetCountry()<<pGuild->GetGuildID()<<pDBCUser->userid());
	pSrvOpt->set_ret(0);
	pSrvOpt->set_taruserid(pDBCUser->userid());
	GuildData* pGData = pSrvOpt->mutable_guild();
	pRDH->FillGuildData(pGuild,pGData);	
	GCG_GuildOpt* pOpt = e->mutable_guildopt();
	pOpt->set_position(pDBCUser->position());
	eh_->sendEventToGamed(e,nGameID);
}