#include "ItemSumTbl.h"
#include "../common/DBC.h"
const char* ItemSumTbl::szConfigFile = "Config/ItemSum.dat";

ItemSumTbl::ItemSumTbl(void)
{
}

ItemSumTbl::~ItemSumTbl(void)
{
	Clear();
}

void ItemSumTbl::LoadInfo()
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
		int sku = fileDBC.Search_Posistion(i, 1)->iValue;
		int sum = fileDBC.Search_Posistion(i, 2)->iValue;

		map<int, int>::iterator iter = m_mItemSum.find(sku);
		if(iter != m_mItemSum.end()){
			return;
		}
		m_mItemSum.insert(make_pair(sku, sum));
	}
	printf("Load %s cnt:%d\n", ItemSumTbl::szConfigFile, nRow);
}

void ItemSumTbl::Clear()
{
	m_mItemSum.clear();
}

int ItemSumTbl::GetSum(int sku)
{
	map<int, int>::iterator iter = m_mItemSum.find(sku);
	if(iter == m_mItemSum.end()){
		return iter->second;
	}
	return 0;
}

