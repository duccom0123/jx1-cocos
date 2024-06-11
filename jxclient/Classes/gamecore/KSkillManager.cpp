 
#include "KCore.h"
#include "KSkillManager.h"
//#include "MyAssert.H"
#include "KThiefSkill.h"

unsigned long g_ulSkillCount = 0;
KSkillManager g_SkillManager;

KSkillManager::KSkillManager()
{
	 nSkillNum=0;
}

// Add By Freeway Chen in 2003.6.6
KSkillManager::~KSkillManager()
{
    unsigned long i = 0;
    unsigned long j = 0;
    
    for (i = 0; i < MAX_SKILL; ++i)
    {
        for (j = 0; j < MAX_SKILLLEVEL; ++j)
        {
            if (m_pOrdinSkill[i][j])
            {
                delete m_pOrdinSkill[i][j];
                m_pOrdinSkill[i][j] = NULL;
            }
        }
    }
}

BOOL KSkillManager::Init()
{
    int i = 0;
    int j = 0;
    for (i = 0; i < MAX_SKILL; ++i)
    {
        for (j = 0; j < MAX_SKILLLEVEL; ++j)
        {
            m_pOrdinSkill[i][j] = NULL;
        }
    }

	memset(m_SkillInfo, 0,  sizeof(m_SkillInfo));

	//	Load OrdinSkill Info 
	   nSkillNum = g_OrdinSkillsSetting.GetHeight();

	if (nSkillNum <= 0 ) 
		return FALSE;
	
	for (i = 0; i < nSkillNum-1; ++i)
	{
		int nSkillId = -1;
		int nSkillStyle = -1;
		int nSkillMaxLevel = 0;
		g_OrdinSkillsSetting.GetInteger(i + 2, "SkillId", -1, &nSkillId);
		g_OrdinSkillsSetting.GetInteger(i + 2, "SkillStyle", -1, &nSkillStyle);
		g_OrdinSkillsSetting.GetInteger(i + 2, "MaxLevel", 20, &nSkillMaxLevel);
		
		//_ASSERT(nSkillMaxLevel >= 0);
		if  (nSkillMaxLevel<0)
			nSkillMaxLevel = 0;
		
		if (nSkillId > 0 && nSkillStyle >= 0)
		{
			m_SkillInfo[nSkillId - 1].m_nSkillStyle = nSkillStyle;
			m_SkillInfo[nSkillId - 1].m_nTabFileRowId = i + 2;
			m_SkillInfo[nSkillId - 1].m_ulMaxSkillLevel = nSkillMaxLevel;
		}
		else
		{    //结构体
			m_SkillInfo[nSkillId - 1].m_nSkillStyle = -1;
			m_SkillInfo[nSkillId - 1].m_nTabFileRowId = 0;
			m_SkillInfo[nSkillId - 1].m_ulMaxSkillLevel = 0;
		}
	}
	KTabFile ThiefSkillTab;
	if (ThiefSkillTab.Load(THIEFSKILL_SETTINGFILE))
	{
		int nSkillId = 0;
		ThiefSkillTab.GetInteger(2, "SkillId", 400, &nSkillId);   //抢富济贫
		//_ASSERT(nSkillId > 0);
		if (nSkillId>0)
		{
			m_SkillInfo[nSkillId - 1].m_nSkillStyle = SKILL_SS_Thief;
			m_SkillInfo[nSkillId - 1].m_nTabFileRowId = 2;
			m_SkillInfo[nSkillId - 1].m_ulMaxSkillLevel = 1;
		}
	}
	ThiefSkillTab.Clear();
	//printf(" KSkillManager::Init(%d)MUN(%d) OK..!!!!\n",MAX_SKILL,nSkillNum);
	return TRUE;
}
//解析技能脚本数据
ISkill*	KSkillManager::InstanceSkill( unsigned long ulSkillID, unsigned long ulSkillLevel)
{
	ISkill *pRetSkill = NULL;

    int nStyle = GetSkillStyle(ulSkillID);
	
	switch (nStyle) // eSkillStyle
	{
	case SKILL_SS_Missles:			        //	子弹类 0	本技能用于发送子弹类
	case SKILL_SS_Melee:                    //         1    攻击技能
	case SKILL_SS_InitiativeNpcState:	    //	主动类 2	本技能用于改变当前Npc的主动状态
	case SKILL_SS_PassivityNpcState:		//	被动类 3	本技能用于改变Npc的被动状态
		{
            KSkill * pNewOrdinSkill = NULL;
			unsigned long ulFirstLoadLevel = 0;

            if (m_pOrdinSkill[ulSkillID - 1][ulSkillLevel - 1])
            { 
                pRetSkill = m_pOrdinSkill[ulSkillID - 1][ulSkillLevel - 1];
				break;
            }

			pNewOrdinSkill   = new KSkill;  //为新技能文件 开内存

			if  (pNewOrdinSkill==NULL)
				break;

			ulFirstLoadLevel = m_SkillInfo[ulSkillID - 1].m_ulFirstLoadLevel;

			if (!ulFirstLoadLevel)  //如果是没有加载过的
			{   //从原始TXT获取技能数据
				pNewOrdinSkill->GetInfoFromTabFile(m_SkillInfo[ulSkillID - 1].m_nTabFileRowId);
				//从脚本获取技能数据更新原始TXT数据
				pNewOrdinSkill->LoadSkillLevelData(ulSkillLevel, m_SkillInfo[ulSkillID - 1].m_nTabFileRowId);
				m_SkillInfo[ulSkillID - 1].m_ulFirstLoadLevel = ulSkillLevel;
			}
			else
			{
				//_ASSERT(m_pOrdinSkill[ulSkillID - 1][ulFirstLoadLevel - 1]);  //有老数据的为前提条件
                if (!m_pOrdinSkill[ulSkillID - 1][ulFirstLoadLevel - 1])
					break;

				*pNewOrdinSkill = *(KSkill*)m_pOrdinSkill[ulSkillID - 1][ulFirstLoadLevel - 1];
				//从脚本获取技能数据
				pNewOrdinSkill->LoadSkillLevelData(ulSkillLevel, m_SkillInfo[ulSkillID - 1].m_nTabFileRowId);
			 }
			
			pNewOrdinSkill->SetSkillId(ulSkillID);
			pNewOrdinSkill->SetSkillLevel(ulSkillLevel);

			m_pOrdinSkill[ulSkillID - 1][ulSkillLevel - 1] = pNewOrdinSkill;

			pRetSkill = pNewOrdinSkill;
            pNewOrdinSkill = NULL;		
        }
		break;
	case SKILL_SS_Thief:
		{
			if (!m_pOrdinSkill[ulSkillID - 1][0])
			{
				m_pOrdinSkill[ulSkillID - 1][0] = (ISkill*)new KThiefSkill;

				if  (m_pOrdinSkill[ulSkillID - 1][0]==NULL)
					break;

			}

			pRetSkill = m_pOrdinSkill[ulSkillID - 1][0];
			
			((KThiefSkill*)pRetSkill)->LoadSetting(THIEFSKILL_SETTINGFILE);

		}
		break;	
	default:
		break;
	}
    		
	return pRetSkill;
}


