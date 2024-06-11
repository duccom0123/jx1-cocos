#ifndef KPROTOCOL_H
#define KPROTOCOL_H
#include "cocos2d.h"
USING_NS_CC;
#include "GameDataDef.h"
#include <string.h>
#include "KProtocolDef.h"
#include "KRelayProtocol.h"

#pragma pack(push, enter_protocol)
#pragma	pack(1)  //4字节对齐

#define	PROTOCOL_MSG_TYPE	BYTE
#define PROTOCOL_MSG_SIZE	(sizeof(PROTOCOL_MSG_TYPE))
#define	MAX_PROTOCOL_NUM	249  //协议数量限制
#define _NAME_LEN	32

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;
typedef struct _GUID GUID;
#endif

////
typedef struct
{
	BYTE	ProtocolType;
	int 	WalkSpeed;
	int  	RunSpeed;
	int 	HelmType;
	int 	ArmorType;
	int 	WeaponType;
	int 	HorseType;
	int 	RankID;
	UINT	ID;
	BYTE	m_btSomeFlag;	// 0x01 PKFlag 0x02 FightModeFlag 0x04 SleepModeFlag
	int		MaskType;	// 面具
	int 	PifengType; // 披风
	BYTE	m_bBaiTan;	// ban hang
	char	TongName[32]; //帮会
	int 	m_bFigure;	// 是否加帮派
	BYTE	RankFFID;
	BYTE    AutoplayId;  //挂机
	BYTE	ExItemID; // hanh trang
	BYTE	ExBoxID; // ruong mo rong
	int 	RankInWorld; // xep hang tin tuc
	int 	Repute; // 声望
	int 	PKValue; // pk tin tuc
	int 	ReBorn; // trung sinh tin tuc
	int     mFaction;
	int     mCJtaskID;      //转盘
} PLAYER_SYNC;

struct SFuMoInfo
{
	int nNpxidx;
	int nNpxSetNo;
	int nNpxSkill;
	int nSlevel;
};

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;//
	int 	WalkSpeed;			// 是否只要传一份就够了（只改变一个或是同时改变）
	int 	RunSpeed;
	int 	HelmType;
	int 	ArmorType;
	int 	WeaponType;
	int 	HorseType;
	int 	RankID;
	BYTE	m_btSomeFlag;	// 0x01 PKFlag 0x02 FightModeFlag 0x04 SleepModeFlag 0x08 TongOpenFlag
	int 	PifengType;//披风
	BYTE    nChiBangType;
	BYTE	m_bBaiTan;	// ban hang
	BYTE	RankFFID;
	BYTE    AutoplayId;
	BYTE	ExItemID; // hanh trang
	BYTE	ExBoxID; // ruong mo rong
	int 	RankInWorld; // xep hang tin tuc
	int 	Repute; // danh vong tin tuc
	int 	PKValue; // pk tin tuc
	int 	ReBorn; // trung sinh tin tuc
	int     mFaction;
	BYTE    mIsCreatTongBan;//是否已经有同伴
	int     mJinMaiBingJia;
	int     mCJtaskID;      // 转盘
	int     mZhenYuan;
	int     msPlayerIdx;
	BYTE    nIsTranse;
	BYTE    nIsSerLock;
	BYTE    nVipType;
	BYTE    nIsOpenMapType;
	int     mAttackState;
	int     m_IsInCity;		  
	int     IsVip;
	int     mIsWarCity;
	int     m_bFigure; //帮会职位
	char    TongName[32];
	UINT	m_LockNpcDwID;//DWORD
	char    ShopName[32];
	BYTE    nCurGsSerIdx;
} PLAYER_NORMAL_SYNC;
		     

typedef struct
{
	BYTE	ProtocolType;
	UINT   ID;//DWORD
	int     m_IsInCity;

} PLAYER_NORMALMAP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT    ID;//
	int     m_Skillid;
	int     m_Time;
} PLAYER_ONESTATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				int_PW;	
} PLAYER_REQUEST_CP_UNLOCK;

typedef struct
{
	BYTE			ProtocolType;
	int             _isMobile; //是否手机端
	int             _clientVer;//客户端版本 1 中国 2越南 其他 英语
} PLAYER_REQUEST_CP_LOCK;

typedef struct
{
	BYTE			ProtocolType;
} RESET_PASS;

typedef struct
{
	BYTE			ProtocolType;
	char m_szTitle[64];
    char m_SCallback[64];
    char m_SError[16];
	int  m_nNum;
	int  m_MaxLen;	 //最大长度
}PLAYER_STRING;
 /*
typedef struct
{
	BYTE			ProtocolType; //stringbox
} STRING_BOX;
 */
typedef struct
{
	BYTE			ProtocolType;
} DATAU_BOX;

typedef struct
{
	BYTE			ProtocolType;
} PKVALUE;

typedef struct
{
	BYTE			ProtocolType;
	int				int_OldPW;	
	int				int_NewPW;	
} PLAYER_REQUEST_CP_CHANGE;

typedef struct
{
	BYTE			ProtocolType;	
	int				int_ResetPW;	
} PLAYER_REQUEST_CP_RESET;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nReputeValue;
} REPUTE_VALUE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nFuYuanValue;
} FUYUAN_VALUE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nReBornValue;
	int             m_nReBornLevel;		       //当前转生的等级
	int             m_nReBornKeepQpiont;       //保留的潜能点
	int	            m_nReBornKeepJpiont;       //保留的技能
	int             m_nReBornSetSkillMaxLevel; //技能等级上限增加
	int             m_nReBornSetFanYuMaxVal;   //防御上限增加
	int             m_nCurPlyaerLvel;
	int             m_nCurReBornNum;
	int             m_nReBornLifeMaxVal;
} REBORN_VALUE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nRoleVipValue;
	int             m_nRoleVipLevel;		       //当前转生的等级
	int             m_nRoleVipKeepQpiont;       //保留的潜能点
	int	            m_nRoleVipKeepJpiont;       //保留的技能
	int             m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
	int             m_nRoleVipSetFanYuMaxVal;   //防御上限增加
	int             m_nCurPlyaerLvel;
	int             m_nCurRoleVipNum;
	int             m_nRoleVipLifeMaxVal;
	int             m_nRoleVipDamageMaxVal;
} ROLEVIP_VALUE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;//DWORD
	int		RankFF;
}	RANKFF_SYNC; // danh hieu
/*
typedef struct	
{
	BYTE	ProtocolType; //stringbox
	int		nbutton;
} CP_STRING;

typedef struct	
{
	BYTE	ProtocolType; //stringbox
	int		nbutton;
} CP_AUTOPLAYOP;

typedef struct	
{
	BYTE	ProtocolType; //stringbox
	int		nbutton;
} CP_AUTOPLAYCL;
 */
typedef struct	
{
	BYTE	ProtocolType;
	int		nbutton;
	int     nXuHao;
} CP_DATAU;

typedef struct	
{
	BYTE	ProtocolType; //pkvalue
	int		nbutton;
} CP_PKVALUE;

typedef struct	
{
	BYTE	ProtocolType; //pkvalue
	int		nbutton;
    int     ncishu;
	int     ByteVal;
	int     ByteVala;
	int     ByteValb;
} CP_AUTOPLAY;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	Camp;				// 阵营
	BYTE	CurrentCamp;		// 当前阵营
	BYTE	m_bySeries;			// 五行系
	//BYTE 	LifePerCent;		// 生命百分比
	int 	m_btMenuState;		// 组队、交易等状态
	BYTE	m_Doing;			// 行为
	BYTE	m_btKind;			// npc类型
	int 	MapX;				// 位置信息
	int 	MapY;				// 位置信息
	UINT	ID;					// Npc的唯一ID  DWORD
	int		NpcSettingIdx;		// 客户端用于加载玩家资源及基础数值设定
	WORD	NpcEnchant;			// 加强的NPC（后面的bit表示加强类型，名字是否金色之类的由加强的数目在客户端确定）
	int     Dangge;
	int     Zhongji;
	int     nLifeYu;
	char	m_szName[32];		// 名字
} NPC_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;//DWORD
	int 	MapX;
	int 	MapY;
	BYTE	Camp;
	BYTE	nKind;
	BYTE	Doing;
	BYTE	nState;
	int		ASpeed;
	int		CSpeed;
	int     Dangge;
	int     Zhongji;
	BYTE    RankFFID;
	int     AIsLukey;
	int     AIsJinYan;
	int     AIsJinQian;
	int     IsXingYunXing;
	char    nName[32];
	char    _clientName[32]; //客户端显示的说明名称
	int     nLifeDamagePerCent;
	int 	mliveTime;
	UINT	nGuiShuDwid;//DWORD
	int     nZhuaVal;
	UINT    nTongBanNum;//DWORD
	int     nAttackFrame;
	int     nCastFrame;
	int     nNpcMaxLife;
	int     nNpcDefen;
	int     nNpcMaxDefen;
	BYTE    nIsSerLock;
	int     nExpPer;
	int     gsLukey;
	int     nMaskType;
	BYTE    nIsSynDir;
	int     nDir;
	BYTE    nIsMove;
	int     nReForm;
	int     nCurrentLifeMax;
	int     nLifeYu;
	int     nNpcTitle;
	int     nCurNpcTitle;
} NPC_NORMAL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	m_dwNpcID;//DWORD
	int 	m_dwMapX;
	int 	m_dwMapY;
	int 	m_wOffX;
	int 	m_wOffY;
	UINT    m_GameLeftTiem;//DWORD
} NPC_PLAYER_TYPE_NORMAL_SYNC;

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	int		m_nMissionGroup;		// 新阵营
} PLAYER_MISSION_DATA;

typedef struct
{
	BYTE	ProtocolType;
	TMissionLadderSelfInfo SelfData;
	TMissionLadderInfo MissionRank[MISSION_STATNUM]; //680+209=
} PLAYER_MISSION_RANKDATA;

