#ifndef	KSkillListH
#define	KSkillListH
//#include "Windows.h"
#include "GameDataDef.h"

#define	MAX_NPCSKILL	80
#define	MAX_TRAPNUM		5

/*typedef struct
{
	int		nPlace;
	int		nX;     // 0 1 2 3 4
	int		nY;     // 0123456789
} SkillPos;*/

typedef struct tagNpcSkill
{
	int		SkillId;
	int		SkillLevel;         //该位置的实际等级（除装备 技能的附加值）
	int		MaxTimes;
	int		RemainTimes;
	DWORD	NextCastTime;		//下次可发送时的最小时间
	DWORD	NextHorseCastTime;  //下次骑马可发送时的最小时间
	int		CurrentSkillLevel;  //该位置当前技能的等级（包含 装备 技能的等级附加值）
	int		mAddPoint;          //该位置的额外技能点 （装备 对技能的 附加值）
	int		EnChance;           //技能加成百分比
	int     nSkillStyle;        //这个位置的技能类型(主动 被动 攻击 子弹)
	int     nEquiptLimited;     //给这个技能位置设置武器限制的类型 -2 为无限制
	int     nHorseLimited;      //设置该位置的技能是否是骑马限制的
	BYTE    nIsExpSkill;        //是否是熟练度技能
	int     nExpSkill;          //该位置的总熟练度
	int     nCurSkillExp;       //当前熟练度 存档
	int     nTempEnChance;      //备份的技能原始的加成数据
} NPCSKILL, *LPNPCSKILL;

//struct TDBPlayerData;

class  KSkillList
{
public:
	int			m_nTrapNpcID[5];
	NPCSKILL	m_Skills[MAX_NPCSKILL];//技能列表 下标从1开始，0为无效index
    int			m_nNpcIndex;
public:
	KSkillList();
	~KSkillList();
	void		ReEnChance();
	int	        CheckNoSkill(char *nKey,int nSkillidx);
	int			Add(int nSkillID, int nLevel = 1, int nMaxTimes = 0, int RemainTimes = 0, int nAdd = 0,int nCurExpSkill=0,int nExpSkillPonit=0);//上线添加技能 和 使用技能调用的函数
	int		    Remove(int nSkillID);
	void        RemoveAllSkill();  //删除全部技能

	int GetSkillId(unsigned long ulSkillIdx) const
	{
		if (ulSkillIdx >= MAX_NPCSKILL || ulSkillIdx == 0) 
			return 0;
		else 
			return m_Skills[ulSkillIdx].SkillId;
	};

	BOOL CheckHaveSkill(unsigned long ulSkillId) const
	{
		for (int i=1;i<MAX_NPCSKILL;i++)
		{
			if 	(m_Skills[i].SkillId==ulSkillId)
				return TRUE;
		}
		return FALSE;
	};


	int	SetSkillLevelDirectlyUsingIndex(unsigned long ulSkillIdx, unsigned long ulLevel);/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
	
	int	SetSkillLevelDirectlyUsingId(unsigned long ulSkillId, unsigned long ulLevel);/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
	//void SetSkillLeftTime(int nSkillId,int nTime);
	void		Clear(){memset(m_Skills, 0, sizeof(m_Skills));};

	void		SetAddPoint(int nId, int nPoint);
	void		SetSkillEnChance(int nId, int nEnChance);
	void		AddCEnChance(int nId, int nEnChance);
	void		SetSkillLevel(int nId, int nLevel);
	BOOL		SetLevel(int nIndex, int nLevel);		// 把某个编号技能设为某一级
	int			GetSkillSortList(KUiSkillData *);		// 获得角色当前所有技能排序列表
	int			GetSkillPosition(int nSkillId);         //获得技能在技能界面的位置
	int			GetLeftSkillSortList(KUiSkillData*);	// 获得角色当前左键技能排序列表
	int			GetRightSkillSortList(KUiSkillData*);	// 获得角色当前右键技能排序列表
//  int         Add(int nIdx, int nPlace, int nX, int nY, BOOL bInit);

