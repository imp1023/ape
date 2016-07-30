#ifndef _CFG_COLLECTABLESTBL_H_
#define _CFG_COLLECTABLESTBL_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct  
{
	int id;
	string sku;
	string name;
	string itemSkuList;
	string reward;
}CFG_Collectables;


class CollectablesTbl
{
public:
	CollectablesTbl();
	~CollectablesTbl();

	void			LoadInfo();
	void			Clear();
	CFG_Collectables*       GetItem(string sku);

protected:
	static const char*  szConfigFile;
	map<string, CFG_Collectables*>	m_mCollectables;

};

typedef Singleton<CollectablesTbl> CollectablesTblInst;

#endif

