#include "InviteFrdGiftCfg.h"
#include "../common/DBC.h"
#include "../common/string-util.h"
#include "../common/distribution.h"
#include <time.h>

const char* InviteFrdGiftCfg::szGiftFile		= "Config/InviteFrdGift.dat";
const char* InviteFrdGiftCfg::szReLvGiftFile	= "Config/InvitedFrdLvGift.dat";


InviteFrdGiftCfg::InviteFrdGiftCfg()
{
	ClearTbl();
}

InviteFrdGiftCfg::~InviteFrdGiftCfg()
{
	ClearTbl();
}

bool InviteFrdGiftCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szGiftFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		InviteFrdGiftTbl* pTbl = new InviteFrdGiftTbl();
		pTbl->m_nGiftID = fileDBC.Search_Posistion(i, E_INVITE_GIFT_ID	)->iValue;

		pTbl->m_nFrdCnt	= fileDBC.Search_Posistion(i, E_INVITE_GIFT_FRD_CNT )->iValue;
		pTbl->m_bCanRepeated = ( fileDBC.Search_Posistion(i, E_INVITE_GIFT_CAN_REPEAT )->iValue != 0 );
		pTbl->m_nRepeatCnt	= fileDBC.Search_Posistion(i, E_INVITE_GIFT_REPEAT_CNT )->iValue;

		pTbl->m_nMedal	= fileDBC.Search_Posistion(i, E_INVITE_GIFT_MEDAL	)->iValue;
		pTbl->m_nOil	= fileDBC.Search_Posistion(i, E_INVITE_GIFT_OIL	)->iValue;
		pTbl->m_nHonor	= fileDBC.Search_Posistion(i, E_INVITE_GIFT_HONOR	)->iValue;
		pTbl->m_nCredit = fileDBC.Search_Posistion(i, E_INVITE_GIFT_CREDIT	)->iValue;

		int nType=0;
		int nCnt =0;
		for(int j=0;j<MAX_INVITE_GIFT_ITEM_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_INVITE_GIFT_Item1 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_INVITE_GIFT_ItemCnt1 + j*2)->iValue;

			pTbl->m_lstItemType.push_back(nType);
			pTbl->m_lstItemCnt.push_back(nCnt);
		}
		for(int j=0;j<MAX_INVITE_GIFT_ARMY_CNT;j++)
		{
			nType	= fileDBC.Search_Posistion(i,E_INVITE_GIFT_ARMY0 + j*2	)->iValue;
			nCnt	= fileDBC.Search_Posistion(i,E_INVITE_GIFT_CNT0 + j*2 )->iValue;

			pTbl->m_lstArmyType.push_back(nType);
			pTbl->m_lstArmyCnt.push_back(nCnt);
		}


		m_mapGift[pTbl->m_nGiftID] = pTbl;
	}
	printf("Load %s cnt:%d\n", szGiftFile, nRow);

	LoadReLvTbl();
	return true;

}

void InviteFrdGiftCfg::ClearTbl()
{
	map<int,InviteFrdGiftTbl*>::iterator iter;
	for(iter = m_mapGift.begin(); iter!= m_mapGift.end();iter++)
	{
		delete iter->second;
	}
	m_mapGift.clear();

	map<int,InvitedFrdLvGiftTbl*>::iterator iterRe;
	for(iterRe = m_mapReLvGift.begin(); iterRe!= m_mapReLvGift.end();iterRe++)
	{
		delete iterRe->second;
	}
	m_mapReLvGift.clear();
}


const InviteFrdGiftTbl* InviteFrdGiftCfg::GetInviteGiftTbl(int nGiftID)
{
	map<int,InviteFrdGiftTbl*>::iterator iter;
	iter = m_mapGift.find(nGiftID);
	if(iter!=m_mapGift.end())
		return iter->second;
	return NULL;
}

const InviteFrdGiftTbl* InviteFrdGiftCfg::GetInviteGiftTblByNum( int nNum )
{
	if ( nNum >= (int)m_mapGift.size() )
	{
		return NULL;
	}
	map<int,InviteFrdGiftTbl*>::iterator iter;
	iter = m_mapGift.begin();

	for ( int i = 0; i < nNum; i++ )
	{
		iter++;
	}
	return iter->second;
}

const InvitedFrdLvGiftTbl* InviteFrdGiftCfg::GetReLvGiftTblByNum( int nNum )
{
	if ( nNum >= (int)m_mapReLvGift.size() )
	{
		return NULL;
	}
	map<int,InvitedFrdLvGiftTbl*>::iterator iter;
	iter = m_mapReLvGift.begin();

	for ( int i = 0; i < nNum; i++ )
	{
		iter++;
	}
	return iter->second;
}

const InvitedFrdLvGiftTbl* InviteFrdGiftCfg::GetReLvGiftTbl( int nGiftID )
{

	map<int,InvitedFrdLvGiftTbl*>::iterator iter;
	iter = m_mapReLvGift.find(nGiftID);
	if ( iter != m_mapReLvGift.end() )
	{
		return iter->second;
	}

	return NULL;
}


bool InviteFrdGiftCfg::LoadReLvTbl()
{
	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szReLvGiftFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		InvitedFrdLvGiftTbl* pTbl = new InvitedFrdLvGiftTbl();
		int nLv,nGiftID;
		pTbl->m_nID		= fileDBC.Search_Posistion(i,E_RE_LV_GIFT_ID )->iValue;
		pTbl->m_nLv		= fileDBC.Search_Posistion(i,E_RE_LV_GIFT_PLAYER_LEVEL )->iValue;
		pTbl->m_nCoupon	= fileDBC.Search_Posistion(i,E_RE_LV_GIFT_GIFT_ID )->iValue;

		m_mapReLvGift[pTbl->m_nID] = pTbl;
	}
	printf("Load %s cnt:%d\n", szReLvGiftFile, nRow);
	return true;
}