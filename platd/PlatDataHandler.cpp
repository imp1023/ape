#define _CRT_SECURE_NO_WARNINGS

#include "../event/DataHandler.h"

#include "PlatDataHandler.h"
#include "PlatDataSaver.h"
#include "PlatEventHandler.h"
#include "MemCacheServerHandler.h"
#include <algorithm>

#include <math.h>
#include "platDB.pb.h"
#include "../logic/friendDB.pb.h"
#include "../common/Clock.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"
#include "FriendInfoHandler.h"
using namespace std;


PlatDataHandler::PlatDataHandler(int nid) : DataHandler(nid)
{
    nid_ = nid;
    ds_ = new PlatDataSaver(nid);

	game_status_.resize(serverConfig.gamedNum()+1, NOT_CONNECTED);
	game_status_[0] = NORMAL;
	game_all_ready_ = false;

    revision_ = 1;
    logger_ = log4cxx::Logger::getLogger("PlatDataHandler");


    next_user_id_ = 10000000;
    next_item_id_ = 1;

	load_list_.clear();
	m_mapInviteFrd.clear();


    loadAllocateSetting();

	map<string,DB_Plat*>::iterator iter;

	for (iter = platid_uid_.begin();iter!=platid_uid_.end();++iter)
	{
		delete iter->second;
		iter->second = NULL;
	}
    platid_uid_.clear();
	//m_mapPlatRMRevision.clear();
	//m_listRemove.clear();
	//m_mapRemove.clear();


    pthread_mutex_init(&data_mutex_, NULL);
    timeUpdate			= time(NULL);
	m_timeRemoveUpdate	= Clock::getCurrentSystemTime();
	m_ltStart			= Clock::getCurrentSystemTime();
}

PlatDataHandler::~PlatDataHandler(void)
{

	map<string,DB_Plat*>::iterator iterPlat;
	for (iterPlat = platid_uid_.begin();iterPlat!=platid_uid_.end();++iterPlat)
	{
		delete iterPlat->second;
		iterPlat->second = NULL;
	}
	platid_uid_.clear();
	//m_mapPlatRMRevision.clear();
	//m_listRemove.clear();
	//m_mapRemove.clear();

    acquireDataLock();


    releaseDataLock();

    delete ds_;

	pthread_mutex_destroy(&data_mutex_);
}

void PlatDataHandler::loadAllocateSetting()
{
    ds_->loadAllocateSetting(next_user_id_, next_item_id_);
    int step = 10000;
    if (next_user_id_ > 1)
    {
        next_user_id_ += step;
    }
    if (next_item_id_ > 1)
    {
        next_item_id_ += step;
    }
    ds_->saveAllocateSetting(next_user_id_, next_item_id_);
}

void PlatDataHandler::saveAllocateSetting()
{
    ds_->saveAllocateSetting(next_user_id_, next_item_id_);
}

int64 PlatDataHandler::allocateUserID(const string& platid,int nRegion)
{
    int hash = getPlatidHash(platid);
    int64 uid = next_user_id_;
    next_user_id_ ++;
	int nSrvID = nid_%1024;
	//int nPhysicsRegionID = serverConfig.GetPhysiceRegion(nid_);
	int nPhysicsRegionID = serverConfig.GetPhysicsRegionByLogicRegion(nRegion);
	
    //uid |= (((int64) (nSrvID & 0xffff)) << 48) | (((int64) (hash & 0xffff)) << 32);

	uid |= (((int64) (nPhysicsRegionID & 0xffff)) << 58) |(((int64) (nSrvID & 0xffff)) << 48) | (((int64) (hash & 0xffff)) << 32);

	return uid;
}

int64 PlatDataHandler::allocateItemID(int num)
{
    int64 iid = next_item_id_;
    next_item_id_ += num;
    iid |= ((int64) (nid_ & 0xffff)) << 48;
    //iid |= ((int64)(nid_ &0x7f)) << 24;
    return iid;
}

void PlatDataHandler::CheckDBPlat(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL)
		return;
	for(int i=pDBPlat->name_size();i<PLAT_TYPE_MAX;i++)
	{
		pDBPlat->add_name("");
	}
	for (int i=pDBPlat->profile_link_size();i<PLAT_TYPE_MAX;i++)
	{
		pDBPlat->add_profile_link("");
	}
}

