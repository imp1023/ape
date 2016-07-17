#ifndef _GMHANDLER_H_
#define _GMHANDLER_H_

#include "BaseEvent.h"

class User;

class GMHandler : public CBaseEvent
{
public:
    GMHandler(){}
    ~GMHandler(){}

	static void createInstance(GameEventHandler* eh)
	{
		getInstance()->eh_ = eh;
		eh->getEventHandler()->registHandler(EVENT_GM,
			(ProcessRoutine)GMHandler::handle_);
	}

	static GMHandler* getInstance()
	{
		static GMHandler instance_;
		return &instance_;
	}

	static void handle_(Event* e)
	{
		getInstance()->handle(e);
	}

private:

    void handle(Event* e);
    string handle_setmoney(const vector<string>& gmtoken, User* user, string& sid);
	string handle_costmoney(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addmail(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addsysmail(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delmail(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addprop(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delprop(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addpaper(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delpaper(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addship(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delship(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addparts(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delparts(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setlevel(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setviplevel(const vector<string>& gmtoken, User* user, string& sid);
	string handle_winstage(const vector<string>& gmtoken, User* user, string& sid);
	string handle_losestage(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearguardcount(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleararenacount(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addexp(const vector<string>& gmtoken, User* user, string& sid);
	string handle_skipguide(const vector<string>& gmtoken, User* user, string& sid);
	string handle_broadcast(const vector<string>& gmtoken, string& sid);
	string handle_reload(const vector<string>& gmtoken, string& sid);
	string handle_addjuniorpaper(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addseniorpaper(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearroleinfo(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearuid(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addmonthcard(const vector<string>& gmtoken, User* user, string& sid);
	string handle_resetmonthcard(const vector<string>& gmtoken, User* user, string& sid);
	string handle_funcswitch(const vector<string>& gmtoken, string& sid);	
	string handle_clearfirstrecharge(const vector<string>& gmtoken, User* user, string& sid);
	string handle_recharge(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearoncebuy(const vector<string>& gmtoken, User* user, string& sid);
	string handle_profile(const vector<string>& gmtoken, string& sid);
	string handle_resetstarttime(const vector<string>& gmtoken, string& sid);
	string handle_banlogin(const vector<string>& gmtoken, User* user, string& sid);
	string handle_usermapsize(const vector<string>& gmtoken, string& sid);
	string handle_testdrop(const vector<string>& gmtoken, string& sid);
	string handle_testcaptainrecruit(const vector<string>& gmtoken, string& sid);
	string hanlde_setguildlevel(const vector<string>& gmtoken, User* user, string& sid);
	string handle_virtualcharge(const vector<string>& gmtoken, string& sid);
	string handle_addpartpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delpartpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_newday(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setarenascore(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearrobberyguide(const vector<string>& gmtoken, User* user, string& sid);
	string handle_fixpartlist(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearstage(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addjuniorstar(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addseniorstar(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setmedal(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleargrowup(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addguildcontribute(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addusertotalcontribute(const vector<string>& gmtoken, User* user, string& sid);
	string handle_createguild(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearguilddeputy(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addghostshiptime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addlimitshiptime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addsoul(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delsoul(const vector<string>& gmtoken, User* user, string& sid);
	string handle_expsoul(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addawardcenter(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addaward(const vector<string>& gmtoken, User* user, string& sid);
	string handle_battletest(const vector<string>& gmtoken);	
	string handle_setchattime(const vector<string>& gmtoken, string& sid);
	string handle_setcampbattlewinning(const vector<string>& gmtoken, string& sid);
	string handle_setcampbattledate(const vector<string>& gmtoken, string& sid);
	string handle_setmorale(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setport(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearattackcount(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearluckygoddess(const vector<string>& gmtoken, User* user, string& sid);
	string handle_Set_MilitaryRankType(const vector<string>& gmtoken, User* user, string& sid);
	string handle_Set_MilitaryRankHon(const vector<string>& gmtoken, User* user, string& sid);
	string handle_Clear_MilitaryRankRewradTime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_Set_MilitaryRankCampCount(const vector<string>& gmtoken, User* user, string& sid);
	string handle_refreshmisteryshop(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setrefreshtimes(const vector<string>& gmtoken, User* user, string& sid);
    string handle_christmasfree(const vector<string>& gmtoken, User* user, string& sid);
    string handle_christmasscore(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearrechargereward(const vector<string>& gmtoken, User* user, string& sid);
	string handle_fixname(const vector<string>& gmtoken, string& sid);
	string handle_setgoddesspoint(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setlogouttime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addluckboxexchangepoint(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearloginpokerday(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addloginpokerbroadcast(const vector<string>& gmtoken, User* user, string& sid);
	string handle_guildchangechief(const vector<string>& gmtoken, string& sid);
	string handle_setguide(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleanspydiatime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleanrecruitdiatime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleansoulpaperflag(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleanshipformation(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addjuniorcaptain(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addseniorcaptain(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addcaptain(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addcaptainpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delcaptain(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delcaptainpiece(const vector<string>& gmtoken, User* User, string& sid);
    string handle_addprestige(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setplattouid(const vector<string>& gmtoken, User* user, string& sid);
	string handle_loadguildfromold(const vector<string>& gmtoken, string& sid);
	string handle_checkuserguildid(const vector<string>& gmtoken, User* user, string& sid);
	string handle_deleteguildmember(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearfirstbuyrecruitaward(const vector<string>& gmtoken, User* user, string& sid);
    string handle_addexploit(const vector<string>& gmtoken, User* user, string& sid);
    string handle_addSignGuildContribute(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearguildbattle(const vector<string>& gmtoken, string& sid);
	string handle_clearplayerguildbattle(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addawardbygiftid(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleararmadamoduledata(const vector<string>& gmtoken, string& sid);
	string handle_delhaoduostage(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setarmadabattletimes(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addsoulpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delsoulpiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_getarmadarank(const vector<string>& gmtoken, string& sid);
	string handle_getarmadalastendtime( const vector<string>& gmtoken, string& sid );
	string handle_settaskreward( const vector<string>& gmtoken, User* user, string& sid );
	string handle_clearcopybytype( const vector<string>& gmtoken, User* user, string& sid );
	string handle_addtalent(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setguardstage(const vector<string>& gmtoken, User* user, string& sid);
	string handle_cleartalentinitflag(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setscience(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setrole(const vector<string>& gmtoken, User* user, string& sid);
	string handle_globalarenasignup(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addlimitcaptain1time(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addlimitcaptain2time(const vector<string>& gmtoken, User* user, string& sid);
	//string handle_setdrawluckwheel( const vector<string>& gmtoken, User* user, string& sid );
	string handle_addseiko(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delseiko(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addseikopiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_delseikopiece(const vector<string>& gmtoken, User* user, string& sid);
	string handle_clearprostrateflag(const vector<string>& gmtoken, User* user, string& sid);
	string handle_goldleadersendward(const vector<string>& gmtoken, string& sid);
	string handle_setgoldislandforceclear(const vector<string>& gmtoken, string& sid);
	string handle_goldleadersendwardbyrank(const vector<string>& gmtoken, User* user, string& sid);
	string handle_undorecharge(const vector<string>& gmtoken, User* user, string& sid);
	string handle_dbuserpartdata(const vector<string>& gmtoken, User* user, string& sid);
	string handle_setstagetowertime(const vector<string>& gmtoken, User* user, string& sid);
	string handle_changepartid(const vector<string>& gmtoken, User* user, string& sid);
	string handle_sweepstage(const vector<string>& gmtoken, User* user, string &sid);						//关卡扫荡GM
	string handle_activity_opt(const vector<string>& gmtoken, User* user, string &sid);						//活动相关GM
	string handle_friends_opt(const vector<string>& gmtoken, User* user, string &sid);						//好友相关GM
	string handle_cleanfirstorshop( const vector<string>& gmtoken, User* user, string& sid );
	string handle_superexchange(const vector<string>& gmtoken, User* user, string& sid);
	string handle_resetstagetower( const vector<string>& gmtoken, User* user, string& sid );
	string handle_challengesignup(const vector<string>& gmtoken, User* user, string& sid);
	string handle_addgoldcaptain(const vector<string>& gmtoken, User* user, string& sid);
	string handle_syncgoldactvity(const vector<string>& gmtoken, User* user, string& sid);
	string handle_synclimitship(const vector<string>& gmtoken, User* user, string& sid);
	string handle_syncguildbattlesign(const vector<string>& gmtoken, string& sid);
	string handle_guildbattlesign(const vector<string>& gmtoken, string& sid);
	string handle_guildbattleresetmap(const vector<string>& gmtoken, string& sid);
	string handle_addchamptionRecord(const vector<string>& gmtoken, string& sid);
	string handle_tuangouopt(const vector<string>& gmtoken, User* user, string& sid);						//团购相关
	string handle_seikoexchange(const vector<string>& gmtoken, User* user, string &sid);					//精工积分兑换相关
	string handle_activitymailopt(const vector<string>& gmtoken, User* user, string &sid);					//限时战舰操作（发奖、加分）
	string handle_setgifttime(const vector<string>& gmtoken,User *user, string &sid); 
	string handle_setWaitGiftTime(const vector<string>& gmtoken,User *user, string &sid);
	string handle_savedata(const vector<string>& gmtoken, string& sid);
	string handle_getranking(const vector<string>& gmtoken , string& sid);
	string handle_unifyuid(const vector<string>& gmtoken, User* user, string &sid);
	string handle_clearchargebackgift( const vector<string>& gmtoken, User* user, string &sid );
	string handle_chargebackgiftopt( const vector<string>& gmtoken, User* user, string &sid );
	string handle_autosoulopt(const vector<string>& gmtoken, User* user, string &sid);
	string handle_addguildmember(const vector<string>& gmtoken, User* user, string& sid);
	string handle_globalguildbattleopt(const vector<string>& gmtoken, string& sid);
    string successContent(const string& sid);
	string successContent(const string& sid,const vector<string>& params);
    string errorContent(const string& msg, int errorCode, const string& sid);
    string failContent(const string& sid);
    string formatErrorContent(const string& sid);

    int64 getUid(const string& str);
	int64 getInt64(const string& str);
    int getInt(const string& str);
    string getSid(const vector<string>& gmtoken);
};

#endif

