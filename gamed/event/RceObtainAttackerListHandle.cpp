#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "RceObtainAttackerListHandle.h"
#include "RseObtainAttackerList.pb.h"

RceObtainAttackerListHandle::RceObtainAttackerListHandle()
:CBaseEvent()
{

}
RceObtainAttackerListHandle::~RceObtainAttackerListHandle()
{

}

void RceObtainAttackerListHandle::handle(Event* e)
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

	DB_Player *pDBPlayer = pPlayer->GetDBPlayer();
	if(!pDBPlayer){
		return;
	}
	
	RseObtainAttackerList rse;
	rse.set_damageprotectionappliedat(time(NULL));
	int replayIdx = 0;
	for(int i = 0; i < pDBPlayer->battlereplay_size(); i++){
		DB_BattleReplay *pDBBattleReplay = pDBPlayer->mutable_battlereplay(i);
		MsgAttacker *pMsgAttacker = rse.add_attackerlist();
		if(pMsgAttacker && pDBBattleReplay){
			pMsgAttacker->set_id(replayIdx++);
			pMsgAttacker->set_attackdate(pDBBattleReplay->time());
			pMsgAttacker->set_accountid(toString(pDBBattleReplay->accountid()));
			pMsgAttacker->set_planetsku(pDBBattleReplay->starsku());
			pMsgAttacker->set_damage(pDBBattleReplay->damage());
			pMsgAttacker->set_planetdamage(pDBBattleReplay->planetdamage());
			pMsgAttacker->set_coinstaken(pDBBattleReplay->coinstaken());
			pMsgAttacker->set_mineralstaken(pDBBattleReplay->mineralstaken());
			pMsgAttacker->set_obtaineddamageprotectiontime(0);
			pMsgAttacker->set_name(pDBBattleReplay->name());
			pMsgAttacker->set_url(pDBBattleReplay->url());
		}
	}

	string text;
	rse.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseObtainAttackerList,text);
}
