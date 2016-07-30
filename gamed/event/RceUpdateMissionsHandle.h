#ifndef _RCEUPDATEMISSIONSHANDLE_H_
#define _RCEUPDATEMISSIONSHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceUpdateMissionsHandle : public CBaseEvent
{
public:
	RceUpdateMissionsHandle();
	~RceUpdateMissionsHandle();
	RegistEvent(RceUpdateMissionsHandle,C2S_RceUpdateMissions)
	void SendRet2User(User* pUser,int nRet,RseUpdateMissions& rseUpdateMissions);
	string progressToString(string progress, int index, int amount);
	string getStringBySize(int size);

private:
	void handle(Event* e);
};
#endif

