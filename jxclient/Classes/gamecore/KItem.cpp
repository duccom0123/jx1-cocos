#include "KCore.h"
//#include "MyAssert.H"
#include "engine/KTabFile.h"
#include "engine/KRandom.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KNpcTemplate.h"    //后来加的
//#include "../../Engine/src/Text.h"
/*#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif*/


#include "ImgRef.h"
#include "KPlayer.h"
#include "KRUImage.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KMagicDesc.h"
#include <time.h>
/////////////////////////////////
//#include "../../Headers/IClient.h"
#include "CoreShell.h"
////////////////////////////////
//#endif

KItem	*Item = NULL;//Item[MAX_ITEM];

int GetRandomNumber(int nMin, int nMax);
//extern  const KScript * g_GetScript(DWORD dwScriptId);  //原来没有的
KItem::KItem()
{   //内存初始化
	::memset(&m_CommonAttrib,    0, sizeof(m_CommonAttrib));
	::memset(m_aryBaseAttrib,    0, sizeof(m_aryBaseAttrib));
	::memset(m_aryRequireAttrib, 0, sizeof(m_aryRequireAttrib));
	::memset(m_aryMagicAttrib,   0, sizeof(m_aryMagicAttrib));
	::memset(m_ronMagicAttrib,   0, sizeof(m_ronMagicAttrib));       //熔炼
//	::memset(m_BaoShiMagicAttrib,   0, sizeof(m_BaoShiMagicAttrib)); //宝石
	::memset(&m_GeneratorParam,	 0, sizeof(m_GeneratorParam));
    ::memset(&m_TempMagicAttrib,	 0, sizeof(m_TempMagicAttrib));  //(临时储蓄的魔法属性值,用于强化)
	::memset(&m_TempRMagicAttrib,	 0, sizeof(m_TempRMagicAttrib)); //
//	::memset(&m_TempBMagicAttrib,	 0, sizeof(m_TempBMagicAttrib)); 
	::memset(&m_TempPlatinaAttrib,	 0, sizeof(m_TempPlatinaAttrib));
	::memset(&m_yinMagicAttrib,	 0, sizeof(m_yinMagicAttrib));
	
	m_nCurrentDur = -1;

//	nstrNoteInfo.clear();

//#ifndef _SERVER
	::memset(&m_Image,   0, sizeof(KRUImage));
//#endif
	m_nIndex = 0;
}

KItem::~KItem()
{
}

void* KItem::GetRequirement(IN int nReq)
{
	int i = sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);
	if (nReq >= i)
		return NULL;
	return &m_aryRequireAttrib[nReq];
}

BOOL KItem::CheckRequirement(IN int nReq)
{
	int i = sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);
	BOOL nReg=FALSE;
//	m_aryRequireAttrib[nReq];

	for (int k=0;k<i;++k)
	{
		if (m_aryRequireAttrib[k].nAttribType==nReq)
		{
		   nReg=TRUE;
		   break;
		}
	}

	return nReg;
}

/******************************************************************************
功能:	将item上的魔法应用到NPC身上
入口：	pNPC: 指向NPC的指针，nMagicAcive：打开的隐藏属性数目
出口:	魔法被应用。
		具体工作由KNpc的成员函数完成。
		KItem 对象本身没有成员变量被修改
******************************************************************************/
void KItem::ApplyMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive,int *nType) const
{
	//_ASSERT(this != NULL);
	//_ASSERT(nMagicActive >= 0);
     if (!pNPC || nMagicActive<0)
		return;

	int nCount = nMagicActive;
	int i;

	// 基础属性调整NPC
	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); ++i)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
		}
	}
	// 魔法属性调整NPC
	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); ++i)
	{//蓝装 紫装 黄装 白金装
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if (i & 1)						// 为奇数，是后缀（i从零开始）
			{// 1 3 5
				if (nCount >0)
				{
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
					nCount--;
				}
			}
			else
			{// 2 4 6
				    pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
			}
		}
	}

	//for (i = 0; i < sizeof(m_yinMagicAttrib)/sizeof(m_yinMagicAttrib[0]); ++i)//sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0])
	//隐藏的属性
	if (nType[0]==3)
	{//黄金或白金
			const KItemNormalAttrib* pRAttrib;
			 if (nType[1] >=5 && nType[1] < 10)
			 {
			    pRAttrib = &(m_yinMagicAttrib[0]);	//第一个属性
			    if (INVALID_ATTRIB != pRAttrib->nAttribType)
				{  
				   pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
				}  
			 }
			 else  if (nType[1]>=10)
			 {
				 pRAttrib = &(m_yinMagicAttrib[0]);	//第一个属性
				 if (INVALID_ATTRIB != pRAttrib->nAttribType)
				 {  
					 pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
				 }
				 pRAttrib = &(m_yinMagicAttrib[1]);	//第二个属性
				 if (INVALID_ATTRIB != pRAttrib->nAttribType)
				 {  
					 pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
				 } 
			 }
	}
	
 //熔炼辨识
	for (i = 0; i < sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0]); ++i)//sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0])
	{ 
		  const KItemNormalAttrib* pRAttrib;
		  pRAttrib = &(m_ronMagicAttrib[i]);

		  if (nType[0]==1)
		  {//是蓝装
			  // 熔炼属性调整NPC
			  if (nType[1]>-1)
			  {//已经辨识 和 纹钢属性
		          if (INVALID_ATTRIB != pRAttrib->nAttribType)
				  {  
			        pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
				  }  
			  }
		  }
		  else
		  {//其他的直接属性
			  if (INVALID_ATTRIB != pRAttrib->nAttribType)
			  {  
				  pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
			  } 
		  }
	} 
	// 宝石属性调整NPC
/*	for (i = 0; i < sizeof(m_BaoShiMagicAttrib)/sizeof(m_BaoShiMagicAttrib[0]); ++i)//sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0])
	{
		const KItemNormalAttrib* pRAttrib;
		pRAttrib = &(m_BaoShiMagicAttrib[i]);	
		if (INVALID_ATTRIB != pRAttrib->nAttribType)
		{
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)pRAttrib);
		}
	} */
}

/******************************************************************************
功能:	将item上的魔法从NPC身上移除
入口：	pNPC: 指向NPC的指针，nMagicAcive：打开的隐藏属性数目
出口:	魔法被应用。
		具体工作由KNpc的成员函数完成。
		KItem 对象本身没有成员变量被修改
******************************************************************************/
void KItem::RemoveMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive,IN int *nType) const
{
	//_ASSERT(this != NULL);
	//_ASSERT(nMagicActive >= 0);
    if (!pNPC || nMagicActive<0)
    	return;

	int nCount = nMagicActive;
	int	i;
	
	// 基础属性调整NPC
	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); ++i) 
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			KItemNormalAttrib RemoveAttrib;
			RemoveAttrib.nAttribType = pAttrib->nAttribType;
			RemoveAttrib.nValue[0]   = -pAttrib->nValue[0];
			RemoveAttrib.nValue[1]   = -pAttrib->nValue[1];
			RemoveAttrib.nValue[2]   = -pAttrib->nValue[2];
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
		}
	}

	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); ++i)// 魔法属性
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: 为 -1 定义一个常量?
		{
			KItemNormalAttrib RemoveAttrib;
			if (i & 1)						            //为奇数，是后缀（i从零开始）明属性
			{// 1 3 5
				if (nCount >0)
				{
					RemoveAttrib.nAttribType = pAttrib->nAttribType;
					RemoveAttrib.nValue[0]   = -pAttrib->nValue[0];
					RemoveAttrib.nValue[1]   = -pAttrib->nValue[1];
					RemoveAttrib.nValue[2]   = -pAttrib->nValue[2];
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
					nCount--;
				}
			}
			else
			{// 2 4 6
				RemoveAttrib.nAttribType = pAttrib->nAttribType;
				RemoveAttrib.nValue[0]   = -pAttrib->nValue[0];
				RemoveAttrib.nValue[1]   = -pAttrib->nValue[1];
				RemoveAttrib.nValue[2]   = -pAttrib->nValue[2];
				pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
			}
		}
	}

//-----------------黄金 或白金的 隐藏属性--------------------------------
		if  (nType[0]==3 && nType[1]>5 && nType[1]<10)
		{
			const KItemNormalAttrib* pAttrib;
		    pAttrib = &(m_yinMagicAttrib[0]);
		    if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: 为 -1 定义一个常量?
			{   
				KItemNormalAttrib RemoveAttrib;
				RemoveAttrib.nAttribType = pAttrib->nAttribType;
				RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
				RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
				RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
				pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
			} 	 
		}
		else if  (nType[0]==3 && nType[1]>=10)
		{
			for (i = 0; i < sizeof(m_yinMagicAttrib)/sizeof(m_yinMagicAttrib[0]); ++i)//隐藏属性
			{
				const KItemNormalAttrib* pAttrib;
				pAttrib = &(m_yinMagicAttrib[i]);
				if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: 为 -1 定义一个常量?
				{   
					KItemNormalAttrib RemoveAttrib;
					RemoveAttrib.nAttribType = pAttrib->nAttribType;
					RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
					RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
					RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
				} 

			}
		}
//-----------------------------------------------------------------------------------------
	for (i = 0; i < sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0]); ++i)// 熔炼魔法属性
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_ronMagicAttrib[i]);
		
		if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: 为 -1 定义一个常量?
		{
			        KItemNormalAttrib RemoveAttrib;
				    RemoveAttrib.nAttribType = pAttrib->nAttribType;
				    RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
				    RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
				    RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
				    pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
		}
	}

/*	for (i = 0; i < sizeof(m_BaoShiMagicAttrib)/sizeof(m_BaoShiMagicAttrib[0]); ++i)// 宝石魔法属性
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_BaoShiMagicAttrib[i]);
		
		if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: 为 -1 定义一个常量?
		{
			KItemNormalAttrib RemoveAttrib;
			RemoveAttrib.nAttribType = pAttrib->nAttribType;
			RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
			RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
			RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
		}
	} */
}

/******************************************************************************
功能:	将item上的第N项隐藏魔法属性应用到NPC身上
入口：	pNPC: 指向NPC的指针
出口:	魔法被应用。
		具体工作由KNpc的成员函数完成。
		KItem 对象本身没有成员变量被修改   暗属性  未激活的属性
******************************************************************************/
void KItem::ApplyHiddenMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	//_ASSERT(this != NULL);

	if (!pNPC || nMagicActive < 0)
		return;

	const KItemNormalAttrib* pAttrib; //普通属性
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	// 后缀为隐藏属性所以乘2减一
	if (-1 != pAttrib->nAttribType)
	{
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}
}

/******************************************************************************
功能:	将item上的第N项隐藏魔法属性从NPC身上移除
入口：	pNPC: 指向NPC的指针，nMagicActive：第n项魔法属性
出口:	魔法被移除。
		具体工作由KNpc的成员函数完成。
		KItem 对象本身没有成员变量被修改
******************************************************************************/
void KItem::RemoveHiddenMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	//_ASSERT(this != NULL);
	if (!pNPC || nMagicActive <0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	// 后缀为隐藏属性所以乘2减一
	if (-1 != pAttrib->nAttribType)
	{
		KItemNormalAttrib RemoveAttrib;
		RemoveAttrib.nAttribType = pAttrib->nAttribType;
		RemoveAttrib.nValue[0]   = -pAttrib->nValue[0];
		RemoveAttrib.nValue[1]   = -pAttrib->nValue[1];
		RemoveAttrib.nValue[2]   = -pAttrib->nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
	}
}

/******************************************************************************
功能:	根据配置文件中的数据,为item的各项赋初值
入口：	pData: 给出来自配置文件的数据
出口:	成功时返回非零, 以下成员变量被值:
			m_CommonAttrib,m_aryBaseAttrib,m_aryRequireAttrib
		失败时返回零
说明:	CBR: Common,Base,Require
******************************************************************************/
BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT* pData)
{
	//_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;

	if (!pData)
		return bEC;


	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		// 运算符重载
		SetAttrib_Base(pData->m_aryPropBasic); //基本属性  
		SetAttrib_Req(pData->m_aryPropReq);    //需求属性
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD* pData)  //黄金装备的 基本和 需求属性 设置
{
	//_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (!pData)
			return bEC;

	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		                   // 运算符重载
		SetAttrib_Base(pData->m_aryPropBasic); //基本
		SetAttrib_Req(pData->m_aryPropReq);    //需求
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_BCBR(IN const KBASICPROP_EQUIPMENT_PLATINA* pData)
{
	//_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (!pData)
			return bEC;


	if (pData)
	{
		*this = *pData;		// 运算符重载
		SetAttrib_Base(pData->m_aryPropBasic); //基本属性  
		SetAttrib_Req(pData->m_aryPropReq);    //需求属性
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_RCBR(IN const KBASICPROP_EQUIPMENT_GOLD* pData,BOOL nIsDel)
{
	//_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (!pData)
			return bEC;

	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		// 运算符重载
		SetAttrib_Base(pData->m_aryPropBasic); //基本属性  
		SetAttrib_Req(pData->m_aryPropReq,nIsDel);    //需求属性

       // memcpy(pData->m_aryMagicAttribs, bb, sizeof(int) * 6); 
        ///  =bb;
	
      //  SetAttrib_Mof(pData->m_aryMagicAttribs); //魔法属性
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_Base(const KEQCP_BASIC* pBasic)
{
	for (int i = 0;
		 i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); ++i)
	{
		KItemNormalAttrib* pDst;
		const KEQCP_BASIC* pSrc;
		pDst = &(m_aryBaseAttrib[i]);
		pSrc = &(pBasic[i]);
		pDst->nAttribType = pSrc->nType;  //0
		pDst->nValue[0] = ::GetRandomNumber(pSrc->sRange.nMin, pSrc->sRange.nMax);  //就是这个值 
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
		if (pDst->nAttribType == magic_durability_v)
			SetDurability(pDst->nValue[0]);
	}

	if (m_nCurrentDur == 0)	// 说明没有耐久度属性
		m_nCurrentDur = -1;

	return TRUE;
}
//需求属性
BOOL KItem::SetAttrib_Req(const KEQCP_REQ* pReq,BOOL inDel)
{
	for (int i = 0;
	i < sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]); ++i)
	{
		KItemNormalAttrib* pDst;
		pDst = &(m_aryRequireAttrib[i]);
		if (inDel && pReq[i].nType==39)
		{//去掉门派需求
			pDst->nAttribType =0;
			pDst->nValue[0] = 0;
			pDst->nValue[1] = 0;	// RESERVED
			pDst->nValue[2] = 0;	// RESERVED
		}
		else
		{
			pDst->nAttribType = pReq[i].nType;
			pDst->nValue[0] = pReq[i].nPara;
			pDst->nValue[1] = 0;	// RESERVED
			pDst->nValue[2] = 0;	// RESERVED
		}

	}
	return TRUE;
}


//熔炼属性

BOOL KItem::SetAttrib_RON(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
	{ return FALSE;}

	for (int i = 0;i < sizeof(m_ronMagicAttrib)/sizeof(m_ronMagicAttrib[0]); ++i)
	{ 
		
			m_ronMagicAttrib[i] = pMA[i]; //是技能编号  赋值给技能编号数组
			m_TempRMagicAttrib[i] = pMA[i];
			// 永不磨损
			if (m_ronMagicAttrib[i].nAttribType == magic_indestructible_b)
			{
				SetDurability(-1);  
			}
		
	}
	return 1;
}
//宝石属性
BOOL KItem::SetAttrib_Bao(IN const KItemNormalAttrib* pMA)
{
/*	if (NULL == pMA)
	{ _ASSERT(FALSE); return FALSE; }
	for (int i = 0;i < sizeof(m_BaoShiMagicAttrib)/sizeof(m_BaoShiMagicAttrib[0]); ++i)
	{ 
		
		m_BaoShiMagicAttrib[i] = pMA[i]; //是技能编号  赋值给技能编号数组
		m_TempBMagicAttrib[i] = pMA[i];  //备份这个属性的原始值(用于强化)
		// 永不磨损
		if (m_BaoShiMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);  
		}
		
	}  */
	return TRUE;
}


//隐藏属性
BOOL KItem::SetAttrib_Yin(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
	{ return FALSE; }
	for (int i = 0;i < sizeof(m_yinMagicAttrib)/sizeof(m_yinMagicAttrib[0]); ++i)
	{ 
		
		m_yinMagicAttrib[i] = pMA[i];    //是技能编号  赋值给技能编号数组
		//m_TempBMagicAttrib[i] = pMA[i];  //备份这个属性的原始值(用于强化)
		// 永不磨损
		if (m_yinMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);  
		}
		
	}
	return 1;
}

