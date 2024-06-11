//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Data Compress / Decompress Base Class
//---------------------------------------------------------------------------
#include "gamecore/KCore.h"
#include "KMemBase.h"
#include "KEngine.h"
#include "Kgetinidata.h"
#include "dataChecksum.h"
//#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
//#define APP_STRINFO_1 "vvlmXQy0KkYOvN9gEi0IW4kkp602Lym0"
//#define APP_STRINFO_2 "IzOINKf_Rz1T0Lvdf106klw0HxpZ4pWh"
//#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
//#endif
KImageStore2 m_ImageStore; //全局调用

#define GET_SPR_PALETTE(pHeader) ( ((char*)pHeader) + sizeof(SPRHEAD))

Kgetinidata::Kgetinidata(void)
{
	nFrameData = NULL;
	pHeader=NULL;
	m_Palette=NULL;
	m_pPal16=NULL;
	m_pOffset=NULL;
	m_pSprite=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_nCenterX=0;
	m_nCenterY=0;
	m_nOffsetX=0;
	m_nOffsetY=0;
	m_nFrames=0;
	m_nColors=0;
	m_nDirections=1;
	m_nInterval=10;
	data = NULL;
	m_size = 0;
	//memset(pPalette,0,sizeof(pPalette));
	//m_ImageStore.Init(); //初始化清空数据
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	m_ReadModel = 1; //包里
#else
	m_ReadModel = 1; //win32 路径读取
#endif
}

