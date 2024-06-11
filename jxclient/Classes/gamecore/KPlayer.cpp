#include "KCore.h"
#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER 0x02A1
#endif
#include "engine/KEngine.h"
//#include "../../Headers/IClient.h"
//#include "Scene/KScenePlaceC.h"
//#include "KIme.h"
#include "KNpcAI.h"
#include "KNpc.h"
#include "KObj.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#include "engine/Text.h"
#include "LuaFuns.h"
#include "KSortScript.h"
//#include "KScriptValueSet.h"
#include "KObjSet.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KSubWorldSet.h"
#include "engine/KFile.h"
#include "GameDataDef.h"
#include "KBuySell.h"
//#include "MyAssert.h"
#include "MsgGenreDef.h"
#include "KItemSet.h"
#include "KTaskFuns.h"
#include "engine/Text.h"
#include "KSellItem.h"
//#include "KForBitGua.h"
#include  <map>
#include  "CoreShell.h"
extern int LuaGetNpcTalk(Lua_State *L);
#define		defPLAYER_LOGIN_TIMEOUT			20 * 20   // 10 sec
#define		PLAYER_LEVEL_1_EXP				48
#define		PLAYER_LEVEL_ADD_ATTRIBUTE		5
#define		PLAYER_LEVEL_ADD_SKILL			1
#define		PLAYER_SHARE_EXP_DISTANCE		768
#define		PLAYER_TEAM_EXP_ADD				50

#define		MAX_APPLY_TEAM_TIME				500

#define		BASE_WALK_SPEED					5    //走速
#define		BASE_RUN_SPEED					10   //跑速

#define		BASE_ATTACK_SPEED				0       //基本的攻击速度
#define		BASE_CAST_SPEED					0       //基本的出招速度
#define		BASE_VISION_RADIUS				120     //
#define		BASE_HIT_RECOVER				6

#define		TOWN_PORTAL_TIME				1800    //回城时间


KPlayer	*Player = NULL;//Player[MAX_PLAYER];
int		g_nLastNetMsgLoop;

//-------------------------------------------------------------------------
//	功能：构造函数
//-------------------------------------------------------------------------
KPlayer::KPlayer()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：析构函数
//-------------------------------------------------------------------------
KPlayer::~KPlayer()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：清空  退出游戏时候
//-------------------------------------------------------------------------
void	KPlayer::Release()
{
	m_isopenjuli   = false;
	m_nLeftSkillID = 0;
	m_nLeftListidx = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightListidx = 0;
	m_nRightSkillLevel = 0;

	m_DebugMode=FALSE;
	m_RunStatus = 0;
	m_dwNextLevelLeadExp = 0;
	/*m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;*/
	m_nSendMoveFrames = defMAX_PLAYER_SEND_MOVE_FRAME;
	m_MouseDown[0] = FALSE;
	m_MouseDown[1] = FALSE;
	m_nImageNpcID  =0;
	m_ItemLinkDwid =0;
	//ZeroMemory(szClienResult,sizeof(szClienResult));

	m_MissionData.Clear();
	memset(m_MissionRank, 0, sizeof(m_MissionRank));

	m_nCurPinDao=-1;

	m_GameLeftTime = 0;
	m_Isinvalid=FALSE;
	m_dwID = 0;
	m_nIndex = 0;
	m_nNetConnectIdx = -1;
	m_cMenuState.Release();
	m_cTrade.Release();
	m_nAttributePoint = 0;
	m_nSkillPoint = 0;

	m_nStrength = 0;
	m_nDexterity = 0;
	m_nVitality = 0;
	m_nEngergy = 0;
	m_nLucky = 0;
	m_nCurStrength = 0;
	m_nCurDexterity = 0;
	m_nCurVitality = 0;
	m_nCurEngergy = 0;
	m_nCurLucky = 0;
	m_nTempLucky_p=0;
	m_btChatSpecialChannel = 0;
	m_nUpExp = 0;
	
	m_nExp = 0;
	m_nNextLevelExp = PLAYER_LEVEL_1_EXP;
	m_bExchangeServer = FALSE;
	m_CUnlocked = 0;  //默认解锁
	
	m_dwLeadExp = 0;
	m_dwLeadLevel = 1;
	
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;	//
    m_nForbiddenFlag = 0; //默认是解除禁言
	m_nExtPoint = 0;

	m_ExpTime= (g_SubWorldSet.m_nLoopRate)/(18*60);
	m_ExpPiontTime= (g_SubWorldSet.m_nLoopRate)/(18*60);
	m_YinLiangTime= (g_SubWorldSet.m_nLoopRate)/(18*60);
	m_JinBiTime= (g_SubWorldSet.m_nLoopRate)/(18*60);

}
//-------------------------------------------------------------------------
//	功能：设定 m_nPlayerIndex (本函数只允许在PlayerSet的Init中调用)
//-------------------------------------------------------------------------
void	KPlayer::SetPlayerIndex(int nNo)
{
	if (nNo < 0)
		m_nPlayerIndex = 0;
	else
		m_nPlayerIndex = nNo;
}

//-------------------------------------------------------------------------
//	功能：获得本实例在 Player 数组中的位置
//-------------------------------------------------------------------------
int		KPlayer::GetPlayerIndex()
{
	return m_nPlayerIndex;
}

//-------------------------------------------------------------------------
//	功能：玩家每次游戏循环都需要处理的东西
//-------------------------------------------------------------------------
void	KPlayer::Active()
{
	// 队伍申请人的处理
	if ( !m_cTeam.m_nFlag )
	{
		if (m_cTeam.m_nApplyCaptainID > 0)
		{
			if ( m_cTeam.m_dwApplyTimer == 0 )
			{
				m_cTeam.m_nApplyCaptainID = 0;	
			}
			else
			{
				m_cTeam.m_dwApplyTimer--;
				if ( !NpcSet.SearchID(m_cTeam.m_nApplyCaptainID) )
				{
					m_cTeam.m_nApplyCaptainID = 0;
					m_cTeam.m_dwApplyTimer = 0;
				}
			}
		}
	}
	// 队长的处理
	else if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
	{
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
		{
			if (m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
			{
				if (m_cTeam.m_sApplyList[i].m_dwTimer == 0)
				{
					m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
					m_cTeam.UpdateInterface();
					m_cTeam.UpdateamUI(); //显示主界面队伍信息
				}
				else
				{
					m_cTeam.m_sApplyList[i].m_dwTimer--;
					if ( !Npc[this->m_nIndex].SearchAroundID(m_cTeam.m_sApplyList[i].m_dwNpcID) )
					{
						m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
						m_cTeam.m_sApplyList[i].m_dwTimer = 0;
						m_cTeam.UpdateInterface();
						m_cTeam.UpdateamUI(); //显示主界面队伍信息
					}
				}
			}
		}
	}
	

	++m_nSendMoveFrames;
	// 仇杀倒计时
	this->m_cPK.Active();

	//this->m_cTong.Active();            //无限循环宣战
	

}

/*
void	KPlayer::ProcessMsg(KWorldMsgNode *lpMsg)
{
	switch (lpMsg->m_dwMsgType)
	{
	case GWM_PLAYER_SKILL:
		break;
	case GWM_PLAYER_RUNTO:
		break;
	case GWM_PLAYER_WALKTO:
		break;
	case GWM_PLAYER_JUMPTO:
		break;
	default:
		break;
	}	
}
*/

void KPlayer::ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*switch(uMsg)
	{
	case WM_MOUSEMOVE:   //鼠标离开
	case WM_MOUSEHOVER:  //鼠标经过
		if (wParam & MK_LBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		else if (wParam & MK_RBUTTON)
			OnButtonMove(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		else
			OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
		
	case WM_LBUTTONUP:   //左键上
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_left);
		break;
		
	case WM_LBUTTONDOWN: //左键按下
		OnButtonDown(LOWORD(lParam), HIWORD(lParam), 
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_left);
		break;
		
	case WM_RBUTTONUP:   //右键上
		OnButtonUp(LOWORD(lParam), HIWORD(lParam), button_right);
		break;	
		
	case WM_RBUTTONDOWN: //右键按下
		OnButtonDown(LOWORD(lParam), HIWORD(lParam),
			(wParam & ~(MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)), button_right);
		break;
	}*/
}
//按下鼠标的时候
void KPlayer::OnButtonDown(int x,int y, int Key, MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = TRUE;

	FindSelectNpc(x, y, relation_all);	 //赋值锁定 NPC 
	FindSelectObject(x, y);	             //赋值锁定 物品
	//	Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
	ProcessMouse(x, y, Key, nButton);
}

void KPlayer::OnButtonMove(int x,int y,int Key, MOUSE_BUTTON nButton)
{
	if (m_MouseDown[(int)nButton])
	{
		ProcessMouse(x, y, Key, nButton);
	}
}

void KPlayer::OnButtonUp(int x,int y,MOUSE_BUTTON nButton)
{
	m_MouseDown[(int)nButton] = 0;
}
//客户端鼠标经过显示的图标
void KPlayer::OnMouseMove(int x,int y)
{
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	FindSelectNpc(x, y, relation_all);
	FindSelectObject(x, y);

	if (m_nPeapleIdx) //m_nPeapleIdx
	{
		if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)
		{
			//char nMsg[64];
			//t_sprintf(nMsg,"提示: %s 打开面板..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_player && Npc[m_nPeapleIdx].m_BaiTan)
		{
			//char nMsg[64];
			//t_sprintf(nMsg,"提示: %s 摆摊中..", Npc[m_nPeapleIdx].Name);
			//m_ItemList.ClientShowMsg(nMsg);
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);  //设置鼠标指针图标
		}
		else if (NpcSet.GetRelation(m_nIndex, m_nPeapleIdx) == relation_enemy)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_FIGHT);
		else
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
	}
	else if (m_nObjectIdx)
	{
		if (Object[m_nObjectIdx].m_nKind == Obj_Kind_MapObj)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_DIALOG);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Item || Object[m_nObjectIdx].m_nKind == Obj_Kind_Money)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_PICK);
		else if (Object[m_nObjectIdx].m_nKind == Obj_Kind_Prop)
			CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_USE);
	}
	else
		CoreDataChanged(GDCNI_SWITCH_CURSOR, NULL, MOUSE_CURSOR_NORMAL);
}
//鼠标行为 鼠标线程
void KPlayer::ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton)
{

	if (CheckTrading())
		return;

	if (m_ItemList.m_Hand > 0)
	{
		if (nButton == button_left) //左键
		{
			ThrowAwayItem();  //丢弃物品 丢掉物品
			return;
		}
	}

	int nX = x;
	int nY = y;
	int nZ = 0;
	//g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	
	/*	if (Key & MK_SHIFT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	return;
}*/
	
	if (Npc[m_nIndex].IsCanInput())
	{

		int nIdx = 0;
		
		if (nButton == button_right)
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
			//g_DebugLog("[skill]right");
		}
		else
		{
			nIdx = Npc[m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);
			//g_DebugLog("[skill]left");
		}
		Npc[m_nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		//g_DebugLog("[skill]return");
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	//if ((Key & MK_SHIFT) || (nButton == button_right))  //强制攻击
	{
		if (Npc[m_nIndex].m_ActiveSkillID > 0)
		{
			ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(Npc[m_nIndex].m_ActiveSkillID, 1);
			if (!pISkill) 
                return;
			
			if (pISkill->IsAura())
				return;
			int nAttackRange = pISkill->GetAttackRadius();
			
			int nTargetIdx = 0;
			//m_nPeapleIdx = 0;
			//按照Object / Enemy / Ally 的优先级找到需要打的对象id
			
			
			if (pISkill->IsTargetAlly())
			{
				FindSelectNpc(x, y, relation_ally);
				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetEnemy())
			{
				FindSelectNpc(x, y, relation_enemy);

				if (m_nPeapleIdx)
				{
					nTargetIdx = m_nPeapleIdx;
				}
			}
			
			if (pISkill->IsTargetObj())
			{
				FindSelectObject(x, y);
				if (m_nObjectIdx)
				{
					nTargetIdx = m_nObjectIdx;
				}
			}
			//如果技能必须指定对象，而当前位置无对象的话，直接退出
			if (pISkill->IsTargetOnly() && !nTargetIdx)
            {
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
			
			if (m_nIndex == nTargetIdx)
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return;
			}
		  /*
			if (Npc[m_nIndex].m_SkillList.GetNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>0 && (Npc[m_nIndex].m_SkillList.GetNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
			{//还没解冻
				return;
			}
			
			if (Npc[m_nIndex].m_HorseType)
			{//骑马时间限制
				if (Npc[m_nIndex].m_SkillList.GetHorseNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>0 && (Npc[m_nIndex].m_SkillList.GetHorseNextCastTime(Npc[m_nIndex].m_ActiveSkillID)>SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
				return;
			} */
		
			if ((!Npc[m_nIndex].m_SkillList.CanCast(Npc[m_nIndex].m_ActiveSkillID, SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_dwCurrentTime))
				||(!Npc[m_nIndex].Cost(pISkill->GetSkillCostType() , pISkill->GetSkillCost(&Npc[m_nIndex]), TRUE)))
			{
				Npc[m_nIndex].m_nPeopleIdx = 0;
				m_nPeapleIdx = 0;
				return ;
			}
			
			//无对象，直接发坐标
			if (!nTargetIdx)
			{
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, nX, nY);		
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, nX, nY);
                return ;   //无对象直接退出 不发技能
			}
			else
			{
				if (pISkill->IsTargetOnly())
				{
					int distance = NpcSet.GetDistance(m_nIndex , nTargetIdx);
					if (distance > pISkill->GetAttackRadius())
					{
						m_nPeapleIdx = nTargetIdx;
						return ;
					}
				}
				// 
				if (m_nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
					return ;
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nTargetIdx);
				// Send to Server		
				SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
			}
		}

		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}
	
	if (Key == 0 )
	{//没有键盘键时
		int nRelation = NpcSet.GetRelation(m_nIndex, m_nPeapleIdx);
		if(nRelation == relation_enemy || nRelation == relation_dialog ||(Npc[m_nPeapleIdx].m_Kind==kind_player && Npc[m_nPeapleIdx].m_BaiTan==1))
		{
			Npc[m_nIndex].m_nPeopleIdx = m_nPeapleIdx;
		}
		
		if (m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
		{
			m_nPickObjectIdx = m_nObjectIdx;
			Npc[m_nIndex].m_nObjectIdx = m_nPickObjectIdx;

			if (!m_RunStatus)
			{
				Npc[m_nIndex].SendSerCommand(do_walk, nX, nY);
				// Send to Server
				SendClientCmdWalk(nX, nY);
			}
			else
			{
				Npc[m_nIndex].SendSerCommand(do_run, nX, nY);
				// Send to Server
				SendClientCmdRun(nX, nY);
			}
			m_nSendMoveFrames = 0;
		}
		return;
	}
	
	/*	if (Key & MK_ALT)
	{
	Npc[m_nIndex].SendCommand(do_jump, nX, nY);
	SendClientCmdJump(nX, nY);
	}*/
}

void KPlayer::Walk(int nDir, int nSpeed)
{
	int	nMapX = Npc[m_nIndex].m_MapX;
	int nMapY = Npc[m_nIndex].m_MapY;
	int	nOffX = Npc[m_nIndex].m_OffX;
	int	nOffY = Npc[m_nIndex].m_OffY;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nX, nY;
	
	SubWorld[nSubWorld].NewMap2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &nX, &nY);

	SubWorld[nSubWorld].GetMps(&nX, &nY, nSpeed * 2, nDir);
	
	if (m_RunStatus)
	{
		Npc[m_nIndex].SendSerCommand(do_run, nX, nY);
		// Send to Server
		if ( !CheckTrading() )
			SendClientCmdRun(nX, nY);
	}
	else
	{
		Npc[m_nIndex].SendSerCommand(do_walk, nX, nY);
		// Send to Server
		if (!CheckTrading())
			SendClientCmdWalk(nX, nY);
	}
}


void KPlayer::TurnLeft()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir > 8)
		Npc[m_nIndex].m_Dir -= 8;
	else
		Npc[m_nIndex].m_Dir = MAX_NPC_DIR - 1;
}

void KPlayer::TurnRight()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR - 9)
		Npc[m_nIndex].m_Dir += 8;
	else
		Npc[m_nIndex].m_Dir = 0;
}

void KPlayer::TurnBack()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_Dir < MAX_NPC_DIR / 2)
		Npc[m_nIndex].m_Dir += MAX_NPC_DIR / 2;
	else
		Npc[m_nIndex].m_Dir -= MAX_NPC_DIR / 2;
}

void KPlayer::FindSelectNpc(int x, int y, int nRelation)
{
	int	nNpcIdx = 0;
	
	    nNpcIdx = NpcSet.SearchNpcAt(x, y, nRelation, 16);

	if  (nNpcIdx>0)
	{
		if (Npc[nNpcIdx].m_Hide.nTime > 0 && NpcSet.GetRelation(m_nIndex,nNpcIdx) == relation_enemy)
			m_nPeapleIdx = 0; //只对敌人隐身
		else
		{
			m_nPeapleIdx  = nNpcIdx;
			m_nImageNpcID = nNpcIdx;
		} 
	}
	else
		m_nPeapleIdx = 0;
}

void KPlayer::FindSelectObject(int x, int y)
{
	int	nObjIdx = 0;
	
	nObjIdx = ObjSet.SearchObjAt(x, y, 40);
	if (nObjIdx)
		m_nObjectIdx = nObjIdx;
	else
		m_nObjectIdx = 0;
}
// need change
int	KPlayer::NetCommandPlayerTalk(BYTE * pProtocol)
{
/*	DWORD	dwNpcID;
int		nSize;
dwNpcID = *(DWORD *) &pProtocol[1];
nSize = pProtocol[5];
int nIdx = NpcSet.SearchID(dwNpcID);

  if (ConformIdx(nIdx))
  {
		strcpy(Npc[nIdx].m_szChatBuffer, (char *)(pProtocol+6));
		Npc[nIdx].m_nCurChatTime = NPC_SHOW_CHAT_TIME_LENGTH;
		}
		
		  return (7 + nSize);	// need test
	*/
	return 0;
}


BOOL KPlayer::ConformIdx(int nIdx)
{
	if (nIdx == m_nIndex || nIdx == 0)
		return FALSE;
	return TRUE;
}


