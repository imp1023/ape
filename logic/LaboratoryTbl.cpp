#include "LaboratoryTbl.h"
#include "../common/DBC.h"
const char* LaboratoryTbl::szConfigFile = "Config/Laboratories.dat";

LaboratoryTbl::LaboratoryTbl(void)
{
}

LaboratoryTbl::~LaboratoryTbl(void)
{
	Clear();
}

void LaboratoryTbl::LoadInfo()
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
		CFG_Laboratory *pItem = new CFG_Laboratory;
		int id = fileDBC.Search_Posistion(i, HangarTbl_id)->iValue;
		pItem->sku = fileDBC.Search_Posistion(i, HangarTbl_sku)->pString;
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

		map<int, CFG_Laboratory*>::iterator iter = m_mapLabs.find(id);
		if(iter != m_mapLabs.end()){
			return;
		}
		m_mapLabs.insert(make_pair(pItem->level, pItem));
	}
	printf("Load %s cnt:%d\n", LaboratoryTbl::szConfigFile, nRow);
}

void LaboratoryTbl::Clear()
{
	for (map<int,CFG_Laboratory*>::iterator iter = m_mapLabs.begin(); iter != m_mapLabs.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapLabs.clear();
}

CFG_Laboratory* LaboratoryTbl::GetInfo(int nIndex)
{
	map<int, CFG_Laboratory*>::iterator iterFind = m_mapLabs.find(nIndex);
	if (iterFind != m_mapLabs.end())
	{
		return iterFind->second;
	}
	return NULL;
}