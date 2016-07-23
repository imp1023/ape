#include "PlanetManager.h"
#include "Player.h"
#include "dbinterface.pb.h"
#include "ItemManager.h"

Planet::Planet(DB_Planet *pDBPlanet)
{
	m_pDBPlanet = pDBPlanet;
	m_pItemManager = new ItemManager();
}

Planet::~Planet()
{
	m_pDBPlanet = NULL;
	if(m_pItemManager){
		delete m_pItemManager;
		m_pItemManager = NULL;
	}
}

void Planet::init()
{
	m_pItemManager->init(m_pDBPlanet);
}

DB_Item* Planet::CreateItem(MsgBuildingItem *pItem)
{
	return m_pItemManager->CreateItem(pItem);
}

DB_Item* Planet::CreateItem(CFG_InitItem *pItem)
{
	return m_pItemManager->CreateItem(pItem);
}

bool Planet::DestroyItem(int nItemId)
{
	return m_pItemManager->DestroyItem(nItemId);
}

DB_Item* Planet::GetItem(int nItemId)
{
	return m_pItemManager->GetItem(nItemId);
}

bool Planet::SetHQlevel(int nLvl)
{
	if(nLvl < 0 || nLvl > MAX_HQ_LEVEL){
		return false;	
	}
	m_pDBPlanet->set_hqlevel(nLvl);
	return true;
}

int Planet::GetHQLevel()
{
	return m_pDBPlanet->hqlevel();	
}

PlanetManager::PlanetManager(Player *pPlayer)
{
	m_pDBPlayer = NULL;
	m_pPlayer = pPlayer;
	m_mPlanets.clear();
	m_MaxPlanetId = 0;
}

PlanetManager::~PlanetManager(void)
{
	Clear();
}

void PlanetManager::Clear()
{
	m_pPlayer	= NULL;
	m_pDBPlayer = NULL;
	m_mPlanets.clear();
	m_MaxPlanetId = 0;
}

void PlanetManager::InitDB(DB_Player* pDBPlaper)
{
	if(pDBPlaper){
		m_pDBPlayer = pDBPlaper;
		for (int i = 0; i < pDBPlaper->planets_size(); i++){
			DB_Planet *pDBPlanet = pDBPlaper->mutable_planets(i);
			if(pDBPlanet){
				map<int, Planet*>::iterator iter = m_mPlanets.find(pDBPlanet->id());
				if(iter == m_mPlanets.end()){
					Planet *pPlanet = new Planet(pDBPlanet);
					pPlanet->init();
					m_mPlanets.insert(make_pair(pDBPlanet->id(), pPlanet));
					if(pDBPlanet->id() > m_MaxPlanetId){
						m_MaxPlanetId = pDBPlanet->id();
					}
				}
			}
		}
	}
}

Planet* PlanetManager::GetPlanet(int nPlanetId)
{
	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end()){
		return NULL;
	}
	return iter->second;
}

Planet* PlanetManager::CreatePlanet()
{
	DB_Planet *pDBPlanet = m_pDBPlayer->add_planets();
	if(!pDBPlanet){
		return NULL;
	}
	pDBPlanet->set_id(m_MaxPlanetId + 1);

	Planet *pPlanet = new Planet(pDBPlanet);
	if(!pPlanet){
		return NULL;
	}
	pPlanet->init();
	m_mPlanets.insert(make_pair(pDBPlanet->id(), pPlanet));
	return pPlanet;
}

bool PlanetManager::DestroyPlanet(int nPlanetId)
{
	if(!m_pPlayer || !m_pDBPlayer || nPlanetId <= 0)
		return false;

	if(!m_pDBPlayer->planets_size() || !m_mPlanets.size()){
		return false;
	}

	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end() || !iter->second){
		return false;
	}
	Planet *pPlanet = iter->second;

	DB_Planet *pLastDBPlanet = m_pDBPlayer->mutable_planets(m_pDBPlayer->planets_size() - 1);
	if(pLastDBPlanet && pLastDBPlanet != pPlanet->GetDBPlanet()){
		pLastDBPlanet->Swap(pPlanet->GetDBPlanet());
	}
	m_pDBPlayer->mutable_planets()->RemoveLast();
	delete pPlanet;
	pPlanet = NULL;
	m_mPlanets.erase(iter);
	return true;
}

DB_Item* PlanetManager::CreateItem(int nPlanetId, MsgBuildingItem *pItem)
{
	if(!pItem || !m_pDBPlayer || 0 >= nPlanetId){
		return NULL;
	}

	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end() || !iter->second){
		return NULL;
	}

	Planet *pPlanet = iter->second;
	DB_Item *pDBItem = pPlanet->CreateItem(pItem);
	return pDBItem;
}

DB_Item* PlanetManager::CreateItem(int nPlanetId, CFG_InitItem *pCfgItem)
{
	if(!pCfgItem || !m_pDBPlayer || 0 >= nPlanetId){
		return NULL;
	}

	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end() || !iter->second){
		return NULL;
	}

	Planet *pPlanet = iter->second;
	DB_Item *pDBItem = pPlanet->CreateItem(pCfgItem);
	return pDBItem;
}

bool PlanetManager::DestroyItem(int nPlanetId, int nItemId)
{
	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end() || !iter->second){
		return NULL;
	}
	Planet *pPlanet = iter->second;
	return pPlanet->DestroyItem(nItemId);
}

DB_Item* PlanetManager::GetItem(int nPlanetId, int nItemId)
{
	map<int, Planet*>::iterator iter = m_mPlanets.find(nPlanetId);
	if(iter == m_mPlanets.end() || !iter->second){
		return NULL;
	}
	Planet *pPlanet = iter->second;
	return pPlanet->GetItem(nItemId);
}

