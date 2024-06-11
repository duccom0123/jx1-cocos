// ***************************************************************************************
// 场景对象树上的树叶的函数
// Copyright : Kingsoft 2002
// Author    : wooy(wu yue)
// CreateTime: 2002-11
// ***************************************************************************************
#include "gamecore/KCore.h"
//#include "Windows.h"
#include "KIpotLeaf.h"
#include "KIpotBranch.h"
#include "gamecore/CoreDrawGameObj.h"
#include "gamecore/ImgRef.h"
#include "gamecore/KRUImage.h"
extern KImageStore2 m_ImageStore;
///#include "../../Represent/iRepresent/iRepresentShell.h"
//#include <crtdbg.h>
#define	ONE_OF_COS_30					1.1547005383792515290182975610039

void KIpotRuntimeObj::Pluck()
{
	if (pAheadBrother)
		pAheadBrother->pBrother = pBrother;
	if (pParentBranch)
	{
		if (//(pParentBranch->m_uFlag & KIpotBranch::IPOT_BF_HAVE_LEFT_BRANCH) == 0 &&
			pParentBranch->m_pLeafs[0] == (KIpotLeaf*)this)
		{
			pParentBranch->m_pLeafs[0] = pBrother;
		}
		else if (//(pParentBranch->m_uFlag & KIpotBranch::IPOT_BF_HAVE_RIGHT_BRANCH) == 0 &&
			pParentBranch->m_pLeafs[1] == (KIpotLeaf*)this)
		{
			pParentBranch->m_pLeafs[1] = pBrother;
		}
		pParentBranch = NULL;
	}
	if (pParentLeaf)
	{
		if (pParentLeaf->pLChild == (KIpotLeaf*)this)
			pParentLeaf->pLChild = pBrother;
		else if (pParentLeaf->pRChild == (KIpotLeaf*)this)
			pParentLeaf->pRChild = pBrother;
		pParentLeaf = NULL;
	}

	//_ASSERT(pLChild == NULL && pRChild == NULL);
	if (pBrother && pBrother->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)
		((KIpotRuntimeObj*)pBrother)->pAheadBrother = pAheadBrother;
	pAheadBrother = NULL;
	pBrother = NULL;
}

void KIpotLeaf_EnumerateObjects(KIpotLeaf* pLeaf, void* p, ObjectsCallbackFn CallbackFn)
{
	while(pLeaf)
	{
		if (pLeaf->pLChild)
			KIpotLeaf_EnumerateObjects(pLeaf->pLChild, p, CallbackFn);
		CallbackFn(p, pLeaf);
		if (pLeaf->pRChild)
			KIpotLeaf_EnumerateObjects(pLeaf->pRChild, p, CallbackFn);
		pLeaf = pLeaf->pBrother;
	};
}

extern int		g_bShowObstacle;

void KIpotLeaf_PaintObjectLayer(KIpotLeaf* pLeaf, RECT* pRepresentArea)
{
	return;
	while(pLeaf)
	{
		if (pLeaf->pLChild)
			KIpotLeaf_PaintObjectLayer(pLeaf->pLChild, pRepresentArea); //绘画子节点
		//paint self
		if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)
		{//人物怪物等
			/*KIpotRuntimeObj* pObj = (KIpotRuntimeObj*)pLeaf;
			if ((pObj->eLayerParam & IPOT_RL_OBJECT) &&
				pObj->oPosition.y >= pRepresentArea->top &&
				pObj->oPosition.y < pRepresentArea->bottom &&
				pObj->oPosition.x >= pRepresentArea->left &&
				pObj->oPosition.x < pRepresentArea->right)
			{
				CoreDrawGameObj(pObj->uGenre, pObj->nId, pObj->oPosition.x,pObj->oPosition.y, 0, 0, IPOT_RL_OBJECT);
			}*/
		}
		else// if (eLeafType == IPOTL_T_BUILDIN_OBJ)
		{//树木房屋
			((KIpotBuildinObj*)pLeaf)->PaintABuildinObject(pRepresentArea);
		}

		if (pLeaf->pRChild)
			KIpotLeaf_PaintObjectLayer(pLeaf->pRChild, pRepresentArea);

		pLeaf = pLeaf->pBrother;
	}
}

