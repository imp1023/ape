#pragma once
#include <vector>
#include <map>
#include <string>
#include "Commons.h"
#include "const_country_def.h"

//#ifdef _DEBUG
//#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
//#else
//#define DEBUG_CLIENTBLOCK
//#endif
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifdef _DEBUG
//#define new DEBUG_CLIENTBLOCK
//#endif
//


enum
{
	 _MAX_RC4BUFF_SIZE = 1024*512,
};

using namespace std;
typedef long long		time_d;
typedef long long       int64;
typedef unsigned char   byte;
typedef unsigned int    u_int;
typedef short           SHORT;
typedef long            LONG;

enum GameWorkingStatus 
{
	NOT_CONNECTED	= 0,
	NOT_SYNC_USER	= 1,
	LOST_CONNECTION = 2,
	NORMAL			= 3
};

//性别
const int Gender_NULL = 0;
const int Gender_Girl = 1;
const int Gender_Boy  = 2;
const int Gender_ALL  = 3;
//const int WeathLimit  = 200;
const int SEND_STAR_CNT = 20;		//暂定每次发20人
const int SEND_WEALTH_STAR_CNT = 10;
const int SEND_COMMON_STAR_CNT = 25;
const int ENEMY_COUNT  = 30;
const int BAGITEMLOG_COUNT = 30;	//背包日志数量
const int MAX_LV_CNT = 100*1000;	//暂定每个等级最多10万人
//const int STAR_LOCAL_MAX_LV = 30;	//game端保存的间谍卫星最高等级
//const int STAR_LOCAL_GET_LV = 25;	//game端从本地获得间谍卫星的最高等级
const int STAR_UPLIMIT_LV	= 5;
const int STAR_DOWNLIMIT_LV = 3;
const int MAX_ATK_NPC_ACTIVECNT = 4;	//每天最大NPC活动次数
const int64 MAX_RES_CNT		= 2000000000;//资源最大20亿
const int BAG_MAX_PILE_CNT	= 99;		//背包物品最大堆叠数,临时改为2000
const int BAGITEM_ID_START	= 1000;		//背包物品起始ID
const int TMP_BATITEM_ID_START=2000;	//临时背包物品起始ID
const int VIRTUAL_BAGITEM_ID_START=6000;	//虚拟背包物品起始ID
const int BUILDING_ID_START = 10000;	//普通建筑ID从10000开始
const int MINE_ID_START		= 20000;	//地雷的ID从20000开始
const int ARMY_ID_START	= 30000;		//兵种队列的ID开始
const int WPC_COM_ID_START  = 10000;	//组件开始ID
const int WPC_EMBLEM_ID_START = 20000;	//徽记开始ID
const int ADMIRAL_SKILL_ID_START = 10000; //将领技能开始ID
const int WPC_BOOK_ID_START = 30000;	 //书籍开始ID
const int EQUIP_ID_START = 60000;	     //装备开始ID
const int WPC_JAMMER_ID_START = 90000;   //干扰器开始ID

const int MEDALHONOR_ID_START = 70000;   //荣誉勋章开始ID

const int TRAP_ID_START	= 100000;		//陷阱的ID从100000开始

const int VIP_MINE_CNT	= 3;			//黄钻地雷数量
const int REDWAR_VIP_MINE_CNT = 8;      //红警VIP地雷数量
const int REDWAR_VIP_TRAP_CNT = 8;      //红警VIP陷阱数量
const int PRC_ARMY_MAX_GROUP_SIZE = 4;	//产兵建筑的最大队列数
const int PRC_ARMY_MAX_NUM	= 20;		//每个产兵队列的最大兵数
const double CREDITS_RATIO = 5.0;

const int DAY_SEND_GIFT_MAX_CNT = 60;	//每日最大赠送礼物数
const int DAY_ASK_GIFT_MAX_CNT	= 60;	//每日最大索取礼物数
const int DAY_GET_GIFT_2_BAG_MAX_CNT = 60;	//每日接收礼物到背包的最大数量
const int DAY_ASK_GIFT_2_BAG_MAX_CNT= 60;	//每日索取礼物到背包的最大数量
const int DAY_GET_GIFT_MAX_CNT  = DAY_GET_GIFT_2_BAG_MAX_CNT * 2; //每日可接收的最大礼物数
const int DAY_RESEND_GIFT_MAX_CNT	= DAY_ASK_GIFT_2_BAG_MAX_CNT * 2;	//每日可回赠的最大礼物数

const int ADMIRAL_TRAINLV_ADD_PROP = 20;	//将领培养等级增加属性
//const int MAX_GIFT_BAG_CNT		= 100;	//礼物最大保留数量
//注：补偿标志永远不可删除！！！！！！
const int EXPIATE_FLAG_CREDUT0506= 0x01;		//补偿标志
const int EXPIATE_FLAG_CREDUT0718= 0x02;		//资源扣除
const int EXPIATE_FLAG_CREDUT0323= 0x04;		//某次补偿
const int EXPIATE_FLAG_CREDUT032301= 0x08;		//PVE补偿
const int EXPIATE_FLAG_CREDUT20130506 = 0x10;		//国家补偿
const int EXPIATE_FLAG_CREDUT20130508 = 0x20;		//国家补偿
const int EXPIATE_FLAG_DAILYCOST20130517 = 0x40;	//每日充值补偿
const int EXPIATE_FLAG_DAILYCOST2013051702 = 0x80;	//每日充值补偿2
const int EXPIATE_FLAG_CHICKEN20130812 = 0x100;	//火鸡补偿
const int EXPIATE_FLAG_EQUIP_LOGON = 0x200;	//装备问题登陆补偿
const int EXPIATE_FLAG_CHICKEN20140428 = 0x400;	//火鸡配表时间错误补偿
const int EXPIATE_FLAG_DEFENSE0140625 = 0x800;	//防守补偿礼包
const int EXPIATE_FLAG_MOON20150319 = 0x1000;	//魔盒补偿礼包
const int EXPIATE_FLAG_MOON20150321 = 0x2000;	//魔盒补偿礼包
const int EXPIATE_FLAG_MOON20150522 = 0x4000;	//演戏补偿功能
const int EXPIATE_FLAG_GONGHUI20150601 = 0x8000;	//公会战补偿功能
const int EXPIATE_FLAG_XUEZHAN20150623 = 0x10000;	//血战补偿功能
const int EXPIATE_FLAG_XUEZHAN20150624 = 0x20000;	//血战补偿功能
const int EXPIATE_FLAG_HAPPYLLIB20150722 = 0x40000;	//快乐实验室功能补偿
const int EXPIATE_FLAG_GUILDBTL20150804 = 0x80000;	//公会战活动问题补偿
const int EXPIATE_FLAG_SERVER20150827 = 0x100000;	//停服补偿
const int EXPIATE_FLAG_JUNHUN20150910 = 0x200000;	//军魂功能补偿
const int EXPIATE_FLAG_BATTLEW20150916 = 0x400000;   //战争学院补偿
const int EXPIATE_FLAG_GVE20151012 = 0x800000;   //一区GVE回档补偿
const int EXPIATE_FLAG_GVE20151102 = 0x1000000;	 //GVE回档补偿
const int EXPIATE_FLAG_EMBLEM20151126 = 0x2000000;	 //法国巨炮徽记重置
const int EXPIATE_FLAG_PARTNER20151201 = 0x4000000;	 //小伙伴槽位功能开启
const int EXPIATE_FLAG_PARTNER20160121 = 0x8000000;	 //集团军营小伙伴问题修复
const int EXPIATE_FLAG_EQUIP20160225 = 0x10000000;	 //修复装备重复问题
const int EXPIATE_FLAG_DEFNPC20160306 = 0x20000000;	 //周日防守问题补偿

const int MaxHonorPerDay = 100000;
const int MAX_REGION_CNT = 100;	//最大分区数

const int MAX_USER_LEVEL = 100;	//玩家最大等级
const int MAX_USER_VIP_LEVEL = 7;	//玩家最大VIP等级
const int MAX_ADMIRAL_SKILL_LV = 12;
const int MAX_HERO_SKILL_LV = 12;	//最多技能等级，目前全是1
const int MAX_HERO_GRADE_LV = 20;	//最多星级别
const int MAX_HERO_QUALITY = 5;		//最大品质
const int MAX_BUILDING_LEVEL = 50;	//建筑最大等级

const int MAX_HERO_TRAIN_NUM = 4;	//英雄训练位最大个数
const int HERO_QUALITY2FIVE_GRADE = 7;	//紫色变成橙色的星级


const int nBtCDTime = 2;

const int MAX_SUIT_POKER_NUM = 13;          ///每种花色的牌个数
const int MAX_SUIT_TYPE_NUM = 4;            ///花色种类
const int MAX_OWN_POKER = 5;                ///可拥有的牌数（玩家和NPC）
const int INIT_NPC_SHOW_POKER = 3;          ///NPC默认显示的牌数

const int INTERNAL_WALL_PER_GIFT = 10;      //每次购买围墙礼包增加的围墙上限
const int MAX_POKER_VALUE = 14;             ///最大的牌面值
const int MIN_POKER_VALUE = 1;              ///最小的牌面值

const int MAX_CARDDUEL_NPC_NUM = 200;           ///卡片决斗最大NPC数量
const int MAX_CARDDUEL_WAIT_MATCH_TIME = 20;    ///卡片决斗等待匹配的最大时长
const int MAX_CARDDUEL_WAIT_CHOOSE_TIME = 15;   ///卡片决斗等待选择卡片的最大时长
const int MAX_CARDDUEL_MATCH_NPC_TIME = 5;      ///匹配NPC时最大的循环数
const int MAX_CARDDUEL_WINNUM = 10;
const int MIN_CARDDUEL_LIMITWINNUM = 8;         ///每日仅能领取一次经历的标志

const int LEVEL_GIFT_TYPE_FLAG = 19;            ///等级豪礼的标志

const int MAXSENDCNT = 400;

const int SUPER_STORM_DROPTIME = 60;            //超级强攻令掉线时长
const int SUPER_STORM_SAMEUSERINTERVAL = 15;    //对同一个玩家使用强攻令的间隔（分钟）

const int MEDALHONOR_MAX_QUALITY = 5;       //荣誉勋章最高的品质
const int MEDALHONOR_YELLOW_QUALITY = 5;       //荣誉勋章的橙色品质类型
const int MEDALHONOR_MAX_EXPTYPE = 7;       //荣誉勋章最高的经验类型
const int MEDALHONOR_MAX_LEVEL = 50;        //荣誉勋章最高等级
const int MEDALHONOR_JOKER_COUNTRY = 0;       //荣誉勋章中的无国家勋章
const int MEDALHONOR_EXP_TYPE = 7;         //经验荣誉勋章

#ifndef PIcons
const int MAX_ARMY_GRADE = 3; //兵种最大阶级
const int MAX_ARMY_GROUP = 3; //兵种组类
#endif

#ifndef SUPER_WEAPON
const int MAX_SUPERWEAPON_USE_NUM		= 1; //单次投放超级武器的最大次数
enum
{
	WAR_STAGE_BEFORE	= 0,	//进攻战斗前投放
	WAR_STAGE_MIDDLE,			//进攻战斗中投放
	WAR_STAGE_AFTER,			//进攻战斗后投放
};
#endif

#ifndef PI
const double PI					= 3.141592653589793;
#endif


//解决winsock冲突
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

#define SENDGIFTCOUNT 60
#define ATTACKCOUNT   20 // 战报玩家数

#ifdef _WIN32
#define usleep Sleep
#endif


// 平台类型，对应WEB的siteid
enum PLAT_TYPE
{
	PLAT_XIAOYOU = 0,	//校友
	PLAT_QZONE,			//空间
	PLAT_QQ_WEIBO,		//QQ微博
	PLAT_WAP,			//手机
	PLAT_MANYOU,		//漫游
	PLAT_QAdd,			//Q+
   // PLAT_QQGAME,        //游戏大厅
	PLAT_QHALL,			//QQ游戏大厅
	PLAT_TYPE_MAX
};

//玩家日志类型
enum UserLog_Type
{
	UserLog_Get = 0,
	UserLog_Use,
	UserLog_Cost,
	UserLog_Destroy,
	UesrLog_2TmpBag,	//放入临时背包
	UesrLog_Lost4Full,	//背包已满导致丢弃
	UserLog_Tmb2Bag,	//临时背包放入背包
	UserLog_TmpDestroy, //临时背包丢弃
	UesrLog_GetVirtualBagItem,	//放入虚拟背包
	UesrLog_CostVirtualBagItem,	//扣除虚拟物品
	UserLog_AutoDelete, //过期自动删除
};
//战斗类型
enum BtlType
{
	Btl_UnStart,
	Btl_Attack,
	Btl_Defence,
	Btl_Npc,
	Btl_DefenceNpc,	//防守活动
	Btl_Capture,	//占领
	Btl_Rescue,		//解救
	Btl_Reave,		//抢夺
	Btl_Gainst,		//反抗
	Btl_NpcActive,  // NPC活动，军团通缉令
	Btl_BaseDefNPC, //基地保卫战
	Btl_NpcArmyGroup,//推图活动
	Btl_PveBattle,
};
//战俘营资源生产类型
enum SlaveRoomPrcType
{
	SR_PRC_MEDAL = 0,
	SR_PRC_OIL,
	SR_PRC_HONOR,
	SR_PRC_FEATS,
	SR_PRC_CNT,
};
enum Admin_AdSet_Type
{
	AAT_Enable = 0,
	AAT_RegTime,
	AAT_PopTime,
	AAT_WebSet,
};
enum Admin_Opt_Type
{
	AOT_ADMIN_ADDRES	 = 0,
	AOT_ADMIN_ADDARMY,
	AOT_ADMIN_DELARMY,
	AOT_ADMIN_ADDBLD,
	AOT_ADMIN_DELBLD,
	AOT_ADMIN_SETBLDCNT,
	AOT_ADMIN_SETALLBLDLV,
	AOT_ADMIN_SETBLDLV,
	AOT_ADMIN_SETARMYLV,
	AOT_ADMIN_ADDNPC,
	AOT_ADMIN_DELNPC,
	AOT_ADMIN_ADDITEM,
	AOT_ADMIN_DELBLDCNT,
	AOT_ADMIN_ADDEMBLEM,
	AOT_ADMIN_ADDFEATS,
	AOT_ADMIN_SETINIT,
	AOT_ADMIN_SETSCIENCELVL,
	AOT_ADMIN_SETGMTESTVALUE,
	AOT_ADMIN_ADDWPCCOM,
	AOT_ADMIN_ADDMOONCNT,
	AOT_ADMIN_SAVENPC,
	AOT_ADMIN_ADDADMIRAL,
	AOT_ADMIN_DELADMIRAL,
	AOT_ADMIN_ADDSOUL,
	AOT_ADMIN_RECOVERBLDHP,
	AOT_ADMIN_RECOVERARMYHP,
	AOT_ADMIN_ADDADMIRALTRAINEXP,
	AOT_ADMIN_ADDBOOK,
    AOT_ADMIN_DELBOOKEXP,

	AOT_ADMIN_CLEARSTRATEGIC,
	AOT_ADMIN_PVESET_CUSTOM,
	AOT_ADMIN_PVE_FIGHT_START,
	AOT_ADMIN_ADMINAL_REINITDB,
	AOT_ADMIN_ADDCOUPONS,

	AOT_ADMIN_ADD_COUNTRYTHEW,
	AOT_ADMIN_ADD_COUNTRYMORALE,
	AOT_ADMIN_ADD_COUNTRYZHANGONG,
	AOT_ADMIN_ADD_COUNTRYBOX,
	AOT_ADMIN_COUNTRY_RELOADRANK,
	AOT_ADMIN_ARENAFIGHTSET,

	AOT_ADMIN_ADDADMIRALTIMES,
	AOT_ADMIN_ADD_EQUIP,
	AOT_ADMIN_FINISHGUIDE,
	AOT_ADMIN_WORLDARENAFIGHTSET,

	AOT_ADMIN_ADD_GUILDSCORE,
	AOT_ADMIN_ADD_GUILDBLDVALUE,
	AOT_ADMIN_SET_GUILDSCIENCELV,
	AOT_ADMIN_SET_GUILDBLDLV,
	AOT_ADMIN_CLEAR_PASSWORD,
	AOT_ADMIN_SET_OFFICERPOSITION,
	AOT_ADMIN_SET_AddBuff,
	AOT_ADMIN_DELEMBLEM,
    AOT_ADMIN_DELEMBLEMEXP,
	AOT_ADMIN_SET_VipLevel,
	AOT_ADMIN_SET_ADMIRALSKILLLEVEL,
	AOT_ADMIN_SET_EQUIPSTRENGTHLEVEL,
	AOT_ADMIN_SPECIAL_VALUE,
	AOT_ADMIN_REGIONARENAFIGHTSET,
    AOT_ADMIN_CLEAR_NEWDAYINFO,             ///清楚每天的国战数据
	AOT_ADMIN_PVE_BATTLE_SET,
	AOT_ADMIN_ADD_JUNLING,
    AOT_ADMIN_ADD_POKERS,                   ///添加纸牌及换牌卡
    AOT_ADMIN_RESET_POKEREXC,               ///清除纸牌兑换信息
    AOT_ADMIN_RESET_DUELINFO,               ///清除卡片决斗信息

	AOT_ADMIN_ADDHERO,
	AOT_ADMIN_DELHERO,
	AOT_ADMIN_ADDLETTER,
	AOT_ADMIN_DELLETTER,
	AOT_ADMIN_SETHEROHP,
	AOT_ADMIN_RECOVERHEROHP,

	AOT_ADMIN_CLEAR_WHOLE_COUNTRY,			//跨服国战重置
	AOT_ADMIN_WHOLE_COUNTRY_SIGN,			//跨服国战报名

	AOT_ADMIN_ADDADMIRALLETTER,
	AOT_ADMIN_DELADMIRALLETTER,
	AOT_ADMIN_SETPROPERTY,
	AOT_ADMIN_DELWPCCOM,
    AOT_ADMIN_DELWPCCOMEXP,

	AOT_ADMIN_ADD_GVETHEW,
	AOT_ADMIN_ADD_GVEPOINT,
	AOT_ADMIN_GVE_LIST,
	AOT_ADMIN_HEROARENAFIGHTSET,
    AOT_ADMIN_CLEARBAGITEM,
    AOT_ADMIN_ADD_RENTWPC,
	AOT_ADMIN_ADD_GUILDTASK,
	AOT_ADMIN_SET_GUILDTASK_PROGRESS,

	AOT_ADMIN_GVG_REWARD,

	AOT_ADMIN_SET_GUILDLEADER_DELTIME,
	AOT_ADMIN_SET_GUILDLEADER,

    AOT_ADMIN_DEL_EQUIP,        //删除装备

	AOT_ADMIN_ADD_SEARCHTIME,

	AOT_ADMIN_DEL_ADMIRALSPECIAL,
	AOT_ADMIN_DEL_HEROSPECIAL,
	AOT_ADMIN_DEL_HEROEXPSPECIAL,
	AOT_ADMIN_DEL_WPCCOMEXPSPECIAL,
	AOT_ADMIN_DEL_EMBLEMEXPSPECIAL,
	AOT_ADMIN_DEL_BOOKXPSPECIAL,

	AOT_ADMIN_ADD_DBROLLBACK,

	AOT_ADMIN_ADD_EXPWPC,
	AOT_ADMIN_ADD_EXPEMBLEM,
	AOT_ADMIN_SET_WCPROMOTE_EXP,
	AOT_ADMIN_SET_WCSKILL_EXP,
	AOT_ADMIN_ADD_WCSKILL_EXP,
	AOT_ADMIN_SUB_WCSKILL_EXP,
	AOT_ADMIN_WCOP_TYPE,
	AOT_ADMIN_WCOP_UID,
	AOT_ADMIN_SADOP,
    AOT_ADMIN_ADD_WARGAME_REPORT,
    AOT_ADMIN_ADD_WARGAME_TOKEN,
	AOT_ADMIN_SET_STORMBOX,
	AOT_ADMIN_SET_QQTENYEARFLAG,
	AOT_ADMIN_HAPPYLIB_AWARD,

	AOT_ADMIN_COUNTRYCHATUI,
	AOT_ADMIN_COUNTRYCHATMSG,
	AOT_ADMIN_COUNTRYCHATMASK,

    AOT_ADMIN_MILITARYSOUL_POINT,

    AOT_ADMIN_SETSEVENDAYSLOGONDAYS,
	AOT_ADMIN_NPCACTIVERANK,

	AOT_ADMIN_TOWERHANCE,
	AOT_ADMIN_SETTOWERHANCE,

    AOT_ADMIN_ADDYASUIQIAN,

	AOT_ADMIN_BDNTIMES,

    AOT_ADMIN_ADDJAMMER,

    AOT_ADMIN_MEDALHONOROPT,

	AOT_ADMIN_SITETEST,

    AOT_ADMIN_SETCOSTCREDITGIFTINFO,
//	AOT_ADMIN_SITECREDITRELOAD,
};
enum Star_User_Group
{
	Star_Group_A, //付费<=100
	Star_Group_B, //付费<=1000
	Star_Group_C, //付费<=5000
	Star_Group_D, //付费>5000
	Star_Group_Cnt,
	Star_Group_All = Star_Group_Cnt, //所有用户
};
enum Admin_SetGMTest_Value
{
	Admin_SetGMTest_Value_INVITE = 0,	//邀请好友
	Admin_SetGMTest_Value_SENDGIFT,		//发送礼物
	Admin_SetGMTest_Value_GETGIFT,		//请求礼物
};

