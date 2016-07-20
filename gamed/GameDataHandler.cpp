#define _CRT_SECURE_NO_WARNINGS
#include "../gamed/MemCacheServerHandler.h"
#include "../event/DataHandler.h"

#include "GameDataHandler.h"
#include "GameDataSaver.h"
#include "GameEventHandler.h"

#include <algorithm>

#include <math.h>
#include "../logic/dbinterface.pb.h"
#include "../common/Clock.h"
#include "../common/distribution.h"
#include "../common/SysLog.h"
#include "../common/Msg2QQ.h"
#include "../logic/Player.h"
#include "../logic/GameSrvCfg.h"
using namespace std;
//====================================================================================
GameFriendInfo::GameFriendInfo()
{
	m_pFrdInfo = new FriendInfoLite();
	Clear();
}
GameFriendInfo::~GameFriendInfo()
{
	Clear();
	SafeDelete(m_pFrdInfo);
}
void GameFriendInfo::Clear()
{
	for(int i=0;i<PLAT_TYPE_MAX;i++)
	{
		m_strName[i] = "";
		m_strProfileLink[i] = "";
		m_bLoad[i] = false;
	}
	m_mapRegionLV.clear();
	m_mapRegionUID.clear();
	//timestamp = 0;
	//fid		  = 0;
	m_pFrdInfo->Clear();
}

//====================================================================================
GameDataHandler::GameDataHandler(int nid) : DataHandler(nid)
{
	eh_ = NULL;
	nid_ = nid;
	ds_ = new GameDataSaver(nid);
	users_.clear();
	onlineUsersID_.clear();
	revision_ = 1;
	logger_ = log4cxx::Logger::getLogger("GameDataHandler");

	//m_ltGetUserRankTime = 0;
	m_ltStart = time(NULL);
	next_user_id_ = 10000000;
	next_item_id_ = 1;	
	m_bAdFlag	  = false;

	m_nRemoveKey = 0;
	m_bandUser = 0;

	for(int i=0;i<serverConfig.region_num();i++)
	{
		GameRgnDataHandler* pRgnHD = new GameRgnDataHandler(this,i);
		pRgnHD->Init();
		m_mapRgnDataHandler[i] = pRgnHD;
	}

	pthread_mutex_init(&data_mutex_, NULL);
	timeUpdate			= time(NULL);
	m_timeRemoveUpdate	= Clock::getCurrentSystemTime();
	m_ltStart4RM		= Clock::getCurrentSystemTime();
	m_listRemove.clear();
	m_mapRemove.clear();

	for(int i=RC_CountryStart;i<RC_End;i++)
	{
		CLiteCountry* pCountry = new CLiteCountry(0,i);
		m_mapLiteRegionCountry[i] = pCountry;
	}
}

GameDataHandler::~GameDataHandler(void)
{
	m_listRemove.clear();
	m_mapRemove.clear();
    hash_map<int64, User*>::iterator iter;
    for (iter = users_.begin(); iter != users_.end(); ++iter)
    {
        delete iter->second;
    }
	onlineUsersID_.clear();
	map<int64, User*>::iterator iter1;
    acquireDataLock();
    for (iter1 = loaded_user_.begin(); iter1 != loaded_user_.end(); ++iter1)
    {
        delete iter1->second;
    }

    releaseDataLock();

    delete ds_;

	for (int i = 0; i < PLAT_TYPE_MAX; i++)
	{
		hash_map<string, GameFriendInfo*>::iterator iterFriend;
		for (iterFriend = platid_friendinfo_.begin(); iterFriend != platid_friendinfo_.end(); ++iterFriend)
		{
			delete iterFriend->second;
		}
		platid_friendinfo_.clear();
	}
	
	map<int,GameRgnDataHandler*>::iterator itRgn;
	for(itRgn = m_mapRgnDataHandler.begin();itRgn!=m_mapRgnDataHandler.end();itRgn++)
	{
		delete itRgn->second;
	}
	m_mapRgnDataHandler.clear();

	pthread_mutex_destroy(&data_mutex_);

	map<int,CLiteCountry*>::iterator itCntr;
	for(itCntr = m_mapLiteRegionCountry.begin();itCntr!=m_mapLiteRegionCountry.end();itCntr++)
	{
		delete itCntr->second;
	}
	m_mapLiteRegionCountry.clear();
}

//void GameDataHandler::loadAllocateSetting()
//{
//    ds_->loadAllocateSetting(next_user_id_, next_item_id_);
//    int step = 10000;
//    if (next_user_id_ > 1)
//    {
//        next_user_id_ += step;
//    }
//    if (next_item_id_ > 1)
//    {
//        next_item_id_ += step;
//    }
//    ds_->saveAllocateSetting(next_user_id_, next_item_id_);
//}
//
//void GameDataHandler::saveAllocateSetting()
//{
//    ds_->saveAllocateSetting(next_user_id_, next_item_id_);
//}

//int64 GameDataHandler::allocateUserID(const string& platid)
//{
//    int hash = getPlatidHash(platid);
//    int64 uid = next_user_id_;
//    next_user_id_ ++;
//	int nSrvID = nid_%1024;
//	int nPhysicsRegionID = getPhysicsRegion(uid);
//    //uid |= (((int64) (nSrvID & 0xffff)) << 48) | (((int64) (hash & 0xffff)) << 32);
//
//	uid |= (((int64) (nPhysicsRegionID & 0xffff)) << 58) |(((int64) (nSrvID & 0xffff)) << 48) | (((int64) (hash & 0xffff)) << 32);
//
//	return uid;
//}

