#include "CostGiftCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include <time.h>

const char* CostGiftCfg::szGiftFile		= "Config/CostGift.dat";

/////////////////////////////////////////////////////////////////////////////////////////

bool CostGiftTbl::IsCurrentCanGainGiftByCredit( int nCredit ) const
{
	//非充值礼物
	if ( m_nCostCredit == 0 )
	{
		return true;
	}

	if( nCredit >= m_nCostCredit )
	{
		return true;
	}

	return false;
}

bool CostGiftTbl::IsCurrentCanGainGiftByYellow( int nYellowTimes ) const
{
	//非续费黄钻礼物
	if ( m_nYellowTimes == 0 )
	{
		return true;
	}

	if( nYellowTimes >= m_nYellowTimes )
	{
		return true;
	}
	return false;
}

bool CostGiftTbl::IsCurrentCanGainGiftByDraw( int nDrawTimes ) const
{
	if ( m_nDrawTimes == 0 )
	{
		return true;
	}

	if( nDrawTimes >= m_nDrawTimes )
	{
		return true;
	}
	return false;
}

bool CostGiftTbl::IsCurrentCanGainGiftByAdmiral( int nAdmiralTimes ) const
{
	if ( m_nAdmiralTimes == 0 )
	{
		return true;
	}

	if( nAdmiralTimes >= m_nAdmiralTimes )
	{
		return true;
	}
	return false;
}

bool CostGiftTbl::IsCurrentCanGainGiftByDailyToken( int nTokens ) const 
{
	if ( m_nToken == 0 )
	{
		return true;
	}

	if( nTokens >= m_nToken )
	{
		return true;
	}
	return false;
}

bool CostGiftTbl::IsCurrentCanGainGiftByCustomFlag( int nCustonTime, int nTimeCheck ) const 
{
	if ( m_nComeBackDay == 0 )
	{
		return true;
	}

	if ( nCustonTime <= 0 )
	{
		return false;
	}

	time_t tCustomTime = (time_t)nCustonTime;

	struct tm tmCustom = *localtime( &tCustomTime );

	time_t tNow;
	if ( nTimeCheck == 0 )
	{
		tNow = time(NULL);
	}
	else
	{
		tNow = (time_t)nTimeCheck;
	}
	
	struct tm tmNow = *localtime( &tNow );

	tmCustom.tm_hour = 0;
	tmCustom.tm_min = 0;
	tmCustom.tm_sec = 0;

	tCustomTime = mktime( &tmCustom );
	tCustomTime += (m_nComeBackDay - 1) * 24*3600;

	tmCustom = *localtime( &tCustomTime );

	if ( tmNow.tm_yday == tmCustom.tm_yday && tmNow.tm_year == tmCustom.tm_year )
	{
		return true;
	}

// 	if ( tmNow.tm_yday - tmCustom.tm_yday == m_nComeBackDay - 1 )
// 	{
// 		return true;
// 	}
	return false;
}

bool CostGiftTbl::IsCurrentCanGainGift( int nCredit, int nYellowTimes, int nDrawTimes, int nAdmiralTimes, int nTokens, int nCustomTime, int nCheckTime ) const
{
	return( IsCurrentCanGainGiftByCredit(nCredit) 
			&& IsCurrentCanGainGiftByYellow(nYellowTimes) 
			&& IsCurrentCanGainGiftByDraw(nDrawTimes) 
			&& IsCurrentCanGainGiftByAdmiral(nAdmiralTimes) 
			&& IsCurrentCanGainGiftByDailyToken(nTokens) 
			&& IsCurrentCanGainGiftByCustomFlag(nCustomTime,nCheckTime) );
}

