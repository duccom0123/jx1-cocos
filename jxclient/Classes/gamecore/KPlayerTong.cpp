//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.cpp
// Date:	2003.08.12
// Code:	边城浪子
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTong.h"
#include    "engine/Text.h"
//#include "../../Engine/src/KFilePath.h"
#include     "engine/KFilePath.h"
#include     "engine/KTabFile.h"
#include     "engine/KTabFileCtrl.h"

#include "KProtocolDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"
#include	"CoreShell.h"

#define     TONG_DATA_FILE             "\\TongSet\\"
#define     TONG_SET_FILE             "\\TongSet\\TongSet.ini"
//#define		defTONG_NAME_LENGTH			8
#define		defFuncShowNormalMsg(str,nKind)		\
	{										\
		KSystemMessage	sMsg;				\
		sMsg.eType = SMT_NORMAL;			\
		sMsg.byConfirmType = SMCT_NONE;		\
		sMsg.byPriority = 0;				\
		sMsg.byParamSize = 0;				\
		t_sprintf(sMsg.szMessage, str);		\
		if (nKind==0)\
		   sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));\
		else\
		   sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));\
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);\
	}

//-------------------------------------------------------------------------
//	功能：初始化
//-------------------------------------------------------------------------
void	KPlayerTong::Init(int nPlayerIdx)
{
	nAttAckInfo.clear();
	m_nPlayerIndex = nPlayerIdx;

	ActiveLoop=0;

	Clear();
}

//-------------------------------------------------------------------------
//	功能：清空
//-------------------------------------------------------------------------
void	KPlayerTong::Clear()
{
	m_nFlag				= 0;
	m_nFigure			= enumTONG_FIGURE_MEMBER;
	m_nCamp				= 0;
	m_dwTongNameID		= 0;
	m_szName[0]			= 0;
	m_szTitle[0]		= 0;
	m_BMasterName[0]	= 0;
	m_nApplyTo			= 0;
	m_nMoney			= 0;
	m_AttackState       = 0;
	m_AttackCamp        = 0;
	m_nDeathCount       = 0;
    m_AttackTime        = 0;
    m_AttackName[0]     = 0;
	m_Mapid             = 0;
	m_CurRenShu			= 0;
	m_nLevel            = 1;

	m_nAttackNum		= 0;	                            // 参战场数
    m_nWinNum		    = 0;								// 胜利场数
    m_nLoseNum		    = 0;
    t_sprintf(m_AttackName,"暂无");
}

//-------------------------------------------------------------------------
//	功能：申请创建帮会
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyCreateTong(int nCamp, char *lpszTongName)
{
	defFuncShowNormalMsg(strCoreInfo[MSG_TONG_APPLY_CREATE].c_str(),1);
	// 帮会名问题
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR01].c_str(),1);
		return FALSE;
	}
	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR06].c_str(),1);
		return FALSE;
	}
	// 已经是帮会成员
	if (m_nFlag)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR05].c_str(),1);
		return FALSE;
	}
	// 自己的阵营问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR04].c_str(),1);
		return FALSE;
	}
	// 等级问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR07].c_str(),1);
		return FALSE;
	}
	if ((int)Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR06].c_str(),1);
		return FALSE;
	}
	// 交易过程中
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// 钱问题
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoneyAmount() < PlayerSet.m_sTongParam.m_nMoney)
	{
		char	szBuf[80];
		t_sprintf(szBuf, strCoreInfo[MSG_TONG_CREATE_ERROR08].c_str(), PlayerSet.m_sTongParam.m_nMoney);
		defFuncShowNormalMsg(szBuf,1);
		return FALSE;
	}
	// 组队不能建帮会
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_ERROR09].c_str(),1);
		return FALSE;
	}

	TONG_APPLY_CREATE_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_CREATE_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CREATE;
	sApply.m_btCamp = (BYTE)nCamp;
	strcpy(sApply.m_szName, lpszTongName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请加入帮会
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyAddTong(DWORD dwNpcID)
{
	defFuncShowNormalMsg(strCoreInfo[MSG_TONG_APPLY_ADD].c_str(),1);
	// 已经是帮会成员
	if (m_nFlag)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_APPLY_ADD_ERROR1].c_str(),1);
		return FALSE;
	}
	// 自己的阵营问题
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_APPLY_ADD_ERROR2].c_str(),1);
		return FALSE;
	}
	// 交易过程中
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// 组队不能建帮会
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_APPLY_ADD_ERROR3].c_str(),1);
		return FALSE;
	}

	TONG_APPLY_ADD_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_ADD_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_ADD;
	sApply.m_dwNpcID = dwNpcID;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TONG_APPLY_ADD_COMMAND));

	return TRUE;
}

