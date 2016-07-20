#pragma once


//国家相关常量定义
const int OLD_GUILD_SIZE = 100000;
const int MAX_CITY_ID = 10000;			//城市允许的最大ID,该值为DB索引，不可修改
#ifdef WIN32
const int MAX_COUNTRYUSER_IDX	= 5;
const int MAX_COUNTRYGUILD_IDX	= 2;
#else
const int MAX_COUNTRYUSER_IDX	= 5000;
const int MAX_COUNTRYGUILD_IDX	= 50;
#endif

const int MAX_COUNTRYBTL_5RATE_REWARD_CNT = 3;	//获得夺城奖励次数
const int MAX_COUNTRYBTL_2RATE_REWARD_CNT = 20;	//获得击退奖励次数
const int COUNTRY_ATK_SWEEP_RATE		  = 3;	//扫荡的计算倍率

const int CITY_USER_GROUP  = 10;
const int COUNTRY_NPC_RATE = 10000;		
const int SEND_CITY_USER_CNT = 15;		//一次发送15个玩家
//const int MAX_COUNTRY_BOX_CNT = 500;	//国家宝箱最大数量
const int MAX_COUNTRY_REPORT_CNT = 20;	//国家纪录的战报最大数量
const int GUILD_LIST_PAGE_CNT	= 11;	//工会列表，每次给玩家展示的数量
const int GUILDBTL_LIST_PAGE_CNT= 10;	//工会战列表，每次给玩家展示的数量

const int MAX_GUILD_ITEMLOG_CNT	= 50;		//工会战利品分配日志
const int MAX_GUILD_LOG_CNT		= 30;
const int GUILD_LOG_TIME		= 2*24*3600;	//工会日志保留时间
const int GUILD_BTL_DAY_CNT = 3;
const int HISTORT_RANK_USER_CNT = 10;		//历届工会战排名，保留10个
enum CountryID
{
	C_Unknow = 0,
	C_AllCountry = C_Unknow,	//所有国家，用于工会排名

	C_Start,				//玩家国家开始
	C_Italy		= C_Start,	//意大利
	C_USA,
	C_England,
	C_Germany,	//德国
	C_Russia,	//苏联
	C_France,
	C_UserCoutryEnd, //玩家国家结束
	
	C_NPCStart= 21,
	C_NPC = C_NPCStart,	
	C_NPCEnd,
	C_CountryCnt,
};



enum E_ADD_COIN_TYPE
{//增加战功，体力，士气等代币的途径
	E_ADD_COIN_Unknow = 0,
	E_ADD_COIN_Admin,	//管理员
	E_ADD_COIN_CostGift,	//消费礼物
	E_ADD_COIN_Item,	//使用物品

	//国战相关
	E_ADD_COIN_CountryBattle = 100, //国战
	E_ADD_CDTIME,					//通过CD时间恢复
	E_ADD_CreditRecover,			//勋章恢复			
	E_ADD_ITEM,						//背包
	E_ADD_CostThewRevover,			//消耗体力恢复	
	E_ADD_COIN_CountryBattle_Def,	//国战防守
	E_ADD_COIN_BloodBtlWinRedward,		//血战胜利奖励
	E_ADD_COIN_LaunchBloodBtlWinRedwar, //发起血战并且胜利的奖励
	E_ADD_OfficerRight,                 //国家官员使用权限恢复
	E_ADD_OfficerPosition,              //国家官员职位变化
	E_ADD_MaxChange,                    //最大值变化

    E_ADD_COIN_CountryBomb,             //国战炸弹

	//工会相关
	E_Add_COIN_GuildContribute = 200,//工会捐献
	E_ADD_COIN_GuildBtlEmbrave,		//鼓舞

	//跨服阵营战
	E_ADD_COIN_Fail, //跨服阵营战回城恢复
	E_ADD_COIN_RegiongCountryBattle, //跨服阵营战战斗
	E_ADD_COIN_RegiongCountryBattleDef, //跨服阵营战战斗防守
	//其他
};	

enum E_COST_COIN_TYPE
{//消耗战功，体力，士气等代币的途径
	E_COST_COIN_Unknow = 0,
	E_COST_COIN_Admin,	//管理员
	//国战相关
	E_COST_COIN_CountryBattle = 100, //国战
	E_COST_COIN_ChgCity, //切换城市
	E_COST_COIN_SPY,		//战车
	E_COST_BOX_CHG,			//战功兑换国家宝箱
	E_COST_RECOVER_MORALE,		//恢复士气
	E_COST_MORALE_Officer, //国家官员职位变化
	E_COST_MaxChange,                    //最大值变化

