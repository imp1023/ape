#include "DealAdminEvent.h"
#include "../../event/event.pb.h"
#include "../../event/EventQueue.h"
#include "../GameEventHandler.h"
#include "../GameDataHandler.h"
#include "../../logic/Player.h"
#include "../../logic/GameConfigMgr.h"
#include "../../logic/GameSrvCfg.h"
#include "../../common/SysLog.h"
#include "../../logic/NPCInfoCfg.h"
#include "../GameHttpHandler.h"
#include "../../logic/GameConstantSetCfg.h"
#include "../../common/distribution.h"
#include "../../common/Clock.h"
#include <math.h>

extern ServerConfig serverConfig;

DealAdminEvent::DealAdminEvent()
: CBaseEvent()
{
}

DealAdminEvent::~DealAdminEvent()
{
}

void DealAdminEvent::AddEvent_AddCash(EventQueue* eq, int gid, int fd, int64 uid, int gold)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_ADDCASH);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_AddCash_Req* req = e->mutable_adminaddcashreq();
	req->set_gid(gid);
	req->set_platid(req->platid());
	req->set_uid(uid);
	req->set_type(0);
	req->set_cash(gold);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
#if 0
void DealAdminEvent::AddEvent_Profile(EventQueue* eq, int cmd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_PROFILE);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_Profile_Req* req = e->mutable_adminprofile_req();
	req->set_cmd(cmd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddExp(EventQueue* eq, int gid, int fd, int64 uid, int exp)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_ADDEXP);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(-1);
    Admin_AddExp_Req* req = e->mutable_adminaddexp_req();
    req->set_gid(gid);
    req->set_uid(uid);
    req->set_exp(exp);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_CanAttack(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_CANATTACK);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_CanAttack_Req* req = e->mutable_admincanattack_req();
	req->set_gid(gid);
	req->set_platid(req->platid());
	req->set_uid(uid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}


void DealAdminEvent::AddEvent_AddHonor(EventQueue* eq, int gid, int fd, int64 uid, int gold)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_ADDHONOR);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_AddHonor_Req* req = e->mutable_adminaddhonor_req();
	req->set_gid(gid);
	req->set_uid(uid);
	req->set_honor(gold);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetLevel(EventQueue* eq, int gid, int fd, int64 uid, int level)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_SETLEVEL);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(-1);
    Admin_SetLevel_Req* req = e->mutable_adminsetlevel_req();
    req->set_gid(gid);
    req->set_uid(uid);
    req->set_level(level);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_BanChat(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min)
{
    bool ban = isban ? true : false;
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_BANCHAT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(-1);
    Admin_BanChat_Req* req = e->mutable_adminbanchat_req();
    req->set_gid(gid);
    req->set_uid(uid);
    req->set_isban(ban);
    req->set_time(min);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_BanLogin(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min)
{
    bool ban = isban ? true : false;
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_BANLOGIN);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(-1);
    Admin_BanLogin_Req* req = e->mutable_adminbanlogin_req();
    req->set_gid(gid);
    req->set_uid(uid);
    req->set_isban(ban);
    req->set_time(min);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AdSet(EventQueue* eq, int gid, int fd,Admin_AdSet_Type nType,int nEnable,int nMinPerchage,int nMaxPerchage,string strDayStart,string strDayEnd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_AD_SET);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_AdSet_Req* req = e->mutable_adset_req();
	req->set_gid(gid);
	req->set_adminfd(fd);
	req->set_opttype(nType);
	req->set_enable(nEnable);
	req->set_minrecharge(nMinPerchage);
	req->set_maxrecharge(nMaxPerchage);
	req->set_daystart(strDayStart);
	req->set_dayend(strDayEnd);

	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_Reload(EventQueue* eq, int gid, int fd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_RELOAD);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_Reload_Req* req = e->mutable_adminreload_req();
	req->set_gid(gid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddGuildBtlReward(EventQueue* eq, int gid, int fd, int nGuildID,int nRegion,int nRank)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_ADDGUILDBTLREWARD);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_AddGuildBtlReward_Req* req = e->mutable_adminaddguildbtlreward_req();
	req->set_gid(gid);
	req->set_adminfd(fd);
	req->set_guildid(nGuildID);
	req->set_region(nRegion);
	req->set_guildrank(nRank);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_BandWGUser(EventQueue* eq, int gid, int bWGUser,int fd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_BANDWGUSER);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	Admin_BandWGUser_Req* req = e->mutable_adminbandwguser_req();
	req->set_banduser(bWGUser);
	req->set_gid(gid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ReloadGameRank(EventQueue* eq, int gid, int fd, int nType )
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_RELOADCHICKENRANK);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(0);
	Admin_ReloadChickenRank_Req* req = e->mutable_adreloadchickenrank_req();
	req->set_gid(gid);
	req->set_adminfd(fd);
	req->set_type(nType);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddGuildScore(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_GUILDSCORE);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddGuildBldValue(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_GUILDBLDVALUE);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetGuildScienceLV(EventQueue* eq, int gid, int fd, int64 uid, int nScienceID,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_GUILDSCIENCELV);
	req->set_scienceid(nScienceID);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetGuildBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldID,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_GUILDBLDLV);
	req->set_bldid(nBldID);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetOfficerPosition(EventQueue* eq, int gid, int fd, int64 uid, int nPosition,int nFlag)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_OFFICERPOSITION);
	req->set_itemtype(nPosition);
	req->set_bldtype(nFlag);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddBuff(EventQueue* eq, int gid, int fd, int64 uid, int nBuffId)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_AddBuff);
	req->set_itemtype(nBuffId);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetVip(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_VipLevel);
	req->set_itemtype(nType);
	req->set_bldtype(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetEquipStrLv(EventQueue* eq, int gid, int fd, int64 uid, int nEquipId,int nOldLevel,int nNewLevel)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_EQUIPSTRENGTHLEVEL);
	req->set_itemtype(nEquipId);
	req->set_bldtype(nOldLevel);
	req->set_armytype(nNewLevel);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddCountryCoin(EventQueue* eq, int gid, int fd, int64 uid, Admin_Opt_Type emType, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(emType);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_ReloadCountryRank(EventQueue* eq, int gid, int fd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(0);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_opttype(AOT_ADMIN_COUNTRY_RELOADRANK);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_AddRes(EventQueue* eq, int gid, int fd, int64 uid, int nRes1, int nRes2)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADDRES);
	req->set_res1(nRes1);
	req->set_res2(nRes2);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}
void DealAdminEvent::AddEvent_AddArmy(EventQueue* eq, int gid, int fd, int64 uid, int nArmyType, int nCnt,bool bAdd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	if(bAdd)
	{
		req->set_opttype(AOT_ADMIN_ADDARMY);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_DELARMY);
	}
	req->set_gid(gid);
	req->set_armytype(nArmyType);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_AddBld(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV,int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADDBLD);
	req->set_bldtype(nBldType);
	req->set_level(nLV);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelBld(EventQueue* eq, int gid, int fd, int64 uid, int nBldID)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DELBLD);
	req->set_bldid(nBldID);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetBldCnt(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV,int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETBLDCNT);
	req->set_bldtype(nBldType);
	req->set_level(nLV);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetAllBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETALLBLDLV);
	req->set_bldtype(nBldType);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_SetBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldID,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETBLDLV);
	req->set_bldid(nBldID);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_SetArmyLV(EventQueue* eq, int gid, int fd, int64 uid, int nArmyType,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETARMYLV);
	req->set_armytype(nArmyType);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SaveNpc(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SAVENPC);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddNpc(EventQueue* eq, int gid, int fd, int64 uid, int nNPCID,bool bAdd)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	if(bAdd)
		req->set_opttype(AOT_ADMIN_ADDNPC);
	else
		req->set_opttype(AOT_ADMIN_DELNPC);
	req->set_npcid(nNPCID);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ClearDefense(EventQueue* eq, int gid, int fd, int64 uid,int ntype, int ncustom)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_CLEARDEFENSE);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	adminClearDefense* req = e->mutable_admincleardefense();
	req->set_gid(gid);
	req->set_uid(uid);
	req->set_ntype(ntype);
	req->set_ncustom(ncustom);
	req->set_fd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetNewComerGift(EventQueue* eq, int gid, int fd, int64 uid,int ntype, int nid, int nstate)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_SETNEWCOMERGIFT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	adminSetNewComerGift* req = e->mutable_adminsetnewcomergift();
	req->set_gid(gid);
	req->set_uid(uid);
	req->set_ntype(ntype);
	req->set_nid(nid);
	req->set_nstate(nstate);
	req->set_fd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetContinueLandDay(EventQueue* eq, int gid, int fd, int64 uid,int nDay)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_SETCONTINUELANDDAY);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	adminSetContinueLandDay* req = e->mutable_adminsetcontinuelandday();
	req->set_gid(gid);
	req->set_uid(uid);
	req->set_nday(nDay);
	req->set_fd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetPlatInviteID(EventQueue* eq, int gid, int fd,int64 uid, string pid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_SETPLATINVITEID);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(-1);
	adminSetPlatInviteID* req = e->mutable_adminsetplatinviteid();
	req->set_gid(gid);
	req->set_uid(uid);
	req->set_npid(pid);
	req->set_fd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_AddFeats(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDFEATS);

	req->set_gid(gid);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddHero(EventQueue* eq, int gid, int fd, int64 uid, int nHeroID,bool bAdd)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	if(bAdd)
	{
		req->set_opttype(AOT_ADMIN_ADDHERO);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_DELHERO);
	}

	req->set_gid(gid);
	req->set_admiralid(nHeroID);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetHero(EventQueue* eq, int gid, int fd, int64 uid, int nHeroID,int nType,int nValue)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_SETHEROHP);

	req->set_gid(gid);
	req->set_admiralid(nHeroID);//////////////////////
	req->set_bldtype(nType);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddHeroLetter(EventQueue* eq, int gid, int fd, int64 uid,int nHeroLetterID, int nCnt,bool bAdd)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	if(bAdd)
	{
		req->set_opttype(AOT_ADMIN_ADDLETTER);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_DELLETTER);
	}
	req->set_gid(gid);
	req->set_admiralid(nHeroLetterID);//////////////////////
	req->set_count(nCnt);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddAdmiralTrainExp(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nCnt)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_ADDADMIRALTRAINEXP);

	req->set_admiralid(nAdmiralID);
	req->set_gid(gid);
	req->set_count(nCnt);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddAdmiralSoul(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_ADDSOUL);

	req->set_gid(gid);
	req->set_count(nCnt);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddAdmiral(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nLV,bool bAdd)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	if(bAdd)
	{
		req->set_opttype(AOT_ADMIN_ADDADMIRAL);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_DELADMIRAL);
	}

	req->set_gid(gid);
	req->set_admiralid(nAdmiralID);//////////////////////
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetAdmiralSkill(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nLV)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
    req->set_opttype(AOT_ADMIN_SET_ADMIRALSKILLLEVEL);

	req->set_gid(gid);
	req->set_admiralid(nAdmiralID);//////////////////////
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddWPCCom(EventQueue* eq, int gid, int fd, int64 uid, int nWPCComID, int nCnt,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDWPCCOM);

	req->set_gid(gid);
	req->set_wpccomid(nWPCComID);//////////////////////
	req->set_count(nCnt);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_AddBook(EventQueue* eq, int gid, int fd, int64 uid, int nBookID, int nCnt,int nLV)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDBOOK);

	req->set_gid(gid);
	req->set_bookid(nBookID);//////////////////////
	req->set_count(nCnt);
	req->set_level(nLV);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelBookExp(EventQueue* eq, int gid, int fd, int64 uid, int nBookID, int nExp)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);

    req->set_opttype(AOT_ADMIN_DELBOOKEXP);

    req->set_gid(gid);
    req->set_bookid(nBookID);//////////////////////
    req->set_count(nExp);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDEMBLEM);

	req->set_gid(gid);
	req->set_emblemtype(nEmblemType);//////////////////////
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddItem(EventQueue* eq, int gid, int fd, int64 uid, int nItemType, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDITEM);

	req->set_gid(gid);
	req->set_itemtype(nItemType);//////////////////////
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}
void DealAdminEvent::AddEvent_RecoverHP(EventQueue* eq, int gid, int fd, int64 uid, int nType)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	if(nType==0)
	{
		req->set_opttype(AOT_ADMIN_RECOVERARMYHP);
	}
	else if(nType==1)
	{
		req->set_opttype(AOT_ADMIN_RECOVERBLDHP);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_RECOVERHEROHP);
	}
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_DetBldCnt(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DELBLDCNT);
	req->set_bldtype(nBldType);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetInit(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETINIT);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_SetScienceLvl(EventQueue* eq, int gid, int fd, int64 uid, int nRes1, int nRes2)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETSCIENCELVL);
	req->set_res1(nRes1);
	req->set_res2(nRes2);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_SetGMTestValue(EventQueue* eq, int gid, int fd, int64 uid, int nRes1, int nRes2)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SETGMTESTVALUE);
	req->set_res1(nRes1);
	req->set_res2(nRes2);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_GetBldCnt(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_GETBLDCNT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_GetBldCnt_Req* req = e->mutable_admingetbldcnt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}


void DealAdminEvent::AddEvent_MoonCnt(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDMOONCNT);
	
	req->set_gid(gid);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);

}

void DealAdminEvent::AddEvent_ClearStrategic(EventQueue* eq, int gid, int fd, int64 uid, int nType)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_CLEARSTRATEGIC);

	req->set_gid(gid);
	req->set_itemtype(nType);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_PveSetCustom(EventQueue* eq, int gid, int fd, int64 uid, int nCustom,int nRefresh,int nBuyRefresh,int nFail)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_PVESET_CUSTOM);

	req->set_gid(gid);
	req->set_itemtype(nCustom);
	req->set_bldtype(nRefresh);
	req->set_armytype(nBuyRefresh);
	req->set_bldid(nFail);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ArenaFightSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1,int nValue2)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ARENAFIGHTSET);

	req->set_gid(gid);
	req->set_itemtype(nType);
	req->set_bldtype(nValue1);
	req->set_armytype(nValue2);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_WorldArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_WORLDARENAFIGHTSET);

	req->set_gid(gid);
	req->set_itemtype(nType);
	req->set_bldtype(nValue1);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_HeroArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_HEROARENAFIGHTSET);

	req->set_gid(gid);
	req->set_itemtype(nType);
	req->set_bldtype(nValue1);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_RegionArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_REGIONARENAFIGHTSET);

	req->set_gid(gid);
	req->set_itemtype(nType);
	req->set_bldtype(nValue1);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_PveFightStart(EventQueue* eq, int gid, int fd, int64 uid,int nTime,int nKind)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_PVE_FIGHT_START);

	req->set_gid(gid);
	req->set_itemtype(nTime);
	req->set_bldtype(nKind);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ReInitDB(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADMINAL_REINITDB);

	req->set_gid(gid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddCoupon(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDCOUPONS);

	req->set_gid(gid);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddAdmiralTimes(EventQueue* eq, int gid, int fd, int64 uid, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADDADMIRALTIMES);

	req->set_gid(gid);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_FinishGuide(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_FINISHGUIDE);
	req->set_gid(gid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ClearPassword(EventQueue* eq, int gid, int fd, int64 uid, int nTime)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_CLEAR_PASSWORD);
	req->set_gid(gid);
	req->set_count(nTime);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_DELEMBLEM);

	req->set_gid(gid);
	req->set_emblemtype(nEmblemType);//////////////////////
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelEmblemExp(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);

    req->set_opttype(AOT_ADMIN_DELEMBLEMEXP);

    req->set_gid(gid);
    req->set_emblemtype(nEmblemType);//////////////////////
    req->set_count(nExp);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelWpccom(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_DELWPCCOM);

	req->set_gid(gid);
	req->set_emblemtype(nEmblemType);//////////////////////
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelWpccomExp(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);

    req->set_opttype(AOT_ADMIN_DELWPCCOMEXP);

    req->set_gid(gid);
    req->set_emblemtype(nEmblemType);//////////////////////
    req->set_count(nExp);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SpecialValue(EventQueue* eq, int gid, int fd, int64 uid, int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SPECIAL_VALUE);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ClearNewDayInfo(EventQueue* eq, int gid, int fd, int64 uid)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_CLEAR_NEWDAYINFO);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_PveBattleSet(EventQueue* eq, int gid, int fd, int64 uid,int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_PVE_BATTLE_SET);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddJunLing(EventQueue* eq, int gid, int fd, int64 uid,int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_JUNLING);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddPokers(EventQueue* eq, int gid, int fd, int64 uid, int nPoker, int nChangeCard, int nPoints)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_ADD_POKERS);
    req->set_res1((int64)nPoker);
    req->set_res2((int64)nChangeCard);
    req->set_count(nPoints);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ResetPokerExc(EventQueue* eq, int gid, int fd, int64 uid)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_RESET_POKEREXC);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_ResetDuelInfo(EventQueue* eq, int gid, int fd, int64 uid)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_RESET_DUELINFO);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}


void DealAdminEvent::AddEvent_ResetWholeCountry(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_RESETWHOLECOUNTRY);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_CLEAR_WHOLE_COUNTRY);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_WholeCountrySign(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_WHOLE_COUNTRY_SIGN);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddGveCoin(EventQueue* eq, int gid, int fd, int64 uid, Admin_Opt_Type emType, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(emType);
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_GveList(EventQueue* eq, int gid, int fd, int64 uid, int nCnt, int nType,int nGveCity)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_GVELIST);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_GVE_LIST);
	req->set_count(nCnt);
	req->set_level(nType);
	req->set_bldid(nGveCity);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::ClearEvent_ClearAllBagItem(EventQueue* eq, int gid, int fd, int64 uid)
{
    Event * e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_CLEARBAGITEM);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddRentWpc(EventQueue* eq, int gid, int fd, int64 uid, int nType, int nWPCID, int nLevel, int nQPointID, int nFlag)
{
    Event * e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_ADD_RENTWPC);
    req->set_adminfd(fd);
    req->set_wpccomid(nWPCID);
    req->set_itemtype(nQPointID);
    req->set_level(nLevel);
    req->set_emblemtype(nType);
    req->set_flag(nFlag);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddGuildTask(EventQueue* eq , int gid , int fd , int64 uid , int taskid)
{
	Event *e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_GUILDTASK);
	req->set_count(taskid);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetGuildTaskProgress(EventQueue* eq , int gid , int fd , int64 uid , int count)
{
	Event *e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_GUILDTASK_PROGRESS);
	req->set_count(count);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_GvgReward(EventQueue* eq, int gid, int fd, int64 uid, int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_GVG_REWARD);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetGuildLeaderDelTime(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_GUILDLEADER_DELTIME);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetGuildLeader(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_GUILDLEADER);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddSearchTime(EventQueue* eq, int gid, int fd, int64 uid, int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_SEARCHTIME);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelAdmiralSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_ADMIRALSPECIAL);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelHeroSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nValue,int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_HEROSPECIAL);
	req->set_count(nValue);
	req->set_bldid(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}


void DealAdminEvent::AddEvent_DelHeroExpSpecial(EventQueue* eq, int gid, int fd, int64 uid,int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_HEROEXPSPECIAL);
	req->set_count(nValue);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelWpccomExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_WPCCOMEXPSPECIAL);
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelEmblemExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_EMBLEMEXPSPECIAL);
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelBookExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_DEL_BOOKXPSPECIAL);
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddExpWpc(EventQueue* eq, int gid, int fd, int64 uid, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_EXPWPC);
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DBRollBack(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_DBROLLBACK);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddExpEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_EXPEMBLEM);
	req->set_count(nExp);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}


void DealAdminEvent::AddEvent_SetWCPromoteExp(EventQueue* eq, int gid, int fd, int64 uid , int nExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_WCPROMOTE_EXP);
	req->set_adminfd(fd);
	req->set_count(nExp);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_WCSKILL_EXP);
	req->set_adminfd(fd);
	req->set_itemtype(nSkillType);
	req->set_count(nSkillExp);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_ADD_WCSKILL_EXP);
	req->set_adminfd(fd);
	req->set_itemtype(nSkillType);
	req->set_count(nSkillExp);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SubWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SUB_WCSKILL_EXP);
	req->set_adminfd(fd);
	req->set_itemtype(nSkillType);
	req->set_count(nSkillExp);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_WCOpType(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype , int nOptype)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_WCOP_TYPE);
	req->set_adminfd(fd);
	req->set_itemtype(nEvtype);
	req->set_count(nOptype);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_WCOpUid(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_WCOP_UID);
	req->set_adminfd(fd);
	req->set_count(nEvtype);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SADOp(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype , int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SADOP);
	req->set_adminfd(fd);
	req->set_itemtype(nEvtype);
	req->set_count(nValue);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddWarGameReport(EventQueue* eq, int gid, int fd, int64 uid , int nNum)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_ADD_WARGAME_REPORT);
    req->set_adminfd(fd);
    req->set_count(nNum);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddWarGameToken(EventQueue* eq, int gid, int fd, int64 uid , int nNum)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_ADD_WARGAME_TOKEN);
    req->set_adminfd(fd);
    req->set_count(nNum);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetStormBox(EventQueue* eq, int gid, int fd, int64 uid , int nFlag)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_STORMBOX);
	req->set_adminfd(fd);
	req->set_count(nFlag);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_SetQQTenyearFlag(EventQueue* eq, int gid, int fd, int64 uid , int nFlag)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_SET_QQTENYEARFLAG);
	req->set_adminfd(fd);
	req->set_count(nFlag);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_HappylibAward(EventQueue* eq, int gid, int fd, int64 uid , int nFlag)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_HAPPYLIB_AWARD);
	req->set_adminfd(fd);
	req->set_count(nFlag);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddMilitarySoulPoint(EventQueue* eq, int gid, int fd, int64 uid , int nNum)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_gid(gid);
    req->set_opttype(AOT_ADMIN_MILITARYSOUL_POINT);
    req->set_adminfd(fd);
    req->set_count(nNum);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_CountryChatUI(EventQueue* eq, int gid, int fd, int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_COUNTRYCHATUI);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_CountryChatMsg(EventQueue* eq, int gid, int fd, int64 uid , string strMsg)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_sid(strMsg);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_COUNTRYCHATMSG);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::AddEvent_AddCountryChatMask(EventQueue* eq , int gid , int fd , int64 uid , int type , int64 tuid , string strMsg)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_armytype(type);
	req->set_res1(tuid);
	req->set_sid(strMsg);
	req->set_gid(gid);
	req->set_opttype(AOT_ADMIN_COUNTRYCHATMASK);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::ResetEvent_ResetGuildPKScore(EventQueue* eq , int gid , int fd, int nRegion , int nScore)
{
    Event *e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_RESETGUILDPKSCORE);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(0);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(0);
    req->set_opttype(AOT_ADMIN_COUNTRYCHATMASK);
    req->set_gid(gid);
    req->set_adminfd(fd);
    req->set_count(nScore);

    nRegion = serverConfig.getmapregion(nRegion);
    int nCountryID = nRegion%serverConfig.country_num()+1;
    GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
    pNeedArgs->set_countrysrvid(nCountryID);
    pNeedArgs->set_gamesrvid(gid);
    pNeedArgs->set_region(nRegion);

    eq->safePushEvent(e);
}

void DealAdminEvent::SetEvent_SevenDaysLogonDays(EventQueue* eq , int gid , int fd, int64 uid , int nDays)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_count(nDays);
    req->set_opttype(AOT_ADMIN_SETSEVENDAYSLOGONDAYS);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddYaSuiQian(EventQueue* eq , int gid , int fd, int64 uid , int nCnt)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_count(nCnt);
    req->set_opttype(AOT_ADMIN_ADDYASUIQIAN);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_NpcactiveRank(EventQueue* eq , int gid , int fd , int64 uid)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_opttype(AOT_ADMIN_NPCACTIVERANK);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::TowerEnhance(EventQueue* eq , int gid , int fd, int64 uid , int bldid , int etype)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_count(bldid);
	req->set_itemtype(etype);
	req->set_opttype(AOT_ADMIN_TOWERHANCE);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::SetTowerEnhance(EventQueue* eq , int gid , int fd, int64 uid , int bldid , int etype)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_count(bldid);
	req->set_itemtype(etype);
	req->set_opttype(AOT_ADMIN_SETTOWERHANCE);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}
void DealAdminEvent::BaseDefNpcTimes(EventQueue* eq , int gid , int fd, int64 uid , int nStep)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_count(nStep);
	req->set_opttype(AOT_ADMIN_BDNTIMES);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::SiteTestAdmin(EventQueue* eq , int gid , int fd, int64 uid , int nType , int nValue)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	req->set_count(nType);
	req->set_itemtype(nValue);
	req->set_opttype(AOT_ADMIN_SITETEST);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_AddJammer(EventQueue* eq , int gid , int fd, int64 uid , int nJammerType , int nLv, int nCount)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);
    req->set_count(nCount);
    req->set_level(nLv);
    req->set_itemtype(nJammerType);
    req->set_opttype(AOT_ADMIN_ADDJAMMER);
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

void DealAdminEvent::OutputGuildUserList(EventQueue* eq , int gid , int fd, int guildID, int nRegion)
{
    Event *e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OUTPUTGUILDUSERLIST);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(0);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(0);
    req->set_opttype(AOT_ADMIN_ADDYASUIQIAN);
    req->set_gid(gid);
    req->set_adminfd(fd);
    req->set_count(guildID);

    nRegion = serverConfig.getmapregion(nRegion);
    int nCountryID = nRegion%serverConfig.country_num()+1;
    GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
    pNeedArgs->set_countrysrvid(nCountryID);
    pNeedArgs->set_gamesrvid(gid);
    pNeedArgs->set_region(nRegion);

    eq->safePushEvent(e);
}
#endif

void DealAdminEvent::handle(Event* e)
{
    switch (e->cmd())
    {
		case EVENT_ADMIN_ADDCASH:
			processAdminAddCash(e);
			break;
#if 0
		case EVENT_ADMIN_PROFILE:
			processAdminProfile(e);
			break;
        case EVENT_ADMIN_ADDEXP:
            processAdminAddExp(e);
            break;
		case EVENT_ADMIN_ADDHONOR:
			processAdminAddHonor(e);
			break;
		case EVENT_ADMIN_OPT:
			processAdminOpt(e);
			break;
		case EVENT_ADMIN_CANATTACK:
			processAdminCanAttack(e);
			break;
        case EVENT_ADMIN_SETLEVEL:
            processAdminSetLevel(e);
            break;
        case EVENT_ADMIN_BANCHAT:
            processAdminBanChat(e);
            break;
        case EVENT_ADMIN_BANLOGIN:
            processAdminBanLogin(e);
            break;
		case EVENT_ADMIN_RELOAD:
			processAdminReload(e);
			break;
		case EVENT_AD_SET:
			processAdSet(e);
			break;
		case EVENT_ADMIN_BANDWGUSER:
			processAdminBandWGUser(e);
			break;
		case EVENT_ADMIN_CLEARDEFENSE:
			processAdminClearDefense(e);
			break;
		case EVENT_ADMIN_SETNEWCOMERGIFT:
			processAdminSetNewComerGift(e);
			break;
		case  EVENT_ADMIN_SETCONTINUELANDDAY:
			processAdminSetContinueLandDay(e);
			break;
		case EVENT_ADMIN_SETPLATINVITEID:
			processAdminSetPlatInviteId(e);
			break;
		case EVENT_ADMIN_GETBLDCNT:
			processAdminGetBldCnt(e);
			break;
		case EVENT_ADMIN_RELOADCHICKENRANK:
			processAdminReloadChickenRank(e);
			break;
		case EVENT_ADMIN_ADDGUILDBTLREWARD:
			processAdminAddGuildBtlReward(e);
			break;
		case EVENT_ADMIN_RESETWHOLECOUNTRY:
			processAdminResetWholeCountry(e);
			break;
		case EVENT_ADMIN_GVELIST:
			processAdminGveList(e);
			break;
        case EVENT_ADMIN_RESETGUILDPKSCORE:
            processAdminResetGuildPKScore(e);
            break;
        case EVENT_ADMIN_OUTPUTGUILDUSERLIST:
            processAdminOutputGuildUserList(e);
            break;
#endif
        default:
            break;
    }
}