//客户端
//-------------------------------------------------------------------------
//	功能：得到服务器通知创建帮会
//-------------------------------------------------------------------------
void	KPlayerTong::Create(TONG_CREATE_SYNC *psCreate)
{
	if (!psCreate)
		return;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = psCreate->m_btCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = psCreate->m_btCamp;

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= psCreate->m_btCamp;
	m_AttackState   = 0;
	m_szTitle[0]	= 0;
	m_AttackCamp        = 0;
    m_AttackTime        = 0;
    m_AttackName[0]     = 0;
	m_nAttackNum		= 0;	                            // 参战场数
    m_nWinNum		    = 0;								// 胜利场数
    m_nLoseNum		    = 0;

	memset(m_szName, 0, sizeof(m_szName)); //帮会名
	memcpy(m_szName, psCreate->m_szName, sizeof(psCreate->m_szName));
	strcpy(m_BMasterName,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name)
	//strcpy(m_szMasterName,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);//Npc[P
	m_dwTongNameID	= g_FileName2Id(psCreate->m_szName);

	// 界面通知帮会建立成功
	defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CREATE_SUCCESS].c_str(),1);

	// 通知帮会频道 更新基本信息
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

}

void KPlayerTong::Active()
{
	
/*	ActiveLoop++;
	//if (!m_nFlag)
	//	return;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		//return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		//defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		//return FALSE;
	}

   if (ActiveLoop>18 && ActiveLoop%18==0)
   {
	TONG_APPLY_GET_ATTACK	sAttackGet;
	sAttackGet.ProtocolType		= c2s_extendtong;
	sAttackGet.m_wLength		= sizeof(sAttackGet) - 1;
	sAttackGet.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_ATTACK_GET;
	if (m_nFlag)
	   sAttackGet.m_dwTongNameID= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID; //g_FileName2Id(this->m_szName);
	else
	   sAttackGet.m_dwTongNameID= 0;

	sAttackGet.m_NpcDwid        = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAttackGet, sAttackGet.m_wLength + 1);
   }

   if (ActiveLoop>30000000)
	   ActiveLoop=0; */
}

void KPlayerTong::SendAttack(char *lpszTongName)
{	
	if (!m_nFlag)
	{
		defFuncShowNormalMsg("提示:只有 加入帮派 才可以发起帮会宣战!",0);
		return;
	}
	if (m_nFigure== enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		TONG_APPLY_SEND_ATTACK	sAttackSend;
		sAttackSend.ProtocolType	= c2s_extendtong;
		sAttackSend.m_wLength		= sizeof(sAttackSend) - 1;
		sAttackSend.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_ATTACK_SEND;
		sAttackSend.m_dwYTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;        //g_FileName2Id(this->m_szName);
		sAttackSend.m_dwDTongNameID = g_FileName2Id(lpszTongName);
		sAttackSend.m_NpcDwid       = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sAttackSend, sAttackSend.m_wLength + 1);
	}
    else
	{
		defFuncShowNormalMsg("提示:只有 帮主 或 长老 才可以发起帮会宣战!",0);
	}
	return;
}

void    KPlayerTong::SetTongLevel(char *lpszTongName,int nLevel)
{

}

void    KPlayerTong::SetTongMap(char *lpszTongName,int nMap)
{	
	
}

