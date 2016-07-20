#pragma once
#include <string>
#include <map>
#include "../common/const_def.h"


///////////////////////////////////////////////////////////////////////////////////////////
class GameConstantSetLogic
{
public:
	GameConstantSetLogic(int nLogic){m_nLogic = nLogic;}
	~GameConstantSetLogic(void){}

	virtual	void	Init(string* pStr,int* pInt){}
	virtual int		GetLogic(){return m_nLogic;}
private:
	int		m_nLogic;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_PlayerDefenseNpc : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_PlayerDefenseNpc(int nLogic);
	~GameConstantSetLogic_PlayerDefenseNpc(void);

	virtual	void	Init(string* pStr,int* pInt);
	bool		IsWaiGuaKickEnable(){return m_bWaiGuaKickEnable;}
private:
	bool		m_bWaiGuaKickEnable;
};

class GameConstantSetLogic_FightValueFilter : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_FightValueFilter(int nLogic);
	~GameConstantSetLogic_FightValueFilter(void);

	virtual	void	Init(string* pStr,int* pInt);
	int		GetRegionLimit(int nRegion);
private:
	map<int,int>	m_rmapRegionLimit;
	int				m_nCommon;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_WaiGuaKick : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_WaiGuaKick(int nLogic);
	~GameConstantSetLogic_WaiGuaKick(void);

	virtual	void	Init(string* pStr,int* pInt);
	int		GetKickLimit() {return m_nKickNum;}
private:
	int				m_nKickNum;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_PVEPaTa : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_PVEPaTa(int nLogic);
	~GameConstantSetLogic_PVEPaTa(void);

	virtual	void	Init(string* pStr,int* pInt);
	int				GetCntFree(){return m_nCntFree;}
	int				GetCntToll(){return m_nCntToll;}
	int				GetCredit(){return m_nCredit;}
	bool			IsPveOpen(){return m_bPveOpen;}
	bool			IsStrategicOpen(){return m_bStrategicOpen;}
private:
	int				m_nCntFree;
	int				m_nCntToll;
	int				m_nCredit;
	bool			m_bPveOpen;
	bool			m_bStrategicOpen;
};


/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_StrategicGeneral : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_StrategicGeneral(int nLogic);
	~GameConstantSetLogic_StrategicGeneral(void);

	virtual	void	Init(string* pStr,int* pInt);
	int				GetCommonAdd(){return m_nCommonAdd;}
	int				GetCommonNum(){return m_nCommonNum;}
	int				GetCommonLand(){return m_nCommonLand;}
	int				GetCommonAir(){return m_nCommonAir;}
    int             GetMaxArmyNum(){return m_nMaxArmyNum;}
private:
	int				m_nCommonAdd;
	int				m_nCommonNum;
	int				m_nCommonLand;
	int				m_nCommonAir;
    int             m_nMaxArmyNum;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_Explore : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_Explore(int nLogic);
	~GameConstantSetLogic_Explore(void);

	virtual	void	Init(string* pStr,int* pInt);
	int		GetMaxExploreTimes() {return m_nMaxExploreTimes;}
	int		GetMaxExploredTimes() {return m_nMaxExploredTimes;}
	int		GetCDTime() {return m_nCDTime;}
private:
	int				m_nMaxExploreTimes;
	int             m_nMaxExploredTimes;
	int             m_nCDTime;
};

/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_ArenaConfig : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_ArenaConfig(int nLogic);
	~GameConstantSetLogic_ArenaConfig(void);

	virtual	void	Init(string* pStr,int* pInt);

	int				GetBuyCreditByTimes(int nTime);
	int				GetPlayerLvl(){return m_nPlayerLvl;}
	int				GetPveCustom(){return m_nPveCustom;}
	int				GetPlayTimeFree(){return m_nPlayTimeFree;}
	int				GetPlayTimeCredit(){return m_nPlayTimeCredit;}
	int				GetWinScore(){return m_nWinScore;}
	int				GetFailScore(){return m_nFailScore;}
	int				GetArenaOpen(){return m_nArenaOpen;}
	bool			IsClearAll(){return m_bClearAll;}

