#include "SocialItemManager.h"
#include "dbinterface.pb.h"
#include "../event/event.pb.h"
#include "SkuIdTbl.h"

SocialItemManager::SocialItemManager()
{
	Clear();
}

SocialItemManager::~SocialItemManager(void)
{
	Clear();
}

void SocialItemManager::Clear()
{
	m_pDBPlayer = NULL;
	m_mSocialItems.clear();
}

void SocialItemManager::init(DB_Player *pDBPlayer)
{
	if(!pDBPlayer){
		return;
	}
	m_pDBPlayer = pDBPlayer;

	for (int i = 0; i < pDBPlayer->socialitems_size(); i++){
		DB_SocialItem *pDBSocialItem = pDBPlayer->mutable_socialitems(i);
		if(pDBSocialItem){
			map<string, DB_SocialItem*>::iterator iter = m_mSocialItems.find(pDBSocialItem->sku());
			if(iter == m_mSocialItems.end()){
				m_mSocialItems.insert(make_pair(pDBSocialItem->sku(), pDBSocialItem));
			}
		}
	}
}


void SocialItemManager::InitDB(DB_Player* pDBPlayer)
{
	if(pDBPlayer)
	{
		m_pDBPlayer = pDBPlayer;
		for (int i = 0; i < pDBPlayer->socialitems_size(); i++)
		{
			DB_SocialItem *pDBSocialItem = pDBPlayer->mutable_socialitems(i);
			if(pDBSocialItem)
			{
				map<string, DB_SocialItem*>::iterator iter = m_mSocialItems.find(pDBSocialItem->sku());
				if(iter == m_mSocialItems.end())
				{
					m_mSocialItems.insert(make_pair(pDBSocialItem->sku(), pDBSocialItem));
				}
			}
		}
	}
}

DB_SocialItem* SocialItemManager::GetSocialItem(string sku)
{
	map<string, DB_SocialItem*>::iterator iter = m_mSocialItems.find(sku);
	if(iter == m_mSocialItems.end()){
		return NULL;
	}
	return iter->second;
}

bool SocialItemManager::CostSocialItem(string sku, int cnt)
{
	DB_SocialItem *pDbSocialItem = GetSocialItem(sku);
	if(!pDbSocialItem || pDbSocialItem->amount() < cnt){
		return false;
	}
	pDbSocialItem->set_amount(pDbSocialItem->amount() - cnt);
	if(pDbSocialItem->amount() == 0)
	{
		RemoveSocialItem(sku);
	}
	return true;
}

bool SocialItemManager::RemoveSocialItem(string sku)
{
	DB_SocialItem* pDBSocialItem = GetSocialItem(sku);
	if(!pDBSocialItem)
		return false;

	DB_SocialItem* pDBSI_last = m_pDBPlayer->mutable_socialitems(m_pDBPlayer->socialitems_size() - 1);
	if(pDBSI_last && pDBSI_last != pDBSocialItem)
	{
		pDBSI_last->Swap(pDBSI_last);
	}

	map<string, DB_SocialItem*>::iterator iter = m_mSocialItems.find(pDBSocialItem->sku());
	if(iter != m_mSocialItems.end()){
		m_mSocialItems.erase(iter);
	}

	m_pDBPlayer->mutable_socialitems()->RemoveLast();

	
	return true;
}

bool SocialItemManager::AddSocialItem(string sku, int cnt)
{
	DB_SocialItem *pDbSocialItem = GetSocialItem(sku);
	if(!pDbSocialItem){
		pDbSocialItem = m_pDBPlayer->add_socialitems();
		pDbSocialItem->set_sku(sku);
		pDbSocialItem->set_amount(cnt);
		m_mSocialItems.insert(make_pair(pDbSocialItem->sku(), pDbSocialItem));
	}
	else
	{
		pDbSocialItem->set_amount(pDbSocialItem->amount() + cnt);
	}
	return true;
}

bool SocialItemManager::AddSocialItem(string sku, int cnt, int currentcount, int tm)
{
	DB_SocialItem* pDBSI = GetSocialItem(sku);
	if(!pDBSI)
	{
		pDBSI = m_pDBPlayer->add_socialitems();
		pDBSI->set_sku(sku);
		pDBSI->set_counter(currentcount);
		pDBSI->set_amount(cnt);
		if(tm == 0)
		{
			pDBSI->set_timeleft(0);
			pDBSI->set_timeover(0);
		}
		else
		{
			pDBSI->set_timeleft(tm);
			pDBSI->set_timeover(time(NULL));
		}
	}
	else
	{
		if(pDBSI->timeleft() == 0 || tm <= time(NULL) - pDBSI->timeover())
		{
			pDBSI->set_amount(pDBSI->amount() + cnt);
			pDBSI->set_timeleft(tm);
			pDBSI->set_counter(currentcount);
			pDBSI->set_timeover(time(NULL));
		}
	}
	return true;
}