/******************************************************************************
功能:	根据传入的数据, 为item的魔法属性赋初值
入口：	pMA: 给出数据
出口:	成功时返回非零, 以下成员变量被值:
			m_aryMagicAttrib
		失败时返回零
	//	KMagicAttrib* pAttrib = (KMagicAttrib *)pData
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
		{return FALSE; }
    for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); ++i)
//	for (int i = 0; i <6; ++i)
	{
		m_aryMagicAttrib[i]  = pMA[i]; //魔法编号，但是值没有设置
		m_TempMagicAttrib[i] = pMA[i];//备份这个魔法属性的原始值（用于强化加成的）
		// 永不磨损
		if (m_aryMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);   
		}
	}
	return TRUE;
}

/******************************************************************************
功能:	根据传入的数据, 为item的魔法属性赋初值
入口：	pMA: 给出数据
出口:	成功时返回非零, 以下成员变量被值:
			m_aryMagicAttrib
		失败时返回零
******************************************************************************/
BOOL KItem::SetAttrib_MAB(IN const KMACP* pMA)  // 传入魔法属性
{
	if (NULL == pMA)
		{return FALSE; }

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); ++i)
	{
		const KMACP* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);

		pDst->nAttribType = pSrc->nPropKind;
		pDst->nValue[0] =  ::GetRandomNumber(pSrc->aryRange[0].nMin, pSrc->aryRange[0].nMax);
		pDst->nValue[1] =  ::GetRandomNumber(pSrc->aryRange[1].nMin, pSrc->aryRange[1].nMax);
		pDst->nValue[2] =  ::GetRandomNumber(pSrc->aryRange[2].nMin, pSrc->aryRange[2].nMax);
	}
	return TRUE;
}
//传入指定的 魔法黄金属性
BOOL KItem::SetAttrib_MAC(IN const int* pMA)
{
	if (NULL == pMA)
	   {return FALSE; }
	
	KTabFile MagicTab;
	MagicTab.Load(TABFILE_MAGICAGOLD_PATH);
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); ++i)
	{
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
		pDst->nAttribType = nType;
		MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
		pDst->nValue[0] =  ::GetRandomNumber(nLow, nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
		pDst->nValue[1] =  ::GetRandomNumber(nLow, nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
		pDst->nValue[2] =  ::GetRandomNumber(nLow, nHigh);
	}
	MagicTab.Clear();
	return TRUE;
}
//白装
void KItem::operator = (const KBASICPROP_EQUIPMENT& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nParticularTypea= sData.m_nParticularTypea; //类型2
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = 0;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = sData.m_nYingNuma;               //隐藏属性1
//    pCA->nYingNumb       = sData.m_nYingNumb;               //隐藏属性2
	pCA->nRongNum        = sData.m_nRongNum;               //可溶属性数量
    pCA->nWengangPin     = sData.m_nWengangPin;          //可溶纹钢品质
    pCA->nBinfujiazhi    = sData.m_nBinfujiazhi;         //兵富甲值
	pCA->nChiBangRes     =0;              //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
    pCA->nIsKuaiJie      =0;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;     //技能魔法行号
	pCA->nMagicID        =0;     //技能ID
    pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nCanUse         =1;     //是否可以使用
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName, sData.m_szName);
	//::strcpy(pCA->szScript,"");
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");

	/*pCA->szItemName = sData.m_szName;
	pCA->szScript = NULL;
	pCA->szWonName = "系统";*/

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;

	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro; 
#ifndef _SERVER
	pCA->uPrice = 0; 
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	pCA->szImageName = sData.m_szImageName;
	//::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);
	//pCA->szIntro = sData.m_szIntro;
    ///////////////////////////////////////////
    m_Image.Color.Color_b.a = 255;
    m_Image.nFrame = 0;
    m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
    m_Image.nType = ISI_T_SPR;
    //::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
    m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
	  if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
	  {//如果包里没有这个SPR 就设置默认的SPR路径
		 t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
		 t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
	  }
	}*/

#endif
}

void KItem::operator = (const KBASICPROP_MEDMATERIAL& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &(m_CommonAttrib);
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma        = 0;               //隐藏属性1
//    pCA->nYingNumb        = 0; //隐藏属性2
	pCA->nRongNum        = 0;               //可溶属性数量
    pCA->nWengangPin     = 0;            //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;            //兵富甲值
	pCA->nChiBangRes     = 0;            //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =1;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;     //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,	   "");
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");

	/*pCA->szItemName = sData.m_szName;
	pCA->szScript = NULL;
	pCA->szWonName = "系统"; */

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;

	pCA->szImageName = sData.m_szImageName;
	//pCA->szIntro     = sData.m_szIntro;
//	::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);

	// 赋值: 基本属性部分
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_nAttrib1_Type;
	pBA[0].nValue[0]   = sData.m_nAttrib1_Para;
	pBA[1].nAttribType = sData.m_nAttrib2_Type;
	pBA[1].nValue[0]   = sData.m_nAttrib2_Para;
	pBA[2].nAttribType = sData.m_nAttrib3_Type;
	pBA[2].nValue[0]   = sData.m_nAttrib3_Para;
	
	// 赋值: 需求属性部分: 无
	// 赋值: 魔法属性部分: 无

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/
}
//矿石类物品
void KItem::operator = (const KBASICPROP_MINE& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nBigSet		 = sData.m_nDelet;   //是否使用了删除
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = 0;                //隐藏属性1
//    pCA->nYingNumb       = 0;                //隐藏属性2
	pCA->nRongNum        = 0;                //可溶属性数量
    pCA->nWengangPin     = 0;                //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;                //兵富甲值
	pCA->nChiBangRes     = 0;            //翅膀的外观序号
	pCA->nRongpiont      = 0;
	pCA->nIsBang         = 0;
	pCA->nIsKuaiJie      = sData.m_IsKuaiJie; //是否快捷栏
	pCA->nISMagic        = 0;                //是否有属性
	pCA->nSkillType      = 0;                //技能魔法行数
	pCA->nMagicID        = sData.m_Magic[0]; //技能ID
	pCA->nIsUse          = sData.m_IsUse;    //是否立即使用
	pCA->nLianjieFlg     =0;
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,	   sData.m_szScript);
	pCA->szScript = sData.m_szScript;
	::strcpy(pCA->szWonName,"系统");

	/*pCA->szItemName = sData.m_szName;
	pCA->szScript   = sData.m_szScript;
	pCA->szWonName  = "系统";*/

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
    /*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
    pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
//	::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);

	pCA->szImageName = sData.m_szImageName;
	//pCA->szIntro     = sData.m_szIntro;

	// 赋值: 需求属性部分: 无
	// 赋值: 魔法属性部分: 无
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));

	ZeroMemory(m_ronMagicAttrib, sizeof(m_ronMagicAttrib));
//	ZeroMemory(m_BaoShiMagicAttrib, sizeof(m_BaoShiMagicAttrib));
	ZeroMemory(m_yinMagicAttrib, sizeof(m_yinMagicAttrib));
	ZeroMemory(m_TempMagicAttrib, sizeof(m_TempMagicAttrib));
	ZeroMemory(m_TempRMagicAttrib, sizeof(m_TempRMagicAttrib));
//	ZeroMemory(m_TempBMagicAttrib, sizeof(m_TempBMagicAttrib));


	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/

}

//---------------------------------------------------纹钢
void KItem::operator = (const KBASICPROP_FUSION& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
//-----------------------记录着可熔炼的部位-
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 1;
	pCA->nBigSet		 = 0;
	pCA->nISMagic        = 0;
	pCA->nMagicID        = 0;
//-------------------------------------------
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = 0;                  //隐藏属性1
//    pCA->nYingNumb       = 0;                  //隐藏属性2
	pCA->nRongNum        = 0;                  //可溶属性数量
    pCA->nWengangPin     = sData.m_inPin;      //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;     //兵富甲值
	pCA->nIsSell		 = 1;     //是否买卖
	pCA->nIsTrade		 = 1;     //是否交易
	pCA->nIsDrop		 = 0;     //材料类型
	pCA->nRongpiont      = 0;
	pCA->nIsBang         = 0;
	pCA->nIsKuaiJie      = 0;
	pCA->nSkillType      = sData.m_MagIndex; //纹钢的魔法ID在可纹钢列表的行号
	pCA->nIsUse          = 0;
	pCA->nLianjieFlg     = 0;
	pCA->nChiBangRes     = 0;                //翅膀的外观序号
	pCA->nSellModel      = 0;
	::strcpy(pCA->szItemName,sData.m_szName);
	//::strcpy(pCA->szScript,"");
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");
	/*pCA->szItemName = sData.m_szName;
	pCA->szScript   = NULL;
	pCA->szWonName  = "系统";*/


	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0;
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	//::strcpy(pCA->szImageName,sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);
	pCA->szImageName = sData.m_szImageName;
	//pCA->szIntro     = sData.m_szIntro;

	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// 赋值: 需求属性部分: 无
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	// 赋值: 魔法属性部分: 无
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
/*
	pCA->nSet			 = sData.m_Magic[0];
	pCA->nSetId			 = sData.m_Magic[1];
	pCA->nSetNum		 = sData.m_Magic[2];
	pCA->nBigSet		 = sData.m_Magic[3];
	pCA->nISMagic        = sData.m_Magic[4];
	pCA->nMagicID        = sData.m_Magic[5];
*/
    KItemNormalAttrib* pBA = m_aryMagicAttrib;
	pBA[0].nAttribType = 0;
	pBA[0].nValue[0]   = sData.m_Magic[0];
	pBA[1].nAttribType = 0;
	pBA[1].nValue[0]   = sData.m_Magic[1];
	pBA[2].nAttribType = 0;
	pBA[2].nValue[0]   = sData.m_Magic[2];
	pBA[3].nAttribType = 0;
	pBA[3].nValue[0]   = sData.m_Magic[3];
	pBA[4].nAttribType = 0;
	pBA[4].nValue[0]   = sData.m_Magic[4];
	pBA[5].nAttribType = 0;
	pBA[5].nValue[0]   = sData.m_Magic[5];

	ZeroMemory(m_ronMagicAttrib, sizeof(m_ronMagicAttrib));
//	ZeroMemory(m_BaoShiMagicAttrib, sizeof(m_BaoShiMagicAttrib));
	ZeroMemory(m_yinMagicAttrib, sizeof(m_yinMagicAttrib));
	ZeroMemory(m_TempMagicAttrib, sizeof(m_TempMagicAttrib));
	ZeroMemory(m_TempRMagicAttrib, sizeof(m_TempRMagicAttrib));
//	ZeroMemory(m_TempBMagicAttrib, sizeof(m_TempBMagicAttrib));

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/

}

//----------------------------------------------------
void KItem::operator = (const KBASICPROP_QUEST& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = 0;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nSeries		 = -1;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = sData.m_nDelet;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = 0;                  //隐藏属性1
//    pCA->nYingNumb       = 0;                  //隐藏属性2
	pCA->nRongNum        = 0;                  //可溶属性数量
    pCA->nWengangPin     = 0;                  //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;                  //兵富甲值
	pCA->nIsSell		 = sData.m_nIsSell;    //是否买卖
	pCA->nIsTrade		 = sData.m_nIsTrade;   //是否交易
	pCA->nIsDrop		 = sData.m_nIsDrop;    //材料类型
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =sData.m_IsKuaiJie;
	pCA->nSkillType      =sData.m_SkillType;   //纹钢的魔法ID在可纹钢列表的行号
	pCA->nISMagic        =sData.m_ISMagic;     //是否有属性
	pCA->nMagicID        =sData.m_MagicID;     //技能ID
	pCA->nIsUse          =sData.m_IsUse;
	pCA->nLianjieFlg     =0;
	pCA->nChiBangRes     =0;            //翅膀的外观序号
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,	   sData.m_szScript);
	pCA->szScript = sData.m_szScript;
	::strcpy(pCA->szWonName,"系统");

	/*pCA->szItemName=  sData.m_szName;
	pCA->szScript  = sData.m_szScript;
	pCA->szWonName = "系统";*/

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0;
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	//::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);

	pCA->szImageName = sData.m_szImageName;
    //pCA->szIntro     = sData.m_szIntro;
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// 赋值: 需求属性部分: 无
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	// 赋值: 魔法属性部分: 无
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));

	ZeroMemory(m_ronMagicAttrib, sizeof(m_ronMagicAttrib));
//	ZeroMemory(m_BaoShiMagicAttrib, sizeof(m_BaoShiMagicAttrib));
	ZeroMemory(m_yinMagicAttrib, sizeof(m_yinMagicAttrib));
	ZeroMemory(m_TempMagicAttrib, sizeof(m_TempMagicAttrib));
	ZeroMemory(m_TempRMagicAttrib, sizeof(m_TempRMagicAttrib));
//	ZeroMemory(m_TempBMagicAttrib, sizeof(m_TempBMagicAttrib));

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/

}
//土地符
void KItem::operator = (const KBASICPROP_TOWNPORTAL& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = 0;
	pCA->nParticularType = 0;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = 0;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nPriceXu		 = 0;  //金币
	pCA->nLevel			 = 1;
	pCA->nSeries		 = -1;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma        = 0;               //隐藏属性1
//    pCA->nYingNumb        = 0;               //隐藏属性2
	pCA->nRongNum        = 0;               //可溶属性数量
    pCA->nWengangPin     = 0;            //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;            //兵富甲值
	pCA->nChiBangRes     = 0;            //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =1;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;   //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,	   "");
	pCA->szScript = NULL;

	::strcpy(pCA->szWonName,"系统");

	 /*pCA->szItemName = sData.m_szName;
	 pCA->szScript = NULL;
	 pCA->szWonName = "系统";*/

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;
	pCA->uPrice = 0; 
    pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	//::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);
	pCA->szImageName = sData.m_szImageName;
	//pCA->szIntro = sData.m_szIntro;
	// 赋值: 基本属性部分: 无
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// 赋值: 需求属性部分: 无
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	// 赋值: 魔法属性部分: 无
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));

	ZeroMemory(m_ronMagicAttrib, sizeof(m_ronMagicAttrib));
//	ZeroMemory(m_BaoShiMagicAttrib, sizeof(m_BaoShiMagicAttrib));
	ZeroMemory(m_yinMagicAttrib, sizeof(m_yinMagicAttrib));
	ZeroMemory(m_TempMagicAttrib, sizeof(m_TempMagicAttrib));
	ZeroMemory(m_TempRMagicAttrib, sizeof(m_TempRMagicAttrib));
//	ZeroMemory(m_TempBMagicAttrib, sizeof(m_TempBMagicAttrib));

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/

}
//药品
void KItem::operator = (const KBASICPROP_MEDICINE& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nUseMap         = sData.m_UseMap;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma        = 0;               //隐藏属性1
//    pCA->nYingNumb        = 0;               //隐藏属性2
	pCA->nRongNum        = 0;               //可溶属性数量
    pCA->nWengangPin     = 0;            //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;            //兵富甲值
	pCA->nChiBangRes     = 0;            //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =1;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;   //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,sData.m_szName);
	//::strcpy(pCA->szScript,"");
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");
	/*pCA->szItemName	=  sData.m_szName;
	pCA->szItemName	= NULL;
    pCA->szWonName  = "系统";
	*/
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
    pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	//::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);
	pCA->szImageName = sData.m_szImageName;
	//pCA->szIntro = sData.m_szIntro;

	// 赋值: 基本属性部分
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	pBA[0].nValue[0]   = sData.m_aryAttrib[0].nValue;
	pBA[0].nValue[1]   = sData.m_aryAttrib[0].nTime;

	pBA[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	pBA[1].nValue[0]   = sData.m_aryAttrib[1].nValue;
	pBA[1].nValue[1]   = sData.m_aryAttrib[1].nTime;

	pBA[2].nAttribType = sData.m_aryAttrib[2].nAttrib;
	pBA[2].nValue[0]   = sData.m_aryAttrib[2].nValue;
	pBA[2].nValue[1]   = sData.m_aryAttrib[2].nTime;

	pBA[3].nAttribType = sData.m_aryAttrib[3].nAttrib;
	pBA[3].nValue[0]   = sData.m_aryAttrib[3].nValue;
	pBA[3].nValue[1]   = sData.m_aryAttrib[3].nTime;

	pBA[4].nAttribType = sData.m_aryAttrib[4].nAttrib;
	pBA[4].nValue[0]   = sData.m_aryAttrib[4].nValue;
	pBA[4].nValue[1]   = sData.m_aryAttrib[4].nTime;

	pBA[5].nAttribType = sData.m_aryAttrib[5].nAttrib;
	pBA[5].nValue[0]   = sData.m_aryAttrib[5].nValue;
	pBA[5].nValue[1]   = sData.m_aryAttrib[5].nTime;


	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));

	ZeroMemory(m_ronMagicAttrib, sizeof(m_ronMagicAttrib));