private:
	vector<int>		m_rVecBuyCredit;
	int				m_nPlayerLvl;
	int				m_nPveCustom;
	int				m_nPlayTimeFree;
	int				m_nPlayTimeCredit;
	int				m_nWinScore;
	int				m_nFailScore;
	int				m_nArenaOpen;
	bool			m_bClearAll;
};

/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_WorldArena : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_WorldArena(int nLogic);
	~GameConstantSetLogic_WorldArena(void);

	virtual	void	Init(string* pStr,int* pInt);

	int				GetBuyCreditByTimes(int nTime);
	int				GetPlayTimeFree(){return m_nPlayTimeFree;}
	int				GetPlayTimeCredit(){return m_nPlayTimeCredit;}
	int				GetArenaOpenByRegion(int region);
	bool			IsClearAll(){return m_bClearAll;}
	int				GetWinScore(){return m_nWinScore;}
	int				GetFailScore(){return m_nFailScore;}

private:
	vector<int>		m_rVecBuyCredit;
	map<int,int>	m_rmapArenaOpen;
	int				m_nPlayTimeFree;
	int				m_nPlayTimeCredit;
	bool			m_bClearAll;
	int				m_nWinScore;
	int				m_nFailScore;
};


/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_Officer : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_Officer(int nLogic);
	~GameConstantSetLogic_Officer(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool			IsOpen(){return m_bOpen;}
private:
	bool			m_bOpen;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_RegionArena : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_RegionArena(int nLogic);
	~GameConstantSetLogic_RegionArena(void);

	virtual	void	Init(string* pStr,int* pInt);

	int				GetBuyCreditByTimes(int nTime);
	int				GetPlayTimeFree(){return m_nPlayTimeFree;}
	int				GetPlayTimeCredit(){return m_nPlayTimeCredit;}
	int				GetArenaOpen();
	bool			IsClearAll(){return m_bClearAll;}
	int				GetRankGift(){return m_nRankGift;}
	int				GetPveCustom(){return m_nPveCustom;}

private:
	vector<int>		m_rVecBuyCredit;
	int	            m_nArenaOpen;
	int				m_nPlayTimeFree;
	int				m_nPlayTimeCredit;
	bool			m_bClearAll;
	int				m_nRankGift;
	int				m_nPveCustom;
};


/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

class GameConstantSetLogic_HeroArena : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_HeroArena(int nLogic);
	~GameConstantSetLogic_HeroArena(void);

	virtual	void	Init(string* pStr,int* pInt);

	int				GetBuyCreditByTimes(int nTime);
	int				GetPlayTimeFree(){return m_nPlayTimeFree;}
	int				GetPlayTimeCredit(){return m_nPlayTimeCredit;}
	int				GetArenaOpenByRegion(int region);
	bool			IsClearAll(){return m_bClearAll;}
	int				GetWinScore(){return m_nWinScore;}
	int				GetFailScore(){return m_nFailScore;}

private:
	vector<int>		m_rVecBuyCredit;
	map<int,int>	m_rmapArenaOpen;
	int				m_nPlayTimeFree;
	int				m_nPlayTimeCredit;
	bool			m_bClearAll;
	int				m_nWinScore;
	int				m_nFailScore;
};


/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
class GameConstantSetLogic_ExploreAsrenal : public GameConstantSetLogic
{
public:
    GameConstantSetLogic_ExploreAsrenal(int nLogic);
    ~GameConstantSetLogic_ExploreAsrenal(void);

    virtual	void	Init(string* pStr,int* pInt);

    bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}

    int             GetFreeNum(){return m_nFreeNum;}

    int             GetCostCreditPerTime() {return m_nCostCredit;}

    int             GetMaxExploreNum(){return m_nMaxExploreNum;}

    int             GetRewardRes(){return m_nGetRes;}

    int             GetStartTime(){return m_tStartTime;}

    int             GetEndTime(){return m_tEndTime;}

private:

    int     m_tStartTime;
    int     m_tEndTime;
    int     m_nFreeNum;
    int     m_nCostCredit;
    int     m_nMaxExploreNum;
    int     m_nGetRes;
};

class GameConstantSetLogic_CostCreditGift : public GameConstantSetLogic
{
public:
    GameConstantSetLogic_CostCreditGift(int nLogic);
    ~GameConstantSetLogic_CostCreditGift(void);

    virtual	void	Init(string* pStr,int* pInt);

    bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}

    int             GetStartTime(){return m_tStartTime;}

    int             GetEndTime(){return m_tEndTime;}

    int             GetGiftListSize(){return m_giftList.size();}

    int             GetGiftIDByIndex(int nIndex){return m_giftList[nIndex];}

private:

    int     m_tStartTime;
    int     m_tEndTime;
    vector<int> m_giftList;
};

class GameConstantSetLogic_AdmiralSoulBuy : public GameConstantSetLogic
{
public:
    GameConstantSetLogic_AdmiralSoulBuy(int nLogic);
    ~GameConstantSetLogic_AdmiralSoulBuy(void);

    virtual	void	Init(string* pStr,int* pInt);
    int     GetFlag(){return m_nFlag;}
    int     GetNum(){return m_nNum;}
private:
    int m_nFlag;
    int m_nNum;
};

class GameConstantSetLogic_CreditRebate : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_CreditRebate(int nLogic):GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0){}
	~GameConstantSetLogic_CreditRebate(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}
	int             GetStartTime(){return m_tStartTime;}
	int             GetEndTime(){return m_tEndTime;}
private:

	int     m_tStartTime;
	int     m_tEndTime;
};

class GameConstantSetLogic_HappyCard : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_HappyCard(int nLogic) : GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0),
												m_nLabCostItmeID(0),m_nGreatCostItemID(0){}
	~GameConstantSetLogic_HappyCard(){};

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}
	//int             GetStartTime(){return m_tStartTime;}
	int             GetEndTime(){return m_tEndTime;}
	int				GetLabFetchCostItem(){return m_nLabCostItmeID;}
	int				GetLabFetchChgItem(){return m_nLabChgItemID;}
	int				GetGreatFetchCostItem(){return m_nGreatCostItemID;}
	int				GetGreatFetchChgItem(){return m_nGreatChgItemID;}
	
private:

	int				m_tStartTime;
	int				m_tEndTime;
	
	int				m_nLabCostItmeID;			//实验室招募消耗的id
	int				m_nLabChgItemID;			//活动结束后实验室招募消耗转换的id
	int				m_nGreatCostItemID;			//高级招募消耗的id
	int				m_nGreatChgItemID;			//活动结束后高级招募消耗转换的id
};
//神将抽不停活动
class GameConstantSetLogic_GetAdmiral : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_GetAdmiral(int nLogic):GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0){}
	~GameConstantSetLogic_GetAdmiral(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}
	int             GetCountLimit(){return m_nCountLimit;}
	int				GetEndTime(){return m_tEndTime;}

private:

	int     m_tStartTime;
	int     m_tEndTime;
	int		m_nCountLimit;
};
//探囊取物
class GameConstantSetLogic_WeaponSearch : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_WeaponSearch(int nLogic):GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0){}
	~GameConstantSetLogic_WeaponSearch(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}
	
private:

	int     m_tStartTime;
	int     m_tEndTime;

};

//开箱积分活动
class GameConstantSetLogic_OpenBox : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_OpenBox(int nLogic):GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0){
		m_nIntermediateBoxScore = 0;
		m_nSeniorBoxScore = 0;
	}
	~GameConstantSetLogic_OpenBox(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}
	int				GetIntermediateBoxScore(){return m_nIntermediateBoxScore;}
	int				GetSeniorBoxScore(){return m_nSeniorBoxScore;}

private:

	int     m_tStartTime;
	int     m_tEndTime;
	int		m_nIntermediateBoxScore;
	int		m_nSeniorBoxScore;

};

class GameConstantSetLogic_NewReCall : public GameConstantSetLogic
{
public:
	GameConstantSetLogic_NewReCall(int nLogic):GameConstantSetLogic(nLogic),m_tStartTime(0),m_tEndTime(0){}
	~GameConstantSetLogic_NewReCall(void);

	virtual	void	Init(string* pStr,int* pInt);

	bool            IsCurrentTime(time_t tTime) {return (tTime >= m_tStartTime && tTime <= m_tEndTime);}

	int				GetMinDay(){return m_nMinDay;}
	int				GetGiftCnt(int nDay);

private:

	int     m_tStartTime;
	int     m_tEndTime;
	int		m_nMinDay;
	int		m_nMinDayGiftCnt;

	int		m_nLargeDay;
	int		m_nLargeDayGiftCnt;

};