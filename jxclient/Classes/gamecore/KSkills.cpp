            // KSkills.cpp: implementation of the KSkills class.
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"

#include "engine/KSG_StringProcess.h"
#include "engine/Text.h"
#include "KSkills.h"
#include "KMissle.h"
#include "KMissleSet.h"
#include "KNpc.h"
#include "KNpcAI.h"
#include "math.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KMath.h"
#include "engine/KEngine.h"
#include "engine/KTabFile.h"
#include "engine/KTabFileCtrl.h"
#include "KMissleMagicAttribsData.h"
#include "KPlayer.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
//#include "scene/KScenePlaceC.h"
//#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "imgref.h"
#include "KMagicDesc.h"
//#include "KOption.h"
//#endif

//#define SHOW_SKILL_MORE_INFO
#define	 NPCINDEXOFOBJECT 0 //物件发魔法时所对应的Npc编号
const char * g_MagicID2String(int nAttrib);
extern  const KScript * g_GetScript(DWORD dwScriptId);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
KNpcMissileList::KNpcMissileList(INT nNpcIndex)
{
	m_nNpcIndex	= nNpcIndex;
	m_nCount	= 0;
}

// ----------------------------------------------------------------------------
KNpcMissileList::~KNpcMissileList()
{
	DelMissile(m_nCount);
	//_ASSERT(m_nCount == 0);
}

// ----------------------------------------------------------------------------
BOOL KNpcMissileList::OnCreated(INT nIndex, INT nMaxCount)
{
	if (nMaxCount <= 0)
	{
		return FALSE;
	}
	OnVanish(nIndex);	// 避免表中含有遗留的错误的同Index子弹
	KIdxNode* pNode	= new KIdxNode();
	pNode->m_nIndex	= nIndex;
	m_listIndex.AddTail(pNode);
	if (++m_nCount >= nMaxCount)
		DelMissile(m_nCount - nMaxCount);
	return TRUE;
}

// ----------------------------------------------------------------------------
BOOL KNpcMissileList::OnVanish(INT nIndex)
{
	KIdxNode* pNode	= (KIdxNode*)m_listIndex.GetHead();
	while (pNode)
	{
		if (pNode->m_nIndex == nIndex)
		{
			pNode->Remove();
			 delete  pNode;
			 pNode = NULL;
			
			m_nCount --;
			
			return TRUE;
		}
		pNode	= (KIdxNode*)pNode->GetNext();
	}
	return FALSE;
}

// ----------------------------------------------------------------------------
BOOL KNpcMissileList::DelMissile(INT nCount)
{
	KIdxNode* pNode	= NULL;
	while (nCount-- > 0)
	{
		pNode = (KIdxNode*)m_listIndex.RemoveHead();
		if (!pNode)
		{
			return FALSE;
		}
		
		if (Missle[pNode->m_nIndex].m_nLauncher == m_nNpcIndex)
			Missle[pNode->m_nIndex].Remove();
		
		//SAFE_DELETE(pNode);
		delete pNode;
		pNode = NULL;
		
		m_nCount --;
	}
	
	return TRUE;
}

// ----------------------------------------------------------------------------

/*!*****************************************************************************
// Function		: KSkill::KSkill
// Purpose		: 
// Return		: 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
KSkill::KSkill()
{
	m_nFlySkillId =  m_nCollideSkillId = m_nVanishedSkillId = 0;
	
    // add by FreewayChen in 2003.6.6
    m_nImmediateAttribsNum = m_nStateAttribsNum = m_nMissleAttribsNum = m_nDamageAttribsNum = 0;
	m_nSkillCostType = attrib_mana;
    m_nWaitTime = 0;
	m_nEquiptLimited = 0;
	m_nHorseLimited  = 0;
	m_bDoHurt = 0;
	m_nSKillExp=0;
	ZeroMemory(m_szSkillDesc,sizeof(m_szSkillDesc));
	//m_szSkillDesc[300] ={0};
	//m_szDesc[512] = {0};
	ZeroMemory(m_szDesc,sizeof(m_szDesc));
	ZeroMemory(m_szManPreCastSoundFile,sizeof(m_szManPreCastSoundFile));
	ZeroMemory(m_szFMPreCastSoundFile,sizeof(m_szFMPreCastSoundFile));
	
}
/*!*****************************************************************************
// Function		: KSkill::~KSkill
// Purpose		: 
// Return		: 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
KSkill::~KSkill()
{
	
}

/*!*****************************************************************************
// Function		: KSkill::Param2PCoordinate
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nParam3
// Argumant		: int *npPX
// Argumant		: int *npPY
// Comments		:
// Author		: RomanDou
*****************************************************************************/
inline int	KSkill::Param2PCoordinate(int nLauncher, int nParam1, int nParam2 , int *npPX, int *npPY, eSkillLauncherType eLauncherType)  const 
{
	
	int nRegionId, nDesMapX, nDesMapY ;
	int nTargetId = -1;
	if (eLauncherType == SKILL_SLT_Obj)  //子弹取消对物品有效
		return 0;
	
	switch(nParam1)
	{
	case -1://nParam2 参数指向某个Npc，或Obj的Index
		nTargetId		= nParam2;
		nRegionId		= Npc[nParam2].m_RegionIndex;
		nDesMapX		= Npc[nParam2].m_MapX;
		nDesMapY		= Npc[nParam2].m_MapY;
		
		if (eLauncherType == SKILL_SLT_Npc)
			SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(nRegionId, nDesMapX , nDesMapY, Npc[nParam2].m_OffX , Npc[nParam2].m_OffY, npPX, npPY);
		else if(eLauncherType == SKILL_SLT_Obj)
			SubWorld[Object[nLauncher].m_nSubWorldID].NewMap2Mps(nRegionId, nDesMapX, nDesMapY, Object[nParam2].m_nOffX , Object[nParam2].m_nOffY, npPX, npPY);
		//else

		break;
	case -2:   //nParam 参数指向某个方向
		
		break;
	default:   //默认时, nParam1 与nParam2 为实际点坐标
		*npPX = nParam1;
		*npPY = nParam2;
		break;
	}
	
	if (*npPX < 0 || *npPY < 0)	
		printf("--函数获得参数违法！nParam1 ,nParam2 [%d,%d]--\n", nParam1, nParam2);
	
	return nTargetId;
}
//是否能使用技能
BOOL KSkill::CanCastSkill(int nLauncher, int &nParam1, int &nParam2) const 
{
	//对自已的主动辅助技能
	if (m_bTargetSelf && nParam1 != -1) 
	{
		nParam1 = -1;
		nParam2 = nLauncher;
		return TRUE;
	}
	else
	{
		//if (m_bTargetOnly && nParam1 != -1) 
		//	return FALSE;
		
		if (nParam1 == -1)
		{
			if ( nParam2 <= 0 || nParam2 >= MAX_NPC) 
				return FALSE;
			NPC_RELATION  Relation = NpcSet.GetRelation(nLauncher, nParam2);
			
			if (m_bTargetEnemy) //敌人
			{
				if (Relation & relation_enemy) goto relationisvalid;
			}
			
			if (m_bTargetAlly)  //同盟
			{
				if (Relation & relation_ally) goto relationisvalid;
			}
			
			if (m_bTargetSelf)  //自己
			{
				if (Relation & relation_self) goto relationisvalid;
			}

            if (m_bTargetOther) //其他的
			{
                if (Relation & relation_none) goto relationisvalid;
                if (Relation & relation_dialog) goto relationisvalid;
			}

			return FALSE;
		}
		
	}
	
relationisvalid:

	if (Npc[nLauncher].IsPlayer())
	{
	/*	if (IsPhysical()) //是物理攻击？是否跟武器的技能相符合
		{
			int nWeapoinSkill = Npc[nLauncher].GetCurActiveWeaponSkill();
			if ((DWORD)nWeapoinSkill != m_nId)
			{
				return FALSE;
			}
		}
    */
		//-2表示技能不受当前装备的限制,
		//-1表示空手限制
		//0-99受某种近身攻击类的装备限制 取值为该装备的具体类别号
		//100-199受某种远程攻击类的装备限制 取值为该装备的具体类别号 加100

		if (-2 != m_nEquiptLimited)
		{//装备限制
			int nPlayerIdx		= CLIENT_PLAYER_INDEX;
			int nDetailType		= Player[nPlayerIdx].m_ItemList.GetWeaponType();
			int nParticularType = Player[nPlayerIdx].m_ItemList.GetWeaponParticular();
			
			//近身武器
			if (nDetailType == 0)
			{
				if (nParticularType==6)
                     nParticularType = -1;

			}//远程武器
			else if (nDetailType == 1)
			{//暗器
				nParticularType += MAX_MELEEWEAPON_PARTICULARTYPE_NUM;
			}//空手
			else if (nDetailType == -1)
			{
				nParticularType = -1;
			}
			
			if (nParticularType != m_nEquiptLimited)  //是否可以发技能 装备对技能的限制
				return FALSE;
		}
		
		//0表示不限制
		//1表示不可以骑马发该技能
		//2表示必须骑马发该技能
		if (m_nHorseLimited)
		{
			switch(m_nHorseLimited)
			{
			case 1:
				{
					if (Npc[nLauncher].m_bRideHorse)
						return FALSE;
				}
				break;
			case 2:
				{
					if (!Npc[nLauncher].m_bRideHorse)
						return FALSE;
				}
				break;
			default:
				return FALSE;
			}
		}
		
		if ((m_bTargetOnly) && nParam1 == -1)
		{
			int distance = NpcSet.GetDistance(nLauncher, nParam2);
			if (distance > GetAttackRadius()) return FALSE;
		}
		/*else
		{
		if (nParam1 < 0 || nParam2 < 0) return FALSE;
		
		  #ifndef _SERVER
		  int nLauncherPX = 0, nLauncherPY = 0;
		  Npc[nLauncher].GetMpsPos(&nLauncherPX, &nLauncherPY);
		  int ndistance = g_GetDistance(nLauncherPX, nLauncherPY, nParam1, nParam2);
		  if (ndistance > GetAttackRadius()) return FALSE;
		  #endif
		  }
		*/
	}
	return TRUE;
}
//是否能使用技能
int KSkill::NewCanCastSkill(int nLauncher, int nParam1, int nParam2) 
{
	if (Npc[nLauncher].IsPlayer())
	{
		//-2表示技能不受当前装备的限制,
		//-1表示空手限制
		//0-99受某种近身攻击类的装备限制 取值为该装备的具体类别号
		//100-199受某种远程攻击类的装备限制 取值为该装备的具体类别号 加100
		if (-2 != GetEquiptLimited()) //m_nEquiptLimited
		{//装备有限制
          int nPlayerIdx;
			  nPlayerIdx		= CLIENT_PLAYER_INDEX;
			int nDetailType		= Player[nPlayerIdx].m_ItemList.GetWeaponType();
			int nParticularType = Player[nPlayerIdx].m_ItemList.GetWeaponParticular();
			
			//近身武器
			if (nDetailType == 0)
			{
				if (nParticularType==6)  //缠手
                     nParticularType = -1;
				
			}//远程武器
			else if (nDetailType == 1)
			{//暗器
				nParticularType += MAX_MELEEWEAPON_PARTICULARTYPE_NUM;
			}
			else if (nDetailType == -1)
			{//空手
				nParticularType = -1;
                //return -1;
			}
			//m_nEquiptLimited
			if (nParticularType != GetEquiptLimited())  //是否可以发技能 装备对技能的限制
				return 0;
		}
		
		//0表示不限制
		//1表示不可以骑马发该技能
		//2表示必须骑马发该技能
	/*	if (m_nHorseLimited)
		{
			switch(m_nHorseLimited)
			{
			case 1:
				{
					if (Npc[nLauncher].m_bRideHorse)
						return FALSE;
				}
				break;
			case 2:
				{
					if (!Npc[nLauncher].m_bRideHorse)
						return FALSE;
				}
				break;
			default:
				return FALSE;
			}
		}
		*/
		return 1;
	}
	return 0;
}

//		当玩家调用某个技能时发生 [5/28/2002]
//		客户端和服务器端在技能的调用方面有一些不同
//		服务器端一般收到的经过客户端处理完的参数
//		游戏世界以消息命令机制执行每个变化，因此对于发技能应该也是统一数据接口
//		客户端时，如果该消息是来自本机玩家的输入，则必须将其转换为实际的消息
//		来执行。同时还应将转换好的消息传给服务器端
/*
有关传的参数是MapX，还是PointX根据具体的魔法技能而定
比如一般飞行魔法为Map坐标，而定点魔法为Point坐标
*/
/*
注意当调用Cast时，必须已确保当前的nLauncherIndex与Socket相对应的dwId一致，即IsMatch()通过。
*/

