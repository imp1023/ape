#include "SkuIdTbl.h"
#include "../common/DBC.h"
const char* SkuIDTbl::szConfigFile = "Config/SkuId.dat";

SkuIDTbl::SkuIDTbl(void)
{
}

SkuIDTbl::~SkuIDTbl(void)
{
	Clear();
}

void SkuIDTbl::LoadInfo()
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
		string name = fileDBC.Search_Posistion(i, 1)->pString;
		int sku = fileDBC.Search_Posistion(i, 2)->iValue;

		map<int, string>::iterator iter = m_mSkuID.find(sku);
		if(iter != m_mSkuID.end()){
			return;
		}
		map<string, int>::iterator _iter = m_mIDSku.find(name);
		if(_iter != m_mIDSku.end()){
			return;
		}
		m_mSkuID.insert(make_pair(sku, name));
		m_mIDSku.insert(make_pair(name, sku));
	}
	printf("Load %s cnt:%d\n", SkuIDTbl::szConfigFile, nRow);
}

void SkuIDTbl::Clear()
{
	m_mSkuID.clear();
	m_mIDSku.clear();
}

string SkuIDTbl::GetName(int sku)
{
	map<int, string>::iterator iter = m_mSkuID.find(sku);
	if(iter != m_mSkuID.end()){
		return iter->second;
	}
	return "";
}

int SkuIDTbl::GetSku(string name)
{
	map<string, int>::iterator iter = m_mIDSku.find(name);
	if(iter != m_mIDSku.end()){
		return iter->second;
	}
	return 0;
}

