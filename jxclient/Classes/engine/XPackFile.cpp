/*****************************************************************************************
//	读取打包文件
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-9-16
*****************************************************************************************/
#include "XPackFile.h"
#include "stdlib.h"
//#include "file.h"
#include "ucl/ucl.h"
//#include <crtdbg.h>

//一个Pack文件具有的头结构:
/*
struct XPackFileHeader
{
	unsigned char cSignature[4];		//四个字节的文件的头标志，固定为字符串'PACK'
	unsigned long uCount;				//文件数据的条目数
	unsigned long uIndexTableOffset;	//索引的偏移量
	unsigned long uDataOffset;			//数据的偏移量
	unsigned long uCrc32;				//校验和
	unsigned char cReserved[12];		//保留的字节
};

#define	XPACKFILE_SIGNATURE_FLAG		0x4b434150	//'PACK'

//Pack中对应每个子文件的索引信息项
struct XPackIndexInfo
{
	unsigned long	uId;				//子文件id
	unsigned long	uOffset;			//子文件在包中的偏移位置
	long			lSize;				//子文件的原始大小
	long			lCompressSizeFlag;	//子文件压缩后的大小和压缩方法
										//最高字节表示压缩方法，见XPACK_METHOD
										//低的三个字节表示子文件压缩后的大小
};

//包文件的压缩方式
enum XPACK_METHOD
{
	TYPE_NONE	= 0x00000000,			//没有压缩
	TYPE_UCL	= 0x01000000,			//UCL压缩
	TYPE_BZIP2	= 0x02000000,			//bzip2压缩
	TYPE_FRAME	= 0x10000000,			//使用了独立帧压缩,子文件为spr类型时才可能用到
	TYPE_METHOD_FILTER = 0x0f000000,	//过滤标记
	TYPE_FILTER = 0xff000000,			//过滤标记
};

//pak包中保存的spr帧信息项
struct XPackSprFrameInfo
{
	long lCompressSize;
	long lSize;
} ;
*/
XPackFile::XPackElemFileCache	XPackFile::ms_ElemFileCache[MAX_XPACKFILE_CACHE];
int								XPackFile::ms_nNumElemFileCache = 0;

XPackFile::XPackFile()
{
	m_hFile = NULL;
	m_uFileSize = 0;
	m_pIndexList = NULL;
	m_nElemFileCount = 0;  // 子文件数量
	m_nSelfIndex     = 0;
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	   InitializeCriticalSection(&m_ReadCritical);
    #else
	   pthread_mutex_init(&m_ReadCritical,NULL);
    #endif
	ZeroMemory(ms_ElemFileCache,sizeof(XPackElemFileCache)*MAX_XPACKFILE_CACHE);
}

XPackFile::~XPackFile()
{
	Close();
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	    DeleteCriticalSection(&m_ReadCritical);
    #else
	    pthread_mutex_destroy(&m_ReadCritical);
    #endif
}

