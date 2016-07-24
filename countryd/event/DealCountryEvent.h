#pragma once

#include "BaseEvent.h"
class DB_C_City;
class GCG_CountryOpt;
class PVPFightUser;
class CountryReport;
class CountryRgnDatHandler;
class Admin_AdminOpt_Req;

struct SelCity4BloodBtl
{
	CCity* pCity1;
	CCity* pCity2;
	int nKey;
};

class DealCountryEvent : public CBaseEvent
{
public:
	DealCountryEvent();
	~DealCountryEvent();
	RegistCountryEvent(DealCountryEvent)
private:
	void registHandler();
	void handle(Event* e);
private:
	void processReloadConfig(Event* e);
	void CheckCountryBloodBtl();
	void CheckAutoBloodBtlStart(CountryRgnDatHandler* pRDH);	//判断自动血战开启

	void HandleCountryTimer(Event* e);
	void HandleCountryLite(Event* e);
	void HandleountryBloodBtlOver(Event* e);
	void HandleUpdateCountryUser(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCountryUpdateUserNameData(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRceCountryOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleFouceChgCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleAdminOpt(CountryRgnDatHandler* pRDH,Admin_AdminOpt_Req* req);
	void HandleUpdateWorldArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRceOfficerOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRceOfficerOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRceOfficerRightInfo(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCountryUserLite(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleRceWPCBaseOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleUpdateCountryWpcRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCountryChatOpt(Event* e , CountryRgnDatHandler* pRDH , int nGameID);
	void HandleCountryChatUI(Event* e , CountryRgnDatHandler* pRDH , int nGameID);
	void HandleCountryChat(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCountryChatWorld(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleResetGuildPKScore(Event* e,CountryRgnDatHandler* pRDH, int nRegionID);
	void HandleUpdataNpcActiveRecord(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGetNpcActiveRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleOutputGuildUserList(Event* e,CountryRgnDatHandler* pRDH, int nRegionID);
	void HandGetAllArmRaceRank(Event* e,CountryRgnDatHandler* pRDH, int nGameID);
	void HandGetSelfArmRaceRank(Event* e,CountryRgnDatHandler* pRDH, int nGameID);
	//void HandYiYuanPurchaseOpt(Event* e,CountryRgnDatHandler* pRDH, int nGameID);
	
private:
	void AttackNPCOver(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID);
	void AttackUserOver(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH);
	void OptGotoCity(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID);
	void OptLaunchBloodBtl(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID);
	bool CheckCaptureCity(CountryRgnDatHandler* pRDH,DB_C_User* pDBCUser,CCity* pSelfCity,CCity* pTarCity);
	void AddCountryReport(CountryReport* pReport,CountryRgnDatHandler* pRDH,int nGameID);
	void AddCountryReport(PVPFightUser* pMsgAtk,PVPFightUser* pMsgDef,CountryRoportType emType,CountryRgnDatHandler* pRDH,int nGameID);
	void AddCountryReport(DB_C_BloodBtl* pDBBloodBtl,CountryRgnDatHandler* pRDH);
	void SendBloodWinRedward(CountryRgnDatHandler* pRDH,CCity* pWinCity,DB_C_BloodBtl* pDBBloodBtl,int64 nCaputreUserID=0);
	void SendBloodLoseRedward(CountryRgnDatHandler* pRDH,CCity* pWinCity,DB_C_BloodBtl* pDBBloodBtl);

	void OptBalanceBloodBtl(CountryRgnDatHandler* pRDH,int nCityID);

    void OptTeleportCity(Event* e,int64 nUserID,CountryRgnDatHandler* pRDH,int nGameID);

};
