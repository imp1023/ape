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
		ResourceSilosTbl_energy = 3,
		ResourceSilosTbl_slotCapacity = 12,


	};
	void			LoadInfo();
	void			Clear();
	CFG_ResourceSilos*		GetInfo(int index);
	inline int		GetTotalSize(){return m_mapResSilos.size();}
	int GetMaxNum(int sku, int HQlevel);
	int GetEnergy(int sku, int lv);


protected:
	static const char*  szConfigFile;
	map<int, CFG_ResourceSilos*>	m_mapResSilos;
};

typedef Singleton<ResourceSilosTbl> ResourceSilosTblInst;

#endif

