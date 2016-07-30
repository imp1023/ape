#ifndef _CFG_MISSIONS_H_
#define _CFG_MISSIONS_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct  
{
	int id;
	string sku;
	int size;
	int finishTime;
	int rewardCoins;
	int rewardMinerals;
	string rewardItemSku;
}CFG_Missions;

class MissionsTbl
{
public:
	MissionsTbl();
	~MissionsTbl();

	void			LoadInfo();
	void			Clear();
	CFG_Missions*   GetMission(string sku);

protected:
	static const char*  szConfigFile;
	map<string, CFG_Missions*>	m_missions;
};

typedef Singleton<MissionsTbl> MissionsTblInst;

#endif

