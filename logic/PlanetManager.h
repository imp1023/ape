#pragma once

#include <map>
#include <string>
#include "../common/const_def.h"
#include "InitItemTbl.h"
using namespace std;

class Player;
class User;
class DB_Player;
class DB_Planet;
class DB_Item;
class ItemManager;
class MsgBuildingItem;

class Planet
{
public:
	Planet(DB_Planet* pDBPlanet);
	~Planet();
	
	void init();
	inline DB_Planet* GetDBPlanet(){return m_pDBPlanet;}
	DB_Item* CreateItem(MsgBuildingItem *pItem);
	DB_Item* CreateItem(CFG_InitItem *pItem);
	bool DestroyItem(int nItemId);
	DB_Item* GetItem(int nItemId);
	bool SetHQlevel(int nLvl);
	int	GetHQLevel();

private:
	DB_Planet *m_pDBPlanet;
	ItemManager *m_pItemManager;
};

class PlanetManager
{
public:
	PlanetManager(Player *pPlayer);
	~PlanetManager(void);

	void		InitDB(DB_Player* pDBPlaper);
	void		Clear();
	Player*		GetPlayer(){return m_pPlayer;}
	//planet
	bool		DestroyPlanet(int nPlanetId);
	Planet*		GetPlanet(int nPlanetId);
	Planet*		CreatePlanet();
	//item
	DB_Item* CreateItem(int nPlanetId, MsgBuildingItem *pItem);
	DB_Item* CreateItem(int nPlanetId, CFG_InitItem *pCfgItem);
	bool DestroyItem(int nPlanetId, int nItemId);
	DB_Item* GetItem(int nPlanetId, int nItemId);


private:
	Player *m_pPlayer;
	DB_Player *m_pDBPlayer;
	map<int, Planet*> m_mPlanets;
	//map<int, DB_Shipyard> m_mShipyard;
	int m_MaxPlanetId;
};
