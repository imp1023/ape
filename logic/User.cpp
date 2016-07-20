#include "Player.h"
#include "../common/Clock.h"
#include "../common/SysLog.h"
#include "../../logic/GameConstantSetCfg.h"
//#include "../event/EventDefine.h"

extern ServerConfig serverConfig;

User::User()
{
	Init();
}

User::User(int64 id, const string& pid, const string &name,
		   const string &profile_link, int gender, PLAT_TYPE plat_type,
		   bool bIsYellowDmd, bool bIsYellowDmdYear, int i4YellowDmdLv,
		   const vector<string> &friends_platid,int nRegion,int nCity,bool bIsHighYellowDmd , bool bIsHighDmdYear, int nBlueTime, int nBlueYearTime,int nHighBlueTime,
		   int nHighBlueYearTime)
{
	Init();

	m_dbUser.set_id(id);
	m_dbUser.set_platform_id(pid);
	m_dbUser.set_name(plat_type, name);
	m_dbUser.set_profile_link(plat_type, profile_link);
	m_dbUser.set_region(nRegion);
	m_dbUser.set_city(nCity);
	//关联player数据库
	m_pPlayer->InitDB(m_dbUser.mutable_player());
	//m_pPlayer->InitNpcDB(&m_dbNPCList);
	m_pPlayer->InitNewUserFromCfg();

	SetGender(gender);
	SetProfileLink(profile_link,plat_type);
	SetName(name,plat_type);
 	plat_type_ = plat_type;
//  	if (plat_type == PLAT_QHALL)
//  	{
// 		SetQHallDmd(bIsYellowDmd);
// 		SetQHallDmdYear(bIsYellowDmdYear);
// 		SetHighQHallDmd(bIsHighYellowDmd);
// 		SetHighQHallDmdYear(bIsHighDmdYear);
// 		SetQHallDmdLvl(i4YellowDmdLv);
// 		m_dbUser.set_bluetime(nBlueTime);
// 		m_dbUser.set_blueyeartime(nBlueYearTime);
// 		m_dbUser.set_highbluetime(nHighBlueTime);
// 		m_dbUser.set_highblueyeartime(nHighBlueYearTime);
// 	}
// 	else{
		if(plat_type != PLAT_QAdd)
		{
			m_dbUser.set_isyellowdmd(bIsYellowDmd);
			m_dbUser.set_isyellowdmdyear(bIsYellowDmdYear);
			m_dbUser.set_yellowdmdlvl(i4YellowDmdLv);
		}
		else
		{
			m_dbUser.set_isqqdmd(bIsYellowDmd);
			m_dbUser.set_isqqdmdyear(bIsYellowDmdYear);
			m_dbUser.set_qqdmdlvl(i4YellowDmdLv);
		}
		m_dbUser.set_ishighyellowdmd(bIsHighYellowDmd);
// 	}

	// 注册时间为空才设置
	if (m_dbUser.regist_time() <= 0)
	{
		m_dbUser.set_regist_time(time(NULL));
	}
}

