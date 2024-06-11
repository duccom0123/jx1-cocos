//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerMenuState.h
// Date:	2002.12.10
// Code:	边城浪子
// Desc:	PlayerMenuState Class
//---------------------------------------------------------------------------

#ifndef KPLAYERMENUSTATE_H
#define KPLAYERMENUSTATE_H

#include "GameDataDef.h"
enum
{
	PLAYER_MENU_STATE_NORMAL = 0,
	PLAYER_MENU_STATE_TEAMOPEN,  //队伍打开组队
	PLAYER_MENU_STATE_TRADEOPEN, //交易打开
	PLAYER_MENU_STATE_TRADING,   //交易中
	PLAYER_MENU_STATE_IDLE,      //睡眠状态
	PLAYER_MENU_STATE_NUM,
};

class KPlayerMenuState
{
public:
	int			m_nState;
	int			m_nTradeDest;						// 服务器端记的是 player index 客户端记的是 npc id
	int			m_nTradeState;						// 是否已经点了ok 0 没有 1 点了
	char		m_szSentence[MAX_SENTENCE_LENGTH];
	int			m_nTradeDestState;

public:
	KPlayerMenuState();
	void		Release();
	void		SetState(int nState);

};

class KPlayerMenuStateGraph
{
public:
	char		m_szName[PLAYER_MENU_STATE_NUM][80];

public:
	BOOL		Init();
	void		GetStateSpr(int nState, char *lpszGetName);
};

#endif