//---------------------------------------------------------------------------
// 函数:	GetPackLen
// 功能:	取得压缩输出缓存的最小长度
// 参数:	dwDataLen	待压缩数据长度
// 返回:	压缩输出缓存的最小长度(in Bytes)
// 注释:	有可能压缩后的数据比源数据多, 所以需要得到输出缓冲区的最小长度
//---------------------------------------------------------------------------
unsigned long Kgetinidata::GetLen(unsigned long dwDataLen)
{
	return (dwDataLen + dwDataLen / 10 + 1024);
}
//---------------------------------------------------------------------------
// 函数:	Encode
// 功能:	压缩
// 参数:	pCodeInfo	压缩数据结构
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
/*bool Kgetinidata::SprEncode(TSprCodeInfo* pCodeInfo)
{
	//g_MemCopyMmx(pCodeInfo->lpPack, pCodeInfo->lpData, pCodeInfo->dwDataLen);
	//pCodeInfo->dwPackLen = pCodeInfo->dwDataLen;
	return TRUE;
}
*/
/*
bool Kgetinidata::SaveBMPFile(const char *filename,  const stImageInfo& m_info)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bi;
	bi.biSize   = sizeof(BITMAPINFOHEADER);
	bi.biWidth  = m_info.width;
	bi.biHeight = m_info.height;
	bi.biPlanes = 1;
	bi.biBitCount  = m_info.bytes_per_pixel * 8;
	bi.biCompression = 0;
	bi.biSizeImage   = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed       = 0;
	bi.biClrImportant  = 0;

	bfh.bfType=0x4d42;
	bfh.bfOffBits=sizeof(bfh)+sizeof(BITMAPINFOHEADER);
	bfh.bfSize=bfh.bfOffBits+m_info.width*m_info.height*3;
	bfh.bfReserved1=0;
	bfh.bfReserved2=0;
	FILE * file;
	//CFile file;
	file = fopen(filename, "wb+");

	if (file == NULL)
	{
		return FALSE;
	}
	//if(!file.Open(filename,CFile::modeCreate|CFile::modeWrite))
	//	return -1;
	fwrite(&bfh, 1, sizeof(bfh), file);	
	fwrite(&bi, 1, sizeof(bi), file);	
	fwrite(m_info.buffer,1,m_info.width*m_info.height*3,file);
	//file.Write(&bfh,sizeof(bfh));
	//file.Write(&bi,sizeof(bi));
	//file.Write(m_info.buffer,m_info.width*m_info.height*3);
	//file.Close();
	fclose((FILE*)file);
	
	return true;
}
*/
bool Kgetinidata::ReadFileData(char *nFileName,int nFrameIndex)
{
	//资源文件中的 spr
	 /*if (m_ReadModel)
	 {
		 //g_pPakList->GetSprHeader();
		 //g_pPakList->GetSprFrame()
		 SPROFFS*  pOffsTable = NULL;
		 SPRHEAD*  pSprHeader = NULL;
			 pSprHeader= SprGetHeader(nFileName, pOffsTable);
		 nFrameData = NULL;
		 if (pSprHeader)
		 {   
			 if (pOffsTable)	//一次加载完整的spr图
			 {
					 if (nFrameIndex >= 0 && nFrameIndex < pSprHeader->Frames)
					 {
						 nFrameData = (SPRFRAME*)((char*)pOffsTable + sizeof(SPROFFS) * pSprHeader->Frames + pOffsTable[nFrameIndex].Offset);
					 }					
				 //}
			 }
			 else
			 {
				 unsigned int nFrameSize = 0;
		       nFrameData = SprGetFrame(pSprHeader, nFrameIndex,nFrameSize); //分帧加载
			 }
		 }
		 else
		 {
			 CCMessageBox("pSprHeader is NULL","error");
			 return false;
		 }

		 if (nFrameData==NULL)
		 {
		    SprReleaseHeader(pSprHeader);
			CCMessageBox("FrameFiledata is NULL","error");
			return false;
		 }

		 m_nWidth      = pSprHeader->Width;
		 m_nHeight     = pSprHeader->Height;
		 m_nCenterX    = pSprHeader->CenterX;
		 m_nCenterY    = pSprHeader->CenterY;
		 m_nFrames     = pSprHeader->Frames;
		 m_nColors     = pSprHeader->Colors;
		 m_nDirections = pSprHeader->Directions;
		 m_nInterval   = pSprHeader->nInterval;

		 m_pPal24  = (KPAL24*)GET_SPR_PALETTE(pSprHeader);//(&pSprHeader[1]);
		 m_pOffset = pOffsTable;
		 //m_pSprite = (LPBYTE)(m_pOffset + sizeof(SPROFFS) * pSprHeader->Frames); 
		 for(int i=0;i<m_nColors;i++)
		 {
			 pPalette[i].Red   = m_pPal24->Red;
			 pPalette[i].Green = m_pPal24->Green;
			 pPalette[i].Blue  = m_pPal24->Blue;
			 //pPalette[i]
			 m_pPal24++;
		 }
		 SprReleaseHeader(pSprHeader);
		 return true;
	 }
	 else
	   data = CCFileUtils::sharedFileUtils()->getFileData(nFileName, "rb", &m_size);
	//开始转换成png 数据

	if (!data)
	{
		CCMessageBox("ReadFiledata is NULL","error");
		return false;
	}

	pHeader = (SPRHEAD*)data;
	//get sprite info   获取spr的信息
	m_nWidth      = pHeader->Width;
	m_nHeight     = pHeader->Height;
	m_nCenterX    = pHeader->CenterX;
	m_nCenterY    = pHeader->CenterY;
	m_nFrames     = pHeader->Frames;
	m_nColors     = pHeader->Colors;
	m_nDirections = pHeader->Directions;
	m_nInterval   = pHeader->nInterval;

	// setup palette pointer
	data += sizeof(SPRHEAD);  //色斑开始的指针
	m_pPal24 = (KPAL24*)data; //24色真的色板 记录的一个rgb结构 数据

	// setup offset pointer
	data += m_nColors * sizeof(KPAL24);//每帧偏移记录的开始指针
	m_pOffset = (SPROFFS*)data;

	// setup sprite pointer
	data += m_nFrames * sizeof(SPROFFS);//总帧数 全部位图数据开始指针
	m_pSprite = (LPBYTE)data;           //为图数据
	// make color table
	//m_Palette = new BYTE[m_nColors * sizeof(KPAL16)]; //空的色板

	for(int i=0;i<m_nColors;i++)
	{
		pPalette[i].Red   = m_pPal24->Red;
		pPalette[i].Green = m_pPal24->Green;
		pPalette[i].Blue  = m_pPal24->Blue;
		//pPalette[i]
		m_pPal24++;
	}
*/
	return true;
}


