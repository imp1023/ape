#ifndef _RCEQUERYRECEIVEWISHITEMHANDLE_H_
#define _RCEQUERYRECEIVEWISHITEMHANDLE_H_
#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
class RceQueryReceiveWishItemHandle : public CBaseEvent
{
public:
	RceQueryReceiveWishItemHandle();
	~RceQueryReceiveWishItemHandle();
	RegistEvent(RceQueryReceiveWishItemHandle,C2S_RceQueryReceiveWishItem)

private:
	void handle(Event* e);
};
#endif

