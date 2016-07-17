#ifndef _MESSAGEDEF_H_
#define _MESSAGEDEF_H_
#pragma once
//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。
enum S2C_EVENT
{
	S2C_EVENT_BASE = 511,
	S2C_RseBroadcast,		//^s^RseBroadcast#
	S2C_RseAuthState,		//^s^RseAuthState#
};
enum C2S_EVENT
{
	C2S_EVENT_BASE = 1023,
	C2S_EVENT_MAX,
};
#endif

