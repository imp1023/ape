#define _CRT_SECURE_NO_WARNINGS

#include "../event/DataHandler.h"

#include "CountryDataHandler.h"
#include "CountryDataSaver.h"
#include "CountryEventHandler.h"
#include "MemCacheServerHandler.h"
#include <algorithm>

#include <math.h>
#include "CountryData.pb.h"

#include "../logic/Clock.h"
#include "../logic/CountryInfoCfg.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"

using namespace std;


CountryDataHandler::CountryDataHandler(int nid) : DataHandler(nid)
{
    nid_ = nid;
    ds_ = new CountryDataSaver(nid);

	game_status_.resize(serverConfig.gamedNum()+1, NOT_CONNECTED);
	game_status_[0] = NORMAL;
	game_all_ready_ = false;

    revision_ = 1;
    logger_ = log4cxx::Logger::getLogger("CountryDataHandler");

	timeUpdate			= time(NULL);
	m_ltStart			= Clock::getCurrentSystemTime();
	
	InitCountry(); //调用此函数时，程序没有完全启动

    pthread_mutex_init(&data_mutex_, NULL);

}

CountryDataHandler::~CountryDataHandler(void)
{


    acquireDataLock();

    releaseDataLock();

	Clear();
    delete ds_;

	pthread_mutex_destroy(&data_mutex_);
}

void CountryDataHandler::Clear()
{

}

void CountryDataHandler::tick()
{
	saveAllDBData();
	MemCacheServerHandler::SaveAllMemDBData(revision_);
}
void CountryDataHandler::quit()
{
	MemCacheServerHandler::SaveAllMemDBData(revision_,true);
	//saveAllUserData(true);
	saveAllDBData(true);
}

void CountryDataHandler::termsThread()
{
	ds_->termThreads();
	exit(0);
}

int CountryDataHandler::getGamedIdByUserId(int64 uid)
{ 
	int hash = getUidHash(uid);

	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = getPhysicsRegion(uid);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;
}

void CountryDataHandler::InitCountry()
{
	m_lstRegion.clear();
	int nSrvCnt = serverConfig.country_num();
	for(int i=0;i<serverConfig.region_num();i++)
	{
		int nRegion = i;
		int nNewRegion = serverConfig.getmapregion(nRegion);
		if(nRegion != nNewRegion)
		{//被合掉的分区，可以不启动该分区了
			continue;
		}
		if((i%nSrvCnt+1) == nid_)
		{
			m_lstRegion.push_back(i);
			CountryRgnDatHandler* pRDH = new CountryRgnDatHandler(this,i);
			m_mapRegionDH[i] = pRDH;
			pRDH->InitCountry();
		}
	}
}
//void CountryDataHandler::InitCountry()
//{
//	m_lstRegion.clear();
//	int nSrvCnt = serverConfig.country_num();
//	for(int i=0;i<serverConfig.region_num();i++)
//	{
//		if((i%nSrvCnt+1) == nid_)
//		{
//			m_lstRegion.push_back(i);
//			for(int j=C_Start;j<C_CountryCnt;j++)
//			{
//				if(!IsValidCountry(j))
//					continue;
//				CCountry* pCountry = new CCountry(j,i);
//				m_mapCountry[pCountry->GetKey()] = pCountry;
//			}
//		}
//	}
//	LoadAllCountryInfo();
//
//	//检查国家和首都
//	for(int i=0;i<m_lstRegion.size();i++)
//	{
//		int nRegion = m_lstRegion[i];
//		for(int j=C_Start;j<C_UserCoutryEnd;j++)
//		{
//			if(!IsValidCountry(j))
//				continue;
//			CCountry* pCountry = GetCountry(nRegion,j);
//			if(pCountry==NULL)
//			{//
//				printf("ERROR!!!! can't find country:%d,region:%d..............\r\n",j,nRegion);
//				exit(0);
//			}
//			if(pCountry->GetCapital()==NULL)
//			{
//				printf("ERROR!!!! country:%d,region:%d can't find capital..............\r\n",j,nRegion);
//				exit(0);
//			}
//		}
//	}
//	
//}

