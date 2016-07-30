#define _CRT_SECURE_NO_WARNINGS

#include "../event/DataHandler.h"
#include "../event/EventDefine.h"
#include "CountryRgnDataHandler.h"
#include "CountryDataHandler.h"
#include "CountryDataSaver.h"
#include "CountryEventHandler.h"
#include "MemCacheServerHandler.h"
#include "../event/event.pb.h"
#include <algorithm>
#include <math.h>
#include "../common/Clock.h"
#include "../logic/GameSrvCfg.h"
#include "../logic/StarLogic.h"
#include "../common/SysLog.h"

CountryRgnDatHandler::CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion)
:m_pDH(pDH)
,m_nRegion(nRegion)
{
	m_MaxCityID = 0;
}

CountryRgnDatHandler::~CountryRgnDatHandler()
{

}

void CountryRgnDatHandler::Clear()
{
	
}

void CountryRgnDatHandler::InitCountry()
{
	LoadAllCountryInfo();
}

bool CountryRgnDatHandler::LoadAllCountryInfo()
{
	bool bLoadError = false;
	char szKey[128];
	time_t ltNow = time(NULL);
	m_pCountry = new CCountry(m_nRegion);
	sprintf(szKey,"Country_%d",m_nRegion);
	bool bGet = m_pDH->LoadCountryFromDB(szKey,m_pCountry->m_dbCountry,bLoadError);
	if(bLoadError)
	{//连接数据库失败，直接退出，不会走到这里
		exit(0);
	}
	
	for(int i = 0; i <  m_pCountry->m_dbCountry.city_size(); i++){
		DB_C_City *pDBCity = m_pCountry->m_dbCountry.mutable_city(i);
		CCity *pCity = new CCity(m_nRegion, pDBCity);
		m_mapCitys.insert(make_pair(pDBCity->cityid(), pCity));
		if(pDBCity->cityid() > m_MaxCityID){
			m_MaxCityID = pDBCity->cityid();
		}
	}

	for(int k = 0; k < m_pCountry->m_dbCountry.users_size(); k++){
		DB_C_UserLite *pDBUser = m_pCountry->m_dbCountry.mutable_users(k);
		CUser *pUser = new CUser(pDBUser);
		m_mapUsers.insert(make_pair(pDBUser->uid(), pUser));
	}
}

CCity* CountryRgnDatHandler::GetCity(int nCityID)
{
	map<int, CCity*>::iterator iter = m_mapCitys.find(nCityID);
	if(iter != m_mapCitys.end())
		return iter->second;
	return NULL;
}

CUser* CountryRgnDatHandler::GetUser(int uid)
{
	map<int, CUser*>::iterator iter = m_mapUsers.find(uid);
	if(iter != m_mapUsers.end())
		return iter->second;
	return NULL;
}

void CountryRgnDatHandler::markCountryDirty(CCountry* pCountry)
{
	m_pDH->markCountryDirty(pCountry);
}

