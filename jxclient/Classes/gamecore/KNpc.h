#ifndef KNpcH
#define KNpcH
//---------------------------------------------------------------------------
class ISkill;
class KSkill;
#include "KCore.h"
#include "KSkillList.h"
#include "KMagicAttrib.h"
#include "GameDataDef.h"
#include "KNpcFindPath.h"
#include "KNpcDeathCalcExp.h"
#include "KIndexNode.h"
#include "KNpcGold.h"

#include <map>
//#include <hash_map>
//#include <algorithm>
//#include "KSubworld.h"
#include "KNpcRes.h"
//---------------------------------------------------------------------------
#define	MAX_NPC			200        // NPc限制  500
#define VER_INFO "50JX-专用端"
#define MAX_NPCSTYLE	3000       // npc模板数量 2000
//#define VER_INFO "剑侠<color=red>★<color>江湖-专用端"
#define	defMAX_SHOW_BLOOD_NUM		5    //50
#define	MAX_AI_PARAM				10   //AI模式 类别
#define	MAX_NPC_USE_SKILL			4    //NPC怪物最大技能数量

#define	MAX_FUMO_COUNT              5    //神将的数量
#define defLOGIC_CELL_WIDTH         32


//#define	TABFILE_MAGICALEVEL_PATH	"\\Settings\\item\\004\\magicattriblevel_index.txt"

//#define	KItemNormalAttrib KMagicAttrib
//	跑动速度与行走速度的倍数
//#define	WALK_RUN_TIMES	3

#define		STATE_FREEZE	0x01   //2 的 0 次方
#define		STATE_POISON	0x02   //2 的 1 次方
#define		STATE_CONFUSE	0x04   //4 2的 2次方
#define		STATE_STUN		0x08   //8 2的 3 次方
#define		STATE_HIDE		0x10   //16 2的 4 次方
//#define		STATE_MOVE		0x32   //32 2的 5 次方

//#define		STATE_ZHUA		0x09
enum NPCATTRIB
{
	attrib_mana,
	attrib_stamina,
	attrib_life,
	attrib_maxmana,
	attrib_maxstamina,
	attrib_maxlife,
};

enum NPCCMD
{
	do_none,		// 什么也不干
	do_stand,		// 站立
	do_walk,		// 行走
	do_run,			// 跑动
	do_jump,		// 跳跃
	do_skill,		// 发技能的命令
	do_magic,		// 施法
	do_attack,		// 攻击
	do_sit,			// 打坐
	do_hurt,		// 受伤
	do_death,		// 死亡
	do_defense,		// 格挡
	do_idle,		// 喘气
	do_specialskill,// 技能控制动作
	do_special1,	// 特殊1
	do_special2,	// 偷窃技能
	do_special3,	// 特殊3
	do_special4,	// 移动
	do_runattack,
	do_manyattack,
	do_jumpattack,
	do_revive,      //重生
	do_stall,
	do_movepos,		// 瞬间移动
	do_knockback,	// 震退
	do_drag,		// 拉扯过来
	do_rushattack,	// 冲砍
	do_runattackmany, //冲刺多人
	do_num,
/*	do_sunyi,       //瞬间移动
	do_yidong,      //跨地图移动

	do_movepos,		// 瞬间移动
	do_knockback,	// 震退
	do_drag,		// 拉扯过来
	do_rushattack,	// 冲砍
	do_runattackmany, //冲刺多人 */
};

enum CLIENTACTION
{
	cdo_fightstand,
	cdo_stand,
	cdo_stand1,
	cdo_fightwalk,
	cdo_walk,
	cdo_fightrun,
	cdo_run,
	cdo_hurt,
	cdo_death,
	cdo_attack,
	cdo_attack1,
	cdo_magic,
	cdo_sit,
	cdo_jump,
	cdo_none,   
	cdo_count,
};
 //伤害类型
enum DAMAGE_TYPE
{
	damage_physics = 0,		// 物理伤害
	damage_fire,			// 火焰伤害
	damage_cold,			// 冰冻伤害
	damage_light,			// 闪电伤害
	damage_poison,			// 毒素伤害
	damage_magic,			// 无属性伤害
	damage_num,				// 伤害类型数目
};

// DoDeath 时的参数，对应不同的死亡惩罚
enum DEATH_MODE
{
	DEATH_MODE_NPC_KILL = 0,		// 被npc杀死0
	DEATH_MODE_PLAYER_NO_PUNISH,	// 切磋模式被玩家杀死  1  没有惩罚
	DEATH_MODE_PLAYER_PUNISH,		// 被玩家PK致死，根据PK值进行惩罚2
	DEATH_MODE_PKBATTLE_PUNISH,		// 类式于国战时的惩罚处理 3
	DEATH_MODE_EXP_PUNISH,		    // 经验惩罚
	DEATH_MODE_MONEY_PUNISH,		// 银两惩罚
	DEATH_MODE_EQUIP_PUNISH,		// 装备惩罚
	DEATH_MODE_JINBI_PUNISH,        // 金币惩罚
	DEATH_MODE_NUM,
};

typedef struct
{
	NPCCMD		CmdKind;		// 命令C
	int			Param_X;		// 参数X
	int			Param_Y;		// 参数Y
	int			Param_Z;		// 参数Y
} NPC_COMMAND;

typedef struct
{
	int		nTotalFrame;  //全部帧
	int		nCurrentFrame;//当前帧
	void SetFrame(int nTotal)
	{
		if (nTotal <= 0)	// 一般不需要时间为0的动作
			nTotalFrame = 1;
		else
			nTotalFrame = nTotal;

		nCurrentFrame = 0;
	};
} DOING_FRAME;

struct KState
{
	int	nMagicAttrib;
	int	nValue[3];
	int	nTime;
};
//改变NPC的基本数据结构
struct KChanelBaseInfo
{
	int     PhysicsDamage;             // 基本伤害
	int		m_PhysicsDamage;		   // Npc的当前伤害
	int		m_FireDamage;	           // Npc的当前火伤害
	int		m_ColdDamage;	           // Npc的当前冰伤害
	int		m_LightDamage;	           // Npc的当前电伤害
	int		m_PoisonDamage;	           // Npc的当前毒伤害
	
	int		m_FireResist;	            // Npc的当前火抗性
	int		m_ColdResist;	            // Npc的当前冰抗性
	int		m_PoisonResist;	            // Npc的当前毒抗性
	int		m_LightResist;	            // Npc的当前电抗性
	int		m_PhysicsResist;	        // Npc的当前物理抗性
};

struct skillAuraInfo
{
	int skillid;
	int skilllistIndex;
	int level;
};

struct sNpcShadowInfo
{
	bool bInMenu;
	unsigned int uImage;
	short nFrame;
	int nX;
	int nY;
	int nZ;
	char szImage[256];
};

