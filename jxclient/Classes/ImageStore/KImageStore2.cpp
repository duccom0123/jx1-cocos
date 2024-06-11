/*****************************************************************************************
//  无贴图的图形资源管理
//	Copyright : Kingsoft 2002-2003
//	Author	: Wooy(Wu yue)
//	CreateTime:	2002-11-11
*****************************************************************************************/
#include "engine/KEngine.h"
#include "KImageStore2.h"
//#include "../iRepresent/KRepresentUnit.h"
//#include <crtdbg.h.>
//#include "../../Engine/Src/KColors.h"

//文件名转化为字符串
unsigned int KImageStore2::ImageNameToId(char* pszName)
{
	//															-哩函数....,偶其实好委哇嘎-
	unsigned long uId = 0;
	if (pszName && pszName[0])
	{
		for (int i = 1; (*pszName); pszName++)
		{
			if ((*pszName) < 'A' || (*pszName) > 'Z')
				uId = (uId + i * (*pszName)) % 0x8000000b * 0xffffffef;
			else						//中文字符的后字节亦可能被转换，人为地加大了重码的概率■
				uId = (uId + i * ((*pszName) + 0x20)) % 0x8000000b * 0xffffffef;
		}
		uId ^= 0x12345678;
	}
	return uId;
}
//bmp
/*
unsigned int KImageStore2::CreateImage(const char* pszName, int nWidth, int nHeight, int nType)
{
	unsigned int uImage = ImageNameToId(pszName);
	KSGImageContent* pBitmap = NULL;
	int nIdx;


    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (nWidth > 0 && nHeight > 0 && uImage && nType == ISI_T_BITMAP16)
	{
		if ((nIdx = FindImage(uImage, 0)) < 0 &&	            //必须是不存在同id的图形
			(m_nNumImages < m_nNumReserved || ExpandSpace()))	//有空间存放图形对象
		{
			pBitmap = (KSGImageContent *)malloc(KSG_IMAGE_CONTENT_SIZE(nWidth, nHeight));
		}
	}
	
	if (pBitmap)
	{
		pBitmap->nWidth = nWidth;
		pBitmap->nHeight = nHeight;

		nIdx = - nIdx - 1;
		for (int i = m_nNumImages; i > nIdx; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}
		m_pObjectList[nIdx].bNotCacheable = true;
		m_pObjectList[nIdx].bSingleFrameLoad = false;
		m_pObjectList[nIdx].bType = ISI_T_BITMAP16;
		m_pObjectList[nIdx].pObject = pBitmap;
		m_pObjectList[nIdx].pFrames = NULL;
		m_pObjectList[nIdx].uId = uImage;
		m_pObjectList[nIdx].pSurface = NULL;
		m_nNumImages++;
	}
	else
		uImage = 0;
	return uImage;
}
*/
void KImageStore2::Free()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (m_pObjectList)
	{
        for (int i = 0; i < m_nNumImages; i++)
			FreeImageObject(m_pObjectList[i]);

		free(m_pObjectList);
		m_pObjectList = NULL;
	}
	m_nNumImages   = 0;
	m_nNumReserved = 0;

	m_uImageAccessCounter = 0;

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

}

void KImageStore2::FreeImage(char* pszImage)
{//释放某张图片
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	unsigned int uImage = ImageNameToId(pszImage);
	int nIdx = FindImage(uImage, 0);
	if (nIdx >= 0)
	{
		FreeImageObject(m_pObjectList[nIdx]);
		m_nNumImages--;
		for (int i = nIdx; i < m_nNumImages; i++)
			m_pObjectList[i] = m_pObjectList[i + 1];
	}

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}
/*
void* KImageStore2::GetExistedCreateBitmap(const char* pszImage, unsigned int uImage, short& nImagePosition)
{
	if (uImage == 0)
		uImage = ImageNameToId(pszImage);

    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	nImagePosition = FindImage(uImage, nImagePosition);
	void* pObject = NULL;
	if (nImagePosition >= 0 &&
		m_pObjectList[nImagePosition].bType == ISI_T_BITMAP16 &&
		m_pObjectList[nImagePosition].bNotCacheable == true &&
		m_pObjectList[nImagePosition].pObject)
	{
		pObject = (m_pObjectList[nImagePosition].pObject);
	}
	return pObject;
}
*/