CountryRgnDatHandler*  CountryDataHandler::GetRegionDatHandler(int nRegionID)
{
	map<int,CountryRgnDatHandler*>::iterator iter;
	iter = m_mapRegionDH.find(nRegionID);
	if(iter!=m_mapRegionDH.end())
		return iter->second;
	return NULL;

}

bool CountryDataHandler::LoadGuildLstFromDB(string szKey,DB_C_GuildList& dbLst,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbLst))
	{
		return true;
	}
	int nRet = ds_->loadGuildLst(szKey,dbLst);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

int	CountryDataHandler::GetTotalArenaPlayerNum()
{
	int nTotal = 0;
	map<int,CountryRgnDatHandler*>::iterator iter = m_mapRegionDH.begin();
	for(;iter!=m_mapRegionDH.end();iter ++)
	{
		nTotal += iter->second->GetArenaPlayerNum();
	}
	return nTotal;
}

void CountryDataHandler::CheckClearArenaData()
{
	for(int i=0;i<(int)m_lstRegion.size();i++)
	{
		CountryRgnDatHandler* pRDH = GetRegionDatHandler(m_lstRegion[i]);
		if(pRDH != NULL)
		{
			pRDH->CheckClearCountryArenaData();
			pRDH->UpdateWorldArenaData();
			pRDH->UpdateHeroArenaData();
		}
	}
}

bool CountryDataHandler::LoadUserLstFromDB(string szKey,DB_C_UserList& dbLst,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbLst))
	{
		return true;
	}
	int nRet = ds_->loadUserLst(szKey,dbLst);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadCountryFromDB(string szKey,DB_C_Country& dbCountry,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbCountry))
	{
		return true;
	}
	int nRet = ds_->loadCountry(szKey,dbCountry);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadCityFromDB(string szKey,DB_C_City& dbCity,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbCity))
	{
		return true;
	}
	int nRet = ds_->loadCity(szKey,dbCity);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadResourceFromDB(string szKey,DB_C_ResourceData& dbResourceData,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbResourceData))
	{
		return true;
	}
	int nRet = ds_->loadResourceData(szKey,dbResourceData);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadGvgCityFromDB(string szKey,DB_C_GvgCity& dbCity,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbCity))
	{
		return true;
	}
	int nRet = ds_->loadGvgCity(szKey,dbCity);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadWarGameFromDB(string szKey,DB_C_WarGameData& dbWarGameData,bool& bError)
{
    bError = false;
    string strInfo;
    if(LoadDBDataFromMemCache(szKey,dbWarGameData))
    {
        return true;
    }
    int nRet = ds_->loadWarGameData(szKey,dbWarGameData);
    if(nRet == -1)
    {//连接数据库失败，直接关闭
        bError = true;
        printf("CONNECT DB ERROR!!!\r\n");
        exit(0);
    }
    if(nRet == 0)
        return true;
    else
        return false;
}

bool CountryDataHandler::LoadBossBtlMsgFromDB(string szKey,DB_C_BossBtlMsg& dbWarBossBtlMsg,bool& bError)
{
	bError = false;
	string strInfo;
	if(LoadDBDataFromMemCache(szKey,dbWarBossBtlMsg))
	{
		return true;
	}
	int nRet = ds_->loadBossBtlMsg(szKey,dbWarBossBtlMsg);
	if(nRet == -1)
	{//连接数据库失败，直接关闭
		bError = true;
		printf("CONNECT DB ERROR!!!\r\n");
		exit(0);
	}
	if(nRet == 0)
		return true;
	else
		return false;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_Country& dbCountry)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbCountry.ParseFromString(infostr);
	}
	return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_GuildList& dbGuildLst)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbGuildLst.ParseFromString(infostr);
	}
	return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_UserList& dbUserLst)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbUserLst.ParseFromString(infostr);
	}
	return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_ResourceData& dbResourceData)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbResourceData.ParseFromString(infostr);
	}
	return hasInfo;
}


bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_City& dbCity)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbCity.ParseFromString(infostr);
	}
	return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_GvgCity& dbCity)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbCity.ParseFromString(infostr);
	}
	return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string & key,DB_C_WarGameData& dbWarGameData)
{
    string infostr("");
    bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
    if(hasInfo)
    {
        return dbWarGameData.ParseFromString(infostr);
    }
    return hasInfo;
}

