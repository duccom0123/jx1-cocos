//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemBase.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMemBase_H
#define KMemBase_H
#include "cocos2d.h"

USING_NS_CC;
#include "KbugInfo.h"
//---------------------------------------------------------------------------
void	g_MemInfo(void);
void*	g_MemAlloc(unsigned long dwSize);
void	g_MemFree(void* lpMem);
void	g_MemFreeSign(unsigned long nSign);


void	g_MemFreeSec(void * lpMem);
void	g_MemCopy(void * lpDest, void * lpSrc, unsigned long dwLen);
void	g_MemCopyMmx(void * lpDest, void * lpSrc, unsigned long dwLen);
bool	g_MemComp(void * lpDest, void * lpSrc, unsigned long dwLen);
void	g_MemFill(void * lpDest, unsigned long dwLen, unsigned char byFill);
void	g_MemFill(void * lpDest, unsigned long dwLen, unsigned short  wFill);
void	g_MemFill(void * lpDest, unsigned long dwLen, unsigned long dwFill);
void	g_MemZero(void * lpDest, unsigned long dwLen);
void	g_MemXore(void * lpDest, unsigned long dwLen, unsigned long dwXor);
//---------------------------------------------------------------------------
#endif
