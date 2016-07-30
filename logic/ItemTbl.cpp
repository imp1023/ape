#include "ItemTbl.h"
#include "../common/DBC.h"
const char* ItemTbl::szConfigFile = "Config/Item.dat";

ItemTbl::ItemTbl(void)
{
}

ItemTbl::~ItemTbl(void)
{
	Clear();
}

void ItemTbl::LoadInfo()
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
		CFG_Item* pItem = new CFG_Item;
		pItem->id = fileDBC.Search_Posistion(i, 0)->iValue;
		pItem->sku = fileDBC.Search_Posistion(i, 1)->pString;
		pItem->name = fileDBC.Search_Posistion(i, 2)->pString;
		pItem->price_pc = fileDBC.Search_Posistion(i, 3)->iValue;
		pItem->givenCondition = fileDBC.Search_Posistion(i, 4)->pString;
		pItem->timeToGive = fileDBC.Search_Posistion(i, 5)->iValue;
		pItem->maxAmountInventory = fileDBC.Search_Posistion(i, 6)->iValue;

		map<string, CFG_Item*>::iterator iter = m_mItem.find(pItem->sku);
		if(iter != m_mItem.end()){
			return;
		}
		m_mItem.insert(make_pair(pItem->sku, pItem));
		
		map<int, CFG_Item*>::iterator iter_id = m_mItem_id.find(pItem->id);
		if(iter_id != m_mItem_id.end()){
			return;
		}
		m_mItem_id.insert(make_pair(pItem->id, pItem));
		
	}
	printf("Load %s cnt:%d\n", ItemTbl::szConfigFile, nRow);
}

void ItemTbl::Clear()
{
	for (map<string, CFG_Item*>::iterator iter = m_mItem.begin(); iter != m_mItem.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mItem.clear();
	for (map<int, CFG_Item*>::iterator iter1 = m_mItem_id.begin(); iter1 != m_mItem_id.end(); ++iter1)
	{
		if (iter1->second != NULL)
		{
			delete iter1->second;
		}
	}
	m_mItem_id.clear();
}

CFG_Item* ItemTbl::GetItem(string sku)
{
	map<string, CFG_Item*>::iterator iter = m_mItem.find(sku);
	if(iter != m_mItem.end()){
		return iter->second;
	}
	return NULL;
}

CFG_Item* ItemTbl::GetItem(int id)
{
	map<int, CFG_Item*>::iterator iter = m_mItem_id.find(id);
	if(iter != m_mItem_id.end()){
		return iter->second;
	}
	return NULL;
}

