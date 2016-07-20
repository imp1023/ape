#ifndef _RCEOBTAINHANGARSHELPHANDLE_H_
#define _RCEOBTAINHANGARSHELPHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceObtainHangarsHelpHandle : public CBaseEvent
{
public:
	RceObtainHangarsHelpHandle();
	~RceObtainHangarsHelpHandle();
	RegistEvent(RceObtainHangarsHelpHandle,C2S_RceObtainHangarsHelp)

private:
	void handle(Event* e);
};
#endif

