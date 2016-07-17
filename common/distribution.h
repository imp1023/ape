#pragma once
#include "const_def.h"
#include <string>
using namespace std;

namespace distribution
{
    int getRegion(const string& openId);
    int getRegion(int64 uid);
    int64 allocUid(int locale, int channel, int region, int region_index);
	int64 allocGuildId(int region, int region_index);
    int64 allocUnionId(int locale, int channel, int region, int region_index);
}
