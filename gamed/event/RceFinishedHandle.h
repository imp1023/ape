#ifndef _RCEFINISHEDHANDLE_H_
#define _RCEFINISHEDHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceFinishedHandle : public CBaseEvent
{
public:
	RceFinishedHandle();
	~RceFinishedHandle();
	RegistEvent(RceFinishedHandle,C2S_RceFinished)

private:
	void handle(Event* e);
	void handle_selfload(Event* e);
	void handle_romateload(Event* e);
	void handle_romatereturn(Event* e);

};
#endif

