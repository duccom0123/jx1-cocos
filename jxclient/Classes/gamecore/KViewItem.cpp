//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.cpp
// Date:	2003.07.28
// Code:	边城浪子---查看别人装备
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"CoreShell.h"
#include	"KNpc.h"
#include	"KNpcSet.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KViewItem.h"

KViewItem	g_cViewItem;

KViewItem::KViewItem()
{
	Init();
}

void	KViewItem::Init()
{
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;
	m_nProcess  = 0;
	memset(this->m_sItem, 0, sizeof(this->m_sItem));
}

void	KViewItem::ApplyViewEquip(int npcIndex)
{
	if  (npcIndex<=0 || npcIndex>=MAX_NPC) return;

	VIEW_EQUIP_COMMAND	sView;
	sView.ProtocolType = c2s_viewequip;
	sView.m_dwNpcID    = Npc[npcIndex].m_dwID;
    sView.m_bPrcess    = 0; 
	//int nNpcidx=NpcSet.SearchID(Npc[npcIndex].m_dwID);

//   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("正被查看装备");
     //Npc[nNpcidx].ClientShowMsg("正在查看别人的装备");

	 if (g_pClient)
		g_pClient->SendPackToServer(&sView, sizeof(sView));
	 m_nProcess  = 0;
}

void	KViewItem::DeleteAll()
{
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;
	m_nProcess  = 0;
    m_nId       = 0;
	for (int i = 0; i < itempart_num; ++i)
	{
		if (m_sItem[i].nIdx > 0)
			ItemSet.Remove(m_sItem[i].nIdx);
		m_sItem[i].nIdx = 0;
	}
}


void	KViewItem::DeleteNewAll(int nUpdate)
{
	m_dwNpcID	= 0;
	m_szName[0]	= 0;
	m_nProcess = 0;
	m_nId = 0;
	ZeroMemory(m_nIdx,sizeof(m_nIdx));

	for (int i = 0; i < 16; ++i)
	{
		if (!nUpdate)
		{
			if (m_sItem[i].nIdx > 0)
				ItemSet.Remove(m_sItem[i].nIdx);  //删除里面的东西
			m_nDorp[i] = 0;
		}
		else
		{
			if (m_nDorp[i])
				ItemSet.Remove(m_nDorp[i]);
			m_nDorp[i] = m_sItem[i].nIdx;
		}
		m_sItem[i].nIdx = 0;
		//m_sItem[i].nPrice = 0;
		//m_sItem[i].nX = 0;
		//m_sItem[i].nY = 0;
		//m_sItem[i].nLianJieFlg = 0;
	}
}

int		KViewItem::FindIdx(int uId)
{
	for (int i = 0; i < 16; ++i)
	{
		if (m_sItem[i].nIdx == uId)
		{
			return m_nIdx[i];
		}
	}
	return 0;
}

