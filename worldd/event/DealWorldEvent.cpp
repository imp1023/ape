#include "DealWorldEvent.h"
#include "../WorldNetHandler.h"
#include "../WorldDataHandler.h"
#include "../../logic/WorldConfigMgr.h"
#include "../worldd.h"
#include "../../logic/CommonSystemMgr.h"
#include "../../common/string-util.h"
#include "RseBroadcast.pb.h"
#include <vector>
#include <string>

DealWorldEvent::DealWorldEvent()
: CBaseEvent()
{

}

DealWorldEvent::~DealWorldEvent()
{

}

/**
* 注册转发动作
*/
void DealWorldEvent::regist()
{
    for (int event_id = 0; event_id <= EventHandler::maxProcessRoutines; event_id++)
    {
        eh_->getEventHandler()->registHandler(event_id, ((ProcessRoutine) DealWorldEvent::handle_));
    }
}

void DealWorldEvent::handle(Event* e)
{
	LOG4CXX_DEBUG(logger_, "Handle message: " << e->cmd());
	switch (e->cmd())
	{
	case EVENT_GM:
	   {
		   string gmstr = e->content();
		   vector<string> gmtoken;
		   static const string delim = ",";
		   tokenize(gmstr, gmtoken, delim);
		   if (gmtoken.size() == 0)
			   return;
		   if (e->forward().serveridto() == WORLD_SPECIAL){
				//handleWorldGM(e);
		   }else{
			   processEventForward(e);
		   }
	   }
	   break;
/*	case EVENT_TICKCENTERCAMPBATTLE:
	   {
 		   if (GlobalCampBattleLevelTblInst::instance().IsCenterServer(World::Instance().centerID()))
 			   CenterCampBattleModuleInst::instance().SaveAll();
	   }
	   break;
	case C2S_DceEnterCenterCampBattle:
	   {
		   if (e->state() != Game_to_World)
			   return;
		   processEnterCenterCampBattle(e);
	   }
	   break;
	case C2S_DceCenterCampBattleData:
	   {
		   if (e->state() != Game_to_World)
			   return;
		   processCenterCampBattleData(e);
	   }
	   break;
	case C2S_DceCenterCampBattleMove:
	   {
		   if (e->state() != Game_to_World)
			   return;
		   processCenterCampBattleMove(e);
	   }
	   break;
	case C2S_DceCenterCampBattleResult:
	   {
		   if (e->state() != Game_to_World)
			   return;
		   processCenterCampBattleResult(e);
	   }
	   break;
	case C2S_DceClearCenterCampBattleCD:
	   {
		   if (e->state() != Game_to_World)
			   return;
		   processClearCenterCampBattleCD(e);
	   }
	   break;
	case C2S_DceCenterCampBattleRankList:
		{
			if (e->state() != Game_to_World)
				return;
			processCenterCampBattleRankList(e);
		}
		break;
	case C2S_DceCenterCampBattleCampRank:
		{
			if (e->state() != Game_to_World)
				return;
			processCenterCampBattleCampRank(e);
		}
		break;
	case C2S_DceCenterCampBattleBuy:
		{
			if (e->state() != Game_to_World)
				return;
			processCenterCampBattleBuy(e);
		}
		break;
	case EVENT_WORLD_CENTERCAMPBATTLE_AWARD:
		{
			if (e->state() != World_to_Game)
				return;
			processCenterCampBattleAward(e);
		}
		break;
    case C2S_DceLuckyGoddessData:
        {
            if (e->state() != Game_to_World)
            {
                return;
            }
            LuckyGodHandleInst::instance().HandleLuckyGodData(e);
            eh_->sendEventToUser(e,e->uid());
        }
        break;
    case C2S_DceLuckyGoddnessForCharge:
        {
            if (e->state() != Game_to_World)
            {
                return;
            }
            LuckyGodHandleInst::instance().HandleLuckyGoddnessForCharge(e,eh_);
        }
        break;
    case C2S_DceChallengeDataRequest:
        {
            ChallengeWorldModuleInst::instance().ProessDataRequest(e);
        }
        break;
    case C2S_DceChallengeSignUp:
        {
            ChallengeWorldModuleInst::instance().ProessSignRequest(e);
        }
        break;
    case C2S_DceChallengeFight:
        {
            ChallengeWorldModuleInst::instance().ProessFightRequest(e);
        }
        break;
    case C2S_DceChallengeAction:
        {
            ChallengeWorldModuleInst::instance().ProessActionRequest(e);
        }
        break;
    case C2S_DceChallengeEnemyData:
        {
            ChallengeWorldModuleInst::instance().ProessEnemyData(e);
        }
        break;
    case C2S_DceChallengeRevenge:
        {
            ChallengeWorldModuleInst::instance().ProessRevenge(e);
        }
        break;
    case C2S_DceChallengeRank:
        {
            ChallengeWorldModuleInst::instance().ProessRankList(e);
        }
        break;
    case EVENT_TICK_WORLD_GLOBAL_DATA:
        {
            GlobalDbData::instance().SaveAll();
        }
        break;
    case C2S_DceChallengeRefresh:
        {
            ChallengeWorldModuleInst::instance().ProessRefresh(e);
        }
        break;
    case C2S_DceChallengeRefreshCharge:
        {
            ChallengeWorldModuleInst::instance().ProessRefreshCharge(e);
        }
        break;
    case C2S_DceChallengeChampion:
        {
            ChallengeWorldModuleInst::instance().ProessChampion(e);
        }
        break;
    case EVENT_CHALLENGE_SYNC_TACTIC_INFO:
        {
            ChallengeWorldModuleInst::instance().ProessActionResult(e);
        }
        break;
    case EVNET_CHALLENGE_REVENGE:
        {
            ChallengeWorldModuleInst::instance().ProessRevengeResult(e);
        }
        break;
    case C2S_DceChallengeBuy:
        {
            ChallengeWorldModuleInst::instance().ProessBuyChance(e);
        }
        break;
    case EVENT_CHALLENGE_LOGIN:
        {
            ChallengeWorldModuleInst::instance().ProessLogin(e);
        }
        break;
	case C2S_DceSalute:
		{
			if (e->state() != Game_to_World)
				return;
			processSalute(e);
		}
		break;
	case C2S_DceSaluteData:
		{
			if(e->state()!= Game_to_World)
				return;
			processSaluteData(e);
		}
		break;
	case EVENT_TICKGLOBALARENA:
		{
			if (globalArenaServerTblInst::instance().IsCenterServer(World::Instance().centerID()))
				GlobalArenaModuleInst::instance().SaveAll();
		}
		break;
	case EVENT_TICKALLGLOBALARENA:
		{
			AllGlobalArenaModuleInst::instance().SaveAll();
		}
		break;
	case C2S_DceGlobalArenaData:
		{
			if (e->state() != Game_to_World)
				return;
			const DseGlobalArenaData& data = e->dse_dseglobalarenadata();
			if (data.type() == 1)
				GlobalArenaModuleInst::instance().reqGlobalArenaData(e);
			else
				AllGlobalArenaModuleInst::instance().reqGlobalArenaData(e);
			
		}
		break;
	case C2S_DceSignUpAudition:
		{
			if (e->state() != Game_to_World)
				return;
			GlobalArenaModuleInst::instance().signUp(e);
		}
		break;
	case C2S_DceGlobalArenaPlayOffList:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaPlayOffList& req = e->dce_dceglobalarenaplayofflist();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqTop32List(e);
			else if (req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqTop32List(e);
		}
		break;
	case C2S_DceGlobalArenaSelfInfo:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaSelfInfo& req = e->dce_dceglobalarenaselfinfo();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqSelfInfo(e);
			else if (req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqSelfInfo(e);		
		}
		break;
	case C2S_DceGlobalArenaBattleProgress:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaBattleProgress& req = e->dce_dceglobalarenabattleprogress();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqBattleProgress(e);
			else if (req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqBattleProgress(e);
		}
		break;
	case C2S_DceGlobalArenaBattle:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaBattle& req = e->dce_dceglobalarenabattle();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqBattle(e);
			else if(req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqBattle(e);
		}
		break;
	case C2S_DceGlobalArenaUpdateTactic:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaUpdateTactic& req = e->dce_dceglobalarenaupdatetactic();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqUpdateTactic(e);
			else if(req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqUpdateTactic(e);
		}
		break;
	case C2S_DceGlobalArenaEncourage:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaEncourage& req = e->dce_dceglobalarenaencourage();
			if (req.type() == 1)
				GlobalArenaModuleInst::instance().reqEncourage(e);
			else if(req.type() == 2)
				AllGlobalArenaModuleInst::instance().reqEncourage(e);
		}
		break;
	case C2S_DceGlobalArenaChamption:
		{
			if (e->state() != Game_to_World)
				return;
			AllGlobalArenaModuleInst::instance().reqChamption(e);
		}
		break;
	case C2S_DceGlobalArenaBack:
		{
			GlobalArenaModuleInst::instance().reqBack(e);
		}
		break;
	case C2S_DceGlobalArenaCheckEncourage:
		{
			if (e->state() != Game_to_World)
				return;
			const DceGlobalArenaCheckEncourage& req = e->dce_dceglobalarenacheckencourage();
			if (req.step() == 1)
				GlobalArenaModuleInst::instance().reqCheckEncourage(e);
			else if(req.step() == 2)
				AllGlobalArenaModuleInst::instance().reqCheckEncourage(e);
		}
		break;
	case EVENT_SYNGLOBALARENATOALLARENA:
		{
			if (e->state() == World_to_Game)
				processCenterCampBattleAward(e);
			else if (e->state() == Game_to_World)
			{
				AllGlobalArenaModuleInst::instance().signUp(e);
			}
		}
		break;
	case EVENT_GETALLALOBALCHAMPTIONAWARD:
		{
			if (e->state() != Game_to_World)
				return;
			AllGlobalArenaModuleInst::instance().reqAward(e);
		}
		break;
	case C2S_DceLuckyGodBuyItem:
		{
			if(e->state()!=Game_to_World)
				return;
			DceLuckyGodBuyItem * data = e->mutable_dce_dceluckygodbuyitem();
			if (data->id() == 1 && data->mode() == 2)
			{

				GlobalDbData::instance().AddLuckyPool(LuckyGodHandleInst::instance().getpara().at(5));
			}
			if (data->id() == 2 && data->mode() == 2)
			{

				GlobalDbData::instance().AddLuckyPool(LuckyGodHandleInst::instance().getpara().at(6));
			}
		}
		break;
	//case EVENT_TICK_WORLD_STAGE_RANK:
	//   {
	//		if (e->state() != World_to_Self)
	//			return;
	//	   StageTowerWorldModuleInst::instance().saveAll();
	//   }
	//   break;
	//case EVENT_SYN_TACTICINFO_STAGE_RANK:
	//   {
	//		if (e->state() != Game_to_World)
	//			return;
	//	   StageTowerWorldModuleInst::instance().ProcessSynStageRank(e);
	//   }
	//   break;
	//case EVENT_GET_ENEMYLIST_FROM_STAGE_RANK:
	//   {
	//		if (e->state() != Game_to_World)
	//			return;
	//	   StageTowerWorldModuleInst::instance().ProcessGetEnemyList(e);
	//   }
	//   break;
	case EVENT_ADD_ALLGLOBALARENACHAMPTION:
		{
			if(e->state()!=Game_to_World)
				return;
			AllGlobalArenaAddChamption * data = e->mutable_addchamption();
			if (data != NULL)
				AllGlobalArenaModuleInst::instance().addChamptionRecordByGM(data->uid(), data->head(), data->region(), data->name());
		}
		break;
	case EVENT_TUANGOU_JOIN_REQUEST:
		{
			CommonSystemMgrInst::instance().JoinTuanGouRequest(e);
		}
		break;
	case EVENT_TUANGOU_JOIN_INFOMAT:
		{
			CommonSystemMgrInst::instance().JoinTuanGouInformation(e);
		}
		break;
	case EVENT_TUANGOU_NUMBER_REQUEST:
		{
			CommonSystemMgrInst::instance().JoinTuanGouNumber(e);
		}
		break;
	case EVENT_TUANGOU_CLEAR_VERSION:
		{
			CommonSystemMgrInst::instance().ResetTuanGouData(e);
		}
		break;
	case EVENT_SAVE_WORLD_TUANGOUMODUL_DATA:
		{
			CommonSystemMgrInst::instance().SaveAll();
		}
		break;
	case EVENT_TICKGLOBALGUILDBATTLE:
		{
			GlobalGuildBattleModuleInst::instance().SaveAll();
		}
		break;
	case EVENT_GLOBALGUILDBATTLEGAME:
		{
			GlobalGuildBattleModuleInst::instance().reqGameHandler(e);
		}
		break;
	case C2S_DceGlobalGuildBattleSign:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().signUp(e);
		}
		break;
	case C2S_DceGlobalGuildBattleUpdate:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqUpdateTactic(e);
		}
		break;
	case C2S_DceGlobalGuildBattleEncourage:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqEncourage(e);
		}
		break;
	case C2S_DceGlobalGuildBattleData:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqGlobalGuildData(e);
		}
		break;
	case C2S_DceGlobalGuildBattleChamptionList:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqChamption(e);
		}
		break;
	case C2S_DceGlobalGuildBattleChamptionMember:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqChamptionMember(e);
		}
		break;
	case C2S_DceGlobalGuildBattleGuildVs:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqGuildVs(e);
		}
		break;
	case C2S_DceGlobalGuildBattleGuildGroupVs:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqGuildGroupVs(e);
		}
		break;
	case C2S_DceGlobalGuildBattleVsBattle:
		{	
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqVsBattleInfo(e);
		}
		break;
	case EVENT_GETGLOBALGUILDCHAMPTIONAWARD:
		{
			if (e->state() != Game_to_World)
				return;
			GlobalGuildBattleModuleInst::instance().reqChamptionAward(e);
		}
		break;*/
	default:
	   processEventForward(e);
	   break;
	}
}

