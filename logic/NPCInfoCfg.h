#pragma once
#include <map>
#include "../common/const_def.h"
#include "../common/json/json.h"
using namespace std;

class NPCPveBattleTbl
{
public:
	NPCPveBattleTbl(){}
	~NPCPveBattleTbl(){}
	int m_nID; //流水ID
	int m_nBattleID; //战役ID
	int m_nType; //关卡类型
	int m_nUnlockID; //解锁关卡ID
	time_t m_ltStart;
	time_t m_ltEnd;
	int m_nCDTime;
	time_t m_ltRefreshTime;
	int m_nRewardID; 
	int m_nSpecialRewardID;
	int m_nSpecialKey;
	int m_nNpcID;
	int m_nPveID;
	int m_nChangjingType;
	int m_nCount;
	int m_nMetal;
	int m_nOil;
	int m_nWaitTime;
	int m_nCredit;
	int m_nAdExp;
	int m_nTime;
	int m_nPopulation;
	vector<int> m_nBattleValue;
	int m_nJunling;
	int m_nMinusCredit;
};

struct  BuildingItemJson
{
	int sid;
	int skuId;
	int id;
	int upgradeId;
	int type;
	int state;
	int x;
	int y;
	int energy;
	int energyPercent;
};

struct BunkerJson 
{
	int sid;
	int skuId;
	int num;
};

struct GameUnitJson
{
	int skuId;
	int upgradeId;
};

struct NpcCfgJson 
{
	vector<BuildingItemJson> items;
	vector<BunkerJson> bunkers;
	vector<GameUnitJson> gameUnits;
};

class NPCInfoCfg
{
	enum E_PVEBATTLE_FILE
	{
		E_PF_ID = 0,
		E_PF_BID,
		E_PF_TYPE,
		E_PF_UNLOCKID,
		E_PF_STARTDAY,
		E_PF_STARTTIME,
		E_PF_ENDDAY,
		E_PF_ENDTIME,
		E_PF_REWARDID,
		E_PF_CDTIME,
		E_PF_REFRESHTIME,
		E_PF_SPECIALREWARDID,
		E_PF_SPECIALKEY,
		E_PF_NPCID,
		E_PF_PVEID,
		E_PF_CHANGJINGTYPE,
		E_PF_COMMON3,
		E_PF_COUNT,
		E_PF_COMMON5,
		E_PF_COMMON6,
		E_PF_METAL,
		E_PF_OIL,
		E_PF_WAITTIME,
		E_PF_CREDIT,
		E_PF_COMMON7,
		E_PF_COMMON8,
		E_PF_ADEXP,
		E_PF_TIME,
		E_PF_POPULATION,
		E_PF_BATTLEVALUE,
		E_PF_JUNLING,
		E_PF_MINUS_CREDIT,
	};
public:
	NPCInfoCfg(void);
	~NPCInfoCfg(void);

	bool			LoadTbl();
	void			ClearTbl();

	static NPCInfoCfg& Instance()
	{
		static NPCInfoCfg inst;
		return inst;
	}

	NpcCfgJson* GetNPCInfo(int nID);

	bool			LoadNPCPveBattleTbl();
	const NPCPveBattleTbl* GetNPCPveBattleTbl(int nID);
	const NPCPveBattleTbl* GetNPCPveBattleTblByNum(int nNum);
	int				GetNPCPveBattleTblNum(){ return m_mapPveBattleTbl.size(); }
	const NPCPveBattleTbl* GetNPCPveBattleTblByBase(int nBaseID);

private:
	void FillJsonNpc(int nID,const Json::Value& jsNPC,NpcCfgJson* pDBNpc);
	int    GetJsonAsInt(const Json::Value& jsNPC,const char* szType);
	int64  GetJsonAsInt64(const Json::Value& jsNPC,const char* szType);
	string GetJsonAsStr(const Json::Value& jsNPC,const char* szType);

	void FillBuildingData(const Json::Value& jsBld, vector<BuildingItemJson>& ItemJson);
	void FillBunkerData(const Json::Value& jsBld, vector<BunkerJson>& bunkerJson);
	void FillGameUnitData(const Json::Value& jsBld, vector<GameUnitJson>& gameUnitJson);
	
protected:
	static const char*		szNPCPveBattleFile;
	map<int,NpcCfgJson*>	m_mapNPC;
	map<int,NPCPveBattleTbl*> m_mapPveBattleTbl;
};

