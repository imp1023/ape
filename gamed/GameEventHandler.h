#ifndef _GAMEEVENTHANDLER_H_
#define _GAMEEVENTHANDLER_H_
#pragma once
#include <log4cxx/logger.h>
#include "../common/const_def.h"
#include "../event/EventHandler.h"

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

    void initialEventProcessors();
    void initAutoEventProcessors();
	void sendEventToWorld(Event *e);
	void sendEventToCenter(Event *e, int wid);
    bool sendDataToUser(int fd, int type, const string &text);
    void createUserFdMap(int fd, int64 uid);
    void removeUserFdMap(int fd, int64 uid);
    void SendToAllOnlineUser(int cmd, const string& text);
	//void SendToOnlineCampUser(int cmd, const string& text, int camp);
	void SendBroadCast( int type, const string& content );
	void SendBroadCast( int type, int id, vector<string> param, const string& content );

public:

    inline EventHandler* getEventHandler()
    {
        return eh_;
    }

    inline GameDataHandler* getDataHandler()
    {
        return dh_;
    }

	inline GameNetHandler* getGameNetHandler()
	{
		return nh_;
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

private:
    string serverIp;
    int serverPort;

	EventQueue* eq_;
	EventHandler* eh_;
    GameDataHandler* dh_;
    GameNetHandler* nh_;
    int nid_;
    log4cxx::LoggerPtr logger_;
} ;

#endif
