#include "NPCInfoCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../common/json/json.h"
#include "../common/json-util.h"

#include <fstream>
#include <time.h>

const char* NPCInfoCfg::szNPCPveBattleFile		= "Config/NPC_PveBattle.dat";

using namespace std;

NPCInfoCfg::NPCInfoCfg()
{
	ClearTbl();
}

NPCInfoCfg::~NPCInfoCfg()
{
	ClearTbl();
}

bool NPCInfoCfg::LoadTbl()
{
	ClearTbl();
	
	if (!LoadNPCPveBattleTbl())
	{
		return false;
	}

	return true;
}

bool  NPCInfoCfg::LoadNPCPveBattleTbl()
{
	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szNPCPveBattleFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		NPCPveBattleTbl* pPveBattle = new NPCPveBattleTbl();
		pPveBattle->m_nID		= fileDBC.Search_Posistion(i, E_PF_ID	)->iValue;
		pPveBattle->m_nBattleID		= fileDBC.Search_Posistion(i, E_PF_BID	)->iValue;
		pPveBattle->m_nType	= fileDBC.Search_Posistion(i, E_PF_TYPE	)->iValue;
		pPveBattle->m_nUnlockID	= fileDBC.Search_Posistion(i, E_PF_UNLOCKID	)->iValue;

		pPveBattle->m_ltStart	= GetDayTimeByStr(fileDBC.Search_Posistion(i,E_PF_STARTDAY)->pString
			,fileDBC.Search_Posistion(i,E_PF_STARTTIME)->pString);
		pPveBattle->m_ltEnd	= GetDayTimeByStr(fileDBC.Search_Posistion(i,E_PF_ENDDAY)->pString
			,fileDBC.Search_Posistion(i,E_PF_ENDTIME)->pString);

		pPveBattle->m_nCDTime		= fileDBC.Search_Posistion(i, E_PF_CDTIME	)->iValue;
		pPveBattle->m_nRewardID		= fileDBC.Search_Posistion(i, E_PF_REWARDID	)->iValue;

		pPveBattle->m_nSpecialRewardID		= fileDBC.Search_Posistion(i, E_PF_SPECIALREWARDID	)->iValue;
		pPveBattle->m_nSpecialKey		= fileDBC.Search_Posistion(i, E_PF_SPECIALKEY	)->iValue;
		pPveBattle->m_nNpcID		= fileDBC.Search_Posistion(i, E_PF_NPCID	)->iValue;
		pPveBattle->m_nPveID		= fileDBC.Search_Posistion(i, E_PF_PVEID	)->iValue;
		pPveBattle->m_nChangjingType		= fileDBC.Search_Posistion(i, E_PF_CHANGJINGTYPE	)->iValue;

		pPveBattle->m_nCount		= fileDBC.Search_Posistion(i, E_PF_COUNT	)->iValue;
		pPveBattle->m_nMetal		= fileDBC.Search_Posistion(i, E_PF_METAL	)->iValue;
		pPveBattle->m_nOil		= fileDBC.Search_Posistion(i, E_PF_OIL	)->iValue;
		pPveBattle->m_nWaitTime		= fileDBC.Search_Posistion(i, E_PF_WAITTIME	)->iValue;
		pPveBattle->m_nCredit		= fileDBC.Search_Posistion(i, E_PF_CREDIT	)->iValue;

		pPveBattle->m_nAdExp		= fileDBC.Search_Posistion(i, E_PF_ADEXP	)->iValue;
		pPveBattle->m_nTime			= fileDBC.Search_Posistion(i, E_PF_TIME	)->iValue;
		pPveBattle->m_nPopulation	= fileDBC.Search_Posistion(i, E_PF_POPULATION	)->iValue;

		string strTmp = ",";
		strTmp = fileDBC.Search_Posistion(i, E_PF_BATTLEVALUE )->pString;
		if(strTmp.size() > 0)
		{
			Token2Data(strTmp,pPveBattle->m_nBattleValue);
		}

		pPveBattle->m_nJunling		= fileDBC.Search_Posistion(i, E_PF_JUNLING	)->iValue;
		pPveBattle->m_nMinusCredit	= fileDBC.Search_Posistion(i, E_PF_MINUS_CREDIT	)->iValue;

		m_mapPveBattleTbl[pPveBattle->m_nID] = pPveBattle;
	}
	printf("Load %s cnt:%d\n", szNPCPveBattleFile, nRow);
	return true;
}

void NPCInfoCfg::ClearTbl()
{
	map<int,NpcCfgJson*>::iterator iter;
	for(iter = m_mapNPC.begin(); iter!= m_mapNPC.end();iter++)
	{
		delete iter->second;
	}
	m_mapNPC.clear();

	map<int,NPCPveBattleTbl*>::iterator itBt;
	for(itBt = m_mapPveBattleTbl.begin();itBt!=m_mapPveBattleTbl.end();itBt++)
	{
		itBt->second->m_nBattleValue.clear();
		delete itBt->second;
	}
	m_mapPveBattleTbl.clear();
}

