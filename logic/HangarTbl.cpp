#include "HangarTbl.h"
#include "../common/DBC.h"
const char* HangarTbl::szConfigFile = "Config/Hangars.dat";

HangarTbl::HangarTbl(void)
{
}

HangarTbl::~HangarTbl(void)
{
	Clear();
}

void HangarTbl::LoadInfo()
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
		CFG_Hangar *pItem = new CFG_Hangar;
		int id = fileDBC.Search_Posistion(i, HangarTbl_id)->iValue;
		pItem->level = fileDBC.Search_Posistion(i, HangarTbl_level)->iValue;
		pItem->energy = fileDBC.Search_Posistion(i, HangarTbl_energy)->iValue;
		pItem->energyProgressBar = fileDBC.Search_Posistion(i, HangarTbl_energyProgressBar)->iValue;
		pItem->maxNumPerHQLevel = fileDBC.Search_Posistion(i, HangarTbl_maxNumPerHQLevel)->pString;
		pItem->constructionCoins = fileDBC.Search_Posistion(i, HangarTbl_constructionCoins)->iValue;
		pItem->constructionMineral = fileDBC.Search_Posistion(i, HangarTbl_constructionMineral)->iValue;
		pItem->constructionTime = fileDBC.Search_Posistion(i, HangarTbl_constructionTime)->iValue;
		pItem->scoreBuilt = fileDBC.Search_Posistion(i, HangarTbl_scoreBuilt)->iValue;
		pItem->scoreAttack = fileDBC.Search_Posistion(i, HangarTbl_scoreAttack)->iValue;
		pItem->repairTime = fileDBC.Search_Posistion(i, HangarTbl_repairTime)->iValue;
		pItem->storageAmount = fileDBC.Search_Posistion(i, HangarTbl_storageAmount)->iValue;
		pItem->storageAmountProgressBar = fileDBC.Search_Posistion(i, HangarTbl_storageAmountProgressBar)->iValue;

		map<int, CFG_Hangar*>::iterator iter = m_mapHangars.find(id);
		if(iter != m_mapHangars.end()){
			return;
		}
		m_mapHangars.insert(make_pair(pItem->level, pItem));
	}
	printf("Load %s cnt:%d\n", HangarTbl::szConfigFile, nRow);
}

void HangarTbl::Clear()
{
	for (map<int,CFG_Hangar*>::iterator iter = m_mapHangars.begin(); iter != m_mapHangars.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapHangars.clear();
}

CFG_Hangar* HangarTbl::GetInfo(int nIndex)
{
	map<int, CFG_Hangar*>::iterator iterFind = m_mapHangars.find(nIndex);
	if (iterFind != m_mapHangars.end())
	{
		return iterFind->second;
	}
	return NULL;
}