//神将类
struct CFuMoInfo
{
	int nNpcIdx;  
	int nNpcSetings;     //NPC settings	存档
	int	nNpcSkill;       //同伴的技能id 存档
	int nSkillLevel;	 //技能等级     存档
	int nNowLevel;       //同伴现在的等级
	DWORD nCastTime;     //释放时间间隔
	DWORD nNextCastTime; //下次释放时间
	int nNowExp;         //现在的经验
};

//技能加成
struct CEnhanceInfo
{
	int	nSkillIdx;   
	int nEnhance;
};
/*
struct CTongBanInfo
{
	int nNum1;
	int nNum2;
	int nNum3;
	int nNum4;
	int nNum5;
};
*/
struct	KSyncPos
{
	DWORD	m_dwRegionID;
	int		m_nMapX;
	int		m_nMapY;
	int		m_nOffX;
	int		m_nOffY;
	int		m_nDoing;
};

struct	KNpcAutoChat
{
	int                 m_Model;      //
	//char                m_NpcTalkMsg[250];
	int                 m_NpcTalkTime;
	//char                m_NpcTalkName[64];
};

class KStateNode : public KNode
{
public:

	int				m_SkillID;					// 技能ID
	int				m_Level;					// 技能等级
	int				m_LeftTime;					// 剩余时间
	KMagicAttrib	m_State[MAX_SKILL_STATE];	// 属性数量状态列表
	//KMagicAttrib	m_JinMaiState;	            // 经脉状态列表
	int				m_StateGraphics;			// 状态动画索引
	int             m_IsClientState;            // 是否对方看本人的状态
};

class KNpc
{
	friend class KNpcSet;
public:
	sNpcShadowInfo      _NpcShadow;
	DWORD				m_dwID;					// Npc的ID  第个NPC 编号 无限增大
	//BOOL              m_IsSyn;
	DWORD               m_bLockNpcDwID;			// 锁定NPC的	DWid
    BYTE                m_IsHaveAttack;         //是否设置为攻击无效了
	DWORD               m_AttackerDwid;         //上次攻击着的DWID
	DWORD               m_IsRevive;             //是否重生
	BYTE                m_IsRe;                 //同步重生帧数
	//bool                m_isClearSpr;           //是否同步到客户端删除精灵
	int					m_Index;				// Npc的索引
	KIndexNode			m_Node;					// Npc's Node
	int					m_Level;				// Npc的等级
	BYTE				m_Kind;					// Npc的类型
	BYTE				m_Series;				// Npc的系
	int					m_Height;				// Npc的高度(跳跃的时候非零)
	int 				m_btRankId;             // 称号的id
	int 				m_NpcTitle;             // NPC新称号（存档）
	int                 m_CurNpcTitle;          // 当前称号	 （不存档临时称号）
	BYTE				m_btRankFFId;           //SPR称号的
	BYTE                m_AutoplayId;           //挂机SPR称号的
	int 				nRankInWorld;           //世界排名
	int                 nLevelInWorld;          //世界等级排行
	int 				nRepute;                //声望
	int 				nPKValue;
	int 				nReBorn;                //转生次数
	int					m_nStature;				//Tall
	int                 m_CJtaskID;             //172抽奖专用
	int                 m_KillNumber;           //杀人数量 排名调用
//	CJinMaiInfo         m_JinMai[10];           //0 0 为筋脉大类
	//CFuMoInfo         m_nFuMoNum[MAX_FUMO_COUNT];          //附魔列表
	int                 m_nCurJiHuo;            //当前激活的附魔的NPC
	int                 m_ZhenYuan;             //真元值
	int                 m_JinMaiBingJia;        //经脉兵甲值
	int                 m_GameliveTime;         //NPC 存活时间
	int                 m_TempliveTime;         //临时保存存活时间
	int                 m_liveType;             //时间设定的类型
	//BYTE                m_WhereSer;		        //那个服务器的

	///临时参数/////不加入数据库
	int					m_njb;				    //携带的金币
    int					m_njxb;				    //携带的剑侠B
	//char				m_WarTongNamea[32];     //占领帮会
    //char				m_WarMaster[32];        //城主
//	int				    m_WarShuishou;          //税收
//	int                 m_Warzhi;               //物价指数
	int                 m_WarCityGX;            //个人贡献
	int                 m_IsWarCity;            //哪个城的成员
	int                 m_IsInCity;             //是否加入城市
	int					m_nsPlayerIdx;          // 玩家的服务器同步过来的
	int	                m_nMissionGroup;
/*	BYTE                 m_WarFucheng;           //副成
    BYTE                 m_WarZuoHu;             //左护法
	BYTE                 m_WarYouHu;             //右护法
	BYTE                 m_WarTaishi;            //太史
	BYTE                 m_WarZhongShu;          //中书
	BYTE                 m_WarShangShu;          //尚书
    BYTE                 m_WarMiShu;             //秘书
	BYTE                 m_WarTaiLe;             //太乐	
	*/
    BYTE                m_IsDoing;              //是否正在执行脚本
	int                 m_ReviceNum;            //重生次数
    BYTE                m_IsSynDir;             //是否可以同步方向
//	int                 m_WarIsGong;            //属于攻方
//    int               m_WarIsShou;            //属于守方
	//char                m_GuishuName[32];       //NPC归属谁的同伴
	DWORD               m_GuiShuDwid;           //主人的Dwid
	//char              m_BiaoCheName[32];      //NPC归属谁的镖车
	BYTE                m_RestNameCount;          //改名的次数
	//char                m_ChenHaoName[32];      //称号的名称

 //   BYTE                m_IsWaiGua;              //是否使用外挂
 /*
	int                 m_GuziYajing;            //押金
	int                 m_GuziDianshu;           //点数
	BYTE                m_GuziZhuang;            //是否庄家
	BYTE                m_IsShuai;               //是否已经甩了
	BYTE                m_GuziMenber;            //1为主参与者 2为旁观下注者
	BYTE                m_ZuoWeihao;             //座位号
*/	
//	BYTE                IsDeath;                 
	int                 IsJinYan;                 //活动经验
	int                 IsLuKey;                  //活动幸运
	int                 m_GoldLucky;              //全局幸运
	int                 IsJinQian;                //活动金钱
//	char                m_ItmeInfo[128];          //物品说明
//	BYTE                m_IsTuiGuang;             //是否推广员
	int                 m_IsVip;                  //会员
	int                 m_IsXingYunXing;

	int                 m_mMapX;
	int                 m_mMapY;
	int                 m_IsgetSkill;             //是否拾取技能状态
	BYTE                IsExeGoldScript;          //是否执行全局死亡脚本
	BYTE                IsCreatBoss;
	BYTE                IsCreatTongBan;           //是否已经可以召唤同伴
	int                 m_ZhuaVal;                //当前的被抓捕值
    DWORD               m_TongBanNum;             //同伴数量
	BYTE                m_IsSerLock;              //是否锁住某人    
	BOOL                m_nIsOver;