    E_COST_COIN_CountryBomb,         //国战炸弹

	//工会相关
	E_COST_COIN_GuildStake = 200,	//工会摇杆
	E_COST_COIN_KickStake,			//关闭选项
	E_COST_COIN_Shop,				//商城
	E_COST_COIN_PickWelfar,			//领取福利

};

enum CountryBloodBtlStepType
{//血战计算，记录在内存中
	CBloodBtlStep_Start = 0,
	CBloodBtlStep_PK,
	CBloodBtlStep_Balance,
	CBloodBtlStep_Over,
};

enum CountryOptType
{		//0：选择国家 1:更换国家 2:点击世界地图 3:点击城市 4:前往城市 5:普通侦查 6:高级侦查 
	//7:刷新玩家列表 8:增加行动力 9:增加士气 10:领取国家宝箱 11:开启国家宝箱  12:战功兑换 13:查看战报 14:攻击 100,战斗返回
	COT_SelCountry = 0,
	COT_ChgCountry,
	COT_ClickMap,
	COT_ClickCity,
	COT_GotoCity,
	COT_NormalSpy,
	COT_SeniorSpy,
	COT_UpdateUser,
	COT_AddThew,
	COT_AddMorale,
	COT_PickCountryBox,
	COT_OpenCountryBox,
	COT_ZhanGongChg,
	COT_ShowReport,
	COT_Attack,				//攻击
	COT_BoxPageOpen,		//打开国家宝箱页面
	COT_CountryRank,		//国家排行榜
	COT_ReAttack,			//反击
	COT_AttackByUser,		//被玩家攻击
	COP_AtkSweep,			//扫荡
	COP_SetTargetPage,		//打开设置目标窗口
	COP_SetTarget,			//设置目标城市
	COP_LaunchBloodBtl,		//发起血战
	COP_SeeBloodBtl,		//查看血战信息
	COP_BloodBtlBroadCast,	//广播血战信息
	COP_AutoBloodBtlRedward, //自动血战奖励推送

	COP_DailyReward,	    //每日战功奖励
    COT_AttackBomb,         //投弹
    COT_AttackBombByUser,   //被玩家投弹轰炸
	COT_LeagueBtlMsg,		//联盟战信息请求
	COT_LeagueBtlPlayerRank, //联盟战排行请求
	COT_LeagueBtlGuildRank,	 //联盟战排行请求
	COT_LeagueGetReward,	 //32:领取奖励 
	COT_LeagueRequestReward, //33:请求奖励排行
	COT_LeagueRewardRecord, //34:服务器信息 记录同盟战领取奖励的会长

    COT_TeleportCity,       //35:国战传送城市功能

	COT_AttackOver = 100,	//战斗返回
	COT_ReAttackOver,			//反击
	COP_AtkSweepOver,		

    COT_AttackBombOver,     //投弹攻击返回
    
};

enum CountryRoportType
{//国家战报类型
	Cntr_Report_Attack = 0, //进攻
	Cntr_Report_Defence,	//防守
	Cntr_Report_AtkSweep,	//扫荡
	Cntr_Report_DefSweep,	//防守扫荡

	Cntr_Report_Winning	= 6,	//连胜
	Cntr_Report_OverWinning,//终止连胜
	Cntr_Report_Capture,	//占领城市
	Cntr_Report_BloodBtl,	//血战

    Cntr_Report_Bomb,       //投弹
    Cntr_Report_DefBomb,    //被丢炸弹
};

enum CountryErrorLogType
{//国家错误日志类型
	CntrError_UserHasCountry = 0 ,	//玩家已经有了国家
	CntrError_UserNoCountry,			//玩家无国家
	CntrError_UserNoCity,			//玩家无城市
	CntrError_UserNotFind,			//无玩家
	CntrError_CityError,				//城市属性不一致
	CntrError_TmpChg0506,				//2013.05.06临时修改
	CntrError_LoadUserError,			//加载玩家出错
	CntrError_UserNotFind4Guild,			//无玩家
	CntrError_UserNotFind4ChgGuildLeader,			//无玩家
	CntrError_GuildIDError,					//工会ID错误
	CntrError_GuildCountryError,				//工会ID错误
	CntrError_NoLeader,					//未找到会长
	CntrError_LeaderError,				//会长数据错误
	CntrError_GuildLeaderError,				//会长数据错误
	CntrError_LeaderCntrError,			//会长国家错误

};


