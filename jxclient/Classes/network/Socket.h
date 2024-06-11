/********************************************************************
	created:	2003/02/14
	file base:	Socket
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_SOCKET_H__
#define __INCLUDE_SOCKET_H__
#pragma once

#ifdef WIN32
#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif

#include "engine/KbugInfo.h"
#include <string>
#include "cocos2d.h"
#ifdef WIN32
#include <winsock.h>
//typedef int  socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
/*#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/wait.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  
#include <sys/un.h>  
#include <sys/time.h>  
#include <sys/ioctl.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <fcntl.h>  
#include <unistd.h>*/
#endif
//typedef int   SOCKET;
USING_NS_CC;
/*
 * namespace OnlineGameLib::Win32
 */
/*
 * CSocket
 */
class CSocket  
{
public:
	
	class InternetAddress;
	
	CSocket();	
	explicit CSocket( SOCKET theSocket );
	
	~CSocket();
	
	void Attach( SOCKET theSocket );	
	SOCKET Detatch();
	
	void Close();	
	void AbortiveClose();
	
	void Shutdown( int how );
	
	void Listen( int backlog );
	
	void Bind( const SOCKADDR_IN &address );	
	void Bind( const struct sockaddr &address, size_t addressLength );
	
	void Connect( const SOCKADDR_IN &address );	
	void Connect( const struct sockaddr &address, size_t addressLength );
	// Connect socket
	bool Connect_(const std::string ip, unsigned short port);
	
private:
		
	SOCKET m_socket;
		
	/*
	 * No copies do not implement
	 */
	CSocket( const CSocket &rhs );
	CSocket &operator=( const CSocket &rhs );

};

/*
 * CSocket::InternetAddress
 */
class CSocket::InternetAddress : public SOCKADDR_IN
{
public:
      
	InternetAddress( unsigned long address, unsigned short port );
	InternetAddress( const std::string &addressToConnectServer,
				unsigned short port );
};
#endif //__INCLUDE_SOCKET_H__
