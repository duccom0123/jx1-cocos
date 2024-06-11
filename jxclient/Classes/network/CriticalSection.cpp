#include "CriticalSection.h"


CCriticalSection::CCriticalSection()
{
	//	::InitializeCriticalSection( &m_crit );
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::InitializeCriticalSection(&m_crit);
#else
	pthread_mutex_init(&m_crit,NULL);
#endif
}

CCriticalSection::~CCriticalSection()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::DeleteCriticalSection( &m_crit );
#else
	pthread_mutex_destroy(&m_crit);
#endif

}

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


inline BOOL CCriticalSection::TryEnter()
{
	//return bool(::TryEnterCriticalSection(&m_crit));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return BOOL(::TryEnterCriticalSection(&m_crit));
#else
	return BOOL(pthread_mutex_trylock(&m_crit));
#endif
	
}


CCriticalSection::Owner::Owner(CCriticalSection &crit)
		: m_crit(crit)
{
	m_crit.Enter();
}

CCriticalSection::Owner::~Owner()
{
	m_crit.Leave();
}