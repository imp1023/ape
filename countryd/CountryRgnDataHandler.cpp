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
#include "../common/Clock.h"
#include "../logic/GameSrvCfg.h"
#include "../logic/StarLogic.h"
#include "../common/SysLog.h"

int maxAllianceId = 0;
int MailTime_Cmp(DB_C_Alliance* pAln1, DB_C_Alliance* pAln2)
{
	return pAln1->rank() > pAln2->rank();
}
CountryRgnDatHandler::CountryRgnDatHandler(CountryDataHandler* pDH,int nRegion)
:m_pDH(pDH)
,m_nRegion(nRegion)
{
	m_MaxCityID = 0;
}

CountryRgnDatHandler::~CountryRgnDatHandler()
{

}

void CountryRgnDatHandler::Clear()
{
	
}

void CountryRgnDatHandler::InitCountry()
{
	LoadAllCountryInfo();
}

bool CountryRgnDatHandler::LoadAllCountryInfo()
{
	bool bLoadError = false;
	char szKey[128];
	time_t ltNow = time(NULL);
	m_pCountry = new CCountry(m_nRegion);
	sprintf(szKey,"Country_%d",m_nRegion);
	bool bGet = m_pDH->LoadCountryFromDB(szKey,m_pCountry->m_dbCountry,bLoadError);
	if(bLoadError)
	{//连接数据库失败，直接退出，不会走到这里
		exit(0);
	}
	
	for(int i = 0; i <  m_pCountry->m_dbCountry.city_size(); i++){
		DB_C_City *pDBCity = m_pCountry->m_dbCountry.mutable_city(i);
		CCity *pCity = new CCity(m_nRegion, pDBCity);
		m_mapCitys.insert(make_pair(pDBCity->cityid(), pCity));
		if(pDBCity->cityid() > m_MaxCityID){
			m_MaxCityID = pDBCity->cityid();
		}
	}

	for(int k = 0; k < m_pCountry->m_dbCountry.users_size(); k++){
		DB_C_UserLite *pDBUser = m_pCountry->m_dbCountry.mutable_users(k);
		CUser *pUser = new CUser(pDBUser);
		m_mapUsers.insert(make_pair(pDBUser->uid(), pUser));
	}

	DB_C_AllianceList* pDBALST = m_pCountry->m_dbCountry.mutable_alliancelist(); //hansoul
	for(int i = 0; i < pDBALST->alliance_size();i++)
	{
		DB_C_Alliance* pDBAln = pDBALST->mutable_alliance(i);
		if(pDBAln->allianceid() > maxAllianceId)
			maxAllianceId = pDBAln->allianceid();
		m_mapAlliances.insert(make_pair(pDBAln->allianceid(), pDBAln));
		
	}
	
	map<int, DB_C_Alliance*>::iterator iter;
	for(iter = m_mapAlliances.begin();iter != m_mapAlliances.end(); ++iter)
	{
		m_allianceByRank.push_back(iter->second);
	}
	sort(m_allianceByRank.begin(),m_allianceByRank.end(),MailTime_Cmp);
}

DB_C_Alliance* CountryRgnDatHandler::GetAlliance(int aid)//hansoul
{
	map<int, DB_C_Alliance*>::iterator iter = m_mapAlliances.find(aid);
	if(iter != m_mapAlliances.end())
		return iter->second;
	return NULL;
}


CCity* CountryRgnDatHandler::GetCity(int nCityID)
{
	map<int, CCity*>::iterator iter = m_mapCitys.find(nCityID);
	if(iter != m_mapCitys.end())
		return iter->second;
	return NULL;
}

CUser* CountryRgnDatHandler::GetUser(int uid)
{
	map<int, CUser*>::iterator iter = m_mapUsers.find(uid);
	if(iter != m_mapUsers.end())
		return iter->second;
	return NULL;
}

void CountryRgnDatHandler::markCountryDirty(CCountry* pCountry)
{
	m_pDH->markCountryDirty(pCountry);
}

