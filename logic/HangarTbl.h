#ifndef _CFG_HANGARTBL_H_
#define _CFG_HANGARTBL_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct  
{
	int id;
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
	int storageAmount;
	int storageAmountProgressBar;
}CFG_Hangar;

class HangarTbl
{
public:
	HangarTbl();
	~HangarTbl();
	enum{
		HangarTbl_id = 0,
		HangarTbl_level = 1,
		HangarTbl_energy = 2,
		HangarTbl_energyProgressBar = 3,
		HangarTbl_maxNumPerHQLevel = 4,
		HangarTbl_constructionCoins = 5,
		HangarTbl_constructionMineral = 6,
		HangarTbl_constructionTime = 7,
		HangarTbl_scoreBuilt = 8,
		HangarTbl_scoreAttack = 9,
		HangarTbl_repairTime = 10,
		HangarTbl_storageAmount = 11,
		HangarTbl_storageAmountProgressBar = 12,
	};
	void			LoadInfo();
	void			Clear();
	CFG_Hangar*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapHangars.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_Hangar*>	m_mapHangars;
};

typedef Singleton<HangarTbl> HangarTblInst;

#endif

