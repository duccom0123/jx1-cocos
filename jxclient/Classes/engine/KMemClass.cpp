//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemClass.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Memory Opration Class Using Heap Memory Functions
//---------------------------------------------------------------------------
//#include "KWin32.h"
//#include "KDebug.h"
#include "KMemBase.h"
#include "KMemClass.h"
//---------------------------------------------------------------------------
// 函数:	KMemClass
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KMemClass::KMemClass()
{
	m_lpMemPtr = NULL;
	m_lpMemLen = 0;
}
//---------------------------------------------------------------------------
// 函数:	~KMemClass
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KMemClass::~KMemClass()
{
	Free();
}
//---------------------------------------------------------------------------
// 函数:	Alloc
// 功能:	分配内存
// 参数:	dwSize		内存块大小
// 返回:	PVOID		内存块指针
//---------------------------------------------------------------------------
PVOID KMemClass::Alloc(DWORD dwSize)
{
	// 已经分配的就不要再分配了
	if (m_lpMemLen == dwSize)
		return m_lpMemPtr;

	// 释放已经分配的内存
	if (m_lpMemPtr)
		Free();

	// 分配内存
	m_lpMemPtr = g_MemAlloc(dwSize);

	if (m_lpMemPtr != NULL)
		m_lpMemLen = dwSize;
	return m_lpMemPtr;

}
//---------------------------------------------------------------------------
// 函数:	Free
// 功能:	释放内存
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMemClass::Free()
{
	if (m_lpMemPtr)
		g_MemFree(m_lpMemPtr);
	m_lpMemPtr = NULL;
	m_lpMemLen = 0;
}
//---------------------------------------------------------------------------
// 函数:	Zero
// 功能:	内存清零
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMemClass::Zero()
{
	if (m_lpMemPtr)
		g_MemZero(m_lpMemPtr, m_lpMemLen);
}
//---------------------------------------------------------------------------
// 函数:	Fill
// 功能:	内存填充
// 参数:	byFill	用来填充的字节
// 返回:	void
//---------------------------------------------------------------------------
void KMemClass::Fill(BYTE byFill)
{
	if (m_lpMemPtr)
		g_MemFill(m_lpMemPtr, m_lpMemLen, byFill);
}
//---------------------------------------------------------------------------
// 函数:	Fill
// 功能:	内存填充
// 参数:	byFill	用来填充的字节
// 返回:	void
//---------------------------------------------------------------------------
void KMemClass::Fill(WORD wFill)
{
	if (m_lpMemPtr)
		g_MemFill(m_lpMemPtr, m_lpMemLen, wFill);
}
//---------------------------------------------------------------------------
// 函数:	Fill
// 功能:	内存填充
// 参数:	byFill	用来填充的字节
// 返回:	void
//---------------------------------------------------------------------------
void KMemClass::Fill(DWORD dwFill)
{
	if (m_lpMemPtr)
		g_MemFill(m_lpMemPtr, m_lpMemLen, dwFill);
}
//---------------------------------------------------------------------------
