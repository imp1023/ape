#include "NPCManager.h"
#include "Player.h"
#include "dbinterface.pb.h"
/*#include "BuildingInfoCfg.h"*/
#include "NPCInfoCfg.h"
#if 0
NPCManager::NPCManager(Player *pPlayer)
: m_pPlayer(pPlayer)
,m_pDBPlayer(NULL)
,m_pDBNPCList(NULL)
{
}

NPCManager::~NPCManager(void)
{
	Clear();
}

void NPCManager::Clear()
{
	m_pPlayer	= NULL;
	m_pDBPlayer	= NULL;
	m_pDBNPCList= NULL;
}

void NPCManager::InitNPCList(DB_NPCList* pDBNPCList)
{
	m_pDBNPCList = pDBNPCList;
}

void NPCManager::InitDB(DB_Player* pDBPlaper)
{
	m_pDBPlayer = pDBPlaper; 

	OldDBInfo2New();
}

void NPCManager::InitNewWmstatusFromCfg()
{
	if(m_pPlayer==NULL||m_pDBPlayer==NULL||m_pDBNPCList==NULL)
		return;
	const map<int,NPCWmstatusTbl*>& mapWS =  NPCInfoCfg::Instance().GetWmstatusTbl();
	map<int,NPCWmstatusTbl*>::const_iterator iter;
	for(iter = mapWS.begin();iter!=mapWS.end();iter++)
	{
		const NPCWmstatusTbl* pWS = iter->second;
		DB_WmStatus* pDBWS = m_pDBPlayer->add_wmstatus();
		pDBWS->set_baseid(pWS->m_nNPCID);
		pDBWS->set_destroyed(pWS->m_bDestroyed);
		pDBWS->set_factionid(pWS->m_nFactionid);
		pDBWS->set_level(pWS->m_nLevel);
	}
}

DB_Npc* NPCManager::InitNewNPCFromCfg(int64 nNPCID)
{
	DB_Npc* pCfgNpc= NPCInfoCfg::Instance().GetNPCInfo((int)nNPCID);
	if(pCfgNpc==NULL)
		return false;
	//DB_Npc* pDBNPC = m_pDBPlayer->add_npc();
	int nSize = m_pDBNPCList->npc_size();
	DB_Npc* pDBNPC = m_pDBNPCList->add_npc();
	pDBNPC->CopyFrom(*pCfgNpc);
	return pDBNPC;
}

void NPCManager::OldDBInfo2New()
{
	if(m_pDBNPCList==NULL)
		return;
	for(int i=0;i<m_pDBNPCList->npc_size();i++)
	{
		DB_Npc* pDBNPC = m_pDBNPCList->mutable_npc(i);
		if(pDBNPC==NULL)
			continue;
		DB_Bases* pDBBase = pDBNPC->mutable_base();
		DB_Npc* pCfgNpc= NPCInfoCfg::Instance().GetNPCInfo((int)pDBNPC->userid());
		if(pCfgNpc)
		{
			for(int i=0;i<2&&i<pCfgNpc->resmax_size()&&i<pDBNPC->res_size();i++)
			{
				if(pCfgNpc->resmax(i)>0)
				{
					pDBNPC->set_res(i,pCfgNpc->resmax(i)/2);
				}
			}
		}
		//for(int i=0;i<2&&i<pDBNPC->res_size()&&i<pDBNPC->resmax_size();i++)
		//{
		//	if(pDBNPC->resmax(i)>0)
		//	{
		//		pDBNPC->set_res(i,pDBNPC->resmax(i)/2);
		//	}
		//}
		int nMineIdx = 0;
		for(int j=0;j<pDBBase->building_size();j++)
		{
			DB_Building* pDBBld = pDBBase->mutable_building(j);
			if(pDBBld==NULL)
				continue;
			int nType = pDBBld->type();
			if(nType==BLD_Mine)
			{
				DB_Mine* pDBMine = pDBBase->add_mine();			
				pDBMine->set_id(MINE_ID_START+nMineIdx);
				nMineIdx++;
				pDBMine->set_level(pDBBld->level());
				pDBMine->set_x(pDBBld->x());
				pDBMine->set_y(pDBBld->y());
				if(pDBBld->f())
				{
					pDBMine->set_state(0);
				}
				else
				{
					pDBMine->set_state(3);
				}
				
				pDBBld->Clear();
				pDBBld->set_type(0);
				pDBBld->set_id(0);
				continue;
			}

			const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(nType);
			if(pBldTbl==NULL&&nType!=BLD_Mine)
				continue;
			int nLV = pDBBld->level();
			if(nLV<=0)
			{
				nLV = 1;
				pDBBld->set_level(nLV);
			}
			else if(nLV>pBldTbl->m_nMaxLV)
			{
				nLV = pBldTbl->m_nMaxLV;
				pDBBld->set_level(nLV);
			}
			pDBBld->set_id(BUILDING_ID_START+j);
			if(!pDBBld->has_hp())
			{
				pDBBld->set_hp(pBldTbl->m_nHP[nLV]);
				switch(pBldTbl->m_nType)
				{
				case BLD_Metal_Factory:
				case BLD_Oil_Pump:
					{
						pDBBld->set_st(pBldTbl->m_nCapacity[nLV]);
					}
					break;
				}
			}
			pDBBld->clear_mq();
		}
	}
}