//-------------------------------------------------------------------------
//	功能：新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值   ----没有调用
//-------------------------------------------------------------------------
BOOL	KPlayer::NewPlayerGetBaseAttribute(int Series)
{
	if (Series < series_metal || Series > series_earth)
		return FALSE;
	
	Npc[m_nIndex].SetSeries(Series);
	
	m_nStrength = PlayerSet.m_cNewPlayerAttribute.m_nStrength[Series];
	m_nDexterity = PlayerSet.m_cNewPlayerAttribute.m_nDexterity[Series];
	m_nVitality = PlayerSet.m_cNewPlayerAttribute.m_nVitality[Series];
	m_nEngergy = PlayerSet.m_cNewPlayerAttribute.m_nEngergy[Series];
	m_nLucky = PlayerSet.m_cNewPlayerAttribute.m_nLucky[Series]; //幸运值
	m_nUpExp = 0;  //杀死敌人获得经验增加
	
	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	CalcCurLucky();
	
	m_cFaction.SetSeries(Series);
	
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：计算当前力量
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurStrength()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurStrength = m_nStrength;
//}

//-------------------------------------------------------------------------
//	功能：计算当前敏捷
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurDexterity()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurDexterity = m_nDexterity;
//}

//-------------------------------------------------------------------------
//	功能：计算当前活力
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurVitality()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurVitality = m_nVitality;
//}

//-------------------------------------------------------------------------
//	功能：计算当前精力
//-------------------------------------------------------------------------
//void	KPlayer::CalcCurEngergy()
//{	// 还需要考虑 装备、技能、状态 的影响
//	m_nCurEngergy = m_nEngergy;
//}

//-------------------------------------------------------------------------
//	功能：计算当前运气
//-------------------------------------------------------------------------
void	KPlayer::CalcCurLucky()
{	// 还需要考虑 装备、技能、状态 的影响
	m_nCurLucky = m_nLucky;
}

//-------------------------------------------------------------------------
//	功能：改变当前力量(当 nData 小于 0 时，减少)
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurStrength(int nData)
{

	m_nCurStrength +=nData;

	SetNpcPhysicsDamage();  //物理攻击增加
	// 把当前力量传给客户端
}

//-------------------------------------------------------------------------
//	功能：改变当前敏捷(当 nData 小于 0 时，减少)
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurDexterity(int nData)
{
/*	if (m_nCurDexterity + nData < 0)
return;
	*/
	m_nCurDexterity += nData;
	
	int nRating = nData * 4;
	Npc[m_nIndex].m_CurrentAttackRating += nRating;
	
	int nDefence = nData / 4;
	Npc[m_nIndex].m_CurrentDefend += nDefence;
	
	SetNpcPhysicsDamage();
	
}


//-------------------------------------------------------------------------
//	功能：改变当前活力(当 nData 小于 0 时，减少)  、//增加当前生命 和内力值
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurVitality(int nData)	    //状态
{
/*	if (nData + m_nAttributePoint < 0)
return;
	*/
	m_nCurVitality += nData;  //外功增加多少点
	
	Npc[m_nIndex].AddCurLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
	Npc[m_nIndex].AddCurStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);	//体力
	
}

//-------------------------------------------------------------------------
//	功能：改变当前精力(当 nData 小于 0 时，减少) 内功
//-------------------------------------------------------------------------
void	KPlayer::ChangeCurEngergy(int nData)
{
/*	if (m_nAttributePoint + nData < 0)
return;
	*/
	m_nCurEngergy += nData;
	
	Npc[m_nIndex].AddCurManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);

	SetNpcPhysicsDamage();
}

//-------------------------------------------------------------------------
//	功能：增加基本运气
//-------------------------------------------------------------------------
void	KPlayer::AddBaseLucky(int nData)
{
	m_nLucky += nData;
	CalcCurLucky();
}

//-------------------------------------------------------------------------
//	功能：由当前力量计算对应npc的物理伤害(PhysicsDamage)---根据武器的伤害为基点---换装是调用
//-------------------------------------------------------------------------
void	KPlayer::SetNpcPhysicsDamage()
{
	int		nMinDamage=0, nMaxDamage=0,mLeftskillidex=0;

	        m_ItemList.GetWeaponDamage(&nMinDamage, &nMaxDamage);  //获取武器的基本伤害

	        mLeftskillidex=GetLeftSkill();                         //左键技能

		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(mLeftskillidex,1);

		if (!pOrdinSkill) 
            return;

	if (m_ItemList.GetWeaponType() == equip_meleeweapon)	
	{// (当前装备是近身武器)（力量）
	  /*nMinDamage = nMinDamage * (m_nCurStrength + STRENGTH_SET_DAMAGE_VALUE) / STRENGTH_SET_DAMAGE_VALUE;
	    nMaxDamage = nMaxDamage * (m_nCurStrength + STRENGTH_SET_DAMAGE_VALUE) / STRENGTH_SET_DAMAGE_VALUE;
	  */
		if (Npc[m_nIndex].m_Series==0)
		{//金系
		   nMinDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
		   nMaxDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
		}
		else if (Npc[m_nIndex].m_Series==1) 
		{//木系
			if (m_ItemList.GetWeaponParticular()==1 && !pOrdinSkill->IsMagic())
			{//如果武器是单刀的话 就是加力量
			   
			   nMinDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
		       nMaxDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
			}
			else
			{//加内功 级加攻击
				nMinDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
		        nMaxDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
			}
		}
		else if (Npc[m_nIndex].m_Series==2)
		{//水系
			if ((m_ItemList.GetWeaponParticular()==1 || m_ItemList.GetWeaponParticular()==0) && !pOrdinSkill->IsMagic())
			{//如果武器是单刀 和 剑的话 就是加力量
				nMinDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
			}
			else
			{//加内功 级加攻击
				nMinDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
			}
		}
		else if (Npc[m_nIndex].m_Series==3)
		{//火系
			if ((m_ItemList.GetWeaponParticular()==2 || m_ItemList.GetWeaponParticular()==3) && !pOrdinSkill->IsMagic())
			{//如果武器是棍棒 和 枪的话 就是加力量
				nMinDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
			}
			else
			{//加内功 级加攻击
				nMinDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
			}
		}
		else if (Npc[m_nIndex].m_Series==4)
		{//土系
			if ((m_ItemList.GetWeaponParticular()==0 || m_ItemList.GetWeaponParticular()==1) && !pOrdinSkill->IsMagic())
			{//如果武器是剑 和 单刀的话 就是加力量
				nMinDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurStrength/STRENGTH_SET_DAMAGE_VALUE;
			}
			else
			{//加内功 级加攻击
				nMinDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
				nMaxDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
			}
		}

		Npc[m_nIndex].SetPhysicsDamage(nMinDamage,nMaxDamage);
	}
	else if (m_ItemList.GetWeaponType() == equip_rangeweapon)	
	{// (当前装备是远程武器)暗器(身法)
	     /*nMinDamage = nMinDamage * (m_nCurDexterity + DEXTERITY_SET_DAMAGE_VALUE) / DEXTERITY_SET_DAMAGE_VALUE;
	       nMaxDamage = nMaxDamage * (m_nCurDexterity + DEXTERITY_SET_DAMAGE_VALUE) / DEXTERITY_SET_DAMAGE_VALUE;
		 */
		if (!pOrdinSkill->IsMagic())
		{//如果是外功
			nMinDamage += m_nCurDexterity/DEXTERITY_SET_DAMAGE_VALUE;
			nMaxDamage += m_nCurDexterity/DEXTERITY_SET_DAMAGE_VALUE;
		}
		else
		{//内功加内功
			nMinDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
			nMaxDamage += m_nCurEngergy/STRENGTH_SET_DAMAGE_VALUE;
		}

		Npc[m_nIndex].SetPhysicsDamage(nMinDamage,nMaxDamage);
	
	}
	else
	{//空手 
		Npc[m_nIndex].SetPhysicsDamage(nMinDamage,nMaxDamage);
	}
}

//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的攻击命中率(AttackRating)
//-------------------------------------------------------------------------
void	KPlayer::SetNpcAttackRating()
{
	int		nRating;
	
	nRating = m_nDexterity * 4 - 28;
	Npc[m_nIndex].SetBaseAttackRating(nRating);
}

//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的防御力
//-------------------------------------------------------------------------
void	KPlayer::SetNpcDefence()
{
	int		nDefence;
	
	nDefence = m_nDexterity / 4;
	Npc[m_nIndex].SetBaseDefence(nDefence);
}

/*
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的行走速度
//-------------------------------------------------------------------------
void	KPlayer::SetNpcWalkSpeed()
{
int		nSpeed;
nSpeed = BASE_WALK_SPEED * (m_nCurDexterity + 320) / 320;
Npc[m_nIndex].SetBaseWalkSpeed(nSpeed);
}
#endif
*/

/*
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：由当前敏捷计算对应npc的跑步速度
//-------------------------------------------------------------------------
void	KPlayer::SetNpcRunSpeed()
{
int		nSpeed;
nSpeed = BASE_RUN_SPEED * (m_nCurDexterity + 320) / 320;
Npc[m_nIndex].SetBaseRunSpeed(nSpeed);
}
#endif
*/

//-------------------------------------------------------------------------
//	功能：直接增加经验值，不考虑其他因素
//-------------------------------------------------------------------------
void	KPlayer::DirectAddExp(int nExp)
{

	if (nExp == 0 || Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;
	
    int nMaxLevel=1000;
       g_GameSetTing.GetInteger("ServerConfig","PlayerMaxLevel",1000,&nMaxLevel);

     if (Npc[m_nIndex].m_Level>=nMaxLevel)
		 return;

	m_nExp += nExp;
 
}

//-------------------------------------------------------------------------
//	功能：玩家升一级
//-------------------------------------------------------------------------
void	KPlayer::LevelUp()
{
	m_nExp = 0;

	if (Npc[m_nIndex].m_Level >= MAX_LEVEL)
		return;

	Npc[m_nIndex].m_Level++;

	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	// -------------------------- 等级增加时各项属性增加 ----------------------
	// 增加未分配属性点
	m_nAttributePoint += PLAYER_LEVEL_ADD_ATTRIBUTE;   //一次增加5点属性
	// 增加未分配技能点
	m_nSkillPoint     += PLAYER_LEVEL_ADD_SKILL;       //一级增加1点技能点
	
	// 处理生命、体力、内力值（影响基本生命、体力、内力最大值的因素：等级、属性点，影响当前生命最大值的因素：装备、技能、药物（临时））
	LevelAddBaseLifeMax();
	LevelAddBaseStaminaMax();
	LevelAddBaseManaMax();
	// Npc[m_nIndex].ResetLifeReplenish();
	
	// 处理各种抗性的变化 火、冰、毒、电、物理  原始
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;

	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
/*
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
*/
	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// 根据装备、技能信息更新当前数据
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp = nOldCurCamp;
	
	// 生命、体力、内力值加满
	Npc[m_nIndex].m_CurrentLife = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana = Npc[m_nIndex].m_CurrentManaMax;
}

// 同步客户端属性状态技能等数据
void	KPlayer::UpdataCurData(int nIsLoign,int nUnEquip) //更新自身的属性参数
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	for (int j = 0 ; j< MAX_NPCSKILL;++j)
	{
		int nAdd = 	Npc[m_nIndex].m_SkillList.GetAddPoint(j); //获取该位置的额外技能点
		if (nAdd > 0)
		{
			nAdd = -nAdd;  //等于负的
			Npc[m_nIndex].m_SkillList.QeuipAddPoint(j,nAdd); //设置该位置技能点为实际技能点
		}

		Npc[m_nIndex].m_SkillList.m_Skills[j].mAddPoint = 0;
		Npc[m_nIndex].m_SkillList.m_Skills[j].EnChance = 0;  // 技能的加成
		Npc[m_nIndex].m_SkillList.m_Skills[j].nTempEnChance = 0;  // 技能的加成
	}

	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageP, sizeof(KMagicAttrib));
	ZeroMemory(&Npc[m_nIndex].m_CurrentPhysicsMagicDamageV, sizeof(KMagicAttrib));

	int nCurbei=0; //,nFomoidx=Npc[m_nIndex].GetCurFuMoIdx()
	
	/*if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			nCurbei=Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
		}
	} */

	Npc[m_nIndex].m_CurrentLifeMax		= Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//;Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_LifeMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentManaMax		= Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornLifeMaxVal()+nCurbei*50)/100;
	//Npc[m_nIndex].TakeTrader(Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
	//Npc[m_nIndex].m_ManaMax*(100+m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
	Npc[m_nIndex].m_CurrentStaminaMax	= Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentAttackRating	= Npc[m_nIndex].m_AttackRating;
	Npc[m_nIndex].m_CurrentAttackSpeed	= Npc[m_nIndex].m_AttackSpeed;
	Npc[m_nIndex].m_CurrentCastSpeed	= Npc[m_nIndex].m_CastSpeed;
    Npc[m_nIndex].m_CurrentRunSpeed	    = Npc[m_nIndex].m_RunSpeed; 
	Npc[m_nIndex].m_CurrentJumpSpeed	= Npc[m_nIndex].m_JumpSpeed;
	Npc[m_nIndex].m_CurrentVisionRadius	= Npc[m_nIndex].m_VisionRadius;
	Npc[m_nIndex].m_CurrentWalkSpeed	= Npc[m_nIndex].m_WalkSpeed;
	Npc[m_nIndex].m_CurrentNuQiMax		= Npc[m_nIndex].m_NuqiMax;//怒气
	m_nCurStrength						= m_nStrength;
	m_nCurDexterity						= m_nDexterity;
	m_nCurVitality						= m_nVitality;
	m_nCurEngergy						= m_nEngergy;
	m_nCurLucky							= m_nLucky;

	m_nTempLucky_p                      = 0;

	Npc[m_nIndex].m_TempFireResist = 0;	        // Npc的当前火抗性
	Npc[m_nIndex].m_TempColdResist = 0;	        // Npc的当前冰抗性
	Npc[m_nIndex].m_TempPoisonResist = 0;	    // Npc的当前毒抗性
	Npc[m_nIndex].m_TempLightResist = 0;	    // Npc的当前电抗性
	Npc[m_nIndex].m_TempPhysicsResist = 0;	    // Npc的当前物理抗性
	Npc[m_nIndex].m_CurrentHitNpcRecover = 0;
	Npc[m_nIndex].m_CurrentHitRank       = 0;

	Npc[m_nIndex].m_CurrentColdResist	= Npc[m_nIndex].m_ColdResist;  //原始的防御
	Npc[m_nIndex].m_CurrentDefend		= Npc[m_nIndex].m_Defend;

	Npc[m_nIndex].m_CurrentFireResist	= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_CurrentLifeReplenish= Npc[m_nIndex].m_LifeReplenish;
	Npc[m_nIndex].m_CurrentLifeReplenish_p	= 0;
	Npc[m_nIndex].m_CurrentManaReplenish_p  = 0;
	Npc[m_nIndex].m_CurrentHitRecover	= Npc[m_nIndex].m_HitRecover;  //Npc的受伤动作时间
	Npc[m_nIndex].m_CurrentLightResist	= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_CurrentManaReplenish= Npc[m_nIndex].m_ManaReplenish;
	Npc[m_nIndex].m_CurrentPhysicsResist= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_CurrentPoisonResist	= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_CurrentStaminaGain	= Npc[m_nIndex].m_StaminaGain;
	Npc[m_nIndex].m_CurrentStaminaLoss	= Npc[m_nIndex].m_StaminaLoss;

	//SetBaseSpeedAndRadius();
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicColdDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&Npc[m_nIndex].m_CurrentColdDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentColdEnhance	= 0;

	Npc[m_nIndex].m_CurrentColdResistMax	= BASE_FANGYU_ALL_MAX+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentFireResistMax    = BASE_FANGYU_ALL_MAX+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornFanYuMaxVal();
	Npc[m_nIndex].m_CurrentLightResistMax   = BASE_FANGYU_ALL_MAX+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= BASE_FANGYU_ALL_MAX+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_PhysicsResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= BASE_FANGYU_ALL_MAX+m_cReBorn.GetReBornNum()*m_cReBorn.GetReBornFanYuMaxVal();//Npc[m_nIndex].m_PoisonResistMax;

	Npc[m_nIndex].m_CurrentDamage2Mana	= 0;
	Npc[m_nIndex].m_CurrentDamageReduce	= 0;
	Npc[m_nIndex].m_CurrentDeadlyStrike	= 0;
//	Npc[m_nIndex].m_CurrentElementDamageReduce = 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicFireDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&Npc[m_nIndex].m_CurrentFireDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentFireEnhance	= 0;
	
	Npc[m_nIndex].m_CurrentHandEnhance	= 0;
	Npc[m_nIndex].m_CurrentKnockBack	= 0;
	Npc[m_nIndex].m_CurrentLifeStolen	= 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicLightDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&Npc[m_nIndex].m_CurrentLightDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&Npc[m_nIndex].m_WaiPhysicsDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentLightEnhance	= 0;
	Npc[m_nIndex].m_CurrentPoisonTime=0;

//	Npc[m_nIndex].m_CurrentManaPerEnemy	= 0;
	Npc[m_nIndex].m_CurrentManaStolen	= 0;
	Npc[m_nIndex].m_CurrentMeleeDmgRet	= 0;        // 近程反弹
	Npc[m_nIndex].m_CurrentHulueMeleeDmgRet=0;      // 忽略近程反弹多少点、
   	Npc[m_nIndex].m_CurrentHulueRangeDmgRet=0;      // 忽略远程程反弹多少点、
	Npc[m_nIndex].m_CurrentMeleeDmgRetPercent = 0;  // 近程反弹百分比
	Npc[m_nIndex].m_CurrentTempSpeed=0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentMeleeEnhance, sizeof(Npc[m_nIndex].m_CurrentMeleeEnhance));

//	Npc[m_nIndex].m_CurrentPiercePercent	= 0;
	ZeroMemory(&Npc[m_nIndex].m_CurrentMagicPoisonDamage, sizeof(KMagicAttrib));
