#pragma once
#include "stdio.h"

class PlatConfigMgr
{
public:

	static PlatConfigMgr* CreateInst(int nPlatSrvID)
	{
		if ( !_sMgr )
		{
			_sMgr = new PlatConfigMgr(nPlatSrvID);
		}
		return _sMgr;
	}

	static void ReleaseInst()
	{
		delete _sMgr;
		_sMgr = NULL;
	}

	static PlatConfigMgr* GetInst()
	{
		return _sMgr;
	}

	void LoadPlatConfigInfo(int nPlatSrvID,bool bRootLoad=true);

private:
	PlatConfigMgr(int nPlatSrvID);
	~PlatConfigMgr(void);

private:
	static PlatConfigMgr*   _sMgr;
	int m_nPlatSrvID;
} ;