void PlatDataHandler::mapPlatidToUid(const string& pid,DB_Plat* pDBPlat)
{
	bool bPushLst = false;
	map<string, DB_Plat*>::iterator iter = platid_uid_.find(pid);
	if (iter != platid_uid_.end())
	{
		if(iter->second==NULL)
		{
			platid_uid_[pid] = pDBPlat;
			bPushLst = true;
		}
		else
		{
			delete pDBPlat;
			pDBPlat = NULL;
		}
	}
	else
	{
		platid_uid_[pid] = pDBPlat;
		bPushLst = true;
	}

	map<string, LoadStatus>::iterator load_iter = load_list_.find(pid);
	if (load_iter != load_list_.end())
	{
		load_list_.erase(load_iter);
		bPushLst = true;
	}

	//if(bPushLst)
	//{
	//	if(m_mapRemove.find(pid) == m_mapRemove.end())
	//	{
	//		m_listRemove.push_back(pid);
	//		m_mapRemove[pid] = 1;
	//	}
	//}
}


void PlatDataHandler::tick()
{
    updatePlatidMap();
	saveAllPlatData();
	MemCacheServerHandler::SaveAllPlatData(revision_);

	UpdateRemovePlat_New();
	//UpdateRemovePlat();

	//MemCacheServerHandler::UpdateRemoveUser();
}
//void PlatDataHandler::PushRemovePlat(string strPlatID,time_t ltRevision)
//{
//	if(m_strRemoveKey.size()<2)
//	{
//		m_strRemoveKey = strPlatID;
//	}
//	if(m_mapPlatRMRevision.find(strPlatID) == m_mapPlatRMRevision.end())
//	{
//		m_listRemove.push_back(strPlatID);
//	}
//	m_mapPlatRMRevision[strPlatID] = ltRevision;
//}

void PlatDataHandler::quit()
{
	MemCacheServerHandler::SaveAllPlatData(revision_,true);
	//saveAllUserData(true);
	saveAllPlatData(true);
}

void PlatDataHandler::termsThread()
{
	ds_->termThreads();
	exit(0);
}

void PlatDataHandler::UpdateRemovePlat_New()
{

	if (GameSrvCfg::Instance().PlatFreeEnableFlag() == 0)
	{
		return;
	}
	time_t timeNow = Clock::getCurrentSystemTime();

	if(m_ltStart + GameSrvCfg::Instance().PlatFreeMaxUserTime() > timeNow)
	{//服务器刚启动，所有玩家都不会删除
		return;
	}

	if (timeNow - m_timeRemoveUpdate < GameSrvCfg::Instance().PlatFreeUpdateTime())
	{
		return;
	}

	int nPlatSize = platid_uid_.size();
	if (nPlatSize <  GameSrvCfg::Instance().PlatFreeMaxUserCnt())
	{
		m_timeRemoveUpdate = Clock::getCurrentSystemTime();
		return;
	}
	map<string, DB_Plat*>::iterator iter,iterOld; 
	iter = platid_uid_.find(m_strRemoveKey);
	if(iter == platid_uid_.end())
	{
		iter = platid_uid_.begin();
	}
	int64 nTM1=0,nTM2=0,nTM3=0,nTM4=0,nTM5=0;
	int nCnt1=0,nCnt2=0,nCnt3=0,nCnt4=0;
	int64 nTmFlag = timeNow;
	int64 nTmFlag2 = 0;
	bool bFree = false;
	int nFlag = -1;
	time_t time_last = Clock::getCurrentSystemTime();

	int nFreeCnt = 0;
	while(iter != platid_uid_.end())
	{
		DB_Plat* pDBPlat = iter->second;

		nTmFlag2 = Clock::getCurrentSystemTime();
		nTM1 += nTmFlag2 - nTmFlag;
		nTmFlag = nTmFlag2;

		if(nCnt1>200||nCnt2>200||nCnt3>200)
		{
			time_last = Clock::getCurrentSystemTime();
			if (time_last - timeNow > 80)
			{
				nFlag = 2;
				break;
			}
		}
		if(pDBPlat== NULL)
		{
			iter++;
			nCnt1++;
			continue;
		}
		if (pDBPlat->ltrevision()<=m_ltStart||timeNow <  pDBPlat->ltrevision() + GameSrvCfg::Instance().PlatFreeMaxUserTime())
		{
			iter++;
			nCnt2++;
			continue;
		}

		string strPlatID = iter->first;
		if(dirty_platlst_.find(strPlatID) != dirty_platlst_.end())
		{
			iter++;
			nCnt3++;
			continue;
		}

		iterOld = iter;
		iter++;
		platid_uid_.erase(iterOld);
		delete pDBPlat;
		pDBPlat = NULL; 
		nFreeCnt++;


		nTmFlag2 = Clock::getCurrentSystemTime();
		nTM2 += nTmFlag2 - nTmFlag;
		nTmFlag = nTmFlag2;


		//SYS_LOG(0,LT_RemovePlat,0,0,2<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<nTM1<<nTM2<<nTM3<<nTM4<<nTM5);


		if (nFreeCnt >= GameSrvCfg::Instance().PlatFreeCnt())
		{
			nFlag = 0;
			break;
		}

		time_last = Clock::getCurrentSystemTime();
		if (time_last - timeNow > 80)
		{
			nFlag = 1;
			break;
		}

	}
	if(iter != platid_uid_.end())
	{
		m_strRemoveKey = iter->first;
	}
	else
	{
		m_strRemoveKey = "";
	}
	SYS_LOG(0,LT_RemovePlat,0,0,nFlag<<nFreeCnt<<(int)platid_uid_.size()<<nCnt1<<nCnt2<<nCnt3<<"-----------"<<nTM1<<nTM2<<m_strRemoveKey);

	
	m_timeRemoveUpdate = Clock::getCurrentSystemTime();
}

