/********************************************************************
	created:	2003/02/18
	file base:	SocketClient
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_SOCKETCLIENT_H__
#define __INCLUDE_SOCKETCLIENT_H__
#pragma once
#include "cocos2d.h"
#ifdef WIN32 
#include <winsock2.h>
#endif
//#include "UsesWinsock.h"
#include "Thread.h"
#include "CriticalSection.h"
#include "IOBuffer.h"
#include "NodeList.h"
#include <string>
#include "Cipher.h"

#ifdef WIN32
#pragma comment(lib, "wsock32")
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/select.h>
#endif


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
class CSocketClient:protected CThread,protected CIOBuffer::Allocator //private CUsesWinsock, 
#else
class CSocketClient:protected CThread,protected CIOBuffer::Allocator
#endif
{
public:
	virtual ~CSocketClient();
	using CThread::Start;
#ifdef WIN32
//	HANDLE  CThread::GetHandle;
	WSADATA m_data;
#endif
	void Connect(
		const std::string &addressToConnectServer,
		unsigned short portToConnectServer
		);
	bool StartConnections();
	void StopConnections();
	void InitiateShutdown();
	void WaitForShutdownToComplete(int isCleartheThread=false);
	void Write( const char *pData, size_t dataLength );
	bool Write( CIOBuffer *pBuffer );
protected:
	CSocketClient(
		const std::string &addressToConnectServer,
		unsigned short portToConnectServer,
		size_t maxFreeBuffers,
		size_t bufferSize, /* = 1024 */
		int  mSocketNo
		);
	CSocketClient(
		size_t maxFreeBuffers,
		size_t bufferSize, /* = 1024 */
		int  mSocketNo
		);
	void ReleaseBuffers();
	bool WaitAndVerifyCipher();
	/*
	 * Virtual function has different access specifier to base class
	 */
	virtual void Run();
    unsigned m_uKeyMode;
    unsigned m_uServerKey;
    unsigned m_uClientKey;

    #pragma pack( 1 )
    struct 
	{
		WORD			wLen;
		ACCOUNT_BEGIN	AccountBegin;
	}m_theSendAccountBegin;
    #pragma pack()

private:
	/*
	 * Override this to create the listening socket of your choice
	 */	
	virtual SOCKET CreateConnectionSocket( 
				const std::string &addressToConnectServer,
				unsigned short port);
	/*
	 * Interface for derived classes to receive state change notifications...
	 */	
	virtual void OnStartConnections() {};  //通知已经链接
	virtual void OnStopConnections() {};   //通知已经断开网络
	virtual void OnShutdownInitiated() {};
	virtual void OnShutdownComplete() {};
	virtual void OnConnect() {};
	virtual void OnClose() {};
	void OnRead(CIOBuffer *pBuffer);
	virtual void ReadCompleted( CIOBuffer *pBuffer ) = 0;
	virtual void OnWrite(){};
	virtual void OnError( const std::string &message );
	SOCKET m_connectSocket;
	//CEventSelect	m_eventSelect;
    CCriticalSection m_criticalSection_s;
	std::string	m_address;
	unsigned short m_port;

	enum SocketState
	{
		STATE_NONE,  //空闲
		STATE_STOP,  //断开
		STATE_START, //连接
	};
	int m_SocketState;
	int m_SocketNo;

};
#endif //__INCLUDE_SOCKETCLIENT_H__
