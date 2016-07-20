#pragma once
#include <map>
#include "../common/const_def.h"
using namespace std;

const int MAX_COST_GIFT_ARMY_CNT = 3;
const int MAX_COST_GIFT_ITEM_CNT = 5;

class CostGiftTbl
{
public:
	CostGiftTbl()
	{

	}
	~CostGiftTbl()
	{
		m_lstItemType.clear();
		m_lstItemCnt.clear();
		m_lstArmyType.clear();
		m_lstArmyCnt.clear();
	}

	int m_nGiftID;

	time_t m_tBeginTime;
	time_t m_tEndTime;

	int m_nTimes;

	int m_nYellowTimes;

	int m_nCostCredit;

	int m_nMedal;
	int m_nOil;
	int m_nHonor;
	int m_nFeats;

	vector<int> m_lstItemType;
	vector<int> m_lstItemCnt;

	vector<int> m_lstArmyType;
	vector<int> m_lstArmyCnt;

	int m_nAdmiralID;
	int m_nAdmiralLV;

	bool m_bNeedGet;
	int  m_nGetCredit;

	bool m_bCostCreditPlus;
	int  m_nDrawTimes;
	int  m_nAdmiralTimes;

	int	 m_nType;

	int	 m_nToken;
	int  m_nThew;

	int  m_nInviteFrd;

	bool m_bAutoGet;

	int	 m_nComeBackDay;

	int	 m_nYellowTypes; //黄钻类型，1：一个月 2：三个月 3:一年

	bool IsCurrentCanGainGiftByCredit( int nCredit ) const;
	bool IsCurrentCanGainGiftByYellow( int nYellowTimes ) const;
	bool IsCurrentCanGainGiftByDraw( int nDrawTimes ) const;
	bool IsCurrentCanGainGiftByAdmiral( int nAdmiralTimes ) const;
	bool IsCurrentCanGainGiftByDailyToken( int nTokens ) const;
	bool IsCurrentCanGainGiftByCustomFlag( int nCustonTime, int nTimeCheck ) const;
	bool IsCurrentTime( ) const;
	bool IsCurrentCanGainGift( int nCredit,int nYellowTimes,int nDrawTimes,int nAdmiralTimes = 0, int nTokens = 0, int nCustomTime = 0, int nCheckTime = 0 ) const;
};
class CostGiftCfg
{
	enum E_COST_GIFT_FILE
	{
		E_COST_GIFT_ID=0,
		E_COST_GIFT_COMMON,
		E_COST_GIFT_TIME_BG_D,
		E_COST_GIFT_TIME_BG_T,
		E_COST_GIFT_TIME_ED_D,
		E_COST_GIFT_TIME_ED_T,
		E_COST_GIFT_TIMES,
		E_COST_GIFT_YELLOW_TIMES,
		E_COST_GIFT_COST_CREDIT,
		E_COST_GIFT_MEDAL,
		E_COST_GIFT_OIL,
		E_COST_GIFT_HONOR,
		E_COST_GIFT_FEATS,
		E_COST_GIFT_Item1,
		E_COST_GIFT_ItemCnt1,
		E_COST_GIFT_Item2,
		E_COST_GIFT_ItemCnt2,
		E_COST_GIFT_Item3,
		E_COST_GIFT_ItemCnt3,
		E_COST_GIFT_Item4,
		E_COST_GIFT_ItemCnt4,
		E_COST_GIFT_Item5,
		E_COST_GIFT_ItemCnt5,
		E_COST_GIFT_ARMY0,
		E_COST_GIFT_CNT0,
		E_COST_GIFT_ARMY1,
		E_COST_GIFT_CNT1,
		E_COST_GIFT_ARMY2,
		E_COST_GIFT_CNT2,
		E_COST_GIFT_ADMIRAL,
		E_COST_GIFT_ADMIRAL_LV,
		E_COST_GIFT_NEED_GET,
		E_COST_GIFT_GET_CREDIT,
		E_COST_GIFT_COST_CREDIT_PLUS,
		E_COST_GIFT_DRAW_TIMES,
		E_COST_GIFT_ADMIRAL_TIMES,
		E_COST_GIFT_TYPE,
		E_COST_GIFT_TOKEN,
		E_COST_GIFT_THEW,
		E_COST_GIFT_INVITE_FRIEND,
		E_COST_GIFT_AUTO_GET,
		E_COST_GIFT_COME_BACK,
		E_COST_GIFT_YELLOW_TYPES,
	};


public:
	CostGiftCfg(void);
	~CostGiftCfg(void);

	static CostGiftCfg& Instance()
	{
		static CostGiftCfg inst;
		return inst;
	}

	bool			LoadTbl();
	void			ClearTbl();

	const CostGiftTbl* GetCostGiftTbl(int nGiftID);
	const CostGiftTbl* GetCostGiftTblByNum( int nNum );

	int				GetTblNum(){ return m_mapGift.size(); }
protected:
	static const char*		szGiftFile;
	map<int,CostGiftTbl*> m_mapGift;
};

