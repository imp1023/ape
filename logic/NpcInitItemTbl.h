#ifndef _CFG_NPCINITITEM_H_
#define _CFG_NPCINITITEM_H_
#include <map>
#include <vector>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct {
	string npcId;
	int sid;
	int sku;
	int type;
	int upgradeId;
	int isFlipped;
	int energy;
	int x;
	int y;
	int state;
	int time;
}CFG_NPCInitItem;

class NPCInitItemTbl
{
public:
	NPCInitItemTbl();
	~NPCInitItemTbl();

	// NPC(BCD)模版数据最大个数
	enum{
		NPC_B_MAX_NPCID = 9,
		NPC_C_MAX_NPCID = 9,
		NPC_D_MAX_NPCID = 2,
	};

	void			LoadInfo();
	void			Clear();
	bool GetItems(string npc, int npcId, vector<CFG_NPCInitItem*>& items);
	typedef vector<CFG_NPCInitItem*> VecCfgNpcInitItem;
protected:
	static const char*  szConfigFile;
	map<int, VecCfgNpcInitItem>	m_mNpcBInitItems;
	map<int, VecCfgNpcInitItem>	m_mNpcCInitItems;
	map<int, VecCfgNpcInitItem>	m_mNpcDInitItems;
};

typedef Singleton<NPCInitItemTbl> NPCInitItemTblInst;
#endif

