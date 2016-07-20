#ifndef _RCEOBTAINPAYURLPARAMEHANDLE_H_
#define _RCEOBTAINPAYURLPARAMEHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceObtainPayUrlParameHandle : public CBaseEvent
{
public:
	RceObtainPayUrlParameHandle();
	~RceObtainPayUrlParameHandle();
	RegistEvent(RceObtainPayUrlParameHandle,C2S_RceObtainPayUrlParame)

private:
	void handle(Event* e);
};
#endif

