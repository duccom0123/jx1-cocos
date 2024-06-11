/*****************************************************************************************
//	外界访问Core用到数据结构等的定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
	一些定义可能处于在游戏世界各模块的头文件中，请在此处包含那个头文件，并请那样的头文件
不要包含一些与游戏世界对外无关的内容。
    开发过程中游戏世界的外部客户在未获得游戏世界接口完整定义的情况下，会现先直接在此文件
定义它需要的数据定义，游戏世界各模块可根据自身需要与设计把定义作修改或移动到模块内的对外
头文件，并在此建立充要的包含。
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"
#include "KRUImage.h"
#define		_CHAT_SCRIPT_OPEN                           // 聊天内容过滤开启

#define		MAX_TEAM_MEMBER						7		// 最大队员数量(不包括队长)
#define		MAX_SENTENCE_LENGTH					256		// 聊天每个语句最大长度

#define		FILE_NAME_LENGTH		            256		


#define		defMAX_PLAYER_SEND_MOVE_FRAME		5

//#define		PLAYER_PICKUP_CLIENT_DISTANCE		20000
//#define		MAX_INT								0x7fffffff

#define		ROLE_NO								2
#define		PLAYER_MALE_NPCTEMPLATEID			-1
#define		PLAYER_FEMALE_NPCTEMPLATEID			-2

#define		MAX_DEATH_PUNISH_PK_VALUE			10		// PK处罚，PK值从 0 到 10
#define		MAX_REPUTE_VALUE					100000	//声望
#define		MAX_FUYUAN_VALUE					100000	//福源

/*--- 用于标明客户端npc是哪个region的第几个npc ，
如果这是一个服务器控制的npc ，ID 值为 0 ，No 值为 -1  ---*/
struct	KClientNpcID
{
	DWORD	m_dwRegionID;
	int		m_nNo;
};

typedef struct
{
	int		nIdx;
	int		nPlace;
	int		nX;
	int		nY;
	int		nPrice;      //商店购买价格
	int     nIsRongQi;   //哪个容器里的东西
	BYTE    nLianJieFlg; //是否正在链接
	BYTE    nModel;
} PlayerItem;


#define		MAX_HAND_ITEM				1	  //手上

#define		EQUIPMENT_ROOM_WIDTH		5
#define		EQUIPMENT_ROOM_HEIGHT		6
#define		MAX_EQUIPMENT_ITEM			(EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)	   //包袱

#define		REPOSITORY_ROOM_WIDTH		5
#define		REPOSITORY_ROOM_HEIGHT		6
#define		MAX_REPOSITORY_ITEM			(REPOSITORY_ROOM_WIDTH * REPOSITORY_ROOM_HEIGHT*5)   //储物箱（包括3扩展箱 1子母袋）

#define		TRADE_ROOM_WIDTH			8
#define		TRADE_ROOM_HEIGHT			4
#define		MAX_TRADE_ITEM				(TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)	           //交易栏
#define		MAX_TRADE1_ITEM				MAX_TRADE_ITEM

#define		IMMEDIACY_ROOM_WIDTH		9     //快捷键数量限制 3
#define		IMMEDIACY_ROOM_HEIGHT		1
#define		MAX_IMMEDIACY_ITEM			(IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)	   //快捷栏

#define		DAZAO_ROOM_WIDTH		    14    //打造单对象容器
#define		DAZAO_ROOM_HEIGHT		    4
#define	    MAX_DAZAO_ITEM              (DAZAO_ROOM_WIDTH*DAZAO_ROOM_HEIGHT) 	           //打造栏

#define		CAILIAO_ROOM_WIDTH		    2     //材料多对象容器
#define		CAILIAO_ROOM_HEIGHT		    4
#define		MAX_CAILIAO_ITEM            (CAILIAO_ROOM_WIDTH*CAILIAO_ROOM_HEIGHT*5)         //材料栏DAZAO_ROOM_WIDTH*CAILIAO_ROOM_HEIGHT+

#define		MAX_PLAYER_ITEM_RESERVED	32
#define		MAX_ITEM_BOX                100	 //单对象容器（包括 同伴栏  和  打造装备栏）
#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED+MAX_ITEM_BOX+MAX_CAILIAO_ITEM)
 
// 复活模式
//#define		REMOTE_REVIVE_TYPE			0
//#define		LOCAL_REVIVE_TYPE			1



/*
#define		MAX_ARMOR					14
#define		MAX_HELM					14
#define		MAX_RING					1
#define		MAX_BELT					2
#define		MAX_PENDANT					2
#define		MAX_AMULET					2
#define		MAX_CUFF					2
#define		MAX_BOOT					4
#define		MAX_HORSE					6
*/
#define		MAX_NPC_TYPE	1000      //NPC种类限制
#define		MAX_NPC_LEVEL	300       //NPc等级限制

#define		MAX_NPC_DIR		64        //方向
//#define		MAX_WEAPON		MAX_MELEE_WEAPON + MAX_RANGE_WEAPON
#define		MAX_SKILL_STATE 20       //每个技能的属性数量的状态(最大为20个 决定于skill.txt)
//#define		MAX_NPC_HEIGHT	128
#define		MAX_RESIST		150        //2000  //最大防御值
#define		MAX_BASE_RESIST		75     //2000  //最大防御值
#define		BASE_NPC_RESIST_MAX 50     //NPC基本防御
//#define		MAX_HIT_PERCENT	2000   //最大防御值--闪避
#define		MIN_HIT_PERCENT	10         //最小防御值--闪避
#define		MAX_NPC_RECORDER_STATE 36  //最大同步给NPC最新的状态数量; 20

//#define	PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5

#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
#endif
//----------共用开始---------------------------------
#define MAX_MESSAGE_LENGTH          512
#define	ITEM_VERSION				1    //装备版本
#define	MAX_RANGE_WEAPON			3
#define	MAX_MELEE_WEAPON			6

enum GAME_STR_NOTE
{
	STR_NONE = 0,
	STR_ACTION_SCRIPT,    //脚本
	STR_NPCTALK_MSG,
	STR_NPCTALK_NAME,
	STR_WARTONG_NAME,	  //占领帮会
	STR_WARMASTER_NAME,	  //占领帮主
	STR_GUISHU_NAME,
	STR_CHEGNHAO_NAME,
	STR_SHOP_NAME,       //摆台的名字
	STR_PICTATH_CLIENT,	 //头像路径
	STR_SCRIPTTATH_CLIENT, //客户端脚本路径
	STR_MAP_CURTIME,   //聊天的内容
	STR_MAP_NEXTTIME,
	STR_MAP_NAME,
	STR_MAP_TGONGNAME,
	STR_MAP_TSHOUNAME,
	STR_MAP_MASTER,
	STR_MAP_ZTONGNAME,
	STR_ROLE_CHANGENAME, //记录改名的数据 不存档
	STR_CHECK_NAME_RET,  //记录查询结果
	STR_ITEM_DESCINFO,   //物品的说明文字
	STR_ITEM_WONNAME,    //归属名字
	STR_ITEM_NAME,
	//m_WarTongNamea  m_WarMaster	 m_GuishuName PicPath	  m_ScriptPicPath  m_SubWorldName
};
enum GAME_STATE_NOTE
{
	    STATE_NOTE_NONE = 0,
	    STATE_NOTE_POISON,      //毒
		STATE_NOTE_FREEZE,      //冰
		STATE_NOTE_BURN,	    //燃烧
		STATE_NOTE_CONFUSE,     //混乱
		STATE_NOTE_STUN,        //眩晕
		STATE_NOTE_LIFE,        //BU XUE
		STATE_NOTE_MANA,        //BU LAN
		STATE_NOTE_MEUN,	    //菜单
		STATE_NOTE_DRUNK,
		STATE_NOTE_HIDE,	    //隐藏状态
		STATE_NOTE_ZHUA,	    //抓捕状态
		STATE_NOTE_LOSE,
		STATE_NOTE_EXP,	        //经验状态
		STATE_NOTE_DOSCRIPT,    //执行脚本状态
		STATE_NOTE_RANDMOVE,	//群体随机移动状态
		STATE_NOTE_MAPUSEMODEL, //地图 使用类型状态
		STATE_NOTE_PHYSICSARMOR,
		STATE_NOTE_COLDARMOR,
		STATE_NOTE_LIGHTARMOR,
		STATE_NOTE_FIREARMOR,
		STATE_NOTE_MANASHIELD,	//被偷蓝状态
		STATE_NOTE_RETURNSKILL,	//被敌人攻击是 自动释放某技能几率
		STATE_NOTE_DEATHSKILL,	//被敌人攻击是 自动释放某技能几率
		STATE_NOTE_RESCUESKILL,	//救命自动释放技能
		STATE_NOTE_REPLYSKILL,	////自动反击技能
};

