#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/SkuIdTbl.h"
#include "RceUpdateShipsHandle.h"


RceUpdateShipsHandle::RceUpdateShipsHandle()
:CBaseEvent()
{

}
RceUpdateShipsHandle::~RceUpdateShipsHandle()
{

}

void RceUpdateShipsHandle::handle(Event* e)
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

	RceUpdateShips* req = e->mutable_ce_rceupdateships();
	if( !req )
	{
		return;
	}

	RseUpdateShips rse;
	rse.set_ret(RseUpdateShipsRet_Success);

	int nPlanetId = req->planetid();
	DB_Planet *pDBPlanet = pPlayer->GetPlanet(nPlanetId);
	if(!pDBPlanet){
		SendRet2User(pUser,RseUpdateShipsRet_CantFindPlanet,rse);
		return;	
	}
	string strAction = req->action();
	if(strAction == "shipAdded")
	{
		string sku = req->sku();
		int slot = req->slot();
		int timeLeft = req->timeleft();

		// 验证LimeLeft省略
		int sid = req->sid();
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Coin,MT->coins()))
		{
			SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
		}
		for(int i = 0; i < pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == sid)
			{
				if(slot == 0)
					pDBShipyard->set_updatedat(time(NULL));
				if((slot + 1) > pDBShipyard->slot_size())
				{
					DB_Slot* pDBSlot = pDBShipyard->add_slot();
					pDBSlot->set_sku(sku);
					pDBSlot->set_timeleft(timeLeft);
					pDBSlot->set_skunum(pDBSlot->skunum() + 1);
					break;
				}
				else
				{
					DB_Slot* pDBSlot = pDBShipyard->mutable_slot(slot);
					pDBSlot->set_sku(sku);
					pDBSlot->set_timeleft(timeLeft);
					pDBSlot->set_skunum(pDBSlot->skunum() + 1);
					break;
				}
			}
		}
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		eh_->getDataHandler()->markUserDirty(pUser);
		return ;
	}
	else if(strAction == "shipRemoved")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Coin,MT->coins()))
		{
			SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
		}
		string sku = req->sku();
		int slot = req->slot();
		int sid = req->sid();
		for(int i = 0; i < pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == sid)
			{
				if(slot == 0)//第一个建造槽
				{
					DB_Slot* pDBSlot = pDBShipyard->mutable_slot(0);
					DB_Slot* pDBSlot_1 = pDBShipyard->mutable_slot(1);

					if(!pDBSlot || !pDBSlot_1)
					{
						SendRet2User(pUser,RseUpdateShipsRet_SlotNotFind,rse);
						return;
					}
					if(pDBSlot_1->skunum() > 0)
					{
						pDBSlot_1->set_skunum(pDBSlot_1->skunum() - 1);
						pDBSlot->set_skunum(1);
						pDBSlot->set_sku(pDBSlot_1->sku());
						pDBSlot->set_timeleft(pDBSlot_1->timeleft());
					}
				}
				else
				{
					DB_Slot* pDBSlot = pDBShipyard->mutable_slot(slot);
					if(!pDBSlot)
					{
						SendRet2User(pUser,RseUpdateShipsRet_SlotNotFind,rse);
						return ;
					}
					pDBSlot->set_skunum(pDBSlot->skunum() - 1);
				}

			}
			//markdb
		}
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		eh_->getDataHandler()->markUserDirty(pUser);
	}
	else if(strAction == "shipCompleted")
	{	
		for(int i = 0;i<pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == req->sid())
			{
				bool bFind = false;
				DB_Slot* pDBSlot = pDBShipyard->mutable_slot(0);
				for(int i = 1; i < pDBShipyard->slot_size();i++)
				{
						
					DB_Slot* pDBSlot_1 = pDBShipyard->mutable_slot(i);
					if(pDBSlot && pDBSlot_1 && pDBSlot_1->skunum()>0 )//后槽补前槽
					{
						pDBSlot_1->set_skunum(pDBSlot_1->skunum() - 1);
						pDBSlot->set_sku(pDBSlot_1->sku());
						pDBSlot->set_skunum(1);
						pDBSlot->set_timeleft(pDBSlot_1->timeleft());
						bFind = true;
						break;
					}
				}
				if(!bFind)
					pDBSlot->set_skunum(0);
				break;
			}
		}
		//加兵
		bool bFind = false;
 		int Sku = SkuIDTblInst::instance().GetSku(req->sku());
		for(int i = 0;i < pDBPlanet->hangars_size();i++)
		{
			DB_Hangar* pDBHangar = pDBPlanet->mutable_hangars(i);
			if(pDBHangar->sid() == req->hangarsid())
			{
				for(int i = 0;i<pDBHangar->units_size();i++)
				{
					DB_HangarUnit* pDBHangarUnit = pDBHangar->mutable_units(i);
					if(pDBHangarUnit->sku() == Sku)
					{
						pDBHangarUnit->set_num(pDBHangarUnit->num() + 1);
						bFind = true;
						break;
					}
				}
				if(!bFind)//传送门里原来没有这个兵种
				{
					DB_HangarUnit* pDBHangarUnit = pDBHangar->add_units();
					pDBHangarUnit->set_num(1);
					int Sku = SkuIDTblInst::instance().GetSku(req->sku());
					pDBHangarUnit->set_sku(Sku);
					bFind = true;
					break;
				}	
			}
		}
		if(!bFind)
		{
			DB_Hangar* pDBHangar = pDBPlanet->add_hangars();
			pDBHangar->set_sid(req->hangarsid());
			DB_HangarUnit* pDBHangarUnit = pDBHangar->add_units();
			pDBHangarUnit->set_sku(Sku);
			pDBHangarUnit->set_num(pDBHangarUnit->num() + 1);

		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "speedUp")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Cash,MT->cash()))
		{	
			SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
		}
		//秒兵  
		bool bFind = false;
		for(int i = 0;i<req->slotscontentsaccelerated_size();i++)
		{
			SlotsContentsAccelerated* SCA = req->mutable_slotscontentsaccelerated(i);
			for(int i = 0;i < pDBPlanet->hangars_size();i++)
			{
				DB_Hangar* pDBHangar = pDBPlanet->mutable_hangars(i);
				if(pDBHangar->sid() == SCA->hangarsid())
				{
					for(int i = 0;i<pDBHangar->units_size();i++)
					{
						DB_HangarUnit* pDBHangarUnit = pDBHangar->mutable_units(i);
						int Sku = SkuIDTblInst::instance().GetSku(SCA->sku());
						if(pDBHangarUnit->sku() == Sku)
						{
							pDBHangarUnit->set_num(pDBHangarUnit->num() + SCA->amount());
							bFind = true;
							break;
						}
					}
					if(!bFind)//传送门里原来没有这个兵种
					{
						DB_HangarUnit* pDBHangarUnit = pDBHangar->add_units();
						pDBHangarUnit->set_num(SCA->amount());
						int Sku = SkuIDTblInst::instance().GetSku(SCA->sku());
						pDBHangarUnit->set_sku(Sku);
					}
				}		
			}
		}
		//清除造兵槽
		for(int i = 0;i<pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == req->sid())
			{
				for(int i = 0;i < pDBShipyard->slot_size();i++)
				{
					DB_Slot* pDBSlot = pDBShipyard->mutable_slot(i);
					pDBSlot->Clear();
				}
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "block")
	{
		for(int i = 0;i<pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == req->sid());
				//pDBShipyard->set_block(req->block());
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "moveFromHangarToBunker")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Coin,MT->coins()))
		{	
			SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
		}
		//先检查hangar中的兵够不够

		int cnt = 0;
		int Sku = SkuIDTblInst::instance().GetSku(req->unitsku());
		for(int i = 0;i < pDBPlanet->hangars_size();i++)
		{
			DB_Hangar* pDBHangar = pDBPlanet->mutable_hangars(i);
	
			for(int i = 0;i<pDBHangar->units_size();i++)
			{
				DB_HangarUnit* pDBHangarUnit = pDBHangar->mutable_units(i);
				if(pDBHangarUnit->sku() == Sku)
				{
					cnt = pDBHangarUnit->num();
					if(cnt < req->amount())
					{
						SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
						return;
					}
					else
					{
						pDBHangarUnit->set_num(pDBHangarUnit->num() - req->amount());//够了就扣
					}
				}
			}

		}

		//移动到地堡里头
		for(int i = 0;i < pDBPlanet->bunkers_size();i++)
		{
			DB_Bunker* pDBBunker = pDBPlanet->mutable_bunkers(i);
			if(pDBBunker->sid() == req->bunkersid())
			{
				bool bFind = false;
				for(int i = 0;i<pDBBunker->bunker_size();i++)
				{
					DB_BunkerUnit* pDBBunkerUnit = pDBBunker->mutable_bunker(i);
					if(pDBBunkerUnit->sku() == req->unitsku())
					{
						pDBBunkerUnit->set_num(pDBBunkerUnit->num() + req->amount());
						bFind = true;
						break;
					}
				}
				if(!bFind)
				{
					DB_BunkerUnit* pDBBunkerUnit = pDBBunker->add_bunker();
					pDBBunkerUnit->set_sku(req->unitsku());
					pDBBunkerUnit->set_num(req->amount());
				}
			}

		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "killUnitFromHangar")
	{
		int Sku = SkuIDTblInst::instance().GetSku(req->unitsku());
		for(int i = 0;i < pDBPlanet->hangars_size();i++)
		{
			DB_Hangar* pDBHangar = pDBPlanet->mutable_hangars(i);
			if(pDBHangar->sid() == req->hangarsid())
			{
				for(int i = 0;i<pDBHangar->units_size();i++)
				{
					DB_HangarUnit* pDBHangarUnit = pDBHangar->mutable_units(i);
					if(pDBHangarUnit->sku() == Sku)
					{
						if(pDBHangarUnit->num() - 1 < 0)
						{
							SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
							break;
						}
						else 
						{
							pDBHangarUnit->set_num(pDBHangarUnit->num() - 1);
							if(pDBHangarUnit->num() <= 0)
								pDBHangarUnit->set_sku(0);
							break;
						}	
					}
				}
			
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "killUnitFromBunker")
	{
		int Sku = SkuIDTblInst::instance().GetSku(req->unitsku());
		for(int i = 0;i < pDBPlanet->bunkers_size();i++)
		{
			DB_Bunker* pDBBunker = pDBPlanet->mutable_bunkers(i);
			if(pDBBunker->sid() == req->bunkersid())
			{
				for(int i = 0;i<pDBBunker->bunker_size();i++)
				{
					DB_BunkerUnit* pDBBunkerUnit = pDBBunker->mutable_bunker(i);
					if(pDBBunkerUnit->sku() == req->unitsku())
					{
						if(pDBBunkerUnit->num() - req->amount() < 0)
						{
							SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
						}
						else
							pDBBunkerUnit->set_num(pDBBunkerUnit->num() - 1);
					}
				}
			}

		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "addSlot")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Cash,MT->cash()))
		{	
			SendRet2User(pUser,RseUpdateShipsRet_ResNotEnough,rse);
		}
		for(int i = 0;i<pDBPlanet->shipyard_size();i++)
		{
			DB_Shipyard* pDBShipyard = pDBPlanet->mutable_shipyard(i);
			if(pDBShipyard->sid() == req->sid())
			{
				pDBShipyard->set_unlockedslots(pDBShipyard->unlockedslots() + 1);
				pDBShipyard->add_slot();
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateShipsRet_Success,rse);
		return;
	}
	else if(strAction == "giftUnitToHangar")
	{
		int Sku = SkuIDTblInst::instance().GetSku(req->unitsku());
		DB_HangarUnit* pDBHangarUnit = NULL;
		for(int i = 0;i < pDBPlanet->hangars_size();i++)
		{
			DB_Hangar* pDBHangar = pDBPlanet->mutable_hangars(i);
			if(pDBHangar->sid() == req->hangarsid())
			{
				for(int i = 0;i<pDBHangar->units_size();i++)
				{
					DB_HangarUnit* pTMP = pDBHangar->mutable_units(i);
					if(pTMP->sku() == Sku)
					{
						pTMP->set_num(pTMP->num() + req->amount());
						pDBHangarUnit = pTMP;
						break;
					}
				}
				if(!pDBHangarUnit)
				{
					pDBHangarUnit = pDBHangar->add_units();
					pDBHangarUnit->set_sku(Sku);
					pDBHangarUnit->set_num(req->amount());
					break;
				}
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);

	}
	else
	{
		
	}
}

void RceUpdateShipsHandle::SendRet2User(User* pUser,int nRet,RseUpdateShips& rseUpdateShips)
{
	rseUpdateShips.set_ret(nRet);
	string text;
	rseUpdateShips.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateShips,text);
}