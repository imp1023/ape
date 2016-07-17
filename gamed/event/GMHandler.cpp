#include "GMHandler.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;
#include "../../common/error_def.h"
#include "../../common/Proto2Json.h"
#include "../../event/event.pb.h"
#include "../../event/EventQueue.h"
#include "../GameNetHandler.h"
#include "../GameDataHandler.h"
#include "../../logic/Player.h"
#include "../../logic/GameConfigMgr.h"
#include "../../logic/CashTbl.h"
#include "../../logic/FuncSwitch.h"
#include "../../common/Clock.h"
#include "../../logic/GuildModule.h"
#include "../../logic/GlobalModule.h"
#include "../../logic/CashTbl.h"
#include "../../logic/NpcTbl.h"
#include "../../logic/StageMgr.h"
#include "../../logic/PlayerBaseInfoModule.h"
#include "../../logic/GhostShipModule.h"
#include "../../logic/BattleMgr.h"
#include "../../logic/CampBattleModule.h"
#include "../../logic/MilitaryRankTbl.h"
#include "../../logic/MilitaryRankRightsTbl.h"
#include "../../logic/PlayerNameModule.h"
#include "../../logic/TurnOverTbl.h"
#include "../../logic/GuildBattleModule.h"
#include "../../logic/LimitShipModule.h"
#include "../../logic/GiftTbl.h"
#include "../../logic/ArmadaModule.h"
#include "../common/DateTime.h"
#include "../../logic/GuardMgr.h"
#include "../../logic/DefenceStageDataTbl.h"
#include "../../logic/LimitCaptainModule.h"
#include "../../logic/LimitCaptainModuleTwo.h"
#include "../../logic/LuckyWheelRewardTbl.h"
#include "../../logic/GoldActiveModule.h"
#include "../../logic/GoldIslandModule.h"
#include "../../logic/VipTbl.h"
#include "../../logic/DropTbl.h"
#include "../../logic/GoldActiveModule.h"
#include "../../logic/captainRecruitRndTbl.h"
#include "../../logic/TuanGouModule.h"
#include "../../logic/OnearmedBanditModule.h"
#include "../../logic/RankingModule.h"
#include "../../event/RankPlayerList.pb.h"
#include "../../logic/GlobalGuildBattleGameModule.h"
/*
addmoney,(u)id,resType,num,sid
setlevel,(u)id,level,sid
setviplevel,(u)id,level,sid
reload,(g)gameid,sid
addexp,(u)id,exp,sid
skipguide,(u)id,sid
addmail,(u)id,text,id1,cnt1,id2,cnt2,id3,cnt3,sid
addsysmail,(u)id,mailid,param1,param2,...,param8,sid
delmail,(u)id,type,id,sid
addpiece,(u)id,pieceid,count,sid
delpiece,(u)id,pieceid,count,sid
addprop,(u)id,propid,count,sid
delprop,(u)id,propid,count,sid
addpaper,(u)id,paperid,count,sid
delpaper,(u)id,paperid,count,sid
addship,(u)id,shipid,count,sid
delship,(u)id,id,sid
addparts,(u)id,partsid,count,sid
delparts,(u)id,id,sid
winstage,(u)id,id,star,sid
losestage,(u)id,id,sid
broadcast,(g)gameid,text,sid
clearguardcount,(u)id,sid
cleararenacount,(u)id,sid
addjuniorpaper,(u)id,count,sid
addseniorpaper,(u)id,count,sid
clearroleinfo,(u)id,sid
clearuid,(u)id,sid
addmonthcard,(u)id,id,sid
resetmonthcard,(u)id,sid
funcswitch,(g)gameid,id,enable,sid
clearfirstrecharge,(u)id,sid
charge,(u)id,type,sid
clearoncebuy,(u)id,sid
resetstarttime,(g)gameid,2013-1-1-0:0:0,sid
banlogin,(u)id,isban,hour,sid
setguildlevel,(u)id,guildid,level,sid
usermapsize,(g)gameid,sid
addpartpiece,(u)id,partpieceid,count,sid
delpartpiece,(u)id,partpieceid,count,sid
clearrobberyguide,(u)id,sid
setarenascore,(u)id,score,sid
setnewday,(u)id,sid
fixpartlist,(u)id,sid
clearstage,(u)id,mode,stageid,sid
addjuniorstar,(u)id,count,sid
addseniorstar,(u)id,count,sid
setmedal,(u)id,count,sid
cleargrowup,(u)id,sid
addguildcontribute,(u)id,guildId,contribute,sid
createguild,(u)id,name,sid
clearguilddeputy,(u)id,guildid,sid
setchattime,(g)gameid,time,sid
addghostshiptime,(u)id,count,sid
addlimitshiptime,(u)id,count,sid
addsoul,(u)id,soulid,sid
delsoul,(u)id,soulid,sid
expsoul,(u)id,soulid,exp,sid
addawardcenter,(u)id,awardcenterid,type1,id1,count1,type2,id2,count2,...,fromtype,sid
addaward,(u)id,name,desc,type1,id1,count1,type2,id2,count2,...,fromtype,sid
battletest,(g)gameid,attackerid,defenderid,battlecount  //only for local test server, other server is strictly prohibited!
setcampbattlewinning,(g)gameid,camp,count,sid
setmorale,(u)id,morale,sid
setport,(u)id,protid,sid
clearattackcount,(u)id,sid
clearluckygoddess,(u)id,sid
refreshmisteryshop,(u)id,sid
setrefreshtimes,(u)id,count,sid
clearrechargereward,(u)id,sid
setmilitaryranktype,(u)id,ranktype,sid
setmilitaryrankhon,(u)id,count,sid
clearmilitaryranktime,(u)id,sid
setmilitaryrankcount,(u)id,count,sid
setcampbattledate,(g)gameid,2013-1-1-0:0:0,sid
fixname,(g)gameid,name,sid
setgoddesspoint,(u)id,num,sid
setlogouttime,(u)id,2013-1-1-0:0:0,sid
addluckexchangepoint,(u)id,point,sid
clearloginpokerday,(u)id,day,sid
addloginpokerbroadcast,(u)id,index,info,sid
guildchangechief,(g)gameid,guildid,newchiefid,sid
setguide,(u)id,step,sid
cleanspytime,(u)id,sid
clearsoulpaperflag,(u)id,sid
clearshipformation,(u)id,sid
addjuniorcaptain,(u)id,count,sid
addseniorcaptain,(u)id,count,sid
addcaptainpiece,(u)id,id,count,sid
addcaptain,(u)id,id,count,sid
setplattouid,(u)id,sid
loadguildfromold,(g)gamedid,sid
checkuserguildid,(u)uid,sid
deleteguildmember,(u)id,guildid,sid
addusertotalcontribute,(u)id,count,sid
clearfirstbuyrecruitaward,(u)id,sid
addSignGuildContribute,u(id),sid
clearguildbattle,g1,2013-1-1-0:0:0,sid
clearplayerguildbattle,(u)id,sid
cleararmadamoduledata,g(n)/a,sid
getarmadarank,g(n)/a,sid
getarmadalastendtime,g(n)/a,sid
addawardbygiftid,uid,index,giftid,param1,param2.....,fromtype,sid
delhaoduostage,(u)id,mode,stageid,sid
setarmadabattletimes,(u)id,times,sid
settaskreward,(u)id,whichfield,type,times,sid
clearcopywarbytype,(u)id,copytype,sid
//setdrawluckwheel,(u)id,type,state,sid	//设置幸运转盘某玩家是否有抽奖机会, 屏蔽了
addseikopiece,(u)id,seikopieceid,count,sid
delseikopiece,(u)id,seikopieceid,count,sid
addseiko,(u)id,seikoid,count,sid
delseiko,(u)id,id,sid
goldleadersendward,g1,version,sid//金牌舰长全服排行榜发奖
goldleadersendwardbyrank,(u)id,rank,sid//金牌舰长根据排行发奖
setgoldlandforceclear,g(n),sid
undocharge,(u)id,type,sid //撤销充值
setstagetowertime,(u)id,type,num, sid //type = 1，重置纵横四海的挑战次数， type = 2增加星星的数量
dbuser,(u)id,path,type,sid	//获取dbuser数据 path：路径 DB_User为根目录，例如：player/gold 含义为查看玩家角色的金币信息 type 0：单行输出 1：格式化输出
activitytest,(u)id,type,opt,sid	//活动设置（可扩展）
friendtest,(u)id,opt,value,sid	//好友相关设置（可扩展）
cleanfirstorshop,(u)id, type, sid// type = 1 清空firstreward; = 2 清空seikoshop
setsuperexchangescore,(u)id,score,sid//补超级兑换积分
resetstagetower,(u)id,sid //重置纵横四海
tuangouopt,(u)id,type,value,sid	//type(0报名2购买)value(购买的索引)
challengesignup,uid,sid
addprestige,u(id),num,sid
activitymailopt,u(id),opt,sid	// （opt: 1限时战舰发送邮件）
savedata,a/g(num),-1	//
setscience,(u)id,num,sid
setgifttime,(u)id,time,sid
setwaittime,uid,waittime,sid
getranking,g(num),type,sid // 获取排行榜, (0 等级 1 战斗力) 
clearchargebackgift,(u)id,sid //[充值返好礼活动，配在funcswith]重置玩家身上的记录 
chargebackgiftopt,(u)id,opt,status,sid //[充值返好礼活动，配在funcswith] opt: 0充值档位; status: 0置为未领取 1置为已经领取
autosoulopt,(u)id,opt,pos,sid //一键勋章操作 opt 0：一键装备 1：一键卸下
addguildmember,uid,guildid,sid
*/