long Kgetinidata::GetGreaterNear2Fold(long d)
{
	long fd = 1;
	while(d > fd)
	{
		fd <<= 1;
	}
	return fd;
}

//free in data, return row reverted data.
PBYTE Kgetinidata::RevertRowRGBA(PBYTE data, long &width, long &height)
{
	if(!data)return NULL;
	long nearW = GetGreaterNear2Fold(width);
	long nearH = GetGreaterNear2Fold(height);
	long size = nearW*nearH*4;
	PBYTE toData = (PBYTE)malloc(size);
	if(!toData)
	{
		free(data);
		return NULL;
	}
	memset(toData, 0, size);
	int rowStep   = width*4;
	int toRowStep = nearW*4;
	int i = 0;
	for(long h = height - 1; h >=0; h--)
	{
		long begin = h * width * 4;
		memcpy(toData + i, data+begin, rowStep);
		i += toRowStep;
	}
	free(data);
	width  = nearW;
	height = nearH;
	return toData;
}

CCTexture2D *Kgetinidata::getpicPath(char *picPath)
{
  if (picPath==NULL)
	  return NULL;

  XPackElemFileRef	m_PackRef;
  ZeroMemory(&m_PackRef,sizeof(XPackElemFileRef));
  CCTexture2D* nPicTexture =NULL;
  char nSprFileKey[64]={0},m_szEntireMapFile[128];
  t_sprintf(m_szEntireMapFile,picPath);
  g_StrLower(m_szEntireMapFile);
  DWORD nFielpahtdwid = g_FileName2Id(m_szEntireMapFile);
  t_sprintf(nSprFileKey,"%u-%d",nFielpahtdwid,0);
  nPicTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFileKey);
  int isHave = false;

  if  (nPicTexture)
	  return nPicTexture;

  if (nPicTexture==NULL)
  {
	if (g_pPakList)
		isHave  = g_pPakList->pGetFilePath(m_szEntireMapFile,m_PackRef);
	if (isHave)
	{
		char * sBuf=NULL;
		sBuf = (char *)malloc(m_PackRef.nSize+1);
		if (g_pPakList) //读取图片的纹理 addImageWihtData
			g_pPakList->ElemFileRead(m_PackRef,sBuf,m_PackRef.nSize);

		if (sBuf)
		{
			nPicTexture = CCTextureCache::sharedTextureCache()->addImageWihtData(nSprFileKey,sBuf,m_PackRef.nSize,0);
			free(sBuf);
			sBuf = NULL;
		}
	}
  }
  return nPicTexture;
}

