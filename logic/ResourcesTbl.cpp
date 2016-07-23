#include "ResourcesTbl.h"
#include "../common/DBC.h"
const char* ResourceTbl::szConfigFile = "Config/Resources.dat";

ResourceTbl::ResourceTbl(void)
{
}

ResourceTbl::~ResourceTbl(void)
{
	Clear();
}

void ResourceTbl::LoadInfo()
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
		CFG_Resource *pItem = new CFG_Resource;
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
		pItem->incomePerMinute = fileDBC.Search_Posistion(i, HangarTbl_incomePerMinute)->iValue;

		map<int, CFG_Resource*>::iterator iter = m_mapResources.find(id);
		if(iter != m_mapResources.end()){
			return;
		}
		m_mapResources.insert(make_pair(pItem->level, pItem));
	}
	printf("Load %s cnt:%d\n", ResourceTbl::szConfigFile, nRow);
}

void ResourceTbl::Clear()
{
	for (map<int,CFG_Resource*>::iterator iter = m_mapResources.begin(); iter != m_mapResources.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapResources.clear();
}

CFG_Resource* ResourceTbl::GetInfo(int nIndex)
{
	map<int, CFG_Resource*>::iterator iterFind = m_mapResources.find(nIndex);
	if (iterFind != m_mapResources.end())
	{
		return iterFind->second;
	}
	return NULL;
}