void GMHandler::handle(Event* e)
{
    if (e->state() != Web_To_Game)
        return;

    if (!e->has_forward())
        return;

    if (!e->has_content())
        return;

    LOG4CXX_INFO(logger_, "Handle GM: " << e->content());

    string response;
    do 
    {
        string gmstr = e->content();
        vector<string> gmtoken;
        static const string delim = ",";
        tokenize(gmstr, gmtoken, delim);
        string sid = gmtoken[gmtoken.size()-1];
        if (gmtoken.size() < 2)
        {
            response = formatErrorContent(sid);
            break;
        }

		int64 uid = getUid(gmtoken[1]);
		string cmd = gmtoken[0];
		if (cmd == "reload")
		{
			response = handle_reload(gmtoken, sid);
			break;
		}
		else if (cmd == "broadcast")
		{
			response = handle_broadcast(gmtoken, sid);
			break;
		}
		else if(cmd == "goldleadersendward")
		{
			response = handle_goldleadersendward(gmtoken, sid);
			break;
		}
		else if (cmd == "funcswitch")
		{
			response = handle_funcswitch(gmtoken, sid);
			break;
		}
		else if (cmd == "profile")
		{
			response = handle_profile(gmtoken, sid);
			break;
		}
		else if (cmd == "resetstarttime")
		{
			response = handle_resetstarttime(gmtoken, sid);
			break;
		}
		else if (cmd == "usermapsize")
		{
			response = handle_usermapsize(gmtoken, sid);
			break;
		}
		else if (cmd == "virtualcharge")
		{
			response = handle_virtualcharge(gmtoken, sid);
			break;
		}
		else if (cmd == "setchattime")
		{
			response = handle_setchattime(gmtoken, sid);
			break;
		}
		else if (cmd == "battletest")
		{
			response = handle_battletest(gmtoken);
			if (response.size() > 0)
				break;
			eh_->postBackEvent(e);
			return;			
		}
		else if (cmd == "setcampbattlewinning")
		{
			response = handle_setcampbattlewinning(gmtoken, sid);
			break;
		}
		else if (cmd == "setcampbattledate")
		{
			response = handle_setcampbattledate(gmtoken, sid);
			break;
		}
		else if (cmd == "fixname")
		{
			response = handle_fixname(gmtoken,sid);
			break;
		}
		else if (cmd == "loadguildfromold")
		{
			response = handle_loadguildfromold(gmtoken, sid);
			break;
		}
		else if (cmd== "guildchangechief")
		{
			response = handle_guildchangechief(gmtoken, sid);
			break;
		}
		else if ( cmd == "clearguildbattle" )
		{
			response = handle_clearguildbattle(gmtoken, sid);
			break;
		}
		else if ( cmd == "cleararmadamoduledata" )
		{
			response = handle_cleararmadamoduledata(gmtoken, sid);
			break;
		}
		else if ( cmd == "getarmadarank" )
		{
			response = handle_getarmadarank(gmtoken, sid);
			break;
		}
		else if ( cmd == "getarmadalastendtime" )
		{
			response = handle_getarmadalastendtime(gmtoken, sid);
			break;
		}
		else if(cmd == "setgoldlandforceclear")
		{
			response = handle_setgoldislandforceclear(gmtoken, sid);
			break;
		}
		else if (cmd == "testdrop")
		{
			response = handle_testdrop(gmtoken, sid);
			break;
		}
		else if (cmd == "testcaptainrecruit")
		{
			response = handle_testcaptainrecruit(gmtoken, sid);
			break;
		}
		else if (cmd == "syncguildbattlesign")
		{
			response = handle_syncguildbattlesign(gmtoken, sid);
			break;
		}
		else if (cmd == "guildbattlesign")
		{
			response = handle_guildbattlesign(gmtoken, sid);
			break;
		}
		else if (cmd == "guildbattleresetmap")
		{
			response = handle_guildbattleresetmap(gmtoken, sid);
			break;
		}
		else if (cmd == "addChamptionRecord")
		{
			response = handle_addchamptionRecord(gmtoken, sid);
			break;
		}
		else if (cmd == "savedata")
		{
			response = handle_savedata(gmtoken, sid);
			break;
		}
		else if(cmd == "getranking")
		{
			response = handle_getranking(gmtoken,sid);
			break;
		}
		else if (cmd == "globalguildbattleopt")
		{
			response = handle_globalguildbattleopt(gmtoken, sid);
			break;
		}
        LoadStatus loadStatus = LOAD_INVALID;
        GameDataHandler* dh = eh_->getDataHandler();
        User* user = dh->getUser(uid, &loadStatus, true);
        if (user == NULL && loadStatus == LOAD_WAITING)
        {
			eh_->postBackEvent(e);
            return;
        }

        if (user == NULL)
        {
            response = errorContent("Load User Failed", ERR_LOADUSER_FAILED, sid);
            break;
        }
		
		user->GetPlayer()->SetEventHandler(Daemon::Instance().GetGameEventHandler());
        if (cmd == "addmoney")
            response = handle_setmoney(gmtoken, user, sid);
		else if (cmd == "costmoney")
			response = handle_costmoney(gmtoken, user, sid);
		else if (cmd == "addmail")
			response = handle_addmail(gmtoken, user, sid);
		else if (cmd == "addsysmail")
			response = handle_addsysmail(gmtoken, user, sid);
		else if (cmd == "delmail")
			response = handle_delmail(gmtoken, user, sid);
		else if (cmd == "addprop")
			response = handle_addprop(gmtoken, user, sid);
		else if (cmd == "delprop")
			response = handle_delprop(gmtoken, user, sid);
		else if (cmd == "addpaper")
			response = handle_addpaper(gmtoken, user, sid);
		else if (cmd == "delpaper")
			response = handle_delpaper(gmtoken, user, sid);
		else if (cmd == "addpiece")
			response = handle_addpiece(gmtoken, user, sid);
		else if (cmd == "delpiece")
			response = handle_delpiece(gmtoken, user, sid);
		else if (cmd == "addship")
			response = handle_addship(gmtoken, user, sid);
		else if (cmd == "delship")
			response = handle_delship(gmtoken, user, sid);
		else if (cmd == "addparts")
			response = handle_addparts(gmtoken, user, sid);
		else if (cmd == "delparts")
			response = handle_delparts(gmtoken, user, sid);
		else if (cmd == "setlevel")
			response = handle_setlevel(gmtoken, user, sid);
		else if (cmd == "setviplevel")
			response = handle_setviplevel(gmtoken, user, sid);
		else if (cmd == "winstage")
			response = handle_winstage(gmtoken, user, sid);
		else if (cmd == "losestage")
			response = handle_losestage(gmtoken, user, sid);
		else if (cmd == "clearguardcount")
			response = handle_clearguardcount(gmtoken, user, sid);
		else if (cmd == "cleararenacount")
			response = handle_cleararenacount(gmtoken, user, sid);
		else if (cmd == "addexp")
			response = handle_addexp(gmtoken, user, sid);
		else if (cmd == "skipguide")
			response = handle_skipguide(gmtoken, user, sid);
		else if (cmd == "addjuniorpaper")
			response = handle_addjuniorpaper(gmtoken, user, sid);
		else if (cmd == "addseniorpaper")
			response = handle_addseniorpaper(gmtoken, user, sid);
		else if (cmd == "clearroleinfo")
			response = handle_clearroleinfo(gmtoken, user, sid);
		else if (cmd == "clearuid")
			response = handle_clearuid(gmtoken, user, sid);
		else if (cmd == "addmonthcard")
			response = handle_addmonthcard(gmtoken, user,sid);
		else if (cmd == "resetmonthcard")
			response = handle_resetmonthcard(gmtoken, user, sid);
		else if (cmd == "clearfirstrecharge")
			response = handle_clearfirstrecharge(gmtoken,user,sid);
		else if (cmd == "charge")
			response = handle_recharge(gmtoken, user, sid);
		else if (cmd == "clearoncebuy")
			response = handle_clearoncebuy(gmtoken, user, sid);
		else if (cmd == "banlogin")
			response = handle_banlogin(gmtoken, user, sid);
		else if (cmd == "setguildlevel")
			response = hanlde_setguildlevel(gmtoken, user, sid);
		else if (cmd == "addpartpiece")
			response = handle_addpartpiece(gmtoken, user, sid);
		else if (cmd == "delpartpiece")
			response = handle_delpartpiece(gmtoken, user, sid);
		else if (cmd == "setarenascore")
			response = handle_setarenascore(gmtoken, user, sid);
		else if (cmd == "setnewday")
			response = handle_newday(gmtoken, user, sid);
		else if (cmd == "clearrobberyguide")
			response = handle_clearrobberyguide(gmtoken, user, sid);
		else if (cmd == "fixpartlist")
			response = handle_fixpartlist(gmtoken, user, sid);
		else if (cmd == "clearstage")
			response = handle_clearstage(gmtoken, user, sid);
		else if (cmd == "addjuniorstar")
			response = handle_addjuniorstar(gmtoken, user, sid);
		else if (cmd == "addseniorstar")
			response = handle_addseniorstar(gmtoken, user, sid);
		else if (cmd == "setmedal")
			response = handle_setmedal(gmtoken, user, sid);
		else if (cmd == "cleargrowup")
			response = handle_cleargrowup(gmtoken, user, sid);
		else if (cmd == "addguildcontribute")
			response = handle_addguildcontribute(gmtoken, user, sid);
		else if (cmd == "createguild")
			response = handle_createguild(gmtoken, user, sid);
		else if (cmd == "clearguilddeputy")
			response = handle_clearguilddeputy(gmtoken, user, sid);
		else if (cmd == "addghostshiptime")
			response = handle_addghostshiptime(gmtoken, user, sid);
		else if (cmd == "addlimitshiptime")
			response = handle_addlimitshiptime(gmtoken, user, sid);
		else if (cmd == "addsoul")
			response = handle_addsoul(gmtoken, user, sid);
		else if (cmd == "delsoul")
			response = handle_delsoul(gmtoken, user, sid);
		else if (cmd == "expsoul")
			response = handle_expsoul(gmtoken, user, sid);
		else if (cmd == "addawardcenter")
			response = handle_addawardcenter(gmtoken, user, sid);
		else if (cmd == "addaward")
			response = handle_addaward(gmtoken, user, sid);
		else if (cmd == "setmorale")
			response = handle_setmorale(gmtoken, user, sid);
		else if (cmd == "setport")
			response = handle_setport(gmtoken, user, sid);
		else if (cmd == "clearattackcount")
			response = handle_clearattackcount(gmtoken, user, sid);
		else if (cmd == "clearluckygoddess")
			response = handle_clearluckygoddess(gmtoken, user, sid);
		else if(cmd == "setmilitaryranktype")
			response = handle_Set_MilitaryRankType(gmtoken, user, sid);
		else if(cmd == "setmilitaryrankhon")
			response = handle_Set_MilitaryRankHon(gmtoken, user, sid);
		else if(cmd == "clearmilitaryranktime")
			response = handle_Clear_MilitaryRankRewradTime(gmtoken, user, sid);
		else if(cmd == "setmilitaryrankcount")
			response = handle_Set_MilitaryRankCampCount(gmtoken, user, sid);
		else if (cmd == "refreshmisteryshop")
			response = handle_refreshmisteryshop(gmtoken, user, sid);
		else if (cmd == "setrefreshtimes")
			response = handle_setrefreshtimes(gmtoken, user, sid);
		else if(cmd == "clearloginpokerday")
			response = handle_clearloginpokerday(gmtoken, user, sid);
		else if(cmd == "addloginpokerbroadcast")
			response = handle_addloginpokerbroadcast(gmtoken, user, sid);
		else if(cmd == "cleanspytime")
			response =  handle_cleanspydiatime(gmtoken, user, sid);
		else if(cmd == "cleanrecruittime")
			response = handle_cleanrecruitdiatime(gmtoken, user, sid);
        else if (cmd == "christmasfree")
        {
            response = handle_christmasfree(gmtoken,user,sid);
        }
        else if (cmd == "christmasscore")
        {
            response = handle_christmasscore(gmtoken,user,sid);
        }
		else if (cmd == "clearrechargereward")
			response = handle_clearrechargereward(gmtoken, user, sid);
		else if (cmd == "setgoddesspoint")
		{
			response = handle_setgoddesspoint(gmtoken, user, sid);
		}
		else if (cmd == "addluckexchangepoint")
		{
			response = handle_addluckboxexchangepoint(gmtoken, user, sid);
		}
		 else if (cmd == "setlogouttime")
		{
			response= handle_setlogouttime(gmtoken, user, sid);
		}
		else if ( cmd == "setguide" )
		{
			response = handle_setguide(gmtoken, user, sid);
		}
		else if (cmd == "clearsoulpaperflag")
		{
			response = handle_cleansoulpaperflag(gmtoken, user, sid);
		}
		else if(cmd == "clearshipformation")
		{
			response = handle_cleanshipformation(gmtoken, user, sid);
		}
		else if (cmd == "addjuniorcaptain")
		{
			response = handle_addjuniorcaptain(gmtoken, user, sid);
		}
		else if (cmd == "addseniorcaptain")
		{
			response = handle_addseniorcaptain(gmtoken, user, sid);
		}
		else if(cmd == "addcaptain")
		{
			response = handle_addcaptain(gmtoken, user, sid);
		}
		else if(cmd == "delcaptain")
		{
			response = handle_delcaptain(gmtoken, user, sid);
		}
		else if (cmd == "addcaptainpiece")
		{
			response = handle_addcaptainpiece(gmtoken, user, sid);
		}
		else if (cmd == "delcaptainpiece")
		{
			response = handle_delcaptainpiece(gmtoken, user, sid);
		}
        else if (cmd == "addprestige")
        {
            response = handle_addprestige(gmtoken, user,sid);
        }
		else if (cmd == "setplattouid")
			response = handle_setplattouid(gmtoken, user, sid);
		else if (cmd == "deleteguildmember")		
			response = handle_deleteguildmember(gmtoken, user, sid);
		else if ( cmd == "checkuserguildid")
		{
			response = handle_checkuserguildid(gmtoken, user, sid);
		}
		else if (cmd == "addusertotalcontribute")
		{
			response = handle_addusertotalcontribute(gmtoken, user, sid);
		}
		else if(cmd == "clearfirstbuyrecruitaward")
			response = handle_clearfirstbuyrecruitaward(gmtoken, user, sid);
        else if (cmd == "addexploit")
        {
            response = handle_addexploit(gmtoken,user,sid);
        }
		else if (cmd == "addSignGuildContribute")
		{
			response = handle_addSignGuildContribute(gmtoken, user, sid);
		}
		else if (cmd == "clearplayerguildbattle")
		{
			response = handle_clearplayerguildbattle(gmtoken, user, sid);
		}
		else if ( cmd == "addawardbygiftid")
		{
			response = handle_addawardbygiftid(gmtoken, user, sid);
		}
		else if ( cmd == "delhaoduostage")
		{
			response = handle_delhaoduostage(gmtoken, user, sid);
		}
		else if ( cmd == "setarmadabattletimes")
		{
			response = handle_setarmadabattletimes(gmtoken, user, sid);
		}
		else if (cmd == "addsoulpiece")
		{
			response = handle_addsoulpiece(gmtoken, user, sid);
		}
		else if (cmd == "delsoulpiece")
		{
			response = handle_delsoulpiece(gmtoken, user, sid);
		}
		else if ( cmd == "settaskreward")
		{
			response = handle_settaskreward(gmtoken, user, sid);
		}
		else if (cmd == "addtalent")
		{
			response = handle_addtalent(gmtoken, user, sid);
		}
		else if ( cmd == "setguardstage" )
		{
			response = handle_setguardstage(gmtoken, user, sid);
		}
		else if ( cmd == "cleartalentinitflag" )
		{
			response = handle_cleartalentinitflag(gmtoken, user, sid);
		}
		else if (cmd == "clearcopywarbytype")
		{
			response = handle_clearcopybytype(gmtoken, user, sid);
		}

		else if (cmd=="setscience")
		{
			response = handle_setscience(gmtoken,user,sid);
		}
		else if (cmd == "setrole")
		{
			response = handle_setrole(gmtoken, user, sid);
		}
		else if (cmd == "globalarenasignup")
		{
			response = handle_globalarenasignup(gmtoken, user, sid);
		}
		else if (cmd == "addlimitcaptain1time")
		{
			response = handle_addlimitcaptain1time(gmtoken, user, sid);
		}
		else if (cmd == "addlimitcaptain2time")
		{
			response = handle_addlimitcaptain2time(gmtoken, user, sid);
		}
		//else if (cmd == "setdrawluckwheel")
		//{
		//	response = handle_setdrawluckwheel(gmtoken, user, sid);
		//}
		else if (cmd == "addseikopiece")
			response = handle_addseikopiece(gmtoken, user, sid);
		else if (cmd == "delseikopiece")
			response = handle_delseikopiece(gmtoken, user, sid);
		else if (cmd == "addseiko")
			response = handle_addseiko(gmtoken, user, sid);
		else if (cmd == "delseiko")
			response = handle_delseiko(gmtoken, user, sid);
		else if(cmd == "goldleadersendwardbyrank") 
		{
			response = handle_goldleadersendwardbyrank(gmtoken, user, sid);
		}
        else if (cmd == "undocharge")
		{
            response = handle_undorecharge(gmtoken,user,sid);
		}
		else if (cmd == "dbuser" )
		{
			response = handle_dbuserpartdata(gmtoken, user, sid);
		}
		else if(cmd == "setstagetowertime")
		{
			response = handle_setstagetowertime(gmtoken, user, sid);
		}
		else if (cmd == "sweeptest" )
		{
			response = handle_sweepstage(gmtoken, user, sid);
		}
		else if (cmd == "activitytest")
		{
			response = handle_activity_opt(gmtoken, user, sid);
		}
		else if (cmd == "friendtest")
		{
			response = handle_friends_opt(gmtoken, user, sid);
		}
		else if (cmd == "cleanfirstorshop")
		{
			response = handle_cleanfirstorshop(gmtoken, user, sid);
		}	
		
		if(cmd == "setsuperexchangescore")
		{
			response = handle_superexchange(gmtoken, user, sid);
		}
		else if(cmd == "resetstagetower")
		{
			response = handle_resetstagetower(gmtoken, user, sid);
		}
		else if (cmd == "challengesignup")
		{
			response = handle_challengesignup(gmtoken, user, sid);
		}
		else if (cmd == "addgoldcaptain")
		{
			response = handle_addgoldcaptain(gmtoken, user, sid);
		}
		else if (cmd == "syncgoldcaptain")
		{
			response = handle_syncgoldactvity(gmtoken, user, sid);
		}
		else if (cmd == "synclimitship")
		{
			response = handle_synclimitship(gmtoken, user, sid);
		}
		else if (cmd == "tuangouopt")
		{
			response = handle_tuangouopt(gmtoken, user, sid);
		}
		else if (cmd == "activitymailopt")
		{
			response = handle_activitymailopt(gmtoken, user, sid);
		}
		else if (cmd == "unifyuid")
		{
			response = handle_unifyuid(gmtoken, user, sid);
		}
		else if(cmd == "setgifttime")
		{
			response = handle_setgifttime(gmtoken,user,sid);
		}
		else if(cmd == "setwaittime")
		{
			response = handle_setWaitGiftTime(gmtoken,user,sid);
		}
		else if (cmd == "clearchargebackgift")
		{
			response = handle_clearchargebackgift(gmtoken, user, sid);
		}
		else if (cmd == "chargebackgiftopt")
		{
			response = handle_chargebackgiftopt(gmtoken, user, sid);
		}
		else if (cmd == "autosoulopt")
		{
			response = handle_autosoulopt(gmtoken, user, sid);
		}
		else if (cmd == "addguildmember")
		{
			response = handle_addguildmember(gmtoken, user, sid);
		}
		if (response == "")
            response = errorContent("Cmd Not Found", ERR_WRONG_PARAMTER, sid);

        dh->markUserDirty(user);        
    } while (0);

    e->set_state(Game_to_Web);
    e->set_content(response);
    int webfd = e->mutable_forward()->serverfdfrom();
    e->mutable_forward()->set_serverfdto(webfd);
    eh_->sendEventToWorld(e);

    LOG4CXX_INFO(logger_, "Response GM: " << e->content());
}

string GMHandler::errorContent(const string& msg, int errorCode, const string& sid)
{
    ptree pt;
    pt.put("msg", msg);
    pt.put("errorCode", errorCode);
    pt.put("sid", sid);

    stringstream ssm;
    write_json(ssm, pt, false);
    return ssm.str();
}

string GMHandler::failContent(const string& sid)
{
    return errorContent("Failed", -1, sid);
}

