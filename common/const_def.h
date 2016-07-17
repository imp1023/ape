#ifndef _CONST_DEF_H_
#define _CONST_DEF_H_

typedef long long		time_d;
typedef long long       int64;
typedef unsigned char   byte;
typedef unsigned int    u_int;

#define BASE_RATE 10000.0
#define MAX_NUMBER 2100000000
#define WORLD_SPECIAL 88888
#define INVALID_VALUE -1

/** 每天秒数 */
#define TIME_DAY_SECOND 86400

// --------------新手玩家基本属性--------------------
enum INIT_PLAYER_DATA
{
	INIT_PLAYER_COINS = 2800,
	INIT_PLAYER_MINERALS = 900,
	INIT_PLAYER_TOTALCOINS = 9000,
	INIT_PLAYER_TOTALMINERALS = 9000,
	INIT_PLAYER_CASH = 100,
	INIT_PLAYER_HQLEVEL = 0,
	INIT_PLAYER_DROIDS = 1,
	INIT_PLAYER_PLANETTYPE = 3,// 總共有3中
	INIT_PLAYER_DMGPROTECTIONTIMETOTAL = TIME_DAY_SECOND * 3 * 1000,// 初始化殖民保护时间
};

enum GameWorkingStatus 
{
	NOT_CONNECTED	= 0,
	NOT_SYNC_USER	= 1,
	LOST_CONNECTION = 2,
	NORMAL			= 3
};

enum Gender_Type
{
    Gender_None     = 0,
    Gender_Boy      = 1,
    Gender_Girl     = 2,
    Gender_Both     = 3,
};

//平台类型，对应WEB的siteid
enum PLAT_TYPE
{
	PLAT_TYPE_MIN   = 0,
	PLAT_TYPE_TEST  = 1,
	PLAT_PP		    = 10,	//PP助手
	PLAT_YINGYONGBAO,		//应用宝
	PLAT_UC,				//UC平台
	PLAT_360,				//360平台
	PLAT_MI,				//小米平台
	PLAT_BAIDU,				//百度平台
	PLAT_WANDOUJIA,			//豌豆荚平台
	PLAT_DANGLE,			//当乐平台
	PLAT_HUAWEI,			//华为平台
	PLAT_LENOVO,			//联想平台
	PLAT_JINLI,	      		//金立平台
	PLAT_OPPO,	      		//OPPO平台
	PLAT_ANZHI,	      		//安智平台
	PLAT_APPSTORE_CHINA,	//中国APPSTORE
	PLAT_YINGYONGHUI,		//应用汇平台
	PLAT_GOOGLEPLAY_ENGLAND,//英国GooglePlay
	PLAT_JINSHAN,			//金山平台
	PLAT_MEIZU,				//魅族平台
	PLAT_HTC,				//HTC平台
	PLAT_YOUKU,				//优酷平台
	PLAT_SOUGOU,			//搜狗平台
	PLAT_MUZHIWAN,			//拇指玩
	PLAT_WARANDROID,		//海战安卓
	PLAT_ENGFBGOOGLEPLAY,	//英国facebook登陆googleplay支付
	PLAT_ENGGFGOOGLEPLAY,   //英国公司官方登陆googleplay支付
	PLAT_91,				//91
	PLAT_KUAIYONG,			//快用
	PLAT_TONGBUTUI,			//同步推
	PLAT_ITOOLS,			//iTools
	PLAT_TWGOOGLEPLAY,		//台湾googleplay
	PLAT_OUPENG,			//欧朋
	PLAT_PIPANET,			//琵琶网
	PLAT_KUPAI,				//酷派
	PLAT_AIBEICLOUD,		//爱贝云计费
	PLAT_WARIOS,			//海战IOS(中国)
	PLAT_QQTOANDROID,		//QQ授权安卓
	PLAT_QQTOIOS,			//QQ授权IOS
	PLAT_SINAWEIBOTOANDROID,//新浪微博授权安卓
	PLAT_SINAWEIBOTOIOS,	//新浪微博授权IOS
	WORLDSHIP_ANDROID_CN,	//海战安卓(中国)
	CHANNEL_VIVO,			//VIVO
	CHANNEL_FACEBOOK_TW,	//Facebook登陆台湾版
	CHANNEL_WORLDSHIP_TW,	//官方登陆台湾版
	CHANNEL_WEIXIN_ANDROID,	//微信授权安卓
	CHANNEL_WEIXIN_IOS,		//微信授权IOS
	FACEBOOK_US,			//Facebook登陆美国版
	WORLDSHIP_US,			//官方登陆美国版
	FACEBOOK_JP 	 = 60,	//Facebook登陆日本版
	WORLDSHIP_JP 	 = 61,	//官方登陆日本版
	FACEBOOK_IOS_JP  = 63,	//Facebook登陆IOS日本版
	WORLDSHIP_IOS_JP = 64,	//官方登陆IOS日本版
	LoveApple_IOS_YY = 65,  //登陆支付_爱苹果_ios越狱
	SeaHorse_IOS_YY  = 66,  //登陆_海马_ios越狱
	AiSi_IOS_YY		 = 67,	//登陆支付_爱思_ios越狱
	XY_IOS_YY		 = 68,	//登陆支付_XY_ios越狱
	SeaHorse_AIBEI	 = 69,	//支付_海马_爱贝
	PLAT_XUNLEI		 = 70,	//登陆支付_迅雷
	PLAT_PPS		 = 71,  //登陆支付_pps
	PLAT_MOBILE		 = 72,  //登陆支付_移动
	PLAT_MOBILE_ZFB	 = 73, //支付_移动支付宝
	PLAT_LOGIN_9S_ANDROID	 = 74,    //登录_9s_安卓 
	PLAT_PAY_9S_GPLAY = 75,     //支付_9s_GooglePlay 
	PLAT_PAY_9S_MyCard = 76,     //支付_9s_MyCard 
	PLAT_LOGIN_9S_IOS = 77,     //登录_9s_IOS 
	PLAT_PAY_9S_IOS = 78,     //支付_9s_IOS 
	PLAT_PAY_9S_WEB = 79,     //支付_9s第三方web支付
	PLAT_LOGIN_FD_ANDROID = 80,    // 登录_飞豆_安卓
	PLAT_LOGIN_FD_IOS = 81,     //登录_飞豆_IOS
	PLAT_PAY_FD_ANDROID = 82,     //支付_飞豆_GooglePlay
	PLAT_LOGIN_9S_ANDROID2 = 83,     //登录_9s_戰艦帝國_安卓
	PLAT_PAY_9S_GPLY2 = 84,     //支付_9s_戰艦帝國_GooglePlay
	PLAT_PAY_9S_MYCARD2 = 85,  //支付_9s_戰艦帝國_MyCard
	PLAT_LOGIN_9S_IOS2 = 86,     //登录_9s_戰艦帝國_IOS
	PLAT_PAY_9S_IOS2 = 87,     //支付_9s_戰艦帝國_IOS
	PLAT_PAY_9S_WEB2 = 88,     //支付_9s_戰艦帝國_第三方web支付
	PLAT_LOGIN_9S_ANDROID3 = 89,     //登录_9s_戦争が迫る_安卓
	PLAT_PAY_9S_GPLY_3 = 90,     //支付_9s_戦争が迫る_GooglePlay
	PLAT_PAY_9S_MYCARD3 = 91,     //支付_9s_戦争が迫る_MyCard
	PLAT_LOGIN_9S_IOS3 = 92,     //登录_9s_戦争が迫る_IOS
	PLAT_PAY_9S_IOS3 = 93,     //支付_9s_戦争が迫る_IOS
	PLAT_PAY_9S_WEB3 = 94,     //支付_9s_戦争が迫る_第三方web支付
	PLAT_LOGIN_9S_ANDROID4 = 95,     //登录_9s_海戰帝國_安卓
	PLAT_PAY_9S_GPLY_4 = 96,     //支付_9s_海戰帝國_GooglePlay
	PLAT_PAY_9S_MYCARD4 = 97,     //支付_9s_海戰帝國_MyCard
	PLAT_LOGIN_9S_IOS4 = 98,     //登录_9s_海戰帝國_IOS
	PLAT_PAY_9S_IOS4 = 99,     //支付_9s_海戰帝國_IOS
	PLAT_PAY_9S_WEB4 = 100,   //支付_9s_海戰帝國_第三方web支付
	PLAT_TYPE_MAX
};

