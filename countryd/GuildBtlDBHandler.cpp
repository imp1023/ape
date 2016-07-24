#include "GuildBtlDBHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../logic/GuildBtlDBCfg.h"

#include "event.pb.h"
#include "EventDefine.h"
#include "CountryEventHandler.h"
#include "CountryRgnDataHandler.h"
#include "EventQueue.h"
#include "../logic/PVEFightManager.h"



GuildBtlDBHandler::GuildBtlDBHandler(void)
: m_pEventHandler(NULL)
, m_bEnable(true)
, m_bRunning(true)
{
	logger_ = log4cxx::Logger::getLogger("EventQueue");

	for (int i=0;i<THREADNUM;i++)
	{
		pthread_mutex_init(&m_mutexStarMsgGet[i], NULL);
		pthread_mutex_init(&m_mutexStarMsgPut[i], NULL);
	}
}

GuildBtlDBHandler::~GuildBtlDBHandler(void)
{
	for (map<int,void *>::iterator iter = m_mapDBGuildBtl.begin(); iter != m_mapDBGuildBtl.end(); iter++)
	{
		TCRDB* pDbStar = (TCRDB*) iter->second;
		tcrdbclose(pDbStar);
		tcrdbdel(pDbStar);
	}
	m_mapDBGuildBtl.clear();

	for (int i=0;i<THREADNUM;i++)
	{
		for (list<MsgGuildBtlDBGet*>::iterator iter = m_listMessageGet[i].begin(); iter != m_listMessageGet[i].end(); iter++)
		{
			MsgGuildBtlDBGet *pMessageGet = (MsgGuildBtlDBGet*)*iter;
			if (pMessageGet != NULL)
			{
				delete pMessageGet;
			}
		}
		m_listMessageGet[i].clear();

		for (list<MsgGuildBtlDBPut*>::iterator iter = m_listMessagePut[i].begin(); iter != m_listMessagePut[i].end(); iter++)
		{
			MsgGuildBtlDBPut *pMessagePut = (MsgGuildBtlDBPut*)*iter;
			if (pMessagePut != NULL)
			{
				delete pMessagePut;
			}
		}
		m_listMessagePut[i].clear();
	}
	

	for (int i=0;i<THREADNUM;i++)
	{
		pthread_mutex_destroy(&m_mutexStarMsgGet[i]);
		pthread_mutex_destroy(&m_mutexStarMsgPut[i]);
	}
	
}

void GuildBtlDBHandler::initThread()
{
	int ret = 0;
	
	{
		m_ltStart = time(NULL);
		
		for (int i=0;i<THREADNUM;i++)
		{
			m_ThredPara[i].pHandler = this;
			m_ThredPara[i].no       = i;
			ret = pthread_create(&m_threadStarInfo[i], NULL, GuildBtlDBHandler::StarInfoProc, (void *)(&m_ThredPara[i]));
		}
		
		if(ret != 0)
		{

		}
	}
}

void GuildBtlDBHandler::termThread()
{
	m_bRunning = false;
	for (int i=0;i<THREADNUM;i++)
	{
		pthread_join(m_threadStarInfo[i], NULL);
	}
	
}

void* GuildBtlDBHandler::StarInfoProc(void* arg)
{
	GuildBtlDBThread* pThreadPara = static_cast<GuildBtlDBThread*>(arg);
	GuildBtlDBHandler* pStarInfoHandler = pThreadPara->pHandler;
	pStarInfoHandler->Run(pThreadPara->no);
	pthread_exit(NULL);
	return NULL;
}

bool GuildBtlDBHandler::GetDBKey(int64 nUserID,GuildBtlDBType emType,string& strKey)
{
	char szKey[128];
	switch(emType)
	{
	case GuildBtlDB_USER:
		{
			sprintf(szKey,"GuildBtl_User_%016llX", nUserID);
		}
		break;
	case GuildGvgDB_USER:
		{
			sprintf(szKey,"GvgBtl_User_%016llX", nUserID);
		}
		break;
	default:
		return false;
	}
	strKey = szKey;
	return true;
}

bool GuildBtlDBHandler::GetDBKey(int nRegion,int nGuildID,int nTurns,int nWay,string& strKey)
{
	char szKey[128];
	sprintf(szKey,"GuidlBtlRetWay_%d_%d_%d_%d",nRegion,nGuildID,nTurns,nWay);
	strKey = szKey;
	return true;
}

bool GuildBtlDBHandler::GetGvgDBKey(int nRegion,int nCity,int nSession,string& strKey)
{
	char szKey[128];
	sprintf(szKey,"GvgBtlRet_%d_%d_%d",nRegion,nCity,nSession);
	strKey = szKey;
	return true;
}

bool GuildBtlDBHandler::SafePushGuildBtlResult(GuidlBtlRetWay* pRetWay,int nGuild1,int nGuild2,int nRegion,int nTurns,int nWay)
{
	if(pRetWay == NULL)
		return false;
	DB_C_GuidlBtlRetWay dbRet;
	dbRet.set_session(nTurns);
	dbRet.set_region(nRegion);
	dbRet.set_way(nWay);
	dbRet.set_winguildid(pRetWay->winguildid());
	dbRet.set_guildid1(nGuild1);
	dbRet.set_guildid2(nGuild2);

	for(int i=0;i<pRetWay->pklst_size();i++)
	{
		GuildBtlUserPK* pUserPK = pRetWay->mutable_pklst(i);
		DB_C_GuildBtlUserPK* pDBPK = dbRet.add_pklst();
		pDBPK->set_userid1(pUserPK->userid1());
		pDBPK->set_userid2(pUserPK->userid2());
		pDBPK->set_winid(pUserPK->winid());
		pDBPK->set_winlefthp(pUserPK->winlefthp());
	}
	for(int i=0;i<pRetWay->userlst1_size();i++)
	{
		GuildBtlUser* pRetUser = pRetWay->mutable_userlst1(i);
		DB_C_GuildBtlUser* pDBRetUser = dbRet.add_userlst1();
		pDBRetUser->set_userid(pRetUser->userid());
		pDBRetUser->set_name(pRetUser->name());
		pDBRetUser->set_pic(pRetUser->pic());
		pDBRetUser->set_combatvalue(pRetUser->combatvalue());
		pDBRetUser->set_addhppct(pRetUser->addhppct());
		pDBRetUser->set_addatkpct(pRetUser->addatkpct());
	}
	for(int i=0;i<pRetWay->userlst2_size();i++)
	{
		GuildBtlUser* pRetUser = pRetWay->mutable_userlst2(i);
		DB_C_GuildBtlUser* pDBRetUser = dbRet.add_userlst2();
		pDBRetUser->set_userid(pRetUser->userid());
		pDBRetUser->set_name(pRetUser->name());
		pDBRetUser->set_pic(pRetUser->pic());
		pDBRetUser->set_combatvalue(pRetUser->combatvalue());
		pDBRetUser->set_addhppct(pRetUser->addhppct());
		pDBRetUser->set_addatkpct(pRetUser->addatkpct());
	}

	string strKey1,strKey2;
	if(!GetDBKey(nRegion,nGuild1,nTurns,nWay,strKey1))
		return false;
	if(!GetDBKey(nRegion,nGuild2,nTurns,nWay,strKey2))
		return false;

	MsgGuildBtlDBPut *pMessagePut1 = new MsgGuildBtlDBPut();
	pMessagePut1->m_nUserID	= nGuild1;
	pMessagePut1->m_strKey	= strKey1;
	dbRet.SerializeToString(&(pMessagePut1->m_strInfo));

	MsgGuildBtlDBPut *pMessagePut2 = new MsgGuildBtlDBPut();
	pMessagePut2->m_nUserID	= nGuild2;
	pMessagePut2->m_strKey	= strKey2;
	pMessagePut2->m_strInfo = pMessagePut1->m_strInfo;

	int nrand = nGuild1%THREADNUM;
	LockMessagePut(nrand);
	m_listMessagePut[nrand].push_back(pMessagePut1);
	UnLockMessagePut(nrand);

	nrand = nGuild2%THREADNUM;
	LockMessagePut(nrand);
	m_listMessagePut[nrand].push_back(pMessagePut2);
	UnLockMessagePut(nrand);
}

