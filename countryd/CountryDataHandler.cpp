#define _CRT_SECURE_NO_WARNINGS

#include "../event/DataHandler.h"

#include "CountryDataHandler.h"
#include "CountryDataSaver.h"
#include "CountryEventHandler.h"
#include "MemCacheServerHandler.h"
#include <algorithm>
#include <math.h>
#include "../common/Clock.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"

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
	
	InitCountry();

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

CountryRgnDatHandler*  CountryDataHandler::GetRegionDatHandler(int nRegionID)
{
	map<int,CountryRgnDatHandler*>::iterator iter;
	iter = m_mapRegionDH.find(nRegionID);
	if(iter!=m_mapRegionDH.end())
		return iter->second;
	return NULL;
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
	return !nRet;
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

void CountryDataHandler::markCountryDirty(CCountry* pCountry)
{
	if(pCountry==NULL)
		return;
	int nKey = pCountry->GetKey();

	map<int, CCountry*>::iterator iter = m_mapDirtyCountry.find(pCountry->GetKey());
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