    BOOL                m_bIsHideNpc;
    BOOL                m_bIsHidePlayer;
    BOOL                m_bIsHideMissle;
	BOOL                m_bIsHideLife;
	BOOL                m_bIsHideTong;
	BOOL                m_bIsHideNuqi;

	//char                m_PicPath[256];  //头像路径
	//char                m_ScriptPicPath[256];//脚本路径

	BYTE                m_BtnFlag;                //是否插旗状态
	BOOL                m_BtnFindPath;            //是否自动寻路状态
	int 				m_btStateInfo[MAX_NPC_RECORDER_STATE];	// Npc当前最新的几个状态  BYTE
//	DWORD				m_NpcEnchant;			                // Npc的加强类型（黄金怪物）
	int					m_nNextStatePos;		                // 下一次状态在m_btState的位置
	KNpcGold			m_cGold;

	//用于Npc模板库中，当FALSE表示该Npc数据当前是无效的 ，数值未经过脚本计算，需要生成.
	//TRUE表示有效数据
	//BOOL				m_bHaveLoadedFromTemplate;// 
	
	//KState			m_PowerState;			// 怒火状态

	//typedef std::map<INT, KState> _KStateNote;                 //SkillId,Echance
	//_KStateNote nKStateNote;
	KState				m_PoisonState;			// 中毒状态
	KState				m_FreezeState;			// 冰冻状态
	KState				m_BurnState;			// 燃烧状态
	KState				m_ConfuseState;			// 混乱状态
	KState				m_StunState;			// 眩晕状态
	KState				m_LifeState;			// 补血状态
	KState				m_ManaState;			// 补MANA状态
	KState				m_MenuState;			// 菜单
	KState				m_DrunkState;			// 醉酒状态
	KState				m_Hide;                 // 隐藏？
	KState				m_ZhuaState;            // 被抓捕状态
	KState				m_LoseMana;             // 吊兰状态
    KState				m_ExpState;             // 经验状态
	KState				m_DoScriptState;        // 执行脚本状态
    KState              m_randmove;             // 群体走动状态
	KState              m_MapUseModel;          // 地图消耗类型
	KState				m_PhysicsArmor;
	KState				m_ColdArmor;
	KState				m_LightArmor;
	KState				m_PoisonArmor;
	KState				m_FireArmor;
	KState				m_ManaShield;
	KState              m_Returnskill;          //被敌人攻击是 自动释放某技能几率
	KState              m_Deathkill;            //被敌人攻击是 自动释放某技能几率
	KState              m_Rescueskill; 	        //救命自动释放技能
	KState              m_Replyskill; 	        //自动反击技能
	//KState            m_Staticmagicshield;    //魔法盾状态
	
	int                 m_AdjustColorKind;      //NPC的当前偏色
	int					m_RideState;            //Npc骑马上下马时间限制

	KList				m_StateSkillList;		// 附加状态技能列表（同技能不叠加）
	BYTE				m_Camp;					// Npc的阵营
	BYTE				m_CurrentCamp;			// Npc的当前阵营
	NPCCMD				m_Doing;				// Npc的行为
	CLIENTACTION		m_ClientDoing;			// Npc的客户端行为
	char	            szNpcTypeName[32];
	DOING_FRAME			m_Frames;				// Npc的行为帧数
	KSkillList			m_SkillList;			// Npc的技能列表
	int					m_SubWorldIndex;		// Npc所在的SubWorld ID maps里面 第几个地图
	int					m_RegionIndex;			// Npc所在的Region ID
	int					m_ActiveSkillID;		// Npc激活的技能ID
	int                 m_ActiveSkListIndex;

	int					m_ActiveAuraID;			// Npc激活的光环技能ID
	int                 m_ActiveAuraIndex;
	int                 m_ActiveXinYunXingID;

	bool                m_IsMoreAura;
	skillAuraInfo		m_TmpAuraID[5];         // 峨眉技能多光临时的光环技能ID
	int					m_ExtSkill[5];          // 称号扩展状态技能

//Npc的实际数据（已经经过装备及技能的运算了）===================================
	int					m_CurrentExperience;	// Npc被杀后送出的经验
	int 				m_CurrentLife;			// Npc的当前生命
	int 				m_CurrentLifeDamage;    // 主要用于冒血处理
	int 				m_CurPoisonDamage;      // 主要用于冒血处理
	int 				m_CurFireDamage;        // 主要用于冒血处理
	int 				m_CurrentLifeMax;		// Npc的当前生命最大值
	int					m_CurrentLifeReplenish;	// Npc的当前生命回复速度
	int					m_CurrentLifeReplenish_p;// Npc的当前生命回复速度 百分比
    int   				m_CurrentMana;			// Npc的当前内力
	int				    m_CurrentManaMax;		// Npc的当前最大内力
	int					m_CurrentManaReplenish;	// Npc的当前内力回复速度
	int                 m_CurrentManaReplenish_p;//
	int				    m_CurrentStamina;		// Npc的当前体力
	int				    m_CurrentStaminaMax;	// Npc的当前最大体力
	int					m_CurrentStaminaGain;	// Npc的当前体力回复速度
	int					m_CurrentStaminaLoss;	// Npc的当前体力下降速度

	int					m_CurrentNuQi;			// Npc的当前怒气值
	int					m_CurrentNuQiMax;		// Npc的当前怒气最大值
	int					m_CurrentNuQiReplenish;	// Npc的当前怒气回复速度
//////===================技能攻击数据（从脚本获得）=============================
	KMagicAttrib		m_PhysicsDamage;		        // Npc的基本伤害点(由力量敏捷与武器伤害决定，不考虑直接加伤害的魔法属性)(攻击点)
	KMagicAttrib		m_CurrentPhysicsMagicDamageP;	// Npc的当前普点百分比
	KMagicAttrib		m_CurrentPhysicsMagicDamageV;	// Npc的当前普点伤害
	KMagicAttrib		m_CurrentMagicFireDamage;	    // Npc的当前火伤害
	KMagicAttrib		m_CurrentMagicColdDamage;	    // Npc的当前冰伤害
	KMagicAttrib		m_CurrentMagicLightDamage;	    // Npc的当前电伤害
	KMagicAttrib		m_CurrentMagicPoisonDamage;	    // Npc的当前毒伤害
	int					m_CurrentAttackRating;	        // Npc的当前命中率（身法命中率+技能命中率）
	int					m_CurrentDefend;		        // Npc的当前闪避率点
///=============================================================================
	int 				m_CurrentSkillMingZhong;	// Npc的当前生命最大值
	int					m_TempFireResist;	        // Npc的当前火抗性
	int					m_TempColdResist;	        // Npc的当前冰抗性
	int					m_TempPoisonResist;	        // Npc的当前毒抗性
	int					m_TempLightResist;	        // Npc的当前电抗性
	int					m_TempPhysicsResist;	    // Npc的当前物理抗性