void	KPlayerTong::SetTongCredit(char *lpszTongName,int nCredit)        //建设资金
{

}
void	KPlayerTong::SetTongMasterTitle(char *lpszTongName,char *nTitle)  //帮主称号
{

}
void	KPlayerTong::SetTongDirectorTitle(char *lpszTongName,char *nTitle)//长老称号
{

}
void	KPlayerTong::SetTongManagerTitle(char *lpszTongName,char *nTitle)//队长称号
{

}
void	KPlayerTong::SetTongNormalTitle(char *lpszTongName,char *nTitle)  //帮众称号
{

}

//-------------------------------------------------------------------------
//	功能：是否接受成员 bFlag == TRUE 接受 == FALSE 不接受
//-------------------------------------------------------------------------
void	KPlayerTong::AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag)
{
	if (nPlayerIdx <= 0)
		return;

	TONG_ACCEPT_MEMBER_COMMAND	sAccept;
	sAccept.ProtocolType	= c2s_extendtong;
	sAccept.m_wLength		= sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1;
	sAccept.m_btMsgId		= enumTONG_COMMAND_ID_ACCEPT_ADD;
	sAccept.m_nPlayerIdx	= nPlayerIdx;   // 申请的Playerid
	sAccept.m_dwNameID		= dwNameID;     // 申请人的名字ID  Npc[Player[nPlayerIdx].m_nIndex].m_dwID;//
	sAccept.m_btFlag		= (bFlag != 0);

	if (g_pClient)
		g_pClient->SendPackToServer(&sAccept, sAccept.m_wLength + 1);
}


void	KPlayerTong::GetTongName(char *lpszGetName)
{
	//if (!lpszGetName)
	//	return;
	if (!m_nFlag)
	{
		lpszGetName[0] = 0;
		return;
	}

	strcpy(lpszGetName, m_szName);
}