typedef struct
{
	BYTE	    ProtocolType;
	int			m_nSaleType;
	BuySellInfo m_BuySellInfo;
} S2C_SUPERSHOP;

                 
typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;//DWORD
    char	nDeathName[32];
    
}NPC_SIT_SYNC, NPC_DEATH_SYNC, NPC_REQUEST_COMMAND, NPC_REQUEST_FAIL;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;//DWORD
	BOOL	Rv;
} NPC_REMOVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	char	szMsg[MAX_SENTENCE_LENGTH];
	int		nMsgLen;
}NPC_CHAT_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int 	mType;
	
} NPC_REVIVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_JUMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int		nFrames;
	int		nX;
	int		nY;
} NPC_HURT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	BYTE	Camp;
} NPC_CHGCURCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	BYTE	Camp;
} NPC_CHGCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	ID;
	int		nSkillID;
	int		nSkillLevel;
	int		nMpsX;
	int		nMpsY;
	int		nSkillEnChance;
	int     nIsEnChance;
	int     nMaxBei;   
	int     nWaitTime;
} NPC_SKILL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	int		nSkillID;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	ReviveType;
} NPC_REVIVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
	int     nMapID;
} NPC_RUN_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nShopIndex;
} SALE_BOX_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nNpcId;
} PLAYER_DIALOG_NPC_COMMAND; //主角与nNpcId对话的请求

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nExp;				// 当前经验
} PLAYER_EXP_SYNC;				// 玩家同步经验

typedef struct
{
	BYTE	ProtocolType;
	unsigned long  nNpcDwid;
} PLAYER_REQ_COMMAND;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_CREATE_TEAM;		// 客户端玩家创建队伍，向服务器发请求

struct PLAYER_SEND_CREATE_TEAM_SUCCESS
{
	BYTE	ProtocolType;		// 协议名称
	UINT	nTeamServerID;		// 队伍在服务器上的唯一标识
	PLAYER_SEND_CREATE_TEAM_SUCCESS() {nTeamServerID = -1;}
};	// 服务器通知玩家队伍创建成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btErrorID;		// 队伍创建不成功原因：0 同名 1 玩家本身已经属于某一支队伍 3 当前处于不能组队状态
} PLAYER_SEND_CREATE_TEAM_FALSE;// 服务器通知客户端队伍创建不成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwTarNpcID;		// 查询目标 npc id
} PLAYER_APPLY_TEAM_INFO;		// 客户端向服务器申请查询某个npc的组队情况

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int  	m_IsNoRoom;		    // 查询目标 npc id
} PLAYER_NO_ROOM_INFO;		    // 客户端向服务器报告包袱不足

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT   nNpcDwid;
} NPC_SYN_INFO;		            // 客户端向服务器要去更新NPC信息


typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_INFO_FALSE;	// 服务器告知客户端申请查询某个npc的组队情况失败

typedef struct PLAYER_SEND_TEAM_INFO_DATA
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nCaptain;			// 队长 npc id
	int		m_nMember[MAX_TEAM_MEMBER];	// 所有队员 npc id
	UINT	nTeamServerID;		// 队伍在服务器上的唯一标识
	PLAYER_SEND_TEAM_INFO_DATA() {nTeamServerID = -1;};
} PLAYER_SEND_TEAM_INFO;		// 服务器向客户端发送某个队伍的信息数据

typedef struct PLAYER_SEND_SELF_TEAM_INFO_DATA
{
	BYTE	ProtocolType;							// 协议名称
	BYTE	m_btState;								// 队伍状态
	UINT	m_dwNpcID[MAX_TEAM_MEMBER + 1];			// 每名成员的npc id （队长放在第一位）
	char	m_szNpcName[MAX_TEAM_MEMBER + 1][32];	// 每名成员的名字（队长放在第一位）
	UINT	nTeamServerID;							// 队伍在服务器上的唯一标识
	UINT	m_dwLeadExp;							// 玩家的统率力经验
	WORD	m_btLevel[MAX_TEAM_MEMBER + 1];			// 每名成员的等级（队长放在第一位）
	PLAYER_SEND_SELF_TEAM_INFO_DATA() {memset(m_szNpcName, 0, 32 * (MAX_TEAM_MEMBER + 1)); nTeamServerID = -1;};
} PLAYER_SEND_SELF_TEAM_INFO;						// 服务器向客户端发送客户端自身队伍的信息数据

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btOpenClose;		// 打开或关闭
} PLAYER_TEAM_OPEN_CLOSE;		// 队伍队长向服务器申请开放、关闭队伍是否允许接收成员状态

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwTarNpcID;		// 目标队伍队长npc id 或者 申请人 npc id
} PLAYER_APPLY_ADD_TEAM;		// 玩家向服务器申请加入某个队伍或者服务器向某个队长转发某个玩家的加入申请

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwNpcID;			// 被接受入队伍的npc id
} PLAYER_ACCEPT_TEAM_MEMBER;	// 玩家通知服务器接受某个玩家入队伍

typedef struct PLAYER_TEAM_ADD_MEMBER_DATA
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_btLevel;			// 加入者等级
	UINT	m_dwNpcID;			// 加入者npc id
	char	m_szName[32];		// 加入者姓名
	PLAYER_TEAM_ADD_MEMBER_DATA() {memset(m_szName, 0, 32);};
} PLAYER_TEAM_ADD_MEMBER;		// 服务器通知队伍中的各个玩家有新成员加入

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_LEAVE_TEAM;		// 客户端玩家申请离队

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwNpcID;			// 离队npc id
} PLAYER_LEAVE_TEAM;			// 服务器通知各队员某人离队

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwNpcID;			// 离队npc id
} PLAYER_TEAM_KICK_MEMBER;		// 队长踢除某个队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwNpcID;			// 目标npc id
} PLAYER_APPLY_TEAM_CHANGE_CAPTAIN;// 队长向服务器申请把自己的队长身份交给别的队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwCaptainID;		// 新队长npc id
	UINT	m_dwMemberID;		// 新队员npc id
} PLAYER_TEAM_CHANGE_CAPTAIN;	// 服务器通知各队员更换队长

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_DISMISS;	// 向服务器申请解散队伍

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPKFlag;			// pk 开关
} PLAYER_SET_PK;				// 向服务器申请打开、关闭PK

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	BYTE	m_btCamp;				// 新阵营
	BYTE	m_btCurFaction;			// 当前门派
	BYTE	m_btFirstFaction;		// 首次加入门派
	int		m_nAddTimes;			// 加入门派次数
} PLAYER_FACTION_DATA;				// 服务器发给客户端门派信息

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
} PLAYER_LEAVE_FACTION;				// 服务器通知玩家离开门派

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	BYTE	m_btCurFactionID;		// 当前门派id
	WORD	m_btLevel;				// 技能开放等级
} PLAYER_FACTION_SKILL_LEVEL;		// 服务器通知玩家开放当前门派技能到某个等级

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
} PLAYER_APPLY_FACTION_DATA;		// 客户端申请更新门派数据

typedef struct PLAYER_SEND_CHAT_DATA_COMMAND
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_wLength;
	BYTE	m_btCurChannel;		// 当前聊天频道
	BYTE	m_btType;			// MSG_G_CHAT 或 MSG_G_CMD 或……
	BYTE	m_btChatPrefixLen;	// 格式控制字符长度
	WORD	m_wSentenceLen;		// 聊天语句长度
	UINT	m_dwTargetID;		// 聊天对象 id
	int		m_nTargetIdx;		// 聊天对象在服务器端的 idx
	char	m_szSentence[MAX_SENTENCE_LENGTH + CHAT_MSG_PREFIX_MAX_LEN];	// 聊天语句内容
	PLAYER_SEND_CHAT_DATA_COMMAND() {memset(m_szSentence, 0, sizeof(m_szSentence));};
} PLAYER_SEND_CHAT_COMMAND;		// 客户端聊天内容发送给服务器

typedef struct PLAYER_SEND_CHAT_DATA_SYNC
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_wLength;
	BYTE	m_btCurChannel;		// 当前聊天状态
	BYTE	m_btNameLen;		// 名字长度
	BYTE	m_btChatPrefixLen;	// 控制字符长度
	WORD	m_wSentenceLen;		// 聊天语句长度
	UINT	m_dwSourceID;		// 
	char	m_szSentence[32 + CHAT_MSG_PREFIX_MAX_LEN + MAX_SENTENCE_LENGTH];	// 聊天语句内容
	PLAYER_SEND_CHAT_DATA_SYNC() { memset(m_szSentence, 0, sizeof(m_szSentence)); };
} PLAYER_SEND_CHAT_SYNC;		// 客户端聊天内容发送给服务器

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btState;
	int		m_nID;
	int		m_nDataID;
	int		m_nXpos;
	int		m_nYpos;
	int		m_nMoneyNum;
	int		m_nItemID;
	BYTE	m_btDir;
	BYTE	m_btItemWidth;
	WORD	m_wCurFrame;
	BYTE	m_btItemHeight;
	int 	m_btColorID;
	BYTE	m_btFlag;
	int     m_Genre;
	int     m_DetailType;
	int     m_ParticularType;
	int     m_GoldId;
	int     m_ItemLevel;
	int     m_StackNum;
	char	m_szName[FILE_NAME_LENGTH];
} OBJ_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	//WORD    m_wLength;
	int		m_nID;
	//int		m_nDataID;
	//int		m_nItemID;
	char	_clientOgjName[FILE_NAME_LENGTH];                   //用于翻译的名称
} OBJ_ADD_SYNC_VN;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btState;
	int		m_nID;
} OBJ_SYNC_STATE;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btDir;
	int		m_nID;
} OBJ_SYNC_DIR;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	BYTE	m_btSoundFlag;
} OBJ_SYNC_REMOVE;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	int		m_nTarX;
	int		m_nTarY;
} OBJ_SYNC_TRAP_ACT;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
} OBJ_CLIENT_SYNC_ADD;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	UINT	m_dwLeadExp;		// 统率力经验值
} PLAYER_LEAD_EXP_SYNC;			// 同步统率力经验值

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	WORD  	m_btLevel;			// 当前等级
	int		m_nExp;				// 当前经验
	int		m_nAttributePoint;	// 剩余属性点
	int		m_nSkillPoint;		// 剩余技能点
	UINT	m_nBaseLifeMax;		// 当前最大生命值
	UINT	m_nBaseStaminaMax;	// 当前最大体力值
	UINT	m_nBaseManaMax;		// 当前最大内力值
} PLAYER_LEVEL_UP_SYNC;			// 玩家升级

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_btLevel;			// 当前等级
	UINT	m_dwTeammateID;		// 队友 npc id
} PLAYER_TEAMMATE_LEVEL_SYNC;	// 玩家升级的时候通知队友

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nAddNo;			// 加的点数
} PLAYER_ADD_BASE_ATTRIBUTE_COMMAND;	// 玩家添加基本属性点

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nSkillID;			// 技能id
	int		m_nAddPoint;		// 要加的点数
} PLAYER_ADD_SKILL_POINT_COMMAND;// 玩家申请增加某个技能的点数

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nBasePoint;		// 基本点数
	int		m_nCurPoint;		// 当前点数
	int		m_nLeavePoint;		// 剩余未分配属性点
} PLAYER_ATTRIBUTE_SYNC;		// 玩家同步属性点

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nSkillID;			// 技能id
	int		m_nSkillLevel;		// 技能等级
	int		m_nLeavePoint;		// 剩余未分配技能点
	int		m_nAddPoint;		// 装备添加的技能点
	int     m_Type;
} PLAYER_SKILL_LEVEL_SYNC;		// 玩家同步技能点


typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BOOL	m_nIsOpen;			// 是否开启
	int		m_nDianShu;		    // 点数
	int		m_nYinL;		    // 世界银两
	int		m_nJinBi;		    // 世界金币
	int     m_Type;             // 甩股类型
	int     m_Beilv;
} PLAYER_SYNCALLBACK_GUZI;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nItemID;			// 物品id
	int		m_nSourcePos;		// 来源位置
	int		m_nTargetPos;		// 目的位置
} PLAYER_EQUIP_ITEM_COMMAND;	// 玩家鼠标右键点击使用物品(装备)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPlace;			// 药品位置
	BYTE	m_btX;				// 药品在容器中位置
	BYTE	m_btY;				// 药品在容器中位置
	int		m_nItemID;			// 物品id
} PLAYER_EAT_ITEM_COMMAND;		// 玩家鼠标右键点击使用物品(吃药)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPlace;			// 药品位置
	BYTE	m_btX;				// 药品位置
	BYTE	m_btY;				// 药品位置
	unsigned int m_nItemID;		// 物品id
	int     m_destRoom;         // 目标容器
	int     m_ItemWidth;
	int     m_ItemHigth;
} PLAYER_AUTOMUVE_ITEM_COMMAND;		// 玩家鼠标右键点击使用物品(吃药)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nObjID;			// 鼠标点击的obj的id
	int 	m_btPosType;		// 位置类型
	int 	m_btPosX;			// 坐标 x
	int 	m_btPosY;			// 坐标 y
} PLAYER_PICKUP_ITEM_COMMAND;	// 玩家获得物品（鼠标点击地图上的obj）     int		m_nColorID;         //名字颜色 0白色  1蓝色  3黄色 4紫色 
/*
struct sMagInfo
{
	int nMagType;
	int nMagVala;  //6个v0 值
	int nMagValb;  //6个v1 值
	int nMagValc;  //6个v2 值
};

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE            nRoomType;
    int             nItemIdx;
	int             nOldx;
	int             nOldy;
	int             nCurX;
	int             nCurY;

}ITEM_SYNC_POSITIONS; 
*/
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	int 			m_Genre;			// 物品的类型
	int			    m_Detail;			// 物品的类别
	int				m_Particur;			// 物品的详细类别
	int 			m_Series; 			// 物品的五行
	int 			m_Level;			// 物品的等级
	int 			m_btPlace;			// 容器的类型
	int			    m_btX;				// 坐标
	int 			m_btY;				// 坐标
	int				m_Luck;				// MF
	int				m_MagicLevel[6];	// 魔法生成参数
	int				m_RongMagicLev[6];	// 熔炼生成参数
//	int		        m_BaoMagicLev[6];
	int             m_JBLevel[7];
	WORD			m_Version;			// 装备版本
	int 			m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	int				m_GoldId;			// 黄金Id
	int				m_StackNum;
	int 			m_EnChance;
	int 			m_Point;
	KTime			m_Time;
    int 	        m_Rongpiont;//  熔炼
	int             m_IsBang;
	int             m_IsKuaiJie;
	int             m_IsMagic;  
	int             m_SkillType; 
	int             m_MagicID; 
	char            m_ItmeInfo[516];
	//char            *m_ItmeInfo;
	char            m_WonName[32]; 
//	sMagInfo        m_MagInfo[6];
	int             m_IsWhere;
	int             m_SyncType;
	int             m_IsCanUse;
	BYTE            m_IsLoign;
	int             m_IsPlatima;
    int             m_UseMap;
	int             m_Res;
	int             m_UseKind;
} ITEM_SYNC;

typedef struct
{
   BYTE			ProtocolType;		// 协议类型
   int          nShopIdx; //ROWS
   int          nBuyIdx;  //COLS
   char         m_ItmeInfo[516];

}ITEM_SYNC_SHOP;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	int             m_model;
	int             m_dx;
	int             m_dy;
} ITEM_REMOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	int             m_ClinetKind;       // 客户端的类型
} PLAYER_SELL_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_BuyIdx;			// 买第几个东西
	BYTE			m_Place;			// 放在身上哪个地方
	BYTE			m_X;				// 坐标X
	BYTE			m_Y;				// 坐标Y
	int             m_shuishou;
	int             m_CityMenber;
    int             m_MapID;
	int             m_ItemNum;
} PLAYER_BUY_ITEM_COMMAND;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_TaskId;			// 任务ID号
	UINT           m_pDwid;
	int             m_TaskVal;
}PLAYER_SET_TASKVAL_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nKind;
	int		nVala;
	int		nValb;
	int		nValc;
} CLIENT_RUN_SCRIPT;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_TaskId;			// 任务ID号
	UINT           m_pDwid;

}PLAYER_GET_TASKVAL_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_TaskId;			// 任务ID号
	UINT           m_pDwid;
	int             m_TaskVal;
	
}PLAYER_GETBACK_TASKVAL;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int             m_TaskVala;
	int             m_TaskValb;
	int             m_TaskValc;
	int             m_TaskVald;
	int             m_TaskVale;
	int             m_TaskValf;
	int             m_TaskValg;
	int             m_TaskValh;
	
}PLAYER_JINMAI_GETBACK;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT           m_NpcDwid;
	int             m_TaskVala;
	int             m_TaskValb;
	int             m_TaskValc;
	int             m_TaskVald;
	int             m_TaskVale;
	int             m_TaskValf;
	int             m_TaskValg;
	int             m_TaskValh;
	int             m_TaskVali;
	int             m_TaskValj;
	
}NPC_SYN_INFO_GETBACK;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney1;			// 装备栏
	int				m_nMoney2;			// 贮物箱
	int				m_nMoney3;			// 交易栏
} PLAYER_FS_MONEY_SYNC;					// 服务器通知客户端钱的数量

/*typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE            m_CurDir;
	
}NPC_DIR_SET; */

typedef struct
{
	BYTE			ProtocolType;	
	int				m_nXu;					
} PLAYER_FS_XU_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int			    m_btDownPos;
	int			    m_btDownX;
	int			    m_btDownY;
	int			    m_btUpPos;
	int			    m_btUpX;
	int			    m_btUpY;
	int             m_DownRongqi;
	int             m_UpRongqi;
	int             m_IsComp;
} PLAYER_MOVE_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_btDownPos;
	int			    m_btDownX;
	int			    m_btDownY;
	int			    m_btUpPos;
	int			    m_btUpX;
	int			    m_btUpY;
	int             m_DownRongqi;
	int             m_UpRongqi;
	int             m_IsMianBan;
} PLAYER_MOVE_ITEM_SYNC;

// s2c_ItemAutoMove
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT            m_ItenDwid;
	int 			m_btSrcPos;
	int			    m_btSrcX;
	int			    m_btSrcY;
	int			    m_btDestPos;
	int			    m_btDestX;
	int		        m_btDestY;
	int             m_DisRongqi;
    int             m_SisRongqi;
} ITEM_AUTO_MOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT            m_ItemDwId;
	int			    m_btSrcX;
	int			    m_btSrcY;
	int			    m_btDestX;
	int		        m_btDestY;
	int             m_Dplace;
    int             m_Splace;
	int             m_Kind;
}PLAYER_AUTOCALLBACK_EQUIP;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int             m_Kind;
	UINT           m_ItemDwidx;
	BYTE            m_IsAuToFenJie;
	int             m_isAttack;
} PLAYER_THROW_AWAY_ITEM_COMMAND;

typedef struct
{
	BYTE		ProtocolType;		// 协议类型
	int 			m_shLife;
	int 			m_shStamina;
	int 			m_shMana;
	//short	    m_shAngry;          //当前怒值
	BYTE		   m_btTeamData;
	int 			m_shLifeMax;
	int 			m_shStaminaMax;
	int 			m_shManaMax;
    int 	     m_shAngryMax;
	BYTE         m_nForbiddenFlag;
} CURPLAYER_NORMAL_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	// npc部分
	UINT			m_dwID;				// Npc的ID
	WORD  			m_btLevel;			// Npc的等级
	BYTE			m_btSex;			// 性别
	BYTE			m_btKind;			// Npc的类型
	BYTE			m_btSeries;			// Npc的五行系
	UINT			m_wLifeMax;			// Npc的最大生命
	UINT			m_wStaminaMax;		// Npc的最大体力
	UINT			m_wManaMax;			// Npc的最大内力
	int				m_HeadImage;
	// player 部分
	WORD			m_wAttributePoint;	// 未分配属性点
	WORD			m_wSkillPoint;		// 未分配技能点
	WORD			m_wStrength;		// 玩家的基本力量（决定基本伤害）
	WORD			m_wDexterity;		// 玩家的基本敏捷（决定命中、体力）
	WORD			m_wVitality;		// 玩家的基本活力（决定生命、体力）
	WORD			m_wEngergy;			// 玩家的基本精力（决定内力）
	WORD			m_wLucky;			// 玩家的基本幸运值
	int 			m_nExp;				// 当前经验值(当前等级在npc身上)
	UINT			m_dwLeadExp;		// 统率力经验值
	BYTE 			m_btCUnlocked;      //是否上锁

	// 门派
	BYTE			m_btCurFaction;		// 当前门派
	BYTE			m_btFirstFaction;	// 第一次加入的是哪个门派
	int				m_nFactionAddTimes;	// 加入各种门派的总次数
	
	// 排名
	WORD			m_wWorldStat;		// 世界排名
	WORD			m_wSectStat;		// 门派排名

	// 钱
	int				m_nMoney1;
	int				m_nMoney2;
	int				m_nXu;

	int				m_nRequte;
	int				m_nFuYuan;
	int             m_wNuqiMax;
    //int             m_nForbiddenFlag;      //是否给禁言
} CURPLAYER_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BOOL            m_IsLogin;
	unsigned int    m_clientKey;
}CLIENT_SYNC_END;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int iid;        //编号idx 1开始
	int iequipgen;  //
	int idetailtype;
	int iparticulartype;
	char itemName[64];//物品的名称
	int ilevel;     //等级
	int iseries;    //武行
	UINT irandseed; //随机种子
	int iparam2;    //蓝装
	int iparam3;
	int iparam5;
	int iparam4;
	int iparam6;
	int iparam1;
	int iparamr2;   //熔炼
	int iparamr3;
	int iparamr5;
	int iparamr4;
	int iparamr6;
	int iparamr1;
	int ilucky;   //装备的幸运值
	int igoldid;  //是否黄金
	int	istacknum;//叠加数量
	int ienchance;//加成
	BYTE ipoint;  //是否紫装 7为特殊装 8 为随机黄金   1-6 为紫装
	UINT addtime; //开始时间
	UINT endtime; //结束时间
	BYTE iPriceKind;   //货币类型
	int  iSellPrice;   //售卖价格
	BYTE iRongpiont;   //是否熔炼
	char iWonName[32]; //挂售者
	char iaccName[32]; //帐号
	int iswhere;    //是否洗练装
	int isplatina;  //是否白金
	UINT isSellOut; //是否已经卖出
	int iparamj1;   //(保留使用)
	int iparamj2;
	UINT iparamj3;
	int iparamj5;
	int iparamj4;
	int iparamj6;
	int iparamj7;
}ITEMDATA_SYNC_END;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// Npc的ID
	int				m_nRequte;
	int				m_nFuYuan;
}CURPLAYER_TASKINFO_SYNC;