//int64 GameDataHandler::allocateItemID(int num)
//{
//    int64 iid = next_item_id_;
//    next_item_id_ += num;
//    iid |= ((int64) (nid_ & 0xffff)) << 48;
//    //iid |= ((int64)(nid_ &0x7f)) << 24;
//    return iid;
//}

//void GameDataHandler::loadBulltin()
//{
//	char pBulltin[409600]={0};
//	ds_->loadBulltin(pBulltin);
//
//	string s(pBulltin);
//	string delim("###");
//	size_t last = 0;
//	size_t index=s.find(delim,last);
//	while (index!=std::string::npos)
//	{
//		m_bulltin.push_back(s.substr(last,index-last));
//		last=index+3;
//		index=s.find(delim,last);
//	}
//}

//void GameDataHandler::saveBulltin()
//{
//	string s("");
//	for (deque<string>::iterator iter=m_bulltin.begin(); iter!=m_bulltin.end(); ++iter)
//	{
//		s += *iter;
//		s += "###";
//	}
//
//	ds_->saveBulltin(s);
//}
//
//#define MAX_BULLTIN_SIZE 10
//void GameDataHandler::addBulltin(const string& info)
//{
//	if (m_bulltin.size() == MAX_BULLTIN_SIZE)
//	{
//		m_bulltin.pop_front();
//	}
//	m_bulltin.push_back(info);
//
//	saveBulltin();
//}
//
//void GameDataHandler::delBulltin()
//{
//	if (m_bulltin.size() > 0)
//	{
//		m_bulltin.pop_back();
//		saveBulltin();
//	}
//}

void GameDataHandler::mapUidToUser(int64& uid, User* user)
{
	bool bPushLst = false;
    hash_map<int64, User*>::iterator iter = users_.find(uid);
    if (iter != users_.end())
    {
        if (user != NULL)
        {
            if (iter->second == NULL)
            {
                users_[uid] = user;
				bPushLst = true;
            }
            else
            {
                delete user;
				user = NULL;
            }
        }
    }
    else
    {
        users_.insert(make_pair(uid, user));
		bPushLst = true;
    }

    map<int64, LoadStatus>::iterator load_iter = load_list_.find(uid);
    if (load_iter != load_list_.end())
    {
        load_list_.erase(load_iter);
		bPushLst = true;
    }
	//if(bPushLst)
	//{
	//	if(m_mapRemove.find(uid) == m_mapRemove.end())
	//	{
	//		m_listRemove.push_back(uid);
	//		m_mapRemove[uid] = 1;
	//	}
	//	
	//}
}

void GameDataHandler::tick()
{
    updateUserMap();
    //updatePlatidMap();
    saveAllUserData();
	//UpdateRemoveUser();

	MemCacheServerHandler::SaveAllUserData(revision_);

	UpdateRemoveUser_new();
	
	//MemCacheServerHandler::UpdateRemoveUser();
}

void GameDataHandler::quit()
{
	MemCacheServerHandler::SaveAllUserData(revision_,true);
	saveAllUserData(true);
}

void GameDataHandler::termsThread()
{
	ds_->termThreads();
	exit(0);
}

void GameDataHandler::PushOnlineUserID(int64 nID)
{
	onlineUsersID_.insert(make_pair(nID,1));
}

void GameDataHandler::PopOnlineUserID(int64 nID)
{
	map<int64,int>::iterator iter;
	iter = onlineUsersID_.find(nID);
	if(iter!=onlineUsersID_.end())
	{
		onlineUsersID_.erase(nID);
	}
}

