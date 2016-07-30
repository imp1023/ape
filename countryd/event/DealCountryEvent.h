#pragma once

#include "BaseEvent.h"

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
	void HandleCountryLite(Event* e);
	void HandleNewPlayerLogin(Event* e, CountryRgnDatHandler *pRDH, int nGameID);
	void HandleQueryStarInfo(Event* e, CountryRgnDatHandler *pRDH, int nGameID);
	void HandleQueryGalaxyWindow(Event* e, CountryRgnDatHandler *pRDH, int nGameID);
};
