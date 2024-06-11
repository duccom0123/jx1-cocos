#include "KCore.h"
#include "KThiefSkill.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
//#include "scene/KScenePlaceC.h"
//#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "imgref.h"
#include "KMagicDesc.h"
//#include "KOption.h"
#include "KMissle.h"

KThiefSkill ::KThiefSkill()
{
	m_ulAttackRadius = 50;
	m_ulDelayPerCast = 40;
	m_nSkillCostType = attrib_mana;
	m_szTargetMovie[0] = 0;
	m_szSkillSound[0] = 0;
	m_nCost = 50;
}

KThiefSkill::~KThiefSkill()
{

}

int		KThiefSkill::GetSkillId()
{
	return 400;
	
}

const char *KThiefSkill::GetSkillName()
{
	return m_szSkillName;
}

int		KThiefSkill::GetSkillStyle()
{
	return SKILL_SS_Thief;
}



void	KThiefSkill::LoadSkillLevelData(unsigned long  ulLevel, int nParam)
{
	
	return;
}

int		KThiefSkill::DoSkill(KNpc * pNpc, int nX, int nY)
{
	//_ASSERT(pNpc);
	if  (!pNpc)
		return 0;
	if (nX != -1 || nY <= 0)
		return 0;
	pNpc->m_Frames.nTotalFrame = pNpc->m_AttackFrame;

	//if (m_szSkillSpecialFile[0])
		//pNpc->m_DataRes.SetSpecialSpr(m_szSkillSpecialFile);

		pNpc->m_ClientDoing = cdo_stand;

		TMissleForShow Show;
		Show.nLauncherIndex = pNpc->m_Index;
		Show.nNpcIndex = nY;
		//KMissle::CreateMissleForShow("\\spr\\skill\\NPC\\mag_npc_毒水消失.spr", "30,1,1", "\\sound\\skill\\狂雷震地.wav", &Show);
		KMissle::CreateMissleForShow(m_szTargetMovie, m_szTargetMovieParam, m_szSkillSound, &Show);
	pNpc->m_Doing = do_special2;  //偷窃技能
	pNpc->m_ProcessAI = 0;
	pNpc->m_Frames.nCurrentFrame = 0;	
	return 1;
}
//获取对方信息技能
int KThiefSkill::Cast(KPlayer * pLaucherPlayer, KPlayer * pTargetPlayer)
{
	return TRUE;
}

int KThiefSkill::OnSkill(KNpc * pNpc)
{
	//_ASSERT(pNpc);
	
	if (!pNpc)
		return 0;
	KPlayer * pLauncherPlayer	= NULL;
	KPlayer * pTargetPlayer		= NULL;
	
	//pNpc->m_DataRes.SetBlur(FALSE);

	if (pNpc->m_DesX == -1) 
	{
		if (pNpc->m_DesY <= 0) 
			goto Label_ProcessAI1;
		
		//此时该角色已经无效时
		if (Npc[pNpc->m_DesY].m_RegionIndex < 0) 
			goto Label_ProcessAI1;
	}
	
	if (pNpc->m_nPlayerIdx < 0 || pNpc->m_DesY < 0 || pNpc->m_DesY >= MAX_NPC)
		goto Label_ProcessAI1;
	
	pLauncherPlayer = &Player[pNpc->m_nPlayerIdx];
	pTargetPlayer = &Player[Npc[pNpc->m_DesY].m_nPlayerIdx];
	
	Cast(pLauncherPlayer, pTargetPlayer); //显示对方的信息 金钱 生命 内力等

	pNpc->m_SkillList.SetNextCastTimeByIndex(pNpc->m_ActiveSkListIndex, SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime + GetDelayPerCast());
	
Label_ProcessAI1:
	if (pNpc->m_Frames.nTotalFrame == 0)
	{
		pNpc->m_ProcessAI = 1;
	}
	
	return 1;
	
}

BOOL	KThiefSkill::CanCastSkill(int nLauncher, int &nParam1, int &nParam2)  const 
{
	if (nParam1 != -1)
		return FALSE;
	
	if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return FALSE;
	NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);

	if (Npc[nParam2].m_Index <= 0 || Npc[nLauncher].m_Index <= 0) 
		return FALSE;
	
	if (Npc[nParam2].m_Camp == camp_begin || Npc[nParam2].m_Camp == camp_event)
		return FALSE;
		int distance = NpcSet.GetDistance(nLauncher, nParam2);
		if (distance > (int)m_ulAttackRadius)
			return FALSE;
	return TRUE;
}

int	KThiefSkill::NewCanCastSkill(int nLauncher, int nParam1, int nParam2)
{
	if (nParam1 != -1)
		return FALSE;
	
	if ( nParam2 <= 0 || nParam2 >= MAX_NPC)
		return FALSE;
	
	return TRUE;
}