enum LOCALE_TYPE
{
    LOCALE_ZH_CN    = 1,
    LOCALE_ZH_TW,//台湾
    LOCALE_EN_US,//欧美版
    LOCALE_TR_TR,//东南亚使用
	LOCALE_EN_EN,
	LOCALE_JP_JP,//日本
	LOCALE_JP_JP2,//韩国使用
	LOCALE_RUSSIA,//俄罗斯
	LOCALE_GERMANY,//德国
	LOCALE_FRANCE,//法国
	LOCALE_TUR_TUR,//土耳其
	LOCALE_POR_POR,//葡萄牙
	LOCAL_ITA_ITA,//意大利
	LOCAL_MID_EAST,//中东英文版
	LOCAL_MID_EAST_ARAB,//中东阿拉伯版
};

enum CHANNEL_TYPE
{
    CHANNEL_APPSTORE    = 1,
    CHANNEL_UC,
	CHANNEL_YYB,
	CHANNEL_GPLAY,
};

enum Money_Type
{
	Money_Gold	= 1,
	Money_Cash,
	Money_Point,
	Money_Oil,
	Money_GuildPoint,
	Money_Electric,
	Money_CampBattleScore,
	Money_JuniorPaper,
	Money_SeniorPaper,
    Money_Prestige,
	Money_JuniorCaptain,
	Money_SeniorCaptain,
    Money_Exploit,
    Money_RemouldMaterial,
    Money_SeikoCoin,
};

enum BattleType
{//与客户端通信用
	BATTLE_TYPE_STAGE 	 = 1,//
	BATTLE_TYPE_ADVSTAGE = 2,//
	BATTLE_TYPE_ARENA 	 = 3,//
	BATTLE_TYPE_TREASURESTAGE = 4,//
	BATTLE_TYPE_ROBBERY = 8,
	BATTLE_TYPE_GUARD = 9,
	BATTLE_TYPE_CAMPBATTLE = 10,
	BATTLE_TYPE_CENTERCAMPBATTLE = 11,
	BATTLE_TYPE_GUILDBATTLE = 13,//12据说客户端 争霸赛用了
	BATTLE_TYPE_ARMADA = 14,//挑战无敌舰队
	BATTLE_TYPE_STAGETOWER = 16
};

