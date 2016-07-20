#include "FriendGiftCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include <time.h>

const char* FriendGiftCfg::szGiftFile		= "Config/FriendGift.dat";

const int FRD_GIFT_ARMY_CNT = 3;
const int FRD_GIFT_Item_CNT = 4;

/////////////////////////////////////////////////////////////////////////////////////////

bool FrdGiftTbl::IsCurrentCanGainGift() const
{
	if(m_tBeginTime == 0 && m_tEndTime == 0)
	{
		return true;
	}

	time_t now = time(NULL);
	if(now >= m_tBeginTime && now <= m_tEndTime)
	{
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
FriendGiftCfg::FriendGiftCfg()
{
	ClearTbl();
}

FriendGiftCfg::~FriendGiftCfg()
{
	ClearTbl();
}

bool FriendGiftCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szGiftFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		FrdGiftTbl* pTbl = new FrdGiftTbl();
		pTbl->m_nGiftID = fileDBC.Search_Posistion(i, E_GIFT_ID	)->iValue;
		pTbl->m_nNeedLV = fileDBC.Search_Posistion(i, E_GIFT_LV	)->iValue;
		pTbl->m_nType	= fileDBC.Search_Posistion(i, E_GIFT_TYPE	)->iValue;

		pTbl->m_tBeginTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_GIFT_TIME_BG_D)->pString
			,fileDBC.Search_Posistion(i,E_GIFT_TIME_BG_T)->pString);
		pTbl->m_tEndTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_GIFT_TIME_ED_D)->pString
			,fileDBC.Search_Posistion(i,E_GIFT_TIME_ED_T)->pString);

		pTbl->m_nMedal	= fileDBC.Search_Posistion(i, E_GIFT_MEDAL	)->iValue;
		pTbl->m_nOil	= fileDBC.Search_Posistion(i, E_GIFT_OIL	)->iValue;
		pTbl->m_nCredit	= fileDBC.Search_Posistion(i, E_GIFT_CREDIT	)->iValue;
		pTbl->m_nHonor	= fileDBC.Search_Posistion(i, E_GIFT_HONOR	)->iValue;
		
		int nType=0;
		int nCnt =0;
		for(int j=0;j<FRD_GIFT_Item_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_GIFT_Item1 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_GIFT_ItemCnt1 + j*2)->iValue;
			if(nType>0&&nCnt>0)
			{
				pTbl->m_lstItemType.push_back(nType);
				pTbl->m_lstItemCnt.push_back(nCnt);
			}
		}
		for(int j=0;j<FRD_GIFT_ARMY_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_GFIT_ARMY0 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_GIFT_CNT0 + j*2 )->iValue;
			if(nType>0&&nCnt>0)
			{
				pTbl->m_lstArmyType.push_back(nType);
				pTbl->m_lstArmyCnt.push_back(nCnt);
			}
		}


		pTbl->m_nWebType	= fileDBC.Search_Posistion(i, E_GIFT_WEBTYPE	)->iValue;
		pTbl->m_nWebFlag	= fileDBC.Search_Posistion(i, E_GIFT_WEBFLAG	)->iValue;

		pTbl->m_nGainOneDay	= fileDBC.Search_Posistion(i, E_GIFT_GAINONEDAY	)->iValue;

		pTbl->m_nCoupon 	= fileDBC.Search_Posistion(i, E_GIFT_COUPON		)->iValue;

		if(pTbl->m_nType<FG_NoUse||pTbl->m_nType>=FG_Cnt)
		{
			pTbl->m_nType = FG_Other;
		}
		m_mapGift[pTbl->m_nGiftID] = pTbl;
	}
	printf("Load %s cnt:%d\n", szGiftFile, nRow);
	return true;

}

void FriendGiftCfg::ClearTbl()
{
	map<int,FrdGiftTbl*>::iterator iter;
	for(iter = m_mapGift.begin(); iter!= m_mapGift.end();iter++)
	{
		delete iter->second;
	}
	m_mapGift.clear();
}


const FrdGiftTbl* FriendGiftCfg::GetFrdGiftTbl(int nGiftID)
{
	map<int,FrdGiftTbl*>::iterator iter;
	iter = m_mapGift.find(nGiftID);
	if(iter!=m_mapGift.end())
		return iter->second;
	return NULL;
}