void* KImageStore2::GetSprFrameData(char* pszImage, unsigned int& uImage,
	short& nImagePosition, int nFrame,unsigned int& nSingFrameSize)
{
	if  (!pszImage)
		return NULL;

	void* pFrameData = NULL;

	if (uImage == 0)
	{
		if ((uImage = ImageNameToId(pszImage)) == 0)
			return NULL;
	}
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{//是否存在 如果存在
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
			if (ImgObj.pObject && ImgObj.bType == ISI_T_SPR)
			{
				/*pObject        = ImgObj.pObject;
				pOffsetTable     = ImgObj.pFrames->pOffsetTable;
				bSingleFrameLoad = ImgObj.bSingleFrameLoad;*/
			    //获取某帧的数据,自动加入缓存
				pFrameData = GetSprFrame(pszImage, ImgObj, nFrame,nSingFrameSize);
			}
	}
	return pFrameData;
}
//spr加载
void* KImageStore2::GetImage(char* pszImage, unsigned int& uImage,
					short& nImagePosition, int nFrame, int nType, void*& pFrameData,void*&pOffsetTable,int& bSingleFrameLoad,unsigned int&nSingFrameSize)
{
	if  (!pszImage)
		return NULL;

	pFrameData = NULL;

	if (uImage == 0)
	{
		if ((uImage = ImageNameToId(pszImage)) == 0)
			return NULL;
	}

    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	
	void* pObject = NULL;
	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{//是否存在 如果存在
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.bType == (unsigned char)nType)
		{
			if (ImgObj.pObject && ImgObj.bType == ISI_T_SPR)
			{
				pObject          = ImgObj.pObject;
				pOffsetTable     = ImgObj.pFrames->pOffsetTable;
				bSingleFrameLoad = ImgObj.bSingleFrameLoad;
			    //获取某帧的数据
				//unsigned int nFramesize =0;
				if ((pFrameData = GetSprFrame(pszImage, ImgObj, nFrame,nSingFrameSize)) == NULL)
					pObject = NULL;
			}
			ImgObj.bRef = true;
		}
		//else 略去同id但是不同图形类型情况的处理。因为这是受限情况
		//为了执行效率所以未把下面的判断放在CheckBalance函数体里面。下同。
		if (m_nNumImages > m_nBalanceNum && (++m_uImageAccessCounter) > m_uCheckPoint)
			CheckBalance();

		return pObject;
	}

	// 如果不存在缓存中 就重新加载 有空间
	if (m_nNumImages < m_nNumReserved || ExpandSpace())
	{
		nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
		for (int i = m_nNumImages; i > nImagePosition; i--)
		{//重新排列数据
			m_pObjectList[i] = m_pObjectList[i - 1];
		}
		
		_KISImageObj &ImgObj = m_pObjectList[nImagePosition];	
		
		ImgObj.bNotCacheable = false;
		ImgObj.bRef = true;
		ImgObj.bSingleFrameLoad = false;  //是否独立帧数 加载
		ImgObj.bType = (unsigned char)nType;
		ImgObj.pFrames = NULL;
		ImgObj.pObject = NULL;
		ImgObj.uId = uImage;
		//ImgObj.pcAdjustColorPalettes = NULL;
		pObject      = LoadImage(pszImage, ImgObj, nFrame, pFrameData,nSingFrameSize);
		if (pObject)
		{
			pOffsetTable     = ImgObj.pFrames->pOffsetTable;
			bSingleFrameLoad = ImgObj.bSingleFrameLoad;
			m_nNumImages++;  //图片的计算+1
			//如果图片的数量大于486 张 就释放内存
			if (m_nNumImages > m_nBalanceNum && (++m_uImageAccessCounter) > m_uCheckPoint)
				CheckBalance();
		}

	}
	return pObject;
}