#if 0
void DealAdminEvent::processAdminProfile(Event* e)
{
	if(e->state() == Admin_AG_Req)
	{
		if(!e->has_adminprofile_req())
		{
			return;
		}
		Admin_Profile_Req* req = e->mutable_adminprofile_req();
		int cmd = req->cmd();
		if(cmd == 1) // report
		{
			eh_->getEventHandler()->reportProfile();
		}
		else if(cmd == 2) // clear
		{
			eh_->getEventHandler()->clearProfile();
		}
	}
}

void DealAdminEvent::processAdminAddExp(Event* e)
{
/*    char buf[1024];
    if (e->state() == Admin_AG_Req)
    {
        if (!e->has_adminaddexp_req())
        {
            return;
        }
        Admin_AddExp_Req* req = e->mutable_adminaddexp_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            e->set_state(Admin_GW_Req);
            eh_->sendEventToWorld(e);
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }
        int oldlevel = pUser->GetPlayer()->GetLevel();
        int oldexp = pUser->GetPlayer()->GetExp();
        pUser->GetPlayer()->AddExp(req->exp());
        int newlevel = pUser->GetPlayer()->GetLevel();
        int newexp = pUser->GetPlayer()->GetExp();

        dh->markUserDirty(pUser);

        if (pUser->fd() != 0 && pUser->Online())
        {
            string text;
            if (newlevel != oldlevel)
            {
//                 BseUserData data;
//                 pUser->GetPlayer()->FillUserData(&data);
//                 data.SerializeToString(&text);
//                 eh_->sendDataToUser(pUser->fd(), S2C_BseUserData, text);
            }

            if (newexp != oldexp)
            {
//                 BseUserExData exdata;
//                 pUser->GetPlayer()->FillUserExData(&exdata);
//                 exdata.SerializeToString(&text);
//                 eh_->sendDataToUser(pUser->fd(), S2C_BseUserExData, text);
            }
        }

        sprintf(buf, "uid=%lld\r\noldlevel=%d\r\noldexp=%d\r\nnewlevel=%d\r\nnewexp=%d\r\n",
                nUserID, oldlevel, oldexp, newlevel, newexp);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(req->adminfd(), buf, strlen(buf));
    }
    else if (e->state() == Admin_WG_Req)
    {
        if (!e->has_adminaddexp_req())
        {
            return;
        }
        Admin_AddExp_Req* req = e->mutable_adminaddexp_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }
        int oldlevel = pUser->GetPlayer()->GetLevel();
        int oldexp = pUser->GetPlayer()->GetExp();
        pUser->GetPlayer()->AddExp(req->exp());
        int newlevel = pUser->GetPlayer()->GetLevel();
        int newexp = pUser->GetPlayer()->GetExp();

        dh->markUserDirty(pUser);

        if (pUser->fd() != 0 && pUser->Online())
        {
            string text;
            if (newlevel != oldlevel)
            {
//                 BseUserData data;
//                 pUser->GetPlayer()->FillUserData(&data);
//                 data.SerializeToString(&text);
//                 eh_->sendDataToUser(pUser->fd(), S2C_BseUserData, text);
            }

            if (newexp != oldexp)
            {
//                 BseUserExData exdata;
//                 pUser->GetPlayer()->FillUserExData(&exdata);
//                 exdata.SerializeToString(&text);
//                 eh_->sendDataToUser(pUser->fd(), S2C_BseUserExData, text);
            }
        }

        Admin_AddExp_Rsp* rsp = e->mutable_adminaddexp_rsp();
        rsp->set_gid(req->gid());
        rsp->set_uid(nUserID);
        rsp->set_adminfd(req->adminfd());
        rsp->set_oldexp(oldexp);
        rsp->set_oldlevel(oldlevel);
        rsp->set_newexp(newexp);
        rsp->set_newlevel(newlevel);
		rsp->set_fromweb(req->fromweb());
        e->clear_adminaddexp_req();
        e->set_state(Admin_GW_Rsp);
        eh_->sendEventToWorld(e);
    }
    else if (e->state() == Admin_WG_Rsp)
    {
        if (!e->has_adminaddexp_rsp())
        {
            return;
        }
        const Admin_AddExp_Rsp& rsp = e->adminaddexp_rsp();
        int gid = rsp.gid();
        if (gid != eh_->GetSrvID())
        {
            return;
        }
        int64 nUserID = rsp.uid();
        int oldlevel = rsp.oldlevel();
        int oldexp = rsp.oldexp();
        int newlevel = rsp.newlevel();
        int newexp = rsp.newexp();

        sprintf(buf, "uid=%lld\r\noldlevel=%d\r\noldexp=%d\r\nnewlevel=%d\r\nnewexp=%d\r\n",
                nUserID, oldlevel, oldexp, newlevel, newexp);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
    }
    else
    {
        LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
    }
*/
}

void DealAdminEvent::processAdminCanAttack(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_admincanattack_req())
		{
			return;
		}
		Admin_CanAttack_Req* req = e->mutable_admincanattack_req();
		int64 nUserID = req->uid();
		string strPlatID = req->platid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
			gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID, &state, true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		pUser->SetAdminCanAttack();
		pUser->SetGameStarCache(pUser->GetPlattype(),dh);

		dh->markUserDirty(pUser);

		char buf[1024];
		sprintf(buf, "uid=%lld\r\ncanattack!!!!\r\n",nUserID);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->adminfd(), buf, strlen(buf));
	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_admincanattack_req())
		{
			return;
		}
		Admin_CanAttack_Req* req = e->mutable_admincanattack_req();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID, &state, true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		pUser->SetAdminCanAttack();
		pUser->SetGameStarCache(pUser->GetPlattype(),dh);

		dh->markUserDirty(pUser);

		Admin_CanAttack_Rsp* rsp = e->mutable_admincanattack_rsp();
		rsp->set_gid(req->gid());
		rsp->set_uid(nUserID);
		rsp->set_adminfd(req->adminfd());
		rsp->set_fromweb(req->fromweb());
		e->clear_adminaddcredit_req();
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);

	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_admincanattack_rsp())
		{
			return;
		}
		const Admin_CanAttack_Rsp& rsp = e->admincanattack_rsp();
		int gid = rsp.gid();
		if (gid != eh_->GetSrvID())
		{
			return;
		}


		char buf[1024];
		sprintf(buf, "uid=%lld\r\ncanattack\r\n",rsp.uid());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}

}

void DealAdminEvent::processAdminAddHonor(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adminaddhonor_req())
		{
			return;
		}
		Admin_AddHonor_Req* req = e->mutable_adminaddhonor_req();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}
		int64 oldgold = pUser->GetPlayer()->GetHonor();
		pUser->GetPlayer()->AddHonor(req->honor(),HONOR_TYPE_DEF_ADMIN);
		int64 newgold = pUser->GetPlayer()->GetHonor();
		dh->markUserDirty(pUser);

		if (pUser->fd() != 0 && pUser->Online() && newgold != oldgold)
		{
			RseUserInfo userinfo;
			pUser->FillUserInfo(userinfo);
			/*userinfo.set_credit(toString(pUser->GetPlayer()->GetCredit()));
			userinfo.set_honor(toString(pUser->GetPlayer()->GetHonor()));
			userinfo.set_metal(toString(pUser->GetPlayer()->GetResCnt(RC_Metal)));
			userinfo.set_oil(toString(pUser->GetPlayer()->GetResCnt(RC_Oil)));*/
		
			string text;
			userinfo.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseUserInfo, text);
		}

		char buf[1024];
		sprintf(buf, "uid=%lld\r\noldhonor=%lld\r\nnewhonor=%lld\r\n",
			nUserID, oldgold, newgold);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->adminfd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adminaddhonor_req())
		{
			return;
		}
		Admin_AddHonor_Req* req = e->mutable_adminaddhonor_req();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;

		gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		int64 oldgold = pUser->GetPlayer()->GetHonor();
		pUser->GetPlayer()->AddHonor(req->honor(),HONOR_TYPE_DEF_ADMIN);
		int64 newgold = pUser->GetPlayer()->GetHonor();

		dh->markUserDirty(pUser);

		if (pUser->fd() != 0 && pUser->Online() && newgold != oldgold)
		{
			RseUserInfo userinfo;
			pUser->FillUserInfo(userinfo);

			string text;
			userinfo.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseUserInfo, text);

		}

		Admin_AddHonor_Rsp* rsp = e->mutable_adminaddhonor_rsp();
		rsp->set_gid(req->gid());
		rsp->set_uid(nUserID);
		rsp->set_adminfd(req->adminfd());
		rsp->set_oldhonor(oldgold);
		rsp->set_newhonor(newgold);
		rsp->set_fromweb(req->fromweb());
		rsp->set_sid(req->sid());
		e->clear_adminaddhonor_req();
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);

		eh_->SendUserCredit(pUser);

	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adminaddhonor_rsp())
		{
			return;
		}
		const Admin_AddHonor_Rsp& rsp = e->adminaddhonor_rsp();
		int gid = rsp.gid();
		if (gid != eh_->GetSrvID())
		{
			return;
		}
		int64 nUserID = rsp.uid();
		int64 oldgold = rsp.oldhonor();
		int64 newgold = rsp.newhonor();

		char buf[1024];
		sprintf(buf, "uid=%lld\r\noldhonor=%lld\r\nnewhonor=%lld\r\n",
			nUserID, oldgold, newgold);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}
#endif

void DealAdminEvent::processAdminAddCash(Event* e)
{
	if (e->state() == Admin_AG_Req)
    {
        if (!e->has_adminaddcashreq())
        {
            return;
        }
        Admin_AddCash_Req* req = e->mutable_adminaddcashreq();
        int64 nUserID = req->uid();
		string strPlatID = req->platid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
			gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
        {
            e->set_state(Admin_GW_Req);
            eh_->sendEventToWorld(e);
            return;
        }

        LoadStatus state;
        User* pUser = NULL;
		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
			pUser = dh->getUser(nUserID,&state,true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

		int oldCash = pUser->GetPlayer()->GetRes(RC_Cash);
		pUser->GetPlayer()->CostRes(RC_Cash, req->cash());
		int newcash = pUser->GetPlayer()->GetRes(RC_Cash);
        dh->markUserDirty(pUser);

        char buf[1024];
        sprintf(buf, "uid=%lld\r\noldgold=%lld\r\nnewgold=%lld\r\n",
                nUserID, oldCash, newcash);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(req->adminfd(), buf, strlen(buf));
    }
#if 0
    else if (e->state() == Admin_WG_Req)
    {
        if (!e->has_adminaddcredit_req())
        {
            return;
        }
        Admin_AddCredit_Req* req = e->mutable_adminaddcredit_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
		string strPlatID = req->platid();

		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
			gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        LoadStatus state;
		User* pUser = NULL;

		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
			pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		Player* pPlayer = pUser->GetPlayer();
		if (pPlayer == NULL)
		{
			return;
		}

        Admin_AddCredit_Rsp* rsp = e->mutable_adminaddcredit_rsp();
        rsp->set_gid(req->gid());
        rsp->set_uid(nUserID);
        rsp->set_adminfd(req->adminfd());
        rsp->set_fromweb(req->fromweb());
        rsp->set_type(req->type());
        rsp->set_sid(req->sid());
        rsp->set_istxcross(req->istxcross());
        rsp->set_platid(req->platid());
        rsp->set_nchannel(req->nchannel());
        rsp->set_orderid(req->orderid());
        rsp->set_nprepayflag(req->nprepayflag());

        if (req->nprepayflag() > 0)
        {
            bool bResult = pPlayer->HandlePrePay(req->nchannel(), req->nprepayflag(), req->orderid(), req->credit());
            if (req->nprepayflag() == 1 || !bResult)
            {
                rsp->set_oldcredit(0);
                rsp->set_newcredit(0);
                rsp->set_bsuc(bResult);
                e->clear_adminaddcredit_req();
                e->set_state(Admin_GW_Rsp);
                eh_->sendEventToWorld(e);
                return;
            }
        }

        int64 oldgold = pUser->GetPlayer()->GetCredit();
		int64 nOldtotalvalue = pUser->GetDbUser().player().totalpercharge();
        CREDIT_TYPE_DEF nAddType = (CREDIT_TYPE_DEF)req->type();
        if (req->nchannel() == CREDIT_BUY_FROM_YEEPAY)
        {
            nAddType = CREDIT_TYPE_DEF_BUY_YEEPAY;
        }
        pPlayer->AddCredit(req->credit(), nAddType);
		int64 nNewtotalvalue = pUser->GetDbUser().player().totalpercharge();

		if ( req->type() == CREDIT_TYPE_DEF_BUY || req->type() == CREDIT_TYPE_DEF_BUY_QPOINT)
		{
            if (req->sid() != "admin")
            {
                HttpRequireHandler::Instance().SafePushHttpAddCreditToQQ(pUser, pUser->GetPlattype(), req->credit());
            }
			//判断每日任务权限(充值)
			if ( pPlayer->SetCreditPrivailege( req->credit() ) )
			{
				RseDailyTask rseRet;

				pPlayer->FillRseDailyTask(&rseRet);

				string text;
				rseRet.SerializeToString(&text);

				eh_->sendDataToUser(pUser->fd(), S2C_RseDailyTask, text);
			}
			
			//圣诞节任务- 充值
			if(!pPlayer->CheckChristmasTaskComplete(CT_RECHARGE))
			{
				pPlayer->ChristmasTaskComplete(CT_RECHARGE);
			}

			//充值礼物、奖励相关

			time_t tNow = time(NULL);
            if (req->type() == CREDIT_TYPE_DEF_BUY_QPOINT)
            {//每日首冲
                Admin_DailyFirstCost(pPlayer, pUser);
            }

			for ( int i = 0; i < CostGiftCfg::Instance().GetTblNum(); i++ )
			{
				const CostGiftTbl* pCostGiftTbl = CostGiftCfg::Instance().GetCostGiftTblByNum( i );

				if ( pCostGiftTbl == NULL )
				{
					continue;
				}

				if ( pCostGiftTbl->m_nCostCredit == 0 || !pCostGiftTbl->IsCurrentTime())
				{
					continue;
				}

				DB_CostGift* pDBCostGift = pPlayer->GetDBCostGift( pCostGiftTbl->m_nGiftID );
				if ( pDBCostGift == NULL )
				{
					pDBCostGift = pPlayer->GetDBPlayer()->add_costgift();
					pDBCostGift->set_id( pCostGiftTbl->m_nGiftID );
					pDBCostGift->set_lasttimes((int)tNow);
				}

				//需要先抽奖，然后充值
				if ( pDBCostGift->drawtimes() < pCostGiftTbl->m_nDrawTimes )
				{
					continue;
				}

				pDBCostGift->set_credit( pDBCostGift->credit() + req->credit() );
				if ( pCostGiftTbl->m_nType == COST_GIFT_TYPE_DATE )
				{
					pDBCostGift->set_lasttimes((int)tNow);
				}

				if ( ( pCostGiftTbl->IsCurrentCanGainGift( req->credit(),pDBCostGift->yellowtimes(),pDBCostGift->drawtimes(),pDBCostGift->admiraltimes() ) && !pCostGiftTbl->m_bNeedGet && !pCostGiftTbl->m_bCostCreditPlus) ||
					 ( pCostGiftTbl->IsCurrentCanGainGift( pDBCostGift->credit(),pDBCostGift->yellowtimes(),pDBCostGift->drawtimes(),pDBCostGift->admiraltimes() ) && !pCostGiftTbl->m_bNeedGet && pCostGiftTbl->m_bCostCreditPlus) )//判断是否可以领取(可累计,不可累积)
				{
					//可以获取奖励
					
					//判断领取礼物数量
					if ( pDBCostGift->count() < pCostGiftTbl->m_nTimes || pCostGiftTbl->m_nTimes == 0 )
					{
						//礼物
						int medel = (int)(pCostGiftTbl->m_nMedal);
						int oil = (int)(pCostGiftTbl->m_nOil);
						int64 nAddMetal = pPlayer->AddRes(RC_Metal,medel);
						int64 nAddOil = pPlayer->AddRes(RC_Oil,oil);
						//int nAddBasePoints = (int)ceil(double(medel+oil)/50); 

						if ( pCostGiftTbl->m_nGetCredit > 0 )
						{
							pPlayer->AddCredit( pCostGiftTbl->m_nGetCredit,CREDIT_TYPE_DEF_YELLOW );
						}

						pPlayer->AddHonor(pCostGiftTbl->m_nHonor,HONOR_TYPE_DEF_GIFT);
						pPlayer->AddFeats(pCostGiftTbl->m_nFeats,FEATS_TYPE_DEF_QPLUS);

						RseUserInfo userinfo;

						string text;
						pUser->FillUserInfo(userinfo); 
						userinfo.SerializeToString(&text);
						eh_->sendDataToUser(pUser->fd(), S2C_RseUserInfo, text);

						text.clear();

						BaseManager* pBaseManager = pPlayer->GetBaseManager();

						if ( pBaseManager == NULL )
						{
							break;
						}

						//pBaseManager->AddBasePoints(nAddBasePoints);
						//pPlayer->CheckUpgrade();

						//SYS_LOG(m_pUser->GetUid(),LT_FinishTask,0,0,nID<<pGiftTbl->m_nMedal<<pGiftTbl->m_nOil);

						RseBuildingUpdate rseUP;
						rseUP.set_type(7);
						for(int i=0;i<MAX_COST_GIFT_ARMY_CNT;i++)
						{
							int nArmyID = pCostGiftTbl->m_lstArmyType[i];
							if(nArmyID>0)
							{
								for(int j=0;j<pCostGiftTbl->m_lstArmyCnt[i];j++)
								{
									DB_Army* pDBArmy = pBaseManager->CreateArmyByTask(nArmyID);
									if(pDBArmy)
									{
										Army* pArmy = rseUP.add_armylst();
										pBaseManager->FillArmyData(pDBArmy,pArmy);
										SYS_LOG(pUser->GetUid(),LT_COST_GIFT_ADDARMY,0,0,nArmyID);
									}
								}

							}
						}

						if(rseUP.armylst_size()>0)
						{
							text.clear();
							rseUP.SerializeToString(&text);
							eh_->sendDataToUser(pPlayer->GetParent()->fd(), S2C_RseBuildingUpdate, text);
						}

						for(int i=0;i<MAX_COST_GIFT_ITEM_CNT;i++)
						{
							int nItemID = pCostGiftTbl->m_lstItemType[i];
							if(nItemID>0)
							{
								pPlayer->AddBagItem( nItemID, pCostGiftTbl->m_lstItemCnt[i] );
							}
						}
						eh_->SendBagList(pUser);

						const AdmiralTbl* pAdmTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl( pCostGiftTbl->m_nAdmiralID );
						if(pAdmTbl!=NULL)
						{
							DB_Admiral* pDBAdm = pBaseManager->GetDBAdmiral( pCostGiftTbl->m_nAdmiralID );
							if(pDBAdm==NULL)
							{// 添加将领
								pDBAdm = pBaseManager->AddAdmiral( pCostGiftTbl->m_nAdmiralID );
								if(pDBAdm==NULL)
									return;
								if( pCostGiftTbl->m_nAdmiralLV > 1 )
								{
									pDBAdm->set_level( pCostGiftTbl->m_nAdmiralLV );
									pDBAdm->set_exp(AdmiralInfoCfg::Instance().GetUpgradeNeedExp( pCostGiftTbl->m_nAdmiralLV )+10);
								}
								pBaseManager->CacuAdmiralFinalProp(pDBAdm);
								SYS_LOG(pUser->GetUid(),LT_AddAdmiralCostGift,0,0,pDBAdm->admiralid()<<pDBAdm->level());
							}
							else
							{
								if(pAdmTbl->m_nQuality == 4)
								{
									DB_AdmiralTrainLV* pDBTrainLV = pDBAdm->mutable_trainlv();
									pDBTrainLV->set_cnt(pDBTrainLV->cnt()+1);
									int nLV = AdmiralInfoCfg::Instance().CacuTrainSkillLV(pDBTrainLV->cnt());
									if(pDBTrainLV->level()!=nLV)
									{
										pDBTrainLV->set_level(nLV);
									}
									pBaseManager->CacuAdmiralFinalProp(pDBAdm,pAdmTbl);
									if(pDBAdm->usetype())
									{
										pBaseManager->UpdateAdmiralEffect();
										pBaseManager->UpdateArmyHP();
										pBaseManager->UpdateBldHP();
										pBaseManager->UpdateAllHeroHP();
									}
									SYS_LOG(pUser->GetUid(),LT_AdmiralCostGiftTrain,0,0,pCostGiftTbl->m_nAdmiralID<<1);
								}
							}
							
						}
						
						//记录数量
						pDBCostGift->set_count( pDBCostGift->count() + 1 );
						pDBCostGift->set_credit( 0 );
						SYS_LOG(pUser->GetUid(), LT_COST_GIFT_GET, 0, 0,pDBCostGift->id()<<pCostGiftTbl->m_nGiftID);
					}
				}
			}

			const DailyCostTbl* pCostGiftTblDaily = DailyCostCfg::Instance().GetDailyCostTblByDate( time(NULL) );
			//const AdmiralExchangeTbl* pAETbl = AdmiralInfoCfg::Instance().GetAdmiralExchange();
			if( pCostGiftTblDaily != NULL )
			//if(pCostGiftTblDaily != NULL || pAETbl != NULL)
			{
				time_t now = time(NULL);
				bool rightTime = false;
				struct tm tmNow = *localtime(&now);
				DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
				if ( pDBPlayer == NULL )
				{
					return;
				}

				if ( now >= pCostGiftTblDaily->m_tBeginTime && now <= pCostGiftTblDaily->m_tEndTime )
				//if (pCostGiftTblDaily != NULL && now >= pCostGiftTblDaily->m_tBeginTime && now <= pCostGiftTblDaily->m_tEndTime )
				{
					DB_DailyCost* pDailyCost = NULL;
					for ( int j = 0; j < pDBPlayer->dailycosts_size(); j++ )
					{
						pDailyCost = pDBPlayer->mutable_dailycosts(j);
						if ( pDailyCost->costyday() == tmNow.tm_yday )
						{
							break;
						}
						pDailyCost = NULL;
					}

					if ( pDailyCost == NULL )
					{
						pDailyCost = pDBPlayer->add_dailycosts();
						pDailyCost->set_costyday(tmNow.tm_yday);
						pDailyCost->set_costmonth(tmNow.tm_mon);
						pDailyCost->set_costday(tmNow.tm_mday);
						pDailyCost->set_costcredit(0);
					}

					int nLastCredit = pDailyCost->costcredit();
					pDailyCost->set_costcredit( pDailyCost->costcredit() + req->credit() );
					SYS_LOG(pUser->GetUid(), LT_DAILYCOST_CREDIT, 0, 0,nLastCredit<<pDailyCost->costcredit());

					int nLastToken = pDailyCost->costtoken();

// 					ConstantSetUnit* pUnit = GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_DEF_DAILY_COST);
// 					if(pUnit != NULL)
// 					{
// 
// 					}

					if ( pDailyCost->costcredit() >= pCostGiftTblDaily->m_costCredit[2] )
					{
						pDailyCost->set_costtoken( 3 );
					}

					else if ( pDailyCost->costcredit() >= pCostGiftTblDaily->m_costCredit[1] )
					{
						pDailyCost->set_costtoken( 2 );
					}

					else if ( pDailyCost->costcredit() >= pCostGiftTblDaily->m_costCredit[0]  )
					{
						pDailyCost->set_costtoken( 1 );
					}

					//统计
					for ( int k = nLastToken; k < pDailyCost->costtoken(); k++ )
					{
						SYS_UserStat(pUser,false,"DailyCost",1,"","", k+1 );
						SYS_LOG(pUser->GetUid(), LT_DAILYCOST_TOKEN, 0, 0,k+1);
					}

					pPlayer->GetDailyCostGift();
				}
				//else
				//{
				//	//将魂兑换也要每日充值,如果上面达人活动没记,再加上
				//	if (pAETbl != NULL && now >= pAETbl->m_tStart && now < pAETbl->m_tEnd)
				//	{
				//		DB_DailyCost* pDailyCost = NULL;
				//		for ( int j = 0; j < pDBPlayer->dailycosts_size(); j++ )
				//		{
				//			pDailyCost = pDBPlayer->mutable_dailycosts(j);
				//			if ( pDailyCost->costyday() == tmNow.tm_yday )
				//			{
				//				break;
				//			}
				//			pDailyCost = NULL;
				//		}

				//		if ( pDailyCost == NULL )
				//		{
				//			pDailyCost = pDBPlayer->add_dailycosts();
				//			pDailyCost->set_costyday(tmNow.tm_yday);
				//			pDailyCost->set_costmonth(tmNow.tm_mon);
				//			pDailyCost->set_costday(tmNow.tm_mday);
				//			pDailyCost->set_costcredit(0);
				//		}

				//		int nLastCredit = pDailyCost->costcredit();
				//		pDailyCost->set_costcredit( pDailyCost->costcredit() + req->credit() );
				//		SYS_LOG(pUser->GetUid(), LT_DAILYCOST_CREDIT, 0, 0,nLastCredit<<pDailyCost->costcredit());
				//	}
				//}
			}

			//新增每日充值单独记录
			DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
			if (pDBPlayer != NULL)
			{
				pDBPlayer->set_dailycredit(pDBPlayer->dailycredit() + req->credit());
			}

			RseCostGiftMsg rseCostGiftMsg;
			pPlayer->FillRseCostGiftMsg( &rseCostGiftMsg );

			string text;
			rseCostGiftMsg.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseCostGiftMsg, text);

			RseDaliyCostMsg rseDailyCostMsg;
			pPlayer->FillRseDailyCostMsg( &rseDailyCostMsg );

			text.clear();
			rseDailyCostMsg.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseDaliyCostMsg, text);
			
			const CostGiftTbl* pCostGiftTbl = CostGiftCfg::Instance().GetCostGiftTbl( 10060 );
			if( pCostGiftTbl != NULL )
			{
				//充值排行榜记录
				//time_t tBeginTime = GetDayTimeByStr( pConstantSetUnit->m_strArgs[0].c_str(), pConstantSetUnit->m_strArgs[1].c_str() );
				//time_t tEndTime   = GetDayTimeByStr( pConstantSetUnit->m_strArgs[2].c_str(), pConstantSetUnit->m_strArgs[3].c_str() );

				if ( pPlayer->GetDBPlayer()->lastcosttime() <= pCostGiftTbl->m_tBeginTime )
				{
					//清除上次数据
					pPlayer->GetDBPlayer()->set_playercost( 0 );
				}
				time_t now = time(NULL);
				bool rightTime = false;

				if ( now >= pCostGiftTbl->m_tBeginTime && now <= pCostGiftTbl->m_tEndTime )
				{
					pPlayer->GetDBPlayer()->set_playercost( pPlayer->GetDBPlayer()->playercost() + req->credit() );

					if ( pPlayer->GetDBPlayer()->playercost() > 50000 )
					{
						SYS_UserStat4WebRank(pUser,"PlayCostYuandan","",pPlayer->GetDBPlayer()->playercost());
					} 
					pPlayer->GetDBPlayer()->set_lastcosttime( now );
				}
			}

			pPlayer->AddStCredit(req->credit());
		}
        //新手第一次充值
		if(nOldtotalvalue <= 0 && nNewtotalvalue > 0 )
		{
			if(pUser->isNewComer7Day() == true)
			{
				pUser->GetPlayer()->GainNewComerGift_MarkGift(NEWCOMERGIFTCFG_LOGIC_TYPE_LEVEL(NEWCOMERGIFT_ADD_TYPE_MARKGIFT, 1));
			}

			if(pUser->GetPlayer()->IsTXMissionMarked("100616028T2201210170001") == true )
			{
				req->set_istxcross(true);
			}
			else
			{
				req->set_istxcross(false);
			}
		}

        int64 newgold = pUser->GetPlayer()->GetCredit();
		if (req->type() == 1 && nOldtotalvalue<5000 && nNewtotalvalue>=5000)
		{
			pUser->SetGameStarCache(pUser->GetPlattype(),dh);
			eh_->SendUserInfo2Star(pUser);
		}
        dh->markUserDirty(pUser);

        if (pUser->fd() != 0 && pUser->Online() && newgold != oldgold)
        {
			RseCredit rseCredit;
			rseCredit.set_credit(toString(pUser->GetPlayer()->GetCredit()));
			rseCredit.set_type(1);
			rseCredit.set_error(0);

			string text;
			rseCredit.SerializeToString(&text);
			eh_->sendDataToUser(pUser->fd(), S2C_RseCredit, text);

        }

        rsp->set_oldcash(oldgold);
        rsp->set_newcash(newgold);
        rsp->set_bsuc(true);
        e->clear_adminaddcredit_req();
        e->set_state(Admin_GW_Rsp);
        eh_->sendEventToWorld(e);
		eh_->SendUserCredit(pUser);
    }