void	KThiefSkill::GetDesc(unsigned long ulSkillId, unsigned long ulCurLevel, char * pszMsg, int nOwnerIndex,  bool bGetNextLevelDesc)
{
	strcpy(pszMsg, m_szSkillDesc);
}

BOOL	KThiefSkill::LoadSetting(char * szSettingFile)
{
	//_ASSERT(szSettingFile);
	if  (!szSettingFile)
		return false;

	KTabFile TabFile ;
	if (!TabFile.Load(szSettingFile))
	{
//		g_DebugLog("无法打开%s", szSettingFile);
		return false;
	}
	int nCurVal = 0;
	TabFile.GetInteger(2, "AttackRadius", 100, (int *)&m_ulAttackRadius);
	TabFile.GetInteger(2, "TimePerCast", 100, (int*)&m_ulDelayPerCast);
	TabFile.GetInteger(2, "SkillCostType", 0, (int*)&m_nSkillCostType );
	TabFile.GetInteger(2, "Cost", 50, (int *)&m_nCost);
	TabFile.GetInteger(2, "ThiefPercent", 30, (int *)&m_nThiefPercent);

	TabFile.GetInteger(2, "TargetEnemy", TRUE, &nCurVal);
	 m_bTargetEnemy = (nCurVal>0)?true:false;
	TabFile.GetInteger(2, "TargetAlly",	 TRUE, &nCurVal);
	m_bTargetAlly   = (nCurVal>0)?true:false;
	TabFile.GetInteger(2, "TargetObj",	 FALSE, &nCurVal);
	m_bTargetObj    = (nCurVal>0)?true:false;
	TabFile.GetInteger(2, "ThiefStyle",		0, &m_nThiefStyle);
	TabFile.GetString(2, "SkillName", "", m_szSkillName, sizeof(m_szSkillName));
	TabFile.GetInteger(2, "CostUsePrecent", 1, &nCurVal);
	m_bUseCostPercent = (nCurVal>0)?true:false;
	if (m_nThiefStyle == thief_runscript)
	{
		char szScript[200];
		TabFile.GetString(2, "Param1", "", szScript, sizeof(szScript));
		g_StrLower(szScript);
		m_dwParam1 =g_CheckFileExist(szScript);
	}
	else
	{
		TabFile.GetInteger(2, "Param1", 0, (int*)&m_dwParam1);
	}
	TabFile.GetString(2, "Desc", "", m_szSkillDesc, sizeof(m_szSkillDesc));
	TabFile.GetString(2, "SkillIcon", "\\spr\\skill\\图标\\通用.spr", m_szSkillIcon, sizeof(m_szSkillIcon));
	TabFile.GetString(2, "Movie", "", m_szSkillSpecialFile, sizeof(m_szSkillSpecialFile));
	TabFile.GetString(2, "TargetMovie", "", m_szTargetMovie, sizeof(m_szTargetMovie));
	TabFile.GetString(2, "TargetMovieInfo", "1,1,1", m_szTargetMovieParam, sizeof(m_szTargetMovieParam));
	TabFile.GetString(2, "SkillSound", "", m_szSkillSound, sizeof(m_szSkillSound));

	TabFile.Clear();
	return TRUE;

}

int		KThiefSkill::GetSkillCost(void * pParam) const
{
	if (m_bUseCostPercent)
	{
		//_ASSERT(pParam);
		if (!pParam)
			return 100;
		KNpc * pNpc = (KNpc*)pParam;
		int nValue = 1;
		if (pNpc->m_Index > 0)
		{
			switch(m_nSkillCostType)
			{
			case	attrib_mana:
			case	attrib_maxmana:
				{
					nValue = pNpc->m_CurrentManaMax;
				}
				break;
			case attrib_stamina:
			case attrib_maxstamina:				
				{
					nValue = pNpc->m_CurrentStaminaMax;
				}
				break;
			case attrib_life:
			case attrib_maxlife:
				{
					nValue = pNpc->m_CurrentLifeMax;
				}break;
			default:
				nValue = 1;
			}
			
			nValue = m_nCost * nValue / 100;
			
		}
		else
		{
			return nValue;
		}
	}
	else
		return m_nCost;
	
	return 100;
}

//画图标
void	KThiefSkill::DrawSkillIcon(int x, int y, int Width, int Height,int nParam)  
{
	if (!m_szSkillIcon[0]) return ;
	
	m_RUIconImage.nType = ISI_T_SPR;
	m_RUIconImage.Color.Color_b.a = 255;
	m_RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_RUIconImage.uImage = 0;
	m_RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	m_RUIconImage.bRenderFlag = 0;
	strcpy(m_RUIconImage.szImage, m_szSkillIcon);
	m_RUIconImage.oPosition.nX = x;
	m_RUIconImage.oPosition.nY = y;
	m_RUIconImage.oPosition.nZ = 0;
	m_RUIconImage.nFrame = 0;
//	g_pRepresent->DrawPrimitives(1, &m_RUIconImage, RU_T_IMAGE, 1);
}
