#define _CRT_SECURE_NO_WARNINGS
#include "CountryEventHandler.h"
#include "CountryNetHandler.h"
#include "CountryDataHandler.h"
#include "CountryRgnDataHandler.h"
#include "../logic/ServerConfig.h"
#include "countryDB.pb.h"
#include "../event/EventDefine.h"
#include "event/DealCountryEvent.h"
#include "event/DealGuildEvent.h"
#include "event/DealArenaEvent.h"
#include "event/DealResourceEvent.h"
#include "event/DealGvgEvent.h"
#include "event/DealWarGameEvent.h"
#include "event/DealBossBtlEvent.h"
#include "event/DealGuildRedEnvelopeEvent.h"

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
	DealGuildEvent::createInstance(this);
	DealArenaEvent::createInstance(this);
	DealResourceEvent::createInstance(this);
	DealGvgEvent::createInstance(this);
    DealWarGameEvent::createInstance(this);
	DealBossBtlEvent::createInstance(this);
    DealGuildRedEnvelopeEvent::createInstance(this);
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


void CountryEventHandler::SendBloodBtlInfo(int nSelRegion /* = -1 */, int nGameID /* = -1 */,int nBroadAtkCity /* = 0 */, int nBroadDefCity /* = 0 */, bool bOver/* =false */,map<int,bool>* pMapBroacdCity /* = NULL */)
{
	CountryDataHandler* pDH = getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();

	Event evn;
	evn.set_cmd(EVENT_COUNTRY_BLOODBTL_INFO);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);

	//GCG_CountryLite* pRseCnt = evn.mutable_countrycnt();
	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];
		if(nSelRegion>=0&&nRegion!=nSelRegion)
			continue;
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(pRDH==NULL)
			continue;
		map<int,DB_C_BloodBtl*>& mapBlt = pRDH->GetBloobBtlLst();

		map<int,DB_C_BloodBtl*>::iterator iter;
		for(iter = mapBlt.begin();iter!=mapBlt.end();iter++)
		{
			DB_C_BloodBtl* pDBBtl = iter->second;
			if(pDBBtl==NULL||pDBBtl->launchtime()<=0)
				continue;
			GCG_CountryBloodBtl* pLite = evn.add_countrybloodbtl();
			pLite->set_region(nRegion);
			if(nRegion>=0)
			{
				if(nBroadAtkCity==pDBBtl->atkcity()&&nBroadDefCity==pDBBtl->defcity())
				{
					pLite->set_broadcast(true);
				}
				if(pMapBroacdCity)
				{
					if((*pMapBroacdCity)[pDBBtl->atkcity()])
					{
						pLite->set_launch(true);
					}
					if((*pMapBroacdCity)[pDBBtl->defcity()])
					{
						pLite->set_launch(true);
					}
				}
			}

			CityBloodBattle* pInfo = pLite->mutable_bloodbtl();
			pRDH->FillBloodBtl(pDBBtl,pInfo,bOver);
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
}
void CountryEventHandler::SendCityBuffInfo(int nGameID /* = -1 */)
{
	Event evn;
	evn.set_cmd(EVENT_CITYBUFF_INFO);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);

	CountryDataHandler* pDH = getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();
	for(int i = 0; i < (int)lstRgn.size(); i++){
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(lstRgn[i]);
		if(!pRDH){continue;}

		RseCityBuffInfo *pRse = evn.mutable_se_rsecitybuffinfo();
		pRDH->FillCityBuffInfo(pRse);
		if(nGameID > 0){
			sendEventToGamed(&evn,nGameID);
		}else{
			sendEventToAllGamed(&evn);
		}
	}
}

void CountryEventHandler::SendCountryLiteInfo(int nGameID /* = -1 */)
{
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
}

void CountryEventHandler::BroadCastMsg_GuildBtlSessionOver(int nRegion,string strGuildName,string strLeaderNmae)
{
	Event evn;
	evn.set_cmd(EVENT_COUNTRY_BROADCAST);
	evn.set_state(Status_Game_To_Country);
	evn.set_time(0);
	evn.set_uid(0);
	RseComBroadcast* pBrd = evn.mutable_se_rsecombroadcast();
	pBrd->set_id(43);
	pBrd->set_args1(strGuildName);
	pBrd->set_args2(strLeaderNmae);
	pBrd->set_argslist("s1,s2");	
	pBrd->set_region(nRegion);

	sendEventToGamed(&evn,1);
}

