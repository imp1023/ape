#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;
class GCG_ResourceOpt;

class DealResourceEvent  : public CBaseEvent
{
public:
	DealResourceEvent ();
	~DealResourceEvent ();
	RegistCountryEvent(DealResourceEvent )
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleRceResourceOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleSearch(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleAttackOver(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleUpdate(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void FillResourceData( CResource* pResource, GCG_ResourceOpt* pResourceMsg, int nTime, bool bWin = true );
};
