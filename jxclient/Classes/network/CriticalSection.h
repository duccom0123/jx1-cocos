/********************************************************************
	created:	2003/02/13
	file base:	CriticalSection
	file ext:	h
	author:		liupeng
	
	purpose:	Header file for CRITICAL_SECTION class
*********************************************************************/
#ifndef __INCLUDE_CRITICALSECTION_H__
#define __INCLUDE_CRITICALSECTION_H__
#include "cocos2d.h"
USING_NS_CC;
#include <pthread.h>
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#define CRITICAL_SECTION pthread_mutex_t
#define BOOL int
#endif
#pragma once
//#include "Utils.h"
/*
 * CCriticalSection
 */
class CCriticalSection 
{
public:
	
	class Owner
	{
	public:
		
		explicit Owner(CCriticalSection &crit);
		
		~Owner();
		
	private:

/*#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
		pthread_mutex_t      m_crit;
#else
		CRITICAL_SECTION     m_crit;		//操作包文件时的临界区控制
#endif*/
		CCriticalSection &m_crit;
		
		/*
		 * No copies do not implement
		 */
		Owner(const Owner &rhs);
		Owner &operator=(const Owner &rhs);
	};
	
	CCriticalSection();
	
	~CCriticalSection();

	BOOL TryEnter();
	  
    void Enter();
	  
    void Leave();
	  
private:
	//CRITICAL_SECTION	m_crit;
	CRITICAL_SECTION     m_crit;		//操作包文件时的临界区控制
	/*
	 * No copies do not implement
	 */
	CCriticalSection(const CCriticalSection &rhs);
	CCriticalSection &operator=(const CCriticalSection &rhs);

};
/*
inline void CCriticalSection::Enter()
{
//	::EnterCriticalSection( &m_crit );
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::EnterCriticalSection(&m_crit);
#else
	pthread_mutex_lock(&m_crit);
#endif

}

inline void CCriticalSection::Leave()
{
//	::LeaveCriticalSection( &m_crit );
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::LeaveCriticalSection(&m_crit);
#else
	pthread_mutex_unlock(&m_crit);
#endif
}

inline CCriticalSection::CCriticalSection()
{
//	::InitializeCriticalSection( &m_crit );
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::InitializeCriticalSection(&m_crit);
#else
	pthread_mutex_init(&m_crit,NULL);
#endif
}
      
inline CCriticalSection::~CCriticalSection()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::DeleteCriticalSection( &m_crit );
#else
	 pthread_mutex_destroy(&m_crit);
#endif
	
}

inline BOOL CCriticalSection::TryEnter()
{
	//return bool(::TryEnterCriticalSection(&m_crit));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return BOOL(::TryEnterCriticalSection(&m_crit));
#else
	return BOOL(pthread_mutex_trylock(&m_crit));
#endif
	
}

inline CCriticalSection::Owner::Owner( CCriticalSection &crit )
		: m_crit( crit )
{
	m_crit.Enter();
}

inline CCriticalSection::Owner::~Owner()
{
	m_crit.Leave();
}
*/
#endif //__INCLUDE_CRITICALSECTION_H__
