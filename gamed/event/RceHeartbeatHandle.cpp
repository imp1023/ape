#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/GameConstantSetCfg.h"
#include "RceHeartbeatHandle.h"
#include "RseHeartbeat.pb.h"
#include "RseError.pb.h"

RceHeartbeatHandle::RceHeartbeatHandle()
:CBaseEvent()
{

}
RceHeartbeatHandle::~RceHeartbeatHandle()
{

}

void RceHeartbeatHandle::handle(Event* e)
{
	if(!e->has_ce_rceheartbeat())
	{
		return;
	}
	int64 uid = e->uid();
	GameDataHandler* dh = eh_->getDataHandler();
	User* user = dh->getUser(uid);
	if(user == NULL)
	{
		return;
	}

	ConstantSetUnit* pUnit = GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_DEF_FANGCHENMI);

	if ( pUnit == NULL || pUnit->m_nArgs[0] == 0 )
	{

		if(user->GetResFlag()!=0)
		{
			user->SetResFlag(0);

			RseError rseErr;
			rseErr.set_errorid(user->GetResFlag());
			string text1;
			rseErr.SerializeToString(&text1);
			eh_->sendDataToUser(user->fd(), S2C_RseError, text1);
		}
	}
	else
	{
		int n3Hour	= pUnit->m_nArgs[1] * 60;
		int n5Hour	= pUnit->m_nArgs[3] * 60;

		// 	if(user->GetOnlineTime()<n6Hour&&user->GetResFlag()!=103)
		// 	{
		//	user->AddOnlineTime(time(NULL));
		//	}
		// 	if(user->GetOnlineTime()>=n6Hour)
		// 	{
		// 		if(user->GetResFlag()!=103)
		// 		{
		// 			user->SetResFlag(103);
		// 
		// 			RseError rseErr;
		// 			rseErr.set_errorid(user->GetResFlag());
		// 			string text1;
		// 			rseErr.SerializeToString(&text1);
		// 			eh_->sendDataToUser(user->fd(), S2C_RseError, text1);
		// 		}
		// 
		// 		//ÌßµôÍæ¼Ò
		// 		//eh_->removeUserFdMap(user->fd(), user->GetUid());
		// 	}
		user->AddOnlineTime(time(NULL));
		if(user->GetOnlineTime()>=n5Hour)
		{
			if(user->GetResFlag()!=102)
			{
				user->SetResFlag(102);

				RseError rseErr;
				rseErr.set_errorid(user->GetResFlag());
				string text1;
				rseErr.SerializeToString(&text1);
				eh_->sendDataToUser(user->fd(), S2C_RseError, text1);
			}
		}
		else if(user->GetOnlineTime()>=n3Hour)
		{
			if(user->GetResFlag()!=101)
			{
				user->SetResFlag(101);

				RseError rseErr;
				rseErr.set_errorid(user->GetResFlag());
				string text1;
				rseErr.SerializeToString(&text1);
				eh_->sendDataToUser(user->fd(), S2C_RseError, text1);
			}
		}
	}

	RseHeartbeat hb;
	string text;
	hb.SerializeToString(&text);
	eh_->sendDataToUser(user->fd(), S2C_RseHeartbeat, text ,false);
}