NpcCfgJson* NPCInfoCfg::GetNPCInfo(int nID)
{
	map<int,NpcCfgJson*>::iterator iter;
	iter = m_mapNPC.find(nID);
	if(iter != m_mapNPC.end())
	{
		return iter->second;
	}
	string strInfo;
	char szFile[256];
	sprintf(szFile,"Config/NPCConfig/npc_config_%d.json",nID);
	std::fstream fs(szFile,std::ios::in|std::ios::binary);
	if(!fs)
		return NULL;
	char szBuf[1024];
	memset(szBuf,0,1024);
	while(fs.read(szBuf,1023))
	{
		strInfo+=szBuf;
		memset(szBuf,0,1024);
	}
	strInfo+=szBuf;
	//²éÕÒÎÄ¼þ
	bool bSuc=false;
	Json::Value jsNPC = parseJsonStr(strInfo,bSuc);
	if(!bSuc)
		return false;
	NpcCfgJson* pJsonNpc = new NpcCfgJson();
	FillJsonNpc(nID,jsNPC,pJsonNpc);
	m_mapNPC[nID] = pJsonNpc;
	return pJsonNpc;
}

int64 NPCInfoCfg::GetJsonAsInt64(const Json::Value& jsNPC,const char* szType)
{
	const Json::Value& key = jsNPC[szType];
	if(!key.isNull())
	{
		int64 nKey = (int64)key.asDouble();
		return nKey;
	}
	else
	{
		return 0;
	}
}

int NPCInfoCfg::GetJsonAsInt(const Json::Value& jsNPC,const char* szType)
{
	const Json::Value& key = jsNPC[szType];
	if(!key.isNull())
	{
		if(key.isInt())
			return key.asInt(); 
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

string NPCInfoCfg::GetJsonAsStr(const Json::Value& jsNPC,const char* szType)
{
	const Json::Value& key = jsNPC[szType];
	if(!key.isNull())
	{
		return key.asString(); 
	}
	else
	{
		return "";
	}
}

void NPCInfoCfg::FillJsonNpc(int nID,const Json::Value& jsNPC,NpcCfgJson* pDBNpc)
{
	char szTmp[256];
	string strRet;
	const Json::Value& jsItems = jsNPC["items"];
	if(!jsItems.isNull())
	{
		FillBuildingData(jsItems, pDBNpc->items);
	}
	const Json::Value& jsBunkers = jsNPC["bunkers"];
	if(!jsBunkers.isNull())
	{
		FillBunkerData(jsBunkers, pDBNpc->bunkers);
	}
	const Json::Value& jsGameUnits = jsNPC["gameUnits"];
	if(!jsGameUnits.isNull())
	{
		FillGameUnitData(jsGameUnits, pDBNpc->gameUnits);
	}
}

void NPCInfoCfg::FillBuildingData(const Json::Value& jsBld,vector<BuildingItemJson> &ItemJson)
{
	if(jsBld.isArray())
	{
		int nCnt = jsBld.size();
		for(int i=0;i<nCnt;i++)
		{
			BuildingItemJson bij;
			const Json::Value& subData = jsBld[i];
			bij.sid = subData[(UINT)0].asInt();
			bij.skuId = subData[(UINT)1].asInt();
			bij.id = subData[(UINT)2].asInt();
			bij.upgradeId = subData[(UINT)3].asInt();
			bij.type = subData[(UINT)4].asInt();
			bij.state = subData[(UINT)5].asInt();
			bij.x = subData[(UINT)6].asInt();
			bij.y = subData[(UINT)7].asInt();
			bij.energy = subData[(UINT)8].asInt();
			bij.energyPercent = subData[(UINT)9].asInt();
			ItemJson.push_back(bij);
		}
	}
}

void NPCInfoCfg::FillBunkerData(const Json::Value& jsBld,vector<BunkerJson>& bunkerJson)
{
	if(jsBld.isArray())
	{
		int nCnt = jsBld.size();
		for(int i=0;i<nCnt;i++)
		{
			BunkerJson bij;
			const Json::Value& subData = jsBld[i];
			bij.skuId = subData[(UINT)0].asInt();
			bij.num = subData[(UINT)1].asInt();
			bunkerJson.push_back(bij);
		}
	}
}

void NPCInfoCfg::FillGameUnitData(const Json::Value& jsBld,vector<GameUnitJson> &gameUnitJson)
{
	if(jsBld.isArray())
	{
		int nCnt = jsBld.size();
		for(int i=0;i<nCnt;i++)
		{
			GameUnitJson bij;
			const Json::Value& subData = jsBld[i];
			bij.skuId = subData[(UINT)0].asInt();
			bij.upgradeId = subData[(UINT)1].asInt();
			gameUnitJson.push_back(bij);
		}
	}
}

const NPCPveBattleTbl* NPCInfoCfg::GetNPCPveBattleTbl(int nID)
{
	map<int,NPCPveBattleTbl*>::iterator iter;
	iter = m_mapPveBattleTbl.find(nID);
	if(iter!=m_mapPveBattleTbl.end())
		return iter->second;
	return NULL;
}

const NPCPveBattleTbl* NPCInfoCfg::GetNPCPveBattleTblByNum(int nNum)
{
	if ( nNum >= m_mapPveBattleTbl.size() )
	{
		return NULL;
	}

	map<int,NPCPveBattleTbl*>::iterator iter = m_mapPveBattleTbl.begin();

	for ( int i = 0; i < nNum; i++ )
	{
		iter++;
	}

	return iter->second;
}

const NPCPveBattleTbl* NPCInfoCfg::GetNPCPveBattleTblByBase(int nBaseID)
{
	map<int,NPCPveBattleTbl*>::iterator iter = m_mapPveBattleTbl.begin();

	for ( int i = 0; i < m_mapPveBattleTbl.size(); i++ )
	{
		NPCPveBattleTbl* pTbl = iter->second;
		if ( pTbl == NULL )
		{
			iter++;
			continue;
		}

		if ( pTbl->m_nNpcID == nBaseID )
		{
			return pTbl;
		}
		iter++;
	}

	return NULL;
}