	int					m_CurrentFireResist;	    // Npc的当前火抗性
	int					m_CurrentColdResist;	    // Npc的当前冰抗性
	int					m_CurrentPoisonResist;	    // Npc的当前毒抗性
	int					m_CurrentLightResist;	    // Npc的当前电抗性
	int					m_CurrentPhysicsResist;	    // Npc的当前物理抗性

	int					m_CurrentFireResistMax;		// Npc的当前最大火抗性
	int					m_CurrentColdResistMax;		// Npc的当前最大冰抗性
	int					m_CurrentPoisonResistMax;	// Npc的当前最大毒抗性
	int					m_CurrentLightResistMax;	// Npc的当前最大电抗性
	int					m_CurrentPhysicsResistMax;	// Npc的当前最大物理抗性

	//BOOL                m_AttackState;
	int                 m_CurrentTempSpeed;
	int					m_CurrentWalkSpeed;		// Npc的当前走动速度
	int					m_CurrentRunSpeed;		// Npc的当前跑动速度
	int					m_CurrentJumpSpeed;		// Npc的当前跳跃速度
	int					m_CurrentJumpFrame;		// Npc的当前跳跃帧数
	int					m_CurrentAttackSpeed;	// Npc的当前攻击速度  --外功
	int					m_CurrentCastSpeed;		// Npc的当前施法速度  --内功
	int					m_CurrentVisionRadius;	// Npc的当前视野范围
	int					m_CurrentAttackRadius;	// Npc的当前攻击范围
	int					m_CurrentActiveRadius;	// Npc的当前活动范围
	int					m_CurrentHitRecover;	// Npc的受伤动作时间
	int					m_CurrentHitNpcRecover;	// 造成对方的受伤动作增加多少
	int					m_CurrentTreasure;		// Npc丢落装备的数量
	int					m_CurrentHitRank;       // 受伤的概率
//以下数据只需要当前值，基础值均为0 = 技能状态数据+装备数据（没状态-直接赋值）==================================
	int					m_CurrentMeleeDmgRetPercent;	// Npc近程伤害返回的百分比
	int					m_CurrentMeleeDmgRet;			// Npc近程被击时返回的伤害点数
	int					m_CurrentRangeDmgRetPercent;	// Npc远程伤害返回的百分比
	int					m_CurrentRangeDmgRet;			// Npc远程被击时返回的伤害点数
	BOOL				m_CurrentSlowMissle;			// Npc是否在慢速子弹状态下
	int	                m_CurrentHulueMeleeDmgRet;      // 忽略近程反弹多少点、
    int	                m_CurrentHulueRangeDmgRet;      // 忽略远程程反弹多少点、

	int					m_CurrentDamageReduce;			// 物理伤害减少
//	int					m_CurrentElementDamageReduce;	// 元素伤害减少

	int					m_CurrentDamage2Mana;			// 伤害转内力百分比
//	int					m_CurrentManaPerEnemy;			// 杀一个敌人加多少点内力 //
	int					m_CurrentLifeStolen;			// 偷生命百分比 吸血
	int					m_CurrentManaStolen;			// 偷内力百分比 吸内
	int					m_CurrentStaminaStolen;			// 偷体力百分比
	int					m_CurrentKnockBack;				// 震退百分比
	int					m_CurrentDeadlyStrike;			// 致命一击百分比
//	int					m_CurrentBlindEnemy;			// 致盲敌人百分比
//	int					m_CurrentPiercePercent;			// 穿透攻击百分比
	int					m_CurrentFreezeTimeReducePercent;	// 冰冻时间减少百分比
	int					m_CurrentPoisonTimeReducePercent;	// 中毒时间减少百分比
	int					m_CurrentStunTimeReducePercent;		// 眩晕时间减少百分比
	int					m_CurrentBurnTimeReducePercent;		// 燃烧时间减少百分比
    int                 m_CurrentautoReviverate;            //当前的复活概率
	int					m_CurrentStunRank_p;		        // 减少眩晕概率增加百分比

	int					m_EnemyPoisonTimeReducePercent;	   // 造对方中毒时间增加百分比
	int					m_EnemyStunTimeReducePercent;       // 造对方眩晕时间增加百分比

    int                 m_CurrentBuZhuoRate;                   //当前的捕获概率
	int				    m_CurrentUpExp;                        //经验倍率
	int					m_CurrentdanggeRate;		           //档格概率
    int					m_CurrentzhongjiRate;		           //重击概率
    int                 m_CurrentcjdanggeRate;                 //拆解档格
    int                 m_CurrentcjzhongjiRate;                //拆解重击
    int                 m_Currentsorbdamage;                   // 抵消伤害 点数
    int                 m_Currentsorbdamage_v;                 // 抵消伤害百分比
	int                 m_Currenadddamagev;                    //加深伤害点
	int                 m_Currenadddamagep;                    //加深伤害百分比
    int                 m_Currentpoisonres;                    //=忽略对方毒防:#d1-%
    int                 m_Currentfireres;                      //=忽略对方火防:#d1-%
    int                 m_Currentlightingres;                  //=忽略对方雷防:#d1-%
    int                 m_Currentphysicsres;                   //=忽略对方普防:#d1-%
    int                 m_Currentcoldres;                      //=忽略对方冰防:#d1-%
    int 	            m_Currentallres;                       //=忽略对方全抗:#d1-%
	int                 m_CurrentIgnoredefensep;               //当前忽略对方闪避率   //技能的数据
	int                 m_CurrentIgnorenAttacRating;           //当前忽略对方命中率   //状态数据
    int                 m_Currentnopkvalue;                    //=不增加PK值概率:#d1+%
    int                 m_Currentbossdamage;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
    int                 m_Currentelementsenhance;              //=五行强化值：#d1-点。强化对相克五行的克制效果 我克
    int                 m_Currentelementsresist;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果 克我

    int                 m_Currentskillenhance;                 // 攻击技能状态加强
    int                 m_CurrentFullManaskillenhance;         // 满内力时攻击技能加成;
	//CEnhanceInfo        m_CurrentSkillEnhance[MAX_NPCSKILL]; // 对某技能状态加强
	typedef std::map<int, CEnhanceInfo> _EnhanceInfo;                 //SkillId,Echance
	_EnhanceInfo nEnhanceInfo;
	int					m_CurrentFireEnhance;			// 火加强
	int					m_CurrentColdEnhance;			// 迟缓时间增加
	int					m_CurrentPoisonEnhance;			// 毒发时间间隔
	int					m_CurrentLightEnhance;			// 眩晕时间持续
	int                 m_CurrentPoisonTime;            // 技能的毒发时间