//-------------------------------------------------------------------------
//	功能：加入帮会，成为普通帮众
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char *lpszTongName, char *lpszTitle, char *lpszMaster)
{
	if (!lpszTongName || !lpszTongName[0]/* || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH*/)
		return FALSE;
	// 帮会阵营问题
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	strcpy(m_szName, lpszTongName);
	if (!lpszTitle)
		this->m_szTitle[0] = 0;
	else
		strcpy(m_szTitle, lpszTitle);
	if (!lpszMaster)
		this->m_BMasterName[0] = 0;
	else
		strcpy(m_BMasterName, lpszMaster);
	m_dwTongNameID	= g_FileName2Id(lpszTongName);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;

	defFuncShowNormalMsg(strCoreInfo[MSG_TONG_ADD_SUCCESS].c_str(),1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请任命
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure < 0 || nCurFigure >= enumTONG_FIGURE_NUM ||
		nNewFigure < 0 || nNewFigure >= enumTONG_FIGURE_NUM)
		return FALSE;
	if (nCurPos < 0 || nNewPos < 0)
		return FALSE;
	if (nCurFigure == nNewFigure && nCurPos == nNewPos)
		return FALSE;

	switch (m_nFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		// 帮众没有任命权力
		return FALSE;
	case enumTONG_FIGURE_MANAGER:
		// 队长没有任命权力
		return FALSE;
	case enumTONG_FIGURE_DIRECTOR:
		// 长老只有对队长、帮众的任命权力
		if ((nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER) ||
			(nNewFigure != enumTONG_FIGURE_MANAGER && nNewFigure != enumTONG_FIGURE_MEMBER))
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		break;
	case enumTONG_FIGURE_MASTER:
		// 帮主有对长老、队长、帮众的任命权力
		if (nCurFigure == enumTONG_FIGURE_MASTER || nNewFigure == enumTONG_FIGURE_MASTER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_DIRECTOR && nCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_DIRECTOR && nNewPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		break;
	default:
		return FALSE;
	}

	TONG_APPLY_INSTATE_COMMAND	sApply;

	if (strlen(lpszName) >= sizeof(sApply.m_szName))
		return FALSE;
	sApply.ProtocolType   = c2s_extendtong;
	sApply.m_btMsgId      = enumTONG_COMMAND_ID_APPLY_INSTATE;
	sApply.m_dwTongNameID = g_FileName2Id(this->m_szName);
	sApply.m_btCurFigure  = nCurFigure;
	sApply.m_btCurPos     = nCurPos;
	sApply.m_btNewFigure  = nNewFigure;
	sApply.m_btNewPos     = nNewPos;
	memset(sApply.m_szName, 0, sizeof(sApply.m_szName));
	strcpy(sApply.m_szName, lpszName);
	sApply.m_wLength      = sizeof(sApply) - 1;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请踢人
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyKick(int nCurFigure, int nCurPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;

	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)   //执行踢人的职位
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能进行踢人操作!");
		return FALSE;
	}

	if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)  //被踢的人的职位
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能对长老进行操作 !");
		return FALSE;
	}

	if (nCurFigure == enumTONG_FIGURE_MANAGER /*&& (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER)*/)
	{
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:不能对队长进行操作 !");
		return FALSE;//被踢人的职位
	}

	TONG_APPLY_KICK_COMMAND	sKick;
	sKick.ProtocolType		= c2s_extendtong;
	sKick.m_wLength			= sizeof(sKick) - 1;
	sKick.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_KICK;
	sKick.m_btFigure		= nCurFigure;
	sKick.m_btPos			= nCurPos;
	sKick.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sKick.m_szName, lpszName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sKick, sKick.m_wLength + 1);
	char msg[64];
	t_sprintf(msg,"提示:已将(%s),踢出帮会!",lpszName);
    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请传位
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;

	TONG_APPLY_CHANGE_MASTER_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nPos;
	strcpy(sChange.m_szName, lpszName);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请离开帮会 叛帮
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyLeave()
{
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CANNOT_LEAVE1].c_str(),1);
		return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		defFuncShowNormalMsg(strCoreInfo[MSG_TONG_CANNOT_LEAVE2].c_str(),1);
		return FALSE;
	}

	TONG_APPLY_LEAVE_COMMAND	sLeave;
	sLeave.ProtocolType		= c2s_extendtong;
	sLeave.m_wLength		= sizeof(sLeave) - 1;
	sLeave.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_LEAVE;
	sLeave.m_btFigure		= m_nFigure;
	sLeave.m_btPos			= 0;
	sLeave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sLeave.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	if (g_pClient)
		g_pClient->SendPackToServer(&sLeave, sLeave.m_wLength + 1);

	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：申请获得帮会信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName/*=NULL*/)
{
	if (nInfoID < 0 || nInfoID >= enumTONG_APPLY_INFO_ID_NUM)
		return FALSE;

	TONG_APPLY_INFO_COMMAND	sInfo;
	sInfo.ProtocolType = c2s_extendtong;
	sInfo.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INFO;
	sInfo.m_btInfoID = nInfoID;
	sInfo.m_nParam1 = nParam1;	  //TONGID
	sInfo.m_nParam2 = nParam2;	  //开始位置
	sInfo.m_nParam3 = nParam3;    //每次获取数量

	switch (nInfoID)
	{
	case enumTONG_APPLY_INFO_ID_SELF:  //自己查询自己的帮会信息
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)   //gs
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;

	case enumTONG_APPLY_INFO_ID_MASTER: //帮主
		break;

	case enumTONG_APPLY_INFO_ID_DIRECTOR: //长老
		break;

	case enumTONG_APPLY_INFO_ID_MANAGER:  // 队长
		if (!m_nFlag)
			break;
		/*if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR)
			break;*///只能是帮主和长老才更新

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;
	case enumTONG_APPLY_INFO_ID_LIST:
		{
			sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
			if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		}
		break;
	case enumTONG_APPLY_INFO_ID_MEMBER:   //帮众
		if (!m_nFlag)
			break;
		/*if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR)
			break;*///只能是帮主和长老才更新
            
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_ONE:  //某人帮会成员的信息
		if (!lpszName || !lpszName[0])
			break;
		if (strlen(lpszName) >= 32)
			break;
		strcpy(sInfo.m_szBuf, lpszName);
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf) + strlen(lpszName);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	case enumTONG_APPLY_INFO_ID_TONG_HEAD:  //获取帮主，长老等信息
		if (nParam1 <= 0 || nParam1 >= MAX_NPC)
			break;
		if (nParam1 != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nParam1].m_nTongFlag == 0)
			break;   
		sInfo.m_nParam1 = Npc[nParam1].m_dwID;
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	}
	return TRUE;
}

