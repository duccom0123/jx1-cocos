//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
//#include <windows.h>
//#include "KWin32.h"
//#include "KDebug.h"
#include "KStrBase.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "KPakList.h"
#include "KGbktoUtf8.h"
//#include "crtdbg.h"

//---------------------------------------------------------------------------
KPakList* g_pPakList = NULL;

//---------------------------------------------------------------------------
// 功能:	购造函数
//---------------------------------------------------------------------------
KPakList::KPakList()
{
	g_pPakList   = this;
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// 功能:	分造函数
//---------------------------------------------------------------------------
KPakList::~KPakList()
{
	Close();
}

//---------------------------------------------------------------------------
// 功能:	关闭所有文件
//---------------------------------------------------------------------------
void KPakList::Close()
{
	for (int i = 0; i < m_nPakNumber; ++i)
	{
		if (m_PakFilePtrList[i])
		{
			delete m_PakFilePtrList[i];
			m_PakFilePtrList[i]=NULL;
		}
	}
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定子文件
// 参数:	uId			文件名ID
//			ElemRef		用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::pFindElemFileA(unsigned long uId, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	for (int i = 0; i < m_nPakNumber; ++i)
	{
		if (m_PakFilePtrList[i]->XFindElemFile(uId, ElemRef))  //查找读取包内文件
		{
			bFounded = true;
			break;
		}
	}
	return bFounded;
}

//---------------------------------------------------------------------------
// 功能:	把文件名转换为包中的id
// 参数:	pszFileName	文件名
// 返回:	文件名对应的包中的id
//---------------------------------------------------------------------------
unsigned long KPakList::FileNameToId(const char* pszFileName)
{
	//_ASSERT(pszFileName && pszFileName[0]);
	if  (!pszFileName && !pszFileName[0])
		return 0;

	unsigned long id = 0;
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
		if(*ptr >= 'A' && *ptr <= 'Z') 
			id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else 
		    id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定文件
// 参数:	pszFileName	文件名
//			ElemRef	用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::pFindElemFile(char* pszFileName, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
		char szPackName[256];
		ZeroMemory(szPackName,sizeof(szPackName));
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			szPackName[0] = '\\';
		#else
			szPackName[0] = '\\';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);//合并路径 去除.. 转换小写
		unsigned long uId = 0;
		uId      = g_FileName2Id(szPackName);
		bFounded = pFindElemFileA(uId, ElemRef);
	}
	return bFounded;
}

//--------------------------------------------------------------------
// 功能:	Open package ini file
// 参数:	char* filename
// 返回:	BOOL
//---------------------------------------------------------------------------
int KPakList::Open(char* pPakListFile,int nKind)
{
	Close();

	KIniFile IniFile;
	#define	SECTION "Package"
	bool bResult = false;
	if (IniFile.Load(pPakListFile)) //加载本地的 packpage.ini
	{
		//CCMessageBox(pPakListFile,"KPakList::Open::IniFile.Load");

		char szBuffer[32]={0}, szKey[32]={0}, szFile[MAX_PATH]={0};

		if (IniFile.GetString(SECTION, "Path","\\data", szBuffer, sizeof(szBuffer)))
		{
#ifndef WIN32
			strcat(szBuffer,"data");
#endif 
			g_GetFullPath(szFile, szBuffer);
			//MessageBox(NULL, szFile,"Information",MB_OK | MB_ICONEXCLAMATION);
			//CCMessageBox(szFile,"IniFile.GetString");

			int nNameStartPos = strlen(szFile);
			if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
			{
				#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
					szFile[nNameStartPos++] = '\\';
				#else
					szFile[nNameStartPos++] = '/';
				#endif

				szFile[nNameStartPos] = 0;
			}

			for (int i = 0; i < MAX_PAK; ++i)   //PAK数量限制
			{
				//itoa(i, szKey, 10);
				t_sprintf(szKey,"%d",i);

				if (!IniFile.GetString(SECTION,szKey,"", szBuffer, sizeof(szBuffer)))
					continue;
				
				if (szBuffer[0] == 0)
					continue;

				strcpy(szFile + nNameStartPos, szBuffer);
				m_PakFilePtrList[m_nPakNumber] = new XPackFile;  //分配内存

				//printf("--(%s)PAK包分配内存:%u b..--\n",szBuffer,sizeof(XPackFile));
					
				if (m_PakFilePtrList[m_nPakNumber])
				{
					if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber)) //检测是否能打开PAK包
					{
						m_nPakNumber++;
					}
					else
					{
						delete (m_PakFilePtrList[m_nPakNumber]);
						m_PakFilePtrList[m_nPakNumber]=NULL;
					}
				}
			}
			bResult = true;
		}
	}
	 IniFile.Clear();
	return m_nPakNumber;
}

//读取包内的子文件
int KPakList::ElemFileRead(XPackElemFileRef& ElemRef,void* pBuffer, unsigned uSize)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[ElemRef.nPackIndex]->XElemFileRead(ElemRef, pBuffer, uSize);

	//MessageBox(NULL,"包中找不到文件A","Information",MB_OK | MB_ICONEXCLAMATION);
	return 0;
}

//读取spr文件头部或整个spr
SPRHEAD* KPakList::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return (m_PakFilePtrList[ElemRef.nPackIndex]->GetSprHeader(ElemRef, pOffsetTable));
	return NULL;
}

//读取按帧压缩的spr的一帧的数据
SPRFRAME* KPakList::GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame,unsigned int &nSingFrameSize)
{
	if (nPackIndex >= 0 && nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[nPackIndex]->GetSprFrame(pSprHeader, nFrame,nSingFrameSize);
	return NULL;
}

bool KPakList::pGetFilePath(char* pszFileName, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
		char szPackName[256];
		ZeroMemory(szPackName,sizeof(szPackName));
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			szPackName[0] = '\\';
		#else
			szPackName[0] = '\\';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);//合并路径 去除.. 转换小写
		//char BackStr[256];                       //备份原来的路径
		//ZeroMemory(BackStr,sizeof(BackStr));
		//t_sprintf(BackStr,szPackName);
		unsigned long uId = 0;
		uId = g_CheckFileExist(U2G(szPackName).c_str());
		bFounded = pFindElemFileA(uId, ElemRef);
		if (!bFounded)
		{//如果再查找一次 还找不到 就放弃查找
			uId = g_CheckFileExist(G2U(szPackName).c_str());//FileNameToId(szPackName);
			bFounded = pFindElemFileA(uId, ElemRef);
			if (!bFounded)
			{//不转再查找一次 还找不到 就是没有的了 
				uId = g_CheckFileExist(szPackName);//FileNameToId(szPackName);
				bFounded = pFindElemFileA(uId, ElemRef);
			}
		}
		if (!bFounded)
		{
		}
	}
	return bFounded;
}