enum DropType
{
	DROP_TYPE_NULL  = -1,
	DROP_TYPE_PROP  = 1,//道具
	DROP_TYPE_PARTS,	//配件
	DROP_TYPE_GOLD,		//金币
	DROP_TYPE_CASH,		//钻石
	DROP_TYPE_POINT,	//科技点
	DROP_TYPE_PAPER,	//图纸
	DROP_TYPE_PAPERPIECE,//图纸碎片
	DROP_TYPE_VIRTUAL,	//虚拟道具
	DROP_TYPE_OIL,		//原油
	DROP_TYPE_PARTPIECE,//配件碎片
	DROP_TYPE_SOUL,		//徽章
	DROP_TYPE_CAMPBATTLESCORE, //声望
    DROP_TYPE_TOKEN,        //代币
	DROP_TYPE_MILITARYRANKHONOUR, //荣誉
	DROP_TYPE_FORMATION,//阵型
	DROP_TYPE_CAPTAIN,//舰长
	DROP_TYPE_CAPTAINPIECE,//舰长碎片
	DROP_TYPE_JUNIORCAPTAIN,//普通将魂
	DROP_TYPE_SENIORCAPTAIN,//高级将魂
    DROP_TYPE_PRESTIGE = 20,    //威望
	DROP_TYPE_SOULPIECE = 21,	//勋章碎片
	DROP_TYPE_EXPLOIT	= 22,	//功勋点
	DROP_TYPE_SEIKO		= 23,	//精工
	DROP_TYPE_SEIKOPIECE	= 24,	//精工碎片
	DROP_TYPE_SEIKOCOIN		= 25,	//精工币
	DROP_TYPE_SEIKOPOINT	= 26,	//精工点
	DROP_TYPE_GUILDPOINT	= 27,	
	DROP_TYPE_MAX,
};

enum ShipTokenType
{
    TOKEN_TYPE_NULL  = 0,
    TOKEN_TYPE_CHRITMAS_CANN,    //酒杯
    TOKEN_TYPE_CHRITMAS_RING,    //戒指
    TOKEN_TYPE_CHRITMAS_HEART,    //海洋之心
    TOKEN_TYPE_CHRITMAS_CANVAS,    //油画
    TOKEN_TYPE_CHRITMAS_CROWN,    //皇冠
    TOKEN_TYPE_VALENTINE_ROSE,         //情人节活动红玫瑰
//    TOKEN_TYPE_CHALLENGE_PRESTIGE,  //挑战的威望
    TOKEN_TYPE_MAX,
};

enum ChristmasLotteryError
{
    CHRISTMAS_LOTTERY_INVAILD = -1,
    CHRISTMAS_LOTTERY_SUCCESS,
    CHRISTMAS_LOTTERY_NOT_CARSH,
    CHRISTMAS_LOTTERY_NOT_BEGIN,
    CHRISTMAS_LOTTERY_MAX,
};

enum ChristmasMineralError
{
    CHRISTMAS_MINERAL_INVAILD = -1,
    CHRISTMAS_MINERAL_SUCCESS,
    CHRISTMAS_MINERAL_NOT_CROWN,
    CHRISTMAS_MINERAL_NOT_CANVAS,
    CHRISTMAS_MINERAL_NOT_RING,
    CHRISTMAS_MINERAL_NOT_CANN,
    CHRISTMAS_MINERAL_NOT_HEART,
    CHRISTMAS_MINERAL_NOT_BEGIN,
    CHRISTMAS_MINERAL_TAKEN,
	CHRISTMAS_MINERAL_ERR_NUMBER,	//最少兑换一个
    CHRISTMAS_MINERAL_ERROR_MAX,
};

enum VirtualPropType
{
	VIRTUALPROP_TYPE_NULL  = -1,
	VIRTUALPROP_TYPE_GOLD  = 1,
	VIRTUALPROP_TYPE_CASH,
	VIRTUALPROP_TYPE_POINT,
	VIRTUALPROP_TYPE_JUNIORPAPER,
	VIRTUALPROP_TYPE_SENIORPAPER,
	VIRTUALPROP_TYPE_ARENACOUNT,
	VIRTUALPROP_TYPE_ELECTRIC,
	VIRTUALPROP_TYPE_MILITARYHON,//军衔荣誉
};

enum CHAT_TYPE
{
	CHAT_TYPE_WORLD,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_PRIVATE,
	CHAT_TYPE_CAMP,
};