enum Web_TX_MissionOperate
{
	WEB_TX_MISSION_OPT_START = 0,
	WEB_TX_MISSION_OPT_MARK,
	WEB_TX_MISSION_OPT_QUERY,
	WEB_TX_MISSION_OPT_MARK_TOPLAT,
};

enum GiftDealType
{
	Gift_Ask=0,			//索取的礼物	
	Gift_Send,			//赠送的礼物
	Gift_Resend,		//回赠的礼物
	Gift_Send7Day,		//7天礼物

	Gift_System = 10,		//系统奖励、web礼物等
	Gift_Friend = 11,		//邀请好友奖励
};

enum SysGiftID
{
	SG_CallFrd = 10000,
	SG_InviteFrd,
	SG_WebGift,
	SG_InviteFrd_Gift,
	SG_SendGitf_Self,
	SG_AskGift_Self,
};


enum UserSource
{
	RC_Coin = 0,
	RC_Mineral,
	RC_Cash,
	RC_Exp,
	RC_Score,
	RC_Cnt,
};

enum UserSource_CostType
{
	E_UserSource_CostType_None = 0,					//无效
	E_UserSource_CostType_TREATSTRATEGIC,			//治疗战略兵营
};

enum UserSource_AddType
{
	E_UserSource_AddType_None = 0,					//无效
	E_UserSource_AddType_TREATSTRATEGIC,			//遣散战略兵营
	E_UserSource_AddType_PVEGIFT,					//PVE获得
};

enum DelArmyType
{
	DA_Unknow = 0,
	DA_Die,
	DA_OutCircle,
	DA_Sell,
	DA_NPC,
	DA_Admin,
	DA_Die_Base,
	DA_NPCArmyGroup,
	DA_Strategic,
	DA_TYPEALL,
	DA_NPCBattle,
};

enum DelArmyState
{
	DAS_NONE = 0,	//不需删除
	DAS_NEEDDEL,	//删除
};

enum BattleModifyCheck
{
	E_BattleModifyCheck_NULL = 0,
	E_BattleModifyCheck_AtkArmy_Defence,
	E_BattleModifyCheck_DefenceArmy_Atk,
	E_BattleModifyCheck_DefenceBld_Atk,
	E_BattleModifyCheck_DefenceArmy_NPC,
};


//相性类型
enum Restraint_Type
{
	Rst_To_Foot,
	Rst_To_Tank,
	Rst_To_Plane,
	Rst_To_Build,
	Rst_Cnt,      //将领相性最大值
	Rst_To_Hero = Rst_Cnt,
	Rst_ArmyCnt,  //兵种属性克制最大值
};
//将领属性
enum Admiral_Prop_Type
{
	AdmiralProp_AddAtk = 0,
	AdmiralProp_AddDef,
	AdmiralProp_AddHP,
	AdmiralProp_AddCrt,
	AdmiralProp_Cnt,
};

//武器中心书籍类型
enum WPC_BookType
{
	WBook_Bag = 0,		//背包中，未使用
	WBook_Admiral,	   //将领
	WBook_TypeMax,
};
//武器中心书籍增加属性类型


//武器中心徽记类型
enum WPC_EmblemType
{
	WE_Bag = 0,		//背包中，未使用
	WE_GunTurret,	//机枪塔
	WE_MortarTower,	//爱国者
	WE_OtherBld,	//其他建筑    
	WE_LaserTower,	//光棱塔
	WE_GiantCanon,  //法国巨炮
	WE_TypeMax,		
};

//武器中心干扰器类型
enum WPC_JammerType
{
    WJammer_Bag = 0,    //背包
    WJammer_Base,
    WJammer_Army,
};

//荣誉勋章激活的buf效果类型
enum MedalHonor_Ach_BufType
{
    MH_AF_BT_ThewReduce = 1,
};

//荣誉勋章的类型
enum MedalHonor_BagType
{
    MH_BT_Bag = 0,
    MH_BT_Admiral,
};

enum MedalHonor_DecorateType
{
    MH_DecorateType_Team = 1,   //战队
    MH_DecorateType_Army,       //三军
    MH_DecorateType_Country,    //国家
    MH_DecorateType_Allies,     //同门
    MH_DecorateType_UNO,        //联合国
    MH_DecorateType_MAX,
};

enum MHItemDecorateType
{//授勋类型0为道具授勋 1为勋章授勋
    MH_IDT_Item = 0,
    MH_IDT_Credit,
};

const int MAX_JAMMER_NUM = 6;
const int JAMMER_SPACE_ADD = 1;

enum WPC_JAMMERPropType
{
    WPC_JAMMERProp_UnKnow = 0,
    WPC_JAMMERProp_ReduceAtk,
    WPC_JAMMERProp_ReduceSpeed,
    WPC_JAMMERProp_ReduceCrit,
    WPC_JAMMERProp_ReduceCritDamage,
    WPC_JAMMERProp_ReduceToBuild,

    WPC_JAMMERProp_AllCnt,
};

//武器中心组装增加属性类型
enum WPC_COMAddPropType
{
	WPC_Unknow = 0,
	WPC_AddAtk,			//加攻击力
	WPC_AddHP,			//加血量
	WPC_AddRange,		//加射程
	WPC_AddSpeed,		//加速度
	WPC_AddTurnSpeed,	//加转向速度
	WPC_AddToPlane,		//加对飞机克制
	WPC_AddToTank,		//加对坦克克制
	WPC_AddToBld,		//加对建筑克制
	WPC_AddToFoot,		//加对步兵克制
	WPC_AddDamageDef,	//加伤害防御
	WPC_AddCrtDef,		//加暴击防御
	WPC_AddCrtDamage,	//加暴击伤害
	WPC_AddCrt,			//加暴击
	WPC_AddToHero,			//加对英雄克制
	WPC_AddToPlaneDef,		//加对飞机防御
	WPC_AddToTankDef,		//加对坦克防御
	WPC_AddToFootDef,		//加对步兵防御
	WPC_AddToHeroDef,		//加对英雄防御

    WPC_AddCnt,
    //军魂技能增加的属性类型
    WPC_AddGuardRange = WPC_AddCnt, //增加警戒范围
    WPC_DeftGuardRange,             //削弱警戒范围
    WPC_DeftDamage,                 //削弱伤害
    WPC_DeftCritRate,               //削弱暴击率
    WPC_DeftCritRange,              //削弱暴击幅度
    WPC_AllCnt,
};

enum WPC_JammerDeftType
{
    //
};

enum WPC_EmblemAddPropType
{
    WPC_EmblAdd_Unknow = 0,
    WPC_EmblAdd_AddHp,                  //增加血量
    WPC_EmblAdd_AddAtkToFoot,           //增加对步兵伤害
    WPC_EmblAdd_AddAtkToPlane,          //增加对飞机伤害
    WPC_EmblAdd_AddAtkToTanke,          //增加对坦克伤害
    WPC_EmblAdd_AddPlaneDef,            //增加对飞机防御
    WPC_EmblAdd_AddTankDef,             //增加对坦克防御
    WPC_EmblAdd_AddFootDef,             //增加对步兵防御
    WPC_EmblAdd_AddRange,               //增加射程
    WPC_EmblAdd_AddMotorFireRate,       //增加爱国者的射速
    WPC_EmblAdd_AddMetalRate,           //增加金属产量
    WPC_EmblAdd_AddOilRate,             //增加石油产量
    WPC_EmblAdd_ExpEmb,                 //经验徽记
    WPC_EmblAdd_AddAtkToAllArmy,        //增加对所有兵种攻击
    WPC_EmblAdd_AddAllArmyDef,          //增加对所有兵种防御

    //军魂增加的徽记属性
    WPC_EmblAdd_AddMineDamageToFoot,          //增加地雷对士兵伤害
    WPC_EmblAdd_AddMineDamageToTank,          //增加地雷对坦克伤害
    WPC_EmblAdd_AddMineDamageToHero,          //增加地雷对英雄伤害
    WPC_EmblAdd_AddAtkToHero,           //增加对英雄伤害
    WPC_EmblAdd_DeftMineDamageToFoot,   //削弱地雷对士兵伤害
    WPC_EmblAdd_DeftMineDamageToTank,   //削弱地雷对坦克伤害
    WPC_EmblAdd_DeftMineDamageToHero,   //削弱地雷对英雄伤害
    WPC_EmblAdd_DeftGunTurretAtkToFoot, //削弱机枪塔对士兵攻击
    WPC_EmblAdd_DeftGunTurretAtkToTank, //削弱机枪塔对坦克攻击
    WPC_EmblAdd_DeftGunTurretAtkToPlane,//削弱机枪塔对飞机攻击
    WPC_EmblAdd_DeftGunTurretAtkToHero, //削弱机枪塔对英雄攻击
    WPC_EmblAdd_DeftMortarTowerAtkToFoot,     //削弱爱国者对士兵攻击
    WPC_EmblAdd_DeftMortarTowerAtkToTank,     //削弱爱国者对坦克攻击
    WPC_EmblAdd_DeftMortarTowerAtkToPlane,    //削弱爱国者对飞机攻击
    WPC_EmblAdd_DeftMortarTowerAtkToHero,     //削弱爱国者对英雄攻击
    WPC_EmblAdd_ReduceHp,                   //削弱防御塔血量
    WPC_EmblAdd_MaxType,
};

//武器中心组件类型
enum WPC_BageType
{
	WB_Bag=0,
	WB_Base,
	WB_Army,
	WB_Tank,
	WB_Plane,
	WB_Hero,
	WB_TypeMax,
};
//英雄技能增加属性类型
enum HeroSkillType
{
	Hero_Skill_AddHP = 1,		//加生命上限
	Hero_Skill_AddRange,		//加射程
	Hero_Skill_AddDamage,		//加伤害
	Hero_Skill_AddSpeed,        //加速度
	Hero_Skill_AddTurnAngle,	//加转向角速度
	Hero_Skill_ToFoot,			//增加对步兵克制(armyToArmy)
	Hero_Skill_ToTank,			//增加对坦克克制(armyToTank)
	Hero_Skill_ToPlane,			//增加对飞机克制(armyToAir)
	Hero_Skill_ToBld,			//增加对建筑克制(armyToBuilding)
	
	Hero_Skill_AddCrt,          //加暴击
	Hero_Skill_AddCrtDef,       //加暴击防御
	Hero_Skill_AddDamageDef,    //加伤害防御
	Hero_Skill_AddArmyCount,    //加兵种人口
	Hero_Skill_AddCrtDamage,    //加暴击伤害
	Hero_Skill_AddDamageAndDecHP,	//增加攻击力和启动伤血

	Hero_Skill_AddDamageMinus = 17,	//减少敌方非英雄攻击力
	Hero_Skill_MinusSpeed,		//减少移动速度
	Hero_Skill_MinusEnemyTankCrt,		//减少坦克暴击率
	Hero_Skill_MinusEnemyTankDmg,		//减少敌方坦克对我方坦克的攻击
};
enum AddItemRet
{
	AR_Succ = 0,	//成功
	AR_TmpBag,		//临时背包
	AR_PartTmpBag,	//部分放入临时背包，部分丢失
	AR_Fail,		//失败
	AR_ITEMID,		//道具id错误
	AR_NUMLESS,		//扣道具数量不够
};

enum BagItemType
{
	Item_Unknow=0,
	Item_Normal,	//代币
	Item_Package,	//礼包
	ITem_Box,		//宝箱
	ITem_Special,	//特殊逻辑
	ITem_EquipItem,	//装备相关物品
	Item_EquipBox,  //装备宝箱
	Item_Exchange,  //交换券
};

enum ITEM_ID
{
	ITEM_ID_STRIKE_COMMAND	= 10004,
	ITEM_ID_COUNTRY_BOX		= 30028,
	ITEM_ID_BLOOD_TOOL		= 10042,
	ITEM_ID_JUNLING			= 10044,
    ITEM_ID_POKERS          = 10055,
    ITEM_ID_CHANGECARD      = 10056,
    ITEM_ID_CHANGBOX        = 20191,        //换牌礼包 为换牌卡的补偿
    ITEM_ID_DUELCARD        = 10057,        //卡片决斗卡
	ITEM_ID_PROPERTYCARD        = 10062,   //改造图纸

    ITEM_ID_WARGAME_TRUCEPROTOL = 10068,        //免战协议
    ITEM_ID_WARGAME_FORCEBOOK = 10069,          //宣战书

    ITEM_ID_SUPERSTORM_TOKEN = 10070,           //超级强攻令

    ITEM_ID_MILITARYSOUL_RESETTOOL = 10071,         //军魂技能重置道具

    ITEM_ID_TELEPORT_SCROLL=10075,      //国战传送卷轴
};

enum WallChgClothType
{
	WallChgCloth_Free = 1,
	WallChgCloth_Credit,
};

enum UpdatePlat_Type
{
	UpdatePlat_Level=0,
	UpdatePlat_InviteFrdCnt,	//已废弃
	UpdatePlat_Capture,
	UpdatePlat_Vip,
};

enum SlaveRoomOpt
{//战俘营相关操作
	SO_TimeOut = 0,		//时间到
	SO_Capture = 1,		//占领
	SO_Free,			//释放
	SO_BuySelf,			//赎身
	SO_LostSlaveByGainst,			//反抗
	SO_Rescue,			//解救
	SO_Reave,			//抢夺
	SO_LostSlaveByRescue,//俘虏被别人解救
	SO_LostSlaveBuReave,//战俘被抢
	
};
enum ErrorInfo
{
	Error_Success	 = 0,	//成功
	Error_IsAttacked = 1,	//正在被攻击
	Error_NoNPC,			//未找到NPC
	Error_NoBase,			//未找到基地
	Error_Online,			//玩家在线
	Error_NewProtected,		//新手保护
	Error_HurtProtected,	//伤害保护
	Error_AtkSelf,			//攻击自己
	Error_BattleNotOver,	//战斗尚未结束
	Error_LeverError,		//等级不符
	Error_AtkCntOverMax,	//当日攻击一个玩家超过3次
	Error_OnLineMore5Hour,	//在线超过5小时
	Error_NofindSlaveRoom,	//未找到战俘营
	Error_NoPos4SlaveRoom,	//战俘营已满
	Error_OverMaxRescueCnt,	//超过最大解救次数
	Error_OverMaxCaptureCnt,//超过最大被占领次数
	Error_HasCaptured,		//已经被占领
	Error_HasRescued,		//已经被解救
	Error_NoResuceID,		//无解救玩家ID
	Error_NoReaveID,		//无抢占玩家ID
	Erroe_NoCaptureID,		//无占领玩家ID
	Erroe_IsCaptureding,	//正在被占领
	Error_Strike,			//强攻成功
	Error_NoStrikeCommand,	//没有强攻令
	Error_LogoutProtected,  //离线保护时间
	Error_NotJunling,
};

enum UseItemChannelType
{//使用物品的渠道，用于立即使用物品类型
	UserItem_Unknow = 0,	//未知
	UserItem_By_BagItem,	//背包使用物品
	UserItem_By_CountryBox,	//国家宝箱
	UserItem_By_ArenaRank,	//竞技场排行榜
	UserItem_By_ActivityPush,//活动主动推送奖励
	UserItem_By_EquipBox,	//装备宝箱
	UserItem_By_EquipDestroy,	//装备分解
	UserItem_By_ArenaWeek,	//竞技场每周积分排行
	UserItem_By_GuildSignUp, //工会战报名
	UserItem_By_GuildBtl,	//工会战奖励
	UserItem_By_GuildBtlStorage, // 工会战战利品分配
	UserItem_By_GuildEmbrave,	//工会战

	UserItem_By_VipLevelUp, // 工会战战利品分配
	UserItem_By_Purchase, // 商城购买
	UserItem_By_Web,     //web发放
	UserItem_By_BloodBtl,	//血战
	UserItem_By_RegionArena,	//跨区竞技场奖励
	UserItem_By_Battle,		//PVE战役
	UserItem_By_UniteRegion, //合服礼包
	UserItem_By_RegionCountry, //跨区阵营战	
    UserItem_By_BaseExplore,    //基地探索 额外奖励
	UserItem_By_Gvg, //gvg	
    UserItem_ByDailyLand,       //每日登陆领取礼包
};

enum UserStat	//玩家状态
{	
	US_CanAttack=0,
	US_NewProtected,	//新手保护
	US_HurtProtect,		//伤害保护
	US_IsAttaccking,	//正在被攻击
	US_Destroy,			//基地被完全摧毁
};
enum StarState	//玩家卫星中的状态
{
	SS_Unknow = 0,
	SS_CanAttack,		//可以被攻击
	SS_HightLV,			//对方等级高，可以攻击
	SS_LowLVProted	= 3,//低等级保护
	SS_CanRevenge	= 4,//可复仇
	SS_Protected	= 5,//伤害保护
	SS_NewProtected = 6,//新手保护
	SS_IsAttacking	= 7,//正在被攻击
	SS_Unknow8,
	SS_LV2Hight,		//等级过高
	SS_BaseDestroy	= 10,//基地被破坏
	SS_MORE5ONLINE	= 11,//超过在线5小时
	SS_LogoutProtected	= 12,//下线保护
};

enum MineOpterate
{
	//1:设置雷区；	2生产地雷:；		5:撤销生产
	//4:地雷升级		5:撤销升级			
	//11:勋章造雷加速
	//12.勋章升级	
	//13:勋章升级加速
	MO_PutMine	= 1,			//布雷
	MO_Produce,		//生成地雷
	MO_UnProduce,	//撤销生成地雷
	MO_Upgrade,
	MO_UnUpgrade,	
	
	MO_CreditProduce = 11,
	MO_CreditUpgrade,
	MO_CreditAccUpgrade,
	MO_UnlockVipMine,	//解锁黄钻地雷

};

enum TrapOpterate
{
	//1:设置陷阱区；	2生产陷阱；		5:撤销生产
	//4:陷阱升级		5:撤销升级			
	//11:勋章陷阱加速
	//12.勋章升级	
	//13:勋章升级加速
	MO_PutTrap	= 1,			//布陷阱
	MO_ProduceTrap,		//生成陷阱
	MO_UnProduceTrap,	//撤销生成陷阱
	MO_UpgradeTrap,
	MO_UnUpgradeTrap,	

	MO_CreditProduceTrap = 11,
	MO_CreditUpgradeTrap,
	MO_CreditAccUpgradeTrap,
	MO_UnlockVipTrap,	//解锁黄钻陷阱
};

enum BuildingOpterate
{
	//1:造兵；			2:撤销造兵；		3:建造；				4:撤销建造；
	//5:建筑升级；	6:撤销建筑升级；7:科技升级			8：取消科技升级
	//9:维修；			10:进驻；				11:	超负荷；		12:	收取；	
	//13：全部收取；14：移动；			15:设置集结点		16:战车回收

	//勋章相关
	//31:勋章建造		32：勋章升级		33：勋章科技升级
	//34:勋章建造加速		35：勋章升级加速		36：勋章科技升级加速		
	//37:勋章造兵加速
	BO_UnKnow=0,
	BO_Produce,			//生成兵种
	BO_UnProduce,	
	BO_Create,			//建造
	BO_UnCreate,		//撤销建造
	BO_Upgrade,			//升级
	BO_UnUpgrade,		//取消升级
	BO_ArmyUpgrade,		//兵种、科技升级
	BO_UnArmyUpgrade,	//取消兵种、科技升级
	BO_Maintain,		//维修(无效)
	BO_Enter,			//进驻
	BO_OverLoading,		//超载
	BO_GetRes,			//收取
	BO_GetAllRes,		//全部收取
	BO_Move,			//移动
	BO_SetPoint,		//设置集结点
	BO_SellTank,		//战车回收
	BO_MaintainAll,		//全部维修(无效)
	BO_CreateMushRooms,	//创建报废汽车
	BO_PickMushRooms,	//收取报废汽车
	BO_Monsterbaiter,	//作战演习室
	BO_UpdateRemouldLvl,//改造等级升级
	BO_SlaveRoomGetRes, //战俘营收取资源
	BO_SlaveRoomFree,	//战俘营释放


	BO_CreditCreate=31,			//勋章建造
	BO_CreditUpgrade,			//勋章升级
	BO_CreditArmyUpgrade,		//勋章科技升级
	BO_CreditAccCreate,			//勋章建筑加速
	BO_CreateAccUpgrade,		//勋章升级加速
	BO_CreateAccArmyUpgrade,	//勋章兵种升级加速
	BO_CreditAccArmyCreate,		//勋章兵种建造加速
	BO_CreditMaintain,			//勋章维修
	BO_CreditMaintainAll,		//勋章全部维修
	BO_CreditMaxEnergy,			//作战演习室最大能量
	BO_CreditSlaveRoomHit,		//战俘营鞭笞
	BO_CreditSlaveRoomRansom,	//战俘营赎身

	BO_UpdateBldPlayReward = 61,//打赏升级18
	BO_UpdateBldPlayReward2,	//打赏升级180

	BO_UpdateBldChangeStyle,	//建筑变身

	BO_UpdateBldPlayReward_Army,//打赏升级18兵种
	BO_UpdateBldPlayReward2_Army,//打赏升级180兵种

	BO_UpdateBldPlayReward_Science,//打赏升级18科技
	BO_UpdateBldPlayReward2_Science,//打赏升级180科技

	BO_Chengjiu_Res = 70,//成就建筑资源注资
	BO_Chengjiu_Credit = 71,//成就建筑勋章注资

	BO_TrapAddArmy = 72,//陷阱中加入兵
	BO_TrapDelArmy = 73,//陷阱中分解兵
	BO_TrapCaptive_Res = 74,//陷阱使用资源俘虏兵