//	ZeroMemory(&Npc[m_nIndex].m_CurrentPoisonDamage, sizeof(KMagicAttrib));
	Npc[m_nIndex].m_CurrentPoisonEnhance	= 0;

	Npc[m_nIndex].m_CurrentRangeDmgRet	= 0;         //远程反弹
	Npc[m_nIndex].m_CurrentRangeDmgRetPercent	= 0; //远程反弹百分比
	Npc[m_nIndex].m_CurrentRangeEnhance	= 0;
	Npc[m_nIndex].m_CurrentSlowMissle	= 0;         //子弹减速
	Npc[m_nIndex].m_CurrentStaminaStolen	= 0;
	Npc[m_nIndex].m_CurrentAddPhysicsDamage = 0;      //外功普点
	m_nUpExp							= 0;
	Npc[m_nIndex].m_CurrentUpExp        = 0;
	Npc[m_nIndex].m_CurrentFreezeTimeReducePercent = 0;
	Npc[m_nIndex].m_CurrentPoisonTimeReducePercent = 0;
	Npc[m_nIndex].m_EnemyPoisonTimeReducePercent   = 0;
	Npc[m_nIndex].m_CurrentStunTimeReducePercent = 0;
	Npc[m_nIndex].m_EnemyStunTimeReducePercent   = 0;
	Npc[m_nIndex].m_CurrentSerisesEnhance = 0;
	Npc[m_nIndex].m_CurrentDamageReduce = 0;
	Npc[m_nIndex].m_CurrentdanggeRate = 0;  //档格
	Npc[m_nIndex].m_CurrentzhongjiRate = 0; //重击
	 Npc[m_nIndex].m_CurrentcjdanggeRate	= 0;               //拆解档格
	 Npc[m_nIndex].m_CurrentcjzhongjiRate	= 0;               //拆解重击
	 Npc[m_nIndex].m_Currentsorbdamage	= 0;                   //抵消伤害
	 Npc[m_nIndex].m_Currentsorbdamage_v=0;
	 Npc[m_nIndex].m_Currenadddamagev   = 0;
	 Npc[m_nIndex].m_Currenadddamagep   = 0;
	 Npc[m_nIndex].m_Currentpoisonres	= 0;                   //=忽略对方毒防:#d1-%
	 Npc[m_nIndex].m_Currentfireres	= 0;                       //=忽略对方火防:#d1-%
	 Npc[m_nIndex].m_Currentlightingres	= 0;                   //=忽略对方雷防:#d1-%
	 Npc[m_nIndex].m_Currentphysicsres	= 0;                   //=忽略对方普防:#d1-%
	 Npc[m_nIndex].m_Currentcoldres	= 0;                       //=忽略对方冰防:#d1-%
	 Npc[m_nIndex].m_Currentallres	= 0; 
	 Npc[m_nIndex].m_Currentnopkvalue	= 0;                   //=不增加PK值概率:#d1+%
	 Npc[m_nIndex].m_Currentbossdamage	= 0;                   //=对黄金boss攻击伤害<color=orange>#d1+%<color>
	 Npc[m_nIndex].m_Currentelementsenhance	= 0;               //=五行强化值：#d1-点。强化对相克五行的克制效果
     Npc[m_nIndex].m_Currentelementsresist	= 0;               //=五行弱化值：#d1-点。弱化受相克五行的克制效果
	 Npc[m_nIndex].m_Currentskillenhance=0;                    //技能的加成
	  //ZeroMemory(Npc[m_nIndex].m_CurrentSkillEnhance,sizeof(Npc[m_nIndex].m_CurrentSkillEnhance));

	 //std::_EnhanceInfo::iterator it;
	 /*typedef std::map<INT, CEnhanceInfo>::iterator it;
	 for( it = Npc[m_nIndex].nEnhanceInfo.begin(); it != Npc[m_nIndex].nEnhanceInfo.end(); ++it)
	 {
		 it->second.nSkillIdx = 0;
		 it->second.nEnhance = 0;
	 }*/
	 Npc[m_nIndex].nEnhanceInfo.clear();
	 Npc[m_nIndex].m_CurrentFullManaskillenhance=0;
	 Npc[m_nIndex].m_CurrentautoReviverate=0;                  //复活概率
	 Npc[m_nIndex].m_CurrentAddPhysicsDamageP= 0;     // 当前被动外普百分比
	 Npc[m_nIndex].m_CurrentAddFireDamagev= 0;        // 当前被动外火点
	 Npc[m_nIndex].m_CurrentAddColdDamagev= 0;        // 当前被动外冰点 
	 Npc[m_nIndex].m_CurrentAddLighDamagev= 0;        // 当前被动外雷点
	 Npc[m_nIndex].m_CurrentAddPoisonDamagev= 0;      // 当前被动外毒点
	 Npc[m_nIndex].m_CurrentAddmagicphysicsDamage= 0; // 当前被动内普点
	 Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP= 0;// 当前被动内普百分比
	 Npc[m_nIndex].m_CurrentAddmagicColdDamagicv= 0;  // 当前被动内冰点
	 Npc[m_nIndex].m_CurrentAddmagicFireDamagicv= 0;  // 当前被动内火点  
	 Npc[m_nIndex].m_CurrentAddmagicLightDamagicv= 0; // 当前被动内雷点
     Npc[m_nIndex].m_CurrentAddmagicPoisonDamagicv= 0;// 当前被动内毒点
	 Npc[m_nIndex].m_CurrentPoisondamagereturnV=0;
	 Npc[m_nIndex].m_CurrentPoisondamagereturnP=0;
	 Npc[m_nIndex].m_Currentbaopoisondmax_p=0;
	 Npc[m_nIndex].m_CurrentReturnskillp=0;
	 Npc[m_nIndex].m_CurrentIgnoreskillp=0;
	 Npc[m_nIndex].m_CurrentReturnresp=0;
	 Npc[m_nIndex].m_CurrentCreatnpcv=0;
	 Npc[m_nIndex].m_CurrentAllJiHuo=0; //是否全身激活
	 Npc[m_nIndex].m_CurrentCreatStatus=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancep=0;
	 Npc[m_nIndex].m_CurrentAttackRatingEnhancev=0;
	 Npc[m_nIndex].m_CurrentIgnorenAttacRating  =0;
	 Npc[m_nIndex].m_Me2metaldamage_p=0;              //=对金系伤害增加：#d1+%
	 Npc[m_nIndex].m_Metal2medamage_p=0;              //=减少来自金系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2wooddamage_p=0;              //=对木系伤害增加：#d1+%
	 Npc[m_nIndex].m_Wood2medamage_p=0;              //=减少来自木系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2waterdamage_p=0;              //=对水系伤害增加：#d1+%
	 Npc[m_nIndex].m_Water2medamage_p=0;              //=减少来自水系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2firedamage_p=0;              //=对火系伤害增加：#d1+%
	 Npc[m_nIndex].m_Fire2medamage_p=0;              //=减少来自火系的伤害：#d1-%
	 Npc[m_nIndex].m_Me2earthdamage_p=0;              //=对土系伤害增加：#d1+%
	 Npc[m_nIndex].m_Earth2medamage_p=0;              //=减少来自土系的伤害：#d1-%
     Npc[m_nIndex].m_CurrentStunRank_p=0;
	 Npc[m_nIndex].m_Staticmagicshield_p=0;
	 ReCalcEquip();         //重新计算身上的装备
	 ReCalcState(nIsLoign); //重新计算技能的状态
}

void KPlayer::ReCalcEquip()
{
	//CCAssert(m_nIndex > 0 && m_nIndex < MAX_NPC,"");
	if (m_nIndex <=0 || m_nIndex >= MAX_NPC)
		return;
	
	for (int i = 0; i < itempart_num; ++i)
	{
		int nIdx = m_ItemList.m_EquipItem[i];
		if (!nIdx)
			continue;

		if (Item[nIdx].GetIsCanUse()==0)  //不可用的
		    continue;

		int nActive = m_ItemList.GetEquipEnhance(i);  //获取激活的数量

		if (m_ItemList.m_bActiveSet)  //如果是黄金套装激活的 或者 全身激活
		{
			nActive = 3;              //默认的黄金明属性
		}
//除马匹
		if (i != itempart_horse || Npc[m_nIndex].m_bRideHorse)  //除掉马的部位 或 骑着马
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
			   Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}
			else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
			{//面具
               Item[nIdx].ApplyMagicAttribToNPC(&Npc[m_nIndex], nActive,nActiveInfo);
			}

		}
	}
}

void KPlayer::ReCalcState(int nIsLoign)
{
	if (m_nIndex <=0 || m_nIndex >= MAX_NPC)
		return;
	
	Npc[m_nIndex].ReCalcStateEffect();        //重置状态效果

	Npc[m_nIndex].m_SkillList.ReEnChance();   //重置加成
	
	if (!nIsLoign)
	   Npc[m_nIndex].NpcNewInitJinMaiVal();   //重置经脉状态  
	       
	Npc[m_nIndex].ReSkillEnhance(m_nLeftSkillID,1); //重置对攻击技能的加成!
}

//-------------------------------------------------------------------------
//	功能：增加统率力经验
//-------------------------------------------------------------------------
void	KPlayer::AddLeadExp(int nExp)
{
	if (m_dwLeadLevel >= MAX_LEAD_LEVEL)
		return;
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
		return;
	m_dwLeadExp += nExp;
	if (m_dwLeadExp >= (DWORD)PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel))
	{
		// 升级
		m_dwLeadLevel = PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp, m_dwLeadLevel);
		m_dwNextLevelLeadExp = PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
	}	
}

//-------------------------------------------------------------------------
//	功能：原始玩家升一级加生命
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseLifeMax()
{
	Npc[m_nIndex].m_LifeMax += PlayerSet.m_cLevelAdd.GetLifePerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentLifeMax = Npc[m_nIndex].m_LifeMax;
}

//-------------------------------------------------------------------------
//	功能：玩家升一级加体力
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseStaminaMax()
{
	Npc[m_nIndex].m_StaminaMax += PlayerSet.m_cLevelAdd.GetStaminaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
}

//-------------------------------------------------------------------------
//	功能：玩家升一级加内力
//-------------------------------------------------------------------------
void KPlayer::LevelAddBaseManaMax()
{
	Npc[m_nIndex].m_ManaMax += PlayerSet.m_cLevelAdd.GetManaPerLevel(Npc[m_nIndex].m_Series);
	Npc[m_nIndex].m_CurrentManaMax = Npc[m_nIndex].m_ManaMax;
}

//-------------------------------------------------------------------------
//	功能：改变玩家阵营
//-------------------------------------------------------------------------
void	KPlayer::ChangePlayerCamp(int nCamp)
{
	if (nCamp < camp_begin || nCamp >= camp_num)
		return;
	Npc[m_nIndex].m_Camp = (NPCCAMP)nCamp;
}

//-------------------------------------------------------------------------
//	功能：向服务器申请查询某个npc所在队伍的信息
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamInfo(DWORD dwNpcID)
{
	if (dwNpcID <= 0)
		return;
	PLAYER_APPLY_TEAM_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_teamapplyinfo;
	sApplyInfo.m_dwTarNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(PLAYER_APPLY_TEAM_INFO));
}

//-------------------------------------------------------------------------
//	功能：向服务器报告包袱不足了
//-------------------------------------------------------------------------
void	KPlayer::ApplyRoomInfo(int nIsFull)
{
	if (nIsFull < 0)
		return;
	PLAYER_NO_ROOM_INFO	sApplyInfo;
	sApplyInfo.ProtocolType = (BYTE)c2s_roomisfull;
	sApplyInfo.m_IsNoRoom = nIsFull;
	m_nRoomFull           = nIsFull;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApplyInfo, sizeof(PLAYER_NO_ROOM_INFO));
}


//-------------------------------------------------------------------------
//	功能：向服务器申请查询玩家自身的队伍情况
//-------------------------------------------------------------------------
void	KPlayer::ApplySelfTeamInfo()
{
	ApplyTeamInfo(Npc[m_nIndex].m_dwID);
}



//-------------------------------------------------------------------------
//	功能：玩家向服务器申请创建队伍
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyCreateTeam()//char *lpszTeamName)
{
	return m_cTeam.ApplyCreate();//lpszTeamName);
}

//-------------------------------------------------------------------------
//	功能：队长向服务器申请开放、关闭队伍是否允许加入成员状态
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyTeamOpenClose(BOOL bFlag)
{
	if (!m_cTeam.m_nFlag )					// 未组队
		return FALSE;
	if (m_cTeam.m_nFigure != TEAM_CAPTAIN)	// 不是队长
		return FALSE;
	if (this->CheckTrading())	// 处在交易状态
		return FALSE;
	
	if (bFlag)
	{
		if (PlayerSet.m_cLeadExp.GetMemNumFromLevel(m_dwLeadLevel) <= g_Team[0].m_nMemNum)	// 统帅力不够
			return FALSE;
	}
	
	PLAYER_TEAM_OPEN_CLOSE	sTeamState;
	sTeamState.ProtocolType = c2s_teamapplyopenclose;
	sTeamState.m_btOpenClose = bFlag;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamState, sizeof(PLAYER_TEAM_OPEN_CLOSE));
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：玩家向服务器申请加入某个队伍
//	参数：int nNpcIndex 目标队伍队长在客户端Npc数组中的位置
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddTeam(int nNpcIndex)
{
	if (this->CheckTrading())
		return;

	if (m_cTeam.m_nFlag)					// 已经组队，不能申请加入别的队伍
		return;
	
	// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
	if (Npc[nNpcIndex].m_Camp == camp_begin && Npc[m_nIndex].m_Camp != camp_begin)
		return;
	//	if (Npc[nNpcIndex].m_Camp != Npc[m_nIndex].m_Camp)	// 阵营不同
	//		return;
	
	// 判断目标队伍队长是不是处在队伍开放状态
	if (Npc[nNpcIndex].GetMenuState() != PLAYER_MENU_STATE_TEAMOPEN)
	{
		m_ItemList.ClientShowMsg("对方队伍处于关闭状态!");
		return;
	}
	
	m_cTeam.m_nApplyCaptainID = Npc[nNpcIndex].m_dwID;
	m_cTeam.m_dwApplyTimer = MAX_APPLY_TEAM_TIME;
	PLAYER_APPLY_ADD_TEAM	sAddTeam;
	sAddTeam.ProtocolType = c2s_teamapplyadd;
	sAddTeam.m_dwTarNpcID = Npc[nNpcIndex].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAddTeam, sizeof(PLAYER_APPLY_ADD_TEAM));
	
	KSystemMessage	sMsg;
	t_sprintf(sMsg.szMessage,strCoreInfo[MSG_TEAM_APPLY_ADD_SELF_MSG].c_str(), Npc[nNpcIndex].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//-------------------------------------------------------------------------
//	功能：玩家通知服务器接受某个npc为队伍成员
//-------------------------------------------------------------------------
void	KPlayer::AcceptTeamMember(DWORD dwNpcID)
{
	// 判断状态 （缺少对npc队伍开放状态的检测 not end）
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		//CCMessageBox("不是队长","error");
		ApplySelfTeamInfo();
		return;
	}
	if (dwNpcID == 0)
	{
		// CCMessageBox("dwNpcID = 0","同意入队");
		return;
	}
	
	// 查找申请列表
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.UpdateInterface();
			break;
		}
	}
	// 如果没找到
//	if (i >= MAX_TEAM_APPLY_LIST)
//		return;
	//char msg[64];
	/*int  nindex = NpcSet.SearchID(dwNpcID);
	if  (nindex>0)
	   CCMessageBox(Npc[nindex].Name,"同意入队");*/
	// 向服务器发申请
	PLAYER_ACCEPT_TEAM_MEMBER	sAcceptTeam;
	sAcceptTeam.ProtocolType = c2s_teamacceptmember;
	sAcceptTeam.m_dwNpcID    = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAcceptTeam, sizeof(PLAYER_ACCEPT_TEAM_MEMBER));
}

//-------------------------------------------------------------------------
//	功能：队长删除加入队伍申请列表中的某个npc
//-------------------------------------------------------------------------
void	KPlayer::TeamDropApplyOne(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return;
	// 查找申请列表
	int		i;
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
			m_cTeam.m_sApplyList[i].m_dwTimer = 0;
			m_cTeam.m_sApplyList[i].m_nLevel = 0;
			m_cTeam.m_sApplyList[i].m_szName[0] = 0;
			break;
		}
	}
}

//-------------------------------------------------------------------------
//	功能：通知服务器本玩家离开队伍
//-------------------------------------------------------------------------
void	KPlayer::LeaveTeam()
{
	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	if (g_pClient)
		g_pClient->SendPackToServer(&sLeaveTeam, sizeof(PLAYER_APPLY_LEAVE_TEAM));
}

//-------------------------------------------------------------------------
//	功能：队长通知服务器踢除某个队员
//-------------------------------------------------------------------------
void	KPlayer::TeamKickMember(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_TEAM_KICK_MEMBER	sKickOne;
	sKickOne.ProtocolType = c2s_teamapplykickmember;
	sKickOne.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sKickOne, sizeof(PLAYER_TEAM_KICK_MEMBER));
}

//-------------------------------------------------------------------------
//	功能：队长向服务器申请把自己的队长身份交给别的队员
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamChangeCaptain(DWORD dwNpcID)
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN	sTeamChange;
	sTeamChange.ProtocolType = c2s_teamapplychangecaptain;
	sTeamChange.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamChange, sizeof(PLAYER_APPLY_TEAM_CHANGE_CAPTAIN));
}

//-------------------------------------------------------------------------
//	功能：队长向服务器申请解散队伍
//-------------------------------------------------------------------------
void	KPlayer::ApplyTeamDismiss()
{
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
		return;
	
	PLAYER_APPLY_TEAM_DISMISS	sTeamDismiss;
	sTeamDismiss.ProtocolType = c2s_teamapplydismiss;
	if (g_pClient)
		g_pClient->SendPackToServer(&sTeamDismiss, sizeof(PLAYER_APPLY_TEAM_DISMISS));
}

//-------------------------------------------------------------------------
//	功能：玩家向服务器申请打开、关闭pk开关
//-------------------------------------------------------------------------
void	KPlayer::ApplySetPK(BOOL bPK)
{
	PLAYER_SET_PK	sPK;
	sPK.ProtocolType = c2s_playerapplysetpk;
	sPK.m_btPKFlag = bPK;
	if (g_pClient)
		g_pClient->SendPackToServer(&sPK, sizeof(PLAYER_SET_PK));
}

//-------------------------------------------------------------------------
//	功能：玩家向服务器申请门派数据
//-------------------------------------------------------------------------
void	KPlayer::ApplyFactionData()
{
	PLAYER_APPLY_FACTION_DATA	sFaction;
	sFaction.ProtocolType = c2s_playerapplyfactiondata;
	if (g_pClient)
		g_pClient->SendPackToServer(&sFaction, sizeof(PLAYER_APPLY_FACTION_DATA));
}

//-------------------------------------------------------------------------
//	功能：客户端发送聊天语句给服务器
//-------------------------------------------------------------------------
void	KPlayer::SendChat(KUiMsgParam *pMsg, char *lpszSentence)
{
	if (!lpszSentence || !pMsg || pMsg->nMsgLength <= 0)
		return;
	int nLength = pMsg->nMsgLength;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	
	if (pMsg->eGenre == MSG_G_CHAT)
	{
		Npc[m_nIndex].SetChatInfo(pMsg->szName, lpszSentence, nLength);  //处理自己聊天内容 赋值给 m_szChatBuffer
		//char m_szChatBuffer[256]={0};
		//Npc[m_nIndex].GetstrInfo(STR_CHATBUFF_CLIENT,m_szChatBuffer);
//		m_cChat.SendSentence(pMsg,Npc[m_nIndex].m_szChatBuffer);        //发送给服务器
	}
	else if (pMsg->eGenre == MSG_G_CMD)	// not end
	{

	}
}

