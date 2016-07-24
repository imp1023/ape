#include "../CountryEventHandler.h"
#include "../CountryDataHandler.h"
#include "DealArenaEvent.h"
#include "../CountryRgnDataHandler.h"
#include "../../event/event.pb.h"
#include "../../common/json/json.h"
#include "../../common/string-util.h"
#include "../../common/const_def.h"
#include "../../event/EventDefine.h"

#include <math.h>
#include "../../event/EventQueue.h"
#include "../../common/SysLog.h"
#include "../CountryConfigMgr.h"
#include "../logic/CountryInfoCfg.h"
#include "../CountryDefine.h"
#include "../logic/HeroArenaActive.h"

extern int G_CountrySrvD_ID; 

//const int STAR_STAT_TIME = 60*1000;

DealArenaEvent::DealArenaEvent()
:CBaseEvent()
{
	//debug_idx = -1;
}
DealArenaEvent::~DealArenaEvent()
{

}

void DealArenaEvent::registHandler()
{
	eh_->getEventHandler()->registHandler(EVENT_GF_FIGHT_REQUEST_ARENA, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_GF_FIGHT_REQUEST_WORLDARENA, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(EVENT_GF_FIGHT_REQUEST_HEROARENA, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceArenaRankInfo, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceArenaOpt, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceWorldArenaRankInfo, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceWorldArenaOpt, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceHeroArenaRankInfo, ((ProcessRoutine) DealArenaEvent::handle_));
	eh_->getEventHandler()->registHandler(C2S_RceHeroArenaOpt, ((ProcessRoutine) DealArenaEvent::handle_));
}

void DealArenaEvent::handle(Event* e)
{	
	GCG_CountryNeed* pNeedArgs = e->mutable_countryneed();
	int nGameID = pNeedArgs->gamesrvid();
	int nRegion = pNeedArgs->region();

	CountryDataHandler* pDH= eh_->getDataHandler();
	if(!pDH->IsRightRegion(nRegion))
	{
		return;//分区错误
	}
	
	CountryRgnDatHandler* pRDH = pDH->GetRegionDatHandler(nRegion);
	if(pRDH==NULL)
	{
		return;
	}

	switch(e->cmd())
	{
	case EVENT_GF_FIGHT_REQUEST_ARENA:
		{
			HandleArenaFight(e,pRDH,nGameID);
		}
		break;
	case C2S_RceArenaRankInfo:
		{
			HandleArenaRank(e,pRDH,nGameID);
		}
		break;
	case C2S_RceArenaOpt:
		{
			HandleArenaOpt(e,pRDH,nGameID);
		}
		break;
	case C2S_RceWorldArenaRankInfo:
		{
			HandleWorldArenaRank(e,pRDH,nGameID);
		}
		break;
	case C2S_RceWorldArenaOpt:
		{
			HandleWorldArenaOpt(e,pRDH,nGameID);
		}
		break;
	case EVENT_GF_FIGHT_REQUEST_WORLDARENA:
		{
			HandleWorldArenaFight(e,pRDH,nGameID);
		}
		break;
	case C2S_RceHeroArenaRankInfo:
		{
			HandleHeroArenaRank(e,pRDH,nGameID);
		}
		break;
	case C2S_RceHeroArenaOpt:
		{
			HandleHeroArenaOpt(e,pRDH,nGameID);
		}
		break;
	case EVENT_GF_FIGHT_REQUEST_HEROARENA:
		{
			HandleHeroArenaFight(e,pRDH,nGameID);
		}
		break;
	default:
		break;
	}
}

void DealArenaEvent::HandleArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}

	//上来就是设置位置的

	RseArenaOpt* pOpt = e->mutable_se_rsearenaopt();
	Arena_FightBackData* pArenaFight = e->mutable_arenafightbackdata();
	
	int nRelsult = pOpt->result();
	if(nRelsult ==  ARENA_OPT_REL_ATKSELFWIN || nRelsult == ARENA_OPT_REL_ATKBYENEMYWIN)
	{
		//设置位置
		int nCountryId = pOpt->countryidself();
		if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == false)
		{
			nRelsult = ARENA_OPT_REL_ERRNOCOUNTRYID;
		}
		else
		{
			CCountry* pCountry = pRDH->GetCountry(nCountryId);
			if(pCountry == NULL)
			{
				nRelsult = ARENA_OPT_REL_ERRNOCOUNTRYID;
			}
			else
			{
				if(pArenaFight->nposself() > pArenaFight->nposenemy())		//只能是由低打高
				{
					if(pCountry->ExChangePos(pRDH,pArenaFight->uidself(),pArenaFight->nposself(),pArenaFight->uidenemy(),pArenaFight->nposenemy()) == false)
					{
						nRelsult = ARENA_OPT_REL_ATKSETPOSERR;
					}
					else
					{
						pRDH->markUserLstDirty(pArenaFight->uidself());
						if(pArenaFight->uidenemy() >= 1000000000)
						{
							pRDH->markUserLstDirty(pArenaFight->uidenemy());
						}
						SYS_LOG(0,LT_ARENA_COUNTRY_RANKEXCHANGE,pRDH->GetRegion(),0,(int64)pArenaFight->uidself()<<pArenaFight->nposenemy()<<(int64)pArenaFight->uidenemy()<<pArenaFight->nposself());
					}				
				}
			}			
		}
	}

	pOpt->set_result(nRelsult);

	int64 uidEnemy = pArenaFight->uidenemy();

	if(nRelsult != ARENA_OPT_REL_ATKSELFWIN &&
		nRelsult != ARENA_OPT_REL_ATKSELFFAIL &&
		nRelsult != ARENA_OPT_REL_ATKBYENEMYWIN &&
		nRelsult != ARENA_OPT_REL_ATKBYENEMYFAIL)
	{
		//失败，清除数据包信息
		pArenaFight->Clear();
	}

	//发送给自己和敌人
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);

	//发送结果，对方服务器
	if(uidEnemy >= 1000000000)
	{
		e->set_uid(uidEnemy);
		pArenaFight->clear_lastfightrecord();

		e->set_state(Status_Country_To_Game);
		eh_->sendEventToGamed(e,pOpt->fightgameid());
	}
}