	BO_HeroLevelUp,		    //英雄升级
	BO_UnHeroLevelUp,	    //取消英雄升级
	BO_CreditHeroLevelUp,	//勋章英雄升级
	BO_AccHeroLevelUp,	    //勋章英雄升级加速
	BO_AccHeroRecover,	    //英雄加速回复
	BO_HeroRelive,	    //英雄复活
	BO_CreditHeroRelive,	    //勋章英雄复活
	BO_AllHeroRelive,	    //勋章英雄复活
	BO_CreditAllHeroRelive,	    //勋章英雄复活
	BO_TowerEnhance,	//防御塔强化
};

enum UpdateOperate
{
	UO_Unknow=0,
	UO_BldUpgrade,		//建筑建造或升级完成
	UO_ArmyProduce,		//兵种生产完成
	UO_ArmyUpgrade,		//兵种升级完成
	UO_Maintain,		//修理完成
	UO_MineProduce,		//地雷生产完成
	UO_MineUpgrade,		//地雷升级完成
	UO_AdminAddArmy,	//管理员加兵	
	UO_AdminDelArmy,	//管理员扣兵
	UO_AdminAddBld,		//管理员加建筑	
	UO_AdminDelBld,		//管理员删建筑
	UO_UpdateInfo,		//同步数据
	UO_SlaveRoom,		//同步战俘营数据
	UO_OverLoad,		//核电站超负载
	UO_HeroUpgrade,		//英雄升级
	UO_HeroRecover,		//英雄血量回复
	UO_TrapProduce,		//陷阱生产完成
	UO_TrapUpgrade,		//地雷升级完成
};

///纸牌颜色类型
enum PokerColorType
{
    POKER_SPADE = 0,
    POKER_HEART,
    POKER_CLUB,
    POKER_DIAMOND,
};

///纸牌的牌型
enum PokerHandShowType
{
    POKER_HS_UNKNOW = 0,
    POKER_HS_Single = 1,            //散牌
    POKER_HS_OnePair,               //一对
    POKER_HS_TwoPair,               //两对
    POKER_HS_Three,                 //三条
    POKER_HS_Straight,              //顺子
    POKER_HS_Flush,                 //同花
    POKER_HS_FullHouse,             //葫芦
    POKER_HS_Four,                  //四条
    POKER_HS_StraightFlush,         //同花顺
    POKER_HS_RoyalFlush,            //皇家同花顺
    POKER_HS_FIVE,                  //5条

    POKER_HS_Five,                  //5条，当前只能通过换牌获得
};

///纸牌屋操作类型
enum PokerOptType
{
    POKER_UNKOWN_OPT = 0,                   //未知操作
    POKER_CURRENT_GAMBLING = 1,             //获取当前的牌局
    POKER_DEAL_POKERS,                      //发牌
    POKER_SHOW_POKERS,                      //开牌
    POKER_REDEAL_POKERS,                    //重新发牌
    POKER_REPLACE,                          //换牌
    POKER_EXCHANGE,                         //兑换
    POKER_REFRESHLIST,                      //刷新对话列表
};

///纸牌屋开牌结果
enum PokerShowRtn
{
    POKER_Show_Win = 0,                  //获胜
    POKER_Show_Fail,                     //失败
    POKER_Show_Deuce,                    //平手
};

enum OperateRtn
{
	OR_Success = 0,		//正确
	OR_Unknow,			//未知错误
	OR_NoCenter,		//未找到指挥中心
	OR_NoBuilding,		//无建筑物
	OR_CreditError,		//客户端服务器端判断勋章数不一致
	OR_ErrorOptType,	//错误的操作类型
	OR_NoEnoughRes,		//资源不足
	OR_NoEnoughCredit,	//勋章不足
	OR_OutNum,			//超出最大数量
	OR_WrongType,		//错误的建筑类型
	OR_NoEnoughBld,		//现有建筑不满足条件
	OR_NoGroup,			//无生产队列
	OR_CreateError,		//创建失败
	OR_DeleteError,		//删除失败
	OR_WrongArmy,		//错误的兵种
	OR_UnlockArmy,		//兵种未解锁
	OR_OutArmyStorage,	//人口超出上限
	OR_NoFindArmy,		//未找到兵种
	OR_CreateFinish,	//建筑物已建筑完成
	OR_MaxLevel,		//达到最高等级
	OR_UpdateFinish,	//升级完成
	OR_NeedUnlockArmy,	//需解锁前置兵种
	OR_NeedBldLevel,	//建筑物等级不符
	OR_CanNotSell,		//无法回收
	OR_NoMineFatcroy,	//无地雷工厂
	OR_NoFindMine,		//未找到地雷
	OR_MineExit,		//地雷已存在
	OR_TimeError,		//时间错误
	OR_NoNeedMaintain,	//不需要维修
	OR_ErrorBuyType,	//错误的商品类型
	OR_MushRoomExist,	//报废汽车已经存在
	OR_NoMushRoomSt,	//客户端未发送报废汽车数据
	OR_NoMushRoom,	//未找到报废汽车
	OR_BldDestroy,	//建筑物已损坏
	OR_NotVIP,		//不是黄钻
	OR_PlayRewardFailed,	//打赏升级失败
	OR_RemouldUnOpen,	//相关改造未开启
	OR_RemouldNeedLvl,	//建筑等级不够
	OR_RemouldNeedScience,	//科技总和不够
	OR_RemouldNeedHonour,	//荣誉不够
	OR_RemouldNeedCredit,	//勋章不够
	OR_RemouldCostCredit,	//扣勋章失败
	OR_RemouldCostHonour,	//扣荣誉失败
	OR_RemouldFail,			//改造失败
	OR_RemouldFail_LvlLow,	//改造失败，降级
	OR_YellowBld_MustYellow,//必须是黄钻用户
	OR_YellowBld_MustYellowToday,//必须是当天充值或续费黄钻用户
	OR_SpecialItemNotEnough,//特殊物品不满足要求
	OR_SpecialItemCostError,//特殊物品扣除失败
	OR_OutTime,				//超出时间
	OR_NoSlaveRoom,			//没有对应俘虏
	OR_CanNotChgStyle,		//不能更改
	OR_ShopBuyMoreMax,		//商店购买超过上限
	OR_NoEnoughFeats,		//没有足够的功绩
	OR_PlayerLevelLow,		//玩家等级低
	OR_WaiGua,				//外挂
	OR_PlayerLvlErr,		//玩家等级不符
	OR_NoEnoughScore,		//没有足够竞技场积分
	OR_NoEnoughGScore,		//没有足够的工会贡献
	OR_NoGuild,				//未找到工会
	OR_GuildShopUpLimit,	//达到工会商品上限
	OR_NoPermit,			//无权限
	OR_UnLockPassWord,			//二级密码未解锁
	OR_TrapUnActive,			//陷阱非活跃状态
	OR_CanNotInterTrapArmy,			//不能掉进陷进
	OR_NotAchiveVipLevel,         //Vip等级不够
	OR_VipExpired,         //Vip过期

    ///纸牌屋操作结果
    OR_InsufficientPoker,              //纸牌数不够
    OR_NoDealPokers,                   //尚未发牌
    OR_NoReplaceCard,                  //勋章不足 无法换牌
    OR_InsufficientPoints,             //积分不够
    OR_NoExchange,                     //无指定的对换物品
    OR_WrongCard,                      //无指定的牌
    OR_OutEchangeLimit,                //兑换超出限额
    OR_NoEnoughPoints,                 //积分不足 用于刷新兑换列表时使用

    //卡片决斗
    OR_NoEnoughDuelCard,                //卡片决斗卡不足
    OR_NoMatchingOpponent,              //没有匹配的对手
    OR_WAITRIVALCHOOSE,                 //等待对手选牌
    OR_WrongActiveTime,                 //活动时间未到
	OR_NoHero,         //英雄不存在
	OR_HeroMaxLevel,   //英雄到达最大等级
	OR_HeroIsInTrain, //英雄正在训练当中
	OR_HeroNotInTrain,   //英雄不在训练中
	OR_HeroTrainIndexError,      //训练位错误
	OR_HeroTrainLock,   //训练位锁定
	OR_HeroFinishTrain,   //训练完成
	OR_HeroNotDie,      //英雄没有死亡
	OR_HeroFullHP,      //英雄满血
	OR_HeroIsDie,      //英雄死亡

    //年中好礼
    OR_NoEnoughTopUP,           //未达到领取条件
    OR_HasGetGift,              //已经领取过礼物

    //vip每日礼包
    OR_HasGetVipDailyGift,      //已经领取了vip每日免费礼包

    //红警月卡
    OR_HasGetDayCredit,         //已经领取每日勋章

    //情人节
    OR_VDG_SEND_OVER,         //超出每日发送限额
    OR_VDG_RECV_OVER,         //超出每日接收限额
    OR_VDG_DIF_REGION,        //不同区
    OR_VDG_NULL_RECV,         //获赠方ID无效
    OR_VDG_SENDSELF,          //赠送给自己
    OR_VDG_SENDSUCC,          //礼物发送成功

    //VIP月卡
    OR_HasGetVipMonthCardGift,  //已经领取过VIP月卡的每日奖励

    //临时军火商
    OR_HasRentSameWeaponCom,    //已经租用过同类型配件

    //探宝军火库
    OR_ExploreOutNum,           //超出探宝上限

    //全民抽奖
    OR_AllMen_NoEnoughPoint,
    OR_AllMen_HasGetGift,

    //额外配件位置
    OR_ExtraWPC_OutMaxNum,      //超出最大数量
    OR_ExtraWPC_WrongWPCLevel,  //其他配件的等级不够

    //每日充值抽奖
    OR_DailyRechargeLottery_HasRaffle,      //已经完成抽奖
    OR_DailyRechargeLottery_NoEnoughCredit, //充值额度不够

    //空间回流
    OR_BackFlow_HasGetGift,         //已经领取奖励
    OR_BackFlow_NeedMoreLimit,      //未达到条件

    //每日签到
    OR_DailySignIn_HasSignIn,       //已经签到
    OR_DailySignIn_hasGetGift,      //已经领取累计签到奖励
    OR_DailySignIn_WrongDay,        //不能补签没到的日期

    //新增升级兵种需要公会等级
    OR_NeedGuildLv,     //公会等级不够
	OR_TowerEnhance_TypeErr,		//防御塔强化,建筑或强化类型错误
	OR_TowerEnhance_ELvel,			//强化类型错误,需要更高等级

	//陷阱
	OR_NoTrapFatcroy,	//无陷阱工厂
	OR_NoFindTrap,		//未找到陷阱
	OR_TrapExist,		//陷阱已存在
};
//活动返回结果 通用
enum EventResult
{
	ER_Success,			//成功
	ER_Plat_Error,		//平台不正确
	ER_Repeat_Fetch,	//重复领取
	ER_Unkown_Error,	//未知错误
	ER_Time_Error,		//时间错误
	ER_Table_Error,		//表错误
	ER_Credit_Error,	//勋章不足	
	ER_BuyCnt_Error,	//购买的次数不足
	ER_Fetcher_Error,	//领取人错误

};
//礼包类型
enum GiftType
{
	GT_NewReCall_Gift =1,  //英雄回归
};

enum Army_Group
{
	ArmyGroup_Error= 0,
	ArmyGroup_Foot,
	ArmyGroup_Tank,
	ArmyGroup_Plane,
	ArmyGroup_NormalCnt, //普通兵种数量
	ArmyGroup_Hero_erth = ArmyGroup_NormalCnt,
	ArmyGroup_Hero_air,
	ArmyGroup_Cnt,
};

//建筑队列
enum Building_Group
{
	BldGroup_Upgrade = 0,	//建造、升级队列
	BldGroup_Count,			//队列数量
};

//建筑类型：
enum Building_Type
{
	BLD_ERROR			= 0,
	BLD_Mine			= 24,         //地雷,仅用于兼容flash接口，服务器代码中不属于建筑

	BLD_Metal_Factory   = 1,          //金属工厂              
	BLD_Oil_Pump        = 2,          //油井                  
	BLD_Metal_Storage   = 3,          //金属仓库              
	BLD_Oil_Storage     = 4,          //石油仓库              
	BLD_Mine_Factory    = 5,          //地雷工厂              
	BLD_Power_Plant     = 6,          //发电厂                
	BLD_Junkyard        = 9,          //回收工厂              
	BLD_War_Room        = 11,         //间谍卫星              
	BLD_Airfield        = 13,         //空指部                
	BLD_Command_Center  = 14,         //指挥中心              
	BLD_Wall            = 17,         //墙                    
	BLD_GoGo_Bar        = 19,         //作战演习室            
	BLD_Mortar_Tower    = 20,         //防空炮台              
	BLD_Gun_Turret      = 21,         //机枪塔                
	BLD_Bunker          = 22,         //堡垒                         
	BLD_Academy         = 26,         //步兵训练营            
	BLD_Barracks        = 29,         //兵工厂                
	BLD_War_Factory     = 30,         //战车工厂              
	BLD_Tech_Center     = 31,         //作战实验室
	BLD_Yellow_Gun_Turret = 41,		  //黄钻机枪塔
	BLD_Weapon_Center		= 42,				  //武器中心
	BLD_CamoMortar_Tower	= 43,     //伪装防空炮台              
	BLD_CamoGun_Turret		= 44,     //伪装机枪塔
	BLD_SlaveRoom			= 45,		//战俘营
	BLD_StrategicCamp		= 46,		//战略兵营
	BLD_TowerofPisa			= 47,		//比萨斜塔
	BLD_Eiffel				= 48,		//埃菲尔铁塔
	BLD_BigBen				= 49,		//大本钟
	BLD_Liberty				= 50,		//自由女神像
	BLD_RedSquare			= 51,		//红场
	BLD_StateBuilding		= 52,		//帝国大厦
	BLD_PrisimTower		    = 53,		//光棱塔
	BLD_VipTower		    = 54,		//VIP爱国者导弹塔
	BLD_Trap		        = 55,		//战车控制器
	BLD_Hero_Center		    = 56,		//英雄祭坛
    BLD_MagneticGyro        = 57,       //万磁陀螺
    BLD_TrinketSculpture    = 58,       //英雄徽章雕塑
	BLD_WarCollege			= 59,		//战争学院
	BLD_GiantCanon			= 60,		//法国巨炮
	BLD_TrapMaker			= 61,		//现金制造厂
	BLD_StaticTrap			= 62,		//静止陷阱
	BLD_ExplosiveTrap		= 63,		//爆炸陷阱
	BLD_MAX_TYPE_COUNT,				  //建筑类型的最大值
};

enum JunGongType
{
	JunGong_Unknow = 0,
	JunGong_Admin,		//管理员
	JunGong_WPC,		//武器中心
	JunGong_Admiral,	//拜访将领
};

enum CREDIT_TYPE_DEF
{
	CREDIT_TYPE_DEF_ADMIN = 0,		//0:管理员加勋章 
	CREDIT_TYPE_DEF_BUY,			//1：购买勋章 
	CREDIT_TYPE_DEF_ASKFRIEND,		//2：邀请好友 
	CREDIT_TYPE_DEF_TIAOFU,			//3:条幅活动 
	CREDIT_TYPE_DEF_WEB,			//4:web活动 
	CREDIT_TYPE_DEF_GIFT,			//5:礼物 
	CREDIT_TYPE_DEF_DEFENSENPC,		//6:防御NPC
	CREDIT_TYPE_DEF_NEWCOMER,		//7:新手礼物
	CREDIT_TYPE_DEF_PLATINTIVE,		//8:邀请好友
	CREDIT_TYPE_DEF_EXPLORE,		//9：探索
	CREDIT_TYPE_DEF_YELLOW,		    //10：黄钻活动
	CREDIT_TYPE_DEF_BATTLE,		    //11：战斗
	CREDIT_TYPE_DEF_ITEM,		    //12：使用物品
	CREDIT_TYPE_DEF_HOLIDAY_DRAW,   //13：节日抽奖
	CREDIT_TYPE_DEF_REGET,			//14：购买礼物失败勋章恢复
	CREDIT_TYPE_DEF_PVEGIFT,		//15: PVE爬塔获得
	CREDIT_TYPE_DEF_FRIENDEXPLORE,	//16: 基地探宝获得
	CREDIT_TYPE_DEF_UNKNOW,			//17：未知
	CREDIT_TYPE_DEF_SELCOUNTRY,		//18：选择国家送勋章
	CREDIT_TYPE_DEF_COUPON_DRAW,	//19：点券抽奖
	CREDIT_TYPE_DEF_INTIVE_CREDIT,	//20：邀请好友
	CREDIT_TYPE_DEF_ITEM_BOX1,		//21：装备补给箱1
	CREDIT_TYPE_DEF_ITEM_BOX2,		//22：装备补给箱2
	CREDIT_TYPE_DEF_ITEM_BOX3,		//23：装备补给箱3
	CREDIT_TYPE_DEF_EXPLORE_1,		//24：野外探索
	CREDIT_TYPE_DEF_EXPLORE_2,		//25：工厂探索
	CREDIT_TYPE_DEF_EXPLORE_3,		//26：实验室探索
	CREDIT_TYPE_DEF_HOLIDAY_DRAW_CHICKEN,	//27：火鸡
	CREDIT_TYPE_DEF_HOLIDAY_DRAW_YUANDAN,	//28：砸蛋
	CREDIT_TYPE_DEF_CATHECTIC,				//29：幸运卷返回
	CREDIT_TYPE_DEF_LEVEL_BUY,			//30：等级礼包
    CREDIT_TYPE_DEF_BUY_QPOINT,         //31:购买Q点或金券
    CREDIT_TYPE_DEF_WCBET,              //32:世界杯竞猜奖励
    CREDIT_TYPE_DEF_MIDDLEYEAR,         //33:年中好礼购买
    CREDIT_TYPE_DEF_MONTHCARD,          //34;月卡每日领取勋章
    CREDIT_TYPE_DEF_MOONCOMPENSATE,     //35：魔盒补偿的3888勋章
    CREDIT_TYPE_DEF_DAILYSIGNIN,        //36:每日签到勋章
    CREDIT_TYPE_DEF_BUY_YEEPAY,         //37:yeepay渠道购买勋章
    CREDIT_TYPE_DEF_WARGAME_RETURN,     //38:军事演习返还
	CREDIT_TYPE_DEF_DOUBLE_ELEVEN,      //双十一空降军需官奖励
    CREDIT_TYPE_DEF_REDENVELOPE,        //40:红包返还
    CREDIT_TYPE_DEF_ZHAOCAIJINBAO,        //41:招财进宝
};

enum CREDIT_BUY_CHANNEL
{//勋章购买渠道
    CREDIT_BUY_FROM_QQ = 0, //QQ购买
    CREDIT_BUY_FROM_YEEPAY,
};

enum CREDIT_COST_TYPE_DEF
{
	CREDIT_COST_TYPE_DEF_ADMIN = 0,	//0:管理员扣勋章
	CREDIT_COST_TYPE_DEF_BUY_RES,	//1:购买资源
	CREDIT_COST_TYPE_DEF_BUY_ENL,	//2:购买领土扩张
	CREDIT_COST_TYPE_DEF_BUY_BIP,	//3:购买资源压缩
	CREDIT_COST_TYPE_DEF_SHOP_BUY,	//4:商城购买
	CREDIT_COST_TYPE_DEF_RECOVERARMY,//5:一键补兵
	CREDIT_COST_TYPE_DEF_SCIENCEUP,	//6:科技升级
	CREDIT_COST_TYPE_DEF_ADDBUFF,	//7:加buff
	CREDIT_COST_TYPE_DEF_BLDCHG,//8:建筑修改
	CREDIT_COST_TYPE_DEF_WEAPONCENTER, //9:武器中心
	CREDIT_COST_TYPE_DEF_LEAVEMESSAGE, //10:留言
	CREDIT_COST_TYPE_DEF_SLAVEROOM_RANSOM, //11:战俘营赎身
	CREDIT_COST_TYPE_DEF_SLAVEROOM_HIT, //12:战俘营鞭笞
	CREDIT_COST_TYPE_DEF_WALLCHGCLOTH,	//13:围墙换装
	CREDIT_COST_TYPE_DEF_BAGUPGRADE,	//14:背包扩充
	CREDIT_COST_TYPE_DEF_PLAYREWARD,	//15:打赏专家
	CREDIT_COST_TYPE_DEF_VISITADMITAL,	//16:拜访将领
	CREDIT_COST_TYPE_DEF_SHOPFORGIFT,	//17:赠送礼物
	CREDIT_COST_TYPE_DEF_STRATEGIC_TREET,	//18:战略兵营治疗
	CREDIT_COST_TYPE_DEF_PVE_BUYTIMES,	//19:PVE购买次数
	CREDIT_COST_TYPE_DEF_LUCKYNUM,		//20:幸运数字
	CREDIT_COST_TYPE_DEF_PRIVILEGE,	//幸运魔盒
	CREDIT_COST_TYPE_DEF_ARMYGRADE,  //22:兵种阶别转移
	CREDIT_COST_TYPE_DEF_CHGCOUNTRY,	//切换国家
	CREDIT_COST_TYPE_DEF_SPY,			//城市侦查
	CREDIT_COST_TYPE_DEF_ADD_THEW,		//恢复体力
	CREDIT_COST_TYPE_DEF_ADD_MORALE,	//恢复士气
	CREDIT_COST_TYPE_DEF_LUCKY_WHEEL,	//幸运轮盘刷新
	CREDIT_COST_TYPE_DEF_DAILY_COST,	//每日充值补充
	CREDIT_COST_TYPE_DEF_COM_DOWNGRADE, //配件分解
	CREDIT_COST_TYPE_DEF_ARENABUY,		//竞技场购买
	CREDIT_COST_TYPE_DEF_LUCKY_BOX,		//幸运魔盒
	CREDIT_COST_TYPE_DEF_EQUIP_MIGRATE,  //装备强化等级转移
	CREDIT_COST_TYPE_DEF_ZHUZI,  //注资
	CREDIT_COST_TYPE_DEF_REDRAW,  //注资
	CREDIT_COST_TYPE_DEF_WORLDARENABUY,		//世界竞技场购买
	CREDIT_COST_TYPE_DEF_ACTIVE_SHOP,	//活动购买
	
