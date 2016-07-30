#ifndef _RCEQUERYSTARINFOHANDLE_H_
#define _RCEQUERYSTARINFOHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceQueryStarInfoHandle : public CBaseEvent
{
public:
	RceQueryStarInfoHandle();
	~RceQueryStarInfoHandle();
	RegistEvent(RceQueryStarInfoHandle,C2S_RceQueryStarInfo)

private:
	void handle(Event* e);
	void handle_c2country(Event *e);
	void handle_country2s(Event *e);
};
#endif

