//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcResList.h
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------

#pragma once

#include "engine/KList.h"
#include "KNpcResNode.h"
#include "KPlayerMenuState.h"

class KNpcResList : public KList
{
public:
	CActionName				m_cActionName;  //行为名称 "动作编号表.txt" 英文
	CActionName				m_cNpcAction;   //动作种类 "npc动作表.txt"  英文
	CStateMagicTable		m_cStateTable;  //状态与随身光效对应表
	KPlayerMenuStateGraph	m_cMenuState; //

private:

	// 查找同名的 NpcResNode 并返回节点
	KNpcResNode*		FindNpcRes(char *lpszNpcName);

public:
    KNpcResList();
    ~KNpcResList();
	
    // 初始化 ActionName
	BOOL				Init();
	// 添加一个 NpcResNode 并返回节点，如果已存在，直接返回节点
	KNpcResNode*		AddNpcRes(char *lpszNpcName);
	KNpcResNode*	    ClearAllNpcRes();
	KNpcResNode*	    ClearOneNpcRes(char *lpszNpcName);
};

extern KNpcResList	g_NpcResList;