/********************************************************************
	created:	2003/02/14
	file base:	UsesWinsock
	file ext:	h
	author:		liupeng
	
	purpose:	Create an environment for socket
*********************************************************************/
#ifndef __INCLUDE_USESWINSOCK_H__
#define __INCLUDE_USESWINSOCK_H__
#pragma once

#ifdef WIN32
#include <winsock2.h>

class CUsesWinsock
{
public:
	
	CUsesWinsock();	
	~CUsesWinsock();
	
private:
	
	WSADATA m_data;
	
	/*
	 * No copies do not implement
	 */
	CUsesWinsock( const CUsesWinsock &rhs );
	CUsesWinsock &operator=( const CUsesWinsock &rhs );
};
#endif //CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#endif //__INCLUDE_USESWINSOCK_H__