void GameDataHandler::UpdateRemoveUser_new()
{
	if (GameSrvCfg::Instance().FreeEnableFlag() != 2)
	{
		return;
	}

	if ((int) users_.size() <  GameSrvCfg::Instance().FreeMaxUserCnt())
	{
		return;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if(m_ltStart4RM + GameSrvCfg::Instance().FreeMaxUserTime() > timeNow)
	{//服务器刚启动，所有玩家都不会删除
		return;
	}

	if (timeNow - m_timeRemoveUpdate < GameSrvCfg::Instance().FreeUpdateTime())
	{
		return;
	}
	hash_map<int64, User*>::iterator iter,iterOld; 
	iter = users_.find(m_nRemoveKey);
	if(iter == users_.end())
	{
		iter = users_.begin();
	}
	int64 nTM1=0,nTM2=0,nTM3=0,nTM4=0,nTM5=0;
	int nCnt1=0,nCnt2=0,nCnt3=0,nCnt4=0;
	int64 nTmFlag = timeNow;
	int64 nTmFlag2 = 0;
	bool bFree = false;
	int nFreeCnt = 0;
	time_t time_last = Clock::getCurrentSystemTime();

	while(iter != users_.end())
	{
		User * pUser = iter->second;

		nTmFlag2 = Clock::getCurrentSystemTime();
		nTM1 += nTmFlag2 - nTmFlag;
		nTmFlag = nTmFlag2;

		if(nCnt1>200||nCnt2>200||nCnt3>200||nCnt4>200)
		{
			time_last = Clock::getCurrentSystemTime();
			if (time_last - timeNow > 80)
			{
				bFree = true;
				SYS_LOG(0,LT_RemoveUser,0,0,1<<nFreeCnt<<(int)users_.size()<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<"-----------"<<nTM1<<nTM2<<m_nRemoveKey);
				break;
			}
		}

		if (pUser == NULL)
		{
			iter++;
			nCnt1++;
			continue;
		}

		if (pUser->Online() == true)
		{
			iter++;
			nCnt2++;
			continue;
		}

		if (timeNow < pUser->RMRevision() + GameSrvCfg::Instance().FreeMaxUserTime())
		{
			iter++;
			nCnt3++;
			continue;
		}

		int64 uid = iter->first;
		hash_map<int64, User*>::iterator iter_dirty = dirty_users_.find(uid);
		if (iter_dirty != dirty_users_.end())
		{
			iter++;
			nCnt4++;
			continue;
		}

		iterOld = iter;
		iter++;
		nFreeCnt++;
		{//删除用户
			users_.erase(iterOld);
			delete pUser;
			pUser = NULL;
		}

		nTmFlag2 = Clock::getCurrentSystemTime();
		nTM2 += nTmFlag2 - nTmFlag;
		nTmFlag = nTmFlag2;

		int nUserSize = (int)users_.size();
		if (nUserSize<=GameSrvCfg::Instance().FreeMaxUserCnt()||
			nFreeCnt >= GameSrvCfg::Instance().FreeCnt())
		{
			bFree = true;
			SYS_LOG(0,LT_RemoveUser,0,0,0<<nFreeCnt<<nUserSize<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<"-----------"<<nTM1<<nTM2<<m_nRemoveKey);
			break;
		}

		time_t time_last = Clock::getCurrentSystemTime();
		if (time_last - timeNow > 80)
		{
			bFree = true;
			SYS_LOG(0,LT_RemoveUser,0,0,1<<nFreeCnt<<nUserSize<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<"-----------"<<nTM1<<nTM2<<m_nRemoveKey);
			break;
		}

	}

	if(iter != users_.end())
	{
		m_nRemoveKey = iter->first;
	}
	else
	{
		m_nRemoveKey = 0;
	}
	if(!bFree)
	{
		SYS_LOG(0,LT_RemoveUser,0,0,-1<<nFreeCnt<<(int)users_.size()<<nCnt1<<nCnt2<<nCnt3<<nCnt4<<"-----------"<<nTM1<<nTM2<<m_nRemoveKey);
	}

	m_timeRemoveUpdate = Clock::getCurrentSystemTime();
}

void GameDataHandler::UpdateRemoveUser()
{
	return;
	if (GameSrvCfg::Instance().FreeEnableFlag() != 1)
	{
		return;
	}

	if ((int) users_.size() <  GameSrvCfg::Instance().FreeMaxUserCnt())
	{
		return;
	}

	time_t timeNow = Clock::getCurrentSystemTime();
	if(m_ltStart4RM + GameSrvCfg::Instance().FreeMaxUserTime() > timeNow)
	{//服务器刚启动，所有玩家都不会删除
		return;
	}

	if (timeNow - m_timeRemoveUpdate < GameSrvCfg::Instance().FreeUpdateTime())
	{
		return;
	}

	//vector<User*> vecRemoveUser;
	//list<int64>::iterator iter = m_listRemove.begin();
	list<int64> lstPushUser;
	hash_map<int64,int>::iterator iter_RM; 

	lstPushUser.clear();
	int nFreeCnt = 0;
	//while (iter != m_listRemove.end())
	while(m_listRemove.size()>0)
	{
		int64 uid = m_listRemove.front();
		m_listRemove.pop_front();
		//int64 uid = *iter;
		hash_map<int64, User*>::iterator iter_user = users_.find(uid);
		if (iter_user == users_.end() )
		{//error
			iter_RM = m_mapRemove.find(uid);
			if(iter_RM != m_mapRemove.end())
			{
				m_mapRemove.erase(iter_RM);
			}
			continue;
		}

		User * pUser = iter_user->second;
		if (pUser == NULL)
		{
			iter_RM = m_mapRemove.find(uid);
			if(iter_RM != m_mapRemove.end())
			{
				m_mapRemove.erase(iter_RM);
			}
			continue;
		}

		if (pUser->Online() == true)
		{
			lstPushUser.push_back(uid);
			continue;
		}

		if (timeNow < pUser->RMRevision() + GameSrvCfg::Instance().FreeMaxUserTime())
		{
			lstPushUser.push_back(uid);
			continue;
		}

		hash_map<int64, User*>::iterator iter_dirty = dirty_users_.find(uid);
		if (iter_dirty != dirty_users_.end())
		{
			lstPushUser.push_back(uid);
			continue;
		}

		nFreeCnt++;
		//RemoveUser(pUser);
		{//删除用户
			users_.erase(iter_user);
			delete pUser;
			pUser = NULL;
		}
		iter_RM = m_mapRemove.find(uid);
		if(iter_RM != m_mapRemove.end())
		{
			m_mapRemove.erase(iter_RM);
		}

		int nUserSize = (int)users_.size();
		if (nUserSize<=GameSrvCfg::Instance().FreeMaxUserCnt()||
			nFreeCnt >= GameSrvCfg::Instance().FreeCnt())
		{

			SYS_LOG(uid,LT_RemoveUser,0,0,0<<nFreeCnt<<nUserSize);
			break;
		}

		time_t time_last = Clock::getCurrentSystemTime();
		if (time_last - timeNow > 80)
		{
			SYS_LOG(uid,LT_RemoveUser,0,0,1<<nFreeCnt<<nUserSize);
			break;
		}
	}
	m_listRemove.insert(m_listRemove.end(),lstPushUser.begin(),lstPushUser.end());
	lstPushUser.clear();
	m_timeRemoveUpdate = Clock::getCurrentSystemTime();
}

void GameDataHandler::RemoveUser(User *pUser)
{
	if (pUser)
	{
		hash_map<int64, User*>::iterator iter;
		iter = users_.find(pUser->GetUid());
		if (iter != users_.end())
		{
			users_.erase(iter);
			delete pUser;
			pUser = NULL;
		}
	}	
}
void GameDataHandler::updateUserMap()
{
    acquireDataLock();
    map<int64, User*>::iterator iter = loaded_user_.begin();
    while (iter != loaded_user_.end())
    {
        map<int64, User*>::iterator oiter = iter;
        ++iter;
        int64 uid = oiter->first;
        User* user = oiter->second;
        loaded_user_.erase(oiter);

        if (user != NULL)
        {
            const string& pid = user->GetPlatformId();
			user->SetRmRevision(revision_);
            //mapPlatidToUid(pid, uid);
        }
        LOG4CXX_DEBUG(logger_, "Update user map uid=" << uid << " ,user=" << user);
        mapUidToUser(uid, user);
    }
    releaseDataLock();
}

//void GameDataHandler::updatePlatidMap()
//{
//    //acquireDataLock();
//    //map<string, int64>::iterator iter = loaded_platid_.begin();
//    //while (iter != loaded_platid_.end())
//    //{
//    //    map<string, int64>::iterator oiter = iter;
//    //    ++iter;
//    //    string pid = oiter->first;
//    //    int64 uid = oiter->second;
//    //    loaded_platid_.erase(oiter);
//
//    //    LOG4CXX_DEBUG(logger_, "Update user map platid=" << pid << " ,uid=" << uid);
//    //    mapPlatidToUid(pid, uid);
//    //}
//    //releaseDataLock();
//	acquireDataLock();
//	//map<string, DB_PlatidInfoList*>::iterator iter = loaded_platid_.begin();
//	//while (iter != loaded_platid_.end())
//	//{
//	//	map<string, DB_PlatidInfoList*>::iterator oiter = iter;
//	//	++iter;
//	//	string pid = oiter->first;
//	//	DB_PlatidInfoList* pPlatidInfoList = oiter->second;
//	//	if (pPlatidInfoList == NULL)
//	//		continue;
//	//	loaded_platid_.erase(oiter);
//	//	mapPlatidToUid(pid,pPlatidInfoList);
//	//
//	//	//DB_PlatidInfoList* pReturn = NULL;
//	//	//for (int i=0;i<pPlatidInfoList->roleinfo_size();i++)
//	//	//{
//	//	//	const DB_PlatidInfo& platidinfo = pPlatidInfoList->roleinfo(i);
//	//	//	pReturn = mapPlatidToUid(pid, platidinfo.uid(),platidinfo.srvregion());
//	//	//	LOG4CXX_DEBUG(logger_, "Update user map platid=" << pid << " ,uid=" << platidinfo.uid());
//	//	//}
//	//	//if (pReturn)
//	//	//{	//坑，绝对坑
//	//	//	pReturn->set_lastuid(pPlatidInfoList->lastuid());
//
//	//	//	DB_GiftInfo* pDBGiftInfo = pReturn->mutable_giftinfo();
//	//	//	DB_GiftInfo* pTmpGiftInfo = pPlatidInfoList->mutable_giftinfo();
//	//	//	pDBGiftInfo->CopyFrom(*pTmpGiftInfo);	
//
//	//	//}
//	//	//delete pPlatidInfoList;
//	//	//pPlatidInfoList = NULL;
//
//	//	//loaded_platid_.erase(oiter);
//
//	//}
//	//loaded_platid_.clear();
//	releaseDataLock();
//}

void GameDataHandler::updateLoadUser(int64& uid, User* user)
{
    acquireDataLock();
    map<int64, User*>::iterator iter = loaded_user_.find(uid);
    if (iter != loaded_user_.end())
    {
        if (user != NULL)
        {
            if (iter->second == NULL)
            {
                loaded_user_[uid] = user;
            }
            else
            {
                delete user;
            }
        }
    }
    else
    {
        loaded_user_.insert(make_pair(uid, user));
    }
    releaseDataLock();
}


void GameDataHandler::markUserDirty(User* user)
{
    if (user == NULL)
    {
        return;
    }
    int64 uid = user->GetUid();
	int nSize = 0;

	//user->CheckDBBackUp();

	if (MemCacheServerHandler::GetInst() && MemCacheServerHandler::GetInst()->CanUse())
	{
		map<int64,User*> & mem_user_list = MemCacheServerHandler::GetInst()->GetUserList();
		map<int64, User*>::iterator iter = mem_user_list.find(uid);
		if (iter == mem_user_list.end())
		{
			user->setMemRevision(revision_);
			MemCacheServerHandler::SafePushUser(user->GetUid(),user);
			LOG4CXX_DEBUG(logger_, "Marking mem_user dirty: uid=" << uid << ", revision" << revision_);
		}
	}
    hash_map<int64, User*>::iterator iter = dirty_users_.find(uid);
    if (iter == dirty_users_.end())
    {
        user->setRevision(revision_);
        dirty_users_.insert(make_pair(uid, user));
        LOG4CXX_DEBUG(logger_, "Marking user dirty: uid=" << uid << ", revision" << revision_);
    }


}

void GameDataHandler::saveAllUserData(bool urgent)
{
    if (!urgent && revision_ < timeUpdate + 10 * 1000)
    {
        return;
    }
    ds_->safeSaveAllUser(revision_, this, urgent);
//	ds_->safeSaveAllMap(revision_, this, urgent);
//  saveAllocateSetting();
    timeUpdate = revision_;
}

void GameDataHandler::saveUserData(User* user)
{
    ds_->safeSaveUser(this, user);
}

void GameDataHandler::loadUserData(int64& uid)
{
	if(LoadUserFromMemCache(uid))
		return;
    ds_->safeLoadUser(this, uid);
}

User* GameDataHandler::getUser(int64 uid, LoadStatus* status, bool load)
{
    LoadStatus tmp_status, *status_ = (status == NULL) ? &tmp_status : status;
    if (!isUidLocal(uid))
    {
        *status_ = LOAD_MISS;
        return NULL;
    }

    hash_map<int64, User*>::iterator iter = users_.find(uid);
    if (iter != users_.end())
    {
        if (iter->second == NULL)
        {
            *status_ = LOAD_EMPTY;
            return NULL;
        }
        *status_ = LOAD_SUCCESS;
		User* pUser = iter->second;
		if(pUser->GetPlayer())
		{
			pUser->GetPlayer()->SetEventHandler(eh_);
		}

		pUser->SetRmRevision(revision_);
        return iter->second;
    }

    if (!load)
    {
        *status_ = LOAD_EMPTY;
        return NULL;
    }

    map<int64, LoadStatus>::iterator load_iter = load_list_.find(uid);
    if (load_iter != load_list_.end())
    {
        *status_ = LOAD_WAITING;
        return NULL;
    }

    // max wait for 10000
    int load_count = load_list_.size();
    if (load_count >= MAX_LOAD_WAITING)
    {
        *status_ = LOAD_BUSY;
        return NULL;
    }

    load_list_.insert(make_pair(uid, LOAD_WAITING));
    *status_ = LOAD_WAITING;
    loadUserData(uid);
    return NULL;
}

//bool GameDataHandler::isPlatidLocal(const string& platid)
//{
//    //int hash = getPlatidHash(platid);
//    //return hash % serverConfig.gamedNum() + 1 == nid_;
//}

bool GameDataHandler::isUidLocal(int64 uid)
{
    int hash = getUidHash(uid);
	int nPhysicsRegion = getPhysicsRegion(uid);

	int nSrvID = hash % serverConfig.physicsGameNum() + 1;
	nSrvID += nPhysicsRegion * serverConfig.physicsGameNum() ;
    return nSrvID == nid_;
}

int	 GameDataHandler::getPhysicsGameNum()
{
	return nid_;
}

LoadStatus GameDataHandler::getUserStatus(int64 uid, bool load)
{
    LoadStatus status = LOAD_INVALID;

    User* user = getUser(uid, &status, load);
    // found
    if (user != NULL)
    {
        return LOAD_SUCCESS;
    }
    // remote user
    if (status == LOAD_MISS)
    {
        return LOAD_MISS;
    }
	// loading data
    if (status == LOAD_WAITING)
    {
        return LOAD_WAITING;
    }
    // not found or error
    return LOAD_ERROR;
}
/*
void GameDataHandler::AddBroadCast2Lst(const RseComBroadcast* pBroadcast)
{
	if(pBroadcast==NULL)
		return;
	RseComBroadcast* pNewBroadcast = new RseComBroadcast();
	pNewBroadcast->CopyFrom(*pBroadcast);

	m_lstBroadcast.push_back(pNewBroadcast);
}
*/
//

void GameDataHandler::updateUser(User* user, const string &name,
								 const string &profile_link, int gender, 
								 PLAT_TYPE plat_type, bool bIsYellowDmd, 
								 bool bIsYellowDmdYear, int i4YellowDmdLv, 
								 const vector<string> &fpid,int nCity,bool isHighYellowDmd,GameEventHandler* eh_,bool isHighDmdYear,
								 int nBlueTime,int nBlueYearTime,int nHighBlueTime,int nHighBlueYearTime)
{
//     user->SetName(name, plat_type);
//     user->SetProfileLink(profile_link, plat_type);
// 	user->SetCity(nCity);
// 
//     //user->setGender(gender);
//     if (fpid.size() != 0)
//     {
//         user->friends_platid().clear();
//         user->friends_id().clear();
//         user->setFriendsPlatID(fpid);
//     }
//     // 注册时间为空才设置
//     if (user->GetRegisterTime() <= 0)
//     {
//         user->SetRegisterTime(time(NULL));
//     }
//     user->SetPlattype(plat_type);
// 	if (plat_type == PLAT_QHALL)
// 	{
// 		user->SetQHallDmd(bIsYellowDmd);
// 		user->SetQHallDmdYear(bIsYellowDmdYear);
// 		user->SetQHallDmdLvl(i4YellowDmdLv);
// 		user->SetHighQHallDmd(isHighYellowDmd);
// 		user->SetHighQHallDmdYear(isHighDmdYear);
// 		user->SetBlueTime(nBlueTime);
// 		user->SetBlueYearTime(nBlueYearTime);
// 		user->SetHighBlueTime(nHighBlueTime);
// 		user->SetHighBlueYearTime(nHighBlueYearTime);
// 	}
// 	else{
// 		if(plat_type != PLAT_QAdd)
// 		{
// 			user->SetYellowDmd(bIsYellowDmd);
// 			user->SetYellowDmdYear(bIsYellowDmdYear);
// 			user->SetYellowDmdLvl(i4YellowDmdLv);
// 		}
// 		else
// 		{
// 			user->SetQQDmd(bIsYellowDmd);
// 			user->SetQQDmdYear(bIsYellowDmdYear);
// 			user->SetQQDmdLvl(i4YellowDmdLv);
// 		}
// 		user->SetHighYellowDmd(isHighYellowDmd);
// 	}
	user->GetPlayer()->SetEventHandler(eh_);
 //   
}

User* GameDataHandler::createUser(int64 uid,const string& pid, const string &name, const string& profile_link, int gender, 
								  PLAT_TYPE plat_type, bool bIsYellowDmd, bool bIsYellowDmdYear, int i4YellowDmdLv, 
								  const vector<string> &fpid,int nRegion, int nCity,bool bIsNewUser,string strVIA,
								  bool isHighYellowDmd,GameEventHandler* eh_,string strChannel,bool isHighDmdYear, int nBlueTime, int nBlueYearTime, int nHighBlueTime,
								   int nHighBlueYearTime)
{
	//int64 uid = allocateUserID(pid);

    User* u = newUser(uid, pid, name, profile_link, gender, plat_type,
		bIsYellowDmd, bIsYellowDmdYear, i4YellowDmdLv, fpid,nRegion,nCity,isHighYellowDmd,eh_,isHighDmdYear,nBlueTime,nBlueYearTime,nHighBlueTime,nHighBlueYearTime);
	u->GetPlayer()->SetEventHandler(eh_);

     markUserDirty(u);
     saveUserData(u);

    SYS_LOG(uid, LT_Register, 0, 0, pid.c_str());
	SYS_UserStat(u,false,"Register",strVIA,strChannel,"",pid.c_str(),plat_type,nRegion,bIsNewUser);

	if (u)
	{
		CMsg2QQ::GetInstance()->TellMsg(MQ_Regist,u,0,0,0);
	}
    return u;
}

User* GameDataHandler::newUser(int64 uid, const string& pid,
							   const string &name, const string &profile_link, 
							   int gender, PLAT_TYPE plat_type, 
							   bool bIsYellowDmd, bool bIsYellowDmdYear, 
							   int i4YellowDmdLv, const vector<string> &fpid,int nRegion,
							   int nCity,bool isHighYellowDmd,GameEventHandler* eh_,bool isHighDmdYear,
							    int nBlueTime, int nBlueYearTime, int nHighBlueTime, int nHighBlueYearTime)
{
    User *u = new User(uid, pid, name, profile_link, gender, plat_type,
            bIsYellowDmd, bIsYellowDmdYear, i4YellowDmdLv, fpid,nRegion,nCity,isHighYellowDmd,isHighDmdYear,nBlueTime,nBlueYearTime,nHighBlueTime,nHighBlueYearTime);
    mapUidToUser(uid, u);
    return u;
}

GameFriendInfo* GameDataHandler::getFriendInfo(const string& platID, enum PLAT_TYPE i4PlatType)
{
    if (i4PlatType < 0 || i4PlatType >= PLAT_TYPE_MAX)
    {
        return NULL;
    }

    //hash_map<string, GameFriendInfo*> &refPlatid_FrInfo = platid_friendinfo_[i4PlatType];
    hash_map<string, GameFriendInfo*>::iterator iter = platid_friendinfo_.find(platID);
    if (iter == platid_friendinfo_.end())
    {
        return NULL;
    }
    GameFriendInfo* friendinfo = iter->second;
    //if (friendinfo->timestamp < revision_ - 12 * 3600 * 1000)
    //{
    //    refPlatid_FrInfo.erase(iter);
    //    delete friendinfo;
    //    friendinfo = NULL;
    //}
    return friendinfo;
}


void GameDataHandler::setFriendInfo(const string& platID, int64& uid, FriendInfoLite* pLite, enum PLAT_TYPE i4PlatType,int nRegion,int nLV)
{
    if (i4PlatType < 0 || i4PlatType >= PLAT_TYPE_MAX || pLite== NULL)
    {
        return;
    }
    //hash_map<string, GameFriendInfo*> &refPlatid_FrInfo = platid_friendinfo_[i4PlatType];
    hash_map<string, GameFriendInfo*>::iterator iter = platid_friendinfo_.find(platID);
    //hash_map<int64, string>  &friPidtoUid = friendinfo_platid_uid_[i4PlatType];
    GameFriendInfo* pInfo = NULL;
    if (iter != platid_friendinfo_.end())
    {
        GameFriendInfo* oldinfo = iter->second;
        //refPlatid_FrInfo.erase(iter);
        //oldinfo->Clear();
        pInfo = oldinfo;
    }
    else
    {
        pInfo = new GameFriendInfo();
    }

    if (NULL == pInfo || pInfo->m_pFrdInfo==NULL )
    {
        return;
    }
	pInfo->m_mapRegionUID[nRegion]	= uid;
	pInfo->m_mapRegionLV[nRegion]	= nLV;
	pInfo->m_strName[i4PlatType] = pLite->first_name();
	pInfo->m_strProfileLink[i4PlatType] = pLite->pic();
	pInfo->m_bLoad[i4PlatType] = true;

	pInfo->m_pFrdInfo->CopyFrom(*pLite);
	platid_friendinfo_[platID] = pInfo;

	//pInfo->timestamp = revision_;
	//pInfo->fid = fid;
    //refPlatid_FrInfo.insert(make_pair(fpid, pInfo));
    //friPidtoUid[fid] = fpid;
}

int GameDataHandler::getGamedIdByUserId(int64 uid)
{ 
    int hash = getUidHash(uid);
    //return hash % serverConfig.gamedNum() + 1;
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = getPhysicsRegion(uid);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;
}

int GameDataHandler::getGameIdfromPlatId(string const& platid,int nRegion)
{
	//int hash = getPlatidHash(platid);
	//return hash % serverConfig.gamedNum() + 1;
	int hash = getPlatidHash(platid);
	int nGameID = hash % serverConfig.physicsGameNum() + 1;
	int nPhysicsRgn = serverConfig.GetPhysicsRegionByLogicRegion(nRegion);
	nGameID += nPhysicsRgn * serverConfig.physicsGameNum();
	return nGameID;

}
bool GameDataHandler::LoadUserFromMemCache( int64 nUserID )
{
	string infostr("");
	string infoeffstr("");
	string infonpcstr("");
	bool hasInfo = MemCacheServerHandler::GetUserInfo( nUserID, infostr);//,infonpcstr,infoeffstr);
	if (hasInfo)
	{
		//测试内存长度
		//if(infostr.size()>100*1024)
		//{
		//	DB_User dbTmp;
		//	bool bSuc = dbTmp.ParseFromString( infostr );
		//	string text;
		//	int nLen = 0;
		//	for(int i=0;i<dbTmp.fightrecord_size();i++)
		//	{
		//		DB_LootReport* pReport = dbTmp.mutable_fightrecord(i);
		//		pReport->SerializeToString(&text);
		//		nLen = text.size();

		//	}
		//	DB_Player* pDBPlay = dbTmp.mutable_player();
		//	pDBPlay->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_Bases* pDBBase = pDBPlay->mutable_base(0);
		//	pDBBase->SerializeToString(&text);
		//	nLen = text.size();
		//	
		//	DB_LootReport* pDBLootReport = pDBBase->mutable_lootreportuser();
		//	pDBLootReport->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_WeaponCenter* pDBWPC = pDBBase->mutable_weaponcenter();
		//	pDBWPC->SerializeToString(&text);
		//	nLen = text.size();
		//	
		//	DB_AdmiralInfo* pDBAdmiral = pDBBase->mutable_admiralinfo();
		//	pDBAdmiral->SerializeToString(&text);
		//	nLen = text.size();

		//	pDBAdmiral->mutable_admiral(0)->SerializeToString(&text);
		//	nLen = text.size();
		//	nLen *= pDBAdmiral->admiral_size();
		//
		//	DB_Strategic* pDBStrate = pDBBase->mutable_strategicinfo();
		//	pDBStrate->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_PVEFightData* pDBPVE = pDBBase->mutable_pvefightdata();
		//	pDBPVE->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_Country* pDBCountry = pDBBase->mutable_countrydata();
		//	pDBCountry->SerializeToString(&text);
		//	nLen = text.size();

		//	pDBBase->mutable_army(0)->SerializeToString(&text);
		//	nLen = text.size();
		//	nLen *= pDBBase->army_size();

		//	pDBBase->mutable_building(0)->SerializeToString(&text);
		//	nLen = text.size();
		//	nLen *= pDBBase->building_size();


		//	DB_UserStar* pDBStar = pDBPlay->mutable_userstar();
		//	pDBStar->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_DefenseActivity* pDBDef = pDBPlay->mutable_defenseactivity();
		//	pDBDef->SerializeToString(&text);
		//	nLen = text.size();

		//	DB_Bag* pDBBag = pDBPlay->mutable_bag();
		//	pDBBag->SerializeToString(&text);
		//	nLen = text.size();

		//	int a;
		//	a = 4;
		//}
		User* pUser = new User();
		DB_User dbUser;
		if (dbUser.ParseFromString( infostr ))
		{
			/*counter_.increase("parse_from_mem");
			stat_.capture("parse_from_mem",counter_.count("parse_from_mem"));
			if (Clock::GetMinute()%30 == 0)
			{
				counter_.clear("parse_from_mem");
			}*/
			pUser->SetDbUser(dbUser);
			updateLoadUser(nUserID, pUser);
			return true;
		}
		else
		{
			
			delete pUser;
			pUser = NULL;
			LOG4CXX_ERROR(logger_, "GameDataHandler::LoadUserFromMemCache() ERROR!!! ParseFromString UserID:" << nUserID);
			return false;
		}
	}
	return false;
}

GameRgnDataHandler* GameDataHandler::GetRgnDataHandler(int nRegion)
{
	map<int,GameRgnDataHandler*>::iterator iter;
	iter = m_mapRgnDataHandler.find(nRegion);
	if(iter != m_mapRgnDataHandler.end())
		return iter->second;
	return NULL;
}

CLiteCountry* GameDataHandler::GetLiteRegionCountryInfo(int nCountry,bool bCreateIfNotExit)
{
	map<int,CLiteCountry*>::iterator iter;

	iter = m_mapLiteRegionCountry.find(nCountry);
	if(iter != m_mapLiteRegionCountry.end())
	{
		return iter->second;
	}
	if(bCreateIfNotExit)
	{
		CLiteCountry* pLite = new CLiteCountry(0,nCountry);
		pLite->m_nCountryID = nCountry;
		pLite->m_nRegion    = 0;
		m_mapLiteRegionCountry[nCountry] = pLite;
		return pLite;
	}
	return NULL;
}


int GameDataHandler::GetRegionCountryUser(int nCountry)
{
	CLiteCountry* pLite = GetLiteRegionCountryInfo(nCountry);
	if(pLite)
	{
		return pLite->m_nUserCnt;
	}
	return 0;

}

void GameDataHandler::SetRegionCountryUser(int nCountry,int nCnt)
{
	CLiteCountry* pLite = GetLiteRegionCountryInfo(nCountry,true);
	pLite->m_nUserCnt = nCnt;
}

void GameDataHandler::AddRegionCountryUser(int nCountry,int nCnt)
{
	CLiteCountry* pLite = GetLiteRegionCountryInfo(nCountry,true);
	pLite->m_nUserCnt += nCnt;

}

int GameDataHandler::GetMaxUserRegionCountry()
{
	int nID = C_Start;
	int nNum = GetRegionCountryUser(nID);
	for(int i=C_Start+1;i<C_UserCoutryEnd;i++)
	{
		int nCnt = GetRegionCountryUser(i);
		if(nCnt>nNum)
		{
			nID = i;
			nNum = nCnt;
		}
	}
	return nID;
}
int GameDataHandler::GetMinUserRegionCountry()
{
	int nID = C_Start;
	int nNum = GetRegionCountryUser(nID);
	for(int i=C_Start+1;i<C_UserCoutryEnd;i++)
	{
		int nCnt = GetRegionCountryUser(i);
		if(nCnt<nNum)
		{
			nID = i;
			nNum = nCnt;
		}
	}
	return nID;
}

int GameDataHandler::GetRegionCountryBoom(int nCountry)
{
	CLiteCountry* pLite = GetLiteRegionCountryInfo(nCountry,true);

	return pLite->m_nBoom;
}


void GameDataHandler::ResetRegionCountryRank()
{
	map<int,CLiteCountry*>::iterator iter;
	vector<CLiteCountry*> lstCountry;
	vector<CLiteCountry*>::iterator itCt;
	for(iter = m_mapLiteRegionCountry.begin();iter!=m_mapLiteRegionCountry.end();iter++)
	{
		CLiteCountry* pCntr = iter->second;

		bool bInsert = false;
		for(itCt = lstCountry.begin();itCt!=lstCountry.end();itCt++)
		{
			CLiteCountry* pTmp = *itCt;
			if(pCntr->m_nBoom >= pTmp->m_nBoom)
			{
				if(itCt == lstCountry.begin())
				{
					bInsert = true;
					lstCountry.insert(itCt,pCntr);
					break;
				}
				else
				{	
					bInsert = true;
					lstCountry.insert(itCt,pCntr);
					break;
				}
			}
		}
		if(!bInsert)
		{
			lstCountry.push_back(pCntr);
		}
	}


	if(lstCountry.size()<=0)
		return;
	string strInfo;
	char szTmp[256];
	int nRank = 1;
	lstCountry[0]->m_nCurRank = nRank;
	for(int i=1;i<(int)lstCountry.size();i++)
	{
		if(lstCountry[i]->m_nBoom >= lstCountry[i-1]->m_nBoom)
		{
			lstCountry[i]->m_nCurRank = nRank;
		}	
		else
		{
			nRank++;
			lstCountry[i]->m_nCurRank = nRank;
		}
		sprintf(szTmp,"%d:%d:%d:%d   ;",lstCountry[i]->m_nCountryID,lstCountry[i]->m_nUserCnt,lstCountry[i]->m_lstCityID.size(),lstCountry[i]->m_nBoom);
		strInfo+=szTmp;
	}
	//SYS_LOG(0,LT_CNTR_Rank,0,0,m_nRegion<<strInfo);

}
