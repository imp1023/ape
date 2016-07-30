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
	rse.set_ret(0);
	
	int nPlanetId = req->planetid();
	string strAction = req->action();
	MsgTransaction* MT = req->mutable_transaction();
	if(MT)
	{
		for(int i = 0;i<MT->socialitems_size();i++)
		{
			MsgSocialItems* MSI = MT->mutable_socialitems(i);
			DB_SocialItem* pDBSI = pPlayer->GetSocialItem(MSI->sku());
			pDBSI->set_amount(pDBSI->amount() - MSI->amount());
			if(pDBSI->amount() == 0)
				pPlayer->RemoveSocialItem(MSI->sku());
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
		DB_SocialItem* pDBSI = pPlayer->GetSocialItem(req->sku());
		if(!pDBSI)
		{
			pDBSI = pPlayer->GetDBPlayer()->add_socialitems();
			pDBSI->set_sku(req->sku());
			pDBSI->set_counter(req->currentcount());
			pDBSI->set_amount(quantity);
			if(tm == 0)
			{
				pDBSI->set_timeleft(0);
				pDBSI->set_timeover(0);
			}
			else
			{
				pDBSI->set_timeleft(tm);
				pDBSI->set_timeover(time(NULL));
			}
		}
		else
		{
			if(pDBSI->timeleft() == 0 || tm <= time(NULL) - pDBSI->timeover())
			{
				pDBSI->set_amount(pDBSI->amount() + quantity);
				pDBSI->set_timeleft(tm);
				pDBSI->set_counter(req->currentcount());
				pDBSI->set_timeover(time(NULL));
			}
		}
		SendRet2User(pUser,0,rse);
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
				SendRet2User(pUser,1,rse);
				return ;
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
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
			SendRet2User(pUser,1,rse);
			return;
		}
		if(Sku == 33)//雇佣工人
		{
			DB_Planet* pPlanet = pPlayer->GetPlanet(1);
			pPlanet->set_droids(pPlanet->droids() + 1);
		}
		if(Sku >= 5000 && Sku <= 5006)
		{
			switch (Sku) {
			case 5000:// 金币×1000
				pPlayer->CostRes(RC_Coin,1000);
				break;
			case 5002:// 矿石×1000
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
			pDBSI->set_amount(pDBSI->amount() - 1);
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
					model->set_coins(model->coins() + addCoins);
				}
				else if(seed == 2)
				{
					int addMinerals = model->minerals() * 3 / 100;
					if (addMinerals < 1000) {
						addMinerals = 1000;
					}
					model->set_minerals(model->minerals() + addMinerals);
				}
			}
			else
			{
				DB_SocialItem* pDBReward = pPlayer->GetSocialItem(randomSku);//req->num()空缺
				if(!pDBReward)
				{
					pDBReward = pPlayer->GetDBPlayer()->add_socialitems();
					pDBReward->set_sku(randomSku);
					pDBReward->set_counter(0);
					pDBReward->set_timeleft(0);
					pDBReward->set_timeover(0);
					pDBReward->set_amount(1);
				}
				else
					pDBReward->set_amount(pDBReward->amount() + 1);
			}
			pDBSI->set_counter(nextCounter);
			pDBSI->set_amount(pDBSI->amount() - 1);

		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
	}
	else if(strAction == "buyItem")
	{
		SendRet2User(pUser,0,rse);
	}
	else if(strAction == "removeItem")
	{
		DB_SocialItem* pDBSI = pPlayer->GetSocialItem(req->sku());
		if(!pDBSI)
		{
			SendRet2User(pUser,1,rse);
			return;
		}
		pDBSI->set_amount(pDBSI->amount() - 1);
	}
	else if(strAction == "applyCrafting")// 制作炸弹和落石
	{
		string sku = "";
		if(req->sku() == "1")//洲际导弹
			sku = "7000";
		else
			sku = "7001";
		DB_SocialItem* pDBSI = pPlayer->GetSocialItem(sku);
		if(!pDBSI)
		{
			pDBSI = pPlayer->GetDBPlayer()->add_socialitems();
			pDBSI->set_sku(sku);
			pDBSI->set_amount(1);
			pDBSI->set_counter(0);
			pDBSI->set_timeleft(0);
			pDBSI->set_timeover(0);
		}
		else
		{
			pDBSI->set_amount(pDBSI->amount() + 1);
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
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
			DB_SocialItem* pDBSIS = pPlayer->GetSocialItem(Sku);
			if(!pDBSIS)
			{
				SendRet2User(pUser,1,rse);
				return;
			}
			pDBSIS->set_amount(pDBSIS->amount() - 1);
		}

		DB_SocialItem* pDBSI = pPlayer->GetSocialItem(Tbl->reward);
		if(!pDBSI)
		{
			pDBSI = pPlayer->GetDBPlayer()->add_socialitems();
			pDBSI->set_sku(Tbl->reward);
			pDBSI->set_amount(1);
			pDBSI->set_counter(0);
			pDBSI->set_timeleft(0);
			pDBSI->set_timeover(0);
		}
		else
		{
			pDBSI->set_amount(pDBSI->amount() + 1);
		}
		SendRet2User(pUser,0,rse);
	}
	else if(strAction == "addItemToWishList")
	{
		string sku = req->sku();
		DB_WishItemList* pDBWIL = pPlayer->GetDBPlayer()->mutable_wishitemlist();
		DB_WishItem* pDBWI = NULL;
		for(int i = 0;i<pDBWIL->wishitem_size();i++)
		{
			DB_WishItem* pDBTmp= pDBWIL->mutable_wishitem(i);
			if(pDBTmp->sku() == "")
				pDBWI = pDBTmp;
		}
		if(!pDBWI)
		{
			pDBWI = pDBWIL->add_wishitem();
		}
		pDBWI->set_sku(sku);
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
	}
	else if(strAction == "removeItemFromWishList")
	{
		string sku = req->sku();
		DB_WishItemList* pDBWIL = pPlayer->GetDBPlayer()->mutable_wishitemlist();
		for(int i = 0;i<pDBWIL->wishitem_size();i++)
		{
			DB_WishItem* pDBTmp= pDBWIL->mutable_wishitem(i);
			if(pDBTmp->sku() == sku)
			{
				/*DB_WishItem* pDBWI_last = pDBWIL->mutable_wishitem(pDBWIL->wishitem_size() - 1);
				if(pDBWI_last && pDBWI_last != pDBTmp)
				{
					pDBWI_last->Swap(pDBTmp);
				}
				pDBWIL->mutable_wishitem()->RemoveLast();*/
				pDBTmp->set_sku("");
			}
		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,0,rse);
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