bool GuildBtlDBHandler::SafePushGuildGvgResult(GuildBtlRet* pRet,int nGuild1,int nGuild2,int nRegion,int nCityID,int nSession)
{
	if ( pRet == NULL )
	{
		return false;
	}
 	DB_C_GvgBtlRetSave dbRet;
	dbRet.set_session(nSession);
	dbRet.set_region(nRegion);
	dbRet.set_winguildid(pRet->winid());
	dbRet.set_guildid1(nGuild1);
	dbRet.set_guildid2(nGuild2);
	dbRet.set_playercnt1( pRet->playercnt1() );
	dbRet.set_playercnt2( pRet->playercnt2() );

	for ( int j = 0; j < GUILD_BTL_DAY_CNT; j++ )
	{
		GuidlBtlRetWay* pRetWay;
		if ( j == 0 )
		{
			pRetWay = pRet->mutable_way0();
		}
		else if ( j == 1 )
		{
			pRetWay = pRet->mutable_way1();
		}
		else if ( j == 2 )
		{
			pRetWay = pRet->mutable_way2();
		}
		DB_C_GvgBtlRetWay* pDBWay = dbRet.add_retway();
		pDBWay->set_way(j);
		for ( int i=0;i<pRetWay->pklst_size();i++ )
		{
			GuildBtlUserPK* pUserPK = pRetWay->mutable_pklst(i);
			DB_C_GuildBtlUserPK* pDBPK = pDBWay->add_pklst();
			pDBPK->set_userid1(pUserPK->userid1());
			pDBPK->set_userid2(pUserPK->userid2());
			pDBPK->set_winid(pUserPK->winid());
			pDBPK->set_winlefthp(pUserPK->winlefthp());
			pDBPK->set_bstreakleave(pUserPK->bstreakleave());
		}

		for ( int i = 0; i < pRetWay->userlst1_size(); i++ )
		{
			DB_C_GuildBtlUser* pDBUser = pDBWay->add_userlst1();
			GuildBtlUser* pBtlUser = pRetWay->mutable_userlst1(i);

			pDBUser->set_userid(pBtlUser->userid());
			pDBUser->set_pic(pBtlUser->pic());
			pDBUser->set_name(pBtlUser->name());
			pDBUser->set_combatvalue(pBtlUser->combatvalue());
			pDBUser->set_addhppct(pBtlUser->addhppct());
			pDBUser->set_addatkpct(pBtlUser->addatkpct());
			pDBUser->set_maxwintime(pBtlUser->maxwintime());
			pDBUser->set_wintime(pBtlUser->wintime());
		}

		for ( int i = 0; i < pRetWay->userlst2_size(); i++ )
		{
			DB_C_GuildBtlUser* pDBUser = pDBWay->add_userlst2();
			GuildBtlUser* pBtlUser = pRetWay->mutable_userlst2(i);

			pDBUser->set_userid(pBtlUser->userid());
			pDBUser->set_pic(pBtlUser->pic());
			pDBUser->set_name(pBtlUser->name());
			pDBUser->set_combatvalue(pBtlUser->combatvalue());
			pDBUser->set_addhppct(pBtlUser->addhppct());
			pDBUser->set_addatkpct(pBtlUser->addatkpct());
			pDBUser->set_maxwintime(pBtlUser->maxwintime());
			pDBUser->set_wintime(pBtlUser->wintime());
		}
	}
 
 	string strKey;
 	if(!GetGvgDBKey(nRegion,nCityID,nSession,strKey))
 		return false;
 
 	MsgGuildBtlDBPut *pMessagePut1 = new MsgGuildBtlDBPut();
 	pMessagePut1->m_nUserID	= nCityID;
 	pMessagePut1->m_strKey	= strKey;
 	dbRet.SerializeToString(&(pMessagePut1->m_strInfo));
 
 	int nrand = nGuild1%THREADNUM;
 	LockMessagePut(nrand);
 	m_listMessagePut[nrand].push_back(pMessagePut1);
 	UnLockMessagePut(nrand);
}


bool GuildBtlDBHandler::SafeLoadGuildBtlResult(int64 nLoadUserID,int nGuild,int nRegion,int nTurns,int nGameID)
{
	MsgGuildBtlDBGet *pMessageGet = new MsgGuildBtlDBGet();
	pMessageGet->m_nRegion		= nRegion;
	pMessageGet->m_nGuild1		= nGuild;
	pMessageGet->m_nTurns		= nTurns;
	pMessageGet->m_emType		= GuildBtlDB_RESULT;
	pMessageGet->m_nGetUserID	= nLoadUserID;
	pMessageGet->m_nGameID		= nGameID;

	int nRand = rand()%THREADNUM;	
	LockMessageGet(nRand);
	m_listMessageGet[nRand].push_back(pMessageGet);
	UnLockMessageGet(nRand);

	return true;
}

bool GuildBtlDBHandler::SafeLoadGvgBtlResult(int64 nLoadUserID,int nCityID,int nRegion,int nSession,int nGameID)
{
	MsgGuildBtlDBGet *pMessageGet = new MsgGuildBtlDBGet();
	pMessageGet->m_nRegion		= nRegion;
	pMessageGet->m_nTurns		= nSession;
	pMessageGet->m_emType		= GuildGvgDB_RESULT;
	pMessageGet->m_nGetUserID	= nLoadUserID;
	pMessageGet->m_nGameID		= nGameID;
	pMessageGet->m_nCityID		= nCityID;

	int nRand = rand()%THREADNUM;	
	LockMessageGet(nRand);
	m_listMessageGet[nRand].push_back(pMessageGet);
	UnLockMessageGet(nRand);

	return true;
}


bool GuildBtlDBHandler::SafePushGuildBtlUpdate(int64 nUserID,GuildBtlDBType emType,const DB_C_Guild_FightSendData* pDBGuildBtl)
{
	if(pDBGuildBtl==NULL)
		return false;
	string strKey;
	if(!GetDBKey(nUserID,emType,strKey))
		return false;

	MsgGuildBtlDBPut *pMessagePut = new MsgGuildBtlDBPut();
	pMessagePut->m_nUserID	= nUserID;
	pMessagePut->m_strKey	= strKey;
	pDBGuildBtl->SerializeToString(&(pMessagePut->m_strInfo));

	int nrand = getUidHash(nUserID)%THREADNUM;
	LockMessagePut(nrand);
	m_listMessagePut[nrand].push_back(pMessagePut);
	UnLockMessagePut(nrand);

	return true;
}

bool GuildBtlDBHandler::SafePushGuildGvgUpdate(int64 nUserID,GuildBtlDBType emType,const DB_C_Gvg_FightSendData* pDBGuildBtl)
{
	if(pDBGuildBtl==NULL)
		return false;
	string strKey;
	if(!GetDBKey(nUserID,emType,strKey))
		return false;

	MsgGuildBtlDBPut *pMessagePut = new MsgGuildBtlDBPut();
	pMessagePut->m_nUserID	= nUserID;
	pMessagePut->m_strKey	= strKey;
	pDBGuildBtl->SerializeToString(&(pMessagePut->m_strInfo));

	int nrand = getUidHash(nUserID)%THREADNUM;
	LockMessagePut(nrand);
	m_listMessagePut[nrand].push_back(pMessagePut);
	UnLockMessagePut(nrand);

	return true;
}

