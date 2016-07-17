#include "StarLogic.h"
#include "CountryNameTbl.h"
#include "../common/rand_util.h"

StarLoigc::StarLoigc()
{
	m_mGlobalStarInfo.clear();
	m_FindStarKeyVal.clear();
	m_FindStarKeyVal.insert(make_pair(1,30));
	m_FindStarKeyVal.insert(make_pair(2,30));
	m_FindStarKeyVal.insert(make_pair(3,40));
}

int StarLoigc::GetStarName()
{
	int num = CountryNameTblInst::instance().GetTotalSize();
	return rand() % num + 1;
}

int StarLoigc::GetNewPlayerStar(int64 uid)
{
	Star star;
	int nNum = m_mGlobalStarInfo.size();
	if(0 == nNum){
		star.id = 1;
		star.x = 0;
		star.y = 0;
		star.type = rand() % STAR_TYPE_MAX;
		PlanetLiteInfo lite;
		lite.uid = uid;
		lite.planetId = 1;
		star.pli.push_back(lite);
		star.name = GetStarName();
		m_mGlobalStarInfo.insert(make_pair(star.id, star));
		return star.id;
	}else{
		star = m_mGlobalStarInfo[nNum];
		if(star.pli.size() < STAR_ALLOCATE_OCCUPIED){
			PlanetLiteInfo lite;
			lite.uid = uid;
			lite.planetId = 1;
			star.pli.push_back(lite);
			return star.id;
		}else{
			int ran = rand() % 100;
			int totalProb = 0, number = 0;
			for(map<int,int>::iterator iter = m_FindStarKeyVal.begin(); iter != m_FindStarKeyVal.end(); iter++){
				number++;
				totalProb += iter->second;
				if(ran <= totalProb){
					break;
				}
			}
			
			int x = star.x;
			int y = star.y;
			Star newStar;
			for(int i = 0; i < number; i++){
				newStar = GetNearbyStar(x, y);
				x = newStar.x;
				y = newStar.y;
			}
			
			star.id = nNum + 1;
			star.x = newStar.x;
			star.y = newStar.y;
			star.type = rand() % STAR_TYPE_MAX;
			PlanetLiteInfo lite;
			lite.uid = uid;
			lite.planetId = 1;
			star.pli.push_back(lite);
			star.name = GetStarName();
			m_mGlobalStarInfo.insert(make_pair(star.id, star));
			return star.id;
		}
	}
	return 0;
}

Star StarLoigc::GetNearbyStar(int x, int y)
{
	Star newStar;
	Area area = GetNewPoint(x, y, GetArea(x, y));
	newStar.x = area.newX;
	newStar.y = area.newY;
	return newStar;
}

Area StarLoigc::GetArea(int x, int y) {
	int hx = 0, hy = 0, wx = 0, wy = 0;
	bool left = false;

	int offsetX = x / STAR_SEED;
	int offsetY = y / STAR_SEED;

	hx = offsetX * STAR_SEED;
	wx = (offsetX + 1) * STAR_SEED;

	hy = (offsetY + 1) * STAR_SEED;
	wy = offsetY * STAR_SEED;

	if ((x + y) <= (wx + wy)) {// 判断是左半边还是右半边
		left = true;
	}

	Area hwxy;
	hwxy.hx = hx;
	hwxy.hy = hy;
	hwxy.wx = wx;
	hwxy.wy = wy;
	hwxy.left = left;
	return hwxy;
}

Area StarLoigc::GetNewPoint(int x, int y, Area hwxy)
{
	int hx = hwxy.hx;
	int hy = hwxy.hy;
	int wx = hwxy.wx;
	int wy = hwxy.wy;
	bool left = hwxy.left;
	int newX = 0, newY = 0;

	if (x == hx && y == hy) {// 如果当前坐标为(hx,hy)点
		newX = wx;
		newY = wy + 1;
	} else {
		if (left) {// 左半边
			if (x == hx) {// 左半边的竖列(0,0),(0,1),(0,2)
				if (y == wy) {// 则为本方格的原点(0,0)
					newX = x + 1;
					newY = y;
				} else {
					newX = y + STAR_SEED - hy + hx + 1;
					newY = wy;
				}
			} else {
				newX = x - 1;
				newY = y + 1;
			}
		} else {// 右半边
			if (y == hy) {// 右边的横列(0,0),(0,1),(0,2)
				if (x == wx) {// 右边的最终点(10,10)
					if (x == STAR_MAX_WEIGHT) {// 已经到此行的终点
						newX = 0;
						newY = y + 1;
					} else {
						newX = wx + 1;
						newY = wy;
					}
				} else {
					newX = wx;
					newY = x - hx + wy + 1;
				}
			} else {
				newX = x - 1;
				newY = y + 1;
			}
		}
	}
	hwxy.newX = newX;
	hwxy.newY = newY;

	while (newX == wx || newY == hy) {
		if (newX == wx && newY == hy) {// 右边的最后一个点
			if (newX == STAR_MAX_WEIGHT) {// 已生成到最右边,另起一行从头开始
				newX = 0;
			} else {
				newY = wy;
			}
			hwxy = GetArea(newX, newY);
			hwxy.newX = newX;
			hwxy.newY = newY;
		} else {
			if ((newX + newY) <= (wx + wy)) {// 判断是左半边还是右半边
				hwxy.left = true;
			} else {
				hwxy.left = false;
			}
			hwxy = GetNewPoint(newX, newY, hwxy);
		}
		newX = hwxy.newX;
		newY = hwxy.newY;
		hx = hwxy.hx;
		hy = hwxy.hy;
		wx = hwxy.wx;
		wy = hwxy.wy;
	}
	return hwxy;
}

void StarLoigc::serizlize(DB_GlobalStarInfo &db)
{
	db.Clear();
	for(map<int, Star>::iterator iter = m_mGlobalStarInfo.begin(); iter != m_mGlobalStarInfo.end(); iter++){
		DB_Star *star = db.add_stars();
		if(star){
			star->set_id(iter->first);
			star->set_type(iter->second.type);
			star->set_name(iter->second.name);
			star->set_x(iter->second.x);
			star->set_y(iter->second.y);
			for(int i = 0; i < iter->second.pli.size(); i++){
				DB_PlanetLiteInfo* pLite = star->add_lite();
				if(pLite){
					pLite->set_uid(iter->second.pli[i].uid);
					pLite->set_planetid(iter->second.pli[i].planetId);
				}
			}
		}
	}
}

bool StarLoigc::unserialize(DB_GlobalStarInfo db)
{
	for (int i = 0; i < db.stars_size(); i++){
		const DB_Star dbStar = db.stars(i);
		Star star;
		star.id = dbStar.id();
		star.type = dbStar.type();
		star.name = dbStar.name();
		star.x = dbStar.x();
		star.y = dbStar.y();
		for(int k = 0; k < dbStar.lite_size(); k++){
			PlanetLiteInfo lite;
			lite.uid = dbStar.lite(k).uid();
			lite.planetId = dbStar.lite(k).planetid();
			star.pli.push_back(lite);
		}
		map<int, Star>::iterator iter = m_mGlobalStarInfo.find(star.id);
		if(iter != m_mGlobalStarInfo.end()){
			return false;
		}
		m_mGlobalStarInfo.insert(make_pair(star.id, star));
	}
	return true;
}