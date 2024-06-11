#include "KCore.h"
#include "KItem.h"
#include "KInventory.h"

KInventory::KInventory()
{
	m_pArray = NULL;
	m_nWidth = 0;
	m_nHeight= 0;
	m_nBakWidth = 0;
	m_nBakHeight = 0;
}

KInventory::~KInventory()
{
    Release();
}

BOOL KInventory::Init(int nWidth, int nHeight)
{
	if (m_pArray)
	{
		delete [] m_pArray;
		m_pArray = NULL;
	}
	m_pArray = new int[nWidth * nHeight];

	if (!m_pArray)
		return FALSE;

	//ZeroMemory(m_pArray, sizeof(int) * nWidth * nHeight);
	memset(m_pArray, 0,sizeof(int) * nWidth * nHeight);

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nBakWidth = nWidth;
	m_nBakHeight = nHeight;
	return TRUE;
}

void KInventory::Release()
{
	if (m_pArray)
	{
		delete []m_pArray;
		m_pArray = NULL;
	}
	this->m_nWidth = 0;
	this->m_nHeight = 0;
	this->m_nBakWidth = 0;
	this->m_nBakHeight = 0;
	this->m_nMoney = 0;
	this->m_nXu = 0;
}

void KInventory::Clear()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
	m_nMoney = 0;
	m_nXu    = 0;
}

#pragma optimize( "y", off)
//检测物品是否能放进容器，位置上是否有物品,否则设置该位置的标示为idx
BOOL KInventory::PlaceItem(int nX, int nY, int nIdx, int nWidth, int nHeight)
{//m_nWidth m_nHeight 为容器的大小
	if (!m_pArray)//容器面积
		return FALSE;

	if (nX < 0 || nY < 0 || nWidth <= 0 || nHeight <= 0 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;

	if (nIdx <= 0)
	{
		//_ASSERT(0);
		return FALSE;
	}

	int i, j;
	/*int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)  //容器宽度
	{//检查 是否能把东西放进去
		for (j = nY; j < nY + nHeight; j++)  //容器高度
		{
			//if (!nOldIdx) //为0
			nOldIdx = m_pArray[j * m_nWidth + i]; //该位置上的标示idx	

			if (nOldIdx)
			{//位置上有东西 
				 return FALSE;
			}
		}	
	}
	*/
	//设置物品在容器里面的idx 标示
	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = nIdx; //设置物品在容器里面的idx 标示
		}
	}
    
	return TRUE;
}


void KInventory::DirectSetPosition(int nIdx, int nX, int nY,int nWidth,int nHeight)
{
     //设置物品在容器里面的idx 标示
	int i, j;
	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = nIdx; //设置物品在容器里面的idx 标示
		}
	}
}





//检测物品放进容器时，设置该位置的标示为idx
BOOL KInventory::SetEmptyPlaceItemIdx(int nIdx, int nWidth, int nHeight,POINT *pPos)
{//m_nWidth m_nHeight 为容器的大小
	if (!m_pArray)//容器面积
		return FALSE;
	if (nWidth < 1 || nHeight < 1)
		return FALSE;
	if (nIdx <= 0)
	{
		//_ASSERT(0);
		return FALSE;
	}
	
	int i, j;
	
	if (FindEmptyPlace(nWidth,nHeight,pPos)) //查找空位置
	{
		//设置物品在容器里面的idx 标示
		for (i = pPos->x; i < pPos->x + nWidth; i++)
		{ 
			for (j = pPos->y; j < pPos->y + nHeight; j++)
			{
				m_pArray[j * m_nWidth + i] = nIdx; //设置物品在容器里面的idx 标示
			}
		}
		return TRUE;
	}
	
	return FALSE;
}

#pragma optimize( "", on)

BOOL KInventory::HoldItem(int nIdx, int nWidth, int nHeight)
{//剩余容器空间是否能放下该物品
	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{
			if (PlaceItem(i, j, nIdx, nWidth, nHeight))
				return TRUE;
		}
	}
	return FALSE;
}
//检测idx是否与位置上的标示相同，检测是否能拿起该物品
BOOL	KInventory::PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight)
{
	if (nX < 0 || nY < 0 || nWidth < 1  || nHeight < 1 || nX + nWidth > this->m_nWidth || nY + nHeight > this->m_nHeight)
		return FALSE;

	int	i;
/*	for (i = nX; i < nX + nWidth; i++) //位置加+物品的宽度
	{//检测是否能拿起该物品 保持不能越界
		for (int j = nY; j < nY + nHeight; j++)  //位置+物品的高度
		{
			if (m_pArray[j * m_nWidth + i] != nIdx)  //容器的物品标示 是否相同
			{
				//_ASSERT(0);
				return FALSE;
			}
		}
	}*/
//拿起后设置该位置为空
	for (i = nX; i < nX + nWidth; i++)
	{
		for (int j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = 0;
		}
	}

	return TRUE;
}