//-------------------------------------------------
//功能：打开包文件
//返回：成功与否
//-------------------------------------------------
bool XPackFile::Open(const char* pszPackFileName, int nSelfIndex)
{
	bool bResult = false;
	Close();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	EnterCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_lock(&m_ReadCritical);
#endif
	m_nSelfIndex = nSelfIndex;
	//m_hFile = ::CreateFile(pszPackFileName, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING , 0, NULL);
	//std::string fullPath = fullPathForFilename(pszFileName);
	m_hFile = fopen(pszPackFileName, "rb");
	
	//CCMessageBox(pszPackFileName,"bool XPackFile::Open start");
	while (m_hFile)
	{
		//m_uFileSize = ::GetFileSize(m_hFile, NULL);
		//m_uFileSize = m_hFile->size();
		
		fseek(m_hFile,0,SEEK_END);
		m_uFileSize = ftell(m_hFile);  //文件的大小
		fseek(m_hFile,0,SEEK_SET);     //指针移至开头

		if (m_uFileSize == 0 || m_uFileSize == INVALID_FILE_SIZE || m_uFileSize <= sizeof(XPackFileHeader))
		{
			break;
		}

		XPackFileHeader	Header;
	   // ZeroMemory(&Header,sizeof(XPackFileHeader));
		memset(&Header,0,sizeof(XPackFileHeader));
		DWORD dwListSize, dwReaded=0;
		//--读取包文件头-- size = fread(pData,sizeof(unsigned char), size,fp);
		char nFileInfo[64]={0};
		//sprintf(nFileInfo,"文件大小:%d",m_uFileSize); fread(lpBuffer, 1, dwReadBytes, m_hFile);
		//fwrite(&header, 1, sizeof(header), output); //写包头结构
		if (!(dwReaded = fread(&Header,1,sizeof(Header),m_hFile)))
		//if (::ReadFile(m_hFile, &Header, sizeof(Header), &dwReaded, NULL) == FALSE)
		{
//          if (::ReadFile(m_hFile, &xHeader, sizeof(xHeader), &dwReaded, NULL) == FALSE)
			break;
		}
		//--包文件标记与内容的合法性判断--
		
		if (dwReaded != sizeof(Header) ||
			*(int*)(&Header.cSignature) != XPACKFILE_SIGNATURE_FLAG ||
			Header.uCount == 0 ||
			Header.uIndexTableOffset < sizeof(XPackFileHeader) ||
			Header.uIndexTableOffset >= m_uFileSize ||
			Header.uDataOffset < sizeof(XPackFileHeader) ||
			Header.uDataOffset >= m_uFileSize)
		{
			
			break;
		}
		//--读取索引子信息表--
		dwListSize = sizeof(XPackIndexInfo) * Header.uCount;
		m_pIndexList = (XPackIndexInfo*)malloc(dwListSize);   //分配内存。。。。
		//printf("---PAK(%s)分配内存: %u Mb,共%d个文件--\n",pszPackFileName,dwListSize/(1024*1024),Header.uCount);
		//if (m_pIndexList == NULL ||
		//	::SetFilePointer(m_hFile, Header.uIndexTableOffset, NULL, FILE_BEGIN) != Header.uIndexTableOffset)
		int  nCurOffset = 0;
		//if (m_pIndexList == NULL || (nCurOffset = fseek(m_hFile,Header.uIndexTableOffset,FILE_BEGIN)) != Header.uIndexTableOffset)
		if (m_pIndexList == NULL)
		{
			//sprintf(nFileInfo,"偏移:%d==%d 文件大小:%d %d |%d = %d",nCurOffset,Header.uIndexTableOffset,m_uFileSize,Header.uCount,dwReaded,sizeof(Header));
			//MessageBox(NULL, nFileInfo,"Information",MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		fseek(m_hFile,Header.uIndexTableOffset,FILE_BEGIN);//设置指针的开始位置
		//if (::ReadFile(m_hFile, m_pIndexList, dwListSize, &dwReaded, NULL) == FALSE)
		if (!(dwReaded = fread(m_pIndexList,1,dwListSize,m_hFile))) //m_uFileSize
		   break;

		if (dwReaded != dwListSize)
			break;

		m_nElemFileCount = Header.uCount;  //子文件数量
		bResult = true;
		break;
	};
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LeaveCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_unlock(&m_ReadCritical);
#endif

	if (bResult == false)
		Close();

	//CCMessageBox(pszPackFileName,"bool XPackFile::Open return");
	return bResult;
}
//-------------------------------------------------
//功能：打开新包文件
//返回：成功与否
//-------------------------------------------------
/*bool XPackFile::OpenX(const char* pszPackFileName, int nSelfIndex)
{
	bool bResult = false;
	Close();
	EnterCriticalSection(&m_ReadCritical);
	m_nSelfIndex = nSelfIndex;
	m_hFile = ::CreateFile(pszPackFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	while (m_hFile != INVALID_HANDLE_VALUE)
	{
		m_uFileSize = ::GetFileSize(m_hFile, NULL);
		if (m_uFileSize == 0 || m_uFileSize == INVALID_FILE_SIZE || m_uFileSize <= sizeof(XPackFileHeaderX))
		{
			break;
		}
		XPackFileHeaderX xHeader;
		DWORD dwListSize, dwReaded;
		//--读取包文件头--
		if (::ReadFile(m_hFile, &xHeader, sizeof(xHeader), &dwReaded, NULL) == FALSE)
		{
			break;
		}
		//--包文件标记与内容的合法性判断--
		if (dwReaded != sizeof(xHeader) ||
			*(int*)(&xHeader.cSignature) != XPACKFILE_SIGNATURE_FLAG ||
			xHeader.uCount == 0 ||
			xHeader.SubHeader[0].uIndexTableOffset < sizeof(XPackFileHeaderX) ||
			xHeader.SubHeader[0].uIndexTableOffset >= m_uFileSize ||
			xHeader.SubHeader[0].uDataOffset < sizeof(XPackFileHeaderX) ||
			xHeader.SubHeader[0].uDataOffset >= m_uFileSize)
		{
			break;
		}
		
		//--读取索引子信息表--
		dwListSize = sizeof(XPackIndexInfoX) * xHeader.SubHeader[0].uCount;
		m_pIndexListX = (XPackIndexInfoX*)malloc(dwListSize);   //分配内存。。。。
		if (m_pIndexListX == NULL ||
			::SetFilePointer(m_hFile, xHeader.SubHeader[0].uIndexTableOffset, NULL, FILE_BEGIN) != xHeader.SubHeader[0].uIndexTableOffset)
		{//设置当前文件读取位置
			break;
		}
		//直接读取子文件
		if (::ReadFile(m_hFile, m_pIndexListX, dwListSize, &dwReaded, NULL) == FALSE)
			break;
		if (dwReaded != dwListSize)
			break;		
		m_nElemFileCount= xHeader.SubHeader[0].uCount;  //子文件数量
		bResult = true;
		break;
	};
	if (bResult == false)
		Close();
	LeaveCriticalSection(&m_ReadCritical);
	return bResult;
}	*/
//-------------------------------------------------
//功能：关闭包文
//-------------------------------------------------
void XPackFile::Close()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	EnterCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_lock(&m_ReadCritical);
#endif

	if (m_pIndexList)
	{
		//----清除cache中缓存到的（可能）是此包中的子文件----
		for (int i = ms_nNumElemFileCache - 1; i >=0; i--)
		{
			if (ms_ElemFileCache[i].nPackIndex == m_nSelfIndex)
			{
				FreeElemCache(i);
				ms_nNumElemFileCache --;
				for (int j = i; j < ms_nNumElemFileCache; ++j)
					ms_ElemFileCache[j] = ms_ElemFileCache[j + 1];
			}
		}
		free (m_pIndexList);
		m_pIndexList = NULL;
	}
	m_nElemFileCount = 0;

	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
	m_uFileSize = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LeaveCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_unlock(&m_ReadCritical);
#endif
}

//-------------------------------------------------
//功能：释放一个cache结点的内容
//返回：成功与否
//-------------------------------------------------
void XPackFile::FreeElemCache(int nCacheIndex)
{
	//_ASSERT(nCacheIndex >= 0 && nCacheIndex < ms_nNumElemFileCache);
	if (nCacheIndex<0 || nCacheIndex >= ms_nNumElemFileCache) return;
	if (ms_ElemFileCache[nCacheIndex].pBuffer)
	{
		///free(ms_ElemFileCache[nCacheIndex].pBuffer);　
		ms_ElemFileCache[nCacheIndex].pBuffer = NULL;
	}
	ms_ElemFileCache[nCacheIndex].uId = 0;
	ms_ElemFileCache[nCacheIndex].lSize = 0;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0;
	ms_ElemFileCache[nCacheIndex].nPackIndex = -1;
}

//-------------------------------------------------
//功能：直接读取包文件数据中的数据到缓冲区
//返回：成功与否
//-------------------------------------------------
bool XPackFile::DirectRead(void* pBuffer, unsigned int uOffset, unsigned int uSize) const
{
	bool bResult = false;
	DWORD dwReaded = 0;
	//_ASSERT(pBuffer && m_hFile != INVALID_HANDLE_VALUE);
    if  (pBuffer==NULL || !m_hFile)
		return bResult;

	//if (uOffset + uSize <= m_uFileSize && ::SetFilePointer(m_hFile, uOffset, 0, FILE_BEGIN) == uOffset) //设置当前的读写位置
    //dwBytesRead = fread(lpBuffer, 1, dwReadBytes, m_hFile);	fseek(m_hFile, lDistance, dwMoveMethod);
    if (uOffset + uSize <= m_uFileSize /*&& fseek(m_hFile, 0, FILE_BEGIN) == uOffset*/) //设置当前的读写位置
    {
		//if (ReadFile(m_hFile, pBuffer, uSize, &dwReaded, NULL))
		fseek(m_hFile,uOffset,FILE_BEGIN); //设置指针的开始位置

    	if (dwReaded = fread(pBuffer,1,uSize,m_hFile))
		{
			if (dwReaded == uSize)
				bResult = true;
		}
	}
	return bResult;
}