void* KImageStore2::GetSprFrame(char* pszImageFile, _KISImageObj& ImgObject, int nFrame,unsigned int& nSingFrameSize)
{
	if  (!pszImageFile)
		return NULL;

	SPRHEAD* pSprHeader = (SPRHEAD*)ImgObject.pObject;
	void* pFrameData = NULL;
	if (!pSprHeader)
		return NULL;
	//_ASSERT(pszImageFile && pSprHeader);
	if (nFrame >= 0 && nFrame < pSprHeader->Frames)
	{
		_KISImageFrameObj * pFrame=NULL;
		if (ImgObject.bSingleFrameLoad == false)
		{//不是独立保存的
			pFrame = ImgObject.pFrames;
			//_ASSERT(pFrame);
			//_ASSERT(pFrame->pOffsetTable);
			if (pFrame && pFrame->pOffsetTable)
			{
				pFrameData = (((char*)pFrame->pOffsetTable) + pFrame->sOffTableSize +
					((SPROFFS*)pFrame->pOffsetTable)[nFrame].Offset);
				nSingFrameSize = ((SPROFFS*)pFrame->pOffsetTable)[nFrame].Length;
				pFrame->bRef = true;
			}
		}
		else
		{//
			pFrame = &ImgObject.pFrames[nFrame];
			if (pFrame)
			{
				pFrame->bRef    = true;
				nSingFrameSize  = pFrame->pFrameSize;
				if ((pFrameData = pFrame->pFrameData) == NULL)
				{	//指定的帧数据还不存在
					/*char nRoleInfo[128]={0};
					sprintf(nRoleInfo,"总:%d index:%d",pSprHeader->Frames,nFrame);
					CCMessageBox(nRoleInfo,"独立帧不存在");*/
					unsigned int nFrameSize = 0;
					pFrame->pFrameData = SprGetFrame((SPRHEAD*)ImgObject.pObject,nFrame,nFrameSize);
					pFrameData         = pFrame->pFrameData;
					nSingFrameSize     = nFrameSize;
					pFrame->pFrameSize = nFrameSize;
				}
			}
		}
	}
	return pFrameData;
}
//获取整个spr基本数据
bool KImageStore2::GetImageParam(char* pszImage, int nType, KImageParam* pImageData)
{
	unsigned int uImage = ImageNameToId(pszImage);
	
	if (uImage == 0)
		return false;
    //KAutoCriticalSection AutoLock(m_ImageProcessLock);
	void*  pOffsTable = NULL;
	int    bSingleFrameLoad = false;
	unsigned int    bSingleFrameSize=0;
	short nImagePosition = FindImage(uImage, -1);

	if (nImagePosition < 0)
	{//如果没有
		void* pFrameData;
		if (GetImage(pszImage,uImage,nImagePosition,0,nType,pFrameData,pOffsTable,bSingleFrameLoad,bSingleFrameSize) == NULL)
			nImagePosition = -1;
	}

	if (nImagePosition >= 0)
	{//如果已经加载了
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.pObject && ImgObj.bType == (unsigned char)nType)
		{
			if (pImageData && ImgObj.bType == ISI_T_SPR)
			{
					SPRHEAD* pSprHeader         = (SPRHEAD*)ImgObj.pObject;
					pImageData->nWidth          = pSprHeader->Width;
					pImageData->nHeight         = pSprHeader->Height;
					pImageData->nInterval       = pSprHeader->nInterval;
					pImageData->nNumFrames      = pSprHeader->Frames;
					pImageData->nNumFramesGroup = pSprHeader->Directions;
					pImageData->nReferenceSpotX = pSprHeader->CenterX;
					pImageData->nReferenceSpotY = pSprHeader->CenterY;				
			}
			return true;
		}
	}
	return false;
}
//某帧 的数据 
bool KImageStore2::GetImageFrameParam(char* pszImage, int nType,
		int nFrame, KRPosition2* pOffset, KRPosition2* pSize)
{	
	bool bRet = false;
	short	nPos = -1;
	unsigned int uImage = 0;
	void* pFrame;
	void* pOffsTable = NULL;
	int  bSingleFrameLoad=false;
	unsigned int    bSingleFrameSize=0;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, pFrame,pOffsTable,bSingleFrameLoad,bSingleFrameSize);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			if (pOffset)
			{
				pOffset->nX = ((SPRFRAME*)pFrame)->OffsetX;
				pOffset->nY = ((SPRFRAME*)pFrame)->OffsetY;
			}
			if (pSize)
			{
				pSize->nX = ((SPRFRAME*)pFrame)->Width;
				pSize->nY = ((SPRFRAME*)pFrame)->Height;
			}
		}
		bRet = true;
	}
	return bRet;
}
/*
//获取某点的 通道
int KImageStore2::GetImagePixelAlpha(const char* pszImage, int nType, int nFrame, int nX, int nY)
{
	int nRet = 0;
	short	nPos = -1;
	unsigned int uImage = 0;
	SPRFRAME* pFrame;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, (void*&)pFrame);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			nX -= pFrame->OffsetX;
			nY -= pFrame->OffsetY;
			if (nX >= 0  && nX < pFrame->Width && nY >= 0 && nY < pFrame->Height)
			{
				int	nNumPixels = pFrame->Width;
				void*	pSprite =  pFrame->Sprite;
				nY++;
				_asm
				{
					//使SDI指向sprite中的图形数据位置
					mov		esi, pSprite
				dec_line:
					dec		nY				//减掉一行
					jz		last_line
					
					mov		edx, nNumPixels
				skip_line:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		skip_line_continue
					add		esi, eax
				skip_line_continue:
					sub		edx, eax
					jg		skip_line
					jmp		dec_line

				last_line:
					mov		edx, nX
				last_line_alpha_block:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		last_line_continue
					add		esi, eax
				last_line_continue:
					sub		edx, eax
					jg		last_line_alpha_block

					mov		nRet, ebx
				}
			}
		}
		else if (nType == ISI_T_BITMAP16)
		{
			if (nX >= 0 && nY >= 0 &&
				nX < ((KSGImageContent*)pImage)->nWidth &&
				nY < ((KSGImageContent*)pImage)->nHeight)
			{
				nRet = 255;
			}
		}
	}
	return nRet;
}
*/
bool KImageStore2::Init()
{
	Free();
	return ExpandSpace();
}

