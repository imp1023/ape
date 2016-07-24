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
#include "../common/SysLog.h"

CountryRgnDatHandler::CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion)
:m_pDH(pDH)
,m_nRegion(nRegion)
{
	
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
		CCity *pCity = new CCity(m_nRegion);
		m_mapCity.insert(make_pair(pDBCity->cityid() ,pCity));
	}
}

CCity* CountryRgnDatHandler::GetCity(int nCityID)
{
	map<int, CCity*>::iterator iter = m_mapCity.find(nCityID);
	if(iter!=m_mapCity.end())
		return iter->second;
	return NULL;
}

void CountryRgnDatHandler::markCountryDirty(CCountry* pCountry)
{
	m_pDH->markCountryDirty(pCountry);
}
