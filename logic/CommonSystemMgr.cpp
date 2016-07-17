/********************************************************************
	created:	2015/12/29
	created:	29:12:2015   15:41
	filename: 	\Server\logic\CommonSystemMgr.cpp
	file path:	\Server\logic
	file base:	CommonSystemMgr
	file ext:	cpp
	author:		likun
	
	purpose:	
*********************************************************************/
#include <cmath>
#include "CommonSystemMgr.h"
#include <boost/typeof/typeof.hpp>
#include "../common/Clock.h"
#include "../common/DateTime.h"
#include "../common/string-util.h"
#include "../common/rand_util.h"
#include "../common/SysLog.h"
#include "../worldd/worldd.h"
#include "../worldd/WorldEventHandler.h"
#include "../event/event.pb.h"
#include "../event/EventDefine.h"
#include "../event/EventQueue.h"

#define RECONNECT_DB_OR_RETURN \
	do {\
	if (!m_pLocalDB) ConnectDB();\
	if (!m_pLocalDB) return;\
	} while(0)

#define RETURN_IF_NOT_COMMON_WORLD \
	do {\
	if(World::Instance().worldid() != COMMON_WORLD_ID) return;\
	} while (0)

#define RETURN_0_IF_NOT_COMMON_WORLD \
	do {\
	if(World::Instance().worldid() != COMMON_WORLD_ID) return 0;\
	} while (0)

/************************************************************************/
/*                                                                      */
/************************************************************************/

CommonSystemMgr::CommonSystemMgr()
: m_bDirty(false) 
, m_pLocalDB(NULL)
{
	RETURN_IF_NOT_COMMON_WORLD;
	m_logger = log4cxx::Logger::getLogger("CommonSystemMgr");
	//m_dbTuanGouData.Clear();
}

CommonSystemMgr::~CommonSystemMgr()
{
	RETURN_IF_NOT_COMMON_WORLD;
	ReleaseDB();
}

void CommonSystemMgr::Tick()
{
	RETURN_IF_NOT_COMMON_WORLD;
	if (m_bDirty && m_LastSave.elapsed() > 60)
	{
		EventQueue* pQueue = World::Instance().GetWorldEventHandler()->getEventQueue();
		Event* pEvent = pQueue->allocateEvent();
		pEvent->set_cmd(EVENT_SAVE_WORLD_TUANGOUMODUL_DATA);
		pEvent->set_state(World_to_Self);
		pEvent->set_time(-1);
		pQueue->safePushEvent(pEvent);
		m_bDirty = false;
		m_LastSave.update();
	}
}

void CommonSystemMgr::Load()
{
	RETURN_IF_NOT_COMMON_WORLD;
	RECONNECT_DB_OR_RETURN;
	int nLen = 0;
	char* pBuf = (char*)tcrdbget(m_pLocalDB, "TuanGouWorldData", strlen("TuanGouWorldData"), &nLen);

	if (NULL == pBuf)
	{
		int ecode = tcrdbecode(m_pLocalDB);
		LOG4CXX_ERROR(m_logger, "Get TuanGouWorldData : " << tcrdberrmsg(ecode));
		ReleaseDB();
		return;		
	}
	else
	{
		string strData(pBuf, nLen);
		//m_dbTuanGouData.ParseFromString(strData);
		free(pBuf);
	}

	//LOG4CXX_INFO(m_logger, "Load CommonSystemMgr Finish. version:" << m_dbTuanGouData.version() << " join number:" << m_dbTuanGouData.join_num());
}

void CommonSystemMgr::SaveAll()
{
	RETURN_IF_NOT_COMMON_WORLD;
	RECONNECT_DB_OR_RETURN;

	string strData = "";
	//m_dbTuanGouData.SerializeToString(&strData);
	const char* pBuf = strData.c_str();
	if (!tcrdbput(m_pLocalDB, "TuanGouWorldData", strlen("TuanGouWorldData"), pBuf, strData.length()))
	{
		int ecode = tcrdbecode(m_pLocalDB);
		LOG4CXX_ERROR(m_logger, "Save CommonSystemMgr ERROR : " << tcrdberrmsg(ecode));
		ReleaseDB();
	}
	//LOG4CXX_INFO(m_logger, "CommonSystemMgr Lenth:" << toString(strData.length()).c_str() << " join number:" << m_dbTuanGouData.join_num());
}