CCTexture2D *Kgetinidata::getinidata_one(char *nFileName,int nFrameIndex,int *mCurwidth, int *mCurheight,int *mFrams,SPRFRAMSINFO *nSprInfo)
{

	if  (!nFileName || !nFileName[0])
		return NULL;

	if (!strstr(nFileName,".spr"))
		return NULL;

	SPROFFS*  pOffsTable = NULL;
	SPRHEAD*  pSprHeader = NULL;
	SPRFRAME* pFrameData = NULL;
	CCTexture2D* nTextureOld=NULL;
	unsigned int nfleddwid = 0;
	short nImagePosition   = -1; 
	int bSingleFrameLoad = false; //是否独立帧加载
	unsigned int nSingFrameSize = 0;
	//int nFarmIndex = 0;           //0为
	//获取图片的信息
	pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(nFileName,nfleddwid,nImagePosition,nFrameIndex,ISI_T_SPR,(void*&)pFrameData,(void*&)pOffsTable,bSingleFrameLoad,nSingFrameSize);
	if (pSprHeader == NULL || pFrameData==NULL)
	{
		//CCMessageBox(nFileName,"SprHeader is null");
		return NULL;
	}

	if  (!bSingleFrameLoad && pOffsTable==NULL)
	{
		//CCMessageBox(nFileName,"OffsTable is null");
		return NULL;
	}

	if (nSprInfo)
	{
		nSprInfo->mFrams    = pSprHeader->Frames;
		nSprInfo->pHwidth   = pSprHeader->Width;
		nSprInfo->pHheight  = pSprHeader->Height;
		nSprInfo->pHCenterX = pSprHeader->CenterX;
		nSprInfo->pHCenterY = pSprHeader->CenterY;
		nSprInfo->nInterval = pSprHeader->nInterval;
		nSprInfo->pFwidth   = pFrameData->Width;
		nSprInfo->pFheight	= pFrameData->Height;
		nSprInfo->pFoffsetX	= pFrameData->OffsetX;
		nSprInfo->pFoffsetY = pFrameData->OffsetY;
	}
	*mFrams             = pSprHeader->Frames;
	char nSprFilePath[64]={0};
	DWORD nFielpahtdwid = g_FileName2Id(nFileName);
	t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,nFrameIndex);
	if (nTextureOld = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))
	{//如果有存在 就直接返回了
		if(mCurwidth)
		{
			*mCurwidth =(int)pFrameData->Width;//pSprHeader->Width;
		}
		if(mCurheight)
		{
			*mCurheight=(int)pFrameData->Height;//pSprHeader->Height;
		}

		return nTextureOld;
	}

	KPAL24* m_pPal24=NULL;
	Palette_Colour pPalette[256];
	memset(pPalette,0,sizeof(pPalette));
	if (m_ReadModel)
	{
		 m_pPal24      = (KPAL24*)GET_SPR_PALETTE(pSprHeader);//(&pSprHeader[1]);
		 //m_pOffset     = pOffsTable; 
		 for(int i=0;i<pSprHeader->Colors;i++)
		 {
			 pPalette[i].Red   = m_pPal24->Red;
			 pPalette[i].Green = m_pPal24->Green;
			 pPalette[i].Blue  = m_pPal24->Blue;
			 //pPalette[i]
			 m_pPal24++;
		 }
	 }

	SPRFRAME* pFrame = NULL;
	unsigned char* decdata = NULL;

	if (!bSingleFrameLoad && !pOffsTable)
	{
		//CCMessageBox("m_pOffset is Error","m_pOffset");
		return NULL;
	}

	//char nRoleInfo[256]={0};
	if (!bSingleFrameLoad)
	    pFrame       = (SPRFRAME*)pFrameData;//(SPRFRAME*)((char*)pOffsTable + sizeof(SPROFFS)*pSprHeader->Frames + pOffsTable[nFrameIndex].Offset);
	else
        pFrame       = (SPRFRAME*)pFrameData;//(SPRFRAME*)m_ImageStore.GetSprFrameData(nFileName,nfleddwid,nImagePosition,nFrameIndex,nSingFrameSize);

	if  (pFrame==NULL)
	{
		return NULL;
	}

	BYTE *pSrc	      = pFrame->Sprite; //原图压缩数据 //m_pOffset[0].Length;
	long height       = pFrame->Height;
	long width        = pFrame->Width;
	//WORD* mPalette  =(WORD*)m_Palette;//(WORD*)GetPalette();　原始色 为　24色	
	unsigned long datalength = 0;

	if (bSingleFrameLoad)
     //独立帧
		datalength = nSingFrameSize; //独立帧的长度
	else
	    datalength = pOffsTable[nFrameIndex].Length;

	unsigned long curdecposition = 0;
	long decdatalen  = width * height*4;
	 decdata = (unsigned char*)malloc((size_t)decdatalen);
	if (!decdata)
	{
		//CCMessageBox("men Error","for");
		return NULL;
	}

	for(long datidx = 0; datidx < decdatalen;)
	{
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
	}
	//开始解码
	int n=0,calpha;
	unsigned char alpha;
	unsigned long temppos = 0;