void DealArenaEvent::HandleArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	//校验竞技场数据清理
	pRDH->CheckClearCountryArenaData();

	//获取排行榜的数据

	RceArenaRankInfo* pOpt = e->mutable_ce_rcearenarankinfo();
	RseArenaRankInfo* pRankInfo = e->mutable_se_rsearenarankinfo();

	int nOptType = pOpt->type();

	pRankInfo->set_type(nOptType);

	//校验玩家的竞技场数据，信任国家信息
	//能看到竞技场，必须是加入国家了
	int nCountryId = pOpt->ncountry();
	if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == false)
	{
		return;
	}

	CCountry* pCountry = pRDH->GetCountry(nCountryId);
	if(pCountry == NULL)
	{
		return;					
	}

	int nRankIndex = pOpt->nindex();
	nRankIndex = pCountry->CheckPlayerRankIndex(pRDH,nRankIndex,e->uid());
	pOpt->set_nindex(nRankIndex);

	pRankInfo->set_ntotalnum(pRDH->GetArenaPlayerNum());

	switch(nOptType)
	{
	case E_FILL_ARENA_TYPE_ASKRANK:
		{
			//获取排行榜数据
			for(int i=C_Start;i<C_UserCoutryEnd;i++)
			{
				CCountry* pCountrySec = pRDH->GetCountry(i);
				if(pCountrySec)
				{
					if(pCountrySec->GetRankInfo(pRankInfo) == true)
					{
						pRDH->markCountryDirty(pCountrySec);
					}
				}
			}
		}
		break;
	case E_FILL_ARENA_TYPE_ASKATK:
		{
			//获取可战斗玩家数据
			if(pCountry->GetRankFightInfo(nRankIndex,pRankInfo) == true)
			{
				pRDH->markCountryDirty(pCountry);
			}
		}
		break;
	}

	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);

}

