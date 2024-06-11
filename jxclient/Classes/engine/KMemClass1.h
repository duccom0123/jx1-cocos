#ifndef __KMEMCLASS1_H__
#define __KMEMCLASS1_H__

#include "KEngine.h"
#include "KbugInfo.h"

 class  KMemClass1 
{
	int m_lpMemLen;
	PVOID m_lpMemPtr;
public:
	PVOID		Alloc(DWORD dwSize);
	void		Free();
	void		Zero();
	void		Fill(BYTE byFill);
	void		Fill(WORD wFill);
	void		Fill(DWORD dwFill);
	PVOID		GetMemPtr() { return m_lpMemPtr; };
	DWORD		GetMemLen() { return m_lpMemLen; };
};

#endif //__KMEMCLASS1_H__