void CountryRgnDatHandler::GetNewPlayerStar(int64 uid, int nPlanetId, string platId, string url, string &strRspSku, int &nRspName, int &nRspType, int &nStarId)
{
	DB_C_Country *pDBCountry = m_pCountry->GetDBCountry();
	map<int, CUser*>::iterator _iter = m_mapUsers.find(uid);
	if(_iter == m_mapUsers.end()){
		DB_C_UserLite *pDBUser = pDBCountry->add_users();
		pDBUser->set_uid(uid);
		pDBUser->set_name(platId);
		pDBUser->set_url(url);
		pDBUser->set_xp(0);
		pDBUser->set_score(0);
		pDBUser->set_damageprotectiontimeleft(INIT_PLAYER_DMGPROTECTIONTIMETOTAL);
		pDBUser->set_tutorialcompleted(1);
		pDBUser->set_isonline(0);
		pDBUser->set_levelbasedonscore(1);
		CUser *pUser = new CUser(pDBUser);
		m_mapUsers.insert(make_pair(pDBUser->uid(), pUser));
	}
	
	if(pDBCountry && pDBCountry->city_size() == 0){
		DB_C_City *pDBCity = pDBCountry->add_city();
		CCity *pCCity = new CCity(m_nRegion, pDBCity);
		pCCity->SetCityID(1);
		pCCity->SetX(0);
		pCCity->SetY(0);
		nRspName = StarLogicInst::instance().GetStarName();
		pCCity->SetName(nRspName);
		pCCity->SetType(StarLogicInst::instance().GetStarType());

		DB_C_PlanetLite *pDBPlanet = pDBCity->add_planets();
		pDBPlanet->set_uid(uid);
		pDBPlanet->set_planetid(nPlanetId);
		pDBPlanet->set_type(3);
		nRspType = INIT_PLAYER_PLANETTYPE;
		pDBPlanet->set_hqlevel(1);
		pDBPlanet->set_capital(1);
		char sku[128] = {0};
		sprintf(sku, "%d:%d:%d", pCCity->GetX(), pCCity->GetY(), 1);
		strRspSku = sku;
		pDBPlanet->set_sku(sku);
		m_MaxCityID = 1;
		nStarId = 1;
		m_mapCitys.insert(make_pair(pCCity->GetCityID(), pCCity));
	}else{
		map<int, CCity*>::iterator iter = m_mapCitys.find(m_MaxCityID);
		if(iter != m_mapCitys.end()){
			CCity *pCity = iter->second;
			int nSize = pCity->GetPlanetSize();
			if(nSize < StarLogicInst::instance().STAR_ALLOCATE_OCCUPIED){
				DB_C_City *pDBCity = pCity->GetDBCity();
				DB_C_PlanetLite *pDbPlanet = pDBCity->add_planets();
				pDbPlanet->set_uid(uid);
				pDbPlanet->set_planetid(nPlanetId);
				pDbPlanet->set_type(INIT_PLAYER_PLANETTYPE);
				nRspType = INIT_PLAYER_PLANETTYPE;
				pDbPlanet->set_hqlevel(1);
				pDbPlanet->set_capital(1);
				char sku[128] = {0};
				sprintf(sku, "%d:%d:%d", pCity->GetX(), pCity->GetY(), nSize+1);
				pDbPlanet->set_sku(sku);
				strRspSku = sku;
				nRspName = pCity->GetName();
				nStarId = m_MaxCityID;
			}else{
				int x = 0, y = 0, newx = 0, newy = 1;
				map<int, CCity*>::iterator iter = m_mapCitys.find(m_MaxCityID);
				if(iter != m_mapCitys.end()){
					x = iter->second->GetX();
					y = iter->second->GetY();
				}
				StarLogicInst::instance().GetNewStar(x, y, newx, newy);
				DB_C_City *pDBCity = pDBCountry->add_city();
				CCity *pCity = new CCity(m_nRegion, pDBCity);
				pDBCity->set_cityid(++m_MaxCityID);
				pDBCity->set_x(newx);
				pDBCity->set_y(newy);
				pDBCity->set_type(StarLogicInst::instance().GetStarType());
				nRspName = StarLogicInst::instance().GetStarName();
				pDBCity->set_name(nRspName);
				DB_C_PlanetLite *pDBPlanet = pDBCity->add_planets();
				pDBPlanet->set_uid(uid);
				pDBPlanet->set_planetid(nPlanetId);
				pDBPlanet->set_type(INIT_PLAYER_PLANETTYPE);
				nRspType = INIT_PLAYER_PLANETTYPE;
				pDBPlanet->set_hqlevel(1);
				pDBPlanet->set_capital(1);
				char sku[128] = {0};
				sprintf(sku, "%d:%d:%d", pCity->GetX(), pCity->GetY(), 1);
				pDBPlanet->set_sku(sku);
				strRspSku = sku;
				nStarId = m_MaxCityID;
				m_mapCitys.insert(make_pair(m_MaxCityID, pCity));
			}
		}
	}
	markCountryDirty(m_pCountry);
}

void CountryRgnDatHandler::FillinStarInfo(int nStarId, RseQueryStarInfo *rsp)
{
	if(!rsp){
		return;
	}
	CCity *PCCity = GetCity(nStarId);
	if(PCCity){
		char sku[128] = {0};
		sprintf(sku, "%d:%d", PCCity->GetX(), PCCity->GetY());
		rsp->set_sku(sku);
		rsp->set_starid(PCCity->GetCityID());
		rsp->set_name(PCCity->GetName());
		rsp->set_type(PCCity->GetType());
		DB_C_City *pDBCity = PCCity->GetDBCity();
		if(pDBCity){
			for(int i = 0; i < pDBCity->planets_size(); i++){
				DB_C_PlanetLite *pDBPlanet = pDBCity->mutable_planets(i);
				SpaceStarInfo *pMsgPlanet = rsp->add_spacestarinfo();
				if(pDBPlanet && pMsgPlanet){
					pMsgPlanet->set_sku(pDBPlanet->sku());
					pMsgPlanet->set_planetid(pDBPlanet->planetid());
					pMsgPlanet->set_type(pDBPlanet->type());
					pMsgPlanet->set_hqlevel(pDBPlanet->hqlevel());
					pMsgPlanet->set_capital(pDBPlanet->capital());
					pMsgPlanet->set_accountid(toString(pDBPlanet->uid()));
					CUser *pUser = GetUser(pDBPlanet->uid());
					if(pUser){
						pMsgPlanet->set_name(pUser->GetName());
						pMsgPlanet->set_url(pUser->GetURL());
						pMsgPlanet->set_xp(pUser->GetXP());
						pMsgPlanet->set_score(pUser->GetScore());
						pMsgPlanet->set_damageprotectiontimeleft(0/*pUser->GetDmgProtectionTimeLeft()*/);
						pMsgPlanet->set_tutorialcompleted(pUser->GetTutorialCompleted());
						pMsgPlanet->set_isonline(pUser->GetOnline());
						pMsgPlanet->set_levelbasedonscore(pUser->GetLevelOnScore());
					}
				}
			}
		}
	}
}