//全部设置为空 容器
BOOL KInventory::SetRoomNull()
{
	if ( this->m_nWidth <=0 || this->m_nHeight <=0)
		return FALSE;

	int		i;

//拿起后设置该位置为空
	for (i = 0; i < m_nWidth; i++)
	{
		for (int j = 0; j < m_nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = 0;
		}
	}

	return TRUE;
}


int	KInventory::FindItem(int nX, int nY)
{
	if (!m_pArray)
		return -1;
	if (nX < 0 || nX >= this->m_nWidth || nY < 0 || nY >= this->m_nHeight)
		return -1;

	int	nPos = nY * m_nWidth + nX; //=m_pArray[j * m_nWidth + i]
	int	*pArray = &m_pArray[nPos];  //这个位置上的标示
	if (*pArray <= 0)
		return 0;
	int	nIdx = *pArray;

	//if ((nPos > 0 && *(pArray - 1) == nIdx) || (nPos >= m_nWidth && *(pArray - m_nWidth) == nIdx))
	//	return -1;

	return nIdx;

/*
	for (int i = 0; i < nY * m_nWidth + nX; i++)
	{
		if (m_pArray[i] == nIdx)
			return -1;
	}

	return nIdx;
*/

}

//获取 某个宽度 和 高度空闲的包袱数量
int KInventory::GetFreeRoomCount(int nWidth, int nHeight)
{
	int nCount =0;
	if (nWidth <= 0 || nWidth > m_nWidth || nHeight <= 0 || nHeight > m_nHeight)
		return FALSE;

	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{//历遍容器的宽度
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{//历遍容器的高度
			if (CheckRoom(i,j,nWidth,nHeight))
			{
				nCount ++;
				//return TRUE;
			}
		}
	}

	return nCount;
}

BOOL KInventory::FindRoom(int nWidth, int nHeight, POINT* pPos)
{
	if (!pPos)
		return FALSE;
	if (nWidth <= 0 || nWidth > m_nWidth || nHeight <= 0 || nHeight > m_nHeight)
		return FALSE;

	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{//历遍容器的宽度
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{//历遍容器的高度
			if (CheckRoom(i,j,nWidth,nHeight))
			{
				pPos->x = i;
				pPos->y = j;
				return TRUE;
			}
		}
	}
	pPos->x = 0;
	pPos->y = 0;
	return FALSE;
}
//检测这个位置是否有东西
BOOL KInventory::CheckRoom(int nX, int nY, int nWidth, int nHeight)
{
	if (!m_pArray)
	{
		//_ASSERT(0);
		return FALSE;
	}
//物品最小为 1 格
	if (nX < 0 || nY < 0 || nWidth <=0 || nHeight <=0 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;

	int i, j;
	int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			if (m_pArray[j * m_nWidth + i])//如果有物品 就返回假
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL KInventory::AddmClient(int nMoney)
{
	if (m_nMoney + nMoney < 0)
		return FALSE;
	m_nMoney += nMoney;
	return TRUE;
}

BOOL KInventory::AddxCliet(int ni)
{
	if (m_nXu + ni < 0)
		return FALSE;
	m_nXu += ni;
	return TRUE;
}

int		KInventory::GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY)
{
	if (!m_pArray)
		return 0;

	if  (m_nWidth<=0)
		return 0;

	if (nXpos < 0 || nYpos < 0 || nXpos >= m_nWidth || nYpos >= m_nHeight || !pX || !pY)
		return 0;
	int		nSize = m_nWidth * m_nHeight;
	int		i = nYpos * m_nWidth + nXpos;
	int		*pArray = &m_pArray[i];
	for ( ; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (*pArray == nStartIdx)
			continue;
		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			*pX = i % m_nWidth;
			*pY = i / m_nWidth;
			return *pArray;
		}
	}
	return 0;
}


