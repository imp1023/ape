#include "CollectablesTbl.h"
#include "../common/DBC.h"
const char* CollectablesTbl::szConfigFile = "Config/collectables.dat";

CollectablesTbl::CollectablesTbl(void)
{
}

CollectablesTbl::~CollectablesTbl(void)
{
	Clear();
}

void CollectablesTbl::LoadInfo()
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
		CFG_Collectables* pItem = new CFG_Collectables;
		pItem->id = fileDBC.Search_Posistion(i, 0)->iValue;
		pItem->sku = fileDBC.Search_Posistion(i, 1)->pString;
		pItem->name = fileDBC.Search_Posistion(i, 2)->pString;
		pItem->itemSkuList = fileDBC.Search_Posistion(i, 3)->pString;
		pItem->reward = fileDBC.Search_Posistion(i, 4)->pString;


		map<string, CFG_Collectables*>::iterator iter = m_mCollectables.find(pItem->sku);
		if(iter != m_mCollectables.end()){
			return;
		}
		m_mCollectables.insert(make_pair(pItem->sku, pItem));
		
	}
	printf("Load %s cnt:%d\n", CollectablesTbl::szConfigFile, nRow);
}

void CollectablesTbl::Clear()
{
	for (map<string, CFG_Collectables*>::iterator iter = m_mCollectables.begin(); iter != m_mCollectables.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mCollectables.clear();
}

CFG_Collectables* CollectablesTbl::GetItem(string sku)
{
	map<string, CFG_Collectables*>::iterator iter = m_mCollectables.find(sku);
	if(iter != m_mCollectables.end()){
		return iter->second;
	}
	return NULL;
}