//-------------------------------------------------
//功能：带解压地读取包文件到缓冲区
//参数：pBuffer --> 缓冲区指针
//		uExtractSize  --> 数据（期望）解压后的大小，pBuffer缓冲区的大小不小于此数
//		lCompressType --> 直接从包中度取得原始（/压缩）大小
//		uOffset  --> 从包中的此偏移位置开始读取
//		uSize    --> 从包中直接读取得（压缩）数据的大小
//返回：成功与否
//-------------------------------------------------
bool XPackFile::ExtractRead(void* pBuffer, unsigned int uExtractSize,
	long lCompressType, unsigned int uOffset, unsigned int uSize) const
{
	if (!pBuffer)return false;
	bool bResult = false;
	if (lCompressType == TYPE_NONE)
	{//没有压缩  直接读取
		if (uExtractSize == uSize)
			bResult = DirectRead(pBuffer, uOffset, uSize);

		//CCMessageBox("DirectRead","ExtractRead");
	}
	else
	{
		//CCMessageBox("ucl_nrv2e_decompress_8_1","ExtractRead");
		void*	pReadBuffer = malloc(uSize);  //分配内存
		//|| lCompressType == TYPE_BZIP2
		if (pReadBuffer)
		{
			if ((lCompressType == TYPE_UCL || lCompressType == TYPE_UCL_OLD/* || lCompressType == TYPE_BZIP2_OLD || lCompressType == TYPE_BZIP2*/) && DirectRead(pReadBuffer, uOffset, uSize))
			{//0x01000000
				unsigned int uDestLength = 0;
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);  //解压读取
				//ucl_nrv2e_decompress_8_1((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength);
				bResult =  (uDestLength == uExtractSize); 
			}
			else if ((/*lCompressType == TYPE_FRAGMENT || */lCompressType ==TYPE_FRAGMENT_OLD) && DirectRead(pReadBuffer, uOffset, uSize))
			{//0x03000000
				unsigned int uDestLength;
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);  //解压读取
				//ucl_nrv2e_decompress_8_2((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength);
				//ucl_nrv2e_decompress_8_fs((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength); 
				bResult =  (uDestLength == uExtractSize);
			}
			else if ((/*lCompressType == TYPE_FRAGMENTA || */lCompressType == TYPE_FRAGMENTA_OLD)  && DirectRead(pReadBuffer, uOffset, uSize))
			{//0x04000000
				unsigned int uDestLength;
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);  //解压读取
				//ucl_nrv2e_decompress_8_3((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength);
				//ucl_nrv2e_decompress_8_fs((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength); 
				bResult =  (uDestLength == uExtractSize);
			}
			else 
			{//新包 
				if (DirectRead(pReadBuffer, uOffset, uSize))  //读入原始数据
				{ 
					unsigned int uDestLength; 
					//ucl_nrv2e_decompress_8_fs((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength); 
					//ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);  //解压读取
					//ucl_nrv2b_decompress_safe_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);  //解压读取
					ucl_decompress_8_New((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength); 

					bResult =  (uDestLength == uExtractSize);                                            //解压数据
				} 
			}  
			free (pReadBuffer);
		}
	}
	return bResult;
}


typedef char                int8;
typedef short               int16;
typedef int	                int32;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
//typedef unsigned __int64    uint64;

#define _BYTE  uint8
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define BYTE1(x)      BYTEn(x,  1)  
//
int  XPackFile::ucl_decompress_8_New(unsigned char * a1, unsigned int a2, unsigned char * a3, unsigned int *a4) const
	//(_BYTE *a1, int a2, int a3, _DWORD *a4)
{
	BYTE *v4; // ecx@1
	int v5; // eax@1
	int v6; // ebx@1
	int v7; // eax@4
	int v8; // edx@7
	int v9; // eax@9
	int v10; // eax@10
	int v11; // eax@13
	unsigned int v12; // ebp@16
	int v13; // ebp@17
	int v14; // eax@20
	int v15; // eax@21
	int v16; // edx@22
	int v17; // eax@24
	int v18; // esi@25
	int v19; // esi@26
	int v20; // eax@28
	int v21; // edx@29
	int v22; // eax@32
	int v23; // edi@37
	BYTE *v24; // edi@37
	int v25; // ecx@40
	int result; // eax@41
	signed int v27; // [sp+10h] [bp-4h]@1

	v4 = a1;
	v5 = 0;
	v27 = 1;
	v6 = 0;
	while (1)
	{
		while (1)
		{
			if (v5&0x7F)
			{
				v5 *= 2;
			}
			else
			{
				v7 = *v4++;
				v5 = 2 * v7 + 1;
			}
			if (!(v5&0x0100)) //(v5&0x100)
				break;
			*(v6++ + a3) = *v4++;
		}
		v8 = 1;
		do
		{
			if (v5&0x7F)
			{
				v9 = 2 * v5;
			}
			else
			{
				v10 = *v4++;
				v9 = 2 * v10 + 1;
			}
			v8 = ((v9&0x0100) >> 8) + 2 * v8;
			if (v9&0x7F)
			{
				v5 = 2 * v9;
			}
			else
			{
				v11 = *v4++;
				v5 = 2 * v11 + 1;
			}
		}
		while (!(v5&0x0100));
		if ( v8 == 2 )
		{
			v12 = v27;
			goto LABEL_19;
		}
		v13 = *v4++ + ((v8 + 0x00fffffd) << 8);
		if ( v13 == 0xffffffff )
			break;
		v12 = v13 + 1;
		v27 = v12;
LABEL_19:
		if ( v5 & 0x7F )
		{
			v14 = 2 * v5;
		}
		else
		{
			v15 = *v4++;
			v14 = 2 * v15 + 1;
		}
		v16 = (v14&0x0100)>> 8;
		if ( v14 & 0x7F )
		{
			v5 = 2 * v14;
		}
		else
		{
			v17 = *v4++;
			v5 = 2 * v17 + 1;
		}
		v18 = ((v5&0x0100) >> 8) + 2 * v16;
		if ( !v18 )
		{
			v19 = 1;
			do
			{
				if ( v5 & 0x7F ) //一个字节?
				{
					v20 = 2 * v5;
				}
				else
				{
					v21 = *v4++;
					v20 = 2 * v21 + 1;
				}
				v19 = ((v20&0x0100) >> 8) + 2 * v19;
				if ( v20 & 0x7F )
				{
					v5 = 2 * v20;
				}
				else
				{
					v22 = *v4++;
					v5 = 2 * v22 + 1;
				}
			}
			while (!(v5&0x0100));
			v18 = v19 + 2;
		}
		if ( v12 > 0x0D00 )
			++v18;
		v23 = v6++ - v12;
		*(v6 + a3 - 1) = *(v23 + a3);
		v24 = (v23 + a3 + 1);
		do
		{
			*(v6++ + a3) = *v24++;
			--v18;
		}
		while ( v18 );
	}
	v25 = v4 - a1;
	*a4 = v6;
	if ( a2 <= v25 )
	{
		if ( a2 == v25 )
			result = 0;
		else
			result = (a2 >= v25) - 1;
	}
	else
	{
		result = 1;
	}
	return result;
}