DWORD	KPlayerTong::GetTongNameID()
{
///	return (m_nFlag ? m_dwTongNameID : 0);
	return m_dwTongNameID;
//	if (!m_nFlag)
//		return 0;
//	return g_FileName2Id(m_szName);
}

//-------------------------------------------------------------------------
//	功能：是否有权力查询队长信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetManagerInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：是否有权力查询帮众信息
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetMemberInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
	return TRUE;
}
//服务器端 同步给客户端
#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定自身帮会信息
//-------------------------------------------------------------------------
void	KPlayerTong::SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo)
{
	if (pInfo->m_btJoinFlag == 0)
	{//没有入帮的
		if (m_nFlag)
		{//客户端原来是 帮会成员
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_LEAVE;
			sUi.nParam = m_nFigure;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);

			// 被踢出帮会
			defFuncShowNormalMsg("提示:您被踢出帮会!",0);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = camp_free;
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = camp_free;
		}

		Clear();

		CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

		return;
	}

	if (m_nFlag == 1 && m_nFigure != pInfo->m_btFigure)
	{	// 被任命
		defFuncShowNormalMsg("提示:您在帮派中职位发生变化!",0);
	}

	this->m_nFlag = 1;                   //客户端是否入帮
	this->m_nFigure = pInfo->m_btFigure; //客户端帮派的身份
	this->m_nCamp = pInfo->m_btCamp;     //客户端帮派的阵营
	this->m_nMoney = pInfo->m_dwMoney;   //客户单帮派的钱
	this->m_AttackState= pInfo->m_nAttackState;	  //帮会的宣战状态
	this->m_nDeathCount= pInfo->m_nDeathCount;
	this->m_nLevel=pInfo->m_nLevel;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;

	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag == 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;

	memcpy(this->m_BMasterName, pInfo->m_szMaster, sizeof(pInfo->m_szMaster)); //重新设置了帮主的名字
	memcpy(this->m_szName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));  //帮会名
	memcpy(this->m_szTitle, pInfo->m_szTitle, sizeof(pInfo->m_szTitle));       //帮会中的职位
	m_dwTongNameID	= g_FileName2Id(pInfo->m_szTongName);                      //帮会的代码
    //更新界面信息
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}
#endif

//打开建帮UI
void    KPlayerTong::OpenCreateInterface()
{
	CoreDataChanged(GDCNI_OPEN_TONG_CREATE_SHEET, 1, 0);
}
//帮户存钱
BOOL KPlayerTong::ApplySaveMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment) < nMoney)
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage,"您身上没有那么多钱呢!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_SAVE;
	sSave.m_dwTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(Player[CLIENT_PLAYER_INDEX].m_cTong.m_szName);  //this->m_szName
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
//帮会取钱
BOOL KPlayerTong::ApplyGetMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (this->m_nMoney < nMoney)
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, "提示:金钱不足!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
        sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_GET;
	sSave.m_dwTongNameID	=  Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}

