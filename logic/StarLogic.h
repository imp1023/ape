#ifndef _STAR_LOGIC_H
#define _STAR_LOGIC_H

#include "../common/Singleton.h"
#include "../logic/dbinterface.pb.h"
#include "../common/const_def.h"

using namespace std;
typedef struct{
	int x;
	int y;
	int newX;
	int newY;
	int hx;
	int hy;
	int wx;
	int wy;
	bool left;
}Area;

class Star
{
public:
	Star():id(0),x(0),y(0),type(0),name(0){}

	int id;
	int x;
	int y;
	int type;
	int name;
};

class StarLoigc
{
public:
	StarLoigc();
	enum{
		STAR_ALLOCATE_OCCUPIED = 5,
		STAR_TYPE_MAX = 6,
		STAR_SEED = 50,	// 一块星云生成的范围大小
		STAR_MAX_WEIGHT = 1000,	// 星云生成的最大宽度
		STAR_MAX_COUNTRY = 12,
	};
	
	int GetStarName();
	int GetStarType();
	void GetNewStar(int x, int y, int &newx, int &newy);
	Star GetNearbyStar(int x, int y);
	Area GetArea(int x, int y);
	Area GetNewPoint(int x, int y, Area hwxy);

private:
	map<int, int> m_FindStarKeyVal;
};

typedef Singleton<StarLoigc> StarLogicInst;

#endif