//int __cdecl sub_10025D00(int a1, unsigned int a2, int a3, int *a4)
int  XPackFile::ucl_nrv2e_decompress_8_2(unsigned char * a1, unsigned int a2, unsigned char *a3, unsigned int *a4) const
{
	BYTE *v4; // ebp@1
	int v5; // eax@1
	unsigned int v6; // ecx@1
	int v7; // edi@1
	int v8; // eax@4
	int v9; // edx@7
	int v10; // eax@9
	int v11; // eax@10
	int v12; // esi@11
	int v13; // ebx@12
	int v14; // edx@13
	int v15; // eax@17
	int v16; // edx@20
	int v17; // edx@21
	unsigned int v18; // eax@22
	int v19; // eax@26
	int v20; // edi@27
	int v21; // edi@28
	int v22; // eax@30
	int v23; // edx@31
	int v24; // eax@34
	int v25; // edi@37
	BYTE *v26; // esi@37
	int v27; // ebp@37
	char v28; // bl@37
	BYTE *v29; // esi@37
	int v30; // ST10_4@39
	int result; // eax@41
	int v32; // eax@42
	int v33; // [sp+10h] [bp-Ch]@7
	unsigned int v34; // [sp+14h] [bp-8h]@19
	int v35; // [sp+18h] [bp-4h]@1

	v4 = a1;
	v5 = 0;
	v6 = 0;
	v7 = 0;
	v35 = 1;
	while ( 1 )
	{
		while ( 1 )
		{
			if ( v5 & 0x7F )
			{
				v5 *= 2;
			}
			else
			{
				v8 = *(v6++ + v4);
				v5 = 2 * v8 + 1;
			}
			if ( !(v5&0x100) )  //(v5&0x100)
				break;
			*(v7++ + a3) = *(v6++ + v4);
		}
		v33 = v7;
		v9 = 1;
		while ( 1 )
		{
			if ( v5 & 0x7F )
			{
				v10 = 2 * v5;
			}
			else
			{
				v11 = *(v6++ + v4);
				v10 = 2 * v11 + 1;
			}
			v12 = ((v10&0x100) >> 8) + 2 * v9;
			if ( v10 & 0x7F )
			{
				v13 = 2 * v10;
			}
			else
			{
				v14 = *(v6++ + v4);
				v13 = 2 * v14 + 1;
			}
			if (v13&0x100)
				break;
			if ( v13 & 0x7F )
			{
				v5 = 2 * v13;
				v9 = (((unsigned int)(2 * v13) >> 8) & 1) + 2 * v12 - 2;
			}
			else
			{
				v15 = *(v6++ + v4);
				v5 = 2 * v15 + 1;
				v9 = ((v5&0x100) >> 8) + 2 * v12 - 2;
			}
		}
		if ( v12 == 2 )
		{
			v34 = v35;
			if ( v13 & 0x7F )
			{
				v13 *= 2;
				v16 = (v13&0x100) >> 8;
			}
			else
			{
				v17 = *(v6++ + v4);
				v13 = 2 * v17 + 1;
				v16 = (v13&0x100) >> 8;
			}
			goto LABEL_24;
		}
		v18 = ((v12 + 0x00fffffd) << 8) + *(v6++ + v4);
		if ( v18 == 0xffffffff)
			break;
		v16 = ~(BYTE)v18 & 0x100;
		v34 = (v18 >> 1) + 1;
		v35 = (v18 >> 1) + 1;
LABEL_24:
		if ( v13 & 0x7F )
		{
			v5 = 2 * v13;
		}
		else
		{
			v19 = *(v6++ + v4);
			v5 = 2 * v19 + 1;
		}
		v20 = ((v5&0x100) >> 8) + 2 * v16;
		if ( !v20 )
		{
			v21 = 1;
			do
			{
				if ( v5 & 0x7F )
				{
					v22 = 2 * v5;
				}
				else
				{
					v23 = *(v6++ + v4);
					v22 = 2 * v23 + 1;
				}
				v21 = ((v22&0x100) >> 8) + 2 * v21;
				if ( v22 & 0x7F )
				{
					v5 = 2 * v22;
				}
				else
				{
					v24 = *(v6++ + v4);
					v5 = 2 * v24 + 1;
				}
			}
			while ( !(v5&0x100) );
			v20 = v21 + 2;
		}
		v25 = (v34 > 0x500) + v20;
		v26 = (a3 + v33 - v34);
		v27 = v33 + 1;
		v28 = *v26;
		v29 = v26 + 1;
		*(a3 + v33 + 1 - 1) = v28;
		do
		{
			*(a3 + v27++) = *v29++;
			--v25;
		}
		while ( v25 );
		v30 = v27;
		v4 = a1;
		v7 = v30;
	}
	*a4 = v7;
	if ( v6 == a2 )
	{
		result = 0;
	}
	else
	{
		v32 = -(v6 < a2);
		//LOBYTE(v32) = v32 & 0xFC;
		result = v32;// - 201;
	}
	return result;
}
//int __cdecl ucl_nrv2e_decompress_8_3(int a1, unsigned int a2, int a3, int *a4)
int  XPackFile::ucl_nrv2e_decompress_8_1(unsigned char * a1, unsigned int a2, unsigned char *a3, unsigned int *a4) const
{
	unsigned int v4; // ecx@1
	int v5; // eax@1
	int v6; // eax@4
	int v7; // esi@7
	int v8; // eax@9
	int v9; // eax@10
	int v10; // eax@13
	unsigned int v11; // ebp@16
	int v12; // ebp@17
	int v13; // eax@20
	int v14; // eax@21
	int v15; // edx@22
	int v16; // eax@24
	int v17; // edi@25
	int v18; // edi@26
	int v19; // eax@28
	int v20; // edx@29
	int v21; // eax@32
	int v22; // edi@35
	BYTE *v23; // esi@35
	int v24; // edx@35
	char v25; // bl@35
	BYTE *v26; // esi@35
	int result; // eax@39
	int v28; // eax@40
	int v29; // [sp+10h] [bp-8h]@1
	signed int v30; // [sp+14h] [bp-4h]@1

	v4 = 0;
	v5 = 0;
	v29 = 0;
	v30 = 1;
	while ( 1 )
	{
		while ( 1 )
		{
			if ( v5 & 0x7F )
			{
				v5 *= 2;
			}
			else
			{
				v6 = *(v4++ + a1);
				v5 = 2 * v6 + 1;
			}
			if ( !(v5&0x100) ) //(v5&0x100)
				break;
			*(v29++ + a3) = *(v4++ + a1);
		}
		v7 = 1;
		do
		{
			if ( v5 & 0x7F )
			{
				v8 = 2 * v5;
			}
			else
			{
				v9 = *(v4++ + a1);
				v8 = 2 * v9 + 1;
			}
			v7 = ((v8&0x100) >> 8) + 2 * v7;
			if ( v8 & 0x7F )
			{
				v5 = 2 * v8;
			}
			else
			{
				v10 = *(v4++ + a1);
				v5 = 2 * v10 + 1;
			}
		}
		while ( !(v5&0x100) );
		if ( v7 == 2 )
		{
			v11 = v30;
			goto LABEL_19;
		}
		v12 = ((v7 + 16777213) << 8) + *(v4++ + a1);
		if ( v12 == -1 )
			break;
		v11 = v12 + 1;
		v30 = v11;
LABEL_19:
		if ( v5 & 0x7F )
		{
			v13 = 2 * v5;
		}
		else
		{
			v14 = *(v4++ + a1);
			v13 = 2 * v14 + 1;
		}
		v15 = (v13&0x100) >> 8;
		if ( v13 & 0x7F )
		{
			v5 = 2 * v13;
		}
		else
		{
			v16 = *(v4++ + a1);
			v5 = 2 * v16 + 1;
		}
		v17 = ((v5&0x100) >> 8) + 2 * v15;
		if ( !v17 )
		{
			v18 = 1;
			do
			{
				if ( v5 & 0x7F )
				{
					v19 = 2 * v5;
				}
				else
				{
					v20 = *(v4++ + a1);
					v19 = 2 * v20 + 1;
				}
				v18 = ((v19&0x100) >> 8) + 2 * v18;
				if ( v19 & 0x7F )
				{
					v5 = 2 * v19;
				}
				else
				{
					v21 = *(v4++ + a1);
					v5 = 2 * v21 + 1;
				}
			}
			while ( !(v5&0x100) );
			v17 = v18 + 2;
		}
		v22 = (v11 > 0xD00) + v17;
		v23 = (a3 + v29 - v11);
		v24 = v29 + 1;
		v25 = *v23;
		v26 = v23 + 1;
		*(v29 + 1 + a3 - 1) = v25;
		do
		{
			*(v24++ + a3) = *v26++;
			--v22;
		}
		while ( v22 );
		v29 = v24;
	}
	*a4 = v29;
	if ( v4 == a2 )
	{
		result = 0;
	}
	else
	{
		v28 = -(v4 < a2);
		//LOBYTE(v28) = v28 & 0xFC;
		result = v28;// - 201;
	}
	return result;
}
int  XPackFile::ucl_nrv2e_decompress_8_3(unsigned char * a1, unsigned int a2, unsigned char *a3, unsigned int *a4) const
{
	int v4; // eax@1
	unsigned int v5; // ecx@1
	int v6; // ebp@1
	int v7; // eax@4
	int v8; // edx@7
	int v9; // eax@9
	int v10; // eax@10
	int v11; // esi@11
	int v12; // edx@12
	int v13; // edx@13
	int v14; // eax@17
	unsigned int v15; // ebx@19
	int v16; // esi@20
	int v17; // eax@21
	unsigned int v18; // eax@22
	int v19; // esi@26
	int v20; // edx@27
	int v21; // eax@30
	int v22; // esi@33
	int v23; // edx@34
	int v24; // eax@37
	int v25; // eax@38
	int v26; // eax@41
	int v27; // esi@44
	BYTE *v28; // edi@44
	char v29; // bl@44
	BYTE *v30; // edi@44
	int result; // eax@48
	int v32; // eax@49
	int v33; // [sp+10h] [bp-4h]@1

	v4 = 0;
	v5 = 0;
	v6 = 0;
	v33 = 1;
	while ( 1 )
	{
		while ( 1 )
		{
			if ( v4 & 0x7F )
			{
				v4 *= 2;
			}
			else
			{
				v7 = *(v5++ + a1);
				v4 = 2 * v7 + 1;
			}
			if (!(v4&0x100)) //ch & 0x100
				break;
			*(a3 + v6++) = *(v5++ + a1);
		}
		v8 = 1;
		while ( 1 )
		{
			if ( v4 & 0x7F )
			{
				v9 = 2 * v4;
			}
			else
			{
				v10 = *(v5++ + a1);
				v9 = 2 * v10 + 1;
			}
			v11 = ((v9&0x100) >> 8) + 2 * v8;
			if ( v9 & 0x7F )
			{
				v12 = 2 * v9;
			}
			else
			{
				v13 = *(v5++ + a1);
				v12 = 2 * v13 + 1;
			}
			if (v12&0x100)
				break;
			if ( v12 & 0x7F )
			{
				v4 = 2 * v12;
				v8 = (((unsigned int)(2 * v12) >> 8) & 0x100) + 2 * v11 - 2;
			}
			else
			{
				v14 = *(v5++ + a1);
				v4 = 2 * v14 + 1;
				v8 = ((v4&0x100) >> 8) + 2 * v11 - 2;
			}
		}
		if ( v11 != 2 )
			break;
		v15 = v33;
		if ( v12 & 0x7F )
		{
			v12 *= 2;
			v16 = (v12&0x100) >> 8;
		}
		else
		{
			v17 = *(v5++ + a1);
			v12 = 2 * v17 + 1;
			v16 = (v12&0x100) >> 8;
		}
LABEL_24:
		if ( v16 )
		{
			if ( v12 & 0x7F )
			{
				v4 = 2 * v12;
				v19 = (((unsigned int)(2 * v12) >> 8) & 0x100) + 1;
			}
			else
			{
				v20 = *(v5++ + a1);
				v4 = 2 * v20 + 1;
				v19 = ((v4&0x100) >> 8) + 1;
			}
		}
		else
		{
			if ( v12 & 0x7F )
			{
				v4 = 2 * v12;
			}
			else
			{
				v21 = *(v5++ + a1);
				v4 = 2 * v21 + 1;
			}
			if (v4&0x100)
			{
				if ( v4 & 0x7F )
				{
					v4 *= 2;
					v22 = (v4&0x100)>> 8;
				}
				else
				{
					v23 = *(v5++ + a1);
					v4 = 2 * v23 + 1;
					v22 = (v4&0x100) >> 8;
				}
			}
			else
			{
				v22 = 1;
				do
				{
					if ( v4 & 0x7F )
					{
						v24 = 2 * v4;
					}
					else
					{
						v25 = *(v5++ + a1);
						v24 = 2 * v25 + 1;
					}
					v22 = ((v24&0x100) >> 8) + 2 * v22;
					if ( v24 & 0x7F )
					{
						v4 = 2 * v24;
					}
					else
					{
						v26 = *(v5++ + a1);
						v4 = 2 * v26 + 1;
					}
				}
				while (!(v4&0x100));
			}
			v19 = v22 + 3;
		}
		v27 = (v15 > 0x500) + v19;
		v28 = (a3 + v6++ - v15);
		v29 = *v28;
		v30 = v28 + 1;
		*(a3 + v6 - 1) = v29;
		do
		{
			*(a3 + v6++) = *v30++;
			--v27;
		}
		while ( v27 );
	}
	v18 = ((v11 + 0x00fffffd) << 8) + *(v5++ + a1);
	if ( v18 != 0xffffffff)
	{
		v16 = ~v18 & 0x100;
		v15 = (v18 >> 1) + 1;
		v33 = (v18 >> 1) + 1;
		goto LABEL_24;
	}
	*a4 = v6;
	if ( v5 == a2 )
	{
		result = 0;
	}
	else
	{
		v32 = -(v5 < a2);
		//LOBYTE(v32) = v32 & 0xFC;
		result = v32;// - 201;
	}
	return result;
}

