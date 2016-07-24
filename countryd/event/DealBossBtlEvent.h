#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;
class GCG_ResourceOpt;

class DealBossBtlEvent  : public CBaseEvent
{
public:
	DealBossBtlEvent ();
	~DealBossBtlEvent ();
	RegistCountryEvent(DealBossBtlEvent )
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleRceBossBtlOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlOpenBtl(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlUp(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlSeeUp(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlSeeRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID,int nType);
	void HandleBossBtlGetReward(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlSetWords(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlReportWords(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleBossBtlAtk(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

};
