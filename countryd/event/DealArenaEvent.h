#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;

class DealArenaEvent : public CBaseEvent
{
public:
	DealArenaEvent();
	~DealArenaEvent();
	RegistCountryEvent(DealArenaEvent)
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleWorldArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleWorldArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleWorldArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleHeroArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleHeroArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleHeroArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
};
