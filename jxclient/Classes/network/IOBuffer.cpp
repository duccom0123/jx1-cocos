//#include "stdafx.h"
#include "IOBuffer.h"
//#include "Exception.h"
//#include "Utils.h"
CIOBuffer::CIOBuffer( Allocator &allocator, size_t size )
	:  m_operation( 0 )
      , m_sequenceNumber( 0 )
      , m_allocator( allocator )
      , m_ref(1)
      , m_size( size )
      , m_used( 0 )
{
	//( ( BYTE * )( ( DWORD )( pMemory + 3 ) & ( ~3 ) ) )
	m_buffer_ptr = new BYTE[size];

	memset(this,0,sizeof(OVERLAPPED));

	Empty();
}

CIOBuffer::~CIOBuffer()
{
	//SAFE_DELETE_ARRAY( m_buffer_ptr );
	if(m_buffer_ptr) { delete[] (m_buffer_ptr); (m_buffer_ptr)=NULL;}
}

void CIOBuffer::Empty()
{
	m_wsabuf.buf = reinterpret_cast<char *>(m_buffer_ptr);
	m_wsabuf.len = m_size;

	m_used = 0;
}

void *CIOBuffer::operator new( size_t objectSize, size_t /* bufferSize */ )
{
	/*
	 * ASSERT( sizeof( DWORD ) == 4 );
	 *
	 * For four bytes aligned base on win32 system
	 */

	void *pMem = new char[objectSize]; // + bufferSize + 4 ];

	return pMem;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
void CIOBuffer::operator delete( void *pObject, size_t/* bufferSize*/)
#else
void CIOBuffer::operator delete(void *pObject)
#endif
{
	//SAFE_DELETE_ARRAY( pObject );
	if(pObject) { delete [] (pObject); (pObject)=NULL;}
}

CIOBuffer *CIOBuffer::SplitBuffer( size_t bytesToRemove )
{
	CIOBuffer *pNewBuffer = m_allocator.Allocate();

	pNewBuffer->AddData( m_buffer_ptr, bytesToRemove );

	m_used -= bytesToRemove;

	memmove(m_buffer_ptr, m_buffer_ptr + bytesToRemove, m_used);

	return pNewBuffer;
}

VOID CIOBuffer::RemoveBuffer( size_t bytesToRemove )
{
	if ( m_used < bytesToRemove )
		return;

	m_used -= bytesToRemove;

	memmove( m_buffer_ptr, m_buffer_ptr + bytesToRemove, m_used );
}

CIOBuffer *CIOBuffer::AllocateNewBuffer() const
{
	return m_allocator.Allocate();
}

void CIOBuffer::AddData( const char * const pData, size_t dataLength )
{
	if (dataLength > m_size - m_used)
	{
		return;
	}

	memcpy(m_buffer_ptr + m_used,pData,dataLength);

	m_used += dataLength;
}

void CIOBuffer::AddData( const BYTE * const pData, size_t dataLength )
{
	AddData( reinterpret_cast< const char * >( pData ), dataLength );
}

void CIOBuffer::AddData(BYTE data)
{
	AddData(&data,1);
}

void CIOBuffer::Release()
{
	if (m_ref == 0)
	{
		return;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if ( 0 == ::InterlockedDecrement(&m_ref))
#else
	//if ( 0 == __sync_fetch_and_sub(&m_ref,1))
	m_ref--;
#endif
	
	if (m_ref<=0)
	{
		m_sequenceNumber = 0;
		m_operation = 0;
		m_used = 0;

		m_allocator.Release(this);
	}
}

/*
 * CIOBuffer::Allocator
 */

CIOBuffer::Allocator::Allocator( size_t bufferSize, size_t maxFreeBuffers )
   :  m_bufferSize( bufferSize ),
      m_maxFreeBuffers( maxFreeBuffers )
{

}

CIOBuffer::Allocator::~Allocator()
{
	try
	{
		Flush();
	}
	catch(...)
	{
		//TRACE( "CIOBuffer::Allocator::~Allocator exception!" );
	}
}

CIOBuffer *CIOBuffer::Allocator::Allocate()
{
	CCriticalSection::Owner lock( m_criticalSection );

	CIOBuffer *pBuffer = 0;

	if (!m_freeList.Empty())
	{//如果在空闲列表 就直接用
		pBuffer = m_freeList.PopNode();
		pBuffer->AddRef();//计数增加+
	}
	else
	{ //否则就使用 新建
		pBuffer = new(m_bufferSize)CIOBuffer(*this,m_bufferSize);
		if (!pBuffer)
		{
		  return NULL;
		}
		OnBufferCreated();
	}
	//已经使用的buf
	//m_activeList.PushNode( pBuffer );
	OnBufferAllocated();
	
	return pBuffer;
}

void CIOBuffer::Allocator::Release(CIOBuffer *pBuffer)
{
	if (!pBuffer)
	{
		return;
	}

	CCriticalSection::Owner lock(m_criticalSection);
	
	OnBufferReleased();
	
	/*
	 * unlink from the in use list
	 */	
	pBuffer->RemoveFromList();
	
	if (m_maxFreeBuffers == 0 || m_freeList.Count() < m_maxFreeBuffers)
	{//放入空闲列表
		pBuffer->Empty();           
		
		/*
		 * add to the free list
		 */
		
		m_freeList.PushNode( pBuffer );
	}
	else
	{//直接删包
		DestroyBuffer(pBuffer);
	}
}

void CIOBuffer::Allocator::DestroyBuffer(CIOBuffer *pBuffer)
{
	//SAFE_DELETE( pBuffer );
	 if(pBuffer){delete pBuffer;pBuffer=NULL;}
}

void CIOBuffer::Allocator::Flush()
{
	CCriticalSection::Owner lock( m_criticalSection );
	
	while ( !m_activeList.Empty() )
	{
		OnBufferReleased();
		DestroyBuffer( m_activeList.PopNode() );
	}
	
	while ( !m_freeList.Empty() )
	{
		DestroyBuffer( m_freeList.PopNode() );
	}
}

/*
 * CIOBuffer::InOrderBufferList
 */

CIOBuffer::InOrderBufferList::InOrderBufferList(
				CCriticalSection &criticalSection)
			:   m_next(0),
				m_criticalSection( criticalSection )
{
}

void CIOBuffer::InOrderBufferList::AddBuffer( CIOBuffer *pBuffer )
{
	CCriticalSection::Owner lock( m_criticalSection );
	
	std::pair< BufferSequence::iterator, bool > result = 
		m_list.insert( BufferSequence::value_type( pBuffer->GetSequenceNumber(), pBuffer ) );
	
	if ( !result.second )
	{
	}
}

CIOBuffer *CIOBuffer::InOrderBufferList::ProcessAndGetNext()
{
	CCriticalSection::Owner lock( m_criticalSection );
	
	m_next ++;
	
	CIOBuffer *pNext = 0;
	
	BufferSequence::iterator it;
	
	it = m_list.begin();
	
	if ( it != m_list.end() )
	{
		if ( it->first == m_next )
		{
			pNext = it->second;
			
			m_list.erase( it );
		}
		else
		{
			/*DEBUG_ONLY( Output( ToString( this ) +
				_T(" Got buffer : ") + 
				ToString( it->first ) +
				_T("Want buffer : ") +
				ToString( m_next ) ) );*/
		}
	}
	
	return pNext;
}

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext()
{
	CCriticalSection::Owner lock( m_criticalSection );
	
	CIOBuffer *pNext = 0;
	
	BufferSequence::iterator it;
	
	it = m_list.begin();
	
	if ( it != m_list.end() )
	{
		if ( it->first == m_next )
		{
			pNext = it->second;
			
			m_list.erase(it);
		}
		else
		{
			/*DEBUG_ONLY( Output( ToString( this ) + 
				_T(" Got buffer  : ") + 
				ToString( it->first ) + 
				_T("Want buffer : ") + 
				ToString( m_next ) ) );*/
		}
	}
	
	return pNext;
}

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext( CIOBuffer *pBuffer )
{
	CCriticalSection::Owner lock( m_criticalSection );
	
	if ( m_next == pBuffer->GetSequenceNumber() )
	{
		return pBuffer;
	}
	
	std::pair< BufferSequence::iterator, bool > result = 
		m_list.insert( BufferSequence::value_type( pBuffer->GetSequenceNumber(), pBuffer ) );
	
	if ( !result.second )
	{
	}
	
	CIOBuffer *pNext = 0;
	
	BufferSequence::iterator it;
	
	it = m_list.begin();
	
	if (it != m_list.end()) 
	{
		if (it->first == m_next)
		{
			pNext = it->second;
			
			m_list.erase(it);
		}
		else
		{
			/*DEBUG_ONLY( Output( ToString( this ) +
				_T(" Got buffer  : ") + 
				ToString( it->first ) + 
				_T("Want buffer : ") + 
				ToString( m_next ) ) );*/
		}
	}
	
	return pNext;
}

void CIOBuffer::InOrderBufferList::ProcessBuffer()
{
	CCriticalSection::Owner lock( m_criticalSection );

	/*DEBUG_ONLY( Output( ToString( this ) +
		_T(" Processed : ") + 
		ToString( m_next ) ) );*/

	m_next ++;
}

void CIOBuffer::InOrderBufferList::Reset()
{
	m_next = 0;

	if ( !m_list.empty() )
	{
	}
}