bool CountryRgnDatHandler::CheckAlnName(string name)//hansoul
{
	map<int, DB_C_Alliance*>::iterator iter;
	for(iter = m_mapAlliances.begin(); iter != m_mapAlliances.end();++iter)
	{
		DB_C_Alliance* pTmp = iter->second;
		if(pTmp->name() == name)
			return false;
	}
	return true;
}

int CountryRgnDatHandler::CreateAlliance(string name, string logo, string description, int publicRecruit,string guid, Members* mb)//hansoul
{
	if(!CheckAlnName(name))
	{
		return 0;
	}
	
	DB_C_AllianceList* ALST = m_pCountry->m_dbCountry.mutable_alliancelist();
	DB_C_Alliance* pAln = ALST->add_alliance();
	pAln->set_allianceid(++maxAllianceId);//从1 开始
	pAln->set_logo(logo);
	pAln->set_name(name);
	pAln->set_description(description);
	pAln->set_createat(time(NULL));
	pAln->set_rank(m_mapAlliances.size() + 1);
	pAln->set_publicrecruit(publicRecruit);
	pAln->set_warswon(0);
	pAln->set_warslost(0);
	pAln->set_totalwarscore(0);
	pAln->set_totalmembers(1);
	pAln->set_postwarshield(0);
	pAln->set_enemyallianceid(0);
	pAln->set_currentwarscore(0);
	pAln->set_warstarttime(0);
	int64 Guid = 0;
	safe_atoll(guid,Guid);
	pAln->set_guid(Guid);

	//创建不成功退钱问题未解决！
	DB_C_AliMember* pM = pAln->add_members();
	pM->set_id(Guid);
	pM->set_name(mb->name);
	pM->set_pictureurl(mb->pictureUrl);
	pM->set_role(Alliances_Role_LEADER);//
	pM->set_score(mb->score);
	pM->set_currentwarscore(mb->currentWarScore);
	m_mapAlliances.insert(make_pair(pAln->allianceid(), pAln));
	m_allianceByRank.push_back(pAln);
	markCountryDirty(m_pCountry);
	return pAln->allianceid();
}

void CountryRgnDatHandler::FillinAln(int alnId, RseUpdateAlliances* rse, bool includeMembers)
{
	DB_C_Alliance* pAln = GetAlliance(alnId);
	if(!pAln)
		return;
	Alliance* aln = rse->add_alliance();
	aln->set_id(pAln->allianceid());
	aln->set_logo(pAln->logo());
	aln->set_publicrecruit(pAln->publicrecruit());
	aln->set_createdat(pAln->createat());
	aln->set_description(pAln->description());
	aln->set_name(pAln->name());
	aln->set_totalmembers(pAln->totalmembers());
	aln->set_totalwarscore(pAln->totalwarscore());
	aln->set_warswon(pAln->warswon());
	aln->set_warslost(pAln->warslost());
	aln->set_enemyallianceid(pAln->enemyallianceid());
	aln->set_rank(pAln->rank());
	aln->set_warstarttime(pAln->warstarttime());
	aln->set_postwarshield(pAln->postwarshield());
	aln->set_warendtime(pAln->warendtime());
	aln->set_allianceid(pAln->allianceid());
	aln->set_totalmembers(pAln->totalmembers());
	//rse->set_knockoutPoints(20);
	//rse->set_unreadnews();
	//rse->set_totalnews();
	if(includeMembers)
	{
		DB_C_AliMember* pMb = NULL;
		for(int i = 0;i < pAln->members_size();i++)
		{
			pMb = pAln->mutable_members(i);
			Member* mb = aln->add_member();
			string id = toString(pMb->id());
			mb->set_id(id);
			mb->set_name(pMb->name());
			mb->set_pictureurl(pMb->pictureurl());
			mb->set_role(pMb->role());
			mb->set_score(pMb->score());
			mb->set_currentwarscore(pMb->currentwarscore());
			mb->set_totalwarscore(pMb->totalwarscore());
		}
	}
}

bool CountryRgnDatHandler::editAlliance(int allianceId, string logo, string description,RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(!pAln)
		return false;
	pAln->set_logo(logo);
	pAln->set_description(description);
	rseUpdateAlliances->set_ret(0);
	FillinAln(allianceId,rseUpdateAlliances, false);
	markCountryDirty(m_pCountry);
	return true;
}

bool CountryRgnDatHandler::updatePublic(int allianceId, int publicRecruit,RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(!pAln)
		return false;
	pAln->set_publicrecruit(publicRecruit);
	rseUpdateAlliances->set_ret(0);
	FillinAln(allianceId,rseUpdateAlliances, false);
	markCountryDirty(m_pCountry);
	return true;
}