#endif
    else if (e->state() == Admin_WG_Rsp)
    {
        if (!e->has_adminaddcashrsp())
        {
            return;
        }
        const Admin_AddCash_Rsp& rsp = e->adminaddcashrsp();
        int gid = rsp.gid();
        if (gid != eh_->GetSrvID())
        {
            return;
        }
        int64 nUserID = rsp.uid();
        int64 oldgold = rsp.oldcash();
        int64 newgold = rsp.newcash();

        char buf[1024];
        sprintf(buf, "uid=%lld\r\noldgold=%lld\r\nnewgold=%lld\r\n",
                nUserID, oldgold, newgold);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
    }
    else
    {
        LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
    }
}

#if 0
//每日首冲
void DealAdminEvent::Admin_DailyFirstCost(Player* pPlayer, User *pUser)
{
    DB_Player *pDBPlayer = pPlayer->GetDBPlayer();
    if (NULL == pDBPlayer)
    {
        return;
    }
    //每日首冲判断
    const QPointShopItemTbl* pTbl = QPointShopItemCfg::Instance().GetQPointShopTbl( 1034 );
    if (NULL == pTbl || !pTbl->IsCorrectTime())
    {
        return;
    }
    int nGiftID = pTbl->m_nFirstGift;
    if (nGiftID > 0 && !pDBPlayer->isgetfirstcost())
    {
        pDBPlayer->set_isgetfirstcost(true);
        DB_DailyCost *pDailyCost = NULL;
        time_t now = time(NULL);
        struct tm nowtime = *localtime(&now);

        for ( int j = 0; j < pDBPlayer->dailyfirstcosts_size(); j++ )
        {
            pDailyCost = pDBPlayer->mutable_dailyfirstcosts(j);
            if ( pDailyCost->costyday() == nowtime.tm_yday )
            {
                break;
            }
            pDailyCost = NULL;
        }

        if ( pDailyCost == NULL )
        {
            pDailyCost = pDBPlayer->add_dailyfirstcosts();
            pDailyCost->set_costyday(nowtime.tm_yday);
            pDailyCost->set_costmonth(nowtime.tm_mon);
            pDailyCost->set_costday(nowtime.tm_mday);
            pDailyCost->set_costcredit(1);

            pPlayer->AddBagItem( nGiftID, 1);
            if (pDBPlayer->dailyfirstcosts_size() == pTbl->m_nDailyCost)
            {//终极礼包
                pPlayer->AddBagItem( 30044, 1);
            }
            eh_->SendBagList(pUser);
        }
    }
}

void DealAdminEvent::processAdminResetGuildPKScore(Event *e)
{
    GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
    if (pNeedArgs == NULL)
    {
        return;
    }
    eh_->sendEventToCountry(e, pNeedArgs->region());
}

void DealAdminEvent::processAdminOutputGuildUserList(Event *e)
{
    GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
    if (pNeedArgs == NULL)
    {
        return;
    }
    eh_->sendEventToCountry(e, pNeedArgs->region());
}

bool DealAdminEvent::Admin_DealMsg(Event* e,User* pUser,Admin_AdminOpt_Req* req,string& strCmd)
{
	if(e == NULL || pUser == NULL || req == NULL)
	{
		return false;
	}


	bool bSuc = false;
	switch(req->opttype())
	{
	case AOT_ADMIN_ADDRES:
		{
			bSuc = Admin_AddRes(pUser,req->res1(),req->res2());
			strCmd = "addres";
		}			
		break;
	case AOT_ADMIN_ADDARMY:
		{
			bSuc = Admin_AddArmy(pUser,req->armytype(),req->count());
			strCmd = "addarmy";
		}
		break;
	case AOT_ADMIN_DELARMY:
		{
			bSuc = Admin_DelArmy(pUser,req->armytype(),req->count());
			strCmd = "delarmy";
		}
		break;
	case AOT_ADMIN_ADDBLD:
		{
			bSuc = Admin_AddBld(pUser,req->bldtype(),req->level(),req->count());
			strCmd = "addbld";
		}
		break;
	case AOT_ADMIN_DELBLD:
		{
			bSuc = Admin_DelBld(pUser,req->bldid());
			strCmd = "delbld";
		}
		break;
	case AOT_ADMIN_SETBLDCNT:
		{
			bSuc = Admin_SetBldCnt(pUser,req->bldtype(),req->level(),req->count());
			strCmd = "setbldcnt";
		}
		break;
	case AOT_ADMIN_SETALLBLDLV:
		{
			bSuc = Admin_SetAllBldLV(pUser,req->bldtype(),req->level());
			strCmd = "setallbldlv";
		}
		break;
	case AOT_ADMIN_SETBLDLV:
		{
			bSuc = Admin_SetBldLV(pUser,req->bldid(),req->level());
			strCmd = "setbldlv";
		}
		break;
	case AOT_ADMIN_SETARMYLV:
		{
			bSuc = Admin_SetArmyLV(pUser,req->armytype(),req->level());
			strCmd = "setarmylv";
		}
		break;
	case AOT_ADMIN_SAVENPC:
		{
			bSuc = Admin_SaveNpc(pUser);
			strCmd = "addnpc";
		}
		break;
	case AOT_ADMIN_ADDNPC:
		{
			bSuc = Admin_AddNpc(pUser,req->npcid(),true);
			strCmd = "addnpc";
		}
		break;
	case AOT_ADMIN_DELNPC:
		{
			bSuc = Admin_AddNpc(pUser,req->npcid(),false);
			strCmd = "delnpc";
		}
		break;
	case AOT_ADMIN_ADDITEM:
		{
			bSuc = Admin_AddItem(pUser,req->itemtype(),req->count());
			strCmd = "additem";
		}
		break;
	case AOT_ADMIN_ADDEMBLEM:
		{
			bSuc = Admin_AddEmblem(pUser,req->emblemtype(),req->count());
			strCmd = "addemblem";
		}
		break;
	case AOT_ADMIN_ADDWPCCOM:
		{
			bSuc = Admin_AddWPCCOM(pUser,req->wpccomid(),req->count(),req->level());
			strCmd = "addwpccom";
		}
		break;
	case AOT_ADMIN_ADDFEATS:
		{
			bSuc = Admin_AddFeats(pUser,req->count());
			strCmd = "addfeats";
		}
		break;
	case AOT_ADMIN_ADDHERO:
		{
			bSuc = Admin_AddHero(pUser,req->admiralid());
			strCmd = "addhero";
		}
		break;
	case AOT_ADMIN_DELHERO:
		{
			bSuc = Admin_DelHero(pUser,req->admiralid());
			strCmd = "delhero";
		}
		break;
	case AOT_ADMIN_ADDLETTER:
		{
			bSuc = Admin_AddHeroLetter(pUser,req->admiralid(),req->count());
			strCmd = "addheroletter";
		}
		break;
	case AOT_ADMIN_DELLETTER:
		{
			bSuc = Admin_DelHeroLetter(pUser,req->admiralid(),req->count());
			strCmd = "delheroletter";
		}
		break;
	case AOT_ADMIN_ADDADMIRALLETTER:
		{
			bSuc = Admin_AddAdmiralLetter(pUser,req->admiralid(),req->count());
			strCmd = "addadmiralletter";
		}
		break;
	case AOT_ADMIN_DELADMIRALLETTER:
		{
			bSuc = Admin_DelAdmiralLetter(pUser,req->admiralid(),req->count());
			strCmd = "deladmiralletter";
		}
		break;
	case AOT_ADMIN_SETHEROHP:
		{
			bSuc = Admin_SetHero(pUser,req->admiralid(),req->bldtype(),req->count());
			strCmd = "sethero";
		}
		break;
	case AOT_ADMIN_RECOVERHEROHP:
		{
			bSuc = Admin_RecoverHeroHP(pUser);
			strCmd = "recoverherohp";
		}
		break;
	case AOT_ADMIN_ADDADMIRAL:
		{
			bSuc = Admin_AddAdmiral(pUser,req->admiralid(),req->level());
			strCmd = "addadmiral";
		}
		break;
	case AOT_ADMIN_SET_ADMIRALSKILLLEVEL:
		{
			bSuc = Admin_SetAdmiralSkill(pUser,req->admiralid(),req->level());
			strCmd = "setadmiralskill";
		}
		break;
	case AOT_ADMIN_DELADMIRAL:
		{
			bSuc = Admin_DelAdmiral(pUser,req->admiralid());
			strCmd = "deladmiral";
		}
		break;
	case AOT_ADMIN_ADDSOUL:
		{
			bSuc = Admin_AddAdmiralSoul(pUser,req->count());
			strCmd = "addsoul";
		}
		break;
	case AOT_ADMIN_ADDADMIRALTRAINEXP:
		{
			bSuc = Admin_AddAdmiralTrainExp(pUser,req->admiralid(),req->count());
			strCmd = "addadmiraltrainexp";
		}
		break;
	case AOT_ADMIN_DELBLDCNT:
		{
			bSuc = Admin_DelBldCnt(pUser,req->bldtype(),req->count());
			strCmd = "delbldcnt";
		}
		break;
	case AOT_ADMIN_RECOVERARMYHP:
		{
			bSuc = Admin_RecoverArmyHP(pUser);
			strCmd = "recoverarmyhp";
		}
		break;
	case AOT_ADMIN_RECOVERBLDHP:
		{
			bSuc = Admin_RecoverBldHP(pUser);
			strCmd = "recoverbldhp";
		}
		break;
	case AOT_ADMIN_SETINIT:
		{
			bSuc = Admin_SetInit(pUser);
			strCmd = "setinit";
		}
		break;
	case AOT_ADMIN_SETSCIENCELVL:
		{
			bSuc = Admin_SetScienceLvl(pUser,req->res1(),req->res2());
			strCmd = "setsciencelvl";
		}
		break;
	case AOT_ADMIN_SETGMTESTVALUE:
		{
			bSuc = Admin_SetGMTestValue(pUser,req->res1(),req->res2());
			if(bSuc == false)
			{
				e->set_userplat(pUser->GetPlatformId());
				e->set_state(Status_Game_To_Plat);
				eh_->sendEventToPlat(e,eh_->GetPlatSrvID(pUser->GetPlatformId()));
			}
			else
			{
				bSuc = true;
			}
			strCmd = "setgmtestvalue";
		}
		break;
	case AOT_ADMIN_ADDMOONCNT:
		{
			bSuc = Admin_AddMoonCnt(pUser,req->count());
			strCmd = "addmooncnt";
		}
		break;
	case AOT_ADMIN_ADDBOOK:
		{
			bSuc = Admin_AddBook(pUser,req->bookid(),req->count(),req->level());
			strCmd = "addbook";
		}
		break;
    case AOT_ADMIN_DELBOOKEXP:
        {
            bSuc = Admin_DelBookExp(pUser,req->bookid(),req->count());
            strCmd = "delbookExp";
        }
        break;
	case AOT_ADMIN_CLEARSTRATEGIC:
		{
			bSuc = Admin_ClearStrategic(pUser,req->itemtype());
			strCmd = "clearstrategic";
		}
		break;
	case AOT_ADMIN_PVESET_CUSTOM:
		{
			bSuc = Admin_PveSetCustom(pUser,req->itemtype(),req->bldtype(),req->armytype(),req->bldid());
			strCmd = "pvesetcustom";
		}
		break;
	case AOT_ADMIN_PVE_FIGHT_START:
		{
			int nTime = Admin_PveFightStart(pUser,req->itemtype(),req->bldtype());
			strCmd = strCmd + "pvefighttest use time :" + toString(nTime);
		}
		break;
	case AOT_ADMIN_ADMINAL_REINITDB:
		{
			bSuc = Admin_ReInitDB(pUser);
			strCmd = "reinitdb";
		}
		break;
	case AOT_ADMIN_ADDCOUPONS:
		{
			bSuc = Admin_AddCoupons(pUser,req->count());
			strCmd = "addcoupon";
		}
		break;
	case AOT_ADMIN_ADD_COUNTRYTHEW:
		{
			bSuc = Admin_AddThew(pUser,req->count());
			strCmd = "addthew";
		}
		break;
	case AOT_ADMIN_ADD_COUNTRYMORALE:
		{
			bSuc = Admin_AddMorale(pUser,req->count());
			strCmd = "addmorale";
		}
		break;
	case AOT_ADMIN_ADD_COUNTRYZHANGONG:
		{
			bSuc = Admin_AddZhangong(pUser,req->count());
			strCmd = "addzhangong";
		}
		break;
	case AOT_ADMIN_ADD_COUNTRYBOX:
		{
			bSuc = Admin_AddCountryBox(pUser,req->count());
			strCmd = "addcountrybox";
		}
		break;
	case AOT_ADMIN_ADD_GUILDSCORE:
		{
			bSuc = Admin_AddGuildScore(pUser,req->count());
			strCmd = "addguildscore";
		}
		break;
	case AOT_ADMIN_ADD_GUILDBLDVALUE:
		{
			bSuc = Admin_AddGuildBldValue(pUser,e);
			strCmd = "addguildbldvalue";
		}
		break;
	case AOT_ADMIN_SET_GUILDSCIENCELV:
		{
			bSuc = Admin_SetGuildScienceLV(pUser,req->scienceid(),req->level());
			strCmd = "setguildsciencelv";
		}
		break;
	case AOT_ADMIN_SET_GUILDBLDLV:
		{
			bSuc = Admin_SetGuildBldLV(pUser,e);
			strCmd = "setguildbldlv";
		}
		break;
	case AOT_ADMIN_COUNTRY_RELOADRANK:
		{
			bSuc = Admin_ReloadCountryRank();
			strCmd = "reloadcountryrank";
		}
		break;
	case AOT_ADMIN_ARENAFIGHTSET:
		{
			bSuc = Admin_ArenaFightSet(e,pUser,req->itemtype(),req->bldtype(),req->armytype());
			strCmd = "arenafightset";
		}
		break;
	case AOT_ADMIN_ADDADMIRALTIMES:
		{
			bSuc = Admin_AddAdmiralTimes(pUser,req->count());
			strCmd = "addadmiraltimes";
		}
		break;
	case AOT_ADMIN_ADD_EQUIP:
		{
			bSuc = Admin_AddEquip(pUser,req->equipid(),req->count());
			strCmd = "addequip";
		}
		break;
    case AOT_ADMIN_DEL_EQUIP:
        {
            bSuc = Admin_DelEquip(pUser,req->equipid());
            strCmd = "delequip";
        }
        break;
	case AOT_ADMIN_FINISHGUIDE:
		{
			bSuc = Admin_FinishGuide(pUser);
			strCmd = "finishguide";
		}
		break;
	case AOT_ADMIN_WORLDARENAFIGHTSET:
		{
			bSuc = Admin_WorldArenaSet(e,pUser,req->itemtype(),req->bldtype());
			strCmd = "worldarenaset";
		}
		break;
	case AOT_ADMIN_HEROARENAFIGHTSET:
		{
			bSuc = Admin_HeroArenaSet(e,pUser,req->itemtype(),req->bldtype());
			strCmd = "heroarenaset";
		}
		break;
	case AOT_ADMIN_REGIONARENAFIGHTSET:
		{
			bSuc = Admin_RegionArenaSet(e,pUser,req->itemtype(),req->bldtype());
			strCmd = "regionarenaset";
		}
		break;
	case AOT_ADMIN_CLEAR_PASSWORD:
		{
			bSuc = Admin_ClearPassword(pUser,req->count());
			strCmd = "clearpassword";
		}
		break;
	case AOT_ADMIN_SET_OFFICERPOSITION:
		{
			bSuc = Admin_SetOfficerPosition(e,pUser,req->itemtype(),req->bldtype());
			strCmd = "setofficer";
		}
		break;
	case AOT_ADMIN_SET_AddBuff:
		{
			bSuc = Admin_AddBuff(e,pUser,req->itemtype());
			strCmd = "addbuff";
		}
		break;
	case AOT_ADMIN_DELEMBLEM:
		{
			bSuc = Admin_DelEmblem(pUser,req->emblemtype(),req->count());
			strCmd = "delemblem";
		}
		break;
    case AOT_ADMIN_DELEMBLEMEXP:
        {
            bSuc = Admin_DelEmblemExp(pUser,req->emblemtype(),req->count());;
            strCmd = "delemblemexp";
        }
        break;
	case AOT_ADMIN_SET_VipLevel:
		{
			bSuc = Admin_SetVip(e,pUser,req->itemtype(),req->bldtype());
			strCmd = "setvip";
		}
		break;
	case AOT_ADMIN_SET_EQUIPSTRENGTHLEVEL:
		{
			bSuc = Admin_SetEquipStrLv(e,pUser,req->itemtype(),req->bldtype(),req->armytype());
			strCmd = "setequiplv";
		}
		break;
	case AOT_ADMIN_SPECIAL_VALUE:
		{
			bSuc = Admin_SpecialValue(e,pUser,req->count());
			strCmd = "specialvalue";
		}
		break;
    case AOT_ADMIN_CLEAR_NEWDAYINFO:
        {
            bSuc = Admin_ClearNewDayInfo(e,pUser);
            strCmd = "clearNewDayInfo";
        }
        break;
	case AOT_ADMIN_PVE_BATTLE_SET:
		{
			bSuc = Admin_PveBattleSet(e,pUser);
			strCmd = "pvebattleset";
		}
		break;
	case AOT_ADMIN_ADD_JUNLING:
		{
			bSuc = Admin_AddJunling(e,pUser);
			strCmd = "addjunling";
		}
		break;
    case AOT_ADMIN_ADD_POKERS:
        {
            bSuc = Admin_AddPokers(e,pUser);
            strCmd = "addpokers";
        }
        break;
    case AOT_ADMIN_RESET_POKEREXC:
        {
            bSuc = Admin_ResetPokerExc(e,pUser);
            strCmd = "resetpokerexc";
        }
        break;
    case AOT_ADMIN_RESET_DUELINFO:
        {
            bSuc = Admin_ResetDuelInfo(e,pUser);
            strCmd = "resetduelinfo";
        }
        break;
	case AOT_ADMIN_CLEAR_WHOLE_COUNTRY:
		{
			bSuc = Admin_ResetWholeCountry(e);
			strCmd = "resetwholecountry";
		}
		break;
	case AOT_ADMIN_WHOLE_COUNTRY_SIGN:
		{
			bSuc = Admin_WholeCountrySign(e,pUser);
			strCmd = "wholecountrysign";
		}
		break;
	case AOT_ADMIN_SETPROPERTY:
		{
			bSuc = Admin_SetProperty(pUser,req->admiralid(),req->count());
			strCmd = "setproperty";
		}
		break;
	case AOT_ADMIN_DELWPCCOM:
		{
			bSuc = Admin_DelWpccom(pUser,req->emblemtype(),req->count());
			strCmd = "delwpccom";
		}
		break;
    case AOT_ADMIN_DELWPCCOMEXP:
        {
            bSuc = Admin_DelWpccomExp(pUser,req->emblemtype(),req->count());
            strCmd = "delwpccomexp";
        }
        break;
	case AOT_ADMIN_ADD_GVETHEW:
		{
			bSuc = Admin_AddGveThew(pUser,req->count());
			strCmd = "addgvethew";
		}
		break;
	case AOT_ADMIN_ADD_GVEPOINT:
		{
			bSuc = Admin_AddGvePoint(pUser,req->count());
			strCmd = "addgvepoint";
		}
		break;
	case AOT_ADMIN_GVE_LIST:
		{
			bSuc = Admin_AddGuildBldValue(pUser,e);
			strCmd = "gvelist";
		}
		break;
    case AOT_ADMIN_CLEARBAGITEM:
        {
            bSuc = Admin_ClearAllBagItem(pUser);
            strCmd = "clearbagitem";
        }
        break;
    case AOT_ADMIN_ADD_RENTWPC:
        {
            bSuc = Admin_AddRentParts(e, pUser);
            strCmd = "addrentwpc";
        }
        break;
	case AOT_ADMIN_ADD_GUILDTASK:
		{
			bSuc = Admin_AddGuildTask(pUser , req->count());
			strCmd = "addguildtask";
		}
		break;
	case AOT_ADMIN_SET_GUILDTASK_PROGRESS:
		{
			bSuc = Admin_SetGuildTaskProgress(pUser , req->count());
			strCmd = "setguildtaskprogress";
		}
		break;
	case AOT_ADMIN_GVG_REWARD:
		{
			bSuc = Admin_GvgReward(e,pUser,req->count());
			strCmd = "gvgreward";
		}
		break;
	case AOT_ADMIN_SET_GUILDLEADER_DELTIME:
		{
			bSuc = Admin_SetGuildLeaderDelTime(pUser);
			strCmd = "setguildleaderdeltime";
		}
		break;
	case AOT_ADMIN_SET_GUILDLEADER:
		{
			bSuc = Admin_SetGuildLeader(pUser);
			strCmd = "setguildleader";
		}
		break;
	case AOT_ADMIN_ADD_SEARCHTIME:
		{
			bSuc = Admin_AddSearchTime(pUser,req->count());
			strCmd = "addsearchtime";
		}
		break;
	case AOT_ADMIN_DEL_ADMIRALSPECIAL:
		{
			bSuc = Admin_DelAdmiralSpecial(pUser,req->count());
			strCmd = "deladmiralspecial";
		}
		break;
	case AOT_ADMIN_DEL_HEROSPECIAL:
		{
			bSuc = Admin_DelHeroSpecial(pUser,req->count(),req->bldid());
			strCmd = "delherospecial";
		}
		break;
	case AOT_ADMIN_DEL_HEROEXPSPECIAL:
		{
			bSuc = Admin_DelHeroExpSpecial(pUser,req->count());
			strCmd = "delheroexpspecial";
		}
		break;
	case AOT_ADMIN_DEL_WPCCOMEXPSPECIAL:
		{
			bSuc = Admin_DelWpcExpSpecial(pUser,req->count());
			strCmd = "delwpccomexpspecial";
		}
		break;
	case AOT_ADMIN_DEL_EMBLEMEXPSPECIAL:
		{
			bSuc = Admin_DelEmblemExpSpecial(pUser,req->count());
			strCmd = "delemblemexpspecial";
		}
		break;
	case AOT_ADMIN_DEL_BOOKXPSPECIAL:
		{
			bSuc = Admin_DelBookExpSpecial(pUser,req->count());
			strCmd = "delbookexpspecial";
		}
		break;
	case AOT_ADMIN_ADD_DBROLLBACK:
		{
			bSuc = Admin_DBRoolBack(pUser);
			strCmd = "dbrollback";
		}
		break;
	case AOT_ADMIN_ADD_EXPWPC:
		{
			bSuc = Admin_AddExpWpc(pUser,req->count());
			strCmd = "addexpwpc";
		}
		break;
	case AOT_ADMIN_ADD_EXPEMBLEM:
		{
			bSuc = Admin_AddExpEmblem(pUser,req->count());
			strCmd = "addexpemblem";
		}
		break;
	case AOT_ADMIN_SET_WCPROMOTE_EXP:
		{
			bSuc = Admin_SetWCPromoteExp(pUser , req->count());
			strCmd = "setwcpromoteexp";
		}
		break;
	case AOT_ADMIN_SET_WCSKILL_EXP:
		{
			bSuc = Admin_SetWCSkillExp(pUser , req->itemtype() , req->count());
			strCmd = "setwcskillexp";
		}
		break;
	case AOT_ADMIN_ADD_WCSKILL_EXP:
		{
			bSuc = Admin_AddWCSkillExp(pUser , req->itemtype() , req->count());
			strCmd = "addwcskillexp";
		}
		break;
	case AOT_ADMIN_SUB_WCSKILL_EXP:
		{
			bSuc = Admin_SubWCSkillExp(pUser , req->itemtype() , req->count());
			strCmd = "subwcskillexp";
		}
		break;
	case AOT_ADMIN_WCOP_TYPE:
		{
			bSuc = Admin_WCOpType(pUser , req->itemtype() , req->count());
			strCmd = "wcoptype";
		}
		break;
	case AOT_ADMIN_WCOP_UID:
		{
			bSuc = Admin_WCOpUid(pUser , req->count());
			strCmd = "wcopuid";
		}
		break;
	case AOT_ADMIN_SADOP:
		{
			bSuc = Admin_SADOp(pUser , req->itemtype() , req->count());
			strCmd = "sadop";
		}
        break;
    case AOT_ADMIN_ADD_WARGAME_REPORT:
        {
            bSuc = Admin_AddReport(pUser , req->count());
            strCmd = "addreport";
        }
        break;
    case AOT_ADMIN_ADD_WARGAME_TOKEN:
        {
            bSuc = Admin_AddWarGameToken(pUser , req->count());
            strCmd = "addwargametoken";
        }
        break;
	case AOT_ADMIN_SET_STORMBOX:
		{
			bSuc = Admin_SetStormBox(pUser , req->count());
			strCmd = "setstormbox";
		}
		break;
	case AOT_ADMIN_SET_QQTENYEARFLAG:
		{
			bSuc = Admin_SetQQTenyearFlag(pUser , req->count());
			strCmd = "setqqtenyearflag";
		}
		break;
	case AOT_ADMIN_HAPPYLIB_AWARD:
		{
			bSuc = Admin_HappylibAward(pUser , req->count());
			strCmd = "happylibaward";
		}
		break;
    case AOT_ADMIN_MILITARYSOUL_POINT:
        {
            bSuc = Admin_MilitarySoulPoint(pUser , req->count());
            strCmd = "addmilitarysoulpoint";
        }
        break;
	case AOT_ADMIN_COUNTRYCHATUI:
		{
			bSuc = Admin_CountryChatUI(pUser);
			strCmd = "countrychatui";
		}
		break;
	case AOT_ADMIN_COUNTRYCHATMSG:
		{
			bSuc = Admin_CountryChatMsg(pUser , req->sid());
			strCmd = "countrychatmsg";
		}
		break;
	case AOT_ADMIN_COUNTRYCHATMASK:
		{
			bSuc = Admin_CountryChatMask(pUser , req->armytype() , req->res1() , req->sid());
			strCmd = "countrychatmask";
		}
		break;
    case AOT_ADMIN_SETSEVENDAYSLOGONDAYS:
        {
            bSuc = Admin_SetSevenDaysLogonDays(pUser , req->count());;
            strCmd = "setsevendaylogondays";
        }
        break;
    case AOT_ADMIN_ADDYASUIQIAN:
        {
            bSuc = Admin_AddYaSuiQian(pUser, req->count());
            strCmd = "addyasuiqian";
        }
        break;
	case AOT_ADMIN_TOWERHANCE:
		{
			bSuc = Admin_TowerEnhance(pUser , req->count() , req->itemtype());;
			strCmd = "towerenhance";
		}
		break;
	case AOT_ADMIN_SETTOWERHANCE:
		{
			bSuc = Admin_SetTowerEnhance(pUser , req->count() , req->itemtype());
			strCmd = "settowerenhance";
		}
		break;
	case AOT_ADMIN_NPCACTIVERANK:
		{
			bSuc = Admin_NpcactiveRank(e , pUser);
			strCmd = "npcactiverank";
		}
        break;
    case AOT_ADMIN_ADDJAMMER:
        {
            bSuc = Admin_AddJammer(pUser, req->itemtype(), req->level(), req->count());
            strCmd = "addjammer";
        }
		break;
	case AOT_ADMIN_BDNTIMES:
		{
			bSuc = Admin_SetBDNTimes(pUser , req->count());
			strCmd = "setbdntimes";
		}
		break;
	case AOT_ADMIN_SITETEST:
		{
			bSuc = Admin_SiteTestAdmin(pUser , req->count() , req->itemtype());
			strCmd = "sitetestadmin";
		}
		break;
	default:
		break;
	}
	if(bSuc)
	{
		
		switch(req->opttype())
		{
		case AOT_ADMIN_ADDARMY:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->armytype(),req->count());
			}
			break;
		case AOT_ADMIN_ADDITEM:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->itemtype(),req->count());
			}
			break;
		case AOT_ADMIN_ADDEMBLEM:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->emblemtype(),req->count());
			}
			break;
		case AOT_ADMIN_ADDWPCCOM:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->wpccomid(),req->count(),req->level());
			}
			break;
		case AOT_ADMIN_ADDADMIRAL:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->admiralid(),1,req->level());
			}
			break;
		case AOT_ADMIN_ADDSOUL:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),0,req->count());
			}
			break;
		case AOT_ADMIN_ADDADMIRALTRAINEXP:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->admiralid(),req->count());
			}
			break;
		case AOT_ADMIN_ADDBOOK:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->bookid(),req->count(),req->level());
			}
			break;
		case AOT_ADMIN_ADDCOUPONS:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),0,req->count());
			}
			break;
		case AOT_ADMIN_ADD_COUNTRYZHANGONG:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),0,req->count());
			}
			break;
		case AOT_ADMIN_ADD_COUNTRYBOX:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),0,req->count());
			}
			break;
		case AOT_ADMIN_ADD_EQUIP:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->equipid(),req->count());
			}
			break;
		case AOT_ADMIN_ADDHERO:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->admiralid(),1);
			}
			break;
		case AOT_ADMIN_ADDLETTER:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->admiralid(),req->count());
			}
			break;
		case AOT_ADMIN_ADDADMIRALLETTER:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),req->admiralid(),req->count());
			}
			break;
		case AOT_ADMIN_ADD_JUNLING:
			{
				SYS_UserStat(pUser,false,"AdminAdd",strCmd,req->opttype(),req->fromweb(),0,req->count());
			}
			break;
		default:
			break;		
		}
	}
	return bSuc;
}

