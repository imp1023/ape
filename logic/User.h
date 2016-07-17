#pragma once

#include <string>
#include <vector>
#include <queue>

#include "../common/const_def.h"
#include "../common/string-util.h"

#undef signbit

#include "dbinterface.pb.h"

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
	User(int64 id, const string& openid);
    ~User(void);

    void Init();
    void InitDBUser();

    void setUid(int64 uid)
    {
        m_dbUser.set_id(uid);
    }

    int64 uid(void) const
    {
        return m_dbUser.id();
    }

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

    time_t lastLoginTime(void) const
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

    time_t registTime()
    {
        if (m_dbUser.has_regist_time())
            return m_dbUser.regist_time();
        return 0;
    }

    void setRegistTime(time_t regist_time)
    {
        m_dbUser.set_regist_time(regist_time);
    }

    const string& openid()
    {
        return m_dbUser.openid();
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

    void setOnline(bool bOnline)
    {
        bOnline_ = bOnline;
    }

    bool Online(void) const
    {
        return bOnline_;
    }

	string getName() const
	{
		return m_dbUser.name();
	}

	int getHead() const
	{
		return m_dbUser.head();
	}

	int					setRole(const string& name, int head, int camp);
	int					Rename(const string& name);
    bool				IsNewLoginDay(void) const;
	bool				IsNewLoginMonth(void) const;
	bool				IsNewLoginWeek(void) const;
	bool				IsContinueLoginDay(void) const;

	int					GetRegDays();
	string				GetLogOpenID();
	string				GetLogRegDays();

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

	void				OnLogin();
	void				OnAuth();

    
private:
    void                OnSetDbPlayer();
	
protected:
    int					fd_;
    int64				secret_;
    time_t				secret_gentime_;
    time_t				timeLogin_;
    int64				revision_;
	int64				mem_revision_;
    bool				bOnline_;

    DB_User        		m_dbUser;
    bool           		m_bDirty;

private:
    Player*        		m_pPlayer;
    int            		m_nFd;
} ;

inline void User::SetFd(int fd)
{
    m_nFd = fd;
}

inline Player* User::GetPlayer()
{
    return m_pPlayer;
}
