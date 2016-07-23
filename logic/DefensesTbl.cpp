#include "DefensesTbl.h"
#include "../common/DBC.h"
const char* DefenseTbl::szConfigFile = "Config/Defenses.dat";

DefenseTbl::DefenseTbl(void)
{
}

DefenseTbl::~DefenseTbl(void)
{
	Clear();
}

void DefenseTbl::LoadInfo()
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
		CFG_Defense *pItem = new CFG_Defense;
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
		pItem->shotDamage = fileDBC.Search_Posistion(i, HangarTbl_shootDamage)->iValue;

		map<int, CFG_Defense*>::iterator iter = m_mapDefenses.find(id);
		if(iter != m_mapDefenses.end()){
			return;
		}
		m_mapDefenses.insert(make_pair(pItem->level, pItem));
	}
	printf("Load %s cnt:%d\n", DefenseTbl::szConfigFile, nRow);
}

void DefenseTbl::Clear()
{
	for (map<int,CFG_Defense*>::iterator iter = m_mapDefenses.begin(); iter != m_mapDefenses.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapDefenses.clear();
}

CFG_Defense* DefenseTbl::GetInfo(int nIndex)
{
	map<int, CFG_Defense*>::iterator iterFind = m_mapDefenses.find(nIndex);
	if (iterFind != m_mapDefenses.end())
	{
		return iterFind->second;
	}
	return NULL;
}