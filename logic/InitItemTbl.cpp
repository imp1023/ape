#include "InitItemTbl.h"
#include "../common/DBC.h"
const char* InitItemTbl::szConfigFile = "Config/InitItem.dat";

InitItemTbl::InitItemTbl(void)
{
}

InitItemTbl::~InitItemTbl(void)
{
	Clear();
}

void InitItemTbl::LoadInfo()
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
		CFG_ITEM *pItem = new CFG_ITEM;
		int id = fileDBC.Search_Posistion(i, 0)->iValue;
		pItem->sid = fileDBC.Search_Posistion(i, 1)->iValue;
		pItem->sku = fileDBC.Search_Posistion(i, 2)->iValue;
		pItem->type = fileDBC.Search_Posistion(i, 3)->iValue;
		pItem->energy = fileDBC.Search_Posistion(i, 4)->iValue;
		pItem->x = fileDBC.Search_Posistion(i, 5)->iValue;
		pItem->y = fileDBC.Search_Posistion(i, 6)->iValue;
		pItem->time = fileDBC.Search_Posistion(i, 7)->iValue;

		map<int, CFG_ITEM*>::iterator iter = m_mapInitItem.find(id);
		if(iter != m_mapInitItem.end()){
			return;
		}
		m_mapInitItem.insert(make_pair(id, pItem));
	}
	
	printf("Load %s cnt:%d\n", InitItemTbl::szConfigFile, nRow);
}

void InitItemTbl::Clear()
{
	for (map<int,CFG_ITEM*>::iterator iter = m_mapInitItem.begin(); iter != m_mapInitItem.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapInitItem.clear();
}

CFG_ITEM* InitItemTbl::GetInfo(int nIndex)
{
	map<int, CFG_ITEM*>::iterator iterFind = m_mapInitItem.find(nIndex);
	if (iterFind != m_mapInitItem.end())
	{
		return iterFind->second;
	}
	return NULL;
}