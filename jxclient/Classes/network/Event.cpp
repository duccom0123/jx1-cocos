#include "Event.h"

CEvent::CEvent(bool bIsManualReset, bool bInitialSignaled)
	: m_bIsManualReset(bIsManualReset)
	, m_bEventStatus(bInitialSignaled)
	, m_bMutexInitialized(false)
	, m_bCondInitialized(false)
{

}

CEvent::~CEvent()
{
	if (m_bMutexInitialized)
	{
		pthread_mutex_destroy(&m_mutex); //互斥锁销毁函数在执行成功后返回 0，否则返回错误码。
		m_bMutexInitialized = false; 
	}

	if (m_bCondInitialized)
	{
		pthread_cond_destroy(&m_cond);
		/*
		pthread_cond_destroy是一个函数，被用来销毁一个条件变量。
		函数pthread_cond_destroy()被用来销毁一个条件变量。
		函数原型：int pthread_cond_destroy(pthread_cond_t *cond);
		函数作用：销毁cond指向的条件变量。
		形式参数：cond是指向pthread_cond_t结构的指针。
		需要注意的是只有在没有线程在该条件变量上等待时，才可以注销条件变量，
		否则会返回EBUSY。同时Linux在实现条件变量时，并没有为条件变量分配资源，
		所以在销毁一个条件变量时，只要注意该变量是否仍有等待线程即可。
		*/
		m_bCondInitialized = false;   
	}
}


bool CEvent::CreateEvent()
{
	if (!m_bMutexInitialized)
	{
		if (0 == pthread_mutex_init(&m_mutex, NULL))  //该函数用于C函数的多线程编程中，互斥锁的初始化。
		{
			m_bMutexInitialized = true; 
		}
	}

	if (!m_bCondInitialized)
	{
		if (0 == pthread_cond_init(&m_cond, NULL)) //初始化条件变量pthread_cond_init
		{//返回值：函数成功返回0；任何其他返回值都表示错误
			m_bCondInitialized = true;   
		}
	}

	return ( m_bMutexInitialized && m_bCondInitialized); 
}

bool CEvent::EnsureInitialized()
{
	return ( m_bMutexInitialized && m_bCondInitialized); 
}

bool CEvent::Set()
{
	if (!EnsureInitialized())
	{
		return false;
	}    

	pthread_mutex_lock(&m_mutex);
	m_bEventStatus = true;
	pthread_cond_broadcast(&m_cond); //唤醒条件变量
	pthread_mutex_unlock(&m_mutex);
	return true;    
}

bool CEvent::Reset()
{
	if (!EnsureInitialized())
	{
		return false;
	}

	pthread_mutex_lock(&m_mutex);
	m_bEventStatus = false;
	pthread_mutex_unlock(&m_mutex);
	return true;    
}

bool CEvent::Wait(int cms)
{
	if (!EnsureInitialized())
	{
		return false;
	}

	pthread_mutex_lock(&m_mutex);
	int error = 0;

	if (cms != INFINITE)
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		struct timespec ts;
		ts.tv_sec = tv.tv_sec + (cms / 1000);
		ts.tv_nsec = tv.tv_usec * 1000 + (cms % 1000) * 1000000; 

		if (ts.tv_nsec >= 1000000000)
		{
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000;
		}

		while ((!m_bEventStatus) && (error == 0))
		{
			error = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
		}
	}
	else
	{
		while ((!m_bEventStatus) && (error == 0))
		{
			error = pthread_cond_wait(&m_cond, &m_mutex);
		}
	}

	if (0 == error && !m_bIsManualReset)
	{
		m_bEventStatus = false;
	} 
	pthread_mutex_unlock(&m_mutex);
	return (0 == error);
}