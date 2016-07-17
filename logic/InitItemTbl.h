#ifndef _CFG_INITITEMTBL_H_
#define _CFG_INITITEMTBL_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct  
{
	int sid;
	int sku;
	int type;
	int energy;
	int x;
	int y;
	int time;
}CFG_ITEM;

class InitItemTbl
{
public:
	InitItemTbl();
	~InitItemTbl();

	void			LoadInfo();
	void			Clear();
	CFG_ITEM*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapInitItem.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_ITEM*>	m_mapInitItem;
};

typedef Singleton<InitItemTbl> InitItemTblInst;

#endif