DB_Npc* NPCManager::GetDBNPC(int64 nNPCID)
{
	bool bFind = false;
	for(int i=0;i<m_pDBNPCList->npc_size();i++)
	{
		if(m_pDBNPCList->npc(i).userid() == nNPCID)
		{
			bFind = true;
			return m_pDBNPCList->mutable_npc(i);
		}
	}

	return NULL;
}

void NPCManager::CheckNPC(DB_Npc* pDBNPC)
{
	if(pDBNPC==NULL)
		return;
	const DB_Npc* pCfgNpc= NPCInfoCfg::Instance().GetNPCInfo((int)pDBNPC->userid());
	if(pCfgNpc==NULL)
		return;
	for(int i=pDBNPC->res_size();i<4;i++)
	{
		pDBNPC->add_res(0);
	}
	for(int i=pDBNPC->resmax_size();i<4;i++)
	{
		pDBNPC->add_resmax(0);
	}
	for(int i=0;i<4;i++)
	{
		if(pCfgNpc->resmax_size()>i&&pCfgNpc->resmax(i)<pDBNPC->resmax(i))
		{
			pDBNPC->set_resmax(i,pCfgNpc->resmax(i));
		}
		if(pCfgNpc->res_size()>i&&pCfgNpc->res(i)<pDBNPC->res(i))
		{
			pDBNPC->set_res(i,pCfgNpc->res(i));
		}
	}
	DB_Bases* pDBBase = pDBNPC->mutable_base();
	for(int i=0;i<pDBBase->building_size();i++)
	{
		DB_Building* pDBBld = pDBBase->mutable_building(i);
		if(pDBBld->st()>0)
		{
			const BldInfoTbl* pBldTbl = BuildingInfoCfg::Instance().GetBldInfoTbl(pDBBld->type());
			if(pBldTbl==NULL)
			{
				pDBBld->set_st(0);
				continue;
			}
			int nLV = pDBBld->level();
			if(nLV<=0)
				nLV = 1;
			else if(nLV>pBldTbl->m_nMaxLV)
				nLV = pBldTbl->m_nMaxLV;
			if(pDBBld->st()>pBldTbl->m_nCapacity[nLV])
			{
				pDBBld->set_st(pBldTbl->m_nCapacity[nLV]);
			}
		}
	}
}

void NPCManager::CleanNPCInDB(int nDelID)
{
	if(m_pPlayer==NULL||m_pDBPlayer==NULL ||m_pDBNPCList == NULL)
		return;
	if(nDelID>0)
	{
		for(int j=0;j<m_pDBNPCList->npc_size();j++)
		{
			DB_Npc* pDBNPC = m_pDBNPCList->mutable_npc(j);
			if(pDBNPC->userid() == nDelID)
			{
				DB_Npc* pLastdata = m_pDBNPCList->mutable_npc(m_pDBNPCList->npc_size()-1);
				pDBNPC->Swap( pLastdata);
				m_pDBNPCList->mutable_npc()->RemoveLast();
				break;
			}
		}
	}
	else
	{
		for(int i=0;i<m_pDBPlayer->wmstatus_size();i++)
		{
			DB_WmStatus* pDBWS = m_pDBPlayer->mutable_wmstatus(i);
			if(pDBWS->destroyed())
			{
				int nNPCID = pDBWS->baseid();
				for(int j=0;j<m_pDBNPCList->npc_size();j++)
				{
					DB_Npc* pDBNPC = m_pDBNPCList->mutable_npc(j);
					if(pDBNPC->userid() == nNPCID)
					{
						DB_Npc* pLastdata = m_pDBNPCList->mutable_npc(m_pDBNPCList->npc_size()-1);
						pDBNPC->Swap( pLastdata);
						m_pDBNPCList->mutable_npc()->RemoveLast();
						break;
					}
				}
			}
		}
	}

}	

#endif