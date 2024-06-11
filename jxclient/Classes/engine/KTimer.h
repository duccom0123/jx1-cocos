//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTimer.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KTimer_H
#define KTimer_H
//---------------------------------------------------------------------------
#include "cocos2d.h"

USING_NS_CC;

typedef union _LARGE_INTEGER_S {
	long long QuadPart;
} LARGE_INTEGERS;

#ifdef WIN32
class  KTimer
#else
class KTimer
#endif
{
private:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LARGE_INTEGERS	m_nFrequency;
	//LARGE_INTEGERS	m_nTimeStart;
	//LARGE_INTEGERS	m_nTimeStop;
	cc_timeval m_nTimeStart;
	cc_timeval m_nTimeStop;
#else
	timeval m_nTimeStart;
	timeval m_nTimeStop;
#endif
	int				m_nFPS;
public:
	KTimer();
	void			Start();
	void			Stop();
	long long		GetElapse();
	long long		GetElapseFrequency();
	long long		GetInterval();
	bool			Passed(unsigned long nTime);
	bool			GetFPS(int* nFPS);
};
#endif