#ifndef WIN32
	dataChecksum nappInfo;
	char nstrps[64]={0},destStr[128];
	ZeroMemory(destStr,sizeof(destStr));
	ZeroMemory(nstrps,sizeof(nstrps));
	//njjj.SimplyDecrypt(bstrmak,nstrmak);
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_1);
	strcat(destStr,nstrps);
	ZeroMemory(nstrps,sizeof(nstrps));
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_2);
	strcat(destStr,nstrps);
	if (strcmp(m_MobileKey.c_str(),destStr)!= 0)
		goto _Return;
#endif

	for(unsigned long i = 0; i < datalength - 8;)
	{
		if(curdecposition > (unsigned long)decdatalen)
		{
			break;
		}
		if (i == 0)
		{//一个图像块的数据
			n      = *pSrc++; //当前行的原始数据指针位置
			calpha = *pSrc++; //rgba
		}

		if(calpha == (unsigned char)0x00)
		{
			temppos = n;
			for(unsigned char j = 0; j < temppos; j++)
			{
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
			}
		}
		else
		{
			temppos = n;
			alpha   = calpha;
			for(unsigned char m = 0; m < temppos; m++)
			{
				int nTemp = *pSrc++;
				i++;
				decdata[curdecposition++] = pPalette[nTemp].Red;
				decdata[curdecposition++] = pPalette[nTemp].Green;
				decdata[curdecposition++] = pPalette[nTemp].Blue;
				decdata[curdecposition++] = alpha;
			}
		}
		n     =*pSrc++;
		calpha=*pSrc++;
		i += 2;
	}

	if (decdata)
	  nTextureOld = CCTextureCache::sharedTextureCache()->addImageWihtData(nSprFilePath,decdata,width,height,CCSizeMake((float)(width), (float)(height)));
	
	if(mCurwidth)
	{
		*mCurwidth = (int)width;
	}
	if(mCurheight)
	{
		*mCurheight = (int)height;
	}

	if  (decdata)
	{
		free(decdata);
		decdata =NULL;
	}
	//t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,nFrameIndex);  
_Return:
	return nTextureOld;//CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
}

CCTexture2D *Kgetinidata::getinidata_new(char *nFileName,int nFrameIndex,int *mCurwidth, int *mCurheight,int *mFrams)
//void Kgetinidata::RenderToSprDecode(WORD* m_pDest, BYTE* m_pSrc, int m_width, int m_height, BYTE* m_pPalette,unsigned long nPitch,int m_Mask32)
{
	if  (!nFileName || !nFileName[0])
		return NULL;

	if (!strstr(nFileName,".spr"))
		return NULL;

	SPROFFS*  pOffsTable = NULL;
	SPRHEAD*  pSprHeader = NULL;
	SPRFRAME* pFrameData = NULL;
	CCTexture2D* nTextureOld=NULL;
	unsigned int nfleddwid = 0;
	short nImagePosition   = -1; 
	int bSingleFrameLoad = false; //是否独立帧加载
	unsigned int nSingFrameSize = 0;
	int nFarmIndex = 0;           //0为
	//获取图片的信息
	pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(nFileName,nfleddwid,nImagePosition,nFarmIndex,ISI_T_SPR,(void*&)pFrameData,(void*&)pOffsTable,bSingleFrameLoad,nSingFrameSize);
	if (pSprHeader == NULL || pFrameData==NULL)
	{
		//CCMessageBox(nFileName,"SprHeader is null");
		return NULL;
	}

	if  (!bSingleFrameLoad && pOffsTable==NULL)
	{
		//CCMessageBox(nFileName,"OffsTable is null");
		return NULL;
	}
	
	*mFrams = pSprHeader->Frames;
	char nSprFilePath[64]={0};
	DWORD nFielpahtdwid = g_FileName2Id(nFileName);
	t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0);
	if (nTextureOld = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))
	{//如果有存在 就直接返回了
		if(mCurwidth)
		{
			*mCurwidth =(int)pSprHeader->Width;
		}
		if(mCurheight)
		{
			*mCurheight=(int)pSprHeader->Height;
		}

		return nTextureOld;
	}
	KPAL24* m_pPal24=NULL;
	Palette_Colour pPalette[256];
	memset(pPalette,0,sizeof(pPalette));
	if (m_ReadModel)
	{
		 m_pPal24      = (KPAL24*)GET_SPR_PALETTE(pSprHeader);//(&pSprHeader[1]);
		 //m_pOffset     = pOffsTable; 
		 for(int i=0;i<pSprHeader->Colors;i++)
		 {
			 pPalette[i].Red   = m_pPal24->Red;
			 pPalette[i].Green = m_pPal24->Green;
			 pPalette[i].Blue  = m_pPal24->Blue;
			 //pPalette[i]
			 m_pPal24++;
		 }
	 }

	SPRFRAME* pFrame = NULL;
	unsigned char* decdata = NULL;