//解压函数
int XPackFile::ucl_nrv2e_decompress_8_fs(unsigned char * s_buf,int s_size,unsigned char * d_buf,unsigned int *d_size)  const
{
	unsigned int ch=0;
	unsigned int repeat_pri=1;		    //上一次的 REPEAT VALUE
	unsigned int repeat_cur;			//本次的 REPEAT VALUE
	unsigned int repeat_cnt;			// repeat count
	unsigned int d_cnt=0;
	unsigned char * bak_s_buf;		    //备份的数据
	
	bak_s_buf=s_buf;
	do
	{
		do
		{
			if((ch & 0x7f)!=0)
				ch<<=1;
			else
				ch=((*(s_buf++))<<1)+1;
			if((ch & 0x100)==0) break;
			*(d_buf+d_cnt++)=*(s_buf++);
		}while(1);
		repeat_cur=1;

		do
		{
			if((ch & 0x7f)!=0)
				ch<<=1;
			else
				ch=((*(s_buf++))<<1)+1;
			repeat_cur<<=1;
			repeat_cur +=(ch & 0x100)>>8;
			if((ch & 0x7f)!=0)
				ch<<=1;
			else
				ch=((*(s_buf++))<<1)+1;
		}while((ch & 0x100)==0);

		if(repeat_cur==2)
			repeat_cur=repeat_pri;
		else
		{
			repeat_cur +=0x00fffffd;
			repeat_cur<<=8;
			repeat_cur +=*(s_buf++);
			if(repeat_cur==0xffffffff) break;
			  repeat_cur++;
			repeat_pri=repeat_cur;
		}
		if((ch & 0x7f)!=0)
			ch<<=1;
		else
			ch=((*(s_buf++))<<1)+1;
		repeat_cnt=(ch & 0x100)>>8;
		if((ch & 0x7f)!=0)
			ch<<=1;
		else
			ch=((*(s_buf++))<<1)+1;
		repeat_cnt<<=1;
		repeat_cnt+=(ch & 0x100)>>8;
		if(repeat_cnt==0)
		{
			repeat_cnt=1;
			do
			{
				if((ch & 0x7f)!=0)
					ch<<=1;
				else
					ch=((*(s_buf++))<<1)+1;
				repeat_cnt<<=1;
				repeat_cnt+=(ch & 0x100)>>8;
				if((ch & 0x7f)!=0) //压缩类型
					ch<<=1;
				else
					ch=((*(s_buf++))<<1)+1;
			}while((ch & 0x100)==0);
			repeat_cnt+=2;
		}
		if(repeat_cur>0x0d00) repeat_cnt++;
		repeat_cur=d_cnt-repeat_cur;
		*(d_buf+d_cnt++)=*(d_buf+repeat_cur++);
		do
		{
			*(d_buf+d_cnt++)=*(d_buf+repeat_cur++);
			repeat_cnt--;
		}while(repeat_cnt!=0);

	}while(1);
	*d_size=d_cnt;
	if(s_size>(s_buf-bak_s_buf))
		return 1;
	if(s_size==(s_buf-bak_s_buf))
		return 0;
	if(s_size<(s_buf-bak_s_buf))
		return -1;
	
	return 0;
}
//-------------------------------------------------
//功能：在索引表中查找子文件项(二分法找)
//返回：如找到返回在索引表中的位置(>=0)，如未找到返回-1
//-------------------------------------------------
int XPackFile::XFindElemFileA(unsigned long ulId) const
{
	int nBegin, nEnd, nMid;
	nBegin = 0;
	nEnd = m_nElemFileCount - 1;
	while (nBegin <= nEnd)
	{
		nMid = (nBegin + nEnd) / 2;
		if (ulId < m_pIndexList[nMid].uId)
			nEnd = nMid - 1;
		else if (ulId > m_pIndexList[nMid].uId)
			nBegin = nMid + 1;
		else
			break;
	}
	return ((nBegin <= nEnd) ? nMid : -1);
}