bool GuildBtlDBHandler::DealGuildBtl(CGuild* pGuild1,CGuild* pGuild2,int nRegion,int nTurns,int nWay)
{
	if(pGuild1==NULL||pGuild2==NULL)
		return false;
	//for(int i=0;i<3;i++)
	{
		vector<int64> lstGuidUser1;
		vector<int64> lstGuidUser2;
		switch(nWay)
		{
		case 0:
			{
				for(int j=0;j<pGuild1->GetDBGuild()->mutable_btlinfo()->userway0_size();j++)
				{
					int64 nId =  pGuild1->GetDBGuild()->mutable_btlinfo()->userway0(j);
					if(nId != 0){
						lstGuidUser1.push_back(nId);	
					}
				}
				for(int j=0;j<pGuild2->GetDBGuild()->mutable_btlinfo()->userway0_size();j++)
				{
					int64 nId = pGuild2->GetDBGuild()->mutable_btlinfo()->userway0(j);
					if(nId != 0){
						lstGuidUser2.push_back(nId);
					}
				}
			}
			break;
		case 1:
			{
				for(int j=0;j<pGuild1->GetDBGuild()->mutable_btlinfo()->userway1_size();j++)
				{
					int64 nId = pGuild1->GetDBGuild()->mutable_btlinfo()->userway1(j);
					if(nId != 0){
						lstGuidUser1.push_back(nId);
					}
				}
				for(int j=0;j<pGuild2->GetDBGuild()->mutable_btlinfo()->userway1_size();j++)
				{
					int64 nId = pGuild2->GetDBGuild()->mutable_btlinfo()->userway1(j);
					if(nId != 0){
						lstGuidUser2.push_back(nId);
					}
				}
			}
			break;
		case 2:
			{
				for(int j=0;j<pGuild1->GetDBGuild()->mutable_btlinfo()->userway2_size();j++)
				{
					int64 nId = pGuild1->GetDBGuild()->mutable_btlinfo()->userway2(j);
					if(nId != 0){
						lstGuidUser1.push_back(nId);
					}
				}
				for(int j=0;j<pGuild2->GetDBGuild()->mutable_btlinfo()->userway2_size();j++)
				{
					int64 nId = pGuild2->GetDBGuild()->mutable_btlinfo()->userway2(j);
					if(nId != 0){
						lstGuidUser2.push_back(nId);
					}
				}
			}
			break;
		}

		int nThread = rand()%THREADNUM;
		int nNum1 = lstGuidUser1.size();
		int nNum2 = lstGuidUser2.size();
		if(nNum1 == 0 && nNum2 == 0){
			MsgGuildBtlDBGet *pMessageGet = new MsgGuildBtlDBGet();
			pMessageGet->m_nRegion	= nRegion;
			pMessageGet->m_nGuild1	= pGuild1->GetGuildID();
			pMessageGet->m_nGuild2	= pGuild2->GetGuildID();
			pMessageGet->m_nWay		= nWay;
			pMessageGet->m_nTurns	= nTurns;
			pMessageGet->m_emType	= GuildBtlDB_USER;
			pMessageGet->m_nMaxPlayerNum = 0;
			pMessageGet->m_nCityID = 0;
			pMessageGet->m_lstGuildUser1.push_back(0);
			pMessageGet->m_lstGuildUser2.push_back(0);
			LockMessageGet(nThread);
			m_listMessageGet[nThread].push_back(pMessageGet);
			UnLockMessageGet(nThread);
		}else{
			int nNumMax = nNum1 > nNum2 ? nNum1: nNum2;
			int nTotalMsg = nNumMax / MsgGuildBtlDBGet::e_maxplayer_eachmsg;
			if(nNumMax % MsgGuildBtlDBGet::e_maxplayer_eachmsg){
				nTotalMsg += 1;
			}
			for(int x = 0; x < nTotalMsg; x++){
				MsgGuildBtlDBGet *pMessageGet = new MsgGuildBtlDBGet();
				pMessageGet->m_nRegion	= nRegion;
				pMessageGet->m_nGuild1	= pGuild1->GetGuildID();
				pMessageGet->m_nGuild2	= pGuild2->GetGuildID();
				pMessageGet->m_nWay		= nWay;
				pMessageGet->m_nTurns	= nTurns;
				pMessageGet->m_emType	= GuildBtlDB_USER;
				pMessageGet->m_nMaxPlayerNum = nNumMax;
				pMessageGet->m_nCityID = x;
				for(int y = 0; y < MsgGuildBtlDBGet::e_maxplayer_eachmsg; y++){
					int z = x * MsgGuildBtlDBGet::e_maxplayer_eachmsg + y;
					if(z < nNum1){
						pMessageGet->m_lstGuildUser1.push_back(lstGuidUser1[z]);
					}
					if(z < nNum2){
						pMessageGet->m_lstGuildUser2.push_back(lstGuidUser2[z]);
					}
				}
				LockMessageGet(nThread);
				m_listMessageGet[nThread].push_back(pMessageGet);
				UnLockMessageGet(nThread);
			}
		}
	}
	return true;
}

bool GuildBtlDBHandler::DealGvgBtl(CGuild* pGuild1,CGuild* pGuild2,int nRegion,int nSession,int nWay,int nCityID)
{
	if(pGuild1==NULL||pGuild2==NULL)
		return false;
	int nRand = rand()%THREADNUM;

	DB_C_GvgBtlCityMsg* pDBGvgCity1 = pGuild1->GetGvgBtlCity(nCityID,nSession);
	DB_C_GvgBtlCityMsg* pDBGvgCity2 = pGuild2->GetGvgBtlCity(nCityID,nSession);
	if ( pDBGvgCity1 == NULL || pDBGvgCity2 == NULL )
	{
		return false;
	}
	int nCnt1 = pGuild1->GetGvgPlayerCnt(pDBGvgCity1);
	int nCnt2 = pGuild2->GetGvgPlayerCnt(pDBGvgCity2);

	if ( nCnt1 > pDBGvgCity1->userway1_size() )
	{
		nCnt1 = pDBGvgCity1->userway1_size();
	}

	if ( nCnt2 > pDBGvgCity2->userway1_size() )
	{
		nCnt2 = pDBGvgCity2->userway1_size();
	}

	int nMaxSize = nCnt1 > nCnt2 ? nCnt1 : nCnt2;

	const GvgBaseTbl* pBaseTbl = GvgCfg::Instance().GetBaseTbl();
	if (!pBaseTbl){ return false; }
	int nMaxPerRoad = pBaseTbl->m_nMaxPerRoad;
	int nStartNum = nWay * nMaxPerRoad;

	vector<int64> lstGuidUser1;
	for(int j = nStartNum; j < pDBGvgCity1->userway1_size() && j < nCnt1; j++){
		if ( j - nStartNum >= nMaxPerRoad){
			break;
		}
		lstGuidUser1.push_back(pDBGvgCity1->userway1(j));
	}
	vector<int64> lstGuidUser2;
	for(int j=nStartNum;j<pDBGvgCity2->userway1_size()&&j<nCnt2;j++){
		if ( j - nStartNum >= nMaxPerRoad ){
			break;
		}
		lstGuidUser2.push_back(pDBGvgCity2->userway1(j));
	}

	int nNum1 = lstGuidUser1.size();
	int nNum2 = lstGuidUser2.size();
	int nNumMax = nNum1 > nNum2 ? nNum1: nNum2;
	int nTotalMsg = nNumMax / MsgGuildBtlDBGet::e_maxplayer_eachmsg;
	if(nNumMax % MsgGuildBtlDBGet::e_maxplayer_eachmsg){
		nTotalMsg += 1;
	}
	for(int x = 0; x < nTotalMsg; x++){
		MsgGuildBtlDBGet *pMessageGet = new MsgGuildBtlDBGet();
		pMessageGet->m_nRegion	= nRegion;
		pMessageGet->m_nGuild1	= pGuild1->GetGuildID();
		pMessageGet->m_nGuild2	= pGuild2->GetGuildID();
		pMessageGet->m_nWay		= nWay;
		pMessageGet->m_nTurns	= nSession;
		pMessageGet->m_emType	= GuildGvgDB_USER;
		pMessageGet->m_nCityID	= nCityID;
		pMessageGet->m_nMaxPlayerNum = nMaxSize;
		for(int y = 0; y < MsgGuildBtlDBGet::e_maxplayer_eachmsg; y++){
			int z = x * MsgGuildBtlDBGet::e_maxplayer_eachmsg + y;
			if(z < nNum1){
				pMessageGet->m_lstGuildUser1.push_back(lstGuidUser1[z]);
			}
			if(z < nNum2){
				pMessageGet->m_lstGuildUser2.push_back(lstGuidUser2[z]);
			}
		}
		LockMessageGet(nRand);
		m_listMessageGet[nRand].push_back(pMessageGet);
		UnLockMessageGet(nRand);
	}

	return true;
}
 
