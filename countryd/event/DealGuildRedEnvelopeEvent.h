#pragma once

#include "BaseEvent.h"
class CountryRgnDatHandler;
class GCG_ResourceOpt;

class DealGuildRedEnvelopeEvent  : public CBaseEvent
{
public:
	DealGuildRedEnvelopeEvent ();
	~DealGuildRedEnvelopeEvent ();
	RegistCountryEvent(DealGuildRedEnvelopeEvent )
private:
	void registHandler();
	void handle(Event* e);
private:
	void HandleRceGuildRedEnvelopeOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandleOpen(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCheckRedEnvelopeInfo(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleIssueRedEnvelope(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleCheckRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleSnappeRedEnvelope(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
	void HandleGetRedEnvelopeList(Event* e,CountryRgnDatHandler* pRDH,int nGameID);

	void HandYiYuanPurchaseOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID);
};
