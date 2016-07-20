#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "WebServerHandler.h"
#include "../worldd/WorldNetHandler.h"
#include "../net/NetCache.h"
#include "../event/EventQueue.h"
#include "../logic/User.h"
#include "../common/json-util.h"
#include "../common/string-util.h"
#include "event/UserLogin.h"

using namespace std;

WebServerHandler::WebServerHandler(EventQueue *eq, int fd, int nid) :
eq_(eq), fd_(fd), nid_(nid)
{
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
	static string delims = ",";
	vector<string> tokens;
	tokenize(req, tokens, delims);
	string &cmd = tokens[0];
	if (checkCmd(cmd, "ul", tokens, USER_LOGIN_ARG_NUM))
	{
		processUserLogin(tokens);
	}
}

void WebServerHandler::processUserLogin(vector<string> &tokens)
{
	bool succ = true;
	string &platid = tokens[1];

	int siteid = 0;
	succ = succ && safe_atoi(tokens[2].c_str(), siteid);

	string &name = tokens[3];
	name = base64_decode(name);

	string &head = tokens[4];

	int gender = 0;
	succ = succ && safe_atoi(tokens[5].c_str(), gender);

	int i4IsYellowDmd = 0;
	succ = succ && safe_atoi(tokens[6].c_str(), i4IsYellowDmd);

	int i4IsYellowDmdYear = 0;
	succ = succ && safe_atoi(tokens[7].c_str(), i4IsYellowDmdYear);

	int i4YellowDmdLv = 0;
	succ = succ && safe_atoi(tokens[8].c_str(), i4YellowDmdLv);

	int fcnt = 0;
	succ = succ && safe_atoi(tokens[9].c_str(), fcnt);

	vector<string> friends;
	string delims = ";";
	tokenize(tokens[10], friends, delims);

	int region = 0;
	succ  = succ && safe_atoi(tokens[11].c_str(),region);


	int nCity=0;
	nCity = 0;
	safe_atoi(tokens[12].c_str(), nCity);

	string strVIA;
	strVIA = tokens[13];

	bool isRecall = false;
	bool isHighVip = false;
	bool isApp51Act = false;
	int nCustomFlag = 0;
    int nBackFlowFlag = 0;
	bool isSuperVip = false;//豪华蓝钻年费版,web的文档里是super vip
	int	nQHallDmdTime = 0;
	int nQHallDmdYearTime = 0;
	int nHighQHallDmdTime = 0;
	int nHighQHallDmdYearTime = 0;
	string strTaskMarketId;
	string strChannel;
	vector<string> lstCDK;
	vector<string> extras;
	delims = ";";
	tokenize(tokens[14], extras, delims);
	
	for ( int i = 0; i < extras.size(); i++ )
	{
		vector<string> extrasMsg;
		int int_value = 0;
		delims = ":";
		tokenize(extras[i], extrasMsg, delims);

		if ( extrasMsg.size() < 2 )
		{
			continue;
		}
		if ( strcmp( extrasMsg[0].c_str(), "recall" ) == 0 )
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str(), int_value);
			isRecall = ( int_value == 1 );
		}

		if ( strcmp( extrasMsg[0].c_str(), "isHighVip" ) == 0 )
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str(), int_value);
			isHighVip = ( int_value == 1 );
		}

		if ( strcmp( extrasMsg[0].c_str(), "isApp51Act" ) == 0 )
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str(), int_value);
			isApp51Act = ( int_value == 1 );
		}

		if ( strcmp( extrasMsg[0].c_str(), "customflag" ) == 0 )
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str(), int_value);
			nCustomFlag = int_value;
		}

		if ( strcmp( extrasMsg[0].c_str(), "taskMarketId" ) == 0 )
		{
			strTaskMarketId = extrasMsg[1];
		}

		if ( strcmp( extrasMsg[0].c_str(), "channel" ) == 0 )
		{
			strChannel = extras[i];
		}

        if ( strcmp(extrasMsg[0].c_str(), "backflow") == 0)
        {
            safe_atoi(extrasMsg[1].c_str(), nBackFlowFlag);
        }
		if (strcmp(extrasMsg[0].c_str() , "supervip") == 0)
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str() , int_value);
			isSuperVip = (int_value == 1);
		}
		if (strcmp(extrasMsg[0].c_str() , "bluetime") == 0)
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str() , nQHallDmdTime);
		}
		if (strcmp(extrasMsg[0].c_str() , "blueyeartime") == 0)
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str() , nQHallDmdYearTime);
		}
		if (strcmp(extrasMsg[0].c_str() , "highbluetime") == 0)
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str() , nHighQHallDmdTime);
		}
		if (strcmp(extrasMsg[0].c_str() , "highblueyeartime") == 0)
		{
			succ = succ && safe_atoi(extrasMsg[1].c_str() , nHighQHallDmdYearTime);
		}
	}

	string sid = tokens[15];

	if (succ)
	{
		UserLogin::addEvent(eq_, fd_, siteid, platid, name, head, gender,
			friends, i4IsYellowDmd, i4IsYellowDmdYear, i4YellowDmdLv, sid,nCity,strVIA,region,isRecall,isHighVip,isApp51Act,nCustomFlag,strTaskMarketId,strChannel,nBackFlowFlag,isSuperVip,nQHallDmdTime,nQHallDmdYearTime,nHighQHallDmdTime,nHighQHallDmdYearTime);
	}
}