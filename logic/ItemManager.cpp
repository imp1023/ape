#include "ItemManager.h"
#include "dbinterface.pb.h"
#include "../event/event.pb.h"
#include "SkuIdTbl.h"

ItemManager::ItemManager()
{
	Clear();
}

ItemManager::~ItemManager(void)
{
	Clear();
}

void ItemManager::Clear()
{
	m_pDBPlanet = NULL;
	m_mDBItems.clear();
	m_nMaxItemId = 0;
}

void ItemManager::init(DB_Planet *pDBPlanet)
{
	if(!pDBPlanet){
		return;
	}
	m_pDBPlanet = pDBPlanet;

	for (int i = 0; i < pDBPlanet->items_size(); i++){
		DB_Item *pDBItem = pDBPlanet->mutable_items(i);
		if(pDBItem){
			map<int, DB_Item*>::iterator iter = m_mDBItems.find(pDBItem->id());
			if(iter == m_mDBItems.end()){
				m_mDBItems.insert(make_pair(pDBItem->id(), pDBItem));
				if(pDBItem->id() > m_nMaxItemId){
					m_nMaxItemId = pDBItem->id();
				}
			}
		}
	}
}

DB_Item* ItemManager::CreateItem(MsgBuildingItem *pItem)
{
	DB_Item *pDBItem = m_pDBPlanet->add_items();
	if(!pDBItem){
		return NULL;
	}
	pDBItem->set_id(++m_nMaxItemId);
	pDBItem->set_x(pItem->x());
	pDBItem->set_y(pItem->y());
	pDBItem->set_sku(pItem->sku());
	pDBItem->set_state(pItem->state());
	pDBItem->set_time(pItem->time());
	pDBItem->set_energy(pItem->energy());
	pDBItem->set_upgradeid(pItem->upgradeid());
	pDBItem->set_repairing(pItem->repairing());
	pDBItem->set_incometorestore(pItem->incometorestore());
	pDBItem->set_isflipped(pItem->isflipped());
	pDBItem->set_type(pItem->type());
	pDBItem->set_sid(pItem->sid());
	pDBItem->set_updateat(time(NULL));
	m_mDBItems.insert(make_pair(pDBItem->id(), pDBItem));
	//if(type == )//µØ±¤

	return pDBItem;
}

DB_Item* ItemManager::CreateItem(CFG_InitItem *pCfgItem)
{
	DB_Item *pDBItem = m_pDBPlanet->add_items();
	if(!pDBItem){
		return NULL;
	}
	pDBItem->set_id(++m_nMaxItemId);
	pDBItem->set_sid(pCfgItem->sid);
	pDBItem->set_sku(SkuIDTblInst::instance().GetName(pCfgItem->sku));
	pDBItem->set_type(pCfgItem->type);
	pDBItem->set_upgradeid(0);
	pDBItem->set_isflipped(0);
	pDBItem->set_energypercent(100);
	pDBItem->set_energy(pCfgItem->energy);
	pDBItem->set_x(pCfgItem->x);
	pDBItem->set_y(pCfgItem->y);
	pDBItem->set_state(1);
	pDBItem->set_time(0);
	pDBItem->set_incometorestore(0);
	pDBItem->set_repairing(0);
	pDBItem->set_repairstart(0);
	pDBItem->set_updateat(time(NULL));
	m_mDBItems.insert(make_pair(pDBItem->id(), pDBItem));
	return pDBItem;
}

DB_Item* ItemManager::GetItem(int nItemId)
{
	map<int, DB_Item*>::iterator iter = m_mDBItems.find(nItemId);
	if(iter == m_mDBItems.end()){
		return NULL;
	}
	return iter->second;
}

bool ItemManager::DestroyItem(int nItemId)
{
	if(!m_pDBPlanet || nItemId < 0){
		return false;
	}
	
	if(!m_pDBPlanet->items_size() || !m_mDBItems.size()){
		return false;
	}

	map<int, DB_Item*>::iterator iter = m_mDBItems.find(nItemId);
	if(iter == m_mDBItems.end() || !iter->second){
		return false;
	}

	DB_Item* pItem = iter->second;
	DB_Item *pLastDbItem = m_pDBPlanet->mutable_items(m_pDBPlanet->items_size() - 1);
	if(pLastDbItem && pLastDbItem != pItem){
		pLastDbItem->Swap(pItem);
	}
	m_pDBPlanet->mutable_items()->RemoveLast();
	m_mDBItems.erase(iter);
	return true;	
}