bool CountryDataHandler::LoadDBDataFromMemCache(const string& key,DB_C_BossBtlMsg& dbWarBossBtlMsg)
{
	string infostr("");
	bool hasInfo = MemCacheServerHandler::GetMemDBInfo( key, infostr);
	if(hasInfo)
	{
		return dbWarBossBtlMsg.ParseFromString(infostr);
	}
	return hasInfo;
}

void CountryDataHandler::markUserLstDirty(DB_C_UserList* pDBUserLst)
{
	if(pDBUserLst==NULL)
		return;
	int nKey = pDBUserLst->key();
	map<int,DB_C_UserList*>::iterator iter = m_mapDrtyUserLst.find(nKey);
	if(iter == m_mapDrtyUserLst.end())
	{
		pDBUserLst->set_ltsaverevision(revision_);
		m_mapDrtyUserLst.insert(make_pair(nKey, pDBUserLst));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int,DB_C_UserList*> & mem_user_list = MemCacheServerHandler::GetInst()->GetUserList();
		map<int, DB_C_UserList*>::iterator iter = mem_user_list.find(nKey);
		if (iter == mem_user_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBUserLst,revision_);
		}
	}
}


void CountryDataHandler::markGuildLstDirty(DB_C_GuildList* pDBGuildLst)
{
	if(pDBGuildLst==NULL)
		return;
	int nKey = pDBGuildLst->key();
	map<int,DB_C_GuildList*>::iterator iter = m_mapDrtyGuildLst.find(nKey);
	if(iter == m_mapDrtyGuildLst.end())
	{
		pDBGuildLst->set_ltsaverevision(revision_);
		m_mapDrtyGuildLst.insert(make_pair(nKey, pDBGuildLst));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int,DB_C_GuildList*> & mem_user_list = MemCacheServerHandler::GetInst()->GetGuildList();
		map<int, DB_C_GuildList*>::iterator iter = mem_user_list.find(nKey);
		if (iter == mem_user_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBGuildLst,revision_);
		}
	}
}

void CountryDataHandler::markCountryDirty(CCountry* pCountry)
{
	if(pCountry==NULL)
		return;
	int nKey = pCountry->GetKey();

	map<int, CCountry*>::iterator iter = m_mapDirtyCountry.find(nKey);
	if(iter == m_mapDirtyCountry.end())
	{
		pCountry->m_dbCountry.set_ltsaverevision(revision_);
		m_mapDirtyCountry.insert(make_pair(nKey, pCountry));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CCountry*> & mem_country_list = MemCacheServerHandler::GetInst()->GetCountryList();
		map<int, CCountry*>::iterator iter = mem_country_list.find(nKey);
		if (iter == mem_country_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCountry,revision_);
		}
	}
}

void CountryDataHandler::markCityDirty(CCity* pCity)
{
	if(pCity==NULL)
		return;
	int nKey = pCity->GetKey();
	if(nKey<0)
		return;
	map<int, CCity*>::iterator iter = m_mapDirtyCity.find(nKey);
	if(iter == m_mapDirtyCity.end())
	{
		pCity->m_dbCity.set_ltsaverevision(revision_);
		m_mapDirtyCity.insert(make_pair(nKey, pCity));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CCity*> & mem_city_list = MemCacheServerHandler::GetInst()->GetCityList();
		map<int, CCity*>::iterator iter = mem_city_list.find(nKey);
		if (iter == mem_city_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCity,revision_);
		}
	}
}

void CountryDataHandler::markResourceDataDirty(DB_C_ResourceData* pDBResourceData)
{
	if(pDBResourceData==NULL)
		return;
	int nKey = pDBResourceData->key();
	map<int,DB_C_ResourceData*>::iterator iter = m_mapDrtyResourceLst.find(nKey);
	if(iter == m_mapDrtyResourceLst.end())
	{
		pDBResourceData->set_ltsaverevision(revision_);
		m_mapDrtyResourceLst.insert(make_pair(nKey, pDBResourceData));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int,DB_C_ResourceData*> & mem_resource_list = MemCacheServerHandler::GetInst()->GetResourceList();
		map<int, DB_C_ResourceData*>::iterator iter = mem_resource_list.find(nKey);
		if (iter == mem_resource_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBResourceData,revision_);
		}
	}
}