//===============================================================
//工会相关
//===============================================================
enum GuildStakeType
{
	G_Stake_Low = 0,	
	G_Stake_High,	
	G_Stake_Cnt,
};
enum GuildScienceType
{
	GS_Start = 0,
	GS_DecSelfMorale = GS_Start,
	GS_AddEnemyMorale,
	GS_AddThewSpeed,
	GS_AddTankAtk,
	GS_AddTankHP,
	GS_AddFootAtk,
	GS_AddFootHP,
	GS_AddPlaneAtk,
	GS_AddPlaneHP,
	GS_AddThewMax,
	GS_Cnt,
};
enum GuildBuildingType
{//工会建筑
	GB_Hall = 0,	//工会大厅
	GB_ScienceStore,//科技大厅
	GB_Shop,		//工会商城
	GB_FootCenter,	//精英军校（步兵）
	GB_PlaneCenter,	//航空中心（空军）
	GB_TankCenter,	//军工研究所（坦克）
	GB_TimeCenter,	//时空研究所
	GB_RateCenter,	//全军备战（打折）
	GB_Cnt,
};

enum GuildContributeType
{//工会捐献类型
	GuildCost_Credit = 0,
	GuildCost_MedalAndOil,	
	GuildCost_Feats,
	GuildCost_Item,		//消耗物品：军令
	GuildCost_Cnt,
};
enum GuildPositionType
{
	GP_Normal	= 0,	//普通成员
	GP_Leader	= 1,	//会长
	GP_SecLeader,		//副会长
	GP_Elder,			//长老
	GP_Cream,			//精英
	GP_LastPos = GP_Cream, //用于升降职位
	GP_Cnt,
};

enum GuildPermit
{
	G_Pmt_UpPos			= 0x01,	//晋升职位
	G_Pmt_DownPos		= 0x02,	//降低职位
	G_Pmt_AppleApp		= 0x04,	//同意申请
	G_Pmt_KickUser		= 0x08,	//踢出成员
	G_Pmt_ChgBulletin	= 0x10,	//修改公告
	G_Pmt_ChgSlogan		= 0x20, //修改口号
	G_Pmt_SendReward	= 0x40, //分配战利品
	G_Pmt_SetLineup		= 0x80,	//布阵
};

enum GuildLogType
{//工会日志
	G_Log_Enter = 0,	//加入工会
	G_Log_Quit,			//退出工会
	G_Log_ChgPos,		//职位变化
	G_Log_Contribute,	//工会捐献
	G_Log_PickWelfare,	//领取福利
	G_Log_BldUpgrade,	//升级建筑
	G_Log_Kick,			//踢出工会
	G_Log_SendReward,	//分配战利品
	G_Log_RankReward,	//排行奖励
	G_Log_BtlRank,		//工会战排名
	G_Log_GvgReward,	//GVG领奖
};

enum GuildOptType
{
	GOT_Open = 0,
	GOT_FindGuild,
	GOT_AllGuildLst,
	GOT_ChgShowGuild,		//换一批工会
	GOT_CltCreateGuild,		//创建工会
	GOT_ApplyGuild,
	GOT_CancelApply,
	GOT_SetFlag,	
	GOT_ChgBulletin	= 10,	// 修改公告
	GOT_ChgSlogan,			//修改口号
	GOT_QuitGuild,			//退出工会
	GOT_DelateLeader,		//弹劾会长
	GOT_GuildLog,			//工会日志
	GOT_PickWelfare,		//领取福利
	GOT_Contribute,			//工会捐献
	GOT_GuildCoinLog,		//工会捐献日志

	GOT_OpenMemberPage=30,	//打开工会成员窗口
	GOT_ChgLeader,			//转让会长
	GOT_UpPos,				//晋升职位
	GOT_DownPos,			//降低职位
	GOT_AgreeApply,			//同意申请
	GOT_KickMember,			//踢出成员
	GOT_ApplyList,			//申请成员列表
	GOT_DisAgreeApply,		//拒绝申请

	GOT_BldUpgrade= 40,	//升级工会建筑
	