bool CountryRgnDatHandler::getMyAlliance(int allianceId, int attackScore, int warScore, int role, int64 playerId, RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(!pAln)
	{
		pAln = GetAllianceByPlayer(playerId);
		if(!pAln)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
			return false;
		}
		allianceId = pAln->allianceid();
	}
	if(role == 3 && attackScore > 0)
	{
		DB_C_AliMember* pMember = GetAllianceMember(allianceId, playerId);
		pMember->set_currentwarscore(pMember->currentwarscore() + attackScore);
		pMember->set_totalwarscore(pMember->totalwarscore() + attackScore);
		pAln->set_currentwarscore(pAln->currentwarscore() + attackScore);
		pAln->set_totalwarscore(pAln->totalwarscore() + attackScore);

		DB_C_AllianceWar* pMyWar = pAln->mutable_allianceswar(pAln->allianceswar_size() -1);
		if(pMyWar)
		{
			pMyWar->set_myalliancewarscore(pMyWar->myalliancewarscore() + attackScore);
		}
		DB_C_Alliance* pEnemyAln = GetAlliance(pAln->enemyallianceid());
		if(pEnemyAln)
		{
			DB_C_AllianceWar* pEnemyWar = pEnemyAln->mutable_allianceswar(pEnemyAln->allianceswar_size() -1);
			if(pEnemyWar)
				pEnemyWar->set_myalliancewarscore(pEnemyWar->myalliancewarscore() + attackScore);
		}
		
	}
	FillinAln(allianceId, rseUpdateAlliances,true);
	markCountryDirty(m_pCountry);
	rseUpdateAlliances->set_ret(0);
	return true;
}
DB_C_AliMember*	CountryRgnDatHandler::GetAllianceMember(int allianceId, int64 playerId)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	for(int i = 0; i < pAln->members_size(); i++)
	{
		DB_C_AliMember* pAlnMember = pAln->mutable_members(i);
		if(pAlnMember->id() == playerId)
			return pAlnMember;
	}
	return NULL;
}

DB_C_Alliance*	CountryRgnDatHandler::GetAllianceByPlayer(int64 playerId)
{
	map<int, DB_C_Alliance*>::iterator iter;
	for(iter = m_mapAlliances.begin();iter != m_mapAlliances.end();++iter)
	{
		DB_C_Alliance* pAln = iter->second;
		for(int i = 0; i < pAln->members_size(); i++)
		{
			DB_C_AliMember* pAlnMember = pAln->mutable_members(i);
			if(pAlnMember->id() == playerId)
				return pAln;
		}
	}
	return NULL;
}

bool CountryRgnDatHandler::joinAlliance(int allianceId, int64 guid, int level,string name, string pictureUrl, int score,RseUpdateAlliances* rseUpdateAlliances)
{
	if(level < 15)//15
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NEED_LEVEL);
		return false;
	}
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	else if(pAln->publicrecruit() == 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	else if(pAln->members_size() >= 50)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_IS_FULL);
		return false;
	}
	else 
	{
		DB_C_AllianceNews* pAlnNews = pAln->add_alliancesnews();
		pAlnNews->set_allianceid(allianceId);
		pAlnNews->set_guid(guid);
		pAlnNews->set_level(level);
		pAlnNews->set_type(1);
		pAlnNews->set_subtype(14);
		pAlnNews->set_name(name);
		pAlnNews->set_content(pictureUrl);
		pAlnNews->set_timestamp(time(NULL));
		pAlnNews->set_pictureurl(pictureUrl);
		pAlnNews->set_score(score);
	}
	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->add_alliance()->set_allianceid(allianceId);
	markCountryDirty(m_pCountry);
	return true;
}

bool CountryRgnDatHandler::refuseJoinAlliance(int allianceId, int64 playerId, int64 guid, RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	DB_C_AliMember* pAlnMember = GetAllianceMember(allianceId, playerId);
	if(!pAlnMember)
	{	
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_FOUND);
		return false;
	}
	if(pAlnMember->role() == Alliances_Role_REGULAR)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_NO_PERMISSION);
		return false;
	}
	bool bFind = false;
	for(int i =0;i<pAln->alliancesnews_size();i++)
	{
		DB_C_AllianceNews* pAlnNews = pAln->mutable_alliancesnews(i);
		if(guid == pAlnNews->guid())
		{
			DB_C_AllianceNews* pAlnNews_last = pAln->mutable_alliancesnews(pAln->alliancesnews_size() - 1);
			if(pAlnNews_last && pAlnNews_last != pAlnNews)
				pAlnNews_last->Swap(pAlnNews);
			pAln->mutable_alliancesnews()->RemoveLast();
			bFind = true;
			break;
		}
	}
	if(!bFind)
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
	else
	{
		rseUpdateAlliances->set_ret(0);
		markCountryDirty(m_pCountry);
	}
	return bFind;
}

