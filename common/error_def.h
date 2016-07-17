#ifndef _ERROR_DEF_H_
#define _ERROR_DEF_H_


enum    ERROR_CODE
{
        SUCCESS = 0,                                    //成功
        ERR_UNKNOWN = 1,                                //未知
        ERR_CONFIG_MISS = 2,                            //未找到配置
        ERR_CONFIG_WRONG = 3,                           //配置有误
        ERR_WRONG_PARAMTER = 4,                         //错误的参数

        ERR_LOADUSER_FAILED = 101,                      //加载用户失败
        ERR_CREATEUSER_FAILED = 102,                    //创建用户失败
        ERR_ALREADY_REGIST = 103,                       //已注册
        ERR_OPENID_NOT_LOCAL = 104,                     //非当前分区

        ERR_PLAYER_LEVEL = 201,                         //玩家等级不足
        ERR_PLAYER_GEM = 202,                           //宝石不足
        ERR_PLAYER_NOTFOUND = 203,                      //玩家未找到

        ERR_CONSTRUCTION_NOT_FOUND = 1001,              //未找到建筑
        ERR_CONSTRUCTION_CASTLE_LEVEL_TO_LOW = 1002,    //城堡等级不足
        ERR_CONSTRUCTION_POS = 1003,                    //建造位置错误
        ERR_CONSTRUCTION_BUILD_TYPE = 1004,             //建造类型错误
        ERR_CONSTRUCTION_TOO_MANY = 1005,               //建筑太多
        ERR_DEPOT_TOO_MANY = 1006,                      //太多仓库
        ERR_TAVERN_LEVEL = 1007,                        //酒馆科级不足

        ERR_RESOURCE_NOT_ENOUGH = 2001,                 //资源不足
        ERR_GOLD_NOT_ENOUGH = 2002,                     //金币不足
        ERR_ITEM_NOT_ENOUGH = 2003,                     //物品不足

        ERR_ACTION_LIST_FULL = 3001,                    //动作队列满
        ERR_ACTION_HAS_SAME_TARGET = 3002,              //已有相同目标的动作
        ERR_WAITACTION_LIST_FULL = 3003,                //等待队列满
        ERR_ACTION_NOT_FOUND = 3004,                    //动作未找到
        ERR_ACTION_NOT_FULL = 3005,                     //动作队列未满
        ERR_ACTION_NOT_COUNTING = 3006,                 //动作未激活
        ERR_ACTION_CANNOT_SPEED = 3007,                 //无法加速

        ERR_ACADEMY_LEVEL = 4001,                       //大学等级不足

        ERR_BARRACK_LEVEL = 5001,                       //兵营等级不足
        ERR_SOILDER_NOT_HURT = 5002,                    //士兵未受伤
        ERR_SOILDER_NOT_ENOUGH = 5003,                  //士兵不足
        ERR_FORGE_LEVEL = 5004,                         //铁匠铺

        ERR_GENERAL_NOT_WAIT = 6001,                    //将领不处于等待召唤状态
        ERR_GENERAL_NOT_FOUND = 6002,                   //将领未找到
        ERR_GENERAL_NOT_IDLE = 6003,                    //将领不空闲
        ERR_GENERAL_CMD_SOILDER_NUM = 6004,             //将领带兵数量不足
        ERR_GENERAL_REFRESH_TIME = 6005,                //刷新时间未到
        ERR_GENERAL_TOO_MANY = 6006,                    //将领数量太多

        ERR_BATTLE_GRID_NOT_FOUND = 7001,               //战场未找到
        ERR_BATTLE_GRID_STATE = 7002,                   //战场状态错误
        ERR_BATTLE_GRID_SELF_CITY = 7003,               //战场被自己占领
        ERR_ATTACK_IMUNITY = 7004,                      //开启了免战
        ERR_MARCH_NOT_FOUND = 7005,                     //派兵未找到
        ERR_MARCH_STATE = 7006,                         //派兵状态不对
        ERR_MARCH_ALREADY_DEFEND = 7007,                //已经防守
        ERR_MARCH_CANNOT_DETECT = 7008,                 //无法侦查

