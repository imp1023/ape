#include "WonderDefinitionsTbl.h"
#include "../common/DBC.h"
const char* WonderDefinitionsTbl::szConfigFile = "Config/wonderdefinitions.dat";

WonderDefinitionsTbl::WonderDefinitionsTbl(void)
{
}

WonderDefinitionsTbl::~WonderDefinitionsTbl(void)
{
	Clear();
}

void WonderDefinitionsTbl::LoadInfo()
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
		int lv = fileDBC.Search_Posistion(i, 1)->iValue;
		int energy = fileDBC.Search_Posistion(i, 2)->iValue;

		map<int, int>::iterator iter = m_mWonderDef.find(lv);
		if(iter != m_mWonderDef.end()){
			return;
		}
		m_mWonderDef.insert(make_pair(lv, energy));
	}
	printf("Load %s cnt:%d\n", WonderDefinitionsTbl::szConfigFile, nRow);
}

void WonderDefinitionsTbl::Clear()
{
	m_mWonderDef.clear();
}


int WonderDefinitionsTbl::GetHQEnergy(int lv)
{
	map<int, int>::iterator iter = m_mWonderDef.find(lv);
	if(iter != m_mWonderDef.end()){
		return iter->second;
	}
	return 0;
}

