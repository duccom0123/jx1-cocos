//#include "stdafx.h"
#include "Thread.h"
//
#ifdef WIN32
//#include "Macro.h"
#include <process.h>		//Thread define
CThread::CThread()
	: m_hThread( NULL )
{

}
CThread::~CThread()
{
	
#ifdef WIN32
	try{ if (m_hThread) {CloseHandle(m_hThread); (m_hThread) = NULL; } } catch(...) { }
#else
	SAFE_CLOSEHANDLE(m_hThread);
#endif
}

void * CThread::GetHandle() const
{
	return m_hThread;
}

void CThread::Start()
{
	if (m_hThread ==NULL)
	{
		unsigned int threadIDc = 0;
		//开始创建线程
		m_hThread = (void *)::_beginthreadex(0, 
			0, 
			ThreadFunction,
			(void *)this, 
			0, 
			&threadIDc);

		//第1个参数：安全属性，NULL为默认安全属性
		//第2个参数：指定线程堆栈的大小。如果为0，则线程堆栈大小和创建它的线程的相同。一般用0
		//第3个参数：指定线程函数的地址，也就是线程调用执行的函数地址(用函数名称即可，函数名称就表示地址)
		//第4个参数：传递给线程的参数的指针，可以通过传入对象的指针，在线程函数中再转化为对应类的指针
		//第5个参数：线程初始状态，0:立即运行；CREATE_SUSPEND：suspended（悬挂）
        //第6个参数：用于记录线程ID的地址
		char ninfo[64]={0};
		if (m_hThread == NULL)
		{
			//sprintf(ninfo,"_beginthreadex:filed");
			//CCMessageBox(ninfo,"_beginthreadex");
			return;
		}

		//CCMessageBox("_beginthreadex:suss","_beginthreadex");
	}
}

void CThread::Wait() const
{
	if (!Wait(INFINITE))
	{
//		throw CException(_T("CThread::Wait()"), _T("Unexpected timeout on infinite wait"));
	}
}

bool CThread::Wait(unsigned long timeoutMillis) const
{
	bool ok;

	if ( !m_hThread )
	{
		return true;
	}

	DWORD result = ::WaitForSingleObject( m_hThread, timeoutMillis );

	if ( result == WAIT_TIMEOUT )
	{
		ok = false;
	}
	else if ( result == WAIT_OBJECT_0 )
	{
		ok = true;
	}
	else
	{
		//throw CWin32Exception( _T( "CThread::Wait() - WaitForSingleObject" ), ::GetLastError() );
	}
    
	return ok;
}
//线程回调函数 __stdcall
unsigned int __stdcall  CThread::ThreadFunction( void *pV )
{
	int result = 0;

	CThread* pThis = ( CThread * )pV;
   
	if (pThis)
	{
		try
		{
			//result = 
			pThis->Run();
		}
		catch(...)
		{
		}
	}

	return result;
}
//终止线程
void CThread::Terminate(unsigned long exitCode /* = 0 */)
{
	if (m_hThread && !::TerminateThread(m_hThread,exitCode))
	{
		//TRACE( "CThread::Terminate error!" );
	}

	//SAFE_CLOSEHANDLE(m_hThread);
	try{ if (m_hThread) {CloseHandle(m_hThread); (m_hThread) = NULL; } } catch(...) { }
}
#else
//linux
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

CThread::CThread()
	:a_thread(0),
	autoDelete_(false)
{
}

CThread::~CThread()
{
	if (&thread_attr)
		pthread_attr_destroy(&thread_attr);
}

pthread_t CThread::GetHandle() const
{
	return a_thread;
}

void CThread::setAutoDelete(bool autoDelete)
{
	autoDelete_ = autoDelete;
}
//线程回调函数 __stdcall  
void * CThread::ThreadFunction(void *pV)
{//执行子线程
	CThread* pThis = (CThread *)pV;

	if (pThis)
	{

		try
		{
			//result =
			//pthread_detach(pthread_self()); //设置分离状态
			pThis->Run();
			//Terminate(NULL);
			if (pThis->autoDelete_)
			{
				delete pThis;
			}
		}
		catch(...)
		{
		}
	}

	//return;
}

void CThread::Start()
{
	/*int pthread_attr_getdetachstat(const pthread_attr_t *restrict attr,int *detachstate);
	  int pthread_attr_setdetachstat(const pthread_attr_t *attr,int detachstate);
       返回:成功返回0,出错返回错误编号
	   detachstat取值：
	   PTHREAD_CREATE_JOINABLE //(默认值)正常启动线程
	   PTHREAD_CREATE_DETACHED //以分离状态启动线程
	   */
		pthread_attr_init(&thread_attr);
		pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);//设置分离状态
		//pthread_create(&a_thread, &thread_attr, thread_function, NULL);
		//pthread_create(&a_thread, &thread_attr, thread_function, NULL);
		//开始创建主线程
		//int err;
		//err = pthread_create(&a_thread,NULL,ThreadFunction,this);
		pthread_create(&a_thread, &thread_attr,ThreadFunction,this);
		//pthread_create(&thid2,NULL,(void*)thread2,NULL);
		//pthread_attr_destroy(&thread_attr);
		//return NULL;
}


void CThread::Terminate( unsigned long exitCode)
{
	//CCMessageBox("exit the thread","exit the thread");
	if (exitCode)
	   pthread_attr_destroy(&thread_attr);
	//if (a_thread)
	  //pthread_cancel(a_thread);

	//pthread_exit(NULL); //设置一个退出的信号
	/*(1) 执行完成后隐式退出；

		(2) 由线程本身显示调用pthread_exit 函数退出；
		pthread_exit (void * retval) ; 

	(3) 被其他线程用pthread_cancel函数终止：
		pthread_cancel(pthread_t thread); */
	//pthread_exit((void *)0);
}


void CThread::Wait() const
{//等待线程结束
	if (a_thread > 0)
	{//等待退出的信号
		pthread_join(a_thread,NULL);
	}

}

void CThread::Wait(unsigned long timeoutMillis) const
{
	if (a_thread == 0)
	{
		return;
	}
	if (timeoutMillis == 0)
	{
		Wait();
	}else
	{
		unsigned long k = 0;
		while (/*threadStatus != THREAD_STATUS_EXIT && */k <= timeoutMillis)
		{
			usleep(100);
			k++;
		}
	}
}


#endif  //end linxu