	int                 m_CurrentAttackRatingEnhancep;  // 装备属性命中率加强百分比
    int                 m_CurrentAttackRatingEnhancev;  // 装备属性命中率加强点

	int					m_CurrentAddPhysicsDamage;		// 当前被动外功普点 直接的物理伤害加强点数
	int                 m_CurrentAddPhysicsDamageP;     // 当前被动外普百分比
   	int                 m_CurrentAddFireDamagev;        // 当前被动外火点
 	int                 m_CurrentAddColdDamagev;        // 当前被动外冰点 
	int                 m_CurrentAddLighDamagev;        // 当前被动外雷点
	int                 m_CurrentAddPoisonDamagev;      // 当前被动外毒点
    BOOL                m_IsDel;                        // 是否删除NPC
    int                 m_CurrentAddmagicphysicsDamage; // 当前被动内普点
	int                 m_CurrentAddmagicphysicsDamageP;// 当前被动内普百分比
	int                 m_CurrentAddmagicColdDamagicv;  // 当前被动内冰点
	int                 m_CurrentAddmagicFireDamagicv;  // 当前被动内火点  
	int                 m_CurrentAddmagicLightDamagicv; // 当前被动内雷点
	int                 m_CurrentAddmagicPoisonDamagicv;// 当前被动内毒点

	int					m_CurrentMeleeEnhance[MAX_MELEE_WEAPON];	// 近程物理加强
	int					m_CurrentRangeEnhance;			// 远程物理加强
	int					m_CurrentHandEnhance;			// 空手物理加强
	int					m_CurrentSerisesEnhance;        // 五行的伤害加强

    int                  m_CurrentPoisondamagereturnV;   //当前毒反弹点
    int                  m_CurrentPoisondamagereturnP;   //当前毒反弹百分比
	int                  m_CurrentReturnskillp;          //当前诅咒反弹概率
	int                  m_CurrentIgnoreskillp;          //无视诅咒反弹概率
	int                  m_CurrentReturnresp;            //对反弹伤害的抗性
	int                  m_CurrentCreatnpcv;             //当前抓捕BB的概率
    int                  m_CurrentAllJiHuo;              //是否全身激活
	int                  m_CurrentCreatStatus;           //当前产生的状态
	int                  m_Currentbaopoisondmax_p;  
	int                  m_nCurNpcLucky;

    int                  m_Me2metaldamage_p;              //=对金系伤害增加：#d1+%
    int                  m_Metal2medamage_p;              //=减少来自金系的伤害：#d1-%
    int                  m_Me2wooddamage_p;              //=对木系伤害增加：#d1+%
    int                  m_Wood2medamage_p;              //=减少来自木系的伤害：#d1-%
    int                  m_Me2waterdamage_p;              //=对水系伤害增加：#d1+%
    int                  m_Water2medamage_p;              //=减少来自水系的伤害：#d1-%
    int                  m_Me2firedamage_p;              //=对火系伤害增加：#d1+%
    int                  m_Fire2medamage_p;              //=减少来自火系的伤害：#d1-%
    int                  m_Me2earthdamage_p;              //=对土系伤害增加：#d1+%
    int                  m_Earth2medamage_p;              //=减少来自土系的伤害：#d1-%

	int                  m_Staticmagicshield_p;		      //低于内力最大值多少倍的伤害

// 只需要当前值的数据结束
	int					m_Dir;							// Npc的方向
	int					m_RedLum;						// Npc的亮度
	int					m_GreenLum;
	int					m_BlueLum;
	int					m_MapX, m_MapY, m_MapZ;			// Npc的地图坐标(格子坐标) x （0-15） y（0-31）
	int					m_OffX, m_OffY;					// Npc在格子中的偏移坐标（放大了1024倍）
	int					m_DesX, m_DesY;					// Npc的需要移动到的目标坐标
	int					m_SkillParam1, m_SkillParam2;   // 
	int					m_OriginX, m_OriginY;			// Npc的原始坐标 （像素坐标）
	DWORD				m_NextAITime;
	BYTE				m_AIMAXTime;                //NpcAI
// Npc的装备（决定客户端的换装备）	
	int					m_HelmType;					// Npc的头盔类型
	int					m_ArmorType;				// Npc的盔甲类型
	int					m_WeaponType;				// Npc的武器类型
	int					m_HorseType;				// Npc的骑马类型
	BOOL				m_bRideHorse;				// Npc是否骑马
	int					m_MaskType;					// Npc 面具功能
	//int				m_FuMoType;					// Npc 附魔功能
	int 				m_PifengType;				// Npc 披风功能
	BYTE				m_ChiBangType;				// Npc 披风功能
	int					m_MaskMark;					// 面具市场
	//BYTE				m_IsFuMo;					// 
	BYTE				m_BaiTan;					// Npc是否在摆摊
	//char				ShopName[32];
//客户端
	char				TongName[32];
	int					m_nFigure;
	char				Name[32];				// Npc的名称-玩家
	char                _clientName[32];
	BYTE				m_nSex;					// Npc的性别0为男，1为女
	int					m_NpcSettingIdx;		// Npc的设定文件索引  （行号？）
	int					m_CorpseSettingIdx;		// Npc的尸体定义索引
	//char				ActionScript[256];		// Npc的行为脚本
	typedef std::map<int, std::string> _StrNoteInfo;
	_StrNoteInfo nstrNoteInfo;

	void ClearstrInfo(int i)
	{
		if (nstrNoteInfo.count(i)>0)
		{
		   nstrNoteInfo.erase(i);
		}
	}

	char * GetstrInfo(int i,char * str)
	{
		//char nMsg[256]={0};
		/*_StrNoteInfo::iterator it;
		for( it = nstrNoteInfo.begin(); it != nstrNoteInfo.end(); ++it)
		{
			 if (it->first == i)
			 {
#ifdef _SERVER
				 printf("--获取(%s)脚本:%s --\n",Name,it->second.c_str());
#endif
				 t_sprintf(str,it->second.c_str());
			     return str;
			 }
		} */
		if (nstrNoteInfo.count(i)>0)
		{//如果存在
			t_sprintf(str,nstrNoteInfo[i].c_str());

/*#ifdef _SERVER
			printf("--获取(%s)(%d)脚本:%s --\n",Name,i,nstrNoteInfo[i].c_str());
#endif */

			return str;
		}
		return NULL;
	}
	void   SetstrInfo(int i,char * str)
	{
		if  (str)
			nstrNoteInfo[i] = str;
		else
			nstrNoteInfo[i] = "";
/*#ifdef _SERVER
			     printf("--设置(%s)(%d)脚本:%s --\n",Name,i,nstrNoteInfo[i].c_str());
#endif */
	}

