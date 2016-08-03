#pragma once
#include "const_def.h"
#include <string>
#ifndef WIN32
#include <string.h>
#include <stdio.h>
#endif
using namespace std;

int getPlatidHash(const string& platid);
int getUidHash(int64 uid);
int getPhysicsRegion(int64 uid);

time_t	GetDayTimeByStr(const char* pDay, const char* pTime);
time_t	GetDayTimeByNum(int nYear,int nMonth,int nDay, int nHour, int nMinut, int nSec);

tm		GetDayTimeByStr_tm(const char* pDay, const char* pTime);
tm		GetDayTimeByNum_tm(int nYear,int nMonth,int nDay, int nHour, int nMinut, int nSec);
bool	Comparetm(tm& first, tm& last);
int     julianDate(time_t time);//儒略日,用来计算间隔日数

bool IsSameDay(time_t ltTime1,time_t ltTime2);

bool	Token2Data(string& strInput,vector<int> & intvec);
bool	Token2Data(string& strInput,vector<string> & strvec);
bool	FillData(int* pArr,int nMaxLen,vector<int> & intvec);
bool	FillData(string* pArr,int nMaxLen,vector<string> & strvec);
