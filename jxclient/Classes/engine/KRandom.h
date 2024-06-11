//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KRandom.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KRandom_H
#define KRandom_H
//---------------------------------------------------------------------------
//ENGINE_API unsigned __int64 g_Random(UINT nMax);
//ENGINE_API  unsigned __int64 g_GetRandomSeed();
void g_RandomSeed(unsigned int nSeed);
unsigned int g_GetRandomSeed();
unsigned int g_Random(unsigned int nMax);

//---------------------------------------------------------------------------
#endif