void DealArenaEvent::HandleArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}


	RceArenaOpt* pOpt = e->mutable_ce_rcearenaopt();
	RseArenaOpt* pOptBack = e->mutable_se_rsearenaopt();

	pOptBack->set_indexself(-1);

	int nOptType = pOpt->type();

	switch(nOptType)
	{
	case E_ARENA_OPT_ADDARENA:
		{
			int nCountryId = pOpt->countryidself();
			if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
			{
				CCountry* pCountry = pRDH->GetCountry(nCountryId);
				if(pCountry)
				{
					if(pCountry->UpdateRank() == true)
					{
						pRDH->markCountryDirty(pCountry);
					}

					DB_C_User* pUser = pRDH->GetDBCityUser(e->uid());
					if(pUser && pUser->countryid() == nCountryId)
					{
						pOptBack->set_indexself(pCountry->AddNewPlayer(pUser));	
						pRDH->markUserLstDirty(pUser->userid());
					}
				}
			}
		}
		break;
	case E_ARENA_OPT_UPDATASCORE:
		{
			int nArenaIndex = pOpt->narenaindex();
			int nArenaScore = pOpt->narenascore();
			int nArenaScoreAdd = pOpt->narenascoreadd();
			int nCountryId = pOpt->countryidself();

			if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
			{
				CCountry* pCountry = pRDH->GetCountry(nCountryId);
				if(pCountry)
				{
					if(pCountry->UpdateScoreGiftIndex(e->uid(),nArenaIndex,nArenaScore,nArenaScoreAdd) == true)
					{
						pRDH->markCountryDirty(pCountry);
					}
				}
			}
		}
		break;
	case E_ARENA_OPT_GAINSCORE:
		{
			int nCountryId = pOpt->countryidself();

			if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == true)
			{
				CCountry* pCountry = pRDH->GetCountry(nCountryId);
				if(pCountry)
				{
					pOpt->set_ngainscoregift(pCountry->GainScoreGiftIndex(e->uid()));
				}
			}
		}
		break; 
	}
	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);

}


void DealArenaEvent::HandleWorldArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}

    //世界竞技场数据更新
	pRDH->UpdateWorldArenaData();

	//获取世界竞技场排行榜的数据
	RceWorldArenaRankInfo* pOpt = e->mutable_ce_rceworldarenarankinfo();
	RseWorldArenaRankInfo* pRankInfo = e->mutable_se_rseworldarenarankinfo();

	int nOptType = pOpt->type();
	pRankInfo->set_type(nOptType);

	int nRankIndex = pEarth->CheckPlayerWorldRankIndex(pRDH,e->uid());
	pOpt->set_nindex(nRankIndex);
	switch(nOptType)
	{
	case E_WORLD_ARENARANKINFO_ASKRANK:
		{
			//获取昨日排行榜数据
            pEarth->GetWorldRankInfo(pRDH,pRankInfo);
		}
		break;
	case E_WORLD_ARENARANKINFO_ASKATK:
		{
			//获取可战斗玩家数据
			pEarth->GetWorldRankFightInfo(pRDH,nRankIndex,pRankInfo);
		}
		break;
	case E_WORLD_ARENARANKINFO_ALLRANK:
		{
			//获取每期排行版
			int nActiveId = pOpt->nactiveid();
            pEarth->GetWorldALLRankInfo(pRDH,pRankInfo,nActiveId);
		}
		break;
	case E_WORLD_ARENARANKINFO_LASTRANK:
		{
			//获取上期排行版
			pEarth->GetWorldLastRankInfo(pRDH,pRankInfo);
		}
		break;
	}

	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);

}

void DealArenaEvent::HandleWorldArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}

	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	//国家竞技场数据更新
	pRDH->CheckClearCountryArenaData();

	RceWorldArenaOpt* pOpt = e->mutable_ce_rceworldarenaopt();
	RseWorldArenaOpt* pOptBack = e->mutable_se_rseworldarenaopt();

	pOptBack->set_indexself(-1);

	int nOptType = pOpt->type();
	int nRel = E_WORLD_ARENA_OPT_REL_OK;
	switch(nOptType)
	{
	case E_WORLD_ARENA_OPT_ADDARENA:
		{
			DB_C_User* pUser = pRDH->GetDBCityUser(e->uid());
			if(pUser)
			{
				nRel = pEarth->CheckCanAddWorldArena(pUser);
				if(nRel == E_WORLD_ARENA_OPT_REL_OK)
				{
					pOptBack->set_indexself(pEarth->AddWorldNewPlayer(pUser));
					pRDH->markUserLstDirty(pUser->userid());
				}
			}
		}
		break;
	}
	//发送
	pOptBack->set_result(nRel);
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}


