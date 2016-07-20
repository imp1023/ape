#pragma once

#include <string>
#include <vector>
#include <queue>

#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/SysLog.h"

#undef signbit

#include "dbinterface.pb.h"
#include "../event/event.pb.h"

#include "../gamed/GameEventHandler.h"
#include "../gamed/GameNetHandler.h"
#include "../gamed/Daemon.h"

using namespace std;

//show tips stat

#define MAX_USER_ID_LEN         (32)
#define MAX_USER_NICKNAME_LEN   (200)


class Player;
class GameDataHandler;

class User
{
public:
    static const int    nMaxUserIdLen       = MAX_USER_ID_LEN;
    static const int    nMaxUserNicknameLen = MAX_USER_NICKNAME_LEN;
    
public:
    User();
	User(int64 id, const string &pid, const string &name,
		const string &profile_link, int gender, PLAT_TYPE plat_type,
		bool bIsYellowDmd, bool bIsYellowDmdYear, int i4YellowDmdLv,
		const vector<string> &friends_platid,int nRegion,int nCity,bool bIsHighYellowDmd,bool bIsHighDmdYear,int,int,int,int nHighBlueYearTime);
    ~User(void);

    void Init();
    void InitDBUser();

    bool checkSecret(int64 secret, time_t now)
    {
		int64 defaultSecret = 8613910246800ll;
        return (secret == defaultSecret) ||
                (now < secret_gentime_ + 3600 * 1000 && secret == secret_);
    }

    string secret(time_t now)
    {
        //if (now >= secret_gentime_ + 3600 * 1000) // 1 hour
        {
            secret_ = now;
            secret_gentime_ = now;
        }
        return toString(secret_);
    }

    int fd(void) const
    {
        return fd_;
    }

    void setFd(int fd)
    {
        fd_ = fd;
    }

	time_t	GetLastLoginTime() const
	{
		if (m_dbUser.has_last_login_time())
			return m_dbUser.last_login_time();
		return 0;
	}

    void setLastLoginTime(time_t login_time)
    {
        m_dbUser.set_last_login_time(login_time);
    }

	time_t lastLogoutTime(void) const
	{
		if (m_dbUser.has_last_logout_time())
			return m_dbUser.last_logout_time();
		return 0;
	}

	void setLastLogoutTime(time_t logout_time)
	{
		m_dbUser.set_last_logout_time(logout_time);
	}

    time_t GetRegisterTime()
    {
        if (m_dbUser.has_regist_time())
            return m_dbUser.regist_time();
        return 0;
    }

    void SetRegisterTime(time_t regist_time)
    {
        m_dbUser.set_regist_time(regist_time);
    }

	DB_BanLogin* GetDBBanLogin()
	{
		return m_dbUser.mutable_banlogin();
	}

    int64 revision()
    {
        return revision_;
    }

    void setRevision(int64 revision)
    {
        revision_ = revision;
    }

	int64 getMemRevision()
	{
		return mem_revision_;
	}

	void setMemRevision(int64 revision)
	{
		mem_revision_ = revision;
	}

	inline int64 RMRevision()
	{
		return rmrevision_;
	}
	inline void SetRmRevision(int64 revision)
	{
		rmrevision_ = revision;
	}
	
	inline string GetPlatformId() const 
	{ 
		return m_dbUser.platform_id(); 
	}

	inline void SetPlatformId(string val) 
	{
		m_dbUser.set_platform_id(val);
	}

    void setOnline(bool bOnline)
    {
        bOnline_ = bOnline;
    }

    bool Online(void) const
    {
        return bOnline_;
    }

	int					Rename(const string& name);
    bool				IsNewLoginDay(void) const;
	bool				IsNewLoginMonth(void) const;
	bool				IsNewLoginWeek(void) const;
	bool				CheckPlayerAdult();

	int					GetUserLevel(void) const;
	int					GetUserExp(void) const;
	int					GetUserTotalExp(void) const;
    void                InitNewUser();
    DB_User&            GetDbUser(void);
    const DB_User&      GetDbUser(void) const;
	DB_Player*			GetDBPlayer();
    void                SetDbUser(const DB_User& dbuser);

	void				SetFd(int fd);
	Player*				GetPlayer(void);
	const Player*		GetPlayer(void) const;
	void				SetName(const string &name, enum PLAT_TYPE nPlatType);

	int64				GetUid() const;
	void				SetUid(int64 val);

	void				OnLogin();

    int                 GetUserRegion(bool bCheckGroup=true); //bCheckGroup：true合区后的分区 false:合区前的分区
	
	//Rc4加密
	void					InitRc4Key(int nNum,string szSid);		//获得Key
	string&					GetRc4Send();
	string&					GetRc4Receive();

private:
    void                OnSetDbUser();
	
protected:
    int					fd_;
    int64				secret_;
    time_t				secret_gentime_;
    time_t				timeLogin_;
    int64				revision_;
	int64				mem_revision_;
    bool				bOnline_;
	int64				rmrevision_;

	PLAT_TYPE			plat_type_;
	int					m_nRegDays;
	vector<int64>		friends_id_;
	vector<string>		friends_platid_;

    DB_User        		m_dbUser;
    bool           		m_bDirty;

private:
    Player*        		m_pPlayer;
    int            		m_nFd;

	string				m_strRc4Send;
	string				m_strRc4Receive;
} ;

inline void User::SetFd(int fd)
{
    m_nFd = fd;
}

inline Player* User::GetPlayer()
{
    return m_pPlayer;
}

inline void User::SetName(const string &name, enum PLAT_TYPE nPlatType)
{
	if (m_dbUser.name_size() == 0  && nPlatType >= 1)
	{
		m_dbUser.set_name(0, "");
	}
	if (nPlatType >= 0 && nPlatType < PLAT_TYPE_MAX)
	{
		m_dbUser.set_name(nPlatType, name);
	}
}

inline string&	User::GetRc4Send()
{
	if(m_strRc4Send.length() <= 0)
	{
		m_strRc4Send = toString(m_dbUser.id()) /*+ toString(m_dbUser.player().level() * 100)*/;
	}
	return m_strRc4Send;
}

inline string&	User::GetRc4Receive()
{
	if(m_strRc4Receive.length() <= 0)
	{
		m_strRc4Receive = /*toString(m_dbUser.player().level() * 100) +*/ toString(m_dbUser.id());
	}
	return m_strRc4Receive;
}

inline int64 User::GetUid() const 
{ 
	return m_dbUser.id(); 
}

inline void User::SetUid(int64 val) 
{ 
	m_dbUser.set_id(val);
}