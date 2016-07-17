#include "distribution.h"
#include "string-util.h"

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning(disable: 4996)   // disable warning about security.
#endif

namespace distribution
{
    int getRegion(const string& openId)
    {
        int region = 0;

        // locate last not number
        size_t index = openId.size()-1;
        while (openId[index] >= '0' && openId[index] <= '9' && index > 0)
            index--;
        index++;
        for (; index < openId.size(); index++)
            region = region*10 + openId[index] - '0';

        return region;
    }

    int getRegion(int64 uid)
    {
        return (int)(uid >> 32) & 0xffff;
    }

    int64 allocUid(int locale, int channel, int region, int region_index)
    {
        return ((int64) (locale  & 0xff)   << 56) |
               ((int64) (channel & 0xff)   << 48) |
               ((int64) (region  & 0xffff) << 32) |
               (int64)  region_index;
    }

	int64 allocGuildId(int region, int region_index)
	{
		return (int64)(region & 0xffff) << 32 | (int64) region_index;
	}

    int64 allocUnionId(int locale, int channel, int region, int region_index)
    {
        return allocUid(locale, channel, region, region_index);
    }
}

