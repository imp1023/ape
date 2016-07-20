#include "PlatInviteFriendMgr.h"
#include "../common/DBC.h"
#include "../common/distribution.h"
#include <time.h>


PlatInviteFriendMgr PlatInviteFriendMgr::m_platinvitefriendmgr;


const char* szPlatInviteFile		= "Config/inviteFriend.dat";

////////////////////////////////////////////////////////////////////////////////////////////////
PlatInviteFriendMgr::PlatInviteFriendMgr()
{
	Clear();
}

PlatInviteFriendMgr::~PlatInviteFriendMgr()
{

}

void PlatInviteFriendMgr::Clear()
{
	m_rSet.Clear();
}

bool PlatInviteFriendMgr::LoadCfgFile()
{
	Clear();
	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szPlatInviteFile);
	int nRow = fileDBC.GetRecordsNum();

	if(nRow > 1 || nRow <= 0)
	{
		printf("Load %s cnt:%d\n", szPlatInviteFile, nRow);
		return false;
	}

	m_rSet.m_tBeginTime = GetDayTimeByStr(fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_BG_D)->pString,
										fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_BG_T)->pString);

	m_rSet.m_tEndTime = GetDayTimeByStr(fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_ED_D)->pString,
										fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_ED_T)->pString);

	m_rSet.m_ninviteCredit = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_INVCREDIT)->iValue;

	m_rSet.m_ninviteCreditMax = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_INVCREDITMAX)->iValue;

	for(int i=0;i<PLATINVITE_LVLCREDIT_NUM;i++)
	{
		m_rSet.m_lvlCreditGroup[i].m_nLvl = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_LVLCREDIT_BG + i*2)->iValue;
		m_rSet.m_lvlCreditGroup[i].m_nCredit = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_LVLCREDIT_BG + i*2 + 1)->iValue;
	}

	m_rSet.m_nLvlCreditMax = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_LVLCREDITMAX)->iValue;
	m_rSet.m_nFriendAddGain = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_FRIENDADDGAIN)->iValue;
	m_rSet.m_nSelfFriendAddGain = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_SELFADDGAIN)->iValue;
	m_rSet.m_nAddCreditMax = fileDBC.Search_Posistion(0,E_LOADPLATINVITE_FRIEND_ADDGAINMAX)->iValue;
	
	return true;
}


bool PlatInviteFriendMgr::IsPlatInviteFriendAvail()
{
	time_t nowtime = time(NULL);
	
	if(nowtime >= m_rSet.m_tBeginTime && nowtime <= m_rSet.m_tEndTime)
	{
		return true;
	}

	return false;
}
