#ifndef _RCEUPDATEGAMEUNITSHANDLE_H_
#define _RCEUPDATEGAMEUNITSHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceUpdateGameUnitsHandle : public CBaseEvent
{
public:
	RceUpdateGameUnitsHandle();
	~RceUpdateGameUnitsHandle();
	RegistEvent(RceUpdateGameUnitsHandle,C2S_RceUpdateGameUnits)
	void SendRet2User(User* pUser,int nRet,RseUpdateGameUnits& rseUpdateGameUnits);
enum
{
	RceUpdateGameUnitsRet_Success = 0,
	RceUpdateGameUnitsRet_ResNotEnough = 1,
	RceUpdateGameUnitsRet_UnitNotFind = 2,
	RceUpdateGameUnitsRet_CantFindPlanet = 3,
	RceUpdateGameUnitsRet_DestroyFailure = 4,
	RceUpdateGameUnitsRet_RotateFailure = 5,
	RceUpdateGameUnitsRet_MoveFailure = 6,
	RceUpdateGameUnitsRet_CancelBuildFailure = 7,
	RceUpdateGameUnitsRet_CancelUpgradeFailure = 8,
	RceUpdateGameUnitsRet_UpdateNewStateFailure = 9,
};
private:
	void handle(Event* e);
};
#endif

