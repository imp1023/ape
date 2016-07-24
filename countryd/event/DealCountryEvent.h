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
	void HandleCountryLite(Event* e);
	void HandleUpdateCountryUser(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
};