bool KImageStore2::SaveImage(const char* pszFile, const char* pszImage, int nFileType)
{
	return 0;
}

void KImageStore2::SetBalanceParam(int nNumImage, unsigned int uCheckPoint)
{
	m_nBalanceNum = nNumImage;
	m_uCheckPoint = uCheckPoint;
}

void KImageStore2::CheckBalance()
{
	m_uImageAccessCounter = 0;
	int i, j;
    int nNewNumImages = 0;
#ifdef WIN32
    MEMORYSTATUS MemStatus;
    GlobalMemoryStatus(&MemStatus);
    // 如果剩余可分配的物理内存大于整个物理内存的一个比例，则不需要进行Cache调整
    if (MemStatus.dwAvailPhys > (MemStatus.dwTotalPhys / 32))
    {
        if (m_nNumImages < 1024)    // Cache中图片的上限，Cache中最多有1024张图片
            return;                 // 不做调整
    }
#else
	if (m_nNumImages < 1024)    // Cache中图片的上限，Cache中最多有1024张图片
		return;                 // 不做调整
#endif

    KAutoCriticalSection AutoLock(m_ImageProcessLock);
    nNewNumImages = 0;
    for (i = 0; i < m_nNumImages; i++)
    {
        if (m_pObjectList[i].bNotCacheable == false)
        {//不保存缓存的
			if (m_pObjectList[i].bRef == false)
			{
				FreeImageObject(m_pObjectList[i]);
                continue;
            }

			if (//独立帧数 加载的 释放掉
                (m_pObjectList[i].bSingleFrameLoad == true) &&
				(m_pObjectList[i].pObject) &&
				(m_pObjectList[i].bType == ISI_T_SPR)
            )
			{
				int nFrameCount = ((SPRHEAD*)(m_pObjectList[i].pObject))->Frames;
				for (j = 0; j < nFrameCount; j++)
				{
					if (m_pObjectList[i].pFrames[j].bRef == false)
					{
						FreeImageObject(m_pObjectList[i],j);
					}
                    else
                    {//如果用的了 设置为false
                        m_pObjectList[i].pFrames[j].bRef = false;
                    }
				}
			}
        }

        m_pObjectList[nNewNumImages] = m_pObjectList[i];
        m_pObjectList[nNewNumImages].bRef = false;
        nNewNumImages++;
    }
    m_nNumImages =  nNewNumImages;

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

}