//-------------------------------------------------------------------------
//	功能：队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
//-------------------------------------------------------------------------
void	KPlayer::ApplyAddBaseAttribute(int nAttribute, int nNo)
{
	if (nAttribute < 0 || nAttribute > 3)
		return;
	if (nNo <= 0 || nNo >= 255)
		return;
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_playeraddbaseattribute;
	sAdd.m_btAttribute = nAttribute;
	sAdd.m_nAddNo = nNo;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(PLAYER_ADD_BASE_ATTRIBUTE_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：向服务器申请某个技能升级
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyAddSkillLevel(int nSkillID, int nAddPoint)
{
	if ( !Npc[m_nIndex].m_SkillList.FindSame(nSkillID)) //查找是否有这个技能
		return FALSE;
	if (m_nSkillPoint < nAddPoint || nAddPoint + Npc[m_nIndex].m_SkillList.GetLevel(nSkillID) >=MAX_SKILLLEVEL)
		return FALSE;
	
	PLAYER_ADD_SKILL_POINT_COMMAND	sSkill;
	sSkill.ProtocolType = c2s_playerapplyaddskillpoint;
	sSkill.m_nSkillID = nSkillID;
	sSkill.m_nAddPoint= nAddPoint; //增加的技能点
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sSkill, sizeof(PLAYER_ADD_SKILL_POINT_COMMAND));
	
	return TRUE;
}
//自动储存交换容器,不经过手
BOOL	KPlayer::AutoChangeItem(int nItemID, ItemPos SrcPos,ItemPos DestPos)
{
	return TRUE;
}
//-------------------------------------------------------------------------
//	功能：向服务器申请使用某个物品（鼠标右键点击该物品，只能用于吃药）
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyUseItem(int nItemID, ItemPos SrcPos)
{
	if (this->CheckTrading())
		return FALSE;

	int nRet = m_ItemList.UseItem(nItemID,SrcPos.nX,SrcPos.nY);  //同意使用物品
	if (nRet == 0)
		return FALSE;
	
	if (nRet == REQUEST_EQUIP_ITEM)         //物品装备---右键换装-设置
	{//等于装备
	}
	else if (nRet == REQUEST_EAT_MEDICINE)  // 药品
	{
		PLAYER_EAT_ITEM_COMMAND	sEat;
		sEat.ProtocolType = c2s_playereatitem;
		sEat.m_nItemID    = nItemID;
		sEat.m_btPlace    = SrcPos.nPlace;
		sEat.m_btX        = SrcPos.nX;
		sEat.m_btY        = SrcPos.nY;
		//CCMessageBox("使用物品","提示:");
		if (g_pClient)
			g_pClient->SendPackToServer(&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));
	}
	
	return TRUE;
}
//客户端要求移动物品
BOOL KPlayer::ApplyAutoMoveItem(int nItemID, ItemPos SrcPos,int destRoom)
{
	   //if (this->CheckTrading())
	  //	return FALSE;

		PLAYER_AUTOMUVE_ITEM_COMMAND	sMoveItem;
		sMoveItem.ProtocolType = c2s_playerAutoMoveitem;
		sMoveItem.m_nItemID    = Item[nItemID].GetID();//nItemdwid;
		sMoveItem.m_btPlace    = SrcPos.nPlace;
		sMoveItem.m_btX        = SrcPos.nX;
		sMoveItem.m_btY        = SrcPos.nY;
        sMoveItem.m_destRoom   = destRoom;
		sMoveItem.m_ItemWidth  = Item[nItemID].GetWidth();
        sMoveItem.m_ItemHigth  = Item[nItemID].GetHeight();

		if (g_pClient)
			g_pClient->SendPackToServer(&sMoveItem, sizeof(PLAYER_AUTOMUVE_ITEM_COMMAND));

	return TRUE;
}


//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个物品，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpObj(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item && Object[nObjIndex].m_nKind != Obj_Kind_Money)
		return;
	
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{//钱
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;

		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("助手:捡取物品超时,执行取消捡取!");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }
	}
	else
	{//其他的物品
		ItemPos	sItemPos;
		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos))
		{
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("提示:转存子母袋失败,空间不足！");
				return;
			}
			
			int nItemEX = Npc[m_nIndex].m_ExItemId; //字母袋是否开启
			
			if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}
				
				m_ItemList.ClientShowMsg("提示:转存子母袋失败,子母袋还未开启！");
				return;
			}	

		}
	
		 if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("助手:捡取物品超时,执行取消捡取!");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }

/*		  char msg[64];
          t_sprintf(msg,"测试:禁止捡:%d,dwidx:%d",Object[nObjIndex].m_IsHaveAttack,Object[nObjIndex].m_AttackerDwid);
          m_ItemList.ClientShowMsg(msg);
*/
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;
		sPickUp.m_btPosX = sItemPos.nX;  //包袱容器的位置
		sPickUp.m_btPosY = sItemPos.nY;  //包袱容器的位置
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个钱，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpMoney(int nObjIndex,int rid)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Money)
	{
      Npc[rid].m_nObjectIdx=0;
	  return;
	}
	//m_ItemList.ClientShowMsg("测试怪物捡钱！！");
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //对象
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money) //掉在地上的东西类型为==钱
	{
		sPickUp.ProtocolType = c2s_playerpickupitem; //捡东西协议标识
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = 0;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;

		m_Autoplay.nShiQuNum=0;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个装备，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::PickUpItem(int nObjIndex,int rid)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item)
	{
		Npc[rid].m_nObjectIdx=0;
		return;
	}
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp; //对象
	if (Object[nObjIndex].m_nKind == Obj_Kind_Item) //掉在地上的东西类型为==装备
	{
		ItemPos	sItemPos;

		if (FALSE == m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos) )
		{//检测包袱
			if (FALSE == m_ItemList.SearchItemPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos,room_equipmentex))
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("提示:转存子母袋失败,空间不足！");
				return;
			}

			int nItemEX = Npc[m_nIndex].m_ExItemId; //字母袋是否开启

          	if (nItemEX==0)
			{
				if (Npc[m_nIndex].m_AutoplayId==1)
				{ //挂机状态
					Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
					Object[nObjIndex].m_IsHaveAttack=1;
					Object[nObjIndex].m_AttackerTime=18*60*10;
					//开始发送命令告诉服务器包袱不足了
					ApplyRoomInfo(1);
				}

				m_ItemList.ClientShowMsg("提示:转存子母袋失败,子母袋还未开启！");
				return;
			}	
		}

         if (Npc[m_nIndex].m_AutoplayId==1)
		 {//挂机状态			
			if (m_Autoplay.nShiQuNum>m_Autoplay.nVal && m_Autoplay.nShiQuNum%m_Autoplay.nVal==0)
			{
                Object[nObjIndex].m_AttackerDwid=Npc[m_nIndex].m_dwID;
                Object[nObjIndex].m_IsHaveAttack=1;
			    Object[nObjIndex].m_AttackerTime=18*60*10;
				m_Autoplay.nShiQuNum=0;
				m_ItemList.ClientShowMsg("助手:捡取物品超时,执行取消捡取!");
				return;
			}
			  m_Autoplay.nShiQuNum++;
		 }
		 else
		 {
		      m_Autoplay.nShiQuNum=0;
		 }

		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosType = sItemPos.nPlace;  //目标容器
		sPickUp.m_btPosX = sItemPos.nX;         //目标位置
		sPickUp.m_btPosY = sItemPos.nY;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj，向服务器发消息   执行脚本
//-------------------------------------------------------------------------
void	KPlayer::ObjMouseClick(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Box && Object[nObjIndex].m_nKind != Obj_Kind_Prop)
		return;
	if (Object[nObjIndex].m_nRegionIdx<0)
		return;

	SendObjMouseClick(Object[nObjIndex].m_nID, SubWorld[0].m_Region[Object[nObjIndex].m_nRegionIdx].m_RegionID);
}

// DownPos Vi tri can chuyen den，UpPos vi tri goc
BOOL	KPlayer::MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp)
{
	
	//Check vi tri can chuyen toi co item ko (DownPos)
	if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchItemPositionKienTM(1,1,DownPos.nX,DownPos.nY,room_equipment)){
		//Neu vi tri can chuyen toi (DownPos) da co item thi chuyen lai item ve vi tri cu (UpPos)
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoMoveItem(DownPos,UpPos,nIsComp);
		return FALSE;
	}
	
	SendClientCmdMoveItem(&UpPos,&DownPos,nIsComp); //DownPos 目标容器
	return TRUE;
}

// DownPos 是面板上的物品的当前坐标，UpPos 必须是手上物品放到面板上的坐标
void	KPlayer::MoveSkill(ItemPos DownPos, ItemPos UpPos)
{
	if (!CheckTrading() && DownPos.nPlace == pos_immediacy)
	{
		int nHandIdx = m_ItemList.Hand();
		if (nHandIdx)
		{
			if (m_ItemList.m_Room[room_immediacy].CheckSameDetailType(Item[nHandIdx].GetGenre(), Item[nHandIdx].GetDetailType(),Item[nHandIdx].GetParticular()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				t_sprintf(sMsg.szMessage, "M?o: Thanh phím t?t ?? có k? n?ng t??ng t?！");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
	}
	
//	SendClientCmdMoveItem(&DownPos, &UpPos);
}

//v?t b? v?t ph?m v?t b? v?t ph?m
int	KPlayer::ThrowAwayItem()
{
	if (this->CheckTrading())
	{
	    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("M?o: Trong quá trình giao d?ch, b?n kh?ng ???c b? v?t ph?m!");
		return 0;
	}

	if (!m_ItemList.Hand())
	{

	    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("物品为空！！！！");
		return 0;
	}
/*	else
	{
	   char msg[64];
	   t_sprintf(msg,"物品索引:%d..",m_ItemList.Hand());
	   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	}*/
	int	nIdx =m_ItemList.Hand();

	if (/*Item[nIdx].GetTime()->bYear>0 || */Item[nIdx].GetIsBang())
	{
       int nReg=0;
	       g_GameSetTing.GetInteger("SYSTEM","ThrowAwayItem",1,&nReg);	//设置是否启用丢弃限制

     if (!nReg)
	 { 
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage,"警告:(限时/绑定)装备不能丢弃!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	    return 0;
	 } 
	}

	PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
	
  sThrow.ProtocolType = c2s_playerthrowawayitem;
  sThrow.m_Kind=0;
  sThrow.m_isAttack =0;
  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId==1)
  {
    sThrow.m_ItemDwidx=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
	sThrow.m_IsAuToFenJie=m_Autoplay.nIsAuToFenJie;
  }
  else
  {
    sThrow.m_ItemDwidx=0;
	sThrow.m_IsAuToFenJie=0;
  }
	if (g_pClient)
		g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
	
	return 1;
}


int	KPlayer::AutoAwayItem(DWORD nItmeDwIdx,bool isAttack)
{

    PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
	
	sThrow.ProtocolType = c2s_playerthrowawayitem;
	sThrow.m_Kind=1;
    sThrow.m_ItemDwidx=nItmeDwIdx;
	sThrow.m_IsAuToFenJie=m_Autoplay.nIsAuToFenJie;
	sThrow.m_isAttack =isAttack;
	if (g_pClient)
		g_pClient->SendPackToServer(&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
	
	return 1;
}

void	KPlayer::ChatAddFriend(int nPlayerIdx)
{
	CHAT_ADD_FRIEND_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_chataddfriend;
	sAdd.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(CHAT_ADD_FRIEND_COMMAND));
}

void	KPlayer::ChatRefuseFriend(int nPlayerIdx)
{
	CHAT_REFUSE_FRIEND_COMMAND	sRefuse;
	sRefuse.ProtocolType = c2s_chatrefusefriend;
	sRefuse.m_nTargetPlayerIdx = nPlayerIdx;
	if (g_pClient)
		g_pClient->SendPackToServer(&sRefuse, sizeof(CHAT_REFUSE_FRIEND_COMMAND));
}


void	KPlayer::TradeApplyOpen(char *lpszSentence, int nLength)
{
	if (this->CheckTrading())
		return;

	if (!lpszSentence)
		nLength = 0;

	TRADE_APPLY_OPEN_COMMAND	sOpen;
	sOpen.ProtocolType = c2s_tradeapplystateopen;
	if (nLength >= MAX_SENTENCE_LENGTH)
		nLength = MAX_SENTENCE_LENGTH - 1;
	sOpen.m_wLength = sizeof(TRADE_APPLY_OPEN_COMMAND) - 1 - sizeof(sOpen.m_szSentence) + nLength;
	memset(m_cMenuState.m_szSentence, 0, sizeof(m_cMenuState.m_szSentence));
	if (lpszSentence)
		memcpy(m_cMenuState.m_szSentence, lpszSentence, nLength);

	memset(sOpen.m_szSentence, 0, sizeof(sOpen.m_szSentence));
	memcpy(sOpen.m_szSentence, m_cMenuState.m_szSentence, nLength);
	if (g_pClient)
		g_pClient->SendPackToServer(&sOpen, sOpen.m_wLength + 1);
}

void	KPlayer::TradeApplyClose()
{
	TRADE_APPLY_CLOSE_COMMAND	sApply;
	sApply.ProtocolType = c2s_tradeapplystateclose;
	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TRADE_APPLY_CLOSE_COMMAND));
}

void	KPlayer::TradeApplyStart(int nNpcIdx)
{
	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPC)
		return;
	TRADE_APPLY_START_COMMAND	sStart;
	sStart.ProtocolType = c2s_tradeapplystart;
	sStart.m_dwID       = Npc[nNpcIdx].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sStart, sizeof(TRADE_APPLY_START_COMMAND));
	
	KSystemMessage	sMsg;
	t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TRADE_SEND_APPLY].c_str(), Npc[nNpcIdx].Name);
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

BOOL	KPlayer::TradeMoveMoney(int nMoney)
{
	if (!CheckTrading())
		return FALSE;
	if (this->m_cTrade.m_nTradeLock)
		return FALSE;
	// 钱数量错误
	if (nMoney < 0 || nMoney > m_ItemList.GetEquipmentMoney())
		return FALSE;
	
	TRADE_MOVE_MONEY_COMMAND	sMoney;
	
	sMoney.ProtocolType = c2s_trademovemoney;
	sMoney.m_nMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer(&sMoney, sizeof(TRADE_MOVE_MONEY_COMMAND));
	
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：交易确定或取消
//	if nDecision == 0 推出交易  if nDecision == 1 确定交易  if nDecision == 2 取消交易确定
//-------------------------------------------------------------------------
void	KPlayer::TradeDecision(int nDecision)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nDecision == 1)
	{
		sDecision.m_btDecision = 1;
	}
	else if (nDecision == 0)
	{
		sDecision.m_btDecision = 0;
	}
	else if (nDecision == 2)
	{
		sDecision.m_btDecision = 4;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：交易锁定或取消锁定
//-------------------------------------------------------------------------
void	KPlayer::TradeApplyLock(int nLockOrNot)
{
	if ( !CheckTrading() )
		return;
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	if (nLockOrNot)
	{
		sDecision.m_btDecision = 2;
	}
	else
	{
		sDecision.m_btDecision = 3;
	}
	if (g_pClient)
		g_pClient->SendPackToServer(&sDecision, sizeof(TRADE_DECISION_COMMAND));
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有人申请加入队伍
//-------------------------------------------------------------------------
void	KPlayer::s2cApplyAddTeam(BYTE* pProtocol)
{
	// 状态检查 (还需要检查 npc 的队伍开放状态 not end)
	if ( !m_cTeam.m_nFlag || m_cTeam.m_nFigure != TEAM_CAPTAIN)
	{
		ApplySelfTeamInfo();
		return;
	}
	
	// 寻找 npc
	PLAYER_APPLY_ADD_TEAM	*pAddTeam = (PLAYER_APPLY_ADD_TEAM*)pProtocol;
	int nNpcNo = NpcSet.SearchID(pAddTeam->m_dwTarNpcID);
	if (nNpcNo == 0)
		return;
	
	int i, nFreeListNo;
	// 如果已经存在，更新
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == pAddTeam->m_dwTarNpcID)
		{
			m_cTeam.m_sApplyList[i].m_nLevel = Npc[nNpcNo].m_Level;
			m_cTeam.m_sApplyList[i].m_dwTimer = MAX_APPLY_TEAM_TIME;
			strcpy(m_cTeam.m_sApplyList[i].m_szName, Npc[nNpcNo].Name);
			m_cTeam.UpdateInterface();
			m_cTeam.UpdateamUI(); //显示主界面队伍信息
			return;
		}
	}
	// 寻找申请人list空位
	for (i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_cTeam.m_sApplyList[i].m_dwNpcID == 0)
		{
			nFreeListNo = i;
			break;
		}
	}
	if (i >= MAX_TEAM_APPLY_LIST)	// 没找到申请人空位
		return;
	
	m_cTeam.m_sApplyList[nFreeListNo].m_dwNpcID = pAddTeam->m_dwTarNpcID;
	m_cTeam.m_sApplyList[nFreeListNo].m_nLevel = Npc[nNpcNo].m_Level;
	m_cTeam.m_sApplyList[nFreeListNo].m_dwTimer = MAX_APPLY_TEAM_TIME;
	strcpy(m_cTeam.m_sApplyList[nFreeListNo].m_szName, Npc[nNpcNo].Name);
	
	m_cTeam.UpdateInterface();
	m_cTeam.UpdateamUI(); //显示主界面队伍信息
//-------------------------------------------------//自动应答
    //pPlayer = (KUiPlayerItem*)(&m_pHandlingMsg[1]);
			//g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE,(unsigned int)pPlayer, (nSelAction == 0));	//nSelAction=0：答应, nSelAction=1：拒绝
   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsHuiDa==1 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiZhang==1)
   {
      Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(pAddTeam->m_dwTarNpcID);
	  return;
   }
//-------------------------------------------------

	KSystemMessage	sMsg;
	t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TEAM_APPLY_ADD].c_str(), Npc[nNpcNo].Name);
	sMsg.eType = SMT_TEAM;
	sMsg.byConfirmType = SMCT_UI_TEAM_APPLY;
	sMsg.byPriority = 3;   
	sMsg.byParamSize = sizeof(KUiPlayerItem);
    sMsg.nMsgLen=TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
	KUiPlayerItem	player;
	strcpy(player.Name, Npc[nNpcNo].Name);
	player.nIndex = 0;
	player.uId = pAddTeam->m_dwTarNpcID;
	
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&player);
	
	return;
}

//设置快捷技能 --默认
void	KPlayer::SetDefaultImmedSkill()  
{
	//获取玩家左右键技能为默认物理技能
	int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
	int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
	
	//近身武器
	if (nDetailType == 0)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
	}//远程武器
	else if (nDetailType == 1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
	}//空手
	else if (nDetailType == -1)
	{
		Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
		Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
	}

}
//设置快捷栏拾取技能
void	KPlayer::SetShiquSkill(int nSkillID)
{

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) <= 0) 
		return;

	KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = nSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -1);

}

