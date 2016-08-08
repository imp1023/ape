#ifndef _RCEUPDATESOCIALITEMHANDLE_H_
#define _RCEUPDATESOCIALITEMHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceUpdateSocialItemHandle : public CBaseEvent
{
public:
	RceUpdateSocialItemHandle();
	~RceUpdateSocialItemHandle();
	RegistEvent(RceUpdateSocialItemHandle,C2S_RceUpdateSocialItem)
	void SendRet2User(User* pUser,int nRet,RseUpdateSocialItem& rseUpdateSocialItem);
	vector<string> split( string str, string pattern);

	enum
	{
		RseUpdateSocialItemRet_Success = 0,
		RseUpdateSocialItemRet_ResNotEnough = 1,
		RseUpdateSocialItemRet_SocialItemNotEnough = 2,
		RseUpdateSocialItemRet_AddSocialItemFailed = 3,
		RseUpdateSocialItemRet_AddDroidFailed = 4,
		RseUpdateSocialItemRet_ApplyCraftingFailed = 5,
		RseUpdateSocialItemRet_AddToWishListFailed = 6,
		RseUpdateSocialItemRet_RemoveItemFromWishListFailed = 7,
	};

private:
	void handle(Event* e);
};
#endif

