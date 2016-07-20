#define _CRT_SECURE_NO_WARNINGS
#include "AdminHandler.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../common/string-util.h"
#include "GameNetHandler.h"
#include "GameHttpHandler.h"
#include "../net/NetCache.h"
#include "../event/EventQueue.h"
//#include "event/DealAdminEvent.h"

using namespace std;

AdminHandler::AdminHandler(GameNetHandler *nh, int fd, int nid)
{
	this->nh = nh;
	this->fd = fd;
	this->nid_ = nid;
	welcome();
}

AdminHandler::~AdminHandler(void)
{
}

int AdminHandler::handlerType()
{
	return ProtocolHandler::ADMIN;
}

void AdminHandler::print(bool prompt, const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	if(prompt)
	{
		strcat(buf, "\r\nBombBaby# ");
	}

	nh->sendFdString(fd, buf, strlen(buf));
}

void AdminHandler::welcome()
{
	print(true, "Welcome!\r\n");
}

void AdminHandler::invalid()
{
	print(true, "Invalid command!\r\n");
}

void AdminHandler::handle(int64 uid, string &req)
{
#if 0
	if (req[0]==(char)255) // process telnet commands
	{
		if (req[1]==(char)0xfb && req[2]==(char)0x22) // WILL LINEMODE
		{
			req[1] = (char)0xfd;
			print(true, req.c_str());
		}
	}
	else
	{
		bool bPart_1 = true , bPart_2 = true;
		vector<string> tokens;
		static string delims = ",";
		tokenize(req, tokens, delims);
		if (tokens.size()<1)
		{
			invalid();
			return;
		}
		string cmd = tokens[0];
		tokens.erase(tokens.begin());
		if (cmd=="exit")
		{
			print(false, "bye");
			nh->closeConnection(fd);
		}
		else if (checkCmd(cmd, "addcredit", tokens, 2))
		{
			int64 puid;
			int credit;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), credit))
			{
				DealAdminEvent::AddEvent_AddCredit(nh->eq, nid_, fd, puid, credit);
			}
			else
			{
				invalid();
			}
		}
		
		else if (checkCmd(cmd, "addhonor", tokens, 2))
		{
			int64 puid;
			int honor;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), honor))
			{
				DealAdminEvent::AddEvent_AddHonor(nh->eq, nid_, fd, puid, honor);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "canattack", tokens, 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_CanAttack(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setlevel", tokens, 2))
		{
			int64 puid;
			int level;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), level))
			{
				DealAdminEvent::AddEvent_SetLevel(nh->eq, nid_, fd, puid, level);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "banlogin", tokens, 3))
		{
			int64 puid;
			int isban;
			int min;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), isban) &&
				safe_atoi(tokens[2].c_str(), min))
			{
				DealAdminEvent::AddEvent_BanLogin(nh->eq, nid_, fd, puid, isban, min);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addres", tokens, 3))
		{
			int64 puid;
			int nRes1;
			int nRes2;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nRes1) &&
				safe_atoi(tokens[2].c_str(), nRes2))
			{
				DealAdminEvent::AddEvent_AddRes(nh->eq, nid_, fd, puid, nRes1, nRes2);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "addarmy", tokens, 3))
		{
			int64 puid;
			int nArmyType;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nArmyType) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddArmy(nh->eq, nid_, fd, puid, nArmyType, nCnt,true);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "delarmy", tokens, 3))
		{
			int64 puid;
			int nArmyType;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nArmyType) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddArmy(nh->eq, nid_, fd, puid, nArmyType, nCnt,false);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "addbld", tokens, 4))
		{
			int64 puid;
			int nBldType;
			int nLV;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldType) &&
				safe_atoi(tokens[2].c_str(), nLV)&&
				safe_atoi(tokens[3].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddBld(nh->eq, nid_, fd, puid, nBldType, nLV,nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "delbld", tokens, 2))
		{
			int64 puid;
			int nBldID;
			
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldID))
			{
				DealAdminEvent::AddEvent_DelBld(nh->eq, nid_, fd, puid, nBldID);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setbldcnt", tokens, 4))
		{
			int64 puid;
			int nBldType;
			int nLV;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldType) &&
				safe_atoi(tokens[2].c_str(), nLV)&&
				safe_atoi(tokens[3].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_SetBldCnt(nh->eq, nid_, fd, puid, nBldType, nLV,nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setallbldlv", tokens, 3))
		{
			int64 puid;
			int nBldType;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldType) &&
				safe_atoi(tokens[2].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_SetAllBldLV(nh->eq, nid_, fd, puid, nBldType, nLV);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setbldlv", tokens, 3))
		{
			int64 puid;
			int nBldID;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldID) &&
				safe_atoi(tokens[2].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_SetBldLV(nh->eq, nid_, fd, puid, nBldID, nLV);
			}
			else
			{
				invalid();
			}		
		}
		else if (checkCmd(cmd, "setarmylv", tokens, 3))
		{
			int64 puid;
			int nArmyType;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nArmyType) &&
				safe_atoi(tokens[2].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_SetArmyLV(nh->eq, nid_, fd, puid, nArmyType, nLV);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "addnpc", tokens, 2))
		{
			int64 puid;
			int nNPcID;
			
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nNPcID))
			{
				DealAdminEvent::AddEvent_AddNpc(nh->eq, nid_, fd, puid, nNPcID, true);
			}
			else
			{
				invalid();
			}			
		}
		else if (checkCmd(cmd, "delnpc", tokens, 2))
		{
			int64 puid;
			int nNPcID;

			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nNPcID))
			{
				DealAdminEvent::AddEvent_AddNpc(nh->eq, nid_, fd, puid, nNPcID, false);
			}
			else
			{
				invalid();
			}				
		}
		else if (checkCmd(cmd, "savenpc", tokens, 1))
		{//将玩家数据保存为NPC的json数据
			int64 puid;

			if (safe_atoll(tokens[0].c_str(), puid) )
			{
				DealAdminEvent::AddEvent_SaveNpc(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}				
		}
		else if (checkCmd(cmd, "reload", tokens, 0))
		{
			DealAdminEvent::AddEvent_Reload(nh->eq, nid_, fd);
		}
		else if (checkCmd(cmd, "profile", tokens, 1))
		{
			int nCmd = 1;
			if (safe_atoi(tokens[0].c_str(), nCmd))
			{
				DealAdminEvent::AddEvent_Profile(nh->eq, nCmd);
			}
			else
			{
				invalid();
			}		
			
		}
		else if (checkCmd(cmd, "adenable", tokens, 3))
		{//adenable,0,10000
			int nEnable;
			int nMinRecharge;
			int nMaxRecharge;
			if (safe_atoi(tokens[0].c_str(), nEnable) &&
				safe_atoi(tokens[1].c_str(), nMinRecharge) &&
				safe_atoi(tokens[2].c_str(), nMaxRecharge))
			{
				DealAdminEvent::AddEvent_AdSet(nh->eq, nid_, fd,AAT_Enable,nEnable,nMinRecharge,nMaxRecharge,"","");
			}
			else
			{
				invalid();
			}		
		}
		else if (checkCmd(cmd, "adregtime", tokens, 2))
		{//adregtime,2013/01/01,2013/02/01
			DealAdminEvent::AddEvent_AdSet(nh->eq, nid_, fd,AAT_RegTime,0,0,0,tokens[0],tokens[1]);	
		}
		else if (checkCmd(cmd, "adpoptime", tokens, 2))
		{//adpoptime,2013/01/01,2013/02/01
			DealAdminEvent::AddEvent_AdSet(nh->eq, nid_, fd,AAT_PopTime,0,0,0,tokens[0],tokens[1]);	
		}
		else if (checkCmd(cmd,"bandwguser",tokens,1))
		{
			int isban;
			if (safe_atoi(tokens[0].c_str(), isban))
			{
				DealAdminEvent::AddEvent_BandWGUser(nh->eq,nid_,isban,fd);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "cleardefensenpc", tokens, 3))
		{
			int64 nuid;
			int nType;
			int nCustom;
			if (safe_atoll(tokens[0].c_str(), nuid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nCustom))
			{
				DealAdminEvent::AddEvent_ClearDefense(nh->eq, nid_, fd, nuid, nType, nCustom);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setnewcomergift", tokens, 4))
		{
			int64 nuid;
			int nType;
			int nId;
			int nState;
			if (safe_atoll(tokens[0].c_str(), nuid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nId) &&
				safe_atoi(tokens[3].c_str(), nState))
			{
				DealAdminEvent::AddEvent_SetNewComerGift(nh->eq, nid_, fd, nuid, nType, nId, nState);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setContinulandday", tokens, 2))
		{
			int64 nuid;
			int nDay;
			if (safe_atoll(tokens[0].c_str(), nuid) &&
				safe_atoi(tokens[1].c_str(), nDay) )
			{
				DealAdminEvent::AddEvent_SetContinueLandDay(nh->eq, nid_, fd, nuid, nDay);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setplatinviteplatid", tokens, 2))
		{
			int64 nuid;
			if (safe_atoll(tokens[0].c_str(), nuid))
			{
				DealAdminEvent::AddEvent_SetPlatInviteID(nh->eq, nid_, fd, nuid,tokens[1]);
			}
			else
			{
				invalid();
			}	
		}

		else if (checkCmd(cmd, "additem", tokens, 3))
		{
			int64 puid;
			int nItemType;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nItemType) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddItem(nh->eq, nid_, fd, puid, nItemType, nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"addfeats",tokens,2))
		{
			int64 puid;
			int feats;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), feats))
			{
				DealAdminEvent::AddEvent_AddFeats(nh->eq, nid_, fd, puid, feats);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"addemblem",tokens,3))
		{
			int64 puid;
			int nEmblemID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nEmblemID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddEmblem(nh->eq, nid_, fd, puid, nEmblemID, nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"addwpccom",tokens,4))
		{
			int64 puid;
			int nComID;
			int nCnt;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nComID) &&
				safe_atoi(tokens[2].c_str(), nCnt)	&&
				safe_atoi(tokens[3].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_AddWPCCom(nh->eq, nid_, fd, puid, nComID, nCnt, nLV);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"addbook",tokens,4))
		{
			int64 puid;
			int nBookID;
			int nCnt;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBookID) &&
				safe_atoi(tokens[2].c_str(), nCnt)&&
				safe_atoi(tokens[3].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_AddBook(nh->eq, nid_, fd, puid, nBookID, nCnt, nLV);
			}
			else
			{
				invalid();
			}	
		}	
        else if (checkCmd(cmd,"delbookexp",tokens,3))
        {
            int64 puid;
            int nBookID;
            int nExp;
            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nBookID) &&
                safe_atoi(tokens[2].c_str(), nExp))
            {
                DealAdminEvent::AddEvent_DelBookExp(nh->eq, nid_, fd, puid, nBookID, nExp);
            }
            else
            {
                invalid();
            }
        }
		else if(checkCmd(cmd,"addadmiral",tokens,3))
		{
			int64 puid;
			int nAdmiralID;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nAdmiralID) &&
				safe_atoi(tokens[2].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_AddAdmiral(nh->eq, nid_, fd, puid, nAdmiralID,nLV, true);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"setadmiralskill",tokens,3))
		{
			int64 puid;
			int nAdmiralID;
			int nLV;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nAdmiralID) &&
				safe_atoi(tokens[2].c_str(), nLV))
			{
				DealAdminEvent::AddEvent_SetAdmiralSkill(nh->eq, nid_, fd, puid, nAdmiralID,nLV);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"deladmiral",tokens,2))
		{
			int64 puid;
			int nAdmiralID;
	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nAdmiralID) )
			{
				DealAdminEvent::AddEvent_AddAdmiral(nh->eq, nid_, fd, puid, nAdmiralID,0, false);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"addsoul",tokens,2))
		{
			int64 puid;
			int nCnt;

			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddAdmiralSoul(nh->eq, nid_, fd, puid, nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"addadmiraltrainexp",tokens,3))
		{
			int64 puid;
			int nCnt;
			int nAdmiralID;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nAdmiralID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddAdmiralTrainExp(nh->eq, nid_, fd, puid,nAdmiralID, nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "delbldcnt", tokens, 3))
		{
			int64 puid;
			int nBldType;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldType) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_DetBldCnt(nh->eq, nid_, fd, puid, nBldType,nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "recoverarmyhp", tokens,1)) 
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_RecoverHP(nh->eq, nid_, fd, puid, 0);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "recoverbldhp", tokens,1)) 
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_RecoverHP(nh->eq, nid_, fd, puid, 1);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "recoverherohp", tokens,1)) 
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_RecoverHP(nh->eq, nid_, fd, puid, 2);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setinit", tokens, 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid) )
			{
#ifdef _WIN32
				DealAdminEvent::AddEvent_SetInit(nh->eq, nid_, fd, puid);
#endif	
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setsciencelvl", tokens, 3))
		{
			int64 puid;
			int nRes1;
			int nRes2;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nRes1) &&
				safe_atoi(tokens[2].c_str(), nRes2))
			{
				DealAdminEvent::AddEvent_SetScienceLvl(nh->eq, nid_, fd, puid, nRes1, nRes2);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "setgmtestvalue", tokens, 3))
		{
			int64 puid;
			int nRes1;
			int nRes2;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nRes1) &&
				safe_atoi(tokens[2].c_str(), nRes2))
			{
				DealAdminEvent::AddEvent_SetGMTestValue(nh->eq, nid_, fd, puid, nRes1, nRes2);
			}
			else
			{
				invalid();
			}	
		}

		else if (checkCmd(cmd, "getbldcnt", tokens, 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_GetBldCnt(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addmooncnt", tokens, 2))
		{
			int64 puid;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_MoonCnt(nh->eq, nid_, fd, puid, nCnt);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "clearstrategic", tokens, 2))
		{
			int64 puid;
			int nType;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType))
			{
				DealAdminEvent::AddEvent_ClearStrategic(nh->eq, nid_, fd, puid, nType);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "pvesetcustom", tokens, 5))
		{//PVE设置
			//uid,custom,refresh,buyrefresh,fail
			int64 puid;
			int nCustom,nRefresh,nBuyRefresh,nFail;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCustom) &&
				safe_atoi(tokens[2].c_str(), nRefresh) &&
				safe_atoi(tokens[3].c_str(), nBuyRefresh) &&
				safe_atoi(tokens[4].c_str(), nFail) )
			{
				DealAdminEvent::AddEvent_PveSetCustom(nh->eq, nid_, fd, puid, nCustom,nRefresh,nBuyRefresh,nFail);
			}
			else
			{
				invalid();
			}	
		}
		else if (checkCmd(cmd, "pvefighttest", tokens, 3))
		{
			//uid
			int64 puid;
			int nTimes,nKind;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nTimes) &&
				safe_atoi(tokens[2].c_str(), nKind))
			{
				DealAdminEvent::AddEvent_PveFightStart(nh->eq, nid_, fd, puid, nTimes,nKind);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "reinitdb", tokens, 1))
		{
			//uid
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_ReInitDB(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addcoupon", tokens, 2))
		{
			int64 puid;
			int coupon;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), coupon))
			{
				DealAdminEvent::AddEvent_AddCoupon(nh->eq, nid_, fd, puid, coupon);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "testhttpreq", tokens, 6))
		{
			//openid,uid,range,type,typesec,argvs
			int64 nuid;
			int ntype,nRegion,ntypesec;
			safe_atoll(tokens[1].c_str(), nuid);
			safe_atoi(tokens[2].c_str(), nRegion);
			safe_atoi(tokens[3].c_str(), ntype);
			safe_atoi(tokens[4].c_str(), ntypesec);

			if(HttpRequireHandler::Instance().CanUse())
			{
				HttpRequireHandler::Instance().SafePushHttpRequest(tokens[0],uid,nRegion,ntype,ntypesec,tokens[5]);
			}
		}

		else if (checkCmd(cmd, "addthew", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddCountryCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_COUNTRYTHEW, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addmorale", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddCountryCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_COUNTRYMORALE, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addzhangong", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddCountryCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_COUNTRYZHANGONG, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addcountrybox", tokens, 2))
		{
			int64 puid; 
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddCountryCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_COUNTRYBOX, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "reloadcountryrank", tokens, 0))
		{
			DealAdminEvent::AddEvent_ReloadCountryRank(nh->eq, nid_, fd);
		}
		else if (checkCmd(cmd, "arenafightset", tokens, 4))
		{//国家竞技场
			//uid,type,value1,value2
			//1成就 2成就领取 3上次排名 4上次排名领奖 5当前可战斗次数 6已购买次数 7增加战斗积分 
			//8清除成就完成数据、成就、成就领取标记 9设置当前排名 10清除竞技场排名 11清除活动领取记录
			//12设置为未结算 13增加本期积分 14设置上期排名，用于世界竞技场
			int64 puid;
			int nType,nValue1,nValue2;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nValue1) &&
				safe_atoi(tokens[3].c_str(), nValue2))
			{
				DealAdminEvent::AddEvent_ArenaFightSet(nh->eq, nid_, fd, puid, nType,nValue1,nValue2);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "worldarenaset", tokens, 3))
		{//世界竞技场
			//uid,type,value1
			//1上期排名 2上期排名领奖 3当前可战斗次数 4已购买次数 5昨日排名 6设置当前排名 
			int64 puid;
			int nType,nValue1;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nValue1))
			{
				DealAdminEvent::AddEvent_WorldArenaSet(nh->eq, nid_, fd, puid, nType,nValue1);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "regionarenaset", tokens, 3))
		{//跨区竞技场
			//uid,type,value1
			// 1上期排名 2上次排名领奖 3当前可战斗次数 4已购买次数 5昨日排名 6设置积分 7设置报名 8刷新排名

			int64 puid;
			int nType,nValue1;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nValue1))
			{
				DealAdminEvent::AddEvent_RegionArenaSet(nh->eq, nid_, fd, puid, nType,nValue1);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "heroarenaset", tokens, 3))
		{//英雄竞技场
			//uid,type,value1
			//1上期排名 2上期排名领奖 3当前可战斗次数 4已购买次数 5昨日排名 6设置当前排名 
			int64 puid;
			int nType,nValue1;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType) &&
				safe_atoi(tokens[2].c_str(), nValue1))
			{
				DealAdminEvent::AddEvent_HeroArenaSet(nh->eq, nid_, fd, puid, nType,nValue1);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "reloadgamerank", tokens, 1))
		{
			int nType;	
			if (safe_atoi(tokens[0].c_str(), nType) )
			{
				DealAdminEvent::AddEvent_ReloadGameRank(nh->eq, nid_, fd, nType);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "addadmiraltimes", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddAdmiralTimes(nh->eq, nid_, fd, puid, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "finishguide", tokens, 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_FinishGuide(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"addequip",tokens,3))
		{
			int64 puid;
			int nEquipID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nEquipID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddEquip(nh->eq, nid_, fd, puid, nEquipID, nCnt);
			}
			else
			{
				invalid();
			}	
		}
        else if(checkCmd(cmd,"delequip",tokens,2))
        {//2015.3.9 添加删除装备命令接口
            int64 puid;
            int nEquipID;
            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nEquipID))
            {
                DealAdminEvent::AddEvent_DelEquip(nh->eq, nid_, fd, puid, nEquipID);
            }
            else
            {
                invalid();
            }	
        }
		else if (checkCmd(cmd, "addguildscore", tokens, 2))
		{//增加工会贡献值
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddGuildScore(nh->eq, nid_, fd, puid, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addguildbldvalue", tokens, 2))
		{//增加工会建设值
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddGuildBldValue(nh->eq, nid_, fd, puid, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setguildsciencelv", tokens, 3))
		{//设置工会科技等级
			int64 puid;
			int nScienceID;
			int nLV;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nScienceID) &&
				safe_atoi(tokens[2].c_str(), nLV) )
			{
				DealAdminEvent::AddEvent_SetGuildScienceLV(nh->eq, nid_, fd, puid, nScienceID, nLV);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setguildbldlv", tokens, 3))
		{//设置工会建筑等级
			int64 puid;
			int nBldID;
			int nLV;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBldID) &&
				safe_atoi(tokens[2].c_str(), nLV) )
			{
				DealAdminEvent::AddEvent_SetGuildBldLV(nh->eq, nid_, fd, puid, nBldID, nLV);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addguildbtlreward", tokens, 3))
		{//发送工会战排名奖励
			int nRegion;
			int nGuildID;
			int nRank;
			if (safe_atoi(tokens[0].c_str(), nGuildID) &&
				safe_atoi(tokens[1].c_str(), nRegion) &&
				safe_atoi(tokens[2].c_str(), nRank) )
			{
				DealAdminEvent::AddEvent_AddGuildBtlReward(nh->eq, nid_, fd,nGuildID, nRegion, nRank);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "clearpassword", tokens, 2))
		{
			int64 puid;
			int time;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), time))
			{
				DealAdminEvent::AddEvent_ClearPassword(nh->eq, nid_, fd, puid, time);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setofficer", tokens, 3))
		{//设置官员职位
			int64 puid;
			int nPosition;
			int nFlag;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nPosition)&&
				safe_atoi(tokens[2].c_str(), nFlag))
			{
				DealAdminEvent::AddEvent_SetOfficerPosition(nh->eq, nid_, fd, puid, nPosition,nFlag);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addbuff", tokens, 2))
		{//加buff
			int64 puid;
			int nBuffId;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nBuffId))
			{
				DealAdminEvent::AddEvent_AddBuff(nh->eq, nid_, fd, puid, nBuffId);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"delemblem",tokens,3))
		{
			int64 puid;
			int nEmblemID;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nEmblemID) &&
				safe_atoi(tokens[2].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_DelEmblem(nh->eq, nid_, fd, puid, nEmblemID, nExp);
			}
			else
			{
				invalid();
			}	
		}
        else if (checkCmd(cmd, "delemblemexp",tokens,3))
        {
            int64 puid;
            int nEmblemDBID;
            int nExp;
            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nEmblemDBID) &&
                safe_atoi(tokens[2].c_str(), nExp))
            {
                DealAdminEvent::AddEvent_DelEmblemExp(nh->eq, nid_, fd, puid, nEmblemDBID, nExp);
            }
            else
            {
                invalid();
            }	
        }
		else if(checkCmd(cmd,"delwpccom",tokens,3))
		{
			int64 puid;
			int nEmblemID;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nEmblemID) &&
				safe_atoi(tokens[2].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_DelWpccom(nh->eq, nid_, fd, puid, nEmblemID, nExp);
			}
			else
			{
				invalid();
			}	
		}
        else if(checkCmd(cmd,"delwpccomexp",tokens,3))
        {
            int64 puid;
            int nEmblemID;
            int nExp;
            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nEmblemID) &&
                safe_atoi(tokens[2].c_str(), nExp))
            {
                DealAdminEvent::AddEvent_DelWpccomExp(nh->eq, nid_, fd, puid, nEmblemID, nExp);
            }
            else
            {
                invalid();
            }	
        }
		else if (checkCmd(cmd, "setvip", tokens, 3))
		{
			int64 puid;
			int nType;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nType)&&
				safe_atoi(tokens[2].c_str(), nValue)
				)
			{
				DealAdminEvent::AddEvent_SetVip(nh->eq, nid_, fd, puid,nType, nValue);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "setequiplv", tokens, 4))
		{
			int64 puid;
			int nEquipType;
			int nCurLevel;
			int nLastLevel;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nEquipType)&&
				safe_atoi(tokens[2].c_str(), nLastLevel)&&
				safe_atoi(tokens[3].c_str(), nCurLevel)
				)
			{
				DealAdminEvent::AddEvent_SetEquipStrLv(nh->eq, nid_, fd, puid,nEquipType, nCurLevel,nLastLevel);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "specialvalue", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue)
				)
			{
				DealAdminEvent::AddEvent_SpecialValue(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd, "clearNewDayInfo", tokens, 1))
        {
            int64 puid;
            if (safe_atoll(tokens[0].c_str(), puid))
            {
                DealAdminEvent::AddEvent_ClearNewDayInfo(nh->eq, nid_, fd, puid);
            }
            else
            {
                invalid();
            }
        }

		else if (checkCmd(cmd, "pvebattleset", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue))
			{
				DealAdminEvent::AddEvent_PveBattleSet(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "addjunling", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue))
			{
				DealAdminEvent::AddEvent_AddJunLing(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}

        else if (checkCmd(cmd, "addpokers", tokens, 3))
        {//添加纸牌、换牌卡及积分
            int64 puid;
            int nPokerNum;
            int nChangeCard;
            int nPoints;
            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nPokerNum) &&
                safe_atoi(tokens[2].c_str(), nPoints))
            {
                DealAdminEvent::AddEvent_AddPokers(nh->eq, nid_, fd, puid, nPokerNum, 0, nPoints);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd, "resetpokerexc", tokens, 1))
        {//添加纸牌、换牌卡及积分
            int64 puid;
            if (safe_atoll(tokens[0].c_str(), puid))
            {
                DealAdminEvent::AddEvent_ResetPokerExc(nh->eq, nid_, fd, puid);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd, "resetduelinfo", tokens, 1))
        {
            int64 puid;
            if (safe_atoll(tokens[0].c_str(), puid))
            {
                DealAdminEvent::AddEvent_ResetDuelInfo(nh->eq, nid_, fd, puid);
            }
            else
            {
                invalid();
            }
        }
		else if(checkCmd(cmd,"addhero",tokens,2))
		{
			int64 puid;
			int nHeroID;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nHeroID))
			{
				DealAdminEvent::AddEvent_AddHero(nh->eq, nid_, fd, puid, nHeroID,true);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"delhero",tokens,2))
		{
			int64 puid;
			int nHeroID;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nHeroID))
			{
				DealAdminEvent::AddEvent_AddHero(nh->eq, nid_, fd, puid, nHeroID,false);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"addheroletter",tokens,3))
		{
			int64 puid;
			int nHeroLetterID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nHeroLetterID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddHeroLetter(nh->eq, nid_, fd, puid, nHeroLetterID,nCnt,true);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"delheroletter",tokens,3))
		{
			int64 puid;
			int nHeroLetterID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nHeroLetterID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddHeroLetter(nh->eq, nid_, fd, puid, nHeroLetterID,nCnt,false);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"sethero",tokens,4))
		{
			int64 puid;
			int nHeroID;
			int nType;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nHeroID) &&
				safe_atoi(tokens[2].c_str(), nType) &&
				safe_atoi(tokens[3].c_str(), nValue))
			{
				DealAdminEvent::AddEvent_SetHero(nh->eq, nid_, fd, puid, nHeroID,nType,nValue);
			}
			else
			{
				invalid();
			}	
		}

		else if (checkCmd(cmd, "resetwholecountry", tokens, 0))
		{//重置跨服阵营战
			int64 puid = 0;
			DealAdminEvent::AddEvent_ResetWholeCountry(nh->eq, nid_, fd, puid);
		}

		else if (checkCmd(cmd, "wholecountrysign", tokens, 1))
		{//阵营战报名
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_WholeCountrySign(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"addadmiralletter",tokens,3))
		{
			int64 puid;
			int nLetterID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nLetterID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddAdmiralLetter(nh->eq, nid_, fd, puid, nLetterID,nCnt,true);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"deladmiralletter",tokens,3))
		{
			int64 puid;
			int nLetterID;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nLetterID) &&
				safe_atoi(tokens[2].c_str(), nCnt))
			{
				DealAdminEvent::AddEvent_AddAdmiralLetter(nh->eq, nid_, fd, puid, nLetterID,nCnt,false);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"setproperty",tokens,3))
		{
			int64 puid;
			int nPropertyID;
			int nLevel;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nPropertyID) &&
				safe_atoi(tokens[2].c_str(), nLevel))
			{
				DealAdminEvent::AddEvent_SetProperty(nh->eq, nid_, fd, puid, nPropertyID,nLevel);
			}
			else
			{
				invalid();
			}	
		}

		else if (checkCmd(cmd, "addgvethew", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddGveCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_GVETHEW, nCount);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addgvepoint", tokens, 2))
		{
			int64 puid;
			int nCount;	
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) )
			{
				DealAdminEvent::AddEvent_AddGveCoin(nh->eq, nid_, fd, puid, AOT_ADMIN_ADD_GVEPOINT, nCount);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "gvelist", tokens, 4))
		{
			int64 puid;
			int nCount;	
			int nType;
			int nGveCity;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nCount) && 
				safe_atoi(tokens[2].c_str(), nType)  && 
				safe_atoi(tokens[3].c_str(), nGveCity))
			{
				DealAdminEvent::AddEvent_GveList(nh->eq, nid_, fd, puid, nCount,nType,nGveCity);
			}
			else
			{
				invalid();
			}
		}
