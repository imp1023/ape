#pragma once

#include "BaseEvent.h"
class DB_Plat;
class DB_PlatGift;
class DB_FrdInfoLite;
class DB_PlatGiftFrdInfo;
class FrdGiftTbl;
class FriendInfoLite;
class WebCrossPromotion;


class DealPlatEvent : public CBaseEvent
{
public:
	DealPlatEvent();
	~DealPlatEvent();
	RegistPlatEvent(DealPlatEvent)
private:
	void registHandler();
	void handle(Event* e);
private:
	void processReloadConfig(Event* e);
	void HandleUserLogin(Event* e);

	void OnPlayerOnLine(DB_Plat* pDBPlat);

private:
	DB_Plat* processUserLogin(const string& platid, int siteid, const string& name,
		const string& profile_link, int gender, vector<string> friends_platid,
		bool isYellowDmd, bool isYellowDmdYear, int lvlYellowDmd, LoadStatus& state,
		int region,int nNewUserRegion,int& level,int& nRtnRegion,int64& uid,
		bool& bIsNewPlayer,bool isHighYellowDmd,string strTaskMarketId,WebCrossPromotion* pPromotion,bool isHighDmdYear);

private:
};
