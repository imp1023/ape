#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;
class Country_FightSendData;

class DealGvgEvent  : public CBaseEvent
{
public:
	DealGvgEvent ();
	~DealGvgEvent ();
	RegistCountryEvent(DealGvgEvent )
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleRceGvgOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGvgBtlCacu(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGvgSeeReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandleGvgOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgClick(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgReport(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgSign(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgEnter(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgAddProp(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgBuyStreak(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgBrokeCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgDefCity(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgGetReward(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgSeeCityPlayer(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgInterface(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgRewardRecord(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);

	void HandleGvgOffLineEnter(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);
	void HandleGvgOffLineLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID,CGuild* pGuild);

	void SaveGvgBtlSendData(int64 nUid,Country_FightSendData* pFightData);

public:
	void CheckGvgBtlStep(CountryRgnDatHandler* pRDH);

};
