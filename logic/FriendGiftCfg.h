#pragma once
#include <map>
#include "../common/const_def.h"
using namespace std;

enum FrdGiftType
{
	FG_NoUse=0,
	FG_Frd,			//礼物系统
	FG_CallBack,	//老友找回
	FG_System,		//系统补偿
	FG_Web,			//web礼包
	FG_Invite,		//邀请好友
	FG_Other,		//其他
	FG_Cnt,
};
class FrdGiftTbl
{
public:
	FrdGiftTbl()
	{

	}
	~FrdGiftTbl()
	{
		m_lstItemType.clear();
		m_lstItemCnt.clear();
		m_lstArmyType.clear();
		m_lstArmyCnt.clear();
	}

	int m_nGiftID;
	int m_nType;

	time_t m_tBeginTime;
	time_t m_tEndTime;

	int m_nMedal;
	int m_nOil;
	int m_nNeedLV;
	int m_nCredit;
	int m_nHonor;


	int m_nWebType;
	int m_nWebFlag;
	int m_nGainOneDay;

	int m_nCoupon;

	vector<int> m_lstItemType;
	vector<int> m_lstItemCnt;

	vector<int> m_lstArmyType;
	vector<int> m_lstArmyCnt;


	bool IsCurrentCanGainGift() const;
};
class FriendGiftCfg
{
	enum E_Gift_FILE
	{
		E_GIFT_ID=0,
		E_GIFT_LV,
		E_GIFT_TYPE,
		E_GIFT_COMMON,
		E_GIFT_TIME_BG_D,
		E_GIFT_TIME_BG_T,
		E_GIFT_TIME_ED_D,
		E_GIFT_TIME_ED_T,
		E_GIFT_MEDAL,
		E_GIFT_OIL,
		E_GIFT_CREDIT,
		E_GIFT_HONOR,
		E_GIFT_Item1,
		E_GIFT_ItemCnt1,
		E_GIFT_Item2,
		E_GIFT_ItemCnt2,
		E_GIFT_Item3,
		E_GIFT_ItemCnt3,
		E_GIFT_Item4,
		E_GIFT_ItemCnt4,
		E_GFIT_ARMY0,
		E_GIFT_CNT0,
		E_GFIT_ARMY1,
		E_GIFT_CNT1,
		E_GFIT_ARMY2,
		E_GIFT_CNT2,
		E_GIFT_WEBTYPE,
		E_GIFT_WEBFLAG,
		E_GIFT_SHOWINDEX,
		E_GIFT_GAINONEDAY,
		E_GIFT_COUPON,
	};


public:
	FriendGiftCfg(void);
	~FriendGiftCfg(void);

	static FriendGiftCfg& Instance()
	{
		static FriendGiftCfg inst;
		return inst;
	}

	bool			LoadTbl();
	void			ClearTbl();

	const FrdGiftTbl* GetWebGiftTbl(int nID);
	const FrdGiftTbl* GetFrdGiftTbl(int nGiftID);
protected:
	static const char*		szGiftFile;
	map<int,FrdGiftTbl*> m_mapGift;
};