bool CountryRgnDatHandler::agreeJoinAlliance(int allianceId, int64 playerId ,int64 guid ,RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(playerId);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	DB_C_AliMember* pAlnMember = GetAllianceMember(allianceId, playerId);
	if(!pAlnMember)
	{	
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_FOUND);
		return false;
	}
	if(pAlnMember->role() == Alliances_Role_REGULAR)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_NO_PERMISSION);
		return false;
	}
	bool bFind = false;
	string na = "";
	string pictureUrl = "";
	int score = 0;
	int level = 0;
	for(int i =0;i<pAln->alliancesnews_size();i++)
	{
		DB_C_AllianceNews* pAlnNews = pAln->mutable_alliancesnews(i);
		if(guid == pAlnNews->guid())
		{
			na = pAlnNews->name();
			pictureUrl = pAlnNews->pictureurl();
			score = pAlnNews->score();
			level = pAlnNews->level();
			DB_C_AllianceNews* pAlnNews_last = pAln->mutable_alliancesnews(pAln->alliancesnews_size() - 1);
			if(pAlnNews_last && pAlnNews_last != pAlnNews)
				pAlnNews_last->Swap(pAlnNews);
			pAln->mutable_alliancesnews()->RemoveLast();
			bFind = true;
			break;
		}
	}
	if(!bFind)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
		return bFind;
	}

	pAln->set_totalmembers(pAln->totalmembers() + 1);
	DB_C_AliMember* pDBMember = pAln->add_members();
	pDBMember->set_id(guid);
	pDBMember->set_name(na);
	pDBMember->set_pictureurl(pictureUrl);
	pDBMember->set_role(Alliances_Role_REGULAR);
	pDBMember->set_currentwarscore(0);
	pDBMember->set_totalwarscore(0);
	pDBMember->set_score(score);
	
	FillinAln(allianceId,rseUpdateAlliances, false);
	Alliance* aln = rseUpdateAlliances->mutable_alliance(0);
	Member* mb = aln->add_member();
	mb->set_id(toString(pDBMember->id()));
	mb->set_name(pDBMember->name());
	mb->set_pictureurl(pDBMember->pictureurl());
	mb->set_role(pDBMember->role());
	mb->set_totalwarscore(0);
	mb->set_score(score);
	mb->set_currentwarscore(0);
	
	DB_C_AllianceNews* pAlnNews = pAln->add_alliancesnews();
	pAlnNews->set_allianceid(allianceId);
	pAlnNews->set_guid(guid);
	pAlnNews->set_level(level);
	pAlnNews->set_type(1);
	pAlnNews->set_subtype(1);
	pAlnNews->set_name(na);
	pAlnNews->set_content(pictureUrl);
	pAlnNews->set_timestamp(time(NULL));
	pAlnNews->set_pictureurl(pictureUrl);
	pAlnNews->set_score(score);

	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->set_totalsize(0);
	markCountryDirty(m_pCountry);
	return bFind;
}

void CountryRgnDatHandler::DisMissAlliance(int allianceId)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	DB_C_AllianceList* pDBALST = m_pCountry->m_dbCountry.mutable_alliancelist(); //hansoul
	if(pDBALST->alliance_size() > 1)
	{
		DB_C_Alliance* pAln_last = pDBALST->mutable_alliance(pDBALST->alliance_size() - 1);
		if(pAln_last && pAln_last != pAln)
		{
			pAln_last->Swap(pAln);
		}
	}
	map<int, DB_C_Alliance*>::iterator iter = m_mapAlliances.find(allianceId);
	if(iter != m_mapAlliances.end()){
		m_mapAlliances.erase(iter);
	}

	vector<DB_C_Alliance*>::iterator iterv;
	for(iterv = m_allianceByRank.begin(); iterv!= m_allianceByRank.end(); ++iterv)
	{
		if(*iterv == pAln)
			m_allianceByRank.erase(iterv);
		break;
	}
	
	pDBALST->mutable_alliance()->RemoveLast();
}

void CountryRgnDatHandler::RemoveMember(int allianceId, int64 playerId)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	DB_C_AliMember* pDBMember = GetAllianceMember(allianceId,playerId);
	DB_C_AliMember* pDBMember_last = pAln->mutable_members(pAln->members_size() - 1);
	if(pDBMember_last && pDBMember_last != pDBMember)
		pDBMember_last->Swap(pDBMember);
	pAln->mutable_members()->RemoveLast();
	pAln->set_totalmembers(pAln->totalmembers() - 1);
	markCountryDirty(m_pCountry);
}

bool CountryRgnDatHandler::leaveAlliance(int allianceId,int64 guid ,RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	allianceId = pAln->allianceid();
	if(pAln->enemyallianceid() != 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_WAR_Not_ALLOWED);
		return false;
	}
	DB_C_AliMember* pDBMember = GetAllianceMember(allianceId, guid);
	if(!pDBMember)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE);
		return false;
	}
	if(pDBMember->role() == Alliances_Role_LEADER)//如果是将军
	{
		bool bFind = false;
		for(int i = 0; i < pAln->members_size();i++)
		{
			DB_C_AliMember* pDBM = pAln->mutable_members(i);
			if(pDBM->role() == Alliances_Role_ADMIN)
			{
				pDBM->set_role(Alliances_Role_LEADER);
				RemoveMember(allianceId,guid);
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			for(int i = 0; i < pAln->members_size();i++)
			{
				DB_C_AliMember* pDBM = pAln->mutable_members(i);
				if(pDBM->role() == Alliances_Role_REGULAR)
				{
					pDBM->set_role(Alliances_Role_LEADER);
					RemoveMember(allianceId,guid);
					bFind = true;
					break;
				}
			}
		}
		if(!bFind)
		{
			DisMissAlliance(allianceId);
		}
		rseUpdateAlliances->set_ret(0);
		markCountryDirty(m_pCountry);
		return true;
	}
	else
	{
		RemoveMember(allianceId,guid);
		rseUpdateAlliances->set_ret(0);
		markCountryDirty(m_pCountry);
		return true;
	}

}

bool CountryRgnDatHandler::kickMember(int64 adminId, int64 memberId, RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(adminId);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	if(pAln->enemyallianceid() != 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_WAR_Not_ALLOWED);
		return false;
	}
	RemoveMember(pAln->allianceid(),memberId);
	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->set_memberid(toString(memberId));
	return true;
}