enum ITEM_PART
{
	itempart_head = 0,	// 头
		itempart_body,		// 身体
		itempart_belt,		// 腰带
		itempart_weapon,	// 武器
		itempart_foot,	    //靴子
		itempart_cuff,		//手镯或护腕
		itempart_amulet,    //项链
		itempart_ring1,	    //戒子
		itempart_ring2,		//戒子
		itempart_pendant,	//香囊或玉佩
		itempart_horse,	    //马
		itempart_mask,	    //面具
		itempart_pifeng,
		itempart_yinjian,
		itempart_shiping,
		itempart_num,
};

enum MONEYUNIT
{
	    moneyunit_money = 0,//银两
		moneyunit_moneya, //银两
		moneyunit_xu,	  //金币
		moneyunit_fuyuan, //富源
		moneyunit_repute, //声望
		moneyunit_expoint,//点卡
		moneyunit_jifen,  //积分
		moneyunit_num,
};

enum INVENTORY_ROOM
{
	room_equipment = 0,	// 装备栏-包袱
		room_repository,	// 贮物箱
		room_exbox1,		// 扩展箱1
		room_exbox2,		// 扩展箱2
		room_exbox3,		// 扩展箱3
		room_equipmentex,	// 同伴包袱
		room_trade,			// 交易栏
		room_tradeback,		// 交易过程中装备栏的备份
		room_trade1,		// 交易过程中对方的交易栏
		room_immediacy,		// 快捷物品
		room_give,          // 给予空间
		room_giveback,       
		room_dazao,         // 打造框
		room_cailiao,       // 材料框
		room_num,			// 空间数量
};

enum ITEM_POSITION
{
	pos_hand = 1,		// 手上
		pos_equip,			// 装备着的
		pos_equiproom,		// 道具栏  包袱
		pos_repositoryroom,	// 贮物箱
		pos_exbox1room,		// mo rong ruong 1  扩展箱
		pos_exbox2room,		// mo rong ruong 2
		pos_exbox3room,		// mo rong ruong 3
		pos_equiproomex,	// 同伴包袱
		pos_traderoom,		// 交易栏
		pos_trade1,			// 交易过程中对方的交易栏
		pos_immediacy,		// 快捷栏物品
		pos_give,           // 给予界面
		pos_dazao,          // 打造框界面
		pos_cailiao,        // 材料栏
		pos_xiangqi,        // 象棋栏
};
//主角身份地位等一些关键属性项
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//登级变化	nParam表示当前等级
		PBP_FACTION,	//门派		nParam表示门派属性，如果nParam为-1表示没有门派
		PBP_CLIQUE,		//帮派		nParam为非0值表示入了帮派，0值表示脱离了帮派
};

//新闻消息的类型定义
enum NEWS_MESSAGE_TYPE
{
	NEWSMESSAGE_NORMAL,			//一般消息，显示（一次）就消息消亡了
								//无时间参数
	NEWSMESSAGE_COUNTING,		//倒计（秒）数消息，计数到0时，就消息就消亡了。
								//时间参数中的数据结构中仅秒数据有效，倒计数以秒为单位。
	NEWSMESSAGE_TIMEEND,		//定时消息，定时到时，消息就消完了，否则每半分钟显示一次。
								
	NEWSMESSAGE_SHUIJI,         //发送随机消息  显示一次就消亡
								//时间参数表示消亡的指定时间。
};

enum	// 物件类型
{
	Obj_Kind_MapObj = 0,		    // 地图物件，主要用于地图动画
		Obj_Kind_Body,				// npc 的尸体
		Obj_Kind_Box,				// 宝箱
		Obj_Kind_Item,				// 掉在地上的物品
		Obj_Kind_Money,				// 掉在地上的钱
		Obj_Kind_LoopSound,			// 循环音效
		Obj_Kind_RandSound,			// 随机音效
		Obj_Kind_Light,				// 光源（3D模式中发光的东西）
		Obj_Kind_Door,				// 门类
		Obj_Kind_Trap,				// 陷阱
		Obj_Kind_Prop,				// 小道具，可重生
		Obj_Kind_Num,				// 物件的种类数
};

struct KMapPos
{
	int		nSubWorld;
	int		nRegion;
	int		nMapX;
	int		nMapY;
	int		nOffX;
	int		nOffY;
};

// 注意：此枚举不允许更改(by zroc)
enum OBJ_ATTRIBYTE_TYPE
{
	    series_metal = 0,		//	金系
		series_wood,			//	木系
		series_water,			//	水系
		series_fire,			//	火系
		series_earth,			//	土系
		series_num,
};

enum NPCCAMP
{
	    camp_begin,				// 新手阵营（加入门派前的玩家）白
		camp_justice,			// 正派阵营  1	 灰色
		camp_evil,				// 邪派阵营  2   浅紫色 
		camp_balance,			// 中立阵营  3	 浅绿色
		camp_free,				// 杀手阵营（出师后的玩家）4  红色	
		camp_animal,			// 野兽阵营   5	 深紫色
		camp_event,				// 路人阵营   6	 深紫色
		camp_blue,				// 蓝
		camp_green,				// 绿
		camp_num,				// 阵营数
};
enum NPC_RELATION
{
	    relation_none	= 1,
		relation_self	= 2,   // 自己
		relation_ally	= 4,   // 同盟
		relation_enemy	= 8,   // 敌人
		relation_dialog	= 16,  // 中立对话
		relation_all	= relation_none | relation_ally | relation_enemy | relation_self | relation_dialog,	
		relation_num,
};
enum NPCKIND
{
	    kind_normal = 0, //动物		
		kind_player,     //玩家
		kind_partner,    //同伴
		kind_dialoger,	 //对话者
		kind_bird,       //天上飞的
		kind_mouse,      //地上走的
		kind_num
};

enum COLORKIND
{
	kind_color_physics = 0, //白色	
	kind_color_freeze,     //冰
	kind_color_stun,       //晕
	kind_color_poison,	   //毒
	kind_color_burn,       //燃烧
	kind_color_confuse,    //混乱
	kind_color_num
};

//==================================
//	魔法属性
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
	int				nAttribType;					//属性类型
	int				nValue[3];						//属性参数
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};
#else
struct KMagicAttrib;
#endif

#define MAX_MISSION_PARAM			10
#define MAX_GLBMISSION_PARAM		3
#define MISSION_STATNUM				10
#define MISSION_PARAM_AVAILABLE		0
#define MISSION_AVAILABLE_VALUE		1

#define	defTONG_NAME_MAX_LENGTH		12//8
#define	defTONG_STR_LENGTH		    32



