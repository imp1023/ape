#ifndef _CFG_DEFENSETBL_H_
#define _CFG_DEFENSETBL_H_
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
	int shotDamage;
	int repairTime;
}CFG_Defense;

class DefenseTbl
{
public:
	DefenseTbl();
	~DefenseTbl();
	enum{
		HangarTbl_id = 0,
		HangarTbl_sku = 1,
		HangarTbl_level = 2,
		HangarTbl_energy = 3,
		HangarTbl_energyProgressBar = 4,
		HangarTbl_maxNumPerHQLevel = 5,
		HangarTbl_constructionTime = 6,
		HangarTbl_constructionCoins = 7,
		HangarTbl_constructionMineral = 8,
		HangarTbl_scoreBuilt = 9,
		HangarTbl_scoreAttack = 10,
		HangarTbl_shootDamage = 11,
		HangarTbl_repairTime = 12,
	};
	void			LoadInfo();
	void			Clear();
	CFG_Defense*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapDefenses.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_Defense*>	m_mapDefenses;
};

typedef Singleton<DefenseTbl> DefenseTblInst;

#endif

