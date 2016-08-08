#include "ResourceSilos.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/const_def.h"
#include "../logic/HangarTbl.h"
#include "../logic/LaboratoryTbl.h"
#include "../logic/ResourcesTbl.h"
#include "../logic/DefensesTbl.h"
#include "../logic/ShipyardsTbl.h"
#include "../logic/WonderDefinitionsTbl.h"

const char* ResourceSilosTbl::szConfigFile = "Config/Resourcessilos.dat";

ResourceSilosTbl::ResourceSilosTbl(void)
{
}

ResourceSilosTbl::~ResourceSilosTbl(void)
{
	Clear();
}

void ResourceSilosTbl::LoadInfo()
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
		CFG_ResourceSilos *pItem = new CFG_ResourceSilos;
		int id = fileDBC.Search_Posistion(i, ResourceSilosTbl_id)->iValue;
		pItem->id = fileDBC.Search_Posistion(i, ResourceSilosTbl_id)->iValue;
		pItem->level = fileDBC.Search_Posistion(i, ResourceSilosTbl_level)->iValue;
		pItem->maxNumPerHQLevel = fileDBC.Search_Posistion(i, ResourceSilosTbl_maxNumPerHQLevel)->pString;
		pItem->energy =fileDBC.Search_Posistion(i, ResourceSilosTbl_energy)->iValue;
		pItem->slotCapacity =fileDBC.Search_Posistion(i, ResourceSilosTbl_slotCapacity)->iValue;
		


		map<int,CFG_ResourceSilos*>::iterator iter = m_mapResSilos.find(id);
		if(iter != m_mapResSilos.end()){
 			return ;
 		}
		m_mapResSilos.insert(make_pair(id, pItem));
	}
	
	printf("Load %s cnt:%d\n", ResourceSilosTbl::szConfigFile, nRow);
}

void ResourceSilosTbl::Clear()
{
	for (map<int,CFG_ResourceSilos*>::iterator iter = m_mapResSilos.begin(); iter != m_mapResSilos.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapResSilos.clear();

}


CFG_ResourceSilos* ResourceSilosTbl::GetInfo(int nIndex)
{
	map<int, CFG_ResourceSilos*>::iterator iterFind = m_mapResSilos.find(nIndex);
	if (iterFind != m_mapResSilos.end())
	{
		return iterFind->second;
	}
	return NULL;
}


int ResourceSilosTbl::GetMaxNum(int sku, int HQlevel)
{
	if(HQlevel < INIT_PLAYER_HQLEVEL || HQlevel > MAX_HQ_LEVEL)
		return 0;

	if(sku >= SKU_d_001_013 && sku <= SKU_d_001_010)
	{
		return 4;//允许装饰品最大数量为4 后期补define
	}

	switch(sku)
	{
	case SKU_hangar_001_001:
		{
			CFG_Hangar* tbl =  HangarTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);
			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	case SKU_labs_observatory:
	case SKU_labs_001_001:
	case SKU_labs_002_001:
	case SKU_bunker_001_002:
	case SKU_bunker_001_001:
		{
			CFG_Laboratory* tbl =  LaboratoryTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);
			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	case SKU_rc_001_001:
	case SKU_rm_001_001:
		{
			CFG_Resource* tbl =  ResourceTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);

			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	case SKU_rs_001_001:
	case SKU_rs_002_001:
		{
			CFG_ResourceSilos* tbl =  ResourceSilosTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);
			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	case SKU_barracks_001_001:
	case SKU_mechas_001_001:
	case SKU_shipyards_001_001:
		{
			CFG_Shipyard* tbl =  ShipyardTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);
			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	case SKU_df_001_004:
	case SKU_df_001_001:
	case SKU_df_001_005:
	case SKU_df_001_003:
	case SKU_df_001_007:
	case SKU_df_001_010:
	case SKU_df_001_002:
	case SKU_df_001_006:
	case SKU_df_001_008:
	case SKU_df_001_009:
		{
			CFG_Defense* tbl =  DefenseTblInst::instance().GetInfo(MAX_NUM_COMMON_ID);
			vector<string> vec;
			static string delims = ",";
			string maxNumPerHQLevel = tbl->maxNumPerHQLevel;
			maxNumPerHQLevel.erase(0,1);
			maxNumPerHQLevel.erase(maxNumPerHQLevel.size() - 1, 1);
			tokenize(maxNumPerHQLevel, vec, delims);
			int nlv = 0;
			safe_atoi(vec[HQlevel-1], nlv);
			return nlv;
		}
	}
	return 0;
}

int ResourceSilosTbl::GetEnergy(int sku, int lv)
{
	int energy = 0;
	switch(sku)
	{
	case SKU_wonders_headquarters:
		{
			return WonderDefinitionsTblInst::instance().GetHQEnergy(lv);
		}
	case SKU_hangar_001_001:
		{
			CFG_Hangar* tbl =  HangarTblInst::instance().GetInfo(lv);
			energy = tbl->energy;
			return energy;
		}
	case SKU_labs_observatory:
	case SKU_labs_001_001:
	case SKU_labs_002_001:
	case SKU_bunker_001_002:
	case SKU_bunker_001_001:
		{
			CFG_Laboratory* tbl =  LaboratoryTblInst::instance().GetInfo(lv);
			energy = tbl->energy;
			return energy;
		}
	case SKU_rc_001_001:
	case SKU_rm_001_001:
		{
			CFG_Resource* tbl =  ResourceTblInst::instance().GetInfo(lv);
			energy = tbl->energy;
			return energy;
		}
	case SKU_rs_001_001:
	case SKU_rs_002_001:
		{
			CFG_ResourceSilos* tbl =  ResourceSilosTblInst::instance().GetInfo(lv);
			energy =  tbl->energy;
			return energy;
		}
	case SKU_barracks_001_001:
	case SKU_mechas_001_001:
	case SKU_shipyards_001_001:
		{
			CFG_Shipyard* tbl =  ShipyardTblInst::instance().GetInfo(lv);
			energy = tbl->energy;
			return energy;
		}
	case SKU_df_001_004:
	case SKU_df_001_001:
	case SKU_df_001_005:
	case SKU_df_001_003:
	case SKU_df_001_007:
	case SKU_df_001_010:
	case SKU_df_001_002:
	case SKU_df_001_006:
	case SKU_df_001_008:
	case SKU_df_001_009:
		{
			CFG_Defense* tbl =  DefenseTblInst::instance().GetInfo(lv);
			energy = tbl->energy;
			return energy;
		}
	}
	return 0;
}