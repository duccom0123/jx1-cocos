//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcGold.cpp
// Date:	2003.07.23
// Code:	边城浪子
// Desc:	KNpcGold Class
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KSubWorld.h" 
#include	"KNpcTemplate.h"
#include	"KNpc.h"
#include	"KNpcSet.h"
#include	"KNpcGold.h"

#define		defNPC_GOLD_SKILL_NO		5
#define		defNPC_GOLD_RATE			20

void	KNpcGold::Init(int nIdx)
{
	this->m_nNpcIdx				= nIdx;
	this->m_nIsGold				= 0;
	this->m_nIsGolding			= 0;
	this->m_nGoldType			= 0;

	this->m_dwSkill5ID			= 0;
	this->m_nSkill5Level		= 0;
	this->m_nFireResist			= 0;
	this->m_nFireResistMax		= 0;
	this->m_nColdResist			= 0;
	this->m_nColdResistMax		= 0;
	this->m_nLightingResist		= 0;
	this->m_nLightingResistMax	= 0;
	this->m_nPoisonResist		= 0;
	this->m_nPoisonResistMax	= 0;
	this->m_nPhycicsResist		= 0;
	this->m_nPhycicsResistMax	= 0;
	////////////////////////////////
    /*this->m_nAiMode	= 0;
    this->m_nAiParam1	= 0;
    this->m_nAiParam2	= 0;
    this->m_nAiParam3	= 0;
    this->m_nAiParam4= 0;
    this->m_nAiParam5= 0;
    this->m_nAiParam6= 0;
    this->m_nAiParam7= 0;
    this->m_nAiParam8= 0;
    this->m_nAiParam9= 0;
    this->m_nAiParam10= 0;
    this->m_nAiMaxTime= 0;
    this->m_nPhysicalDamageBase= 0;
    this->m_nPhysicalMagicBase= 0;
    this->m_nPoisonDamageBase= 0;
    this->m_nPoisonMagicBase= 0;
    this->m_nColdDamageBase= 0;
    this->m_nColdMagicBase= 0;
    this->m_nFireDamageBase= 0;
    this->m_nFireMagicBase= 0;
    this->m_nLightingDamageBase= 0;
    this->m_nLightingMagicBase= 0;
	*/
    //ZeroMemory(this->m_nDeathScript,sizeof(this->m_nDeathScript));
    //ZeroMemory(this->m_nDropRate,sizeof(this->m_nDropRate));
    //this->m_nRate= 0;
}

//---------------------------------------------------------------------------
//	功能：设定是否为黄金怪物
//---------------------------------------------------------------------------
void	KNpcGold::SetGoldType(BOOL bFlag)
{
	this->m_nIsGold = (bFlag != 0 ? 1 : 0);
}

//---------------------------------------------------------------------------
//	功能：设定类型为黄金怪物，同时备份相应数据
//---------------------------------------------------------------------------
void	KNpcGold::SetGoldTypeAndBackData()
{
	m_nIsGold = 1;
	m_nIsGolding = 0;
	m_nGoldType = 0;

	m_nFireResist			= Npc[m_nNpcIdx].m_CurrentFireResist;
	m_nFireResistMax		= Npc[m_nNpcIdx].m_CurrentFireResistMax;
	m_nColdResist			= Npc[m_nNpcIdx].m_CurrentColdResist;
	m_nColdResistMax		= Npc[m_nNpcIdx].m_CurrentColdResistMax;
	m_nLightingResist		= Npc[m_nNpcIdx].m_CurrentLightResist;
	m_nLightingResistMax	= Npc[m_nNpcIdx].m_CurrentLightResistMax;
	m_nPoisonResist			= Npc[m_nNpcIdx].m_CurrentPoisonResist;
	m_nPoisonResistMax		= Npc[m_nNpcIdx].m_CurrentPoisonResistMax;
	m_nPhycicsResist		= Npc[m_nNpcIdx].m_CurrentPhysicsResist;
	m_nPhycicsResistMax		= Npc[m_nNpcIdx].m_CurrentPhysicsResistMax;
}
//---------------------------------------------------------------------------
//	功能：设定类型为黄金怪物，同时备份相应数据
//---------------------------------------------------------------------------
void	KNpcGold::SetBossTypeAndBackData(int nBoss)
{
	m_nIsGold = 1;
	m_nIsGolding = 1;
	m_nGoldType = nBoss;
	
	m_nFireResist			= Npc[m_nNpcIdx].m_CurrentFireResist;
	m_nFireResistMax		= Npc[m_nNpcIdx].m_CurrentFireResistMax;
	m_nColdResist			= Npc[m_nNpcIdx].m_CurrentColdResist;
	m_nColdResistMax		= Npc[m_nNpcIdx].m_CurrentColdResistMax;
	m_nLightingResist		= Npc[m_nNpcIdx].m_CurrentLightResist;
	m_nLightingResistMax	= Npc[m_nNpcIdx].m_CurrentLightResistMax;
	m_nPoisonResist			= Npc[m_nNpcIdx].m_CurrentPoisonResist;
	m_nPoisonResistMax		= Npc[m_nNpcIdx].m_CurrentPoisonResistMax;
	m_nPhycicsResist		= Npc[m_nNpcIdx].m_CurrentPhysicsResist;
	m_nPhycicsResistMax		= Npc[m_nNpcIdx].m_CurrentPhysicsResistMax;
}

