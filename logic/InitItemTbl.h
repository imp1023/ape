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
}CFG_InitItem;

class InitItemTbl
{
public:
	InitItemTbl();
	~InitItemTbl();

	void			LoadInfo();
	void			Clear();
	CFG_InitItem*		GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapInitItem.size();}

protected:
	static const char*  szConfigFile;
	map<int, CFG_InitItem*>	m_mapInitItem;
};

typedef Singleton<InitItemTbl> InitItemTblInst;

#endif

