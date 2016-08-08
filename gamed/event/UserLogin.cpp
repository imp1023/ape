#include "UserLogin.h"
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/Player.h"
#include "../../logic/GameSrvCfg.h"
#include "../../logic/FlashVersionCfg.h"
#include "../../common/SysLog.h"
#include "../../common/Msg2QQ.h"
#include "../../common/distribution.h"
#include "../../event/EventQueue.h"
#include "../../logic/Player.h"
#include "../common/distribution.h"
#include "../GameHttpHandler.h"
#include <time.h>

UserLogin* UserLogin::instance_ = NULL;

void UserLogin::handle(Event* e)
{
    switch (e->cmd())
    {
        case EVENT_USER_LOGIN:
            HandleUserLogin(e);
            break;
        case EVENT_USER_LOGOUT:
            HandleUserLogout(e);
            break;
        default:
            break;
    }
}

void UserLogin::HandleUserLogin(Event* e)
{
    if (e->state() == UserLogin_WG_Req)
    {
        if (!e->has_userlogin_req())
        {
            return;
        }
		const UserLogin_Req& req = e->userlogin_req();
		int nRegion = req.region();
		//nRegion: -1.上次登录区
		if(nRegion <0)
		{
			e->mutable_userlogin_req()->set_defregion(GameSrvCfg::Instance().DefaultRegion());
		}
		else if (!serverConfig.findregion_id(req.region()))
		{
			return;
		}
		GameDataHandler* dh = eh_->getDataHandler();
		
		//int64 uid = dh->GetUserID(req.platform_id(),req.region());
		//if(uid>0)
		//{//该用户已在内存中
		//	UserLogin_Req* pReq = e->mutable_userlogin_req();
		//	pReq->set_uid(uid);
		//	e->set_state(UserLogin_PG_Rsp);
		//	handle_WG_UserLogin(e);
		//}
		//else
		{
			e->set_state(UserLogin_GP_Req);
			eh_->sendEventToPlat(e,eh_->GetPlatSrvID(req.platform_id()));
		}
    }
	else if(e->state() == UserLogin_PG_Rsp)
	{
		if (!e->has_userlogin_req())
		{
			return;
		}
		const UserLogin_Req& req = e->userlogin_req();
		if(req.isnewplayer()){
			NewStar_Req *pReq = e->mutable_newstarreq();
			pReq->set_accountid(req.uid());
			pReq->set_planetid(1);
			pReq->set_name(req.platform_id());
			pReq->set_url(req.profile_link());
			e->set_state(UserLogin_GCo_Req);
			eh_->sendEventToCountry(e, req.region());
		}else{
			handle_WG_UserLogin(e);
		}
	}
	else if(e->state() == UserLogin_CoG_Rsp)
	{
		handle_WG_UserLogin(e);
 	}
    else
    {
        LOG4CXX_ERROR(logger_, "Invalid Event.\n" << e->DebugString());
    }
}

