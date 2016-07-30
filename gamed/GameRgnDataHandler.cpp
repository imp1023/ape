#define _CRT_SECURE_NO_WARNINGS
#include "../gamed/MemCacheServerHandler.h"
#include "../event/DataHandler.h"

#include "GameDataHandler.h"
#include "GameRgnDataHandler.h"
#include "GameDataSaver.h"
#include "GameEventHandler.h"

#include <algorithm>

#include <math.h>
#include "../logic/dbinterface.pb.h"
#include "../common/Clock.h"
#include "../common/distribution.h"

#include "../common/SysLog.h"
#include "../common/Msg2QQ.h"
#include "../logic/Player.h"
#include "../logic/GameSrvCfg.h"

using namespace std;

GameRgnDataHandler::GameRgnDataHandler(GameDataHandler* pDH,int nRegion)
:m_pGameDataHandler(pDH)
,m_nRegion(nRegion)
{
}

GameRgnDataHandler::~GameRgnDataHandler()
{

	map<int,CLiteCountry*>::iterator itCntr;
	for(itCntr = m_mapLiteCountry.begin();itCntr!=m_mapLiteCountry.end();itCntr++)
	{
		delete itCntr->second;
	}
	m_mapLiteCountry.clear();
}

void GameRgnDataHandler::Init()
{
	for(int i=C_Start;i<C_UserCoutryEnd;i++)
	{
		CLiteCountry* pCountry = new CLiteCountry(m_nRegion,i);
		m_mapLiteCountry[i] = pCountry;
	}
	m_tRankFamoseHallTimer = 0;
	m_mapFamouseGiftMap.clear();
}

CLiteCountry* GameRgnDataHandler::GetLiteCountryInfo(int nCountry,bool bCreateIfNotExit)
{

	map<int,CLiteCountry*>::iterator iter;

	iter = m_mapLiteCountry.find(nCountry);
	if(iter != m_mapLiteCountry.end())
	{
		return iter->second;
	}
	if(bCreateIfNotExit)
	{
		CLiteCountry* pLite = new CLiteCountry(m_nRegion,nCountry);
		pLite->m_nCountryID = nCountry;
		pLite->m_nRegion    = m_nRegion;
		m_mapLiteCountry[nCountry] = pLite;
		return pLite;
	}
	return NULL;
}


int GameRgnDataHandler::GetCountryUser(int nCountry)
{
	CLiteCountry* pLite = GetLiteCountryInfo(nCountry);
	if(pLite)
	{
		return pLite->m_nUserCnt;
	}
	return 0;

}

void GameRgnDataHandler::SetCountryUser(int nCountry,int nCnt)
{
	CLiteCountry* pLite = GetLiteCountryInfo(nCountry,true);
	pLite->m_nUserCnt = nCnt;
}

void GameRgnDataHandler::AddCountryUser(int nCountry,int nCnt)
{
	CLiteCountry* pLite = GetLiteCountryInfo(nCountry,true);
	pLite->m_nUserCnt += nCnt;

}

int GameRgnDataHandler::GetMaxUserCountry()
{
	int nID = C_Start;
	int nNum = GetCountryUser(nID);
	for(int i=C_Start+1;i<C_UserCoutryEnd;i++)
	{
		int nCnt = GetCountryUser(i);
		if(nCnt>nNum)
		{
			nID = i;
			nNum = nCnt;
		}
	}
	return nID;
}
int GameRgnDataHandler::GetMinUserCountry()
{
	int nID = C_Start;
	int nNum = GetCountryUser(nID);
	for(int i=C_Start+1;i<C_UserCoutryEnd;i++)
	{
		int nCnt = GetCountryUser(i);
		if(nCnt<nNum)
		{
			nID = i;
			nNum = nCnt;
		}
	}
	return nID;
}

int GameRgnDataHandler::GetCountryLastDayRank(int nCountry)
{
	CLiteCountry* pLite = GetLiteCountryInfo(nCountry,true);
	return pLite->m_nLastDayRank;
}
int GameRgnDataHandler::GetCountryBoom(int nCountry)
{
	CLiteCountry* pLite = GetLiteCountryInfo(nCountry,true);

	return pLite->m_nBoom;
}

void GameRgnDataHandler::ResetCountryRank()
{
	map<int,CLiteCountry*>::iterator iter;
	vector<CLiteCountry*> lstCountry;
	vector<CLiteCountry*>::iterator itCt;
	for(iter = m_mapLiteCountry.begin();iter!=m_mapLiteCountry.end();iter++)
	{
		CLiteCountry* pCntr = iter->second;

		bool bInsert = false;
		for(itCt = lstCountry.begin();itCt!=lstCountry.end();itCt++)
		{
			CLiteCountry* pTmp = *itCt;
			if(pCntr->m_nBoom >= pTmp->m_nBoom)
			{
				if(itCt == lstCountry.begin())
				{
					bInsert = true;
					lstCountry.insert(itCt,pCntr);
					break;
				}
				else
				{	
					bInsert = true;
					lstCountry.insert(itCt,pCntr);
					break;
				}
			}
		}
		if(!bInsert)
		{
			lstCountry.push_back(pCntr);
		}
	}
	
	
	if(lstCountry.size()<=0)
		return;
	string strInfo;
	char szTmp[256];
	int nRank = 1;
	lstCountry[0]->m_nCurRank = nRank;
	for(int i=1;i<(int)lstCountry.size();i++)
	{
		if(lstCountry[i]->m_nBoom >= lstCountry[i-1]->m_nBoom)
		{
			lstCountry[i]->m_nCurRank = nRank;
		}	
		else
		{
			nRank++;
			lstCountry[i]->m_nCurRank = nRank;
		}
		sprintf(szTmp,"%d:%d:%d:%d   ;",lstCountry[i]->m_nCountryID,lstCountry[i]->m_nUserCnt,lstCountry[i]->m_lstCityID.size(),lstCountry[i]->m_nBoom);
		strInfo+=szTmp;
	}
	//SYS_LOG(0,LT_CNTR_Rank,0,0,m_nRegion<<strInfo);
	
}

// the rank
void GameRgnDataHandler::SetFaouseHallRank(int64 uid, int nRank )
{
	m_mapFamouseGiftMap[uid] = nRank;
}

int GameRgnDataHandler::GetFaouseHallRank(int64 uid)
{
	if (m_mapFamouseGiftMap.size() == 0)
	{
		return 0;
	}
	map<int64,int>::iterator iter = m_mapFamouseGiftMap.find(uid);
	if ( iter != m_mapFamouseGiftMap.end() )
	{
		return iter->second;
	}
	return -1;
}