//	ZeroMemory(m_BaoShiMagicAttrib, sizeof(m_BaoShiMagicAttrib));
	ZeroMemory(m_yinMagicAttrib, sizeof(m_yinMagicAttrib));
	ZeroMemory(m_TempMagicAttrib, sizeof(m_TempMagicAttrib));
	ZeroMemory(m_TempRMagicAttrib, sizeof(m_TempRMagicAttrib));

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/
}

void KItem::operator = (const KBASICPROP_EQUIPMENT_UNIQUE& sData)
{
	// 赋值: 共同属性部分
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->bStack			 = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = 0;               //隐藏属性1
//    pCA->nYingNumb       = 0;              //隐藏属性2
	pCA->nRongNum        = 0;               //可溶属性数量
    pCA->nWengangPin     = 0;            //可溶纹钢品质
    pCA->nBinfujiazhi    = 0;            //兵富甲值
	pCA->nChiBangRes     = 0;            //翅膀的外观序号
	pCA->nRongpiont      = 0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =0;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;   //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nCanUse         =1;     //是否可以使用
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,"");
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");

	/*pCA->szItemName	=  sData.m_szName;
	pCA->szItemName	= NULL;
    pCA->szWonName  = "系统";*/

	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
	//::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);

	pCA->szImageName = sData.m_szImageName;
    //pCA->szIntro = sData.m_szIntro;

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//::strcpy(m_Image.szImage, pCA->szImageName);
	t_sprintf(m_Image.szImage,pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/
}
//白金
void KItem::operator = (const KBASICPROP_EQUIPMENT_PLATINA& sData) //const 
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;	
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nSet			 = sData.m_nSet;
	pCA->nSetId			 = sData.m_nSetId;  //套装的ID
	pCA->nSetNum		 = sData.m_nSetNum; //
	pCA->nBigSet		 = sData.m_nUpSet;
	pCA->nSixSkill		 = sData.m_nSixSkill;
	pCA->nTenSkill		 = sData.m_nTenSkill;
	pCA->nGoldId		 = 0;              
	pCA->nIsPlatina	     = 1;	         //白金
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;
	pCA->bStack			 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = sData.m_nYingNuma;            //隐藏属性1
//  pCA->nYingNumb       = sData.m_nYingNumb;            //隐藏属性2
	pCA->nRongNum        = sData.m_nRongNum;             //可溶属性数量
    pCA->nWengangPin     = sData.m_nWengangPin;          //可溶纹钢品质
    pCA->nBinfujiazhi    = sData.m_nBinfujiazhi;         //兵富甲值
    pCA->nChiBangRes     = 0;           //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =0;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;     //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nCanUse         =1;     //是否可以使用
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,"");	
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");
	/*pCA->szItemName	=  sData.m_szName;
	pCA->szItemName	= NULL;
    pCA->szWonName  = "系统";
	*/
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
//	::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);
	pCA->szImageName =  sData.m_szImageName;
	//pCA->szIntro	 =  sData.m_szIntro;
	
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	//游戏物品图标显示
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/
}


//黄金基本属性
void KItem::operator = (const KBASICPROP_EQUIPMENT_GOLD& sData) //const 
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;	
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nSet			 = sData.m_nSet;
	pCA->nSetId			 = sData.m_nSetId;  // 套装的ID
	pCA->nSetNum		 = sData.m_nSetNum;
	pCA->nBigSet		 = sData.m_nUpSet;
	pCA->nGoldId		 = sData.m_nId;
	pCA->nIsPlatina	     = 0;	         //白金
	pCA->nSixSkill		 = 0;
	pCA->nTenSkill		 = 0;
	pCA->nUseMap         = 0;
	pCA->nUseKind	     = 0;
	pCA->nRes            = 0;	         //改变的外观
	pCA->bStack			 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsWhere        = 0;
//	pCA->nYingNuma       = sData.m_nYingNuma;              //隐藏属性1
//  pCA->nYingNumb       = sData.m_nYingNumb;              //隐藏属性2
	pCA->nRongNum        = sData.m_nRongNum;               //可溶属性数量
    pCA->nWengangPin     = sData.m_nWengangPin;            //可溶纹钢品质
    pCA->nBinfujiazhi    = sData.m_nBinfujiazhi;         //兵富甲值
    pCA->nChiBangRes     = 0;              //翅膀的外观序号
	pCA->nRongpiont      =0;
	pCA->nIsBang         =0;
	pCA->nIsKuaiJie      =0;
	pCA->nISMagic        =0;     //是否有属性
	pCA->nSkillType      =0;   //技能ID
	pCA->nMagicID        =0;     //技能ID
	pCA->nIsUse          =0;
	pCA->nLianjieFlg     =0;
	pCA->nCanUse         =1;     //是否可以使用
	pCA->nSellModel      =0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	//::strcpy(pCA->szScript,"");	
	pCA->szScript = NULL;
	::strcpy(pCA->szWonName,"系统");
	/*
	pCA->szItemName	=  sData.m_szName;
	pCA->szItemName	= NULL;
    pCA->szWonName  = "系统";
	*/
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->LimitTime.bMin = 0;
	//::strcpy(pCA->ItmeInfo,sData.m_szIntro);
	/*char szIntro[516];
	ZeroMemory(szIntro,sizeof(szIntro));
	t_sprintf(szIntro,"%s", sData.m_szIntro);
	t_sprintf(pCA->ItmeInfo,szIntro);*/
	pCA->ItmeInfo = sData.m_szIntro;

	pCA->uPrice = 0; 
	pCA->uLianjieFlg =0;
	pCA->ncSellModel =0;
//	::strcpy(pCA->szImageName, sData.m_szImageName);
	t_sprintf(pCA->szIntro,sData.m_szIntro);

	pCA->szImageName	=  sData.m_szImageName;
	//pCA->szIntro	=  sData.m_szIntro;

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
	/*KImageParam	Param;
	if (g_pRepresent)
	{
		if (!g_pRepresent->GetImageParam(m_Image.szImage, &Param, ISI_T_SPR))
		{//如果包里没有这个SPR 就设置默认的SPR路径
			t_sprintf(m_Image.szImage,"\\spr\\others\\问号.spr");
			t_sprintf(pCA->szIntro,"提示:客户端资源不足,请更新为最新客户端!");
		}
	}*/
}
//蓝装
BOOL KItem::Gen_Equipment_Unique(const KBASICPROP_EQUIPMENT* pEqu,
								 const KBASICPROP_EQUIPMENT_UNIQUE* pUni)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pEqu != NULL);
	//_ASSERT(pUni != NULL);
	if (NULL == pEqu || NULL == pUni)
		{ return FALSE;}

	// 赋值: 共同属性部分
	*this = *pUni;		// 运算符重载
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bStack  = pEqu->m_bStack;
	pCA->nWidth  = pEqu->m_nWidth;
	pCA->nHeight = pEqu->m_nHeight;

	SetAttrib_Base(pEqu->m_aryPropBasic);		// 赋值: 基本属性部分
	SetAttrib_Req(pUni->m_aryPropReq);			// 赋值: 需求属性部分
	SetAttrib_MAB(pUni->m_aryMagicAttribs);		// 赋值: 魔法属性部分

	return TRUE;
}

void KItem::Remove()
{//索引设置为0
	m_nIndex = 0;
}

void KItem::SetIndex(int i)
{//索引设置为0
	m_nIndex = i;
}


BOOL KItem::SetBaseAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); ++i)
	{
		m_aryBaseAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetRequireAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryRequireAttrib) / sizeof(m_aryRequireAttrib[0]); ++i)
	{
		m_aryRequireAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetMagicAttrib(IN const KItemNormalAttrib* pAttrib)
{
	return SetAttrib_MA(pAttrib);
}

//------------------------------------------------------------------
//	磨损，返回值表示剩余耐久度
//------------------------------------------------------------------
int KItem::Abrade(IN const int nRandRange)
{
	if (m_nCurrentDur == -1 || nRandRange == 0)	// 永不磨损
		return -1;

     if (nRandRange<0)
	 {//小于0的 必定磨损 
	      m_nCurrentDur--;
	      if (m_nCurrentDur == 0)
		  {  
		    return 0;
		  } 
	 } 
     else if (g_Random(nRandRange) == 0)	// nRandRange分之一的概率
	 { 
		m_nCurrentDur--;
		if (m_nCurrentDur == 0)
		{
			return 0;
		}
	 } 
	return m_nCurrentDur;
}

void KItem::SetItemInfo(int nVale)
{  
	char strVal[516]={0};
	t_sprintf(strVal,GetSItmeInfo());

 	if (strrchr(strVal,'.lua'))
	{
		KLuaScript * pScript = NULL;
		char szResult[256];
		ZeroMemory(szResult,sizeof(szResult));
		if (nVale<=0)
             nVale=GetBaseMagic();
		KLuaScript nClentLua;
        pScript = (KLuaScript*)(nClentLua.Load(strVal));

		if (pScript)
		{
			int nSafeIndex = 0;
			nSafeIndex=pScript->SafeCallBegin();
			//执行这个脚本的函数	
		
			pScript->CallFunction("GetItemDescription", 1, "sds",GetName(),nVale,"");
			//	t_sprintf(Npc[Player[nIndex].m_nIndex].m_ItmeInfo,"测试说明C:%s,%s,%d,%s",JView->nCallFun,JView->nCanShuA,JView->nCanShuB,JView->nCanShuC);
			const char * szType = lua_typename(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
			if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
			{//Lua_PushNumber
				int nResult = (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState));
				t_sprintf(szResult, "%d", nResult);  // int 类型
			}
			else if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)  // 字符串类型
			{
				t_sprintf(szResult, (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
				//szResult[strlen(szResult)]='\0';
				//t_sprintf(Npc[Player[nIndex].m_nIndex].m_ItmeInfo,szResult);
			}
			else
				t_sprintf(szResult,"<color=Red>数据错误,请联系GM处理!<color>"); //JView->nCallFun,JView->nCanShuA,JView->nCanShuB,JView->nCanShuC
			//	ParseString2MagicAttrib(nLevel, szSettingNameValue, szResult);  //解析脚本属性	
			nSafeIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nSafeIndex);
		}
		else
		{					  
				 pScript->Exit();
			     t_sprintf(szResult,"数据有误:%s,%s,%d,%s","GetItemDescription",GetName(),nVale,"暂无数据");
			//       printf("[说明]:函数:%s,参数1:%s,参数2:%d,参数3:%s,返回:%s\n",pView->nCallFun,pView->nCanShuA,pView->nCanShuB,pView->nCanShuC,Npc[Player[nIndex].m_nIndex].m_ItmeInfo);
		}
		SetSItmeInfo(szResult);
	}
}
////////描绘显示图标叠放数量///////////////////////////////////////////
void KItem::Paint(int nX, int nY,BOOL bStack,int nzStackNum,int nKind)  //叠放物品的颜色
{
	m_Image.oPosition.nX = nX;
	m_Image.oPosition.nY = nY;

	if (nKind==CGOG_NPCSELLITEM_PAI)
	{//拍卖行
		if (m_CommonAttrib.nWidth==1 && m_CommonAttrib.nHeight>1)
		{//
			m_Image.oPosition.nY -= (m_CommonAttrib.nHeight>>1)*25;
		}
		if (m_CommonAttrib.nWidth>1 && m_CommonAttrib.nHeight==1)
		{//
			m_Image.oPosition.nX -= (m_CommonAttrib.nWidth>>1)*25;
		}
		if (m_CommonAttrib.nWidth>1 && m_CommonAttrib.nHeight>1)
		{//
			m_Image.oPosition.nX -= (m_CommonAttrib.nWidth>>1)*25;
			m_Image.oPosition.nY -= (m_CommonAttrib.nHeight>>1)*25;
		}
	}

	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_BORDER;
	//g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);  //RU_T_IMAGE4 个点坐标
    
	if (nKind!=CGOG_NPCSELLITEM_PAI && m_CommonAttrib.bStack && bStack)
	{//是否绘画叠加数量
		int nNum = m_CommonAttrib.nStackNum+nzStackNum;

		if (nNum >= 1 && nNum < 5000)   //小于10000
		{
			int nFontSize = 14;
			char szNum[16];
			int nLen = t_sprintf(szNum, "%d", nNum);  //数量
			    szNum[15] = 0;
			/*g_pRepresent->OutputText(nFontSize, szNum, KRF_ZERO_END,
				nX + (m_CommonAttrib.nWidth * 27) - nLen * (nFontSize ) / 2,
				nY + (m_CommonAttrib.nHeight) + nFontSize + 1, TGetColor("255,255,0")); //黄色
			*/
		}
	}
}

char SubBuWei[15][24]={"武器","暗器","衣服","戒子","项链","靴子","腰带","帽子","手镯护腕","玉佩香囊","马匹","面具","披风","印鉴","饰品"};

void KItem::GetDesc(char* pszMsg, bool bShowPrice, int nPriceScale, int nActiveAttrib,int nType) //得到装备的描述
{
	char	szColor[item_number][32] = 
	{
		"",
		"<color=White>",
		"",
		"",
		"<color=Yellow>",
	};
	
	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		if (m_CommonAttrib.nGoldId)	// 黄金装备
		{
			if (m_CommonAttrib.nIsPlatina==0)
			    strcpy(szColor[item_equip], "<color=Yellow>");	   //0xffdcdc00	 Yellow
			else
			    strcpy(szColor[item_equip], "<bclr=blue><color=white>");
		}

		else if (m_CommonAttrib.nPoint > 0 && m_CommonAttrib.nPoint<=6 ) //紫色
		{
			strcpy(szColor[item_equip], "<color=Purple>");
		}
		else if (m_aryMagicAttrib[0].nAttribType)	// 蓝色装备
		{
			strcpy(szColor[item_equip], "<color=Blue>");
		}
		else
		{
			strcpy(szColor[item_equip], "<color=White>");
		}
	}
	else
	{
	   strcpy(szColor[m_CommonAttrib.nItemGenre], "<color=White>");
	}

  

	if (m_CommonAttrib.nEnChance)
	{
		if (m_CommonAttrib.nItemGenre == 0)
		{
			strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);

			char sItemName[64]={0};
		   if (nType==1)
		   {
			   if (m_CommonAttrib.nIsPlatina==0)
		          t_sprintf(sItemName,"%s(已装备)<bclr=White><color=blue>+%d<color><bclr>",m_CommonAttrib.szItemName,m_CommonAttrib.nEnChance);//强化           
		   	   else
				  t_sprintf(sItemName,"%s(已装备)+%d",m_CommonAttrib.szItemName,m_CommonAttrib.nEnChance);//强化      
		   }
		   else
		   {   
			   if (m_CommonAttrib.nIsPlatina==0)
		          t_sprintf(sItemName,"%s<bclr=White><color=blue>+%d<color><bclr>",m_CommonAttrib.szItemName,m_CommonAttrib.nEnChance);//强化
		   		else
				  t_sprintf(sItemName,"%s+%d",m_CommonAttrib.szItemName,m_CommonAttrib.nEnChance);//强化

		   }
		   char    TextLevel[128]={0};
			int     LevelItem = m_CommonAttrib.nLevel;

			if(LevelItem > 10)
			{
				if (LevelItem < 100)
				{
					if (LevelItem%10 == 0)
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem/(LevelItem/10));
					}
					else
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem%10);
					}
				}
				else if (LevelItem < 1000)
				{
					if (LevelItem%100 == 0)
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem/(LevelItem/100));
					}
					else
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem%100);
					}
				}
			}
			else
			{
				t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem);
			}
			strcat(pszMsg, TextLevel);

		    if (m_CommonAttrib.nIsPlatina==1)
				strcat(pszMsg, "<color><bclr>\n");
			else
			    strcat(pszMsg, "<color>\n");
		}
		else
		{
			strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);
			strcat(pszMsg, m_CommonAttrib.szItemName);
			if (m_CommonAttrib.nIsPlatina==1)
				strcat(pszMsg, "<color><bclr>\n");
			else
			    strcat(pszMsg, "<color>\n");
		}

	}
    else
	{
		if (m_CommonAttrib.nItemGenre == 0)
		{
		  strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);

		  char sItemName[64]={0};

		if (nType==1)
		   t_sprintf(sItemName,"%s(已装备)",m_CommonAttrib.szItemName);       
		else  
		   t_sprintf(sItemName,"%s",m_CommonAttrib.szItemName);
		

		char    TextLevel[128]={0};
			int        LevelItem = m_CommonAttrib.nLevel;

			if(LevelItem > 10)
			{
				if (LevelItem < 100)
				{
					if (LevelItem%10 == 0)
					{
						
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem/(LevelItem/10));
					}
					else
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem%10);
					}
				}
				else if (LevelItem < 1000)
				{
					if (LevelItem%100 == 0)
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem/(LevelItem/100));
					}
					else
					{
						t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem%100);
					}
				}
			}
			else
			{
				t_sprintf(TextLevel, "%s[等级%d]", sItemName, LevelItem);
			}
			strcat(pszMsg, TextLevel);
			if (m_CommonAttrib.nIsPlatina==1)
				strcat(pszMsg, "<color><bclr>\n");
			else
			    strcat(pszMsg, "<color>\n");
		}
		else
		{//非装备
			strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);
			strcat(pszMsg, m_CommonAttrib.szItemName);  //物品名字

			if (m_CommonAttrib.nLevel>10)
			{
				char nMsg[32]={0};
				t_sprintf(nMsg,"[%d]级",m_CommonAttrib.nLevel);
				strcat(pszMsg, nMsg); 
			}

			if (m_CommonAttrib.nIsPlatina==1)
				strcat(pszMsg, "<color><bclr>\n");
			else
			    strcat(pszMsg, "<color>\n");
		}
   
	}

