#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;
class CWarGameSite;

class DealWarGameEvent : public CBaseEvent
{
public:
    DealWarGameEvent ();
    ~DealWarGameEvent ();
    RegistCountryEvent(DealWarGameEvent )
private:
    void registHandler();
    void handle(Event* e);
private:
    void HandleRceWarGameOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

    void HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleOccupy(Event*e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleLootOver(Event*e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleLeave(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleDelay(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleGetSiteList(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
    void HandleTruce(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

    void FillSiteInfoToRse(RseWarGameOpt *pRseOpt, CWarGameSite *pWarGameSite);
};