bool CountryRgnDatHandler::grantMember(int64 memberId, int role, RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(memberId);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	DB_C_AliMember* pAlnMember = GetAllianceMember(pAln->allianceid(), memberId);
	if(!pAlnMember)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_FOUND);
		return false;
	}
	pAlnMember->set_role(role);
	rseUpdateAlliances->set_ret(0);
	Member* mb = rseUpdateAlliances->add_alliance()->add_member();
	mb->set_id(toString(memberId));
	mb->set_role(role);
	markCountryDirty(m_pCountry);
	return true;
}

bool CountryRgnDatHandler::getAlliance(int allianceId, int64 guid, bool includeMembers, RseUpdateAlliances* rseUpdateAlliances)
{
	DB_C_Alliance* pAln = NULL;
	if(allianceId != 0)
	{
		pAln = GetAlliance(allianceId);
		if(!pAln)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
			return false;
		}
	}
	else if (guid != 0)
	{
		pAln = GetAllianceByPlayer(guid);
		if(!pAln)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
			return false;
		}
		allianceId = pAln->allianceid();
	}
	FillinAln(allianceId,rseUpdateAlliances,includeMembers);
	rseUpdateAlliances->set_ret(0);
	return true;
}

bool CountryRgnDatHandler::getAlliances(int64 guid, int count, int from, bool userPage, string searchKey, RseUpdateAlliances* rseUpdateAlliances)
{
	int currentCount = 0;
	int totalSize = m_mapAlliances.size();
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
		return false;
	}
	if(userPage)
	{
		DB_C_AllianceList* pDBALST = m_pCountry->m_dbCountry.mutable_alliancelist();
		int rank = pAln->rank();
		for(int i = from;i < m_allianceByRank.size();i++)
		{
			if(m_allianceByRank[i]->rank() >= rank && currentCount < count && m_allianceByRank[i] != pAln)
			{
				FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
				currentCount++;
			}
		}
	}
	else
	{
		if(searchKey == "")
		{
			for(int i = from;i < m_allianceByRank.size();i++)
			{
				if(currentCount < count && pAln != m_allianceByRank[i])
				{
					FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
					currentCount++;
				}
			}
		}
		else
		{
			totalSize = 0;
			string::size_type position;
			for(int i = from;i < m_allianceByRank.size();i++)
			{
				string name = m_allianceByRank[i]->name();
				position = name.find(searchKey);
				if(position != name.npos && currentCount < count && pAln != m_allianceByRank[i])
				{
					FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
					currentCount++;
				}
				if(position != name.npos)
				{
					totalSize++;
				}
			}
		}
	}
	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->set_totalsize(totalSize);
	return true;
}

bool CountryRgnDatHandler::CheckAlnInWar(int allianceId)
{
	DB_C_Alliance* pAln = GetAlliance(allianceId);
	if(pAln->enemyallianceid() != 0)
		return true;
	if(time(NULL) - pAln->postwarshield() < 0)
		return true;
	return false;
}

bool CountryRgnDatHandler::getAlliancesNotInWar(int64 guid, int count, int from, bool userPage, string searchKey, RseUpdateAlliances* rseUpdateAlliances)
{
	int currentCount = 0;
	int totalSize = 0;
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
		return false;
	}
	if(userPage)
	{
		DB_C_AllianceList* pDBALST = m_pCountry->m_dbCountry.mutable_alliancelist();
		int rank = pAln->rank();
		for(int i = from;i < m_allianceByRank.size();i++)
		{
			bool bInWar = CheckAlnInWar(m_allianceByRank[i]->allianceid());
			if(m_allianceByRank[i]->rank() >= rank && currentCount < count && m_allianceByRank[i] != pAln && !bInWar)
			{
				FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
				currentCount++;
			}
			if(m_allianceByRank[i]->rank() >= rank  && m_allianceByRank[i] != pAln && !bInWar)
			{
				totalSize++;
			}
		}
	}
	else
	{
		if(searchKey == "")
		{
			for(int i = from;i < m_allianceByRank.size();i++)
			{
				bool bInWar = CheckAlnInWar(m_allianceByRank[i]->allianceid());
				if(currentCount < count  && m_allianceByRank[i] != pAln && !bInWar)
				{
					FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
					currentCount++;
				}
			}
		}
		else
		{
			totalSize = 0;
			string::size_type position;
			for(int i = from;i < m_allianceByRank.size();i++)
			{
				string name = m_allianceByRank[i]->name();
				position = name.find(searchKey);
				bool bInWar = CheckAlnInWar(m_allianceByRank[i]->allianceid());
				if(position != name.npos && currentCount < count && m_allianceByRank[i] != pAln && !bInWar)
				{
					FillinAln(m_allianceByRank[i]->allianceid(),rseUpdateAlliances,false);
					currentCount++;
				}
				if(position != name.npos)
				{
					totalSize++;
				}
			}
		}
	}
	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->set_totalsize(totalSize);
	return true;
}

bool CountryRgnDatHandler::getSuggestedAlliances(int count,RseUpdateAlliances* rseUpdateAlliances)
{
	int total = m_allianceByRank.size();
	if(total > 5)
	{
		vector<DB_C_Alliance*> tmp ;
		tmp.assign(m_allianceByRank.begin(), m_allianceByRank.end());
		random_shuffle(tmp.begin(),tmp.end());
		for(int i = 0;i<count;i++)
		{
			FillinAln(m_allianceByRank[i]->allianceid(), rseUpdateAlliances, false);
		}

	}
	else
	{
		for(int i = 0; i < m_allianceByRank.size(); i++)
		{
			FillinAln(m_allianceByRank[i]->allianceid(), rseUpdateAlliances, false);
		}
	}
	return true;
}

