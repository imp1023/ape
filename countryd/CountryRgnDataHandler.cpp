#define _CRT_SECURE_NO_WARNINGS

#include "../event/DataHandler.h"
#include "../event/EventDefine.h"
#include "CountryRgnDataHandler.h"
#include "CountryDataHandler.h"
#include "CountryDataSaver.h"
#include "CountryEventHandler.h"
#include "MemCacheServerHandler.h"
#include "../event/event.pb.h"
#include <algorithm>

#include <math.h>
#include "CountryData.pb.h"

#include "../logic/Clock.h"
#include "../logic/CountryInfoCfg.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"
#include "../logic/ArenaNpcInitConfig.h"
#include "../logic/ArenaFightRule.h"
#include "../logic/ArenaActive.h"
#include "../logic/GuildInfoCfg.h"
#include "../logic/WorldArenaFightRule.h"
#include "../logic/WorldArenaActive.h"
#include "../logic/WorldArenaNpcInital.h"
#include "../logic/OfficerInfoCfg.h"
#include "../logic/GameConstantSetCfg.h"
#include "../logic/HeroArenaFightRule.h"
#include "../logic/HeroArenaActive.h"
#include "../logic/HeroArenaNpcInital.h"
#include "../logic/ResourceCfg.h"
#include "../logic/GvgCfg.h"
#include "../logic/WeaponRankActiveCfg.h"
#include "../logic/WarGameCfg.h"
#include "../logic/LeagueBattleCfg.h"
#include "../logic/BossBtlCfg.h"
#include "../logic/GuildRedEnvelopeCfg.h"
#include "../logic/NpcActiveCfg.h"
#include "../logic/NewRegionActivityCfg.h"


using namespace std;

const int MAX_WORLD_ARENA_ACTIVE_RECORD = 50;//最大存储记录
void CCountry::AddCity(CCity* pCity)
{
	if(pCity==NULL)
		return;
	for(int i=0;i<(int)m_lstCity.size();i++)
	{
		CCity* pTmpCity = m_lstCity[i];
		if(pTmpCity->GetCityID() == pCity->GetCityID())
			return;
	}
	m_lstCity.push_back(pCity);
	if(CountryInfoCfg::Instance().IsCapital(pCity->GetCityID()))
	{
		m_pCapital = pCity;
	}
	CheckBoom();
}

void CCountry::EraseCity(int nCityID)
{
	vector<CCity*>::iterator iter;
	for(iter = m_lstCity.begin();iter!=m_lstCity.end();iter++)
	{
		CCity* pCity = *iter;
		if(pCity->GetCityID()==nCityID)
		{
			m_lstCity.erase(iter);
			CheckBoom();
			return;
		}
	}
}

void CCountry::CheckBoom()
{
	int nBoom = 0;
	vector<CCity*>::iterator iter;
	for(iter = m_lstCity.begin();iter!=m_lstCity.end();iter++)
	{
		CCity* pCity = *iter;
		if ( pCity == NULL )
		{
			continue;
		}

		int nBoomAdd = CountryInfoCfg::Instance().GetCityBoom(pCity->GetCityID());
		nBoom += nBoomAdd;
	}
	if ( nBoom > 0 )
	{
		m_nBoom = nBoom;
	}
}

bool	 CCountry::ExChangePos(DB_C_User* pCUser,int64 nuidFight)
{
	if(pCUser == NULL)
	{
		return false;
	}

	int nPosSelf = pCUser->arenaindex();

	pCUser->set_arenaindex(-1);

	ChgArenaNpc(nPosSelf,nuidFight);

	return true;
}

bool	 CCountry::ExChangePos(DB_C_User* pCUser,DB_C_User* pCFUser)
{
	if(pCUser == NULL || pCFUser == NULL)
	{
		return false;
	}

	int nPosA = pCUser->arenaindex();
	int nPosB = pCFUser->arenaindex();

	int64 nUidA = pCUser->userid();
	int64 nUidB = pCFUser->userid();

	UpdateArenaListPos(nPosA,nUidA,nUidB);
	UpdateArenaListPos(nPosB,nUidB,nUidA);

	pCUser->set_arenaindex(nPosB);
	pCFUser->set_arenaindex(nPosA);

	return true;
}

bool	 CCountry::ExChangePos(DB_C_User* pCUser,DB_Arena_Rank_NPC* pNpc)
{
	if(pCUser == NULL || pNpc == NULL)
	{
		return false;
	}

	int nPosA = pCUser->arenaindex();
	int nPosB = pNpc->npcindex();

	int64 nUidA = pCUser->userid();
	int64 nUidB = pNpc->npcid();

	UpdateArenaListPos(nPosA,nUidA,nUidB);
	UpdateArenaListPos(nPosB,nUidB,nUidA);

	pCUser->set_arenaindex(nPosB);
	pNpc->set_npcindex(nPosA);

	return true;
}

bool	 CCountry::ExChangePos(CountryRgnDatHandler* pRDH,int64 uidSelf,int posSelf,int64 uidFight,int posFight)
{
	if(pRDH)
	{
		if(uidSelf >= 1000000000 && uidFight >= 1000000000)		//都是玩家
		{
			DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
			DB_C_User* pCUserF = pRDH->GetDBCityUser(uidFight);

			if(pCUserS && pCUserS->arenaindex() == posSelf && pCUserF && pCUserF->arenaindex() == posFight)
			{
				return ExChangePos(pCUserS,pCUserF);
			}
		}

		if(uidSelf >= 1000000000 && uidFight < 1000000000)		//玩家 NPC
		{
			DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
			DB_Arena_Rank_NPC* pNpc = GetArenaNpc((int)uidFight,posFight);

			if(pCUserS && pCUserS->arenaindex() == posSelf && pNpc)
			{
				return ExChangePos(pCUserS,pNpc);
			}
		}
	}

	return false;
}

int		 CCountry::CheckPlayerRankIndex(CountryRgnDatHandler* pRDH,int nIndex,int64 nUid)
{
	int64 nUidTmp = GetRankUidByIndex(nIndex);
	if(nUidTmp == nUid)
	{
		return nIndex;
	}

	DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
	if(pDBUser == NULL) 
	{
		SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,1);
		return -1;
	}
	if(pDBUser->countryid() != m_nCountryID)
	{
		SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,2<<m_nCountryID<<pDBUser->countryid());
		return -1;
	}
	if(pDBUser->arenaindex() > -1)
	{
		nIndex = pDBUser->arenaindex();
	}

	nUidTmp = GetRankUidByIndex(nIndex);

	if(nUid == nUidTmp)
	{
		pDBUser->set_arenaindex(nIndex);

		SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,3<<nIndex);
		return nIndex;
	}

	if(pDBUser->arenaindex() <= -1)
	{
		SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,4<<pDBUser->arenaindex());
		return -1;
	}

	if(UpdateRank() == true)
	{
		pRDH->markCountryDirty(this);
	}

	//数据错乱，重新排序
	UpdateArenaList(pRDH);

	nIndex = pDBUser->arenaindex();

	nUidTmp = GetRankUidByIndex(nIndex);

	if(nUid == nUidTmp)
	{
		SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,5<<nIndex);
		return nIndex;
	}
	SYS_LOG(nUid,LT_ARENA_COUNTRY_DEBUG,0,0,6);
	return -1;
}

int		CCountry::GetArenaPlayerNum()
{
	return m_rvecPlayArena.size();
}

DB_Arena_Rank_NPC* CCountry::GetArenaNpc(int nId,int nPos)
{
	vector<DB_Arena_Rank_NPC*>& rVec = m_rmapNpcArena[nId];
	for(int i=0;i<(int)rVec.size();i++)
	{
		DB_Arena_Rank_NPC* pNode = rVec[i];
		if(pNode && pNode->npcindex() == nPos)
		{
			return pNode;
		}
	}
	return NULL;
}

void	 CCountry::ChgArenaNpc(int nPos,int64 nuid)
{
	if(nPos < 0 || nPos >= (int)m_rvecPlayArena.size())
	{
		return;
	}

	m_rvecPlayArena[nPos] = nuid;

	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	DB_Arena_Rank_NPC* pArenaNpc = pArenaRank->add_arenaranknpc();

	pArenaNpc->set_npcindex(nPos);
	pArenaNpc->set_npcid((int)nuid);

	m_rmapNpcArena[(int)nuid].push_back(pArenaNpc);
}

void	 CCountry::UpdateArenaListPos(int nPos,int64 nuidO,int64 nuidN)
{
	if(nPos < 0 || nPos >= (int)m_rvecPlayArena.size())
	{
		return;
	}

	if(nuidO == m_rvecPlayArena[nPos])
	{
		m_rvecPlayArena[nPos] = nuidN;
	}
}

void	 CCountry::UpdateArenaList(CountryRgnDatHandler* pRDH)
{
	if(pRDH)
	{
		DB_Arena_Rank* pArenaR = m_dbCountry.mutable_arenarank();
		pRDH->UpdateCountryArenaList(m_nCountryID,pArenaR,m_rvecPlayArena);


		m_rmapNpcArena.clear();
		for(int i=0;i<pArenaR->arenaranknpc_size();i++)
		{
			DB_Arena_Rank_NPC* pNode = pArenaR->mutable_arenaranknpc(i);
			if(pNode)
			{
				m_rmapNpcArena[pNode->npcid()].push_back(pNode);
			}
		}
	}
}


int	 CCountry::GainScoreGiftIndex(int64 uid)
{
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	const DB_Arena_Score_List& rScoreList = pArenaRank->arenascorelistold();
	for(int i=0;i<rScoreList.scorelist_size();i++)
	{
		const DB_Arena_Score_Unit& unit = rScoreList.scorelist(i);
		if(uid == unit.uid())
		{
			return i+1;
		}
	}

	return -1;
}

int UpdateUserScoreList_Cmp(const pair<int64,int>& a, const pair<int64,int>& b)   
{   
	return a.second > b.second;   
}

bool CCountry::UpdateUserScoreList(DB_Arena_Score_List* plist,int64 uid,int nScore)
{
	if(plist == NULL)
	{
		return false;
	}

	if(plist->scorelist_size() == 0 && nScore > 0)
	{
		DB_Arena_Score_Unit* pUnit = plist->add_scorelist();
		if(pUnit)
		{
			pUnit->set_uid(uid);
			pUnit->set_score(nScore);
		}
		return true;
	}

	if(nScore > 0 && plist->scorelist_size() > 3 && nScore < plist->scorelist(plist->scorelist_size() - 1).score())
	{
		return false;
	}
	//使用map排序
	map<int64,int> rmapList;
	rmapList.clear();

	for(int i=0;i<plist->scorelist_size();i++)
	{
		const DB_Arena_Score_Unit& runit = plist->scorelist(i);
		rmapList[runit.uid()] = runit.score();
	}

	rmapList[uid] = nScore;

	vector<pair<int64,int> > tVector;
	tVector.clear();
	for(map<int64,int>::iterator itr = rmapList.begin();itr != rmapList.end();itr ++)
	{
		tVector.push_back(make_pair(itr->first,itr->second));
	}

	sort(tVector.begin(),tVector.end(),UpdateUserScoreList_Cmp);

	plist->clear_scorelist();

	for(int i=0;i<3 && i < (int)tVector.size();i++)
	{
		const pair<int64,int>& node = tVector[i];
		if(node.second > 0)
		{
			DB_Arena_Score_Unit* pUnit = plist->add_scorelist();
			if(pUnit)
			{
				pUnit->set_uid(node.first);
				pUnit->set_score(node.second);
			}
		}		
	}
	return true;
}

bool CCountry::UpdateScoreGiftIndex(int64 uid,int nArenaID,int nArenaScore,int nScoreAdd)
{
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	DB_Arena_Score_List* pScoreList = pArenaRank->mutable_arenascorelist();
	DB_Arena_Score_List* pScoreListOld = pArenaRank->mutable_arenascorelistold();
	DB_Arena_Score_List* pScoreListCurr = pArenaRank->mutable_currentscorelist();

	//更新当前展示表
#ifdef _WIN32 
	if(nArenaID != pScoreListCurr->nactive())
#else
	if(nArenaID > pScoreListCurr->nactive())
#endif
	{
		//新一期
		pScoreListCurr->Clear();
		pScoreListCurr->set_nactive(nArenaID);
	}

	if(nArenaID == pScoreListCurr->nactive())
	{
		UpdateUserScoreList(pScoreListCurr,uid,nArenaScore);
	}	

	if(nArenaID == pScoreList->nactive())
	{
		//更新新表
		return UpdateUserScoreList(pScoreList,uid,nArenaScore + nScoreAdd);
	}

#ifdef _WIN32 
	if(nArenaID != pScoreList->nactive())
#else
	if(nArenaID > pScoreList->nactive())
#endif
	{
		pScoreListOld->Clear();
		pScoreListOld->CopyFrom(*pScoreList);
		for(int i=0;i<pScoreListOld->scorelist_size();i++)
		{
			const DB_Arena_Score_Unit& runit = pScoreListOld->scorelist(i);
			SYS_STAT(0,"arenascorerank",0,0,false,0,0,0,0,0,m_nRegion,0,0,false,m_nCountryID,pScoreList->nactive(),i+1,(int64)runit.uid(),runit.score());
		}
		//新表清空
		pScoreList->Clear();
		pScoreList->set_nactive(nArenaID);

		SYS_LOG(0,LT_ARENA_COUNTRY_NEWSCORELIST,m_nRegion,0,0<<m_nCountryID<<nArenaID<<toString(uid)<<nArenaScore);

		UpdateUserScoreList(pScoreList,uid,nArenaScore + nScoreAdd);

		return true;
	}

	SYS_LOG(0,LT_ARENA_COUNTRY_NEWSCORELIST,m_nRegion,0,1<<m_nCountryID<<nArenaID<<toString(uid)<<nArenaScore);

	return false;
}

void	CCountry::ClearScoreGiftIndex(int64 uid)
{
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	DB_Arena_Score_List* pScoreList = pArenaRank->mutable_arenascorelist();
	UpdateUserScoreList(pScoreList,uid,0);

	pScoreList = pArenaRank->mutable_currentscorelist();
	UpdateUserScoreList(pScoreList,uid,0);
	
	SYS_LOG(0,LT_ARENA_COUNTRY_NEWSCORELIST,m_nRegion,0,2<<m_nCountryID<<pScoreList->nactive()<<toString(uid)<<0);
}

bool	CCountry::UpdateRankNpc()
{
	bool bUpdate = false;

	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	if(pArenaRank->arenaranknpc_size() <= 0)
	{
		InitRankNpc();
		bUpdate = true;
	}
	return bUpdate;
}


bool	CCountry::UpdateRank()
{
	bool bUpdate = UpdateRankNpc();
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	time_t nowTime = time(NULL);
	time_t lastTime = pArenaRank->nranktimelast();

	struct tm nowtime = *localtime(&nowTime);
	struct tm lastime = *localtime(&lastTime);

	if (lastime.tm_yday != nowtime.tm_yday || lastime.tm_year != nowtime.tm_year)
	{		
		//记录更新
		pArenaRank->clear_arenaranklast();

		DB_Arena_Rank_List* pArenaRankLast = pArenaRank->mutable_arenaranklast();

		int nRankNow = E_COUNTRY_ARENA_RANK_NUM;

		if((int)m_rvecPlayArena.size() < nRankNow)
		{
			nRankNow = m_rvecPlayArena.size();
		}

		for(int i=0;i<nRankNow;i++)
		{
			pArenaRankLast->add_uidlist(m_rvecPlayArena[i]);
		}

		pArenaRank->set_nranktimelast((int)nowTime);

		bUpdate = true;

		SYS_LOG(0,LT_ARENA_COUNTRY_RANKSTART,m_nRegion,0,m_nCountryID<<lastTime<<nowTime<<nRankNow);
	}
	return bUpdate;
}

bool	 CCountry::GetRankInfo(RseArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL)
	{
		return false;
	}

	bool bUpdate = UpdateRank();

	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();

	//填充排行榜数据
	ArenaRankCountry* pArenaRankCountry = pArenaRankInfo->add_arenaranklist();

	pArenaRankCountry->set_countryid(m_dbCountry.countryid());
	pArenaRankCountry->set_nranktime(pArenaRank->nranktimelast());
	pArenaRankCountry->set_narenanum(m_rvecPlayArena.size());

	const DB_Arena_Rank_List& rArenaRankLast = pArenaRank->arenaranklast();

	for(int i=0;i<rArenaRankLast.uidlist_size();i++)
	{
		ArenaRankNode* pNode = pArenaRankCountry->add_arenaranklist();
		pNode->set_nindex(i);
		pNode->set_uid(toString(rArenaRankLast.uidlist(i)));
	}

	const DB_Arena_Score_List & rArenaScoreLast1 = pArenaRank->currentscorelist();
	for(int i=0;i<rArenaScoreLast1.scorelist_size();i++)
	{
		ArenaRankNode* pNode = pArenaRankCountry->add_currentscorelist();
		pNode->set_nindex(i+1);
		pNode->set_uid(toString(rArenaScoreLast1.scorelist(i).uid()));
	}

	const DB_Arena_Score_List & rArenaScoreLast2 = pArenaRank->arenascorelistold();
	for(int i=0;i<rArenaScoreLast2.scorelist_size();i++)
	{
		ArenaRankNode* pNode = pArenaRankCountry->add_lastscorelist();
		pNode->set_nindex(i+1);
		pNode->set_uid(toString(rArenaScoreLast2.scorelist(i).uid()));
	}

	return bUpdate;
}

bool	 CCountry::GetRankFightInfo(int nPos,RseArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL)
	{
		return false;
	}

	bool bUpdate = UpdateRank();

	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	
	//获取排行榜数据
	ArenaFightRuleUnit* pRuleUnit = ArenaFightRule::Instance().GetWalkSpaceByRankIndex(nPos);
	if(pRuleUnit == NULL)
	{
		return bUpdate;
	}

	ArenaRankCountry* pCountryInfo = pArenaRankInfo->add_arenaranklist();

	pCountryInfo->set_countryid(m_dbCountry.countryid());
	pCountryInfo->set_nranktime((int)time(NULL));
	pCountryInfo->set_narenanum(m_rvecPlayArena.size());

	for(int i=0;i<ARENA_FIGHT_RULE_NUM;i++)
	{
		ArenaRankNode* pNode = pCountryInfo->add_arenaranklist();

		int nIndex = nPos;

		switch(pRuleUnit->m_nwalkWay)
		{
		case E_ARENA_FIGHT_RULE_PEC:		//乘百分比
			{
				nIndex = nIndex * pRuleUnit->m_nArgv[i] / 100;
			}
			break;
		case E_ARENA_FIGHT_RULE_DEL:		//减数值
			{
				nIndex = nIndex - pRuleUnit->m_nArgv[i];
			}
			break;
		case E_ARENA_FIGHT_RULE_INDEX:		//名次值
			{
				nIndex = pRuleUnit->m_nArgv[i];
			}
			break;
		}
		pNode->set_nindex(nIndex);
		pNode->set_uid(toString(GetRankUidByIndex(nIndex)));
	}

	//前10名
	if(pRuleUnit->m_nwalkWay != E_ARENA_FIGHT_RULE_INDEX)
	{
		for(int i=0;i<ARENA_FIGHT_RULE_NUM;i++)
		{
			ArenaRankNode* pNode = pCountryInfo->add_arenatoplist();
			pNode->set_nindex(i);
			pNode->set_uid(toString(GetRankUidByIndex(i)));
		}
	}

	return bUpdate;
}

int64	 CCountry::GetRankUidByIndex(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_rvecPlayArena.size())
	{
		return 0;
	}

	return m_rvecPlayArena[nIndex];
}

int		 CCountry::AddNewNpc(int64 nUid)
{	
	int nIndex = m_rvecPlayArena.size();
	m_rvecPlayArena.push_back(nUid);

	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	DB_Arena_Rank_NPC* pArenaNpc = pArenaRank->add_arenaranknpc();
	pArenaNpc->set_npcindex(nIndex);
	pArenaNpc->set_npcid((int)nUid);
	m_rmapNpcArena[(int)nUid].push_back(pArenaNpc);
	return nIndex;
}

int		 CCountry::AddNewPlayer(DB_C_User* pCUser)
{
	if(pCUser == NULL || pCUser->countryid() <=0)
	{
		return -1;
	}
	int nIndex = m_rvecPlayArena.size();
	if(pCUser->arenaindex() > 0)
	{
		return pCUser->arenaindex();
	}
	m_rvecPlayArena.push_back(pCUser->userid());
	pCUser->set_arenaindex(nIndex);
	return nIndex;
}

void	 CCountry::InitRankNpc()
{
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	pArenaRank->clear_arenaranknpc();

	int nNpcCunt = ArenaNpcInital::Instance().GetArenaNpcInitalUnitCnt();

	for(int i=0;i<nNpcCunt;i++)
	{
		ArenaNpcInitalUnit* pUnit = ArenaNpcInital::Instance().GetArenaNpcInitalUnitByIndex(i);
		if(pUnit)
		{
			for(int j=0;j<pUnit->m_nNum;j++)
			{
				AddNewNpc(pUnit->m_nNpcID);
			}
		}
	}
}

void	CCountry::CleraArenaData()
{
	m_rmapNpcArena.clear();
	m_rvecPlayArena.clear();
	//m_dbCountry.mutable_arenarank()->Clear();
	DB_Arena_Rank* pArenaRank = m_dbCountry.mutable_arenarank();
	pArenaRank->clear_arenaranknpc();
	pArenaRank->clear_arenaranklast();
	pArenaRank->clear_nranktimelast();
	//pArenaRank->clear_arenascorelist();
	//pArenaRank->clear_arenascorelistold();
	//pArenaRank->clear_currentscorelist();
}

void	CCountry::ClearWorldArenaData()
{
	m_rmapNpcArena.clear();
	m_rvecPlayArena.clear();
	DB_WorldArena_Rank* pWorldArenaRank = m_dbCountry.mutable_worldarenarank();
	pWorldArenaRank->clear_worldarenaranknpc();
	//pWorldArenaRank->clear_worldarenaranklast();

}

void CGuild::SetPKGuildID(int nTurns,int nGuildID)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return;
	DB_C_GuildBtl* pDBBtl = m_pDBGuild->mutable_btlinfo();
	for(int i=pDBBtl->result_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBBtl->add_result();
	}
	DB_C_GuildBtlResult* pDBRet = pDBBtl->mutable_result(nTurns);
	pDBRet->set_pkguild(nGuildID);
}

int CGuild::GetPKGuildID(int nTurns)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return 0;
	DB_C_GuildBtl* pDBBtl = m_pDBGuild->mutable_btlinfo();
	for(int i=pDBBtl->result_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBBtl->add_result();
	}
	DB_C_GuildBtlResult* pDBRet = pDBBtl->mutable_result(nTurns);
	return pDBRet->pkguild();
}

bool CGuild::HasPKToday()
{
	DB_C_GuildBtl* pDBGBInfo = GetDBGuild()->mutable_btlinfo();
	//for(int i=0;i<pDBGBInfo->result_size();i++)
	//{
	//	if(pDBGBInfo->mutable_result(i)->result() != GBR_Unknow)
	//		return true;
	//}
	//return false;
	return (pDBGBInfo->signupcnt()>=GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinJionCnt);
}

bool CGuild::IsUserInGvgBtl(int64 nUserID)
{
	DB_C_GVG_Msg* pDBGvgMsg = GetDBGvgMsg();
	if(pDBGvgMsg){
		for ( int i = 0; i < pDBGvgMsg->userway_size(); i++ ){
			DB_C_GvgBtlCityMsg* pCityMsg = pDBGvgMsg->mutable_userway(i);
			if (pCityMsg){
				for ( int j = 0; j < pCityMsg->userway1_size(); j++ ){
					if ( pCityMsg->userway1(j) == nUserID){
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CGuild::IsUserSignUpBtl(int64 nUserID)
{
	DB_C_GuildBtl* pDBGBInfo = GetDBGuild()->mutable_btlinfo();
	time_t ltNow = time(NULL);
	tm tmNow	= *localtime(&ltNow);
	const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();
	
	int nSecNow	= tmNow.tm_hour*3600 + tmNow.tm_min*60 + tmNow.tm_sec;
	if(nSecNow > pGBtlTbl->m_nBalanceTM)
	{
		return false;
	}

	for(int i=0;i<pDBGBInfo->userway0_size();i++)
	{
		if(pDBGBInfo->userway0(i) == nUserID)
			return true;
	}
	for(int i=0;i<pDBGBInfo->userway1_size();i++)
	{
		if(pDBGBInfo->userway1(i) == nUserID)
			return true;
	}
	for(int i=0;i<pDBGBInfo->userway2_size();i++)
	{
		if(pDBGBInfo->userway2(i) == nUserID)
			return true;
	}
	return false;
}

bool CGuild::GetGvgCity( int nID )
{
	DB_C_GVG_Msg* pDBGvgMsg = GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return false;
	}

	if ( pDBGvgMsg->controlcity() == 0 )
	{
		pDBGvgMsg->set_controlcity( nID );
		return true;
	}

	int nNum = 1;
	for ( int i = 0; i < pDBGvgMsg->signcity_size(); i++ )
	{
		if ( pDBGvgMsg->controlcity() == pDBGvgMsg->signcity(i) )
		{
			//已经占领报名的城市，需要根据报名顺序决定是否占领
			nNum = i;
		}
	}
	
	bool bGet = false;
	for ( int i = 0; i < pDBGvgMsg->signcity_size(); i++ )
	{
		if ( nID == pDBGvgMsg->signcity(i) && nNum != 0 )
		{
			pDBGvgMsg->set_controlcity( nID );
			return true;
		}
	}

	if ( pDBGvgMsg->controlcity() == nID )
	{
		return true;
	}
	
	return false;
}

void CGuild::LeaveGvgCity( int nID )
{
	DB_C_GVG_Msg* pDBGvgMsg = GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	if ( pDBGvgMsg->controlcity() == nID )
	{
		pDBGvgMsg->set_controlcity( 0 );
	}
}

void CGuild::CheckGvg( int nFlag )
{
	if ( nFlag == 0 )
	{
		const GvgActiveTbl* pTbl = GvgCfg::Instance().GetActiveTbl();
		if ( pTbl == NULL )
		{
			return;
		}

		nFlag = pTbl->m_nID;
	}
	DB_C_GVG_Msg* pDBGvgMsg = GetDBGvgMsg();
	if ( pDBGvgMsg == NULL )
	{
		return;
	}

	if ( nFlag != pDBGvgMsg->flag() )
	{
		pDBGvgMsg->clear_playerid();
		for ( int i = 0; i < pDBGvgMsg->userway_size(); i++ )
		{
			DB_C_GvgBtlCityMsg* pDBBtlCityMsg = pDBGvgMsg->mutable_userway(i);
			if ( pDBBtlCityMsg == NULL )
			{
				continue;
			}

			for ( int j = 0; j < pDBBtlCityMsg->userway1_size(); j++ )
			{
				if ( pDBBtlCityMsg->userway1(j) == 0 )
				{
					continue;
				}
				pDBGvgMsg->add_playerid( pDBBtlCityMsg->userway1(j) );
			}
		}

		int nOldFLag = pDBGvgMsg->flag();
		//重置数据
		pDBGvgMsg->clear_signcity();
		pDBGvgMsg->clear_userway();
		pDBGvgMsg->clear_addprop();
		pDBGvgMsg->clear_fightend();
		pDBGvgMsg->clear_lastcontributetime();
		pDBGvgMsg->clear_state();
		if ( nOldFLag == 2 && nFlag == 3 )
		{
			//第二期活动数据出错，不清除上期贡献
			pDBGvgMsg->set_contribute( pDBGvgMsg->contributerecorcd() + pDBGvgMsg->contribute() );
			pDBGvgMsg->set_contributerecorcd(0);
		}
		else
		{
			pDBGvgMsg->set_contribute( pDBGvgMsg->contributerecorcd() );
			pDBGvgMsg->set_contributerecorcd(0);
		}
		pDBGvgMsg->clear_rewardrecord();

		pDBGvgMsg->set_flag( nFlag );
	}
}

void CGuild::CheckLeaguePoint( int nFlag )
{
	if ( GetGuildLV() < 2 )
	{
		return;
	}

	if ( nFlag == 0 )
	{
		const LeagueBattleActiveTbl* pTbl = LeagueBattleCfg::Instance().GetActiveTbl();
		if ( pTbl == NULL )
		{
			return;
		}

		nFlag = pTbl->m_nID;
	}

	static int nTenMin = 900;
	time_t tNow = time(NULL);

	if ( nFlag != m_pDBGuild->leagueactiveid() )
	{
		m_pDBGuild->set_leaguepoint( 0 );
		m_pDBGuild->clear_leaguelist();
		m_pDBGuild->set_leagueleaderuid( 0 );
		m_pDBGuild->set_leagueactiveid( nFlag );
		m_ltLastLeagueCount = tNow;
		return;
	}

	if(tNow - m_ltLastLeagueCount > nTenMin)
	{
		m_ltLastLeagueCount = tNow;
	}
}

bool CGuild::AddLeaguePoint( int nPoint )
{
	CheckLeaguePoint();
	m_pDBGuild->set_leaguepoint( m_pDBGuild->leaguepoint() + nPoint );

	static int nTenMin = 600;

	time_t tNow = time(NULL);
	if ( tNow - m_ltLastSaveTime > nTenMin )
	{
		m_ltLastSaveTime = tNow;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

bool compareGvg(CGuild* pGuild1, CGuild* pGuild2)
{
	DB_C_GVG_Msg* pDBGvg1 = pGuild1->GetDBGvgMsg();
	DB_C_GVG_Msg* pDBGvg2 = pGuild2->GetDBGvgMsg();
	if(pDBGvg1->contribute() > pDBGvg2->contribute())
		return true;
	else if(pDBGvg1->contribute() < pDBGvg2->contribute())
		return false;

	if(pDBGvg1->lastcontributetime() < pDBGvg2->lastcontributetime())
		return true;
	else if(pDBGvg1->lastcontributetime() > pDBGvg2->lastcontributetime())
		return false;

	if(pGuild1->GetGuildLV() > pGuild2->GetGuildLV())
		return true;
	else if ( pGuild1->GetGuildLV() < pGuild2->GetGuildLV())
		return false;

	if(pGuild1->GetGuildID() > pGuild2->GetGuildID())
		return true;
	else if ( pGuild1->GetGuildID() < pGuild2->GetGuildID())
		return false;
}

void CGvgCity::SortGvg()
{
	sort( m_lstSignGuild.begin(), m_lstSignGuild.end(), compareGvg );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CWarGameSite::CWarGameSite(DB_C_WarGameSite *pDBSite)
:m_pDBSiteInfo(NULL)
{
    if (pDBSite == NULL)
    {
        return;
    }
    m_pDBSiteInfo = pDBSite;
}
CWarGameSite::~CWarGameSite()
{
    m_pDBSiteInfo = NULL;
}

bool CWarGameSite::ChangeOccupant(string strUid, string strName, int nCountryID, int nStartTime, int nEndTime, int nPower, const vector<CWarGameAdmiral>& vecAdmiral)
{
    OccupantLeave();
    UpdateName(strName);
    UpdateCountry(nCountryID);
    UpdatePower(nPower);
    UpdateUid(strUid);
    UpdateStartTime(nStartTime);
    UpdateEndTime(nEndTime);
    UpdateAdmiralInfo(vecAdmiral);
    m_pDBSiteInfo->set_delaynum(0);

    return true;
}

bool CWarGameSite::OccupantDelay()
{
    if (MAX_WARGAME_DELAY_NUM <= m_pDBSiteInfo->delaynum())
    {
        return false;
    }

    m_pDBSiteInfo->set_endtime(m_pDBSiteInfo->endtime() + WarGameCfg::Instance().GetDelayInterval());
    m_pDBSiteInfo->set_delaynum(m_pDBSiteInfo->delaynum() + 1);
    return true;
}

bool CWarGameSite::IsOccupy()
{
    int tNow = time(NULL);
    if (m_pDBSiteInfo->endtime() >= tNow)
    {
        return true;
    }

    OccupantLeave();

    return false;
}

bool CWarGameSite::IsOccupyByUser(string strUid)
{
    if (IsOccupy() && m_pDBSiteInfo->uid() == strUid)
    {
        return true;
    }

    return false;
}

bool CWarGameSite::GetOccupantInfo(string &strUid, string &strName, int &nCountryID)
{
    strUid = m_pDBSiteInfo->uid();
    strName = m_pDBSiteInfo->name();
    nCountryID = m_pDBSiteInfo->countryid();

    return true;
}

bool CWarGameSite::OccupantLeave()
{
    int nId = m_pDBSiteInfo->sitid();
    int nLv = m_pDBSiteInfo->sitlv();
    m_pDBSiteInfo->Clear();
    m_pDBSiteInfo->set_sitid(nId);
    m_pDBSiteInfo->set_sitlv(nLv);

    return true;
}

void CWarGameSite::FillSiteInfo(WarGameSiteInfo *pSiteInfo, int &tNow)
{
    pSiteInfo->set_id(m_pDBSiteInfo->sitid());
    pSiteInfo->set_lv(m_pDBSiteInfo->sitlv());
    if (!IsOccupy())
    {
        pSiteInfo->set_hasdelay(false);
        pSiteInfo->set_surplusdelaynum(MAX_WARGAME_DELAY_NUM);
        pSiteInfo->set_boccupy(false);
        return;
    }
    (m_pDBSiteInfo->delaynum() > 0)?pSiteInfo->set_hasdelay(true) : pSiteInfo->set_hasdelay(false);
    pSiteInfo->set_surplusdelaynum(MAX_WARGAME_DELAY_NUM - m_pDBSiteInfo->delaynum());
    pSiteInfo->set_boccupy(true);
    pSiteInfo->set_uid(m_pDBSiteInfo->uid());
    pSiteInfo->set_name(m_pDBSiteInfo->name());
    pSiteInfo->set_power(m_pDBSiteInfo->power());
    pSiteInfo->set_country(m_pDBSiteInfo->countryid());
    pSiteInfo->set_nstarttime(m_pDBSiteInfo->starttime());
    for (int i = 0; i < m_pDBSiteInfo->admirallist_size(); i++)
    {
        DB_C_WarGameAdmiralInfo *pAdmiralInfo = m_pDBSiteInfo->mutable_admirallist(i);
        if (pAdmiralInfo == NULL)
        {
            continue;
        }
        WarGameAdmiralInfo *pAdmiral = pSiteInfo->add_admirallist();
        pAdmiral->set_admiralid(pAdmiralInfo->admiralid());
        pAdmiral->set_admirallv(pAdmiralInfo->admirallv());
        pAdmiral->set_admiraltrainlv(pAdmiralInfo->admiraltrainlv());
    }
    if (tNow < m_pDBSiteInfo->truceendtime())
    {
        pSiteInfo->set_truceflag(true);
        pSiteInfo->set_ntruceendttime(m_pDBSiteInfo->truceendtime());
    }
}

bool CWarGameSite::IsSiteOnProtect()
{
    int tNow = time(NULL);
    if ((tNow - m_pDBSiteInfo->starttime()) < WarGameCfg::Instance().GetProtectInterval())
    {
        return true;
    }

    return false;
}

bool CWarGameSite::IsSiteOnTruce()
{
    int tNow = time(NULL);
    if (tNow < m_pDBSiteInfo->truceendtime())
    {
        return true;
    }

    return false;
}

int CWarGameSite::GetSiteLv()
{
    return m_pDBSiteInfo->sitlv();
}

int CWarGameSite::GetSiteID()
{
    return m_pDBSiteInfo->sitid();
}
int CWarGameSite::GetStartTime()
{
    return m_pDBSiteInfo->starttime();
}
int CWarGameSite::GetEndTime()
{
    return m_pDBSiteInfo->endtime();
}
string CWarGameSite::GetUid()
{
    return m_pDBSiteInfo->uid();
}
int CWarGameSite::GetDelayNum()
{
    return m_pDBSiteInfo->delaynum();
}
string CWarGameSite::GetName()
{
    return m_pDBSiteInfo->name();
}
int CWarGameSite::GetTruceStartTime()
{
    return m_pDBSiteInfo->trucestarttime();
}

int CWarGameSite::GetTruceEndTime()
{
    return m_pDBSiteInfo->truceendtime();
}

void CWarGameSite::UpdateUid(string strUID)
{
    m_pDBSiteInfo->set_uid(strUID);
}
void CWarGameSite::UpdateStartTime(int nStarTime)
{
    m_pDBSiteInfo->set_starttime(nStarTime);
}
void CWarGameSite::UpdateEndTime(int nEndTime)
{
    m_pDBSiteInfo->set_endtime(nEndTime);
}
void CWarGameSite::UpdateName(string strName)
{
    m_pDBSiteInfo->set_name(strName);
}
void CWarGameSite::UpdateCountry(int nCountryID)
{
    m_pDBSiteInfo->set_countryid(nCountryID);
}
void CWarGameSite::UpdatePower(int nPower)
{
    m_pDBSiteInfo->set_power(nPower);
}
void CWarGameSite::UpdateAdmiralInfo(const vector<CWarGameAdmiral>& vecAdmiral)
{
    m_pDBSiteInfo->clear_admirallist();
    for (int i = 0; i < vecAdmiral.size(); i++)
    {
        DB_C_WarGameAdmiralInfo *pDBAdm = m_pDBSiteInfo->add_admirallist();
        if (pDBAdm != NULL)
        {
            pDBAdm->set_admiralid(vecAdmiral[i].m_nAdmID);
            pDBAdm->set_admirallv(vecAdmiral[i].m_nAdmLv);
            pDBAdm->set_admiraltrainlv(vecAdmiral[i].m_nAdmTrainLv);
        }
    }
}

void CWarGameSite::AddTruceTime()
{
    int tNow =time(NULL);
    if (tNow > m_pDBSiteInfo->truceendtime())
    {
        m_pDBSiteInfo->set_trucestarttime(tNow);
        m_pDBSiteInfo->set_truceendtime(tNow);
    }
    m_pDBSiteInfo->set_truceendtime(m_pDBSiteInfo->truceendtime() + WarGameCfg::Instance().GetTruceTime());
}

CWarGameData::CWarGameData()
{
    m_pDBWarGameData = NULL;
    m_mapSiteList.clear();
}

CWarGameData::~CWarGameData()
{
    m_pDBWarGameData = NULL;
    m_mapSiteList.clear();
}

CWarGameSite *CWarGameData::GetWarGameSiteByID(int nID)
{
    map<int, CWarGameSite *>::iterator iter = m_mapSiteList.find(nID);
    if (iter != m_mapSiteList.end())
    {
        return iter->second;
    }

    return NULL;
}

CWarGameSite *CWarGameData::GetWarGameSiteByUid(string strUid)
{
    for (map<int, CWarGameSite *>::iterator iter = m_mapSiteList.begin(); iter != m_mapSiteList.end(); iter++)
    {
        if (iter->second != NULL && iter->second->IsOccupyByUser(strUid))
        {
            return iter->second;
        }
    }

    return NULL;
}

DB_C_WarGameData * CWarGameData::GetDBWarGameData()
{
    return m_pDBWarGameData;
}

bool CWarGameData::InitByDBData(DB_C_WarGameData *pWarGameData, bool &bNeedBrushDB)
{
    if (pWarGameData == NULL)
    {
        return false;
    }

    m_pDBWarGameData = pWarGameData;

    const map<int, WarGameSiteTbl*> allSiteInfo = WarGameCfg::Instance().GetAllSiteInfo();
    for (map<int, WarGameSiteTbl*>::const_iterator iter = allSiteInfo.begin(); iter != allSiteInfo.end(); iter++)
    {
        DB_C_WarGameSite *pDBWarGameSite = NULL;
        for (int i = 0; i < m_pDBWarGameData->sitelist_size(); i++)
        {
            pDBWarGameSite = m_pDBWarGameData->mutable_sitelist(i);
            if (pDBWarGameSite != NULL && pDBWarGameSite->sitid() == iter->first)
            {
                break;
            }
            pDBWarGameSite = NULL;
        }
        if (pDBWarGameSite == NULL)
        {
            pDBWarGameSite = m_pDBWarGameData->add_sitelist();
            pDBWarGameSite->set_sitid(iter->second->m_nID);
            pDBWarGameSite->set_sitlv(iter->second->m_nLv);
            pDBWarGameSite->set_delaynum(0);
            pDBWarGameSite->set_endtime(0);
            pDBWarGameSite->set_starttime(0);
			bNeedBrushDB = true;
        }
        m_mapSiteList[pDBWarGameSite->sitid()] = new CWarGameSite(pDBWarGameSite);
    }
}

void CWarGameData::FillAllWarGameSiteInfo(RseWarGameOpt *pRse)
{
    int tNow = time(NULL);
    for (map<int, CWarGameSite *>::iterator iter = m_mapSiteList.begin(); iter != m_mapSiteList.end(); iter++)
    {
        int nLv = iter->second->GetSiteLv();
        WarGameSiteList *pSiteList = NULL;
        for (int i = 0; i < pRse->sitelistbylv_size(); i++)
        {
            pSiteList = pRse->mutable_sitelistbylv(i);
            if (pSiteList->nlv() == nLv)
            {
                break;
            }
            pSiteList = NULL;
        }

        if (pSiteList == NULL)
        {
            pSiteList = pRse->add_sitelistbylv();
            pSiteList->set_nlv(nLv);
        }
        WarGameSiteInfo *pSiteInfo = pSiteList->add_sitelist();
        iter->second->FillSiteInfo(pSiteInfo, tNow);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//同盟战

bool compareLeaguePeasonRank(DB_C_User* pUser1, DB_C_User* pUser2)
{
	if(pUser1->leaguepoint() > pUser2->leaguepoint())
		return true;
	else if (pUser1->leaguepoint() < pUser2->leaguepoint())
		return false;
	else if (pUser1->userid() > pUser2->userid())
		return true;
	else 
		return false;

}

bool compareLeagueGuildRank(CGuild* pGuild1, CGuild* pGuild2)
{
// 	pGuild1->CheckLeaguePoint();
// 	pGuild2->CheckLeaguePoint();
	int nPoint1 = pGuild1->GetDBGuild()->leaguepoint();
	int nPoint2 = pGuild2->GetDBGuild()->leaguepoint();
	if( nPoint1 > nPoint2 )
		return true;
	else if( nPoint1 < nPoint2 )
		return false;
	else if (pGuild1->GetGuildID() > pGuild2->GetGuildID())
		return true;
	else 
		return false;
}

void CCountryLeague::SortPlayer( CountryRgnDatHandler* pRDH )
{
	if ( pRDH == NULL )
	{
		return;
	}
	sort(m_lstUserRank.begin(),m_lstUserRank.end(),compareLeaguePeasonRank);

	m_pDBCountryLeague->clear_playerranklist();

	for ( int i = 0; i < MAX_LEAGUE_RANK_NUM && i < m_lstUserRank.size(); i++ )
	{
		DB_C_LeaguePlayerUnit* pUnit = m_pDBCountryLeague->add_playerranklist();
		DB_C_User* pUser = m_lstUserRank[i];
		if ( pUser == NULL || pUnit == NULL )
		{
			continue;
		}

		pUnit->set_uid(toString(pUser->userid()));
		pUnit->set_name(pUser->name());
		pUnit->set_pic(pUser->pic());
		pUnit->set_countryid(pUser->countryid());

		pUnit->set_fightpoint(pRDH->GetWpcFightPoint(toString(pUser->userid())));
		pUnit->set_level(pUser->level());
		pUnit->set_leaguepoint( pUser->leaguepoint() );
		pUnit->set_pic(pUser->pic());
	}
}

void CCountryLeague::SortGuild()
{
	sort(m_lstGuildRank.begin(),m_lstGuildRank.end(),compareLeagueGuildRank);

	m_pDBCountryLeague->clear_guildlist();

	for ( int i = 0; i < MAX_LEAGUE_RANK_NUM && i < m_lstGuildRank.size(); i++ )
	{
		CGuild* pGuild = m_lstGuildRank[i];
		if ( pGuild == NULL )
		{
			continue;
		}

		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();

		DB_C_LeagueGuildUnit* pDBUnit = m_pDBCountryLeague->add_guildlist();
		pDBUnit->set_guildid(pGuild->GetGuildID());
		pDBUnit->set_guildname(pGuild->GetGuildName());
		pDBUnit->set_leadername(pGuild->GetLeaderName());
		pDBUnit->set_leaguepoint(pDBGuild->leaguepoint());
		pDBUnit->set_countryid(pGuild->GetCountry());
	}
}

int CCountryLeague::GetPlayerRank( DB_C_User* pUser )
{
	for ( int i = 0; i < m_pDBCountryLeague->playerranklist_size(); i++ )
	{
		DB_C_LeaguePlayerUnit* pUnit = m_pDBCountryLeague->mutable_playerranklist(i);

		int64 uid = 0;
		safe_atoll( pUnit->uid(), uid );
		if ( uid == pUser->userid() )
		{
			return i+1;
		}
	}
	return 0;
}

int CCountryLeague::GetGuildRank( CGuild* pGuild )
{
	for ( int i = 0; i < m_pDBCountryLeague->guildlist_size(); i++ )
	{
		DB_C_LeagueGuildUnit* pUnit = m_pDBCountryLeague->mutable_guildlist(i);
		if ( pUnit->guildid() == pGuild->GetGuildID() )
		{
			return i+1;
		}
	}
	return 0;
}

void CCountryLeague::FillRankList( LeagueMsg* pLeagueMsg, LeagueRewardType rankType, int nCnt )
{
	if ( pLeagueMsg == NULL )
	{
		return;
	}

	if ( nCnt == 0 )
	{
		nCnt = MAX_LEAGUE_RANK_SEND_NUM;
	}

	if ( rankType == LEAGUERWDTYPEPERSON )
	{
		for ( int i = 0; i < nCnt && i < m_pDBCountryLeague->playerranklist_size(); i++ )
		{
			DB_C_LeaguePlayerUnit* pUnit = m_pDBCountryLeague->mutable_playerranklist(i);
			CountryUser* pUser = pLeagueMsg->add_userrank();

			pUser->set_baseid( pUnit->uid() );
			pUser->set_first_name( pUnit->name() );
			pUser->set_lvl( pUnit->level() );
			pUser->set_combatpowervalue( pUnit->fightpoint() );
			pUser->set_leaguepoint( pUnit->leaguepoint() );
			pUser->set_countryid( pUnit->countryid() );
			pUser->set_pic(pUnit->pic());
		}
	}

	else if ( rankType == LEAGUERWDTYPEGUILD )
	{
		for ( int i = 0; i < nCnt && i < m_pDBCountryLeague->guildlist_size(); i++ )
		{
			DB_C_LeagueGuildUnit* pUnit = m_pDBCountryLeague->mutable_guildlist(i);
			GuildData* pGuild = pLeagueMsg->add_guildrank();

			pGuild->set_guildid(pUnit->guildid());
			pGuild->set_guildname(pUnit->guildname());
			pGuild->set_leadername(pUnit->leadername());
			pGuild->set_leaguepoint(pUnit->leaguepoint());
			pGuild->set_countryid(pUnit->countryid());
		}
	}
}

void CCountryLeague::FillLastRankList( LeagueMsg* pLeagueMsg, LeagueRewardType rankType, int nCnt )
{
	if ( pLeagueMsg == NULL )
	{
		return;
	}

	if ( nCnt == 0 )
	{
		nCnt = MAX_LEAGUE_RANK_SEND_NUM;
	}

	if ( rankType == LEAGUERWDTYPEPERSON )
	{
		for ( int i = 0; i < nCnt && i < m_pDBCountryLeague->playerranklastlist_size(); i++ )
		{
			DB_C_LeaguePlayerUnit* pUnit = m_pDBCountryLeague->mutable_playerranklastlist(i);
			CountryUser* pUser = pLeagueMsg->add_userrank();

			pUser->set_baseid( pUnit->uid() );
			pUser->set_first_name( pUnit->name() );
			pUser->set_lvl( pUnit->level() );
			pUser->set_combatpowervalue( pUnit->fightpoint() );
			pUser->set_leaguepoint( pUnit->leaguepoint() );
			pUser->set_countryid( pUnit->countryid() );
			pUser->set_pic(pUnit->pic());
		}
	}

	else if ( rankType == LEAGUERWDTYPEGUILD )
	{
		for ( int i = 0; i < nCnt && i < m_pDBCountryLeague->guildlastlist_size(); i++ )
		{
			DB_C_LeagueGuildUnit* pUnit = m_pDBCountryLeague->mutable_guildlastlist(i);
			GuildData* pGuild = pLeagueMsg->add_guildrank();

			pGuild->set_guildid(pUnit->guildid());
			pGuild->set_guildname(pUnit->guildname());
			pGuild->set_leadername(pUnit->leadername());
			pGuild->set_leaguepoint(pUnit->leaguepoint());
			pGuild->set_countryid(pUnit->countryid());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
CGuildRedEnvelope::CGuildRedEnvelope()
{
    Clear();

    m_nActFlag = GuildRedEnvelopeCfg::Instance().GetActiveFlag();
}

void CGuildRedEnvelope::SetCountryRgnData(CountryRgnDatHandler *pRgn)
{
    m_pRgnData = pRgn;
}

CGuildRedEnvelope::~CGuildRedEnvelope()
{
    Clear();
}

void CGuildRedEnvelope::Clear()
{
    m_nActFlag = 0;
    m_pRgnData = NULL;
    m_mapAllGuild.clear();
    m_mapAllUser.clear();
    m_lstRedGuildRank.clear();
    m_lstRedUserRank.clear();
    m_lstRegionBriefInfo.clear();
}

void CGuildRedEnvelope::AddUser(DB_C_User* pUser)
{
    if (NULL == pUser)
    {
        return;
    }
    if (0 != m_nActFlag)
    {
        DB_C_RedEnvelopeCount *pRedEnveLope = pUser->mutable_redenvelopecount();
        if (NULL == pRedEnveLope)
        {
            return;
        }
        if (pRedEnveLope->nflag() != m_nActFlag)
        {//清空上期的记录
            pRedEnveLope->Clear();
            pRedEnveLope->set_nflag(m_nActFlag);
            if (NULL != m_pRgnData)
            {
                m_pRgnData->markUserLstDirty(pUser->userid());
            }
        }
        if (pRedEnveLope->ntotalnum() > 0 || pRedEnveLope->nsendtotal() > 0)
        {
            m_mapAllUser[pUser->userid()] = pUser;
        }
    }
}

void CGuildRedEnvelope::AddGuildRedEnvelopeInfo(CGuild *pGuild)
{
    if (NULL == pGuild)
    {
        return;
    }
    DB_C_GuildRedEnvelope * pRedEnvelope = pGuild->GetDBGuildRedEnvelope();
    if (NULL == pRedEnvelope || 0 == pRedEnvelope->ntotal() || 0 == pRedEnvelope->listredenvelope_size())
    {
        return;
    }

    if (m_nActFlag != 0)
    {
        if (m_nActFlag != pRedEnvelope->nflag())
        {
            pRedEnvelope->Clear();
            pRedEnvelope->set_nflag(m_nActFlag);
            m_pRgnData->markGuildLstDirty(pGuild);
        }
        for (int i = 0; i < pRedEnvelope->listredenvelope_size(); i++)
        {
            DB_C_RedEnvelopeInfo *pRedInfo = pRedEnvelope->mutable_listredenvelope(i);
            if (pRedInfo == NULL)
            {
                continue;
            }
            DB_C_User *pUser = m_pRgnData->GetDBCityUser(pRedInfo->uid());
            if (pUser == NULL)
            {
                continue;
            }
            AddRedEnvelopeRecord(pRedInfo->nid(), pGuild->GetGuildID(), pUser->name(), pGuild->GetGuildName(), pRedInfo->nissuetime(), pRedInfo->nadjtype());
        }
    }
    if (pRedEnvelope->ntotal() > 0)
    {
        m_mapAllGuild[pGuild->GetGuildID()] = pGuild;
    }
}

DB_C_User * CGuildRedEnvelope::GetUserInfo(int64 lUid)
{
    map<int64, DB_C_User*>::iterator iter = m_mapAllUser.find(lUid);
    if (m_mapAllUser.end() == iter)
    {
        return NULL;
    }

    return iter->second;
}

CGuild * CGuildRedEnvelope::GetGuildInfo(int nGuildID)
{
    map<int, CGuild*>::iterator iter = m_mapAllGuild.find(nGuildID);
    if (iter == m_mapAllGuild.end())
    {
        return NULL;
    }
    return iter->second;
}

void CGuildRedEnvelope::RemoveGuild(CGuild *pGuild)
{
    map<int, CGuild*>::iterator iter = m_mapAllGuild.find(pGuild->GetGuildID());
    if (iter == m_mapAllGuild.end())
    {
        return;
    }
    m_mapAllGuild.erase(iter);
}

void CGuildRedEnvelope::SortGuildRank()
{
    multimap<int, CGuild*> rankGuild;

    for (map<int, CGuild*>::iterator iter = m_mapAllGuild.begin(); iter != m_mapAllGuild.end(); iter++)
    {
        DB_C_GuildRedEnvelope *pInfo = iter->second->GetDBGuildRedEnvelope();
        rankGuild.insert(make_pair(pInfo->ntotal(), iter->second));
    }

    m_lstRedGuildRank.clear();
    int nGuildRankNum = GuildRedEnvelopeCfg::Instance().GetGuildRankNum();
    //mutimap中的数据存储是从小到大的  因此需要反向迭代 获取排名
    multimap<int, CGuild*>::reverse_iterator tempIter = rankGuild.rbegin();
    for (int i = 0; tempIter != rankGuild.rend() && i < nGuildRankNum; tempIter++)
    {
        RedEnvelopeRankInfo tempRank;
        tempRank.set_ncountry( tempIter->second->GetCountry());
        tempRank.set_guildname(tempIter->second->GetGuildName());
        tempRank.set_grabbername(tempIter->second->GetLeaderName());
        tempRank.set_totalnum(toString(tempIter->first));
        i++;
        m_lstRedGuildRank.push_back(tempRank);
    }
}

void CGuildRedEnvelope::SortUserRank()
{
    multimap<int64, DB_C_User*> rankPersonal;

    for (map<int64, DB_C_User*>::iterator iter = m_mapAllUser.begin(); iter != m_mapAllUser.end(); iter++)
    {
        DB_C_RedEnvelopeCount *pInfo = iter->second->mutable_redenvelopecount();
        if (NULL == pInfo || pInfo->ntotalnum() <= 0)
        {
            continue;
        }
        rankPersonal.insert(make_pair(pInfo->ntotalnum(), iter->second));
    }

    m_lstRedUserRank.clear();
    int nPersonalRankNum = GuildRedEnvelopeCfg::Instance().GetPersonalRankNum();
    //mutimap中的数据存储是从小到大的  因此需要反向迭代 获取正确排名
    multimap<int64, DB_C_User*>::reverse_iterator tempIter = rankPersonal.rbegin();
    for (int i = 0; tempIter != rankPersonal.rend() && i < nPersonalRankNum; tempIter++)
    {
        RedEnvelopeRankInfo tempRank;
        tempRank.set_ncountry( tempIter->second->countryid());
        CGuild *pGuild = m_pRgnData->GetGuild(tempIter->second->guildid());
        if (pGuild != NULL)
        {
            tempRank.set_guildname(pGuild->GetGuildName());
        }
        else
        {
            tempRank.set_guildname("");
        }
        DB_C_RedEnvelopeCount *pInfo = tempIter->second->mutable_redenvelopecount();
        tempRank.set_grabbername(tempIter->second->name());
        tempRank.set_totalnum(toString(pInfo->ntotalnum()));
        i++;
        m_lstRedUserRank.push_back(tempRank);
    }
}

void CGuildRedEnvelope::SortSendUserRank()
{
    multimap<int, DB_C_User*> rankPersonal;

    for (map<int64, DB_C_User*>::iterator iter = m_mapAllUser.begin(); iter != m_mapAllUser.end(); iter++)
    {
        DB_C_RedEnvelopeCount *pInfo = iter->second->mutable_redenvelopecount();
        if (NULL == pInfo || pInfo->nsendtotal() <= 0)
        {
            continue;
        }
        rankPersonal.insert(make_pair(pInfo->nsendtotal(), iter->second));
    }

    m_lstRedSendUserRank.clear();
    int nPersonalRankNum = GuildRedEnvelopeCfg::Instance().GetPersonalRankNum();
    //mutimap中的数据存储是从小到大的  因此需要反向迭代 获取正确排名
    multimap<int, DB_C_User*>::reverse_iterator tempIter = rankPersonal.rbegin();
    for (int i = 0; tempIter != rankPersonal.rend() && i < nPersonalRankNum; tempIter++)
    {
        RedEnvelopeRankInfo tempRank;
        tempRank.set_ncountry( tempIter->second->countryid());
        CGuild *pGuild = m_pRgnData->GetGuild(tempIter->second->guildid());
        if (pGuild != NULL)
        {
            tempRank.set_guildname(pGuild->GetGuildName());
        }
        else
        {
            tempRank.set_guildname("");
        }
        DB_C_RedEnvelopeCount *pInfo = tempIter->second->mutable_redenvelopecount();
        tempRank.set_grabbername(tempIter->second->name());
        tempRank.set_totalnum(toString(pInfo->nsendtotal()));
        i++;
        m_lstRedSendUserRank.push_back(tempRank);
    }
}

void CGuildRedEnvelope::SnappeRedEnvelope(RseGuildRedEnvelopeOpt *pRse, int64 lUid, int nGuildID, int nRedEnvelopeID, CountryEventHandler *pEH)
{
    if (m_nActFlag == 0 || m_pRgnData == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_ACTOVERDUE);
        return;
    }
    DB_C_User *pUser = GetUserInfo(lUid);
    if (pUser == NULL)
    {
        pUser = m_pRgnData->GetDBCityUser(lUid);
    }

    if (pUser == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        return;
    }
    if (pUser->guildid() != nGuildID)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_NOTGUILDMEMBER);
        return;
    }
    DB_C_RedEnvelopeCount *pCount = pUser->mutable_redenvelopecount();
    if (pCount->nflag() != m_nActFlag)
    {
        pCount->Clear();
        pCount->set_nflag(m_nActFlag);
        pCount->set_ntotalnum(0);
        pCount->set_nsendtotal(0);
        m_pRgnData->markUserLstDirty(lUid);
    }

    CGuild *pGuild = GetGuildInfo(pUser->guildid());

    if (NULL == pGuild)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }
    DB_C_GuildRedEnvelope *pGuildRed = pGuild->GetDBGuildRedEnvelope();
    if (pGuildRed == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }
    DB_C_RedEnvelopeInfo *pRedEnvelope = NULL;
    for (int i = 0; i < pGuildRed->listredenvelope_size(); i++)
    {
        pRedEnvelope = pGuildRed->mutable_listredenvelope(i);
        if (pRedEnvelope == NULL)
        {
            continue;
        }
        if (pRedEnvelope->nid() == nRedEnvelopeID)
        {
            break;
        }
        pRedEnvelope = NULL;
    }
    if (pRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }

    FillRedEnveLopeInfo(pRse, lUid, nGuildID, nRedEnvelopeID);

    RedEnvelopeInfo *pRseRedInfo = pRse->mutable_envelopeinfo();

    if (pRseRedInfo->boverdue())
    {//过期
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_HASOVERDUE);
        return;
    }

    if (pRseRedInfo->bsnappedup())
    {//已经抢完
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_HASSNAPPEUP);
        return;
    }

    if (pRseRedInfo->bget())
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_HASGET);
        return;
    }

    int tNow = time(NULL);
    int nSize = pRedEnvelope->listreport_size();
    int nTotalCopies = pRedEnvelope->ntotalnum();

    int nGetCredit = 0;
    int nLeft = pRedEnvelope->ntotalcredit() - pRedEnvelope->ngiveout();
    int nLeftCopies = nTotalCopies - nSize;
    if (nLeftCopies <= 0 || nTotalCopies <= 0)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_HASSNAPPEUP);
        return;
    }
    if (pRedEnvelope->ntype() == 2)
    {//均分红包
        nGetCredit = pRedEnvelope->ntotalcredit()/nTotalCopies;
        nLeftCopies -= 1;
    }
    else if (nTotalCopies - nSize == 1)
    {
        nGetCredit = pRedEnvelope->ntotalcredit() - pRedEnvelope->ngiveout();
        nLeftCopies -= 1;
    }
    else if (nLeftCopies == nLeft)
    {
        nGetCredit = 1;
        nLeftCopies -= 1;
    }
    else
    {
        int nMaxGet = (pRedEnvelope->ntotalcredit() - pRedEnvelope->ngiveout())/nLeftCopies;
        nMaxGet *= 2;
        if (nMaxGet <= 0)
        {
            pRse->set_nresult(E_GUILD_REDENVELOPE_R_HASSNAPPEUP);
            return;
        }
        nGetCredit = rand()%nMaxGet + 1;

        //剩余的勋章数 至少要等于剩余的可领取的奖励分数
        nLeftCopies -= 1;
        if ((nLeft - nGetCredit) < nLeftCopies)
        {
            nGetCredit = nLeft - nLeftCopies;
        }
    }
    if (nGetCredit < 0)
    {//这里不会进 以防万一
        nGetCredit = 0;
    }
    nLeft -= nGetCredit;
    pRse->set_nsnappenum(nGetCredit);
    pRedEnvelope->set_ngiveout(pRedEnvelope->ngiveout() + nGetCredit);
    DB_C_RedEnvelopeReport *pReport = pRedEnvelope->add_listreport();
    pReport->set_ngetnum(nGetCredit);
    pReport->set_ntime(tNow);
    pReport->set_nuid(lUid);

    pRseRedInfo->set_ngetcredit(pRedEnvelope->ngiveout());
    pRseRedInfo->set_ngetcopies(pRedEnvelope->listreport_size());
    pRseRedInfo->set_bget(true);
    GrabberInfo *pGrabber = pRseRedInfo->add_listgrabber();
    pGrabber->set_ncrede(nGetCredit);
    pGrabber->set_ntime(tNow);
    pGrabber->set_strname(pUser->name());

    if (nLeft <= 0)
    {//红包发放完毕需要通知红包发送者
        Event evn;
        evn.set_cmd(C2S_RceGuildRedEnvelopeOpt);
        evn.set_state(Status_Country_To_Game);
        evn.set_time(0);
        evn.set_uid(pRedEnvelope->uid());
        RceGuildRedEnvelopeOpt *pRce = evn.mutable_ce_rceguildredenvelopeopt();
        pRce->set_ncredit(pRedEnvelope->ntotalcredit());
        pRce->set_opttype(E_GUILD_REDENVELOPE_SNAPPEOVER);
        pRce->set_nid(pRedEnvelope->nid());
        pRce->set_nguildid(nGuildID);
        pRce->set_redtype(pRedEnvelope->ntype());
        pRce->set_ncopies(pRedEnvelope->ntotalnum());
        pRce->set_ntotalcredit(pRedEnvelope->ntotalcredit());
        SYS_LOG((int64)pRedEnvelope->uid(), LT_GUILDREDENVELOPE_SNAPPEOVER, m_pRgnData->GetRegion(), 0, pGuild->GetGuildID()<<pRedEnvelope->nid()<<pRce->ncredit());
        pEH->sendEventToGamed(&evn, pEH->getDataHandler()->getGamedIdByUserId(pRedEnvelope->uid()));
    }

    pCount->set_ntotalnum(pCount->ntotalnum()+nGetCredit);
    m_mapAllUser[lUid] = pUser;
    m_pRgnData->markUserLstDirty(lUid);
    m_pRgnData->markGuildLstDirty(pGuild);

    SYS_LOG((int64)lUid, LT_GUILDREDENVELOPE_SNAPPE, m_pRgnData->GetRegion(), 0 , nGetCredit<<pGuild->GetCountry()<<pGuild->GetGuildID()<<nRedEnvelopeID<<pRedEnvelope->ntotalcredit()<<pRedEnvelope->ngiveout()<<nLeft<<nLeftCopies);
}

int CGuildRedEnvelope::AddRedEnvelope(int64 lUid, int nType, int nTotalCredit, int nCopiesNum, RseGuildRedEnvelopeOpt *pRse)
{
    if (m_nActFlag == 0)
    {
        return E_GUILD_REDENVELOPE_R_ACTOVERDUE;
    }
    DB_C_User *pUser = m_pRgnData->GetDBCityUser(lUid);
    if (pUser == NULL)
    {
        return E_GUILD_REDENVELOPE_R_UNKNOW;
    }

    if (!GuildRedEnvelopeCfg::Instance().CheckCanIssueRedEnvelope())
    {
        return E_GUILD_REDENVELOPE_R_ACTOVERDUE;
    }
    if (pUser->guildid() == 0)
    {
        return E_GUILD_REDENVELOPE_R_NOGUILD;
    }

    CGuild *pGuild = GetGuildInfo(pUser->guildid());
    if (NULL == pGuild)
    {
        pGuild = m_pRgnData->GetGuild(pUser->guildid());
    }

    if (pGuild == NULL)
    {
        return E_GUILD_REDENVELOPE_R_NOGUILD;
    }

    if (pGuild->GetGuildLV() < 2)
    {
        return E_GUILD_REDENVELOPE_R_GUILDLVLIMIT;
    }

    DB_C_GuildRedEnvelope * pGuildRedInfo = pGuild->GetDBGuildRedEnvelope();
    if (m_nActFlag != pGuildRedInfo->nflag())
    {
        pGuildRedInfo->Clear();
        pGuildRedInfo->set_nflag(m_nActFlag);
    }

    int tTime = time(NULL);
    pGuildRedInfo->set_ntotal(pGuildRedInfo->ntotal() + nTotalCredit);
    pGuildRedInfo->set_ncurrentid(pGuildRedInfo->ncurrentid() + 1);
    DB_C_RedEnvelopeInfo *pRedEnvelope = pGuildRedInfo->add_listredenvelope();
    pRedEnvelope->set_nid(pGuildRedInfo->ncurrentid());
    pRedEnvelope->set_ntotalcredit(nTotalCredit);
    pRedEnvelope->set_ntotalnum(nCopiesNum);
    pRedEnvelope->set_ntype(nType);
    pRedEnvelope->set_uid(lUid);
    pRedEnvelope->set_nissuetime(tTime);
    pRedEnvelope->set_ngiveout(0);
    pRedEnvelope->clear_listreport();
    //配表中有效时长改为分钟 2016.1.11
    int tOverdue = tTime + GuildRedEnvelopeCfg::Instance().GetLastTime() * 60;
    pRedEnvelope->set_noverduetime(tOverdue);
    int nAdjType = rand()%REDENEVELOPE_ADJTYPENUM;
    pRedEnvelope->set_nadjtype(nAdjType);     //形容词类型

    AddRedEnvelopeRecord(pGuildRedInfo->ncurrentid(), pUser->guildid(), pUser->name(), pGuild->GetGuildName(), tTime, nAdjType);

    //AddGuildRedEnvelopeInfo(pGuild);
    m_mapAllGuild[pGuild->GetGuildID()] = pGuild;
    m_mapAllUser[pUser->userid()] = pUser;
    DB_C_RedEnvelopeCount *pCountInfo = pUser->mutable_redenvelopecount();
    if (pCountInfo != NULL)
    {
        if (pCountInfo->nflag() != m_nActFlag)
        {
            pCountInfo->Clear();
            pCountInfo->set_nflag(m_nActFlag);
            pCountInfo->set_ntotalnum(0);
            pCountInfo->set_nsendtotal(0);
        }
        pCountInfo->set_nsendtotal(pCountInfo->nsendtotal() + nTotalCredit);

        m_pRgnData->markUserLstDirty(lUid);
    }

    m_pRgnData->markGuildLstDirty(pGuild);

    pRse->set_nid(pGuildRedInfo->ncurrentid());
    pRse->set_nguildid(pUser->guildid());

    SYS_LOG((int64)lUid, LT_GUILDREDENVELOPE_ISSUE, m_pRgnData->GetRegion(), 0 , nType<<nCopiesNum<<nTotalCredit<<pGuildRedInfo->ncurrentid()<<pUser->countryid()<<pUser->guildid()<<pGuildRedInfo->ntotal());
    SYS_STAT(0,"GuildRedEnvelope",0,0,false,0,0,0,0,0,m_pRgnData->GetRegion(),0,0,false,pGuild->GetCountry(),pGuild->GetGuildID(),pGuild->GetGuildLV(),nTotalCredit,0,0);
    return E_GUILD_REDENVELOPE_R_SUC;
}

void CGuildRedEnvelope::AddRedEnvelopeRecord(int nID, int nGuildID, string strName, string strGuild, int tTime, int nAdjType)
{
    RedEnvelopeReport tempReport;
    tempReport.set_nid(nID);
    tempReport.set_nguildid(nGuildID);
    tempReport.set_nissuetime(tTime);
    tempReport.set_strname(strName);
    tempReport.set_strguild(strGuild);
    tempReport.set_nadjtype(nAdjType);
    m_lstRegionBriefInfo.insert(make_pair(tTime, tempReport));
    if (m_lstRegionBriefInfo.size() > 50)
    {
        m_lstRegionBriefInfo.erase(m_lstRegionBriefInfo.begin());
    }
}

//返回公会红包
void CGuildRedEnvelope::FillGuildRedEnvelopeInfo(RseGuildRedEnvelopeOpt *pRse, int64 lUid)
{
    DB_C_User *pUser = m_pRgnData->GetDBCityUser(lUid);
    if (pUser == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_UNKNOW);
        return;
    }
    CGuild *pGuild = GetGuildInfo(pUser->guildid());
    if (pGuild == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_SUC);
        return;
    }
    int tNow = time(NULL);
    DB_C_GuildRedEnvelope * pGuildRedEnvelope = pGuild->GetDBGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        return;
    }
    for (int i = 0; i < pGuildRedEnvelope->listredenvelope_size(); i++)
    {
        DB_C_RedEnvelopeInfo *pRedEnvelopeInfo = pGuildRedEnvelope->mutable_listredenvelope(i);
        if (pRedEnvelopeInfo == NULL)
        {
            continue;
        }
        DB_C_User *pUser = m_pRgnData->GetDBCityUser(pRedEnvelopeInfo->uid());
        if (pUser == NULL)
        {
            return;
        }
        RedEnvelopeReport *pReport = pRse->add_listguild();
        pReport->set_nid(pRedEnvelopeInfo->nid());
        pReport->set_nguildid(pGuild->GetGuildID());
        pReport->set_nissuetime(pRedEnvelopeInfo->nissuetime());
        pReport->set_strname(pUser->name());
        pReport->set_nadjtype(pRedEnvelopeInfo->nadjtype());
        if (pRedEnvelopeInfo->noverduetime() <= tNow)
        {
            pReport->set_boverdue(true);
        }
        if (pRedEnvelopeInfo->ntotalnum() == pRedEnvelopeInfo->listreport_size())
        {
            pReport->set_bsnappedup(true);
        }
    }
}

//指定红包的信息
void CGuildRedEnvelope::FillRedEnveLopeInfo(RseGuildRedEnvelopeOpt *pRse, int64 lUid, int nGuildID, int nRedEnvelopeID)
{
    CGuild *pGuild = GetGuildInfo(nGuildID);
    if (pGuild == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_GUILDNOFOUND);
        return;
    }
    DB_C_GuildRedEnvelope * pGuildRedEnvelope = pGuild->GetDBGuildRedEnvelope();
    if (pGuildRedEnvelope == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }
    DB_C_RedEnvelopeInfo *pRedEnvelopeInfo = NULL;
    for (int i = 0; i < pGuildRedEnvelope->listredenvelope_size(); i++)
    {
        pRedEnvelopeInfo = pGuildRedEnvelope->mutable_listredenvelope(i);
        if (pRedEnvelopeInfo == NULL)
        {
            continue;
        }
        if (pRedEnvelopeInfo->nid() == nRedEnvelopeID)
        {
            break;
        }
        pRedEnvelopeInfo = NULL;
    }
    if (pRedEnvelopeInfo == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }
    DB_C_User *pUser = m_pRgnData->GetDBCityUser(pRedEnvelopeInfo->uid());
    if (pUser == NULL)
    {
        pRse->set_nresult(E_GUILD_REDENVELOPE_R_WRONGID);
        return;
    }
    pRse->set_nadjtype(pRedEnvelopeInfo->nadjtype());
    RedEnvelopeInfo *pRedInfo = pRse->mutable_envelopeinfo();
    pRedInfo->set_nid(pRedEnvelopeInfo->nid());
    pRedInfo->set_nguildid(nGuildID);
    pRedInfo->set_ntype(pRedEnvelopeInfo->ntype());
    pRedInfo->set_strname(pUser->name());
    pRedInfo->set_ntotalcopies(pRedEnvelopeInfo->ntotalnum());
    pRedInfo->set_ntotalcredit(pRedEnvelopeInfo->ntotalcredit());
    pRedInfo->set_ngetcopies(pRedEnvelopeInfo->listreport_size());
    pRedInfo->set_ngetcredit(pRedEnvelopeInfo->ngiveout());
    pRedInfo->set_nissuetime(pRedEnvelopeInfo->nissuetime());
    pRedInfo->set_strpic(pUser->pic());
    int tNow = time(NULL);
    if (pRedEnvelopeInfo->noverduetime() <= tNow)
    {
        pRedInfo->set_boverdue(true);
    }

    if (pRedEnvelopeInfo->ntotalnum() == pRedEnvelopeInfo->listreport_size())
    {
        pRedInfo->set_bsnappedup(true);
    }

    for (int i = 0; i < pRedEnvelopeInfo->listreport_size(); i++)
    {
        DB_C_RedEnvelopeReport *pReport = pRedEnvelopeInfo->mutable_listreport(i);
        if (pReport == NULL)
        {
            continue;
        }
        DB_C_User *pReportUser = m_pRgnData->GetDBCityUser(pReport->nuid());
        if (pReportUser == NULL)
        {
            continue;
        }
        GrabberInfo *pGrabber = pRedInfo->add_listgrabber();
        pGrabber->set_strname(pReportUser->name());
        pGrabber->set_ntime(pReport->ntime());
        pGrabber->set_ncrede(pReport->ngetnum());
        if (pReport->nuid() == lUid)
        {
            pRedInfo->set_bget(true);
            pRse->set_nsnappenum(pReport->ngetnum());
            pGrabber->set_bviewer(true);
        }
    }
}

void CGuildRedEnvelope::ClearAllRedEnveLopreInfo(CountryEventHandler* eh_, bool bClear)
{
    if (m_mapAllGuild.size() <= 0)
    {
        return;
    }
    int tNow = time(NULL);
    for (map<int, CGuild*>::iterator iter = m_mapAllGuild.begin(); iter != m_mapAllGuild.end(); iter++)
    {
        DB_C_GuildRedEnvelope * pGuildRedInfo = iter->second->GetDBGuildRedEnvelope();
        if (pGuildRedInfo == NULL)
        {
            continue;
        }
        bool bChange = false;
        for (int i = 0; i < pGuildRedInfo->listredenvelope_size(); i++)
        {
            DB_C_RedEnvelopeInfo *pRedEnvelope = pGuildRedInfo->mutable_listredenvelope(i);
            if (pRedEnvelope == NULL)
            {
                continue;
            }
            if (bClear)
            {//第一期清理数据时加上该日志 便于出问题时核对数据 后期可删除
                SYS_LOG((int64)pRedEnvelope->uid(), LT_GUILDREDENVELOPE_CLEARDATA, m_pRgnData->GetRegion(), 0, iter->second->GetGuildID()<<pRedEnvelope->nid()<<pRedEnvelope->ntype()<<pRedEnvelope->ntotalcredit()<<pRedEnvelope->ngiveout());
            }

            if (pRedEnvelope->noverduetime() <= tNow || bClear)
            {
                if (pRedEnvelope->ntotalcredit() == pRedEnvelope->ngiveout() || pRedEnvelope->brefund())
                {
                    continue;
                }
                bChange = true;
                pRedEnvelope->set_brefund(true);
                Event evn;
                evn.set_cmd(C2S_RceGuildRedEnvelopeOpt);
                evn.set_state(Status_Country_To_Game);
                evn.set_time(0);
                evn.set_uid(pRedEnvelope->uid());
                RceGuildRedEnvelopeOpt *pRce = evn.mutable_ce_rceguildredenvelopeopt();
                pRce->set_nguildid(iter->second->GetGuildID());
                pRce->set_ntotalcredit(pRedEnvelope->ntotalcredit());
                pRce->set_ncredit(pRedEnvelope->ntotalcredit() - pRedEnvelope->ngiveout());
                pRce->set_opttype(E_GUILD_REDENVELOPE_REFUND);
                pRce->set_nid(pRedEnvelope->nid());
                pRce->set_ncopies(pRedEnvelope->ntotalnum());
                pRce->set_redtype(pRedEnvelope->ntype());
                eh_->sendEventToGamed(&evn, eh_->getDataHandler()->getGamedIdByUserId(pRedEnvelope->uid()));
                SYS_LOG((int64)pRedEnvelope->uid(), LT_GUILDREDENVELOPE_REFUND, m_pRgnData->GetRegion(), 0, pRce->ncredit()<<iter->second->GetGuildID()<<pRedEnvelope->nid());
            }
        }
        if (bClear)
        {
            pGuildRedInfo->Clear();
            bChange = true;
        }
        if (bChange)
        {
            m_pRgnData->markGuildLstDirty(iter->second);
        }
    }
    if (bClear)
    {
        m_mapAllGuild.clear();
        m_mapAllUser.clear();
        m_lstRedGuildRank.clear();
        m_lstRedUserRank.clear();
        m_lstRegionBriefInfo.clear();
        m_lstRedSendUserRank.clear();
    }
}

void CGuildRedEnvelope::FillRankInfo(RseGuildRedEnvelopeOpt *pRse)
{
    for (multimap<int, RedEnvelopeReport>::iterator iter = m_lstRegionBriefInfo.begin(); iter != m_lstRegionBriefInfo.end(); iter++)
    {
        RedEnvelopeReport *pReport = pRse->add_listregion();
        pReport->CopyFrom(iter->second);
    }
    for (std::list<RedEnvelopeRankInfo>::iterator iter = m_lstRedGuildRank.begin(); iter != m_lstRedGuildRank.end(); iter++)
    {
        RedEnvelopeRankInfo * pRank = pRse->add_rankguild();
        pRank->CopyFrom(*iter);
    }

    for (std::list<RedEnvelopeRankInfo>::iterator iter = m_lstRedUserRank.begin(); iter != m_lstRedUserRank.end(); iter++)
    {
        RedEnvelopeRankInfo * pRank = pRse->add_rankpersonal();
        pRank->CopyFrom(*iter);
    }

    for (std::list<RedEnvelopeRankInfo>::iterator iter = m_lstRedSendUserRank.begin(); iter != m_lstRedSendUserRank.end(); iter++)
    {
        RedEnvelopeRankInfo *pRank = pRse->add_ranksendpersonal();
        pRank->CopyFrom(*iter);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

CountryRgnDatHandler::CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion)
:m_pDH(pDH)
,m_nRegion(nRegion)
,m_earthCountry(0,m_nRegion)
,m_bHasLaunchAutoBloodBtl(false)
,m_bCleanArmRaceData(false)
,m_ResourceData(m_nRegion)
{

	m_mapBloodBtlStep.clear();
	m_mapGuildName.clear();
	m_ltLastRank = time(NULL);
	m_ltLastStat = time(NULL);
	m_mapUserUpdateFlag.clear();
	m_ltLastAllGuildGveRank = time(NULL);
	m_nFlag = 0;
	m_nGvgFlag = 0;
	m_ltLastGvgCheck = 0;
	m_ltGvgLastStat = 0;
	m_ltWpcRankCheck = 0;
	m_strChampionUid.clear();
	m_ltLeagueCheck = 0;
	m_mapLeagueBattle.clear();

	m_lstBossBtlRank.clear();
	m_ltBossCheck = 0;

	m_lstBossBtlRecord.clear();

	m_nBossBtlTick = 0;
	m_ltBossLiteCheck = 0;

	for ( int i = 0; i < BOSSBTL_UP_CNT; i++ )
	{
		m_lstUpList[i].clear();
	}

    m_ltSortRank = 0;
    m_ltSendRank = 0;
    m_ltCheckOverDue = 0;

	m_ltLastUpdateLabSearchRankTime = 0;
	for(int i = 0; i < GUILD_BTL_DAY_CNT; i++){
		m_GuildBtlReceive[i] = 0;
	}
	m_pMainUserList = NULL;
}

CountryRgnDatHandler::~CountryRgnDatHandler()
{

}

void CountryRgnDatHandler::Clear()
{

}

bool compareGuild(CGuild* pGuild1, CGuild* pGuild2)
{
	if(pGuild1->GetGuildLV() > pGuild2->GetGuildLV())
		return true;
	else if(pGuild1->GetGuildLV() < pGuild2->GetGuildLV())
		return false;
	else 
	{
		return pGuild1->GetDBGuild()->totalbldvalue() > pGuild2->GetDBGuild()->totalbldvalue();
	}
}

bool compartPKGuild(CGuild* pGuild1, CGuild* pGuild2)
{
	if(pGuild1->GetGuildPKScore() > pGuild2->GetGuildPKScore())
		return true;
	else if(pGuild1->GetGuildPKScore() < pGuild2->GetGuildPKScore())
		return false;

	if(pGuild1->GetGuildLV() > pGuild2->GetGuildLV())
		return true;
	else if(pGuild1->GetGuildLV() < pGuild2->GetGuildLV())
		return false;
	else 
	{
		return pGuild1->GetDBGuild()->totalbldvalue() > pGuild2->GetDBGuild()->totalbldvalue();
	}
}

bool compareGveMsg(DB_GVE_Msg* pDBGve1,DB_GVE_Msg* pDBGve2,CGuild* pGuild1, CGuild* pGuild2)
{
	if ( pDBGve1 == NULL || pDBGve2 == NULL || pGuild1 == NULL || pGuild2 == NULL )
	{
		return false;
	}

	if ( pDBGve1->point() > pDBGve2->point() )
	{
		return true;
	}
	else if ( pDBGve1->point() < pDBGve2->point() )
	{
		return false;
	}

	if ( pDBGve1->gvestep() > pDBGve2->gvestep() )
	{
		return true;
	}
	else if ( pDBGve1->gvestep() < pDBGve2->gvestep() )
	{
		return false;
	}

	if(pGuild1->GetGuildLV() > pGuild2->GetGuildLV())
		return true;
	else if ( pGuild1->GetGuildLV() < pGuild2->GetGuildLV())
	{
		return false;
	}

	if ( pGuild1->GetGuildID() > pGuild2->GetGuildID())
	{
		return true;
	}
	else
		return false;
}

bool compareGuildGve(CGuild* pGuild1, CGuild* pGuild2)
{
	if ( pGuild1 == NULL || pGuild2 == NULL )
	{
		return true;
	}

	DB_GVE_Msg* pDBGve1 = pGuild1->GetDBGuild()->mutable_gvemsg()->mutable_nowmsg();
	DB_GVE_Msg* pDBGve2 = pGuild2->GetDBGuild()->mutable_gvemsg()->mutable_nowmsg();

	pGuild1->m_nPoint = pDBGve1->point();
	pGuild2->m_nPoint = pDBGve2->point();

	return compareGveMsg(pDBGve1,pDBGve2,pGuild1,pGuild2);
// 	else 
// 	{
// 		return pGuild1->GetDBGuild()->mutable_gvemsg()->lastrefreshtime() < pGuild2->GetDBGuild()->mutable_gvemsg()->lastrefreshtime();
// 	}
}

bool compareGuildGveLast(CGuild* pGuild1, CGuild* pGuild2)
{
	if ( pGuild1 == NULL || pGuild2 == NULL )
	{
		return false;
	}

	DB_GVE_Msg* pDBGve1 = pGuild1->GetDBGuild()->mutable_gvemsg()->mutable_lastmsg();
	DB_GVE_Msg* pDBGve2 = pGuild2->GetDBGuild()->mutable_gvemsg()->mutable_lastmsg();

	if ( pDBGve1 == NULL || pDBGve2 == NULL )
	{
		return false;
	}

	return compareGveMsg(pDBGve1,pDBGve2,pGuild1,pGuild2);
	// 	else 
	// 	{
	// 		return pGuild1->GetDBGuild()->mutable_gvemsg()->lastrefreshtime() < pGuild2->GetDBGuild()->mutable_gvemsg()->lastrefreshtime();
	// 	}
}

bool compareGuildGveHighest(CGuild* pGuild1, CGuild* pGuild2)
{
	if ( pGuild1 == NULL || pGuild2 == NULL )
	{
		return false;
	}

	DB_GVE_Msg* pDBGve1 = pGuild1->GetDBGuild()->mutable_gvemsg()->mutable_highestmsg();
	DB_GVE_Msg* pDBGve2 = pGuild2->GetDBGuild()->mutable_gvemsg()->mutable_highestmsg();

	if ( pDBGve1 == NULL || pDBGve2 == NULL )
	{
		return false;
	}

	return compareGveMsg(pDBGve1,pDBGve2,pGuild1,pGuild2);
	// 	else 
	// 	{
	// 		return pGuild1->GetDBGuild()->mutable_gvemsg()->lastrefreshtime() < pGuild2->GetDBGuild()->mutable_gvemsg()->lastrefreshtime();
	// 	}
}

bool compareWpcRank(DB_C_User* pDBUser1, DB_C_User* pDBUser2)
{
	if( pDBUser1->wpcsearchtime() > pDBUser2->wpcsearchtime() )
		return true;
	else if( pDBUser1->wpcsearchtime() < pDBUser2->wpcsearchtime() )
		return false;
	else if ( pDBUser1->userid() > pDBUser2->userid() )
		return true;
	else 
		return false;
}

bool conpareArmRaceRank(DB_C_User* pDBUser1, DB_C_User* pDBUser2)
{
	if( pDBUser1->wpclabsearchcnt() > pDBUser2->wpclabsearchcnt() )
		return true;
	else if( pDBUser1->wpclabsearchcnt() < pDBUser2->wpclabsearchcnt() )
		return false;
	else if ( pDBUser1->userid() > pDBUser2->userid() )
		return true;
	else 
		return false;
}

bool compareBossBtlRank(DB_C_User* pDBUser1, DB_C_User* pDBUser2)
{
	if( pDBUser1->bossbtldmg() > pDBUser2->bossbtldmg() )
		return true;
	else if ( pDBUser1->bossbtldmg() < pDBUser2->bossbtldmg() )
		return false;
	else if ( pDBUser1->userid() > pDBUser2->userid() )
		return true;
	else 
		return false;
}

bool compareBossBtlHighRank(DB_C_BossBtlRankUnit pDBRank1, DB_C_BossBtlRankUnit pDBRank2)
{
	if( pDBRank1.dmg() > pDBRank2.dmg() )
		return true;
	else if ( pDBRank1.dmg() < pDBRank2.dmg() )
		return false;
	else if ( pDBRank1.uid() > pDBRank2.uid() )
		return true;
	else 
		return false;
}

bool compareBossBtlUpCnt(DB_C_BossBtlRankUnit pDBRank1, DB_C_BossBtlRankUnit pDBRank2)
{
	if( pDBRank1.count() > pDBRank2.count() )
		return true;
	else if ( pDBRank1.count() < pDBRank2.count() )
		return false;
	else if ( pDBRank1.uid() > pDBRank2.uid() )
		return true;
	else 
		return false;
}


void CountryRgnDatHandler::InitCountry()
{
	//合服标记为，如果该值不为零切与DB值不符，则进行合服操作
	int nCombineRegionFlag = serverConfig.getRegionResetFlag(m_nRegion);
	/**************************************************************************
	合服操作国家服务器相关
	1.被合并区将不再创建
	2.城市恢复初始设置，所有人回首都
	3.官员清档
	4.战报清档，血战清档
	5.公会相关
	****************************************************************************/

	ResetEmptyGuildIDLst();

	Clear();

    m_GuildRedEnvelope.SetCountryRgnData(this);

	bool bCombineRegionOpt = LoadAllCountryInfo(nCombineRegionFlag);
	if(bCombineRegionOpt)
	{//处理国家合服操作
		DealCombineRegion_Country();
	}
	LoadAllCityInfo(bCombineRegionOpt);	
	LoadAllGuildInfo(bCombineRegionOpt);
	LoadAllUserInfo(bCombineRegionOpt);
	LoadAllResourceInfo(bCombineRegionOpt);
	LoadAllGvgCity(bCombineRegionOpt);
    LoadAllWarGameInfo(bCombineRegionOpt);
	LoadBossBtlMsg(bCombineRegionOpt);
	

    int nTempSize = m_lstGuild[C_AllCountry].size();
    if (nTempSize > 0)
    {
        for (int i = 0; i < nTempSize; i++)
        {
            CGuild *pGuild = m_lstGuild[C_AllCountry][i];
            if (pGuild != NULL)
            {
                m_GuildRedEnvelope.AddGuildRedEnvelopeInfo(pGuild);
            }
        }
    }

	if(bCombineRegionOpt)
	{//合服操作结束，存储数据
		CCountry* pEarth = GetEarthCountry();
		pEarth->GetDBCountry()->set_dataresetflag(nCombineRegionFlag);
		markCountryDirty(pEarth);
		for(int j=C_Start;j<C_UserCoutryEnd;j++)
		{
			if(!IsValidCountry(j))
				continue;
			CCountry* pCountry = GetCountry(j);
			markCountryDirty(pCountry);
		}
		map<int, CCity*>::iterator itCt;
		for(itCt = m_mapCity.begin();itCt!=m_mapCity.end();itCt++)
		{
			markCityDirty(itCt->second);
		}
		map<int,DB_C_GuildList*>::iterator itGld;
		for(itGld = m_mapGuildLst.begin();itGld!=m_mapGuildLst.end();itGld++)
		{
			m_pDH->markGuildLstDirty(itGld->second);
		}
		map<int,DB_C_UserList*>::iterator itUser;
		for(itUser = m_mapUserLst.begin();itUser!= m_mapUserLst.end();itUser++)
		{
			markUserLstDirty(itUser->second);
		}
	}

	//数据校验
	CheckClearCountryArenaData();
	
	//校验工会数据
	CheckGuild();

	//工会根据等级排序
	SortGuild();
	SortPKGuild();
	//检查国家和首都
	CCountry* pEarth = GetEarthCountry();
	if(pEarth==NULL)
	{
		printf("ERROR!!!! can't find country:0,region:%d..............\r\n",m_nRegion);
		exit(0);
	}

	for(int j=C_Start;j<C_UserCoutryEnd;j++)
	{
		if(!IsValidCountry(j))
			continue;
		CCountry* pCountry = GetCountry(j);
		if(pCountry==NULL)
		{//
			printf("ERROR!!!! can't find country:%d,region:%d..............\r\n",j,m_nRegion);
			exit(0);
		}
		if(pCountry->GetCapital()==NULL)
		{
			printf("ERROR!!!! country:%d,region:%d can't find capital..............\r\n",j,m_nRegion);
			exit(0);
		}

		//初始化国家竞技场数据
		if(pCountry->UpdateRankNpc() == true)
		{
			markCountryDirty(pCountry);
		}
		pCountry->UpdateArenaList(this);
	}
	//初始化世界竞技场
	if(pEarth->UpdateWorldRankNpc()==true)
	{
		markCountryDirty(pEarth);
	}
	pEarth->UpdateWorldArenaList(this);
	UpdateWorldArenaData();

	//初始化英雄竞技场
	if(pEarth->UpdateHeroRankNpc()==true)
	{
		markCountryDirty(pEarth);
	}
	pEarth->UpdateHeroArenaList(this);
	UpdateHeroArenaData();

	//初始化工会战数据
	CheckGuildBtlData();

	ResetGveGuildRank();

	LoadWpcRankUser(bCombineRegionOpt);

	LoadLeagueBattle(bCombineRegionOpt);

	//一元购数据初始化
	InitOneYuanPurchaseData();
}



void CountryRgnDatHandler::CheckGuildBtlData()
{
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	time_t ltNow = time(NULL);
	int nSession = 0,nStep = 0;
	GuildInfoCfg::Instance().GetGuildPKSession(ltNow,nSession,nStep);
	if(nSession != pDBGBtl->cursession())
	{//新的一期
		pDBGBtl->set_cursession(nSession);
		pDBGBtl->set_cursesionbtlover(false);
		markCountryDirty(pCountry);
		//清空所有工会站积分
		for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
		{
			CGuild* pTarGuild = m_lstPKGuild[C_AllCountry][i];
			DB_C_GuildBtl* pDBGBtl = GetDBGuildBtl(pTarGuild);
			if(pDBGBtl->totalscore()>0)
			{
				pDBGBtl->set_totalscore(0);
				pDBGBtl->set_btlcnt(0);
				markGuildLstDirty(pTarGuild);
			}
		}
		SortPKGuild();
	}
	tm tmNow = *localtime(&ltNow);
	int nNow = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
	if(nNow != pDBGBtl->btlday())
	{//新一天
		pDBGBtl->set_btlday(nNow);
		pDBGBtl->set_pkstep(0);
		pDBGBtl->clear_curpkover();
		pDBGBtl->clear_curaddscore();
		pDBGBtl->clear_pklst();
		pDBGBtl->set_pklstidx(0);
		for (int i = 0; i < GUILD_BTL_DAY_CNT; i++){
			m_GuildBtlReceive[i] = 0;
		}
		markCountryDirty(pCountry);
	}
}

void CountryRgnDatHandler::ResetGuildPKScore(int nScore)
{
    for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
    {
        CGuild* pTarGuild = m_lstPKGuild[C_AllCountry][i];
        DB_C_GuildBtl* pDBGBtl = GetDBGuildBtl(pTarGuild);
        pDBGBtl->set_totalscore(nScore);
        pDBGBtl->set_btlcnt(nScore/4);
        markGuildLstDirty(pTarGuild);
    }
    SortPKGuild();
}

void CountryRgnDatHandler::CheckGuild()
{//
	map<int,CGuild*>::iterator iter;
	for(iter = m_mapGuild.begin();iter!=m_mapGuild.end();iter++)
	{
		CGuild* pGuild = iter->second;
		if(pGuild==NULL||pGuild->GetGuildID()<=0)
			continue;
		int64 nLeaderID = pGuild->GetDBGuild()->leaderid();
		DB_C_User* pDBCUser = GetDBCityUser(nLeaderID);
		if(pDBCUser==NULL)
		{//出错		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_CityError<<nSelfCity<<pCity->GetCountry()<<pCountry->GetCountryID());

			SYS_LOG(nLeaderID,LT_CNTR_ERROR,m_nRegion,0,CntrError_NoLeader<<pGuild->GetGuildID()<<pGuild->GetCountry());
			continue;
		}
		if(pDBCUser->guildid()!=pGuild->GetGuildID())
		{//会长ID出错
			if(pDBCUser->countryid() != pGuild->GetCountry())
			{
				SYS_LOG(nLeaderID,LT_CNTR_ERROR,m_nRegion,0,CntrError_LeaderError<<pGuild->GetGuildID()<<pGuild->GetCountry());
			}
			else
			{
				int nOldGID = pDBCUser->guildid();
				CGuild* pOldGuild = GetGuild(nOldGID);
				if(pOldGuild)
				{
					if(pOldGuild->GetDBGuild()->leaderid()==nLeaderID)
					{//出错
						SYS_LOG(nLeaderID,LT_CNTR_ERROR,m_nRegion,0,CntrError_GuildLeaderError<<pGuild->GetGuildID()<<pGuild->GetCountry()<<nOldGID);
						continue;
					}
					pOldGuild->EraseUser(nLeaderID);
				}
				pGuild->AddUser(pDBCUser);
				pDBCUser->set_guildid(pGuild->GetGuildID());
				pDBCUser->set_position(GP_Leader);
				SYS_LOG(nLeaderID,LT_CNTR_ERROR,m_nRegion,0,CntrError_LeaderError<<pGuild->GetGuildID()<<pGuild->GetCountry()<<nOldGID);

			}
		}
	}
}

void CountryRgnDatHandler::GuildStat(CountryEventHandler* eh_)
{
	static int nOneHour = 3600;
	time_t ltNow = time(NULL);
	static int nMaxGuildLV = 11;
	int BldCnt[GB_Cnt][C_UserCoutryEnd][11];
	memset(BldCnt,0,GB_Cnt*C_UserCoutryEnd*nMaxGuildLV*sizeof(int));

	if(ltNow - m_ltLastStat > nOneHour)
	{
		m_ltLastStat = ltNow;
		for(int i=C_Start;i<C_UserCoutryEnd;i++)
		{
			vector<CGuild*>::iterator iter;
			for(iter = m_lstGuild[i].begin();iter!=m_lstGuild[i].end();iter++)
			{
				CGuild* pGuild = *iter;
				if(pGuild==NULL||pGuild->GetGuildID()<=0)
					continue;
				DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
				for(int j=0;j<pDBGuild->build_size()&&j<GB_Cnt;j++)
				{
					DB_C_GuildBld* pDBGBld = pDBGuild->mutable_build(j);
					int nLV = pDBGBld->level();
					if(nLV<0||nLV>=nMaxGuildLV)
						continue;
					BldCnt[j][i][nLV] ++;
				}
			}	
		}
		Event evn;
		evn.set_cmd(EVENT_GUILD_STAT);
		evn.set_state(Status_Game_To_Country);
		evn.set_time(0);
		evn.set_uid(0);
		GCG_GuildStat* pStat = evn.mutable_guildstat();
		for(int j=0;j<GB_Cnt;j++)
		{
			string strStat;
			for(int nLV=0;nLV<nMaxGuildLV;nLV++)
			{
				for(int i=C_Start;i<C_UserCoutryEnd;i++)
				{
					char szTmp[256];
					int nCnt = BldCnt[j][i][nLV];
					sprintf(szTmp,"%d-%d|",i,nCnt);
					strStat += szTmp;
				}
				strStat += ",";
			}
			pStat->add_bldid(j);
			pStat->add_region(GetRegion());
			pStat->add_strstat(strStat);
		}
		eh_->sendEventToGamed(&evn,1);
	}
}
void CountryRgnDatHandler::CheckGuildRankSort()
{
	static int nOneHour = 3600;
	time_t ltNow = time(NULL);
	if(ltNow - m_ltLastRank > nOneHour)
	{
		SortGuild();
	}
}

int CountryRgnDatHandler::GetGuildPKTurns()
{
	GuildBtlStepType emStep = GetGuildBtlStep();
	switch(emStep)
	{	
	case GBtlType_UnKnow:
	case GBtlType_SignUp:
	case GBtlType_LineUp0:
	case GBtlType_Btl0:
		return 0;
	case GBtlType_LineUp1:
	case GBtlType_Btl1:
		return 1;
	case GBtlType_LineUp2:
	case GBtlType_Btl2:
	case GBtlType_Over:
		return 2;
	default:
		return 0;
	}
}

void CountryRgnDatHandler::SetTurnsAddScore(int nTurns,bool bAdd)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return;
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	for(int i=pDBGBtl->curaddscore_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBGBtl->add_curaddscore(false);
	}
	pDBGBtl->set_curaddscore(nTurns,bAdd);
}

bool CountryRgnDatHandler::HasTurnsAddScore(int nTurns)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return true;
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	for(int i=pDBGBtl->curaddscore_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBGBtl->add_curaddscore(false);
	}
	return pDBGBtl->curaddscore(nTurns);
}

void CountryRgnDatHandler::SetCurTurnsPKOver(int nTurns,bool bOver)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return;

	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	for(int i=pDBGBtl->curpkover_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBGBtl->add_curpkover(false);
	}
	pDBGBtl->set_curpkover(nTurns,bOver);
}

bool CountryRgnDatHandler::IsCurTurnsPKOver(int nTurns)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return true;
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	for(int i=pDBGBtl->curpkover_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBGBtl->add_curpkover(false);
	}
	return pDBGBtl->curpkover(nTurns);
}
void CountryRgnDatHandler::SetGuildBtlSetp(GuildBtlStepType emStep)
{
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	pDBGBtl->set_pkstep(emStep);
}

GuildBtlStepType CountryRgnDatHandler::GetGuildBtlStep()
{
	CCountry* pCountry = GetEarthCountry();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_PubGuildBtlInfo* pDBGBtl = pDBCountry->mutable_pubguildbtlinfo();
	return (GuildBtlStepType)pDBGBtl->pkstep();
}
void CountryRgnDatHandler::BalanceGuildBtlTurns(int nTurns,CountryEventHandler* eh)
{
	if(nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return;
	if(HasTurnsAddScore(nTurns))
		return;
	int nMinUserCnt = GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinJionCnt;
	for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
	{
		CGuild* pGuild = m_lstPKGuild[C_AllCountry][i];
		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
		if(pDBGuild->mutable_btlinfo()->signupcnt()<nMinUserCnt)
			continue;
		DB_C_GuildBtlResult* pDBGRet =  GetDBGuildBtlResult(pGuild,nTurns);
		if(pDBGRet==NULL||pDBGRet->addflag()||pDBGRet->score()<=0)
			continue;
		DB_C_GuildBtl* pDBGBtl = pDBGuild->mutable_btlinfo();
		pDBGBtl->set_totalscore(pDBGBtl->totalscore()+pDBGRet->score());
		pDBGRet->set_addflag(true);
		markGuildLstDirty(pGuild);
	}
	SetTurnsAddScore(nTurns,true);
	markCountryDirty(&m_earthCountry);

	SortPKGuild();

	if(nTurns<2)
		return;

	//如果是第三轮，增加日志
	for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
	{
		CGuild* pTarGuild = m_lstPKGuild[C_AllCountry][i];
		DB_C_GuildBtl* pDBBtl = GetDBGuildBtl(pTarGuild);
		if(pDBBtl->totalscore()<=0)
			continue;
		AddGuildLog_BtlRank(pTarGuild,G_Log_BtlRank,pTarGuild->m_nAllGuildPKRank);
		markGuildLstDirty(pTarGuild);
		SYS_LOG(pTarGuild->GetGuildID(),LT_GBtl_Rank,GetRegion(),0,pTarGuild->GetGuildID()<<pTarGuild->GetCountry()<<pTarGuild->m_nAllGuildPKRank<<pTarGuild->m_nCountryGuildPKRank<<pDBBtl->signupcnt());
	}

	int nSession=0;
	int nStep = 0;
	time_t ltNow = time(NULL);
	GuildInfoCfg::Instance().GetGuildPKSession(ltNow,nSession,nStep);
	//如果是本届的最后一天，添加历史记录
	if(GuildInfoCfg::Instance().IsGuildBtlFinishDay(nSession,ltNow))
	{
		CCountry* pECountry = GetEarthCountry();
		DB_C_PubGuildBtlInfo* pDBPGB = pECountry->GetDBCountry()->mutable_pubguildbtlinfo();
		pDBPGB->set_cursesionbtlover(true);
		//清除老的官员数据
		for(int j=C_Start;j<C_UserCoutryEnd;j++)
		{
			CCountry* pCountry = GetCountry(j);
			if(pCountry)
			{
				ClearCountryOfficer(pCountry,E_Officer_Type_Guild);
				markCountryDirty(pCountry);
			}
		}
		for(int j=C_AllCountry;j<C_UserCoutryEnd;j++)
		{
			CCountry* pCountry = GetCountry(j);
			if(pCountry)
			{
				PushGuildBtlRecord(pCountry,nSession);
				markCountryDirty(pCountry);
			}
		}
		CheckCountryUserUpdateFlag(eh);
		eh->SendCountryLiteInfo();
		//添加工会战仓库物品
		for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
		{
			CGuild* pTarGuild = m_lstPKGuild[C_AllCountry][i];
			DB_C_GuildBtl* pDBBtl = GetDBGuildBtl(pTarGuild);
			if(pDBBtl->btlcnt()<=0)
				continue;
			if(pTarGuild->m_nAllGuildPKRank==1)
			{
				//发送结束广播,排名第一的是大总统
				eh->BroadCastMsg_GuildBtlSessionOver(GetRegion(),pTarGuild->GetGuildName(),pTarGuild->GetLeaderName());
			}
			pDBBtl->set_lastbtlsession(nSession);
			pDBBtl->set_lastbtlrank(pTarGuild->m_nAllGuildPKRank);
			AddGuildBtlReward(pTarGuild,pTarGuild->m_nAllGuildPKRank);	
		}
	}
}

void CountryRgnDatHandler::AddGuildBtlReward(CGuild* pGuild,int nRank)
{
	if(pGuild == NULL)
		return;
	const GuildRewardTbl* pRewardTbl = GuildInfoCfg::Instance().GetGuildBtlRedwardTbl(nRank);
	if(pRewardTbl)
	{
		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
		SYS_LOG(pGuild->GetGuildID(),LT_GBtl_SesionReward,GetRegion(),0,pGuild->GetGuildID()
			<<pGuild->GetCountry()<<pGuild->m_nAllGuildPKRank<<pGuild->m_nCountryGuildPKRank
			<<pRewardTbl->m_nItemID<<pRewardTbl->m_nItemCnt<<pRewardTbl->m_nGuildBldValue);

		bool bFind = false;
		for(int j=0;j<pDBGuild->storage_size();j++)
		{
			DB_C_GuildStorage* pDBStorage = pDBGuild->mutable_storage(j);
			if(pDBStorage->itemid() == pRewardTbl->m_nItemID)
			{
				pDBStorage->set_itemcnt(pDBStorage->itemcnt()+pRewardTbl->m_nItemCnt);
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			DB_C_GuildStorage* pDBStorage = pDBGuild->add_storage();
			pDBStorage->set_itemid(pRewardTbl->m_nItemID);
			pDBStorage->set_itemcnt(pRewardTbl->m_nItemCnt);
		}
		pDBGuild->set_bldvalue(pDBGuild->bldvalue()+pRewardTbl->m_nGuildBldValue);
		AddGuildLog_BtlRankReward(pGuild,G_Log_RankReward,pRewardTbl->m_nItemID,pRewardTbl->m_nItemCnt,pRewardTbl->m_nGuildBldValue,nRank,pGuild->m_nCountryGuildPKRank);
		markGuildLstDirty(pGuild);
	}	
}

void CountryRgnDatHandler::PushGuildBtlRecord(CCountry* pCountry,int nSession)
{
	if(pCountry==NULL)
		return;
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	DB_C_GuildBtl_Record* pDBRecored = NULL;
	for(int i=0;i<pDBCountry->gbtlrecord_size();i++)
	{
		pDBRecored = pDBCountry->mutable_gbtlrecord(i);
		if(pDBRecored->seasons()==nSession)
		{
			break;
		}
		else
		{
			pDBRecored = NULL;
		}
	}
	if(pDBRecored == NULL)
	{
		pDBRecored = pDBCountry->add_gbtlrecord();
	}
	pDBRecored->set_seasons(nSession);
	pDBRecored->clear_guilddata();
	int nCountry = pCountry->GetCountryID();
	int nRank = 1;
	for(int i=0;i<HISTORT_RANK_USER_CNT&&i<m_lstPKGuild[nCountry].size();i++)
	{
		CGuild* pGuild = m_lstPKGuild[nCountry][i];
		DB_C_GuildBtl* pDBGBtl = pGuild->GetDBGuild()->mutable_btlinfo();
		DB_C_GuildBtl_Top_Guild* pDBRec = pDBRecored->add_guilddata();
		pDBRec->set_guildid(pGuild->GetGuildID());
		pDBRec->set_guildname(pGuild->GetGuildName());
		pDBRec->set_guildlv(pGuild->GetGuildLV());
		pDBRec->set_countryid(pGuild->GetCountry());
		pDBRec->set_usercnt(pGuild->GetUserCnt());
		pDBRec->set_btlscore(pDBGBtl->totalscore());
		pDBRec->set_leadername(pGuild->GetLeaderName());

		//工会战排名产生官员
		DB_C_User* pDBCUser = GetDBCityUser(pGuild->GetLeaderID());
		if(pDBCUser)
		{
			UpdateOfficerPosition(pDBCUser,i,nCountry,E_Officer_Type_Guild,pGuild);
		}
	}
}

DB_C_GuildBtl_Record* CountryRgnDatHandler::GetGuildBtlRecord(CCountry* pCountry,int nSession)
{
	if(pCountry==NULL)
		return NULL;
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	for(int i=0;i<pDBCountry->gbtlrecord_size();i++)
	{
		DB_C_GuildBtl_Record* pDBRecored = pDBCountry->mutable_gbtlrecord(i);
		if(pDBRecored->seasons()==nSession)
		{
			return pDBRecored;
		}
	}
	return NULL;
}

void CountryRgnDatHandler::SortPKGuild()
{
	for(int i=C_AllCountry;i<C_UserCoutryEnd;i++)
	{
		sort(m_lstPKGuild[i].begin(),m_lstPKGuild[i].end(),compartPKGuild);
	}
	int nRank = 1;
	vector<CGuild*>::iterator iter;
	for(iter = m_lstPKGuild[C_AllCountry].begin();iter!=m_lstPKGuild[C_AllCountry].end();iter++)
	{
		CGuild* pGuild = *iter;
		if(pGuild==NULL)
			continue;
		pGuild->m_nAllGuildPKRank = nRank++;
	}

	for(int i=C_Start;i<C_UserCoutryEnd;i++)
	{
		int nRank = 1;
		vector<CGuild*>::iterator iter;
		for(iter = m_lstPKGuild[i].begin();iter!=m_lstPKGuild[i].end();iter++)
		{
			CGuild* pGuild = *iter;
			if(pGuild==NULL)
				continue;
			pGuild->m_nCountryGuildPKRank = nRank++;
		}
	}
}

void CountryRgnDatHandler::SortGuild()
{
	for(int i=C_AllCountry;i<C_UserCoutryEnd;i++)
	{
		sort(m_lstGuild[i].begin(),m_lstGuild[i].end(),compareGuild);
	}

	int nRank = 1;
	vector<CGuild*>::iterator iter;
	for(iter = m_lstGuild[C_AllCountry].begin();iter!=m_lstGuild[C_AllCountry].end();iter++)
	{
		CGuild* pGuild = *iter;
		if(pGuild==NULL)
			continue;
		pGuild->m_nAllGuildRank = nRank++;
	}

	for(int i=C_Start;i<C_UserCoutryEnd;i++)
	{
		int nRank = 1;
		vector<CGuild*>::iterator iter;
		for(iter = m_lstGuild[i].begin();iter!=m_lstGuild[i].end();iter++)
		{
			CGuild* pGuild = *iter;
			if(pGuild==NULL)
				continue;
			pGuild->m_nCountryGuildRank = nRank++;
		}
	}
	m_ltLastRank = time(NULL);
}


void CountryRgnDatHandler::ResetEmptyGuildIDLst()
{
	m_lstEmptyGuildID.clear();
	for(int i=0;i<10000;i++)
	{
		int nID = rand()%99000 + 1000; // id<1000的不会出现
		m_lstEmptyGuildID.push_back(nID);
	}
}


bool CountryRgnDatHandler::LoadAllCountryInfo(int nCombineRegionFlag)
{
	//城市在启动的时候，将所有的城市和国家信息加载进内存
	bool bLoadError = false;
	bool bCombineRegionOpt = false;
	int nKey = 0;
	char szKey[128];

	//加载通过国家数据
	nKey = m_earthCountry.GetKey();
	sprintf(szKey,"Country_%d",nKey);
	bool bGet = m_pDH->LoadCountryFromDB(szKey,m_earthCountry.m_dbCountry,bLoadError);
	m_earthCountry.m_dbCountry.set_countryid(0);
	m_earthCountry.m_dbCountry.set_region(m_nRegion);

	//通过世界服务器判断是否合服
	if(nCombineRegionFlag >0 && m_earthCountry.m_dbCountry.dataresetflag() != nCombineRegionFlag)
	{
		bCombineRegionOpt = true;
	}

	time_t ltNow = time(NULL);
	//加载国家数据
	for(int j=C_Start;j<C_CountryCnt;j++)
	{
		if(!IsValidCountry(j))
			continue;
		CCountry* pCountry = new CCountry(j,m_nRegion);
		int nKey = pCountry->GetKey();
		sprintf(szKey,"Country_%d",nKey);
		bool bGet = m_pDH->LoadCountryFromDB(szKey,pCountry->m_dbCountry,bLoadError);
		if(bLoadError)
		{//连接数据库失败，直接退出，不会走到这里
			exit(0);
		}
		//if(!bGet)
		//{
			pCountry->m_dbCountry.set_countryid(j);
			pCountry->m_dbCountry.set_region(m_nRegion);
		//}
		
		m_mapCountry[pCountry->GetKey()] = pCountry;
	}

	return bCombineRegionOpt;
}

void CountryRgnDatHandler::LoadAllCityInfo(bool bCombineRegionOpt/* = false*/)
{
	bool bLoadError = false;
	int nKey = 0;
	//char szKey[128];
	time_t ltNow = time(NULL);

	vector<int>::iterator iter;
	const vector<int> lstCityID = CountryInfoCfg::Instance().GetCityLst();

	for(int i=0;i<(int)lstCityID.size();i++)
	{
		int nCityID = lstCityID[i];
		const CityDataTbl* pCityTbl = CountryInfoCfg::Instance().GetCityTbl(nCityID);
		if(pCityTbl==NULL)
			continue;

		CCountry*	pCountry = NULL;

		CCity* pCity = new CCity();
		pCity->m_bUserCaptial	= CountryInfoCfg::Instance().IsCapital(nCityID);
		int nKey = GetCityKey(nCityID);
		char szKey[128];
		sprintf(szKey,"City_%d",nKey);
		bool bGet = m_pDH->LoadCityFromDB(szKey,pCity->m_dbCity,bLoadError);
		if(bLoadError)
		{//连接数据库失败，直接退出，不会走到这里
			exit(0);
		}
		if(bGet)
		{
			//此处临时代码 begin by sskylin
			//const CountryTmpChgTbl* pTmpTbl = CountryInfoCfg::Instance().GetCityTmpChg(m_nRegion,nCityID);
			//time_t ltSaveTime = pCity->GetDBCity()->ltsaverevision()/1000;
			//if(pTmpTbl&&(ltSaveTime < pTmpTbl->m_ltTime))
			//{
			//	int nOldCountry = pCity->GetCountry();
			//	pCity->GetDBCity()->set_countryid(pTmpTbl->m_nCountry);
			//	SYS_LOG(0,LT_CNTR_ERROR,0,0,CntrError_TmpChg0506<<m_nRegion<<nCityID<<pTmpTbl->m_nCountry<<pTmpTbl->m_nErrCountry<<nOldCountry);
			//	
			//	markCityDirty(pCity);
			//}
			//此处临时代码 end
			if(bCombineRegionOpt)
			{//合服操作，城市回归配表属性
				DB_C_City* pDBCity = pCity->GetDBCity();

				pDBCity->clear_npc();
				pDBCity->clear_npcinittm();
				pDBCity->clear_lastbloodbtl();
				pDBCity->clear_officerrightuselst();

				pDBCity->set_countryid(pCityTbl->m_nCountry);
				pDBCity->set_region(m_nRegion);
				pDBCity->set_cityid(pCityTbl->m_nID);
			}

			if(pCountry==NULL)
			{
				pCountry = GetCountry(pCity->GetCountry());
				if(pCountry==NULL)
				{
					delete pCity;
					continue;
				}
			}
			pCountry->AddCity(pCity);
			m_mapCity.insert(make_pair(pCity->GetKey(),pCity));
			static int n3Day = 3*24*3600;
			if(pCity->GetDBCity()->has_lastbloodbtl())
			{
#ifdef WIN32
				//#ss#测试使用
				pCity->GetDBCity()->clear_lastbloodbtl();
#endif
				
				
				DB_C_BloodBtl* pDBBloodBtl = pCity->GetDBCity()->mutable_lastbloodbtl();
				if(pDBBloodBtl->launchuserid()<=0)
				{
					pCity->GetDBCity()->clear_lastbloodbtl();
				}
				else if(pDBBloodBtl->wincity()>0 && pDBBloodBtl->overtime()+CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pDBBloodBtl->type())<=ltNow)
				{
					pCity->GetDBCity()->clear_lastbloodbtl();
				}
				else 
				{
					m_mapCurBloodBtl.insert(make_pair(pCity->GetCityID(),pDBBloodBtl));
					if(pDBBloodBtl->type() == CBT_SYSLaunch)
						m_bHasLaunchAutoBloodBtl = true;
				}
			}

			//pCity->ResetEmptyUser();
			//for(int j=0;j<pCity->m_dbCity.user_size();j++)
			//{
			//	DB_C_User* pDBUser = pCity->m_dbCity.mutable_user(j);
			//	if(pDBUser->userid()<=0)
			//	{
			//		continue;
			//	}
			//	else
			//	{
			//		m_mapUser[pDBUser->userid()] = pDBUser;
			//	}
			//}
		}
		else
		{
			//还没有放入数据库,根据配置文件创建数据
			pCountry = GetCountry(pCityTbl->m_nCountry);
			if(pCountry==NULL)
			{
				delete pCity;
				continue;
			}
			DB_C_City* pDBCity = pCity->GetDBCity();
			pDBCity->set_countryid(pCityTbl->m_nCountry);
			pDBCity->set_region(m_nRegion);
			pDBCity->set_cityid(pCityTbl->m_nID);
			pCountry->AddCity(pCity);
			m_mapCity.insert(make_pair(pCity->GetKey(),pCity));
		}

		pCity->m_bNPCCity		= pCountry->m_nCountryID > C_UserCoutryEnd; 
		if(pCity->m_bNPCCity)
		{//NPC城市
			
			DB_C_City* pDBCity = pCity->GetDBCity();
			if(pDBCity->npcinittm()!=CountryInfoCfg::Instance().GetCountryBaseTbl()->m_ltNPCResetTM)
			{//初始化NPCID
				int nInitMorale = CountryInfoCfg::Instance().GetCountryBaseTbl()->m_nInitMorale;
				pDBCity->clear_npc();
				int nIdx = 0;
				map<int,int>::const_iterator itNPC;
				for(itNPC = pCityTbl->m_mapNPCArmy.begin();itNPC!=pCityTbl->m_mapNPCArmy.end();itNPC++)
				{
					int nNPCID = itNPC->first;
					int nCnt   = itNPC->second;
					for(int k=0;k<nCnt&&k<COUNTRY_NPC_RATE;k++)
					{
						DB_C_NPC* pNPC = pDBCity->add_npc();
						pNPC->set_npcid(nNPCID*COUNTRY_NPC_RATE+nIdx);
						pNPC->set_morale(nInitMorale);
						nIdx++;
					}
				}
				pDBCity->set_npcinittm(CountryInfoCfg::Instance().GetCountryBaseTbl()->m_ltNPCResetTM);
			}
			int nIdx = 0;
			int nUserCnt = 0;
			while(nIdx<pDBCity->npc_size())
			{//清除挂掉的NPC
				DB_C_NPC* pDBNPC = pDBCity->mutable_npc(nIdx);
				if(pDBNPC->npcid()<=0)
				{
					int nLastIdx = pDBCity->npc_size()-1;
					if(nLastIdx>=0)
					{
						DB_C_NPC* pLastdata = pDBCity->mutable_npc(nLastIdx);
						pDBNPC->Swap( pLastdata);
						//SYS_LOG(m_pPlayer->GetUserID(),LT_ClearData,0,0,"building"<<pLastdata->id()<<pLastdata->type());
						pDBCity->mutable_npc()->RemoveLast();
						continue;
					}
				}
				else
				{
					int nNPCID = pDBNPC->npcid();
					nNPCID /= COUNTRY_NPC_RATE;
					nNPCID *= COUNTRY_NPC_RATE;
					nNPCID += nIdx;
					pDBNPC->set_npcid(nNPCID);
					pCity->m_lstNPC.push_back(pDBNPC);
					nIdx++;
					nUserCnt++;
				}
			}
		}
	}
}

void CountryRgnDatHandler::LoadAllUserInfo(bool bCombineRegionOpt/* = false*/)
{
	char szKey[128];
	bool bLoadError = false;
	map<int,int> mapUserCnt;
	vector<int> lstRegion;
	lstRegion.clear();
	lstRegion.push_back(m_nRegion);	//先放入主区
	if(bCombineRegionOpt)
	{//合服操作
		int nSize = serverConfig.getgroupsize(m_nRegion);
		for(int i=0;i<nSize;i++)
		{
			int nRegion = serverConfig.getregionfromgroup(m_nRegion,i);
			if(nRegion>=0&&nRegion != m_nRegion)
			{
				lstRegion.push_back(nRegion);
			}
		}
	}
	else
	{
		
	}
	for(int k=0;k<lstRegion.size();k++)
	{
		int nRegion = lstRegion[k];
		for(int nIdx=0;nIdx<MAX_COUNTRYUSER_IDX;nIdx++)
		{

			int nKey = nRegion*MAX_CITY_ID+nIdx;
			sprintf(szKey,"CUserLst_%d",nKey);

			DB_C_UserList* pDBUserLst = new DB_C_UserList();
			pDBUserLst->set_key(nKey);
			pDBUserLst->set_index(nIdx);
			pDBUserLst->set_region(nRegion);
			m_mapUserLst[nKey] = pDBUserLst;
			//获取最主要的UserList，
			if(nRegion == m_nRegion)
			{
				m_pMainUserList = pDBUserLst;
			}
			bool bGet = m_pDH->LoadUserLstFromDB(szKey,*pDBUserLst,bLoadError);
			if(bLoadError)
			{//连接数据库失败，直接退出，不会走到这里
				exit(0);
			}
			if(bGet)
			{
				pDBUserLst->set_key(nKey);
				pDBUserLst->set_index(nIdx);
				pDBUserLst->set_region(nRegion);

				for(int i=0;i<pDBUserLst->user_size();i++)
				{
					DB_C_User* pDBUser = pDBUserLst->mutable_user(i);
					if(!pDBUser->has_userid())
						continue;
					if(bCombineRegionOpt)
					{//合服操作
						pDBUser->clear_arenaindex();
						pDBUser->clear_worldarenaindex();
						pDBUser->clear_lastarenaindex();
						pDBUser->clear_lastworldarenaindex();
						pDBUser->clear_officerposition();
						pDBUser->set_cityid(CountryInfoCfg::Instance().GetCapital(pDBUser->countryid()));

						static int nAdd = MAX_CITY_ID*10;
						if(pDBUser->guildid()>0&&pDBUser->guildid()<nAdd)
						{
							pDBUser->set_guildid(pDBUser->guildid()+nRegion*nAdd);
						}
						//将其他服的玩家放入合并后服
						if(nRegion != m_nRegion)
						{	
							DB_C_User* pTmpUser = GetDBCityUser(pDBUser->userid());
							if(pTmpUser!=NULL)
								continue;;
							pTmpUser = AllocDBUser(pDBUser->userid());
							pTmpUser->CopyFrom(*pDBUser);

							pDBUser = pTmpUser;	//此处更改指针
						}
					}

					CCity* pCity = GetCity(pDBUser->cityid());
					if(pCity==NULL || pCity->GetCountry() != pDBUser->countryid()||pDBUser->userid()<1000000000000)
					{//玩家数据错误
						int64 nUserID = pDBUser->userid();
						SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_LoadUserError<<pDBUser->cityid()<<pDBUser->countryid()<<pCity->GetCountry()<<pDBUser->guildid());
						pDBUser->Clear();
						continue;
					}

					m_mapUser[pDBUser->userid()] = pDBUser;
					AddDbUserToCountry(pDBUser);
                    m_GuildRedEnvelope.AddUser(pDBUser);

					mapUserCnt[pDBUser->countryid()] ++;
					if(pDBUser->has_guildid()&&pDBUser->guildid()>0)
					{

						static int nAdd = OLD_GUILD_SIZE*10;
						if(nRegion>0&&pDBUser->guildid()<nAdd)
						{
							pDBUser->set_guildid(pDBUser->guildid()+nRegion*nAdd);
						}

						CGuild* pGuild = GetGuild(pDBUser->guildid());
						if(pGuild)
						{
							pGuild->AddUser(pDBUser);
						}
#ifdef WIN32
						else
						{
							pDBUser->set_guildid(0);
							pDBUser->set_position(0);
						}
#endif
					}

					if(CountryInfoCfg::Instance().IsCapital(pDBUser->cityid()))
					{
						pCity->m_nCaptureUserCnt++;
						continue;//暂不处理首都玩家列表
					}
					int64 nUserID = pDBUser->userid();
					pCity->m_lstUser[nUserID%CITY_USER_GROUP].push_back(pDBUser);
					pCity->m_mapHasUser[nUserID] = 1;

					if ( IsChampion(pDBUser->userid()) )
					{
						pCity->m_bChampionIn = true;
					}
					//pCity->m_lstUser.push_back(pDBUser->userid());
					//pCity->m_mapUserIdx[pDBUser->userid()] = pCity->m_lstUser.size()-1;
				}
			}
			else
			{//数据暂未创建	
				//	markUserLstDirty(pDBUserLst);
			}

		}
	}
	

	for(int j=C_Start;j<C_CountryCnt;j++)
	{
		if(!IsValidCountry(j))
			continue;
		CCountry* pCountry = GetCountry(j);
		pCountry->GetDBCountry()->set_usercnt(mapUserCnt[j]);
	}
	
}

void CountryRgnDatHandler::LoadAllGuildInfo(bool bCombineRegionOpt /*= false*/)
{
	vector<int> lstRegion;
	//加载合并后所有公会DB
	int nSize = serverConfig.getgroupsize(m_nRegion);
	if(nSize<=0)
	{//未合区
		lstRegion.push_back(m_nRegion);
	}
	else
	{
		for(int i=0;i<nSize;i++)
		{
			int nRegion = serverConfig.getregionfromgroup(m_nRegion,i);
			if(nRegion>=0)
			{
				lstRegion.push_back(nRegion);
			}
		}
	}
	char szKey[128];
	bool bLoadError = false;
	map<int,int> mapUserCnt;
	for(int k=0;k<lstRegion.size();k++)
	{
		int nRegion = lstRegion[k];
		for(int nIdx=0;nIdx<MAX_COUNTRYGUILD_IDX;nIdx++)
		{
			int nKey = nRegion*MAX_CITY_ID+nIdx;
			sprintf(szKey,"GuildLst_%d",nKey);

			DB_C_GuildList* pDBGuildLst = new DB_C_GuildList();
			pDBGuildLst->set_key(nKey);
			pDBGuildLst->set_index(nIdx);
			pDBGuildLst->set_region(nRegion);
			m_mapGuildLst[nKey] = pDBGuildLst;
			bool bGet = m_pDH->LoadGuildLstFromDB(szKey,*pDBGuildLst,bLoadError);
			if(bLoadError)
			{//连接数据库失败，直接退出，不会走到这里
				exit(0);
			}
			if(bGet)
			{
				for(int i=0;i<pDBGuildLst->guild_size();i++)
				{
					DB_C_Guild* pDBGuild = pDBGuildLst->mutable_guild(i);
					if(bCombineRegionOpt)
					{//合服操作，清工会战数据
						pDBGuild->clear_btlinfo();

						pDBGuild->mutable_gvemsg()->clear_lastmsg();

						pDBGuild->mutable_gvgmsg()->clear_signcity();
						pDBGuild->mutable_gvgmsg()->clear_controlcity();
					}
					//兼容合区操作,将分区号放在公会ID前
					static int nAdd = OLD_GUILD_SIZE*10;
					if(nRegion>0&&pDBGuild->guildid()<nAdd)
					{
						pDBGuild->set_guildid(pDBGuild->guildid()+nRegion*nAdd);
					}

					CGuild* pGuild = new CGuild(m_nRegion,nRegion,pDBGuild);
					int nCountry = pGuild->GetCountry();
					if(nCountry>=C_Start&&nCountry<C_UserCoutryEnd&&pGuild->GetGuildLV()>0)
					{
						m_mapGuild[pGuild->GetGuildID()] = pGuild;
						
						if(m_mapGuildName.find(pDBGuild->guildname()) != m_mapGuildName.end())
						{
							char szTmp[1024];
							sprintf(szTmp,"%d-%s",nRegion+1,pDBGuild->guildname().c_str());
							pGuild->SetGuildName(szTmp);
						}	
						else
						{
							pGuild->SetGuildName(pDBGuild->guildname().c_str());
						}
						m_mapGuildName[pDBGuild->guildname()] = 1;

						m_lstGuild[C_AllCountry].push_back(pGuild);

						m_lstGuild[nCountry].push_back(pGuild);
						if(pGuild->GetGuildLV() >= GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinGuildLV)
						{
							m_lstPKGuild[C_AllCountry].push_back(pGuild);
							m_lstPKGuild[nCountry].push_back(pGuild);
						}

						m_lstGuildGveNow.push_back(pGuild);
						m_lstGuildGveLast.push_back(pGuild);
						m_lstGuildGveHighest.push_back(pGuild);

						m_lstGuildGveNowNew[nCountry].push_back(pGuild);
						m_lstGuildGveLastNew[nCountry].push_back(pGuild);
						m_lstGuildGveHighestNew[nCountry].push_back(pGuild);
												
					}
					else if(nRegion == m_nRegion)
					{//空工会,仅使用合服后的公会
						m_lstEmptyGuild[nCountry].push_back(pGuild);
					}
				}
			}
			else
			{//数据暂未创建	
				//	markUserLstDirty(pDBUserLst);
			}

		}
	}
	
}

void CountryRgnDatHandler::LoadAllResourceInfo(bool bCombineRegionOpt /*= false*/)
{
	char szKey[128];
	bool bLoadError = false;
	map<int,int> mapUserCnt;

	int nRegion = m_nRegion;

	int nKey = nRegion*MAX_RESOURSE_SAVE_ID;
	sprintf(szKey,"CResourse_%d",nKey);

	DB_C_ResourceData* pDBResourseData = new DB_C_ResourceData();
	pDBResourseData->set_key(nKey);
	pDBResourseData->set_region(nRegion);
	m_ResourceData.SetDBResourceData( pDBResourseData );

	bool bGet = m_pDH->LoadResourceFromDB(szKey,*pDBResourseData,bLoadError);
	if(bLoadError)
	{//连接数据库失败，直接退出，不会走到这里
		exit(0);
	}

	//初始化数据
	bool bSave = false;
	for ( int nSearchType = 1; nSearchType <= MAX_RESOURSE_SEARCH_TYPE; nSearchType++ )
	{
		for ( int nType = 1; nType <= MAX_RESOURSE_TYPE; nType++ )
		{
			int nCnt = ResourceCfg::Instance().GetResourceCnt(nSearchType,nType);

			for ( int nIdx = 0; nIdx < nCnt; nIdx++ )
			{
				int nID = nSearchType*MAX_RESOURSE_TYPE_ID + nType*MAX_RESOURSE_ID + nIdx;
				CResource* pResource = new CResource( nID );

				m_lstResource[nSearchType][nID] = pResource;
				DB_C_Resource* pDBResource = m_ResourceData.GetDBResource(nID);
				if ( pDBResource == NULL )
				{
					pDBResource = m_ResourceData.m_pDBResourceData->add_resourselist();
					pDBResource->set_id(nID);
					bSave = true;
				}

				if ( bCombineRegionOpt )
				{
					pDBResource->clear_uid();
					pDBResource->clear_name();
					pDBResource->clear_endtime();
				}

				pResource->m_pDBResource = pDBResource;
				pResource->m_sUid = pDBResource->uid();
				pResource->m_sName = pDBResource->name();
				pResource->m_nEndTime = pDBResource->endtime();
			}
		}
	}

	if ( bSave )
	{
		markResourceInfoDirty();
	}
}


void CountryRgnDatHandler::LoadAllGvgCity(bool bCombineRegionOpt /*= false*/)
{
	bool bLoadError = false;
	int nKey = 0;
	//char szKey[128];
	time_t ltNow = time(NULL);

	int nNum = GvgCfg::Instance().GetCityTblNum();

	for(int i=0;i<nNum;i++)
	{
		const GvgCityTbl* pCityTbl = GvgCfg::Instance().GetCityTblByNum(i);
		if(pCityTbl==NULL)
			continue;

		CGvgCity* pCity = new CGvgCity();
		DB_C_GvgCity* pDBGveCity = new DB_C_GvgCity();
		int nKey = GetCityKey(pCityTbl->m_nID);
		char szKey[128];
		sprintf(szKey,"GvgCity_%d",nKey);
		bool bGet = m_pDH->LoadGvgCityFromDB(szKey,*pDBGveCity,bLoadError);
		if(bLoadError)
		{//连接数据库失败，直接退出，不会走到这里
			exit(0);
		}
		if(bGet)
		{
			
		}
		else
		{
			//还没有放入数据库,根据配置文件创建数据
			pDBGveCity->set_region(m_nRegion);
			pDBGveCity->set_cityid(pCityTbl->m_nID);
			pDBGveCity->set_citypoint(pCityTbl->m_nPoint);
			pDBGveCity->set_key( nKey );

		}

		pCity->SetDBGvgCityData(pDBGveCity);
		pCity->m_nCityID = pCityTbl->m_nID;

		if ( bCombineRegionOpt )
		{
			pDBGveCity->set_guildid( 0 );
			pDBGveCity->set_citypoint( pCityTbl->m_nPoint );
			pDBGveCity->set_guildstreak( 0 );
			pDBGveCity->clear_signguildlist();

			SYS_LOG(pCityTbl->m_nID,LT_CNTGVG_SIGN,GetRegion(),0,"hefuclear");
		}

		if ( pDBGveCity->guildid() != 0 )
		{
			CGuild* pGuild = GetGuild(pDBGveCity->guildid());
			if ( pGuild != NULL )
			{
				pCity->SetGvgCityGuildMsg(pGuild);
			}
		}
		
		m_mapGvgCity[pCityTbl->m_nID] = pCity;

	}
}

void CountryRgnDatHandler::LoadAllWarGameInfo(bool bCombineRegionOpt/* = false*/)
{
    char szKey[128];
    bool bLoadError = false;
    map<int,int> mapUserCnt;

    int nRegion = m_nRegion;

    int nKey = nRegion*MAX_WARGAME_SAVE_ID;
    sprintf(szKey,"CWarGame_%d",nKey);

    DB_C_WarGameData* pDBWarGameData = new DB_C_WarGameData();
    pDBWarGameData->set_key(nKey);
    pDBWarGameData->set_region(nRegion);
    bool bGet = m_pDH->LoadWarGameFromDB(szKey,*pDBWarGameData,bLoadError);
    if(bLoadError)
    {//连接数据库失败，直接退出，不会走到这里
        exit(0);
    }
	if (bCombineRegionOpt)
	{
		pDBWarGameData->clear_sitelist();
	}
    bool bNeedBrushDB = false;
    m_WarGameData.InitByDBData( pDBWarGameData , bNeedBrushDB);
    if (bNeedBrushDB || bCombineRegionOpt)
    {
        markWarGameDataDirty();
    }
}

void CountryRgnDatHandler::LoadBossBtlMsg(bool bCombineRegionOpt/* = false*/)
{
	char szKey[128];
	bool bLoadError = false;
	map<int,int> mapUserCnt;

	int nRegion = m_nRegion;

	int nKey = nRegion*MAX_BOSSBTLMSG_SAVE_ID;
	sprintf(szKey,"CBossBattle_%d",nKey);

	m_pDBBossBtlMsg = new DB_C_BossBtlMsg();
	m_pDBBossBtlMsg->set_key(nKey);
	m_pDBBossBtlMsg->set_region(nRegion);
	bool bGet = m_pDH->LoadBossBtlMsgFromDB(szKey,*m_pDBBossBtlMsg,bLoadError);
	if(bLoadError)
	{//连接数据库失败，直接退出，不会走到这里
		exit(0);
	}

	for ( int i = 0; i < m_pDBBossBtlMsg->ranklist_size(); i++ )
	{
		DB_C_BossBtlRankUnit* pDBRankUnit = m_pDBBossBtlMsg->mutable_ranklist(i);

		int64 uid = pDBRankUnit->uid();
		DB_C_User* pDBCUser = GetDBCityUser(uid);
		if ( pDBCUser != NULL )
		{
			m_lstBossBtlRank.push_back(pDBCUser);
		}
	}


	DB_C_BossBtlRankUnit* pDBUnit = NULL;
	pDBUnit = m_pDBBossBtlMsg->mutable_killer();

	for ( int i = 0; i < pDBUnit->namelist_size(); i++ )
	{
		m_lstUpList[0].push_back(pDBUnit->namelist(i));
	}

	for ( int j = 0; j < m_pDBBossBtlMsg->ranklist_size() && j < 3; j++ )
	{
		pDBUnit = m_pDBBossBtlMsg->mutable_ranklist(j);
		if ( pDBUnit == NULL )
		{
			break;
		}
		
		for ( int i = 0; i < pDBUnit->namelist_size(); i++ )
		{
			m_lstUpList[j+1].push_back(pDBUnit->namelist(i));
		}
	}
}



CCountry* CountryRgnDatHandler::GetCountry(int nCountryID)
{
	if(nCountryID==0)
		return GetEarthCountry();

	map<int,CCountry*>::iterator iter;
	int nKey = m_nRegion * MAX_CITY_ID + nCountryID;
	iter = m_mapCountry.find(nKey);
	if(iter != m_mapCountry.end())
		return iter->second;
	return NULL;
}

CCity* CountryRgnDatHandler::GetCity(int nCityID)
{
	int nKey = GetCityKey(nCityID);

	map<int, CCity*>::iterator iter = m_mapCity.find(nKey);
	if(iter!=m_mapCity.end())
		return iter->second;
	return NULL;
}

int	CountryRgnDatHandler::GetGuildCnt(int nCountryID/* =0 */)
{
	if(nCountryID<C_Start||nCountryID>=C_UserCoutryEnd)
		nCountryID = C_Unknow;
	return m_lstGuild[nCountryID].size();
}

CGuild*	CountryRgnDatHandler::AllocEmptyGuild(int nCountryID)
{
	if(nCountryID<C_Start||nCountryID>=C_UserCoutryEnd)
		return NULL;
	CGuild* pAllocGuild = NULL;
 	while(m_lstEmptyGuild[nCountryID].size()>0)
	{//有闲置工会,这里增加验证
		pAllocGuild = m_lstEmptyGuild[nCountryID].front();
		m_lstEmptyGuild[nCountryID].pop_front();
		if(pAllocGuild->GetGuildLV()<=0&&pAllocGuild->GetGuildID()>0)
		{
			break;
		}
		else
		{
			pAllocGuild = NULL;
		}
	}
	static int nAdd = OLD_GUILD_SIZE*10;
	if(pAllocGuild == NULL)
	{
		if(m_lstEmptyGuildID.size()<=10)
		{
			ResetEmptyGuildIDLst();
		}
		int nID = m_lstEmptyGuildID.front();
		nID += nCountryID*100000;
		
		nID += m_nRegion * nAdd;

		m_lstEmptyGuildID.pop_front();
		int nCount = 1000;
		while(GetGuild(nID) != NULL)
		{
			nID = m_lstEmptyGuildID.front();
			nID += nCountryID*100000;
			m_lstEmptyGuildID.pop_front();
			nCount--;
			if(m_lstEmptyGuildID.size()<=10)
			{
				ResetEmptyGuildIDLst();
			}
			if(nCount<=0)
			{//防止死循环
				return NULL;
			}
		}
		DB_C_GuildList* pDBGuildLst = GetDBGuildLst(nID,m_nRegion);
		if(pDBGuildLst==NULL)
			return NULL;
		DB_C_Guild* pDBGuild = pDBGuildLst->add_guild();
		pDBGuild->set_guildid(nID);
		pAllocGuild = new CGuild(m_nRegion,m_nRegion,pDBGuild);
	}
	else
	{
		DB_C_Guild* pDBGuild = pAllocGuild->GetDBGuild();
		if(m_nRegion>0&&pDBGuild->guildid()<nAdd)
		{
			pDBGuild->set_guildid(pDBGuild->guildid()+m_nRegion*nAdd);
		}
	}
	DB_C_Guild* pDBGuild = pAllocGuild->GetDBGuild();
	pDBGuild->set_countryid(nCountryID);
	pDBGuild->set_guildlv(1);
	for(int i=pDBGuild->build_size();i<GB_Cnt;i++)
	{
		DB_C_GuildBld* pDBBld = pDBGuild->add_build();
		pDBBld->set_id(i);
		pDBBld->set_level(0);
		pDBBld->set_exp(0);
	}
	pDBGuild->mutable_build(GB_Hall)->set_level(1);

	m_mapGuild[pAllocGuild->GetGuildID()] = pAllocGuild;
	
	m_lstGuild[nCountryID].push_back(pAllocGuild);
	m_lstGuild[C_Unknow].push_back(pAllocGuild);

	m_lstGuildGveNow.push_back(pAllocGuild);
	m_lstGuildGveLast.push_back(pAllocGuild);
	m_lstGuildGveHighest.push_back(pAllocGuild);

	m_lstGuildGveNowNew[nCountryID].push_back(pAllocGuild);
	m_lstGuildGveLastNew[nCountryID].push_back(pAllocGuild);
	m_lstGuildGveHighestNew[nCountryID].push_back(pAllocGuild);
	
	return pAllocGuild;
}

void CountryRgnDatHandler::DismissGuild(CGuild* pGuild)
{
	if(pGuild==NULL)
		return;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	int nGuildID = pDBGuild->guildid();
	map<string,bool>::iterator itName;
	itName = m_mapGuildName.find(pGuild->GetGuildName());
	if(itName != m_mapGuildName.end())
	{
		m_mapGuildName.erase(itName);
	}	
	map<int,CGuild*>::iterator itGuild;
	itGuild = m_mapGuild.find(nGuildID);
	if(itGuild != m_mapGuild.end())
	{
		m_mapGuild.erase(itGuild);
	}
	vector<CGuild*>::iterator iter;
	for(iter = m_lstGuild[C_AllCountry].begin();iter!=m_lstGuild[C_AllCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuild[C_AllCountry].erase(iter);
			break;
		}
	}
	int nCountry = pGuild->GetCountry();
	for(iter = m_lstGuild[nCountry].begin();iter!=m_lstGuild[nCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuild[nCountry].erase(iter);
			break;
		}
	}

	for(iter = m_lstPKGuild[C_AllCountry].begin();iter!=m_lstPKGuild[C_AllCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstPKGuild[C_AllCountry].erase(iter);
			break;
		}
	}
	
	for(iter = m_lstPKGuild[nCountry].begin();iter!=m_lstPKGuild[nCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstPKGuild[nCountry].erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveNow.begin();iter!=m_lstGuildGveNow.end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveNow.erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveLast.begin();iter!=m_lstGuildGveLast.end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveLast.erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveHighest.begin();iter!=m_lstGuildGveHighest.end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveHighest.erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveNowNew[nCountry].begin();iter!=m_lstGuildGveNowNew[nCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveNowNew[nCountry].erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveLastNew[nCountry].begin();iter!=m_lstGuildGveLastNew[nCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveLastNew[nCountry].erase(iter);
			break;
		}
	}

	for(iter = m_lstGuildGveHighestNew[nCountry].begin();iter!=m_lstGuildGveHighestNew[nCountry].end();iter++)
	{
		CGuild* pTmpGuild = *iter;
		if(pTmpGuild == pGuild)
		{
			iter = m_lstGuildGveHighestNew[nCountry].erase(iter);
			break;
		}
	}

    m_GuildRedEnvelope.RemoveGuild(pGuild);

	string strBldLV;
	char szTmp[32];
	for(int i=0;i<pDBGuild->build_size();i++)
	{
		DB_C_GuildBld* pDBBld = pDBGuild->mutable_build(i);
		sprintf(szTmp,"%d|",pDBBld->level());
		strBldLV += szTmp;
	}
	SYS_LOG(int64(pDBGuild->leaderid()),LT_Guild_Dismiss,GetRegion(),0,nGuildID<<pDBGuild->countryid()<<GetGuildCnt()<<GetGuildCnt(pDBGuild->countryid())<<strBldLV);

	pDBGuild->Clear();
	pDBGuild->set_guildid(nGuildID);
	pDBGuild->set_countryid(C_Unknow);
	pDBGuild->set_guildlv(0);

	SortGuild();
	SortPKGuild();

}
CGuild*	CountryRgnDatHandler::GetGuild(int nGuildID)
{
	if(nGuildID<=0)
		return NULL;
	map<int,CGuild*>::iterator iter;
	iter = m_mapGuild.find(nGuildID);
	if(iter == m_mapGuild.end())
	{
		return NULL;
	}
	return iter->second;
}

DB_C_GuildList* CountryRgnDatHandler::GetDBGuildLst(int nGuildID,int nOldRegion)
{
	int nKey = GetGuildKey(nGuildID,nOldRegion);
	map<int,DB_C_GuildList*>::iterator iter;
	iter = m_mapGuildLst.find(nKey);
	if(iter != m_mapGuildLst.end())
		return iter->second;
	return NULL;
}


DB_C_UserList* CountryRgnDatHandler::GetDBUserLst(int64 nUserID)
{
	int nKey = GetUserKey(nUserID);

	map<int,DB_C_UserList*>::iterator iter;
	iter = m_mapUserLst.find(nKey);
	if(iter != m_mapUserLst.end())
		return iter->second;
	return NULL;
}

DB_C_User* CountryRgnDatHandler::GetDBCityUser(int64 nUserID)
{
	hash_map<int64,DB_C_User*>::iterator iter;
	iter = m_mapUser.find(nUserID);
	if(iter != m_mapUser.end())
		return iter->second;
	return NULL;
}


DB_C_User* CountryRgnDatHandler::AllocDBUser(int64 nUserID)
{
	DB_C_UserList* pDBUserLst = GetDBUserLst(nUserID);
	if(pDBUserLst==NULL)
	{
		return NULL;
	}
	DB_C_User* pDBUser = NULL;
	for(int i=0;i<pDBUserLst->user_size();i++)
	{
		DB_C_User* pFindDBUser = pDBUserLst->mutable_user(i);
		if(pFindDBUser->userid()<=0)
		{
			pDBUser = pFindDBUser;
			break;
		}
	}
	if(pDBUser==NULL)
	{
		pDBUser = pDBUserLst->add_user();
	}
	pDBUser->set_userid(nUserID);
	m_mapUser[nUserID] = pDBUser;	
	return pDBUser;
}

void CountryRgnDatHandler::AddDbUserToCountry(DB_C_User* pUser)
{
	if(pUser == NULL)
	{
		return;
	}

	int nCountry = pUser->countryid();
	if(nCountry < C_Start || nCountry >= C_UserCoutryEnd)
	{
		return;
	}

	vector<DB_C_User*>& rvecList = m_vecCountryUser[nCountry];
	rvecList.push_back(pUser);
}

void CountryRgnDatHandler::UpdateCountryArenaList(int nCountry,DB_Arena_Rank* pArenaR,vector<int64>& rVecUid)
{
	if(nCountry < C_Start || nCountry >= C_UserCoutryEnd || pArenaR == NULL)
	{
		return;
	}

	rVecUid.clear();

	vector<DB_Arena_Node> rVecUserTmp;
	rVecUserTmp.clear();

	//玩家
	vector<DB_C_User*>& rvecList = m_vecCountryUser[nCountry];
	for(int i=0;i<(int)rvecList.size();i++)
	{
		DB_C_User* pUser = rvecList[i];
		if(pUser && pUser->countryid() == nCountry)
		{
			bool bRankErr = false;
			int oldIndex = pUser->arenaindex();
			_UpdateArenaUserList(pUser,rVecUserTmp,bRankErr);
			if(bRankErr)
			{
				SYS_LOG(0,LT_ARENA_COUNTRY_RANKERRO,m_nRegion,0,0<<(int64)pUser->userid()<<oldIndex<< pUser->arenaindex()<<nCountry);
			}
		}
	}

	//NPC
	for(int i=0;i<pArenaR->arenaranknpc_size();i++)
	{
		_UpdateArenaNpcList(pArenaR->mutable_arenaranknpc(i),rVecUserTmp);
	}

	for(int i=0;i<(int)rVecUserTmp.size();i++)
	{
		DB_Arena_Node& rNode = rVecUserTmp[i];
		if(rNode.pUser)
		{
			rVecUid.push_back(rNode.pUser->userid());
			rNode.pUser->set_arenaindex(rVecUid.size() - 1);
		}

		if(rNode.pNpc)
		{
			rVecUid.push_back(rNode.pNpc->npcid());
			rNode.pNpc->set_npcindex(rVecUid.size() - 1);
		}
	}
}

void CountryRgnDatHandler::_UpdateArenaUserList(DB_C_User* pUser,vector<DB_Arena_Node>& rUserVec,bool& bRankErr)
{
	if(pUser == NULL)
	{
		return;
	}

	int64 nUid = pUser->userid();
	int nPos = pUser->arenaindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		bRankErr = true;
		pUser->set_arenaindex(nPos + 1);
		_UpdateArenaUserList(pUser,rUserVec,bRankErr);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rUserVec[nPos] = rNode;
	}
}

void CountryRgnDatHandler::_UpdateArenaNpcList(DB_Arena_Rank_NPC* pNpc,vector<DB_Arena_Node>& rUserVec)
{
	if(pNpc == NULL)
	{
		return;
	}

	int64 nUid = pNpc->npcid();
	int nPos = pNpc->npcindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	rNode.pUser = NULL;
	rNode.pNpc = NULL;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		pNpc->set_npcindex(nPos + 1);
		_UpdateArenaNpcList(pNpc,rUserVec);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = NULL;
		rNode.pNpc = pNpc;
		rUserVec[nPos] = rNode;
	}
}

void CountryRgnDatHandler::markGuildLstDirty(CGuild* pGuild)
{
	DB_C_GuildList* pDBGuildLst = GetDBGuildLst(pGuild->GetGuildID(),pGuild->m_nOldRegion);
	if(pDBGuildLst)
	{
		m_pDH->markGuildLstDirty(pDBGuildLst);
	}
}
void CountryRgnDatHandler::markUserLstDirty(int64 nUserID)
{
	DB_C_UserList* pDBUserLst = GetDBUserLst(nUserID);
	if(pDBUserLst)
	{
		m_pDH->markUserLstDirty(pDBUserLst);
	}
}

void CountryRgnDatHandler::markUserLstDirty(DB_C_UserList* pDBUserLst)
{
	m_pDH->markUserLstDirty(pDBUserLst);
}

void CountryRgnDatHandler::markCityDirty(CCity* pCity)
{
	m_pDH->markCityDirty(pCity);
}

void CountryRgnDatHandler::markCountryDirty(CCountry* pCountry)
{
	m_pDH->markCountryDirty(pCountry);
}

void CountryRgnDatHandler::markResourceInfoDirty()
{
	m_pDH->markResourceDataDirty(m_ResourceData.m_pDBResourceData);
}

void CountryRgnDatHandler::markGvgCityDirty(CGvgCity* pCity)
{
	m_pDH->markGvgCityDirty(pCity);
}

void CountryRgnDatHandler::markWarGameDataDirty()
{
    m_pDH->markWarGameDataDirty(m_WarGameData.GetDBWarGameData());
}

void CountryRgnDatHandler::markBossBtlInfoDirty()
{
	m_pDH->markBossBtlMsgDataDirty(m_pDBBossBtlMsg);
}

void CountryRgnDatHandler::CheckUserInfo(int64 nUserID,CCountry* pCountry,int nSelfCity,int& nRightCity,bool& bNeedSave)
{
	if(pCountry==NULL)
		return;
	DB_C_User* pDBCUser = GetDBCityUser(nUserID);
	CCity*		pCity	= GetCity(nSelfCity);

	if(pDBCUser==NULL)
	{//出错
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_UserNotFind<<nSelfCity<<pCountry->GetCountryID());
		pDBCUser = AllocDBUser(nUserID);
		pDBCUser->set_userid(nUserID);
		pDBCUser->set_cityid(nSelfCity);
		pDBCUser->set_countryid(pCountry->GetCountryID());
		if(pCity&&pCity->GetCountry()==pCountry->GetCountryID())
		{//是一个国家
			bool bIsChampion = IsChampion(pDBCUser->userid());
			pCity->UserEnterCity(pDBCUser,bIsChampion);
		}
		else
		{
			pDBCUser->set_cityid(pCountry->GetCapital()->GetCityID());
			CCity*	pTmpCity = GetCity(pDBCUser->cityid());
			if(pTmpCity)
			{
				bool bIsChampion = IsChampion(pDBCUser->userid());
				pTmpCity->UserEnterCity(pDBCUser,bIsChampion);
			}
		}
		nRightCity = pDBCUser->cityid();
		bNeedSave = true;
		AddDbUserToCountry(pDBCUser);
	}
	if(pDBCUser==NULL)
		return;
	if(pDBCUser->countryid()!=pCountry->GetCountryID())
	{//国家不一致
		pDBCUser->set_countryid(pCountry->m_nCountryID);
		pDBCUser->set_cityid(pCountry->GetCapital()->GetCityID());
		bNeedSave = true;
	}

	if(pDBCUser->cityid()!=nSelfCity)
	{//城市不一致，以game为准
		CCity* pOldCity = GetCity(pDBCUser->cityid());
		bool bIsChampion = IsChampion(pDBCUser->userid());
		if(pOldCity)
		{
			pOldCity->UserLeaveCity(pDBCUser,bIsChampion);
		}
		pDBCUser->set_cityid(nSelfCity);
		if(pCity)
		{	
			pCity->UserEnterCity(pDBCUser,bIsChampion);
		}
		bNeedSave = true;
	}
	if(pCity==NULL)
	{
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_UserNoCity<<nSelfCity<<pCountry->GetCountryID());
		pCity = pCountry->GetCapital();
		nRightCity = pCity->GetCityID();
		pDBCUser->set_cityid(pCity->GetCityID());
		if(pCity)
		{
			bool bIsChampion = IsChampion(pDBCUser->userid());
			pCity->UserEnterCity(pDBCUser,bIsChampion);
		}
		bNeedSave = true;
	}
	if(pCity->GetCountry()!=pCountry->GetCountryID())
	{//城市所属国不一致
		SYS_LOG(nUserID,LT_CNTR_ERROR,0,0,CntrError_CityError<<nSelfCity<<pCity->GetCountry()<<pCountry->GetCountryID());
		bool bIsChampion = IsChampion(pDBCUser->userid());
		pCity->UserLeaveCity(pDBCUser,bIsChampion);
		pCity = pCountry->GetCapital();
		nRightCity = pCity->GetCityID();
		pCity->UserEnterCity(pDBCUser,bIsChampion);
		bNeedSave = true;
	}
}

int CountryRgnDatHandler::GetCountryLastDayRank(int nCountryID)
{
	CCountry* pCountry = GetCountry(nCountryID);
	if(pCountry==NULL)
		return 0;
	
	DB_C_CountryRank* pDBRank = pCountry->GetDBCountry()->mutable_lastrank();
	CCity* pCaptial = pCountry->GetCapital();
	time_t ltNow = time(NULL);
	if(ltNow < pDBRank->nexttime())
	{
		return pDBRank->rank();	
	}
	else
	{//重新计算国家城市排行榜
		ResetDayCountryRank();
		pDBRank = pCountry->GetDBCountry()->mutable_lastrank();
		return pDBRank->rank();	
	}
	return 0;
}

void CountryRgnDatHandler::ResetDayCountryRank()
{	

	time_t ltNow = time(NULL);
	tm day = *localtime(&ltNow);
	day.tm_hour = 23;
	day.tm_min  = 59;
	day.tm_sec	= 59;
	time_t ltFlag = mktime(&day);

	vector<CCountry*> lstCountry;
	vector<CCountry*>::iterator itCntr;

	vector<CCity*>::iterator itCT;
	map<int,CCountry*>::iterator iter;

	for(int i=C_Start;i<C_UserCoutryEnd;i++)
	{
		CCountry* pCountry = GetCountry(i);
		if(pCountry==NULL)
		{
			continue;
		}

		lstCountry.push_back(pCountry);
		
		DB_C_CountryRank* pDBRank = pCountry->GetDBCountry()->mutable_lastrank();
		pDBRank->Clear();
		for(itCT = pCountry->m_lstCity.begin();itCT!=pCountry->m_lstCity.end();itCT++)
		{
			CCity* pCity = *itCT;
			if(pCity==NULL)
				continue;
			pDBRank->set_boom(pDBRank->boom()+CountryInfoCfg::Instance().GetCityBoom(pCity->GetCityID()));
		}
	}

	for(int i=0;i<(int)lstCountry.size()-1;i++)
	{
		for(int j=i+1;j<(int)lstCountry.size();j++)
		{
			if(lstCountry[i]->GetDBCountry()->mutable_lastrank()->boom() < lstCountry[j]->GetDBCountry()->mutable_lastrank()->boom())
			{
				CCountry* pTmpCountry = lstCountry[i];
				lstCountry[i] = lstCountry[j];
				lstCountry[j] = pTmpCountry;
			}
		}
	}
	int nIdx = 0;
	itCntr = lstCountry.begin();
	int nMaxBoom = 99999999;
	while(itCntr != lstCountry.end())
	{
		CCountry* pTmpCountry = *itCntr;
		DB_C_CountryRank* pDBRank = pTmpCountry->GetDBCountry()->mutable_lastrank();
		if(pDBRank->boom()<nMaxBoom)
		{//有并列
			nIdx++;
			nMaxBoom = pDBRank->boom();
		}
		pDBRank->set_rank(nIdx);
		pDBRank->set_nexttime((int)ltFlag);


		DB_C_CountryLeagueBattle* pDBCountryLeagueBattle = pTmpCountry->GetDBCountryLeagueBattle();
		if ( pDBCountryLeagueBattle != NULL )
		{
			pDBCountryLeagueBattle->set_boom( pDBRank->boom() + pDBCountryLeagueBattle->boom() );

			SYS_LOG(0,LT_LEAGUEBATTLE_DEBUG,m_nRegion,0,"DayBoom"<<pTmpCountry->GetCountryID()<<pDBRank->boom()<<pDBCountryLeagueBattle->boom());
		}

		//计算国家累计繁荣度
		markCountryDirty(pTmpCountry);

		itCntr++;
	}
}

void CountryRgnDatHandler::UserLeaveCountry(DB_C_User* pDBCUser,int nCountry,int nCity)
{
	CCountry* pCountry = GetCountry(nCountry);
	if(pCountry==NULL||pDBCUser==NULL)
	{
		return ;
	}
	CCity* pOldCity = GetCity(pDBCUser->cityid());
	if(pOldCity)
	{
		bool bIsChampion = IsChampion(pDBCUser->userid());
		pOldCity->UserLeaveCity(pDBCUser,bIsChampion);
	}
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	pDBCountry->set_usercnt(pDBCountry->usercnt()-1);
}


int CountryRgnDatHandler::UserSelCountry(DB_C_User* pDBCUser,int nCountry,int& nUserCnt)
{
	//玩家选择国家，当game服务器与国家服务器数据不一致时，以game服务器为准
	if(pDBCUser==NULL||!CountryInfoCfg::Instance().IsUserCountry(nCountry))
		return 1;

	CCountry* pCountry = GetCountry(nCountry);
	if(pCountry==NULL)
		return 1;
	int64 nUserID = pDBCUser->userid();
	DB_C_Country* pDBCountry = pCountry->GetDBCountry();
	pDBCountry->set_usercnt(pDBCountry->usercnt()+1);
	nUserCnt = pDBCountry->usercnt();

	pDBCUser->set_userid(nUserID);
	pDBCUser->set_cityid(pCountry->GetCapital()->GetCityID());
	pDBCUser->set_countryid(nCountry);

	UserEnterCity(pDBCUser,pCountry->GetCapital()->GetCityID());

	//加入国家记录，离开国家就不记录了
	AddDbUserToCountry(pDBCUser);

	return 0;
}

bool CountryRgnDatHandler::UserEnterCity(DB_C_User* pDBCUser,int nCityID)
{
	CCity* pCity = GetCity(nCityID);
	if(pCity==NULL||pDBCUser==NULL)
		return false;
	bool bIsChampion = IsChampion(pDBCUser->userid());
	pCity->UserEnterCity(pDBCUser,bIsChampion);
	return true;
}

bool CountryRgnDatHandler::UserChgCity(DB_C_User* pDBCUser,int nOldCityID,int nNewCityID)
{
	//UserLeaveCity(nUserID,nOldCityID);
	CCity* pOldCity = GetCity(nOldCityID);
	CCity* pNewCity = GetCity(nNewCityID);
	if(pOldCity==NULL||pNewCity==NULL||pDBCUser==NULL)
		return false;
	bool bIsChampion = IsChampion(pDBCUser->userid());
	pOldCity->UserLeaveCity(pDBCUser,bIsChampion);
	bool bRet = UserEnterCity(pDBCUser,nNewCityID);
	return bRet;
}

bool CountryRgnDatHandler::CityChgCountry(int64 nUserID,CCity* pCity,int nNewCountry)
{
	if(!CountryInfoCfg::Instance().CanCapure(pCity->GetCityID()))
		return false;
	if(pCity->GetCountry()==nNewCountry)
		return true;
	CCountry* pNewCountry = GetCountry(nNewCountry);
	CCountry* pOldCountry = GetCountry(pCity->GetCountry());
	if(pNewCountry==NULL||pOldCountry==NULL)
		return false;
	pOldCountry->EraseCity(pCity->GetCityID());
	pNewCountry->AddCity(pCity);
	pCity->GetDBCity()->set_countryid(nNewCountry);
	pCity->m_bNPCCity = !(CountryInfoCfg::Instance().IsUserCountry(nNewCountry));
	m_pDH->saveCity(pCity);

	return true;
}

int	CountryRgnDatHandler::UserAddArena(CCountry* pCountry,DB_C_User* pCUser)
{
	if(pCountry==NULL || pCUser == NULL)
	{
		return -1;
	}
	
	if(pCountry->UpdateRank() == true)
	{
		markCountryDirty(pCountry);
	}

	return pCountry->AddNewPlayer(pCUser);
}

int	CountryRgnDatHandler::UserLeaveArena(CCountry* pCountry,DB_C_User* pCUser,int nChgNpcId)
{
	if(pCountry && pCUser)
	{
		pCountry->ExChangePos(pCUser,nChgNpcId);
		pCountry->ClearScoreGiftIndex(pCUser->userid());
	}
	return 0;
}

int	CountryRgnDatHandler::GetArenaPlayerNum()
{
	int nTotal = 0;
	map<int,CCountry*>::iterator iter = m_mapCountry.begin();
	for(;iter != m_mapCountry.end();iter ++)
	{
		CCountry* pCountry = iter->second;
		if(pCountry)
		{
			nTotal += pCountry->GetArenaPlayerNum();
		}
	}
	return nTotal;
}


int CountryRgnDatHandler::CheckHasWay(int64 nUserID,CCity* pOldCity,CCity* pNewCity,int& nNeedThew)
{
	//该函数判断自己城市到自己国家所有城市和自己国家所有相邻城市的路线
	if(pOldCity==NULL||pNewCity==NULL)
		return 8;
	//if(pNewCity->GetCountry()>0&&pNewCity->GetCountry()!=pNewCity->GetCountry())
	//	return 6;
	nNeedThew = 0;
	int n1CityThew = CountryInfoCfg::Instance().GetCountryBaseTbl()->m_nMoveCostThew;


	int nCountryID = pOldCity->GetCountry();
	int nOldCityID = pOldCity->GetCityID();
	int nNewCityID = pNewCity->GetCityID();
	if(CountryInfoCfg::Instance().IsNearCity(pOldCity->GetCityID(),pNewCity->GetCityID()))
	{//相邻的城市
		nNeedThew = n1CityThew;
		return 0;
	}

	//static int	wayPT[WAY_CITY_CNT];	//城市寻路使用的路点
	//static int  wayValue[WAY_CITY_CNT];
	//memset(wayPT,0,WAY_CITY_CNT*sizeof(int));
	//memset(wayValue,0,WAY_CITY_CNT*sizeof(int));
	//临时修改sskylin
	//nNeedThew = n1CityThew;
	//return 0;
	map<int,int> mapValue;		//城市权值		
	map<int,int> mapWay;		//城市的上一个城市ID
	int nMinValue = 1000;
	int nIdx1 = 0 , nIdx2 = 0;
	mapValue[nOldCityID] = 1;		
	mapWay[nOldCityID] = nOldCityID;
	_FindWay(nOldCityID,nOldCityID,nNewCityID,nCountryID,mapWay,mapValue,nMinValue);
	if(nMinValue >= 1000)
	{//无路
		return 8;
	}

#ifdef WIN32
	//调试路点
	vector<int> lstIDLst;		//能到达城市的最近通路
	lstIDLst.push_back(nNewCityID);
	int nPreID = mapWay[nNewCityID];
	int nCnt = WAY_CITY_CNT;
	
	while(nPreID != nOldCityID && nCnt>0)
	{
		lstIDLst.push_back(nPreID);
		nPreID = mapWay[nPreID];
		nCnt--;
	}
	lstIDLst.push_back(nPreID);
	char szTmp[64];
	string strWay;
	for(int i=lstIDLst.size()-1;i>=0;i--)
	{
		sprintf(szTmp,"%d - ",lstIDLst[i]);
		strWay+= strWay;
	}
	SYS_LOG(nUserID,LT_Debug,0,0,lstIDLst.size()<<strWay);
#endif
	nNeedThew = (nMinValue-1) * n1CityThew;
	return 0;
}

void CountryRgnDatHandler::_FindWay(int nInitCityID,int nFromCityID,int nTarCityID,int nCountryID,map<int,int>& mapWay,map<int,int>& mapValue,int& nMinValue)
{
	const vector<int>* pWay = CountryInfoCfg::Instance().GetWayPoints(nFromCityID);
	if(pWay == NULL)
		return ;

	int nValue = mapValue[nFromCityID] + 1;
	if(nValue >= nMinValue) //没必要遍历
		return;

	vector<int>::const_iterator iter;
	map<int,int>::iterator itMap;
	vector<int> lstFindID;	//需深度遍历的城市ID
	for(iter = pWay->begin();iter!=pWay->end();iter++)
	{//遍历第一层
		int nWID = *iter;
		if(nWID == nTarCityID)
		{//找到了
			if(nValue < nMinValue)
			{
				mapValue[nTarCityID] = nValue;
				mapWay[nTarCityID]   = nFromCityID;
				nMinValue = nValue;
			}
			return;
		}

		CCity* pFindCity = GetCity(nWID);
		if(pFindCity==NULL||pFindCity->GetCountry()!=nCountryID)
			continue;
		if(mapWay[nWID] == 0)
		{
			mapWay[nWID] = nFromCityID;
			mapValue[nWID] = nValue;
			lstFindID.push_back(nWID);
		}
		else if(nValue < mapValue[nWID])
		{
			mapWay[nWID] = nFromCityID;
			mapValue[nWID] = nValue;
			lstFindID.push_back(nWID);
		}
	}

	//遍历第二层
	for(int i=0;i<(int)lstFindID.size();i++)
	{
		int nID = lstFindID[i];
		_FindWay(nInitCityID,nID,nTarCityID,nCountryID,mapWay,mapValue,nMinValue);
	}
}

void CountryRgnDatHandler::FillNPCLst(CCity* pCity,int nPage,RseCountryUserLst* pSendLst)
{
	if(pCity==NULL||pSendLst==NULL)
		return;
	if(nPage<0)
		nPage = 0;
	DB_C_City* pDBCity = pCity->GetDBCity();
	int nSize = pCity->GetUserCnt();
	if(nSize<=0)
		return;
	pSendLst->set_ncityid(pCity->GetCityID());
	//int nIdx = rand()%nSize;
	int nIdx = nPage * SEND_CITY_USER_CNT;
	int nCnt = 0;
	for(int i=nIdx;i<(int)pCity->m_lstNPC.size()&&nCnt<SEND_CITY_USER_CNT;i++)
	{
		DB_C_NPC* pUser = pCity->m_lstNPC[i];
		if(pUser->npcid()<=0)
			continue;
		int nNPCTblID = pUser->npcid()/COUNTRY_NPC_RATE;
		const CountryNPC* pNPC = CountryInfoCfg::Instance().GetCountryNPC(nNPCTblID);
		if(pNPC==NULL)
			continue;
		CountryUser* pSendUser = pSendLst->add_user();
		pSendUser->set_baseid(toString(pUser->npcid()));
		pSendUser->set_morale(pUser->morale());
		pSendUser->set_first_name("");
		pSendUser->set_combatpowervalue(pNPC->m_nCombatValue);
		pSendUser->set_lvl(pNPC->m_nLevel);
		
		nCnt++;
	}

	//for(int i=0;i<nIdx&&nCnt<SEND_CITY_USER_CNT;i++)
	//{
	//	DB_C_NPC* pUser = pDBCity->mutable_npc(i);
	//	if(pUser->npcid()<=0)
	//		continue;
	//	int nNPCTblID = pUser->npcid()/1000;
	//	const CountryNPC* pNPC = CountryInfoCfg::Instance().GetCountryNPC(nNPCTblID);
	//	if(pNPC==NULL)
	//		continue;
	//	CountryUser* pSendUser = pSendLst->add_user();
	//	pSendUser->set_baseid(toString(pUser->npcid()));
	//	pSendUser->set_morale(pUser->morale());
	//	pSendUser->set_first_name("");
	//	pSendUser->set_combatpowervalue(pNPC->m_nCombatValue);
	//	pSendUser->set_lvl(pNPC->m_nLevel);

	//	nCnt++;
	//}

}

void CountryRgnDatHandler::FillUserIDLst(CCity* pCity,int nPage,GCG_CountryOpt* pRetData,bool bFillAll/* =false */,RseCountryUserLst* pRseUserLst /* = NULL */)
{
	if(pCity==NULL||pRetData==NULL)
		return;
	
	//DB_C_City* pDBCity = pCity->GetDBCity();
	int nSize = pCity->GetUserCnt();
	if(nSize<=0)
		return;
	if(nPage<0)
		nPage = 0;
	bool bHasError = false;
	int nSpyType = pRetData->syptype();
	int nNoDataCnt = 0;
	if(bFillAll)
	{//填充所有玩家
		for(int i=0;i<CITY_USER_GROUP;i++)
		{
			for(int j=0;j<(int)pCity->m_lstUser[i].size();j++)
			{
				DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
				if(pDBCUser==NULL)
				{
					bHasError = true;
					continue;
				}
				if(pDBCUser->cityid()!=pCity->GetCityID())
				{
					bHasError = true;
					continue;
				}
				if(nSpyType == 0 && pRseUserLst)
				{
					CountryUser* pSendUser = pRseUserLst->add_user();
					pSendUser->set_baseid(toString(pDBCUser->userid()));
					pSendUser->set_first_name(pDBCUser->name());
					pSendUser->set_pic(pDBCUser->pic());
					pSendUser->set_lvl(pDBCUser->level());
					pSendUser->set_morale(0);
					pSendUser->set_combatpowervalue(0);
					pSendUser->set_officerposition(pDBCUser->officerposition());
					pSendUser->set_viplevel(pDBCUser->viplevel());
					pSendUser->set_viptype(pDBCUser->viptype());
					if(pDBCUser->level()<=0)
						nNoDataCnt++;
				}
				else
				{
					pRetData->add_userid(pDBCUser->userid());
				}
				
			}
		}
	}
	else
	{
		if(nSize<=SEND_CITY_USER_CNT)
		{
			for(int i=0;i<CITY_USER_GROUP;i++)
			{
				for(int j=0;j<(int)pCity->m_lstUser[i].size();j++)
				{
					DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
					if(pDBCUser==NULL)
					{
						bHasError = true;
						continue;
					}
					if(pDBCUser->cityid()!=pCity->GetCityID())
					{
						bHasError = true;
						continue;
					}
					//pRetData->add_userid(pDBCUser->userid());
					if(nSpyType == 0 && pRseUserLst)
					{
						CountryUser* pSendUser = pRseUserLst->add_user();
						pSendUser->set_baseid(toString(pDBCUser->userid()));
						pSendUser->set_first_name(pDBCUser->name());
						pSendUser->set_pic(pDBCUser->pic());
						pSendUser->set_lvl(pDBCUser->level());
						pSendUser->set_morale(0);
						pSendUser->set_combatpowervalue(0);
						pSendUser->set_officerposition(pDBCUser->officerposition());
						pSendUser->set_viplevel(pDBCUser->viplevel());
						pSendUser->set_viptype(pDBCUser->viptype());
						if(pDBCUser->level()<=0)
							nNoDataCnt++;
					}
					else
					{
						pRetData->add_userid(pDBCUser->userid());
					}
				}
			}
		}
		else
		{
			int nStart = nPage*SEND_CITY_USER_CNT;
			int nCnt = 0;
			int nIdx = 0;
			int i=0;
			for(i=0;i<CITY_USER_GROUP;i++)
			{
				int nSize = pCity->m_lstUser[i].size();
				if(nSize < nStart)
				{
					nStart -= pCity->m_lstUser[i].size();
					if(nStart<0)
						nStart = 0;
					continue;
				}
				for(int j=nStart;j<nSize&&nCnt<SEND_CITY_USER_CNT;j++)
				{
					DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
					if(pDBCUser==NULL)
					{
						bHasError = true;
						continue;
					}
					if(pDBCUser->cityid()!=pCity->GetCityID())
					{
						bHasError = true;
						continue;
					}
					nCnt++;
					//pRetData->add_userid(pDBCUser->userid());
					if(nSpyType == 0 && pRseUserLst)
					{
						CountryUser* pSendUser = pRseUserLst->add_user();
						pSendUser->set_baseid(toString(pDBCUser->userid()));
						pSendUser->set_first_name(pDBCUser->name());
						pSendUser->set_pic(pDBCUser->pic());
						pSendUser->set_lvl(pDBCUser->level());
						pSendUser->set_morale(0);
						pSendUser->set_combatpowervalue(0);
						pSendUser->set_officerposition(pDBCUser->officerposition());
						pSendUser->set_viplevel(pDBCUser->viplevel());
						pSendUser->set_viptype(pDBCUser->viptype());
						if(pDBCUser->level()<=0)
							nNoDataCnt++;
					}
					else
					{
						pRetData->add_userid(pDBCUser->userid());
					}
				}
				i++;
				break;
			}
			for(;i<CITY_USER_GROUP&&nCnt<SEND_CITY_USER_CNT;i++)
			{
				for(int j=0;j<(int)pCity->m_lstUser[i].size()&&nCnt<SEND_CITY_USER_CNT;j++)
				{
					DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
					if(pDBCUser==NULL)
					{
						bHasError = true;
						continue;
					}
					if(pDBCUser->cityid()!=pCity->GetCityID())
					{
						bHasError = true;
						continue;
					}
					nCnt++;
					//pRetData->add_userid(pDBCUser->userid());
					if(nSpyType == 0 && pRseUserLst)
					{
						CountryUser* pSendUser = pRseUserLst->add_user();
						pSendUser->set_baseid(toString(pDBCUser->userid()));
						pSendUser->set_first_name(pDBCUser->name());
						pSendUser->set_pic(pDBCUser->pic());
						pSendUser->set_lvl(pDBCUser->level());
						pSendUser->set_morale(0);
						pSendUser->set_combatpowervalue(0);
						pSendUser->set_officerposition(pDBCUser->officerposition());
						pSendUser->set_viplevel(pDBCUser->viplevel());
						pSendUser->set_viptype(pDBCUser->viptype());
						if(pDBCUser->level()<=0)
							nNoDataCnt++;
					}
					else
					{
						pRetData->add_userid(pDBCUser->userid());
					}
				}
			}
		}
	}
	if(pRseUserLst)
	{
		pRseUserLst->set_nodatacnt(nNoDataCnt);
	}
	if(bHasError)
	{
		pCity->ResetUserLst();
	}
}


void	CountryRgnDatHandler::ClearPlayerArenaIndex(bool bSaveLast /* = true */)
{
	//清除老的官员数据
	if(bSaveLast)
	{
		for(map<int,CCountry*>::iterator iter = m_mapCountry.begin();iter != m_mapCountry.end();iter ++)
		{
			CCountry* pCountry = iter->second;
			if(pCountry)
			{
				ClearCountryOfficer(pCountry,E_Officer_Type_Arena);
				markCountryDirty(pCountry);
			}
		}
	}

	for(hash_map<int64,DB_C_User*>::iterator iter = m_mapUser.begin();iter != m_mapUser.end();iter ++)
	{
		DB_C_User* pUser = iter->second;
		if(pUser)
		{
			if(bSaveLast)
			{
				pUser->set_lastarenaindex(pUser->arenaindex());
				//竞技场排名产生官员
				UpdateOfficerPosition(pUser,pUser->lastarenaindex(),pUser->countryid(),E_Officer_Type_Arena);
				if(pUser->arenaindex()>=0&&pUser->arenaindex()<200)
				{
					SYS_LOG(0,LT_ARENA_COUNTRY_RANKLAST,m_nRegion,0,(int64)pUser->userid()<<pUser->arenaindex()<<pUser->countryid());
				}
			}
			if(pUser->arenaindex() != -1)
			{
				pUser->set_arenaindex(-1);
			}
		}
	}

	for(map<int,DB_C_UserList*>::iterator iter = m_mapUserLst.begin();iter != m_mapUserLst.end();iter ++)
	{
		DB_C_UserList* pUserList = iter->second;
		if(pUserList)
		{
			markUserLstDirty(pUserList);
		}
	}
}

void	CountryRgnDatHandler::ClearCountryArenaData()
{
	for(map<int,CCountry*>::iterator iter = m_mapCountry.begin();iter != m_mapCountry.end();iter ++)
	{
		CCountry* pCountry = iter->second;
		if(pCountry)
		{
			pCountry->CleraArenaData();
			markCountryDirty(pCountry);
		}
	}
}

void	CountryRgnDatHandler::CheckClearCountryArenaData()
{
	int nActiveID = ArenaConfig_Active::Instance().GetArenaActiveID();
	int nOldId	=	m_earthCountry.GetDBCountry()->nactivityindex();
	if(nActiveID != nOldId)
	{
		bool bSaveLast = nOldId>0?true:false;
		ClearPlayerArenaIndex(bSaveLast);
		ClearCountryArenaData();

		m_earthCountry.GetDBCountry()->set_nactivityindex(nActiveID);
		markCountryDirty(&m_earthCountry);

		SYS_LOG(0,LT_ARENA_COUNTRY_RELOADDATA,m_nRegion,0,nActiveID<<nOldId<<time(NULL));
	}
}

void CountryRgnDatHandler::ClearUnuseBloodBtl()
{
	time_t ltNow = time(NULL);

	map<int,DB_C_BloodBtl*>::iterator iter,iterTmp;
	iter = m_mapCurBloodBtl.begin();
	while(iter != m_mapCurBloodBtl.end())
	{
		DB_C_BloodBtl* pBtl = iter->second;
		if(pBtl==NULL)
		{
			iterTmp = iter;
			iter++;
			m_mapCurBloodBtl.erase(iterTmp);
			continue;
		}
		if(pBtl->launchtime()<=0)
		{
			pBtl->Clear();
			iterTmp = iter;
			iter++;
			m_mapCurBloodBtl.erase(iterTmp);
			continue;
		}
		if(pBtl->overtime()>0 && (pBtl->overtime()+CountryInfoCfg::Instance().GetBloodBtlCityProtectTm(pBtl->type())<ltNow))
		{
			pBtl->Clear();
			iterTmp = iter;
			iter++;
			m_mapCurBloodBtl.erase(iterTmp);
			continue;
		}
		iter++;
	}
}

bool CountryRgnDatHandler::HasLaunchBloodBtl(int nCountryID)
{
	map<int,DB_C_BloodBtl*>::iterator iter;
	for(iter = m_mapCurBloodBtl.begin(); iter!= m_mapCurBloodBtl.end();iter++)
	{
		DB_C_BloodBtl* pBtl = iter->second;
		if(pBtl==NULL||pBtl->launchtime()<=0||pBtl->overtime()>0)
			continue;
		if(pBtl->atkcountry()==nCountryID)
		{
			return true;
		}
	}
	return false;
}

DB_C_BloodBtl*  CountryRgnDatHandler::GetDBBloodBtl(CCity* pCity,bool bCreateIfNotExit/* =false */)
{
	map<int,DB_C_BloodBtl*>::iterator iter;
	iter = m_mapCurBloodBtl.find(pCity->GetCityID());
	if(iter != m_mapCurBloodBtl.end())
		return iter->second;
	if(!bCreateIfNotExit)
		return NULL;

	DB_C_BloodBtl* pDBBtl = pCity->GetDBCity()->mutable_lastbloodbtl();
	m_mapCurBloodBtl[pCity->GetCityID()] = pDBBtl;
	return pDBBtl;

}

DB_C_BloodBtl* CountryRgnDatHandler::GetLastDBBloodBtl(int nCityID)
{
	map<int,DB_C_BloodBtl*>::iterator iter;
	DB_C_BloodBtl* pBloodBtl = NULL;

	for(iter = m_mapCurBloodBtl.begin(); iter!= m_mapCurBloodBtl.end();iter++)
	{
		DB_C_BloodBtl* pBtl = iter->second;
		if(pBtl==NULL||pBtl->launchtime()<=0)
			continue;
		if(pBtl->atkcity()==nCityID || pBtl->defcity() == nCityID)
		{
			if(pBloodBtl==NULL)
			{
				pBloodBtl=pBtl;
			}
			else 
			{
				if(pBtl->overtime()<=0)
					return pBtl;//血战未结束，直接返回
				if(pBloodBtl->overtime() < pBtl->overtime())
				{
					pBloodBtl = pBtl;
				}
			}
		}
	}
	return pBloodBtl;
}

void CountryRgnDatHandler::FillBloodBtl(const DB_C_BloodBtl* pDBBtl,CityBloodBattle* pInfo,bool bFillStep)
{
	if(pInfo==NULL||pDBBtl==NULL)
		return;
	pInfo->set_launchuserid(toString(pDBBtl->launchuserid()));
	pInfo->set_launchname(pDBBtl->launchname());
	pInfo->set_launchtime(pDBBtl->launchtime());

	pInfo->set_atkcountry(pDBBtl->atkcountry());
	pInfo->set_atkcity(pDBBtl->atkcity());
	pInfo->set_atktotalcnt(pDBBtl->atktotalcnt());
	pInfo->set_atkleftcnt(pDBBtl->atkleftcnt());

	pInfo->set_defcountry(pDBBtl->defcountry());
	pInfo->set_defcity(pDBBtl->defcity());
	pInfo->set_deftotalcnt(pDBBtl->deftotalcnt());
	pInfo->set_defleftcnt(pDBBtl->defleftcnt());

	pInfo->set_wincity(pDBBtl->wincity());
	pInfo->set_overtime(pDBBtl->overtime());

	pInfo->set_type(pDBBtl->type());
	if(bFillStep)
	{
		for(int i=0;i<pDBBtl->btlstep_size();i++)
		{
			const DB_C_BloodBtlStep& dbStep = pDBBtl->btlstep(i);
			CityBloodBtlStep* pStep = pInfo->add_btlstep();
			pStep->set_time(dbStep.time());
			pStep->set_atkid(dbStep.atkid());
			pStep->set_atkname(dbStep.atkname());
			pStep->set_atkcountry(dbStep.atkcountry());
			pStep->set_defid(dbStep.defid());
			pStep->set_defname(dbStep.defname());
			pStep->set_defcountry(dbStep.defcountry());
			pStep->set_winid(dbStep.winid());
		}
	}
}

void CountryRgnDatHandler::SetBloodBtlCacuTime(int nCity)
{
	m_mapBloodBtlCacuTime[nCity] = int(time(NULL));
}

int CountryRgnDatHandler::GetBloodBtlCacuTime(int nCity)
{
	map<int,int>::iterator iter;
	iter = m_mapBloodBtlCacuTime.find(nCity);
	if(iter!=m_mapBloodBtlCacuTime.end())
		return iter->second;
	return 0;
}

int CountryRgnDatHandler::GetBloodBtlStep(int nCity)
{
	return m_mapBloodBtlStep[nCity];
}
void CountryRgnDatHandler::SetBloodBtlStep(int nCity,CountryBloodBtlStepType emStep)
{
	m_mapBloodBtlStep[nCity] = emStep;
}

void	CountryRgnDatHandler::ClearWorldArenaData()
{
	CCountry* pEarth = GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	pEarth->ClearWorldArenaData();
	markCountryDirty(pEarth);
}
void	CountryRgnDatHandler::ClearWorldArenaIndex(bool bSaveLast /* = true */)
{
	for(hash_map<int64,DB_C_User*>::iterator iter = m_mapUser.begin();iter != m_mapUser.end();iter ++)
	{
		DB_C_User* pUser = iter->second;
		if(pUser)
		{
			if(bSaveLast)
			{
				pUser->set_lastworldarenaindex(pUser->worldarenaindex());
				SYS_LOG(0,LT_ARENA_WORLD_RANKLAST,0,0,(int64)pUser->userid()<<pUser->worldarenaindex()<<pUser->countryid());
			}
			if(pUser->worldarenaindex() != -1)
			{
				pUser->set_worldarenaindex(-1);
			}
		}
	}

	for(map<int,DB_C_UserList*>::iterator iter = m_mapUserLst.begin();iter != m_mapUserLst.end();iter ++)
	{
		DB_C_UserList* pUserList = iter->second;
		if(pUserList)
		{
			markUserLstDirty(pUserList);
		}
	}
}
void	CountryRgnDatHandler::UpdateWorldArenaData()
{
	CCountry* pEarth = GetEarthCountry();
	if(pEarth==NULL)
	{
		return;
	}
	DB_WorldArena_Rank* pDBWorldRank = pEarth->GetDBCountry()->mutable_worldarenarank();
	if(pDBWorldRank)
	{
		//每天更新
		if(true==pEarth->UpdateWorldRankEveryDay())
		{
			markCountryDirty(pEarth);
		}
		//每期更新
		int nActiveID = WorldArenaActive::Instance().GetArenaActiveID();
		int nOldActiveID	=	pDBWorldRank->currentactivityindex();
		if(nActiveID != nOldActiveID)
		{
			if(nOldActiveID>0)
			{
				m_earthCountry.SaveWorldArenaRankLast();
			}
			bool bSaveLast = nOldActiveID>0?true:false;
			ClearWorldArenaIndex(bSaveLast);
			ClearWorldArenaData();
			pEarth->UpdateWorldRankNpc();
			pDBWorldRank->set_currentactivityindex(nActiveID);
			markCountryDirty(pEarth);
			SYS_LOG(0,LT_ARENA_WORLD_RELOADDATA,m_nRegion,0,nActiveID<<nOldActiveID);
		}
	}
}






void CountryRgnDatHandler::_UpdateWorldArenaUserList(DB_C_User* pUser,vector<DB_Arena_Node>& rUserVec,vector<DB_Arena_Node>& rErrVec)
{
	if(pUser == NULL)
	{
		return;
	}

	int64 nUid = pUser->userid();
	int nPos = pUser->worldarenaindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		//pUser->set_worldarenaindex(nPos + 1);
		//_UpdateWorldArenaUserList(pUser,rUserVec,bRankError);
		DB_Arena_Node rNode;
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rErrVec.push_back(rNode);
		if(rOldNode.pUser != NULL)
		{
			SYS_LOG(0,LT_ARENA_WORLD_RANKERRO,m_nRegion,0,0<<(int64)pUser->userid()<<(int64)rOldNode.pUser->userid()<<nPos);
		}
		else if(rOldNode.pNpc != NULL)
		{
			SYS_LOG(0,LT_ARENA_WORLD_RANKERRO,m_nRegion,0,1<<(int64)pUser->userid()<<rOldNode.pNpc->npcid()<<nPos);
		}
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rUserVec[nPos] = rNode;
	}
}void CountryRgnDatHandler::_UpdateWorldArenaErrUserList(DB_C_User* pUser,vector<DB_Arena_Node>& rUserVec)
{
	if(pUser == NULL)
	{
		return;
	}

	int64 nUid = pUser->userid();
	int nPos = pUser->worldarenaindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		pUser->set_worldarenaindex(nPos + 1);
		_UpdateWorldArenaErrUserList(pUser,rUserVec);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rUserVec[nPos] = rNode;
	}
}
void CountryRgnDatHandler::_UpdateWorldArenaNpcList(DB_Arena_Rank_NPC* pNpc,vector<DB_Arena_Node>& rUserVec)
{
	if(pNpc == NULL)
	{
		return;
	}

	int64 nUid = pNpc->npcid();
	int nPos = pNpc->npcindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	rNode.pUser = NULL;
	rNode.pNpc = NULL;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		pNpc->set_npcindex(nPos + 1);
		_UpdateWorldArenaNpcList(pNpc,rUserVec);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = NULL;
		rNode.pNpc = pNpc;
		rUserVec[nPos] = rNode;
	}
}
void CountryRgnDatHandler::UpdateWorldArenaList(DB_WorldArena_Rank* pArenaR,vector<int64>& rVecUid)
{
	if(pArenaR == NULL)
	{
		return;
	}

	rVecUid.clear();

	vector<DB_Arena_Node> rVecUserTmp;
	rVecUserTmp.clear();
	vector<DB_Arena_Node> rVecErrUserTmp;
	rVecErrUserTmp.clear();

	//玩家
	for(int countryIndex=C_Start;countryIndex<C_UserCoutryEnd;countryIndex++)
	{
		vector<DB_C_User*>& rvecList = m_vecCountryUser[countryIndex];
		for(int i=0;i<(int)rvecList.size();i++)
		{
			DB_C_User* pUser = rvecList[i];
			if(pUser && pUser->countryid() ==countryIndex&&pUser->worldarenaindex()>=0)
			{
				_UpdateWorldArenaUserList(pUser,rVecUserTmp,rVecErrUserTmp);
			}
		}

	}
	//名次冲突玩家
	for(int i=0;i<(int)rVecErrUserTmp.size();i++)
	{
		DB_Arena_Node& rNode = rVecErrUserTmp[i];
		if(rNode.pUser)
		{
			_UpdateWorldArenaErrUserList(rNode.pUser,rVecUserTmp);
			SYS_LOG(0,LT_ARENA_WORLD_RANKERRO,m_nRegion,0,2<<(int64)rNode.pUser->userid()<<(int64)rNode.pUser->userid()<<rNode.pUser->worldarenaindex());
		}
	}

	//NPC
	for(int i=0;i<pArenaR->worldarenaranknpc_size();i++)
	{
		_UpdateWorldArenaNpcList(pArenaR->mutable_worldarenaranknpc(i),rVecUserTmp);
	}

	for(int i=0;i<(int)rVecUserTmp.size();i++)
	{
		DB_Arena_Node& rNode = rVecUserTmp[i];
		if(rNode.pUser)
		{
			rVecUid.push_back(rNode.pUser->userid());
			rNode.pUser->set_worldarenaindex(rVecUid.size() - 1);
			markUserLstDirty(rNode.pUser->userid());
		}

		if(rNode.pNpc)
		{
			rVecUid.push_back(rNode.pNpc->npcid());
			rNode.pNpc->set_npcindex(rVecUid.size() - 1);
		}
	}
	markCountryDirty(&m_earthCountry);
}


bool	 CCountry::GetWorldRankInfo(CountryRgnDatHandler* pRDH,RseWorldArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL||pRDH==NULL)
	{
		return false;
	}

	DB_WorldArena_Rank* pDBWorldArenaRank = m_dbCountry.mutable_worldarenarank();

	//获取昨日排行榜
	WorldArenaRank* pWorldArenaRank = pArenaRankInfo->mutable_arenaranklist();
	DB_WorldArena_Rank_List* pDBWorldArenaRankLast = pDBWorldArenaRank->mutable_worldarenaranklast();
	pWorldArenaRank->set_ranktime(pDBWorldArenaRankLast->ranktimelast());
	pWorldArenaRank->set_activityindex(pDBWorldArenaRankLast->activityindex());
	for(int i=0;i<pDBWorldArenaRankLast->worldarenaranklist_size();i++)
	{
		DB_WorldArena_Rank_Unit* pDBUnit = pDBWorldArenaRankLast->mutable_worldarenaranklist(i);
		if(pDBUnit)
		{
			WorldArenaRankUnit* pUnit = pWorldArenaRank->add_arenaranklist();
			pUnit->set_index(i);
			pUnit->set_uid(toString(pDBUnit->uid()));
		}
	}
	return true;
}

bool	 CCountry::GetWorldRankFightInfo(CountryRgnDatHandler* pRDH,int nPos,RseWorldArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL||pRDH==NULL)
	{
		return false;
	}

	DB_WorldArena_Rank* pDBWorldArenaRank = m_dbCountry.mutable_worldarenarank();

	//获取可攻击的敌人
	WorldArenaFightRuleUnit* pRuleUnit = WorldArenaFightRule::Instance().GetWalkSpaceByRankIndex(nPos);
	if(pRuleUnit == NULL)
	{
		return false;
	}

	WorldArenaRank* pArenaRankList = pArenaRankInfo->mutable_arenaranklist();
	pArenaRankList->set_ranktime((int)time(NULL));
	pArenaRankList->set_activityindex(pDBWorldArenaRank->currentactivityindex());


	for(int i=0;i<WORLD_ARENA_FIGHT_RULE_NUM;i++)
	{
		WorldArenaRankUnit* pUnit = pArenaRankList->add_arenaranklist();

		int nIndex = nPos;

		switch(pRuleUnit->m_nwalkWay)
		{
		case E_WORLD_ARENA_FIGHT_RULE_PEC:		//乘百分比
			{
				nIndex = nIndex * pRuleUnit->m_nArgv[i] / 100;
			}
			break;
		case E_WORLD_ARENA_FIGHT_RULE_DEL:		//减数值
			{
				nIndex = nIndex - pRuleUnit->m_nArgv[i];
			}
			break;
		case E_WORLD_ARENA_FIGHT_RULE_INDEX:		//名次值
			{
				nIndex = pRuleUnit->m_nArgv[i];
			}
			break;
		}
		int64 uid = GetRankUidByIndex(nIndex);
		pUnit->set_index(nIndex);
		pUnit->set_uid(toString(uid));
	}

	WorldArenaRank* pArenatopList = pArenaRankInfo->mutable_arenaattacklist();
	pArenatopList->set_ranktime((int)time(NULL));
	pArenatopList->set_activityindex(pDBWorldArenaRank->currentactivityindex());

	//前10名
	if(pRuleUnit->m_nwalkWay != E_WORLD_ARENA_FIGHT_RULE_INDEX)
	{
		for(int i=0;i<WORLD_ARENA_FIGHT_RULE_NUM;i++)
		{
			int64 uid = GetRankUidByIndex(i);
			WorldArenaRankUnit* pUnit = pArenatopList->add_arenaranklist();
			pUnit->set_index(i);
			pUnit->set_uid(toString(uid));
		}
	}

	return true;
}

bool	 CCountry::GetWorldALLRankInfo(CountryRgnDatHandler* pRDH,RseWorldArenaRankInfo* pArenaRankInfo,int activeId)
{
	if(pArenaRankInfo == NULL||pRDH==NULL||activeId<0)
	{
		return false;
	}

	//获取每期排行榜
	DB_WorldArena_Rank* pDBWorldArenaRank = m_dbCountry.mutable_worldarenarank();
	for(int i=pDBWorldArenaRank->worldarenarankall_size()-1;i>=0;i--)
	{
		DB_WorldArena_Rank_List* pDBWorldArenaRankEvery = pDBWorldArenaRank->mutable_worldarenarankall(i);
		if(pDBWorldArenaRankEvery->activityindex()>0&&activeId==pDBWorldArenaRankEvery->activityindex())
		{
			WorldArenaRank* pWorldArenaRank = pArenaRankInfo->add_arenaalllist();
			pWorldArenaRank->set_ranktime(pDBWorldArenaRankEvery->ranktimelast());
			pWorldArenaRank->set_activityindex(pDBWorldArenaRankEvery->activityindex());
			for(int j=0;j<pDBWorldArenaRankEvery->worldarenaranklist_size();j++)
			{
				DB_WorldArena_Rank_Unit* pDBUnit = pDBWorldArenaRankEvery->mutable_worldarenaranklist(j);
				if(pDBUnit)
				{
					WorldArenaRankUnit* pUnit = pWorldArenaRank->add_arenaranklist();
					pUnit->set_index(j);
					pUnit->set_uid(toString(pDBUnit->uid()));
				}
			}
			break;
		}
	}
	return true;
}

bool	 CCountry::GetWorldLastRankInfo(CountryRgnDatHandler* pRDH,RseWorldArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL||pRDH==NULL)
	{
		return false;
	}
	//获取上期排行榜
	DB_WorldArena_Rank* pDBWorldArenaRank = m_dbCountry.mutable_worldarenarank();
	int nRecordIndex = pDBWorldArenaRank->rankrecordindex();
	if(nRecordIndex<MAX_WORLD_ARENA_ACTIVE_RECORD)
	{
		for(int i=pDBWorldArenaRank->worldarenarankall_size()-1;i>=0;i--)
		{
			DB_WorldArena_Rank_List* pDBWorldArenaRankEvery = pDBWorldArenaRank->mutable_worldarenarankall(i);
			if(pDBWorldArenaRankEvery->activityindex()>0)
			{
				WorldArenaRank* pWorldArenaTopRankLast = pArenaRankInfo->mutable_arenalasttoplist();
				pWorldArenaTopRankLast->set_ranktime(pDBWorldArenaRankEvery->ranktimelast());
				pWorldArenaTopRankLast->set_activityindex(pDBWorldArenaRankEvery->activityindex());
				int count = 0;
				for(int j=0;j<pDBWorldArenaRankEvery->worldarenaranklist_size()&&count<10;j++)
				{
					count++;
					DB_WorldArena_Rank_Unit* pDBUnit = pDBWorldArenaRankEvery->mutable_worldarenaranklist(j);
					if(pDBUnit)
					{
						WorldArenaRankUnit* pUnit = pWorldArenaTopRankLast->add_arenaranklist();
						pUnit->set_index(j);
						pUnit->set_uid(toString(pDBUnit->uid()));
					}
				}
				break;
			}
		}
	}
	else
	{
		for(int i=nRecordIndex;i>0;i--)
		{
			int nIndex = (nRecordIndex-1)%MAX_WORLD_ARENA_ACTIVE_RECORD;
			DB_WorldArena_Rank_List* pDBWorldArenaRankEvery = pDBWorldArenaRank->mutable_worldarenarankall(nIndex);
			if(pDBWorldArenaRankEvery->activityindex()>0)
			{
				WorldArenaRank* pWorldArenaTopRankLast = pArenaRankInfo->mutable_arenalasttoplist();
				pWorldArenaTopRankLast->set_ranktime(pDBWorldArenaRankEvery->ranktimelast());
				pWorldArenaTopRankLast->set_activityindex(pDBWorldArenaRankEvery->activityindex());
				int count = 0;
				for(int j=0;j<pDBWorldArenaRankEvery->worldarenaranklist_size()&&count<10;j++)
				{
					count++;
					DB_WorldArena_Rank_Unit* pDBUnit = pDBWorldArenaRankEvery->mutable_worldarenaranklist(j);
					if(pDBUnit)
					{
						WorldArenaRankUnit* pUnit = pWorldArenaTopRankLast->add_arenaranklist();
						pUnit->set_index(j);
						pUnit->set_uid(toString(pDBUnit->uid()));
					}
				}
				break;
			}
		}

	}
	
	return true;
}

int		 CCountry::AddWorldNewNpc(int64 nUid)
{
	if(m_nCountryID != 0)
	{
		return -1;
	}
	int nIndex = m_rvecPlayArena.size();
	m_rvecPlayArena.push_back(nUid);
	DB_WorldArena_Rank* pArenaRank = m_dbCountry.mutable_worldarenarank();
	DB_Arena_Rank_NPC* pArenaNpc = pArenaRank->add_worldarenaranknpc();
	pArenaNpc->set_npcindex(nIndex);
	pArenaNpc->set_npcid((int)nUid);
	m_rmapNpcArena[(int)nUid].push_back(pArenaNpc);
	return nIndex;
}

int		 CCountry::AddWorldNewPlayer(DB_C_User* pCUser)
{
	if(m_nCountryID != 0)
	{
		return -1;
	}
	if(pCUser == NULL || pCUser->countryid() <0)
	{
		return -1;
	}
	int nIndex = m_rvecPlayArena.size();
	if(pCUser->worldarenaindex() > 0)
	{
		return pCUser->worldarenaindex();
	}
	m_rvecPlayArena.push_back(pCUser->userid());
	pCUser->set_worldarenaindex(nIndex);
	SYS_LOG((int64)pCUser->userid(),LT_ARENA_WORLD_JION,m_nRegion,0,(int64)pCUser->userid()<<pCUser->worldarenaindex()<<pCUser->countryid());
	return nIndex;
}

int		 CCountry::CheckCanAddWorldArena(DB_C_User* pCUser)
{
	if(pCUser == NULL)
	{
		return E_WORLD_ARENA_OPT_REL_ERRDATAINIT;
	}

	//判断是否已经加入竞技场
	if(pCUser->worldarenaindex()>= 0)
	{
		return E_WORLD_ARENA_OPT_REL_ERRHASADDARENA;
	}
	if(pCUser->lastarenaindex()<0||pCUser->lastarenaindex()>=E_WORLD_ARENA_LIMIT)
	{
		return E_WORLD_ARENA_OPT_REL_NOT_TOP200;
	}

	return E_WORLD_ARENA_OPT_REL_OK;
}

void	 CCountry::InitWorldRankNpc()
{
	if(m_nCountryID != 0)
	{
		return ;
	}
	DB_WorldArena_Rank* pArenaRank = m_dbCountry.mutable_worldarenarank();
	pArenaRank->clear_worldarenaranknpc();

	int nNpcCunt = WorldArenaNpcInital::Instance().GetArenaNpcInitalUnitCnt();

	for(int i=0;i<nNpcCunt;i++)
	{
		WorldArenaNpcInitalUnit* pUnit = WorldArenaNpcInital::Instance().GetArenaNpcInitalUnitByIndex(i);
		if(pUnit)
		{
			for(int j=0;j<pUnit->m_nNum;j++)
			{
				AddWorldNewNpc(pUnit->m_nNpcID);
			}
		}
	}
}


bool	CCountry::UpdateWorldRankNpc()
{
	if(m_nCountryID != 0)
	{
		return false;
	}
	bool bUpdate = false;

	DB_WorldArena_Rank* pArenaRank = m_dbCountry.mutable_worldarenarank();
	if(pArenaRank->worldarenaranknpc_size() <= 0)
	{
		InitWorldRankNpc();
		bUpdate = true;
	}
	return bUpdate;
}


bool	CCountry::UpdateWorldRankEveryDay()
{
	if(m_nCountryID != 0)
	{
		return false;
	}
	bool bUpdate = UpdateWorldRankNpc();
	DB_WorldArena_Rank* pArenaRank = m_dbCountry.mutable_worldarenarank();
	DB_WorldArena_Rank_List* pWorldArenaRankLast = pArenaRank->mutable_worldarenaranklast();
	time_t nowTime = time(NULL);
	time_t lastTime = pWorldArenaRankLast->ranktimelast();

	struct tm nowtime = *localtime(&nowTime);
	struct tm lastime = *localtime(&lastTime);

	if (lastime.tm_yday != nowtime.tm_yday || lastime.tm_year != nowtime.tm_year)
	{		
		//更新昨日排行
		pWorldArenaRankLast->clear_worldarenaranklist();
		int nRankNow = E_WORLD_ARENA_RANK_NUM;
		if((int)m_rvecPlayArena.size() < nRankNow)
		{
			nRankNow = m_rvecPlayArena.size();
		}
		for(int i=0;i<nRankNow;i++)
		{
			int64 uid = m_rvecPlayArena[i];
			DB_WorldArena_Rank_Unit* pUnit=pWorldArenaRankLast->add_worldarenaranklist();
			pUnit->set_uid(uid);
		}
		pWorldArenaRankLast->set_ranktimelast((int)nowTime);
		pWorldArenaRankLast->set_activityindex(pArenaRank->currentactivityindex());
		bUpdate = true;
		SYS_LOG(0,LT_ARENA_WORLD_RANKSTART,m_nRegion,0,m_nCountryID<<lastTime<<nowTime<<nRankNow);
	}
	return bUpdate;
}


bool	CCountry::SaveWorldArenaRankLast()
{
	if(m_nCountryID != 0)
	{
		return false;
	}
	DB_WorldArena_Rank* pDBWorldRank = m_dbCountry.mutable_worldarenarank();
	if(NULL == pDBWorldRank)
	{
		return false;
	}
	int activityIndex = pDBWorldRank->currentactivityindex();
	if(activityIndex<=0)
	{
		return false;
	}
	time_t nowTime = time(NULL);
	DB_WorldArena_Rank_List* pDBWorldArenaNew = NULL;
	int nRecordIndex = pDBWorldRank->rankrecordindex();
	if(nRecordIndex<MAX_WORLD_ARENA_ACTIVE_RECORD)
	{
		pDBWorldArenaNew = pDBWorldRank->add_worldarenarankall();
	}
	else
	{
		int nIndex = nRecordIndex % MAX_WORLD_ARENA_ACTIVE_RECORD;
		pDBWorldArenaNew = pDBWorldRank->mutable_worldarenarankall(nIndex);
	}
	if(pDBWorldArenaNew)
	{
		if(pDBWorldRank->worldarenarankall_size()<MAX_WORLD_ARENA_ACTIVE_RECORD)
		{
			nRecordIndex = pDBWorldRank->worldarenarankall_size();
		}
		else
		{
			nRecordIndex+=1;
		}
		pDBWorldRank->set_rankrecordindex(nRecordIndex);
		pDBWorldArenaNew->clear_worldarenaranklist();
		int nRankNow = E_WORLD_ARENA_RANK_NUM;
		if((int)m_rvecPlayArena.size() < nRankNow)
		{
			nRankNow = m_rvecPlayArena.size();
		}
		for(int i=0;i<nRankNow;i++)
		{
			int64 uid = m_rvecPlayArena[i];
			DB_WorldArena_Rank_Unit* pUnit=pDBWorldArenaNew->add_worldarenaranklist();
			pUnit->set_uid(uid);
		}
		pDBWorldArenaNew->set_ranktimelast((int)nowTime);
		pDBWorldArenaNew->set_activityindex(activityIndex);
	}
	return true;
}



void	 CCountry::UpdateWorldArenaList(CountryRgnDatHandler* pRDH)
{
	if(m_nCountryID != 0)
	{
		return ;
	}
	if(pRDH)
	{
		DB_WorldArena_Rank* pArenaR = m_dbCountry.mutable_worldarenarank();
		pRDH->UpdateWorldArenaList(pArenaR,m_rvecPlayArena);
		m_rmapNpcArena.clear();
		for(int i=0;i<pArenaR->worldarenaranknpc_size();i++)
		{
			DB_Arena_Rank_NPC* pNode = pArenaR->mutable_worldarenaranknpc(i);
			if(pNode)
			{
				m_rmapNpcArena[pNode->npcid()].push_back(pNode);
			}
		}
	}
}

void	 CCountry::UpdateWorldArenaPos(int nPos,int64 nuidO,int64 nuidN)
{
	if(m_nCountryID!=0)
	{
		return;
	}
	if(nPos < 0 || nPos >= (int)m_rvecPlayArena.size())
	{
		return;
	}

	if(nuidO == m_rvecPlayArena[nPos])
	{
		m_rvecPlayArena[nPos] = nuidN;
	}
}


bool	 CCountry::ChangeWorldArenaPos(CountryRgnDatHandler* pRDH,int64 uidSelf,int posSelf,int64 uidFight,int posFight)
{
	if(pRDH==NULL||posSelf<0||posFight<0)
	{
		return false;
	}
	if(uidSelf >= 1000000000 && uidFight >= 1000000000)		//都是玩家
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_C_User* pCUserF = pRDH->GetDBCityUser(uidFight);

		if(pCUserS && pCUserS->worldarenaindex() == posSelf && pCUserF && pCUserF->worldarenaindex() == posFight)
		{

			UpdateWorldArenaPos(posSelf,pCUserS->userid(),pCUserF->userid());
			UpdateWorldArenaPos(posFight,pCUserF->userid(),pCUserS->userid());

			pCUserS->set_worldarenaindex(posFight);
			pCUserF->set_worldarenaindex(posSelf);
			return true;
		}
	}

	if(uidSelf >= 1000000000 && uidFight < 1000000000)		//玩家 NPC
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_Arena_Rank_NPC* pNpc = GetArenaNpc((int)uidFight,posFight);

		if(pCUserS && pCUserS->worldarenaindex() == posSelf && pNpc)
		{
			UpdateWorldArenaPos(posSelf,pCUserS->userid(),pNpc->npcid());
			UpdateWorldArenaPos(posFight,pNpc->npcid(),pCUserS->userid());

			pCUserS->set_worldarenaindex(posFight);
			pNpc->set_npcindex(posSelf);
			return true;
		}
	}

	return false;
}

bool	 CCountry::CheckWorldArenaPos(CountryRgnDatHandler* pRDH,int64 uidSelf,int posSelf,int64 uidFight,int posFight)
{
	if(pRDH==NULL||posSelf<0||posFight<0)
	{
		return false;
	}
	if(uidSelf >= 1000000000 && uidFight >= 1000000000)		//都是玩家
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_C_User* pCUserF = pRDH->GetDBCityUser(uidFight);

		if(pCUserS && pCUserS->worldarenaindex() == posSelf && pCUserF && pCUserF->worldarenaindex() == posFight)
		{
			return true;
		}
	}

	if(uidSelf >= 1000000000 && uidFight < 1000000000)		//玩家 NPC
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_Arena_Rank_NPC* pNpc = GetArenaNpc((int)uidFight,posFight);
		if(pCUserS && pCUserS->worldarenaindex() == posSelf && pNpc)
		{
			return true;
		}
	}

	return false;
}

int		 CCountry::CheckPlayerWorldRankIndex(CountryRgnDatHandler* pRDH,int64 nUid)
{
	if(m_nCountryID != 0||NULL == pRDH)
	{
		return -1;
	}
	DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
	if(pDBUser == NULL||pDBUser->worldarenaindex()<0)
	{
		return -1;
	}
	int   nIndex = pDBUser->worldarenaindex();
	int64 nUidTmp = GetRankUidByIndex(nIndex);
	if(nUid!=nUidTmp)
	{//数据错乱，重新排序
		DB_C_User* pDBUserTmp = pRDH->GetDBCityUser(nUidTmp);
		UpdateWorldArenaList(pRDH);
		SYS_LOG(0,LT_ARENA_WORLD_RERANK,m_nRegion,0,nIndex<<(int64)nUid<<pDBUser->worldarenaindex()<<(int64)nUidTmp<<pDBUserTmp->worldarenaindex());
	}
	return pDBUser->worldarenaindex();
}

E_Officer_Type CCountry::GetOfficerType(int nPosition)
{
	int nType = OfficerInfoCfg::Instance().GetOfficerType(nPosition);
	if(nType<=E_Officer_Type_NULL||nType>=Officer_Type_MAX)
	{
		return E_Officer_Type_NULL;
	}
	return (E_Officer_Type)nType;
	
}
int CountryRgnDatHandler::CreateOfficerPosition(int nIndex,int nCountryID,E_Officer_Type nType)
{
	if(nCountryID<C_AllCountry||nCountryID>=C_UserCoutryEnd)
	{
		return 0;
	}
	int nPosition = 0;
	if(nCountryID==C_AllCountry)
	{//世界
		if(nIndex==0&&nType==E_Officer_Type_Guild)
		{//世界工会战第一名产生大总统
			nPosition = Officer_Chairman;
		}
	}
	else
	{//国家
		nPosition = OfficerInfoCfg::Instance().GetOfficerPosition(nIndex,nType);
	}
	return nPosition;
}

void CountryRgnDatHandler::ClearCountryOfficer(CCountry* pCountry,E_Officer_Type nType)
{
	if(NULL == pCountry)
	{
		return;
	}
	DB_C_OfficerInfo *pDBOfficerInfo = pCountry->GetDBCountry()->mutable_officerinfo();
	if(E_Officer_Type_Arena == nType)
	{
		for(int i=0;i<pDBOfficerInfo->arenaofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_arenaofficers(i);
			int nPosition = pDBOfficerTmp->officerposition();
			if(nPosition>0&&pDBOfficerTmp->uid()>0)
			{
				DB_C_User* pDBCUser = GetDBCityUser(pDBOfficerTmp->uid());
				if(pDBCUser)
				{
					pDBCUser->set_officerposition(pDBCUser->officerposition()&(~(1<<nPosition)));
					SetCountryUserUpdateFlag(pDBOfficerTmp->uid(),true);
					markUserLstDirty(pDBCUser->userid());
				}
			}
		}
		pDBOfficerInfo->clear_arenaofficers();
	}
	else if(E_Officer_Type_Guild == nType)
	{
		for(int i=0;i<pDBOfficerInfo->guildofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_guildofficers(i);
			int nPosition = pDBOfficerTmp->officerposition();
			if(nPosition>0&&pDBOfficerTmp->uid()>0)
			{
				DB_C_User* pDBCUser = GetDBCityUser(pDBOfficerTmp->uid());
				if(pDBCUser)
				{
					pDBCUser->set_officerposition(pDBCUser->officerposition()&(~(1<<nPosition)));
					SetCountryUserUpdateFlag(pDBOfficerTmp->uid(),true);
					markUserLstDirty(pDBCUser->userid());
				}
			}
		}
		pDBOfficerInfo->clear_guildofficers();
	}
	SYS_LOG(0,LT_OFFICER_POSITION_CLEAR,m_nRegion,0,nType<<pCountry->GetCountryID());
}

int64 CCountry::GetOfficerPositionUser(int nPosition)
{
	if(nPosition <= Officer_NULL ||nPosition>=Officer_MAX)
	{
		return 0;
	}
	E_Officer_Type nType = GetOfficerType(nPosition);
	if(nType<=E_Officer_Type_NULL||nType>=Officer_Type_MAX)
	{
		return 0;
	}
	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(E_Officer_Type_Arena == nType)
	{
		for(int i=0;i<pDBOfficerInfo->arenaofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_arenaofficers(i);
			if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==nPosition)
			{
				return pDBOfficerTmp->uid();
			}
		}
	}
	else if(E_Officer_Type_Guild == nType)
	{
		for(int i=0;i<pDBOfficerInfo->guildofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_guildofficers(i);
			if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==nPosition)
			{
				return pDBOfficerTmp->uid();
			}
		}
	}
	return 0;
}

//void CountryRgnDatHandler::FillGuildData(CGuild* pGuild,GuildData* pGData)
void CountryRgnDatHandler::FillGuildData(CGuild* pGuild,GuildData* pGData,bool bNormalRank/* =true */)
{
	if(pGData==NULL||pGuild==NULL)
		return;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	pGData->set_guildid(pGuild->GetGuildID());
	pGData->set_countryid(pGuild->GetCountry());
	pGData->set_guildlv(pGuild->GetGuildLV());
	pGData->set_guildslogan(pDBGuild->guildslogan());
	pGData->set_guildname(pGuild->GetGuildName());
	pGData->set_guildbulletin(pDBGuild->guildbulletin());
	pGData->set_bldvalue(pDBGuild->bldvalue());
	pGData->set_totalbldvalue(pDBGuild->totalbldvalue());
	pGData->set_guilduesrcnt(pGuild->GetUserCnt());
	pGData->set_leaderid(toString(pDBGuild->leaderid()));
	pGData->set_leadername(pDBGuild->leadername());
	pGData->set_leaderlasttm(pDBGuild->leaderlasttm());
	pGData->set_delatetm(pDBGuild->delatetm());
	pGData->set_guildbtlscore(pDBGuild->mutable_btlinfo()->totalscore());
	pGData->set_guildrank(pGuild->m_nCountryGuildRank);
	pGData->set_totalrank(pGuild->m_nAllGuildRank);
	pGData->set_guildpkrank(pGuild->m_nCountryGuildPKRank);
	pGData->set_totalpkrank(pGuild->m_nAllGuildPKRank);
	pGData->set_chgleadertm(pGuild->GetChgLeaderTM());

	for(int i=0;i<pDBGuild->build_size();i++)
	{
		DB_C_GuildBld* pDBBld = pDBGuild->mutable_build(i);
		GuildBld* pBld = pGData->add_build();
		pBld->set_id(pDBBld->id());
		pBld->set_exp(pDBBld->exp());
		pBld->set_level(pDBBld->level());
	}

}

void CountryRgnDatHandler::FillRandGuildLst(RseGuildOpt* pRseOpt,GCG_GuildOpt* pSrvOpt,int nCountry)
{
	if(pRseOpt==NULL||pSrvOpt==NULL)
		return;
	if(nCountry<C_Start||nCountry>=C_UserCoutryEnd)
		nCountry=C_AllCountry; 
	int nTotal = m_lstGuild[nCountry].size();
	map<int,int> mapGuild;
	for(int i=0;i<pSrvOpt->appguildlst_size();i++)
	{//先放入申请的工会
		CGuild* pGuild = GetGuild(pSrvOpt->appguildlst(i));
		if(pGuild&&pGuild->GetCountry()==nCountry)
		{
			GuildData* pGData = pRseOpt->add_guildlst();
			FillGuildData(pGuild,pGData);
			mapGuild[pGuild->GetGuildID()] = 1;
		}
	}
	vector<CGuild*> lstGuild;
	//前十的工会随机抽三个
	for(int i=0;i<m_lstGuild[nCountry].size()&&i<10;i++)
	{
		CGuild* pGuild = m_lstGuild[nCountry][i];
		if(pGuild==NULL||pGuild->GetGuildID()<=0)
			continue;
		if(mapGuild.find(pGuild->GetGuildID())!=mapGuild.end())
			continue;
		lstGuild.push_back(pGuild);
	}

	if(lstGuild.size()>0)
	{
		int nMaxKey = lstGuild.size();
		for(int i=0;i<nMaxKey&&i<3;i++)
		{
			int nIdx = rand()%nMaxKey;
			CGuild* pGuild1 = lstGuild[i];
			lstGuild[i]		= lstGuild[nIdx];
			lstGuild[nIdx]	= pGuild1;
		}
		for(int i=0;i<nMaxKey&&i<3;i++)
		{
			CGuild* pGuild = lstGuild[i];
			GuildData* pGData = pRseOpt->add_guildlst();
			FillGuildData(pGuild,pGData);
			mapGuild[pGuild->GetGuildID()] = 1;
		}
	}
	int nLeftCnt = GUILD_LIST_PAGE_CNT - mapGuild.size(); //剩余显示的个数


	if(m_lstGuild[nCountry].size() <= 100)
	{//全随机
		lstGuild.clear();
		for(int i=0;i<m_lstGuild[nCountry].size();i++)
		{
			CGuild* pGuild = m_lstGuild[nCountry][i];
			if(pGuild==NULL||pGuild->GetGuildID()<=0)
				continue;
			if(mapGuild.find(pGuild->GetGuildID())!=mapGuild.end())
				continue;
			lstGuild.push_back(pGuild);
		}
		if(lstGuild.size()>0)
		{
			int nMaxKey = lstGuild.size();
			for(int i=0;i<nMaxKey&&i<=nLeftCnt;i++)
			{
				int nIdx = rand()%nMaxKey;
				CGuild* pGuild1 = lstGuild[i];
				lstGuild[i]		= lstGuild[nIdx];
				lstGuild[nIdx]	= pGuild1;
			}
			for(int i=0;i<nMaxKey&&i<=nLeftCnt;i++)
			{
				CGuild* pGuild = lstGuild[i];
				GuildData* pGData = pRseOpt->add_guildlst();
				FillGuildData(pGuild,pGData);
				mapGuild[pGuild->GetGuildID()] = 1;
			}
		}
	}
	else
	{//随机抽
		int nMaxKey = m_lstGuild[nCountry].size();
		int nCnt = 100;
		while(nLeftCnt>=0&&nCnt>0)
		{
			nCnt--;
			int nIdx = rand()%nMaxKey;
			CGuild* pGuild = m_lstGuild[nCountry][nIdx];
			if(pGuild==NULL||pGuild->GetGuildID()<=0)
				continue;
			if(mapGuild.find(pGuild->GetGuildID())!=mapGuild.end())
				continue;
			GuildData* pGData = pRseOpt->add_guildlst();
			FillGuildData(pGuild,pGData);
			mapGuild[pGuild->GetGuildID()] = 1;
			nLeftCnt--;
		}
		
	}
}

void CountryRgnDatHandler::FillPKGuildLst(RseGuildOpt* pRseOpt,int nCountry,int nPage)
{
	//nCountry=0,为所有国家列表
	if(pRseOpt==NULL)
		return;
	if(nCountry<C_Start||nCountry>=C_UserCoutryEnd)
		nCountry=C_AllCountry; 
	int nTotal = m_lstPKGuild[nCountry].size();
	if(nPage*GUILDBTL_LIST_PAGE_CNT>=nTotal)
	{
		nPage = 0;
	}
	pRseOpt->set_page(nPage);
	pRseOpt->set_totalguildcnt(m_lstPKGuild[nCountry].size());
	int nStart	= nPage*GUILDBTL_LIST_PAGE_CNT;
	int nEnd	= nStart+GUILDBTL_LIST_PAGE_CNT;

	bool bAllCountryRank = (C_AllCountry==nCountry);
	for(int i=nStart;i<nEnd&&i<m_lstPKGuild[nCountry].size();i++)
	{
		CGuild* pGuild = m_lstPKGuild[nCountry][i];
		if(pGuild==NULL||pGuild->GetGuildID()<=0)
			continue;
		GuildData* pGData = pRseOpt->add_guildlst();
		FillGuildData(pGuild,pGData,false);
	}
}

void CountryRgnDatHandler::FillGuildLst(RseGuildOpt* pRseOpt,int nCountry,int nPage)
{
	//nCountry=0,为所有国家列表
	if(pRseOpt==NULL)
		return;
	if(nCountry<C_Start||nCountry>=C_UserCoutryEnd)
		nCountry=C_AllCountry; 
	int nTotal = m_lstGuild[nCountry].size();
	if(nPage*GUILD_LIST_PAGE_CNT>=nTotal)
	{
		nPage = 0;
	}
	pRseOpt->set_page(nPage);
	pRseOpt->set_totalguildcnt(m_lstGuild[nCountry].size());
	int nStart	= nPage*GUILD_LIST_PAGE_CNT;
	int nEnd	= nStart+GUILD_LIST_PAGE_CNT;

	bool bAllCountryRank = (C_AllCountry==nCountry);
	for(int i=nStart;i<nEnd&&i<m_lstGuild[nCountry].size();i++)
	{
		CGuild* pGuild = m_lstGuild[nCountry][i];
		if(pGuild==NULL||pGuild->GetGuildID()<=0)
			continue;
		GuildData* pGData = pRseOpt->add_guildlst();
		FillGuildData(pGuild,pGData);
	}
}

void CountryRgnDatHandler::ResetGuildLog(CGuild* pGuild)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	int nIdx = 0;
	while(nIdx<pDBGuild->coinlog_size())
	{
		DB_C_GuildLog* pDBLog = pDBGuild->mutable_coinlog(nIdx);

		//清除老的数据
		if(pDBLog->time() + GUILD_LOG_TIME < ltNow)
		{
			int nLastIdx = pDBGuild->coinlog_size()-1;
			DB_C_GuildLog* pLastLog = pDBGuild->mutable_coinlog(nLastIdx);
			pDBLog->Swap(pLastLog);
			pDBGuild->mutable_coinlog()->RemoveLast();
			continue;
		}
		else
		{
			nIdx++;
		}
	}
	pGuild->m_nCheckLogTM = ltNow;
}

DB_C_GuildLog* CountryRgnDatHandler::AddGuildLog(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName)
{
	if(pGuild==NULL)
		return NULL;
	time_t ltNow = time(NULL);
	DB_C_GuildLog* pDBLog = NULL;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	if(emType == G_Log_Contribute)
	{//捐献日志保留2天,并且最大1000条
		if(pDBGuild->coinlog_size()<1000)
		{
			pDBLog = pDBGuild->add_coinlog();
		}
		else
		{
			pDBLog = pDBGuild->mutable_coinlog(0);
			for(int i=1;i<pDBGuild->coinlog_size();i++)
			{
				DB_C_GuildLog* pDBLog1 = pDBGuild->mutable_coinlog(i);
				if(pDBLog->time() > pDBLog1->time())
				{
					pDBLog = pDBLog1;
				}
			}
		}
		//pDBLog = pDBGuild->add_coinlog();
	}
	else if(emType == G_Log_SendReward)
	{//战利品日志保留50条
		if(pDBGuild->rewardlog_size()<MAX_GUILD_ITEMLOG_CNT)
		{
			pDBLog = pDBGuild->add_rewardlog();
		}
		else
		{
			pDBLog = pDBGuild->mutable_rewardlog(0);
			for(int i=1;i<pDBGuild->rewardlog_size();i++)
			{
				DB_C_GuildLog* pDBLog1 = pDBGuild->mutable_rewardlog(i);
				if(pDBLog->time() > pDBLog1->time())
				{
					pDBLog = pDBLog1;
				}
			}
		}
	}
	else
	{//其他日志保留30条
		if(pDBGuild->log_size()<MAX_GUILD_LOG_CNT)
		{
			pDBLog = pDBGuild->add_log();
		}
		else
		{
			pDBLog = pDBGuild->mutable_log(0);
			for(int i=1;i<pDBGuild->log_size();i++)
			{
				DB_C_GuildLog* pDBLog1 = pDBGuild->mutable_log(i);
				if(pDBLog->time() > pDBLog1->time())
				{
					pDBLog = pDBLog1;
				}
			}
		}
	}
	pDBLog->Clear();
	pDBLog->set_logtype(emType);
	pDBLog->set_time(ltNow);
	pDBLog->set_userid(toString(nUserID));
	pDBLog->set_username(strName);
	return pDBLog;
}

void CountryRgnDatHandler::AddGuildLog_Pos(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName,int nPos)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,nUserID,strName);
	if(pDBLog==NULL)
		return;
	pDBLog->set_userpos(nPos);
}

void CountryRgnDatHandler::AddGuildLog_Bld(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName,int nBldID,int nBldLV)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,nUserID,strName);
	if(pDBLog==NULL)
		return;
	pDBLog->set_bldid(nBldID);
	pDBLog->set_bldlv(nBldLV);
}

void CountryRgnDatHandler::AddGuildLog_Pay(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName,int nPayType,int nGScore)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,nUserID,strName);
	if(pDBLog==NULL)
		return;
	pDBLog->set_paytype(nPayType);
	pDBLog->set_gscore(nGScore);
}

void CountryRgnDatHandler::AddGuildLog_BtlRank(CGuild* pGuild,GuildLogType emType,int nRank)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,0,"");
	if(pDBLog==NULL)
		return;
	pDBLog->set_pkrank(nRank);
}

void CountryRgnDatHandler::AddGuildLog_BtlRankReward(CGuild* pGuild,GuildLogType emType,int nItemID,int nItemCnt,int nBldValue,int nRank,int nCountryRank)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,0,"");
	if(pDBLog==NULL)
		return;
	pDBLog->set_itemid(nItemID);
	pDBLog->set_itemcnt(nItemCnt);
	pDBLog->set_bldvalue(nBldValue);
	pDBLog->set_pkrank(nRank);
	pDBLog->set_pkrank1(nCountryRank);
}

void CountryRgnDatHandler::AddGuildLog_Storage(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName,int nItemID,int nItemCnt)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,nUserID,strName);
	if(pDBLog==NULL)
		return;
	pDBLog->set_itemid(nItemID);
	pDBLog->set_itemcnt(nItemCnt);
}

void CountryRgnDatHandler::AddGuildLog_GvgReward(CGuild* pGuild,GuildLogType emType,int64 nUserID,string strName,int nPos)
{
	if(pGuild==NULL)
		return;
	time_t ltNow = time(NULL);
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildLog* pDBLog = AddGuildLog(pGuild,emType,nUserID,strName);
	if(pDBLog==NULL)
		return;
	pDBLog->set_userpos(nPos);
}

bool CountryRgnDatHandler::CheckGuildShopRecord(CGuild* pGuild)
{
	if(pGuild==NULL)
		return false;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildShop* pDBShop = pDBGuild->mutable_guildshop();
	time_t ltNow = time(NULL);
	time_t lasttime = pDBShop->buyrecordday();
	if(lasttime<=0)
	{
		pDBShop->clear_buyrecord();
		pDBShop->set_buyrecordday(ltNow);
		return true;
	}
	struct tm lastDay = *localtime(&lasttime);
	struct tm nowtime = *localtime(&ltNow);
	
	if (lastDay.tm_yday != nowtime.tm_yday)
	{//不是同一天
		pDBShop->clear_buyrecord();
		pDBShop->set_buyrecordday(ltNow);
		return true;
	}
	return false;
}

void CountryRgnDatHandler::ClearUserOfficerPosition(DB_C_User* pDBCUser,E_Officer_Type nType)
{
	if(NULL == pDBCUser)
	{
		return;
	}
	CCountry* pCountry = GetCountry(pDBCUser->countryid());
	if(NULL==pCountry)
	{
		return;
	}
	for(int i=Officer_Chairman;i<Officer_MAX;i++)
	{
		if(pCountry->GetOfficerType(i)==nType)
		{
			pCountry->DelUserOfficerPosition(pDBCUser,(E_Officer_Position)i);
		}
	}
	markCountryDirty(pCountry);
	SYS_LOG(0,LT_OFFICER_POSITION_DEL,m_nRegion,0,(int64)pDBCUser->userid()<<pDBCUser->countryid()<<nType);
}

void CountryRgnDatHandler::SetCountryOfficerPosition(DB_C_User* pDBCUser,CCountry* pCountry,int nPosition,E_Officer_Type nType)
{
	if(pDBCUser==NULL||pCountry==NULL||nPosition<=Officer_NULL||nPosition>=Officer_MAX)
	{
		return;
	}
	//老官员
	int nOldPosition,nNewPosition;
	if(nPosition!=Officer_PresidentGuard)
	{
		int64 nOldUserId = pCountry->GetOfficerPositionUser(nPosition);
		DB_C_User* pDBCOldUser = GetDBCityUser(nOldUserId);
		if(pDBCOldUser)
		{
			nOldPosition = pDBCOldUser->officerposition();
			if(pCountry->DelUserOfficerPosition(pDBCOldUser,(E_Officer_Position)nPosition))
			{
				nNewPosition = pDBCOldUser->officerposition();
				SetCountryUserUpdateFlag(pDBCOldUser->userid(),true);
				markUserLstDirty(pDBCOldUser->userid());
				SYS_LOG(0,LT_OFFICER_POSITION_UPDATE,m_nRegion,0,nType<<(int64)pDBCOldUser->userid()<<pDBCOldUser->countryid()<<nPosition<<nOldPosition<<nNewPosition);
			}
		}
	}
	//新官员
	nOldPosition = pDBCUser->officerposition();
	if(pCountry->SetUserOfficerPosition(pDBCUser,(E_Officer_Position)nPosition))
	{
		nNewPosition = pDBCUser->officerposition();
		SetCountryUserUpdateFlag(pDBCUser->userid(),true);
		markUserLstDirty(pDBCUser->userid());
		SYS_LOG(0,LT_OFFICER_POSITION_UPDATE,m_nRegion,0,nType<<(int64)pDBCUser->userid()<<pDBCUser->countryid()<<nPosition<<nOldPosition<<nNewPosition);
	}
}
void CountryRgnDatHandler::UpdateOfficerPosition(DB_C_User* pDBCUser,int nIndex,int nCountryID,E_Officer_Type nType,CGuild* pGuild/*=NULL*/)
{//nCountryID = C_AllCountry 说明是世界工会战
	if(NULL == pDBCUser||nIndex<0||nIndex>=10)
	{
		return;
	}
	GameConstantSetLogic_Officer* pLogic = NULL;
	ConstantSetUnit* pUnit = GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_DEF_OFFICER);
	if(pUnit)
	{
		pLogic = dynamic_cast<GameConstantSetLogic_Officer*>(pUnit->m_pLogic);
	}

	if(pLogic == NULL)
	{
		return ;
	}
	if(false == pLogic->IsOpen())
	{
		return ;
	}
	CCountry* pCountry = GetCountry(pDBCUser->countryid());
	if(pCountry)
	{
		//设置职位
		int nPosition = CreateOfficerPosition(nIndex,nCountryID,nType);
		if(Officer_NULL!=nPosition)
		{
			SetCountryOfficerPosition(pDBCUser,pCountry,nPosition,nType);
		}
		//工会战第一名的所有成员为总统卫队
		if(nType == E_Officer_Type_Guild&&nCountryID!=C_AllCountry)
		{
			if(pGuild&&nIndex==0)
			{
				map<int64,DB_C_User*>::iterator iter;
				for(iter = pGuild->m_mapGuildUser.begin();iter!=pGuild->m_mapGuildUser.end();iter++)
				{
					DB_C_User* pDBCUser = iter->second;
					if(pDBCUser&&pDBCUser->position()!=GP_Leader)
					{
						SetCountryOfficerPosition(pDBCUser,pCountry,Officer_PresidentGuard,nType);
					}
				}
			}
		}
		markCountryDirty(pCountry);
	}
	if(pGuild)
	{
		SYS_LOG(0,LT_OFFICER_CREATE,m_nRegion,0,nType<<nCountryID<<(int64)pDBCUser->userid()<<pDBCUser->countryid()<<nIndex<<pGuild->GetGuildID()<<pGuild->GetGuildName());
	}
	else
	{
		SYS_LOG(0,LT_OFFICER_CREATE,m_nRegion,0,nType<<nCountryID<<(int64)pDBCUser->userid()<<pDBCUser->countryid()<<nIndex<<0<<0);
	}
}
bool CountryRgnDatHandler::ClearUserCityOfficerRight(DB_C_User* pDBCUser,CCountry* pCountry)
{
	if(pDBCUser==NULL||pCountry==NULL)
	{
		return false;
	}
	//城市权限buff信息
	bool bUpdate = false;
	vector<CCity*>::iterator iter;
	for(iter = pCountry->m_lstCity.begin();iter!=pCountry->m_lstCity.end();iter++)
	{
		CCity* pCity = *iter;
		if(NULL == pCity)
			continue;
		if(pCity->UpdateOfficerRightBuff())
		{
			markCityDirty(pCity);
		}
		if(pCity->ClearOfficerRightByUid(pDBCUser->userid()))
		{
			bUpdate = true;
		}
	}
	return bUpdate;
}
void CountryRgnDatHandler::FillCountrOfficerInfo(CCountry* pCountry,CoutryOfficer* pCountryOfficer,E_Officer_Fill_Type fillType /*=E_Officer_Fill_ALL*/)
{
	if(NULL == pCountry||NULL == pCountryOfficer)
	{
		return;
	}
	DB_C_Country* pDBCCountry = pCountry->GetDBCountry();
	if(NULL == pDBCCountry)
	{
		return;
	}
	pCountryOfficer->set_countryid(pDBCCountry->countryid());
	//官员信息
	DB_C_OfficerInfo* pDBCOfficerInfo = pDBCCountry->mutable_officerinfo();

	if(fillType==E_Officer_Fill_ALL||fillType==E_Officer_Fill_Officer)
	{
		for(int i=0;i<pDBCOfficerInfo->arenaofficers_size();i++)
		{
			DB_C_Officer* pDBCOfficer = pDBCOfficerInfo->mutable_arenaofficers(i);
			if(pDBCOfficer->officerposition()>0&&pDBCOfficer->uid()>0)
			{
				Officer *pOfficer = pCountryOfficer->add_officers();
				pOfficer->set_uid(toString(pDBCOfficer->uid()));
				pOfficer->set_position(pDBCOfficer->officerposition());
				DB_C_User* pDBCUser = GetDBCityUser(pDBCOfficer->uid());
				if(pDBCUser)
				{
					pOfficer->set_city(pDBCUser->cityid());
				}
			}
		}
		for(int i=0;i<pDBCOfficerInfo->guildofficers_size();i++)
		{
			DB_C_Officer* pDBCOfficer = pDBCOfficerInfo->mutable_guildofficers(i);
			if(pDBCOfficer->officerposition()>0&&pDBCOfficer->uid()>0)
			{
				Officer *pOfficer = pCountryOfficer->add_officers();
				pOfficer->set_uid(toString(pDBCOfficer->uid()));
				pOfficer->set_position(pDBCOfficer->officerposition());
				DB_C_User* pDBCUser = GetDBCityUser(pDBCOfficer->uid());
				if(pDBCUser)
				{
					pOfficer->set_city(pDBCUser->cityid());
				}
			}
		}
	}
	
	if(fillType==E_Officer_Fill_ALL||fillType==E_Officer_Fill_Right)
	{
		//城市权限buff信息
		vector<CCity*>::iterator iter;
		for(iter = pCountry->m_lstCity.begin();iter!=pCountry->m_lstCity.end();iter++)
		{
			CCity* pCity = *iter;
			if(NULL == pCity)
				continue;
			if(pCity->UpdateOfficerRightBuff())
			{
				markCityDirty(pCity);
			}
			if(pCity->HasOfficerRightBuff())
			{
				CityRightInfo* pCityRightInfo = pCountryOfficer->add_cityrgihts();
				FillOfficerRightUsed(pCity->GetDBCity(),pCityRightInfo);
			}
			else
			{
				pCity->ClearOfficerRightBuff();
			}
		}
	}
}
void CountryRgnDatHandler::FillOfficerRightUsed(DB_C_City* pDBCCity,CityRightInfo* pInfo)
{
	if(NULL == pDBCCity||NULL == pInfo)
	{
		return;
	}
	pInfo->set_cityid(pDBCCity->cityid());
	for(int nId=0;nId<pDBCCity->officerrightuselst_size();nId++)
	{
		DB_C_OfficerRightUse* pDBCRightUse = pDBCCity->mutable_officerrightuselst(nId);
		if(pDBCRightUse->endtime()>=time(NULL))
		{
			RightInfo* pRightInfo = pInfo->add_rightinfo();
			pRightInfo->set_rightid(pDBCRightUse->rightid());
			pRightInfo->set_endtime(pDBCRightUse->endtime());
			pRightInfo->set_uid(toString(pDBCRightUse->uid()));
			pRightInfo->set_name(pDBCRightUse->name());
			pRightInfo->set_position(pDBCRightUse->position());
		}
	}
}
bool CountryRgnDatHandler::AddOfficerRightUsed(CCity* pCity,RightInfo* pRightInfo)
{
	if(NULL == pCity||NULL==pRightInfo)
	{
		return false;
	}
	DB_C_City* pDBCity = pCity->GetDBCity();
	if(NULL == pDBCity)
	{
		return false;
	}
	bool bFind =false;
	int64 uid = 0;
	safe_atoll(pRightInfo->uid(),uid);
	for(int i=0;i<pDBCity->officerrightuselst_size();i++)
	{
		DB_C_OfficerRightUse* pDBUse = pDBCity->mutable_officerrightuselst(i);
		if(pDBUse&&pDBUse->rightid()==pRightInfo->rightid())
		{
			bFind = true;
			pDBUse->set_uid(uid);
			pDBUse->set_endtime(pRightInfo->endtime());
			pDBUse->set_name(pRightInfo->name());
			pDBUse->set_position(pRightInfo->position());
			break;
		}
	}
	if(!bFind)
	{
		DB_C_OfficerRightUse* pDBUse = pDBCity->add_officerrightuselst();
		pDBUse->set_rightid(pRightInfo->rightid());
		pDBUse->set_uid(uid);
		pDBUse->set_endtime(pRightInfo->endtime());
		pDBUse->set_name(pRightInfo->name());
		pDBUse->set_position(pRightInfo->position());
	}
	SYS_LOG(0,LT_OFFICER_USE_RIGHT,m_nRegion,0,(int64)uid<<pRightInfo->position()<<pRightInfo->rightid()<<pRightInfo->endtime()<<pCity->GetCityID());
	return true;
}


bool CCountry::SetUserOfficerPosition(DB_C_User* pDBCUser,E_Officer_Position ePosition)
{
	if(NULL == pDBCUser||ePosition<=Officer_NULL||ePosition>=Officer_MAX)
	{
		return false;
	}
	E_Officer_Type nType = GetOfficerType(ePosition);
	if(nType<=E_Officer_Type_NULL||nType>=Officer_Type_MAX)
	{
		return false;
	}

	pDBCUser->set_officerposition(pDBCUser->officerposition()|(1<<ePosition));

	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(E_Officer_Type_Arena == nType)
	{
		bool bFind = false;
		for(int i=0;i<pDBOfficerInfo->arenaofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_arenaofficers(i);
			if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==(int)ePosition)
			{
				pDBOfficerTmp->set_officerposition((int)ePosition);
				pDBOfficerTmp->set_uid(pDBCUser->userid());
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			DB_C_Officer *pDBOfficer = pDBOfficerInfo->add_arenaofficers();
			pDBOfficer->set_officerposition((int)ePosition);
			pDBOfficer->set_uid(pDBCUser->userid());
		}
	}
	else if(E_Officer_Type_Guild == nType)
	{
		if(ePosition == Officer_PresidentGuard)
		{
			bool bFind = false;
			for(int i=0;i<pDBOfficerInfo->guildofficers_size();i++)
			{
				DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_guildofficers(i);
				if(pDBOfficerTmp->officerposition()==(int)ePosition&&pDBOfficerTmp->uid()==pDBCUser->userid())
				{
					pDBOfficerTmp->set_officerposition((int)ePosition);
					pDBOfficerTmp->set_uid(pDBCUser->userid());
					bFind = true;
					break;
				}
			}
			if(!bFind)
			{
				DB_C_Officer *pDBOfficer = pDBOfficerInfo->add_guildofficers();
				pDBOfficer->set_officerposition((int)ePosition);
				pDBOfficer->set_uid(pDBCUser->userid());
			}
		}
		else
		{
			bool bFind = false;
			for(int i=0;i<pDBOfficerInfo->guildofficers_size();i++)
			{
				DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_guildofficers(i);
				if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==(int)ePosition)
				{
					pDBOfficerTmp->set_officerposition((int)ePosition);
					pDBOfficerTmp->set_uid(pDBCUser->userid());
					bFind = true;
					break;
				}
			}
			if(!bFind)
			{
				DB_C_Officer *pDBOfficer = pDBOfficerInfo->add_guildofficers();
				pDBOfficer->set_officerposition((int)ePosition);
				pDBOfficer->set_uid(pDBCUser->userid());
			}
		}
	}
	return true;
}

bool CCountry::DelUserOfficerPosition(DB_C_User* pDBCUser,E_Officer_Position ePosition)
{
	if(NULL == pDBCUser||ePosition<=Officer_NULL||ePosition>=Officer_MAX)
	{
		return false;
	}
	E_Officer_Type nType = GetOfficerType(ePosition);
	if(nType<=E_Officer_Type_NULL||nType>=Officer_Type_MAX)
	{
		return false;
	}

	pDBCUser->set_officerposition(pDBCUser->officerposition()&(~(1<<ePosition)));

	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(E_Officer_Type_Arena == nType)
	{
		for(int i=0;i<pDBOfficerInfo->arenaofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_arenaofficers(i);
			if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==(int)ePosition&&pDBOfficerTmp->uid()==pDBCUser->userid())
			{
				pDBOfficerTmp->Clear();
			}
		}
	}
	else if(E_Officer_Type_Guild == nType)
	{
		for(int i=0;i<pDBOfficerInfo->guildofficers_size();i++)
		{
			DB_C_Officer *pDBOfficerTmp = pDBOfficerInfo->mutable_guildofficers(i);
			if(pDBOfficerTmp->officerposition()>0&&pDBOfficerTmp->officerposition()==(int)ePosition&&pDBOfficerTmp->uid()==pDBCUser->userid())
			{
				pDBOfficerTmp->Clear();
			}
		}
	}
	return true;
}

bool CCountry::HasUserOfficerPosition(DB_C_User* pDBCUser,E_Officer_Position ePosition)
{
	if(NULL == pDBCUser||ePosition<=Officer_NULL||ePosition>=Officer_MAX)
	{
		return false;
	}
	return pDBCUser->officerposition()&(1<<ePosition)?true:false;
}

bool CCountry::ChangeOfficerPosition(DB_C_User* pDBCUser,DB_C_User* pDBCTarUser,E_Officer_Type nType)
{
	if(NULL==pDBCUser || NULL == pDBCTarUser || nType == E_Officer_Type_NULL)
	{
		return false;
	}
	if(pDBCUser->countryid()!=pDBCTarUser->countryid())
	{
		return false;
	}
	int nOldPosition1 = pDBCUser->officerposition();
	int nOldPosition2 = pDBCTarUser->officerposition();
	bool bUpdate =false;
	vector<E_Officer_Position> vecPosition1,vecPosition2;
	vecPosition1.clear();
	vecPosition2.clear();
	for(int i=Officer_Chairman;i<Officer_MAX;i++)
	{
		if(nType==GetOfficerType((E_Officer_Position)i))
		{
			if(HasUserOfficerPosition(pDBCUser,(E_Officer_Position)i))
			{
				vecPosition1.push_back((E_Officer_Position)i);
			}
			if(HasUserOfficerPosition(pDBCTarUser,(E_Officer_Position)i))
			{
				vecPosition2.push_back((E_Officer_Position)i);
			}
		}
	}
	for(int i=0;i<(int)vecPosition1.size();i++)
	{
		DelUserOfficerPosition(pDBCUser,vecPosition1[i]);
		bUpdate = true;
	}
	for(int i=0;i<(int)vecPosition2.size();i++)
	{
		DelUserOfficerPosition(pDBCTarUser,vecPosition2[i]);
		bUpdate = true;
	}
	for(int i=0;i<(int)vecPosition1.size();i++)
	{
		SetUserOfficerPosition(pDBCTarUser,vecPosition1[i]);
		bUpdate = true;
	}
	for(int i=0;i<(int)vecPosition2.size();i++)
	{
		SetUserOfficerPosition(pDBCUser,vecPosition2[i]);
		bUpdate = true;
	}
	SYS_LOG(0,LT_OFFICER_POSITION_CHANGE,m_nRegion,0,pDBCUser->countryid()<<nType<<(int64)pDBCUser->userid()<<nOldPosition1<<pDBCUser->officerposition()<<(int64)pDBCTarUser->userid()<<nOldPosition2<<pDBCTarUser->officerposition());
	return bUpdate;
}

E_Officer_Position CCountry::GetUserHighestPosition(DB_C_User *pDBCUser)
{
	if(NULL == pDBCUser)
	{
		return Officer_NULL;
	}
	for(int i=Officer_Chairman;i<Officer_MAX;i++)
	{
		if(HasUserOfficerPosition(pDBCUser,(E_Officer_Position)i))
		{
			return (E_Officer_Position)i;
		}
	}
	return Officer_NULL;
}

void CCountry::SetOfficerRightUsed(E_Officer_Position ePosition)
{
	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(pDBOfficerInfo)
	{
		pDBOfficerInfo->set_officerrightuseflag(pDBOfficerInfo->officerrightuseflag()|(1<<ePosition));
		pDBOfficerInfo->set_updatetimelast((int)time(NULL));
	}
}

bool CCountry::IsOfficerRightUsed(E_Officer_Position ePosition)
{
	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(pDBOfficerInfo)
	{
		return pDBOfficerInfo->officerrightuseflag()&(1<<ePosition)?true:false;
	}
	return false;
}

bool CCountry::UpdateUserOfficerPerDay()
{
	DB_C_OfficerInfo *pDBOfficerInfo = m_dbCountry.mutable_officerinfo();
	if(NULL == pDBOfficerInfo)
	{
		return false;
	}
	time_t nowTime = time(NULL);
	time_t lastTime = pDBOfficerInfo->updatetimelast();

	struct tm nowtime = *localtime(&nowTime);
	struct tm lastime = *localtime(&lastTime);

	if (lastime.tm_yday != nowtime.tm_yday || lastime.tm_year != nowtime.tm_year)
	{	
		pDBOfficerInfo->set_officerrightuseflag(0);
		pDBOfficerInfo->set_updatetimelast((int)time(NULL));
	}
}
void CountryRgnDatHandler::SetCountryUserUpdateFlag(int64 uid,bool bFlag /* = true */)
{
	if(uid<=0)
		return;
	m_mapUserUpdateFlag[uid] = bFlag;
}

bool CountryRgnDatHandler::CheckCountryUserUpdateFlag(CountryEventHandler* eh_)
{
	if(NULL == eh_)
	{
		return false;
	}
	if(m_mapUserUpdateFlag.size()<=0)
	{
		return false;
	}
	bool bUpdate = false;
	map<int,bool> rmapCountryUpdateFlag;
	rmapCountryUpdateFlag.clear();

	map<int64,bool>::iterator iter;
	for(iter=m_mapUserUpdateFlag.begin();iter!=m_mapUserUpdateFlag.end();iter++)
	{
		if(true == iter->second)
		{
			int64 uid = iter->first;
			DB_C_User* pDBCUser = GetDBCityUser(uid);
			if(pDBCUser)
			{
				rmapCountryUpdateFlag[pDBCUser->countryid()] = true;
				eh_->SendCountryUserLiteInfo(this,pDBCUser);
				bUpdate = true;
			}
		}
	}
	map<int,bool>::iterator iterCountry;
	for(iterCountry=rmapCountryUpdateFlag.begin();iterCountry!=rmapCountryUpdateFlag.end();iterCountry++)
	{
		if(true == iterCountry->second)
		{
			int countryId = iterCountry->first;
			eh_->SendOfficerRightInfo2CountryUser(countryId,GetRegion(),-1,E_Officer_Fill_Officer);
		}
	}
	m_mapUserUpdateFlag.clear();
	return bUpdate;
}

void CountryRgnDatHandler::MatchGuildBtlGroup(int nTurns,bool bSortByScore)
{
	CCountry* pECountry = GetEarthCountry();
	DB_C_PubGuildBtlInfo* pDBPGB = pECountry->GetDBCountry()->mutable_pubguildbtlinfo();
	if(pDBPGB->pklst_size()>0)
		return;
	pDBPGB->set_pklstidx(0);
	pDBPGB->clear_pklst();

	if(bSortByScore)
	{
		SortPKGuild();
	}

	time_t ltNow = time(NULL);
	tm tmNow = *localtime(&ltNow);
	int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;

	vector<CGuild*> lstGuild;
	int nMinUserCnt = GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nMinJionCnt;
	for(int i=0;i<m_lstPKGuild[C_AllCountry].size();i++)
	{
		CGuild* pGuild = m_lstPKGuild[C_AllCountry][i];
		DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
		CheckNewDayGuildPK(nYearDay,pGuild);

		if(pDBGuild->mutable_btlinfo()->signupcnt()<nMinUserCnt)
			continue;
		lstGuild.push_back(pGuild);
	}
	if(!bSortByScore)
	{
		//随机打乱
		for(int i=0;i<lstGuild.size();i++)
		{
			int nIdx = rand()%lstGuild.size();
			CGuild* pTmpGuild = lstGuild[i];
			lstGuild[i] = lstGuild[nIdx];
			lstGuild[nIdx] = pTmpGuild;
		}
	}

	//放入对战队列
	CGuild* pPVGuild1=NULL;
	CGuild* pPVGuild2=NULL;
	DB_C_PubGuildPKGroup* pDBGroup = NULL;
	for(int i=0;i<lstGuild.size();i++)
	{
		CGuild* pTmpGuild = lstGuild[i];
		if(i%2==0)
		{
			pPVGuild1 = pTmpGuild;
			pPVGuild2 = NULL;
			pPVGuild1->SetPKGuildID(nTurns,0);
			pDBGroup = pDBPGB->add_pklst();
			pDBGroup->set_guildid1(pPVGuild1->GetGuildID());
			pDBGroup->set_guildid2(0);
		}
		else
		{
			pPVGuild2 = pTmpGuild;
			pPVGuild2->SetPKGuildID(nTurns,0);

			if(pPVGuild1)
			{
				pPVGuild1->SetPKGuildID(nTurns,pPVGuild2->GetGuildID());
				pPVGuild2->SetPKGuildID(nTurns,pPVGuild1->GetGuildID());
				pDBGroup->set_guildid2(pPVGuild2->GetGuildID());
			}
			pPVGuild1=NULL;
		}

	}
	
	markCountryDirty(pECountry);

}


DB_C_GuildBtl* CountryRgnDatHandler::GetDBGuildBtl(CGuild* pGuild)
{
	if(pGuild==NULL)
		return NULL;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	return pDBGuild->mutable_btlinfo();
}

DB_C_GuildBtlReward_Turns* CountryRgnDatHandler::GetDBGuildRewardTurns(CGuild* pGuild,int nTurns)
{
	if(pGuild==NULL||nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return NULL;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildBtlReward* pDBReward = pDBGuild->mutable_btlinfo()->mutable_reward();
	for(int i=pDBReward->turns_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBReward->add_turns();
	}
	return pDBReward->mutable_turns(nTurns);
}

DB_C_GuildBtlResult* CountryRgnDatHandler::GetDBGuildBtlResult(CGuild* pGuild,int nTurns)
{
	if(pGuild==NULL||nTurns<0||nTurns>=GUILD_BTL_DAY_CNT)
		return NULL;
	DB_C_Guild* pDBGuild = pGuild->GetDBGuild();
	DB_C_GuildBtl* pDBGBtl = pDBGuild->mutable_btlinfo();
	for(int i=pDBGBtl->result_size();i<GUILD_BTL_DAY_CNT;i++)
	{
		pDBGBtl->add_result();
	}
	return pDBGBtl->mutable_result(nTurns);
}

bool CountryRgnDatHandler::CheckNewDayGuildPK(int nYearDay,CGuild* pGuild)
{
	if(pGuild==NULL)
		return false;
	DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();
	int nBtlDay = pDBGBInfo->btlday();
	if(nYearDay != nBtlDay)
	{//新一天
		SYS_LOG(pGuild->GetGuildID(),LT_GBtl_NewDay,GetRegion(),0,nBtlDay<<nYearDay);

		pDBGBInfo->set_btlday(nYearDay);

		pDBGBInfo->clear_addprop();
		pDBGBInfo->clear_userway0();
		pDBGBInfo->clear_userway1();
		pDBGBInfo->clear_userway2();
		const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();
		int nCnt = pGBtlTbl->GetWayUserCnt(pGuild->GetGuildLV());
		for(int i=0;i<nCnt;i++)
		{
			pDBGBInfo->add_userway0(0);
			pDBGBInfo->add_userway1(0);
			pDBGBInfo->add_userway2(0);
		}
		if(nCnt>2)
		{
			pDBGBInfo->set_userway0(nCnt-2,GBTool_CostHP);
			pDBGBInfo->set_userway0(nCnt-1,GBTool_CostAtk);
			pDBGBInfo->set_userway1(nCnt-2,GBTool_CostHP);
			pDBGBInfo->set_userway1(nCnt-1,GBTool_CostAtk);
			pDBGBInfo->set_userway2(nCnt-2,GBTool_CostHP);
			pDBGBInfo->set_userway2(nCnt-1,GBTool_CostAtk);
		}
		pDBGBInfo->clear_signupcnt();
		pDBGBInfo->clear_result();
		return true;
	}
	return false;
}


void CountryRgnDatHandler::AddPKGuild(CGuild* pGuild)
{
	if(pGuild==NULL)
		return;
	m_lstPKGuild[C_AllCountry].push_back(pGuild);
	int nCountry = pGuild->GetCountry();
	if(nCountry>C_Start&&nCountry<C_UserCoutryEnd)
	{
		m_lstPKGuild[nCountry].push_back(pGuild);
	}
	SortPKGuild();
}

void CountryRgnDatHandler::CacuGuildBtlScore(CGuild* pGuild,int nTurns)
{
	if(pGuild==NULL)
		return;
	if(nTurns <0 ||nTurns >= GUILD_BTL_DAY_CNT)
		return;
	DB_C_GuildBtlResult* pDBRet = GetDBGuildBtlResult(pGuild,nTurns);
	if(pDBRet==NULL||pDBRet->result() != GBOResult_Unknow)
		return;
	if(pDBRet->winway0() == GBOResult_Unknow || 
		pDBRet->winway1() == GBOResult_Unknow|| 
		pDBRet->winway2() == GBOResult_Unknow)
		return;
	if(pDBRet->winway0() == GBOResult_WinNoEnemy && 
		pDBRet->winway0() == GBOResult_WinNoEnemy&& 
		pDBRet->winway0() == GBOResult_WinNoEnemy)
	{
		pDBRet->set_result(GBR_WinNoEnemy);
	}
	else
	{
		int nWinCnt = 0;
		if(pDBRet->winway0() == GBOResult_Win)
		{
			nWinCnt++;
		}
		if(pDBRet->winway1() == GBOResult_Win)
		{
			nWinCnt++;
		}
		if(pDBRet->winway2() == GBOResult_Win)
		{
			nWinCnt++;
		}
		pDBRet->set_result(GBR_Win0Way+nWinCnt);
	}
	int nScore = GuildInfoCfg::Instance().GetGuildBtlBaseTbl()->m_nGuildBtlScore[pDBRet->result()];
	pDBRet->set_score(nScore);
	pDBRet->set_addflag(false);

	DB_C_GuildBtl* pDBGBtl = pGuild->GetDBGuild()->mutable_btlinfo();
	//pDBGBtl->set_totalscore(pDBGBtl->totalscore()+nScore);
	pDBGBtl->set_btlcnt(pDBGBtl->btlcnt()+1);

	SYS_LOG(0,LT_GBtl_GuildPKCacu,GetRegion(),0,nTurns<<pGuild->GetGuildID()<<pDBRet->pkguild()<<pDBRet->result()<<pDBRet->score()<<pDBRet->winway0()<<pDBRet->winway1()<<pDBRet->winway2());

	//奖励结构
	time_t ltNow = time(NULL);
	tm tmNow = *localtime(&ltNow);
	int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
	DB_C_GuildBtlReward* pDBReward = pDBGBtl->mutable_reward();
	if(pDBReward->btlday()!=nYearDay)
	{
		pDBReward->Clear();
		pDBReward->set_btlday(nYearDay);
	}
	DB_C_GuildBtlReward_Turns* pDBRTurns = GetDBGuildRewardTurns(pGuild,nTurns);
	if(pDBRTurns)
	{
		pDBRTurns->set_guildid(pGuild->GetGuildID());
		pDBRTurns->set_guildlv(pGuild->GetGuildLV());
		pDBRTurns->set_guildname(pGuild->GetGuildName());

		int nPKID = pDBRet->pkguild();
		CGuild* pEGuild = GetGuild(nPKID);
		if(pEGuild)
		{
			pDBRTurns->set_guildenemyid(pEGuild->GetGuildID());
			pDBRTurns->set_guildenemylv(pEGuild->GetGuildLV());
			pDBRTurns->set_guildenemyname(pEGuild->GetGuildName());
		}

		pDBRTurns->set_winway0(pDBRet->winway0());
		pDBRTurns->set_winway1(pDBRet->winway1());
		pDBRTurns->set_winway2(pDBRet->winway2());
		pDBRTurns->set_result(pDBRet->result());
		pDBRTurns->set_score(pDBRet->score());
	}
}

bool CountryRgnDatHandler::FillGuildBtlRetTotal(int64 nUserID,CGuild* pGuild,GuildBtlRetTotal* pRseTotal)
{
    if(pGuild==NULL||pRseTotal==NULL)
        return false;
    const GuildBtlBaseTbl* pGBtlTbl = GuildInfoCfg::Instance().GetGuildBtlBaseTbl();

    DB_C_GuildBtl* pDBGBInfo = pGuild->GetDBGuild()->mutable_btlinfo();

    DB_C_GuildBtlReward* pDBReward = pDBGBInfo->mutable_reward();
    time_t ltNow = time(NULL);
    tm tmNow = *localtime(&ltNow);
    //int nYearDay = (tmNow.tm_year+1900)*10000 + (tmNow.tm_mon+1)*100 + tmNow.tm_mday;
    //if(nYearDay!=pDBReward->btlday())
    //{
    //	return false;
    //}

    bool bSucc = true;
    for(int i=0;i<pDBReward->turns_size();i++)
    {
        DB_C_GuildBtlReward_Turns* pDBTurns = pDBReward->mutable_turns(i);
        if(pDBTurns->result() == GBR_Unknow)
        {
            //bSucc = false;
            //continue;
            SYS_LOG(nUserID,LT_CNTR_ERROR,GetRegion(),0,"Guild Battle Not End!!!"<<pGuild->GetGuildID()<<i);
            GuildBtlRetLite* pRet = pRseTotal->add_ret();
            pRet->set_result(GBR_Unknow);
            pRet->set_winway0(pDBTurns->winway0());
            pRet->set_winway1(pDBTurns->winway1());
            pRet->set_winway2(pDBTurns->winway2());
            pRet->set_score(pDBTurns->score());
            GuildData* pGD1 = pRet->mutable_guild1();
            GuildData* pGD2 = pRet->mutable_guild2();
            pGD1->set_guildid(pDBTurns->guildid());
            pGD1->set_guildlv(pDBTurns->guildlv());
            pGD1->set_guildname(pDBTurns->guildname());

            pGD2->set_guildid(pDBTurns->guildenemyid());
            pGD2->set_guildlv(pDBTurns->guildenemylv());
            pGD2->set_guildname(pDBTurns->guildenemyname());

            pRet->set_rewardid(pGBtlTbl->m_nGuildWinReward);
            //if(pDBTurns->result()==GBR_Win2Way || pDBTurns->result()==GBR_Win3Way || pDBTurns->result()==GBR_WinNoEnemy)
            //{
            //	pRet->set_rewardid(pGBtlTbl->m_nGuildWinReward);
            //}
            //else
            //{
            //	pRet->set_rewardid(pGBtlTbl->m_nGuildLoseReward);
            //}
            continue;
        }
        GuildBtlRetLite* pRet = pRseTotal->add_ret();

        pRet->set_result(pDBTurns->result());
        pRet->set_winway0(pDBTurns->winway0());
        pRet->set_winway1(pDBTurns->winway1());
        pRet->set_winway2(pDBTurns->winway2());
        pRet->set_score(pDBTurns->score());
        GuildData* pGD1 = pRet->mutable_guild1();
        GuildData* pGD2 = pRet->mutable_guild2();
        pGD1->set_guildid(pDBTurns->guildid());
        pGD1->set_guildlv(pDBTurns->guildlv());
        pGD1->set_guildname(pDBTurns->guildname());

        pGD2->set_guildid(pDBTurns->guildenemyid());
        pGD2->set_guildlv(pDBTurns->guildenemylv());
        pGD2->set_guildname(pDBTurns->guildenemyname());

        if(pDBTurns->result()==GBR_Win2Way || pDBTurns->result()==GBR_Win3Way || pDBTurns->result()==GBR_WinNoEnemy)
        {
            pRet->set_rewardid(pGBtlTbl->m_nGuildWinReward);
        }
        else
        {
            pRet->set_rewardid(pGBtlTbl->m_nGuildLoseReward);
        }
    }
    for(int i=pDBReward->turns_size();i<3;i++)
    {
        SYS_LOG(nUserID,LT_CNTR_ERROR,GetRegion(),0,"Guild Battle Not End!!!"<<pGuild->GetGuildID()<<i);
        GuildBtlRetLite* pRet = pRseTotal->add_ret();
        pRet->set_result(GBR_Unknow);
        pRet->set_winway0(GBOResult_Unknow);
        pRet->set_winway1(GBOResult_Unknow);
        pRet->set_winway2(GBOResult_Unknow);
        pRet->set_score(0);
        GuildData* pGD1 = pRet->mutable_guild1();
        GuildData* pGD2 = pRet->mutable_guild2();
        pGD1->set_guildid(0);
        pGD1->set_guildlv(0);
        pGD1->set_guildname("");

        pGD2->set_guildid(0);
        pGD2->set_guildlv(0);
        pGD2->set_guildname("");

        pRet->set_rewardid(pGBtlTbl->m_nGuildWinReward);

    }
    return bSucc;
}


bool CountryRgnDatHandler::GetMatchGuild(CGuild** ppGuild1,CGuild** ppGuild2,int nTurns,int& nSize)
{
	bool bFind = false;
	
	CCountry* pECountry = GetEarthCountry();
	DB_C_PubGuildBtlInfo* pDBPGB = pECountry->GetDBCountry()->mutable_pubguildbtlinfo();
	int nIdx = pDBPGB->pklstidx();
	/*if(nIdx >= pDBPGB->pklst_size())
	{
		nIdx = 0;
	}*/
	nSize =  pDBPGB->pklst_size();
	for(int i=nIdx;i<pDBPGB->pklst_size();i++)
	{
		DB_C_PubGuildPKGroup* pDBGroup = pDBPGB->mutable_pklst(i);
		int nGuildID1 = pDBGroup->guildid1();
		int nGuildID2 = pDBGroup->guildid2();
		if(nGuildID1<=0&&nGuildID2<=0)
			continue;

		(*ppGuild1) = GetGuild(pDBGroup->guildid1());
		(*ppGuild2) = GetGuild(pDBGroup->guildid2());
		/*
		if(*ppGuild1==NULL||*ppGuild2==NULL)
		{//有一方轮空，没必要入栈了
			bFind = true;
			pDBGroup->set_guildid1(0);
			pDBGroup->set_guildid2(0);
			nIdx = i+1;
			break;
		}
		DB_C_GuildBtlResult* pDBRet1 = GetDBGuildBtlResult(*ppGuild1,nTurns);
		DB_C_GuildBtlResult* pDBRet2 = GetDBGuildBtlResult(*ppGuild2,nTurns);
		if(pDBRet1->winway0()!=0&&
			pDBRet1->winway1()!=0&&
			pDBRet1->winway2()!=0
			)
		{//该组战斗已经结束
			//
			pDBGroup->set_guildid1(0);
			pDBGroup->set_guildid2(0);
			continue;
		}
		if(pDBRet2->winway0()!=0&&
			pDBRet2->winway1()!=0&&
			pDBRet2->winway2()!=0
			)
		{//该组战斗已经结束
			//
			pDBGroup->set_guildid1(0);
			pDBGroup->set_guildid2(0);
			continue;
		}*/
		bFind = true;
		nIdx = i+1;
		break;
	}
	/*if(!bFind)
	{
		int nEnd = nIdx;
		for(int i=0;i<nEnd;i++)
		{
			DB_C_PubGuildPKGroup* pDBGroup = pDBPGB->mutable_pklst(i);
			int nGuildID1 = pDBGroup->guildid1();
			int nGuildID2 = pDBGroup->guildid2();
			if(nGuildID1<=0&&nGuildID2<=0)
				continue;

			(*ppGuild1) = GetGuild(pDBGroup->guildid1());
			(*ppGuild2) = GetGuild(pDBGroup->guildid2());

			if(*ppGuild1==NULL||*ppGuild2==NULL)
			{//有一方轮空，没必要入栈了
				bFind = true;
				pDBGroup->set_guildid1(0);
				pDBGroup->set_guildid2(0);
				nIdx = i+1;
				break;
			}
			DB_C_GuildBtlResult* pDBRet1 = GetDBGuildBtlResult(*ppGuild1,nTurns);
			DB_C_GuildBtlResult* pDBRet2 = GetDBGuildBtlResult(*ppGuild2,nTurns);
			if(pDBRet1->winway0()!=0&&
				pDBRet1->winway1()!=0&&
				pDBRet1->winway2()!=0
				)
			{//该组战斗已经结束
				//
				pDBGroup->set_guildid1(0);
				pDBGroup->set_guildid2(0);
				continue;
			}
			if(pDBRet2->winway0()!=0&&
				pDBRet2->winway1()!=0&&
				pDBRet2->winway2()!=0
				)
			{//该组战斗已经结束
				//
				pDBGroup->set_guildid1(0);
				pDBGroup->set_guildid2(0);
				continue;
			}
			bFind = true;
			nIdx = i+1;
			break;
		}
	}*/
	if(bFind)
	{
		pDBPGB->set_pklstidx(nIdx);
		markCountryDirty(pECountry);
	}
	return bFind;
}

void CountryRgnDatHandler::DealCombineRegion_Country()
{
	DB_C_Country* pDBEarth = GetEarthCountry()->GetDBCountry();

	pDBEarth->clear_lastrank();
	pDBEarth->clear_pubguildbtlinfo();
	pDBEarth->clear_gbtlrecord();
	pDBEarth->clear_btlrecord();
	pDBEarth->clear_arenarank();
	pDBEarth->clear_nactivityindex();
	pDBEarth->clear_worldarenarank();
	pDBEarth->clear_officerinfo();
	pDBEarth->clear_npcactiverank();
	
	map<int,CCountry*>::iterator iter;
	for(iter = m_mapCountry.begin();iter!=m_mapCountry.end();iter++)
	{
		CCountry* pCountry = iter->second;
		DB_C_Country* pDBCountry = pCountry->GetDBCountry();
		pDBCountry->clear_lastrank();
		pDBCountry->clear_pubguildbtlinfo();
		pDBCountry->clear_gbtlrecord();
		pDBCountry->clear_btlrecord();
		pDBCountry->clear_arenarank();
		pDBCountry->clear_nactivityindex();
		pDBCountry->clear_worldarenarank();
		pDBCountry->clear_officerinfo();

		pDBCountry->clear_atktargetcity();
		pDBCountry->clear_deftargetcity();
		pDBCountry->clear_npcactivecnt();
	}
}


void CountryRgnDatHandler::CheckGveRank()
{
	static int nOneHour = 3600;
	time_t ltNow = time(NULL);

	GveBaseTbl* pTbl = GVEInfoCfg::Instance().GetGveBaseTbl();
	if ( pTbl != NULL )
	{
		ResetFirstDown(pTbl->m_nID);
		if ( m_nFlag != pTbl->m_nID )
		{
			ResetGveGuildRank();
			m_nFlag = pTbl->m_nID;
			m_ltLastAllGuildGveRank = time(NULL);
			return;
		}
		if ( pTbl->CanSave(ltNow) && ltNow - m_ltLastAllGuildGveRank > nOneHour )
		{
			ResetGveGuildRank();
			m_ltLastAllGuildGveRank = time(NULL);
			return;
		}
	}

	if(ltNow - m_ltLastAllGuildGveRank > nOneHour)
	{
		for ( int i = 0; i < m_lstGuildGveNow.size(); i++ )
		{
			CGuild* pGuild = m_lstGuildGveNow[i];
			if ( pGuild != NULL )
			{
				pGuild->CheckGve();
			}
		}
		sort(m_lstGuildGveNow.begin(),m_lstGuildGveNow.end(),compareGuildGve);

		for ( int i = C_Start; i < C_UserCoutryEnd; i++ )
		{
			sort(m_lstGuildGveNowNew[i].begin(),m_lstGuildGveNowNew[i].end(),compareGuildGve);
		}

		m_ltLastAllGuildGveRank = time(NULL);
	}

}

void CountryRgnDatHandler::ResetGveGuildRank()
{
	for ( int i = 0; i < m_lstGuildGveNow.size(); i++ )
	{
		CGuild* pGuild = m_lstGuildGveNow[i];
		if ( pGuild != NULL )
		{
			pGuild->CheckGve();
		}
	}
	sort(m_lstGuildGveNow.begin(),m_lstGuildGveNow.end(),compareGuildGve);
	sort(m_lstGuildGveLast.begin(),m_lstGuildGveLast.end(),compareGuildGveLast);
	sort(m_lstGuildGveHighest.begin(),m_lstGuildGveHighest.end(),compareGuildGveHighest);

	for ( int i = C_Start; i < C_UserCoutryEnd; i++ )
	{
		sort(m_lstGuildGveNowNew[i].begin(),m_lstGuildGveNowNew[i].end(),compareGuildGve);
		sort(m_lstGuildGveLastNew[i].begin(),m_lstGuildGveLastNew[i].end(),compareGuildGveLast);
		sort(m_lstGuildGveHighestNew[i].begin(),m_lstGuildGveHighestNew[i].end(),compareGuildGveHighest);
	}
}

void CountryRgnDatHandler::CheckGuildGveMsg(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return;
	}

	time_t tNow = time(NULL);
	static int nHour = 3600;

	if(tNow - pGuild->m_ltLastGveMsg <= nHour)
	{
		return;
	}

	DB_GVE_Msg* pDBGveMsg = pGuild->GetDBGve()->mutable_nowmsg();
	if ( pDBGveMsg == NULL )
	{
		return;
	}

	int nStep = pDBGveMsg->gvestep();
	int nPlayCnt = pDBGveMsg->playercnt();
	int nPoint = pDBGveMsg->point();

	int nStepOverCnt = 0;
	int nPlayCntOverCnt = 0;
	int nPointOverCnt = 0;

	int nAllCnt = 0;

	for ( int i = 0; i < m_lstGuild[C_AllCountry].size(); i++ )
	{
		CGuild* pGuildOther = m_lstGuild[C_AllCountry][i];
		if ( pGuildOther == NULL )
		{
			continue;
		}
		if ( pGuildOther == pGuild )
		{
			continue;
		}

		if ( pGuildOther->GetGuildLV() < 2 )
		{
			continue;
		}
		
		DB_GVE_Msg* pDBGveMsgOther = pGuildOther->GetDBGve()->mutable_nowmsg();
		if ( pDBGveMsgOther == NULL )
		{
			continue;
		}

		int nStepOther = pDBGveMsgOther->gvestep();
		int nPlayCntOther = pDBGveMsgOther->playercnt();
		int nPointOther = pDBGveMsgOther->point();

		if ( pGuild->GetDBGve()->flag() != pGuildOther->GetDBGve()->flag() )
		{
			//对方公会未进行本次GVE
			nStepOther = GVE_START_STEP;
			nPlayCntOther = 0;;
			nPointOther = 0;
		}


		if ( nStepOther == -1 )
		{
			//未初始化
			nStepOther = GVE_START_STEP;
		}

		if ( nStepOther < nStep )
		{
			nStepOverCnt++;
		}

		if ( nPlayCntOther < nPlayCnt )
		{
			nPlayCntOverCnt++;
		}

		if ( nPointOther < nPoint )
		{
			nPointOverCnt++;
		}

		nAllCnt++;
	}

	if ( nAllCnt != 0 )
	{
		pDBGveMsg->set_overguildstep(nStepOverCnt*100/nAllCnt);
		pDBGveMsg->set_overguildmem(nPlayCntOverCnt*100/nAllCnt);
		pDBGveMsg->set_overguildpoint(nPointOverCnt*100/nAllCnt);
	}

	pDBGveMsg->set_rank(GetRank(pGuild));



	pGuild->m_ltLastGveMsg = tNow;
}

int CountryRgnDatHandler::GetRank(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return -1;
	}

	if ( pGuild->m_nPoint <= 0 )
	{
		return -1;
	}

	for ( int i = 0; i < m_lstGuildGveNow.size(); i++ )
	{
		if ( pGuild == m_lstGuildGveNow[i])
		{
			return i+1;
		}
	}
	return -1;
}

int CountryRgnDatHandler::GetLastRank(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return -1;
	}

	DB_GVE_Msg* pDBGveMsg = pGuild->GetDBGve()->mutable_lastmsg();
	if ( pDBGveMsg->point() <= 0 )
	{
		return -1;
	}

	for ( int i = 0; i < m_lstGuildGveLast.size(); i++ )
	{
		if ( pGuild == m_lstGuildGveLast[i])
		{
			return i+1;
		}
	}
	return -1;
}

int CountryRgnDatHandler::GetHighestRank(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return -1;
	}

	for ( int i = 0; i < m_lstGuildGveHighest.size(); i++ )
	{
		if ( pGuild == m_lstGuildGveHighest[i])
		{
			return i+1;
		}
	}
	return -1;
}

int CountryRgnDatHandler::GetLastRankNew(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return -1;
	}

	int nCountry = pGuild->GetCountry();
	if ( nCountry < C_Start || nCountry > C_UserCoutryEnd )
	{
		return -1;
	}

	DB_GVE_Msg* pDBGveMsg = pGuild->GetDBGve()->mutable_lastmsg();
	if ( pDBGveMsg->point() <= 0 )
	{
		return -1;
	}

	for ( int i = 0; i < m_lstGuildGveLastNew[nCountry].size(); i++ )
	{
		if ( pGuild == m_lstGuildGveLastNew[nCountry][i])
		{
			return i+1;
		}
	}
	return -1;
}

int CountryRgnDatHandler::GetHighestRankNew(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return -1;
	}

	int nCountry = pGuild->GetCountry();
	if ( nCountry < C_Start || nCountry > C_UserCoutryEnd )
	{
		return -1;
	}

	for ( int i = 0; i < m_lstGuildGveHighestNew[nCountry].size(); i++ )
	{
		if ( pGuild == m_lstGuildGveHighestNew[nCountry][i])
		{
			return i+1;
		}
	}
	return -1;
}


int CountryRgnDatHandler::GetOverCnt(CGuild* pGuild)
{
	if ( pGuild == NULL )
	{
		return 0;
	}

	DB_GVE_City* pDB_Gve = pGuild->GetDBGve();

	if ( pDB_Gve == NULL )
	{
		return 0;
	}

	int nCnt = 0;
	int nStep = pDB_Gve->nowmsg().gvestep();

	for ( int i = 0; i < m_lstGuildGveNow.size(); i++ )
	{
		CGuild* pGuildAnother;
		pGuildAnother = m_lstGuildGveNow[i];
			
		if ( pGuildAnother == NULL )
		{
			continue;
		}

		DB_GVE_City* pDB_GveAnother = pGuildAnother->GetDBGve();

		if ( pDB_GveAnother->nowmsg().gvestep() > nStep )
		{
			nCnt++;
		}
	}
	return nCnt;
}

void CountryRgnDatHandler::SetFirstDown( CGuild* pGuild, int nStep )
{
	if ( pGuild == NULL || nStep < GVE_START_STEP )
	{
		return;
	}

	DB_GVE_FirstDown* pDBDownMsg = NULL;
	DB_C_Country* pDBCountry = m_earthCountry.GetDBCountry();
	if( pDBCountry == NULL )
	{
		return;
	}

	for ( int i = 0; i < pDBCountry->gvefirstdownlist_size(); i++ )
	{
		pDBDownMsg = pDBCountry->mutable_gvefirstdownlist(i);
		if ( pDBDownMsg->stepid() == nStep )
		{
			return;
		}
	}
	
	pDBDownMsg = pDBCountry->add_gvefirstdownlist();
	if ( pDBDownMsg == NULL )
	{
		return;
	}
	pDBDownMsg->set_stepid(nStep);
	pDBDownMsg->set_guildid(pGuild->GetGuildID());
	pDBDownMsg->set_guildname(pGuild->GetGuildName());
	pDBDownMsg->set_guildcountry(pGuild->GetCountry());

	SYS_LOG(pGuild->GetGuildID(),LT_GVE_COUNGRY_FirstDown,0,0,nStep);

	markCountryDirty(&m_earthCountry);
}

void CountryRgnDatHandler::ResetFirstDown( int nFlag )
{
	DB_C_Country* pDBCountry = m_earthCountry.GetDBCountry();
	if( pDBCountry == NULL )
	{
		return;
	}
	if ( nFlag != pDBCountry->gveflag() )
	{
		pDBCountry->clear_gvefirstdownlist();
		pDBCountry->set_gveflag(nFlag);
	}
}

void CountryRgnDatHandler::FillFirstDownMsg(RseGuildOpt* pRseOpt)
{
	if ( pRseOpt == NULL )
	{
		return;
	}

	DB_C_Country* pDBCountry = m_earthCountry.GetDBCountry();
	if( pDBCountry == NULL )
	{
		return;
	}


	for ( int i = 0; i < pDBCountry->gvefirstdownlist_size(); i++ )
	{
		DB_GVE_FirstDown* pDBDownMsg = pDBCountry->mutable_gvefirstdownlist(i);
		if ( pDBDownMsg == NULL )
		{
			continue;
		}

		GVEFirstDown* pFDMsg = pRseOpt->add_firstdownlist();
		pFDMsg->set_step(pDBDownMsg->stepid());
		pFDMsg->set_guildid(pDBDownMsg->guildid());
		pFDMsg->set_guildname(pDBDownMsg->guildname());
		pFDMsg->set_guildcountry(pDBDownMsg->guildcountry());
	}
}

void	CCountry::ClearHeroArenaData()
{
	m_rmapNpcHeroArena.clear();
	m_rvecPlayHeroArena.clear();
	DB_HeroArena_Rank* pHeroArenaRank = m_dbCountry.mutable_heroarenarank();
	pHeroArenaRank->clear_heroarenaranknpc();
}

bool	 CCountry::GetHeroRankInfo(CountryRgnDatHandler* pRDH,RseHeroArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL||pRDH==NULL)
	{
		return false;
	}

	DB_HeroArena_Rank* pDBHeroArenaRank = m_dbCountry.mutable_heroarenarank();

	//获取昨日排行榜
	HeroArenaRank* pHeroArenaRank = pArenaRankInfo->mutable_arenaranklist();
	DB_HeroArena_Rank_List* pDBHeroArenaRankLast = pDBHeroArenaRank->mutable_heroarenaranklast();
	pHeroArenaRank->set_ranktime(pDBHeroArenaRankLast->ranktimelast());
	pHeroArenaRank->set_activityindex(pDBHeroArenaRankLast->activityindex());
	for(int i=0;i<pDBHeroArenaRankLast->heroarenaranklist_size();i++)
	{
		DB_HeroArena_Rank_Unit* pDBUnit = pDBHeroArenaRankLast->mutable_heroarenaranklist(i);
		if(pDBUnit)
		{
			HeroArenaRankUnit* pUnit = pHeroArenaRank->add_arenaranklist();
			pUnit->set_index(i);
			pUnit->set_uid(toString(pDBUnit->uid()));
		}
	}
	return true;
}

bool	 CCountry::GetHeroRankFightInfo(CountryRgnDatHandler* pRDH,int nPos,RseHeroArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL||pRDH==NULL)
	{
		return false;
	}

	DB_HeroArena_Rank* pDBHeroArenaRank = m_dbCountry.mutable_heroarenarank();

	//获取可攻击的敌人
	HeroArenaFightRuleUnit* pRuleUnit = HeroArenaFightRule::Instance().GetWalkSpaceByRankIndex(nPos);
	if(pRuleUnit == NULL)
	{
		return false;
	}

	HeroArenaRank* pArenaRankList = pArenaRankInfo->mutable_arenaranklist();
	pArenaRankList->set_ranktime((int)time(NULL));
	pArenaRankList->set_activityindex(pDBHeroArenaRank->currentactivityindex());


	for(int i=0;i<HERO_ARENA_FIGHT_RULE_NUM;i++)
	{
		HeroArenaRankUnit* pUnit = pArenaRankList->add_arenaranklist();

		int nIndex = nPos;

		switch(pRuleUnit->m_nwalkWay)
		{
		case E_HERO_ARENA_FIGHT_RULE_PEC:		//乘百分比
			{
				nIndex = nIndex * pRuleUnit->m_nArgv[i] / 100;
			}
			break;
		case E_HERO_ARENA_FIGHT_RULE_DEL:		//减数值
			{
				nIndex = nIndex - pRuleUnit->m_nArgv[i];
			}
			break;
		case E_HERO_ARENA_FIGHT_RULE_INDEX:		//名次值
			{
				nIndex = pRuleUnit->m_nArgv[i];
			}
			break;
		}
		int64 uid = GetHeroRankUidByIndex(nIndex);
		pUnit->set_index(nIndex);
		pUnit->set_uid(toString(uid));
	}

	HeroArenaRank* pArenatopList = pArenaRankInfo->mutable_arenaattacklist();
	pArenatopList->set_ranktime((int)time(NULL));
	pArenatopList->set_activityindex(pDBHeroArenaRank->currentactivityindex());

	//前10名
	if(pRuleUnit->m_nwalkWay != E_HERO_ARENA_FIGHT_RULE_INDEX)
	{
		for(int i=0;i<HERO_ARENA_FIGHT_RULE_NUM;i++)
		{
			int64 uid = GetHeroRankUidByIndex(i);
			HeroArenaRankUnit* pUnit = pArenatopList->add_arenaranklist();
			pUnit->set_index(i);
			pUnit->set_uid(toString(uid));
		}
	}

	return true;
}

bool	 CCountry::GetLastdayHeroRankFightInfo(RseHeroArenaRankInfo* pArenaRankInfo)
{
	if(pArenaRankInfo == NULL)
	{
		return false;
	}

	DB_HeroArena_Rank* pDBHeroArenaRank = m_dbCountry.mutable_heroarenarank();

	HeroArenaRank* pArenatopList = pArenaRankInfo->mutable_arenaattacklist();
	pArenatopList->set_ranktime((int)time(NULL));
	pArenatopList->set_activityindex(pDBHeroArenaRank->currentactivityindex());

	//前10名
	for(int i=0;i<HERO_ARENA_FIGHT_RULE_NUM;i++)
	{
		int64 uid = GetHeroRankUidByIndex(i);
		HeroArenaRankUnit* pUnit = pArenatopList->add_arenaranklist();
		pUnit->set_index(i);
		pUnit->set_uid(toString(uid));
	}

	return true;
}

int64	 CCountry::GetHeroRankUidByIndex(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_rvecPlayHeroArena.size())
	{
		return 0;
	}

	return m_rvecPlayHeroArena[nIndex];
}

int		 CCountry::AddHeroNewPlayer(DB_C_User* pCUser)
{
	if(m_nCountryID != 0)
	{
		return -1;
	}
	if(pCUser == NULL)
	{
		return -1;
	}
	int nIndex = m_rvecPlayHeroArena.size();
	if(pCUser->heroarenaindex() > 0)
	{
		return pCUser->heroarenaindex();
	}
	m_rvecPlayHeroArena.push_back(pCUser->userid());
	pCUser->set_heroarenaindex(nIndex);
	SYS_LOG((int64)pCUser->userid(),LT_ARENA_HERO_JION,m_nRegion,0,(int64)pCUser->userid()<<pCUser->heroarenaindex()<<pCUser->countryid());
	return nIndex;
}


int		 CCountry::AddHeroNewNpc(int64 nUid)
{
	if(m_nCountryID != 0)
	{
		return -1;
	}
	int nIndex = m_rvecPlayHeroArena.size();
	m_rvecPlayHeroArena.push_back(nUid);
	DB_HeroArena_Rank* pArenaRank = m_dbCountry.mutable_heroarenarank();
	DB_Arena_Rank_NPC* pArenaNpc = pArenaRank->add_heroarenaranknpc();
	pArenaNpc->set_npcindex(nIndex);
	pArenaNpc->set_npcid((int)nUid);
	m_rmapNpcHeroArena[(int)nUid].push_back(pArenaNpc);
	return nIndex;
}

void	 CCountry::InitHeroRankNpc()
{
	if(m_nCountryID != 0)
	{
		return ;
	}
	DB_HeroArena_Rank* pArenaRank = m_dbCountry.mutable_heroarenarank();
	pArenaRank->clear_heroarenaranknpc();

	int nNpcCunt = HeroArenaNpcInital::Instance().GetArenaNpcInitalUnitCnt();

	for(int i=0;i<nNpcCunt;i++)
	{
		HeroArenaNpcInitalUnit* pUnit = HeroArenaNpcInital::Instance().GetArenaNpcInitalUnitByIndex(i);
		if(pUnit)
		{
			for(int j=0;j<pUnit->m_nNum;j++)
			{
				AddHeroNewNpc(pUnit->m_nNpcID);
			}
		}
	}
}

bool	CCountry::UpdateHeroRankEveryDay()
{
	if(m_nCountryID != 0)
	{
		return false;
	}
	bool bUpdate = UpdateHeroRankNpc();
	DB_HeroArena_Rank* pArenaRank = m_dbCountry.mutable_heroarenarank();
	DB_HeroArena_Rank_List* pHeroArenaRankLast = pArenaRank->mutable_heroarenaranklast();
	time_t nowTime = time(NULL);
	time_t lastTime = pHeroArenaRankLast->ranktimelast();

	struct tm nowtime = *localtime(&nowTime);
	struct tm lastime = *localtime(&lastTime);

	if (lastime.tm_yday != nowtime.tm_yday || lastime.tm_year != nowtime.tm_year)
	{		
		//更新昨日排行
		pHeroArenaRankLast->clear_heroarenaranklist();
		int nRankNow = E_HERO_ARENA_RANK_NUM;
		if((int)m_rvecPlayHeroArena.size() < nRankNow)
		{
			nRankNow = m_rvecPlayHeroArena.size();
		}
		for(int i=0;i<nRankNow;i++)
		{
			int64 uid = m_rvecPlayHeroArena[i];
			DB_HeroArena_Rank_Unit* pUnit=pHeroArenaRankLast->add_heroarenaranklist();
			pUnit->set_uid(uid);
		}
		pHeroArenaRankLast->set_ranktimelast((int)nowTime);
		pHeroArenaRankLast->set_activityindex(pArenaRank->currentactivityindex());
		bUpdate = true;
		SYS_LOG(0,LT_ARENA_HERO_RANKSTART,m_nRegion,0,m_nCountryID<<lastTime<<nowTime<<nRankNow);
	}
	return bUpdate;
}

bool	CCountry::UpdateHeroRankNpc()
{
	if(m_nCountryID != 0)
	{
		return false;
	}
	bool bUpdate = false;

	DB_HeroArena_Rank* pArenaRank = m_dbCountry.mutable_heroarenarank();
	if(pArenaRank->heroarenaranknpc_size() <= 0)
	{
		InitHeroRankNpc();
		bUpdate = true;
	}
	return bUpdate;
}

int		 CCountry::CheckPlayerHeroRankIndex(CountryRgnDatHandler* pRDH,int64 nUid)
{
	if(m_nCountryID != 0||NULL == pRDH)
	{
		return -1;
	}
	DB_C_User* pDBUser = pRDH->GetDBCityUser(nUid);
	if(pDBUser == NULL||pDBUser->heroarenaindex()<0)
	{
		return -1;
	}
	int   nIndex = pDBUser->heroarenaindex();
	int64 nUidTmp = GetHeroRankUidByIndex(nIndex);
	if(nUid!=nUidTmp)
	{//数据错乱，重新排序
		DB_C_User* pDBUserTmp = pRDH->GetDBCityUser(nUidTmp);
		UpdateHeroArenaList(pRDH);
		SYS_LOG(0,LT_ARENA_HERO_RERANK,m_nRegion,0,nIndex<<(int64)nUid<<pDBUser->heroarenaindex()<<(int64)nUidTmp<<pDBUserTmp->heroarenaindex());
	}
	return pDBUser->heroarenaindex();
}

void	 CCountry::UpdateHeroArenaList(CountryRgnDatHandler* pRDH)
{
	if(m_nCountryID != 0)
	{
		return ;
	}
	if(pRDH)
	{
		DB_HeroArena_Rank* pArenaR = m_dbCountry.mutable_heroarenarank();
		pRDH->UpdateHeroArenaList(pArenaR,m_rvecPlayHeroArena);
		m_rmapNpcHeroArena.clear();
		for(int i=0;i<pArenaR->heroarenaranknpc_size();i++)
		{
			DB_Arena_Rank_NPC* pNode = pArenaR->mutable_heroarenaranknpc(i);
			if(pNode)
			{
				m_rmapNpcHeroArena[pNode->npcid()].push_back(pNode);
			}
		}
	}
}


void	 CCountry::UpdateHeroArenaPos(int nPos,int64 nuidO,int64 nuidN)
{
	if(m_nCountryID!=0)
	{
		return;
	}
	if(nPos < 0 || nPos >= (int)m_rvecPlayHeroArena.size())
	{
		return;
	}

	if(nuidO == m_rvecPlayHeroArena[nPos])
	{
		m_rvecPlayHeroArena[nPos] = nuidN;
	}
}

bool	 CCountry::ChangeHeroArenaPos(CountryRgnDatHandler* pRDH,int64 uidSelf,int posSelf,int64 uidFight,int posFight)
{
	if(pRDH==NULL||posSelf<0||posFight<0)
	{
		return false;
	}
	if(uidSelf >= 1000000000 && uidFight >= 1000000000)		//都是玩家
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_C_User* pCUserF = pRDH->GetDBCityUser(uidFight);

		if(pCUserS && pCUserS->heroarenaindex() == posSelf && pCUserF && pCUserF->heroarenaindex() == posFight)
		{

			UpdateHeroArenaPos(posSelf,pCUserS->userid(),pCUserF->userid());
			UpdateHeroArenaPos(posFight,pCUserF->userid(),pCUserS->userid());

			pCUserS->set_heroarenaindex(posFight);
			pCUserF->set_heroarenaindex(posSelf);
			return true;
		}
	}

	if(uidSelf >= 1000000000 && uidFight < 1000000000)		//玩家 NPC
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_Arena_Rank_NPC* pNpc = GetHeroArenaNpc((int)uidFight,posFight);

		if(pCUserS && pCUserS->heroarenaindex() == posSelf && pNpc)
		{
			UpdateHeroArenaPos(posSelf,pCUserS->userid(),pNpc->npcid());
			UpdateHeroArenaPos(posFight,pNpc->npcid(),pCUserS->userid());

			pCUserS->set_heroarenaindex(posFight);
			pNpc->set_npcindex(posSelf);
			return true;
		}
	}

	return false;
}

DB_Arena_Rank_NPC* CCountry::GetHeroArenaNpc(int nId,int nPos)
{
	vector<DB_Arena_Rank_NPC*>& rVec = m_rmapNpcHeroArena[nId];
	for(int i=0;i<(int)rVec.size();i++)
	{
		DB_Arena_Rank_NPC* pNode = rVec[i];
		if(pNode && pNode->npcindex() == nPos)
		{
			return pNode;
		}
	}
	return NULL;
}

bool	 CCountry::CheckHeroArenaPos(CountryRgnDatHandler* pRDH,int64 uidSelf,int posSelf,int64 uidFight,int posFight)
{
	if(pRDH==NULL||posSelf<0||posFight<0)
	{
		return false;
	}
	if(uidSelf >= 1000000000 && uidFight >= 1000000000)		//都是玩家
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_C_User* pCUserF = pRDH->GetDBCityUser(uidFight);

		if(pCUserS && pCUserS->heroarenaindex() == posSelf && pCUserF && pCUserF->heroarenaindex() == posFight)
		{
			return true;
		}
	}

	if(uidSelf >= 1000000000 && uidFight < 1000000000)		//玩家 NPC
	{
		DB_C_User* pCUserS = pRDH->GetDBCityUser(uidSelf);
		DB_Arena_Rank_NPC* pNpc = GetHeroArenaNpc((int)uidFight,posFight);
		if(pCUserS && pCUserS->heroarenaindex() == posSelf && pNpc)
		{
			return true;
		}
	}

	return false;
}

int		CCountry::GetHeroArenaPlayerNum()
{
	return m_rvecPlayHeroArena.size();
}

void	CountryRgnDatHandler::UpdateHeroArenaData()
{
	CCountry* pEarth = GetEarthCountry();
	if(pEarth==NULL)
	{
		return;
	}
	DB_HeroArena_Rank* pDBHeroRank = pEarth->GetDBCountry()->mutable_heroarenarank();
	if(pDBHeroRank)
	{
		//每天更新
		if(true==pEarth->UpdateHeroRankEveryDay())
		{
			markCountryDirty(pEarth);
		}
		//每期更新
		int nActiveID = HeroArenaActive::Instance().GetArenaActiveID();
		int nOldActiveID	=	pDBHeroRank->currentactivityindex();
		if(nActiveID != nOldActiveID)
		{
			ClearHeroArenaIndex(false);
			ClearHeroArenaData();
			pEarth->UpdateHeroRankNpc();
			pDBHeroRank->set_currentactivityindex(nActiveID);
			markCountryDirty(pEarth);
			SYS_LOG(0,LT_ARENA_HERO_RELOADDATA,m_nRegion,0,nActiveID<<nOldActiveID);
		}
	}
}

void CountryRgnDatHandler::UpdateHeroArenaList(DB_HeroArena_Rank* pArenaR,vector<int64>& rVecUid)
{
	if(pArenaR == NULL)
	{
		return;
	}

	rVecUid.clear();

	vector<DB_Arena_Node> rVecUserTmp;
	rVecUserTmp.clear();
	vector<DB_Arena_Node> rVecErrUserTmp;
	rVecErrUserTmp.clear();

	//玩家
	for(int countryIndex=C_Start;countryIndex<C_UserCoutryEnd;countryIndex++)
	{
		vector<DB_C_User*>& rvecList = m_vecCountryUser[countryIndex];
		for(int i=0;i<(int)rvecList.size();i++)
		{
			DB_C_User* pUser = rvecList[i];
			if(pUser && pUser->countryid() ==countryIndex&&pUser->heroarenaindex()>=0)
			{
				_UpdateHeroArenaUserList(pUser,rVecUserTmp,rVecErrUserTmp);
			}
		}

	}
	//名次冲突玩家
	for(int i=0;i<(int)rVecErrUserTmp.size();i++)
	{
		DB_Arena_Node& rNode = rVecErrUserTmp[i];
		if(rNode.pUser)
		{
			_UpdateHeroArenaErrUserList(rNode.pUser,rVecUserTmp);
			SYS_LOG(0,LT_ARENA_HERO_RANKERRO,m_nRegion,0,2<<(int64)rNode.pUser->userid()<<(int64)rNode.pUser->userid()<<rNode.pUser->heroarenaindex());
		}
	}

	//NPC
	for(int i=0;i<pArenaR->heroarenaranknpc_size();i++)
	{
		_UpdateHeroArenaNpcList(pArenaR->mutable_heroarenaranknpc(i),rVecUserTmp);
	}

	for(int i=0;i<(int)rVecUserTmp.size();i++)
	{
		DB_Arena_Node& rNode = rVecUserTmp[i];
		if(rNode.pUser)
		{
			rVecUid.push_back(rNode.pUser->userid());
			rNode.pUser->set_heroarenaindex(rVecUid.size() - 1);
			markUserLstDirty(rNode.pUser->userid());
		}

		if(rNode.pNpc)
		{
			rVecUid.push_back(rNode.pNpc->npcid());
			rNode.pNpc->set_npcindex(rVecUid.size() - 1);
		}
	}
	markCountryDirty(&m_earthCountry);
}

void	CountryRgnDatHandler::ClearHeroArenaIndex(bool bSaveLast /* = true */)
{
	for(hash_map<int64,DB_C_User*>::iterator iter = m_mapUser.begin();iter != m_mapUser.end();iter ++)
	{
		DB_C_User* pUser = iter->second;
		if(pUser)
		{
			if(pUser->heroarenaindex() != -1)
			{
				pUser->set_heroarenaindex(-1);
			}
		}
	}

	for(map<int,DB_C_UserList*>::iterator iter = m_mapUserLst.begin();iter != m_mapUserLst.end();iter ++)
	{
		DB_C_UserList* pUserList = iter->second;
		if(pUserList)
		{
			markUserLstDirty(pUserList);
		}
	}
}

void	CountryRgnDatHandler::ClearHeroArenaData()
{
	CCountry* pEarth = GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	pEarth->ClearHeroArenaData();
	markCountryDirty(pEarth);
}

void CountryRgnDatHandler::_UpdateHeroArenaUserList(DB_C_User* pUser,vector<DB_Arena_Node>& rUserVec,vector<DB_Arena_Node>& rErrVec)
{
	if(pUser == NULL)
	{
		return;
	}

	int64 nUid = pUser->userid();
	int nPos = pUser->heroarenaindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		DB_Arena_Node rNode;
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rErrVec.push_back(rNode);
		if(rOldNode.pUser != NULL)
		{
			SYS_LOG(0,LT_ARENA_HERO_RANKERRO,m_nRegion,0,0<<(int64)pUser->userid()<<(int64)rOldNode.pUser->userid()<<nPos);
		}
		else if(rOldNode.pNpc != NULL)
		{
			SYS_LOG(0,LT_ARENA_HERO_RANKERRO,m_nRegion,0,1<<(int64)pUser->userid()<<rOldNode.pNpc->npcid()<<nPos);
		}
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rUserVec[nPos] = rNode;
	}
}void CountryRgnDatHandler::_UpdateHeroArenaErrUserList(DB_C_User* pUser,vector<DB_Arena_Node>& rUserVec)
{
	if(pUser == NULL)
	{
		return;
	}

	int64 nUid = pUser->userid();
	int nPos = pUser->heroarenaindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		pUser->set_heroarenaindex(nPos + 1);
		_UpdateHeroArenaErrUserList(pUser,rUserVec);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = pUser;
		rNode.pNpc = NULL;
		rUserVec[nPos] = rNode;
	}
}
void CountryRgnDatHandler::_UpdateHeroArenaNpcList(DB_Arena_Rank_NPC* pNpc,vector<DB_Arena_Node>& rUserVec)
{
	if(pNpc == NULL)
	{
		return;
	}

	int64 nUid = pNpc->npcid();
	int nPos = pNpc->npcindex();

	if(nUid <= 0 || nPos < 0)
	{
		return;
	}

	int nSize = rUserVec.size();
	DB_Arena_Node rNode;
	rNode.pUser = NULL;
	rNode.pNpc = NULL;
	for(int i=nSize;i<=nPos;i++)
	{
		rUserVec.push_back(rNode);
	}

	DB_Arena_Node& rOldNode = rUserVec[nPos];
	if((rOldNode.pUser != NULL && rOldNode.pUser->userid() != nUid)
		||(rOldNode.pNpc != NULL && rOldNode.pNpc->npcid() != nUid))
	{
		//说明Pos错误，名次往后延
		pNpc->set_npcindex(nPos + 1);
		_UpdateHeroArenaNpcList(pNpc,rUserVec);
		return;
	}

	if(rOldNode.pUser == NULL && rOldNode.pNpc == NULL)
	{
		rNode.pUser = NULL;
		rNode.pNpc = pNpc;
		rUserVec[nPos] = rNode;
	}
}

CResource* CountryRgnDatHandler::GetCResource( int nID )
{
	int nType = nID / MAX_RESOURSE_TYPE_ID;
	if ( nType > MAX_RESOURSE_SEARCH_TYPE || nType <= 0 )
	{
		return NULL;
	}
	map<int,CResource*>::iterator iter;
	iter = m_lstResource[nType].find( nID );
	if ( iter != m_lstResource[nType].end() )
	{
		return iter->second;
	}
	return NULL;
}

CResource* CountryRgnDatHandler::GetRandCResource( int nType )
{	
	if ( nType > MAX_RESOURSE_SEARCH_TYPE || nType <= 0 )
	{
		return NULL;
	}
	
	int nSize = m_lstResource[nType].size();

	if ( nSize == 0 )
	{
		return NULL;
	}

	int nRand = rand()%nSize;

	map<int,CResource*>::iterator iter;
	iter = m_lstResource[nType].begin();
	for ( int i = 0; i < nRand; i++ )
	{
		if ( iter == m_lstResource[nType].end() )
		{
			return NULL;
		}
		iter++;
	}

	return iter->second;
}

CResource* CountryRgnDatHandler::GetCResourceByNum( int nNum, int nType )
{
	int nSize = m_lstResource[nType].size();

	if ( nSize == 0 )
	{
		return NULL;
	}

	if ( nNum >= nSize )
	{
		return NULL;
	}

	map<int,CResource*>::iterator iter;
	iter = m_lstResource[nType].begin();
	for ( int i = 0; i < nNum; i++ )
	{
		if ( iter == m_lstResource[nType].end() )
		{
			return NULL;
		}
		iter++;
	}

	return iter->second;
}

CGvgCity* CountryRgnDatHandler::GetGvgCity( int nCityID )
{
	map<int, CGvgCity*>::iterator iter = m_mapGvgCity.find(nCityID);
	if(iter!=m_mapGvgCity.end())
		return iter->second;
	return NULL;
}

void CountryRgnDatHandler::CheckGvg()
{
	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}

	time_t tNow = time(NULL);

	if ( m_nGvgFlag != pActiveTbl->m_nID )
	{
		CheckAllGvgCity( pActiveTbl->m_nID );
		m_nGvgFlag = pActiveTbl->m_nID;
		m_ltLastGvgCheck = tNow;
	}

	if ( tNow - m_ltLastGvgCheck >= GVG_UPDATE_TIME )
	{
		CheckAllGvgCity( pActiveTbl->m_nID );
		m_ltLastGvgCheck = tNow;
	}
}

void CountryRgnDatHandler::CheckAllGvgCity( int nFlag )
{
	const GvgActiveTbl* pActiveTbl = GvgCfg::Instance().GetActiveTbl();
	if ( pActiveTbl == NULL )
	{
		return;
	}

	DB_C_PubGvgBtlInfo* pPublicInfo = GetGvgEarthInfo();
	if ( pPublicInfo == NULL )
	{
		return;
	}

	map<int,CGvgCity*>::iterator iter;
	for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
	{
		CGvgCity* pCity = iter->second;
		if ( pCity == NULL )
		{
			continue;
		}

		DB_C_GvgCity* pDBCity = pCity->GetDBGvgCity();
		if ( pDBCity == NULL )
		{
			continue;
		}

		const GvgCityTbl* pCityTbl = GvgCfg::Instance().GetCityTbl( pDBCity->cityid() );
		if ( pCityTbl == NULL )
		{
			continue;
		}

		if ( pDBCity->activeflag() != nFlag )
		{
			//下一期活动
			int nWinID = 0;
			DB_C_GvgBtlRet* pBtlRet = pCity->GetDBRet( 2 );
			//获取获胜城市ID
			if ( pBtlRet != NULL )
			{
				nWinID = pBtlRet->winguildid();
			}

			if ( nWinID >= 0 )
			{
				CGuild* pTmpGuild = GetGuild( pDBCity->guildid() );
				if ( pTmpGuild != NULL )
				{
					//让原来的工会离开城市
					if ( pDBCity->guildid() != nWinID )
					{
						pTmpGuild->LeaveGvgCity( pCity->m_nCityID );

					}
				}
				bool bGet = false;
				CGuild* pGuild = GetGuild(nWinID);
				if ( pGuild != NULL )
				{
					pCity->SetGvgCityGuildMsg(pGuild);

					if ( pDBCity->guildid() == nWinID )
					{
						//连胜次数加一
						pDBCity->set_guildstreak( pDBCity->guildstreak() + 1 );
					}
					else
					{
						pDBCity->set_guildstreak(1);
					}
					pDBCity->set_guildid(nWinID);
					bGet = pGuild->GetGvgCity( pCity->m_nCityID );
					if ( bGet )
					{
						markGuildLstDirty(pGuild);
						SYS_LOG(0,LT_CNTGVG_GET_CITY,0,0,nWinID<<pCity->m_nCityID);
					}
					else
					{
						//该工会已经获得占领城市
						pDBCity->set_guildid(0);
						pCity->ClearGuildMsg();
					}
				}
			}
			
			if ( pDBCity->guildstreak() > 1 && pDBCity->guildid() > 0 )
			{
				//连胜>1,减少城防
				pCity->AddGvgCityPoint( -pCityTbl->m_nPointMinus );

			}
			else
			{
				pCity->SetGvgCityPoint( pCityTbl->m_nPoint );
			}

			int nOldFlag = pDBCity->activeflag();

			pDBCity->set_activeflag( nFlag );
			pDBCity->clear_signguildlist();
			pDBCity->set_step(GvgBtlType_UnKnow);
			pCity->m_lstSignGuild.clear();

			if ( nFlag == 3 && nOldFlag == 2 )
			{
				//清除第二期公会数据
				pCity->SetGvgCityPoint( pCityTbl->m_nPoint );
				pDBCity->set_guildid(0);
				pDBCity->clear_gvgbtl();
			}
		}

		CheckGvgRank( pCity );
		CheckFightGuild( pCity, pActiveTbl->GetGvgStep() );

		markGvgCityDirty(pCity);
	}

	//清楚重复公会占领的城市数据
	for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
	{
		CGvgCity* pCity = iter->second;
		if ( pCity == NULL )
		{
			continue;
		}

		DB_C_GvgCity* pDBCity = pCity->GetDBGvgCity();
		if ( pDBCity == NULL )
		{
			continue;
		}

		CGuild* pGuild = GetGuild(pDBCity->guildid());
		if ( pGuild == NULL )
		{
			pDBCity->set_guildid(0);
			markGvgCityDirty(pCity);
			pCity->ClearGuildMsg();
			continue;
		}
		DB_C_GVG_Msg* pDBGvgMsg = pGuild->GetDBGvgMsg();
		if ( pDBGvgMsg == NULL )
		{
			pDBCity->set_guildid(0);
			markGvgCityDirty(pCity);
			pCity->ClearGuildMsg();
			continue;
		}

		if ( pDBGvgMsg->controlcity() == 0 )
		{
			pDBGvgMsg->set_controlcity(pCity->m_nCityID);
		}

		else if ( pDBGvgMsg->controlcity() != pCity->m_nCityID )
		{
			//改公会已经占领另一城市
			pDBCity->set_guildid(0);
			markGvgCityDirty(pCity);
			pCity->ClearGuildMsg();
			continue;
		}
			
		markGvgCityDirty(pCity);
	}

	pPublicInfo->set_flag(nFlag);
	markCountryDirty(GetEarthCountry());
}

void CountryRgnDatHandler::FillGvgAllCityData( RseGvgOpt* pRseOpt )
{
	if ( pRseOpt == NULL )
	{
		return;
	}
	
	map<int,CGvgCity*>::iterator iter;
	for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
	{
		CGvgCity* pCity = iter->second;
		if ( pCity == NULL )
		{
			continue;
		}

		DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
		if ( pDBGvgCity == NULL )
		{
			continue;
		}

		GvgCity* pGvgCity = pRseOpt->add_allgvgcitydata();
		if ( pGvgCity == NULL )
		{
			continue;
		}

		pGvgCity->set_guildid( pDBGvgCity->guildid() );
		pGvgCity->set_cityid( pCity->m_nCityID );
		pGvgCity->set_guildname( pCity->m_strGuildName );
		pGvgCity->set_guildcountry( pCity->m_nCountryID );
		pGvgCity->set_citypoint( pDBGvgCity->citypoint() );
	}
}

void CountryRgnDatHandler::FillGvgCityData( RseGvgOpt* pRseOpt, int nCityID, GCG_GuildOpt* pOpt, int nGuildID )
{
	if ( pRseOpt == NULL )
	{
		return;
	}

	map<int,CGvgCity*>::iterator iter = m_mapGvgCity.find( nCityID );
	
	if ( iter == m_mapGvgCity.end() )
	{
		return;
	}
	
	CGvgCity* pCity = iter->second;
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

	pRseOpt->set_cityid( nCityID );
	pRseOpt->set_citypoint( pDBGvgCity->citypoint() );

	if ( pDBGvgCity->guildid() != 0 )
	{
		CGuild* pGuild = GetGuild( pDBGvgCity->guildid() );
		GuildData* pData = pRseOpt->mutable_guilddata();
		if ( pGuild != NULL &&  pData != NULL  )
		{
			pData->set_guildid( pGuild->GetGuildID() );
			pData->set_guildname( pGuild->GetGuildName() );
			pData->set_guildlv( pGuild->GetGuildLV() );
			pData->set_guilduesrcnt( pGuild->GetUserCnt() );

			DB_C_GVG_Msg* pDBGvgGuild = pGuild->GetDBGvgMsg();
			if ( pDBGvgGuild != NULL )
			{
				pData->set_contribute(pDBGvgGuild->contribute());
			}
			
			bool bAtk = !( nGuildID == pGuild->GetGuildID() );
			GetGvgFightUid(pCity,pOpt,bAtk);

		}
	}


	for ( int i = 0; i < pDBGvgCity->gvgbtl_size(); i++ )
	{
		DB_C_GvgBtlRet* pDBRet = pDBGvgCity->mutable_gvgbtl(i);
		GuildBtlRet* pRet = pRseOpt->add_bltret();
		pRet->set_session( pDBRet->session() );

		CGuild* pGuild1 = GetGuild( pDBRet->guildid1() );
		if ( pGuild1 != NULL )
		{
			pRet->set_guildid1( pGuild1->GetGuildID() );
			GuildData* pData1 = pRet->mutable_guild1();
			pData1->set_guildid( pGuild1->GetGuildID() );
			pData1->set_guildname( pGuild1->GetGuildName() );
			pData1->set_guildlv( pGuild1->GetGuildLV() );
			pData1->set_guilduesrcnt( pGuild1->GetUserCnt() );
		}

		CGuild* pGuild2 = GetGuild( pDBRet->guildid2() );
		if ( pGuild2 != NULL )
		{
			pRet->set_guildid2( pGuild2->GetGuildID() );
			GuildData* pData2 = pRet->mutable_guild2();
			pData2->set_guildid( pGuild2->GetGuildID() );
			pData2->set_guildname( pGuild2->GetGuildName() );
			pData2->set_guildlv( pGuild2->GetGuildLV() );
			pData2->set_guilduesrcnt( pGuild2->GetUserCnt() );
		}

		pRet->set_winid( pDBRet->winguildid() );
		pRet->set_bwinbyfight( pDBRet->bwinbyfight() );
	}
}

void CountryRgnDatHandler::FillGvgBtlData( RseGvgOpt* pRseOpt, int nCityID, int nSession )
{
	if ( pRseOpt == NULL )
	{
		return;
	}

	map<int,CGvgCity*>::iterator iter = m_mapGvgCity.find( nCityID );

	if ( iter == m_mapGvgCity.end() )
	{
		return;
	}

	CGvgCity* pCity = iter->second;
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

	pRseOpt->set_cityid( nCityID );
	pRseOpt->set_citypoint( pDBGvgCity->citypoint() );

	for ( int i = 0; i < pDBGvgCity->gvgbtl_size(); i++ )
	{
		DB_C_GvgBtlRet* pDBRet = pDBGvgCity->mutable_gvgbtl(i);
		GuildBtlRet* pRet = pRseOpt->add_bltret();

		if ( pDBRet->session() != nSession )
		{
			continue;
		}

		pRet->set_session( pDBRet->session() );

		CGuild* pGuild1 = GetGuild( pDBRet->guildid1() );
		if ( pGuild1 != NULL )
		{
			pRet->set_guildid1( pGuild1->GetGuildID() );
			GuildData* pData1 = pRet->mutable_guild1();
			pData1->set_guildid( pGuild1->GetGuildID() );
			pData1->set_guildname( pGuild1->GetGuildName() );
			pData1->set_guildlv( pGuild1->GetGuildLV() );
			pData1->set_guilduesrcnt( pGuild1->GetUserCnt() );
		}

		CGuild* pGuild2 = GetGuild( pDBRet->guildid2() );
		if ( pGuild2 != NULL )
		{
			pRet->set_guildid2( pGuild1->GetGuildID() );
			GuildData* pData2 = pRet->mutable_guild2();
			pData2->set_guildid( pGuild2->GetGuildID() );
			pData2->set_guildname( pGuild2->GetGuildName() );
			pData2->set_guildlv( pGuild2->GetGuildLV() );
			pData2->set_guilduesrcnt( pGuild2->GetUserCnt() );
		}

		pRet->set_winid( pDBRet->winguildid() );
	}
}

void CountryRgnDatHandler::CheckGvgRank( CGvgCity* pCity, GvgBtlStepType gvgStep )
{
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

// 	if ( pDBGvgCity->step() >= GvgBtlType_PreTime )
// 	{
// 		//已经决定对战公会
// 		return;
// 	}

	pCity->m_lstSignGuild.clear();
	pCity->m_mapGuildContribute.clear();

	for ( int i = 0; i < pDBGvgCity->signguildlist_size(); i++ )
	{
		DB_C_GvgSignMsg* pDBSignMsg = pDBGvgCity->mutable_signguildlist(i);

		if ( pDBSignMsg == NULL )
		{
			continue;
		}

		int nGuildID = pDBSignMsg->guildid();

		CGuild* pGuild = GetGuild( nGuildID );
		if ( pGuild == NULL )
		{
			continue;
		}
	
		DB_C_GVG_Msg* pDBGvgGuild = pGuild->GetDBGvgMsg();
		if ( pDBGvgGuild == NULL  )
		{
			continue;
		}

		//同步公会贡献
		pDBSignMsg->set_guildcontribute( pDBGvgGuild->contribute() );
		pCity->m_mapGuildContribute[nGuildID] = pDBGvgGuild->contribute();
		pCity->m_lstSignGuild.push_back( pGuild );
	}
	
	pCity->SortGvg();
}

int GetGuildID( CGuild* pGuild )
{
	if ( pGuild == NULL )
	{
		return 0;
	}

	return pGuild->GetGuildID();
}

CGuild* GetGvgGuild( vector<CGuild*>& lstSignCity, int nCnt )
{
	if ( nCnt >= lstSignCity.size() )
	{
		return NULL;
	}
	return lstSignCity[nCnt];
}

void CountryRgnDatHandler::CheckFightGuild( CGvgCity* pCity, GvgBtlStepType gvgStep )
{
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

	if ( gvgStep < GvgBtlType_PreTime )
	{
		return;
	}

	if ( pDBGvgCity->step() >= GvgBtlType_PreTime )
	{
		//已经到下一阶段
		return;
	}

// 	if ( pCity->m_lstSignGuild.size() == 0 )
// 	{
// 		//无人报名
// 		pDBGvgCity->set_step( GvgBtlType_PreTime );
// 		return;
// 	}

	pDBGvgCity->clear_gvgbtl();
	
	//记录对战公会
	if ( pDBGvgCity->guildid() == 0 )
	{
		//没有占领公会,取前三名战斗
// 		if ( pCity->m_lstSignGuild.size() <= 1 )
// 		{
// 			//只有一个公会报名,没有战斗
// 			pDBGvgCity->set_step( GvgBtlType_PreTime );
// 			return;
// 		}

		DB_C_GvgBtlRet* pDBGvgBtl1 = pDBGvgCity->add_gvgbtl();

		CGuild* pGuild1 = GetGvgGuild( pCity->m_lstSignGuild, 2 );

		pDBGvgBtl1->set_session( 1 );
		pDBGvgBtl1->set_guildid1( GetGuildID(pGuild1) );

		CGuild* pGuild2 = GetGvgGuild( pCity->m_lstSignGuild, 1 );

		if ( pDBGvgBtl1->guildid1() == 0 )
		{
			pDBGvgBtl1->set_guildid1( GetGuildID(pGuild2) );
		}
		else
		{
			pDBGvgBtl1->set_guildid2( GetGuildID(pGuild2) );
		}
		

		DB_C_GvgBtlRet* pDBGvgBtl2 = pDBGvgCity->add_gvgbtl();
		pDBGvgBtl2->set_session( 2 );
		CGuild* pGuild3 = GetGvgGuild( pCity->m_lstSignGuild, 0 );
		if ( pDBGvgBtl1->guildid1() == 0 )
		{
			pDBGvgBtl1->set_guildid1( GetGuildID(pGuild3) );
		}
		else if ( pDBGvgBtl1->guildid2() == 0 )
		{
			pDBGvgBtl1->set_guildid2( GetGuildID(pGuild3) );
		}
		else
		{
			pDBGvgBtl2->set_guildid2( GetGuildID(pGuild3) );
		}
		

		pDBGvgBtl1->set_cityid(pCity->m_nCityID);
		pDBGvgBtl2->set_cityid(pCity->m_nCityID);
	}
	else
	{
		//占领公会与前两名战斗
		DB_C_GvgBtlRet* pDBGvgBtl1 = pDBGvgCity->add_gvgbtl();

		CGuild* pGuild1 = GetGvgGuild( pCity->m_lstSignGuild, 1 );

		pDBGvgBtl1->set_session( 1 );
		pDBGvgBtl1->set_guildid1( GetGuildID(pGuild1) );

		CGuild* pGuild2 = GetGvgGuild( pCity->m_lstSignGuild, 0 );

		if ( pDBGvgBtl1->guildid1() == 0 )
		{
			pDBGvgBtl1->set_guildid1( GetGuildID(pGuild2) );
		}
		else
		{
			pDBGvgBtl1->set_guildid2( GetGuildID(pGuild2) );
		}


		DB_C_GvgBtlRet* pDBGvgBtl2 = pDBGvgCity->add_gvgbtl();
		pDBGvgBtl2->set_session( 2 );
		CGuild* pGuild3 = GetGuild( pDBGvgCity->guildid() );
		if ( pDBGvgBtl1->guildid1() == 0 )
		{
			pDBGvgBtl1->set_guildid1( GetGuildID(pGuild3) );
		}
		else if ( pDBGvgBtl1->guildid2() == 0 )
		{
			pDBGvgBtl1->set_guildid2( GetGuildID(pGuild3) );
		}
		else
		{
			pDBGvgBtl2->set_guildid2( GetGuildID(pGuild3) );
		}


		pDBGvgBtl1->set_cityid(pCity->m_nCityID);
		pDBGvgBtl2->set_cityid(pCity->m_nCityID);
	}

	pDBGvgCity->set_step( GvgBtlType_PreTime );

	markGvgCityDirty(pCity);
}

void CountryRgnDatHandler::FillGvgRank( RseGvgOpt* pRseOpt, CGvgCity* pCity )
{
	if ( pRseOpt == NULL || pCity == NULL )
	{
		return;
	}

	for ( int i = 0; i < pCity->m_lstSignGuild.size(); i++ )
	{
		CGuild* pRankGuild = pCity->m_lstSignGuild[i];
		if ( pRankGuild == NULL )
		{
			continue;
		}

		GuildData* pGuildData = pRseOpt->add_signguildlist();
		if ( pGuildData == NULL )
		{
			continue;
		}

		pGuildData->set_guildname( pRankGuild->GetGuildName() );
		pGuildData->set_guildid( pRankGuild->GetGuildID() );
		pGuildData->set_guildlv( pRankGuild->GetGuildLV() );
		pGuildData->set_leadername( pRankGuild->GetLeaderName() );

		map<int,int>::iterator iter = pCity->m_mapGuildContribute.find( pRankGuild->GetGuildID() );

		if ( iter == pCity->m_mapGuildContribute.end() )
		{
			pGuildData->set_contribute( 0 );
		}
		else
		{
			pGuildData->set_contribute( iter->second );
		}
	}
}

bool CountryRgnDatHandler::CheckCanEnterCity( CGvgCity* pCity, CGuild* pGuild, int nSession )
{
	if ( pCity == NULL )
	{
		return false;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return false;
	}

	DB_C_GvgBtlRet* pDBBtlRet = NULL;
	for ( int i = 0; i < pDBGvgCity->gvgbtl_size(); i++ )
	{
		if ( pDBGvgCity->gvgbtl(i).session() == nSession )
		{
			pDBBtlRet = pDBGvgCity->mutable_gvgbtl(i);
			break;
		}
	}

	if ( pDBBtlRet == NULL )
	{
		return false;
	}

	if ( pGuild->GetGuildID() != pDBBtlRet->guildid1() && pGuild->GetGuildID() != pDBBtlRet->guildid2() )
	{
		//不是该场战斗参加工会
		return false;
	}
	return true;
}


void CountryRgnDatHandler::FillGvgPlayerData( RseGvgOpt* pRseOpt, int nCityID, int nSession )
{
	if ( pRseOpt == NULL )
	{
		return;
	}

	CGvgCity* pCity = GetGvgCity(nCityID);
	if ( pCity == NULL )
	{
		return;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return;
	}

	int nGuild1 = 0;
	int nGuild2 = 0;

	pCity->GetFightGuild(nSession,nGuild1,nGuild2);

	if ( nGuild1 > 0 )
	{
		CGuild* pGuild = GetGuild( nGuild1 );
		if ( pGuild != NULL )
		{
			pGuild->FillGvgPlayerData(pRseOpt,nCityID,nSession);
		}
	}

	if ( nGuild2 > 0 )
	{
		CGuild* pGuild = GetGuild( nGuild2 );
		if ( pGuild != NULL )
		{
			pGuild->FillGvgPlayerData(pRseOpt,nCityID,nSession);
		}
	}
}

void CountryRgnDatHandler::SetPlayerProp( CGuild* pGuild, int64 uid, GCG_GuildOpt* pOpt )
{
	if ( pGuild == NULL || pOpt == NULL || uid == 0 )
	{
		return;
	}

	DB_C_GVG_Msg* pDBGvgGuild = pGuild->GetDBGvgMsg();
	if ( pDBGvgGuild == NULL )
	{
		return;
	}

	DB_C_GvgBtlAddProp* pAddProp = pGuild->GetDBGvgAddProp( uid, pOpt->session() );
	if ( pAddProp == NULL )
	{
		pAddProp = pDBGvgGuild->add_addprop();
		pAddProp->set_userid( uid );
		pAddProp->set_session( pOpt->session() ); 
	}

	pAddProp->set_name( pOpt->selfname() );
	pAddProp->set_pic( pOpt->selfpic() );

	pAddProp->set_addatkpct( pOpt->addatkpct() );
	pAddProp->set_addbloodpct( pOpt->addbloodpct() );
	pAddProp->set_wintime( pOpt->streak() );
	pAddProp->set_isoffline( pOpt->isgvgoffline() );
}

DB_C_GvgBtlRet* CountryRgnDatHandler::GetGvgFightCity( int nSession )
{
	time_t ltNow = time(NULL);

	map<int,CGvgCity*>::iterator iter;

	for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
	{
		CGvgCity* pCity = iter->second;
		if ( pCity == NULL )
		{
			continue;
		}

		DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
		if ( pDBGvgCity == NULL )
		{
			continue;
		}

		if ( !pCity->CanPushFight( ltNow ) )
		{
			continue;
		}

		DB_C_GvgBtlRet* pDBRet = pCity->GetDBRet(nSession);
		if ( pDBRet == NULL )
		{
			continue;
		}
		if ( pDBRet->winguildid() != -1 )
		{
			//该城市战斗已经结束
			continue;
		}

		CheckFightRet(pDBRet);
		if ( pDBRet->winguildid() != -1 )
		{
			//该城市战斗已经结束
			continue;
		}

		pCity->m_nLastRoundtime = ltNow;
		
		return pDBRet;
	}
	return NULL;
}

DB_C_GvgBtlRet* CountryRgnDatHandler::GetGvgFightCityByID( int nSession, int nCity )
{
	time_t ltNow = time(NULL);

	map<int,CGvgCity*>::iterator iter;

	for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
	{
		CGvgCity* pCity = iter->second;
		if ( pCity == NULL )
		{
			continue;
		}

		if ( nCity != pCity->m_nCityID )
		{
			continue;
		}

		DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
		if ( pDBGvgCity == NULL )
		{
			continue;
		}

		DB_C_GvgBtlRet* pDBRet = pCity->GetDBRet(nSession);
		if ( pDBRet->winguildid() != -1 )
		{
			//该城市战斗已经结束
			continue;
		}

		//CheckFightRet(pDBRet);

		return pDBRet;
	}
	return NULL;
}

void CountryRgnDatHandler::CheckFightRet( DB_C_GvgBtlRet* pDBRet )
{
	if ( pDBRet == NULL )
	{
		return;
	}

	if ( pDBRet->winguildid() != -1 )
	{
		return;
	}

	int nGuildID1 = pDBRet->guildid1();
	int nGuildID2 = pDBRet->guildid2();

	if ( nGuildID1 == -1 || nGuildID2 == -1 )
	{
		return;
	}
	CGuild* pGuild1;
	CGuild* pGuild2;
	int nWinID = -1;

	if ( nGuildID1 == 0 )
	{
		nWinID = nGuildID2;
	}
	if ( nGuildID2 == 0 )
	{
		nWinID = nGuildID1;
	}

	if ( nWinID == -1 )
	{
		CGuild* pGuild1 = GetGuild(nGuildID1);
		CGuild* pGuild2 = GetGuild(nGuildID2);
		int nPlayerCnt1 = 0;
		int nPlayerCnt2 = 0;

		//判断轮空的情况
		if ( pGuild1 == NULL )
		{
			nWinID = nGuildID2;
			pDBRet->set_bwinbyfight(false);

			SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug guild1 Null");
		}
		else
		{
			DB_C_GvgBtlCityMsg* pDBGvgCity = pGuild1->GetGvgBtlCity(pDBRet->cityid(),pDBRet->session());
			if ( pDBGvgCity != NULL )
			{
				nPlayerCnt1 = pGuild1->GetGvgPlayerCnt(pDBGvgCity);
			}
		}
		if ( pGuild2 == NULL )
		{
			nWinID = nGuildID1;
			pDBRet->set_bwinbyfight(false);

			SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug guild2 Null");
		}
		else
		{
			DB_C_GvgBtlCityMsg* pDBGvgCity = pGuild2->GetGvgBtlCity(pDBRet->cityid(),pDBRet->session());
			if ( pDBGvgCity != NULL )
			{
				nPlayerCnt2 = pGuild2->GetGvgPlayerCnt(pDBGvgCity);
			}
		}

		if ( pGuild1 == NULL && pGuild2 == NULL )
		{
			nWinID = 0;
			pDBRet->set_bwinbyfight(false);
			SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug guild Null");
		}

		if ( nWinID == -1 )
		{
			//继续通过人数判断
			if ( nPlayerCnt1 == 0 )
			{
				nWinID = nGuildID2;
				pDBRet->set_bwinbyfight(false);
				SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug player1 Null");
			}

			if ( nPlayerCnt2 == 0 )
			{
				nWinID = nGuildID1;
				pDBRet->set_bwinbyfight(false);
				SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug player2 Null");
			}

			if ( nPlayerCnt1 == 0 && nPlayerCnt2 == 0 )
			{
				nWinID = 0;
				pDBRet->set_bwinbyfight(false);
				SYS_LOG(0,LT_ARENA_COUNTRY_DEBUG,0,0,"GVGDebug player Null");
			}
		}
	}

	if ( nWinID != -1 )
	{
		//已出结果
		pDBRet->set_winguildid( nWinID );
		pDBRet->set_bwinbyfight(false);

		if ( pDBRet->session() == 1 )
		{
			//记录第二战公会
			DB_C_GvgBtlRet* pDBRet2 = GetGvgFightCityByID( 2, pDBRet->cityid() );
			if ( pDBRet2 != NULL )
			{
				pDBRet2->set_guildid1( nWinID );
			}	
		}
	}
}


void CountryRgnDatHandler::GetGvgFightUid( CGvgCity* pCity, GCG_GuildOpt* pOpt, bool bAtk )
{
	if ( pOpt == NULL || pCity == NULL )
	{
		return;
	}

	
	if ( !bAtk )
	{
		list<int64> &m_lstUser = pCity->m_lstAtkUser;
		if ( m_lstUser.size() <= 0 )
		{
			pOpt->set_taruserid(0);
			return;
		}
		list<int64>::iterator iter;
		iter = m_lstUser.begin();

		pOpt->set_taruserid(*iter);
	}
	else
	{
		list<int64> &m_lstUser = pCity->m_lstDefUser;
		if ( m_lstUser.size() <= 0 )
		{
			pOpt->set_taruserid(0);
			return;
		}
		list<int64>::iterator iter;
		iter = m_lstUser.begin();

		pOpt->set_taruserid(*iter);
	}


}

DB_C_PubGvgBtlInfo* CountryRgnDatHandler::GetGvgEarthInfo()
{
	DB_C_Country* pDBCountry = m_earthCountry.GetDBCountry();
	if ( pDBCountry == NULL )
	{
		return NULL;
	}

	return pDBCountry->mutable_pubgvgbtlinfo();
}

void CountryRgnDatHandler::GvgStat(CountryEventHandler* eh_)
{
	static int nOneHour = 3600;
	time_t ltNow = time(NULL);
	map<int,int> mapSignGuild;
	int nControlCnt = 0;

	if(ltNow - m_ltGvgLastStat > nOneHour)
	{
		m_ltGvgLastStat = ltNow;
		map<int,CGvgCity*>::iterator iter;
		for ( iter = m_mapGvgCity.begin(); iter != m_mapGvgCity.end(); iter++ )
		{
			CGvgCity* pGvgCity = iter->second;
			if ( pGvgCity == NULL )
			{
				continue;
			}

			DB_C_GvgCity* pDBGvg = pGvgCity->GetDBGvgCity();
			if ( pDBGvg == NULL )
			{
				continue;
			}

			for ( int i = 0; i < pDBGvg->signguildlist_size(); i++ )
			{
				DB_C_GvgSignMsg dbMsg = pDBGvg->signguildlist(i);
				int nGuildID = dbMsg.guildid();
				if ( nGuildID > 0 )
				{
					mapSignGuild[nGuildID] = 1;
				}
			}

			if ( pDBGvg->guildid() > 0 )
			{
				nControlCnt++;
			}
		}
		
		Event evn;
		evn.set_cmd(EVENT_GVG_STAT);
		evn.set_state(Status_Game_To_Country);
		evn.set_time(0);
		evn.set_uid(0);
		GCG_GvgStat* pStat = evn.mutable_gvgstat();

		string strStat;

		string strTmp;
		strTmp = toString( mapSignGuild.size() );
		strStat += strTmp;
		strStat += ",";
		strTmp.clear();
		strTmp = toString( nControlCnt );
		strStat += strTmp;
		strStat += ",";

		pStat->set_region(GetRegion());
		pStat->set_strstat(strStat);

		eh_->sendEventToGamed(&evn,1);
	}
}


bool CountryRgnDatHandler::CheckCanOffLineEnterCity( CGvgCity* pCity, CGuild* pGuild )
{
	if ( pCity == NULL )
	{
		return false;
	}

	DB_C_GvgCity* pDBGvgCity = pCity->GetDBGvgCity();
	if ( pDBGvgCity == NULL )
	{
		return false;
	}

	DB_C_GvgBtlRet* pDBBtlRet = NULL;
	for ( int i = 0; i < pDBGvgCity->gvgbtl_size(); i++ )
	{

		pDBBtlRet = pDBGvgCity->mutable_gvgbtl(i);

		if ( pDBBtlRet == NULL )
		{
			continue;
		}
		
		if ( pGuild->GetGuildID() == pDBBtlRet->guildid1() || pGuild->GetGuildID() == pDBBtlRet->guildid2() )
		{
			//不是该场战斗参加工会
			return true;
		}
		
	}
	return false;
}

void CountryRgnDatHandler::CheckWpcRank(CountryEventHandler* eh_)
{
	static int nTenMin = 600;
	time_t ltNow = time(NULL);
	DB_C_WpcRank* pDBRank = GetDBWpcRank();
	if ( pDBRank == NULL )
	{
		return;
	}

	int nAcitveID = WeaponRankActiveCfg::Instance().GetWpcRankActiveID(ltNow);
	if ( nAcitveID != pDBRank->activeid() )
	{
		SortWpcRank();
		pDBRank->clear_nowrank();
		vector<DB_C_User*>::iterator iter;
		for ( iter = m_lstWpcRank.begin(); iter != m_lstWpcRank.end(); iter++ )
		{
			DB_C_User* pDBUser = *iter;
			if ( pDBUser == NULL )
			{
				continue;
			}

			DB_C_WpcRankUnit* pDBUnit = pDBRank->add_nowrank();
			pDBUnit->set_uid(toString(pDBUser->userid()));
			pDBUnit->set_explorecnt(pDBUser->wpcsearchtime());
			pDBUnit->set_name(pDBUser->name());
			pDBUnit->set_pic(pDBUser->pic());
			pDBUnit->set_countryid(pDBUser->countryid());

			pDBUnit->set_fightpoint(GetWpcFightPoint(toString(pDBUser->userid())));
			pDBUnit->set_level(pDBUser->level());

			CGuild* pGuild = GetGuild(pDBUser->guildid());
			if ( pGuild != NULL )
			{
				pDBUnit->set_guildname(pGuild->GetGuildName());
			}
			
		}
		
		string sUidLast = GetChampoinUid();
		string sUidNow;


		pDBRank->clear_lastrank();
		for ( int i = 0; i < pDBRank->nowrank_size(); i++ )
		{
			DB_C_WpcRankUnit* pDBUnit = pDBRank->add_lastrank();
			pDBUnit->CopyFrom(pDBRank->nowrank(i));
		}

		sUidNow = GetChampoinUid();

		pDBRank->clear_nowrank();
		m_lstWpcRank.clear();
		pDBRank->set_activeid( nAcitveID );

		CCountry* pECountry = GetEarthCountry();
		if ( pECountry != NULL )
		{
			markCountryDirty(pECountry);
		}
		
		int64 uidNow,uidLast;
		safe_atoll( sUidNow, uidNow );
		safe_atoll( sUidLast, uidLast );
		DB_C_User* pDBLastUser = GetDBCityUser(uidLast);
		DB_C_User* pDBNowUser = GetDBCityUser(uidNow);

		eh_->SendCountryUserLiteInfo(this,pDBLastUser);
		eh_->SendCountryUserLiteInfo(this,pDBNowUser);

		m_strChampionUid.clear();

		//清除冠军显示
		map<int, CCity*>::iterator iterCity;
		for ( iterCity = m_mapCity.begin(); iterCity != m_mapCity.end(); iterCity++ )
		{
			CCity* pCity = iterCity->second;
			if ( pCity != NULL )
			{
				pCity->m_bChampionIn = false;
			}
		}
		return;
	}

	if ( ltNow - m_ltWpcRankCheck > nTenMin )
	{
		SortWpcRank();
		pDBRank->clear_nowrank();
		vector<DB_C_User*>::iterator iter;
		for ( iter = m_lstWpcRank.begin(); iter != m_lstWpcRank.end(); iter++ )
		{
			DB_C_User* pDBUser = *iter;
			if ( pDBUser == NULL )
			{
				continue;
			}
		
			DB_C_WpcRankUnit* pDBUnit = pDBRank->add_nowrank();
			pDBUnit->set_uid(toString(pDBUser->userid()));
			pDBUnit->set_explorecnt(pDBUser->wpcsearchtime());
			pDBUnit->set_name(pDBUser->name());
			pDBUnit->set_pic(pDBUser->pic());
			pDBUnit->set_countryid(pDBUser->countryid());
			
			pDBUnit->set_fightpoint(GetWpcFightPoint(toString(pDBUser->userid())));
			pDBUnit->set_level(pDBUser->level());

			CGuild* pGuild = GetGuild(pDBUser->guildid());
			if ( pGuild != NULL )
			{
				pDBUnit->set_guildname(pGuild->GetGuildName());
			}
		}
		m_ltWpcRankCheck = ltNow;

		CCountry* pECountry = GetEarthCountry();
		if ( pECountry != NULL )
		{
			markCountryDirty(pECountry);
		}
	}
}

DB_C_WpcRank* CountryRgnDatHandler::GetDBWpcRank()
{
	DB_C_Country* pDBCountry = GetEarthCountry()->GetDBCountry();
	if ( pDBCountry == NULL )
	{
		return NULL;
	}
	
	return pDBCountry->mutable_wpcrank();
}

void CountryRgnDatHandler::SortWpcRank()
{
	if ( m_lstWpcRank.size() <= MAX_WPC_RANK_NUM )
	{
		sort(m_lstWpcRank.begin(),m_lstWpcRank.end(),compareWpcRank);
	}
	else
	{
		sort(m_lstWpcRank.begin(),m_lstWpcRank.end(),compareWpcRank);
		m_lstWpcRank.pop_back();
	}
}

void CountryRgnDatHandler::SetWpcFightPoint( string uid, int nFightPoint )
{
	m_mapWpcFightPoint[uid] = nFightPoint;
}

int CountryRgnDatHandler::GetWpcFightPoint( string uid )
{
	map<string,int>::iterator iter = m_mapWpcFightPoint.find(uid);
	if ( iter != m_mapWpcFightPoint.end() )
	{
		return m_mapWpcFightPoint[uid];
	}
	return 0;
}

void CountryRgnDatHandler::FillWpcRank( RseWPCBaseOpen* pRseRet )
{
	DB_C_WpcRank* pDBWpcRank = GetDBWpcRank();
	if ( pDBWpcRank == NULL )
	{
		return;
	}

	for ( int i = 0; i < MAX_WPC_RANK_NUM && i < pDBWpcRank->nowrank_size(); i++ )
	{
		WpcRank* pDBUnit = pRseRet->add_list();
		DB_C_WpcRankUnit* pDBWpcRankUnit = pDBWpcRank->mutable_nowrank(i);
		
		if ( pDBUnit != NULL && pDBWpcRankUnit != NULL )
		{
			pDBUnit->set_uid(pDBWpcRankUnit->uid());
			pDBUnit->set_name(pDBWpcRankUnit->name());
			pDBUnit->set_pic(pDBWpcRankUnit->pic());
			pDBUnit->set_countryid(pDBWpcRankUnit->countryid());

			pDBUnit->set_fightpoint(pDBWpcRankUnit->fightpoint());
			pDBUnit->set_level(pDBWpcRankUnit->level());
			pDBUnit->set_explorecnt(pDBWpcRankUnit->explorecnt());
			pDBUnit->set_guildname(pDBWpcRankUnit->guildname());
		}
	}

	for ( int i = 0; i < MAX_WPC_RANK_NUM && i < pDBWpcRank->lastrank_size(); i++ )
	{
		WpcRank* pDBUnit = pRseRet->add_lastlist();
		DB_C_WpcRankUnit* pDBWpcRankUnit = pDBWpcRank->mutable_lastrank(i);

		if ( pDBUnit != NULL && pDBWpcRankUnit != NULL )
		{
			pDBUnit->set_uid(pDBWpcRankUnit->uid());
			pDBUnit->set_name(pDBWpcRankUnit->name());
			pDBUnit->set_pic(pDBWpcRankUnit->pic());
			pDBUnit->set_countryid(pDBWpcRankUnit->countryid());

			pDBUnit->set_fightpoint(pDBWpcRankUnit->fightpoint());
			pDBUnit->set_level(pDBWpcRankUnit->level());
			pDBUnit->set_explorecnt(pDBWpcRankUnit->explorecnt());
			pDBUnit->set_guildname(pDBWpcRankUnit->guildname());
		}
	}
}

void CountryRgnDatHandler::CheckWpcMsg( DB_C_User* pDBCUser )
{
	if ( pDBCUser == NULL )
	{
		return;
	}
	for ( int i = 0; i < m_lstWpcRank.size(); i++ )
	{
		DB_C_User* pDBTmpUser = m_lstWpcRank[i];
		if ( pDBTmpUser == pDBCUser )
		{
			return;
		}
	}

	if ( m_lstWpcRank.size() < MAX_WPC_RANK_NUM )
	{
		//人数不足十人
		m_lstWpcRank.push_back( pDBCUser );
		SortWpcRank();
		return;
	}

	int nMinSearchTime = 0; 
	if ( m_lstWpcRank.size() > 0 )
	{
		DB_C_User* pDBTmpUser = m_lstWpcRank[m_lstWpcRank.size() - 1];
		if ( pDBTmpUser != NULL )
		{
			nMinSearchTime = pDBTmpUser->wpcsearchtime();
		}
	}
	if ( pDBCUser->wpcsearchtime() > nMinSearchTime )
	{
		m_lstWpcRank.push_back( pDBCUser );
		SortWpcRank();
	}
}

int	CountryRgnDatHandler::GetWpcRankRight( DB_C_User* pDBCUser )
{
	if ( pDBCUser == NULL )
	{
		return 0;
	}

	DB_C_WpcRank* pDBWpcRank = GetDBWpcRank();
	if ( pDBWpcRank == NULL )
	{
		return 0;
	}

	if ( pDBWpcRank->lastrank_size() <= 0 )
	{
		return 0;
	}

	DB_C_WpcRankUnit* pUnit = pDBWpcRank->mutable_lastrank(0);
	if ( pUnit == NULL )
	{
		return 0;
	}

	int64 uid;
	safe_atoll(pUnit->uid(),uid); 

	if ( uid == pDBCUser->userid() )
	{
		return WPC_RANK_CHAMPION;
	}
	return 0;
}


void CountryRgnDatHandler::FillWpcRankChampion( RseWPCBaseOpen* pRseRet )
{
	DB_C_WpcRank* pDBWpcRank = GetDBWpcRank();
	if ( pDBWpcRank == NULL )
	{
		return;
	}

	if ( pDBWpcRank->lastrank_size() <= 0 )
	{
		return;
	}

	WpcRank* pDBUnit = pRseRet->mutable_champion();
	DB_C_WpcRankUnit* pDBWpcRankUnit = pDBWpcRank->mutable_lastrank(0);
	{
		pDBUnit->set_uid(pDBWpcRankUnit->uid());
		pDBUnit->set_name(pDBWpcRankUnit->name());
		pDBUnit->set_pic(pDBWpcRankUnit->pic());
		pDBUnit->set_countryid(pDBWpcRankUnit->countryid());

		pDBUnit->set_fightpoint(pDBWpcRankUnit->fightpoint());
		pDBUnit->set_level(pDBWpcRankUnit->level());
		pDBUnit->set_explorecnt(pDBWpcRankUnit->explorecnt());
		pDBUnit->set_guildname(pDBWpcRankUnit->guildname());
	}
}

void CountryRgnDatHandler::LoadWpcRankUser(bool bCombineRegionOpt )
{
	DB_C_WpcRank* pDBWpcRank = GetDBWpcRank();
	if ( pDBWpcRank == NULL )
	{
		return;
	}

	m_lstWpcRank.clear();
	if ( bCombineRegionOpt )
	{
		//合服操作，清除数据
		pDBWpcRank->clear_lastrank();
		pDBWpcRank->clear_nowrank();
		SortWpcRank();
		return;
	}
	
	for ( int i = 0; i < pDBWpcRank->nowrank_size(); i++ )
	{
		DB_C_WpcRankUnit* pDBWpcRankUnit = pDBWpcRank->mutable_nowrank(i);

		int64 uid;
		safe_atoll(pDBWpcRankUnit->uid(),uid);
		DB_C_User* pDBCUser = GetDBCityUser(uid);
		if ( pDBCUser != NULL )
		{
			m_lstWpcRank.push_back(pDBCUser);
		}
		SetWpcFightPoint(pDBWpcRankUnit->uid(),pDBWpcRankUnit->fightpoint());
	}

	SortWpcRank();
}

string CountryRgnDatHandler::GetChampoinUid()
{
	string uid; 
	uid.clear();

	DB_C_WpcRank* pDBWpcRank = GetDBWpcRank();
	if ( pDBWpcRank == NULL )
	{
		return uid;
	}

	if ( m_strChampionUid.size() > 0 )
	{
		return m_strChampionUid;
	}

	if ( pDBWpcRank->lastrank_size() <= 0 )
	{
		return uid;
	}
	else
	{
		DB_C_WpcRankUnit* pDBWpcRankUnit = pDBWpcRank->mutable_lastrank(0);
		if ( pDBWpcRankUnit != NULL )
		{
			uid = pDBWpcRankUnit->uid();
		}
	}

	if ( uid.size() > 0 )
	{
		m_strChampionUid = uid;
	}
	return uid;
}

bool CountryRgnDatHandler::IsChampion( int64 uid )
{
	if ( uid == 0 )
	{
		return false;
	}
	int64 championUid = 0;
	
	safe_atoll( GetChampoinUid(), championUid );
	if ( uid == championUid )
	{
		return true;
	}
	return false;
}

CWarGameSite* CountryRgnDatHandler::GetWarGameSite(int nSiteID)
{
    return m_WarGameData.GetWarGameSiteByID(nSiteID);
}

CWarGameSite* CountryRgnDatHandler::GetWarGameSiteByUid(string strUid)
{
    return m_WarGameData.GetWarGameSiteByUid(strUid);
}

void CountryRgnDatHandler::FillAllWarGameSiteInfo(RseWarGameOpt *pRse)
{
    m_WarGameData.FillAllWarGameSiteInfo(pRse);
}

DB_C_CountryLeagueMsg* CountryRgnDatHandler::GetDBCountryLeague( int leagueID )
{
	DB_C_Country* pDBCountry = GetEarthCountry()->GetDBCountry();
	if ( pDBCountry == NULL )
	{
		return NULL;
	}

	if ( leagueID > LEAGUEBTLCNTEND || leagueID < LEAGUEBTLCNTSTART )
	{
		return NULL;
	}

	DB_C_CountryLeagueMsg* pDBMsg = NULL;

	for ( int i = 0; i < pDBCountry->leagueallmsg_size(); i++ )
	{
		pDBMsg = pDBCountry->mutable_leagueallmsg(i);
		if ( pDBMsg->leagueid() == leagueID )
		{
			return pDBMsg;
		}
	}

	pDBMsg = pDBCountry->add_leagueallmsg();
	pDBMsg->set_leagueid(leagueID);
	return pDBMsg;
}

CCountryLeague* CountryRgnDatHandler::GetCountryLeague( int leagueID )
{
	map<int,CCountryLeague*>::iterator iter = m_mapLeagueBattle.find( leagueID );
	if ( iter != m_mapLeagueBattle.end() )
	{
		return iter->second;
	}
	return NULL;
}

void CountryRgnDatHandler::LoadLeagueBattle(bool bCombineRegionOpt)
{
	m_mapLeagueBattle.clear();
	if ( bCombineRegionOpt )
	{
// 		//合服操作，清除数据

		return;
	}

	for ( int i = LEAGUEBTLCNTSTART; i <= LEAGUEBTLCNTEND; i++ )
	{
		DB_C_CountryLeagueMsg* pDBMsg = GetDBCountryLeague(i);
		if ( pDBMsg == NULL )
		{
			//数据错误
			return;
		}

		CCountryLeague* pLeague = new CCountryLeague(i,pDBMsg);
		m_mapLeagueBattle[i] = pLeague;

		for ( int j = 0; j < pDBMsg->guildlist_size(); j++ )
		{
			DB_C_LeagueGuildUnit* pGuildUnit = pDBMsg->mutable_guildlist(j);
			CGuild* pGuild = GetGuild(pGuildUnit->guildid());
			if ( pGuild != NULL )
			{
				pLeague->m_lstGuildRank.push_back(pGuild);
			}
		}

		for ( int j = 0; j < pDBMsg->playerranklist_size(); j++ )
		{
			DB_C_LeaguePlayerUnit* pPlayerUnit = pDBMsg->mutable_playerranklist(j);
			int64 uid = 0;
			safe_atoll( pPlayerUnit->uid(), uid );

			DB_C_User* pUser = GetDBCityUser( uid );
			if ( pUser != NULL )
			{
				pLeague->m_lstUserRank.push_back(pUser);
			}
		}
	}

}

bool compareLeagueCountry(CCountry* pCountry1, CCountry* pCountry2)
{
	DB_C_CountryLeagueBattle* pDBCountryLeagueBattle1 = pCountry1->GetDBCountryLeagueBattle();
	DB_C_CountryLeagueBattle* pDBCountryLeagueBattle2 = pCountry2->GetDBCountryLeagueBattle();
	if ( pDBCountryLeagueBattle1->boom() > pDBCountryLeagueBattle2->boom() )
	{
		return true;
	}

	else if ( pDBCountryLeagueBattle1->boom() < pDBCountryLeagueBattle2->boom() )
	{
		return false;
	}

	else if ( pCountry1->GetCountryID() > pCountry2->GetCountryID() )
	{
		return true;
	}

	else
	{
		return false;
	}

}

bool CountryRgnDatHandler::OnBuffCityChangeCountry(int nCityId)
{
	DB_C_CityBuffData *pBuffData = GetCityBuffData();
	if(pBuffData && pBuffData->state() == E_CITYBUFF_STATE_ACTIVE){
		for(int i = 0; i < pBuffData->randombuffs_size(); i++){
			if(pBuffData->randombuffs(i).cityid() == nCityId){
				DB_C_RandomBuff *pBuff = pBuffData->mutable_randombuffs(i);
				pBuff->set_activetm(int(time(NULL)));
				markCountryDirty(&m_earthCountry);
				return true;
			}
		}
	}
	return false;
}

void CountryRgnDatHandler::CheckCityBuffActive(CountryEventHandler* eh_)
{
	DB_C_CityBuffData* pBuffData = GetCityBuffData();
	if(!pBuffData){
		return;
	}
	if(!pBuffData->randombuffs_size()){
		pBuffData->set_flag(0);
		pBuffData->set_state(E_CITYBUFF_STATE_INVALID);
		for(int i = 0; i < E_CITYBUFF_COUNT; i++){
			DB_C_RandomBuff* pBuff = pBuffData->add_randombuffs();
			if(pBuff){
				pBuff->set_cityid(E_CITYBUFF_FANDIGANG + i);
				pBuff->set_buffid(0);
				pBuff->set_activetm(0);
			}
		}
		markCountryDirty(&m_earthCountry);
	}

	int nFlag = 0, nState = 0;
	CountryInfoCfg::Instance().GetBuffActiveTime(nFlag, nState);
	if(nFlag == pBuffData->flag() && nState == pBuffData->state()){
		return;
	}

	switch (nState)
	{
	case E_CITYBUFF_STATE_PRE:
		{
			//if(E_CITYBUFF_STATE_INVALID == pBuffData->state()){
				pBuffData->set_flag(nFlag);
				pBuffData->set_state(nState);
				time_t curTm = time(NULL);
				for(int i = 0; i < pBuffData->randombuffs_size(); i++){
					DB_C_RandomBuff *pBuff = pBuffData->mutable_randombuffs(i);
					if(pBuff){
						pBuff->set_activetm(int(curTm));
					}
				}
				markCountryDirty(&m_earthCountry);

				Event evt;
				evt.set_cmd(EVENT_CITYBUFF_INFO);
				evt.set_state(Status_Country_To_Game);
				evt.set_time(0);
				RseCityBuffInfo *pRse = evt.mutable_se_rsecitybuffinfo();
				if(pRse){
					pRse->set_region(GetRegion());
					FillCityBuffInfo(pRse);
					eh_->sendEventToAllGamed(&evt);
				}
				
				for(int idx = 0; idx < pBuffData->randombuffs_size(); idx++){
					int nCityId = pBuffData->randombuffs(idx).cityid();
					CCity* pCity = GetCity(nCityId);
					if(pCity && false == pCity->m_bNPCCity){
						//把城市内玩家移回首都
						for(int i = 0; i < CITY_USER_GROUP; i++){
							for(int j = 0; j < (int)pCity->m_lstUser[i].size(); j++){
								DB_C_User* pDBCUser = pCity->m_lstUser[i][j];
								if(!pDBCUser){
									continue;
								}
								if(pDBCUser->cityid() != pCity->GetCityID()){
									continue;
								}

								bool bIsChampion = this->IsChampion(pDBCUser->userid());
								CCity* pCaptail = GetCountry(pDBCUser->countryid())->GetCapital();
								pCaptail->UserEnterCity(pDBCUser,bIsChampion);
								this->markUserLstDirty(pDBCUser->userid());

								Event evn;
								evn.set_cmd(EVENT_CITYBUFF_RETURNHOME);
								evn.set_state(Status_Game_To_Country);
								evn.set_time(0);
								evn.set_uid(pDBCUser->userid());
								eh_->sendEventToGamed(&evn, eh_->getDataHandler()->getGamedIdByUserId(pDBCUser->userid()));
								eh_->SendOfficerRightUsed(this, this->GetCity(pDBCUser->cityid()),pDBCUser->userid());
							}
							pCity->m_lstUser[i].clear();
						}  
						CityChgCountry(0, pCity, C_NPC);
						eh_->SendCountryLiteInfo();

						//重置城市内npc
						DB_C_City *dbCity = pCity->GetDBCity();
						dbCity->clear_npc();
						dbCity->clear_npcinittm();
						dbCity->clear_lastbloodbtl();
						dbCity->clear_officerrightuselst();

						CCountry *pUserCountry = GetCountry(pCity->GetCountry());
						if(pUserCountry){
							pUserCountry->EraseCity(pCity->GetCityID());
							markCountryDirty(pUserCountry);
						}
						const CityDataTbl *pTbl = CountryInfoCfg::Instance().GetCityTbl(nCityId);
						dbCity->set_countryid(pTbl->m_nCountry);
						dbCity->set_cityid(pTbl->m_nID);
						dbCity->set_region(m_nRegion);
						CCountry *pCountry = GetCountry(pTbl->m_nCountry);
						if(pCountry){
							pCountry->AddCity(pCity);
							markCountryDirty(pCountry);
						}
						
						if(dbCity->npcinittm()!=CountryInfoCfg::Instance().GetCountryBaseTbl()->m_ltNPCResetTM)
						{//初始化NPCID
							int nInitMorale = CountryInfoCfg::Instance().GetCountryBaseTbl()->m_nInitMorale;
							dbCity->clear_npc();
							int nIdx = 0;
							map<int,int>::const_iterator itNPC;
							for(itNPC = pTbl->m_mapNPCArmy.begin(); itNPC != pTbl->m_mapNPCArmy.end();itNPC++)
							{
								int nNPCID = itNPC->first;
								int nCnt   = itNPC->second;
								for(int k=0;k<nCnt&&k<COUNTRY_NPC_RATE;k++)
								{
									DB_C_NPC* pNPC = dbCity->add_npc();
									pNPC->set_npcid(nNPCID*COUNTRY_NPC_RATE+nIdx);
									pNPC->set_morale(nInitMorale);
									nIdx++;
								}
							}
							dbCity->set_npcinittm(CountryInfoCfg::Instance().GetCountryBaseTbl()->m_ltNPCResetTM);
						}
						int nIdx = 0;
						int nUserCnt = 0;
						while(nIdx < dbCity->npc_size())
						{//清除挂掉的NPC
							DB_C_NPC* pDBNPC = dbCity->mutable_npc(nIdx);
							if(pDBNPC->npcid()<=0)
							{
								int nLastIdx = dbCity->npc_size()-1;
								if(nLastIdx>=0)
								{
									DB_C_NPC* pLastdata = dbCity->mutable_npc(nLastIdx);
									pDBNPC->Swap( pLastdata);
									//SYS_LOG(m_pPlayer->GetUserID(),LT_ClearData,0,0,"building"<<pLastdata->id()<<pLastdata->type());
									dbCity->mutable_npc()->RemoveLast();
									continue;
								}
							}
							else
							{
								int nNPCID = pDBNPC->npcid();
								nNPCID /= COUNTRY_NPC_RATE;
								nNPCID *= COUNTRY_NPC_RATE;
								nNPCID += nIdx;
								pDBNPC->set_npcid(nNPCID);
								pCity->m_lstNPC.push_back(pDBNPC);
								nIdx++;
								nUserCnt++;
							}
						}
						markCityDirty(pCity);
					}
				}
				SYS_LOG(0, LT_CITYBUFF_FLAGSTATE_CHANGE, GetRegion(), 0, "flag:"<<nFlag<<"state:"<<nState);
			//}
		}
		break;
	case E_CITYBUFF_STATE_ACTIVE:
		{
			//if(E_CITYBUFF_STATE_PRE == pBuffData->state()){
				pBuffData->set_flag(nFlag);
				pBuffData->set_state(nState);

				time_t nTime = time(NULL);
				vector<int> vecBuff;
				CountryInfoCfg::Instance().GetMapCityBuff(vecBuff);
				for(int i = 0; i < pBuffData->randombuffs_size(); i++){
					int nRandom = rand()%vecBuff.size();
					DB_C_RandomBuff * pBuff = pBuffData->mutable_randombuffs(i);
					if(pBuff){
						pBuff->set_activetm(int(nTime));
						pBuff->set_buffid(vecBuff[nRandom]);
						vecBuff.erase(vecBuff.begin()+nRandom);
					}
				}
				markCountryDirty(&m_earthCountry);

				Event evt;
				evt.set_cmd(EVENT_CITYBUFF_INFO);
				evt.set_state(Status_Country_To_Game);
				evt.set_time(0);
				RseCityBuffInfo *pRse = evt.mutable_se_rsecitybuffinfo();
				if(pRse){
					pRse->set_region(GetRegion());
					FillCityBuffInfo(pRse);
					eh_->sendEventToAllGamed(&evt);
				}
			//}
			SYS_LOG(0, LT_CITYBUFF_FLAGSTATE_CHANGE, GetRegion(), 0, "flag:"<<nFlag<<"state:"<<nState);
		}
		break;
	case E_CITYBUFF_STATE_INVALID:
		{
			//if(E_CITYBUFF_STATE_ACTIVE == pBuffData->state()){
				pBuffData->set_flag(nFlag);
				pBuffData->set_state(nState);

				for(int i = 0; i < pBuffData->randombuffs_size(); i++){
					DB_C_RandomBuff *pBuff = pBuffData->mutable_randombuffs(i);
					if(pBuff){
						pBuff->set_activetm(0);
						pBuff->set_buffid(0);
					}
				}
				markCountryDirty(&m_earthCountry);

				Event evt;
				evt.set_cmd(EVENT_CITYBUFF_INFO);
				evt.set_state(Status_Country_To_Game);
				evt.set_time(0);
				RseCityBuffInfo *pRse = evt.mutable_se_rsecitybuffinfo();
				if(pRse){
					pRse->set_region(GetRegion());
					FillCityBuffInfo(pRse);
					eh_->sendEventToAllGamed(&evt);
				}
			//}
			SYS_LOG(0, LT_CITYBUFF_FLAGSTATE_CHANGE, GetRegion(), 0, "flag:"<<nFlag<<"state:"<<nState);
		}
		break;
	}
}

void CountryRgnDatHandler::FillCityBuffInfo(RseCityBuffInfo* rse)
{
	DB_C_CityBuffData *pData = GetCityBuffData();
	if(pData){
		rse->set_region(GetRegion());
		rse->set_flag(pData->flag());
		rse->set_state(pData->state());
		for(int i = 0; i < pData->randombuffs_size(); i++){
			RseRandomBuff * buff = rse->add_randombuffs();
			if(buff){
				buff->set_cityid(pData->randombuffs(i).cityid());
				buff->set_buffid(pData->randombuffs(i).buffid());
				buff->set_activetm(pData->randombuffs(i).activetm());
			}
		}
	}
}

void CountryRgnDatHandler::CheckLeagueBattle(CountryEventHandler* eh_)
{
	static int nOneHour = 1200;
	time_t ltNow = time(NULL);
	DB_C_CountryLeagueBattle* pDBCountryLeagueBattle = m_earthCountry.GetDBCountryLeagueBattle();
	if ( pDBCountryLeagueBattle == NULL )
	{
		return;
	}

	const LeagueBattleActiveTbl* pTbl = LeagueBattleCfg::Instance().GetActiveTbl(ltNow);
	if ( pTbl == NULL )
	{
		return;
	}

	if ( pTbl->m_nID != pDBCountryLeagueBattle->activeflag() )
	{
		vector<CCountry*> lstCountry;
		for ( int i = C_Start; i < C_UserCoutryEnd; i++ )
		{
			CCountry* pCountry = GetCountry(i);
			if ( pCountry == NULL )
			{
				return;
			}

			lstCountry.push_back( pCountry );
		}

		for ( int i = LEAGUEBTLCNTSTART; i <= LEAGUEBTLCNTEND; i++ )
		{
			CCountryLeague* pLeague = GetCountryLeague(i);
			if ( pLeague != NULL )
			{
				pLeague->ClearAllMsg();
			}
		}
		
		sort( lstCountry.begin(), lstCountry.end(), compareLeagueCountry );

		for ( int i = 0; i < lstCountry.size(); i++ )
		{
			DB_C_CountryLeagueMsg* pDBLeagueMsg = NULL;
			CCountry* pCountry = lstCountry[i];
			if ( i == 0 || i == 3 || i == 5 )
			{
				pDBLeagueMsg = GetDBCountryLeague(LEAGUEBTLCNTSTART);
			}
			else
			{
				pDBLeagueMsg = GetDBCountryLeague(LEAGUEBTLCNTEND);
			}
			if ( pDBLeagueMsg == NULL )
			{
				continue;
			}

			pDBLeagueMsg->add_countryid( pCountry->GetCountryID() );
		}

		pDBCountryLeagueBattle->set_winid(0);
		pDBCountryLeagueBattle->set_activestate( LeagueBtlType_PreTime );
		pDBCountryLeagueBattle->set_activeflag( pTbl->m_nID );
		m_ltLeagueCheck = ltNow;

		for ( int i = C_Start; i < C_UserCoutryEnd; i++ )
		{
			//清除繁荣度累计
			map<int,CCountry*>::iterator iter =	m_mapCountry.find(i);
			if ( iter == m_mapCountry.end() || iter->second == NULL )
			{//数据错误 
				continue;
			}

			DB_C_CountryLeagueBattle* pDBCountryLeagueBattle = iter->second->GetDBCountryLeagueBattle();
			if ( pDBCountryLeagueBattle != NULL )
			{
				pDBCountryLeagueBattle->set_boom( 0 );
			}
		}

		SYS_LOG(0,LT_LEAGUEBATTLE_REFRESH,m_nRegion,0,pTbl->m_nID);
		markCountryDirty(&m_earthCountry);
		return;
	}

	int nState = LeagueBattleCfg::Instance().GetActiveState(pTbl,ltNow);
	if ( ltNow - m_ltLeagueCheck > nOneHour )
	{
		if ( nState < LeagueBtlType_Over )
		{
			for ( int i = LEAGUEBTLCNTSTART; i <= LEAGUEBTLCNTEND; i++ )
			{
				CCountryLeague* pLeague = GetCountryLeague(i);
				if ( pLeague != NULL )
				{
					pLeague->SortGuild();
					pLeague->SortPlayer( this );
				}
			}
			m_ltLeagueCheck = ltNow;
			markCountryDirty(&m_earthCountry);
		}
	}

	if ( pDBCountryLeagueBattle->activestate() != nState )
	{
		if ( nState == LeagueBtlType_Over )
		{
			//计算胜利方
			map<int,CCountryLeague*>::iterator iter;
			
			int nMaxBoom = 0;
			int nWinID = 0;

			for ( iter = m_mapLeagueBattle.begin(); iter != m_mapLeagueBattle.end(); iter++ )
			{
				CCountryLeague* pCountryLeague = iter->second;
				if ( pCountryLeague == NULL )
				{
					continue;
				}

				DB_C_CountryLeagueMsg* pDBCountryLeague = pCountryLeague->m_pDBCountryLeague;
				if ( pDBCountryLeague == NULL  )
				{
					continue;
				}

				pCountryLeague->SortGuild();
				pCountryLeague->SortPlayer( this );

				int nBoom = 0;
				for ( int i = 0; i < pDBCountryLeague->countryid_size(); i++ )
				{
					CCountry* pCountry = GetCountry( pDBCountryLeague->countryid(i) );
					nBoom += pCountry->m_nBoom;
					pDBCountryLeague->add_boomrecord( pCountry->m_nBoom );
				}
				
				if ( nBoom > nMaxBoom )
				{
					nMaxBoom = nBoom;
					nWinID = pCountryLeague->m_nID;
				}
				else if ( nBoom == nMaxBoom && nMaxBoom > 0 )
				{
					//平局
					nWinID = LEAGUEBTLDRAW;
				}

				SYS_LOG(0,LT_LEAGUEBATTLE_DEBUG,m_nRegion,0,"LeagueBoomCnt"<<pCountryLeague->m_nID<<nBoom);
			}

			pDBCountryLeagueBattle->set_winid( nWinID );

			SYS_LOG(0,LT_LEAGUEBATTLE_RESULT,m_nRegion,0,nWinID);
		}
		pDBCountryLeagueBattle->set_activestate( nState );
		markCountryDirty(&m_earthCountry);
	}
}

int CountryRgnDatHandler::GetLeagueID( int nCountryID )
{
	for ( int i = LEAGUEBTLCNTSTART; i <= LEAGUEBTLCNTEND; i++ )
	{
		DB_C_CountryLeagueMsg* pDBLeagueMsg = GetDBCountryLeague(i);
		if ( pDBLeagueMsg != NULL )
		{
			for ( int j = 0; j < pDBLeagueMsg->countryid_size(); j++ )
			{
				if ( pDBLeagueMsg->countryid(j) == nCountryID )
				{
					return i;
				}
			}
		}
	}
	return 0;
}

CCountryLeague*	CountryRgnDatHandler::GetCountryLeague( DB_C_User* pDBCUser )
{
	if ( pDBCUser == NULL )
	{
		return NULL;
	}

	int nLeagueID = GetLeagueID( pDBCUser->countryid() );

	return GetCountryLeague( nLeagueID );
}

CCountryLeague*	CountryRgnDatHandler::GetCountryLeague( CGuild* pGuild )
{
	if ( pGuild == NULL )
	{
		return NULL;
	}

	int nLeagueID = GetLeagueID( pGuild->GetCountry() );

	return GetCountryLeague( nLeagueID );
}

void CountryRgnDatHandler::CheckLeagueMsg( DB_C_User* pDBCUser )
{
	if ( pDBCUser == NULL )
	{
		return;
	}

	CCountryLeague* pCountryLeague = GetCountryLeague(pDBCUser);
	if ( pCountryLeague == NULL )
	{
		return;
	}

	for ( int i = 0; i < pCountryLeague->m_lstUserRank.size(); i++ )
	{
		DB_C_User* pDBTmpUser = pCountryLeague->m_lstUserRank[i];
		if ( pDBTmpUser == pDBCUser )
		{
			return;
		}
	}

	if ( pCountryLeague->m_lstUserRank.size() < MAX_LEAGUE_RANK_NUM )
	{
		//人数不足十人
		pCountryLeague->m_lstUserRank.push_back( pDBCUser );
		return;
	}

// 	int nMinSearchTime = 0; 
// 	if ( m_lstWpcRank.size() > 0 )
// 	{
// 		DB_C_User* pDBTmpUser = pCountryLeague->m_lstUserRank[m_lstWpcRank.size() - 1];
// 		if ( pDBTmpUser != NULL )
// 		{
// 			nMinSearchTime = pDBTmpUser->wpcsearchtime();
// 		}
// 	}
// 	if ( pDBCUser->wpcsearchtime() > nMinSearchTime )
// 	{
// 		
// 	}

	pCountryLeague->m_lstUserRank.push_back( pDBCUser );
}

void CountryRgnDatHandler::CheckLeagueMsg( CGuild* pGuild )
{
	if ( pGuild == NULL )
	{
		return;
	}

	CCountryLeague* pCountryLeague = GetCountryLeague(pGuild);
	if ( pCountryLeague == NULL )
	{
		return;
	}

	for ( int i = 0; i < pCountryLeague->m_lstGuildRank.size(); i++ )
	{
		CGuild* pTmpGuild = pCountryLeague->m_lstGuildRank[i];
		if ( pTmpGuild == pGuild )
		{
			return;
		}
	}
	pCountryLeague->m_lstGuildRank.push_back(pGuild);
}

bool CountryRgnDatHandler::CheckSameLeague(DB_C_User* pDBCUser1,DB_C_User* pDBCUser2)
{
	if ( pDBCUser1 == NULL || pDBCUser2 == NULL )
	{
		return false;
	}

	int nLeagueID1 = GetLeagueID( pDBCUser1->countryid() );
	int nLeagueID2 = GetLeagueID( pDBCUser2->countryid() );
	if ( nLeagueID1 == nLeagueID2 )
	{
		return true;
	}

	return false;
}

bool CountryRgnDatHandler::CheckSameLeague(DB_C_User* pDBCUser1,CCity* pCity)
{
	if ( pDBCUser1 == NULL || pCity == NULL )
	{
		return false;
	}

	int nLeagueID1 = GetLeagueID( pDBCUser1->countryid() );
	int nLeagueID2 = GetLeagueID( pCity->GetCountry() );
	if ( nLeagueID1 == nLeagueID2 )
	{
		return true;
	}

	return false;
}

void CCountry::AddChatMsg(int64 uid , int country , string name , string msg , int type)
{
	bool bNew = m_dbCountry.has_chatmsg();
	DB_C_ChatMsg* pDBMsg = m_dbCountry.mutable_chatmsg();
	if (NULL == pDBMsg)
	{
		return;
	}
	if (!bNew)
	{
		pDBMsg->Clear();
	}
	DB_C_ChatMsgOne* pOneMsg = NULL;
	//if (pDBMsg->num() < MAX_COUNTRY_CHAT_MSGNUM)//尽量用msg_size(),避免存储异常,下同
	if(pDBMsg->msg_size() < MAX_COUNTRY_CHAT_MSGNUM)
	{
		pOneMsg = pDBMsg->add_msg();
		pDBMsg->set_end(pDBMsg->end() + 1);
		//pDBMsg->set_num(pDBMsg->num() + 1);
	}
	else
	{
		if (pDBMsg->end() >= MAX_COUNTRY_CHAT_MSGNUM)
		{
			pDBMsg->set_end(pDBMsg->end() % MAX_COUNTRY_CHAT_MSGNUM);
		}
		pOneMsg = pDBMsg->mutable_msg(pDBMsg->end());
		pOneMsg->Clear();
		pDBMsg->set_end(pDBMsg->end() + 1);

	}
	if (pOneMsg != NULL)
	{
		pOneMsg->set_uid(uid);
		pOneMsg->set_type(type);
		pOneMsg->set_country(country);
		pOneMsg->set_name(name);
		pOneMsg->set_msg(msg);
	}
}

void CCountry::GetChatMsg(RseCountryChatMsg* pRse)
{
	if (NULL == pRse)
	{
		return;
	}
	if (!m_dbCountry.has_chatmsg())
	{
		pRse->set_ret(CCOR_NOMSG);
		return;
	}
	else
	{
		//return m_dbCountry.mutable_chatmsg();
		DB_C_ChatMsg* pDBMsg = m_dbCountry.mutable_chatmsg();
		if (NULL == pDBMsg)
		{
			return;
		}
		//if (pDBMsg->num() >= MAX_COUNTRY_CHAT_MSGNUM)
		if(pDBMsg->msg_size() >= MAX_COUNTRY_CHAT_MSGNUM)
		{
			//for (int i = pDBMsg->end() ; i < pDBMsg->num() ; i++)
			for(int i = pDBMsg->end() ; i < pDBMsg->msg_size() ; i++)
			{
				MsgData* pOutMsg = pRse->add_chatmsg();
				DB_C_ChatMsgOne* pOneMsg = pDBMsg->mutable_msg(i);
				stringstream tmp;
				tmp<<pOneMsg->uid();
				pOutMsg->set_uid(tmp.str());
				pOutMsg->set_countryid(pOneMsg->country());
				pOutMsg->set_name(pOneMsg->name());
				pOutMsg->set_msg(pOneMsg->msg());
				pOutMsg->set_type(pOneMsg->type());
			}
		}
		//for (int i = 0 ; i < pDBMsg->end() ; i++)
		int nEnd = pDBMsg->end() < pDBMsg->msg_size() ? pDBMsg->end() : pDBMsg->msg_size();
		for(int i = 0 ; i < nEnd ; i++)
		{
			MsgData* pOutMsg = pRse->add_chatmsg();
			DB_C_ChatMsgOne* pOneMsg = pDBMsg->mutable_msg(i);
			stringstream tmp;
			tmp<<pOneMsg->uid();
			pOutMsg->set_uid(tmp.str());
			pOutMsg->set_countryid(pOneMsg->country());
			pOutMsg->set_name(pOneMsg->name());
			pOutMsg->set_msg(pOneMsg->msg());
			pOutMsg->set_type(pOneMsg->type());
		}
	}
}

::google::protobuf::RepeatedField< ::google::protobuf::int32 >* CCountryLeague::GetLeagueCountry()
{
	return m_pDBCountryLeague->mutable_countryid();
}

void CountryRgnDatHandler::CheckBossBtlMsg( CountryEventHandler* eh_, int nRegion, time_t ltNow )
{
	if ( eh_ == NULL )
	{
		return;
	}
	static int nTwoMin = 120;
	static int nTyS = 10;

	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL )
	{ 
		return;
	}
    int nBossBtlFlag = BossBtlCfg::Instance().GetBossBtlFlag();
    if (nBossBtlFlag <= 0)
    {
        return;
    }
    const BossBtlBaseTbl *pTbl = BossBtlCfg::Instance().GetBaseTbl(nBossBtlFlag);
    if (pTbl == NULL)
    {
        return;
    }
    if (nBossBtlFlag != pDBBossMsg->nbossbtlflag())
    {
        pDBBossMsg->set_nbossbtlflag(nBossBtlFlag);
        pDBBossMsg->set_state(E_BOSSBTLSTATE_PREPARE);
        //清除战斗记录,排行榜
        ClearAllRankList(pDBBossMsg);
        //更换boss
        ChangeBossInfo(pTbl->m_nBossID, nRegion);

        markBossBtlInfoDirty();
    }
    else
    {
        if ( ltNow == 0 )
        {
            ltNow = time(NULL);
        }
        int nState = pTbl->GetBossBtlState(ltNow);

        if (E_BOSSBTLSTATE_BATTLE == nState)
        {//战斗期间定时刷新排行榜和战斗信息
            if ( ltNow - m_ltBossCheck >= nTwoMin )
            {//伤害排行
                SortBossBtl();
                markBossBtlInfoDirty();
                m_ltBossCheck = ltNow;
            }
            if (ltNow - m_ltBossLiteCheck >= nTyS)
            {//发送战斗信息
                eh_->SendBossBtlLiteInfo();
                markBossBtlInfoDirty();
                m_ltBossLiteCheck = ltNow;
            }
        }

        if ( nState > pDBBossMsg->state() )
        {
            if ( nState >= E_BOSSBTLSTATE_BATTLE && pDBBossMsg->state() < E_BOSSBTLSTATE_BATTLE )
            {
                m_ltBossCheck = ltNow;
                m_ltBossLiteCheck = ltNow;
                pDBBossMsg->set_state( E_BOSSBTLSTATE_BATTLE );
            }
            if ( nState >= E_BOSSBTLSTATE_END && pDBBossMsg->state() < E_BOSSBTLSTATE_END )
            {
                //抽取幸运玩家
                SortBossBtl();
                eh_->SendBossBtlLiteInfo();
                ExtractLuckPlayer(pDBBossMsg);

                pDBBossMsg->set_state( E_BOSSBTLSTATE_END );

                SYS_STAT(0,"BossBattle",0,0,false,0,0,0,0,0,GetRegion(),0,0,false,"BossHP", "","",(pDBBossMsg->fightingbossmsg().hp() * 100/pDBBossMsg->fightingbossmsg().maxhp()));
            }
            if ( nState >= E_BOSSBTLSTATE_OVER && pDBBossMsg->state() < E_BOSSBTLSTATE_OVER )
            {
                pDBBossMsg->set_state( E_BOSSBTLSTATE_OVER );
                //重新排列最高伤害玩家
                UpdateHighPointList(pDBBossMsg);
            }
            if (nState >= E_BOSSBTLSTATE_UPOVER && pDBBossMsg->state() < E_BOSSBTLSTATE_UPOVER)
            {
                pDBBossMsg->set_state( E_BOSSBTLSTATE_UPOVER );
                for (int i = 0; i < 3 && i < pDBBossMsg->uplist_size(); i++)
                {
                    SYS_STAT(0,"BossBattle",0,0,false,0,0,0,0,0,GetRegion(),0,0,false,"PraiseNum", "","",(i+1),pDBBossMsg->uplist(i).count());
                }
                ClearAllRankList(pDBBossMsg);
            }
            markBossBtlInfoDirty();
        }
    }
}

void CountryRgnDatHandler::ChangeBossInfo(int nBossID, int nRegion)
{
    const BossBtlNpcTbl* pNpcTbl = BossBtlCfg::Instance().GetBtlTypeTbl( nBossID );
    if ( pNpcTbl != NULL )
    {
        DB_C_BossMsg* pDBBoss = GetDBBossMsg();
        if ( pDBBoss == NULL )
        {
            return;
        }

        if( pDBBoss->hp() <= 0 && pDBBoss->bossid() > 0 )
        {
            //升级
            DB_C_BossInfo* pDBOldBossInfo = GetDBBossInfo(pDBBoss->bossid());
            if ( pDBOldBossInfo != NULL && pDBOldBossInfo->bosslv() < BossBtlCfg::Instance().GetMaxLV())
            {
                pDBOldBossInfo->set_bosslv( pDBOldBossInfo->bosslv() + 1 );
            }
        }
        DB_C_BossInfo* pDBBossInfo = GetDBBossInfo(pNpcTbl->m_nId);
        if ( pDBBossInfo == NULL )
        {
            return;
        }

        const BossBtlLvTbl* pLvTbl = BossBtlCfg::Instance().GetLvTbl(pDBBossInfo->bosslv());
        const BossBtlRegionTbl *pRegionTbl = BossBtlCfg::Instance().GetBtlRegionTbl(nRegion + 1);
        int64 nAdd = 100;
        if ( pLvTbl != NULL )
        {
            nAdd += (int64)pLvTbl->m_nAddHp;
        }

        if (pRegionTbl != NULL)
        {
            nAdd += (int64)pRegionTbl->m_nAddHp;
        }

        int64 nMaxHp = ((int64)pNpcTbl->m_nBossHp) * nAdd / 100;
        pDBBoss->set_bossid( pDBBossInfo->bossid() );
        pDBBoss->set_hp(nMaxHp);
        pDBBoss->set_maxhp(nMaxHp);
    }
}

void CountryRgnDatHandler::ClearAllRankList(DB_C_BossBtlMsg* pDBBossMsg)
{
    CheckUpRank();

    list<GCG_BossBtlLiteUnit*>::iterator iter;
    while(m_lstBossBtlRecord.size() > 0)
    {
        iter = m_lstBossBtlRecord.begin();
        delete *iter;
        m_lstBossBtlRecord.erase(iter);
    }
    m_lstBossBtlRecord.clear();

    m_nBossBtlTick = 0;

    for( int i = 0; i < BOSSBTL_UP_CNT; i++ )
    {	
        m_lstUpList[i].clear();
    }

    m_lstBossBtlRank.clear();

    pDBBossMsg->clear_killer();
    pDBBossMsg->clear_luckylist();
    pDBBossMsg->clear_lastranklist();

    pDBBossMsg->clear_ranklist();
}

void CountryRgnDatHandler::ExtractLuckPlayer(DB_C_BossBtlMsg* pDBBossMsg)
{
    pDBBossMsg->clear_luckylist();

    SortBossBtl();

    int nRankSize = m_lstBossBtlRank.size();
    int64 userList[10];
    memset( userList, 0, sizeof(int64)*10 );

    int nCnt = 0;
    for ( int i = 0; i < 2; )
    {
        nCnt++;
        if ( nCnt > 100 )
        {
            break;
        }
        int nRand = 0;
        DB_C_User* pLuckUser = NULL;
        if ( nRankSize <= 0 )
        {

        }
        else if( nRankSize <= 200 )
        {
            //完全随机幸运玩家
            nRand = rand()%nRankSize;
            pLuckUser = m_lstBossBtlRank[ nRand ];
        }

        else
        {
            //从100名之后挑选
            nRand = rand()%(nRankSize - 100);
            pLuckUser = m_lstBossBtlRank[ nRand + 100 ];
        }

        if ( pLuckUser == NULL )
        {
            continue;
        }

        bool bFound = false;
        for ( int j = 0; j < 10 && j < i; j++ )
        {
            if ( userList[j] == pLuckUser->userid() )
            {
                bFound = true;
                break;
            }
        }

        if ( bFound )
        {
            continue;
        }

        DB_C_BossBtlRankUnit* pLuckyUnit = pDBBossMsg->add_luckylist();
        SaveUnit(pLuckyUnit,pLuckUser);
        userList[i] = pLuckUser->userid();
        i++;
    }
}

void CountryRgnDatHandler::UpdateHighPointList(DB_C_BossBtlMsg* pDBBossMsg)
{
    vector<DB_C_BossBtlRankUnit> lstHighPoint;
    for ( int i = 0; i < pDBBossMsg->highpointlist_size(); i++ )
    {
        DB_C_BossBtlRankUnit unit;
        unit.CopyFrom(pDBBossMsg->highpointlist(i));
        lstHighPoint.push_back(unit);
    }

    for ( int i = 0; i < pDBBossMsg->ranklist_size() && i < BOSSBTL_MAXRANK_CNT; i++ )
    {
        DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->mutable_ranklist(i);

        bool bFound = false;
        for ( int j = 0; j < lstHighPoint.size(); j++ )
        {
            DB_C_BossBtlRankUnit &unitTmp = lstHighPoint[j];
            if ( unitTmp.uid() == pUnit->uid() )
            {
                if ( pUnit->dmg() > unitTmp.dmg())
                {
                    unitTmp.set_dmg(pUnit->dmg());
                }

                bFound = true;
                break;
            }
        }

        if ( !bFound && pUnit->dmg() > 0)
        {
            DB_C_BossBtlRankUnit unit;
            unit.CopyFrom(*pUnit);
            lstHighPoint.push_back(unit);
        }
    }

    sort( lstHighPoint.begin(), lstHighPoint.end(), compareBossBtlHighRank );

    pDBBossMsg->clear_highpointlist();

    for ( int i = 0; i < lstHighPoint.size() && i < BOSSBTL_MAXRANK_CNT; i++ )
    {
        DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->add_highpointlist();
        pUnit->CopyFrom(lstHighPoint[i]);
    }
}

DB_C_BossInfo* CountryRgnDatHandler::GetDBBossInfo( int nId )
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL )
	{ 
		return NULL;
	}

	DB_C_BossInfo* pDBInfo = NULL;
	for ( int i = 0; i < pDBBossMsg->bossinfo_size(); i++ )
	{
		pDBInfo = pDBBossMsg->mutable_bossinfo(i);
		if ( pDBInfo->bossid() == nId )
		{
			break;
		}
		pDBInfo = NULL;
	}

	if ( pDBInfo == NULL )
	{
		pDBInfo = pDBBossMsg->add_bossinfo();
		pDBInfo->set_bossid( nId );
		pDBInfo->set_bosslv(1);
	}
	return pDBInfo;
}

DB_C_BossMsg* CountryRgnDatHandler::GetDBBossMsg()
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL )
	{ 
		return NULL;
	}

	return pDBBossMsg->mutable_fightingbossmsg();
}

void CountryRgnDatHandler::FillBossBtlMsg(RseBossOpt *pRse)
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL || pRse == NULL )
	{ 
		return;
	}

	DB_C_BossMsg* pDBBoss = GetDBBossMsg();
	if ( pDBBoss == NULL )
	{
		return;
	}

    if ( pDBBoss->bossid() != 0 )
    {
        DB_C_BossInfo* pDBBossLVInfo = NULL;
        pDBBossLVInfo = GetDBBossInfo(pDBBoss->bossid());
        if ( pDBBossLVInfo != NULL )
        {
            pRse->set_bosslevel( pDBBossLVInfo->bosslv() );
        }
    }


	pRse->set_bossid(pDBBoss->bossid());
	pRse->set_bosshp(toString(pDBBoss->hp()));
	if (pDBBoss->hp() == 0)
	{
		pRse->set_bkill(true);
	}

    if (pDBBossMsg->state() == E_BOSSBTLSTATE_BATTLE)
    {
        pRse->set_bopen(true);
    }

	FillBossBtlRankList(pRse,BOSSBTL_RANK_CNT_BASE);

	for ( int i = 0; i < pDBBossMsg->luckylist_size(); i++ )
	{
		DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_luckylist(i);
		BossBattlePlayerUnit* pUnit = pRse->add_luckyunit();
		FillBossBtlPlayerMsg( pUnit, pRankUnit );
	}

	DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_killer();
	BossBattlePlayerUnit* pUnit = pRse->mutable_killerunit();
	FillBossBtlPlayerMsg( pUnit, pRankUnit );
}

void CountryRgnDatHandler::FillBossBtlFightMsg(RseBossOpt *pRse)
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL || pRse == NULL )
	{ 
		return;
	}
	DB_C_BossMsg* pDBBoss = GetDBBossMsg();
	if ( pDBBoss == NULL )
	{
		return;
	}

	pRse->set_bossid(pDBBoss->bossid());
	pRse->set_bosshp(toString(pDBBoss->hp()));
	if (pDBBoss->hp() == 0)
	{
		pRse->set_bkill(true);
	}
	
	DB_C_BossInfo* pDBBossInfo = NULL;
	if ( pDBBoss->bossid() != 0 )
	{
		pDBBossInfo = GetDBBossInfo(pDBBoss->bossid());
		if ( pDBBossInfo != NULL )
		{
			pRse->set_bosslevel( pDBBossInfo->bosslv() );
		}
	}

	FillBossBtlRankList(pRse,BOSSBTL_RANK_CNT_BTL);

	pRse->set_perhp(pDBBoss->hp()*100/pDBBoss->maxhp());

}

void CountryRgnDatHandler::FillBossBtlRankList(RseBossOpt *pRse,int nCnt,int nType )
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL || pRse == NULL )
	{ 
		return;
	}

	if ( nType == BOT_SeeRank )
	{
// 		if ( nCnt == 100 )
// 		{
// 			for ( int j = 0; j < 25; j++ )
// 			{
// 				for ( int i = 0; i < pDBBossMsg->ranklist_size() && i < nCnt; i++ )
// 				{
// 					DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_ranklist(i);
// 
// 					BossBattlePlayerUnit* pUnit = pRse->add_playerrankunit();
// 					FillBossBtlPlayerMsg( pUnit, pRankUnit );
// 				}
// 			}
// 		}
// 		else
// 		{
// 			
// 		}
		for ( int i = 0; i < pDBBossMsg->ranklist_size() && i < nCnt; i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_ranklist(i);

			BossBattlePlayerUnit* pUnit = pRse->add_playerrankunit();
			FillBossBtlPlayerMsg( pUnit, pRankUnit );
		}
	
	}

	if ( nType == BOT_SeeHighRank )
	{
		for ( int i = 0; i < pDBBossMsg->highpointlist_size() && i < nCnt; i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_highpointlist(i);

			BossBattlePlayerUnit* pUnit = pRse->add_playerrankunit();
			FillBossBtlPlayerMsg( pUnit, pRankUnit );
		}
	}

	if ( nType == BOT_SeeUpRank )
	{
		for ( int i = 0; i < pDBBossMsg->uplist_size() && i < nCnt; i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_uplist(i);
            if (pRankUnit->count() <= 0)
            {
                continue;
            }

			BossBattlePlayerUnit* pUnit = pRse->add_playerrankunit();
			FillBossBtlPlayerMsg( pUnit, pRankUnit );
		}
	}

}

void CountryRgnDatHandler::FillBossBtlPlayerMsg( BossBattlePlayerUnit* pUnit, DB_C_BossBtlRankUnit* pRankUnit )
{
	if ( pUnit == NULL || pRankUnit == NULL )
	{
		return;
	}
	else
	{
		pUnit->set_uid( toString(pRankUnit->uid()) );
		pUnit->set_name(pRankUnit->name());

		pUnit->set_power(pRankUnit->power());
		pUnit->set_praisecont(pRankUnit->count());
		pUnit->set_declaration(pRankUnit->words());
		pUnit->set_damage(toString(pRankUnit->dmg()));
		pUnit->set_countryid(pRankUnit->countryid());
		pUnit->set_guildname(pRankUnit->guildname());
		pUnit->set_level(pRankUnit->level());
		pUnit->set_praisecont(pRankUnit->count());
		pUnit->set_pic(pRankUnit->pic());

		for ( int i = 0; i < pRankUnit->reports_size(); i++ )
		{
			DB_C_Report* pDBReport = pRankUnit->mutable_reports(i);
			if ( pDBReport->count() > BOSSBTL_MAX_REPORT_CNT )
			{
				pUnit->set_reason(pDBReport->type());
				break;
			}
		}
	}
}

int CountryRgnDatHandler::GetBossBtlRank( int64 uid,int nType )
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL )
	{ 
		return 0;
	}

	if ( nType == BOT_SeeRank )
	{
		for ( int i = 0; i < pDBBossMsg->ranklist_size(); i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_ranklist(i);
			if ( pRankUnit->uid() == uid )
			{
				return i+1;
			}
		}

        return (BOSSBTL_RANK_CNT_ALL + 1);
	}

	else if ( nType == BOT_SeeHighRank )
	{
		for ( int i = 0; i < pDBBossMsg->highpointlist_size(); i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_highpointlist(i);
			if ( pRankUnit->uid() == uid )
			{
				return i+1;
			}
		}
	}

	else if ( nType == BOT_SeeUpRank )
	{
		for ( int i = 0; i < pDBBossMsg->uplist_size(); i++ )
		{
			DB_C_BossBtlRankUnit* pRankUnit = pDBBossMsg->mutable_uplist(i);
			if ( pRankUnit->uid() == uid )
			{
				return i+1;
			}
		}
	}

	return 0;
}

void CountryRgnDatHandler::CheckBossBtlDmg( DB_C_User* pDBCUser )
{
	if ( pDBCUser == NULL )
	{
		return;
	}

	for ( int i = 0; i < m_lstBossBtlRank.size(); i++ )
	{
		DB_C_User* pDBTmpUser = m_lstBossBtlRank[i];
		if ( pDBTmpUser == pDBCUser )
		{
			return;
		}
	}

	if ( m_lstBossBtlRank.size() < BOSSBTL_RANK_CNT_ALL )
	{
		//人数不足一百人
		m_lstBossBtlRank.push_back( pDBCUser );
		return;
	}

	m_lstBossBtlRank.push_back( pDBCUser );
}


void CountryRgnDatHandler::BossBtlPlayerUp( int nRank, DB_C_User* pUser, DB_C_BossBtlRankUnit* pDBUnit)
{
	if ( nRank >= BOSSBTL_UP_CNT || nRank < 0 || pDBUnit == NULL)
	{
		return;
	}

	bool bReset = false;

	m_lstUpList[nRank].push_back( pUser->name() );

	if ( m_lstUpList[nRank].size() > BOSSBTL_UP_MAX )
	{
		m_lstUpList[nRank].pop_front();
		bReset = true;
	}

	if ( bReset )
	{
		pDBUnit->clear_namelist();
		for ( list<string>::iterator iter = m_lstUpList[nRank].begin(); iter != m_lstUpList[nRank].end(); iter++ )
		{
			string strName = *iter;

			pDBUnit->add_namelist(strName);
		}
	}
	
	else
	{
		pDBUnit->add_namelist(pUser->name());
	}

}

void CountryRgnDatHandler::SortBossBtl()
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg();
	if ( pDBBossMsg == NULL )
	{ 
		return;
	}

    vector<DB_C_BossBtlRankUnit> lstRank;

    for ( int i = 0; i < pDBBossMsg->ranklist_size(); i++ )
    {
        DB_C_BossBtlRankUnit unit;
        unit.CopyFrom(pDBBossMsg->ranklist(i));
        lstRank.push_back(unit);
    }

	pDBBossMsg->clear_ranklist();
	
	sort( m_lstBossBtlRank.begin(), m_lstBossBtlRank.end(), compareBossBtlRank );
	for ( int i = 0; i < m_lstBossBtlRank.size() && i < BOSSBTL_RANK_CNT_ALL; i++ )
	{
		DB_C_User* pUser = m_lstBossBtlRank[i];

		if ( pUser == NULL )
		{
			continue;
		}
        DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->add_ranklist();
        for (vector<DB_C_BossBtlRankUnit>::iterator iter = lstRank.begin(); iter != lstRank.end(); iter++)
        {
            if (pUser->userid() == iter->uid())
            {
                pUnit->CopyFrom(*iter);
                lstRank.erase(iter);
                break;
            }
        }
		pUnit->set_uid(pUser->userid());
		pUnit->set_name(pUser->name());
		pUnit->set_pic(pUser->pic());
		pUnit->set_countryid(pUser->countryid());

		pUnit->set_level(pUser->level());
		pUnit->set_dmg(pUser->bossbtldmg());
		pUnit->set_power( GetWpcFightPoint( toString( pUser->userid()) ) );

		if ( pUser->guildid() > 0 )
		{
			CGuild* pGuild = GetGuild( pUser->guildid() );
			if ( pGuild != NULL )
			{
				pUnit->set_guildname( pGuild->GetGuildName() );
			}
		}
		
	}
}

void CountryRgnDatHandler::FillBossBtlLite( GCG_BossBtlLite* pLite )
{
	list<GCG_BossBtlLiteUnit*>::iterator iter;
	int i = 0;
	for ( iter = m_lstBossBtlRecord.begin() ; iter != m_lstBossBtlRecord.end() && i < BOSSBTL_RECORD_MAX; iter++ )
	{
		GCG_BossBtlLiteUnit* pUnit = pLite->add_list();
		GCG_BossBtlLiteUnit* pUnitTmp = *iter;
		pUnit->CopyFrom( (const GCG_BossBtlLiteUnit)*pUnitTmp );

		i++;
	}
}

void CountryRgnDatHandler::AddBossBtlRecord( DB_C_User* pUser, int64 nDmg, int64 nLeft, bool bKill, int nPerHp )
{
	if ( pUser == NULL || nDmg == 0 )
	{
		return;
	}

	GCG_BossBtlLiteUnit* pUnit = NULL;
	if ( m_lstBossBtlRecord.size() < BOSSBTL_RECORD_MAX )
	{
		pUnit = new GCG_BossBtlLiteUnit();
	}

	else
	{
		pUnit = m_lstBossBtlRecord.front();
		m_lstBossBtlRecord.pop_front();
	}

	if ( pUnit == NULL )
	{
		return;
	}

	m_lstBossBtlRecord.push_back(pUnit);
	pUnit->set_uid(toString(pUser->userid()));
	pUnit->set_name(pUser->name());
	pUnit->set_dmg(toString(nDmg));
	pUnit->set_lefthp(toString(nLeft));
	pUnit->set_kill(bKill);
	pUnit->set_tick( m_nBossBtlTick + 1 );
	pUnit->set_perhp(nPerHp);

	if ( bKill )
	{
		//击杀成功，记录击杀人员
		DB_C_BossBtlMsg* pDBBossBtlMsg = GetBossBtlMsg();
		if ( pDBBossBtlMsg != NULL )
		{
			pDBBossBtlMsg->clear_killer();
			pDBBossBtlMsg->clear_luckylist();

			DB_C_BossBtlRankUnit* pKillUnit = pDBBossBtlMsg->mutable_killer();
			SaveUnit(pKillUnit,pUser);

			SortBossBtl();
		}
        markBossBtlInfoDirty();
	}

	m_nBossBtlTick++;
}

void CountryRgnDatHandler::SaveUnit( DB_C_BossBtlRankUnit* pRankUnit, DB_C_User* pUser )
{
	if ( pRankUnit == NULL || pUser == NULL )
	{
		return;
	}
	
	pRankUnit->set_uid( pUser->userid() );

	pRankUnit->set_pic( pUser->pic() );
	pRankUnit->set_name(pUser->name());

	pRankUnit->set_dmg(pUser->bossbtldmg());
	pRankUnit->set_countryid(pUser->countryid());
	pRankUnit->set_level(pUser->level());

	CGuild* pGuild = GetGuild(pUser->guildid());
	if ( pGuild != NULL )
	{
		pRankUnit->set_guildname(pGuild->GetGuildName());
	}
	
}

void CountryRgnDatHandler::CheckUpRank()
{
	DB_C_BossBtlMsg* pDBBossMsg = GetBossBtlMsg(); 
	if ( pDBBossMsg == NULL )
	{
		return;
	}

	vector<DB_C_BossBtlRankUnit> lstUp;
	
	for ( int i = 0; i < pDBBossMsg->uplist_size(); i++ )
	{
		DB_C_BossBtlRankUnit unit;
		unit.CopyFrom(pDBBossMsg->uplist(i));
		lstUp.push_back(unit);
	}

	for ( int i = 0; i < pDBBossMsg->ranklist_size() && i < 3; i++ )
	{
		DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->mutable_ranklist(i);
        if (pUnit->count() <= 0)
        {//为零时不计入排行榜
            continue;
        }
		bool bFound = false;
		for ( int j = 0; j < lstUp.size(); j++ )
		{
			if ( lstUp[j].uid() == pUnit->uid() )
			{
				if ( pUnit->count() > lstUp[j].count())
				{
					lstUp[j].set_count(pUnit->count());
				}

				bFound = true;
				break;
			}
		}

		if ( !bFound && pUnit->count() > 0)
		{
			DB_C_BossBtlRankUnit unit;
			unit.CopyFrom(*pUnit);
			lstUp.push_back(unit);
		}
	}
// 
// 	{
// 		DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->mutable_killer();
// 		bool bFound = false;
// 		for ( int j = 0; j < lstUp.size(); j++ )
// 		{
// 			DB_C_BossBtlRankUnit unitTmp;
// 			unitTmp.CopyFrom(lstUp[j]);
// 			if ( unitTmp.uid() == pUnit->uid() )
// 			{
// 				if ( pUnit->count() > unitTmp.count())
// 				{
// 					unitTmp.set_count(pUnit->count());
// 				}
// 
// 				bFound = true;
// 				break;
// 			}
// 		}
// 
// 		if ( !bFound && pUnit->uid() != 0)
// 		{
// 			DB_C_BossBtlRankUnit unit;
// 			unit.CopyFrom(*pUnit);
// 			lstUp.push_back(unit);
// 		}
// 	}
	

	sort( lstUp.begin(), lstUp.end(), compareBossBtlUpCnt );

	pDBBossMsg->clear_uplist();

	for ( int i = 0; i < lstUp.size() && i < 10; i++ )
	{
        if (lstUp[i].count() <= 0)
        {
            continue;
        }
		DB_C_BossBtlRankUnit* pUnit = pDBBossMsg->add_uplist();
		pUnit->CopyFrom(lstUp[i]);
	}
}

void CountryRgnDatHandler::CheckRedEnvelopeInfo(CountryEventHandler* eh_)
{
    int nFlag = GuildRedEnvelopeCfg::Instance().GetActiveFlag();

    if (nFlag == 0)
    {
        m_GuildRedEnvelope.ClearAllRedEnveLopreInfo(eh_, true);
        return;
    }
    time_t tNow = time(NULL);
    if (nFlag != m_GuildRedEnvelope.GetFlag())
    {
        m_GuildRedEnvelope.ClearAllRedEnveLopreInfo(eh_, true);
        m_GuildRedEnvelope.SetFlag(nFlag);
        m_ltCheckOverDue = tNow;
        m_ltSendRank = tNow;
        m_ltSortRank = tNow;
        return;
    }

    if (tNow - m_ltCheckOverDue >= GuildRedEnvelopeCfg::Instance().GetSortInterval())
    {
        m_GuildRedEnvelope.ClearAllRedEnveLopreInfo(eh_);
        m_ltCheckOverDue = tNow;
    }

    //排名间隔改为配表中配置 2016.1.11
    if (tNow - m_ltSortRank >= GuildRedEnvelopeCfg::Instance().GetSortInterval())
    {
        m_ltSortRank = tNow;
        m_GuildRedEnvelope.SortGuildRank();
        m_GuildRedEnvelope.SortUserRank();
        m_GuildRedEnvelope.SortSendUserRank();
    }
    if (tNow - m_ltSendRank >= REDENEVELOPE_SENDRANK)
    {
        m_ltSendRank = tNow;
        Event evn;
        evn.set_cmd(EVNT_REDENVELOPE_RANK);
        evn.set_state(Status_Country_To_Game);
        evn.set_time(0);
        RseGuildRedEnvelopeOpt *pRse = evn.mutable_se_rseguildredenvelopeopt();
        m_GuildRedEnvelope.FillRankInfo(pRse);
        pRse->set_nregionnum(GetRegion());
        eh_->sendEventToAllGamed(&evn);
    }
}

const int NPCACTIVE_SIZE = 3 ; //目前军团通缉令是 s/a/b 3档npc
int CCountry::AddNpcActiveRecord(int nLv)
{
	int nSize = m_dbCountry.npcactivecnt_size();
	if (nSize < NPCACTIVE_SIZE)
	{
		for(int i = nSize ; i < NPCACTIVE_SIZE ; i++)
		{
			m_dbCountry.add_npcactivecnt(0);
		}
	}
	if (nLv <= 0 || nLv > NPCACTIVE_SIZE)
	{
		return E_ARMYGROUP_ACTIVE_REL_WINLVERR;
	}
	int nCnt = m_dbCountry.npcactivecnt(nLv - 1);
	m_dbCountry.set_npcactivecnt(nLv - 1 , ++nCnt);
	return 0;
}

void CountryRgnDatHandler::CheckNpcActiveRank()
{
	/*if(!NpcActiveCfg::Instance().IsNpcActiveStart())
	{
		return;
	}*/
	CCountry* pEarth = GetEarthCountry();
	if (NULL == pEarth)
	{
		return;
	}
	DB_C_Country* pDbEarth = pEarth->GetDBCountry();
	if (NULL == pDbEarth)
	{
		return;
	}
	DB_C_NpcActiveRank* pDBActRank = pDbEarth->mutable_npcactiverank();
	if (NULL == pDBActRank)
	{
		return;
	}
	time_t nRankLast = (time_t)pDBActRank->ranktime();
	time_t nNow = time(NULL);
	struct tm tLast , tNow , tDate;
	if (localtime_r(&nRankLast , &tLast) == NULL || localtime_r(&nNow , &tNow) == NULL || localtime_r(&nNow , &tDate) == NULL)
	{
		return;
	}
	//先读表判是否开启军团通缉令活动
	tDate.tm_hour = 0;
	tDate.tm_min = 0;
	tDate.tm_sec = 0;
	tDate.tm_isdst = -1;//夏令时,同读表中处理
	time_t nDate = mktime(&tDate);
	NPCActiveTbl* pTbl = NpcActiveCfg::Instance().GetNpcActiveTbl(nDate);
	if (NULL == pTbl)
	{
		return;
	}

	if (tNow.tm_yday != tLast.tm_yday)
	{
		//不同天,清理数据,服务器不会隔年,year day就行了
		for (int i = C_Start ; i < C_UserCoutryEnd ; i++)
		{
			CCountry* pCountry = GetCountry(i);
			if (pCountry == NULL)
			{
				continue;
			}
			DB_C_Country* pDBCountry = pCountry->GetDBCountry();
			if (NULL == pDBCountry)
			{
				continue;
			}
			pDBCountry->clear_npcactivecnt();
			markCountryDirty(pCountry);
		}
		//pDBActRank->set_ranktime((int)nNow);
		//pDBActRank->clear_rank();
		SortNpcActiveRank(nNow);
		//markCountryDirty(pEarth);
		return;
	}
	if (tNow.tm_hour != tLast.tm_hour)
	{
		//每小时排一次
		SortNpcActiveRank(nNow);
	}

}

bool npcactsortfun(DB_C_NpcActiveRankItem item1 , DB_C_NpcActiveRankItem item2)
{
	for (int i = 0 ; i < item1.wincnt_size() ; i++)
	{
		if (item2.wincnt_size() < i + 1)
		{
			return true;
		}
		if (item1.wincnt(i) > item2.wincnt(i))
		{
			return true;
		}
		else if (item1.wincnt(i) == item2.wincnt(i))
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	//有数的都相等??
	/*if(item1.wincnt_size() < item2.wincnt_size())
	{
		return false;
	}
	return true;*/
	return false;
}

void CountryRgnDatHandler::SortNpcActiveRank(time_t nNow)
{
	CCountry* pEarth = GetEarthCountry();
	if (NULL == pEarth)
	{
		return;
	}
	DB_C_Country* pDbEarth = pEarth->GetDBCountry();
	if (NULL == pDbEarth)
	{
		return;
	}
	DB_C_NpcActiveRank* pDbRank = pDbEarth->mutable_npcactiverank();
	if (NULL == pDbRank)
	{
		return;
	}

	vector<DB_C_NpcActiveRankItem> vRankData;
	for (int i = C_Start ; i < C_UserCoutryEnd ; i++)
	{
		CCountry* pCountry = GetCountry(i);
		if (pCountry == NULL)
		{
			return;
		}
		DB_C_Country* pDBCountry = pCountry->GetDBCountry();
		if (NULL == pDBCountry)
		{
			return;
		}
		DB_C_NpcActiveRankItem Item;
		Item.set_country(i);
		for (int i = 0 ; i < pDBCountry->npcactivecnt_size() ; i++)
		{
			Item.add_wincnt(pDBCountry->npcactivecnt(i));
		}
		if (Item.wincnt_size() < NPCACTIVE_SIZE)
		{
			for(int j = pDBCountry->npcactivecnt_size() ; j < NPCACTIVE_SIZE ; j++)
			{
				Item.add_wincnt(0);
			}
		}
		vRankData.push_back(Item);
	}
	sort(vRankData.begin() , vRankData.end() , npcactsortfun);
	
	pDbRank->clear_rank();
	for (vector<DB_C_NpcActiveRankItem>::iterator itr = vRankData.begin() ; itr != vRankData.end() ; itr++)
	{
		DB_C_NpcActiveRankItem* pDbItem = pDbRank->add_rank();
		if (NULL == pDbItem)
		{
			return;
		}
		pDbItem->CopyFrom(*itr);
	}
	pDbRank->set_ranktime((int)nNow);
	markCountryDirty(pEarth);
}

void CountryRgnDatHandler::CheckWpcLabSearchRank()
{
	static int nOneHour = 1*60*60;
	time_t ltNow = time(NULL);

	if(ltNow-m_ltLastUpdateLabSearchRankTime>nOneHour)
	{
		if(NewRegionActivityCfg::Instance().InRightTimeByRegionAndType(m_nRegion,ET_ACTIVITY_ARM_RACE,NG_ACTIVITY_ACTIVE_TIME))
		{
			SortWpcLabSearchRank();
		}
		m_ltLastUpdateLabSearchRankTime = ltNow;
	}


	if(NewRegionActivityCfg::Instance().InRightTimeByRegionAndType(m_nRegion,ET_ACTIVITY_ARM_RACE,NG_ACTIVITY_PICK_END_TIME)&&!m_bCleanArmRaceData)
	{
		hash_map<int64,DB_C_User*>::iterator iter =m_mapUser.begin();
		for(; iter!=m_mapUser.end();iter++)
		{
			DB_C_User* pUser = iter->second;
			if(pUser->wpclabsearchcnt()>0)
			{
				pUser->set_wpclabsearchcnt(0);
				markUserLstDirty(pUser->userid());
			}
			m_bCleanArmRaceData = true;
		}
	}
}

void CountryRgnDatHandler::SortWpcLabSearchRank()
{
	m_vecLabSearchRank.clear();

	vector<DB_C_User*>	vecUser;
	hash_map<int64,DB_C_User*>::iterator iter =m_mapUser.begin();
	for(; iter!=m_mapUser.end();iter++)
	{
		if(iter->second->wpclabsearchcnt()>0)
			vecUser.push_back(iter->second);
	}

	sort(vecUser.begin(),vecUser.end(),conpareArmRaceRank);

	for(int i=0;i<vecUser.size()&&i<MAX_WPC_RANK_NUM;i++)
		m_vecLabSearchRank.push_back(vecUser[i]);

}

void CountryRgnDatHandler::FillWpcLabSearchRank(RseArmRace*pRse)
{
	for(int i=0;i<m_vecLabSearchRank.size()&&i<MAX_WPC_RANK_NUM;i++)
	{
		labSearchRank*pLabRank = pRse->add_list();
		pLabRank->set_labsearchcnt(m_vecLabSearchRank[i]->wpclabsearchcnt());
		pLabRank->set_name(m_vecLabSearchRank[i]->name());
	}
}

int CountryRgnDatHandler::GetSelfLabSearchRank(int64 userID)
{
	int nRank = 0;
	for(int i=0;i<m_vecLabSearchRank.size()&&i<MAX_WPC_RANK_NUM;i++)
	{
		if(m_vecLabSearchRank[i]->userid()==userID)
		{
			nRank = i+1;
			break;
		}
	}

	return nRank;
}

void CountryRgnDatHandler::GetRewardKey(int64 userid, int nFlag, vector<int>& vecNewkey)
{
	DB_C_User*pUser = GetDBCityUser(userid);
	if(pUser==NULL)
		return ;


	LeftRewardKey*pLeft = GetLeftRewardKey(nFlag);
	if(pLeft == NULL)
		return ;
	int nRandKey = pLeft->GetRandkey();
	m_OneYuanRewardKey[nFlag][nRandKey]=userid;

	DB_One_Yuan*pDBOne = GetOneYuanDataByFlag(pUser, nFlag);
	if(pDBOne==NULL)
	{
		pDBOne = pUser->add_oneyuan();
		pDBOne->set_flag(nFlag);
	}

	DB_One_Yuan_Reward_key*pRewardkey = pDBOne->add_rewardkey();
	pRewardkey->set_redwardkey(nRandKey);
	pRewardkey->set_nindex(m_OneYuanRewardKey[nFlag].size());
	vecNewkey.push_back(nRandKey);

	SYS_LOG(int64(pUser->userid()),LT_ONE_YUAN_PURCHASE_BUY_INFO,0,0,nFlag<<m_OneYuanRewardKey[nFlag].size()<<nRandKey<<GetRegion());

}

void CountryRgnDatHandler::InitOneYuanPurchaseData()
{
	hash_map<int64,DB_C_User*>::iterator iter = m_mapUser.begin();
	for(;iter!=m_mapUser.end();iter++)
	{
		DB_C_User*pDBRUser = iter->second;

		for(int i=0; i<pDBRUser->oneyuan_size();i++)
		{
			DB_One_Yuan*pOY = pDBRUser->mutable_oneyuan(i);
			int nFlag = pOY->flag();
			map<int,int64>& m_mapRewardKeyFlag = m_OneYuanRewardKey[nFlag];
			for(int j=0;j<pOY->rewardkey_size();j++)
			{
				DB_One_Yuan_Reward_key*pDBRewardKey = pOY->mutable_rewardkey(j);
				m_mapRewardKeyFlag[pDBRewardKey->redwardkey()]=pDBRUser->userid();
			}

		}
	}
	//填充一个没用的数值

	map<int, OneYuanPurchaseTbl*>::iterator itTbl = OneYuanPurchaseCfg::instance().GetMapTbl().begin();
	for(;itTbl!=OneYuanPurchaseCfg::instance().GetMapTbl().end();itTbl++)
	{
		OneYuanPurchaseTbl*pTbl = itTbl->second;
		if(pTbl==NULL)
			continue;
		int nMax = 10000;
		int nFlag = pTbl->m_nID;

		vector<int>vecRewardKey;
		for(int k=0; k<nMax;k++)
		{
			map<int,int64>::iterator it = m_OneYuanRewardKey[nFlag].find(k);

			if(it == m_OneYuanRewardKey[nFlag].end())
			{
				vecRewardKey.push_back(k);
			}
		}

		LeftRewardKey* pLeftkey = new LeftRewardKey();
		pLeftkey->Init(vecRewardKey);
		pLeftkey->nFlag = pTbl->m_nID;
		m_vecLeftRewardKey.push_back(pLeftkey);
	}

}

DB_C_User* CountryRgnDatHandler::GetWinner(int nFlag, int nRewardKey)
{
	OneYuanPurchaseTbl*pTbl = OneYuanPurchaseCfg::instance().GetOneYuanTbl(nFlag);
	if(pTbl==NULL)
		return NULL;


	//如果数据库里有直接读取不用计算了
	DB_Region_One_Yuan*pRegionDB = m_pMainUserList->mutable_regiononeyuan();
	for(int i=0; i<pRegionDB->win_size();i++)
	{
		DB_One_Yuan_Purchase_Winner*pRegionWiner = pRegionDB->mutable_win(i);
		if(pRegionWiner->flag() == nFlag)
		{
			return GetDBCityUser(pRegionWiner->userid());
		}
	}

	//没有的话就需要计算了
	map<int,int64>& mapRewardKey = m_OneYuanRewardKey[nFlag];
	map<int,int64>::iterator iter = mapRewardKey.begin();

	int nMinGap = pTbl->m_nTotalCopies;
	vector<int64> vecWinUser;
	int nWinKey = 0;
	//获取与抽奖码最近的值，如果有两个相同的话，就比较时间
	//1,现获取数组中与nRewardKey最近的差值
	for(;iter!=mapRewardKey.end();iter++)
	{
		int nTempKey = iter->first;
		int nTempGap = abs(nTempKey-nRewardKey);
		if(nTempGap<nMinGap)
		{
			vecWinUser.clear();
			nMinGap = nTempGap;
			nWinKey = nTempKey;
			vecWinUser.push_back(iter->second);
		}
		else if(nTempGap==nMinGap)
		{
			vecWinUser.push_back(iter->second);
		}
	}

	DB_C_User*pWinUser = NULL;
	int nMinIndex = pTbl->m_nTotalCopies;
	for(int i=0; i<vecWinUser.size(); i++)
	{
		DB_C_User*pUser = GetDBCityUser(vecWinUser[i]);
		if(pUser==NULL)
			continue;

		DB_One_Yuan*pDBOne = NULL;
		for(int i=0; i<pUser->oneyuan_size();i++)
		{
			if(pUser->mutable_oneyuan(i)->flag()==nFlag)
				pDBOne = pUser->mutable_oneyuan(i);
		}

		if(pDBOne==NULL)
			continue;

		for(int i=0;i<pDBOne->rewardkey_size();i++)
		{
			DB_One_Yuan_Reward_key*pDBRewardKey = pDBOne->mutable_rewardkey(i);
			if(pDBRewardKey->redwardkey()==nWinKey)
			{
				if(nMinIndex>=pDBRewardKey->nindex())
				{
					nMinIndex = pDBRewardKey->nindex();
					pWinUser = pUser;
				}
			}
		}
	}

	if(pWinUser==NULL)
		return NULL;
	//计算出得奖的人保存到数据库中
	DB_One_Yuan_Purchase_Winner*pAddWiner = pRegionDB->add_win();
	pAddWiner->set_flag(nFlag);
	pAddWiner->set_userid(pWinUser->userid());

	SYS_LOG(int64(pWinUser->userid()),LT_ONE_YUAN_WINNER_RECORD,0,0,nFlag<<pWinUser->name()<<nMinIndex<<nWinKey<<GetRegion());

	return pWinUser;
}

void CountryRgnDatHandler::CheckClearOneYuanPurchaseData()
{
	if(m_pMainUserList == NULL)
		return ;

	if(m_pMainUserList->has_regiononeyuan())
	{
		DB_Region_One_Yuan*pRegionDB = m_pMainUserList->mutable_regiononeyuan();
		time_t ltNow = time(NULL);
		if(ltNow>=pRegionDB->oneyuanpurchasecleartime())
		{
			hash_map<int64, DB_C_User*>::iterator iter = m_mapUser.begin();
			for(;iter!=m_mapUser.end();iter++)
			{
				DB_C_User*pUser = iter->second;
				pUser->clear_oneyuan();
			}

			pRegionDB->Clear();
		}

		markUserLstDirty(m_pMainUserList);
	}
}

int CountryRgnDatHandler::GetParticitionCntByFlag(int nFlag)
{
	return m_OneYuanRewardKey[nFlag].size();
}

string CountryRgnDatHandler::IntTo4String(int nValue)
{
	stringstream ss;  
	string strValue;  
	ss << nValue;  
	ss >> strValue; 
	strValue = "0000"+strValue;
	int nLength = strValue.length();
	return strValue.substr(nLength-4,nLength);
}

DB_One_Yuan* CountryRgnDatHandler::GetOneYuanDataByFlag(DB_C_User*pUser, int nFlag)
{
	for(int i=0; i<pUser->oneyuan_size();i++)
	{
		DB_One_Yuan*pDBOne = pUser->mutable_oneyuan(i);
		if(pDBOne->flag()==nFlag)
		{
			return pDBOne;
		}
	}

	return NULL;
}