//Player对游戏内资源的操作(经验，金钱等)
enum PLAYER_ADD_EXP_OPTYPE
{
	EXP_AdvStageMgr_Attack,
	EXP_StageMgr_Attack,
	EXP_TreasureStageMgr_Attack,
	EXP_GM_Add,
};
enum PLAYER_ADD_MONEY_OPTYPE
{
	GM_AddMoney_Add = 0,
	Player_AddDropData_Add,
	PropMgr_sellProp_Add,
	User_setRole_Add,
	PropMgr_useVirtualProp_Add,
	AccBind_Add,
	Recharge_Add,
	First_Recharge_Add,
	Activity_Recharge_Add,
	Plyaer_MontCard_Add,
	AdvStageMgr_Attack_Add,
	BuildMgr_supply_Add,
	BuildMgr_cashSupply_Add,
	PartsMgr_DecomposeParts_Add,
	QuizMgr_checkFinish_Add,
	ShipMgr_RetireShip_Add,
	StageMgr_Attack_Add,
	TreasureStageMgr_Attack_Add,
	PartsMgr_sellProp_Add,
	Player_UpdateOil_Add,
	BuildMgr_buyOil_Add,
	ActivityMgr_Oil_Add,
	Player_LevelUp_Add,
	GuildMgr_Donate_Add,
	Player_UpdateElectric_Add,
	Activity_GROWUP_REWARD,
	FriendMgr_DrawElectric_Add,
	GuardMgr_StartBattle_Add,
	CampBattle_Award_Add,
	PaperMgr_SalePaper_Add,
	GM_Add,
	MilitaryRank_DailyReward_Add,
	Arena_MilitaryRankHonour_Add,
	ArenaMgr_UpdateScore_Add,
    LuckyCatMgr_Add,
	SpyPaer_EscapeBet,
	ShopBuy_EscapeBet,
	InviterReward_Add,
    BonusFly_Add,
	LoginPoker_Add,
	Reduction_Parts_Add,
	LuckyBox_PointExchange_Add,
	UserComBack_Reward_Add,
	Activity_SignIn_Add,
	Activity_Vip_Add,
	Activity_Level_Add,
	Activity_StartServer_Add,
	Activity_FirstRecharge_Add,
	Activity_ConsumeExchange_Add,
	Activity_RechargeReward_Add,
	Activity_LuckyWheel_Add,
	Activity_MisteryShop_Add,
	Activity_LuckyGoddnessFree_Add,
	Activity_LuckyGoddnessCharge_Add,
	Activity_LuckyGoddnessReward_Add,
	AwardCenter_GetAward_Add,
	GhostShip_Explore_Add,
	LimitShip_Add,
	LimitFight_Add,
	Christmas_Lottery_Add,
	Christmas_ScoreExchange_Add,
	Christmas_MineralExchange_Add,
	RechargeSeven_Award_Add,
	Robbery_Attack_Add,
	Prop_UseProp_Add,
	EscapeBet_Add,
	MonthSign_DistributeReward_Add,
	MonthSign_RedistributeReward_Add,
	Shop_Buy_Add,
	InviteUser_Add,
    Valentine_Add,
    OnearmedBandkit_Add,
	Richest_Award_Add,
	CampBattle_Add,
	CenterCampBattle_Add,
	Compensate_Add,
	HolidayGift_Add,
	IosLevel_Add,
	FirstPay_Recharge_Add,	
	FiveStarEvaluate_Add,
	IOSDownLoad_Add,
	IOSShare_Add,
	SubPackage_Add,
	OnearmBandit_Add,
	DayDayGift_Add = 84,
	BINDMAIL_Add = 85,//绑定邮箱
	OnearmBandit_60_Add,
	GM_Compensate,
	Activity_HandleRegist2DayPaper_Add,
    Challenge_Worship_Add,
    Challenge_Reward,
	Captain_RetireAdd,
	CaptainRecruit_Add,
	LIMITCAPTAIN_ADD,
	LIMITCAPTAIN_SOCRE_ADD,
	LIMITCAPTAIN_RANK_ADD,
	CAPTAINSOUL_GM_ADD,
	CAPTAINSOUL_CAPTAINRETIRE_ADD,//解雇
	SuperCaptain_Gift_ADD,
	LIMITSHIP_BUY_ADD,
	BuyLimitCaptain_Add,
	BuyGHostShip_Add,
    Decompose_Exploit_Add,
    Restore_Exploit_Add,
	GuildBattle_PersonalRank,
	GuildBattle_GuildRank,
	ArmadaMgr_Attack_Add,
	GoldIsland_Add,
	Armada_Award_Add,
	LimitTimeShop_Add,
	EveryDay_Task_Reward_Add,
	PlotCopy_Add,
	Stage_AddTalent,
	Guard_AddTalent,
	Military_AddTalent,
	INITSYS_ADDTalent,
	GM_ADDTalent,
	EveryDay_Task_ScoreReward_Add,
	Salute_ADD,
	Valentine_Award_Add,
	Valentine_Socre_Add,
	GlobalArena_UpdateTacBack,
	GlobalArena_EncourageBack,
	GlobalArena_Prostrate,
	GoldLeader_Rank_Add,
	GoldLeader_Gold_Add,
	GoldLeader_Explore_Add,
	Reduction_Seiko_Add,
	SeikoMgr_DecomposeSeiko_Add,
	StageTowerMgr_Attack_Add,
	Activity_SeikoShop_Add,
	AwardCenter_Recharge_Gift,
	TuanGou_BuyItem_Opt,
	TuanGou_GiftToUser,		
	TuanGou_JoinCashBack,
	SeiKo_Exchange_Add,		//精工兑换增加
	NewWelfare_Add,   // 新手福利
	RechargeBackGift_Add,	//充值按档位返礼活动（funcswitch.txt表）
	GlobalGuildBattle_RankAdd,
	GlobalGuildPVP_Prostrate,
	WeChatShare_Add, //微信分享
};