/*!*****************************************************************************
// Function		: KSkill::Cast
// Purpose		: 发技能的统一接口
// Return		: 
// Argumant		: int nLauncher 发送者Id
// Argumant		: int nParam1   
// Argumant		: int nParam2
// Argumant		: int nWaitTime 发送的延迟时间
// Argumant		: eSkillLauncherType eLauncherType 发送者类型
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::Cast(int nLauncher, int nParam1, int nParam2, int nWaitTime, eSkillLauncherType eLauncherType,int nMaxShangHai,int nIsEuq)  const 
{
	//-----------------接口函数入口点，检测参数合法性-------------------------------
    //if (!CanCastSkill(nLauncher,nParam1,nParam2))
    //return FALSE;
	
	if (nLauncher < 0 )
	{
		//printf("Skill::Cast(), nLauncher < 0 , Return False;\n"); 
		return FALSE; 
	}
//===============================检查发送者是否符合要求==================================
	switch(eLauncherType)
	{
	  case SKILL_SLT_Npc:
		{
			if (MAX_NPC <= nLauncher) 
				return FALSE;
			if (Npc[nLauncher].m_dwID < 0) 
				return FALSE;
			if (nParam1 == -1)
			{//被动技能？对自己使用
				if (nParam2 >= MAX_NPC)   //自己的索引
					return FALSE;	
				if ((Npc[nParam2].m_Index <= 0) || Npc[nLauncher].m_SubWorldIndex != Npc[nParam2].m_SubWorldIndex)
					return FALSE; 
			}
		}
		break;		
	  case SKILL_SLT_Obj:
		{
			return FALSE;   //物品类技能取消

			if (MAX_OBJECT <= nLauncher)
				return FALSE;
			if (Object[nLauncher].m_nDataID < 0) 
				return FALSE;
		}
		break;
	  case SKILL_SLT_Missle:
		{
			if (MAX_MISSLE <= nLauncher) 
				return FALSE;
			
			if (Missle[nLauncher].m_nMissleId < 0) 
				return FALSE;
			
			if (nParam1 == -1)
			{
				if (nParam2 >= MAX_NPC) 
					return FALSE;
				
				if ((Npc[nParam2].m_Index <= 0) ||  Missle[nLauncher].m_nSubWorldId != Npc[nParam2].m_SubWorldIndex)
					return FALSE;
			}
		}
		break;
	default:
		{
			return FALSE;
		}
	}
//=======================================================================================
	if (nParam1 < 0 && nParam2 < 0 )
		return FALSE;
	
	/*if (nLauncher>0 && nLauncher<MAX_NPC)
	{
		char msg[128]={0};
		t_sprintf(msg,"发技能:%s",Npc[nLauncher].Name);
		CCMessageBox(msg,"cast skill");	
	}*/
	
	switch(m_eSkillStyle)
	{
	//case    SKILL_SS_NewMissles:
	case	SKILL_SS_Missles:				//发子弹
		{
			if (nWaitTime < 0 ) 
			{
				nWaitTime = 0;
			}
		
			CastMissles(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType,nMaxShangHai);
//		    printf("[子弹技能]Skill::Cast(%d), ok....\n",nMaxShangHai);
		}
		break;		
	case	SKILL_SS_Melee:                 //主动攻击技能
		{		
		}
		break;		
	case	SKILL_SS_InitiativeNpcState:	//改变角色的主动状态
		{	
			if (nWaitTime < 0 ) 
			{
				nWaitTime = 0;
			} 

			CastInitiativeSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
//		    printf("[主动技能]Skill::Cast(%d), ok....\n",nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_PassivityNpcState:	//改变角色的被动状态	
		{
			CastPassivitySkill(nLauncher, nParam1, nParam2, nWaitTime,nIsEuq);
//		     printf("[被动技能]Skill::Cast(%d), ok....\n",nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_CreateNpc:			   //产生新的Npc、怪物 
		{
		//	CastCreateNpcSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_BuildPoison:			//炼毒术
		{
		//	CastBuildPoisonSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_AddPoison:			    //加毒术
		{
		//	CastAddPoisonSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_GetObjDirectly:		//隔空取物	
		{
			//CastGetObjDirectlySkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_StrideObstacle:		//跨越障碍
		{
			//CastStrideObstacleSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_BodyToObject:		    //尸变
		{
			//CastBodyToObjectSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_Mining:				//采矿
		{
			//CastMiningSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_RepairWeapon:		    //修复术  
		{
			//CastRepairWeaponSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
		
	case	SKILL_SS_Capture:				//捕捉术 
		{
			//CastCaptureSkill(nLauncher, nParam1, nParam2, nWaitTime,nMaxShangHai);
		}
		break;
	default :
		{//默认是产生子弹
			if (nWaitTime < 0 ) 
			{
				nWaitTime = 0;
			}
			CastMissles(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType,nMaxShangHai);
		}
		break;
	}
	//事件技能
	int mEventSkillLevel=0;
	if (m_bStartEvent && m_nStartSkillId > 0/* && m_nEventSkillLevel > 0*/)
	{//释放事件技能 子弹开始时
		if  (m_nEventSkillLevel==0)
			return FALSE;

		   if  (m_nEventSkillLevel<=-1)
			   mEventSkillLevel=m_ulLevel;
		   else
			   mEventSkillLevel= m_nEventSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, mEventSkillLevel);
			if (!pOrdinSkill) 
				return FALSE;
			
			pOrdinSkill->Cast(nLauncher, nParam1, nParam2, nWaitTime, eLauncherType,nMaxShangHai);

		
	
//	    printf("[子弹飞行技能]Skill::Cast(%d), ok....\n",nMaxShangHai);
/*		if (Npc[nLauncher].IsPlayer())
		{  
#ifndef _SERVER
		   char nMsg[64];
		   t_sprintf(nMsg,"开始技能:%d,时间:%d,人索引:%d",m_nStartSkillId,nWaitTime,nLauncher);
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
#endif
		}*/
	} 
	
	return TRUE;	  
}

/*!*****************************************************************************
// Function		: KSkill::Vanish
// Purpose		: 子弹生命结束时回调
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::Vanish(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_VanishEvent, pMissle);
}
//子弹事件
BOOL KSkill::OnMissleEvent(unsigned short usEvent, KMissle * pMissle)  const 
{
	if (!pMissle) 
        return FALSE;

	int nLauncherIdx = pMissle->m_nLauncher;
	
    if (
		pMissle->m_nMissleId <= 0 
		|| pMissle->m_nMissleId >= MAX_MISSLE 
		|| nLauncherIdx <= 0
		|| nLauncherIdx >= MAX_NPC
		|| Npc[nLauncherIdx].m_Index <= 0
		)
        return FALSE;

	
	if (
		(!Npc[nLauncherIdx].IsMatch(pMissle->m_dwLauncherId)) 
		|| Npc[nLauncherIdx].m_SubWorldIndex != pMissle->m_nSubWorldId
		|| Npc[nLauncherIdx].m_RegionIndex < 0
		)
	{
		return FALSE;
	}
	
	int nEventSkillId = 0;
	int nEventSkillLevel = 0;
	switch(usEvent)
	{
	case Missle_FlyEvent:     //子弹飞行时
		if (!m_bFlyingEvent || m_nFlySkillId <= 0/* || m_nEventSkillLevel <= 0*/)
			return FALSE;

		if (m_nEventSkillLevel==0)
			return FALSE;
		   
		nEventSkillId = m_nFlySkillId ;
		if (m_nEventSkillLevel<=-1)
		   nEventSkillLevel = m_ulLevel;
		else
		    nEventSkillLevel= m_nEventSkillLevel;

		break;
		
	case Missle_StartEvent:   //子弹开始时
		if (!m_bStartEvent || m_nStartSkillId <= 0/* || m_nEventSkillLevel <= 0*/)
			return FALSE;

		if (m_nEventSkillLevel==0)
			return FALSE;

		nEventSkillId    = m_nStartSkillId ;

		if (m_nEventSkillLevel<=-1)
			nEventSkillLevel = m_ulLevel;
		else
			nEventSkillLevel= m_nEventSkillLevel;

		break;
		
	case Missle_VanishEvent:  //子弹结束是产生技能子弹事件
		if (!m_bVanishedEvent || m_nVanishedSkillId <= 0/* || m_nEventSkillLevel <= 0*/)
			return FALSE;
		if (m_nEventSkillLevel==0)
			return FALSE;

		nEventSkillId    = m_nVanishedSkillId ;

		if (m_nEventSkillLevel<=-1)
			nEventSkillLevel = m_ulLevel;
		else
			nEventSkillLevel= m_nEventSkillLevel;

		break;
		
	case Missle_CollideEvent:  //碰撞子弹产生的技能事件
		if (!m_bCollideEvent || m_nCollideSkillId <= 0/* || m_nEventSkillLevel <= 0*/)
			return FALSE;

		if (m_nEventSkillLevel==0)
			return FALSE;

		nEventSkillId    = m_nCollideSkillId;


		if (m_nEventSkillLevel<=-1)
			nEventSkillLevel = m_ulLevel;
		else
			nEventSkillLevel= m_nEventSkillLevel;
//#ifdef _SERVER
//		printf("--技能碰撞效果:%d,技能:%d,脚本:%d--\n",nEventSkillLevel,m_ulLevel,m_nEventSkillLevel);
//#endif 


		break;
	default:
		return FALSE;
	}
		
	int nDesPX = 0, nDesPY = 0,nDmap=0;
	
	if (m_bByMissle)
	{//是否由子弹本身产生子弹
		pMissle->GetMpsPos(&nDesPX, &nDesPY);
	}
	else
	{//是否由玩家产生子弹
		Npc[nLauncherIdx].GetMpsPos(&nDesPX, &nDesPY,&nDmap);
	}
	
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nEventSkillId, nEventSkillLevel);
	if (!pOrdinSkill) 
        return FALSE;
	
	BOOL bRetCode = FALSE;
	
    if (m_bByMissle)  
	{//是否由子弹本身产生子弹
		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
		{   //使用子弹
			bRetCode = pOrdinSkill->CastMissles(pMissle->m_nMissleId, nDesPX, nDesPY, 0, SKILL_SLT_Missle);
		}
	}
	else
	{//是否由玩家产生子弹
		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
		{   
            bRetCode = pOrdinSkill->CastMissles(nLauncherIdx, nDesPX, nDesPY, 0, SKILL_SLT_Npc);
		}
	}
	
	return bRetCode;
}

/*!*****************************************************************************
// Function		: KSkill::FlyEvent
// Purpose		: 子弹飞行时
// Return		: void 
// Argumant		: int nMissleId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KSkill::FlyEvent(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_FlyEvent, pMissle);
}

/*!*****************************************************************************
// Function		: KSkill::Collidsion
// Purpose		: 子弹被撞时回调
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::Collidsion(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_CollideEvent, pMissle);
}

BOOL KSkill::__CastWall(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai ) const
{

	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;

	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	   //墙形魔法不可以只传方向
	if (nParam1 == SKILL_SPT_Direction) 
		return FALSE;
	
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{	
			nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
			
			if (Npc[nLauncher].m_SubWorldIndex < 0) 
			{
				return FALSE;
			}
			
			SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
			
			nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
			nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
			nDir = nDir + MaxMissleDir / 4;
			if (nDir >= MaxMissleDir) nDir -= MaxMissleDir;
			SkillParam.nLauncher = nLauncher;
			SkillParam.eLauncherType = eLauncherType;
			
			CastWall(&SkillParam , nDir, nDesPX, nDesPY,nMaxShangHai);
		}	break;
	case SKILL_SLT_Obj:
		{
		}break;
	case SKILL_SLT_Missle:
		{
			KMissle * pMissle = &Missle[nLauncher];
			if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
			
			SubWorld[Missle[nLauncher].m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
			int nDir = pMissle->m_nDir + MaxMissleDir / 4;
			if (nDir >= MaxMissleDir) nDir -= MaxMissleDir;
			SkillParam.nLauncher = pMissle->m_nLauncher;
			SkillParam.nParent = nLauncher;
			SkillParam.nParent = SKILL_SLT_Missle;
			SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
			CastWall(&SkillParam,  nDir, nRefPX, nRefPY,nMaxShangHai);
		}break;
	default:
		break;
	}

		return TRUE;
}


BOOL	KSkill::__CastLine(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai)	const
{

	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	if (nParam1 == SKILL_SPT_Direction)
	{
		switch(eLauncherType)
		{
		case SKILL_SLT_Npc:
			{
				SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
				if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
				nDir = nParam2;
				SkillParam.nLauncher = nLauncher;
				SkillParam.eLauncherType = eLauncherType;
				SkillParam.nTargetId = nTargetId;
				CastLine(&SkillParam, nDir, nSrcPX,nSrcPY,nMaxShangHai);
				
			}break;
		case SKILL_SLT_Obj:
			{
				
			}break;
		case SKILL_SLT_Missle:
			{
				KMissle * pMissle = &Missle[nLauncher];
				if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
				if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
				nDir = nParam2;
				SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
				SkillParam.nLauncher = pMissle->m_nLauncher;
				SkillParam.nParent = nLauncher;
				SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
				CastWall(&SkillParam, nDir,  nRefPX, nRefPY,nMaxShangHai);
			}break;
		default:
			break;
		}
		
	}
	else
	{
		switch(eLauncherType)
		{
		case SKILL_SLT_Npc:
			{
				nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
				SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
				nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
				nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
				SkillParam.nLauncher = nLauncher;
				SkillParam.eLauncherType = eLauncherType;
				SkillParam.nTargetId = nTargetId;  //目标NPC索引
				if (m_nChildSkillNum == 1 && (g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Line || g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Parabola) ) 
				{//单一子弹  直线子弹
					if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ; //如果目标坐标 和 发技能者坐标相同则返回

					nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
					
					if (nDistance == 0 ) return FALSE; //距离等于0  也返回

					int		nYLength = nDesPY - nSrcPY;
					int		nXLength = nDesPX - nSrcPX;
					int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍
					int		nCos = (nXLength << 10) / nDistance;
					
					if (abs(nSin) > 1024) //取绝对值 
						return FALSE;
					
					if (abs(nCos) > 1024) 
						return FALSE;
					
					CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY,nMaxShangHai);
				}
				else
					CastLine(&SkillParam, nDir, nSrcPX,nSrcPY,nMaxShangHai);
			}break;
		case SKILL_SLT_Obj:
			{
			}break;
		case SKILL_SLT_Missle:
			{
				KMissle * pMissle = &Missle[nLauncher];
				if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
				SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
				SkillParam.nLauncher = pMissle->m_nLauncher;
				SkillParam.nParent = nLauncher;
				SkillParam.eParentType = eLauncherType;
				SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
				CastLine(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY,nMaxShangHai);
			}break;
		default:
			break;
		}

	}
	return TRUE;
}



BOOL KSkill::__CastSpread(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai) const
{
	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;



	if (nParam1 == SKILL_SPT_Direction)
	{
		switch(eLauncherType)
		{
		case SKILL_SLT_Npc:
			{
				SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
				if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
				nDir = nParam2;
				SkillParam.nLauncher = nLauncher;
				SkillParam.eLauncherType = eLauncherType;
				CastSpread(&SkillParam, nDir, nSrcPX,nSrcPY,nMaxShangHai);
			}break;
		case SKILL_SLT_Obj:
			{
				
			}break;
		case SKILL_SLT_Missle:
			{
				KMissle * pMissle = &Missle[nLauncher];
				if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
				if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
				nDir = nParam2;
				SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
				SkillParam.nLauncher = pMissle->m_nLauncher;
				SkillParam.nParent = nLauncher;
				SkillParam.eParentType = eLauncherType;
				SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
				CastSpread(&SkillParam, nDir,  nRefPX, nRefPY,nMaxShangHai);
			}break;
		default:
			break;
		}
	}
	else
	{
		switch(eLauncherType)
		{
		case SKILL_SLT_Npc:
			{
				nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY, SKILL_SLT_Npc);		
				SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
				nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
				nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
				SkillParam.nLauncher     = nLauncher;
				SkillParam.eLauncherType = eLauncherType;
				SkillParam.nTargetId     = nTargetId;
				
				if (m_nChildSkillNum == 1 && (g_MisslesLib[m_nChildSkillId].m_eMoveKind == MISSLE_MMK_Line) ) 
				{
					if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ;
					nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
					
					if (nDistance == 0 ) return FALSE;
					int		nYLength = nDesPY - nSrcPY;
					int		nXLength = nDesPX - nSrcPX;
					int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍
					int		nCos = (nXLength << 10) / nDistance;
					
					if (abs(nSin) > 1024) 
						return FALSE;
					
					if (abs(nCos) > 1024) 
						return FALSE;
					
					CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY,nMaxShangHai);
				}
				else
				{
					CastSpread(&SkillParam, nDir, nSrcPX, nSrcPY,nMaxShangHai);
				}
			}break;
		case SKILL_SLT_Obj:
			{
				
			}break;
		case SKILL_SLT_Missle:
			{
				KMissle * pMissle = &Missle[nLauncher];
				if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
				SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
				SkillParam.nLauncher = pMissle->m_nLauncher;
				SkillParam.nParent = nLauncher;
				SkillParam.eParentType = eLauncherType;
				SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
				CastSpread(&SkillParam ,pMissle->m_nDir,  nRefPX, nRefPY,nMaxShangHai);
			}break;
		default:
			break;
		}
	}

	return TRUE;
}

BOOL	KSkill::__CastCircle(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai) const
{

	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	if (nParam1 == SKILL_SPT_Direction) return FALSE;
	
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY, eLauncherType);
			SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
			nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
			nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
			SkillParam.nLauncher = nLauncher;
			SkillParam.eLauncherType = eLauncherType;
			SkillParam.nTargetId = nTargetId;
			
			if (m_nValue1 == 0)
				CastCircle(&SkillParam, nDir, nSrcPX, nSrcPY,nMaxShangHai);
			else
				CastCircle(&SkillParam, nDir, nDesPX, nDesPY,nMaxShangHai);
		}break;
	case SKILL_SLT_Obj:
		{
			
		}break;
	case SKILL_SLT_Missle:
		{
			KMissle * pMissle = &Missle[nLauncher];
			if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
			SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
			SkillParam.nLauncher = pMissle->m_nLauncher;
			SkillParam.nParent = nLauncher;
			SkillParam.eParentType = eLauncherType;
			SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
			CastCircle(&SkillParam, pMissle->m_nDir,  nRefPX, nRefPY,nMaxShangHai);
		}break;
	default:
		break;
	}
	
	return TRUE;
}

BOOL	KSkill::__CastZone(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai)	const
{
	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	if (nParam1 == SKILL_SPT_Direction) return FALSE;
	
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY);
			SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
			nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
			nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
			SkillParam.nLauncher = nLauncher;
			SkillParam.eLauncherType = eLauncherType;
			SkillParam.nTargetId = nTargetId;
			CastZone(&SkillParam, nDir, nSrcPX, nSrcPY,nMaxShangHai);
			//printf("[子弹技能D]Skill::Cast(tid:%d,X:%d,Y:%d), ok.... \n",nTargetId,nDesPX,nDesPY);
		}break;
	case SKILL_SLT_Obj:
		{
			
		}break;
	case SKILL_SLT_Missle:
		{
			KMissle * pMissle = &Missle[nLauncher];
			if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) 
				return FALSE;
			SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
			SkillParam.nLauncher   = pMissle->m_nLauncher;//NPC索引
			SkillParam.nParent     = nLauncher;	 //Missidx
			SkillParam.eParentType = eLauncherType;
			SkillParam.nTargetId   = pMissle->m_nFollowNpcIdx;
			CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY,nMaxShangHai);
		}break;
	default:
		break;
	} 
   return TRUE;
}


BOOL	KSkill::__CastFixed(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai) const
{
	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	if (nParam1 == SKILL_SPT_Direction) return FALSE;	
	
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			try
			{
				nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY);
				nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
				nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
				SkillParam.nLauncher = nLauncher;//SkillParam.nParent
				SkillParam.eLauncherType = eLauncherType;
				SkillParam.nTargetId = nTargetId;
				//if (nTargetId==nLauncher)
				//	SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nDesPX, &nDesPY);
				CastZone(&SkillParam, nDir, nDesPX, nDesPY,nMaxShangHai);
				//printf("[子弹技能B]Skill::Cast(tid:%d,nDirIndex:%d,nDir:%d), ok.... \n",nTargetId,nDirIndex,nDir);
			}
			catch (...)
			{
				/*if (Npc[nLauncher].IsPlayer())
				{
#ifndef  _SERVER
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("技能有误!");
#endif
				}*/
			}
		}break;
	case SKILL_SLT_Obj:
		{
			
		}break;
	case SKILL_SLT_Missle:
		{
			KMissle * pMissle = &Missle[nLauncher];
			if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
			SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
			SkillParam.nLauncher = pMissle->m_nLauncher;
			SkillParam.nParent = nLauncher;
			SkillParam.eParentType = eLauncherType;
			SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
			CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY,nMaxShangHai);
		}break;
	default:
		break;
	} 
   return TRUE;
}

