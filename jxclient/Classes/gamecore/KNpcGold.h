//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcGold.h
// Date:	2003.07.23
// Code:	边城浪子
// Desc:	KNpcGold Class
//---------------------------------------------------------------------------

#ifndef _KNPCGOLD_H
#define	_KNPCGOLD_H

#include "engine/KLuaScript.h"

#define	defMAX_NPC_GOLD_TYEP	30  //蓝怪NPC的种类限制

class KNpcGold
{
	friend class KNpc;
private:
	int		m_nNpcIdx;
	int		m_nIsGold;			// 是否是黄金怪物类
	int		m_nIsGolding;		// 当前是否是黄金怪物
	int		m_nGoldType;		// 如果当前是黄金怪物，变化的类型

// 备份的 npc 数据
	DWORD	m_dwSkill5ID;				// 技能名转成的id  SkillString2Id()
	int		m_nSkill5Level;				// 技能等级
	int		m_nFireResist;				// 火抗性
	int		m_nFireResistMax;			// 火抗性最大值
	int		m_nColdResist;				// 冰抗性
	int		m_nColdResistMax;			// 冰抗性最大值
	int		m_nLightingResist;			// 电抗性
	int		m_nLightingResistMax;		// 电抗性最大值
	int		m_nPoisonResist;			// 毒抗性
	int		m_nPoisonResistMax;			// 毒抗性最大值
	int		m_nPhycicsResist;			// 物理抗性
	int		m_nPhycicsResistMax;		// 物理抗性最大值
//////////////////////////////////////////////////////
	/*int		m_nAiMode;
	int		m_nAiParam1;
	int		m_nAiParam2;
	int		m_nAiParam3;
	int		m_nAiParam4;
	int		m_nAiParam5;
	int		m_nAiParam6;
	int		m_nAiParam7;
	int		m_nAiParam8;
	int		m_nAiParam9;
	int		m_nAiParam10;
	int		m_nAiMaxTime;
	int		m_nPhysicalDamageBase;
	int		m_nPhysicalMagicBase;
	int		m_nPoisonDamageBase;
	int		m_nPoisonMagicBase;
	int		m_nColdDamageBase;
	int		m_nColdMagicBase;
	int		m_nFireDamageBase;
	int		m_nFireMagicBase;
	int		m_nLightingDamageBase;
	int		m_nLightingMagicBase;
	//char	m_nDeathScript[128]; //死亡脚本
	//char	m_nDropRate[128]; //爆率
	//int   m_nRate;
	*/
	int     GetGoldNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam,int nSeries);
public:
	void	Init(int nIdx);
	void	SetGoldType(BOOL bFlag);	// 设定是否为黄金怪物
	void	SetGoldTypeAndBackData();	// 设定类型为黄金怪物，同时备份相应数据
	void	SetBossTypeAndBackData(int nBoss);
	void	RecoverBackData();			// 恢复旧数据，变成普通npc
	void	RandChangeGold(BOOL bGold = 0,int BossType=0,int nSubWorld=0);			// 按一定概率变成黄金怪物
	int		GetGoldType();				// 获得当前黄金类型，0 非黄金怪
	void	SetGoldCurrentType(int nType,int nSubWorld=0);
	void	ClientClearState();

};

struct	KNpcGoldTemplateInfo
{
// 按整数倍变化，可反向恢复
	int		m_nExp;						// 经验
	int		m_nLife;					// 生命
	int		m_nLifeReplenish;			// 回血
	int		m_nAttackRating;			// 命中
	int		m_nDefense;					// 防御
	int		m_nMinDamage;
	int		m_nMaxDamage;

// 加加减减的东西，可反向恢复
	int		m_nTreasure;				// 装备
	int		m_nWalkSpeed;
	int		m_nRunSpeed;
	int		m_nAttackSpeed;
	int		m_nCastSpeed;

// 需要备份的东西，不可反向恢复
	int	    m_dwSkill5ID;				// 技能名转成的id  SkillString2Id()
	//char	m_szSkill5Level[32];		// 技能等级
	int 	m_szSkill5Level;		    // 技能等级
	int		m_nFireResist;				// 火抗性
	int		m_nFireResistMax;			// 火抗性最大值
	int		m_nColdResist;				// 冰抗性
	int		m_nColdResistMax;			// 冰抗性最大值
	int		m_nLightingResist;			// 电抗性
	int		m_nLightingResistMax;		// 电抗性最大值
	int		m_nPoisonResist;			// 毒抗性
	int		m_nPoisonResistMax;			// 毒抗性最大值
	int		m_nPhycicsResist;			// 物理抗性
	int		m_nPhycicsResistMax;		// 物理抗性最大值
//////////////////////////////////////////////////////
	/*int		m_nAiMode;
	int		m_nAiParam1;
	int		m_nAiParam2;
	int		m_nAiParam3;
	int		m_nAiParam4;
	int		m_nAiParam5;
	int		m_nAiParam6;
	int		m_nAiParam7;
	int		m_nAiParam8;
	int		m_nAiParam9;
	int		m_nAiParam10;
	int		m_nAiMaxTime;
	int		m_nPhysicalDamageBase;
	int		m_nPhysicalMagicBase;
	int		m_nPoisonDamageBase;
	int		m_nPoisonMagicBase;
	int		m_nColdDamageBase;
	int		m_nColdMagicBase;
	int		m_nFireDamageBase;
	int		m_nFireMagicBase;
	int		m_nLightingDamageBase;
	int		m_nLightingMagicBase;
	//char	m_nDeathScript[128]; //死亡脚本
	//char	m_nDropRate[128]; //爆率
	int     m_nRate;
	*/
};

class KNpcGoldTemplate
{
	friend class KNpcSet;
public:
	KNpcGoldTemplateInfo m_sInfo[defMAX_NPC_GOLD_TYEP];
	int	 m_nEffectTypeNum; //蓝怪的最大种类
public:
	KNpcGoldTemplate();
	BOOL	Init();
};
//extern 	KNpcGoldTemplateInfo *m_sInfo;
#endif
