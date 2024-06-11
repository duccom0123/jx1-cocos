//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerMenuState.cpp
// Date:	2002.12.10
// Code:	边城浪子
// Desc:	PlayerMenuState Class
//---------------------------------------------------------------------------

#include	"KCore.h"
//#include	"MyAssert.H"
#include	"KPlayer.h"
#ifdef _SERVER
//#include	"KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif
#include	"KPlayerMenuState.h"
#include	"CoreUseNameDef.h"

KPlayerMenuState::KPlayerMenuState()
{
	Release();
}

void	KPlayerMenuState::Release()
{
	m_nState = PLAYER_MENU_STATE_NORMAL;
	m_nTradeDest = -1;
	m_nTradeState = 0;
	m_nTradeDestState = 0;
	memset(m_szSentence, 0, MAX_SENTENCE_LENGTH);
}

void	KPlayerMenuState::SetState(int nState)
{
	if (nState < PLAYER_MENU_STATE_NORMAL || nState >= PLAYER_MENU_STATE_NUM)
		return;
	m_nState = nState;
}

//初始化 玩家系统状态文件
BOOL	KPlayerMenuStateGraph::Init()
{
	KTabFile	cFile;
//	g_SetFilePath("\\");
	if( !cFile.Load(PLAYER_MENU_STATE_RES_FILE) )
		return FALSE;

	m_szName[0][0] = 0;
	for (int i = 1; i < PLAYER_MENU_STATE_NUM; ++i)
	{
		cFile.GetString(i + 1, 2, "", m_szName[i], sizeof(m_szName[i]));
	}
	cFile.Clear();
	return TRUE;
}

void	KPlayerMenuStateGraph::GetStateSpr(int nState, char *lpszGetName)
{
	if ( !lpszGetName )
		return;
	if (nState < 0 || nState >= PLAYER_MENU_STATE_NUM)
	{
		lpszGetName[0] = 0;
		return;
	}
	g_StrCpy(lpszGetName, m_szName[nState]);
}