//-----------------------------
struct TMissionLadderSelfInfo
{
	char		  szMissionName[32];	 //64
	unsigned char ucGroup;				 // 2
	int			  nGlbParam[MAX_GLBMISSION_PARAM]; //6
	int			  nParam[MAX_MISSION_PARAM];   // 32
	void Clear() {
		memset(szMissionName, 0, sizeof(szMissionName));
		memset(nGlbParam, 0, sizeof(nGlbParam));
		memset(nParam, 0, sizeof(nParam));
	};
};

struct TMissionLadderInfo
{
	unsigned char ucGroup;	 //2
	char		  Name[32];		//32
	int			  nParam[MAX_MISSION_PARAM]; //32
	/*void Clear() {
		ucGroup = 0;
		memset(Name, 0, sizeof(Name));
		memset(nParam, 0, sizeof(nParam));
	};*/
};
typedef struct
{
	
	int		m_nBuyIdx;
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	//int		m_nShopIdx[MAX_SUPERSHOP_SHOPTAB];
	//int		m_nShopNum;
	int		m_nPriceType;
	
	void	Clear() {
		//memset(m_nShopIdx, -1, sizeof(m_nShopIdx)); 
		m_nBuyIdx = -1 ; m_nPriceType = moneyunit_money; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;
	}
} BuySellInfo;

struct KUiMsgParam
{
	unsigned char	eGenre;	//取值范围为枚举类型MSG_GENRE_LIST,见MsgGenreDef.h文件
	unsigned char	cChatPrefixLen;
	unsigned short	nMsgLength;
	char			szName[32];  //聊天者的姓名
#define	CHAT_MSG_PREFIX_MAX_LEN	16
	unsigned char	cChatPrefix[CHAT_MSG_PREFIX_MAX_LEN];
};
struct KTime
{
	int bYear;
	int bMonth;
	int bDay;
	int bHour;
	int bMin;
};
struct KviewTime
{
	int bYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bHour;
	BYTE bMin;
};


//---------------------------- 帮会相关 ------------------------

#define		defTONG_MAX_DIRECTOR				7   //长老数
#define		defTONG_MAX_MANAGER					7   //队长数
#define		defTONG_MAX_LENMENG					10
#define		defTONG_ONE_PAGE_MAX_NUM			7   //每次要求获取同步最大数
enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// 帮众
		enumTONG_FIGURE_MANAGER,			// 队长
		enumTONG_FIGURE_DIRECTOR,			// 长老
		enumTONG_FIGURE_MASTER,				// 帮主
		enumTONG_FIGURE_LENMENG,            // 联盟
		enumTONG_FIGURE_LIST,               // 列表
		enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// 申请查询自身信息
		enumTONG_APPLY_INFO_ID_MASTER,		// 申请查询帮主信息
		enumTONG_APPLY_INFO_ID_DIRECTOR,	// 申请查询长老信息
		enumTONG_APPLY_INFO_ID_MANAGER,		// 申请查询队长信息
		enumTONG_APPLY_INFO_ID_MEMBER,		// 申请查询帮众信息(一批帮众)
		enumTONG_APPLY_INFO_ID_ONE,			// 申请查询某帮会成员信息(一个帮众)
		enumTONG_APPLY_INFO_ID_TONG_HEAD,	// 申请查询某帮会信息，用于申请加入帮会
		enumTONG_APPLY_INFO_ID_LIST,        // 申请获取帮会列表
		enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1,	// Player[m_nPlayerIndex].m_nIndex <= 0
		enumTONG_CREATE_ERROR_ID2,	// 交易过程中
		enumTONG_CREATE_ERROR_ID3,	// 帮会名问题
		enumTONG_CREATE_ERROR_ID4,	// 帮会阵营问题
		enumTONG_CREATE_ERROR_ID5,	// 已经是帮会成员
		enumTONG_CREATE_ERROR_ID6,	// 自己的阵营问
		enumTONG_CREATE_ERROR_ID8,	// 钱问题题
		enumTONG_CREATE_ERROR_ID7,	// 等级问题	
		enumTONG_CREATE_ERROR_ID9,	// 组队不能建帮会
		enumTONG_CREATE_ERROR_ID10,	// 帮会模块出错
		enumTONG_CREATE_ERROR_ID11,	// 名字字符串出错
		enumTONG_CREATE_ERROR_ID12,	// 名字字符串过长
		enumTONG_CREATE_ERROR_ID13,	// 帮会同名错误
		enumTONG_CREATE_ERROR_ID14,	// 帮会产生失败
};

//-------- 共用结束------------------------------------------
//---------测试服务器端开始----------------------------------
#ifdef _SERVER
#define	    MAX_SUPERSHOP_SHOPTAB 10
#define		PLAYER_PICKUP_SERVER_DISTANCE		40000
#define		PLAYER_SHARE_EXP_DISTANCE			768
#define		defMAX_EXEC_OBJ_SCRIPT_DISTANCE		200     //物品执行脚本距离
#define		MAX_ITEMLIST_CONUT		            250	    //最大储存的物品数量


enum ITEM_PARM_DROP
{
	    ITEM_PARM_COUNT,
		ITEM_PARM_RATESUM,			
		ITEM_PARM_LUCKRATESUM,
		ITEM_PARM_MAGRATE,
		ITEM_PARM_MAXRATE,
		ITEM_PARM_MONEYRATE,
		ITEM_PARM_MONEYNUM,
		ITEM_PARM_MONEYSCALE,
	    ITEM_PARM_MINLEVELSCALE,
		ITEM_PARM_MAXLEVELSCALE,
		ITEM_PARM_MINLEVEL,
		ITEM_PARM_MAXLEVEL,
		ITEM_PARM_TYPENUM,
		ITEM_PARM_BIANSHI,

		/*
		  INT	        m_nRandSum;		// 掉落率之和，100w分之一	（随机到超过此数值，则不会掉落）
		  int         nLuckRateSum;
		  int			nMagicRate;
		  int			nMaxRandRate;
		  int			nMoneyRate;
		  int         nMoneyNum;
		  int			nMoneyScale;
		  int			nMinItemLevelScale;   //最小等级概率
		  int			nMaxItemLevelScale;	  //
		  int		    nMinItemLevel;
		  int		    nMaxItemLevel;
		  int         nTypeNum;             //掉下的数量
		  int         nIsBianShi;           //是否可以爆 可辨识装备			  
		*/
};


#endif
//测试服务器端结束-------------------------------------------

//-------测试客户端开始-------------------------------------
#ifndef _SERVER

#define	NORMAL_NPC_PART_NO		6		           // 普通npc图像默认为只有一个部件，这是第几个
#define	PLAYER_PICKUP_CLIENT_DISTANCE		63     //捡东西的距离

struct ItemCounmd
{
	int Uid[16];
	int nMun;	
};
enum
{
	CHAT_S_STOP = 0,						// 非聊天状态
	CHAT_S_SCREEN,							// 与同屏幕玩家聊天
	CHAT_S_SINGLE,							// 与同服务器某玩家私聊
	CHAT_S_TEAM,							// 与队伍全体成员交谈
	CHAT_S_NUM,								// 聊天状态中类数
};

enum PLAYER_INSTANT_STATE
{
	enumINSTANT_STATE_LEVELUP = 0,
	enumINSTANT_STATE_REVIVE,
	enumINSTANT_STATE_CREATE_TEAM,
	enumINSTANT_STATE_LOGIN,
    enumINSTANT_STATE_DANGGE,
	enumINSTANT_STATE_NUM,
};

enum CHAT_STATUS
{
	CHAT_S_ONLINE = 0,		//在线
	CHAT_S_BUSY,			//忙碌
	CHAT_S_HIDE,			//隐身
	CHAT_S_LEAVE,			//离开
	CHAT_S_DISCONNECT,		//掉线
};



enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//雄性，男的
	OBJ_G_FEMALE,		//雌的，女的
};


