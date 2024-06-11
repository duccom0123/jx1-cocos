//#include "stdafx.h"
#include "gamecore/KCore.h"
#include "SocketClient.h"
//#include "gamecore/KCore.h"
#include "gamecore/CoreShell.h"
//#include "Win32Exception.h"
//#include "Utils.h"
#include "Socket.h"
//#include "Macro.h"
#include <vector>
//#include "event.h"
#ifndef WIN32
#include "unistd.h"
#endif
//CEvent *m_ReadEvent = NULL;
extern iCoreShell*	g_pCoreShell; //全局调用
using std::vector;
/*
CEvent *_CreateEvent(bool bManualReset, bool bInitialState)
{
	CEvent *pEvent = new CEvent(bManualReset, bInitialState);
	assert(pEvent);

	bool bRet = pEvent->CreateEvent();
	assert(bRet);

	return pEvent;
}

unsigned int WaitForSingleObject(CEvent *pEvent, int cms)
{
	assert(pEvent);
	if( pEvent->Wait(cms) )
	{
		return 0;
	}

	return 1;
}
*/
CSocketClient::CSocketClient(
		const std::string &addressToConnectServer,
		unsigned short portToConnectServer,
		size_t maxFreeBuffers,
		size_t bufferSize, /* = 1024 */
		int mSocketNo)
	  : CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
		m_address(addressToConnectServer),
		m_port(portToConnectServer),
		m_connectSocket(INVALID_SOCKET),
		m_SocketState(STATE_NONE),
		m_SocketNo(mSocketNo)
		//m_ReadEvent(_CreateEvent(true, true))
{
	 // m_ReadEvent = _CreateEvent(true, true);
#ifdef WIN32 
	unsigned short wVersionRequested = 0x202;
	if ( 0 != ::WSAStartup(wVersionRequested, &m_data ))
	{
		CCMessageBox("socket is fail..","socket");
		//throw CWin32Exception( _T("CUsesWinsock::CUsesWinsock()"), ::WSAGetLastError() );
	}
#endif
}

CSocketClient::CSocketClient(
		size_t maxFreeBuffers,
		size_t bufferSize, /* = 1024 */
		int   mSocketNo
		)
	: CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
	m_port(0),
	m_connectSocket(INVALID_SOCKET),
	m_SocketNo(mSocketNo)
{
	//m_ReadEvent = _CreateEvent(true, true);
#ifdef WIN32 
	unsigned short wVersionRequested = 0x202;
	if ( 0 != ::WSAStartup(wVersionRequested, &m_data ))
	{
		CCMessageBox("socket is fail..","socket");
		//throw CWin32Exception( _T("CUsesWinsock::CUsesWinsock()"), ::WSAGetLastError() );
	}
#endif

}
void CSocketClient::Connect(
			   const std::string &addressToConnectServer,
			   unsigned short portToConnectServer
			   )
{
	m_address = addressToConnectServer;
	m_port = portToConnectServer;
}

