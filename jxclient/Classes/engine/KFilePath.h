//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KFilePath.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KFilePath_H
#define KFilePath_H
#include "cocos2d.h"
//#include "cocos-ext.h"
#include "KbugInfo.h"
USING_NS_CC;
using namespace std;
//---------------------------------------------------------------------------
#ifndef MAXPATH
#define MAXPATH   260
#define MAXDIR    256
#define MAXFILE   256
#define MAXEXT    256
#define MAXDRIVE    3
#endif
//---------------------------------------------------------------------------
void	g_SetRootPath(char *lpPathName = NULL,bool isChange=false);
void	g_GetRootPath(char *lpPathName);
void	g_SetFilePath(char *lpPathName);
void	g_GetFilePath(char * lpPathName);
void	g_GetFullPath(char * lpPathName, char * lpFileName);
void	g_GetHalfPath(char * lpPathName, char * lpFileName);
void	g_GetPackPath(char * lpPathName, char * lpFileName);
void	g_GetDiskPath(char * lpPathName, char * lpFileName);
void	g_CreatePath(char * lpPathName);
// 一个路径和一个文件名，合并到lpGet中形成一个完整的路径文件名
void	g_UnitePathAndName(char *lpPath, char *lpFile, char *lpGet);

//---------------------------------------------------------------------------
BOOL	g_FileExists(char * lpPathName);
unsigned long	g_FileName2Id(char * lpFileName);//DWORD
void	g_ChangeFileExt(char * lpFileName, char * lpNewExt);
void	g_ExtractFileName(char * lpFileName, char * lpFilePath);
void	g_ExtractFilePath(char * lpPathName, char * lpFilePath);
unsigned long g_CheckFileExist(const char * lpFileName);
//---------------------------------------------------------------------------
#endif
