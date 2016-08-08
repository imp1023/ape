#ifndef PlayerMgr_H
#define	PlayerMgr_H

#include <deque>
#include <string>
#include "User.h"
#include <math.h>
#include "../common/distribution.h"
#include "../common/const_def.h"
#include "../logic/InitItemTbl.h"
#include "PlanetManager.h"
#include "BattleManager.h"
#include "SocialItemManager.h"

using namespace std;

#define INVITER_BIND_REWARD 331

/*#define ALL_GLOBALARENA_WORLDID 2*/
static const int FIXED_SEIKO_NUM = 4;
class Player
{
public:
	friend class User;

	void		InitDB(DB_Player* pDBPlayer);
	void		InitNewUserFromCfg();

private:
	Player(User* pParent);
	~Player();

	void		SendBroadCastToSelf(int type, int id, vector<string> param);
public:
	void		SetEventHandler(GameEventHandler* eh_);
	User*		GetParent(void) const;

	inline DB_Player*			GetDBPlayer(){return m_pdbPlayer;}
	inline GameEventHandler*	GetPlayerEh(){return eh_;}
	inline GameDataHandler*		GetPlayerDh(){return eh_->getDataHandler();}

	int GetCurPlanetId();
	void SetCurPlaentId(int nPlanetId);

	bool CanUse(void) const;

	//logic
	int GetRes(UserSource emType);
	bool CostRes(UserSource emType,int nCount,bool bLog = false,UserSource_CostType eCosttype = E_UserSource_CostType_None);
	bool SetDroidInUse(DB_Planet *pPlanet, int cnt);
	bool CheckDroidInUse(DB_Planet *pPlanet, int cnt);
	bool CheckDroids(DB_Planet *pPlanet, int workingCnt);
	bool CostSocialItem(string sku, int cnt);
	bool AddSocialItem(string sku, int cnt);
	bool AddSocialItem(string sku, int cnt, int currentcount, int timeLeft);
	bool AddItemToWishList(string sku);
	bool RemoveItemFromWishList(string sku);
	bool AddDroid(int nPlanetId);
	bool SetLevel(int level);
	bool SetFlag(string key ,int val);


	DB_SocialItem* GetSocialItem(string sku);

	int CreateBuilding(int nPlanetId, CFG_InitItem *pCfgItem);
	DB_Item * CreateBuilding(int nPlanetId, MsgBuildingItem *pItem);
	bool DestroyBuilding(int nPlanetId, int id, int sid);
	bool RotateBuilding(int nPlanetId, int id, int sid, int x, int y, int flip);
	bool MoveBuilding(int nPlanetId, int id, int sid, int x, int y);
	bool cancelBuild(int nPlanetId, int id, int sid);
	bool cancelUpgrade(int nPlanetId, int id, int sid, int ntime);
	bool upgradePremium(int nPlanetId, int id, int sid, int ntime,int incometorestore);
	bool updateNewState(int nPlanetId,int newState, int oldState,int id, int sid, int ntime, int incomeToRestore);
	void RemoveSocialItem(string sku);
	bool updateCoinsLimit(int slot);
	bool updateMineralsLimit(int slot);
	bool repairingStart(int nPlanetId, int sid, int ntime, int id);
	bool repairingCompleted(int nPlanetId, int sid, int id);
	DB_Planet* GetPlanet(int nPlanetId);
	Planet* GetRealPlanet(int nPlanetId);
	bool AddStarBookmark(int nStarId, int nStarType, int nStarName, int x, int y);
	bool DelStarBookmark(int nStarId);

	//npc
	void FillinNpcs(RseObtainNpcList *lst);
	void FillinNpcs(RseObtainUniverse *rse, int skuId);
	void FillInMissoin(RseObtainUniverse *rse);
	void FillinBookmarks(RseQueryStarsBookmarks *rse);
	void FillinUniverse(RseObtainUniverse *rse, int nPlanetId);
	void FillInPlanet(RseObtainUniverse *rse, int nPlanetId);
	void FillinItem(RseObtainUniverse *rse, int nPlanetId);
	void FillinShipyard(RseObtainUniverse *rse, int nPlanetId);
	void FillinHangar(RseObtainUniverse *rse, int nPlanetId);
	void FillinBunker(RseObtainUniverse *rse, int nPlanetId);
	void FillinGameUnit(RseObtainUniverse *rse, int nPlanetId);
	void FillInWishList(RseObtainSocialItems* rse);
	void FillSocialItems(RseObtainSocialItems *rse);
	void FillinFlag(RseObtainUniverse* rse);

	//pve
	void FillinPvELite(RseQueryPvE* rsp);

	//init new default player
	void InitPlayerModel();
	void InitPlayerFlag();
	void InitPlayerState();
	void InitPlayerPlanets(int nID, int nName, int nType, string sku);
	void InitPlayerNPC();

	//battle
	void BeginBattle(int64 nAttacker, int nFromPlanet, int64 nTargetPlayer, int nPlanetId, time_t nTime);
	Battle* GetBattle(){return m_pBattleManager->GetBattle();}
	void GetGWGBattleInfo(GWG_BattleInfo *pBattleInfo);
	void FillBattleLogAttackerInfo(GWG_BattleInfo *pBattleInfo);
	void CopyUniverse();
	void SetBattleType(int nType);
	bool IsAttacked();
	bool IsAttacking();

private:
	DB_Player*           m_pdbPlayer;
	User*                m_pUser;
	GameEventHandler*    eh_;
	//manager
	PlanetManager*		m_pPlanetManager;
	BattleManager*		m_pBattleManager;
	//variable
	int					m_nCurrentPlanetId;
	SocialItemManager*  m_pSocialManager;
};

inline User* Player::GetParent() const
{
	return m_pUser;
}

inline bool Player::CanUse() const
{
	return m_pdbPlayer != NULL && m_pUser != NULL;
}

inline int Player::GetCurPlanetId()
{
	return m_nCurrentPlanetId;
}

inline void Player::SetCurPlaentId(int nPlanetId)
{
	if(nPlanetId == m_nCurrentPlanetId || 0 >= nPlanetId || !GetPlanet(nPlanetId)){
		return;
	}
	m_nCurrentPlanetId = nPlanetId;
}

//向统计服务器发送统计信息
template <class T1,class V1,class V2,class V3,class V4,class V5,class V6,class V7>
void SYS_UserStat(User* pUser,int logType,T1 subTp1,V1 val1,V2 nVal2=0,V3 nVal3=0,V4 nVal4=0,V5 nVal5=0,V6 nVal6=0,V7 nVal7=0)
{
	//Player* pPlayer = pUser->GetPlayer();
	//SYS_STAT(pUser->uid(),logType,pUser->GetUserLevel(),pUser->GetLogOpenID(),pPlayer->GetTotalPercharge(),pUser->GetLogRegDays(),pPlayer->GetCash(),
	//	distribution::getRegion(pUser->uid()), pUser->getChannel(),pPlayer->GetVIPLevel(), subTp1,val1,nVal2,nVal3,nVal4,nVal5,nVal6,nVal7);
	//SYS_STAT(pUser->uid(), logType, pUser->GetUserLevel(), pUser->GetLogOpenID(), 0, "", 0,
	//	distribution::getRegion(pUser->uid()), 0, 0, subTp1,val1,nVal2,nVal3,nVal4,nVal5,nVal6,nVal7);
}

#endif /* PlayerMgr_H */