/*   if  (m_CommonAttrib.nPoint==7)
    strcat(pszMsg, "[<color=green>VIP<color><color=pink>归属者:");
   else
    strcat(pszMsg, "[<color=pink>归属者:");
    strcat(pszMsg,m_CommonAttrib.szWonName);  //所有者
    strcat(pszMsg, "<color>]\n");
*/
    strcat(pszMsg, "<color=green>");

	if (m_CommonAttrib.nIsBang<=0)
       strcat(pszMsg,"该物品还未绑定");  
	else
       strcat(pszMsg,"该物品已永久绑定");

    strcat(pszMsg, "<color>\n");

 
	 if (m_CommonAttrib.nUseMap>0)
	 {  
		 char nMapKey[32]={0},nMapName[64]={0};
		 ZeroStruct(nMapName);
		 t_sprintf(nMapKey,"%d_name",m_CommonAttrib.nUseMap);
		 
		 g_NpcMapDropRate.GetString("List",nMapKey,"暂无数据",nMapName,sizeof(nMapName));

		 if (m_CommonAttrib.nItemGenre == item_equip)
		 {//锁魂
			 strcat(pszMsg, "<color=yellow>该装备已被锁魂：");
		 }
		 else
			 strcat(pszMsg, "<color=yellow>该物品已被锁魂：");

		  if (m_CommonAttrib.nUseKind==0)  //锁魂地图
			     strcat(pszMsg, nMapName);
		  else
		  { 
			  t_sprintf(nMapName,"等级<%d级",m_CommonAttrib.nUseMap);
			  strcat(pszMsg, nMapName);
		  }
		  strcat(pszMsg, "<color>\n");
	 }  
	 
	 if (m_CommonAttrib.nIsSell == 1 && m_CommonAttrib.nItemGenre != item_equip) //可交易
	 {	
		strcat(pszMsg, "<color=Green>该物品可以交易<color>\n");
	 }
	 else if (m_CommonAttrib.nItemGenre != item_equip)
	 {	
		strcat(pszMsg, "<color=Red>该物品不可以交易<color>\n");
	 } 

	 //if (m_CommonAttrib.nUseMap >0 && m_CommonAttrib.nItemGenre == item_medicine) //药品
	 //{	
	 //	 strcat(pszMsg, "<color=Red>该药品限制地图使用<color>\n");
	 //}
     
	int qianghua=m_CommonAttrib.nEnChance;

    if (qianghua>0)
	{
		if (qianghua==1)
			strcat(pszMsg,"强化星级：<bclr=blue><color=White>★<color><bclr>\n");
		else if (qianghua==2)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★<color><bclr>\n");
		else if (qianghua==3)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★<color><bclr>\n");
		else if (qianghua==4)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★<color><bclr>\n");
		else if (qianghua==5)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★<color><bclr>\n");
		else if (qianghua==6)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★★<color><bclr>\n");
		else if (qianghua==7)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★★★<color><bclr>\n");
		else if (qianghua==8)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★★★★<color><bclr>\n");
		else if (qianghua==9)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★★★★★<color><bclr>\n");
		else if (qianghua==10)
            strcat(pszMsg,"强化星级：<bclr=blue><color=White>★★★★★★★★★★<color><bclr>\n");
		else
		{
			char nInfo[64]={0};
            t_sprintf(nInfo,"强化星级：<bclr=blue><color=White>%d ★<color><bclr>\n",qianghua);
		    strcat(pszMsg,nInfo);
		}

	}
	
	if (bShowPrice && nPriceScale > 0)
	{
		/*if (m_CommonAttrib.nPriceXu>0)//(m_CommonAttrib.nItemGenre == item_task || m_CommonAttrib.nItemGenre == item_mine)
		{//金币
			char szPrice[32];
			t_sprintf(szPrice, "价值：%d金币", m_CommonAttrib.nPriceXu);
			strcat(pszMsg, szPrice);
			strcat(pszMsg, "\n");
		}
		else*/
		{
			//	else if (m_PriceInfo.nPriceType==moneyunit_repute
			char szPrice[32]={0},nInfo[4]={0};
			//if (TakeTrader(m_CommonAttrib.nPrice,nPriceScale) < 10000)
			if (m_CommonAttrib.ncSellModel==moneyunit_money || m_CommonAttrib.ncSellModel==moneyunit_moneya)
				t_sprintf(nInfo,"两");
			else if (m_CommonAttrib.ncSellModel==moneyunit_xu)
				t_sprintf(nInfo,"金");
			else 
				t_sprintf(nInfo,"点");

			if (m_CommonAttrib.nPrice/nPriceScale < 10000)
			{//万
			        //t_sprintf(szPrice, "价值：%d两",TakeTrader(m_CommonAttrib.nPrice,nPriceScale));
					t_sprintf(szPrice, "价值：%d%s",m_CommonAttrib.nPrice/nPriceScale,nInfo);
			}
			else
			{
					int vPrice = (m_CommonAttrib.nPrice/nPriceScale)/10000;//TakeTrader(TakeTrader(m_CommonAttrib.nPrice,nPriceScale), 10000);
					int lPrice = (m_CommonAttrib.nPrice/nPriceScale)%10000;//TakeRemainder(TakeTrader(m_CommonAttrib.nPrice, nPriceScale),10000);
					
					if (lPrice == 0)
					{
						t_sprintf(szPrice, "价值：%d万%s", vPrice,nInfo);
					}
					else
					{
						t_sprintf(szPrice, "价值：%d万%d%s", vPrice,lPrice,nInfo);
					}
			}
		strcat(pszMsg, szPrice);
		strcat(pszMsg, "\n");			
		}

	}

 
/*     char  nMagid[6];
        t_sprintf(nMagid,"<color=yellow>%d<color>",m_CommonAttrib.nIsUse);
	    strcat(pszMsg,nMagid); //是否立即使用
        strcat(pszMsg, "\n");
*/

//---------------------------------TASK
	if (m_CommonAttrib.nItemGenre == item_task)
	{	
          char nSkillType[4];
		//	strcat(pszMsg, _itoa(m_CommonAttrib.nSkillType,nSkillType,10));
		  if (m_CommonAttrib.nDetailType == 371)  //熔炼晶石
		  { 
			strcat(pszMsg, "\n");
			char szLevel[32];
			int  mLeve; 
			if (m_CommonAttrib.nLevel>0 && m_CommonAttrib.nLevel<=6) 
				mLeve=m_CommonAttrib.nLevel; 
			else if (m_CommonAttrib.nLevel>6) 
			    mLeve= 6;
			t_sprintf(szLevel, "<color=Blue>(可熔炼:%d个属性)<color>", mLeve);
		    strcat(pszMsg, szLevel);
		  } 
		  else if 	(m_CommonAttrib.nDetailType == 374 || m_CommonAttrib.nDetailType == 375)  //提炼晶石
		  { 
			  strcat(pszMsg, "\n");
			  char szLevel[32];
			  t_sprintf(szLevel, "<color=Blue>(品质等级:%d品)<color>", m_CommonAttrib.nLevel);
		      strcat(pszMsg, szLevel);
		  } 

			strcat(pszMsg, "\n");
//	}

        if (strstr(m_CommonAttrib.szItemName,"玄火炭"))
		{
			/*if (m_CommonAttrib.nLevel==1)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第一属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			}
			else if (m_CommonAttrib.nLevel==2)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第二属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			}
			else if (m_CommonAttrib.nLevel==3)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第三属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			}
			else if (m_CommonAttrib.nLevel==4)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第四属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			}
			else if (m_CommonAttrib.nLevel==5)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第五属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			}
			else if (m_CommonAttrib.nLevel>5)
			{
				strcat(pszMsg, "<color=Yellow>说明:<color><color=Blue>可以提炼第六属性纹钢<color>\n");
				//strcat(pszMsg, "\n");
			} */
			strcat(pszMsg, "<color=Blue>可以提炼所有纹钢<color>\n");
		}

		if (m_CommonAttrib.nSkillType>0)
		{//标示是否给启用的纹钢
			char szIntor[64];
			int szRows,nMin=0;
			KTabFile MagicTab,MagicTabA;			
		    MagicTab.Load(TABFILE_MAGICALEVEL_PATH);
            MagicTab.GetString(m_CommonAttrib.nSkillType+1,"DESC","",szIntor,sizeof(szIntor));
			MagicTab.GetInteger(m_CommonAttrib.nSkillType+1,"MAGATTRLVL_START",1,&szRows);
            MagicTabA.Load(TABFILE_MAGICATTRIB_PATH);
            MagicTabA.GetInteger(szRows+m_CommonAttrib.nLevel,6,1,&nMin);
         /* if (strstr(m_CommonAttrib.szItemName,"纹钢"))
		  {
            strcat(pszMsg, "<color=Yellow>纹钢属性:<color><color=Cyan>");
			strcat(pszMsg, szIntor);
			strcat(pszMsg, "<color>\n");
			MagicTab.GetString(m_CommonAttrib.nSkillType+1,"FIT_EQUIP","",szIntor,sizeof(szIntor));
			strcat(pszMsg, "<color=Yellow>纹钢位置:<color><color=Cyan>");
			strcat(pszMsg, szIntor);
		    strcat(pszMsg, "<color>\n");
			char szLevel[32]; 		
			t_sprintf(szLevel, "<color=Blue>品质等级:%d品(增加:%d点)<color>", m_CommonAttrib.nLevel,nMin);
			strcat(pszMsg, szLevel);
		  }
		  else*/
		 if (strstr(m_CommonAttrib.szItemName,"宝石"))
		  {
			  strcat(pszMsg, "<color=Yellow>灵合属性:<color><color=Cyan>");
			  strcat(pszMsg, szIntor);
			  strcat(pszMsg, "<color>\n");
			  MagicTab.GetString(m_CommonAttrib.nSkillType+1,"FIT_EQUIP","",szIntor,sizeof(szIntor));
			  strcat(pszMsg, "<color=Yellow>灵合位置:<color><color=Cyan>");
			  strcat(pszMsg, szIntor);
			  strcat(pszMsg, "<color>\n");
			  char szLevel[64]={0}; 		
			  t_sprintf(szLevel, "<color=Blue>品质等级:%d品(增加:%d点)<color>", m_CommonAttrib.nLevel,nMin);
			  strcat(pszMsg, szLevel);
		  }

			MagicTabA.Clear();
			MagicTab.Clear();
		    strcat(pszMsg, "\n");
        //  strcat(pszMsg,)
		}  
		
	}

//---------------------------------装备 和 MINE 	
	if (m_CommonAttrib.nItemGenre == item_equip ||
		(m_CommonAttrib.nItemGenre == item_mine &&
		(m_CommonAttrib.nParticularType == 150 ||
		m_CommonAttrib.nParticularType == 152 ||
		m_CommonAttrib.nParticularType == 154 ||
		(m_CommonAttrib.nParticularType >= 200 && 
		m_CommonAttrib.nParticularType <= 205)) //装备或 孔雀原石，孔雀石 ，芙蓉石
		))
	{
		switch(m_CommonAttrib.nSeries)
		{
		case series_metal:
			strcat(pszMsg, "<color=White>五行属性：<color><color=Metal>金<color>");
			break;
		case series_wood:
			strcat(pszMsg, "<color=White>五行属性：<color><color=Wood>木<color>");
			break;
		case series_water:
			strcat(pszMsg, "<color=White>五行属性：<color><color=Water>水<color>");
			break;
		case series_fire:
			strcat(pszMsg, "<color=White>五行属性：<color><color=Fire>火<color>");
			break;
		case series_earth:
			strcat(pszMsg, "<color=White>五行属性：<color><color=Earth>土<color>");
			break;
		}
			strcat(pszMsg, "\n");
	}
//----------------------------------其他----
	/*if (strstr(m_CommonAttrib.szItemName,"任务卷轴"))
	{
		strcat(pszMsg, "<color=Orange>任务描述：<color>\n");
	} */
	strcat(pszMsg, m_CommonAttrib.szIntro);  //说明文字  物品的说明文字
	strcat(pszMsg, "\n"); //<color>
  
	/*if (strstr(m_CommonAttrib.szItemName,"任务卷轴"))
	{
		strcat(pszMsg, "<color=yellow>提示：右键使用可接任务<color>\n");
	} */
//---------------------------------------------纹纲
    if (m_CommonAttrib.nItemGenre == item_fusion) //如果是纹纲
	{
		    char szIntor[64];
		    int szRows,nMin=0;
		    KTabFile MagicTab;
						
		    MagicTab.Load(TABFILE_MAGICAGOLD_PATH);
            MagicTab.GetString(m_CommonAttrib.nSkillType,1,"暂无数据",szIntor,sizeof(szIntor));
            MagicTab.GetInteger(m_CommonAttrib.nSkillType,6,1,&nMin);   //魔法的索引

            strcat(pszMsg, "<color=Yellow>纹钢属性：<color><color=Cyan>");
			strcat(pszMsg, szIntor);
			strcat(pszMsg, "\n");
			
			if (strstr(m_CommonAttrib.szItemName,"专属"))
			    t_sprintf(szIntor, "<color=Blue>品质等级：%d品(增加:%d%s)<color>\n", m_CommonAttrib.nWengangPin,nMin,"%%");
			else
		        t_sprintf(szIntor, "<color=Blue>品质等级：%d品(增加:%d点)<color>\n", m_CommonAttrib.nWengangPin,nMin);

		    strcat(pszMsg, szIntor);

            MagicTab.Clear();

	  for (int i = 0; i < 6; ++i)  //蓝装，黄装，紫装
	  {
		if (m_aryMagicAttrib[i].nValue[0]>-1) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
		{
             t_sprintf(szIntor, "<bclr=blue><color=White>可纹纲的部位：%s<color><bclr>",SubBuWei[m_aryMagicAttrib[i].nValue[0]]);
		     strcat(pszMsg, szIntor);
			 strcat(pszMsg, "\n");
		}
	  }
	}