//---------------------------------------------------------------------------
//	功能：恢复旧数据，变成普通npc
//---------------------------------------------------------------------------
void	KNpcGold::RecoverBackData()
{
	if (/*!m_nIsGold || */!m_nIsGolding || m_nGoldType >defMAX_NPC_GOLD_TYEP)
		return;

	m_nIsGolding = 0;

	// 用备份数据覆盖
	Npc[m_nNpcIdx].m_CurrentFireResist			= m_nFireResist;	   //原始数据
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= m_nPhycicsResist;
	Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= m_nPhycicsResistMax;

	if (m_nGoldType < 0 || m_nGoldType >= NpcSet.m_cGoldTemplate.m_nEffectTypeNum)
		return;

	KNpcGoldTemplateInfo	*pInfo = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];

	// 按比例缩小
	//Npc[m_nNpcIdx].m_Experience					/= pInfo->m_nExp;
	if  (pInfo->m_nLife>0)
		Npc[m_nNpcIdx].m_CurrentLifeMax				/= pInfo->m_nLife;
	else
		Npc[m_nNpcIdx].m_CurrentLifeMax = 100;

	if (pInfo->m_nExp>0)
		Npc[m_nNpcIdx].m_Experience	/= pInfo->m_nExp;
	else
		Npc[m_nNpcIdx].m_Experience   = 1;


	if (pInfo->m_nLifeReplenish>0)
		Npc[m_nNpcIdx].m_CurrentLifeReplenish	/= pInfo->m_nLifeReplenish;
	else
		Npc[m_nNpcIdx].m_CurrentLifeReplenish   = 0;

	if (pInfo->m_nAttackRating>0)
		Npc[m_nNpcIdx].m_CurrentAttackRating		/= pInfo->m_nAttackRating;
	else
		Npc[m_nNpcIdx].m_CurrentAttackRating    = 0;

	if (pInfo->m_nDefense>0)
		Npc[m_nNpcIdx].m_CurrentDefend				/= pInfo->m_nDefense;
	else
		Npc[m_nNpcIdx].m_CurrentDefend          = 0;

	if (pInfo->m_nMinDamage>0)
		Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	/= pInfo->m_nMinDamage;	  //基本伤害
	else
		Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0] = 0;
	if (pInfo->m_nMaxDamage>0)
		Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	/= pInfo->m_nMaxDamage;	  //基本伤害--不包括技能
	else
		Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2] = 5;
	// 扣点数
	Npc[m_nNpcIdx].m_CurrentTreasure	-= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	-= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	-= pInfo->m_nRunSpeed;   //
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	-= pInfo->m_nAttackSpeed;
	Npc[m_nNpcIdx].m_CurrentCastSpeed	-= pInfo->m_nCastSpeed;
}

