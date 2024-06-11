//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KStrBase.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KStrBase_H
#define KStrBase_H
#include "cocos2d.h"

USING_NS_CC;
#include "KbugInfo.h"

//---------------------------------------------------------------------------
int		g_StrLen(LPCSTR lpStr);
LPSTR	g_StrEnd(LPCSTR lpStr);
void	g_StrCpy(LPSTR lpDest, LPCSTR lpSrc);
void	g_StrCpyLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen);
void	g_StrCat(LPSTR lpDest, LPCSTR lpSrc);
void	g_StrCatLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen);
BOOL	g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc);
BOOL	g_StrCmpLen(LPCSTR lpDest, LPCSTR lpSrc, int nMaxLen);
void	g_StrUpper(LPSTR lpDest);
void	g_StrLower(LPSTR lpDest);
void	g_StrRep(LPSTR lpDest, LPSTR lpSrc, LPSTR lpRep);
UINT    g_Atoui(LPSTR str);  //½«×Ö·û´®×¨³ÌUINT
void    g_StrCopy(std::string nSdcardPath, char *pMusicName);
int     g_ExtractChar(const char *inStr,char scrchar,char destchar,char *outStr=NULL,char *outStra=NULL,int nMoedel=0);
int     g_Round2Int(double d);
float   g_Int2Round(int d);
int     GetBig5Count(char *str);

int t_snprintf(char *buf, unsigned int count, const char *format, ... );
int t_sprintf(char *out, const char *format, ...);
int t_printf(const char *format, ...);
bool __CreateFolder(std::string nSdcardPath,char * nPath);
unsigned int _ccHash(const char *key);
//---------------------------------------------------------------------------
#endif
