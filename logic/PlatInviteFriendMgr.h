#pragma once

#include "../common/const_def.h"

#define  PLATINVITE_LVLCREDIT_NUM 4


struct PlatInviteLvlCredit
{
	int			m_nLvl;
	int			m_nCredit;
};

struct PlatInviteFriendSet
{
	time_t		m_tBeginTime;
	time_t		m_tEndTime;
	int			m_ninviteCredit;
	int			m_ninviteCreditMax;
	PlatInviteLvlCredit m_lvlCreditGroup[PLATINVITE_LVLCREDIT_NUM];
	int			m_nLvlCreditMax;
	int			m_nFriendAddGain;
	int			m_nSelfFriendAddGain;
	int			m_nAddCreditMax;

	void		Clear()
	{
		m_tBeginTime = 0;
		m_tEndTime = 0;
		m_ninviteCredit = 0;
		m_ninviteCreditMax = 0;
		for(int i=0;i<PLATINVITE_LVLCREDIT_NUM;i++)
		{
			m_lvlCreditGroup[i].m_nCredit = 0;
			m_lvlCreditGroup[i].m_nLvl = 0;
		}
		m_nLvlCreditMax = 0;
		m_nFriendAddGain = 0;
		m_nSelfFriendAddGain = 0;
		m_nAddCreditMax = 0;
	}
};


class PlatInviteFriendMgr
{
	enum
	{
		E_LOADPLATINVITE_FRIEND_ID = 0,
		E_LOADPLATINVITE_FRIEND_DESC,
		E_LOADPLATINVITE_FRIEND_BG_D,
		E_LOADPLATINVITE_FRIEND_BG_T,
		E_LOADPLATINVITE_FRIEND_ED_D,
		E_LOADPLATINVITE_FRIEND_ED_T,
		E_LOADPLATINVITE_FRIEND_INVCREDIT,
		E_LOADPLATINVITE_FRIEND_INVCREDITMAX,
		E_LOADPLATINVITE_FRIEND_LVLCREDIT_BG,
		E_LOADPLATINVITE_FRIEND_LVLCREDIT_ED = E_LOADPLATINVITE_FRIEND_LVLCREDIT_BG + PLATINVITE_LVLCREDIT_NUM*2 -1,
		E_LOADPLATINVITE_FRIEND_LVLCREDITMAX,
		E_LOADPLATINVITE_FRIEND_FRIENDADDGAIN,
		E_LOADPLATINVITE_FRIEND_SELFADDGAIN,
		E_LOADPLATINVITE_FRIEND_ADDGAINMAX,
	};
public:
	PlatInviteFriendMgr();
	~PlatInviteFriendMgr();

	void Clear();
	bool LoadCfgFile();

	PlatInviteFriendSet* GetPlatInviteFriendSet(){return &m_rSet;}

	bool IsPlatInviteFriendAvail();

	static PlatInviteFriendMgr&	Instance(){return m_platinvitefriendmgr;}
private:
	static PlatInviteFriendMgr		 m_platinvitefriendmgr;

	PlatInviteFriendSet		m_rSet;
};