void DealArenaEvent::HandleWorldArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	//上来就是设置位置的

	RseWorldArenaOpt* pOpt = e->mutable_se_rseworldarenaopt();
	Arena_FightBackData* pArenaFight = e->mutable_arenafightbackdata();

	int nRelsult = pOpt->result();
	//检查位置
	if(nRelsult == E_WORLD_ARENA_OPT_REL_ATKSELFWIN ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKSELFFAIL ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYFAIL)
	{
		if(false==pEarth->CheckWorldArenaPos(pRDH,pArenaFight->uidself(),pArenaFight->nposself(),pArenaFight->uidenemy(),pArenaFight->nposenemy()))
		{
			nRelsult = E_WORLD_ARENA_OPT_REL_ATKSETPOSERR;
		}
		else
		{
			//战斗胜利
			if(nRelsult ==  E_WORLD_ARENA_OPT_REL_ATKSELFWIN || nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN)
			{
				//设置位置
				int nCountryId = pOpt->countryidself();
				if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == false)
				{
					nRelsult = E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYID;
				}
				else
				{
					CCountry* pCountry = pRDH->GetCountry(nCountryId);
					if(pCountry == NULL)
					{
						nRelsult = E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYID;
					}
					else
					{
						if(pArenaFight->nposself()>=0&&pArenaFight->nposenemy()>=0&&pArenaFight->nposself() > pArenaFight->nposenemy())		//只能是由低打高
						{
							if(pEarth->ChangeWorldArenaPos(pRDH,pArenaFight->uidself(),pArenaFight->nposself(),pArenaFight->uidenemy(),pArenaFight->nposenemy()) == false)
							{
								nRelsult = E_WORLD_ARENA_OPT_REL_ATKSETPOSERR;
							}
							else
							{
								pRDH->markUserLstDirty(pArenaFight->uidself());
								if(pArenaFight->uidenemy() >= 1000000000)
								{
									pRDH->markUserLstDirty(pArenaFight->uidenemy());
								}
								SYS_LOG(0,LT_ARENA_WORLD_RANKEXCHANGE,pRDH->GetRegion(),0,(int64)pArenaFight->uidself()<<pArenaFight->nposenemy()<<(int64)pArenaFight->uidenemy()<<pArenaFight->nposself());
							}				
						}
					}			
				}
			}
		}
	}
	pOpt->set_result(nRelsult);
	int64 uidEnemy = pArenaFight->uidenemy();
	if(nRelsult != E_WORLD_ARENA_OPT_REL_ATKSELFWIN &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKSELFFAIL &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKBYENEMYFAIL)
	{
		//清除数据包信息
		pArenaFight->Clear();
	}
	//发送给自己和敌人
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
	//发送结果，对方服务器
	if(uidEnemy >= 1000000000)
	{
		e->set_uid(uidEnemy);
		pArenaFight->clear_lastfightrecord();

		e->set_state(Status_Country_To_Game);
		eh_->sendEventToGamed(e,pOpt->fightgameid());
	}
}


void DealArenaEvent::HandleHeroArenaRank(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}

	//英雄竞技场数据更新
	pRDH->UpdateHeroArenaData();

	//获取世界竞技场排行榜的数据
	RceHeroArenaRankInfo* pOpt = e->mutable_ce_rceheroarenarankinfo();
	RseHeroArenaRankInfo* pRankInfo = e->mutable_se_rseheroarenarankinfo();

	int nOptType = pOpt->type();
	pRankInfo->set_type(nOptType);

	int nRankIndex = pEarth->CheckPlayerHeroRankIndex(pRDH,e->uid());
	pOpt->set_nindex(nRankIndex);
	bool bLastday = HeroArenaActive::Instance().IsInLastDays();
	/*if(nRankIndex == -1 && bLastday)
	{
		pRankInfo->set_type(E_HERO_ARENARANKINFO_ASKATK);
		pEarth->GetLastdayHeroRankFightInfo(pRankInfo);
		e->set_state(Status_Country_To_Game);
		eh_->sendEventToGamed(e,nGameID);
		return;
	}*/
	switch(nOptType)
	{
	case E_HERO_ARENARANKINFO_ASKRANK:
		{
			//获取昨日排行榜数据
			pEarth->GetHeroRankInfo(pRDH,pRankInfo);
		}
		break;
	case E_HERO_ARENARANKINFO_ASKATK:
		{
			//获取可战斗玩家数据
			//pEarth->GetHeroRankFightInfo(pRDH,nRankIndex,pRankInfo);
			if (nRankIndex == -1 && bLastday)
			{
				pEarth->GetLastdayHeroRankFightInfo(pRankInfo);
			} 
			else
			{
				pEarth->GetHeroRankFightInfo(pRDH,nRankIndex,pRankInfo);
			}
		}
		break;
	}

	//发送
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);

}