bool CostGiftTbl::IsCurrentTime( ) const
{
	if( m_tBeginTime == 0 && m_tEndTime == 0 )
	{
		return true;
	}

	time_t now = time(NULL);

	if( now >= m_tBeginTime && ( now <= m_tEndTime || m_tEndTime == 0 ) )
	{
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
CostGiftCfg::CostGiftCfg()
{
	ClearTbl();
}

CostGiftCfg::~CostGiftCfg()
{
	ClearTbl();
}

bool CostGiftCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szGiftFile);
	int nRow = fileDBC.GetRecordsNum();

	time_t tNow = time(NULL);
	for (int i = 0; i < nRow; i++)
	{
		CostGiftTbl* pTbl = new CostGiftTbl();
		pTbl->m_nGiftID = fileDBC.Search_Posistion(i, E_COST_GIFT_ID	)->iValue;

		pTbl->m_tBeginTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_COST_GIFT_TIME_BG_D)->pString
			,fileDBC.Search_Posistion(i,E_COST_GIFT_TIME_BG_T)->pString);
		pTbl->m_tEndTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_COST_GIFT_TIME_ED_D)->pString
			,fileDBC.Search_Posistion(i,E_COST_GIFT_TIME_ED_T)->pString);

		pTbl->m_nTimes	= fileDBC.Search_Posistion(i, E_COST_GIFT_TIMES	)->iValue;

		pTbl->m_nYellowTimes	= fileDBC.Search_Posistion(i, E_COST_GIFT_YELLOW_TIMES )->iValue;

		pTbl->m_nCostCredit	= fileDBC.Search_Posistion(i, E_COST_GIFT_COST_CREDIT )->iValue;

		pTbl->m_nMedal	= fileDBC.Search_Posistion(i, E_COST_GIFT_MEDAL	)->iValue;
		pTbl->m_nOil	= fileDBC.Search_Posistion(i, E_COST_GIFT_OIL	)->iValue;
		pTbl->m_nHonor	= fileDBC.Search_Posistion(i, E_COST_GIFT_HONOR	)->iValue;
		pTbl->m_nFeats	= fileDBC.Search_Posistion(i, E_COST_GIFT_FEATS	)->iValue;
		 
		int nType=0;
		int nCnt =0;
		for(int j=0;j<MAX_COST_GIFT_ITEM_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_COST_GIFT_Item1 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_COST_GIFT_ItemCnt1 + j*2)->iValue;

			pTbl->m_lstItemType.push_back(nType);
			pTbl->m_lstItemCnt.push_back(nCnt);
		}
		for(int j=0;j<MAX_COST_GIFT_ARMY_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_COST_GIFT_ARMY0 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_COST_GIFT_CNT0 + j*2 )->iValue;

			pTbl->m_lstArmyType.push_back(nType);
			pTbl->m_lstArmyCnt.push_back(nCnt);
		}

		pTbl->m_nAdmiralID	= fileDBC.Search_Posistion(i, E_COST_GIFT_ADMIRAL )->iValue;
		pTbl->m_nAdmiralLV	= fileDBC.Search_Posistion(i, E_COST_GIFT_ADMIRAL_LV )->iValue;

		pTbl->m_bNeedGet	= (fileDBC.Search_Posistion(i, E_COST_GIFT_NEED_GET )->iValue>0);
		pTbl->m_nGetCredit	= fileDBC.Search_Posistion(i, E_COST_GIFT_GET_CREDIT )->iValue;

		pTbl->m_bCostCreditPlus	= (fileDBC.Search_Posistion(i, E_COST_GIFT_COST_CREDIT_PLUS )->iValue>0);
		pTbl->m_nDrawTimes = fileDBC.Search_Posistion(i, E_COST_GIFT_DRAW_TIMES )->iValue;
		pTbl->m_nAdmiralTimes = fileDBC.Search_Posistion(i, E_COST_GIFT_ADMIRAL_TIMES )->iValue;
		pTbl->m_nType = fileDBC.Search_Posistion(i, E_COST_GIFT_TYPE )->iValue;
		pTbl->m_nToken = fileDBC.Search_Posistion(i, E_COST_GIFT_TOKEN )->iValue;
		pTbl->m_nThew = fileDBC.Search_Posistion(i, E_COST_GIFT_THEW )->iValue;
		pTbl->m_nInviteFrd = fileDBC.Search_Posistion(i, E_COST_GIFT_INVITE_FRIEND )->iValue;

		pTbl->m_bAutoGet = (fileDBC.Search_Posistion(i, E_COST_GIFT_AUTO_GET )->iValue>0);

		pTbl->m_nComeBackDay = fileDBC.Search_Posistion(i, E_COST_GIFT_COME_BACK )->iValue;
		pTbl->m_nYellowTypes = fileDBC.Search_Posistion(i, E_COST_GIFT_YELLOW_TYPES )->iValue;

		if ( tNow > pTbl->m_tEndTime && pTbl->m_tEndTime != 0 && !pTbl->m_bAutoGet && pTbl->m_nYellowTypes == 0 )
		{
			delete pTbl;
			continue;
		}

		m_mapGift[pTbl->m_nGiftID] = pTbl;
	}
	printf("Load %s cnt:%d\n", szGiftFile, nRow);
	return true;

}

void CostGiftCfg::ClearTbl()
{
	map<int,CostGiftTbl*>::iterator iter;
	for(iter = m_mapGift.begin(); iter!= m_mapGift.end();iter++)
	{
		delete iter->second;
	}
	m_mapGift.clear();
}


const CostGiftTbl* CostGiftCfg::GetCostGiftTbl(int nGiftID)
{
	map<int,CostGiftTbl*>::iterator iter;
	iter = m_mapGift.find(nGiftID);
	if(iter!=m_mapGift.end())
		return iter->second;
	return NULL;
}

const CostGiftTbl* CostGiftCfg::GetCostGiftTblByNum( int nNum )
{
	if ( nNum >= (int)m_mapGift.size() )
	{
		return NULL;
	}
	map<int,CostGiftTbl*>::iterator iter;
	iter = m_mapGift.begin();

	for ( int i = 0; i < nNum; i++ )
	{
		iter++;
	}
	return iter->second;
}
