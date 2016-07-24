#pragma once

#include <log4cxx/logger.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>

#include <pthread.h>
#include "../event/DataHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../common/Rand.h"
#include "countryDB.pb.h"

using namespace std;
#ifndef WIN32
#include <ext/hash_map>
using namespace __gnu_cxx;
namespace __gnu_cxx
{

	template<>
	struct hash<long long>
	{

		size_t
			operator()(long long __x) const
		{
			return __x;
		}
	} ;

	template<>
	struct hash<std::string>
	{

		size_t
			operator()(std::string __s) const
		{
			return __stl_hash_string(__s.c_str());
		}
	} ;
}
#else
#include <hash_map>
using namespace stdext;
#endif

class CCity
{
public:
	CCity(int nRegion):m_nRegion(nRegion)
	{	
		m_dbCity.Clear();
	}
	~CCity()
	{
		m_dbCity.Clear();
	}

	int GetCityID(){return m_dbCity.cityid();}
	int GetRegion(){return m_nRegion;}
	DB_C_City* GetDBCity(){return &m_dbCity;}

private:
	DB_C_City m_dbCity;
	int	m_nRegion;
};

class CCountry
{
public:
	CCountry(int nRegionID)
		:m_nRegion(nRegionID)
	{
		m_lstCity.clear();
		m_dbCountry.set_region(nRegionID);
	}

	~CCountry()
	{
		Clear();
	}

	void Clear()
	{
		m_lstCity.clear();
	}

	int GetKey(){return m_nRegion;}
	DB_C_Country* GetDBCountry(){return &m_dbCountry;}

public:
	DB_C_Country m_dbCountry;
	int m_nRegion;
	vector<CCity*> m_lstCity;
};

class CountryDataHandler;
class CountryRgnDatHandler 
{//国家分区数据信息
public:
	CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion);
	~CountryRgnDatHandler();

public:
	void InitCountry();
	bool LoadAllCountryInfo();
	void LoadAllCityInfo();	

	void Clear();
	int GetRegion(){return m_nRegion;}

	void markCityDirty(CCity* pCity);
	void markCountryDirty(CCountry* pCountry);

public:
	CCity*		GetCity(int nCityID);
	int			GetCityKey(int nCityID){return (m_nRegion*MAX_CITY_ID + nCityID);}

private:
	CountryDataHandler* m_pDH;
	CCountry*			m_pCountry;
	int					m_nRegion;
	map<int, CCity*>	m_mapCity;
};