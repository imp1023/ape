#ifndef _UPDATEWORKINGSTATUS_H_
#define _UPDATEWORKINGSTATUS_H_
#pragma once

#include "BaseEvent.h"

/*

自定义EventHandler:
1，继承自CBaseEvent
2，使用BIND_1_EVENT/BIND_2_EVENT等注册事件
3，initialEventProcessors()中创建实例

*/

class UpdateWorkingStatus : CBaseEvent 
{
public:
    UpdateWorkingStatus(){}
	~UpdateWorkingStatus() {}

    BIND_1_EVENT(UpdateWorkingStatus, EVENT_UPDATE_WORKING_STATUS)

	static void addEvent(EventQueue* eq, int state, int gid);

private:
	void handle(Event* e);
	void handle_GW_Disconn(Event* e);
	void handle_GW_Conn(Event* e);
	void handle_GW_Sync(Event* e);
};

#endif
