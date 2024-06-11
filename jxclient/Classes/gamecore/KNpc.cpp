//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
//#include <crtdbg.h>
#include "KNpcAI.h"
#include "KSkills.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KRegion.h"
#include "KNpcTemplate.h"
#include "KItemSet.h"
//#include "KForBitGua.h"
///////原来没有的/////////////////////////////////////
#include  "KPlayerFaction.h"
#include  <time.h>
//#include "KSubWorld.h"
#include "KSubWorldSet.h"

////////////////////////
#ifdef WIN32
#include <Tlhelp32.h>
#include "tchar.h"
#include "psapi.h"
#endif
////////////////////////////////////////////////////////
//客户端
//#include "../../../Headers/IClient.h"
#include "CoreShell.h"
//#include "Scene/KScenePlaceC.h"
//#include "KIme.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
#include "ImgRef.h"
#include "engine/Text.h"

//extern iRepresentShell*	g_pRepresentShell;
extern KImageStore2 m_ImageStore;

#include "KNpcAttribModify.h"
#include "CoreUseNameDef.h"
#include "gamescene/ObstacleDef.h"
#include "KThiefSkill.h"
/*
#ifdef _STANDALONE
#include "KThiefSkill.cpp"
#endif
*/
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

extern KLuaScript		*g_pNpcLevelScript;

#define	ATTACKACTION_EFFECT_PERCENT		60	// 发技能动作完成百分之多少才真正发出来
#define	MIN_JUMP_RANGE					0
#define	ACCELERATION_OF_GRAVITY			10
#define		SHOW_CHAT_COLOR				0xffffffff
#define		SHOW_BLOOD_COLOR			0x00ff0000
#define		defMAX_SHOW_BLOOD_TIME		50
#define		defSHOW_BLOOD_MOVE_SPEED	1

#define		SHOW_LIFE_WIDTH				38
#define		SHOW_LIFE_HEIGHT			3

#define		SHOW_SPACE_HEIGHT			1

#define		FIND_PATH_DISTANCE		1
//-----------------------------------------------------------------------
#define FRAME2TIME				18
//#define AEXP_INTERVAL			5400
#define	GAME_UPDATE_TIME		10
//#define	GAME_SYNC_LOSS			100
//#define	STAMINA_RECOVER_SCALE	4
// 区域的宽高（格子单位）
#define	REGIONWIDTH			SubWorld[m_SubWorldIndex].m_nRegionWidth
#define	REGIONHEIGHT		SubWorld[m_SubWorldIndex].m_nRegionHeight
// 格子的宽高（像素单位，放大了1024倍）
#define	CELLWIDTH			(SubWorld[m_SubWorldIndex].m_nCellWidth << 10)
#define	CELLHEIGHT			(SubWorld[m_SubWorldIndex].m_nCellHeight << 10)
// 当前区域
#define	CURREGION			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex]
// 相邻区域的索引
#define	LEFTREGIONIDX		CURREGION.m_nConnectRegion[2]
#define	RIGHTREGIONIDX		CURREGION.m_nConnectRegion[6]
#define	UPREGIONIDX			CURREGION.m_nConnectRegion[4]
#define	DOWNREGIONIDX		CURREGION.m_nConnectRegion[0]
#define	LEFTUPREGIONIDX		CURREGION.m_nConnectRegion[3]
#define	LEFTDOWNREGIONIDX	CURREGION.m_nConnectRegion[1]
#define	RIGHTUPREGIONIDX	CURREGION.m_nConnectRegion[5]
#define	RIGHTDOWNREGIONIDX	CURREGION.m_nConnectRegion[7]

#define	LEFTREGION			SubWorld[m_SubWorldIndex].m_Region[LEFTREGIONIDX]
#define	RIGHTREGION			SubWorld[m_SubWorldIndex].m_Region[RIGHTREGIONIDX]
#define	UPREGION			SubWorld[m_SubWorldIndex].m_Region[UPREGIONIDX]
#define	DOWNREGION			SubWorld[m_SubWorldIndex].m_Region[DOWNREGIONIDX]
#define	LEFTUPREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTUPREGIONIDX]
#define	LEFTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTDOWNREGIONIDX]
#define	RIGHTUPREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTUPREGIONIDX]
#define	RIGHTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTDOWNREGIONIDX]

#define	CONREGION(x)		SubWorld[m_SubWorldIndex].m_Region[CURREGION.m_nConnectRegion[x]]
#define	CONREGIONIDX(x)		CURREGION.m_nConnectRegion[x]

//-----------------------------------------------------------------------
// Npc[0]不在游戏世界中使用，做为一个NpcSet用于添加新的NPC。
//原来有的 MAX_NPC
KNpc	*Npc= NULL;//Npc[MAX_NPC];

KSubWorldSet    *M_SubWorldSet;
//KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点

//-----------------------------------------------------------------------

KNpc::KNpc()
{

	Init();
}


KNpc::~KNpc()
{//析构函数 释放资源
//#ifdef _SERVER
   	nEnhanceInfo.clear();
	nstrNoteInfo.clear();
//#endif
}

void KNpc::Init() //npc数据初始化
{
	m_dwID = 0;
	m_AdjustColorKind = kind_color_physics;
	m_IsRevive = 0;
	m_IsRe = 0;  //是否可以重生
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_Series = series_metal;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_Doing = do_stand;
	m_Height = 0;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = 0;
	m_SubWorldIndex =0;
	m_RegionIndex = -1;
	m_Experience = 0;
	m_ActiveSkillID = 0;
	m_ActiveSkListIndex=0;
	m_IsMoreAura=false;
	m_SkillParam1 = 0;
	m_SkillParam2 = 0;
	m_KillNumber  =0;  //杀人数量 排名调用
	//ZeroMemory(m_nFuMoNum,sizeof(m_nFuMoNum));

	//m_isClearSpr = false;
	m_nCurJiHuo=0;
	m_ZhenYuan=0;             //真元值
	//m_WhereSer=1;             //那个服务器
    m_JinMaiBingJia=0;        //经脉兵甲值
	IsCreatTongBan=0;         //是否可以召唤同伴
	//////临时变量，不加入数据库///////
	m_ZhuaVal = 0;
	m_TongBanNum = 0;
	m_IsSerLock  = 0;
    m_njxb=0;    //携带的金币
	m_njb=0;
	m_IsDoing=0;
	m_IsSynDir=0;
//	ZeroMemory(m_WarTongNamea,sizeof(m_WarTongNamea)); 
//	ZeroMemory(m_WarMaster,sizeof(m_WarMaster)); 
//	ZeroMemory(m_ChenHaoName,sizeof(m_ChenHaoName)); 	
//	ZeroMemory(m_GuishuName,sizeof(m_WarMaster)); //同伴归属
	ZeroMemory(&m_ExpState, sizeof(m_ExpState));
	ZeroMemory(&m_DoScriptState, sizeof(m_DoScriptState));
	ZeroMemory(&m_randmove, sizeof(m_randmove));
	ZeroMemory(&m_MapUseModel, sizeof(m_MapUseModel));
	ZeroMemory(&_NpcShadow, sizeof(_NpcShadow));
	/*_EnhanceInfo::iterator it;
	for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it )
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}
    nEnhanceInfo.clear();*/

    //t_sprintf(m_GuishuName,"系统");
	SetstrInfo(STR_GUISHU_NAME,"系统");
	m_GuiShuDwid = 0;
	m_IsDel=FALSE;
//	m_WarShuishou=0;   //税收
//	m_Warzhi=0;
	m_IsWarCity=-1;
	m_IsInCity=0;
	m_nMissionGroup =-1;
/*	m_WarFucheng=0;   //副成
    m_WarZuoHu=0;     //左护法
	m_WarYouHu=0;     //右护法
	m_WarTaishi=0;     //太史
	m_WarZhongShu=0;   //中书
	m_WarShangShu=0;   //尚书
	m_WarMiShu=0;      //秘书
	m_WarTaiLe=0;      //太乐
	*/
	m_WarCityGX=0;     //个人贡献
	m_ReviceNum=0;
//    m_WarIsGong=-1;      //属于攻方
//    m_WarIsShou=-1;      //属于守方
    m_RestNameCount=0;     //改名的次数
    m_CJtaskID=0;        //172抽奖专用
	//m_IsWaiGua=0;        //是否使用外挂
	m_GameliveTime = -1; //NPC存活的时间
	m_liveType = 0;       //时间类型
    m_TempliveTime = 0;
   /*
    m_GuziYajing=0;      //押金
    m_GuziDianshu=0;     //点数
    m_GuziZhuang=0;      //是否庄家
	m_GuziMenber=0;
	m_IsShuai=0;         //是否已经甩了
	m_ZuoWeihao=0;       //座位号
	*/
	//IsDeath=0;           //是否死亡
	IsLuKey=1;
	IsJinYan=0;
	m_GoldLucky=1;
	IsJinQian=1;
	//m_IsTuiGuang=0;
    m_IsVip=0;                  //会员
    m_IsXingYunXing=0;

	m_IsgetSkill=0;             //是否拾取技能状态
    m_mMapX=0;
    m_mMapY=0;
	IsExeGoldScript=1;          //默认执行全局脚本
	IsCreatBoss=1;              //是否触发BOSS
    //t_sprintf(m_ItmeInfo,"暂无数据");
	m_nIsOver  = FALSE;
	m_bLockNpcDwID      = 0;
////////////隐藏////////////////
	m_BtnFlag=0;                //是否插旗状态
	m_BtnFindPath=FALSE;
	/*KIniFile nClient;
	if (nClient.Load("//config.ini"))
	{
		nClient.GetInteger("Client","Represent",3,&m_ISrepresent2);
	}
	nClient.Clear();*/
	//m_ISrepresent2=2;

    m_bIsHideNpc        = FALSE;
    m_bIsHidePlayer     = FALSE;
    m_bIsHideMissle     = FALSE;
	m_bIsHideLife       = FALSE;
	m_bIsHideTong		= TRUE;
	m_bIsHideNuqi	    = FALSE;



//	ZeroMemory(m_PicPath,sizeof(m_PicPath));
//	ZeroMemory(m_ScriptPicPath,sizeof(m_ScriptPicPath));	
	m_nChatContentLen = 0;
	m_nCurChatTime    = 0;
	m_nChatNumLine    = 0;
	m_nChatFontWidth  = 0;
	m_nStature        = 0;
	ZeroMemory(TongName,sizeof(TongName)); //清空帮会名？
	m_nFigure = -1;
	//m_IsHaveAttack=0;  //是否设置为攻击无效了
	//m_AttackerDwid=0;  //上次攻击着的DWID	

    m_IsHaveAttack=0;  //是否设置为攻击无效了
	m_AttackerDwid=0;  //上次攻击着的DWID
	m_CurrentLife    = 100;		// Npc的当前生命
	m_CurrentLifeMax = 100;		// Npc的当前生命最大值
	m_CurrentLifeReplenish = 0;	// Npc的当前生命回复速度
	m_CurrentLifeReplenish_p = 0;
    m_CurrentLifeDamage = 0;
	m_CurPoisonDamage   = 0;
	m_CurFireDamage     = 0;
	m_CurrentNuQi = 0;		    // Npc的当前怒气
	m_CurrentNuQiMax = 100;		// Npc的当前怒气最大值
	m_CurrentNuQiReplenish = 1;	// Npc的当前怒气回复速度
    
//	m_CurrentFuMoVal    =0;		        // Npc的当前附魔值
//	m_CurrentFuMoValMax =500;
//	m_CurrentFuMoValReplenish =1;	    // Npc的当前附魔值回复速度
    //m_FuMoTimesVal      =5;		    // Npc的当前附魔持续时间值（间隔值）
	//m_AttackState=FALSE;
	m_CurrentMana    = 100;		// Npc的当前内力
	m_CurrentManaMax = 100;		// Npc的当前最大内力
	m_CurrentManaReplenish = 0;	// Npc的当前内力回复速度
	m_CurrentManaReplenish_p = 0;
	m_CurrentStamina = 100;		// Npc的当前体力
	m_CurrentStaminaMax = 100;	// Npc的当前最大体力
	m_CurrentStaminaGain = 0;	// Npc的当前体力回复速度
	m_CurrentStaminaLoss = 0;	// Npc的当前体力下降速度
	m_CurrentAttackRating = 100;// Npc的当前命中率点
    m_CurrentSkillMingZhong  = 0;
	m_CurrentDefend = 10;		// Npc的当前闪避
	m_CurrentWalkSpeed = 5;		// Npc的当前走动速度
	m_CurrentRunSpeed = 10;		// Npc的当前跑动速度
	m_CurrentJumpSpeed = 12;	// Npc的当前跳跃速度
	m_CurrentJumpFrame = 40;	// Npc的当前跳跃时间
	m_CurrentAttackSpeed = 0;	// Npc的当前攻击速度
	m_CurrentCastSpeed = 0;		// Npc的当前施法速度
	m_CurrentVisionRadius = 40;	// Npc的当前视野范围
	m_CurrentAttackRadius = 30;	// Npc的当前攻击范围
	m_CurrentHitRecover = 0;	// Npc的受伤动作时间
	m_CurrentHitNpcRecover=0;
	m_CurrentHitRank=0;
    m_CurrentStunRank_p=0;          // 眩晕几率减少
	m_CurrentFireResistMax=75;		// Npc的当前最大火抗性
	m_CurrentColdResistMax=75;		// Npc的当前最大冰抗性
	m_CurrentPoisonResistMax=75;	// Npc的当前最大毒抗性
	m_CurrentLightResistMax=75;	    // Npc的当前最大电抗性
	m_CurrentPhysicsResistMax=75;	// Npc的当前最大物理抗性

	m_CurrentAddPhysicsDamage = 0;	// Npc的当前物理伤害直接加的点数

    m_CurrentAddPhysicsDamageP= 0;     // 当前被动外普百分比
    m_CurrentAddFireDamagev= 0;        // 当前被动外火点
    m_CurrentAddColdDamagev= 0;        // 当前被动外冰点 
    m_CurrentAddLighDamagev= 0;        // 当前被动外雷点
    m_CurrentAddPoisonDamagev= 0;      // 当前被动外毒点
	
    m_CurrentAddmagicphysicsDamage= 0; // 当前被动内普点
    m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
    m_CurrentAddmagicColdDamagicv= 0;  // 当前被动内冰点
    m_CurrentAddmagicFireDamagicv= 0;  // 当前被动内火点  
	m_CurrentAddmagicLightDamagicv= 0; // 当前被动内雷点
    m_CurrentAddmagicPoisonDamagicv= 0;// 当前被动内毒点
	m_Currentbaopoisondmax_p=0;
	m_CurrentPoisondamagereturnV=0;
	m_CurrentPoisondamagereturnP=0;
	m_CurrentReturnskillp=0;
	m_CurrentIgnoreskillp=0;
	m_CurrentReturnresp=0;
    m_CurrentCreatnpcv=0;
	m_CurrentAllJiHuo=0; //是否全身激活
	m_CurrentdanggeRate = 0;  //档格
    m_CurrentzhongjiRate = 0; // 重击

    m_Me2metaldamage_p= 0;             //=对金系伤害增加：#d1+%
    m_Metal2medamage_p= 0;             //=减少来自金系的伤害：#d1-%
    m_Me2wooddamage_p= 0;             //=对木系伤害增加：#d1+%
    m_Wood2medamage_p= 0;              //=减少来自木系的伤害：#d1-%
    m_Me2waterdamage_p= 0;              //=对水系伤害增加：#d1+%
    m_Water2medamage_p= 0;              //=减少来自水系的伤害：#d1-%
    m_Me2firedamage_p= 0;              //=对火系伤害增加：#d1+%
    m_Fire2medamage_p= 0;             //=减少来自火系的伤害：#d1-%
    m_Me2earthdamage_p= 0;             //=对土系伤害增加：#d1+%
    m_Earth2medamage_p= 0;             //=减少来自土系的伤害：#d1-%

	m_Staticmagicshield_p=0;

	m_CurrentPoisonTime=0;
	m_nCurNpcLucky =100;
    m_CurrentUpExp=0;
	m_Dir = 0;					// Npc的方向
	m_JumpStep = 0;
	m_JumpDir = 0;			
	m_MapZ = 0;					// Npc的高度
	m_HelmType = 1;				// Npc的头盔类型
	m_ArmorType = 1;			// Npc的盔甲类型
	m_WeaponType = 1;			// Npc的武器类型
	m_HorseType = -1;			// Npc的骑马类型  (默认是没骑马)
	m_bRideHorse = FALSE;		// Npc是否骑马
	m_MaskType = 0;				// Npc 面具功能
	m_PifengType =0;			// Npc ，功能
	m_ChiBangType=0;            // 翅膀类型
	m_MaskMark = 0;				// mat na
//	m_IsFuMo   = 0;
	m_BaiTan = 0;
//	ZeroMemory(ShopName,sizeof(ShopName));
	m_RideState = 0;			// Npc骑马时间限制
	ZeroMemory(Name, 32);		// Npc的名称
	ZeroMemory(_clientName,32);
	m_NpcSettingIdx = 0;		// Npc的设定文件索引
	m_CorpseSettingIdx = 0;		// Body的设定文件索引
	//ZeroMemory(ActionScript,sizeof(ActionScript)); //脚本

	nstrNoteInfo.clear();

	m_ActionScriptID = 0;				    
	m_TrapScriptID = 0;
	ZeroMemory(m_TmpAuraID,sizeof(m_TmpAuraID));
	ZeroMemory(m_ExtSkill,sizeof(m_ExtSkill));
	m_btRankId					= 0; //文字称号
	m_NpcTitle                  = 0;
	m_CurNpcTitle               = 0;
	m_btRankFFId				= 0; //spr称号
	m_AutoplayId                = 0; //guaji
	m_ExItemId					= 0; // hanh trang
	m_ExBoxId					= 0; // ruong mo rong
	nRankInWorld				= 0; //世界排名
	nLevelInWorld               = 0; //等级排行
	nRepute						= 0;
	nPKValue					= 0;
	nReBorn						= 0;

	m_LifeMax					= 100;		// Npc的最大生命
	m_LifeReplenish				= 0;		// Npc的生命回复速度

	m_NuqiMax                   = 100;		// Npc怒气最大值
	m_NuqiReplenish             = 1;	    // Npc怒气回复速度

	m_ManaMax					= 100;		// Npc的最大内力
	m_ManaReplenish				= 0;		// Npc的内力回复速度
	m_StaminaMax				= 100;		// Npc的最大体力
	m_StaminaGain				= 0;		// Npc的体力回复速度
	m_StaminaLoss				= 0;		// Npc的体力下降速度
	m_AttackRating				= 100;		// Npc的命中率 点
	m_Defend					= 10;		// Npc的闪避率
	m_WalkSpeed					= 6;		// Npc的行走速度
	m_RunSpeed					= 10;		// Npc的跑动速度
	m_JumpSpeed					= 12;		// Npc的跳跃速度
	m_AttackSpeed				= 0;		// Npc的攻击速度
	m_CastSpeed					= 0;		// Npc的施法速度
	m_VisionRadius				= 40;		// Npc的视野范围
	m_DialogRadius				= 124;		// Npc的对话范围
	m_HitRecover				= 12;		// Npc的受伤动作恢复的时间帧数
	m_nPeopleIdx				= 0;
	m_LoopFrames				= 0;
	m_WalkFrame					= 12;
	m_RunFrame					= 15;
	m_StandFrame				= 15;
	m_DeathFrame				= 15;
	m_HurtFrame					= 10; //受伤动作
	m_AttackFrame				= 20; //外功攻击速度帧数
	m_CastFrame					= 20; //内功攻击速度帧数
	m_SitFrame					= 15;
	m_JumpFrame					= 40;
	m_AIMAXTime					= 25;
	m_NextAITime				= 0;
	m_ProcessState				= 1;
	m_ReviveFrame				= 1080;  //重生时间
	m_bExchangeServer			= FALSE;
	m_bActivateFlag				= FALSE;
	m_FightMode					= 0;
	m_OldFightMode				= 0;
	m_BayBan					= 0;
	m_SyncSignal				= 0;
	m_PiFenLoop                 = 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
	//uFlipTime                   = 0;
	m_nPKFlag					= 0;
	m_nSleepFlag				= 0;   //睡眠状态
	memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
/*
	for(int i=0;i<50;++i)
	{
		m_nBloodNo[i][0]		= 0;
		m_nBloodNo[i][1]		= 0;
	}

	m_nBloodAlpha			    = 0;
	m_nBloodTime[0]				= 0;
	m_szBloodNo[0]				= 0;
	*/
	//m_nBloodAlpha			    = 0;
	memset(m_nBloodNo, 0, sizeof(m_nBloodNo));
	//memset(m_nBloodAlpha, 0, sizeof(m_nBloodAlpha));
	//memset(m_nBloodTime, 0, sizeof(m_nBloodTime));
	memset(m_szBloodNo, 0, sizeof(m_szBloodNo));
	m_nTongFlag					= 0;
	m_IsbeSel                   = 0;
	m_nLastPoisonDamageIdx = 0;
	m_nLastBurnDamageIdx = 0;
	m_nLastDamageIdx = 0;
	//m_bHaveLoadedFromTemplate = FALSE;
	m_bClientOnly = FALSE;
	isRemoveMenu  = false;
}

ISkill* KNpc::GetActiveSkill()
{
	//CCAssert(m_ActiveSkillID < MAX_SKILL,"");
	if  (m_ActiveSkillID <=0||m_ActiveSkillID >= MAX_SKILL)
		return NULL;

	int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
	if (nCurLevel > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	else 
		return NULL;
};

void KNpc::SetCurrentCamp(int nCamp)
{
	m_CurrentCamp = nCamp;

}

void KNpc::SetCamp(int nCamp)
{
	m_Camp = nCamp;//实际阵营
}
//设置恢复当前阵营为 实际阵营
void KNpc::RestoreCurrentCamp()
{
	m_CurrentCamp = m_Camp;
}

#define		NPC_SHOW_CHAT_TIME		15000
int		IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer);

/*void KNpc::ActivateDrop()
{
#ifdef _SERVER

	   if (strstr(nDropFlie,".ini"))
	   {
		   if (g_pNpcTemplate[m_NpcSettingIdx][0])
	          g_pNpcTemplate[m_NpcSettingIdx][0]->InitDropRate(m_Index,nDropFlie);  //白明爆率修改	
		   else if (g_pNpcTemplate[m_NpcSettingIdx][m_Level])
		   	     g_pNpcTemplate[m_NpcSettingIdx][m_Level]->InitDropRate(m_Index,nDropFlie);  //白明爆率修改	   
	   }

#endif
}*/
//取商
/*DWORD KNpc::TakeTrader(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//DWORD nYuShu=0;
	__asm
	{
		    mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,eax
			//mov nYuShu,edx
	}
	return nRet;
}
//取余数
DWORD KNpc::TakeRemainder(DWORD a,DWORD b)
{
	DWORD nRet = 0;
//	DWORD nYuShu=0;
	__asm
	{
		    mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,edx
			//mov nYuShu,edx
	}
	return nRet;
}
 */
  


void KNpc::Activate()
{
	// 不存在这个NPC
	if (m_Index<=0)
	{
		return;
	}

	// 切换地图中，不处理,跨服
	if (m_bExchangeServer)  
	{
		return;
	}
	
	// Check here
	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag  会卡一下
		return;
	}

/*#ifndef _SERVER
	// 超出同步距离删除Npc，9屏，32个格子
	if (!IsPlayer() && 
		(NpcSet.GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= 32 || NpcSet.GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= 32))
	{
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY,obj_npc);
		m_RegionIndex = -1;
		return;
	} 
	
	// lbh_06_06_20：强制同步次数超过限制而仍未收到服务端的该npc同步协议，删除些npc
	if (!IsPlayer() && SubWorld[0].m_dwCurrentTime - m_SyncSignal > 120)
	{
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY,obj_npc);
		m_RegionIndex = -1;
		return;
	}
#endif */

	++m_LoopFrames;

	if (m_ProcessState && m_CurrentLife>0)
	{
		if (ProcessState())      //属性状态的持续循环--会导致mainloop错误，不明原因
			return; 
	}

	if (m_ProcessAI)
	{
		NpcAI.Activate(m_Index); //npcAI		
	}
	ProcCommand(m_ProcessAI);    //使用技能等 
//------------------------------------有错
	ProcStatus();                     //动作协议状态     ----会导致mainloop错误，不明原因
//-------------------------------------------------------------------------
	//面具持续状态
 if (m_Kind == kind_player)
 {
	if (m_MaskType > 0 && m_MaskMark != 0 && m_MaskMark != m_MaskType)// 面具
	{//防止出错  恢复原样
		ReSetRes(1);
		m_MaskMark = 0;
	}
	else if (m_MaskType > 0 && !m_MaskMark)
	{//设置变样 带了面具
		ReSetRes(0);
		m_MaskMark = m_MaskType;
	}
	else if (m_MaskType == 0 && m_MaskMark)
	{//恢复原样 脱了面具
		ReSetRes(1);
		m_MaskMark = 0;
	} 
	
	if (m_MaskMark >0)//获取NPC的模样
	{
		//m_MaskMark = 2;
		GetFrameCopyFromTemplate(m_MaskType,m_Level);
	}
 } 
  if (m_Kind == kind_player)
  {
//实时更新基本攻击
	  Player[CLIENT_PLAYER_INDEX].SetNpcDamageAttrib();	
    //重新设置技能加成
	//NewSetNpcEnChance();
	ReFullManaSkillEnhance(Player[CLIENT_PLAYER_INDEX].GetLeftSkill(),Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx());
  }
///------------------------------------
	if (m_RegionIndex == -1)
		return;
//	HurtAutoMove();	  //伤害移动？
	int	nMpsX, nMpsY;
//------------------------------------
//------------------------------------
	m_DataRes.SetAction(m_ClientDoing);  //动作行为
	
	m_DataRes.SetRideHorse(m_bRideHorse);//是否骑马
	m_DataRes.SetArmor(m_ArmorType);     //衣服
	m_DataRes.SetHelm(m_HelmType);       //头发
	m_DataRes.SetHorse(m_HorseType);     //马匹类型
	m_DataRes.SetWeapon(m_WeaponType);   //武器
	
   if (m_Kind==kind_player)
   { 
	   m_DataRes.SetPifeng(m_PifengType);   //披风 m_PifengType
	   //m_DataRes.SetChiBang(m_ChiBangType);
   } 
	//客户端处理技能产生的状态的特效
	m_DataRes.SetState(&m_StateSkillList, &g_NpcResList);      //光环持续状态

	if (m_CurNpcTitle>0)
	   m_DataRes.SetSprState(m_CurNpcTitle,&g_NpcResList);	   //自定义的SPR状态
	else
	   m_DataRes.SetSprState(m_NpcTitle,&g_NpcResList);
	
	if (g_GameWorld)
	{//主场景正常运行中
	  if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index/* && !Player[CLIENT_PLAYER_INDEX].m_bExchangeServer*/)   //如果是客户端本人     if (m_Kind==kind_player)
	  {//客户端本人
		SubWorld[0].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, TRUE);  //设定人物外观的位置
	  }
	  else
	  {//其他NPC
		SubWorld[0].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);
	  }
	}
	// client npc 时间计数处理：不往后跳  ()
	if ( m_Kind == kind_bird || m_Kind == kind_mouse || m_Kind == kind_normal)
		 m_SyncSignal = SubWorld[0].m_dwCurrentTime;   //同步当前地图时间帧数

	if (m_nChatContentLen > 0)
	{
		if (IR_GetCurrentTime() - m_nCurChatTime > NPC_SHOW_CHAT_TIME)
		{
			m_nChatContentLen = 0;
			m_nChatNumLine = 0;
			m_nChatFontWidth = 0;
			m_nCurChatTime = 0;
		}
	}
}

//协议状态 m_Doing
void KNpc::ProcStatus()
{
	if (m_bExchangeServer)  //跳转地图时 不运行
		return;

	switch(m_Doing)
	{
	case do_stand:
		OnStand(); 
		break;
	case do_run:
		OnRun();    //有导致core mainloop错误
		break;
	case do_walk:
		OnWalk();   //有导致core mainloop错误
		break;
	case do_attack: //做攻击动作时使用技能
	case do_magic:  //做攻击动作时使用技能
		OnSkill();
		break;
	case do_sit:
		OnSit();
		break;
	case do_jump:
		OnJump();
		break;
	case do_hurt:
		OnHurt();
		break;
	case do_revive:
		OnRevive();
		break;
	case do_death:
		OnDeath();
		break;
	case do_defense:  //档格
		OnDefense();
		break;
	case do_special1: //放技能
		OnSpecial1();
		break;
	case do_special2:  //偷取技能
		OnSpecial2();
		break;
	case do_special3:
		OnSpecial3();
		break;
	case do_special4:
		OnSpecial4();
		break;
	case do_manyattack:
		OnManyAttack();
		break;
	case do_runattack: //断魂刺
		OnRunAttack();
		break;
	case do_jumpattack:
		OnJumpAttack();
		break;
	case do_idle:
		OnIdle();
		break;
	case do_stall:
          break;
	case do_movepos:
		break;
	case do_knockback:
		break;
	case do_drag:
		OnDrag();
		break;
	case do_rushattack:
		break;
	case do_runattackmany:
		break;
		  /*
		  
			do_stall,
			do_movepos,		// 瞬间移动
			do_knockback,	// 震退
			do_drag,		// 拉扯过来
			do_rushattack,	// 冲砍
	do_runattackmany, //冲刺多人 
		  */
	default:
		break;
	}
}

void KNpc::ProcCommand(int nAI)  //协议命令？
{
	// CmdKind < 0 表示没有指令	交换地图也不处理
	if (m_Command.CmdKind == do_none || m_bExchangeServer)
		return;

	if (nAI)
	{
		if (m_RegionIndex < 0)
			return;
		switch (m_Command.CmdKind)
		{
		case do_stand:
			DoStand();
			break;
		case do_walk:
			Goto(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_run:
			RunTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_jump:
			JumpTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_skill:  //客户端服务器端共用执行发技能
		{
			if (int nSkillIdx = m_SkillList.FindSame(m_Command.Param_X))  //在技能列表中找到技能ID
			{
				if  (SetActiveSkill(nSkillIdx))//设置为攻击技能
				{
				}
				DoSkill(m_Command.Param_Y, m_Command.Param_Z); //在这个NPC索引上使用技能
			}
			else
			{
			  DoStand();  //站着
			}
		}
			break;
		case do_sit:
			DoSit();
			break;
		case do_defense:  //档格
			DoDefense();
			break;
		case do_idle:     // 喘气
			DoIdle();
			break;
		case do_hurt: // 伤害
			DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;	
		case do_revive://站着？
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效
			}
			break;
		case  do_special4://转换地图
				DoSpecial4(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_stall:
			break;
		case do_movepos:
			break;
		case do_knockback:
			break;
		case do_drag:
			break;
		case do_rushattack:
			break;
		case do_runattackmany:
			break;

		}
	}
	else
	{
		switch(m_Command.CmdKind)
		{
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;
		case do_revive:  //重生
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效
			}
			break;
		case do_stall:
			break;
		case do_movepos:
			break;
		case do_knockback:
			break;
		case do_drag:
			break;
		case do_rushattack:
			break;
		case do_runattackmany:
			break;

		default:
			break;
		}
	}
	m_Command.CmdKind = do_none;
}
//无限同步状态数据
BOOL KNpc::ProcessState()
{
	int nRet = FALSE;
	if (m_RegionIndex < 0)
		return FALSE;
    if (!(m_LoopFrames % GAME_UPDATE_TIME)) //每半秒执行一次
	//if (m_LoopFrames - (m_LoopFrames>>4<<4)==0)
	{
////////////////////////////////////////////////////////////
        if (m_Kind==kind_player)
		{
			Player[CLIENT_PLAYER_INDEX].m_ItemList.CheckBianShiItemTime();
		}
//---------------------光环技能同步----------------------------------------------------
		/*if (m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态
			//int nListidx = m_SkillList.FindSame(m_ActiveAuraID);
			int nLevel    = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);
			int nEnChance = m_SkillList.GetEnChance(m_ActiveAuraIndex);
			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
				//_ASSERT(m_ActiveAuraID < MAX_SKILL && nLevel < MAX_SKILLLEVEL)			
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
//-----------------幸运星技能同步--------------------------------------
		if (IsPlayer() && m_ActiveXinYunXingID>0 && m_ActiveXinYunXingID < MAX_SKILL)  //主要用于技能的 主动技能
		{//只能开一个光环状态

			int nLevel =1;// m_SkillList.GetCurrentLevel(m_ActiveXinYunXingID);
			int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(m_ActiveXinYunXingID));
			if (nLevel > 0 && nLevel < MAX_SKILLLEVEL)
			{
				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
			//	_ASSERT(m_ActiveXinYunXingID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);				
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveXinYunXingID, nLevel);
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId(); //子技能
                    //执行子技能
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
					//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
					if (pOrdinSkill2)
                    {
						pOrdinSkill2->m_nEnChance = nEnChance;
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY); //使用光环
                    }
				}
			}
		}
//-----------------------------技能多光环技能数据同步----------------------------------------------------
		int nMpsX, nMpsY,k;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		//-----------------------------------------------------------------------------------------------
		if (m_IsMoreAura && IsPlayer())
		{//如果是多光环
			if (m_ActiveAuraID >0 && m_ActiveAuraID < MAX_SKILL)  //主要用于技能的 主动技能
			{
				//int nListIdx  = m_SkillList.FindSame(m_ActiveAuraID); 
				int nMainLevel = m_SkillList.GetCurrentLevelByIdx(m_ActiveAuraIndex);//母主动光环技能ID

				for (k = 0;k < 5;++k)
				{
					if (m_TmpAuraID[k].skillid>0 && m_TmpAuraID[k].skillid < MAX_SKILL) //包含光环的技能ID
					{//峨眉技能多光环技能数据
						//int nListIdx     = m_TmpAuraID[k].skilllistIndex;//m_SkillList.FindSame(m_TmpAuraID[k]);    //m_ActiveAuraID
						int nLevel       = m_SkillList.GetCurrentLevelByIdx(m_TmpAuraID[k].skilllistIndex);
						int nEnChance    = m_SkillList.GetEnChance(m_TmpAuraID[k].skilllistIndex);
						nLevel =  (((nLevel)<(nMainLevel))?(nLevel):(nMainLevel));//Min(nLevel,nMainLevel); //取最小值
						if (nLevel > 0 &&  nLevel < MAX_SKILLLEVEL)
						{//s2c_castskilldirectly									
							KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_TmpAuraID[k].skillid, nLevel);
							int nChildSkillId = 0;
							if (pOrdinSkill1)
							{
								nChildSkillId = pOrdinSkill1->GetChildSkillId();

								KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nLevel);
								//int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(nChildSkillId));
								if (pOrdinSkill2)
								{
									pOrdinSkill2->m_nEnChance = nEnChance;
									pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY);
								}
							}
						}
					}//end if
				} //end for
			}
		}*/
///----------------------------------特殊技能同步完毕-------------------------------------------------------
	}
	if (!(m_LoopFrames%GAME_UPDATE_TIME))
	//if (m_LoopFrames-(m_LoopFrames>>3<<3)==0)
	{
	    m_CurrentNuQi += m_CurrentNuQiReplenish;
	  if (m_CurrentNuQi > m_CurrentNuQiMax)
		 m_CurrentNuQi = m_CurrentNuQiMax;		        // 怒气半秒自然回复
	}

	if (!(m_LoopFrames%540))
	//if  (m_LoopFrames-18*30*(m_LoopFrames/(18*30))==0)
	{
	    m_IsHaveAttack=0;  //是否设置为攻击无效了
	    m_AttackerDwid=0;  //上次攻击着的DWID
	}

/*
	int					m_CurrentFuMoVal;		// Npc的当前附魔值
	int					m_CurrentFuMoValReplenish;	// Npc的当前附魔值回复速度
    int	                m_FuMoTimesVal;		    // Npc的当前附魔持续时间值（间隔值）
	int					m_CurrentFuMoValMax;    // Npc的当前附魔最大值
*/

	bool bAdjustColorId = false;

	if (m_FreezeState.nTime > 0)
	{//冰冻
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_freeze); //
		m_AdjustColorKind = kind_color_freeze;
		bAdjustColorId = true;

		if (SubWorld[0].m_dwCurrentTime & 1)
			nRet = TRUE;
	}
	
	if (m_StunState.nTime > 0)
	{//眩晕
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_stun);
		m_AdjustColorKind = kind_color_stun;
		bAdjustColorId = true;
		nRet = TRUE;
	}

	//if (m_PoisonState.nTime > 0)
	if (m_PoisonState.nTime>0)
	{//毒
	//	m_PoisonState.nTime--;
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_poison);
		m_AdjustColorKind = kind_color_poison;
		bAdjustColorId = true;
	}
	// 怒满状态(战斗模式才上色)
/*	if (m_CurrentNuQi >= m_CurrentNuQiMax && m_FightMode==1)
	{//怒去
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_red);
		bAdjustColorId = true;
	}
	*/
	//被抓捕状态上色
    /*if (m_ZhuaState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_confuse);
		//nRet = TRUE;
		bAdjustColorId = true;
	}*/ 
	// 燃烧状态
	if (m_BurnState.nTime > 0)
	{
	//	m_BurnState.nTime--;
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_burn);
		m_AdjustColorKind = kind_color_burn;
		bAdjustColorId = true;
	}
	// 混乱状态
	if (m_ConfuseState.nTime > 0)
	{
	//	m_ConfuseState.nTime--;
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_confuse);
		m_AdjustColorKind = kind_color_confuse;
		bAdjustColorId = true;
	}

	if (!bAdjustColorId)
		m_AdjustColorKind = kind_color_physics;
		//m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_physics);
//状态的移除 和 同步=========================================
	KStateNode* pNode=NULL;
	            pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
        pNode = (KStateNode *)pNode->GetNext();
		if (pTempNode->m_LeftTime <= -1)	
		{//被动技能 无时间限制的不会被清除！
			continue;
		}
	
		if (pTempNode->m_LeftTime == 0 && pTempNode->m_IsClientState==0)    //第几个状态时间 服务器端
		{//移除技能状态 有时间限制的 自己的状态
			int i;
			for (i = 0; i < MAX_SKILL_STATE; ++i)
			{//重新计算技能状态属性各项值
				if (pTempNode->m_State[i].nAttribType) //所有状态中的第几个属性
				{//移除属性值
					KMagicAttrib nMagicAttrib;   
					nMagicAttrib.nAttribType = pTempNode->m_State[i].nAttribType;
					nMagicAttrib.nValue[0]   = pTempNode->m_State[i].nValue[0];
					nMagicAttrib.nValue[1]   = pTempNode->m_State[i].nValue[1];
					nMagicAttrib.nValue[2]   = pTempNode->m_State[i].nValue[2];
                    ModifyAttrib(m_Index, &nMagicAttrib);
					/*
					char msg[64];		
					#ifdef _SERVER	 
					   t_sprintf(msg,"S(%d)移除技能:%d,值：Min:%d,max:%d",i,pTempNode->m_State[i].nAttribType,pTempNode->m_State[i].nValue[0],pTempNode->m_State[i].nValue[2]);
					   Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
                    #else
					   //ModifyAttrib(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Index, &nMagicAttrib);
					   t_sprintf(msg,"<color=yellow>C(%d)移除技能:%d,值：Min:%d,max:%d",i,pTempNode->m_State[i].nAttribType,pTempNode->m_State[i].nValue[0],pTempNode->m_State[i].nValue[2]);
                       Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
                    #endif 
					*/
				}
			}
			//ReCalcStateEffect();
			//_ASSERT(pTempNode != NULL);
			if (pTempNode != NULL)
			{//状态的移除 这里开始移除 精灵
			   int nIdx = pTempNode->m_StateGraphics;
			   pTempNode->Remove();
			   delete pTempNode;
			   pTempNode = NULL;
			   if (g_GameWorld)//删除状态精灵
				   g_GameWorld->removespriteByIdx(m_Index,nIdx);
			}
			continue;
		}
		else
		{//时间减少
			pTempNode->m_LeftTime --;
			//画图标
			//======
		}
	}
///===========================================================
	return nRet;

}

int KNpc::GetSkillLeftTime(int nSkillId)
{
    KStateNode* pNode;
	    pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMunTime=-1;

	while(pNode)
	{
		if (pNode->m_SkillID == nSkillId)
		{//ID相同 如果有这个技能
			nMunTime= pNode->m_LeftTime;    
			break;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}

    return nMunTime;
}


int KNpc::SetSkillLeftTime(int nSkillId,int nTime)
{
    KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nReut=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillId)
		{//ID相同
			nReut =pNode->m_LeftTime;
			pNode->m_LeftTime=nTime;
			//----------------------
			break;
		}
		pNode = (KStateNode *)pNode->GetNext();
	}
	return nReut;

}


void KNpc::ClearOneSkillState(int nSkillId)
{
  return;
}

//玩家死亡 Npc死亡？
void KNpc::DoDeath(int nMode,int nLastDamageIdx)
{
//	_ASSERT(m_Doing != do_death);
	// do_death == 10
	//g_DebugLog("[DEATH] m_Doing: %d", m_Doing);
	//CCAssert(m_RegionIndex >= 0,"");
	//_ASSERT
	if (m_RegionIndex < 0 || m_Doing == do_death)
		return;
//服务器端
	if (IsPlayer() && !m_FightMode)	// 城镇内不会死亡（有效）
	{
		m_CurrentLife = 1;  
		return;
	}

	if (IsPlayer())//（有效）
	{
		Player[m_nPlayerIdx].m_ItemList.SetMaskLock(FALSE);// 死亡者
		int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_mask); //
		m_MaskType = Item[nIdx].GetBaseMagic();
	}
	m_IsbeSel  = 0;
	//m_DataRes.SetBlur(FALSE);	 //清除残影

	if (this->m_Kind == kind_normal || this->m_Kind == kind_partner)  //所有都冒血处理
		this->SetBlood(this->m_CurrentLife);  //冒血数字显示为当前生命this->m_CurrentLife

//要等分配经验完毕
	m_bLockNpcDwID       = 0;
	m_Doing              = do_death;      //指向调用 ondeath 函数
	m_ProcessAI	         = 0;
	m_ProcessState       = 0;
	m_Frames.nTotalFrame = m_DeathFrame;  //定格在死亡帧数	
	m_Height = 0;                         //高度为零

	m_ClientDoing = cdo_death;

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}

	this->m_cGold.ClientClearState();  //删除黄金怪状态

	if (IsPlayer())
	{// 自动离队
		if (Player[m_nPlayerIdx].m_cTeam.m_nFlag)
		{
			Player[m_nPlayerIdx].LeaveTeam();
		}
	}

	//CoreDataChanged(GDCNI_OPEN_JINDUTIAO,0,0);  //死亡关闭进度条
}

//玩家 NPC死亡脚本  无限循环死亡状态
//int                 IsExeGoldScript;          //是否执行全局死亡脚本
//int                 IsCreatBoss;
void KNpc::OnDeath()
{
	if (WaitForFrame())
	{
//		printf("[DEATH] 死亡(%s) 触发BOSS(%d),执行全局脚本(%d) TRUE\n",Name,IsCreatBoss,IsExeGoldScript);
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		// 保证不会有重回第一帧的情况m_Frames.nTotalFrame - 1
		int		nTempX, nTempY,nObjIndex;
		KCObjItemInfo	sInfo;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nTempX, &nTempY);
		sInfo.m_nItemID = 0;
		sInfo.m_nItemWidth = 0;
		sInfo.m_nItemHeight = 0;
		sInfo.m_nMoneyNum = 0;
		sInfo.m_nColorID = 0;
		sInfo.m_nMovieFlag = 0;
		sInfo.m_nSoundFlag = 0;
		sInfo.m_szName[0] = 0;
		sInfo.m_cHaveAttack=0;
		sInfo.m_Genre=-1;
		sInfo.m_DetailType=-1;
		sInfo.m_ParticularType=-1;
		sInfo.m_GoldId=0;
		sInfo.m_ItemLevel=0;
		sInfo.m_StackNum=0;

		//sInfo.m_cAttackerDwid=0;
		nObjIndex=ObjSet.ClientAdd(0, m_CorpseSettingIdx, 0, m_Dir, 0, nTempX, nTempY, sInfo);  //增加尸体
        m_ProcessAI	= 0;
		// 重生点
		if (m_Kind != kind_partner)//战斗Npc时
		{//怪物与人
			if (this->m_cGold.GetGoldType() <= 4) //不是黄金怪物就复活 1-4 5-11 12-16
			{
				DoRevive();  //开始重生	  已经删除了节点 加入重生列表
			}
			// 客户端把NPC删除
			if (m_Kind!= kind_player)
			{
			   if (nObjIndex >0 && nObjIndex<MAX_OBJECT)
			   {//删除尸体
				  //SubWorld[0].m_WorldMessage.NewSend(GWM_OBJ_DEL,nObjIndex);
				   if (Object[nObjIndex].m_nSubWorldID>=0 && Object[nObjIndex].m_nRegionIdx >= 0)
					   SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
				   
		           ObjSet.Remove(nObjIndex);
			   }

			   if (m_Index>0  && m_Index<MAX_NPC)
			   {
				   if (Npc[m_Index].m_SubWorldIndex>=0 && Npc[m_Index].m_RegionIndex >= 0)
				   {				
					   int nSubWorld = Npc[m_Index].m_SubWorldIndex;
					   int nRegion = Npc[m_Index].m_RegionIndex;

					   SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(m_Index);
					   SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[m_Index].m_MapX, Npc[m_Index].m_MapY);
				   }
		            NpcSet.Remove(m_Index);
			   } 
			  return;
			}			
		}
		else
		{//同伴类直接不能重生
			//Remove();
		}
	}
	else
	{
//		printf("[DEATH] 死亡帧数错误WaitForFrame FALSE\n");
	}
}

void KNpc::DoDefense() //档格
{
	m_ProcessAI = 0;
}

void KNpc::OnDefense()  //档格
{

}

void KNpc::DoIdle()
{
	if (m_Doing == do_idle)
		return;
	m_Doing = do_idle;
}

void KNpc::OnIdle()
{

}

VOID KNpc::DoDrag(INT nDragFrame, INT nDesX, INT nDesY)
{
	if (m_RegionIndex < 0 || m_Doing == do_death)
		return;
	if (m_Doing == do_drag)
		return;
	
	INT nMyX, nMyY,nMap;
	GetMpsPos(&nMyX, &nMyY,&nMap);
	if (nDesX != nMyX || nDesY != nMyY)
		m_Dir	= g_GetDirIndex(nDesX, nDesY, nMyX, nMyY);
	
	m_Doing = do_drag;
	
	//ClearProcessAI();
	m_ProcessAI = 0;
	//m_ProcessState = 0;
	
	m_ClientDoing = cdo_hurt;
	
	m_Frames.SetFrame(nDragFrame);
	
	m_DesX	= nDesX;
	m_DesY	= nDesY;
}

//拉扯过来
VOID KNpc::OnDrag()
{
	if (m_RegionIndex < 0)
		return;
	
	INT nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);
	
	INT nRestSteps	= m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
	if (nRestSteps <= 0)
		nRestSteps	= 1;
	
	INT nMoveX	= ((m_DesX - nX) << 10) / nRestSteps;
	INT nMoveY  = ((m_DesY - nY) << 10) / nRestSteps;
	
	MovePos(nMoveX, nMoveY);
	
	if (WaitForFrame())
	{
		DoStand();
		//SetProcessAI();
		m_ProcessAI = 1;
	}
}

// -------------------------------------------------------------------------
// 函数		: KNpc::MovePos
// 功能		: 移动一定偏移量
// 返回值	: VOID 
// 参数		: INT nMoveX, nMoveY	Npc在格子中的偏移坐标（放大了1024倍）
// 作者		: FanZai
// 附注		: 来自ServeMove与ServeJump，合并重复代码
// -------------------------------------------------------------------------
#define CORRECT_SYNC_RANGE 30 

VOID KNpc::MovePos(INT nMoveX, INT nMoveY)
{
	INT nOldRegion = m_RegionIndex;
	INT nOldMapX = m_MapX;
	INT nOldMapY = m_MapY;
	INT nOldOffX = m_OffX;
	INT nOldOffY = m_OffY;

	m_OffX += nMoveX;
	m_OffY += nMoveY;

	if (!m_bClientOnly)
		CURREGION.DecNpcRef(m_MapX, m_MapY);

	INT nCellWidth	= (SubWorld[m_SubWorldIndex].m_nCellWidth << 10);
	INT nCellHeight	= (SubWorld[m_SubWorldIndex].m_nCellHeight << 10);

	if (nCellWidth <= 0 || nCellHeight <= 0)
	{
		//_ASSERT(FALSE);
		return;
	}

	while (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += nCellWidth;
	}
	while (m_OffX >= nCellWidth)
	{
		m_MapX++;
		m_OffX -= nCellWidth;
	}

	while (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += nCellHeight;
	}
	while (m_OffY >= nCellHeight)
	{
		m_MapY ++;
		m_OffY -= nCellHeight;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddNpcRef(m_MapX,m_MapY);
			//CURREGION.AddRef(m_MapX, m_MapY,obj_npc);
	}

	if (m_RegionIndex < 0)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
		m_RegionIndex = -1;
		return;
	}
	if  (m_OffX>=nCellWidth || m_OffY>=nCellHeight)
		return;
	//CCAssert(m_OffX < nCellWidth && m_OffY < nCellHeight,"");
	//快到同步边缘时同步一下坐标
	if (!IsPlayer() && (NpcSet.GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= CORRECT_SYNC_RANGE ||
		NpcSet.GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) >= CORRECT_SYNC_RANGE ))
	{
//		ForceSyncPos(TRUE);
	}

	if (nOldRegion != m_RegionIndex)
	{
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
}


//做受伤动作
void KNpc::DoHurt(int nHurtFrames, int nX, int nY,int nRank)
{
	//_ASSERT(m_RegionIndex >= 0);
	//m_DataRes.SetBlur(FALSE);
	if (m_RegionIndex < 0)
		return;
	if (m_Doing == do_hurt || m_Doing == do_death)
		return;

	// Npc的受伤动作时间已经达到100%了，不做受伤动作
	m_Doing = do_hurt;
	//m_ProcessAI	= 0;  //取消做动作不能走动
	m_ClientDoing = cdo_hurt;
	m_Frames.nTotalFrame = nHurtFrames; //受伤帧数
	m_nHurtDesX = nX;
	m_nHurtDesY = nY;
	if (m_Height > 0)
	{
		// 临时记录下来做为高度变化，在OnHurt中使用
		m_nHurtHeight = m_Height;
	}
	else
	{
		m_nHurtHeight = 0;
	}

	CoreDataChanged(GDCNI_OPEN_JINDUTIAO,0,0); //取消进度条
	if (m_Frames.nTotalFrame == 0)
		m_Frames.nTotalFrame = 1;

	    m_Frames.nCurrentFrame = 0;
}

void KNpc::OnHurt()
{
	if (m_RegionIndex < 0 || m_Frames.nTotalFrame<=0)
	{
		return;
	}
	int nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);

	//CCAssert(m_Frames.nTotalFrame>0,"");
	if (m_Frames.nTotalFrame<=0)
		return;

	m_Height = m_nHurtHeight * (m_Frames.nTotalFrame - m_Frames.nCurrentFrame - 1) / m_Frames.nTotalFrame;
	nX = nX + (m_nHurtDesX - nX) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;
	nY = nY + (m_nHurtDesY - nY) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;

	int nOldRegion = m_RegionIndex;
	//SetPos(nX, nY);
	CURREGION.DecNpcRef(m_MapX, m_MapY);

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	nRegion = -1;
	nMapX = nMapY = nOffX = nOffY = 0;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
		m_dwRegionID = 0;
	}
	else if (nOldRegion != nRegion || nRegion < 0)
	{
		m_RegionIndex = nRegion;
		m_MapX = nMapX;
		m_MapY = nMapY;
		m_OffX = nOffX;
		m_OffY = nOffY;
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
	if (nRegion >= 0)
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	//SubWorld[m_SubWorldIndex].m_Region[nRegion].AddNpcRef(m_MapX,m_MapY);
	if (WaitForFrame())   //等待的帧数
	{
	//	g_DebugLog("[DEATH]On Hurt Finished");                 //受到伤害的动作
	   if (m_Kind==kind_player)
	   {
           Player[CLIENT_PLAYER_INDEX].UpdataCurData();
	   }
		DoStand();
		m_ProcessAI = 1; //完成受伤动作
	}
}

void KNpc::DoSpecial1()
{
	 DoBlurAttack();
}

void KNpc::OnSpecial1()
{
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (pSkill)
		{
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
			pSkill->m_nEnChance = nEnChance;
			int nChildSkill = pSkill->GetChildSkillId();

			int nChildSkillLevel = pSkill->m_ulLevel;
			
			if (nChildSkill > 0)
			{
				KSkill * pChildSkill = (KSkill*)g_SkillManager.GetSkill(nChildSkill, nChildSkillLevel);
				if (pChildSkill)
				{
					pChildSkill->m_nEnChance = nEnChance;
					pChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
				}
			}
		}

        if (m_Kind==kind_player)
            NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);

		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::DoSpecial2()
{
}

void KNpc::OnSpecial2()
{

	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		ISkill * pSkill = GetActiveSkill();
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		switch(eStyle)
		{
		case SKILL_SS_Thief:
			{
				((KThiefSkill*)pSkill)->OnSkill(this);  //偷取技能
			}
			break;
		} 
		
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
	
}

void KNpc::DoSpecial3()
{
}

void KNpc::OnSpecial3()
{
}
//转换坐标 转换地图
void KNpc::DoSpecial4(int nX, int nY)
{
	WaitForFrame();
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		DoStand();
		return;
	}
	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	
	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
	
	if (!m_bClientOnly)
		CURREGION.DecNpcRef(m_MapX, m_MapY);
	
	SubWorld[m_SubWorldIndex].Mps2Map(nX,nY,&m_RegionIndex,&m_MapX,&m_MapY,&m_OffX,&m_OffY);
	
	if (!m_bClientOnly && m_RegionIndex >= 0)
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	
	
	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		return;
	}
	
	if (nOldRegion != m_RegionIndex)
	{
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
	DoStand();
}



void KNpc::OnSpecial4()
{

}

void KNpc::DoStand()
{
	
	m_Frames.nTotalFrame = m_StandFrame;
	if (m_Doing == do_stand)
	{
		m_bLockNpcDwID=0;
		return;
	}
	else
	{
		m_Doing = do_stand;
		m_bLockNpcDwID=0;
		m_Frames.nCurrentFrame = 0;
		int  m_Dmap=0;
		GetMpsPos(&m_DesX, &m_DesY,&m_Dmap);
		if (m_FightMode)
			m_ClientDoing = cdo_fightstand;
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
		//m_DataRes.StopSound();
	}
}


void KNpc::OnStand()
{
	if (WaitForFrame())
	{
		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightstand;
		}
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
	}
}
//玩家 怪物 死亡后重生设置	return m_Kind == kind_player;//服务器  玩家return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;
//客户端界面显示
	
void KNpc::DoRevive() //NPC重生
{
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_revive)
	{
		m_bLockNpcDwID=0;
		return;
	}
	else
	{
		m_bLockNpcDwID=0;
		m_Doing = do_revive;
		m_ProcessAI = 0;
		m_ProcessState = 0;
		ClearStateSkillEffect();  //删除状态效果
		ClearNormalState();       //清除不良状态
		if (IsPlayer())  //如果是玩家
		{
			KSystemMessage MsgA;
			MsgA.byConfirmType = SMCT_UI_RENASCENCE; //发送消息 选择重生 方式	
			MsgA.byParamSize = 0;
			MsgA.byPriority =255;
			MsgA.eType = SMT_PLAYER;
			//t_sprintf(MsgA.szMessage, MSG_NPC_DEATH, Name); //气绝身亡
			t_sprintf(MsgA.szMessage, strCoreInfo[MSG_NPC_DEATH].c_str(), Name); //气绝身亡
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(0);
			MsgA.nMsgLen = TEncodeText_(MsgA.szMessage, strlen(MsgA.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&MsgA, NULL);
		}	
		m_Frames.nTotalFrame = m_DeathFrame;  //定格在死亡帧数
		m_ClientDoing = cdo_death;            //客户端行为	
	}
}

void KNpc::OnRevive()  //重生
{
	m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
}

void KNpc::DoRun()
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex<0)
		return ;
	if (m_CurrentRunSpeed) //当前的跑速	  m_RunSpeed 原始跑速 m_CurrentRunSpeed +=  m_RunSpeed*百分比
	{
		if (m_CurrentRunSpeed<10)
			m_CurrentRunSpeed =10;

		if (m_CurrentRunSpeed>m_RunFrame * m_RunSpeed)
		   m_Frames.nTotalFrame=m_RunFrame;
		else
		   m_Frames.nTotalFrame = (m_RunFrame * m_RunSpeed)/m_CurrentRunSpeed; //15*跑速百分比
	}
	else
		m_Frames.nTotalFrame = m_RunFrame;

	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightrun;
	}
	else
	{
		m_ClientDoing = cdo_run;
	}
/*
	if (IsPlayer())
	{
		if (!Cost(attrib_stamina, m_CurrentStaminaLoss))
		{
			DoWalk();
			return;
		}
	}
*/
	if (m_Doing == do_run)
	{
		m_bLockNpcDwID=0;
		return;
	}
	m_bLockNpcDwID=0;
	m_Doing = do_run;

	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnRun()
{		
	WaitForFrame();
	//if (!(m_LoopFrames % GAME_UPDATE_TIME) && Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == 2)
	if (m_LoopFrames - (m_LoopFrames>>3<<3)==0 && Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == 2)
	{
		m_CurrentStamina -= 18;  //减少的体力值

		if (m_CurrentStamina <= 0)
		{
			m_CurrentStamina = 0;
		}
	}

	if (m_CurrentStamina == 0)  //没有体力值
		ServeMove(m_CurrentWalkSpeed);

	else if(m_Doing == do_runattack)
	{//跑着攻击	
		m_CurrentRunSpeed += 50;
		ServeMove(m_CurrentRunSpeed);
		m_CurrentRunSpeed -= 50;
	}
	else
	{//普通的跑步
		ServeMove(m_CurrentRunSpeed);
	}

}

void KNpc::OnWalk()
{
	//#ifndef	_SERVER
	// 处理客户端的动画换帧等……
	//#endif
	WaitForFrame();
	ServeMove(m_CurrentWalkSpeed);
	
}

void KNpc::DoSit()
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex<0)
		return ;

	if (m_Doing == do_sit)
	{
//		DoStand();
		m_bLockNpcDwID=0;

		return;
	}
	m_bLockNpcDwID=0;
	m_Doing = do_sit;
    m_ClientDoing = cdo_sit;
	m_Frames.nTotalFrame = m_SitFrame;
	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnSit()
{
	// 体力换内力（没有设定）
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
	}
}
//使用技能(使用技能入口)
void KNpc::DoSkill(int nX, int nY)
{
	//_ASSERT(m_RegionIndex >= 0);

	if (m_RegionIndex<0)
		return ;

	if (m_Doing == do_skill || m_Doing == do_hurt)
		return;

	m_Hide.nTime = 0;      //使用技能后 隐身取消
	// 非战斗状态不能发技能
	if (IsPlayer())
	{//如果是客户端本人
		if (!m_FightMode)
		{
			m_bLockNpcDwID=0;
			return;
		}
		//CoreDataChanged(GDCNI_OPEN_JINDUTIAO,0,0);       //死亡关闭进度条
	}
	if (m_randmove.nTime>0)                          //混乱状态不能发招
		return;

	ISkill * pSkill = GetActiveSkill(); // 获取激活技能数据
	
	if(pSkill)
	{
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		if (m_SkillList.GetNextCastTimeByIndex(m_ActiveSkListIndex)>0 && (m_SkillList.GetNextCastTimeByIndex(m_ActiveSkListIndex)>SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{//还没解冻
			m_bLockNpcDwID=0;
			return;
		}
		
		if (m_HorseType)
		{//骑马时间限制
			if (m_SkillList.GetHorseNextCastTimeByIndex(m_ActiveSkListIndex)>0 && (m_SkillList.GetHorseNextCastTimeByIndex(m_ActiveSkListIndex)>SubWorld[m_SubWorldIndex].m_dwCurrentTime))
			{ 
				m_bLockNpcDwID=0;
				return;
			}
		}
		if (m_SkillList.CanCastByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime)   //是否可以发技能
			&& pSkill->CanCastSkill(m_Index, nX, nY) && (/*m_Kind != kind_player || */Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this))))
		{	
		/*------------------------------------------------------------------------------------
		发技能时，当需指定目标对象时，传至Skill.Cast的两个参数第一个参数为-1,第二个为Npc index
		在S2C时，第二个参数必须由Server的NpcIndex转为NpcdwID参出去。
		在C收到该指令时，将NpcdwID转为本机的NpcIndex
			-------------------------------------------------------------------------------------*/
		    if (m_HorseType)
				m_SkillList.SetHorseNextCastTimeByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime + pSkill->GetHorsePerCast());  //设置骑马使用限制时间
			else
			{
			    m_SkillList.SetNextCastTimeByIndex(m_ActiveSkListIndex, SubWorld[m_SubWorldIndex].m_dwCurrentTime + pSkill->GetDelayPerCast());       //发该技能的最小间阁时间
			}
			if (eStyle == SKILL_SS_Missles || eStyle == SKILL_SS_Melee|| eStyle == SKILL_SS_InitiativeNpcState || eStyle == SKILL_SS_PassivityNpcState)
			{
				DoOrdinSkill((KSkill *) pSkill, nX, nY);
			}
			else
			{
				switch(eStyle)
				{
				case SKILL_SS_Thief:
					{
						((KThiefSkill*)pSkill)->DoSkill(this, nX, nY);

					}
					break;
				default:
					return;
				}
			}
		}	
		else
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_bLockNpcDwID=0;
			DoStand();
		}
	}
} 

int KNpc::DoOrdinSkill(KSkill * pSkill, int nX, int nY)
{
	//_ASSERT(pSkill);
	if (!pSkill)
		return 0;

	int nEnChance = m_SkillList.GetEnChance(m_SkillList.FindSame(pSkill->GetSkillId()));
	//m_DataRes.StopSound();
	int x, y, tx, ty,tmap;
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
	
	if (nY < 0)
		return 0;
	
	if (nX < 0)
	{
		if (nX != -1) 
			return 0;
		
		if (nY >= MAX_NPC || Npc[nY].m_dwID == 0 || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
			return 0;
		Npc[nY].GetMpsPos(&tx, &ty,&tmap);
	}
	else
	{
		tx = nX;
		ty = nY;
	}
	
	m_SkillParam1 = nX;
	m_SkillParam2 = nY;
	m_DesX = nX;
	m_DesY = nY;
	
	m_Dir = g_GetDirIndex(x, y, tx, ty);  //获取人物与 怪物的方向

	//if (pSkill->GetPreCastEffectFile()[0])
		//m_DataRes.SetSpecialSpr((char *)pSkill->GetPreCastEffectFile()); //设置SPR动作状态
	
	if (IsPlayer())
		pSkill->PlayPreCastSound(m_nSex,x, y);	//使用技能的声音
	
	//if (pSkill->IsNeedShadow())		
		//m_DataRes.SetBlur(TRUE);
	//else
		//m_DataRes.SetBlur(FALSE);
	
	CLIENTACTION ClientDoing = pSkill->GetActionType();  //出招动作类型
	if (ClientDoing >= cdo_count) 
		m_ClientDoing = cdo_magic;
	else if (ClientDoing != cdo_none)
		m_ClientDoing = ClientDoing;
	pSkill->m_nEnChance = nEnChance;

	int naAttackSpeed=100,nbAttackSpeed=100;
	    g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);

	if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
	{//
		if (CastMeleeSkill(pSkill) == FALSE)
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_ProcessAI = 1;
			DoStand();
			return 1 ;
		}
	}
	//物理技能的技能释放时间与普通技能不同，一个是AttackFrame,一个是CastFrame
	else if (pSkill->IsPhysical() || !pSkill->IsMagic())
	{//外功攻击
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
				m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed); //  外功攻速
			else
				m_Frames.nTotalFrame = m_AttackFrame;

		}
		if (g_Random(3))
			m_ClientDoing = cdo_attack;
		else 
			m_ClientDoing = cdo_attack1;
		m_Doing = do_attack; //外功攻击动作
	}
	else
	{//内功
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			if (m_CurrentCastSpeed + nbAttackSpeed !=0)
				m_Frames.nTotalFrame = m_CastFrame * naAttackSpeed / (m_CurrentCastSpeed + nbAttackSpeed);  // 内功攻速
			else
				m_Frames.nTotalFrame = m_CastFrame ;
		}

		m_Doing  = do_magic;//内功攻击动作
	}
	
	m_ProcessAI = 0;
	m_Frames.nCurrentFrame = 0;	
	return 1;
}


//外功攻击动作
void KNpc::DoAttack()
{
	if (m_Doing == do_attack)
		return;

	if (g_Random(2) == 1)
	{
		m_ClientDoing = cdo_attack;
	}
	else
	{
		m_ClientDoing = cdo_attack1;
	}
    int naAttackSpeed=100,nbAttackSpeed=100;
	    g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);
	
	m_ProcessAI = 0;

	if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
		m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed/(nbAttackSpeed + m_CurrentAttackSpeed);
	else
		m_Frames.nTotalFrame = m_AttackFrame;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_attack;
}


BOOL	KNpc::CastMeleeSkill(KSkill * pSkill)
{
	BOOL bSuceess = FALSE;
	//_ASSERT(pSkill);
	if (!pSkill)
		return bSuceess;

	switch(pSkill->GetMeleeType())
	{
	case Melee_AttackWithBlur:
		{
			bSuceess = DoBlurAttack();
		}break;
	case Melee_Jump:
		{
			if (NewJump(m_DesX, m_DesY))
			{
				DoJump();
				bSuceess = TRUE;
			}
			
		}break;
	case Melee_JumpAndAttack:
		{
			if (m_DesX < 0 && m_DesY > 0) 
			{
				int x, y;
				SubWorld[m_SubWorldIndex].NewMap2Mps
					(
					Npc[m_DesY].m_RegionIndex,
					Npc[m_DesY].m_MapX, 
					Npc[m_DesY].m_MapY, 
					Npc[m_DesY].m_OffX, 
					Npc[m_DesY].m_OffY, 
					&x,
					&y
					);
				
				m_DesX = x + 1;
				m_DesY = y;
			}

			if (NewJump(m_DesX, m_DesY))
			{
				DoJumpAttack();
				bSuceess = TRUE;
			}

		}break;
	case Melee_RunAndAttack:
		{
			bSuceess = DoRunAttack();

		}break;
	case Melee_ManyAttack:
		{
			bSuceess = DoManyAttack();
		}break;
	case Melee_Move:  //轻功 跳跃
		{
			if (NewJump(m_DesX, m_DesY))
				bSuceess = DoSecMove(pSkill->GetParam1());
		}break;
	default:
		m_ProcessAI = 1;
		break;
	}
	return bSuceess;

}

BOOL KNpc::DoBlurAttack()// DoSpecail1
{
	if (m_Doing == do_special1)
		return FALSE;
	
	KSkill * pSkill = (KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	//_ASSERT(pSkill->GetSkillStyle() == SKILL_SS_Melee);
	if  (pSkill->GetSkillStyle()!=SKILL_SS_Melee)
		return false;

		m_ClientDoing = pSkill->GetActionType();
		//m_DataRes.SetBlur(TRUE);
	int naAttackSpeed=100,nbAttackSpeed=100;
	    g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);

		if  (nbAttackSpeed + m_CurrentAttackSpeed !=0)
			m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed);
		else
			m_Frames.nTotalFrame = m_AttackFrame ;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_special1;
	return TRUE;
}
//内攻动作功速
void KNpc::DoMagic()
{
	if (m_Doing == do_magic)
		return;
	m_ProcessAI = 0;
	m_ClientDoing = cdo_magic;
	int naAttackSpeed=100,nbAttackSpeed=100;
	    g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);

		if (m_CurrentCastSpeed + nbAttackSpeed!=0) 
			m_Frames.nTotalFrame = m_CastFrame * naAttackSpeed/(m_CurrentCastSpeed + nbAttackSpeed);
		else
			m_Frames.nTotalFrame = m_CastFrame;

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_magic;
}
//使用技能
void KNpc::OnSkill()
{
	KSkill * pSkill = NULL;
	KSkill * pFmSkill = NULL;
	int nSkill=0,nSkllLevel=0;
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		//m_DataRes.SetBlur(FALSE);
		if (m_DesX == -1) 
		{
			if (m_DesY <= 0) 
				goto Label_ProcessAI;
			
			//此时该角色已经无效时
			if (Npc[m_DesY].m_RegionIndex < 0) 
				goto Label_ProcessAI;
		}
		
	    //设置冷却时间	
		pSkill =(KSkill*) GetActiveSkill();  //获取激活的技能

		if (pSkill)
		{
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex); //计算技能加成  m_SkillList.FindSame(pSkill->GetSkillId())
			pSkill->m_nEnChance = nEnChance;       //技能加成
			pSkill->Cast(m_Index, m_DesX, m_DesY); //对某人 坐标 使用技能
			//设置下次使用时间间隔
		}
		  //神将技能
		  NpcFuMoCastSkll(m_Index, m_DesX, m_DesY);

Label_ProcessAI:
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}	
}
//神将附加技能
void KNpc::NpcFuMoCastSkll(int nLauncher, int nParam1, int nParam2, int nWaitTime)
{
/*	KSkill * pFmSkill = NULL;
	int nSkill=0,nCaseTime=0,nSKillBiaoShi;

	if (m_nFuMoNum[0].nNpcSetings>1)
	{//金系技能	
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_0",&nSkill,&nCaseTime);
		if (m_nFuMoNum[0].nSkillLevel<=0)
			m_nFuMoNum[0].nSkillLevel=1;
		 m_nFuMoNum[0].nCastTime = nCaseTime;
		 nSKillBiaoShi = 0;

#ifdef _SERVER
		 m_nFuMoNum[1].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif

		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[0].nSkillLevel);
	}
	else if (m_nFuMoNum[1].nNpcSetings>1)
	{//木系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_1",&nSkill,&nCaseTime);
		if (m_nFuMoNum[1].nSkillLevel<=0)
			m_nFuMoNum[1].nSkillLevel=1;

		 m_nFuMoNum[1].nCastTime = nCaseTime;
		 nSKillBiaoShi = 1;

#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[1].nSkillLevel);
	}
	else if (m_nFuMoNum[2].nNpcSetings>1)
	{//水系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_2",&nSkill,&nCaseTime);
		if (m_nFuMoNum[2].nSkillLevel<=0)
			m_nFuMoNum[2].nSkillLevel=1;
		 m_nFuMoNum[2].nCastTime = nCaseTime;
		 nSKillBiaoShi = 2;

#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[1].nNextCastTime = 0;
		 m_nFuMoNum[3].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[2].nSkillLevel);
	}
	else if (m_nFuMoNum[3].nNpcSetings>1)
	{//火系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_3",&nSkill,&nCaseTime);
		if (m_nFuMoNum[3].nSkillLevel<=0)
			m_nFuMoNum[3].nSkillLevel=1;
		 m_nFuMoNum[3].nCastTime = nCaseTime;
		 nSKillBiaoShi = 3;
#ifdef _SERVER
		 m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		 m_nFuMoNum[1].nNextCastTime = 0;
		 m_nFuMoNum[2].nNextCastTime = 0;
		 m_nFuMoNum[4].nNextCastTime = 0;
#else
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		 Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[4].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[3].nSkillLevel);
	}
	else if (m_nFuMoNum[4].nNpcSetings>1)
	{//土系技能
		g_GameSetTing.GetInteger2("FuMoNpcSet","Skill_4",&nSkill,&nCaseTime);
		if (m_nFuMoNum[4].nSkillLevel<=0)
			m_nFuMoNum[4].nSkillLevel=1;
        m_nFuMoNum[4].nCastTime = nCaseTime;
		nSKillBiaoShi = 4;
#ifdef _SERVER
		m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		m_nFuMoNum[1].nNextCastTime = 0;
		m_nFuMoNum[2].nNextCastTime = 0;
		m_nFuMoNum[3].nNextCastTime = 0;
#else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[0].nNextCastTime = 0;//	发该技能的最小间阁时间
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[1].nNextCastTime = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[2].nNextCastTime = 0;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[3].nNextCastTime = 0;
#endif
		pFmSkill = (KSkill *)g_SkillManager.GetSkill(nSkill,m_nFuMoNum[4].nSkillLevel);	
	}

#ifdef _SERVER
	//if  (pFmSkill)
	
		if (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{//还没解冻
			return;
		}
		
		//if (m_HorseType)
		//{//骑马时间限制
		//   if (m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime)
	    //      return;
		//} 
	
        //char msg[64];
        //t_sprintf(msg,"S冻结时间间隔:%d,S下次释放时间:%d",m_nFuMoNum[nSKillBiaoShi].nCastTime,m_nFuMoNum[nSKillBiaoShi].nNextCastTime);
		//Player[GetnPlayerIdx()].m_ItemList.msgshow(msg);
#else
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime > 0 && (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime > SubWorld[0].m_dwCurrentTime))
		{//还没解冻
			return;
		}	
       // char msg[64];
       // t_sprintf(msg,"C冻结时间间隔:%d,C下次释放时间:%d",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nCastTime,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime);
	   //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
#endif
	   //if (Npc[nParam2].m_Index <= 0)  //不能空放
	   //return;

	if  (pFmSkill)
			pFmSkill->Cast(nLauncher, nParam1, nParam2,nWaitTime);
	//设置下次使用时间间隔
	//if (m_HorseType)
	//	m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + nCaseTime;  //设置骑马使用限制时间
	//else
	#ifdef _SERVER
	    m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + nCaseTime;//	发该技能的最小间阁时间
    #else
	    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nFuMoNum[nSKillBiaoShi].nNextCastTime = SubWorld[0].m_dwCurrentTime + nCaseTime;//	发该技能的最小间阁时间
    #endif

    pFmSkill=NULL;
	*/
   return;
}

void KNpc::JumpTo(int nMpsX, int nMpsY)
{
	if (NewJump(nMpsX, nMpsY))
		DoJump();
	else
	{
		RunTo(nMpsX, nMpsY);
	}
}

void KNpc::RunTo(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoRun();
}

void KNpc::Goto(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoWalk();
}

void KNpc::DoWalk()
{
	//_ASSERT(m_RegionIndex >= 0);
  if (m_RegionIndex < 0)
	     return;

    if (m_CurrentWalkSpeed<5)
		m_CurrentWalkSpeed = 5;

	if ((m_CurrentWalkSpeed + 1)!=0)
		m_Frames.nTotalFrame = (m_WalkFrame * m_WalkSpeed) / m_CurrentWalkSpeed + 1;
	else
		m_Frames.nTotalFrame = m_WalkFrame;
	if (m_Doing == do_walk)
	{
		return;
	}
	m_Doing = do_walk;
	m_Frames.nCurrentFrame = 0;

	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightwalk;
	}
	else
	{
		m_ClientDoing = cdo_walk;
	}
}

/*void KNpc::ClearEnhanceNote()
{
	  for (int i = 0;i<MAX_NPCSKILL)
	  {
		  //SAFE_DELETE(it->second);
		  if  (nEnhanceInfo.count(i)>0)
			   nEnhanceInfo.erase(i);
	  }

	  nEnhanceInfo.clear();

}


void KNpc::SetEnhanceNote(int i,int v)
{
   if  (nEnhanceInfo.count(i)>0)
	    nEnhanceInfo[i] = v;	

	    nEnhanceInfo[i] = v;
}


int  KNpc::GetEnhanceNote(int i)
{

    if  (nEnhanceInfo.count(i)>0)
		return 	nEnhanceInfo[i];
	else
		return 0;

} */

void KNpc::DoPlayerTalk(char * szTalk)
{
/*	
#ifdef _SERVER
	_ASSERT(m_RegionIndex >= 0);
	int nTalkLen = strlen(szTalk);
	if (!nTalkLen) return;
	BYTE * pNetCommand = new  BYTE[nTalkLen + 6 + 1];
	pNetCommand[0] = (BYTE)s2c_playertalk;
	*(DWORD *)(pNetCommand + 1) = m_dwID;
	pNetCommand[5] = nTalkLen;
	strcpy((char*)(pNetCommand + 6), szTalk);
	pNetCommand[nTalkLen + 6 ] = '\0';

	POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; ++i)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
	if (pNetCommand)
	{
		delete [] pNetCommand;
	}
#endif
*/
}

void KNpc::OnPlayerTalk()
{
	//没有完成
}

int	KNpc::GetSkillLevel(int nSkillId)
{
	int nIndex = m_SkillList.FindSame(nSkillId);
	if (nIndex)
	{
		return m_SkillList.m_Skills[nIndex].SkillLevel;
	}
	else
	{
		return 0;
	}
}
//技能属性应该到NPC身上  把技能属性应用到NPC身上  被动技能
void KNpc::ModifyAttrib(int nAttacker, void* pData)
{
	if (pData != NULL)
		g_NpcAttribModify.ModifyAttrib(this,pData,nAttacker);  // 修改属性

}

//设置立即技能状态效果
void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
{
	if (!pData || !nDataNum)
		return;

	KMagicAttrib*	pTemp = (KMagicAttrib *)pData;
	//_ASSERT(nDataNum <= MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;

	for (int i = 0; i < nDataNum; ++i)
	{
		ModifyAttrib(nLauncher, pTemp);//改变属性-技能加成
		++pTemp;
	}
}

//技能子弹属性数据  --被动子弹伤害计算
void KNpc::AppendSkillEffect(int nIsMaigc ,BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData, int nEnChance)
{
	int nMinDamage,nMaxDamage;

		nMinDamage = m_PhysicsDamage.nValue[0] + m_CurrentPhysicsMagicDamageV.nValue[0]; //基本伤害+铺点伤害
		nMaxDamage = m_PhysicsDamage.nValue[2] + m_CurrentPhysicsMagicDamageV.nValue[2];

		if (IsPlayer())
		{//是人物
			if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_meleeweapon)
			{//如果有进程武器
				nMinDamage += nMinDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / 100;
				nMaxDamage += nMaxDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / 100;
			}
			else if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_rangeweapon)
			{//如果有远程武器
				nMinDamage += nMinDamage * m_CurrentRangeEnhance / 100;
				nMaxDamage += nMaxDamage * m_CurrentRangeEnhance / 100;
			}
			else	// 空手
			{
				nMinDamage += nMinDamage * m_CurrentHandEnhance / 100;
				nMaxDamage += nMaxDamage * m_CurrentHandEnhance / 100;
			}
		}
//===========================================================================
	KMagicAttrib* pTemp = (KMagicAttrib *)pSrcData;   //脚本解析出来的技能数据
	KMagicAttrib* pDes  = (KMagicAttrib *)pDesData;   //通过计算后的数据
	//命中率点
	/*if (pTemp->nAttribType == magic_attackrating_p)
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = (m_CurrentAttackRating+ m_CurrentAttackRatingEnhancev) * (m_CurrentAttackRatingEnhancep + pTemp->nValue[0]+100) / 100;
	}
	else
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = (m_CurrentAttackRating+m_CurrentAttackRatingEnhancev)* (m_CurrentAttackRatingEnhancep +100) / 100;
	}*/
	if (pTemp->nAttribType == magic_attackrating_p)
	{
	   pDes->nAttribType = magic_attackrating_v;
	   pDes->nValue[0] =  m_AttackRating*(100+pTemp->nValue[0])/100;
	}
	else if (pTemp->nAttribType == magic_attackrating_v)
	{
	   pDes->nAttribType = magic_attackrating_v;
	   pDes->nValue[0] = m_AttackRating+pTemp->nValue[0];
	}
	pTemp++;
	pDes++;                                          //1 
	if (pTemp->nAttribType == magic_ignoredefense_p) //忽略敌人闪避率
	{
		pDes->nAttribType = magic_ignoredefense_p;   //忽略敌人闪避率
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++;
	pDes++; //2  普功伤害 百分比
	if (pTemp->nAttribType == magic_physicsenhance_p)
	{ 
		pDes->nAttribType = magic_physicsenhance_p;//magic_physicsdamage_v;   //赋值给为普点
		pDes->nValue[0] = nMinDamage * (100 + pTemp->nValue[0]) / 100;
		pDes->nValue[2] = nMaxDamage * (100 + pTemp->nValue[0]) / 100;
			   
	if (nIsMaigc)
	{//脚本基本数据+内普状态(装备+技能的状态)
		pDes->nValue[0] = (pDes->nValue[0]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
		pDes->nValue[2] = (pDes->nValue[2]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
	}
	else
	{//脚本基本数据+外普状态(装备+技能的状态)
		pDes->nValue[0] = (pDes->nValue[0]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
		pDes->nValue[2] = (pDes->nValue[2]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
	} 

	}
	pTemp++;
	pDes++;//3  冰伤害
	if (pTemp->nAttribType == magic_colddamage_v)
	{
		pDes->nAttribType = magic_colddamage_v;
		pDes->nValue[0] = pTemp->nValue[0]+nMinDamage;
		pDes->nValue[1] = pTemp->nValue[1]; 
		pDes->nValue[2] = pTemp->nValue[2]+nMaxDamage;//*(100+ m_CurrentColdEnhance)/100;  
	if (nIsMaigc) 
	{//内冰状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicColdDamage.nValue[0]+m_CurrentAddmagicColdDamagicv;
		if  ((2+m_CurrentColdEnhance+m_CurrentMagicColdDamage.nValue[1])!= 0)
			pDes->nValue[1] = (pTemp->nValue[0]+pTemp->nValue[2]+m_CurrentAddmagicColdDamagicv)/2+m_CurrentColdEnhance+m_CurrentMagicColdDamage.nValue[1];//max(pDes->nValue[1]+m_CurrentAddmagicColdDamagicv,m_CurrentAddmagicColdDamagicv+m_CurrentMagicColdDamage.nValue[1] + m_CurrentColdEnhance); //冰冻时间
		else
			pDes->nValue[1] = 0;
		pDes->nValue[2] += m_CurrentMagicColdDamage.nValue[2]+m_CurrentAddmagicColdDamagicv;
	} 
	else
	{//外冰状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicColdDamage.nValue[0]+m_CurrentAddColdDamagev;
		if ((2+m_CurrentColdEnhance+m_CurrentMagicColdDamage.nValue[1])!=0) 
			pDes->nValue[1] = (pTemp->nValue[0]+pTemp->nValue[2]+m_CurrentAddColdDamagev)/2+m_CurrentColdEnhance+m_CurrentMagicColdDamage.nValue[1];//max(pDes->nValue[1]+m_CurrentAddColdDamagev, m_CurrentAddColdDamagev+m_CurrentMagicColdDamage.nValue[1] + m_CurrentColdEnhance);   //冰冻时间
		else
			pDes->nValue[1] = 0;
		pDes->nValue[2] += m_CurrentMagicColdDamage.nValue[2]+m_CurrentAddColdDamagev;
	}
	}
	pTemp++;
	pDes++;//4 火伤害
	if (pTemp->nAttribType == magic_firedamage_v)
	{
		pDes->nAttribType = magic_firedamage_v;
		pDes->nValue[0] = pTemp->nValue[0]+nMinDamage; //基本技能数据
		pDes->nValue[2] = (pTemp->nValue[2]+nMaxDamage)*(100 + m_CurrentFireEnhance)/ 100; //基本技能数据

	if (nIsMaigc) 
	{//脚本技能基本数据+内火状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicFireDamage.nValue[0]+m_CurrentAddmagicFireDamagicv;
	 	pDes->nValue[2] += m_CurrentMagicFireDamage.nValue[2]+m_CurrentAddmagicFireDamagicv;
	}
	else
	{ //脚本技能基本数据+外火状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicFireDamage.nValue[0]+m_CurrentAddFireDamagev;
		pDes->nValue[2] += m_CurrentMagicFireDamage.nValue[2]+m_CurrentAddFireDamagev;
	} 
	}
	pTemp++;
	pDes++;//5 雷伤害
	if (pTemp->nAttribType == magic_lightingdamage_v)
	{
		pDes->nAttribType = magic_lightingdamage_v;
		pDes->nValue[0]   = pTemp->nValue[0]+nMinDamage;
		pDes->nValue[2]   = pTemp->nValue[2]+nMaxDamage;//(pTemp->nValue[2]+nMaxDamage) * (100 + m_CurrentLightEnhance)/ 100;
	  if (nIsMaigc)
	  { //脚本技能基本数据+内雷状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicLightDamage.nValue[0]+m_CurrentAddmagicLightDamagicv;
		pDes->nValue[2] += m_CurrentMagicLightDamage.nValue[2]+m_CurrentAddmagicLightDamagicv;
	  } 
	  else//if (bIsPhysical)
	  { //脚本技能基本数据+外雷状态(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicLightDamage.nValue[0]+m_CurrentAddLighDamagev;
		pDes->nValue[2] += m_CurrentMagicLightDamage.nValue[2]+m_CurrentAddLighDamagev;
	  }  
	}
	pTemp++;
	pDes++;//6 毒伤害
	if (pTemp->nAttribType == magic_poisondamage_v)
	{
		pDes->nAttribType = magic_poisondamage_v;
		pDes->nValue[0] = pTemp->nValue[0];//* (100 + m_CurrentPoisonEnhance) / 100;//技能基本数据
		pDes->nValue[1] = pTemp->nValue[1];//技能持续时间
		pDes->nValue[2] = pTemp->nValue[2];//技能毒发时间

		if (pDes->nValue[2] <= 0)
			pDes->nValue[2] = 18;

	if (nIsMaigc) 
	{//内毒(装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicPoisonDamage.nValue[0]+m_CurrentAddmagicPoisonDamagicv;  //毒伤害
		pDes->nValue[1] += m_CurrentMagicPoisonDamage.nValue[1];  //持续时间
		pDes->nValue[2] += m_CurrentMagicPoisonDamage.nValue[2];  //毒发时间
		
	} 
	else
	{//外毒 (装备+技能的状态)
		pDes->nValue[0] += m_CurrentMagicPoisonDamage.nValue[0]+m_CurrentAddPoisonDamagev;        //毒伤害
		pDes->nValue[1] += m_CurrentMagicPoisonDamage.nValue[1];  //持续时间
		pDes->nValue[2] += m_CurrentMagicPoisonDamage.nValue[2];  //毒发时间
	} 
	//合成新的毒伤害
        //g_NpcAttribModify.MixPoisonDamage(&m_CurrentPoisonDamage,pDes);
	}
	pTemp++;
	pDes++;//7 魔法伤害（穿刺伤害 无视防御）
	if (pTemp->nAttribType == magic_magicdamage_v)  // 魔法伤害上限
	{
		pDes->nAttribType = magic_magicdamage_v;
		pDes->nValue[0] = pTemp->nValue[0]+nMinDamage;
		pDes->nValue[2] = pTemp->nValue[2]+nMaxDamage;
	}
	pTemp++;
	pDes++;// 8 眩晕伤害
	if (pTemp->nAttribType == magic_stun_p)
	{
		pDes->nAttribType = magic_stun_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		//pDes->nValue[1] += pTemp->nValue[1] * m_CurrentLightEnhance / 100;
	}
	pTemp++;
	pDes++; // 9 会心攻击
	if (pTemp->nAttribType == magic_deadlystrike_p)
	{
		pDes->nAttribType = magic_deadlystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];

	  if (bIsPhysical)
	  { //是外功系
		pDes->nAttribType = magic_deadlystrike_p;
		pDes->nValue[0] += m_CurrentDeadlyStrike;
	  }  
	}
	pTemp++;
	pDes++;// 10 致命一击
	if (pTemp->nAttribType == magic_fatallystrike_p)
	{
		pDes->nAttribType = magic_fatallystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++;
	pDes++;// 11 吸血 设置
	if (pTemp->nAttribType == magic_steallife_p || pTemp->nAttribType == magic_steallifeenhance_p)
	{
		pDes->nAttribType = magic_steallife_p;   
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++;
	pDes++; // 12 吸蓝 设置
	if (pTemp->nAttribType == magic_stealmana_p || pTemp->nAttribType == magic_stealmanaenhance_p)
	{
		pDes->nAttribType = magic_stealmana_p; 
    //	if (nIsMaigc)
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++;
	pDes++; // 13  五行伤害
	if (pTemp->nAttribType == magic_seriesdamage_p)  //13
	{
		pDes->nAttribType = magic_seriesdamage_p;
		pDes->nValue[0] = pTemp->nValue[0];	         //技能

	    if (bIsPhysical)
		{  
		   pDes->nAttribType = magic_seriesdamage_p;
		   pDes->nValue[0] += m_CurrentSerisesEnhance;	//状态
		}  
	}
	pTemp++;
	pDes++;  // 14 自动释放技能
	if (pTemp->nAttribType == magic_autoattackskill )    //自动技能 14
	{
		pDes->nAttribType = magic_autoattackskill;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
      #ifdef _SERVER
	    pDes->nAttribType = magic_autoattackskill;
	    pDes->nValue[0] = m_nAutoSkillId;
	    pDes->nValue[1] = m_nAutoLevel;
	    pDes->nValue[2] = m_nAutoRate;
      #endif // _SERVER
    }
	pTemp++;
	pDes++;  // 15  普点
	if (pTemp->nAttribType == magic_physicsdamage_v)
	{ 
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = pTemp->nValue[0]+nMinDamage;
		pDes->nValue[2] = pTemp->nValue[2]+nMaxDamage;

	if (nIsMaigc)
	{//脚本基本数据+内普状态(装备+技能的状态)
		pDes->nValue[0] = (pDes->nValue[0]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
		pDes->nValue[2] = (pDes->nValue[2]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
	}
	else
	{//脚本基本数据+外普状态(装备+技能的状态)
		pDes->nValue[0] = (pDes->nValue[0]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
		pDes->nValue[2] = (pDes->nValue[2]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
	}
	}
	pTemp++;
	pDes++;  // 16  毒发时间
	if (pTemp->nAttribType == magic_poisonenhance_p)
	{
		pDes->nAttribType = magic_poisonenhance_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;
	pDes++;  // 17  迟缓时间
	if (pTemp->nAttribType == magic_coldenhance_p)
	{
		pDes->nAttribType = magic_coldenhance_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;
	pDes++;  // 18  抓捕技能  
	if (pTemp->nAttribType == magic_addzhuabu_v)    //抓捕技能 18
	{
		pDes->nAttribType = magic_addzhuabu_v;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	} 

 }

void KNpc::ServeMove(int MoveSpeed)  // 服务器移动
{

	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack)
		return;

	if (MoveSpeed <= 0)
		return;

	if (MoveSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{//移动的速度太快的
		MoveSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;	//31   15*
	}
	if (m_RegionIndex < 0 || m_RegionIndex >= 9) //区域数量判断
	{
		//g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		//_ASSERT(0);
		DoStand(); //站着
		return;
	}
	int x, y;

	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,0,0,&x,&y); //区域格子转换成像素坐标

	x = (x << 10) + m_OffX;
	y = (y << 10) + m_OffY;

	int nRet = m_PathFinder.GetDir(x,y,m_Dir,m_DesX,m_DesY,MoveSpeed,&m_Dir);  //调用寻路函数

	if(nRet == 1)  //有路可以走
	{
		if (m_Dir >= MaxMissleDir)
			m_Dir=m_Dir%MaxMissleDir;

		x = g_DirCos(m_Dir,MaxMissleDir)*MoveSpeed*FIND_PATH_DISTANCE;  //下个要走的点
		y = g_DirSin(m_Dir,MaxMissleDir)*MoveSpeed*FIND_PATH_DISTANCE;

		if (m_Kind==kind_player)
		{ 
			/*char nPos[128];
			t_sprintf(nPos,"<color=gyellow>方向:%d,下一个点:X:%d,Y:%d<color>",m_Dir,x,y);
			//Player[nPlayerIndex].m_ItemList.ClientShowMsg(nPos);
			CCMessageBox(nPos,"ServeMove");*/
		} 

	}
	else if (nRet == 0) //路不通
	{
		DoStand(); //站着
		//CCMessageBox("路不通","ServeMove");
		return;
	}
	else if (nRet == -1) //到地图外面
	{//删除NPC
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
		m_RegionIndex = -1;
		return;
	}
	else
	{
		//CCMessageBox("其他错误","ServeMove");
		return;
	}

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;

	m_OffX +=x;
	m_OffY +=y;
//	处理NPC的坐标变幻
//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍

	if (!m_bClientOnly)
		CURREGION.DecNpcRef(m_MapX, m_MapY);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}

	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}

	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}

		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddNpcRef(m_MapX,m_MapY);
	}

	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{//恢复原点
		//要不要处理下 删除精灵?
		if (g_GameWorld)
		{//删除所有精灵
           g_GameWorld->FreeAllSprite();
		}
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{//如果是新区域就转新区域
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID,SubWorld[0].m_Region[m_RegionIndex].m_RegionID,m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
}

void KNpc::ServeJump(int nSpeed)
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (!(m_Doing == do_jump || m_Doing == do_jumpattack))
		return;
	
	if (nSpeed <= 0)
		return;

	if (nSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		nSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

	m_OffX += g_DirCos(m_JumpDir, 64) * nSpeed;
	m_OffY += g_DirSin(m_JumpDir, 64) * nSpeed;

	// s = vt - a * t * t / 2
	m_Height = (m_JumpFirstSpeed * m_Frames.nCurrentFrame - ACCELERATION_OF_GRAVITY * m_Frames.nCurrentFrame * m_Frames.nCurrentFrame / 2) / 8;
	if (m_Height < 0)
		m_Height = 0;

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	CURREGION.DecNpcRef(m_MapX, m_MapY);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}
	
	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (m_RegionIndex >= 0)
			CURREGION.AddNpcRef(m_MapX, m_MapY);
	}

	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddNpcRef(m_MapX, m_MapY);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
		if (m_RegionIndex >= 0)
		{
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
	}		
}

void KNpc::SendSerCommand(NPCCMD cmd,int x,int y, int z)
{
	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;
}

BOOL KNpc::NewPath(int nMpsX, int nMpsY)
{
	m_DesX = nMpsX;
	m_DesY = nMpsY;
	return TRUE;
}

BOOL KNpc::NewJump(int nMpsX, int nMpsY)
{
	//_ASSERT(m_CurrentJumpSpeed > 0);
	if (m_CurrentJumpSpeed <= 0)
		return FALSE;
	
	int nX, nY,nMap;
	    GetMpsPos(&nX, &nY,&nMap);
														
		if (nX == nMpsX && nY == nMpsY)	//如果NPC坐标与 目标相同就 不计算了
		return FALSE;

	int nDir = g_GetDirIndex(nX, nY, nMpsX, nMpsY);	 //获取两个点之间的方向
	int	nMaxLength = m_CurrentJumpSpeed * m_CurrentJumpFrame;
	int	nWantLength = g_GetDistance(nX, nY, nMpsX, nMpsY); //两个点的距离

	if (m_Dir >= MaxMissleDir)
		m_Dir -= MaxMissleDir;

	int	nSin = g_DirSin(nDir, MaxMissleDir);
	int	nCos = g_DirCos(nDir, MaxMissleDir);

	if (nWantLength > nMaxLength)
	{
		m_DesX = nX + ((nMaxLength * nCos) >> 10);
		m_DesY = nY + ((nMaxLength * nSin) >> 10);
		nWantLength = nMaxLength;
	}
	else if (nWantLength <= MIN_JUMP_RANGE)
	{
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		return FALSE;
	}

	m_JumpStep = nWantLength / m_CurrentJumpSpeed;	  //要挑多少步

	int nTestX = 0;
	int nTestY = 0;
	int nSuccessStep = 0;

	for (int i = 1; i < m_JumpStep + 1; ++i)
	{
		nTestX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
		nTestY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
		int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY); //这个点是否有障碍
		if (Obstacle_NULL == nBarrier)
		{
			nSuccessStep = i;
		}
		if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier)
		{
			if (nSuccessStep <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * nSuccessStep) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * nSuccessStep) >> 10);
			m_JumpStep = nSuccessStep;
			break;
		}

		int	nTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);

		if (nTrap)
		{//有Trap
			if (i <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
			m_JumpStep = i;
			break;
		}
	}
	m_JumpDir = nDir;
	return TRUE;
}

/*void KNpc::SelfDamage(int nDamage)
{
	m_CurrentLife -= nDamage;

	if (m_CurrentLife <= 0)
	{
		m_CurrentLife = 1;
	}
} */
//提示内力不足 生命不足 体力不足设置
BOOL KNpc::Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast)
{
	if (!IsPlayer())
		return TRUE;

	int *pSource = NULL;

	switch(nType)
	{
	case attrib_mana:
		pSource = &m_CurrentMana;
		break;
	case attrib_life:
		pSource = &m_CurrentLife;
		break;
	case attrib_stamina:
		pSource = &m_CurrentStamina;
		break;
	default:
		break;
	}

	if (pSource)
	{
		if (*pSource - nCost<=0)
		{
			KSystemMessage Msg;

			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 1;
			Msg.eType = SMT_NORMAL;

			switch(nType)
			{
			case attrib_mana:
				g_StrCpyLen(Msg.szMessage,  strCoreInfo[MSG_NPC_NO_MANA].c_str(), sizeof(Msg.szMessage));
				break;
			case attrib_life:
				g_StrCpyLen(Msg.szMessage, strCoreInfo[MSG_NPC_NO_LIFE].c_str(), sizeof(Msg.szMessage));
				break;
			case attrib_stamina:
				g_StrCpyLen(Msg.szMessage, strCoreInfo[MSG_NPC_NO_STAMINA].c_str(), sizeof(Msg.szMessage));
				break;
			default:
				break;
			}
			//Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
			//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
			return FALSE;
		}
		else
		{
			if (!bOnlyCheckCanCast)
				*pSource -= nCost;
			return TRUE;
		}
	}
	return FALSE;
}

void KNpc::DoJump()
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_jump)
		return;
	
	m_Doing = do_jump;
	m_Dir = m_JumpDir;
	m_ProcessAI	= 0;
	m_JumpFirstSpeed = ACCELERATION_OF_GRAVITY * (m_JumpStep - 1) / 2 ;
	m_ClientDoing = cdo_jump;
	m_Frames.nTotalFrame = m_JumpStep;
	m_Frames.nCurrentFrame = 0;	
}

BOOL KNpc::OnJump()
{
	ServeJump(m_CurrentJumpSpeed);
	if (WaitForFrame())
	{
		DoStand();
		m_ProcessAI	= 1;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::WaitForFrame()
{
	m_Frames.nCurrentFrame ++;
	if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame)
	{
		return FALSE;
	}
	m_Frames.nCurrentFrame = 0;
	return TRUE;
}

BOOL KNpc::IsReachFrame(int nPercent)
{
	if (m_Frames.nCurrentFrame == m_Frames.nTotalFrame * nPercent / 100)
	{
		return TRUE;
	}
	return FALSE;
}
//刷怪
//客户端从网络得到的NpcSettingIdx是包含高16位Npc的模板号与低16位为等级
void KNpc::Load(int nNpcSettingIdx, int nLevel,int nSubWorld,int nBoss)
{
	m_PathFinder.Init(m_Index);

	if (nLevel <= 0) 
	{
		nLevel = 1;
	}

	//特殊NPC 玩家
	if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || nNpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	{
		m_NpcSettingIdx = nNpcSettingIdx;
		m_Level = nLevel;
		
		if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			//CCMessageBox("start add nan role","start add nan");
			strcpy(szNpcTypeName, "MainMan");
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			//CCMessageBox("start add nv role","start add nv");
			strcpy(szNpcTypeName,"MainLady");
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
		m_WalkSpeed   = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed    = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	  = NpcSet.GetPlayerHurtFrame();
	}
	else
	{ //普通npc 怪物
		GetNpcCopyFromTemplate(nNpcSettingIdx, nLevel);  //拷贝 NPCs.TXT 里面的基本信息！！
		g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{ //人物类型表 npcres
			g_NpcKindFile.GetString(2, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
		}
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIMode", 12, &m_AiMode);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam1", 12, &m_AiParam[0]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam2", 12, &m_AiParam[1]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam3", 12, &m_AiParam[2]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam4", 12, &m_AiParam[3]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam5", 12, &m_AiParam[4]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam6", 12, &m_AiParam[5]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam7", 12, &m_AiParam[6]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam8", 12, &m_AiParam[7]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam9", 12, &m_AiParam[8]);

		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ActiveRadius", 12, &m_ActiveRadius); //活动范围
		int m_nIsRevive;
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ClientOnly", 0, &m_nIsRevive);
		if  (m_nIsRevive>0)
			m_bClientOnly = true;
		else
			m_bClientOnly =false;
		m_nIsRevive = 0;
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "IsRevive", 0, &m_nIsRevive);
	    m_IsRevive = (DWORD)m_nIsRevive;
        char nMapNpc[32]={0};
		int  nIsCreat=0;
		ZeroMemory(nMapNpc,sizeof(nMapNpc));
		t_sprintf(nMapNpc,"%d_IsScript",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);
		IsExeGoldScript = nIsCreat; 
		t_sprintf(nMapNpc,"%d_IsCreat",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);//默认执行全局脚本
	    IsCreatBoss     = nIsCreat; 
		// 飞行类，11，12，17，用AiParam[6]保存策划设定高度
		// add by flying
		if (m_AiMode == 11 || m_AiMode == 12 || m_AiMode == 17)
			m_AiParam[6] = m_AiMode;
	}
    //普通NPC 和特殊NPC 通用
	/*m_DataRes.Init(szNpcTypeName, &g_NpcResList);//外观初始化
	m_DataRes.SetAction(m_ClientDoing);  //设定外观行为对接
	m_DataRes.SetRideHorse(m_bRideHorse);//设定是否骑马接帧数
	m_DataRes.SetArmor(m_ArmorType);     //衣服
	m_DataRes.SetHelm(m_HelmType);       //头部
	m_DataRes.SetHorse(m_HorseType);     //马位置
	m_DataRes.SetWeapon(m_WeaponType);   //武器
	*/
  if (m_Kind==kind_player)
  { 
        // m_DataRes.SetPifeng(m_PifengType); //披风
        // m_DataRes.SetChiBang(m_ChiBangType);//翅膀
	 // CCMessageBox("add player finish..","add player finish.");
  }
	m_CurrentCamp = m_Camp;
}

void KNpc::GetMpsPos(int *pPosX, int *pPosY,int *nMapid)
{
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex,m_MapX,m_MapY,m_OffX,m_OffY,pPosX,pPosY);
    *nMapid=m_SubWorldIndex;
}


//设置为激活技能
BOOL	KNpc::ChangeSkillAttackRadius(int nSkillIdx,int nDis)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

    if (!IsPlayer())
      return FALSE;

	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;

	int nLevel = m_SkillList.m_Skills[nSkillIdx].SkillLevel;

	int nChanSkillID= m_SkillList.m_Skills[nSkillIdx].SkillId;

	if  (nChanSkillID >=MAX_SKILL && nLevel >=MAX_SKILLLEVEL && nLevel <=0)
		return FALSE;

	ISkill * pISkill =  g_SkillManager.GetSkill(nChanSkillID, nLevel);

	if (pISkill)
	{
		//int nTempDis = 0;
		if (!Player[CLIENT_PLAYER_INDEX].m_isopenjuli)
			nDis  = pISkill->getBackAttackRadius();
		//恢复攻击距离
		m_CurrentAttackRadius = pISkill->ChangeAttackRadius(nDis); //得到技能的攻击范围
     //客户端发送协议同步给服务器
		//char Info[64];
		//t_sprintf(Info,"助手调整攻击距离:%d",m_CurrentAttackRadius);
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Info);
		SendClientRunScript(1,nSkillIdx,nDis,nChanSkillID);
	}
	return TRUE;
}

//设置为激活技能
BOOL	KNpc::SetActiveSkill(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;

	m_ActiveSkillID = m_SkillList.m_Skills[nSkillIdx].SkillId;
	m_ActiveSkListIndex =nSkillIdx;
	int nLevel = m_SkillList.m_Skills[nSkillIdx].SkillLevel;
	//_ASSERT(m_ActiveSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL && nLevel > 0);
	if  (m_ActiveSkillID >=MAX_SKILL || nLevel >=MAX_SKILLLEVEL || nLevel <=0)
		return FALSE;

	ISkill * pISkill =  g_SkillManager.GetSkill(m_ActiveSkillID, nLevel);

	if (pISkill)
    {
		m_CurrentAttackRadius = pISkill->GetAttackRadius(); //得到技能的攻击范围
    }
	return TRUE;
}
//获取第几个技能的ID
int	KNpc::GetSkillID(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;
	
	if (!(m_SkillList.m_Skills[nSkillIdx].SkillId && m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel))
		return FALSE;
	return m_SkillList.m_Skills[nSkillIdx].SkillId;
}

// 同步客户端属性状态技能等数据
void KNpc::UpdataNpcCurData(int nWonerIndex,int nEquipPlace) //更新自身的属性参数
{

/*	for (int j = 0 ; j< MAX_NPCSKILL;j++)
	{
		int nAdd = 	Npc[m_nIndex].m_SkillList.GetAddPoint(j);
		if (nAdd > 0)
		{
			nAdd = -nAdd;
#ifdef _SERVER	
			Npc[m_nIndex].m_SkillList.IncreaseLevel(j,nAdd); //更新清除技能被动状态
#endif
			Npc[m_nIndex].m_SkillList.QeuipAddPoint(j,nAdd);
		}

		Npc[m_nIndex].m_SkillList.m_Skills[j].mAddPoint = 0;
		Npc[m_nIndex].m_SkillList.m_Skills[j].EnChance = 0;  // 技能的加成
	}*/

	ZeroMemory(&m_CurrentPhysicsMagicDamageP, sizeof(KMagicAttrib));
	ZeroMemory(&m_CurrentPhysicsMagicDamageV, sizeof(KMagicAttrib));

/*	int nFomoidx=Npc[m_nIndex].GetCurFuMoIdx(),nCurbei=0;
	
	if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	}*/

	m_CurrentLifeMax		= m_LifeMax;
	m_CurrentManaMax		= m_ManaMax;
	m_CurrentStaminaMax	= m_StaminaMax;
	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed	= m_CastSpeed;
    m_CurrentRunSpeed	    = m_RunSpeed; 
	m_CurrentJumpSpeed	= m_JumpSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentWalkSpeed	= m_WalkSpeed;
	m_CurrentNuQiMax		= 0;//怒气
	m_CurrentDefend		= m_Defend;
	m_CurrentLifeReplenish= m_LifeReplenish;
	m_CurrentHitRecover	= m_HitRecover;  //Npc的受伤动作时间
	m_CurrentManaReplenish= m_ManaReplenish;

	m_TempFireResist = 0;	        // Npc的当前火抗性
	m_TempColdResist = 0;	            // Npc的当前冰抗性
	m_TempPoisonResist = 0;	        // Npc的当前毒抗性
	m_TempLightResist = 0;	        // Npc的当前电抗性
	m_TempPhysicsResist = 0;	    // Npc的当前物理抗性

	m_CurrentLightResist	= BASE_NPC_RESIST_MAX; //m_LightResist;
	m_CurrentPhysicsResist  = BASE_NPC_RESIST_MAX; //m_PhysicsResist;
	m_CurrentPoisonResist	= BASE_NPC_RESIST_MAX; //m_PoisonResist;
	m_CurrentColdResist	    = BASE_NPC_RESIST_MAX; //m_ColdResist;
	m_CurrentFireResist	    = BASE_NPC_RESIST_MAX; //m_FireResist;
	m_nCurNpcLucky          = BASE_NPC_RESIST_MAX;

	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;
	//SetBaseSpeedAndRadius();
	ZeroMemory(&m_CurrentMagicColdDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_CurrentColdDamage, sizeof(KMagicAttrib));
	m_CurrentColdEnhance	= 0;

	m_CurrentColdResistMax	= MAX_RESIST;//m_ColdResistMax;
	m_CurrentFireResistMax  = MAX_RESIST;
	m_CurrentLightResistMax = MAX_RESIST;//m_LightResistMax;
	m_CurrentPhysicsResistMax = MAX_RESIST;//m_PhysicsResistMax;
	m_CurrentPoisonResistMax  = MAX_RESIST;//m_PoisonResistMax;

	m_CurrentDamage2Mana	= 0;
	m_CurrentDamageReduce	= 0;
	m_CurrentDeadlyStrike	= 0;
//	m_CurrentElementDamageReduce = 0;
	ZeroMemory(&m_CurrentMagicFireDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_CurrentFireDamage, sizeof(KMagicAttrib));
	m_CurrentFireEnhance	= 0;
	
	m_CurrentHandEnhance	= 0;
	m_CurrentKnockBack	= 0;
	m_CurrentLifeStolen	= 0;
	ZeroMemory(&m_CurrentMagicLightDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_CurrentLightDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_WaiPhysicsDamage, sizeof(KMagicAttrib));
	m_CurrentLightEnhance	= 0;
	m_CurrentPoisonTime=0;

//	m_CurrentManaPerEnemy	= 0;
	m_CurrentManaStolen	= 0;
	m_CurrentMeleeDmgRet	= 0;        // 近程反弹
	m_CurrentHulueMeleeDmgRet=0;      // 忽略近程反弹多少点、
   	m_CurrentHulueRangeDmgRet=0;      // 忽略远程程反弹多少点、
	m_CurrentMeleeDmgRetPercent = 0;  // 近程反弹百分比
	m_CurrentTempSpeed=0;
	ZeroMemory(&m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));

//	m_CurrentPiercePercent	= 0;
	ZeroMemory(&m_CurrentMagicPoisonDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&m_CurrentPoisonDamage, sizeof(KMagicAttrib));
	m_CurrentPoisonEnhance	= 0;

	m_CurrentRangeDmgRet	= 0;         //远程反弹
	m_CurrentRangeDmgRetPercent	= 0; //远程反弹百分比
	m_CurrentRangeEnhance	= 0;
	m_CurrentSlowMissle	= 0;         //子弹减速
	m_CurrentStaminaStolen	= 0;
	m_CurrentAddPhysicsDamage = 0;      //外功普点
	m_CurrentUpExp        = 0;
	m_CurrentFreezeTimeReducePercent = 0;
	m_CurrentPoisonTimeReducePercent = 0;
	m_EnemyPoisonTimeReducePercent   = 0;
	m_CurrentStunTimeReducePercent = 0;
	m_EnemyStunTimeReducePercent   = 0;
	m_CurrentSerisesEnhance = 0;
	m_CurrentDamageReduce = 0;
	m_CurrentdanggeRate = 0;  //档格
	m_CurrentzhongjiRate = 0; //重击
	 m_CurrentcjdanggeRate	= 0;               //拆解档格
	 m_CurrentcjzhongjiRate	= 0;               //拆解重击
	 m_Currentsorbdamage	= 0;                   //抵消伤害
	 m_Currentsorbdamage_v=0;
	 m_Currenadddamagev   = 0;
	 m_Currenadddamagep   = 0;
	 m_Currentpoisonres	= 0;                   //=忽略对方毒防:#d1-%
	 m_Currentfireres	= 0;                       //=忽略对方火防:#d1-%
	 m_Currentlightingres	= 0;                   //=忽略对方雷防:#d1-%
	 m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
	 m_Currentcoldres	= 0;                       //=忽略对方冰防:#d1-%
	 m_Currentallres	= 0; 
	 m_Currentnopkvalue	= 0;                   //=不增加PK值概率:#d1+%
	 m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	 m_Currentelementsenhance	= 0;               //=五行强化值：#d1-点。强化对相克五行的克制效果
     m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	 m_Currentskillenhance=0;                    //技能的加成
//	  ZeroMemory(m_CurrentSkillEnhance,sizeof(m_CurrentSkillEnhance));
	/* _EnhanceInfo::iterator it;
	 for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	 {
		 it->second.nSkillIdx = 0;
		 it->second.nEnhance = 0;
	 }*/
	 nEnhanceInfo.clear();

	 m_CurrentFullManaskillenhance=0;
	 m_CurrentautoReviverate=0;                  //复活概率
	 m_CurrentAddPhysicsDamageP= 0;     // 当前被动外普百分比
	 m_CurrentAddFireDamagev= 0;        // 当前被动外火点
	 m_CurrentAddColdDamagev= 0;        // 当前被动外冰点 
	 m_CurrentAddLighDamagev= 0;        // 当前被动外雷点
	 m_CurrentAddPoisonDamagev= 0;      // 当前被动外毒点
	 m_CurrentAddmagicphysicsDamage= 0; // 当前被动内普点
	 m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	 m_CurrentAddmagicColdDamagicv= 0;  // 当前被动内冰点
	 m_CurrentAddmagicFireDamagicv= 0;  // 当前被动内火点  
	 m_CurrentAddmagicLightDamagicv= 0; // 当前被动内雷点
     m_CurrentAddmagicPoisonDamagicv= 0;// 当前被动内毒点
	 m_CurrentPoisondamagereturnV=0;
	 m_CurrentPoisondamagereturnP=0;
	 m_Currentbaopoisondmax_p=0;
	 m_CurrentReturnskillp=0;
	 m_CurrentIgnoreskillp=0;
	 m_CurrentReturnresp=0;
	 m_CurrentCreatnpcv=0;
	 m_CurrentAllJiHuo=0; //是否全身激活
	 m_CurrentCreatStatus=0;
	 m_CurrentAttackRatingEnhancep=0;
	 m_CurrentAttackRatingEnhancev=0;
	 m_CurrentIgnorenAttacRating  =0;
	 m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	 m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	 m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	 m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	 m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	 m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	 m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	 m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	 m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	 m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
     m_CurrentStunRank_p=0;
	 m_Staticmagicshield_p=0;
	 ReCalcNpcEquip(nWonerIndex,nEquipPlace); //重新计算身上的装备
	 //ReCalcState(nIsLoign); //重新计算技能的状态
}

void KNpc::ReCalcNpcEquip(int nWonerIndex,int nEquipPlace)
{
	
  if (nWonerIndex<=0)	//如果主人不存在的话
		return;

  if (nEquipPlace==-1)
  {
	  int nNsta,nNstb,nNstc,nNstd,nNste;
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_0",0,&nNsta);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_1",0,&nNstb);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_2",0,&nNstc);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_3",0,&nNstd);
	      g_GameSetTing.GetInteger("FuMoNpcSet","Mo_4",0,&nNste);
	  
	  if (m_NpcSettingIdx ==nNsta)
	  {//金
		 nEquipPlace=53;
	  }
	  else if (m_NpcSettingIdx ==nNstb)
	  {//木
		  nEquipPlace=58;
	  }
	  else if (m_NpcSettingIdx ==nNstc)
	  {//水
		  nEquipPlace=63;
	  }
	  else if (m_NpcSettingIdx ==nNstd)
	  {//火
		  nEquipPlace=68;
	  }
	  else if (m_NpcSettingIdx ==nNste)
	  {//土
		  nEquipPlace=73;
	  }
  }
 //金53-57 木58-62 水63-67 火68-72 土73-77
  int i;
  if (53<=nEquipPlace && nEquipPlace<=57)
  {//金
	for (i = 53; i < 58; ++i)
	{
		if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	}
  }
  else if (58<=nEquipPlace && nEquipPlace<=62)
  {//木
	  for (i = 58; i < 63; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (63<=nEquipPlace && nEquipPlace<=67)
  {//水
	  for (i = 63; i < 68; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (68<=nEquipPlace && nEquipPlace<=72)
  {//火
	  for (i = 68; i < 73; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }
  else if (73<=nEquipPlace && nEquipPlace<=77)
  {//土
	  for (i = 73; i < 78; ++i)
	  {
		  if (!UpMagicAttrib(i,nWonerIndex))
			  continue;
	  }
  }

}

BOOL KNpc::UpMagicAttrib(int i,int nWonerIndex)
{
	int nIdx =Player[nWonerIndex].m_ItemList.GetItemBox(i);
	
	if (!nIdx)
	   return FALSE;
	
	int nActive = 3;//m_ItemList.GetEquipEnhance(i);  //获取激活的数量
	
	//除马匹
	//	if (i != itempart_horse || Npc[m_nIndex].m_bRideHorse)  //除掉马的部位 或 骑着马
	{
		int nActiveInfo[2];
		
		ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
		
		if (Item[nIdx].IsBlue())
		{//如果是蓝装
			nActiveInfo[0] = 1;
			nActiveInfo[1] = Item[nIdx].GetIsWhere();
			
		} 
		else if  (Item[nIdx].IsPurple())
		{//如果是紫装
			nActiveInfo[0] = 2;
			nActiveInfo[1] = Item[nIdx].GetIsWhere();
		} 
		else if  (Item[nIdx].IsGold())
		{//如果是黄装
			nActiveInfo[0] = 3;
			nActiveInfo[1] = Item[nIdx].GetLevel();//等级
		} 
		else
		{
			nActiveInfo[0] = 0;
			nActiveInfo[1] = 0;
		}
		
		if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
		{     
			Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_Index], nActive,nActiveInfo);
		}
		else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
		{//面具
			Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_Index], nActive,nActiveInfo);
		}
		
		}
	return TRUE;
}

//设置为光环技能
void KNpc::SetAuraSkill(int nSkillID)
{
	int nCurLevel = 0;

	if (nSkillID>g_SkillManager.GetSkillCount())
	{
		m_ActiveAuraID = 0;
		m_ActiveAuraIndex =0;
		return;
	}

	if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
    {
        nSkillID = 0;
    }
	else
	{
		nCurLevel = m_SkillList.GetCurrentLevel(nSkillID);
		if (nCurLevel <= 0) 
        {
            nSkillID = 0;
        }
		else
		{
			//_ASSERT(nSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL);
		   if (nSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL)
		   {
			   KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nCurLevel);
               if (!pOrdinSkill || !pOrdinSkill->IsAura())
			   { 
				 nSkillID  = 0;
			   }
		   }
		}
	}

	m_ActiveAuraID = nSkillID;
	m_ActiveAuraIndex = m_SkillList.FindSame(m_ActiveAuraID);

	SKILL_CHANGEAURASKILL_COMMAND ChangeAuraMsg;
	ChangeAuraMsg.ProtocolType = c2s_changeauraskill;
	ChangeAuraMsg.m_nAuraSkill = m_ActiveAuraID;
	if (g_pClient)
		g_pClient->SendPackToServer(&ChangeAuraMsg, sizeof(SKILL_CHANGEAURASKILL_COMMAND));
}

BOOL KNpc::SetPlayerIdx(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return FALSE;

	if (m_Kind != kind_player)
		return FALSE;

	m_nPlayerIdx = nIdx;
	return TRUE;
}

// -------------------------------------------------------------------------
// 函数		: KNpc::TestMovePos
// 功能		: 测试是否能够直线到达目标点
// 返回值	: BOOL 是否能到达
// 参数		: INT& nMpsX, INT& nMpsY	传入目标点地图坐标；	返回实际可以到达的坐标
// 参数		: INT& nLength				传入允许最大长度；		返回实际长度
// 参数		: INT  nSpeed				传入移动速度
// 参数		: BOOL bCanJumpOver			传入是否可以越过一部分障碍（可能是Npc之类的）
// 作者		: FanZai
// 附注		: 从旧的NewJump()修改而来
// -------------------------------------------------------------------------

BOOL KNpc::TestMovePos(IN OUT INT& nMpsX, IN OUT INT& nMpsY, IN OUT INT& nLength, INT nSpeed, BOOL bCanJumpOver)
{
	if (nMpsX < 0 || nMpsY < 0)
	{
		return FALSE;
	}
	if (nSpeed <= 0 || nSpeed > defLOGIC_CELL_WIDTH)
	{
		//_ASSERT(FALSE);	// 怎么会这样呢？
		return FALSE;
	}
	
	if (nLength <= 0)
	{
		//_ASSERT(FALSE);	// 允许最大长度0，不能移动
		return FALSE;
	}
	
	INT nX, nY,nMap;
	GetMpsPos(&nX, &nY,&nMap);
	
	INT	nTargetX	= nMpsX - nX;
	INT	nTargetY	= nMpsY - nY;	// 目标点偏移坐标
	
	INT	nTargetLength =				// 目标点偏移距离
		(INT)sqrt((FLOAT)(nTargetX * nTargetX + nTargetY * nTargetY));
	
	if (!nTargetLength)				// 距离度是0后面就要出错了
		return FALSE;
	
	INT	nDx	= (nTargetX * nSpeed << 10) / nTargetLength;
	INT	nDy	= (nTargetY * nSpeed << 10) / nTargetLength;	// 每步偏移坐标（放大1024倍）
	
	if (nTargetLength > nLength)
		nTargetLength = nLength;	// 实际跳跃长度
	
	INT	nBarrierTestCounts	= nTargetLength / nSpeed;		// 要测试的步数
	
	INT	nSuccessCounts = 0;			// 成功的步数（只算可以停的点。可以越过、但不可以停的点不算）
	
	INT nTestEX = nX << 10;
	INT nTestEY = nY << 10;			// 当前测试点放大1024倍坐标
	INT nTestX, nTestY;				// 当前测试点地图坐标
	Obstacle_Kind eTestBarrier;		// 当前测试点障碍信息
	
	for (INT i = 1; i <= nBarrierTestCounts; i++)
	{
		nTestX	= (nTestEX += nDx) >> 10;
		nTestY	= (nTestEY += nDy) >> 10;
		eTestBarrier	= (Obstacle_Kind)SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
		switch (eTestBarrier)
		{
		case Obstacle_NULL:
			nSuccessCounts = i;
			continue;
		case Obstacle_Normal:
		case Obstacle_Fly:
			break;
		case Obstacle_Jump:
		case Obstacle_JumpFly:
			if (bCanJumpOver)
				continue;
			else
				break;
		case 0xff:
			//_ASSERT(FALSE);	// 坐标超出地图范围？！
			return FALSE;
			break;
		default:
			//_ASSERT(FALSE);	// 有新障碍类型了？！
			return FALSE;
		}
		break;
	}
	nLength	= nSuccessCounts * nSpeed;
	nMpsX	= nX + ((nDx * nSuccessCounts) >> 10);
	nMpsY	= nY + ((nDy * nSuccessCounts) >> 10);
	
	return TRUE;
}

int	KNpc::CheckMaps(char *nKey,int nMapIdx)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nMaps =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nMaps);
		if (nMapIdx==nMaps)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}


int	KNpc::CheckAllItem(int nKeyCol,int nGen,int nDetail,int nPart)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nMaps[3];
		nMaps[0]=0;
		nMaps[1]=0;
		nMaps[2]=0;

		g_ForbitMap.GetInt3(i,nKeyCol,nMaps);

		if (nMaps[2]==-1)
			continue;

		if (nMaps[0]==nGen && nMaps[1]==nDetail && nMaps[2]==nPart)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}
//#include "scene/KScenePlaceC.h"
//绘制名字 血条 等
int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)  //绘制信息
{
	int nMpsX, nMpsY,nmMap;
	    GetMpsPos(&nMpsX, &nMpsY,&nmMap);
	///nFontSize=12;
	DWORD	dwColor;
	int nHeightOff = nHeightOffset + nFontSize + 1;
	int  nHigthOff = 0;
	
	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_Hide.nTime > 0 )//和玩家有敌对关系并且隐藏时间大于的 不显示
	{
		return 0;
	}

	if (m_Kind == kind_player) //玩家阵营颜色显示
	{
		switch(m_CurrentCamp)
		{
		case camp_begin: 
			dwColor = 0xffffffff;   //新手
			break;
		case camp_justice:
			dwColor = 0xff000000 | (255 << 16) | (168 << 8) | 94;
			break;
		case camp_evil:
			dwColor = 0xff000000 | (255 << 16) | (146 << 8) | 255;
			break;
		case camp_balance:
			dwColor = 0xff000000 | (85 << 16) | (255 << 8) | 145;
			break;
		case camp_free:
			dwColor = 0xff000000 | (255 << 16);  //杀手
			break;
		case camp_blue:
			dwColor = TGetColor("74,74,255");
			break;
		case camp_green:
			dwColor = TGetColor("0,249,0");  //杀手
			break;
		default:
			dwColor = 0xffff00ff;
			break;
		}
//		char  xinxi[128];
//      t_sprintf(xinxi,"[CQ:%d,SH:%d,PE:%d,LU:%d,PF:%d,JY:%d,JQ:%d,生命:%d,JN:%d,dwID:%d,TYPE:%d]",m_BtnFlag,m_WarShuishou,m_nPeopleIdx,IsLuKey,m_PifengType,IsJinYan,IsJinQian,m_CurrentLife,m_ActiveSkillID,m_dwID,this->m_cGold.GetGoldType());  
//		g_pRepresent->OutputText(nFontSize, xinxi, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(xinxi)/4, nMpsY-24, dwColor, 0, nHeightOff, dwBorderColor);
//		nHeightOffset += nFontSize + 1;
	
		char	szString[128]={0};		
        t_sprintf(szString,"%s",Name); 

		if (m_IsInCity==1)
		{
           strcat(szString, "☆凤");
		   //strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==11)
		{
			strcat(szString, "☆都");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==162)
		{
			strcat(szString, "☆理");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==37)
		{
			strcat(szString, "☆京");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==78)
		{
			strcat(szString, "☆阳");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==80)
		{
			strcat(szString, "☆州");
			//strcat(szString, m_ScriptPicPath);
		}
		else if (m_IsInCity==176)
		{
			strcat(szString, "☆安");
			//strcat(szString, m_ScriptPicPath);
		}
		/*else
		{	char nMsg[32];
		    t_sprintf(nMsg,"锁定:%d",m_nPeopleIdx);
			strcat(szString, nMsg);	 //m_WeaponType
		}*/

		//if (m_FreezeState.nTime || m_PoisonState.nTime || m_ConfuseState.nTime || m_StunState.nTime || m_Hide.nTime || m_randmove.nTime)
		 if (m_FreezeState.nTime || m_PoisonState.nTime || m_ConfuseState.nTime || m_StunState.nTime || m_Hide.nTime || m_randmove.nTime)
		{
			strcat(szString, "(");
			if (m_FreezeState.nTime)
				strcat(szString, "冰");
			//if (m_PoisonState.nTime)
			if (m_PoisonState.nTime)
				strcat(szString, "毒");
			if (m_ConfuseState.nTime)
				strcat(szString, "乱");
			if (m_StunState.nTime)
				strcat(szString, "晕");
			if (m_randmove.nTime)
				strcat(szString, "恐");
			if (m_Hide.nTime)
			   strcat(szString, "隐");
			strcat(szString, ")");
		}

  /*    if ( m_PifengType>0)	// 披风称号
		g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX +20- nFontSize * g_StrLen(Name)/4, nMpsY-10, dwColor, 0, nHeightOff, dwBorderColor); //+100
      else
        g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX- nFontSize * g_StrLen(Name)/4, nMpsY-10, dwColor, 0, nHeightOff, dwBorderColor); //+100   		
 */

	   nMpsY=nMpsY-10;

       nHeightOffset += nFontSize + 1;

       char Livename[40]={0}; 

       /*if (m_GameliveTime>0)
	   { 
		  dwColor=TGetColor("255,255,255");
		  dwBorderColor=TGetColor("0,0,0");
          int nHour=0,nMin=0,nSec=0;
		  
		  nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时
		  
		  nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
		  
		  if (nMin>0)
		  {
			  nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
			  
		  }
		  else
			  nSec  = m_GameliveTime-nHour*3600;
		  	  
		  wt_sprintf(Livename,"(剩余时间:%d小时%d分%d秒)",nHour,nMin,nSec);
		  g_pRepresent->OutputText(nFontSize,Livename, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name)/4-50, nMpsY+150, dwColor, 0, nHeightOff, dwBorderColor);
		  nHeightOffset += nFontSize + 1;
	   }
	   else	*/
	 /* if (m_MapUseModel.nTime>0)
	   {
		   dwColor=TGetColor("255,255,255");
		   dwBorderColor=TGetColor("0,0,0");
		   if (m_MapUseModel.nTime==1)
			   wt_sprintf(Livename,"(剩余金币:%d个)",Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu());
		   else	if (m_MapUseModel.nTime==2)
			   wt_sprintf(Livename,"(剩余银两:%d两)",Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney());
		   else	if (m_MapUseModel.nTime==3)
			   wt_sprintf(Livename,"(剩余经验:%d点)",Player[CLIENT_PLAYER_INDEX].m_nExp);
		   else	if (m_MapUseModel.nTime==4)
			   wt_sprintf(Livename,"(剩余富源:%d点)",Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue());
		   else	if (m_MapUseModel.nTime==5)
			   wt_sprintf(Livename,"(剩余声望:%d点)",Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue());
		   
		   g_pRepresent->OutputText(nFontSize,Livename, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name)/4-25, nMpsY+150, dwColor, 0, nHeightOff, dwBorderColor);
		   nHeightOffset += nFontSize + 1;
	   }
	   else
	   { 
		    ZeroMemory(Livename,sizeof(Livename));
	   }*/ 

		//------------------------------------------
		if ( m_PifengType>0)	// 披风称号
		{	
			nHeightOffset +=8;
		    if (m_Doing == do_sit && !m_MaskType)
				nHeightOffset +=6;
			else if	(m_MaskType)
				nHeightOffset +=-6;
			
			PaintPifeng(m_PifengType,nMpsX -40- nFontSize * g_StrLen(Name)/4,nMpsY-GetNpcPate()-nHeightOffset,7);	//显示spr称号  nMpsX-30	
		}
		//-------------------------------------------
      //  int bb= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
		char *nPath=NULL;
	 	if(m_btRankFFId > 0 && m_btRankFFId!=88 && m_btRankFFId!=99)	//显示spr称号
		{	
			PaintHonor(m_btRankFFId,nMpsX,nMpsY,6);	         //显示spr称号	
			nPath="\\spr\\skill\\special\\skill_heart.spr";  //红心右旋转
			SetClientSpr(nPath,nMpsX,nMpsY+50,0,8);
		}

		if (m_AutoplayId ==1)
		{
			//this->SetFrameSpr("\\spr\\Ui3\\剑侠助手\\state_autofight.spr",0, 120,GetNpcPate(),100);
			nPath="\\spr\\Ui3\\剑侠助手\\state_autofight.spr";
			SetClientSpr(nPath,nMpsX, nMpsY-GetNpcPate()-nHeightOff*1.8,0,0);
		}
		//转生的光环
		if (nReBorn>0)
		{
			//nPath="\\spr\\skill\\others\\重击.spr";
			//SetClientSpr(nPath,nMpsX - nFontSize * g_StrLen(Name)/4+30, nMpsY-GetNpcPate()+80,0,6);
			//lanyue.spr  zq_sl_003.spr  
			//nPath="\\spr\\skill\\zhongqiu\\zq_sl_003.spr";  \\spr\\skill\\special\\skill_heart.spr
			nPath="\\spr\\skill\\others\\totempole_phoenix.spr"; //凤凰印
			//nPath="\\spr\\skill\\special\\skill_heart.spr";  //红心右旋转
			//nPath="\\spr\\skill\\others\\totempole_mask.spr";
			//nPath="\\spr\\skill\\task\\randomtask.spr"; //感叹号
			SetClientSpr(nPath,nMpsX,nMpsY-GetNpcPate(),0,7);	 //
		}
		//神将的状态 --红月
		/*if (m_nFuMoNum[0].nNpcSetings >0 || m_nFuMoNum[1].nNpcSetings >0||m_nFuMoNum[2].nNpcSetings >0||m_nFuMoNum[3].nNpcSetings >0||m_nFuMoNum[4].nNpcSetings >0)
		{	
			nPath="\\spr\\skill\\zhongqiu\\zq_sl_003.spr";
			SetClientSpr(nPath,nMpsX,nMpsY-GetNpcPate(),0,7);	 //-nHeightOff
		}  */

		if (TongName[0])  //帮派 显示帮派称号
		{
			char szTong[64];
			//dwColor=GetColor("255,255,255");
            //dwBorderColor=GetColor("255,255,0");
			dwBorderColor=TGetColor("0,0,0");

            strcpy(szTong,TongName);

			switch(m_nFigure)  //帮会职位
			{
			case enumTONG_FIGURE_MEMBER://帮众
				{
                if (m_IsWarCity==78)
                {
				   ZeroMemory(szTong,sizeof(szTong));
				   strcpy(szTong,"襄阳·");
		           strcat(szTong,TongName);
				}
				else if (m_IsWarCity==1)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"凤翔·");
		            strcat(szTong,TongName);
				}
				else if (m_IsWarCity==162)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"大理·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==11)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"成都·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==53)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"巴陵·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==176)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"临安·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==80)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"扬州·");
					strcat(szTong,TongName);
				}
				else if (m_IsWarCity==28)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"汴京·");
					strcat(szTong,TongName);
				}
			/*	if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				strcat(szTong,"(帮众)");

				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_MANAGER:  // 队长
				{
				 if (m_IsWarCity==78)
				 {
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"襄阳·");
					strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==1)
				 {
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"凤翔·");
					strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==162)
				 { 
				   ZeroMemory(szTong,sizeof(szTong));
				   strcpy(szTong,"大理·");
				   strcat(szTong,TongName);
				 } 
				 else if(m_IsWarCity==11)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"成都·");
				  strcat(szTong,TongName);
				 } 
				 else if (m_IsWarCity==53)
				 {   
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"巴陵·");
				  strcat(szTong,TongName);
				 } 
				 else if (m_IsWarCity==176)
				 {  
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"临安·");
				  strcat(szTong,TongName);
				 } 
			     else if (m_IsWarCity==80)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"扬州·");
				  strcat(szTong,TongName);
				 }
				 else if (m_IsWarCity==28)
				 { 
				  ZeroMemory(szTong,sizeof(szTong));
				  strcpy(szTong,"汴京·");
				  strcat(szTong,TongName);
				 }
				/*if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				strcat(szTong,"(队长)");

				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_DIRECTOR:  // 长老
				{
					if (m_IsWarCity==78)
					{
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"襄阳·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==1)
					{
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"凤翔·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==162)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"大理·");
						strcat(szTong,TongName);
					} 
					else if(m_IsWarCity==11)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"成都·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==53)
					{   
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"巴陵·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==176)
					{  
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"临安·");
						strcat(szTong,TongName);
					} 
					else if (m_IsWarCity==80)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"扬州·");
						strcat(szTong,TongName);
					}
					else if (m_IsWarCity==28)
					{ 
						ZeroMemory(szTong,sizeof(szTong));
						strcpy(szTong,"汴京·");
						strcat(szTong,TongName);
					}
				/*if (m_WarFucheng==1)
                    strcat(szTong,"(副城主)");
				else if (m_WarZuoHu==2)
					strcat(szTong,"(左护法)");
				else if (m_WarYouHu==3)
					strcat(szTong,"(右护法)");
				else if (m_WarTaishi==4)
					strcat(szTong,"(太史令)");
				else if (m_WarZhongShu==5)
					strcat(szTong,"(中书令)");
				else if (m_WarShangShu==6)
					strcat(szTong,"(尚书令)");
				else if (m_WarMiShu==7)
					strcat(szTong,"(秘书令)");
				else if (m_WarTaiLe==8)
					strcat(szTong,"(太乐令)");
                else */
				     strcat(szTong,"(长老)");
				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			case enumTONG_FIGURE_MASTER:  // 帮主
				{
                if (m_IsWarCity==78)
				{
					ZeroMemory(szTong,sizeof(szTong));
					strcpy(szTong,"襄阳·");
				 	strcat(szTong,TongName);
					strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==1)
				{ 
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"凤翔·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				} 
                else if (m_IsWarCity==162)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"大理·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if(m_IsWarCity==11)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"成都·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==53)
				{    
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"巴陵·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==176)
				{   
	            ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"临安·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}  
                else if (m_IsWarCity==80)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"扬州·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				} 
                else if (m_IsWarCity==28)
				{  
	             ZeroMemory(szTong,sizeof(szTong));
	             strcpy(szTong,"汴京·");
	             strcat(szTong,TongName);
				 strcat(szTong,"(城主)");
				}
				else
				{
				  strcat(szTong,"(帮主)");
				}
				//dwColor=GetColor("255,255,0");
                //dwBorderColor=GetColor("255,0,0");
				dwBorderColor=TGetColor("0,0,0");
				}
				break;
			default:
				dwColor=TGetColor("255,255,255");
				break;
			}

			if (!m_bIsHideTong)
			{
//			   g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY-24, dwColor, 0, nHeightOff, dwBorderColor);
			   nHeightOffset += nFontSize + 1;
			}
			else
			{
				ZeroMemory(TongName,sizeof(TongName));
			}
		}

		char PlayerRank[32]={0};

		if  (m_CurNpcTitle>0)
		{
			int nVal=0;
			char nClorVal[32]={0};
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			g_PlayerTitle.GetString(m_CurNpcTitle+1,"Color","",nClorVal,sizeof(nClorVal));
			if (strstr(nClorVal,","))
				dwColor=TGetColor(nClorVal);
			else
				dwColor=atoi(nClorVal);
			
			if (dwColor<=0)
				dwColor = 0xffff00ff;
			
			g_PlayerTitle.GetString(m_CurNpcTitle+1,"TitleName","",PlayerRank,sizeof(PlayerRank));
			
			int nYY = nMpsY;

			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;
			
//			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}
		else if  (m_NpcTitle>0)
		{

			int nVal=0;
			char nClorVal[32]={0};
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			g_PlayerTitle.GetString(m_NpcTitle+1,"Color","",nClorVal,sizeof(nClorVal));
			if (strstr(nClorVal,","))
				dwColor=TGetColor(nClorVal);
			else
				dwColor=atoi(nClorVal);
			
			if (dwColor<=0)
				dwColor = 0xffff00ff;

			g_PlayerTitle.GetString(m_NpcTitle+1,"TitleName","",PlayerRank,sizeof(PlayerRank));

			int nYY = nMpsY;
			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;
			//测试调试
			/*char nMsg[16]={0};
			t_sprintf(nMsg,"：%d:%d",m_ChiBangType,m_Camp);
			strcat(PlayerRank,nMsg);*/
		
//			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			/*if(m_NpcTitle>0 && m_btRankFFId <=0)//没有属性称号的时候就显示
			{	
				int nStateID=0;
				g_PlayerTitle.GetInteger(m_NpcTitle+1,"SpeicalGraphic",0,&nStateID);
				if (nStateID>0)
				{
					//int sprMpsX, sprMpsY,sprmMap;
					//GetMpsPos(&sprMpsX, &sprMpsY,&sprmMap);
					
					char nSpePath[128]={0},ncPart[32]={0};
					cTabFile.GetString(nStateID+1,"文件名","",nSpePath,sizeof(nSpePath));
					cTabFile.GetString(nStateID+1,"类型","",ncPart,sizeof(ncPart));
					if (nSpePath)
						m_DataRes.SetClientSpr(nSpePath,nMpsX,nMpsY,0,0,ncPart,nStateID);
				}
				
			}*/ 
	
		}
		else if (m_btRankId>0) //显示老的称号
		{
			//char szRankId[5];
			//itoa(m_btRankId, szRankId, 10); //int 转 char
			g_RankTabSetting.GetString(m_btRankId, "RANKSTR", "", PlayerRank, sizeof(PlayerRank));
			dwColor = 0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			int nYY = nMpsY;

			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;


//			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		} 
		else
		{
			t_sprintf(PlayerRank,"剑侠情缘");
			dwColor = TGetColor("15,249,68");//0xffff00ff;
			dwBorderColor=TGetColor("0,0,0");
			int nYY = nMpsY;
			if (TongName[0])
			{
				nYY -= 50;
			}
			else
				nYY -= 26;

//			g_pRepresent->OutputText(nFontSize, PlayerRank, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(PlayerRank) / 4, nYY, dwColor,0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		if (m_MaskType > 0)	//带面具的时候才显示
		{
			if (m_bRideHorse)  //如何骑马则 绘制窗口
			{
				char* sOut = "\\Spr\\UI3\\Npc\\ActionIcon_RideHorse.spr";//Spr
				SetClientSpr(sOut,nMpsX + (nFontSize * g_StrLen(Name) / 4),nMpsY-nHeightOff*1.8,0,1);
			}
			else if (m_Doing == do_sit)
			{
				char* sOut = "\\Spr\\UI3\\Npc\\ActionIcon_Sit.spr";
				SetClientSpr(sOut,nMpsX + (nFontSize * g_StrLen(Name) / 4),nMpsY-nHeightOff*1.7,0,2);
				
			}
		}
		if (m_BaiTan) //摆摊
		{
			int	nMpsX, nMpsY,nMmap;
			    GetMpsPos(&nMpsX, &nMpsY,&nMmap);
				char nstrName[32]={0};
		        GetstrInfo(STR_SHOP_NAME,nstrName);
			int nWid = nFontSize * g_StrLen(nstrName)/2 + 10;
			int nHei = nFontSize + 12;
			char*sOutm = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－中.spr";
			KRUImage RUIconImageR;
			RUIconImageR.nType = ISI_T_SPR;
			RUIconImageR.Color.Color_b.a = 255;
			RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImageR.uImage = 0;
			RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImageR.szImage, sOutm);
			RUIconImageR.oPosition.nX = nMpsX - nWid / 2;
			RUIconImageR.oPosition.nY = nMpsY - 35;
			RUIconImageR.oPosition.nZ = nHeightOffset;
			RUIconImageR.nFrame = 0;
			
			for (int i = 0; i < nWid;++i)
			{
				RUIconImageR.oPosition.nX = nMpsX - nWid / 2 + i;
//				g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
			}

			if (PlayerRank[0] || TongName[0])
			{
				dwColor = 0x00c3a94e;
				dwBorderColor=TGetColor("0,0,0");

				/*if (TongName[0] && PlayerRank[0]) //显示 摆摊广告  和 是称号 为空的话
					g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 76, dwColor, 0, nHeightOff, dwBorderColor);
				else if (TongName[0]||PlayerRank[0])
					g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 50, dwColor, 0, nHeightOff, dwBorderColor);
			*/
			}
			else
			{
				dwColor = 0x00c3a94e;
				dwBorderColor=TGetColor("0,0,0");
//				g_pRepresent->OutputText(nFontSize, nstrName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nstrName) / 4, nMpsY - 34, dwColor, 0, nHeightOff, dwBorderColor);
			}

			char*   sOut = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－左.spr";
			SetClientSpr(sOut,nMpsX - nWid / 2 - 10,nMpsY+6,nHeightOffset + 23,4);
		            sOut = "\\Spr\\Ui3\\摆摊\\摆摊头顶条－右.spr";
			SetClientSpr(sOut,nMpsX + nWid / 2,nMpsY+6,nHeightOffset + 23,3);

			nHeightOffset += nFontSize + 1;
			//----------------------------------------------------------------------------------------------------------------------------

		}
	}
	else if (m_Kind == kind_dialoger)  //对话NPC
	{ 
        char outname[64]="";           //怪物名称

		if (m_GameliveTime>0)
		{
			dwColor=TGetColor("251,84,4");
            dwBorderColor=TGetColor("0,0,0");

			int nHour=0,nMin=0,nSec=0;
			
			nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时
		  
		    nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
			
			if (nMin>0)
			{
				nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
				
			}
			else
				nSec  = m_GameliveTime-nHour*3600;
			char nstrName[64]={0};
		    GetstrInfo(STR_GUISHU_NAME,nstrName);

			t_sprintf(outname,nstrName);
//			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-57, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			t_sprintf(outname,"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
//			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		   dwColor=TGetColor("255,255,255");
           dwBorderColor=TGetColor("0,0,0");
	       t_sprintf(outname,"%s",Name);//m_NpcSettingIdx
//	       g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-5, dwColor, 0, nHeightOff, dwBorderColor);
	 	   nHeightOffset += nFontSize + 1;
	}
	else if (m_Kind == kind_bird || m_Kind == kind_mouse )  //
	{  

	}
	else if (m_Kind == kind_partner)  //
	{   
		char outname[64]=""; //怪物名称
		if (m_btRankFFId==99) 
		{//宠物  
			dwColor=TGetColor("255,255,255");
			dwBorderColor=TGetColor("0,0,0");	

			if (m_GameliveTime>0)
			{//限时同伴
                int nHour=0,nMin=0,nSec=0;

                //nHour = TakeTrader(m_GameliveTime,3600);  //小时
		        //nMin  = TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分


					if (nMin>0)
					{
                         nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒

					}
					else
                         nSec  = m_GameliveTime-nHour*3600;
			
				
                t_sprintf(outname,"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
//			    g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
		        nHeightOffset += nFontSize + 1;

				char nFileName[128];
				t_sprintf(nFileName,"\\spr\\skill\\others\\助攻王.spr");
				SetClientSpr(nFileName,nMpsX - (nFontSize * g_StrLen(outname)/4+10),nMpsY-nHeightOff*2.7,0,7);

			}

            dwColor=TGetColor("237,157,24");
			dwBorderColor=TGetColor("0,0,0");
			t_sprintf(outname,"%s",Name);
		    strcat(outname,"(宠物)");
		   //strcat(outname,m_GuishuName);  //同伴归属
		   //t_sprintf(outnamea,"[阵营:%d,name:%s,M_dwID:%d,M_Index:%d,PeopleId:%d]",m_Camp);             
//			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		    nHeightOffset += nFontSize + 1;

			//PaintHonor(30,nMpsX,nMpsY,6);	//显示spr称号
			
		}
		else
		{//其他	镖车
			dwColor      = TGetColor("234,289,11");
			dwBorderColor= TGetColor("0,0,0");
			if (m_GameliveTime>0)
			{	
                int nHour=0,nMin=0,nSec=0;

                //nHour = TakeTrader(m_GameliveTime,3600);  //小时
		        //nMin  = TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分

				
				if (nMin>0)
				{
					nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
					
				}
				else
                    nSec  = m_GameliveTime-nHour*3600;

                t_sprintf(outname,"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
//				g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
				nHeightOffset += nFontSize + 1;
			}

			t_sprintf(outname,"%s",Name);
//		    g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		    nHeightOffset += nFontSize + 1;
		}

		   
	}
    else if (bSelect) //是否 
	{//怪物	
		 char *nPath=NULL;
		char outname[64]={0}; //怪物名称
		char outnamea[64]={0}; //怪物名称
		char* series;
		if  (m_IsbeSel)
		{//被选中了
			nPath="\\spr\\skill\\task\\randomtask.spr"; //感叹号
			SetClientSpr(nPath,nMpsX,nMpsY-GetNpcPate(),0,7);//
		}

		if (m_GameliveTime>0)
		{//限时NPC
			    dwColor=TGetColor("255,255,255");
			    int nHour=0,nMin=0,nSec=0;
				nHour = m_GameliveTime/3600;//TakeTrader(m_GameliveTime,3600);  //小时	
				nMin  = (m_GameliveTime-nHour*3600)/60;//TakeTrader(m_GameliveTime-nHour*3600,60); //剩余的秒大于60秒，就计算分
				if (nMin>0)
				{
					nSec  = (m_GameliveTime-nHour*3600-nMin*60);  //秒
					
				}
				else
                    nSec  = m_GameliveTime-nHour*3600;

            t_sprintf(outname,"(存活时间:%d小时%d分%d秒)",nHour,nMin,nSec);
//			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-30, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;

			/*wt_sprintf(outname,"归属:%s",m_GuishuName);
			g_pRepresent->OutputText(nFontSize,outname, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY+100, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;*/

		 }
		 
		  int nType = this->m_cGold.GetGoldType();  //获取是否黄金怪物！0为非黄金
		  int nVal=0;
		      g_NpcSetting.GetInteger(m_NpcSettingIdx+2, "AuraSkillId", 0, &nVal);

		        if (nType == 0 && nVal==0)
					dwColor = 0xffffffff;           //白色名字怪物
				else if (nType ==1) 
					dwColor = 0xff00ff00;	        // 绿怪  爆钱
				else if (nType >=2 && nType <= 4)
					dwColor=TGetColor("100,100,255");//浅蓝100,100,255 深蓝 0,0,160
				else if (nType >=5 && nType <= 11)
                    dwColor=TGetColor("234,189,11"); //GetColor(" 255,255,0");
				else if (nType >=12 && nType <= 16)
					dwColor=TGetColor("255,62,62");	//   深红"210,0,0"
				else 
				{
				   if (nVal>0)
				   {
					  dwColor=TGetColor("234,189,11");
				   }

				   //g_NpcSetting.GetInteger(m_NpcSettingIdx+2, "PasstSkillId", 0, &nVal)
				}

           if (m_ZhuaVal>0)
		   {//被抓捕状态
			   dwColor=TGetColor("255,217,78");
			   dwBorderColor=TGetColor("0,0,0");
               t_sprintf(outname,"%s(被抓捕中..)",Name);
		   }
		   else
		   {
                 t_sprintf(outname,"%s",Name);
		   }
           
           if (m_FreezeState.nTime || m_PoisonState.nTime/*m_PoisonState.nTime*/ || m_ConfuseState.nTime || m_StunState.nTime || m_randmove.nTime)
           {
		       strcat(outname, "(");
			if (m_FreezeState.nTime)
				strcat(outname, "冰");
			//if (m_PoisonState.nTime)
			if  (m_PoisonState.nTime)
			   strcat(outname, "毒");
			if (m_ConfuseState.nTime)
				strcat(outname, "乱");
			if (m_StunState.nTime)
				strcat(outname, "晕");
			if (m_randmove.nTime)
			    strcat(outname, "恐");
			strcat(outname, ")");
		   }

		      t_sprintf(outnamea,"%s,Lv:%d",outname,m_Level);  // m_NpcSettingIdx      
//		   g_pRepresent->OutputText(nFontSize,outnamea, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(outname)/4, nMpsY-6, dwColor, 0, nHeightOff, dwBorderColor);
		   nHeightOffset += nFontSize + 1;
		  

		switch(m_Series) 
		{
		case 0:
			series= "\\Spr\\Ui3\\Npc\\series0.spr";
			break;
		case 1:
			series= "\\Spr\\Ui3\\Npc\\series1.spr";
			break;
		case 2:
			series= "\\Spr\\Ui3\\Npc\\series2.spr";
			break;
		case 3:
			series= "\\Spr\\Ui3\\Npc\\series3.spr";
			break;
		case 4:
			series= "\\Spr\\Ui3\\Npc\\series4.spr";
			break;
		}

		if  (m_Series >= 0  && m_Series<= 4)
		   SetClientSpr(series,nMpsX - (nFontSize * g_StrLen(outname)/4+20),nMpsY-nHeightOff*1.8+1,0,5);
		 
/*
		KRUImage RUIconImage;
		RUIconImage.nType = ISI_T_SPR;
		RUIconImage.Color.Color_b.a = 255;
		RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		RUIconImage.uImage = 0;
		RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
		RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		strcpy(RUIconImage.szImage, series);
		RUIconImage.oPosition.nX = nMpsX - (nFontSize * g_StrLen(outname)/4+20); //图片的位置
		RUIconImage.oPosition.nY = nMpsY-nHeightOff*1.75;
	//	RUIconImage.oPosition.nY = nMpsY;
		RUIconImage.oPosition.nZ = 0;
		RUIconImage.nFrame = 0;
		g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);
*/

	  
		dwColor=TGetColor("255,255,255");
		dwBorderColor=TGetColor("0,0,0");
		ZeroMemory(outnamea,sizeof(outnamea));

		if (!m_bIsHideLife && !Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideLife)
		{ 
		   t_sprintf(outnamea,"%d/%d",m_CurrentLife,m_CurrentLifeMax);
		} 
		else
		  ZeroMemory(outnamea,sizeof(outnamea));

//		g_pRepresent->OutputText(nFontSize,outnamea,KRF_ZERO_END,nMpsX-nFontSize*g_StrLen(outnamea)/4, nMpsY+30, dwColor, 0, nHeightOff, dwBorderColor);
		nHeightOffset += nFontSize + 1;
	 
	}
/*	
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char szNameID[50];
		t_sprintf(szNameID,"[%d]", m_dwID);
		g_pRepresent->OutputText(nFontSize, szNameID, KRF_ZERO_END, nMpsX, nMpsY + 20, 0xfff0fff0, 0, m_Height);
	}
	
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index && Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char	szMsg[256];
		int nCount[9];
		for (int i = 0; i < 9; ++i)
			nCount[i] = 0;
		if (LEFTUPREGIONIDX >= 0)
			nCount[0] = LEFTUPREGION.m_NpcList.GetNodeCount();
		if (UPREGIONIDX >= 0)
			nCount[1] = UPREGION.m_NpcList.GetNodeCount();
		if (RIGHTUPREGIONIDX >= 0)
			nCount[2] = RIGHTUPREGION.m_NpcList.GetNodeCount();
		if (LEFTREGIONIDX >= 0)
			nCount[3] = LEFTREGION.m_NpcList.GetNodeCount();
		if (m_RegionIndex >= 0)
			nCount[4] = CURREGION.m_NpcList.GetNodeCount();
		if (RIGHTREGIONIDX >= 0)
			nCount[5] = RIGHTREGION.m_NpcList.GetNodeCount();
		if (LEFTDOWNREGIONIDX >= 0)
			nCount[6] = LEFTDOWNREGION.m_NpcList.GetNodeCount();
		if (DOWNREGIONIDX >= 0)
			nCount[7] = DOWNREGION.m_NpcList.GetNodeCount();
		if (RIGHTDOWNREGIONIDX >= 0)
			nCount[8] = RIGHTDOWNREGION.m_NpcList.GetNodeCount();
		
		int nPosX, nPosY;
		GetMpsPos(&nPosX, &nPosY);
		t_sprintf(szMsg,
			"NpcID:%d  Life:%d\nRegionIndex:%d Pos:%d,%d\nPlayerNumber:%d\n"
			"NpcNumber:\n%02d,%02d,%02d\n%02d,%02d,%02d\n%02d,%02d,%02d",
			m_dwID,
			m_CurrentLife,			
			m_RegionIndex,
			m_MapX,
			m_MapY,
			CURREGION.m_PlayerList.GetNodeCount(),
			nCount[0], nCount[1], nCount[2],
			nCount[3], nCount[4], nCount[5],
			nCount[6], nCount[7], nCount[8]			
			);
		
		g_pRepresent->OutputText(14, szMsg, -1, 320, 40, 0xffffffff);

	}
#endif
	*/	   
	

	return nHeightOffset;
}

void KNpc::SetClientSpr(char *nSprPath,int nxLeft,int nyTop,int nzPos,int i,char *ncPart)
{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImage.bRenderFlag =RUIMAGE_RENDER_FLAG_REF_SPOT;

				strcpy(RUIconImage.szImage, nSprPath);  //改变坐姿

				if (ncPart)
				{
					if (strstr(ncPart,"Head")) //头部
					{
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop-10;
					}
					else if (strstr(ncPart,"Body"))	//中部
					{
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop;
					}
					else
					{//脚步
						RUIconImage.oPosition.nX = nxLeft;
						RUIconImage.oPosition.nY = nyTop;
					}
				    
				}
				else
				{
					RUIconImage.oPosition.nX = nxLeft;
					RUIconImage.oPosition.nY = nyTop;
				}
				RUIconImage.oPosition.nZ = nzPos; 

				/*
				KRUImage RUIconImageR;
				RUIconImageR.nType = ISI_T_SPR;
				RUIconImageR.Color.Color_b.a = 255;
				RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImageR.uImage = 0;
				RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImageR.bRenderFlag = 0;
				strcpy(RUIconImageR.szImage,TaskBg);  //高亮图
				RUIconImageR.oPosition.nX = nYx;//nMpsX - nWid / 2;
				RUIconImageR.oPosition.nY = nYy;//nMpsY - 35;
				RUIconImageR.oPosition.nZ = 0;
				RUIconImageR.nFrame = 0;
				g_pRepresentShell->DrawPrimitives(1,&RUIconImageR, RU_T_IMAGE,TRUE);

				*/
				KImageParam	 imgParam; 
				imgParam.nNumFrames = 0;
    			m_ImageStore.GetImageParam(nSprPath,ISI_T_SPR,&imgParam);
				
				if (imgParam.nNumFrames > 0 && m_DurFrame[i] < imgParam.nNumFrames)
				{			
					IR_NextFrame(m_DurFrame[i], imgParam.nNumFrames,imgParam.nInterval,uFlipTime[i]);
					RUIconImage.nFrame =m_DurFrame[i];
				}
				else
				{
					m_DurFrame[i]=0;
					RUIconImage.nFrame =0;		
				}	
//				if (g_GameWorld)
//				g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);	 //RU_T_IMAGE
}

//--------------------------------------------------------------------------
//	功能：把字符串表示的颜色信息转为数值表示
//--------------------------------------------------------------------------
/*unsigned int KNpc::GetColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;
	
	unsigned int Color = 0xFF000000;  //黑色
	
	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}
//转换成字符串 颜色
const char* KNpc::GetColorString(unsigned int nColor)
{
	static char szColor[12];
	KRColor c;
	c.Color_dw = nColor;
	t_sprintf(szColor, "%d,%d,%d", c.Color_b.r, c.Color_b.g, c.Color_b.b);
	szColor[11] = 0;
	
	return szColor;
}
*/
//是玩家头顶信息显现
int	KNpc::PaintChat(int nHeightOffset)
{
	//if (m_Kind != kind_player)  //是玩家头顶才显现
	//	return nHeightOffset;
	/*
	if (m_nChatContentLen <= 0)
		return nHeightOffset;
	if (m_nChatNumLine <= 0)
		return nHeightOffset;

	int nFontSize = 12;
	int					nWidth, nHeight;
	int					nMpsX, nMpsY,nMap;
	KRUShadow			sShadow;
	//KOutputTextParam	sParam;
	sParam.BorderColor = 0XFF000000; //字体边缘

	sParam.nNumLine = m_nChatNumLine;

	nWidth = m_nChatFontWidth * nFontSize / 2;//
	nHeight = sParam.nNumLine * (nFontSize + 1);

	nWidth += 10;	//为了好看
	nHeight += 5;	//为了好看


	GetMpsPos(&nMpsX, &nMpsY,&nMap);
	sParam.nX = nMpsX - nWidth / 2;
	sParam.nY = nMpsY;
	sParam.nZ = nHeightOffset + nHeight;
	sParam.Color = SHOW_CHAT_COLOR;
	sParam.nSkipLine = 0;
	sParam.nVertAlign = 0;

	sShadow.oPosition.nX = sParam.nX;
	sShadow.oPosition.nX -= 3;	//为了好看
	sShadow.oPosition.nY = sParam.nY;
	sShadow.oPosition.nZ = sParam.nZ;
	sShadow.oEndPos.nX = sParam.nX + nWidth;
	sShadow.oEndPos.nX += 2;	//为了好看
	sShadow.oEndPos.nY = sParam.nY;
	sShadow.oEndPos.nZ = sParam.nZ - nHeight;
	//sShadow.Color.Color_dw = 0x00FFFF00;
	sShadow.Color.Color_dw =0x14000000; // 0XFF000000
	sParam.bPicPackInSingleLine = true;

	//g_pRepresent->OutputRichText(nFontSize, &sParam, m_szChatBuffer, m_nChatContentLen, nWidth);

	return sParam.nZ;*/
	return 0;
}

#include "engine/Text.h"
//自己聊天头顶显示
int	KNpc::SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength)
{
	int nFontSize = 12;
	int nMaxStrLen;

    if (m_Kind == kind_player)
       nMaxStrLen = 32;	// 显示的长度
    else
       nMaxStrLen = 64;	// 显示的长度

	char szChatBuffer[MAX_SENTENCE_LENGTH];

	memset(szChatBuffer, 0, sizeof(szChatBuffer));
  //  m_nChatContentLen = TEncodeText(szChatBuffer, strlen(szChatBuffer));
	
	if (nMsgLength)
	{
		int nOffset = 0;
		if (pMsgBuff[0] != KTC_TAB) //空格
		{
			szChatBuffer[nOffset] = (char)KTC_COLOR;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;	  //三个颜色
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0x00;
			nOffset++;
			strncpy(szChatBuffer + nOffset, Name, 32);
			nOffset += strlen(Name);
			szChatBuffer[nOffset] = ':';
			nOffset++;
			szChatBuffer[nOffset] = (char)KTC_COLOR_RESTORE;
			nOffset++;
		}
		else
		{
			pMsgBuff ++;
			nMsgLength --;
		}

		if (nMsgLength)
		{
			memcpy(szChatBuffer + nOffset, pMsgBuff, nMsgLength); //增加内容
			nOffset += nMsgLength;

			memset(m_szChatBuffer, 0, sizeof(m_szChatBuffer));
           // TEncodeText(szChatBuffer, strlen(szChatBuffer));
			m_nChatContentLen = MAX_SENTENCE_LENGTH; //头顶信息显示


			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, nMaxStrLen,m_szChatBuffer, m_nChatContentLen, 2, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, nMaxStrLen, m_nChatFontWidth,nFontSize, 0, 0, true);//nFontSize
			//SetstrInfo(STR_CHATBUFF_CLIENT,m_szChatBuffer);

			if (m_nChatNumLine >= 2)
				m_nChatNumLine = 2;
			m_nCurChatTime = IR_GetCurrentTime();

			return true;
		}
	}
	return false;
}
//绘制血条
int	KNpc::PaintLife(int nHeightOffset, bool bSelect)
{
	if (!bSelect && (m_Kind != kind_player && m_Kind != kind_partner))
		return nHeightOffset;

	if (m_Kind == kind_bird || m_Kind == kind_mouse )  //
	{  
		return nHeightOffset;
	}

	if (m_CurrentLifeMax <= 0)
		return nHeightOffset;

	if (m_Hide.nTime > 0 && relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
		return nHeightOffset; //隐藏和敌对关系

	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player /*|| m_Kind == kind_partner*/) && m_nPKFlag != 2)
		return nHeightOffset;		//有敌对关系的玩家不显示生命

	int	nMpsX, nMpsY,nMmap;
	GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	int nWid = SHOW_LIFE_WIDTH;
	int nHei = SHOW_LIFE_HEIGHT;
	KRUShadow	Blood;
	//DWORD nX = m_CurrentLife*100/m_CurrentLifeMax; //血条范围颜色显示
	int nCols=0;
	if (m_CurrentLifeMax>=300)
	   nCols=m_CurrentLifeMax/100;	                //分做100份
	else
	{
	   nCols=m_CurrentLife*100/m_CurrentLifeMax;
	}

	int nCurCols=0;
	if (nCols>0)
	   nCurCols=m_CurrentLife/nCols;                //当前的占用了多少份

	if (m_CurrentLifeMax<300)
			nCurCols= nCols;


	if (nCurCols >= 50)
	{
		Blood.Color.Color_b.r = 0;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else if (nCurCols >= 25)
	{
		//Blood.Color.Color_b.r = 255;
		//Blood.Color.Color_b.g = 255;
		//Blood.Color.Color_b.b = 0;
		Blood.Color.Color_b.r = 255;//45;
		Blood.Color.Color_b.g = 217;//45;
		Blood.Color.Color_b.b = 78;//255;

	}
	else
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}

	if (m_nPKFlag == 2)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 204;
	}

	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nCurCols / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
//	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 128;
	Blood.Color.Color_b.g = 128;
	Blood.Color.Color_b.b = 128;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
//	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	if (m_Kind == kind_player)
	   nHei = nHei - 2;

	return nHeightOffset + nHei;
}
//绘制内力条
int	KNpc::PaintMana(int nHeightOffset)
{
	if (m_Kind != kind_player && m_Kind != kind_partner)
		return nHeightOffset;

	if (m_CurrentManaMax <= 0)
		return nHeightOffset;

     if (m_Hide.nTime > 0 && relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
		return nHeightOffset; //隐藏和敌对关系

	 if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_nPKFlag != 2)
		return nHeightOffset;		//有敌对关系的玩家不显示内力

	int	nMpsX, nMpsY,nMmap;
	GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	int nWid = 38;
	int nHei = 5;
	KRUShadow	Blood;
	DWORD nX = m_CurrentMana*100 /m_CurrentManaMax ;  //255,217,78
	if (nX >= 50)
	{
		Blood.Color.Color_b.r = 10;
		Blood.Color.Color_b.g = 10;
		Blood.Color.Color_b.b = 255;
	}
	else if (nX >= 25)
	{
		Blood.Color.Color_b.r = 255;//45;
		Blood.Color.Color_b.g = 217;//45;
		Blood.Color.Color_b.b = 78;//255;
	}
	else
	{
		Blood.Color.Color_b.r = 100;
		Blood.Color.Color_b.g = 100;
		Blood.Color.Color_b.b = 255;
	}

	if (m_nPKFlag == 2)
	{
		Blood.Color.Color_b.r = 45;
		Blood.Color.Color_b.g = 45;
		Blood.Color.Color_b.b = 255;
	}

	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
//	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 128;
	Blood.Color.Color_b.g = 128;
	Blood.Color.Color_b.b = 128;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
//	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	
	return nHeightOffset + nHei;
}

void KNpc::Paint()  // 绘画人物和NPC
{

	if (/*relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&*/
		m_Index!=Player[CLIENT_PLAYER_INDEX].m_nIndex &&
		(m_Kind == kind_player || m_Kind == kind_partner) && m_Hide.nTime > 0)
	{
		if (g_GameWorld)
			g_GameWorld->MoveObject(OBJ_NODE_NPC,m_Index,true);  //

		return;
	}

	if (m_ResDir != m_Dir)
	{
		int nDirOff = m_Dir - m_ResDir;
		if (nDirOff > 32)
			nDirOff -= 64;
		else if (nDirOff < - 32)
			nDirOff += 64;
		m_ResDir += nDirOff / 2;
		if (m_ResDir >= 64)
			m_ResDir -= 64;
		if (m_ResDir < 0)
			m_ResDir += 64;
	}
	
	int nHeight = GetNpcPate() + GetNpcPatePeopleInfo();
	if (m_btRankId || TongName[0])
		nHeight += 10;


	DrawMenuState(nHeight);		 //描绘头顶 交易 睡眠 交易 等SPR
	
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHidePlayer)  //是否隐藏NPC   和玩家
    {
    	if (m_Kind == kind_player && !IsPlayer())
		{	
			if (g_GameWorld)
				g_GameWorld->MoveObject(OBJ_NODE_NPC,m_Index,true);
			return;			 
		}
    }
	
  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideNpc)  // 如果隐藏NPC
  {
	if (m_Kind == kind_bird || m_Kind == kind_mouse || m_Kind == kind_normal)   // 动物 和 对话者
	{
		if (g_GameWorld)
			g_GameWorld->MoveObject(OBJ_NODE_NPC,m_Index,true);
	     return;
	}
  }
   m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame); //客户端绘画人物外观
}

//--------------------------------------------------------------------------
//	功能：增加基本最大生命点
//--------------------------------------------------------------------------
void	KNpc::AddBaseLifeMax(int nLife)
{
	m_LifeMax += nLife;
	m_CurrentLifeMax = m_LifeMax;
}
//--------------------------------------//
void	KNpc::SetBaseLifeMax(int nLifeMax) 
{
	m_LifeMax = nLifeMax;
	m_CurrentLifeMax = m_LifeMax;
}
//------------------------------------//
//--------------------------------------------------------------------------
//	功能：增加当前最大生命点
//--------------------------------------------------------------------------
void	KNpc::AddCurLifeMax(int nLife)
{
	m_CurrentLifeMax += nLife;
}

//--------------------------------------------------------------------------
//	功能：增加基本最大体力点
//--------------------------------------------------------------------------
void	KNpc::AddBaseStaminaMax(int nStamina)
{
	m_StaminaMax += nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}
//------------------------------------//
void	KNpc::SetBaseStaminaMax(int nStamina)
{
	m_StaminaMax = nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}
//------------------------------------//

//--------------------------------------------------------------------------
//	功能：增加当前最大体力点
//--------------------------------------------------------------------------
void	KNpc::AddCurStaminaMax(int nStamina)
{
	m_CurrentStaminaMax += nStamina;
}

//--------------------------------------------------------------------------
//	功能：增加基本最大内力点
//--------------------------------------------------------------------------
void	KNpc::AddBaseManaMax(int nMana)
{
	m_ManaMax += nMana;
	m_CurrentManaMax = m_ManaMax;
}
//---------------------------------------//
void	KNpc::SetBaseManaMax(int nMana)
{
	m_ManaMax = nMana;
	m_CurrentManaMax = m_ManaMax;
}
//---------------------------------------//
//--------------------------------------------------------------------------
//	功能：增加当前最大内力点
//--------------------------------------------------------------------------
void	KNpc::AddCurManaMax(int nMana)
{
	m_CurrentManaMax += nMana;
}

/*
//--------------------------------------------------------------------------
//	功能：重新计算生命回复速度
//--------------------------------------------------------------------------
void	KNpc::ResetLifeReplenish()
{
	m_LifeReplenish = (m_Level + 5) / 6;
	m_CurrentLifeReplenish = m_LifeReplenish;
}

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大生命点
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeMax()
{
}
*/

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大体力点
//--------------------------------------------------------------------------
void	KNpc::CalcCurStaminaMax()
{
	m_CurrentStaminaMax = m_StaminaMax;		// 还需要加上 装备、技能、药物（临时）等的影响
}
*/

/*
//--------------------------------------------------------------------------
//	功能：计算当前最大内力点
//--------------------------------------------------------------------------
void	KNpc::CalcCurManaMax()
{
	m_CurrentManaMax = m_ManaMax;			// 还需要加上 装备、技能、药物（临时）等的影响
}
*/

//--------------------------------------------------------------------------
//	功能：计算当前生命回复速度
//--------------------------------------------------------------------------
void	KNpc::CalcCurLifeReplenish()
{
	m_CurrentLifeReplenish = m_LifeReplenish;	// 与角色系别、角色等级和是否使用药剂、技能和魔法装备有关
}

void	KNpc::Remove()
{
/*	m_LoopFrames = 0;
	m_Index = 0;
	m_PlayerIdx = -1;
	m_Kind = 0;
	m_dwID = 0;
	Name[0] = 0;*/
	m_DataRes.Remove(m_Index);  //残影外观删除
	Init();
}

void	KNpc::RemoveRes()
{
	m_DataRes.Remove(m_Index);//残影外观删除  m_Index
}

//客户端显示公告消息
#ifndef	_SERVER
void	KNpc::ClientShowMsg(const char *Msg)
{
	//strCoreInfo[MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE].c_str()
	KSystemMessage	sMsg;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.eType = SMT_NORMAL;
	sMsg.byParamSize = 0;
	sMsg.byPriority = 0;	

	if (sizeof(Msg)<125)
	{
		t_sprintf(sMsg.szMessage,Msg);
	}
	else
	{
		t_sprintf(sMsg.szMessage,"警告:<colro=gyellow>信息过长,取消发送<color>");
	}
	//t_sprintf(sMsg.szMessage,Msg);
	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif
//--------------------------------------------------------------------------
//	功能：设定此 npc 的五行属性（内容还没完成）not end
//--------------------------------------------------------------------------
void	KNpc::SetSeries(int nSeries)
{
	if (nSeries >= series_metal && nSeries < series_num)
		m_Series = nSeries;
	else
		m_Series = series_metal; //金系？
}


void	KNpc::SetBaseDamage(int nMin,int nMax,int nType)
{
  if (nType==0)
  {//直接设置
   	   m_PhysicsDamage.nValue[0]              = nMin;
       m_PhysicsDamage.nValue[2]              = nMax;
  }
  else 
  {//倍率
       m_PhysicsDamage.nValue[0]              *= nMin;
       m_PhysicsDamage.nValue[2]              *= nMax;
  }

}

//改变当前MPC的属性
void	KNpc::SetFangAndHai(KChanelBaseInfo *nNpcInfo)
{

	m_PhysicsDamage.nValue[0]              = nNpcInfo->PhysicsDamage/5;
    m_PhysicsDamage.nValue[2]              = nNpcInfo->PhysicsDamage;

	m_CurrentPhysicsMagicDamageV.nValue[0] = nNpcInfo->m_PhysicsDamage/5;
	m_CurrentPhysicsMagicDamageV.nValue[2] = nNpcInfo->m_PhysicsDamage;

	m_CurrentMagicFireDamage.nValue[0]     = nNpcInfo->m_FireDamage/5;
    m_CurrentMagicFireDamage.nValue[2]     = nNpcInfo->m_FireDamage;

	m_CurrentMagicColdDamage.nValue[0]     = nNpcInfo->m_ColdDamage/5;
    m_CurrentMagicColdDamage.nValue[2]     = nNpcInfo->m_ColdDamage;

	m_CurrentMagicLightDamage.nValue[0]    = nNpcInfo->m_LightDamage/5;
    m_CurrentMagicLightDamage.nValue[2]    = nNpcInfo->m_LightDamage;

	m_CurrentMagicPoisonDamage.nValue[0]   = nNpcInfo->m_PoisonDamage/5;
    m_CurrentMagicPoisonDamage.nValue[2]   = nNpcInfo->m_PoisonDamage;

	m_CurrentFireResist    = nNpcInfo->m_FireResist;	         // Npc的当前火抗性
	m_CurrentColdResist    = nNpcInfo->m_ColdResist;	         // Npc的当前冰抗性
	m_CurrentPoisonResist  = nNpcInfo->m_PoisonResist;	         // Npc的当前毒抗性
	m_CurrentLightResist   = nNpcInfo->m_LightResist;	         // Npc的当前电抗性
	m_CurrentPhysicsResist = nNpcInfo->m_PhysicsResist;	         // Npc的当前物理抗性
/*
	int		m_FireDamage;	           // Npc的当前火伤害
	int		m_ColdDamage;	           // Npc的当前冰伤害
	int		m_LightDamage;	           // Npc的当前电伤害
	int		m_PoisonDamage;	           // Npc的当前毒伤害
*/

}

//获取当前MPC的属性
void	KNpc::GetFangAndHai(KChanelBaseInfo *nNpcInfo)
{
    nNpcInfo->PhysicsDamage=m_PhysicsDamage.nValue[2];

	nNpcInfo->m_PhysicsDamage=m_PhysicsDamage.nValue[2] + m_CurrentAddPhysicsDamage;//m_MagicPhysicsDamage.nValue[2];

    nNpcInfo->m_FireDamage  =m_CurrentMagicFireDamage.nValue[2];

    nNpcInfo->m_ColdDamage  =m_CurrentMagicColdDamage.nValue[2];

    nNpcInfo->m_LightDamage =m_CurrentMagicLightDamage.nValue[2];

    nNpcInfo->m_PoisonDamage=m_CurrentMagicPoisonDamage.nValue[2];

	nNpcInfo->m_FireResist   =m_CurrentFireResist;	         // Npc的当前火抗性
	nNpcInfo->m_ColdResist   =m_CurrentColdResist;	         // Npc的当前冰抗性
	nNpcInfo->m_PoisonResist =m_CurrentPoisonResist;	     // Npc的当前毒抗性
	nNpcInfo->m_LightResist  =m_CurrentLightResist;	         // Npc的当前电抗性
	nNpcInfo->m_PhysicsResist=m_CurrentPhysicsResist;	     // Npc的当前物理抗性
/*
	int		m_FireDamage;	           // Npc的当前火伤害
	int		m_ColdDamage;	           // Npc的当前冰伤害
	int		m_LightDamage;	           // Npc的当前电伤害
	int		m_PoisonDamage;	           // Npc的当前毒伤害
	int nMinNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[0] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
	int nMaxNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[2] + Npc[m_nIndex].m_CurrentAddPhysicsDamage;
*/
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限   没有状态属性数据的 直接使用同步效果
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetToolNoStateEffect(int nLauncher, int nSkillID, int nLevel,int nTime)
{
	if (nLevel <= 0|| nSkillID <= 0) 
		return ;

	KStateNode* pNode;

	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同

			if (pNode->m_Level == nLevel)
			{//等级相同,不更新数据
					pNode->m_LeftTime  = nTime;
			}
			else
			{//等级不相同的 就更新数据
				for (int i = 0; i < 2; ++i)
				{//nDataNum 为每个技能的属性状态的数量
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType =0;
					pNode->m_State[i].nValue[0]   =0;
					pNode->m_State[i].nValue[1]   =0;
					pNode->m_State[i].nValue[2]   =0;
				}

			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
		return;
	}

	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;
    pNode->m_LeftTime = nTime;

	pNode->m_IsClientState =0; //自己的状态

	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;


	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
	{
		pNode->m_StateGraphics = 0;
	}

	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}
	for (int i = 0; i < 2; ++i)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = 0;
		pNode->m_State[i].nValue[0]   = 0;
		pNode->m_State[i].nValue[1]   = 0;
		pNode->m_State[i].nValue[2]   = 0;
	}

	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetToolStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime/* = -1*/)
{
	if (nLevel <= 0|| nSkillID <= 0) 
		return ;
	//_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;
	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;
	pTemp = (KMagicAttrib *)pData;    //最新这个技能的属性数量

	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同

			if (pNode->m_Level == nLevel)
			{//等级相同,不更新数据
			///	pNode->m_LeftTime = pTemp->nValue[1];      //持续的时间 
				if (nTime)
					pNode->m_LeftTime      = nTime;
				else
					pNode->m_LeftTime      = pTemp->nValue[1];      //持续的时间

			}
			else
			{//等级不相同的 就更新数据
				for (int i = 0; i < nDataNum; ++i)
				{//nDataNum 为每个技能的属性状态的数量
					//清除原技能的影响
					ModifyAttrib(nLauncher, &pNode->m_State[i]);  //清除源数据
					//把新等级下技能的影响计算到NPC身上
					ModifyAttrib(nLauncher, pTemp);
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
					++pTemp;
				}
			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
		return;
	}

	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;

	if (nTime)
		pNode->m_LeftTime      = nTime;
	else
		pNode->m_LeftTime      = pTemp->nValue[1];      //持续的时间

	//pNode->m_LeftTime = nTime;
	pNode->m_IsClientState =0; //自己的状态

	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;


	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
	{
		//g_OrdinSkillsSetting.GetInteger()
		pNode->m_StateGraphics = 0;
	}

	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}

	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; ++i)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		// 调整NPC属性
		ModifyAttrib(nLauncher, pTemp);
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
		++pTemp;
	}

	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点
}

/*!*****************************************************************************
// Function		: KNpc::SetStateSkill（客户端和服务器端同时进行）
// Purpose		: 
// Return		: void 
// Argumant		: int nSkillID
// Argumant		: int nLevel
// Argumant		: void *pData
// Argumant		: int nDataNum
// Argumant		: int nTime -1表示被动技能，时间无限
// Comments		:
// Author		: Spe
*****************************************************************************/
void KNpc::SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime,int nIsEuq)
{
	if (!pData || nDataNum<=0 || nLevel <= 0|| nSkillID <= 0) 
		return ;

	if (nDataNum >= MAX_SKILL_STATE)
		nDataNum = MAX_SKILL_STATE;

	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;
	pNode = (KStateNode *)m_StateSkillList.GetHead(); //第一个节点的 技能
	//KStateNode *mNode =(KStateNode *)m_StateSkillList.GetStatusNode(5);  //第5个节点的技能
	int nMun=0;
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID)
		{//ID相同
/*#ifdef _SERVER
			printf("老开始同步客户端技能(%d),节等级(%d),技等级(%d),时间(%d)\n",nSkillID,pNode->m_Level,nLevel,nTime);
#endif*/
			if (pNode->m_Level == nLevel && nIsEuq==0)
			{//等级相同,不更新数据
				pNode->m_LeftTime = nTime;      //持续的时间   
			}
			else
			{//等级不相同的 就更新数据
				//pNode->m_LeftTime = nTime;      //持续的时间
				pTemp = (KMagicAttrib *)pData;    //最新这个技能的属性数量
				for (int i = 0; i < nDataNum; ++i)
				{//nDataNum 为每个技能的属性状态的数量
					//清除原技能的影响
					ModifyAttrib(nLauncher, &pNode->m_State[i]);  //清除源数据
					//把新等级下技能的影响计算到NPC身上
					ModifyAttrib(nLauncher, pTemp);
                    //更新反值状态保存数据
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
					++pTemp;
					//CCMessageBox("同步状态成功","同步状态成功");
				}
			}
			return;
		}
		nMun++;
		pNode = (KStateNode *)pNode->GetNext();
	}
	// 没有在循环中返回，说明是新技能
	//_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	if (nSkillID >= MAX_SKILL)
	{
		return;
	}

	pNode = new KStateNode;
	pNode->m_SkillID  = nSkillID;
	pNode->m_Level    = nLevel;
	pNode->m_LeftTime = nTime;
	pNode->m_IsClientState =0; //自己的状态

	if (nLevel>=MAX_SKILLLEVEL)
		nLevel = MAX_SKILLLEVEL-1;

	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);  
	if (pOrdinSkill)
        pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();  //获取技能的状态调用的id
	else
		pNode->m_StateGraphics = 0;

	/*if (IsPlayer()&& pOrdinSkill)
	{
		char testInfo[128];
		t_sprintf(testInfo,"收到状态同步:%s,%d,%d",pOrdinSkill->GetSkillName(),nTime,nLevel);
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(testInfo);
	}*/

	if (IsPlayer() && pNode->m_StateGraphics)  
	{//客户端
		m_btStateInfo[m_nNextStatePos] = pNode->m_StateGraphics;
		if ((++m_nNextStatePos) >= MAX_NPC_RECORDER_STATE)
			m_nNextStatePos = 0;
	}
	//CCMessageBox("创建状态成功","创建状态成功");
	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; ++i)  //每个技能的 属性数
	{//等于重新设置了全部状态数据
		// 调整NPC属性
		ModifyAttrib(nLauncher, pTemp);
		//把相反值加入链表中以供移除时使用
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0]   = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1]   = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2]   = -pTemp->nValue[2];
		++pTemp;
	}

	m_StateSkillList.AddTail(pNode);  //将每一个独立的技能（属性状态）数据加入节点
}

//同步客户端使用技能
void KNpc::SysnCastSkillEffect(int nLauncher,int nSkillID,int nParam1,int nParam2,int nLevel, int nTime,int mMaxBei)
{
    if (nLevel <= 0|| nSkillID <= 0) 
		return;
} 
/*!*****************************************************************************
// Function		: KNpc::ModifyMissleCollsion
// Purpose		: 
// Return		: BOOL 
// Argumant		: BOOL bCollsion
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KNpc::ModifyMissleCollsion(BOOL bCollsion)
{
	if (bCollsion)
		return TRUE;

	//if (g_RandPercent(m_CurrentPiercePercent))
	//	return TRUE;
	else
		return FALSE;
}

int KNpc::ModifyMissleLifeTime(int nLifeTime)
{
	if (IsPlayer())
	{
		//return Player[m_PlayerIdx].GetWeapon().GetRange();
		return nLifeTime;
	}
	else
	{
		return nLifeTime;
	}
}

int	KNpc::ModifyMissleSpeed(int nSpeed)
{
	if (m_CurrentSlowMissle)  //子弹速度减慢
	{
		return nSpeed / 2;
	}
	return nSpeed;
}

BOOL KNpc::DoManyAttack()
{
	m_ProcessAI = 0;
	
	KSkill * pSkill =(KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())
	pSkill->m_nEnChance = nEnChance;
	if (pSkill->GetChildSkillNum() <= m_SpecialSkillStep) 		
        goto ExitManyAttack;


    //m_DataRes.SetBlur(TRUE);

	
	m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(m_SpecialSkillStep);
	
	int x, y;
	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
//	m_DesX = x;
//	m_DesY = y;

	if (m_nPlayerIdx > 0)
		pSkill->PlayPreCastSound(m_nSex, x ,y);
	if (g_Random(2))
		m_ClientDoing = cdo_attack;
	else 
		m_ClientDoing = cdo_attack1;

	
	m_Doing = do_manyattack;
	
	m_Frames.nCurrentFrame = 0;

	return TRUE;

ExitManyAttack:

    //m_DataRes.SetBlur(FALSE);
	DoStand();
	m_ProcessAI = 1;
	m_SpecialSkillStep = 0;

	return TRUE;
}

void KNpc::OnManyAttack()
{
	if (WaitForFrame())
	{

		//m_DataRes.SetBlur(FALSE);

		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;

		int nPhySkillId =  pSkill->GetChildSkillId();//GetCurActiveWeaponSkill(); Changed 
		int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);//m_SkillList.FindSame(pSkill->GetSkillId())

		if (nPhySkillId > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
			if (pOrdinSkill)
            {
				pOrdinSkill->m_nEnChance = nEnChance;
				pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
            }
		}
		NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
		m_SpecialSkillStep ++;
		DoManyAttack();

	}	
}

BOOL	KNpc::DoRunAttack()
{
	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		m_Frames.nTotalFrame = m_RunSpeed;
		m_ProcessAI = 0;
		
		//m_DataRes.SetBlur(TRUE);

		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
		
		if (m_DesX < 0 && m_DesY > 0) 
		{
			int x, y;
			SubWorld[m_SubWorldIndex].NewMap2Mps
				(
				Npc[m_DesY].m_RegionIndex,
				Npc[m_DesY].m_MapX, 
				Npc[m_DesY].m_MapY, 
				Npc[m_DesY].m_OffX, 
				Npc[m_DesY].m_OffY, 
				&x,
				&y
				);

		m_DesX = x;
		m_DesY = y;
		}

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;

		int x, y, tx, ty,tMap;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty,&tMap);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		//设置为不能跑步攻击？
		m_Frames.nTotalFrame = 0;//m_AttackFrame * 100 / (100 + m_CurrentAttackSpeed);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 2:
	case 3:
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}

	m_Frames.nCurrentFrame = 0;
		
	return TRUE;

}

void	KNpc::OnRunAttack()
{
				
	if (m_SpecialSkillStep == 0)
	{
		OnRun();
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;
		int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex); //m_SkillList.FindSame(pSkill->GetSkillId())
		pSkill->m_nEnChance = nEnChance;
        if (m_Doing == do_stand || (DWORD)m_nCurrentMeleeTime > pSkill->GetMissleGenerateTime(0)) 
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;

			DoRunAttack();
		
		}
		else
			m_nCurrentMeleeTime ++;

		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill = (KSkill*)GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);
			pSkill->m_nEnChance = nEnChance;
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->m_nEnChance = nEnChance;
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
            NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
		//m_DataRes.SetBlur(FALSE);
	}
	else
	{
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::DoJumpAttack()
{
	m_ProcessAI = 0;
	int naAttackSpeed=100,nbAttackSpeed=100;
	    g_GameSetTing.GetInteger2("SYSTEM","NpcAttackSpeed",&naAttackSpeed,&nbAttackSpeed);

	switch(m_SpecialSkillStep)
	{
	case 0:
		DoJump();

		//m_DataRes.SetBlur(TRUE);
		m_ClientDoing = cdo_jump;
		m_Doing = do_jumpattack;
		break;
		
	case 1:
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;
		int x, y, tx, ty,tMap;
		SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty,&tMap);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
		if  (nbAttackSpeed + m_CurrentAttackSpeed!=0)
			m_Frames.nTotalFrame = m_AttackFrame * naAttackSpeed / (nbAttackSpeed + m_CurrentAttackSpeed);
		else
			m_Frames.nTotalFrame = m_AttackFrame;

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_jumpattack;
		break;
		
	case 2:
	case 3:
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}
	
	m_Frames.nCurrentFrame = 0;
	
	return TRUE;
	
}

BOOL KNpc::OnJumpAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		if (!OnJump())
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;
			DoJumpAttack();
		}
		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		//m_DataRes.SetBlur(FALSE);
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill =(KSkill*) GetActiveSkill();
			if (!pSkill) 
                return FALSE;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			int nEnChance = m_SkillList.GetEnChance(m_ActiveSkListIndex);
			pSkill->m_nEnChance = nEnChance;
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkillB(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->m_nEnChance = nEnChance;
					pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			NpcFuMoCastSkll(m_Index, m_SkillParam1, m_SkillParam2);
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
	}
	else
	{
		//m_DataRes.SetBlur(FALSE);
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
	}
	return TRUE;
}
//命中率(发技能方) 闪避率(被攻击方) 忽略对方的闪避率(发技能方) 忽略对方命中率(被攻击方)	
BOOL KNpc::CheckHitTarget(int nAR, int nDf, int nIngore,int nInHuLieAr)
{
	int nDefense = nDf * (100 - nIngore) / 100;  //敌人的闪避率
	int bRet=0;

    int nCurAr   = nAR - nInHuLieAr;  //忽略对方命中点数

	if  (nCurAr<=0) // 攻击方剩余的命中
	{//如果当前攻击方的命中为 小于 0 或 等于0时,设置默认命中率为 10
	    nCurAr = MIN_HIT_PERCENT;
	}

	if  (nDefense<=0)// (被攻击方的剩余闪避率)
	{//如果敌人的闪避率小于 或 等与0  必中
        bRet = 1;
		return bRet;
	}
	else
	{//否则对比  命中率 与 敌人的 闪避的  随机值
        if (g_Random(nCurAr) >=g_Random(nDefense))
		{//打中了
			bRet = 1;
		}
	}

	return bRet;
}

void KNpc::GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel < 1 ) 
		return ;

	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
	//if (g_pNpcTemplate[nNpcTemplateId][nLevel]) //数据有效则拷贝，否则重新生成
		LoadDataFromTemplate(pNpcTemp);//(nNpcTemplateId, nLevel);

	/*else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0])
		{//大类基本数据,没有等级之分
			g_pNpcTemplate[nNpcTemplateId][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0]->InitNpcBaseData(nNpcTemplateId);  //初始化基本数据和爆率文件
			g_pNpcTemplate[nNpcTemplateId][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0]->m_bHaveLoadedFromTemplate = TRUE;
		}
	

//然后加载脚本等级数据		
KLuaScript * pLevelScript = NULL;		

#ifdef _SERVER //g_pNpcTemplate[nNpcTemplateId][0]
			pLevelScript = (KLuaScript*)g_GetScript(g_pNpcTemplate[nNpcTemplateId][0]->m_dwLevelSettingScript);
		
		if (pLevelScript == NULL)  //如果没有脚本的 就设置默认的脚本
			pLevelScript = g_pNpcLevelScript;   //npcLevelscript.lua
#else
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			    LevelScript.Init();   //初始化 脚本函数
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript))
			{
				//g_DebugLog ("[error]致命错误,无法正确读取%s", g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript);
				//_ASSERT(0);
				pLevelScript = g_pNpcLevelScript;
				LevelScript.Exit();
			}
			else
				pLevelScript = &LevelScript;
		}

#endif
		g_pNpcTemplate[nNpcTemplateId][nLevel] = new KNpcTemplate; //重新生成当前等级下的 NPC数据
		*g_pNpcTemplate[nNpcTemplateId][nLevel] = *g_pNpcTemplate[nNpcTemplateId][0]; //把基本数据赋值
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_nLevel = nLevel; //改变等级
		//加载改等级下的脚本数据 
		g_pNpcTemplate[nNpcTemplateId][nLevel]->InitNpcLevelData(nNpcTemplateId, pLevelScript, nLevel);
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_bHaveLoadedFromTemplate = TRUE;  //设置已经加载了 模板NPC数据
		LoadDataFromTemplate(nNpcTemplateId,nLevel);       //加载更新后的数据基本数据
	} */

}
//从NPC模板导入数据
void	KNpc::LoadDataFromTemplate(VOID * nNpcTemp)//(int nNpcTemplateId, int nLevel)  //从NPC模板导入数据
{
	/*if (nNpcTemplateId < 0 || nNpcTemplateId > MAX_NPCSTYLE-1 || nLevel<0 || nLevel > MAX_NPC_LEVEL-1)
	{
		return ;
	} */
	
	//KNpcTemplate* pNpcTemp = NULL;
	//if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))

	KNpcTemplate * pNpcTemp = (KNpcTemplate *)nNpcTemp;//NpcSet.GetTemplate(nNpcTemplateId, nLevel);//
	//KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel];

	if (!pNpcTemp)
		return;

	strcpy(Name,pNpcTemp->Name);
	m_Kind   = pNpcTemp->m_Kind;
	m_Camp   = pNpcTemp->m_Camp;
	m_CurrentCamp  = pNpcTemp->m_Camp;

	if (pNpcTemp->m_Series<0 && pNpcTemp->m_Series>4)
	   m_Series = GetRandomNumber(0,4);
	else
	   m_Series = (BYTE)pNpcTemp->m_Series;

	m_Level  = pNpcTemp->m_nLevel;

	if  (m_Level<=0)
		m_Level =1;

	m_HeadImage   =	pNpcTemp->m_HeadImage;
	m_bClientOnly = pNpcTemp->m_bClientOnly;   //是否只是客户端的NPC
	m_CorpseSettingIdx = pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame =	pNpcTemp->m_DeathFrame;
	m_WalkFrame  =	pNpcTemp->m_WalkFrame;
	m_RunFrame   =	pNpcTemp->m_RunFrame;
	m_HurtFrame   =	pNpcTemp->m_HurtFrame;
	m_WalkSpeed   = pNpcTemp->m_WalkSpeed;
	m_AttackFrame =	pNpcTemp->m_AttackFrame;  //外攻攻击速度
	m_CastFrame   = pNpcTemp->m_CastFrame;    //内攻攻击速度
	m_RunSpeed =	pNpcTemp->m_RunSpeed;
	m_StandFrame =  pNpcTemp->m_StandFrame;
	m_StandFrame1 = pNpcTemp->m_StandFrame1;
	m_NpcSettingIdx = pNpcTemp->m_NpcSettingIdx;
	m_nStature		= pNpcTemp->m_nStature;

	m_LifeMax				= pNpcTemp->m_nLifeMax;
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
//	m_PifengType		    = pNpcTemp->m_PifengType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
	m_HorseType				= pNpcTemp->m_HorseType;
	m_bRideHorse			= pNpcTemp->m_bRideHorse;
	//strcpy(ActionScript, pNpcTemp->ActionScript);  //行为脚本赋值
	SetstrInfo(STR_ACTION_SCRIPT,pNpcTemp->ActionScript);

	/*for (int j  = 0; j < 4; j ++)
		m_SkillList.Add()
	int		m_nSkillID[4];
	int		m_nSkillLevel[4];*/

	//BOOL	m_bHaveLoadedFromTemplate;
	//int		m_nNpcTemplateId;
	
	RestoreNpcBaseInfo();  //设置NPC基本信息
	
}

//-----------------------------------------------------------------------
//	功能：设定物理攻击的最大最小值 not end 需要考虑调用的地方
//-----------------------------------------------------------------------
void	KNpc::SetPhysicsDamage(int nMinDamage, int nMaxDamage)
{
	m_PhysicsDamage.nValue[0] = nMinDamage;
	m_PhysicsDamage.nValue[2] = nMaxDamage;
}

//-----------------------------------------------------------------------
//	功能：设定攻击命中率
//-----------------------------------------------------------------------
void	KNpc::SetBaseAttackRating(int nAttackRating)
{
	m_AttackRating = nAttackRating;
	// 此处还需要加上装备、技能的影响，计算出当前值
	m_CurrentAttackRating += m_AttackRating;
}

//-----------------------------------------------------------------------
//	功能：设定防御力
//-----------------------------------------------------------------------
void	KNpc::SetBaseDefence(int nDefence)
{
	m_Defend = nDefence;
	// 此处还需要加上装备、技能的影响，计算出当前值
	m_CurrentDefend = m_Defend;
}

/*
//-----------------------------------------------------------------------
//	功能：设定行走速度
//-----------------------------------------------------------------------
void	KNpc::SetBaseWalkSpeed(int nSpeed)
{
	m_WalkSpeed = nSpeed;
	// 此处还需要加上装备、技能的影响，计算出当前值 (not end)
	m_CurrentWalkSpeed = m_WalkSpeed;
}
*/

/*
//-----------------------------------------------------------------------
//	功能：设定跑步速度
//-----------------------------------------------------------------------
void	KNpc::SetBaseRunSpeed(int nSpeed)
{
	m_RunSpeed = nSpeed;
	// 此处还需要加上装备、技能的影响，计算出当前值 (not end)
	m_CurrentRunSpeed = m_RunSpeed;
}
*/

#ifdef _SERVER

void KNpc::ScriptDropItem(int nBelongPlayer,int nLastDamageIdx,int nItemNum,int nSeries)
{
	if (nBelongPlayer > 0 && m_pDropRate) // 是否有爆率  是否有 攻击者 并且 被攻击者 有爆率在身
	{
	
		int nNum=0,nLuckyRate=0;
		int nLuckBeiShu = Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;    //活动幸运倍数

		int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);

		if (nGoldLuck<=0)
            nGoldLuck=1;

        int nLuck       = Player[nBelongPlayer].m_nCurLucky*nLuckBeiShu*nGoldLuck;  //攻击者的幸运值
		//int nSysZcount  = 2;
		//    nSysZcount  = m_pDropRate->nCount;   //总数量
        int nSysBcount  = 0;
		    nSysBcount  = m_pDropRate->nTypeNum;//GetItemParm(ITEM_PARM_TYPENUM); //要掉下的数量

		g_GameSetTing.GetInteger("ServerConfig","LuckyRate",100,&nLuckyRate);

		 nNum=nSysBcount;

		 if (nItemNum >0)
			 nNum = nItemNum;

       //钱只掉一次	

		 /*
		 {
		 int		nGenre;
		 int		nDetailType;
		 int		nParticulType;
		 int		nRate;              //爆率
		 int     nQuality;           //是否黄金
		 int 	nMinItemLevel;      //物品的最小等级
		 int     nMaxItemLevel;      //物品的最大等级
		 int     nHour;              //限时
		 int     nIsBang;            //是否绑定
		 int     nIsNoBian;          //是否是辨识
		 int     nStackNum;          //叠加的数量
		 //int     nDropType;        //类型
		 };  //物品部分
		 
		   int			nCount;
		   int			nMagicRate;
		   int			nMaxRandRate;
		   int			nMoneyRate;
		   int         nMoneyNum;
		   int			nMoneyScale;
		   int			nMinItemLevelScale;   //最小等级概率
		   int			nMaxItemLevelScale;
		   int		    nMinItemLevel;
		   int		    nMaxItemLevel;
		   int         nTypeNum;             //掉下的数量
    int         nIsBianShi;           //是否可以爆 可辨识装备
			 
		*/

	   g_GameSetTing.GetInteger("ServerConfig","GoldMoneyRate",100,&nSysBcount);

	   if (g_RandPercent(nSysBcount) && g_RandPercent(m_pDropRate->nMoneyRate))//GetItemParm(ITEM_PARM_MONEYRATE)))         //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
	   {  
		   LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//GetItemParm(ITEM_PARM_MONEYNUM));  //钱
	   } 
//----------------------------------------------------------时间段设置
	   	int nIsCloseAllItem=0,nIsBreak=0;

        if (/*nBelongPlayer>0 && */Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId>0)
		{//挂机的
			int nIsCloseGuaItem=0,nTimeStart=0,nTimeEnd=9,nIsOpenFullForbit=0;
			g_GameSetTing.GetInteger("ServerConfig","IsOpenFullForbit",0,&nIsOpenFullForbit);

			if (nIsOpenFullForbit>=1 && Player[nBelongPlayer].m_nRoomFull==1)	  //包袱满了 不产生物品
			{
				nIsBreak=1;
				return;
			}

            g_GameSetTing.GetInteger2("ServerConfig","IsCloseItem",&nIsCloseGuaItem,&nIsCloseAllItem);
			g_GameSetTing.GetInteger2("ServerConfig","TimeCloseItem",&nTimeStart,&nTimeEnd);

			if (nIsCloseAllItem>=1)
			{
                return;
			}

			time_t rawtime;      //定义一个long 型存放秒数
			struct tm * timeinfo;	
			time (&rawtime);     //距离现在的时间（秒）
			timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
			int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;			
			bYear   = timeinfo->tm_year+1900;
			bMonth  = timeinfo->tm_mon+1;
			bDay    = timeinfo->tm_mday;
			bHour   = timeinfo->tm_hour;
	        bMin    = timeinfo->tm_min;
            
            if (nIsCloseGuaItem>=1 && bHour>=nTimeStart && bHour<nTimeEnd)
			{
				nIsBreak=1;
				return;
			}
		}

		/*if (Player[nBelongPlayer].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
		{
			//Player[nBelongPlayer].m_ItemList.msgshow("提示:<color=yollow>物品超限,请清理物品空间<color>!");
			nIsBreak=1;
			return;
		}*/
//----------------------------------------------------------
		//------------------------------------------------新的
		//CONST KItemDropRate::KItemParam* pItemDropParam = NULL;  //物品信息部分

		int  nCurIdx=0;
		for (int i = 0; i < nNum; ++i) 
		{  			
            if (nIsCloseAllItem>=1)
				break;
            if (nIsBreak>=1)
				break;
			//	CONST KItemDropRate::KItemParam* pItemDropParam = NULL;  //物品信息部分
			//pItemDropParam = m_pDropRate->GetRandItem(nLuck); //掉的就是这个物品
			
			//if (!pItemDropParam)
			//   continue;
			char nstrName[64]={0};
                 GetstrInfo(STR_GUISHU_NAME,nstrName);
           if (strstr(nstrName,"系统"))    
		   {//公共boss
			     nCurIdx = nBelongPlayer;
			     LoseSingleItem(nBelongPlayer,NULL,nLuckyRate,nLastDamageIdx,nSeries);   //NPC掉下装备 m_nLastDamageIdx	 
		   }
		   else 
		   {//私人BOSS
			 //  int nNpcidx=NpcSet.SearchGSName(m_GuishuName);
			    int mPlayerIdx = PlayerSet.FindNameID(nstrName);
                if (mPlayerIdx>0)
				{//在线
					nCurIdx = mPlayerIdx;
				    LoseSingleItem(mPlayerIdx,NULL,nLuckyRate,nLastDamageIdx,nSeries);   //NPC掉下装备
				}
				else
				{//不在线
					LoseSingleItem(nBelongPlayer,NULL,nLuckyRate,nLastDamageIdx,nSeries);   //NPC掉下装备
				}
		   }
		}

		if (g_RandPercent(m_pDropRate->nMoneyRate))//GetItemParm(ITEM_PARM_MONEYRATE)))  //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
		{   
			LoseMoney(nCurIdx);  //钱
		}  
	}

}
//  死亡处罚--NPC掉下装备
void KNpc::DeathPunish(int nMode, int nBelongPlayer,int nLastDamageIdx)
{
	#define	LOSE_EXP_SCALE	10

	if (IsPlayer())
	{//玩家死亡
		int nExpMin=0,nExpMax=0,nGoldExpMin=0,nGoldExpMax=0,nSubExp=0,nDeathLevelpro;
				g_GameSetTing.GetInteger2("SYSTEM","DeathExpData",&nExpMin,&nExpMax);
				g_GameSetTing.GetInteger("SYSTEM","DeathLevelpro",10,&nDeathLevelpro);
				g_GameSetTing.GetInteger2("SYSTEM","GoldDeathExpData",&nGoldExpMin,&nGoldExpMax);
		

		if (nMode == DEATH_MODE_NPC_KILL)
		{//被npc kill
			nSubExp=0;
			if (Player[m_nPlayerIdx].m_nExp > 0)
			{//当前的经验
				
				if (m_Level <= 10)
					nSubExp = Player[m_nPlayerIdx].m_nExp/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);	 //PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)
				else
					nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  	 //4分之一
                
				if (nSubExp>0)
				{
			        Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
				}
				else
				{
					if (nSubExp==0)
						Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
					else
						Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
				}

			}
			else
			{//当前经验为零时,调用全局死亡经验
				if (m_Level > nDeathLevelpro)
				    Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
			}

			// 钱减少
			int nMoney = (Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney())/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// 损失金钱消息
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType =s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);//
				sMsg.m_lpBuf = 0;

				//if (TakeTrader(nMoney,2) > 0)
				if ((nMoney/2) > 0)
					PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱
			}
		}
		else if (nMode == DEATH_MODE_PLAYER_NO_PUNISH)
		{//切磋，没有惩罚
			return;
		}
		else if (nMode == DEATH_MODE_EXP_PUNISH)
		{//经验惩罚
			 nSubExp=0;
			 if (Player[m_nPlayerIdx].m_nExp > 0)
			 {//当前的经验

				if (m_Level <= 10)
					nSubExp = (Player[m_nPlayerIdx].m_nExp)/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);
				else
					nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  
                
				if (nSubExp>0)
				{
					Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
				}
				else
				{
					if (nSubExp==0)
						Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
					else
						Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
				}

			 } 
			 else
			 {//当前经验为零时,调用全局死亡经验
				    nSubExp= GetRandomNumber(nExpMin,nExpMax);
					Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
			 }
				if (Npc[nLastDamageIdx].m_Kind==kind_player)
				{//全局惩罚脚本
					int nTarPlayer=0;
					char szNpcIndex[30];

					if (nLastDamageIdx>0)
					{
				        t_sprintf(szNpcIndex, "%d", nLastDamageIdx);
						nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
					} 
					if (nTarPlayer>0)    //是否执行全局脚本
						Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nSubExp);
				}
		}
		else if (nMode == DEATH_MODE_MONEY_PUNISH)
		{//银两惩罚
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// 损失金钱消息
				char msg[64];
				t_sprintf(msg,"提示:您损失了 %d 银两!",nMoney);
				Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
				
				if ((nMoney/2) > 0)
					PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱

			}

			if (Npc[nLastDamageIdx].m_Kind==kind_player)
			{//全局惩罚脚本
				int nTarPlayer=0;
				char szNpcIndex[30];
				if (nLastDamageIdx>0)
				{
					t_sprintf(szNpcIndex, "%d", nLastDamageIdx);
					nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
				} 
				if (nTarPlayer>0)    //是否执行全局脚本
					Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nMoney);
			}
		}
		else if (nMode == DEATH_MODE_EQUIP_PUNISH)
		{//装备惩罚
			int  nLostRate;

            nLostRate=g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky;
			// 丢失物品
			Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(nLostRate);
			
			// 丢失穿在身上的装备
			if (g_Random(100) > g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky)
			{//幸运越多就越难掉下
				Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
			}

		}
		else if (nMode == DEATH_MODE_JINBI_PUNISH)
		{//金币惩罚
			int nMoney = (Player[m_nPlayerIdx].m_ItemList.GetEquipmentXu())/10;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentXu(),10);  //10分之一
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostXu(nMoney);
				// 损失金钱消息
                char msg[64];
				t_sprintf(msg,"提示:您损失了 %d 金币!",nMoney);
				Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
			}

			if (Npc[nLastDamageIdx].m_Kind==kind_player)
			{//全局惩罚脚本
				int nTarPlayer=0;
				char szNpcIndex[30];
				if (nLastDamageIdx>0)
				{
					t_sprintf(szNpcIndex, "%d", nLastDamageIdx);
					nTarPlayer = Npc[nLastDamageIdx].m_nPlayerIdx;  //Npc[m_nLastDamageIdx].m_nPlayerIdx
				} 
				if (nTarPlayer>0)    //是否执行全局脚本
					Player[nTarPlayer].ExecuteScriptC("\\script\\global\\goldpunish.lua", "nGoldPunish", szNpcIndex,Name,m_Index,m_dwID,nMoney);
			}

		}	
		else if (nMode == DEATH_MODE_PKBATTLE_PUNISH)
		{//经验减少 ,掉装备  国战
			nSubExp=0;
			if (Player[m_nPlayerIdx].m_nExp > 0)
			{
				
				if (m_Level <= 10)
					nSubExp = Player[m_nPlayerIdx].m_nExp/50;//TakeTrader(Player[m_nPlayerIdx].m_nExp,50);
				else
					nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(Player[m_nPlayerIdx].m_nExp,25);  
                
				if (nSubExp>0)
					Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
				else
				{
					if (nSubExp==0)
						Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
					 else
					    Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
				}
			}
			else
			{//当前经验为零时,调用全局死亡经验
				if (m_Level > nDeathLevelpro)
				   Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
			}

			// 钱减少
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/2;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),2);  //减少一半的钱
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// 损失金钱消息
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType =s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);//
				sMsg.m_lpBuf = 0;
				
				if (nMoney/2 > 0)
					PlayerDeadCreateMoneyObj(nMoney/2);  //生成1/4 的钱
			}

			int  nLostRate;
                 nLostRate=g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky;
			// 丢失物品
			Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(nLostRate);
			
			// 丢失穿在身上的装备
			if (g_Random(100) > g_Random(100)+Player[m_nPlayerIdx].m_nCurLucky)
			{
				Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
			}
		}
		// PK致死，按PK值计算惩罚 2
		else
		{
			int	nPKValue;
			    nPKValue = Player[this->m_nPlayerIdx].m_cPK.GetPKValue();
			if (nPKValue < 0)
				nPKValue = 0;
			if (nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
				nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

			// 经验减少
			//int	nLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level);  //获取当前等级的经验
			//    Player[m_nPlayerIdx].DirectAddExp(-(nLevelExp/PlayerSet.m_sPKPunishParam[nPKValue].m_nExp));

             nSubExp=0;
			 if (Player[m_nPlayerIdx].m_nExp > 0)
			 {
				if (m_Level <= 10)
					nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level))/50;//TakeTrader(PlayerSet.m_cLevelAdd.GetLevelExp(m_Level),50);
				else
					nSubExp = GetRandomNumber(nGoldExpMin*m_Level,nGoldExpMax*m_Level);//TakeTrader(PlayerSet.m_cLevelAdd.GetLevelExp(m_Level),25);  
                
				if (nSubExp>0)
			        Player[m_nPlayerIdx].DirectAddExp(-nSubExp);
				else
				{
					if (nSubExp==0)
						Player[m_nPlayerIdx].DirectAddExp(-Player[m_nPlayerIdx].m_nExp);
					else
						Player[m_nPlayerIdx].DirectAddExp(nSubExp);//Player[m_nPlayerIdx].m_nExp
				}
			 }
			 else
			 {//当前经验为零时,调用全局死亡经验
				 Player[m_nPlayerIdx].DirectAddExp(-GetRandomNumber(nExpMin,nExpMax));
			 } 

			// 钱减少
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney()/50;//TakeTrader(Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney(),50);

			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);
				// 损失金钱消息
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;
				
				if (nMoney/2 > 0)
					PlayerDeadCreateMoneyObj(nMoney/2);
			}
          int sPKValue=0;
          g_GameSetTing.GetInteger("ServerConfig","PKValue",5,&sPKValue);
			// 丢失物品GetPKValue()
          if (nPKValue>=sPKValue)
          {//PK值大于5才掉下装备
			Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(PlayerSet.m_sPKPunishParam[nPKValue].m_nItem);
			// 丢失穿在身上的装备
			if (g_Random(100) < PlayerSet.m_sPKPunishParam[nPKValue].m_nEquip)  //按照PK值的 惩罚概率
			{
				//Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
			}
          }
            //被人杀死 PK值减少一点
			Player[m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nBeKilledAddPKValue); //增加PK值  m_Currentnopkvalue
			
			if (nLastDamageIdx)
			{
				if (Npc[nLastDamageIdx].IsPlayer())
				{
					KPlayerChat::MakeEnemy(Name,Npc[nLastDamageIdx].Name);  //拉为仇人
				}
			}
		}
	}
//怪物死亡
	else if (nBelongPlayer > 0 && m_pDropRate) // 是否有爆率  是否有 攻击者 并且 被攻击者 有爆率在身
	{
	
		int nNum=0,nLuckyRate=0;

		int nLuckBeiShu = Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;    //活动幸运倍数

		int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);

		if (nGoldLuck<=0)
            nGoldLuck=1;

        int nLuck       = Player[nBelongPlayer].m_nCurLucky*nLuckBeiShu*nGoldLuck;  //攻击者的幸运值
		//int nSysZcount  = 2;
		//    nSysZcount  = m_pDropRate->nCount;   //总数量
        int nSysBcount  = 0;
		   // nSysBcount  = m_pDropRate->nTypeNum; //要掉下的数量

		g_GameSetTing.GetInteger("ServerConfig","LuckyRate",100,&nLuckyRate);

		nNum = m_pDropRate->nTypeNum;//GetItemParm(ITEM_PARM_TYPENUM);

		if (m_CurrentTreasure >0)
		{
		    nNum = m_CurrentTreasure;
		}

		if (m_pDropRate->nCount<=0)
		   return;


      /* if  (nLuck>100)
	   { 
		   if (nLuck>100 && nLuck <=200)
              nNum=nSysBcount+GetRandomNumber(0,3);
			else if (nLuck>200 && nLuck <=500)
              nNum=nSysBcount+GetRandomNumber(1,3);
			else if (nLuck>500 && nLuck <=700)
              nNum=nSysBcount+GetRandomNumber(1,5);
			else if (nLuck>700)
			  nNum=nSysBcount+GetRandomNumber(1,6);
	   } 
       else
	   { 
		if (nSysBcount<=1)  //爆率文件的掉装备数量
          nNum=m_CurrentTreasure;   //模板的 掉装备数量
		else
          nNum=nSysBcount; 
	   } 
	   */
       //钱只掉一次

	   g_GameSetTing.GetInteger("ServerConfig","GoldMoneyRate",100,&nSysBcount);

	   if (g_RandPercent(nSysBcount) && g_RandPercent(m_pDropRate->nMoneyRate))//GetItemParm(ITEM_PARM_MONEYRATE)))         //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
	   {  
		   LoseMoney(nBelongPlayer,m_pDropRate->nMoneyNum);//GetItemParm(ITEM_PARM_MONEYNUM));  //钱
	   } 
//----------------------------------------------------------时间段设置
	   	int nIsCloseAllItem=0,nIsBreak=0;

        if (/*nBelongPlayer>0 && */Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId>0)
		{//挂机的
			int nIsCloseGuaItem=0,nTimeStart=0,nTimeEnd=9,nIsOpenFullForbit=0;
			g_GameSetTing.GetInteger("ServerConfig","IsOpenFullForbit",0,&nIsOpenFullForbit);

			 /*char mag[64];
			 t_sprintf(mag,"S是否禁止爆物品:%d,是否开启:%d",Player[nBelongPlayer].m_nRoomFull,nIsOpenFullForbit);
			 Player[nBelongPlayer].m_ItemList.msgshow(mag);*/

			if (nIsOpenFullForbit>=1 && Player[nBelongPlayer].m_nRoomFull==1)	  //包袱满了 不产生物品
			{
				nIsBreak=1;
				return;
			}

            g_GameSetTing.GetInteger2("ServerConfig","IsCloseItem",&nIsCloseGuaItem,&nIsCloseAllItem);
			g_GameSetTing.GetInteger2("ServerConfig","TimeCloseItem",&nTimeStart,&nTimeEnd);

			if (nIsCloseAllItem>=1)
			{
                return;
			}

			time_t rawtime;      //定义一个long 型存放秒数
			struct tm * timeinfo;	
			time (&rawtime);     //距离现在的时间（秒）
			timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
			int	bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0;			
			bYear   = timeinfo->tm_year+1900;
			bMonth  = timeinfo->tm_mon+1;
			bDay    = timeinfo->tm_mday;
			bHour   = timeinfo->tm_hour;
	        bMin    = timeinfo->tm_min;
            
            if (nIsCloseGuaItem>=1 && bHour>=nTimeStart && bHour<nTimeEnd)
			{
				nIsBreak=1;
				return;
			}
		}

		/*if (Player[nBelongPlayer].m_ItemList.GetPlayerItemCount()>=MAX_ITEMLIST_CONUT)
		{
			//Player[nBelongPlayer].m_ItemList.msgshow("提示:<color=yollow>物品超限,请清理物品空间<color>!");
			nIsBreak=1;
			return;
		}*/
//----------------------------------------------------------
		//------------------------------------------------新的
		//CONST KItemDropRate::KItemParam* pItemDropParam = NULL;  //物品信息部分
		if (nNum<=0)
		    nNum = 5;

		int nCurIdx = 0;
		for (int i = 0; i < nNum; ++i) 
		{  			
            if (nIsCloseAllItem>=1)
				return;
            if (nIsBreak>=1)
				return;

		/*	pItemDropParam = m_pDropRate->GetRandItem(nLuck); //掉的就是这个物品
			
			if (!pItemDropParam)
			   continue;*/

		   //printf("--取得物品:%d,%d,%d -- \n",pItemDropParam->nGenre,pItemDropParam->nDetailType,pItemDropParam->nParticulType);
			char nstrName[64]={0};
                 GetstrInfo(STR_GUISHU_NAME,nstrName);
		   if (strstr(nstrName,"系统"))    
		   {//公共boss
			     nCurIdx = nBelongPlayer;
			     LoseSingleItem(nBelongPlayer,NULL,nLuckyRate,nLastDamageIdx);   //NPC掉下装备 m_nLastDamageIdx	 
		   }
		   else 
		   {//私人BOSS
			 //  int nNpcidx=NpcSet.SearchGSName(m_GuishuName);
			 
			    int mPlayerIdx = PlayerSet.FindNameID(nstrName);
				    nCurIdx = nBelongPlayer;

                if (mPlayerIdx>0)
				{//在线 
					    nCurIdx = mPlayerIdx;
				        LoseSingleItem(mPlayerIdx,NULL,nLuckyRate,nLastDamageIdx);   //NPC掉下装备
				}
				else
				{//不在线
						LoseSingleItem(nBelongPlayer,NULL,nLuckyRate,nLastDamageIdx);   //NPC掉下装备
				} 
		   }  
		}

		
		if (g_RandPercent(m_pDropRate->nMoneyRate));//GetItemParm(ITEM_PARM_MONEYRATE)))  //随机取爆率范围数 return ((int)g_Random(100) < nPercent);
		{   
			LoseMoney(nCurIdx);  //钱
		} 
	}
}

// 玩家死的时候掉出来的钱生成一个object
void	KNpc::PlayerDeadCreateMoneyObj(int nMoneyNum)
{
	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
}
//掉下的钱数
void KNpc::LoseMoney(int nBelongPlayer,int nMoneyNum)
{

	if  (nBelongPlayer<=0)
		return ;

	int nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
    int  nJinQian=Npc[Player[nBelongPlayer].m_nIndex].IsJinQian;
	int  nMoneyRate[3];
         nMoneyRate[0]=1;
		 nMoneyRate[1]=0;
		 nMoneyRate[2]=100;
		g_GameSetTing.GetInt3("ServerConfig","MoneyRate",nMoneyRate);
//m_CurrentExperience
	int nMoney = (nMoneyRate[2]*Npc[Player[nBelongPlayer].m_nIndex].m_Level*nMoneyRate[0]* m_pDropRate->nMoneyScale*nJinQian)/100;	//GetItemParm(ITEM_PARM_MONEYSCALE)
	//TakeTrader(nMoneyRate[2]*Npc[Player[nBelongPlayer].m_nIndex].m_Level*nMoneyRate[0]* m_pDropRate->nMoneyScale*nJinQian,100);   //金钱的倍率
	
	if (nMoney <= 0)
		return;

    if (nMoney>=nMoneyRate[1])
        nMoney=nMoneyRate[1]/2+GetRandomNumber(1,nMoneyRate[1]/2);
	//TakeTrader(nMoneyRate[1],2)+GetRandomNumber(1,TakeTrader(nMoneyRate[1],2));

	if (nMoneyNum>0)
		nMoney=GetRandomNumber(nMoneyNum/2,nMoneyNum);

	GetMpsPos(&nX, &nY,&nMap);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoney);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		if (nBelongPlayer > 0)
			Object[nObjIdx].SetItemBelong(nBelongPlayer);
		else
			Object[nObjIdx].SetItemBelong(-1);
	}

}

//随机掉下某个装备
int  KNpc::DropOneItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,\
					   int nItemLevel, int nItemSeries,int nLuckey,int nRate,int nStackNum,\
					   int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
 /*   
	int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);//全服幸运
	if (nGoldLuck<=0)
        nGoldLuck=1;
*/
	if (nItemSeries<=-1 || nItemSeries>=5)
	      nItemSeries   = GetRandomNumber(0,4);//m_Series;
	
//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	int bYear,bMonth,bDay,bHour,bMin,pnMagicLevel[6],nSLevel=0;
	ZeroMemory(pnMagicLevel,sizeof(pnMagicLevel));

	if (IsHour>0)
	{
      bYear  =timeinfo->tm_year+1900;
      bMonth =timeinfo->tm_mon+1;
      bDay   =timeinfo->tm_mday;
      bHour  =timeinfo->tm_hour+IsHour;
	  bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
    	bMin   =0;
	}

	if (nMgCount<=0)
		nMgCount=GetRandomNumber(1,6);

	if (nStackNum<=0)
	   nStackNum=1;
	
	for (int j = 0; j <nMgCount; ++j)    // 掉下的属性数量
	{
		if (!nItemGenre) 
		{//是装备
			if (nMgLevel<=0)
			    nSLevel=GetRandomNumber(1,10);
			else
				nSLevel=nMgLevel;

				pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围
		}
		else
		{//为白装  爆其他的 	 
			pnMagicLevel[j] = 0;  //空属性
		}
	}
//--------------------------------------------------------------------
	if (nItemLevel<=0)
        nItemLevel = 1;

	int nItemIdx=0;

    if (IsQuality==0)
		nItemIdx = ItemSet.AddOther(nItemGenre,nItemSeries,nItemLevel,nLuckey,\
		                  nDetailType, nParticulType, pnMagicLevel,\
		                  g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nStackNum),\
		                  0,0,bYear,bMonth,bDay,bHour,bMin,0,\
                            NULL,NULL,NULL,0);
	else if(IsQuality==1) //黄装的产生
	    nItemIdx = ItemSet.AddGold(nDetailType,pnMagicLevel,nItemSeries,0,0,\
		                   bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nItemLevel,nLuckey);

	if (nItemIdx <= 0)
		return 0;

    if (IsBang)
        Item[nItemIdx].SetBang(IsBang);

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);                         // 怪物的坐标=转换后的坐标

	ptLocal.x = nX;
	ptLocal.y = nY;

	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);  //获取能掉下东西的坐标
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		              &Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		                           &Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID     = nItemIdx;                   //物品索引
	sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
	sInfo.m_nMoneyNum   = 0;
	strcpy(sInfo.m_szName, Item[nItemIdx].GetName()); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
    sInfo.m_sHaveAttack = 0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
	{
		ItemSet.Remove(nItemIdx);
		return 0;
	}
	else
	{
		if (nBelongPlayer > 0)
		{
		   
              Object[nObj].SetItemBelong(nBelongPlayer);
		} 
		else
		{
			  Object[nObj].SetItemBelong(-1);
		}
	}

		return nItemIdx;

}

//NPC掉出装备   NPC掉装备  NPC掉下装备 玩家PK掉下装备
void KNpc::LoseSingleItem(int nBelongPlayer,CONST KItemDropRate::KItemParam* pItem,int LuckyRate,int LastDamageIdx,int IsWhere,int Series)
{
	if (!m_pDropRate /*|| m_IsClearDropRank!=0*/)  //如果没有爆率 就不掉出
		return;

//	if  (!pItem)
//		return;

	if (m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand = g_Random(m_pDropRate->nMaxRandRate);  //总爆率  取余数

	int nCheckRand  = 0;	// 累加概率，确认是否落在区间内
    //int nIsThisItem =-1;
	int i;
	for (i = 0; i < m_pDropRate->nCount; ++i)   //循环掉装备  装备的总数量
	{
		if (nRand >= nCheckRand && nRand < nCheckRand + m_pDropRate->pItemParam[i].nRate)
		{//这个数大于等于上个物品的累计爆率并小于当时这个物品的累加爆率
			//nIsThisItem = i;
			break;
		}

		nCheckRand += m_pDropRate->pItemParam[i].nRate;   //每个物品爆率加起来累加
	}

	if (i >= m_pDropRate->nCount)  //没找到合适物品爆 就返回了
		return;	
  
//==============爆的就是这个物品了=================
	int nGenre, nLuck=0, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang=0,nHour=0,bYear=0,bMonth=0,bDay=0,bHour=0,bMin=0,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	int pnRMagicLevel[6],nIsNoBian=0,nMaxStackNum=1,nNpcCurLucky=0;
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	nGenre         = m_pDropRate->pItemParam[i].nGenre;
	nDetail        = m_pDropRate->pItemParam[i].nDetailType;
	nParticular    = m_pDropRate->pItemParam[i].nParticulType;

	if 	(nGenre<0)
		return;

	int nGoldLuck=g_GlobalMissionArray.GetMissionValue(28);//全服幸运
	if (nGoldLuck<=0)
        nGoldLuck=1;

	char nstrName[64]={0};
			Npc[LastDamageIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);

	if (LastDamageIdx>0 && Npc[LastDamageIdx].m_Kind==kind_partner)
	{//同伴
	   if (strstr(nstrName,Npc[Player[nBelongPlayer].m_nIndex].Name))
	   {//并且是主人的话 
		   nNpcCurLucky= Npc[LastDamageIdx].m_nCurNpcLucky;
	   }
	}

	/*if (nNpcCurLucky==0 && nBelongPlayer>0 && Npc[Player[nBelongPlayer].m_nIndex].m_TongBanNum)
	{//已经召唤了宠物的
		
		int nNpcIdx=0;
		    nNpcIdx =NpcSet.SearchID(Npc[Player[nBelongPlayer].m_nIndex].m_TongBanNum);
		if (nNpcIdx>0)
		{
			nNpcCurLucky=Npc[nNpcIdx].m_nCurNpcLucky;
		} 
	}*/

   if (nBelongPlayer>0)
	    nLuck= Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey*nGoldLuck+nNpcCurLucky;  //玩家的幸运值
	
	//if ((m_pDropRate->pItemParam[i].nRate+nLuck) < g_Random(nRand))
   //    	return;

	if  (Series<0 || Series>4)
	      Series   = GetRandomNumber(0,4);//m_Series;
	
    n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = m_pDropRate->pItemParam[i].nQuality;         //是否黄金 或紫装
    nwMinItemLevel = m_pDropRate->pItemParam[i].nMinItemLevel;    //物品部分的等级
    nwMaxItemLevel = m_pDropRate->pItemParam[i].nMaxItemLevel;    //物品的等级
    nIsBang        = m_pDropRate->pItemParam[i].nIsBang;
    nHour          = m_pDropRate->pItemParam[i].nHour;
    nIsNoBian      = m_pDropRate->pItemParam[i].nIsNoBian;
    nMaxStackNum   = m_pDropRate->pItemParam[i].nStackNum;
//==============爆的就是这个物品了==================
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	if (nHour>0)
	{
      bYear  =timeinfo->tm_year+1900;
      bMonth =timeinfo->tm_mon+1;
      bDay   =timeinfo->tm_mday;
      bHour  =timeinfo->tm_hour+nHour;
	  bMin   =timeinfo->tm_min;
	}
	else
	{
		bYear  =0;
		bMonth =0;
		bDay   =0;
		bHour  =0;
    	bMin   =0;
	}
//    printf("[玩家](%s)幸运(%d) OK...\n",Npc[Player[nBelongPlayer].m_nIndex].Name,nLuck);
    int nMaxLevel,nMinLevel,mCount=0,mAllLuck=nLuck/*+n_Wlevel+m_Level*/,nBossType=4,nIsNoRandomGoald=0;
    int nLuckItem[5];
	    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;
		
		//g_GameSetTing.GetInteger("SYSTEM","IsRandomGold",0,&nIsRandomGoald);
		nIsNoRandomGoald = m_pDropRate->pItemParam[i].nIsNoRandGold;
	    g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyCount",nLuckItem);

	if (mAllLuck>=nLuckItem[0] && mAllLuck <=nLuckItem[1])
		mCount=GetRandomNumber(1,2);
	else if (mAllLuck>nLuckItem[1] && mAllLuck <=nLuckItem[2])
		mCount=GetRandomNumber(1,4);
	else if  (mAllLuck>nLuckItem[2] && mAllLuck<=nLuckItem[3])
		mCount=GetRandomNumber(1,5);
	else if (mAllLuck>nLuckItem[3] && mAllLuck <=nLuckItem[4])
	    mCount=GetRandomNumber(1,6);
	else if (mAllLuck>nLuckItem[4])
	    mCount=GetRandomNumber(1,12);
	else
        mCount=1;

	if (mCount>6)
		mCount=6;

	g_GameSetTing.GetInteger("SYSTEM","MagciCountFull",-1,&nBossType);

	if (nBossType>=0 && this->m_cGold.GetGoldType() >= nBossType)
	{
	   mCount=6;
	}

	//nLuck *=IsLuKey;

	BOOL bSkip = FALSE;

	int j;
	//printf("----爆的就是这个物品(%d,%d,%d)---\n",nGenre,nDetail,nParticular);
	for (j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{//不等Ture 就执行掉 GetItemParm(ITEM_PARM_MAGRATE)
			 if (g_Random(m_pDropRate->nMagicRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
				 if (nwMinItemLevel>0 && nwMaxItemLevel>0)
				 {//物品指定的等级	 
					 nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
				 }
				 else
				 {//NPC等级 / 等级倍率 ==物品的等级
					 nMaxLevel = m_Level/m_pDropRate->nMaxItemLevelScale;//GetItemParm(ITEM_PARM_MAXLEVELSCALE);
					 nMinLevel = m_Level/m_pDropRate->nMinItemLevelScale;//GetItemParm(ITEM_PARM_MINLEVELSCALE);
					 
					 if (nMaxLevel > m_pDropRate->nMaxItemLevel)  //全部等级参数
						 nMaxLevel = m_pDropRate->nMaxItemLevel;
					 
					 if (nMinLevel < m_pDropRate->nMinItemLevel)
						 nMinLevel = m_pDropRate->nMinItemLevel;
					 
					 if (nMaxLevel < m_pDropRate->nMinItemLevel)
						 nMaxLevel = m_pDropRate->nMinItemLevel;
					 
					 if (nMinLevel > m_pDropRate->nMaxItemLevel)
						 nMinLevel = m_pDropRate->nMaxItemLevel;
					 
					 if (nMaxLevel < nMinLevel)
					 {
						 int nTemp;
						 nTemp     = nMinLevel;
						 nMinLevel = nMaxLevel;
						 nMaxLevel = nTemp;
					 }

					 nLevel = g_Random(nMaxLevel - nMinLevel) + nMinLevel; //*随机种子 然互取 取余数 装备等级
				 } 


				 if (nLevel>=10)
					 nLevel = 10;

//////////////////////////这里的等级影响属性的等级///////////////////////////////////////
				 g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyLevel",nLuckItem);
				 //m_IsNoLevel
				 if (m_IsNoLevel<=0)
				 {//如果不检测等级
					 int nCha=0;
					 if ((n_Wlevel-m_Level)>0)
						 nCha=n_Wlevel-m_Level;
					 else
						 nCha=m_Level-n_Wlevel;                //等级差

					 if (nCha<GetRandomNumber(1,nLuckItem[0])) //对比的等级
					 {//物品的品数，否则就是白装
						  nSLevel =GetRandomNumber(1,10);
					 } 
					 else
						  nSLevel =GetRandomNumber(0,nLuckItem[1]); 
				 }
				 else
				 {//如果不检测等级的差距 
					 nSLevel = nLevel;
				 }
				 		 
				 pnMagicLevel[j] = nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现  属性的品数 1-10范围

				 if (nSLevel==0)
					bSkip = TRUE;
			 }
			 else
			 {//概率不在这个范围的 不再掉下有属性的
				     pnMagicLevel[j] = 0; // 空装备
				     bSkip = TRUE;        // 不再掉下装备了
			 }
		}
		else
		{//为白装  爆其他的 
			if (nwMinItemLevel>0 && nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}

	int nItemIdx=0,nRpiont=0,nMagicNum=0; 

	//-------蓝装nLevel物品的产生影响装备的外观等级---辨识属性设置----	
	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&nMagicNum);

    if (nGenre==0 && m_pDropRate->nIsBianShi && pnMagicLevel[nMagicNum] && nIsNoBian!=1)
	{//有多少个属性以上的才可以爆辨识装备
		g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyCount",nLuckItem);

		if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
			nRpiont = GetRandomNumber(0,1);
		else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
			nRpiont = GetRandomNumber(0,2);
		else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
			nRpiont = GetRandomNumber(0,3);
		else if (nLuck>nLuckItem[3]&& nLuck <=nLuckItem[4])
			nRpiont = GetRandomNumber(0,4);
		else if (nLuck>nLuckItem[4])
			nRpiont = GetRandomNumber(0,6);
		else
            nRpiont = 0;

        if (nRpiont>0)
		{ //是否产生辨识属性
		/*	KTabFile nRongt;

		nRongt.Load(TABFILE_FUSION_PATH);
		int nRows = nRongt.GetHeight(),nAddRows=0;

	    g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyLevel",nLuckItem);

		if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
			nAddRows=GetRandomNumber(0,2);
		else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
			nAddRows=GetRandomNumber(0,3);
		else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
			nAddRows=GetRandomNumber(0,4);
		else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
			nAddRows=GetRandomNumber(0,5);
		else if (nLuck >nLuckItem[4])
			nAddRows=GetRandomNumber(0,8);
		else
			nAddRows=GetRandomNumber(0,1);*/
		
		  for (j=0;j<nRpiont;++j)
		  { //熔炼属性赋值 == 行数
			  pnRMagicLevel[j] = 0;//isHave; //设定行数			
		  } 
		 
		  IsWhere = -1;                      //设置未辨识

          int nTime = 1;
		    g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&nTime);

          if (pnRMagicLevel[0])
		  {  
            bYear  =timeinfo->tm_year+1900;
			bMonth =timeinfo->tm_mon+1;
			bDay   =timeinfo->tm_mday;
			bHour  =timeinfo->tm_hour+nTime;   //默认有一个小时来辨识
	        bMin   =timeinfo->tm_min;
		  } 
		}
        else
		{  
            nRpiont = 0;
		}   

	}   

    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;
//--------------------------------------------------------------------
	if (nLevel<=0)
        nLevel = 1;
	int nForBitItem[3];
	    g_GameSetTing.GetInt3("ServerConfig","AutoPlayForBitItem",nForBitItem);

	if (nBelongPlayer>0 && Npc[Player[nBelongPlayer].m_nIndex].m_AutoplayId>0 && nForBitItem[0] && timeinfo->tm_hour>=nForBitItem[1] && timeinfo->tm_hour<nForBitItem[2])
	{//挂机的 开启并且是这时间段的 开始检测 不能掉落的物品
		if (CheckAllItem(3,nGenre,nDetail,nParticular))
			return;
	}
	//nLuck *=IsLuKey
	int nSerLucky = 0;

	if (nLuck*IsLuKey>2000000000)
		nSerLucky = 2000000000;
	else
		nSerLucky = nLuck*IsLuKey;

    if (nQuality==0)
		nItemIdx = ItemSet.AddOther(nGenre,Series,nLevel,nSerLucky,\
		                  nDetail, nParticular, pnMagicLevel,\
		                  g_SubWorldSet.GetGameVersion(),0,GetRandomNumber(1,nMaxStackNum),\
		            0,0,bYear,bMonth,bDay,bHour,bMin,nRpiont,\
                            pnRMagicLevel,NULL,NULL,IsWhere);
	else if(nQuality>0) //黄装的产生
	{
		if (nIsNoRandomGoald<=0)
		  nItemIdx = ItemSet.AddRondomGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,nLevel,nSerLucky,nDetail);
		else
          nItemIdx = ItemSet.AddGold(nQuality,pnMagicLevel,Series,0,0,bYear,bMonth,bDay,bHour,bMin,NULL,NULL,nLevel,nSerLucky);
	}  
	//char msg[80];
	//t_sprintf(msg,"S测试:%s(%d,%d,%d,%d)",Item[nItemIdx].GetName(),nGenre,nDetail,nParticular,nSLevel);
    //Player[nBelongPlayer].m_ItemList.msgshow(msg);//m_PlayerIdx

	if (nItemIdx <= 0)
		return;

    if (nIsBang)
          Item[nItemIdx].SetBang(nIsBang);

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);                         // 怪物的坐标=转换后的坐标

	ptLocal.x = nX;
	ptLocal.y = nY;

	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);  //获取能掉下东西的坐标
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,\
		              &Pos.nRegion, &Pos.nMapX, &Pos.nMapY,\
		                           &Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID     = nItemIdx;                   //物品索引
	sInfo.m_nItemWidth  = Item[nItemIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
	sInfo.m_nMoneyNum   = 0;
	strcpy(sInfo.m_szName, Item[nItemIdx].GetName()); //物品的名称
	sInfo.m_nColorID    = 0;                      //颜色
	sInfo.m_nMovieFlag  = 1;
	sInfo.m_nSoundFlag  = 1;
    sInfo.m_sHaveAttack = 0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID
	nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(),Pos,sInfo);   //开始同步地上物品

	if (nObj == -1)
	{
		ItemSet.Remove(nItemIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
		{
		    //int nmIdx=0;
            //nmIdx=NpcSet.SearchName(Npc[nAttacker].m_GuishuName);   //查找归属者的索引
            //m_cDeathCalcExp.AddDamage(Npc[nmIdx].m_nPlayerIdx, (m_CurrentLife - nDamage > 0 ? nDamage : m_CurrentLife));
	     	if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
			   	int nmIdx=0;
				char nstrName[64]={0};
			         Npc[LastDamageIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);
                    nmIdx=NpcSet.SearchName(nstrName);   //查找归属者的索引
					//if (strstr(Npc[Player[nBelongPlayer].m_nIndex].Name,Npc[LastDamageIdx].m_GuishuName))
					if (nmIdx)
					{
					    Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
					}
					else
					    Object[nObj].SetItemBelong(-1);
			}
			else
              Object[nObj].SetItemBelong(nBelongPlayer);
		} 
		else
		{
			if (Npc[LastDamageIdx].m_Kind==kind_partner)
			{//如果是同伴的话
				int nmIdx=0;
			         Npc[LastDamageIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);
				nmIdx=NpcSet.SearchName(nstrName);   //查找归属者的索引
				//if (strstr(Npc[Player[nBelongPlayer].m_nIndex].Name,Npc[LastDamageIdx].m_GuishuName))
				if (nmIdx)
				{
					Object[nObj].SetItemBelong(Npc[nmIdx].GetPlayerIdx());
				}
				else
					Object[nObj].SetItemBelong(-1);
			}
			else
			  Object[nObj].SetItemBelong(-1);
		}
	}
}

//NPC脚本掉出装备   NPC掉装备  NPC掉下装备 玩家PK掉下装备
int KNpc::DropSingleItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nStackNum,int *JBlevel,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;

	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	int LuckyRate;
      g_GameSetTing.GetInteger("ServerConfig","LuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}
//   printf("[玩家](%s)幸运(%d) OK...\n",Npc[Player[nBelongPlayer].m_nIndex].Name,nLuck);
    int nMaxLevel,nMinLevel,mCount=0;

	int nLuckItem[5];
	    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;

	    g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyCount",nLuckItem);

	if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
		mCount=GetRandomNumber(1,2);
	else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
		mCount=GetRandomNumber(1,4);
	else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
		mCount=GetRandomNumber(1,5);
	else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
		mCount=GetRandomNumber(1,6);
	else if (nLuck>nLuckItem[4])
		mCount=GetRandomNumber(2,6);
	else
        mCount=1;
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
				{
                  nSLevel=nMgLevel;
				}
				else
				{
				 int nCha=0;
				 if ((n_Wlevel-m_Level)>0)
					 nCha=n_Wlevel-m_Level;
				 else
					 nCha=m_Level-n_Wlevel;  //等级差
				 
				 int mBiJIAO=GetRandomNumber(1,250);

				  g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyLevel",nLuckItem);

					 if (nCha<mBiJIAO)
					 {
					   if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
						 nSLevel=GetRandomNumber(1,2);
					   else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
						 nSLevel=GetRandomNumber(1,3);
					   else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
						 nSLevel=GetRandomNumber(1,4);
					   else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
						 nSLevel=GetRandomNumber(1,5);
					   else if (nLuck >nLuckItem[4])
						   nSLevel=GetRandomNumber(1,10);
					   else
                         nSLevel=GetRandomNumber(0,1);		 
					 } 
				     else
					     nSLevel=GetRandomNumber(0,1);
				}
				     pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现
			 }
			 else
			 {
				     pnMagicLevel[j] = 0; //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}

			pnMagicLevel[j] = 0;  //空属性
		}
	}


    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;

	int nIdx=0; 
	if (nLevel<=0)
        nLevel = 1;

	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,JBlevel,NULL,0);
	/*

  bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	AddOther(IN int nItemGenre, IN int nSeries, IN int nLevel=1, IN int nLuck=0, IN int nDetail = -1, \
		                   IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0,\
			    IN UINT nRandomSeed = 0,IN int nStackNum = 1, IN int nEnChance = 0,IN int nPoint = 0, \
			  IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int nMin = 0,\
						                               IN int nRongpiont=0,IN int* pnRMagicLevel = NULL,\
						           IN int* pnJBLevel = NULL,IN int* pnBsLevel=NULL,IN int IsWhere=0); 
	*/
//黄装的产生
	else if(nQuality==1)
	{
		for (int i=0;i<6;++i)
			pnMagicLevel[i]=JBlevel[i];

	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,
	                                bYear,bMonth,bDay,bHour,bMin,
						                  NULL,NULL,nLevel,nLuck);
	}
	
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
    memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	t_sprintf(sInfo.m_szName, Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}

	return nIdx;
}
//NPC脚本调出辨识装备
int KNpc::DropBianShiItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nRpiont,int IsWhere,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsMin,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;
	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

	int LuckyRate;
      g_GameSetTing.GetInteger("ServerConfig","LuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,nMin,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	int pnRMagicLevel[6];

	ZeroMemory(pnRMagicLevel,sizeof(pnRMagicLevel));

	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;      //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;
	nMin           = IsMin;
 //   nMin=m_pDropRate->pItemParam[i].nMin;
	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime ( &rawtime );

	if (nHour>0 || nMin>0)
	{
		bYear   =timeinfo->tm_year+1900;
		bMonth  =timeinfo->tm_mon+1;
		bDay    =timeinfo->tm_mday;
		bHour   =timeinfo->tm_hour+nHour;
		bMin    =timeinfo->tm_min+nMin;
	}
	else
	{
		bYear   = 0;
		bMonth  = 0;
		bDay    = 0;
		bHour   = 0;
		bMin    = 0;
	}

    int nMaxLevel,nMinLevel,mCount=0;
	int nLuckItem[5];
	    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;

	 g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyCount",nLuckItem);

	if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
		mCount=GetRandomNumber(1,2);
	else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
		mCount=GetRandomNumber(1,4);
	else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
		mCount=GetRandomNumber(1,5);
	else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
		mCount=GetRandomNumber(1,6);
	else if (nLuck>nLuckItem[4])
		mCount=GetRandomNumber(2,6);
	else
        mCount=1;
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
    int j;
	for (j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
				{
                  nSLevel=nMgLevel;
				}
				else
				{
				 int nCha=0;
				 if ((n_Wlevel-m_Level)>0)
					 nCha=n_Wlevel-m_Level;
				 else
					 nCha=m_Level-n_Wlevel;  //等级差
				 
				 int mBiJIAO=GetRandomNumber(1,250);

                 g_GameSetTing.GetInt5("SYSTEM","BlueMagciLuckyLevel",nLuckItem);

				if (nCha<mBiJIAO)
				{
					   if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
						 nSLevel=GetRandomNumber(1,2);
					   else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
						 nSLevel=GetRandomNumber(1,3);
					   else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
						 nSLevel=GetRandomNumber(1,4);
					   else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
						 nSLevel=GetRandomNumber(1,5);
					   else if (nLuck >nLuckItem[4])
						   nSLevel=GetRandomNumber(1,10);
					   else
                         nSLevel=GetRandomNumber(0,1);		 
				 }
				 else
                         nSLevel=GetRandomNumber(0,1);		 
				}
				     pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  出现
			 }
			 else
			 {
				     pnMagicLevel[j] = 0;       //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}
	int nIdx=0,nMagicNum=0; ; 
//-------蓝装nLevel物品的产生影响装备的外观等级---------------------

	g_GameSetTing.GetInteger("ServerConfig","MagicNum",0,&nMagicNum);

	if (nRpiont>0 && pnMagicLevel[nMagicNum] && nGenre==0)
	{//是蓝装
	   /* KTabFile nRongt;
       nRongt.Load(TABFILE_FUSION_PATH);
	   int nRows = nRongt.GetHeight(),nAddRows;

	  g_GameSetTing.GetInt5("SYSTEM","BianShiLuckyLevel",nLuckItem);

		if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
			nAddRows=GetRandomNumber(0,2);
		else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
			nAddRows=GetRandomNumber(0,3);
		else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
			nAddRows=GetRandomNumber(0,4);
		else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
			nAddRows=GetRandomNumber(0,5);
		else if (nLuck >nLuckItem[4])
			nAddRows=GetRandomNumber(0,8);
		else
			nAddRows=GetRandomNumber(0,1);*/

       for (j=0;j<nRpiont;++j)
	   {//熔炼属性赋值 == 行数
	  /*	 
	    KTabFile nRongt;
       nRongt.Load(TABFILE_FUSION_PATH);
	   int nRows = nRongt.GetHeight(),nAddRows;
	
	     // int bRows = GetRandomNumber(3,nRows);
		 // int isHave=0;
		 //	  nRongt.GetInteger(bRows,"魔法属性索引1",0,&isHave);
		*/

         // if (pnRMagicLevel[j]==0)
		  //{
              pnRMagicLevel[j] = 0;//bRows; //设定行数
			  //IsWhere++;
		 // }

	   }
       IsWhere = -1;                  //设置未辨识
       int nTime = 1;
	   g_GameSetTing.GetInteger("ServerConfig","BianShiTime",1,&nTime);

	   if (pnRMagicLevel[0])
	   {//设置为未辨识
		   bYear  =timeinfo->tm_year+1900;
		   bMonth =timeinfo->tm_mon+1;
		   bDay   =timeinfo->tm_mday;
		   bHour  =timeinfo->tm_hour+nTime;   //默认有一个小时来辨识
		   bMin   =timeinfo->tm_min;
//		   IsWhere = -1;                  //设置未辨识
		}
//	    nRongt.Clear();
}
	nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;
//----------------------------------------------------------------
//蓝装 白装 紫装 和其他东西的产生
	if (nLevel<=0)
        nLevel = 1;

    if (nQuality==0)
	   nIdx = ItemSet.AddOther(nGenre,nSeries, nLevel,nLuck,\
	                           nDetail, nParticular, pnMagicLevel, \
							   g_SubWorldSet.GetGameVersion(),0,1,\
                               0,0,bYear,bMonth,bDay,bHour,bMin,nRpiont,\
                               pnRMagicLevel,NULL,NULL,IsWhere);
//黄装的产生
	else if(nQuality==1)
	    nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,\
		                       bYear,bMonth,bDay,bHour,bMin,\
							   NULL,NULL,nLevel,nLuck);
	
	if (nIdx <= 0)
		return FALSE;

	int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	t_sprintf(sInfo.m_szName, Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;   //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}

	return nIdx;
}



int KNpc::GiveSingleItem(int nBelongPlayer,int nItemGenre,int nDetailType,int nParticulType,int nRate,int nItemMinLevel,int nItemMaxLevel, int nItemSeries,int nMgCount,int nMgLevel,int IsBang,int IsHour,int IsQuality)
{
	if (!nRate)  //如果没有爆率 就不掉出
		return FALSE;

	if (g_Random(nRate) < g_Random(2000)) //概率检测
		return FALSE;

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	int LuckyRate;
      g_GameSetTing.GetInteger("ServerConfig","DuanZaoLuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0, pnMagicLevel[6],nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin,n_Wlevel;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=0;

	int nLuckItem[5];
	    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;

	    g_GameSetTing.GetInt5("ServerConfig","DuanZaoBlueMagciLucky",nLuckItem);

	if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
		mCount=GetRandomNumber(1,2);
	else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
		mCount=GetRandomNumber(1,4);
	else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
		mCount=GetRandomNumber(1,5);
	else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
		mCount=GetRandomNumber(1,6);
	else if (nLuck>nLuckItem[4])
		mCount=GetRandomNumber(2,6);  //属性数量
	else
        mCount=1;
/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
/////////////////////////////////////////////////////////////////////
	BOOL	bSkip = FALSE;
	for (int j = 0; j <mCount; ++j)    // 掉下的属性数量
	{
		if (!bSkip)
		{
			 if (g_Random(nRate + nLuck) >= g_Random(LuckyRate))   // 取余数 幸运+蓝装的爆率  魔法属性的爆率
			 {
//////////////////////////这里的等级影响属性的等级////////////////////////////////////////////////////////////////
				if (nMgLevel>0)
				{
                    nSLevel=nMgLevel;
				}
				else
				{
				  g_GameSetTing.GetInt5("ServerConfig","DuanZaoMagciLuckyLevel",nLuckItem);

					   if (nLuck>=nLuckItem[0] && nLuck <=nLuckItem[1])
						 nSLevel=GetRandomNumber(1,2);
					   else if (nLuck>nLuckItem[1] && nLuck <=nLuckItem[2])
						 nSLevel=GetRandomNumber(1,3);
					   else if  (nLuck>nLuckItem[2] && nLuck <=nLuckItem[3])
						 nSLevel=GetRandomNumber(1,4);
					   else if (nLuck>nLuckItem[3] && nLuck <=nLuckItem[4])
						 nSLevel=GetRandomNumber(1,5);
					   else if (nLuck >nLuckItem[4])
						   nSLevel=GetRandomNumber(1,10);
					   else
                           nSLevel=GetRandomNumber(0,1);		 
				}
				     pnMagicLevel[j] =nSLevel;  //魔法属性的类型 魔法属性里面的行数  品数
			 }
			 else
			 {
				     pnMagicLevel[j] = 0; //空装备
				     bSkip = TRUE;
			 }
		}
		else
		{
			if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
			{//物品指定的等级	 
				nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  
			}
			pnMagicLevel[j] = 0;  //空属性
		}
	}

    nLuckItem[0]=0;nLuckItem[1]=0;nLuckItem[2]=0;nLuckItem[3]=0;nLuckItem[4]=0;

	int nIdx=0; 

	if (nLevel<=0)
        nLevel = 1;
//nStackNum,nEnChance,nPoint
	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries,nLevel,nLuck, nDetail,\
	        nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(),\
			0,1,0,0,bYear,bMonth,bDay,bHour,bMin,0,NULL,NULL,NULL,0);
//黄装的产生
	else if(nQuality==1)
	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,0,\
	                                bYear,bMonth,bDay,bHour,bMin,\
						                  NULL,NULL,nLevel,nLuck);
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);


	return nIdx;
}


int KNpc::GiveZhiDingItem(int nBelongPlayer,int nItemGenre,int nDetailType,\
           int nParticulType,int nItemMinLevel,int nItemMaxLevel,\
                       int nItemSeries,int IsBang,int IsHour,int IsQuality,\
                       int nStackNum,int nEnChance,int nPoint,int nIsWhere,\
         int* pnMagicLevel,int* pnRMagicLevel,int* pnJbLevel, int* pnBsLevel)
{

    if (nItemSeries==-1)
        nItemSeries=GetRandomNumber(0,4);

	//int LuckyRate;
    //  g_GameSetTing.GetInteger("ServerConfig","ZhiDingoLuckyRate",5000,&LuckyRate);

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel=1,nSLevel=0,nQuality,nwMinItemLevel,nwMaxItemLevel,nIsBang,nHour,bYear,bMonth,bDay,bHour,bMin;  //Quality=1是黄装 2为白金，3 为紫装 等
	nGenre         = nItemGenre;
	nDetail        = nDetailType;
	nParticular    = nParticulType;
	nSeries        = nItemSeries;//m_Series;
	nLuck          = Player[nBelongPlayer].m_nCurLucky*Npc[Player[nBelongPlayer].m_nIndex].IsLuKey;  //玩家的幸运值
    //n_Wlevel       = Npc[Player[nBelongPlayer].m_nIndex].m_Level; //玩家等级
	nQuality       = IsQuality;  //是否黄金
    nwMinItemLevel = nItemMinLevel;  //物品部分的等级
    nwMaxItemLevel = nItemMaxLevel;
    nIsBang        = IsBang;
    nHour          = IsHour;

	time_t rawtime;
	struct tm * timeinfo;	
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	if (nHour>0)
	{
       bYear   =timeinfo->tm_year+1900;
       bMonth  =timeinfo->tm_mon+1;
       bDay    =timeinfo->tm_mday;
       bHour   =timeinfo->tm_hour+nHour;
	   bMin    =timeinfo->tm_min;
	}
	else
	{
        bYear   =0;
		bMonth  =0;
		bDay    =0;
		bHour   =0;
		bMin    =0;
	}

    int nMaxLevel,nMinLevel,mCount=0;

/////////////////////////////////////////////////////////////////////////////////////////////////
	if (nwMinItemLevel>0 &&  nwMaxItemLevel>0)
	{
		nLevel = GetRandomNumber(nwMinItemLevel,nwMaxItemLevel);  //物品指定的等级	 
	}
	else
	{ 
		nLevel = GetRandomNumber(1,7);
	} 
////////////////////////////////////////////////////////////////////

	int nIdx=0; 

	if (nLevel<=0)
        nLevel = 1;

	if (nQuality==0)
         nIdx = ItemSet.AddOther(nGenre,nSeries,nLevel,nLuck,nDetail,\
	         nParticular,pnMagicLevel,g_SubWorldSet.GetGameVersion(),\
			0,nStackNum,nEnChance,nPoint,bYear,bMonth,bDay,bHour,bMin,\
			           0,pnRMagicLevel,pnJbLevel,pnBsLevel,nIsWhere);
//黄装的产生
	else if(nQuality==1)
	     nIdx = ItemSet.AddGold(nDetail,pnMagicLevel,nSeries,0,nPoint,\
	                                     bYear,bMonth,bDay,bHour,bMin,\
						          pnRMagicLevel,pnBsLevel,nLevel,nLuck);
	if (nIdx <= 0)
		return FALSE;

	if (nIsBang)
		Item[nIdx].SetBang(nIsBang);


    int		nX, nY,nMap;
	POINT	ptLocal;
	KMapPos	Pos;
	        GetMpsPos(&nX,&nY,&nMap);  // 转换后的坐标
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	//////掉下装备保存在KObjItemInfo//////结构体中的参数，以便调用
	KObjItemInfo sInfo;
	memset(&sInfo, 0, sizeof(sInfo));
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	t_sprintf(sInfo.m_szName, Item[nIdx].GetName()); //物品的名称
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
    sInfo.m_sHaveAttack=0;      //是否设置为攻击无效了
	//sInfo.m_AttackerDwid=0;   //上次攻击着的DWID

	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}


	return nIdx;
}

//NPC复活 怪物重生
void KNpc::Revive()  //NPC重生
{	
	//RestoreLiveData();
	RestoreNpcBaseInfo();  //设置基本信息(客户端和 服务器 共调用)
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(m_OriginX, m_OriginY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;

	if (m_RegionIndex < 0)
		return;

	//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);

	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddNpcRef(m_MapX,m_MapY);
	SubWorld[0].NpcChangeRegion(MAP_REGION, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);
	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	m_AiAddLifeTime = 0;
}

void KNpc::RestoreLiveData()
{

}
/////////////////////////////吸血 吸蓝 伤转血///////////////////////////////////
/*void KNpc::RestoreDamage2Life(int nDamage)
{	
	m_CurrentLife = m_CurrentLife + (nDamage*m_CurrentLifeStolen/100);
	if(m_CurrentLife >= m_CurrentLifeMax)
		m_CurrentLife = m_CurrentLifeMax;
}

void KNpc::RestoreDamage2Mana(int nDamage)
{	
	m_CurrentMana = m_CurrentMana + (nDamage*m_CurrentManaStolen/100);
	if(m_CurrentMana >= m_CurrentManaMax)
		m_CurrentMana = m_CurrentManaMax;
}

void KNpc::RestoreDamage2Stamina(int nDamage)
{	
	m_CurrentStamina = m_CurrentStamina + (nDamage*m_CurrentStaminaStolen/100);
	if(m_CurrentStamina >= m_CurrentStaminaMax)
		m_CurrentStamina = m_CurrentStaminaMax;
}*/
////////////////////////////////////////////////////////////////////////////////
#endif

//-------------------------------------------------------------------------
//	功能：设定头顶状态
//-------------------------------------------------------------------------
void	KNpc::SetMenuState(int nState, char *lpszSentence, int nLength)
{
	this->m_DataRes.SetMenuState(nState, lpszSentence, nLength);
}

//-------------------------------------------------------------------------
//	功能：获得头顶状态
//-------------------------------------------------------------------------
int		KNpc::GetMenuState()
{
	return this->m_DataRes.GetMenuState();
}

//-------------------------------------------------------------------------
//	功能：查找周围9个Region中是否有指定 ID 的 npc  返回NPC索引
//-------------------------------------------------------------------------
DWORD	KNpc::SearchAroundID(DWORD dwID)
{
	int		nIdx, nRegionNo;
	nIdx = SubWorld[0].m_Region[m_RegionIndex].SearchNpc(dwID);
	if (nIdx)
		return nIdx;
	for (int i = 0; i < 8; ++i)
	{
		nRegionNo = SubWorld[0].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nIdx = SubWorld[0].m_Region[nRegionNo].SearchNpc(dwID);
		if (nIdx)
			return nIdx;
	}
	return 0;
}

//-------------------------------------------------------------------------
//	功能：设定特殊的只播放一遍的随身spr文件
//-------------------------------------------------------------------------
void	KNpc::SetSpecialSpr(char *lpszSprName)
{
	//m_DataRes.SetSpecialSpr(lpszSprName);
}
//设置循环动态SPR 不能同时显示多SPR
void	KNpc::SetFrameSpr(char *lpszSprName, int nX, int nY, int nHeight,int mInterval) // ve danh hieu ff
{
	//m_DataRes.SetFrameSpr(lpszSprName, nX, nY,nHeight,mInterval);
}

//-------------------------------------------------------------------------
//	功能：设定瞬间特效
//-------------------------------------------------------------------------
void	KNpc::SetInstantSpr(int nNo)
{
	char	szName[FILE_NAME_LENGTH];
	szName[0] = 0;
	NpcSet.m_cInstantSpecial.GetSprName(nNo, szName, sizeof(szName));
	if (szName[0])
		this->SetSpecialSpr(szName);
}

int		KNpc::GetNormalNpcStandDir(int nFrame)
{
	return m_DataRes.GetNormalNpcStandDir(nFrame);
}
//崩溃
void	KNpc::SetNpcState(int* pNpcState)
{
    ClearNpcState();  //释放内存

	if (!pNpcState)
		return ;

	for (int i = 0; i < MAX_NPC_RECORDER_STATE;++i)
	{
		if (*(pNpcState + i) != 0)//(*pNpcState > 0)  //*(pNpcState + i) != 0	 *pNpcState > 0
		{
			KStateNode * pNewNode = new KStateNode;      //新增加的状态
			pNewNode->m_StateGraphics = *(pNpcState + i);//*pNpcState;
			pNewNode->m_IsClientState = 1;               //别人的状态 设置为客户端
			m_StateSkillList.AddTail(pNewNode);          //在末尾添加节点加入客户端的链表
			//*pNpcState++;	                             //赋值后增加
		}
	}

   //m_DataRes.SetState(&m_StateSkillList, &g_NpcResList);  //光环持续状态	
}

//清除所有客户端状态
void KNpc::ClearNpcState()
{
	KStateNode * pNode = (KStateNode*)m_StateSkillList.GetHead();
	KStateNode * pTempNode = NULL;
	
	while(pNode)
	{
		int nIdx = pNode->m_StateGraphics;
		if (g_GameWorld)//删除全部状态精灵
			g_GameWorld->removespriteByIdx(m_Index,nIdx);

		pTempNode = pNode;
		pNode = (KStateNode*) pNode->GetNext();
		pTempNode->Remove();
		delete pTempNode;
        pTempNode=NULL;
	}
	return;
}

//重置恢复NPC的基本信息  重生 或 进入游戏时调用	人与怪物 共用
void	KNpc::RestoreNpcBaseInfo()
{

   int nmPlayerIdx=0;

   if  (m_Kind==kind_player)
   {//如果是玩家
	nmPlayerIdx		= CLIENT_PLAYER_INDEX;
   }
	m_CurrentCamp = m_Camp;
	m_ActiveSkillID = 0;
	m_ActiveSkListIndex=0;
	m_IsMoreAura=false;

	m_ActiveAuraID = 0;
	m_ActiveAuraIndex= 0;
	m_ActiveXinYunXingID = 0;

	m_nPeopleIdx = 0;
	m_nLastDamageIdx = 0;
	m_nLastPoisonDamageIdx = 0;
	m_nLastBurnDamageIdx = 0;
	m_nObjectIdx = 0;

	m_TempFireResist = 0;	                       // Npc的当前火抗性
	m_TempColdResist = 0;	                       // Npc的当前冰抗性
	m_TempPoisonResist = 0;	                       // Npc的当前毒抗性
	m_TempLightResist = 0;	                       // Npc的当前电抗性
	m_TempPhysicsResist = 0;	                   // Npc的当前物理抗性

	m_CurrentFireResist		= m_FireResist;		   //原始的五防
	m_CurrentColdResist		= m_ColdResist;
	m_CurrentPoisonResist	= m_PoisonResist;
	m_CurrentLightResist	= m_LightResist;
	m_CurrentPhysicsResist	= m_PhysicsResist;

  if (m_Kind==kind_player)
  {//如果是玩家
	int nCurbei=0;

	/*int nFomoidx=GetCurFuMoIdx();

	if 	(nFomoidx>-1)
	{
		if (GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	}
	*/
	m_CurrentLifeMax		 = m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//TakeTrader(m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
    m_CurrentManaMax		 = m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//TakeTrader(m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;//脚本基本的生命最大值赋值
	m_CurrentFireResistMax	 = m_FireResistMax+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal(); //200
	m_CurrentColdResistMax	 = m_ColdResistMax+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentPoisonResistMax = m_PoisonResistMax+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentLightResistMax	 = m_LightResistMax+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	m_CurrentPhysicsResistMax= m_PhysicsResistMax+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
  }
  else
  {
	  m_CurrentLifeMax		     = m_LifeMax;
	  m_CurrentManaMax		     = m_ManaMax;//脚本基本的生命最大值赋值
	  m_CurrentFireResistMax	 = m_FireResistMax; //200
	  m_CurrentColdResistMax	 = m_ColdResistMax;
	  m_CurrentPoisonResistMax   = m_PoisonResistMax;
	  m_CurrentLightResistMax	 = m_LightResistMax;
	  m_CurrentPhysicsResistMax  = m_PhysicsResistMax;
  }
    m_CurrentLife			= m_LifeMax;    //脚本基本的生命最大值赋值
	m_CurrentLifeReplenish	= m_LifeReplenish;
	m_CurrentNuQi			= 0;    //设置为零
	m_CurrentLifeDamage     = 0;
	m_CurPoisonDamage       = 0;
	m_CurFireDamage         = 0;
	m_CurrentNuQiMax		= m_NuqiMax;
	m_CurrentNuQiReplenish	= m_NuqiReplenish;

	m_CurrentMana			= m_ManaMax;

	m_CurrentManaReplenish	= m_ManaReplenish;
	m_CurrentStamina		= m_StaminaMax;
	m_CurrentStaminaMax		= m_StaminaMax;
	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;
	
//	memset(&m_CurrentJinMai, 0, sizeof(m_CurrentJinMai));
//	memset(&m_WaiPhysicsDamage, 0, sizeof(m_WaiPhysicsDamage));
//	memset(&m_CurrentFireDamage, 0, sizeof(m_CurrentFireDamage));
//	memset(&m_CurrentColdDamage, 0, sizeof(m_CurrentColdDamage));
//	memset(&m_CurrentLightDamage, 0, sizeof(m_CurrentLightDamage));
//	memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));

	memset(&m_CurrentPhysicsMagicDamageV, 0, sizeof(m_CurrentPhysicsMagicDamageV));
	memset(&m_CurrentPhysicsMagicDamageP, 0, sizeof(m_CurrentPhysicsMagicDamageP));	
	memset(&m_CurrentMagicFireDamage, 0, sizeof(m_CurrentMagicFireDamage));
	memset(&m_CurrentMagicColdDamage, 0, sizeof(m_CurrentMagicColdDamage));
	memset(&m_CurrentMagicLightDamage, 0, sizeof(m_CurrentMagicLightDamage));
	memset(&m_CurrentMagicPoisonDamage, 0, sizeof(m_CurrentMagicPoisonDamage));

	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentDefend			= m_Defend;

	m_CurrentWalkSpeed		= m_WalkSpeed;
	m_CurrentRunSpeed		= m_RunSpeed;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed		= m_CastSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentActiveRadius	= m_ActiveRadius;
	m_CurrentHitRecover		= m_HitRecover;
	m_CurrentTreasure		= m_Treasure;   //掉落装备的数量

	m_CurrentDamage2Mana	= 0;
	//m_CurrentManaPerEnemy	= 0;
	m_CurrentLifeStolen		= 0;
	m_CurrentManaStolen		= 0;
	m_CurrentStaminaStolen	= 0;
	m_CurrentKnockBack		= 0;	
	m_CurrentDeadlyStrike	= 0;
//	m_CurrentBlindEnemy		= 0;
//	m_CurrentPiercePercent	= 0;
	m_CurrentFreezeTimeReducePercent	= 0;
	m_CurrentPoisonTimeReducePercent	= 0;
	m_EnemyPoisonTimeReducePercent      = 0;
	m_CurrentStunTimeReducePercent		= 0;
	m_EnemyStunTimeReducePercent        = 0;
	m_CurrentFireEnhance	= 0;
	m_CurrentColdEnhance	= 0;
	m_CurrentPoisonEnhance	= 0;
	m_CurrentLightEnhance	= 0;
	m_CurrentRangeEnhance	= 0;
	m_CurrentHandEnhance	= 0;
	m_CurrentSerisesEnhance = 0;
	m_CurrentdanggeRate		= 0;  //档格
	m_CurrentzhongjiRate	= 0;  //
    m_CurrentcjdanggeRate	= 0;                 //拆解档格
    m_CurrentcjzhongjiRate	= 0;                //拆解重击
    m_Currentsorbdamage	 = 0;                   //抵消伤害
	m_Currentsorbdamage_v=0;
	m_Currenadddamagev   = 0;
	m_Currenadddamagep   = 0;
    m_Currentpoisonres	= 0;                    //=忽略对方毒防:#d1-%
    m_Currentfireres	= 0;                      //=忽略对方火防:#d1-%
    m_Currentlightingres	= 0;                  //=忽略对方雷防:#d1-%
    m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
    m_Currentcoldres	= 0;                   //=忽略对方冰防:#d1-%
	m_Currentallres     =0;                    //=忽略对方全抗:#d1-%      
    m_Currentnopkvalue	= 0;                    //=不增加PK值概率:#d1+%
    m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
    m_Currentelementsenhance	= 0;              //=五行强化值：#d1-点。强化对相克五行的克制效果
    m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	m_Currentskillenhance=0;
//	ZeroMemory(m_CurrentSkillEnhance,sizeof(m_CurrentSkillEnhance));
	/*_EnhanceInfo::iterator it;
	for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	{
		it->second.nSkillIdx = 0;
		it->second.nEnhance = 0;
	}*/
	nEnhanceInfo.clear();
	m_CurrentFullManaskillenhance=0;
	m_CurrentUpExp=0;
	m_CurrentautoReviverate=0;                 //复活概率
    m_CurrentCreatnpcv=0;
    m_CurrentAllJiHuo=0;                       //是否全身激活
	m_CurrentCreatStatus=0;
    m_CurrentAddPhysicsDamageP= 0;     // 当前被动外普百分比
    m_CurrentAddFireDamagev= 0;        // 当前被动外火点
    m_CurrentAddColdDamagev= 0;        // 当前被动外冰点 
    m_CurrentAddLighDamagev= 0;        // 当前被动外雷点
    m_CurrentAddPoisonDamagev= 0;      // 当前被动外毒点
	
    m_CurrentAddmagicphysicsDamage= 0; // 当前被动内普点
    m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
    m_CurrentAddmagicColdDamagicv= 0;  // 当前被动内冰点
    m_CurrentAddmagicFireDamagicv= 0;  // 当前被动内火点  
	m_CurrentAddmagicLightDamagicv= 0; // 当前被动内雷点
    m_CurrentAddmagicPoisonDamagicv= 0;// 当前被动内毒点
	m_Currentbaopoisondmax_p=0;
	m_CurrentPoisondamagereturnV=0;
	m_CurrentPoisondamagereturnP=0;
	m_CurrentReturnskillp=0;
	m_CurrentIgnoreskillp=0;
	m_CurrentReturnresp=0;
    m_CurrentAttackRatingEnhancep=0;
	m_CurrentAttackRatingEnhancev=0;
	m_CurrentIgnorenAttacRating  =0;

	m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
	m_CurrentStunRank_p=0;
	m_Staticmagicshield_p=0;
	ZeroMemory(m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));
	ClearStateSkillEffect();
	ClearNormalState();

}
//描绘NPC边框
void KNpc::DrawBorder()
{
	if (m_Index <= 0)
		return;
	//m_DataRes.DrawBorder();
}

int KNpc::DrawMenuState(int n)
{
	if (m_Index <= 0) //有状态才绘画)
		return n;

	if (GetMenuState())
	{  
		isRemoveMenu = false;
	   return  m_DataRes.DrawMenuState(n,m_Index);
	}
	else
	{//没有状态
		if (g_GameWorld && !isRemoveMenu)
		{
			isRemoveMenu = true; //设置已经删除过了
			g_GameWorld->removeMenuByIdx(m_Index,0);
		}

		return n;
	}
}
//动物 冒血处理  绘制血条
void KNpc::DrawBlood()
{
	if (m_Kind != kind_normal) 
		return;

	int nFontSize = 12;
	int nHeightOff = GetNpcPate();
	//if (NpcSet.CheckShowLife())
	{
		nHeightOff = PaintLife(nHeightOff, true);
		nHeightOff += SHOW_SPACE_HEIGHT;
	}
	//if (NpcSet.CheckShowName())
    nHeightOff = PaintInfo(nHeightOff,true,nFontSize,0XFF000000);  //选 动物 NPC中后 的显示

}

 //开始跨服处理
/*#ifdef _SERVER
void KNpc::TobeExchangeServer(DWORD dwMapID, int nX, int nY)
{
	if (!IsPlayer())
	{
		return;
	}
	m_OldFightMode = m_FightMode;
	m_bExchangeServer = TRUE;  //正在跨服
	if (m_nPlayerIdx > 0 && m_nPlayerIdx <= MAX_PLAYER)
	{
		Player[m_nPlayerIdx].TobeExchangeServer(dwMapID, nX, nY);
	}
}
#endif */
/*
int KNpc::GetCurFuMoIdx()
{//当前使用神将编号
	for (int i=0;i<MAX_FUMO_COUNT;++i)
	{
	    if (m_nFuMoNum[i].nNpcSetings>1)
			return i;
	} 
	return -1;
} 

int KNpc::GetCurFuMoNpcNo(int idx)
{//当前使用神将编号
	if (idx<=-1 || idx>MAX_FUMO_COUNT)
		return 0;

	return	m_nFuMoNum[idx].nNpcSetings;
}


int KNpc::GetCurFoMoSkllLevel(int idx)
{//当前的神将技能等级
    if (idx<=-1 || idx>MAX_FUMO_COUNT)
		return 0;

	return m_nFuMoNum[idx].nSkillLevel;
}
*/

//获取人物最大和最小最终伤害
//#ifndef _SERVER
void KNpc::GetTongBanDamage(int* nMin, int* nMax)
{
	int nSkillId;
	*nMin = 0;
	*nMax = 0;
	
	if (NULL == nMin || NULL == nMax)
		return;
	//左右技能选择

    nSkillId = m_ActiveSkillID;
	int nListidx = m_SkillList.FindSame(nSkillId);
	int nLevel = m_SkillList.GetCurrentLevelByIdx(nListidx);
	int nEnChance = m_SkillList.GetEnChance(nListidx);

	if (nSkillId <= 0 || nLevel <= 0 || nLevel >= MAX_SKILLLEVEL)
		return;
	
	KMagicAttrib*	pMagicData = NULL;
	
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nLevel);
	if (!pOrdinSkill) 
        return ;
 /*
	switch(pOrdinSkill->GetSkillStyle())
	{
	case SKILL_SS_Missles:		        	//	子弹类		本技能用于发送子弹类
        break;
	case SKILL_SS_Melee:                    //
	    break;
	case SKILL_SS_InitiativeNpcState:	    //	主动类		本技能用于改变当前Npc的主动状态
	    break;
	case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
		{

		}
		break;
	default:
		return;
	} */
	
	BOOL	bIsPhysical = pOrdinSkill->IsPhysical();  //物理攻击
	int 	bIsMagic = pOrdinSkill->IsMagic();        //1内功系 或0外攻系

	pMagicData = pOrdinSkill->GetDamageAttribs();     //攻击技能数据
	
	if (!pMagicData)
		return;
	//力量伤害+武器基本伤害+普点=基本的伤害
	int nMinNpcDamage = m_PhysicsDamage.nValue[0]+m_CurrentPhysicsMagicDamageV.nValue[0];
	int nMaxNpcDamage = m_PhysicsDamage.nValue[2]+m_CurrentPhysicsMagicDamageV.nValue[2];
 	
	int nEnhancew=0;
/*
	if (equip_meleeweapon == m_ItemList.GetWeaponType())
	{
		nEnhancew = m_CurrentMeleeEnhance[m_ItemList.GetWeaponParticular()];
	}
	else if (equip_rangeweapon == m_ItemList.GetWeaponType())
	{
		nEnhancew = m_CurrentRangeEnhance;
	}
	else 
	{
		nEnhancew = m_CurrentHandEnhance;
	} */
//基本伤害
	*nMin += nMinNpcDamage +(nMinNpcDamage*nEnhancew)/100;
	*nMax += nMaxNpcDamage +(nMinNpcDamage*nEnhancew)/100;
	
	//命中率
	pMagicData++;//1 //闪避率
	pMagicData++; //2 普点伤害+普百分比伤害
	if (magic_physicsenhance_p == pMagicData->nAttribType)
	{
		int Vmin=0,Vmax=0;

		    Vmin = (*nMin)*pMagicData->nValue[0]/100;
            Vmax = (*nMax)*pMagicData->nValue[0]/100;

		*nMin += Vmin;
		*nMax += Vmax;	

	//*nMin += nMinNpcDamage * nEnhancew / 100;
	//*nMax += nMaxNpcDamage * nEnhancew / 100;

	if (bIsMagic)
	{//脚本基本数据+内普状态(装备+技能的状态)
		*nMin = (*nMin+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
		*nMax = (*nMax+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
	}
	else
	{//脚本基本数据+外普状态(装备+技能的状态)
		*nMin = (*nMin+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
		*nMax = (*nMax+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
	} 
	}
	pMagicData++; // 3 冰伤害
	if (magic_colddamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2];//*(100+ m_CurrentColdEnhance)/100;	

	if (bIsMagic)  //内冰
	{
		*nMin += m_CurrentMagicColdDamage.nValue[0]+m_CurrentAddmagicColdDamagicv;
		*nMax += m_CurrentMagicColdDamage.nValue[2]+m_CurrentAddmagicColdDamagicv;
	}
	else
	{ 
		*nMin += m_CurrentMagicColdDamage.nValue[0]+m_CurrentAddColdDamagev;
		*nMax += m_CurrentMagicColdDamage.nValue[2]+m_CurrentAddColdDamagev;
	} 
	}
	pMagicData++; //4  火伤害
	if (magic_firedamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2]*(100 + m_CurrentFireEnhance)/ 100;
	if (bIsMagic)
	{
		*nMin += m_CurrentMagicFireDamage.nValue[0]+m_CurrentAddmagicFireDamagicv;
		*nMax += m_CurrentMagicFireDamage.nValue[2]+m_CurrentAddmagicFireDamagicv;
	}
	else
	{ 
		*nMin += m_CurrentMagicFireDamage.nValue[0]+m_CurrentAddFireDamagev;
		*nMax += m_CurrentMagicFireDamage.nValue[2]+m_CurrentAddFireDamagev;
	} 
	}
	pMagicData++;// 5 雷伤害
	if (magic_lightingdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2]; 

	if (bIsMagic)
	{
		*nMin += m_CurrentMagicLightDamage.nValue[0]+m_CurrentAddmagicLightDamagicv;
		*nMax += m_CurrentMagicLightDamage.nValue[2]+m_CurrentAddmagicLightDamagicv;
	}
	else
	{ 
		*nMin += m_CurrentMagicLightDamage.nValue[0]+m_CurrentAddLighDamagev;
		*nMax += m_CurrentMagicLightDamage.nValue[2]+m_CurrentAddLighDamagev;
	}
	}
	pMagicData++; // 6 毒伤害
	if (magic_poisondamage_v == pMagicData->nAttribType)
	{
			*nMin += pMagicData->nValue[0];//* (100 +m_CurrentPoisonEnhance) / 100;//技能基本数据
			*nMax += pMagicData->nValue[0]; 

	  if (bIsMagic)
	  { 
		*nMin += m_CurrentMagicPoisonDamage.nValue[0]+m_CurrentAddmagicPoisonDamagicv;
		*nMax += m_CurrentMagicPoisonDamage.nValue[0]+m_CurrentAddmagicPoisonDamagicv;
	  } 
	  else
	  {  
		int nPoisonDamage = m_CurrentMagicPoisonDamage.nValue[0]+m_CurrentAddPoisonDamagev;
		*nMin += nPoisonDamage;
		*nMax += nPoisonDamage;
	  } 
	}
	pMagicData++;// 7 穿刺伤害（无视防御）

	if (magic_magicdamage_v == pMagicData->nAttribType)
	{ 
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2];
	}
	pMagicData++;//8
	pMagicData++;//9
	pMagicData++;//10
	pMagicData++;//11
	pMagicData++;//12
	pMagicData++;//13
	pMagicData++;//14
	pMagicData++;//15
	if (magic_physicsdamage_v == pMagicData->nAttribType) //普点伤害 点
	{
		//*nMin += pMagicData->nValue[0];
		//*nMax += pMagicData->nValue[2];
      if (bIsMagic)
	  {//脚本基本数据+内普状态(装备+技能的状态)
		*nMin += (pMagicData->nValue[0]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
		*nMax += (pMagicData->nValue[2]+m_CurrentAddmagicphysicsDamage)*(100+m_CurrentAddmagicphysicsDamageP)/100;
	  }
	  else
	  {//脚本基本数据+外普状态(装备+技能的状态)
		*nMin += (pMagicData->nValue[0]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
		*nMax += (pMagicData->nValue[2]+m_CurrentAddPhysicsDamage)*(100+m_CurrentAddPhysicsDamageP)/100;
	  } 
    }
	pMagicData++;//16
	pMagicData++;//17
	pMagicData++;//18
	
/*	int nFomoidx=GetCurFuMoIdx(),nCurbei=0;
	
	if 	(nFomoidx>-1)
	{
		if (GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	}
	*/
	*nMin += *nMin * nEnChance/100;
	*nMax += *nMax * nEnChance/100; 

//	*nMin = *nMin * (nCurbei*5+100)/100;
//  *nMax = *nMax * (nCurbei*5+100)/100;
}





BOOL KNpc::IsPlayer()
{
	return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex; //客户端 NPC索引
}

// 清除NPC身上的非被动类的技能状态
void KNpc::ClearStateSkillEffect()
{
	KStateNode* pNode;
	          pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetNext();

		if (pTempNode->m_LeftTime == -1)	// 无时间限制技能
			continue;

		if (pTempNode->m_LeftTime > 0)
		//if (pTempNode->m_LeftTime == 0 && pTempNode->m_IsClientState==0)    //第几个状态时间
		{

			if  (nExpSkillIdx!=0 && pTempNode->m_SkillID == nExpSkillIdx)
			{//备份双倍经验时间
                 continue;
			}

		    if  (nLuckySkillIdx!=0 && pTempNode->m_SkillID == nLuckySkillIdx)
			{//备份双倍幸运时间
                continue;
			}

			if  (nKangkillIdx!=0 && pTempNode->m_SkillID == nKangkillIdx)
			{//备份全抗时间
                continue;
			}
			if  (nYaoPinSkillIdx!=0 && pTempNode->m_SkillID == nYaoPinSkillIdx)
			{//备份长效药品时间
                continue;
			}
	
			for (int i = 0; i < MAX_SKILL_STATE; ++i)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}
			int nIdx = pTempNode->m_StateGraphics;
			//_ASSERT(pTempNode != NULL);
			if (pTempNode != NULL)
			{
		       pTempNode->Remove();
			   delete pTempNode;
			   pTempNode = NULL;
			   if (g_GameWorld)//删除状态精灵
				   g_GameWorld->removespriteByIdx(m_Index,nIdx);
			}
			//pTempNode = NULL;
			continue;
		}
	}
}

void KNpc::ClearNormalState() //释放内存数据 清空内存数据
{

	/*_KStateNote::iterator it;
	for( it = nKStateNote.begin(); it != nKStateNote.end(); ++it)
	{
		it->second.nMagicAttrib = 0;
		it->second.nTime = 0;
		it->second.nValue[0] = 0;
		it->second.nValue[1] = 0;
	 }*/
	//nKStateNote.clear();

	ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));
	ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));
	ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));
	ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));
	ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));
	ZeroMemory(&m_ManaShield, sizeof(m_ManaShield));
	ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));
	ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));
	ZeroMemory(&m_BurnState, sizeof(m_BurnState));
	ZeroMemory(&m_ConfuseState, sizeof(m_ConfuseState));
	ZeroMemory(&m_StunState, sizeof(m_StunState));
	ZeroMemory(&m_LifeState, sizeof(m_LifeState));
	ZeroMemory(&m_ManaState, sizeof(m_ManaState));
	ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
	ZeroMemory(&m_Hide,sizeof(m_Hide));
	ZeroMemory(&m_LoseMana,sizeof(m_LoseMana));
	ZeroMemory(&m_ZhuaState,sizeof(m_ZhuaState));
	ZeroMemory(&m_Returnskill,sizeof(m_Returnskill));
	ZeroMemory(&m_ExpState, sizeof(m_ExpState));
	ZeroMemory(&m_DoScriptState, sizeof(m_DoScriptState));
    ZeroMemory(&m_randmove, sizeof(m_randmove));
	ZeroMemory(&m_Deathkill, sizeof(m_Deathkill));
	ZeroMemory(&m_Rescueskill, sizeof(m_Rescueskill));
	ZeroMemory(&m_Replyskill, sizeof(m_Replyskill));
}
//服务器端执行
void KNpc::CheckTrap()
{
	if (m_Kind != kind_player)
		return;
	
	if (m_Index <= 0)
		return;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return;

	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
/*
#ifdef _SERVER
	char msg[64];
	     t_sprintf(msg,"当前坐标:R:%d,X:%d,Y:%d,Tr:%d",m_RegionIndex,m_MapX,m_MapY,dwTrap);
    Player[m_nPlayerIdx].m_ItemList.msgshow(msg);
#endif
*/
	if (m_TrapScriptID == dwTrap)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
	}

	if (!m_TrapScriptID)
	{
		return;
	}

	Player[m_nPlayerIdx].ExecuteScriptD(m_TrapScriptID, "main",0,"",0,0,0,0,0,FALSE);
}

void KNpc::SetFightMode(BOOL bFightMode)
{
	m_FightMode = bFightMode;

}

void KNpc::SetBayBan(BOOL bBayBan)
{
	m_BayBan = bBayBan;
}

void KNpc::setNpcDir(int nX2,int nY2)
{
	if (!m_Index)
		return;
	int nX1, nY1,nMap1;
	GetMpsPos(&nX1, &nY1,&nMap1);
	m_Dir = g_GetDirIndex(nX1, nY1, nX2, nY2); //获取方向
}

void KNpc::TurnTo(int nIdx)
{
	if (!Npc[nIdx].m_Index || !m_Index)
		return;

	int nX1, nY1,nMap1, nX2, nY2,nMap2;

	GetMpsPos(&nX1, &nY1,&nMap1);
	Npc[nIdx].GetMpsPos(&nX2, &nY2,&nMap2);

	m_Dir = g_GetDirIndex(nX1, nY1, nX2, nY2); //获取方向
}
//状态的计算
void KNpc::ReCalcStateEffect()
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetHead();
	while(pNode)
	{
		if (pNode->m_LeftTime != 0)	// 包括被动(-1)和主动(>0)
		{
			int i;
			for (i = 0; i < MAX_SKILL_STATE; ++i)
			{
				if (pNode->m_State[i].nAttribType)
				{
					KMagicAttrib	nMagicAttrib;
					nMagicAttrib.nAttribType = pNode->m_State[i].nAttribType;
					nMagicAttrib.nValue[0]   = -pNode->m_State[i].nValue[0];
					nMagicAttrib.nValue[1]   = -pNode->m_State[i].nValue[1];
					nMagicAttrib.nValue[2]   = -pNode->m_State[i].nValue[2];
					//改变NPC属性
					ModifyAttrib(m_Index, &nMagicAttrib);
				}
			}
		}
		pNode = (KStateNode *)pNode->GetNext();
	}
}

extern KTabFile g_ClientWeaponSkillTabFile;

int		KNpc::GetCurActiveWeaponSkill()
{
	int nSkillId = 0;
	if (IsPlayer())
	{//玩家
		
		int nDetailType = Player[m_nPlayerIdx].m_ItemList.GetWeaponType();
		int nParticularType = Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular();
		
		//近身武器
		if (nDetailType == 0)
		{
			nSkillId = g_nMeleeWeaponSkill[nParticularType];
		}//远程武器
		else if (nDetailType == 1)
		{
			nSkillId = g_nRangeWeaponSkill[nParticularType];
		}//空手
		else if (nDetailType == -1)
		{
			nSkillId = g_nHandSkill;
		}
	}
	else
	{//怪物
		if (m_Kind == kind_player)
		{
			g_ClientWeaponSkillTabFile.GetInteger(m_WeaponType + 1, "SkillId", 0, &nSkillId);
		}
		else						//Real Npc
		{
			return 0;//
		}
	}
	return nSkillId;
}
//断魂刺？瞬移
void	KNpc::HurtAutoMove()
{
	if (this->m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		return;
	if (this->m_Doing != do_hurt)
		return;
	if (m_sSyncPos.m_nDoing != do_hurt && m_sSyncPos.m_nDoing != do_stand)
		return;

	int	nFrames, nRegionIdx;

	nFrames = m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
	if (nFrames <= 1)
	{
		if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
			//SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY);
		}
		else
		{
			nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
			if (nRegionIdx < 0)
				return;
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
		}
	}
	else
	{
		nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
		if (nRegionIdx < 0)
			return;
		int		nNpcX, nNpcY, nSyncX, nSyncY;
		int		nNewX, nNewY, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].NewMap2Mps(m_RegionIndex, 
			m_MapX, m_MapY,
			m_OffX, m_OffY,
			&nNpcX, &nNpcY);
		SubWorld[0].NewMap2Mps(nRegionIdx, 
			m_sSyncPos.m_nMapX, m_sSyncPos.m_nMapY,
			m_sSyncPos.m_nOffX, m_sSyncPos.m_nOffY,
			&nSyncX, &nSyncY);
		nNewX = nNpcX + (nSyncX - nNpcX) / nFrames;
		nNewY = nNpcY + (nSyncY - nNpcY) / nFrames;
		SubWorld[0].Mps2Map(nNewX, nNewY, &nRegionIdx, &nMapX, &nMapY, &nOffX, &nOffY);
		//_ASSERT(nRegionIdx >= 0);
		if (nRegionIdx < 0)
			return;
		if (nRegionIdx != m_RegionIndex)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
		}
		else
		{
			SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY);
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
			//SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY);
		}
	}
}

//客户端
void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)
{
	switch (cmd)
	{
	case do_death:   
		{
		DoDeath();
		//SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个死亡的瞬间特效
		}
		break;
	case do_hurt:
		DoHurt(x, y, z);
		break;
	case do_revive:  //站着？
		{
		DoStand();
		m_ProcessAI    = 1;
		m_ProcessState = 1;
		//IsDeath=0;
		SetInstantSpr(enumINSTANT_STATE_REVIVE); //释放一个重生的瞬间特效	
		}
		break;
	case do_stand:
		{
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		}
	    break;
	default:
		break;
	}
}
/*
#ifndef _SERVER
void	KNpc::ClearBlood(int i)
{
	m_nBloodNo[i]		= 0;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= 0;
	m_szBloodNo[i][0]	= 0;
}
#endif


#ifndef _SERVER
void	KNpc::SetBlood(int nNo)
{
	if (nNo <= 0)
		return;
	for (int i = 0;i < defMAX_SHOW_BLOOD_NUM;i++)
	{
		if (!m_szBloodNo[i][0])
			break;
	}
	if (i == defMAX_SHOW_BLOOD_NUM) i = 0;
	m_nBloodNo[i]		= nNo;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= defMAX_SHOW_BLOOD_TIME;
	t_sprintf(m_szBloodNo[i], "%d", nNo);
}
#endif

#ifndef _SERVER
int	KNpc::PaintBlood(int nHeightOffset)
{
	for (int i = 0;i < defMAX_SHOW_BLOOD_NUM;i++)
	{
		if (!m_szBloodNo[i][0])
			continue;
		
		int	nHeightOff = nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * defSHOW_BLOOD_MOVE_SPEED;
		int nFontSize = 14;
		DWORD	dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha[i] << 24);
		int		nMpsX, nMpsY,nMapID;
		GetMpsPos(&nMpsX,&nMpsY,&nMapID);
		g_pRepresent->OutputText(nFontSize, m_szBloodNo[i], KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(m_szBloodNo[i]) / 4, nMpsY, dwColor, 0, nHeightOff);
		
		m_nBloodTime[i]--;
		if (m_nBloodTime[i] <= 0)
		{
			ClearBlood(i);
			continue;
		}
		m_nBloodAlpha[i]++;
		if (m_nBloodAlpha[i] > 31)
			m_nBloodAlpha[i] = 31;
	}
	
	return nHeightOffset;
}
#endif
*/

void	KNpc::SetBlood(int nNo)
{	
		if (nNo <= 0)
			return;

		BOOL flag = FALSE;
		int i = 0;
		for (i; i< defMAX_SHOW_BLOOD_NUM;++i)
		{
			 if (m_nBloodNo[i][0] == 0) 
			{//空伤害值
				 flag = TRUE;
				 //m_nBloodNo[i][0] = nNo;                   //赋值给 绘制生命数值？
			     //m_nBloodNo[i][1] = defMAX_SHOW_BLOOD_TIME;//间隔时间 还是距离/?	
				 break;
			}
		}
		
		if (flag == TRUE)
		{//有空的就显示这个
			m_nBloodNo[i][0] = nNo;                   //赋值给 绘制生命数值？
			m_nBloodNo[i][1] = defMAX_SHOW_BLOOD_TIME;//间隔时间 还是距离/?	
		}
		else
		{//否则 就显示在第一个
			 m_nBloodNo[0][0] = nNo;
			 m_nBloodNo[0][1] = defMAX_SHOW_BLOOD_TIME;
		}
}

int	KNpc::PaintOther()
{
	if (g_GameWorld)
		g_GameWorld->DrawPrimitives_NpcOther(m_Index,m_Kind,false);
	return true;
}

//冒血数字显示
int	KNpc::PaintBlood(int nHeightOffset)
{	
	int nHeight = GetNpcPate();

	if  (IsPlayer() || m_Kind==kind_dialoger)
		return false;

	nHeightOffset = nHeight/2;

	 BOOL _flag = FALSE;

	 for (int j = 0; j< defMAX_SHOW_BLOOD_NUM; ++j)
	 {
		 if (m_nBloodNo[j][0] > 0)
		{
			_flag = TRUE;
			break;
		}
	 }

	 if (_flag == FALSE )
	 {//如果没有冒血的 就要删除父亲节点精灵
		 if (g_GameWorld)
			 g_GameWorld->DrawPrimitives_NpcBlood(m_Index,-1,NULL,0,0,0,true);

		return nHeightOffset;
	 }
	int nFontSize = 16;
	int  nMpsX, nMpsY,nMmap;
	      GetMpsPos(&nMpsX, &nMpsY,&nMmap);

	int nHeightOff = nHeightOffset;

	for (int i = 0; i < defMAX_SHOW_BLOOD_NUM; ++i)
	{
		if (m_nBloodNo[i][0] > 0)
		{//血量大于0
			 nHeightOff = nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodNo[i][1]) * defSHOW_BLOOD_MOVE_SPEED;
			 t_sprintf(m_szBloodNo,"%d",m_nBloodNo[i][0]); //冒血数字显示
			 if (g_GameWorld)
				 g_GameWorld->DrawPrimitives_NpcBlood(m_Index,i,m_szBloodNo,nMpsX,nMpsY,nHeightOff,false); // - nFontSize * g_StrLen(m_szBloodNo)/4

			 m_nBloodNo[i][1]--; //时间减1

			 if (m_nBloodNo[i][1] <=0)
			 {//清空精灵
				 if (g_GameWorld)
					 g_GameWorld->DrawPrimitives_NpcBlood(m_Index,i,NULL,0,0,0,true);

				 m_nBloodNo[i][0] = 0;
				 m_nBloodNo[i][1] = 0;
				// m_nBloodAlpha	= 0;
				 m_szBloodNo[0]	= 0;
			 }
		 }
	}

	return nHeightOff;
}

//要求同步NPC信息
void KNpc::ApplySynNpcInfo(DWORD nNpcDwid)
{
	NPC_SYN_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_synnpcinfo;
	sApplyInfo.nNpcDwid=nNpcDwid;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(NPC_SYN_INFO));
}


int	KNpc::PaintNewBlood(int nHeightOffset,int Val)
{	
	int nFontSize = 12;
	//DWORD dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha << 24); //颜色
	//DWORD dwColor = SHOW_BLOOD_COLOR;
     DWORD	dwColor=TGetColor("255,255,255");
	 int  nMpsX, nMpsY,nMmap;
	      GetMpsPos(&nMpsX, &nMpsY,&nMmap);
	 char nNewBlood[32];
	int nHeightOff = nHeightOffset;
			 t_sprintf(nNewBlood, "%d",Val); //冒血数字显示 
//			 g_pRepresent->OutputText(nFontSize, nNewBlood, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(nNewBlood) / 4, nMpsY, dwColor, 0, nHeightOff,0XFF000000);
			 
	//m_nBloodAlpha++;
	//if (m_nBloodAlpha > 31)
	//	m_nBloodAlpha = 31;

	return nHeightOff;
}
//高度调整
int	KNpc::GetNpcPate()
{
	int nHeight = m_Height + m_nStature;

	if (m_Kind == kind_player)
	{
		if (m_nSex)
			nHeight += 80;	//女
		else
			nHeight += 80;	//男

		if (m_MaskType == 0)
		{//没带面具的
			if (m_Doing == do_sit &&(10*m_Frames.nCurrentFrame/m_Frames.nTotalFrame) >= 8)
				nHeight -= (30*m_Frames.nCurrentFrame/m_Frames.nTotalFrame);
			
			if (m_bRideHorse)
				nHeight += 20;	//骑马
		}
		else
		{//带面具
			nHeight += 20;
		}
		
	}
	return nHeight;
}
//生命条显示
int	KNpc::GetNpcPatePeopleInfo()
{
	int nFontSize = 18;
	if (m_nChatContentLen > 0 && m_nChatNumLine > 0)
		return m_nChatNumLine * (nFontSize + 1);

	int nHeight = 0;
	if (NpcSet.CheckShowLife())
	{
		if (m_Kind == kind_player || m_Kind == kind_partner)
		{
			if (m_CurrentLifeMax > 0 &&\
				(relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
				)
				nHeight += SHOW_LIFE_HEIGHT;
		}
	}
	if (NpcSet.CheckShowName())
	{
		if (nHeight != 0)
			nHeight += SHOW_SPACE_HEIGHT;//好看

		if (m_Kind == kind_player || m_Kind == kind_dialoger)
			nHeight += nFontSize + 1;
	}
	return nHeight;
}

void KNpc::GetFrameCopyFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel < 1 ) 
		return ;
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
	//if (g_pNpcTemplate[nNpcTemplateId][nLevel])        //数据有效则拷贝，否则重新生成
		LoadFrameFromTemplate(pNpcTemp);//(nNpcTemplateId, nLevel); //从模板获取数据
	/*else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0])
		{
			g_pNpcTemplate[nNpcTemplateId][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0]->InitNpcBaseData(nNpcTemplateId);  //NPC基本
			g_pNpcTemplate[nNpcTemplateId][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0]->m_bHaveLoadedFromTemplate = TRUE;
		}

		KLuaScript * pLevelScript = NULL;		
		
#ifdef _SERVER	 //g_pNpcTemplate[nNpcTemplateId][0]
		pLevelScript = (KLuaScript*)g_GetScript(
			g_pNpcTemplate[nNpcTemplateId][0]->m_dwLevelSettingScript
			);
		
		if (pLevelScript == NULL)
			pLevelScript = g_pNpcLevelScript;
#else
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			LevelScript.Init();
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript))
			{
				//g_DebugLog ("[error]致命错误,无法正确读取[%s]", g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript);
				//_ASSERT(0);
				pLevelScript = g_pNpcLevelScript;
				LevelScript.Exit();
			}
			else
				pLevelScript = &LevelScript;
		}
		
#endif
		g_pNpcTemplate[nNpcTemplateId][nLevel] = new KNpcTemplate;
		*g_pNpcTemplate[nNpcTemplateId][nLevel] = *g_pNpcTemplate[nNpcTemplateId][0];
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_nLevel = nLevel;
		g_pNpcTemplate[nNpcTemplateId][nLevel]->InitNpcLevelData(nNpcTemplateId, pLevelScript, nLevel);
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_bHaveLoadedFromTemplate = TRUE;
		LoadFrameFromTemplate(nNpcTemplateId,nLevel);
	   }
	 */

}
//面具变形
void	KNpc::ReSetRes(int nMark)
{
	char	szNpcTypeName[32];
	if (nMark == 1)
	{//恢复原来的样子
	  if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || m_NpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	  { //如果是人物的话
		if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "MainMan");  //"男主角"
			/*strcpy(szNpcTypeName, "ani018");*/
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame  = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame   = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "MainLady"); //"女主角"
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame  = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame   = NpcSet.GetPlayerRunFrame(FALSE);
		}
		m_WalkSpeed   = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed    = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	  = NpcSet.GetPlayerHurtFrame();

	  }
	  else
	  {//普通npc 怪物
			GetNpcCopyFromTemplate(m_NpcSettingIdx,m_Level);  //拷贝 NPCs.TXT 里面的基本信息！！			
			g_NpcSetting.GetString(m_NpcSettingIdx + 2, "NpcResType", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));
			if (!szNpcTypeName[0])
			{ //人物类型表 npcres
				g_NpcKindFile.GetString(2, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
			} 
	  }
	  m_DataRes.Remove(m_Index);                //清除残影  m_Index
	  m_DataRes.Init(szNpcTypeName,&g_NpcResList,m_Index);
	  m_DataRes.SetAction(m_ClientDoing);
	  m_DataRes.SetRideHorse(m_bRideHorse);
	  m_DataRes.SetArmor(m_ArmorType);
	  m_DataRes.SetHelm(m_HelmType);
	  m_DataRes.SetHorse(m_HorseType);
	  m_DataRes.SetWeapon(m_WeaponType);
	  m_DataRes.SetPifeng(m_PifengType);
	  m_DataRes.SetChiBang(m_ChiBangType);
	  if (g_GameWorld)
		  g_GameWorld->setBuWeiHide(m_Index,true,true);
	}
	else
	{//否则就变形
		GetFrameCopyFromTemplate(m_MaskType,m_Level);	//获取NPC的样子 数据 =m_MaskType
		g_NpcSetting.GetString(m_MaskType + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));	// 面具
		
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(4, "人物名称", "enemy003", szNpcTypeName, sizeof(szNpcTypeName));//如果没找到，用第一个npc代替
		}
		m_DataRes.Remove(m_Index);               //清除残影 m_Index
		m_DataRes.Init(szNpcTypeName,&g_NpcResList,m_Index);
		m_DataRes.SetAction(m_ClientDoing);

		if (g_GameWorld)
			g_GameWorld->setBuWeiHide(m_Index,false,true);
		/*m_DataRes.SetRideHorse(0);
		m_DataRes.SetArmor(m_ArmorType);
		m_DataRes.SetHelm(m_HelmType);
		m_DataRes.SetHorse(0);
		m_DataRes.SetWeapon(0);
		m_DataRes.SetPifeng(0);
		m_DataRes.SetChiBang(0);*/
	}
}
//---------------

void	KNpc::LoadFrameFromTemplate(VOID* nNpcTemp)//(int nNpcTemplateId, int nLevel)
{
	/*if (nNpcTemplateId < 0 )
	{
		return ;
	}
	
	KNpcTemplate* pNpcTemp = NULL;
	if (pNpcTemp = NpcSet.GetTemplate(nNpcTemplateId, nLevel))
   */
	//KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel];	  //NpcSet.GetTemplate(nNpcTemplateId, nLevel);
	KNpcTemplate* pNpcTemp =NULL;
	              pNpcTemp  =  (KNpcTemplate *)nNpcTemp;

	if (!pNpcTemp)
		return ;

	m_HeadImage        =	pNpcTemp->m_HeadImage;
	m_CorpseSettingIdx =	pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame       =	pNpcTemp->m_DeathFrame;
	m_WalkFrame        =	pNpcTemp->m_WalkFrame;
	m_RunFrame         =	pNpcTemp->m_RunFrame;
	m_HurtFrame        =	pNpcTemp->m_HurtFrame;
	//m_AttackFrame      =	pNpcTemp->m_AttackFrame;
	//m_CastFrame        =	pNpcTemp->m_CastFrame;
	m_StandFrame       =    pNpcTemp->m_StandFrame;
	m_StandFrame1      =    pNpcTemp->m_StandFrame1;
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
 // m_PifengType		    = pNpcTemp->m_PifengType; //披风
//	m_HorseType				= pNpcTemp->m_HorseType;
//	m_bRideHorse			= pNpcTemp->m_bRideHorse;	
}
//移动Npc 
BOOL KNpc::DoSecMove(int nDistance)
{
	WaitForFrame();
	if (nDistance <= 0)
		return FALSE;

	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		//g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		DoStand();
		return FALSE;
	}
	int x, y;

	SubWorld[m_SubWorldIndex].NewMap2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
	int nDis = SubWorld[m_SubWorldIndex].GetDistance(x,y,m_DesX,m_DesY);

	if (nDis > nDistance)
	{
		if (m_JumpDir >= MaxMissleDir)
			m_JumpDir -= MaxMissleDir;

		x = m_DesX - (g_DirCos(m_JumpDir, MaxMissleDir) * (nDis - nDistance) >> 10);
		y = m_DesY - (g_DirSin(m_JumpDir, MaxMissleDir) * (nDis - nDistance) >> 10);
	}
	else
	{
		x = m_DesX;
		y = m_DesY;
	}

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	
	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
	
	if (!m_bClientOnly)
		CURREGION.DecNpcRef(m_MapX, m_MapY);

	SubWorld[m_SubWorldIndex].Mps2Map(x,y,&m_RegionIndex,&m_MapX,&m_MapY,&m_OffX,&m_OffY);

	if (!m_bClientOnly && m_RegionIndex >= 0)
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	
	
	if (m_RegionIndex == -1)	// 不可能移动到-1 Region，如果出现这种情况，恢复原坐标
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		//CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		CURREGION.AddNpcRef(m_MapX,m_MapY);
		return FALSE;
	}
	
	if (nOldRegion != m_RegionIndex)
	{
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
	return FALSE;
}

//============================
void KNpc::SetRankFF(int nRankFF)
{	
	m_btRankFFId = nRankFF;
}

//spr状态称号==========
void KNpc::PaintHonor(int nbtRankFFId,int nMpsX,int nMpsY,int i)
{
	KTabFile nBank;
   nBank.Load("\\Settings\\HonorSetting.txt");
   int nRows=nBank.GetHeight(); //行数
   int nLies=nBank.GetWidth();  //列数
	char szImageName[128], nLuaField[64];
	int n_X, n_Y,mInterval=100;
	ZeroMemory(szImageName,strlen(szImageName));
    ZeroMemory(nLuaField,strlen(nLuaField));
     if (nbtRankFFId<=nRows)
	 {
          nBank.GetString(nbtRankFFId,"HONORLINK","",nLuaField,sizeof(nLuaField)); 
		  t_sprintf(szImageName,"\\spr\\skill\\others\\%s",nLuaField);
		  nBank.GetInteger(nbtRankFFId,"HONORX",10,&n_X);
		  nBank.GetInteger(nbtRankFFId,"HONORY",10,&n_Y);
		  nBank.GetInteger(nbtRankFFId,"nInterval",100,&mInterval);  
	 }
	 else
	 {//宠物
		 t_sprintf(szImageName,"\\spr\\skill\\others\\助攻王.spr");

	 }
	 this->SetFrameSpr(szImageName, n_X, n_Y+6,GetNpcPate(),mInterval);
	 //SetClientSpr(szImageName,nMpsX+n_X,nMpsY+n_Y,0,i);
	 ZeroMemory(szImageName,strlen(szImageName));
     ZeroMemory(nLuaField,strlen(nLuaField));
     nBank.Clear();
}
//披风=================
void KNpc::PaintPifeng(int m_PifengType,int nMpsX,int nMpsY,int i)
{	
	KIniFile nBank;
	nBank.Load("\\Ui\\npcbobo.ini");
	int nCount=0,nTime=20;
	char szImageName[128]={0},nDir[8]={0};
	      nBank.GetInteger("FortuneRank","count",11,&nCount);
		  nBank.GetInteger("FortuneRank","nTime",20,&nTime);
		 if (m_PifengType>nCount)
				 m_PifengType=nCount;
		 /*if (m_PiFenLoop%18*nTime==0)
		 {
            m_PifengType=GetRandomNumber(0,nCount);
			if  (m_PiFenLoop>50000)
			     m_PiFenLoop=0;
		 }*/
        t_sprintf(nDir,"Spr_%d",m_PifengType);
		nBank.GetString("FortuneRank",nDir,"",szImageName,sizeof(szImageName)); 		
	    SetClientSpr(szImageName,nMpsX,nMpsY,0,i); //szImageName
	    nBank.Clear();
}


//#ifdef _SERVER
//经脉初始化(死亡是初始化)
int KNpc::NpcNewInitJinMaiVal()
{

    int nPlayeridx=0;

    nPlayeridx=CLIENT_PLAYER_INDEX;
    KTabFile nMagFile;
	  if(!nMagFile.Load(TABFILE_MAGICAGOLD_PATH)) //"\\Settings\\item\\004\\GoldMagic.txt"
	  {
		Player[nPlayeridx].m_ItemList.ClientShowMsg("加载文件失败C!");
		return 0;
	  }
	//int nLeftTime = Player[nPlayeridx].m_cTask.GetSaveVal(TASKVALUE_LIXIAN); //离线时间


	BYTE nDuMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),1); //筋脉大类 督脉 0 表示关闭中 1 表示进行中 2 表示完成了
	BYTE nDuMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),2); //开始的行数
	//BYTE nDuMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),3); //当前穴道
		 //Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3501),4);  
	BYTE nRenMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),1);
	BYTE nRenMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),2);
	//BYTE nRenMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3502),3);

	BYTE nChoMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),1);
	BYTE nChoMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),2);
	//BYTE nChoMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3503),3);

	BYTE nDaiMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),1);
	BYTE nDaiMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),2);
	//BYTE nDaiMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3504),3);

	BYTE nYinWMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),1);
	BYTE nYinWMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),2);
	//BYTE nYinWMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3505),3);

	BYTE nYanWMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),1);
	BYTE nYanWMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),2);
	//BYTE nYanWMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3506),3);

	BYTE nYinQMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),1);
	BYTE nYinQMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),2);
	//BYTE nYinQMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),3);
	//BYTE nYinQMaiStaus=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3507),4);

	BYTE nYanQMaiStaus=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),1);
	BYTE nYanQMaiRow=EGetByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),2);
	//BYTE nYanQMaiCurXue=Player[nPlayeridx].m_cTask.GetTaskByte(Player[nPlayeridx].m_cTask.GetSaveVal(3508),3);


	int nNpcIdx = m_Index;
    int nAllStaus=0;
	if (nNpcIdx>0)
	{	
		if (nDuMaiStaus==2&&  //丹田完成
			nRenMaiStaus==2&&    
			nChoMaiStaus==2&&
			nDaiMaiStaus==2&&
			nYinWMaiStaus==2&&
			nYanWMaiStaus==2&&
			nYinQMaiStaus==2&&
			nYanQMaiStaus==2
			)
		{//丹田全部经脉大类完成处理  激活全抗 和 技能加成
            /*  int nMacLevel[2];
			g_FsJinMai.GetInt2(114,6,nMacLevel);
			UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx);*/ //设置技能光环效果
			nAllStaus=1;
		}
      //以下是处理各个经脉的分脉状态
	 KMagicAttrib m_CurrentJinMai;	    // Npc的当前经脉数据
	 ZeroStruct(m_CurrentJinMai);

     for (int i=1;i<9;++i)
	 {
		//char mMagcid[16];
		//int mMgLevel=0,nIfMagic=1,nTime=-1;
		if (nAllStaus==1)
		{//大类全部完成处理 分支状态
		    int nRows=g_FsJinMai.GetHeight();
            for (int k=1;k<nRows;++k)  //13个分脉（行数）
			{   
			    /*if (k>nDuMaiCurXue)
				    break;*/
                int nMacidx;
                
				g_FsJinMai.GetInteger(k+1,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			  //const  KItemNormalAttrib* pAttrib;
		      //pAttrib = &(m_aryMagicAttrib[i]);
              /*
			    	KMagicAttrib* pTemp = NULL;
		            pTemp = (KMagicAttrib *)pData;  //最新等级数据
			  */
				m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;
                // pAttrib=&(m_CurrentJinMai);
		       if (-1 != m_CurrentJinMai.nAttribType)
			   {//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
			   }	
			} 
			//UseSkills(533,1,nNpcIdx); //设置技能光环效果
		}
		else 	
		{//部分完成
		 //int nMacLevel[2];//nMacLevel[0]=技能ID,nMacLevel[1]=技能等级
			if (i==1 && nDuMaiStaus>0) //督脉状态为进行中
			{
			   
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nDuMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+1,7,0,&nMacidx);
                int mAttribType=0,mValue=0;
				nMagFile.GetInteger(nMacidx+1,"属性调整类别",0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,"参数1最小值",0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
                //char msg[64];

			    /*KItemNormalAttrib* pAttrib=NULL;
		        //pAttrib = &(m_aryMagicAttrib[i]);
				pAttrib->nAttribType=mAttribType;
                pAttrib->nValue[0]=mValue;
                pAttrib->nValue[1]=-1;
				pAttrib->nValue[2]=0;

		        if (-1 != pAttrib->nAttribType)
	            {//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, (void *)pAttrib);
	            }*/
				m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}
                /*#ifdef _SERVER
				  t_sprintf(msg,"督脉(%d)状态S:%d,穴道:%d,类型:%d,值:%d||%d",nMacidx,nDuMaiStaus,nDuMaiRow,mAttribType,mValue,m_CurrentJinMai.nValue[0]);
                  Player[nPlayeridx].m_ItemList.msgshow(msg);
	            #else
				  t_sprintf(msg,"督脉(%d)状态C:%d,穴道:%d,类型:%d,值:%d||%d",nMacidx,nDuMaiStaus,nDuMaiRow,mAttribType,mValue,m_CurrentJinMai.nValue[0]);
				  Player[nPlayeridx].m_ItemList.ClientShowMsg(msg);
				#endif*/
					
			  } 
			    
			//	UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==2 && nRenMaiStaus>0) //任脉分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nRenMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+17,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}
			  	
			  } 			
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==3 && nChoMaiStaus>0) //冲脉分脉
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nChoMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+33,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	
			  } 							
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}	
 	
			else if (i==4 && nDaiMaiStaus>0) //带脉分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nDaiMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+49,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{//将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	
			  } 											
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			}
			else if (i==5 && nYinWMaiStaus>0) //阴维分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYinWMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+65,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 
			  //UseSkills(533,1,nNpcIdx); //设置技能光环效果												
			}
			else if (i==6 && nYanWMaiStaus>0) //阳维分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYanWMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+81,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 										
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			} 
			else if (i==7 && nYinQMaiStaus>0) //阴跷分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYinQMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+97,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			   m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 
			  //UseSkills(533,1,nNpcIdx); //设置技能光环效果												
			} 
			else if (i==8 && nYanQMaiStaus>0) //阳跷分脉	
			{
			  for (int k=1;k<17;++k)  //13个分脉（行数）
			  {   
			    if (k>nYanQMaiRow)
				    break;

                int nMacidx;

				g_FsJinMai.GetInteger(k+113,7,0,&nMacidx);
                int mAttribType,mValue;
				nMagFile.GetInteger(nMacidx+1,5,0,&mAttribType) ;
				nMagFile.GetInteger(nMacidx+1,6,0,&mValue) ;
				//UseSkills(nMacLevel[0],nMacLevel[1],nNpcIdx); //设置技能光环效果
				
			    m_CurrentJinMai.nAttribType=mAttribType;
                m_CurrentJinMai.nValue[0]=mValue;
                m_CurrentJinMai.nValue[1]=-1;
				m_CurrentJinMai.nValue[2]=0;

                // pAttrib=&(m_CurrentJinMai);

		        if (m_CurrentJinMai.nAttribType)
				{ //将属性应用再NPC身上
					ModifyAttrib(nNpcIdx, &m_CurrentJinMai);
				}	 
			  } 										
				//UseSkills(533,1,nNpcIdx); //设置技能光环效果
			} 
							
		}
	 }

	} 
	else
	{
	  nMagFile.Clear();
      return 0;
	}

	nMagFile.Clear();
    return 1;
}

void KNpc::UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData,BOOL nIfMagic,int nTime)
{										   
	      KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(sKillID,sLevel);
		  if (nIfMagic)
		  {	   if (pSkill)
			      pSkill->CastStateSkill(nNpcIdx,0,0,nTime);  //是否
		  }
		  else
		     Npc[nNpcIdx].SetStateSkillEffect(nNpcIdx,sKillID,sLevel,pData,0,nTime);  //设置技能光环效果	
}

//#endif

//重置满内里对攻击技能的加成
void KNpc::ReFullManaSkillEnhance(int nLeftSkill,int nLeftListidx)
{
	int nActiveSkillID,nCurSkillEnchance=0;
	nActiveSkillID = nLeftSkill;
	//客户端需要换装才能修复，刚登陆时造成的加成数据误差
	if (IsPlayer())
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax <= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana)
			nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentFullManaskillenhance;
		else
			nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance;
	}

	if (nCurSkillEnchance!= 0)
	{
		int nListIndex = 0;
		if  (nLeftListidx>0)
			nListIndex  = nLeftListidx;
		else
			nListIndex = m_SkillList.FindSame(nActiveSkillID);

		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			int nEnchance = m_SkillList.GetTempEnChance(nListIndex);  //获取原始的技能加成 

			if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				//printf("----左键技能:%d 原始加成:%d -----\n",nListIndex,nEnchance);
				//char Infostr[64]={0};
				//t_sprintf(Infostr,"----左键技能:%d 原始加成:%d -----",nListIndex,nEnchance);
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Infostr);
				m_SkillList.SetEnChance(nListIndex,nEnchance+nCurSkillEnchance);
				//printf("----左键技能: 总加成:%d，另外:%d -----\n",nEnchance+nCurSkillEnchance,nCurSkillEnchance);
				//t_sprintf(Infostr,"----左键技能: 总加成:%d，另外:%d -----",nEnchance+nCurSkillEnchance,nCurSkillEnchance);
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Infostr);
			}			
		}
	}

}

//重置攻击技能的加成
void KNpc::ReSkillEnhance(int nLeftSkill,int nIsAdd)
{
	int nActiveSkillID,nCurSkillEnchance=0;
	
	nActiveSkillID = nLeftSkill;
//客户端需要换装才能修复，刚登陆时造成的加成数据误差
	if (IsPlayer())
	{
     if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax <= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana)
		nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance+Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentFullManaskillenhance;
	 else
        nCurSkillEnchance = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance;
	}

	if (nCurSkillEnchance!= 0)
	{   
		int nListIndex = m_SkillList.FindSame(nActiveSkillID);
		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
			if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)  //等级大于0的才 增加
			{
				if (nIsAdd==0)
				    m_SkillList.AddEnChance(nListIndex,-nCurSkillEnchance);
				else if (nIsAdd > 0)
					m_SkillList.AddEnChance(nListIndex,nCurSkillEnchance);
			}			
		}
	}
/*
#ifdef _SERVER
	char msg[64];
	t_sprintf(msg,"S攻击加成:%d",nCurSkillEnchance);
	Player[m_nsPlayerIdx].m_ItemList.msgshow(msg);
#else
	char msg[64];
	t_sprintf(msg,"C攻击加成:%d",nCurSkillEnchance);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);	
#endif
*/
}

//重置某技能的加成
void KNpc::ReWhereSkillEnhance(int nActiveSkillID,int nCurSkillEnchance,int nIsAdd)
{

/*
#ifdef _SERVER
				char Msg[64];
				t_sprintf(Msg,"S:对某技能(%d)(%d)加成成功",nActiveSkillID,nCurSkillEnchance);
				Player[GetPlayerIdx()].m_ItemList.msgshow(Msg);	
#else
				char Msg[64];
				t_sprintf(Msg,"C:对某技能(%d)(%d)加成成功",nActiveSkillID,nCurSkillEnchance);
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Msg);
#endif;
*/				
	if (nCurSkillEnchance!= 0)
	{
		int nListIndex = m_SkillList.FindSame(nActiveSkillID);
		if (nListIndex)                 //查找激活的当前攻击技能ID
		{
		  if (m_SkillList.GetCurrentLevelByIdx(nListIndex) > 0)    //等级大于0的才 增加
		  {
			 if (nIsAdd==0)
				m_SkillList.AddEnChance(nListIndex,-nCurSkillEnchance);
			 else if (nIsAdd > 0)
				m_SkillList.AddEnChance(nListIndex,nCurSkillEnchance);
		  }	 		
		}
	}
}

void KNpc::NewSetNpcEnChance()
{
	int j;
	for (j = 0 ; j< MAX_NPCSKILL;++j)
	{	
		m_SkillList.m_Skills[j].EnChance = 0;  // 技能的加成清零
		m_SkillList.m_Skills[j].nTempEnChance = 0;
	} 
	m_SkillList.ReEnChance();                  //重置加成
	ReSkillEnhance(Player[CLIENT_PLAYER_INDEX].GetLeftSkill(),1);	
	_EnhanceInfo::iterator it;
	for( it = nEnhanceInfo.begin(); it != nEnhanceInfo.end(); ++it)
	{
		if (it->second.nSkillIdx>0)
		{////重置某技能的加成
		   ReWhereSkillEnhance(it->second.nSkillIdx,it->second.nEnhance,1);	 		
		}
	}
}