void	KViewItem::GetData(BYTE* pMsg,int nUpdate)
{
	if (!pMsg)
		return;

	DeleteAll();

	int					i, j;
	int					nNpcIdx, nItemIdx, nMagicParam[6],nRMagicParam[6];
	VIEW_EQUIP_SYNC		*pItemSync = (VIEW_EQUIP_SYNC*)pMsg;
	KUiPlayerItem	    sPlayer;

	m_dwNpcID = pItemSync->m_dwNpcID;
	sPlayer.uId = m_dwNpcID;
	nNpcIdx = NpcSet.SearchID(m_dwNpcID);
	if (nNpcIdx > 0)
	{
		m_nLevel = Npc[nNpcIdx].m_Level;
		strcpy(m_szName, Npc[nNpcIdx].Name);
		sPlayer.nIndex = nNpcIdx;
		strcpy(sPlayer.Name, m_szName);
		sPlayer.nData = Npc[nNpcIdx].GetMenuState();
	}
	else
	{
		sPlayer.nIndex = 0;
		sPlayer.Name[0] = 0;
		sPlayer.nData = 0;
	}

	for (i = 0; i < itempart_num; ++i)
	{

		if (pItemSync->m_sInfo[i].m_ID <= 0)
			continue;
		//-------------------------------------------------------------
		int pnMagicParam[6],pnRMagicParam[6],pnBMagicParam[6],pnJBParam[7];
		
		for (j = 0; j < 6; ++j)
		{
			pnMagicParam[j]  = pItemSync->m_sInfo[i].m_MagicLevel[j];
			pnRMagicParam[j] = 0;//pItemSync->m_sInfo[i].m_RongMagicLev[j];
			//pnBMagicParam[j] = 0;//pItemSync->m_sInfo[i].m_BaoMagicLev[j];
			
		}
		for (j = 0; j < 7; ++j)
		{
			pnJBParam[j]     = 0;//pItemSync->m_sInfo[i].m_JBLevel[j];
		}
		
		if (pItemSync->m_sInfo[i].m_GoldId<=0)
			nItemIdx = ItemSet.AddCilentItem(\
			pItemSync->m_sInfo[i].m_Genre,\
			pItemSync->m_sInfo[i].m_Series,\
			pItemSync->m_sInfo[i].m_Level,\
			pItemSync->m_sInfo[i].m_Luck,\
			pItemSync->m_sInfo[i].m_Detail,\
			pItemSync->m_sInfo[i].m_Particur,\
			pnMagicParam,\
			pItemSync->m_sInfo[i].m_Version,\
			pItemSync->m_sInfo[i].m_RandomSeed,\
			1,\
			pItemSync->m_sInfo[i].m_EnChance,\
			pItemSync->m_sInfo[i].m_Point,\
			pItemSync->m_sInfo[i].m_Time.bYear,\
			pItemSync->m_sInfo[i].m_Time.bMonth,\
			pItemSync->m_sInfo[i].m_Time.bDay,\
			pItemSync->m_sInfo[i].m_Time.bHour,\
			pItemSync->m_sInfo[i].m_Time.bMin,\
			pItemSync->m_sInfo[i].m_Rongpiont,\
			pnRMagicParam,\
			pnJBParam,\
			NULL,\
			NULL,\
			NULL,\
			NULL,-2); //pItemSync->m_sInfo[i].m_IsWhere
		else if (pItemSync->m_sInfo[i].m_GoldId>0)
		{
			if (pItemSync->m_sInfo[i].m_Point==8)
				nItemIdx = ItemSet.AddRondomGold(
				pItemSync->m_sInfo[i].m_GoldId,\
				pnMagicParam,\
				pItemSync->m_sInfo[i].m_Series,\
				pItemSync->m_sInfo[i].m_EnChance,\
				pItemSync->m_sInfo[i].m_Rongpiont,\
				pItemSync->m_sInfo[i].m_Time.bYear,\
				pItemSync->m_sInfo[i].m_Time.bMonth,\
				pItemSync->m_sInfo[i].m_Time.bDay,\
				pItemSync->m_sInfo[i].m_Time.bHour,\
				pItemSync->m_sInfo[i].m_Time.bMin,\
				pnRMagicParam,\
				pItemSync->m_sInfo[i].m_Level,pItemSync->m_sInfo[i].m_Luck,pItemSync->m_sInfo[i].m_Detail,\
				-1,1,0,pItemSync->m_sInfo[i].m_IsBaiJin,FALSE,pItemSync->m_sInfo[i].m_RandomSeed);
			else

			nItemIdx = ItemSet.AddGold(\
			pItemSync->m_sInfo[i].m_GoldId,\
			pnMagicParam,\
			pItemSync->m_sInfo[i].m_Series,\
			pItemSync->m_sInfo[i].m_EnChance,\
			pItemSync->m_sInfo[i].m_Rongpiont,\
			pItemSync->m_sInfo[i].m_Time.bYear,\
			pItemSync->m_sInfo[i].m_Time.bMonth,\
			pItemSync->m_sInfo[i].m_Time.bDay,\
			pItemSync->m_sInfo[i].m_Time.bHour,\
			pItemSync->m_sInfo[i].m_Time.bMin,\
			pnRMagicParam,\
			pnBMagicParam,pItemSync->m_sInfo[i].m_Level,1,-1,-1,1,0,pItemSync->m_sInfo[i].m_IsBaiJin,FALSE,pItemSync->m_sInfo[i].m_RandomSeed);
		}
		if (nItemIdx <= 0)
			continue;
		
		//if (nItemIdx > 0) //pItemSync->m_sInfo[i].m_Time.bYear, \\pItemSync->m_sInfo[i].m_StackNum, pItemSync->m_sInfo[i].m_EnChance
		{//设置客户端各种信息
			Item[nItemIdx].SetIsCanUse(pItemSync->m_sInfo[i].m_IsCanUse); //客户端设置是否可以使用
			Item[nItemIdx].SetWonName("系统");//pItemSync->m_WonName pItemSync->m_sInfo[i].m_WonName
			//Item[nItemIdx].SetDurability((short)pItemSync->m_sInfo[i].m_Durability);  // 同步持久度		
			Item[nItemIdx].SetID(pItemSync->m_sInfo[i].m_ID);
		}
		     m_sItem[i].nIdx       = nItemIdx;
	}
	// 通知界面
	CoreDataChanged(GDCNI_VIEW_PLAYERITEM, (DWORD)&sPlayer, 0);
}

void KViewItem::UpdateItem( DWORD dwNpcID )
{
	if (m_nProcess >= 16)
	{
		VIEW_EQUIP_COMMAND	sView;
		sView.ProtocolType = c2s_updateviewequip;
		sView.m_dwNpcID = dwNpcID;
		sView.m_bPrcess = 0;
		if (g_pClient)
			g_pClient->SendPackToServer(&sView, sizeof(sView));
		m_nProcess = 0;
	}
}

int KViewItem::GetCount()
{
	if (m_nProcess < 16)
	{
		return -1;
	}
	else
	{
		return m_nId;
	}
}

void KViewItem::Check( int nCount, DWORD dwId )
{
	//if (nCount == 0)
	//	CoreDataChanged(GDCNI_CLOSE_BAITAN, 0 ,0); //关闭查看装备
	
	if (nCount != m_nId)
	{
		UpdateItem(dwId);
	}
}












