#include "FriendInfoHandler.h"
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include "../logic/friendDB.pb.h"
#include "../logic/FriendInfoCfg.h"
#include "../logic/GameSrvCfg.h"
#include "EventDefine.h"



FriendInfoHandler::FriendInfoHandler(void)
: m_bEnable(true)
, m_bRunning(false)
{
	//pthread_mutex_init(&m_mutexMessageGet, NULL);
	pthread_mutex_init(&m_mutexMessagePut, NULL);
}

FriendInfoHandler::~FriendInfoHandler(void)
{
	for (map<int,void *>::iterator iter = m_mapDbFriend.begin(); iter != m_mapDbFriend.end(); iter++)
	{
		TCRDB* pDbFriend = (TCRDB*) iter->second;
		tcrdbclose(pDbFriend);
		tcrdbdel(pDbFriend);
	}
	m_mapDbFriend.clear();

	//for (list<MessageGet*>::iterator iter = m_listMessageGet.begin(); iter != m_listMessageGet.end(); iter++)
	//{
	//	MessageGet *pMessageGet = (MessageGet*)*iter;
	//	if (pMessageGet != NULL)
	//	{
	//		delete pMessageGet;
	//	}
	//}
	//m_listMessageGet.clear();

	for (list<MessagePut*>::iterator iter = m_listMessagePut.begin(); iter != m_listMessagePut.end(); iter++)
	{
		MessagePut *pMessagePut = (MessagePut*)*iter;
		if (pMessagePut != NULL)
		{
			delete pMessagePut;
		}
	}
	m_listMessagePut.clear();

	//pthread_mutex_destroy(&m_mutexMessageGet);
	pthread_mutex_destroy(&m_mutexMessagePut);
}

void FriendInfoHandler::initThread()
{
	int ret = 0;
	if (!m_bRunning)
	{
		m_bRunning = true;
		ret = pthread_create(&m_threadFriendInfo, NULL, FriendInfoHandler::FriendInfoProc, (void *)this);
		if(ret != 0)
		{

		}
	}
}

void FriendInfoHandler::termThread()
{
	m_bRunning = false;
	pthread_join(m_threadFriendInfo, NULL);
}

void* FriendInfoHandler::FriendInfoProc(void* arg)
{
	FriendInfoHandler* pFriendInfoHandler = static_cast<FriendInfoHandler*>(arg);
	pFriendInfoHandler->Run();
	pthread_exit(NULL);
	return NULL;
}


void FriendInfoHandler::Run()
{
	//vector<MessageGet*> vecMessageGet;
	vector<MessagePut*>	vecMessagePut;
	//vecMessageGet.reserve(10240);
	vecMessagePut.reserve(10240);
	while(m_bRunning)
	{
		for (size_t i = 0; i < vecMessagePut.size(); i++)
		{
			MessagePut *pMessagePut = vecMessagePut.at(i);
			if (pMessagePut != NULL)
			{
				delete pMessagePut;
			}
		}
		vecMessagePut.clear();
		LockMessagePut();
		while (!m_listMessagePut.empty())
		{
			vecMessagePut.push_back(m_listMessagePut.front());
			m_listMessagePut.pop_front();
		}
		UnLockMessagePut();

		for (size_t i = 0; i < vecMessagePut.size(); i++)
		{
			MessagePut *pMessagePut = vecMessagePut.at(i);
			if (pMessagePut != NULL)
			{
				PutUserInfo(pMessagePut->PlatId(), pMessagePut->FriendInfo());
			}
		}
		usleep(1000);
	}
}


bool FriendInfoHandler::SafePushFriendUpdate(const string& strPlatId,const DB_FrdInfoLite * pLite)
{
	if(pLite==NULL||!CanUse())
		return false;
	
	MessagePut *pMessagePut = new MessagePut;
	pMessagePut->PlatId(strPlatId);
	pLite->SerializeToString(&(pMessagePut->m_strFriendInfo));

	LockMessagePut();
	m_listMessagePut.push_back(pMessagePut);
	UnLockMessagePut();
	return true;
}

bool FriendInfoHandler::PutUserInfo(string strPlatId, string& strFriendInfo)
{
	if (!CanUse())
	{
		return false;
	}
	int     nDbFriendId = GetDBFriendId(strPlatId);
	TCRDB*  pConn		= GetDBFiend(nDbFriendId);
	if (pConn == NULL)
	{
		SetEnable(false);
		return false;
	}

	const char* buf = strFriendInfo.c_str();
	int         len = strFriendInfo.length();
	std::string key = "Frd_" + strPlatId ;
	int klen = key.length();
	if (!tcrdbput(pConn, key.c_str(), klen, buf, len))
	{
		SetEnable(false);
		return false;
	}

	return true;
}

int FriendInfoHandler::GetDBFriendId(const std::string& openid)
{
	int hash = getPlatidHash(openid);
	int nDbFriendId = hash % FriendInfoCfg::Instance().FriendDbNum() + 1;
	return nDbFriendId;
}

TCRDB* FriendInfoHandler::GetDBFiend(int nDbFriendId)
{
	TCRDB* pDbFriend = (TCRDB*) (m_mapDbFriend[nDbFriendId]);
	if (pDbFriend == NULL)
	{
		pDbFriend = tcrdbnew();
		string	strAddr = FriendInfoCfg::Instance().FriendInfoAddr(nDbFriendId);
		int		nPort	= FriendInfoCfg::Instance().FriendInfoPort(nDbFriendId);
		if (tcrdbopen(pDbFriend, strAddr.c_str() , nPort))
		{
			m_mapDbFriend[nDbFriendId] = pDbFriend;
		}

	}
	return pDbFriend;
}

bool FriendInfoHandler::CanUse()
{
	int bInited = GameSrvCfg::Instance().FriendInfoEnable();
	return m_bEnable && bInited;
}