//---------------------------------------------------------------------------
//	功能：按一定概率变成黄金怪物---蓝怪几率产生设置-
//---------------------------------------------------------------------------
void KNpcGold::RandChangeGold(BOOL bGold,int BossType,int nSubWorld)  //默认 5 和 flase
{
	//if (SubWorld[nSubWorld].m_SubWorldID==38)
	//printf("--BOSS类型B：%d 地图:%d--\n",BossType,SubWorld[nSubWorld].m_SubWorldID);

    if (BossType<0)
	{
		m_nIsGold		= 0;
		m_nIsGolding	= 0;
        return;
	}
    else if (BossType>0)
	{ 
      m_nGoldType =BossType;	
	  if (NpcSet.m_cGoldTemplate.m_nEffectTypeNum <= 0)
		return;
	} 
    else
	{  
	  if (/*m_nIsGold || */this->m_nIsGolding==1)
		return;

	  //if (SubWorld[nSubWorld].m_SubWorldID==38)
	  //	  printf("--BOSS类型A：%d--\n",BossType);


      int npMapGoldRate=0,nMapAutoGoldNpcRank;
	  char nMapGoldRate[32]={0};
	  t_sprintf(nMapGoldRate,"%d_AutoGoldenNpc",SubWorld[nSubWorld].m_SubWorldID);  //38_AutoGoldenNpc
      g_NpcMapDropRate.GetInteger("List",nMapGoldRate,0,&npMapGoldRate);//生成加强怪物的概率
	  g_GameSetTing.GetInteger("SYSTEM","MapAutoGoldNpcRank",0,&nMapAutoGoldNpcRank);
    
	  if (npMapGoldRate>=0 && g_Random(npMapGoldRate)<g_Random(nMapAutoGoldNpcRank))
	  {//普通怪物   
		return;
	  }  

	if (NpcSet.m_cGoldTemplate.m_nEffectTypeNum <= 0)
		return;
//原来没有的  蓝黄怪产生类型
   if (bGold==1)
   { 
	   char nMapGoldType[32]={0};
       int  npMapGoldType=0;
       t_sprintf(nMapGoldType,"%d_GoldenType"   ,SubWorld[nSubWorld].m_SubWorldID); 
       g_NpcMapDropRate.GetInteger("List",nMapGoldType,4,&npMapGoldType);
	  //if (SubWorld[nSubWorld].m_SubWorldID==38)
	  //    printf("--BOSS概率A：%d--\n",npMapGoldRate);
       if (npMapGoldRate<0 || npMapGoldRate>1)
	   {  
         //if (g_Random(npMapGoldRate) >= g_Random(npMapGoldRate))
	   	    m_nGoldType = GetRandomNumber(1,npMapGoldType); //绿 蓝 黄 红
         //else
         // m_nGoldType = GetRandomNumber(1,4); //蓝怪
	   } 
	   else
		   return;
	}
    else
       return;
	}
////////////////////////////////////////////////////以下是脚本刷怪
      m_nIsGolding = 1;

    KNpcGoldTemplateInfo	*pInfo              = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];
	// 用模板数据直接辅值
	Npc[m_nNpcIdx].m_CurrentFireResist			= pInfo->m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= pInfo->m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= pInfo->m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= pInfo->m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= pInfo->m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= pInfo->m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= pInfo->m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= pInfo->m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= pInfo->m_nPhycicsResist;
    Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= pInfo->m_nPhycicsResistMax;
	// 按比例放大
	Npc[m_nNpcIdx].m_Experience					*= pInfo->m_nExp;   //模板经验比例
	Npc[m_nNpcIdx].m_CurrentLifeMax				*= pInfo->m_nLife;  //模板倍数
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		*= pInfo->m_nLifeReplenish;  //生命恢复速度
	Npc[m_nNpcIdx].m_CurrentAttackRating		*= pInfo->m_nAttackRating;  //命中
	Npc[m_nNpcIdx].m_CurrentDefend				*= pInfo->m_nDefense;   //闪避点
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	*= pInfo->m_nMinDamage; //最小物理伤害
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	*= pInfo->m_nMaxDamage; //最大物理伤害
	// 加点数
	Npc[m_nNpcIdx].m_CurrentTreasure	+= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	+= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	+= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	+= pInfo->m_nAttackSpeed;  //外功速度
	Npc[m_nNpcIdx].m_CurrentCastSpeed	+= pInfo->m_nCastSpeed;    //内攻速
    //设置满血
	Npc[m_nNpcIdx].m_CurrentLife = Npc[m_nNpcIdx].m_CurrentLifeMax;
}

