#ifndef _CFG_WONDERDEFINITIONS_H_
#define _CFG_WONDERDEFINITIONS_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

class WonderDefinitionsTbl
{
public:
	WonderDefinitionsTbl();
	~WonderDefinitionsTbl();

	void			LoadInfo();
	void			Clear();
	int				GetHQEnergy(int lv);

protected:
	static const char*  szConfigFile;
	map<int, int>	m_mWonderDef;
};

typedef Singleton<WonderDefinitionsTbl> WonderDefinitionsTblInst;

#endif