//void PlatDataHandler::UpdateRemovePlat()
//{
//	return;
//	if (GameSrvCfg::Instance().PlatFreeEnableFlag() == 0)
//	{
//		return;
//	}
//
//	time_t timeNow = Clock::getCurrentSystemTime();
//	
//	if(m_ltStart + GameSrvCfg::Instance().PlatFreeMaxUserTime() > timeNow)
//	{//服务器刚启动，所有玩家都不会删除
//		return;
//	}
//	if (timeNow - m_timeRemoveUpdate < GameSrvCfg::Instance().PlatFreeUpdateTime())
//	{
//		return;
//	}
//
//	int nPlatSize = platid_uid_.size();
//	if (nPlatSize <  GameSrvCfg::Instance().PlatFreeMaxUserCnt())
//	{
//		m_timeRemoveUpdate = Clock::getCurrentSystemTime();
//		return;
//	}
//  
//
//	list<string> lstPushPlat;
//	lstPushPlat.clear();
//	int nFreeCnt = 0;
//	int64 nTM1=0,nTM2=0,nTM3=0,nTM4=0,nTM5=0;
//	int nCnt1,nCnt2,nCnt3,nCnt4;
//	int64 nTmFlag = timeNow;
//	int64 nTmFlag2 = 0;
//	nCnt1 = 0;
//	nCnt2 = 0;
//	nCnt3 = 0;
//	nCnt4 = 0;
//	bool bFree = false;
//	hash_map<string,int>::iterator iter_Rm;
//	hash_map<string,int64>::iterator itRevision;
//	map<string, DB_Plat*>::iterator iter_plat; 
//	while(m_listRemove.size()>0)
//	{
//
//		nTmFlag2 = Clock::getCurrentSystemTime();
//		nTM1 += nTmFlag2 - nTmFlag;
//		nTmFlag = nTmFlag2;
//
//		string platID = m_listRemove.front();
//		m_listRemove.pop_front();
//
//		itRevision = m_mapPlatRMRevision.find(platID);
//		if(itRevision == m_mapPlatRMRevision.end())
//		{
//			continue;
//		}
//		int64 lRMRevision = itRevision->second;
//		if (lRMRevision<=0||timeNow <  lRMRevision + GameSrvCfg::Instance().PlatFreeMaxUserTime())
//		{
//			nCnt1 ++;
//			lstPushPlat.push_back(platID);
//			continue;
//		}
//
//		iter_plat = platid_uid_.find(platID);
//		if (iter_plat == platid_uid_.end() )
//		{//error
//			m_mapPlatRMRevision.erase(itRevision);
//			nCnt2 ++;
//			continue;
//		}
//
//		nTmFlag2 = Clock::getCurrentSystemTime();
//		nTM2 += nTmFlag2 - nTmFlag;
//		nTmFlag = nTmFlag2;
//
//
//		DB_Plat* pDBPlat = iter_plat->second;
//		if (pDBPlat == NULL)
//		{
//			m_mapPlatRMRevision.erase(itRevision);
//			nCnt3 ++;
//			continue;
//		}
//
//		nTmFlag2 = Clock::getCurrentSystemTime();
//		nTM3 += nTmFlag2 - nTmFlag;
//		nTmFlag = nTmFlag2;
//
//
//		if(dirty_platlst_.find(platID) != dirty_platlst_.end())
//		{
//			nCnt4 ++;
//			lstPushPlat.push_back(platID);
//			continue;
//		}
//
//		nTmFlag2 = Clock::getCurrentSystemTime();
//		nTM4 += nTmFlag2 - nTmFlag;
//		nTmFlag = nTmFlag2;
//
//
//		nFreeCnt++;
//		m_mapPlatRMRevision.erase(itRevision);
//
//		platid_uid_.erase(iter_plat);
//		delete pDBPlat;
//		pDBPlat = NULL; 
//		
//		nTmFlag2 = Clock::getCurrentSystemTime();
//		nTM5 += nTmFlag2 - nTmFlag;
//		nTmFlag = nTmFlag2;
//
//		//SYS_LOG(0,LT_RemovePlat,0,0,2<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<nTM1<<nTM2<<nTM3<<nTM4<<nTM5);
//
//		if (nFreeCnt >= GameSrvCfg::Instance().PlatFreeCnt())
//		{
//			bFree = true;
//			SYS_LOG(0,LT_RemovePlat,0,0,0<<nFreeCnt<<(int)platid_uid_.size()<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<nTM1<<nTM2<<nTM3<<nTM4<<nTM5);
//			break;
//		}
//
//		time_t time_last = Clock::getCurrentSystemTime();
//		if (time_last - timeNow > 80)
//		{
//			bFree = true;
//			SYS_LOG(0,LT_RemovePlat,0,0,1<<nFreeCnt<<(int)platid_uid_.size()<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<nTM1<<nTM2<<nTM3<<nTM4<<nTM5);
//			break;
//		}
//	}
//
//	if(!bFree)
//	{
//		SYS_LOG(0,LT_RemovePlat,0,0,4<<(int)lstPushPlat.size()<<(int)m_listRemove.size()<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<nTM1<<nTM2<<nTM3<<nTM4<<nTM5);
//	}
//
//	m_listRemove.insert(m_listRemove.end(),lstPushPlat.begin(),lstPushPlat.end());
//	lstPushPlat.clear();
//
//	m_timeRemoveUpdate = Clock::getCurrentSystemTime();
//}

