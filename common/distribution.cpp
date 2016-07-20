#include "distribution.h"
#include "string-util.h"
#include <time.h>
#include <ctime>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning(disable: 4996)   // disable warning about security.
#endif

int getPlatidHash(const string& platid)
{
	int hash = 0, mid = 0;
	for(size_t i = 0; i < platid.length(); i += 4)
	{
		sscanf(platid.substr(i, 4).c_str(), "%x", &mid);
		hash += mid;
	}
	hash &= 0xffff;
	return hash;
}

int getUidHash(int64 uid)
{
	return (int)(uid >> 32) & 0xffff;
}


int getPhysicsRegion(int64 uid)
{
	return (int)(uid >> 58) & 0xffff;
}


time_t	GetDayTimeByStr(const char* pDay, const char* pTime)
{
	tm tmp;
	tmp.tm_yday = -1;
	if(pDay == NULL || pTime == NULL || strlen(pDay) <= 0 || strlen(pTime) <= 0)
	{
		return 0;
	}

	string req = pDay;
	vector<string> tokens;
	string delims = "/";
	tokenize(req, tokens, delims);
	if(tokens.size() < 3)
	{
		return 0;
	}
	int nYear = 0;
	safe_atoi(tokens[0],nYear);
	int nMonth = 0;
	safe_atoi(tokens[1],nMonth);
	int nDay = 0;
	safe_atoi(tokens[2],nDay);

	req = pTime;
	tokens.clear();
	delims = ":";
	tokenize(req, tokens, delims);
	if(tokens.size() < 2)
	{
		return 0;
	}
	int nHour = 0;
	safe_atoi(tokens[0],nHour);
	int nMinut = 0;
	safe_atoi(tokens[1],nMinut);
	int nSec = 0;
	if(tokens.size() >= 3)
	{
		safe_atoi(tokens[2],nSec);
	}
	return GetDayTimeByNum(nYear,nMonth,nDay,nHour,nMinut,nSec);
}

time_t	GetDayTimeByNum(int nYear,int nMonth,int nDay, int nHour, int nMinut, int nSec)
{
	struct tm tm1;

	tm1.tm_year = nYear;
	tm1.tm_mon = nMonth;
	tm1.tm_mday = nDay;
	tm1.tm_hour = nHour;
	tm1.tm_min = nMinut;
	tm1.tm_sec = nSec;

	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;

	return  mktime(&tm1);
}

tm	 GetDayTimeByStr_tm(const char* pDay, const char* pTime)
{
	tm tmp;
	tmp.tm_yday = -1;
	if(pDay == NULL || pTime == NULL)
	{
		return tmp;
	}

	string req = pDay;
	vector<string> tokens;
	string delims = "/";
	tokenize(req, tokens, delims);
	if(tokens.size() < 3)
	{
		return tmp;
	}
	int nYear = 0;
	safe_atoi(tokens[0],nYear);
	int nMonth = 0;
	safe_atoi(tokens[1],nMonth);
	int nDay = 0;
	safe_atoi(tokens[2],nDay);

	req = pTime;
	tokens.clear();
	delims = ":";
	tokenize(req, tokens, delims);
	if(tokens.size() < 2)
	{
		return tmp;
	}
	int nHour = 0;
	safe_atoi(tokens[0],nHour);
	int nMinut = 0;
	safe_atoi(tokens[1],nMinut);
	int nSec = 0;
	if(tokens.size() >= 3)
	{
		safe_atoi(tokens[2],nSec);
	}
	return GetDayTimeByNum_tm(nYear,nMonth,nDay,nHour,nMinut,nSec);
}

tm GetDayTimeByNum_tm(int nYear,int nMonth,int nDay, int nHour, int nMinut, int nSec)
{
	struct tm tm1;

	tm1.tm_year = nYear;
	tm1.tm_mon = nMonth;
	tm1.tm_mday = nDay;
	tm1.tm_hour = nHour;
	tm1.tm_min = nMinut;
	tm1.tm_sec = nSec;

	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;

	return tm1;
}

bool	Comparetm(tm& first, tm& last)
{
	time_t tfirst = mktime(&first);
	time_t tlast = mktime(&last);

	return tfirst >= tlast;
}

bool IsSameDay(time_t ltTime1,time_t ltTime2)
{
	struct tm day1 = *localtime(&ltTime1);

	struct tm day2 = *localtime(&ltTime2);

	if (day1.tm_year==day2.tm_year&&
		day1.tm_yday == day2.tm_yday)
	{
		return true;
	}

	return false;
}

int julianDate(time_t time)
{
	if((int)time<=0)
	{
		return 0;
	}
	int year_ = 0;
	int month_ = 0;
	int day_ = 0;

	tm m = *localtime(&time);
	year_ = m.tm_year + 1900;
	month_ = m.tm_mon + 1;
	day_ = m.tm_mday;

	int jd = day_ - 32075L +
		1461L * ( year_ + 4800L + ( month_ - 14L ) / 12L ) / 4L +
		367L * ( month_ - 2L - ( month_ - 14L ) / 12L * 12L ) / 12L -
		3L * ( ( year_ + 4900L + ( month_ - 14L ) / 12L ) / 100L ) / 4L;
	return jd;
}

bool	Token2Data(string& strInput,vector<int> & intvec)
{
	intvec.clear();
	if(strInput.size()<1)
	{
		return false;
	}
	if(strInput.substr(0,1)=="\"")
	{
		strInput = strInput.substr(1);
	}
	if(strInput.substr(strInput.size()-1,1)=="\"")
	{
		strInput = strInput.substr(0,strInput.size()-1);
	}
	if(strInput.substr(0,1)==",")
	{
		strInput = strInput.substr(1);
	}

	static string delims = ",";
	tokenize_as_int(strInput, intvec, delims);
	return true;
}

bool	Token2Data(string& strInput,vector<string> & strvec)
{
	strvec.clear();
	if(strInput.size()<1)
	{
		return false;
	}
	if(strInput.substr(0,1)=="\"")
	{
		strInput = strInput.substr(1);
	}
	if(strInput.substr(strInput.size()-1,1)=="\"")
	{
		strInput = strInput.substr(0,strInput.size()-1);
	}
	if(strInput.substr(0,1)==",")
	{
		strInput = strInput.substr(1);
	}

	static string delims = ",";
	tokenize(strInput, strvec, delims);
	return true;
}

bool	FillData(int* pArr,int nMaxLen,vector<int> & intvec)
{
	if(pArr == NULL || nMaxLen <= 0)
	{
		return false;
	}

	for(int i=0;i<nMaxLen;i++)
	{
		if(i < (int)intvec.size())
		{
			pArr[i] = intvec[i];
		}
		else
		{
			pArr[i] = 0;
		}
	}

	return true;
}

bool	FillData(string* pArr,int nMaxLen,vector<string> & strvec)
{
	if(pArr == NULL || nMaxLen <= 0)
	{
		return false;
	}

	for(int i=0;i<nMaxLen;i++)
	{
		if(i < (int)strvec.size())
		{
			pArr[i] = strvec[i];
		}
		else
		{
			pArr[i] = "";
		}
	}

	return true;
}
