#ifndef _WORLDCONFIGMGR_H_
#define _WORLDCONFIGMGR_H_
#include <stdio.h>

class WorldConfigMgr
{
public:

	static WorldConfigMgr* CreateInst()
	{
		if ( !_sMgr )
		{
			_sMgr = new WorldConfigMgr();
		}
		return _sMgr;
	}

	static void ReleaseInst()
	{
		delete _sMgr;
		_sMgr = NULL;
	}

	static WorldConfigMgr* GetInst()
	{
		return _sMgr;
	}

	void LoadWorldConfigInfo();
	void ClearWorldConfigInfo();

private:
	WorldConfigMgr(void);
	~WorldConfigMgr(void);

private:
	static WorldConfigMgr*   _sMgr;
} ;

#endif
