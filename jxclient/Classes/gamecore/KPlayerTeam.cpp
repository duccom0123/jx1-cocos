//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTeam.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Team Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
//#ifdef _SERVER
//#include	"KNetServer.h"
//#include "../../Headers/IServer.h"
//#else
//#include	"KNetClient.h"
//#include "../../Headers/IClient.h"
#include	"CoreShell.h"

#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTeam.h"
//#include	"MyAssert.h"
#include "engine/Text.h"

enum
{
	Team_S_Close = 0,
	Team_S_Open,
};

KTeam		g_Team[MAX_TEAM];
KPlayerTeam::KPlayerTeam()
{
	Release();
}

void	KPlayerTeam::Release()
{
	m_nFlag = 0;
	m_nFigure = TEAM_CAPTAIN;
	m_nApplyCaptainID = -1;
	m_nApplyCaptainID = 0;
	m_dwApplyTimer = 0;
	m_bAutoRefuseInviteFlag = FALSE;	 // TRUE 自动拒绝   FALSE 手动
	ReleaseList();
}

void	KPlayerTeam::ReleaseList()
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
		m_sApplyList[i].Release();
}


BOOL	KPlayerTeam::ApplyCreate()//char *lpszTeamName)
{
//	if (!lpszTeamName || !lpszTeamName[0])
//		return FALSE;
//	if (strlen(lpszTeamName) >= 32)
//		return FALSE;
	if (m_nFlag)
		return FALSE;

	PLAYER_APPLY_CREATE_TEAM	sCreateTeam;
	sCreateTeam.ProtocolType = c2s_teamapplycreate;
//	memset(sCreateTeam.m_szTeamName, 0, sizeof(sCreateTeam.m_szTeamName));
//	strcpy(sCreateTeam.m_szTeamName, lpszTeamName);
	if (g_pClient)
		g_pClient->SendPackToServer(&sCreateTeam, sizeof(PLAYER_APPLY_CREATE_TEAM));

	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：邀请加入队伍
//---------------------------------------------------------------------------
void	KPlayerTeam::InviteAdd(DWORD dwNpcID)
{
	if (!this->m_nFlag || this->m_nFigure != TEAM_CAPTAIN || g_Team[0].m_nState != Team_S_Open)
		return;
	TEAM_INVITE_ADD_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_teaminviteadd;
	sAdd.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(TEAM_INVITE_ADD_COMMAND));
}

//---------------------------------------------------------------------------
//	功能：收到邀请
//---------------------------------------------------------------------------
void	KPlayerTeam::ReceiveInvite(TEAM_INVITE_ADD_SYNC *pInvite)
{
	if (!pInvite || pInvite==NULL)
		return;
	char	szName[32]={0};
	int		nIdx;
	memset(szName, 0, sizeof(szName));
	memcpy(szName, pInvite->m_szName, sizeof(pInvite->m_szName) - (sizeof(TEAM_INVITE_ADD_SYNC) - pInvite->m_wLength - 1));
	nIdx = pInvite->m_nIdx;

	if (m_bAutoRefuseInviteFlag)
	{
		ReplyInvite(nIdx, 0); //拒绝
		//if (uParam)
		//	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
	}
	else
	{
		// 通知界面有人邀请玩家加入某个队伍
		KUiPlayerItem	sPlayer;
		KSystemMessage	sMsg;

		strcpy(sPlayer.Name, szName);
		sPlayer.nIndex = pInvite->m_nIdx;
		sPlayer.uId = 0;
		sPlayer.nData = 0;
		t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TEAM_GET_INVITE].c_str(), szName);
		sMsg.eType         = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM_INVITE;
		sMsg.byPriority    = 3;
		sMsg.byParamSize   = sizeof(KUiPlayerItem);
		sMsg.nMsgLen       = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (unsigned int)&sPlayer);
	}
}

//---------------------------------------------------------------------------
//	功能：回复邀请
//	参数：nResult  if == 0 拒绝  if == 1 接受
//---------------------------------------------------------------------------
void	KPlayerTeam::ReplyInvite(int nIdx, int nResult)
{

	if (nIdx < 0 || nResult < 0 || nResult > 1)
		return;
	TEAM_REPLY_INVITE_COMMAND	sReply;
	sReply.ProtocolType = c2s_teamreplyinvite;
	sReply.m_nIndex     = nIdx;
	sReply.m_btResult   = nResult;
	if (g_pClient)
		g_pClient->SendPackToServer(&sReply, sizeof(TEAM_REPLY_INVITE_COMMAND));
}

