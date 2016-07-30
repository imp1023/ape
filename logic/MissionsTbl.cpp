#include "MissionsTbl.h"
#include "../common/DBC.h"
const char* MissionsTbl::szConfigFile = "Config/Mission.dat";

MissionsTbl::MissionsTbl(void)
{
}

MissionsTbl::~MissionsTbl(void)
{
	Clear();
}

void MissionsTbl::LoadInfo()
{
	Clear();

	DBCFile fileDBC(0);
	if( fileDBC.OpenFromTXT(szConfigFile) == false)
	{
		return;
	}

	int nRow = fileDBC.GetRecordsNum();
	if( nRow <= 0 )
	{
		return;
	}

	for (int i = 0; i < nRow; i++)
	{
		CFG_Missions* pMissions = new CFG_Missions;
		pMissions->id = fileDBC.Search_Posistion(i, 0)->iValue;
		string sku = fileDBC.Search_Posistion(i, 1)->pString;
		pMissions->sku = fileDBC.Search_Posistion(i, 1)->pString;
		pMissions->size = fileDBC.Search_Posistion(i, 2)->iValue;
		pMissions->finishTime = fileDBC.Search_Posistion(i, 3)->iValue;
		pMissions->rewardCoins = fileDBC.Search_Posistion(i, 4)->iValue;
		pMissions->rewardMinerals = fileDBC.Search_Posistion(i, 5)->iValue;
		pMissions->rewardItemSku = fileDBC.Search_Posistion(i, 6)->pString;

		map<string, CFG_Missions*>::iterator iter = m_missions.find(sku);
		if(iter != m_missions.end()){
			return;
		}
		m_missions.insert(make_pair(sku, pMissions));
	}
	printf("Load %s cnt:%d\n", MissionsTbl::szConfigFile, nRow);
}

void MissionsTbl::Clear()
{
	for (map<string, CFG_Missions*>::iterator iter = m_missions.begin(); iter != m_missions.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_missions.clear();
}

CFG_Missions* MissionsTbl::GetMission(string sku)
{
	map<string, CFG_Missions*>::iterator iter = m_missions.find(sku);
	if(iter != m_missions.end()){
		return iter->second;
	}
	return NULL;
}