bool PlatDataHandler::LoadPlatFromMemCache(const string& pid)
{
	string infostr("");

	bool hasInfo = MemCacheServerHandler::GetPlatInfo( pid, infostr);
	if (hasInfo)
	{
		if(infostr.size()<4 || infostr.substr(0,4)!="new:")
		{
			return false;
		}
		infostr = infostr.substr(4);
		DB_Plat* pDBPlat = new DB_Plat();
		if (pDBPlat->ParseFromString( infostr )&&pDBPlat->has_platform_id())
		{
			updateLoadPlatid(pid, pDBPlat);
			return true;
		}
		else
		{
			delete pDBPlat;
			pDBPlat = NULL;
			LOG4CXX_ERROR(logger_, "GameDataHandler::LoadPlatFromMemCache() ERROR!!! ParseFromString platID:" << pid);
			return false;
		}
	}
	return false;
}

void PlatDataHandler::updatePlatidMap()
{
    //acquireDataLock();
    //map<string, int64>::iterator iter = loaded_platid_.begin();
    //while (iter != loaded_platid_.end())
    //{
    //    map<string, int64>::iterator oiter = iter;
    //    ++iter;
    //    string pid = oiter->first;
    //    int64 uid = oiter->second;
    //    loaded_platid_.erase(oiter);

    //    LOG4CXX_DEBUG(logger_, "Update user map platid=" << pid << " ,uid=" << uid);
    //    mapPlatidToUid(pid, uid);
    //}
    //releaseDataLock();
	acquireDataLock();
	map<string, DB_Plat*>::iterator iter = loaded_platid_.begin();
	while (iter != loaded_platid_.end())
	{
		map<string, DB_Plat*>::iterator oiter = iter;
		++iter;
		string pid = oiter->first;
		DB_Plat* pDBPlat = oiter->second;
		//if (pDBPlat == NULL)
		//	continue;
		loaded_platid_.erase(oiter);
		if(pDBPlat)
		{
			pDBPlat->set_ltrevision(revision_);
		}
		mapPlatidToUid(pid,pDBPlat);
	}
	loaded_platid_.clear();
	releaseDataLock();
}

