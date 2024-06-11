/*******************************************************************************
// FileName			:	KMissleRes.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-7-8 16:21:44
// FileDescription	:	
// Revision Count	:	子弹绘画 
*******************************************************************************/
#include "KCore.h"
#include "engine/KEngine.h"
#include "KMissle.h"
#include "KMissleRes.h"
#include "KSubWorld.h"
#include "KSkillSpecial.h"
#include "ImgRef.h"
#include "KPlayer.h"
/*#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "KOption.h"
#endif*/
#include "KSubWorldSet.h"
KMissleRes::KMissleRes()
{
	m_pSndNode = NULL;
	m_bLoopAnim = 0;
	m_nLastShadowLifeTime = 0;
	m_bHaveEnd = FALSE;
	
	for (int i = 0 ; i < MAX_MISSLE_STATUS; ++i)
	{
		m_RUImage[i].nType = ISI_T_SPR;
		m_RUImage[i].Color.Color_b.a = 255;
		m_RUImage[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT;//IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT;
		m_RUImage[i].uImage = 0;
		m_RUImage[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_RUImage[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}
/*
	
	  IMAGE_RENDER_STYLE_ALPHA = 0,		    //带alpha绘制
	  IMAGE_RENDER_STYLE_OPACITY,			//只分全通透与完全不透明，KRUImage::Alpha值被忽略。
	  IMAGE_RENDER_STYLE_3LEVEL,			//三级alpha绘制，KRUImage::Alpha值被忽略。
	  IMAGE_RENDER_STYLE_BORDER,            //绘画边框
	  IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT,  //带alpha绘制但是不被光照
	  IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST,//带alpha绘制，且带偏色调整	
*/	
#ifdef TOOLVERSION
	m_pSprNode = NULL;
#endif
}

KMissleRes::~KMissleRes()
{
	while(m_ShadowList.GetTail())
	{
		KShadowNode * pShadowNode = (KShadowNode*)m_ShadowList.GetTail();
		delete pShadowNode;
        pShadowNode=NULL;
		m_ShadowList.RemoveTail();
	}
}
/*!*****************************************************************************
// Function		: KMissleRes::LoadResource
// Purpose		: 
// Return		: void 
// Argumant		: eMissleStatus nStatus
// Argumant		: char * MissleImage
// Argumant		: char * MissleSound
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissleRes::LoadResource(int nStatus, char * MissleImage, char * MissleSound)
{
	strcpy(m_MissleRes[nStatus].AnimFileName, MissleImage);
	strcpy(m_MissleRes[nStatus].SndFileName,  MissleSound);
}

BOOL KMissleRes::Init()
{
	Clear();
	return TRUE;
}

void KMissleRes::Remove()
{

}

void KMissleRes::Clear()
{
	m_bNeedShadow = FALSE;

	while (m_ShadowList.GetHead())
		m_ShadowList.RemoveHead();

	m_nLastShadowLifeTime = 0;
	
	while(m_SkillSpecialList.GetHead())
	{//删除第一个?
		KSkillSpecialNode * pNode = (KSkillSpecialNode*)m_SkillSpecialList.GetHead();

		if (pNode)
		{
			//---------------------------
			pNode->m_pSkillSpecial->Remove();
			delete pNode->m_pSkillSpecial;
            pNode->m_pSkillSpecial=NULL;
			//---------------------------
			pNode->Remove();
			delete pNode;
            pNode=NULL;
		}
	}
	
	for(int i = 0; i <MAX_MISSLE_STATUS; ++i)
	{
		m_RUImage[i].szImage[0] = 0;
		m_RUImage[i].uImage = 0;
	}
}

/*!*****************************************************************************
// Function		: KMissleRes::Draw
// Purpose		: 
// Return		: void 
// Argumant		: int nX 实际像素点坐标
// Argumant		: int nY 实际像素点坐标
// Argumant		: int nZ
// Argumant		: int nFrame
// Comments		:
// Author		: RomanDou   子弹绘画
*****************************************************************************/
int KMissleRes::Draw(int eStatus,  int nX, int nY , int nZ, int nDir, int nAllFrame,  int nCurLifeFrame)
{
	//当nAllFrame == 0时，表示为默认数
//	char SprFileName[80];
//	g_SetFilePath("\\");
	//return FALSE;
	if (eStatus == MS_DoWait)
	{
		
	}

	if (eStatus == MS_DoFly)
	{
		if (nCurLifeFrame < 0 || (nAllFrame != 0 && nAllFrame < nCurLifeFrame)) 
			return FALSE;
		
		if (!m_RUImage[eStatus].szImage[0])
		{
//			g_GetFullPath(SprFileName, m_MissleRes[eStatus].AnimFileName);
			//g_StrCpy(m_RUImage[eStatus].szImage, m_MissleRes[eStatus].AnimFileName);
			t_sprintf(m_RUImage[eStatus].szImage, m_MissleRes[eStatus].AnimFileName);
		}
		
		int nSprDir    = m_MissleRes[eStatus].nDir;        //1
		int nSprFrames = m_MissleRes[eStatus].nTotalFrame; //90

		if (nSprDir && nSprFrames)
		{
			//处理子弹显示时实际方向
			int	nImageDir = (nDir / (64 / nSprDir));
			int nImageDir1 = (nDir % (64 / nSprDir));
			if (nImageDir1 >= 32 / nSprDir) 	
				nImageDir ++;
			if (nImageDir >= nSprDir)		
				nImageDir = 0;

			int nFramePerDir = (nSprFrames / nSprDir);	//每个方向有多少帧数

			if (nAllFrame == 0) 
				nAllFrame = nFramePerDir;

			int nFirstFrame = nImageDir * nFramePerDir;
			int nTotalFrame = nSprFrames / nSprDir;
			int nFrame = nCurLifeFrame ;
			{
				if (m_bLoopAnim) //如果是循环播放的话则每帧都换帧
				{
					if (!m_bSubLoop)//无子显示循环
					{	
						nFrame = (nCurLifeFrame/m_MissleRes[eStatus].nInterval)%nTotalFrame;
					}
					else
					{
						//  未显示到循环播放的开始帧
						if ( (nCurLifeFrame/m_MissleRes[eStatus].nInterval) < m_nSubStart)
							nFrame = nCurLifeFrame/m_MissleRes[eStatus].nInterval;
						else
						{
							if (m_nSubStart == m_nSubStop) 
								nFrame = m_nSubStart;
							else
								nFrame = m_nSubStart + ((nCurLifeFrame - m_nSubStart)/m_MissleRes[eStatus].nInterval)%(m_nSubStop - m_nSubStart);
						}
					}
				}
				else
					nFrame = nTotalFrame * nCurLifeFrame/nAllFrame;

				if (nFrame > (nTotalFrame - 1)) 
					return FALSE;
			}
			nFrame = nFirstFrame + nFrame;

			m_RUImage[eStatus].oPosition.nX = nX;
			m_RUImage[eStatus].oPosition.nY = nY;
			m_RUImage[eStatus].oPosition.nZ = nZ;
			m_RUImage[eStatus].nFrame = nFrame;
			//char msg[128];
			//t_sprintf(msg,"misid:%d,nSprDir:%d,TotalFrame:%d nFrame:%d eStatus:%d\n CurLifeFrame:%d nFirstFrame:%d nInterval:%d",m_nMissleId,nSprDir,m_MissleRes[eStatus].nTotalFrame,m_RUImage[eStatus].nFrame,eStatus,nCurLifeFrame,nFirstFrame,m_MissleRes[eStatus].nInterval);
			//CCMessageBox(msg,m_RUImage[eStatus].szImage);
			
			if (g_GameWorld)
			   g_GameWorld->DrawPrimitives_miss(m_nMissleId,eStatus,&m_RUImage[eStatus],0,0,1,0);//[eStatus]
		}
	}

	KSkillSpecialNode * pNode = (KSkillSpecialNode*)m_SkillSpecialList.GetHead(); 
	while (pNode)
	{//碰撞效果
		DWORD dwCurrentTime =  g_SubWorldSet.GetGameTime();
		if(pNode->m_pSkillSpecial->m_nEndTime <= dwCurrentTime)
		{//时间结束 就删除了
			KSkillSpecialNode * pDelNode = (KSkillSpecialNode*)pNode->GetNext();
			//--------------------------
			//--------这里需要做删除精灵处理-------
			//-------------------------------------
			pNode->m_pSkillSpecial->Remove();
			delete pNode->m_pSkillSpecial;
            pNode->m_pSkillSpecial=NULL;
			//--------------------------
			pNode->Remove();
			delete pNode;
            pNode=NULL;
			pNode = pDelNode;
			continue;
		}
		else
		{//否则就绘画
			if (pNode->m_pSkillSpecial->m_nBeginTime <= dwCurrentTime)
				pNode->m_pSkillSpecial->DrawC(dwCurrentTime);
		}
		
		pNode = (KSkillSpecialNode*)pNode->GetNext();
	} 
	return TRUE;
}

/*!*****************************************************************************
// Function		: KMissleRes::PlaySound
// Purpose		: 
// Return		: void 
// Argumant		: int nLoop
// Argumant		: int nPan
// Argumant		: int nVal
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissleRes::PlaySound(int eStatus, int nX, int nY, int nLoop)
{
	if (m_MissleRes[eStatus].SndFileName[0] == 0)	return;

	//int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;
	// 获得屏幕中心点的地图坐标 not end
//	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);

/*	KWavSound * pSound = NULL;
//	g_SetFilePath("\\");
	m_pSndNode	= (KCacheNode*) g_SoundCache.GetNode(m_MissleRes[eStatus].SndFileName, (KCacheNode * ) m_pSndNode);
	pSound		= (KWavSound*) m_pSndNode->m_lpData;
	if (pSound)
	{
	///	if (pSound->IsPlaying()) return ;
		int nVol = -(abs(nX - nCenterX) + abs(nY - nCenterY));
		pSound->Play((nX - nCenterX) * 5,  GetSndVolume(nVol), nLoop);
	}*/
	if (_openSound && __pSound && !m_SDcardDirPath.empty() && m_MissleRes[eStatus].SndFileName[0])
	{
		char nTemp[256];
		ZeroMemory(nTemp,sizeof(nTemp));
		//t_sprintf(nTemp,"%s%s",m_SDcardDirPath.c_str(),m_MissleRes[eStatus].SndFileName);
#ifdef WIN32
		t_sprintf(nTemp,"%s%s",m_SDcardDirPath.c_str(),m_MissleRes[eStatus].SndFileName);
#else
		t_sprintf(nTemp,"%s%s",m_SDcardDirPath.c_str(),G2U(m_MissleRes[eStatus].SndFileName).c_str());
#endif
		bool _loop =false;
		if (nLoop>=1)
			_loop = true;
		//__pSound->playBackgroundMusic(nTemp,_loop);
		//__pSound->stopBackgroundMusic(true);
		__pSound->playEffect(nTemp,_loop);
		//__pSound->unloadEffect(nTemp);
	}
	m_nLastSndIndex = eStatus;
}

int KMissleRes::GetSndVolume(int nVol)
{
	return 0;//(10000 + nVol) * Option.GetSndVolume() / 100 - 10000;
}
/*!*****************************************************************************
// Function		: KMissleRes::StopSound
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissleRes::StopSound()
{
	if (m_MissleRes[m_nLastSndIndex].SndFileName[0] == 0)		return;
	
	/*KWavSound * pSound = NULL;
	m_pSndNode	= (KCacheNode*) g_SoundCache.GetNode(m_MissleRes[m_nLastSndIndex].SndFileName, (KCacheNode * ) m_pSndNode);
	pSound		= (KWavSound*) m_pSndNode->m_lpData;
	if (pSound)
	{
		pSound->Stop();
	}*/

	if (__pSound && !m_SDcardDirPath.empty() && m_MissleRes[m_nLastSndIndex].SndFileName[0])
	{
		char nTemp[256];
		ZeroMemory(nTemp,sizeof(nTemp));
		t_sprintf(nTemp,"%s%s",m_SDcardDirPath.c_str(),m_MissleRes[m_nLastSndIndex].SndFileName);
		unsigned int nRet = _ccHash(nTemp);
		//__pSound->stopBackgroundMusic(true);
		__pSound->stopEffect(nRet);
		__pSound->unloadEffect(nTemp);
		//CCMessageBox(nTemp,"1111");
	}
}