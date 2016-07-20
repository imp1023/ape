#ifndef PlayerMgr_H
#define	PlayerMgr_H

#include <deque>
#include <string>
#include "User.h"
#include <math.h>
#include "../common/distribution.h"
#include "../common/const_def.h"

using namespace std;

#define INVITER_BIND_REWARD 331

/*#define ALL_GLOBALARENA_WORLDID 2*/
static const int FIXED_SEIKO_NUM = 4;
class Player
{
public:
    friend class User;

	void		InitDB(DB_Player* pDBPlaper);
	void		InitNewUserFromCfg();

private:
    Player(User* pParent, DB_Player* pdbPlayer);
	Player();
	Player(Player&);
    ~Player();

	void		SendBroadCastToSelf(int type, int id, vector<string> param);
public:
	void		SetEventHandler(GameEventHandler* eh_);
    User*		GetParent(void) const;

	inline DB_Player*			GetDBPlayer(){return m_pdbPlayer;}
	inline GameEventHandler*	GetPlayerEh(){return eh_;}
	inline GameDataHandler*		GetPlayerDh(){return eh_->getDataHandler();}

	void InitPlayerData();
	void InitPlayerModel();
	void InitPlayerState();
	void InitPlayerPlanets();
	void InitPlayerNPC();

private:
    DB_Player*           m_pdbPlayer;
    User*                m_pUser;
    GameEventHandler*    eh_;
	
};

inline User* Player::GetParent() const
{
    return m_pUser;
}

//向统计服务器发送统计信息
template <class T1,class V1,class V2,class V3,class V4,class V5,class V6,class V7>
void SYS_UserStat(User* pUser,int logType,T1 subTp1,V1 val1,V2 nVal2=0,V3 nVal3=0,V4 nVal4=0,V5 nVal5=0,V6 nVal6=0,V7 nVal7=0)
{
	Player* pPlayer = pUser->GetPlayer();
	//SYS_STAT(pUser->uid(),logType,pUser->GetUserLevel(),pUser->GetLogOpenID(),pPlayer->GetTotalPercharge(),pUser->GetLogRegDays(),pPlayer->GetCash(),
	//	distribution::getRegion(pUser->uid()), pUser->getChannel(),pPlayer->GetVIPLevel(), subTp1,val1,nVal2,nVal3,nVal4,nVal5,nVal6,nVal7);
	//SYS_STAT(pUser->uid(), logType, pUser->GetUserLevel(), pUser->GetLogOpenID(), 0, "", 0,
	//	distribution::getRegion(pUser->uid()), 0, 0, subTp1,val1,nVal2,nVal3,nVal4,nVal5,nVal6,nVal7);
}

#endif /* PlayerMgr_H */