enum PLAYER_COST_MONEY_OPTYPE
{
	Player_CostMoney_Cost,
	BuildMgr_cashSupply_Cost,
	BuildMgr_buyOil_Cost,
	PaperMgr_canSpyPaper_Cost,
	PartsMgr_DecomposeParts_Cost,
	ShipMgr_CanTrain_Cost,
	ShopMgr_BuyShop_Cost,
	StageMgr_resetStageCount_Cost,
	GuildMgr_Donate_Cost,
	GuildMgr_CreateGuild_Cost,
	BuildMgr_levelUp_Cost,
	PartsMgr_UpgradeParts_Cost,
	ScienceMgr_CanLevelUp_Cost,
	ShipMgr_CanCreateShip_Cost,
	ShipMgr_RemouldShip_Cost,
	ShipMgr_UpgradeShip_Cost,
	PartsMgr_RemouldParts_Cost,
	AdvStageMgr_Attack_Cost,
	GuardMgr_EnterGuard_Cost,
	StageMgr_canAttack_Cost,
	TreasureStageMgr_Attack_Cost,
	GuildMgr_ShopBuy_Cost,
	GuildMgr_SetMedal_Cost,
	ActivityMgr_GrowUpBuy_Cost,
	PartsMgr_CheckCanRobPartPiece_Cost,
	MedalMgr_GetMedal_Cost,
	GHOSTSHIP_COST,
	PaperMgr_SpyLimitShip_Cost,
	GuardMgr_BuyChance_Cost,
	GuardMgr_BuyResetCount_Cost,
	GuardMgr_BuySweepCD_Cost,
	SoulMgr_RandSoul_Cost,
	SoulMgr_ChooseScene_Cost,
	SoulMgr_RandAdvSoul_Cost,
	ShipMgr_Retire_Cost,
	CampBattleMgr_ClearCD_Cost,
	ActivityMgr_MisteryShop_Cost,
	PaperMgr_ChargePaperByWanNeng_Cost,
	ActivityMgr_MisteryShopRefresh_Cost,
	LUCKYGODDESS_COST,
	MilitaryRank_Upgrad_Cost,
    CHRISTMAS_LOTTERY,
    LuckyCatMgr_Cost,
	Reduction_Parts_Cost,
    Valentine_Cost,
    OnearmBandit_Cost,
	ReName_Role_Cost,
	ReName_Ship_Cost,
	ShipAdvanced_Cost,
	CampBattle_Detect_Cost,
	CampBattle_Blood_Cost,
	CenterCampBattle_Detect_Cost,
	CenterCampBattle_Blood_Cost,
	ShipFormation_LevelUP_Cost,
    Challenge_Worship_Cost,
    Challenge_Buy_Cost,
    Challenge_Refresh_Cost,
    Challenge_Action_Cost,
    Challenge_Revenge_Cost,
    Challenge_Worship_Duluxe_Cost,    //=60
	UpgradeCaptain_Cost,
	RemouldCaptain_Cost,
	CaptainRetire_cost,
	CaptainRecruitJunior_Cost,
	CaptainRecruitSenior_Cost,
	SpyLimitCaptain_Cost,
	UPGRADECAPTAIN_COST,
	EXCHANGECAPTAIN_COST,
	LIMITSHIP_BUY_COST,
    Promote_Exploit_Cost,
    Restore_Cash_Cost,
	GuildBattle_ClearCD_Cost,
	GuildBattle_Blood_Cost,
	Change_Role_Cost,
	OccupyIsland_Oil_Cost,
	GoldIsland_DelayTime_Cash_Cost,
	Challenge_Armada_Cost,
	PlotCopy_Roll_Cost,
	PlotCopy_SkipBattle_Cost,
	Talent_Activate_Cost,
	Talent_Upgrade_cost,
	GlobalArena_UpdateTacCost,
	GlobalArena_EncourageCost,
	GlobalArena_ProstrateCost,
	GlodLeader_ExploeCost,
	SeikoMgr_UpgradeSeiko_Cost,
	SeikoMgr_RemouldSeiko_Cost,
	SeikoMgr_Forge_Cost,
	Reduction_Seiko_Cost,
	Undo_Recharge_Cost,
	StageTowerMgr_BuyChallengeTimes_Cost,
	StageTowerMgr_Puschase_Cost,
	ActivityMgr_SeikoShopRefresh_Cost,
	ActivityMgr_SeikoShop_Cost,
	GM_Cost,
	Player_RenameGuild_Cost,
	ChatMgr_Chat_Cost,
	JoinTuanGou_Cost,			//团购报名
	JoinTuanGou_BuyItemCost,	//团购购买
	GlobalGuildBattleUpdateCost,
	GlobalGuildBattleCheerCost,
	PROMOTEREDCAPTAINCOST,		//改造红舰长消耗
};