//----------------------------------------------------MINE
	if (m_CommonAttrib.nItemGenre == item_mine && 
		m_CommonAttrib.nParticularType >= 200 && 
		m_CommonAttrib.nParticularType <= 205)  //玄铁矿----钟乳石
	{
		char szIntor[64];
		
		KTabFile MagicTab;
		
		MagicTab.Load(TABFILE_MAGICALEVEL_PATH);

		char szTmp[8];
		int nskillidx,nRows;
		//t_sprintf(szTmp,"%d",m_aryBaseAttrib[0].nValue[0]); //只能是第一个？ 行数 为 自身的幸运值
		nRows=MagicTab.GetHeight();
        nskillidx=m_aryBaseAttrib[0].nValue[0];
      if (nskillidx>0)
        for (int k=0;k<nRows;++k)
		{
			int nDestidx,nMagcType;
           MagicTab.GetInteger(k+1,"MAGIC_ID",0,&nDestidx);
		   MagicTab.GetInteger(k+1,"MAGIC_TTPE",0,&nMagcType);
		   if (nDestidx==nskillidx && nMagcType==0)
		   {
			   MagicTab.GetString(k+1,"DESC","",szIntor,64);
			   strcat(pszMsg, "<color=Yellow>镶嵌属性: <color><color=Cyan>");
			   strcat(pszMsg, szIntor);
			   strcat(pszMsg, "<color>\n");
			   MagicTab.GetString(k+1,"FIT_EQUIP","",szIntor,64);
			   strcat(pszMsg, "<color=Yellow>镶嵌位置: <color><color=Cyan>");
			   strcat(pszMsg, szIntor);
		       strcat(pszMsg, "<color>\n");
			   break;
		   }

		}

		MagicTab.Clear();
		/*
		MagicTab.GetString(szTmp,"DESC","",szIntor,64);
		strcat(pszMsg, "<color=Yellow>镶嵌属性说明: <color><color=Cyan>");
		strcat(pszMsg, szIntor);
		strcat(pszMsg, "<color>\n");
		MagicTab.GetString(szTmp,"FIT_EQUIP","",szIntor,64);
		strcat(pszMsg, "<color=Yellow>可以镶嵌的部件位置: <color><color=Cyan>");
		strcat(pszMsg, szIntor);
		strcat(pszMsg, "<color>\n");*/
	//	strcat(pszMsg, "\n");
	}

	if (m_CommonAttrib.nItemGenre == item_mine && (m_CommonAttrib.nParticularType == 147 ||
		(m_CommonAttrib.nParticularType >= 200 && m_CommonAttrib.nParticularType <= 205) || 
		(m_CommonAttrib.nParticularType >= 149 && m_CommonAttrib.nParticularType <= 154)
		))
	{
		char szLevel[32];
		t_sprintf(szLevel, "<color=Blue>品质等级: %d<color>", m_CommonAttrib.nLevel);
		strcat(pszMsg, szLevel);
		strcat(pszMsg, "\n");	
	}
   
	int i;
	for ( i = 0; i < 7; i++)  //基本属性
	{
		if (!m_aryBaseAttrib[i].nAttribType)
		{
			continue;
		}

		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			char	szDurInfo[64]={0};
			if (m_nCurrentDur == -1)
			{	
				   t_sprintf(szDurInfo, "<color=Yellow>永不磨损<color>\n");
			}
			else
			{
				if  (GetDurability()<=3 && GetDurability()>0)
				{
					if (GetGenre()==item_equip && GetDetailType()==equip_mask)
						t_sprintf(szDurInfo, "<color=blue>剩余使用次数：%d/%d<color>\n", GetDurability(), GetMaxDurability());
					else
                        t_sprintf(szDurInfo, "<color=red>耐久度(需要修理)：%d/%d<color>\n", GetDurability(), GetMaxDurability());
				}
				else 
				{
					if (GetGenre()==item_equip && GetDetailType()==equip_mask)
						t_sprintf(szDurInfo, "<color=blue>剩余使用次数：%d/%d<color>\n", GetDurability(), GetMaxDurability());
					else
				        t_sprintf(szDurInfo, "<color=White>耐久度：%d/%d<color>\n", GetDurability(), GetMaxDurability());
				}
			}
			strcat(pszMsg, szDurInfo);
		}
		else
		{//其他属性
		 
			char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryBaseAttrib[i]);
			if (!pszInfo || !pszInfo[0])
				continue;
            strcat(pszMsg, "<color=White>");
			strcat(pszMsg, pszInfo);
			strcat(pszMsg, "<color>\n");
		}
	}
	////白装需求属性nAttribType
	for (i = 0; i < 6; ++i) 
	{
		if (!m_aryRequireAttrib[i].nAttribType)
		{
			continue;
		}
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryRequireAttrib[i]);
		if (!pszInfo || !pszInfo[0])
			continue;
		if (Player[CLIENT_PLAYER_INDEX].m_ItemList.EnoughAttrib(&m_aryRequireAttrib[i]))
		{
			strcat(pszMsg, "<color=White>");
			strcat(pszMsg, pszInfo);
			strcat(pszMsg, "<color>\n");
		}
		else
		{
			strcat(pszMsg, "<color=Red>");
			strcat(pszMsg, pszInfo);
			strcat(pszMsg, "<color>\n");
		}		
	}

	for (i = 0; i < 6; ++i)  //蓝装，黄装，紫装
	{
		if (m_CommonAttrib.nItemGenre != item_equip)
			break;

		if (!m_aryMagicAttrib[i].nAttribType) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
		{
			if (IsPurple() && i< m_CommonAttrib.nPoint)  //打孔了
			{//紫装
				strcat(pszMsg, "<color=GYellow>未镶嵌<color=white>\n");
			}
		//	else if (IsBlue())
		//	{
		//		  strcat(pszMsg, "<color=Yellow>可辨识属性:<color>");	
		//		  strcat(pszMsg,"<color=DYellow>未辨识<color=white>");
		//		  strcat(pszMsg, "\n");	
		//	}
			continue;
		}
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryMagicAttrib[i]); 
		if (!pszInfo||!pszInfo[0])
			continue;
		//char pPin[16]; 
		//int smLevel=CheckBluePin(m_aryMagicAttrib[i].nAttribType,m_aryMagicAttrib[i].nValue[0]); //获取蓝装品质
		if ((i & 1) == 0) //i=1 默认是激活 奇数判断
		{//1 3 5 奇数
			if (m_CommonAttrib.nGoldId > 0)  //黄金装
			{
				strcat(pszMsg, "<color=HYellow>");
			    strcat(pszMsg, pszInfo);
                strcat(pszMsg, "<color>");
			}
			else if (IsPurple())   //紫装
			{
				strcat(pszMsg, "<color=Purple>");
			    strcat(pszMsg, pszInfo);
                strcat(pszMsg, "<color>");
			}
			else
			{//蓝装
				  strcat(pszMsg, "<color=HBlue>");  //蓝装
			      strcat(pszMsg, pszInfo);
                  strcat(pszMsg, "<color>");
			}
		}
		else
		{//2 4 6 偶数
			if ((i>>1) < nActiveAttrib) // 如果是被激活的
			{
				if (m_CommonAttrib.nGoldId > 0)
				{
					strcat(pszMsg, "<color=HYellow>");
				    strcat(pszMsg, pszInfo);
                    strcat(pszMsg, "<color>");
				}
				else if (IsPurple()) 
				{
					strcat(pszMsg, "<color=Purple>");
				    strcat(pszMsg, pszInfo);
                    strcat(pszMsg, "<color>");
				}
				else
				{
					strcat(pszMsg, "<color=HBlue>");
				    strcat(pszMsg, pszInfo);
                    strcat(pszMsg, "<color>");
				}
			}
			else
			{//没有被激活
				if (m_CommonAttrib.nGoldId > 0)
				{
					strcat(pszMsg, "<color=DYellow>"); //  没有被激活的
				    strcat(pszMsg, pszInfo);
                    strcat(pszMsg, "<color>");
				}
				else
				{
					strcat(pszMsg, "<color=DBlue>");
				    strcat(pszMsg, pszInfo);
                    strcat(pszMsg, "<color>");
				}
			}
		}
    //蓝 黄 紫色 基本魔法属性显示
	    strcat(pszMsg,"\n");
	} //end for
	
	//strcat(pszMsg,"\n");
//-----------------两个隐藏的属性---------------------------
	for (i = 0; i < 2; ++i)  //蓝装，黄装，紫装
	{
		if (m_CommonAttrib.nItemGenre != item_equip)
			break;

		if (!m_yinMagicAttrib[i].nAttribType)
           continue;
		
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_yinMagicAttrib[i]); 
		if (!pszInfo||!pszInfo[0])
		   continue;

		if (m_CommonAttrib.nGoldId > 0)
		{
		   if (m_CommonAttrib.nLevel>= 5 && m_CommonAttrib.nLevel<10)
		   {//第一属性
			 if(i==0)
			 {
		       strcat(pszMsg, "<color=HYellow>"); //  激活的
		       strcat(pszMsg, pszInfo);
		       strcat(pszMsg, "<color>");
		       strcat(pszMsg,"\n");
			 }
			 if(i==1)
			 {
				 strcat(pszMsg, "<color=DYellow>");
				 strcat(pszMsg, pszInfo);
				 strcat(pszMsg, "<color>");
				 strcat(pszMsg,"\n");
			 }
		   }
		   else if (m_CommonAttrib.nLevel>=10)
		   {//都激活 
			   strcat(pszMsg, "<color=HYellow>"); //  激活的
			   strcat(pszMsg, pszInfo);
			   strcat(pszMsg, "<color>");
			   strcat(pszMsg,"\n");
		   }
		   else
		   {
			   strcat(pszMsg, "<color=DYellow>");
			   strcat(pszMsg, pszInfo);
			   strcat(pszMsg, "<color>");
			   strcat(pszMsg,"\n");
		   }
		}
	}

//==================显示黄金熔炼属性=========================
	int RongID;
	char* pszInfor;
          RongID=m_CommonAttrib.nRongNum;

 if (RongID>0 && m_CommonAttrib.nRongpiont>0 && (m_CommonAttrib.nGoldId>0 || IsPurple()))
 {  
   for (i = 0; i < m_CommonAttrib.nRongpiont; ++i)
   {  
	 if (!m_ronMagicAttrib[i].nAttribType) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
	 {            
		 strcat(pszMsg,"<bclr=Blue><color=White>[未纹钢]<color><bclr>\n");
	     continue;
	 }
	 pszInfor = (char *)g_MagicDesc.GetDesc(&m_ronMagicAttrib[i]);
	 if (!pszInfor || !pszInfor[0])
		continue;
	//char pPin[32];
	//t_sprintf(pPin,"<color=pink>%d<color>",m_CommonAttrib.nRongpiont);
	//strcat(pszMsg,pPin);
	///int smLevel=CheckWenPin(m_ronMagicAttrib[i].nAttribType,m_ronMagicAttrib[i].nValue[0]); //获取品质
	//char pPin[32]; 
	// t_sprintf(pPin,"<color=pink>%d品<color>",smLevel);
    //strcat(pszMsg,pPin);
   	strcat(pszMsg, "<bclr=Blue><color=White>");
	strcat(pszMsg, pszInfor); //熔炼属性 显示
	strcat(pszMsg, "<color><bclr>\n");
   } 	
 }  
//=================显示熔炼属性结束======================
//==================显示蓝装属性=========================
char* szBlueInfo=NULL;

 if (RongID>0 && m_CommonAttrib.nRongpiont>0 && IsBlue())
 { 
	 char pPin[32]={0};

	 if (m_CommonAttrib.nIsWhere==-1)
	 {  
		 t_sprintf(pPin,"<color=yellow>该装备可以洗练出随机特殊属性");
		 strcat(pszMsg,pPin);
		 strcat(pszMsg, "<color>\n");
	 }

	 if (m_CommonAttrib.nIsWhere==-2)
	 {
		 t_sprintf(pPin,"<color=yellow>该装备洗练属性不可见");
		 strcat(pszMsg,pPin);
		 strcat(pszMsg, "<color>\n");
	 }

    if (m_CommonAttrib.nIsWhere==0)
	{ //熔炼
    
	 for (i = 0; i < m_CommonAttrib.nRongpiont; ++i)
	 {  
	   if (!m_ronMagicAttrib[i].nAttribType) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
	   {            
		   strcat(pszMsg,"<bclr=Blue><color=White>[未纹钢]<color><bclr>\n");
		   continue;
	   }
	   szBlueInfo = (char *)g_MagicDesc.GetDesc(&m_ronMagicAttrib[i]);
	   if (!szBlueInfo || !szBlueInfo[0])
		   continue;
	   //int smLevel=CheckWenPin(m_ronMagicAttrib[i].nAttribType,m_ronMagicAttrib[i].nValue[0]); //获取品质
	   //char pPin[32]; 
	   //t_sprintf(pPin,"<color=Pink>%d品<color>",smLevel);
	   //strcat(pszMsg,pPin);
	   strcat(pszMsg, "<bclr=Blue><color=White>");
	   strcat(pszMsg, szBlueInfo); //熔炼属性 显示
	   strcat(pszMsg, "<color><bclr>\n");
	 }
   }
   else if (m_CommonAttrib.nIsWhere>0)
   {//辨识属性
	 for (i = 0; i < m_CommonAttrib.nRongpiont; ++i)
	 {   
		if (!m_ronMagicAttrib[i].nAttribType) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
		{            
			strcat(pszMsg,"<bclr=Blue><color=White>[未洗练]<color><bclr>\n");
			continue;
		}
		szBlueInfo = (char *)g_MagicDesc.GetDesc(&m_ronMagicAttrib[i]); //BaoShiGetDesc
		if (!szBlueInfo || !szBlueInfo[0])
			continue;
		//int smLevel=CheckWenPin(m_ronMagicAttrib[i].nAttribType,m_ronMagicAttrib[i].nValue[0]); //获取品质
		//char pPin[32]; 
		//t_sprintf(pPin,"<color=Cyan>%d品",smLevel);
		//strcat(pszMsg,pPin);
		strcat(pszMsg, "<color=Cyan>");
		strcat(pszMsg, szBlueInfo); //辨识属性 显示
		strcat(pszMsg, "<color>\n");
	 }  
   }
 }  
//=================显示蓝装辨识属性结束

//=================显示宝石属性
/* if  (m_CommonAttrib.nItemGenre==item_equip && m_CommonAttrib.nGoldId>0)
 { 
	 if (GetBaoShiCount())
	    strcat(pszMsg, "<color=green>宝石灵合:<color>\n");
	 //else
     //   strcat(pszMsg, "<color=green>宝石灵合:<color>暂无灵合\n");
 
 //if (m_CommonAttrib.nGoldId > 0 && (m_CommonAttrib.nDetailType==equip_meleeweapon || m_CommonAttrib.nDetailType==equip_rangeweapon))
 //只能是武器显示
	for (i = 0; i < 6; ++i)
	{  
		if (!m_BaoShiMagicAttrib[i].nAttribType) //这个属性位置的数组中没有数据的 // nAttribType属性类型编码（int数字）
		{            
			//if (m_CommonAttrib.nGoldId>0 && m_CommonAttrib.nRongpiont>0)  //打孔了
				//strcat(pszMsg,"<color=yellow>[未镶嵌宝石]<color>");
				//strcat(pszMsg, "\n");
			continue;
		}
		pszInfor = (char *)g_MagicDesc.BaoShiGetDesc(&m_BaoShiMagicAttrib[i]);
		if (!pszInfor || !pszInfor[0])
			continue;
		//int smLevel=CheckWenPin(m_BaoShiMagicAttrib[i].nAttribType,m_BaoShiMagicAttrib[i].nValue[0]); //获取品质
		//char pPin[32]; 
		//t_sprintf(pPin,"<color=pink>%d品<color>",smLevel);
		//strcat(pszMsg,pPin);
		strcat(pszMsg, "<bclr=red><color=yellow>");
		strcat(pszMsg, pszInfor); //宝石属性 显示
		strcat(pszMsg, "<color><bclr>\n");
	} 
 } 	*/