//---------------------------------------------------------------------------
//	功能：设定是否自动拒绝别人的加入队伍的邀请
//---------------------------------------------------------------------------
void	KPlayerTeam::SetAutoRefuseInvite(BOOL bFlag)
{
	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;

	if (bFlag)
	{
		m_bAutoRefuseInviteFlag = TRUE;
		//t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TEAM_AUTO_REFUSE_INVITE].c_str());
	}
	else
	{
		m_bAutoRefuseInviteFlag = FALSE;
		//t_sprintf(sMsg.szMessage,  strCoreInfo[MSG_TEAM_NOT_AUTO_REFUSE_INVITE].c_str());
	}
    //sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

//---------------------------------------------------------------------------
//	功能：获得是否自动拒绝别人的加入队伍的邀请状态
//---------------------------------------------------------------------------
BOOL	KPlayerTeam::GetAutoRefuseState()
{
	return m_bAutoRefuseInviteFlag;
}

//---------------------------------------------------------------------------
//	功能：获得自身队伍信息（给界面）
//---------------------------------------------------------------------------
int		KPlayerTeam::GetInfo(KUiPlayerTeam *pTeam)
{
	if (!pTeam)
		return 0;
	pTeam->nCaptainPower = g_Team[0].CalcCaptainPower(); //队长能组多少人

	if (m_nFlag == 0)
		return 0;

	if (m_nFigure == TEAM_CAPTAIN)
	{
		pTeam->bTeamLeader = true;
		int nNo = 0;
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
		{
			if (this->m_sApplyList[i].m_dwNpcID > 0)
				nNo++;
		}
		pTeam->cNumTojoin = nNo;
	}
	else
	{
		pTeam->bTeamLeader = false;
		pTeam->cNumTojoin = 0;
	}
	pTeam->nTeamServerID = g_Team[0].m_nTeamServerID;
	pTeam->cNumMember = g_Team[0].m_nMemNum + 1;
	pTeam->bOpened = 0;
	if (g_Team[0].m_nState == Team_S_Open)
		pTeam->bOpened = 1;
	
	return 1;
}

//---------------------------------------------------------------------------
//	功能：更新界面显示
//---------------------------------------------------------------------------
void	KPlayerTeam::UpdateInterface()
{
	KUiPlayerTeam	sTeam;
	if (GetInfo(&sTeam) == 0)
	{
		CoreDataChanged(GDCNI_TEAM,NULL,0);
	}
	else
	{
		CoreDataChanged(GDCNI_TEAM,(unsigned int)&sTeam,0);
	}
}

//---------------------------------------------------------------------------
//	功能：循环更新的东西 活动更新更新界面显示
//---------------------------------------------------------------------------
void	KPlayerTeam::UpdateamUI()
{
	KUiPlayerTeam	sTeam;
	if (GetInfo(&sTeam) == 0)
	{
		//CoreDataChanged(GDCNI_TEAM_UI,NULL,0);  //更新队伍UI信息
	}
	else
	{
		//CoreDataChanged(GDCNI_TEAM_UI,(unsigned int)&sTeam,0);
	}
}

//---------------------------------------------------------------------------
//	功能：从申请人列表中删除某个申请人
//---------------------------------------------------------------------------
void	KPlayerTeam::DeleteOneFromApplyList(DWORD dwNpcID)
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; ++i)
	{
		if (m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_sApplyList[i].Release();
			return;
		}
	}
}

//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KTeam::KTeam()
{
	Release();
}

//---------------------------------------------------------------------------
//	功能：清空
//---------------------------------------------------------------------------
void	KTeam::Release()
{
	m_nCaptain = -1;
	m_nMemNum = 0;
	int		i;
	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		m_nMember[i] = -1;
	}
	m_nState = Team_S_Close;
	for (i = 0; i < MAX_TEAM_MEMBER + 1; ++i)
	{
		m_nMemLevel[i] = 0;
		memset(m_szMemName, 0, 32 * (MAX_TEAM_MEMBER + 1));
	}
	m_nTeamServerID = -1;
}

//---------------------------------------------------------------------------
//	功能：设定 Team 在 g_Team 中的位置
//---------------------------------------------------------------------------
void	KTeam::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
	Release();
}

//---------------------------------------------------------------------------
//	功能：寻找队员空位
//---------------------------------------------------------------------------
int		KTeam::FindFree()
{
	for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (m_nMember[i] < 0)
			return i;
	}
	return -1;
}