void PlatDataHandler::updateLoadPlatid(const string& pid,DB_Plat* pDBPlat)
{//pDBPlat可能为空，表示无此用户
	acquireDataLock();

	map<string, DB_Plat*>::iterator iter = loaded_platid_.find(pid);
	if (iter != loaded_platid_.end())
	{
		if(pDBPlat != NULL)
		{
			if(iter->second  == NULL)
			{
				loaded_platid_[pid] = pDBPlat;
			}	
			else
			{
				delete pDBPlat;
			}
		}	
	}
	else
	{
		loaded_platid_.insert(make_pair(pid, pDBPlat));
	}

	releaseDataLock();
}

int64 PlatDataHandler::GetUid(DB_Plat* pDBPlat,int nRegion)
{
	if (NULL == pDBPlat)
	{
		return -1;
	}

	int nSize = pDBPlat->platregion_size();
	for (int i=0;i<nSize;i++)
	{
		const DB_PlatRegion& platidinfo = pDBPlat->platregion(i);
		if (platidinfo.srvregion() == nRegion )
		{
			return platidinfo.uid();
		}
	}

	// 不在合区分组中
	int nGroupid = serverConfig.getgroupidbyregion(nRegion);

	// 检测分组内的其他区是否有角色，有角色就返回该区
	int ngroupsize = serverConfig.getgroupsize(nGroupid);
	for (int i=0;i<ngroupsize;i++)
	{
		int nsubregion = serverConfig.getregionfromgroup(nGroupid,i);
		if (-1 != nsubregion && nsubregion != nRegion)
		{
			for (int j=0;j<nSize;j++)
			{
				const DB_PlatRegion& platidinfo = pDBPlat->platregion(j);
				if (platidinfo.srvregion() == nsubregion)
				{
					return platidinfo.uid();
				}
			}
		}
	}

	return 0;
	//return pDBPlat->lastuid();
}

void PlatDataHandler::markPlatDirty(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL)
		return;
	string pid = pDBPlat->platform_id();
	if(pid.size()<=1)
	{
		return;
	}



	map<string, DB_Plat*>::iterator iter = dirty_platlst_.find(pid);
	if(iter == dirty_platlst_.end())
	{
		//PushRemovePlat(pid,revision_);
		pDBPlat->set_ltrevision(revision_);
		pDBPlat->set_ltsaverevision(revision_);
		dirty_platlst_.insert(make_pair(pid, pDBPlat));
		//dirty_platlst_revision_[pid] = revision_;

	}

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<string,DB_Plat*> & mem_plat_list = MemCacheServerHandler::GetInst()->GetPlatList();
		map<string, DB_Plat*>::iterator iter = mem_plat_list.find(pid);
		if (iter == mem_plat_list.end())
		{
			MemCacheServerHandler::SafePushPlat(pid,pDBPlat,revision_);
			LOG4CXX_DEBUG(logger_, "Marking mem_plat dirty: uid=" << pid << ", revision" << revision_);
		}
	}


}


void PlatDataHandler::saveAllPlatData(bool urgent)
{
    if (!urgent && revision_ < timeUpdate + 10 * 1000)
    {
        return;
    }
	ds_->safeSaveAllMap(revision_, this, urgent);
	time_t ltNow = revision_/1000;
	tm date = *localtime(&ltNow);
	if(date.tm_hour==4&&date.tm_min<6)
	{//每天早上四点的时候，备份数据库DB1，这个时候，save allocate会锁死线程
	}
	else
	{
		saveAllocateSetting();
	}
   // saveAllocateSetting();
    timeUpdate = revision_;
}

void PlatDataHandler::savePlatidMap(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL||!pDBPlat->has_platform_id())
		return;
	ds_->safeSaveMap(this, pDBPlat);
}

void PlatDataHandler::loadPlatidMap(const string& pid)
{
	if(LoadPlatFromMemCache(pid))
		return;

    ds_->safeLoadMap(this, pid);
}


