#ifndef _RCEQUERYVISITHELPSGIFTUNITSONBUNKERHANDLE_H_
#define _RCEQUERYVISITHELPSGIFTUNITSONBUNKERHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceQueryVisitHelpsGiftUnitsOnBunkerHandle : public CBaseEvent
{
public:
	RceQueryVisitHelpsGiftUnitsOnBunkerHandle();
	~RceQueryVisitHelpsGiftUnitsOnBunkerHandle();
	RegistEvent(RceQueryVisitHelpsGiftUnitsOnBunkerHandle,C2S_RceQueryVisitHelpsGiftUnitsOnBunker)

private:
	void handle(Event* e);
};
#endif

