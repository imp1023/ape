#include "UserLogin.h"
#include "../WorldNetHandler.h"
#include "../WorldDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"


UserLogin* UserLogin::instance_ = NULL;

void UserLogin::addEvent(EventQueue* eq, int fd, int siteid, string const& platid,
						 string const& name, string const& head, int gender, vector<string>& friends,
						 int isYellowDmd, int isYellowDmdYear, int levelYellowDmd, string sid,int nCity,
						 string strVIA,int region,bool isRecall,bool isHighVip,bool isApp51Act,int nCustomFlag,
						 string strTaskMarketId,string strChannel,int nBackFlowFlag,bool isSuperVip,unsigned int nBlueTime,unsigned int nBlueYearTime,unsigned int nHighBlueTime,unsigned int nHighBlueYearTime)
{
	Event* e=eq->allocateEvent();
	e->set_cmd(EVENT_USER_LOGIN);
	e->set_time(-1);
	e->set_state(0);
	UserLogin_Req* req = e->mutable_userlogin_req();
	req->set_gettime(1);
	req->set_webfd(fd);
	req->set_siteid(siteid);
	req->set_platform_id(platid);
	req->set_name(name);
	req->set_profile_link(head);
	req->set_gender(gender);
	req->set_city(nCity);
	req->set_via(strVIA);
	for(size_t i = 0; i < friends.size(); i++)
	{
		if(friends[i].length() > 1 && friends[i] != platid)
		{
			req->add_friends_platid(friends[i]);
		}
	}
	req->set_is_yellow_dmd(isYellowDmd != 0);
	req->set_is_yellow_dmd_year(isYellowDmdYear != 0);
	req->set_yellow_dmd_lv(levelYellowDmd);
	req->set_isrecall(isRecall);
	req->set_is_high_yellow_dmd(isHighVip);
	req->set_isapp51act(isApp51Act);
	req->set_sid(sid);
	req->set_region(region);
	req->set_customflag( nCustomFlag );
	req->set_taskmarketid(strTaskMarketId);
    req->set_nbackflowflag(nBackFlowFlag);
	//∑÷Ω‚channel
	if(strChannel.size()>0)
	{
		TokenizeChannel(strChannel,req);
	}
	req->set_is_high_dmd_year(isSuperVip);
	req->set_bluetime(nBlueTime);
	req->set_blueyeartime(nBlueYearTime);
	req->set_highbluetime(nHighBlueTime);
	req->set_highblueyeartime(nHighBlueYearTime);
	
	eq->safePushEvent(e);
}

void UserLogin::TokenizeChannel(string& strChannel,UserLogin_Req* req)
{
	if(req==NULL)
		return;
	vector<string> extrasMsg;
	string delims = ":";
	int int_value = 0;
	tokenize(strChannel, extrasMsg, delims);
	if(extrasMsg.size()>=2 && strcmp( extrasMsg[0].c_str(), "channel" ) == 0)
	{
		WebCrossPromotion* pPromotion = req->mutable_promotion();
		pPromotion->set_chanle(extrasMsg[1]);
		if(extrasMsg.size()>=8)
		{
			pPromotion->set_date(extrasMsg[2]);
			int nCnt = atoi(extrasMsg[3].c_str());
			for(int i=0;i<nCnt&&i<4;i++)
			{
				pPromotion->add_cdk(extrasMsg[4+i]);
			}
		}
	}
}

void UserLogin::handle(Event* e)
{
	if (e->state() == UserLogin_WW_Req) // request from web
	{
		handle_WW_Req(e);
	}
	else if (e->state() == UserLogin_GW_Rsp) // response from game
	{
		handle_GW_Rsp(e);
	}
	else
	{
		LOG4CXX_ERROR(logger_, "Invalid Event.\n"<<e->DebugString());
	}
}

void UserLogin::handle_WW_Req(Event* e)
{
	if(!e->has_userlogin_req())
	{
		return;
	}
	UserLogin_Req* req = e->mutable_userlogin_req();
	WorldDataHandler* dh = eh_->getDataHandler();
	req->set_gettime(eh_->getReversion());
	//const UserLogin_Req& req = e->userlogin_req();
	const string& platid = req->platform_id();
	LOG4CXX_INFO(logger_, "Web request UserLogin with openid: "<<platid);
	int gid = dh->getGameIdfromPlatId(platid,req->region());
	e->set_state(UserLogin_WG_Req);
	eh_->sendEventToGamed(e, gid);
}

void UserLogin::handle_GW_Rsp(Event* e)
{
	if(!e->has_userlogin_rsp())
	{
		return;
	}
	const UserLogin_Rsp& rsp = e->userlogin_rsp();
	int64 uid = rsp.uid();
	const string& secret = rsp.secret();
	const string& server = rsp.server();
	int port = rsp.port();
	int port1 = rsp.port1();
	int webfd = rsp.webfd();
	string sid = rsp.sid();
	string strMainLoad = rsp.mainload();
	string strVersion = rsp.version();
	int nRegion = rsp.region();

	string strExtras;
	strExtras += "customflag:";
	strExtras += toString(rsp.customflag());

	Json::Value json(objectValue);
	buildUserLoginJson(uid, secret, server, port,port1, sid,nRegion,strMainLoad,strVersion,rsp.logintimes(),rsp.playerlvl(),rsp.isnewplayer(),strExtras,json);
	eh_->sendFdString(webfd, getJsonStr(json));

	LOG4CXX_INFO(logger_, "Game response UserLogin with uid: "<<uid);

	time_t ltCost = eh_->getReversion() - rsp.gettime();
	if(ltCost>=4000)
	{//≥¨π˝4√Î÷”
		LOG4CXX_ERROR(logger_, "Logon Time Out....platid:"<<rsp.platform_id()<<",uid:"<<uid<<",time:"<<ltCost);	
	}
}

void UserLogin::buildUserLoginJson(int64 uid, const string& secret, const string& server,
								   int port, int port1, string sid, int nRegion,const string& strMainLoad, 
								   const string& strVersion, int nlogintimes,int nlvl,bool bIsNewPlayer,string strExtras,Json::Value& json)
{
	json["id"] = toString(uid);
	json["secret"] = toString(secret);
	json["server"] = server;
	json["port"] = port;
	json["port1"] = port1;
	json["sid"] = sid;
	json["mainload"] = strMainLoad;
	json["version"] = strVersion;
	json["region"] = nRegion;
	json["logintimes"] = nlogintimes;
	json["playerlvl"] = nlvl;
	json["isnewplayer"] = (bIsNewPlayer == true)?1:0;
	json["extras"] = strExtras;
}
