#ifndef _TICKEVENT_H_
#define _TICKEVENT_H_
#include "BaseEvent.h"

class TickEventHandler : public CBaseEvent
{
public:
	TickEventHandler(){}
	~TickEventHandler(){}

	static void createInstance(GameEventHandler* eh)
	{
		getInstance()->eh_ = eh;
#if 0   
		eh->getEventHandler()->registHandler(EVENT_TICKARENA,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKQUIZ,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGUARD,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKPLAYERNAME,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKSTAGE,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKADVSTAGE,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKCAMP,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGUILD,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGLOBAL,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKROBBERY,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKRICHEST,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGHOSTSHIP,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKPLAYERBASEINFO,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKLIMITSHIP,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKCHAT,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKCAMPBATTLE,
			(ProcessRoutine)TickEventHandler::handle_);
        eh->getEventHandler()->registHandler(EVENT_TICK_ONE_ARMED_BANDIT,
            (ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKLIMITCAPTAIN,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKLIMITCAPTAIN2,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICK_GOLDISLAND,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGUILDBATTLE,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKARMADA,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKVALENTINE,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGOLDACTIVE,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_SAVE_GMAES_TUANGOUMODUL_DATA,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKRANKING,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGOLDACTIVE2,
			(ProcessRoutine)TickEventHandler::handle_);
		eh->getEventHandler()->registHandler(EVENT_TICKGLOBALGUILDBATTLEGAME,
			(ProcessRoutine)TickEventHandler::handle_);
#endif
	}

	static TickEventHandler* getInstance()
	{
		static TickEventHandler instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		TickEventHandler::getInstance()->handle(e);
	}

	static void addEvent(EVENT_DEFINE eventid)
	{
		TickEventHandler::getInstance()->addEvent_(eventid);
	}

private:
	void addEvent_(EVENT_DEFINE eventid);
	void handle(Event* e);
// 	void handle_arena();
// 	void handle_quiz();
// 	void handle_guard();
// 	void handle_playername();
// 	void handle_stage();
// 	void handle_advstage();
// 	void handle_camp();
// 	void handle_guild();
// 	void handle_global();
// 	void handle_robbery();
// 	void handle_richest();
// 	void handle_ghostship();
// 	void handle_playerbaseinfo();
// 	void handle_limitship();
// 	void handle_chat();
// 	void handle_campbattle();
//     void handle_onearmedbandit();
// 	void handle_limitcaptain();
// 	void handle_limitcaptain2();
// 	void handle_goldisland();
// 	void handle_guildbattle();
// 	void handle_armada();
// 	void handle_valentine();
// 	void handle_goldactive();
// 	void handle_tuangou_save();
// 	void handle_ranking_save();
// 	void handle_goldactive2();
// 	void handle_globalguildbattlegame();
};

#endif