enum ITEM_IN_ENVIRO_PROP
{
	IIEP_NORMAL = 0,	//一般/正常/可用
	IIEP_NOT_USEABLE,	//不可用/不可装配
	IIEP_SPECIAL,		//特定的不同情况
};

#define	GOD_MAX_OBJ_TITLE_LEN 1024	//128临时改为1024为了兼容旧代码 to be modified
#define	GOD_MAX_OBJ_PROP_LEN 512
#define	GOD_MAX_OBJ_DESC_LEN 512

//==================================
//	游戏对象的描述
//==================================
struct KGameObjDesc
{
	char szTitle[GOD_MAX_OBJ_TITLE_LEN];	//标题，名称
	char	 szProp[GOD_MAX_OBJ_PROP_LEN];	//属性，每行可以tab划分为靠左与靠右对齐两部分
	char	 szDesc[GOD_MAX_OBJ_DESC_LEN];	//描述
};


//==================================
//	问题与可选答案--选项
//==================================

struct KUiAnswer
{
	char  AnswerText[64];	//可选答案文字（可以包含控制符）
	int		AnswerLen;		//可选答案存储长度（包括控制符，不包含结束符）
};
struct KUiQuestionAndAnswer
{
	char		Question[512];	//问题文字（可以包含控制符） //主信息
	int			QuestionLen;	//问题文字存储长度（包括控制符，不包含结束符）
	int			AnswerCount;	//可选答案的数目
	KUiAnswer	Answer[1];		//候选答案
};

struct KUiNpcSpr 
{ 
    char                ImageFile[128]; 
    unsigned short         MaxFrame; 
};

//==================================
//	物品名字信息
//	Added by Fanghao_Wu 2004.8.30
//==================================
struct KUiItemNameInfo
{
	char	szItemName[64];			//物品名称
	DWORD	dwItemNameColor;		//物品名称颜色
};
//==================================
//	可以游戏对象容纳的地方
//==================================
enum UIOBJECT_CONTAINER
{
	UOC_IN_HAND	= 1,		//手中拿着
	UOC_GAMESPACE,			//游戏窗口
	UOC_IMMEDIA_ITEM,		//快捷物品
	UOC_IMMEDIA_SKILL,		//快捷武功0->右键武功，1,2...-> F1,F2...快捷武功
	UOC_ITEM_TAKE_WITH,		//随身携带
	UOC_TO_BE_TRADE,		//要被买卖，买卖面板上
	UOC_OTHER_TO_BE_TRADE,	//买卖面板上，别人要卖给自己的，
	UOC_EQUIPTMENT,			//身上装备
	UOC_NPC_SHOP,			//npc买卖场所
	UOC_MARKET,             //系统商城
	UOC_STORE_BOX,			//储物箱
	UOC_EX_BOX1,            //扩展箱1
	UOC_EX_BOX2,
	UOC_EX_BOX3,
	UOC_ITEM_EX,            //同伴包袱
	UOC_SKILL_LIST,			//列出全部拥有技能的窗口，技能窗口
	UOC_SKILL_TREE,			//左、右可用技能树
	UOC_ITEM_GIVE,          //给予空间
    UOC_ITEM_DAZAO,         //设定打造界面为容器类
	UOC_ITEM_CAILIAO,       //设定打造界面为容器类
	UOC_CHESSGAME,			//象棋
	UOC_NPC_SHOP_PAI,       //拍卖行

};

//==================================
// iCoreShell::GetGameData函数调用,uDataId取值为GDI_TRADE_DATA时，
// uParam的许可取值列表
// 注释中的Return:行表示相关的GetGameData调用的返回值的含义
//==================================
enum UI_TRADE_OPER_DATA
{
	UTOD_IS_WILLING,		//是否交易意向(叫卖中)
	//Return: 返回自己是否处于叫卖中的布尔值
	UTOD_IS_LOCKED,			//自己是否处于已锁定状态
	//Return: 返回自己是否处于已锁定状态的布尔值
	UTOD_IS_TRADING,		//是否可以正在等待交易操作（交易是否已确定）
	//Return: 返回是否正在等待交易操作（交易是否已确定）
	UTOD_IS_OTHER_LOCKED,	//对方是否已经处于锁定状态
	//Return: 返回对方是否已经处于锁定状态的布尔值
};

//==================================
//	买卖物品
//==================================
struct KUiItemBuySelInfo
{
	char			szItemName[64];	//物品名称
	int				nPrice;			//银两买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
	int				nPriceXu;       //金币
	BYTE            nPriceType;     //货币的类型

	int             nOldPrice;
	int             nOldPriceXu;
	int             nCurPriceXu;
	BOOL            bNewArrival;
//	int             nMoneyUnit;

};

//==================================
//	一些通用结构
//==================================
struct KUiTongYong
{
	int inKind;
	int inParma;
	int inParmb;
	int inParmc;
	int inParmd;
};
/*
struct	nJinDuClient
{
	int  nTimeVal;	   
	int	 nMsgidx;
	char szCallFun[32];   //回调函数
	char szParma[32];     //参数
	char szParmb[32];     //参数
	char szParmc[32];     //参数
}; 
*/
struct KUiScrptinfo
{
	struct	KTaskClient
	{
		char szContent[128];	//内容
		int	 nTaskidx;			//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
		int	 nTaskType;
		char  nTaskName[32];
	}; 
	KTaskClient nTaskInfo[20];
};
//==================================
//	简略表示游戏对象的结构
//==================================
struct KUiGameObject
{
	unsigned int uGenre;	//对象类属
	unsigned int uId;		//对象id
	//	int			 nData;		//与对象实例相关的某数据
};

//==================================
//	以坐标表示的一个区域范围
//==================================
struct KUiRegion
{
	int		h;		//左上角起点横坐标
	int		v;		//左上角起点纵坐标
	int		Width;	//区域横宽
	int		Height;	//区域纵宽
	int     IsRongqi; //是哪个容器产生的 单个对象 或 多个对象容器
};

struct KUiItemdObject
{
	unsigned int	uGenre;
	unsigned int	uId;
	unsigned int    nindex;
	int				DataX;
	int				DataY;
	int				DataW;   //宽度
	int				DataH;   //高度
	int             nGenkind;//所在的容器类型
	CCRect          nRect;
};

struct KUiDraggedObject
{
	unsigned int	uGenre;
	unsigned int	uId;
	int				DataX;
	int				DataY;
	int				DataW;  //宽度
	int				DataH;  //高度
	//    char            uName[32];
	int m_posvs;
	unsigned char m_dirvs,m_vs_lastframe;
	int bCheckpVS;
	KRUPoint* pVS;
	int* ndir;
	unsigned int nDelay;
	KUiDraggedObject()
	{
		bCheckpVS = 0; //FALSE
		nDelay = 0;
		m_posvs=0;
		m_dirvs=0;
		m_vs_lastframe=0;
	};
};
//==================================
//	表示某个游戏对象在坐标区域范围的信息结构
//==================================
struct KUiObjAtRegion
{
	KUiGameObject	Obj;
	KUiRegion		Region;
};

struct KUiObjAtContRegion : public KUiObjAtRegion
{
	union
	{
		UIOBJECT_CONTAINER	eContainer; 
		int					nContainer;
	};
};
//========经脉描述===============
struct KUiJinMaiDesc

{
	struct KUiDesc
	{
		int ncZhenYuan;
		int ncHuMaiDan;
		int ncRaTe;
		int ncMgcid;
		int ncBinjia;
		char ncMiaoShu[215];
		char nXueNane[32];
	
	};
	KUiDesc  nJinMaiDesc[17];
};

//========老虎机===============
struct KUiLaoHuJi
{
	int nIsOpen;
	int nType;
	int nYinL;
	int nJinBi;
	int nBeiLv;
	int nDianShu;
};