BOOL	KSkill::__CastRound(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai) const
{

	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

    if (nParam1 == SKILL_SPT_Direction)
		return FALSE;
	
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			SubWorld[Npc[nLauncher].m_SubWorldIndex].NewMap2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
			nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
			nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
			SkillParam.nLauncher = nLauncher;
			SkillParam.eLauncherType = eLauncherType;
			SkillParam.nTargetId = nTargetId;
			CastZone(&SkillParam,  nDir, nSrcPX, nSrcPY,nMaxShangHai);
			//printf("[子弹技能C]Skill::Cast(tid:%d,X:%d,Y:%d), ok.... \n",nTargetId,nDesPX,nDesPY);
		}break;
	case SKILL_SLT_Obj:
		{
			
		}break;
	case SKILL_SLT_Missle:
		{
			KMissle * pMissle = &Missle[nLauncher];
			if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
			SubWorld[pMissle->m_nSubWorldId].NewMap2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
			SkillParam.nLauncher = pMissle->m_nLauncher;
			SkillParam.nParent = nLauncher;
			SkillParam.eParentType = eLauncherType;
			SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
			CastZone(&SkillParam , pMissle->m_nDir, nRefPX, nRefPY,nMaxShangHai);
		}break;
	default:
		break;
	}
   return TRUE;
}	
/*!*****************************************************************************
// Function		: KSkill::CastMissles
// Purpose		: 发送子弹技能
// Return		: 
// Argumant		: int nLauncher  发送者id
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nWaitTime  延长时间
// Argumant		: eSkillLauncherType eLauncherType 发送者类型
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::CastMissles(int nLauncher, int nParam1, int nParam2, int nWaitTime  , eSkillLauncherType eLauncherType,int nMaxShangHai )  const 
{
	if (nLauncher <= 0 || nLauncher>=MAX_NPC) return FALSE;

	switch(m_eMisslesForm)
	{
	/*
	火墙时，第一数字参数表示子弹之间的长度间隔
	X2  = X1 + N * SinA
	Y2  = Y2 - N * CosA
	*/	
	case SKILL_MF_Wall:			   //墙形	多个子弹呈垂直方向排列，类式火墙状
		{
		  __CastWall(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}break;	
	case SKILL_MF_Line:				//线形	多个子弹呈平行于玩家方向排列
		{
          __CastLine(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}
		break;
		
		//  数字参数一表示子弹之间的角度差，以64方向为准
		//  传来的X/Y参数为格子坐标		
	case	SKILL_MF_Spread:				//散形	多个子弹呈一定的角度的发散状	
		{//亢龙有悔子弹
			__CastSpread(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}break;	
		//以当前点为圆点产生多个围扰的子弹
		//分成两种情况，一种为以原地为原心发出，另一种为以目标点为原心发出
		// 数字参数一表示 是否为原地发出		
	case SKILL_MF_Circle:				//圆形	多个子弹围成一个圈
		{
		   __CastCircle(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}break;
		
	case	SKILL_MF_Random:				//随机	多个子弹随机排放
		{
		}
		break;		
	case	SKILL_MF_AtTarget:				//定点	多个子弹根据 烟花  玫瑰花
		{
			__CastFixed(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}break;	
	case	SKILL_MF_AtFirer:				//本身	多个子弹停在玩家当前位置
		{
			__CastRound(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);	
		}break;	
	case	SKILL_MF_Zone:
		{
			__CastZone(nLauncher, nParam1, nParam2, nWaitTime  , eLauncherType, nMaxShangHai);
		}
	break;
	default:
		break;
	}
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::CastZone
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
//nValue1 = 0 表示矩形区域  nValue1 = 1 表示圆形区域
//nValue2 = 0 
int KSkill::CastZone(TOrdinSkillParam * pSkillParam , int nDir, int nRefPX, int nRefPY,int nMaxShangHai)  const 
{
	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;
	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int nCastMissleNum	= 0;
	int nBeginPX ;
	int nBeginPY ;


	/*
	
	  // 尝试将子弹运行方向调整得更精确
	     INT nMpsX, nMpsY;
	     sp.GetDesMps(nMpsX, nMpsY);
	  
		 INT nDx = 0;
		 INT nDy = 0;
		
		  // 在同一个格子内，不需要精确
		  if ((nMpsX & ~31) == (sp.nSrcX & ~31) && (nMpsY & ~31) == (sp.nSrcY & ~31))
		  {
		    nDx = g_DirCos(SkillParam.nDir, MaxMissileDir);
		    nDy = g_DirSin(SkillParam.nDir, MaxMissileDir);	
		  }
		  else
		  {
		  // 瞄准目标格子中心而不是一定瞄准目标精确位置，这样更容易命中
		    nDx	= nMpsX - sp.nSrcX;
		    nDy	= nMpsY - sp.nSrcY;
		    FLOAT fLength	= sqrt((FLOAT)(nDx*nDx + nDy*nDy));
		    nDx		= (INT)((nDx << 10) / fLength + .5);
		    nDy		= (INT)((nDy << 10) / fLength + .5);
		  }
	
	*/

	if (m_nChildSkillNum == 1)
	{
		nBeginPX = nRefPX;
		nBeginPY = nRefPY;
	}
	else 
	{
		nBeginPX		= nRefPX - m_nChildSkillNum * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth / 2;
		nBeginPY		= nRefPY - m_nChildSkillNum * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight / 2;
	}
		
	for (int i = 0; i < m_nChildSkillNum; ++i)   //子技能的数量	或子弹的数量
		for (int j = 0; j < m_nChildSkillNum; ++j)
		{
			if (m_bBaseSkill)
			{ //是否基本技能
				int nMissleIndex ;
				int nSubWorldId ; 
				
				nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
				
				if (m_nValue1 == 1)	//表示圆形区域
					if ( ((i - m_nChildSkillNum / 2) * (i - m_nChildSkillNum / 2) + (j - m_nChildSkillNum / 2) * (j - m_nChildSkillNum / 2)) > (m_nChildSkillNum * m_nChildSkillNum / 4))
						continue;
					
					if (nSubWorldId < 0)
						goto exit;

					int nDesSubX = nBeginPX + j * SubWorld[nSubWorldId].m_nCellWidth;
					int nDesSubY = nBeginPY + i * SubWorld[nSubWorldId].m_nCellHeight;

					    nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX , nDesSubY);
					
					if (nMissleIndex < 0)        //子弹满了
						continue;
					
					Missle[nMissleIndex].m_nDir				= nDir;
					Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
					CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);  //子技能ID	创建子弹数据
//-------------------------------------//玫瑰花 客户端容易崩溃	
	
					Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId; //跟踪的目标索引
					Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
					Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
					Missle[nMissleIndex].m_nLauncher		= nLauncher;	 //发送者索引
					Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;

					if (pSkillParam->nParent)
						Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
					else 
					    Missle[nMissleIndex].m_nParentMissleIndex = 0;

					Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
					Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
					Missle[nMissleIndex].m_nSkillId			= m_nId;   // 技能ID
					
					Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j);
					Missle[nMissleIndex].m_nLifeTime		+=Missle[nMissleIndex].m_nStartLifeTime;
					Missle[nMissleIndex].m_nRefPX			= nDesSubX;
					Missle[nMissleIndex].m_nRefPY			= nDesSubY;
					
					Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic; 

//--------------------------------------					

					if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line|| Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack)
					{
				      
						Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
						Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
					}
					nCastMissleNum ++;
			}
			else
			{//不是基本技能的 直接使用子技能
				//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel !=0); //子技能等级
			   if  (m_nChildSkillId<=0) return nCastMissleNum;
	           int m_ChildSkillLevel=0;
			   if (m_nChildSkillLevel<=0)
				   m_ChildSkillLevel=m_ulLevel;
			   else
				   m_ChildSkillLevel=m_nChildSkillLevel;

				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
				if (pOrdinSkill) 
				{
					if (!pSkillParam->nParent)
						nCastMissleNum += pOrdinSkill->Cast(nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), eLauncherType, nMaxShangHai);
					else 
						nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j), pSkillParam->eLauncherType,nMaxShangHai);
				}
			}
			
		}
