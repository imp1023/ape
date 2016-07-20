#pragma once

#include <log4cxx/logger.h>
#include "../event/EventHandler.h"

class Event;
class EventQueue;
class PlatDataHandler;
class PlatNetHandler;
class DB_Plat;

class PlatEventHandler
{
public:
	PlatEventHandler(EventQueue *eq, PlatDataHandler *dh, PlatNetHandler *nh, int nid);
	~PlatEventHandler();

public:
	void start();
	EventHandler* getEventHandler(){ return eh_; }
	PlatDataHandler* getDataHandler() { return dh_; }
	EventQueue* getEventQueue() { return eq_; }
	int	 GetPlatSrvID() {return nid_;}
	// Helper
	bool sendEventToGamed(Event *e, int gid);
	bool sendEventToUser(Event *e, int64 uid);
	void sendFdString(int fd, const string &content);

	int getGamedIdByUserId(int64 uid);

	inline time_t getReversion() {return eh_->getReversion();}
	int GetSrvID() {return nid_;}

public:
	//void SendGiftList2User(int64 uid,DB_Plat* pDBPlat,int nType); //type:0服务器推 1：客户端请求
	//void SendGift2Frd(int64 uid,string& platID,int nGiftID,int nType,const string& strFrdPlatID);	//向好友发送礼物,type:0赠送 1:索取2:回赠

public:
	inline void postBackEvent(Event* e)
	{
		eh_->postBackEvent(e);
	}

public:
private:
	void initialEventProcessors();

	EventQueue* eq_;
	PlatDataHandler* dh_;
	PlatNetHandler* nh_;
	EventHandler* eh_;
	int nid_;
	log4cxx::LoggerPtr logger_;
};