//========经脉===============
struct KUiJinMaiLevel
{
	struct KUiLevel
	{
		int  nLeVel[18];
	};
  KUiLevel  nJinMai[10];
  int nZhenYuan;
  int nBinJia;
};
//========经脉===============
struct KUiJinMaiType
{
	int  nType;
	int  nLevel;
	//	KUiJinMaiLevel  nJinMai[10];
};



//========经脉===============
struct KUiJinMaiInfo
{
	int  nZhenYuan;
	int  nBingJia;
//	KUiJinMaiLevel  nJinMai[10];
};

//========附魔===============
struct KUiFuMoInfo
{
	int  nNpcSkill[5];
	BYTE nSkillLevel[5];
	int  nNpcGongji[5];
	int  nNpcFangyu[5];
	int  nNpclife[5];
	int  nNpcGongsu[5];
	int  nNpcXingYun[5];

};

//========帮战信息===============
struct KUiAttackInfo
{
	int         m_AttackState;
	int         m_AttackTime;				        
	int         m_nDeathCount;	   //死亡的次数
	int         m_nDDeathCount;	   //死亡的次数\	
	int         m_nAttackNum;	                            // 参战场数
	int         m_nWinNum;								// 胜利场数
	int         m_nLoseNum;
	char        m_AttackName[32];  //对战的帮会	
};

/*typedef struct
{
	int		m_nBuyIdx;     //买卖ID
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	void	Clear() {m_nBuyIdx = -1; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;}
} NewBuySellInfo;*/


//========同伴===============
struct KUiTongBanbaseInfo
{
int  nmLife;
int  nmLifeFull;
int  nmMana;
int  nmManaFull;
};
//========同伴===============
struct KUiTongBanInfo
{
	int   nDwID;
	char  nTname[32];
	char  nGuiShuName[32];
	int   nCount;
};
//========转盘抽奖===============
struct KUiZhuanpanInfo
{
int  nIsVip;
int  nCJcishu;
char nPlayerName[32];
};
//========股子===============
struct KUiGuZiInfo
{
	int  nIsVip;
	int  nCJcishuA;   //点数
	int  nCJcishuB;   //点数
	int  nCJcishuC;   //点数
	int  nCJcishuD;   //点数
	int  nCJcishuE;   //点数
	char nPlayerNameA[32];
	char nPlayerNameB[32];
	char nPlayerNameC[32];
	char nPlayerNameD[32];
	char nPlayerNameE[32];
	int  nIsShuai;
	int  nGyajinA;  //押金
	int  nGyajinB;  //押金
	int  nGyajinC;  //押金
	int  nGyajinD;  //押金
	int  nGyajinE;  //押金
	int  nIsStartA;  //开始的标示
	int  nIsStartB;  //开始的标示
	int  nIsStartC;  //开始的标示
	int  nIsStartD;  //开始的标示
	int  nIsStartE;  //开始的标示
	int  nZuoweiHao;  //座位号
	int  nIsMenber;
	int  nmodel;   //模式
	int  nSubZuoWeiHao; //地图的座位 该谁甩股了
	int  nIsXiaZhuA;//是否下主了
	int  nIsXiaZhuB;
	int  nIsXiaZhuC;
	int  nIsXiaZhuD;
	int  nIsXiaZhuE;
	int  nCurFarmA;  //当前帧
	int  nCurFarmB;  //当前帧

};

struct KUiItemLian
{
	int nItemDWID;
	char nPlayerName[32];	
};

struct KUiGuZiXinxi
{
int nMapID;
int nDianshu;
int nIsStartID;

};
struct KUiGuZiJieSuan
{
	int nMaxDian;
	int nKFmoney;
	int nGRdian[5];
	int nGRjinbi[5];
	char nPlayerNameA[32];
	char nPlayerNameB[32];
	char nPlayerNameC[32];
	char nPlayerNameD[32];
	char nPlayerNameE[32];
};
struct KUiGuZiCurFarms
{
	int nCurFarmsA;
	int nCurFarmsB;
	int nMapID;
};
//================================

struct PLAYER_GIVEUI
{	
	BYTE	ProtocolType;
	char	m_szName[256];
	BYTE	m_nNum;
    char    m_Callback[64];
	char    m_Error[16];
	//char    m_Content[125];
	unsigned int uId[24];
    char 	m_szTitle[64];
};

struct PLAYER_JISHOU
{	
	BYTE	ProtocolType;
	char    m_szName[256];
	BYTE	m_nNum;
    char    m_Callback[16];
	char    m_Error[16];
	char    m_Content[125];
	unsigned int uId[32];
	char 	m_szTitle[125];
};

struct PLAYER_STRINGUI
{	
	BYTE	ProtocolType;
	char    m_szTitle[64];
	BYTE	m_nNum;
    char    m_SCallback[64];
	char    m_SError[16];
	char    m_Content[125];  //内容
};

struct KUiInformationParam
{
	char	sInformation[256];	//消息文字内容
	char	sConfirmText[64];	//确认消息(按钮)的标题文字
	short	nInforLen;			//消息文字内容的存储长度
	bool	bNeedConfirmNotify;	//是否要发回确认消息(给core)
	bool	bReserved;			//保留，值固定为0
};
//行为清单


enum PLAYER_ACTION_LIST
{
	PA_NONE = 0,	//无动作
	PA_RUN  = 0x01,	//跑
	PA_SIT  = 0x02,	//打坐
	PA_RIDE = 0x04,	//骑（马）
	EX_BOX,			// mo rong ruong 1
	EX_BOX2,		// mo rong ruong 2
	EX_BOX3,		// mo rong ruong 3
	ITEMEX,			// hanh trang
	HT_CN,			// chuc nang dang hoan thien
	OPEN_MARKET,	// ky tran cac
	PW_NOT_SAME,	// mat khau ko giong nhau
	PW_ACCEPTED,	// mat khau dc chap nhan
	PW_NOT_LONG,	// mat khau ko du do dai
	CN_GH,			// chuc nang gioi han
	KD_VP,
	MO_AUTO,
	TAT_AUTO,
	ERORR_AUTO,// 错误
	CESHI_AUTO,//测试
    OPEN_JISHOU,//寄售
    HAVE_SKILL,
	GET_BOX_OPEN,
	GET_BOXEX_OPEN,
};

//==================================
//	系统消息分类
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//不参加分类的消息
	SMT_SYSTEM,		//系统，连接相关
	SMT_PLAYER,		//玩家相关
	SMT_TEAM,		//组队相关
	SMT_FRIEND,		//聊天好友相关
	SMT_MISSION,	//任务相关
	SMT_CLIQUE,		//帮派相关
};

//==================================
//	系统消息响应方式
//==================================
enum SYS_MESSAGE_CONFIRM_TYPE 
{
	SMCT_NONE,				//在对话消息窗口直接掠过，不需要响应。
	SMCT_CLICK,				//点击图标后立即删除。
	SMCT_MSG_BOX,			//点击图标后弹出消息框。
	SMCT_UI_RENASCENCE,		//选择重生
	SMCT_UI_ATTRIBUTE,		//打开属性页面
	SMCT_UI_SKILLS,			//打开技能页面
	SMCT_UI_ATTRIBUTE_SKILLS,//打开属性页面技能页面
	SMCT_UI_TEAM_INVITE,	//答应或拒绝加入队伍的邀请,
	SMCT_UI_TEAM_APPLY,		//答应或拒绝加入队伍的申请,
	SMCT_UI_TEAM,			//打开队伍管理面板
	SMCT_UI_INTERVIEW,		//打开聊天对话界面,
	SMCT_UI_FRIEND_INVITE,	//批准或拒绝别人加自己为好友
	SMCT_UI_TRADE,			//答应或拒绝交易的请求,
	SMCT_DISCONNECT,		//断线
	SMCT_UI_TONG_JOIN_APPLY,//答应或拒绝加入帮会的申请
	SMCT_UI_WAIGUA_FORBIT,  //外挂提示
	SMCT_UI_LOCK_FORBIT,    //账号冻结
};