	CREDIT_COST_TYPE_DEF_CREATE_GUILD,	//创建工会
	CREDIT_COST_TYPE_DEF_GUILD_CONTRIBUTE, //工会捐献
	CREDIT_COST_TYPE_DEF_SEVENDAYS, //十一活动
	CREDIT_COST_TYPE_DEF_GUILD_EMBRAVE,		//工会战鼓舞
	CREDIT_COST_TYPE_DEF_LEVEL_BUY,		//等级礼包
	CREDIT_COST_TYPE_DEF_BUYVIPCARD,		//开通Vip
	CREDIT_COST_TYPE_DEF_SEVENDAYS_TARGET,	//七日目标
	CREDIT_COST_TYPE_DEF_BOOK_COLLECTION_HALF,	//半价书籍收集
	CREDIT_COST_TYPE_DEF_BOOK_COLLECTION_WHOLE,	//全价书籍收集
	CREDIT_COST_TYPE_DEF_REGIONARENABUY,		//跨区竞技场购买
	CREDIT_COST_TYPE_DEF_PVE_BATTLE,	//PVE战役
	CREDIT_COST_TYPE_DEF_PVE_BUY,
    CREDIT_COST_TYPE_DEF_POKER_REPLACE,     ///纸牌屋换牌
	CREDIT_COST_TYPE_DEF_VISITHERO,	//招募英雄
	CREDIT_COST_TYPE_DEF_HEROTREAT,	//治疗英雄
	CREDIT_COST_TYPE_DEF_CHANGEEXP,	//英雄传授

	CREDIT_COST_TYPE_DEF_REGIONCOUNTRY_SPY,			//跨区国战城市侦查
	CREDIT_COST_TYPE_DEF_REGIONCOUNTRY_ADD_THEW,	//跨区国战恢复体力
	CREDIT_COST_TYPE_DEF_RESETPROPERTY,	//重置改造
	CREDIT_COST_TYPE_DEF_GVE_ADD_THEW,
	CREDIT_COST_TYPE_DEF_HEROARENABUY,		//英雄竞技场购买
	CREDIT_COST_TYPE_DEF_RESOURCE_SEARCH20, //资源探索
	CREDIT_COST_TYPE_DEF_RESOURCE_SEARCH100, //资源探索100
	CREDIT_COST_TYPE_DEF_FLUSH_GUILDTASK,//刷新工会任务
	CREDIT_COST_TYPE_DEF_COMPLETE_GUILDTASK,//勋章完成工会任务

	CREDIT_COST_TYPE_DEF_GVG_EMBRAVE,		//GVG鼓舞
	CREDIT_COST_TYPE_DEF_GVG_STREAK,		//GVG购买连胜次数

    CREDIT_COST_TYPE_DEF_EXPLORE_SECRET_ARSENAL,    //探宝军火库消耗勋章

    CREDIT_COST_TYPE_DEF_RENT_WEAPONCOM,            //租用配件消耗勋章

	CREDIT_COST_TYPE_DEF_RESET_WCSKILLTRAINTIMES,	//战争学院重置技能训练次数//功能取消
	CREDIT_COST_TYPE_DEF_BUY_WCSKILLTRAINTIMES,		//战争学院购买技能训练次数

    CREDIT_COST_TYPE_DEF_DAILYSIGNIN,               //每日签到补签消耗勋章

    CREDIT_COST_TYPE_DEF_WARGAMEBUYTOKEN,           //购买演习令
    CREDIT_COST_TYPE_DEF_WARGAME_DELAY,             //军事演习延期

    CREDIT_COST_TYPE_DEF_MILITARYSOU_LOCKSKILL,     //锁定技能
	CREDIT_COST_TYPE_DEF_BUY_WQLB,					//购买围墙礼包

	CREDIT_COST_TYPE_DEF_CREDITREBATE,				//小冲值大折扣
	CREDIT_COST_TYPE_BIG_GIFT,						//十一月的距惠活动
    CREDIT_COST_TYPE_REDUCE_ADMIRALPROP,            //减少将领属性
	CREDIT_COST_TYPE_DEF_TOWERENHANCE,				//防御塔强化
	CREDIT_COST_TYPE_DEF_TOWERENHANCE_2,
	CREDIT_COST_TYPE_DEF_TOWERENHANCE_3,			//web统计那边需要按等级分
	CREDIT_COST_TYPE_DEF_CHRISTMAS_1,					//圣诞节活动礼包第一档
	CREDIT_COST_TYPE_DEF_CHRISTMAS_2,					//圣诞节活动礼包第二档
	CREDIT_COST_TYPE_DEF_CHRISTMAS_3,					//圣诞节活动礼包第三档
    CREDIT_COST_TYPE_DEF_JUMPDEFENSECUT,                //周五六防守跳到15关

    CREDIT_COST_TYPE_DEF_BOSS_EMBRAVE,		//boss战鼓舞
    CREDIT_COST_TYPE_DEF_BOSS_RE,			//boss战复活

    CREDIT_COST_TYPE_DEF_MEDALHONOR_DECORATE,   //荣誉勋章授勋

    CREDIT_COST_TYPE_DEF_ZHAOCAIJINBAO,			//招财进宝消耗勋章
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_1, //一元购活动
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_2, //一元购活动
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_3, //一元购活动
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_4, //一元购活动
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_5, //一元购活动
	CREDIT_COST_TYPE_DEF_ONE_YUAN_PURCHASE_6, //一元购活动

	CREDIT_COST_TYPE_STATIC_TRAP_UPGRADE,//静态陷阱升级
	CREDIT_COST_TYPE_EXPLOSIVE_TRAP_UPGRADE,//爆炸陷阱升级
	CREDIT_COST_TYPE_TRAP_ACC_PRC,//陷阱加速生产
	CREDIT_COST_TYPE_STATICTRAP_ACC_UPGRADE,//静态陷阱加速升级
	CREDIT_COST_TYPE_EXPLOSIVETRAP_ACC_UPGRADE,//爆炸陷阱加速升级
};

enum CREDIT_COST_ARG2_DEF
{
	CREDIT_COST_ARG2_DEF_NULL = 0,			//0:无
	CREDIT_COST_ARG2_DEF_PLAYREWARD_ARMY,	//兵种升级
	CREDIT_COST_ARG2_DEF_PLAYREWARD_BUILD,	//建筑升级
};

enum FEATS_TYPE_DEF
{
	FEATS_TYPE_DEF_UNKNOW = 0,
	FEATS_TYPE_DEF_ADMIN,	//管理员加功勋
	FEATS_TYPE_DEF_FIGHT,		//战斗获得功勋
	FEATS_TYPE_DEF_SlAVEROOM,	//战俘营收取
	FEATS_TYPE_DEF_HIT,			//鞭笞获得
	FEATS_TYPE_DEF_SELLEMBLEM,	//出售徽记
	FEATS_TYPE_DEF_SHARE,		//分享游戏
	FEATS_TYPE_DEF_QPLUS,		//空间回流礼包
	FEATS_TYPE_DEF_PVEADDHONOR,	//PVE
	FEATS_TYPE_DEF_DRAW,		//抽奖
	FEATS_TYPE_DEF_EXPLORE,	   //基地探宝
	FEATS_TYPE_DEF_ITEM,		//背包物品
	FEATS_TYPE_DEF_REWARD,		//
};

enum FEATS_COST_TYPE_DEF
{
	FEATS_COST_TYPE_DEF_SHOP = 0,	//商店购买消耗
	FEATS_COST_TYPE_DEF_GUILD_CONTRIBUTE, //工会捐献
	FEATS_COST_TYPE_DEF_BOSSBTL, //BOSS战鼓舞
};

enum ARENA_SC_COST_TYPE_DEF
{
	ARENA_SC_COST_TYPE_SHOP = 0,	//商店购买消耗
};

enum EMBLEM_ADD_TYPE_DEF
{
	EMBLEM_ADD_TYPE_DEF_Admin = 0,	//管理员
	EMBLEM_ADD_TYPE_DEF_SHOP,		//商店购买徽记
	EMBLEM_ADD_TYPE_DEF_WPC_UP,		//武器中心升级
	EMBLEM_ADD_TYPE_DEF_WPC_TurnOff,//武器中心卸下
	EMBLEM_ADD_TYPE_DEF_WPC_GET,	//武器中心获得
	EMBLEM_ADD_TYPE_DEF_ITEM_USE,	//物品使用
	EMBLEM_ADD_TYPE_DEF_EXPLORE_GET,    //基地探宝获得
    EMBLEM_ADD_TYPE_DEF_POKER_REWARD,   //纸牌兑换
	EMBLEM_ADD_TYPE_DEF_EXTRA,   //额外位置开启奖励

};

enum EMBLEM_COST_TYPE_DEF
{
	EMBLEM_COST_TYPE_DEF_Admin= 0,	//管理员
	EMBLEM_COST_TYPE_DEF_WPC_UP,	//武器中心升级
	EMBLEM_COST_TYPE_DEF_WPC_TurnOn,//武器中心装备
	EMBLEM_COST_TYPE_DEF_Destroy,	//销毁	
};

enum HONOR_TYPE_DEF
{
	HONOR_TYPE_DEF_ADMIN = 0,	//管理员加荣誉
	HONOR_TYPE_DEF_FIGHT,		//战斗获得荣誉
	HONOR_TYPE_DEF_CBKFRIEND,	//老友召回获得荣誉
	HONOR_TYPE_DEF_WEBGIFT,		//web活动
	HONOR_TYPE_DEF_GIFT,		//礼物	
	HONOR_TYPE_DEF_Extend,		//推广活动
	HONOR_TYPE_DEF_DEFENSENPC,	//防御NPC
	HONOR_TYPE_DEF_DEFENCEPLAYER,	//玩家战斗防守方获得荣誉
	HONOR_TYPE_DEF_NEWCOMER,	//新手礼包
	HONOR_TYPE_DEF_EXPLORE,		//探索
	HONOR_TYPE_DEF_SlaveRoom,	//战俘营
	HONOR_TYPE_DEF_PVEADDHONOR,	//PVE爬塔获得
	HONOR_TYPE_DEF_Item,		//使用物品	
	HONOR_TYPE_DEF_REWARD,		//	
    HONOR_TYPE_DEF_ScienceFailReturn,   //升级科技失败返还

};

enum OBJECT_TYPE
{
	OBJECT_INVAILD = 0,
	OBJECT_BUILD,
	OBJECT_ARM,
	OBJECT_TECHNOLOGY,
	OBJECT_DEFENCENPC,
	OBJECT_PLAYREWARD,
	OBJECT_UPDATEREMOULDLVL,
	OBJECT_PLAYREWARD_ARMY,
	OBJECT_PLAYREWARD_SCIENCE,
	OBJECT_PVE_FIGHT,
};

enum NEWCOMERGIFT_TYPE
{
	NEWCOMERGIFT_TYPE_INVAILD = 0,
	NEWCOMERGIFT_TYPE_MEDAL,		//勋章
	NEWCOMERGIFT_TYPE_METAL,		//金属
	NEWCOMERGIFT_TYPE_OIL,			//石油
	NEWCOMERGIFT_TYPE_ARMY,			//兵
	NEWCOMERGIFT_TYPE_HONOUR,		//荣誉
	NEWCOMERGIFT_TYPE_BAGITEM,		//背包
	NEWCOMERGIFT_TYPE_GIFT,			//礼物
};

enum NEWCOMERGIFT_ADD_TYPE
{
	NEWCOMERGIFT_ADD_TYPE_INVAILD = 0,
	NEWCOMERGIFT_ADD_TYPE_LEVELUP,		//等级提升
	NEWCOMERGIFT_ADD_TYPE_ATTACKNPC,	//打败NPC
	NEWCOMERGIFT_ADD_TYPE_LUCKYGAIN,	//幸运星
	NEWCOMERGIFT_ADD_TYPE_MARKGIFT,		//作为礼物标记
	NEWCOMERGIFT_ADD_TYPE_MARKLOGIN7DAY,//连续登陆7天
	NEWCOMERGIFT_ADD_TYPE_MARKLOGIN7DAY_28,//第二天及第八天领礼物
};

enum CHECK_START_REASON
{
	CHECK_START_REASON_OK,				//OK
	CHECK_START_REASON_DIFFACTIVITY,	//不同活动
	CHECK_START_REASON_RES,				//资源问题
	CHECK_START_REASON_PLAYERNULL,		//玩家为空
	CHECK_START_REASON_CUSTOMNULL,		//关卡找不到
	CHECK_START_REASON_HASATTACK,		//还在进攻
	CHECK_START_REASON_CUSTOMUNR,		//获取礼物关卡数不对
	CHECK_START_REASON_HASGET,			//已经领取了礼物
	CHECK_START_REASON_LEVELLESS,       //等级不够
	CHECK_START_REASON_NOTFINISHNEW,       //未完成新手引导
	CHECK_START_REASON_MINLV,			//未达到等级或战斗力
    CHECK_START_REASON_NEEDMORECREDIT,      //跳过关卡时 勋章不足
    CHECK_START_REASON_HASOVERMAXCUT,       //已经超过要跳过的关卡
};

enum DEFENSE_END_REASON
{
	DEFENSE_END_REASON_NORMAL,			//正常结束
	DEFENSE_END_REASON_LOSTCONN,		//连接丢失
	DEFENSE_END_REASON_CLIENTERROR,		//客户端数据错误
	DEFENSE_END_REASON_ZIHUIZHONGXIN,	//指挥中心被摧毁
	DEFENSE_END_REASON_OVERTIME_5,		//5分钟超时
	DEFENSE_END_REASON_OVERTIME_ALL,	//活动结束
	DEFENSE_END_REASON_USERACNCEL,		//玩家取消
	DEFENSE_END_REASON_REPLACED,		//被顶掉
	DEFENSE_END_REASON_RESERROR,		//服务器配置错误
	DEFENSE_END_REASON_ASKENDERROR,		//发送错误结束消息
	DEFENSE_END_REASON_OVERTIME_5_S,	//服务器端验证超时
};

//战报类型枚举
enum EQUIPNODE_SHOWTYPE
{
	EQUIPNODE_SHOWTYPE_ARMY,			//部队
	EQUIPNODE_SHOWTYPE_BUILDING,		//建筑
	EQUIPNODE_SHOWTYPE_LEAVE,			//撤退
	EQUIPNODE_SHOWTYPE_ATTACK_1,		//进攻方向 开始
	EQUIPNODE_SHOWTYPE_ATTACK_2,
	EQUIPNODE_SHOWTYPE_ATTACK_3,
	EQUIPNODE_SHOWTYPE_ATTACK_4,
	EQUIPNODE_SHOWTYPE_ATTACK_5,
	EQUIPNODE_SHOWTYPE_ATTACK_6,
	EQUIPNODE_SHOWTYPE_ATTACK_7,
	EQUIPNODE_SHOWTYPE_ATTACK_8,		//进攻方向 结束
	EQUIPNODE_SHOWTYPE_RETREAT,			//撤退损失兵力
	EQUIPNODE_SHOWTYPE_GAINHONOR,		//获得荣誉
	EQUIPNODE_SHOWTYPE_SPRWPN_ARMY,		//超级武器破坏军队
	EQUIPNODE_SHOWTYPE_SPRWPN_BUILD,	//超级武器破坏建筑
	EQUIPNODE_SHOWTYPE_SPRWPN_EMPTY,	//超级武器未造成伤害
	EQUIPNODE_SHOWTYPE_TRAP_ARMY,	    //陷进捕获
};

enum BROADCAST_ROBOT_MSG
{
	BROADCAST_ROBOT_MSG_UPDATE = 1,		//间隔1分钟的Update消息
	BROADCAST_ROBOT_MSG_HASSEND,		//发完公告的全服通知
};


//加Buff错误
enum E_ADDBUFF_ERROE
{
	E_ADDBUFF_OK = 0,					//OK
	E_ADDBUFF_ERROR_BUILD_ID,			//建筑ID错误
	E_ADDBUFF_NOTMORE100CREDIT,			//不够一百勋章
	E_ADDBUFF_HASGAINEDBUFF,			//已经有这个buff了
	E_ADDBUFF_CREDITNOTMORE300,			//勋章不足300
	E_ADDBUFF_CREDITCOST300,			//扣除300勋章失败
	E_ADDBUFF_BuyVipCardFail,			//vip续费失败
	E_ADDBUFF_NeedVipLevel,			        //vip等级不够
};
//邀请好友操作
enum PLAT_INVITEFRIEND
{
	PLAT_INVITEFRIEND_INVITE = 1,
	PLAT_INVITEFRIEND_ARGCHG,
	PLAT_INVITEFRIEND_ARGCHG_SELF,
	PLAT_INVITEFRIEND_GAINCREDIT,
};

//邀请好友奖励
enum PLAT_INVITEFRIEND_GAIN
{
	PLAT_INVITEFRIEND_GAIN_ALL = 0,
	PLAT_INVITEFRIEND_GAIN_1,
	PLAT_INVITEFRIEND_GAIN_LVL,
	PLAT_INVITEFRIEND_GAIN_CREDIT,
};

//邀请好友领取错误
enum PLAT_INVITEFRIEND_GAIN_ERROR
{
	PLAT_INVITEFRIEND_GAIN_ERROR_OK = 0,	//成功
	PLAT_INVITEFRIEND_GAIN_ERROR_1,			//已经领取过邀请礼包
	PLAT_INVITEFRIEND_GAIN_ERROR_1_1,		//达到今日邀请领取上线
	PLAT_INVITEFRIEND_GAIN_ERROR_LVL1,		//好友等级未达到下一领取等级
	PLAT_INVITEFRIEND_GAIN_ERROR_LVL2,		//已经全部领取完毕
	PLAT_INVITEFRIEND_GAIN_ERROR_LVL3,		//达到今日升级领取上线
	PLAT_INVITEFRIEND_GAIN_ERROR_CREDIT,	//达到今日充值领取上限
};

//留言错误
enum LEAVE_MESSAGE_ENUM
{
	ENUM_LEAVE_MESSAGE_OK,					//成功
	ENUM_LEAVE_MESSAGE_NOCREDIT,			//勋章不够
	ENUM_LEAVE_MESSAGE_DEFAULT_ID_ERR,		//默认ID错误
	ENUM_LEAVE_MESSAGE_SELF_STR_LEN_ERR,	//自定义语句长度错误
	ENUM_LEAVE_MESSAGE_UNKNOWN,				//未知错误
};

//战斗力设置枚举
//对应表combatPowerSet.dat
enum COMBATPOWERTYPE
{
	COMBATPOWERTYPE_NULL = 0,
	COMBATPOWERTYPE_ARMY_LIFE,				//兵种生命/点
	COMBATPOWERTYPE_ARMY_DPS,				//兵种DPS/点
	COMBATPOWERTYPE_ARMY_RANGE,				//兵种射程/点
	COMBATPOWERTYPE_PART_ATTACK,			//零件攻击力/1%
	COMBATPOWERTYPE_PART_LIFE,				//零件生命值/1%
	COMBATPOWERTYPE_PART_RANGE,				//零件射程/1点
	COMBATPOWERTYPE_PART_MOVE,				//零件移动速度/1%
	COMBATPOWERTYPE_PART_TURN,				//零件转向速度/1%
	COMBATPOWERTYPE_PART_DAMAGE_SOLDIER,	//对士兵伤害加成/1%
	COMBATPOWERTYPE_PART_DAMAGE_TANK,		//对坦克伤害加成/1%
	COMBATPOWERTYPE_PART_DAMAGE_PLAN,		//对飞机伤害加成/1%
	COMBATPOWERTYPE_PART_DAMAGE_BUILD,		//对建筑伤害加成/1%
	COMBATPOWERTYPE_PART_CRT,			    //零件暴击率/1%
	COMBATPOWERTYPE_PART_CRTDAMAGE,			//零件暴击伤害幅度/1%
	COMBATPOWERTYPE_PART_CRTDEF,			//零件暴击防御/1%
	COMBATPOWERTYPE_PART_DAMAGEDEF,			//零件伤害防御/1%
	COMBATPOWERTYPE_PART_DAMAGE_HERO,		//对英雄伤害加成/1%
	COMBATPOWERTYPE_PART_DEF_SOLDIER,	//对士兵防御加成/1%
	COMBATPOWERTYPE_PART_DEF_TANK,		//对坦克防御加成/1%
	COMBATPOWERTYPE_PART_DEF_PLAN,		//对飞机防御加成/1%
	COMBATPOWERTYPE_PART_DEF_BUILD,		//对建筑防御加成/1%
	COMBATPOWERTYPE_PART_DEF_HERO,		//对英雄防御加成/1%
    COMBATPOWERTYPE_PART_DEF_GUNTOWER,  //对机枪塔防御加成/1%
    COMBATPOWERTYPE_PART_DEF_MortarTower,//对爱国者防御加成/1%
    COMBATPOWERTYPE_PART_DEF_ReduceTowerHp, //减少敌方 防御塔 血量

};
//兵种类型：
enum Army_Type
{
	Army_Unkonw			= 0,		
	Army_Grunt          = 2,		// 步枪兵     
	Army_Ripper         = 3,		// 机枪兵     
	Army_Blaster        = 4,		// 炮兵       
	Army_Torch          = 5,		// 喷火兵     
	Army_Ghost          = 6,		// 狙击手     
	Army_Boomer         = 7,		// 恐怖分子   
	Army_Gearhead       = 8,		// 工程师     
	Army_Doc            = 9,		// 医生
	Army_Little_Dog     = 10,		// 轻型履带车 
	Army_Jack_Rabbit    = 11,		// 火箭越野车 
	Army_Badger         = 12,		// 防空车     
	Army_Crossbow       = 13,		// V3火箭车   
	Army_Loki           = 14,		// 灰熊坦克   
	Army_Grendel        = 15,		// 犀牛坦克   
	Army_Armadillo      = 16,		// 坦克杀手   
	Army_LaserTank		= 17,		//光凌坦克
	Army_Big_Dog        = 21,		// 天启坦克