	GOT_KickScienceStake = 50,	//勾选科技
	GOT_ScienceStakeLow,		//科技拉杆
	GOT_ScienceStakeHigh,		//科技拉杆

	GOT_OpenGuildShop = 60,			//打开神秘商城
	GOT_SetGuildShopPmt,				//设置商城权限

	GOT_GBtlSinUp		= 70,	//报名工会战
	GOT_GBtlSeeLineup,			//查看阵型
	GOT_GBtlSetLineup,			//布阵
	GOT_GBtlOpenPage,			//打开工会战页面
	GOT_GBtlCreditEmbrave,		//勋章鼓舞
	GOT_GBtlHonorEmbrave,		//荣誉鼓舞
	GOT_GBtlSeeLastBtl,			//观看上次战斗
	GOT_GBtlSeeRankReward,		//查看排名奖励
	GOT_GBtlGetReward,			//领取工会战奖励
	GOT_GBtlAllRank,			//历届工会排名
	GOT_GBtlGuildLst	= 80,	//参战工会列表
	GOT_GBtlSeeReward,			//查看工会战奖励
	GOT_GBtlAllRankTotal,		//历届工会排名总览
	GOT_GBtlSeeStorage,			//查看工会仓库
	GOT_GBtlSendStorageItem,	//分配仓库奖励
	GOT_GBtlSendItemLog,		//仓库分配日志

	GOT_SendAgreeUser = 90,
	GOT_SendKickUser,
	GOT_SendPosChg,
	GOT_SendBldUpgrade,
	//该服务器端使用
	GOT_SrvCreateGuild=100,	//判断工会可否创建
	GOT_SrvUpdateUserPos,		//更新玩家职位
	GOT_SrvAgreeUserApply,		//同意玩家的申请
	GOT_SrvKickMember,			//踢出成员
	GOT_ChgLeaderByDelate,		//弹劾造成更换会长
	GOT_ChgLeaderByDelate2Game,	//弹劾造成更换会长
	GOT_SrvSendStorageItem,		//发放战利品

	//GVE
	GOT_GVE_Open = 200,
	GOT_GVE_NpcList,
	GOT_GVE_Attack,
	GOT_GVE_AtkSweep,
	GOT_GVE_GetBox,
	GOT_GVE_UseBuff,
	GOT_GVE_NormalSyp,
	GOT_GVE_HighSyp,
	GOT_GVE_Map,
	GOT_GVE_BuyThew,
	GOT_GVE_Rank,
	GOT_GVE_GuildRank,
	GOT_GVE_GetGuildRank,
	GOT_GVE_LastRank,
	GOT_GVE_LastGuildRank,
	GOT_GVE_HistoryRank,
	GOT_GVE_GetPersonalBox,
	GOT_GVE_SpNpcList,
	GOT_GVE_GetAllGuildRank,

	GOT_GVE_AttackOver = 250,
	GOT_GVE_AtkSweepOver,
	//TASK
	GOT_TASK_APPLY = 300,//接任务
	GOT_TASK_FLAUSH,		//刷新任务
	GOT_TASK_AWARD,			//领奖
	GOT_TASK_DROP,			//放弃任务
	GOT_TASK_COMPLETE,		//勋章完成

	GOT_SET_DELATETM,		//设置弹劾会长的时间,测试用
	GOT_SET_LEADER,			//指令设置新会长
};

enum GuildOptRet
{
	GOR_SUCCESS = 0,
	GOR_TASK_INPROGRESS_FLUSH = 210,//任务进行中,不能刷新,不能领奖
	GOR_TASK_CREDIT_LESS_FLUSH,//勋章不够,不能刷新,不能花勋章完成
	GOR_TASK_CREDIT_LESS_COMPLETE,//勋章不够,不能立即完成
	GOR_TASK_CREDIT_STATUSERR_COMPLETE,//任务状态不对,如未接或已经完成,不能立即完成
	GOR_TASK_NOT_COMPLETE_AWARD,//任务未完成,不能领奖
	GOR_TASK_ERROR,
};
enum GuildBtlResultType
{//工会战结果类型
	GBR_Unknow	= 0,
	GBR_Win0Way,		//胜0路
	GBR_Win1Way,		//胜1路
	GBR_Win2Way,		//胜2路
	GBR_Win3Way,		//全胜
	GBR_WinNoEnemy,		//轮空
	GBR_Cnt,
};

