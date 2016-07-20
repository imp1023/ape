#include "GameConstantSetLogic.h"
#include "../common/string-util.h"
#include "../common/distribution.h"

#include <vector>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_FightValueFilter::GameConstantSetLogic_FightValueFilter(int nLogic):GameConstantSetLogic(nLogic)
{
	m_rmapRegionLimit.clear();
	m_nCommon = 0;
}

GameConstantSetLogic_FightValueFilter::~GameConstantSetLogic_FightValueFilter(void){}

void	GameConstantSetLogic_FightValueFilter::Init(string* pStr,int* pInt)
{
	if(pStr == NULL || pInt == NULL)
	{
		return;
	}

	string tmpstr = pStr[0];
	vector<string> tokens;
	static string delims = ",";
	tokenize(tmpstr, tokens, delims);

	int nValue = 0;
	for(int i = 1; i<(int)tokens.size(); i++)
	{
		safe_atoi(tokens[i].c_str(), nValue);
		m_rmapRegionLimit[i-1] = nValue;
	}
	
	m_nCommon = pInt[0];
}

int		GameConstantSetLogic_FightValueFilter::GetRegionLimit(int nRegion)
{
	map<int,int>::iterator itr = m_rmapRegionLimit.find(nRegion);
	if(itr == m_rmapRegionLimit.end())
	{
		return m_nCommon;
	}

	return itr->second;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_WaiGuaKick::GameConstantSetLogic_WaiGuaKick(int nLogic):GameConstantSetLogic(nLogic)
{
	m_nKickNum = 0;
}

GameConstantSetLogic_WaiGuaKick::~GameConstantSetLogic_WaiGuaKick(void){}

void	GameConstantSetLogic_WaiGuaKick::Init(string* pStr,int* pInt)
{
	if(pInt)
	{
		m_nKickNum = pInt[0];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_PVEPaTa::GameConstantSetLogic_PVEPaTa(int nLogic):GameConstantSetLogic(nLogic)
{
	m_nCntFree = 0;
	m_nCntToll = 0;
	m_nCredit = 0;
}

GameConstantSetLogic_PVEPaTa::~GameConstantSetLogic_PVEPaTa(void){}

void	GameConstantSetLogic_PVEPaTa::Init(string* pStr,int* pInt)
{
	if(pInt)
	{
		m_nCntFree = pInt[0];
		m_nCntToll = pInt[1];
		m_nCredit = pInt[2];
		m_bPveOpen = pInt[3] > 0;
		m_bStrategicOpen = pInt[4] > 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_StrategicGeneral::GameConstantSetLogic_StrategicGeneral(int nLogic):GameConstantSetLogic(nLogic)
{
	m_nCommonAdd = 0;
	m_nCommonNum = 0;
	m_nCommonLand = 0;
	m_nCommonAir = 0;
    m_nMaxArmyNum = 0;
}

GameConstantSetLogic_StrategicGeneral::~GameConstantSetLogic_StrategicGeneral(void){}

void	GameConstantSetLogic_StrategicGeneral::Init(string* pStr,int* pInt)
{
	if(pInt)
	{
		m_nCommonAdd = pInt[0];
		m_nCommonNum = pInt[1];
		m_nCommonLand = pInt[2];
		m_nCommonAir = pInt[3];
        m_nMaxArmyNum = pInt[4];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_Explore::GameConstantSetLogic_Explore(int nLogic):GameConstantSetLogic(nLogic)
{
	m_nMaxExploreTimes = 0;
	m_nMaxExploredTimes = 0;
	m_nCDTime = 0;
}

GameConstantSetLogic_Explore::~GameConstantSetLogic_Explore(void){}

void	GameConstantSetLogic_Explore::Init(string* pStr,int* pInt)
{
	if(pInt)
	{
		m_nMaxExploreTimes = pInt[0];
		m_nMaxExploredTimes = pInt[1];
		m_nCDTime = pInt[2];
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_ArenaConfig::GameConstantSetLogic_ArenaConfig(int nLogic):GameConstantSetLogic(nLogic)
{
	m_rVecBuyCredit.clear();
	m_nPlayerLvl = 0;
	m_nPveCustom = 0;
	m_nPlayTimeFree = 0;
	m_nPlayTimeCredit = 0;
	m_nWinScore = 0;
	m_nFailScore = 0;
}

GameConstantSetLogic_ArenaConfig::~GameConstantSetLogic_ArenaConfig(void){}

void	GameConstantSetLogic_ArenaConfig::Init(string* pStr,int* pInt)
{
	if(pStr)
	{
		m_rVecBuyCredit.clear();
		string strInput = pStr[0].c_str();
		strInput = strInput.substr(strInput.find(",")+1);

		static string delims = ",";
		vector<int> tokens;
		tokenize_as_int(strInput, m_rVecBuyCredit, delims);

		strInput = pStr[1].c_str();
		safe_atoi(strInput,m_nArenaOpen);

		strInput = pStr[2].c_str();
		int nTmpVal = 0;
		safe_atoi(strInput,nTmpVal);
		m_bClearAll = nTmpVal > 0;
	}

	if(pInt)
	{
		m_nPlayerLvl = pInt[0];
		m_nPveCustom = pInt[1];
		m_nPlayTimeFree = pInt[2];
		m_nPlayTimeCredit = pInt[3];
		m_nWinScore = pInt[4];
		m_nFailScore = pInt[5];
	}
}

int		GameConstantSetLogic_ArenaConfig::GetBuyCreditByTimes(int nTime)
{
	if(nTime < 0 || nTime >= (int)m_rVecBuyCredit.size())
	{
		return 0;
	}

	return m_rVecBuyCredit[nTime];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_WorldArena::GameConstantSetLogic_WorldArena(int nLogic):GameConstantSetLogic(nLogic)
{
	m_rVecBuyCredit.clear();
	m_rmapArenaOpen.clear();
	m_nPlayTimeFree = 0;
	m_nPlayTimeCredit = 0;
	m_nWinScore =0;
	m_nFailScore = 0;

}

GameConstantSetLogic_WorldArena::~GameConstantSetLogic_WorldArena(void){}

void	GameConstantSetLogic_WorldArena::Init(string* pStr,int* pInt)
{
	if(pStr)
	{
		m_rVecBuyCredit.clear();
		string strInput = pStr[0].c_str();
		strInput = strInput.substr(strInput.find(",")+1);

		static string delims = ",";
		tokenize_as_int(strInput, m_rVecBuyCredit, delims);

		strInput = pStr[1].c_str();
		vector<string> tokens;
		tokenize(strInput, tokens, delims);

		int nValue = 0;
		for(int i = 1; i<(int)tokens.size(); i++)
		{
			safe_atoi(tokens[i].c_str(), nValue);
			m_rmapArenaOpen[i-1] = nValue;
		}

		strInput = pStr[2].c_str();
		int nTmpVal = 0;
		safe_atoi(strInput,nTmpVal);
		m_bClearAll = nTmpVal > 0;
	}

	if(pInt)
	{
		m_nPlayTimeFree = pInt[0];
		m_nPlayTimeCredit = pInt[1];
		m_nWinScore = pInt[2];
		m_nFailScore = pInt[3];
	}
}
int		GameConstantSetLogic_WorldArena::GetBuyCreditByTimes(int nTime)
{
	if(nTime < 0 || nTime >= (int)m_rVecBuyCredit.size())
	{
		return 0;
	}

	return m_rVecBuyCredit[nTime];
}

int		GameConstantSetLogic_WorldArena::GetArenaOpenByRegion(int region)
{
	map<int,int>::iterator itr = m_rmapArenaOpen.find(region);
	if(itr == m_rmapArenaOpen.end())
	{
		return 0;
	}
	return itr->second;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_Officer::GameConstantSetLogic_Officer(int nLogic):GameConstantSetLogic(nLogic)
{
	m_bOpen = false;
}

GameConstantSetLogic_Officer::~GameConstantSetLogic_Officer(void){}

void	GameConstantSetLogic_Officer::Init(string* pStr,int* pInt)
{
	if(pInt)
	{
		m_bOpen = pInt[0]>0?true:false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_RegionArena::GameConstantSetLogic_RegionArena(int nLogic):GameConstantSetLogic(nLogic)
{
	m_rVecBuyCredit.clear();
	m_nArenaOpen=0;
	m_nPlayTimeFree = 0;
	m_nPlayTimeCredit = 0;
	m_nRankGift  = 0;
	m_nPveCustom = 0;

}

GameConstantSetLogic_RegionArena::~GameConstantSetLogic_RegionArena(void){}

void	GameConstantSetLogic_RegionArena::Init(string* pStr,int* pInt)
{
	if(pStr)
	{
		m_rVecBuyCredit.clear();
		string strInput = pStr[0].c_str();
		strInput = strInput.substr(strInput.find(",")+1);

		static string delims = ",";
		tokenize_as_int(strInput, m_rVecBuyCredit, delims);

		strInput = pStr[1].c_str();
		safe_atoi(strInput, m_nArenaOpen);

		strInput = pStr[2].c_str();
		int nTmpVal = 0;
		safe_atoi(strInput,nTmpVal);
		m_bClearAll = nTmpVal > 0;
	}

	if(pInt)
	{
		m_nPlayTimeFree = pInt[0];
		m_nPlayTimeCredit = pInt[1];
		m_nRankGift = pInt[2];
		m_nPveCustom = pInt[3];
	}
}
int		GameConstantSetLogic_RegionArena::GetBuyCreditByTimes(int nTime)
{
	if(nTime < 0 || nTime >= (int)m_rVecBuyCredit.size())
	{
		return 0;
	}

	return m_rVecBuyCredit[nTime];
}

int		GameConstantSetLogic_RegionArena::GetArenaOpen()
{
	return m_nArenaOpen;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GameConstantSetLogic_PlayerDefenseNpc::GameConstantSetLogic_PlayerDefenseNpc(int nLogic):GameConstantSetLogic(nLogic)
{
	m_bWaiGuaKickEnable = true;
}
GameConstantSetLogic_PlayerDefenseNpc::~GameConstantSetLogic_PlayerDefenseNpc(void)
{

}
void GameConstantSetLogic_PlayerDefenseNpc::Init(string* pStr,int* pInt)
{
	if(pStr == NULL || pInt == NULL)
	{
		return;
	}

	string tmpstr = pStr[0];
	vector<string> tokens;
	static string delims = ",";
	tokenize(tmpstr, tokens, delims);

	int nValue = 0;
	safe_atoi(tokens[0].c_str(), nValue);
	m_bWaiGuaKickEnable = nValue>0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_HeroArena::GameConstantSetLogic_HeroArena(int nLogic):GameConstantSetLogic(nLogic)
{
	m_rVecBuyCredit.clear();
	m_rmapArenaOpen.clear();
	m_nPlayTimeFree = 0;
	m_nPlayTimeCredit = 0;
	m_nWinScore =0;
	m_nFailScore = 0;

}

GameConstantSetLogic_HeroArena::~GameConstantSetLogic_HeroArena(void){}

void	GameConstantSetLogic_HeroArena::Init(string* pStr,int* pInt)
{
	if(pStr)
	{
		m_rVecBuyCredit.clear();
		string strInput = pStr[0].c_str();
		strInput = strInput.substr(strInput.find(",")+1);

		static string delims = ",";
		tokenize_as_int(strInput, m_rVecBuyCredit, delims);

		strInput = pStr[1].c_str();
		vector<string> tokens;
		tokenize(strInput, tokens, delims);

		int nValue = 0;
		for(int i = 1; i<(int)tokens.size(); i++)
		{
			safe_atoi(tokens[i].c_str(), nValue);
			m_rmapArenaOpen[i-1] = nValue;
		}

		strInput = pStr[2].c_str();
		int nTmpVal = 0;
		safe_atoi(strInput,nTmpVal);
		m_bClearAll = nTmpVal > 0;
	}

	if(pInt)
	{
		m_nPlayTimeFree = pInt[0];
		m_nPlayTimeCredit = pInt[1];
		m_nWinScore = pInt[2];
		m_nFailScore = pInt[3];
	}
}
int		GameConstantSetLogic_HeroArena::GetBuyCreditByTimes(int nTime)
{
	if(nTime < 0 || nTime >= (int)m_rVecBuyCredit.size())
	{
		return 0;
	}

	return m_rVecBuyCredit[nTime];
}

int		GameConstantSetLogic_HeroArena::GetArenaOpenByRegion(int region)
{
	map<int,int>::iterator itr = m_rmapArenaOpen.find(region);
	if(itr == m_rmapArenaOpen.end())
	{
		return 0;
	}
	return itr->second;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameConstantSetLogic_ExploreAsrenal::GameConstantSetLogic_ExploreAsrenal(int nLogic)
:GameConstantSetLogic(nLogic)
,m_tStartTime(0)
,m_tEndTime(0)
,m_nFreeNum(0)
,m_nCostCredit(0)
,m_nMaxExploreNum(0)
{
    //
}
GameConstantSetLogic_ExploreAsrenal::~GameConstantSetLogic_ExploreAsrenal(void)
{
    //
}

void GameConstantSetLogic_ExploreAsrenal::Init(string* pStr,int* pInt)
{
    if (pStr)
    {
        m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
        m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
    }

    if (pInt)
    {
        m_nFreeNum = pInt[0];
        m_nCostCredit = pInt[1];
        m_nMaxExploreNum = pInt[2];
        m_nGetRes = pInt[3];
    }
}

void GameConstantSetLogic_CostCreditGift::Init(string* pStr,int* pInt)
{
    if (pStr)
    {
        m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
        m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
    }

    if (pInt)
    {
        m_giftList.clear();
        m_giftList.push_back(pInt[0]);
        m_giftList.push_back(pInt[1]);
        m_giftList.push_back(pInt[2]);
        m_giftList.push_back(pInt[3]);
    }
}

GameConstantSetLogic_CostCreditGift::GameConstantSetLogic_CostCreditGift(int nLogic)
:GameConstantSetLogic(nLogic)
,m_tStartTime(0)
,m_tEndTime(0)
{
    //
}
GameConstantSetLogic_CostCreditGift::~GameConstantSetLogic_CostCreditGift(void)
{
    //
}

GameConstantSetLogic_AdmiralSoulBuy::GameConstantSetLogic_AdmiralSoulBuy(int nLogic)
:GameConstantSetLogic(nLogic)
,m_nFlag(0)
,m_nNum(0)
{
    //
}
GameConstantSetLogic_AdmiralSoulBuy::~GameConstantSetLogic_AdmiralSoulBuy(void)
{
    //
}

void GameConstantSetLogic_AdmiralSoulBuy::Init(string* pStr,int* pInt)
{
    if(pStr == NULL || pInt == NULL)
    {
        return;
    }

    //解析是否开启
    string tmpstr = pStr[0];
    vector<string> tokens;
    string delims = ",";
    tokenize(tmpstr, tokens, delims);

    int nValue = 0;
    safe_atoi(tokens[0].c_str(), nValue);
    m_nFlag = nValue;

    //解析次数
    tmpstr = pStr[1];
    tokens.clear();
    tokenize(tmpstr, tokens, delims);

    nValue = 0;
    safe_atoi(tokens[0].c_str(), nValue);
    m_nNum = nValue;
}

void GameConstantSetLogic_CreditRebate::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}
}

void GameConstantSetLogic_HappyCard::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}

	if (pInt)
	{
		m_nLabCostItmeID	= pInt[0];
		m_nLabChgItemID		= pInt[1];

		m_nGreatCostItemID	= pInt[2];
		m_nGreatChgItemID	= pInt[3];
	}
}

void GameConstantSetLogic_GetAdmiral::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}

	if (pInt)
	{
		m_nCountLimit	= pInt[0];
	}
}


void GameConstantSetLogic_WeaponSearch::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}

}

void GameConstantSetLogic_OpenBox::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}

	if (pInt)
	{
		m_nIntermediateBoxScore	= pInt[0];
		m_nSeniorBoxScore = pInt[1];
	}
}

void GameConstantSetLogic_NewReCall::Init(string* pStr,int* pInt)
{
	if (pStr)
	{
		m_tStartTime = GetDayTimeByStr(pStr[0].c_str(),pStr[1].c_str());
		m_tEndTime = GetDayTimeByStr(pStr[2].c_str(),pStr[3].c_str());
	}

	if (pInt)
	{
		m_nMinDay				= pInt[0];
		m_nMinDayGiftCnt		= pInt[1];

		m_nLargeDay				= pInt[2];
		m_nLargeDayGiftCnt		= pInt[3];
	}
}

int GameConstantSetLogic_NewReCall::GetGiftCnt(int nDay)
{
	if(nDay>=m_nLargeDay)
		return m_nLargeDayGiftCnt;

	if(nDay>=m_nMinDay&&nDay<m_nLargeDay)
		return m_nMinDayGiftCnt;
	return 0;
}