#define MAX_SCIRPTACTION_BUFFERNUM 1024

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wProtocolLong;
	BYTE	m_nOperateType;				//操作类型
	BYTE	m_bUIId, m_bOptionNum, m_bParam1, m_bParam2, m_Select;// m_bParam1,主信息是数字标识还是字符串标识, m_bParam2,是否是与服务器交互的选择界面
	int		m_nParam;
	int		m_nBufferLen;
	char	m_szSprPath[128];
	char	m_pContent[MAX_SCIRPTACTION_BUFFERNUM];				//带控制符
} PLAYER_SCRIPTACTION_SYNC;

typedef struct
{
	WORD	SkillId;
	BYTE	SkillLevel;
	BYTE	SkillAdd;
	int     CurSkillExp;
} SKILL_SEND_ALL_SYNC_DATA;

typedef struct
{
	BYTE						ProtocolType;
	WORD						m_wProtocolLong;
	SKILL_SEND_ALL_SYNC_DATA	m_sAllSkill[80];
} SKILL_SEND_ALL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	WeatherID;
} SYNC_WEATHER;


typedef struct defWORLD_SYNC
{
	BYTE	ProtocolType;
	int		SubWorld;
	int		Region;
	BYTE	Weather;
	UINT	Frame;
	int     SRegion;
	char WarMaster[32];       //城主
	char WarTong[32];         //占领帮会
	char WarGongTong[32]; //攻
	char WarShouTong[32]; //守
	BYTE WarIsWho;
	int  mShuiType;    //税收的类型
	BYTE IsWarCity;    // 是否被占领
	int WarCityManey;  //城市建设资金
	int WarCityJB;     //城市建设资金
	int WarCityGX;     //城市建设贡献
	int WPKFlag;       //地图的pk模式设置
	BOOL m_IsShowLoop;
	int m_GameStat;
} WORLD_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nSelectIndex;
}PLAYER_SELECTUI_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwTakeChannel;	// 订阅频道
} CHAT_SET_CHANNEL_COMMAND;				// 设定订阅频道

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	UINT			m_dwTargetNpcID;	// 目标 npc id
	char			m_szInfo[MAX_SENTENCE_LENGTH];// 给对方的话
} CHAT_APPLY_ADD_FRIEND_COMMAND;		// 聊天添加好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nSrcPlayerIdx;	// 来源 player idx
	char			m_szSourceName[32];	// 来源玩家名字
	char			m_szInfo[MAX_SENTENCE_LENGTH];// 对方给的话
} CHAT_APPLY_ADD_FRIEND_SYNC;			// 聊天添加好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 被接受player idx
} CHAT_ADD_FRIEND_COMMAND;				// 添加某玩家为聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 被拒绝player idx
} CHAT_REFUSE_FRIEND_COMMAND;			// 拒绝添加某玩家为聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 新添加好友的 id
	int				m_nIdx;				// 新添加好友在 player 数组中的位置
	char			m_szName[32];		// 新添加好友的名字
} CHAT_ADD_FRIEND_SYNC;					// 通知客户端成功添加一个聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	char			m_szName[32];		// 拒绝者名字
} CHAT_REFUSE_FRIEND_SYNC;				// 通知客户端添加聊天好友的申请被拒绝

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 出错 player idx (一般可能是此player下线或者换服务器了)
} CHAT_ADD_FRIEND_FAIL_SYNC;			// 通知客户端添加聊天好友失败

typedef struct
{
	BYTE			ProtocolType;	    // c2s_viewequip  查看装备信息
	UINT			m_dwNpcID;
	BYTE			m_bPrcess;
} VIEW_EQUIP_COMMAND;
	

//此结构已经被tagDBSelPlayer结构替换
/*typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nSelect;			// 
} DB_PLAYERSELECT_COMMAND;*/

/*
 * { Add by liupeng 2003.05.10
 *
 * #pragma pack( push, 1 )
*/

/*
 * Nonstandard extension used : zero-sized array in struct/union
 */
#pragma warning(disable: 4200)

#define KSG_PASSWORD_MAX_SIZE   64

typedef struct tagKSG_PASSWORD
{
    char szPassword[KSG_PASSWORD_MAX_SIZE];    // 现在采用MD5的字符串，由于是32个字符，加上末尾'\0'，需要至少33个空间，因此使用64
} KSG_PASSWORD;

#define _NAME_LEN	32

struct tagProtoHeader
{
	BYTE	cProtocol;
};

/*struct tagResult : public tagProtoHeader
{
	BYTE	cResult;
}; */

struct tagDBSelPlayer : public tagProtoHeader
{
	char	szRoleName[_NAME_LEN];
};

struct tagDBDelPlayer : public tagProtoHeader
{
	char	        szAccountName[_NAME_LEN];
    KSG_PASSWORD    Password;
	char	        szRoleName[_NAME_LEN];
};

//删除与新建角色的返回消息带的数据
struct tagNewDelRoleResponse : public tagDBSelPlayer
{
	bool	bSucceeded;		//是否成功
};

// 2003.05.11
/*struct tagDBSyncPlayerInfo : public tagProtoHeader
{
	size_t	dataLength;
	char	szData[0];
}; */

/*
 * 2003.06.27
 * s2c_gateway_broadcast
*/

#define	AP_WARNING_ALL_PLAYER_QUIT			1
#define	AP_NOTIFY_GAMESERVER_SAFECLOSE		2
#define	AP_NOTIFY_ALL_PLAYER				3
//#define	AP_NOTIFY_WELCOME_MSG				4   //欢迎语
#define	MAX_GATEWAYBROADCAST_LEN	260
struct tagGatewayBroadCast : public tagProtoHeader
{
	UINT	uCmdType;
	BOOL    iSavePai;
	char	szData[MAX_GATEWAYBROADCAST_LEN];
};

/*
 * 2003.05.22
 * s2c_syncgamesvr_roleinfo_cipher
*/
struct tagGuidableInfo : public tagProtoHeader
{
	GUID guid;
	UINT nExtPoint;			//可用的附送点
	int  nChangePoint;		    //变化的附送点
	UINT nLeftTime;            //
	int  nGameTime;            //变化的时间
    int   nVipType;
	UINT	datalength;
	char	szData[0];
};

/*
 * c2s_permitplayerlogin
 */
struct tagPermitPlayerLogin : public tagProtoHeader
{
	GUID guid;

	BYTE szRoleName[_NAME_LEN];
	BYTE szAccountName[_NAME_LEN];
	char strRoleNmae[_NAME_LEN];
	/*
	 * Succeeded : true
	 * Failed	 : false
	 */
	bool bPermit;
};

struct tagPermitPlayerExchange
{
	BYTE cProtocol;
	GUID guid;
	UINT dwIp;
	WORD wPort;
	bool bPermit;
};
/*
 * c2s_notifyplayerlogin
 */
struct tagNotifyPlayerLogin : public tagPermitPlayerLogin
{
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * s2c_querymapinfo
 */
struct tagQueryMapInfo : public tagProtoHeader
{	
};

/*
 * s2c_querygameserverinfo
 */
struct tagQueryGameSvrInfo : public tagProtoHeader
{	
};

/*
 * s2c_notifysvrip
 */
struct tagNotifySvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	UINT	dwMapID;

	UINT	dwSvrIP;
};

/*
 * s2c_notifyplayerexchange
 */
struct tagNotifyPlayerExchange : public tagProtoHeader
{
	GUID			guid;
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * c2s_requestsvrip
 */

/*
 * BYTE	cIPType
 */
#define INTRANER_IP	0
#define INTERNET_IP 1

struct tagRequestSvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	UINT	dwMapID;
};

/*
 * c2c_notifyexchange
 */
struct tagSearchWay : public tagProtoHeader
{
	int		lnID;
	int		nIndex;
	UINT	dwPlayerID;
};

/*
 * c2s_updatemapinfo
 */
struct tagUpdateMapID : public tagProtoHeader
{
	/*
	 * For example : Are your clear older information when it 
	 *		update local informatin
	 */
	/*BYTE cReserve;
	BYTE cMapCount;
	BYTE szMapID[0];*/

	UINT cMapCount;
};

/*
 * c2s_updategameserverinfo
 */
struct tagGameSvrInfo : public tagProtoHeader
{
	UINT			nIPAddr_Intraner;
	UINT			nIPAddr_Internet;
	unsigned short	nPort;
	WORD			wCapability;
};

/*
 * s2c_identitymapping
 */
/*struct tagIdentityMapping : public tagGameSvrInfo
{
	GUID guid;
}; */

/*
 * c2s_logiclogin
 * s2c_gmgateway2relaysvr
 * s2c_gmnotify
 */
struct tagLogicLogin : public tagProtoHeader
{
	GUID guid;
};

/*
 * s2c_logiclogout
 */
struct tagLogicLogout : public tagProtoHeader
{
	BYTE szRoleName[_NAME_LEN];
};

/*
 * c2s_registeraccount
 */
struct tagRegisterAccount : public tagProtoHeader
{
	BYTE szAccountName[_NAME_LEN];
};

/*
 * c2s_entergame
 */
struct tagEnterGame : public tagProtoHeader
{
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */

	BYTE szAccountName[_NAME_LEN];  //账号名
};

struct tagEnterGame2 : public EXTEND_HEADER
{
	char szAccountName[_NAME_LEN];
	char szCharacterName[_NAME_LEN];
	UINT	dwNameID;
	unsigned int	lnID;
	unsigned int    nSelSerVer;
};

/*
 * c2s_leavegame
 */

/*
 * BYTE cCmdType
 */
#define NORMAL_LEAVEGAME	0x0		// lock account
#define HOLDACC_LEAVEGAME	0x1A	// clear resource but don't to unlock account 

struct tagLeaveGame : public tagProtoHeader
{
	BYTE cCmdType;
	int nExtPoint;        //将要改变的扩展点 正为加 负数为减
	int nGameExtTime;     //将要改变的时间 正为加 负数为减
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */
	char nClientIp[KSG_PASSWORD_MAX_SIZE];
	char szAccountName[_NAME_LEN];
};

struct tagMsgInGame : public tagProtoHeader
{
	BYTE cCmdType;
	int nSerIdx;        //服务器编号
	int nMgsIdx;        
	int nValIdx;
};

struct tagLeaveGame2 : public EXTEND_HEADER
{
	BYTE cCmdType;
	char szAccountName[_NAME_LEN];
	UINT nSelServer;   //哪个服务器的玩家退出游戏了 
};
/*
*  c2s_registerfamily
*/
/*struct tagRegisterFamily : public tagProtoHeader
{
	BYTE bRegister;		//1 is Register, 0 is unRegister
	BYTE nFamily;
	BYTE RelayMethod;
};*/

/*
 * c2s_gmsvr2gateway_saverole
 */
/*struct tagGS2GWSaveRole : public tagProtoHeader
{
	size_t	datalength;
	BYTE	szData[0];
}; */

/*
 * #pragma pack( pop )
 *
 * } End of the struct define
 */

typedef struct
{
	char	szName[32];
	BYTE	Sex;
	BYTE	Series;
//	BYTE	HelmType;
//	BYTE	ArmorType;
//	BYTE	WeaponType;
	BYTE	Level;
} RoleBaseInfo/* client */, S3DBI_RoleBaseInfo /* server */;

typedef struct
{
	BYTE				ProtocolType;
	RoleBaseInfo		m_RoleList[MAX_PLAYER_IN_ACCOUNT];
} ROLE_LIST_SYNC;

//移自RoleDBManager/kroledbheader.h
//用来替换上面的ROLE_LIST_SYNC,ROLE_LIST_SYNC结构不再需要了
struct TProcessData
{
	unsigned char	nProtoId;
	unsigned int	nDataLen;//TRoleNetMsg时表示该Block的实际数据长度,TProcessData时表示Stream的实际数据长度 size_t
	unsigned int	ulIdentity;//long
	bool			bLeave;
	UINT            uSelServer;     //当前所登陆的服务器
	char			pDataBuffer[1]; //实际的数据
};


struct TCheckNameData
{
	unsigned char	nProtoId;
	UINT			nDataLen;//TRoleNetMsg时表示该Block的实际数据长度,TProcessData时表示Stream的实际数据长度
	unsigned int	ulIdentity;
	char            nAccName[32];   //账号名
	char            nRoleName[32]; 
};


struct tagRoleEnterGame
{
	BYTE			ProtocolType;
	bool			bLock;
	char			Name[_NAME_LEN];
};

//新建角色的信息结构
//注释：新建决消息c2s_newplayer，传送的参数为TProcessData结构描述的数据，其中TProcessData::pDataBuffer要扩展为NEW_PLAYER_COMMAND
struct NEW_PLAYER_COMMAND
{
	BYTE			m_btRoleNo;			// 角色编号
	BYTE			m_btSeries;			// 五行系
	unsigned short	m_NativePlaceId;	// 出生地ID
	char			m_szName[32];		// 姓名
};


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 好友 id
	int				m_nPlayerIdx;		// 好友 player index
} CHAT_LOGIN_FRIEND_NONAME_SYNC;		// 玩家登录时发送玩家聊天好友数据（不带名字）

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	UINT			m_dwID;				// 好友 id
	int				m_nPlayerIdx;		// 好友 player index
	char			m_szName[32];		// 好友名字
} CHAT_LOGIN_FRIEND_NAME_SYNC;			// 玩家登录时发送玩家聊天好友数据（带名字）

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
} CHAT_APPLY_RESEND_ALL_FRIEND_NAME_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 好友 id
} CHAT_APPLY_SEND_ONE_FRIEND_NAME_COMMAND;	// 申请得到玩家某个聊天好友的完整数据

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 好友 id
	int				m_nPlayerIdx;		// 好友 player index
	char			m_szName[32];		// 好友名字
} CHAT_ONE_FRIEND_DATA_SYNC;			// 发送玩家某一个聊天好友数据（带名字）

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 好友 id
	int				m_nPlayerIdx;		// 好友 player index
} CHAT_FRIEND_ONLINE_SYNC;				// 通知客户端有好友上线

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 被删除id
} CHAT_DELETE_FRIEND_COMMAND;			// 删除某个聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 删除id
} CHAT_DELETE_FRIEND_SYNC;				// 被某个聊天好友删除

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 被删除id
} CHAT_REDELETE_FRIEND_COMMAND;			// 删除某个聊天好友
/*
typedef struct 
{
	BYTE			ProtocolType;
	BYTE			m_LogoutType;
} LOGOUT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			szAccName[32];
} LOGIN_COMMAND;  */

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;				// 好友 id
} CHAT_FRIEND_OFFLINE_SYNC;				// 通知客户端有好友下线

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} TRADE_APPLY_OPEN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
} TRADE_APPLY_CLOSE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btState;			// if == 0 close if == 1 open if == 2 trading
	UINT			m_dwNpcID;			// 如果是开始交易，对方的 npc id
} TRADE_CHANGE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	UINT			m_dwID;
	BYTE			m_btState;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} NPC_SET_MENU_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwID;
} TRADE_APPLY_START_COMMAND;

// 服务器转发交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nDestIdx;			// 申请者在服务器端的player idx
	UINT			m_dwNpcId;			// 申请者的 npc id
} TRADE_APPLY_START_SYNC;

// 接受或拒绝别人的交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_bDecision;		// 同意 1 不同意 0
	int				m_nDestIdx;			// 交易对方在服务器端的player idx
} TRADE_REPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MOVE_MONEY_COMMAND;		// c2s_trademovemoney

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MONEY_SYNC;				// s2c_trademoneysync

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 确定交易 1  退出交易 0  取消确定 4  锁定交易 2  取消锁定 3
} TRADE_DECISION_COMMAND;				// 交易执行或取消 c2s_tradedecision


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	bool			m_isout;		    // 是否退出
} KICK_OUT_GAME_COMMAND;				// 交易执行或取消 c2s_tradedecision


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 交易ok 1  交易取消 0  锁定 2  取消锁定 3
} TRADE_DECISION_SYNC;					// s2c_tradedecision

typedef struct
{
	BYTE			ProtocolType;		
	BYTE			m_byDir;			// 取钱的方向（0存，1取）
	UINT			m_dwMoney;			// 钱数
} STORE_MONEY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;			// 长度
	BYTE			m_btError;			// 错误类型	0 对方关闭了此频道，1 找不到对方
	char			m_szName[32];		// 对方名字
} CHAT_SCREENSINGLE_ERROR_SYNC;

typedef struct 
{
	BYTE		ProtocolType;		// 协议类型
	int			m_btStateInfo[MAX_NPC_RECORDER_STATE];  //BYTE
	UINT			m_ID;				// Npc的GID
}	NPC_SYNC_STATEINFO;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_dwNpcID;
} TEAM_INVITE_ADD_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;			// 长度
	int				m_nIdx;             //玩家NPC索引
	char			m_szName[32];
} TEAM_INVITE_ADD_SYNC;

typedef struct
{
	BYTE			ProtocolType;		//
	int				m_nAuraSkill;
} SKILL_CHANGEAURASKILL_COMMAND;		//更换光环技能

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btResult;
	int				m_nIndex;
} TEAM_REPLY_INVITE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSelfLock;
	BYTE			m_btDestLock;
	BYTE			m_btSelfOk;
	BYTE			m_btDestOk;
} TRADE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	UINT			m_dwSkillID;		// 技能
	int				m_nLevel;           //等级
	int				m_nTime;			// 时间
	int             m_IsEuq;
	KMagicAttrib	m_MagicAttrib[MAX_SKILL_STATE];
} STATE_EFFECT_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	UINT			m_dwSkillID;		// 技能
	int				m_nLevel;           //等级
	int				m_nTime;			// 时间
	int             m_IsEuq;
}STATE_NODATA_EFFECT_SYNC;


typedef struct
{
	BYTE			ProtocolType;
	UINT			m_dwTime;
} PING_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	UINT			m_dwReplyServerTime;
	UINT			m_dwClientTime;
} PING_CLIENTREPLY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSitFlag;
} NPC_SIT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
} NPC_RIDE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMapID;   
	int		nMpsX;
	int		nMpsY;
	int     nMap;
	int     nDwid;
}NPC_SUNYI_COMMAND; //瞬移协议


typedef struct
{
	BYTE	ProtocolType;
	int     nmapid ;
    int     ndianshu;
	int     nNetConnectIdx;
    int     nZuoWeiHao;
}CP_GUZI;


typedef struct
{
	BYTE	ProtocolType;
	int     nmapid ;
    int     nFarmsA ;
    int     nFarmsB;
}CP_GUZI_FARMS;


typedef struct
{
	BYTE	ProtocolType;
	int     nJinBi ;
}CP_ADD_JINBI;

typedef struct
{
	BYTE	ProtocolType;
	char m_PLname[32];
    int  m_ItemDwid;
	int  m_bLPrcess;
}VIEW_LIANJIE_COMMAND;


typedef struct
{
	BYTE	ProtocolType;
	int     ntype;
    int     nlevel;
}CP_JINGMAI;


typedef struct
{
	BYTE	ProtocolType;
    int  GuaType;
}NPC_GUAFORBIT_COMMAND;


typedef struct
{
	BYTE	ProtocolType;
	int     m_nLeftskill;
    INT     m_Type;
}SKILL_LEFT_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				nMpsX;
	int				nMpsY;
} NPC_JUMP_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	int				m_dwRegionID;
	int				m_nObjID;
} OBJ_MOUSE_CLICK_SYNC;

