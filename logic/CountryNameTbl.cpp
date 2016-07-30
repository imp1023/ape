#include "CountryNameTbl.h"
#include "../common/DBC.h"
const char* CountryNameTbl::szConfigFile = "Config/CountryId.dat";

CountryNameTbl::CountryNameTbl(void)
{
}

CountryNameTbl::~CountryNameTbl(void)
{
	Clear();
}

bool CountryNameTbl::LoadInfo()
{
	Clear();

	DBCFile fileDBC(0);
	if( fileDBC.OpenFromTXT(szConfigFile) == false)
	{
		return false;
	}

	int nRow = fileDBC.GetRecordsNum();
	if( nRow <= 0 )
	{
		return false;
	}

	for (int i = 0; i < nRow; i++)
	{
		int id = fileDBC.Search_Posistion(i, 0)->iValue;
		string name = fileDBC.Search_Posistion(i, 1)->pString;
		map<int, string>::iterator iter = m_mapCountryName.find(id);
		if(iter != m_mapCountryName.end()){
			return false;
		}
		m_mapCountryName.insert(make_pair(id, name));
	}
	
	printf("Load %s cnt:%d\n", CountryNameTbl::szConfigFile, nRow);
	return true;
}

void CountryNameTbl::Clear()
{
	m_mapCountryName.clear();
}

string CountryNameTbl::GetInfo(int nIndex)
{
	map<int, string>::iterator iterFind = m_mapCountryName.find(nIndex);
	if (iterFind != m_mapCountryName.end())
	{
		return iterFind->second;
	}
	return "";
}