void GuildBtlDBHandler::Run(int no)
{
	if (no<0 || no>=THREADNUM)
		return;
	vector<MsgGuildBtlDBGet*> vecMessageGet;
	vector<MsgGuildBtlDBPut*>	vecMessagePut;
	vecMessageGet.reserve(10240);
	vecMessagePut.reserve(10240);
	while(m_bRunning)
	{
		Run4Load(no,vecMessageGet);
		Run4Save(no,vecMessagePut);
	
		usleep(1000);
	}
}

void GuildBtlDBHandler::Run4Save(int no,vector<MsgGuildBtlDBPut*>& vecMessagePut)
{
	for (size_t i = 0; i < vecMessagePut.size(); i++)
	{
		MsgGuildBtlDBPut *pMessagePut = vecMessagePut.at(i);
		if (pMessagePut != NULL)
		{
			delete pMessagePut;
		}
	}
	vecMessagePut.clear();
	LockMessagePut(no);
	while (!m_listMessagePut[no].empty())
	{
		vecMessagePut.push_back(m_listMessagePut[no].front());
		m_listMessagePut[no].pop_front();
	}
	UnLockMessagePut(no);

	for (size_t i = 0; i < vecMessagePut.size(); i++)
	{
		MsgGuildBtlDBPut *pMessagePut = vecMessagePut.at(i);
		if (pMessagePut != NULL)
		{
			SaveGuildBtlInfo(pMessagePut->m_nUserID,pMessagePut->m_strKey,pMessagePut->m_strInfo);
		}
	}
}