typedef struct tagSHOW_MSG_SYNC
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	//LPVOID			m_lpBuf;
	DWORD           m_lpBuf;
	char            m_Mmsgs[125];
	//tagSHOW_MSG_SYNC() {m_lpBuf = NULL;};
	//~tagSHOW_MSG_SYNC() {Release();}
   //void	Release() {if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}
} SHOW_MSG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_APPLY_NORMAL_FLAG_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_NORMAL_FLAG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	UINT			m_dwNpcID;
} PK_APPLY_ENMITY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	UINT			m_dwNpcID;
	char			m_szName[32];
} PK_ENMITY_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	UINT			m_dwNpcID;
	char			m_szName[32];
} PK_EXERCISE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nPKValue;
} PK_VALUE_SYNC;
//查看别人装备
struct SViewItemInfo
{
	int				m_ID;				// 物品的ID
	BYTE			m_Genre;			// 物品的类型
	BYTE 			m_Detail;			// 物品的类别
	BYTE 			m_Particur;			// 物品的详细类别
	BYTE			m_Series; 			// 物品的五行
	BYTE 			m_Level;			// 物品的等级
	int  			m_Luck;				// MF
	int  			m_MagicLevel[6];	// 魔法生成参数
    //int  			m_RongMagicLev[6];	// 熔炼生成参数
	//int		    m_BaoMagicLev[6];
	//int           m_JBLevel[7];
	WORD			m_Version;			// 装备版本
	//BYTE 			m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	UINT  			m_GoldId;			// 黄金Id
	UINT  			m_IsBaiJin;
	BYTE    		m_EnChance;
	BYTE  			m_Point;
	KviewTime	    m_Time;
	//int             m_Year;
    BYTE 	        m_Rongpiont;//  熔炼
	//BYTE            m_IsBang;
	//BYTE             m_IsKuaiJie;
	//BYTE             m_IsMagic;  
	//BYTE             m_SkillType; 
	//BYTE             m_MagicID; 
	//int              m_IsWhere;
	BYTE             m_IsCanUse;
	//char            m_WonName[32];
};
/*
typedef struct
{
	int				m_ID;				// 物品的ID
	BYTE			m_Genre;			// 物品的类型
	BYTE			m_Detail;			// 物品的类别
	BYTE			m_Particur;			// 物品的详细类别
	BYTE			m_Series; 			// 物品的五行
	BYTE			m_Level;			// 物品的等级
//	BYTE			m_btPlace;			// 坐标
//	BYTE			m_btX;				// 坐标
//	BYTE			m_btY;				// 坐标
	BYTE			m_Luck;				// MF
	int  			m_MagicLevel[6];	// 魔法生成参数
    int  			m_RongMagicLev[6];	// 熔炼生成参数
//	int		        m_BaoMagicLev[6];
//	int             m_JBLevel[7];
	WORD			m_Version;			// 装备版本
	BYTE			m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	BYTE			m_GoldId;			// 黄金Id
	BYTE			m_StackNum;
	BYTE			m_EnChance;
	BYTE			m_Point;
	KTime			m_Time;
    BYTE	        m_Rongpiont;//  熔炼
	BYTE             m_IsBang;
	BYTE             m_IsKuaiJie;
	BYTE             m_IsMagic;  
	BYTE             m_SkillType; 
	BYTE             m_MagicID; 
	//	char            m_ItmeInfo[516]; 
//	char            m_WonName[32]; 
	//char          m_DescInfo[516];
	BYTE            m_IsWhere;
//	int             m_SyncType;
	BYTE            m_IsCanUse;
//	int             m_Count;
//	char            nShopName[32];
//	int		        m_nIdx;
//	UINT	        m_uPrice;
}SViewItemInfo;
*/
typedef struct
{
	BYTE			ProtocolType;
	UINT			m_dwNpcID;
	SViewItemInfo	m_sInfo[itempart_num]; //itempart_num
}VIEW_EQUIP_SYNC;  // s2c_viewequip

typedef struct//该结构是所统计的玩家的基本数据
{
	char	Name[20];
	int		nValue;
	BYTE	bySort;
}TRoleList;

// 游戏统计结构
typedef struct
{
	TRoleList MoneyStat[10];			//金钱最多排名列表（十个玩家，最多可达到100个）
	TRoleList XuStat[10];
	TRoleList LevelStat[10];			//级别最多排名列表（十个玩家，最多可达到100个）
	TRoleList KillerStat[10];			//杀人最多排名列表
	
	//[门派号][玩家数]，其中[0]是没有加入门派的玩家
	TRoleList MoneyStatBySect[11][10];	//各门派金钱最多排名列表
	TRoleList XuStatBySect[11][10];
	TRoleList LevelStatBySect[11][10];	//各门派级别最多排名列表
	
	//[门派号]，其中[0]是没有加入门派的玩家
	int SectPlayerNum[11];				//各个门派的玩家数
	int SectMoneyMost[11];				//财富排名前一百玩家中各门派所占比例数
	int SectXuMost[11];
	int SectLevelMost[11];				//级别排名前一百玩家中各门派所占比例数
}  TGAME_STAT_DATA;


typedef struct
{
	BYTE	ProtocolType;
	BYTE	bSleep;
	UINT	NpcID;
} NPC_SLEEP_SYNC;

//////////////
//排名相关
typedef struct
{
	BYTE		ProtocolType;
	UINT		dwLadderID;
	TRoleList	StatData[10];
} LADDER_DATA;

typedef struct
{
	BYTE		ProtocolType;
	WORD		wSize;
	int			nCount;
	UINT		dwLadderID[0];
} LADDER_LIST;

typedef struct
{
	BYTE		ProtocolType;
	UINT		dwLadderID;
} LADDER_QUERY;


///////////////////
//chat 相关

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	UINT	packageID;
	char	someone[_NAME_LEN];
	UINT    nItemDwid;
	BYTE	sentlen;
} CHAT_SOMEONECHAT_CMD, CHAT_SOMEONECHAT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	UINT	packageID;
	BYTE	filter;
	UINT	channelid;
	UINT    nItemDwid;
	BYTE	cost;	//0: 无限制，1: 10元/句，2: <10Lv ? 不能说 : MaxMana/2/句, 3: MaxMana/10/句,4: <20Lv ? 不能说 : MaxMana*4/5/句
	BYTE	sentlen;
} CHAT_CHANNELCHAT_CMD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	UINT	packageID;
	char	someone[_NAME_LEN];
	UINT	channelid;
	UINT	nItemDwid;
	BYTE	sentlen;
} CHAT_CHANNELCHAT_SYNC;

enum {codeSucc, codeFail, codeStore};
typedef struct
{
	BYTE	ProtocolType;
	UINT	packageID;
	BYTE	code;
} CHAT_FEEDBACK;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
} CHAT_EVERYONE;


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
	BYTE	byHasIdentify;
	WORD	wPlayerCount;
//	UINT   nItemDwid;
} CHAT_GROUPMAN;   ////城市 和 世界 ，队伍 门派


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	UINT	nameid;
	unsigned int lnID;
	WORD	wChatLength;
} CHAT_SPECMAN;


enum { tgtcls_team, tgtcls_fac, tgtcls_tong, tgtcls_scrn, tgtcls_bc,tgtcls_msgr};
typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	UINT	nFromIP;
	UINT	nFromRelayID;
	UINT	channelid;
	//UINT	nItemDwid;
	BYTE	TargetCls;
	UINT	TargetID;  //网路号
	WORD	routeDateLength;
} CHAT_RELEGATE;


///////////////////////////////////////
// tong 相关

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} S2C_TONG_HEAD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} STONG_PROTOCOL_HEAD;

typedef struct
{
	int		m_nPlayerIdx;
	int		m_nCamp;
	char	m_szTongName[defTONG_NAME_MAX_LENGTH + 1];
} STONG_SERVER_TO_CORE_APPLY_CREATE;

typedef struct
{
	int		m_nPlayerIdx;
	UINT	m_dwNpcID;
} STONG_SERVER_TO_CORE_APPLY_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	UINT	m_dwNameID;
} STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION;

typedef struct
{
	int		m_nCamp;
	int		m_nPlayerIdx;
	UINT	m_dwPlayerNameID;
	char	m_szTongName[64];
} STONG_SERVER_TO_CORE_CREATE_SUCCESS;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	UINT	m_dwNameID;
} STONG_SERVER_TO_CORE_REFUSE_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nInfoID;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_GET_INFO;

typedef struct
{
	int		m_nCamp;
	int		m_nPlayerIdx;
	UINT	m_dwPlayerNameID;
	char	m_szTongName[32];
	char	m_szMasterName[32];
	char	m_szTitleName[32];
} STONG_SERVER_TO_CORE_ADD_SUCCESS;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];
	char	m_szName[32];
} STONG_SERVER_TO_CORE_BE_INSTATED;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_BE_KICKED;

typedef struct
{
	int		m_nPlayerIdx;
	BOOL	m_bSuccessFlag;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_LEAVE;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	UINT	m_dwTongNameID;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	UINT	m_dwTongNameID;
	char	m_szTitle[32];
	char	m_szName[32];
} STONG_SERVER_TO_CORE_CHANGE_AS;

typedef struct
{
	UINT	m_dwTongNameID;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_CHANGE_MASTER;

typedef struct
{
	UINT	m_dwTongNameID;
	UINT	m_dwMoney;
	UINT	m_nMoney;
	BYTE		nType;
	int		m_nPlayerIdx;
} STONG_SERVER_TO_CORE_MONEY;

typedef struct
{
	UINT	m_dwParam;
	int		m_nFlag;
	int		m_nCamp;
	int		m_nFigure;
	int		m_nPos;
	char	m_szTongName[32];
	char	m_szTitle[32];
	char	m_szMaster[32];
	char	m_szName[32];
	UINT	m_nMoney;
	int     m_nTempCamp;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	char	m_szAttackName[defTONG_STR_LENGTH];		// 被宣战的帮会名
	int     m_AttackState;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	int     m_AttackTime;                           // 宣战的时长
	int     m_nDeathCount;
	int     m_nLevel;
} STONG_SERVER_TO_CORE_LOGIN;

// 玩家申请建立帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	char	m_szName[defTONG_NAME_MAX_LENGTH + 1];  //帮会名字
} TONG_APPLY_CREATE_COMMAND;

// 玩家申请加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwNpcID;
} TONG_APPLY_ADD_COMMAND;

// 玩家申请加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btCurFigure;
	BYTE	m_btCurPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szName[32];
} TONG_APPLY_INSTATE_COMMAND;