CSocketClient::~CSocketClient()
{
	//if (m_ReadEvent)
	  // CloseHandle(m_ReadEvent);
	try
	{
		StopConnections();
	}
	catch(...)
	{	
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	::WSACleanup();
#endif
}

bool CSocketClient::StartConnections()
{
	CCriticalSection::Owner lock(m_criticalSection_s);
    
	if (INVALID_SOCKET == m_connectSocket)
	{//如果是空链接的话 就创建一个链接
		/*
		 * Call to unqualified virtual function
		 */
		  // OnStartConnections();
		/*
		 * call to unqualified virtual function
		 */
		//CCMessageBox("CreateConnectionSocket","Socket");
		m_connectSocket = CreateConnectionSocket(m_address, m_port);

		if (!WaitAndVerifyCipher())
		{//等待验证连接
			//CCMessageBox("WaitAndVerifyCipher false","Socket");
			return false;
		}

		//m_eventSelect.AssociateEvent( m_connectSocket, FD_CONNECT | FD_CLOSE | FD_READ );
		//m_ReadEvent->Set();
		m_SocketState   =STATE_START;
		return true;
	}
	return false;
}

void CSocketClient::StopConnections()
{
	CCriticalSection::Owner lock(m_criticalSection_s);

	if (INVALID_SOCKET != m_connectSocket)
	{
		/*
		 * Change the sockH to be blocking
		 */
		/*
		 * Force an abortive close.
		 */	
		LINGER lingerStruct;
		
		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;
		
		if (SOCKET_ERROR == ::setsockopt(m_connectSocket, 
								SOL_SOCKET, 
								SO_LINGER, 
								(char *)&lingerStruct, 
								sizeof(lingerStruct)))
		{
		}
		//设置无信号 断开了
		//m_ReadEvent->Reset();
		//m_eventSelect.DissociateEvent();
		
#ifdef WIN32
	if (0!= ::closesocket(m_connectSocket))
		{
		}
#else
      close(m_connectSocket);
#endif

		m_connectSocket =INVALID_SOCKET;
		m_SocketState   =STATE_STOP;
		/*
		 * Call to unqualified virtual function
		 */
		OnStopConnections();	  //通知服务器已经断开链接
	}
}

SOCKET CSocketClient::CreateConnectionSocket( 
					  const std::string &addressToConnectServer,
					  unsigned short port)
{
#ifdef WIN32
	//WORD wVersionRequested = 0x202;
	//WSAStartup(wVersionRequested, &wsaData);
	SOCKET s = ::WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,0);
	unsigned long ul=1;
	int ret;
	ret=ioctlsocket(s,FIONBIO,(unsigned long *)&ul);//设置成非阻塞模式。
	if(ret==SOCKET_ERROR)//设置失败。
	{
	  CCMessageBox("ioctlsocket is fail..","ioctlsocket");
	}
#else
	SOCKET s = socket(AF_INET, SOCK_STREAM,IPPROTO_IP);
	int flags = fcntl(s,F_GETFL,0);                        //获取文件的flags值。
	fcntl(s,F_SETFL,flags|O_NONBLOCK);                     //设置成非阻塞模式；
#endif
	char ninfo[64]={0};
	if ( INVALID_SOCKET == s )
	{
		t_sprintf(ninfo,"INVALID_SOCKET:%d",s);
		CCMessageBox(ninfo,"CreateConnectionSocket");
		return INVALID_SOCKET;
	}

	//t_sprintf(ninfo,"SOCKET:%d",s);
	//CCMessageBox(ninfo,"CreateConnectionSocket");

	CSocket connectionSocket(s);
#ifdef WIN32
	CSocket::InternetAddress localAddress(addressToConnectServer,port);
	//connectionSocket.NewConnect(localAddress);
	connectionSocket.Connect(localAddress);
#else
	connectionSocket.Connect_(addressToConnectServer,port);
#endif

	return connectionSocket.Detatch();	
}

void CSocketClient::InitiateShutdown()
{
	/*
	 * Signal that the dispatch thread should shut down all worker threads and then exit
	 */
	//m_ReadEvent->Reset();
	StopConnections();
	/*
	 * Call to unqualified virtual function
	 */
	OnShutdownInitiated();
}

