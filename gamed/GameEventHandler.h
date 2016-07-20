#ifndef _GAMEEVENTHANDLER_H_
#define _GAMEEVENTHANDLER_H_
#pragma once
#include <log4cxx/logger.h>
#include "../common/const_def.h"
#include "../event/EventHandler.h"
#include "../event/event.pb.h"

class EventHandler;
class EventQueue;
class GameDataHandler;
class GameNetHandler;
class User;
class GameEventHandler
{
public:
    GameEventHandler(EventQueue *eq, GameDataHandler *dh, GameNetHandler *nh, int nid);
    ~GameEventHandler(void);
    void start();
	void quit();
    void sendFdString(int fd, const char* str, size_t size);

    int  GetSrvID()
    {
        return nid_;
    }
    void initialEventProcessors();
    void initAutoEventProcessors();
	void sendEventToWorld(Event *e);
	void sendEventToPlat(Event *e,int nPlatID);
	void sendEventToAllPlat(Event* e);
// 	void sendEventToCountry(Event* e,int nRegion);
// 	void sendEventToAllCountry(Event* e);
// 	void sendEventToFight(Event *e,int nFightID);
// 	void sendEventToAllFight(Event* e);
// 	void sendEventToStar(Event* e,int nStarID);
// 	void sendEventToAllStar(Event* e);
// 	void sendEventToRegion(Event* e,int nRegion);
// 	void sendEventToAllRegion(Event* e);
// 	void sendEventToRank(Event* e,int nRegion);
// 	void sendEventToAllRank(Event* e);
// 	void sendEventToStat(Event* e,int nRegion);
// 	void sendEventToAllStat(Event* e);

	bool sendInfoToMonitor(int64 uid,const string& info);

	void SendUserInfo(User* pUser);
    bool sendDataToUser(int fd, int type, const string &text,bool bUseRc4=true);
    void createUserFdMap(int fd, int64 uid);
    void removeUserFdMap(int fd, int64 uid);
    void sendToGlobalUser(int64 uid, int cmd, const string &text);
    void SendToAllOnlineUser(int cmd, const string& text,int nRegion,bool bUseRc4=true);
	void SendToAllOnLineUserByCountry(int cmd, const string& text, int nCountry1, int nCountry2,int nRegion,bool bUseRc4=true);
//	void SendGift2Frd(User* pUser,int nGiftID,int nType,const string& strFrdPlatID);	//向好友发送礼物,type:0赠送 1:索取2:回赠
	void SendToAllOnlineUserByLevel(int cmd, const string& text, int iMinLvl, int iMaxLvl,int nRegion,bool bUseRc4=true);
	
	int  GetPlatSrvID(const string& pid);
	int	 GetFightSrvID(const string& pid);
	int	 GetFightSrvID(int64 uid);

	void SendCountryNumQuery();	//向国家服务器发送请求，获得国家人数
public:

    inline EventHandler* getEventHandler()
    {
        return eh_;
    }

    inline GameDataHandler* getDataHandler()
    {
        return dh_;
    }

    inline time_t getReversion()
    {
        return eh_->getReversion();
    }

    inline void postBackEvent(Event* e)
    {
        eh_->postBackEvent(e);
    }

    inline const string& getServerIp()
    {
        return serverIp;
    }

    inline int getServerPort()
    {
        return serverPort;
    }

	inline int getServerPort1()
	{
		return serverPort1;
	}

    inline GameWorkingStatus getWorkingStatus()
    {
        return eh_->getWorkingStatus();
    }

    inline void setWorkingStatus(GameWorkingStatus status)
    {
        eh_->setWorkingStatus(status);
    }

    inline EventQueue*  getEventQueue()
    {
        return eq_;
	}

	inline GameNetHandler* getNetHandler() { return nh_; }

private:
    string serverIp;
    int serverPort;
	int serverPort1;

	EventQueue* eq_;
	EventHandler* eh_;
    GameDataHandler* dh_;
    GameNetHandler* nh_;
    int nid_;
    log4cxx::LoggerPtr logger_;

	vector<int> m_runningSaleVec;
} ;

#endif