//客户端
void    KPlayerTong::SetAttAckCityInfo(BYTE *Msg)
{ 
	  CTONG_CITY_INFO_SYNC	*pAinfo = (CTONG_CITY_INFO_SYNC*)Msg;

	  for (int i =1;i<pAinfo->m_WarCityCount+1;++i)
	  {  

		  nAttAckInfo[i].m_idx = pAinfo->snWarInfo[i-1].m_idx; 
		  nAttAckInfo[i].m_levle=pAinfo->snWarInfo[i-1].m_levle;
		  nAttAckInfo[i].m_mapidx=pAinfo->snWarInfo[i-1].m_mapidx;
		  nAttAckInfo[i].m_shushou=pAinfo->snWarInfo[i-1].m_shushou;
		  nAttAckInfo[i].m_Mastername = pAinfo->snWarInfo[i-1].m_Mastername;
		  nAttAckInfo[i].m_szMapName = pAinfo->snWarInfo[i-1].m_szMapName; //地图名
		  nAttAckInfo[i].m_Tongmaster = pAinfo->snWarInfo[i-1].m_Tongmaster;

		/*AttAck[i].m_idx=pAinfo->snWarInfo[i].m_idx;
		AttAck[i].m_levle=pAinfo->snWarInfo[i].m_levle;
		AttAck[i].m_mapidx=pAinfo->snWarInfo[i].m_mapidx;
		AttAck[i].m_shushou=pAinfo->snWarInfo[i].m_shushou;
		sprintf(AttAck[i].m_Mastername,pAinfo->snWarInfo[i].m_Mastername);
		sprintf(AttAck[i].m_szMapName,pAinfo->snWarInfo[i].m_szMapName); //地图名
		sprintf(AttAck[i].m_Tongmaster,pAinfo->snWarInfo[i].m_Tongmaster);
		*/
		// Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(AttAck[i].m_Tongmaster);
		/*if (m_nFlag && strstr(m_szName,AttAck[i].m_Tongmaster)) //如果入帮了
		   //如果帮会相同,就是属于这个城市的成员
		  Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=AttAck[i].m_mapidx;
		else
		  Npc[Player[m_nPlayerIndex].m_nIndex].m_IsWarCity=-1; */

	  }  	
}


void    KPlayerTong::SetAttAckInfo(BYTE *Msg)
{
	CTONG_ATTTACK_INFO_SYNC	*pAinfo = (CTONG_ATTTACK_INFO_SYNC*)Msg;
   	/*
	for (int i =0;i<pAinfo->m_WarCityCount;i++)
	{
		AttAck[i].m_idx=pAinfo->snWarInfo[i].m_idx;
		AttAck[i].m_levle=pAinfo->snWarInfo[i].m_levle;
		AttAck[i].m_mapidx=pAinfo->snWarInfo[i].m_mapidx;
		AttAck[i].m_shushou=pAinfo->snWarInfo[i].m_shushou;
		sprintf(AttAck[i].m_Mastername,pAinfo->snWarInfo[i].m_Mastername);
		sprintf(AttAck[i].m_szMapName,pAinfo->snWarInfo[i].m_szMapName);
		sprintf(AttAck[i].m_Tongmaster,pAinfo->snWarInfo[i].m_Tongmaster);
	}
	*/
	m_AttackState=pAinfo->m_AttackState;
	m_AttackCamp=pAinfo->m_nTempCamp;
	m_AttackTime=pAinfo->m_AttackTime;
	m_nLevel=pAinfo->m_nLevel;
	m_nDeathCount=pAinfo->m_YDeathCount;
	m_nDDeathCount=pAinfo->m_DDeathCount;
	m_Mapid= pAinfo->m_nMapidx;
	m_nAttackNum=pAinfo->m_nAttackNum;	                            // 参战场数
	m_nWinNum=pAinfo->m_nWinNum;								// 胜利场数
	m_nLoseNum=pAinfo->m_nLoseNum;
	t_sprintf(m_AttackName,pAinfo->m_szAttackName); //对战的帮会

	if (pAinfo->m_nMapidx>0)
	{//客户端 帮会地图的设置
		//sprintf(SubWorld[pAinfo->m_nMapidx].m_WarSubTongname,m_szName);
		SubWorld[pAinfo->m_nMapidx].nWorldMapInfo[STR_MAP_ZTONGNAME] = m_szName;
	}
}

//帮会发钱
BOOL KPlayerTong::ApplySndMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (this->m_nMoney < nMoney)
	{
		KSystemMessage	sMsg;
		t_sprintf(sMsg.szMessage, "提示:金钱不足!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_SND;
	sSave.m_dwTongNameID	= Player[CLIENT_PLAYER_INDEX].m_cTong.m_dwTongNameID;//g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}