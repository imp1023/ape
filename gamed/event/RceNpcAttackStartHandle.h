#ifndef _RCENPCATTACKSTARTHANDLE_H_
#define _RCENPCATTACKSTARTHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceNpcAttackStartHandle : public CBaseEvent
{
public:
	RceNpcAttackStartHandle();
	~RceNpcAttackStartHandle();
	RegistEvent(RceNpcAttackStartHandle,C2S_RceNpcAttackStart)

private:
	void handle(Event* e);
};
#endif

