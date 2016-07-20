#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
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
	if(!pUserManager){
		return;
	}
	User *pUser = pUserManager->getUser(uid);
	if ( !pUser){
		return;
	}

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer /*|| !pPlayer->CanUse()*/){
		return ;
	}

	RceUpdateMisc* req = e->mutable_ce_rceupdatemisc();
	if( !req ){
		return;
	}
	
	int nPlanetId = req->planetid();
	int wk = req->wk();
	string strAction = req->action();
	if("ckDr" == strAction){
		
	}else if("visitHelpDone" == strAction){

	}else if("bunkerHelpDone" == strAction){
		
	}else if("firstLoadingSuccess" == strAction){

	}else if("coopMission_advanceProgress" == strAction){
		
	}else if("spyCapsuleForFree" == strAction){
		
	}else if("spyCapsuleBought" == strAction){
		
	}
}