void GuildBtlDBHandler::Run4Load(int no,vector<MsgGuildBtlDBGet*>&	vecMessageGet)
{
	for (size_t i = 0; i < vecMessageGet.size(); i++)
	{
		MsgGuildBtlDBGet *pMessageGet = vecMessageGet.at(i);
		if (pMessageGet != NULL)
		{
			delete pMessageGet;
		}
	}
	vecMessageGet.clear();

	LockMessageGet(no);
	while (!m_listMessageGet[no].empty())
	{
		vecMessageGet.push_back(m_listMessageGet[no].front());
		m_listMessageGet[no].pop_front();
	}
	UnLockMessageGet(no);

	for (size_t i = 0; i < vecMessageGet.size(); i++)
	{
		MsgGuildBtlDBGet *pMessageGet = vecMessageGet[i];
		if (pMessageGet == NULL || m_pEventHandler == NULL)
			continue;

		Event *pEvent = AddEventRequest(0,pMessageGet->m_emType);
		if(pEvent==NULL)
			continue;
		switch(pMessageGet->m_emType)
		{
		case GuildBtlDB_RESULT:
			{
				GCG_CountryNeed* pNeedArgs = pEvent->mutable_countryneed();
				pNeedArgs->set_gamesrvid(pMessageGet->m_nGameID);
				pNeedArgs->set_region(pMessageGet->m_nRegion);

				RseGuildOpt* pRseOpt = pEvent->mutable_se_rseguildopt();

				pEvent->set_uid(pMessageGet->m_nGetUserID);
				
				GuildBtlRet* pBtlRet = pRseOpt->mutable_bltret();
				for(int nWay=0;nWay<3;nWay++)
				{
					string strKey;
					GetDBKey(pMessageGet->m_nRegion,pMessageGet->m_nGuild1,pMessageGet->m_nTurns,nWay,strKey);
					string strDBInfo;
					bool bResult = LoadGuildBtlInfo(pMessageGet->m_nGuild1, strKey,strDBInfo);
					if(!bResult)
						continue;
					DB_C_GuidlBtlRetWay dbRet;
					dbRet.ParseFromString(strDBInfo);
					GuidlBtlRetWay* pRetWay = NULL;
					switch(nWay)
					{
					case 0:
						{
							pBtlRet->set_region(pMessageGet->m_nRegion);
							pBtlRet->set_session(pMessageGet->m_nTurns);
							pBtlRet->set_guildid1(dbRet.guildid1());
							pBtlRet->set_guildid2(dbRet.guildid2());
							pRetWay = pBtlRet->mutable_way0();
						}
						break;
					case 1:
						{
							pRetWay = pBtlRet->mutable_way1();
						}
						break;
					case 2:
						{
							pRetWay = pBtlRet->mutable_way2();
						}
						break;
					default:
						break;
					}
					if(pRetWay==NULL)
						continue;
					pRetWay->set_winguildid(dbRet.winguildid());
					for(int j=0;j<dbRet.userlst1_size();j++)
					{
						DB_C_GuildBtlUser* pDBBUser = dbRet.mutable_userlst1(j);
						GuildBtlUser* pBUser = pRetWay->add_userlst1();
						pBUser->set_userid(pDBBUser->userid());
						pBUser->set_pic(pDBBUser->pic());
						pBUser->set_name(pDBBUser->name());
						pBUser->set_combatvalue(pDBBUser->combatvalue());
						pBUser->set_addhppct(pDBBUser->addhppct());
						pBUser->set_addatkpct(pDBBUser->addatkpct());
					}
					for(int j=0;j<dbRet.userlst2_size();j++)
					{
						DB_C_GuildBtlUser* pDBBUser = dbRet.mutable_userlst2(j);
						GuildBtlUser* pBUser = pRetWay->add_userlst2();
						pBUser->set_userid(pDBBUser->userid());
						pBUser->set_pic(pDBBUser->pic());
						pBUser->set_name(pDBBUser->name());
						pBUser->set_combatvalue(pDBBUser->combatvalue());
						pBUser->set_addhppct(pDBBUser->addhppct());
						pBUser->set_addatkpct(pDBBUser->addatkpct());
					}
					for(int j=0;j<dbRet.pklst_size();j++)
					{
						DB_C_GuildBtlUserPK* pDBPK = dbRet.mutable_pklst(j);
						GuildBtlUserPK* pPK = pRetWay->add_pklst();
						pPK->set_userid1(pDBPK->userid1());
						pPK->set_userid2(pDBPK->userid2());
						pPK->set_winid(pDBPK->winid());
						pPK->set_winlefthp(pDBPK->winlefthp());
					}
				}
			}
			break;
		case GuildBtlDB_USER:
			{

				int nGameID = pMessageGet->m_nGuild1 % serverConfig.physicsGameNum() + 1;
				GCG_CountryNeed* pNeedArgs = pEvent->mutable_countryneed();
				pNeedArgs->set_gamesrvid(nGameID);
				pNeedArgs->set_region(pMessageGet->m_nRegion);

				GuildBtl_FightCacuData* pGBSendData = pEvent->mutable_guildbtlcacudata();
				pGBSendData->set_region(pMessageGet->m_nRegion);
				pGBSendData->set_guildid1(pMessageGet->m_nGuild1);
				pGBSendData->set_guildid2(pMessageGet->m_nGuild2);
				pGBSendData->set_way(pMessageGet->m_nWay);
				pGBSendData->set_session(pMessageGet->m_nTurns);
				pGBSendData->set_maxplayercnt(pMessageGet->m_nMaxPlayerNum);
				pGBSendData->set_cityid(pMessageGet->m_nCityID);

				for(int j=0;j<pMessageGet->m_lstGuildUser1.size();j++)
				{
					int64 nUserID = pMessageGet->m_lstGuildUser1[j];
					if(nUserID > GBTool_Empty && nUserID < GBTool_Cnt)
					{//道具
						Country_FightSendData* pFightData = pGBSendData->add_fightdata1();
						PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
						pAtkUser->set_userid(toString(nUserID));
						continue;
					}
					string strKey;
					string strDBInfo;
					GetDBKey(nUserID,pMessageGet->m_emType,strKey);
					bool bResult = LoadGuildBtlInfo(nUserID, strKey,strDBInfo);
					if(!bResult)
						continue;
					DB_C_Guild_FightSendData dbFight;
					dbFight.ParseFromString(strDBInfo);
					Country_FightSendData* pFightData = pGBSendData->add_fightdata1();
					FillCountryFightSendData(dbFight,pFightData);
				}
				for(int j=0;j<pMessageGet->m_lstGuildUser2.size();j++)
				{
					int64 nUserID = pMessageGet->m_lstGuildUser2[j];
					if(nUserID > GBTool_Empty && nUserID < GBTool_Cnt)
					{//道具
						Country_FightSendData* pFightData = pGBSendData->add_fightdata2();
						PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
						pAtkUser->set_userid(toString(nUserID));
						continue;
					}
					string strKey;
					string strDBInfo;
					GetDBKey(nUserID,pMessageGet->m_emType,strKey);
					bool bResult = LoadGuildBtlInfo(nUserID, strKey,strDBInfo);
					if(!bResult)
						continue;
					DB_C_Guild_FightSendData dbFight;
					dbFight.ParseFromString(strDBInfo);
					Country_FightSendData* pFightData = pGBSendData->add_fightdata2();
					FillCountryFightSendData(dbFight,pFightData);
				}
			}
			break;

		case GuildGvgDB_RESULT:
			{
				GCG_CountryNeed* pNeedArgs = pEvent->mutable_countryneed();
				pNeedArgs->set_gamesrvid(pMessageGet->m_nGameID);
				pNeedArgs->set_region(pMessageGet->m_nRegion);

				RseGvgOpt* pRseOpt = pEvent->mutable_se_rsegvgopt();

				pEvent->set_uid(pMessageGet->m_nGetUserID);

				string strKey;
				GetGvgDBKey(pMessageGet->m_nRegion,pMessageGet->m_nCityID,pMessageGet->m_nTurns,strKey);
				string strDBInfo;
				bool bResult = LoadGuildBtlInfo(pMessageGet->m_nCityID, strKey,strDBInfo);
				if(!bResult)
					return;
				DB_C_GvgBtlRetSave dbAllRet;
				dbAllRet.ParseFromString(strDBInfo);

				GuildBtlRet* pBtlRet = pRseOpt->add_bltret();
				pBtlRet->set_winid(dbAllRet.winguildid());
				pBtlRet->set_playercnt1( dbAllRet.playercnt1() );
				pBtlRet->set_playercnt2( dbAllRet.playercnt2() );
				for(int nWay=0;nWay<3;nWay++)
				{
					GuidlBtlRetWay* pRetWay = NULL;
					DB_C_GvgBtlRetWay dbRet;

					if ( nWay >= dbAllRet.retway_size() )
					{
						continue;
					}
					switch(nWay)
					{
					case 0:
						{
							pBtlRet->set_region(pMessageGet->m_nRegion);
							pBtlRet->set_session(pMessageGet->m_nTurns);
							pBtlRet->set_guildid1(dbAllRet.guildid1());
							pBtlRet->set_guildid2(dbAllRet.guildid2());
							pBtlRet->set_cityid(dbAllRet.cityid());
							dbRet = dbAllRet.retway(0);
							pRetWay = pBtlRet->mutable_way0();
						}
						break;
					case 1:
						{
							dbRet = dbAllRet.retway(1);
							pRetWay = pBtlRet->mutable_way1();
						}
						break;
					case 2:
						{
							dbRet = dbAllRet.retway(2);
							pRetWay = pBtlRet->mutable_way2();
						}
						break;
					default:
						break;
					}
					if(pRetWay==NULL)
						continue;
					pRetWay->set_winguildid(dbRet.winguildid());
					for(int j=0;j<dbRet.userlst1_size();j++)
					{
						DB_C_GuildBtlUser* pDBBUser = dbRet.mutable_userlst1(j);
						GuildBtlUser* pBUser = pRetWay->add_userlst1();
						pBUser->set_userid(pDBBUser->userid());
						pBUser->set_pic(pDBBUser->pic());
						pBUser->set_name(pDBBUser->name());
						pBUser->set_combatvalue(pDBBUser->combatvalue());
						pBUser->set_addhppct(pDBBUser->addhppct());
						pBUser->set_addatkpct(pDBBUser->addatkpct());
						pBUser->set_maxwintime(pDBBUser->maxwintime());
						pBUser->set_wintime(pDBBUser->wintime());
					}
					for(int j=0;j<dbRet.userlst2_size();j++)
					{
						DB_C_GuildBtlUser* pDBBUser = dbRet.mutable_userlst2(j);
						GuildBtlUser* pBUser = pRetWay->add_userlst2();
						pBUser->set_userid(pDBBUser->userid());
						pBUser->set_pic(pDBBUser->pic());
						pBUser->set_name(pDBBUser->name());
						pBUser->set_combatvalue(pDBBUser->combatvalue());
						pBUser->set_addhppct(pDBBUser->addhppct());
						pBUser->set_addatkpct(pDBBUser->addatkpct());
						pBUser->set_maxwintime(pDBBUser->maxwintime());
						pBUser->set_wintime(pDBBUser->wintime());
					}
					for(int j=0;j<dbRet.pklst_size();j++)
					{
						DB_C_GuildBtlUserPK* pDBPK = dbRet.mutable_pklst(j);
						GuildBtlUserPK* pPK = pRetWay->add_pklst();
						pPK->set_userid1(pDBPK->userid1());
						pPK->set_userid2(pDBPK->userid2());
						pPK->set_winid(pDBPK->winid());
						pPK->set_winlefthp(pDBPK->winlefthp());
						pPK->set_bstreakleave( pDBPK->bstreakleave() );
					}
				}
			}
			break;
		case GuildGvgDB_USER:
			{
				int nGameID = pMessageGet->m_nGuild1 % serverConfig.physicsGameNum() + 1;
				GCG_CountryNeed* pNeedArgs = pEvent->mutable_countryneed();
				pNeedArgs->set_gamesrvid(nGameID);
				pNeedArgs->set_region(pMessageGet->m_nRegion);

				GuildBtl_FightCacuData* pGBSendData = pEvent->mutable_guildbtlcacudata();
				pGBSendData->set_region(pMessageGet->m_nRegion);
				pGBSendData->set_guildid1(pMessageGet->m_nGuild1);
				pGBSendData->set_guildid2(pMessageGet->m_nGuild2);
				pGBSendData->set_way(pMessageGet->m_nWay);
				pGBSendData->set_session(pMessageGet->m_nTurns);
				pGBSendData->set_cityid(pMessageGet->m_nCityID);
				pGBSendData->set_maxplayercnt(pMessageGet->m_nMaxPlayerNum);

				for(int j=0;j<pMessageGet->m_lstGuildUser1.size();j++)
				{
					int64 nUserID = pMessageGet->m_lstGuildUser1[j];
					string strKey;
					string strDBInfo;
					GetDBKey(nUserID,pMessageGet->m_emType,strKey);
					bool bResult = LoadGuildBtlInfo(nUserID, strKey,strDBInfo);
					if(!bResult)
					{
						Country_FightSendData* pFightData = pGBSendData->add_fightdata1();
						PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
						string strUid;
						strUid.clear();
						pAtkUser->set_userid(strUid.c_str());
						LOG4CXX_ERROR(logger_, "Gvg Load DB ERROR , userID"<<nUserID);
						continue;
					}
					DB_C_Gvg_FightSendData dbFight;
					dbFight.ParseFromString(strDBInfo);
					Country_FightSendData* pFightData = pGBSendData->add_fightdata1();
					FillCountryFightSendData(dbFight,pFightData);
				}
				for(int j=0;j<pMessageGet->m_lstGuildUser2.size();j++)
				{
					int64 nUserID = pMessageGet->m_lstGuildUser2[j];
					string strKey;
					string strDBInfo;
					GetDBKey(nUserID,pMessageGet->m_emType,strKey);
					bool bResult = LoadGuildBtlInfo(nUserID, strKey,strDBInfo);
					if(!bResult)
					{
						Country_FightSendData* pFightData = pGBSendData->add_fightdata2();
						PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
						string strUid;
						strUid.clear();
						pAtkUser->set_userid(strUid.c_str());
						LOG4CXX_ERROR(logger_, "Gvg Load DB ERROR , userID"<<nUserID);
						continue;
					}
					DB_C_Gvg_FightSendData dbFight;
					dbFight.ParseFromString(strDBInfo);
					Country_FightSendData* pFightData = pGBSendData->add_fightdata2();
					FillCountryFightSendData(dbFight,pFightData);
				}
			}
			break;
		default:
			break;
		}
		m_pEventHandler->getEventQueue()->safePushEvent(pEvent);	
	}
}

