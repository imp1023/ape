#include "UserAuth.h"
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../../common/LogDef.h"
#include "../../common/Clock.h"

void UserAuth::addAuthEvent(EventQueue* eq, int64 uid, int64 secret, int fd)
{
	Event* e=eq->allocateEvent();
	e->set_cmd(EVENT_USER_AUTH);
	e->set_state(Game_to_Self);
	e->set_time(-2);
	UserAuthData* req = e->mutable_userauthdata();
	req->set_uid(uid);
	req->set_secret(secret);
	req->set_sockfd(fd);
	eq->safePushEvent(e);
}

void UserAuth::handle(Event* e)
{
    if (e->state() != Game_to_Self)
        return;

    if (e->cmd() == EVENT_USER_AUTH)
    {
        handle_auth(e);
    }
    else if (e->cmd() == EVENT_USER_DISCONNECT)
    {
        handle_disconnect(e);
    }
}

void UserAuth::handle_auth(Event* e)
{
    if (!e->has_userauthdata())
        return;

    const UserAuthData& req = e->userauthdata();

    GameDataHandler* dh = eh_->getDataHandler();
    if (! dh)
        return;

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

	
//     DB_BanLogin* login = user->GetDbUser().mutable_banlogin();
//     int64 delay = time(NULL) - login->time();
//     if (login->banlogin() && delay >= 0)
//     {
//         login->set_banlogin(false);
// 		dh->markUserDirty(user);
//     }
// 
//     if (delay < 0)
//     {
//         leftMin = 1 - (int)(delay / 60);
//     }
// 
//     bool pass2 = !login->banlogin();
//     if (!pass2)
//     {
//         type = 2;
//     }
// 
//     bool pass = pass1 && pass2;
// 
//     if (pass)
    {
        if(user->fd() != 0 && user->fd() != fd)
        {
            LOG4CXX_ERROR(logger_, "KickSelf: " << uid << ",oldfd: " << user->fd() << ",newfd: " << fd);
            eh_->removeUserFdMap(user->fd(), uid);
        }
        eh_->createUserFdMap(fd, uid);
        user->setFd(fd);
        user->setOnline(true);
        user->OnAuth();
		dh->markUserDirty(user);
    }

    RseAuthState state;
    state.set_pass(true);
    state.set_type(type);
    state.set_leftmin(leftMin);
	state.set_time(int(time(NULL)));
	state.set_zone(Clock::getTimeZone());
	string text;
	state.SerializeToString(&text);
	eh_->sendDataToUser(fd, S2C_RseAuthState, text);

// 	if (pass)
// 	{
// 		user->GetPlayer()->SendFuncSwitch();
//         user->GetPlayer()->RequestChallengeLoginData();
// 		user->GetPlayer()->SendStartServerActivity();
// 		SYS_UserStat(user, LT_LogOn, "", "", "", "", "", "", "", "");
// 	}
}

void UserAuth::handle_disconnect(Event* e)
{
    int64 uid = e->uid();

    GameDataHandler* dh = eh_->getDataHandler();
    if ( !dh )
        return;

    User* user = dh->getUser(uid, NULL, false);
    if ( !user )
        return;

	if (e->userleave().fd() != user->fd())
	{
		LOG4CXX_ERROR(logger_, "handle_disconnect userFd: " << user->fd() <<", "<< e << "\n" << e->DebugString());
		return;
	}

    user->setOnline(false);
	//user->GetPlayer()->GetGuildMgr()->UpdateScore(false);
    user->setFd(0);
    //user->OnDisconnect();
	user->setLastLogoutTime(time(NULL));
    dh->markUserDirty(user);
	double onlineTime =	difftime(user->lastLogoutTime(), user->lastLoginTime());

	//SYS_UserStat(user, LT_LogOut, onlineTime, user->GetPlayer()->CalcPower(), "", "", "", "", "", "");
	//if (user->GetDbUser().totalbuy() > 0)
	//	SYS_UserStat(user, LT_PAYUSER_LOGOUT, onlineTime, "", "", "", "", "", "", "");
}