void CountryEventHandler::SendCountryUserLiteInfo(CountryRgnDatHandler* pRDH,DB_C_User* pDBCUser)
{
	if(NULL == pDBCUser||NULL == pRDH)
	{
		return;
	}
	CCountry* pCountry = pRDH->GetCountry(pDBCUser->countryid());
	if(NULL == pCountry)
	{
		return;
	}
	pCountry->UpdateUserOfficerPerDay();
	DB_C_OfficerInfo* pDBCOfficerInfo = pCountry->GetDBCountry()->mutable_officerinfo();

	Event evn;
	evn.set_cmd(EVENT_COUNTRY_USER_LITE);
	evn.set_state(Status_Game_To_Country);
	evn.set_time(0);
	evn.set_uid(pDBCUser->userid());
	GCG_CountryUserLite* pUserLite= evn.mutable_countryuserlite();
	pUserLite->set_countryid(pDBCUser->countryid());
	pUserLite->set_officerposition(pDBCUser->officerposition());
	pUserLite->set_officerrightuseflag(pDBCOfficerInfo->officerrightuseflag());
	pUserLite->set_weaponrank(pRDH->GetWpcRankRight( pDBCUser ));
	int nGameId = getGamedIdByUserId(pDBCUser->userid());
	sendEventToGamed(&evn,nGameId);
}
void CountryEventHandler::SendOfficerRightUsed(CountryRgnDatHandler* pRDH,CCity* pCity,int64 nUid /* = 0 */)
{
	if(pRDH==NULL||pCity==NULL)
		return;
	Event evn;
	evn.set_cmd(EVENT_OFFICER_RIGHT_USE_INFO);
	evn.set_state(Status_Game_To_Country);
	evn.set_time(0);
	evn.set_uid(0);
	CityRightInfo* pCityRight = evn.mutable_cityright();

	if(pCity->UpdateOfficerRightBuff())
	{
		pRDH->markCityDirty(pCity);
	}
	if(!pCity->HasOfficerRightBuff())
	{
		return;
	}
	pRDH->FillOfficerRightUsed(pCity->GetDBCity(),pCityRight);
	if(nUid>0)
	{
		evn.set_uid(nUid);
		sendEventToGamed(&evn,getGamedIdByUserId(nUid));
	}
	else
	{
		for(int i=0;i<CITY_USER_GROUP;i++)
		{
			for(int j=0;j<(int)pCity->m_lstUser[i].size();j++)
			{
				DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
				if(pDBCUser==NULL)
				{
					continue;
				}
				if(pDBCUser->cityid()!=pCity->GetCityID())
				{
					continue;
				}
				evn.set_uid(pDBCUser->userid());
				sendEventToGamed(&evn,getGamedIdByUserId(pDBCUser->userid()));
			}
		}
	}

}
void CountryEventHandler::SendOfficerRightInfo2User(CountryRgnDatHandler* pRDH,int countryId /* = 0 */,int64 nUid /* = 0 */,E_Officer_Fill_Type fillType /* = E_Officer_Fill_ALL */)
{
	if(NULL == pRDH||nUid<=0)
	{
		return;
	}
	if(!pRDH->IsValidCountry(countryId))
		return;
	CCountry* pCountry = pRDH->GetCountry(countryId);
	if(pCountry==NULL)
		return;

	Event evn;
	evn.set_cmd(C2S_RceOfficerRightInfo);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);
	evn.set_uid(nUid);

	RseOfficerRightInfo* pInfo = evn.mutable_se_rseofficerrightinfo();

	pInfo->set_countryid(countryId);
	pInfo->set_type(fillType);
	CoutryOfficer* pOfficerInfo = pInfo->mutable_officerinfo();
	pRDH->FillCountrOfficerInfo(pCountry,pOfficerInfo,fillType);
	sendEventToGamed(&evn,getGamedIdByUserId(nUid));
}
void CountryEventHandler::SendOfficerRightInfo2CountryUser(int countryId/* = 0 */,int nSelRegion/* = -1 */,int nGameID /* = -1 */,E_Officer_Fill_Type fillType /*=E_Officer_Fill_ALL*/)
{
	CountryDataHandler* pDH = getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();

	Event evn;
	evn.set_cmd(EVENT_OFFICER_RIGHT_CITY_ALL);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);

	RseOfficerRightInfo* pInfo = evn.mutable_se_rseofficerrightinfo();
	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];
		if(nSelRegion>=0&&nRegion!=nSelRegion)
			continue;
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(pRDH==NULL)
			continue;
		if(!pRDH->IsValidCountry(countryId))
			continue;
		CCountry* pCountry = pRDH->GetCountry(countryId);
		if(pCountry==NULL)
			continue;
		pInfo->set_countryid(countryId);
		pInfo->set_region(nRegion);
		pInfo->set_type(fillType);
		CoutryOfficer* pOfficerInfo = pInfo->mutable_officerinfo();
		pRDH->FillCountrOfficerInfo(pCountry,pOfficerInfo,fillType);
	}
	if(nGameID>0)
	{
		sendEventToGamed(&evn,nGameID);
	}
	else
	{
		sendEventToAllGamed(&evn);
	}
}

void CountryEventHandler::SendBossBtlLiteInfo(int nGameID /* = -1 */)
{
	CountryDataHandler* pDH = getDataHandler();
	const vector<int>& lstRgn = pDH->GetRegionLst();

	Event evn;
	evn.set_cmd(EVENT_BOSSBTL_LITE);
	evn.set_state(Status_Country_To_Game);
	evn.set_time(0);

	//GCG_CountryLite* pRseCnt = evn.mutable_countrycnt();
	for(int i=0;i<(int)lstRgn.size();i++)
	{
		int nRegion = lstRgn[i];

		if ( nRegion == 0 )
		{
			int n = 1;
		}
		CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
		if(pRDH==NULL)
			continue;

		GCG_BossBtlLite* pLite = evn.mutable_bossbtllite();
		pLite->set_region(nRegion);
		
		pRDH->FillBossBtlLite(pLite);

		if(nGameID>0)
		{
			sendEventToGamed(&evn,nGameID);
		}
		else
		{
			sendEventToAllGamed(&evn);
		}
	}
}
