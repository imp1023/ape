#include "ShipyardsTbl.h"
#include "../common/DBC.h"
const char* ShipyardTbl::szConfigFile = "Config/Shipyards.dat";

ShipyardTbl::ShipyardTbl(void)
{
}

ShipyardTbl::~ShipyardTbl(void)
{
	Clear();
}

void ShipyardTbl::LoadInfo()
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
		CFG_Shipyard *pItem = new CFG_Shipyard;
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
		pItem->slotCapacity = fileDBC.Search_Posistion(i, HangarTbl_SlotCapacity)->iValue;

		map<int, CFG_Shipyard*>::iterator iter = m_mapShipyards.find(id);
		if(iter != m_mapShipyards.end()){
			return;
		}
		m_mapShipyards.insert(make_pair(pItem->level, pItem));
	}
	printf("Load %s cnt:%d\n", ShipyardTbl::szConfigFile, nRow);
}

void ShipyardTbl::Clear()
{
	for (map<int,CFG_Shipyard*>::iterator iter = m_mapShipyards.begin(); iter != m_mapShipyards.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapShipyards.clear();
}

CFG_Shipyard* ShipyardTbl::GetInfo(int nIndex)
{
	map<int, CFG_Shipyard*>::iterator iterFind = m_mapShipyards.find(nIndex);
	if (iterFind != m_mapShipyards.end())
	{
		return iterFind->second;
	}
	return NULL;
}