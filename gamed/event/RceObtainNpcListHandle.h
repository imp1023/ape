#ifndef _RCEOBTAINNPCLISTHANDLE_H_
#define _RCEOBTAINNPCLISTHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceObtainNpcListHandle : public CBaseEvent
{
public:
	RceObtainNpcListHandle();
	~RceObtainNpcListHandle();
	RegistEvent(RceObtainNpcListHandle,C2S_RceObtainNpcList)

private:
	void handle(Event* e);
};
#endif

