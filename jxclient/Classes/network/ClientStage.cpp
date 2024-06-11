#include "ClientStage.h"
#include "KSG_EncodeDecode.h"
#include <time.h>

//CException;
#ifdef WIN32
CCriticalSection;
#endif

/*static unsigned gs_holdrand = time(NULL);

static inline unsigned _Rand()
{
    gs_holdrand = gs_holdrand * 214013L + 2531011L;
     
    return gs_holdrand;
}

static inline void RandMemSet(int nSize, unsigned char *pbyBuffer)
{
    _ASSERT(nSize);
    _ASSERT(pbyBuffer);

    while (nSize--)
    {
        *pbyBuffer++ = (unsigned char)_Rand();
    }
}*/

/*
 * class CGameClient
 */
CGameClient::CGameClient(size_t maxFreeBuffers, 
		size_t maxFreeBuffers_Cache,
		size_t bufferSize_Cache /*= 8192*/,
		size_t bufferSize,/*= 1024*/
		int    mSocketNo)
		: CSocketClient(maxFreeBuffers,bufferSize,mSocketNo)
		, m_theCacheAllocator(bufferSize_Cache, maxFreeBuffers_Cache) //大小,保留的数量
		, m_lRefCount(0)
		,__SocketStates(false)
		//, m_pfnCallbackClientEvent(NULL)
{
	m_pReadBuffer = m_theCacheAllocator.Allocate();//
	m_pRecvBuffer = m_theCacheAllocator.Allocate();
}

CGameClient::~CGameClient()
{
	m_pReadBuffer->Release();
	m_pRecvBuffer->Release();
}

//创建线程
bool CGameClient::Startup()
{
	bool hr = false;

	try
	{
		Start();

		hr = true;
	}
	catch(...)
	{
	}
	
	return hr;	
}

bool CGameClient::Cleanup(int isCleartheThread)
{
	bool hr = false;

	try
	{
		if  (isCleartheThread)
		{//只杀掉线程
			WaitForShutdownToComplete(isCleartheThread);
		}
		else
		{
			StopConnections();
			WaitForShutdownToComplete();
		}

		hr = true;
	}
	catch(...)
	{
	}

	return hr;
}

bool CGameClient::FsGameServerConnectTo(
				const char * const &pAddressToConnectServer, 
				unsigned short usPortToConnectServer )
{
	bool hr = false;

	try
	{
		Connect(pAddressToConnectServer,usPortToConnectServer);
		
		if (StartConnections())
		{
			hr = true;
		}
	}
	catch(...)
	{
	}
	
	return hr;
}
/*
bool CGameClient::RegisterMsgFilter( 
				void * lpParam, 
				CALLBACK_CLIENT_EVENT pfnEventNotify )
{
	m_lpCallBackParam = lpParam;
	m_pfnCallbackClientEvent = pfnEventNotify;
	return true;
}
*/

int CGameClient::SendMsg(const void *pBuffer, int nSize)
{
	return SendPackToServer((BYTE*)pBuffer, nSize);
}

bool CGameClient::SendPackToServer(
				const void * const pData,
				const size_t &datalength )
{
	static const size_t s_len_protocol = sizeof(WORD);//2字节
	
	if (!pData || 0 == datalength)
	{
		return false;
	}
	if (m_uKeyMode!=0)
		return false;

	CIOBuffer *pBuffer = Allocate();

	/*
	 * Add package header
	 */
	const size_t headlength = s_len_protocol + datalength;
	pBuffer->AddData(reinterpret_cast<const char *>(&headlength), s_len_protocol);//写入包头
	//unsigned uOldKey = m_uClientKey;
	//pBuffer->AddData(reinterpret_cast< const char* const>(pData), datalength);   //结构体数据
	//加密
	//KSG_EncodeBuf(datalength,(unsigned char*)(pBuffer->GetBuffer() + s_len_protocol),&m_uClientKey);
	KSG_EncodeBuf(datalength,(unsigned char*)pData,&m_uClientKey);
	pBuffer->AddData(reinterpret_cast< const char* const>(pData), datalength);   //结构体数据
	//unsigned char Buff[1024*8];
	//memset(Buff,'a',sizeof(Buff));
	//KSG_EncodeBuf(sizeof(Buff),Buff,&m_uClientKey);
	//开始发送
	bool isSend = false;
	//return false;
	if (Write(pBuffer))
		isSend =true;

	pBuffer->Release();

	return isSend;
}

const void * CGameClient::GetPackFromServer(size_t &datalength)
{
	CCriticalSection::Owner locker(m_csReadAction);
	m_pReadBuffer->Empty();
	m_pRecvBuffer = ProcessDataStream(m_pRecvBuffer);           //把收到的m_pRecvBuffer数据 解码填充到 m_pReadBuffer
	const unsigned char *pPackData = m_pReadBuffer->GetBuffer();
	datalength = m_pReadBuffer->GetUsed();
	return reinterpret_cast<const char*>(pPackData);
}

void CGameClient::setBufferEmpty()
{
	m_pReadBuffer->Empty();
	m_pReadBuffer->Empty();
}

bool CGameClient::Shutdown()
{
	StopConnections();
	return true;
}

bool CGameClient::QueryInterface( void** ppv )
{
	/*
	 * By definition all COM objects support the IUnknown interface
	 */
	*ppv = dynamic_cast< void * >(this);	
	return true;
}
    