void CSocketClient::WaitForShutdownToComplete(int isCleartheThread)
{
	/*
	 * If we havent already started a shut down, do so...
	 */
#ifdef WIN32
	if (isCleartheThread)
		Terminate();
	else
	{
		InitiateShutdown();
		Terminate();
	}
#else
	if (isCleartheThread)
		Terminate(1);
	else
	{
		InitiateShutdown();
		//Wait();
		Terminate(1);
	}
#endif 
}
/*
void CSocketClient::Run()
{//会阻塞。。其它 sk 接收数据
	//return;
	try
	{		
		timeval pstTime = {0,0};
		DWORD g_nServiceLoop = 0;
		while (true)
		{ 
		 if (++g_nServiceLoop & 0x80000000)
		 {
			g_nServiceLoop = 0;
		 }
		 if (g_nServiceLoop & 0x1)
		 {
#ifdef WIN32
			Sleep(1);     //挂起1毫秒
#else
			usleep(1000); //微秒 毫秒=1000微妙  sleep 为秒
#endif
		 }
				//登陆服务器后 帐号服务器挂起 不接收数据
		 if  (m_bIsGameSevConnecting && m_SocketNo==0)
			 continue;
		 CIOBuffer *pReadContext = Allocate();

		 if (m_SocketNo==1 && m_bIsGameSevConnecting)
	     {//游戏服务器
				DWORD dwNumBytes = 0;
	            DWORD dwFlags = 0;
				if (!pReadContext)
					break;
	            pReadContext->SetupRead(); //设置结构里的数据的当前指针
	            dwNumBytes = recv(m_connectSocket,pReadContext->GetWSABUF()->buf,pReadContext->GetWSABUF()->len,dwFlags);
	//接收到的字节数量
	            if (SOCKET_ERROR == dwNumBytes || 0 == dwNumBytes)
	            {//错误或者 服务器端已经断开
					//pReadContext->Release();
	            }
	            else if (dwNumBytes>0)
	            {//开始执行客户端函数
					pReadContext->Use(dwNumBytes);//设置长度增加了多少
					//pReadContext->Use(dwNumBytes);//设置长度增加了多少
					//完成数据接收 填充缓存区
					//ReadCompleted(pReadContext);

					//const unsigned char *pPackData = pBuffer->GetBuffer();
					//size_t used = pBuffer->GetUsed();
					unsigned int nSize           = pReadContext->GetUsed();
					const unsigned char* pBuffer = pReadContext->GetBuffer();
					
					PROTOCOL_MSG_TYPE*	pMsg = (PROTOCOL_MSG_TYPE*)pBuffer; //当前包
					
					while(pMsg < (PROTOCOL_MSG_TYPE*)(pBuffer + nSize))
					{//帅选数据包
						PROTOCOL_MSG_TYPE Msg = *pMsg;
						// 跨服务器的协议
						if (Msg == s2c_notifyplayerexchange)
						{//转换地图或转服
							//ProcessSwitchGameSvrMsg(pMsg);
							break;
						}
						else if (g_pCoreShell)
						{
							char xieyi[64]={0};
							t_sprintf(xieyi,"%d:%d",Msg,dwNumBytes);

							CCMessageBox(xieyi,"g_pCoreShell");
							if  (Msg <= s2c_clientbegin ||  Msg >= s2c_end || g_pCoreShell->GetProtocolSize(Msg)==0)
								break;
							if (Msg == s2c_syncworld)
								CCMessageBox("s2c_syncworld...","s2c_syncworld");
							if (Msg == s2c_syncnpc)
								CCMessageBox("s2c_syncnpc...","s2c_syncnpc");
							if (Msg == s2c_syncplayer)
								CCMessageBox("s2c_syncplayer...","s2c_syncplayer");
							if (Msg == s2c_synonlineplayer) //收不到
								CCMessageBox("s2c_synonlineplayer...","s2c_synonlineplayer");
							if (Msg == s2c_syncclientend)
								CCMessageBox("s2c_syncclientend....","s2c_syncclientend");

							g_pCoreShell->NetMsgCallbackFunc(pMsg);          //接受从服务器发来的协议消息

							if (g_pCoreShell->GetProtocolSize(Msg) > 0)
								pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + g_pCoreShell->GetProtocolSize(Msg));
							else //没有限制长度的 -1
								pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + PROTOCOL_MSG_SIZE + (*(unsigned short*)(((char*)pMsg) + PROTOCOL_MSG_SIZE)));			
						}
	                 }//END WIHEL
			  }
		 }
		else
		{//帐号服务器的
			OnRead(pReadContext);
		}
		pReadContext->Release();
	  } // while ( ... 
	}
	catch(...)
	{
		CCMessageBox("run is catch Error","Run");
		StopConnections();
		//m_ReadEvent->Reset();
	}
	OnShutdownComplete();
	//return 1;
}
*/
void CSocketClient::Run()
{//会阻塞。。其它 sk 接收数据
	//return;
	try
	{		
		timeval pstTime = {0,0};
		DWORD g_nServiceLoop = 0;
		//fd_set fdRead={0};
		//fd_set fdRead;
		 while (true)
		 { 

			//登陆服务器后 帐号服务器挂起 不接收数据
			if (m_SocketNo==0 && m_bIsGameSevConnecting)
				continue;

			//FD_ZERO(&fdRead);
			//FD_SET(m_connectSocket,&fdRead);
			//memset(&fdRead,0,sizeof(fdRead));
			//fdRead.fd_count=0;
			//fdRead.fd_array[0] = m_connectSocket;
			//是否有数据可以
			//int ret = select(m_connectSocket+1,&fdRead,NULL,NULL,0); //&pstTime 
			//if (ret== SOCKET_ERROR || ret==0)  
			//{//出错   
				//break;  
			//}  
			//else if (ret>0)  
			//{   
			   //if(FD_ISSET(m_connectSocket, &fdRead))
			   //{ 
		       //}
			//}
			if  (INVALID_SOCKET == m_connectSocket)
				break;

			CIOBuffer *pReadContext = Allocate();
			if (pReadContext)
			{
				OnRead(pReadContext);
				pReadContext->Release();
			}
#ifdef WIN32
			if (++g_nServiceLoop & 0x80000000)
			{
				g_nServiceLoop = 0;
			}
			if (g_nServiceLoop & 0x1)
				Sleep(10);       //挂起1毫秒
#else
			usleep(10000); //微秒 毫秒=1000微妙  sleep 为秒
			//sleep(1);
#endif
		} // while ( ... 	
	}
	catch(...)
	{
		//CCMessageBox("run is catch Error","Run");
		StopConnections();
		//m_ReadEvent->Reset();
	}
	//OnShutdownComplete();
	//return 1;
}

