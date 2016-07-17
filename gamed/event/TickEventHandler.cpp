#include "TickEventHandler.h"
#include "../../event/EventQueue.h"

void TickEventHandler::addEvent_(EVENT_DEFINE eventid)
{
	Event* e = eh_->getEventQueue()->allocateEvent();
	e->set_cmd(eventid);
	e->set_state(Game_to_Self);
	e->set_time(0);
	eh_->getEventQueue()->safePushEvent(e);
}

void TickEventHandler::handle(Event* e)
{
    if (e->state() != Game_to_Self)
        return;

//     if (e->cmd() == EVENT_TICKARENA)
//         handle_arena();
//     else if (e->cmd() == EVENT_TICKQUIZ)
//         handle_quiz();
// 	else if (e->cmd() == EVENT_TICKGUARD)
// 		handle_guard();
// 	else if (e->cmd() == EVENT_TICKPLAYERNAME)
// 		handle_playername();
// 	else if (e->cmd() == EVENT_TICKSTAGE)
// 		handle_stage();
// 	else if (e->cmd() == EVENT_TICKADVSTAGE)
// 		handle_advstage();
// 	else if (e->cmd() == EVENT_TICKCAMP)
// 		handle_camp();
// 	else if (e->cmd() == EVENT_TICKGUILD)
// 		handle_guild();
// 	else if (e->cmd() == EVENT_TICKGLOBAL)
// 		handle_global();
// 	else if (e->cmd() == EVENT_TICKROBBERY)
// 		handle_robbery();
// 	else if (e->cmd() == EVENT_TICKRICHEST)
// 		handle_richest();
// 	else if (e->cmd() == EVENT_TICKGHOSTSHIP)
// 		handle_ghostship();
// 	else if (e->cmd() == EVENT_TICKPLAYERBASEINFO)
// 		handle_playerbaseinfo();
// 	else if (e->cmd() == EVENT_TICKLIMITSHIP)
// 		handle_limitship();
// 	else if (e->cmd() == EVENT_TICKCHAT)
// 		handle_chat();
// 	else if (e->cmd() == EVENT_TICKCAMPBATTLE)
// 		handle_campbattle();
//     else if (e->cmd() == EVENT_TICK_ONE_ARMED_BANDIT)
//     {
//         this->handle_onearmedbandit();
//     }
// 	else if (e->cmd() == EVENT_TICKLIMITCAPTAIN)
// 	{
// 		handle_limitcaptain();
// 	}
// 	else if (e->cmd() == EVENT_TICKLIMITCAPTAIN2)
// 	{
// 		handle_limitcaptain2();
// 	}
// 	else if (e->cmd() == EVENT_TICK_GOLDISLAND)
// 		handle_goldisland();
// 	else if (e->cmd() == EVENT_TICKGUILDBATTLE)
// 	{
// 		handle_guildbattle();
// 	}
// 	else if (e->cmd() == EVENT_TICKARMADA)
// 		handle_armada();
// 	else if (e->cmd() == EVENT_TICKVALENTINE)
// 		handle_valentine();
// 	else if(e->cmd() == EVENT_TICKGOLDACTIVE)
// 		handle_goldactive();
// 	else if(e->cmd() == EVENT_SAVE_GMAES_TUANGOUMODUL_DATA)
// 	{
// 		handle_tuangou_save();
// 	}
// 	else if(e->cmd() == EVENT_TICKRANKING)
// 	{
// 		handle_ranking_save();
// 	}
// 	else if(e->cmd() == EVENT_TICKGOLDACTIVE2)
// 	{
// 		handle_goldactive2();
// 	}
// 	else if(e->cmd() == EVENT_TICKGLOBALGUILDBATTLEGAME)
// 		handle_globalguildbattlegame();
}

// void TickEventHandler::handle_arena()
// {
// 	ArenaModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_quiz()
// {
// 	QuizModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_guard()
// {
// 	GuardModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_playername()
// {
// 	PlayerNameModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_stage()
// {
// 	StageModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_advstage()
// {
// 	AdvStageModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_camp()
// {
// 	CampModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_guild()
// {
// 	GuildModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_global()
// {
// 	GlobalModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_robbery()
// {
// 	RobberyModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_richest()
// {
// 	RichestModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_ghostship()
// {
// 	GhostShipModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_playerbaseinfo()
// {
// 	PlayerBaseInfoModuleInst::instance().saveAll();
// }
// 
// void TickEventHandler::handle_limitship()
// {
// 	LimitShipModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_campbattle()
// {
// 	CampBattleModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_chat()
// {
// 	ChatModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_onearmedbandit()
// {
//     OnearmedBanditModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_limitcaptain()
// {
// 	LimitCaptainModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_limitcaptain2()
// {
// 	LimitCaptainModuleTwoInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_goldisland()
// {
// 	GoldIslandModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_guildbattle()
// {
// 	GuildBattleModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_armada()
// {
// 	ArmadaModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_valentine()
// {
// 	ValentineModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_goldactive()
// {
// 	GoldActiveModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_tuangou_save()
// {
// 	TuanGouModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_ranking_save()
// {
// 	RankingModuleInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_goldactive2()
// {
// 	GoldActiveModuleTwoInst::instance().SaveAll();
// }
// 
// void TickEventHandler::handle_globalguildbattlegame()
// {
// 	GlobalGuildBattleGameModuleInst::instance().SaveAll();
// }

