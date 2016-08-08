#include "UserAuth.h"
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../event/EventQueue.h"
#include "../../event/RseLogin.pb.h"
#include "../../common/SysLog.h"
#include "../../common/Msg2QQ.h"

void UserAuth::addEvent(EventQueue* eq, int64 uid, int64 secret, int fd)
{
	Event* e=eq->allocateEvent();
	e->set_cmd(EVENT_USER_AUTH);
	e->set_state(UserAuth_CG_Req);
	e->set_time(-2);
	UserAuth_Req* req = e->mutable_userauth_req();
	req->set_uid(uid);
	req->set_secret(secret);
	req->set_sockfd(fd);
	eq->safePushEvent(e);
}

void UserAuth::handle(Event* e)
{
	if (e->state() == UserAuth_CG_Req)
	{
		if(!e->has_userauth_req())
		{
			return;
		}
		handle_CG_Req(e);
	}
	else
	{
		LOG4CXX_ERROR(logger_, "Invalid Event.\n"<<e->DebugString());
	}
}

void UserAuth::handle_CG_Req(Event* e)
{

	GameDataHandler* dh = eh_->getDataHandler();
	const UserAuth_Req& req = e->userauth_req();
	int64 uid = req.uid();
	int64 secret = req.secret();
	int fd = req.sockfd();
	
	if(uid <= 0)
		return;
	User *user = dh->getUser(uid);
	if(user == NULL)
	{
		return;
	}	

	int type = 0;
	int leftMin = 0;
	bool pass1 = user->checkSecret(secret, dh->Revision());
	if (!pass1)
	{
		type = 1;
	}

	if(eh_ && eh_->getNetHandler())
	{
		eh_->getNetHandler()->SetRc4Keys(fd,user->GetRc4Send(),user->GetRc4Receive());
	}

	bool pass3 = !(user->GetPlayer()->IsAttacked());

	DB_BanLogin* login = user->GetDBBanLogin();
	int64 delay = time(NULL) - login->time();
	if (login->banlogin() && delay >= 0)
	{
		login->set_banlogin(false);
		dh->markUserDirty(user);
	}

	if (delay < 0)
	{
		leftMin = 0 - (int)(delay / 60);
	}

	bool pass2 = !login->banlogin();
	if (!pass2)
	{
		type = 2;
	}

    bool pass5 = user->CheckPlayerAdult();
    if (!pass5)
    {
        type = 5;
    }

	if(!pass3){//被攻击中，再判断一次
#if 0
		BattleManager* pBtlMgr = user->GetBattleManager();
		if(pBtlMgr)
		{	
			const NormalBtlInfo* pBtlInfo = pBtlMgr->GetNormalBtlInfo();
			
			time_t ltTime = pBtlInfo->m_btlData.m_DBReport.attacktime();
			int64 nAtkID  =pBtlInfo->m_btlData.m_DBReport.attackid();
			time_t ltNow  = time(NULL);
			if(ltNow - ltTime > 420)
			{//超过7分钟，重新check一下，本来是5分钟，现在有问题改成7分钟
				eh_->PushEventCheckAttack(nAtkID,user->GetUid());
			}
		}
#endif
		type = 3;
	}

	bool pass = pass1 && pass2 && pass3;

	if (pass)
	{
		if(user->fd() != 0 && user->fd() != fd)
		{
			eh_->removeUserFdMap(user->fd(), uid);
		}
		eh_->createUserFdMap(fd, uid);
		user->setFd(fd);
		user->setOnline(true);
		user->Logon(dh);
		dh->PushOnlineUserID(user->GetUid());
#if 0
		BattleManager* pBtlMgr = user->GetBattleManager();
		if(pBtlMgr)
		{
			if(pBtlMgr->IsAttacking())
			{//正在攻击别人
				BtlType emAtkType = pBtlMgr->GetAtkBtlType();
				eh_->PushEventAttackFalse(user,emAtkType);
			}		
		}

		user->SetInBlackList(BlackListConfig::Instance().IsInBlackList(uid));
#endif
		dh->markUserDirty(user); 

#if 0
		user->GetResStat()->Clear();
		bool bStatUser = CSysLog::GetInstance()->IsStatUser(uid);

		bool bIsActivePlay = user->GetDbUser().isactivelogon();

		SYS_LOG(uid,LT_LogOn,0,0,bStatUser<<user->GetGender() <<user->GetUserLevel() 
			<<user->IsYellowDmd()<<user->IsYellowDmdYear()<<user->GetYellowDmdLvl()<<(int)user->friends_platid().size()
			<<user->GetUserRegion(false)<<user->GetUserRegion(true)
			<<user->GetPlayer()->GetResCnt(RC_Metal)<<user->GetPlayer()->GetResCnt(RC_Oil)<<user->GetPlayer()->GetCredit()<<user->GetPlayer()->GetHonor()<<user->GetPlatformId()<<user->IsHighYellowDmd()<<bIsActivePlay);
		int nGuildID = user->GetCountryManager()->GetDBGuild()->guildid();
		SYS_UserStat(user,false,"LogOn",user->GetPlatformId(),user->GetChannel(),"",user->GetPlattype(),(int)user->GetPlayer()->GetResCnt(RC_Metal),(int)user->GetPlayer()->GetResCnt(RC_Oil),(int)user->GetPlayer()->GetHonor(),user->GetCity(),nGuildID);
		int nTotalLV = user->GetPlayer()->CacuScienceTotalLV();
		/*if(nTotalLV>20)
		{
			SYS_UserStat4WebRank(user,"RankScience","",nTotalLV);
		}*/

		CMsg2QQ::GetInstance()->TellMsg(MQ_Logon,user,0,0,0);
#endif
	}

	RseAuthState state;
	state.set_pass(pass);
	state.set_type(type);
	state.set_leftmin(leftMin);
	string text;
	state.SerializeToString(&text);
	eh_->sendDataToUser(fd, S2C_RseAuthState, text);
}