///////////////////显示宝石属性结束//////////////////////////
/*//////////////////隐藏属性显示/////////////////////////////
	int YingIDA,YingIDB;
	char AszInfoy[16], BszInfoy[16];
    YingIDA=m_CommonAttrib.nYingNuma;
	YingIDB=m_CommonAttrib.nYingNumb;
 if (YingIDA>0)
 {
        strcat(pszMsg, "<color=White>");
	    t_sprintf(AszInfoy,"[隐藏属性1ID:%d]",YingIDA);
	    strcat(pszMsg, AszInfoy); //魔法属性1 显示
	    strcat(pszMsg, "<color>\n");
 }
 if (YingIDB>0)
 {      strcat(pszMsg, "<color=White>");
	    t_sprintf(BszInfoy,"[隐藏属性2ID:%d]",YingIDB);
		strcat(pszMsg, BszInfoy); //魔法属性2 显示
		strcat(pszMsg, "<color>\n");
 }
*///////////////////隐藏属性结束/////////////////////////////
	if (m_CommonAttrib.LimitTime.bYear>0) // 时间限制装备
	{ 
		time_t rawtime;
		struct tm * timeinfo;		
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		//timeinfo->tm_year
		char sTmp[64];
		if (m_CommonAttrib.nIsWhere>0 && m_CommonAttrib.nCanUse==0)
		    t_sprintf(sTmp,"<color=Red>已到期:%d年%d月%d日%d时%d分<color>\n",m_CommonAttrib.LimitTime.bYear,m_CommonAttrib.LimitTime.bMonth,m_CommonAttrib.LimitTime.bDay,m_CommonAttrib.LimitTime.bHour,m_CommonAttrib.LimitTime.bMin);
		else if (m_CommonAttrib.nCanUse==0)
		{
			t_sprintf(sTmp,"<color=Green>使用期限至:%d年%d月%d日%d时%d分<color>\n",m_CommonAttrib.LimitTime.bYear,m_CommonAttrib.LimitTime.bMonth,m_CommonAttrib.LimitTime.bDay,m_CommonAttrib.LimitTime.bHour,m_CommonAttrib.LimitTime.bMin);
		    strcat(sTmp,"<color=red>不可装备<color>\n");
		}
		else
		    t_sprintf(sTmp,"<color=Green>使用期限至:%d年%d月%d日%d时%d分<color>\n",m_CommonAttrib.LimitTime.bYear,m_CommonAttrib.LimitTime.bMonth,m_CommonAttrib.LimitTime.bDay,m_CommonAttrib.LimitTime.bHour,m_CommonAttrib.LimitTime.bMin);

		strcat(pszMsg,sTmp);
/*        int mYear = m_CommonAttrib.LimitTime.bYear-1900-timeinfo->tm_year;
		int mMon  = m_CommonAttrib.LimitTime.bMonth-1-timeinfo->tm_mon;
		int mDay  = m_CommonAttrib.LimitTime.bDay-timeinfo->tm_mday;
		int mHour = m_CommonAttrib.LimitTime.bHour-timeinfo->tm_hour;
		int mMin  = m_CommonAttrib.LimitTime.bMin-timeinfo->tm_min;
        t_sprintf(sTmp,"<color=Green>剩余时间:");
        strcat(pszMsg, sTmp);
        if (mYear>0)
		{
           t_sprintf(sTmp,"%d年",mYear);
		   strcat(pszMsg, sTmp);
		}
		if (mMon>0)
		{
		   t_sprintf(sTmp,"%d月",mMon);
		   strcat(pszMsg, sTmp);
		}
		if (mDay>0)
		{
			t_sprintf(sTmp,"%d日",mDay);
			strcat(pszMsg, sTmp);
		} 

		if (mHour>0)
		{
			t_sprintf(sTmp,"%d小时",mHour);
			strcat(pszMsg, sTmp);
		}

        if (mMin>0)
		{
			t_sprintf(sTmp,"%d分",mMin);
			strcat(pszMsg, sTmp);
		}
*/
		strcat(pszMsg, " \n");
	}
	
    int IsShowThis=1;
	    g_GameSetTing.GetInteger("SYSTEM","IsShowDesc",1,&IsShowThis);   //是否显示

	if (nType==0 && (m_CommonAttrib.nColor==2 || m_aryMagicAttrib[0].nAttribType) && IsShowThis) //装备类型
	{	
		strcat(pszMsg, " \n");
        switch(m_CommonAttrib.nDetailType)
        {
        case equip_meleeweapon:  //武器
            {
             switch(m_CommonAttrib.nSeries)
                {
                  case series_metal:
					  { 
			            strcat(pszMsg, "<color=Metal>");
			            strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>衣服<color>和<color=Earth>项链<color>激活暗属性<color>\n");
					  }  
			      break;
	         	  case series_wood:
					  {
			            strcat(pszMsg, "<color=Wood>");
			            strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>衣服<color>和<color=Water>项链<color>激活暗属性<color>\n");
					  }
			      break;
		          case series_water:
					  {
			            strcat(pszMsg, "<color=Water>");
		                strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>衣服<color>和<color=Metal>项链<color>激活暗属性<color>\n");
					  } 
			      break;
	        	  case series_fire:
					  {
			            strcat(pszMsg, "<color=Fire>");
			            strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>衣服<color>和<color=Wood>项链<color>激活暗属性<color>\n");
					  }
			      break;
	        	  case series_earth:
					  {  
			            strcat(pszMsg, "<color=Earth>");
			            strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>衣服<color>和<color=Fire>项链<color>激活暗属性<color>\n");
					  }  
			      break;
			 }
             break;
            }
        case equip_rangeweapon:  // 暗器
            {
                 switch(m_CommonAttrib.nSeries)
				 {  
				    case series_metal:
						{   
						   strcat(pszMsg, "<color=Metal>");
						   strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>衣服<color>和<color=Earth>项链<color>激活暗属性<color>\n");
						}  
					break;
				   case series_wood:
					   {  
						   strcat(pszMsg, "<color=Wood>");
						   strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>衣服<color>和<color=Water>项链<color>激活暗属性<color>\n");
					   }  
					break;
				   case series_water:
					   { 
						   strcat(pszMsg, "<color=Water>");
						   strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>衣服<color>和<color=Metal>项链<color>激活暗属性<color>\n");
					   }  
					break;
				   case series_fire:
					   {  
						   strcat(pszMsg, "<color=Fire>");
						   strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>衣服<color>和<color=Wood>项链<color>激活暗属性<color>\n");
					   }  
					break;
				   case series_earth:
					   {   
						  strcat(pszMsg, "<color=Earth>");
						  strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>衣服<color>和<color=Fire>项链<color>激活暗属性<color>\n");
					   }   
					break;
				 }  
             break;   
            }
        case  equip_armor:
            {
                switch(m_CommonAttrib.nSeries)
                {
              	   case series_metal:
					   { 
			             strcat(pszMsg, "<color=Metal>");
			             strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>下戒<color>和<color=Earth>腰带<color>激活暗属性<color>\n");
					   }	  
			       break;
	         	   case series_wood:
					  { 
			             strcat(pszMsg, "<color=Wood>");
			             strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>下戒<color>和<color=Water>腰带<color>激活暗属性<color>\n");
					  }  
			       break;
		           case series_water:
					  { 	 
			             strcat(pszMsg, "<color=Water>");
			             strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>下戒<color>和<color=Metal>腰带<color>激活暗属性<color>\n");
					  }	 
			       break;
	        	   case series_fire:
					  { 
			             strcat(pszMsg, "<color=Fire>");
			             strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>下戒<color>和<color=Wood>腰带<color>激活暗属性<color>\n");
					  }  
			       break;
	        	   case series_earth:
					  { 
			            strcat(pszMsg, "<color=Earth>");
			            strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>下戒<color>和<color=Fire>腰带<color>激活暗属性<color>\n");
					  }  
                   break;
				}  
				break;   
            }
        case equip_ring:
            {
                switch(m_CommonAttrib.nSeries)
                {
              	  case series_metal:
			          strcat(pszMsg, "<color=Metal>");
			       break;
	         	  case series_wood:
			          strcat(pszMsg, "<color=Wood>");
			       break;
		          case series_water:
		              strcat(pszMsg, "<color=Water>");
			       break;
	        	  case series_fire:
			           strcat(pszMsg, "<color=Fire>");
		          break;
	        	  case series_earth:
		              strcat(pszMsg, "<color=Earth>");
			     //     strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>下戒<color>和<color=Fire>腰带<color>激活暗属性<color>\n");
			      break;
				  default:
					  strcat(pszMsg, "<color=White>");
					  break;
                } 
				strcat(pszMsg, "上戒需要<color=Cyan>衣服和头盔,<color><color=Cyan>下戒<color>需要<color=Cyan>护腕和玉佩<color>激活暗属性<color>\n");
			    break;
            }
        case equip_amulet:
            {
                switch(m_CommonAttrib.nSeries)
                {
              	   case series_metal:
						{ 
			                strcat(pszMsg, "<color=Metal>");
			                strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>下戒<color>和<color=Earth>腰带<color>激活暗属性<color>\n");
						}  
			       break;
	         	   case series_wood:
					   { 
			                strcat(pszMsg, "<color=Wood>");
			                strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>下戒<color>和<color=Water>腰带<color>激活暗属性<color>\n");
					   }  
			       break;
		           case series_water:
					   {   
			                strcat(pszMsg, "<color=Water>");
			                strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>下戒<color>和<color=Metal>腰带<color>激活暗属性<color>\n");
					   }   
			       break;
	        	   case series_fire:
					   { 
			                strcat(pszMsg, "<color=Fire>");
			                strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>下戒<color>和<color=Wood>腰带<color>激活暗属性<color>\n");
					   }	 
			       break;
	        	   case series_earth:
					   { 
			                strcat(pszMsg, "<color=Earth>");
		                    strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>下戒<color>和<color=Fire>腰带<color>激活暗属性<color>\n");
					   }
			       break;
                }    
			break;
            }
         case equip_boots:
			 {
                switch(m_CommonAttrib.nSeries)
				{
              	   case series_metal:
					   { 
			               strcat(pszMsg, "<color=Metal>");
			               strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>头盔<color>和<color=Earth>武器<color>激活暗属性<color>\n");
					   }
			       break;
	               case series_wood:
					   { 
			              strcat(pszMsg, "<color=Wood>");
			              strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>头盔<color>和<color=Water>武器<color>激活暗属性<color>\n");
					   }   
			       break;
		           case series_water:
					   { 
			              strcat(pszMsg, "<color=Water>");
			              strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>头盔<color>和<color=Metal>武器<color>激活暗属性<color>\n");
					   }	 
			      break;
	        	  case series_fire:
					  { 
			              strcat(pszMsg, "<color=Fire>");
			              strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>头盔<color>和<color=Wood>武器<color>激活暗属性<color>\n");
					  }	 
		 	      break;
	        	  case series_earth:
					  { 
			              strcat(pszMsg, "<color=Earth>");
			              strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>头盔<color>和<color=Fire>武器<color>激活暗属性<color>\n");
					  }	 
			      break;
				}    
            break;
			 }
           case equip_belt:
                    {
                       switch(m_CommonAttrib.nSeries)
                        {
              	          case series_metal:
							  { 
			                     strcat(pszMsg, "<color=Metal>");
			                     strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>护腕<color>和<color=Earth>玉佩<color>激活暗属性<color>\n");
							  }   
			              break;
	         	          case series_wood:
							  {  
			                     strcat(pszMsg, "<color=Wood>");
			                     strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>护腕<color>和<color=Water>玉佩<color>激活暗属性<color>\n");
							  }	  
			              break;
		                  case series_water:
							  {    
			                     strcat(pszMsg, "<color=Water>");
			                     strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>护腕<color>和<color=Metal>玉佩<color>激活暗属性<color>\n");
							  }	   
			              break;
	        	          case series_fire:
							  { 	    
			                     strcat(pszMsg, "<color=Fire>");
			                     strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>护腕<color>和<color=Wood>玉佩<color>激活暗属性<color>\n");
							  }  
			              break;
	        	          case series_earth:
							  { 
		                         strcat(pszMsg, "<color=Earth>");
			                     strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>护腕<color>和<color=Fire>玉佩<color>激活暗属性<color>\n");
							  }		    
			              break;
					   }  
                     break;
                    }
            case equip_helm:
                    {
                        switch(m_CommonAttrib.nSeries)
                        {
              	          case series_metal:
							   {  
			                      strcat(pszMsg, "<color=Metal>");
			                      strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>衣服<color>和<color=Earth>项链<color>激活暗属性<color>\n");	
							   }	    
			              break;
	         	          case series_wood:
							  {   
			                      strcat(pszMsg, "<color=Wood>");
			                      strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>衣服<color>和<color=Water>项链<color>激活暗属性<color>\n");	
							  }	    
			              break;
		                  case series_water:
							  { 	 
			                      strcat(pszMsg, "<color=Water>");
			                      strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>衣服<color>和<color=Metal>项链<color>激活暗属性<color>\n");	
							  }		 
			              break;
	        	          case series_fire:
							  {   
			                      strcat(pszMsg, "<color=Fire>");
			                      strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>衣服<color>和<color=Wood>项链<color>激活暗属性<color>\n");
							  } 	   
			              break;
	        	          case series_earth:
							  { 
			                      strcat(pszMsg, "<color=Earth>");
			                      strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>衣服<color>和<color=Fire>项链<color>激活暗属性<color>\n");
							  }	  
			              break;
						}
                   break;
					}
            case equip_cuff:
                    {
                        switch(m_CommonAttrib.nSeries)
                        {
              	           case series_metal:
							   {  
			                       strcat(pszMsg, "<color=Metal>");
			                       strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>上戒<color>和<color=Earth>靴子<color>激活暗属性<color>\n");
							   }	 
			                break;
	         	           case series_wood:
							   {   
			                       strcat(pszMsg, "<color=Wood>");
			                       strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>上戒<color>和<color=Water>靴子<color>激活暗属性<color>\n");
							   }	  
			                break;
		                   case series_water:
							   {   
			                       strcat(pszMsg, "<color=Water>");
			                       strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>上戒<color>和<color=Metal>靴子<color>激活暗属性<color>\n");
							   }   
			                break;
	        	           case series_fire:
							   {   
			                       strcat(pszMsg, "<color=Fire>");
			                       strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>上戒<color>和<color=Wood>靴子<color>激活暗属性<color>\n");
							   }		 
			                break;
	        	           case series_earth:
							   {		 
			                       strcat(pszMsg, "<color=Earth>");
			                       strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>上戒<color>和<color=Fire>靴子<color>激活暗属性<color>\n");
							   }	  
			                break;
						}                       
                    break;
					}
           case equip_pendant:
                    {
                        switch(m_CommonAttrib.nSeries)
						{
              	           case series_metal:
							   { 
			                      strcat(pszMsg, "<color=Metal>");
			                      strcat(pszMsg, "需要<color=Cyan>土系<color><color=Earth>上戒<color>和<color=Earth>靴子<color>激活暗属性<color>\n");
							   } 	    
			               break;
	         	           case series_wood:
							  { 
			                      strcat(pszMsg, "<color=Wood>");
			                      strcat(pszMsg, "需要<color=Cyan>水系<color><color=Water>上戒<color>和<color=Water>靴子<color>激活暗属性<color>\n");
							  } 		 
			               break;
		                   case series_water:
							  {    
			                      strcat(pszMsg, "<color=Water>");
			                      strcat(pszMsg, "需要<color=Cyan>金系<color><color=Metal>上戒<color>和<color=Metal>靴子<color>激活暗属性<color>\n");
							  } 	  
			               break;
			               case series_fire:
							   {  
			                      strcat(pszMsg, "<color=Fire>");
			                      strcat(pszMsg, "需要<color=Cyan>木系<color><color=Wood>上戒<color>和<color=Wood>靴子<color>激活暗属性<color>\n");
							   } 	   
			               break;
	        	           case series_earth:
							   {	 
			                      strcat(pszMsg, "<color=Earth>");
			                      strcat(pszMsg, "需要<color=Cyan>火系<color><color=Fire>上戒<color>和<color=Fire>靴子<color>激活暗属性<color>\n");
							   }   
			               break;
						} 
                       break;
					}
		   default:
			    strcat(pszMsg, "<color=Cyan>请看F1帮助五行激活图<color>\n");
                break;  
				}
        }