//读取脚本 从脚本获取返回值
int KNpcGold::GetGoldNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam,int nSeries)
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (pScript==NULL || szParam == NULL|| szParam[0] == 0 /*|| strlen(szParam) < 3*/)
	{
		return 0;
	}
	nTopIndex=pScript->SafeCallBegin();
	pScript->CallFunction("GetNpcLevelData",1, "dssd",nSeries,nLevel,szDataName,szParam);   // 函数命令
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);  //返回数字

	nTopIndex=pScript->SafeCallBegin();
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}


//---------------------------------------------------------------------------
//	功能：获得当前黄金类型，0 非黄金怪
//---------------------------------------------------------------------------
int		KNpcGold::GetGoldType()
{
	if (/*!m_nIsGold || */!m_nIsGolding)
		return 0;
	return m_nGoldType;//m_nGoldType+1
}


void	KNpcGold::SetGoldCurrentType(int nType,int nSubWorld)
{
	if (nType <= 0 || nType > 16)
	{
		m_nIsGold		= 0;
		m_nIsGolding	= 0;
		return;
	}
	RecoverBackData();  //恢复基本数据
	m_nIsGold		= 1;
	m_nIsGolding	= 1;
	m_nGoldType		= nType;

    KNpcGoldTemplateInfo	*pInfo              = &NpcSet.m_cGoldTemplate.m_sInfo[m_nGoldType];
	// 用模板数据直接辅值
	Npc[m_nNpcIdx].m_CurrentFireResist			= pInfo->m_nFireResist;
	Npc[m_nNpcIdx].m_CurrentFireResistMax		= pInfo->m_nFireResistMax;
	Npc[m_nNpcIdx].m_CurrentColdResist			= pInfo->m_nColdResist;
	Npc[m_nNpcIdx].m_CurrentColdResistMax		= pInfo->m_nColdResistMax;
	Npc[m_nNpcIdx].m_CurrentLightResist			= pInfo->m_nLightingResist;
	Npc[m_nNpcIdx].m_CurrentLightResistMax		= pInfo->m_nLightingResistMax;
	Npc[m_nNpcIdx].m_CurrentPoisonResist		= pInfo->m_nPoisonResist;
	Npc[m_nNpcIdx].m_CurrentPoisonResistMax		= pInfo->m_nPoisonResistMax;
	Npc[m_nNpcIdx].m_CurrentPhysicsResist		= pInfo->m_nPhycicsResist;
    Npc[m_nNpcIdx].m_CurrentPhysicsResistMax	= pInfo->m_nPhycicsResistMax;
	// 按比例放大
	Npc[m_nNpcIdx].m_Experience					*= pInfo->m_nExp;   //模板经验比例
	Npc[m_nNpcIdx].m_CurrentLifeMax				*= pInfo->m_nLife;  //模板倍数
	Npc[m_nNpcIdx].m_CurrentLifeReplenish		*= pInfo->m_nLifeReplenish;  //生命恢复速度
	Npc[m_nNpcIdx].m_CurrentAttackRating		*= pInfo->m_nAttackRating;  //命中
	Npc[m_nNpcIdx].m_CurrentDefend				*= pInfo->m_nDefense;   //闪避点
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[0]	*= pInfo->m_nMinDamage; //最小物理伤害
	Npc[m_nNpcIdx].m_PhysicsDamage.nValue[2]	*= pInfo->m_nMaxDamage; //最大物理伤害
	
	// 加点数
	Npc[m_nNpcIdx].m_CurrentTreasure	+= pInfo->m_nTreasure;   //掉落装备的数量
	Npc[m_nNpcIdx].m_CurrentWalkSpeed	+= pInfo->m_nWalkSpeed;
	Npc[m_nNpcIdx].m_CurrentRunSpeed	+= pInfo->m_nRunSpeed;
	Npc[m_nNpcIdx].m_CurrentAttackSpeed	+= pInfo->m_nAttackSpeed;  //外功速度
	Npc[m_nNpcIdx].m_CurrentCastSpeed	+= pInfo->m_nCastSpeed;    //内攻速
    //设置满血
	//if (Npc[m_nNpcIdx].m_CurrentLifeMax>15000000)
	//   Npc[m_nNpcIdx].m_CurrentLifeMax = 15000000;

	Npc[m_nNpcIdx].m_CurrentLife = Npc[m_nNpcIdx].m_CurrentLifeMax;
}