exit:	
//			printf("[释放子弹成功]子技能:NUM:%d,ID:%d,LV:%d,INdex:%d\n",nCastMissleNum,m_nChildSkillId,m_nChildSkillLevel,nLauncher);
			return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastLine
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou	   直线
*****************************************************************************/
// Value1 子弹之间的间距
// Value2 
int	KSkill::CastLine(TOrdinSkillParam *pSkillParam, int nDir, int nRefPX, int nRefPY,int nMaxShangHai)  const 
{
	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;

	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	//子弹之间的间距
	int nMSDistanceEach = m_nValue1;	
	int nNum = 0;
	//分别生成多少子弹
	for(int i = 0; i < m_nChildSkillNum; ++i)
	{
		if (m_nValue2)
		{
			int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;

			int nDir1 = nDirIndex + MaxMissleDir / 4;
			if (nDir1 > MaxMissleDir)
				nDir1 -= MaxMissleDir;

			int nDIndex = g_Dir2DirIndex(nDir1,MaxMissleDir);

			if (i % 2)
			{
				nCurMSDistance = -nCurMSDistance;
				nNum++;
			}
			//nRefPX人的x坐标
			if (nDIndex < 0)
				nDIndex = MaxMissleDir + nDIndex;
			
			if (nDIndex >= MaxMissleDir)
			    nDIndex -= MaxMissleDir;

			nDesSubX	= nRefPX + ((nCurMSDistance * nNum * g_DirCos(nDIndex, MaxMissleDir) )>>10);
			nDesSubY	= nRefPY + ((nCurMSDistance * nNum * g_DirSin(nDIndex, MaxMissleDir) )>>10);

			/*int nDirTemp = nDir - MaxMissleDir / 4;
			if (nDirTemp < 0) nDirTemp += MaxMissleDir;
			Missle[nMissleIndex].m_nDir				= nDirTemp;
			Missle[nMissleIndex].m_nDirIndex = g_Dir2DirIndex(nDirTemp, 64); */

		}
		else
		{
			if (nDirIndex < 0)
				nDirIndex = MaxMissleDir + nDirIndex;
			
			if (nDirIndex >= MaxMissleDir)
			    nDirIndex -= MaxMissleDir;

			nDesSubX	= nRefPX + ((nMSDistanceEach * (i + 1) * g_DirCos(nDirIndex, MaxMissleDir) )>>10);
			nDesSubY	= nRefPY + ((nMSDistanceEach * (i + 1) * g_DirSin(nDirIndex, MaxMissleDir) )>>10);
		}

		if (nDesSubX < 0 || nDesSubY < 0) 
			continue;
		
		if (m_bBaseSkill)
		{//基本技能
			int nMissleIndex ;
			int nSubWorldId ; 
			    nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			   nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);	 //增加索引
			
			if (nMissleIndex < 0)	continue;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;

			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;

			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack)
			{ //如果是 直线攻击 择时精确打击
				if (nDir < 0)
					nDir = MaxMissleDir + nDir;
				
				if (nDir >= MaxMissleDir)
					nDir -= MaxMissleDir;

				Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
			}
			nCastMissleNum ++;
		}
		else
		{//直接使用子技能
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel != 0);
			if  (m_nChildSkillId<=0) return nCastMissleNum;

			int m_ChildSkillLevel=0;

			if (m_nChildSkillLevel<=0)
				m_ChildSkillLevel=m_ulLevel;
			else
				m_ChildSkillLevel=m_nChildSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType,nMaxShangHai);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType,nMaxShangHai);
				
			}
		}
		
	}
	
exit:	
		return nCastMissleNum;
}

int		KSkill::CastExtractiveLineMissle(TOrdinSkillParam* pSkillParam,  int nDir,int nSrcX, int nSrcY, int nXOffset, int nYOffset, int nDesX, int nDesY,int nMaxShangHai)  const 
{
	
	//_ASSERT(pSkillParam);
	if  (!pSkillParam) return 0;
	
	int nLauncher = pSkillParam->nLauncher;
	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	if (pSkillParam->eLauncherType != SKILL_SLT_Npc) return 0;	
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
		//分别生成多少子弹
	{
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nSrcX, nSrcY);
			
			if (nMissleIndex < 0)	goto exit;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Parabola)
			{
				int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);

				if  (Missle[nMissleIndex].m_nSpeed<=0)
					 Missle[nMissleIndex].m_nSpeed = 4;

				int nTime    = nLength / Missle[nMissleIndex].m_nSpeed;

				Missle[nMissleIndex].m_nHeightSpeed	= Missle[nMissleIndex].m_nZAcceleration * (nTime - 1) / 2;
				
			}
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;  //目标的索引
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
		
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(0);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nSrcX;
			Missle[nMissleIndex].m_nRefPY			= nSrcY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;

			int nTempR = 0;
			int nTempMapX = 0;
			int nTempMapY = 0;
			int nTempOffsetX = 0;
			int nTempOffsetY = 0;

			Missle[nMissleIndex].m_bNeedReclaim = TRUE;
			int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);

			Missle[nMissleIndex].m_nFirstReclaimTime = nLength / Missle[nMissleIndex].m_nSpeed + Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nEndReclaimTime = Missle[nMissleIndex].m_nFirstReclaimTime + SubWorld[nSubWorldId].m_nCellWidth / Missle[nMissleIndex].m_nSpeed + 2;

			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Parabola)
			{
				
				Missle[nMissleIndex].m_nXFactor = nXOffset;
				Missle[nMissleIndex].m_nYFactor = nYOffset;
			}
			nCastMissleNum ++;
		}
		else
		{
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel !=0); //子技能等级
			if  (m_nChildSkillId<=0) return nCastMissleNum;

	           int m_ChildSkillLevel=0;
			   if (m_nChildSkillLevel<=0)
				   m_ChildSkillLevel=m_ulLevel;
			   else
				   m_ChildSkillLevel=m_nChildSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eLauncherType,nMaxShangHai);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eParentType,nMaxShangHai);
				
			}
		}
	}
	
exit:	
		return nCastMissleNum;
		
}

/*!*****************************************************************************
// Function		: KSkill::CastWall
// Purpose		: Wall Magic 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
m_nValue1 表示子弹之间的距离，单位像素点
*/
int KSkill::CastWall(TOrdinSkillParam * pSkillParam,  int nDir , int nRefPX , int nRefPY,int nMaxShangHai)  const 
{

	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;

	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;

	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	
	//子弹之间的间距
	int nMSDistanceEach = m_nValue1;
	int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;

	//分别生成多少子弹
	if (nDirIndex < 0)
		nDirIndex = MaxMissleDir + nDirIndex;
				
	if (nDirIndex >= MaxMissleDir)
		nDirIndex -= MaxMissleDir;

	for(int i = 0; i < m_nChildSkillNum; ++i)
	{
		nDesSubX	= nRefPX + ((nCurMSDistance * g_DirCos(nDirIndex, MaxMissleDir)) >>10);
		nDesSubY	= nRefPY + ((nCurMSDistance * g_DirSin(nDirIndex, MaxMissleDir)) >>10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{//如果是基本技能
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	
			{
				goto exit;
			}
			
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			if (nMissleIndex < 0)	
			{
				continue;
			}

			if (m_nValue2)
			{
				int nDirTemp = nDir - MaxMissleDir / 4;
				if (nDirTemp < 0) nDirTemp += MaxMissleDir;
				Missle[nMissleIndex].m_nDir				= nDirTemp;
				Missle[nMissleIndex].m_nDirIndex = g_Dir2DirIndex(nDirTemp, MaxMissleDir);

			}
			else
			{
				Missle[nMissleIndex].m_nDir				= nDir;
				Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			}
			
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			
			/*if (pSkillParam->nTargetId>0 && pSkillParam->nTargetId<MAX_NPC)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			else
				Missle[nMissleIndex].m_dwFollowNpcID	= 0;*/
			
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;   //子弹技能ID
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line|| Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack)
			{
				if (Missle[nMissleIndex].m_nDir < 0)
					Missle[nMissleIndex].m_nDir = MaxMissleDir + Missle[nMissleIndex].m_nDir;
				
				if (Missle[nMissleIndex].m_nDir >= MaxMissleDir)
		            Missle[nMissleIndex].m_nDir -= MaxMissleDir;

				Missle[nMissleIndex].m_nXFactor = g_DirCos(Missle[nMissleIndex].m_nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(Missle[nMissleIndex].m_nDir, MaxMissleDir);
			}
						
			nCastMissleNum ++;
		}
		else
		{//否则直接使用子技能（不是子弹技能）
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel!=0);
			if  (m_nChildSkillId<=0) return nCastMissleNum;
			int m_ChildSkillLevel=0;
			if (m_nChildSkillLevel<=0)
				m_ChildSkillLevel=m_ulLevel;
			else
				m_ChildSkillLevel=m_nChildSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType, nMaxShangHai);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime +  GetMissleGenerateTime(i), pSkillParam->eParentType, nMaxShangHai);
			}
		}
		
		nCurMSDistance += nMSDistanceEach;
	}
	
exit:	
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastNotWall
// Purpose		: Wall Magic 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
m_nValue1 表示子弹之间的距离，单位像素点
*/
int KSkill::CastNotWall(TOrdinSkillParam * pSkillParam,  int nDir , int nRefPX , int nRefPY,int nMaxShangHai)  const 
{
	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;
	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
		
	//子弹之间的间距
	int nMSDistanceEach = m_nValue1;
	int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;	
	//分别生成多少子弹
	for(int i = 0; i < m_nChildSkillNum; ++i)
	{
		int nDir1 = nDirIndex + MaxMissleDir / 4;

		if (nDir1 < 0)
			nDir1 = MaxMissleDir +nDir1;
		
		if (nDir1 >= MaxMissleDir)
		    nDir1 -= MaxMissleDir;

		nDesSubX	= nRefPX + ((nCurMSDistance * g_DirCos(nDir1, MaxMissleDir)) >>10);
		nDesSubY	= nRefPY + ((nCurMSDistance * g_DirSin(nDir1, MaxMissleDir)) >>10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex;
			int nSubWorldId; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			   nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	
				continue;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack)
			{
				if (nDir < 0)
					nDir = MaxMissleDir +nDir;
				
				if (nDir >= MaxMissleDir)
		            nDir -= MaxMissleDir;

				Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
			}
			
			nCastMissleNum ++;
		}
		else
		{
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel !=0);
			if  (m_nChildSkillId<=0) return nCastMissleNum;

			int m_ChildSkillLevel=0;
			if (m_nChildSkillLevel<=0)
				m_ChildSkillLevel=m_ulLevel;
			else
				m_ChildSkillLevel=m_nChildSkillLevel;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType, nMaxShangHai);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType, nMaxShangHai);
				
			}
		}
		
	}
	
exit:	
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastCircle
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType  eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Value1  == 0 表示发送者为圆心产生圆，否则以目标点为圆心产生圆
int		KSkill::CastCircle(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY,int nMaxShangHai)  const 
{
	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;
	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	eSkillLauncherType  eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;	
	int nDesSubPX	= 0;
	int nDesSubPY	= 0;
	int nFirstStep	= m_nValue2;			//第一步的长度，子弹在刚发出去时离玩家的距离
	int nCurSubDir	= 0;
	int nDirPerNum  = 0;

	if  (m_nChildSkillNum>0)
	     nDirPerNum = MaxMissleDir / m_nChildSkillNum ;  //有可能出现BUG

	int nCastMissleNum = 0;
	
	//分别生成多个子弹
	for(int i = 0; i < m_nChildSkillNum; ++i)
	{
		int nCurSubDir	= nDir + nDirPerNum * i ;
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	= g_DirSin(nCurSubDir, MaxMissleDir);
		int nCosAB	= g_DirCos(nCurSubDir, MaxMissleDir);
		
		nDesSubPX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubPY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
	
		if (nDesSubPX < 0 || nDesSubPY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubPX, nDesSubPY);
			
			if (nMissleIndex < 0)	
			{
				continue;
			}
			
			Missle[nMissleIndex].m_nDir			= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex	= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubPX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubPY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;
			
			if (Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_Line || Missle[nMissleIndex].m_eMoveKind == MISSLE_MMK_RollBack)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(nCurSubDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nCurSubDir, MaxMissleDir);
			}
			nCastMissleNum ++;
			
		}
		else
		{
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel != 0);
			if  (m_nChildSkillId<=0) return nCastMissleNum;

			int m_ChildSkillLevel=0;

			if (m_nChildSkillLevel<=0)
				m_ChildSkillLevel=m_ulLevel;
			else
				m_ChildSkillLevel=m_nChildSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType, nMaxShangHai);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType,nMaxShangHai);
			}
		}
		
	}
	
exit:	
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastSpread
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
Value1 每个子弹相差的角度单位
Value2 每一步的长度，第一步的长度，子弹在刚发出去时离玩家的距离
*/
int		KSkill::CastSpread(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY,int nMaxShangHai)  const 
{
	if  (!pSkillParam) return 0;

	int nLauncher = pSkillParam->nLauncher;
	if  (nLauncher <= 0 || nLauncher>=MAX_NPC) return 0;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int nDesSubMapX		= 0;
	int nDesSubMapY		= 0;
	int nFirstStep		= m_nValue2;			//第一步的长度，子弹在刚发出去时离玩家的距离
	int nCurMSRadius	= m_nChildSkillNum / 2 ; 
	int nCurSubDir		= 0;
	int	nCastMissleNum  = 0;			//实际发送的Missle的数量
	
	// Sin A+B = SinA*CosB + CosA*SinB
	// Cos A+B = CosA*CosB - SinA*SinB
	// Sin A = nYFactor
	// Cos A = nXFactor
	int nDesSubX = 0;
	int nDesSubY = 0;
	int nXFactor = 0;
	int nYFactor = 0;
	
	if (pSkillParam->nTargetId > 0)
	{
		int nTargetId = pSkillParam->nTargetId;
		int nDistance = 0;
		int nDesX, nDesY;
		if (Npc[nTargetId].m_Index > 0 && Npc[nTargetId].m_SubWorldIndex >= 0) 
			SubWorld[Npc[nTargetId].m_SubWorldIndex].NewMap2Mps(Npc[nTargetId].m_RegionIndex, Npc[nTargetId].m_MapX, Npc[nTargetId].m_MapY, Npc[nTargetId].m_OffX, Npc[nTargetId].m_OffY, &nDesX, &nDesY);
		
		nDistance = (int)sqrt(double((nDesX - nRefPX))*(nDesX - nRefPX) +	(nDesY - nRefPY)*(nDesY - nRefPY));
		
		if (nDistance == 0) nDistance = 1;

		nXFactor = ((nDesX - nRefPX)<<10) / nDistance;
		nYFactor = ((nDesY - nRefPY)<<10) / nDistance;
		
		nDesSubX = nRefPX + ((nXFactor * nFirstStep)>>10);
		nDesSubY = nRefPY + ((nYFactor * nFirstStep)>>10);
		
		if (nDesSubX < 0  || nDesSubY < 0 ) return 0;
	}
	
	int nTargetId = pSkillParam->nTargetId;

	//分别生成多个子弹
	for(int i = 0; i < m_nChildSkillNum; ++i)
	{
		int nDSubDir	= m_nValue1 * nCurMSRadius;   //子弹间的相隔距离
		   nCurSubDir	= nDir - m_nValue1 * nCurMSRadius;
		
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	;
		int nCosAB	;
		
		if (nTargetId > 0)
		{
			nDSubDir	+= 48;

			if (nDSubDir < 0)
				nDSubDir = MaxMissleDir +nDSubDir;
			
			if (nDSubDir >= MaxMissleDir)
				nDSubDir = nDSubDir%MaxMissleDir;
			//sin(a - b) = sinacosb - cosa*sinb
			//cos(a - b) = cosacoab + sinasinb
			nSinAB = (nYFactor * g_DirCos(nDSubDir, MaxMissleDir) - nXFactor * g_DirSin(nDSubDir, MaxMissleDir)) >> 10;
			nCosAB = (nXFactor * g_DirCos(nDSubDir, MaxMissleDir) + nYFactor * g_DirSin(nDSubDir , MaxMissleDir)) >> 10;
		}
		else
		{
			nSinAB = g_DirSin(nCurSubDir, MaxMissleDir);
			nCosAB = g_DirCos(nCurSubDir, MaxMissleDir);
		}
		
		nDesSubX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			//产生索引
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	
				continue;
			
			Missle[nMissleIndex].m_nDir				= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);

			/*if (nLauncher>0 && nLauncher<MAX_NPC)
			{
				char msg[128]={0};
				t_sprintf(msg,"发技能%d:%s,%s,MisslesForm:%d",i,Npc[nLauncher].Name,m_szName,m_eMisslesForm);
				CCMessageBox(msg,"CastSpread");	
			}*/

			Missle[nMissleIndex].m_nFollowNpcIdx	= nTargetId;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			Missle[nMissleIndex].m_nShangBei        = nMaxShangHai;
			Missle[nMissleIndex].m_nEnChance		= m_nEnChance;
			Missle[nMissleIndex].m_nSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nXFactor			= nCosAB;
			Missle[nMissleIndex].m_nYFactor			= nSinAB;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			Missle[nMissleIndex].m_nIsMagic         = m_nIsMagic;
			nCastMissleNum ++;
		}
		else
		{
			//_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel !=0);
			if  (m_nChildSkillId<=0) return nCastMissleNum;

			int m_ChildSkillLevel=0;
			if (m_nChildSkillLevel<=0)
				m_ChildSkillLevel=m_ulLevel;
			else
				m_ChildSkillLevel=m_nChildSkillLevel;

			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_ChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum +=  pOrdinSkill->Cast(nLauncher,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType,nMaxShangHai);
				else
					nCastMissleNum +=  pOrdinSkill->Cast(pSkillParam->nParent,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType, nMaxShangHai); 
			}
		}
		
		nCurMSRadius -- ;
	}
