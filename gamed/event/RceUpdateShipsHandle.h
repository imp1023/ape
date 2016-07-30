#ifndef _RCEUPDATESHIPSHANDLE_H_
#define _RCEUPDATESHIPSHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceUpdateShipsHandle : public CBaseEvent
{
public:
	RceUpdateShipsHandle();
	~RceUpdateShipsHandle();
	RegistEvent(RceUpdateShipsHandle,C2S_RceUpdateShips)
	void SendRet2User(User* pUser,int nRet,RseUpdateShips& rseUpdateShips);
enum
{
	RseUpdateShipsRet_Success = 0,
	RseUpdateShipsRet_ResNotEnough = 1,
	RseUpdateShipsRet_SlotNotFind = 2,
	RseUpdateShipsRet_CantFindPlanet = 3,
	RseUpdateShipsRet_DestroyFailure = 4,
	RseUpdateShipsRet_RotateFailure = 5,
	RseUpdateShipsRet_MoveFailure = 6,
	RseUpdateShipsRet_CancelBuildFailure = 7,
	RseUpdateShipsRet_CancelUpgradeFailure = 8,
	RseUpdateShipsRet_UpdateNewStateFailure = 9,
};
private:
	void handle(Event* e);
};
#endif

