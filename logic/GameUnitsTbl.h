#ifndef _CFG_GAMEUNIT_H_
#define _CFG_GAMEUNIT_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

typedef struct
{
	int sku;
	string name;
	int unlock;
}CFG_GameUnit;

class GameUnitTbl
{
public:
	GameUnitTbl();
	~GameUnitTbl();

	void			LoadInfo();
	void			Clear();
	CFG_GameUnit*	GetInfo(int id);
	inline int		GetTotalSize(){return m_mGameUnits.size();}

protected:
	static const char*  szConfigFile;
	map<int,CFG_GameUnit*>	m_mGameUnits;
};

typedef Singleton<GameUnitTbl> GameUnitTblInst;

#endif