//无脑转发
void DealWorldEvent::processEventForward(Event* e)
{
	Forward* forward = e->mutable_forward();
	if (forward == NULL)
		return;

	switch (e->state())
	{
	case Game_to_Game:
	case World_to_Game:
	case Web_To_Game:
		{
			if (forward->has_serveridto())
			{
				int gid = forward->serveridto();
				if (gid == MAX_NUMBER)
				{
					eh_->sendEventToAllGamed(e);
				}
				else
				{
					eh_->sendEventToGamed(e, forward->serveridto());
				}
			}
			else if (forward->has_serverfdto())
			{
				eh_->sendEventToGamedWithFd(e, forward->serverfdto());
			}
			else
			{
				LOG4CXX_ERROR(logger_, "Cannot Forward Event:" << e->DebugString());
			}
		}
		break;
	case Game_to_Web:
	case World_to_Web:
		{
			if (forward->has_serverfdto() && e->has_content())
				eh_->sendFdString(forward->serverfdto(), e->content());
			else
				LOG4CXX_ERROR(logger_, "Cannot Forward Event:" << e->DebugString());
		}
		break;
	default:
		{
			LOG4CXX_ERROR(logger_, "Cannot Forward Event:" << e->DebugString());
		}
		break;
	}
}
/*
void DealWorldEvent::processEnterCenterCampBattle( Event* e )
{
	e->set_state(World_to_Game);
	const DceEnterCenterCampBattle& data = e->dce_dceentercentercampbattle();
	if (CenterCampBattleModuleInst::instance().IsEnable())
	{
		bool bFirst = false;
		if (!CenterCampBattleModuleInst::instance().GetPlayer(e->uid()))
			bFirst = true;
		CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().UpdatePlayer(data.tactic(), data.regionid());
		if (pPlayer == NULL)
			return;
		if (bFirst)
			e->mutable_dse_dseentercentercampbattle()->set_camp(pPlayer->Camp());
		pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	}
	else
	{
		CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
		if (pPlayer)
			pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
		else
			CenterCampBattleModuleInst::instance().FillRanklistDataToProto(e->mutable_dse_dsecentercampbattledata());
	}	
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleData( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	if (pPlayer == NULL)
		return;
	pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleMove( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	if (pPlayer == NULL)
		return;
	int action = CAMPBATTLE_ACTION_MOVE;
	int res = pPlayer->Move(e->dce_dcecentercampbattlemove(), action, e->mutable_dse_dsebattlestart());
	DseCenterCampBattleMove* rsp = e->mutable_dse_dsecentercampbattlemove();
	rsp->set_res(res);
	rsp->set_actiontype(action);
	if (res == 0)
		pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleResult( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	if (pPlayer == NULL)
		return;
	DseCenterCampBattleResult* rsp = e->mutable_dse_dsecentercampbattleresult();
	rsp->set_attacknum(pPlayer->GetDB()->attacknum());
	rsp->set_defendnum(pPlayer->GetDB()->defendnum());
	rsp->set_killnum(pPlayer->GetDB()->killnum());
	rsp->set_occupynum(pPlayer->GetDB()->occupynum());
	rsp->set_rank(CenterCampBattleModuleInst::instance().GetRank(e->uid(), CAMP_TYPE_NONE));
	pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processClearCenterCampBattleCD( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	if (pPlayer == NULL)
		return;
	if (pPlayer->ClearCD() == 0)
		pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleRankList( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	DseRankList* rsp = e->mutable_dse_dseranklist();
	CenterCampBattleModuleInst::instance().FillTopList(rsp->mutable_data(), CAMP_TYPE_NONE);
	rsp->set_camp(CAMP_TYPE_NONE);
	rsp->set_rank(CenterCampBattleModuleInst::instance().GetRank(e->uid(), CAMP_TYPE_NONE));
	rsp->set_score(pPlayer ? pPlayer->GetScore() : 0);
	rsp->set_type(8);
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleCampRank( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattleModuleInst::instance().FillCampList(e->mutable_dse_dsecentercampbattlecamprank(), CAMP_TYPE_NONE, false);
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleBuy( Event* e )
{
	e->set_state(World_to_Game);
	CenterCampBattlePlayer* pPlayer = CenterCampBattleModuleInst::instance().GetPlayer(e->uid());
	if (pPlayer == NULL)
		return;
	const DceCenterCampBattleBuy& req = e->dce_dcecentercampbattlebuy();
	int res = 0;
	if (req.type() == 1)
		res = pPlayer->BuyDetect();
	else if (req.type() == 2)
		res = pPlayer->BuyBlood();
	if (res == 0)
		pPlayer->FillDataToProto(e->mutable_dse_dsecentercampbattledata());
	DseCenterCampBattleBuy* rsp = e->mutable_dse_dsecentercampbattlebuy();
	rsp->set_res(res);
	rsp->set_type(req.type());
	eh_->sendEventToUser(e, e->uid());
}

void DealWorldEvent::processCenterCampBattleAward( Event* e )
{
	if (!eh_->sendEventToUser(e, e->uid()))
		eh_->getEventHandler()->postBackEvent(e);
}

void DealWorldEvent::processReload( Event* e )
{
	WorldConfigMgr::GetInst()->LoadWorldConfigInfo();
	eh_->sendFdString(e->forward().serverfdfrom(), "reload success");
	LOG4CXX_INFO(logger_, "Response World Reload: " << e->content());
}

void DealWorldEvent::processLuckyGoddnessData(Event* e)
{
    if (e == NULL)
    {
        return;
    }
    
}

void DealWorldEvent::processSalute(Event* e)
{
	//需要广播 保存温拿 名字 type item
	const SaluteWinner& winnerplayer = e->m_salutewinner();
	GlobalDbData::instance().addsalutewinner(winnerplayer);
	
	
	string pname = winnerplayer.name(),dtype = toString(winnerplayer.proptype()),ditem = toString(winnerplayer.propitem());

	vector<string> param;
	param.push_back(pname);
	param.push_back(dtype);
	param.push_back(ditem);

	Event me;
	me.set_cmd(EVENT_WORLD_BROADCAST);
	me.set_state(World_to_Game);
	me.set_time(-1);
	me.set_uid(-1);
	//Forward* forward = e.mutable_forward();
	//forward->set_serveridto(nRegion);
	WorldBroadcast* broadcast = me.mutable_worldbroadcast();
	//broadcast->set_gameid(nRegion);
	DseBroadcast* proto = broadcast->mutable_event();
	proto->set_id(54); //
	for (int i = 0; i < (int)param.size(); i++)
		proto->add_param(param[i]);
	World::Instance().GetWorldEventHandler()->sendEventToAllGamed(&me);

}

void DealWorldEvent::processSaluteData(Event* e) //game来word请求数据了
{
	//int nowversion = e->m_saluteversion();

	GlobalDbData::instance().fillproto(e);//填充数据
	e->set_state(World_to_Game);
	eh_->sendEventToUser(e,e->uid());//发回客户端
}

void DealWorldEvent::handleWorldGM( Event* e )
{
	string gmstr = e->content();
	vector<string> gmtoken;
	static const string delim = ",";
	tokenize(gmstr, gmtoken, delim);
	if (gmtoken.size() == 0)
		return;
	//world reload表
	if (gmtoken.size() >= 3 && gmtoken[0] == "reload")
	{
		processReload(e);
	}
	//全服争霸赛 打印名单
	else if (gmtoken.size() >= 4 && gmtoken[0] == "printGlobalArena")
	{
		int type = 0;
		safe_atoi(gmtoken[2], type);
		if (type == 1)
			GlobalArenaModuleInst::instance().printResult();
		if (type == 2)
			AllGlobalArenaModuleInst::instance().printResult();
		eh_->sendFdString(e->forward().serverfdfrom(), "print success");
	}
	//全服争霸赛清楚32强对阵标识
	else if (gmtoken.size() >= 4 && gmtoken[0] == "clearglobalarenainitflag")
	{
		//clearglobalarenainitflag,w,1,1
		int type = 0;
		safe_atoi(gmtoken[2], type);
		if (type == 1)
			GlobalArenaModuleInst::instance().setisinit(false);
		if (type == 2)
			AllGlobalArenaModuleInst::instance().setisinit(false);
		eh_->sendFdString(e->forward().serverfdfrom(), "set success");
	}
	//纵横四海
	else if (gmtoken.size() >= 4 && gmtoken[0] == "StageWorldRank")
	{
		int type = 0, num = 0;
		safe_atoi(gmtoken[2], type);
		if (type == 1)//获得总人数
		{
			string strNum = toString(StageTowerWorldModuleInst::instance().getRankSize());
			eh_->sendFdString(e->forward().serverfdfrom(), "totalnum:" + strNum);
		}
		else if (type == 2)//获取高中低玩家样品信息
		{
			if(gmtoken.size() >= 4)
			{
				safe_atoi(gmtoken[3], num);
				StageTowerWorldModuleInst::instance().printUserInfo(num);
				eh_->sendFdString(e->forward().serverfdfrom(), "print success");
			}
			else
				eh_->sendFdString(e->forward().serverfdfrom(), "print error");
		}
		else if (type == -2)//清空排名
		{
			StageTowerWorldModuleInst::instance().clearData();
			eh_->sendFdString(e->forward().serverfdfrom(), "clear success");
		}
	}
	//清楚全服争霸赛发送多服4强到全服的标识
	else if (gmtoken.size() >= 3 && gmtoken[0] == "cleararenasendflag")
	{
		GlobalArenaModuleInst::instance().clearSendTop4Flag();
		eh_->sendFdString(e->forward().serverfdfrom(), "clear flag success!");
		LOG4CXX_INFO(logger_, "Response World Cleararenasendflag: " << e->content());
	}
	else if (!gmtoken.empty() && gmtoken[0] == "savedata")
	{
		World::Instance().GetWorldEventHandler()->getDataHandler()->quit();
		eh_->sendFdString(e->forward().serverfdfrom(), "save world success");
		LOG4CXX_INFO(logger_, "success! save world data");
	}
	//军团pvp打印result
	else if (gmtoken.size() >= 3 && gmtoken[0] == "printGlobalGuildBattle")
	{
		//int type = 0;
		GlobalGuildBattleModuleInst::instance().printResult();
		eh_->sendFdString(e->forward().serverfdfrom(), "print success");
	}
	else if (gmtoken.size() >= 3 && gmtoken[0] == "clearglobalguildaward")
	{
		GlobalGuildBattleModuleInst::instance().clearSendAwardFlag();
		eh_->sendFdString(e->forward().serverfdfrom(), "clear flag success!");
		LOG4CXX_INFO(logger_, "Response World clearglobalguildaward: " << e->content());
	}
	else if (gmtoken.size() >= 3 && gmtoken[0] == "printGlobalGuildData")
	{
		string result = GlobalGuildBattleModuleInst::instance().printData();
		eh_->sendFdString(e->forward().serverfdfrom(), result);
		LOG4CXX_INFO(logger_, "Response World printGlobalGuildData: " << e->content());
	}
}

*/