void UserLogin::HandleUserLogout(Event* e)
{

    int64 nUserID = e->uid();
    switch (e->state())
    {
        case Status_Normal_Game:
        {
            GameDataHandler* dh = eh_->getDataHandler();
            User* pUser = dh->getUser(nUserID, NULL, false);
            if (pUser == NULL)
            {
                return;
            }
            WG_UserLeave* pUserLeave = e->mutable_wg_userleave();
            if (pUserLeave->fd() != pUser->fd())
                return;

            pUser->setOnline(false);
//			pUser->OnUserOnOffLine();
			dh->PopOnlineUserID(pUser->GetUid());
//			pUser->GetPlayer()->SetInviteUserID(0,false);

			//pUser->SetFriendCache(pUser->GetPlattype(), dh);
//			pUser->SetGameStarCache(pUser->GetPlattype(),dh,true);
			
			if(pUser->GetPlayer()->IsAttacking()){
				eh_->PushEventAttackFalse(pUser);
			}

			pUser->setFd(0);
           
            time_t lt = time(NULL) - pUser->GetLastLoginTime();

			//设置离线保护时间
// 			if ( pUser->GetPlayer() != NULL )
// 			{
// 				GameDataHandler* pUserManager = eh_->getDataHandler();
// 				GameStarInfo* pStar = pUserManager->getGameStarInfo(pUser->GetUid());
// 				if( pStar == NULL )
// 				{
// 					return;
// 				}
// 				int nProtectTm = 60;
// 				pStar->m_pPlayerBase->set_logoutsafetime((int)(time(NULL) + nProtectTm));
// 				pUser->GetPlayer()->SetLogoutProtectTm( nProtectTm );
// 			}
// 			
// 			SYS_LOG(nUserID, LT_LogOut, 0, 0, lt << pUser->GetUserLevel()
// 				<<pUser->GetPlayer()->GetBaseValue()<<pUser->GetPlayer()->GetBasePoint()
// 				<<pUser->GetPlayer()->GetResCnt(RC_Metal)<<pUser->GetPlayer()->GetResCnt(RC_Oil)
// 				<<pUser->GetPlayer()->GetCredit());
// 			SYS_UserStat(pUser,false,"LogOut","","","",lt,
// 				pUser->GetResStat()->m_nAddMedal,pUser->GetResStat()->m_nAddOil
// 				,pUser->GetResStat()->m_nCostMedal,pUser->GetResStat()->m_nCostOil,
// 				pUser->GetResStat()->m_nPickMedal,pUser->GetResStat()->m_nPickOil);
// 			int nTotalLV = pUser->GetPlayer()->CacuScienceTotalLV();
// 			/*if(nTotalLV>20)
// 			{
// 				SYS_UserStat4WebRank(pUser,"RankScience","",nTotalLV);
// 			}*/
// 
// 			CMsg2QQ::GetInstance()->TellMsg(MQ_Logout,pUser,0,0,0);
// 
// 			Player* pPlayer = pUser->GetPlayer();
// 			
// 			if ( ServerStatMgr::Instance().GetStatUser(nUserID) )
// 			{
// 				pPlayer->SendServerStat( lt );
// 			}
// 
//             HttpRequireHandler::Instance().SafePushHttpLogOutToQQ(pUser, pUser->GetPlattype());

			dh->markUserDirty(pUser);
		}
	}
}

