#ifndef _RCEQUERYCHECKANDLOCKACCOUNTIFPOSSIBLEHANDLE_H_
#define _RCEQUERYCHECKANDLOCKACCOUNTIFPOSSIBLEHANDLE_H_
#pragma once
//���ļ���.cpp�ļ�Ϊʹ�ù��ߴ����������ļ����޸ģ�����������ָ��ļ������������´�����
#include "BaseEvent.h"
class RceQueryCheckAndLockAccountIfPossibleHandle : public CBaseEvent
{
public:
	RceQueryCheckAndLockAccountIfPossibleHandle();
	~RceQueryCheckAndLockAccountIfPossibleHandle();
	RegistEvent(RceQueryCheckAndLockAccountIfPossibleHandle,C2S_RceQueryCheckAndLockAccountIfPossible)

private:
	void handle(Event* e);
};
#endif