string GMHandler::formatErrorContent(const string& sid)
{
    return errorContent("Format", -1, sid);
}

string GMHandler::successContent(const string& sid)
{
    ptree pt;
    pt.put("msg", "success");
    pt.put("sid", sid);

    stringstream ssm;
    write_json(ssm, pt, false);
    return ssm.str();
}

string GMHandler::successContent( const string& sid,const vector<string>& params )
{
	ptree pt;
	char title[10] = {0};
	pt.put("msg", "success");
	for (int i=0; i<(int)params.size(); ++i)
	{
		sprintf(title, "param%d", i);
		pt.put(title, params[i]);
	}
	pt.put("sid", sid);
	stringstream ssm;
	write_json(ssm, pt, false);
	return ssm.str();
}
string GMHandler::handle_setmoney(const vector<string>& gmtoken, User* user, string& sid)
{
    if (gmtoken.size() <= 3)
        return formatErrorContent(sid);

    int resType = getInt(gmtoken[2]);
    int num = getInt(gmtoken[3]);
    if ( user->GetPlayer()->AddMoney(resType, num, GM_AddMoney_Add))
	{
		if (user->Online())
			user->GetPlayer()->SendUserData();		
        return successContent(sid);
	}
	
    return errorContent("Failed", -1, sid);
}

string GMHandler::handle_costmoney(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int resType = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if ( user->GetPlayer()->CostMoney(resType, num, GM_Cost))
	{
		if (user->Online())
			user->GetPlayer()->SendUserData();		
		return successContent(sid);
	}

	return errorContent("Failed", -1, sid);
}

int64 GMHandler::getUid(const string& str)
{
    int64 uid = 0;
    safe_atoll(str.substr(1), uid);
    return uid;
}

int64 GMHandler::getInt64(const string& str)
{
	int64 uid = 0;
	safe_atoll(str, uid);
	return uid;
}

int GMHandler::getInt(const string& str)
{
    int i = 0;
    safe_atoi(str, i);
    return i;
}

string GMHandler::getSid(const vector<string>& gmtoken)
{
    return gmtoken[gmtoken.size()-1];
}

string GMHandler::handle_reload( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() <= 1)
		return formatErrorContent(toString(Daemon::Instance().gameid()));

	GameConfigMgr::GetInst()->LoadGameConfigInfo();
	
	
	return successContent(toString(Daemon::Instance().gameid()));
}

