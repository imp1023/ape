#pragma once
#include <map>
#include "../common/const_def.h"
using namespace std;

const int MAX_INVITE_GIFT_ARMY_CNT = 3;
const int MAX_INVITE_GIFT_ITEM_CNT = 2;

class InviteFrdGiftTbl
{
public:
	InviteFrdGiftTbl()
	{

	}
	~InviteFrdGiftTbl()
	{
		m_lstItemType.clear();
		m_lstItemCnt.clear();
		m_lstArmyType.clear();
		m_lstArmyCnt.clear();
	}

	int m_nGiftID;

	int m_nFrdCnt;
	bool m_bCanRepeated;
	int m_nRepeatCnt;

	int m_nMedal;
	int m_nOil;
	int m_nHonor;
	int m_nCredit;

	vector<int> m_lstItemType;
	vector<int> m_lstItemCnt;

	vector<int> m_lstArmyType;
	vector<int> m_lstArmyCnt;
};

class InvitedFrdLvGiftTbl
{
public:
	InvitedFrdLvGiftTbl()
	{

	}
	~InvitedFrdLvGiftTbl()
	{

	}

	int m_nID;

	int m_nLv;
	int m_nCoupon;
};

class InviteFrdGiftCfg
{
	enum E_INVITE_GIFT_FILE
	{
		E_INVITE_GIFT_ID=0,
		E_INVITE_GIFT_FRD_CNT,
		E_INVITE_GIFT_CAN_REPEAT,
		E_INVITE_GIFT_REPEAT_CNT,
		E_INVITE_GIFT_MEDAL,
		E_INVITE_GIFT_OIL,
		E_INVITE_GIFT_HONOR,
		E_INVITE_GIFT_CREDIT,
		E_INVITE_GIFT_Item1,
		E_INVITE_GIFT_ItemCnt1,
		E_INVITE_GIFT_Item2,
		E_INVITE_GIFT_ItemCnt2,
		E_INVITE_GIFT_ARMY0,
		E_INVITE_GIFT_CNT0,
		E_INVITE_GIFT_ARMY1,
		E_INVITE_GIFT_CNT1,
		E_INVITE_GIFT_ARMY2,
		E_INVITE_GIFT_CNT2,
	};

	enum E_RE_LV_GIFT_FILE
	{
		E_RE_LV_GIFT_ID=0,
		E_RE_LV_GIFT_PLAYER_LEVEL,
		E_RE_LV_GIFT_GIFT_ID,
	};

public:
	InviteFrdGiftCfg(void);
	~InviteFrdGiftCfg(void);

	static InviteFrdGiftCfg& Instance()
	{
		static InviteFrdGiftCfg inst;
		return inst;
	}

	bool			LoadTbl();
	void			ClearTbl();
	bool			LoadReLvTbl();

	const InviteFrdGiftTbl* GetInviteGiftTbl(int nGiftID);
	const InviteFrdGiftTbl* GetInviteGiftTblByNum( int nNum );

	const InvitedFrdLvGiftTbl* GetReLvGiftTblByNum( int nNum );
	const InvitedFrdLvGiftTbl* GetReLvGiftTbl( int nGiftID );

	int				GetTblNum(){ return m_mapGift.size(); }
	int				GetReLvTblNum(){ return m_mapReLvGift.size(); }
protected:
	static const char*		szGiftFile;
	static const char*		szReLvGiftFile;
	map<int,InviteFrdGiftTbl*> m_mapGift;
	map<int,InvitedFrdLvGiftTbl*> m_mapReLvGift;
};

