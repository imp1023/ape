#include "WebServerHandler.h"
#include "../event/EventDefine.h"
#include "../event/EventQueue.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../common/ServerConfig.h"
#include "../common/Clock.h"
#include "../worldd/worldd.h"
#include "../worldd/WorldEventHandler.h"

using namespace std;

WebServerHandler::WebServerHandler(EventQueue *eq, int fd, int nid) :
eq_(eq), fd_(fd), nid_(nid)
{
    logger_ = log4cxx::Logger::getLogger("EventHelper");
}

WebServerHandler::~WebServerHandler(void)
{
}

int WebServerHandler::handlerType()
{
    return ProtocolHandler::WEBSERVER;
}

void WebServerHandler::handle(int64 uid, string &req)
{
    LOG4CXX_INFO(logger_, "Handle web message: " << req );
    
    vector<string> reqtoken;
    static const string delim = ",";
    tokenize(req, reqtoken, delim); 

    if (reqtoken.size() == 0)
        return;

    string& cmd = reqtoken[0];

    int serverId = 0;
    int nCmd = -1;

    if (cmd == "ul")
    {
        int region = distribution::getRegion(reqtoken[1]);
        serverId = ServerConfig::Instance().gameid(region);
        nCmd = EVENT_USER_LOGIN;
    }
    else if (cmd == "reg")
    {
        int region = distribution::getRegion(reqtoken[1]);
        serverId = ServerConfig::Instance().gameid(region);
        nCmd = EVENT_USER_REGIST;
    }
	else if (cmd == "bind")
	{
		int region = distribution::getRegion(reqtoken[1]);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_USER_BIND;
	}
	else if (cmd == "player_info")
	{
		int64 uid = 0;
		safe_atoll(reqtoken[1], uid);
		int region = distribution::getRegion(uid);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_PLAYER_INFO;		
	}
	else if (cmd == "recharge")
	{
		int64 uid = 0;
		safe_atoll(reqtoken[1], uid);
		int region = distribution::getRegion(uid);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_USER_RECHARGE;	
	}
	else if (cmd == "start")
	{
		safe_atoi(reqtoken[1], serverId);	
		nCmd = EVENT_STARTSERVER;	
	}
	else if (cmd == "luckygoddess")
	{
		int64 uid = 0;
		safe_atoll(reqtoken[1], uid);
		int region = distribution::getRegion(uid);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_USER_LUCKYGODDESS;
	}
	else if (cmd == "recharge_reward_check")
	{
		int64 uid = 0;
		safe_atoll(reqtoken[1], uid);
		int region = distribution::getRegion(uid);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_RECHARGE_REWARD_CHECK;
	}
	else if ( cmd == "InviterReward")
	{
		int64 uid = 0;
		safe_atoll(reqtoken[1], uid);
		int region = distribution::getRegion(uid);
		serverId = ServerConfig::Instance().gameid(region);
		nCmd = EVENT_USER_INVITERSREWARD;
	}
	else if (cmd == "centerbattleday")
	{
		int time = (int)GetTimeVal(reqtoken[1]);
		int nDay = Clock::getLocalDay(time);
		//CenterCampBattleModuleInst::instance().SetDate(nDay);
		World::Instance().GetWorldEventHandler()->sendFdString(fd_, "SetDay Success");
	}
    else if (cmd == "goddnesstest")
    {
        //LuckyGodHandleInst::instance().HandleLuckyGoddnessForChargeTest();
        return;
    }
    else if (cmd == "addchallengetime")
    {
        int count = 0;
        if (reqtoken.size() <= 1)
        {
            return;
        }
        safe_atoi(reqtoken[1],count);
        //ChallengeWorldModuleInst::instance().ProessGMRequest(count);
        return;
    }
	else if(cmd == "deletechallengesign")
	{
		int64 uid = StringToUid(reqtoken[1]);
		//ChallengeWorldModuleInst::instance().ProessGMDeleteSign(uid);
		return;
	}
	else if(cmd == "resetchallengegrade")
	{
		int64 uid = StringToUid(reqtoken[1]);
// 		bool suc = ChallengeWorldModuleInst::instance().ProcessResetChallengeGrade(uid);
// 		if (suc)
// 			World::Instance().GetWorldEventHandler()->sendFdString(fd_, "setchallengegrade success!");	
// 		else
// 			World::Instance().GetWorldEventHandler()->sendFdString(fd_, "setchallengegrade fail!");	
		return;
	}
    else if (reqtoken.size() > 1)
    {                                            
        serverId = getServerId(reqtoken[1]);
        nCmd = EVENT_GM;
    }

    if (serverId <= 0 || nCmd <= 0)
        return;

    Event* e = eq_->allocateEvent();
    e->set_cmd(nCmd);
    e->set_state(Web_To_Game);
    e->set_time(-1);
    e->set_uid(-1);
    Forward* forward = e->mutable_forward();
    forward->set_serveridto(serverId);
    forward->set_serverfdfrom(fd_);
    e->set_content(req);
    eq_->safePushEvent(e);
}

int WebServerHandler::getServerId(const string& query)
{
    if (query.length() < 1)
        return 0;

    char type = query[0];
    string value = query.substr(1, query.length()-1);

    if (type == 'u')
    {
        int region = distribution::getRegion(StringToUid(value));
        return ServerConfig::Instance().gameid(region);
    }
    else if (type == 'o')
    {
        int region = distribution::getRegion(value);
        return ServerConfig::Instance().gameid(region);
    }
    else if (type == 'r')
    {
        int region = 0;
        safe_atoi(value, region);
        return ServerConfig::Instance().gameid(region);
    }
    else if (type == 'g')
    {
        int serverid = 0;
        safe_atoi(value, serverid);
        return serverid;
    }
	else if (type == 'a')
	{
		return MAX_NUMBER;
	}
	else if (type == 'w')
	{
		return WORLD_SPECIAL;
	}

    return 0;
}
