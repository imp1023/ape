#pragma once
#include <map>
#include <list>
#include <pthread.h>
#include "../common/const_def.h"
#ifdef _WIN32
#include "../common/tcrdb_win.h"
#include <windows.h>
#else
#include <log4cxx/logger.h>
#include <tcrdb.h>
#endif
using namespace std;

class User;

class DB_FrdInfoLite;

//class MessageGet
//{
//public:
//	MessageGet(){};
//	~MessageGet(){};
//
//	int64	UidFrom() const { return m_llUidFrom; }
//	void	UidFrom(int64 val) { m_llUidFrom = val; }
//
//	string	PlatId() const { return m_strPlatId; }
//	void	PlatId(string val) { m_strPlatId = val; }
//
//	int		PlatType() const { return m_nPlatType; }
//	void	PlatType(int val) { m_nPlatType = val; }
//
//	int		Region() const {return m_nRegion;}
//	void	Region(int val){m_nRegion = val;}
//protected:
//	int64	m_llUidFrom;
//	string	m_strPlatId;
//	int		m_nPlatType;
//	int		m_nRegion;
//};

class MessagePut
{
public:
	MessagePut(){};
	~MessagePut(){};

	string	PlatId() const { return m_strPlatId; }
	void	PlatId(string val) { m_strPlatId = val; }

	string& FriendInfo(){ return m_strFriendInfo; }
	void	FriendInfo(const string& val) { m_strFriendInfo = val; }
protected:
	string	m_strPlatId;
public:
	string	m_strFriendInfo;
};

class Event;
class FriendInfoHandler
{
public:
	FriendInfoHandler(void);
	~FriendInfoHandler(void);

	void	initThread();
	void	termThread();
	void	Run();

	//bool	FriendInfoGet(int64 llUid, string& strPlatId, int nPlatType, string& strFriendInfo);
	//bool	FriendInfoPut(string& strPlatId, int nPlatType, string& strFriendInfo);

	//bool	SafePushFriendRequest(int64 llUid, string& strPlatId, int nPlatType,int nRegion);
	bool	SafePushFriendUpdate(const string& strPlatId,const DB_FrdInfoLite * pLite);

	bool	CanUse();
	void	SetEnable(bool bEnable);

	//void	SetEventHandler(GameEventHandler* eh);
	//GameEventHandler* GetEventHandler(void);

	static FriendInfoHandler& Instance()
	{
		static FriendInfoHandler inst;
		return inst;
	}
protected:
	static void* FriendInfoProc(void* arg);

	//Event*	AddFriendRequest(int64 llUidFrom, const string& strPlatId, int nPlatType,int nRegion);

	//bool	GetUserInfo(string strPlatId, int nPlatType, string& strFriendInfo);
	bool	PutUserInfo(string strPlatId, string& strFriendInfo);

	int     GetDBFriendId(const std::string& openid);
	TCRDB*	GetDBFiend(int nDbFriendId);

	//void	LockMessageGet();
	//void	UnLockMessageGet();	

	void	LockMessagePut();
	void	UnLockMessagePut();	
protected:

	bool				m_bEnable;

	map<int,void *>		m_mapDbFriend;

	//list<MessageGet*>	m_listMessageGet;
	list<MessagePut*>	m_listMessagePut;

	//pthread_mutex_t		m_mutexMessageGet;
	pthread_mutex_t		m_mutexMessagePut;

	pthread_t			m_threadFriendInfo;
	bool				m_bRunning;
};

//////////////////inline////////////////////
//inline
//void FriendInfoHandler::SetEventHandler(GameEventHandler* pEventHandler)
//{
//	m_pEventHandler = pEventHandler;
//}
//
//inline
//GameEventHandler*	FriendInfoHandler::GetEventHandler(void)
//{
//	return m_pEventHandler;
//}

inline
void FriendInfoHandler::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

//inline 
//void FriendInfoHandler::LockMessageGet()
//{
//	pthread_mutex_lock(&m_mutexMessageGet);
//}

//inline
//void FriendInfoHandler::UnLockMessageGet()
//{
//	pthread_mutex_unlock(&m_mutexMessageGet);
//}

inline 
void FriendInfoHandler::LockMessagePut()
{
	pthread_mutex_lock(&m_mutexMessagePut);
}

inline
void FriendInfoHandler::UnLockMessagePut()
{
	pthread_mutex_unlock(&m_mutexMessagePut);
}
