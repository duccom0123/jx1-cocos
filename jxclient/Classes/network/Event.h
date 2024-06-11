/********************************************************************
	created:	2003/02/13
	file base:	Event
	file ext:	h
	author:		liupeng
	
	purpose:	Header file for CEvent class
*********************************************************************/
#ifndef __INCLUDE_EVENT_H__
#define __INCLUDE_EVENT_H__
#include "cocos2d.h"
//stdio.h 如果没有这个头文件 会出现一些错误 意外类型的xxx 意思就是没有这个类型的 声明
#include "engine/KbugInfo.h"
//#pragma once
#include <pthread.h>
//#include <sys/time.h>

#define INFINITE  -1

class CEvent
{
public:
	CEvent(bool bIsManualReset, bool bInitialSignaled);
	~CEvent();
	bool CreateEvent();
	bool Set();
	bool Reset();
	bool Wait(int cms);
private:
	bool EnsureInitialized();
	bool m_bIsManualReset;
	bool m_bEventStatus;
	bool m_bMutexInitialized;
	pthread_mutex_t m_mutex;
	bool m_bCondInitialized;
	pthread_cond_t m_cond;
};
#endif //__INCLUDE_EVENT_H__
/*
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
*/