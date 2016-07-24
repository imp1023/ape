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

	//GCG_CountryLite* pRseCnt = evn.mutable_countrycnt();
	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(pRDH==NULL)
			continue;
		for(int j=C_Start;j<C_UserCoutryEnd;j++)
		{

			if(!pRDH->IsValidCountry(j))
				continue;
			CCountry* pCountry = pRDH->GetCountry(j);
			if(pCountry==NULL)
				continue;
			GCG_CountryLite* pLite = evn.add_countrylite();
			
			pLite->set_guildcnt(pRDH->GetGuildCnt(j));
			pLite->set_countryid(pCountry->m_nCountryID);
			pLite->set_region(nRegion);
			pLite->set_usercnt(pCountry->GetCountryUserCnt());
			pLite->set_lastrank(pRDH->GetCountryLastDayRank(pCountry->m_nCountryID));

			pLite->set_atktargetcity(pCountry->GetDBCountry()->atktargetcity());
			pLite->set_deftargetcity(pCountry->GetDBCountry()->deftargetcity());

			int64 nPrensidentUid = pCountry->GetOfficerPositionUser(Officer_Chairman);
			if(nPrensidentUid<=0)
			{
				nPrensidentUid = pCountry->GetOfficerPositionUser(Officer_President);
			}
			if(nPrensidentUid>0)
			{
				DB_C_User* pUser = pRDH->GetDBCityUser(nPrensidentUid);
				if(pUser)
				{
					CGuild* pGuild = pRDH->GetGuild(pUser->guildid());
					if(pGuild&&pGuild->GetLeaderID()==nPrensidentUid)
					{
						pLite->set_presidentuid(pGuild->GetLeaderID());
						pLite->set_presidentname(pGuild->GetLeaderName());
						pLite->set_presidentguildname(pGuild->GetGuildName());
					}
				}
			}
			vector<CCity*>::iterator iter;
			for(iter = pCountry->m_lstCity.begin();iter!=pCountry->m_lstCity.end();iter++)
			{
				CCity* pCity = *iter;
				pLite->add_cityid(pCity->GetCityID());
			}
		}
	}
	if(nGameID>0)
	{
		sendEventToGamed(&evn,nGameID);
	}
	else
	{
		sendEventToAllGamed(&evn);
	}
#endif
}