enum PLAYER_ADD_PROP_OPTYPE
{
	PROP_ARENA_ADD,			//竞技场交换物品
	PROP_MAIL_ADD,			//邮件获得物品
	PROP_DROP_ADD,			//掉落获得物品
	PROP_RETIRESHIP_ADD,	//船只退役获得物品
	PROP_SHOPBUY_ADD,		//商店购买物品
	PROP_GM_ADD,			//GM添加物品
	PROP_ACTIVITY_SIGNIN_ADD,	//签到活动
	PROP_ACTIVITY_VIP_ADD,	//VIP豪礼活动
	PROP_ACTIVITY_LEVEL_ADD,//等级礼包活动
	PROP_INITDATA_ADD,		//初始给予
	PROP_GUILDSHOP_ADD,		//军团商店购买
	PROP_ACTIVITY_SEVENDAYS_ADD,  //十一七天乐
	PROP_ACTIVITY_COST_REWARD_ADD,//消费好礼活动
	PROP_FREE_RECRUIT_ADD,//免费招募
	PROP_CHARGE_RECRUIT_ADD,//付费招募
	PROP_BUYSPYITEM,
	PROP_FREE_LIMITCAPTAIN_ADD,
	PROP_CHARGE_LIMITCAPTAIN_ADD,
	PROP_FREE_GHOSTSHIP_ADD,
	PROP_CHARGE_GHOSTSHIP_ADD,
	PROP_LIMITSHIP_FREE_ADD,
	PROP_LIMITSHIP_CHARGE_ADD,
	PROP_GOLDLEADER_FREE_ADD,
	PROP_GOLDLEADER_CHARGE_ADD,
	PROP_GOLDLEADER_EXPLORE_ADD,
	PROP_LUCKYGOD_FREE_ADD, 
	PROP_LUCKYGOD_CHARGE_ADD,
	PROP_SEIKO_REDUCTION_ADD,
	PROP_SEIKO_DECOMPOSE_ADD,
};

enum PLAYER_COST_PROP_OPTYPE
{
	PROP_SELL_COST,			//出售物品
	PROP_USE_COST,			//使用物品
	PROP_SPYUSE_COST,		//派遣间谍消耗物品
	PROP_UPGRADESCIENCE_COST,//科技升级消耗物品
	PROP_TRAINSHIP_COST,	//训练船只消耗物品
	PROP_UPGRADESHIPSKILL_COST,	//升级船只技能消耗物品
	PROP_RETIREHIP_COST,	//船只退役消耗物品
	PROP_RENAMESHIP_COST,	//重命名船只消耗物品
	PROP_GM_COST,			//GM删除物品
	PROP_STAGE_SWEEP_COST,	//关卡扫荡
	PROP_RENAMEROLE_COST,	//重命名角色消耗物品
	PROP_SOULCHOOSESCENE_COST,//选择战魂场景
	PROP_MISTERYSHOP_REFRESH_COST,	//神秘商店刷新
	PROP_MILITARYRANK_ORDER_COST,//升级军衔消耗
	PROP_USEPIECEGIFT_COST,//使用碎片礼包消耗
	PROP_SHIPADVANCED_COST,		//战舰进阶消耗
	PROP_SHIPFORMATIONLEVELUP_COST,//阵型升级
    PROP_CHALLENGE_COST,
	PROP_RECRUITCAPTIAN_COST,//招募舰长消耗
	PROP_USESUPERCAPTAINGIFT_COST,//超级舰长碎片包
	PROP_LIMITCAPTAIN_COST,
	PROP_GHOSTSHIP_COST,
	PROP_LIMITSHIPPROP_COST,//
	PROP_CHANGEROLE_COST,//使用头像更换卡
	PROP_ACTIVATEORUPDATETALENT,
	PROP_GOLDLEADER_EXPLOE_COST,
	PROP_LUCKYGOD_FREE_COST, 
	PROP_LUCKYGOD_CHARGE_COST,
	PROP_SEIKO_UPGRADE_COST,
	PROP_SEIKO_FORGE_COST, //精工洗练消耗
	PROP_SEIKOSHOP_REFRESH_COST,	//精工商店刷新
	PROP_RENAME_GUILD_COST,
	PROP_SOULGIFT_COST,
	PROP_PROMOTEREDCAPTAIN_COST,
};

enum PLAYER_ADD_PARTS_OPTYPE
{
	PARTS_DROP_ADD,			//掉落零件
	PARTS_GM_ADD,			//GM添加零件
	PARTS_INITDATA_ADD,		//初始给予
	PARTS_COMPOSE_ADD,
	PARTS_REDUCTION_ADD,
};

enum PLAYER_COST_PARTS_OPTYPE
{
	PARTS_DECOMPOSE_COST,	//分解零件
	PARTS_GM_COST,			//GM删除零件
	PARTS_REDUCTION_COST,//还原
};

enum PLAYER_ADD_PARTPIECE_OPTYPE
{
	PARTPIECE_DROP_ADD,		//掉落
	PARTPIECE_GM_ADD,
	PARTPIECE_GUIDE_ADD,
};