//==================================
//	系统消息
//==================================
struct KSystemMessage
{
	char			szMessage[256];	//消息文本
	unsigned int	uReservedForUi;	//界面使用的数据域,core里填0即可
	unsigned char	eType;			//消息分类取值来自枚举类型 SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//响应类型
	unsigned char	byPriority;		//优先级,数值越大，表示优先级越高
	unsigned char	byParamSize;	//伴随GDCNI_SYSTEM_MESSAGE消息的pParamBuf所指参数缓冲区空间的大小。
    int		        nMsgLen;
};

//==================================
//	聊天频道的描述
//==================================
struct KUiChatChannel
{
	int			 nChannelNo;
	unsigned int uChannelId;
	union
	{
		int		 nChannelIndex;
		int		 nIsSubscibed;	//是否被订阅
	};
	char		 cTitle[32];
};

//==================================
//	聊天好友的一个分组的信息
//==================================
struct KUiChatGroupInfo
{
	char	    szTitle[32];	//分组的名称
	int		nNumFriend;		//组内好友的数目
};

//==================================
//	好友发来的聊天话语
//==================================
struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char    szContent[256];
};

//==================================
//	获取任务号值
//==================================

struct KUiBaseTask
{
	int nVala;
	int nValb;
};

//==================================
//	远程控制
//==================================

struct KUiKongZhi
{
	char nRoleName[32];
	char nMiYu[32];
};

//==================================
//	主角的一些不易变的数据
//==================================
struct KUiPlayerBaseInfo
{
	char	    Agname[32];	//绰号
	char	    Name[32];	//名字
	char    Title[32];	//称号
	int		nCurFaction;// Hi?n t?i ?ang gia nh?p m?n phái id N?u là -1 thì hi?n t?i kh?ng có trong m?n phái.
	int		nRankInWorld;//Giá tr? x?p h?ng Jianghu, giá tr? 0 có ngh?a là kh?ng có trong b?ng x?p h?ng
	unsigned int nCurTong;// Id tên nhóm hi?n ?? tham gia, n?u nó là 0, hi?n kh?ng có trong nhóm
	int		nMissionGroup;// Hi?n t?i ?ang gia nh?p m?n phái id N?u là -1 thì hi?n t?i kh?ng có trong m?n phái.
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int     nReBornLevel;
	int		nPKValue; // pk
	int     nLucky;
	int     nTonLuky;//B?n ??ng hành may m?n
	int     nCurPinDao;//kênh t?m th?i -1 cho kh?ng
};

struct KUiPlayerPicInfo
{
	char  nPicPath[256];	//头像路径
	int     nSprRankid;
};

struct KUiPlayerActivityInfo
{
	char	nTimeInfo[64];	//时间信息
	int     nTaskInfo;
	int     nHour;
	int     nMin;
	int     nNpcLevel;
	int     nExpTime;            //上次领取经验的时间
	int     nYinLiangTime;       //上次领取经验的时间
	int     nJinBiTime; 
	int     nExpPiontTime;
};

struct KUiPlayerGameTime
{

	LONG   npTime;
	
};



//----------------------
struct KUiPlayerSetTimeInfo
{
	char	  nTimeInfo[64];	//时间信息
	int     nTaskInfo;
	int     nHour;
	int     nMin;
	int     nNpcLevel;
	int     nExpTime;            //上次领取经验的时间
	int     nYinLiangTime;       //上次领取经验的时间
	int     nJinBiTime; 
	int     nExpPiontTime;
	
};
//==================================
//	主角的一些易变的数据
//==================================
struct KUiPlayerRuntimeInfo
{
	int		nLifeFull;			//生命满值
	int		nLife;				//生命
	int		nManaFull;			//内力满值
	int		nMana;				//内力
	int		nStaminaFull;		//体力满值
	int		nStamina;			//体力
	int		nAngryFull;			//怒满值
//	int		nAngry;				//怒
	int		nExperienceFull;	//经验满值
	int		nExperience;		//当前经验值
	int		nCurLevelExperience;//当前级别升级需要的经验值
    int     nIsGetSkill;
	int     nIsGetChaqi;
	unsigned char	byActionDisable;//是否不可进行各种动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned char	byAction;	//正在进行的行为动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned short wReserved;	//保留
	int     nVipType;
	DWORD  nGameLeftTime;
	unsigned int     nIsTranse;
};

struct KUiGetCurAngry
{
	int		nAngry;				//怒
};

struct KItemlian
{    
	
	unsigned	int     uGenre;
	int                cClorid;	
	unsigned	int    cItemID;	    
	char           cItemName[32];  //名字
	char         cPlayerName[32];  //玩家名字
    int         nDwid;
};

struct KUiPlayerindexbaseInfo
{
	int		nmLifeFull;			//生命满值
	int		nmLife;				//当前生命
	int		nmManaFull;			//内力满值
	int		nmMana;				//当前内力
	int		nmStaminaFull;		//体力满值
	int		nmStamina;			//体力
	int		nmAngryFull;			//怒满值
	int		nmAngry;				//怒
	int		nmExperienceFull;	//经验满值
	int		nmExperience;		//当前经验值
	int		nmCurLevelExperience;//当前级别升级需要的经验值
	
	unsigned char	mbyActionDisable;//是否不可进行各种动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned char	mbyAction;	//正在进行的行为动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned short mwReserved;	//保留
	int     nmLuky;//当前幸运值
};

//==================================
//	主角的一些属性数据索引
//==================================
enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			 //力量
	UIPA_DEXTERITY,				//身法
	UIPA_VITALITY,				//外功
	UIPA_ENERGY,				//内功
};

//==================================
//	主角的一些易变的属性数据
//==================================
struct KUiPlayerAttribute
{
	int		nMoney;				//银两
	int		nXu;
	int		nLevel;				//等级
	char    StatusDesc[16];		//状态描述

	int		nBARemainPoint;		//基本属性剩余点数
	int		nStrength;			//力量
	int		nDexterity;			//敏捷
	int		nVitality;			//活力
	int		nEnergy;			//精力

	int		nKillMAX;			//sat thuong lon nhat
	int		nKillMIN;			//sat thuong nho nhat
	int		nRightKillMax;		//sat thuong lon nhat tay phai
	int		nRightKillMin;		//sat thuong nho nhat tay phai

	int		nAttack;			//命中力
	int		nDefence;			//防御力
	int		nMoveSpeed;			//移动速度
	int		nAttackSpeed;		//外攻击速度
	int		nCastSpeed;		    //内攻击速度

	char		nPhyDef[32];			// Phòng ng? v?t l? 
	char		nCoolDef[32];			//冰冻防御
	char		nLightDef[32];			//闪电防御
	char		nFireDef[32];			//火焰防御
	char		nPoisonDef[32];			//毒素防御

	int		nRankInWorld;	// xep hang
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // phuc duyen
	int     nLuky;//giá tr? may m?n hi?n t?i
	int     nZhonji; // Tr?ng kích 
	int     nDangge; // Ng?n cách 
	int     nSprRankId;
	int     nForBitChat;
};

//==================================
//	主角的立即使用快捷键物品与武功
//==================================
struct KUiPlayerImmedItemSkill
{
	KUiGameObject	ImmediaItem[9];  //快捷键物品数量限制
	KUiGameObject	IMmediaSkill[2]; 
	int nStackNum;
};