enum GuildBtlOneWayResultType
{//工会战单路结果
	GBOResult_Lose	= -1,
	GBOResult_Unknow = 0,
	GBOResult_Win	= 1,
	GBOResult_Draw	= 2,	//平局
	GBOResult_WinNoEnemy,	//轮空
};

enum GuildBtlStepType
{//工会战阶段
	GBtlType_UnKnow = 0,
	GBtlType_SignUp,
	GBtlType_LineUp0,
	GBtlType_Btl0,
	GBtlType_LineUp1,
	GBtlType_Btl1,
	GBtlType_LineUp2,
	GBtlType_Btl2,
	GBtlType_Over,
	
};

enum GuildBtlMatchType
{//工会战组队模式
	GBMType_Rand = 0,	//随机组队
	GBMType_Score,		//根据积分组队
};

enum GuildBtlToolType
{//工会战道具
	GBTool_Empty = 0,
	GBTool_CostHP,	//电磁风暴，减血
	GBTool_CostAtk,		//核爆辐射，减攻
	GBTool_Cnt,
};

enum CountryBloodBtlType
{//城市血战类型
	CBT_UserLaunch = 0,	//玩家发起
	CBT_SYSLaunch,		//系统发起
};

const int MAX_REGION_COUNTRY_CNT = 100; //跨区国战最大配表数量

enum RegionCountryOptType
{	//0:点击世界地图 		1:点击城市 		2:前往城市 		3:普通侦查 		4:高级侦查 					5:刷新玩家列表 		6:增加行动力 
	//7:增加士气 		 	8:查看战报 	9:攻击
	//10:反击 				11:推送被攻击信息 	12:扫荡 		13:报名	
	RCOT_ClickMap = 0,
	RCOT_ClickCity,
	RCOT_GotoCity,
	RCOT_NormalSpy,
	RCOT_SeniorSpy,
	RCOT_UpdateUser,
	RCOT_AddThew,
	RCOT_AddMorale,
	RCOT_ShowReport,
	RCOT_Attack,				//攻击
	RCOT_ReAttack,			//反击
	RCOT_AttackByUser,		//被玩家攻击
	RCOP_AtkSweep,			//扫荡
	RCOP_Sign,				//报名
	RCOP_AdminSign,			//报名
	RCOT_Init,				//初始化
	RCOT_Over,				//战斗结束
	RCOT_PlayerList,		//战功排名
	RCOT_LastList,			//上次排名
	RCOT_ZhangongRecord,	//战功记录
	RCOT_SignCheck,			//判断是否符合报名条件
	RCOT_GetGift,
	RCOT_SignFree,			//各国战功排行前5的玩家免费报名
	RCOT_MapUpdate,			//更新双方分数与地图

	RCOT_AttackOver = 100,	//战斗返回
	RCOT_ReAttackOver,			//反击
	RCOP_AtkSweepOver,		
};

enum RegionCountryID
{
	RC_Unknow = 0,
	RC_CountryStart,
	RC_Country1 = RC_CountryStart,
	RC_Country2,
	RC_CountryNPC,
	RC_CountryNum = RC_CountryNPC,
	RC_End,	
};

const int MAX_REGION_COUNTRY_RANK_NUM = 100;//跨区国战战功排行最大数量

const int WHOLE_COUNTRY_FAIL_MOVE_CD = 15;
const int WHOLE_COUNTRY_FAIL_PROTECT_CD = 300;
const int WHOLE_COUNTRY_FAIL_MORALE = 50;

enum RegionCountryWinType
{
	RCWT_Normal = 0,
	RCWT_Boom,
	RCWT_Head,
};

const int WHOLE_COUNTRY_UPDATE_TIME = 60;

const int GVE_START_STEP = 3101;
const int GVE_SP_STEP = 4001;
const int GVE_SP_NPC_ADD = 1000;

enum GveBuffType
{
	GveBuffType_Master = 1, //会长用BUFF
	GveBuffType_First,	//上届第一名用BUFF
};

const int GVE_MAX_RANK = 100;

const int MAX_RESOURSE_SEARCH_TYPE = 3;
const int MAX_RESOURSE_TYPE = 6;

const int MAX_RESOURSE_ID = 10000;	
const int MAX_RESOURSE_TYPE_ID = 100000;
const int MAX_RESOURSE_SAVE_ID = 10000;

const int RESOURCE_SEARCH_CNT = 3;