#ifndef WIN32
	dataChecksum nappInfo;
	char nstrps[64]={0},destStr[128];
	ZeroMemory(destStr,sizeof(destStr));
	ZeroMemory(nstrps,sizeof(nstrps));
	//njjj.SimplyDecrypt(bstrmak,nstrmak);
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_1);
	strcat(destStr,nstrps);
	ZeroMemory(nstrps,sizeof(nstrps));
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_2);
	strcat(destStr,nstrps);
	if (strcmp(m_MobileKey.c_str(),destStr)!= 0)
		return NULL;
#endif

 for (int k=0;k<pSprHeader->Frames+1;k++)
 {//96
	//nImagePosition = -1;
   
	if (k>=pSprHeader->Frames && decdata)
	{
	    free (decdata);  //删除最后一个
	    decdata =NULL;
	    break;
     }

	if (decdata)
	{
		free (decdata);
		decdata =NULL;
	}

	if (!bSingleFrameLoad && !pOffsTable)
	{
		//CCMessageBox("m_pOffset is Error","m_pOffset");
		break;
	}

	//char nRoleInfo[256]={0};
	if (!bSingleFrameLoad)
	    pFrame       = (SPRFRAME*)((char*)pOffsTable + sizeof(SPROFFS)*pSprHeader->Frames + pOffsTable[k].Offset);
	else
        pFrame       = (SPRFRAME*)m_ImageStore.GetSprFrameData(nFileName,nfleddwid,nImagePosition,k,nSingFrameSize);

	if  (pFrame==NULL)
	{
		continue;
	}

	t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,k);
	if (CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))
		continue;

	BYTE *pSrc	      = pFrame->Sprite; //原图压缩数据 //m_pOffset[0].Length;
	long height       = pFrame->Height;
	long width        = pFrame->Width;
	//WORD* mPalette  =(WORD*)m_Palette;//(WORD*)GetPalette();　原始色 为　24色	
	unsigned long datalength = 0;

	if (bSingleFrameLoad)
     //独立帧 从1 开始 没有第0帧
		datalength = nSingFrameSize; //独立帧的长度
	else
	    datalength = pOffsTable[k].Length;

	unsigned long curdecposition = 0;
	long decdatalen  = width * height*4;
	 decdata = (unsigned char*)malloc((size_t)decdatalen);
	if (!decdata)
	{
		//CCMessageBox("men Error","for");
		break;
	}

	for(long datidx = 0; datidx < decdatalen;)
	{
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
		decdata[datidx++] = 0x00;
	}
	//开始解码
	int n=0,calpha;
	unsigned char alpha;
	unsigned long temppos = 0;
	for(unsigned long i = 0; i < datalength - 8;)
	{
		if(curdecposition > (unsigned long)decdatalen)
		{
			break;
		}
		if (i == 0)
		{//一个图像块的数据
			n      = *pSrc++; //当前行的原始数据指针位置
			calpha = *pSrc++; //rgba
		}

		if(calpha == (unsigned char)0x00)
		{
			temppos = n;
			for(unsigned char j = 0; j < temppos; j++)
			{
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
				decdata[curdecposition++] = 0x0;
			}
		}
		else
		{
			temppos = n;
			alpha   = calpha;
			for(unsigned char m = 0; m < temppos; m++)
			{
				/*if(file.fail())
				{
					file.close();
					free(decdata);
					return NULL;
				}*/
				//file.read((char*)temp,1);
				int nTemp = *pSrc++;
				//*pSrc++;
				i++;
				decdata[curdecposition++] = pPalette[nTemp].Red;
				decdata[curdecposition++] = pPalette[nTemp].Green;
				decdata[curdecposition++] = pPalette[nTemp].Blue;
				decdata[curdecposition++] = alpha;
			}
		}
		//file.read((char*)temp,2);
		n     =*pSrc++;
		calpha=*pSrc++;
		i += 2;
	}

	if (decdata)
	  CCTextureCache::sharedTextureCache()->addImageWihtData(nSprFilePath,decdata,width,height,CCSizeMake((float)(width), (float)(height)));
	
	if(mCurwidth)
	{
		*mCurwidth = (int)width;
	}
	if(mCurheight)
	{
		*mCurheight = (int)height;
	}
