#include "CountryConfigMgr.h"
#include "../logic/GameSrvCfg.h"
#include "../logic/CountryNameTbl.h"
#include "MemCacheServerHandler.h"
#include "../logic/GameConstantSetCfg.h"

CountryConfigMgr* CountryConfigMgr::_sMgr = NULL;

CountryConfigMgr::CountryConfigMgr(int nCountrySrvID)
{

}

CountryConfigMgr::~CountryConfigMgr(void)
{
}

void CountryConfigMgr::LoadCountryConfigInfo(int nCountrySrvID,bool bRootLoad/* =true */)
{
	bool bSuc = true;
	if(!GameSrvCfg::Instance().LoadTbl()
		||!GameConstantSetCfg::Instance().LoadTbl()
		||!CountryNameTblInst::instance().LoadInfo())
	{
		printf("Read Config File ERROR!!!!!\r\n");
		exit(0);
	}

	MemCacheServerHandler::GetInst()->LoadConfig();
	MemCacheServerHandler::GetInst()->SetEnbale(GameSrvCfg::Instance().CountryCacheEnable() != 0);
	
	printf("Country Read Config File Success!!!!!!!!!!!\r\n");
	
}
