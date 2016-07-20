#pragma once
//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。
#include "BaseEvent.h"
#include "../../event/EventDefine.h"
#include "../WorldEventHandler.h"

class DealWorldEvent : public CBaseEvent
{
public:
    DealWorldEvent();
    ~DealWorldEvent();
    RegistWorldEvent(DealWorldEvent)

private:
    void registHandler();
    void handle(Event* e);

protected:
    void processEventForward(Event* e);
    void processEventForwardByUid(Event* e);
    void processEventForwardByPlatId(Event* e);
} ;