exit:	
		return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::GetChildSkillNum
// Purpose		: 由于可能某些技能中，随着技能等级的升级，子弹的数目也会因此增加，所以通过该函数获得实际的子技能数目
// Return		: 
// Argumant		: int nLevel
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int 	KSkill::GetChildSkillNum(int nLevel)  const 
{
	return m_nChildSkillNum;
};
/*!*****************************************************************************
// Function		: KSkill::CreateMissle
// Purpose		: 设置子弹的基本数据，以及该技能该等级下的对子弹信息的变动数据
//					设置用于数值计算的指针
// Return		: 
// Argumant		: int nChildSkillId
// Argumant		: int nMissleIndex
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void	KSkill::CreateMissle(int nLauncher, int nChildSkillId, int nMissleIndex)  const 
{
	//_ASSERT(nChildSkillId > 0 && nChildSkillId < MAX_MISSLESTYLE && nMissleIndex > 0);
	if (nChildSkillId <= 0 || nChildSkillId >= MAX_MISSLESTYLE || nMissleIndex <= 0)
		return;

	if (nLauncher <= 0) 
	{
		return ;
	}
/*	
#ifdef _SERVER
	if (Npc[nLauncher].IsPlayer())
	   printf("--创建子弹S:人物:%d,子技能:%d,子弹索引:%d--\n",nLauncher,nChildSkillId,nMissleIndex);
#else
	if (Npc[nLauncher].IsPlayer())
	{
		char nMsg[64]={0};
		t_sprintf(nMsg,"--创建子弹C:人物:%d,子技能:%d,子弹索引:%d--",nLauncher,nChildSkillId,nMissleIndex);
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);
	}
#endif*/

	KMissle * pMissle = &Missle[nMissleIndex];
	
	g_MisslesLib[nChildSkillId] = *pMissle;//复制拷贝对象到子技能里
	
	pMissle->m_nLevel			= m_ulLevel;        //当前的等级
	pMissle->m_bCollideEvent	= m_bCollideEvent;	//碰撞时
	pMissle->m_bVanishedEvent   = m_bVanishedEvent;	//结束时
	pMissle->m_bStartEvent		= m_bStartEvent;    //开始时
	pMissle->m_bFlyEvent		= m_bFlyingEvent;   //飞行时
	pMissle->m_nFlyEventTime	= m_nFlyEventTime;
	pMissle->m_nMissleId		= nMissleIndex;
	pMissle->m_bClientSend      = m_bClientSend;    //客户端技能无伤害
	pMissle->m_bMustBeHit		= m_bMustBeHit;     //是否必中技能
	pMissle->m_bIsMelee			= m_bIsMelee;       //是否近身技能
	pMissle->m_bByMissle		= m_bByMissle;      //是否由子弹产生子弹 否则 有NPC产生子弹
	pMissle->m_bTargetSelf		= (m_bTargetSelf == 1);
	pMissle->m_nInteruptTypeWhenMove = m_nInteruptTypeWhenMove;	//移动则消亡子弹
	pMissle->m_bHeelAtParent	= m_bHeelAtParent;  //是否需要更正位置
	pMissle->m_bUseAttackRating	= m_bUseAttackRate; //是否使用命中率
	pMissle->m_bDoHurt			= m_bDoHurt;        //是否做受伤动作
	//pMissle->m_nIsMagic	        = m_nIsMagic;
	
	if (pMissle->m_nInteruptTypeWhenMove)
	{
		int m_nMsp=0;
		Npc[nLauncher].GetMpsPos(&pMissle->m_nLauncherSrcPX, &pMissle->m_nLauncherSrcPY,&m_nMsp);
	}

	pMissle->m_eRelation = m_eRelation;
	pMissle->m_MissleRes.m_bNeedShadow   = m_bNeedShadow;
	pMissle->m_MissleRes.m_nMaxShadowNum = m_nMaxShadowNum;
	pMissle->m_MissleRes.m_nMissleId	 = nMissleIndex;
	pMissle->m_MissleRes.Init(); //客户端删除子弹外观
		/*
	if (!pMissle->m_MissleRes.Init()) 
		g_DebugLog("创建子弹贴图失败！！！%s", __FILE__) ;
		*/
	pMissle->DoWait();
	
	for (int i = 0  ; i < m_nMissleAttribsNum; ++i)
	{
		switch (m_MissleAttribs[i].nAttribType)
		{
		case magic_missle_movekind_v:
			{
				pMissle->m_eMoveKind	= (eMissleMoveKind) m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_speed_v:	
			{
				pMissle->m_nSpeed		= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_lifetime_v:
			{
				pMissle->m_nLifeTime	= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_height_v:	
			{
				pMissle->m_nHeight		= m_MissleAttribs[i].nValue[0];
			}break;
			
		case magic_missle_damagerange_v:
			{
				pMissle->m_nDamageRange = m_MissleAttribs[i].nValue[0];	  //伤害范围
			}break;	
		case magic_missle_radius_v:	
			{
			}break;
	    case magic_missle_missrate:	 // 子弹击中的概率
			{
			   pMissle->m_nRate = m_MissleAttribs[i].nValue[0];
			}break;
	    case magic_missle_hitcount:	 // 子弹击中的人数
			{
			   pMissle->m_nHitCount = m_MissleAttribs[i].nValue[0];
			}break;	
		case magic_missile_drag:	 // 是否有拉扯
		{
		}break;

		}
	}
	
	if (m_bIsMelee)	  //近程反弹  改变生存时间
		pMissle->m_nLifeTime = Npc[nLauncher].ModifyMissleLifeTime(pMissle->m_nLifeTime);
	else
	{  //子弹减速
		pMissle->m_nSpeed = Npc[nLauncher].ModifyMissleSpeed(pMissle->m_nSpeed);
		pMissle->m_bCollideVanish = Npc[nLauncher].ModifyMissleCollsion(pMissle->m_bCollideVanish);
	}
	
}

/*!*****************************************************************************
// Function		: KSkill::GetInfoFromTabFile
// Purpose		: 从TabFile中获得该技能的常规属性
// Return		: 
// Argumant		: int nCol
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::GetInfoFromTabFile(int nRow)
{
	KITabFile * pITabFile = &g_OrdinSkillsSetting;
	return GetInfoFromTabFile(&g_OrdinSkillsSetting, nRow);
}


BOOL	KSkill::GetInfoFromTabFile(KITabFile *pSkillsSettingFile, int nRow)
{
	if (!pSkillsSettingFile || nRow < 0) return FALSE;
	//	
	int nCurVal=0;
	pSkillsSettingFile->GetString(nRow, "SkillName",		"", m_szName, sizeof(m_szName) ,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillId",			0, (int *)&m_nId,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Attrib",			0, (int *)&m_nAttrib,TRUE); //武功流派
	int nReqLevel = 0;
	pSkillsSettingFile->GetInteger(nRow, "ReqLevel",		0, (int *)&nReqLevel, TRUE);  //需要的等级才能升级技能
	m_usReqLevel = (unsigned short)nReqLevel;

	pSkillsSettingFile->GetInteger(nRow, "EqtLimit",		-2, (int *)&m_nEquiptLimited, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "HorseLimit",		0, (int *)&m_nHorseLimited, TRUE);

	pSkillsSettingFile->GetInteger(nRow, "DoHurt",			0, &m_bDoHurt);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillNum",	0, &m_nChildSkillNum,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MisslesForm",		0, (int *)&m_eMisslesForm, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CharClass",		0, &m_nCharClass, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillStyle",		0, (int *)&m_eSkillStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CharAnimId",		0, (int *)&m_nCharActionId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsAura",			0, &nCurVal, TRUE);       //是否光环技能
	m_bIsAura = (nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "IsUseAR",			0, &nCurVal, TRUE);//是否使用命中率
	m_bUseAttackRate = (nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetOnly",		0, &nCurVal, TRUE);
	m_bTargetOnly = (nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetEnemy",		0, &nCurVal, TRUE);
	m_bTargetEnemy =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetAlly",		0, &nCurVal, TRUE);
	m_bTargetAlly  = (nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetObj",		0, &nCurVal, TRUE);
	m_bTargetObj     =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetOther",		0, &nCurVal, TRUE);
	m_bTargetOther   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetNoNpc",	    0, &nCurVal, TRUE);
	m_bTargetNoNpc   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "BaseSkill",		0, &nCurVal, TRUE);  //是否直接使用子技能
	m_bBaseSkill     =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "ByMissle",		0, &nCurVal, TRUE);   //是否由子弹产生子弹 
	m_bByMissle      =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "MustBeHit",		0, &nCurVal, TRUE);  //是否必中技能
	m_bMustBeHit     =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillId",	0, &m_nChildSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlyEvent",		0, &nCurVal, TRUE);
	m_bFlyingEvent   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "StartEvent",		0, &nCurVal, TRUE);
	m_bStartEvent    =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "CollideEvent",	0, &nCurVal, TRUE);
	m_bCollideEvent  =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "VanishedEvent",	0, &nCurVal, TRUE);
	m_bVanishedEvent =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "FlySkillId",		0, &m_nFlySkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StartSkillId",	0, &m_nStartSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "VanishedSkillId",	0, &m_nVanishedSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CollidSkillId",	0, &m_nCollideSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillCostType",	0, (int *)&m_nSkillCostType, TRUE);  //消耗的类型
	pSkillsSettingFile->GetInteger(nRow, "CostValue",		0, &m_nCost, TRUE);                  //值
	pSkillsSettingFile->GetInteger(nRow, "TimePerCast",		0, &m_nMinTimePerCast, TRUE);               //技能释放时间间隔
	pSkillsSettingFile->GetInteger(nRow, "TimePerCastOnHorse",0, &m_nMinTimePerCastOnHorse, TRUE);//骑马技能释放时间间隔
	pSkillsSettingFile->GetInteger(nRow, "Param1",			0, &m_nValue1, TRUE); //子弹之间的距离
	pSkillsSettingFile->GetInteger(nRow, "Param2",			0, &m_nValue2, TRUE); //距离NPC多远处开始产生子弹
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillLevel", 0, &m_nChildSkillLevel, TRUE);   // 子技能等级
	pSkillsSettingFile->GetInteger(nRow, "EventSkillLevel", 0, &m_nEventSkillLevel, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsMelee",			0, &nCurVal, TRUE);//是否近程反弹伤害
	m_bIsMelee   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "FlyEventTime",	0, &m_nFlyEventTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ShowEvent",	    0, &m_nShowEvent, TRUE);         //是否显示子弹事件
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerate",	0, (int *)&m_eMisslesGenerateStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerateData",0, &m_nMisslesGenerateData, TRUE); //时间
	pSkillsSettingFile->GetInteger(nRow, "MaxShadowNum",	0, &m_nMaxShadowNum, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "AttackRadius",	50, &m_nAttackRadius, TRUE);//攻击范围
	m_nBackAttackRadius = m_nAttackRadius;
	pSkillsSettingFile->GetInteger(nRow, "WaitTime",		0, &m_nWaitTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ClientSend",		0, &nCurVal, TRUE);  //是否客户端发送过来的技能
	m_bClientSend   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "TargetSelf",		0, &nCurVal, TRUE);
	m_bTargetSelf   =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "StopWhenMove",	0, &m_nInteruptTypeWhenMove, TRUE); //移动终止子弹
	pSkillsSettingFile->GetInteger(nRow, "HeelAtParent",    0,&nCurVal, TRUE);	//是否需要更正位置
	m_bHeelAtParent =(nCurVal>0)?true:false;
	pSkillsSettingFile->GetInteger(nRow, "IsExpSkill",      0,&m_nIsExpSkill, TRUE );          //是否是熟练度技能
	pSkillsSettingFile->GetInteger(nRow, "Series",		-1,&m_nSeries, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ShowAddition",	0,&m_nShowAddition, TRUE);
	//m_nSkillTime
	//pSkillsSettingFile->GetInteger(nRow, "IsMagic",		    0, &m_nIsMagic, TRUE);              //内外攻
	pSkillsSettingFile->GetInteger(nRow, "IsPhysical",		0,&nCurVal, TRUE);   //是否物理攻击
	m_bIsPhysical=(nCurVal>0)?true:false;

	if (m_bIsPhysical==1)       //外功
		m_nIsMagic=0;
	else if (m_bIsPhysical==0)	//内功
		m_nIsMagic=1;

	//服务器端需要获得并通知客户端
	pSkillsSettingFile->GetInteger(nRow, "StateSpecialId",  0, &m_nStateSpecialId, TRUE);       // 状态id
    //pSkillsSettingFile->GetInteger(nRow, "IsSkillPhysical",    0, &m_bIsSkillPhysical, TRUE);
    //pSkillsSettingFile->GetInteger(nRow, "IsSkillMagic",    0, &m_bIsSkillMagic, TRUE);         //是技能魔法

	m_eRelation = 0;

	if (m_bTargetEnemy)
		m_eRelation |= relation_enemy;
	
	if (m_bTargetAlly)
		m_eRelation |= relation_ally;
	
	if (m_bTargetSelf)
		m_eRelation |= relation_self;

	if (m_bTargetOther) //其他的
	{
		m_eRelation |= relation_dialog;
        m_eRelation |= relation_none;
	}
	pSkillsSettingFile->GetString(nRow, "SkillDesc", "<暂无数据>",m_szSkillDesc,sizeof(m_szSkillDesc)); //技能说明
	pSkillsSettingFile->GetInteger(nRow, "NeedShadow",0, (int *)&nCurVal, TRUE);
	m_bNeedShadow = (nCurVal>0)?true:false;
	pSkillsSettingFile->GetString(nRow, "SkillIcon","\\spr\\skill\\图标\\通用.spr",	m_szSkillIcon, sizeof(m_szSkillIcon));
	if (!m_szSkillIcon[0])	strcpy(m_szSkillIcon, "\\spr\\skill\\图标\\通用.spr");
	pSkillsSettingFile->GetInteger(nRow, "LRSkill",		0, (int*)&m_eLRSkillInfo);
	pSkillsSettingFile->GetString(nRow, "PreCastSpr", "", m_szPreCastEffectFile, sizeof(m_szPreCastEffectFile));
	pSkillsSettingFile->GetString(nRow, "ManCastSnd","", m_szManPreCastSoundFile, sizeof(m_szManPreCastSoundFile));
	pSkillsSettingFile->GetString(nRow, "FMCastSnd", "", m_szFMPreCastSoundFile, sizeof(m_szFMPreCastSoundFile));
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::LoadSkillLevelData
// Purpose		: 读表获得当前等级下当前技能的技能、子弹、碰撞数值影响
// Return		: 
// Argumant		: int nLevel
// Comments		:
// Author		: Romandou
****************************************************************************/
void KSkill::LoadSkillLevelData(unsigned long  nLevel /* =0*/, int nParam)
{
	m_nMissleAttribsNum = 0;         //
	m_nDamageAttribsNum = 0;        //伤害属性
	m_nImmediateAttribsNum = 0;     //立即状态属性数量
	m_nStateAttribsNum	= 0;		//状态属性个数，最大10
	
	char szSettingScriptName[MAX_PATH]={0};
	char szSettingNameValue[64]={0};
	char szSettingDataValue[64]={0};
	char szResult[512]={0};
	ZeroMemory(szSettingScriptName,sizeof(szSettingScriptName));
	ZeroMemory(szSettingNameValue,sizeof(szSettingNameValue));
	ZeroMemory(szSettingDataValue,sizeof(szSettingDataValue));
	ZeroMemory(szResult,sizeof(szResult));
	int nRowId = nParam;

	if (nRowId < 2) 
		return ;

	KLuaScript * pScript = NULL;

	g_OrdinSkillsSetting.GetString(nRowId,  "LvlSetScript", "", szSettingScriptName, sizeof(szSettingScriptName));
	if (!szSettingScriptName[0])
		return;
	g_SetFilePath("\\");
	//加载Lua脚本
	KLuaScript Script;
	Script.Init();
	if (!Script.Load(szSettingScriptName))  //加载脚本文件
	{
		//printf("无法读取技能设定脚本文件[%s],请确认是否文件存在或脚本语法有误!\n", szSettingScriptName);
		Script.Exit();
		return;
	}
	pScript  = &Script;	
//	pScript = (KLuaScript*)g_GetScript(m_dwSkillLevelDataScriptId);
	int nSafeIndex = 0;
	nSafeIndex=pScript->SafeCallBegin();
//		char nSkillNun[64];
	for(int i = 0 ;  i  < MAX_SKILLVEDATA_COUNT ; ++i)  //一个技能的属性数量级限制
	{
		char szSettingName[64]={0};
		char szSettingData[64]={0};
		t_sprintf(szSettingName, "LvlSetting%d", i + 1);  //技能属性名
		t_sprintf(szSettingData, "LvlData%d", i + 1);     //技能名称
		
		g_OrdinSkillsSetting.GetString(nRowId, szSettingName, "", szSettingNameValue, sizeof(szSettingNameValue));
		g_OrdinSkillsSetting.GetString(nRowId, szSettingData, "", szSettingDataValue, sizeof(szSettingDataValue));
		
		if (szSettingNameValue[0] == 0 /* 	|| szSettingDataValue[0] == '0'	*/)
		{
			continue;
		}

//执行这个脚本的函数		
		pScript->CallFunction("GetSkillLevelData", 1, "ssd", szSettingNameValue, szSettingDataValue, nLevel);
		const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
		if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			t_sprintf(szResult, "%d", nResult);
		}
		else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			//strcpy(szResult , (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
			t_sprintf(szResult , (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
		}
		else
		{
			break;
		}
/*
#ifdef _SERVER
		printf("--当前技能(%s)ID(%d)等级为[%d]([%s],[%s])时,返回值(%s)!--\n",m_szName,m_nId,nLevel, szSettingNameValue, szSettingDataValue,szResult);

#endif
 */
	   if (szResult[0])	
		  ParseString2MagicAttrib(nLevel,szSettingNameValue,szResult); //szResult为 脚本返回 技能等级 属性名 和 返回值？
	}
	nSafeIndex=pScript->SafeCallBegin();
	pScript->SafeCallEnd(nSafeIndex);

}
/*!*****************************************************************************
// Function		: KSkill::SetMissleGenerateTime
// Purpose		: 获得当前的子弹的实际产生时间
// Return		: void 
// Argumant		: Missle * pMissle
// Argumant		: int nNo
// Comments		:
// Author		: RomanDou
*****************************************************************************/
unsigned int KSkill::GetMissleGenerateTime(int nNo) const 
{
	
	switch(m_eMisslesGenerateStyle)
	{
	case SKILL_MGS_NULL:
		{
			return m_nWaitTime;
		}break;
		
	case SKILL_MGS_SAMETIME:
		{
			return  m_nWaitTime + m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_ORDER:		
		{
			return  m_nWaitTime + nNo * m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_RANDONORDER:	
		{
			if (g_Random(2) == 1) 
				return m_nWaitTime + nNo * m_nMisslesGenerateData + g_Random(m_nMisslesGenerateData);
			else 
				return m_nWaitTime + nNo * m_nMisslesGenerateData - g_Random(m_nMisslesGenerateData/2);
		}break;
		
	case SKILL_MGS_RANDONSAME:	
		{
			return  m_nWaitTime + g_Random(m_nMisslesGenerateData);
		}break;
		
	case SKILL_MGS_CENTEREXTENDLINE:
		{
			if (m_nChildSkillNum <= 1) return m_nWaitTime;
			int nCenter = m_nChildSkillNum / 2	;
			return m_nWaitTime + abs(nNo - nCenter) * m_nMisslesGenerateData ;
		}
	}
	return m_nWaitTime;
}

int KSkill::GetSkillIdFromName(char * szSkillName)  
{
	//	
	if (!szSkillName || !szSkillName[0]) 
        return -1;
	
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(i, 1);
		if (pOrdinSkill) 
		{
			if (!strcmp(pOrdinSkill->m_szName, szSkillName))
            {
                return i;
            }
		}
	}
	return -1;
	
}


/*!*****************************************************************************
// Function		: KSkill::CastInitiativeSkill
// Purpose		: 主动辅助技能
// Return		: BOOL 
// Argumant		: int nLauncher 为发技能者的索引
// Argumant		: int nParam1
// Argumant		: int nParam2  为对方的索引 技能的目标
// Argumant		: int nWaitTime
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KSkill::CastInitiativeSkill(int nLauncher, int nParam1, int nParam2, int nWaitTime,int nMaxShangHai)  const 
{
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::CastPassivitySkill
// Purpose		: 被动技能
// Return		: BOOL 
// Argumant		: int nLauncher 为发技能者的索引
// Argumant		: int nParam1
// Argumant		: int nParam2  为对方的索引 技能的目标
// Argumant		: int nWaitTime
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KSkill::CastPassivitySkill(int nLauncher, int nParam1, int nParam2, int nWaitTime,int nIsEuq)  const 
{
	return TRUE;
}
//加成技能状态
void KSkill::EnChanceSkill(int nLauncher)
{
	KMagicAttrib sMa[MAXSKILLLEVELSETTINGNUM];
	ZeroMemory(sMa,sizeof(sMa));
	int nCount = 0;
	for(int i = 0;i < m_nImmediateAttribsNum;++i)
	{
		if (m_ImmediateAttribs[i].nAttribType == magic_addskilldamage1 || 
			m_ImmediateAttribs[i].nAttribType == magic_addskilldamage2 ||  //原来只有两个
            m_ImmediateAttribs[i].nAttribType == magic_addskilldamage3 ||
			m_ImmediateAttribs[i].nAttribType == magic_addskilldamage4 ||
			m_ImmediateAttribs[i].nAttribType == magic_addskilldamage5 ||
			m_ImmediateAttribs[i].nAttribType == magic_addskilldamage6// ||
			//m_ImmediateAttribs[i].nAttribType == magic_skill_enhance   
			)
		{
			sMa[nCount] = m_ImmediateAttribs[i];
			nCount++;
		}
	}

	Npc[nLauncher].SetImmediatelySkillEffect(nLauncher,sMa,nCount);

}

/*!*****************************************************************************
// Function		: KSkill::ParseString2MagicAttrib
// Purpose		: 解析通过脚本运算获得的技能数据
// Return		: 
// Argumant		: char * szMagicAttribName
// Argumant		: char * szValue
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::ParseString2MagicAttrib(unsigned long ulLevel, char * szMagicAttribName, char * szValue)  
{
	int nValue1 = 0;
	int nValue2 = 0;
	int nValue3 = 0;
    const char *pcszTemp = NULL;
	if ((!szMagicAttribName) || (!szMagicAttribName[0])) 
		return FALSE;

	if  (szValue==NULL)
		return FALSE;

	///if (!strstr(szValue,","))
	//	return FALSE;
	//nValue2 当值为-1时为永久性状态，0为非状态，其它值为有时效性状态魔法效果
	//需要将状态数据与非状态数据分离出来，放入相应的数组内，并记录总数量
	
	for (int i  = 0 ; i <= magic_normal_end; ++i)
	{
		if (strcmp(szMagicAttribName,MagicAttrib2String(i))==0)   //如果属性名 和 源数据属性名相同
		{
            pcszTemp = szValue;
            nValue1 = KSG_StringGetInt(&pcszTemp, 0);
            KSG_StringSkipSymbol(&pcszTemp, ',');
            nValue2 = KSG_StringGetInt(&pcszTemp, 0);
            KSG_StringSkipSymbol(&pcszTemp, ',');
            nValue3 = KSG_StringGetInt(&pcszTemp, 0);
			//sscanf(szValue, "%d,%d,%d", &nValue1, &nValue2, &nValue3);


			if (i > magic_missle_begin && i < magic_missle_end) 
			{
				m_MissleAttribs[m_nMissleAttribsNum].nAttribType = i;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[0] = nValue1;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[1] = nValue2;
				m_MissleAttribs[m_nMissleAttribsNum].nValue[2] = nValue3;
				m_nMissleAttribsNum ++;
				return TRUE;
			}
			if (i > magic_skill_begin && i < magic_skill_end) //技能方面的 属性
			{
				switch(i)
				{
				case magic_skill_cost_v:				// 消耗MANA
					{
						m_nCost = nValue1;
					}
					break;
					
				case magic_skill_costtype_v:
					{
						m_nSkillCostType = (NPCATTRIB)nValue1;
					}break;
					
				case magic_skill_mintimepercast_v: 		       // 每次发魔法的间隔时间
					{
						m_nMinTimePerCast = nValue1;
					}break;

				case magic_skill_mintimepercastonhorse_v: 		// 骑马冷冻时间
					{
						m_nMinTimePerCastOnHorse = nValue1;
					}break;	
				case magic_skill_misslenum_v:
					{
						m_nChildSkillNum = nValue1;
					}break;
					
				case magic_skill_misslesform_v:
					{
						m_eMisslesForm = (eMisslesForm) nValue1;
					}break;
				case magic_skill_param1_v:
					{
						m_nValue1 = nValue1;   //子弹间的间隔
					}
					break;
				case magic_skill_param2_v:	
					{
						m_nValue2 = nValue2;   //距离NPC多远处开始产生子弹
					}
					break;
				case magic_skill_eventskilllevel:   //子弹事件等级
					{
						m_nEventSkillLevel = nValue1;
//#ifdef _SERVER
//						printf("--脚本事件数据:%d,脚本:%d--\n",m_nEventSkillLevel,nValue1);
//#endif 
					}
					break;
				case magic_skill_desc:  //技能描述
					{

						t_sprintf(m_szDesc,szValue);
					}
					break;
				case magic_skill_skillexp_v:
					{
						m_nSKillExp=nValue1;  //该技能的总熟练度
					}
					break;

				case magic_skill_attackradius:
					{
					   m_nAttackRadius =  nValue1;
					   m_nBackAttackRadius = nValue1;
					}
					break;
				}
			
				return TRUE;
			}
			
			if (i > magic_damage_begin && i < magic_damage_end)
			{//伤害技能
				switch(i)
				{               
				case magic_attackrating_p:
				case magic_attackrating_v:  //命中率百分比
					m_DamageAttribs[0].nAttribType = i;
					m_DamageAttribs[0].nValue[0] = nValue1;
					m_DamageAttribs[0].nValue[1] = nValue2;
					m_DamageAttribs[0].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_ignoredefense_p: //忽略敌人闪避率
					m_DamageAttribs[1].nAttribType = i;
					m_DamageAttribs[1].nValue[0] = nValue1;
					m_DamageAttribs[1].nValue[1] = nValue2;
					m_DamageAttribs[1].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_physicsenhance_p:     //普攻百分比
				//case magic_physicsdamage_v:      //普点伤害
					m_DamageAttribs[2].nAttribType = i;
					m_DamageAttribs[2].nValue[0] = nValue1;
					m_DamageAttribs[2].nValue[1] = nValue2;
					m_DamageAttribs[2].nValue[2] = nValue3;   
					m_nDamageAttribsNum ++;
					break;
				//case magic_coldenhance_p:        //冰攻加强
				case magic_colddamage_v:         //冰攻伤害
					m_DamageAttribs[3].nAttribType = i;
					m_DamageAttribs[3].nValue[0] = nValue1;
					m_DamageAttribs[3].nValue[1] = nValue2;
					m_DamageAttribs[3].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_fireenhance_p:   //火攻加强
				case magic_firedamage_v:    //火攻伤害
					m_DamageAttribs[4].nAttribType = i;
					m_DamageAttribs[4].nValue[0] = nValue1;
					m_DamageAttribs[4].nValue[1] = nValue2;
					m_DamageAttribs[4].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_lightingenhance_p: //雷攻加强
				case magic_lightingdamage_v:  //雷攻伤害
					m_DamageAttribs[5].nAttribType = i;
					m_DamageAttribs[5].nValue[0] = nValue1;
					m_DamageAttribs[5].nValue[1] = nValue2;
					m_DamageAttribs[5].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				//case magic_poisonenhance_p:   //毒攻加强
				case magic_poisondamage_v:    //毒攻伤害
					m_DamageAttribs[6].nAttribType = i;
					m_DamageAttribs[6].nValue[0] = nValue1;
					m_DamageAttribs[6].nValue[1] = nValue2;
					m_DamageAttribs[6].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					/*m_StateAttribs[m_nStateAttribsNum].nAttribType = i;
					m_StateAttribs[m_nStateAttribsNum].nValue[0] = nValue1;
					m_StateAttribs[m_nStateAttribsNum].nValue[1] = nValue2;
					m_StateAttribs[m_nStateAttribsNum].nValue[2] = nValue3;
					m_nStateAttribsNum ++; */
					break;
				case magic_magicenhance_p:   //魔法加强
				case magic_magicdamage_v:    //魔法伤害
					m_DamageAttribs[7].nAttribType = i;
					m_DamageAttribs[7].nValue[0] = nValue1;
					m_DamageAttribs[7].nValue[1] = nValue2;
					m_DamageAttribs[7].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_stun_p:  // 眩晕
					m_DamageAttribs[8].nAttribType = i;
					m_DamageAttribs[8].nValue[0] = nValue1;
					m_DamageAttribs[8].nValue[1] = nValue2;
					m_DamageAttribs[8].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_deadlystrike_p: //会心攻击
				case magic_deadlystrikeenhance_p:
					m_DamageAttribs[9].nAttribType = i;
					m_DamageAttribs[9].nValue[0] = nValue1;
					m_DamageAttribs[9].nValue[1] = nValue2;
					m_DamageAttribs[9].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_fatallystrike_p: //致命一击
					m_DamageAttribs[10].nAttribType = i;
					m_DamageAttribs[10].nValue[0] = nValue1;
					m_DamageAttribs[10].nValue[1] = nValue2;
					m_DamageAttribs[10].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_steallifeenhance_p:	
				case magic_steallife_p:  //吸血
					m_DamageAttribs[11].nAttribType = i;
					m_DamageAttribs[11].nValue[0] = nValue1;
					m_DamageAttribs[11].nValue[1] = nValue2;
					m_DamageAttribs[11].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_stealmanaenhance_p:		
				case magic_stealmana_p: //吸蓝
					m_DamageAttribs[12].nAttribType = i;
					m_DamageAttribs[12].nValue[0] = nValue1;
					m_DamageAttribs[12].nValue[1] = nValue2;
					m_DamageAttribs[12].nValue[2] = nValue3;
					m_nDamageAttribsNum ++;
					break;
				case magic_seriesdamage_p: //五行相克
					m_DamageAttribs[13].nAttribType = i;
					m_DamageAttribs[13].nValue[0] = nValue1;  
					m_DamageAttribs[13].nValue[1] = nValue2;
					m_DamageAttribs[13].nValue[2] = nValue3;
					m_nDamageAttribsNum ++; //伤害属性
					break;
				case magic_autoattackskill: //自动技能
					m_DamageAttribs[14].nAttribType = i;
					m_DamageAttribs[14].nValue[0] = nValue1;  //技能ID
					m_DamageAttribs[14].nValue[1] = nValue2;  //等级
					m_DamageAttribs[14].nValue[2] = nValue3;  //几率
					m_nDamageAttribsNum ++;
					m_StateAttribs[m_nStateAttribsNum].nAttribType = i;
					m_StateAttribs[m_nStateAttribsNum].nValue[0] = nValue1;
					m_StateAttribs[m_nStateAttribsNum].nValue[1] = nValue2;
					m_StateAttribs[m_nStateAttribsNum].nValue[2] = nValue3;
					m_nStateAttribsNum ++;  //技能状态属性的数量
						break;
				case magic_physicsdamage_v: //普点伤害
                    m_DamageAttribs[15].nAttribType = i;
				    m_DamageAttribs[15].nValue[0] = nValue1;  
				    m_DamageAttribs[15].nValue[1] = nValue2;
				    m_DamageAttribs[15].nValue[2] = nValue3;
				    m_nDamageAttribsNum ++; //伤害属性 
				     break;  
				case magic_poisonenhance_p:   //毒发时间
				    m_DamageAttribs[16].nAttribType = i;
				    m_DamageAttribs[16].nValue[0] = nValue1;  
				    m_DamageAttribs[16].nValue[1] = nValue2;
				    m_DamageAttribs[16].nValue[2] = nValue3;
				    m_nDamageAttribsNum ++; //伤害属性 
				     break;
				case magic_coldenhance_p:     //造成迟缓时间
				    m_DamageAttribs[17].nAttribType = i;
				    m_DamageAttribs[17].nValue[0] = nValue1;  
				    m_DamageAttribs[17].nValue[1] = nValue2;
				    m_DamageAttribs[17].nValue[2] = nValue3;
				    m_nDamageAttribsNum ++; //伤害属性 
				     break;  
			     case  magic_addzhuabu_v:      //抓捕伤害点
				    m_DamageAttribs[18].nAttribType = i;
				    m_DamageAttribs[18].nValue[0] = nValue1;  
				    m_DamageAttribs[18].nValue[1] = nValue2;
				    m_DamageAttribs[18].nValue[2] = nValue3;
				    m_nDamageAttribsNum ++; //伤害属性  
         	        break;
            }  

				return TRUE;
			}        
		   
			//剩下的为数据计算时的数据参数
			//根据nValue2值决定状态参数还是非状态参数
			 if (i == magic_mintimepercastonhorse_v)	// 骑马每次发魔法的间隔时间             	
			 {  
				m_nMinTimePerCastOnHorse = nValue1;
				// m_nMinTimePerCast= nValue1;
				return TRUE;
			 }
		     else if (i == magic_skill_flyevent  ||
				 i == magic_skill_collideevent ||
				 i == magic_skill_vanishedevent ||
                 i == magic_skill_startevent ||
				 i == magic_skill_showevent ||
				 i == magic_addskilldamage1 || //后来加的
				 i == magic_addskilldamage2 ||
				 i == magic_addskilldamage3 ||
				 i == magic_addskilldamage4 ||
				 i == magic_addskilldamage5 ||
				 i == magic_addskilldamage6 ||
				// i == magic_skill_enhance ||
				nValue2 == 0 
				)  
			{ 
				m_ImmediateAttribs[m_nImmediateAttribsNum].nAttribType = i;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[0] = nValue1;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[1] = nValue2;
				m_ImmediateAttribs[m_nImmediateAttribsNum].nValue[2] = nValue3;	//
				m_nImmediateAttribsNum ++;

			    if  (i == magic_skill_showevent)
				{
					 m_nShowEvent = nValue1;
				}

				if  (i == magic_skill_startevent)
				{
					if (nValue1 >0)
					   m_bStartEvent=TRUE;
					else
					   m_bStartEvent=FALSE;
					
					m_nStartSkillId = nValue3;
				}

				if  (i == magic_skill_flyevent)
				{
					if (nValue1 >0)
						m_bFlyingEvent=TRUE;
					else
						m_bFlyingEvent=FALSE;

					if (nValue2>0)
					   m_nFlyEventTime	= nValue2;
					else
					   m_nFlyEventTime  = 0;
					
					m_nFlySkillId = nValue3;
				}

				if  (i == magic_skill_collideevent)
				{

					if (nValue1 >0)
						m_bCollideEvent=TRUE;
					else
						m_bCollideEvent=FALSE;

					m_nCollideSkillId = nValue3;
				}

				if  (i == magic_skill_vanishedevent)
				{
					if (nValue1 >0)
						m_bVanishedEvent=TRUE;
					else
						m_bVanishedEvent=FALSE;
					
					m_nVanishedSkillId = nValue3;

				}
				
				
				//	m_nCollideSkillId = 
				/*
				magic_skill_collideevent,       // 碰撞子弹时
				magic_skill_vanishedevent,      // 子弹结束时
				magic_skill_startevent,         // 子弹开始时
				magic_skill_flyevent,		    // 子弹飞行时

				  BOOL				m_bFlyingEvent;			//	是否需要在飞行过程消息发生是，调用相关回调函数
				  BOOL				m_bStartEvent;			//	是否需要在技能第一次Active时，调用相关回调函数
				  BOOL				m_bCollideEvent;		//	是否需要在子技能魔法碰撞时，调用相关回调函数
				  BOOL				m_bVanishedEvent;		//	是否需要在子技能消亡时，调用相关的回调函数

				  int					m_nFlySkillId;			//	整 个飞行的相关技能
				  int					m_nFlyEventTime;		//	每多少帧回调FlyEvent;
				  int                 m_nShowEvent;			//  是否显示子弹事件
				  int					m_nStartSkillId;		//	技能刚刚才发出时所引发的事件时，所需要的相关技能id
				  int					m_nVanishedSkillId;		//	技能发出的子弹结束时引发的技能Id;
				  int					m_nCollideSkillId;		//	技能发出的子弹碰撞到物件引发的技能Id;

				*/

				return TRUE;				
			}
			else
			{//其他技能的全为当前状态光环属性
				if (i==magic_autoreplyskill)
				{
					m_StateAttribs[m_nStateAttribsNum].nAttribType = i;
					int	nSkillInfo = MAKELONG((nValue1-ulLevel)/256,ulLevel); 
					m_StateAttribs[m_nStateAttribsNum].nValue[0] = nSkillInfo; //技能等级+技能ID
					m_StateAttribs[m_nStateAttribsNum].nValue[1] = nValue2;    //概率	-1
				    m_StateAttribs[m_nStateAttribsNum].nValue[2] = nValue3/(256*18)+nValue3%(256*18); //释放时间 F

				}
				else
				{
				  m_StateAttribs[m_nStateAttribsNum].nAttribType = i;
				  m_StateAttribs[m_nStateAttribsNum].nValue[0] = nValue1; //脚本中发回的第一个值
				  m_StateAttribs[m_nStateAttribsNum].nValue[1] = nValue2; //脚本中发回的第二个值 （持续的时间）
				  m_StateAttribs[m_nStateAttribsNum].nValue[2] = nValue3; //脚本中发回的第三个值
				}

				m_nStateAttribsNum ++;
				return TRUE;
			}
		    
	  		
		}
	}
	return FALSE;
}

