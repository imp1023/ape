#include "../PlatEventHandler.h"
#include "../PlatDataHandler.h"
#include "DealPlatEvent.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../common/const_def.h"
#include "../../event/EventDefine.h"
#include "../../logic/friendDB.pb.h"
#include "../../logic/FriendGiftCfg.h"
#include <math.h>
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../PlatConfigMgr.h"
#include "../PlatDefine.h"
#include "../FriendInfoHandler.h"
#include "../../logic/PlatInviteFriendMgr.h"
#include "../../logic/InviteFrdGiftCfg.h"
#include "../../logic/CostGiftCfg.h"

extern int G_PlatSrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealPlatEvent::DealPlatEvent()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealPlatEvent::~DealPlatEvent()
{

}

void DealPlatEvent::registHandler()
{

	eh_->getEventHandler()->registHandler(EVENT_ADMIN_RELOAD, ((ProcessRoutine) DealPlatEvent::handle_));	
	eh_->getEventHandler()->registHandler(EVENT_USER_LOGIN, ((ProcessRoutine) DealPlatEvent::handle_));	
}

void DealPlatEvent::handle(Event* e)
{
	int64 nUserID = e->uid();
	int nPlatSrvID = e->platsrvid();

	if(e->cmd() == EVENT_ADMIN_RELOAD)
	{
		processReloadConfig(e);
		return;
	}

	LOG4CXX_DEBUG(logger_,"get msg,type:"<<e->cmd()<<",userID:"<<nUserID);

	switch(e->cmd())
	{
	case EVENT_USER_LOGIN:
		{
			HandleUserLogin(e);
		}
		break;
	default:
		break;
	}
}

