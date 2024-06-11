//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.h
// Date:	2003.07.28
// Code:	边城浪子
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#ifndef _KVIEWITEM_H
#define _KVIEWITEM_H

class KViewItem
{
private:
	DWORD		m_dwNpcID;				// 对方玩家的 npc 的 id
	int			m_nLevel;				// 对方玩家的等级
	char		m_szName[32];			// 对方玩家的名字
    int	        m_nProcess;

public:
	PlayerItem	m_sItem[itempart_num];	// 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息
	int			m_nIdx[itempart_num];
	int			m_nDorp[itempart_num];
	int			m_nId;

public:
	KViewItem();						// 构造
	void		Init();					// 初始化
	void		ApplyViewEquip(int npcIndex);
	void		UpdateItem(DWORD dwNpcID);
	void		DeleteAll();
	void		DeleteNewAll(int nUpdate=0);
	void		GetData(BYTE* pMsg,int nUpdate = 0);
	void		Check(int nCount, DWORD dwId);
	int			GetCount();
	int		    FindIdx(int uId);
};

extern	KViewItem	g_cViewItem;

#endif