void DealAdminEvent::processAdminOpt(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adminopt_req())
		{
			return;
		}
		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		int64 nUserID = req->uid();
		//string strPlatID = req->platid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
			gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = NULL;
		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
			pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}
		string strCmd;
		bool bSuc = Admin_DealMsg(e,pUser,req,strCmd);
		if(bSuc)
		{
			dh->markUserDirty(pUser);
		}
		
		char buf[1024];
		sprintf(buf, "uid=%lld\r\nsucc:%d\r\nmsg:%s\r\n",
			nUserID, bSuc,strCmd.c_str());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->adminfd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adminopt_req())
		{
			return;
		}
		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		//string strPlatID = req->platid();

		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
			gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = NULL;

		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
			pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}
		string strCmd;
		bool bSuc = Admin_DealMsg(e,pUser,req,strCmd);
		if(bSuc)
		{
			dh->markUserDirty(pUser);
		}


		Admin_AdminOpt_Rsp* rsp = e->mutable_adminopt_rsp();
		
		rsp->set_uid(nUserID);
		rsp->set_adminfd(req->adminfd());
		rsp->set_opttype(req->opttype());
		rsp->set_admin(strCmd);
		rsp->set_level(pUser->GetUserLevel());
		rsp->set_res1(pUser->GetPlayer()->GetResCnt(RC_Metal));
		rsp->set_res2(pUser->GetPlayer()->GetResCnt(RC_Oil));
		rsp->set_credit(pUser->GetPlayer()->GetCredit());
		rsp->set_adminfd(req->adminfd());
		rsp->set_fromweb(req->fromweb());
		rsp->set_gid(req->gid());
		rsp->set_sid(req->sid());
		rsp->set_succ(bSuc);
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);

	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adminopt_rsp())
		{
			return;
		}
		Admin_AdminOpt_Rsp* rsp = e->mutable_adminopt_rsp();
		GameDataHandler* dh = eh_->getDataHandler();
		int64 nUserID = rsp->uid();
		int gid = rsp->gid();
	
		if (gid != eh_->GetSrvID())
		{
			return;
		}
		char buf[1024];
		sprintf(buf, "uid=%lld\r\nsucc:%d\r\nmsg:%s\r\n",
			nUserID, rsp->succ(), rsp->admin().c_str());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp->adminfd(), buf, strlen(buf));

	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminSetLevel(Event* e)
{
    if (e->state() == Admin_AG_Req)
    {
        if (!e->has_adminsetlevel_req())
        {
            return;
        }
        Admin_SetLevel_Req* req = e->mutable_adminsetlevel_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            e->set_state(Admin_GW_Req);
            eh_->sendEventToWorld(e);
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

        int oldlevel = pUser->GetPlayer()->GetLevel();
		int nNewLV = req->level();
		int64 nNeedExp = UserLevelCfg::Instance().GetNeedExp(nNewLV);
		DB_Player* pDBPlayer = pUser->GetPlayer()->GetDBPlayer();
		DB_Bases* pDBBase = NULL;
		if(pDBPlayer->base_size()<=0)
		{
			pDBBase = pDBPlayer->add_base();
		}
		else
		{
			pDBBase = pDBPlayer->mutable_base(0);
		}
		int64 nBaseValue = pDBBase->basevalue();
		int64 nBasePoint = nNeedExp - nBaseValue;
		if(nBasePoint<0)
			nBasePoint = 0;
		pDBBase->set_basepoints(nBasePoint);
		 pUser->GetPlayer()->SetLevel(req->level());
		 pUser->GetPlayer()->CheckUpgrade();
		//pUser->CheckUserNewLevel(nBaseValue,nBasePoint);
       
        int newlevel = pUser->GetPlayer()->GetLevel();
   //     pUser->GetDBPlayer()->set_exp(0);	

        dh->markUserDirty(pUser);

        if (pUser->fd() != 0 && pUser->Online() && newlevel != oldlevel)
        {

        }

        char buf[1024];
        sprintf(buf, "uid=%lld\r\noldlevel=%d\r\nnewlevel=%d\r\n", nUserID, oldlevel, newlevel);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(req->adminfd(), buf, strlen(buf));
    }
    else if (e->state() == Admin_WG_Req)
    {
        if (!e->has_adminsetlevel_req())
        {
            return;
        }
        Admin_SetLevel_Req* req = e->mutable_adminsetlevel_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

		int oldlevel = pUser->GetPlayer()->GetLevel();
		int nNewLV = req->level();
		int64 nNeedExp = UserLevelCfg::Instance().GetNeedExp(nNewLV);
		DB_Player* pDBPlayer = pUser->GetPlayer()->GetDBPlayer();
		DB_Bases* pDBBase = NULL;
		if(pDBPlayer->base_size()<=0)
		{
			pDBBase = pDBPlayer->add_base();
		}
		else
		{
			pDBBase = pDBPlayer->mutable_base(0);
		}
		int64 nBaseValue = pDBBase->basevalue();
		int64 nBasePoint = nNeedExp - nBaseValue;
		if(nBasePoint<0)
			nBasePoint = 0;
		pDBBase->set_basepoints(nBasePoint);
		pUser->GetPlayer()->SetLevel(req->level());
		pUser->GetPlayer()->CheckUpgrade();

        int newlevel = pUser->GetPlayer()->GetLevel();
    //    pUser->GetDBPlayer()->set_exp(0);

        dh->markUserDirty(pUser);

        if (pUser->fd() != 0 && pUser->Online() && newlevel != oldlevel)
        {
        }

        Admin_SetLevel_Rsp* rsp = e->mutable_adminsetlevel_rsp();
        rsp->set_gid(req->gid());
        rsp->set_uid(nUserID);
        rsp->set_adminfd(req->adminfd());
        rsp->set_oldlevel(oldlevel);
        rsp->set_newlevel(newlevel);
		rsp->set_fromweb(req->fromweb());
        e->clear_adminsetlevel_req();
        e->set_state(Admin_GW_Rsp);
        eh_->sendEventToWorld(e);
    }
    else if (e->state() == Admin_WG_Rsp)
    {
        if (!e->has_adminsetlevel_rsp())
        {
            return;
        }
        const Admin_SetLevel_Rsp& rsp = e->adminsetlevel_rsp();
        int gid = rsp.gid();
        if (gid != eh_->GetSrvID())
        {
            return;
        }
        int64 nUserID = rsp.uid();
        int oldlevel = rsp.oldlevel();
        int newlevel = rsp.newlevel();

        char buf[1024];
        sprintf(buf, "uid=%lld\r\noldlevel=%d\r\nnewlevel=%d\r\n",
                nUserID, oldlevel, newlevel);
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
    }
    else
    {
        LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
    }
}

void DealAdminEvent::processAdminBanChat(Event* e)
{
/*
    if (e->state() == Admin_AG_Req)
    {
        if (!e->has_adminbanchat_req())
        {
            return;
        }
        Admin_BanChat_Req* req = e->mutable_adminbanchat_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            e->set_state(Admin_GW_Req);
            eh_->sendEventToWorld(e);
            return;
        }

        if (nUserID == 1)
        {
            //DealBceChat::getInstance()->SetCanChat(req->isban());
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

        time_t now = time(NULL);
 //       DB_Player* pdbPlayer = pUser->GetDbUser().mutable_player();
        //DB_BanChat* pChat = pdbPlayer->mutable_banchat();
//         pChat->set_banchat(req->isban());
//         pChat->set_time(now + req->time()*60);

        dh->markUserDirty(pUser);

        char buf[1024];
        sprintf(buf, "uid=%lld\r\nBanMin=%d\r\n", nUserID, req->time());
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(req->adminfd(), buf, strlen(buf));

//		SYS_LOG(nUserID, LT_BanChat, 0, 0, pUser->platform_id().c_str() << pdbPlayer->level() << pdbPlayer->exp() << req->time() << req->isban());
    }
    else if (e->state() == Admin_WG_Req)
    {
        if (!e->has_adminbanchat_req())
        {
            return;
        }
        Admin_BanChat_Req* req = e->mutable_adminbanchat_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

        time_t now = time(NULL);
 //       DB_Player* pdbPlayer = pUser->GetDbUser().mutable_player();
//         DB_BanChat* pChat = pdbPlayer->mutable_banchat();
//         pChat->set_banchat(req->isban());
//         pChat->set_time(now + req->time()*60);

        dh->markUserDirty(pUser);
//		SYS_LOG(nUserID, LT_BanChat, 0, 0, pUser->platform_id().c_str() << pdbPlayer->level() << pdbPlayer->exp() << req->time() << req->isban());

        Admin_BanChat_Rsp* rsp = e->mutable_adminbanchat_rsp();
        rsp->set_gid(req->gid());
        rsp->set_uid(nUserID);
        rsp->set_adminfd(req->adminfd());
        rsp->set_isban(req->isban());
        rsp->set_time(req->time());
		rsp->set_fromweb(req->fromweb());
        e->clear_adminbanchat_req();
        e->set_state(Admin_GW_Rsp);
        eh_->sendEventToWorld(e);
    }
    else if (e->state() == Admin_WG_Rsp)
    {
        if (!e->has_adminbanchat_rsp())
        {
            return;
        }
        const Admin_BanChat_Rsp& rsp = e->adminbanchat_rsp();
        int gid = rsp.gid();
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        char buf[1024];
        sprintf(buf, "uid=%lld\r\nBanMin=%d\r\n", (int64)rsp.uid(), rsp.time());
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
    }
*/
}

void DealAdminEvent::processAdminAddGuildBtlReward(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adminaddguildbtlreward_req())
		{
			return;
		}
		Admin_AddGuildBtlReward_Req* req = e->mutable_adminaddguildbtlreward_req();
		//发送给国家服务器
		eh_->sendEventToCountry(e,req->region());
		//char buf[1024];
		//sprintf(buf, "uid=%lld\r\nGuildID=%d,rank=%d\r\n", nUserID, req->guildid(), req->guildrank() );
		//strcat(buf, "\r\nBombBaby# ");
		//eh_->sendFdString(req->adminfd(), buf, strlen(buf));
	}
	else if(e->state() == Admin_CG_Rsq)
	{
		if(!e->has_adminaddguildbtlreward_rsp())
		{
			return;
		}
		Admin_AddGuildBtlReward_Rsp* rsp = e->mutable_adminaddguildbtlreward_rsp();
		char buf[1024];
		sprintf(buf, "ret=%d\r\nGuildID=%d,rank=%d,region=%d\r\n", rsp->ret(), rsp->guildid(), rsp->guildrank(),rsp->region() );
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp->adminfd(), buf, strlen(buf));
	}

}

void DealAdminEvent::processAdminResetWholeCountry(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		Admin_ResetWholeCountry(e);

		char buf[1024];

		Admin_AdminOpt_Rsp* rsp = e->mutable_adminopt_rsp();
		sprintf(buf, "resetwholecountry\r\n");
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp->adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminGveList(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		Admin_GveList(e);

		char buf[1024];

		Admin_AdminOpt_Rsp* rsp = e->mutable_adminopt_rsp();
		sprintf(buf, "gvelist\r\n");
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp->adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminBanLogin(Event* e)
{

    if (e->state() == Admin_AG_Req)
    {
        if (!e->has_adminbanlogin_req())
        {
            return;
        }
        Admin_BanLogin_Req* req = e->mutable_adminbanlogin_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            e->set_state(Admin_GW_Req);
            eh_->sendEventToWorld(e);
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

        time_t now = time(NULL);

   //     DB_Player* pdbPlayer = pUser->GetDbUser().mutable_player();
        DB_BanLogin* pLogin = pUser->GetDBBanLogin();
        if (req->isban())
        {
            eh_->removeUserFdMap(pUser->fd(), nUserID);
        }
        pLogin->set_banlogin(req->isban());
        pLogin->set_time(now + req->time()*60);

        dh->markUserDirty(pUser);

        char buf[1024];
        sprintf(buf, "uid=%lld\r\nBanMin=%d\r\n", nUserID, req->time());
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(req->adminfd(), buf, strlen(buf));

		SYS_LOG(nUserID, LT_BanLogin, 0, 0, 1<<pUser->GetPlatformId().c_str() << pUser->GetUserLevel()<< req->time() << req->isban());
    }
    else if (e->state() == Admin_WG_Req)
    {
        if (!e->has_adminbanlogin_req())
        {
            return;
        }
        Admin_BanLogin_Req* req = e->mutable_adminbanlogin_req();
        int64 nUserID = req->uid();
        GameDataHandler* dh = eh_->getDataHandler();
        int gid = dh->getGamedIdByUserId(nUserID);
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        LoadStatus state;
        User* pUser = dh->getUser(nUserID, &state, true);
        if (pUser == NULL)
        {
            if (state == LOAD_WAITING)
            {
                eh_->postBackEvent(e);
            }
            return;
        }

        time_t now = time(NULL);

 //       DB_Player* pdbPlayer = pUser->GetDBPlayer();
        DB_BanLogin* pLogin = pUser->GetDBBanLogin();
        if (req->isban())
        {
            eh_->removeUserFdMap(pUser->fd(), nUserID);
        }
        pLogin->set_banlogin(req->isban());
        pLogin->set_time(now + req->time()*60);

        dh->markUserDirty(pUser);
		SYS_LOG(nUserID, LT_BanLogin, 0, 0, 1<<pUser->GetPlatformId().c_str() << pUser->GetUserLevel()<< req->time() << req->isban());

        Admin_BanLogin_Rsp* rsp = e->mutable_adminbanlogin_rsp();
        rsp->set_gid(req->gid());
        rsp->set_uid(nUserID);
        rsp->set_adminfd(req->adminfd());
        rsp->set_isban(req->isban());
        rsp->set_time(req->time());
		rsp->set_fromweb(req->fromweb());
		rsp->set_sid(req->sid());
        e->clear_adminbanlogin_req();
        e->set_state(Admin_GW_Rsp);
        eh_->sendEventToWorld(e);

    }
    else if (e->state() == Admin_WG_Rsp)
    {
        if (!e->has_adminbanlogin_rsp())
        {
            return;
        }
        const Admin_BanLogin_Rsp& rsp = e->adminbanlogin_rsp();
        int gid = rsp.gid();
        if (gid != eh_->GetSrvID())
        {
            return;
        }

        char buf[1024];
        sprintf(buf, "uid=%lld\r\nBanMin=%d\r\n", (int64)rsp.uid(), rsp.time());
        strcat(buf, "\r\nBombBaby# ");
        eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
    }

}

void DealAdminEvent::processAdminBandWGUser(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adminbandwguser_req())
		{
			return;
		}
		//Admin_Reload_Req* req = e->mutable_adminreload_req();
		e->set_state(Admin_GW_Req);
		eh_->sendEventToWorld(e);
		//eh_->sendEventToStar(e);
	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adminbandwguser_req())
		{
			return;
		}

		Admin_BandWGUser_Req* req = e->mutable_adminbandwguser_req();

		GameDataHandler* dh = eh_->getDataHandler();
		dh->setBandUser(req->banduser());

		Admin_BandWGUser_Rsp* rsp = e->mutable_adminbandwguser_rsp();
		rsp->set_gid(req->gid());
		rsp->set_adminfd(req->adminfd());
		rsp->set_fromweb(req->fromweb());
		e->clear_adminbandwguser_req();
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adminbandwguser_rsp())
		{
			return;
		}

		const Admin_BandWGUser_Rsp& rsp = e->adminbandwguser_rsp();

		char buf[1024];
		sprintf(buf, "Game %d banwaiguauser!\r\n", rsp.gid());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminClearDefense(Event* e)
{
	if (e->has_admincleardefense() == false)
	{
		return;
	}

	if (e->state() == Admin_AG_Req)
	{
		adminClearDefense* req = e->mutable_admincleardefense();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		if(pUser->GetPlayer())
		{
			pUser->GetPlayer()->ResetDefenseNpcData(req->ntype(),req->ncustom());
			dh->markUserDirty(pUser);

			if(pUser->Online())
			{
				pUser->GetPlayer()->SendPlayerDefenseData();
			}			
		}

		char buf[1024];
		sprintf(buf, "uid=%lld\r\ncleardefensenpc ok\r\n",nUserID);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		adminClearDefense* req = e->mutable_admincleardefense();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;

		gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		req->set_succ(false);

		if(pUser->GetPlayer())
		{
			pUser->GetPlayer()->ResetDefenseNpcData(req->ntype(),req->ncustom());
			dh->markUserDirty(pUser);

			req->set_succ(true);

			if(pUser->Online())
			{
				pUser->GetPlayer()->SendPlayerDefenseData();
			}			
		}

		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	if (e->state() == Admin_WG_Rsp)
	{
		adminClearDefense* req = e->mutable_admincleardefense();

		char buf[1024];
		if(req->succ() == true)
		{
			sprintf(buf, "uid=%lld\r\ncleardefensenpc ok\r\n",req->uid());
		}
		else
		{
			sprintf(buf, "uid=%lld\r\ncleardefensenpc error\r\n",req->uid());
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminSetNewComerGift(Event* e)
{
	if (e->has_adminsetnewcomergift() == false)
	{
		return;
	}

	if (e->state() == Admin_AG_Req)
	{
		adminSetNewComerGift* req = e->mutable_adminsetnewcomergift();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		if(pUser->GetPlayer())
		{
			pUser->GetPlayer()->SetNewComerGiftData(req->ntype(),req->nid(),req->nstate());
			dh->markUserDirty(pUser);

			req->set_succ(true);
		}

		char buf[1024];
		sprintf(buf, "uid=%lld\r\n setnewcomergift ok\r\n",nUserID);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		adminSetNewComerGift* req = e->mutable_adminsetnewcomergift();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;

		gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		req->set_succ(false);

		if(pUser->GetPlayer())
		{
			pUser->GetPlayer()->SetNewComerGiftData(req->ntype(),req->nid(),req->nstate());
			dh->markUserDirty(pUser);

			req->set_succ(true);	
		}

		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	if (e->state() == Admin_WG_Rsp)
	{
		adminSetNewComerGift* req = e->mutable_adminsetnewcomergift();

		char buf[1024];
		if(req->succ() == true)
		{
			sprintf(buf, "uid=%lld\r\n setnewcomergift ok\r\n",req->uid());
		}
		else
		{
			sprintf(buf, "uid=%lld\r\n setnewcomergift error\r\n",req->uid());
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminSetContinueLandDay(Event* e)
{
	if (e->has_adminsetcontinuelandday() == false)
	{
		return;
	}

	if (e->state() == Admin_AG_Req)
	{
		adminSetContinueLandDay* req = e->mutable_adminsetcontinuelandday();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		//pUser->SetLastLoginTime(time(NULL));

		pUser->GetDbUser().mutable_dailylogin()->set_daycount(req->nday());
		pUser->GetDbUser().set_landdaycount(req->nday());

		req->set_succ(true);

		char buf[1024];
		sprintf(buf, "uid=%lld\r\n setcontinuelandday ok\r\n",nUserID);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		adminSetContinueLandDay* req = e->mutable_adminsetcontinuelandday();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;

		gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		time_t now = time(NULL);
		struct tm nowtime = *localtime(&now);

		int nDay = nowtime.tm_yday - req->nday();

		pUser->GetDbUser().mutable_dailylogin()->set_firstday(nDay);
		pUser->GetDbUser().mutable_dailylogin()->set_daycount(req->nday());
		pUser->GetDbUser().set_landdaycount(req->nday());

		req->set_succ(true);

		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	if (e->state() == Admin_WG_Rsp)
	{
		adminSetContinueLandDay* req = e->mutable_adminsetcontinuelandday();

		char buf[1024];
		if(req->succ() == true)
		{
			sprintf(buf, "uid=%lld\r\n setcontinuelandday ok\r\n",req->uid());
		}
		else
		{
			sprintf(buf, "uid=%lld\r\n setcontinuelandday error\r\n",req->uid());
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminSetPlatInviteId(Event* e)
{
	if (e->has_adminsetplatinviteid() == false)
	{
		return;
	}

	if (e->state() == Admin_AG_Req)
	{
		adminSetPlatInviteID* req = e->mutable_adminsetplatinviteid();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		pUser->SetPlatInviteFriendOpenid_Force(req->npid());
		pUser->InitPlatInviteFriendTime();
		dh->markUserDirty(pUser);

		req->set_succ(true);

		char buf[1024];
		sprintf(buf, "uid=%lld\r\n setplatinviteplatid ok\r\n",nUserID);
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else if (e->state() == Admin_WG_Req)
	{
		adminSetPlatInviteID* req = e->mutable_adminsetplatinviteid();
		int64 nUserID = req->uid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;

		gid = dh->getGamedIdByUserId(nUserID);
		if (gid != eh_->GetSrvID())
		{
			return;
		}

		LoadStatus state;
		User* pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		pUser->SetPlatInviteFriendOpenid_Force(req->npid());
		pUser->InitPlatInviteFriendTime();
		dh->markUserDirty(pUser);

		req->set_succ(true);

		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	if (e->state() == Admin_WG_Rsp)
	{
		adminSetPlatInviteID* req = e->mutable_adminsetplatinviteid();

		char buf[1024];
		if(req->succ() == true)
		{
			sprintf(buf, "uid=%lld\r\n setplatinviteplatid ok\r\n",req->uid());
		}
		else
		{
			sprintf(buf, "uid=%lld\r\n setplatinviteplatid error\r\n",req->uid());
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->fd(), buf, strlen(buf));

	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdSet(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		e->set_state(Admin_GW_Req);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adset_req())
		{
			return;
		}
		
		bool bSuc = false;
		Admin_AdSet_Req* req = e->mutable_adset_req();
		switch(req->opttype())
		{
		case AAT_Enable:
			{
				AdCfg::Instance().SetPopProp((req->enable()>0));
				if(req->enable())
				{
					AdCfg::Instance().SetPopProp(0,0,0,0,-1,-1,req->daypopcnt(),req->popinterval(),req->showseconds());
				}
				bSuc = true;
			}
			break;
		case AAT_RegTime:
			{
				string strDayBegin	= req->daystart();
				string strDayEnd	= req->dayend();
				string strTimeBegin = "00:00:00";
				string strTimeEnd	= "23:59:59";
				time_t ltStart = GetDayTimeByStr(strDayBegin.c_str(),strTimeBegin.c_str());
				time_t ltEnd   = GetDayTimeByStr(strDayEnd.c_str(),strTimeEnd.c_str());
				if(ltStart>0&&ltEnd>0)
				{
					AdCfg::Instance().SetPopProp(ltStart,ltEnd);
					bSuc = true;
				}
			}
			break;
		case AAT_PopTime:
			{
				string strDayBegin	= req->daystart();
				string strDayEnd	= req->dayend();
				string strTimeBegin = "00:00:00";
				string strTimeEnd	= "23:59:59";
				time_t ltStart = GetDayTimeByStr(strDayBegin.c_str(),strTimeBegin.c_str());
				time_t ltEnd   = GetDayTimeByStr(strDayEnd.c_str(),strTimeEnd.c_str());
				if(ltStart>0&&ltEnd>0)
				{
					AdCfg::Instance().SetPopProp(0,0,ltStart,ltEnd);
					bSuc = true;
				}
			}
			break;
		case AAT_WebSet:
			{
				AdCfg::Instance().SetPopProp((req->enable()>0));
				AdCfg::Instance().SetPopProp(req->dayregstart(),req->dayregend(),
											req->dayshowstart(),req->dayshowend(),
											-1,-1,
											req->daypopcnt(),req->popinterval(),
											req->showseconds(),req->adid(),req->picval());
				bSuc = true;
				GameDataHandler* dh = eh_->getDataHandler();
				dh->SetAdFlag(true);
			}
			break;
		default:
			return;
		}
		
		if(req->fromweb())
			return;

		Admin_AdSet_Rsp* rsp = e->mutable_adset_rsp();
		rsp->set_gid(req->gid());
		rsp->set_adminfd(req->adminfd());
		rsp->set_gameid(req->gid());
		rsp->set_opttype(req->opttype());
		rsp->set_succ(bSuc);
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adset_rsp())
		{
			return;
		}

		const Admin_AdSet_Rsp& rsp = e->adset_rsp();

		char buf[1024];
		if(rsp.succ())
		{
			sprintf(buf, "Game %d succ!\r\n", rsp.gameid());
		}
		else
		{
			sprintf(buf, "Game %d failed!\r\n", rsp.gameid());
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}

void DealAdminEvent::processAdminReload(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adminreload_req())
		{
			return;
		}
		//Admin_Reload_Req* req = e->mutable_adminreload_req();
		e->set_state(Admin_GW_Req);
		eh_->sendEventToWorld(e);
		eh_->sendEventToAllStar(e);
		eh_->sendEventToAllPlat(e);
		eh_->sendEventToAllFight(e);
		eh_->sendEventToAllCountry(e);
		eh_->sendEventToAllRegion(e);
	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adminreload_req())
		{
			return;
		}

		Admin_Reload_Req* req = e->mutable_adminreload_req();

		GameConfigMgr::GetInst()->LoadGameConfigInfo(eh_->getDataHandler());
		
		if(GameSrvCfg::Instance().FriendInfoEnable())
		{
			FriendInfoHandler::Instance().initThread();
		}
		if(GameSrvCfg::Instance().StarInfoEnable())
		{
			StarInfoHandler::Instance().initThread();
		}

		int ngid = req->gid();
		GameDataHandler* pUserManager = eh_->getDataHandler();
		pUserManager->SetAdFlag(true);
		ngid = pUserManager->getPhysicsGameNum();

		Admin_Reload_Rsp* rsp = e->mutable_adminreload_rsp();
		rsp->set_gid(req->gid());
		rsp->set_adminfd(req->adminfd());
		rsp->set_fromweb(req->fromweb());
		rsp->set_gameid(ngid);
		e->clear_adminreload_req();
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adminreload_rsp())
		{
			return;
		}

		const Admin_Reload_Rsp& rsp = e->adminreload_rsp();

		char buf[1024];
		sprintf(buf, "Game %d reloaded!\r\n", rsp.gameid());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}


void DealAdminEvent::processAdminGetBldCnt(Event* e)
{
	if ( e->state() == Admin_AG_Req )
	{
		if (!e->has_admingetbldcnt_req())
		{
			return;
		}
		Admin_GetBldCnt_Req* req = e->mutable_admingetbldcnt_req();
		int64 nUserID = req->uid();
		//string strPlatID = req->platid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
		gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = NULL;
		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
		pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		if( pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse() )
		{
			return;
		}

		BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();

		if(pBaseMgr==NULL)
			return;

		char buf[2048];
		int offset = 0;
		offset += sprintf(buf, "uid=%lld\r\n",
			pUser->GetUid());

		for ( int i = 0; i < BuildingInfoCfg::Instance().GetTblNum(); i++ )
		{
			const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTblByNum(i);
			if(pBldTbl==NULL)
				continue;

			vector<DB_Building*>* pBldLst = pBaseMgr->GetBuildingLst(pBldTbl->m_nType);
			if(pBldLst==NULL)
				continue;
			int nCurCnt = pBldLst->size();

			offset += sprintf(buf + offset, "bldtype=%d,bldcnt=%d\r\n",
				pBldTbl->m_nType, nCurCnt);
		}
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(req->adminfd(), buf, strlen(buf));
	}

	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_admingetbldcnt_req())
		{
			return;
		}
		Admin_GetBldCnt_Req* req = e->mutable_admingetbldcnt_req();
		int64 nUserID = req->uid();
		//string strPlatID = req->platid();
		GameDataHandler* dh = eh_->getDataHandler();
		int gid = 0;
		//if(strPlatID.size()>0)
		//	gid = dh->getGameIdfromPlatId(strPlatID);
		//else
		gid = dh->getGamedIdByUserId(nUserID);

		if (gid != eh_->GetSrvID())
		{
			e->set_state(Admin_GW_Req);
			eh_->sendEventToWorld(e);
			return;
		}

		LoadStatus state;
		User* pUser = NULL;
		//if(strPlatID.size()>0)
		//	pUser = dh->getUser(strPlatID, &state, true);
		//else
		pUser = dh->getUser(nUserID,&state,true);
		if (pUser == NULL)
		{
			if (state == LOAD_WAITING)
			{
				eh_->postBackEvent(e);
			}
			return;
		}

		if( pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse() )
		{
			return;
		}

		BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();

		if(pBaseMgr==NULL)
			return;

		char buf[2048];
		int offset = 0;
		offset += sprintf(buf, "uid=%lld\r\n",
			pUser->GetUid());

		for ( int i = 0; i < BuildingInfoCfg::Instance().GetTblNum(); i++ )
		{
			const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTblByNum(i);
			if(pBldTbl==NULL)
				continue;

			vector<DB_Building*>* pBldLst = pBaseMgr->GetBuildingLst(pBldTbl->m_nType);
			if(pBldLst==NULL)
				continue;
			int nCurCnt = pBldLst->size();

			offset += sprintf(buf + offset, "bldtype=%d,bldcnt=%d\r\n",
				pBldTbl->m_nType, nCurCnt);
		}

		strcat(buf, "\r\nBombBaby# ");
		req->set_msg( buf, strnlen( buf, sizeof(buf) ) );

		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_admingetbldcnt_req())
		{
			return;
		}

		const Admin_GetBldCnt_Req& req = e->admingetbldcnt_req();
		
		string msg = req.msg();
		eh_->sendFdString(req.adminfd(), msg.c_str(), msg.size() );
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}



	if (!e->has_adminopt_req())
	{
		return;
	}

}

void DealAdminEvent::processAdminReloadChickenRank(Event* e)
{
	if (e->state() == Admin_AG_Req)
	{
		if (!e->has_adreloadchickenrank_req())
		{
			return;
		}
		e->set_state(Admin_GW_Req);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Req)
	{
		if (!e->has_adreloadchickenrank_req())
		{
			return;
		}

		Admin_ReloadChickenRank_Req* req = e->mutable_adreloadchickenrank_req();

		for(int i=0;i<serverConfig.region_num();i++)
		{
			if ( req->type() == 0 )
			{
				HttpRequireHandler::Instance().SafePushHttpRequest("",0,i,WEBREQ_ChickenRank,0);
			}
			
			if ( req->type() == 1 )
			{
				HttpRequireHandler::Instance().SafePushHttpRequest("",0,i,WEBREQ_TomorrowRank,0);
				HttpRequireHandler::Instance().SafePushHttpRequest("",0,i,WEBREQ_TomorrowStar,0);
			}
		}

		if ( req->type() == 2 )
		{
			HttpRequireHandler::Instance().SafePushHttpRequest("",0,1,WEBREQ_Cathectic,0);
		}

		Admin_ReloadChickenRank_Rep* rsp = e->mutable_adreloadchickenrank_rep();
		rsp->set_gid(req->gid());
		rsp->set_adminfd(req->adminfd());
		e->set_state(Admin_GW_Rsp);
		eh_->sendEventToWorld(e);
	}
	else if (e->state() == Admin_WG_Rsp)
	{
		if (!e->has_adreloadchickenrank_rep())
		{
			return;
		}

		const Admin_ReloadChickenRank_Rep& rsp = e->adreloadchickenrank_rep();

		char buf[1024];
		sprintf(buf, "Game %d chickenrankreloaded!\r\n", rsp.gid());
		strcat(buf, "\r\nBombBaby# ");
		eh_->sendFdString(rsp.adminfd(), buf, strlen(buf));
	}
	else
	{
		LOG4CXX_DEBUG(logger_, "Invalid admin command status:" << e->state());
	}
}


bool DealAdminEvent::Admin_AddRes(User* pUser,int64 nRes1, int64 nRes2)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	int64 nAdd1 = pUser->GetPlayer()->AddRes(RC_Metal,nRes1);
	int64 nAdd2 = pUser->GetPlayer()->AddRes(RC_Oil,nRes2);
	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDRES,0,0,nRes1<<nRes2<<pUser->GetPlayer()->GetResCnt(RC_Metal)<<pUser->GetPlayer()->GetResCnt(RC_Oil));
	if(pUser->Online()&&pUser->fd()>0)
	{
		string text;
		RseRes res;
		res.set_electric(0);
		res.set_human(0);
		res.set_metal((int)nAdd1);
		res.set_oil((int)nAdd2);
		res.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseRes, text);
	}
	return true;
}

bool DealAdminEvent::Admin_AddArmy(User* pUser, int nArmyType, int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const ArmyItemTbl* pArmyTbl = ArmyInfoCfg::Instance().GetArmyInfoTbl(nArmyType);
	if(pArmyTbl==NULL)
		return false;
	int nBldType = ArmyInfoCfg::Instance().GetProduceArmyBldType(nArmyType);
	int x=-520;
	int y=400;
	//DB_Building* pDBBld = pBaseMgr->GetDBBuildingByType(nBldType);
	//if(pDBBld)
	//{
	//	x = pDBBld->rallyx();
	//	y = pDBBld->rallyy();
	//}

	int nLV = pBaseMgr->GetArmyLV(nArmyType);
	if(pUser->Online()&&pUser->fd()>0)
	{
		RseBuildingUpdate rseUP;
		rseUP.Clear();
		rseUP.set_type(7);

		for(int i=0;i<nCnt;i++)
		{
			int nHP = pBaseMgr->CacuArmyFullHP(nArmyType,nLV,pArmyTbl);
			DB_Army* pDBArmy = pBaseMgr->CreateArmy(nArmyType,x,y,nLV,nHP,0);
			if(pDBArmy)
			{
				pDBArmy->set_campstate(E_ARMY_CAMP_STATE_COMMON);
				Army* pArmy = rseUP.add_armylst();
				pBaseMgr->FillArmyData(pDBArmy,pArmy);
			}
		}

		string text;
		rseUP.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseBuildingUpdate, text);

	}
	else
	{
		if(!pBaseMgr->CreateArmy(nArmyType,x,y,nLV,nCnt))
			return false;
	}
	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDARMY,0,0,nArmyType<<nCnt);
	return true;
}


bool DealAdminEvent::Admin_DelArmy(User* pUser, int nArmyType, int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	int nDelCnt = pBaseMgr->AdminDelArmy(nArmyType,nCnt);
	SYS_LOG(pUser->GetUid(),LT_ADMIN_DELARMY,0,0,nArmyType<<nCnt<<nDelCnt);
	return true;
}

bool DealAdminEvent::Admin_AddBld(User* pUser,int nBldType,int nLV,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(nBldType);
	if(pBldTbl==NULL)
		return false;
	if(nLV<=0||nLV>pBldTbl->m_nMaxLV)
		return false;
	for(int i=0;i<nCnt;i++)
	{
		int nX = rand()%200-100;
		int nY = rand()%200-100;
		DB_Building* pDBBld = pBaseMgr->CreateBuilding(nBldType,nX,nY,nLV);
		if(pDBBld)
		{
			SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDBLD,0,0,pDBBld->id()<<nBldType<<nLV);
		}
	}
	pBaseMgr->UpdateBuildingMap();
	return true;
}

bool DealAdminEvent::Admin_DelBld(User* pUser,int nBldID)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Building* pDBBld = pBaseMgr->GetDBBuilding(nBldID);
	if(pDBBld)
	{
		int nBldType = pDBBld->type();
		int nLV		 = pDBBld->level();
		pBaseMgr->DelDBBuilding(nBldID);
		SYS_LOG(pUser->GetUid(),LT_ADMIN_DELBLD,0,0,nBldID<<nBldType<<nLV);

		return true;
	}
	return false;
}

bool compareBldByLv(DB_Building* build1, DB_Building* build2)
{
	return build1->level() > build2->level();
}

bool DealAdminEvent::Admin_SetBldCnt(User* pUser,int nBldType,int nLV,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(nBldType);
	if(pBldTbl==NULL)
		return false;
	if(nLV<=0||nLV>pBldTbl->m_nMaxLV)
		return false;
	vector<DB_Building*>* pBldLst = pBaseMgr->GetBuildingLst(nBldType);
	if(pBldLst==NULL)
		return false;
	int nCurCnt = pBldLst->size();
	if(nCurCnt<nCnt)
	{
		for(int i=nCurCnt;i<nCnt;i++)
		{
			int nX = rand()%200-100;
			int nY = rand()%200-100;
			DB_Building* pDBBld = pBaseMgr->CreateBuilding(nBldType,nX,nY,nLV);
			if(pDBBld)
			{
				SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDBLD,0,0,pDBBld->id()<<nBldType<<nLV);
			}
		}
		return true;
	}
	else
	{
		return Admin_DelBldCnt(pUser,nBldType,nCurCnt-nCnt);
	}
	return true;
}

bool DealAdminEvent::Admin_SetAllBldLV(User* pUser, int nBldType,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(nBldType);
	if(pBldTbl==NULL)
		return false;
	if(nLV<=0||nLV>pBldTbl->m_nMaxLV)
		return false;
	vector<DB_Building*>* pBldLst = pBaseMgr->GetBuildingLst(nBldType);
	if(pBldLst==NULL)
		return false;
	
	vector<DB_Building*>::iterator iter;
	for(iter = pBldLst->begin();iter!=pBldLst->end();iter++)
	{
		DB_Building* pDBBld = *iter;
		if(pDBBld&&pDBBld->type()==nBldType)
		{
			pDBBld->set_level(nLV);
			pDBBld->set_hp(pBaseMgr->CacuBldFillHP(nBldType,nLV,pDBBld,pBldTbl));
		}
	}
	SYS_LOG(pUser->GetUid(),LT_ADMIN_SETALLBLDLV,0,0,nBldType<<nLV);
	return true;
}

bool DealAdminEvent::Admin_SetBldLV(User* pUser, int nBldID,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Building* pDBBld = pBaseMgr->GetDBBuilding(nBldID);
	if(pDBBld==NULL)
		return false;

	const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(pDBBld->type());
	if(pBldTbl==NULL)
		return false;
	if(nLV<=0||nLV>pBldTbl->m_nMaxLV)
		return false;
	pDBBld->set_level(nLV);
	pDBBld->set_hp(pBaseMgr->CacuBldFillHP(pDBBld->type(),nLV,pDBBld,pBldTbl));
	//pDBBld->set_hp(pUser->GetPlayer()->GetMaxHpReflex(pBldTbl->m_nHP[nLV]));
	SYS_LOG(pUser->GetUid(),LT_ADMIN_SETBLDLV,0,0,nBldID<<pDBBld->type()<<nLV);
	return true;
}

bool DealAdminEvent::Admin_SetArmyLV(User* pUser, int nArmyType,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const ArmyItemTbl* pArmyTbl = ArmyInfoCfg::Instance().GetArmyInfoTbl(nArmyType);
	if(pArmyTbl==NULL)
		return false;
	if(nLV<=0||nLV>pArmyTbl->m_nMaxLV)
		return false;
	pBaseMgr->SetArmyUnlockLV(nArmyType,nLV);
	SYS_LOG(pUser->GetUid(),LT_ADMIN_SETARMYLV,0,0,nArmyType<<nLV);
	return true;

}

bool DealAdminEvent::Admin_SaveNpc(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	pUser->SaveInfo2NPCJson();
	return true;
}

bool DealAdminEvent::Admin_AddNpc(User* pUser, int nNPCID,bool bAdd)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Player* pDBPlayer = pUser->GetPlayer()->GetDBPlayer();
	bool bFind = false;
	for(int i=0;i<pDBPlayer->wmstatus_size();i++)
	{
		DB_WmStatus* pDBWS = pDBPlayer->mutable_wmstatus(i);
		if(pDBWS->baseid()==nNPCID)
		{
			if(bAdd)
			{
				pDBWS->set_destroyed(false);
			}
			else
			{
				pDBWS->set_destroyed(true);
			}
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		NPCWmstatusTbl* pTbl = NPCInfoCfg::Instance().GetWmstatusTbl(nNPCID);
		if(pTbl)
		{
			DB_WmStatus* pDBWS = pDBPlayer->add_wmstatus();
			pDBWS->set_baseid(pTbl->m_nNPCID);
			if(bAdd)
			{
				pDBWS->set_destroyed(false);
			}
			else
			{
				pDBWS->set_destroyed(true);
			}
			pDBWS->set_factionid(pTbl->m_nFactionid);
			pDBWS->set_level(pTbl->m_nLevel);
		}
	}
	if(bAdd)
	{
		SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDNPC,0,0,nNPCID);
	}
	else
	{
		SYS_LOG(pUser->GetUid(),LT_ADMIN_DELNPC,0,0,nNPCID);
	}

	return true;
}