//-------------------------------------------------
//功能：查找包内的子文件
//参数：uId --> 子文件的id
//		ElemRef -->如果找到则在此结构里填上子文件的相关信息
//返回：是否找到
//-------------------------------------------------
bool XPackFile::XFindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	ElemRef.nElemIndex = -1;
	if (uId)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		EnterCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_lock(&m_ReadCritical);
#endif
		ElemRef.nCacheIndex = FindElemFileInCache(uId, -1);  //查找是否在缓存中
		if (ElemRef.nCacheIndex >= 0)
		{
			ElemRef.uId = uId;
			ElemRef.nPackIndex = ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex;
			ElemRef.nElemIndex = ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex;
			ElemRef.nSize = ms_ElemFileCache[ElemRef.nCacheIndex].lSize;
			ElemRef.nOffset = 0;
		}
		else
		{
			ElemRef.nElemIndex = XFindElemFileA(uId);
			if (ElemRef.nElemIndex >= 0)
			{
				ElemRef.uId = uId;
				ElemRef.nPackIndex = m_nSelfIndex;
				ElemRef.nOffset = 0;
				ElemRef.nSize = m_pIndexList[ElemRef.nElemIndex].lSize;
			}
		}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	    LeaveCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_unlock(&m_ReadCritical);
#endif
	}
	return (ElemRef.nElemIndex >= 0);
}

