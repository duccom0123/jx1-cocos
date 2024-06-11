//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPath.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	File Path Utility
//---------------------------------------------------------------------------
//#include "KWin32.h"
//#include "KDebug.h"
//#include "tchar.h"
#include "gamecore/KCore.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include "KFilePath.h"
#include "KPakList.h"
#include "dataChecksum.h"
#ifndef WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <string.h>
//---------------------------------------------------------------------------
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
static char szRootPath[MAXPATH] = "D:\\Source\\jxmbcc\\cocos2dx226\\projects\\jxclient\\data";
//D:\\cocos2dx226\\projects\\jxclient\\Resources";		// ����·��
static char szCurrPath[MAXPATH] = "\\";		// ��ǰ·��
#define CURPOS 1
#else
//#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
static char szRootPath[MAXPATH]="/Android/data/com.jxclient.replease";
static char szCurrPath[MAXPATH] = "/";		// ��ǰ·��
#define CURPOS 1
#endif

int RemoveTwoPointPath(LPTSTR szPath, int nLength)
{
	int nRemove = 0;
	//KASSERT(szPath);
	if (!szPath) return 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LPTSTR lpszOld ="\\..\\";
#else
	LPTSTR lpszOld ="/../";
#endif
	LPTSTR lpszTarget = strstr(szPath, lpszOld);
	if (lpszTarget)
	{
		LPTSTR lpszAfter = lpszTarget + 3;
		while(lpszTarget > szPath)
		{
			lpszTarget--;
			if ((*lpszTarget) == '\\' ||(*lpszTarget) == '/')
				break;
		}
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (lpszAfter - lpszTarget);
		return RemoveTwoPointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveOnePointPath(LPTSTR szPath, int nLength)
{
	int nRemove = 0;
	//KASSERT(szPath);
	if (!szPath) return 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LPTSTR lpszOld = "\\.\\";
#else
	LPTSTR lpszOld = "/./"; //LPCTSTR
#endif
	LPTSTR lpszTarget = strstr(szPath, lpszOld);
	if (lpszTarget)
	{
		LPTSTR lpszAfter = lpszTarget + 2;
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (lpszAfter - lpszTarget);
		return RemoveOnePointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveAllPointPath(LPTSTR szPath, int nLength)
{
	return RemoveOnePointPath(szPath, RemoveTwoPointPath(szPath, nLength));
}

//---------------------------------------------------------------------------
// ����:	SetRootPath
// ����:	���ó���ĸ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_SetRootPath(LPSTR lpPathName,bool isChange)
{
	if (lpPathName)
	{
		g_StrCpy(szRootPath, lpPathName);
	}
	else
	{
//#ifdef WIN32
//		GetCurrentDirectory(MAXPATH, szRootPath);
//#else
		//getcwd(szRootPath, MAXPATH);
//#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (isChange)
		  _getcwd(szRootPath, MAXPATH);	//��ȡ�����е�·��,�������˸�Ŀ¼·��
		else
		{//����Ϊ�յ� �Ż�ȡ����·��
			if (!szRootPath[0])
				_getcwd(szRootPath, MAXPATH);
		}
#else
		//std::string nPath =  CCFileUtils::sharedFileUtils()->getWritablePath();
		if  (m_SDcardDirPath!="")
		    t_sprintf(szRootPath,"%s",m_SDcardDirPath.c_str());
#endif
	}

	// ȥ��·��ĩβ�� '\'
	int len = g_StrLen(szRootPath);
	if (szRootPath[len - 1] == '\\' || szRootPath[len - 1] == '/')
	{
		szRootPath[len - 1] = 0;
	}
}
//---------------------------------------------------------------------------
// ����:	GetRootPath
// ����:	ȡ�ó���ĸ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_GetRootPath(LPSTR lpPathName)
{
	g_StrCpy(lpPathName, szRootPath);
}
//---------------------------------------------------------------------------
// ����:	SetFilePath
// ����:	���õ�ǰ�ļ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_SetFilePath(LPSTR lpPathName)
{
	// ȥ��ǰ��� "\\"
	if (lpPathName[0] == '\\' ||lpPathName[0] == '/')
	{
		g_StrCpy(szCurrPath, lpPathName + 1);
	}
	else
	{
		g_StrCpy(szCurrPath, lpPathName);
	}

	// ĩβ���� "\\"
	int len = g_StrLen(szCurrPath);
	if (len > 0 && szCurrPath[len - 1] != '\\' && szCurrPath[len - 1] != '/')
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		szCurrPath[len] = '\\';
#else
		szCurrPath[len] = '/';
#endif
		szCurrPath[len + 1] = 0;
	}
	RemoveAllPointPath(szCurrPath, len + 1);
#ifndef WIN32
	//'\\' -> '/' [wxb 2003-7-29]
	for (len = 0; szCurrPath[len]; ++len)
	{
		if (szCurrPath[len] == '\\')
			szCurrPath[len] = '/';
	}
#endif
}
//---------------------------------------------------------------------------
// ����:	GetFilePath
// ����:	ȡ�õ�ǰ�ļ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_GetFilePath(LPSTR lpPathName)
{
	g_StrCpy(lpPathName, szCurrPath);
}
//---------------------------------------------------------------------------
// ����:	GetFullPath
// ����:	ȡ���ļ���ȫ·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_GetFullPath(LPSTR lpPathName, LPSTR lpFileName)
{
	// �ļ�����ȫ·��
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	//std::string nPath =  CCFileUtils::sharedFileUtils()->getWritablePath();
	if  (m_SDcardDirPath!="")
	    t_sprintf(szRootPath,"%s",m_SDcardDirPath.c_str());
	//CCMessageBox(szRootPath,"szRootPath-a");
#endif
	//CCMessageBox(szRootPath,"szRootPath-b");
	// ȥ��·��ĩβ�� '\'
	int len = g_StrLen(szRootPath);
	if (szRootPath[len - 1] == '\\' || szRootPath[len - 1] == '/')
	{
		szRootPath[len - 1] = 0;
	}
	if (lpFileName[CURPOS] == ':')
	{
		g_StrCpy(lpPathName, lpFileName);
		return;
	}

	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{
		g_StrCpy(lpPathName, szRootPath);
		g_StrCat(lpPathName, lpFileName);
		return;
	}
	
	// ��ǰ·��Ϊȫ·��
	if (szCurrPath[CURPOS] == ':')
	{
		g_StrCpy(lpPathName, szCurrPath); //��һ��б��
		g_StrCat(lpPathName, lpFileName);
		return;
	}
	// ��ǰ·��Ϊ����·��
	g_StrCpy(lpPathName, szRootPath);
   if(szCurrPath[0] != '\\' && szCurrPath[0] != '/')
   {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	g_StrCat(lpPathName, "\\");
#else
	g_StrCat(lpPathName, "/");
#endif
      }
	g_StrCat(lpPathName, szCurrPath); //��һ��б��

	if (lpFileName[0] == '.' && (lpFileName[1] == '\\'||lpFileName[1] == '/') )
		g_StrCat(lpPathName, lpFileName + 2);
	else
		g_StrCat(lpPathName, lpFileName);
}
//---------------------------------------------------------------------------
// ����:	GetHalfPath
// ����:	ȡ���ļ��İ�·������������·��
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_GetHalfPath(LPSTR lpPathName, LPSTR lpFileName)
{
	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{
		g_StrCpy(lpPathName, lpFileName);
	}
	else
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		g_StrCpy(lpPathName, "\\");
#else
		g_StrCpy(lpPathName, "/");
#endif
		g_StrCat(lpPathName, szCurrPath);
		g_StrCat(lpPathName, lpFileName);
	}
}
//---------------------------------------------------------------------------
// ����:	GetPackPath
// ����:	ȡ���ļ���ѹ�����е�·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_GetPackPath(LPSTR lpPathName, LPSTR lpFileName)
{
	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{//ȥ��һ��б��
		g_StrCpy(lpPathName, lpFileName + 1);
	}
	else
	{
		//g_StrCpy(lpPathName, szCurrPath);
		g_StrCat(lpPathName,lpFileName);
	}

	int len = g_StrLen(lpPathName);
	RemoveAllPointPath(lpPathName,len + 1);
	// ȫ��ת��ΪСд��ĸ
	g_StrLower(lpPathName);

	/*#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	   int nleng = g_StrLen(lpPathName);
	   for(int  i =0;i<nleng;i++)
	   {
		   if (lpPathName[i]=='/')
			   lpPathName[i] = '\\';
	   }
    #endif*/
	//strlwr(lpPathName);
}
//---------------------------------------------------------------------------
// ����:	GetDiskPath
// ����:	ȡ��CDROM��Ӧ���ļ�·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_GetDiskPath(LPSTR lpPathName, LPSTR lpFileName)
{
	g_StrCpy(lpPathName, "C:");
	for (int i = 0; i < 24; lpPathName[0]++, ++i)
	{
//		if (GetDriveType(lpPathName) == DRIVE_CDROM)
//			break;
	}
	if (lpFileName[0] == '\\' || lpPathName[0] == '/')
	{
		g_StrCat(lpPathName, lpFileName);
	}
	else
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		g_StrCat(lpPathName, "\\");
#else
		g_StrCat(lpPathName, "/");
#endif
		g_StrCat(lpPathName, szCurrPath);
		g_StrCat(lpPathName, lpFileName);
	}
}

//---------------------------------------------------------------------------
// ����:	CreatePath
// ����:	����Ϸ��Ŀ¼�½���һ��·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void	g_CreatePath(LPSTR lpPathName)
{
	if (!lpPathName || !lpPathName[0])
		return;

	char	szFullPath[MAXPATH];
	int		i;
	// �ļ�����ȫ·��
	if (lpPathName[CURPOS] == ':')
	{
		if (g_StrLen(lpPathName) < 4)
			return;
		g_StrCpy(szFullPath, lpPathName);
		i = 4;
	}
	else if (lpPathName[0] == '\\' || lpPathName[0] == '/')
	{
		g_StrCpy(szFullPath, szRootPath);
		g_StrCat(szFullPath, lpPathName);
		i = g_StrLen(szRootPath) + 1;
	}
	else
	{
		g_StrCpy(szFullPath, szRootPath);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		g_StrCat(szFullPath, "\\");
#else
		g_StrCat(szFullPath, "/");
#endif
		g_StrCat(szFullPath, lpPathName);
                
		i = g_StrLen(szRootPath) + 1;
	}

	for (; i < g_StrLen(szFullPath); ++i)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (szFullPath[i] == '\\') {
			szFullPath[i] = 0;
			CreateDirectoryA(szFullPath, NULL);
			szFullPath[i] = '\\';
		}
#else
		if (szFullPath[i] == '/') {
			szFullPath[i] = 0;
			szFullPath[i] = '/';
		}
#endif
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CreateDirectoryA(szFullPath, NULL);
#else
	// flying comment
	// �촫��ʵ���������
	//mkdir();
#endif
}

//---------------------------------------------------------------------------
// ����:	g_UnitePathAndName
// ����:	һ��·����һ���ļ������ϲ���lpGet���γ�һ��������·���ļ���
// ����:	lpPath ����·���� lpFile �����ļ��� lpGet ��õ����������ļ���
// ����:	void
// ע�⣺   ����û�п����ַ����ĳ��ȣ�ʹ�õ�ʱ��Ҫ��֤�ַ����ĳ����㹻
//---------------------------------------------------------------------------
void	g_UnitePathAndName(char *lpPath, char *lpFile, char *lpGet)
{
	if (!lpPath || !lpFile || !lpGet)
		return;
	/*std::string strlp =  lpPath;
	std::string strlf =  lpFile;
	strlp +="\\";
	strlp +=lpFile;
	t_sprintf(lpGet,strlp.c_str());
	*/
	strcpy(lpGet, lpPath);
	int	nSize = strlen(lpGet);
	if (lpGet[nSize] - 1 != '\\')
	{
		lpGet[nSize] = '\\';
		lpGet[nSize + 1] = 0;
	}
	if (lpFile[0] != '\\')
	{
		strcat(lpGet, lpFile);
	}
	else
	{
		strcat(lpGet, &lpFile[1]);
	}
}


//---------------------------------------------------------------------------
// ����:	find if file exists in pak or in hard disk
// ����:	����ָ�����ļ��Ƿ����
// ����:	lpPathName	·�������ļ���
// ����:	TRUE���ɹ���FALSE��ʧ�ܡ�
//---------------------------------------------------------------------------
BOOL g_FileExists(LPSTR FileName)
{
	BOOL	bExist = FALSE;
	char	szFullName[MAX_PATH];
	if (FileName && FileName[0])
	{
		//�Ȳ����Ƿ��ڴ���ļ���
		if (g_pPakList)
		{
			XPackElemFileRef	PackRef;
			bExist = g_pPakList->pGetFilePath(FileName, PackRef);
		}
		//�ڼ���Ƿ񵥶������ļ�ϵͳ��
		if (bExist == FALSE)
		{
#ifdef WIN32
			 g_GetFullPath(szFullName, FileName);
			 bExist = !(GetFileAttributesA(szFullName) & FILE_ATTRIBUTE_DIRECTORY);// || dword == INVALID_FILE_ATTRIBUTES)
#else
			 std::string nCurFilePath = FileName;
			 bExist = CCFileUtils::sharedFileUtils()->isFileExist(nCurFilePath);//��Դ��
#endif
		}
	}

	return bExist;
}
//---------------------------------------------------------------------------
// ����:	File Name to 32bit Id
// ����:	�ļ���ת���� Hash 32bit ID
// ����:	lpFileName	�ļ���
// ����:	FileName Hash 32bit ID
// 
// ע��:	��Ϸ����������ؽ����������õĹ�ϣ��������Ҳ����
//			������������������޸��������ʱҲ��Ӧ�޸�������
//			�����Ӧ���Ǹ��������������������Common.lib���̵�Utils.h
//			�У���������Ϊ DWORD HashStr2ID( const char * const pStr );
//---------------------------------------------------------------------------
unsigned long g_FileName2Id(char * lpFileName)
{
////////////////////////////////////////////////////////////////
	if (lpFileName)
	{
		int len = strlen(lpFileName);
		unsigned long dwID = 0;
		for (int i = 0;i<len; ++i) //lpFileName[i]
		{
			dwID =(dwID + (i + 1) * lpFileName[i])%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
		}
		return (dwID^0x12345678);	    //0x12345678  305419896
	}
	return (unsigned long)(-1);
}
//---------------------------------------------------------------------------
// ����:	change file extention
// ����:	�ı��ļ�����չ��
// ����:	lpFileName	�ļ���
//			lpNewExt	����չ����������'.'
// ����:	void
//---------------------------------------------------------------------------
void g_ChangeFileExt(LPSTR lpFileName, LPSTR lpNewExt)
{
	int  i;

	for (i = 0; lpFileName[i]; ++i)
	{
		if (lpFileName[i] == '.')
			break;
	}
	
	if (lpFileName[i] == '.')
	{
		g_StrCpy(&lpFileName[i + 1], lpNewExt);
	}
	else
	{
		g_StrCat(lpFileName, ".");
		g_StrCat(lpFileName, lpNewExt);
	}
}
//---------------------------------------------------------------------------
// ����:	Extract File Name from path name
// ����:	ȡ���ļ�����������·����
// ����:	lpFileName	�ļ�����������·����
//			lpFilePath	�ļ���������·����
// ����:	void
//---------------------------------------------------------------------------
void g_ExtractFileName(LPSTR lpFileName, LPSTR lpFilePath)
{
	int nLen = g_StrLen(lpFilePath);
	if (nLen < 5)
		return;
	int nPos = nLen;
	while (--nPos > 0)
	{
		if (lpFilePath[nPos] == '\\'||lpFilePath[nPos] == '/')
			break;
	}
	g_StrCpy(lpFileName, &lpFilePath[nPos + 1]);
}
//---------------------------------------------------------------------------
// ����:	Extract File Path from path name
// ����:	ȡ��·����
// ����:	lpFileName	·����
//			lpFilePath	·�������ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_ExtractFilePath(LPSTR lpPathName, LPSTR lpFilePath)
{
	int nLen = g_StrLen(lpFilePath);
	if (nLen < 5)
		return;
	int nPos = nLen;
	while (--nPos > 0)
	{
		if (lpFilePath[nPos] == '\\' ||lpFilePath[nPos] == '/')
			break;
	}
	g_StrCpyLen(lpPathName, lpFilePath, nPos);
}

unsigned long g_CheckFileExist(const char * lpFileName)
{
	if  (!lpFileName) return (unsigned long)(-1);

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
		return (unsigned long)(-1);
#endif
	/*char lpFileName[256];
	ZeroMemory(lpFileName,sizeof(lpFileName));
	memcpy(&lpFileName[0],__lpFileName,sizeof(__lpFileName));
	*/
	if (lpFileName)
	{
		int len = strlen(lpFileName);
		unsigned long dwID = 0;
		for (int i = 0;i<len; ++i) //lpFileName[i]
		{
			/*if (lpFileName[i] >= 0xE0) //���� 3���ֽ�
			{
				len = 3;
				CCMessageBox("china","china");
			}
			else if (lpFileName[i]>= 0xC0)
				len = 2;
			else //Ӣ������
			{
				len = 1;
				CCMessageBox("engish","engish");
			}*/
#ifdef WIN32
			dwID =(dwID + (i + 1)*lpFileName[i])%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
#else       //ÿ���ַ��ͱ������� ��android ��
			if(lpFileName[i]&0x80)
			{//������
				dwID =(dwID + (i + 1)*(lpFileName[i]+0xFFFFFF00))%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
				dwID =(dwID + (i + 2)*(lpFileName[i+1]+0xFFFFFF00))%0x8000000B*0xFFFFFFEF;
				i++;
			}
			else
				dwID =(dwID + (i + 1)*lpFileName[i])%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
#endif
		}
		return (dwID^305419896);	    //0x12345678  305419896
	}
	return (unsigned long)(-1);
}
//---------------------------------------------------------------------------