void CSocketClient::ReleaseBuffers()
{
	Flush();
}

void CSocketClient::OnError( const std::string &message )
{
}

void CSocketClient::OnRead( CIOBuffer *pBuffer )
{//接收包
	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;
	pBuffer->SetupRead(); //设置结构里的数据的当前指针
	dwNumBytes = recv(m_connectSocket,pBuffer->GetWSABUF()->buf,pBuffer->GetWSABUF()->len,dwFlags);
	//接收到的字节数量
	/*if (SOCKET_ERROR == dwNumBytes || 0 == dwNumBytes)
	{//错误或者 服务器端已经断开
		//StopConnections();
		return;
		DWORD lastError = ::WSAGetLastError();
		if ( ERROR_IO_PENDING != lastError)
		{
			if (lastError == WSAECONNABORTED || //建立连接失败
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)        //远端结束了连接。
			{
			
				//ECONNRESET是linux环境网络编程产生的错误，错误码为104，
				//WSAECONNRESET是windows环境网络编程产生的错误，错误码为10054
				//两者产生的原因都一样，分以下几种情况： 
				//- 接收端recv或者read， 对端已经关闭连接，recv/read返回该错误 
				//- 对端重启连接，还未建立连接 
				//- 发送端已经断开连接，但是调用send会触发这个错误
			
				//CCMessageBox("read is  Error and stop sk","read");
				//StopConnections();
			}
		}
	}
	*/
	if (dwNumBytes>0)
	{	
		pBuffer->Use(dwNumBytes);//设置长度增加了多少
		//完成数据接收 填充缓存区
		ReadCompleted(pBuffer);
	}
	else
	{
		if (SOCKET_ERROR == dwNumBytes || 0 == dwNumBytes)
		{//服务器已经断开连接
			//StopConnections();
			OnStopConnections();	  //通知服务器已经断开链接
			return;
		}
	}
}

void CSocketClient::Write( const char *pData, size_t dataLength )
{
	if ( INVALID_SOCKET != m_connectSocket &&
		dataLength > 0 &&
		pData )
	{
		CIOBuffer *pBuffer = Allocate();
		
		pBuffer->AddData( pData, dataLength );

		Write( pBuffer );
		
		pBuffer->Release();
	}
}