void DealArenaEvent::HandleHeroArenaOpt(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}

	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}

	RceHeroArenaOpt* pOpt = e->mutable_ce_rceheroarenaopt();
	RseHeroArenaOpt* pOptBack = e->mutable_se_rseheroarenaopt();

	pOptBack->set_indexself(-1);

	int nOptType = pOpt->type();
	int nRel = E_WORLD_ARENA_OPT_REL_OK;
	switch(nOptType)
	{
	case E_HERO_ARENA_OPT_ADDARENA:
		{
			DB_C_User* pUser = pRDH->GetDBCityUser(e->uid());
			if(pUser)
			{
				pOptBack->set_indexself(pEarth->AddHeroNewPlayer(pUser));
				pRDH->markUserLstDirty(pUser->userid());
			}
		}
		break;
	}
	//发送
	pOptBack->set_result(nRel);
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
}


void DealArenaEvent::HandleHeroArenaFight(Event* e,CountryRgnDatHandler* pRDH,int nGameID)
{
	if(e == NULL || e->state() != Status_Game_To_Country || pRDH == NULL)
	{
		return;
	}
	CCountry* pEarth = pRDH->GetEarthCountry();
	if(pEarth == NULL)
	{
		return;					
	}
	//上来就是设置位置的

	RseHeroArenaOpt* pOpt = e->mutable_se_rseheroarenaopt();
	Arena_FightBackData* pArenaFight = e->mutable_arenafightbackdata();

	int nRelsult = pOpt->result();
	//检查位置
	if(nRelsult == E_WORLD_ARENA_OPT_REL_ATKSELFWIN ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKSELFFAIL ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN ||
		nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYFAIL)
	{
		if(false==pEarth->CheckHeroArenaPos(pRDH,pArenaFight->uidself(),pArenaFight->nposself(),pArenaFight->uidenemy(),pArenaFight->nposenemy()))
		{
			nRelsult = E_WORLD_ARENA_OPT_REL_ATKSETPOSERR;
		}
		else
		{
			//战斗胜利
			if(nRelsult ==  E_WORLD_ARENA_OPT_REL_ATKSELFWIN || nRelsult == E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN)
			{
				//设置位置
				int nCountryId = pOpt->countryidself();
				if(CountryInfoCfg::Instance().IsUserCountry(nCountryId) == false)
				{
					nRelsult = E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYID;
				}
				else
				{
					CCountry* pCountry = pRDH->GetCountry(nCountryId);
					if(pCountry == NULL)
					{
						nRelsult = E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYID;
					}
					else
					{
						if(pArenaFight->nposself()>=0&&pArenaFight->nposenemy()>=0&&pArenaFight->nposself() > pArenaFight->nposenemy())		//只能是由低打高
						{
							if(pEarth->ChangeHeroArenaPos(pRDH,pArenaFight->uidself(),pArenaFight->nposself(),pArenaFight->uidenemy(),pArenaFight->nposenemy()) == false)
							{
								nRelsult = E_WORLD_ARENA_OPT_REL_ATKSETPOSERR;
							}
							else
							{
								pRDH->markUserLstDirty(pArenaFight->uidself());
								if(pArenaFight->uidenemy() >= 1000000000)
								{
									pRDH->markUserLstDirty(pArenaFight->uidenemy());
								}
								SYS_LOG(0,LT_ARENA_HERO_RANKEXCHANGE,pRDH->GetRegion(),0,(int64)pArenaFight->uidself()<<pArenaFight->nposenemy()<<(int64)pArenaFight->uidenemy()<<pArenaFight->nposself());
							}				
						}
					}			
				}
			}
		}
	}
	pOpt->set_result(nRelsult);
	int64 uidEnemy = pArenaFight->uidenemy();
	if(nRelsult != E_WORLD_ARENA_OPT_REL_ATKSELFWIN &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKSELFFAIL &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN &&
		nRelsult != E_WORLD_ARENA_OPT_REL_ATKBYENEMYFAIL)
	{
		//清除数据包信息
		pArenaFight->Clear();
	}
	//发送给自己和敌人
	e->set_state(Status_Country_To_Game);
	eh_->sendEventToGamed(e,nGameID);
	//发送结果，对方服务器
	if(uidEnemy >= 1000000000)
	{
		e->set_uid(uidEnemy);
		pArenaFight->clear_lastfightrecord();

		e->set_state(Status_Country_To_Game);
		eh_->sendEventToGamed(e,pOpt->fightgameid());
	}
}