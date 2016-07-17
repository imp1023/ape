#ifndef _CFG_COUNTRYNAMETBL_H_
#define _CFG_COUNTRYNAMETBL_H_
#include <map>
#include <string>
#include "../common/const_def.h"
#include "../common/Singleton.h"

using namespace std;

class CountryNameTbl
{
public:
	CountryNameTbl();
	~CountryNameTbl();

	void			LoadInfo();
	void			Clear();
	string			GetInfo(int nIndex);
	inline int		GetTotalSize(){return m_mapCountryName.size();}

protected:
	static const char*  szConfigFile;
	map<int,string>	m_mapCountryName;
};

typedef Singleton<CountryNameTbl> CountryNameTblInst;

#endif

