#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceUpdateMiscHandle.h"
#include "RseUpdateMisc.pb.h"

//ÔÓÏîÂß¼­
RceUpdateMiscHandle::RceUpdateMiscHandle()
:CBaseEvent()
{

}
RceUpdateMiscHandle::~RceUpdateMiscHandle()
{

}

void RceUpdateMiscHandle::handle(Event* e)
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

	RceUpdateMisc* req = e->mutable_ce_rceupdatemisc();
	if( !req )
	{
		return;
	}

	RseUpdateMisc rse;
	rse.set_ret(RseUpdateMiscRet_Success);

	int nPlanetId = req->planetid();
	DB_Planet *pPlanet = pPlayer->GetPlanet(nPlanetId);
	if(!pPlanet){
		SendRet2User(pUser,RseUpdateMiscRet_CantFindPlanet,rse);
		return;	
	}

	string strAction = req->action();
	if("ckDr" == strAction){
		if(!pPlayer->CheckDroids(pPlanet, req->wk())){
			SendRet2User(pUser,RseUpdateMiscRet_DroidCntError,rse);
			return;	
		}
		pPlayer->SetDroidInUse(pPlanet, req->wk());
		eh_->getDataHandler()->markUserDirty(pUser);
		string text;
		rse.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateMisc,text);
	}else if("visitHelpDone" == strAction){

	}else if("bunkerHelpDone" == strAction){
		
	}else if("firstLoadingSuccess" == strAction){

	}else if("coopMission_advanceProgress" == strAction){
		
	}else if("spyCapsuleForFree" == strAction){
		
	}else if("spyCapsuleBought" == strAction){
		
	}
}

void RceUpdateMiscHandle::SendRet2User(User* pUser,int nRet,RseUpdateMisc& rse)
{
	rse.set_ret(nRet);
	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateMisc,text);
}