void KIpotLeaf_PaintNoneObjectLayer(KIpotLeaf* pLeaf, RECT* pRepresentArea, int eLayer)
{
	while(pLeaf)
	{
		if (pLeaf->pLChild)
			KIpotLeaf_PaintNoneObjectLayer(pLeaf->pLChild, pRepresentArea, eLayer);

		else if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ &&  ((((KIpotRuntimeObj*)pLeaf)->eLayerParam) & eLayer))
		{
			if (pLeaf->oPosition.x >= pRepresentArea->left && pLeaf->oPosition.x < pRepresentArea->right &&
				pLeaf->oPosition.y >= pRepresentArea->top && pLeaf->oPosition.y < pRepresentArea->bottom)
			{//描绘游戏对象
				KIpotRuntimeObj* pObj = (KIpotRuntimeObj*)pLeaf;
				if (pObj)
				   CoreDrawGameObj(pObj->uGenre, pObj->nId, pObj->oPosition.x,pObj->oPosition.y, 0, 0, eLayer);
			}
		}
	    
		if (pLeaf->pRChild)
			KIpotLeaf_PaintNoneObjectLayer(pLeaf->pRChild, pRepresentArea, eLayer);

		pLeaf = pLeaf->pBrother;
	}
}
//房屋树木的绘画
void KIpotBuildinObj::PaintABuildinObject(RECT* pRepresentArea,int bRegionIndex)
{
	if (!g_GameWorld)
		return;
	KRUImage4	Image;
	Image.Color.Color_dw = 0;
	Image.nType = ISI_T_SPR;
	Image.nFrame = pBio->nFrame;
	Image.nISPosition = nISPosition;
	strcpy(Image.szImage, pBio->szImage);
	Image.uImage = uImage;
	if ((pBio->Props & SPBIO_F_HIGHT_LIGHT) == 0)
	{
		if (pBio->nImgWidth > 128 && pBio->nImgHeight > 128)
			Image.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
		else
		{
			Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			Image.Color.Color_b.a = 255;
		}
	}
	else
		Image.bRenderStyle = IMAGE_RENDER_STYLE_BORDER;

	if (pBio->nAniSpeed == 0)
	{//单帧
		Image.bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
		Image.oPosition.nX = pBio->ImgPos1.x;
		Image.oPosition.nY = pBio->ImgPos1.y;
		Image.oPosition.nZ = pBio->ImgPos1.z;
		
		Image.oEndPos.nX = pBio->ImgPos3.x;
		Image.oEndPos.nY = pBio->ImgPos3.y;
		Image.oEndPos.nZ = pBio->ImgPos3.z;

		if ((pBio->Props & SPBIO_P_SORTMANNER_MASK) == SPBIO_P_SORTMANNER_POINT)
		{//树木花草和一些装饰品
			if (Image.oEndPos.nY >= pRepresentArea->top)
			{   
				// / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
				//pos.y = m_FocusPosition.y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
				POINT nCurPoint;
				nCurPoint.x = Image.oPosition.nX/512;
				nCurPoint.y =(Image.oPosition.nY - 6)/1024;

				//char msg[64];
				//sprintf(msg,"RegX:%d,RegY:%d",nCurPoint.x,nCurPoint.y);
				//CCMessageBox(msg,"绘画2");
				if (Image.szImage[0])
				g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,1,&Image,0,MAP_NODE_TREES,0,false,nCurPoint);
			}
				//g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE, false)
		}
		else
		{
			if (Image.oEndPos.nY > pRepresentArea->top &&
				pBio->ImgPos3.x >= pRepresentArea->left &&
				pBio->ImgPos4.x < pRepresentArea->right)
			{//房屋 墙壁
                Image.oSecondPos.nX = pBio->ImgPos2.x;
				Image.oSecondPos.nY = pBio->ImgPos2.y;
				Image.oSecondPos.nZ = pBio->ImgPos2.z;

				Image.oThirdPos.nX = pBio->ImgPos4.x;
				Image.oThirdPos.nY = pBio->ImgPos4.y;
				Image.oThirdPos.nZ = pBio->ImgPos4.z;

				Image.oImgLTPos.nX = 0;
				Image.oImgLTPos.nY = 0;
				Image.oImgRBPos.nX = pBio->nImgWidth;
				Image.oImgRBPos.nY = pBio->nImgHeight;
				POINT nCurPoint;
				nCurPoint.x = Image.oPosition.nX/512;
				nCurPoint.y =(Image.oPosition.nY - 6)/1024;
				if (Image.szImage[0])
				    g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,1,&Image,0,MAP_NODE_HOUSE,0,false,nCurPoint);
				//g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE_4, false);
				//CCMessageBox(Image.szImage,"绘画2");
			}
		}
	}
	/*else
	{////多帧动画帧
		if (pBio->oPos1.y >= pRepresentArea->top)
		{
			Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			Image.oPosition.nX = pBio->oPos1.x;
			Image.oPosition.nY = pBio->oPos1.y;
			Image.oPosition.nZ = pBio->oPos1.z;
//			g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE, false);
			CCMessageBox(Image.szImage,"绘画3");
		}
		BuildinObjNextFrame(pBio);
	}*/
}