// 帮会建立失败 扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btFailId;
} TONG_CREATE_FAIL_SYNC;


// 帮会建立失败 扩展协议

struct CTONG_WAR_CITY_INFO
{
	char	m_szMapName[32];	//地名称图
	int 	m_idx;			 //索引
	int 	m_mapidx;		 //编号
	char	m_Tongmaster[32];//占领帮会名
	char	m_Mastername[32];//城主
	int     m_levle;		  //城市等级
	int     m_shushou;		  //城市税收
};

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwParam;	 //帮会的名称ID
	UINT	m_dwNpcID;
	int     m_nTempCamp;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	char	m_szAttackName[defTONG_STR_LENGTH];		// 宣战帮会名
	int     m_AttackState;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	int     m_AttackTime;                           // 宣战的时长
	int     m_WarCityCount;
	int     m_nLevel;
	int     m_YDeathCount;
	int     m_DDeathCount; 
	int     m_nAttackNum;	                        // 参战场数
	int     m_nWinNum;								// 胜利场数
	int     m_nLoseNum;								// 输掉场数
	int     m_nMapidx;                              // 帮会地图
//	CTONG_WAR_CITY_INFO snWarInfo[7];               // 城战的其他信息
} CTONG_ATTTACK_INFO_SYNC;

typedef struct
{
 BYTE	ProtocolType;
 WORD	m_wLength;
 BYTE	m_btMsgId;
 int    m_WarCityCount;
 CTONG_WAR_CITY_INFO snWarInfo[7];               // 城战的其他信息
}CTONG_CITY_INFO_SYNC;
// 转发加入帮会申请 扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;
	char	m_szName[32];
	int     m_IsAuToAdd;    //是否自动加入
} TONG_APPLY_ADD_SYNC;

// 通知玩家建立帮会成功 用普通协议
typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btCamp;  //阵营
	char	m_szName[defTONG_NAME_MAX_LENGTH + 1]; //帮会名
//	char	m_szMaster[32]; //帮主名
    BYTE	m_btlevle;
} TONG_CREATE_SYNC;  //有错

// 通知玩家加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	char	m_szTongName[32];
	char	m_szTitle[32];
	char	m_szMaster[32];
} TONG_Add_SYNC;

// 玩家申请解散帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} TONG_APPLY_DISMISS_COMMAND;

// 玩家帮会被解散 用普通协议
typedef struct
{
	BYTE	ProtocolType;
	int     m_nKind;
	int     m_inParma;
	int     m_inParmb;
	int     m_inParmc;
	int     m_inParmd;
} TONG_DISMISS_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;       //申请人的Playerid
	UINT	m_dwNameID;         //申请人的名字ID
	BYTE	m_btFlag;			// 是否接受 TRUE 接受 FALSE 不接受
} TONG_ACCEPT_MEMBER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btInfoID;
	int		m_nParam1;   // 请求的帮会名id
	int		m_nParam2;   // 请求开始的位置
	int		m_nParam3;   // 请求最大的数量
	char	m_szBuf[64];
} TONG_APPLY_INFO_COMMAND;

typedef struct
{
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];
	char	m_szName[32];
} TONG_ONE_LEADER_INFO;

typedef struct
{
	char	m_szName[32];
} TONG_ONE_MEMBER_INFO;

typedef struct
{
	char	m_szName[32];  //人物姓名
	int     m_inLevel;     //人物等级
} TONG_CALL_BACK_INFO;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwNpcID;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	WORD	m_btLevel;	  //帮会的等级
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	UINT	m_dwMemberNum;
	int     m_btApplyNum;
	int     m_btZhaoMuNum;
	char	m_szTongName[32];
	TONG_ONE_LEADER_INFO	m_sMember[1 + defTONG_MAX_DIRECTOR];
} TONG_HEAD_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	WORD	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	UINT	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szTongName[32];
	TONG_ONE_LEADER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MANAGER_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	WORD	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	UINT	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szTitle[32];
	char	m_szTongName[32];
	TONG_ONE_MEMBER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MEMBER_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	int     m_Count;
//	char	m_szTitle[32];
//	char	m_szTongName[32];
	TONG_ONE_MEMBER_INFO	m_sTongList[defTONG_ONE_PAGE_MAX_NUM];
} TONG_LIST_INFO_SYNC;

// 玩家自身在帮会中的信息 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btJoinFlag;
	BYTE	m_btFigure;
	BYTE	m_btCamp;
	char	m_szTongName[32];
	char	m_szTitle[32];
	char	m_szMaster[32];
	UINT	m_dwMoney;
	int     m_nAttackState;//宣战状态
    int	    m_AttackCamp;
    int     m_AttackTime;
    char    m_AttackName[32];
	int     m_nDeathCount;
	int     m_nLevel;
} TONG_SELF_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btOldFigure;
	BYTE	m_btOldPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szTitle[32];
	char	m_szName[32];
} TONG_INSTATE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
} TONG_APPLY_KICK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
} TONG_KICK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
} TONG_APPLY_LEAVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
} TONG_APPLY_CHANGE_MASTER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[32];
} TONG_CHANGE_MASTER_FAIL_SYNC;

// tong 相关 end
typedef struct
{
	BYTE	ProtocolType;
	UINT	m_dwNpcID;
	WORD	m_wGoldFlag;
} NPC_GOLD_CHANGE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	UINT	dwItemID;
	int		nChange;
	BYTE    nCanUse;
} ITEM_DURABILITY_CHANGE;

typedef struct
{
	BYTE	ProtocolType;
	UINT	dwItemID;
} ITEM_REPAIR;

typedef struct
{
	BYTE	ProtocolType;
	int 	nType;
} PLAYER_SYN_TYPE;


typedef struct
{
	BYTE	ProtocolType;
	int 	nXpos;
	int 	nYpos;

}PLAYER_SYN_POS;

typedef struct
{
	BYTE	ProtocolType;
	UINT	dwItemID;
	int		nNum;
} ITEM_BREAK;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型			
    UINT			uId[16];
	int				bKind;
	int             mFuMoVal;
}PLAYER_COMPITEM;

typedef struct
{
	BYTE	ProtocolType;		// 协议类型			
    int 	inType;
	char	szQingXiang[32];
	char	szHuoDong1[32];
	char	szHuoDong2[32];
	char	szHuoDong3[32];
	char	szHuoDong4[32];
	char	szJiYu[64];
	char    szGongGao[128];
	int     inAcceptLevel;
	int     inRefuseLevel;
}PLAYER_TONGINFO;


typedef struct
{
	BYTE	ProtocolType;		// 协议类型			
    int 	inKind;
	int     inCurTime;
	int     inCurTimeb;
	int     inCurTimec;
	int     inCurTimed;
    char    szItemInfo[32];
}PLAYER_DOSCRIPT;


typedef struct
{
	BYTE	ProtocolType;		// 协议类型			
  	int     cnKind;
	int     cnCurExp;
	int     cnCurLucky;
	int     cnCurJiHuo;
	int     cnCurWuHang;
	char    snCardAcc[32];
	char    snCardPass[32];
}PLAYER_VIP_DOSCRIPT;

typedef struct
{
	BYTE	ProtocolType;		// 协议类型	
	UINT   m_wLength;
    int 	inType;
	char	szQingXiang[32];
	char	szHuoDong1[32];
	char	szHuoDong2[32];
	char	szHuoDong3[32];
	char	szHuoDong4[32];
	char	szJiYu[64];
	char    szGongGao[128];
	int     inAcceptLevel;
	int     inRefuseLevel;
    TONG_CALL_BACK_INFO  m_sMember[defTONG_ONE_PAGE_MAX_NUM];//
}PLAYER_TONGINFO_CALLBACK;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型			
    int 			uIiemId;
	int             nKind;
	int             nX;
	int             nY;
	//int				bKind;
}PLAYER_ADD_ITEM;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型			
    UINT 			uIiemDwId;
	int				bKind;              // 0 为换装 1 为卸装
}PLAYER_AUTO_EQUIP;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型			
    int 			uIiemIdx;
	//int				bKind;
}PLAYER_DELL_ITEM;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型			
    UINT			uId[11];
	int				bKind;
}PLAYER_RONGITEM;


typedef struct
{
	int				m_ID;				// 物品的ID
	BYTE			m_Genre;			// 物品的类型
	int 			m_Detail;			// 物品的具体类别
	int 			m_Particur;			// 物品的详细类别
	BYTE			m_Series; 			// 物品的五行
	BYTE			m_Level;			// 物品的等级
	BYTE			m_btPlace;			// 坐标
	int			    m_btX;				// 坐标
	int 			m_btY;				// 坐标
	int				m_Luck;				// MF
	int				m_MagicLevel[6];	// 魔法生成参数
	int				m_RongMagicLev[6];	// 熔炼生成参数
//	int		        m_BaoMagicLev[6];
//	int             m_JBLevel[7];
	WORD			m_Version;			// 装备版本
//	WORD			m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	int				m_GoldId;			// 黄金Id
	int             m_IsBaiJin;
	BYTE			m_StackNum;
	BYTE			m_EnChance;
	BYTE			m_Point;
	KTime			m_Time;
    BYTE	        m_Rongpiont;//  熔炼
//    BYTE             m_IsBang;
//	BYTE             m_IsKuaiJie;
//	BYTE             m_IsMagic;  
//    BYTE             m_SkillType; 
//	BYTE             m_MagicID; 
//	char            m_ItmeInfo[516]; 
//	char            m_WonName[32]; 
	//char          m_DescInfo[516];
	int              m_IsWhere;
//	BYTE             m_SyncType;
	BYTE             m_IsCanUse;
//	BYTE             m_Count;
	char            nShopName[32];
	int		        m_nIdx;
	UINT	        m_uPrice;
	BYTE            m_Model;
} SViewSellItemInfo;

typedef struct
{
	BYTE			ProtocolType;
	UINT			m_dwNpcID;
	SViewSellItemInfo	m_sInfo[6];
} VIEW_ITEM_SYNC;				// s2c_viewequip