#ifdef WIN32
        else if (checkCmd(cmd, "clearbagitem", tokens, 1))
        {//清楚所有的背包物品 仅在内网测试时使用  
            int64 puid;
            if (safe_atoi(tokens[0].c_str(), puid))
            {
                DealAdminEvent::ClearEvent_ClearAllBagItem(nh->eq, nid_, fd , puid);
            }
            else
            {
                invalid();
            }
        }
#endif
        else if (checkCmd(cmd, "addrentwpc", tokens, 6))
        {
            //第一个为UID
            //第二个为租用配件类型;2步兵3坦克4飞机5英雄
            //第三个参数为配件的ID
            //第四个为配件等级
            //第5个为QPOINTID
            //最后一个为删除标志位（0为添加，非零值为删除）
            int64 puid;
            int nType;
            int nWPCID;
            int nLevel;
            int nQPointID;
            int nFlag;

            if (safe_atoll(tokens[0].c_str(), puid) &&
                safe_atoi(tokens[1].c_str(), nType) && 
                safe_atoi(tokens[2].c_str(), nWPCID) &&
                safe_atoi(tokens[3].c_str(), nLevel) &&
                safe_atoi(tokens[4].c_str(), nQPointID) &&
                safe_atoi(tokens[5].c_str(), nFlag))
            {
                DealAdminEvent::AddEvent_AddRentWpc(nh->eq, nid_, fd, puid, nType,nWPCID,nLevel,nQPointID,nFlag);
            }
            else
            {
                invalid();
            }
        }

		// ------------ Client admin commands ends here ------------
		else if (checkCmd(cmd , "addguildtask" , tokens , 2))
		{
			int64 puid;
			int taskid;
			if (safe_atoll(tokens[0].c_str() , puid) &&
				safe_atoi(tokens[1].c_str() , taskid))
			{
				DealAdminEvent::AddEvent_AddGuildTask(nh->eq , nid_ , fd , puid , taskid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "setguildtaskprogress" , tokens , 2))
		{
			int64 puid;
			int count;
			if (safe_atoll(tokens[0].c_str() , puid) &&
				safe_atoi(tokens[1].c_str() , count))
			{
				DealAdminEvent::AddEvent_SetGuildTaskProgress(nh->eq , nid_ , fd , puid , count);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "gvgreward", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue)
				)
			{
				DealAdminEvent::AddEvent_GvgReward(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "setguildleaderdeltime" , tokens , 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0] , puid))
			{
				DealAdminEvent::AddEvent_SetGuildLeaderDelTime(nh->eq , nid_ , fd , puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "setguildleader" , tokens , 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0] , puid))
			{
				DealAdminEvent::AddEvent_SetGuildLeader(nh->eq , nid_ , fd , puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "addsearchtime", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue)
				)
			{
				DealAdminEvent::AddEvent_AddSearchTime(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd, "deladmiralspecial", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue)
				)
			{
				DealAdminEvent::AddEvent_DelAdmiralSpecial(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "delherospcial", tokens, 3))
		{
			int64 puid;
			int nValue;
			int nCnt;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue) && 
				safe_atoi(tokens[2].c_str(), nCnt)
				)
			{
				DealAdminEvent::AddEvent_DelHeroSpecial(nh->eq, nid_, fd, puid, nValue,nCnt);
			}
			else
			{
				invalid();
			}
		}

		else if (checkCmd(cmd, "delheroexpspecial", tokens, 2))
		{
			int64 puid;
			int nValue;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nValue) 
				)
			{
				DealAdminEvent::AddEvent_DelHeroExpSpecial(nh->eq, nid_, fd, puid, nValue);
			}
			else
			{
				invalid();
			}
		}

		else if(checkCmd(cmd,"delwpccomexpspecial",tokens,2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_DelWpccomExpSpecial(nh->eq, nid_, fd, puid, nExp);
			}
			else
			{
				invalid();
			}	
		}

		else if(checkCmd(cmd,"delemblemexpspecial",tokens,2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_DelEmblemExpSpecial(nh->eq, nid_, fd, puid, nExp);
			}
			else
			{
				invalid();
			}	
		}

		else if(checkCmd(cmd,"delbookexpspecial",tokens,2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_DelBookExpSpecial(nh->eq, nid_, fd, puid, nExp);
			}
			else
			{
				invalid();
			}	
		}

		else if(checkCmd(cmd,"addexpwpc",tokens,2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_AddExpWpc(nh->eq, nid_, fd, puid, nExp);
			}
			else
			{
				invalid();
			}	
		}

		else if(checkCmd(cmd,"addexpemblem",tokens,2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0].c_str(), puid) &&
				safe_atoi(tokens[1].c_str(), nExp))
			{
				DealAdminEvent::AddEvent_AddExpEmblem(nh->eq, nid_, fd, puid, nExp);
			}
			else
			{
				invalid();
			}	
		}
		else if(checkCmd(cmd,"dbrollback",tokens,1))
		{
			int64 puid;
			if (safe_atoll(tokens[0].c_str(), puid))
			{
				DealAdminEvent::AddEvent_DBRollBack(nh->eq, nid_, fd, puid);
			}
			else
			{
				invalid();
			}	
		}
		else
		{
			bPart_1 = false;
		}
	/*	else*/ if(checkCmd(cmd , "setwcpromoteexp" , tokens , 2))
		{
			int64 puid;
			int nExp;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nExp))
			{
				DealAdminEvent::AddEvent_SetWCPromoteExp(nh->eq , nid_ , fd , puid , nExp);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "setwcskillexp" , tokens , 3))
		{
			int64 puid;
			int nSkillType;
			int nSkillExp;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nSkillType)
				&&safe_atoi(tokens[2] , nSkillExp))
			{
				DealAdminEvent::AddEvent_SetWCSkillExp(nh->eq , nid_ , fd , puid , nSkillType , nSkillExp);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "addwcskillexp" , tokens , 3))
		{
			int64 puid;
			int nSkillType;
			int nSkillExp;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nSkillType)
				&&safe_atoi(tokens[2] , nSkillExp))
			{
				DealAdminEvent::AddEvent_AddWCSkillExp(nh->eq , nid_ , fd , puid , nSkillType , nSkillExp);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "subwcskillexp" , tokens , 3))
		{//测试想要个减的指令,skillexp > 0 正常减,降级;skillexp==0,直接清到刚激活状态
			int64 puid;
			int nSkillType;
			int nSkillExp;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nSkillType)
				&&safe_atoi(tokens[2] , nSkillExp))
			{
				DealAdminEvent::AddEvent_SubWCSkillExp(nh->eq , nid_ , fd , puid , nSkillType , nSkillExp);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "wcoptype" , tokens , 3))
		{
			int64 puid;
			int nEvtype;
			int nOptype;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nEvtype)
				&&safe_atoi(tokens[2] , nOptype))
			{
				DealAdminEvent::AddEvent_WCOpType(nh->eq , nid_ , fd , puid , nEvtype , nOptype);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "wcopuid" , tokens , 2))
		{
			int64 puid;
			int nEvtype;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nEvtype))
			{
				DealAdminEvent::AddEvent_WCOpUid(nh->eq , nid_ , fd , puid , nEvtype);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "sadop" , tokens , 3))
		{
			int64 puid;
			int nEvType;
			int nValue;
			if (safe_atoll(tokens[0] , puid)
				&&safe_atoi(tokens[1] , nEvType)
				&&safe_atoi(tokens[2] , nValue))
			{
				DealAdminEvent::AddEvent_SADOp(nh->eq , nid_ , fd , puid , nEvType , nValue);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd , "addreport" , tokens , 2))
        {
            int64 puid;
            int nNum;
            if (safe_atoll(tokens[0], puid) &&
                safe_atoi(tokens[1], nNum))
            {
                DealAdminEvent::AddEvent_AddWarGameReport(nh->eq, nid_, fd, puid, nNum);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd , "addwargametoken" , tokens, 2))
        {
            int64 puid;
            int nNum;
            if (safe_atoll(tokens[0], puid) &&
                safe_atoi(tokens[1], nNum))
            {
                DealAdminEvent::AddEvent_AddWarGameToken(nh->eq, nid_, fd, puid, nNum);
            }
            else
            {
                invalid();
            }
        }
		else if(checkCmd(cmd , "setstormbox" , tokens , 2))
		{
			int64 puid;
			int nflag;
			if (safe_atoll(tokens[0] , puid)
				&& safe_atoi(tokens[1] , nflag))
			{
				DealAdminEvent::AddEvent_SetStormBox(nh->eq , nid_ , fd , puid , nflag);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd , "setqqtenyearflag" , tokens , 2))
		{
			int64 puid;
			int nflag;
			if (safe_atoll(tokens[0] , puid)
				&& safe_atoi(tokens[1] , nflag))
			{
				DealAdminEvent::AddEvent_SetQQTenyearFlag(nh->eq , nid_ , fd , puid , nflag);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "happylibaward" , tokens , 2))
		{
			int64 puid;
			int nflag;
			if (safe_atoll(tokens[0] , puid)
				&& safe_atoi(tokens[1] , nflag))
			{
				DealAdminEvent::AddEvent_HappylibAward(nh->eq , nid_ , fd , puid , nflag);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd , "addmilitarysoulpoint" , tokens , 2))
        {
            int64 puid;
            int nNum;
            if (safe_atoll(tokens[0] , puid)
                && safe_atoi(tokens[1] , nNum))
            {
                DealAdminEvent::AddEvent_AddMilitarySoulPoint(nh->eq , nid_ , fd , puid , nNum);
            }
            else
            {
                invalid();
            }
        }
		else if (checkCmd(cmd , "countrychatui" , tokens , 1))
		{
			int64 puid;
			if (safe_atoll(tokens[0] , puid))
			{
				DealAdminEvent::AddEvent_CountryChatUI(nh->eq , nid_ , fd , puid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "countrychatmsg" , tokens , 2))
		{
			int64 puid;
			//string strMsg;
			if (safe_atoll(tokens[0] , puid))
			{
				DealAdminEvent::AddEvent_CountryChatMsg(nh->eq , nid_ , fd , puid , tokens[1]);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "countrychatmask", tokens , 4))
		{
			int64 puid;
			int type;//0:mask 1:unmask
			int64 tuid;
			string strName;
			if (safe_atoll(tokens[0] , puid)
				&& safe_atoi(tokens[1] , type)
				&& safe_atoll(tokens[2] , tuid))
			{
				DealAdminEvent::AddEvent_AddCountryChatMask(nh->eq , nid_ , fd , puid , type , tuid , tokens[3]);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd, "resetguildpkscore", tokens, 2))
        {//设置全区的公会战积分，全区所有公会设置为同一值
            int nRegion;
            int nScore;
            if (safe_atoi(tokens[0], nRegion)
                && safe_atoi(tokens[1], nScore))
            {
                DealAdminEvent::ResetEvent_ResetGuildPKScore(nh->eq , nid_ , fd, nRegion, nScore);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd, "setsevendaylogondays", tokens, 2))
        {
            int64 uid;
            int nDays;
            if (safe_atoll(tokens[0], uid)
                && safe_atoi(tokens[1], nDays))
            {
                DealAdminEvent::SetEvent_SevenDaysLogonDays(nh->eq , nid_ , fd, uid, nDays);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd, "addyasuiqian", tokens, 2))
        {
            int64 uid;
            int nCnt;
            if (safe_atoll(tokens[0], uid)
                && safe_atoi(tokens[1], nCnt))
            {
                DealAdminEvent::AddEvent_AddYaSuiQian(nh->eq , nid_ , fd, uid, nCnt);
            }
            else
            {
                invalid();
            }
        }
		else if (checkCmd(cmd , "npcactiverank" , tokens , 1))
		{//军团通缉令刷新排行榜
			int64 uid;
			if (safe_atoll(tokens[0] , uid))
			{
				DealAdminEvent::AddEvent_NpcactiveRank(nh->eq , nid_ , fd , uid);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "towerenhance" , tokens , 3))
		{
			int64 uid;
			int bldid;
			int enhancetype;
			if (safe_atoll(tokens[0] , uid)
				&& safe_atoi(tokens[1] , bldid)
				&& safe_atoi(tokens[2] , enhancetype))
			{
				DealAdminEvent::TowerEnhance(nh->eq , nid_ , fd , uid , bldid , enhancetype);
			}
			else
			{
				invalid();
			}
		}
		else if (checkCmd(cmd , "settowerenhance" , tokens , 3))
		{
			int64 uid;
			int bldid;
			int enhancetype;//type:0,清掉;1-3设置对应类型
			if (safe_atoll(tokens[0] , uid)
				&& safe_atoi(tokens[1] , bldid)
				&& safe_atoi(tokens[2] , enhancetype))
			{
				DealAdminEvent::SetTowerEnhance(nh->eq , nid_ , fd , uid , bldid , enhancetype);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd, "addjammer", tokens, 4))
        {
            int64 uid;
            int nJammerType;
            int nLv;
            int nCount;
            if (safe_atoll(tokens[0], uid)&&
                safe_atoi(tokens[1],nJammerType)&&
                safe_atoi(tokens[2],nLv)&&
                safe_atoi(tokens[3],nCount))
            {
                DealAdminEvent::AddEvent_AddJammer(nh->eq, nid_, fd, uid, nJammerType, nLv, nCount);
            }
            else
            {
                invalid();
            }
        }
        else if (checkCmd(cmd, "outputguilduser", tokens, 2))
        {//输出公会人员名单
            int guildID;
            int nRegion;
            if (safe_atoi(tokens[0], guildID) && safe_atoi(tokens[1], nRegion))
            {
                DealAdminEvent::OutputGuildUserList(nh->eq , nid_ , fd, guildID, nRegion);
            }
            else
            {
                invalid();
            }
        }
		else if (checkCmd(cmd , "setbdntimes" , tokens , 2))
		{
			//set BaseDefNpc step
			int64 uid;
			int nStep;
			if (safe_atoll(tokens[0] , uid) && safe_atoi(tokens[1] , nStep))
			{
				DealAdminEvent::BaseDefNpcTimes(nh->eq , nid_ , fd ,uid , nStep);
			}
			else
			{
				invalid();
			}
		}
        else if (checkCmd(cmd, "medalhonoropt", tokens, 4))
        {
            //荣誉勋章 0:增加授勋令  nValue1 数量
            //1:增加荣誉勋章  nValue1类型 nValue2等级
            //2:设置铸造次数 nValue1次数
            //3:设置当前的授勋类型 nValue1类型
            int64 uid;
            int nOptType;
            int nValue1;
            int nValue2;
            if (safe_atoll(tokens[0] , uid) &&
                safe_atoi(tokens[1], nOptType) &&
                safe_atoi(tokens[2], nValue1) &&
                safe_atoi(tokens[3], nValue2))
            {
                DealAdminEvent::MedalHonorOpt(nh->eq , nid_ , fd ,uid , nOptType, nValue1, nValue2);
            }
            else
            {
                invalid();
            }
        }
		else if (checkCmd(cmd , "SiteTestAdmin" , tokens , 3))
		{
			//封测活动
			int64 uid;
			int type , value;
			if (safe_atoll(tokens[0] , uid)
				&& safe_atoi(tokens[1] , type)
				&& safe_atoi(tokens[2] , value))
			{
				DealAdminEvent::SiteTestAdmin(nh->eq , nid_ , fd , uid , type , value);
			}
			else
			{
				invalid();
			}
		}
		else if(checkCmd(cmd,"SiteCreditReload",tokens,0))
		{
			DealAdminEvent::AddEvent_SiteCreditReload(nh->eq , nid_ ,fd);
		}
        else if(checkCmd(cmd,"SetCostGiftInfo",tokens,3))
        {
            int64 uid;
            int nType,nValue;
            if (safe_atoll(tokens[0] , uid)
                && safe_atoi(tokens[1] , nType)
                && safe_atoi(tokens[2] , nValue))
            {
                DealAdminEvent::SetCostCreditGiftInfo(nh->eq , nid_ , fd , uid , nType , nValue);
            }
            else
            {
                invalid();
            }
        }
		else
		{
			bPart_2 = false;
		}
		if (!bPart_1 && !bPart_2)
		{
			invalid();
		}
	}
#endif
}
