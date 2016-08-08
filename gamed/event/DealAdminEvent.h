#ifndef _DEALADMINEVENT_H_
#define _DEALADMINEVENT_H_
#pragma once
#include "BaseEvent.h"

class DealAdminEvent : public CBaseEvent
{
public:
	DealAdminEvent();
	~DealAdminEvent();
	static void createInstance(GameEventHandler* eh) 
	{ 
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDCASH,((ProcessRoutine)DealAdminEvent::handle_));
#if 0
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDEXP,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_OPT,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_CANATTACK,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETLEVEL,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANCHAT,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANLOGIN,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_RELOAD,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_AD_SET,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_PROFILE,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_BANDWGUSER,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDGUILDBTLREWARD,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_ADDHONOR,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_CLEARDEFENSE,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETNEWCOMERGIFT,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETCONTINUELANDDAY,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_SETPLATINVITEID,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_GETBLDCNT,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_RELOADCHICKENRANK,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_RESETWHOLECOUNTRY,((ProcessRoutine)DealAdminEvent::handle_));
		eh->getEventHandler()->registHandler(EVENT_ADMIN_GVELIST,((ProcessRoutine)DealAdminEvent::handle_));
        eh->getEventHandler()->registHandler(EVENT_ADMIN_RESETGUILDPKSCORE,((ProcessRoutine)DealAdminEvent::handle_));
        eh->getEventHandler()->registHandler(EVENT_ADMIN_OUTPUTGUILDUSERLIST,((ProcessRoutine)DealAdminEvent::handle_));
#endif
	}

	static DealAdminEvent* getInstance()
	{
		static DealAdminEvent instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		getInstance()->handle(e);
	}

	static void AddEvent_AddCash(EventQueue* eq, int gid, int fd, int64 uid, int gold);