const char * KSkill::MagicAttrib2String(int MagicAttrib)  const 
{
	return 	g_MagicID2String(MagicAttrib);
}
//绘画技能图标
#ifndef _SERVER
void	KSkill::DrawSkillIcon(int x, int y, int Width, int Height,int nParam)  
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
	char szNum[4];

	if (nParam>-1)
	{
	 
	  int 	nLen = t_sprintf(szNum,"%d", nParam+1);
	  //g_pRepresent->OutputText(14, szNum, KRF_ZERO_END,x,y,0xFFFFFF00);
//	  g_pRepresent->OutputText(14, szNum, KRF_ZERO_END,x,y, TGetColor("255,255,0"));     //黄色

	}
	/*else
	{//绘画内外功
		if (m_nIsMagic==0 && (m_eSkillStyle==0 ||m_eSkillStyle==1))
		{
		   t_sprintf(szNum,"外");
		   g_pRepresent->OutputText(14, szNum, KRF_ZERO_END,x,y, TGetColor("66,56,252")); //蓝色
		}
	}*/
}

void KSkill::GetDesc(unsigned long ulSkillId, unsigned long ulCurLevel, char * pszMsg, int nOwnerIndex,  bool bGetNextLevelDesc, int nAddPoint,int nEnChance)
{
	
	if (!pszMsg) return;
	if (nOwnerIndex <= 0 )	return ;
	strcpy(pszMsg,"");
	char szTemp[300];
	
	KSkill * pTempSkill = NULL;
	KSkill * pCurSkill = NULL;
	KSkill * pNextSkill = NULL;
	
	if(ulCurLevel == 0)
	{
		pNextSkill = (KSkill *)g_SkillManager.GetSkill(ulSkillId, 1);
		pTempSkill = pNextSkill;
	}
	else
	{
		pCurSkill = (KSkill *) g_SkillManager.GetSkill(ulSkillId, ulCurLevel);
		pNextSkill = (KSkill *) g_SkillManager.GetSkill(ulSkillId, ulCurLevel + 1);//下一等级
		pTempSkill = pCurSkill;
	} 
	
	if (pTempSkill == NULL)
	{
		return;
	}
	
	strcat(pszMsg, "<color=Yellow>");
	strcat(pszMsg, pTempSkill->GetSkillName());       //技能名
	strcat(pszMsg, "<color>\n");

	strcat(pszMsg, pTempSkill->m_szSkillDesc);	//技能的描述
	strcat(pszMsg, "\n");
	if ((pTempSkill->m_eSkillStyle == SKILL_SS_Melee||pTempSkill->m_eSkillStyle ==SKILL_SS_PassivityNpcState||pTempSkill->m_eSkillStyle ==SKILL_SS_InitiativeNpcState|| pTempSkill->m_eSkillStyle == SKILL_SS_Missles))
	{
      int nAttribId = pTempSkill->IsAttrib();  //武功流派
	  if (nAttribId <=1)
	  { 
		if (pTempSkill->IsMagic())  //1 为内功系  0为外攻系
			strcat(pszMsg, "<color=Fire>空手普通攻击<color>");
		else
            strcat(pszMsg, "<color=Fire>空手普通攻击<color>");
	  } 
	  else
	  {//技能描述 
	   char nSkey[6]={0},sDecsInfo[125]={0};
	   t_sprintf(nSkey,"%d",nAttribId);
       g_GameSetTing.GetString("SkillAttrib",nSkey,"<color=red>暂无数据<color>",sDecsInfo,sizeof(sDecsInfo));
	   strcat(pszMsg, sDecsInfo);
	  } 
	  strcat(pszMsg,"\n");
	}
	
//	if (!pTempSkill->IsPhysical())
	{
		if (nAddPoint)
		{
			t_sprintf(szTemp, "当前等级：%d<color=BLUE>(%d+%d)<color>",ulCurLevel , ulCurLevel - nAddPoint,nAddPoint);
		} 
		else
		{
			t_sprintf(szTemp, "当前等级：%d", ulCurLevel);
		}		
		strcat(pszMsg, szTemp);
		strcat(pszMsg, "\n");

		if (pTempSkill->m_bUseAttackRate)
		{
			strcat(pszMsg, "<color=yellow>技能特性：需要命中率<color>");
		    strcat(pszMsg, "\n");
		}

	    if (pTempSkill->m_nIsExpSkill)
		{//如果是 熟练度技能
			int  nCurExpSKill=0;
              //if (Npc[nOwnerIndex].m_SkillList.FindSame(ulSkillId))
				  nCurExpSKill=Npc[nOwnerIndex].m_SkillList.GetCurSkillExp(ulSkillId);
		      
			  int nper =0;
			  if (pTempSkill->m_nSKillExp)
				  nper= nCurExpSKill/(pTempSkill->m_nSKillExp/100);
			  
			  t_sprintf(szTemp, "当前熟练度：%d%s", nper,"%");
			  strcat(pszMsg, szTemp);
		      strcat(pszMsg, "\n");
		} 
	}
	
	if (nEnChance)   //技能加成
	{
		t_sprintf(szTemp, "受技能加成：+<color=Fire>%d%s<color>", nEnChance,"%");
		strcat(pszMsg, "\n");
		strcat(pszMsg, szTemp);
		strcat(pszMsg, "\n");
	}
//	strcat (pszMsg, "\n");
	int i = 0;	
	if (pCurSkill)
	{//但前技能
		pCurSkill->GetDescAboutLevel(pszMsg);	
	}	
	strcat (pszMsg, "\n");
	strcat(pszMsg,"技能限制：");
	//if (-2 !=pTempSkill->m_nEquiptLimited)
	{
	
            char nSkillDesc[64]={0};
			char nKey[8]={0};
            if (pTempSkill->m_nEquiptLimited==-2)
                 t_sprintf(nKey,"%s","F1");  //无限制
			else if (pTempSkill->m_nEquiptLimited==-1)
                 t_sprintf(nKey,"%s","F2");  //空手
            else
			     t_sprintf(nKey,"%d",pTempSkill->m_nEquiptLimited);

			g_GameSetTing.GetString("WeaponLimit",nKey,"暂无数据",nSkillDesc,sizeof(nSkillDesc));
			strcat(pszMsg, nSkillDesc);
			strcat(pszMsg, "\n");
	}

	if (pTempSkill->m_nHorseLimited)  //骑马限制
	{
		switch(pTempSkill->m_nHorseLimited)
		{
		case 1:
			{
				strcat(pszMsg, "<color=Red>骑马不能使用该技能<color>\n");
			}
			break;
		case 2:
			{
				strcat(pszMsg, "<color=Pink>骑马才能使用该技能<color>\n");
			}
			break;
		default:
			return ;
		}
	}
//	if (!pTempSkill->IsPhysical())
	if (bGetNextLevelDesc)
	{
	   if (pNextSkill)
	   {
			strcat(pszMsg, "<color=Red>下一等级\n");
			pNextSkill->GetDescAboutLevel(pszMsg);
			//strcat(pszMsg, "<color>\n");
	   }
	}
	/*strcat(pszMsg,"<color=HGREEN>");
	strcat(pszMsg,VER_INFO);
	strcat(pszMsg,"\n");
	strcat(pszMsg,"本服特色:寻路-内挂-熔炼-瞬移");
	strcat(pszMsg,"\n");							  
	strcat(pszMsg,"经典设置:经脉-押镖-更多<color>");
	strcat(pszMsg,"\n"); */
}

