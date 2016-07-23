#ifndef _CFG_RESOURCETBL_H_
#define _CFG_RESOURCETBL_H_
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
}CFG_Resource;

class ResourceTbl
{
public:
	ResourceTbl();
	~ResourceTbl();
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
		HangarTbl_incomePerMinute = 12,
	};
	void			LoadInfo();
	void			Clear();
	CFG_Resource*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapResources.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_Resource*>	m_mapResources;
};

typedef Singleton<ResourceTbl> ResourceTblInst;

#endif

