#define _CRT_SECURE_NO_WARNINGS
#include "CountryEventHandler.h"
#include "CountryNetHandler.h"
#include "CountryDataHandler.h"
#include "CountryRgnDataHandler.h"
#include "../common/ServerConfig.h"
#include "countryDB.pb.h"
#include "../event/EventDefine.h"
#include "event/DealCountryEvent.h"

extern int G_CountrySrvD_ID; 

CountryEventHandler::CountryEventHandler(EventQueue *eq, CountryDataHandler* dh, CountryNetHandler *nh, int nid)
:eq_(eq), dh_(dh), nh_(nh), nid_(nid)
{
	eh_ = new EventHandler(eq, dh, nh, nid);
	logger_ = log4cxx::Logger::getLogger("CountryEventHandler");
	initialEventProcessors();
}

CountryEventHandler::~CountryEventHandler(void)
{
}

void CountryEventHandler::start()
{
	eh_->start(Srv_Country,1000);
}


bool CountryEventHandler::sendEventToGamed(Event *e, int gid) {
	return nh_->sendEventToGamed(e, gid);
}

bool CountryEventHandler::sendEventToAllGamed(Event *e)
{
	return nh_->sendEventToAllGamed(e);
}

void CountryEventHandler::sendFdString(int fd, const string &content) {
	nh_->sendSizedFdString(fd, content.c_str());
}

#include "event/UpdateWorkingStatus.h"


void CountryEventHandler::initialEventProcessors()
{
	// add Event Processors here
	UpdateWorkingStatus::createInstance(this);
	DealCountryEvent::createInstance(this);
}

int CountryEventHandler::getGamedIdByUserId(int64 uid)
{
	int hash = getUidHash(uid);
	//return hash % serverConfig.gamedNum() + 1;
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = getPhysicsRegion(uid);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;
}

void CountryEventHandler::SendCountryLiteInfo(int nGameID /* = -1 */)
{
#if 0
	CountryDataHandler* pDH = getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();

	Event evn;
	evn.set_cmd(EVENT_COUNTRY_LITE);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);

	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(!pRDH)
			continue;

		CCountry* pCountry = pRDH->GetCountry();
		if(!pCountry)
			continue;

		GCG_CountryLite* pLite = evn.add_countrylite();
		pLite->set_region(nRegion);
		map<int, CCity*>::iterator iter;
		for(iter = pCountry->m_mCitys.begin();iter!=pCountry->m_mCitys.end();iter++)
		{
			GCG_City *pMsgCity = pLite->add_city();
			CCity *pDBCity = iter->second;
			if(pMsgCity && pDBCity){
				pMsgCity->set_cityid(pDBCity->GetCityID());
				pMsgCity->set_x(pDBCity->GetX());
				pMsgCity->set_y(pDBCity->GetY());
				pMsgCity->set_name(pDBCity->GetName());
				for(int k = 0; k < pDBCity->m_dbCity.planets_size(); k++){
					DB_C_PlanetLite *pDBPlanet = pDBCity->m_dbCity.mutable_planets(k);
					GCG_PlanetLite *pMsgPlanet = pMsgCity->add_planets();
					if(pMsgPlanet && pDBPlanet){
						pMsgPlanet->set_planetid(pDBPlanet->planetid());
						pMsgPlanet->set_uid(pDBPlanet->uid());
					}
				}
			}
		}
	}

	if(nGameID>0){
		sendEventToGamed(&evn,nGameID);
	}else{
		sendEventToAllGamed(&evn);
	}
#endif
}