bool CountryRgnDatHandler::warHistory(int allianceId, int count, int startIndex, RseUpdateAlliances*  rseUpdateAlliances)
{

	DB_C_Alliance* pAln = GetAlliance(allianceId);
	// Allience中判断联盟结束Count值为1,计算联盟分值,重置联盟
	if(count == 1)
	{
		if(!pAln)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
			return false;
		}
		int enemyAlnId = pAln->enemyallianceid();
		if(time(NULL) - pAln->warstarttime() < 0)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_MY_ALLIANCE_ALREADY_IN_WAR);
			return false;
		}
		DB_C_Alliance* pEnemyAln = GetAlliance(enemyAlnId);
		if(!pEnemyAln)
		{
			rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
			return false;
		}
		DB_C_AllianceWar* pMyWar = pAln->mutable_allianceswar(pAln->allianceswar_size() - 1);
		DB_C_AllianceWar* pEnWar = pEnemyAln->mutable_allianceswar(pEnemyAln->allianceswar_size() - 1);

		if(pMyWar != NULL && pMyWar->winnerid() == 0 && pEnWar != NULL && pEnWar->winnerid() == 0)
		{
			if( pMyWar!= NULL &&pMyWar->winnerid() == 0)
			{
				int myAllianceWarScore = pMyWar->myalliancewarscore();
				int enemyAllianceWarScore = pMyWar->enemyalliancewarscore();
				int onKnockout = 0;
				int winnerId = allianceId;

				if(myAllianceWarScore == enemyAllianceWarScore)
				{
					if(pAln->rank() < pEnemyAln->rank())//同分数rank高获胜
					{
						onKnockout = 1;
						winnerId = enemyAlnId;
					}
				}
				else if(myAllianceWarScore < enemyAllianceWarScore)
				{
					onKnockout = 1;
					winnerId = enemyAlnId;
				}
				pMyWar->set_winnerid(winnerId);
				pMyWar->set_onknockout(onKnockout);

				if(pAln->warstarttime() != 0)
				{
					pAln->set_warstarttime(0);
					pAln->set_enemyallianceid(0);
					pAln->set_currentwarscore(0);
					pAln->set_postwarshield(time(NULL) + 8 * 3600);
					if(onKnockout == 0)
						pAln->set_warswon(pAln->warswon() + 1);
					else
						pAln->set_warslost(pAln->warslost() + 1);
				}
			}
			if(pEnWar != NULL && pEnWar->winnerid() == 0)
			{
				int enemyAllianceWarScore = pMyWar->myalliancewarscore();
				int myAllianceWarScore = pMyWar->enemyalliancewarscore();
				int onKnockout = 1;
				int winnerId = allianceId;


				if(myAllianceWarScore == enemyAllianceWarScore)
				{
					if(pAln->rank() < pEnemyAln->rank())//同分数rank高获胜
					{
						onKnockout = 0;
						winnerId = enemyAlnId;
					}
				}
				else if(myAllianceWarScore < enemyAllianceWarScore)
				{
					onKnockout = 0;
					winnerId = enemyAlnId;
				}
				pEnWar->set_winnerid(winnerId);
				pEnWar->set_onknockout(onKnockout);

				if(pEnemyAln->warstarttime() != 0)
				{
					pEnemyAln->set_warstarttime(0);
					pEnemyAln->set_enemyallianceid(0);
					pEnemyAln->set_currentwarscore(0);
					pEnemyAln->set_postwarshield(time(NULL) + 8 * 3600);
					if(onKnockout == 0)
						pEnemyAln->set_warswon(pEnemyAln->warswon() + 1);
					else
						pEnemyAln->set_warslost(pEnemyAln->warslost() + 1);
				}
			}

		}
		
		else
		{

			if(pAln)
			{
				if(pAln->warstarttime() != 0)
				{
					pAln->set_currentwarscore(0);
					pAln->set_enemyallianceid(0);
					pAln->set_warstarttime(0);
					pAln->set_postwarshield(time(NULL) + 8 * 3600);
				}
				int enemyAlnId = pAln->enemyallianceid();
				DB_C_Alliance* pEnemyAln = GetAlliance(enemyAlnId);
				if(pEnemyAln)
				{
					pEnemyAln->set_currentwarscore(0);
					pEnemyAln->set_enemyallianceid(0);
					pEnemyAln->set_warstarttime(0);
					pEnemyAln->set_postwarshield(time(NULL) + 8 * 3600);
				}
			}

		}

	}

	for(int i = startIndex; i < count && i < pAln->allianceswar_size();i++)
	{
		DB_C_AllianceWar* pAlnWar = pAln->mutable_allianceswar(i);
		if(!pAlnWar)
			break;
		MsgAlliancesWar* MsgWar = rseUpdateAlliances->add_alliancewar();
		MsgWar->set_winnerid(pAlnWar->winnerid());
		MsgWar->set_onknockout(pAlnWar->onknockout());
		MsgWar->set_enemyalliancelogo(pAlnWar->enemyalliancelogo());
		MsgWar->set_enemyalliancename(pAlnWar->enemyalliancename());
		MsgWar->set_warstarttime(pAlnWar->warstarttime());
		MsgWar->set_warendtime(pAlnWar->warendtime());
		MsgWar->set_myalliancewarscore(pAlnWar->myalliancewarscore());
		MsgWar->set_enemyalliancewarscore(pAlnWar->enemyalliancewarscore());
		MsgWar->set_enemyallianceid(pAlnWar->has_enemyallianceid());
	}
	rseUpdateAlliances->set_ret(0);
	markCountryDirty(m_pCountry);
	return true;
}

