#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/ItemTbl.h"
#include "../../logic/CollectablesTbl.h"
#include "RceUpdateSocialItemHandle.h"


RceUpdateSocialItemHandle::RceUpdateSocialItemHandle()
:CBaseEvent()
{

}
RceUpdateSocialItemHandle::~RceUpdateSocialItemHandle()
{

}

void RceUpdateSocialItemHandle::SendRet2User(User* pUser,int nRet,RseUpdateSocialItem& rseUpdateSocialItem)
{
	rseUpdateSocialItem.set_ret(nRet);
	string text;
	rseUpdateSocialItem.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateSocialItem,text);

}
void RceUpdateSocialItemHandle::handle(Event* e)
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

	RceUpdateSocialItem* req = e->mutable_ce_rceupdatesocialitem();
	if( !req )
	{
		return;
	}

	RseUpdateSocialItem rse;
	rse.set_ret(RseUpdateSocialItemRet_Success);
	
	int planetId = req->planetid();
	string strAction = req->action();
	MsgTransaction* MT = req->mutable_transaction();
	if(MT)
	{
		for(int i = 0;i<MT->socialitems_size();i++)
		{
			MsgSocialItems* MSI = MT->mutable_socialitems(i);
			if(!pPlayer->CostSocialItem(MSI->sku(), 1))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_SocialItemNotEnough,rse);
			}
		}
	}

	if(strAction == "nextStep")
	{
		int quantity = 0;
		int tm = 0;
		if(req->postion() == 1)//爆出物品
		{
			CFG_Item* Tbl = ItemTblInst::instance().GetItem(req->sku());
			if(!Tbl)
				return;
			string givenCondition = Tbl->givenCondition;
			tm = Tbl->timeToGive * 1000;
			string currentCount = toString(req->currentcount());
			if(givenCondition.find("listSequence") >= 0 &&givenCondition.find("listSequence") <= givenCondition.size()&& givenCondition.find(currentCount))
			{
				quantity = 1;
			}
		}
		if(!pPlayer->AddSocialItem(req->sku(),quantity,req->currentquantity(),tm))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
			return;
		}
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
		eh_->getDataHandler()->markUserDirty(pUser);
	}
	else if(strAction == "addItem")
	{
		MsgSocialItems* MSI = NULL;
		for(int i = 0;i<MT->socialitems_size();i++)
		{
			MSI = MT->mutable_socialitems(i);
			if(!pPlayer->AddSocialItem(MSI->sku(),req->currentquantity()))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
				return ;
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "useItem")
	{
		DB_Model* model = pPlayer->GetDBPlayer()->mutable_model();
		int Sku = 0;
		string randomSku = req->num();
		safe_atoi(req->sku(),Sku);
		DB_SocialItem* pDBSI = pPlayer->GetSocialItem(req->sku());
		if(!pDBSI || !model)
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_SocialItemNotEnough,rse);
			return;
		}

		if(Sku == SocialItem_worker_freeWorker)//雇佣工人
		{
			if(!pPlayer->AddDroid(planetId))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_AddDroidFailed,rse);
				return;
			}
			SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
			return;
		}

		if(Sku >= SocialItem_coins && Sku <= 5006)
		{
			switch (Sku) {
			case SocialItem_coins:// 金币×1000
				pPlayer->CostRes(RC_Coin,1000);
				break;
			case SocialItem_minerals:// 矿石×1000
				pPlayer->CostRes(RC_Mineral,1000);;
				break;
			case 5003:// 金币×10000
				pPlayer->CostRes(RC_Coin,10000);
				break;
			case 5004:// 金币×100000
				pPlayer->CostRes(RC_Coin,100000);
				break;
			case 5005:// 矿石×10000
				pPlayer->CostRes(RC_Mineral,10000);
				break;
			case 5006:// 矿石×100000
				pPlayer->CostRes(RC_Mineral,100000);
				break;
			default:
				break;
			}
			if(!pPlayer->CostSocialItem(req->sku(), 1))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_SocialItemNotEnough,rse);
				return;
			}
			SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
			return;
		}
		else
		{
			CFG_Item* Tbl = ItemTblInst::instance().GetItem(req->sku());
			if(!Tbl)
				return;
			string givenCondition = Tbl->givenCondition;
			vector<string> condition = split(givenCondition,":");
			int counter = pDBSI->counter();
			int nextCounter = 0;
			if (condition.size() > 1) {// 不是爆出物品不对counter计算
				nextCounter = (counter + 1) % (condition.size() - 1);
			}
			if(randomSku == "1")// 金币矿石
			{
				int seed = 0;
				safe_atoi(condition[counter + 1],seed);
				if(seed == 1)
				{
					int addCoins = model->coins() * 3 / 100;
					if (addCoins < 1000) {
						addCoins = 1000;
					}
					pPlayer->CostRes(RC_Coin,model->coins() + addCoins);
				}
				else if(seed == 2)
				{
					int addMinerals = model->minerals() * 3 / 100;
					if (addMinerals < 1000) {
						addMinerals = 1000;
					}
					pPlayer->CostRes(RC_Mineral,model->minerals() + addMinerals);
				}
			}
			else
			{
				if(!pPlayer->AddSocialItem(randomSku, 1, 0,0))
				{
					SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
					return;
				}
			}
			pDBSI->set_counter(nextCounter);
			if(!pPlayer->CostSocialItem(req->sku(), 1))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
				return;
			}
			SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
		}
		eh_->getDataHandler()->markUserDirty(pUser);
	}
	else if(strAction == "buyItem")
	{
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "removeItem")
	{
		if(!pPlayer->CostSocialItem(req->sku(), 1))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "applyCrafting")// 制作炸弹和落石
	{
		string sku = "";
		if(req->sku() == "1")//洲际导弹
			sku = "7000";
		else
			sku = "7001";
		if(!pPlayer->AddSocialItem(sku, 1))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_ApplyCraftingFailed,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "applyCollectable")// 收藏里兑换物品
	{
		CFG_Collectables* Tbl = CollectablesTblInst::instance().GetItem(req->sku());
		string itemSkuList = Tbl->itemSkuList;
		itemSkuList.erase(0, 1);
		itemSkuList.erase(itemSkuList.size() - 1, 1);
		vector<string> SkuList = split(itemSkuList,",");
		for(int i = 0;i<SkuList.size();i++)
		{
			string Sku = SkuList[i];
			if(!pPlayer->CostSocialItem(Sku, 1))
			{
				SendRet2User(pUser,RseUpdateSocialItemRet_SocialItemNotEnough,rse);
				return;
			}
		}
		
		if(!pPlayer->AddSocialItem(Tbl->reward, 1))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_AddSocialItemFailed,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "addItemToWishList")
	{
		if(!pPlayer->AddItemToWishList(req->sku()))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_AddToWishListFailed,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "removeItemFromWishList")
	{
		if(!pPlayer->RemoveItemFromWishList(req->sku()))
		{
			SendRet2User(pUser,RseUpdateSocialItemRet_RemoveItemFromWishListFailed,rse);
			return;
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateSocialItemRet_Success,rse);
	}
	else if(strAction == "sendItemToNeighborWishList")
	{

	}

}


vector<string> RceUpdateSocialItemHandle::split( string str, string pattern)
{
	vector<string> ret;
	if(pattern.empty()) return ret;
	size_t start=0,index=str.find_first_of(pattern,0);
	while(index!=str.npos)
	{
		if(start!=index)
			ret.push_back(str.substr(start,index-start));
		start=index+1;
		index=str.find_first_of(pattern,start);
	}
	if(!str.substr(start).empty())
		ret.push_back(str.substr(start));
	return ret;
}