DB_Plat* PlatDataHandler::GetDBPlat(const string& pid,LoadStatus* status /* = NULL */,bool load/* = true */)
{
	LoadStatus tmp_status, *status_ = (status == NULL) ? &tmp_status : status;
	if (!isPlatidLocal(pid))
	{
		*status_ = LOAD_MISS;
		return NULL;
	}
	//m_mapPlatRMRevision[pid] = revision_;

	map<string, DB_Plat*>::iterator iter = platid_uid_.find(pid);
	if (iter != platid_uid_.end())
	{
		if (iter->second == NULL)
		{
			*status_ = LOAD_EMPTY;
			return NULL;
		}
		*status_ = LOAD_SUCCESS;
		DB_Plat* pDBPlat = iter->second;
		pDBPlat->set_ltrevision(revision_);
		//PushRemovePlat(pid,revision_);
		//return iter->second;
		return pDBPlat;
	}
	
	if(!load)
	{
		*status_ = LOAD_EMPTY;
		return NULL;
	}

	map<string, LoadStatus>::iterator load_iter = load_list_.find(pid);
	if (load_iter != load_list_.end())
	{
		*status_ = LOAD_WAITING;
		return NULL;
	}

	load_list_.insert(make_pair(pid,LOAD_WAITING));
	*status_ = LOAD_WAITING;
	loadPlatidMap(pid);
	return NULL;
}

int PlatDataHandler::getPlatSrvID(const string& pid)
{
	int hash = getPlatidHash(pid);
	return (hash % serverConfig.plat_num() + 1);
}

bool PlatDataHandler::isPlatidLocal(const string& platid)
{
    int hash = getPlatidHash(platid);
    return hash % serverConfig.plat_num() + 1 == nid_;
}

int PlatDataHandler::getGamedIdByUserId(int64 uid)
{ 
    int hash = getUidHash(uid);
	int nPhysicsRgn = getPhysicsRegion(uid);
	
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	nGameID += nPhysicsRgn *  serverConfig.physicsGameNum();
	return nGameID;
}

int PlatDataHandler::getGameIdfromPlatId(string const& platid,int nRegion)
{
	int hash = getPlatidHash(platid);
	int nPhysicsRgn = serverConfig.GetPhysicsRegionByLogicRegion(nRegion);
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	nGameID += nPhysicsRgn *  serverConfig.physicsGameNum();
	return nGameID;

}

GameWorkingStatus PlatDataHandler::setWorkingStatus(int gid, GameWorkingStatus status)
{
	GameWorkingStatus old_status = game_status_[gid];
	if(status == NORMAL)
	{
		game_status_[gid] = NORMAL;
		checkAllWorkingStatus();
	}
	else
	{
		game_status_[gid] = status;
		game_all_ready_ = false;
	}
	return old_status;
}

void PlatDataHandler::checkAllWorkingStatus()
{
	bool ready = true;
	for (size_t i = 0; i < game_status_.size(); i ++)
	{
		if (game_status_[i] != NORMAL)
		{
			ready = false;
			break;
		}
	}
	if (!game_all_ready_ && ready)
	{
		game_all_ready_ = true;
		LOG4CXX_INFO(logger_, "All game server already.");
	}
}