//-------------------------------------------------
//功能：分配缓冲区，并读包内的子文件的内容到其中
//参数：子文件在包内的索引
//返回：成功则返回缓冲区的指针，否则返回空指针
//-------------------------------------------------
void* XPackFile::ReadElemFile(int nElemIndex) const
{
	//_ASSERT(nElemIndex >= 0 && nElemIndex < m_nElemFileCount);
	if (nElemIndex<0 || nElemIndex >= m_nElemFileCount) 
	{
		//MessageBox(NULL,"读包文件失败","Information",MB_OK | MB_ICONEXCLAMATION);
		return NULL;
	}

	void*	pDataBuffer = malloc(m_pIndexList[nElemIndex].lSize);
	if (pDataBuffer)
	{//带压缩读取文件
		/*
		   ~为C++中的一个运算符，意思为：对它后面的值按位取反，
		   所以要使~x等于0，就要使x的所有位（包括符号位）都为1.

		   1、用在类中的析构函数之前，表示该函数是析构函数。如类A的析构函数
		   2、用于数字或者整形变量之前，表示对该数取反操作，
		      其规则是~0=1, ~1=0, 如二进制0101 0101取反后就是1010 1010
		*/
		long lCompressType = m_pIndexList[nElemIndex].lCompressSizeFlag & TYPE_FILTER_OLD;
		unsigned int uSize = m_pIndexList[nElemIndex].lCompressSizeFlag & (~TYPE_FILTER_OLD);
		//unsigned int uSize = m_pIndexList[nElemIndex].lCompressSizeFlag & (~TYPE_FILTER_SIZE);

		if (ExtractRead(pDataBuffer,
				m_pIndexList[nElemIndex].lSize,
				lCompressType,  //不等于过滤标志
				m_pIndexList[nElemIndex].uOffset,
				uSize) == false)
		  {
			//MessageBox(NULL,"读包文件失败A","Information",MB_OK | MB_ICONEXCLAMATION);
			free (pDataBuffer);
			pDataBuffer = NULL;
		  }
	}
	return pDataBuffer;
}

//-------------------------------------------------
//功能：在cache里查找子文件
//参数：uId --> 子文件id
//		nDesireIndex --> 在cache中的可能位置
//返回：成功则返回cache节点索引(>=0),失败则返回-1
//-------------------------------------------------
int XPackFile::FindElemFileInCache(unsigned int uId, int nDesireIndex)
{
	if (nDesireIndex >= 0 && nDesireIndex < ms_nNumElemFileCache &&
		uId == ms_ElemFileCache[nDesireIndex].uId)
	{
		ms_ElemFileCache[nDesireIndex].uRefFlag = 0xffffffff;
		return nDesireIndex;
	}

	nDesireIndex = -1;
	for (int i = 0; i < ms_nNumElemFileCache; ++i)
	{
		if (uId == ms_ElemFileCache[i].uId)
		{
			ms_ElemFileCache[i].uRefFlag = 0xffffffff;
			nDesireIndex = i;
			break;
		}
	}
	return nDesireIndex;
}

//-------------------------------------------------
//功能：把子文件数据添加到cache
//参数：pBuffer --> 存有子文件数据的缓冲区
//		nElemIndex --> 子文件在包中的索引
//返回：添加到cache的索引位置
//-------------------------------------------------
int XPackFile::AddElemFileToCache(void* pBuffer, int nElemIndex)
{
	//_ASSERT(pBuffer && nElemIndex >= 0 && nElemIndex < m_nElemFileCount);
	if (!pBuffer || nElemIndex<0 || nElemIndex >= m_nElemFileCount) return 0;

	int nCacheIndex;
	if (ms_nNumElemFileCache < MAX_XPACKFILE_CACHE)
	{	//找到一个空位置
		nCacheIndex = ms_nNumElemFileCache++;
	}
	else
	{	//释放一个旧的cache节点
		nCacheIndex = 0;
		if (ms_ElemFileCache[0].uRefFlag)
			ms_ElemFileCache[0].uRefFlag --;
		for (int i = 1; i < MAX_XPACKFILE_CACHE; ++i)
		{
			if (ms_ElemFileCache[i].uRefFlag)
				ms_ElemFileCache[i].uRefFlag --;
			if (ms_ElemFileCache[i].uRefFlag < ms_ElemFileCache[nCacheIndex].uRefFlag)
				nCacheIndex = i;

		}
		FreeElemCache(nCacheIndex);
	}
	ms_ElemFileCache[nCacheIndex].pBuffer = pBuffer;
	ms_ElemFileCache[nCacheIndex].uId = m_pIndexList[nElemIndex].uId;
	ms_ElemFileCache[nCacheIndex].lSize = m_pIndexList[nElemIndex].lSize;
	ms_ElemFileCache[nCacheIndex].nPackIndex = m_nSelfIndex;
	ms_ElemFileCache[nCacheIndex].nElemIndex = nElemIndex;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0xffffffff;
	return nCacheIndex;
}

//-------------------------------------------------
//功能：读取子文件一定长度的数据到缓冲区
//参数：pBuffer --> 用来读取数据的缓冲区
//		uSize --> 要读取的数据的长度
//返回：成功读取得字节数
//-------------------------------------------------
int XPackFile::XElemFileRead(XPackElemFileRef& ElemRef, void* pBuffer, unsigned uSize)
{
	int nResult = 0;
	if (pBuffer && ElemRef.uId &&ElemRef.nElemIndex >= 0)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		EnterCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_lock(&m_ReadCritical);
#endif
		//--先看是否已经在cache里了---
		ElemRef.nCacheIndex = FindElemFileInCache(ElemRef.uId, ElemRef.nCacheIndex);

		if (ElemRef.nCacheIndex < 0 &&								//在cache里未找到
			ElemRef.nElemIndex < m_nElemFileCount &&
			m_pIndexList[ElemRef.nElemIndex].uId == ElemRef.uId)
		{
			//MessageBox(NULL,"包中存在文件B","Information",MB_OK | MB_ICONEXCLAMATION);
			void*	pDataBuffer = ReadElemFile(ElemRef.nElemIndex); //读包文件

			if (pDataBuffer) //增加文件缓存，返回索引，以便调用访问
			{
				ElemRef.nCacheIndex = AddElemFileToCache(pDataBuffer, ElemRef.nElemIndex);
				//MessageBox(NULL,"增加缓存成功","Information",MB_OK | MB_ICONEXCLAMATION);
			}
		}

		if (ElemRef.nCacheIndex >= 0 &&
			//此下面三项应该展开检查，防止被模块外部改变，引起错误。
			//为效率可考虑省略，但需外部按照规则随便改变ElemRef的内容。
			ElemRef.nPackIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex &&
			ElemRef.nElemIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex &&
			ElemRef.nSize == ms_ElemFileCache[ElemRef.nCacheIndex].lSize
			)
		{
			//_ASSERT(ElemRef.nPackIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nPackIndex);
			//_ASSERT(ElemRef.nElemIndex == ms_ElemFileCache[ElemRef.nCacheIndex].nElemIndex);
			//_ASSERT(ElemRef.nSize == ms_ElemFileCache[ElemRef.nCacheIndex].lSize);
			//MessageBox(NULL,"包中存在文件C","Information",MB_OK | MB_ICONEXCLAMATION);
			if (ElemRef.nOffset < 0)
				ElemRef.nOffset = 0;
			if (ElemRef.nOffset < ElemRef.nSize)
			{
				if (ElemRef.nOffset + (int)uSize <= ElemRef.nSize)
					nResult = uSize;
				else
					nResult = ElemRef.nSize - ElemRef.nOffset;
				memcpy(pBuffer, (char*)ms_ElemFileCache[ElemRef.nCacheIndex].pBuffer + ElemRef.nOffset, nResult);
				ElemRef.nOffset += nResult;
			}
			else
			{
				ElemRef.nOffset = ElemRef.nSize;
			}
		}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		LeaveCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_unlock(&m_ReadCritical);
#endif
	}
	return nResult;
}