	Army_Firefly        = 22,		// 入侵者飞机 
	Army_Osprey         = 23,		// 武装直升机 
	Army_Locust         = 24,		// 夜莺直升机 
	Army_Thunderbolt    = 26,		// 雷霆战机   
	Army_Nightstalker   = 27,		// 黑鹰战机   
	Army_Roadrunner     = 28,		// 恐怖机器人
	Army_Comanche		= 29,		//虎式攻击机
	Army_Airship		= 30,		// 基洛夫飞艇
	Army_AntiMineTank	= 31,		// 扫雷坦克
	Army_GoldTank		= 32,		//黄金天启坦克
	Army_Tanya          = 33,		// 谭雅  
	Army_Magnet			= 34,		//磁暴坦克
	Army_GoldMagnet		= 35,		//黄金磁暴坦克
	Army_RedXi			= 36,		//年兽
	Army_GoldPurpleXi	= 37,		//黄金年兽
	Army_TeZhong        = 38,		//中国对空特种兵
	Army_MagnetS        = 39,       //精英磁暴坦克
	Army_GoldMegaTankS  = 40,       //精英黄金天启坦克
	Army_LaserTankS     = 41,       //精英光棱坦克
	Army_FinalTank		= 42,		//末日天启
	Army_ThorMecha		= 43,		//雷神机甲
	B25Fly              = 44,       //B-25轰炸机
	Mammoth             = 45,       //猛犸坦克
	Army_GoldLoki		= 46,		//黄金灰熊
	Army_GoldGrendel	= 47,		//黄金犀牛
	Army_GoldArmadillo	= 48,		//黄金坦克杀手
	Army_GoldenMammoth      = 49,       //黄金猛犸坦克
	Army_NoDieGearhead      = 50,       //不死工程师
    Army_ChristMammoth      = 51,       //圣诞猛犸
    Army_FrostTanks         = 52,       //冰霜坦克
    Army_FreezeCorps        = 53,       //冷冻军团
    Army_MonarchTanks       = 54,       //帝王坦克
    Army_GoldenMonarch      = 55,       //黄金帝王坦克
    Army_FinalMonarch       = 56,       //末日帝王坦克
    Army_BossFreeze	        = 57,       //冰霜BOSS
    Army_BossLaser	        = 58,       //光棱BOSS
    Army_BossBig_Dog	    = 59,       //天启BOSS
    Army_BossThorMecha	    = 60,       //雷神BOSS
    Army_BossMammoth	    = 61,       //猛犸BOSS
    Army_BossRedXi	        = 62,       //年兽BOSS
	Army_T90				= 79,		//t90
	Army_Max,						//最大兵种
};
//兵种AI类型
enum ArmyAI_Type
{
	AI_NONE,  //无优先
	AI_TOLAND_FIRST,//优先对地
	AI_TOAIR_FIRST, //优先对空
};
//战绩相关
const int Records_Damage = 10;
const int Records_Max_Foot_ID = 9;

//徽记广播
const int Min_Emblem_Broadcast = 7;
//书籍最大装备位
const int MAX_BOOK_EQUIPPOINT = 8;
const int BOOK_EQUIPPOINTADD = 3;
const int MAX_COM_EQUIPPOINT = 6;
const int MAX_EMBLEM_EQUIPPOINT = 6;

enum PLAYER_RECORD
{
	PLAYER_RECORD_START = 1000,
	PLAYER_RECORD_PLUNDER_OIL = 1001,
	PLAYER_RECORD_PLUNDER_METAL = 1002,
	PLAYER_RECORD_HONOR = 1003,
	PLAYER_RECORD_KILL_SOLDIER = 1004,
	PLAYER_RECORD_KILL_ARMOR = 1005,
	PLAYER_RECORD_DISTROY_BLD = 1006,
	PLAYER_RECORD_ATTACK_TIMES = 1007,
	PLAYER_RECORD_DISTROY_COMMAND_CENTER = 1008,
	PLAYER_RECORD_PRODUCE_EARTH = 1009,
	PLAYER_RECORD_PRODUCE_AIR = 1010,
	PLAYER_RECORD_PRODUCE_OIL = 1011,
	PLAYER_RECORD_PRODUCE_METAL = 1012,
	PLAYER_RECORD_CAPTURE_TIMES = 1013,
	PLAYER_RECORD_FEATS = 1014,
	PLAYER_RECORD_TECHNOLOGY = 1015,
	PLAYER_RECORD_COUTRY_ZHANGONG = 1016,
	PLAYER_RECORD_COUTRY_ATTACK = 1017,
	PLAYER_RECORD_COUTRY_WIN = 1018,
	PLAYER_RECORD_PVP_SCORE = 1019,
	PLAYER_RECORD_PVP_CHELLENGE = 1020,
	PLAYER_RECORD_PVP_CHELLENGE_WIN = 1021,
	PLAYER_RECORD_PVP_RANK = 1022,
	PLAYER_RECORD_PVE_MAX_CUSTOMINDEX = 1023,
	PLAYER_RECORD_BASEDEFENDNPC_MAXSTAGE = 1024,
	PLAYER_RECORD_DEFEND_CHELLENGE_MAXCUT = 1025,
	
	PLAYER_RECORD_END ,
};

//商店购买时间标记类型
enum SHOP_BUY_MARK_TYPE
{
	SHOP_BUY_MARK_TYPE_DAY = 1,
	SHOP_BUY_MARK_TYPE_WEEK,
	SHOP_BUY_MARK_TYPE_MONTH,
	SHOP_BUY_MARK_TYPE_YEAR,
};

enum INVITEINTERFACE
{
	INVITEINTERFACE_START = 0,
	INVITEINTERFACE_I     = 1,
	INVITEINTERFACE_URLI  = 2,
};



enum E_ADDARMY_TYPE
{
	E_ADDARMY_TYPE_ADMIN = 0,		//GM
	E_ADDARMY_TYPE_DEFENSE_CUSTOM,	//防守关卡
	E_ADDARMY_TYPE_DEFENSE_GIFT,	//防守礼物
	E_ADDARMY_TYPE_GIFT,			//礼物
	E_ADDARMY_TYPE_PVEARMY,			//PVE爬塔获得
	E_ADDARMY_TYPE_TRAP,            //陷阱俘虏
	E_ADDARMY_TYPE_VIP,             //ViP获得
    E_ADDARMY_TYPE_EXPLOREARSENAL,  //探宝军火库
    E_ADDARMY_TYPE_MAGICBOX,        //魔盒
    E_ADDARMY_TYPE_COSTCREDIT,      //你消费 我送礼活动
    E_ADDARMY_TYPE_DAILYSIGNIN,     //每日签到
};

enum E_SHOP_FOR_GIFT_TYPE
{
	E_SHOP_FOR_GIFT_TYPE_SUCCESS = 0,	//成功
	E_SHOP_FOR_GIFT_TYPE_NOT_ENOUGH_MONEY = 1,	//勋章不足
	E_SHOP_FOR_GIFT_TYPE_WRONG_UID = 2,	//错误的UID
	E_SHOP_FOR_GIFT_TYPE_OUT_COUNT = 3,	//超过发送限制
	E_SHOP_FOR_GIFT_TYPE_RFD_OUT_COUNT = 4,	//超过接收限制
	E_SHOP_FOR_GIFT_TYPE_SAME_PEOPLE = 5,	//不可发给自己
	E_SHOP_FOR_GIFT_TYPE_SAME_REGION = 6,	//不可发给同区
	E_SHOP_FOR_GIFT_TYPE_OUT_LEVEL = 7,	//不可发给同区
	E_SHOP_FOR_GIFT_TYPE_NOT_ENOUGH_ITEM = 8,	//物品不足
};

enum E_ARMYGROUP_ACTIVE_REL
{
	E_ARMYGROUP_ACTIVE_REL_OK = 0,		//成功
	E_ARMYGROUP_ACTIVE_REL_IDERROR,		//相关请求参数错误
	E_ARMYGROUP_ACTIVE_REL_TIMEERROR,	//时间不对
	E_ARMYGROUP_ACTIVE_REL_NPCIDERROR,	//NpcID 错误
	E_ARMYGROUP_ACTIVE_REL_STEPERROR,	//关卡错误
	E_ARMYGROUP_ACTIVE_REL_AWARDED,		//已领取过
	E_ARMYGROUP_ACTIVE_REL_TBLERROR,	//读表错误
	E_ARMYGROUP_ACTIVE_REL_COUNTRYERR,	//country server 数据错误
	E_ARMYGROUP_ACTIVE_REL_WINLVERR,	//击杀的npc等级错误
};

enum E_PVE_BATTEL_REL
{
	E_PVE_BATTEL_REL_OK = 0,		//成功
	E_PVE_BATTEL_REL_LOCKED,		//未解锁
	E_PVE_BATTEL_REL_WRONG_TIME,	//时间不对
	E_PVE_BATTEL_REL_NPCIDERROR,	//NpcID 错误
	E_PVE_BATTEL_REL_CDTIME,		//CD时间
};

/*----------------------------------------------------------------------------*/
/*									PVE										  */
/*----------------------------------------------------------------------------*/


const int LUCKNUMTIMESTOP = 300;
const int MAXLUCKYNUM = 10;
const int MAXLUCKYNUMPERIOD = 100;

enum STRATEGIC_GENERAL_SET
{
	STRATEGIC_GENERAL_SET_FIGHT_NUM = 5,	//一直是5路兵
	STRATEGIC_GENERAL_SET_FIGHT_TOTAL,		//总共6路兵
};

//兵所处类型
enum ARMY_CAMP_STATE
{
	E_ARMY_CAMP_STATE_COMMON = 0,			//野外
	E_ARMY_CAMP_STATE_TROOP,				//碉堡
	E_ARMY_CAMP_STATE_STRATEGIC,			//战略兵营
	E_ARMY_CAMP_STATE_STRATEGIC_GENERAL1,	//战略兵营--将军1
	E_ARMY_CAMP_STATE_STRATEGIC_GENERAL2,	//战略兵营--将军2
	E_ARMY_CAMP_STATE_STRATEGIC_GENERAL3,	//战略兵营--将军3
	E_ARMY_CAMP_STATE_STRATEGIC_GENERAL4,	//战略兵营--将军4
	E_ARMY_CAMP_STATE_STRATEGIC_GENERAL5,	//战略兵营--将军5
	E_ARMY_CAMP_STATE_STRATEGIC_GENERALMAX = E_ARMY_CAMP_STATE_STRATEGIC_GENERAL1 + 5,	//战略兵营--将军5
};

//战略兵营操作
enum STRATEGIC_ARMY_OPT_TYPE
{
	STRATEGIC_ARMY_OPT_NONE = 0,		//无效
	STRATEGIC_ARMY_OPT_ADDARMY,			//从基地加兵
	STRATEGIC_ARMY_OPT_CUTTOBASE,		//兵返回基地
	STRATEGIC_ARMY_OPT_TREAT,			//治疗兵种
	STRATEGIC_ARMY_OPT_TREAT_ONE,		//治疗兵员
	STRATEGIC_ARMY_OPT_DISMISS,			//遣散兵种
	STRATEGIC_ARMY_OPT_GENERALADDARMY,	//将领配兵
	STRATEGIC_ARMY_OPT_GENERALTREAT,	//治疗将领带领兵种
	STRATEGIC_ARMY_OPT_GENERALDISMISS,	//移除兵 
	STRATEGIC_ARMY_OPT_GENERALFIGHTPOS,	//更换将领位置
	STRATEGIC_ARMY_OPT_GENERALCHANGE,	//更换将领
};

//战略兵营操作错误返回
enum STRATEGIC_ARMY_OPT_REL_TYPE
{
	STRATEGIC_ARMY_OPT_REL_INIT = -1,
	STRATEGIC_ARMY_OPT_REL_OK = 0,			//OK
	STRATEGIC_ARMY_OPT_REL_ID,				//兵种ID错误
	STRATEGIC_ARMY_OPT_REL_BYTYPE,			//兵种类型错误
	STRATEGIC_ARMY_OPT_REL_CANOTINCAMP,		//兵种不能进兵营
	STRATEGIC_ARMY_OPT_REL_HPNOTFULL,		//兵种血量不足
	STRATEGIC_ARMY_OPT_REL_TYPENUMNOTFULL,	//兵种数量不足
	STRATEGIC_ARMY_OPT_REL_NOMORECREDIT,	//勋章不足
	STRATEGIC_ARMY_OPT_REL_NOMOREOIL,		//石油不足
	STRATEGIC_ARMY_OPT_REL_NOMOREMETRAL,	//金属不足
	STRATEGIC_ARMY_OPT_REL_NOMORESPACE,		//人口不足
	STRATEGIC_ARMY_OPT_REL_ARMYNUMLESS,		//要求返回基地的兵员数量少
	STRATEGIC_ARMY_OPT_REL_ARMYNUMMORE,		//要求返回基地的兵员数量多
	STRATEGIC_ARMY_OPT_REL_TREATCOSTALLNOTSAME,	//客户端计算的消耗总值与服务器端不一致
	STRATEGIC_ARMY_OPT_REL_COSTCREDIT,		//扣除勋章失败
	STRATEGIC_ARMY_OPT_REL_COSTOIL,			//扣除石油失败
	STRATEGIC_ARMY_OPT_REL_COSTMETRAL,		//扣除金属失败
	STRATEGIC_ARMY_OPT_REL_ARMYHPFULL,		//血量满，不需要加
	STRATEGIC_ARMY_OPT_REL_ARMYHASDISMISS,	//已经被遣散
	STRATEGIC_ARMY_OPT_REL_ARMYDISMISSORGE,	//战略兵营减少人口错误
	STRATEGIC_ARMY_OPT_REL_CAMPDISMISSORGE,	//基地减少人口错误
	STRATEGIC_ARMY_OPT_REL_GENERALIDERROR,	//将领ID错误
	STRATEGIC_ARMY_OPT_REL_GENERAL_FIGHTINDEX,//只能派遣1-5路的兵
	STRATEGIC_ARMY_OPT_REL_GENERAL_ARMYMAX,	//数量或人口已满
	STRATEGIC_ARMY_OPT_REL_GENERAL_SAMEONE,	//一样的将领
	STRATEGIC_ARMY_OPT_REL_INFIGHT,			//战斗中不可设置
	STRATEGIC_ARMY_OPT_REL_GENERALDATAERROR,//派兵数据错误
	STRATEGIC_ARMY_OPT_REL_GENERAL_INDEXERROR,//没有该路兵
	STRATEGIC_ARMY_OPT_REL_ARMYCUTORGE,		//减少战略兵营人口错误
	STRATEGIC_ARMY_OPT_REL_PASSWORD,		//二级密码未解锁
};

//PVE爬塔操作
enum STRATEGIC_PVEFIGHT_OPT_TYPE
{
	STRATEGIC_PVEFIGHT_OPT_NONE = 0,		//无效
	STRATEGIC_PVEFIGHT_OPT_FIGHT,			//战斗
	STRATEGIC_PVEFIGHT_OPT_REFRESH,			//刷新
	STRATEGIC_PVEFIGHT_OPT_WATCH,			//最后一次战斗
	STRATEGIC_PVEFIGHT_OPT_ADDREFRESH,		//增加刷新次数
	STRATEGIC_PVEFIGHT_OPT_INITDATA,		//初始化数据
	STRATEGIC_PVEFIGHT_OPT_FIGHTAUTO,		//自动战斗
	STRATEGIC_PVEFIGHT_OPT_VIPFIGHTAUTO,	//VIP自动战斗
};

//PVE爬塔操作错误返回
enum STRATEGIC_PVEFIGHT_OPT_REL_TYPE
{
	STRATEGIC_PVEFIGHT_OPT_REL_NONE = -1,
	STRATEGIC_PVEFIGHT_OPT_REL_OK = 0,			//OK
	STRATEGIC_PVEFIGHT_OPT_REL_WIN,				//胜利
	STRATEGIC_PVEFIGHT_OPT_REL_FAIL,			//失败
	STRATEGIC_PVEFIGHT_OPT_REL_WATCH,			//观战
	STRATEGIC_PVEFIGHT_OPT_REL_INIT,			//初始化错误
	STRATEGIC_PVEFIGHT_OPT_REL_NOHP,			//三次机会满
	STRATEGIC_PVEFIGHT_OPT_REL_NOREFRESH,		//没有刷新机会
	STRATEGIC_PVEFIGHT_OPT_REL_MORETIMES,		//购买数量超限制
	STRATEGIC_PVEFIGHT_OPT_REL_NOCREDIT,		//勋章不够
	STRATEGIC_PVEFIGHT_OPT_REL_COSTERROR,		//扣除勋章失败
	STRATEGIC_PVEFIGHT_OPT_REL_NOWFIGHT,		//正在战斗
	STRATEGIC_PVEFIGHT_OPT_REL_HASOVER,			//已经完成
	STRATEGIC_PVEFIGHT_OPT_REL_CUSTOMERROR,		//关卡错误
	STRATEGIC_PVEFIGHT_OPT_REL_OVERTIME,		//超时
	STRATEGIC_PVEFIGHT_OPT_REL_INCD,			//在关卡CD中
	STRATEGIC_PVEFIGHT_OPT_REL_NOTOPEN,			//PVE未开启
	STRATEGIC_PVEFIGHT_OPT_REL_ONLYREL,			//只需要知道结果
};

enum COST_GIFT_TYPE
{
	COST_GIFT_TYPE_DEFAULT,
	COST_GIFT_TYPE_DAILY,
	COST_GIFT_TYPE_DATE,
};


/*----------------------------------------------------------------------------*/
/*									竞技场									  */
/*----------------------------------------------------------------------------*/

//竞技场成就ID
enum ARENA_ACHIEVE_ID
{
	ARENA_ACHIEVE_ID_NONE = 0,					//无效ID

	ARENA_ACHIEVE_ID_FIGHTNUM10,				//挑战10次
	ARENA_ACHIEVE_ID_FIGHTNUM50,				//挑战50次
	ARENA_ACHIEVE_ID_FIGHTNUM100,				//挑战100次
	ARENA_ACHIEVE_ID_FIGHTNUM500,				//挑战500次
	ARENA_ACHIEVE_ID_FIGHTNUM1000,				//挑战1000次
	ARENA_ACHIEVE_ID_FIGHTNUM2000,				//挑战2000次
	ARENA_ACHIEVE_ID_FIGHTNUM5000,				//挑战5000次
	ARENA_ACHIEVE_ID_FIGHTNUM10000,				//挑战10000次

	ARENA_ACHIEVE_ID_WINTIMES2,					//累计胜利2次
	ARENA_ACHIEVE_ID_WINTIMES10,				//累计胜利10次
	ARENA_ACHIEVE_ID_WINTIMES50,				//累计胜利50次
	ARENA_ACHIEVE_ID_WINTIMES100,				//累计胜利100次
	ARENA_ACHIEVE_ID_WINTIMES200,				//累计胜利200次
	ARENA_ACHIEVE_ID_WINTIMES500,				//累计胜利500次
	ARENA_ACHIEVE_ID_WINTIMES1000,				//累计胜利1000次

	ARENA_ACHIEVE_ID_WINALWAYS5,				//连胜5次
	ARENA_ACHIEVE_ID_WINALWAYS10,				//连胜10次
	ARENA_ACHIEVE_ID_WINALWAYS15,				//连胜15次
	ARENA_ACHIEVE_ID_WINALWAYS20,				//连胜20次
	ARENA_ACHIEVE_ID_WINALWAYS30,				//连胜30次
	ARENA_ACHIEVE_ID_WINALWAYS50,				//连胜50次
	ARENA_ACHIEVE_ID_WINALWAYS100,				//连胜100次

	ARENA_ACHIEVE_TYPE_TORANK10000,				//到达名次10000
	ARENA_ACHIEVE_TYPE_TORANK5000,				//到达名次5000
	ARENA_ACHIEVE_TYPE_TORANK4000,				//到达名次4000
	ARENA_ACHIEVE_TYPE_TORANK3000,				//到达名次3000
	ARENA_ACHIEVE_TYPE_TORANK2000,				//到达名次2000
	ARENA_ACHIEVE_TYPE_TORANK1000,				//到达名次1000
	ARENA_ACHIEVE_TYPE_TORANK500,				//到达名次500
	ARENA_ACHIEVE_TYPE_TORANK400,				//到达名次400
	ARENA_ACHIEVE_TYPE_TORANK300,				//到达名次300
	ARENA_ACHIEVE_TYPE_TORANK200,				//到达名次200
	ARENA_ACHIEVE_TYPE_TORANK100,				//到达名次100
	ARENA_ACHIEVE_TYPE_TORANK75,				//到达名次75
	ARENA_ACHIEVE_TYPE_TORANK50,				//到达名次50
	ARENA_ACHIEVE_TYPE_TORANK30,				//到达名次30
	ARENA_ACHIEVE_TYPE_TORANK10,				//到达名次10
	ARENA_ACHIEVE_TYPE_TORANK5,					//到达名次5
	ARENA_ACHIEVE_TYPE_TORANK1,					//到达名次1

};