void CountryDataHandler::markGvgCityDirty(CGvgCity* pCity)
{
	if(pCity==NULL)
		return;

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}
	int nKey = pDBGvgCity->key();
	if(nKey<0)
		return;
	map<int, CGvgCity*>::iterator iter = m_mapDirtyGvgCity.find(nKey);
	if(iter == m_mapDirtyGvgCity.end())
	{
		pDBGvgCity->set_ltsaverevision(revision_);
		m_mapDirtyGvgCity.insert(make_pair(nKey, pCity));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CGvgCity*> & mem_city_list = MemCacheServerHandler::GetInst()->GetGvgCityList();
		map<int, CGvgCity*>::iterator iter = mem_city_list.find(nKey);
		if (iter == mem_city_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCity,revision_);
		}
	}
}

void CountryDataHandler::saveWarGameSiteData(DB_C_WarGameData * pDBWarGameData)
{
    if(pDBWarGameData==NULL||pDBWarGameData->key()<0)
        return;
    int nKey = pDBWarGameData->key();
    if(nKey<0)
        return;
    if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
    {
        map<int, DB_C_WarGameData*> & mem_wargame_list = MemCacheServerHandler::GetInst()->GetWarGameList();
        map<int, DB_C_WarGameData*>::iterator iter = mem_wargame_list.find(nKey);
        if (iter == mem_wargame_list.end())
        {
            MemCacheServerHandler::SafePushMemDB(nKey,pDBWarGameData,revision_);
        }
    }

    ds_->safeSave(this, nKey , pDBWarGameData);
}

void CountryDataHandler::markWarGameDataDirty(DB_C_WarGameData *pDBWarGameData)
{
    if(pDBWarGameData==NULL)
        return;
    int nKey = pDBWarGameData->key();
    map<int,DB_C_WarGameData*>::iterator iter = m_mapDirtyWarGameLst.find(nKey);
    if(iter == m_mapDirtyWarGameLst.end())
    {
        pDBWarGameData->set_ltsaverevision(revision_);
        m_mapDirtyWarGameLst.insert(make_pair(nKey, pDBWarGameData));
    }

    if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
    {
        map<int,DB_C_WarGameData*> & mem_wargame_list = MemCacheServerHandler::GetInst()->GetWarGameList();
        map<int, DB_C_WarGameData*>::iterator iter = mem_wargame_list.find(nKey);
        if (iter == mem_wargame_list.end())
        {
            MemCacheServerHandler::SafePushMemDB(nKey,pDBWarGameData,revision_);
        }
    }
}

void CountryDataHandler::saveBossBtlMsgData(DB_C_BossBtlMsg * pDBBossBtlMsg)
{
	if(pDBBossBtlMsg==NULL||pDBBossBtlMsg->key()<0)
		return;
	int nKey = pDBBossBtlMsg->key();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, DB_C_BossBtlMsg*> & mem_bossbtlmsg_list = MemCacheServerHandler::GetInst()->GetBossBtlMsgList();
		map<int, DB_C_BossBtlMsg*>::iterator iter = mem_bossbtlmsg_list.find(nKey);
		if (iter == mem_bossbtlmsg_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBBossBtlMsg,revision_);
		}
	}

	ds_->safeSave(this, nKey , pDBBossBtlMsg);
}

void CountryDataHandler::markBossBtlMsgDataDirty(DB_C_BossBtlMsg *pDBBossBtlMsg)
{
	if(pDBBossBtlMsg==NULL)
		return;
	int nKey = pDBBossBtlMsg->key();
	map<int,DB_C_BossBtlMsg*>::iterator iter = m_mapDirtyBossBtlLst.find(nKey);
	if(iter == m_mapDirtyBossBtlLst.end())
	{
		pDBBossBtlMsg->set_ltsaverevision(revision_);
		m_mapDirtyBossBtlLst.insert(make_pair(nKey, pDBBossBtlMsg));
	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int,DB_C_BossBtlMsg*> & mem_bossbtlmsg_list = MemCacheServerHandler::GetInst()->GetBossBtlMsgList();
		map<int, DB_C_BossBtlMsg*>::iterator iter = mem_bossbtlmsg_list.find(nKey);
		if (iter == mem_bossbtlmsg_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBBossBtlMsg,revision_);
		}
	}
}