void UserLogin::handle_WG_UserLogin(Event* e)
{
	const NewStar_Rsp& rsp = e->newstarrsp();
	const UserLogin_Req& req = e->userlogin_req();
	int64 uid = req.uid();
	if(uid<=0)
		return;
	int nPhysicsRegion = getPhysicsRegion(uid);
	//判断所属物理分区是否正确
	if(nPhysicsRegion != serverConfig.GetPhysiceRegionByGame(eh_->GetSrvID()))
		return;
	GameDataHandler* dh = eh_->getDataHandler();

	const string& platid = req.platform_id();
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
	int lvlYellowDmd = req.yellow_dmd_lv();
	bool isHighYellowDmd = req.is_high_yellow_dmd();
	bool isHighDmdYear = req.is_high_dmd_year();
	int nBlueTime = req.bluetime();
	int nBlueYearTime = req.blueyeartime();
	int nHighBlueTime = req.highbluetime();
	int nHighYearTime = req.highblueyeartime();

	int webfd = req.webfd();
	string sid = req.sid();
	int region = req.region();
	int nCity  = req.city();
	string strVIA = req.via();	//玩家渠道
	bool isRecall = req.isrecall();
	int nCustomFlag = req.customflag();
	int nBackFlowFlag = req.nbackflowflag();
	const WebCrossPromotion* pWebPromotion = e->mutable_userlogin_req()->mutable_promotion();
	string strChanel = pWebPromotion->chanle();
	LOG4CXX_INFO(logger_, "World request UserLogin with openid: " << platid);
	//if(dh->GetUserID(platid,region)!=uid)
	//{
	//	dh->AddUserID(platid,region,uid);
	//}
	LoadStatus state = LOAD_INVALID;
	User* user = processUserLogin(uid,platid, siteid, name, profile_link, gender,
		friends_platid, isYellowDmd, isYellowDmdYear, lvlYellowDmd, state,
		region,nCity,req.isnewplayer(),strVIA,isHighYellowDmd,strChanel,isHighDmdYear,nBlueTime,nBlueYearTime,nHighBlueTime,nHighYearTime,
		rsp.id(), rsp.name(), rsp.type(), rsp.sku());
	if (user != NULL)
	{
		UserLogin_Rsp* rsp = e->mutable_userlogin_rsp();
		rsp->set_server(eh_->getServerIp());
		rsp->set_port(eh_->getServerPort());
		rsp->set_port1(eh_->getServerPort1());
		rsp->set_uid(user->GetUid());
		rsp->set_secret(user->secret(eh_->getReversion()));
		rsp->set_webfd(webfd);
		rsp->set_sid(sid);
		rsp->set_region(region);
		rsp->set_mainload("0");//此项已失效
		rsp->set_version(FlashVersionCfg::Instance().GetCfgVersion(eh_->GetSrvID()));
		rsp->set_gettime(req.gettime());
		rsp->set_platform_id(req.platform_id());
		rsp->set_playerlvl(user->GetUserLevel());
		rsp->set_logintimes(IsSameDay(user->GetLastLoginTime(),time(NULL))?1:0);
		rsp->set_isnewplayer(req.isnewplayer());
		rsp->set_customflag(req.customflag());
		rsp->set_channel(strChanel);
#if 0
		user->SetUndealGiftCount(req.undealgiftcnt());
		user->SetUndealFrdGiftCount(req.undealfrdgiftcnt());
		user->SetInviteFrdCnt(req.invitefrdcnt());
		user->SetDayInviteFrdCnt(req.invitefrdcntday());
		user->SetDayWebGiftSendCnt(req.daysendgiftcnt());
		user->SetChannel(strChanel);
		user->GetDbUser().set_badult((req.nadultflag() >= 100)?true : false);

		bool bIsNewInvite = false;
		if (user->SetPlatInviteFriendOpenid(req.invitefriendid()))
		{
			bIsNewInvite = true;
		}
		user->GetDbUser().mutable_player()->set_yellowdmdtime(req.yellowdmdtime());

		if (req.isnewplayer() && bIsNewInvite)
		{
			HttpRequireHandler::Instance().SafePushHttpAcceptToQQ(user, user->GetPlattype());
		}
		else if (req.isnewplayer())
		{
			HttpRequireHandler::Instance().SafePushHttpSelfRegistToQQ(user, user->GetPlattype());
		}
		HttpRequireHandler::Instance().SafePushHttpLogOnToQQ(user, user->GetPlattype());

		user->GetDbUser().set_isactivelogon( req.isapp51act() );

		RceInviteFriend* pinviteMsg = e->mutable_ce_rceinvitefriend();
		user->GetPlayer()->SetInviteFrdCntGain(pinviteMsg->friendcount(),pinviteMsg->giftgain());

		//交叉推广
		//if(strChanel.size()>0)
		//{
		//	user->m_mapChanlelFirst[strChanel] += 1;
		//}
		user->SetCrossPromotion(pWebPromotion);

		for ( int i = 0; i < req.invitegiftrecord_size(); i++ )
		{
			DB_InvitedFriendLvGift* pDBInvitedFriendLvGift = user->GetPlayer()->GetDBInvitedFriendLvGift(req.invitegiftrecord(i));
			if ( pDBInvitedFriendLvGift == NULL )
			{
				continue;
			}

			pDBInvitedFriendLvGift->set_hasget( true );
		}
#endif

		e->clear_userlogin_req();
		e->set_state(UserLogin_GW_Rsp);
		eh_->sendEventToWorld(e);


		user->InitRc4Key(rsp->logintimes(),sid);

#if 0
		if(user->GetUserLevel() != req.level())
		{
			eh_->SendUserInfo2Plat(user,UpdatePlat_Level);
		}
		//if(user->GetPlayer()->GetDBPlayer()->invite_platid_size()>0)
		//{//分服前后数据同步
		//	eh_->SendUserInfo2Plat(user,1);
		//	user->GetPlayer()->GetDBPlayer()->clear_invite_platid();
		//}

		//召回老友统计
		if ( isRecall )
		{
			SYS_UserStat(user,false,"CallBackFrdBack",1,"","","");
		}

		if ( nCustomFlag > 0 )
		{
			//2015.03.10空间广告位回流
			const CostGiftTbl *pCostTbl = CostGiftCfg::Instance().GetCostGiftTbl(10098);
			DB_QQBackFlow* pDBQQBackFlow = user->GetPlayer()->GetDBPlayer()->mutable_qqrightbackflowflag();
			if (pCostTbl != NULL &&
				pCostTbl->IsCurrentTime() &&
				pDBQQBackFlow != NULL &&
				pDBQQBackFlow->actflag() != pCostTbl->m_tBeginTime)
			{
				//user->GetPlayer()->GetDBPlayer()->set_customflagtime( time(NULL) );
				SYS_LOG(user->GetPlayer()->GetUserID(),LT_COST_GIFT_CUSTOM_FLAG,0,0,0);
				pDBQQBackFlow->Clear();
				pDBQQBackFlow->set_actflag(pCostTbl->m_tBeginTime);
				pDBQQBackFlow->set_curstate(1);
			}
		}

		if (nBackFlowFlag == 1)
		{//空间顶部应用墙回流
			if ( user->GetPlayer()->GetDBPlayer()->qqtopbackflowflag() == 0)
			{
				user->GetPlayer()->GetDBPlayer()->set_qqtopbackflowflag(1);
			}
		}
		else if (nBackFlowFlag == 2)
		{//空间顶部应用墙回流
			if ( user->GetPlayer()->GetDBPlayer()->qqleftbackflowflag() == 0)
			{
				user->GetPlayer()->GetDBPlayer()->set_qqleftbackflowflag(1);
			}
		}
		else if (nBackFlowFlag == 4)
		{
			if(user->GetPlayer()->GetDBPlayer()->qqtenyearbackflowflag() == 0)
			{
				user->GetPlayer()->GetDBPlayer()->set_qqtenyearbackflowflag(1);
			}
		}

		if ( bIsNewInvite )
		{
			ConstantSetUnit* pUnit = GameConstantSetCfg::Instance().GetGameConstantSet(E_GAMECONSTANTSET_DEF_HOLIDAY_INVITE);
			if ( pUnit != NULL )
			{
				time_t now = time(NULL);
				time_t tBeginTime = GetDayTimeByStr( pUnit->m_strArgs[0].c_str(), pUnit->m_strArgs[1].c_str() );
				time_t tEndTime   = GetDayTimeByStr( pUnit->m_strArgs[2].c_str(), pUnit->m_strArgs[3].c_str() );

				if( now >= tBeginTime && now <= tEndTime )
				{
					user->GetPlayer()->GetDBPlayer()->set_canholidayinvitegiftsend(true);
				}
			}
		}

		user->GetPlayer()->GetDBPlayer()->set_qqtaskstep( req.qqtaskstep() );
#endif
		dh->markUserDirty(user);
	}
	else
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
	}
}

