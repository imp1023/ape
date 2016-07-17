#include "GameUnitsTbl.h"
#include "../common/DBC.h"
const char* GameUnitTbl::szConfigFile = "Config/GameUnits.dat";

GameUnitTbl::GameUnitTbl(void)
{
}

GameUnitTbl::~GameUnitTbl(void)
{
	Clear();
}

void GameUnitTbl::LoadInfo()
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
		CFG_GameUnit *pUnit = new CFG_GameUnit();
		int id = fileDBC.Search_Posistion(i, 0)->iValue;
		pUnit->name = fileDBC.Search_Posistion(i, 1)->pString;
		pUnit->sku = fileDBC.Search_Posistion(i, 2)->iValue;
		pUnit->unlock = fileDBC.Search_Posistion(i, 3)->iValue;
		map<int, CFG_GameUnit*>::iterator iter = m_mGameUnits.find(id);
		if(iter != m_mGameUnits.end()){
			return;
		}
		m_mGameUnits.insert(make_pair(id, pUnit));
	}
	printf("Load %s cnt:%d\n", GameUnitTbl::szConfigFile, nRow);
}

void GameUnitTbl::Clear()
{
	for(map<int, CFG_GameUnit*>::iterator iter = m_mGameUnits.begin(); iter != m_mGameUnits.end(); ++iter){
		if(iter->second){
			delete iter->second;
		}
	}
	m_mGameUnits.clear();
}

CFG_GameUnit* GameUnitTbl::GetInfo(int id)
{
	map<int, CFG_GameUnit*>::iterator iterFind = m_mGameUnits.find(id);
	if (iterFind != m_mGameUnits.end())
	{
		return iterFind->second;
	}
	return NULL;
}