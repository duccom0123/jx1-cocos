//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTimer.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Timer Class
//---------------------------------------------------------------------------

//#include <Winbase.h>
//#include "KMemBase.h"
#include "KTimer.h"
#include <time.h>
// flying add this macro.
// must pay more attention in the platform porting job.
#ifdef __linux
#include <sys/time.h>
#endif


//#include <time.h>
// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME (116444736000000000UL)

KTimer::KTimer()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//m_nFrequency.QuadPart = 200 * 1024 * 1024;   //时钟的频率
	//m_nTimeStart.QuadPart = 0;
	//m_nTimeStop.QuadPart  = 0;
	m_nFPS = 0;
	//SetThreadAffinityMask(GetCurrentThread(),0x00000001);
	//QueryPerformanceFrequency(&m_nFrequency);    //对于多核出现错误  返回硬件支持的高精度计数器的频率。获取 cpu频率
#else
    //m_nFrequency = CLOCKS_PER_SEC; //以下原来没有的
#endif
}
//---------------------------------------------------------------------------
// 函数:	Start
// 功能:	开始计时
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTimer::Start()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//	 CCTimer::
    //FILETIME ft;
    //LARGE_INTEGER li;
    //int64_t tt = 0;
    /*::GetSystemTimeAsFileTime(&ft);
    m_nTimeStart.LowPart  = ft.dwLowDateTime;
    m_nTimeStart.HighPart = ft.dwHighDateTime;
    // 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
    m_nTimeStart.QuadPart = (m_nTimeStart.QuadPart - EPOCHFILETIME)/10UL;
    //return tt;
	//DWORD_PTR oldmask =::SetThreadAffinityMask(::GetCurrentThread(),0);
	//QueryPerformanceCounter(&m_nTimeStart);
	//::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
    //time_t rawtime;      //定义一个long 型存放秒数
	//struct tm * timeinfo;	
	//m_nTimeStart.QuadPart = time(NULL);     //距离现在的时间（秒）
    //m_nTimeStart.QuadPart = m_nTimeStart.sQuadPart*1000; */
	//cc_timeval *m_pStartUpdate = NULL;
	CCTime::gettimeofdayCocos2d(&m_nTimeStart,NULL);
   // if (m_pStartUpdate)
	 //   m_nTimeStart.QuadPart = m_pStartUpdate->tv_usec/1000;
	//m_nTimeStart.QuadPart=//GetTickCount();// 精确是毫秒

#else
    gettimeofday(&m_nTimeStart, NULL); //原来
//	QueryPerformanceCounter(&m_nTimeStart);
#endif
}
//---------------------------------------------------------------------------
// 函数:	Stop
// 功能:	停止计时
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------

void KTimer::Stop()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    //FILETIME ft;
    //LARGE_INTEGER li;
    //int64_t tt = 0;
    /*::GetSystemTimeAsFileTime(&ft);
    m_nTimeStop.LowPart  = ft.dwLowDateTime;
    m_nTimeStop.HighPart = ft.dwHighDateTime;
    // 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
    m_nTimeStop.QuadPart = (m_nTimeStop.QuadPart - EPOCHFILETIME)/10UL;	//毫秒
	//DWORD_PTR oldmask =::SetThreadAffinityMask(::GetCurrentThread(),0);
	//QueryPerformanceCounter(&m_nTimeStop);
	//::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
    //m_nTimeStop.QuadPart = time(NULL); 
	*/

    //m_nTimeStop.QuadPart=GetTickCount();// 精确是毫秒
	//cc_timeval *m_pStopUpdate = NULL;
	CCTime::gettimeofdayCocos2d(&m_nTimeStop,NULL);
	//if (m_pStopUpdate)
	  // m_nTimeStop.QuadPart = m_pStopUpdate->tv_usec/1000;

#else
	gettimeofday(&m_nTimeStop, NULL); //原来
