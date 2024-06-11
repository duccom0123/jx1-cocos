#include "KEngine.h"
#include "KLinkArray.h"

KLinkArray::~KLinkArray()
{
	 if (pNodeL) 
	 {
		 delete [] pNodeL;
		 pNodeL = NULL;
	 }
}

void KLinkArray::Init(int nSize)
{
	if (pNodeL)
	{
		delete [] pNodeL;
		pNodeL = NULL;
		nCount = 0;
	}

	pNodeL = new KLinkNode[nSize];
//	if (pNodeL!=NULL)
//	printf(" KLinkNode[%d] OK..!!!!\n",nSize);
}

void KLinkArray::Remove(int nIdx)
{
	if (!nIdx)
		return;
	
	if ((pNodeL[nIdx].nNext == 0 && pNodeL[0].nPrev != nIdx) || (pNodeL[nIdx].nPrev == 0 && pNodeL[0].nNext != nIdx))
	{
		//printf("--[error]Node:%d Remove twice--\n", nIdx);
		return;
	}

	pNodeL[pNodeL[nIdx].nPrev].nNext = pNodeL[nIdx].nNext; //备份下一个的数据
	pNodeL[pNodeL[nIdx].nNext].nPrev = pNodeL[nIdx].nPrev; //备份下一个的数据
	pNodeL[nIdx].nNext = 0;  //当前数据设置为0；
	pNodeL[nIdx].nPrev = 0;
	nCount--;
}

void KLinkArray::Insert(int nIdx)
{
	if (!nIdx)  //>0的
		return;

	if (pNodeL[nIdx].nNext != 0 || pNodeL[nIdx].nPrev != 0)
	{
		//printf("--[error]Node:%d Insert twice--\n", nIdx);
		return;
	}

	pNodeL[nIdx].nNext = pNodeL[0].nNext;  //1=0,2=1
	pNodeL[pNodeL[0].nNext].nPrev = nIdx;
	pNodeL[0].nNext = nIdx;
	pNodeL[nIdx].nPrev = 0;
	nCount++;
}
