//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KRandom.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Random Number Generate Functions
//---------------------------------------------------------------------------
#include "KRandom.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#define IM 139968
#define IA 3877
#define IC 29573
//---------------------------------------------------------------------------
static unsigned int nRandomSeed = 42;  //unsigned long long		UINT
//---------------------------------------------------------------------------
// 函数:	RandomnSeed
// 功能:	设置随机数种子
// 参数:	nRandomSeed	:	随机数种子
// 返回:	void
//---------------------------------------------------------------------------
void g_RandomSeed(unsigned int nSeed)  
{
	nRandomSeed = nSeed;

	//if (nRandomSeed>500000)
	//	nRandomSeed=42; 
}

//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
unsigned int g_Random(unsigned int nMax)
{
	if (nMax)
	{
		nRandomSeed = nRandomSeed * IA + IC;	  //19W

		if ((nRandomSeed%nMax)<0)	//溢出了
		{
			/* 准备生成随机数,以当前时间为种子 */
			srand((unsigned)time(NULL));
			nRandomSeed= rand()%nMax;
			return nRandomSeed%nMax;
		}

		return nRandomSeed%nMax; //取余数				  
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetRandomSeed
// 功能:	取得当时的伪随机种子
// 返回:	返回当前的伪随机种子
//---------------------------------------------------------------------------
unsigned int g_GetRandomSeed()
{
	if  (nRandomSeed<0)
	{
		srand((unsigned)time(NULL));
		nRandomSeed= rand()%100+42;
		return nRandomSeed;
	}

	return nRandomSeed;
}

/*//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
unsigned __int64 g_Random(UINT nMax)
{
	if (nMax)
	{
		//if (nRandomSeed>500000)	 //做个上限判断 20亿
		//    nRandomSeed=42;
		nRandomSeed = nRandomSeed * IA + IC;	  //19W
		return nRandomSeed%nMax; //取余数				  
	}
	else
	{
		return 0;
	}
}
*/