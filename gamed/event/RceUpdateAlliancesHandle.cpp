#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../logic/User.h"
#include "../../logic/SkuIdTbl.h"
#include "RceUpdateAlliancesHandle.h"


RceUpdateAlliancesHandle::RceUpdateAlliancesHandle()
:CBaseEvent()
{

}
RceUpdateAlliancesHandle::~RceUpdateAlliancesHandle()
{

}

void RceUpdateAlliancesHandle::handle(Event* e)
{
	if(e == NULL)
	{
		return;
	}

	if(e->state() == Status_Normal_Game)
	{
		handle_c2country(e);
	}

	if(e->state() == Status_Country_To_Game)
	{
		handle_country2s(e);
	}
}

void RceUpdateAlliancesHandle::handle_c2country(Event *e)//hansoul
{
	int64 uid = e->uid();
	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(!pUserManager)
	{
		return;
	}
	User *pUser = pUserManager->getUser(uid);
	if ( !pUser)
	{
		return;
	}

	Player* pPlayer = pUser->GetPlayer();
	if ( !pPlayer || !pPlayer->CanUse())
	{
		return ;
	}

	RceUpdateAlliances *req = e->mutable_ce_rceupdatealliances();
	RseUpdateAlliances *rse = e->mutable_se_rseupdatealliances();
	if(!req){
		return;
	}
	
	if(req->action() == "createAlliance")
	{
		MsgTransaction* MT = req->mutable_transaction();
		if(!pPlayer->CostRes(RC_Cash, MT->cash()))
		{
			return;
		}
		Alliance* aln = rse->add_alliance();
		Member* mb = aln->add_member();
		mb->set_id(toString(pUser->GetUid()));
		mb->set_score(0);
		string url = pUser->GetProfileLink(PLAT_QHALL);
		mb->set_pictureurl(url);
		mb->set_currentwarscore(0);
		string n = pUser->GetName(PLAT_QHALL);
		mb->set_name(n);

	}
	else if(req->action() == "getMyAlliance")
	{
		DB_Alliance* pAln = pPlayer->GetDBPlayer()->mutable_alliance();
		if(!pAln)
		{

		}
		else if(req->role() == 3 && req->warscore() > 0)
		{
			DB_Model* pDBM = pPlayer->GetDBPlayer()->mutable_model();
			pDBM->set_score(pDBM->score() + req->warscore());
		}
		else if(req->role() == 3 && req->attackscore() > 0)
		{
			pAln->set_totalwarscore(pAln->totalwarscore() + req->attackscore());
			pAln->set_currentwarscore(pAln->currentwarscore() + req->attackscore());
		}
	}
	else if(req->action() == "joinAlliance")
	{
		MsgAllianceNews* news = rse->add_alliancenews();
		news->set_pictureurl(pUser->GetProfileLink(PLAT_QHALL));
		news->set_name(pUser->GetName(PLAT_QHALL));
		news->set_level(pUser->GetUserLevel());
		rse->set_totalsize(pUser->GetUserScore());//暂时利用发送score
	}
	else if(req->action() == "agreeJoinAlliance")
	{
		req->set_playerid(toString(pUser->GetUid()));
	}
	else if(req->action() == "refuseJoinAlliance")
	{
		req->set_playerid(toString(pUser->GetUid()));
	}
	else if(req->action() == "getAlliances")
	{
		req->set_guid(toString(pUser->GetUid()));
	}
	else
	{

	}
	e->set_state(Status_Game_To_Country);
	eh_->sendEventToCountry(e, pUser->GetUserRegion(true));
}

void RceUpdateAlliancesHandle::handle_country2s(Event *e)
{
	int64 uid = e->uid();

	GameDataHandler* pUserManager = eh_->getDataHandler();
	if(pUserManager==NULL)
	{
		return;
	}
	LoadStatus state = LOAD_INVALID;
	User *pUser = pUserManager->getUser(uid, &state, true);
	if (pUser == NULL)
	{
		if (state == LOAD_WAITING)
		{
			eh_->postBackEvent(e);
		}
		return;
	}
	
	RseUpdateAlliances *pRsp = e->mutable_se_rseupdatealliances();
	RceUpdateAlliances *req = e->mutable_ce_rceupdatealliances();
	if(pRsp->action() == "getReward" && pRsp->ret() == 0)
	{
		string reward = SkuIDTblInst::instance().GetName(req->reward());
		if(!pUser->GetPlayer()->AddSocialItem(reward,1))
		{
			return ;
		}
	}
	if(pRsp->action() == "getMyAlliance")
	{
		if(pRsp->ret() == 0 && pRsp->mutable_alliance(0))
		{
			pUser->GetDBPlayer()->mutable_flag()->set_alliancewelcome(0);
		}
	}

	e->set_state(Status_Game_To_Country);
	string text;
	pRsp->SerializeToString(&text);
	eh_->sendDataToUser(pUser->fd(), S2C_RseUpdateAlliances,text);
}