User::~User(void)
{
	if (m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}

void User::Init()
{
    fd_					= 0;
    secret_				= 0;
    secret_gentime_		= 0;
    timeLogin_			= 0;
    revision_			= 0;
	mem_revision_		= 0;
    bOnline_			= false;
    m_pPlayer			= NULL;
	rmrevision_			= 0;
	plat_type_			= PLAT_XIAOYOU;
	m_nRegDays			= 0;
	friends_id_.clear();
	friends_platid_.clear();

	m_pPlayer = new Player(this);
    InitDBUser();
	m_strRc4Send.clear();
	m_strRc4Receive.clear();

}

void User::InitDBUser()
{
	m_dbUser.Clear();
	m_dbUser.set_id(0);

	m_dbUser.set_last_login_ip("");
	m_dbUser.set_last_login_time(0);
	m_dbUser.set_platform_id("");
	m_dbUser.set_regist_time(0);

	for (int i = 0; i < PLAT_TYPE_MAX; i++)
	{
		m_dbUser.add_name("");
		m_dbUser.add_profile_link("");
	}
    OnSetDbUser();
}

void User::OnSetDbUser()
{
	if (m_pPlayer == NULL)
	{
		m_pPlayer = new Player(this);
	}
	m_pPlayer->InitDB(m_dbUser.mutable_player());
	//m_pPlayer->CacuStat();

	time_t ltNow = time(NULL);
 	time_t ltReg = GetRegisterTime();
 	if(ltReg>0)
 	{
 		static int nOneDay = 24*60*60;
 		m_nRegDays = (int)((ltNow-ltReg)/nOneDay);
 	}
}

void User::InitNewUser()
{
	//m_dbUser.set_name("TestPlayer");
	//m_dbUser.set_head(0);
	SetRegisterTime(time(NULL));
	m_pPlayer->InitPlayerData();	
}

DB_User& User::GetDbUser()
{
    return m_dbUser;
}

const DB_User& User::GetDbUser() const
{
    return m_dbUser;
}

DB_Player* User::GetDBPlayer()
{
	return m_dbUser.mutable_player();
}

int User::GetUserLevel(void) const
{
   return m_dbUser.player().model().level();
}

int User::GetUserExp(void) const
{
	return m_dbUser.player().model().exp();
}

int User::GetUserTotalExp(void) const
{
	return 0;
}

void User::SetDbUser(const DB_User& dbuser)
{
    m_dbUser.Clear();
    m_dbUser.CopyFrom(dbuser);
    OnSetDbUser();
	////防止被覆盖
	for (int i = m_dbUser.name_size(); i < PLAT_TYPE_MAX; i++)
	{
		m_dbUser.add_name("");
		m_dbUser.add_profile_link("");
	}
}

bool User::IsNewLoginMonth() const
{
	time_t lasttime = GetLastLoginTime();
	struct tm lastlogintime;
	localtime_r(&lasttime,&lastlogintime);

	time_t now = time(NULL);
	struct tm nowtime;
	localtime_r(&now,&nowtime);

	if (lastlogintime.tm_year == nowtime.tm_year)//同一年
	{
		if (lastlogintime.tm_mon != nowtime.tm_mon)
		{
			return true;
		}
	}
	else//不是同一年
	{
		return true;
	}	

	return false;
}

bool User::IsNewLoginWeek() const
{
	time_t lasttime = GetLastLoginTime();
	struct tm lastlogintime;
	localtime_r(&lasttime,&lastlogintime);
	lastlogintime.tm_hour=0;
	lastlogintime.tm_min=0;
	lastlogintime.tm_sec=0;
	lasttime=mktime(&lastlogintime);

	time_t now = time(NULL);
	struct tm nowtime;
	localtime_r(&now,&nowtime);
	nowtime.tm_hour=0;
	nowtime.tm_min=0;
	nowtime.tm_sec=0;
	now=mktime(&nowtime);

	int lastweek = lastlogintime.tm_wday;
	lasttime -= lastweek*24*3600;

	int nowweek = nowtime.tm_wday;
	now -= nowweek*24*3600;

	if (lasttime == now)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool User::IsNewLoginDay() const
{
    time_t lasttime = GetLastLoginTime();
    struct tm lastlogintime;
	localtime_r(&lasttime,&lastlogintime);

    time_t now = time(NULL);
    struct tm nowtime;
	localtime_r(&now,&nowtime);

    if (lastlogintime.tm_yday != nowtime.tm_yday)
    {
        return true;
    }

    return false;
}

void User::OnLogin()
{

}

int User::Rename( const string& name )
{
	return 0;
}

int User::GetUserRegion(bool bCheckGroup/* =false */)
{
	if(!bCheckGroup)
	{
		return m_dbUser.region();
	}
	else
	{
		return serverConfig.getmapregion(m_dbUser.region());
	}
}

void User::InitRc4Key(int nNum,string szSid)
{
	int64 nKey = /*m_dbUser.player().level() * */ 100 + nNum;

	m_strRc4Send = toString(m_dbUser.id()) + toString(nKey) + szSid;

	m_strRc4Receive = szSid + toString(m_dbUser.id()) + toString(nKey);

	SYS_LOG(GetUid(),LT_PLAYER_RC4_KEY,0,0,m_strRc4Send<<m_strRc4Receive);
}

bool User::CheckPlayerAdult()
{//一刀切防沉迷
	ConstantSetUnit* pUnit = GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_DEF_CHECK_ADULT);
	if (pUnit == NULL || pUnit->m_nArgs[0] == 0)
	{
		return true;
	}

// 	if (m_pPlayer->GetTotalPercharge() >= 5000 || m_pPlayer->GetPrivilegeTimeDay(10201) > 0)
// 	{
// 		return true;
// 	}

	return m_dbUser.badult();
}

/*
void User::Logon(GameDataHandler* dh)
{

	if(m_pPlayer&&m_pPlayer->CanUse())
	{
		m_pPlayer->_TmpCheckLV();

		m_pPlayer->Logon(dh);
	}

	m_UpdateSaveTime = 0;
}*/

inline string User::GetProfileLink(PLAT_TYPE nPlatType) const 
{
	if (nPlatType >= 0 && nPlatType < PLAT_TYPE_MAX)
	{
		return m_dbUser.profile_link(nPlatType);
	}
	else
	{
		return m_dbUser.profile_link(0);
	}
}

inline string User::GetProfileLinkAnyWay(PLAT_TYPE nPlatType) const 
{
	if (nPlatType >= 0 && nPlatType < PLAT_TYPE_MAX && nPlatType < m_dbUser.profile_link_size())
	{
		string _tmpLink = m_dbUser.profile_link(nPlatType);
		if (_tmpLink.size() > 0)
		{
			return _tmpLink;
		}
	}

	for (int i=0;i<PLAT_TYPE_MAX && i < m_dbUser.profile_link_size();i++)
	{
		string _tmpLink = m_dbUser.profile_link(i);
		if (_tmpLink.size() > 0)
		{
			return _tmpLink;
		}
	}
	return m_dbUser.profile_link(0);
}

inline void User::SetProfileLink(const string &profile_link, enum PLAT_TYPE nPlatType) 
{
	if(nPlatType < 0 || nPlatType >= PLAT_TYPE_MAX)
		return;

	if (m_dbUser.profile_link_size() == 0)
	{
		m_dbUser.set_profile_link(0, "");
	}
	else
	{
		m_dbUser.set_profile_link(nPlatType, profile_link);
	}
}