//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Data Compress / Decompress Base Class
//---------------------------------------------------------------------------

#include "KCore.h"
#include "engine/KEngine.h"
#include "KSortScript.h"
#include "gamescene/KScenePlaceC.h"
#include "KItemChangeRes.h"
#include "KNpcSet.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KMissleSet.h"
#include "KFaction.h"
#include "KMath.h"
#include "KPlayerTask.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"
#include "CoreUseNameDef.h"
#include "KBuySell.h"
#include "KTaskFuns.h"
#include "TaskDef.h"
#include "LuaFuns.h"
KLuaScript	*	g_pNpcLevelScript = NULL;
KLuaScript      g_WorldScript;

//客户端开始预加载数据
#define CLIENTWEAPONSKILL_TABFILE		"\\settings\\ClientWeaponSkill.txt"	
#define ADJUSTCOLOR_TABFILE				"\\settings\\AdjustColor.txt"
_ItemDataInfo  iItemDataInfo;
_StrNpcInfo    strNpcInfo;
_StrObjInfo    strObjInfo;
_StrCoreInfo   strCoreInfo;

CCSize  visibleSize;  //可见屏幕窗口的大小
CCPoint origin;       //开始的位置?
CCSize  size;         //
std::string m_SDcardDirPath = "";
std::string maxMapPicPath = "";
std::string m_MobileKey     ="NYc0bUz0z_JhCNAi0WUK8JK0MOlOQlkP";
SimpleAudioEngine *__pSound = NULL;
CCFileUtils *ccFileUtils = NULL;
bool _openSound =true;
unsigned int curMobileVer=30;
unsigned int curMobileKey=14302019;
//KSprCodec SprDecode;

KTimer    timerStart;
KTabFile  g_ClientWeaponSkillTabFile,g_ObjChangeName;
KTabFile  g_OrdinSkillsSetting, g_MisslesSetting,g_PlayerTitle;
KTabFile  g_SkillLevelSetting,g_FsJinMai,g_ForbitMap,g_CompEquip;
KTabFile  g_NpcSetting,g_NpcKindFile ,g_RankTabSetting;
KTabFile  cTabFile,g_CompAtlas,nToolItem;
KIniFile  g_GameSetTing,g_NpcMapDropRate,g_MagicBaoshiDesc;
KIniFile  g_MapTraffic;
KTabFile  g_GetMsg;
int		  g_nMeleeWeaponSkill[MAX_MELEEWEAPON_PARTICULARTYPE_NUM];
int		  g_nRangeWeaponSkill[MAX_RANGEWEAPON_PARTICULARTYPE_NUM];	
int		  g_nHandSkill;
int       nGameVersion;//什么版本的GS
int       nExpSkillIdx;
int       nLuckySkillIdx;
int       nKangkillIdx;
int       nYaoPinSkillIdx;
int       nIsGaldInit=false;
bool      nIsShowShadow=true;
bool      nIsShowTitle=true;
#ifdef WIN32
bool      nisgetinfo=true;
#else
bool      nisgetinfo=false;
#endif
KPakList g_pcPakList;
Kgetinidata _getinidata;
BOOL g_bUISelIntelActiveWithServer = FALSE;//当前选择框是否与服务器端交互
BOOL g_bUISpeakActiveWithServer = FALSE;
int	 g_bUISelLastSelCount = 0;

CGameClient *nAcclient;     //帐号连接
CGameClient *g_pClient;     //gs连接
int m_bIsClientConnecting;  //帐号服务器是否已经连接
int m_bIsGameSevConnecting; //gs是否已经连接
int m_bIsPlayerInGame    = false;//玩家是否已经进入游戏了
int _clientlanguage=2;      //1 为中国 2 为越南 3 英语
bool _clientNoNetWork=false; //是否单机 
void g_InitProtocol();
bool InitSkillSetting()
{
	if (!g_SkillManager.Init())
       CCMessageBox("load filed.","SkillManager");
	if (!g_ClientWeaponSkillTabFile.Load(CLIENTWEAPONSKILL_TABFILE))
		CCMessageBox("load filed..","ClientWeapon");
	return true;
}

