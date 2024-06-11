
#include "Socket.h"
//#include "Macro.h"

/*
 * namespace OnlineGameLib::Win32
 */

CSocket::CSocket()
	: m_socket( INVALID_SOCKET )
{
}

CSocket::CSocket( SOCKET theSocket )
	: m_socket( theSocket )
{
	if ( INVALID_SOCKET == m_socket )
	{
	}
}
      
CSocket::~CSocket()
{
	if (INVALID_SOCKET != m_socket)
	{
		try
		{
			AbortiveClose();
		}
		catch(...)
		{
		}
	}
}

void CSocket::Attach( SOCKET theSocket )
{
	AbortiveClose();
	
	m_socket = theSocket;
}

SOCKET CSocket::Detatch()
{
	SOCKET theSocket = m_socket;
	
	m_socket = INVALID_SOCKET;
	
	return theSocket;
}

void CSocket::Close()
{
#ifdef WIN32
	if (0!= ::closesocket(m_socket))
		{
		}
#else
      close(m_socket);
#endif

}

void CSocket::AbortiveClose()
{
	LINGER lingerStruct;
	
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	
	if ( SOCKET_ERROR == setsockopt( m_socket,
				SOL_SOCKET, 
				SO_LINGER, 
				(char *)&lingerStruct,
				sizeof(lingerStruct)))
	{
	}
	
	Close();
}

void CSocket::Shutdown( int how )
{
	if (0!= ::shutdown(m_socket,how))
	{
	}
}

void CSocket::Listen( int backlog )
{
	if ( SOCKET_ERROR == listen( m_socket, backlog ) )
	{
	}
}

void CSocket::Bind( const SOCKADDR_IN &address )
{
	if ( SOCKET_ERROR == ::bind( m_socket, 
			reinterpret_cast< struct sockaddr * >( const_cast< SOCKADDR_IN * >( &address ) ),
			sizeof( SOCKADDR_IN ) ) )
	{
	}
}

void CSocket::Bind( const struct sockaddr &address, size_t addressLength )
{
	if ( SOCKET_ERROR == ::bind( m_socket, 
			const_cast< struct sockaddr * >( &address ),
			addressLength ) )
	{
	}
}

bool CSocket::Connect_(const std::string ip, unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip.c_str());
	svraddr.sin_port = htons(port);
	int ret =::connect(m_socket,(struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret == SOCKET_ERROR) {
//#ifndef WIN32
		//close(m_socket);
//#endif
		//char nnet[128]={0};
		//sprintf(nnet,"ip:%s,p:%d,s:%d",ip.c_str(),port,m_socket);
		//CCMessageBox(nnet,"Connect_");
		return false;
	}
	return true;
}
void CSocket::Connect(const SOCKADDR_IN &address )
{
	if (SOCKET_ERROR ==::connect(m_socket, 
		reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN *>(&address)), 
		sizeof(SOCKADDR_IN)))
	{
#ifndef WIN32
		//CCMessageBox("connect fail..","connect");
		 //close(m_socket);
#endif
		return;
	}
	//CCMessageBox("connect suss..","connect");
}

void CSocket::Connect( const struct sockaddr &address, size_t addressLength )
{
	if ( SOCKET_ERROR ==::connect( m_socket,
		const_cast<struct sockaddr *>(&address ),
		addressLength ) )
	{
#ifndef WIN32
		close(m_socket);
#endif
	}
}

/*
 * CSocket::InternetAddress
 */

CSocket::InternetAddress::InternetAddress( unsigned long address, unsigned short port )
{
	/*
	 * member 'sockaddr_in::sin_zero not initialised
	 * ...
	 */

	sin_family = AF_INET;
	sin_port = htons( port );
	sin_addr.s_addr = htonl( address );  
}

CSocket::InternetAddress::InternetAddress(
				const std::string &addressToConnectServer,
				unsigned short port)
{
	sin_family = AF_INET;
	sin_port = htons(port);
	sin_addr.s_addr = inet_addr(addressToConnectServer.c_str());
	
	/*
	 * struct sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = inet_addr(ip.c_str());
    svraddr.sin_port = htons(port);
	 * member 'sockaddr_in::sin_zero not initialised
	 */
}

/*
 * CSocket::Exception
 */