//static struct timeval gs_CheckRW_timeout = {0,0};
//开始发包
bool CSocketClient::Write( CIOBuffer *pBuffer )
{
	size_t uDataLength = 0;

	if (NULL== pBuffer ||\
		(0 == (uDataLength = pBuffer->GetUsed())) ||\
		INVALID_SOCKET == m_connectSocket)
	{
		return false;
	}

	/*
	 * Begin to send data
	 */

	int nError = 0;
	DWORD lastError= 0;
	
	DWORD dwFlags = 0;//MSG_NOSIGNAL;
	DWORD dwSendNumBytes = 0;
	timeval gs_CheckRW_timeout = {0,0};
	pBuffer->SetupWrite();
	WSABUF &wsa = *(pBuffer->GetWSABUF());
		//fd_set writefds;
		//memset(&writefds, 0, sizeof(writefds));
		//writefds.fd_count = 1;
		//writefds.fd_array[0] = m_connectSocket;
		do
		{   //循环检测 指到可以发送
			/*
			 * Check socket status //检测 sk 状态 如果可以发送就发送
			 */
/*
#ifdef WIN32
			FD_ZERO(&writefds);
			//FD_SET(m_connectSocket,&writefds);
			//memset( &writefds, 0, sizeof( writefds ) );
			writefds.fd_count = 1;
			writefds.fd_array[0] = m_connectSocket;   //
			nError = select(1,NULL,&writefds,NULL,0); //阻塞在这里了m_connectSocket+1
			//CCMessageBox("select is end","send");
			if (SOCKET_ERROR==nError || 0 ==nError)
			{//连接如果错误 直接返回了
				//StopConnections();
				//CCMessageBox("select is error","send");
				break;
			}

			if (nError>0)
#endif*/
			{//可以发送数据
				//CCMessageBox("send is ok","send");
				wsa.len		-= dwSendNumBytes;
				wsa.buf		+= dwSendNumBytes;//发送的子节
				uDataLength	-= dwSendNumBytes;
				dwSendNumBytes =send(m_connectSocket,wsa.buf,wsa.len,dwFlags);
				if (dwSendNumBytes == -1 || dwSendNumBytes == 0)
				{//服务器断开　或　出错
					break;
				}
			   /*
		       * Continue to send buffer when 'dwSendNumBytes < uDataLength'
		       */
				if (SOCKET_ERROR != nError && dwSendNumBytes >= uDataLength)
				{//发送完了,直接返回了
					return true;
				}
			}	
		} while (true);
	return false;
}

static const DWORD g_dwTimeout = 5;//秒

bool CSocketClient::WaitAndVerifyCipher()
{
	fd_set	fdRead  = { 0 };
	timeval stTime;
	timeval	*pstTime = NULL;
	if (-1 != g_dwTimeout) 
	{
		stTime.tv_sec = g_dwTimeout; //秒
		stTime.tv_usec = 0;
		pstTime = &stTime;
	}

	///m_ReadEvent->Set();
	/*
	 * Select function set read timeout
	 */
	SOCKET s = ( SOCKET )m_connectSocket;
	DWORD dwTotalLength = 0;
	while (true)
	{
		DWORD dwBytesRead = 0L;
		/*
		 * Set Descriptor
		 */
	    //fd_set fdRead;
		//memset(&fdRead,0,sizeof(fdRead));
		//fdRead.fd_count    = 0;
		//fdRead.fd_array[0] = s;
		FD_ZERO(&fdRead);
		FD_SET(s,&fdRead);
		int res;
#ifdef WIN32
		res = select(0,&fdRead,NULL,NULL,pstTime); //FD_SETSIZE
#else
		res = select(s+1,&fdRead,NULL,NULL,pstTime);
#endif
		if (res > 0)
		{
			res = recv(s,(LPSTR)&m_theSendAccountBegin + dwTotalLength, sizeof( m_theSendAccountBegin ) - dwTotalLength,0);
			if (res > 0)
			{
				dwBytesRead = res;
				dwTotalLength += dwBytesRead;			
			}
		}
		
		if (res <= 0)
		{
			/*
			 * Timeout and exit
			 */
			//m_ReadEvent->Reset();
			//char ninfo[64]={0};
			//t_sprintf(ninfo,"Timeout and exit:%d",s);
			//CCMessageBox(ninfo,"WaitAndVerifyCipher");
			break;
		}
		
		if (dwTotalLength == sizeof(m_theSendAccountBegin))
		{
			ACCOUNT_BEGIN *pAccountBegin = ( ACCOUNT_BEGIN * )(&m_theSendAccountBegin.AccountBegin);
			
			if (pAccountBegin->ProtocolType == CIPHER_PROTOCOL_TYPE)
			{
				if (pAccountBegin->nMode != 0)
				{
					CCMessageBox("Server send cliper mode error","cliper");
				/*
				* Write this message and then shutdown the sending side of the socket.
					*/

                #ifdef WIN32
					if (0 != ::closesocket(m_connectSocket))
					{
					}
                #else
                     close(m_connectSocket);
                #endif

					m_connectSocket = INVALID_SOCKET;
					return false;
				}
				m_uKeyMode   = pAccountBegin->nMode; 
				m_uServerKey = ~(pAccountBegin->ServerKey);
				m_uClientKey = ~(pAccountBegin->ClientKey);
				//m_ReadEvent->Reset();
				return true;
			}
		}
	}
	//m_ReadEvent->Reset();
	return false;
}
