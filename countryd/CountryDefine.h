#pragma once

#include "countryDB.pb.h"
#include "../common/SysLog.h"

//向统计服务器发送统计信息
//template <class T1,class T2,class T3,class T4>
//void SYS_CountryStat(DB_C_City* pCountry,string strType,T1 subTp1,T2 subTp2,T3 subTp3,T4 val1,int nVal2=0,int nVal3=0,int nVal4=0,int nVal5=0,int nVal6=0,int nVal7=0)
//{
//	if(pCountry==NULL)
//		return;
//
//	//暂定空间名字
//
//	string str_name = "";
//	string str_profile = "";
//
//	if(pCountry->name_size() > 1)
//	{
//		str_name = pCountry->name(1);
//	}
//	else if(pCountry->name_size() > 0)
//	{
//		str_name = pCountry->name(0);
//	}
//
//	if(pCountry->profile_link_size() > 1)
//	{
//		str_profile = pCountry->profile_link(1);
//	}
//	else if(pCountry->profile_link_size() > 0)
//	{
//		str_profile = pCountry->profile_link(0);
//	}
//
//	SYS_STAT_PLAT(pCountry->platform_id(),strType,str_name,pCountry->gender(),str_profile,pCountry->isyellowdmd(),pCountry->yellowdmdlvl(),pCountry->isyellowdmdyear(),pCountry->isqqdmd(),pCountry->qqdmdlvl(),pCountry->isqqdmdyear(),
//		subTp1,subTp2,subTp3,val1,nVal2,nVal3,nVal4,nVal5,nVal6,nVal7);
//
//}
