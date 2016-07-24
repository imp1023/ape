#pragma once
#include "stdio.h"

class CountryConfigMgr
{
public:

	static CountryConfigMgr* CreateInst(int nCountrySrvID)
	{
		if ( !_sMgr )
		{
			_sMgr = new CountryConfigMgr(nCountrySrvID);
		}
		return _sMgr;
	}

	static void ReleaseInst()
	{
		delete _sMgr;
		_sMgr = NULL;
	}

	static CountryConfigMgr* GetInst()
	{
		return _sMgr;
	}

	void LoadCountryConfigInfo(int nCountrySrvID,bool bRootLoad=true);

private:
	CountryConfigMgr(int nCountrySrvID);
	~CountryConfigMgr(void);

private:
	static CountryConfigMgr*   _sMgr;
} ;
