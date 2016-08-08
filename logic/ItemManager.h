#pragma once

#include <map>
#include <string>
#include "../common/const_def.h"
#include "InitItemTbl.h"
using namespace std;

class DB_Planet;
class DB_Item;
class MsgBuildingItem;

class ItemManager
{
public:
	ItemManager();
	~ItemManager(void);
	
	void init(DB_Planet *pDBPlanet);
	void Clear();

	DB_Item* GetItem(int nItemId);
	DB_Item* GetItemBySid(int sid);
	DB_Item* CreateItem(MsgBuildingItem *pItem);
	DB_Item* CreateItem(CFG_InitItem *pCfgItem);
	bool	 DestroyItem(int nItemId);


private:
	map<int, DB_Item*> m_mDBItems;
	map<int, DB_Item*> m_mDBItemsSid;
	DB_Planet* m_pDBPlanet;
	int	m_nMaxItemId;
};