bool DealAdminEvent::Admin_AddFeats(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	pPlayer->AddFeats(nCnt,FEATS_TYPE_DEF_ADMIN);
	eh_->SendUserInfo(pUser);
	return true;
}

bool DealAdminEvent::Admin_AddAdmiral(User* pUser,int nAdmiralID,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const AdmiralTbl* pAdmTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl(nAdmiralID);
	if(pAdmTbl==NULL)
	{
		return false;
	}
	DB_Admiral* pDBAdm = pBaseMgr->GetDBAdmiral(nAdmiralID);
	if(pDBAdm==NULL)
	{// 添加将领
		pDBAdm = pBaseMgr->AddAdmiral(nAdmiralID);
		if(pDBAdm==NULL)
			return false;
		if(nLV>1)
		{
			pDBAdm->set_level(nLV);
			pDBAdm->set_exp(AdmiralInfoCfg::Instance().GetUpgradeNeedExp(nLV)+10);
		}
		pBaseMgr->CacuAdmiralFinalProp(pDBAdm);
		SYS_LOG(pUser->GetUid(),LT_ADIMN_AddAdmiral,0,0,1<<pDBAdm->admiralid()<<pDBAdm->level());
		return true;
	}
	else
	{
		if(GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_ADMIRAL_BUFF)){
			pDBAdm->set_bufftime(time(NULL));
		}
		if(pDBAdm->level()!=nLV&&nLV>0)
		{
			pDBAdm->set_level(nLV);
			pDBAdm->set_exp(AdmiralInfoCfg::Instance().GetUpgradeNeedExp(nLV)+10);
			pBaseMgr->CacuAdmiralFinalProp(pDBAdm);
		}
		SYS_LOG(pUser->GetUid(),LT_ADIMN_AddAdmiral,0,0,2<<pDBAdm->admiralid()<<pDBAdm->level());

	}
	return true;
}

bool DealAdminEvent::Admin_SetAdmiralSkill(User* pUser,int nAdmiralID,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse()||nLV<=0||nLV>MAX_ADMIRAL_SKILL_LV)
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const AdmiralTbl* pAdmTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl(nAdmiralID);
	if(pAdmTbl==NULL)
	{
		return false;
	}
	DB_Admiral* pDBAdm = pBaseMgr->GetDBAdmiral(nAdmiralID);
	if(pDBAdm==NULL)
	{
		return false;
	}
	DB_AdmiralSkillData* pDBAdmiralSkill = pDBAdm->mutable_initskill();
	if(pDBAdmiralSkill==NULL)
		return false;
	pDBAdmiralSkill->set_level(nLV);
	pBaseMgr->UpdateAdmiralEffect();
	pBaseMgr->UpdateArmyHP();
	pBaseMgr->UpdateBldHP();
	pBaseMgr->UpdateAllHeroHP();
	//更新战斗力
	pBaseMgr->UpdateAllCombatPowerValue_Admiral();
	pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
	//计算将领评分
	pBaseMgr->CacuAdmiralScore(pDBAdm);
	eh_->getDataHandler()->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(),LT_ADIMN_SetAdmiralSkill,0,0,pDBAdm->admiralid()<<pDBAdmiralSkill->level());
	return true;
}

bool DealAdminEvent::Admin_AddAdmiralTrainExp(User* pUser,int nAdmiralID,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const AdmiralTbl* pAdmTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl(nAdmiralID);
	DB_Admiral* pDBAdm = pBaseMgr->GetDBAdmiral(nAdmiralID);
	if(pDBAdm==NULL||pAdmTbl==NULL||pAdmTbl->m_nQuality<4)
		return false;

	DB_AdmiralTrainLV* pDBTrainLV = pDBAdm->mutable_trainlv();
	pDBTrainLV->set_cnt(pDBTrainLV->cnt()+nCnt);
	int nLV = AdmiralInfoCfg::Instance().CacuTrainSkillLV(pDBTrainLV->cnt());
	if(pDBTrainLV->level()!=nLV)
	{
		pDBTrainLV->set_level(nLV);
	}
	pBaseMgr->CacuAdmiralFinalProp(pDBAdm,pAdmTbl);
	if(pDBAdm->usetype())
	{
		pBaseMgr->UpdateAdmiralEffect();
		pBaseMgr->UpdateArmyHP();
		pBaseMgr->UpdateBldHP();
		pBaseMgr->UpdateAllHeroHP();
	}

	SYS_LOG(pUser->GetUid(),LT_ADIMN_AddAdmiralTExp,0,0,nAdmiralID<<pAdmTbl->m_nSkillID<<pDBTrainLV->level()<<pDBTrainLV->cnt());
	
	return true;
}

bool DealAdminEvent::Admin_AddAdmiralSoul(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	int nOldCnt = pBaseMgr->GetAdmiralSoul();
	int nOverCnt = 0;
	
	if ( nOldCnt + nCnt < 0 )
	{
		nOverCnt = nOldCnt + nCnt;
	}
	pBaseMgr->AddAdmiralSoul(nCnt);
	SYS_LOG(pUser->GetUid(),LT_ADIMN_AddSoul,0,0,nCnt<<pBaseMgr->GetAdmiralSoul()<<nOverCnt);
	return true;
}
bool DealAdminEvent::Admin_DelAdmiral(User* pUser,int nAdmiralID)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Admiral* pDBAdm = pBaseMgr->GetDBAdmiral(nAdmiralID);
	if(pDBAdm==NULL)
		return true;
	int nLV = pDBAdm->level();
	pBaseMgr->UseAdmiral(pDBAdm,NULL);
	pBaseMgr->DelAdmiral(nAdmiralID);
	SYS_LOG(pUser->GetUid(),LT_ADIMN_DelAdmiral,0,0,nAdmiralID<<nLV);

	return true;
}

bool DealAdminEvent::Admin_AddWPCCOM(User* pUser,int nWPCCOMID,int nCnt,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const WPCComTbl* pTbl = WeaponCenterCfg::Instance().GetWPCComTbl(nWPCCOMID);
	if(pTbl ==NULL)
		return false;

	pBaseMgr->AddWPCCom(pTbl,nCnt,nLV);

	pUser->SetGameStarCache(pUser->GetPlattype(),eh_->getDataHandler());

	SYS_LOG(pUser->GetUid(),LT_ADIMN_ADDWPCCOM,0,0,nWPCCOMID<<nCnt<<nLV);
	return true;
}

bool DealAdminEvent::Admin_AddBook(User* pUser,int nBookID,int nCnt,int nLV)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const WPCBookTbl* pTbl = WeaponCenterCfg::Instance().GetWPCBookTbl(nBookID);
	if(pTbl ==NULL)
		return false;
	pBaseMgr->AddBook(pTbl,nCnt,nLV);

	SYS_LOG(pUser->GetUid(),LT_ADIMN_AddBook,0,0,nBookID<<nCnt);
	return true;
}

bool DealAdminEvent::Admin_DelBookExp(User* pUser,int nBookID,int nExp)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();
    BaseManager* pBaseMgr = pPlayer->GetBaseManager();
    if(pBaseMgr==NULL)
        return false;
    pBaseMgr->DelBookExp(pUser, nBookID,nExp);
    return true;
}

bool DealAdminEvent::Admin_AddEmblem(User* pUser,int nEmblemType,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	pBaseMgr->AddEmblem(nEmblemType,nCnt,EMBLEM_ADD_TYPE_DEF_Admin);
	eh_->SendWPCEmblemLst(pUser);

	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDEMBLEM,0,0,nEmblemType<<nCnt);
	return true;
}
bool DealAdminEvent::Admin_AddItem(User* pUser, int nItemType, int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	Player* pPlayer = pUser->GetPlayer();

	pPlayer->AddBagItem(nItemType, nCnt,true,true);
	pPlayer->SendBagList();
	eh_->SendVirtualItemList(pUser);

	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDITEM,0,0,nItemType<<nCnt);
	return true;
}

bool DealAdminEvent::Admin_DelBldCnt(User* pUser,int nBldType,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	vector<DB_Building*>* pBldLst = pBaseMgr->GetBuildingLst(nBldType);
	if(pBldLst==NULL)
		return false;

	if(nCnt > (int)pBldLst->size())
	{
		nCnt = pBldLst->size();
	}

	vector<DB_Building*> nBldSortLst = *pBldLst;
	sort(nBldSortLst.begin(),nBldSortLst.end(),compareBldByLv);
	//map<int,DB_Building*> nBuildingMap;

	for(int i=0;i<nCnt;i++)
	{
		DB_Building* pDBBld = nBldSortLst[nBldSortLst.size()-1];
		int nBldID	 = pDBBld->id();
		int nBldType = pDBBld->type();
		int nLV		 = pDBBld->level();
		pBaseMgr->DelDBBuilding(nBldID);
		nBldSortLst.pop_back();
		SYS_LOG(pUser->GetUid(),LT_ADMIN_DELBLD,0,0,nBldID<<nBldType<<nLV);
	}
	pBaseMgr->UpdateBuildingMap();
	return true;

}

bool DealAdminEvent::Admin_RecoverArmyHP(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	pBaseMgr->RecoverAllHurtArmy();
	return true;
}

bool DealAdminEvent::Admin_RecoverBldHP(User* pUser)
{
	return false;
}

bool DealAdminEvent::Admin_SetInit(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	eh_->removeUserFdMap(pUser->fd(), pUser->GetUid());

	pUser->ClearForBlackList();

	eh_->getDataHandler()->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_SetScienceLvl(User* pUser,int64 nRes1, int64 nRes2)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}
	ScienceInfo* pScienceInfo = ScienceInfoCfg::Instance().GetScienceInfo((int)nRes1,(int)nRes2);
	if (pScienceInfo)
	{
		pUser->GetPlayer()->SetScienceLevel((int)nRes1,(int)nRes2,pScienceInfo->combatvalue);

		pUser->GetPlayer()->PlayerRecord(PLAYER_RECORD_TECHNOLOGY,pUser->GetPlayer()->GetScienceLevelAll(),true);

		RseScienceLevelUp rsesLevelUp;
		rsesLevelUp.set_sid((int)nRes1);
		rsesLevelUp.set_result(0);

		ScienceLevel* pSlevel = rsesLevelUp.mutable_sciencelevel();
		pUser->FillScienceInfo(pSlevel);

		string text;
		rsesLevelUp.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(),S2C_RseScienceLevelUp,text);
	}
	return true;
}