void CommonSystemMgr::MarkDirty()
{
	RETURN_IF_NOT_COMMON_WORLD;
	m_bDirty = true;
}
/*
void CommonSystemMgr::JoinTuanGouRequest(Event *e)
{
	RETURN_IF_NOT_COMMON_WORLD;
	TuanGouInfoEvent content;
	content.ParseFromString(e->content());
	content.set_number(INVALID_VALUE);
	if (m_dbTuanGouData.version()==content.version())
	{
		//srand((unsigned int)time(NULL));
		int nRand = (rand()%content.value()) + 1;
		m_dbTuanGouData.set_join_num(m_dbTuanGouData.join_num()+nRand);
		m_bDirty = true;
	}

	content.set_number(m_dbTuanGouData.join_num());
	content.set_version(m_dbTuanGouData.version());
	string strData = "";
	content.SerializeToString(&strData);
	e->set_content(strData);
	e->set_state(World_to_Game);
	WorldEventHandler* eh = World::Instance().GetWorldEventHandler();
	eh->sendEventToUser(e, e->uid());
}

void CommonSystemMgr::JoinTuanGouInformation(Event *e)
{
	RETURN_IF_NOT_COMMON_WORLD;
	TuanGouInfoEvent content;
	content.set_number(m_dbTuanGouData.join_num());
	content.set_version(m_dbTuanGouData.version());
	string strData = "";
	content.SerializeToString(&strData);
	e->set_content(strData);
	e->set_state(World_to_Game);
	WorldEventHandler* eh = World::Instance().GetWorldEventHandler();
	eh->sendEventToUser(e, e->uid());
}

void CommonSystemMgr::JoinTuanGouNumber(Event *e)
{
	RETURN_IF_NOT_COMMON_WORLD;
	TuanGouInfoEvent content;
	content.ParseFromString(e->content());
	content.set_number(m_dbTuanGouData.join_num());
	content.set_version(m_dbTuanGouData.version());
	string strData = "";
	content.SerializeToString(&strData);
	e->set_content(strData);
	e->set_state(World_to_Game);
	WorldEventHandler* eh = World::Instance().GetWorldEventHandler();
	eh->sendEventToGamed(e, content.value());
}

void CommonSystemMgr::ResetTuanGouData(Event *e)
{
	RETURN_IF_NOT_COMMON_WORLD;
	TuanGouInfoEvent content;
	content.ParseFromString(e->content());
	if (content.version()!=m_dbTuanGouData.version())
	{
		m_dbTuanGouData.set_version(content.version());
		m_dbTuanGouData.clear_join_num();
		m_dbTuanGouData.clear_join_time();
		m_bDirty = true;
	}

	string strData = "";
	content.SerializeToString(&strData);
	e->set_content(strData);
	e->set_state(World_to_Game);
	WorldEventHandler* eh = World::Instance().GetWorldEventHandler();
	eh->sendEventToGamed(e, content.value());
}
*/
void CommonSystemMgr::ConnectDB()
{
	RETURN_IF_NOT_COMMON_WORLD;
	m_pLocalDB = tcrdbnew();
	if (m_pLocalDB == NULL)
	{
		assert(false);
	}

	if (!tcrdbopen(m_pLocalDB, "127.0.0.1", ServerConfig::Instance().worlddLocaldbPort(World::Instance().worldid())))
	{
		LOG4CXX_ERROR(m_logger, "open local db error: " << tcrdberrmsg(tcrdbecode(m_pLocalDB)));
		tcrdbdel(m_pLocalDB);
		m_pLocalDB = NULL;
	}
}

void CommonSystemMgr::ReleaseDB()
{
	RETURN_IF_NOT_COMMON_WORLD;
	if (m_pLocalDB)
	{
		tcrdbclose(m_pLocalDB);
		tcrdbdel(m_pLocalDB);
	}
	m_pLocalDB = NULL;
}