        ERR_PROP_NOT_ENOUGH = 8001,                     //道具不足
        ERR_PROP_CANNOT_DIRECT_USE = 8002,              //道具不能直接使用
        ERR_PROP_NOT_IN_SHOP = 8003,                    //道具不在商店中出售
        ERR_PROP_EFFECT = 8004,                         //道具使用效果错误
        ERR_PROP_ALREADY_AWARD = 8005,                  //当日已经嘉奖

        ERR_UNION_ALREADY_APPLY = 9001,                 //已申请
        ERR_UNION_ALREADY_MEMBER = 9002,                //已经是成员
        ERR_UNION_AUTHORITY = 9003,                     //权限不够
        ERR_UNION_NOT_APPLY = 9004,                     //未申请的用户
        ERR_UNION_MEMBER_NOT_FOUND = 9005,              //成员未找到
        ERR_UNION_APPLY_LENGTH = 9006,                  //申请队列满
        ERR_UNION_NAME_LEN = 9007,                      //名字过长
        ERR_UNION_APPLY_NUM = 9008,                     //同时申请联盟过多
        ERR_UNION_NOT_FOUND = 9009,                     //联盟未找到
        ERR_UNION_LEAVE_CD = 9010,                      //申请冷却时间未到
        ERR_UNION_HALL_LEVEL = 9011,                    //大厅等级不足
        ERR_UNION_CONSTRUCTION_NOT_FOUND = 9012,        //建筑未找到
        ERR_UNION_PROP_LOCK = 9013,                     //道具被锁定
        ERR_UNION_SOILDER_LOCK = 9014,                  //士兵被锁定
        ERR_UNION_CONTRIBUTE_NOT_ENOUGH = 9015,         //贡献度不足
        ERR_UNION_MEMBER_NUM = 9016,                    //人数超过上限
        ERR_UNION_CONTRIBUTE_NUM = 9017,                //捐献次数达到上限
        ERR_UNION_HELP_NOT_FOUND = 9018,                //未找到帮助请求
        ERR_UNION_ALREADY_HELP = 9019,                  //已经帮助过了
        ERR_UNION_HELP_SEND_NUM = 9020,                 //申请帮助次数超过限额
        ERR_UNION_NOTICE_LEN = 9021,                    //联盟通知过长

        ERR_AFFAIR_NOT_WAIT = 10001,                    //日常任务没有被刷出
        ERR_AFFAIR_TIME_NOT_ENOUGH = 10002,             //日常任务时间未到
        ERR_AFFAIR_REWARD_NUM = 10003,                  //领取日常任务奖励次数满
        ERR_AFFAIR_REFRESH_TIME = 10004,                //刷新时间未到
        ERR_MISSION_NOT_ACCEPT = 10005,                 //任务未接受
        ERR_MISSION_NOT_COMPLETE = 10006,               //任务完成条件未达成

        ERR_DRAMA_CHAPTER_NOT_FOUND = 11001,            //章节未找到
        ERR_DRAMA_CHAPTER_ALREADY_REWARD = 11002,       //章节奖励已领取
        ERR_DRAMA_CHAPTER_NOT_FINISH = 11003,           //章节未完成
        ERR_DRAMA_CHAPTER_LOCK = 11004,                 //章节锁定
        ERR_DRAMA_FIGHTPOINT_LOCK = 11005,              //战斗点锁定

        ERR_CHAT_CD = 12001,                            //聊天CD
        ERR_CHAT_USER_OFFLINE = 12002,                  //用户不在线

        ERR_LOTTERY_ALREADY_TAKE = 13001,               //奖励已领取
        ERR_LOTTERY_TAKE_NUM = 13002,                   //领取次数过多

        ERR_ENERGY_NOT_ENOUGH = 14001,                   //能量不足
        ERR_ENERGY_FULL = 14002,                         //能量已满
        ERR_ENERGY_FILL_TOO_MUCH = 14003,                //能量补满次数上限
};

#endif