unsigned long CGameClient::AddRef()
{
#ifdef WIN32
	return InterlockedIncrement(&m_lRefCount);
#else
	return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned long CGameClient::Release()
{
#ifdef WIN32
	if ( InterlockedDecrement(&m_lRefCount)>0)
#else
	if ( __sync_fetch_and_sub(&m_lRefCount, 1)>0)
#endif
	{
		return m_lRefCount;
	}
	delete this;
	return 0L;
}

void CGameClient::ProcessCommand(const CIOBuffer *pBuffer)
{
	static const size_t s_len_protocol = sizeof(unsigned short);//一个协议的长度 头几个字节
	const unsigned char *pPackData = pBuffer->GetBuffer();      //获取数据当前指针
	const size_t used              = pBuffer->GetUsed();        //获取已经使用的长度
	if ( used <= s_len_protocol )
	{//如果小于 2的 等于没有数据了 返回
        return;
	}

	if (m_uKeyMode!= 0)
		return;

	//char Buff[1024*8];
/*#ifdef WIN32
	char* Buff = (char*)_alloca(used - s_len_protocol);
#else
	char* Buff = new char[used - s_len_protocol];
#endif
	*/
	char * Buff=NULL;
	    Buff = (char *)malloc(used - s_len_protocol+1);
	if  (Buff==NULL) return;
	memset(Buff,0,sizeof(Buff));
	//RandMemSet(sizeof(Buff),(BYTE*)Buff);		// random memory for make a cipher
	memcpy(Buff,pPackData + s_len_protocol,used - s_len_protocol);
	//KSG_DecodeBuf(used - s_len_protocol,(unsigned char *)(pPackData + s_len_protocol),&m_uServerKey);
    KSG_DecodeBuf(used - s_len_protocol,(unsigned char *)Buff,&m_uServerKey);
	//m_pReadBuffer->AddData(( const unsigned char * )(pPackData + s_len_protocol), used - s_len_protocol);
	m_pReadBuffer->AddData(Buff,used - s_len_protocol);

	free(Buff);
	Buff = NULL;
}

size_t CGameClient::GetMinimumMessageSize() const
{
	static size_t length = sizeof(WORD) + sizeof(BYTE);
	/*
     * The smallest possible command we accept is a byte onlye package
	 */
	return length;
}
//size_t GetMessageSize(const CIOBuffer *pBuffer) const;
size_t CGameClient::GetMessageSize(const CIOBuffer *pBuffer) const
{
	const unsigned char *pData = pBuffer->GetBuffer();   
	const size_t used = pBuffer->GetUsed();
	WORD wHeadLen = (WORD)(*(WORD *)(pData));
	return (size_t)(wHeadLen);
}

CIOBuffer *CGameClient::ProcessDataStream(CIOBuffer *pBuffer)
{
	if  (pBuffer==NULL)
		return NULL;

	const size_t used = pBuffer->GetUsed();
	if (used >= GetMinimumMessageSize())
	{//如果已用数据 大于这个字节数才接收
		const size_t messageSize = GetMessageSize(pBuffer);
		if (messageSize == 0)
		{//如果为空数据
			if (used == (pBuffer->GetSize() - 1))
			{//缓存已经满了 就 关闭？				
     			Shutdown();			
				pBuffer->Empty();					
			}
		}
		else if (used == messageSize)
		{
			ProcessCommand(pBuffer);
			pBuffer->Empty();
		}
		else if (used > messageSize)
		{//实际如果已经大于收到 的重新分配内存
			CIOBuffer *pMessage = pBuffer->SplitBuffer(messageSize);
			ProcessCommand(pMessage);
			pMessage->Release();
		}
	}
	return pBuffer;
}

void CGameClient::OnStartConnections()
{
	__SocketStates =true;
#ifdef WIN32
	//if (m_pfnCallbackClientEvent)
//	{
	//	m_pfnCallbackClientEvent(m_lpCallBackParam, enumServerConnectCreate);
//	}
#endif
}

void CGameClient::OnShutdownInitiated()
{

}

void CGameClient::OnShutdownComplete()
{

}

void CGameClient::OnStopConnections()
{
	__SocketStates =false;
#ifdef WIN32
	//if (m_pfnCallbackClientEvent)
	//{
		//m_pfnCallbackClientEvent(m_lpCallBackParam, enumServerConnectClose);
	//}
	//CCMessageBox("服务器已经断开","服务器已经断开");
#endif
}
//完成收包 填充缓存区
void CGameClient::ReadCompleted(CIOBuffer *pBuffer)
{
	try
	{	
		const unsigned char *pPackData = pBuffer->GetBuffer();
		size_t used = pBuffer->GetUsed();
		if (used > 0)
		{
			CCriticalSection::Owner locker(m_csReadAction);	
			m_pRecvBuffer->AddData(reinterpret_cast< const char * >(pPackData), used);
			//char xieyi[1024]={0};
			//sprintf(xieyi,"%s:%d",pPackData,used);
			//CCMessageBox(xieyi,"ReadCompleted");
		}
		pBuffer->Empty();
	}
	catch(...)
	{
		StopConnections();
	}
}

CClientFactory::CClientFactory()
		: m_lRefCount(0),
		m_bufferSize(0),
		m_MaxPlauerCount(0),
        m_nPrecision(0)

{
	   pObject=NULL;
}

CClientFactory::~CClientFactory()
{

}

unsigned long CClientFactory::AddRef()
{
#ifdef WIN32
	return ::InterlockedIncrement(&m_lRefCount);
#else
	return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned long CClientFactory::Release()
{
#ifdef WIN32
	if ( ::InterlockedDecrement(&m_lRefCount)>0)
#else
	if ( __sync_fetch_and_sub(&m_lRefCount, 1)>0)
#endif
	{
		return m_lRefCount;
	}
	//__sync_fetch_and_add(&m_lRefCount,1);
	delete this;
	return 0L;
}
