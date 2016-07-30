#ifndef _RCEQUERYCHECKANDLOCKACCOUNTIFPOSSIBLEHANDLE_H_
#define _RCEQUERYCHECKANDLOCKACCOUNTIFPOSSIBLEHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceQueryCheckAndLockAccountIfPossibleHandle : public CBaseEvent
{
public:
	RceQueryCheckAndLockAccountIfPossibleHandle();
	~RceQueryCheckAndLockAccountIfPossibleHandle();
	RegistEvent(RceQueryCheckAndLockAccountIfPossibleHandle,C2S_RceQueryCheckAndLockAccountIfPossible)

private:
	void handle(Event* e);
	void handle_selfload(Event* e);
	void handle_romateload(Event* e);
	void handle_romatereturn(Event* e);
};
#endif