#define	NODE_INDEX_STORE_IN_RESERVED	2

SPRHEAD* XPackFile::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	SPRHEAD*		pSpr = NULL;
	bool			bOk = false;

	pOffsetTable = NULL;
	if (ElemRef.uId == 0 || ElemRef.nElemIndex < 0)
		return NULL;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	EnterCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_lock(&m_ReadCritical);
#endif
	if(ElemRef.nElemIndex < m_nElemFileCount && m_pIndexList[ElemRef.nElemIndex].uId == ElemRef.uId)
	{
        //首先检查这个id是什么类型压缩方式 是否分块压缩
		if ((m_pIndexList[ElemRef.nElemIndex].lCompressSizeFlag & TYPE_FRAME) == 0)//如果不是分块压缩
		{//压缩包内SPR读取
			pSpr = (SPRHEAD*)ReadElemFile(ElemRef.nElemIndex);
			if (pSpr)
			{
				if ((*(int*)&pSpr->Comment[0]) == SPR_COMMENT_FLAG) //如果是 SPR 文件
				{
					pOffsetTable = (SPROFFS*)(((char*)pSpr) + sizeof(SPRHEAD) + pSpr->Colors * 3);
					bOk = true;
				}
			}
		}
		else
		{//如果是分块压缩 直接读取
			SPRHEAD Header;
			if (DirectRead(&Header, m_pIndexList[ElemRef.nElemIndex].uOffset, sizeof(SPRHEAD)))
			{//读入文件头信息
				if (*(int*)&(Header.Comment[0]) == SPR_COMMENT_FLAG)  //spr
				{
					unsigned int	u2ListSize = Header.Colors * 3 + Header.Frames * sizeof(XPackSprFrameInfo);
					pSpr = (SPRHEAD*)malloc(sizeof(SPRHEAD) + u2ListSize);
					if (pSpr)
					{//也是加载了全部的数据
						if (DirectRead((char*)(&pSpr[1]),m_pIndexList[ElemRef.nElemIndex].uOffset + sizeof(SPRHEAD),u2ListSize))
						{//然后压入文件头数据
							memcpy(pSpr, &Header, sizeof(SPRHEAD));
							bOk = true;
						}
					}
				}
			}
		}

		if (pSpr)
		{
			if (bOk)
			{
				*((int*)&pSpr->Reserved[NODE_INDEX_STORE_IN_RESERVED]) = ElemRef.nElemIndex;
			}
			else
			{
				free (pSpr);
				pSpr = NULL;
			}
		}
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LeaveCriticalSection(&m_ReadCritical);
#else
	pthread_mutex_unlock(&m_ReadCritical);
#endif
    return pSpr;
}

SPRFRAME* XPackFile::GetSprFrame(SPRHEAD* pSprHeader, int nFrame,unsigned int &nSingFrameSize)
{
	SPRFRAME*	pFrame = NULL;
	if (pSprHeader && nFrame >= 0 && nFrame < pSprHeader->Frames)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		EnterCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_lock(&m_ReadCritical);
#endif
		int nNodeIndex = *((int*)&pSprHeader->Reserved[NODE_INDEX_STORE_IN_RESERVED]);
		if (nNodeIndex >= 0 && nNodeIndex < m_nElemFileCount)
		{
			long lCompressType = m_pIndexList[nNodeIndex].lCompressSizeFlag;
			if ((lCompressType & TYPE_FRAME) != 0) //如果是独立帧压缩的
			{
				bool bOk = false;
				lCompressType &= TYPE_METHOD_FILTER_OLD;  //老包
				long lTempValue = sizeof(SPRHEAD) + pSprHeader->Colors * 3;
				//unsigned int	u2ListSize = Header.Colors * 3 + Header.Frames * sizeof(XPackSprFrameInfo);
				//读出指定帧的信息
				XPackSprFrameInfo* pFrameList = (XPackSprFrameInfo *)((char*)pSprHeader + lTempValue);
				unsigned long	uSrcOffset    = m_pIndexList[nNodeIndex].uOffset + lTempValue +pSprHeader->Frames * sizeof(XPackSprFrameInfo);
			    //nFrame +=1; //0无效 还出错了。。。
				while(nFrame > 0)
				{//偏移移动到指定帧的位置
					uSrcOffset += (pFrameList++)->lCompressSize; //自加前执行
					nFrame--;
				};
				lTempValue = pFrameList->lSize;

				/*if  (nFrame==0)
				{
					char ddd[64];
					sprintf(ddd,"pFrameList->lSize:%d",lTempValue);
					CCMessageBox(ddd,"pFrameList->lSize");
				}*/

				if (lTempValue < 0)
				{
					lTempValue = -lTempValue;
					if (pFrame = (SPRFRAME*)malloc(lTempValue))
						bOk = DirectRead(pFrame,uSrcOffset,lTempValue);

					if (bOk)
						nSingFrameSize = lTempValue;
				}
				else
				{//解压读取
					if (pFrame = (SPRFRAME*)malloc(lTempValue))
					{
						bOk = ExtractRead(pFrame,lTempValue,lCompressType,uSrcOffset,pFrameList->lCompressSize);
					    
						if (bOk)
							nSingFrameSize = lTempValue;//pFrameList->lCompressSize;
					}
				}

				if (bOk == false && pFrame != NULL)
				{
					free(pFrame);
					pFrame = NULL;
					CCMessageBox("GetSprFrame--filed","ExtractRead");
				}
			}
		}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		LeaveCriticalSection(&m_ReadCritical);
#else
		pthread_mutex_unlock(&m_ReadCritical);
#endif
	}

	if (pFrame==NULL) 
		CCMessageBox("pFrame IS NULL","GetSprFrame");

	return pFrame;	
}