enum PLAYER_COST_PARTPIECE_OPTYPE
{
	PARTPIECE_DELETEALL_COST, //
	PARTPIECE_GM_COST,
	PARTPIECE_ROBBED_COST,
};

enum PLAYER_ADD_PAPER_OPTYPE
{
	PAPER_SPY_ADD,				//派遣间谍获得图纸
	PAPER_CHARGE_ADD,			//交换获得图纸
	PAPER_CHARGE_WANNENG_ADD,	//使用万能图纸交换获得图纸
	PAPER_DROP_ADD,				//掉落获得图纸
	PAPER_GM_ADD,				//GM添加图纸
	PAPER_INITDATA_ADD,			//初始给予
	PAPER_RECHARGE_REWARD_ADD,	//充值送图纸
};

enum PLAYER_COST_PAPER_OPTYPE
{
	PAPER_SELL_COST,			//出售图纸
	PAPER_CREATESHIP_COST,		//造船消耗图纸
	PAPER_REMOULDSHIP_COST,		//改造消耗图纸
	PAPER_GM_COST,				//GM删除图纸
};

enum PLAYER_ADD_PAPERPIECE_OPTYPE
{
	PAPERPIECE_DROP_ADD,		//掉落获得图纸碎片
	PAPERPIECE_GM_ADD,			//GM添加图纸碎片
};

enum PLAYER_COST_PAPERPIECE_OPTYPE
{
	PAPERPIECE_CHARGE_COST,		//碎片兑换成图纸
	PAPERPIECE_GM_COST,			//GM删除图纸碎片
};

enum PLAYER_ADD_SHIP_OPTYPE
{
	SHIP_CREATE_ADD,			//创建船只
	SHIP_INIT_ADD,				//初始化玩家数值时送的起始船只
	SHIP_GM_ADD,				//GM添加船只
};

enum PLAYER_COST_SHIP_OPTYPE
{
	SHIP_RETIRE_COST,			//船只退役
	SHIP_GM_COST,				//GM删除船只
};

enum PLAYER_ADD_SOUL_OPTYPE
{
	SOUL_BUYSCENE_ADD,			//购买战魂场景
	SOULMGR_RANDSOUL_ADD,		//购买战魂
	SOUL_GM_ADD,				//GM添加战魂
	SOUL_DROP_ADD,				//掉落战魂
	SOULMGR_RAND50SOUL_ADD,		//购买50次战魂
	SOULMGR_RAND50ADVSOUL_ADD,	//高级购买50次战魂
    SOULMGR_DECOMPOSE_ADD,   //分解
    SOULMGR_RESOTRE_ADD,  //还原
	SOUL_COMPOSEPIECE_ADD,
};

enum PLAYER_COST_SOUL_OPTYPE
{
	SOUL_EAT_COST,				//战魂吞噬
	SOUL_GM_COST,				//GM删除战魂
    SOUL_DECOMPOSE,        //分解
};

enum PLAYER_ADD_MAIL_OPTYPE
{
	MAIL_QUIZAWARD_ADD,
	MAIL_GM_ADD,
	MAIL_LOGINGIFT_ADD,
	MAIL_ARENA_ADD,
	MAIL_FUNCSWITCH_ADD,
	MAIL_GUILD_DISMISS_ADD,
	MAIL_GUILD_EXPEL_ADD,
	MAIL_ROBBERY_ADD,
	MAIL_RICHESTAWARD_ADD,
	MAIL_GHOSTSHIP_ADD,
	MAIL_RENAME_ADD,
	MAIL_LIMITSHIP_ADD,
	MAIL_LIMITSHIP_60AWRAD_ADD,
	MAIL_SETROLE_ADD,
	MAIL_PLAYER_SEND_ADD,
	MAIL_GUILD_AUTOCHANGECHIEF,
	MAIL_GOLDISLAND_INVITEHELP,
	MAIL_GOLDISLAND_APPLYHELP,
	MAIL_GOLDISLAND_BEATED,
	MAIL_VIP12_ADD,
	MAIL_VALENTINE_ADD,
	MAIL_LIMITCAPTION_ADD,
	MAIL_ONEARMEBANDIT_ADD,
	MAIL_GOLDISLAND_OCCUPY,	//金银岛被占
};

enum PLAYER_DEL_MAIL_OPTYPE
{
	MAIL_GM_DEL,
	MAIL_POPFRONTMAIL_DEL,
};

enum SHOP_CLASS
{
	SHOP_CLASS_NORMAL = 1,
	SHOP_CLASS_VIP,
	SHOP_CLASS_GUILD,
	SHOP_CLASS_CAMPBATTLE,
};

enum PLAYER_ADD_JUNIORSTAR_OPTYPE
{
	StageMgr_addStageCount_AddJuniorStar,
	GM_AddJuniorStar,
};

enum PLAYER_COST_JUNIORSTAR_OPTYPE
{
	MedalMgr_GetMedal_CostJuniorStar,
};

enum PLAYER_ADD_SENIORSTAR_OPTYPE
{
	AdvStageMgr_addStageCount_AddSeniorStar,
	GM_AddSeniorStar,
};

enum PLAYER_COST_SENIORSTAR_OPTYPE
{
	MedalMgr_GetMedal_CostSeniorStar,
};