void	KNpcGold::ClientClearState()
{
	m_nIsGold		= 0;
	m_nIsGolding	= 0;
}

//------------------------------------------------------------------
KNpcGoldTemplate::KNpcGoldTemplate()
{
	memset(this->m_sInfo, 0, sizeof(this->m_sInfo));
	this->m_nEffectTypeNum = 0;
}
//蓝怪的模板
BOOL	KNpcGoldTemplate::Init()
{
	KTabFile	cFile;

//	g_SetFilePath("\\");
	if (!cFile.Load(NPC_GOLD_TEMPLATE_FILE))
		return FALSE;

//	char	szTemp[80];
	int     nSkillID=0,nRows=0,i;
            nRows=cFile.GetHeight()-1;
	for (i = 0; i <nRows; ++i)
	{
        if (i>=defMAX_NPC_GOLD_TYEP)  
			break;
		cFile.GetInteger(i + 2, "Exp", 1, &m_sInfo[i+1].m_nExp);
		cFile.GetInteger(i + 2, "Life", 1, &m_sInfo[i+1].m_nLife);
		cFile.GetInteger(i + 2, "LifeReplenish", 1, &m_sInfo[i+1].m_nLifeReplenish);
		cFile.GetInteger(i + 2, "AttackRating", 1, &m_sInfo[i+1].m_nAttackRating);
		cFile.GetInteger(i + 2, "Defense", 1, &m_sInfo[i+1].m_nDefense);
		cFile.GetInteger(i + 2, "MinDamage", 1, &m_sInfo[i+1].m_nMinDamage);
		cFile.GetInteger(i + 2, "MaxDamage", 1, &m_sInfo[i+1].m_nMaxDamage);

		cFile.GetInteger(i + 2, "Treasure", 0, &m_sInfo[i+1].m_nTreasure);
		cFile.GetInteger(i + 2, "WalkSpeed", 0, &m_sInfo[i+1].m_nWalkSpeed);
		cFile.GetInteger(i + 2, "RunSpeed", 0, &m_sInfo[i+1].m_nRunSpeed);
		cFile.GetInteger(i + 2, "AttackSpeed", 0, &m_sInfo[i+1].m_nAttackSpeed);
		cFile.GetInteger(i + 2, "CastSpeed", 0, &m_sInfo[i+1].m_nCastSpeed);

	//	cFile.GetString(i + 2, 14, "", szTemp, sizeof(szTemp));
	//	m_sInfo[i].m_dwSkill5ID = KNpcTemplate::SkillString2Id(szTemp);
        cFile.GetInteger(i + 2, "SkillID",0, &m_sInfo[i+1].m_dwSkill5ID);
		cFile.GetInteger(i + 2, "SkillLevel",5,&m_sInfo[i+1].m_szSkill5Level);
		cFile.GetInteger(i + 2, "FireResist", 0, &m_sInfo[i+1].m_nFireResist);
		cFile.GetInteger(i + 2, "FireResistMax", 0, &m_sInfo[i+1].m_nFireResistMax);
		cFile.GetInteger(i + 2, "ColdResist", 0, &m_sInfo[i+1].m_nColdResist);
		cFile.GetInteger(i + 2, "ColdResistMax", 0, &m_sInfo[i+1].m_nColdResistMax);
		cFile.GetInteger(i + 2, "LightingResist", 0, &m_sInfo[i+1].m_nLightingResist);
		cFile.GetInteger(i + 2, "LightingResistMax", 0, &m_sInfo[i+1].m_nLightingResistMax);
		cFile.GetInteger(i + 2, "PoisonResist", 0, &m_sInfo[i+1].m_nPoisonResist);
		cFile.GetInteger(i + 2, "PoisonResistMax", 0, &m_sInfo[i+1].m_nPoisonResistMax);
		cFile.GetInteger(i + 2, "PhycicsResist", 0, &m_sInfo[i+1].m_nPhycicsResist);
		cFile.GetInteger(i + 2, "PhycicsResistMax", 0, &m_sInfo[i+1].m_nPhycicsResistMax);
		/*cFile.GetInteger(i + 2, "AiMode", 0, &m_sInfo[i+1].m_nAiMode);
		cFile.GetInteger(i + 2, "AiParam1", 0, &m_sInfo[i+1].m_nAiParam1);
		cFile.GetInteger(i + 2, "AiParam2", 0, &m_sInfo[i+1].m_nAiParam2);
		cFile.GetInteger(i + 2, "AiParam3", 0, &m_sInfo[i+1].m_nAiParam3);
		cFile.GetInteger(i + 2, "AiParam4", 0, &m_sInfo[i+1].m_nAiParam4);
		cFile.GetInteger(i + 2, "AiParam5", 0, &m_sInfo[i+1].m_nAiParam5);
		cFile.GetInteger(i + 2, "AiParam6", 0, &m_sInfo[i+1].m_nAiParam6);
		cFile.GetInteger(i + 2, "AiParam7", 0, &m_sInfo[i+1].m_nAiParam7);
		cFile.GetInteger(i + 2, "AiParam8", 0, &m_sInfo[i+1].m_nAiParam8);
		cFile.GetInteger(i + 2, "AiParam9", 0, &m_sInfo[i+1].m_nAiParam9);
		cFile.GetInteger(i + 2, "AiParam10", 0, &m_sInfo[i+1].m_nAiParam10);
		cFile.GetInteger(i + 2, "AiMaxTime", 0, &m_sInfo[i+1].m_nAiMaxTime);
		cFile.GetInteger(i + 2, "PhysicalDamageBase", 0, &m_sInfo[i+1].m_nPhysicalDamageBase);
		cFile.GetInteger(i + 2, "PhysicalMagicBase", 0, &m_sInfo[i+1].m_nPhysicalMagicBase);
		cFile.GetInteger(i + 2, "PoisonDamageBase", 0, &m_sInfo[i+1].m_nPoisonDamageBase);
		cFile.GetInteger(i + 2, "PoisonMagicBase", 0, &m_sInfo[i+1].m_nPoisonMagicBase);
		cFile.GetInteger(i + 2, "ColdDamageBase", 0, &m_sInfo[i+1].m_nColdDamageBase);
		cFile.GetInteger(i + 2, "ColdMagicBase", 0, &m_sInfo[i+1].m_nColdMagicBase);
		cFile.GetInteger(i + 2, "FireDamageBase", 0, &m_sInfo[i+1].m_nFireDamageBase);
		cFile.GetInteger(i + 2, "FireMagicBase", 0, &m_sInfo[i+1].m_nFireMagicBase);
		cFile.GetInteger(i + 2, "LightingDamageBase", 0, &m_sInfo[i+1].m_nLightingDamageBase);
		cFile.GetInteger(i + 2, "LightingMagicBase", 0, &m_sInfo[i+1].m_nLightingMagicBase);
		*/
		//cFile.GetString(i + 2, 48, "", m_sInfo[i+1].m_nDeathScript, sizeof(m_sInfo[i+1].m_nDeathScript)); //死亡脚本
        //cFile.GetString(i + 2, 49, "", m_sInfo[i+1].m_nDropRate,sizeof(m_sInfo[i+1].m_nDropRate));        //爆率
	    //cFile.GetInteger(i + 2, 50, 0, &m_sInfo[i+1].m_nRate);                                            //概率
		
	}
	this->m_nEffectTypeNum = i;                                                                             //模板 黄金怪物的类型数量 
	cFile.Clear();
    //printf(" KNpcGoldTemplate::Init(%d) OK!!!..\n",this->m_nEffectTypeNum);
	return TRUE;
}