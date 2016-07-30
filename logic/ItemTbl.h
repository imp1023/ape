#ifndef _CFG_ITEM_H_
#define _CFG_ITEM_H_
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
	int price_pc;
	string givenCondition;
	int timeToGive;
	int maxAmountInventory;
}CFG_Item;


class ItemTbl
{
public:
	ItemTbl();
	~ItemTbl();

	void			LoadInfo();
	void			Clear();
	CFG_Item*       GetItem(string sku);
	CFG_Item*		GetItem(int id);

protected:
	static const char*  szConfigFile;
	map<string, CFG_Item*>	m_mItem;
	map<int, CFG_Item*>  m_mItem_id;
};

typedef Singleton<ItemTbl> ItemTblInst;

#endif

