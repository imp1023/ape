#include "PlatConfigMgr.h"

#include "../logic/GameSrvCfg.h"
#include "../logic/FriendInfoCfg.h"
#include "../logic/FriendGiftCfg.h"
#include "MemCacheServerHandler.h"
#include "../logic/PlatInviteFriendMgr.h"
#include "../logic/InviteFrdGiftCfg.h"
#include "../logic/GameConstantSetCfg.h"
#include "../logic/CostGiftCfg.h"

PlatConfigMgr* PlatConfigMgr::_sMgr = NULL;

PlatConfigMgr::PlatConfigMgr(int nPlatSrvID)
{
	m_nPlatSrvID = nPlatSrvID;
}

PlatConfigMgr::~PlatConfigMgr(void)
{
}

void PlatConfigMgr::LoadPlatConfigInfo(int nPlatSrvID,bool bRootLoad/* =true */)
{
	bool bSuc = true;
	if(!GameSrvCfg::Instance().LoadTbl()
		||!FriendInfoCfg::Instance().LoadTbl()
		/*||!FriendGiftCfg::Instance().LoadTbl()
		||!PlatInviteFriendMgr::Instance().LoadCfgFile()
		||!InviteFrdGiftCfg::Instance().LoadTbl()
		||!GameConstantSetCfg::Instance().LoadTbl()
		||!GameConstantSetCfg::Instance().LoadTbl()
		||!CostGiftCfg::Instance().LoadTbl()*/)
	{
		printf("Read Config File ERROR!!!!!\r\n");
		exit(0);
	}

	MemCacheServerHandler::GetInst()->LoadConfig();
	MemCacheServerHandler::GetInst()->SetEnbale(GameSrvCfg::Instance().PlatCacheEnable() != 0);
	
	printf("Plat Read Config File Success!!!!!!!!!!!\r\n");
	
}
