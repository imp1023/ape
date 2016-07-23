#ifndef _RCEUPDATEITEMHANDLE_H_
#define _RCEUPDATEITEMHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
#include "RseUpdateItem.pb.h"
class RceUpdateItemHandle : public CBaseEvent
{
public:
	RceUpdateItemHandle();
	~RceUpdateItemHandle();
	RegistEvent(RceUpdateItemHandle,C2S_RceUpdateItem)
	void SendRet2User(User* pUser,int nRet,RseUpdateItem& rseUpdateItem);

enum
{
	RseUpdateItemRet_Success = 0,
	RseUpdateItemRet_ResNotEnough = 1,
	RseUpdateItemRet_CreateFailure = 2,
	RseUpdateItemRet_CantFindPlanet = 3,
	RseUpdateItemRet_DestroyFailure = 4,
	RseUpdateItemRet_RotateFailure = 5,
	RseUpdateItemRet_MoveFailure = 6,
	RseUpdateItemRet_CancelBuildFailure = 7,
	RseUpdateItemRet_CancelUpgradeFailure = 8,
	RseUpdateItemRet_UpdateNewStateFailure = 9,
};

private:
	void handle(Event* e);
};
#endif