void CountryDataHandler::saveAllDBData(bool urgent)
{
    if (!urgent && revision_ < timeUpdate + 10 * 1000)
    {
        return;
    }
	ds_->safeSaveAll(revision_, this, urgent);
	time_t ltNow = revision_/1000;
	tm date = *localtime(&ltNow);

    timeUpdate = revision_;
}



void CountryDataHandler::saveCountry(CCountry* pCountry)
{
	if(pCountry==NULL||pCountry->GetKey()<0)
		return;
	int nKey = pCountry->GetKey();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CCountry*> & mem_country_list = MemCacheServerHandler::GetInst()->GetCountryList();
		map<int, CCountry*>::iterator iter = mem_country_list.find(nKey);
		if (iter == mem_country_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCountry,revision_);
		}
	}

	ds_->safeSave(this, pCountry->GetKey(),pCountry->GetDBCountry());
}

void CountryDataHandler::saveCity(CCity* pCity)
{
	if(pCity==NULL||pCity->GetKey()<0)
		return;
	int nKey = pCity->GetKey();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CCity*> & mem_city_list = MemCacheServerHandler::GetInst()->GetCityList();
		map<int, CCity*>::iterator iter = mem_city_list.find(nKey);
		if (iter == mem_city_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCity,revision_);
		}
	}
	ds_->safeSave(this, pCity->GetKey(),pCity->GetDBCity());
}
void CountryDataHandler::saveGuildLst(DB_C_GuildList* pDBGuildLst)
{
	if(pDBGuildLst==NULL||pDBGuildLst->key()<0)
		return;
	int nKey = pDBGuildLst->key();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CCountry*> & mem_country_list = MemCacheServerHandler::GetInst()->GetCountryList();
		map<int, CCountry*>::iterator iter = mem_country_list.find(nKey);
		if (iter == mem_country_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBGuildLst,revision_);
		}
	}

	ds_->safeSave(this, nKey , pDBGuildLst);
}

void CountryDataHandler::saveResourceData(DB_C_ResourceData* pDBResourceData)
{
	if(pDBResourceData==NULL||pDBResourceData->key()<0)
		return;
	int nKey = pDBResourceData->key();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, DB_C_ResourceData*> & mem_resource_list = MemCacheServerHandler::GetInst()->GetResourceList();
		map<int, DB_C_ResourceData*>::iterator iter = mem_resource_list.find(nKey);
		if (iter == mem_resource_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pDBResourceData,revision_);
		}
	}

	ds_->safeSave(this, nKey , pDBResourceData);
}

void CountryDataHandler::saveGvgCity(CGvgCity* pCity)
{
	if(pCity==NULL)
		return;
	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL || pDBGvgCity->key() < 0 )
	{
		return;
	}
	int nKey = pDBGvgCity->key();
	if(nKey<0)
		return;
	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int, CGvgCity*> & mem_city_list = MemCacheServerHandler::GetInst()->GetGvgCityList();
		map<int, CGvgCity*>::iterator iter = mem_city_list.find(nKey);
		if (iter == mem_city_list.end())
		{
			MemCacheServerHandler::SafePushMemDB(nKey,pCity,revision_);
		}
	}
	ds_->safeSave(this, pDBGvgCity->key(),pDBGvgCity);
}

GameWorkingStatus CountryDataHandler::setWorkingStatus(int gid, GameWorkingStatus status)
{
	GameWorkingStatus old_status = game_status_[gid];
	if(status == NORMAL)
	{
		game_status_[gid] = NORMAL;
		checkAllWorkingStatus();
	}
	else
	{
		game_status_[gid] = status;
		game_all_ready_ = false;
	}
	return old_status;
}

void CountryDataHandler::checkAllWorkingStatus()
{
	bool ready = true;
	for (size_t i = 0; i < game_status_.size(); i ++)
	{
		if (game_status_[i] != NORMAL)
		{
			ready = false;
			break;
		}
	}
	if (!game_all_ready_ && ready)
	{
		game_all_ready_ = true;
		LOG4CXX_INFO(logger_, "All game server already.");
	}
}
