#pragma once

#include <map>
#include <string>
#include "../common/const_def.h"
#include "InitItemTbl.h"
using namespace std;

class DB_Player;
class DB_SocialItem;

class SocialItemManager
{
public:
	SocialItemManager();
	~SocialItemManager(void);
	
	void init(DB_Player *pDBPlayer);
	void Clear();
	void InitDB(DB_Player* pDBPlayer);
	DB_SocialItem* GetSocialItem(string sku);
	bool CostSocialItem(string sku, int cnt);
	bool AddSocialItem(string sku, int cnt);
	bool AddSocialItem(string sku, int cnt, int currentcount, int tm);
	bool RemoveSocialItem(string sku);

private:
	map<string, DB_SocialItem*> m_mSocialItems;
	DB_Player *m_pDBPlayer;
	int	m_nMaxItemId;
};