bool InitMissleSetting()
{
	int nMissleNum = g_MisslesSetting.GetHeight() - 1;
	for (int i = 0; i < nMissleNum; ++i)
	{
		int nMissleId = 0;
		g_MisslesSetting.GetInteger(i + 2, "MissleId", -1, &nMissleId);
		if (nMissleId>=MAX_MISSLESTYLE)
			continue;
		if (nMissleId > 0)
		{
			g_MisslesLib[nMissleId].GetInfoFromTabFile(i + 2);
			g_MisslesLib[nMissleId].m_nMissleId = nMissleId;
		}
	}
	return true;
}

bool InitTaskSetting()
{
	if (!g_StringResourseTabFile.Load(STRINGRESOURSE_TABFILE))
		CCMessageBox("[TASK]CAN NOT LOAD.","TASK");
	if (!g_RankTabSetting.Load(PLAYER_RANK_SETTING_TABFILE))
		CCMessageBox("[RANK]CAN NOT LOAD.","RANK");
	return true;
}

bool InitNpcSetting()
{
	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	//g_pNpcTemplate = new KNpcTemplate[nNpcTemplateNum * MAX_NPC_LEVEL]; //0,0为起点
	//memset(g_pNpcTemplate, 0, sizeof(void*) * MAX_NPCSTYLE * MAX_NPC_LEVEL);
	// 载入文件 人物类型.txt
	//	g_SetFilePath(RES_INI_FILE_PATH);
	if (!g_NpcKindFile.Load(NPC_RES_KIND_FILE_NAME) )
		CCMessageBox("[NpcKind]CAN NOT LOAD.","NpcKind");
	//加载Npc等级设定的脚本文件，用于今后加载Npc时使用
	g_pNpcLevelScript = new KLuaScript;
	if (!g_pNpcLevelScript)
	{
		CCMessageBox("g_pNpcLevelScript is Error","InitNpcSetting");
	}
	else
	{
	  g_pNpcLevelScript->Init();
	  if (!g_pNpcLevelScript->Load(NPC_LEVELSCRIPT_FILENAME))
	  {
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
		CCMessageBox("[NpcLevel]CAN NOT LOAD.","InitNpcSetting");
	  }
	}

	if (!g_NpcResList.Init())   //客户端加载外观文件动作表
		CCMessageBox("[NpcResList]CAN NOT LOAD.","NpcResList");
	return true;
}

