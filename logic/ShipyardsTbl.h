#ifndef _CFG_SHIPYARDTBL_H_
#define _CFG_SHIPYARDTBL_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct  
{
	int id;
	string sku;
	int level;
	int energy;
	int energyProgressBar;
	string maxNumPerHQLevel;
	int constructionCoins;
	int constructionMineral;
	int constructionTime;
	int scoreBuilt;
	int scoreAttack;
	int repairTime;
	int incomePerMinute;
	int slotCapacity;
}CFG_Shipyard;

class ShipyardTbl
{
public:
	ShipyardTbl();
	~ShipyardTbl();
	enum{
		HangarTbl_id = 0,
		HangarTbl_sku = 1,
		HangarTbl_level = 2,
		HangarTbl_energy = 3,
		HangarTbl_energyProgressBar = 4,
		HangarTbl_maxNumPerHQLevel = 5,
		HangarTbl_constructionCoins = 6,
		HangarTbl_constructionMineral = 7,
		HangarTbl_constructionTime = 8,
		HangarTbl_scoreBuilt = 9,
		HangarTbl_scoreAttack = 10,
		HangarTbl_repairTime = 11,
		HangarTbl_SlotCapacity = 12,
	};
	void			LoadInfo();
	void			Clear();
	CFG_Shipyard*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapShipyards.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_Shipyard*>	m_mapShipyards;
};

typedef Singleton<ShipyardTbl> ShipyardTblInst;

#endif

