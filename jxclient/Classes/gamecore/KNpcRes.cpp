//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcRes.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------

#include "KCore.h"
//#include "KSprite.h"
#include "KNpc.h"
#include "KPlayer.h"

#include "KNpcResList.h"
#include "KNpcRes.h"
#include "ImgRef.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
#include "gamescene/KScenePlaceC.h"
#include "KSubWorld.h"
#include "gameui/KDrawGameWorld.h"
//#include "KOption.h"
extern KImageStore2 m_ImageStore;
//BOOL g_bShowGameInfo =TRUE;
//---------------------------------------------------------------------------
//	功能：	构造函数
//---------------------------------------------------------------------------
KNpcRes::KNpcRes()
{
	m_nAction = 0;
	m_nNpcKind = 1;
	m_nHelmType = 0;
	m_nPifengType=0;
	m_nChiBangType=0;
	m_nArmorType = 0;
	m_nWeaponType = 0;
	m_nHorseType = 0;
	m_bRideHorse = FALSE;
	m_nBlurState = 0;
	memset(m_szSoundName, 0, sizeof(m_szSoundName));
	memset(m_nSortTable, 0, sizeof(m_nSortTable));
	//m_pSoundNode = NULL;
	//m_pWave = NULL;

	m_SceneID_NPCIdx = 0;
	m_SceneID = 0;

	m_nMenuState = 0;
	m_nBackMenuState = 0;
	m_nSleepState = 0;
	//memset(m_szSentence, 0, sizeof(m_szSentence));
	//memset(m_szBackSentence, 0, sizeof(m_szBackSentence));
}

//---------------------------------------------------------------------------
//	功能：	初始化
//---------------------------------------------------------------------------
BOOL	KNpcRes::Init(char *lpszNpcName, KNpcResList *pNpcResList,unsigned int nNpcDwIdx)
{
	// 初始化 NpcResNode
	//pNpcResList->ClearAllNpcRes();  ClearOneNpcRes(char *lpszNpcName);
	//ZeroMemory(uSprInfo,sizeof(uSprInfo));
	if (!lpszNpcName || !lpszNpcName[0])
		return FALSE;
	m_pcResNode = pNpcResList->AddNpcRes(lpszNpcName);  //增加外观关联
	if ( m_pcResNode == NULL )
		return FALSE;

	m_nNpcKind = m_pcResNode->GetNpcKind();
	m_nAction = 0;
	m_nHelmType = 0;
	m_nPifengType=0; // 披风
	m_nChiBangType=0; //翅膀
	m_nArmorType = 0;
	m_nWeaponType = 0;
	m_nHorseType = 0;
	m_bRideHorse = FALSE;
	memset(m_szSoundName, 0, sizeof(m_szSoundName));
	memset(m_nSortTable, 0, sizeof(m_nSortTable));
	//m_pSoundNode = NULL;
	//m_pWave = NULL;
	m_SceneID_NPCIdx = nNpcDwIdx;
	m_SceneID = 0;
//	m_pSprNode = NULL;

	int		i;
	char	szBuffer[256]={0};
	for (i = 0; i < MAX_PART;++i)
	{
		if (m_pcResNode->CheckPartExist(i))
		{
			m_pcResNode->GetFileName(i, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, 0, 16), m_pcResNode->GetTotalDirs(i, m_nAction, 0, 16), m_pcResNode->GetInterval(i, m_nAction, 0, 0));
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
		}
	}

	int	nShadowFrame, nShadowDir, nShadowInterval, nShadowCgX, nShadowCgY;
	if ( m_pcResNode->m_cShadowInfo.GetFile(
		m_nAction,
		&nShadowFrame,
		&nShadowDir,
		&nShadowInterval,
		&nShadowCgX,
		&nShadowCgY,
		szBuffer) )
	{
		this->m_cNpcShadow.SetSprFile(szBuffer, nShadowFrame, nShadowDir, nShadowInterval);
		this->m_cNpcShadow.SetCenterPos(nShadowCgX, nShadowCgY);
	}
	else
	{
		this->m_cNpcShadow.Release();
	}
	
	for (i = 0; i <36;++i)  //MAX_PART
	{
		m_cStateSpr[i].Release();
		//m_cSprStateSpr[i].Release();
	}

	/*for (i = 0; i <3;++i)  //MAX_PART
	{
		m_cSprStateSpr[i].Release();
	}*/

	//m_cSpecialSpr.Release();
	//m_cFrameSpr.Release();
	m_cMenuStateSpr.Release();
	m_nMenuState = 0;
	m_nBackMenuState = 0;
	m_nSleepState = 0;
	//memset(m_szSentence, 0, sizeof(m_szSentence));
	//memset(m_szBackSentence, 0, sizeof(m_szBackSentence));

	for (i = 0; i < MAX_NPC_IMAGE_NUM;++i) //36
	{
		m_cDrawFile[i].nType = ISI_T_SPR;
		m_cDrawFile[i].Color.Color_b.a = 255;
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cDrawFile[i].uImage = 0;
		m_cDrawFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cDrawFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;	
	}


	/*for (i=0;i<3;++i)
	{
		m_cDrawSprFile[i].nType = ISI_T_SPR;
		m_cDrawSprFile[i].Color.Color_b.a = 255;
		m_cDrawSprFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cDrawSprFile[i].uImage = 0;
		m_cDrawSprFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cDrawSprFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;

	}*/

	//return m_cNpcBlur.Init();

	return true;

}

//---------------------------------------------------------------------------
//	功能：	设定 npc 位置 增加外观
//---------------------------------------------------------------------------
void	KNpcRes::SetPos(int nNpcIdx, int x, int y, int z, BOOL bFocus, BOOL bMenu)
{
	m_nXpos = x;
	m_nYpos = y;
	m_nZpos = z;

	if (g_GameWorld) //界面开始增加节点
		g_GameWorld->MoveObject(OBJ_NODE_NPC,nNpcIdx,false,x,y,z);

	if (bFocus)
		//SubWorld[0].SetFocusPosition(x, y, z);  //设置人物的位置
		g_ScenePlace.SetFocusPosition(x, y, z);

	/*if (!bMenu)
    {
		m_SceneID_NPCIdx = nNpcIdx; 
        g_ScenePlace.MoveObject(CGOG_NPC, nNpcIdx, x, y, z, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL | IPOT_RL_LIGHT_PROP);

    }*/
}