User* UserLogin::processUserLogin(int64 uid,const string& platid, int siteid, const string& name,
        const string& profile_link, int gender,
        vector<string> friends_platid, bool isYellowDmd,
        bool isYellowDmdYear, int lvlYellowDmd, LoadStatus& state,
		int region,int nCity,bool bIsNewUser,string strVIA,bool isHighYellowDmd,
		string strChannel,bool isHighDmdYear, int nBlueTime ,  int nBlueYearTime,  int nHighBlueTime,  int nHighBlueYearTime,
		int nID, int nName, int nType, string strSku)
{
	GameDataHandler* dh = eh_->getDataHandler();
	User *pUser = dh->getUser(uid, &state, true);
	if(pUser == NULL)
	{
		if(state == LOAD_EMPTY)
		{
			pUser = dh->createUser(uid,platid, name, profile_link, gender, (PLAT_TYPE)siteid,
				isYellowDmd, isYellowDmdYear, lvlYellowDmd, friends_platid,region,nCity,bIsNewUser,strVIA,isHighYellowDmd,eh_,strChannel,isHighDmdYear,nBlueTime, nBlueYearTime, nHighBlueTime, nHighBlueYearTime,
				nID, nName, nType, strSku);
		}
	}
	else
	{
		dh->updateUser(pUser, name, profile_link, gender, (PLAT_TYPE)siteid,
			isYellowDmd, isYellowDmdYear, lvlYellowDmd,	friends_platid,nCity,isHighYellowDmd,eh_,isHighDmdYear,nBlueTime, nBlueYearTime, nHighBlueTime,nHighBlueYearTime);

		dh->markUserDirty(pUser);
	}

	return pUser;
}