bool DealAdminEvent::Admin_AddMorale(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pCntrMgr->AddMorale(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_AddThew(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pCntrMgr->AddThew(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_AddZhangong(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pCntrMgr->AddZhanGong(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_AddCountryBox(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	DB_Country* pDBCountry = pCntrMgr->GetDBCountry();
	pDBCountry->set_boxcnt(pDBCountry->boxcnt()+nCnt);
	if(pDBCountry->boxcnt()<0)
	{
		pDBCountry->set_boxcnt(0);
	}
	SYS_LOG(pUser->GetPlayer()->GetUserID(),LT_CNTR_AddCoin,0,0,5<<"box"<<nCnt<<pDBCountry->boxcnt()<<(int)E_ADD_COIN_Admin);

	return true;
}	
bool DealAdminEvent::Admin_AddGuildScore(User* pUser,int nCnt)
{
	if(pUser==NULL||nCnt==0)
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pCntrMgr->AddGScore(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_AddGuildBldValue(User* pUser,Event* e)
{
	if(pUser==NULL)
		return false;
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	if(req->count()==0)
		return false;

	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
	if(pDBGuild->guildid()<=0)
		return false;
	SYS_LOG(pUser->GetPlayer()->GetUserID(),LT_ADMIN_ADDGUILDBLDVALUE,0,0,pDBGuild->guildid()<<pDBGuild->mutable_guildbase()->totalbldvalue()<<req->count());
	
	req->set_guildid(pDBGuild->guildid());
	e->set_state(Status_Game_To_Country);
	eh_->sendEventToCountry(e,pUser->GetUserRegion(true));

	return true;
}

bool DealAdminEvent::Admin_SetGuildScienceLV(User* pUser,int nScienceID,int nLV)
{
	if(pUser==NULL||nScienceID<GS_Start||nScienceID>=GS_Cnt)
		return false;
	const GuildScienceTbl* pTbl = GuildInfoCfg::Instance().GetGuildScienceTbl(nScienceID);
	if(pTbl==NULL||nLV<0||nLV>pTbl->GetMaxLV())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
	for(int i=pDBGuild->science_size();i<GS_Cnt;i++)
	{
		DB_GuildScience* pDBScience = pDBGuild->add_science();
		pDBScience->set_id(i);
		pDBScience->set_exp(0);
		pDBScience->set_level(0);
	}

	DB_GuildScience* pDBScience = pDBGuild->mutable_science(nScienceID);
	int nOldLV = pDBScience->level();
	if(nLV==nOldLV)
		return true;
	pDBScience->set_level(nLV);
	pDBScience->set_exp(pTbl->m_lstCostExp[nLV]);
	if(nScienceID >= GS_AddTankAtk && nScienceID <= GS_AddPlaneHP)
	{
		pBaseMgr->UpdateGuildScienceAddProp();
		pBaseMgr->UpdateArmyHP();
	}
	
	pBaseMgr->UpdateAllCombatPowerValue_Guild();
	pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());

	SYS_LOG(pUser->GetPlayer()->GetUserID(),LT_ADMIN_SETGUILDSCIENCELV,0,0,nScienceID<<nLV<<nOldLV);

	return true;
}

bool DealAdminEvent::Admin_SetGuildBldLV(User* pUser,Event* e)
{
	if(pUser==NULL)
		return false;
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	int nBldID	= req->bldid();
	int nLV		= req->level();
	const GuildBldTbl* pTbl = GuildInfoCfg::Instance().GetGuildBuildingTbl(nBldID);
	if(pTbl==NULL||nLV<0||nLV>pTbl->GetMaxLV())
		return false;
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
	if(pDBGuild->guildid()<=0)
		return false;
	SYS_LOG(pUser->GetPlayer()->GetUserID(),LT_ADMIN_SETGUILDBLDLV,0,0,pDBGuild->guildid()<<nBldID<<nLV);
	
	req->set_guildid(pDBGuild->guildid());
	e->set_state(Status_Game_To_Country);
	eh_->sendEventToCountry(e,pUser->GetUserRegion(true));

	return true;
}

bool DealAdminEvent::Admin_ReloadCountryRank()
{
	GameDataHandler* pUserManager = eh_->getDataHandler();
	eh_->SendCountryNumQuery();
	for(int i=0;i<serverConfig.region_num();i++)
	{
		GameRgnDataHandler* pRDH = pUserManager->GetRgnDataHandler(i);
		if(pRDH==NULL)
			continue;
		for(int j=C_Start;j<C_UserCoutryEnd;j++)
		{
			if(CountryInfoCfg::Instance().GetZhanGongRankFromWeb())
			{
				HttpRequireHandler::Instance().SafePushHttpRequest("",0,i,WEBREQ_CountryRank,j);
			}
			else
			{
				eh_->Send4LastDayZhanGongRank(j,i);
			}
		}
	}
	return true;
}

bool DealAdminEvent::Admin_AddMoonCnt(User* pUser,int cnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	Player* pPlayer = pUser->GetPlayer();

	DB_MoonCake* pDBMoonCake = pPlayer->GetDBPlayer()->mutable_mooncake();

	int oldcnt = pDBMoonCake->accgetflag2cnt();
	pDBMoonCake->set_accgetflag2cnt( pDBMoonCake->accgetflag2cnt() + cnt );

	if(pUser->Online()&&pUser->fd()>0)
	{
// 		RseMoonOpen rseRet;
// 
// 		time_t ltNow = time(NULL);
// 		const MoonBaseTbl* pTbl = MoonCakeCfg::Instance().GetMoonBaseTbl(ltNow);
// 
// 		if(pTbl==NULL)
// 			return false;
// 
// 		rseRet.set_goldcardcnt(pPlayer->GetBagItemCnt(pTbl->m_nSendGoldID));
// 		rseRet.set_silvercardcnt(pPlayer->GetBagItemCnt(pTbl->m_nSendSilverID));
// 		rseRet.set_goldlevel(pDBMoonCake->goldlevel());
// 		rseRet.set_silverlevel(pDBMoonCake->silverlevel());
// 		rseRet.set_accstakecnt(pDBMoonCake->dayaccsuccnt());
// 		rseRet.set_mooncnt(pDBMoonCake->daygetflag1cnt());
// 		rseRet.set_accmooncnt(pDBMoonCake->accgetflag2cnt());
// 		//rseRet.set_mooncnt(pDBMoonCake->mooncakecnt());
// 		//rseRet.set_accmooncnt(pDBMoonCake->accsucmooncakecnt());
// 		//rseRet.set_accmooncnt(pDBMoonCake->maxsucmooncakecnt());
// 		string text;
// 		rseRet.SerializeToString(&text);
// 		eh_->sendDataToUser(pUser->fd(), S2C_RseMoonOpen, text);

	}
	else
	{
		
	}

	SYS_UserStat4WebRank(pUser,"MoonHappy","",pDBMoonCake->accgetflag2cnt());

	SYS_LOG(pUser->GetUid(),LT_MoonAddCnt,0,0,oldcnt<<cnt);
	return true;
}

bool DealAdminEvent::Admin_ClearStrategic(User* pUser,int nType)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseManager = pPlayer->GetBaseManager();
	if(pBaseManager)
	{
		pBaseManager->ClearStrategicDataAdmin(nType);
		RseStrategicArmyInfo rseArmyInfo;
		pBaseManager->FillAllStrategicArmyInfo(rseArmyInfo);
		string text;
		rseArmyInfo.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseStrategicArmyInfo, text);
	}

	return true;
}

bool DealAdminEvent::Admin_PveSetCustom(User* pUser,int nCustom,int nRefresh,int nBuyRefresh,int nFail)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer == NULL)
	{
		return false;
	}

	PVEFightManager& rPVEFightManager = pPlayer->GetPVEFightManager();
	return rPVEFightManager.SetAdminData(nCustom,nRefresh,nBuyRefresh,nFail);
}

bool DealAdminEvent::Admin_ArenaFightSet(Event* e,User* pUser,int nType,int nValue1,int nValue2)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer == NULL)
	{
		return false;
	}

	DB_ArenaInfo* pDBArenaInfo = pPlayer->GetDBPlayer()->mutable_base(0)->mutable_arenainfo();
	CountryArenaMgr& rCountryArenaMgr = pPlayer->GetCountryArenaMgr();

	//uid,type,value1,value2
	//1成就 2成就领取 3上次排名 4上次排名领奖 5当前可战斗次数 6已购买次数 7增加战斗积分 8清除成就完成数据、成就、成就领取标记 13本期积分
	if(nType == 1)
	{
		if(rCountryArenaMgr.IsHasDoneAchieve(nValue1) == true)
		{
			rCountryArenaMgr.MarkDoneAchieve(nValue1,nValue2>0);
		}
	}

	if(nType == 2)
	{
		if(rCountryArenaMgr.IsHasGainAchieve(nValue1) == true)
		{
			rCountryArenaMgr.MarkGainAchieve(nValue1,nValue2>0);
		}
	}

	if(nType == 3)
	{
		pDBArenaInfo->set_nrankselflast(nValue1);
	}

	if(nType == 4)
	{
		pDBArenaInfo->set_blastrankget(nValue1 > 0);
	}

	if(nType == 5)
	{
		pDBArenaInfo->set_ncanfighttimes(nValue1);
	}
	
	if(nType == 6)
	{
		pDBArenaInfo->set_nbuyfighttimes(nValue1);
	}

	if(nType == 7)
	{
		pDBArenaInfo->set_nintegralscore(pDBArenaInfo->nintegralscore()+nValue1);
	}

	if(nType == 8)
	{
		if(nValue1 == 1)
		{
			pDBArenaInfo->clear_arenachievenodes();
		}
		if(nValue1 == 2)
		{
			pDBArenaInfo->clear_arenachieveflag();
		}
		if(nValue1 == 3)
		{
			pDBArenaInfo->clear_arenagainflag();
		}
	}

	if(nType == 9)
	{
		rCountryArenaMgr.SetArenaIndex(nValue1,false);

		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(rCountryArenaMgr.GetCountryId());
			req->set_armytype(nValue1);
		}

		e->set_state(Status_Normal_To_Fight);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
	}

	if(nType == 10)
	{
		rCountryArenaMgr.SetArenaIndex(nValue1,false);

		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(rCountryArenaMgr.GetCountryId());
			req->set_armytype(ArenaNpcSet::Instance().GetArenaNpcSetUnitByFightPoint(pUser->GetPlayer()->GetCombatPower()));
		}

		e->set_state(Status_Normal_To_Fight);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
	}

	if(nType == 11)
	{
		for(int i=0;i<pDBArenaInfo->arenaindexgain_size();i++)
		{
			DB_ArenaIndexGain* pArenaIndexGain = pDBArenaInfo->mutable_arenaindexgain(i);
			if(pArenaIndexGain)
			{
				pArenaIndexGain->set_bgain(false);
			}
		}
	}

	if(nType == 12)
	{
		pDBArenaInfo->set_bendaccount(false);
	}

	if(nType == 13)
	{
		pDBArenaInfo->set_nnowscore(pDBArenaInfo->nnowscore()+nValue1);

		//同步竞技场初始化数据
		RseArenaInfo rRseArenaInfo;
		string text;
		text.clear();
		rCountryArenaMgr.FillArenaInfoData(&rRseArenaInfo,E_FILL_ARENA_TYPE_INITDATA);
		rRseArenaInfo.SerializeToString(&text);
		eh_->sendDataToUser(pUser->fd(), S2C_RseArenaInfo, text);

		//更新变化
		rCountryArenaMgr.SendUserGainScoreActive(pUser->GetUid(),pUser->GetUserRegion(true),eh_);
		pUser->SetUserLiteCache();
	}
	if(nType == 14)
	{
		pDBArenaInfo->set_nlastactiverank(nValue1);
		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(rCountryArenaMgr.GetCountryId());
			req->set_armytype(nValue1);
		}
		e->set_state(Status_Normal_To_Fight);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
	}
	SYS_LOG(pUser->GetUid(), LT_ADIMN_ArenaSet, 0, 0, nType<<nValue1<<nValue2);
	return true;
}

int DealAdminEvent::Admin_PveFightStart(User* pUser,int nTimes,int nKind)
{
	if(pUser==NULL || pUser->GetPlayer()==NULL || !pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	PVEFightManager& rPVEFightManager = pUser->GetPlayer()->GetPVEFightManager();

	time_t begintm = Clock::getCurrentSystemTime();

	if(nKind == 1)	//测试本地数据初始化所需时间
	{
		for(int i = nTimes; i > 0; i --)
		{
			rPVEFightManager.CheckFillFight();
		}
	}

	if(nKind == 2) //测试直接Fight战斗需要时间
	{
		rPVEFightManager.StartFight(eh_,STRATEGIC_PVEFIGHT_OPT_FIGHT,nTimes - 1);
	}

	return (int)(Clock::getCurrentSystemTime() - begintm);
}

bool DealAdminEvent::Admin_ReInitDB(User* pUser)
{
	if(pUser==NULL)
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();

	if(pPlayer == NULL || pPlayer->CanUse() == false)
	{
		return false;
	}

	pPlayer->InitDB(pPlayer->GetDBPlayer());

	return true;
}

bool DealAdminEvent::Admin_SetGMTestValue(User* pUser,int64 nRes1, int64 nRes2)
{
	return false;
}

bool DealAdminEvent::Admin_AddCoupons(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	pPlayer->AddCoupon(nCnt);
	eh_->SendUserInfo(pUser);
	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDCOUPON,0,0,nCnt);
	return true;
}


bool DealAdminEvent::Admin_AddAdmiralTimes(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;

	Player* pPlayer = pUser->GetPlayer();
	if ( pPlayer == NULL )
	{
		return false;
	}

	time_t tNow = time(NULL);
	for ( int i = 0; i < CostGiftCfg::Instance().GetTblNum(); i++ )
	{
		const CostGiftTbl* pCostGiftTbl = CostGiftCfg::Instance().GetCostGiftTblByNum( i );

		if ( pCostGiftTbl == NULL )
		{
			continue;
		}

		if ( pCostGiftTbl->m_nAdmiralTimes == 0 || !pCostGiftTbl->IsCurrentTime() )
		{
			continue;
		}

		DB_CostGift* pDBCostGift = pPlayer->GetDBCostGift( pCostGiftTbl->m_nGiftID );
		if ( pDBCostGift == NULL )
		{
			pDBCostGift = pPlayer->GetDBPlayer()->add_costgift();
			pDBCostGift->set_id( pCostGiftTbl->m_nGiftID );
			pDBCostGift->set_lasttimes((int)tNow);
		}

		pDBCostGift->set_admiraltimes( pDBCostGift->admiraltimes() + nCnt );
		if ( pCostGiftTbl->m_nType == COST_GIFT_TYPE_DATE )
		{
			pDBCostGift->set_lasttimes((int)tNow);
		}
	}

	SYS_LOG(pUser->GetUid(),LT_ADMIN_ADDADMIRALTIMES,0,0,nCnt);
	return true;
}

bool DealAdminEvent::Admin_FinishGuide(User* pUser)
{
	if (pUser == NULL || pUser->GetPlayer() == NULL || !pUser->GetPlayer()->CanUse())
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	if ( pPlayer == NULL )
	{
		return false;
	}
	pPlayer->SetTutorialStage(1,170);
	pPlayer->FinishTutorial();
	SYS_LOG(pUser->GetUid(), LT_ADMIN_FINISHGUIDE, 0, 0, 170);
	return true;
}

void DealAdminEvent::AddEvent_AddEquip(EventQueue* eq, int gid, int fd, int64 uid, int nEquipID, int nCnt)
{
	Event* e = eq->allocateEvent();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);

	req->set_opttype(AOT_ADMIN_ADD_EQUIP);

	req->set_gid(gid);
	req->set_equipid(nEquipID);//////////////////////
	req->set_count(nCnt);
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

void DealAdminEvent::AddEvent_DelEquip(EventQueue* eq, int gid, int fd, int64 uid, int nEquipID)
{
    Event* e = eq->allocateEvent();
    e->set_cmd(EVENT_ADMIN_OPT);
    e->set_state(Admin_AG_Req);
    e->set_time(-1);
    e->set_uid(uid);
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
    req->set_uid(uid);

    req->set_opttype(AOT_ADMIN_DEL_EQUIP);

    req->set_gid(gid);
    req->set_equipid(nEquipID);//////////////////////
    req->set_adminfd(fd);
    eq->safePushEvent(e);
}

bool DealAdminEvent::Admin_AddEquip(User* pUser,int nEquipID,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const EquipmentTbl* pTbl = EquipCfg::Instance().GetEquipmentTbl(nEquipID);
	if(pTbl ==NULL)
		return false;
	pBaseMgr->AddEquip(pTbl,nCnt);

	SYS_LOG(pUser->GetUid(),LT_ADIMN_AddEquip,0,0,nEquipID<<nCnt);
	return true;
}

bool DealAdminEvent::Admin_DelEquip(User* pUser,int nEquipID)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();
    BaseManager* pBaseMgr = pPlayer->GetBaseManager();
    if(pBaseMgr==NULL)
        return false;
    const EquipmentTbl* pTbl = EquipCfg::Instance().GetEquipmentTbl(nEquipID);
    if(pTbl ==NULL)
        return false;
    int nDelNum = pBaseMgr->DelEquip(pTbl);

    SYS_LOG(pUser->GetUid(),LT_ADIMN_DelEquip,0,0,nEquipID<<nDelNum);
    return true;
}

bool DealAdminEvent::Admin_WorldArenaSet(Event* e,User* pUser,int nType,int nValue1)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer == NULL)
	{
		return false;
	}
	WorldArenaMgr& rWorldArenaMgr = pPlayer->GetWorldArenaMgr();
	DB_WorldArenaInfo * pDBArenaInfo = rWorldArenaMgr.GetDBWorldArenaInfo();

	//uid,type,value1,value2
	// 1上期排名 2上次排名领奖 3当前可战斗次数 4已购买次数 

	if(nType == 1)
	{
		pDBArenaInfo->set_nrankselflast(nValue1);
	}

	if(nType == 2)
	{
		pDBArenaInfo->set_blastrankget(nValue1 > 0);
	}

	if(nType == 3)
	{
		pDBArenaInfo->set_ncanfighttimes(nValue1);
	}

	if(nType == 4)
	{
		pDBArenaInfo->set_nbuyfighttimes(nValue1);
	}
	if(nType == 5)
	{
		pDBArenaInfo->set_nrankselfyesterday(nValue1);
	}
	if(nType == 6)
	{
		rWorldArenaMgr.SetWorldArenaIndex(nValue1);

		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(nValue1);
		}

		e->set_state(Status_Normal_To_Fight);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
	}
	SYS_LOG(pUser->GetUid(), LT_ADIMN_WorldArenaSet, 0, 0, nType<<nValue1);
	return true;
}

bool DealAdminEvent::Admin_HeroArenaSet(Event* e,User* pUser,int nType,int nValue1)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer == NULL)
	{
		return false;
	}
	HeroArenaMgr& rHeroArenaMgr = pPlayer->GetHeroArenaMgr();
	DB_HeroArenaInfo * pDBArenaInfo = rHeroArenaMgr.GetDBHeroArenaInfo();

	//uid,type,value1,value2
	// 1上期排名 2上次排名领奖 3当前可战斗次数 4已购买次数 

	if(nType == 1)
	{
		pDBArenaInfo->set_nrankselflast(nValue1);
	}

	if(nType == 2)
	{
		pDBArenaInfo->set_blastrankget(nValue1 > 0);
	}

	if(nType == 3)
	{
		pDBArenaInfo->set_ncanfighttimes(nValue1);
	}

	if(nType == 4)
	{
		pDBArenaInfo->set_nbuyfighttimes(nValue1);
	}
	if(nType == 5)
	{
		pDBArenaInfo->set_nrankselfyesterday(nValue1);
	}
	if(nType == 6)
	{
		rHeroArenaMgr.SetHeroArenaIndex(nValue1);

		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(nValue1);
		}

		e->set_state(Status_Normal_To_Fight);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
	}
	if(nType == 7)
	{
		pPlayer->AddHeroArenaScore(nValue1);
	}
	SYS_LOG(pUser->GetUid(), LT_ADIMN_HeroArenaSet, 0, 0, nType<<nValue1);
	return true;
}

bool DealAdminEvent::Admin_RegionArenaSet(Event* e,User* pUser,int nType,int nValue1)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}

	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer == NULL)
	{
		return false;
	}
	RegionArenaMgr& rRegionArenaMgr = pPlayer->GetRegionArenaMgr();
	DB_RegionArenaInfo * pDBArenaInfo = rRegionArenaMgr.GetDBRegionArenaInfo();

	//uid,type,value1
	// 1上期排名 2上次排名领奖 3当前可战斗次数 4已购买次数 5昨日排名 6设置积分 7设置报名 8刷新排名

	if(nType == 1)
	{
		pDBArenaInfo->set_nrankselflast(nValue1);
	}

	if(nType == 2)
	{
		pDBArenaInfo->set_blastrankget(nValue1 > 0);
	}

	if(nType == 3)
	{
		pDBArenaInfo->set_ncanfighttimes(nValue1);
	}

	if(nType == 4)
	{
		pDBArenaInfo->set_nbuyfighttimes(nValue1);
	}
	if(nType == 5)
	{
		pDBArenaInfo->set_nrankselfyesterday(nValue1);
	}
	if(nType == 6)
	{
		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(nValue1);
		}

		e->set_state(Status_Game_To_Region);
		eh_->sendEventToRegion(e,pUser->GetUserRegion(true));
	}
	if(nType == 7)
	{
		if(nValue1>0)
		{
			eh_->SendRegionArenaApply(pUser,true);
		}
		else
		{
		
			pDBArenaInfo->set_bapply(false);
			pDBArenaInfo->set_score(0);
			rRegionArenaMgr.SetRegionArenaIndex(-1);
			e->set_state(Status_Game_To_Region);
			eh_->sendEventToRegion(e,pUser->GetUserRegion(true));
		}

	}
	if(nType == 8)
	{
		Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
		if(req)
		{
			req->set_bldtype(nValue1);
		}

		e->set_state(Status_Game_To_Region);
		eh_->sendEventToRegion(e,pUser->GetUserRegion(true));
	}
	SYS_LOG(pUser->GetUid(), LT_ADIMN_RegionArenaSet, 0, 0, nType<<nValue1);
	return true;
}

bool DealAdminEvent::Admin_ClearPassword(User* pUser,int nTime)
{
	if (pUser == NULL || pUser->GetPlayer() == NULL || !pUser->GetPlayer()->CanUse())
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	if ( pPlayer == NULL )
	{
		return false;
	}
	DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
	if ( pDBPlayer == NULL )
	{
		return false;
	}

	pDBPlayer->clear_password();
	DB_PassWord* pDBPassWord = pDBPlayer->mutable_password();

	int nFreezeTime = time(NULL) + nTime*60;
	pDBPassWord->set_freezetime( nFreezeTime );

	SYS_LOG(pUser->GetUid(), LT_ADMIN_RESET_PASSWORD, 0, 0, nTime);

	return true;
}

bool DealAdminEvent::Admin_AddBuff(Event* e,User* pUser,int nBuffId)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	pPlayer->AddBuff(nBuffId);
	return true;
}
bool DealAdminEvent::Admin_SetOfficerPosition(Event* e,User* pUser,int nPosition,int nFlag)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	CountryManager* pCntrMgr = pPlayer->GetCountryManager();
	if(pCntrMgr == NULL)
	{
		return false;
	}
	if(nPosition>Officer_NULL&&nPosition<Officer_MAX)
	{
		e->set_state(Status_Game_To_Country);
		eh_->sendEventToCountry(e,pUser->GetUserRegion(true));
		SYS_LOG(pUser->GetUid(), LT_ADIMN_SetOfficerPosition, 0, 0,nPosition<<nFlag);
	}
	return true;
}

bool DealAdminEvent::Admin_DelEmblem(User* pUser,int nEmblemType,int nExp)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	pBaseMgr->AdminDelEmblem(nEmblemType,nExp);

	SYS_LOG(pUser->GetUid(),LT_ADMIN_DELEMBLEM,0,0,nEmblemType<<nExp);
	return true;
}

bool DealAdminEvent::Admin_DelEmblemExp(User*pUser,int EmblemID,int nExp)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();
    BaseManager* pBaseMgr = pPlayer->GetBaseManager();
    if(pBaseMgr==NULL)
        return false;
    pBaseMgr->AdminDelEmblemExp(pUser, EmblemID,nExp);
    return true;
}

bool DealAdminEvent::Admin_DelWpccom(User* pUser,int nEmblemType,int nExp)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	pBaseMgr->AdminDelWpccom(nEmblemType,nExp);

	SYS_LOG(pUser->GetUid(),LT_ADMIN_DELWPCCOM,0,0,nEmblemType<<nExp);
	return true;
}

bool DealAdminEvent::Admin_DelWpccomExp(User* pUser,int nEmblemID,int nExp)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();
    BaseManager* pBaseMgr = pPlayer->GetBaseManager();
    if(pBaseMgr==NULL)
        return false;
    pBaseMgr->AdminDelWpccomExp(pUser, nEmblemID,nExp);
    return true;
}

bool DealAdminEvent::Admin_SetVip(Event* e,User* pUser,int nType,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	switch(nType)
	{
	case 1:
		{
			pPlayer->SetVipType(nValue);
			eh_->SendUserInfo2Plat(pUser,UpdatePlat_Vip);
		}
		break;
	case 2:
		{
			pPlayer->SetVipGrowValue(nValue);
			pPlayer->CacuVipLevel();
			
		}
		break;
	default:
		return false;
	}
	eh_->SendUserInfo(pUser);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_SetVip, 0, 0,nType<<nValue<<pPlayer->GetVipGrowValue());
	return true;
}

bool DealAdminEvent::Admin_SetEquipStrLv(Event* e,User* pUser,int nEquipType,int nOldLv,int nNewLv)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	if(nOldLv<0||nOldLv>12||nNewLv<0||nNewLv>12)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Bases* pDBBases = pBaseMgr->GetDBBases();
	if(pDBBases==NULL)
		return false;
	DB_EquipInfo* pDBEquipData = pDBBases->mutable_equipdata();
	if(pDBEquipData==NULL)
		return false;
	bool bFind = false;
	for(int i=0;i<pDBEquipData->equiplist_size();i++)
	{
		DB_Equip* pDBEquip = pDBEquipData->mutable_equiplist(i);
		if(pDBEquip==NULL||pDBEquip->id()<=0||pDBEquip->equipid()<=0||pDBEquip->equipid()!=nEquipType||pDBEquip->strengthenlevel()!=nNewLv)
			continue;
		bFind = true;
		pDBEquip->set_strengthenlevel(nOldLv);
		SYS_LOG(pUser->GetUid(), LT_ADIMN_SetEquipStrLv, 0, 0,"find"<<nEquipType<<nOldLv<<nNewLv<<pDBEquip->id()<<pDBEquip->strengthenlevel());
		break;
	}
	if(!bFind)
	{
		SYS_LOG(pUser->GetUid(), LT_ADIMN_SetEquipStrLv, 0, 0,"nfind"<<nEquipType<<nOldLv<<nNewLv<<0<<0);
	}
	else
	{
		pBaseMgr->UpdateAdmiralEffect();
		pBaseMgr->UpdateAllCombatPowerValue_Admiral();
		pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
		pBaseMgr->UpdateAdmiralInuseScore();
	}
	if(pUser->fd()!=0&&pUser->Online())
	{
		eh_->SendEquipList(pUser);
	}
	pUserManager->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_SpecialValue(Event* e,User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	if ( nValue <= 0 )
	{
		return false;
	}

	time_t tNow = time(NULL);

	for ( int i = 0; i < CostGiftCfg::Instance().GetTblNum(); i++ )
	{
		const CostGiftTbl* pCostGiftTbl = CostGiftCfg::Instance().GetCostGiftTblByNum( i );

		if ( pCostGiftTbl == NULL )
		{
			continue;
		}

		if ( pCostGiftTbl->m_nCostCredit == 0 || !pCostGiftTbl->IsCurrentTime())
		{
			continue;
		}

		DB_CostGift* pDBCostGift = pPlayer->GetDBCostGift( pCostGiftTbl->m_nGiftID );
		if ( pDBCostGift == NULL )
		{
			pDBCostGift = pPlayer->GetDBPlayer()->add_costgift();
			pDBCostGift->set_id( pCostGiftTbl->m_nGiftID );
			pDBCostGift->set_lasttimes((int)tNow);
		}

		//需要先抽奖，然后充值
		if ( pDBCostGift->drawtimes() < pCostGiftTbl->m_nDrawTimes )
		{
			continue;
		}

		pDBCostGift->set_credit( pDBCostGift->credit() + nValue );
		if ( pCostGiftTbl->m_nType == COST_GIFT_TYPE_DATE )
		{
			pDBCostGift->set_lasttimes((int)tNow);
		}

		if ( ( pCostGiftTbl->IsCurrentCanGainGift( nValue,pDBCostGift->yellowtimes(),pDBCostGift->drawtimes(),pDBCostGift->admiraltimes() ) && !pCostGiftTbl->m_bNeedGet && !pCostGiftTbl->m_bCostCreditPlus) ||
			( pCostGiftTbl->IsCurrentCanGainGift( pDBCostGift->credit(),pDBCostGift->yellowtimes(),pDBCostGift->drawtimes(),pDBCostGift->admiraltimes() ) && !pCostGiftTbl->m_bNeedGet && pCostGiftTbl->m_bCostCreditPlus) )//判断是否可以领取(可累计,不可累积)
		{
			//可以获取奖励

			//判断领取礼物数量
			if ( pDBCostGift->count() < pCostGiftTbl->m_nTimes || pCostGiftTbl->m_nTimes == 0 )
			{
				//礼物
				int medel = (int)(pCostGiftTbl->m_nMedal);
				int oil = (int)(pCostGiftTbl->m_nOil);
				int64 nAddMetal = pPlayer->AddRes(RC_Metal,medel);
				int64 nAddOil = pPlayer->AddRes(RC_Oil,oil);
				//int nAddBasePoints = (int)ceil(double(medel+oil)/50); 

				if ( pCostGiftTbl->m_nGetCredit > 0 )
				{
					pPlayer->AddCredit( pCostGiftTbl->m_nGetCredit,CREDIT_TYPE_DEF_YELLOW );
				}

				pPlayer->AddHonor(pCostGiftTbl->m_nHonor,HONOR_TYPE_DEF_GIFT);
				pPlayer->AddFeats(pCostGiftTbl->m_nFeats,FEATS_TYPE_DEF_QPLUS);

				RseUserInfo userinfo;

				string text;
				pUser->FillUserInfo(userinfo); 
				userinfo.SerializeToString(&text);
				eh_->sendDataToUser(pUser->fd(), S2C_RseUserInfo, text);

				text.clear();

				BaseManager* pBaseManager = pPlayer->GetBaseManager();

				if ( pBaseManager == NULL )
				{
					break;
				}

				//pBaseManager->AddBasePoints(nAddBasePoints);
				//pPlayer->CheckUpgrade();

				//SYS_LOG(m_pUser->GetUid(),LT_FinishTask,0,0,nID<<pGiftTbl->m_nMedal<<pGiftTbl->m_nOil);

				RseBuildingUpdate rseUP;
				rseUP.set_type(7);
				for(int i=0;i<MAX_COST_GIFT_ARMY_CNT;i++)
				{
					int nArmyID = pCostGiftTbl->m_lstArmyType[i];
					if(nArmyID>0)
					{
						for(int j=0;j<pCostGiftTbl->m_lstArmyCnt[i];j++)
						{
							DB_Army* pDBArmy = pBaseManager->CreateArmyByTask(nArmyID);
							if(pDBArmy)
							{
								Army* pArmy = rseUP.add_armylst();
								pBaseManager->FillArmyData(pDBArmy,pArmy);
								SYS_LOG(pUser->GetUid(),LT_COST_GIFT_ADDARMY,0,0,nArmyID);
							}
						}

					}
				}

				if(rseUP.armylst_size()>0)
				{
					text.clear();
					rseUP.SerializeToString(&text);
					eh_->sendDataToUser(pPlayer->GetParent()->fd(), S2C_RseBuildingUpdate, text);
				}

				for(int i=0;i<MAX_COST_GIFT_ITEM_CNT;i++)
				{
					int nItemID = pCostGiftTbl->m_lstItemType[i];
					if(nItemID>0)
					{
						pPlayer->AddBagItem( nItemID, pCostGiftTbl->m_lstItemCnt[i] );
					}
				}
				eh_->SendBagList(pUser);

				const AdmiralTbl* pAdmTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl( pCostGiftTbl->m_nAdmiralID );
				if(pAdmTbl!=NULL)
				{
					DB_Admiral* pDBAdm = pBaseManager->GetDBAdmiral( pCostGiftTbl->m_nAdmiralID );
					if(pDBAdm==NULL)
					{// 添加将领
						pDBAdm = pBaseManager->AddAdmiral( pCostGiftTbl->m_nAdmiralID );
						if(pDBAdm==NULL)
							return false;
						if( pCostGiftTbl->m_nAdmiralLV > 1 )
						{
							pDBAdm->set_level( pCostGiftTbl->m_nAdmiralLV );
							pDBAdm->set_exp(AdmiralInfoCfg::Instance().GetUpgradeNeedExp( pCostGiftTbl->m_nAdmiralLV )+10);
						}
						pBaseManager->CacuAdmiralFinalProp(pDBAdm);
						SYS_LOG(pUser->GetUid(),LT_AddAdmiralCostGift,0,0,pDBAdm->admiralid()<<pDBAdm->level());
					}
					else
					{
						if(pAdmTbl->m_nQuality == 4)
						{
							DB_AdmiralTrainLV* pDBTrainLV = pDBAdm->mutable_trainlv();
							pDBTrainLV->set_cnt(pDBTrainLV->cnt()+1);
							int nLV = AdmiralInfoCfg::Instance().CacuTrainSkillLV(pDBTrainLV->cnt());
							if(pDBTrainLV->level()!=nLV)
							{
								pDBTrainLV->set_level(nLV);
							}
							pBaseManager->CacuAdmiralFinalProp(pDBAdm,pAdmTbl);
							if(pDBAdm->usetype())
							{
								pBaseManager->UpdateAdmiralEffect();
								pBaseManager->UpdateArmyHP();
								pBaseManager->UpdateBldHP();
								pBaseManager->UpdateAllHeroHP();
							}
							SYS_LOG(pUser->GetUid(),LT_AdmiralCostGiftTrain,0,0,pCostGiftTbl->m_nAdmiralID<<1);
						}
					}

				}

				//记录数量
				pDBCostGift->set_count( pDBCostGift->count() + 1 );
				pDBCostGift->set_credit( 0 );
				SYS_LOG(pUser->GetUid(), LT_COST_GIFT_GET, 0, 0,pDBCostGift->id()<<pCostGiftTbl->m_nGiftID);
			}
		}
	}

	const CostGiftTbl* pCostGiftTbl = CostGiftCfg::Instance().GetCostGiftTbl( 10060 );
	if( pCostGiftTbl != NULL )
	{
		//充值排行榜记录
		//time_t tBeginTime = GetDayTimeByStr( pConstantSetUnit->m_strArgs[0].c_str(), pConstantSetUnit->m_strArgs[1].c_str() );
		//time_t tEndTime   = GetDayTimeByStr( pConstantSetUnit->m_strArgs[2].c_str(), pConstantSetUnit->m_strArgs[3].c_str() );

		if ( pPlayer->GetDBPlayer()->lastcosttime() <= pCostGiftTbl->m_tBeginTime )
		{
			//清除上次数据
			pPlayer->GetDBPlayer()->set_playercost( 0 );
		}
		time_t now = time(NULL);
		bool rightTime = false;

		if ( now >= pCostGiftTbl->m_tBeginTime && now <= pCostGiftTbl->m_tEndTime )
		{
			pPlayer->GetDBPlayer()->set_playercost( pPlayer->GetDBPlayer()->playercost() + nValue );

			if ( pPlayer->GetDBPlayer()->playercost() > 50000 )
			{
				SYS_UserStat4WebRank(pUser,"PlayCostYuandan","",pPlayer->GetDBPlayer()->playercost());
			} 
			pPlayer->GetDBPlayer()->set_lastcosttime( now );
		}
	}

	SYS_LOG(pUser->GetUid(), LT_ADIMN_SpecialValue, 0, 0,nValue);
}