void g_ReleaseCore()
{
//#ifdef WIN32
	g_SubWorldSet.Close();
//#endif
	g_ScenePlace.ClosePlace();

	/*if (g_pNpcLevelScript)
	{
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}*/
	/*
	if (g_pAdjustColorTab)
	{
		delete []g_pAdjustColorTab;
		g_pAdjustColorTab = NULL;
		g_ulAdjustColorCount = 0;
	}*/
    //g_UnInitMath();
}
void g_InitCore()
{   
	//创建帐号　和　ｇｓ　连接
	srand((unsigned)time(NULL));
	timerStart.Start();                           //整个游戏的计时器
	nAcclient = NULL;
	g_pClient = NULL;
	const size_t bufferSize           = 1024;   //Scoket决定发包的大小 分配的内存(m_bufferSize > 0) ? m_bufferSize : (1024*64);
	const size_t bufferSize_Cache     = 1024*512; //分配的内存 读包 接包的缓存大小
	const size_t maxFreeBuffers	      = 2;        //Scoket保留的数量
	const size_t maxFreeBuffers_Cache = 2;        //读包 接包的缓存 保留的数量
	nAcclient     = new CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize,0); //2,2   8
	//g_pClient     = new CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize,1); //2,2   8

	srand((unsigned)time(NULL));
	if (g_GameSetTing.Load(GAME_SETTING_FILE))
	{
		g_GameSetTing.GetInteger("SkillsIcon","ExpSkillIdx",0,&nExpSkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","LuckySkillIdx",0,&nLuckySkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","KangkillIdx",0,&nKangkillIdx);
		g_GameSetTing.GetInteger("SkillsIcon","YaoPinSkillIdx",0,&nYaoPinSkillIdx);
		g_GameSetTing.GetInteger("Gameconfig","GameVersion",1,&nGameVersion);//1为中文版本 2 其他版本 需要替换字符串的
	}

	if (!g_ObjChangeName.Load("\\settings\\vn\\ObjData.txt"))
		CCMessageBox("load vn-ObjData.txt Error","Warning");
		//printf("--加载配置文件失败:%s\n","\\settings\\vn\\ObjData.txt");
	KTabFile nNpcInfo;  //缓冲区有益处
	/*if (nNpcInfo.Load("\\settings\\lang\\zh\\replacename_npc.txt"))
	{
		int  nRows = nNpcInfo.GetHeight()+1;

		for(int i=2;i<nRows;i++)
		{
			char nyNpcName[64]={0},ndNpcName[64]={0};
			nNpcInfo.GetString(i,"sourcename","no npc name",nyNpcName,sizeof(nyNpcName));
			nNpcInfo.GetString(i,"targetname","no npc name",ndNpcName,sizeof(ndNpcName));
			strNpcInfo[nyNpcName]=ndNpcName;
		}
		nNpcInfo.Clear();
	}

	if (nNpcInfo.Load("\\settings\\lang\\zh\\replacename_obj.txt"))
	{
		int  nRows = nNpcInfo.GetHeight()+1;
		for(int i=2;i<nRows;i++)
		{
			char ndNpcName[64]={0};//nyNpcName[64]={0},
			//nNpcInfo.GetString(i,"sourcename","no npc name",nyNpcName,sizeof(nyNpcName));
			nNpcInfo.GetString(i,"targetname","no npc name",ndNpcName,sizeof(ndNpcName));
			strObjInfo[i]=ndNpcName;
		}
		nNpcInfo.Clear();
	}
	*/
	char nTempStr[256];
	ZeroMemory(nTempStr,sizeof(nTempStr));

	if (_clientlanguage!=1)
		t_sprintf(nTempStr,"\\settings\\lang\\vn\\stringtable_core.txt");
	else
	    t_sprintf(nTempStr,"\\settings\\lang\\zh\\stringtable_core.txt");

	if (nNpcInfo.Load(nTempStr))
	{
		int  nRows = nNpcInfo.GetHeight()+1;

		for(int i=2;i<nRows;i++)
		{
			char nyNpcName[64]={0},ndNpcName[256]={0};
			nNpcInfo.GetString(i,"key","no npc name",nyNpcName,sizeof(nyNpcName));
			nNpcInfo.GetString(i,"value","no npc name",ndNpcName,sizeof(ndNpcName));
			strCoreInfo[nyNpcName]=ndNpcName;
		}
		nNpcInfo.Clear();
	}

	char *szRandomMem1, *szRandomMem2, *szRandomMem3;	//这些变量没有应用价值，主要是防外挂找固定内存
	SubWorld = new KSubWorld[MAX_SUBWORLD];
	szRandomMem1 = new char[((rand() % 64) + 6) * 1024];
	Player   = new KPlayer[MAX_PLAYER];
	szRandomMem2 = new char[((rand() % 64) + 6) * 1024];
	Npc      = new KNpc[MAX_NPC];
	szRandomMem3 = new char[((rand() % 64) + 6) * 1024];
	Item     = new KItem[MAX_ITEM];
	Object   = new KObj[MAX_OBJECT];
	Missle   = new KMissle[MAX_MISSLE];

	if (szRandomMem1)
	{
		delete[] szRandomMem1;
		szRandomMem1 = NULL;
	}

	if (szRandomMem2)
	{
		delete[] szRandomMem2;
		szRandomMem2 = NULL;
	}

	if (szRandomMem3)
	{
		delete[] szRandomMem3;
		szRandomMem3 = NULL;
	}
	//_ASSERT(SubWorld && Player && Npc && Item && Object && Missle);
	//---------------------------------------------------------------------------
	g_InitProtocol();
	g_RandomSeed(42);	                               //设置一个随机种子	clock()
	//g_SoundCache.Init(256);

	//g_SubWorldSet.m_cMusic.Init();//VS 2010 有错误

	g_InitSeries();

	if (!g_InitMath())
		CCMessageBox("load InitMath Error","InitMath");
	//return;
	ItemSet.Init();	
	ItemGen.Init();
	//InitAdjustColorTab();                            //VS 2010 有错误状态颜色偏移  如 冰状态的颜色，毒状态的颜色//加载偏色表......

	g_MagicDesc.Init();                              //初始化客户端魔法属性类型
	g_ItemChangeRes.Init();                             //VS 2010 有错误 初始化装备外观文件
	//if(!g_ForbitWaiGua.Load(GAME_FORBITWAIGUA_FILE))  //VS 2010 有错误
	//	CCMessageBox("load ForbitMap Error","ForbitMap");

	if(!g_ForbitMap.Load(GAME_FORBITMAP_FILE))
		CCMessageBox("load ForbitMap Error","ForbitMap");
	if(!g_FsJinMai.Load(GAME_FSJINMAI_FILE))
		CCMessageBox("load JinMai Error","JinMai");
	//return;
	g_PlayerTitle.Load("\\Settings\\playertitle.txt");
	g_NpcMapDropRate.Load(MAP_DROPRATE_FILE);
	NpcSet.m_cGoldTemplate.Init();                       //黄金怪物模板初始化
	NpcSet.Init();
	ObjSet.Init();
	MissleSet.Init();

	g_IniScriptEngine("\\Ui",FALSE);                      //J加载客户端脚本
	if (!g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE))   //初始化技能基本数据
		CCMessageBox("load skills error.","skills");

	g_MisslesSetting.Load(MISSLES_SETTING_FILE);     //子弹初始化  子技能

	g_NpcSetting.Load(NPC_SETTING_FILE);             //加载NPC模板
	InitSkillSetting();                              //技能   //初始化技能基本数据

	InitMissleSetting();                             //子弹

	InitNpcSetting();                                //npc模板
	InitTaskSetting();                               //任务
	// 这个涉及到与技能相关的东西，所以必须放在技能初始化之后
	if (!PlayerSet.Init())
	   CCMessageBox("Init PlayerSet Error","PlayerSet");

	g_CompAtlas.Load(GAME_ATLAS_FILE);
	g_GetMsg.Load(GAME_MSG_FILE);
	g_CompEquip.Load(GAME_COMP_FILE);
	g_MapTraffic.Load("\\settings\\MapTraffic.ini");
	if (!g_ScenePlace.Initialize())//场景初始化
		CCMessageBox("Scene Load Error","Initialize");

	g_Faction.Init();
	memset(g_nMeleeWeaponSkill, 0, sizeof(g_nMeleeWeaponSkill));
	memset(g_nRangeWeaponSkill, 0, sizeof(g_nRangeWeaponSkill));
	KTabFile Weapon_PhysicsSkillIdFile;
	if (Weapon_PhysicsSkillIdFile.Load(WEAPON_PHYSICSSKILLFILE))
	{
		int nHeight = Weapon_PhysicsSkillIdFile.GetHeight() - 1;
		for (int i = 0; i < nHeight; ++i)
		{
			int nDetail = 0,nParticular  = 0,nPhysicsSkill = 0;
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,1, -1, &nDetail); //WEAPON_DETAILTYPE
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,2, -1, &nParticular);//WEAPON_PARTICULARTYPE
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2,3, -1, &nPhysicsSkill); //WEAPON_SKILLID

			//近程武器
			if (nDetail == 0 )
			{
				if (nParticular >= 0 && nParticular < MAX_MELEEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nMeleeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == 1)
			{
				if (nParticular >= 0 && nParticular < MAX_RANGEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nRangeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == -1) //空手
			{
				if (nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nHandSkill = nPhysicsSkill;
			}
		}
	}
	else
		CCMessageBox("PhysicsSkillIdFile load Error","PhysicsSkillIdFile");

	Weapon_PhysicsSkillIdFile.Clear();
	if (!BuySell.Init())  //商店初始化
		CCMessageBox("Init BuySell Error","BuySell");

	//启动一个线程处理 游戏主程序循环
	//CCMessageBox("Init Core is ok....","InitCore");
}

/*
KCore::KCore(void)
{

}*/
