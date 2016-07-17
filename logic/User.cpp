#include "Player.h"
#include "../common/Clock.h"
#include "../common/SysLog.h"
//#include "../event/EventDefine.h"

extern ServerConfig serverConfig;

User::User()
{
	Init();
}

User::User(int64 id, const string& openid)
{
	Init();
	m_dbUser.set_id(id);
	m_dbUser.set_openid(openid);
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

    InitDBUser();
}

void User::InitDBUser()
{
    m_dbUser.Clear();
    m_dbUser.set_id(0);

    m_dbUser.set_last_login_time(0);
    m_dbUser.set_openid("");
    m_dbUser.set_regist_time(0);

    OnSetDbPlayer();
}

void User::OnSetDbPlayer()
{
    if (m_pPlayer != NULL)
    {
        delete m_pPlayer;
        m_pPlayer = NULL;
    }
    if (m_pPlayer == NULL)
    {
        m_pPlayer = new Player(this, m_dbUser.mutable_player());
    }
}

void User::InitNewUser()
{
	//m_dbUser.set_name("TestPlayer");
	//m_dbUser.set_head(0);
	setRegistTime(time(NULL));
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
    OnSetDbPlayer();
}

bool User::IsNewLoginMonth() const
{
	time_t lasttime = lastLoginTime();
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
	time_t lasttime = lastLoginTime();
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
    time_t lasttime = lastLoginTime();
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

bool User::IsContinueLoginDay() const
{
    int lasttime = (int)lastLoginTime();
	return Clock::getLocalDay(lasttime) + 1 == Clock::getLocalDay();
/*
    struct tm lastlogintime = *localtime(&lasttime);

    time_t now = time(NULL);
    struct tm nowtime = *localtime(&now);

    if (lastlogintime.tm_year == nowtime.tm_year
            && lastlogintime.tm_yday + 1 == nowtime.tm_yday)
    {
        return true;
    }
    else if (lastlogintime.tm_year + 1 == nowtime.tm_year)
    {
        // 闰年
        if ((lastlogintime.tm_year+1900) % 400 == 0
                || (lastlogintime.tm_year % 4 == 0 && (lastlogintime.tm_year+1900) % 100 != 0 ))
        {
            if (lastlogintime.tm_yday == 365 && nowtime.tm_yday == 0)
            {
                return true;
            }
        }
        else
        {
            if (lastlogintime.tm_yday == 364 && nowtime.tm_yday == 0)
            {
                return true;
            }
        }
    }

    return false;
	*/
}

void User::OnLogin()
{

}

int User::GetRegDays()
{
	return Clock::getLocalDay() - Clock::getLocalDay((int)registTime());
}

void User::OnAuth()
{
	m_pPlayer->SetEventHandler(Daemon::Instance().GetGameEventHandler());
}

int User::setRole( const string& name, int head, int camp )
{
	if (m_dbUser.has_name() || m_dbUser.has_head())
		return 1;

	if (camp < 1 || camp > 3)
		return 3;

	return 0;
}

 string User::GetLogOpenID()
{
	string openid = m_dbUser.openid();
	int len = openid.length();
	int count = 0;
	int i = 0;
	while (i < len && count < 3)
	{
		if (openid[i] == '_')
		{
			++count;
		}
		++i;
	}
	return openid.substr(i, openid.find('_', i) - i);
}

string User::GetLogRegDays()
{
	string str = toString(GetRegDays()) + "_" + "here";//getFrom();
	return str;
}

int User::Rename( const string& name )
{
	return 0;
}