void CountryRgnDatHandler::GetNewPlayerStar(int64 uid, int nPlanetId, string platId, string url, string &strRspSku, int &nRspName, int &nRspType, int &nStarId)
{
	DB_C_Country *pDBCountry = m_pCountry->GetDBCountry();
	map<int, CUser*>::iterator _iter = m_mapUsers.find(uid);
	if(_iter == m_mapUsers.end()){
		DB_C_UserLite *pDBUser = pDBCountry->add_users();
		pDBUser->set_uid(uid);
		pDBUser->set_name(platId);
		pDBUser->set_url(url);
		pDBUser->set_xp(0);
		pDBUser->set_score(0);
		pDBUser->set_damageprotectiontimeleft(INIT_PLAYER_DMGPROTECTIONTIMETOTAL);
		pDBUser->set_tutorialcompleted(1);
		pDBUser->set_isonline(0);
		pDBUser->set_levelbasedonscore(1);
		CUser *pUser = new CUser(pDBUser);
		m_mapUsers.insert(make_pair(pDBUser->uid(), pUser));
	}
	
	if(pDBCountry && pDBCountry->city_size() == 0){
		DB_C_City *pDBCity = pDBCountry->add_city();
		CCity *pCCity = new CCity(m_nRegion, pDBCity);
		pCCity->SetCityID(1);
		pCCity->SetX(0);
		pCCity->SetY(0);
		nRspName = StarLogicInst::instance().GetStarName();
		pCCity->SetName(nRspName);
		pCCity->SetType(StarLogicInst::instance().GetStarType());

		DB_C_PlanetLite *pDBPlanet = pDBCity->add_planets();
		pDBPlanet->set_uid(uid);
		pDBPlanet->set_planetid(nPlanetId);
		pDBPlanet->set_type(3);
		nRspType = INIT_PLAYER_PLANETTYPE;
		pDBPlanet->set_hqlevel(1);
		pDBPlanet->set_capital(1);
		char sku[128] = {0};
		sprintf(sku, "%d:%d:%d", pCCity->GetX(), pCCity->GetY(), 1);
		strRspSku = sku;
		pDBPlanet->set_sku(sku);
		m_MaxCityID = 1;
		nStarId = 1;
		m_mapCitys.insert(make_pair(pCCity->GetCityID(), pCCity));
	}else{
		map<int, CCity*>::iterator iter = m_mapCitys.find(m_MaxCityID);
		if(iter != m_mapCitys.end()){
			CCity *pCity = iter->second;
			int nSize = pCity->GetPlanetSize();
			if(nSize < StarLogicInst::instance().STAR_ALLOCATE_OCCUPIED){
				DB_C_City *pDBCity = pCity->GetDBCity();
				DB_C_PlanetLite *pDbPlanet = pDBCity->add_planets();
				pDbPlanet->set_uid(uid);
				pDbPlanet->set_planetid(nPlanetId);
				pDbPlanet->set_type(INIT_PLAYER_PLANETTYPE);
				nRspType = INIT_PLAYER_PLANETTYPE;
				pDbPlanet->set_hqlevel(1);
				pDbPlanet->set_capital(1);
				char sku[128] = {0};
				sprintf(sku, "%d:%d:%d", pCity->GetX(), pCity->GetY(), nSize+1);
				pDbPlanet->set_sku(sku);
				strRspSku = sku;
				nRspName = pCity->GetName();
				nStarId = m_MaxCityID;
			}else{
				int x = 0, y = 0, newx = 0, newy = 1;
				map<int, CCity*>::iterator iter = m_mapCitys.find(m_MaxCityID);
				if(iter != m_mapCitys.end()){
					x = iter->second->GetX();
					y = iter->second->GetY();
				}
				StarLogicInst::instance().GetNewStar(x, y, newx, newy);
				DB_C_City *pDBCity = pDBCountry->add_city();
				CCity *pCity = new CCity(m_nRegion, pDBCity);
				pDBCity->set_cityid(++m_MaxCityID);
				pDBCity->set_x(newx);
				pDBCity->set_y(newy);
				pDBCity->set_type(StarLogicInst::instance().GetStarType());
				nRspName = StarLogicInst::instance().GetStarName();
				pDBCity->set_name(nRspName);
				DB_C_PlanetLite *pDBPlanet = pDBCity->add_planets();
				pDBPlanet->set_uid(uid);
				pDBPlanet->set_planetid(nPlanetId);
				pDBPlanet->set_type(INIT_PLAYER_PLANETTYPE);
				nRspType = INIT_PLAYER_PLANETTYPE;
				pDBPlanet->set_hqlevel(1);
				pDBPlanet->set_capital(1);
				char sku[128] = {0};
				sprintf(sku, "%d:%d:%d", pCity->GetX(), pCity->GetY(), 1);
				pDBPlanet->set_sku(sku);
				strRspSku = sku;
				nStarId = m_MaxCityID;
				m_mapCitys.insert(make_pair(m_MaxCityID, pCity));
			}
		}
	}
	markCountryDirty(m_pCountry);
}

void CountryRgnDatHandler::FillinStarInfo(int nStarId, RseQueryStarInfo *rsp)
{
	if(!rsp){
		return;
	}
	CCity *PCCity = GetCity(nStarId);
	if(PCCity){
		char sku[128] = {0};
		sprintf(sku, "%d:%d", PCCity->GetX(), PCCity->GetY());
		rsp->set_sku(sku);
		rsp->set_starid(PCCity->GetCityID());
		rsp->set_name(PCCity->GetName());
		rsp->set_type(PCCity->GetType());
		DB_C_City *pDBCity = PCCity->GetDBCity();
		if(pDBCity){
			for(int i = 0; i < pDBCity->planets_size(); i++){
				DB_C_PlanetLite *pDBPlanet = pDBCity->mutable_planets(i);
				SpaceStarInfo *pMsgPlanet = rsp->add_spacestarinfo();
				if(pDBPlanet && pMsgPlanet){
					pMsgPlanet->set_sku(pDBPlanet->sku());
					pMsgPlanet->set_planetid(pDBPlanet->planetid());
					pMsgPlanet->set_type(pDBPlanet->type());
					pMsgPlanet->set_hqlevel(pDBPlanet->hqlevel());
					pMsgPlanet->set_capital(pDBPlanet->capital());
					pMsgPlanet->set_accountid(toString(pDBPlanet->uid()));
					CUser *pUser = GetUser(pDBPlanet->uid());
					if(pUser){
						pMsgPlanet->set_name(pUser->GetName());
						pMsgPlanet->set_url(pUser->GetURL());
						pMsgPlanet->set_xp(pUser->GetXP());
						pMsgPlanet->set_score(pUser->GetScore());
						pMsgPlanet->set_damageprotectiontimeleft(0/*pUser->GetDmgProtectionTimeLeft()*/);
						pMsgPlanet->set_tutorialcompleted(pUser->GetTutorialCompleted());
						pMsgPlanet->set_isonline(pUser->GetOnline());
						pMsgPlanet->set_levelbasedonscore(pUser->GetLevelOnScore());
					}
				}
			}
		}
	}
}