void KPlayer::SetLeftSkill(int nSkillID,BOOL nIsNoLogin)
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) < 0) 
		return;

    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ReSkillEnhance(m_nLeftSkillID,0); //先减去上一次左键技能加成

	//写内挂技能编号
	if (nIsNoLogin)
	{
		 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:执行保存自动挂机技能成功!");
		 KIniFile nAutoConfig;
		 char nConfig[128]={0};	   
		 t_sprintf(nConfig,"jx50ai/autoset/%u_set.ini",g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
					   
		 if  (nAutoConfig.Load(nConfig))
		 { 
			 nAutoConfig.WriteInteger("AAAA","LeftSkill_0",nSkillID); 
		 }
		 else
		 {
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:角色配置不存在!");

		 }
		 nAutoConfig.Save(nConfig);
		 nAutoConfig.Clear();
	}

	m_nLeftSkillID = nSkillID;	
	m_nLeftListidx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(m_nLeftSkillID);

	SKILL_LEFT_SYNC	sLeftSkill;
	sLeftSkill.ProtocolType = c2s_skillsync;
	sLeftSkill.m_nLeftskill = nSkillID;
    sLeftSkill.m_Type=0;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ReSkillEnhance(nSkillID,1);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sLeftSkill, sizeof(SKILL_LEFT_SYNC));
    
	//界面处理
	//KUiGameObject Info;
	//Info.uGenre = CGOG_SKILL_SHORTCUT;
	//Info.uId = m_nLeftSkillID;
	//CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -1);
}


void	KPlayer::SetRightSkill(int nSkillID)
{
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillID) < 0) 
		return;

	m_nRightSkillID = nSkillID;
	m_nRightListidx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(m_nRightSkillID);
	// 检查是否是光环技能
	ISkill * pOrdinSkill = g_SkillManager.GetSkill(nSkillID, 1);

	if (!pOrdinSkill) 
        return;
	
	if (pOrdinSkill->IsAura())
	{//如果是光环技能 就立即使用
		Npc[m_nIndex].SetAuraSkill(nSkillID);
	}
	else
	{//否则，设置为不是光环技能
		Npc[m_nIndex].SetAuraSkill(0);
	}
    //界面处理
	/*KUiGameObject Info;
	Info.uGenre = CGOG_SKILL_SHORTCUT;
	Info.uId = m_nRightSkillID;
	CoreDataChanged(GDCNI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Info, -2);
	(*/
}
//换装 更新空手武器限制技能数据
void KPlayer::UpdateWeaponSkill()
{
	if (m_nLeftSkillID > 0)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(m_nLeftSkillID, 1);
		if (!pISkill) 
            return;
		
		if (m_nLeftSkillID==1 || m_nLeftSkillID==2 || m_nLeftSkillID==53)
		{//设置左键技能 id
			SetLeftSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
		
	}
	if (m_nRightSkillID > 0)
	{//设置右键技能 id
		ISkill * pISkill = (KSkill *) g_SkillManager.GetSkill(m_nRightSkillID, 1);
		if (!pISkill) 
            return;
		
		//if (pISkill->IsPhysical()) //外功系设置
		if (m_nRightSkillID==1 || m_nRightSkillID==2 || m_nRightSkillID==53)
		{
			SetRightSkill(Npc[m_nIndex].GetCurActiveWeaponSkill());
		}
	}
}

//-------------------------------------------------------------------------
//	功能：设定当前聊天频道
//-------------------------------------------------------------------------
void	KPlayer::SetChatCurChannel(int nChannelNo)
{
//	m_cChat.SetCurChannel(nChannelNo);
}

//-------------------------------------------------------------------------
//	功能：邀请加入队伍
//-------------------------------------------------------------------------
void	KPlayer::TeamInviteAdd(DWORD dwNpcID)
{
	m_cTeam.InviteAdd(dwNpcID);
}


#define MAX_ORDINSKILL_LEVEL_ALWAYS  20//不包括其它情况对技能等级的变动之外的，一般最大技能等级
//地上物品 与 身上物品公用一个索引

BOOL KPlayer::CheckThisItem(int nSetings,char *nKey)
{
     
	int nRow = g_ForbitMap.GetHeight()+1,nReg=FALSE;
	
	for (int i=2;i<nRow;++i)
	{
		int nSkilid =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nSkilid);
		if (nSkilid==nSetings)
		{
			nReg =TRUE;
			break;
		}
	}
	return nReg;   
}

BOOL	KPlayer::ExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam,char * szCanshu,BOOL nIsGive)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	t_sprintf(nCurFileName,ScriptFileName);
	//g_StrLower(ScriptFileName);
	g_StrLower(nCurFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(g_CheckFileExist(nCurFileName));
	
	if (pScript==NULL)
	{
		printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}

	pScript=NULL;


	return ExecuteScriptB(g_CheckFileExist(nCurFileName), szFunName, nParam,szCanshu,nIsGive);	
}
//客户端在执行脚本函数
#define MAX_TRYEXECUTESCRIPT_COUNT 5
BOOL	KPlayer::ExecuteScriptB(DWORD dwScriptId, char * szFunName, int nParam,char * szCanshu,BOOL nIsGive)
{
		int nTopIndex = 0;
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);

		if (pScript==NULL)
		{
			printf("-------脚本不存在,执行[%s]失败!-------- \n",szFunName);
			return FALSE;
		}

     try
	 {
		if (pScript)
		{
			if (nIsGive)
			    Npc[m_nIndex].m_ActionScriptID = dwScriptId;

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);  //返回数字
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			nTopIndex=pScript->SafeCallBegin();

			if (!szCanshu ||!szCanshu[0])
			{
				if (pScript->CallFunction(szFunName,0,"")) 
				{
					bExecuteScriptMistake = false;
				}
			}
			else if (pScript->CallFunction(szFunName,0,"ds",nParam,szCanshu)) 
			{
				bExecuteScriptMistake = false;
			}
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
			//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}
		
		printf("执行脚本发生意外B,Script[%s]!\n",szFunName);
	//	printf("Exception Have Caught When Execute Script[%d],[%s]!!!!!", dwScriptId,szFunName);
//		m_bWaitingPlayerFeedBack = false;
//		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL KPlayer::ExecuteScriptD(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb,int inval,int invala,BOOL nIsGive)
{

//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;

		if (pScript==NULL)
		{
			printf("-------脚本不存在,D执行[%s]失败!-------- \n",szFunName);
			return FALSE;
		}

	try
	{
		if (pScript)
		{
			if (nIsGive)
			   Npc[m_nIndex].m_ActionScriptID = dwScriptId;  //赋值给人物的执行脚本

			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			nTopIndex=pScript->SafeCallBegin();
			
			if (!szParams || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,0,"%d",0))  //一个参数
				{
					bExecuteScriptMistake = false;
				}
			}
			else if (pScript->CallFunction(szFunName,0,"ssddddd",szParams,snParams,inParams,njb,njxb,inval,invala))  //有参数
			{
					bExecuteScriptMistake = false;
			}
		  nTopIndex=pScript->SafeCallBegin();	
		  pScript->SafeCallEnd(nTopIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		
		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		 return TRUE;
	}
	catch(...)
	{

		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
		    pScript->SafeCallEnd(nTopIndex);
			printf("执行脚本发生意外C,Script[%s],[%s]!\n", pScript->m_szScriptName,szFunName);

			//pScript->Exit(); //释放脚本资源
		}
		//printf("Exception Have Caught When Execute Script[%d],[%s]", dwScriptId,szFunName);
//		m_bWaitingPlayerFeedBack = false;
//		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL KPlayer::ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind,int nParama,int nParamb,int nParamc)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;  
	char nCurFileName[128]={0};
	t_sprintf(nCurFileName,ScriptFileName);
	g_StrLower(nCurFileName);
	DWORD dwScriptId = g_CheckFileExist(nCurFileName);

	KLuaScript * pScript=NULL;
	pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		printf("-------Script error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}
    //开始执行脚本
	bool bExecuteScriptMistake = true;
	int nSafeIndex = 0;
	int bResult=1;
	try
	{
		if (pScript)
		{
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);  //PlayerIndex
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			nSafeIndex=pScript->SafeCallBegin();

		    if (pScript->CallFunction(szFunName,1, "dddd", nKind,nParama,nParamb,nParamc))  //有参数
			{
				bExecuteScriptMistake = false;
			}

			if (bExecuteScriptMistake == false)
			{
				char *bResultchar=NULL; 
				if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
				{//Lua_PushNumber
					bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				}
				else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
				{//Lua_PushSting
					bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
					bResult     = atoi(bResultchar);
				}
			}
			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex); 
			return bResult;
		}

		if (bExecuteScriptMistake)
		{
			printf("script error,Script[%s][%s]!\n",ScriptFileName,szFunName);
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nSafeIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nSafeIndex);
		}
		printf("script error,Script[%s]!\n",ScriptFileName);
		return TRUE;
	}
}

BOOL KPlayer::ExecuteScriptC(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb,int inval,int invala,BOOL nIsGive)
{

	//if  (Npc[m_nIndex].m_nPeopleIdx >0)
	//	return FALSE;
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;  
	char nCurFileName[128]={0};
	t_sprintf(nCurFileName,ScriptFileName);
	g_StrLower(nCurFileName);
	//g_StrLower(ScriptFileName);
	DWORD dwScriptId = g_CheckFileExist(nCurFileName);

	KLuaScript * pScript=NULL;
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		printf("-------Script error,[%s][%s]!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}
	pScript=NULL;

	return ExecuteScriptD(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb,inval,invala,nIsGive);
}
////////////////////////物品右键执行脚本/////////////////////////////////////////////////
BOOL	KPlayer::ExeItemScriptA(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb,int inGenre,int inDetail,int inParticular,int inStackNum,int inItemX,int inItemY)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nCurFileName[128]={0};
	t_sprintf(nCurFileName,ScriptFileName);
	//g_StrLower(ScriptFileName);
	g_StrLower(nCurFileName);
	DWORD dwScriptId = g_CheckFileExist(nCurFileName);
	return ExeItemScript(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb,inGenre,inDetail,inParticular,inStackNum,inItemX,inItemY);
}

BOOL KPlayer::ExeItemScript(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb,int inGenre,int inDetail,int inParticular,int inStackNum,int inItemX,int inItemY)
{
//		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript=NULL;
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nSafeIndex = 0;
		int bResult=1; 

		if (pScript==NULL)
		{
			DWORD dwScriptIdx = g_CheckFileExist("\\script\\item\\noscript.lua");
			pScript = (KLuaScript* )g_GetScript(dwScriptIdx);
			if (pScript)
			{
			//printf("-------物品脚本不存在,替换:\\script\\item\\noscript.lua 成功!-------- \n");
			}
			else
			{
			  printf("-------物品脚本不存在!-------- \n");
			  return TRUE;
			} 
		}

  try
  {
		if (pScript)
		{
			//入栈

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------使用物品前:栈中原来元素有:%d 个--------\n",nSafeIndex);


			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName,1,""))  //空参数
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (( (!snParams) || !snParams[0]) )
                     snParams="";
				if (( (!szParams) || !szParams[0]) )
					 szParams="";

				if (pScript->CallFunction(szFunName,1, "ssdddddddddd", szParams,snParams,inParams,njb,njxb,inGenre,inDetail,inParticular,inStackNum,inItemX,inItemY,0))  //有参数
				{
					bExecuteScriptMistake = false;
				}
			}
		  ///////////////////脚本返回值////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{//Lua_PushSting
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			    bResult     = atoi(bResultchar);
			}
		  }
		  //myPrintLuaStackA(pScript->m_LuaState);
         /////////////////////////////////////////////////
		  nSafeIndex=pScript->SafeCallBegin();
		  //printf("-------使用物品后:栈中原来元素有:%d 个--------\n",nSafeIndex);
		  pScript->SafeCallEnd(nSafeIndex); 
		  //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		  return bResult;

		}

		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			printf("执行右键物品脚本失败,Script[%s]!\n",szFunName);
		}
		 return TRUE;
}
catch(...)
{
	if (pScript)
	{
		nSafeIndex=pScript->SafeCallBegin();	
		pScript->SafeCallEnd(nSafeIndex);
	}
	printf("执行右键物品脚本失败,Script[%s]!\n",szFunName);
	//	printf("Exception Have Caught When Execute Script[%d],[%s]!!!!!", dwScriptId,szFunName);
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

}
//客户端执行脚本
int  KPlayer::ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala)
{  

	    if (!nScriptPath)
			return 0;
//	    m_btTryExecuteScriptTimes = 0;
		char nScriptNewPath[128]={0};
		t_sprintf(nScriptNewPath,"%s",nScriptPath);
		//_strupr(nScriptNewPath); //转成大写 _strupr
		//m_ItemList.ClientShowMsg(nScriptNewPath);
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript=NULL;
		//char nStrInfo[128]={0};
		//t_sprintf(nStrInfo,nScriptPath);
		g_StrLower(nScriptNewPath);   //大写转小写  g_StrLower
        
		 //m_ItemList.ClientShowMsg(nScriptNewPath);
		 //return 0;

		DWORD dwScriptId   = g_CheckFileExist(nScriptNewPath);
			       pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nSafeIndex = 0;
		int bResult=1; 
 
		if (pScript==NULL)
		{
			DWORD dwScriptIdx = g_CheckFileExist("script\\item\\noscript.lua");
			pScript = (KLuaScript* )g_GetScript(dwScriptIdx);
			if (pScript)
			{
			   //printf("-------物品脚本不存在,替换:\\script\\item\\noscript.lua 成功!-------- \n");
			}
			else
			{
				char nMsg[128]={0};
				t_sprintf(nMsg,"---脚本不存在:%s--",nScriptNewPath);
			    m_ItemList.ClientShowMsg(nMsg);
			    return FALSE;
			} 
		}

  try
  {
		if (pScript)
		{
			//入栈

			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			//myPrintLuaStackA(pScript->m_LuaState);

			nSafeIndex=pScript->SafeCallBegin();
			
			//printf("-------使用物品前:栈中原来元素有:%d 个--------\n",nSafeIndex);
		 if (pScript->CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala))  //有返回值
  //有参数
		 {
			 bExecuteScriptMistake = false;
		 }
		  ///////////////////脚本返回值////////////////////////
          if (bExecuteScriptMistake == false)
		  {
			char *bResultchar=NULL; 
			//const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				bResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{//Lua_PushSting
				bResultchar = (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			    bResult     = atoi(bResultchar);
			}
		  }
		  nSafeIndex=pScript->SafeCallBegin();
		  pScript->SafeCallEnd(nSafeIndex); 
		  return bResult;

		}

		if (bExecuteScriptMistake)
		{
//			m_bWaitingPlayerFeedBack = false;
//			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			m_ItemList.ClientShowMsg("-------脚本错误!-------- ");
		}
		 return TRUE;
}
catch(...)
{
	if (pScript)
	{
		nSafeIndex=pScript->SafeCallBegin();	
		pScript->SafeCallEnd(nSafeIndex);
	}
	m_ItemList.ClientShowMsg("-------脚本错误!-------- ");
//	m_bWaitingPlayerFeedBack = false;
//	m_btTryExecuteScriptTimes = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	return TRUE;
}

	   /* ZeroMemory(szClienResult,sizeof(szClienResult));
	    KLuaScript nClentLua;
	    nClentLua.Init();                                                    //初始化 注册标准函数库
		nClentLua.RegisterFunctions(GameScriptFuns,g_GetGameScriptFunNum()); //把C脚本函数注册到脚本中

		if (nClentLua.Load(nScriptPath))
		{
			//m_ItemList.ClientShowMsg("脚本执行成功C");
			t_sprintf(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,nScriptPath);
			//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID =g_FileName2Id(nScriptPath);
			int nSafeIndex = 0;
			nSafeIndex=nClentLua.SafeCallBegin();
			//执行这个脚本的函数				
			nClentLua.CallFunction(nFunName,1, "dds",nPaparm,ninVal,nstrVala);  //有返回值
			const char * szType = lua_typename(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
			if (Lua_IsNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) == 1)
			{//Lua_PushNumber
				int nResult = (int)Lua_ValueToNumber(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState));
				t_sprintf(szClienResult, "%d", nResult);  //int类型
			}
			else if (Lua_IsString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)) ==1)//字符串类型
			{
				t_sprintf(szClienResult, (char *)Lua_ValueToString(nClentLua.m_LuaState, Lua_GetTopIndex(nClentLua.m_LuaState)));
			}
			else
				t_sprintf(szClienResult,"<color=Red>脚本数据错误GC,请联系GM处理!<color>");  
			    //m_ItemList.ClientShowMsg("执行脚本成功GC!");
			    nSafeIndex=nClentLua.SafeCallBegin();
			    nClentLua.SafeCallEnd(nSafeIndex);
		}
		else
		{
		  ZeroMemory(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath,sizeof(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ScriptPicPath));
		  //Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActionScriptID=0;
		  m_ItemList.ClientShowMsg("执行脚本失败GC,请联系GM处理!");
		}
		//m_ItemList.ClientShowMsg("脚本执行成功");
		nClentLua.Exit();
		return atoi(szClienResult);*/ 
}

//显示栈信息的函数
void KPlayer::myPrintLuaStackA(lua_State *L)
{
    int stackTop=lua_gettop(L);//获取栈顶的索引值
    int index,t;
    printf("--栈顶(v)(%d)--\n",stackTop);
    //显示栈中的元素
    for(index=stackTop;index>0;--index)
	{  t=lua_type(L,index);
	    printf("(i:%d) %s(%s)\n",index,lua_typename(L,t),lua_tostring(L,index));	     
	}
    printf("--栈底--\n");
}
/////////////////////////////////////////////////////////////////
//GM执行脚本
BOOL	KPlayer::DoScript(char * ScriptCommand)
{

	return TRUE;
/*	if (NULL == ScriptCommand) return FALSE;
	KLuaScript *Script = new KLuaScript;
	Script->Init();
	Script->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
	
	//GM Standand Script Functions 
//	Script->Load("\\script\\system\\startserver--.lua.lua");  //GM执行开始游戏脚本
	
	
	Lua_PushNumber(Script->m_LuaState, m_nPlayerIndex);
	Script->SetGlobalName(SCRIPT_PLAYERINDEX);
	Lua_PushNumber(Script->m_LuaState, m_dwID);
	Script->SetGlobalName(SCRIPT_PLAYERID);	
	Lua_PushNumber(Script->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
	Script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	
	if (Script->LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand))) 
	{
		BOOL bResult = Script->ExecuteCode();	
		delete Script; //Question!
         Script=NULL;
		return bResult;
	}
	delete Script; //Question!
	Script=NULL;
	return FALSE;*/
}


void	KPlayer::DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo) //要求显示某个UI界面
{
	if (!pUIInfo) return;
	
	//服务器端脚本时
	if (pUIInfo->m_bParam2 < 1)
	{//客户端脚本要求显示脚本 直接运行
		OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pUIInfo);
	}
	
}