#if 0
	static void AddEvent_Profile(EventQueue* eq, int cmd);
	static void AddEvent_AddExp(EventQueue* eq, int gid, int fd, int64 uid, int exp);
	static void AddEvent_AddHonor(EventQueue* eq, int gid, int fd, int64 uid, int gold);
	static void AddEvent_CanAttack(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_SetLevel(EventQueue* eq, int gid, int fd, int64 uid, int level);
	static void AddEvent_BanChat(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min);
	static void AddEvent_BanLogin(EventQueue* eq, int gid, int fd, int64 uid, int isban, int min);
	static void AddEvent_Reload(EventQueue* eq, int gid, int fd);
	static void AddEvent_AdSet(EventQueue* eq, int gid, int fd,Admin_AdSet_Type nType,int nEnable,int nMinPerchage,int nMaxPerchage,string strDayStart,string strDayEnd);
	static void AddEvent_BandWGUser(EventQueue* eq, int gid, int bWGUser,int fd);

	static void AddEvent_AddRes(EventQueue* eq, int gid, int fd, int64 uid, int nres1, int nRes2);
	static void AddEvent_AddArmy(EventQueue* eq, int gid, int fd, int64 uid, int nArmyType, int nCnt,bool bAdd);
	static void AddEvent_AddBld(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV,int nCnt);
	static void AddEvent_DelBld(EventQueue* eq, int gid, int fd, int64 uid, int nBldID);
	static void AddEvent_SetBldCnt(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV,int nCnt);
	static void AddEvent_SetAllBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nLV);
	static void AddEvent_SetBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldID,int nLV);
	static void AddEvent_SetArmyLV(EventQueue* eq, int gid, int fd, int64 uid, int nArmyType,int nLV);
	static void AddEvent_AddNpc(EventQueue* eq, int gid, int fd, int64 uid, int nNPCID,bool bAdd);
	static void AddEvent_SaveNpc(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_ClearDefense(EventQueue* eq, int gid, int fd, int64 uid,int ntype, int ncustom);
	static void AddEvent_SetNewComerGift(EventQueue* eq, int gid, int fd, int64 uid,int ntype, int nid, int nstate);
	static void AddEvent_SetContinueLandDay(EventQueue* eq, int gid, int fd, int64 uid,int nDay);
	static void AddEvent_SetPlatInviteID(EventQueue* eq, int gid, int fd, int64 uid,string pid);
	static void AddEvent_AddItem(EventQueue* eq, int gid, int fd, int64 uid, int nItemType, int nCnt);
	static void AddEvent_AddEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nCnt);
	static void AddEvent_AddWPCCom(EventQueue* eq, int gid, int fd, int64 uid, int nWPCComID, int nCnt,int nLV);
	static void AddEvent_AddBook(EventQueue* eq, int gid, int fd, int64 uid, int nBookID, int nCnt,int nLV);
    static void AddEvent_DelBookExp(EventQueue* eq, int gid, int fd, int64 uid, int nBookID, int nExp);
	static void AddEvent_AddFeats(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_AddCountryCoin(EventQueue* eq, int gid, int fd, int64 uid, Admin_Opt_Type emType, int nCnt);
	static void AddEvent_ReloadCountryRank(EventQueue* eq, int gid, int fd);
	static void AddEvent_ArenaFightSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1,int nValue2);
	static void AddEvent_WorldArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1);
	static void AddEvent_HeroArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1);
	static void AddEvent_RegionArenaSet(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue1);
	static void AddEvent_ReloadGameRank(EventQueue* eq, int gid, int fd, int nType);
	static void AddEvent_AddGuildScore(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_AddGuildBldValue(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_SetGuildScienceLV(EventQueue* eq, int gid, int fd, int64 uid, int nScienceID,int nLV);
	static void AddEvent_SetGuildBldLV(EventQueue* eq, int gid, int fd, int64 uid, int nBldID,int nLV);
	static void AddEvent_AddGuildBtlReward(EventQueue* eq, int gid, int fd, int nGuildID,int nRegion,int nRank);

	static void AddEvent_SetOfficerPosition(EventQueue* eq, int gid, int fd, int64 uid, int nPosition,int nFlag);
	static void AddEvent_AddBuff(EventQueue* eq, int gid, int fd, int64 uid, int nBuffId);
	static void AddEvent_SetVip(EventQueue* eq, int gid, int fd, int64 uid, int nType,int nValue);
	static void AddEvent_SetEquipStrLv(EventQueue* eq, int gid, int fd, int64 uid, int nEquipId,int nOldLevel,int nNewLevel);



	static void AddEvent_AddAdmiral(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nLV,bool bAdd);
	static void AddEvent_SetAdmiralSkill(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nLV);
	static void AddEvent_AddAdmiralSoul(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_AddAdmiralTrainExp(EventQueue* eq, int gid, int fd, int64 uid, int nAdmiralID,int nCnt);
	static void AddEvent_DetBldCnt(EventQueue* eq, int gid, int fd, int64 uid, int nBldType,int nCnt);
	static void AddEvent_RecoverHP(EventQueue* eq, int gid, int fd, int64 uid, int nType);
	static void AddEvent_SetInit(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_SetScienceLvl(EventQueue* eq, int gid, int fd, int64 uid, int nRes1, int nRes2);
	static void AddEvent_SetGMTestValue(EventQueue* eq, int gid, int fd, int64 uid, int nRes1, int nRes2);
	static void AddEvent_GetBldCnt(EventQueue* eq, int gid, int fd, int64 uid);

	static void AddEvent_MoonCnt(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_ClearStrategic(EventQueue* eq, int gid, int fd, int64 uid, int nType);
	static void AddEvent_PveSetCustom(EventQueue* eq, int gid, int fd, int64 uid, int nCustom,int nRefresh,int nBuyRefresh,int nFail);
	static void AddEvent_PveFightStart(EventQueue* eq, int gid, int fd, int64 uid, int nTime,int nKind);
	static void AddEvent_ReInitDB(EventQueue* eq, int gid, int fd, int64 uid);

	static void AddEvent_AddCoupon(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_AddEquip(EventQueue* eq, int gid, int fd, int64 uid, int nEquipID, int nCnt);
    static void AddEvent_DelEquip(EventQueue* eq, int gid, int fd, int64 uid, int nEquipID);

	static void AddEvent_AddAdmiralTimes(EventQueue* eq, int gid, int fd, int64 uid, int nCnt);
	static void AddEvent_FinishGuide(EventQueue* eq, int gid, int fd, int64 uid);

	static void AddEvent_ClearPassword(EventQueue* eq, int gid, int fd, int64 uid, int nTime);
	static void AddEvent_DelEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp);
    static void AddEvent_DelEmblemExp(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp);
	static void AddEvent_DelWpccom(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp);
    static void AddEvent_DelWpccomExp(EventQueue* eq, int gid, int fd, int64 uid, int nEmblemType, int nExp);

	static void AddEvent_SpecialValue(EventQueue* eq, int gid, int fd, int64 uid,int nValue);

    static void AddEvent_ClearNewDayInfo(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_PveBattleSet(EventQueue* eq, int gid, int fd, int64 uid,int nValue);
	static void AddEvent_AddJunLing(EventQueue* eq, int gid, int fd, int64 uid,int nValue);
    static void AddEvent_AddPokers(EventQueue* eq, int gid, int fd, int64 uid,int nPoker, int nChangeCard, int nPoints);
    static void AddEvent_ResetPokerExc(EventQueue* eq, int gid, int fd, int64 uid);
    static void AddEvent_ResetDuelInfo(EventQueue* eq, int gid, int fd, int64 uid);

	static void AddEvent_AddHero(EventQueue* eq, int gid, int fd, int64 uid, int nHeroID,bool bAdd);
	static void AddEvent_AddHeroLetter(EventQueue* eq, int gid, int fd, int64 uid,int nHeroLetterID, int nCnt,bool bAdd);
	static void AddEvent_SetHero(EventQueue* eq, int gid, int fd, int64 uid, int nHeroID,int nType,int nValue);
	static void AddEvent_ResetWholeCountry(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_WholeCountrySign(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_AddAdmiralLetter(EventQueue* eq, int gid, int fd, int64 uid,int nLetterID, int nCnt,bool bAdd);
	static void AddEvent_SetProperty(EventQueue* eq, int gid, int fd, int64 uid,int nPropertyId, int nLevel);
	
	static void AddEvent_AddGveCoin(EventQueue* eq, int gid, int fd, int64 uid, Admin_Opt_Type emType, int nCnt);
	static void AddEvent_GveList(EventQueue* eq, int gid, int fd, int64 uid, int nCnt, int nType,int nGveCity);
    static void ClearEvent_ClearAllBagItem(EventQueue* eq, int gid, int fd, int64 uid);
    static void AddEvent_AddRentWpc(EventQueue* eq, int gid, int fd, int64 uid, int nType, int nWPCID, int nLevel, int nQPointID, int nFlag);
	static void AddEvent_AddGuildTask(EventQueue* eq , int gid , int fd , int64 uid , int taskid);
	static void AddEvent_SetGuildTaskProgress(EventQueue* eq , int gid , int fd , int64 uid , int count);

	static void AddEvent_GvgReward(EventQueue* eq, int gid, int fd, int64 uid,int nValue);
	static void AddEvent_SetGuildLeaderDelTime(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_SetGuildLeader(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_AddSearchTime(EventQueue* eq, int gid, int fd, int64 uid,int nValue);
	static void AddEvent_DelAdmiralSpecial(EventQueue* eq, int gid, int fd, int64 uid,int nValue);

	static void AddEvent_DelHeroSpecial(EventQueue* eq, int gid, int fd, int64 uid,int nValue,int nCnt);
	static void AddEvent_DelHeroExpSpecial(EventQueue* eq, int gid, int fd, int64 uid,int nValue);
	static void AddEvent_DelWpccomExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp);
	static void AddEvent_DelEmblemExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp);
	static void AddEvent_DelBookExpSpecial(EventQueue* eq, int gid, int fd, int64 uid, int nExp);

	static void AddEvent_DBRollBack(EventQueue* eq, int gid, int fd, int64 uid);

	static void AddEvent_AddExpWpc(EventQueue* eq, int gid, int fd, int64 uid, int nExp);
	static void AddEvent_AddExpEmblem(EventQueue* eq, int gid, int fd, int64 uid, int nExp);

	static void AddEvent_SetWCPromoteExp(EventQueue* eq, int gid, int fd, int64 uid , int nExp);
	static void AddEvent_SetWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp);
	static void AddEvent_AddWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp);
	static void AddEvent_SubWCSkillExp(EventQueue* eq, int gid, int fd, int64 uid , int nSkillType , int nSkillExp);
	static void AddEvent_WCOpType(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype , int nOptype);
	static void AddEvent_WCOpUid(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype);

	static void AddEvent_SADOp(EventQueue* eq, int gid, int fd, int64 uid , int nEvtype , int nValue);

    static void AddEvent_AddWarGameReport(EventQueue* eq, int gid, int fd, int64 uid , int nNum);
    static void AddEvent_AddWarGameToken(EventQueue* eq, int gid, int fd, int64 uid , int nNum);
	static void AddEvent_SetStormBox(EventQueue* eq, int gid, int fd, int64 uid , int nFlag);
	static void AddEvent_SetQQTenyearFlag(EventQueue* eq, int gid, int fd, int64 uid , int nFlag);
	static void AddEvent_HappylibAward(EventQueue* eq, int gid, int fd, int64 uid , int nFlag);
    static void AddEvent_AddMilitarySoulPoint(EventQueue* eq, int gid, int fd, int64 uid , int nNum);
	static void AddEvent_CountryChatUI(EventQueue* eq, int gid, int fd, int64 uid);
	static void AddEvent_CountryChatMsg(EventQueue* eq, int gid, int fd, int64 uid , string strMsg);
	static void AddEvent_AddCountryChatMask(EventQueue* eq , int gid , int fd , int64 uid , int type , int64 tuid , string strMsg);
    static void ResetEvent_ResetGuildPKScore(EventQueue* eq , int gid , int fd, int nRegion , int nScore);
    static void SetEvent_SevenDaysLogonDays(EventQueue* eq , int gid , int fd, int64 uid , int nDays);
    static void AddEvent_AddYaSuiQian(EventQueue* eq , int gid , int fd, int64 uid , int nCnt);
	static void AddEvent_NpcactiveRank(EventQueue* eq , int gid , int fd , int64 uid);
	static void TowerEnhance(EventQueue* eq , int gid , int fd, int64 uid , int bldid , int etype);
	static void SetTowerEnhance(EventQueue* eq , int gid , int fd, int64 uid , int bldid , int etype);
    static void AddEvent_AddJammer(EventQueue* eq , int gid , int fd, int64 uid , int nJammerType , int nLv, int nCount);
    static void OutputGuildUserList(EventQueue* eq , int gid , int fd, int guildID, int nRegion);
	static void BaseDefNpcTimes(EventQueue* eq , int gid , int fd, int64 uid , int nStep);
	static void SiteTestAdmin(EventQueue* eq , int gid , int fd, int64 uid , int nType , int nValue);
#endif
private:
	void handle(Event* e);

protected:
	void processAdminAddCash(Event* e);
#if 0
	void processAdminProfile(Event* e);
	void processAdminAddExp(Event* e);
	void processAdminAddHonor(Event* e);
	void processAdminSetLevel(Event* e);
	void processAdminBanLogin(Event* e);
	void processAdminBanChat(Event* e);
	void processAdminReload(Event* e);
	void processAdSet(Event* e);
	void processAdminCanAttack(Event* e);
	void processAdminBandWGUser(Event* e);
	void processAdminClearDefense(Event* e);
	void processAdminSetNewComerGift(Event* e);
	void processAdminSetContinueLandDay(Event* e);
	void processAdminSetPlatInviteId(Event* e);
	void processAdminGetBldCnt(Event* e);
	void processAdminReloadChickenRank(Event* e);
	void processAdminAddGuildBtlReward(Event* e);
	void processAdminResetWholeCountry(Event* e);
	void processAdminGveList(Event* e);
	void processAdminOpt(Event* e);
    void processAdminResetGuildPKScore(Event *e);
    void processAdminOutputGuildUserList(Event *e);

private:
	bool Admin_DealMsg(Event* e,User* pUser,Admin_AdminOpt_Req* req,string& strCmd);
	bool Admin_AddRes(User* pUser,int64 nRes1, int64 nRes2);
	bool Admin_AddArmy(User* pUser, int nArmyType, int nCnt);
	bool Admin_DelArmy(User* pUser, int nArmyType, int nCnt);
	bool Admin_AddBld(User* pUser,int nBldType,int nLV,int nCnt);
	bool Admin_DelBld(User* pUser,int nBldID);
	bool Admin_SetBldCnt(User* pUser,int nBldType,int nLV,int nCnt);
	bool Admin_SetAllBldLV(User* pUser, int nBldType,int nLV);
	bool Admin_SetBldLV(User* pUser, int nBldID,int nLV);
	bool Admin_SetArmyLV(User* pUser, int nArmyType,int nLV);
	bool Admin_AddNpc(User* pUser, int nNPCID,bool bAdd);
	bool Admin_SaveNpc(User* pUser);
	bool Admin_AddItem(User* pUser, int nItemType, int nCnt);
	bool Admin_DelBldCnt(User* pUser,int nBldType,int nCnt);
	bool Admin_AddEmblem(User* pUser,int nEmblemType,int nCnt);
	bool Admin_AddWPCCOM(User* pUser,int nWPCCOMID,int nCnt,int nLV);
	bool Admin_AddAdmiral(User* pUser,int nAdmiralID,int nLV);
	bool Admin_SetAdmiralSkill(User* pUser,int nAdmiralID,int nLV);
	bool Admin_DelAdmiral(User* pUser,int nAdmiralID);
	bool Admin_AddAdmiralSoul(User* pUser,int nCnt);
	bool Admin_AddAdmiralTrainExp(User* pUser,int nAdmiralID,int nCnt);
	bool Admin_AddFeats(User* pUser,int nCnt);
	bool Admin_SetInit(User* pUser);
	bool Admin_RecoverArmyHP(User* pUser);
	bool Admin_RecoverBldHP(User* pUser);
	bool Admin_SetScienceLvl(User* pUser,int64 nRes1, int64 nRes2);
	bool Admin_SetGMTestValue(User* pUser,int64 nRes1, int64 nRes2);
	bool Admin_AddMoonCnt(User* pUser,int cnt);
	bool Admin_AddBook(User* pUser,int nBookID,int nCnt,int nLV);
    bool Admin_DelBookExp(User* pUser,int nBookID,int nExp);
	bool Admin_AddMorale(User* pUser,int nCnt);
	bool Admin_AddThew(User* pUser,int nCnt);
	bool Admin_AddZhangong(User* pUser,int nCnt);
	bool Admin_AddCountryBox(User* pUser,int nCnt);

	bool Admin_AddGuildScore(User* pUser,int nCnt);
	bool Admin_AddGuildBldValue(User* pUser,Event* e);
	bool Admin_SetGuildScienceLV(User* pUser,int nScienceID,int nLV);
	bool Admin_SetGuildBldLV(User* pUser,Event* e);

	bool Admin_AddAdmiralTimes(User* pUser,int nCnt);
	bool Admin_ReloadCountryRank();

	bool Admin_ClearStrategic(User* pUser,int nType);
	bool Admin_PveSetCustom(User* pUser,int nCustom,int nRefresh,int nBuyRefresh,int nFail);
	int  Admin_PveFightStart(User* pUser,int nTimes,int nKind);
	bool Admin_ReInitDB(User* pUser);
	bool Admin_AddCoupons(User* pUser,int nCnt);
	bool Admin_ArenaFightSet(Event* e,User* pUser,int nType,int nValue1,int nValue2);
	bool Admin_AddEquip(User* pUser,int nEquipID,int nCnt);
    bool Admin_DelEquip(User* pUser,int nEquipID);
	bool Admin_FinishGuide(User* pUser);
	bool Admin_WorldArenaSet(Event* e,User* pUser,int nType,int nValue1);
	bool Admin_RegionArenaSet(Event* e,User* pUser,int nType,int nValue1);
	bool Admin_HeroArenaSet(Event* e,User* pUser,int nType,int nValue1);

	bool Admin_ClearPassword(User* pUser,int nTime);
	bool Admin_SetOfficerPosition(Event* e,User* pUser,int nPosition,int nFlag);
	bool Admin_AddBuff(Event* e,User* pUser,int nBuffId);
	bool Admin_DelEmblem(User* pUser,int nEmblemType,int nExp);
    bool Admin_DelEmblemExp(User*pUser,int EmblemID,int nExp);
	bool Admin_DelWpccom(User* pUser,int nEmblemType,int nExp);
    bool Admin_DelWpccomExp(User* pUser,int nEmblemID,int nExp);
	bool Admin_SetVip(Event* e,User* pUser,int nType,int nValue);
	bool Admin_SetEquipStrLv(Event* e,User* pUser,int nEquipType,int nOldLv,int nNewLv);
	bool Admin_SpecialValue(Event* e,User* pUser,int nValue);
    bool Admin_ClearNewDayInfo(Event* e,User* pUser);

    void Admin_DailyFirstCost(Player* pPlayer, User* pUser);

	bool Admin_PveBattleSet(Event* e,User* pUser);
	bool Admin_AddJunling(Event* e,User* pUser);
	bool Admin_AddHero(User* pUser,int nHeroID);
	bool Admin_DelHero(User* pUser,int nHeroID);
	bool Admin_AddHeroLetter(User* pUser,int nHeroLetterID,int nCount);
	bool Admin_DelHeroLetter(User* pUser,int nHeroLetterID,int nCount);
	bool Admin_SetHero(User* pUser,int nHeroID,int nType,int nValue);
	bool Admin_RecoverHeroHP(User* pUser);

    bool Admin_AddPokers(Event *e, User *pUser);
    bool Admin_ResetPokerExc(Event *e, User *pUser);

    bool Admin_ResetDuelInfo(Event *e, User *pUser);

	bool Admin_ResetWholeCountry(Event* e);
	bool Admin_WholeCountrySign(Event* e, User *pUser);

	bool Admin_AddAdmiralLetter(User* pUser,int nLetterID,int nCount);
	bool Admin_DelAdmiralLetter(User* pUser,int nLetterID,int nCount);
	bool Admin_SetProperty(User* pUser,int nPropertyId,int nLevel);

	bool Admin_AddGveThew(User* pUser,int nCnt);
	bool Admin_AddGvePoint(User* pUser,int nCnt);
	bool Admin_GveList(Event* e);
    bool Admin_ClearAllBagItem(User *pUser);
    bool Admin_AddRentParts(Event *e, User *pUser);
	bool Admin_AddGuildTask(User* pUser , int taskid);
	bool Admin_SetGuildTaskProgress(User* pUser , int count);

	bool Admin_GvgReward(Event* e,User* pUser,int nValue);
	bool Admin_SetGuildLeaderDelTime(User* pUser);
	bool Admin_SetGuildLeader(User* pUser);
	bool Admin_AddSearchTime(User* pUser,int nValue);
	bool Admin_DelAdmiralSpecial(User* pUser,int nValue);
	bool Admin_DelHeroSpecial(User* pUser,int nValue,int nCnt);
	bool Admin_DelHeroExpSpecial(User* pUser,int nValue);
	bool Admin_DelWpcExpSpecial(User* pUser,int nValue);
	bool Admin_DelEmblemExpSpecial(User* pUser,int nValue);
	bool Admin_DelBookExpSpecial(User* pUser,int nValue);

	bool Admin_DBRoolBack(User* pUser);

	bool Admin_AddExpWpc(User* pUser,int nValue);
	bool Admin_AddExpEmblem(User* pUser,int nValue);
	bool Admin_SetWCPromoteExp(User* pUser , int nExp);
	bool Admin_SetWCSkillExp(User* pUser , int nSkillType , int nSkillExp);
	bool Admin_AddWCSkillExp(User* pUser , int nSkillType , int nSkillExp);
	bool Admin_SubWCSkillExp(User* pUser , int nSkillType , int nSkillExp);
	bool Admin_WCOpType(User* pUser , int nEvtype , int nOptype);
	bool Admin_WCOpUid(User* pUser , int nEvtype);
    bool Admin_AddReport(User* pUser , int nNum);
	bool Admin_SADOp(User* pUser , int nEvtype , int nValue);
    bool Admin_AddWarGameToken(User* pUser , int nNum);
	bool Admin_SetStormBox(User* pUser , int nFlag);
	bool Admin_SetQQTenyearFlag(User* pUser , int nFlag);
	bool Admin_HappylibAward(User* pUser , int nFlag);
    bool Admin_MilitarySoulPoint(User* pUser , int nNum);
    bool Admin_CountryChatUI(User* pUser);
	bool Admin_CountryChatMsg(User* pUser , const string& strMsg);
	bool Admin_CountryChatMask(User* pUser , int type , int64 tuid , const string& strName);
    bool Admin_SetSevenDaysLogonDays(User *pUser, int nDays);
    bool Admin_AddYaSuiQian(User *pUser, int nCnt);
	bool Admin_NpcactiveRank(Event *e , User* pUser);
	bool Admin_TowerEnhance(User* pUser , int nBldid , int nEnhanceType);
	bool Admin_SetTowerEnhance(User* pUser , int nBldid , int nEnhanceType);
    bool Admin_AddJammer(User *pUser, int nJammerType, int nLv, int nCount);
	bool Admin_SetBDNTimes(User *pUser , int nTimes);
	bool Admin_SiteTestAdmin(User* pUser , int nType , int nValue);
#endif
};
#endif