string GMHandler::handle_addmail( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	string text = gmtoken[2];
	vector<int> ids;
	vector<int> counts;
	if (gmtoken.size() == 6 || gmtoken.size() == 8 || gmtoken.size() == 10)
	{
		int id = getInt(gmtoken[3]);
		int count = getInt(gmtoken[4]);
		if (id > 0 && count > 0)
		{
			ids.push_back(id);
			counts.push_back(count);
		}
	}
	if (gmtoken.size() == 8 || gmtoken.size() == 10)
	{
		int id = getInt(gmtoken[5]);
		int count = getInt(gmtoken[6]);
		if (id > 0 && count > 0)
		{
			ids.push_back(id);
			counts.push_back(count);
		}
	}
	if (gmtoken.size() == 10)
	{
		int id = getInt(gmtoken[7]);
		int count = getInt(gmtoken[8]);
		if (id > 0 && count > 0)
		{
			ids.push_back(id);
			counts.push_back(count);
		}
	}
	DB_Mail* pMail = user->GetPlayer()->GetMailMgr()->AddMail(text, ids, counts, MAIL_GM_ADD);
	if ( pMail )
	{
		//if (user->Online())
		//	user->GetPlayer()->SendAddMail(pMail, ids.size()>0 ? 2 : 1);
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addsysmail(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int nMailID = getInt(gmtoken[2]);
	//最多8个参数
	vector<string> vecParamList;
	for (size_t i = 3; i < gmtoken.size()-1&& gmtoken.size() < 4+8; i++)
	{
		vecParamList.push_back(gmtoken[i]);
	}
	DB_Mail* pMail = user->GetPlayer()->GetMailMgr()->AddMail(nMailID, MAIL_GM_ADD, vecParamList);
	if ( pMail )
	{
		//if (user->Online())
		//	user->GetPlayer()->SendAddMail(pMail, pMail->proplist_size() >0 ? 2 : 1);
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delmail( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int type = getInt(gmtoken[2]);
	int64 mailid = StringToUid(gmtoken[3]);	
	if (user->GetPlayer()->GetMailMgr()->DelMail(mailid, type, MAIL_GM_DEL))
		return successContent(sid);
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addprop( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int propid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPropMgr()->addProp(propid, PROP_GM_ADD, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPropList();
		return successContent(sid);
	}
	
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delprop( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int propid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPropMgr()->delProp(propid, PROP_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPropList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addpaper( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int paperid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPaperMgr()->addPaper(paperid, PAPER_GM_ADD, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPaperList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delpaper( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int paperid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPaperMgr()->delPaper(paperid, PAPER_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPaperList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addship( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int shipid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	bool res = false;
	for (int i=0; i<num; i++)
	{
		DB_Ship* pShip = user->GetPlayer()->GetShipMgr()->AddShip(shipid, "", SHIP_GM_ADD);
		if (pShip == NULL)
			continue;
		res = true;
		if (user->Online())
			user->GetPlayer()->SendAddShip(pShip);
	}
	if (res)
		return successContent(sid);
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delship( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	int64 shipid = StringToUid(gmtoken[2]);
	bool res = user->GetPlayer()->GetShipMgr()->DelShipForce(shipid, SHIP_GM_COST);
	if (res)
	{
		if (user->Online())
			user->GetPlayer()->SendDelShip(shipid);
		return successContent(sid);
	}
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addparts( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int partsid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	bool res = false;
	for (int i=0; i<num; i++)
	{
		DB_Parts* pParts = user->GetPlayer()->GetPartsMgr()->addParts(partsid, PARTS_GM_ADD);
		if (pParts == NULL)
			continue;
		res = true;
		if (user->Online())
			user->GetPlayer()->SendAddParts(pParts);
	}
	if (res)
		return successContent(sid);
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delparts( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	int64 partsid = StringToUid(gmtoken[2]);
	DB_Player* dbPlayer = user->GetDBPlayer();
	int64 id = 0;
	for (int i=0; i<dbPlayer->partslist_size(); ++i)
	{
		DB_Parts* part = dbPlayer->mutable_partslist(i);
		if (part != NULL && part->partsid() == partsid)
		{
			id = part->id();
			bool res = user->GetPlayer()->GetPartsMgr()->delPartsForce(id, PARTS_GM_COST);
			if (res)
			{
				if (user->Online())
					user->GetPlayer()->SendDelParts(id);
				return successContent(sid);
			}
		}
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_setlevel( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	int level = getInt(gmtoken[2]);
	user->GetPlayer()->SetLevel(level);
	PlayerBaseInfoModuleInst::instance().updateLevel(user->uid(), level);
	if(user->GetPlayer()->GetLevel() >= 6)
	{
		LevelRankKey key(user->GetPlayer()->GetLevel(),user->GetPlayer()->CalcPower(),user->uid());
		RankingModuleInst::instance().UpdateLevelRanking(user->uid(),key);
	}
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_winstage( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int stageid = getInt(gmtoken[2]);
	int star = getInt(gmtoken[3]);
	CFG_Stage* pModel = StageTblInst::instance().GetInfo(stageid);
	if (pModel)
	{
		user->GetPlayer()->GetStageMgr()->addStageCount(stageid,star);
		if (user->Online())
			user->GetPlayer()->GetStageMgr()->sendStageData(stageid);
	}
	else
	{
		CFG_AdvStage* pAdvModel = AdvStageTblInst::instance().GetInfo(stageid);
		if (pAdvModel == NULL)
			return errorContent("Failed", -1, sid);
		user->GetPlayer()->GetAdvStageMgr()->addStageCount(stageid,star);
		if (user->Online())
			user->GetPlayer()->GetAdvStageMgr()->sendStageData(stageid);
	}
	return successContent(sid);
}

string GMHandler::handle_clearcopybytype( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int type = getInt(gmtoken[2]);
	if (type < 1 || type > 3) {
		return formatErrorContent(sid);
	}
	DB_PlotCopyMgr * pMgr = user->GetPlayer()->GetPlotCopyWarMgr()->GetDBMgr();

	for(int i = 0;i < pMgr->copywars_size(); i ++) {
		DB_PlotCopyWar * pUit = pMgr->mutable_copywars(i);
		if (pUit && pUit->active_id() == type) {
	
				if (pMgr->copywars_size() == 1) {
					pMgr->mutable_copywars()->RemoveLast();
				}
				else {
					pMgr->mutable_copywars()->SwapElements(i, pMgr->copywars_size() - 1);
					pMgr->mutable_copywars()->RemoveLast();
				}

		}
	}
	return successContent(sid);
}

string GMHandler::handle_losestage( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int stageid = getInt(gmtoken[2]);
	bool res = false;
	CFG_Stage* pModel = StageTblInst::instance().GetInfo(stageid);
	if (pModel)
	{
		if (user->GetPlayer()->GetStageMgr()->delStage(stageid))
			res = true;
	}
	else
	{
		CFG_AdvStage* pAdvModel = AdvStageTblInst::instance().GetInfo(stageid);
		if (pAdvModel == NULL)
			return errorContent("Failed", -2, sid);
		if (user->GetPlayer()->GetAdvStageMgr()->delStage(stageid))
			res = true;
	}
	if (res)
		return successContent(sid);
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int pieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPaperMgr()->addPaperPiece(pieceid, PAPERPIECE_GM_ADD, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPaperPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int pieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPaperMgr()->delPaperPiece(pieceid, PAPERPIECE_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPaperPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_broadcast( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	eh_->SendBroadCast(BROADCAST_TYPE_SYSTEM, gmtoken[2]);
	return successContent(sid);
}

string GMHandler::handle_clearguardcount( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	user->GetPlayer()->GetGuardMgr()->ClearTodayData();
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_cleararenacount( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	user->GetDBPlayer()->mutable_arena()->clear_todaycount();
	if (user->Online())
		user->GetPlayer()->SendArenaData();
	return successContent(sid);
}

string GMHandler::handle_setviplevel( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int level = getInt(gmtoken[2]);
	user->GetPlayer()->SetVIPLevel(level);
// 	int randnum = 1000;
// 	CFG_Vip* pVip = VipTblInst::instance().GetInfo(level);
// 	CFG_Vip* pNextVip = VipTblInst::instance().GetInfo(level + 1);
// 	int needCash = 0;
// 	if (pVip != NULL)
// 	{
// 		needCash = pVip->get_costCash();
// 		int totalbuy = user->GetDbUser().totalbuy();
// 		if (totalbuy > needCash)
// 			needCash = totalbuy;
// 		if (pNextVip != NULL)
// 			randnum = pNextVip->get_costCash() - needCash;
// 		int addnum = rand() % randnum;
// 		needCash += addnum;
// 		needCash = needCash + 10 - (needCash % 10);
// 	}
// 	user->GetDbUser().set_totalbuy(needCash);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_addexp( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int exp = getInt(gmtoken[2]);
	bool res = user->GetPlayer()->AddExp(exp, EXP_GM_Add);
	if (res)
	{
		if (user->Online())
			user->GetPlayer()->SendUserData();
		user->GetPlayer()->SendTaskRewardData();
		return successContent(sid);
	}
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_skipguide( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDBPlayer()->set_guidestep(150);
	return successContent(sid);
}

string GMHandler::handle_addjuniorpaper( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int count = getInt(gmtoken[2]);
	user->GetPlayer()->AddJuniorPaper(count, GM_Add);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_addseniorpaper( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int count = getInt(gmtoken[2]);
	user->GetPlayer()->AddSeniorPaper(count, GM_Add);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_clearroleinfo(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDbUser().clear_head();
	user->GetDbUser().clear_name();
	user->GetDbUser().clear_camp();
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_clearuid(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	GameDataHandler* dh = eh_->getDataHandler();
	const string& pid = user->openid();
	int64 uid = 0;
	dh->mapPlatidToUid(pid, uid);
	user->GetDbUser().set_openid("");
	return successContent(sid);
}

string GMHandler::handle_addmonthcard(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int id = getInt(gmtoken[2]);
	CFG_Cash* pModel = CashTblInst::instance().GetInfo(id);
	if (!pModel)
		return formatErrorContent(sid);
	if (pModel->get_dayCount() <= 0)
		return formatErrorContent(sid);
	else
	{
		user->GetDBPlayer()->set_monthcard_received(false);
		user->GetDBPlayer()->set_monthcard_check(0);
		user->GetDbUser().set_monthcard_id(pModel->get_id());
		
		if (user->GetDbUser().monthcard_date()<Clock::getLocalDay())
			user->GetDbUser().set_monthcard_date(Clock::getLocalDay() + pModel->get_dayCount() -1);
		else
			user->GetDbUser().set_monthcard_date(user->GetDbUser().monthcard_date() + pModel->get_dayCount() -1);
		//if (!user->GetDbUser().has_monthcard_date())
			//user->GetDbUser().set_monthcard_date(Clock::getLocalDay()+pModel->get_dayCount() - 1);
		//else
			//user->GetDbUser().set_monthcard_date(user->GetDbUser().monthcard_date() + pModel->get_dayCount());
		user->GetPlayer()->SendUserData();
		return successContent(sid);
	}
}

string GMHandler::handle_resetmonthcard(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDBPlayer()->set_monthcard_received(false);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);	
}

string GMHandler::handle_funcswitch( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);

	const int switchID = getInt(gmtoken[2]);
	if (switchID > 0)
		FuncSwitchInst::instance().SetDefaultEnable(switchID, getInt(gmtoken[3]) != 0);
	const hash_map<int64, User*>& usermap = eh_->getDataHandler()->getUsers();
	for (hash_map<int64, User*>::const_iterator iter = usermap.begin(); iter != usermap.end(); iter++)
	{
		User* pUser = iter->second;
		if (pUser == NULL || pUser->Online() == false)
			continue;
		pUser->GetPlayer()->SendFuncSwitch();
	}
	return successContent(sid);
}

string GMHandler::handle_clearfirstrecharge(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDbUser().clear_first_recharge();
	user->GetDbUser().clear_monthcard_id();
	user->GetDbUser().clear_monthcard_date();
	user->GetDbUser().clear_totalbuy();
	user->GetDBPlayer()->clear_monthcard_received();
	user->GetPlayer()->SetVIPLevel(0);
	user->GetDBPlayer()->mutable_activity()->set_first_recharge(false);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);	
}

string GMHandler::handle_recharge(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int nType = getInt(gmtoken[2]);
	user->GetPlayer()->Recharge(nType);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);	
}

string GMHandler::handle_clearoncebuy(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDBPlayer()->mutable_shop()->clear_oncebuyid();
	return successContent(sid);	
}

string GMHandler::handle_profile( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int nType = getInt(gmtoken[2]);
	if (nType == 1) // report
		eh_->getEventHandler()->reportProfile();
	else if (nType == 0) // clear
		eh_->getEventHandler()->clearProfile();
	return successContent(sid);
}

string GMHandler::handle_resetstarttime(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	if (gmtoken[2].size() == 1 && gmtoken[2].substr(0,1)=="0")//0就代表设置当前时间
	{
		GlobalModuleInst::instance().ResetStartTime();
	}
	else
	{
		GlobalModuleInst::instance().setStartTime((int)GetTimeVal(gmtoken[2]));
	}
	return successContent(sid);
}

string GMHandler::handle_banlogin( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);
	time_t now = time(NULL);
	DB_BanLogin* pLogin = user->GetDbUser().mutable_banlogin();
	bool bBan = getInt(gmtoken[2]) == 1;
	int hour = getInt(gmtoken[3]);
	if (bBan)
		eh_->removeUserFdMap(user->fd(), user->uid());
	pLogin->set_banlogin(bBan);
	pLogin->set_time(now + hour*3600);
	SYS_UserStat(user, LT_BanLogin, bBan, hour, "", "", "", "", "", "");
	return successContent(sid);
}

string GMHandler::hanlde_setguildlevel(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);
	int64 nGuildId = 0;
	int nLevel = 0;
	safe_atoll(gmtoken[2], nGuildId);
	safe_atoi(gmtoken[3], nLevel);
	DB_GuildData* pData = GuildModuleInst::instance().GetGuildData(nGuildId);
	if (!pData)
		return formatErrorContent(sid);
	pData->set_level(nLevel);
	GuildModuleInst::instance().UpdateLevel(nGuildId, pData->level(), pData->totalcontribute(), pData->memeberlist_size(), pData->camp());
	return successContent(sid);
}

string GMHandler::handle_usermapsize( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	return successContent(toString(eh_->getDataHandler()->getUsers().size()));
}

string GMHandler::handle_virtualcharge(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int id = -1;
	safe_atoi(gmtoken[2], id);
	if (id < 0)
	{
		return formatErrorContent(sid);
	}
	CFG_Cash* pModel = CashTblInst::instance().GetInfo(id);
	if (!pModel)
		return formatErrorContent(sid);

	string name = gmtoken[3];
	vector<string> param;
	param.push_back(name);
	param.push_back( toString(pModel->get_firstGet()) );

	eh_->SendBroadCast(BROADCAST_TYPE_TRIGGER, 4, param, "");

	return successContent(sid);
}

string GMHandler::handle_addpartpiece(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int nPartPieceId = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPartsMgr()->addPartPiece(nPartPieceId, PARTPIECE_GM_ADD, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPaperPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delpartpiece(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int nPartPieceId = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetPartsMgr()->delPartPiece(nPartPieceId, PARTPIECE_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->SendPartPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_setarenascore(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size()<3)
	{
		return formatErrorContent(sid);
	}
	int score = getInt(gmtoken[2]);
	user->GetPlayer()->GetDBPlayer()->mutable_arena()->set_score(score);
	return successContent(sid);
}

string GMHandler::handle_newday(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size()<3)
	{
		return formatErrorContent(sid);
	}
	int today = user->GetPlayer()->GetDBPlayer()->dataday();
	user->GetPlayer()->GetDBPlayer()->set_dataday(today - 100);
	if (user->GetPlayer()->SetNewDayData())
	{
		return successContent(sid);
	}
	else
	{
		int today = user->GetPlayer()->GetDBPlayer()->dataday();
		user->GetPlayer()->GetDBPlayer()->set_dataday(today + 100);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_clearrobberyguide(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	user->GetDBPlayer()->set_firstrobbery(false);
	return successContent(sid);	
}

string GMHandler::handle_fixpartlist(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size()<3)
	{
		return formatErrorContent(sid);
	}
	for (int i = 0; i < user->GetDBPlayer()->soldierlist_size(); i++)
	{
		DB_Soldier* pSoldier = user->GetPlayer()->GetShipMgr()->GetSoldier(i);
		if (!pSoldier)
			continue;
		if (pSoldier->partslist_size() < 4)
		{
			int size = pSoldier->partslist_size();
			for (int j = 0; j < 4 - size; j++)
			{
				pSoldier->add_partslist(0);
			}
		}
	}
	return successContent(sid);
}

string GMHandler::handle_clearstage(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int mod = 0;	//1表示普通关卡，2表示精英关卡
	safe_atoi(gmtoken[2].c_str(),mod);
	int endStage = 0;
	safe_atoi(gmtoken[3].c_str(), endStage);
	if (mod == 1)
	{
		vector<int> v = StageTblInst::instance().GetKeys();
		vector<int> result;
		for (size_t i = 0; i<v.size(); i++)
		{
			if (v[i] < endStage)
			{
				result.push_back(v[i]);
			}	
		}
		if (result.size() == 0)
		{
			return formatErrorContent(sid);
		}
		StageMgr *p_stagemgr = user->GetPlayer()->GetStageMgr();
		if (p_stagemgr == NULL)
		{
			return formatErrorContent(sid);
		}
		for (size_t i=0; i<result.size(); i++)
		{
			p_stagemgr->addStageCount(result[i],3);
			if (user->Online())
			{
				user->GetPlayer()->GetAdvStageMgr()->sendStageData(result[i]);
			}
		}
	}
	else if (mod == 2)
	{
		vector<int> v = AdvStageTblInst::instance().GetKeys();
		vector<int> result;
		for (size_t i = 0; i<v.size(); i++)
		{
			if (v[i] < endStage)
			{
				result.push_back(v[i]);
			}	
		}
		if (result.size() == 0)
		{
			return formatErrorContent(sid);
		}
		AdvStageMgr *p_adv	= user->GetPlayer()->GetAdvStageMgr();
		if (p_adv == NULL)
		{
			return formatErrorContent(sid);
		}
		for (size_t i=0; i<result.size(); i++)
		{
			p_adv->addStageCount(result[i],3);
			if (user->Online())
			{
				user->GetPlayer()->GetAdvStageMgr()->sendStageData(result[i]);
			}
		}
	}
	
	GameDataHandler* dh = eh_->getDataHandler();
	
	dh->markUserDirty(user);
	return successContent(sid);
}

string GMHandler::handle_addjuniorstar( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int star = getInt(gmtoken[2]);
	user->GetPlayer()->GetMedalMgr()->AddJuniorStar(star, GM_AddJuniorStar);
	return successContent(sid);
}

string GMHandler::handle_addseniorstar( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int star = getInt(gmtoken[2]);
	user->GetPlayer()->GetMedalMgr()->AddSeniorStar(star, GM_AddSeniorStar);
	return successContent(sid);
}

string GMHandler::handle_setmedal( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int medal = getInt(gmtoken[2]);
	user->GetDBPlayer()->mutable_medaldata()->set_medal(medal);
	return successContent(sid);
}

string GMHandler::handle_cleargrowup(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	user->GetPlayer()->GetDBPlayer()->mutable_growup()->set_buyflag(false);
	user->GetPlayer()->GetDBPlayer()->mutable_growup()->clear_recv_growupcash_list();
	return successContent(sid);
}

string GMHandler::handle_addguildcontribute(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int64 nGuildId = getInt64(gmtoken[2]);
	int nCount = getInt(gmtoken[3]);
	DB_GuildData* pData = GuildModuleInst::instance().GetGuildData(nGuildId);
	if (pData)
	{
		pData->set_activepoint(pData->activepoint() + nCount);
		return successContent(sid);
	}
	else
	{
		return formatErrorContent(sid);
	}
}

string GMHandler::handle_addusertotalcontribute(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	int64 guildid = user->GetDBPlayer()->mutable_guild()->id();
	DB_GuildData* pData = GuildModuleInst::instance().GetGuildData(guildid);
	if (pData)
	{
		DB_GuildMemberData* member = GuildModuleInst::instance().GetMemberData(user->uid(), guildid);
		member->set_totoalcontribute(member->totoalcontribute() + nCount);
		return successContent(sid);
	}
	else
	{
		return formatErrorContent(sid);
	}
}
string GMHandler::handle_createguild(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	DB_Player* player = user->GetDBPlayer();
	player->mutable_guild()->clear_id();
	int res = user->GetPlayer()->GetGuildMgr()->CreateGuild(gmtoken[2], 1);
	if (res == 0)
	{
		Daemon::Instance().GetGameDataHandler()->markUserDirty(user);
		return successContent(sid);
	}
	return errorContent("failed", res, sid);
}

string GMHandler::handle_addguildmember(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	DB_Player* player = user->GetDBPlayer();
	player->mutable_guild()->clear_id();
	int64 guildid = StringToUid(gmtoken[2]);
	int res = GuildModuleInst::instance().AddMember(user, guildid);
	if (res == 0)
	{
		Daemon::Instance().GetGameDataHandler()->markUserDirty(user);
		return successContent(sid);
	}
	return errorContent("failed", res, sid);
}

string GMHandler::handle_clearguilddeputy(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int64 nGuildId = getInt64(gmtoken[2]);
	DB_GuildData* pData = GuildModuleInst::instance().GetGuildData(nGuildId);
	if (pData)
	{
		DB_GuildMemberData* pMember = GuildModuleInst::instance().GetMemberData(pData->deputychiefid_1(), nGuildId);
		if (pMember)
			pMember->set_pos(GUILD_POS_MEMBER);
		pMember = GuildModuleInst::instance().GetMemberData(pData->deputychiefid_1(), nGuildId);
		if (pMember)
			pMember->set_pos(GUILD_POS_MEMBER);
		pData->set_deputychiefid_1(0);
		pData->set_deputychiefid_2(0);

		return successContent(sid);
	}
	else 
	{
		return formatErrorContent(sid);
	}
}

string GMHandler::handle_addghostshiptime(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	DB_GhostShip* pGhostShip = user->GetDBPlayer()->mutable_activity()->mutable_ghostship();
	pGhostShip->set_totalcount(pGhostShip->totalcount()+nCount);
	int64 now = time(NULL);
	DB_GhostShipData ghostshipdata;
	ghostshipdata.set_score(pGhostShip->totalcount());
	ghostshipdata.set_name(user->getName());
	ghostshipdata.set_time(now);
	ghostshipdata.set_uid(user->uid());
	GhostShipRankkey key(ghostshipdata.uid(), ghostshipdata.time(), ghostshipdata.score());
	GhostShipModuleInst::instance().UpdateScore(ghostshipdata, key);
	return successContent(sid);
}

string GMHandler::handle_addlimitshiptime( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	DB_LimitShip* pLimitShip = user->GetDBPlayer()->mutable_limitship();
	pLimitShip->set_score(pLimitShip->score() + nCount*10);
	int64 now = time(NULL);
	DB_LimitShipData limitshipdata;
	limitshipdata.set_score(pLimitShip->score());
	limitshipdata.set_time(now);
	limitshipdata.set_uid(user->uid());
	LimitShipRankKey key(limitshipdata.score(), limitshipdata.time(), limitshipdata.uid() );
	LimitShipModuleInst::instance().UpdateScore(limitshipdata, key);
	return successContent(sid);
}

string GMHandler::handle_setchattime(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int time = 0;
	safe_atoi(gmtoken[2], time);
	GlobalModuleInst::instance().SetChatTime(time);
	return successContent(sid);
}

string GMHandler::handle_addsoul( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int soulID = getInt(gmtoken[2]);
	DB_Soul* pSoul = user->GetPlayer()->GetSoulMgr()->addSoul(soulID, SOUL_GM_ADD);
	if (pSoul)
	{
		if (user->Online())
			user->GetPlayer()->SendSoulList();
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_delsoul( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int64 soulID = getInt64(gmtoken[2]);
	if (user->GetPlayer()->GetSoulMgr()->delSoulForce(soulID, SOUL_GM_COST))
	{
		if (user->Online())
			user->GetPlayer()->SendSoulList();
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_expsoul( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 5)
		return formatErrorContent(sid);
	int64 soulID = getInt64(gmtoken[2]);
	int exp = getInt(gmtoken[3]);
	DB_Soul* pSoul = user->GetPlayer()->GetSoulMgr()->getSoul(soulID);
	if (pSoul == NULL)
		return failContent(sid);
	pSoul->set_point(exp);
	if (user->Online())
		user->GetPlayer()->SendSoulList();
	return successContent(sid);
}

string GMHandler::handle_addawardcenter(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 8)
		return formatErrorContent(sid);
	int nAwardCenterID = 0;
	safe_atoi(gmtoken[2], nAwardCenterID);
	int nSize = (gmtoken.size()-5)/3;
	DropList droplist;
	for (int i = 1; i <= nSize; i++)
	{
		int type = 0;
		int id = 0;
		int count = 0;
		safe_atoi(gmtoken[i*3], type);
		safe_atoi(gmtoken[i*3+1], id);
		safe_atoi(gmtoken[i*3+2], count);
		DropData* pData = droplist.add_droplist();
		pData->set_type(type);
		pData->set_id(id);
		pData->set_count(count);
	}
	vector<string> vParam;
	int fromtype = 0;
	safe_atoi(gmtoken[gmtoken.size() - 2], fromtype);
	user->GetPlayer()->GetAwardCenterMgr()->AddAward(nAwardCenterID, &droplist, vParam, (PLAYER_ADD_MONEY_OPTYPE)fromtype);
	return successContent(sid);
}

string GMHandler::handle_addaward(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 9)
		return formatErrorContent(sid);
	int nSize = (gmtoken.size()-6)/3;
	DropList droplist;
	for (int i = 1; i <= nSize; i++)
	{
		int type = 0;
		int id = 0;
		int count = 0;
		safe_atoi(gmtoken[i*3+1], type);
		safe_atoi(gmtoken[i*3+2], id);
		safe_atoi(gmtoken[i*3+3], count);
		DropData* pData = droplist.add_droplist();
		pData->set_type(type);
		pData->set_id(id);
		pData->set_count(count);
	}
	vector<string> vParam;
	vParam.push_back(gmtoken[2]);
	vParam.push_back(gmtoken[3]);
	int fromtype = 0;
	safe_atoi(gmtoken[gmtoken.size() - 2], fromtype);
	user->GetPlayer()->GetAwardCenterMgr()->AddAward(0 , &droplist, vParam, (PLAYER_ADD_MONEY_OPTYPE)fromtype);
	return successContent(sid);
}

string GMHandler::handle_battletest( const vector<string>& gmtoken )
{
	if (gmtoken.size() < 5)
		return formatErrorContent("");
	int64 attackID = getInt64(gmtoken[2]);
	int64 defendID = getInt64(gmtoken[3]);
	int	count = getInt(gmtoken[4]);
	LoadStatus loadStatus = LOAD_INVALID;
	User* user = Daemon::Instance().GetGameDataHandler()->TestGetUser(attackID, &loadStatus);
	if (user == NULL)
		return "";
	loadStatus = LOAD_INVALID;
	User* targetUser = Daemon::Instance().GetGameDataHandler()->TestGetUser(defendID, &loadStatus);
	if (targetUser == NULL)
		return "";
	TacticInfo atkTac;
	TacticInfo defTac;
	user->GetPlayer()->FillTactic(&atkTac);
	targetUser->GetPlayer()->FillTactic(&defTac);
	int win = 0;
	int round = 0;
	int atkHPCost = 0;
	int defHPCost = 0;
	int atkDeads = 0;
	int defDeads = 0;
	string atcHpState;
	string defHpState;
	for (int i=0; i<count; i++)
	{
		BattleInfo info;
		BattleMgr battle(atkTac, defTac);
		bool bWin = battle.StartBattle(&info);
		if (bWin)
			win++;
		round += info.roundlist_size();
		atkHPCost += battle.GetCostBloodPer(true);
		defHPCost += battle.GetCostBloodPer(false);
		atkDeads += battle.GetDeadShipCount(true);
		defDeads += battle.GetDeadShipCount(false);
		
		atcHpState = "";
		defHpState = "";
		atcHpState = battle.GetAllShipHpStateForGM(true);
		defHpState = battle.GetAllShipHpStateForGM(false);
	}

	ptree pt;
	pt.put("msg", "success");
	pt.put("win", win);
	pt.put("round", round/count);
	pt.put("attackerLevel", user->GetUserLevel());
	pt.put("defenderLevel", targetUser->GetUserLevel());
	pt.put("attackerPower", user->GetPlayer()->CalcPower());
	pt.put("defenderPower", targetUser->GetPlayer()->CalcPower());
	pt.put("attackerHPCost", atkHPCost/count);
	pt.put("defenderHPCost", defHPCost/count);
	pt.put("attackerDeadShips", atkDeads/count);
	pt.put("defenderDeadShips", defDeads/count);
	pt.put("attackerHpState", atcHpState);
	pt.put("defenderHpState", defHpState);

	stringstream ssm;
	write_json(ssm, pt, false);
	return ssm.str();
}

string GMHandler::handle_setcampbattlewinning(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() < 5)
		return formatErrorContent(sid);
	int nCamp = 0;
	int nCount = 0;
	safe_atoi(gmtoken[2], nCamp);
	safe_atoi(gmtoken[3], nCount);
	CampBattleModuleInst::instance().SetWinning(nCamp, nCount);
	return successContent(sid);
}

string GMHandler::handle_setcampbattledate(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int time = (int)GetTimeVal(gmtoken[2]);
	int nDay = Clock::getLocalDay(time);
	CampBattleModuleInst::instance().SetDate(nDay);
	return successContent(sid);
}

string GMHandler::handle_setmorale(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int nMorale = 0;
	safe_atoi(gmtoken[2], nMorale);
	user->GetPlayer()->GetCampBattleMgr()->CostMorale(nMorale, user->GetPlayer(), 31);
	user->GetPlayer()->GetCampBattleMgr()->SendCampBattleData();
	return successContent(sid);
}

string GMHandler::handle_setport(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int nPort = 0;
	safe_atoi(gmtoken[2], nPort);
	int nCurPort = CampBattleModuleInst::instance().GetPosition(user->uid());
	CampBattleModuleInst::instance().LeavePort(user->uid(), nCurPort);
	CampBattleModuleInst::instance().EnterPort(user->uid(), nPort, user->getCamp());
	user->GetPlayer()->GetCampBattleMgr()->SendCampBattleData();
	return successContent(sid);
}

string GMHandler::handle_clearattackcount(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	user->GetDBPlayer()->mutable_campbattle()->set_attacknum(0);
	return successContent(sid);
}

string GMHandler::handle_clearluckygoddess( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	user->GetDBPlayer()->clear_lastluckygoddessfreetime();
	return successContent(sid);
}

string 
GMHandler::handle_Set_MilitaryRankType(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3) {
		return formatErrorContent(sid);
	}

	int militaryType = 0;
	safe_atoi(gmtoken[2], militaryType);

	MilitaryRankCfgUint* pUint = MilltaryRankMgrInst::instance().GetInfo(militaryType);
	if(pUint == NULL) {
		return formatErrorContent(sid);
	}

	user->GetDBPlayer()->mutable_militaryrankinfo()->set_militaryranktype(militaryType);
	DB_PlayerBaseInfoData* pBase = PlayerBaseInfoModuleInst::instance().getBaseInfoByUid(user->uid());
	if (pBase) {
		pBase->set_militaryranktype(militaryType);
	}
	vector<int> rightVec;
	MilltaryRankMgrInst::instance().GetRightVec(militaryType, rightVec);
	if(MilitaryRankRightsTblInst::instance().CheckHasRight(All_ShipLife_Add, rightVec)) {
		user->GetPlayer()->GetShipMgr()->CalcAllShips();
	}
	user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string 
GMHandler::handle_Set_MilitaryRankHon(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3) {
		return formatErrorContent(sid);
	}

	int militaryHon = 0;
	safe_atoi(gmtoken[2], militaryHon);
	militaryHon = militaryHon > 0 ? militaryHon : 0;


	user->GetDBPlayer()->mutable_militaryrankinfo()->set_honour(militaryHon);
	user->GetPlayer()->SendUserData();
	return successContent(sid);

}

string
GMHandler::handle_Clear_MilitaryRankRewradTime(const vector<string>& gmtoken, User* user, string& sid)
{
	user->GetDBPlayer()->mutable_militaryrankinfo()->clear_rewardtime();
	return successContent(sid);
}

string GMHandler::handle_refreshmisteryshop(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	user->GetPlayer()->GetActivityMgr()->RefreshMisteryShop(1);
	return successContent(sid);
}

string GMHandler::handle_setrefreshtimes(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int nCount = 0;
	safe_atoi(gmtoken[2], nCount);
	user->GetDBPlayer()->mutable_activity()->mutable_misteryshop()->set_freetimes(nCount);
	return successContent(sid);
}

string 
GMHandler::handle_Set_MilitaryRankCampCount(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3) {
		return formatErrorContent(sid);
	}

	int militaryHon = 0;
	safe_atoi(gmtoken[2], militaryHon);
	militaryHon = militaryHon > 120 ? 120 : militaryHon;


	user->GetDBPlayer()->mutable_militaryrankinfo()->set_campbattle_count(militaryHon);
	return successContent(sid);
}

string GMHandler::handle_christmasfree(const vector<string>& gmtoken, User* user, string& sid)
{
    if (gmtoken.size() < 3)
    {
        return formatErrorContent(sid);
    }
    user->GetPlayer()->GetDBPlayer()->set_christmasfreecount(0);
    return successContent(sid);
}

string GMHandler::handle_christmasscore(const vector<string>& gmtoken, User* user, string& sid)
{
    if (gmtoken.size() < 5)
    {
        return formatErrorContent(sid);
    }

    int nCount = 0;
    safe_atoi(gmtoken[2], nCount);

    int type = 0;
    safe_atoi(gmtoken[3], type);

    user->GetPlayer()->AddToken((ShipTokenType)type,nCount);

    return successContent(sid);
}

string GMHandler::handle_clearrechargereward(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	user->GetDBPlayer()->mutable_activity()->mutable_rechargereward()->clear_rechargelist();
    return successContent(sid);
}

string GMHandler::handle_fixname( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int ret = PlayerBaseInfoModuleInst::instance().DelName4Fix(gmtoken[2]);
	return successContent(toString(ret));
}

string GMHandler::handle_setgoddesspoint( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int num = 0;
	safe_atoi(gmtoken[2], num);
	user->GetPlayer()->GetDBPlayer()->set_luckygoddess_point(num);
	return successContent(sid);
}

string GMHandler::handle_setlogouttime(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	time_t timelogout = (int)GetTimeVal(gmtoken[2]);
	 user->setLastLogoutTime(timelogout);
	return successContent(sid);
}

string GMHandler::handle_addluckboxexchangepoint(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int luckpoint = 0;
	safe_atoi(gmtoken[2], luckpoint);
	int point = user->GetDBPlayer()->mutable_luckbox_exchange()->exchangepoint() + luckpoint;
	point = point > 0 ? point : 0;
	user->GetDBPlayer()->mutable_luckbox_exchange()->set_exchangepoint(point);
	return successContent(sid);
}

string GMHandler::handle_clearloginpokerday(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int day = 0;
	safe_atoi(gmtoken[2], day);

	if (day < 1 || day > 8)
	{
		return formatErrorContent(sid);
	}

	for(int i = 0; user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->pokers_size(); i ++) {
		if (user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->pokers_size() == 1)
		{
			user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->mutable_pokers()->RemoveLast();
			return successContent(sid);
		}
		if (user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->pokers(i).theday() == day ) {
			//DB_PokerUnit* pUnit = user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->mutable_pokers(i);
			user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->mutable_pokers()->SwapElements(i, user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->pokers_size() - 1);
			user->GetPlayer()->GetLoginPokerMgr()->GetDBLoginPoker()->mutable_pokers()->RemoveLast();
			return successContent(sid);
		}
	}
	return formatErrorContent(sid);
}

string GMHandler::handle_addloginpokerbroadcast(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int index = 0;
	safe_atoi(gmtoken[2], index);
	vector<string > vParam;
	vParam.push_back(user->GetPlayer()->GetParent()->getName());
	vParam.push_back(gmtoken[3]);
	user->GetPlayer()->SendBroadCast(index, vParam);
	return successContent(sid);
}

string GMHandler::handle_guildchangechief( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 5)
		return formatErrorContent(sid);
	int64 guildid = 0;
	safe_atoll(gmtoken[2], guildid);
	int64 newchief = 0;
	safe_atoll(gmtoken[3], newchief);
	GuildModuleInst::instance().ChangeChief(guildid, newchief, false);
	return successContent(sid);
}

string GMHandler::handle_setguide( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int step = 0;
	safe_atoi(gmtoken[2], step);
	user->GetDBPlayer()->set_guidestep(step);
	return successContent(sid);
}

string GMHandler::handle_cleanspydiatime(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}

	DB_Spy* pData = user->GetPlayer()->GetDBPlayer()->mutable_spy();
	pData->set_diamondmidtime(0);
	pData->set_diamondseniortime(0);
	return successContent(sid);
}

string GMHandler::handle_cleanrecruitdiatime(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}

	DB_Recruit* pData = user->GetPlayer()->GetDBPlayer()->mutable_recruit();
	pData->set_diamondjuniortime(0);
	pData->set_diamondseniortime(0);
	return successContent(sid);
}

string 
GMHandler::handle_cleansoulpaperflag(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}

	user->GetPlayer()->GetDBPlayer()->mutable_soulpaper_data()->Clear();
	user->GetPlayer()->GetDBPlayer()->mutable_soulpaper_data()->set_hasinit(false);

	return successContent(sid);
}

string 
GMHandler::handle_cleanshipformation(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}

	user->GetPlayer()->GetDBPlayer()->mutable_formation_data()->Clear();

	return successContent(sid);
}

string GMHandler::handle_addjuniorcaptain( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int count = getInt(gmtoken[2]);
	user->GetPlayer()->AddJuniorCaptain(count, CAPTAINSOUL_GM_ADD);
	if (user->Online())
		user->GetPlayer()->sendcaptainData();
	return successContent(sid);
}

string GMHandler::handle_addseniorcaptain( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int count = getInt(gmtoken[2]);
	user->GetPlayer()->AddSeniorCaptain(count, CAPTAINSOUL_GM_ADD);
	if (user->Online())
		user->GetPlayer()->sendcaptainData();
	return successContent(sid);
}
//addcaptain,(u)id,id,count,sid
string GMHandler::handle_addcaptain( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);

	int captainid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	bool suc = false;
	for (int i=0; i<num; ++i)
	{
		suc = user->GetPlayer()->GetCaptainMgr()->addCaptain(captainid, CAPTAIN_GM_Add);
		if (suc == false)
			break;
	}
	if (suc)
	{
		if (user->Online())
		{
			user->GetPlayer()->sendCaptainList();
		}
		return successContent(sid);
	}
	return errorContent("failed", -1, sid);	
}

//delcaptain,(u)id,id,sid
string GMHandler::handle_delcaptain( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int captainid = getInt(gmtoken[2]);
	bool suc = false;
	suc = user->GetPlayer()->GetCaptainMgr()->delCaptainForce(captainid, CAPTAIN_GM_DEL);
	if (suc)
	{
		if (user->Online())
		{
			user->GetPlayer()->sendCaptainList();
		}
		return successContent(sid);
	}
	return errorContent("failed", -1, sid);	
}

string GMHandler::handle_addcaptainpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);

	int captainpieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetCaptainMgr()->addCaptainPieces(captainpieceid, CAPTAINPIECE_GM_Add, num))
	{
		if (user->Online())
			user->GetPlayer()->sendCaptainPieceList();
		return successContent(sid);
	}
	return errorContent("failed", -1, sid);
}

string GMHandler::handle_delcaptainpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	//delcaptainpiece,uid,captainpieceid,num,sid
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);

	int captainpieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetCaptainMgr()->costCaptainPieces(captainpieceid, CAPTAINPIECE_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->sendCaptainPieceList();
		return successContent(sid);
	}
	return errorContent("failed", -1, sid);
}

string GMHandler::handle_addprestige(const vector<string>& gmtoken, User* user, string& sid)
{
    if (gmtoken.size() <= 3)
        return formatErrorContent(sid);

    int num = getInt(gmtoken[2]);
    user->GetPlayer()->AddPrestige(num,GM_AddMoney_Add);
    if (user->Online())
        user->GetPlayer()->SendUserData();
    return successContent(sid);

}

string GMHandler::handle_setplattouid(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	GameDataHandler* dh = eh_->getDataHandler();
	const string pid = user->openid();
	user->GetDbUser().set_openid("");
	dh->LoadPlatFromBack(pid);
	return successContent(sid);
}

string GMHandler::handle_loadguildfromold(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	GuildModuleInst::instance().LoadFromBack();
	return successContent(sid);
}

string GMHandler::handle_checkuserguildid( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);
	vector<string> guilds;
	guilds.push_back(toString(user->GetDBPlayer()->mutable_guild()->id()));
	GuildModuleInst::instance().getguildofuser(user->uid(), guilds);
	return successContent(sid, guilds);
}

string GMHandler::handle_deleteguildmember(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int64 nGuildID = getInt64(gmtoken[2]);
	DB_GuildData* pData = GuildModuleInst::instance().GetGuildData(nGuildID);
	if (pData)
	{
		if (pData->chiefid() == user->uid())
			return errorContent("Guild Chief", 1, sid);
		if (pData->deputychiefid_1() == user->uid())
			pData->set_deputychiefid_1(0);
		if (pData->deputychiefid_2() == user->uid())
			pData->set_deputychiefid_2(0);
		string guildname = "";
		DB_PlayerBaseInfoData* pData = PlayerBaseInfoModuleInst::instance().getBaseInfoByUid(user->uid());
		if (pData)
			guildname = pData->guildname();
		GuildModuleInst::instance().DelMember(user, nGuildID);
		PlayerBaseInfoModuleInst::instance().updateGuildName(user->uid(), guildname);
		return successContent(sid);
	}
	else
		return errorContent("No Such Guild!", 2, sid);
}


string GMHandler::handle_clearfirstbuyrecruitaward(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	if(!user)
		return formatErrorContent(sid);
	user->GetDBPlayer()->mutable_recruit()->set_juniorfirstbuyaward(false);
	user->GetDBPlayer()->mutable_recruit()->set_seniorfirstbuyaward(false);
	user->GetDBPlayer()->mutable_recruit()->set_usecashbuy2recruit(false);
	user->GetDBPlayer()->mutable_recruit()->set_usecashbuy3recruit(false);
	return successContent(sid);
}

string GMHandler::handle_addexploit(const vector<string>& gmtoken, User* user, string& sid)
{
    if (gmtoken.size() <= 3)
        return formatErrorContent(sid);

    int num = getInt(gmtoken[2]);
    user->GetPlayer()->AddExploit(num,GM_AddMoney_Add);
    if (user->Online())
        user->GetPlayer()->SendUserData();
    return successContent(sid);
}

string GMHandler::handle_addSignGuildContribute( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int contribute = getInt(gmtoken[2]);
	int res = GuildBattleModuleInst::instance().addSignGuildContribute(user->GetDBPlayer()->mutable_guild()->id(), contribute);
	if (res == 0)
	{
		return successContent(sid);
	}
	return errorContent("Add Error!", res, sid);
}

string GMHandler::handle_clearguildbattle( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int time = (int)GetTimeVal(gmtoken[2]);
	int nDay = Clock::getLocalDay(time);
	GuildBattleModuleInst::instance().SetDate(nDay);
	return successContent(sid);
}

string GMHandler::handle_clearplayerguildbattle( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	user->GetDBPlayer()->mutable_guildbattle()->set_date(0);
	return successContent(sid);
}

string GMHandler::handle_cleararmadamoduledata( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	ArmadaModuleInst::instance().ClearData();
	ArmadaModuleInst::instance().SetActivityEndTime(0);
	ArmadaModuleInst::instance().MarkDirty();
	return successContent(toString(Daemon::Instance().gameid()));
}

string GMHandler::handle_addawardbygiftid(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 6)
		return formatErrorContent(sid);
	int paramsize = gmtoken.size() - 6;
	vector<string> vParam;
	for (int i = 4; i < paramsize + 4; i++)
	{
		vParam.push_back(gmtoken[i]);
	}
	int fromtype = 0;
	safe_atoi(gmtoken[gmtoken.size() - 2], fromtype);
	int index = 0;
	safe_atoi(gmtoken[2], index);
	int giftid = 0;
	safe_atoi(gmtoken[3], giftid);
	DropList droplist;
	GiftTblInst::instance().FillGiftList(giftid, &droplist);
	user->GetPlayer()->GetAwardCenterMgr()->AddAward(index , &droplist, vParam, (PLAYER_ADD_MONEY_OPTYPE)fromtype);
	return successContent(sid);
}

string GMHandler::handle_delhaoduostage( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int mod = 0;	//1表示普通关卡，2表示精英关卡
	safe_atoi(gmtoken[2].c_str(),mod);
	int endStage = 0;
	safe_atoi(gmtoken[3].c_str(), endStage);
	if (mod == 1)
	{
		vector<int> v = StageTblInst::instance().GetKeys();
		vector<int> result;
		for (size_t i = 0; i<v.size(); i++)
		{
			if (v[i] < endStage)
			{
				result.push_back(v[i]);
			}	
		}
		if (result.size() == 0)
		{
			return formatErrorContent(sid);
		}
		StageMgr *p_stagemgr = user->GetPlayer()->GetStageMgr();
		if (p_stagemgr == NULL)
		{
			return formatErrorContent(sid);
		}
		for (size_t i=0; i<result.size(); i++)
		{
			p_stagemgr->delStage(result[i]);
			if (user->Online())
			{
				user->GetPlayer()->GetAdvStageMgr()->sendStageData(result[i]);
			}
		}
	}
	else if (mod == 2)
	{
		vector<int> v = AdvStageTblInst::instance().GetKeys();
		vector<int> result;
		for (size_t i = 0; i<v.size(); i++)
		{
			if (v[i] < endStage)
			{
				result.push_back(v[i]);
			}	
		}
		if (result.size() == 0)
		{
			return formatErrorContent(sid);
		}
		AdvStageMgr *p_adv	= user->GetPlayer()->GetAdvStageMgr();
		if (p_adv == NULL)
		{
			return formatErrorContent(sid);
		}
		for (size_t i=0; i<result.size(); i++)
		{
			p_adv->delStage(result[i]);
			if (user->Online())
			{
				user->GetPlayer()->GetAdvStageMgr()->sendStageData(result[i]);
			}
		}
	}

	GameDataHandler* dh = eh_->getDataHandler();

	dh->markUserDirty(user);
	return successContent(sid);
}
string GMHandler::handle_setarmadabattletimes(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int hasBattleCount = 0;
	safe_atoi(gmtoken[2], hasBattleCount);
	int maxBattleCount = user->GetPlayer()->GetArmadaMgr()->GetMaxChallengeTime();

	DB_Armada*pDb = user->GetPlayer()->GetArmadaMgr()->GetDBArmada();
	if(!pDb)
		return formatErrorContent(sid);

	pDb->set_challengetimes(min(hasBattleCount, maxBattleCount));
	return successContent(sid);
}

string GMHandler::handle_addsoulpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	//addsoulpiece,uid,id,count,sid
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int id = 0, count = 0;
	safe_atoi(gmtoken[2], id);
	safe_atoi(gmtoken[3], count);
	DB_SoulPiece* piece = user->GetPlayer()->GetSoulMgr()->addSoulPiece(id, SOULPIECE_GM_ADD, count);
	if (piece != NULL)
	{
		if (user->Online())
		{
			user->GetPlayer()->SendSoulPieceList();
		}
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_delsoulpiece( const vector<string>& gmtoken, User* user, string& sid )
{
	//delsoulpiece,uid,id,count,sid
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int id = 0, count = 0;
	safe_atoi(gmtoken[2], id);
	safe_atoi(gmtoken[3], count);
	bool res = user->GetPlayer()->GetSoulMgr()->delSoulPiece(id, SOULPIECE_GM_COST, count);
	if (res == true)
	{
		if (user->Online())
		{
			user->GetPlayer()->SendSoulPieceList();
		}
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_getarmadarank( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	ArmadaModuleInst::instance().FillRankToString(sid);
	return successContent(toString(Daemon::Instance().gameid()) + ":" + sid);
}

string GMHandler::handle_getarmadalastendtime( const vector<string>& gmtoken, string& sid )
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	int64 endTime = ArmadaModuleInst::instance().GetActivEndTime();
	DateTime now(endTime);
	string stdtime = toString(now.year())+ "-" + toString(now.month())+ "-" + toString(now.day())+ "-" + toString(now.hour())+ "-" + toString(now.minute());
	return successContent("stdtime: " + stdtime + "sec: " + toString(endTime));
}

string GMHandler::handle_settaskreward( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 6)
	{
		return formatErrorContent("args less");
	}
	int which = -1, type = -1, times = -1;
	safe_atoi(gmtoken[2], which);
	safe_atoi(gmtoken[3], type);
	safe_atoi(gmtoken[4], times);
	bool res = user->GetPlayer()->GetTaskRewardMgr()->SetTaskRewardForGM(which, (enumTaskType)type, times, sid);
	user->GetPlayer()->SendTaskRewardData();
	if(res)
		return successContent(sid);
	else
		return formatErrorContent(sid);
}

string GMHandler::handle_addtalent( const vector<string>& gmtoken, User* user, string& sid )
{
	//addtalent,uid,id,sid
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int id = 0;
	safe_atoi(gmtoken[2], id);
	bool res = user->GetPlayer()->GetTalentMgr()->addTalent(id, GM_ADDTalent);
	if (res == true)
	{
		if (user->Online())
		{
			user->GetPlayer()->sendTalentList();
		}
		Daemon::Instance().GetGameDataHandler()->markUserDirty(user);
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_setguardstage( const vector<string>& gmtoken, User* user, string& sid )
{
	//setguardstage,uid,stageid,1
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int stageid = 0;
	safe_atoi(gmtoken[2], stageid);
	CFG_DefenceStageData* stageModel = DefenceStageDataTblInst::instance().GetInfo(stageid);
	if (stageModel == NULL)
	{
		return errorContent("tbl not found", -1, sid);
	}
	user->GetPlayer()->GetGuardMgr()->UpdateStage(stageid);
	return successContent(sid);
}

string GMHandler::handle_cleartalentinitflag( const vector<string>& gmtoken, User* user, string& sid )
{
	//cleartalentinitflag,uid,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	DB_Talent* talentdb = user->GetDBPlayer()->mutable_talent();
	if (talentdb != NULL)
		talentdb->set_initflag(false);
	return successContent(sid);
}


string GMHandler::handle_setscience( const vector<string>& gmtoken, User* user, string& sid )
{
	//setscience,(u)id,sid,num//设置科技中心等级 sid1-5之间
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int scid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	//int re=1;
	switch(scid)
	{
		case 5:
			{
				user->GetPlayer()->GetScienceMgr()->GetpDb()->set_ironlevel(max(1,num)); //max(1,num)
			}
			break;
		case 1:
			{
				user->GetPlayer()->GetScienceMgr()->GetpDb()->set_ammolevel(max(1,num));
			}
			break;
		case 2:
			{
				user->GetPlayer()->GetScienceMgr()->GetpDb()->set_airlevel(max(1,num));
			}
			break;
		case 3:
			{
				user->GetPlayer()->GetScienceMgr()->GetpDb()->set_machinelevel(max(1,num));
			}
			break;
		case 4:
			{
				user->GetPlayer()->GetScienceMgr()->GetpDb()->set_eleclevel(max(1,num));
			}
			break;
		default:
			return errorContent("Failed", -1, sid);
	}
	user->GetPlayer()->GetShipMgr()->CalcAllShips();
	if (user->Online())
	{
		user->GetPlayer()->SendShipList();
		user->GetPlayer()->SendUserData();	
		user->GetPlayer()->SendScienceList();
		user->GetPlayer()->SendPropList();
	}
	return successContent(sid);
	
	//return errorContent("Failed", -1, sid);
}

string GMHandler::handle_setrole( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 6)
	{
		return formatErrorContent(sid);
	}
	string name = gmtoken[2];
	int head = getInt(gmtoken[3]);
	int camp = getInt(gmtoken[4]);
	int res = user->setRole(name, head, camp);
	if (res == 0)
	{
		return successContent(sid);
	}
	return errorContent("failed", res, sid);
}

string GMHandler::handle_globalarenasignup( const vector<string>& gmtoken, User* user, string& sid )
{
	//globalarenasignup,uid,1
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	Event* signup = Daemon::Instance().GetGameEventHandler()->getEventQueue()->allocateEvent();
	signup->set_cmd(C2S_DceSignUpAudition);
	signup->set_state(Client_to_Game);
	signup->set_time(0);
	signup->set_uid(user->uid());
	Daemon::Instance().GetGameEventHandler()->getEventQueue()->safePushEvent(signup);
	return successContent(sid);
}

string GMHandler::handle_addlimitcaptain1time( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	DB_LimitCaptain* pLimitCaptain = user->GetDBPlayer()->mutable_limitcaptain();
	pLimitCaptain->set_score(pLimitCaptain->score() + nCount*10);
	int64 now = time(NULL);
	DB_LimitCaptainData data;
	data.set_score(pLimitCaptain->score());
	data.set_time(now);
	data.set_uid(user->uid());
	LimitCaptainRankKey key(data.score(), data.time(), data.uid() );
	LimitCaptainModuleInst::instance().UpdateScore(data, key);
	return successContent(sid);
}

string GMHandler::handle_addlimitcaptain2time( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	DB_LimitCaptain2* pLimitCaptain = user->GetDBPlayer()->mutable_limitcaptain2();
	pLimitCaptain->set_score(pLimitCaptain->score() + nCount*10);
	int64 now = time(NULL);
	DB_LimitCaptainData data;
	data.set_score(pLimitCaptain->score());
	data.set_time(now);
	data.set_uid(user->uid());
	LimitCaptainRankKey key(data.score(), data.time(), data.uid() );
	LimitCaptainModuleTwoInst::instance().UpdateScore(data, key);
	return successContent(sid);
}

//string GMHandler::handle_setdrawluckwheel( const vector<string>& gmtoken, User* user, string& sid )
//{
//	/*
//		setdrawluckwheel,(u)id,type,state,sid
//		type = 1充值6元抽奖;2,50元抽奖;3,100元抽奖
//		state = 0 设置可以抽奖；1,设置为已经抽过了
//	*/
//	if (gmtoken.size() < 4)
//	{
//		return formatErrorContent(sid);
//	}
//	int type = 0;
//	int state = -1;
//	safe_atoi(gmtoken[2], type);
//	safe_atoi(gmtoken[3], state);
//
//	if(type < 1 || type > 3)
//		return formatErrorContent("type is not 1,2,3");
//
//	if(state != 0 || state != 1)
//		return formatErrorContent("state is not 0,1");
//
//	//state: 0可以抽一次;1 设置为已经抽过了
//	DB_Activity*pDb = user->GetPlayer()->GetActivityMgr()->GetDBActivity();
//	if(!pDb)
//		return formatErrorContent("DB_Activity is null");
//	if(state == 1)
//	{
//		pDb->mutable_luckywheelreward()->add_lotterylist(type);
//	}
//	else if(state == 0)
//	{
//		//使玩家达到抽奖的充值数
//		CFG_LuckyWheelReward* pModel = LuckyWheelRewardTblInst::instance().GetInfo(type);
//		if (!pModel)
//			return formatErrorContent("CFG_LuckyWheelReward *pModel is null");
//		const float HAS_RECHARGE_COUNT = pDb->mutable_luckywheelreward()->rechargecounttoday();
//		float detaRecharge = HAS_RECHARGE_COUNT - pModel->get_rechargeCount();
//		float EPSINON = 0.0001f;
//		if(abs(detaRecharge) >= EPSINON && detaRecharge < 0)
//		{
//			pDb->mutable_luckywheelreward()->set_rechargecounttoday(pModel->get_rechargeCount());
//		}
//
//		//剔除抽过奖的记录,使得玩家还可以抽奖
//		stack<int> vPos;
//		//找出要删除的元素
//		for (int i=0; i<pDb->mutable_luckywheelreward()->lotterylist_size(); ++i)
//		{
//			if (pDb->mutable_luckywheelreward()->lotterylist(i) == type)
//				vPos.push(i);
//		}
//		const int TYPE_NUM = vPos.size();
//		::google::protobuf::RepeatedField< int >* p = pDb->mutable_luckywheelreward()->mutable_lotterylist();
//		if (!p)
//			return formatErrorContent("mutable_lotterylist is null");
//		//把要删除元素的移到末尾
//		for(int i = pDb->mutable_luckywheelreward()->lotterylist_size() - 1; i >= 0; --i)
//		{
//			if(vPos.empty())
//				break;
//			int tmpPos = vPos.top(); vPos.pop();
//			p->SwapElements(i,tmpPos);
//		}
//		for(int i = 0; i < TYPE_NUM; ++i)
//			p->RemoveLast();
//	}
//	return successContent(sid);
//}

string GMHandler::handle_addseiko( const vector<string>& gmtoken, User* user, string& sid )
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int seikoid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	bool res = false;
	for (int i=0; i<num; i++)
	{
		DB_Seiko* pSeiko = user->GetPlayer()->GetSeikoMgr()->addSeiko(seikoid, SEIKO_GM_ADD);
		if (pSeiko == NULL)
			continue;
		res = true;
		if (user->Online())
			user->GetPlayer()->SendSeikoList();
	}
	if (res)
		return successContent(sid);
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_delseiko(const vector<string>& gmtoken, User* user, string& sid)
{	
	if (gmtoken.size() <= 2)
		return formatErrorContent(sid);

	int64 seikoid = StringToUid(gmtoken[2]);
	bool res = user->GetPlayer()->GetSeikoMgr()->delSeiko(seikoid, SEIKO_GM_COST);
	if (res)
	{
		if (user->Online())
			user->GetPlayer()->SendSeikoList();
		return successContent(sid);
	}
	else
		return errorContent("Failed", -1, sid);
}

string GMHandler::handle_addseikopiece(const vector<string>& gmtoken, User* user, string& sid)
{	
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int nSeikoPieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetSeikoMgr()->addSeikoPiece(nSeikoPieceid, SEIKOPIECE_GM_ADD, num))
	{
		if (user->Online())
			user->GetPlayer()->SendSeikoPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);

}

string GMHandler::handle_delseikopiece(const vector<string>& gmtoken, User* user, string& sid)
{	
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);

	int nSeikopieceid = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if (user->GetPlayer()->GetSeikoMgr()->delSeikoPiece(nSeikopieceid, SEIKOPIECE_GM_COST, num))
	{
		if (user->Online())
			user->GetPlayer()->SendSeikoPieceList();
		return successContent(sid);
	}
	return errorContent("Failed", -1, sid);
}

string GMHandler::handle_clearprostrateflag( const vector<string>& gmtoken, User* user, string& sid )
{
	//clearprostrate,uid,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	DB_Player* pPlayer = user->GetDBPlayer();
	if (pPlayer != NULL)
	{
		pPlayer->clear_prostratearena();
		user->GetPlayer()->SendUserData();
	}
	return successContent(sid);
}

string GMHandler::handle_goldleadersendward(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int version = getInt(gmtoken[2]);
	GoldActiveModuleInst::instance().SetSendWardFlag(false);
	GoldActiveModuleInst::instance().SetVersion(version);
	return successContent(toString(Daemon::Instance().gameid()) + ":" + sid);
}

string GMHandler::handle_goldleadersendwardbyrank(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	int rank = getInt(gmtoken[2]);
	bool result = GoldActiveModuleInst::instance().SendAwardByRank(user->uid(), rank);
	if (result)
	{
		return successContent(sid);
	}
	return failContent(sid);
}

string GMHandler::handle_setgoldislandforceclear(const vector<string>& gmtoken, string& sid)
{
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	GoldIslandModuleInst::instance().SetForceClear(true);
	return successContent(sid);
}

string GMHandler::handle_dbuserpartdata(const vector<string>& gmtoken, User* user, string& sid)
{
	string struid = "";
	string strpath = "";
	int type = 0;
	if (gmtoken.size() != 5)
	{
		return formatErrorContent(sid);
	}

	if (gmtoken.size() > 2)
	{
		struid = gmtoken[1];
		strpath = gmtoken[2];
		safe_atoi(gmtoken[3], type);
	}
	else
	{
		struid = gmtoken[1];
		strpath = "";
		type = 0;
	}

	Value ret(objectValue);
	string response;
	do 
	{
		if (!user)
		{
			response = errorContent("Invalid User", 1, struid);
			break;
		}

		Value json(objectValue);
		Proto2Json(json, user->GetDbUser());

		Value *pRoot = &json;
		Value *pCRoot = JsonPathSearch(pRoot, strpath);

		
		ret["sid"] = sid;
		if (NULL==pCRoot)
		{
			if (type==0)
			{
				response = pRoot->toSingleString();
			}
			else
			{
				response = pRoot->toStyledString();
			}
		}
		else
		{
			if (type==0)
			{
				response = pCRoot->toSingleString();
			}
			else
			{
				response = pCRoot->toStyledString();
			}
		}

		ret["ret"] = response;

		LOG4CXX_INFO(logger_, "Player part info[u" << user->uid() <<"]=" << response);
	} while (0);

	if (type == 0)
		return ret.toSingleString();
	else
		return ret.toStyledString();
}

string GMHandler::handle_sweepstage(const vector<string>& gmtoken, User* user, string &sid)
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}

	int res = 0;
	string uid = gmtoken[1];
	int type = -1;
	safe_atoi(gmtoken[2], type);
	int stageID = -1;
	safe_atoi(gmtoken[3], stageID);
	int sweepType = 1;
	safe_atoi(gmtoken[4], sweepType);

	switch (type)
	{
	case BATTLE_TYPE_STAGE:
		res = user->GetPlayer()->GetStageMgr()->AttackBySweep(stageID, sweepType);
		break;
	case BATTLE_TYPE_ADVSTAGE:
		res = user->GetPlayer()->GetAdvStageMgr()->AttackBySweep(stageID, sweepType);
		break;
	case BATTLE_TYPE_ROBBERY:
		break;
	default:
		break;
	}

	return successContent(sid);
}

string GMHandler::handle_activity_opt(const vector<string>& gmtoken, User* user, string &sid)
{
	if (gmtoken.size()<5)
	{
		return formatErrorContent(sid);
	}

	enum
	{
		opt_first_rechage_reset = 0,		//重置首充豪礼
		opt_first_rechage_reward,			//首充豪礼领奖
		opt_first_rechage_version,
	};

	string uid = gmtoken[1];
	int type = 0;
	safe_atoi(gmtoken[2], type);
	int opt = 0;
	safe_atoi(gmtoken[3], opt);

	switch (type)
	{
	case FUNCSWITCHID_FIRSTCHARGEREWARD: //首充豪礼*/
		{
			switch (opt)
			{
			case opt_first_rechage_reset:
				{
					user->GetPlayer()->GetActivityMgr()->ResetFirstRechargeReward();
				}
				break;
			case opt_first_rechage_reward:
				{
					user->GetPlayer()->GetActivityMgr()->HandleFirstRecharge();
				}
				break;
			case opt_first_rechage_version:
				{
					if (gmtoken.size()<=5)
					{
						return formatErrorContent(sid);
					}

					int ver = 0;
					safe_atoi(gmtoken[4], ver);
					DB_FirstRechargeReward *pFirstRechargeReward = user->GetPlayer()->GetDBPlayer()->mutable_activity()->mutable_first_recharge_reward();
					pFirstRechargeReward->set_version(ver);
				}
				break;
			}
		}
 		break;
 	default:
 		break;
	}

	return successContent(sid);
}

string GMHandler::handle_friends_opt(const vector<string>& gmtoken, User* user, string &sid)
{
	
	if (gmtoken.size()<5)
	{
		return formatErrorContent(sid);
	}

	enum
	{
		opt_set_sendeleclist = 0,		//设定寻宝领赠送记录
		opt_exc_sendeleclist,			//一键发送寻宝领
	};

	string uid = gmtoken[1];
	int opt = 0;
	safe_atoi(gmtoken[2], opt);
	int val = 0;
	safe_atoi(gmtoken[3], val);

	switch (opt)
	{
	case opt_set_sendeleclist:
		{
			user->GetPlayer()->GetFriendMgr()->ResetSendEleclistValue(val);
		}
		break;
	case opt_exc_sendeleclist:
		{
			int result = user->GetPlayer()->GetFriendMgr()->sendElectricAllFriends();
			if (result != 0)
			{
				return formatErrorContent(sid);
			}

			user->GetPlayer()->SendFriendList();

			string text;
			DseSendElectric data;
			data.set_type(1);
			data.set_result(result);
			data.SerializeToString(&text);
			eh_->sendDataToUser(user->fd(), S2C_DseSendElectric, text);
		}
		break;
	default:
		{
		}
		break;
	}

	return successContent(sid);
}

string GMHandler::handle_undorecharge(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 3)
		return formatErrorContent(sid);
	int nType = getInt(gmtoken[2]);
	user->GetPlayer()->UndoRecharge(nType);
	if (user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);	
}

string GMHandler::handle_testdrop( const vector<string>& gmtoken, string& sid )
{
	//测试某掉落组的掉率 testdrop,g1,dropid,num,1
	if (gmtoken.size() <= 4)
		return formatErrorContent(toString(sid));
	string dropid = gmtoken[2];
	int dropidint = getInt(gmtoken[2]);
	int testnum = getInt(gmtoken[3]);
	FILE* testdrop = fopen(dropid.c_str(), "a");
	if (testdrop == NULL)
		return errorContent("file open failed.", -1, sid);
	char buffer[1024] = {0};
	sprintf(buffer, "index	times	percentage\n");    
	fwrite(buffer, 1, strlen(buffer), testdrop);
	map<int, int> results;
	for (int i=0; i<testnum; ++i)
	{    
		CFG_Drop* drop = DropTblInst::instance().RandLine(dropidint);
		if (drop)
		{
			results[drop->get_index()] += 1;
		}
	}
	map<int, int >::iterator iter = results.begin();
	int count = 0;
	for (; iter!=results.end(); ++iter)
	{
		memset(buffer, 0, 1024);
		double percent = (iter->second) / (double)testnum;
		sprintf(buffer, "%d     %d		%lf\n", iter->first, iter->second, percent);    
		fwrite(buffer, 1, strlen(buffer), testdrop);
		count += iter->second;
	}
	sprintf(buffer, "total times:     %d\n", count);    
	fwrite(buffer, 1, strlen(buffer), testdrop);
	fclose(testdrop);

	return successContent(sid);
}

string GMHandler::handle_testcaptainrecruit( const vector<string>& gmtoken, string& sid )
{
	//测试某掉落组的掉率 testcaptainrecruit,g1,type,mode,num,1
	if (gmtoken.size() <= 5)
		return formatErrorContent(toString(sid));
	int type = getInt(gmtoken[2]);
	int mode = getInt(gmtoken[3]);
	int testnum = getInt(gmtoken[4]);
	string filename = "recruit_" + toString(type) + "_" + toString(mode);
	FILE* testdrop = fopen(filename.c_str(), "a");
	if (testdrop == NULL)
		return errorContent("file open failed.", -1, sid);
	char buffer[1024] = {0};
	sprintf(buffer, "captainid	times	percentage\n");    
	fwrite(buffer, 1, strlen(buffer), testdrop);
	map<int, int> results;
	for (int i=0; i<testnum; ++i)
	{    
		int captain = captainRecruitRndTblInst::instance().getcaptainid(type, mode);
		results[captain] += 1;
	}
	map<int, int >::iterator iter = results.begin();
	int count = 0;
	for (; iter!=results.end(); ++iter)
	{
		memset(buffer, 0, 1024);
		double percent = (iter->second) / (double)testnum;
		sprintf(buffer, "%d     %d		%lf\n", iter->first, iter->second, percent);    
		fwrite(buffer, 1, strlen(buffer), testdrop);
		count += iter->second;
	}
	sprintf(buffer, "total times:     %d\n", count);    
	fwrite(buffer, 1, strlen(buffer), testdrop);
	fclose(testdrop);

	return successContent(sid);
}

string GMHandler::handle_setstagetowertime(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() <= 4)
		return formatErrorContent(sid);
	int nType = getInt(gmtoken[2]);
	int num = getInt(gmtoken[3]);
	if(1 == nType) //重置挑战次数
	{
		user->GetPlayer()->GetStageTowerMgr()->ResetChallengeTimes();
		return successContent(sid);	
	}
	else if(2 == nType)//增加纵横四海星星
	{
		user->GetPlayer()->GetStageTowerMgr()->AddStar(num);
		return successContent(sid);	
	}
	return formatErrorContent(sid);
}

string GMHandler::handle_changepartid( const vector<string>& gmtoken, User* user, string& sid )
{
	//changepartid,uid,partid,newpartid,1
	if (gmtoken.size() < 5)
		return formatErrorContent(sid);
	int oldid = getInt(gmtoken[2]);
	int newid = getInt(gmtoken[3]);
	DB_Player* pdb = user->GetDBPlayer();
	for (int i=0; i<pdb->partpiecelist_size(); ++i)
	{
		DB_Parts* data = pdb->mutable_partslist(i);
		if (data && data->partsid() == oldid)
		{
			data->set_partsid(newid);
			data->set_point(0);
			data->set_remouldlevel(0);
			data->set_upgradelevel(1);
			data->set_upgradegold(0);
			Daemon::Instance().GetGameDataHandler()->markUserDirty(user); 
		}
	}
	return successContent(sid);	
}

string GMHandler::handle_superexchange(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int score = getInt(gmtoken[2]);
	
	DB_Player* pdb = user->GetDBPlayer();
	bool clear = true;
	user->GetPlayer()->ClearFuncSwitchOldData(FUNCSWITCHID_SUPEREXCHANGE, &clear);
	DB_SuperExchange * pDB = pdb->mutable_superexchange();
	pDB->set_score(score + pDB->score());
	return successContent(sid);	
}

string GMHandler::handle_cleanfirstorshop( const vector<string>& gmtoken, User* user, string& sid )
{
	//cleanfirstorshop,(u)id, type, sid// type = 1 清空firstreward; = 2 清空seikoshop
	if (gmtoken.size() < 4)
		return formatErrorContent(sid);
	int type = getInt(gmtoken[2]);
	DB_Player* pdb = user->GetDBPlayer();
	if(NULL == pdb)
		return formatErrorContent(sid);
	DB_Activity *pAct = pdb->mutable_activity();
	if(NULL == pAct)
		return formatErrorContent(sid);
	
	switch (type)
	{
	case 1:
		pAct->clear_first_recharge_reward();
		break;
	case 2:
		pAct->clear_seikoshop();
		break;
	default:
		return formatErrorContent("type error");
	}
	return successContent(sid);	
}

string GMHandler::handle_resetstagetower( const vector<string>& gmtoken, User* user, string& sid )
{
	//resetstagetower,(u)id,sid //重置纵横四海
	if (NULL == user || gmtoken.size() < 3)
		return formatErrorContent(sid);

	DB_Player* pdb = user->GetDBPlayer();
	if(NULL == pdb)
		return formatErrorContent(sid);
	user->GetPlayer()->GetStageTowerMgr()->ResetDataForNewDay();
	return successContent(sid);	
}

string GMHandler::handle_challengesignup( const vector<string>& gmtoken, User* user, string& sid )
{
	//challengesignup,uid,1
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	Event* signup = Daemon::Instance().GetGameEventHandler()->getEventQueue()->allocateEvent();
	DceChallengeSignUp* data = signup->mutable_dce_dcechallengesignup();
	data->set_head(user->getHead());
	signup->set_cmd(C2S_DceChallengeSignUp);
	signup->set_state(Client_to_Game);
	signup->set_time(0);
	signup->set_uid(user->uid());
	Daemon::Instance().GetGameEventHandler()->getEventQueue()->safePushEvent(signup);
	return successContent(sid);
}

string GMHandler::handle_addgoldcaptain( const vector<string>& gmtoken, User* user, string& sid )
{
	//addgoldcaptain,uid,count,1
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int nCount = getInt(gmtoken[2]);
	DB_GoldActiveData* pGoldActive = user->GetDBPlayer()->mutable_goldleader();
	pGoldActive->set_points(pGoldActive->points() + nCount);
	GoldActiveRankKey key(pGoldActive->points(), (int)time(NULL), user->uid());
	DB_RankUnit dbRank;
	dbRank.set_point(pGoldActive->points());
	dbRank.set_time((int)time(NULL));
	dbRank.set_uid(user->uid());
	GoldActiveModuleInst::instance().AddRank(&dbRank, &key);
	return successContent(sid);
}

string GMHandler::handle_syncgoldactvity( const vector<string>& gmtoken, User* user, string& sid )
{
	//syncgoldcaptain,uid,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	bool enable = FuncSwitchInst::instance().IsEnable(FUNCSWITCHID_GOLDLEADER, user->GetPlayer());
	if (enable == true)
	{
		int nVersion = FuncSwitchInst::instance().GetSwitchVersion(FUNCSWITCHID_GOLDLEADER);
		DB_GoldActiveData* goldactive = user->GetDBPlayer()->mutable_goldleader();
		if (goldactive != NULL && goldactive->points() > 0 && goldactive->version() == nVersion)
		{
			GoldActiveRankKey key(goldactive->points(), (int)time(NULL), user->uid());
			DB_RankUnit dbRank;
			dbRank.set_point(goldactive->points());
			dbRank.set_time((int)time(NULL));
			dbRank.set_uid(user->uid());
			GoldActiveModuleInst::instance().AddRank(&dbRank, &key);
		}
	}
	return successContent(sid);
}

string GMHandler::handle_synclimitship( const vector<string>& gmtoken, User* user, string& sid )
{
	//synclimitship,uid,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	bool enable = FuncSwitchInst::instance().IsEnable(FUNCSWITCHID_LIMITSHIP, user->GetPlayer());
	if (enable == true)
	{
		int nVersion = FuncSwitchInst::instance().GetSwitchVersion(FUNCSWITCHID_LIMITSHIP);
		DB_LimitShip* data = user->GetDBPlayer()->mutable_limitship();	
		if (data != NULL && data->score() > 0 && data->version() == nVersion)
		{
			DB_LimitShipData limitShipData;
			limitShipData.set_score(data->score());
			limitShipData.set_time(time(NULL));
			limitShipData.set_uid(user->uid());
			LimitShipRankKey key(limitShipData.score(), limitShipData.time(), limitShipData.uid() );
			LimitShipModuleInst::instance().UpdateScore(limitShipData, key);
		}
	}
	return successContent(sid);
}

string GMHandler::handle_syncguildbattlesign( const vector<string>& gmtoken, string& sid )
{
	//syncguildbattlesign,g1,guidid,contribute,1
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}
	int contribute = 0;
	int64 guildid = 0;
	safe_atoi(gmtoken[3], contribute);
	safe_atoll(gmtoken[2], guildid);

	GuildBattleModuleInst::instance().addSignGuildContribute(guildid, contribute,false);
	return successContent(sid);
}

string GMHandler::handle_guildbattlesign( const vector<string>& gmtoken, string& sid )
{
	//guildbattlesign,g1,guildid,1
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int64 guildid = 0;
	safe_atoll(gmtoken[2], guildid);

	bool  suc = GuildBattleModuleInst::instance().addSignGuild(guildid);
	if (!suc)
		return failContent(sid);
	return successContent(sid);
}

string GMHandler::handle_guildbattleresetmap( const vector<string>& gmtoken, string& sid )
{
	//guildbattleresetmap,g1,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	GuildBattleModuleInst::instance().setinitislandday(false);
	return successContent(sid);
}

string GMHandler::handle_addchamptionRecord( const vector<string>& gmtoken, string& sid )
{
	//addChamptionRecord,(u)uid,1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	int64 uid = getUid(gmtoken[1]);
	PlayerBaseInfoModuleInst::instance().addChamptionToAllArena(uid);
	return successContent(sid);
}

string GMHandler::handle_tuangouopt(const vector<string>& gmtoken, User* user, string& sid)
{
	if (gmtoken.size() < 5)
	{
		return formatErrorContent(sid);
	}

	//int64 uid = getInt64(gmtoken[1]);
	int nType = getInt(gmtoken[2]);
	int nValue = getInt(gmtoken[3]);
	switch (nType)
	{
	case OPT_JOIN_REQUEST:
		{
			int errCode = 0;//user->GetPlayer()->GetTuanGouMgr()->CanJoinTuanGouRequest();
			if (errCode==0)
			{
				user->GetPlayer()->GetTuanGouMgr()->JoinTuanGouRequest();
				return successContent(sid);
			}
		}
		break;
	case OPT_JOIN_BUY:
		{
			int errCode = 0;//user->GetPlayer()->GetTuanGouMgr()->CanJoinTuanGouBuyItem(nValue);
			if (errCode==0)
			{
				user->GetPlayer()->GetTuanGouMgr()->JoinTuanGouBuyItem(nValue);
				return successContent(sid);
			}
		}
		break;
	case 4:	//发奖
		{
			TuanGouModuleInst::instance().SendJoinTuanGouGift();
			return successContent(sid);
		}
		break;
	case 5:
		{
			TuanGouModuleInst::instance().SetVersion(nValue);
			user->GetPlayer()->GetTuanGouMgr()->SetVersion(nValue);
			return successContent(sid);
		}
		break;
	case 6:
		{
			TuanGouModuleInst::instance().SetJoinInterval(nValue);
			return successContent(sid);
		}
		break;
	case 7:
		{
			TuanGouModuleInst::instance().SetBuysInterval(nValue);
			return successContent(sid);
		}
		break;
	default:
		{

		}
		break;
	}
	return formatErrorContent(sid);
}


string GMHandler::handle_seikoexchange(const vector<string>& gmtoken, User* user, string &sid)
{
	enum
	{
		OPT_ADD_SEIKO_SCORE = 0,	//增加积分
		OPT_SEIKO_EXCHANGES,		//兑换物品
	};

	if (gmtoken.size() < 6)
	{
		return formatErrorContent(sid);
	}

	int nOptType = getInt(gmtoken[2]);
	int nValue1 = getInt(gmtoken[3]);
	int nValue2 = getInt(gmtoken[4]);
	switch (nOptType)
	{
	case OPT_ADD_SEIKO_SCORE:
		{
			user->GetPlayer()->GetSeikoMgr()->addSeiKoExchangeScore(nValue1);
			return successContent(sid);
		}
		break;
	case OPT_SEIKO_EXCHANGES:
		{
			string text;
			DseSeiKoExchange data;
			int result = user->GetPlayer()->GetSeikoMgr()->seiKoScoreExchange(nValue1, nValue2, NULL);
			data.set_result(result);
			data.set_type(nValue1);
			data.set_nuber(nValue2);
			data.SerializeToString(&text);
			eh_->sendDataToUser(user->fd(), S2C_DseSeiKoExchange, text);
			return successContent(sid);
		}
		break;
	default:
		{

		}
		break;
	}
	return formatErrorContent(sid);
}

string GMHandler::handle_activitymailopt(const vector<string>& gmtoken, User* user, string &sid)					
{
	enum
	{
		OPT_SEND_LIMITSHIP_AWARDMAIL = 0,	//限时战舰发奖发邮件
		OPT_SEND_GHOSTSHIP_AWARDMAIL,		//幽灵船发奖发邮件
		OPT_SEND_LIMITCAPT_AWARDMAIL,		//限时舰长发奖发邮件
		OPT_SEND_VALENTINE_AWARDMAIL,		//玫瑰之约发奖励邮件
		OPT_SEND_ONEARMEDS_AWARDMAIL,		//幸运大翻倍发奖励邮件
		OPT_SEND_GOLDLEADER_AWARDMAIL,		//金牌舰长发奖励邮件
	};

	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}

	int nOptType = getInt(gmtoken[2]);
	//int nValue = getInt(gmtoken[3]);
	switch (nOptType)
	{
	case OPT_SEND_LIMITSHIP_AWARDMAIL:
		{
			LimitShipModuleInst::instance().SendAwardToTop20();
			return successContent(sid);
		}
		break;
	case OPT_SEND_GHOSTSHIP_AWARDMAIL:
		{
			GhostShipModuleInst::instance().SendAwardToTop();
			return successContent(sid);
		}
		break;
	case OPT_SEND_LIMITCAPT_AWARDMAIL:		//限时舰长发奖发邮件
		{
			LimitCaptainModuleInst::instance().SendAwardToTop();
			return successContent(sid);
		}
		break;
	case OPT_SEND_VALENTINE_AWARDMAIL:		//玫瑰之约发奖励邮件
		{
			ValentineModuleInst::instance().SendAwardToTop10();
			return successContent(sid);
		}
		break;
	case OPT_SEND_ONEARMEDS_AWARDMAIL:		//幸运大翻倍发奖励邮件
		{
			OnearmedBanditModuleInst::instance().SendAwardToTop10();
			return successContent(sid);
		}
		break;
	case OPT_SEND_GOLDLEADER_AWARDMAIL:		//金牌舰长发奖励邮件
		{
			GoldActiveModuleInst::instance().SendAward();
			return successContent(sid);
		}
		break;
	default:
		{
			return successContent(sid);
		}
		break;
	}

	return formatErrorContent(sid);
}



string GMHandler::handle_savedata(const vector<string>& gmtoken, string& sid)
{
	//savedata,w/g(num),-1
	if (gmtoken.size() < 3)
	{
		return formatErrorContent(sid);
	}
	Daemon::Instance().GetGameDataHandler()->quit();
	return successContent(sid);
}


string GMHandler::handle_unifyuid( const vector<string>& gmtoken, User* user, string &sid )
{
	//unifyuid,uid,1
	if (gmtoken.size() < 3)
		return formatErrorContent(sid);
	int64 uid = getUid(gmtoken[1]);
	user->setUid(uid);
	return successContent(sid);
}

string GMHandler::handle_setgifttime(const vector<string>& gmtoken,User *user, string &sid)
{
	//setgifttime,uid,time,sid
	if (gmtoken.size() < 4)
	{
		return formatErrorContent(sid);
	}
	int ntime = getInt(gmtoken[2]);
		
	if(user->GetDBPlayer()->wealid()>0)
		user->GetDBPlayer()->set_wealtime(ntime);
	if(user->Online())
		user->GetPlayer()->SendUserData();
	return successContent(sid);
}

string GMHandler::handle_clearchargebackgift( const vector<string>& gmtoken, User* user, string &sid )
{
	//clearchargebackgift,(u)id,sid //重置玩家身上的记录 
	if (gmtoken.size() < 3)
		return formatErrorContent("argc need >= 3");

	const int nVersion = FuncSwitchInst::instance().GetSwitchVersion(FUNCSWITCHID_RECHARGEBACKGIFT);
	user->GetPlayer()->GetRechargeBackGiftMgr()->ClearRecharge(nVersion );
	return successContent(sid);
}

string GMHandler::handle_chargebackgiftopt( const vector<string>& gmtoken, User* user, string &sid )
{
	//chargebackgiftopt,(u)id,opt,status,sid //opt: 充值档位; status: 0置为未领取 1置为已经领取
	if (gmtoken.size() < 5)
		return formatErrorContent("argc need >= 5");
	int giftlvl = -1;
	int status = 0;
	safe_atoi(gmtoken[2], giftlvl);
	safe_atoi(gmtoken[3], status);
	bool res = user->GetPlayer()->GetRechargeBackGiftMgr()->SetGetGiftStatus(giftlvl,status);
	if(!res)
		return formatErrorContent("setting error");
	return successContent(sid);
}

string GMHandler::handle_autosoulopt(const vector<string>& gmtoken, User* user, string &sid)
{
	enum
	{
		OPT_AUTO_EQUIP_SOUL = 0,
		OPT_AUTO_UNEQUIP_SOUL,
	};

	if (gmtoken.size() < 5)
	{
		return formatErrorContent("autosoulopt param number error!");
	}

	int opt = -1;
	int pos = -1;
	safe_atoi(gmtoken[2], opt);
	safe_atoi(gmtoken[3], pos);
	switch (opt)
	{
	case OPT_AUTO_EQUIP_SOUL:
		{
			vector<int64> oldlist;
			vector<int64> newlist;
			user->GetPlayer()->GetSoulMgr()->autoEquipSoul(pos, oldlist, newlist);
		}
		break;
	case OPT_AUTO_UNEQUIP_SOUL:
		{
			vector<int64> soullist;
			user->GetPlayer()->GetSoulMgr()->autoUnEquipSoul(pos, soullist);
		}
		break;
	default:
		{
			return formatErrorContent("autosoulopt no this opt!");
		}
		break;
	}

	return successContent(sid);
}

string GMHandler::handle_setWaitGiftTime(const vector<string>& gmtoken,User *user, string &sid)
{
	//setwaittime,uid,waittime,sid
	if (gmtoken.size()<4)
	{
		return formatErrorContent(sid);
	}
	int wait = getInt(gmtoken[2]);
	if(user->GetDBPlayer()->wealid()>0)
	{
		user->GetDBPlayer()->set_wealtime(int(time(NULL)) + wait);
	}
	if (user->Online())
	{
		user->GetPlayer()->SendUserData();
	}
	return successContent(sid);
}


string GMHandler::handle_getranking(const vector<string>& gmtoken , string& sid)
{
	//getranking,g(num),type,sid
	if(gmtoken.size() <4)
		return formatErrorContent(sid);
	RankPlayerList plist ; 
	vector<string> vStr;
	if(gmtoken[2] == "0")
	{
		RankingModuleInst::instance().FillLevelRankProto(&plist);
		for(int i=0;i<plist.list_size();i++)
		{
			RankPlayer rPlayer = plist.list(i);
			string s = rPlayer.uid() + "-" + toString(rPlayer.level()) ;
			vStr.push_back(s);
		}
	}
	else
	{
		RankingModuleInst::instance().FillPowerRankProto(&plist);
		for(int i=0;i<plist.list_size();i++)
		{
			RankPlayer rPlayer = plist.list(i);
			string s = rPlayer.uid() + "-" + toString(rPlayer.power()) ;
			vStr.push_back(s);
		}
	}
	return successContent(sid,vStr);
}

string GMHandler::handle_globalguildbattleopt( const vector<string>& gmtoken, string& sid )
{
	//globalguildbattleopt,g1,type,guildid,param,1
	if (gmtoken.size() < 6)
		return formatErrorContent(sid);
	int type = getInt(gmtoken[2]);
	int64 guildid = getInt64(gmtoken[3]);
	int param = getInt(gmtoken[4]);
	int res = 0;
	switch (type)
	{
	case 1://清楚阵容信息
		{
			
			GlobalGuildBattleGameModuleInst::instance().clearguildjointactic(guildid);
		}
		break;
	case 2://增加调整次数
		{
			GlobalGuildBattleGameModuleInst::instance().addAdjusttime(guildid, param);
		}
		break;
	case 3://自动报名
		{
			res = GlobalGuildBattleGameModuleInst::instance().signUpAudo(guildid);
		}
		break;
	case 4://设置加载一个玩家的时间
		{
			GlobalGuildBattleGameModuleInst::instance().setgettactictime(param);
		}
		break;
	default:
		return "GM Command Type ERROR.";
	}
	if (res == 0)
	{
		return successContent(sid);
	}
	return errorContent("res error", res, sid);
	
}