void KSkill::GetDescAboutLevel(char * pszMsg)
{	
	char szTemp[80]={0};
	int nGetCost = GetSkillCost(NULL);
	if (nGetCost)
	{
#ifdef WIN32
		switch(m_nSkillCostType)  //发该技能所需的内力、体力等的消耗
		{
		case attrib_mana:		
			t_sprintf(szTemp, "消耗内力：%d\n", nGetCost);
			strcat(pszMsg,szTemp);
			break;
		case attrib_stamina:
			t_sprintf(szTemp, "消耗体力：%d\n", nGetCost);
			strcat(pszMsg,szTemp);
			break;
		case attrib_life:
			t_sprintf(szTemp, "消耗生命：%d\n", nGetCost);
			strcat(pszMsg,szTemp);
			break;
		}
#else
		switch(m_nSkillCostType)  //发该技能所需的内力、体力等的消耗
		{
		case attrib_mana:		
			t_sprintf(szTemp, UTEXT("消耗内力：%d\n",1).c_str(), nGetCost);
			strcat(pszMsg,szTemp);
			break;
		case attrib_stamina:
			t_sprintf(szTemp, UTEXT("消耗体力：%d\n",1).c_str(), nGetCost);
			strcat(pszMsg,szTemp);
			break;
		case attrib_life:
			t_sprintf(szTemp, UTEXT("消耗生命：%d\n",1).c_str(), nGetCost);
			strcat(pszMsg,szTemp);
			break;
		}
#endif
	}	
	int nGetAttackRadius = GetAttackRadius();
	if (nGetAttackRadius)
	{
#ifdef WIN32
		t_sprintf(szTemp,"攻击范围：%d\n", nGetAttackRadius);
#else
		t_sprintf(szTemp,UTEXT("攻击范围：%d\n",1).c_str(), nGetAttackRadius);
#endif
		strcat(pszMsg,szTemp);
	}

	//不随等级变化的立即伤害
	int i;
	for (i  = 0; i < m_nImmediateAttribsNum; i++)
	{
		if (!m_ImmediateAttribs[i].nAttribType)
			continue;
		//KMagicAttrib nCurAttrib;
		//ZeroMemory(&nCurAttrib,sizeof(KMagicAttrib));
		//nCurAttrib = Item[m_ItemData.uId].getaryMagicAttrib(i);
#ifdef WIN32
		char * pszInfo = (char *)g_MagicDesc.GetDesc(&m_ImmediateAttribs[i],1);
#else
        char pszInfo[640];
        ZeroMemory(pszInfo,sizeof(pszInfo));
        g_MagicDesc.GetDesc_New(pszInfo,&m_ImmediateAttribs[i]);
#endif 
		if (!pszInfo || !pszInfo[0]) 
			continue;

		strcat(pszMsg, pszInfo);
		strcat(pszMsg, "\n");
		
	}
	//子弹随玩家属性计算而成的伤害  伤害技能
	//KMagicAttrib *DamageAttribs[MAX_MISSLE_DAMAGEATTRIB];
	KMagicAttrib *DamageAttribs = m_DamageAttribs;
	//根据玩家的基本属性，确定子弹的伤害
	//Npc[nOwnerIndex].AppendSkillEffect(m_DamageAttribs, DamageAttribs);
	for (i  = 0; i < MAX_MISSLE_DAMAGEATTRIB; i++) //伤害攻击属性的限制
	{
		if (!(DamageAttribs + i)->nAttribType) 
			continue;
#ifdef WIN32
		char * pszInfo = (char *)g_MagicDesc.GetDesc((DamageAttribs + i),1);
#else
        char pszInfo[640];
        ZeroMemory(pszInfo,sizeof(pszInfo));
        g_MagicDesc.GetDesc_New(pszInfo,(DamageAttribs + i));
#endif 
		if (!pszInfo || !pszInfo[0])
			continue;

		strcat(pszMsg, pszInfo);
		strcat(pszMsg, "\n");	
	}

	//被动技能光环状态
	for (i  = 0; i < m_nStateAttribsNum; i++)  // 状态属性数量？
	{
		if (!m_StateAttribs[i].nAttribType) continue;
#ifdef WIN32
		char * pszInfo = (char *)g_MagicDesc.GetDesc(&m_StateAttribs[i],1); 
#else
        char pszInfo[640];
        ZeroMemory(pszInfo,sizeof(pszInfo));
        g_MagicDesc.GetDesc_New(pszInfo,&m_StateAttribs[i]);
#endif
		if (!pszInfo || !pszInfo[0]) 
			continue;
		strcat(pszMsg, pszInfo);
		strcat(pszMsg, "\n");

	}

	/*if (m_szDesc[0])
	{//
		int len = sizeof(m_szDesc);
		int newLen = TEncodeText_(m_szDesc,strlen(m_szDesc));
		FilterTextColor(m_szDesc,newLen);
		strcat(pszMsg,m_szDesc);	//技能的描述
	}*/

	int nCount = 0;
	//立即技能
	for (i  = 0; i < m_nImmediateAttribsNum; i++)
	{
		if (/*m_bCollideEvent && */m_ImmediateAttribs[i].nAttribType == magic_skill_collideevent && m_ImmediateAttribs[i].nValue[0])
		{
			KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);
			strcat(pszMsg, "\n");
#ifdef WIN32
			if (m_bByMissle && !nCount)
				strcat(pszMsg, "第三式: "); 
			else
				strcat(pszMsg, "第三式: ");
			strcat(pszMsg,pTempSkill->GetSkillName());
#else
			if (m_bByMissle && !nCount)
				strcat(pszMsg, UTEXT("第三式: ",1).c_str()); 
			else
				strcat(pszMsg, UTEXT("第三式: ",1).c_str());
            char nTemp[80];
            t_sprintf(nTemp,pTempSkill->GetSkillName());
			strcat(pszMsg,UTEXT(nTemp,1).c_str());
			
#endif
			strcat(pszMsg, "\n");
			pTempSkill->GetDescAboutLevel(pszMsg);
			//strcat(pszMsg, "\n");
			nCount++;
			continue;
		}
		else
		if (/*m_bVanishedEvent &&*/ m_ImmediateAttribs[i].nAttribType == magic_skill_vanishedevent && m_ImmediateAttribs[i].nValue[0])
		{
			KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);
			strcat(pszMsg, "\n");
#ifdef WIN32
			if (m_bByMissle && !nCount)
				strcat(pszMsg, "第四式: ");
			else
				strcat(pszMsg, "第四式: ");
			strcat(pszMsg,pTempSkill->GetSkillName());
#else
			if (m_bByMissle && !nCount)
				strcat(pszMsg, UTEXT("第四式: ",1).c_str());
			else
				strcat(pszMsg, UTEXT("第四式: ",1).c_str());
			char nTemp[80];
			t_sprintf(nTemp,pTempSkill->GetSkillName());
			strcat(pszMsg,UTEXT(nTemp,1).c_str());
#endif
			strcat(pszMsg, "\n");
			pTempSkill->GetDescAboutLevel(pszMsg);
			//strcat(pszMsg, "\n");
			nCount++;
			continue;
		}
		else
		if (/*m_bStartEvent &&*/ m_ImmediateAttribs[i].nAttribType == magic_skill_startevent && m_ImmediateAttribs[i].nValue[0])
		{
			KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);
			strcat(pszMsg, "\n");
