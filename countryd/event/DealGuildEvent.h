#pragma once

#include "BaseEvent.h"
class DB_C_Guild;
class CGuild;
class GCG_GuildOpt;
class CountryRgnDatHandler;
class Country_FightSendData;
class GVEMsg;
class DB_GVE_List;
class GVEList;

class DealGuildEvent : public CBaseEvent
{
public:
	DealGuildEvent();
	~DealGuildEvent();
	RegistCountryEvent(DealGuildEvent)
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleRceGuildOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRcePurchase(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGuildBtlCacu(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGuildBtlSeeReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleAdminAddGuildBtlRedward(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleAdminGveList(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
private:
	void Handle_GuildPageOpen(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_SrvCreateGuild(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	
	void Handle_OpenMemberPage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_ChgLeader(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_ApplyGuild(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_ApplyList(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GuildBldUpgrade(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_OpenGuildShop(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_SetGuildShopPmt(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GBtlSinUp(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GBtlOpenPage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGBtlGuildLst(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGBtlAllRankTotal(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGBtlAllRank(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GbtlSetLineup(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GBtlSeeLastBtl(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GBtlSeeReward(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GbtlSeeStorage(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GBtlSendStorageItem(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandleChgLdeaderByDelate(DB_C_User* pDBCUser,Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	//GVE
	void Handle_GveOpen(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveNpcList(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveSpNpcList(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveAttackOver(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveGetBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveRankBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveUseBuff(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveSyp(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveRank(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID,int nType,int nCountry = 0);
	void Handle_GveMap(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_SetLeader(DB_C_User* pDBCUser,CGuild* pGuild,Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void Handle_GveAllRankBox(DB_C_User* pDBCUser,CGuild* pGuild ,Event* e,CountryRgnDatHandler* pRDH,int nGameID);

public:
	//工会战
	void CheckGuildBtlStep(CountryRgnDatHandler* pRDH);
	void CacuGuildBtl(CountryRgnDatHandler* pRDH);

private:
	void CheckDelateLeader(CountryRgnDatHandler* pRDH,CGuild* pGuild);	//判断是否符合弹劾会长条件
	void CheckGuildUser(int64 nUserID,GCG_GuildOpt* pSrvOpt,CountryRgnDatHandler* pRDH);

private:
	void SaveGuildBtlSendData(DB_C_User* pDBCUser,Country_FightSendData* pFightData);

private:
	//GVE相关
	void CopyGveMsg( GVEMsg* pMsg, DB_GVE_Msg* pDBMsg );
	bool CheckCaptureCity(DB_GVE_City* pDBCity);
	void FillGveBuffMsg( RseGuildOpt& rep, DB_GVE_City* pDBCity );
	void FillGveRankList( GVEList* pList, DB_GVE_List* pDBList );

};