/*	if (m_CommonAttrib.nGoldId>0)
	{
    	strcat(pszMsg, "\n");
		KTabFile GoldTab;
		char szGoldName[80]={0};
		GoldTab.Load("\\Settings\\item\\004\\GoldItem.txt");

		for (int k = 0;k < m_CommonAttrib.nSetNum;++k)  //显示套装另外件数
		{
			GoldTab.GetString(m_CommonAttrib.nGoldId + 3 - m_CommonAttrib.nSetId + k,1,"",szGoldName,sizeof(szGoldName));
			
			int ncolor = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldColor(m_CommonAttrib.nSet,k + 1);
			
			//if (m_CommonAttrib.nSetId == k + 1)  //当前装备 //扩展套装序号
			//{
			//	strcat(pszMsg, "<color=Yellow>");
			//	strcat(pszMsg, szGoldName);
			//	strcat(pszMsg, "<color>\n");
			//}

		    if (!szGoldName[0])
			   continue;

			if (ncolor == 0)
			{
				strcat(pszMsg, "<color=DGreen>");
			}
			else if (ncolor == 1)
			{//所有物品
				strcat(pszMsg, "<color=HGreen>");
			}
			else if (ncolor == 2)
			{//装备着的
				strcat(pszMsg, "<color=Green>");	
			}
			else
				strcat(pszMsg, "<color=White>");

			strcat(pszMsg, szGoldName);
			strcat(pszMsg, "<color>\n");
		   
		}	  //end for

		GoldTab.Clear();
	} 
	
	  
		if (m_CommonAttrib.nIsWhere==-1)
		{  
		t_sprintf(pPin,"<color=yellow>该装备可以洗练出随机特殊属性");
		strcat(pszMsg,pPin);
		strcat(pszMsg, "<color>\n");
	 }	
		  
	*/

	if (nType==0 && m_CommonAttrib.nRongNum>0)
	{
		 char sRong[32]={0};
		 char sDong[32]={0};

         if (m_CommonAttrib.nIsWhere==-1 && (IsBlue() || IsPurple()))
		 {
			strcat(pszMsg,"\n");
			strcat(pszMsg, "<color=Green>");
			int mRmun=0;
			for (int nRong=0;nRong<6;++nRong)
			{
				if (m_ronMagicAttrib[nRong].nAttribType)
					mRmun++;
			}	
			
			if (GetIsWhere()>0)
			{
			   t_sprintf(sRong,"已洗练属性：%d/%d个",mRmun,m_CommonAttrib.nRongNum);
			}
			else
			{
               t_sprintf(sRong,"已洗练属性：0/%d个",m_CommonAttrib.nRongNum);
			}

			strcat(pszMsg,sRong);
			strcat(pszMsg,"\n");

			t_sprintf(sDong,"最大洗练属性数量为%d",m_CommonAttrib.nRongNum);
			strcat(pszMsg,sDong);
			strcat(pszMsg,"<color>\n");
		 }
		 else if (m_CommonAttrib.nGoldId>0)
		 {
			 strcat(pszMsg,"\n");
			 strcat(pszMsg, "<color=Green>");
			 int mRmun=0;
			 for (int nRong=0;nRong<6;++nRong)
			 {
				 if (m_ronMagicAttrib[nRong].nAttribType>0)
					 mRmun++;
			 }	
			 
			 t_sprintf(sRong,"已熔炼纹钢数量：%d/%d个",mRmun,m_CommonAttrib.nRongNum);
			 strcat(pszMsg,sRong);
			 strcat(pszMsg,"\n");

			 t_sprintf(sDong,"最大可熔炼纹钢等级为%d",m_CommonAttrib.nWengangPin);
			 strcat(pszMsg,sDong);
			 strcat(pszMsg,"<color>\n");
		 }
	}
	

	if (nType==0 && m_CommonAttrib.nBinfujiazhi>0)
	{  
		strcat(pszMsg, "<color=HGREEN>");
		char sBong[32]={0};
		t_sprintf(sBong,"基础兵甲值：%d",m_CommonAttrib.nBinfujiazhi);
		strcat(pszMsg,sBong);
		strcat(pszMsg,"\n");
		strcat(pszMsg,VER_INFO);
		strcat(pszMsg,"<color>\n");
	} 
/*	else
	{  
		strcat(pszMsg, "<color=HGREEN>");
		strcat(pszMsg,"基础兵甲值：0");
		strcat(pszMsg,"\n");
		strcat(pszMsg,VER_INFO);
		strcat(pszMsg,"<color>\n");
	}*/


	if (m_CommonAttrib.uPrice > 0)  //摆摊标价
	{
	    char nD[8];
        if (m_CommonAttrib.ncSellModel==2)
	      t_sprintf(nD,"金");
	    else
          t_sprintf(nD,"两");

		strcat(pszMsg,"\n");
		strcat(pszMsg, "<color=Yellow>");
		char sOrice[32];

		int nN = m_CommonAttrib.uPrice/100000000;//TakeTrader(m_CommonAttrib.uPrice,100000000);
		int nW = (m_CommonAttrib.uPrice%100000000)/10000;//TakeTrader(TakeRemainder(m_CommonAttrib.uPrice,100000000),10000);
		int nG = m_CommonAttrib.uPrice%10000;//TakeRemainder(m_CommonAttrib.uPrice,10000);

	    if (nN > 0)
		{  		
		   t_sprintf(sOrice,"摆摊价值：%d亿%d万%d%s",nN,nW,nG,nD);
		}  
	    else
		{   
		   if (nW == 0)
		      t_sprintf(sOrice,"摆摊价值：%d%s",nG,nD);
		   else
		      t_sprintf(sOrice,"摆摊价值：%d万%d%s",nW,nG,nD);
		}  
		strcat(pszMsg,sOrice);
		strcat(pszMsg,"<color>\n");
	}

    if (m_CommonAttrib.nGoldId>0 /*&& m_CommonAttrib.nSet>0*/)
	{
    	strcat(pszMsg, "\n");
		KTabFile GoldTab;
		char szGoldName[80]={0};

		if (m_CommonAttrib.nIsPlatina ==0 )
		    GoldTab.Load("\\Settings\\item\\004\\GoldItem.txt");
		else
			GoldTab.Load("\\Settings\\item\\004\\platinaequip.txt");

		for (int k = 0;k < m_CommonAttrib.nSetNum;++k)  //显示套装另外件数
		{
			GoldTab.GetString(m_CommonAttrib.nGoldId + 3 - m_CommonAttrib.nSetId + k,1," ",szGoldName,sizeof(szGoldName));
			
			int ncolor = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldColor(m_CommonAttrib.nSet,k + 1);
			
		    if (!szGoldName[0])
			   continue;

			if (ncolor == 0)
			{
				strcat(pszMsg, "<color=DGreen>");
			}
			else if (ncolor == 1)
			{//所有物品
				strcat(pszMsg, "<color=HGreen>");
			}
			else if (ncolor == 2)
			{//装备着的
				strcat(pszMsg, "<color=Fire>");		  //Yellow
			}
			else
				strcat(pszMsg, "<color=White>");
			
		   /*	if (m_CommonAttrib.nSetId == k + 1)  //当前装备 //扩展套装序号
			{
				strcat(pszMsg, "<color=Orange>");
				strcat(pszMsg, szGoldName);
				strcat(pszMsg, "<color>\n");
			}*/
			
			strcat(pszMsg, szGoldName);
			strcat(pszMsg, "<color>\n");
		   
		}	  //end for

		GoldTab.Clear();
	}
	
	//strcat(pszMsg,"\0");
}

int KItem::ExtractChars(const char *inStr,int scrLen,char *outStr)
{
	char *tmp=NULL;         //定义一个临时数组空间，存放字符；
	tmp=outStr; 
	int nLen=0;
	while(*inStr!='\0')
	{//一直前行,直到有结束符号将停止。
		if(nLen==scrLen) //如果长度相等的时候
		{
			//*inStr='|';   //替换掉开始字符
			*tmp++;         //下个字符    
            *tmp='\n';      //加个换行符号
			//inStr++;             //去掉结束符号
		}
		if (*inStr=='\0')          //如果没有找到结束符，并且已经到了结尾，就跳出了！！
			break;
		*tmp=*inStr;               //元数不相同，则存放到tmp中。
		tmp++;                     //进行下一个元数的比较
		inStr++;                    
		nLen++;
	}
	*tmp='\0';              //源字符串 提取完后 加个结束符 防止乱码
    return nLen;
}


//最大持久度
int KItem::GetMaxDurability()
{
	for (int i = 0; i < 7; ++i)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			return m_aryBaseAttrib[i].nValue[0];
		}
	}
	return -1;
}
//取得装备的属性等级参数
int KItem::GetTotalMagicLevel()
{
	int nRet = 0;
	for (int i = 0; i < 6; ++i)
	{
		//nRet += m_GeneratorParam.nGeneratorLevel[i];
		if (m_GeneratorParam.nGeneratorLevel[i])
			nRet++;
		if (m_GeneratorParam.nRGeneratorLevel[i])
			nRet++;
		//if (m_GeneratorParam.nBGeneratorLevel[i])
		//	nRet++;
		//if (m_GeneratorParam.nBGeneratorLevel[i])
		//	nRet++;
	}

	return nRet;
}
//获取修理价格
int KItem::GetRepairPrice()
{
	if (ItemSet.m_sRepairParam.nMagicScale == 0)
		return 0;

	if (GetGenre() != item_equip)          //不是装备
		return 0;

	if (m_nCurrentDur == -1)               //永不磨损
		return 0;

	int nMaxDur   = GetMaxDurability();
	int nSumMagic = GetTotalMagicLevel();  //魔法属性的总等级

	if (nMaxDur <= 0)                      //最大持久度没有
		return 0;

	int nRepairPrice=0;
        nRepairPrice = m_CommonAttrib.nLevel*ItemSet.m_sRepairParam.nLevelScale + m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale/100 * (nMaxDur - m_nCurrentDur)/nMaxDur * (ItemSet.m_sRepairParam.nMagicScale + nSumMagic)/ItemSet.m_sRepairParam.nMagicScale;
    //修理价格=物品等级*等级倍率+物品价格*价格倍率/100*(最大持久-当前持久)/最大持久*(魔法倍率+魔法数量)/魔法倍率	
	//该物品的价格X维修价格的倍率/100
	return nRepairPrice;
}

//是否能修复
BOOL KItem::CanBeRepaired()
{
	if (GetGenre() != item_equip)
		return FALSE;

	//if (GetIsWhere()>0 && GetIsUse()<=0) //辨识装 并且是不可使用的 不能修理
	//   return FALSE;

	if (m_nCurrentDur == -1)  //永远不磨损 
		return FALSE;

	int nMaxDur = GetMaxDurability();
	if (m_nCurrentDur == nMaxDur)
		return FALSE;

	return TRUE;
}

BOOL KItem::CheckItemInfo(int nGen,int nDetail,int nParticular,char *KeyName)
{
	int nItemInfo[3];
	int nRows = g_ForbitMap.GetHeight();
	bool nReturn = false;

	for (int i = 2;i<nRows+1;i++)
	{
		g_ForbitMap.GetInt3s(i,KeyName,nItemInfo);
		if (nItemInfo[0] == nGen && nItemInfo[1]==nDetail && nItemInfo[2]==nParticular)
		{
			nReturn = true;
			break;
		}
	}
	return nReturn;
}