void DealPlatEvent::HandleUserLogin(Event* e)
{
	if (!e->has_userlogin_req())
	{
		return;
	}
	PlatDataHandler* pDH = eh_->getDataHandler();
	const UserLogin_Req& req = e->userlogin_req();

	const string& platid = req.platform_id();
	if(platid.size()<=0)
		return;

	int siteid = req.siteid();
	const string& name = req.name();
	const string& profile_link = req.profile_link();
	int gender = (req.gender() == 0) ? Gender_Girl : Gender_Boy ;
	vector<string> friends_platid;
	for (int i = 0; i < req.friends_platid_size(); i ++)
	{
		friends_platid.push_back(req.friends_platid(i));
	}
	bool isYellowDmd = req.is_yellow_dmd();
	bool isYellowDmdYear = req.is_yellow_dmd_year();
	bool isHighYellowDmd = req.is_high_yellow_dmd();
	int lvlYellowDmd = req.yellow_dmd_lv();
	string strTaskMarketId = req.taskmarketid();
	//string strChannel = req.channel();
	WebCrossPromotion* pPromotion = e->mutable_userlogin_req()->mutable_promotion();
	int region = req.region();
	int nNewUserRegion = (int)req.defregion();
	LOG4CXX_INFO(logger_, "Game request UserLogin with openid: " << platid);

	LoadStatus state = LOAD_INVALID;
	int level = 0;
	int nRtnRegion=0;
	int64 uid=0;
	bool bIsNewPlayer = false;
	bool isHighDmdYear = req.is_high_dmd_year();
	DB_Plat* pDBPlat = processUserLogin(platid, siteid, name, profile_link, gender,
		friends_platid, isYellowDmd, isYellowDmdYear, lvlYellowDmd, state,region,nNewUserRegion,level,nRtnRegion,uid,bIsNewPlayer,isHighYellowDmd,strTaskMarketId,pPromotion,isHighDmdYear);
	if (pDBPlat != NULL)
	{
		//UserLogin_Rsp* rsp = e->mutable_userlogin_rsp();
		//rsp->set_server(eh_->getServerIp());
		//rsp->set_port(eh_->getServerPort());
		//rsp->set_port1(eh_->getServerPort1());
		//rsp->set_uid(user->GetUid());
		//rsp->set_secret(user->secret(eh_->getReversion()));
		//rsp->set_webfd(webfd);
		//rsp->set_sid(sid);
		//rsp->set_region(region);
		//rsp->set_mainload("0");//此项已失效
		//rsp->set_version(FlashVersionCfg::Instance().GetCfgVersion(eh_->GetSrvID()));
		//e->clear_userlogin_req();
		e->set_state(UserLogin_PG_Rsp);
		//空间十年,所有的服只能领一次
		if (req.nbackflowflag()==4)
		{
			if (pDBPlat->qqtenyearbackflowflag() > 1)
			{
				e->mutable_userlogin_req()->set_nbackflowflag(0);
			}
		/*	else
			{
				pDBPlat->set_qqtenyearbackflowflag(1);
			}*/
			//还是领过再置标志
		}
		if(uid>0)
		{
			e->mutable_userlogin_req()->set_uid(uid);
			e->mutable_userlogin_req()->set_level(level);
			e->mutable_userlogin_req()->set_region(nRtnRegion);
			e->mutable_userlogin_req()->set_undealgiftcnt(pDH->GetUnDealGiftCnt(pDBPlat));
			e->mutable_userlogin_req()->set_undealfrdgiftcnt(pDH->GetUnDealFrdGiftCnt(pDBPlat));
			e->mutable_userlogin_req()->set_yellowdmdtime(pDBPlat->yellowdmdtime());
			e->mutable_userlogin_req()->set_invitefrdcnt(pDBPlat->invitefrdcnt());
			e->mutable_userlogin_req()->set_invitefriendid(pDBPlat->invitefriends().invitefriendid());
			e->mutable_userlogin_req()->set_isnewplayer( bIsNewPlayer );
			e->mutable_userlogin_req()->set_bluedmdtime(pDBPlat->bluedmdtime());

			DB_PlatGift* pDBGiftInfo = pDBPlat->mutable_giftinfo();
			e->mutable_userlogin_req()->set_daysendgiftcnt(pDBGiftInfo->daysendgiftcnt());
			e->mutable_userlogin_req()->set_invitefrdcntday(pDBPlat->invitefrdcntday());
			e->mutable_userlogin_req()->set_qqtaskstep(pDBPlat->mutable_taskmark()->step());
            e->mutable_userlogin_req()->set_nadultflag(100);        //每次登陆都设置为可登陆 如果没有验证的话会有其他消息将玩家再次踢下线
			
			pPromotion->set_chanle( pDBPlat->channel() );

// 			RceInviteFriend* pinviteMsg = e->mutable_ce_rceinvitefriend();
// 			pinviteMsg->set_friendcount(pDBPlat->invitefrdcntday());
// 			pinviteMsg->set_giftgain(pDBPlat->gaininvitefrdday());

			for ( int i = 0; i < pDBPlat->invitegiftrecord_size(); i++ )
			{
				e->mutable_userlogin_req()->add_invitegiftrecord( pDBPlat->invitegiftrecord(i) );
			}

			int gid = pDH->getGamedIdByUserId(uid);
			eh_->sendEventToGamed(e,gid);

			//SendInviteFriendMsg(pDBPlat,uid);

			e->set_state(Status_Plat_To_Game);
			e->set_cmd(EVENT_COSTGIFT_INVITE_FRD);
			e->set_userplat( platid );
			e->set_uid( uid );
// 			CostGiftMsg* msg = e->mutable_costgiftmsg();
// 			for( int i = 0; i < pDBPlat->daygetcostgift_size(); i++ ) 
// 			{
// 				msg->add_gotgifts( pDBPlat->daygetcostgift(i) );
// 			}
// 			msg->set_invitedcnt( pDBPlat->invitefrdcntdaybylimit() );
			eh_->sendEventToGamed(e,gid);

			if ( pDBPlat->has_callbackfrduid() )
			{
				e->set_state(Status_Plat_To_Game);
				e->set_cmd(EVENT_CALLBACKFRRD_GIFT);
				e->set_userplat( platid );
				e->set_uid( pDBPlat->callbackfrduid() );
				eh_->sendEventToGamed(e,pDH->getGamedIdByUserId(pDBPlat->callbackfrduid()));
				SYS_LOG(uid, LT_CallBackFriendBack, 0 , 0, platid.c_str());
				pDBPlat->clear_callbackfrduid();
			}
		}

	}
	else
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
	}

}