	BOOL		IncreaseLevel(int nIdx, int nLvl, int Qeuip = FALSE,BOOL nIsClearExp=FALSE);//重新设定技能数据，包括解析脚本数据
	int			GetLevel(int nSkillID);
	int         AddSkillExp(int nSkillID,int nCurExp=0,int nType=1);
	int         GetCurSkillExp(int nSkillID);
	int         GetSkillExp(int nSkillID);
    void        SetSkillExp(int nSkillID,int nExp);
	int         IsSkillExp(int nSkillID);

	int         GetLevelByListidx(int nListidx);
	int         AddSkillExpByListidx(int nListidx,int nCurExp=0,int nType=1);
	int         GetCurSkillExpByListidx(int nListidx);
	int         GetSkillExpByListidx(int nListidx);
	void        SetSkillExpByListidx(int nListidx,int nExp);
	int         IsSkillExpByListidx(int nListidx);
	BOOL        SetSLevelByListidx(int nListidx, int nLevel,BOOL nKind=FALSE);

	BOOL		SetSLevel(int nSkillID, int nLevel,BOOL nKind=FALSE);
	int			GetSkillIdxLevel(int nIdx) 
	{
		if (nIdx <= 0) return 0;
		return m_Skills[nIdx].SkillLevel;
	};
	int			GetCurrentLevel(int nSkillID);
	int         GetCurrentLevelByIdx(int nListIdx);
	int			GetCount();
	int			FindSame(int nSkillID);                //查找该技能在技能列表中的位置
	int			FindFree();

	/*
	BOOL		CanCast(int nSkillID, DWORD dwTime);   //检查时间间隔 ，是否能发技能
	void		SetNextCastTime(int nSkillID, DWORD dwTime);
	void		SetHorseNextCastTime(int nSkillID, DWORD dwTime);
	int 		GetHorseNextCastTime(int nSkillID);
	int		    GetNextCastTime(int nSkillID);
	*/
	BOOL		CanCast(int nSkillID, DWORD dwTime);   //检查时间间隔 ，是否能发技能
	BOOL        CanCastByIndex(int nSkListIndex, DWORD dwTime);
	void		SetNextCastTime(int nSkillID, DWORD dwTime);
	void		SetNextCastTimeByIndex(int nSkListIndex, DWORD dwTime);
	void		SetHorseNextCastTime(int nSkillID, DWORD dwTime);
	void		SetHorseNextCastTimeByIndex(int nSkListIndex, DWORD dwTime);
	int 		GetHorseNextCastTime(int nSkillID);
	int         GetHorseNextCastTimeByIndex(int nSkListIndex);
	int		    GetNextCastTime(int nSkillID);
	int		    GetNextCastTimeByIndex(int nSkListIndex);

	int			GetSkillIdByListidx(int nListIndex)
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].SkillId;
		else 
			return -1;
	};
	int			GetSkillStyle(int nListIndex)
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].nSkillStyle;
		else 
			return -1;
	};

	
	int			GetAddPoint(int nListIndex) //或取某个位置技能的额外技能点（装备等）
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].mAddPoint;
		else 
			return 0;
	};

	int			GetEnChance(int nListIndex)                 //服务器端当前技能受的技能加成
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].EnChance;
		else 
			return 0;
	};

	int			GetTempEnChance(int nListIndex)                 //服务器端当前技能原始的受的技能加成
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return 	m_Skills[nListIndex].nTempEnChance;
		else 
			return 0;
	};

	void		QeuipAddPoint(int nListIndex,int sPoint)    //设置服务器端需要额外增加或减少的技能点
	{
			m_Skills[nListIndex].mAddPoint += sPoint;	   
	};

	void		SetEnChance(int nListIndex,int sEnChance)   //直接设置技能加成的总数
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			m_Skills[nListIndex].EnChance = sEnChance;
		else
			m_Skills[nListIndex].EnChance = 0;

		if (m_Skills[nListIndex].EnChance<0)
		{
			m_Skills[nListIndex].EnChance=0;
			m_Skills[nListIndex].nTempEnChance = 0;
		}
	};
	void		AddEnChance(int nListIndex,int sEnChance)   //服务器端当前技能受技能加成的总数
	{
		    m_Skills[nListIndex].EnChance += sEnChance; 

			if (m_Skills[nListIndex].EnChance<0)
			{
				m_Skills[nListIndex].EnChance=0;
				m_Skills[nListIndex].nTempEnChance =0;
			}
			
	};
};
#endif