bool KImageStore2::ExpandSpace()
{
	_KISImageObj* pNewList = (_KISImageObj *)realloc(m_pObjectList,
						(m_nNumReserved + ISBP_EXPAND_SPACE_STEP) * sizeof(_KISImageObj));
	if (pNewList)
	{
		m_pObjectList = pNewList;
		m_nNumReserved += ISBP_EXPAND_SPACE_STEP;
		return true;
	}
	return false;
}

int KImageStore2::FindImage(unsigned int uImage, int nPossiblePosition)
{
	int nPP = nPossiblePosition;
	if (nPP < 0 || nPP >= m_nNumImages)
	{
		if (m_nNumImages <= 0)
		{
			return -1;
		}
		else
		{
			nPP = m_nNumImages / 2;
		}
	}
	if (m_pObjectList[nPP].uId == uImage)
		return nPP;
	int nFrom, nTo, nTryRange;
	nTryRange = ISBP_TRY_RANGE_DEF;
	if (m_pObjectList[nPP].uId > uImage)
	{
		nFrom = 0;
		nTo = nPP - 1;
		nPP -= nTryRange;
	}
	else
	{
		nFrom = nPP + 1;
		nTo = m_nNumImages - 1;
		nPP += nTryRange;
	}
	if (nFrom + nTryRange >= nTo)
		nPP = (nFrom + nTo) / 2;

	while (nFrom < nTo)
	{
		if (m_pObjectList[nPP].uId < uImage)
		{
			nFrom = nPP + 1;
		}
		else if (m_pObjectList[nPP].uId > uImage)
		{
			nTo = nPP - 1;
		}
		else
		{
			return nPP;
		}
		nPP = (nFrom + nTo) / 2;
	}
	if (nFrom == nTo)
	{
		if (m_pObjectList[nPP].uId > uImage)
		{
			nPP = - nPP - 1;
		}
		else if (m_pObjectList[nPP].uId < uImage)
		{
			nPP = - nPP - 2;
		}
	}
	else
	{
		nPP = - nFrom -1;
	}
	return nPP;
}