int		KInventory::GetCurIndexItem(int nCurCount)
{
	if (!m_pArray)
		return 0;

	int		nNum = 0;
	int		nCurIdx = 0;
	int		nSize = m_nWidth * m_nHeight;
	int		*pArray = m_pArray;
	int     nItemCount =0;
	for (int i = 0; i < nSize; i++)
	{
		if (*pArray <= 0)
		{//没有东西
			pArray++;
			continue;
		}

		if (nCurIdx == *pArray)
		{
			pArray++;
			continue;
		}

		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			nItemCount ++;
			nCurIdx = *pArray;
			if  (nItemCount==nCurCount)
				break;
			//if (Item[nCurIdx].GetGenre() == nGenre && Item[nCurIdx].GetDetailType() == nDetail)
			//	nNum += Item[nCurIdx].GetStackNum();
		}
		pArray++;
	}

	return nCurIdx;
}

//返回这类型的 叠加数量
int		KInventory::CalcSameDetailType(int nGenre, int nDetail)
{
	if (!m_pArray)
		return 0;
	int		nNum = 0;
	int		nCurIdx = 0;
	int		nSize = m_nWidth * m_nHeight;
	int		*pArray = m_pArray;
	for (int i = 0; i < nSize; i++)
	{
		if (*pArray <= 0)
		{
			pArray++;
			continue;
		}
		if (nCurIdx == *pArray)
		{
			pArray++;
			continue;
		}
		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			nCurIdx = *pArray;
			if (Item[nCurIdx].GetGenre() == nGenre && Item[nCurIdx].GetDetailType() == nDetail)
				nNum += Item[nCurIdx].GetStackNum();
		}
		pArray++;
	}

	return nNum;
}
//查找空闲空间
BOOL	KInventory::FindEmptyPlace(int nWidth, int nHeight, POINT *pPos)
{
	if (!m_pArray)
		return FALSE;
	if (!pPos || nWidth <= 0 || nHeight <= 0 || nWidth > m_nWidth || nHeight > m_nHeight)
		return FALSE;

	int		i, j, nIdx, a, b, nFind;
	for (i = 0; i < m_nHeight - nHeight + 1; i++)
	{
		for (j = 0; j < m_nWidth - nWidth + 1; )
		{
			nIdx = m_pArray[i * m_nWidth + j];  //如果有物品
			if (nIdx)
			{
				//_ASSERT(Item[nIdx].GetWidth() > 0);
				if (Item[nIdx].GetWidth()>0)
				   j += Item[nIdx].GetWidth();   //跳开有物品的位置
			}
			else
			{
				nFind = 1;
				for (a = i; a < i + nHeight; a++) //高度
				{
					for (b = j; b < j + nWidth; b++)
					{
						if (m_pArray[a * m_nWidth + b]) //如果有东西
						{
							nFind = 0;
							break;
						}
					}
					if (nFind == 0)
						break;
				}
				//根据物品的 宽度和长度检测完毕 有合适就设置位置
				if (nFind)
				{
					pPos->x = b;
					pPos->y = a;
					return TRUE;
				}
				j++;
			}
		}
	}

	return FALSE;
}
//---------------------------------------------------------------------------------
// 功能：输入物品类型和具体类型，察看Inventory里面有没有相同的物品，输出位置和编号
//---------------------------------------------------------------------------------
BOOL	KInventory::FindSameDetailType(int nGenre, int nDetail, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;

	if  (m_nWidth<=0)
		return FALSE;

	if (!pnIdx || !pnX || !pnY)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetGenre() == nGenre && Item[*pArray].GetDetailType() == nDetail)
		{
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	}

	return FALSE;

/*
	int j;

	for (i = 0; i < this->m_nHeight; i++)
	{
		for (j = 0; j < this->m_nWidth; j++)
		{
			if (!m_pArray[i * m_nWidth + j])
				continue;
			if (Item[m_pArray[i * m_nWidth + j]].GetGenre() != nGenre)
				continue;
			if (Item[m_pArray[i * m_nWidth + j]].GetDetailType() != nDetail)
				continue;
			*pnIdx = m_pArray[i * m_nWidth + j];
			*pnX = j;
			*pnY = i;
			return TRUE;
		}
	}

	return FALSE;
*/
}

//---------------------------------------------------------------------------------
// 快捷栏功能：输入物品类型和具体类型，察看Inventory里面有没有相同的物品
//---------------------------------------------------------------------------------
BOOL	KInventory::CheckSameDetailType(int nGenre, int nDetail,int ParticularType)
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetGenre() == nGenre && Item[*pArray].GetDetailType() == nDetail && Item[*pArray].GetParticular()==ParticularType)
			return TRUE;
	}

	return FALSE;
}
