#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/NPCInfoCfg.h"
#include "../../logic/SkuIdTbl.h"
#include "RceQueryPvEHandle.h"
#include "RseQueryPvE.pb.h"

RceQueryPvEHandle::RceQueryPvEHandle()
:CBaseEvent()
{

}
RceQueryPvEHandle::~RceQueryPvEHandle()
{
	
}

void RceQueryPvEHandle::handle(Event* e)
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

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer || !pPlayer->CanUse())
	{
		return ;
	}
	
	if(!e->has_ce_rcequerypve()){
		return;
	}
	RceQueryPvE *pReq = e->mutable_ce_rcequerypve();
	RseQueryPvE rsp;
	rsp.set_ret(0);

	switch (pReq->type())
	{
	case QUERY_PVE_LITE:
		{
			pPlayer->FillinPvELite(&rsp);
		}
		break;
	case QUERY_PVE_NPC:
		{
			const NPCPveBattleTbl *pPvECfg = NPCInfoCfg::Instance().GetNPCPveBattleTbl(pReq->pass());
			if(!pPvECfg){
				rsp.set_ret(QUERY_PVE_ERR_CANT_FIND_PASS);
				break;
			}
			NpcCfgJson *pNpcCfg = NPCInfoCfg::Instance().GetNPCInfo(pPvECfg->m_nNpcID);
			if(!pNpcCfg){
				rsp.set_ret(QUERY_PVE_ERR_CANT_FINDNPC);
				break;
			}

			MsgNpcJson *pMsgNpc =  rsp.mutable_npc();
			for(int i = 0; i < pNpcCfg->items.size(); i++){
				MsgBuildingItem *pMsgItem = pMsgNpc->add_item();
				BuildingItemJson pCfgItem = pNpcCfg->items[i];
				pMsgItem->set_sid(pCfgItem.sid);
				string skuName = SkuIDTblInst::instance().GetName(pCfgItem.skuId);
				pMsgItem->set_sku(skuName);
				pMsgItem->set_id(pCfgItem.id);
				pMsgItem->set_upgradeid(pCfgItem.upgradeId);
				pMsgItem->set_type(pCfgItem.type);
				pMsgItem->set_state(pCfgItem.state);
				pMsgItem->set_x(pCfgItem.x);
				pMsgItem->set_y(pCfgItem.y);
				pMsgItem->set_energy(pCfgItem.energy);
				pMsgItem->set_energypercent(pCfgItem.energyPercent);
			}
			for(int i = 0; i < pNpcCfg->bunkers.size(); i++){
				MsgSidSkuNum *pMsgItem = pMsgNpc->add_bunker();
				BunkerJson pCfgItem = pNpcCfg->bunkers[i];
				pMsgItem->set_sid(pCfgItem.sid);
				pMsgItem->set_key(pCfgItem.skuId);
				pMsgItem->set_value(pCfgItem.num);
			}
			for(int i = 0; i < pNpcCfg->gameUnits.size(); i++){
				MsgKeyValue *pMsgItem = pMsgNpc->add_gameunit();
				GameUnitJson pCfgItem = pNpcCfg->gameUnits[i];
				pMsgItem->set_key(pCfgItem.skuId);
				pMsgItem->set_value(pCfgItem.upgradeId);
			}
		}
		break;
	}

	string text;
	rsp.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseQueryPvE, text);
}