	DWORD				m_ActionScriptID;		// Npc的行为脚本ID（使用时用这个来检索）
	
	DWORD				m_TrapScriptID;			// Npc的当前Trap脚本ID;

	int					m_nPeopleIdx;			// 锁住的对象人物索引
	int					m_nLastDamageIdx;		// 最后一次伤害的人物索引
	int					m_nLastPoisonDamageIdx;	// 最后一次毒伤害的人物索引
	int                 m_nLastBurnDamageIdx;	// 最后一次火伤害的人物索引 
	int					m_nObjectIdx;			// 对象物品

	// Npc的基本数据（未考虑装备、技能的影响）
	int					m_Experience;			// Npc被杀获得的经验
	int 				m_LifeMax;				// Npc的最大生命
	int					m_LifeReplenish;		// Npc的生命回复速度

	int					m_NuqiMax;		        // Npc怒气最大值
	int					m_NuqiReplenish;	    // Npc怒气回复速度

	int				    m_ManaMax;				// Npc的最大内力
	int					m_ManaReplenish;		// Npc的内力回复速度

	int				    m_StaminaMax;			// Npc的最大体力
	int					m_StaminaGain;			// Npc的体力回复速度
	int					m_StaminaLoss;			// Npc的体力下降速度

	int					m_AttackRating;			// Npc的命中点（身法决定原始命中率）
	int					m_Defend;				// Npc的闪避率
	int					m_FireResist;			// Npc的火抗性
	int					m_ColdResist;			// Npc的冷抗性
	int					m_PoisonResist;			// Npc的毒抗性
	int					m_LightResist;			// Npc的电抗性
	int					m_PhysicsResist;		// Npc的物理抗性

	int					m_FireResistMax;		// Npc的最大火抗性
	int					m_ColdResistMax;		// Npc的最大冰抗性
	int					m_PoisonResistMax;		// Npc的最大毒抗性
	int					m_LightResistMax;		// Npc的最大电抗性
	int					m_PhysicsResistMax;		// Npc的最大物理抗性

	int					m_WalkSpeed;			// Npc的行走速度
	int					m_RunSpeed;				// Npc的跑动速度
	int					m_JumpSpeed;			// Npc的跳跃速度
	int					m_AttackSpeed;			// Npc的外功攻击速度
	int					m_CastSpeed;			// Npc的内功施法速度
	int					m_VisionRadius;			// Npc的视野范围
	int					m_DialogRadius;			// Npc的对话范围
	int					m_ActiveRadius;			// Npc的活动范围
	int					m_HitRecover;			// Npc的受击回复速度
	int					m_Treasure;				// Npc丢落装备的数量(NPC.txt数据)
	BOOL				m_bClientOnly;			// 是否加载客户端NPC数据
//	int					m_AttackFrame;          // 外功攻击帧数						  
//	int					m_CastFrame;            // 内功攻击帧数
//	int					m_nCurrentMeleeSkill;	// Npc当前正执行的格斗技能
	int					m_nCurrentMeleeTime;	
	
	// AI参数
	int					m_AiMode;				// AI模式  挂机模式
	int					m_AiParam[MAX_AI_PARAM];// 用于AI模块计算AI
	int					m_AiAddLifeTime;

	int					m_HeadImage;

	BYTE				m_FightMode;			// 客户端处理动作用。 战斗模式
	BYTE				m_BayBan;			    // bayban
	BYTE				m_OldFightMode;
	BOOL				m_bExchangeServer;
	BYTE				m_ExItemId;             // 子母袋
	BYTE				m_ExBoxId;              // 扩展箱
	DWORD				m_SyncSignal;			// 同步信号
	KClientNpcID		m_sClientNpcID;			// 用于标明客户端npc是哪个region的第几个npc
	DWORD				m_dwRegionID;			// 本npc所在region的id
	KSyncPos			m_sSyncPos;
	BYTE				m_nPKFlag;	               //PK模式
	char				m_szChatBuffer[MAX_SENTENCE_LENGTH];
	int					m_nChatContentLen;
	int					m_nChatNumLine;
	int					m_nChatFontWidth;
	unsigned int		m_nCurChatTime;
	BYTE				m_nSleepFlag;
	int					m_nHurtHeight;
	int					m_nHurtDesX;
	int					m_nHurtDesY;
	int                 m_PiFenLoop;
	BYTE				m_nTongFlag;			// 是否有招人图标
	int                 m_IsbeSel;              // 是否选中了

	int					m_DeathFrame;			// 死亡帧数
	int					m_StandFrame;
	int					m_HurtFrame;
	int					m_AttackFrame;
	int					m_CastFrame;
	int					m_WalkFrame;
	int					m_RunFrame;
	int					m_StandFrame1;
	int					m_ReviveFrame;			// 重生帧数 时间
	int					m_SitFrame;
	int					m_JumpFrame;
private:
	bool                isRemoveMenu;
	DWORD				m_LoopFrames;			// 循环帧数
	int					m_nPlayerIdx;           // 玩家索引号

	int					m_JumpFirstSpeed;
	NPC_COMMAND			m_Command;				// 命令结构
	BOOL				m_ProcessAI;			// 处理AI标志
	BOOL				m_ProcessState;			// 处理状态标志
	int					m_XFactor;
	int					m_YFactor;
	int					m_JumpStep;
	int					m_JumpDir;
	int					m_SpecialSkillStep;		// 特殊技能步骤
	//NPC_COMMAND			m_SpecialSkillCommand;	// 特殊技能行为命令
	KNpcFindPath		m_PathFinder;
	BOOL				m_bActivateFlag;         //是否执行循环
	int					m_ResDir;
	KNpcRes				m_DataRes;				// Npc的客户端资源（图象、声音）
   	int                 m_DurFrame[20];
    unsigned int        uFlipTime[20];

	// 用于普通战斗npc冒血处理
	int			    	m_nBloodNo[defMAX_SHOW_BLOOD_NUM][2];
	//int				m_nBloodAlpha;
	char				m_szBloodNo[64];
private:
	BOOL				WaitForFrame(); //等待重生帧数
	BOOL				IsReachFrame(int nPercent); 
	void				DoStand();
	void				OnStand();
	void				DoRevive(); //重生
	void				OnRevive(); //重生
	void				DoWait();
	void				OnWait();
	void				DoWalk();
	void				OnWalk();
	void				DoRun();
	void				OnRun();
	void				DoSkill(int nX, int nY);
	int					DoOrdinSkill(KSkill * pSkill, int nX, int nY);
	void				OnSkill();
	void				DoAttack();
	void				DoMagic();
	void				DoJump();
	BOOL				OnJump();
	void				DoSit();
	void				OnSit();
	void				DoHurt(int nHurtFrames = 0, int nX = 0, int nY =0,int nRank=0);
	void				OnHurt();
	void				DoDeath(int nMode = 0,int nLastDamageIdx=0);
	void				OnDeath();
	void				DoDefense(); // 档格
	void				OnDefense();  //档格
	void				DoIdle();
	void				OnIdle();
//	有关格斗技能的------------------------------------------

