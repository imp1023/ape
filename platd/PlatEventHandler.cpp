#define _CRT_SECURE_NO_WARNINGS
#include "PlatEventHandler.h"
#include "PlatNetHandler.h"
#include "PlatDataHandler.h"
#include "../common/ServerConfig.h"
#include "platDB.pb.h"
#include "../event/EventDefine.h"
#include "event/DealPlatEvent.h"



extern int G_PlatSrvD_ID; 

PlatEventHandler::PlatEventHandler(EventQueue *eq, PlatDataHandler* dh, PlatNetHandler *nh, int nid)
:eq_(eq), dh_(dh), nh_(nh), nid_(nid)
{
	eh_ = new EventHandler(eq, dh, nh, nid);
	logger_ = log4cxx::Logger::getLogger("PlatEventHandler");
	initialEventProcessors();
}

PlatEventHandler::~PlatEventHandler(void)
{
}

void PlatEventHandler::start()
{
	eh_->start();
}



bool PlatEventHandler::sendEventToUser(Event *e, int64 uid) {

	int gid = dh_->getGamedIdByUserId(uid);
	e->set_uid(uid);//game端，根据uid选择对应用户
	bool bSuc = nh_->sendEventToGamed(e, gid);
	return bSuc;
}

bool PlatEventHandler::sendEventToGamed(Event *e, int gid) {
	return nh_->sendEventToGamed(e, gid);
}


void PlatEventHandler::sendFdString(int fd, const string &content) {
	nh_->sendSizedFdString(fd, content.c_str());
}

#include "event/UpdateWorkingStatus.h"


void PlatEventHandler::initialEventProcessors()
{
	// add Event Processors here
	UpdateWorkingStatus::createInstance(this);
	DealPlatEvent::createInstance(this);
}

#if 0
void PlatEventHandler::SendGiftList2User(int64 uid,DB_Plat* pDBPlat,int nType)
{
	if(pDBPlat==NULL||uid<=0)
		return;

	DB_PlatGift* pDBGiftInfo = pDBPlat->mutable_giftinfo();

	Event evn;
	evn.set_uid(uid);
	evn.set_cmd(C2S_RceGiftList);
	evn.set_time(0);
	evn.set_state(Status_Plat_To_Game);

	RseGiftList* pRseGift = evn.mutable_se_rsegiftlist();
	pRseGift->set_type(nType);
	pRseGift->set_dayaskcnt(pDBGiftInfo->dayaskfrdidlst_size());
	pRseGift->set_askgiftcnt(pDBGiftInfo->dayaskfrdidlst_size());
	pRseGift->set_getgiftcnt(pDBGiftInfo->daysendgiftcnt());

	pRseGift->set_bsendgiftgain(pDBGiftInfo->bsendgiftgain());
	pRseGift->set_baskgiftgain(pDBGiftInfo->baskgiftgain());

	for(int i=0;i<pDBGiftInfo->dayaskfrdidlst_size();i++)
	{
		pRseGift->add_canaskfrdopenid(pDBGiftInfo->dayaskfrdidlst(i));
	}
	int nCount = 0;
	for(int i=0;i<pDBGiftInfo->getgiftinfo_size();i++)
	{
		DB_PlatGiftFrdInfo* pGiftInfo = pDBGiftInfo->mutable_getgiftinfo(i);
		if(pGiftInfo->messageid()<=0||pGiftInfo->friendid()<=0)
			continue;
		nCount++;
		pRseGift->add_messageid(pGiftInfo->messageid());
		pRseGift->add_getgiftid(pGiftInfo->giftid());
		//pRseGift->add_getfrdid(toString(pGiftInfo->friendid()));
		pRseGift->add_getfrdid(pGiftInfo->frdopenid());
		pRseGift->add_gettime((int)pGiftInfo->gettime());
		pRseGift->add_getgifttype(pGiftInfo->gifttype());
		pRseGift->add_hasdone(0);
		pRseGift->add_playername(pGiftInfo->frdname());
		pRseGift->add_playerurl(pGiftInfo->frdurl());
		if(nCount>=100)
		{
			break;
		}
	}
	nCount = 0;
	for(int i=0;i<pDBGiftInfo->finishgiftinfo_size();i++)
	{
		DB_PlatGiftFrdInfo* pGiftInfo = pDBGiftInfo->mutable_finishgiftinfo(i);
		if(pGiftInfo->messageid()<=0||pGiftInfo->friendid()<=0)
			continue;
		nCount++;
		pRseGift->add_messageid(pGiftInfo->messageid());
		pRseGift->add_getgiftid(pGiftInfo->giftid());
		//pRseGift->add_getfrdid(toString(pGiftInfo->friendid()));
		pRseGift->add_getfrdid(pGiftInfo->frdopenid());
		pRseGift->add_gettime((int)pGiftInfo->gettime());
		pRseGift->add_getgifttype(pGiftInfo->gifttype());
		pRseGift->add_hasdone(1);
		pRseGift->add_playername(pGiftInfo->frdname());
		pRseGift->add_playerurl(pGiftInfo->frdurl());
		if(nCount>=100)
			break;
	}

	int nGameID = dh_->getGamedIdByUserId(uid);
	sendEventToGamed(&evn,nGameID);
}

void PlatEventHandler::SendGift2Frd(int64 uid,string& platID,int nGiftID,int nType,const string& strFrdPlatID)
{
	if(uid<=0||nGiftID<=0||strFrdPlatID.size()<2)
		return;
	//SYS_LOG(pUser->GetUid(),LT_GiftSend,0,0,strFrdPlatID.c_str()<<nType<<nGiftID);

	if(dh_->isPlatidLocal(strFrdPlatID))
	{
		Event* ev = eq_->allocateEvent();
		ev->set_cmd(EVENT_WEB_GIFT2FRD);
		ev->set_state(Status_Normal_Game);
		ev->set_time(0);
		ev->set_uid(1);

		ev->set_userplat(strFrdPlatID);
		WebGift2Frd* pRequest = ev->mutable_webgift2frd();
		pRequest->set_giftid(nGiftID);
		pRequest->set_platid(strFrdPlatID);
		pRequest->set_type(nType);
		pRequest->set_sendfrduid(uid);
		pRequest->set_sendfrdplatid(platID);
		eq_->safePushEvent(ev);
	}	
	else
	{//发送给game
		Event evn;
		evn.set_cmd(EVENT_WEB_GIFT2FRD);
		evn.set_state(Status_Plat_To_Game);
		evn.set_time(0);
		evn.set_uid(1);
		evn.set_userplat(strFrdPlatID);
		ForwardInfo* pFInfo = evn.mutable_forwardinfo();
		pFInfo->set_platid(strFrdPlatID);
		WebGift2Frd* pRequest = evn.mutable_webgift2frd();
		pRequest->set_giftid(nGiftID);
		pRequest->set_platid(strFrdPlatID);
		pRequest->set_type(nType);
		pRequest->set_sendfrduid(uid);
		pRequest->set_sendfrdplatid(platID);

		int nGameID = dh_->getGamedIdByUserId(uid);
		sendEventToGamed(&evn,nGameID);
	}
}
#endif