void KImageStore2::FreeImageObject(_KISImageObj& ImgObject, int nFrame/*=-1*/)
{
	if(ImgObject.pObject)
	{
		if (ImgObject.bType == ISI_T_SPR)
		{
			if (ImgObject.bSingleFrameLoad == false)
			{
				free (ImgObject.pFrames);
				ImgObject.pFrames = NULL;

				if (ImgObject.pObject)
				{
					SprReleaseHeader((SPRHEAD*)ImgObject.pObject);
					ImgObject.pObject = NULL;
				}
			}
			else
			{//独立帧 释放
				int nNumFrame = ((SPRHEAD*)ImgObject.pObject)->Frames;
				if (nFrame >= 0 && nFrame < nNumFrame)
				{
					if (ImgObject.pFrames[nFrame].pFrameData)
					{
						SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
						ImgObject.pFrames[nFrame].pFrameData = NULL;
					}
				}
				else if (nFrame < 0)
				{//
					for (nFrame = 0; nFrame < nNumFrame; nFrame++)
					{//0-95
						if (ImgObject.pFrames[nFrame].pFrameData)
						{
							SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
							ImgObject.pFrames[nFrame].pFrameData =NULL;
						}
					}
					free (ImgObject.pFrames);
					ImgObject.pFrames = NULL;
					SprReleaseHeader((SPRHEAD*)(ImgObject.pObject));
					ImgObject.pObject = NULL;
				}
			}
		}
		else
			ImgObject.pObject = NULL;
	}
}
//spr加载
void* KImageStore2::LoadImage(char* pszImageFile, _KISImageObj& ImgObj, int nFrame, void*& pFrameData,unsigned int& nSingFrameSize)
{
	void* pRet = NULL;
	_KISImageFrameObj* pFrameObj = NULL;
	if (ImgObj.bType == ISI_T_SPR)
	{
		SPROFFS*  pOffsTable = NULL;
		SPRHEAD*  pSprHeader = SprGetHeader(pszImageFile, pOffsTable);
		
		if (pSprHeader)
		{
			if (pOffsTable)	//一次加载完整的spr图
			{
				pFrameObj = (_KISImageFrameObj*)malloc(sizeof(_KISImageFrameObj));
				if (pFrameObj)
				{
					memset(pFrameObj, 0, sizeof(_KISImageFrameObj));
					ImgObj.pObject = pSprHeader;
					ImgObj.pFrames = pFrameObj;
					ImgObj.bNotCacheable = false;
					ImgObj.bSingleFrameLoad = false; //是否独立帧加载
					pFrameObj->pOffsetTable = pOffsTable;
					pFrameObj->sOffTableSize = sizeof(SPROFFS) * pSprHeader->Frames;
					pFrameObj->bRef = true;

					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{//某帧数的数据
						pFrameData     = ((char*)pOffsTable + pFrameObj->sOffTableSize + pOffsTable[nFrame].Offset);
						nSingFrameSize = pOffsTable[nFrame].Length;
						pRet = pSprHeader;
					}					
				}
			}
			else	//分帧加载的图
			{
				int nSize = sizeof(_KISImageFrameObj) * pSprHeader->Frames;
				pFrameObj = (_KISImageFrameObj*)malloc(nSize);//全部帧数的独立数据
				if (pFrameObj)
				{
					memset(pFrameObj, 0, nSize);
					ImgObj.pObject          = pSprHeader;
					ImgObj.pFrames          = pFrameObj;
					ImgObj.bNotCacheable    = false;
					ImgObj.bSingleFrameLoad = true; //是独立帧数的保存的
					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{
						unsigned int nFrameSize = 0;
						pFrameData = (SPRFRAME*)SprGetFrame(pSprHeader,nFrame,nFrameSize);
						if (pFrameData)
						{
							ImgObj.pFrames[nFrame].bRef       = true;
							ImgObj.pFrames[nFrame].pFrameData = (SPRFRAME*)pFrameData;
							ImgObj.pFrames[nFrame].pFrameSize = nFrameSize;//这一帧的长度
							nSingFrameSize                    = nFrameSize;
						    pRet = pSprHeader;
						}
					}
					
				}
			}
			if (!ImgObj.pObject)
			{
				SprReleaseHeader(pSprHeader);
				//ImgObj.pcAdjustColorPalettes = NULL;
			}
		}
	}
	return pRet;
}

KImageStore2::KImageStore2()
{
	m_pObjectList = NULL;
	m_nNumReserved= 0;
	m_nNumImages  = 0;
	m_nBalanceNum = ISBP_BALANCE_NUM_DEF; //484
	m_uCheckPoint = ISBP_CHECK_POINT_DEF; //484
	m_uImageAccessCounter = 0;
	Init();
}

KImageStore2::~KImageStore2()
{
	Free();
}
/*
void KImageStore2::ClearAllAdjustColorPalette()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for (int i = 0; i < m_nNumImages; i++)
	{
		if (m_pObjectList[i].pcAdjustColorPalettes)
		{
			free (m_pObjectList[i].pcAdjustColorPalettes);
			m_pObjectList[i].pcAdjustColorPalettes = NULL;
		}
	}
}*/