//	QueryPerformanceCounter(&m_nTimeStop);
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetElapse
// 功能:	计算从开始计时到现在已经过到时间
// 参数:	void
// 返回:	DWORD in ms		 GetTickCount() 精确是毫秒
//---------------------------------------------------------------------------
long long KTimer::GetElapse()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//LARGE_INTEGERS nTime;
	/*
    FILETIME ft;
    //int64_t tt = 0;
    ::GetSystemTimeAsFileTime(&ft);	   //获取现在的时间
    nTime.LowPart  = ft.dwLowDateTime;
    nTime.HighPart = ft.dwHighDateTime;
    // 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
    nTime.QuadPart = (nTime.QuadPart - EPOCHFILETIME)/10UL;	//毫秒
   	*/
	cc_timeval  m_pEndtime;
	CCTime::gettimeofdayCocos2d(&m_pEndtime,NULL);
	//if (m_pStopUpdate)
		//m_nTimeStop.QuadPart = m_pStopUpdate->tv_usec/1000;
	return (m_pEndtime.tv_sec - m_nTimeStart.tv_sec) * 1000 + m_pEndtime.tv_usec/1000;  //毫秒
	    //nTime.QuadPart= m_pEndtime->tv_usec/1000;//GetTickCount();// 精确是毫秒
	//return (DWORD)((nTime.QuadPart - m_nTimeStart.QuadPart)/1000UL);  //返回
	//return  (nTime.QuadPart - m_nTimeStart.QuadPart); 
	//LARGE_INTEGER nTime;
	//DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
	//QueryPerformanceCounter(&nTime);
	//::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
	//return (DWORD)((nTime.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart);

	/* nTime.QuadPart = time(NULL);
    DWORD nValTime;
    nValTime =(DWORD)((nTime.QuadPart - m_nTimeStart.QuadPart)*1000);
	return nValTime;*/  //毫秒数
#else
	timeval tv;
	gettimeofday(&tv, NULL); //原来
    return (tv.tv_sec - m_nTimeStart.tv_sec) * 1000 + tv.tv_usec / 1000;  //毫秒
#endif
}
//---------------------------------------------------------------------------
// 函数:	GetElapseFrequency
// 功能:	计算从开始计时到现在已经过到时间
// 参数:	void
// 返回:	DWORD in frequency
//---------------------------------------------------------------------------
long long KTimer::GetElapseFrequency()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	LARGE_INTEGERS nTime;
   /* FILETIME ft;
    LARGE_INTEGER nTime;
    //int64_t tt = 0;
    ::GetSystemTimeAsFileTime(&ft);
    nTime.LowPart  = ft.dwLowDateTime;
    nTime.HighPart = ft.dwHighDateTime;
	nTime.QuadPart = (nTime.QuadPart - EPOCHFILETIME)/10UL;
	//DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
	//QueryPerformanceCounter(&nTime);
    //::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
	//nTime.QuadPart = time(NULL);
	return (DWORD)((nTime.QuadPart - m_nTimeStart.QuadPart)/1000UL); */
	//nTime.QuadPart=GetTickCount();// 精确是毫秒
	return 0;//(nTime.QuadPart - m_nTimeStart.QuadPart);

#endif
	return 0;
}
//---------------------------------------------------------------------------
// 函数:	GetInterval
// 功能:	取得从开始到停止之间的时间间隔，以毫秒为单位
// 参数:	void
// 返回:	毫秒值
//---------------------------------------------------------------------------
long long KTimer::GetInterval()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//return (DWORD)((m_nTimeStop.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart); //毫秒为单位
    return 0;//(m_nTimeStop.QuadPart - m_nTimeStart.QuadPart);
#endif
  // return (DWORD)((m_nTimeStop.QuadPart - m_nTimeStart.QuadPart) * 1000 / m_nFrequency.QuadPart);
	return 0; //原来
}
//---------------------------------------------------------------------------
// 函数:	Passed
// 功能:	看是否过了nTime毫秒
// 参数:	nTime	毫秒
// 返回:	TRUE	已经过了
//			FALSE	还没有过
//---------------------------------------------------------------------------
bool KTimer::Passed(unsigned long nTime)
{

	if (GetElapse() >= (unsigned long)nTime)
	{
		Start();    //重新设定开始时间
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
// 函数:	GetFPS
// 功能:	取得帧速率
// 参数:	nFPS	帧速率
// 返回:	TRUE	成功
//			FALSE	失败
//---------------------------------------------------------------------------
bool KTimer::GetFPS(int *nFPS)
{
	if (GetElapse() >= 1000UL) 
	{
		*nFPS = m_nFPS;
		m_nFPS = 0;
		Start();     //重新设置开始时间
		return true;
	}
	m_nFPS++;
	return false;
}