void GuildBtlDBHandler::FillCountryFightSendData(DB_C_Guild_FightSendData& dbFight,Country_FightSendData* pFightData)
{
	DB_C_GuildBtlUser* pDBCUser = dbFight.mutable_btluser();
	PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
	pAtkUser->set_userid(pDBCUser->userid());
	pAtkUser->set_name(pDBCUser->name());
	pAtkUser->set_pic(pDBCUser->pic());
	pAtkUser->set_level(pDBCUser->level());
	pAtkUser->set_combatvalue(pDBCUser->combatvalue());

	for(int i=0;i<dbFight.fightsenddata_size();i++)
	{
		DB_C_GuildBtlSendDataUnit* pDBDataUnit = dbFight.mutable_fightsenddata(i);
		Pve_FightSendDataUnit* pDataUnit = pFightData->add_fightsenddata();

        for (int j = 0; j < pDBDataUnit->nreducespeed_size(); j++)
        {
            pDataUnit->add_pvespeedreduce_self(pDBDataUnit->nreducespeed(j));
        }

		//兵种属性
		for(int j=0;j<pDBDataUnit->pvecarmyatkprop_size();j++)
		{
			DB_C_GuildBtlArmyAtkProp* pDBProp = pDBDataUnit->mutable_pvecarmyatkprop(j);

			Pve_CArmyAtkProp* pProp = pDataUnit->add_pvecarmyatkprop_self();

			pProp->set_narmytype(pDBProp->narmytype());
			pProp->set_nhp(pDBProp->nhp());
			pProp->set_ndamagebase(pDBProp->ndamagebase());
			pProp->set_ndamagepec(pDBProp->ndamagepec());
			pProp->set_nrange(pDBProp->nrange());
			pProp->set_ncrt(pDBProp->ncrt());
			pProp->set_nspeed(pDBProp->nspeed());
			pProp->set_nturnspeed(pDBProp->nturnspeed());

			pProp->set_ncrtdamage(pDBProp->ncrtdamage());
			pProp->set_ncrtdef(pDBProp->ncrtdef());
			pProp->set_ndamagedef(pDBProp->ndamagedef());
			for(int k=0;k<pDBProp->nrestraint_size();k++)
			{
				pProp->add_nrestraint(pDBProp->nrestraint(k));
			}
			for(int k=0;k<pDBProp->nrestraintdef_size();k++)
			{
				pProp->add_nrestraintdef(pDBProp->nrestraintdef(k));
			}
			pProp->set_nrangebase(pDBProp->nrangebase());
			pProp->set_nspeedbase(pDBProp->nspeedbase());
			pProp->set_npropertydamagepec(pDBProp->npropertydamagepec());
			pProp->set_npropertydamagedefpec(pDBProp->npropertydamagedefpec());

            for (int k = 0; k < pDBProp->ndefatkpec_size();k++)
            {
                pProp->add_ndefatkpec(pDBProp->ndefatkpec(k));
                pProp->add_ndefcritrange(pDBProp->ndefcritrange(k));
                pProp->add_ndefcritrate(pDBProp->ndefcritrate(k));
            }
			
		}
		//战斗单元
		for(int j=0;j<pDBDataUnit->pvefightunit_size();j++)
		{
			DB_C_GuildBtlFightUnit* pDBUnit = pDBDataUnit->mutable_pvefightunit(j);
			Pve_FightUnit* pFUnit = pDataUnit->add_pvefightunit_self();

			pFUnit->set_nid(pDBUnit->nid());
			pFUnit->set_narmytype(pDBUnit->narmytype());
			pFUnit->set_nhp(pDBUnit->nhp());
			pFUnit->set_nmaxhp(pDBUnit->nmaxhp());
			pFUnit->set_ngrade(pDBUnit->ngrade());
			pFUnit->set_narmygroup(pDBUnit->narmygroup());
			pFUnit->set_nrestframe(pDBUnit->nrestframe());
			Pve_Position* pPos = pFUnit->mutable_pos();
			pPos->set_nposx(pDBUnit->mutable_pos()->nposx());
			pPos->set_nposy(pDBUnit->mutable_pos()->nposy());
			pFUnit->set_nherograde(pDBUnit->nherograde());
			pFUnit->set_nherolv(pDBUnit->nherolv());
		}
		//英雄技能
		Pve_GuildBtlHeroSkill* pDBHeroSkill = pDBDataUnit->mutable_pvefightheroskill();
		if(pDBHeroSkill&& pDBHeroSkill->heroid()>0)
		{
			Pve_HeroSkill* pHeroSkill = pDataUnit->mutable_pveheroskill_self();
			pHeroSkill->set_heroid(pDBHeroSkill->heroid());
			pHeroSkill->set_level(pDBHeroSkill->level());
			pHeroSkill->set_skillid1(pDBHeroSkill->skillid1());
			pHeroSkill->set_skillid2(pDBHeroSkill->skillid2());
			pHeroSkill->set_skill1grade(pDBHeroSkill->skill1grade());
			pHeroSkill->set_skill2grade(pDBHeroSkill->skill2grade());
			pHeroSkill->set_skill1quality(pDBHeroSkill->skill1quality());
			pHeroSkill->set_skill2quality(pDBHeroSkill->skill2quality());
		}
		//将领
		DB_C_GuildBtlAdmiralAdd* pDBAdmiralAdd = pDBDataUnit->mutable_pvefightadmiral();
		Pve_AdmiralAdd* pAdmiralAdd = pDataUnit->mutable_pvefightadmiral_self();
		for(int j=0;j<pDBAdmiralAdd->naddpctfoot_size();j++)
		{
			pAdmiralAdd->add_naddpctfoot(pDBAdmiralAdd->naddpctfoot(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpcttank_size();j++)
		{
			pAdmiralAdd->add_naddpcttank(pDBAdmiralAdd->naddpcttank(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpctplane_size();j++)
		{
			pAdmiralAdd->add_naddpctplane(pDBAdmiralAdd->naddpctplane(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpctbuild_size();j++)
		{
			pAdmiralAdd->add_naddpctbuild(pDBAdmiralAdd->naddpctbuild(j));
		}
		DB_C_GuildBtlGeneral* pDBGeneral = pDBAdmiralAdd->mutable_fightgeneral();
		PVEFightGeneral* pGeneral = pAdmiralAdd->mutable_fightgeneral();
		pGeneral->set_ngeneralid(pDBGeneral->ngeneralid());
		pGeneral->set_ngenerallvl(pDBGeneral->ngenerallvl());
		pGeneral->set_ngeneraltrainlvl(pDBGeneral->ngeneraltrainlvl());
		pGeneral->set_ngeneralatk(pDBGeneral->ngeneralatk());
		pGeneral->set_ngeneraldef(pDBGeneral->ngeneraldef());
		pGeneral->set_ngeneralhp(pDBGeneral->ngeneralhp());
		pGeneral->set_ngeneralcrt(pDBGeneral->ngeneralcrt());
		pGeneral->set_ngeneralscore(pDBGeneral->ngeneralscore());

        for (int mhIdx = 0; mhIdx < pDBGeneral->listgeneralmh_size(); mhIdx++)
        {
            DB_C_PVEFightMH *pDBMH = pDBGeneral->mutable_listgeneralmh(mhIdx);
            PVEFightMH *pMH = pGeneral->add_ngeneralmh();
            pMH->set_nmhid(pDBMH->nmhid());
            pMH->set_nmhlv(pDBMH->nmhlv());
        }
	}
}

void GuildBtlDBHandler::FillCountryFightSendData(DB_C_Gvg_FightSendData& dbFight,Country_FightSendData* pFightData)
{
	DB_C_GuildBtlUser* pDBCUser = dbFight.mutable_btluser();
	PVPFightUser* pAtkUser = pFightData->mutable_atkuser();
	pAtkUser->set_userid(pDBCUser->userid());
	pAtkUser->set_name(pDBCUser->name());
	pAtkUser->set_pic(pDBCUser->pic());
	pAtkUser->set_level(pDBCUser->level());
	pAtkUser->set_combatvalue(pDBCUser->combatvalue());

	for(int i=0;i<dbFight.fightsenddata_size();i++)
	{
		DB_C_GuildBtlSendDataUnit* pDBDataUnit = dbFight.mutable_fightsenddata(i);
		Pve_FightSendDataUnit* pDataUnit = pFightData->add_fightsenddata();
        for (int j = 0; j < pDBDataUnit->nreducespeed_size(); j++)
        {
            pDataUnit->add_pvespeedreduce_self(pDBDataUnit->nreducespeed(j));
        }

		//兵种属性
		for(int j=0;j<pDBDataUnit->pvecarmyatkprop_size();j++)
		{
			DB_C_GuildBtlArmyAtkProp* pDBProp = pDBDataUnit->mutable_pvecarmyatkprop(j);

			Pve_CArmyAtkProp* pProp = pDataUnit->add_pvecarmyatkprop_self();

			pProp->set_narmytype(pDBProp->narmytype());
			pProp->set_nhp(pDBProp->nhp());
			pProp->set_ndamagebase(pDBProp->ndamagebase());
			pProp->set_ndamagepec(pDBProp->ndamagepec());
			pProp->set_nrange(pDBProp->nrange());
			pProp->set_ncrt(pDBProp->ncrt());
			pProp->set_nspeed(pDBProp->nspeed());
			pProp->set_nturnspeed(pDBProp->nturnspeed());

			pProp->set_ncrtdamage(pDBProp->ncrtdamage());
			pProp->set_ncrtdef(pDBProp->ncrtdef());
			pProp->set_ndamagedef(pDBProp->ndamagedef());
			for(int k=0;k<pDBProp->nrestraint_size();k++)
			{
				pProp->add_nrestraint(pDBProp->nrestraint(k));
			}
			for(int k=0;k<pDBProp->nrestraintdef_size();k++)
			{
				pProp->add_nrestraintdef(pDBProp->nrestraintdef(k));
			}
			pProp->set_nrangebase(pDBProp->nrangebase());
			pProp->set_nspeedbase(pDBProp->nspeedbase());
			pProp->set_npropertydamagepec(pDBProp->npropertydamagepec());
			pProp->set_npropertydamagedefpec(pDBProp->npropertydamagedefpec());
            for (int k = 0; k < pDBProp->ndefatkpec_size();k++)
            {
                pProp->add_ndefatkpec(pDBProp->ndefatkpec(k));
                pProp->add_ndefcritrange(pDBProp->ndefcritrange(k));
                pProp->add_ndefcritrate(pDBProp->ndefcritrate(k));
            }

		}
		//战斗单元
		for(int j=0;j<pDBDataUnit->pvefightunit_size();j++)
		{
			DB_C_GuildBtlFightUnit* pDBUnit = pDBDataUnit->mutable_pvefightunit(j);
			Pve_FightUnit* pFUnit = pDataUnit->add_pvefightunit_self();

			pFUnit->set_nid(pDBUnit->nid());
			pFUnit->set_narmytype(pDBUnit->narmytype());
			pFUnit->set_nhp(pDBUnit->nhp());
			pFUnit->set_nmaxhp(pDBUnit->nmaxhp());
			pFUnit->set_ngrade(pDBUnit->ngrade());
			pFUnit->set_narmygroup(pDBUnit->narmygroup());
			pFUnit->set_nrestframe(pDBUnit->nrestframe());
			Pve_Position* pPos = pFUnit->mutable_pos();
			pPos->set_nposx(pDBUnit->mutable_pos()->nposx());
			pPos->set_nposy(pDBUnit->mutable_pos()->nposy());
			pFUnit->set_nherograde(pDBUnit->nherograde());
			pFUnit->set_nherolv(pDBUnit->nherolv());
		}
		//英雄技能
		Pve_GuildBtlHeroSkill* pDBHeroSkill = pDBDataUnit->mutable_pvefightheroskill();
		if(pDBHeroSkill&& pDBHeroSkill->heroid()>0)
		{
			Pve_HeroSkill* pHeroSkill = pDataUnit->mutable_pveheroskill_self();
			pHeroSkill->set_heroid(pDBHeroSkill->heroid());
			pHeroSkill->set_level(pDBHeroSkill->level());
			pHeroSkill->set_skillid1(pDBHeroSkill->skillid1());
			pHeroSkill->set_skillid2(pDBHeroSkill->skillid2());
			pHeroSkill->set_skill1grade(pDBHeroSkill->skill1grade());
			pHeroSkill->set_skill2grade(pDBHeroSkill->skill2grade());
			pHeroSkill->set_skill1quality(pDBHeroSkill->skill1quality());
			pHeroSkill->set_skill2quality(pDBHeroSkill->skill2quality());
		}
		//将领
		DB_C_GuildBtlAdmiralAdd* pDBAdmiralAdd = pDBDataUnit->mutable_pvefightadmiral();
		Pve_AdmiralAdd* pAdmiralAdd = pDataUnit->mutable_pvefightadmiral_self();
		for(int j=0;j<pDBAdmiralAdd->naddpctfoot_size();j++)
		{
			pAdmiralAdd->add_naddpctfoot(pDBAdmiralAdd->naddpctfoot(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpcttank_size();j++)
		{
			pAdmiralAdd->add_naddpcttank(pDBAdmiralAdd->naddpcttank(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpctplane_size();j++)
		{
			pAdmiralAdd->add_naddpctplane(pDBAdmiralAdd->naddpctplane(j));
		}
		for(int j=0;j<pDBAdmiralAdd->naddpctbuild_size();j++)
		{
			pAdmiralAdd->add_naddpctbuild(pDBAdmiralAdd->naddpctbuild(j));
		}
		DB_C_GuildBtlGeneral* pDBGeneral = pDBAdmiralAdd->mutable_fightgeneral();
		PVEFightGeneral* pGeneral = pAdmiralAdd->mutable_fightgeneral();
		pGeneral->set_ngeneralid(pDBGeneral->ngeneralid());
		pGeneral->set_ngenerallvl(pDBGeneral->ngenerallvl());
		pGeneral->set_ngeneraltrainlvl(pDBGeneral->ngeneraltrainlvl());
		pGeneral->set_ngeneralatk(pDBGeneral->ngeneralatk());
		pGeneral->set_ngeneraldef(pDBGeneral->ngeneraldef());
		pGeneral->set_ngeneralhp(pDBGeneral->ngeneralhp());
		pGeneral->set_ngeneralcrt(pDBGeneral->ngeneralcrt());
		pGeneral->set_ngeneralscore(pDBGeneral->ngeneralscore());

        for (int mhIdx = 0; mhIdx < pDBGeneral->listgeneralmh_size(); mhIdx++)
        {
            DB_C_PVEFightMH *pDBMH = pDBGeneral->mutable_listgeneralmh(mhIdx);
            PVEFightMH *pMH = pGeneral->add_ngeneralmh();
            pMH->set_nmhid(pDBMH->nmhid());
            pMH->set_nmhlv(pDBMH->nmhlv());
        }
	}
}

Event*	GuildBtlDBHandler::AddEventRequest(int64 nUserID,GuildBtlDBType emType)
{
	if (m_pEventHandler == NULL)
	{
		return NULL;
	}
	Event *pEvent = NULL;
	switch(emType)
	{
	case GuildBtlDB_USER:
		{
			pEvent = m_pEventHandler->getEventQueue()->allocateEvent();
			pEvent->set_cmd(EVENT_GUILDBTL_CACU);
			pEvent->set_state(Status_Normal_Game);
			pEvent->set_time(0);
			pEvent->set_uid(nUserID);
		}
		break;
	case GuildBtlDB_RESULT:
		{
			pEvent = m_pEventHandler->getEventQueue()->allocateEvent();
			pEvent->set_cmd(EVENT_GUILDBTL_SEEREPORT);
			pEvent->set_state(Status_Normal_Game);
			pEvent->set_time(0);
			pEvent->set_uid(nUserID);
		}
        break;
	case GuildGvgDB_USER:
		{
			pEvent = m_pEventHandler->getEventQueue()->allocateEvent();
			pEvent->set_cmd(EVENT_GVGBTL_CACU);
			pEvent->set_state(Status_Normal_Game);
			pEvent->set_time(0);
			pEvent->set_uid(nUserID);
		}
		break;
	case GuildGvgDB_RESULT:
		{
			pEvent = m_pEventHandler->getEventQueue()->allocateEvent();
			pEvent->set_cmd(EVENT_GVGBTL_SEEREPORT);
			pEvent->set_state(Status_Normal_Game);
			pEvent->set_time(0);
			pEvent->set_uid(nUserID);
		}
		break;
	default:
		break;
	}
	if(pEvent==NULL)
		return NULL;


	return pEvent;
}



bool GuildBtlDBHandler::LoadGuildBtlInfo(int64 nUserID,string strKey, string& strInfo)
{

	int     nDbStarId	= GetDBGuildBtlId(nUserID);
	TCRDB*  pConn		= GetDBGuildBtl(nDbStarId);
	while (pConn == NULL)
	{//不停的连接
		LOG4CXX_ERROR(logger_, "GuildBtlDBHandler Connect DB ERROR , userID"<<nUserID<<",strKey:"<<strKey.c_str());
		usleep(1000);
		pConn	= GetDBGuildBtl(nDbStarId);
		//return false;
	}

	int len  = 0;
	char* buffer = (char*) tcrdbget(pConn, strKey.c_str(), strKey.length() , &len);
	while (buffer == NULL)
	{
		int ecode = tcrdbecode(pConn);
		if (ecode != TTENOREC)
		{
			int ecode = tcrdbecode(pConn);
			LOG4CXX_ERROR(logger_, "Put recordDB ERROR : " << tcrdberrmsg(ecode));
			tcrdbclose(pConn);
			tcrdbdel(pConn);
			m_mapDBGuildBtl[nDbStarId] = NULL;

			//不停的读取
			usleep(1000);
			pConn = NULL;
			while (pConn == NULL)
			{//不断的连接
				LOG4CXX_ERROR(logger_, "GuildBtlDBHandler Connect DB ERROR , userID"<<nUserID<<",strKey:"<<strKey.c_str());
				usleep(1000);
				pConn	= GetDBGuildBtl(nDbStarId);
			}	
			len  = 0;
			buffer = (char*) tcrdbget(pConn, strKey.c_str(), strKey.length() , &len);
		}
		else
			return false;
	}

	std::string outbuf(buffer, len);
	strInfo = outbuf;

	free(buffer);
	return true;
}

bool GuildBtlDBHandler::SaveGuildBtlInfo(int64 nUserID,string strKey, string& strInfo)
{
	int     nDbStarId	= GetDBGuildBtlId(nUserID);
	TCRDB*  pConn		= GetDBGuildBtl(nDbStarId);
	while (pConn == NULL)
	{//不停的连接
		LOG4CXX_ERROR(logger_, "GuildBtlDBHandler Connect DB ERROR , userID"<<nUserID<<",strKey:"<<strKey.c_str());
		usleep(1000);
		pConn	= GetDBGuildBtl(nDbStarId);
		//return false;
	}

	while (!tcrdbput(pConn, strKey.c_str(), strKey.length() , strInfo.c_str(), strInfo.length()))
	{//写不进去不停地写
		int ecode = tcrdbecode(pConn);
		LOG4CXX_ERROR(logger_, "Put recordDB ERROR : " << tcrdberrmsg(ecode));
		tcrdbclose(pConn);
		tcrdbdel(pConn);
		m_mapDBGuildBtl[nDbStarId] = NULL;
		
		usleep(1000);
		pConn = NULL;
		while (pConn == NULL)
		{//不断的连接
			LOG4CXX_ERROR(logger_, "GuildBtlDBHandler Connect DB ERROR , userID"<<nUserID<<",strKey:"<<strKey.c_str());
			usleep(1000);
			pConn	= GetDBGuildBtl(nDbStarId);
			//return false;
		}
		//SetEnable(false);
		//return false;
	}

	return true;
}

int GuildBtlDBHandler::GetDBGuildBtlId(int64 uid)
{
	int hash = 0;
	if(uid > 10000000000)
	{
		hash = getUidHash(uid);
	}
	else
	{//工会战斗结果
		hash = uid;
	}
	int nDbStarId = hash % GuildBtlDBCfg::Instance().GuildBtlDbNum() + 1;
	return nDbStarId;
}

TCRDB* GuildBtlDBHandler::GetDBGuildBtl(int nDbStarId)
{
	TCRDB* pDbStar = (TCRDB*) (m_mapDBGuildBtl[nDbStarId]);
	if (pDbStar == NULL)
	{
		pDbStar = tcrdbnew();
		string	strAddr = GuildBtlDBCfg::Instance().GuildBtlDBAddr(nDbStarId);
		int		nPort	= GuildBtlDBCfg::Instance().GuildBtlDBPort(nDbStarId);
		if (tcrdbopen(pDbStar, strAddr.c_str() , nPort))
		{
			m_mapDBGuildBtl[nDbStarId] = pDbStar;
		}
		else
		{
			pDbStar = NULL;
		}
	}
	return pDbStar;
}

