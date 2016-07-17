#ifndef _CFG_SKUID_H_
#define _CFG_SKUID_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

class SkuIDTbl
{
public:
	SkuIDTbl();
	~SkuIDTbl();

	void			LoadInfo();
	void			Clear();
	string			GetName(int sku);
	int				GetSku(string name);

protected:
	static const char*  szConfigFile;
	map<int, string>	m_mSkuID;
	map<string, int>	m_mIDSku;
};

typedef Singleton<SkuIDTbl> SkuIDTblInst;

#endif