void	KNpcRes::Remove(int nNpcIdx)
{
	if (g_GameWorld) //界面开始增加节点
		//g_GameWorld->RemoveObject(OBJ_NODE_NPC,nNpcIdx);
		g_GameWorld->MoveObject(OBJ_NODE_NPC,nNpcIdx,true);

	if (m_SceneID)
	{
//		g_ScenePlace.RemoveObject(CGOG_NPC, nNpcIdx,m_SceneID);	  //
		m_SceneID = 0;
	}
	//m_cNpcBlur.Remove();
}
//---------------------------------------------------------------------------
//	功能：	绘制外观
//---------------------------------------------------------------------------
void	KNpcRes::Draw(int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, BOOL bInMenu)
{

	int		i, nGetFrame = 1, nGetDir = 1, nFirst, nPos;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = m_nZpos;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
		return;

	if (!m_pcResNode || !g_GameWorld)
		return;

	// 外部控制换帧
	if (nAllFrame > 0)
	{
		// 计算阴影当前帧
		nGetDir = this->m_cNpcShadow.m_nTotalDir;
		if (nGetDir <= 0)
			nGetDir = 1;
		nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
		if (nCurDirNo >= nGetDir)
			nCurDirNo -= nGetDir;
		nGetFrame = this->m_cNpcShadow.m_nTotalFrame;
		nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
		this->m_cNpcShadow.SetCurFrame(nCurFrameNo);
		
		// 各个部件
		nFirst = 0;
		// 找到动画的当前桢

		/*if (m_cNpcImage[20].CheckExist())
		{//翅膀
		   if (m_cNpcImage[20].SetCurDir64(nDir))
		   { 
			  m_cNpcImage[20].GetNextFrame(); //设置下一帧
		   } 
		}*/

		for (i = 0; i < MAX_PART;++i)
		{
			if (m_cNpcImage[i].CheckExist())
			{
				if (nFirst == 0)   //每一次大循环
				{//某个方向的开始帧数					
					nGetDir = m_cNpcImage[i].m_nTotalDir;	//总方向

					if (nGetDir <= 0)
						nGetDir = 1;

					nCurDirNo = (nDir + (32/nGetDir))/(64/nGetDir);

					if (nCurDirNo >= nGetDir)
						nCurDirNo -= nGetDir;

					nGetFrame = m_cNpcImage[i].m_nTotalFrame; //总帧数

					nCurFrameNo = nCurDirNo*(nGetFrame/nGetDir)+(nGetFrame/nGetDir)*nCurFrame/nAllFrame;
				//	nDirFrame * (m_nCurDir + 1)
					if (i==20)
					{
						continue;
					}

					m_cNpcImage[i].SetCurFrame(nCurFrameNo);

					nFirst = 1;
				}
				else
				{
					/*KImageParam	sImage;
					//m_ImageStore.GetImageParam(pszImage, nType, pImageData); 有阻塞
					if (m_ImageStore.GetImageParam(m_cNpcImage[i].m_szName,ISI_T_SPR ,&sImage))
					{//加载各个部位的基本数据,前面已经设置过
						m_cNpcImage[i].m_nTotalDir   = sImage.nNumFramesGroup;
						m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;//设置总帧数
						
					}*/
					if (i==20)
					{
						continue;
					}
					m_cNpcImage[i].SetCurFrame(nCurFrameNo);
				}
			}
		}//end for
	}
	else
	{
		// 计算阴影当前帧
		if (m_cNpcShadow.SetCurDir64(nDir))
		{
			m_cNpcShadow.GetNextFrame();
		}

		// 各个部件
		for (i = 0; i < MAX_PART; ++i)
		{//的下一帧数
			if (!m_cNpcImage[i].CheckExist())
				continue;
			if (m_cNpcImage[i].SetCurDir64(nDir))
			{
				m_cNpcImage[i].GetNextFrame();
				nCurDirNo   = m_cNpcImage[i].m_nCurDir;
				nCurFrameNo = m_cNpcImage[i].m_nCurFrame; 
			}	
		}
	}//end if
	// 状态特效换帧(同时显示多个)
	for (i = 0; i < 36;++i)  //36
	{
		if (m_cStateSpr[i].m_nID>0)
		    m_cStateSpr[i].m_SprContrul.GetNextFrame();
	}

	/*for (i = 0; i < 3;++i)  //36
	{
		if (m_cSprStateSpr[i].m_nID>0)
			m_cSprStateSpr[i].m_SprContrul.GetNextFrame();
	}*/
	/*
	if ( m_cSpecialSpr.GetNextFrame(FALSE) )
	{//设定特殊的只播放一遍的随身spr文件(只能显示一个)
		if (m_cSpecialSpr.CheckEnd())
			m_cSpecialSpr.Release();
	}
	if ( m_cFrameSpr.GetNextFrame(FALSE) )
	{//设置循环动态SPR 不能同时显示多SPR(只能显示一个)
		if (m_cFrameSpr.CheckEnd())
			m_cFrameSpr.Release();
	}*/
	// 播放声音
	if (nCurFrame < nAllFrame / 4)
	{
		this->GetSoundName();
		this->PlaySound(this->m_nXpos, this->m_nYpos);
	}

	// 得到部件排列顺序	当前方向  当前帧的排序 表
	if (!m_pcResNode->GetnSort(m_nAction,nCurDirNo,nCurFrameNo,m_nSortTable,MAX_PART))
    	return;

// ---------------------------------- 处理绘制列表 -------------------------------
	//头顶状态特效
	/*for ( i = 0; i < 12;++i)
	{
		if (m_cStateSpr[i].m_nID)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage       = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame       = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 70;  //默认偏移名字的距离
			if (m_nDoing == cdo_sit)
			{
				if (!Npc[nNpcIdx].m_MaskType)//设置面具高度
					nHeightOff -= 20;
			}
			if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)//设置没面具高度
				nHeightOff += 28;
			if (Npc[nNpcIdx].TongName[0])
			{
				if  (!Npc[nNpcIdx].m_bIsHideTong)
					nHeightOff += 28;
			}
			if (Npc[nNpcIdx].m_MaskType)//设置面具高度
				nHeightOff += 20;
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			nPos++;
		}
	}
	g_GameWorld->DrawPrimitives_State(nNpcIdx,nPos,m_cDrawFile,STATE_PART_HEAD,STATE_PART_HEAD,0,bInMenu);
	
	nPos = 0;	
	//身上状态特效(npc背后)
	for (i = 12; i < 24; ++i)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_nBackStart <= m_cStateSpr[i].m_SprContrul.m_nCurFrame && 
				m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackEnd)
			{
				strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
				m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				int nHeightOff = 0;
				if (m_bRideHorse)
					nHeightOff += 38;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
				nPos++;
			}
		}
	}
    g_GameWorld->DrawPrimitives_State(nNpcIdx,nPos, m_cDrawFile,STATE_PART_BODY_H,STATE_PART_BODY_H,0,bInMenu);
	
	nPos = 0;
	//脚底状态特效
	for ( i = 24; i < 36; ++i)
	{
		if (m_cStateSpr[i].m_nID)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
	}
	g_GameWorld->DrawPrimitives_State(nNpcIdx,nPos,m_cDrawFile,STATE_PART_FOOT,STATE_PART_FOOT,0,bInMenu);
	nPos = 0;
	//身上状态特效(npc身前)-------------------------------------------------------------
	for (i = 12; i < 24; ++i)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackStart || 
				m_cStateSpr[i].m_SprContrul.m_nCurFrame >= m_cStateSpr[i].m_nBackEnd)
			{
				strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
				m_cDrawFile[nPos].uImage       = m_cStateSpr[i].m_SprContrul.m_dwNameID;
				m_cDrawFile[nPos].nFrame       = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				int nHeightOff = 0;
				if (m_bRideHorse)
					nHeightOff += 38;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
				nPos++;
			}
		}
	}
	g_GameWorld->DrawPrimitives_State(nNpcIdx,nPos,m_cDrawFile,STATE_PART_BODY_Q,STATE_PART_BODY_Q,0,bInMenu);
	*/
	nPos = 0;
	for (i = 0; i < 12; ++i)
	{
		if (m_cStateSpr[i].m_nID>0 && m_cStateSpr[i].m_SprContrul.m_szName[0])
		{
			strcpy(m_cDrawFile[nPos].szImage,m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage       = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame       = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;

				int nHeightOff = 70;  //默认偏移名字的距离
				if (m_nDoing == cdo_sit)
				{
					if (!Npc[nNpcIdx].m_MaskType)//设置面具高度
						nHeightOff -= 20;
				}
				if (m_bRideHorse && !Npc[nNpcIdx].m_MaskType)//设置没面具高度
					nHeightOff += 28;
				if (Npc[nNpcIdx].TongName[0])
				{
					if  (!Npc[nNpcIdx].m_bIsHideTong)
						nHeightOff += 28;
				}
				if (Npc[nNpcIdx].m_MaskType)                 //设置面具高度
					nHeightOff += 20;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			    g_GameWorld->DrawPrimitives_State(nNpcIdx,m_cStateSpr[i].m_nID,nPos,m_cDrawFile,STATE_PART_HEAD,STATE_PART_HEAD,0,bInMenu);
			    nPos++;
		}
	}

	nPos = 0;
	for (i = 12; i < 24; ++i)
	{
		if (m_cStateSpr[i].m_nID>0 && m_cStateSpr[i].m_SprContrul.m_szName[0])
		{
			strcpy(m_cDrawFile[nPos].szImage,m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage       = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame       = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 0;
			if (m_bRideHorse)
				nHeightOff += 38;
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;

			if (m_cStateSpr[i].m_nBackStart <= m_cStateSpr[i].m_SprContrul.m_nCurFrame && 
				m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackEnd)
			{
				g_GameWorld->DrawPrimitives_State(nNpcIdx,m_cStateSpr[i].m_nID,nPos,m_cDrawFile,STATE_PART_BODY_H,STATE_PART_BODY_H,0,bInMenu);
				nPos++;
			}
			else
			//if (m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackStart || 
			//	m_cStateSpr[i].m_SprContrul.m_nCurFrame >= m_cStateSpr[i].m_nBackEnd)
			{
				g_GameWorld->DrawPrimitives_State(nNpcIdx,m_cStateSpr[i].m_nID,nPos,m_cDrawFile,STATE_PART_BODY_Q,STATE_PART_BODY_Q,0,bInMenu);
				nPos++;
			}
		}
	}
	nPos = 0;
	//阴影文件名（0）//第一个为阴影的位置
	strcpy(m_cDrawFile[nPos].szImage, this->m_cNpcShadow.m_szName);
	m_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;
	m_cDrawFile[nPos].nFrame = this->m_cNpcShadow.m_nCurFrame;
	m_cDrawFile[nPos].oPosition.nX = nScreenX;
	m_cDrawFile[nPos].oPosition.nY = nScreenY;
	m_cDrawFile[nPos].oPosition.nZ = 0;//nScreenZ;

    strcpy(Npc[nNpcIdx]._NpcShadow.szImage, this->m_cNpcShadow.m_szName);
	Npc[nNpcIdx]._NpcShadow.uImage = m_cNpcShadow.m_dwNameID;
	Npc[nNpcIdx]._NpcShadow.nFrame = this->m_cNpcShadow.m_nCurFrame;
	Npc[nNpcIdx]._NpcShadow.nX = nScreenX;
	Npc[nNpcIdx]._NpcShadow.nY = nScreenY;
	Npc[nNpcIdx]._NpcShadow.nZ = 0;

	Npc[nNpcIdx]._NpcShadow.bInMenu =bInMenu;

	if  (g_GameWorld)//绘画阴影
	    g_GameWorld->DrawShadow(nNpcIdx,nPos, m_cDrawFile);

	nPos++;
	//npc部件--------------------------------------------------------
	for (i = 0; i <MAX_PART ; ++i) 
	{////MAX_PART  0-24个位置   20为 翅膀   16为披风
		    if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{ //如果是人物
				if (m_cNpcImage[m_nSortTable[i]].m_szName[0])
			      strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);  //spr路径 m_nSortTable[i]
				else
				  ZeroMemory(m_cDrawFile[nPos].szImage,sizeof(m_cDrawFile[nPos].szImage));

			   m_cDrawFile[nPos].uImage       = m_cNpcImage[m_nSortTable[i]].m_dwNameID;  //m_nSortTable[i]
			   m_cDrawFile[nPos].nFrame       = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
			   m_cDrawFile[nPos].oPosition.nX = nScreenX;
			   m_cDrawFile[nPos].oPosition.nY = nScreenY;
			   m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
			  if  (g_GameWorld)
			  {
				  int k   = m_nSortTable[i];//k=部位序列号 i 等于是 部位先后绘画的顺序 i=Order
				 /* if  (k==8 && nNpcIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex)
				  {//如果是客户端本人
					  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(m_cDrawFile[nPos].szImage);
				  }*/

				  if  (k >= MAX_BODY_PART_SECT * 0 && k < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT) //头部 头盔  0-4
				       g_GameWorld->DrawPrimitives(nNpcIdx,nPos,m_cDrawFile,EQ_PART_HEAD,k,i,bInMenu);
				  else if  (k >= MAX_BODY_PART_SECT * 1 && k < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT) //躯体 左右手4-8 普通NPC为 第6个
					   g_GameWorld->DrawPrimitives(nNpcIdx,nPos, m_cDrawFile,EQ_PART_BODY,k,i,bInMenu);
				  else if  (k >= MAX_BODY_PART_SECT * 2 && k < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT) //左右武器 8-12
					   g_GameWorld->DrawPrimitives(nNpcIdx,nPos, m_cDrawFile,EQ_PART_HAND_W,k,i,bInMenu);
				  else if  (k >= MAX_BODY_PART_SECT * 3 && k < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT) //马前 中 后 12-16
				  {
					   g_GameWorld->DrawPrimitives(nNpcIdx,nPos, m_cDrawFile,EQ_PART_HORSE,k,i,bInMenu);
					   g_GameWorld->SetHorseState(nNpcIdx);
				  }
				  else if  (k >= MAX_BODY_PART_SECT * 4 && k < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT) //披风 16 -20
				  {
					   g_GameWorld->DrawPrimitives(nNpcIdx,nPos, m_cDrawFile,EQ_PART_PIFENG,k,i,bInMenu);
					   g_GameWorld->SetPiFengState(nNpcIdx);
				  }

				 // if  (!m_cDrawFile[nPos].szImage[0])
				//	  g_GameWorld->_setBuWeiHide(nNpcIdx,false,k+1);

			  }
			   nPos++;
			}
			/*else
			{//这些部位需要隐藏的
				//if  (i>=1 && i<=17)  //这些部位需要隐藏
				//    g_GameWorld->_setBuWeiHide(nNpcIdx,false,i);
			}*/
	}
	
	nPos = 0;
	for (i = 24; i < 36; ++i)
	{
		if (m_cStateSpr[i].m_nID>0 && m_cStateSpr[i].m_SprContrul.m_szName[0])
		{
			strcpy(m_cDrawFile[nPos].szImage,m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].uImage       = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame       = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			g_GameWorld->DrawPrimitives_State(nNpcIdx,m_cStateSpr[i].m_nID,nPos,m_cDrawFile,STATE_PART_FOOT,STATE_PART_FOOT,0,bInMenu);
			nPos++;
		}
	}
	// -------------------------------- 处理绘制列表 end -----------------------------
	
	// ----------------------------------- 处理残影 ----------------------------------

	/*if  (g_pRepresent->GetRepMoDel()==3)   //3D模式不绘画残影
		return;

	int j = 0;
	m_cNpcBlur.ChangeAlpha();
	if (m_nBlurState == TRUE && m_cNpcBlur.NowGetBlur())
	{
		//m_cNpcBlur.ClearCurNo();

		for (i = 0, j = 0; i < MAX_PART; ++i)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				m_cNpcBlur.SetFile(j, m_cNpcImage[m_nSortTable[i]].m_szName, m_cNpcImage[m_nSortTable[i]].m_dwNameID, m_cNpcImage[m_nSortTable[i]].m_nCurFrame, nScreenX, nScreenY, nScreenZ);
				j++;
			}
		}
		m_cNpcBlur.SetMapPos(m_nXpos, m_nYpos, m_nZpos, nNpcIdx);

		m_cNpcBlur.SetNextNo();
	} 
	*/
//	m_cNpcBlur.Draw();
// --------------------------------- 处理残影 end --------------------------------
	// 绘制

//	g_pRepresent->DrawPrimitives(MAX_NPC_IMAGE_NUM, m_cDrawFile, RU_T_IMAGE, bInMenu);	

}

void	KNpcRes::GetShadowName(char *lpszShadow, char *lpszSprName)
{
	KNpcResNode::GetShadowName(lpszShadow, lpszSprName);
}

KNpcRes::~KNpcRes()
{
    // 因为NPC中会自动调用Remove
    //if (m_SceneID)
    //{
	//    //Remove(m_SceneID_NPCIdx);
    //    m_SceneID_NPCIdx = 0;
    //}
}
//---------------------------------------------------------------------------
//	功能：	设定披风类型
//---------------------------------------------------------------------------
BOOL KNpcRes::SetPifeng(int nPifengType)
{
	int		i;

	//if (nPifengType <=0)
	//	return FALSE;

	if ( !m_pcResNode )
		return FALSE;

	if (nPifengType>0 && m_nPifengType == nPifengType)
		return TRUE;
	m_nPifengType = nPifengType;
	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; ++i) //16--19
	{
		if ( m_pcResNode->CheckPartExist(i) && nPifengType>0 )
		{
			m_pcResNode->GetFileName(i, m_nAction, nPifengType, "", szBuffer, sizeof(szBuffer)); //部位，行为动作，SPR类型
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nPifengType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, nPifengType, 16), m_pcResNode->GetInterval(i, m_nAction, nPifengType, 0));
			
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定翅膀类型
//---------------------------------------------------------------------------
BOOL KNpcRes::SetChiBang(int nChiBangType)
{
	int		i;
	//if (nChiBangType <=0)
	//	return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (nChiBangType >0 && m_nChiBangType == nChiBangType)
		return TRUE;

	m_nChiBangType = nChiBangType;
	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	for (i = MAX_BODY_PART_SECT * 5; i < MAX_BODY_PART_SECT * 5 + MAX_BODY_PART_SECT; ++i) //20--23
	{
		if (m_pcResNode->CheckPartExist(i) && nChiBangType>0)
		{//如果部件存在的话，就设置该位置的信息
			m_pcResNode->GetFileName(i, m_nAction, nChiBangType, "", szBuffer, sizeof(szBuffer)); //部位，行为动作，SPR类型
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nChiBangType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, nChiBangType, 16), m_pcResNode->GetInterval(i, m_nAction, nChiBangType, 0));
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定头盔类型
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetHelm(int nHelmType)
{
	int		i;
	

	if (nHelmType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nHelmType == nHelmType)
		return TRUE;
	m_nHelmType = nHelmType;
	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; ++i) //0--4
	{//头部+发型
		if (m_pcResNode->CheckPartExist(i))
		{
			m_pcResNode->GetFileName(i, m_nAction, nHelmType,"", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, nHelmType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, nHelmType, 16), m_pcResNode->GetInterval(i, m_nAction, nHelmType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}


//---------------------------------------------------------------------------
//	功能:设定盔甲类型
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetArmor(int nArmorType)
{
	int	i;
	if (nArmorType < 0)
		return FALSE;

	if (!m_pcResNode)
		return FALSE;

	if (m_nArmorType == nArmorType) //如果相同的就不设置了
	{
		return TRUE;
	}
	m_nArmorType = nArmorType;

	char	szBuffer[256]={0};
	int     nFrams=0,nFramwidth=0,nFramheight=0;
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; ++i)  //4--8
	{//肩膀+躯体+左手+右手 4 5 6 7
		if (m_pcResNode->CheckPartExist(i))
		{
			//CCMessageBox("找到头部spr","SetArmor");
			char nInfo[64]={0};
			m_pcResNode->GetFileName(i,m_nAction,nArmorType,"",szBuffer,sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i,m_nAction,nArmorType,16),m_pcResNode->GetTotalDirs(i,m_nAction,nArmorType,16),m_pcResNode->GetInterval(i,m_nAction,nArmorType,0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
			/*else
			{
				sprintf(nInfo,"躯体部位路径为空:%d",i);
				CCMessageBox(nInfo,"SetArmor");
			}*/
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
			//CCMessageBox("部位不存在头部spr","SetArmor");
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定武器类型外观
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetWeapon(int nWeaponType)
{
	int	i;

	if (nWeaponType < 0)
		return FALSE;
	if (!m_pcResNode)
		return FALSE;
	if (m_nWeaponType == nWeaponType)
		return TRUE;
	m_nWeaponType = nWeaponType;
    //获取在某种行为状态下的 武器的外观编号
	m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nWeaponType, m_bRideHorse);

	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	int	 nFrame, nDir, nInterval, nCgX, nCgY;
    //设置主角阴影
	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; ++i)  //0--4 头部
	{//头部+发型 0 1
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; ++i) //4--8 躯体-衣服
	{//肩膀+躯体+左右手 4 5 6 7
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; ++i)  //8--12 武器
	{//左右手武器 8 9
		if (m_pcResNode->CheckPartExist(i))
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType,"", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; ++i)  //12--16  //  马
	{//马前 中 后 12 13 14
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
//m_nPifengType=0; // 披风
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; ++i)  //16--20  披风
	{//披风 16
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nPifengType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nPifengType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	//m_nChiBangType=0; // 翅膀
	for (i = MAX_BODY_PART_SECT * 5; i < MAX_BODY_PART_SECT * 5 + MAX_BODY_PART_SECT; ++i)  //20--24  披风
	{//翅膀 20
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i,m_nAction,m_nChiBangType,"",szBuffer,sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i,m_nAction,m_nChiBangType,16),m_pcResNode->GetTotalDirs(i, m_nAction, m_nChiBangType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nChiBangType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定马匹类型
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetHorse(int nHorseType)
{
	int	i;

	if (nHorseType < 0)
		return FALSE;
	if ( !m_pcResNode )
		return FALSE;
	if (m_nHorseType == nHorseType)
		return TRUE;
	m_nHorseType = nHorseType;

	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )// 功能:	判断某个部件是否存在
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定动作类型
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetAction(int nDoing)
{
	if (!m_pcResNode)
		return FALSE;
	if (nDoing < 0)
		return FALSE;
	if (m_nDoing == nDoing) //如果动作相同就不设置了
		return TRUE;
	m_nDoing = nDoing;

	if (m_pcResNode)
		m_nAction = m_pcResNode->GetActNo(nDoing, m_nWeaponType, m_bRideHorse);

	int	i;
	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	int	 nFrame, nDir, nInterval, nCgX, nCgY;
	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{//也只能是唯一的部位
			m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	//m_nPifengType 披风
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; ++i)
	{
		if (m_pcResNode->CheckPartExist(i) && m_nPifengType>0)
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nPifengType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nPifengType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	//m_nChiBangType=0; // 翅膀
	for (i = MAX_BODY_PART_SECT * 5; i < MAX_BODY_PART_SECT * 5 + MAX_BODY_PART_SECT; ++i)  //20--24  披风
	{
		if (m_pcResNode->CheckPartExist(i) && m_nChiBangType>0)
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nChiBangType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nChiBangType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nChiBangType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nChiBangType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定是否骑马  换装？
//---------------------------------------------------------------------------
BOOL KNpcRes::SetRideHorse(BOOL bRideHorse)
{
	if (!m_pcResNode)
		return FALSE;
	if (m_bRideHorse == bRideHorse)
		return TRUE;

	m_bRideHorse = bRideHorse;
	if (m_pcResNode)
		m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nWeaponType, m_bRideHorse);

	int		i;
	char szBuffer[256]={0};
	int  nFrams=0,nFramwidth=0,nFramheight=0;
	int		nFrame, nDir, nInterval, nCgX, nCgY;
	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}
	

	for (i = MAX_BODY_PART_SECT * 0; i < MAX_BODY_PART_SECT * 0 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHelmType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHelmType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHelmType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 1; i < MAX_BODY_PART_SECT * 1 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nArmorType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nArmorType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nArmorType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 2; i < MAX_BODY_PART_SECT * 2 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nWeaponType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nWeaponType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nWeaponType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	for (i = MAX_BODY_PART_SECT * 3; i < MAX_BODY_PART_SECT * 3 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nHorseType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nHorseType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nHorseType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
    //m_nPifengType披风
	for (i = MAX_BODY_PART_SECT * 4; i < MAX_BODY_PART_SECT * 4 + MAX_BODY_PART_SECT; ++i)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nPifengType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nPifengType, 16), m_pcResNode->GetInterval(i, m_nAction, m_nPifengType, 0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	//m_nChiBangType=0; // 翅膀
	for (i = MAX_BODY_PART_SECT * 5; i < MAX_BODY_PART_SECT * 5 + MAX_BODY_PART_SECT; ++i)  //20--24  翅膀
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, m_nChiBangType, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, m_nChiBangType, 16), m_pcResNode->GetTotalDirs(i, m_nAction, m_nChiBangType, 16), m_pcResNode->GetInterval(i,m_nAction,m_nChiBangType,0));
			/*if (szBuffer[0])
			{//把spr 加入缓存
				g_StrLower(szBuffer);
				_getinidata.getinidata_new(szBuffer,0,&nFramwidth,&nFramheight,&nFrams);
			}*/
		}
		else
		{
			m_cNpcImage[i].Release();
		}
	}
	return TRUE;
}


void	KNpcRes::SetSprState(int nSprIdx,KNpcResList *pNpcResList)
{
		/*int nStateID;

	    if  (nSprIdx<=0)
		{
		   for (int i=0;i<STATE_MAGIC_TYPE_NUM;++i)
		   {   
			 m_cSprStateSpr[i].Release(); 
		   } 
			return;
		}

		g_PlayerTitle.GetInteger(nSprIdx+1,"SpeicalGraphic",0,&nStateID);
		//char nVal[128];
		//sprintf(nVal,"状态ID(%d)",nStateID);
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nVal);

		if (nStateID<=0)
		{
			for (int i=0;i<STATE_MAGIC_TYPE_NUM;++i)
			{   
				m_cSprStateSpr[i].Release(); 
			} 
			return;
		}

	 	int	nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal,nMapXpos,nMapYpos;
		
		char szBuffer[128]={0};
			//获取这个SPRID的数据
			g_NpcResList.m_cStateTable.GetInfo(nStateID, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal,&nMapXpos,&nMapYpos);
	
		if (!szBuffer[0])
		    return;

		if (nType >= 0 || nType < STATE_MAGIC_TYPE_NUM)  //部位 头顶 脚底 身上 //nType=STATE_MAGIC_HEAD;
		{
		   for (int i=0;i<STATE_MAGIC_TYPE_NUM;++i)
		   {   
			   if (i==nType)
			   {
				  if (m_cSprStateSpr[nType].m_nID!=nStateID)
				  {
			        m_cSprStateSpr[nType].Release();
					m_cSprStateSpr[nType].m_nID        = nStateID;   //状态的ID
					m_cSprStateSpr[nType].m_nType      = nType;
					m_cSprStateSpr[nType].m_nPlayType  = nPlayType;
					m_cSprStateSpr[nType].m_nBackStart = nBackStart;
					m_cSprStateSpr[nType].m_nBackEnd   = nBackEnd;
					m_cSprStateSpr[nType].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
				  }
			   }
			   else
			   {
				    m_cSprStateSpr[i].Release();
			   }   
		   }  
		}*/
}
//---------------------------------------------------------------------------
//	功能：	设定客户端状态特效
//---------------------------------------------------------------------------
void	KNpcRes::SetState(KList *pNpcStateList, KNpcResList *pNpcResList)
{
	if ( !pNpcStateList || !pNpcResList)
		return;

	int	 i, nFind, nFindFlag[36];
	KStateNode	*pNode;
	int	 nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal,nMapXpos,nMapYpos;
	char szBuffer[128];
	ZeroMemory(szBuffer,sizeof(szBuffer));
	// 检查现有的是否要停止
	memset(nFindFlag, 0, sizeof(nFindFlag));

	pNode = (KStateNode*)pNpcStateList->GetHead();

	while (pNode)
	{
		if (pNode->m_StateGraphics > 0)
		{
			for (i = 0; i < 36; ++i)
			{//如果这个状态已经存在
				if (pNode->m_StateGraphics == m_cStateSpr[i].m_nID)
					nFindFlag[i] = 1;
			}
		}
		pNode = (KStateNode*)pNode->GetNext();
	}

	for (i = 0; i < 36; ++i)
	{//删除与 状态ID不相同的 信息
		if (!nFindFlag[i] && m_cStateSpr[i].m_nID)
		{
			if (g_GameWorld)
				g_GameWorld->removespriteByIdx(m_SceneID_NPCIdx,m_cStateSpr[i].m_nID);
			
			m_cStateSpr[i].Release();
		}
	}

	pNode = (KStateNode*)pNpcStateList->GetHead();
	while (pNode)
	{
		if (pNode->m_StateGraphics <= 0)
			goto WHILE_END;               //如果节点为空 进行一下个对比
		// 检查是否已经存在
		{
		  nFind = 0;
		  for (i = 0; i < 36; ++i)
		  { 
			if (pNode->m_StateGraphics == m_cStateSpr[i].m_nID)   //原来有的
			{//如果原来存在的 就进行下一个对比
				nFind = 1;
				break;
			}
		  } 
		
		}

		if (nFind > 0)  //如果原来的？就进行查找下一条？
			goto WHILE_END;
		{
	      szBuffer[0] = 0;
		  //获取这个SPRID的数据
		  pNpcResList->m_cStateTable.GetInfo(pNode->m_StateGraphics, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal,&nMapXpos,&nMapYpos);
		} 
		if (!szBuffer[0])
			goto WHILE_END;
		if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)  //部位 头顶 脚底 身上
			goto WHILE_END;                              //nType=STATE_MAGIC_HEAD;
		 {
		  for (i = nType * 12; i < nType * 12+12; ++i)   //0 1 2
		  { 
			if (m_cStateSpr[i].m_nID == 0)
			{
				// 添加新的
				m_cStateSpr[i].Release();
				m_cStateSpr[i].m_nID        = pNode->m_StateGraphics;   //状态的ID
				m_cStateSpr[i].m_nType      = nType;
				m_cStateSpr[i].m_nPlayType  = nPlayType;
				m_cStateSpr[i].m_nBackStart = nBackStart;
				m_cStateSpr[i].m_nBackEnd   = nBackEnd;
				m_cStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
				break;
			}
		  } 
		}
WHILE_END:
		pNode = (KStateNode*)pNode->GetNext();
	}
}
//---------------------------------------------------------------------------
//	功能：	设定自己看别人客户端状态特效
/*/---------------------------------------------------------------------------
void	KNpcRes::SetClientState(KList *pNpcStateList, KNpcResList *pNpcResList)
{
	if ( !pNpcStateList || !pNpcResList)
		return;
	
	int		i, nFind, nFindFlag[36];
	KStateNode	*pNode;
	int		nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal,nMapXpos,nMapYpos;
	char	szBuffer[128];
	
	// 检查现有的是否要停止
	memset(nFindFlag, 0, sizeof(nFindFlag));
	
	pNode = (KStateNode*)pNpcStateList->GetHead();
	
	while (pNode)
	{
		if (pNode->m_StateGraphics > 0)
		{
			for (i = 0; i < 20; ++i)
			{//如果这个状态已经存在
				if (pNode->m_StateGraphics == m_ClientStateSpr[i].m_nID)
					nFindFlag[i] = 1;
			}
		}
		pNode = (KStateNode*)pNode->GetNext();
	}
	
	for (i = 0; i < 20; ++i)
	{//删除与 状态ID不相同的 信息
		if (!nFindFlag[i] && m_ClientStateSpr[i].m_nID)  
			m_ClientStateSpr[i].Release();
	}
	
	pNode = (KStateNode*)pNpcStateList->GetHead();
	while (pNode)
	{
		if (pNode->m_StateGraphics <= 0)
			goto WHILE_END;               //如果节点为空 进行一下个对比
		// 检查是否已经存在
		{
			nFind = 0;
			for (i = 0; i < 20; ++i)
			{ 
				if (pNode->m_StateGraphics == m_ClientStateSpr[i].m_nID)   //原来有的
				{//如果原来存在的 就进行下一个对比
					nFind = 1;
					break;
				}
			} 
		}
		if (nFind > 0)  //如果原来的？就进行查找下一条？
			goto WHILE_END;
		{
			szBuffer[0] = 0;
			//获取这个SPRID的数据
			pNpcResList->m_cStateTable.GetInfo(pNode->m_StateGraphics, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal,&nMapXpos,&nMapYpos);
		} 
		if (!szBuffer[0])
			goto WHILE_END;
		if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)  //部位 头顶 脚底 身上
			goto WHILE_END;                              //nType=STATE_MAGIC_HEAD;
		{
			for (i = nType * 6; i < nType * 6+6; ++i)   //0 1 2
			{ 
				if (m_cStateSpr[i].m_nID == 0)
				{
					// 添加新的
					m_ClientStateSpr[i].Release();
					m_ClientStateSpr[i].m_nID        = pNode->m_StateGraphics;   //状态的ID
					m_ClientStateSpr[i].m_nType      = nType;
					m_ClientStateSpr[i].m_nPlayType  = nPlayType;
					m_ClientStateSpr[i].m_nBackStart = nBackStart;
					m_ClientStateSpr[i].m_nBackEnd   = nBackEnd;
					m_ClientStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
					break;
				}
			} 
		}
WHILE_END:
		pNode = (KStateNode*)pNode->GetNext();
	}
}
*/
//设定自动SPR (单纯的描绘)
void KNpcRes::SetClientSpr(char *nSprPath,int nxLeft,int nyTop,int nzPos,int i,char *ncPart,int nSprIdx)
{
	   /*sprintf(uSprInfo[i].nSprPath,nSprPath);
	   sprintf(uSprInfo[i].ncPart,ncPart);
	   uSprInfo[i].nXops  = nxLeft;
	   uSprInfo[i].nYops  = nyTop;
	   uSprInfo[i].nZops  = nzPos;
	   uSprInfo[i].nStateidx =nSprIdx; */

}

//---------------------------------------------------------------------------
//	功能：	设定特殊的只播放一遍的随身spr文件
//---------------------------------------------------------------------------
void	KNpcRes::SetSpecialSpr(char *lpszSprName)
{
	/*KImageParam	sImage;
//	g_pRepresent->GetImageParam(lpszSprName, &sImage, ISI_T_SPR);
	if (sImage.nInterval <= 0)
		sImage.nInterval = 100;
	if (sImage.nInterval > 1000)
		sImage.nInterval = 1000;
	if (sImage.nNumFramesGroup <= 0)
		sImage.nNumFramesGroup = 1;
	if (sImage.nNumFrames < sImage.nNumFramesGroup)
		sImage.nNumFrames = sImage.nNumFramesGroup;
	m_cSpecialSpr.SetSprFile(lpszSprName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
	*/
}


void	KNpcRes::SetFrameSpr(char *lpszSprName, int nX, int nY, int nHeight,int mInterval)
{
	/*KImageParam	sImage;
//	g_pRepresent->GetImageParam(lpszSprName, &sImage, ISI_T_SPR);
	if (sImage.nInterval <= 0) //间隔时间
		sImage.nInterval = 100;
	else
        sImage.nInterval = mInterval;

	if (sImage.nInterval > 1000)
		sImage.nInterval = 1000;

	if (sImage.nNumFramesGroup <= 0)
		sImage.nNumFramesGroup = 1;
	if (sImage.nNumFrames < sImage.nNumFramesGroup)
		sImage.nNumFrames = sImage.nNumFramesGroup;
	m_cFrameSpr.m_nXpos = nX;
	m_cFrameSpr.m_nYpos = nY;
	m_nHeightOff = nHeight-84;
	m_cFrameSpr.SetSprFile(lpszSprName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
	*/
}
//---------------------------------------------------------------------------
//	功能：set menu state spr //最大5个状态 交易 睡眠 组队 等
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuStateSpr(int nMenuState)
{
	if (nMenuState < PLAYER_MENU_STATE_NORMAL || nMenuState >= PLAYER_MENU_STATE_NUM)
	{
		this->m_cMenuStateSpr.Release();
		return;
	}
	char	szName[80];
	g_NpcResList.m_cMenuState.GetStateSpr(nMenuState, szName);
	if (szName[0])
	{
		KImageParam	sImage;
	    m_ImageStore.GetImageParam(szName,ISI_T_SPR,&sImage);
		if (sImage.nInterval <= 0)
			sImage.nInterval = 100;
		if (sImage.nInterval > 1000)
			sImage.nInterval = 1000;
		if (sImage.nNumFramesGroup <= 0)
			sImage.nNumFramesGroup = 1;
		if (sImage.nNumFrames < sImage.nNumFramesGroup)
			sImage.nNumFrames = sImage.nNumFramesGroup;
		m_cMenuStateSpr.SetSprFile(szName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
	}
	else
	{
		this->m_cMenuStateSpr.Release();
	}
}

//---------------------------------------------------------------------------
//	功能：	残影打开关闭
//	参数：	bBlur	if == TRUE  打开  if == FLASE  关闭
//---------------------------------------------------------------------------
void	KNpcRes::SetBlur(BOOL bBlur)
{
	/*if (m_nBlurState == bBlur)
		return;

	m_nBlurState = bBlur;
	if (bBlur)
	{
		m_cNpcBlur.AddObj();
	}
	else
	{
		m_cNpcBlur.RemoveObj();
	}*/
}


//---------------------------------------------------------------------------
//	功能：	获得当前动作的音效文件名
//---------------------------------------------------------------------------
void	KNpcRes::GetSoundName()
{
	if (m_pcResNode)
		m_pcResNode->GetActionSoundName(this->m_nAction, this->m_szSoundName);
}

//---------------------------------------------------------------------------
//	功能：	播放当前动作的音效
//---------------------------------------------------------------------------
void	KNpcRes::PlaySound(int nX, int nY)
{
	if (!m_szSoundName[0])
		return;

	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;

	// 获得屏幕中心点的地图坐标 not end
	//g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);

	//m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	//m_pWave = (KWavSound*)m_pSoundNode->m_lpData;

	if (__pSound)
	{
		//if (__pSound->isBackgroundMusicPlaying())
		//	return;
		//int nVol = -(abs(nX - nCenterX) + abs(nY - nCenterY));
		//m_pWave->Play((nX - nCenterX) * 5,  GetSndVolume(nVol), 0);
	}
}

int	KNpcRes::GetSndVolume(int nVol)
{
//	return (10000 + nVol) * Option.GetSndVolume() / 100 - 10000;
	return 0;
}

void	KNpcRes::StopSound()
{
	/*m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		m_pWave->Stop();
	}*/
}
//---------------------------------------------------------------------------
//	功能：设定头顶状态
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuState(int nState, char *lpszSentence, int nSentenceLength)
{
	if (nState < PLAYER_MENU_STATE_NORMAL || nState >= PLAYER_MENU_STATE_NUM)
	{ 
		return;
	}

	if (nState != m_nMenuState)
	{
		m_nBackMenuState = m_nMenuState;
		//strcpy(m_szBackSentence, m_szSentence);
		m_nMenuState = nState;
	}

	if (nSentenceLength > 0 && lpszSentence)
	{
		if (nSentenceLength >= MAX_SENTENCE_LENGTH)
			nSentenceLength = MAX_SENTENCE_LENGTH - 1;
		//memcpy(m_szSentence, lpszSentence, nSentenceLength);
		//m_szSentence[nSentenceLength] = 0;
	}
	else
	{
		//m_szSentence[0] = 0;
	}

	if (!m_nSleepState)
		SetMenuStateSpr(m_nMenuState);
}

//---------------------------------------------------------------------------
//	功能：获得头顶状态
//---------------------------------------------------------------------------
int		KNpcRes::GetMenuState()
{
	if (m_nSleepState)
		return m_nSleepState; 
	return this->m_nMenuState;
}

//---------------------------------------------------------------------------
//	功能：设定睡眠状态
//---------------------------------------------------------------------------
void	KNpcRes::SetSleepState(BOOL bFlag)
{
	if (bFlag)
	{
		m_nSleepState = PLAYER_MENU_STATE_IDLE;
		SetMenuStateSpr(m_nSleepState);
	}
	else
	{
		m_nSleepState = 0;
		if (m_nMenuState)
		{
			SetMenuStateSpr(m_nMenuState);
		}
		else
		{
			SetMenuStateSpr(PLAYER_MENU_STATE_NORMAL);
		}
	}
}

//---------------------------------------------------------------------------
//	功能：获得睡眠状态
//---------------------------------------------------------------------------
BOOL	KNpcRes::GetSleepState()
{
	return (m_nSleepState ? 1 : 0);
}

//---------------------------------------------------------------------------
//	功能：绘制npc的边框(3D模式中改为加亮)
//---------------------------------------------------------------------------
void	KNpcRes::DrawBorder()
{
	if (!m_pcResNode)
		return;

	int		i, nPos = 0;

	for (i = 0; i < MAX_PART; ++i)
	{
		if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
			m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = m_nXpos;
			m_cDrawFile[nPos].oPosition.nY = m_nYpos;
			m_cDrawFile[nPos].oPosition.nZ = m_nZpos;
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_BORDER;//IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;//IMAGE_RENDER_STYLE_BORDER;  //描边绘画
			nPos++;
		}
	}

//	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, FALSE);

	for (i = 0; i < nPos; ++i)
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;//
	nPos = 0;
}

//在指定高度绘制头顶状态
int	KNpcRes::DrawMenuState(int nHeightOffset,int nNpcIndex)
{
	int	nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = 0;

	if (!m_pcResNode)
	{
		if (g_GameWorld)
			g_GameWorld->removeMenuByIdx(nNpcIndex,0);
		return nHeightOffset;
	}

	// 头顶状态特效
	/*int i;
	for ( i = 0; i <12; ++i)
	{
		if (m_cStateSpr[i].m_nID)
		{
			return nHeightOffset;	//有头顶特效时不绘制交易等状态
		}	
	}*/

	int nPos = 0;
	nHeightOffset += 40;
	if (m_cMenuStateSpr.m_szName[0])
	{
		m_cMenuStateSpr.GetNextFrame();
		strcpy(m_cDrawFile[nPos].szImage, m_cMenuStateSpr.m_szName);
		m_cDrawFile[nPos].uImage = m_cMenuStateSpr.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cMenuStateSpr.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOffset;
		nPos++;
		//CCMessageBox(m_cMenuStateSpr.m_szName,"TEST");
		if (g_GameWorld)
			g_GameWorld->DrawPrimitives_MenuState(nNpcIndex,0,0,m_cDrawFile,RU_T_IMAGE,0,0,false);
	}
	
//	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, false);
	
	return nHeightOffset;
}

//---------------------------------------------------------------------------
//	功能：动画帧数转换成逻辑方向(0 - 63)   //m_nPifengType
//---------------------------------------------------------------------------
int		KNpcRes::GetNormalNpcStandDir(int nFrame)
{
	if (!m_pcResNode)
		return 0;

	int nTotalFrames = m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, cdo_stand, m_nHelmType,16);
	if (nTotalFrames <= 0)
		return 0;

	nFrame %= nTotalFrames;

	return (MAX_NPC_DIR * nFrame) / nTotalFrames;
}


//---------------------------------------------------------------------------
//	功能：	构造函数
//---------------------------------------------------------------------------
KStateSpr::KStateSpr()
{
	Release();
}

//---------------------------------------------------------------------------
//	功能：	清空，初始化
//---------------------------------------------------------------------------
void	KStateSpr::Release()
{
	m_nID = 0;
	m_nType = 0;
	m_nPlayType = 0;
	m_nBackStart = 0;
	m_nBackEnd = 0;
	m_SprContrul.Release();
}

//---------------------------------------------------------------------------
//	功能：	构造函数		 ---残影绘制
//---------------------------------------------------------------------------
KNpcBlur::KNpcBlur()
{
	m_nActive = 0;
	m_nCurNo = 0;
	m_dwInterval = 3;
	m_dwTimer = 0;
}

//---------------------------------------------------------------------------
//	功能：	析构函数
//---------------------------------------------------------------------------
KNpcBlur::~KNpcBlur()
{
    //Remove();
}


//---------------------------------------------------------------------------
//	功能：	当前编号指针指向下一个(总共7个，指针循环)
//---------------------------------------------------------------------------
void	KNpcBlur::SetNextNo()
{
	m_nCurNo++;
	if (m_nCurNo >= MAX_BLUR_FRAME)
		m_nCurNo = 0;
}

//---------------------------------------------------------------------------
//	功能：	设定当前残影帧地图坐标
//---------------------------------------------------------------------------
void	KNpcBlur::SetMapPos(int x, int y, int z, int nNpcIdx)
{
	/*m_nMapXpos[m_nCurNo] = x;
	m_nMapYpos[m_nCurNo] = y;
	m_nMapZpos[m_nCurNo] = z;
    m_SceneIDNpcIdx[m_nCurNo] = nNpcIdx;
	g_ScenePlace.MoveObject(CGOG_NPC_BLUR_DETAIL(m_nCurNo), nNpcIdx, x, y, z, m_SceneID[m_nCurNo]);
	*/
}

//---------------------------------------------------------------------------
//	功能：	改变alpha度
//---------------------------------------------------------------------------
void	KNpcBlur::ChangeAlpha()
{
	/*if (m_nActive == 0)
		return;

	int		i, j;
	int		nScreenX, nScreenY, nScreenZ;
	for (i = 0; i < MAX_BLUR_FRAME; ++i)
	{
		nScreenX = m_nMapXpos[i];
		nScreenY = m_nMapYpos[i];
		nScreenZ = m_nMapZpos[i];
//		SubWorld[0].Mps2Screen(&nScreenX, &nScreenY);
		for (j = 0; j < MAX_PART; ++j)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				m_Blur[i][j].oPosition.nX = nScreenX;
				m_Blur[i][j].oPosition.nY = nScreenY;
				m_Blur[i][j].oPosition.nZ = nScreenZ;
			}
		}
	}

	m_dwTimer++;
	if (m_dwTimer < m_dwInterval)
		return;
	m_dwTimer = 0;

	m_nActive = 0;
	for (i = 0; i < MAX_BLUR_FRAME; ++i)
	{
		for (j = 0; j < MAX_PART; ++j)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				if (m_Blur[i][j].Color.Color_b.a > BLUR_ALPHA_CHANGE)
					m_Blur[i][j].Color.Color_b.a -= BLUR_ALPHA_CHANGE;
				else
					m_Blur[i][j].Color.Color_b.a = 0;

				m_nActive = 1;
			}
		}
	}
*/
}

//---------------------------------------------------------------------------
//	功能：	清空当前指针指向的内容
//---------------------------------------------------------------------------
void	KNpcBlur::ClearCurNo()
{
	for (int i = 0; i < MAX_PART; ++i)
	{
		m_Blur[m_nCurNo][i].Color.Color_b.a = 0;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前某一项的内容
//---------------------------------------------------------------------------
void	KNpcBlur::SetFile(int nNo, char *lpszFileName, int nSprID, int nFrameNo, int nXpos, int nYpos, int nZpos)
{
	if (nNo < 0 || nNo >= MAX_PART)
		return;
	if (!lpszFileName)
		return;
	strcpy(m_Blur[m_nCurNo][nNo].szImage, lpszFileName);
	m_Blur[m_nCurNo][nNo].uImage = nSprID;
	m_Blur[m_nCurNo][nNo].nFrame = nFrameNo;
	m_Blur[m_nCurNo][nNo].oPosition.nX = nXpos;
	m_Blur[m_nCurNo][nNo].oPosition.nY = nYpos;
	m_Blur[m_nCurNo][nNo].oPosition.nZ = nZpos;
	m_Blur[m_nCurNo][nNo].Color.Color_b.a = START_BLUR_ALPHA;
	m_nActive = 1;
}

//---------------------------------------------------------------------------
//	功能：	绘制残影
//---------------------------------------------------------------------------
void	KNpcBlur::Draw(int nIdx)
{
	if (m_nActive == 0)
		return;

//	g_pRepresent->DrawPrimitives(MAX_PART, m_Blur[nIdx], RU_T_IMAGE, FALSE);
}

//---------------------------------------------------------------------------
//	功能：	依据时间判断是否取残影
//---------------------------------------------------------------------------
BOOL	KNpcBlur::NowGetBlur()
{
	if (m_dwTimer == 0)
		return TRUE;
	return FALSE;
}

BOOL	KNpcBlur::Init()
{
	for (int i = 0; i < MAX_BLUR_FRAME; ++i)
	{
		for (int j = 0; j < MAX_PART; ++j)
		{
			m_Blur[i][j].nType = ISI_T_SPR;
			m_Blur[i][j].uImage = 0;

			m_Blur[i][j].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_3D;
			//m_Blur[i][j].Color.Color_b.a = 255;	//黑色
			//m_Blur[i][j].Color.Color_dw  = 255;

			m_Blur[i][j].nISPosition = IMAGE_IS_POSITION_INIT;
			m_Blur[i][j].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		}
	}
	return TRUE;
}

void	KNpcBlur::Remove()
{
	for (int i = 0; i < MAX_BLUR_FRAME; ++i)
	{
		if (m_SceneID[i])
		{
//			g_ScenePlace.RemoveObject(CGOG_NPC_BLUR_DETAIL(i), m_SceneIDNpcIdx[i], m_SceneID[i]);
			m_SceneID[i] = 0;
		}
	}
}

void	KNpcBlur::AddObj()
{
}

void	KNpcBlur::RemoveObj()
{

}