//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemClass.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMemClass_H
#define KMemClass_H
#include "cocos2d.h"

USING_NS_CC;
#include "KbugInfo.h"
//---------------------------------------------------------------------------
class KMemClass
{
private:
	PVOID		m_lpMemPtr;
	DWORD		m_lpMemLen;
public:
	KMemClass();
	~KMemClass();
	PVOID		Alloc(DWORD dwSize);
	void		Free();
	void		Zero();
	void		Fill(BYTE byFill);
	void		Fill(WORD wFill);
	void		Fill(DWORD dwFill);
	PVOID		GetMemPtr() { return m_lpMemPtr; };
	DWORD		GetMemLen() { return m_lpMemLen; };
};
//---------------------------------------------------------------------------
#endif