bool DealAdminEvent::Admin_ClearNewDayInfo(Event* e,User* pUser)
{
    ///清理国战信息
    Player * pPlayer = pUser->GetPlayer();
    if(pUser==NULL||pPlayer==NULL||!pUser->GetPlayer()->CanUse())
        return false;

    CountryManager* pCntrMgr = pPlayer->GetCountryManager();
    if (pCntrMgr == NULL || pCntrMgr->GetDBCountry() == NULL)
    {
        return false;
    }
    DB_Country* pDBCountry = pCntrMgr->GetDBCountry();

    pDBCountry->set_haspickbox(false);
    pDBCountry->clear_daygiftgetflag();

    DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
    pDBGuild->clear_daycontributecnt();
    pDBGuild->set_haspickwelfare(false);

    SYS_LOG(pUser->GetUid(), LT_ADMIN_CLEARNEWDAYINFO, 0, 0, 0);

    return true;
}


bool DealAdminEvent::Admin_PveBattleSet(Event* e,User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	time_t tNow = time(NULL);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();

	if ( req == NULL )
	{
		return false;
	}

	if ( req->count() == 0 )
	{
		for ( int i = 0; i < NPCInfoCfg::Instance().GetNPCPveBattleTblNum(); i++ )
		{
			const NPCPveBattleTbl* pTbl = NPCInfoCfg::Instance().GetNPCPveBattleTblByNum(i);
			if ( pTbl == NULL )
			{
				continue;
			}
			DB_PvePassValue* pDBPvePassValue = pBaseMgr->GetDBPvePassValue(pTbl->m_nID);
			if ( pDBPvePassValue == NULL )
			{
				continue;
			}

			pDBPvePassValue->set_data(1);

		}
	}

	else if ( req->count() > 0 )
	{
		int nID = req->count();
		int nData = 1;
		if ( nID > 100000 )
		{
			nData = req->count()/100000;
			nID = nID%100000;
		}
		const NPCPveBattleTbl* pTbl = NPCInfoCfg::Instance().GetNPCPveBattleTbl(nID);
		if ( pTbl != NULL )
		{
			DB_PvePassValue* pDB = pBaseMgr->GetDBPvePassValue(pTbl->m_nID);
			if ( pDB == NULL )
			{
				return false;
			}
			if ( pTbl->m_nType == 1 || pTbl->m_nType == 4 )
			{
				pDB->set_data(nData);
			}
			else
			{
				pDB->set_data(1);
			}
		}
	}

	else if ( req->count() < 0 )
	{
		const NPCPveBattleTbl* pTbl = NPCInfoCfg::Instance().GetNPCPveBattleTbl( 0 - req->count() );
		if ( pTbl != NULL )
		{
			DB_PvePassValue* pDB = pBaseMgr->GetDBPvePassValue(pTbl->m_nID);
			if ( pDB == NULL )
			{
				return false;
			}
			pDB->set_data(0);
			pDB->set_cdtime(0);
		}
	}

	SYS_LOG(pUser->GetUid(),LT_ADIMN_PveBattleSet,0,0,req->count());
	return true;
}

bool DealAdminEvent::Admin_AddJunling(Event* e,User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();

	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();

	if ( req == NULL )
	{
		return false;
	}

	if ( req->count() >= 0 )
	{
		pPlayer->AddJunling( req->count() );
	}
	else
	{
		int nCost = -req->count();

		if ( pPlayer->GetJunling() < nCost )
		{
			nCost = pPlayer->GetJunling();
		}

		pPlayer->CostJunling( nCost );
	}

	SYS_LOG(pUser->GetUid(),LT_ADIMN_AddJunLing,0,0,req->count());
}

bool DealAdminEvent::Admin_AddPokers(Event *e, User *pUser)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();

    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();

    if ( req == NULL )
    {
        return false;
    }
    int nPokers = (int)req->res1();
    int nChangeCard = (int)req->res2();
    int nPoints = req->count();

    pPlayer->AddPokers(nPokers, nChangeCard, nPoints);

	SYS_LOG(pUser->GetUid(), LT_ADMIN_AddPokers, 0, 0, nPokers<<nChangeCard<<nPoints);

	return true;
}

bool DealAdminEvent::Admin_AddHero(User* pUser,int nHeroID)
{
	if(pUser==NULL||nHeroID<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	DB_Hero* pDBHero = pBaseMgr->GetDBHero(nHeroID);
	if(pDBHero!=NULL)
		return false;
	pDBHero=pBaseMgr->AddHero(nHeroID);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_AddHero, 0, 0,nHeroID);
	return true;
}

bool DealAdminEvent::Admin_DelHero(User* pUser,int nHeroID)
{
	if(pUser==NULL||nHeroID<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	if(pBaseMgr->DelHero(nHeroID)==false)
		return false;

	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_DelHero, 0, 0,nHeroID);
	return true;
}

bool DealAdminEvent::Admin_AddHeroLetter(User* pUser,int nHeroLetterID,int nCount)
{
	if(pUser==NULL||nHeroLetterID<=0||nCount<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	const HeroLetterTbl* pHeroLetterTbl = HeroInfoCfg::Instance().GetHeroLetterTbl(nHeroLetterID);
	if(pHeroLetterTbl==NULL)
		return false;
	pBaseMgr->AddHeroLetter(nHeroLetterID,nCount);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_AddLetter, 0, 0,nHeroLetterID<<nCount);
	return true;
}

bool DealAdminEvent::Admin_DelHeroLetter(User* pUser,int nHeroLetterID,int nCount)
{
	if(pUser==NULL||nHeroLetterID<=0||nCount<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	pBaseMgr->CostHeroLetter(nHeroLetterID,nCount);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_DelLetter, 0, 0,nHeroLetterID<<nCount);
	return true;
}

bool DealAdminEvent::Admin_SetHero(User* pUser,int nHeroID,int nType,int nValue)
{// 1 血量 2 等级经验 3 星级经验 4 增加 5 减少星级经验
	if(pUser==NULL||nHeroID<=0||nValue<0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	DB_Hero * pDBHero = pBaseMgr->GetDBHero(nHeroID);
	if(pDBHero==NULL)
		return false;
	int nOldValue = 0;
	int nOverValue = 0;
	if(nType==1)
	{
		nOldValue = pBaseMgr->GetHeroHP(pDBHero);
		pBaseMgr->SetHeroHP(pDBHero,nValue);
	}
	else if(nType==2)
	{
		nOldValue = pDBHero->levelexp();
		bool bUpGrade =false;
		pDBHero->set_levelexp(0);
		pBaseMgr->AddHeroLevelExp(pDBHero,nValue,bUpGrade);
		pBaseMgr->UpdateHeroHP(pDBHero);
		pBaseMgr->UpdateHeroAtkProp(pDBHero->heroid());
		//更新战斗力属性
		pBaseMgr->UpdateAllCombatPowerValue_Hero();
		pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
	}
	else if(nType==3)
	{
		nOldValue = pDBHero->gradeexp();
		bool bUpdate = false;
		pDBHero->set_gradeexp(0);
		pBaseMgr->AddHeroGradeExp(pDBHero,nValue,bUpdate);
		pBaseMgr->UpdateHeroHP(pDBHero);
		pBaseMgr->UpdateHeroAtkProp(pDBHero->heroid());
		pBaseMgr->UpdateHeroCombatPowerValue(pDBHero->heroid());
		pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
	}

	else if(nType==4)
	{
		nOldValue = pDBHero->gradeexp();
		bool bUpdate = false;
		pBaseMgr->AddHeroGradeExp(pDBHero,nValue,bUpdate);
		pBaseMgr->UpdateHeroHP(pDBHero);
		pBaseMgr->UpdateHeroAtkProp(pDBHero->heroid());
		pBaseMgr->UpdateHeroCombatPowerValue(pDBHero->heroid());
		pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
		pBaseMgr->UpdateHeroRank(pDBHero);
		pBaseMgr->UpdateUserHeroCombatRank();
	}

	else if(nType==5)
	{
		nOldValue = pDBHero->gradeexp();
		bool bUpdate = false;
		pBaseMgr->MinusHeroGradeExp(pDBHero,nValue,bUpdate);
		pBaseMgr->UpdateHeroHP(pDBHero);
		pBaseMgr->UpdateHeroAtkProp(pDBHero->heroid());
		pBaseMgr->UpdateHeroCombatPowerValue(pDBHero->heroid());
		pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
		pBaseMgr->UpdateHeroRank(pDBHero);
		pBaseMgr->UpdateUserHeroCombatRank();


		if ( nOldValue < nValue )
		{
			nOverValue = nOldValue - nValue;
		}
	}

	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_SetHero, 0, 0,nType<<nHeroID<<nOldValue<<nValue<<nOverValue);
	return true;
}

bool DealAdminEvent::Admin_RecoverHeroHP(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
	{
		return false;
	}
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	pBaseMgr->RecoverAllHurtHero();
	return true;
}


bool DealAdminEvent::Admin_ResetPokerExc(Event *e, User *pUser)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();

    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();

    if ( req == NULL )
    {
        return false;
    }

    pPlayer->SetPokerReward(true);

    return true;
}

bool DealAdminEvent::Admin_ResetDuelInfo(Event *e, User *pUser)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();

    pPlayer->ClearDuelInfo();

    return true;
}



bool DealAdminEvent::Admin_ResetWholeCountry(Event* e)
{
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Status_Game_To_Region);
	eh_->sendEventToRegion(e,0);

	return true;
}

bool DealAdminEvent::Admin_WholeCountrySign(Event* e,User *pUser)
{
	if ( pUser == NULL )
	{
		return false;
	}
	RegionCountryManager* pRegionCountryMgr = pUser->GetPlayer()->GetRegionCountryManager();
	if ( pRegionCountryMgr == NULL )
	{
		return false;
	}

	pRegionCountryMgr->SignRegionCountry();

	SYS_LOG(pUser->GetUid(), LT_ADMIN_WholeCountrySign, 0, 0, 0);
	return true;
}

void DealAdminEvent::AddEvent_AddAdmiralLetter(EventQueue* eq, int gid, int fd, int64 uid,int nLetterID, int nCnt,bool bAdd)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
	if(bAdd)
	{
		req->set_opttype(AOT_ADMIN_ADDADMIRALLETTER);
	}
	else
	{
		req->set_opttype(AOT_ADMIN_DELADMIRALLETTER);
	}
	req->set_gid(gid);
	req->set_admiralid(nLetterID);//////////////////////
	req->set_count(nCnt);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

bool DealAdminEvent::Admin_AddAdmiralLetter(User* pUser,int nLetterID,int nCount)
{
	if(pUser==NULL||nLetterID<=0||nCount<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_AdmiralInfo* pDBAdmiralInfo = pBaseMgr->GetDBAdmiralInfo();
	if(pDBAdmiralInfo==NULL)
		return false;
	const AdmiralLetterTbl* pAdmiralLetterTbl = AdmiralInfoCfg::Instance().GetAdmiralLetterTbl(nLetterID);
	if(pAdmiralLetterTbl==NULL)
		return false;
	pBaseMgr->AddAdmiralLetter(nLetterID,nCount);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_AddAdmiralLetter, 0, 0,nLetterID<<nCount);
	return true;
}

bool DealAdminEvent::Admin_DelAdmiralLetter(User* pUser,int nLetterID,int nCount)
{
	if(pUser==NULL||nLetterID<=0||nCount<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	return false;
	DB_AdmiralInfo* pDBAdmiralInfo = pBaseMgr->GetDBAdmiralInfo();
	if(pDBAdmiralInfo==NULL)
		return false;
	pBaseMgr->CostAdmiralLetter(nLetterID,nCount);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_DelAdmiralLetter, 0, 0,nLetterID<<nCount);
	return true;
}


void DealAdminEvent::AddEvent_SetProperty(EventQueue* eq, int gid, int fd, int64 uid,int nPropertyId, int nLevel)
{
	Event* e = eq->allocateEvent();

	e->set_cmd(EVENT_ADMIN_OPT);
	e->set_state(Admin_AG_Req);
	e->set_time(-1);
	e->set_uid(uid);
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	req->set_uid(uid);
    req->set_opttype(AOT_ADMIN_SETPROPERTY);
	req->set_gid(gid);
	req->set_admiralid(nPropertyId);//////////////////////
	req->set_count(nLevel);//////////////////////
	req->set_adminfd(fd);
	eq->safePushEvent(e);
}

bool DealAdminEvent::Admin_SetProperty(User* pUser,int nPropertyId,int nLevel)
{
	if(pUser==NULL||nPropertyId<=0||nLevel<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const PropertyTbl* pTbl = PropertyInfoCfg::Instance().GetPropertyTbl(nPropertyId);
	if(pTbl==NULL)
	{
		return false;
	}
	DB_Property* pDBProperty = pPlayer->GetDBProperty(nPropertyId);
	if(pDBProperty==NULL)
	{
		pDBProperty = pPlayer->AddDBProperty(nPropertyId);
	}
	int nOldLevel = pDBProperty->nlevel();
	int nMaxLevel = pTbl->m_nMaxLevel;
	if(nLevel>nMaxLevel)
	{
		return false;
	}
	pDBProperty->set_nlevel(nLevel);
	//战斗力计算
	pBaseMgr->UpdateAllCombatPowerValue_Property();
	pBaseMgr->UpdateArmyHP();
	pBaseMgr->UpdateAllHeroHP();
	pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue(),true);
	pUserManager->markUserDirty(pUser);

	RsePropertyOpt rseRet;
	rseRet.set_opttype(1);
	rseRet.set_propertyid(nPropertyId);
	rseRet.set_ret(0);
	PropertyUpgrade * pProperty = rseRet.mutable_property();
	pPlayer->FillPropertyUpGrade(pProperty);

	string text;
	rseRet.SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RsePropertyOpt, text);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_SetProperty, 0, 0,nPropertyId<<nOldLevel<<pDBProperty->nlevel());
	return true;
}

bool DealAdminEvent::Admin_AddGveThew(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	GveManager* pGveMgr = pUser->GetPlayer()->GetGveManager();
	if ( pGveMgr == NULL )
	{
		return false;
	}
	pGveMgr->AddThew(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_AddGvePoint(User* pUser,int nCnt)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	GveManager* pGveMgr = pUser->GetPlayer()->GetGveManager();
	if ( pGveMgr == NULL )
	{
		return false;
	}
	pGveMgr->AddZhanGong(nCnt,E_ADD_COIN_Admin);
	return true;
}

bool DealAdminEvent::Admin_GveList(Event* e)
{
	Admin_AdminOpt_Req* req = e->mutable_adminopt_req();
	Admin_AdminOpt_Rsp* rsp = e->mutable_adminopt_rsp();
	if(req->count()<0)
		return false;

	rsp->set_adminfd(req->adminfd());
	rsp->set_uid(req->uid());

	req->set_guildid( req->uid() );
	e->set_state(Status_Game_To_Country);
	eh_->sendEventToCountry(e,req->count());

	return true;
}

bool DealAdminEvent::Admin_ClearAllBagItem(User *pUser)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();

    pPlayer->ClearAllBagItem();

    SYS_LOG(pUser->GetUid(), LT_ADMIN_ClearAllBagItem, 0, 0, 0);

    return true;
}

bool DealAdminEvent::Admin_AddRentParts(Event *e, User *pUser)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;;
    Player* pPlayer = pUser->GetPlayer();
    Admin_AdminOpt_Req* req = e->mutable_adminopt_req();

    if (req == NULL || pPlayer == NULL)
    {
        return false;
    }

    int nWPCID = req->wpccomid();
    int nQpointID = req->itemtype();
    int nLevel = req->level();
    int nType = req->emblemtype();
    int nFlag = req->flag();

    const QPointShopItemTbl *pTbl = QPointShopItemCfg::Instance().GetQPointShopTbl(nQpointID);
    if (pTbl == NULL)
    {
        return false;
    }

    if (pPlayer->CheckWeaponComponentCanRent(nType, nWPCID, pTbl->m_nMaxGetNum) != OR_Success)
    {
        return false;
    }
    if (nFlag > 0)
    {
        return pPlayer->DelRentWpc(nType, nWPCID);
    }

    int nRentHour = pPlayer->GetRentHourByShopID(nQpointID);
    pPlayer->AddRentWeaponComponent(nType, nWPCID, nLevel, nRentHour);

    return true;
}

bool DealAdminEvent::Admin_AddGuildTask(User* pUser , int taskid)
{
	if (pUser == NULL || pUser->GetPlayer() == NULL || !pUser->GetPlayer()->CanUse() || taskid <= 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	CountryManager* pCntrMgr = pPlayer->GetCountryManager();
	if (pCntrMgr == NULL || pCntrMgr->GetDBCountry() == NULL)
	{
		return false;
	}
	DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
	DB_GuildTask* pDBTask = pDBGuild->mutable_gtask();
	pDBTask->set_curid(taskid);
	pDBTask->set_progress(0);
	pDBTask->set_status(E_GTS_PROGRESS);
	return true;
}

bool DealAdminEvent::Admin_SetGuildTaskProgress(User* pUser , int count)
{
	if (pUser == NULL || pUser->GetPlayer() == NULL || !pUser->GetPlayer()->CanUse() || count <= 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	CountryManager* pCntrMgr = pPlayer->GetCountryManager();
	if (pCntrMgr == NULL || pCntrMgr->GetDBCountry() == NULL)
	{
		return false;
	}
	DB_Guild* pDBGuild = pCntrMgr->GetDBGuild();
	DB_GuildTask* pDBTask = pDBGuild->mutable_gtask();
	if (pDBTask->curid() <= 0)
	{
		return false;
	}
	pDBTask->set_progress(count);
	return true;
}

bool DealAdminEvent::Admin_GvgReward(Event* e,User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	CountryManager* pCntrMgr = pPlayer->GetCountryManager();
	if ( pCntrMgr == NULL )
	{
		return false;
	}

	if ( nValue <= 0 )
	{
		return false;
	}

	const GvgCityTbl* pTbl = GvgCfg::Instance().GetCityTbl(nValue);
	if ( pTbl == NULL )
	{
		return false;
	}

	int nPsition = pCntrMgr->GetGuildPosition();
	int nCnt = 0;

	if ( nPsition <= GP_Cnt )
	{
		nCnt = pTbl->m_nRewardCnt[nPsition];
	}

	RceBagItemUseHandle::getInstance()->AddItemOrUseItem(pUser,pTbl->m_nRewardID,nCnt,UserItem_By_Gvg,true,true);

	 SYS_LOG(pUser->GetUid(), LT_GVG_GET_REWARD, 0, 0, pUser->GetUid()<<nValue);
}

bool DealAdminEvent::Admin_SetGuildLeaderDelTime(User* pUser)
{
	//only for test , set country guild delatetm to 3 days ago
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	Event evt;
	evt.set_cmd(C2S_RceGuildOpt);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceGuildOpt* pRce = evt.mutable_ce_rceguildopt();
	pRce->set_type(GOT_SET_DELATETM);
	GCG_GuildOpt* pOpt = evt.mutable_guildopt();
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pOpt->set_guildid(pCntrMgr->GetDBGuild()->guildid());
	eh_->sendEventToCountry(&evt , pUser->GetUserRegion());
	return true;
}

bool DealAdminEvent::Admin_SetGuildLeader(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	Event evt;
	evt.set_cmd(C2S_RceGuildOpt);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceGuildOpt* pRce = evt.mutable_ce_rceguildopt();
	pRce->set_type(GOT_SET_LEADER);
	GCG_GuildOpt* pOpt = evt.mutable_guildopt();
	CountryManager* pCntrMgr = pUser->GetPlayer()->GetCountryManager();
	pOpt->set_guildid(pCntrMgr->GetDBGuild()->guildid());
	eh_->sendEventToCountry(&evt , pUser->GetUserRegion());
	return true;
}

bool DealAdminEvent::Admin_AddSearchTime(User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	int nCnt = nValue;
	
	DB_WpcRank* pDBWPCRank = pUser->GetPlayer()->GetDBWpcRank();
	if ( pDBWPCRank == NULL )
	{
		return false;
	}

	pDBWPCRank->set_searchtime( pDBWPCRank->searchtime() + nCnt );

	if ( pDBWPCRank->searchtime() < 0 )
	{
		pDBWPCRank->set_searchtime( 0 );
	}
	SYS_LOG(pUser->GetUid(),LT_WPCRANK_RECORD,0,0,pDBWPCRank->searchtime()<<nCnt<<"adminAddSearchTime");

	eh_->UpdateWpcInfo2CountrySrv(pUser,nValue);
	return true;
}

bool DealAdminEvent::Admin_DelAdmiralSpecial(User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;

	BaseManager* pBaseMgr = pUser->GetBaseManager();
	if ( pBaseMgr == NULL )
	{
		return false;
	}

	int nId = nValue;

	const AdmiralTbl* pTbl = AdmiralInfoCfg::Instance().GetAdmiralTbl(nId);
	if ( pTbl == NULL )
	{
		return false;
	}
	//蓝色，删将，将魂，记录
	if ( pTbl->m_nQuality < 4 )
	{
		DB_Admiral* pDBDelAdm = pBaseMgr->GetDBAdmiral(nId);
		if(pDBDelAdm!=NULL)
		{
			pBaseMgr->DelAdmiral(nId);
			SYS_LOG(pUser->GetUid(),LT_ADIMN_DEL_ADMIRAL_SPECIAL,0,0,nId<<"deladmiral");
		}
		else
		{
			int nOldCnt = pBaseMgr->GetAdmiralSoul();
			int nOverCnt = 0;

			if ( nOldCnt - 25 < 0 )
			{
				nOverCnt = nOldCnt - 25;
			}
			pBaseMgr->AddAdmiralSoul(-25);
			SYS_LOG(pUser->GetUid(),LT_ADIMN_DEL_ADMIRAL_SPECIAL,0,0,nId<<-25<<pBaseMgr->GetAdmiralSoul()<<nOverCnt<<"soul");
		}
	}

	//紫色，扣培养经验，扣将，将魂，记录
	if ( pTbl->m_nQuality == 4 )
	{
		DB_Admiral* pDBDelAdm = pBaseMgr->GetDBAdmiral(nId);
		if(pDBDelAdm!=NULL)
		{
			DB_AdmiralTrainLV* pDBTrainSkill = pDBDelAdm->mutable_trainlv();
			if ( pDBTrainSkill->cnt() > 0 )
			{
				pDBTrainSkill->set_cnt( pDBTrainSkill->cnt() - 1 );

				int nLV = AdmiralInfoCfg::Instance().CacuTrainSkillLV(pDBTrainSkill->cnt());
				if(pDBTrainSkill->level()!=nLV)
				{
					pDBTrainSkill->set_level(nLV);
				}
				pBaseMgr->CacuAdmiralFinalProp(pDBDelAdm,pTbl);
				if(pDBDelAdm->usetype())
				{
					pBaseMgr->UpdateAdmiralEffect();
					pBaseMgr->UpdateArmyHP();
					pBaseMgr->UpdateBldHP();
					pBaseMgr->UpdateAllHeroHP();
				}
				SYS_LOG(pUser->GetUid(),LT_ADIMN_DEL_ADMIRAL_SPECIAL,0,0,nId<<pDBTrainSkill->cnt()<<"deladmiraltrain");
			}
			else
			{
				pBaseMgr->DelAdmiral(nId);
				SYS_LOG(pUser->GetUid(),LT_ADIMN_DEL_ADMIRAL_SPECIAL,0,0,nId<<"deladmiral");
			}
		}
		else
		{
			int nOldCnt = pBaseMgr->GetAdmiralSoul();
			int nOverCnt = 0;

			if ( nOldCnt - 150 < 0 )
			{
				nOverCnt = nOldCnt - 150;
			}
			pBaseMgr->AddAdmiralSoul(-150);
			SYS_LOG(pUser->GetUid(),LT_ADIMN_DEL_ADMIRAL_SPECIAL,0,0,nId<<-150<<pBaseMgr->GetAdmiralSoul()<<nOverCnt<<"soul");
		}
	}

	return true;
}

bool DealAdminEvent::Admin_DelHeroSpecial(User* pUser,int nValue,int nCnt)
{
	if(pUser==NULL||nValue<=0||nCnt<=0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	
	const HeroLetterTbl* pTbl = HeroInfoCfg::Instance().GetHeroLetterTbl(nValue);
	if ( pTbl == NULL )
	{
		return false;
	}

	int nExp = 0;
	if ( pTbl->m_nQuality == 3 )
	{
		nExp = 30;
	}
	if ( pTbl->m_nQuality == 4 )
	{
		nExp = 240;
	}

	DB_HeroLetter* pDBHeroLetter = pBaseMgr->GetDBHeroLetter(nValue);
	if(pDBHeroLetter==NULL)
	{
		SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_SPECIAL, 0, 0,nValue<<"cannot"<<-nExp*nCnt);
		return true;
	}
	int nOldCnt = pDBHeroLetter->count();
	if(pDBHeroLetter->count()<nCnt)
	{
		int nOverCnt = nCnt - pDBHeroLetter->count();
		pDBHeroLetter->set_count(0);
		SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_SPECIAL, 0, 0,nValue<<nOldCnt<<0);
		SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_SPECIAL, 0, 0,nValue<<"cannot"<<-nExp*nOverCnt);
		return true;
	}

	pDBHeroLetter->set_count(pDBHeroLetter->count()-nCnt);
	pUserManager->markUserDirty(pUser);
	SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_SPECIAL, 0, 0,nValue<<nOldCnt<<pDBHeroLetter->count());
	return true;
}