//==================================
//	主角装备安换的位置
//==================================
enum UI_EQUIPMENT_POSITION
{
	UIEP_HEAD = 0,		//头戴
	UIEP_HAND = 1,		//手持
	UIEP_NECK = 2,		//脖子
	UIEP_FINESSE = 3,	//手腕
	UIEP_BODY = 4,		//身穿
	UIEP_WAIST = 5,		//腰部
	UIEP_FINGER1 = 6,	//手指甲
	UIEP_FINGER2 = 7,	//手指乙
	UIEP_WAIST_DECOR= 8,//腰坠
	UIEP_FOOT = 9,		//脚踩
	UIEP_HORSE = 10,	//马匹
	UIEP_MASK = 11,		// mat na
	UIEP_PIFENG=12,	    //披风
	UIEP_YINJIAN=13,    //印鉴
	UIEP_SHIPING=14,	//饰品	
};

//==================================
//	主角的生活技能数据
//==================================
struct KUiPlayerLiveSkillBase
{
	int		nRemainPoint;			//剩余技能点数
	int		nLiveExperience;		//当前技能经验值
	int		nLiveExperienceFull;	//升到下级需要的经验值
};


//==================================
//	进度条数据
//==================================

struct KUiJinDuTiaoInfo
{
	char 	nTitle[32];
	int		nMsgIndex;	
	int		nTimes;	  
	char    nCallFuc[16];
	char    szParma[32];
	char    szParmb[32];
	char    szParmc[32];
};

//==================================
//	单项技能数据
//==================================
struct KUiSkillData : public KUiGameObject
{
	union
	{
		int		nLevel;
		int		nData;
	};
//int	nJiaChengSkill;
};


//==================================
//	单项象棋数据
//==================================
struct KUiChessData : public KUiGameObject
{
	union
	{
		int		nLevel;
		int		nData;
	};
	//int	nJiaChengSkill;
};


//==================================
//	一个队伍中最多包含成员的数目
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	8

//==================================
//	统帅能力相关的数据
//==================================
struct KUiPlayerLeaderShip
{
	int		nLeaderShipLevel;			//统帅力等级
	int		nLeaderShipExperience;		//统帅力经验值
	int		nLeaderShipExperienceFull;	//升到下级需要的经验值
};

/*struct KUiItemObject
{
	unsigned int	uGenre;
	unsigned int	uId;
	int				DataX;
	int				DataY;
	int				DataW;  //宽度
	int				DataH;  //高度
};*/
//==================================
//	一个玩家角色项
//==================================
struct KUiPlayerItem
{
	char			Name[32];	    //玩家角色姓名
	unsigned int	uId;		    //玩家角色DWid
	int				nIndex;		    //玩家角色索引
	int				nData;		    //此玩家相关的一项数值，含义与具体的应用位置有关
	char            BZname[32];     //帮主的名字
	int             nShopType;      //商店的类型
	int             nApplyStartPos; //申请列表开始位置
};
//==================================
//	一个玩家角色项
//==================================
struct KUiLianItem
{
	char			Name[32];	//玩家名称
	unsigned int	uId;		//物品DWid
	int				nIndex;		//物品索引
	int				w;		    //宽度
	int             h;          //长度
};

//==================================
//	组队信息的描述
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//==================================
//	队伍信息
//==================================
struct KUiPlayerTeam
{
	bool			bTeamLeader;			//玩家自己是否队长
	char			cNumMember;				//队员数目
	char			cNumTojoin;				//欲加入的人员的数目
	bool			bOpened;				//队伍是否允许其他人加入
	int				nTeamServerID;			//队伍在服务器上的id，用于标识该队伍，-1 为空
	int				nCaptainPower;
};

/* 这是旧的代码，新的已经放在KNpcGold里面处理了
//==================================
//	NPC加强
//==================================
struct KNpcEnchant
{
	int		nExp;					// 经验
	int		nLife;					// 生命
	int		nLifeReplenish;			// 回血
	int		nAttackRating;			// 命中
	int		nDefense;				// 防御
	int		nMinDamage;
	int		nMaxDamage;

	int		TreasureNumber;				// 装备
	int		AuraSkill;					// 光环
	int		DamageEnhance;				// 伤害
	int		SpeedEnhance;				// 速度
	int		SelfResist;					// 自身抗性
	int		ConquerResist;				// 相生抗性
#ifndef _SERVER
	char	NameModify[32];				// 改名
#endif
};

//==================================
//	NPC单项加强
//==================================
struct KNpcSpeicalEnchant
{
	int		ValueModify;
	char	NameModify[16];
};
*/

//==================================
//	选项设置项
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//透视模式  nParam = (int)(bool)bEnable 是否开启
	OPTION_DYNALIGHT,		//动态光影	nParam = (int)(bool)bEnable 是否开启
	OPTION_MUSIC_VALUE,		//音乐音量	nParam = 音量大小（取值为0到-10000）
	OPTION_SOUND_VALUE,		//音效音量	nParam = 音量大小（取值为0到-10000）
	OPTION_BRIGHTNESS,		//亮度调节	nParam = 亮度大小（取值为0到-100）
	OPTION_WEATHER,			//天气效果开关 nParam = (int)(bool)bEnable 是否开启
};

//==================================
//	所处的地域时间环境信息
//==================================
struct KUiSceneTimeInfo
{
	char    szSceneName[64];		//场景名
	int		nSceneId;				//场景id
	int		nScenePos0;				//场景当前坐标（东）
	int		nScenePos1;				//场景当前坐标（南）
	int		nGameSpaceTime;			//以分钟为单位
	char    nMapType[32];           //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;

	char    nWarGTong[32];
	char    nWarSTong[32];
	int     nIsWhos;