//竞技场成就类型
enum ARENA_ACHIEVE_TYPE
{
	ARENA_ACHIEVE_TYPE_NONE = 0,				//无成就
	ARENA_ACHIEVE_TYPE_FIGHTNUM,				//发起挑战次数
	ARENA_ACHIEVE_TYPE_WINTIMES,				//胜利次数累计
	ARENA_ACHIEVE_TYPE_WINALWAYS,				//连胜次数
	ARENA_ACHIEVE_TYPE_TORANK,					//到达名次
	ARENA_ACHIEVE_TYPE_MAX,						//最大值
};


//竞技场战斗记录 -- 属性
enum ARENA_FIGHTRECORD_TYPE
{
	ARENA_FIGHTRECORD_TYPE_NONE,				//无--初始化
	ARENA_FIGHTRECORD_TYPE_ATKSUCC,				//战斗成功
	ARENA_FIGHTRECORD_TYPEATKFAIL,				//战斗失败
	ARENA_FIGHTRECORD_TYPE_BYATKSUCC,			//被战斗成功
	ARENA_FIGHTRECORD_TYPE_BYATKFAIL,			//被战斗失败
};

//竞技场操作返回结果
enum ARENA_OPT_REL
{
	ARENA_OPT_REL_INIT = 0,						//初始化
	ARENA_OPT_REL_OK,							//成功
	ARENA_OPT_REL_ATKSELFWIN,					//主动挑战--胜利
	ARENA_OPT_REL_ATKSELFFAIL,					//主动挑战--失败
	ARENA_OPT_REL_ATKBYENEMYWIN,				//被挑战--胜利
	ARENA_OPT_REL_ATKBYENEMYFAIL,				//被挑战--失败
	ARENA_OPT_REL_ATKSETPOSERR,					//挑战设置位置失败
	ARENA_OPT_REL_ERRCONFIG,					//配置表相关错误
	ARENA_OPT_REL_ERRDATAINIT,					//数据初始化错误
	ARENA_OPT_REL_ERRBUYMAX,					//今天购买超过最大次数
	ARENA_OPT_REL_ERRBUYCREDIT,					//没有足够的勋章
	ARENA_OPT_REL_ERRPLAYERLVLS,				//玩家等级--自己
	ARENA_OPT_REL_ERRPLAYERLVLE,				//玩家等级--敌人
	ARENA_OPT_REL_ERRPVECUSTOMS,				//PVE关卡--自己
	ARENA_OPT_REL_ERRPVECUSTOME,				//PVE关卡--敌人
	ARENA_OPT_REL_ERRATKCDSELF,					//自己在CD中
	ARENA_OPT_REL_ERRATKCDENEMY,				//对方在CD中
	ARENA_OPT_REL_ERRATKRANKSELF,				//自己竞技场排名无
	ARENA_OPT_REL_ERRATKRANKENEMY,				//对方竞技场排名无
	ARENA_OPT_REL_ERRATKNOTIMES,				//没有战斗次数
	ARENA_OPT_REL_ERRRANKASKDIFFSELF,			//请求的位置信息错误--自己
	ARENA_OPT_REL_ERRRANKASKDIFFENEMY,			//请求的位置信息错误--敌人
	ARENA_OPT_REL_ERRACHIEVENOTDONE,			//成就没有完成
	ARENA_OPT_REL_ERRACHIEVEHASGAIN,			//成就已经被领取
	ARENA_OPT_REL_ERRASKTYPE,					//请求玩家数据之请求类型错误
	ARENA_OPT_REL_ERRHASGETRANKGIFT,			//今天已经领取排行奖励
	ARENA_OPT_REL_ERRRANKGIFTNONE,				//没有对应的礼物可领取
	ARENA_OPT_REL_ERRNOCOUNTRY,					//国家数据错误
	ARENA_OPT_REL_ERRNOCOUNTRYZONE,				//国家和分区不匹配
	ARENA_OPT_REL_ERRNOCOUNTRYID,				//国家ID错误
	ARENA_OPT_REL_ERRNOCOUNTRYSAMEONE,			//不能打自己
	ARENA_OPT_REL_ERRHASADDARENA,				//已经加入竞技场
	ARENA_OPT_REL_ERRCANNOTATKPOS,				//不能挑战该位置的玩家
	ARENA_OPT_REL_ERRPLAYERNOTINRECORD,			//玩家不在反击列表中
	ARENA_OPT_REL_ERRPLAYERBEHANDYOU,			//玩家名次在你之后
	ARENA_OPT_REL_ERRARENASTOPPED,				//竞技场关闭
	ARENA_OPT_REL_ERRNOSUCHACTIVE,				//没有对应的活动
	ARENA_OPT_REL_ERRACTIVEHASGAIN,				//对应的活动已经领取
	ARENA_OPT_REL_ERRNACTIVENOTOVER,			//对应活动没结束
	ARENA_OPT_REL_GAINSCOREGIFT_HASGAIN,		//已经领取过了
	ARENA_OPT_REL_GAINSCOREGIFT_NOTTOP3,		//不是前3
};

enum E_ARENA_MAMAGER
{
	E_COUNTRY_ARENA_CDTIME = 15,			//CD15秒
	E_COUNTRY_ARENA_MAX_RECORD = 20,		//最高20条记录
	E_COUNTRY_ARENA_RANK_NUM = 100,			//排行榜条数
	E_COUNTRY_MAX_ARENA_ROW_CNT = 1000,		//一行最大的uid数量

	E_FILL_ARENA_TYPE_INITDATA = 1,			//填充初始化数据
	E_FILL_ARENA_TYPE_FIGHTRECORD = 2,		//填充战斗数据
	E_FILL_ARENA_TYPE_ALLINFO = 3,			//所有数据
};

enum E_CE_ARENARANKINFO_TYPE
{
	E_FILL_ARENA_TYPE_ASKRANK = 1,			//请求排行榜
	E_FILL_ARENA_TYPE_ASKATK  = 2,			//请求战斗数据
	E_FILL_ARENA_TYPE_USERDATA = 3,			//玩家数据
};

enum E_WORLD_ARENA_MAMAGER
{
	E_WORLD_ARENA_CDTIME = 15,			//CD15秒
	E_WORLD_ARENA_MAX_RECORD = 20,		//最高20条记录
	E_WORLD_ARENA_RANK_NUM = 100,			//排行榜条数
	E_WORLD_MAX_ARENA_ROW_CNT = 1000,		//一行最大的uid数量
	E_WORLD_ARENA_LIMIT = 200,		        //需要的国家争霸战的名次

	E_FILL_WORLD_ARENA_TYPE_INITDATA = 1,			//填充初始化数据
	E_FILL_WORLD_ARENA_TYPE_FIGHTRECORD = 2,		//填充战斗数据
	E_FILL_WORLD_ARENA_TYPE_ALLINFO = 3,			//所有数据
};

enum E_REGION_ARENA_MAMAGER
{
	E_REGION_ARENA_CDTIME = 15,			//CD15秒
	E_REGION_ARENA_MAX_RECORD = 20,		//最高20条记录
	E_REGION_ARENA_RANK_NUM = 100,			//排行榜条数
	E_REGION_MAX_ARENA_ROW_CNT = 1000,		//一行最大的uid数量
	E_REGION_ARENA_LIMIT = 200,		        //需要的国家争霸战的名次
	E_REGION_ARENA_ENEMY_MAX_RECORD = 100,		//最高20条记录

	E_FILL_REGION_ARENA_TYPE_INITDATA = 1,			//填充初始化数据
	E_FILL_REGION_ARENA_TYPE_FIGHTRECORD = 2,		//填充战斗数据
	E_FILL_REGION_ARENA_TYPE_ALLINFO = 3,			//所有数据
	E_FILL_REGION_ARENA_TYPE_APPLYINFO = 4,			//报名数据
};


enum E_HERO_ARENA_MAMAGER
{
	E_HERO_ARENA_CDTIME = 15,			//CD15秒
	E_HERO_ARENA_MAX_RECORD = 20,		//最高20条记录
	E_HERO_ARENA_RANK_NUM = 100,			//排行榜条数
	E_HERO_MAX_ARENA_ROW_CNT = 1000,		//一行最大的uid数量
	E_HERO_ARENA_LIMIT = 200,		        //需要的国家争霸战的名次

	E_FILL_HERO_ARENA_TYPE_INITDATA = 1,			//填充初始化数据
	E_FILL_HERO_ARENA_TYPE_FIGHTRECORD = 2,		//填充战斗数据
	E_FILL_HERO_ARENA_TYPE_ALLINFO = 3,			//所有数据
};

//英雄竞技场操作
enum E_HERO_ARENA_OPT_TYPE
{
	E_HERO_ARENA_OPT_FIGHT = 1,		//战斗
	E_HERO_ARENA_OPT_BUY,			//购买战斗次数
	E_HERO_ARENA_OPT_GAINRANK,		//取得排行榜奖励
	E_HERO_ARENA_OPT_ADDARENA,		//玩家加入竞技场
	E_HERO_ARENA_OPT_FIGHTBACK,		//反击
	E_HERO_ARENA_OPT_HEROFIGHT,		//部署英雄
};

enum E_HERO_ARENARANKINFO_TYPE
{
	E_HERO_ARENARANKINFO_ASKRANK  = 1,			//请求排行榜
	E_HERO_ARENARANKINFO_ASKATK   = 2,			//请求战斗数据
	E_HERO_ARENARANKINFO_USERDATA = 3,			//玩家数据
};
enum E_REGION_ARENARANKINFO_TYPE
{
	E_REGION_ARENARANKINFO_ASKRANK  = 1,			//请求排行榜
	E_REGION_ARENARANKINFO_ASKATK   = 2,			//请求战斗数据
	E_REGION_ARENARANKINFO_USERDATA = 3,			//玩家数据
	E_REGION_ARENARANKINFO_ALLRANK  = 4,			//每期排行榜
	E_REGION_ARENARANKINFO_LASTRANK = 5,			//上一期排行版
};

//跨区竞技场操作
enum E_REGION_ARENA_OPT_TYPE
{
	E_REGION_ARENA_OPT_FIGHT = 1,		//战斗
	E_REGION_ARENA_OPT_BUY,			//购买战斗次数
	E_REGION_ARENA_OPT_GAINRANK,		//取得排行榜奖励
	E_REGION_ARENA_OPT_ADDARENA,		//玩家加入竞技场
	E_REGION_ARENA_OPT_FIGHTBACK,		//反击
	E_REGION_ARENA_OPT_ADDARENA_FROMWEB,		//玩家加入竞技场,web返回
	E_REGION_ARENA_OPT_ADDARENA_ADMIN,		//玩家加入竞技场,管理员
	E_REGION_ARENA_OPT_ADDARENA_SUPER,		//玩家加入竞技场,国家英雄
};

enum E_WORLD_ARENARANKINFO_TYPE
{
	E_WORLD_ARENARANKINFO_ASKRANK  = 1,			//请求排行榜
	E_WORLD_ARENARANKINFO_ASKATK   = 2,			//请求战斗数据
	E_WORLD_ARENARANKINFO_USERDATA = 3,			//玩家数据
	E_WORLD_ARENARANKINFO_ALLRANK  = 4,			//每期排行榜
	E_WORLD_ARENARANKINFO_LASTRANK = 5,			//上一期排行版
};

enum E_ARENA_OPT_TYPE
{
	E_ARENA_OPT_FIGHT = 1,		//战斗
	E_ARENA_OPT_BUY,			//购买战斗次数
	E_ARENA_OPT_GAINRANK,		//取得排行榜奖励
	E_ARENA_OPT_GAINACHIEVE,	//取得成就奖励
	E_ARENA_OPT_ADDARENA,		//玩家加入竞技场
	E_ARENA_OPT_FIGHTBACK,		//反击
	E_ARENA_OPT_GAINACTIVE,		//获取活动奖励
	E_ARENA_OPT_UPDATASCORE,	//同步积分
	E_ARENA_OPT_GAINSCORE,		//领取积分
};
//世界竞技场战斗记录 -- 属性
enum E_WORLD_ARENA_FIGHTRECORD_TYPE
{
	E_WORLD_ARENA_FIGHTRECORD_TYPE_NONE,				//无--初始化
	E_WORLD_ARENA_FIGHTRECORD_TYPE_ATKSUCC,				//战斗成功
	E_WORLD_ARENA_FIGHTRECORD_TYPEATKFAIL,				//战斗失败
	E_WORLD_ARENA_FIGHTRECORD_TYPE_BYATKSUCC,			//被战斗成功
	E_WORLD_ARENA_FIGHTRECORD_TYPE_BYATKFAIL,			//被战斗失败
};
//世界竞技场操作
enum E_WORLD_ARENA_OPT_TYPE
{
	E_WORLD_ARENA_OPT_FIGHT = 1,		//战斗
	E_WORLD_ARENA_OPT_BUY,			//购买战斗次数
	E_WORLD_ARENA_OPT_GAINRANK,		//取得排行榜奖励
	E_WORLD_ARENA_OPT_ADDARENA,		//玩家加入竞技场
	E_WORLD_ARENA_OPT_FIGHTBACK,		//反击
};

//世界竞技场操作返回结果
enum E_WORLD_ARENA_OPT_REL
{
	E_WORLD_ARENA_OPT_REL_INIT = 0,						//初始化
	E_WORLD_ARENA_OPT_REL_OK,							//成功
	E_WORLD_ARENA_OPT_REL_ATKSELFWIN,					//主动挑战--胜利
	E_WORLD_ARENA_OPT_REL_ATKSELFFAIL,					//主动挑战--失败
	E_WORLD_ARENA_OPT_REL_ATKBYENEMYWIN,				//被挑战--胜利
	E_WORLD_ARENA_OPT_REL_ATKBYENEMYFAIL,				//被挑战--失败
	E_WORLD_ARENA_OPT_REL_ATKSETPOSERR,					//挑战设置位置失败
	E_WORLD_ARENA_OPT_REL_ERRCONFIG,					//配置表相关错误
	E_WORLD_ARENA_OPT_REL_ERRDATAINIT,					//数据初始化错误
	E_WORLD_ARENA_OPT_REL_ERRBUYMAX,					//今天购买超过最大次数
	E_WORLD_ARENA_OPT_REL_ERRBUYCREDIT,					//没有足够的勋章
	E_WORLD_ARENA_OPT_REL_ERRPLAYERLVLS,				//玩家等级--自己 废弃
	E_WORLD_ARENA_OPT_REL_ERRPLAYERLVLE,				//玩家等级--敌人 废弃
	E_WORLD_ARENA_OPT_REL_ERRPVECUSTOMS,				//PVE关卡--自己 废弃
	E_WORLD_ARENA_OPT_REL_ERRPVECUSTOME,				//PVE关卡--敌人 废弃
	E_WORLD_ARENA_OPT_REL_ERRATKCDSELF,					//自己在CD中
	E_WORLD_ARENA_OPT_REL_ERRATKCDENEMY,				//对方在CD中
	E_WORLD_ARENA_OPT_REL_ERRATKRANKSELF,				//自己竞技场排名无
	E_WORLD_ARENA_OPT_REL_ERRATKRANKENEMY,				//对方竞技场排名无
	E_WORLD_ARENA_OPT_REL_ERRATKNOTIMES,				//没有战斗次数
	E_WORLD_ARENA_OPT_REL_ERRRANKASKDIFFSELF,			//请求的位置信息错误--自己
	E_WORLD_ARENA_OPT_REL_ERRRANKASKDIFFENEMY,			//请求的位置信息错误--敌人
	E_WORLD_ARENA_OPT_REL_ERRACHIEVENOTDONE,			//成就没有完成 废弃
	E_WORLD_ARENA_OPT_REL_ERRACHIEVEHASGAIN,			//成就已经被领取 废弃
	E_WORLD_ARENA_OPT_REL_ERRASKTYPE,					//请求玩家数据之请求类型错误 废弃
	E_WORLD_ARENA_OPT_REL_ERRHASGETRANKGIFT,			//今天已经领取排行奖励
	E_WORLD_ARENA_OPT_REL_ERRRANKGIFTNONE,				//没有对应的礼物可领取 废弃
	E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRY,					//国家数据错误 废弃
	E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYZONE,				//国家和分区不匹配
	E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYID,				//国家ID错误
	E_WORLD_ARENA_OPT_REL_ERRNOCOUNTRYSAMEONE,			//不能打自己
	E_WORLD_ARENA_OPT_REL_ERRHASADDARENA,				//已经加入竞技场
	E_WORLD_ARENA_OPT_REL_ERRCANNOTATKPOS,				//不能挑战该位置的玩家
	E_WORLD_ARENA_OPT_REL_ERRPLAYERNOTINRECORD,			//玩家不在反击列表中
	E_WORLD_ARENA_OPT_REL_ERRPLAYERBEHANDYOU,			//玩家名次在你之后
	E_WORLD_ARENA_OPT_REL_ERRARENASTOPPED,				//竞技场关闭
	E_WORLD_ARENA_OPT_REL_ERRNOSUCHACTIVE,				//没有对应的活动 废弃
	E_WORLD_ARENA_OPT_REL_ERRACTIVEHASGAIN,				//对应的活动已经领取 废弃
	E_WORLD_ARENA_OPT_REL_ERRNACTIVENOTOVER,			//对应活动没结束 废弃
	E_WORLD_ARENA_OPT_REL_GAINSCOREGIFT_HASGAIN,		//已经领取过了  废弃
	E_WORLD_ARENA_OPT_REL_GAINSCOREGIFT_NOTTOP3,		//不是前3 废弃
	E_WORLD_ARENA_OPT_REL_NOT_TOP200,		             //国家竞技场不是前200
	E_WORLD_ARENA_OPT_REL_NOT_APPLY_TIME,		             //没到报名时间
	E_WORLD_ARENA_OPT_REL_NOT_NOT_REGION,		             //不对该区开放
	E_WORLD_ARENA_OPT_REL_NOT_ARENABEGEIN,		             //竞技赛已经开始不能报名
	E_WORLD_ARENA_OPT_REL_CLOSE,		             //竞技场关闭
	E_WORLD_ARENA_OPT_REL_NOT_TOP10,		             //战功排行不是前10
	E_WORLD_ARENA_OPT_REL_NOT_3Times,		             //没有发起三次挑战
	E_WORLD_ARENA_OPT_REL_NOHERO,		             //没有英雄
	E_WORLD_ARENA_OPT_REL_NOENOUGHSCORE,				//没有足够积分
	E_WORLD_ARENA_OPT_REL_NOGROUNDHERO,				//没有地面英雄
};

const int MAX_FRD_LV_GIFT_RECORD = 20;
//装备
enum E_Equip_BagType
{
	Equip_Bag = 0,		//背包中，未使用
	Equip_Admiral,	   //将领
	Equip_TypeMax,
};
//装备道具类型
enum E_Equip_Item_Type
{
	Equip_Item_Strength=1,
	Equip_Item_Lucky,
	Equip_Item_Protect,
};

//装备类型
enum E_Equip_Type
{
	Equip_Type_Hat = 1,		    //帽子
	Equip_Type_Clothe,	        //衣服
	Equip_Type_Shoes,           //鞋子        
	Equip_Type_Arm,             //武器
	Equip_Type_Ride,            //坐骑
	Equip_Type_Dorn,            //饰品
	Equip_Type_Cnt,
};

//物品掉落类型
enum E_ItemDrop_Type
{
	ItemDrop_Type_Country = 1,		    //国战

	ItemDrop_Cnt,
};

enum E_Server_Stat
{
	ESS_Battle_Begin = 0,
	ESS_Battle_Friend = 0,
	ESS_Battle,
	ESS_Battle_Enemy,
	ESS_Battle_Defence,
	ESS_PVE,
	ESS_COUNTRY_AT,
	ESS_COUNTRY_DE,
	ESS_COUNTRY_PVPAT,
	ESS_COUNTRY_PVPDE,
	ESS_DEFENCE_WAVES,
	ESS_JIANMIE,
	ESS_WORLD_PVP,
	ESS_ZHANLING = 12,
	ESS_Battle_End = 12,
	ESS_Res_Begin = 13,
	ESS_METAL = 13,
	ESS_OIL,
	ESS_HONOR,
	ESS_FEAT,
	ESS_PVP_POINT,
	ESS_ZHANGONG,
	ESS_GONGHUI_POINT,
	ESS_TANSUO_MID,
	ESS_TANSUO_LARGE,
	ESS_EXP_SMALL,
	ESS_EXP_MID,
	ESS_EXP_LARGE,
	ESS_ZHAOMU_SMALL,
	ESS_ZHAOMU_MID,
	ESS_ZHAOMU_LARGE,
	ESS_JUNGONG_FOOT,
	ESS_JUNGONG_TANK,
	ESS_JUNGONG_PLANE,
	ESS_KEY_DOWN,
	ESS_KEY_MID,
	ESS_KEY_UP,
	ESS_SP_ARMY = 34,
	ESS_Res_End = 34,
};

enum E_Password_Type
{
	Password_Type_Locked=0,
	Password_Type_Unlocked,
	Password_Type_Freeze,
};

//官员职位
enum E_Officer_Position
{
	Officer_NULL = 0,          //无官职
	Officer_Chairman ,         //大总统
	Officer_President ,        //总统
	Officer_DefenseMinister,   //国防部长
	Officer_Premier ,          //总理
	Officer_StaffChief,        //参谋长
	Officer_VicePresident ,    //副总统
	Officer_LandLeader,        //陆军司令 
	Officer_AirLeader,         //空军司令
	Officer_ForeignMinister,   //外交部长 
	Officer_SecurityMinister,  //安全部长 
	Officer_PresidentGuard,    //总统卫队 
	Officer_MAX,
};