	BOOL				DoSecMove(int nDistance); //是断魂刺 还是 瞬移？
	BOOL				DoManyAttack();
	void				OnManyAttack();
	BOOL				DoBlurAttack();
	BOOL				DoJumpAttack();
	BOOL				OnJumpAttack();
	BOOL				DoRunAttack();
	void				OnRunAttack();
	BOOL				CastMeleeSkill(KSkill * pSkill);
//-----------------------------------------------------------	
	void				OnPlayerTalk();
	void				DoSpecial1();
	void				OnSpecial1();
	void				DoSpecial2();
	void				OnSpecial2();
	void				DoSpecial3();
	void				OnSpecial3();
	void				DoSpecial4(int nX, int nY);
	void				OnSpecial4();
	void				Goto(int nMpsX, int nMpsY);
	void				RunTo(int nMpsX, int nMpsY);
	void				JumpTo(int nMpsX, int nMpsY);
	void				ServeMove(int nSpeed);
	void				ServeJump(int nSpeed);
//	void				Enchant(void);
//	void				ModifyEnchant(void* pData, int nAttrib);
	BOOL				NewPath(int nMpsX, int nMpsY);
	BOOL				NewJump(int nMpsX, int nMpsY);
	void                MovePos(int nMoveX, int nMoveY);
	BOOL				CheckHitTarget(int nAR, int nDf, int nIngore = 0,int nInHuLieAr=0);
	void                DoDrag(int nDragFrame, int nDesX, int nDesY);

	void				HurtAutoMove();

public:
	friend class KNpcAttribModify;
	friend class KThiefSkill;
	KNpc();
	~KNpc();

	//void                ClearEnhanceNote();
	DWORD               GetLoopFrames(){return m_LoopFrames;};
	void				SetActiveFlag(BOOL bFlag) { m_bActivateFlag = bFlag; };
	void				DoPlayerTalk(char *);
	void				CheckTrap();
//	int                 CheckWaiGua(char *exe_name);
	void                ReSkillEnhance(int nLeftSkill,int nIsAdd=-1);//重置对攻击技能的加成
	void                ReFullManaSkillEnhance(int nLeftSkill,int nLeftListidx);
	void                ReWhereSkillEnhance(int nActiveSkillID,int nCurSkillEnchance,int nIsAdd=-1);
	//int               NpcInitJinMaiVal();                            //经脉初始化
	int                 NpcNewInitJinMaiVal();
	void                UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData=NULL,BOOL nIfMagic=TRUE,int nTime=-1); //设置技能状态效果
	void				Init();
	void				Remove();
	//DWORD             TakeTrader(DWORD a,DWORD b); //取商
	//DWORD             TakeRemainder(DWORD a,DWORD b);	//取余
	void				Activate();
	//void				ActivateDrop();	//爆率的循环
	void                NewSetNpcEnChance();
	BOOL				IsPlayer();
	void                GetTongBanDamage(int* nMin, int* nMax);

	/*int               GetCurFuMoNpcNo(int idx);
	int                 GetCurFuMoIdx();
	int                 GetCurFoMoSkllLevel(int idx);
	*/
	void				SetFightMode(BOOL bFightMode);
	void				SetBayBan(BOOL bBayBan);//bayban
	void				TurnTo(int nIdx);
	void                setNpcDir(int nX2,int nY2);
	int                 GetAttackFrame(){return m_AttackFrame;};    
	int                 GetCastFrame(){return m_CastFrame;}; 
	void                SetAttackFrame(int nCurFrame){ m_AttackFrame=nCurFrame;};    
	void                SetCastFrame(int nCurFrame){m_CastFrame=nCurFrame;}; 
	void				SendSerCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				ProcCommand(int nAI);
	ISkill* 			GetActiveSkill();
	int                 GetSkillLeftTime(int nSkillId);//获取这个技能的状态的剩余时间
	int                 SetSkillLeftTime(int nSkillId,int nTime);   //设置这个技能的存活时间
	void                ClearOneSkillState(int nSkillId);

	BOOL				ProcessState();
	void				ProcStatus();
	void				ModifyAttrib(int nAttacker, void* pData);
	int					GetSkillLevel(int nSkillId);
	void				SetId(DWORD	dwID)	{ m_dwID = dwID;};
	BOOL				IsMatch(DWORD dwID)	{ return dwID == m_dwID; };	// 是否ID与该Index匹配
	BOOL				Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast = FALSE);				// 消耗内力体力等,如果OnlyCheckCanCost为TRUE,表示只是检查当前的内力等是否够消耗，并不实际的扣
	//void				SelfDamage(int nDamage);						// 自身的伤害，如牺牲攻击
	void				Load(int nNpcSettingIdx, int nLevel,int nSubWorld,int nBoss=0);						// 从TabFile中加载
	void				GetMpsPos(int * pPosX, int *pPosY,int *nMapid);
	BOOL				SetActiveSkill(int nSkillIdx);
	BOOL	            ChangeSkillAttackRadius(int nSkillIdx,int nDis);
	int				    GetSkillID(int nSkillIdx);
	void                UpdataNpcCurData(int nWonerIndex=0,int nPlace=-1);
	void                ReCalcNpcEquip(int nWonerIndex,int nEquipPlace=-1);
	BOOL                UpMagicAttrib(int i,int nWonerIndex);
	void				SetAuraSkill(int nSkillID);
	void				SetCamp(int nCamp);
	void				SetRankFF(int nRankFF);
	void                SetPlayFF(int nPlayFF);
	void				SetCurrentCamp(int nCamp); //设置当前阵营
	void				RestoreCurrentCamp();      //恢复阵营
	void				SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime = -1,int nIsEuq=0);	// 主动辅助技能与被动技能
	void                SetToolStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime=-1);
	void                SetToolNoStateEffect(int nLauncher, int nSkillID, int nLevel,int nTime);
	void				SysnCastSkillEffect(int nLauncher,int nSkillID,int nParam1,int nParam2,int nLevel, int nTime = -1,int mMaxBei=1);
	void				ClearStateSkillEffect();//清除NPC身上的非被动类的技能状态nMaxBeiLv
	void				ReCalcStateEffect();
	void				ClearNormalState();
	void				SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum);
	void				AppendSkillEffect(int nIsMaigc,BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData, int nEnChance);
	int					ModifyMissleLifeTime(int nLifeTime);
	int					ModifyMissleSpeed(int nSpeed);
	BOOL				ModifyMissleCollsion(BOOL bCollsion);
	void				RestoreNpcBaseInfo(); //Set Current_Data ;
	int                 GetNpcPepoleID(){return m_nPlayerIdx;};
