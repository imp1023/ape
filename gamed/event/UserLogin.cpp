#include "UserLogin.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../event/EventQueue.h"
#include "../../common/DateTime.h"
#include "../../common/error_def.h"
#include "../../common/SysLog.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"

UserLogin* UserLogin::instance_ = NULL;

void UserLogin::handle(Event* e)
{
    if (e->state() != Web_To_Game)
        return;

    if (e->cmd() == EVENT_USER_REGIST)
    {
        handle_regist(e);
    }
    else if (e->cmd() == EVENT_USER_LOGIN)
    {
        handle_login(e);
    }
}

void UserLogin::handle_regist(Event* e)
{
    Forward* forward = e->mutable_forward();
    if (!forward)
        return;
    if (!e->has_content())
        return;
    LOG4CXX_INFO(logger_, "Handle Regist: " << e->content());

    //reg,openid,sid
    string regstr = e->content();
    vector<string> regtoken;
    static const string delim = ",";
    tokenize(regstr, regtoken, delim);

    string openid, sid;
    if (regtoken.size() == 3)
    {
        openid = regtoken[1];
        sid = regtoken[2];
    }

    LoadStatus loadStatus = LOAD_INVALID;
    GameDataHandler* dh = eh_->getDataHandler();
    User *user = dh->getUser(openid, &loadStatus, true);
    if (user == NULL && loadStatus == LOAD_WAITING)
    {
        eh_->postBackEvent(e);
        return;
    }

    string response;
    do 
    {
        if (user != NULL)
        {
            response = errorContent("Already Registered", ERR_ALREADY_REGIST, sid);
            break;
        }

        if (loadStatus != LOAD_EMPTY)
        {
            response = errorContent("Load Failed", ERR_LOADUSER_FAILED, sid);
            break;
        }

        if (!dh->isPlatidLocal(openid))
        {
            response = errorContent("OpenId Not Local", ERR_OPENID_NOT_LOCAL, sid);
            break;
        }

        user = dh->createUser(openid);
        if (!user)
        {
            response = errorContent("Create Failed", ERR_CREATEUSER_FAILED, sid);
            break;
        }

        user->OnLogin();
        response = succContent(user, sid, 1);
        LOG4CXX_INFO(logger_, "UserLogin[u" << user->uid() << "]");
		SYS_UserStat(user, LT_Register, openid.c_str(), "", "", "", "", "", "", "");
    } while (0);

    e->set_state(Game_to_Web);
    e->set_content(response);
    int webfd = forward->serverfdfrom();
    forward->set_serverfdto(webfd);
    eh_->sendEventToWorld(e);
    LOG4CXX_INFO(logger_, "Response Regist: " << e->content());
}

void UserLogin::handle_login(Event* e)
{
    Forward* forward = e->mutable_forward();
    if (!forward)
        return;
    if (!e->has_content())
        return;
    LOG4CXX_INFO(logger_, "Handle Login: " << e->content());

    // ul,openid,channel,bind,from,sid
    string ulstr = e->content();
    vector<string> ultoken;
    static const string delim = ",";
    tokenize(ulstr, ultoken, delim);

	int channel = -1;
	int gender = 0;
	int isBlueDiamond = 0;
	int isYearBlueDiamond = 0;
	int blueDiamondLvl  = 0;
	int friendNum = 0;
	int region = 0;
	int city = 0;

    string openid = ultoken[1];
	safe_atoi(ultoken[2], channel);
	string name = ultoken[3];
	string avatar = ultoken[4];
	safe_atoi(ultoken[5], gender);
	safe_atoi(ultoken[6], isBlueDiamond);
	safe_atoi(ultoken[7], isYearBlueDiamond);
	safe_atoi(ultoken[8], blueDiamondLvl);
	safe_atoi(ultoken[9], friendNum);
	string friendLst = ultoken[10];
	safe_atoi(ultoken[11], region);
	safe_atoi(ultoken[12], city);
	string via = ultoken[13];
	string expand = ultoken[14];
	string sid = ultoken[15];
	//string bind = ultoken[3];
	string from = "";//ultoken[4];

    string response;
    LoadStatus loadStatus = LOAD_INVALID;
    GameDataHandler* dh = eh_->getDataHandler();
    User *user = dh->getUser(openid, &loadStatus, true);
    if (user == NULL && loadStatus == LOAD_WAITING)
    {
        eh_->postBackEvent(e);
        return;
    }

    if (user == NULL)
    {
		do 
		{
            if (loadStatus != LOAD_EMPTY)
			{
				response = errorContent("Load Failed", ERR_LOADUSER_FAILED, sid);
				break;
			}

			if (!dh->isPlatidLocal(openid)) 
			{
				response = errorContent("OpenId Not Local", ERR_OPENID_NOT_LOCAL, sid);
				break;
			}

			user = dh->createUser(openid);
			if (!user)
			{
				response = errorContent("Create Failed", ERR_CREATEUSER_FAILED, sid);
				break;
			}

			//bool nBind = atoi(bind.c_str()) ? true : false;
			//user->setChannel(channel);
			//user->setFrom(from);
			//user->GetDbUser().set_accbind(false);
			user->OnLogin();
			response = succContent(user, sid, 1);
			SYS_UserStat(user, LT_Register, openid.c_str(), "", "", "", "", "", "", "");
			LOG4CXX_INFO(logger_, "UserLogin[u" << user->uid() <<"]");
		} while (0);
    }
    else
    {
		//bool nBind = atoi(bind.c_str()) ? true : false;
		//user->GetDbUser().set_accbind(false);
		//if (user->getChannel() == 0)
		//	user->setChannel(channel);
		//if (user->getFrom().size() == 0)
		//	user->setFrom(from);
        user->OnLogin();
        response = succContent(user, sid, 0);
        LOG4CXX_INFO(logger_, "UserLogin[u" << user->uid() << "]");
    }

    e->set_state(Game_to_Web);
    e->set_content(response);
    int webfd = forward->serverfdfrom();
    forward->set_serverfdto(webfd);
    eh_->sendEventToWorld(e);
	dh->markUserDirty(user);
    LOG4CXX_INFO(logger_, "Response Login: " << response);
}

string UserLogin::succContent(User* user, const string& sid, int isnew)
{
    if (!user)
        return "";
    ptree pt;
    pt.put("id", user->uid());
    pt.put("secret", user->secret(eh_->getReversion()));
	std::string name = base64_encode(user->getName());
	pt.put("name", name.c_str());
	pt.put("level", user->GetUserLevel());
    pt.put("server", eh_->getServerIp());
    pt.put("port", eh_->getServerPort());
	pt.put("isnew", isnew);
    pt.put("sid", sid);

    stringstream ssm;
    write_json(ssm, pt, false);
    return ssm.str();
}

string UserLogin::errorContent(const string& msg, int errorCode, const string& sid)
{
    ptree pt;
    pt.put("msg", msg);
    pt.put("errorCode", errorCode);
    pt.put("sid", sid);

    stringstream ssm;
    write_json(ssm, pt, false);
    return ssm.str();
}