/*
typedef struct
{
	int		m_nID;				// 物品的ID
	BYTE	m_btGenre;			// 物品的类型
	int		m_btDetail;			// 物品的类别
	int		m_btParticur;		// 物品的详细类别
	BYTE	m_btSeries;			// 物品的五行
	BYTE	m_btLevel;			// 物品的等级
	BYTE	m_btLuck;			// MF
	int		m_btMagicLevel[6];	// 生成参数
    int	    m_btRMagicLevel[6];
	int 	m_btBMagicLevel[6]; // 宝石
	int 	m_btJMagicLevel[7]; // 基本
	WORD	m_wVersion;			// 装备版本
	UINT	m_dwRandomSeed;		// 随机种子
	int		m_nIdx;
	UINT	m_uPrice;
	int		m_bX;
	int		m_bY;
	BYTE	m_bPoint;
	BYTE	m_bRPoint;
	BYTE	m_bStack;
	BYTE	m_bEnChance;
	int		m_nGoldId;
	int     m_Isbule;
	int     m_LianJieFlg;
//	char    m_DescInfo[516];
	int     m_IsWhere;
} SLianSellItemInfo;

typedef struct
{
	BYTE	ProtocolType;
	UINT	m_dwNpcID;
	SLianSellItemInfo m_sLInfo;
	int     m_Count;
	char    nShopName[32];
} VIEW_LIAN_ITEM_SYNC;
*/

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	BYTE			m_Genre;			// 物品的类型
	int			    m_Detail;			// 物品的类别
	int				m_Particur;			// 物品的详细类别
	BYTE			m_Series; 			// 物品的五行
	BYTE			m_Level;			// 物品的等级
//	BYTE			m_btPlace;			// 坐标
	BYTE			m_btX;				// 坐标
	BYTE			m_btY;				// 坐标
	int				m_Luck;				// MF
	int				m_MagicLevel[6];	// 魔法生成参数
	int				m_RongMagicLev[6];	// 熔炼生成参数
//	int		        m_BaoMagicLev[6];
	int             m_JBLevel[7];
	WORD			m_Version;			// 装备版本
	WORD			m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	int				m_GoldId;			// 黄金Id
	int				m_StackNum;
	BYTE			m_EnChance;
	BYTE			m_Point;
	KTime			m_Time;
    BYTE	        m_Rongpiont;//  熔炼
	int             m_IsBang;
	int             m_IsKuaiJie;
	int             m_IsMagic;  
	int             m_SkillType; 
	int             m_MagicID; 
	char            m_ItmeInfo[516]; 
	char            m_WonName[32]; 
	//	sMagInfo        m_MagInfo[6];
	int             m_IsWhere;
	int             m_SyncType;
	int             m_IsCanUse;
//	UINT	        m_dwNpcID;
//	int             m_Count;
//	char            nShopName[32];
	int		        m_nIdx;
//	UINT	        m_uPrice;
	BYTE            m_LianJieFlg;
	int             m_IsBaijin;
	int             m_UseMap;
	int             m_UseKind;
} VIEW_LIAN_ITEM_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			dwId;
	int				nCount;
}PLAYER_GET_COUNT;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			dwId;
}PLAYER_NEED_COUNT;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE            nMsgIndex;
	char            szCallFuc[16];
	char            szParma[32];
	char            szParmb[32];
	char            szParmc[32];

}PLAYER_JINDU_CALLBACK;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_Idx;		    	// 买第几个东西
	UINT			m_PlayerId;         // 玩家的playeridx
	BYTE			m_Place;			// 放在身上哪个地方
	BYTE			m_X;				// 坐标X
	BYTE			m_Y;				// 坐标Y
	int             m_IsCityMenber;		// 是否占帮成员
    int             m_Shuihou;
	int             m_MapID;
	BYTE            m_Model;
} PLAYER_TRADE_BUY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	UINT			m_ID;				// 物品的ID
	int				m_Price;
	BYTE            m_Model;
}PLAYER_SET_PRICE;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	BYTE			m_LianFlg;
}PLAYER_SET_LIANJIEFLG;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	char			m_Name[32];         //摆摊名字？
	BYTE			m_bSet;
}PLAYER_START_TRADE;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	char			m_Name[32];
	UINT			m_dwNpcID;
}PLAYER_PLAYER_SHOPNAME;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	UINT	m_dwMoney;
	char	m_szName[32];
} TONG_APPLY_SAVE_COMMAND;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	UINT   m_NpcDwid;
} TONG_APPLY_GET_ATTACK;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwYTongNameID;
	UINT	m_dwDTongNameID;
	UINT   m_NpcDwid;
} TONG_APPLY_SEND_ATTACK;



typedef struct
{
	BYTE	ProtocolType;
	BYTE	NextProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	UINT	m_dwTongNameID;
	int  	m_inParam;
	UINT   m_NpcDwid;
	char    m_Title[32];
} TONG_APPLY_SEND_SETINFO;


typedef struct
{	
	BYTE	ProtocolType;
	char	m_szName[256]; //说明
	BYTE	m_nNum;
	char    m_szTitle[64];
	char    m_Callback[64];
	char    m_Error[16];
}PLAYER_GIVE;


typedef struct
{	
	BYTE	ProtocolType;
	BYTE	m_nType;

}PLAYER_DUANZHAO;

typedef struct
{	
	BYTE	ProtocolType;
	BYTE	m_nType;
	int     m_nCurExtPiont;
}PLAYER_VIPDAZAO;

typedef struct
{	
	BYTE	ProtocolType;
	UINT	m_uId[24];
	BYTE	nKind;
	int     nPrice;

    char	m_Callback[64]; //回调函数
    char    m_Error[16];
}PLAYER_GIVE_CALLBACK;


typedef struct
{	
	BYTE	ProtocolType;
	char    m_szTitle[32];  //标题
	int     m_MsgIndex;     //模板的编号
	char    m_Callback[16]; //回调的函数
	int     m_inTime;       //暂停的时间
    char    m_szParma[32];  //回调的参数一
    char    m_szParmb[32];
	char    m_szParmc[32];
}PLAYER_OPEN_JINDUTIAO;



typedef struct
{
	BYTE			ProtocolType;
	int             nShopType;
	int             nShopID;
}PLAYER_REQUEST_OPEN_MARKET;


typedef struct
{
	BYTE    ProtocolType;
    char    m_Content[125];
    char    m_SCallback[64];
    char    m_SError[16];
    int     m_nNum;
}PLAYER_STRING_CALLBACK;


typedef struct
{
	BYTE	ProtocolType;
	int		nShopID;
} SALE_MARKET_SYNC;

#ifndef _SERVER

// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSell(int nID);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdBuy(int nBuyIdx, int nPlace, int nX, int nY,int nShuiShou,int nIsCityMenber,int nMapID,int nItemNum=0);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdRun(int nX, int nY,int nMapID=0);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdWalk(int nX, int nY);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSkill(int nSkillID, int nX, int nY);
void SendClientCmdSunyi(int nMaps,int nX, int nY,int nNpcID,int nModel=0); //瞬移
//void SendClientCmdPing();
void SendClientCmdSit(int nSitFlag);
void SendClientCPUnlockCmd(int CP_IntPW);
void SendClientCPLockCmd();
void SendClientCPChangeCmd(int oldPW, int newPW);
void SendClientCPResetCmd(int resetPW);
void SendClientOpenMarket(int nShopid,int iShopType=0);
void SendClientString(char* sSContent,char* sSCallback,char* sSError,int biaozhi);//stringbox
void SendClientAutoplayopen(int nstt); //挂机开启脚本
void SendClientAutoplayclose(int nstt);
void SendClientDaTau(int nstt,int nXuHao);
void SendClientCmdMoveItem(void* pDownPos, void* pUpPos,int nIsComp=0);
void SendClientCmdQueryLadder(UINT	dwLadderID);
void SendClientCmdRequestNpc(int nID);
void SendClientCmdJump(int nX, int nY);
void SendClientCmdStoreMoney(int nDir, int nMoney);
void SendClientCmdRevive(int nReviveType);
void SendObjMouseClick(int nObjID, UINT dwRegionID);
void SendClientCmdRepair(UINT dwID);
void SendClientCmdRide(); //骑马 
void SendClientCmdBreak(UINT dwID, int nNum);
void SendClientCmdSetPrice(UINT nId, int nPrice,BYTE nModel=0); //摆摊标价格
void SendClientCmdStartTrade(int nStart, char* sName); //摆摊
void SendClientCmdPlayerBuy(int nIdx, UINT nPlayerId, int nPlace, int nX, int nY,int IsCityMenber,int nShuishou,int nMapID,BYTE nModel=0);
void SendClientCmdGetCount(UINT dwId);
void SendClientCmdLianCount(UINT dwId);
void SendClientCmdShopName(UINT dwID);
void SendClientCmdSysShop(int nKind,int inParma=0,int inParmb=0,int inParmc=0,int inParmd=0);
void SendClientCmdLiXian();
void SendClientCmdGive(unsigned int uId, int nItemNum, int nKind, char* sCallback,char* sError,int nPrice=0);
void SendClientPKValue(int nstt);

void SendClientGuZi(int nMapID,int dianshu,int nNetIndexid,int ZuoWeiHao);
void SendClientGuZiFarms(int nMapID,int nFarmA,int nFarmB);
void SendClientAddJinBi(int nJinBi);
void SendClientAutoPlay(int nstt,int cishu,int nIsOpen);
void SendClientLianJie(char*nPLname,int nItemDWid);
void SendClientCmdJinMai(int nType,int nLevel);
void SendClientCmdGua(int nGuaType);
void SendClientCmdRong(unsigned int uId, int nItemNum, int nKind);
void SendClientCmdComp(ItemCounmd *pInfo, int nItemNum, int nKind,int nFuMoVal=0);

void SendClientAddItem(void* pInfo); //KUiObjAtContRegion *pInfo   //没有用到
void SendClientDelItem(int nIndex);
void SendClientCmdSetLianFlg(int nId, BYTE nLianFlg); //设置服务器链接状态
void SendClientSetTongInfo(KUiTongInfoWithName *nTong); //保存帮会设置
void SendClientSetTonggg(KUiTongInfoWithName *nTong);
void SendClientGetTongInfo(int nIndex,int nType=0,int nStartPos=2);
void SendClientGetTonggg(int nIndex);
void SendClientDoScipt(KUiPlyerDoscript *nDoscript=NULL);
void SendClientJdCallBack(nJinDuClient *nJdscript=NULL);
void SendClientUpData();            //请求更新当前数据
void SendClientVipDoScipt(KUiPlyerVipDoscript *nVipDoscript=NULL);
void SendClientSetTaskVal(int nTaskNo);
void SendClientCmdTpye(int nType);  //请求更新 某类数据
void SendClientSetPos(int mXpos,int mYpos);
void SendClientRunScript(int nKind,int nVala, int nValb, int nValc);

#endif

extern	int	g_nProtocolSize[MAX_PROTOCOL_NUM];
#pragma pack(pop, enter_protocol)
#endif


