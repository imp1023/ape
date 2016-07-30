#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceUpdateProfileHandle.h"


RceUpdateProfileHandle::RceUpdateProfileHandle()
:CBaseEvent()
{

}
RceUpdateProfileHandle::~RceUpdateProfileHandle()
{

}

void RceUpdateProfileHandle::handle(Event* e)
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

	RceUpdateProfile* req = e->mutable_ce_rceupdateprofile();
	if( !req )
	{
		return;
	}

	RseUpdateProfile rse;
	rse.set_ret(0);
	string strAction = req->action();
	int planetId = req->planetid();
	DB_Planet* planet = pPlayer->GetPlanet(planetId);
	if(!planet)
	{
		return;
	}
	if(strAction == "tutorialCompleted")
	{
		
	}
	else if(strAction == "exchangeCashToCoins")
	{

	}
	else if(strAction == "exchangeCashToMinerals")
	{

	}
	else if(strAction == "buyCash")
	{

	}
	else if(strAction == "buyDroid")
	{
		MsgTransaction* MT = req->mutable_transaction();
		for(int i = 0;i>MT->socialitems_size();i++)
		{
			MsgSocialItems* socialItem = MT->mutable_socialitems(i);
			string sku = socialItem->sku();
			int cnt = socialItem->amount();
			if(!pPlayer->CostSocialItem(sku,cnt))
			{
				SendRet2User(pUser,2,rse);
				return ;
			}
		}
		
		planet->set_droids(planet->droids() + 1 );
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateProfileRet_Success,rse);
	}
	else if(strAction == "buyDamageProtectionTime")
	{

	}
	else if(strAction == "levelUp")
	{
		DB_Model* model = pPlayer->GetDBPlayer()->mutable_model();
		model->set_level(model->level() + 1);
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateProfileRet_Success,rse);
	}
	else if(strAction == "setFlag")
	{
		string key = req->key();
		DB_Flag* pDBFlag = pPlayer->GetDBPlayer()->mutable_flag();
		if("quality" == req->key())
			pDBFlag->set_quality(req->value());
		else if("music" == req->key())
			pDBFlag->set_music(req->value());
		else if("sound" == req->key())
			pDBFlag->set_effect(req->value());
		else
		{

		}
		eh_->getDataHandler()->markUserDirty(pUser);
		SendRet2User(pUser,RseUpdateProfileRet_Success,rse);
		
	}
	else
	{

	}
	
}

void RceUpdateProfileHandle::SendRet2User(User* pUser,int nRet,RseUpdateProfile& rseUpdateProfile)
{
	rseUpdateProfile.set_ret(nRet);
	string text;
	rseUpdateProfile.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateProfile,text);
}