//是否可以叠加
BOOL KItem::CanStack(int nOldIdx)
{
	if (m_CommonAttrib.bStack)
	{
		int nGen        = Item[nOldIdx].GetGenre();
		int nDetail     = Item[nOldIdx].GetDetailType();
		int nParticular = Item[nOldIdx].GetParticular();
		int nSeries     = Item[nOldIdx].GetSeries();
		int m_nLevel    = Item[nOldIdx].GetLevel();

		if (m_CommonAttrib.nItemGenre == nGen && m_CommonAttrib.nDetailType == nDetail && m_CommonAttrib.nParticularType == nParticular && Item[nOldIdx].GetStackNum() < MAX_STACKNUM)   //数量小于50 就可以叠加
		{   
			if (CheckItemInfo(nGen,nDetail,nParticular,"NeedLevel"))
			{//检测等级
				if (m_CommonAttrib.nLevel != m_nLevel)
					return FALSE;
			}

			if (CheckItemInfo(nGen,nDetail,nParticular,"NeedSeries"))
			{//检测等级
				if (m_CommonAttrib.nSeries != nSeries)
					return FALSE;
			}

			return TRUE;
		}
	}
	return FALSE;
}
//是否可以再叠加
int KItem::Stack( int nIdx )
{
	m_CommonAttrib.nStackNum += nIdx;
	
	if (m_CommonAttrib.nStackNum > MAX_STACKNUM)  //可叠加物品的数量限制
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void KItem::SetBackUpMagicAttrib()
{
  for (int i = 0;i < 6;++i)
  {
    m_TempMagicAttrib[i].nValue[0]=m_aryMagicAttrib[i].nValue[0];
  }
}

void KItem::SetBackUpRmagicAttrib()
{
  for (int i = 0;i < 6;++i)
  {
    m_TempRMagicAttrib[i].nValue[0]=m_ronMagicAttrib[i].nValue[0];
  }
}

void KItem::SetBackUpBmagicAttrib()
{
/*  for (int i = 0;i < 6;++i)
  {
    m_TempBMagicAttrib[i].nValue[0]=m_BaoShiMagicAttrib[i].nValue[0];
  }*/
}

//魔法装备的强化
int KItem::EnChance( int nEnChance /*= 1*/ )
{
	    int nCanEn=0,i;

		if (nEnChance < 31)
		{
		   //int nOldEnChance = m_CommonAttrib.nEnChance;  //原来的加成

			for (i = 0;i < 6;++i)
			{
				nCanEn=CheckEnChance("ForBitEn",m_aryMagicAttrib[i].nAttribType);    

				if (m_aryMagicAttrib[i].nAttribType && nCanEn==1)
				{
					if (m_aryMagicAttrib[i].nValue[0] >= (JIACHENG_VAL<<1))
					{
						   int nTempVal=m_TempMagicAttrib[i].nValue[0]; //原始值

                            for (int j=0;j<nEnChance;++j)	//nEnChance+1
							{
							     nTempVal +=nTempVal>>JIACHENG_VAL;//*j
						        //if (nTempVal%JIACHENG_VAL>=2)
						        if (nTempVal-(nTempVal>>JIACHENG_VAL<<JIACHENG_VAL)>2)
						           nTempVal +=1;
							}

						    m_aryMagicAttrib[i].nValue[0] = nTempVal;
					}
				}
			}

			m_CommonAttrib.nEnChance = nEnChance;
		}
		else
		{
			m_CommonAttrib.nEnChance = 30;
		}
	
	return 1;
}
//强化熔炼属性
int KItem::EnChanceRong( int nEnChance /*= 1*/ )
{	
	int nCanEn=0;
	if (nEnChance < 31)
	{
		int nOldEnChance = m_CommonAttrib.nEnChance;//原来的加成

		for (int i = 0;i < 6;++i)
		{		
			nCanEn=CheckEnChance("ForBitEn",m_aryMagicAttrib[i].nAttribType);    
		
		    if (m_aryMagicAttrib[i].nAttribType && nCanEn==1)
			{//蓝色属性	 
			    if (m_aryMagicAttrib[i].nValue[0] >= (JIACHENG_VAL<<1))
				{  
				    int nTempVal=m_TempMagicAttrib[i].nValue[0]; //原始值
				
				    for (int j=0;j<nEnChance;++j)
					{
					     nTempVal +=nTempVal>>JIACHENG_VAL;//*j
						        //if (nTempVal%JIACHENG_VAL>=2)
						 if (nTempVal-(nTempVal>>JIACHENG_VAL<<JIACHENG_VAL)>2)
						     nTempVal +=1;
					} 
				
				    m_aryMagicAttrib[i].nValue[0] = nTempVal;
				} 
			}
			
			nCanEn=CheckEnChance("ForBitEn",m_ronMagicAttrib[i].nAttribType);
			if (m_ronMagicAttrib[i].nAttribType && nCanEn==1)
			{//辨识或 纹钢属性
				if (m_ronMagicAttrib[i].nValue[0] >= (JIACHENG_VAL<<1))
				{
						/* if (nEnChance>nOldEnChance)      //成功的
						 {
						    m_TempRMagicAttrib[i].nValue[0]=m_ronMagicAttrib[i].nValue[0]; //保存这个属性原来的 属性值（用于强化失败用的）
						    m_ronMagicAttrib[i].nValue[0] += m_ronMagicAttrib[i].nValue[0] * nEnChance/JIACHENG_VAL;
						  }
						 else if (nEnChance< nOldEnChance) //失败的
						 {
						    m_ronMagicAttrib[i].nValue[0] = m_TempRMagicAttrib[i].nValue[0];//m_ronMagicAttrib[i].nValue[0] * (nEnChance+1)/JIACHENG_VAL;
						 }*/
						 int nTempVal=m_TempRMagicAttrib[i].nValue[0]; //原始值

                            for (int j=0;j<nEnChance;++j)
							{
							     nTempVal +=nTempVal>>JIACHENG_VAL;//*j
						        //if (nTempVal%JIACHENG_VAL>=2)
						        if (nTempVal-(nTempVal>>JIACHENG_VAL<<JIACHENG_VAL)>2)
						           nTempVal +=1;
							}

						    m_ronMagicAttrib[i].nValue[0] = nTempVal;

				}
			}
		}
	   m_CommonAttrib.nEnChance = nEnChance;
	}
	else
	{
		m_CommonAttrib.nEnChance = 30;

		/*for (int i = 0;i < 6;++i)
		{
			nCanEn=CheckEnChance("ForBitEn",m_ronMagicAttrib[i].nAttribType);
			if (m_ronMagicAttrib[i].nAttribType && nCanEn==0)
			{
				if (m_ronMagicAttrib[i].nValue[0] >= 5)
				{
					m_ronMagicAttrib[i].nValue[0] += m_ronMagicAttrib[i].nValue[0] * nEnChance / 18;
				}
			}
		}*/
	}
	
	return 1;
}

//强化宝石属性
int KItem::EnChanceBao( int nEnChance /*= 1*/)
{	
/*	int nCanEn=0;
	if (nEnChance < 31)
	{
	    int nOldEnChance = m_CommonAttrib.nEnChance;//原来的加成

		for (int i = 0;i < 6;++i)
		{
			nCanEn=CheckEnChance("ForBitEn",m_BaoShiMagicAttrib[i].nAttribType);

			if (m_BaoShiMagicAttrib[i].nAttribType && nCanEn==0)
			{
				if (m_BaoShiMagicAttrib[i].nValue[0] >= JIACHENG_VAL)
				{
					if (nEnChance>nOldEnChance)      //成功的
					{
					    m_TempBMagicAttrib[i].nValue[0]=m_BaoShiMagicAttrib[i].nValue[0]; //保存这个属性原来的 属性值（用于强化失败用的）
					    m_BaoShiMagicAttrib[i].nValue[0] += m_BaoShiMagicAttrib[i].nValue[0] * nEnChance/JIACHENG_VAL;
					}
					else if (nEnChance< nOldEnChance) //失败的
					{//失败就还原原来的值
						m_BaoShiMagicAttrib[i].nValue[0] = m_TempBMagicAttrib[i].nValue[0];//m_BaoShiMagicAttrib[i].nValue[0] * (nEnChance+1)/JIACHENG_VAL;
				    }
					int nTempVal=m_TempBMagicAttrib[i].nValue[0]; //原始值
                            for (int j=0;j<nEnChance+1;++j)
							     nTempVal +=nTempVal*j/JIACHENG_VAL;

						    m_BaoShiMagicAttrib[i].nValue[0] = nTempVal;
				}
			}
		}

	  m_CommonAttrib.nEnChance = nEnChance;
	}
	else
	{
		m_CommonAttrib.nEnChance = 30;
	}
	*/
	return 1;
}


int	KItem::CheckEnChance(char *nKey,int nAttribType)
{
	int nRow = g_ForbitMap.GetHeight()+1,nReg=0;
	
	for (int i=2;i<nRow;++i)
	{
		int nMaps =0;

		g_ForbitMap.GetInteger(i,nKey,0,&nMaps);

		if (nAttribType==nMaps)
		{
			nReg =1;
			break;
		}
	}
	return nReg;     
}


int KItem::IsReceptacle()
{
	return 1;
}


BOOL KItem::HoldItem(int nIdx, int nWidth, int nHeight)
{
//	int i, j;
	//for (i = 0; i < m_nWidth - nWidth + 1; ++i)
//	{
//		for (j = 0; j < m_nHeight - nHeight + 1; ++j)
//		{
	//		if (PlaceItem(i, j, nIdx, nWidth, nHeight))
				return TRUE;
	//	}
//	}
//	return FALSE;
}

int KItem::IsPurple()  //是紫装
{
	if (m_CommonAttrib.nPoint>0 && m_CommonAttrib.nPoint<=6)
	return m_CommonAttrib.nPoint;

	return 0;
	
}

int KItem::GetPoint()  
{

   return m_CommonAttrib.nPoint;
	
}

BOOL KItem::CheckReqability(int n)
{
	for (int i=0;i<6;++i)
	{
		if (m_aryRequireAttrib[i].nAttribType==n)
	     return TRUE;
	}
	return FALSE;
}

int KItem::GetIsWhere()  
{
	
   return m_CommonAttrib.nIsWhere;
	
}

int KItem::getItemKind()
{
	if  (m_CommonAttrib.nGoldId>0)
		return 1; //TB Hoang Kim
	else if ((m_CommonAttrib.nPoint==7 || m_CommonAttrib.nPoint==0) && m_aryMagicAttrib[0].nAttribType>0)
		return 2; //TB XANH
	else if (m_CommonAttrib.nPoint>0 && m_CommonAttrib.nPoint<=6)
	    return 3; //TB TIM
	else
		return 0;//TB THUONG
}

int KItem::IsBlue()  //TB XANH
{
	if ((m_CommonAttrib.nPoint==7 || m_CommonAttrib.nPoint==0) && m_CommonAttrib.nGoldId<=0 && m_aryMagicAttrib[0].nAttribType>0)
    return 1;

    return 0;
}

int KItem::IsPlatina()  //TB BACH KIM

{
	if (m_CommonAttrib.nGoldId>0 && m_CommonAttrib.nIsPlatina>0)
		return 1;
	
    return 0;
}


int KItem::IsRong()  //是熔炼
{
	return m_CommonAttrib.nRongpiont;
}

int KItem::IsGold()  //是熔炼
{
	return m_CommonAttrib.nGoldId;
}

//设置物品的使用时间
void KItem::SetTime( int bYear,int bMonth,int bDay,int bHour,int bMin)
{
	if (bYear>0)
	{
		while (bMin >= 60)
		{
			bHour++;
			bMin -= 60;
		}

		while (bHour >= 24)
		{
			bDay++;
			bHour -= 24;
		}

		BOOL nIsYear=FALSE;

		int  nAllDay=30;

		if (bYear%4==0)	   //当年
		{//润年
		   nIsYear=TRUE;
		   //if (bMonth==2)  //当月
		//	   nAllDay=29;
		}
		else
		{//平年
 			nIsYear=FALSE;
			//if (bMonth==2)	//当月
			 //  nAllDay=28;
		}
		//-----进入时候的当月
		if (bMonth<=12)
		{//小于12个月
			if (bMonth==2 && nIsYear)
			{//润年的2月份
				nAllDay=29;
			}
			else if (bMonth==2 && !nIsYear)
			{//平年的2月份
				nAllDay= 28;
			}
			else
			{
				if (bMonth==1 || bMonth==3 || bMonth==5|| bMonth==7|| bMonth==8|| bMonth==10 || bMonth==12) 
				{//当月
					nAllDay=31;
				}	
				else if (bMonth==4 || bMonth==6 || bMonth==9|| bMonth==11) 
				{//当月
					nAllDay=30;
				}
			}
		}
		else
		{//总月份大于12个月的
			if (bMonth%12==2 && nIsYear)
			{//润年的2月份
				nAllDay=29;
			}
			else if (bMonth%12==2 && !nIsYear)
			{//平年的2月份
				nAllDay= 28;
			}
			else
			{
				if (bMonth%12==0 || bMonth%12==1 || bMonth%12==3 || bMonth%12==5|| bMonth%12==7|| bMonth%12==8|| bMonth%12==10) 
				{//当月
					nAllDay=31;
				}	
				else if (bMonth%12==4 || bMonth%12==6 || bMonth%12==9|| bMonth%12==11) 
				{//当月
					nAllDay=30;
				}
			}		
		}
		
	 while (bDay > nAllDay)
	 {

	/*		if (((bMonth <= 7) && !(bMonth % 2)) || ((bMonth > 7) && !(bMonth % 2)))
			{//双月 2 4 6 8 10 12 
				bMonth++;
				bDay -= 30;
			}
			else if (((bMonth <= 7) && !(bMonth % 2) && bDay > 31 )||((bMonth > 7) && (bMonth % 2) && bDay > 31))
			{//双月 2 4 6  或   单月 9 11  天数》31
				bMonth++;
				bDay -= 31;
			}
			else if (((bMonth <= 7) && !(bMonth % 2) && bDay == 31 )||((bMonth > 7) && (bMonth % 2) && bDay == 31))
			{//单月 1 3 5 7  天数=31 或 双月 8 10 12 天数=31
				break;
			}*/

			bMonth++;
			bDay -= nAllDay;

			//-----------下一个月----------
		 if (bMonth<=12)
		 {//小于12个月
			if (bMonth==2 && nIsYear)
			{//润年的2月份
				nAllDay=29;
			}
			else if (bMonth==2 && !nIsYear)
			{//平年的2月份
				nAllDay= 28;
			}
			else
			{
			   	if (bMonth==1 || bMonth==3 || bMonth==5|| bMonth==7|| bMonth==8|| bMonth==10 || bMonth==12) 
				{//当月
					nAllDay=31;
				}	
				else if (bMonth==4 || bMonth==6 || bMonth==9|| bMonth==11) 
				{//当月
					nAllDay=30;
				}
			}
		 }
		 else
		 {//总月份大于12个月的
				if (bMonth%12==2 && nIsYear)
				{//润年的2月份
					nAllDay=29;
				}
				else if (bMonth%12==2 && !nIsYear)
				{//平年的2月份
					nAllDay= 28;
				}
				else
				{
					if (bMonth%12==0 || bMonth%12==1 || bMonth%12==3 || bMonth%12==5|| bMonth%12==7|| bMonth%12==8|| bMonth%12==10) 
					{//当月
						nAllDay=31;
					}	
					else if (bMonth%12==4 || bMonth%12==6 || bMonth%12==9|| bMonth%12==11) 
					{//当月
						nAllDay=30;
					}
			}

		 }

	 }

		while (bMonth > 12)
		{
			bYear++;
			bMonth -= 12;
		}
         
		if (bDay<=0)
		{
			bDay=1;
        }

		if (bMonth<=0)
			bMonth=1;

		if (bHour<0)
			bHour=1;

        if (bMin<0)
			bMin=0;

		m_CommonAttrib.LimitTime.bYear  = bYear;
		m_CommonAttrib.LimitTime.bMonth = bMonth;
		m_CommonAttrib.LimitTime.bDay   = bDay;
		m_CommonAttrib.LimitTime.bHour  = bHour;
		m_CommonAttrib.LimitTime.bMin   = bMin;

	}
	else
	{
	    m_CommonAttrib.LimitTime.bYear  = 0;
		m_CommonAttrib.LimitTime.bMonth = 0;
		m_CommonAttrib.LimitTime.bDay   = 0;
		m_CommonAttrib.LimitTime.bHour  = 0;
		m_CommonAttrib.LimitTime.bMin   = 0;
	}
}

//检测纹钢的品质
int  KItem::CheckBluePin(int pShuXingID,int pVal)

{
	KTabFile MagicTab,MagicTabA;
	MagicTab.Load(TABFILE_MAGICALEVEL_PATH);
	int nRows=MagicTab.GetHeight();
	int nReg=0,nStart=0,nEnd=0;
	
	for (int i=0;i<nRows;++i)
	{
		int nMagid=0;
		MagicTab.GetInteger(i+1,"MAGIC_ID",1,&nMagid);  
		if (nMagid==pShuXingID) //是否可以纹钢提取的属性
		{ 
			MagicTab.GetInteger(i+1,"MAGATTRLVL_START",0,&nStart);  
			MagicTab.GetInteger(i+1,"MAGATTRLVL_END",0,&nEnd);
			nReg=i;
			break;
		} 
	}
    MagicTab.Clear();

	if (nReg==0)  //没有可以提取的属性
            return 0;

    if (nReg&&nStart&&nEnd)  //得到行数
	{
		MagicTabA.Load(TABFILE_MAGICATTRIB_PATH);
		int nRowsA=MagicTabA.GetHeight(),nAValmin=0,nAValmax=0,nLevel=0;
		for (nStart;nStart<nEnd;++nStart)
		{  
			MagicTabA.GetInteger(nStart+1,6,1,&nAValmin);
			MagicTabA.GetInteger(nStart+1,7,1,&nAValmax);
			if (pVal>=nAValmin && pVal<=nAValmax) //在这个范围内
			{ 
				MagicTabA.GetInteger(nStart+1,4,1,&nLevel);
				break;
			} 
			
		} 
		MagicTabA.Clear();
		return nLevel;
	}
    return 0;
}

//检测纹钢的品质
int  KItem::CheckWenPin(int pShuXingID,int pVal)

{
	KTabFile MagicTab,MagicTabA;
	MagicTab.Load(TABFILE_MAGICALEVEL_PATH);
	int nRows=MagicTab.GetHeight();
	int nReg=0,nStart=0,nEnd=0;
	
	for (int i=0;i<nRows;++i)
	{
		int nMagid=0;
		MagicTab.GetInteger(i+1,"MAGIC_ID",1,&nMagid);  
		if (nMagid==pShuXingID)
		{ 
			MagicTab.GetInteger(i+1,"MAGATTRLVL_START",0,&nStart);  
			MagicTab.GetInteger(i+1,"MAGATTRLVL_END",0,&nEnd);
			nReg=i;
			break;
		} 
	}
    MagicTab.Clear();
	
    if (nReg&&nStart&&nEnd)  //得到行数
	{
		MagicTabA.Load(TABFILE_MAGICATTRIB_PATH);
		int nRowsA=MagicTabA.GetHeight(),nAVal=0,nLevel=0;
		for (nStart;nStart<nEnd;++nStart)
		{  
			MagicTabA.GetInteger(nStart+1,6,1,&nAVal);
			if (nAVal==pVal)
			{ 
				MagicTabA.GetInteger(nStart+1,4,1,&nLevel);
				break;
			} 
			
		} 
		MagicTabA.Clear();
		return nLevel;
	}
    return 0;
}

//根据纹钢属性ID获取物品的 eventid 号
int  KItem::GeteventID(int pShuXingID)

{
	KTabFile MagicTab;
	MagicTab.Load(TABFILE_MAGICALEVEL_PATH);
	int nRows=MagicTab.GetHeight();
	int nReg=0;	
	for (int i=0;i<nRows;++i)
	{
		int nMagid=0;
		MagicTab.GetInteger(i+1,"MAGIC_ID",1,&nMagid);  
		if (nMagid==pShuXingID)
		{ 
			nReg=i;
			break;
		} 
	}
    MagicTab.Clear();
	
		return nReg;
}

void KItem::SetItemMagicLevel(int i,int nRow)
{
	m_aryBaseAttrib[i].nValue[0]=nRow;
}

int KItem::GetItemMagicLevel(int i)
{
   return m_aryBaseAttrib[i].nValue[0];
}
//是否有这个属性 装备
BOOL KItem::HaveMaigc( int nAttribe,int nValue1Min,int nValue1Max,int nValue2Min,int nValue2Max,int nValue3Min,int nValue3Max )
{
	for (int i = 0;i < 6;++i)  //魔法
	{
		if (m_aryMagicAttrib[i].nAttribType == nAttribe)
		{
			if (nValue1Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[0] < nValue1Min)
					return FALSE;
			}
			if (nValue1Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[0] > nValue1Max)
					return FALSE;
			}
			if (nValue2Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[1] < nValue2Min)
					return FALSE;
			}
			if (nValue2Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[1] > nValue2Max)
					return FALSE;
			}
			if (nValue3Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[2] < nValue3Min)
					return FALSE;
			}
			if (nValue3Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[2] > nValue3Max)
					return FALSE;
			}
			return TRUE;
		}
	   else if (m_ronMagicAttrib[i].nAttribType == nAttribe)
		{
			if (nValue1Min != -1)
			{
				if (m_ronMagicAttrib[i].nValue[0] < nValue1Min)
					return FALSE;
			}
			if (nValue1Max != -1)
			{
				if (m_ronMagicAttrib[i].nValue[0] > nValue1Max)
					return FALSE;
			}
			if (nValue2Min != -1)
			{
				if (m_ronMagicAttrib[i].nValue[1] < nValue2Min)
					return FALSE;
			}
			if (nValue2Max != -1)
			{
				if (m_ronMagicAttrib[i].nValue[1] > nValue2Max)
					return FALSE;
			}
			if (nValue3Min != -1)
			{
				if (m_ronMagicAttrib[i].nValue[2] < nValue3Min)
					return FALSE;
			}
			if (nValue3Max != -1)
			{
				if (m_ronMagicAttrib[i].nValue[2] > nValue3Max)
					return FALSE;
			}
			return TRUE;
		}
	}
    return FALSE;
}
