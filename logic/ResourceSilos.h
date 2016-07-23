#ifndef _CFG_RESOURCESILOS_H_
#define _CFG_RESOURCESILOS_H_
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
	int slotCapacity;
	int slotCapacityProgressBar;
}CFG_ResourceSilos;

class ResourceSilosTbl
{
public:
	ResourceSilosTbl();
	~ResourceSilosTbl();
	enum{
		ResourceSilosTbl_id = 0,
		ResourceSilosTbl_sku = 1,
		ResourceSilosTbl_level = 2,
		ResourceSilosTbl_energy = 3,
		ResourceSilosTbl_energyProgressBar = 4,
		ResourceSilosTbl_maxNumPerHQLevel = 5,
		ResourceSilosTbl_constructionCoins = 6,
		ResourceSilosTbl_constructionMineral = 7,
		ResourceSilosTbl_constructionTime = 8,
		ResourceSilosTbl_scoreBuilt = 9,
		ResourceSilosTbl_scoreAttack= 10,
		ResourceSilosTbl_repairTime = 11,
		ResourceSilosTbl_slotCapacity = 12,
		ResourceSilosTbl_slotCapacityProgressBar = 13,

	};
	void			LoadInfo();
	void			Clear();
	CFG_ResourceSilos*		GetInfo(string index);
	CFG_ResourceSilos*		GetInfo(int index);
	inline int		GetTotalSize(){return m_mapResSilos.size();}
	int GetMaxNum(int sku, int HQlevel);
	int GetEnergy(int sku, int lv);

protected:
	static const char*  szConfigFile;
	map<string, CFG_ResourceSilos*>	m_mapResSilos;
	map<int, CFG_ResourceSilos*>	m_mapResSilos_id;
};

typedef Singleton<ResourceSilosTbl> ResourceSilosTblInst;

#endif

