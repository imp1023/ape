#pragma once

#include <map>
#include <string>
#include "../common/const_def.h"
using namespace std;

class Player;
class User;
class DB_Player;
class DB_Npc;
class DB_Army;
class DB_Bases;
class DB_NPCList;

class NPCManager
{
#if 0
public:
	NPCManager(Player *pPlayer);
	~NPCManager(void);

	void		InitDB(DB_Player* pDBPlaper);
	void        InitNPCList(DB_NPCList* pDBNPCList);
	void		InitNewWmstatusFromCfg();
	DB_Npc*		InitNewNPCFromCfg(int64 nNPCID);
	DB_Npc*		GetDBNPC(int64 nNPCID);
	void		CheckNPC(DB_Npc* pDBNPC);

	void		Clear();
	void		OldDBInfo2New();//新老数据结构转移

	Player*		GetPlayer(){return m_pPlayer;}
public:
	void		CleanNPCInDB(int nDelID);

protected:
	Player		*m_pPlayer;
	DB_Player	*m_pDBPlayer;
	DB_NPCList  *m_pDBNPCList;
#endif
};