bool DealAdminEvent::Admin_DelHeroExpSpecial(User* pUser,int nValue)
{
	if(pUser==NULL||nValue<0)
		return false;
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
		return false;
	Player* pPlayer = pUser->GetPlayer();
	if(pPlayer==NULL||!pPlayer->CanUse())
		return false;
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_HeroInfo* pDBHeroInfo = pBaseMgr->GetDBHeroInfo();
	if(pDBHeroInfo==NULL)
		return false;
	
	for ( int i = 0; i < 10; i++ )
	{
		DB_Hero * pDBHero = pBaseMgr->GetMaxExpDBHero();
		if(pDBHero==NULL)
			continue;

		int nOldValue = pDBHero->gradeexp();
		bool bUpdate = false;
		pBaseMgr->MinusHeroGradeExp(pDBHero,nValue/10,bUpdate);
		pBaseMgr->UpdateHeroHP(pDBHero);
		pBaseMgr->UpdateHeroAtkProp(pDBHero->heroid());
		pBaseMgr->UpdateHeroCombatPowerValue(pDBHero->heroid());
		pPlayer->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
		pBaseMgr->UpdateHeroRank(pDBHero);
		pBaseMgr->UpdateUserHeroCombatRank();
		SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_EXP_SPECIAL, 0, 0,pDBHero->heroid()<<nValue);
	}

	SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_HERO_EXP_SPECIAL, 0, 0,"all"<<nValue);
}

bool DealAdminEvent::Admin_DelWpcExpSpecial(User* pUser,int nValue)
{
	if(pUser==NULL||nValue<0)
		return false;

	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	DB_Building* pDBBld = pBaseMgr->GetDBBuildingByType(BLD_Weapon_Center);
	DB_WeaponCenter* pDBWPC = pBaseMgr->GetDBBases()->mutable_weaponcenter();
	if(pDBBld==NULL||pDBWPC==NULL)
	{
		return false;
	}

	nValue /= 10;
	for ( int i = 0; i < 10; i++ )
	{
		DB_WeaponCom* pDBCom = pBaseMgr->GetMaxExpDbWpcCom();
		if ( pDBCom == NULL )
		{
			continue;
		}

		const WPCComTbl* pWpcComTbl = WeaponCenterCfg::Instance().GetWPCComTbl(pDBCom->comid());
		int nDBID = pDBCom->id();
		int nOldExp = pDBCom->exp();
		int nGroup = pDBCom->bagtype();
		if (nOldExp > nValue )
		{
			RceWPCComOptHandle::getInstance()->WpcAddExp(pDBBld,pDBCom,pWpcComTbl,0-nValue);
		}
		else
		{
			continue;
		}

		if ( nGroup > WB_Army && nGroup < WB_Hero )
		{
			pBaseMgr->UpdateAllCombatPowerValue();
			pBaseMgr->UpdateWpcGroup(nGroup,true, pDBCom->extratype());
			pBaseMgr->UpdateWpcGroupAdd();
			pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
			pBaseMgr->UpdateArmyHP();
			pBaseMgr->UpdateAllHeroHP();
		}
		SYS_LOG(pPlayer->GetUserID(),LT_ADMIN_DELWPCCOMEXP,0,0,pDBCom->comid()<<nDBID<<nOldExp<<nValue<<nGroup);
	}

	SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_WPC_EXP_SPECIAL, 0, 0,nValue*10);
}

bool DealAdminEvent::Admin_DelEmblemExpSpecial(User* pUser,int nValue)
{
	if(pUser==NULL||nValue<0)
		return false;

	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	DB_Building* pDBBld = pBaseMgr->GetDBBuildingByType(BLD_Weapon_Center);
	DB_WeaponCenter* pDBWPC = pBaseMgr->GetDBBases()->mutable_weaponcenter();
	if(pDBBld==NULL||pDBWPC==NULL)
	{
		return false;
	}

	nValue /= 10;
	for ( int i = 0; i < 10; i++ )
	{
		DB_EmblemNew* pDBEmb = pBaseMgr->GetMaxExpDbEmblem();
		if ( pDBEmb == NULL )
		{
			continue;
		}

		int nDBID = pDBEmb->id();
		int nOldExp = pDBEmb->exp();
		int nGroup = pDBEmb->bagtype();
		if (nOldExp > nValue )
		{
			RceWPCEmblemOptHandle::getInstance()->WpcAddExp(pUser, pDBEmb, 0-nValue, pDBEmb->emblemid());
		}
		else
		{
			continue;
		}

		if (nGroup > WE_Bag && nGroup < WE_TypeMax)
		{
			pBaseMgr->UpdateEmblemEffect();
			pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());
			pBaseMgr->UpdateBldHP();
		}
		SYS_LOG(pPlayer->GetUserID(),LT_ADMIN_DELEMBLEMEXP,0,0,pDBEmb->emblemid()<<nDBID<<nOldExp<<nValue<<nGroup);

	}

	SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_EMBLEM_EXP_SPECIAL, 0, 0,nValue*10);
}

bool DealAdminEvent::Admin_DelBookExpSpecial(User* pUser,int nValue)
{
	if(pUser==NULL||nValue<0)
		return false;

	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;

	DB_Building* pDBBld = pBaseMgr->GetDBBuildingByType(BLD_Weapon_Center);
	DB_WeaponCenter* pDBWPC = pBaseMgr->GetDBBases()->mutable_weaponcenter();
	if(pDBBld==NULL||pDBWPC==NULL)
	{
		return false;
	}

	nValue /= 10;
	for ( int i = 0; i < 10; i++ )
	{
		DB_Book* pDBBook = pBaseMgr->GetMaxExpDbBook();
		if ( pDBBook == NULL )
		{
			continue;
		}
		int nDBID = pDBBook->id();
		int nOldExp = pDBBook->exp();
		int nGroup = pDBBook->bagtype();
		if (nOldExp > nValue)
		{
			const WPCBookTbl*pBookTbl = WeaponCenterCfg::Instance().GetWPCBookTbl(pDBBook->bookid());
			RceWPCBookOptHandle::getInstance()->WpcAddExp(pDBBld,pDBBook, pBookTbl, 0-nValue);
		}
		else
		{
			continue;
		}

		if (nGroup == WBook_Admiral)
		{
			pBaseMgr->UpdateBookEffect();
			pBaseMgr->UpdateAdmiralEffect();
			pBaseMgr->UpdateAllCombatPowerValue_Admiral();
			pUser->GetPlayer()->UpdateCombatPower(pBaseMgr->CheckAllCombatPowerValue());

			//更新出战将领分数
			pBaseMgr->UpdateAdmiralInuseScore();
			//更新将领排行版
			pBaseMgr->UpdateAdmiralRank();
			//更新使用将领排行版
			pBaseMgr->UpdateAdmiralInuseRank();
		}
		SYS_LOG(pPlayer->GetUserID(),LT_ADMIN_DELBOOKEXP,0,0,pDBBook->bookid()<<nDBID<<nOldExp<<nValue<<nGroup);

	}

	SYS_LOG(pUser->GetUid(), LT_ADIMN_DEL_BOOK_EXP_SPECIAL, 0, 0,nValue*10);
}

bool DealAdminEvent::Admin_DBRoolBack(User* pUser)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	bool bSuc = pUser->DBRollback();
	if(bSuc)
	{
		SYS_LOG(pUser->GetUid(), LT_ADMIN_DBRollBack, 0, 0,"succ"<<1);
	}
	else
	{
		SYS_LOG(pUser->GetUid(), LT_ADMIN_DBRollBack, 0, 0,"fail"<<0);

	}
}


bool DealAdminEvent::Admin_AddExpWpc(User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const WPCComTbl* pTbl = WeaponCenterCfg::Instance().GetWPCComTbl(10083);
	if(pTbl ==NULL)
		return false;

	pBaseMgr->AddExpWPCComSpecial(pTbl,nValue);

	SYS_LOG(pUser->GetUid(),LT_ADIMN_ADD_EXPWPC_SPECIAL,0,0,10083<<nValue);
	return true;
}

bool DealAdminEvent::Admin_AddExpEmblem(User* pUser,int nValue)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;;
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	const WPCEmblemTbl* pTbl = WeaponCenterCfg::Instance().GetWPCEmblemTbl(80111);
	if(pTbl ==NULL)
		return false;

	pBaseMgr->AddExpEmblemSpecial(pTbl,nValue);

	SYS_LOG(pUser->GetUid(),LT_ADIMN_ADD_EXPEMBLEM_SPECIAL,0,0,80111<<nValue);
	return true;
}

bool DealAdminEvent::Admin_SetWCPromoteExp(User* pUser , int nExp)
{
	if (NULL == pUser || NULL == pUser->GetPlayer() || nExp <= 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if (NULL == pBaseMgr)
	{
		return false;
	}
	DB_Bases* pBaseDB = pBaseMgr->GetDBBases();
	if (NULL == pBaseDB)
	{
		return false;
	}
	DB_WarCollege* pWCDB = pBaseDB->mutable_warcollege();
	pWCDB->set_strategyexp(nExp);
	eh_->getDataHandler()->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_SetWCSkillExp(User* pUser , int nSkillType , int nSkillExp)
{
	if (NULL == pUser || NULL == pUser->GetPlayer() || nSkillType <= 0 || nSkillType >= WC_SKILL_MAX || nSkillExp <= 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if (NULL == pBaseMgr)
	{
		return false;
	}
	DB_Bases* pBaseDB = pBaseMgr->GetDBBases();
	if (NULL == pBaseDB)
	{
		return false;
	}
	DB_WarCollege* pWCDB = pBaseDB->mutable_warcollege();
	if (pWCDB->skill_size() < nSkillType)
	{
		return false;
	}
	DB_WCSkillData* pSkillDB = pWCDB->mutable_skill(nSkillType - 1);
	pSkillDB->set_skillexp(nSkillExp);
	eh_->getDataHandler()->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_AddWCSkillExp(User* pUser , int nSkillType , int nSkillExp)
{
	if (NULL == pUser || NULL == pUser->GetPlayer() || nSkillType <= 0 || nSkillType >= WC_SKILL_MAX || nSkillExp <= 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if (NULL == pBaseMgr)
	{
		return false;
	}
	DB_Bases* pBaseDB = pBaseMgr->GetDBBases();
	if (NULL == pBaseDB)
	{
		return false;
	}
	DB_WarCollege* pWCDB = pBaseDB->mutable_warcollege();
	if (pWCDB->skill_size() < nSkillType)
	{
		return false;
	}
	DB_WCSkillData* pSkillDB = pWCDB->mutable_skill(nSkillType - 1);
	pSkillDB->set_skillexp(pSkillDB->skillexp() + nSkillExp);

	WCBaseTbl* pBaseTbl = WarCollegeInfoCfg::Instance().GetWCBaseTbl();
	if (NULL == pBaseTbl)
	{
		return false;
	}

	while(pSkillDB->skillexp() > 0)
	{
		WCSkillTbl* pTbl = WarCollegeInfoCfg::Instance().GetWCSkillTbl(nSkillType , pSkillDB->skilllv());
		if (NULL == pTbl)
		{
			break;
		}

		if (pSkillDB->skillexp() >= pTbl->m_nUpgradeExp && pSkillDB->skilllv() < pBaseTbl->m_nSkillLevelTop)
		{
			pSkillDB->set_skillexp(pSkillDB->skillexp() - pTbl->m_nUpgradeExp);
			pSkillDB->set_skilllv(pSkillDB->skilllv() + 1);
		}
		else
		{
			break;
		}
	}

	eh_->getDataHandler()->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_SubWCSkillExp(User* pUser , int nSkillType , int nSkillExp)
{
	if (NULL == pUser || NULL == pUser->GetPlayer() || nSkillType <= 0 || nSkillType >= WC_SKILL_MAX || nSkillExp < 0)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	BaseManager* pBaseMgr = pPlayer->GetBaseManager();
	if (NULL == pBaseMgr)
	{
		return false;
	}
	DB_Bases* pBaseDB = pBaseMgr->GetDBBases();
	if (NULL == pBaseDB)
	{
		return false;
	}
	DB_WarCollege* pWCDB = pBaseDB->mutable_warcollege();
	if (pWCDB->skill_size() < nSkillType)
	{
		return false;
	}
	DB_WCSkillData* pSkillDB = pWCDB->mutable_skill(nSkillType - 1);
	//pSkillDB->set_skillexp(pSkillDB->skillexp() + nSkillExp);

	WCBaseTbl* pBaseTbl = WarCollegeInfoCfg::Instance().GetWCBaseTbl();
	if (NULL == pBaseTbl)
	{
		return false;
	}

	bool bMark = false;

	if (nSkillExp == 0)
	{
		pSkillDB->set_skilllv(1);
		pSkillDB->set_skillexp(0);
		bMark = true;
	}

	while(nSkillExp > 0)
	{
		if (nSkillExp > pSkillDB->skillexp())
		{
			int nOldlv = pSkillDB->skilllv();
			if (nOldlv > 1)
			{
				WCSkillTbl* pSkillTbl = WarCollegeInfoCfg::Instance().GetWCSkillTbl(nSkillType , nOldlv - 1);
				if (NULL == pSkillTbl)
				{
					return false;
				}
				nSkillExp -= pSkillDB->skillexp() + 1;
				pSkillDB->set_skilllv(nOldlv - 1);
				pSkillDB->set_skillexp(pSkillTbl->m_nUpgradeExp - 1);
			}
			else
			{
				pSkillDB->set_skilllv(1);
				pSkillDB->set_skillexp(0);
				nSkillExp = 0;
			}
		}
		else
		{
			int nNewExp = pSkillDB->skillexp() - nSkillExp;
			nSkillExp -= pSkillDB->skillexp();
			pSkillDB->set_skillexp(nNewExp);
		}
		bMark = true;
	}

	if (bMark)
	{
		eh_->getDataHandler()->markUserDirty(pUser);
		return true;
	}
	return false;
}

enum E_WARCOLLEGE_OPT_TYPE
{
	WCOT_OPEN = 1,
	WCOT_UPSTRATEGY,
	WCOT_PROMOTE,
	WCOT_TRAINSKILL,
	WCOT_UPSKILL,
	WCOT_RESETSKILLTRAINTIMES,
	WCOT_BUYSKILLTRAINTIMES,
	WCOT_MAX,
};
bool DealAdminEvent::Admin_WCOpType(User* pUser , int nEvtype , int nOptype)
{
	
	if (NULL == pUser || NULL == pUser->GetPlayer() || nEvtype <= 0 || nEvtype >= WCOT_MAX)
	{
		return false;
	}

	Event evt;
	evt.set_cmd(C2S_RceWarCollegeOpt);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceWarCollegeOpt* pRce = evt.mutable_ce_rcewarcollegeopt();
	pRce->set_type(nEvtype);
	switch(nEvtype)
	{
	case WCOT_UPSTRATEGY:
		pRce->set_strategytype(nOptype);
		break;
	case WCOT_TRAINSKILL:
		pRce->set_trainskilltype(nOptype);
		break;
	case WCOT_UPSKILL:
		pRce->set_upskilltype(nOptype);
		break;
	default:
		return false;
	}
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_WCOpUid(User* pUser , int nEvtype)
{
	
	if (NULL == pUser || NULL == pUser->GetPlayer() || nEvtype <= 0 || nEvtype >= WCOT_MAX)
	{
		return false;
	}

	Event evt;
	evt.set_cmd(C2S_RceWarCollegeOpt);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceWarCollegeOpt* pRce = evt.mutable_ce_rcewarcollegeopt();

	switch(nEvtype)
	{
	case WCOT_OPEN:
	case WCOT_PROMOTE:
	case WCOT_BUYSKILLTRAINTIMES:
		pRce->set_type(nEvtype);
		break;
	/*case WCOT_RESETSKILLTRAINTIMES:
		pRce->set_type(WCOT_RESETSKILLTRAINTIMES);
		break;*/
	default:
		return false;
	}
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_SADOp(User* pUser , int nEvtype , int nValue)
{
	if (NULL == pUser || NULL == pUser->GetPlayer() || nEvtype <= 0 || nEvtype >= WCOT_MAX)
	{
		return false;
	}

	Event evt;
	evt.set_cmd(C2S_RceSecArmsDealer);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	
	RceSecArmsDealer* pRce = evt.mutable_ce_rcesecarmsdealer();
	pRce->set_type(nEvtype);
	switch(nEvtype)
	{
	case 1:
		break;
	case 2:
		pRce->set_awardidx(nValue);
		break;
	default:
		return false;
	}
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_AddReport(User* pUser , int nNum)
{
    if (NULL == pUser || NULL == pUser->GetPlayer() || NULL == pUser->GetPlayer()->GetWarGameMgr())
    {
        return false;
    }


    pUser->GetPlayer()->GetWarGameMgr()->AddWarGameReport(nNum, 1);
    return true;
}

bool DealAdminEvent::Admin_AddWarGameToken(User* pUser , int nNum)
{
    if (NULL == pUser || NULL == pUser->GetPlayer() || NULL == pUser->GetPlayer()->GetWarGameMgr())
    {
        return false;
    }


    pUser->GetPlayer()->GetWarGameMgr()->AddWarGameToken(nNum, 2);
    return true;
}

bool DealAdminEvent::Admin_SetStormBox(User* pUser , int nFlag)
{
	if (NULL == pUser || NULL == pUser->GetPlayer())
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	CountryManager* pCountryMgr = pPlayer->GetCountryManager();
	if (NULL == pCountryMgr)
	{
		return false;
	}
	DB_Country* pDBCountry = pCountryMgr->GetDBCountry();
	if (NULL == pDBCountry)
	{
		return false;
	}
	pDBCountry->set_nrandstormbox(nFlag);
}

bool DealAdminEvent::Admin_SetQQTenyearFlag(User* pUser , int nFlag)
{
	if (NULL == pUser || NULL == pUser->GetPlayer())
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	DB_Player* pDBPlayer = pPlayer->GetDBPlayer();
	if (NULL == pDBPlayer)
	{
		return false;
	}
	pDBPlayer->set_qqtenyearbackflowflag(nFlag);
	return true;
}

bool DealAdminEvent::Admin_HappylibAward(User* pUser , int nFlag)
{
	if (NULL == pUser)
	{
		return false;
	}
	Event evt;
	evt.set_cmd(C2S_RceHappyLib);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceHappyLib* pRce = evt.mutable_ce_rcehappylib();
	pRce->set_type(1);
	pRce->set_id(nFlag);
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_MilitarySoulPoint(User* pUser , int nNum)
{
    if (NULL == pUser)
    {
        return false;
    }
    BaseManager *pBase = pUser->GetBaseManager();
    if (pBase == NULL)
    {
        return false;
    }
    return pBase->AddMilitarySoulPoint(nNum, E_MS_POINT_OPT_ADD_AddminAdd);
}

bool DealAdminEvent::Admin_CountryChatUI(User* pUser)
{
	if (NULL == pUser)
	{
		return false;
	}
	Event evt;
	evt.set_cmd(C2S_RceCountryChatMsg);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceCountryChatMsg* pRce = evt.mutable_ce_rcecountrychatmsg();
	pRce->set_type(2);
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_CountryChatMsg(User* pUser , const string& strMsg)
{
	if (NULL == pUser || strMsg.empty())
	{
		return false;
	}
	Event evt;
	evt.set_cmd(C2S_RceCountryChatMsg);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceCountryChatMsg* pRce = evt.mutable_ce_rcecountrychatmsg();
	pRce->set_type(3);
	pRce->set_chatmsg(strMsg);
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_CountryChatMask(User* pUser , int type , int64 tuid , const string& strName)
{
	if (NULL == pUser || strName.empty())
	{
		return false;
	}
	Event evt;
	evt.set_cmd(C2S_RceCountryChatMsg);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceCountryChatMsg* pRce = evt.mutable_ce_rcecountrychatmsg();
	switch(type)
	{
	case 0:
		{
			//屏蔽
			pRce->set_type(5);
			pRce->set_maskname(strName);
			stringstream tmp;
			tmp<<tuid;
			pRce->set_maskid(tmp.str());
		}
		break;
	case 1:
		//打开屏蔽界面
		pRce->set_type(6);
		break;
	case 2:
		{
			//解除屏蔽
			pRce->set_type(7);
			stringstream tmp;
			tmp<<tuid;
			pRce->set_maskid(tmp.str());
		}
		break;
	default:
		return false;
	}
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_SetSevenDaysLogonDays(User *pUser, int nDays)
{
    if (NULL == pUser)
    {
        return false;
    }
    Player *pPlayer = pUser->GetPlayer();
    if (pPlayer == NULL)
    {
        return false;
    }
    DB_Player *pDBPlayer = pPlayer->GetDBPlayer();
    if (pDBPlayer == NULL)
    {
        return false;
    }
    DB_SevenDays* pDBSevenDays = pDBPlayer->mutable_sevendays();
    if (pDBSevenDays == NULL)
    {
        return false;
    }

    if (nDays <= 1)
    {
        nDays = 1;
    }
    pDBSevenDays->set_logondays(nDays);
    return true;
}

bool DealAdminEvent::Admin_AddYaSuiQian(User *pUser, int nCnt)
{
    if (NULL == pUser)
    {
        return false;
    }
    Player *pPlayer = pUser->GetPlayer();
    if (pPlayer == NULL)
    {
        return false;
    }
    pPlayer->AddYaSuiQian(nCnt, E_GUILD_REDENVELOPE_YASUIQIAN_ADMINADD);
    return true;
}

bool DealAdminEvent::Admin_NpcactiveRank(Event * e , User* pUser)
{
	if (NULL == pUser || NULL == e)
	{
		return false;
	}
	eh_->sendEventToCountry(e, pUser->GetUserRegion(true));
	return true;
}

bool DealAdminEvent::Admin_TowerEnhance(User* pUser , int nBldid , int nEnhanceType)
{
	if(NULL == pUser)
	{
		return false;
	}
	Event evt;
	evt.set_cmd(C2S_RceBuildingModify);
	evt.set_state(Status_Normal_Game);
	evt.set_time(0);
	evt.set_uid(pUser->GetUid());
	RceBuildingModify* pRce = evt.mutable_ce_rcebuildingmodify();
	pRce->set_type(BO_TowerEnhance);
	pRce->set_id(nBldid);
	pRce->set_towerenhancetype(nEnhanceType);
	eh_->postBackEvent(&evt);
	return true;
}

bool DealAdminEvent::Admin_SetTowerEnhance(User* pUser , int nBldid , int nEnhanceType)
{
	if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
		return false;
	BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
	if(pBaseMgr==NULL)
		return false;
	DB_Building* pDBBld = pBaseMgr->GetDBBuilding(nBldid);
	if(pDBBld==NULL)
		return false;
	//强置,不用判断了
	time_t tEnd = 0;
	if(nEnhanceType > 0)
	{
		BuildingEnhanceTbl* pTbl = BuildingEnhanceCfg::Instance().GetTbl(pDBBld->type() , nEnhanceType);
		if (pTbl != NULL)
		{
			tEnd = time(NULL) + pTbl->m_nLastTime;
			pDBBld->set_enhanceend((int)tEnd);
			pDBBld->set_enhancetype(nEnhanceType);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pDBBld->clear_enhanceend();
		pBaseMgr->CheckTowerEnhanceTime(pDBBld);
	}
	eh_->getDataHandler()->markUserDirty(pUser);
	return true;
}

bool DealAdminEvent::Admin_AddJammer(User *pUser, int nJammerType, int nLv, int nCount)
{
    if(pUser==NULL||pUser->GetPlayer()==NULL||!pUser->GetPlayer()->CanUse())
        return false;
    BaseManager* pBaseMgr = pUser->GetPlayer()->GetBaseManager();
    if(pBaseMgr==NULL)
        return false;
    if (nCount < 0)
    {
        nCount = 1;
    }
    if ( nLv < 1)
    {
        nLv = 1;
    }
    const WPCJammerTbl*pTbl = WeaponCenterCfg::Instance().GetWPCJammerTbl(nJammerType);
    if (pTbl == NULL)
    {
        return false;
    }
    if (nLv > pTbl->m_nMaxLV)
    {
        nLv = pTbl->m_nMaxLV;
    }
    pBaseMgr->AddJammer(pTbl, nCount, nLv);
    return true;
}

bool DealAdminEvent::Admin_SetBDNTimes(User *pUser , int nTimes)
{
	if (NULL == pUser || nTimes < 0)
	{
		return false;
	}
	BaseManager* pBaseMgr = pUser->GetBaseManager(0);
	DB_BaseDefNPC* pDBDef = pBaseMgr->GetDBBaseDefNPC();
	if(pDBDef)
	{
		pDBDef->set_daydefcnt(nTimes);
		eh_->getDataHandler()->markUserDirty(pUser);
		RseBaseDefNPC rseRet;
		rseRet.set_ret(0);
		rseRet.set_msgtype(0);
		rseRet.set_atktimes(pDBDef->daydefcnt());
		rseRet.set_bestscore(pDBDef->bestscore());

		if(pDBDef->leftstakecnt()>0&&pDBDef->stakeitemlst_size()>0)
		{ //有剩余抽奖次数
			for(int i=0;i<pDBDef->showitemlst_size()&&i<pDBDef->showitemcnt_size();i++)
			{
				rseRet.add_stakeitemid(pDBDef->showitemlst(i));
				rseRet.add_stakeitemcnt(pDBDef->showitemcnt(i));
			}
			rseRet.set_leftstakecnt(pDBDef->leftstakecnt());
			rseRet.set_npcid(pDBDef->lastnpcid());
		}
		string strDef;
		rseRet.SerializeToString(&strDef);
		eh_->sendDataToUser(pUser->fd(), S2C_RseBaseDefNPC, strDef);
	}
	return true;
}

bool DealAdminEvent::Admin_SiteTestAdmin(User* pUser , int nType , int nValue)
{
	//ntype 1,福利充值数量 2,福利领奖记录 3,免费登陆天数 4,免费领奖记录
	enum SiteTestAdminType
	{
		STA_CCREDIT=1,
		STA_CAWARD,
		STA_LDAYS,
		STA_LAWARD,
	};

	if (nType < 0 || nValue < 0)
	{
		return false;
	}

	if (NULL == pUser)
	{
		return false;
	}
	Player* pPlayer = pUser->GetPlayer();
	if (NULL == pPlayer)
	{
		return false;
	}
	DB_Player* pDbPlayer = pPlayer->GetDBPlayer();
	if (NULL == pDbPlayer)
	{
		return false;
	}
	
	switch(nType)
	{
	case STA_CCREDIT:
		pDbPlayer->set_stcredit(nValue);
		eh_->getDataHandler()->markUserDirty(pUser);
		break;
	case STA_CAWARD:
		pDbPlayer->set_stcreditaward(nValue);
		eh_->getDataHandler()->markUserDirty(pUser);
		break;
	case STA_LDAYS:
		pDbPlayer->set_stlogindays(nValue);
		eh_->getDataHandler()->markUserDirty(pUser);
		break;
	case STA_LAWARD:
		pDbPlayer->set_stloginaward(nValue);
		eh_->getDataHandler()->markUserDirty(pUser);
		break;
	default:
		return false;
	}
	return true;
}
#endif