//服务器端获知玩家界面UI选择了某项后，处理执行脚本函数~~
void KPlayer::ProcessPlayerSelectFromUI(BYTE* pProtocol)			// 处理当玩家从选择菜单选择某项时的操作	
{
	PLAYER_SELECTUI_COMMAND * pSelUI = (PLAYER_SELECTUI_COMMAND*) pProtocol;
//	m_bWaitingPlayerFeedBack = false;
	//如果返回负数，表示退出该脚本执行环境
	if (pSelUI->nSelectIndex < 0) 
		m_nAvailableAnswerNum = 0;

	if (m_nAvailableAnswerNum > pSelUI->nSelectIndex)
	{
		if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
		{
			//g_SetFilePath("\\script");
			if (m_nIndex)
			{
			      //m_ItemList.ClientShowMsg(Npc[m_nIndex].m_ScriptPicPath);
				  //m_ItemList.ClientShowMsg(m_szTaskAnswerFun[pSelUI->nSelectIndex]);
				  char nstrPath[256]={0};
				  Npc[m_nIndex].GetstrInfo(STR_SCRIPTTATH_CLIENT,nstrPath);
				  ClientExeItemScript(nstrPath,m_szTaskAnswerFun[pSelUI->nSelectIndex],pSelUI->nSelectIndex,0,m_szTaskAnswerCanshu[pSelUI->nSelectIndex]) ;
			}
			/*else
			{//客户端执行
			     Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\Fs_NewTaskWindow.lua","newtaskrequestinfo",0,0,"") ;
			} */
		}
	}
}
//客户端

//玩家在界面交互后，选择了某项后，向服务器端发送
void	KPlayer::OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo)	//当玩家从选择框中选择某项后，将向服务器发送			
{
	if (!pSelectUI) return;
	
	switch(eUIInfo)
	{
	case UI_SELECTDIALOG:
		{
			if (g_bUISelIntelActiveWithServer)
			{//是否是服务器执行
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
			    if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//客户端执行
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);// 处理当玩家从选择菜单选择某项时的操作	
			}
		}
		break;
	case UI_TALKDIALOG:
		{
			if (g_bUISpeakActiveWithServer)
			{//是否是服务器执行
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				if (g_pClient)
     				g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
			}
			else
			{//客户端执行
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);			// 处理当玩家从选择菜单选择某项时的操作	
			}
			
		}break;
	}
}


//-------------------------------------------------------------------------
//	功能：获得通知某好友上线了
//-------------------------------------------------------------------------
void	KPlayer::ChatFriendOnLine(DWORD dwID, int nFriendIdx)
{
	/*if (nFriendIdx <= 0)
		return;
	for (int i = 0; i < MAX_FRIEND_TEAM; ++i)
	{
		if (m_cChat.m_cFriendTeam[i].m_nFriendNo == 0)
			continue;
		CChatFriend	*pFriend;
		pFriend = (CChatFriend*)m_cChat.m_cFriendTeam[i].m_cEveryOne.GetHead();
		while (pFriend)
		{
			if (pFriend->m_dwID == dwID)
			{
				pFriend->m_nPlayerIdx = nFriendIdx;
				// 通知界面有好友上线
				KUiPlayerItem	sPlayer;
				strcpy(sPlayer.Name, pFriend->m_szName);
				sPlayer.uId = dwID;
				sPlayer.nIndex = nFriendIdx;
				sPlayer.nData = CHAT_S_ONLINE;

				CoreDataChanged(GDCNI_CHAT_FRIEND_STATUS, (unsigned int)&sPlayer, i);
				
				KSystemMessage	sMsg;
				t_sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_ONLINE, pFriend->m_szName);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
			pFriend = (CChatFriend*)pFriend->GetNext();
		}
	}*/
}

// 获得当前门派名称
void	KPlayer::GetFactionName(char *lpszName, int nSize)
{
	this->m_cFaction.GetCurFactionName(lpszName);
	return;
}


void	KPlayer::OnScriptAction(PLAYER_SCRIPTACTION_SYNC * pMsg)
{
	if (!g_GameWorld) return;

	PLAYER_SCRIPTACTION_SYNC * pScriptAction = (PLAYER_SCRIPTACTION_SYNC *)pMsg;

	char szString[1024]={0};
	ZeroMemory(szString,sizeof(szString));
	//CCMessageBox("打开对话框","打开对话框...");
	switch(pScriptAction->m_nOperateType)
	{
	case SCRIPTACTION_UISHOW:
		{
			switch(pScriptAction->m_bUIId)
			{
			case UI_SELECTDIALOG://通知客户端显示选择窗口
				{
					KUiQuestionAndAnswer	*pQuest = NULL;
					KUiNpcSpr               *pImage = NULL;
					if (pScriptAction->m_nBufferLen <= 0 || pScriptAction->m_nBufferLen>=1024) break;
					
					if (pScriptAction->m_bOptionNum <= 0)
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer));
					else
						pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer) + sizeof(KUiAnswer) * (pScriptAction->m_bOptionNum - 1));
					
					if (pQuest==NULL)
						break;
					pImage = (KUiNpcSpr *)malloc(sizeof(KUiNpcSpr));
					if (pImage==NULL)
						break;
					memset(pQuest,0,sizeof(KUiQuestionAndAnswer));
					memset(pImage,0,sizeof(KUiNpcSpr));

					char *strContent=NULL;
					      strContent=(char *)malloc(1024);
				    if (strContent==NULL)
					    break;	 
					ZeroMemory(strContent,sizeof(strContent));
					char * pAnswer = NULL;
					pQuest->AnswerCount = 0;
					//主信息为字符串
					int nCurAnswerLen = 0;//strlen(pScriptAction->m_pContent)-pScriptAction->m_nBufferLen -1;
					if (pScriptAction->m_bParam1 == 0)
					{
						//g_StrCpyLen(strContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1); //公告内容
						memcpy(&strContent[0],pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
						strContent[pScriptAction->m_nBufferLen]='\0';
						pAnswer = strstr(strContent,"|");   //获取函数名 取后面的值

						if (!pAnswer)
						{
							pScriptAction->m_bOptionNum = 0;
							pQuest->AnswerCount = 0;
						}
						else
							*pAnswer++ = 0; //过滤 字符串 “|”后面的内容  ---截取出选项内容

						//g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));  //公告内容
						memcpy(&pQuest->Question[0],strContent,sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText_(pQuest->Question, strlen(pQuest->Question));
						pQuest->Question[pQuest->QuestionLen]='\0';
					}
					//主信息为数字标识
					else 
					{
						g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, sizeof(szString)), sizeof(pQuest->Question));
						pQuest->QuestionLen = TEncodeText_(pQuest->Question, strlen(pQuest->Question));
						g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(int) + 1);
						pAnswer = strContent + 1;
					}
					// 以下为选项的设置
					for (int i = 0; i < pScriptAction->m_bOptionNum; ++i)   //选项的个数
					{
						char * pNewAnswer = strstr(pAnswer,"|");
						//pAnswer 前面的内容 pNewAnswer 后面的内容(包括 | )
						if (pNewAnswer)                                     //函数名
						{
							*pNewAnswer = 0;                                //去掉 | 换行
							strcpy(pQuest->Answer[i].AnswerText,pAnswer);  // 函数名
							pQuest->Answer[i].AnswerLen = -1;
                            pAnswer = pNewAnswer+1; //去掉一个 |
						}
						else
						{//最后一个选项
#ifdef WIN32
							strcpy(pQuest->Answer[i].AnswerText,pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
                            pQuest->AnswerCount = i + 1;	
#else
							strcpy(pQuest->Answer[i].AnswerText,pAnswer);
							pQuest->Answer[i].AnswerLen = -1;
							pQuest->AnswerCount = i + 1;
#endif
		     				break;
						}
					}//end for

					if (pScriptAction->m_bParam2>=1)
					   g_bUISelIntelActiveWithServer =TRUE;

					//g_bUISelIntelActiveWithServer = pScriptAction->m_bParam2;
					g_bUISelLastSelCount = pQuest->AnswerCount;
                    /*******************************************Code by thienthanden2*******************************************************/ 
                    char szBuffer[256]={0}; 
					if  (pScriptAction->m_szSprPath[0] && strstr(pScriptAction->m_szSprPath,".spr"))
					{
						strcpy(pImage->ImageFile, pScriptAction->m_szSprPath);
						pImage->MaxFrame = 1;
						goto Next;
					}
					else if (!pScriptAction->m_szSprPath[0] && m_nImageNpcID)  //spr 设置 NPC索引
                    { 

                      /*for (int i = 0; i < 16; ++i) 
					  { 
						Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(i, 3, 0, "", szBuffer, sizeof(szBuffer)); 
                        if (szBuffer[0]) 
                            { 
                            strcpy(pImage->ImageFile, szBuffer); 
                            pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(i, 3, 0, 16))/ 
                            (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(i, 3, 0, 16)); goto Next; 
                            } 
					  } 
                      for (int j = 0; j < 16; ++j) 
					  {  
                         Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetFileName(j, 0, 0, "", szBuffer, sizeof(szBuffer)); 
                        if (szBuffer[0]) 
                            { 
                              strcpy(pImage->ImageFile, szBuffer); 
                              pImage->MaxFrame = (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalFrames(j, 0, 0, 16))/ 
                              (Npc[m_nImageNpcID].GetNpcRes()->m_pcResNode->GetTotalDirs(j, 0, 0, 16)); goto Next; 
                            } 
					  } */
                     
                    } 
                    Next: 
                    if (pScriptAction->m_Select == 1 /*&& m_nImageNpcID*/) 
                          CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int)pQuest,(int)pImage,pScriptAction->m_bParam2); 
                    else
						  CoreDataChanged(GDCNI_QUESTION_CHOOSE,(unsigned int)pQuest, 0); 

                    free(pImage); 
                    pImage = NULL; 
					
                    free(pQuest); 
                    pQuest = NULL;
					
					free(strContent); 
                    strContent = NULL;
                } 
                break;  
				
			case UI_TALKDIALOG: 
				{//talk
					if  (pScriptAction->m_nBufferLen>=512 || pScriptAction->m_nBufferLen<=0)
						return;

					BOOL bUsingSpeakId = pScriptAction->m_bParam1;
					int	 nSentenceCount = pScriptAction->m_bOptionNum;
					if (nSentenceCount <= 0) return ;
					KUiInformationParam *pSpeakList  = new KUiInformationParam[nSentenceCount];
					memset(pSpeakList, 0, sizeof(KUiInformationParam) * nSentenceCount);
					
					char * pAnswer = new char [512];//pScriptAction->m_nBufferLen + 1
					//char *pAnswer=NULL
					 //   pAnswer=(char *)malloc(pScriptAction->m_nBufferLen + 1);
					if (pAnswer==NULL)
						break;

					ZeroMemory(pAnswer,sizeof(pAnswer));
					g_StrCpyLen(pAnswer, pScriptAction->m_pContent, pScriptAction->m_nBufferLen + 1);
					char * pBackupAnswer = pAnswer;

					int nCount = 0;
					for (int i = 0; i < pScriptAction->m_bOptionNum; ++i)
					{
						char * pNewAnswer = strstr(pAnswer, "|");
						
						if (pNewAnswer)
						{
							*pNewAnswer = 0;
							if (!bUsingSpeakId)
								strcpy(pSpeakList[i].sInformation, pAnswer);
							else
								strcpy(pSpeakList[i].sInformation, g_GetStringRes(atoi(pAnswer), szString, sizeof(szString)));

							if (i < pScriptAction->m_bOptionNum - 1)
								strcpy(pSpeakList[i].sConfirmText, "Kplayer so 1");
							else 
							{
								strcpy(pSpeakList[i].sConfirmText, "Kplayer so 2");
								if (pScriptAction->m_nParam == 1)						
									pSpeakList[i].bNeedConfirmNotify = TRUE;
							}
//#ifndef WIN32
//							t_sprintf (pSpeakList[i].sInformation,UTEXT(pSpeakList[i].sInformation,1).c_str());
//#endif
							pSpeakList[i].nInforLen = TEncodeText_(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							pAnswer = pNewAnswer + 1;
						}
						else
						{
							if (!bUsingSpeakId)
								strcpy(pSpeakList[i].sInformation, pAnswer);
							else
								strcpy(pSpeakList[i].sInformation,g_GetStringRes(atoi(pAnswer), szString ,sizeof(szString)));
							
							strcpy(pSpeakList[i].sConfirmText, "Kplayer so 3");
							
							if (pScriptAction->m_nParam == 1)						
								pSpeakList[i].bNeedConfirmNotify = TRUE;
//#ifndef WIN32
//							t_sprintf (pSpeakList[i].sInformation,UTEXT(pSpeakList[i].sInformation,1).c_str());
//#endif
							pSpeakList[i].nInforLen = TEncodeText_(pSpeakList[i].sInformation, strlen(pSpeakList[i].sInformation));
							nCount++;
							break;
						}
						nCount ++;
					} //END FOR

					if (pScriptAction->m_bParam2>=1)
					   g_bUISpeakActiveWithServer =TRUE;

					//g_bUISpeakActiveWithServer = pScriptAction->m_bParam2;
					//客户端调出窗口
					CoreDataChanged(GDCNI_SPEAK_WORDS,(unsigned int)pSpeakList, nCount);
					
					if (pBackupAnswer)
					{
						delete [] pBackupAnswer;
						pBackupAnswer = NULL;
					}
					
					if (pSpeakList)
					{
						delete pSpeakList;	
						pSpeakList = NULL;
					}
				}
				break;
			case UI_MSGINFO:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;

					char strContent[1024];
					//主信息为字符串
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
					}
					else
					{
						g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
					}

					KSystemMessage	sMsg;
					sMsg.eType = SMT_PLAYER;
					sMsg.byConfirmType = SMCT_MSG_BOX;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					g_StrCpyLen(sMsg.szMessage, strContent, sizeof(sMsg.szMessage));
					sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage)); //后来加的
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					//CoreDataChanged(GDCNI_MISSION_RECORD, (unsigned int)strContent, strlen(strContent));

				}break;
			case UI_NEWSINFO:  //滚动公告消息
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					switch(pScriptAction->m_bOptionNum) //OPtionNum代表是哪一种News类型
					{
					case NEWSMESSAGE_NORMAL:  //显示就消亡
						{
							KNewsMessage News;
							News.nType = NEWSMESSAGE_NORMAL;
							
							char strContent[1024];
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));

							News.nMsgLen = TEncodeText_(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, 0);
						}
						break;
					case NEWSMESSAGE_SHUIJI:  //系统随机 显示就消亡
						{							
							KNewsMessage News;
							News.nType = NEWSMESSAGE_SHUIJI;
							
							char strContent[512];
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{//服务器端数字 信息id
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
							}
							else
							{//字符串
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText_(News.sMsg, strlen(News.sMsg));
							CoreDataChanged(GDCNI_SHUIJI_MESSAGE, (unsigned int)&News, 0);
						}
						break;
						//STRING|STRINGID + TIME(INT)
					case NEWSMESSAGE_COUNTING:  //计时消息，时间到就消亡
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(int) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText_(News.sMsg, strlen(News.sMsg));
							
							/*SYSTEMTIME systime;
							memset(&systime, 0, sizeof(SYSTEMTIME));
							systime.wSecond = *(int *)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(int)));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							*/
						}break;

					case NEWSMESSAGE_TIMEEND:  //定时消息
						{
							KNewsMessage News;
							News.nType = pScriptAction->m_bOptionNum;
							
							char strContent[1024];
							int nTime = 0;
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								//g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME) + 1);
							}
							else
							{
								g_GetStringRes(*(int *)pScriptAction->m_pContent, strContent ,sizeof(strContent));
							}
							
							g_StrCpyLen(News.sMsg,  strContent, sizeof(News.sMsg));
							News.nMsgLen = TEncodeText_(News.sMsg, strlen(News.sMsg));

							/*SYSTEMTIME systime;
							systime = *(SYSTEMTIME*)((pScriptAction->m_pContent + pScriptAction->m_nBufferLen - sizeof(SYSTEMTIME)));
							CoreDataChanged(GDCNI_NEWS_MESSAGE, (unsigned int)&News, (unsigned int)&systime);
							*/
						}break;
					
					}
					
				}break;
			case UI_PLAYMUSIC:
				{
					char szMusicFile[MAX_PATH];
					memcpy(szMusicFile, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					szMusicFile[pScriptAction->m_nBufferLen] = 0;
//					g_SubWorldSet.m_cMusic.ScriptPlay(szMusicFile);
				}break;
			
			case UI_OPENTONGUI:		  //打开建帮UI		
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.OpenCreateInterface();
					break;
				}
			}
	} break;
	case SCRIPTACTION_EXESCRIPT://要求客户端调用某个脚本
		{
			if (pScriptAction->m_nBufferLen <= 0 ) break;
			char szScriptInfo[1000];
			g_StrCpyLen(szScriptInfo, pScriptAction->m_pContent,pScriptAction->m_nBufferLen + 1);
			char * pDivPos = strstr(szScriptInfo, "/");  // 斜杠后面的内容 则是脚本参数名
			if (pDivPos)	
				*pDivPos++ = 0; 
			if (pDivPos)
				ExecuteScriptC(szScriptInfo, "OnCall", pDivPos,"");
			else
				ExecuteScriptC(szScriptInfo, "OnCall", "","");
		}
		break;
	}	
}


void  KPlayer::ReSkillEnhance()
{
	int nActiveSkillID;
	
	nActiveSkillID=Player[CLIENT_PLAYER_INDEX].GetLeftSkill();	
	
	if (Npc[m_nIndex].m_Currentskillenhance!= 0)
	{
		int nlistIndex = Npc[m_nIndex].m_SkillList.FindSame(nActiveSkillID);
		if (nlistIndex)                 //查找激活的当前攻击技能ID
		{
			if (Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nlistIndex) > 0)  //等级大于0的才 增加
			{
				Npc[m_nIndex].m_SkillList.AddEnChance(nlistIndex,Npc[m_nIndex].m_Currentskillenhance);
			}			
		}
	}
	
//#ifdef _SERVER
	//char msg[64];
	//t_sprintf(msg,"PS攻击加成:%d",Npc[m_nIndex].m_Currentskillenhance);
	//m_ItemList.msgshow(msg);
//#else
	//char msg[64];
	//t_sprintf(msg,"PC攻击加成:%d",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Currentskillenhance);
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);	
//#endif

}

void	KPlayer::s2cTradeChangeState(BYTE* pMsg)
{

	TRADE_CHANGE_STATE_SYNC	*pTrade = (TRADE_CHANGE_STATE_SYNC*)pMsg;
	
	switch (m_cMenuState.m_nState)
	{
	case PLAYER_MENU_STATE_NORMAL:		// 当前客户端 NORMAL
	case PLAYER_MENU_STATE_TRADEOPEN:	// 当前客户端 TRADEOPEN
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TEAMOPEN:	// 当前客户端 TEAMOPEN
		g_Team[0].SetTeamClose();
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADING);
			m_cTrade.StartTrade(pTrade->m_dwNpcID);
			m_ItemList.StartTrade();
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	case PLAYER_MENU_STATE_TRADING:		// 当前客户端 TRADING
		// 当客户端处于 TRADING 时，不应该收到此协议
		if (pTrade->m_btState == 0)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 1)
		{
			m_cMenuState.SetState(PLAYER_MENU_STATE_TRADEOPEN);
			CoreDataChanged(GDCNI_TRADE_OPER_DATA, 0, 0);
		}
		else if (pTrade->m_btState == 2)	// 客户端此时不应该在这个状态，客户端已出错
		{
			m_cMenuState.m_nState = PLAYER_MENU_STATE_TRADING;
			m_cMenuState.m_nTradeDest = pTrade->m_dwNpcID;
			m_cMenuState.m_nTradeState = 0;
			m_cMenuState.m_nTradeDestState = 0;
			this->m_ItemList.RecoverTrade();
			this->m_ItemList.BackupTrade();
			this->m_ItemList.ClearRoom(room_trade);
			this->m_ItemList.ClearRoom(room_trade1);
			
			// 通知界面进入交易界面
			KUiPlayerItem	sTradePlayer;
			int	nIdx;
			
			nIdx = NpcSet.SearchID(pTrade->m_dwNpcID);
			if (nIdx > 0)
				strcpy(sTradePlayer.Name, Npc[nIdx].Name);
			else
				sTradePlayer.Name[0] = 0;
			sTradePlayer.Name[0] = 0;
			sTradePlayer.nIndex = 0;
			sTradePlayer.uId = 0;
			sTradePlayer.nData = 0;
			CoreDataChanged(GDCNI_TRADE_START, (unsigned int)(&sTradePlayer), 0);
			
			if (Npc[m_nIndex].m_Doing == do_sit)
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
		break;
	}
}