//官员类型
enum E_Officer_Type
{
	E_Officer_Type_NULL,
	E_Officer_Type_Arena,
	E_Officer_Type_Guild,
	Officer_Type_MAX,

};

//官员权限
enum E_Officer_Right
{
	E_Officer_Right_NULL,
	E_Officer_Right_AddHP,    //增加血量
	E_Officer_Right_AddDamage, //增加伤害
	E_Officer_Right_AddMorale, //增加士气
	E_Officer_Right_MAX,

};

enum E_OfficerOptType
{
	E_Officer_UseRight=1,
};

enum E_OfficerOptRet
{
	E_Officer_Ret_Sucess =0,    //成功
	E_Officer_Ret_NoPosition,    //没有职位
	E_Officer_Ret_NoRight,       //没有权限
	E_Officer_Ret_HasUsed,        //已经使用
	E_Officer_Ret_Fail,           //失败
	E_Officer_Ret_ConfigErr,    //初始化失败
	E_Officer_Ret_IsCapital,    //不能再首都使用

	E_Officer_Ret_other,      //其他
};

enum E_Officer_Fill_Type
{
	E_Officer_Fill_Right =1,    //权限
	E_Officer_Fill_Officer,     //官员
	E_Officer_Fill_ALL,         //所有
};


enum E_Buff_Fill_Type
{
	E_Buff_Fill_All =1,    //所有
	E_Buff_Fill_CountryBattle,     //国战
};

const int UI_FLAG_BOX = 1013;		//魔盒
const int UI_FLAG_COSTCREDIT = 1015;//充值
const int UI_FLAG_EGG = 1016;		//砸蛋
const int UI_FLAG_CHICKEN = 1017;	//火鸡
const int UI_FLAG_FIRST_COST = 1018;//首充
const int UI_FLAG_ADMIRAL = 1019;	//神将招募
const int UI_FLAG_YELLOW = 1020;	//黄钻活动
const int UI_FLAG_WHEEL = 1025;		//幸运轮盘
const int UI_FLAG_STAR = 1026;		//星星达人
const int UI_FLAG_QIEGAO = 1028;	//切糕/战场补给包
const int UI_FLAG_JIANGXING = 1030;	//将星招募
const int UI_FLAG_BACKBOX = 1033;	//回归宝箱
const int UI_FLAG_INVITE = 1035;	//邀请好友（5折卡）
const int UI_FLAG_XUNBAO = 1039;	//寻宝活动
const int UI_FLAG_LUCKCOUPON = 1042;//幸运券
const int UI_FLAG_FUZHU = 1043;		//将领辅助
const int UI_FLAG_COUNTRY = 1046;	//献礼国战
const int UI_FLAG_GUID = 1047;		//公会崛起
const int UI_FLAG_YELLOWHAOLI = 1048;//黄钻送豪礼
const int UI_FLAG_SEVENTARGET = 1054;//七日目标
const int UI_FLAG_REGIONARENA = 1066;//跨区争霸战


enum E_VipCard_Type
{
	VipCard_Type_NULL ,    //非VIP
	VipCard_Type_Month,    //月卡
	VipCard_Type_Year,      //年卡
};

enum E_VipOptType
{
	E_Vip_BUYCARD=1,
};

enum E_VipOptRet
{
	E_Vip_Ret_Success,//成功
	E_Vip_Ret_CardUnavailable,//vip卡无效
	E_Vip_Ret_CreditNotEnough,//勋章不够
	E_Vip_Ret_NotYellow,//不是黄砖
	E_Vip_Ret_ConfigErr,//配置错误
	E_Vip_Ret_VipLimit,//已到上限
};

const int SEVEN_DAYS_CNT = 7; //七日目标天数  1：完成全部任务 2：完成任务 3：升级建筑 4：基地等级 5：训练单位 6：收集资源 7：每日任务 8：进攻 9：发出邀请 10：解锁兵种 11：提升科技 12:建筑达到等级

enum E_SEVENDAYS_TARGET
{
	E_ST_ALL = 1,
	E_ST_QUEST,
	E_ST_UP_BUILDING,
	E_ST_BASE_LEVEL,	
	E_ST_CREATE_ARMY,
	E_ST_RES,
	E_ST_DAILY_TASK,
	E_ST_ATTACK,
	E_ST_INVITE,
	E_ST_UNLOCK,
	E_ST_SCIENCE,
	E_ST_BUILDING_LV,
};

enum E_ZHAOMU_TYPE
{
	E_ZHAOMU_TYPE_SUCCESS = 0,	//成功
	E_ZHAOMU_TYPE_WRONG_UID,	//错误的UID
	E_ZHAOMU_TYPE_WRONG_TIME,	//玩家类型错误
	E_ZHAOMU_TYPE_RFD_OUT_COUNT,	//超过接收限制
	E_ZHAOMU_TYPE_SAME_PEOPLE,	//不可发给自己
	E_ZHAOMU_TYPE_SAME_REGION,	//不可发给不同区
};

enum E_FILL_PROPMPT_TYPE
{
	E_MENU_MAIN,
	E_MENU_COUNTRY,
	E_MENU_GUILD,
	E_MENU_ADMIRAL,
	E_MENU_EQUIP,
	E_MENU_BUILDING,
	E_MENU_WEAPON,
	E_MENU_PVE,
	E_MENU_STAR,
	E_MENU_HERO,
};

enum E_PLAYER_RECORD
{
	E_PLAYER_RECORD_TIMELIMIT = 1,
	E_PLAYER_RECORD_SHOPFORGIFT = 4,
};

enum E_QQ_TASK
{
	E_QQ_TASK_SUCC = 0,
	E_QQ_TASK_NO_USER = 1,
	E_QQ_TASK_NOT_COMPLETE = 2,
	E_QQ_TASK_HAS_GET = 3,
	E_QQ_TASK_UNKNOW = 200,
};


enum E_OTHER_GAME_RET
{
	E_RET_OTHERGAME_SUCCESS = 0,   //成功
	E_RET_OTHERGAME_GIFT_HAS_GAIN ,//已经领取
	E_RET_OTHERGAME_GIFT_NOT_EXIST,  //物品不存在
};

enum E_OTHER_GAME_OPT
{
	E_OPT_OTHERGAME_INFO = 0,      //信息
	E_OPT_OTHERGAME_CREATE ,//创建游戏
	E_OPT_OTHERGAME_GIFT,  //领取礼物
	E_OPT_OTHERGAME_OPEN,  //打开面板
};

enum CrossPromotionCDKFlag
{
	CPF_Init = 0,	//未达成
	CPF_Finish,		//达成
	CPF_Pick,		//已领取
};

enum E_BOOKCOLLE_TYPE
{
	E_BOOKCOLLE_FREE = 1,      //免费
	E_BOOKCOLLE_HALF ,         //半价
	E_BOOKCOLLE_WHOLE,          //全价
};

////////////////////////////////////////////////////////////////
//全区服务器定义

enum E_REGION_SEVER_KEY
{
	KEY_WHOLEREGION = 10000,
	KEY_WHOLECOUNTRY = 20000,
	KEY_WHOLEREGION_USERLIST = 10000,
};

enum RegionErrorLogType
{//错误日志类型
	RegionError_LoadUserError,			//加载玩家出错
};

enum E_PVE_BATTLE_CHANGJING_TYPE
{
	E_PVEBATTLE_XIULI = 1,      //1、修理大桥
	E_PVEBATTLE_SAOLEI,			//2、雷区扫雷
	E_PVEBATTLE_HONGZHA,        //3、轰炸机场
	E_PVEBATTLE_SOUSUO,			//4、搜索仓库
};

enum E_PVE_BATTLE_TYPE
{
	E_PVEBATTLE_TYPE_REQ,
	E_PVEBATTLE_TYPE_ACTIVE,   
	E_PVEBATTLE_TYPE_CREDIT_ACTIVE,	
	E_PVEBATTLE_TYPE_SAODANG,
	E_PVEBATTLE_TYPE_CANCEL,       
	E_PVEBATTLE_TYPE_PVE_BATTEL,	
	E_PVEBATTLE_TYPE_PVE_BATTEL_END,
	E_PVEBATTLE_TYPE_PVE_BUY,
};

const int MAX_PVE_BATTLE_VALUE = 3;//七日目标
const int MAX_JUNLING_NUM = 10;
const int BATTLE_SAODANG_TIMES = 1;
const int BATTLE_JUNLING_TIME = 14400;

enum E_ADMIRAL_EXCHANGE_RESULT
{
    E_ADMIRAL_EXCHANGE_SUCESS = 0,
    E_ADMIRAL_EXCHANGE_OUTTIMES,            //超出兑换上限
    E_ADMIRAL_EXCHANGE_WRONGTIME,           //不在活动时间
    E_ADMIRAL_EXCHANGE_LACKSOUL,            //将魂不足
	E_ADMIRAL_EXCHANGE_LACKDAILYCOST,		//当日充值勋章不足
};

enum E_CARDDUEL_OPTTYPE
{
    E_CARDDUEL_OPT_OPEN = 0,                        ///打开
    E_CARDDUEL_OPT_MATCH,                           ///匹配
    E_CARDDUEL_OPT_SELECT,                          ///选择卡片
    E_CARDDUEL_OPT_DUELRESULT,                      ///决斗结果
};

enum E_CARDDUEL_CARDTYPE
{
    E_CARDDUEL_TYPE_UNKNOW = 0,
    E_CARDDUEL_TYPE_KIROV = 1,              ///基洛夫
    E_CARDDUEL_TYPE_APOCALYPSE,             ///天启坦克
    E_CARDDUEL_TYPE_CPLASF,                 ///中国特种兵
};

enum E_CARDDUEL_RESULT
{
    E_CARDDUEL_Win,                         ///胜利
    E_CARDDUEL_Fail,                        ///失败
    E_CARDDUEL_Deuce,                       ///平局
};

enum E_CARDDUEL_MATCHSTATE
{
    E_CARDDUEL_NOMATCH,                 //未匹配
    E_CARDDUEL_ONMATCHING,              //正在匹配
    E_CARDDUEL_MATCHSUC,                //匹配成功
};

enum HeroUseType
{
	HeroUse_NULL, //未使用
	HeroUse_Atk,  //攻击阵营
	HeroUse_Def,  //防守阵营
	HeroUse_Auto, //集团兵营阵营
	HeroUse_Arena, //竞技场
	HeroUse_Max,
};

enum HeroOptRet
{
	HO_Ret_Success = 0,	    //成功
	HO_Ret_HeroNotExist,    //未找到英雄
	HO_Ret_SameHeroInUse,    //英雄处于出战状态
	HO_Ret_InitErro,      //初始错误
	HO_Ret_MaxLevel,      //最大等级
	HO_Ret_NoEnoughCredit, //勋章不够
	HO_Ret_NoEnoughRes,   //资源不够
	HO_Ret_NoHeroCenterLevel, //英雄中心等级不够
	HO_Ret_HeroUseFull,  //英雄已经满
	HO_Ret_HeroIndexError,  //位置不对
	HO_Ret_HeroUseTypeError,  //阵营不对
	H0_Ret_CreditNotEnough, //勋章不够
	HO_Ret_HeroInTrian,    //英雄正在训练
	HO_Ret_GradeError,    //星级不对
	HO_Ret_HeroLetterNotExist,    //信物不存在
	HO_Ret_HeroInUse,    //英雄处于出战状态
};

enum HeroListType
{
	HeroList_0 = 0,	     //默认
	HeroList_1,           //血量变化的英雄，用于客户端计算血量恢复

};

enum E_QSHOP_ITEM_ID
{//Q点商城购买物品ID
    E_QSHOP_ITEM_WC_ONTEAM =  1035,             //世界杯冠军竞猜投注一个队伍
    E_QSHOP_ITEM_MIDDLEYEARGIFT = 1036,         //年中好礼
    E_QSHOP_ITEM_WC_SECTEAM = 1037,             //世界杯冠军竞猜投注两个队伍
    E_QSHOP_ITEM_WC_THRTEAM = 1038,             //世界杯冠军竞猜投注三个队伍
    E_QSHOP_ITEM_LEVELLUXURYGIFT = 1039,        //等级豪礼
    E_QSHOP_ITEM_MONTHCARD = 1040,              //红警月卡
    E_QSHOP_ITEM_VALENTINESGIFT_FREE = 1054,    //情人节礼物 免费
    E_QSHOP_ITEM_VALENTINESGIFT_SMALL = 1055,   //情人节礼物 99
    E_QSHOP_ITEM_VALENTINESGIFT_BIG = 1056,     //情人节礼物 999
    E_QSHOP_ITEM_VIP_MONTHCARD = 1058,          //VIP月卡

    E_QSHOP_ITEM_RENT_EIGHT_24 = 1060,          //租用8级配件24小时
    E_QSHOP_ITEM_RENT_EIGHT_72 = 1061,          //租用8级配件72小时
    E_QSHOP_ITEM_RENT_EIGHT_168 = 1062,          //租用8级配件168小时
    E_QSHOP_ITEM_RENT_NINE_24 = 1063,          //租用9级配件24小时
    E_QSHOP_ITEM_RENT_NINE_72 = 1064,          //租用9级配件72小时
    E_QSHOP_ITEM_RENT_NINE_168 = 1065,          //租用9级配件168小时

    E_QSHOP_ITEM_COUNTRYBOMB = 1066,            //国战炸弹
    E_QSHOP_ITEM_DAILYRECHARGELOTTERY = 1067,   //每日充值抽奖
	E_QSHOP_ITEM_SECARMSDEALER = 1068,			//神秘军火商

    E_QSHOP_ITEM_GUILDREDENVELOPE = 1076,            //公会红包

    E_QSHOP_ITEM_VIP_DAILYGIFT = 2003,          //每日VIP购买礼包
    E_QSHOP_ITEM_VIP_DAILYFREEGIFT = 2004,      //每日VIP免费礼包

	E_QSHOP_ITEM_VIP_BUY_DAILY_GIFT = 2005,		//vip每日限购礼包
	E_QSHOP_ITEM_ALL_BUY_DAILY_GIFT = 2006,		//全民每日限购礼包
	E_QSHOP_ITEM_ASSIST_GIFT		= 1135,		//助战礼包
	E_QSHOP_ITEM_DUANWU_VIP			= 2007,		//端午vip活动
	E_QSHOP_ITEM_NEW_RECALL			= 1136,		//英雄回归

};


enum WpcGroupAddType
{
	WpcGroupAddTypeHP = 1,
	WpcGroupAddTypeAtk = 3,
};

enum E_PropertyGroup
{
	PG_NULL=0,
	PG_Army,
	PG_Tank,
	PG_Plane,
	PG_Hero_erth,
	PG_Hero_air,
	PG_TypeMax,
};

enum E_PropertyAddType
{
	PT_Unknow = 0,
	PT_AddHP,			    //血量
	PT_AddAtk,			    //攻击力
	PT_AddDamageDef,	    //伤害防御
	PT_AddSpeed,		    //速度
	PT_AddCrt,			    //暴击
	PT_AddCrtDef,		    //暴击防御
	PT_AddToFoot,		    //对步兵克制
	PT_AddToPlane,		    //对飞机克制
	PT_AddToTank,		    //对坦克克制
	PT_DefToFoot,		    //对步兵防御
	PT_AddToHero,		    //对英雄克制
	PT_DefToPlane,		    //对飞机防御
	PT_DefToTank,		    //对坦克防御
	PT_DefToHero,		    //对英雄防御
	PT_PropertyAddCnt,
};

enum FightArenaType
{
	Fight_NULL = 0,
	Fight_PVE   = 1,  //PVE
	Fight_Country,    //国战
	Fight_Arena,	  //国家竞技场
	Fight_WorldArena, //世界竞技场	
	Fight_Guild,	  //公会战
	Fight_RegionArena, //世界竞技场
	Fight_Battle,     //战役
	Fight_RegionCountry,    //跨服国战
	Fight_Gve,    //gve
	Fight_HeroArena, //英雄竞技场
	Fight_Resource,
	Fight_Gvg,    //gve
	Fight_BossBtl,    //Boss战
	Fight_Max,
};

enum FightType
{
    Fight_PVE_User = 0,
    Fight_PVE_NPC,
    Fight_PVP_Atk,	//攻击方
    Fight_PVP_Def,	//防御方
    Fight_PVP_NPC,	//国战中攻击NPC
};

enum CountryOptResult
{//国家操作返回值
    E_COTR_NoBomb = 45,         //缺少炸弹
    E_CORT_TPNotAllow,          //不允许传送
    E_CORT_NoTPScroll,          //没有传送卷轴
    E_CORT_OtherCountry,        //国家不对 不可以传送
};

enum WPCCOMExtraType
{
    WET_NORMAL = 0,
    WET_EXTRA  = 2, //额外配件
};

enum WPCOptResultType
{
    WT_OR_EXTRA_NOALLOW = 19,       //额外配件不能装配该配件
};

const int GVG_FIGHT_DEL_TIME = 3600;



//排行榜服务器相关
const int RANK_ZHANGONG_USERCNT = 100;
const int RANK_ZHANGONG_MINSCORE = 2000;

enum E_WARCOLLEGE_STRATEGY_TYPE
{
	WC_STRATEGY_ATTACK = 1,
	WC_STRATEGY_BOUND,//暴击
	WC_STRATEGY_LIFE,
	WC_STRATEGY_SPEED,
	WC_STRATEGY_TANK,
	WC_STRATEGY_PROTECT,//减伤
	WC_STRATEGY_MAX
};

enum E_WARCOLLEGE_SKILL_TYPE
{
	WC_SKILL_SHIELD = 1,
	WC_SKILL_HURT,
	WC_SKILL_BATTER,//连击
	WC_SKILL_MAX
};

enum WPCEmblemExtraType
{
	WEMT_NORMAL = 0,
	WEMT_EXTRA  = 2, //额外配件
};

const int SecArmsDealer_Award_Max = 4;//神秘军火商奖励索引最大值

enum E_Super_Storm_Result
{
    //0：成功 1：没有超级强攻令 2：不同区 3：无效的UID 4：等级保护 5：正在被攻击 6：强攻令使用CD 7：被攻击的太频繁 8：不是仇敌 9：正在被强攻 10:拒绝过期
    E_SSR_Succ = 0,
    E_SSR_NoToken,              //没有超级强攻令
    E_SSR_DiffRegion,           //不同区
    E_SSR_NullityUid,           //无效的UID
    E_SSR_ProtectLv,            //等级保护
    E_SSR_BeAttcked,            //正在被攻击
    E_SSR_StormOnCD,            //强攻令使用CD
    E_SSR_BeStormTooOfen,       //被攻击的太频繁
    E_SSR_NoEnemy,              //不是仇敌
    E_SSR_OnStorm,              //正在被强攻
    E_SSR_RefuseOutTime,        //拒绝过期
    E_SSR_DestroyProtect,       //摧毁保护
    E_SSR_OnSundayDefendActv,   //正在过周日防守
    E_SSR_AtkCntOverMax,	    //当日攻击一个玩家超过3次
};

enum E_BOSSBTLSTATE
{
	E_BOSSBTLSTATE_UNKNOWN,
	E_BOSSBTLSTATE_OVER_DAWN,
	E_BOSSBTLSTATE_PREPARE,//未开始
	E_BOSSBTLSTATE_BATTLE, //开始
	E_BOSSBTLSTATE_END,	   //结束，结算中
	E_BOSSBTLSTATE_OVER,   //结算完
    E_BOSSBTLSTATE_UPOVER,  //点赞清算
};

enum E_BOSSBTL_BOSSID
{
	E_BOSSBTL_BOSSID_START = 50000,
	E_BOSSBTL_BOSSID_SAT = E_BOSSBTL_BOSSID_START + 5,
	E_BOSSBTL_BOSSID_SUN = E_BOSSBTL_BOSSID_START + 6,
	E_BOSSBTL_BOSSID_END = E_BOSSBTL_BOSSID_SUN,
};

const int BOSSBTL_RANK_CNT_BASE = 3;
const int BOSSBTL_RANK_CNT_BTL = 10;
const int BOSSBTL_RANK_CNT_ALL = 100;
const int BOSSBTL_MAXRANK_CNT  = 10;

const int BOSSBTL_UP_CNT = 5;	//boss战点赞人数
const int BOSSBTL_UP_MAX = 100; //boss战点赞显示限制

enum E_BOSS_REWARD_TYPE
{
	E_BOSS_REWARD_TYPE_RANK = 1,
	E_BOSS_REWARD_TYPE_KILL,
	E_BOSS_REWARD_TYPE_LUCKY,
};

const int BOSSBTL_RECORD_MAX = 100; //

const int BOSSBTL_RE_CREDIT = 20;
const int BOSSBTL_CD_TIME = 30;

const int BOSSBTL_MAX_REPORT_CNT = 5;

enum E_MS_OPT_TYPE
{
    // 1:打开 2:激活军魂 3:领悟技能 4:升级军魂 5：升级技能 6：重置技能 7：锁定技能 
    E_MS_OPT_TYPE_OPEN = 1,
    E_MS_OPT_TYPE_ACTIVE,
    E_MS_OPT_TYPE_COMPREHEND,
    E_MS_OPT_TYPE_SOULUPGRADE,
    E_MS_OPT_TYPE_SKILLUPGRADE,
    E_MS_OPT_TYPE_RESETSKILL,
    E_MS_OPT_TYPE_LOCKSKILL,
};