const int MAX_WARGAME_SAVE_ID = 10000;
const int MAX_WARGAME_DELAY_NUM = 2;                //每个演习场地最大的延时次数
const int WARGAME_ALLINFO_REFRESH_INTERVAL = 3;

enum ResourceOptType
{//1:打开页面 2:探索 3:占领 4:撤退 5:更新状态 6:战斗信息
	ROT_Open = 1,
	ROT_Search,
	ROT_Attack,
	ROT_Leave,
	ROT_Update,
	ROT_Record,
	ROT_ReAtk,
	ROT_AttackOver = 100,
	ROT_AttackOverCountry,
	ROT_AttackOverCountryEnemy,
};

enum ResourceSearchType
{
	RST_Free = 1,
	RST_20,
	RST_100,
};

const int MAX_RESOURCE_RECORD = 100;
const int MAX_RESOURCE_THEWTIME = 30;

enum Guild_Task_Status
{
	E_GTS_NOTASK,
	E_GTS_PROGRESS,
	E_GTS_DONE,
};

enum Guild_Task_Type
{
	E_GTT_BROKEN = 1,
	E_GTT_REPAIR,
	E_GTT_CONTRIBUTE,
	E_GTT_UPDATESCIENCE,
	E_GTT_BATTLE,
};

enum GvgBtlStepType
{//gvg阶段
	GvgBtlType_UnKnow = 0,
	GvgBtlType_SignUp,
	GvgBtlType_PreTime,
	GvgBtlType_LineUp0,
	GvgBtlType_Btl0,
	GvgBtlType_LineUp1,
	GvgBtlType_Btl1,
	GvgBtlType_Over,

};

enum GvgOptType
{//0:打开GVG页面 			1:点击城市						2:上轮战报			3:查看排名			4:报名		5:进入战场		
 //6:普通鼓舞		7:勋章鼓舞		8:购买连胜		9:破坏城防		10:修复城防		11:领取奖励		12:更新数据 13:GVG面板 14:离开城市 15:查看城市内玩家列表 16:战斗信息 17:贡献信息 18:离线进场 19:取消离线 
	Gvg_Open = 0,
	Gvg_Click,
	Gvg_Report,
	Gvg_Rank,
	Gvg_Sign,
	Gvg_Enter,
	Gvg_AddProp,
	Gvg_AddPropCredit,
	Gvg_BuyStreak,
	Gvg_BrokeCity,
	Gvg_DefCity,
	Gvg_GetReward,
	Gvg_Update,
	Gvg_GvgInterface,
	Gvg_Leave,
	Gvg_SeeCityPlayer,
	Gvg_BtlMsg,
	Gvg_ContributeData,
	Gvg_OffLineEnter,
	Gvg_OffLineLeave,

	Gvg_BrokeCityOver = 100,
	Gvg_DefCityOver,
	Gvg_RewardRecord,
};

const int GVG_UPDATE_TIME = 3600;	
const int MAX_SIGN_CNT = 2;
const int MAX_FIGHT_TIME = 2;

const int MIN_GVG_FIGHT_TIME = 120;

const int MAX_GVG_SEND_NUM = 20;

const int MAX_GVG_PLAYER_RECORD_NUM = 30;

const int MAX_WPC_RANK_NUM = 10;

const int WPC_RANK_CHAMPION = 10001;

enum WarGameOptType
{//军事演习类型
    WGOT_Open = 1,
    WGOT_Occupy,                        //占领
    WGOT_Loot,                          //抢占
    WGOT_Leave,                         //撤退
    WGOT_ReFresh,                       //刷新
    WGOT_FightRecord,                   //战斗日志
    WGOT_Counterattack,                 //反攻
    WGOT_DelaySite,                     //延时
    WGOT_BuyTokens,                     //购买演习令

    WGOT_BeAttack,                      //被攻击

    WGOT_GetSiteList,                   //获取演习场列表

    WGOT_Truce,                         //免战

    WGOT_AttackedFlag,                  //显示被攻击的气泡

    WGOT_FightReturn = 100,                   //战斗返回
    WGOT_LootOver,
    WGOT_CounterAttackOver,
    WGOT_AttackOverCountryAtck,
    WGOT_AttackOverCountryEnemy,
};