//等级提升
void	KPlayer::s2cLevelUp(BYTE* pMsg)
{
	PLAYER_LEVEL_UP_SYNC	*pLevel = (PLAYER_LEVEL_UP_SYNC*)pMsg;

	if (Npc[m_nIndex].m_Level < pLevel->m_btLevel && m_nExp < PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level))
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, strCoreInfo[MSG_GET_EXP].c_str(), PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level) - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	m_nExp = 0;
	Npc[m_nIndex].m_Level = (WORD)pLevel->m_btLevel;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(Npc[m_nIndex].m_Level);
	
	// -------------------------- 等级增加时各项属性同步 ----------------------
	// 同步未分配属性点
	if (pLevel->m_nAttributePoint > m_nAttributePoint)
	{
		/*KSystemMessage Msg;
		t_sprintf(Msg.szMessage, strCoreInfo[MSG_GET_ATTRIBUTE_POINT].c_str(), pLevel->m_nAttributePoint - m_nAttributePoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_ATTRIBUTE;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);*/
	}
	m_nAttributePoint = pLevel->m_nAttributePoint;
	
	// 同步未分配技能点
	if (m_nSkillPoint < pLevel->m_nSkillPoint)
	{
		/*KSystemMessage Msg;
		t_sprintf(Msg.szMessage, strCoreInfo[MSG_GET_SKILL_POINT].c_str(), pLevel->m_nSkillPoint - m_nSkillPoint);
		Msg.eType = SMT_PLAYER;
		Msg.byConfirmType = SMCT_UI_SKILLS;
		Msg.byPriority = 3;
		Msg.byParamSize = 0;
		Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);*/
	}
	m_nSkillPoint = pLevel->m_nSkillPoint;
	
	// 处理生命、体力、内力值（影响基本生命、体力、内力最大值的因素：等级、属性点，影响当前生命最大值的因素：装备、技能、药物（临时））
	Npc[m_nIndex].m_LifeMax           = pLevel->m_nBaseLifeMax;
	Npc[m_nIndex].m_StaminaMax        = pLevel->m_nBaseStaminaMax;
	Npc[m_nIndex].m_ManaMax           = pLevel->m_nBaseManaMax;
	//	Npc[m_nIndex].ResetLifeReplenish();
	Npc[m_nIndex].m_CurrentLifeMax    = Npc[m_nIndex].m_LifeMax;
	Npc[m_nIndex].m_CurrentStaminaMax = Npc[m_nIndex].m_StaminaMax;
	Npc[m_nIndex].m_CurrentManaMax    = Npc[m_nIndex].m_ManaMax;
	Npc[m_nIndex].m_CurrentNuQiMax    = Npc[m_nIndex].m_NuqiMax;  //怒气
	
	// 处理各种抗性的变化 火、冰、毒、电、物理
	Npc[m_nIndex].m_FireResist				= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentFireResist		= Npc[m_nIndex].m_FireResist;
	Npc[m_nIndex].m_ColdResist				= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentColdResist		= Npc[m_nIndex].m_ColdResist;
	Npc[m_nIndex].m_PoisonResist			= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPoisonResist		= Npc[m_nIndex].m_PoisonResist;
	Npc[m_nIndex].m_LightResist				= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentLightResist		= Npc[m_nIndex].m_LightResist;
	Npc[m_nIndex].m_PhysicsResist			= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_CurrentPhysicsResist	= Npc[m_nIndex].m_PhysicsResist;
	Npc[m_nIndex].m_FireResistMax			= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax			= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax			= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax			= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax		= BASE_PHYSICS_RESIST_MAX;
	/*
	Npc[m_nIndex].m_CurrentFireResistMax	= Npc[m_nIndex].m_FireResistMax;
	Npc[m_nIndex].m_CurrentColdResistMax	= Npc[m_nIndex].m_ColdResistMax;
	Npc[m_nIndex].m_CurrentPoisonResistMax	= Npc[m_nIndex].m_PoisonResistMax;
	Npc[m_nIndex].m_CurrentLightResistMax	= Npc[m_nIndex].m_LightResistMax;
	Npc[m_nIndex].m_CurrentPhysicsResistMax	= Npc[m_nIndex].m_PhysicsResistMax;
*/	
	int nOldCurCamp = Npc[m_nIndex].m_CurrentCamp;
	// 根据装备、技能信息更新当前数据
	this->UpdataCurData();
	SetNpcPhysicsDamage();
	Npc[m_nIndex].m_CurrentCamp    = nOldCurCamp;
	
	// 生命、体力、内力值加满
	Npc[m_nIndex].m_CurrentLife    = Npc[m_nIndex].m_CurrentLifeMax;
	Npc[m_nIndex].m_CurrentStamina = Npc[m_nIndex].m_CurrentStaminaMax;
	Npc[m_nIndex].m_CurrentMana    = Npc[m_nIndex].m_CurrentManaMax;
	
	// 客户端队伍中自己的等级加一
	if (this->m_cTeam.m_nFlag)
	{
		if (m_cTeam.m_nFigure == TEAM_CAPTAIN)
		{//队长
			g_Team[0].m_nMemLevel[0] = Npc[m_nIndex].m_Level;
		}
		else
		{
			for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
			{
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[m_nIndex].m_dwID)
				{
					g_Team[0].m_nMemLevel[i] = Npc[m_nIndex].m_Level;
					break;
				}
			}
		}
	}
	
	/*KSystemMessage Msg;	
	Msg.byConfirmType = SMCT_CLICK;
	Msg.byParamSize = 0;
	Msg.byPriority = 1;
	Msg.eType = SMT_PLAYER;
	t_sprintf(Msg.szMessage, strCoreInfo[MSG_LEVEL_UP].c_str(), Npc[m_nIndex].m_Level);
	Msg.nMsgLen = TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);*/
	
	Npc[this->m_nIndex].SetInstantSpr(enumINSTANT_STATE_LEVELUP);  //释放升级瞬间特效
}

void	KPlayer::s2cGetCurAttribute(BYTE* pMsg)
{
	PLAYER_ATTRIBUTE_SYNC	*pAttribute = (PLAYER_ATTRIBUTE_SYNC*)pMsg;
	m_nAttributePoint = pAttribute->m_nLeavePoint;
	int nData;
	switch (pAttribute->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH://力量
		nData = pAttribute->m_nBasePoint - m_nStrength;
		m_nStrength = pAttribute->m_nBasePoint;
		m_nCurStrength = pAttribute->m_nCurPoint;
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_DEXTERITY://身法
		nData = pAttribute->m_nBasePoint - m_nDexterity;
		m_nDexterity = pAttribute->m_nBasePoint;
		SetNpcAttackRating();
		SetNpcDefence();
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	case ATTRIBUTE_VITALITY://外功
		nData = pAttribute->m_nBasePoint - m_nVitality;
		m_nVitality = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseLifeMax(PlayerSet.m_cLevelAdd.GetLifePerVitality(Npc[m_nIndex].m_Series) * nData);
		Npc[m_nIndex].AddBaseStaminaMax(PlayerSet.m_cLevelAdd.GetStaminaPerVitality(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		break;
	case ATTRIBUTE_ENGERGY: //内功
		nData = pAttribute->m_nBasePoint - m_nEngergy;
		m_nEngergy = pAttribute->m_nBasePoint;
		Npc[m_nIndex].AddBaseManaMax(PlayerSet.m_cLevelAdd.GetManaPerEnergy(Npc[m_nIndex].m_Series) * nData);
		UpdataCurData();
		SetNpcPhysicsDamage();
		break;
	}
	//CoreDataChanged(GDCNI_PLAYER_RT_ATTRIBUTE, 0, 0);
	//CoreDataChanged(GDCNI_PLAYER_NEW_ATTRIBUTE, 0, 0);
}

void	KPlayer::s2cSetExp(int nExp)
{
	if (nExp > m_nExp)
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage,"您获得%d点经验值。"/*strCoreInfo[MSG_GET_EXP].c_str()*/, nExp - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
#ifdef WIN32
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		//CCMessageBox(sMsg.szMessage,"增加经验");
#else
		char msg[256];
		t_sprintf(sMsg.szMessage,UTEXT(sMsg.szMessage,1).c_str());
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		//CCMessageBox(msg,"增加经验");
#endif
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	}
	else if (nExp < m_nExp)
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, strCoreInfo[MSG_DEC_EXP].c_str(), m_nExp - nExp);//"提示:您损失了(%d)点经验值。"
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		//CCMessageBox("减少经验","减少经验");
	}
	
	this->m_nExp = nExp; //经验赋值
}

void	KPlayer::s2cSyncMoney(BYTE* pMsg)
{
	PLAYER_FS_MONEY_SYNC	*pMoney = (PLAYER_FS_MONEY_SYNC*)pMsg;
	
	if ( CheckTrading() )
	{
		if (pMoney->m_nMoney1 >= pMoney->m_nMoney3)
		{
			m_ItemList.SetClient(pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney2, pMoney->m_nMoney3);
		}
		else
		{
			m_ItemList.SetClient(0, pMoney->m_nMoney2 + pMoney->m_nMoney1 - pMoney->m_nMoney3, pMoney->m_nMoney3);
		}
		m_cTrade.m_nTradeState = 0;
		m_cTrade.m_nTradeDestState = 0;
		m_cTrade.m_nBackEquipMoney = pMoney->m_nMoney1;
		m_cTrade.m_nBackRepositoryMoney = pMoney->m_nMoney2;
	}
	else
	{
		// 捡钱一定是到room_equipment
		int nMoney1 = m_ItemList.GetsMon(room_equipment);
		if (pMoney->m_nMoney1 - nMoney1 > 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			t_sprintf(sMsg.szMessage, strCoreInfo[MSG_EARN_MONEY].c_str(), pMoney->m_nMoney1 - nMoney1);
			sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		m_ItemList.SetClient(pMoney->m_nMoney1, pMoney->m_nMoney2, pMoney->m_nMoney3);
	}
	m_Autoplay.nShiQuNum=0;
}
void	KPlayer::s2cSyncXu(BYTE* pMsg)
{
		PLAYER_FS_XU_SYNC	*pXu = (PLAYER_FS_XU_SYNC*)pMsg;
		int nXu = m_ItemList.GetxLient(room_equipment);
		if (pXu->m_nXu - nXu > 0)
		{
			KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			t_sprintf(sMsg.szMessage, strCoreInfo[MSG_EARN_XU].c_str(), pXu->m_nXu - nXu);
			sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		m_ItemList.SetServer(pXu->m_nXu);//设置客户端金币数量
}

//---------------------------------------------------------------------
//	功能：收到服务器通知有人申请交易
//---------------------------------------------------------------------
void	KPlayer::s2cTradeApplyStart(BYTE* pMsg)
{
	if (!pMsg)
		return;
	TRADE_APPLY_START_SYNC	*pApply = (TRADE_APPLY_START_SYNC*)pMsg;
	int		nNpcIdx;
	nNpcIdx = NpcSet.SearchID(pApply->m_dwNpcId);
	if (nNpcIdx == 0)
		return;
	
	KSystemMessage	sMsg;
	KUiPlayerItem	sPlayer;
	char nTempName[32];
	ZeroMemory(nTempName,sizeof(nTempName));
	t_sprintf(nTempName,Npc[nNpcIdx].Name);
	strcpy(sPlayer.Name, nTempName);
	sPlayer.nIndex = pApply->m_nDestIdx;
	sPlayer.uId    = pApply->m_dwNpcId;
	sPlayer.nData  = 0;
	
	t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TRADE_GET_APPLY].c_str(), nTempName);
	sMsg.eType = SMT_SYSTEM;
	sMsg.byConfirmType = SMCT_UI_TRADE;
	sMsg.byPriority = 3;
	sMsg.byParamSize = sizeof(KUiPlayerItem);
	sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
}

void	KPlayer::s2cTradeMoneySync(BYTE* pMsg)
{
	TRADE_MONEY_SYNC	*pMoney = (TRADE_MONEY_SYNC*)pMsg;
	
	m_ItemList.SetRoomMoney(room_trade1, pMoney->m_nMoney);
	
	// 通知界面
	KUiObjAtRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = pMoney->m_nMoney;
	CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (unsigned int)&sMoney, 0);
}

//---------------------------------------------------------------------
//	功能：收到服务器通知交易完成或取消
//---------------------------------------------------------------------
void	KPlayer::s2cTradeDecision(BYTE* pMsg)
{
	TRADE_DECISION_SYNC	*pSync = (TRADE_DECISION_SYNC*)pMsg;
	if (pSync->m_btDecision == 1)	// 完成交易
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TRADE_SUCCESS].c_str(), m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		// 交给ItemList.Remove来清除 modify by spe 03/06/14
		//m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		this->m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// 通知界面交易结束
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		
		//m_ItemList.MenuSetMouseItem();
	}
	else if (pSync->m_btDecision == 0)	// 取消交易
	{
		if (!CheckTrading())
			return;
		
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TRADE_FAIL].c_str(), m_cTrade.m_szDestName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		
		// 取消交易的数据恢复
		m_ItemList.RecoverTrade();
		m_ItemList.SetClient(m_cTrade.m_nBackEquipMoney, m_cTrade.m_nBackRepositoryMoney, 0);
		m_ItemList.ClearRoom(room_trade);
		m_ItemList.RemoveAllInOneRoom(room_trade1);
		//		m_ItemList.ClearRoom(room_trade1);
		m_ItemList.ClearRoom(room_tradeback);
		m_cTrade.Release();
		m_cMenuState.SetState(PLAYER_MENU_STATE_NORMAL);
		
		// 通知界面交易结束
		CoreDataChanged(GDCNI_TRADE_END, 0, 0);
		// 更新物品面版
		// CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED,UOC_ITEM_TAKE_WITH, 0);
		
		//m_ItemList.MenuSetMouseItem();
	}
}
//客户端  上线玩家参数属性
void	KPlayer::SyncCurPlayer(BYTE* pMsg)
{//丢包
	m_ItemList.RemoveAll();  //原来有的
	//CCMessageBox("上线同步人物","SyncCurPlayer");
	CURPLAYER_SYNC* PlaySync = (CURPLAYER_SYNC *)pMsg;

	this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID); //NPC的索引
	//char mgs[128]={0};
	if (this->m_nIndex<=0)
	{
		//t_sprintf(mgs,"数据错误.索引:%d,dwid:%u,等级:%d",this->m_nIndex,PlaySync->m_dwID,PlaySync->m_btLevel);
		//CCMessageBox(mgs,"SyncCurPlayer");
		return;
	}

	this->m_dwID          = g_FileName2Id(Npc[m_nIndex].Name); //稀哈码
	Npc[m_nIndex].m_Kind  = kind_player;
	Npc[m_nIndex].m_Level = (WORD)PlaySync->m_btLevel;
	Npc[m_nIndex].m_nSex  = PlaySync->m_btSex;
	Npc[m_nIndex].m_Series= PlaySync->m_btSeries;
	Npc[m_nIndex].SetPlayerIdx(CLIENT_PLAYER_INDEX); //=1
	
	m_nAttributePoint = PlaySync->m_wAttributePoint;
	m_nSkillPoint     = PlaySync->m_wSkillPoint;
	m_nStrength       = PlaySync->m_wStrength;
	m_nDexterity      = PlaySync->m_wDexterity;
	m_nVitality       = PlaySync->m_wVitality;
	m_nEngergy        = PlaySync->m_wEngergy;
	m_nLucky          = PlaySync->m_wLucky;
	m_nCurStrength    = m_nStrength;
	m_nCurDexterity   = m_nDexterity;
	m_nCurVitality    = m_nVitality;
	m_nCurEngergy     = m_nEngergy;
	m_nCurLucky       = m_nLucky;
	SetFirstDamage();   //设置基本伤害
	SetBaseAttackRating();
	SetBaseDefence();
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nExp               = PlaySync->m_nExp;
	m_nNextLevelExp      =PlayerSet.m_cLevelAdd.GetLevelExp(PlaySync->m_btLevel);
	m_dwLeadExp          =PlaySync->m_dwLeadExp;

	m_dwLeadLevel        =PlayerSet.m_cLeadExp.GetLevel(m_dwLeadExp);
	m_dwNextLevelLeadExp =PlayerSet.m_cLeadExp.GetLevelExp(m_dwLeadLevel);
	
	m_cFaction.m_nCurFaction = (char)PlaySync->m_btCurFaction;
	m_cFaction.m_nFirstAddFaction = (char)PlaySync->m_btFirstFaction;
	m_cFaction.m_nAddTimes = PlaySync->m_nFactionAddTimes;
	
	m_nWorldStat = (int)PlaySync->m_wWorldStat;//世界排名
	m_nSectStat = (int)PlaySync->m_wSectStat; //门派中的排名
	//char mgs[128]={0};
	//t_sprintf(mgs,"A名称:%s 索引:%d,dwid:%u,等级:%d",Npc[m_nIndex].Name,this->m_nIndex,PlaySync->m_dwID,PlaySync->m_btLevel);
	//CCMessageBox(mgs,"SyncCurPlayer");
	m_ItemList.Init(CLIENT_PLAYER_INDEX);
	m_ItemList.SetClient(PlaySync->m_nMoney1, PlaySync->m_nMoney2, 0);
	m_ItemList.SetServer(PlaySync->m_nXu);
	Npc[m_nIndex].m_LifeMax = PlaySync->m_wLifeMax;
	Npc[m_nIndex].m_ManaMax = PlaySync->m_wManaMax;
	Npc[m_nIndex].m_NuqiMax = PlaySync->m_wNuqiMax;  //怒气
	Npc[m_nIndex].m_StaminaMax = PlaySync->m_wStaminaMax;

	Npc[m_nIndex].m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	Npc[m_nIndex].m_ManaReplenish = PLAYER_MANA_REPLENISH;
	Npc[m_nIndex].m_StaminaGain   = PLAYER_STAMINA_GAIN;
	Npc[m_nIndex].m_StaminaLoss   = PLAYER_STAMINA_LOSS;
	SetBaseResistData();
	SetBaseSpeedAndRadius();
	Npc[m_nIndex].RestoreNpcBaseInfo();
	m_BuyInfo.Clear();
	m_cMenuState.Release();