void KIpotLeaf_RemoveAllRtoLeafs(KIpotLeaf* pLeaf, KIpotLeaf* pPermanentRto)
{
	//_ASSERT(pPermanentRto);
	if (pPermanentRto==NULL) return;

	while(pLeaf)
	{
		if (pLeaf->pLChild)
			KIpotLeaf_RemoveAllRtoLeafs(pLeaf->pLChild, pPermanentRto);
		if (pLeaf->pRChild)
			KIpotLeaf_RemoveAllRtoLeafs(pLeaf->pRChild, pPermanentRto);

		KIpotLeaf* pBrother = pLeaf->pBrother;

		if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)
		{
			((KIpotRuntimeObj*)pLeaf)->Pluck();
			if (pPermanentRto->pBrother)
			{
				pLeaf->pBrother = pPermanentRto->pBrother;
				((KIpotRuntimeObj*)pLeaf->pBrother)->pAheadBrother = pLeaf;
			}
			pPermanentRto->pBrother = pLeaf;
			((KIpotRuntimeObj*)pLeaf)->pAheadBrother = pPermanentRto;
		}
		pLeaf = pBrother;
	};
}

void KIpotLeaf_Clear(KIpotLeaf* pLeaf)
{
	while(pLeaf)
	{
		if (pLeaf->pLChild)
		{
			KIpotLeaf_Clear(pLeaf->pLChild);
			pLeaf->pLChild = NULL;
		}
		if (pLeaf->pRChild)
		{
			KIpotLeaf_Clear(pLeaf->pRChild);
			pLeaf->pRChild = NULL;
		}

		if (pLeaf->eLeafType == KIpotLeaf::IPOTL_T_RUNTIME_OBJ)
		{
			((KIpotRuntimeObj*)pLeaf)->pParentBranch = NULL;
			((KIpotRuntimeObj*)pLeaf)->pParentLeaf   = NULL;
			((KIpotRuntimeObj*)pLeaf)->pAheadBrother = NULL;
		}
		KIpotLeaf* pBrother = pLeaf->pBrother;
		pLeaf->pBrother = NULL;
		pLeaf = pBrother;
	};
}

//生成一个克隆体
KIpotBuildinObj* KIpotBuildinObj::Clone(POINT* pDivisionPos)
{
	//_ASSERT(pDivisionPos);
	if (pDivisionPos==NULL) return NULL;
	if (pDivisionPos->x == oEndPos.x)
		return NULL;
	KIpotBuildinObj* pClone = (KIpotBuildinObj*)malloc(sizeof(KIpotBuildinObj));
	if (pClone)
	{
		pClone->bClone = true;
		pClone->eLeafType = eLeafType;
		pClone->fAngleXY = fAngleXY;
		pClone->fNodicalY = fNodicalY;
		pClone->uImage = uImage;
		pClone->nISPosition = nISPosition;
		pClone->oPosition = *pDivisionPos;
		pClone->oEndPos = oEndPos;
		pClone->pBio = pBio;
		pClone->pBrother = NULL;
		pClone->bImgPart = true;

		oEndPos = *pDivisionPos;
		bImgPart = true;
	}
	return pClone;
}

void  BuildinObjNextFrame(KBuildinObj* pBio)
{
	//_ASSERT(pBio);
	if (!pBio)
		return;
	//&& g_pRepresent
	if (pBio->nAniSpeed )
	{
		if (pBio->nAniSpeed == 1)
		{
			KImageParam	Param;
			Param.nNumFrames = 0;

			m_ImageStore.GetImageParam(pBio->szImage,ISI_T_SPR,&Param);
			
			if (Param.nNumFrames > 1)
			{
				pBio->nImgNumFrames = Param.nNumFrames;
				pBio->uFlipTime = IR_GetCurrentTime();
				if ((pBio->nAniSpeed = Param.nInterval) < 20)
					pBio->nAniSpeed = 20;
			}
			else
			{
				pBio->nAniSpeed = 0;
			}
			
		}

		int nFrame = pBio->nFrame;
		IR_NextFrame(nFrame, pBio->nImgNumFrames, pBio->nAniSpeed, pBio->uFlipTime);
		pBio->nFrame = nFrame;
	}
}
