//#include "stdafx.h"
#include "UsesWinsock.h"
//#include "Win32Exception.h"

#ifdef WIN32
CUsesWinsock::CUsesWinsock()
{
	unsigned short wVersionRequested = 0x202;
	
	if ( 0 != ::WSAStartup( wVersionRequested, &m_data ) )
	{
		//throw CWin32Exception( _T("CUsesWinsock::CUsesWinsock()"), ::WSAGetLastError() );
	}
}
      
CUsesWinsock::~CUsesWinsock()
{
	::WSACleanup();
}
#endif