    char    nPlayerName[32];  //自己名字
	char    nTongMaster[32];  //自己帮主名
 /*

	char    nWarFuchengName[32]; //副城名字
	char    nWarZuohuName[32];
	char    nWarYuohuName[32];
	char    nWarTaiShiName[32];
	char    nWarZhonSName[32];
	char    nWarShangSName[32];
	char    nWarMishuName[32];
	char	nWarTaiLeName[32];    // 太乐
 

	int     nIsFuCheng;
	int     nIsZuoHu;
    int     nIsYuoHu;
	int     nIsTaiS;
	int     nIsZhonS;
	int     nIsShangS;
	int     nIsMiShu;
	int     nIsTaiLe;

	int     nCZIsNanOrNv;//是男或女
	int     nFZIsNanOrNv;
	int     nZHIsNanOrNv;
	int     nYHIsNanOrNv;
	int     nTSIsNanOrNv;
	int     nZSIsNanOrNv;
	int     nSSIsNanOrNv;
	int     nMSIsNanOrNv;
	int     nTLIsNanOrNv;
	*/
    int     nTaskVal;
	BOOL    nIsHideNuQi;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoA
{
	char	   szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoB
{
	char	   szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoC
{
	char    szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoD
{
	char	   szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoE
{
	char	    szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoF
{
	char    szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoG
{
	char   szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};
//==================================
//	所处的帮战地图信息
//==================================
struct KUiCityMapInfoH
{
	char    szSceneName[32];		   //场景名
	int		nSceneId;				   //场景id
	int		nScenePos0;				   //场景当前坐标（东）
	int		nScenePos1;				   //场景当前坐标（南）
	int		nGameSpaceTime;			   //以分钟为单位
	char    nMapType[16];               //地图类型
	char    nWarMaster[32];
	char    nWarTong[32];
	int     nWarShui;
	int     nWarZhi;
	int     nIsWarCity;
};

//==================================
//	光源信息
//==================================
//整数表示的三维点坐标
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};

struct KLightInfo
{
	KPosition3 oPosition;			// 光源位置
	DWORD dwColor;					// 光源颜色及亮度
	long  nRadius;					// 作用半径
};


//小地图的显示内容项
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//无东西
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//显示缩略图
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//显示人物
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//显示同队伍人
};

//场景的地图信息
struct KSceneMapInfo
{
	int	nScallH;		//真实场景相对于地图的横向放大比例
	int nScallV;		//真实场景相对于地图的纵向放大比例
	int	nFocusMinH;
	int nFocusMinV;
	int nFocusMaxH;
	int nFocusMaxV;
	int nOrigFocusH;
	int nOrigFocusV;
	int nFocusOffsetH;
	int nFocusOffsetV;
};

struct KSceneMapSizePos
{
int Arealeft;
int Areatop;
};

struct KChaQIMapPos
{
	int	nCursorX;		//真实场景相对于地图的横向放大比例
	int nCursorY;		//真实场景相对于地图的纵向放大比例

};

struct KSkillMapPos
{
	int	nCursorX;		//真实场景相对于地图的横向放大比例
	int nCursorY;		//真实场景相对于地图的纵向放大比例
	
};


struct KNewsMessage
{
	int		nType;						//消息类型
	char	sMsg[MAX_MESSAGE_LENGTH];	//消息内容
	int		nMsgLen;					//消息内容存储长度
};

struct KTaskPathInfo
{
	int		nTaskType;				    //消息类型
	int 	nMsgIndex;	                //消息索引
	int		nMapIdx;					//任务地图
    int		nMapXpos;
	int		nMapYpos;
};


struct	KTaskClient
{
	char szContent[128];	//内容
	int	 nTaskidx;			//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
	int	 nTaskType;
	char szTaskName[32];
}; 

struct KRankIndex
{
	bool			bValueAppened;	//每一项是否有没有额外数据
	bool			bSortFlag;		//每一项是否有没有升降标记
	unsigned short	usIndexId;		//排名项ID数值
};

struct KListIndex
{
    int nTaskId;
	int nType;
	int nClass;
	int nTrace;
};


#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// 文字内容
	unsigned short		usMsgLen;				// 文字内容的长度
	short				cSortFlag;				// 旗标值，QOO_RANK_DATA的时候表示出升降，负值表示降，正值表示升，0值表示位置未变
	int					nValueAppend;			// 此项附带的值

};

struct KMissionRecord
{
	char			sContent[256];	//存储任务提示信息的缓冲区，（字符串为控制符已经编码的字符串）
	int				nContentLen;	//sContent内有效内容的长度(单位：字节)，长度最大一定不超过256字节
	unsigned int	uValue;			//关联数值
};

//帮派信息
struct KTongInfo
{
	char  szName[32];			//帮会名字
	char  szMasterName[32];		//帮主名字
	int   nFaction;				//帮会阵营
	int   nMoney;				//帮会资金
	int   nReserve[2];          //保留数据
	int	  nMemberCount;         //帮众数量
	int   nManagerCount;        //队长数量
	int   nDirectorCount;       //长老数量
	int   nFigure;              //待删除
	int   nLevel;               //帮会等级
	int   nGongxian;            //帮会贡献度
	int   nHouYue;              //帮会活跃度
};

//帮派基本信息
struct KClientTongInfo
{
	char  szName[32];			//帮会名字
	char  szMasterName[32];		//帮主名字
	int   nFaction;				//帮会阵营
	int   nMoney;				//帮会资金
	int   nReserve[2];          //保留数据
	int	  nMemberCount;         //帮众数量
	int   nManagerCount;        //队长数量
	int   nDirectorCount;       //长老数量
	int   nApplyCount;          //申请列表的人数
	int   nZhaoMuCount;         //发布招募信息的数量
	int   nFigure;              //待删除
	int   nLevel;               //帮会等级
	int   nGongxian;            //帮会贡献度
	int   nHouYue;              //帮会活跃度
};

//帮派招募和公告信息
struct KClientZhaoMuInfo
{
    int 	inType;
    char szQingXiang[32];
    char szHuoDong1[32];
    char	 szHuoDong2[32];
    char szHuoDong3[32];
    char szHuoDong4[32];
    char szJiYu[64];
    char    szGongGao[128];
    int     inAcceptLevel;
	int     inRefuseLevel;
};
//申请加入帮列表
struct KClientApplyList
{
    struct ApplyList
	{
      char	  szName[32];  //名字
      int     inLevel;     //等级
	};
      int     nCurNum;
	  int 	  inType;      //类型
	  int     nStartPos;
    ApplyList nInfo[defTONG_ONE_PAGE_MAX_NUM];
};
//对帮内成员做的动作，或改变自己与帮会的关系的动作
enum TONG_ACTION_TYPE
{
	TONG_ACTION_DISMISS,       //踢人
	TONG_ACTION_ASSIGN,        //任命
	TONG_ACTION_DEMISE,        //传位
	TONG_ACTION_LEAVE,         //离帮
	TONG_ACTION_RECRUIT,       //招人状态
	TONG_ACTION_APPLY,         //申请加入

};

enum TONG_MONEY_ACTION_TYPE
{
	TONG_ACTION_SAVE,       //踢人
	TONG_ACTION_GET,        //任命
	TONG_ACTION_SND,        //传位
};

//对帮内成员做的动作，或改变自己与帮会的关系的动作时的参数
struct KTongOperationParam
{
	TONG_ACTION_TYPE	eOper;				//操作项
	int                 nData[4];
	char				szPassword[32];		//操作项需要的密码，如果对应的操作项不需要密码，则直接填密码为空。
};

//帮会成员项
struct KTongMemberItem : public KUiPlayerItem
{
	char szAgname[64];	//在帮内的职务/称号
};

//某个玩家与XX的关系，(XX可以是帮会，队伍等等)
struct KUiPlayerRelationWithOther : KUiPlayerItem
{
	int		nRelation;
	int		nParam;
};

//通用的带名称描述游戏对象的结构
struct KUiGameObjectWithName
{
	char			szName[32];  //帮会名
	char			szString[32];
	int				nData;       //职位
	int				nParam;      //当前开始编号
	unsigned int 	uParam;      //当前数量
};

//通用帮会信息结构
struct KUiTongInfoWithName
{
    char	 szTongName[32];
	char	 szQingXiang[32];
	char	 szHuoDong1[32];
	char	 szHuoDong2[32];
	char szHuoDong3[32];
	char szHuoDong4[32];
	char szJiYu[64];
	char    szGongGao[128];
	int     inAcceptLevel;
	int     inRefuseLevel;
};


//通用帮会信息结构
struct gameAutoplaerInfo
{
	int 	 isopenmoney;
	int      isopendaoju;
	int      insopenzhuang;
	int      isopentuzhi;
	int      ismonyeandzhuang;
};

//通用执行脚本
struct KUiPlyerDoscript
{
	int     inKind;
	int     inCurTime;
	int     inCurTimeb;
	int     inCurTimec;
	int     inCurTimed;
	char    szItemInfo[32];
};

//通用执行脚本
struct KUiPlyerVipDoscript
{
	int     inKind;
	int     inCurExp;
	int     inCurLucky;
	int     inCurJiHuo;
	int     inCurWuHang;
	char    snCardAcc[32];
	char    snCardPass[32];
};


//通用执行脚本
struct KUiPlyerDuanZao
{
	int     inKind;
	int     inCurTimea;
	int     inCurTimeb;
	int     inCurTimec;
	int     inCurTimed;
	//char    szItemInfo[32];
};


//通用执行脚本
struct KUiPlyerVipDaZao
{
	int     inKind;
	int     inCurExp;
	int     inCurLucky;
	int     inCurJiHuo;
	int     inCurWuHang;
	char    snCardAcc[32];
	char    snCardPass[32];

};

struct	nJinDuClient
{
	int  nTimeVal;	   
	int	 nMsgidx;
	char szCallFun[16];   //回调函数
	char szParma[32];     //参数
	char szParmb[32];     //参数
	char szParmc[32];     //参数
}; 
//----------测试客户端结束
#endif


#endif