DB_Plat* DealPlatEvent::processUserLogin(const string& platid, int siteid, const string& name,
										 const string& profile_link, int gender, vector<string> friends_platid,
										 bool isYellowDmd, bool isYellowDmdYear, int lvlYellowDmd, LoadStatus& state,
										 int region,int nNewUserRegion,int& level,int& nRtnRegion,int64& uid,bool& bIsNewPlayer,
										 bool isHighYellowDmd,string strTaskMarketId,WebCrossPromotion* pPromotion,bool isHighDmdYear)
{
	PlatDataHandler* dh = eh_->getDataHandler();
	DB_Plat *pDBPlat = NULL;
	// 检测当前区是否有用户，先检测区号是否合法
	if (region>=0&&!serverConfig.findregion_id(region))
	{
		state = LOAD_ERROR;
		return NULL;
	}
	level = 0;
	uid = 0;
	nRtnRegion = -1;
	bool bCreate = false;
	// 检测映射表是否存在
	pDBPlat = dh->GetDBPlat(platid,&state,true);
	if(pDBPlat==NULL)
	{
		if(state == LOAD_EMPTY)
		{//新用户注册
			pDBPlat = new DB_Plat();
			pDBPlat->set_platform_id(platid);
			dh->mapPlatidToUid(platid,pDBPlat);
			bCreate = true;
			bIsNewPlayer = bCreate;
		}
	}
	else 
	{//加载成功
	}

	if(pDBPlat)
	{
		//调试数据，为了找到部分玩家回档的问题，2013.01.26
		string strOldPlat;
		int nOldRegionSize = 0;
		int nFindType = -1;
		strOldPlat		= pDBPlat->platform_id();
		nOldRegionSize	= pDBPlat->platregion_size();

		// 校验该区是否有角色
		if(region<0)
		{//最后一次登录区
			if(!pDBPlat->has_lastregion()||pDBPlat->lastregion()<0)
			{
				region = nNewUserRegion;
			}
			else
			{
				region = pDBPlat->lastregion();
				if (!serverConfig.findregion_id(region))
				{
					region = 0;
				}
			}
		}
		nRtnRegion = region;

		bool bFind = false;

		for(int i=0;i<pDBPlat->platregion_size();i++)
		{
			DB_PlatRegion* pDBRgn = pDBPlat->mutable_platregion(i);
			if(pDBRgn&&pDBRgn->srvregion()==region)
			{
				uid = pDBRgn->uid();
				level = pDBRgn->level();
				if(uid<=0)
				{//此处应该不会走到
					uid = dh->allocateUserID(platid,region);
					pDBRgn->set_uid(uid);
					level = 1;
					pDBRgn->set_level(level);

					SYS_LOG(uid,LT_RegisterError,0,0,platid<<region<<level);
					nFindType = 1;
					bCreate = true;
				}
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{//新玩家
			uid = dh->GetUid(pDBPlat,region);
			if(uid<=0)
			{
				uid = dh->allocateUserID(platid,region);
			}
			DB_PlatRegion* pDBRgn = pDBPlat->add_platregion();
			pDBRgn->set_srvregion(region);
			pDBRgn->set_uid(uid);
			level = 1;
			pDBRgn->set_level(level);
			nFindType = 2;
			bCreate = true;
		}

		pDBPlat->set_lastregion(region);

		dh->CheckDBPlat(pDBPlat);
		PLAT_TYPE emType = PLAT_TYPE(siteid);
		if(emType>=PLAT_XIAOYOU&&emType<PLAT_TYPE_MAX)
		{
			pDBPlat->set_name(emType,name);
			pDBPlat->set_profile_link(emType,profile_link);
		}
		pDBPlat->set_gender(gender);
		if (emType == PLAT_QHALL)
		{
			pDBPlat->set_qhalldmdlvl(lvlYellowDmd);
			pDBPlat->set_isqhalldmd(isYellowDmd);
			pDBPlat->set_isqhalldmdyear(isYellowDmdYear);
			pDBPlat->set_ishighqhalldmd(isHighYellowDmd);
			pDBPlat->set_ishighqhalldmdyear(isHighDmdYear);
		}
		else{
			if(emType==PLAT_QAdd)
			{
				pDBPlat->set_qqdmdlvl(lvlYellowDmd);
				pDBPlat->set_isqqdmd(isYellowDmd);
				pDBPlat->set_isqqdmdyear(isYellowDmdYear);
			}
			else
			{
				pDBPlat->set_yellowdmdlvl(lvlYellowDmd);
				pDBPlat->set_isyellowdmd(isYellowDmd);
				pDBPlat->set_isyellowdmdyear(isYellowDmdYear);
			}	
			pDBPlat->set_ishighyellowdmd(isHighYellowDmd);
		}

		DB_TaskMark* pDBTaskMark = pDBPlat->mutable_taskmark();

		if ( strTaskMarketId.size() > 0 )
		{
			pDBTaskMark->set_id( strTaskMarketId );
			SYS_LOG(0,LT_QQTASK_RECORD,0,0,platid<<strTaskMarketId<<"taskmark");
		}

		bool bFindCDK = false;
		if (  pPromotion && pPromotion->chanle().size()>0) 
		{
			pDBPlat->set_channel( pPromotion->chanle() );
			
			if(pPromotion->date().size()>0&&pPromotion->cdk_size()>0)
			{
				DB_PlatCrossPromotion* pDBPromotion = pDBPlat->mutable_promotion();
				bFindCDK = false;
				for(int i=0;i<pDBPromotion->data_size();i++)
				{
					DB_PlatCrossPromotionData* pDBData = pDBPromotion->mutable_data(i);
					if(pDBData->chanle()==pPromotion->chanle()&&pDBData->date()==pPromotion->date())
					{
						bFindCDK = true;
						break;
					}
				}
				if(!bFindCDK)
				{
					pPromotion->set_canuse(true);
					DB_PlatCrossPromotionData* pDBData = pDBPromotion->add_data();
					pDBData->set_chanle(pPromotion->chanle());
					pDBData->set_date(pPromotion->date());
				}
				else
				{
					pPromotion->set_canuse(false);
				}
				string strLog;
				for(int i=0;i<pPromotion->cdk_size();i++)
				{
					char szBuf[256];
					sprintf(szBuf,"%s:",pPromotion->cdk(i).c_str());
					strLog += szBuf;
				}
				SYS_LOG(0,LT_CHANNEL,0,0,platid<< pPromotion->chanle().c_str()<<"channel"<<bFindCDK<<pPromotion->date().c_str()<<strLog.c_str());
			}
			else
			{
				SYS_LOG(0,LT_CHANNEL,0,0,platid<< pPromotion->chanle().c_str()<<"channel"<<bFindCDK<<pPromotion->date().c_str()<<"");
			}

		}
		
		if(bCreate)
		{
			
			pDBPlat->set_platform_id(platid);
			dh->savePlatidMap(pDBPlat);
			SYS_LOG(uid, LT_Register, 0 , 0, platid.c_str()<<nRtnRegion<<bIsNewPlayer<<strOldPlat<<nOldRegionSize<<nFindType);

		}

		//登陆设置
		OnPlayerOnLine(pDBPlat);

		dh->markPlatDirty(pDBPlat);
		if(FriendInfoHandler::Instance().CanUse())
		{
			dh->SaveFrd2TT(pDBPlat);
		}
	}

	return pDBPlat;
}


void DealPlatEvent::processReloadConfig(Event* e)
{
	PlatConfigMgr::GetInst()->LoadPlatConfigInfo(eh_->GetPlatSrvID(),false);
}

void DealPlatEvent::OnPlayerOnLine(DB_Plat* pDBPlat)
{
	if(pDBPlat)
	{
		//刚登陆
		pDBPlat->set_cancallback(true);

		//新一天
		time_t tlastcalltime = (time_t)pDBPlat->lastlogintime();
		time_t timenow = time(NULL);

		tm daytm1 = *localtime(&tlastcalltime);
		tm daytm2 = *localtime(&timenow);

		//新一天
		if(daytm2.tm_year != daytm1.tm_year || daytm2.tm_yday != daytm1.tm_yday)
		{
			pDBPlat->set_invitefrdcntday(0);
			pDBPlat->set_gaininvitefrdday(false);

			pDBPlat->set_invitefrdcntdaybylimit(0);
			pDBPlat->clear_daygetcostgift();

			pDBPlat->set_dayinvitefrdbackcnt( 0 );
			pDBPlat->set_daygetcoupon( 0 );

			//InitNewDayGiftInfo(pDBPlat->mutable_giftinfo(),timenow);
		}
		pDBPlat->set_lastlogintime(timenow);

		for(int i=0;i<pDBPlat->platregion_size();i++)
		{
			DB_PlatRegion* pDBRgn = pDBPlat->mutable_platregion(i);
			if(pDBRgn&&pDBRgn->srvregion()==pDBPlat->lastregion())
			{
				pDBRgn->set_lastlogontime((int)timenow);
			}
		}
	}
}