bool CountryRgnDatHandler::sendMessage(RseUpdateAlliances*  rseUpdateAlliances)
{
	rseUpdateAlliances->set_ret(0);
	return true;
}

void CountryRgnDatHandler::FillNews(DB_C_AllianceNews* pNews, RseUpdateAlliances*  rseUpdateAlliances)
{
	if(!pNews)
		return;
	MsgAllianceNews* MsgNews = rseUpdateAlliances->add_alliancenews();
	MsgNews->set_type(pNews->type());
	MsgNews->set_subtype(pNews->subtype());
	MsgNews->set_name(pNews->name());
	MsgNews->set_pictureurl(pNews->pictureurl());
	MsgNews->set_level(pNews->level());
	string strGuid = toString(pNews->guid());
	MsgNews->set_guid(strGuid);
	MsgNews->set_content(pNews->content());
	MsgNews->set_timestamp(pNews->timestamp());
	MsgNews->set_allianceid(pNews->allianceid());
}

bool CountryRgnDatHandler::getNews(int64 guid, int count, int fromIndex, RseUpdateAlliances*  rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE);
		return false;
	}
	for(int i = fromIndex; i < fromIndex + count && i < pAln->alliancesnews_size();i++)
	{
		DB_C_AllianceNews* pNews = pAln->mutable_alliancesnews(i);
		if(!pNews)
			break;
		FillNews(pNews, rseUpdateAlliances);
	}
	rseUpdateAlliances->set_ret(0);
	return true;
}

bool CountryRgnDatHandler::getReward(int64 guid, int alliancesBattlesWon,int warPoints, RseUpdateAlliances*  rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE);
		return false;
	}
	if(pAln->warswon() < alliancesBattlesWon)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
		return false;
	}
	DB_C_AliMember* pMember = GetAllianceMember(pAln->allianceid(), guid);
	if(!pMember)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE);
		return false;
	}
	if(pMember->totalwarscore() < warPoints)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_INVALID_COMMAND);
		return false;
	}
	rseUpdateAlliances->set_ret(0);
	return true;
}

bool CountryRgnDatHandler::declareWar(int64 guid, int enemyAllianceId, RseUpdateAlliances*  rseUpdateAlliances)
{
	DB_C_Alliance* pAln = GetAllianceByPlayer(guid);
	if(!pAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE);
		return false;
	}
	int allianceId = pAln->allianceid();
	DB_C_Alliance* pEnemyAln = GetAlliance(enemyAllianceId);
	if(!pEnemyAln)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_NOT_FOUND);
		return false;
	}
	if(pAln->enemyallianceid() != 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_MY_ALLIANCE_ALREADY_IN_WAR);
		return false;
	}
	if(pEnemyAln->enemyallianceid() != 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_ALREADY_IN_WAR);
		return false;
	}
	if(time(NULL) - pEnemyAln->postwarshield() < 0)
	{
		rseUpdateAlliances->set_ret(ERROR_CODE_ALLIANCE_HAS_POST_WAR_SHIELD);
		return false;
	}

	//满足所有开战条件
	pAln->set_enemyallianceid(enemyAllianceId);
	pAln->set_warstarttime(time(NULL));
	pAln->set_warendtime(time(NULL) + 3 * 24 * 3600);
	pEnemyAln->set_enemyallianceid(allianceId);
	pEnemyAln->set_warstarttime(time(NULL));
	pEnemyAln->set_warendtime(time(NULL) + 3 * 24 * 3600);

	DB_C_AllianceWar* pAlnWar = pAln->add_allianceswar();
	pAlnWar->set_allianceid(allianceId);
	pAlnWar->set_enemyallianceid(enemyAllianceId);
	pAlnWar->set_enemyalliancelogo(pEnemyAln->logo());
	pAlnWar->set_enemyalliancename(pEnemyAln->name());
	pAlnWar->set_enemyalliancewarscore(0);
	pAlnWar->set_myalliancewarscore(0);
	pAlnWar->set_onknockout(0);
	pAlnWar->set_warstarttime(time(NULL));
	pAlnWar->set_warendtime(time(NULL) + 3 * 24 * 3600);
	pAlnWar->set_winnerid(0);

	DB_C_AllianceWar* pAlnEWar = pEnemyAln->add_allianceswar();
	pAlnEWar->set_allianceid(enemyAllianceId);
	pAlnEWar->set_enemyallianceid(allianceId);
	pAlnEWar->set_enemyalliancelogo(pAln->logo());
	pAlnEWar->set_enemyalliancename(pAln->name());
	pAlnEWar->set_enemyalliancewarscore(0);
	pAlnEWar->set_myalliancewarscore(0);
	pAlnEWar->set_onknockout(0);
	pAlnEWar->set_warstarttime(time(NULL));
	pAlnEWar->set_warendtime(time(NULL) + 3 * 24 * 3600);
	pAlnEWar->set_winnerid(0);

	pAln->set_currentwarscore(0);
	pEnemyAln->set_currentwarscore(0);
	
	rseUpdateAlliances->set_ret(0);
	rseUpdateAlliances->set_enemyallianceid(enemyAllianceId);
	markCountryDirty(m_pCountry);
	return true;
}