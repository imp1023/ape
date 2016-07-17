#ifndef _CFG_ITEM_SUM_H_
#define _CFG_ITEM_SUM_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

class ItemSumTbl
{
public:
	ItemSumTbl();
	~ItemSumTbl();

	void			LoadInfo();
	void			Clear();
	int				GetSum(int sku);

protected:
	static const char*  szConfigFile;
	map<int, int>	m_mItemSum;
};

typedef Singleton<ItemSumTbl> ItemSumTblInst;
#endif

