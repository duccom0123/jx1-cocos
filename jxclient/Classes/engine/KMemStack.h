//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMemStack.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef _KMem_Stack_H
#define _KMem_Stack_H

#include "cocos2d.h"
USING_NS_CC;
#include "KbugInfo.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class KMemStack
{
private:
	PBYTE		m_pStack[MAX_CHUNK];
	int			m_nStackTop;
	int 		m_nStackEnd;
	int			m_nChunkTop;
	int			m_nChunkSize;
public:
	KMemStack();
	~KMemStack();
	void		Init(int nChunkSize = 65536);
	PVOID		Push(int nSize);
	void		Free(LPVOID pMem);
	BOOL		AllocNewChunk();
	void		FreeAllChunks();
	int			GetChunkSize();
	int			GetStackSize();
};
//---------------------------------------------------------------------------
#endif