enum WarGameOptResult
{
    //0:成功 1:已经占领演习场地 2:勋章不足 3:无效操作 4:演习场已被占领 
    //5:数据错误 6:占领次数达到上限 7:演习令不足 8:未占领演习场 9:敌人未占领演习场 10:演习场在保护期 11:演习场占领者已经更换
    //12：功勋不足 13:已经达到最大延期次数 14;没有集团军营
    //100战斗成功 101战斗失败
    WGOR_Success = 0,
    WGOR_HasOccupy,
    WGOR_NoEnoughCredit,
    WGOR_NullityOpt,
    WGOR_OccupyByOther,
    WGOR_WrongData,
    WGOR_OverLimit,
    WGOR_NoEnoughToken,
    WGOR_NoOccupySite,
    WGOR_EnemyNoOccpySite,
    WGOR_OccupyOnProtect,
    WGOR_OccupantChange,
    WGOR_NoEnoughFeats,
    WGOR_OverLimitDelayNum,
    WGOR_NoBarracks,
    WGOR_NoStrategicArmy,
    WGOR_NoTruceProtol,
    WGOR_NoForceBook,
    WGOR_SiteOnTruce,

    WGOR_FightWin = 100,
    WGOR_FightDefeat,
};

enum LeagueBtlCnt
{
	LEAGUEBTLCNTSTART = 1,
	LEAGUEBTLCNTEND = 2,
	LEAGUEBTLDRAW = 100,//判断平局
};

enum LeagueRewardType
{
	LEAGUERWDTYPEPERSON = 1,
	LEAGUERWDTYPEGUILD = 2,
};

const int MAX_LEAGUE_RANK_NUM = 50;
const int MAX_LEAGUE_RANK_SEND_NUM = 30;

enum LeagueBtlStepType
{//gvg阶段
	LeagueBtlType_PreTime = 0,
	LeagueBtlType_Battle,
	LeagueBtlType_Over,
	LeagueBtlType_Reward,
};

const int MAX_BOSSBTLMSG_SAVE_ID = 10000;

enum BossOptType
{
	BOT_Open = 1,
	BOT_OpenBtl,
	BOT_Up,
	BOT_SeeUp,
	BOT_SeeRank,
	BOT_GetReward,
	BOT_EmbraveHonor,
	BOT_EmbraveCredit,
	BOT_SetWords,
	BOT_ReportWords,
	BOT_Update,
	BOT_Atk,
	BOT_ClearCD,
	BOT_SeeHighRank,
	BOT_SeeUpRank,
	BOT_UpdateBtl,
	BOT_FinalAtk,		//最后一击
	BOT_AtkOver = 100,
	BOT_AtkMsg = 101,
};

enum CountryChatOptType
{
	CCOT_NONE = 0,
	CCOT_ONE  = 1,//服务器给全国推送单条消息
	CCOT_CHATUI,//打开聊天框,发送100条
	CCOT_CHATCOUNTRY,//会长发聊天
	CCOT_CHATWORLD,		//世界发言
	CCOT_MASK,		//屏蔽某会长的聊天
	CCOT_MASKUI,	//打开屏蔽界面
	CCOT_UNMASK,	//解除屏蔽
};

enum CountryChatOptRet
{
	CCOR_SUCCESS = 0,//成功
	CCOR_NOTLEADER,		//不是会长
	CCOR_GUILDLV,		//公会等级不足
	CCOR_LESSBROAD,		//缺少小广播
	CCOR_REGIONERR,		//countryd上的错误
	CCOR_REGIONCOUNTRY,	//countryd上的错误
	CCOR_NOMSG,			//没有消息记录
	CCOR_INCOLD,		//没过5s cd时间
	CCOR_TBLERR,		//读表错误
	CCOR_MSGLEN,		//消息太长
	CCOR_ERRMASKID,	 //要屏蔽的uid错误
	CCOR_COUNTRYERR,	//取countrymanager错误
	CCOR_COUNTRYDBERR,	//取db_country出错
	CCOR_MASKDB,		//mask db
	CCOR_MASKOVER,		//屏蔽的会长数目满30个了
	CCOR_NOMASK,		//没有屏蔽的会长
	CCOR_NOMASKID,		//没找到要解除屏蔽的uid
	CCOR_MASKSELF,		//屏蔽自己??
	CCOR_MASKAGAIN,		//已经屏蔽了
};

const int MAX_COUNTRY_CHAT_MSGNUM = 100;//国战聊天,最多100条
const int MAX_COUNTRY_CHAT_MSGLEN = 70;//每条最多70个中文