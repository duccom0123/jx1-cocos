//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCriticalSection.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-2 17:17:16
//  Comment     :   参考了刘鹏的的实现 
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KCRITICALSECTION_H_
#define _KCRITICALSECTION_H_    1

//#include "windows.h"
#include "cocos2d.h"
#include "KbugInfo.h"
USING_NS_CC;
class KCriticalSection
{

private:
	//CRITICAL_SECTION	m_CriticalSection;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	 pthread_mutex_t      m_CriticalSection;
#else
	 CRITICAL_SECTION     m_CriticalSection;		//操作包文件时的临界区控制
#endif

public:
    KCriticalSection()
    {
      #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        InitializeCriticalSection(&m_CriticalSection);
      #else
        pthread_mutex_init(&m_CriticalSection,NULL);
      #endif
    }

    ~KCriticalSection()
    {
       //DeleteCriticalSection(&m_CriticalSection);
     #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        DeleteCriticalSection(&m_CriticalSection);
     #else
        pthread_mutex_destroy(&m_CriticalSection);
     #endif
    }

    int Lock()
    {
      //  EnterCriticalSection(&m_CriticalSection);
       #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
          EnterCriticalSection(&m_CriticalSection);
       #else
          pthread_mutex_lock(&m_CriticalSection);
       #endif

       return true;
    }

    int UnLock()
    {
        //LeaveCriticalSection(&m_CriticalSection);
      #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        LeaveCriticalSection(&m_CriticalSection);
      #else
         pthread_mutex_unlock(&m_CriticalSection);
      #endif
      return false;
    }
};

class KAutoCriticalSection
{
    KCriticalSection &m_CriticalSection;

public:
    KAutoCriticalSection(KCriticalSection &CriticalSection)
        : m_CriticalSection(CriticalSection)
    {
        m_CriticalSection.Lock();
    }

    ~KAutoCriticalSection()
    {
        m_CriticalSection.UnLock();
    }
};

#endif