#ifdef WIN32
			if (m_bByMissle && !nCount)
				strcat(pszMsg, "第一式: "); 
			else
				strcat(pszMsg, "第一式: ");
			strcat(pszMsg,pTempSkill->GetSkillName());
#else
			if (m_bByMissle && !nCount)
				strcat(pszMsg, UTEXT("第一式: ",1).c_str()); 
			else
				strcat(pszMsg, UTEXT("第一式: ",1).c_str());
			char nTemp[80];
			t_sprintf(nTemp,pTempSkill->GetSkillName());
			strcat(pszMsg,UTEXT(nTemp,1).c_str());
#endif
			//strcat(pszMsg,pTempSkill->GetSkillName());  
			strcat(pszMsg, "\n");
			pTempSkill->GetDescAboutLevel(pszMsg);
			//strcat(pszMsg, "\n");
			nCount++;
			continue;
		}
		else
		if (/*m_bStartEvent &&*/ m_ImmediateAttribs[i].nAttribType == magic_skill_flyevent && m_ImmediateAttribs[i].nValue[0])
		{
			KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);
			strcat(pszMsg, "\n");
#ifdef WIN32
			if (m_bByMissle && !nCount)
				strcat(pszMsg, "第二式: ");
			else
				strcat(pszMsg, "第二式: ");
			strcat(pszMsg,pTempSkill->GetSkillName());
#else
			if (m_bByMissle && !nCount)
				strcat(pszMsg, UTEXT("第二式: ",1).c_str());
			else
				strcat(pszMsg, UTEXT("第二式: ",1).c_str());
			char nTemp[80];
			t_sprintf(nTemp,pTempSkill->GetSkillName());
			strcat(pszMsg,UTEXT(nTemp,1).c_str());
#endif
			//strcat(pszMsg,pTempSkill->GetSkillName());
			strcat(pszMsg, "\n");
			pTempSkill->GetDescAboutLevel(pszMsg);
			nCount++;		
			continue;
		}
	}
	
}

void KSkill::PlayPreCastSound(BOOL bIsFeMale, int nX, int nY)  const 
{
	/*char * pSoundFile = NULL;
	if ( !bIsFeMale)
		pSoundFile = (char *)m_szManPreCastSoundFile;
	else 
		pSoundFile = (char *)m_szFMPreCastSoundFile;
	
	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;
	
	// 获得屏幕中心点的地图坐标 not end
	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);
	KCacheNode * pSoundNode = NULL;
	pSoundNode = (KCacheNode*) g_SoundCache.GetNode(pSoundFile, (KCacheNode*)pSoundNode);
	KWavSound * pWave = (KWavSound*)pSoundNode->m_lpData;
	if (pWave)
	{
		pWave->Play((nX - nCenterX) * 5, (10000 - (abs(nX - nCenterX) + abs(nY - nCenterY))) * Option.GetSndVolume() / 100 - 10000, 0);
	}*/
}
#endif

//技能状态  释放设置技能状态效果
BOOL KSkill::CastStateSkill( int nLauncher, int nParam1, int nParam2, int nWaitTime) const
{	return TRUE;
}

//技能状态  释放设置技能状态效果
BOOL KSkill::CastToolStateSkill( int nLauncher, int nParam1, int nParam2, int nWaitTime) const
{
	return TRUE;
}

//使用攻击技能,同步到客户端使用技能
BOOL KSkill::CastSkill( int nLauncher,int nSkillID,int nParam1, int nParam2,int nLevel,int nWaitTime ,int nMaxBei) const
{
	return TRUE;
}
