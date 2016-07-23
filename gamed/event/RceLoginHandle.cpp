#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "RceLoginHandle.h"
#include "RseLogin.pb.h"

RceLoginHandle::RceLoginHandle()
:CBaseEvent()
{

}
RceLoginHandle::~RceLoginHandle()
{

}

void RceLoginHandle::handle(Event* e)
{

#if 0
	"currentTimeMillis": 1468377227787,
		"id": 0,
		"levelBasedOnScore": 1,
		"myAccountIsLocked": 0,
		"pop": false,
		"rqId": 326,
		"sync": 0,
		"timeFromLastLogin": 0,
		"timeFromLastUpdate": 0,
		"token": 739379,
		"userId": 1048,
		"version": "0.42.5",
		"vip": 0
#endif

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

	RseLogin rse;
	rse.set_currenttimemillis(time(NULL));
	rse.set_id(0);
	rse.set_levelbasedonscore(1);
	rse.set_myaccountislocked(0);
 	rse.set_pop(false);
	rse.set_rqid(0);
 	rse.set_sync(0);
 	rse.set_timefromlastlogin(0);
 	rse.set_timefromlastupdate(0);
 	rse.set_token(0);
 	rse.set_userid(toString(uid));
	rse.set_version("1");
 	rse.set_vip(0);
 
 	string text;
 	rse.SerializeToString(&text);
 	eh_->sendDataToUser(pUser->fd(), S2C_RseLogin, text);
}