//---------------------------------------------------------------------------
//	功能：寻找具有指定npc id的队员（不包括队长）
//	返回值：队员在 m_nMember 数组中的位置
//---------------------------------------------------------------------------
int		KTeam::FindMemberID(DWORD dwNpcID)
{
	for (int i = 0; i <	MAX_TEAM_MEMBER; ++i)
	{
		if (m_nMember[i] > 0 && (DWORD)m_nMember[i] == dwNpcID)
			return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
//	功能：设定队伍状态：打开（允许接受新成员）
//---------------------------------------------------------------------------
BOOL	KTeam::SetTeamOpen()
{
	m_nState = Team_S_Open;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	//Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateamUI(); //显示主界面队伍信息
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：设定队伍状态：关闭（不允许接受新成员）
//---------------------------------------------------------------------------
BOOL	KTeam::SetTeamClose()
{
	m_nState = Team_S_Close;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：计算队长能统帅队员的人数
//---------------------------------------------------------------------------
int	KTeam::CalcCaptainPower()
{
	return PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
}

//---------------------------------------------------------------------------
//	功能：客户端创建一支队伍（客户端只可能存在一支的队伍，属于本地玩家）
//---------------------------------------------------------------------------
void	KTeam::CreateTeam(int nCaptainNpcID, char *lpszCaptainName, int nCaptainLevel, DWORD nTeamServerID)
{
	Release();
	m_nCaptain = nCaptainNpcID;
	m_nMemLevel[0] = nCaptainLevel;
	strcpy(m_szMemName[0], lpszCaptainName);
	m_nTeamServerID = nTeamServerID;		// 队伍在服务器上的唯一标识
}

//---------------------------------------------------------------------------
//	功能：添加一个队伍成员
//---------------------------------------------------------------------------
BOOL	KTeam::AddMember(DWORD dwNpcID, int nLevel, char *lpszNpcName)
{
	for (int i = 0; i < MAX_TEAM_MEMBER;++i)
	{
		if (m_nMember[i] == (int)dwNpcID)
			return TRUE;
	}

	int nFreeNo;
	nFreeNo = FindFree();
	if (nFreeNo < 0)
		return FALSE;
	m_nMember[nFreeNo] = dwNpcID;
	m_nMemLevel[nFreeNo + 1] = nLevel;
	strcpy(m_szMemName[nFreeNo + 1], lpszNpcName);
	m_nMemNum++;

	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：客户端删除一个队伍成员
//---------------------------------------------------------------------------
void	KTeam::DeleteMember(DWORD dwNpcID)
{
	if (dwNpcID == (DWORD)m_nCaptain)
	{
		Release();
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if ((DWORD)m_nMember[i] == dwNpcID)
		{
			m_nMemNum--;
			m_nMember[i] = -1;
			m_nMemLevel[i + 1] = 0;
			m_szMemName[i + 1][0] = 0;
			return;
		}
	}
}

//---------------------------------------------------------------------------
//	功能：输出队员信息(按界面的要求，给界面)
//---------------------------------------------------------------------------
int		KTeam::GetMemberInfo(KUiPlayerItem *pList, int nCount)
{
	if (this->m_nCaptain < 0)
		return 0;
	int		i, nNum = 0;
	if (!pList || nCount <= m_nMemNum)
	{
		return m_nMemNum + 1;
	}
	
	// 队长名字
	strcpy(pList[nNum].Name, m_szMemName[0]);
	pList[nNum].uId = this->m_nCaptain;
	pList[nNum].nIndex = 0;
	nNum++;

	for (i = 0; i < MAX_TEAM_MEMBER; ++i)
	{
		if (nNum >= nCount)
			break;
		if (this->m_nMember[i] <= 0)
			continue;
		strcpy(pList[nNum].Name, m_szMemName[i + 1]);
		pList[nNum].uId = m_nMember[i];
		pList[nNum].nIndex = 0;
		nNum++;
	}

	return nNum;
}
/*
#ifdef _SERVER
//---------------------------------------------------------------------------
//	功能：判断队名是否可用
//---------------------------------------------------------------------------
BOOL	KTeamSet::CheckName(char *lpszName)
{
	if (!lpszName || !lpszName[0])
		return FALSE;

	for (int i = 0; i < MAX_TEAM; i++)
	{
		if (g_Team[i].m_nCaptain < 0)
			continue;
		if (strcmp(lpszName, g_Team[i].m_szName) == 0)
			return FALSE;
	}

	return TRUE;
}
#endif
*/