enum CountryID
{
	COUNTRY_US = 1,
	COUNTRY_DE,
	COUNTRY_UK,
	COUNTRY_JP,
	COUNTRY_FR,
	COUNTRY_IT,
	COUNTRY_SU,
	COUNTRY_MAX,
};

//科技升级每级提升的属性数值
#define SCIENCE_IRON_ADD 30
#define SCIENCE_AMMO_ADD 10
#define SCIENCE_AIR_ADD 10
#define SCIENCE_MACHINE_ADD 4
#define SCIENCE_ELEC_ADD 4

//充值活动ID
#define RECHARGE_FUNCSWITCH_ID (-1)

//广播类型
#define BROADCAST_TYPE_SYSTEM 1		//后台发送
#define BROADCAST_TYPE_TRIGGER 2	//触发

//限时征战天数
#define LIMIT_FIGHT_DAY_COUNT 7		
//限时征战领奖天数
#define LIMIT_FIGHT_REWARD_DAY_COUNT 9

//阵营数量
#define CAMP_TYPE_MAX 3

#define  MILITARYRANK_TYPE_BASE 10

enum CAMP_TYPE
{
	CAMP_TYPE_NONE,
	CAMP_TYPE_JUSTICE,
	CAMP_TYPE_PREDATOR,
	CAMP_TYPE_HONOUR,
};

enum LUCKY_BOX_TYPE
{
	LUCKY_BOX_TYPE_FREE = 1,//免费
	LUCKY_BOX_TYPE_CHARGE = 2,//付费
};

typedef enum
{
    LUCKY_GODDNESS_ERROR_INVAILED = -1,
    LUCKY_GODDNESS_ERROR_COMMON_SUCCESS,
    LUCKY_GODDNESS_ERROR_NOT_CASH,
    LUCKY_GODDNESS_ERROR_NOT_OPEN,
    LUCKY_GODDNESS_ERROR_UNKNOW,
    LUCKY_GODDNESS_ERROR_MAX
} LuckyGoddnessError;

typedef enum
{
    LUCKY_GODDNESS_TYPE_INVAILED = -1,
    LUCKY_GODDNESS_TYPE_COMMON, //普通
    LUCKY_GODDNESS_TYPE_DELUXE, //十连抽
    LUCKY_GODDNESS_TYPE_MAX,
} LuckyGoddnessType;

enum PLAYER_ADD_CAPTAIN_OPTYPE
{
	CAPTAIN_GM_Add = 0,
	CAPTAIN_RECRUIT_ADD = 1,//招募获得
	CAPTAIN_COMPOSE_ADD = 2,
	CAPTAIN_DROP_ADD = 3,
	CAPTAIN_EXCHANGE_ADD = 4,
};

enum PLAYER_COST_CAPTAIN_OPTYPE
{
	CAPTAIN_GM_DEL = 1,
	CAPTAIN_FIRE_DEL = 2,//解雇
};
enum PLAYER_ADD_CAPTAINPIECE_OPTYPE
{
	CAPTAINPIECE_GM_Add = 0,
	CAPTAINPIECE_AUTOTURN_ADD = 1,//获取同样舰长转换
	CAPTAINPIECE_DROP_ADD,
};

enum PLAYER_COST_CAPTAINPIECE_OPTYPE
{
	CAPTAINPIECE_GM_COST = 0,
	CAPTAINPROMOTE_COST = 1,//舰长晋升
	CAPTAIN_COMPOSE,		//合成舰长
	CAPTAIN_RETIRE,			//解雇舰长删除相关碎片
};

enum PLAYER_ADD_SOULPIECE_OPTYPE
{
	SOULPIECE_GM_ADD = 0,
	SOULPIECE_DROP_ADD,
};

enum PLAYER_COST_SOULPIECE_OPTYPE
{
	SOULPIECE_GM_COST = 0,
	SOULPIECE_COMPOSE_COST,
	SOULPIECE_BEROB_COST,
};

enum PLAYER_ADD_SEIKO_OPTYPE
{
	SEIKO_DROP_ADD,			//掉落零件
	SEIKO_GM_ADD,			//GM添加零件
	SEIKO_INITDATA_ADD,		//初始给予
	SEIKO_COMPOSE_ADD,
	SEIKO_REDUCTION_ADD,
	SEIKO_DECOMPOSE_ADD,
};

enum PLAYER_COST_SEIKO_OPTYPE
{
	SEIKO_DECOMPOSE_COST,	//分解零件
	SEIKO_GM_COST,			//GM删除零件
	SEIKO_REDUCTION_COST,	//还原
	SEIKO_REMOULD_COST,	//改造
};

enum PLAYER_ADD_SEIKOPIECE_OPTYPE
{
	SEIKOPIECE_DROP_ADD,	//掉落
	SEIKOPIECE_GM_ADD,
};

enum PLAYER_COST_SEIKOPIECE_OPTYPE
{
	SEIKOPIECE_COMPOSE_COST, //
	SEIKOPIECE_GM_COST,
};

//解决winsock冲突
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
	#pragma warning(disable:4996)
#endif

#ifdef _WIN32
#define usleep Sleep
#endif

#endif