//	m_cChat.Release();
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	memset(m_szTaskAnswerCanshu, 0, sizeof(m_szTaskAnswerCanshu));
	m_nAvailableAnswerNum = 0;

	Npc[m_nIndex].m_Experience = 0;
	memset(Npc[m_nIndex].m_szChatBuffer, 0, sizeof(Npc[m_nIndex].m_szChatBuffer));
	Npc[m_nIndex].m_nCurChatTime = 0;
	
	m_RunStatus = 1;
	m_nLeftSkillID = 0;
	m_nLeftSkillLevel = 0;
	m_nRightSkillID = 0;
	m_nRightSkillLevel = 0;
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	m_MouseDown[0] = 0;
	m_MouseDown[1] = 0;
	Npc[m_nIndex].m_SyncSignal = 0;

	//m_bIsPlayerInGame = true;//设置玩家已经在进入游戏状态
	/*this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID);
	this->m_dwID = g_FileName2Id(Npc[m_nIndex].Name); //稀哈码
	Npc[m_nIndex].m_Kind = kind_player;
	Npc[m_nIndex].m_Level = (WORD)PlaySync->m_btLevel;*/
	//char mgs[128]={0};
//	t_sprintf(mgs,"名称:%s 索引:%d,dwid:%u,等级:%d",Npc[m_nIndex].Name,this->m_nIndex,PlaySync->m_dwID,PlaySync->m_btLevel);
	//CCMessageBox(mgs,"SyncCurPlayer");
}


BOOL KPlayer::CheckTrading()
{
	return (m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADING);
}
//设置基本伤害
void	KPlayer::SetFirstDamage()
{
	int nDamageBase;

	nDamageBase = m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;   //力量除于 6 等 基本伤害
	
	Npc[m_nIndex].m_PhysicsDamage.nValue[0] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[2] = nDamageBase;
	Npc[m_nIndex].m_PhysicsDamage.nValue[1] = 0;

	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[2] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[1] = 0;

	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[2] = 0;
	Npc[m_nIndex].m_CurrentPhysicsMagicDamageP.nValue[1] = 0;
	
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2] = 0;
	
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0] = 0;
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[1] = 0;
	Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[2] = 0;
	
}

void	KPlayer::SetBaseAttackRating()
{
	Npc[m_nIndex].m_AttackRating = m_nDexterity * 4 - 28;
}

void	KPlayer::SetBaseDefence()
{
	Npc[m_nIndex].m_Defend = m_nDexterity >> 2;
}
//原始的五防
void	KPlayer::SetBaseResistData()
{
	Npc[m_nIndex].m_FireResist			= PlayerSet.m_cLevelAdd.GetFireResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_ColdResist			= PlayerSet.m_cLevelAdd.GetColdResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PoisonResist		= PlayerSet.m_cLevelAdd.GetPoisonResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_LightResist			= PlayerSet.m_cLevelAdd.GetLightResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_PhysicsResist		= PlayerSet.m_cLevelAdd.GetPhysicsResist(Npc[m_nIndex].m_Series, Npc[m_nIndex].m_Level);
	Npc[m_nIndex].m_FireResistMax		= BASE_FIRE_RESIST_MAX;
	Npc[m_nIndex].m_ColdResistMax		= BASE_COLD_RESIST_MAX;
	Npc[m_nIndex].m_PoisonResistMax		= BASE_POISON_RESIST_MAX;
	Npc[m_nIndex].m_LightResistMax		= BASE_LIGHT_RESIST_MAX;
	Npc[m_nIndex].m_PhysicsResistMax	= BASE_PHYSICS_RESIST_MAX;
}

void	KPlayer::SetBaseSpeedAndRadius()
{
	Npc[m_nIndex].m_WalkSpeed    = BASE_WALK_SPEED;
	Npc[m_nIndex].m_RunSpeed     = BASE_RUN_SPEED;
	Npc[m_nIndex].m_AttackSpeed  = BASE_ATTACK_SPEED;
	Npc[m_nIndex].m_CastSpeed    = BASE_CAST_SPEED;
	Npc[m_nIndex].m_VisionRadius = BASE_VISION_RADIUS;
	Npc[m_nIndex].m_HitRecover   = BASE_HIT_RECOVER;
}

//客户端版本   对话NPC 执行脚本
void KPlayer::DialogNpc(int nIndex,BOOL nIsServer)
{
	if (nIndex > 0 && Npc[nIndex].m_Index > 0)
	{
		//char nMgs[256]={0};
		//if (Npc[nIndex].GetstrInfo(STR_ACTION_SCRIPT,nMgs))
		if (!nIsServer)
		{
			//ExecuteScriptD(Npc[nIndex].m_ActionScriptID,"main",Npc[nIndex].Name,"",nIndex,Npc[nIndex].m_dwID,0,0,0,TRUE);
		}
		else
		{
			PLAYER_DIALOG_NPC_COMMAND DialogNpcCmd;
			DialogNpcCmd.nNpcId = Npc[nIndex].m_dwID;
			DialogNpcCmd.ProtocolType = c2s_dialognpc;		
		if (g_pClient)
			g_pClient->SendPackToServer(&DialogNpcCmd, sizeof(PLAYER_DIALOG_NPC_COMMAND));
			
		}
	}
}

void KPlayer::OpenBaiTan(DWORD nIndwId)
{
	g_cSellItem.ApplyViewItem(nIndwId);
}

void KPlayer::CheckObject(int nIdx)
{
/*	enum	// 物件类型
{
Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
Obj_Kind_Body,				// npc 的尸体
Obj_Kind_Box,				// 宝箱
Obj_Kind_Item,				// 掉在地上的装备
Obj_Kind_Money,				// 掉在地上的钱
Obj_Kind_LoopSound,			// 循环音效
Obj_Kind_RandSound,			// 随机音效
Obj_Kind_Light,				// 光源（3D模式中发光的东西）
Obj_Kind_Door,				// 门类
Obj_Kind_Trap,				// 陷阱
Obj_Kind_Prop,				// 小道具，可重生 储物箱
Obj_Kind_Num,				// 物件的种类数
};*/
	
	switch(Object[nIdx].m_nKind)
	{
//////原来没有的/////////////
	case Obj_Kind_Item:
		PickUpObj(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	case Obj_Kind_Money:
		PickUpObj(nIdx);
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	case Obj_Kind_Box:
	case Obj_Kind_Door:
	case Obj_Kind_Trap:  //陷阱类 
	case Obj_Kind_Prop:	 //储物箱等
		this->ObjMouseClick(nIdx);
		Npc[m_nIndex].m_nIsOver=TRUE;
		Npc[m_nIndex].m_nObjectIdx = 0;
		break;
	default:
		break;
	}
	m_nObjectIdx = 0;
	m_nPickObjectIdx = 0;
}

void KPlayer::DrawSelectInfo()   //选择人物后处理 的显示
{
	if (m_nIndex <= 0)
		return;

	if (m_nPeapleIdx)
	{//npc
		if (Npc[m_nPeapleIdx].m_Kind == kind_player) // 玩家
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_dialoger)  //对话者
		{
			if (!NpcSet.CheckShowName())
			{
			    Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else if (Npc[m_nPeapleIdx].m_Kind == kind_partner)  //同伴类
		{
			if (!NpcSet.CheckShowName())
			{
				Npc[m_nPeapleIdx].PaintInfo(Npc[m_nPeapleIdx].GetNpcPate(), true);
			}
		}
		else   //动物冒血处理
		{
			Npc[m_nPeapleIdx].DrawBlood();   //绘制血条  冒血处理
		}

		return;
	}
	if (m_nObjectIdx)
	{//物品
		if (!ObjSet.CheckShowName())
			Object[m_nObjectIdx].DrawInfo();
		//	Object[m_nObjectIdx].DrawBorder();
		return;
	}
}

//获取人物最大和最小最终伤害
//#ifndef _SERVER
void KPlayer::GetEchoDamage(int* nMin, int* nMax, int nType)
{
	int nSkillId;
	*nMin = 0;
	*nMax = 0;
	
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC || nType < 0 || nType > 1 || NULL == nMin || NULL == nMax)
		return;
	//左右技能选择
	nSkillId = (0 == nType)?m_nLeftSkillID:m_nRightSkillID;

	int nListidx  = Npc[m_nIndex].m_SkillList.FindSame(nSkillId);
	int nLevel    = Npc[m_nIndex].m_SkillList.GetCurrentLevelByIdx(nListidx);
	int nEnChance = Npc[m_nIndex].m_SkillList.GetEnChance(nListidx);

	if (nSkillId <= 0 ||nLevel <= 0 || nLevel >= MAX_SKILLLEVEL)
		return;
	
	KMagicAttrib*	pMagicData = NULL;
	
	KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nLevel);
	if (!pOrdinSkill) 
        return ;

	//CCMessageBox(pOrdinSkill->GetSkillName(),"setleftskill");
	switch(pOrdinSkill->GetSkillStyle())
	{
	case SKILL_SS_Missles:		        	//	子弹类		本技能用于发送子弹类
        break;
	case SKILL_SS_Melee:                    //
	    break;
	case SKILL_SS_InitiativeNpcState:	    //	主动类		本技能用于改变当前Npc的主动状态
	    break;
	case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
		break;
	default:
		return;
	}
	
	BOOL	bIsPhysical = pOrdinSkill->IsPhysical();          //物理攻击
	BOOL	bIsMagic    = pOrdinSkill->IsMagic();             //1外功系 或0内攻系

	        pMagicData = pOrdinSkill->GetDamageAttribs();     //攻击技能数据
	
	if (!pMagicData)
		return;
	//力量伤害+武器基本伤害+外普点=基本的伤害
	int nMinNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[0]+Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[0];
	int nMaxNpcDamage = Npc[m_nIndex].m_PhysicsDamage.nValue[2]+Npc[m_nIndex].m_CurrentPhysicsMagicDamageV.nValue[2];

	int nEnhancew=0;
	if (Npc[m_nIndex].IsPlayer())
	{//是人物
/*	   int nTempVal =m_cVip.GetRoleVipNum()*m_cVip.GetRoleVipDamageMaxVal();
		   nMinNpcDamage +=nTempVal;
		   nMaxNpcDamage +=nTempVal;
*/
	   if (equip_meleeweapon == m_ItemList.GetWeaponType())
	   { 
		nEnhancew = Npc[m_nIndex].m_CurrentMeleeEnhance[m_ItemList.GetWeaponParticular()];
	   } 
	   else if (equip_rangeweapon == m_ItemList.GetWeaponType())
	   {  
		nEnhancew = Npc[m_nIndex].m_CurrentRangeEnhance;
	   } 
	   else 
	   { 
		nEnhancew = Npc[m_nIndex].m_CurrentHandEnhance;
	   } 
	}
//基本伤害
	*nMin += nMinNpcDamage +(nMinNpcDamage*nEnhancew)/100;
	*nMax += nMaxNpcDamage +(nMinNpcDamage*nEnhancew)/100;

	if (magic_attackrating_v == pMagicData->nAttribType) //命中点
	{
	    Npc[m_nIndex].m_CurrentSkillMingZhong = Npc[m_nIndex].m_AttackRating+pMagicData->nValue[0];
	}
	else if (magic_attackrating_p == pMagicData->nAttribType)   //命中率百分比
	{
		Npc[m_nIndex].m_CurrentSkillMingZhong = Npc[m_nIndex].m_AttackRating*(100+pMagicData->nValue[0])/100;
	}
	else
	   Npc[m_nIndex].m_CurrentSkillMingZhong = 0;

	pMagicData++; //1 //闪避率
	if  (magic_ignoredefense_p== pMagicData->nAttribType)
	{

	}
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
		*nMin = (*nMin+Npc[m_nIndex].m_CurrentAddmagicphysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP)/100;
		*nMax = (*nMax+Npc[m_nIndex].m_CurrentAddmagicphysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP)/100;
	}
	else
	{//脚本基本数据+外普状态(装备+技能的状态)
		*nMin = (*nMin+Npc[m_nIndex].m_CurrentAddPhysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddPhysicsDamageP)/100;
		*nMax = (*nMax+Npc[m_nIndex].m_CurrentAddPhysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddPhysicsDamageP)/100;
	} 
	}
	pMagicData++; // 3 冰伤害
	if (magic_colddamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2];//*(100+ Npc[m_nIndex].m_CurrentColdEnhance)/100;	

	if (bIsMagic)  //内冰
	{
		*nMin += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddmagicColdDamagicv;
		*nMax += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddmagicColdDamagicv;
	}
	else
	{ 
		*nMin += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddColdDamagev;
		*nMax += Npc[m_nIndex].m_CurrentMagicColdDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddColdDamagev;
	} 
	}
	pMagicData++; //4  火伤害
	if (magic_firedamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2]*(100 + Npc[m_nIndex].m_CurrentFireEnhance)/ 100;
	if (bIsMagic)
	{//内功
		*nMin += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddmagicFireDamagicv;
		*nMax += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddmagicFireDamagicv;
	}
	else
	{//外功
		*nMin += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddFireDamagev;
		*nMax += Npc[m_nIndex].m_CurrentMagicFireDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddFireDamagev;
	} 
	}
	pMagicData++;// 5 雷伤害
	if (magic_lightingdamage_v == pMagicData->nAttribType)
	{
		*nMin += pMagicData->nValue[0];
		*nMax += pMagicData->nValue[2]; 

	if (bIsMagic)
	{
		*nMin += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddmagicLightDamagicv;
		*nMax += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddmagicLightDamagicv;
	}
	else
	{ 
		*nMin += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddLighDamagev;
		*nMax += Npc[m_nIndex].m_CurrentMagicLightDamage.nValue[2]+Npc[m_nIndex].m_CurrentAddLighDamagev;
	}
	}
	pMagicData++; // 6 毒伤害
	if (magic_poisondamage_v == pMagicData->nAttribType)
	{
			*nMin += pMagicData->nValue[0];//* (100 +Npc[m_nIndex].m_CurrentPoisonEnhance) / 100;//技能基本数据
			*nMax += pMagicData->nValue[0]; 

	  if (bIsMagic)
	  {//内毒  
		*nMin += Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddmagicPoisonDamagicv;
		*nMax += Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddmagicPoisonDamagicv;
	  }  
	  else
	  {//外毒  
		int nPoisonDamage = Npc[m_nIndex].m_CurrentMagicPoisonDamage.nValue[0]+Npc[m_nIndex].m_CurrentAddPoisonDamagev;
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
		*nMin += (pMagicData->nValue[0]+Npc[m_nIndex].m_CurrentAddmagicphysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP)/100;
		*nMax += (pMagicData->nValue[2]+Npc[m_nIndex].m_CurrentAddmagicphysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddmagicphysicsDamageP)/100;
	  }
	  else
	  {//脚本基本数据+外普状态(装备+技能的状态)
		*nMin += (pMagicData->nValue[0]+Npc[m_nIndex].m_CurrentAddPhysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddPhysicsDamageP)/100;
		*nMax += (pMagicData->nValue[2]+Npc[m_nIndex].m_CurrentAddPhysicsDamage)*(100+Npc[m_nIndex].m_CurrentAddPhysicsDamageP)/100;
	  } 
    }
	pMagicData++;//16
	pMagicData++;//17
	pMagicData++;//18
	
	int nCurbei=0;//nFomoidx=Npc[m_nIndex].GetCurFuMoIdx(),;
	
	/*if 	(nFomoidx>-1)
	{
		if (Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)>=5) //获取神将当前的等级
		{
			//nCurbei=Npc[m_nIndex].TakeTrader(Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx),5);//Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
			nCurbei=Npc[m_nIndex].GetCurFoMoSkllLevel(nFomoidx)/5;
		}

	}*/
  /*
	*nMin += Npc[m_nIndex].TakeTrader(*nMin * nEnChance,100);//*nMin * nEnChance/100;
	*nMax += Npc[m_nIndex].TakeTrader(*nMax * nEnChance,100);//*nMax * nEnChance/100; 

	*nMin = Npc[m_nIndex].TakeTrader(*nMin * (nCurbei*5+100),100);//*nMin * (nCurbei*5+100)/100;
	*nMax = Npc[m_nIndex].TakeTrader(*nMax * (nCurbei*5+100),100);//*nMax * (nCurbei*5+100)/100;
   */
	*nMin += *nMin * nEnChance/100;//Npc[m_nIndex].TakeTrader((*nMin)*nEnChance,100);//*nMin * nEnChance/100;
	*nMax += *nMax * nEnChance/100;//Npc[m_nIndex].TakeTrader((*nMax)*nEnChance,100);//*nMax * nEnChance/100; 

	*nMin = *nMin * (nCurbei*5+100)/100;//Npc[m_nIndex].TakeTrader((*nMin)*(nCurbei*5+100),100);//*nMin * (nCurbei*5+100)/100;
	*nMax = *nMax * (nCurbei*5+100)/100;//Npc[m_nIndex].TakeTrader((*nMax)*(nCurbei*5+100),100);//*nMax * (nCurbei*5+100)/100;
}
//#endif

//换装是调用函数设置物理伤害
void KPlayer::SetNpcDamageAttrib()
{
	SetNpcPhysicsDamage();
}
//设置跨服的 目标地图点

void KPlayer::SetExtGameTime(DWORD nPoint)
{
	m_GameLeftTime       = nPoint;
}

UINT KPlayer::GetExtPoint()
{
	return m_nExtPoint;  //扩展点 会员充值
}

// 物品相应的操作：
//     获得物品：1、从地上拣物品 2、脚本控制直接给 3、交易 4、player 之间赠
//               送(通过交易实现)
//         客户端鼠标点在物件上，然后客户端通过物件得出物品id、应该出现在装备栏或者
//         物品栏的位置或者跟随鼠标的计算，把计算结果发给服务器；服务器收到后首先判
//         断玩家与物品的位置关系，然后检查客户端的计算结果是否正确，然后进行相应的
//         处理，把处理结果发给客户端
//     物品的位置调整：客户端处理好物品来源位置、鼠标上物品目的位置，然后发给服务器，
//     服务器确认后通知客户端最终确定的操作（如果成功，把协议原样发回去）；如果有数
//     值等其他变化，另外通知客户端；
//     使用物品：1、吃药 2、装备（鼠标右键点击）
//         客户端向服务器端申请吃什么位置的药，同时客户端的相应数值先作相应变化，服
//         务器收到客户端申请后，处理完相应数据，通过player同步的方式通知客户端数据
//         的变化；
//         装备：鼠标右键点在一个装备上，自动处理装备上、卸下的位置信息，发给服务器，
//         服务器处理完后原样发回来；

//         交易的处理还没有