enum E_MS_OPT_RESULT
{
    //0：成功 1：军魂点不足 2：无效的军魂 3：无效的技能 4：勋章不足 5：无效操作 6；错误的数据 7：军魂已经激活 8：军魂已经达到最高等级
    //9：技能个数超出上限 10：已经领悟技能 11：技能等级超出上限 12：技能已经锁定 13：缺少重置技能道具 14：没有可重置技能
    //15:超出锁定技能个数上限 16：玩家等级未到20 17:军魂系统关闭
    E_MS_OPT_RESULT_Succ = 0,
    E_MS_OPT_RESULT_NeedMorePoint,
    E_MS_OPT_RESULT_NullitySoul,
    E_MS_OPT_RESULT_NullitySkill,
    E_MS_OPT_RESULT_NeedMoreCredit,
    E_MS_OPT_RESULT_NullityOpt,
    E_MS_OPT_RESULT_WrongData,
    E_MS_OPT_RESULT_HasActiveSoul,
    E_MS_OPT_RESULT_MaxSoulLv,
    E_MS_OPT_RESULT_OverSkillNumLimit,
    E_MS_OPT_RESULT_HasComprehendSkill,
    E_MS_OPT_RESULT_SkillLVOutLimit,
    E_MS_OPT_RESULT_SkillHasLock,
    E_MS_OPT_RESULT_NoResetItem,
    E_MS_OPT_RESULT_NoResetSkill,
    E_MS_OPT_RESULT_OverLockLimit,
    E_MS_OPT_RESULT_WrongLevel,
    E_MS_OPT_RESULT_MilitarySoulClose,
};

enum E_MS_POINT_OPT_TYPE
{
    //消耗军魂点 1：激活军魂 2：升级军魂 3：领悟技能 4：升级技能 5：锁定技能 
    E_MS_POINT_OPT_Cost_ActiveSoul = 1,
    E_MS_POINT_OPT_Cost_UpdateSoul,
    E_MS_POINT_OPT_Cost_ActiveSkill,
    E_MS_POINT_OPT_Cost_UpdateSkill,
    E_MS_POINT_OPT_Cost_LockSkill,

    //增加军魂 100：管理员接口增加 101：重置技能返还 102：礼包增加 103：征战奖励 104:BOSS战增加
    E_MS_POINT_OPT_ADD_AddminAdd = 100,
    E_MS_POINT_OPT_ADD_ResetSkill,
    E_MS_POINT_OPT_ADD_GiftPackage,
    E_MS_POINT_OPT_ADD_PVEGift,
	E_MS_POINT_OPT_ADD_BossBtl,
};

enum 
{
	//双十一活动持续的时间
	DOUBLE_ELEVEN_MAX_DAY = 5
};

enum E_GUILD_REDENVELOPE_OPT_TYPE
{
    //操作类型 0：打开 1:发放红包 2：抢红包 3:红包详情 4：查看全区的红包 5：查看本公会的红包 6：查看个人信息
    //7:查看排名 8:查看个人信息
    //100:红包返还 101;刷新压岁钱金额 102:红包被领取完
    E_GUILD_REDENVELOPE_OPEN = 0,
    E_GUILD_REDENVELOPE_ISSUE,
    E_GUILD_REDENVELOPE_SNAPPE,
    E_GUILD_REDENVELOPE_REDENVELOPEINFO,
    E_GUILD_REDENVELOPE_ALLREDENVELOPE,
    E_GUILD_REDENVELOPE_GUILDREDENVELOPE,
    E_GUILD_REDENVELOPE_PERSONALLOG,
    E_GUILD_REDENVELOPE_CHECKRANK,
    E_GUILD_REDENVELOPE_CHECKPERSONALINFO,
    E_GUILD_REDENVELOPE_REFUND = 100,
    E_GUILD_REDENVELOPE_REFRESH,
    E_GUILD_REDENVELOPE_SNAPPEOVER,
};

enum E_GUILD_REDENVELOPE_OPT_RESULT
{
    //0：成功 1：压岁钱不足 2：该红包不存在 3：该红包已经领取 4：金额错误 5：请加入公会 6：该红包已经被抢完 7:红包已经过期 8：活动过期
    //9:请先加入该工会 10:公会等级小于2级
    //100:未知错误
    E_GUILD_REDENVELOPE_R_SUC = 0,
    E_GUILD_REDENVELOPE_R_MORECREDIT,
    E_GUILD_REDENVELOPE_R_WRONGID,
    E_GUILD_REDENVELOPE_R_HASGET,
    E_GUILD_REDENVELOPE_R_WRONGCREDIT,
    E_GUILD_REDENVELOPE_R_NOGUILD,
    E_GUILD_REDENVELOPE_R_HASSNAPPEUP,
    E_GUILD_REDENVELOPE_R_HASOVERDUE,
    E_GUILD_REDENVELOPE_R_ACTOVERDUE,
    E_GUILD_REDENVELOPE_R_NOTGUILDMEMBER,
    E_GUILD_REDENVELOPE_R_GUILDLVLIMIT,

    E_GUILD_REDENVELOPE_R_UNKNOW = 100,
    E_GUILD_REDENVELOPE_R_GUILDNOFOUND,
};

enum E_GUILD_REDENVELOPE_YASUIQIAN
{
    E_GUILD_REDENVELOPE_YASUIQIAN_BUY,
    E_GUILD_REDENVELOPE_YASUIQIAN_SNAPPE,
    E_GUILD_REDENVELOPE_YASUIQIAN_REFUND,
    E_GUILD_REDENVELOPE_YASUIQIAN_FAILISSUE,
    E_GUILD_REDENVELOPE_YASUIQIAN_ADMINADD,

    E_GUILD_REDENVELOPE_YASUIQIAN_ISSUE = 100,
};

enum E_GUILD_REDENVELOPE_OWNELOG
{
    ////个人日志 1：为领取 2：发放 3：领取完毕 4：红包返还 5：取现
    E_GUILD_REDENVELOPE_OWNELOG_SNAPPE = 1,
    E_GUILD_REDENVELOPE_OWNELOG_ISSUE,
    E_GUILD_REDENVELOPE_OWNELOG_ALLSNAPPED,
    E_GUILD_REDENVELOPE_OWNELOG_REFUND,
    E_GUILD_REDENVELOPE_OWNELOG_WITHDRAWAL,
};

/** 每天秒数 */
const int TIME_DAY_SECOND = 86400;

enum
{
	INIT_PLAYER_COINS = 2800,
	INIT_PLAYER_MINERALS = 900,
	INIT_PLAYER_TOTALCOINS = 9000,
	INIT_PLAYER_TOTALMINERALS = 9000,
	INIT_PLAYER_CASH = 100,
	INIT_PLAYER_DMGPROTECTIONTIMETOTAL = TIME_DAY_SECOND * 3 * 1000,// 初始化殖民保护时间
	INIT_PLAYER_PLANETTYPE = 3,
	INIT_PLAYER_HQLEVEL = 1,
	INIT_PLAYER_DROIDS = 1,
};

enum
{
	MAX_HQ_LEVEL = 8,
	ID_CAPITAL_PLANET = 1,
	MAX_NUM_COMMON_ID = 1,
};

enum
{
	SKU_wonders_headquarters	=	10001,
	SKU_rc_001_001	=	20001,
	SKU_rm_001_001	=	20002,
	SKU_rs_002_001	=	20003,
	SKU_rs_001_001	=	20004,
	SKU_labs_observatory	=	20005,
	SKU_hangar_001_001	=	30001,
	SKU_barracks_001_001	=	30002,
	SKU_mechas_001_001	=	30003,
	SKU_shipyards_001_001	=	30004,
	SKU_labs_001_001	=	30005,
	SKU_labs_002_001	=	30006,
	SKU_df_001_004	=	40001,
	SKU_df_001_001	=	40002,
	SKU_df_001_005	=	40003,
	SKU_df_001_003	=	40004,
	SKU_df_001_007	=	40005,
	SKU_df_001_010	=	40006,
	SKU_df_001_002	=	50001,
	SKU_df_001_006	=	50002,
	SKU_bunker_001_002	=	50003,
	SKU_bunker_001_001	=	50004,
	SKU_df_001_008	=	50005,
	SKU_df_001_009	=	50006,
	SKU_d_001_013	=	60001,
	SKU_d_001_014	=	60002,
	SKU_d_001_012	=	60003,
	SKU_d_001_011	=	60004,
	SKU_d_001_001	=	60005,
	SKU_d_001_002	=	60006,
	SKU_d_001_016	=	60007,
	SKU_d_001_010	=	60008,
	SKU_groundUnits_001_001	=	70001,
	SKU_groundUnits_001_002	=	70002,
	SKU_groundUnits_001_003	=	70003,
	SKU_groundUnits_001_005	=	70004,
	SKU_groundUnits_001_004	=	70005,
	SKU_groundUnits_001_006	=	70006,
	SKU_mecaUnits_001_000	=	70007,
	SKU_mecaUnits_002_000	=	70008,
	SKU_mecaUnits_003_000	=	70009,
	SKU_mecaUnits_004_000	=	70010,
	SKU_warSmallShips_001_002	=	70011,
	SKU_warSmallShips_001_003	=	70012,
	SKU_warSmallShips_001_001	=	70013,
	SKU_warSmallShips_001_004	=	70014,
	SKU_mecaUnits_005_000	=	70015,
	SKU_o_001_001	=	80001,
	SKU_o_001_002	=	80002,
	SKU_o_001_003	=	80003,
	SKU_o_001_004	=	80004,
	SKU_o_001_005	=	80005,
	SKU_o_001_006	=	80006,
	SKU_o_001_007	=	80007,
	SKU_o_001_008	=	80008,
	SKU_o_001_009	=	80009,
	SKU_o_001_010	=	80010,
	SKU_o_001_011	=	80011,
	SKU_o_001_012	=	80012,
	SKU_o_001_013	=	80013,
	SKU_o_001_014	=	80014,
	SKU_o_001_015	=	80015,
	SKU_o_001_016	=	80016,
	SKU_o_001_017	=	80017,
	SKU_o_001_018	=	80018,
	SKU_o_001_019	=	80019,
	SKU_o_001_020	=	80020,
	SKU_o_001_021	=	80021,
	SKU_o_001_022	=	80022,
	SKU_o_001_023	=	80023,
	SKU_o_001_024	=	80024,
	SKU_o_001_025	=	80025,
	SKU_o_001_026	=	80026,
	SKU_o_001_027	=	80027,
	SKU_o_001_028	=	80028,
	SKU_o_001_029	=	80029,
	SKU_o_001_030	=	80030,
	SKU_o_001_031	=	80031,
	SKU_o_001_032	=	80032,
	SKU_o_001_033	=	80033,
	SKU_o_001_034	=	80034,
	SKU_o_001_035	=	80035,
	SKU_o_001_036	=	80036,
	SKU_o_001_037	=	80037,
	SKU_o_001_038	=	80038,
	SKU_o_001_039	=	80039,
	SKU_o_001_040	=	80040,
	SKU_o_001_041	=	80041,
	SKU_o_001_043	=	80042,
	SKU_o_001_044	=	80043,
	SKU_o_001_045	=	80044,
	SKU_o_001_047	=	80045,
	SKU_o_001_048	=	80046,
	SKU_o_001_049	=	80047,
	SKU_o_001_051	=	80048,
	SKU_o_001_052	=	80049,
	SKU_o_001_053	=	80050,
	SKU_o_001_054	=	80051,
	SKU_o_001_055	=	80052,
	SKU_o_001_056	=	80053,
	SKU_o_001_057	=	80054,
	SKU_o_001_058	=	80055,
	SKU_o_001_059	=	80056,
	SKU_o_001_060	=	80057,
	SKU_o_001_061	=	80058,
	SKU_o_001_062	=	80059,
	SKU_o_001_063	=	80060,
	SKU_o_001_064	=	80061,
	SKU_o_001_065	=	80062,
	SKU_o_001_066	=	80063,
	SKU_o_001_067	=	80064,
	SKU_o_001_068	=	80065,
	SKU_o_001_069	=	80066,
	SKU_o_001_070	=	80067,
	SKU_o_001_071	=	80068,
	SKU_o_001_072	=	80069,
	SKU_o_001_073	=	80070,
	SKU_o_001_074	=	80071,
	SKU_o_001_075	=	80072,
	SKU_o_001_076	=	80073,
	SKU_o_001_077	=	80074,
	SKU_o_001_078	=	80075,
	SKU_o_001_079	=	80076,
	SKU_o_001_080	=	80077,
	SKU_o_001_081	=	80078,
	SKU_o_001_082	=	80079,
	SKU_o_001_083	=	80080,
	SKU_o_001_084	=	80081,
	SKU_o_001_085	=	80082,
	SKU_o_001_086	=	80083,
	SKU_o_001_087	=	80084,
	SKU_o_001_088	=	80085,
	SKU_o_001_090	=	80087,
	SKU_o_001_091	=	80088,
	SKU_o_001_092	=	80089,
	SKU_o_001_093	=	80090,
	SKU_o_001_094	=	80091,
	SKU_o_001_095	=	80092,
	SKU_o_001_096	=	80093,
	SKU_o_001_097	=	80094,
	SKU_o_001_098	=	80095,
	SKU_o_001_099	=	80096,
	SKU_o_001_100	=	80097,
	SKU_o_001_101	=	80098,
	SKU_o_001_102	=	80099,
	SKU_o_001_103	=	80100,
	SKU_o_001_104	=	80101,
	SKU_o_001_105	=	80102,
	SKU_o_001_106	=	80103,
	SKU_o_001_107	=	80104,
	SKU_o_001_108	=	80105,
	SKU_o_001_109	=	80106,
	SKU_o_001_110	=	80107,
	SKU_o_001_111	=	80108,
	SKU_o_001_112	=	80109,
	SKU_o_001_113	=	80110,
	SKU_o_001_114	=	80111,
	SKU_o_001_115	=	80112,
	SKU_o_001_116	=	80113,
	SKU_o_001_117	=	80114,
	SKU_o_001_118	=	80115,
	SKU_o_001_119	=	80116,
	SKU_o_001_120	=	80117,
	SKU_o_001_121	=	80118,
	SKU_o_001_122	=	80119,
	SKU_o_001_123	=	80120,
	SKU_o_001_124	=	80121,
	SKU_o_001_125	=	80122,
	SKU_o_001_126	=	80123,
	SKU_o_001_127	=	80124,
	SKU_o_001_128	=	80125,
	SKU_o_001_129	=	80126,
	SKU_o_001_130	=	80127,
	SKU_o_001_131	=	80128,
	SKU_o_001_132	=	80129,
	SKU_o_001_133	=	80130,
	SKU_o_001_134	=	80131,
	SKU_o_001_135	=	80132,
	SKU_o_001_136	=	80133,
	SKU_o_001_137	=	80134,
	SKU_o_001_138	=	80135,
	SKU_o_001_139	=	80136,
	SKU_o_001_140	=	80137,
	SKU_o_001_141	=	80138,
	SKU_o_001_142	=	80139,
	SKU_o_001_143	=	80140,
	SKU_o_001_144	=	80141,
	SKU_o_001_145	=	80142,
	SKU_o_001_146	=	80143,
	SKU_o_001_147	=	80144,
	SKU_o_001_148	=	80145,
	SKU_o_001_149	=	80146,
	SKU_o_001_150	=	80147,
	SKU_o_001_151	=	80148,
	SKU_o_001_152	=	80149,
	SKU_o_001_153	=	80150,
	SKU_o_001_154	=	80151,
	SKU_o_001_155	=	80152,
	SKU_o_001_156	=	80153,
	SKU_o_001_157	=	80154,
	SKU_o_001_158	=	80155,
	SKU_o_001_159	=	80156,
	SKU_o_001_160	=	80157,
	SKU_finished	=	90001,
	SKU_npc_A	=	1,
	SKU_npc_B	=	2,
	SKU_npc_C	=	3,
	SKU_npc_D	=	4,
	SKU_specialAttack_1	=	7000	,
	SKU_specialAttack_2	=	7001	,
	SKU_specialAttack_3	=	7002	,
	SKU_specialAttack_10	=	7200	,
	SKU_pumpkin	=	4001	,
};

enum
{
	SocialItem_mistery_gift	    		=	1,
	SocialItem_defense_trap				=	6,
	SocialItem_defense_trap1			=	7,
	SocialItem_unit_starlinator			=	8,
	SocialItem_unit_colossus			=	9,
	SocialItem_unit_zeppelin			=	10,
	SocialItem_blue_light_battery		=	25,
	SocialItem_blue_light_fuel			=	26,
	SocialItem_droidPart1				=	30,
	SocialItem_droidPart2				=	31,
	SocialItem_droidPart3				=	32,
	SocialItem_worker_freeWorker		=	33,
	SocialItem_item_alliance1			=	40,
	SocialItem_item_alliance2			=	41,
	SocialItem_item_alliance3			=	42,
	SocialItem_skull					=	100,
	SocialItem_bones					=	101,
	SocialItem_eggs						=	102,
	SocialItem_incubator				=	103,
	SocialItem_ambar_moskito			=	104,
	SocialItem_golden_skull				=	105,
	SocialItem_golden_idol				=	106,
	SocialItem_golden_imperdible		=	107,
	SocialItem_lingote					=	108,
	SocialItem_ring						=	109,
	SocialItem_stone_skull				=	110,
	SocialItem_mineral_skull			=	111,         
	SocialItem_silver_skull				=	112,              
	SocialItem_silver_idol				=	113,               
	SocialItem_stone_idol				=	114,              
	SocialItem_wood_idol				=	115,            
	SocialItem_mineral_idol				=	116,               
	SocialItem_mineral_eye				=	117,               
	SocialItem_mineral_teeth			=	118,           
	SocialItem_mineral_weap				=	119,
	SocialItem_seed2					=	120,                  
	SocialItem_branch1					=	121,           
	SocialItem_toymine					=	122,
	SocialItem_treestar					=	123,
	SocialItem_present					=	124,
	SocialItem_trap_housing				=	150,               
	SocialItem_trap_trigger				=	151,               
	SocialItem_trap_structure			=	152,       
	SocialItem_trap_explosives			=	153,           
	SocialItem_sensor					=	154,               
	SocialItem_flytrap_housing			=	155,           
	SocialItem_flytrap_propellant		=	156,       
	SocialItem_flytrap_cannons			=	157,           
	SocialItem_flytrap_explosives		=	158,       
	SocialItem_gun						=	159,              
	SocialItem_helmet_housing			=	160,           
	SocialItem_eyepart					=	161,          
	SocialItem_booster					=	162,           
	SocialItem_ammunition				=	163,           
	SocialItem_tank_cannon				=	164,           
	SocialItem_tank_housing				=	165,      
	SocialItem_missile_launcher			=	166,           
	SocialItem_missile					=	167,           
	SocialItem_propeller				=	168,           
	SocialItem_zeppelin_housing			=	169,       
	SocialItem_aileron					=	170,       
	SocialItem_bombs					=	171,       
	SocialItem_pirate_hat				=	172,           
	SocialItem_missileBox				=	200,             
	SocialItem_misilSet					=	201,           
	SocialItem_propulsors				=	202,               
	SocialItem_redButton				=	203,           
	SocialItem_catapult					=	208,               
	SocialItem_rope_catapult			=	209,       
	SocialItem_stone					=	210,               
	SocialItem_wood						=	211,       
	SocialItem_decoration_ancestor		=	300,           
	SocialItem_decoration_stoneidol		=	301,           
	SocialItem_alliance_building		=	303,            
	SocialItem_decoration_xmastree		=	304,           
	SocialItem_decoration_skulltemple	=	305,       
	SocialItem_keyjail					=	1000,
	SocialItem_coins					=	5000,              
	SocialItem_experience				=	5001,          
	SocialItem_minerals					=	5002,              
	SocialItem_missile_rain				=	7000,          
	SocialItem_boulder_strike			=	7001,              
	SocialItem_chips1every50			=	8000,              
	SocialItem_chips1every5				=	8001, 


};

enum
{
	ERROR_CODE_INDEFINED = 0,
	ERROR_CODE_INVALID_COMMAND = 1,
	ERROR_CODE_INVALID_PARAMETERS = 2,
	ERROR_CODE_NAME_TOO_SHORT = 3,
	ERROR_CODE_NAME_ALREADY_TAKEN = 4,
	ERROR_CODE_ALREADY_MEMBER_OF_AN_ALLIANCE = 5,
	ERROR_CODE_ALLIANCE_IS_FULL = 6,
	ERROR_CODE_ALLIANCE_WAS_EMPTY = 7,
	ERROR_CODE_ALLIANCE_NOT_FOUND = 8,
	ERROR_CODE_MEMBER_NOT_FOUND = 9,
	ERROR_CODE_MEMBER_NOT_IN_AN_ALLIANCE = 10,
	ERROR_CODE_INTERNAL_ERROR = 11,
	ERROR_CODE_TIMEOUT = 12,
	ERROR_CODE_NO_PERMISSION = 13,
	ERROR_CODE_MY_ALLIANCE_ALREADY_IN_WAR = 14,
	ERROR_CODE_ALLIANCE_ALREADY_IN_WAR = 15,
	ERROR_CODE_ALLIANCE_LOCKED = 16,
	ERROR_CODE_ALLIANCE_HAS_POST_WAR_SHIELD = 17,
	// 18以上都为自定义
	ERROR_CODE_ALLIANCE_WAR_Not_ALLOWED = 18,
	ERROR_CODE_ALLIANCE_NEED_LEVEL = 19,
};

enum
{
	Alliances_Role_LEADER = 1,
	Alliances_Role_ADMIN  = 2,
	Alliances_Role_REGULAR = 3,
};
