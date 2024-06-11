//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodecLzo.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KCodecLzo_H
#define KCodecLzo_H
#include "cocos2d.h"

USING_NS_CC;
//---------------------------------------------------------------------------
#include "KbugInfo.h"
#include "KMemClass.h"
#include "KCodec.h"
//---------------------------------------------------------------------------
class  KCodecLzo : public KCodec
{
private:
	KMemClass	m_WorkMem;
	DWORD Compress(PBYTE pIn, DWORD dwInLen, PBYTE pOut, PDWORD pOutLen);
public:
	bool  Encode(TCodeInfo* pCodeInfo);
	bool  Decode(TCodeInfo* pCodeInfo);
};
//---------------------------------------------------------------------------
#endif