void PlatDataHandler::SaveFrd2TT(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL||!FriendInfoHandler::Instance().CanUse())
		return;
	DB_FrdInfoLite lite;
	lite.set_userid("0");
	//for(int i=0;i<PLAT_TYPE_MAX;i++)
	for(int i=0;i<pDBPlat->name_size()&&i<pDBPlat->profile_link_size();i++)
	{
		DB_FrdPlatInfo* pPlatInfo = lite.add_platinfo();
		pPlatInfo->set_name(pDBPlat->name(i));
		pPlatInfo->set_profile_link(pDBPlat->profile_link(i));
	}

	int lastRegion = pDBPlat->lastregion();
	for(int i=0;i<pDBPlat->platregion_size();i++)
	{
		const DB_PlatRegion& dbRgn = pDBPlat->platregion(i);
		DB_FrdRegionInfo* pDBRgn = lite.add_region();
		pDBRgn->set_uid(dbRgn.uid());
		pDBRgn->set_level(dbRgn.level());
		pDBRgn->set_region(dbRgn.srvregion());
		pDBRgn->set_lastlogontime(dbRgn.lastlogontime());
		pDBRgn->set_viplevel(dbRgn.viplevel());
		pDBRgn->set_viptype(dbRgn.viptype());
	}

	lite.set_is_yellow_vip(pDBPlat->isyellowdmd());
	lite.set_is_yellow_year_vip(pDBPlat->isyellowdmdyear());
	lite.set_yellow_vip_level(pDBPlat->yellowdmdlvl());

	lite.set_qqdmdlvl(pDBPlat->qqdmdlvl());
	lite.set_isqqdmd(pDBPlat->isqqdmd());
	lite.set_isqqdmdyear(pDBPlat->isqqdmdyear());

	lite.set_is_high_yellow_vip( pDBPlat->ishighyellowdmd() );

	lite.set_qhalldmdlvl(pDBPlat->qhalldmdlvl());
	lite.set_isqhalldmd(pDBPlat->isqhalldmd());
	lite.set_isqhalldmdyear(pDBPlat->isqhalldmdyear());
	lite.set_ishighqhalldmd(pDBPlat->ishighqhalldmd());
	lite.set_ishighqhalldmdyear(pDBPlat->ishighqhalldmdyear());

	FriendInfoHandler::Instance().SafePushFriendUpdate(pDBPlat->platform_id(),&lite);

}

int PlatDataHandler::GetUnDealGiftCnt(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL)
		return 0;
	int nCnt = 0;
	DB_PlatGift* pDBGiftInfo = pDBPlat->mutable_giftinfo();
	for(int i=0;i<pDBGiftInfo->getgiftinfo_size();i++)
	{
		DB_PlatGiftFrdInfo* pGiftInfo = pDBGiftInfo->mutable_getgiftinfo(i);
		if(pGiftInfo->messageid()<=0||pGiftInfo->friendid()<=0||pGiftInfo->gifttype()==Gift_Friend)//邀请好友礼物不计入
			continue;
		nCnt++;
	}
	return nCnt;
}

int PlatDataHandler::GetUnDealFrdGiftCnt(DB_Plat* pDBPlat)
{
	if(pDBPlat==NULL)
		return 0;
	int nCnt = 0;
	DB_PlatGift* pDBGiftInfo = pDBPlat->mutable_giftinfo();
	for(int i=0;i<pDBGiftInfo->getgiftinfo_size();i++)
	{
		DB_PlatGiftFrdInfo* pGiftInfo = pDBGiftInfo->mutable_getgiftinfo(i);
		if(pGiftInfo->messageid()<=0||pGiftInfo->friendid()<=0)//邀请好友礼物不计入
			continue;
		if ( pGiftInfo->gifttype()==Gift_Friend )
		{
			nCnt++;
		}
	}
	return nCnt;
}

bool  PlatDataHandler::HasInviteFrd(const string& inviteFrdPlat,const string& invitedFrdPlat)
{	
	string strKey;
	strKey = inviteFrdPlat + "_" + invitedFrdPlat;
	return m_mapInviteFrd[strKey] != 0;
}
void  PlatDataHandler::AddInviteFrd(const string& inviteFrdPlat,const string& invitedFrdPlat)
{
	string strKey;
	strKey = inviteFrdPlat + "_" + invitedFrdPlat;
	m_mapInviteFrd[strKey] = 1;

}

DB_PlatRegion* PlatDataHandler::GetDBPlatRegion(string strPlatid , int nRegionid , int& state)
{
	LoadStatus status = LOAD_INVALID;
	DB_Plat* pDBPlat = GetDBPlat(strPlatid , &status);
	if (pDBPlat == NULL)
	{
		if(LOAD_WAITING == status)
		{
			state = (int)LOAD_WAITING;
			return NULL;
		}
		else if (status == LOAD_EMPTY)
		{
			state = (int)LOAD_REGION_PLATERR;
			return NULL;
		}
	}
	//
	DB_PlatRegion* pDBRegion = NULL;
	for (int i = 0 ; i < pDBPlat->platregion_size() ; i++)
	{
		DB_PlatRegion* pOne = pDBPlat->mutable_platregion(i);
		if (pOne->srvregion() == nRegionid)
		{
			pDBRegion = pOne;
			break;
		}
	}
	if (NULL == pDBRegion)
	{
		state = (int)LOAD_REGION_REGIONERR;
		return NULL;
	}
	else
	{
		state = (int)LOAD_SUCCESS;
		return pDBRegion;
	}
}