//void UserLogin::RereshPlatInfo(string platID,DB_PlatidInfoList* pPlatidInfoList,User *pUser)
//{
//	if(pPlatidInfoList==NULL||pUser==NULL)
//		return;
//	GameDataHandler* dh = eh_->getDataHandler();
//	bool bSave = false;
//	//更新最后一次登录ID
//	int64 nLastUid = pPlatidInfoList->lastuid();
//	if(nLastUid != pUser->GetUid())
//	{
//		pPlatidInfoList->set_lastuid(pUser->GetUid());
//		bSave = true;
//	}
//	DB_User& dbUser = pUser->GetDbUser();
//	if(dbUser.has_giftinfo())
//	{
//		DB_GiftInfo* pDBGiftInfo = dbUser.mutable_giftinfo();
//		if(pDBGiftInfo->msgidx()>0||
//			pDBGiftInfo->daysendgiftcnt()>0||
//			pDBGiftInfo->dayaskfrdidlst_size()>0||
//			pDBGiftInfo->lastlogintime()>0)
//		{
//			DB_GiftInfo* pPlatDBGift = pPlatidInfoList->mutable_giftinfo();
//			pPlatDBGift->CopyFrom(*pDBGiftInfo);
//			bSave = true;
//		}
//		dbUser.clear_giftinfo();
//	}
//	if(!bSave)
//	{
//		dh->savePlatidMap(platID,pPlatidInfoList);	
//	}
//}
//
//int UserLogin::CorrectRegion(DB_PlatidInfoList* pPlatidList,int nRegion)
//{
//	// 不在合区分组中
//	int nGroupid = serverConfig.getgroupidbyregion(nRegion);
//	if (-1 == nGroupid)
//	{
//		return nRegion;
//	}
//	// 检测角色信息
//	if (NULL == pPlatidList)
//	{
//		return nRegion;
//	}
//
//	// 检测nRegion是否在有角色存在
//	int nSize = pPlatidList->roleinfo_size();
//	for (int i=0;i<nSize;i++)
//	{
//		const DB_PlatidInfo& platidinfo = pPlatidList->roleinfo(i);
//		if (platidinfo.srvregion() == nRegion)
//		{
//			return nRegion;
//		}
//	}
//
//	// 检测分组内的其他区是否有角色，有角色就返回该区
//	int ngroupsize = serverConfig.getgroupsize(nGroupid);
//	for (int i=0;i<ngroupsize;i++)
//	{
//		int nsubregion = serverConfig.getregionfromgroup(nGroupid,i);
//		if (-1 != nsubregion && nsubregion != nRegion)
//		{
//			for (int j=0;j<nSize;j++)
//			{
//				const DB_PlatidInfo& platidinfo = pPlatidList->roleinfo(j);
//				if (platidinfo.srvregion() == nsubregion)
//				{
//					return nsubregion;
//				}
//			}
//		}
//	}
//
//	// 所有的区都没有角色，返回主分区,在主分区创建角色
//	return serverConfig.getmapregion(nRegion);
//}