//	int                 GetNpcIsReviveID(){return m_IsRevive;};   
	void				SetNpcIsRe(int nIsRe,int nTime) {m_IsRevive = nIsRe; m_ReviveFrame= nTime;};
	void				SetNpcIsReFrame(int nIsRe,int nTime) {m_IsRe = nIsRe; m_ReviveFrame= nTime;};
	void				RestoreState();
	void				ClearNpcState();
	BOOL				SetPlayerIdx(int nIdx);
	void				DialogNpc(int nIndex);
	void				Revive(); //c重生hon
	void				AddBaseLifeMax(int nLife);	// 增加基本最大生命点
	void				SetBaseLifeMax(int nLifeMax); // 设置基本最大生命点
	void				AddCurLifeMax(int nLife);
	void				AddBaseStaminaMax(int nStamina);// 增加基本最大体力点
	void				SetBaseStaminaMax(int nStamina);
	void				AddCurStaminaMax(int nStamina);
	void				AddBaseManaMax(int nMana);	// 增加基本最大内力点
	void				SetBaseManaMax(int nMana);
	void				AddCurManaMax(int nMana);
    void                NpcFuMoCastSkll(int nLauncher, int nParam1, int nParam2, int nWaitTime=0);
//	void				ResetLifeReplenish();		// 重新计算生命回复速度
//	void				CalcCurLifeMax();			// 计算当前最大生命点
//	void				CalcCurStaminaMax();		// 计算当前最大体力点
//	void				CalcCurManaMax();			// 计算当前最大内力点
	void				CalcCurLifeReplenish();		// 计算当前生命回复速度
	void				SetSeries(int nSeries);// 设定此 npc 的五行属性（内容还没完成）
    void	            SetFangAndHai(KChanelBaseInfo *nNpcInfo);//改变NPC的四防，用于触发BB
	void	            SetBaseDamage(int nMix,int nMax,int nType=0);
	void	            GetFangAndHai(KChanelBaseInfo *nNpcInfo);
//	int					GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam);
//	int					SkillString2Id(char * szSkillString);
	void				GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel);
//	void				InitNpcLevelData(KTabFile * pTabFile, int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel);
//	void				InitNpcBaseData(int nNpcTemplateId);
	void				SetPhysicsDamage(int nMinDamage, int nMaxDamage);	// 设定物理攻击的最大最小值
	void				SetBaseAttackRating(int nAttackRating);					// 设定攻击命中率
	void				SetBaseDefence(int nDefence);							// 设定防御力
//	void				SetBaseWalkSpeed(int nSpeed);							// 设定行走速度
//	void				SetBaseRunSpeed(int nSpeed);							// 设定跑步速度
	int					GetCurActiveWeaponSkill();
	void				LoadDataFromTemplate(void * nNpcTemp=NULL);//(int nNpcTemplateId, int nLevel);
    //unsigned int	    GetColor(const char* pString); //将字符串颜色转换成16进制
	//const               char* GetColorString(unsigned int nColor); //将16进制颜色转换成 字符串形式
	void				ReSetRes(int nMark);
	void				GetFrameCopyFromTemplate(int nNpcTemplateId, int nLevel);
	void				LoadFrameFromTemplate(void * nNpcTemp=NULL);//(int nNpcTemplateId, int nLevel);
//	int                 NpcInitJinMaiVal(); //经脉初始化
//	void                UseSkills(int sKillID,int sLevel,int nNpcIdx); //设置技能状态效果
//    int				GetPlayerIdx();
	int	                CheckMaps(char *nKey,int nMapIdx);
	int	                CheckAllItem(int nKeyCol,int nGen,int nDetail,int nPart);
	BOOL				TestMovePos(int& nMpsX,int& nMpsY,int& nLength, int nSpeed, BOOL bCanJumpOver);

	void                OnDrag();	                       // 拉扯 随机移动

	void                SetClientSpr(char *nSprPath,int nxLeft,int nyTop,int nzPos=0,int i=0,char *ncPart=NULL);//设置 同时运行的SPR动画
	void				SetSleepMode(BOOL bSleep) { m_nSleepFlag = bSleep; /*m_DataRes.SetSleepState(bSleep);*/};
	void				SetNpcState(int* pNpcState);
	void				ActivateWaiGua();
	void				RemoveRes();
	void			    ClientShowMsg(const char *Msg);
	void				ProcNetCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				Paint();
	int					PaintInfo(int nHeightOffset, bool bSelect, int nFontSize = 12, DWORD	dwBorderColor = 0XFF000000);
	void				PaintHonor(int nbtRankFFId,int nMpsX,int nMpsY,int i); //spr称号
	void				PaintPifeng(int m_PifengType,int nMpsX,int nMpsY,int i); //spr披风称号
//	void				PaintHonor(char szString[16], int nHeightOffset, int nFontSize = 12, DWORD	dwBorderColor = 0);
	int					PaintChat(int nHeightOffset);
	int					SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength);//自己NPC说话
	int					PaintLife(int nHeightOffset, bool bSelect);
	int					PaintMana(int nHeightOffset);
	void				DrawBorder();
	int					DrawMenuState(int n);
	void				DrawBlood();	//绘制血条和名字在固定位置
	BOOL				IsCanInput() { return m_ProcessAI; };//人物AI 标志
	void				SetMenuState(int nState, char *lpszSentence = NULL, int nLength = 0);	// 设定头顶状态
	int					GetMenuState();				// 获得头顶状态
	DWORD				SearchAroundID(DWORD dwID);	// 查找周围9个Region中是否有指定 ID 的 npc
	void				SetSpecialSpr(char *lpszSprName);// 设定特殊的只播放一遍的随身spr文件
	void				SetFrameSpr(char *lpszSprName, int nX, int nY, int nHeight,int mInterval=0);
	void				SetInstantSpr(int nNo);
	int					GetNormalNpcStandDir(int nFrame);
	KNpcRes*			GetNpcRes(){return NULL;/*&m_DataRes;*/};  //得到NPC的外观
  //KImageParam	        imgParam;  
	int                 GetNpcPate();
	int                 GetNpcPatePeopleInfo();

	// 冒血处理
	void				ClearBlood(int i);
	void				SetBlood(int nNo);
	int				    PaintBlood(int nHeightOffset);	//绘制冒血
	int	                PaintOther();
	int	                PaintNewBlood(int nHeightOffset,int Val);
	void                ApplySynNpcInfo(DWORD nNpcDwid);
};

//导致内存飙升 MAX_NPC
extern  KNpc *Npc;//Npc[MAX_NPC];
#endif