/*#ifndef WIN32
	usleep(1000); //1毫秒
#endif*/
  }//end for

//if (pSprHeader) //一次性 全部spr数据释放
	//SprReleaseHeader(pSprHeader);


//独立帧数据释放。。。。
//SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
	//PBYTE rData = NULL;
	/*if(decdata)
	{//反方向 long fcw, fch;
		rData = RevertRowRGBA(decdata,width,height); //得出最终的高度宽度

		if(mCurwidth)
		{
			*mCurwidth = (int)width;
		}
		if(mCurheight)
		{
			*mCurheight = (int)height;
		}
	}*/
	t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0);  
	return CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
}
//---------------------------------------------------------------------------
// 函数:	Decode
// 功能:	解压缩
// 参数:	pCodeInfo	压缩数据结构
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
/*bool Kgetinidata::SprDecode(TSprCodeInfo* pCodeInfo)
{
	//g_MemCopyMmx(pCodeInfo->lpData, pCodeInfo->lpPack, pCodeInfo->dwPackLen);
	//pCodeInfo->dwDataLen = pCodeInfo->dwPackLen;
	return TRUE;
}*/
//---------------------------------------------------------------------------
// 函数:	g_InitCodec
// 功能:	初始化编码、解码器
// 参数:	ppCodec				编码、解码器指针
//			nCompressMethod		压缩方法
// 返回:	void
//---------------------------------------------------------------------------
/*void g_InitCodec(KCodec** ppCodec, int nCompressMethod)
{
	*ppCodec = NULL;

	switch (nCompressMethod)
	{
	case CODEC_NONE:
		*ppCodec = new KCodec;
		break;
//
	case CODEC_LHA:
		*ppCodec = new KCodecLha;
		break;

	case CODEC_LZO:
		*ppCodec = new KCodecLzo;
		break;
		
	}
}
//---------------------------------------------------------------------------
// 函数:	g_FreeCodec
// 功能:	释放编码、解码器
// 参数:	ppCodec				编码、解码器指针
//			nCompressMethod		压缩方法
// 返回:	void
//---------------------------------------------------------------------------
void g_FreeCodec(KCodec** ppCodec, int nCompressMethod)
{
	if (*ppCodec == NULL)
		return;

	switch (nCompressMethod)
	{
	case CODEC_NONE:
		{
		  delete (KCodec *)*ppCodec;
		  *ppCodec=NULL;
		}
		break;
//
//	case CODEC_LHA:
//		delete (KCodecLha *)*ppCodec;
//		break;

	case CODEC_LZO:
		{
        	delete (KCodecLzo *)*ppCodec;
			*ppCodec=NULL;
		}
		break;

	}
	*ppCodec = NULL;
}*/
//---------------------------------------------------------------------------

