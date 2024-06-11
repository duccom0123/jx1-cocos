//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemGenerator.CPP
// Date:	2002.08.26
// Code:	DongBo
// Desc:    CPP file. 本文件实现的类用于生成道具
//---------------------------------------------------------------------------
#include "engine/KEngine.h"
#include "KCore.h"
//#include "MyAssert.H"
#include "KItem.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"

//#define	 TABFILE_MAGICATTRIB_PATH	"\\settings\\item\\004\\magicattrib.txt" //蓝装魔法属性
//#define	 TABFILE_MAGICAGOLD_PATH	"\\settings\\item\\004\\GoldMagic.txt"   //黄金魔法属性

KItemGenerator	ItemGen;			//	装备生成器

KItemGenerator::KItemGenerator()
{
}

KItemGenerator::~KItemGenerator()
{
}

/******************************************************************************
	功能：	装备数据初始化. 从tab file中读取数据
******************************************************************************/
BOOL KItemGenerator::Init()   
{
	if (!m_BPTLib.Init())		// 此调用从若干的tab file中载入所有的初始属性
	{
		//printf(" KItemGenerator::m_BPTLib() error!!!..\n");
		CCMessageBox("m_BPTLib() error!","KItemGenerator::Init");
		return FALSE;
	}
	for (int i = equip_meleeweapon; i < equip_detailnum - equip_meleeweapon; ++i)
	{//各装备的 数量
		 switch(i)
		 {//获取各数据的数量 是TXT的行数
		case equip_meleeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetMeleeWeaponRecordNumber();
			break;
		case equip_rangeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRangeWeaponRecordNumber();
			break;
		case equip_armor:
			m_EquipNumOfEntries[i] = m_BPTLib.GetArmorRecordNumber();
			break;
		case equip_helm:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHelmRecordNumber();
			break;
		case equip_boots:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBootRecordNumber();
			break;
		case equip_belt:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBeltRecordNumber();
			break;
		case equip_cuff:
			m_EquipNumOfEntries[i] = m_BPTLib.GetCuffRecordNumber();
			break;
		case equip_amulet:
			m_EquipNumOfEntries[i] = m_BPTLib.GetAmuletRecordNumber();
			break;
		case equip_ring:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRingRecordNumber();
			break;
		case equip_pendant:
			m_EquipNumOfEntries[i] = m_BPTLib.GetPendantRecordNumber();
			break;
		case equip_horse:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHorseRecordNumber();
			break;
		case equip_mask:	// mat na
			m_EquipNumOfEntries[i] = m_BPTLib.GetMaskRecordNumber();
			break;		
		case equip_pifeng:	// 披风
			m_EquipNumOfEntries[i] = m_BPTLib.GetPifengRecordNumber();
			break;
		case equip_yinjian:	// 
			m_EquipNumOfEntries[i] = m_BPTLib.GetYinjianRecordNumber();
			break;
		case equip_shiping:	// 
			m_EquipNumOfEntries[i] = m_BPTLib.GetShipinRecordNumber();
			break;			
	
		 } 
	} 
	  m_MedNumOfEntries = m_BPTLib.GetMedicineRecordNumber();
	  //printf(" KItemGenerator::Init(%d) OK!!!..\n",equip_detailnum-equip_meleeweapon);
	
	return TRUE;
}

/******************************************************************************
	功能：	生成药品
	入口:	nDetailType: 药品类型
			nLevel: 等级
			nVersion: 版本号
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_Medicine(IN int nDetailType,
								  IN int nLevel,
								  IN int nVersion,
								  IN OUT KItem* pItem,
								  IN int nStackNum)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if  (this==NULL || pItem==NULL)
		return false;

	BOOL bEC = FALSE;
	
#ifdef _SERVER	// 服务器版本
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 0;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	int nGameVersion = pItem->m_GeneratorParam.nVersion;
#endif

	const int i = nDetailType * 5 + nLevel - 1;	// 本公式由策划定义
												// 等级数据从1开始，所以要减回来
	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMed;
	pItem->SetStackNum(nStackNum);              //设置可叠加数量
	return TRUE;
}
/******************************************************************************
	功能：	生成药品=--商店用的
	入口:	nDetailType: 药品类型
			nLevel: 等级
			nVersion: 版本号
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_MedicineShop(IN int nDetailType,
								  IN int nLevel,
								  IN int nVersion,
								  IN OUT KItem* pItem,
								  IN int nStackNum,
								  int PerXu,int nIsBang,int nPriceType)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if  (this==NULL || pItem==NULL)
			return false;

	BOOL bEC = FALSE;
	
#ifdef _SERVER	// 服务器版本
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 0;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	int nGameVersion = pItem->m_GeneratorParam.nVersion;
#endif

	const int i = nDetailType * 5 + nLevel - 1;	// 本公式由策划定义
												// 等级数据从1开始，所以要减回来
	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);  //基本信息
	if (NULL == pMed)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMed;
	pItem->SetStackNum(nStackNum); //设置可叠加数量
    pItem->SetBang(nIsBang);

	if (PerXu>0)
	{//如果大于零的 就改变原始数据
       pItem->SetPerXu(PerXu);
	   pItem->SetPerPrice(PerXu);
	}

	pItem->SetModel(nPriceType);
	return TRUE;
}

/******************************************************************************
	功能：	生成随机装备
	入口:	nSeriesReq: 五行属性
			nLevel: 等级
			nLucky: 运气值
			nVersion: 版本号
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
/*BOOL KItemGenerator::Gen_Item(IN int nItemGenre,
							  IN int nSeriesReq,
							  IN int nLevel,
							  IN int nLucky,
							  IN OUT KItem* pItem,
							  IN int nStackNum,
							  IN int nEnChance)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	switch (nItemGenre)
	{
	case item_equip:
		{
			int nDetailType, nParticularType, i;
			nDetailType = g_Random(equip_detailnum);
			nParticularType = g_Random(m_EquipNumOfEntries[nDetailType] + 1);
			int naryMALevel[6];
			ZeroMemory(naryMALevel, 6 * sizeof(int));
			for (i = 0; i < 6; ++i)
			{
				naryMALevel[i] = g_Random(nLevel / 10 + 1);
				if (!naryMALevel[i])
					break;
			}
			Gen_Equipment(nDetailType, nParticularType, nSeriesReq, nLevel, naryMALevel, nLucky, g_SubWorldSet.GetGameVersion(), pItem, nEnChance,0,0);
		}
		break;
	case item_medicine:
		{
			int nDetailType;
			nDetailType = g_Random(medicine_detailnum);
			Gen_Medicine(nDetailType, nLevel, g_SubWorldSet.GetGameVersion(), pItem,nStackNum);
		}
		break;
	case item_task:
		{
			Gen_Quest(nSeriesReq, pItem,nStackNum,nLevel);			
		}
		break;
	case item_townportal:
		{
			Gen_TownPortal(pItem);
		}
		break;
	case item_mine:
		{
			int nParticularType = g_Random(500);
			int nJaryLevel[6];
			Gen_Mine(nParticularType, pItem,nStackNum,nLevel,nSeriesReq,nLucky,nJaryLevel);			
		}
		break;
	default:
		break;
	}
	
	return TRUE;
} */

/******************************************************************************
	功能：	生成装备  ---用于拍卖行

	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出 商店 --怪物掉落装备
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_paiEquipment(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* pnaryRMALevel,\
								   IN UINT nSeed,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nPoint,\
								   IN int nRongpoint,\
								   IN int IsWhere,IN BOOL nIsShop,\
								   IN int nPrice,IN int nFsxu,int nIsBang,\
								   IN int nPriceType,IN int nLucky)
{

	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if  (this==NULL || !pItem==NULL)
		return false;

	BOOL bEC = FALSE;

#ifdef _SERVER	// 服务器端版本
	return bEC;
#endif

#ifndef _SERVER	// 客户端版本

	pItem->m_GeneratorParam.uRandomSeed = nSeed;

	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);       //设置客户端的随机种子

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
	if (pnaryRMALevel)
		memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, pnaryRMALevel, sizeof(int) * 6);


	/*for (int k = 0;k < 6;k++)
	{ 
		char nMsgInfo[64]={0};
		if (pItem->m_GeneratorParam.nRGeneratorLevel[k] > 0)
		{ 
			sprintf(nMsgInfo,"nRGenerator:%d",pItem->m_GeneratorParam.nRGeneratorLevel[k]);
		}
		if (pItem->m_GeneratorParam.nGeneratorLevel[k] > 0)
		{ 
			sprintf(nMsgInfo,"nGenerat--:%d",pItem->m_GeneratorParam.nGeneratorLevel[k]);
		}
	}
	return bEC;*/

	pItem->m_GeneratorParam.nLuck = nLucky;
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();//产生这个装备的幸运值 存档
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	// 根据入口参数, 确定装备的基本数据
    int i = nParticularType * 10 + nLevel - 1;// 本公式由策划定义
												// 详见SPE 2002年8月31日7:40
												// email: 基础道具数值表
												// 等级数据从1开始，所以要减回来
	if (nDetailType==equip_mask)
	{
		i = nParticularType;
	}

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{//获取原始装备属性
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// 面具
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	// 披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;
	case equip_yinjian:	// 印鉴
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// 饰品
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}

	if (NULL == pEqu)
	{//获取原始数据失败
	  //_ASSERT(FALSE);
	  return NULL; 
	}  
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq);
	pItem->SetModel(nPriceType);

	// 确定装备的魔法属性 一般装备
	if (nPoint == 0)  //白装 或 蓝装  打孔
	{
		if (NULL == pnaryMALevel)
		    return NULL;

		KItemNormalAttrib	sMA[6];	// 道具的魔法属性
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
		   pItem->SetAttrib_MA(sMA);  //设置魔法属性
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		   pItem->EnChance(nEnChance);

		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
	           pItem->SetPerPrice(nPrice);

			pItem->SetBang(nIsBang);
		}

		///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件

		if (IsWhere>0)   //蓝装的辨识属性 否则就是 蓝装的熔炼属性
			nRongpoint = IsWhere;

		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongpoint)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);

				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0] = 0;
			sMAR[r].nValue[1] = 0;
			sMAR[r].nValue[2] = 0;
		} 

		MagicTabR.Clear();

		pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetIsWhere(IsWhere); 
		pItem->SetRPoint(nRongpoint); 		
	    return bEC;
	}
	else if (nPoint ==7) 
	{
		KItemNormalAttrib sMA[6];
		ZeroMemory(sMA,sizeof(sMA)); //分配内存
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;

				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);

				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);

				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

                if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;

						nRows = nMax+IsWhere;
						if (nRows>nMin)
                            nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}
                
				continue;
			}
			//剩下的设定为空孔
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		pItem->SetAttrib_MA(sMA);  //设置魔法属性
		pItem->SetPoint(nPoint);   //赋值给 Point 	
		pItem->SetIsWhere(IsWhere);
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		pItem->EnChance(nEnChance);	
		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
				pItem->SetPerPrice(nPrice);
			pItem->SetBang(nIsBang);
		}
		tabMagicAttribe.Clear();
		return bEC;
	}
	else if (nPoint ==8) 
	{
		int bECR=0;

		///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICATTRIB_PATH);  //熔炼蓝装备魔法属性文件

		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongpoint)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);

				if (sMAR[r].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						nRows = sMAR[r].nValue[2]+IsWhere;
						if (nRows>nMinR)
							nRows=nMinR;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMAR[r].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 

						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMAR[r].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
					nJnLevel.Clear();
				} 
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0]   = 0;
			sMAR[r].nValue[1]   = 0;
			sMAR[r].nValue[2]   = 0;
			//  bEC=TRUE;
		} 

		MagicTabR.Clear();

		bECR= pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongpoint); 		

		if (nIsShop)
		{
			if (nFsxu>0)
				pItem->SetPerXu(nFsxu);
			if (nPrice>0)
				pItem->SetPerPrice(nPrice);

			pItem->SetBang(nIsBang);
		}
		//		pItem->EnChanceRong(nEnChance);
		return bECR;
	}
	else //紫装 镶套
	{
		KItemNormalAttrib sMA[6];
		ZeroMemory(sMA,sizeof(sMA)); //分配内存
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		int nType,nMax,nMin,j;
		for (j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;

						nRows = nMax+IsWhere;
						if (nRows>nMin)
							nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 

						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
					nJnLevel.Clear();
				}

				continue;
			}
			//剩下的设定为空孔
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		pItem->SetAttrib_MA(sMA);  //设置魔法属性
		pItem->SetPoint(nPoint);   //赋值给 Point 判断是否是紫装		
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);	

		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
				pItem->SetPerPrice(nPrice);

			pItem->SetBang(nIsBang);
		}

		//----------------------纹钢的属性
		///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];  // 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件		
		int nTypeR,nMaxR,nMinR;

		for (j = 0;j < 6;j++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[j] > 0 && j < nRongpoint)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],7,0,&nMinR);
				sMAR[j].nAttribType = nTypeR;  //属性类型
				sMAR[j].nValue[0] = GetRandomNumber(nMaxR,nMinR);//nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],9,0,&nMinR);
				sMAR[j].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				//sMAR[j].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],11,0,&nMinR);
				//sMAR[j].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[j].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[j].nAttribType = 0;
			sMAR[j].nValue[0] = 0;
			sMAR[j].nValue[1] = 0;
			sMAR[j].nValue[2] = 0;
		} 

		MagicTabR.Clear();
		bEC=pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongpoint); 

	    return bEC;
	}
#endif

}

/******************************************************************************
	功能：	生成随机装备  ---用于商城或商店

	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出 商店 --怪物掉落装备
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_Equipment(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nPoint,\
								   IN int nRongpoint,\
								   IN int IsWhere,IN BOOL nIsShop,\
								   IN int nPrice,IN int nFsxu,int nIsBang,int nPriceType)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if  (this==NULL || pItem==NULL)
			return false;

	BOOL bEC = FALSE;
#ifdef _SERVER	// 服务器版本
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;	                 //产生这个装备的幸运值 存档
#else												
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);       //设置客户端的随机种子
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
#endif
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	// 根据入口参数, 确定装备的基本数据
    int i = nParticularType * 10 + nLevel - 1;// 本公式由策划定义
												// 详见SPE 2002年8月31日7:40
												// email: 基础道具数值表
												// 等级数据从1开始，所以要减回来
	if (nDetailType==equip_mask)
	{
		i = nParticularType;
	}

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{//获取原始装备属性
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// 面具
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	// 披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;
	case equip_yinjian:	// 印鉴
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// 饰品
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}

	if (NULL == pEqu)
	{//获取原始数据失败
	  //_ASSERT(FALSE);
	  return NULL; 
	}  
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq);
	pItem->SetModel(nPriceType);

	// 确定装备的魔法属性 一般装备
	if (nPoint == 0)  //白装 或 蓝装  打孔
	{
		if (NULL == pnaryMALevel)
		return NULL;
		KItemNormalAttrib	sMA[6];	// 道具的魔法属性
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
		   pItem->SetAttrib_MA(sMA);  //设置魔法属性
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		   pItem->EnChance(nEnChance);

		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
	           pItem->SetPerPrice(nPrice);

			pItem->SetBang(nIsBang);
		}
	    return bEC;
	}
	else if (nPoint ==7) 
	{
		KItemNormalAttrib sMA[6];
		ZeroMemory(sMA,sizeof(sMA)); //分配内存
		
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		
		if (nLucky > 10)//0--10之间
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		
		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;

				sMA[j].nValue[0] = nMax +((nMin - nMax) * nLucky / 10);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);

				sMA[j].nValue[1] = nMax +((nMin - nMax) * nLucky / 10);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);

				sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);

                if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;

						nRows = nMax+IsWhere;
						if (nRows>nMin)
                            nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}
                
				continue;
			}
			//剩下的设定为空孔
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		pItem->SetAttrib_MA(sMA);  //设置魔法属性
		pItem->SetPoint(nPoint);   //赋值给 Point 	
		pItem->SetIsWhere(IsWhere);
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		pItem->EnChance(nEnChance);	
		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
				pItem->SetPerPrice(nPrice);
			pItem->SetBang(nIsBang);
		}
		tabMagicAttribe.Clear();
		return bEC;
	}
	else //紫装 镶套
	{
		KItemNormalAttrib sMA[6];
		ZeroMemory(sMA,sizeof(sMA)); //分配内存

		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);

		if (nLucky > 10)//0--10之间
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				continue;
			}
			//剩下的设定为空孔
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		pItem->SetAttrib_MA(sMA);  //设置魔法属性
		pItem->SetPoint(nPoint);   //赋值给 Point 判断是否是紫装		
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);	

		if (nIsShop)
		{
			if (nFsxu>0)
			   pItem->SetPerXu(nFsxu);
			if (nPrice>0)
				pItem->SetPerPrice(nPrice);

			pItem->SetBang(nIsBang);
		}
	    return bEC;
	}
}
/******************************************************************************
功能：	生成魔法属性
入口:	nType: 装备类型
pnaryMALevel: 数组指针, 给出魔法的等级要求, 共6个
nSeriesReq: 五行属性
nLucky: 运气值
出口:	在 pnaryMA 所指数组中返回魔法的属性核心数据, 共6个
******************************************************************************/
// Add by Freeway Chen in 2003.5.30
BOOL KItemGenerator::Gen_MagicAttrib(int nType, const int* pnaryMALevel,\
									         int nSeriesReq, int nLucky,\
						   KItemNormalAttrib* pnaryMA, int nGameVersion)
{
    int nResult = false;

    KBPT_ClassMAIT  SelectedMagicTable;	
	// 记录每一个记录选中的魔法属性，主要是为了最后将使用标志清除
    KMAGICATTRIB_TABFILE *pMagicAttrTable[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

    int i = 0;

    //_ASSERT(pnaryMALevel);
    //_ASSERT(pnaryMA);
    if  (pnaryMALevel==NULL || pnaryMA==NULL)
    		return false;

    for (i = 0; i < 6; ++i) 
    {
        // 如果发现魔法属性为零，表示不需要继续处理了，退出 
        if (pnaryMALevel[i] == 0)	
        	break;

		/*if (pnaryMALevel[i]-10 >0)	
        {//特殊属性
		int nnMgaRows = pnaryMALevel[i]-10;
			//pnaryMA[i].nAttribType = pnaryMALevel[i];
		 	
			KTabFile tabMagicAttribe;
			  tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);

			  
		    int nType,nMax,nMin;
	
			tabMagicAttribe.GetInteger(nnMgaRows,5,0,&nType);
			tabMagicAttribe.GetInteger(nnMgaRows,6,0,&nMax);
		    tabMagicAttribe.GetInteger(nnMgaRows,7,0,&nMin);
		    pnaryMA[i].nAttribType = nType;
		    pnaryMA[i].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				
		    tabMagicAttribe.GetInteger(nnMgaRows,8,0,&nMax);
			tabMagicAttribe.GetInteger(nnMgaRows,9,0,&nMin);
		    pnaryMA[i].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				  
			tabMagicAttribe.GetInteger(nnMgaRows,10,0,&nMax);
			tabMagicAttribe.GetInteger(nnMgaRows,11,0,&nMin);
			pnaryMA[i].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
			
			tabMagicAttribe.Clear();
					  
			if (pnaryMA[i].nValue[2]>1)
			{ 
				nnMgaRows =	pnaryMA[i].nValue[2];

				KTabFile nmpLevel;
				if (nmpLevel.Load(TABFILE_MAGICAGOLD_PATH))
				{  
					int nMinVal=0,nMaxVal=0;
	
					nmpLevel.GetInteger(nnMgaRows,"参数1最小值",0,&nMinVal);
					nmpLevel.GetInteger(nnMgaRows,"参数1最大值",0,&nMaxVal);
					pnaryMA[i].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						  
					nmpLevel.GetInteger(nnMgaRows,"参数3最小值",0,&nMinVal);
					nmpLevel.GetInteger(nnMgaRows,"参数3最大值",0,&nMaxVal);
					pnaryMA[i].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
				}
				nmpLevel.Clear();
			}
		  
		  continue;
		}
	    */
        SelectedMagicTable.Clear();

        // 根据前缀后缀、物品类型、五行需求、需求级别将所有符合条件的魔法属性项取出来
        const KBPT_ClassMAIT *pCMITItem = m_BPTLib.GetCMIT(
            1 - (i & 1),        // 前缀后缀	明暗属性
            nType,              // 物品类型 武器  帽子 戒子等
            nSeriesReq,         // 五行需求 
            pnaryMALevel[i]     // 魔法属性的项  1-10  属性等级
        );
			
        //_ASSERT(pCMITItem);
        if (!pCMITItem)
               break;
        // 遍历符合条件的魔法属性，根据
        // 1.是否已经使用，
        // 2.运气值
        // 3.是否有相同的“属性调整类别”
        // 取出相应的项放在SelectedMagicTable中
        int nCMITItemCount = pCMITItem->GetCount();  //获取符合条件的魔法属性数量
        int j = 0; 
		int nMinDropRate,nMaxDropRate;
        g_GameSetTing.GetInteger2("SYSTEM","ItemDropRate",&nMinDropRate,&nMaxDropRate);

        int nDecide = GetRandomNumber(nMinDropRate,nMaxDropRate-1)/(1 + nLucky * 20/100);  //运气值 10000 500000=25/1250||3000/160000
		for (j = 0; j < nCMITItemCount; ++j)
        {//历遍属性数量
            // 根据索引值，取出相应的项
			//int nIsThis=GetRandomNumber(0,nCMITItemCount-1); //随机取一条属性索引
            int nMAIndex = pCMITItem->Get(j);  //获取魔法属性种类的 索引
            KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(nMAIndex);
            if (!pMAItem)
            {
               // _ASSERT(pMAItem);
                continue;
            }

            // 如果已经使用，跳过取下一个
            if (pMAItem->m_nUseFlag)
                continue;

            // 如果运气不足，取下一个
            if ((pMAItem->m_DropRate[nType]) <=1 || (pMAItem->m_DropRate[nType]) <= nDecide) //25/1250||3000/160000  10--2000
                continue; //停往下执行，执行下一个循环（从头开始for） 
            
            // 检查是否已经有相同的“魔法属性的种类”
            int k = 0;
            for (k = 0; k < i; ++k)
            {
                //_ASSERT(pMagicAttrTable[k]);
                if (!pMagicAttrTable[k])
                	continue;

                if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))
                    break;  //相同则跳出
            }

            // 如果发现 “属性调整类别”相同，说明已经有一个相同的值被选中，那么取下一个
            if (k < i)
                continue;

            SelectedMagicTable.Insert(nMAIndex);  //选择插入 这个 魔法属性种类
        }

        int nSelectedCount = SelectedMagicTable.GetCount();   //获取已经加入属性的数量

        if (nSelectedCount == 0)
        {   
            // 如果没有满足条件的魔法属性，就退出
            break;
        }
        
        // 如果发现满足条件的魔法属性，就设置
        int nLuckyItemIndex = GetRandomNumber(0,nSelectedCount-1);//取符合的随机一个
        KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(SelectedMagicTable.Get(nLuckyItemIndex));
        // 保存满足条件的魔法属性，以便在最后的时候将使用标志清除
        pMagicAttrTable[i] = pMAItem;

        //_ASSERT(pMAItem);
        //_ASSERT(!(pMAItem->m_nUseFlag));     // 确定还没有使用
        if (pMAItem==NULL)
        	break;
		if (pMAItem->m_nUseFlag)
			break;
        // 设置使用标志
        pMAItem->m_nUseFlag = true;
        pnaryMA[i].nAttribType = pMAItem->m_MagicAttrib.nPropKind;  //魔法属性的种类
        pnaryMA[i].nValue[0] = GetRandomNumber(pMAItem->m_MagicAttrib.aryRange[0].nMin,pMAItem->m_MagicAttrib.aryRange[0].nMax);
        pnaryMA[i].nValue[1] = GetRandomNumber(pMAItem->m_MagicAttrib.aryRange[1].nMin,pMAItem->m_MagicAttrib.aryRange[1].nMax);
        pnaryMA[i].nValue[2] = GetRandomNumber(pMAItem->m_MagicAttrib.aryRange[2].nMin,pMAItem->m_MagicAttrib.aryRange[2].nMax);

		if (pnaryMA[i].nValue[2]>1)
		{
			KTabFile nJnLevel;
			if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
			{  
				int nMinVal=0,nMaxVal=0;

				nJnLevel.GetInteger(pnaryMA[i].nValue[2],"参数1最小值",0,&nMinVal);
                nJnLevel.GetInteger(pnaryMA[i].nValue[2],"参数1最大值",0,&nMaxVal);
                pnaryMA[i].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 

                nJnLevel.GetInteger(pnaryMA[i].nValue[2],"参数3最小值",0,&nMinVal);
                nJnLevel.GetInteger(pnaryMA[i].nValue[2],"参数3最大值",0,&nMaxVal);
                pnaryMA[i].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
			}
            nJnLevel.Clear();
		}

    } // for 遍历需要取回的魔法属性表

    for (NULL; i < 6; ++i)
    {
        // 清空剩下的项
        pnaryMA[i].nAttribType = 0;
        pnaryMA[i].nValue[0] = 0;
        pnaryMA[i].nValue[1] = 0;
        pnaryMA[i].nValue[2] = 0;
    }

    for (i = 0; i < 6; ++i)
    {
        if (!pMagicAttrTable[i])
            break;

       //_ASSERT(pMagicAttrTable[i]->m_nUseFlag);   // 确定已经使用
        //if (!pMagicAttrTable[i]->m_nUseFlag)
    	//  ccontinue;

        pMagicAttrTable[i]->m_nUseFlag = false;    // 设置为没用使用
    }

    nResult = true;
	return nResult;
}

// Following Code is implement by DongBo
//BOOL KItemGenerator::Gen_MagicAttrib(int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
//									 KItemNormalAttrib* pnaryMA)
//{
//	
//    // 确定适用于该装备的全部魔法属性及其数量
//	if (FALSE == GMA_GetAvaliableMA(nType))
//		{ _ASSERT(FALSE); return FALSE; }
//
//	// 确定上述各魔法属性的等级,出现概率
//	if (FALSE == GMA_GetLevelAndDropRate(nType))
//		{ _ASSERT(FALSE); return FALSE; }
//
//	// 从上述魔法属性中选出符合条件的. 策划要求选出6个
//	for (int i = 0; i < 6; ++i)				// 奇数: 前缀, 偶数: 后缀
//	{
//		GMA_ChooseMA((i&1), pnaryMALevel[i], nLucky, &(pnaryMA[i]));
//		if (pnaryMA[i].nAttribType == 0)
//			break;
//	}
//	return TRUE;
//}

/******************************************************************************
	功能:	确定适用于指定装备的全部魔法属性及其数量
	入口:	nType: 指定装备
	出口:	成功时返回非零. m_GMA_naryMA[0]给出适用于该装备的全部魔法属性前缀
							m_GMA_nCount[0]给出上述魔法属性的总数
							m_GMA_naryMA[1]给出适用于该装备的全部魔法属性后缀
							m_GMA_nCount[1]给出上述魔法属性的总数
			失败时返回零
******************************************************************************/
/*BOOL KItemGenerator::GMA_GetAvaliableMA(int nType)
{
	//TODO: 可能需要对nType进行转换, 从装备类型值变成 m_BPTLib.m_CMAT 数组的索引
	for (int i = 0; i < 2; ++i)		// 0: 前缀, 1: 后缀
	{
		const KBPT_ClassifiedMAT* pCMAT;
		pCMAT = m_BPTLib.GetCMAT(i, nType);
		if (pCMAT == NULL)
			{ _ASSERT(FALSE); return FALSE; }
		m_GMA_nCount[i] = NUMOFCMA;
		pCMAT->GetAll((int*)m_GMA_naryMA[i], &(m_GMA_nCount[i]));
	}
	return TRUE;
}
*/
/******************************************************************************
	功能:	确定各给定魔法属性的等级要求和出现概率
	入口:	nType: 指定装备
			m_GMA_naryMA数组: 给出全部魔法属性的索引值
	出口:	成功时返回非零. m_GMA_naryLevel数组给出上述魔法属性的等级要求
							m_GMA_naryDropRate给出上述魔法属性的出现概率
			失败时返回零
******************************************************************************/
/*BOOL KItemGenerator::GMA_GetLevelAndDropRate(int nType)
{
	//TODO: 可能需要对nType进行转换, 从装备类型值变成 m_BPTLib.m_CMAT 数组的索引

	for (int i = 0; i < 2; ++i)		// 0: 前缀, 1: 后缀
	{
		for (int n = 0; n < m_GMA_nCount[i]; ++n)
		{
			const KMAGICATTRIB_TABFILE* pMATF;
			pMATF = GetMARecord(m_GMA_naryMA[i][n]);
			if (pMATF == NULL)
				{ _ASSERT(FALSE); return FALSE; }
			m_GMA_naryLevel[i][n] = pMATF->m_nLevel;
			m_GMA_naryDropRate[i][n] = pMATF->m_DropRate[nType];		// 可优化
		}
	}
	return TRUE;
}
/*
/******************************************************************************
	功能:	从全部可用的魔法属性中选出符合条件的魔法
	入口:	nPos: 0: 选前缀, 1: 选后缀
			nLevel: 对魔法属性的等级要求
			nLucky: 运气值
			m_GMA_naryMA数组: 给出全部可用的魔法属性的索引值
	出口:	*pINA 给出所选魔法的核心参数
******************************************************************************/
/*void KItemGenerator::GMA_ChooseMA(int nPos, int nLevel, int nLucky, KItemNormalAttrib* pINA)
{
	_ASSERT(pINA != NULL);

	// 赋初值
	pINA->nAttribType = 0;
	pINA->nValue[0] = 0;
	pINA->nValue[1] = 0;
	pINA->nValue[2] = 0;

	// 筛选出全部可用的魔法属性
	int nDropRate = ::GetRandomNumber(0, 100) - nLucky;
	int nCount = GMA_GetCandidateMA(nPos, nLevel, nDropRate);
	if (nCount > 0)
	{
		int nFinal = ::GetRandomNumber(0, nCount);
		int nMAi = m_GMA_naryCandidateMA[nFinal];
		int nMA = m_GMA_naryMA[nPos][nMAi];
		if (nMA == -1)	// 随机出重复的属性了
		{
			pINA->nAttribType = 0;
			return;
		}
		m_GMA_naryMA[nPos][nMAi] = -1;		// 置为-1,表示该MA已用

		const KMAGICATTRIB_TABFILE* pMATF = GetMARecord(nMA);
		if (NULL != pMATF)
		{
			const KMACP* pMACP = &(pMATF->m_MagicAttrib);
			pINA->nAttribType = pMACP->nPropKind;
			pINA->nValue[0] = ::GetRandomNumber(pMACP->aryRange[0].nMin, pMACP->aryRange[0].nMax);
			pINA->nValue[1] = ::GetRandomNumber(pMACP->aryRange[1].nMin, pMACP->aryRange[1].nMax);
			pINA->nValue[2] = ::GetRandomNumber(pMACP->aryRange[2].nMin, pMACP->aryRange[2].nMax);
		}
	}
}
*/
/******************************************************************************
	功能:	获取指定的魔法属性记录
	入口:	i: 指定记录
	出口:	成功时返回指向该记录的指针
			失败时返回NULL
******************************************************************************/
const KMAGICATTRIB_TABFILE* KItemGenerator::GetMARecord(int i) const
{
	return m_BPTLib.GetMARecord(i);
}

/******************************************************************************
	功能:	从全部可用的魔法属性中选出符合条件的魔法
	入口:	nPos: 0: 选前缀, 1: 选后缀
			nLevel: 对魔法属性的等级要求
			nDropRate: 选出出现概率大于此值的魔法属性
			m_GMA_naryMA数组: 给出全部可用的魔法属性的索引值
			m_GMA_naryLevel数组: 给出上述魔法属性的等级要求
			m_GMA_naryDropRate数组: 给出上述魔法属性的出现概率
	出口:	m_GMA_naryCandidateMA数组给出符合条件的魔法
	说明:	若 n = m_GMA_naryMA[nPos][m_GMA_naryCandidateMA[i]]
			则 GetMARecord(n) 返回指向魔法属性结构的指针
******************************************************************************/
/*int KItemGenerator::GMA_GetCandidateMA(int nPos, int nLevel, int nDropRate)
{
	int nCount = 0;
	for (int i = 0; i < m_GMA_nCount[nPos]; ++i)
	{
		if (m_GMA_naryMA[nPos][i] != -1 &&		// -1表示该MA已用过
			m_GMA_naryLevel[nPos][i] == nLevel &&
			m_GMA_naryDropRate[nPos][i] >= nDropRate)
		{
			m_GMA_naryCandidateMA[nCount++] = i;
		}
	}
	return nCount;
}
 */
BOOL KItemGenerator::GetMedicineCommonAttrib(IN int nDetailType, IN int nLevel, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
//		KASSERT(FALSE);
		return FALSE;
	}

	const int i = nDetailType * 5 + nLevel - 1;

	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		//_ASSERT(FALSE);
		return FALSE;
	}
	*pItem = *pMed;
	return TRUE;
}

/******************************************************************************
	功能：	随机生成黄金装备装备
	入口:	nCondition: 前提条件，(Emf+Pmf)*Level的运算结果
			pItem: 装备项目指针。
	出口:	成功时返回真, 相关数据在 pItem 所指对象中给出
			失败时返回假
******************************************************************************/
// Add by flying on May.30.2003
// Try to get a "Gold Item" if possible.
/*BOOL KItemGenerator::GetGoldItemByRandom(IN int nCondition, OUT KItem * pItem)
{
	BOOL bResult = FALSE;
	const KBASICPROP_EQUIPMENT_GOLD* pArray[MAX_ITEM];
	int i = 0;
	int j = 0;
	int nIndex = 0;

	::memset(pArray, 0, sizeof(KBASICPROP_EQUIPMENT_GOLD *) * MAX_ITEM);

	for (i = 0; i < MAX_ITEM; ++i)
	{
		const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(i);  //获取黄金装备的属性
		if (pTemp == 0)
			break;
//		if (pTemp->m_nRarity < nCondition)
			pArray[j] = pTemp;
	}

	if (j == 0)
		goto Exit0;			 
	nIndex = GetRandomNumber(0, j);	
	*pItem = *pArray[nIndex];
Exit0:
	return FALSE;
} */
//刷白金装备
BOOL KItemGenerator::GetPlatinaByIndex(IN int nIndex, OUT KItem * pItem, IN const int* pnay, IN int nSeries, IN int nEnChance,IN int nRongpiont,IN int nLevel,IN BOOL nIsLogin)
{
	int i = 0;
	int j = 0;
	int nCount = 0;
	
#ifdef _SERVER	// 服务器版本
	if (nIsLogin)
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	else
	    pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();

	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < 6;++i)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10); //取随机值
		}
	}
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 10;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < 6;++i)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10); //取随机值
		}
	}
#endif
	
	nCount = m_BPTLib.GetPlatinaNumber();// 属性表含有多少项数据
	const KBASICPROP_EQUIPMENT_PLATINA* pTemp = m_BPTLib.GetPlatinaRecord(nIndex); //获取黄金原始属性
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;

	pItem->SetAttrib_BCBR((KBASICPROP_EQUIPMENT_PLATINA*)pTemp); //设置基本属性和 需求属性
	pItem->SetGoldId(nIndex);
	pItem->SetLevel(nLevel);
	
	if (nSeries<0 || nSeries > 4)
	{
		pItem->SetSeries(::GetRandomNumber(0,4)); //设置五行
	} 
	else
	{
		pItem->SetSeries(nSeries);
	}
	
	KItemNormalAttrib	sMA[6];
	KTabFile MagicTab;
	MagicTab.Load(TABFILE_MAGICAGOLD_PATH);   //黄金装备魔法属性文件
	for (i = 0; i < 6; ++i)
	{
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pTemp->m_aryMagicAttribs[i]); //技能编号
		pDst = &(sMA[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
		pDst->nAttribType = nType;
		MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
		//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[0] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
		//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[1] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
		//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[2] =  GetRandomNumber(nLow,nHigh);
	}
	
	pItem->SetAttrib_MA(sMA);  //赋值给 属性数组 ---m_aryMagicAttrib[i]  ,设置魔法属性
    pItem->SetRPoint(nRongpiont);
	pItem->EnChance(nEnChance);

	KItemNormalAttrib	syMA[2];
	for (i = 0; i < 2; ++i)
	{//隐藏属性
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pTemp->m_yinMagicAttribs[i]); //技能编号
		pDst = &(syMA[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
		pDst->nAttribType = nType;	//类型
		MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
		//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[0] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
		//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[1] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
		//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[2] =  GetRandomNumber(nLow,nHigh);
	}
	pItem->SetAttrib_Yin(syMA); 
	MagicTab.Clear();
	return TRUE;
}


/******************************************************************************
	功能：	生成指定黄金装备装备   ---刷黄金装备  
	入口:	nCondition: 前提条件，(Emf+Pmf)*Level的运算结果
			pItem: 装备项目指针。
	出口:	成功时返回真, 相关数据在 pItem 所指对象中给出
			失败时返回假
******************************************************************************/
BOOL KItemGenerator::GetGoldItemByIndex(IN int nIndex, OUT KItem * pItem, IN const int* pnay, IN int nSeries, IN int nEnChance,IN int nRongpiont,IN int nLevel,IN BOOL nIsLogin)
{
	int i = 0;
	int j = 0;
	int nCount = 0;

#ifdef _SERVER	// 服务器版本
	if (nIsLogin)
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	else
	   pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();

	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < 6;++i)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10); //取随机值
		}
	}
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 10;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < 6;++i)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10); //取随机值
		}
	}
#endif

	nCount = m_BPTLib.GetGoldItemNumber();// 属性表含有多少项数据
	const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(nIndex); //获取黄金原始属性
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_GOLD*)pTemp); //设置基本属性和 需求属性
	if (nSeries > 4 || nSeries < 0)
	{
		pItem->SetSeries(::GetRandomNumber(0,4)); //设置五行
	} 
	else
	{
		pItem->SetSeries(nSeries);
	}

	pItem->SetLevel(nLevel);

	KItemNormalAttrib	sMA[6];
	KTabFile MagicTab;
	MagicTab.Load(TABFILE_MAGICAGOLD_PATH);   //黄金装备魔法属性文件
	for (i = 0; i < 6; ++i)
	{
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pTemp->m_aryMagicAttribs[i]); //技能编号
		pDst = &(sMA[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
		pDst->nAttribType = nType;
		MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
		//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[0] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
		//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[1] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
		//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[2] =  GetRandomNumber(nLow,nHigh);
	}

	pItem->SetAttrib_MA(sMA);  //赋值给 属性数组 ---m_aryMagicAttrib[i]  ,设置魔法属性
    pItem->SetRPoint(nRongpiont);
	pItem->EnChance(nEnChance);


	KItemNormalAttrib	syMA[2];
	for (i = 0; i < 2; ++i)
	{//隐藏属性
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pTemp->m_yinMagicAttribs[i]); //技能编号
		pDst = &(syMA[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
		pDst->nAttribType = nType;	//类型
		MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
		//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[0] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
		//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[1] =  GetRandomNumber(nLow,nHigh);
		MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
		//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		pDst->nValue[2] =  GetRandomNumber(nLow,nHigh);
	}
	pItem->SetAttrib_Yin(syMA); 
	MagicTab.Clear(); 
	return TRUE;
}

/****************************************************************************
	功能：	生成指定熔炼的白金装备装备 属性
	入口:	nDetailType: 具体类别, 如近程武器、暗器、......
			nParticularType: 详细类别
			nSeriesReq: 五行属性
			nLevel: 等级
			pnaryMALevel: 魔法属性等级数组[6]
			nLucky: 运气值
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::GetPlatinaItemByRong(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* rongALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nRongID,\
								   IN int Goidid,\
								   IN const int* BaoMagLevel,IN BOOL nIsLogin)
{
	
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	  if  (this==NULL || pItem==NULL)
	    		return false;

    int nCount = 0;
	BOOL bEC = FALSE;
	
#ifdef _SERVER	// 服务器版本
	if (nIsLogin)
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	else
	   pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //基本魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
    else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

/*	if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6); */

	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
    
#else //客户端
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel,pnaryMALevel, sizeof(int) * 6);  //物品属性更新参数
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
         memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	/*if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6);*/
#endif
		
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	nCount = m_BPTLib.GetPlatinaNumber();// 装备TXT表含有多少项数据

	const KBASICPROP_EQUIPMENT_PLATINA* pTemp = m_BPTLib.GetPlatinaRecord(Goidid); //获取白金原始属性

	if (Goidid >= nCount-1)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
	if (pTemp == NULL)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_BCBR((KBASICPROP_EQUIPMENT_PLATINA*)pTemp);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq); 

	pItem->SetGoldId(Goidid);
	pItem->SetLevel(nLevel);

		if (nLucky > 10)
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		KItemNormalAttrib	sMA[6];  //魔法
		ZeroMemory(sMA,sizeof(sMA)); //分配内存
		KTabFile MagicTab;
		MagicTab.Load(TABFILE_MAGICAGOLD_PATH);   //装备魔法属性文件
		int i;
	     	for (i = 0;i< 6;++i)
			{ 
			const int* pSrc;
			KItemNormalAttrib* pDst;
			pSrc = &(pTemp->m_aryMagicAttribs[i]); //原始的技能编号
			pDst = &(sMA[i]);		
			int nType,nLow,nHigh;
			MagicTab.GetInteger(*pSrc + 1,5,0,&nType);
			pDst->nAttribType = nType;
			MagicTab.GetInteger(*pSrc + 1,6,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,7,0,&nHigh);
			//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[0] =  GetRandomNumber(nLow,nHigh);
			MagicTab.GetInteger(*pSrc + 1,8,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,9,0,&nHigh);
			//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[1] =  GetRandomNumber(nLow,nHigh);
			MagicTab.GetInteger(*pSrc + 1,10,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,11,0,&nHigh);
			//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[2] =  GetRandomNumber(nLow,nHigh);
			} 

            pItem->SetAttrib_MA(sMA);
			//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
            pItem->EnChance(nEnChance); //重新赋值？强化

//==========================以下为镶套纹钢属性===================================
            KItemNormalAttrib	sMAR[6];// 魔法
			ZeroMemory(sMAR,sizeof(sMAR)); //分配内存

			int nType,nMax,nMin;
	     	for (i = 0;i < 6;++i)
			{ 
			   if (pItem->m_GeneratorParam.nRGeneratorLevel[i] > 0 && i < nRongID)
			   { 
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],5,0,&nType);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],6,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],7,0,&nMin);
				sMAR[i].nAttribType = nType;
				//sMAR[i].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[0] =  GetRandomNumber(nMax,nMin);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],8,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],9,0,&nMin);
				//sMAR[i].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[1] =  GetRandomNumber(nMax,nMin);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],10,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],11,0,&nMin);
				//sMAR[i].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[2] =  GetRandomNumber(nMax,nMin);
				continue;
			   }
			    sMAR[i].nAttribType = 0;
				sMAR[i].nValue[0] = 0;
				sMAR[i].nValue[1] = 0;
		    	sMAR[i].nValue[2] = 0;
              //  bEC=TRUE;
			}  	
		 pItem->SetAttrib_RON(sMAR);   //设置魔法属性
		 //pItem->SetBackUpRmagicAttrib();
	     pItem->SetRPoint(nRongID); 
		 pItem->EnChanceRong(nEnChance);
		 
//==========================以下为镶套宝石属性===================================
/*         KItemNormalAttrib	sbMAR[6];// 魔法
		 ZeroMemory(sbMAR,sizeof(sbMAR)); //分配内存

		 int nbType,nbMax,nbMin;
		 for (int b = 0;b < 6;b++)
		 {
			 if (pItem->m_GeneratorParam.nBGeneratorLevel[b] > 0)
			 { 
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],5,0,&nbType);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],6,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],7,0,&nbMin);
				 sbMAR[b].nAttribType = nbType;
				 sbMAR[b].nValue[0] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],8,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],9,0,&nbMin);
				 sbMAR[b].nValue[1] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],10,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],11,0,&nbMin);
				 sbMAR[b].nValue[2] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 continue;
			 }
			 sbMAR[b].nAttribType = 0;
			 sbMAR[b].nValue[0] = 0;
			 sbMAR[b].nValue[1] = 0;
			 sbMAR[b].nValue[2] = 0;
		 }  	
		 pItem->SetAttrib_Bao(sbMAR);   //设置魔法属性
		 pItem->EnChanceBao(nEnChance);	*/
//-----------------隐藏属性------------------------------------------------------
		 KItemNormalAttrib	syMA[2];
		 for (i = 0; i < 2; ++i)
		 {//隐藏属性
			 const int* pSrc;
			 KItemNormalAttrib* pDst;
			 pSrc = &(pTemp->m_yinMagicAttribs[i]); //技能编号
			 pDst = &(syMA[i]);
			 
			 int nType,nLow,nHigh;
			 MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
			 pDst->nAttribType = nType;	//类型
			 MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
			 pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
			 pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
			 pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		 }
		 pItem->SetAttrib_Yin(syMA); 
//===============================================================================
		 MagicTab.Clear();
     	 return TRUE;
}


/****************************************************************************
	功能：	生成随机属性的黄金
	入口:	nDetailType: 具体类别, 如近程武器、暗器、......
			nParticularType: 详细类别
			nSeriesReq: 五行属性
			nLevel: 等级
			pnaryMALevel: 魔法属性等级数组[6]
			nLucky: 运气值
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::GetGoldItemByRandom(IN int Goidid,\
								   IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* rongALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nRongID,\
								   IN BOOL nIsLogin,int nPoint)
{
	
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	  if  (this==NULL || pItem==NULL)
		    		return false;
    int nCount = 0;
	BOOL bEC = FALSE;
	
#ifdef _SERVER	// 服务器版本

	if (nIsLogin)
	   g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);       //设置随机种子,装备属性就不会变
	else
	   pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //基本魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
    else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	/*if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6);*/

	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
    
#else //客户端
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel,pnaryMALevel, sizeof(int) * 6);  //物品属性更新参数
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
         memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	/*if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6);*/
#endif
		
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	nCount = m_BPTLib.GetGoldItemNumber();// 装备TXT表含有多少项数据
	const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(Goidid); //获取黄金原始属性

	if (Goidid >= nCount)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
	if (pTemp == NULL)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?

	if (NULL == pnaryMALevel)
		return TRUE;

	pItem->SetAttrib_RCBR((KBASICPROP_EQUIPMENT_GOLD*)pTemp,TRUE);  //设置基本属性 和需求属性

	KItemNormalAttrib	sMA[6];	// 道具的魔法属性
	bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);

	if (bEC)
		pItem->SetAttrib_MA(sMA);  //设置魔法属性

    pItem->EnChance(nEnChance);
	
	if (nSeriesReq > 4 || nSeriesReq < 0)
	{
		pItem->SetSeries(::GetRandomNumber(0,4)); //设置五行
	} 
	else
	{
		pItem->SetSeries(nSeriesReq);
	}

	pItem->SetLevel(nLevel);
	pItem->SetPoint(nPoint);


		if (nLucky > 10)
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
//==========================以下为镶套纹钢属性===================================
            KItemNormalAttrib	sMAR[6];   //魔法
			ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
			KTabFile MagicTab;
		    MagicTab.Load(TABFILE_MAGICAGOLD_PATH);   //黄金装备魔法属性文件
			int nType,nMax,nMin,i;
	     	for (i = 0;i < 6;++i)
			{ 
			   if (pItem->m_GeneratorParam.nRGeneratorLevel[i] > 0 && i < nRongID)
			   { 
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],5,0,&nType);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],6,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],7,0,&nMin);
				sMAR[i].nAttribType = nType;
				//sMAR[i].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[0]   =GetRandomNumber(nMax,nMin);

				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],8,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],9,0,&nMin);
				//sMAR[i].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[1]   =GetRandomNumber(nMax,nMin);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],10,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],11,0,&nMin);
				//sMAR[i].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[2]   =GetRandomNumber(nMax,nMin);
				continue;
			   }
			    sMAR[i].nAttribType = 0;
				sMAR[i].nValue[0] = 0;
				sMAR[i].nValue[1] = 0;
		    	sMAR[i].nValue[2] = 0;
              //  bEC=TRUE;
			}  	
		 pItem->SetAttrib_RON(sMAR);   //设置魔法属性
		 //pItem->SetBackUpRmagicAttrib();
	     pItem->SetRPoint(nRongID); 
		 pItem->EnChanceRong(nEnChance);
//-----------------隐藏属性------------------------------------------------------
		 KItemNormalAttrib	syMA[2];
		 for (i = 0; i < 2; ++i)
		 {//隐藏属性
			 const int* pSrc;
			 KItemNormalAttrib* pDst;
			 pSrc = &(pTemp->m_yinMagicAttribs[i]); //技能编号 行号
			 pDst = &(syMA[i]);
			 
			 int nType,nLow,nHigh;
			 MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
			 pDst->nAttribType = nType;	//类型
			 MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
			// pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 pDst->nValue[0]   = GetRandomNumber(nLow,nHigh);
			 MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
			// pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 pDst->nValue[1]   = GetRandomNumber(nLow,nHigh);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
			// pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 pDst->nValue[2]   = GetRandomNumber(nLow,nHigh);
		 }
		 pItem->SetAttrib_Yin(syMA); 
//===============================================================================
		 MagicTab.Clear();
     	 return TRUE;
}

/******************************************************************************
	功能：	生成指定熔炼的黄金装备装备 属性
	入口:	nCondition: 前提条件，(Emf+Pmf)*Level的运算结果
			pItem: 装备项目指针。
	出口:	成功时返回真, 相关数据在 pItem 所指对象中给出
			失败时返回假
****************************************************************************
// Add by flying on May.30.2003
// Try to get a "Gold Item" if possible.
BOOL KItemGenerator::GetGoldItemByRong(IN int nIndex,
									   OUT KItem * pItem, 
									   IN const int* pnay,
									   IN int nSeries, 
									   IN int nEnChance,
									   IN int nRongID)
****************************************************************************
	功能：	生成指定熔炼的黄金装备装备 属性
	入口:	nDetailType: 具体类别, 如近程武器、暗器、......
			nParticularType: 详细类别
			nSeriesReq: 五行属性
			nLevel: 等级
			pnaryMALevel: 魔法属性等级数组[6]
			nLucky: 运气值
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::GetGoldItemByRong(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* rongALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nRongID,\
								   IN int Goidid,\
								   IN const int* BaoMagLevel,IN BOOL nIsLogin)
{
	
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	  if (this==NULL || pItem==NULL)
		return false;
    int nCount = 0;
	BOOL bEC = FALSE;
	
#ifdef _SERVER	// 服务器版本

	if (nIsLogin)
	   g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);       //设置随机种子,装备属性就不会变
	else
	   pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //基本魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
    else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	/*if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6);*/

	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
    
#else //客户端
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel,pnaryMALevel, sizeof(int) * 6);  //物品属性更新参数
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
         memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	/*if (BaoMagLevel)
        memcpy(pItem->m_GeneratorParam.nBGeneratorLevel, BaoMagLevel, sizeof(int) * 6); //宝石魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nBGeneratorLevel, sizeof(int) * 6);*/
#endif
		
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	nCount = m_BPTLib.GetGoldItemNumber();// 装备TXT表含有多少项数据
	const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(Goidid); //获取黄金原始属性
	if (Goidid >= nCount)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
	if (pTemp == NULL)
	{
		//_ASSERT(FALSE);
	     return bEC; 
	}
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_RCBR((KBASICPROP_EQUIPMENT_GOLD*)pTemp);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq); 
	pItem->SetLevel(nLevel);

		if (nLucky > 10)
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

		KItemNormalAttrib	sMA[6];// 魔法
		ZeroMemory(sMA,sizeof(sMA)); //分配内存
		KTabFile MagicTab;
		MagicTab.Load(TABFILE_MAGICAGOLD_PATH);   //黄金装备魔法属性文件
		int i;
	     	for (i = 0;i< 6;++i)
			{ 
			const int* pSrc;
			KItemNormalAttrib* pDst;
			pSrc = &(pTemp->m_aryMagicAttribs[i]); //原始的技能编号
			pDst = &(sMA[i]);		
			int nType,nLow,nHigh;
			MagicTab.GetInteger(*pSrc + 1,5,0,&nType);
			pDst->nAttribType = nType;
			MagicTab.GetInteger(*pSrc + 1,6,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,7,0,&nHigh);
			//pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[0]   =GetRandomNumber(nLow,nHigh);
			MagicTab.GetInteger(*pSrc + 1,8,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,9,0,&nHigh);
			//pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[1]   =GetRandomNumber(nLow,nHigh);
			MagicTab.GetInteger(*pSrc + 1,10,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1,11,0,&nHigh);
			//pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			pDst->nValue[2]   =GetRandomNumber(nLow,nHigh);
			} 

            pItem->SetAttrib_MA(sMA);
			//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
            pItem->EnChance(nEnChance); //重新赋值？强化

//==========================以下为镶套纹钢属性===================================
            KItemNormalAttrib	sMAR[6];   //魔法
			ZeroMemory(sMAR,sizeof(sMAR)); //分配内存

			int nType,nMax,nMin;
	     	for (i = 0;i < 6;++i)
			{ 
			   if (pItem->m_GeneratorParam.nRGeneratorLevel[i] > 0 && i < nRongID)
			   { 
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],5,0,&nType);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],6,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],7,0,&nMin);
				sMAR[i].nAttribType = nType;
				//sMAR[i].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[0]   =GetRandomNumber(nMax,nMin);

				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],8,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],9,0,&nMin);
				//sMAR[i].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[1]   =GetRandomNumber(nMax,nMin);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],10,0,&nMax);
				MagicTab.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[i],11,0,&nMin);
				//sMAR[i].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMAR[i].nValue[2]   =GetRandomNumber(nMax,nMin);
				continue;
			   }
			    sMAR[i].nAttribType = 0;
				sMAR[i].nValue[0] = 0;
				sMAR[i].nValue[1] = 0;
		    	sMAR[i].nValue[2] = 0;
              //  bEC=TRUE;
			}  	
		 pItem->SetAttrib_RON(sMAR);   //设置魔法属性
		 //pItem->SetBackUpRmagicAttrib();
	     pItem->SetRPoint(nRongID); 
		 pItem->EnChanceRong(nEnChance);
		 
//==========================以下为镶套宝石属性===================================
/*         KItemNormalAttrib	sbMAR[6];// 魔法
		 ZeroMemory(sbMAR,sizeof(sbMAR)); //分配内存

		 int nbType,nbMax,nbMin;
		 for (int b = 0;b < 6;b++)
		 {
			 if (pItem->m_GeneratorParam.nBGeneratorLevel[b] > 0)
			 { 
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],5,0,&nbType);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],6,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],7,0,&nbMin);
				 sbMAR[b].nAttribType = nbType;
				 sbMAR[b].nValue[0] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],8,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],9,0,&nbMin);
				 sbMAR[b].nValue[1] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],10,0,&nbMax);
				 MagicTab.GetInteger(pItem->m_GeneratorParam.nBGeneratorLevel[b],11,0,&nbMin);
				 sbMAR[b].nValue[2] = nbMax + ((nbMin - nbMax) * nLucky / 10);
				 continue;
			 }
			 sbMAR[b].nAttribType = 0;
			 sbMAR[b].nValue[0] = 0;
			 sbMAR[b].nValue[1] = 0;
			 sbMAR[b].nValue[2] = 0;
		 }  	
		 pItem->SetAttrib_Bao(sbMAR);   //设置魔法属性
		 pItem->EnChanceBao(nEnChance);	 */
//-----------------隐藏属性------------------------------------------------------
		 KItemNormalAttrib	syMA[2];
		 for (i = 0; i < 2; ++i)
		 {//隐藏属性
			 const int* pSrc;
			 KItemNormalAttrib* pDst;
			 pSrc = &(pTemp->m_yinMagicAttribs[i]); //技能编号
			 pDst = &(syMA[i]);
			 
			 int nType,nLow,nHigh;
			 MagicTab.GetInteger(*pSrc + 1,"属性调整类别",0,&nType);
			 pDst->nAttribType = nType;	//类型
			 MagicTab.GetInteger(*pSrc + 1,"参数1最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数1最大值",0,&nHigh);
			 //pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
             pDst->nValue[0]   = GetRandomNumber(nLow,nHigh);

			 MagicTab.GetInteger(*pSrc + 1,"参数2最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数2最大值",0,&nHigh);
			 //pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 pDst->nValue[1]   = GetRandomNumber(nLow,nHigh);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最小值",0,&nLow);
			 MagicTab.GetInteger(*pSrc + 1,"参数3最大值",0,&nHigh);
			 //pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
			 pDst->nValue[2]   = GetRandomNumber(nLow,nHigh);
		 }
		 pItem->SetAttrib_Yin(syMA); 
//===============================================================================
		 MagicTab.Clear();
     	 return TRUE;
}

/*
	pItem->SetAttrib_MA(sMA);   //设置原来魔法属性
	pItem->SetAttrib_RON(sMAR);  //设置熔炼魔法属性
    pItem->SetRPoint(nRongID);  //设置为已经开光
	MagicTab.Clear();
	pItem->EnChanceRong(nEnChance);
*/

/******************************************************************************
	功能：	生成随机装备  ---用于熔炼蓝装
	入口:	nDetailType: 具体类别, 如近程武器、暗器、......
			nParticularType: 详细类别
			nSeriesReq: 五行属性
			nLevel: 等级
			pnaryMALevel: 魔法属性等级数组[6]
			nLucky: 运气值
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_EquipmentRong(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* rongALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nPoint,\
								   IN int nRongID,\
								   IN int Goidid,\
								   IN int nIsDaZhao,\
								   KMagicAttrib *nRogMag,IN int IsWhere,IN BOOL nIsLogin)
{
	
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
    int nCount = 0;
	BOOL bEC = FALSE;
/*
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	int nGameVersion = pItem->m_GeneratorParam.nVersion;
*/
	
#ifdef _SERVER	// 服务器版本
	if  (nIsLogin)
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	else
	    pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）	
    else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck    = nLucky;
    
#else //客户端
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);  //设置随机种子
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel,pnaryMALevel, sizeof(int) * 6);  //物品属性更新参数	
	if (rongALevel)
		memcpy(pItem->m_GeneratorParam.nRGeneratorLevel,rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	//pItem->m_GeneratorParam.nLuck = nLucky;
#endif
    
	int nGameVersion =pItem->m_GeneratorParam.nVersion;

	// 根据入口参数, 确定装备的基本数据
	int i = nParticularType * 10 + nLevel - 1;      // 本公式由策划定义
												    // 详见SPE 2002年8月31日7:40
												    // email: 基础道具数值表
												    // 等级数据从1开始，所以要减回来
	if (nDetailType==equip_mask)
	{
		i = nParticularType;
	}

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)  //可以打造的装备种类
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);//获取原始装备属性    i为 行数
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// 面具
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	// 披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;
	case equip_yinjian:	// 印鉴
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// 饰品
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}
   //打造失败
	if (NULL == pEqu)
	{ 
	 // _ASSERT(FALSE);
	  return bEC; 
	}  
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);  //设置基本属性 和需求属性

	pItem->SetSeries(nSeriesReq); 

	// 确定装备的魔法属性 一般装备
	if (nPoint == 0)  //白装  蓝装
	{
       int bECA=0;  
		if (NULL == pnaryMALevel)  //白装 不需要设置魔法属性
		return TRUE;
		KItemNormalAttrib sMA[6];  //道具的魔法属性
		
		bECA = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bECA)
			pItem->SetAttrib_MA(sMA);  //  传入 魔法属性
	        //pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		    pItem->EnChance(nEnChance);

		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

       	///以下为辨识属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件

		if  (IsWhere>0)
			  nRongID = IsWhere;

		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongID)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0] = 0;
			sMAR[r].nValue[1] = 0;
			sMAR[r].nValue[2] = 0;
			//  bEC=TRUE;
		} 
		
		MagicTabR.Clear();

		pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongID);
		pItem->SetIsWhere(IsWhere);   //是否辨识了
//		pItem->EnChanceRong(nEnChance);
     	return bECA;

	}
	else if (nPoint ==7) //指定蓝装
	{
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值

		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 )
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{ 
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						    nRows = sMA[j].nValue[2]+IsWhere;//nMax
						if (nRows>nMin)
                            nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}
			
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);
		pItem->SetIsWhere(IsWhere);
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);
		return bEC;
	}

	else if (nPoint ==8) //指定蓝装
	{
		/*int bECA=0;  
		if (NULL == pnaryMALevel)  //白装 不需要设置魔法属性
			return FALSE;
		KItemNormalAttrib sMA[6];  //道具的魔法属性

		bECA = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bECA)
			pItem->SetAttrib_MA(sMA);  //  传入 魔法属性
		*/


		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
	
		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值

		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 10 )
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);


				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);



				if (sMA[j].nValue[2]>1)
				{ 
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						    nRows = sMA[j].nValue[2]+IsWhere;
						if (nRows>nMin)
                            nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}
			
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);
		pItem->SetIsWhere(IsWhere);
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);
		return bEC;
	}
	else  //紫装  和 黄装
	{
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值
		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);

				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{ 
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						nRows = sMA[j].nValue[2]+IsWhere;
						if (nRows>nMin)
                            nRows=nMin;
						
						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}

			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);		
		tabMagicAttribe.Clear();
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		pItem->EnChance(nEnChance);
     	return bEC;
	}
}
/******************************************************************************
	功能：	生成随机装备  ---用于客户端产生装备
	入口:	nDetailType: 具体类别, 如近程武器、暗器、......
			nParticularType: 详细类别
			nSeriesReq: 五行属性
			nLevel: 等级
			pnaryMALevel: 魔法属性等级数组[6]
			nLucky: 运气值
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/
BOOL KItemGenerator::Gen_EquipmentCilent(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN const int* rongALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem, \
								   IN int nEnChance,\
								   IN int nPoint,\
								   IN int nRongID,
								   IN int Goidid,\
								   IN const int* pnaryMAType,\
								   IN const int* pnaryMAVala,\
								   IN const int* pnaryMAValb,\
								   IN const int* pnaryMAValc,IN int IsWhere)
{
	
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
    int nCount = 0;
	BOOL bEC = FALSE;
#ifdef _SERVER	// 服务器版本
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed(); //取得随机种子
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6); //魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);
	
	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）	
	else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
    
#else //客户端

	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
	if (rongALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, rongALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
#endif
    
	int nGameVersion =pItem->m_GeneratorParam.nVersion;	
	// 根据入口参数, 确定装备的基本数据
	int i = nParticularType * 10 + nLevel - 1;// 本公式由策划定义
												// 详见SPE 2002年8月31日7:40
												// email: 基础道具数值表
												// 等级数据从1开始，所以要减回来
	if (nDetailType==equip_mask)
	{
		i = nParticularType;
	}

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)  //可以打造的装备种类
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);//获取原始装备属性    i为 行数
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// 面具
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	// 披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;
	case equip_yinjian:	// 披风
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// 披风
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}
   //打造失败
	if (NULL == pEqu)
	{ 
	//_ASSERT(FALSE);
	return bEC; 
	}  
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq); 
//	pItem->SetRPoint(nRongID); 
	// 确定装备的魔法属性 一般装备
	if (nPoint == 0)  //白装  蓝装
	{
		if (NULL == pnaryMALevel)   //白装 不需要设置魔法属性
		return TRUE;
		KItemNormalAttrib sMA[6];	// 道具的魔法属性
		int bECA=0;
		bECA = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bECA)
			pItem->SetAttrib_MA(sMA);  //传入 魔法属性
            //pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		    pItem->EnChance(nEnChance);//设置加成

		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}

		else if (nLucky < 0)
		{
			nLucky = 0;
		}

       	///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件

		if (IsWhere>0)   //蓝装的辨识属性 否则就是 蓝装的熔炼属性
            nRongID = IsWhere;

		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongID)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);

				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0] = 0;
			sMAR[r].nValue[1] = 0;
			sMAR[r].nValue[2] = 0;
			//  bEC=TRUE;
		} 
		
		MagicTabR.Clear();

		pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
        pItem->SetIsWhere(IsWhere); 
		pItem->SetRPoint(nRongID); 		
//		pItem->EnChanceRong(nEnChance);
     	return bECA;

	}
	else if (nPoint ==7) //指定蓝装
	{
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);

		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值

		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 )
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						   nRows = sMA[j].nValue[2]+IsWhere;
						if (nRows>nMin)
                           nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}
			
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);
		pItem->SetIsWhere(IsWhere);
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);
		return bEC;
	}
	else if (nPoint ==8) //指定蓝装

	{
		int bECR=0;

	  ///以下为镶套纹钢属性
	  KItemNormalAttrib	sMAR[6];// 魔法
	  ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
	  KTabFile MagicTabR;
	  MagicTabR.Load(TABFILE_MAGICATTRIB_PATH);  //熔炼蓝装备魔法属性文件
	  
	  int nTypeR,nMaxR,nMinR;
	  for (int r = 0;r < 6;r++)
	  { 
		  if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongID)
		  { 
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
			  sMAR[r].nAttribType = nTypeR;  //属性类型
			  //sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
			  sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
			  //sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
			  sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
			  MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
			  //sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
			  sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);

			  if (sMAR[r].nValue[2]>1)
			  {
				  KTabFile nJnLevel;
				  if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
				  {  
					  int nMinVal=0,nMaxVal=0,nRows;
					  nRows = sMAR[r].nValue[2]+IsWhere;
					  if (nRows>nMinR)
						  nRows=nMinR;
					  
					  nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
					  nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
					  sMAR[r].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
					  
					  nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
					  nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
					  sMAR[r].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
				  }
				  nJnLevel.Clear();
			  } 
			  continue;
		  }
		  sMAR[r].nAttribType = 0;
		  sMAR[r].nValue[0]   = 0;
		  sMAR[r].nValue[1]   = 0;
		  sMAR[r].nValue[2]   = 0;
		  //  bEC=TRUE;
	  } 
	  
	  MagicTabR.Clear();
	  
	  bECR= pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
	  pItem->SetRPoint(nRongID); 		
	  //		pItem->EnChanceRong(nEnChance);
     return bECR;
	}
	else  //紫装  和 黄装
	{
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值
		int nType,nMax,nMin,j;
		for (j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);


				if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						nRows = sMA[j].nValue[2]+IsWhere;
						if (nRows>nMin)
							nRows=nMin;
						
						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
					nJnLevel.Clear();
				} 
				continue;
			}

			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);		
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);
     	//return bEC;
		//----------------------纹钢的属性
       	///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];  // 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件		
		int nTypeR,nMaxR,nMinR;

		for (j = 0;j < 6;j++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[j] > 0 && j < nRongID)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],7,0,&nMinR);
				sMAR[j].nAttribType = nTypeR;  //属性类型
				sMAR[j].nValue[0] = GetRandomNumber(nMaxR,nMinR);//nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],9,0,&nMinR);
				sMAR[j].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				//sMAR[j].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],11,0,&nMinR);
				//sMAR[j].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[j].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[j].nAttribType = 0;
			sMAR[j].nValue[0] = 0;
			sMAR[j].nValue[1] = 0;
			sMAR[j].nValue[2] = 0;
		} 
		
		MagicTabR.Clear();
		bEC=pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongID); 

	   return bEC;
	}
}


BOOL KItemGenerator::GetEquipmentCommonAttrib(IN int nDetailType, IN int nParticularType, IN int nLevel, IN int nSeries, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
//		KASSERT(FALSE);
		return FALSE;
	}

	const int i = nParticularType * 10 + nLevel - 1;// 本公式由策划定义
												// 详见SPE 2002年8月31日7:40
												// email: 基础道具数值表
	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// mat na
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	//披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;	
	case equip_yinjian:	// mat na
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// mat na
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}
	if (NULL == pEqu)
		{// _ASSERT(FALSE);
		return FALSE; }
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeries);
	return TRUE;
}

/******************************************************************************
	功能：	生成随机装备    登录时加载 同步装备  白装 紫装 
	出口:	成功时返回非零, 相关数据在 pItem 所指对象中给出
			失败时返回零
******************************************************************************/

BOOL KItemGenerator::Gen_ExistEquipment(IN int nDetailType,\
								   IN int nParticularType,\
								   IN int nSeriesReq,\
								   IN int nLevel,\
								   IN const int* pnaryMALevel,\
								   IN int nLucky,\
								   IN int nVersion,\
								   IN OUT KItem* pItem,\
								   IN int nEnChance,\
								   IN int nPoint,\
								   IN const int* pnaryRMALevel,\
								   IN int nRongpiont,IN int nIsWhere)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;

	BOOL bEC = FALSE;

	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);//设置随机种子,装备属性就不会变

	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
    else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);

	if (pnaryRMALevel)
        memcpy(pItem->m_GeneratorParam.nRGeneratorLevel, pnaryRMALevel, sizeof(int) * 6); //熔炼魔法等级（itemlevel数组）
	else
		ZeroMemory(pItem->m_GeneratorParam.nRGeneratorLevel, sizeof(int) * 6);

	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	
	// 根据入口参数, 确定装备的基本数据
	 int i = nParticularType * 10 + nLevel - 1;// 本公式由策划定义
												// 详见SPE 2002年8月31日7:40
												// email: 基础道具数值表
												// 等级数据从1开始，所以要减回来
	 if (nDetailType==equip_mask)
	 {
		 i = nParticularType;
	 }

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// mat na
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_pifeng:	// 披风
		pEqu = m_BPTLib.GetPifengRecord(i);
		break;
	case equip_yinjian:	// mat na
		pEqu = m_BPTLib.GetYinjianRecord(i);
		break;
	case equip_shiping:	// mat na
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	default:
		break;
	}
   //打造失败
	if (NULL == pEqu)
	{ 
	//_ASSERT(FALSE);
	return bEC; 
	}  
		// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
		// 检查3点: nParticularType 有误?
		//			nLevel 有误?
		//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
		//			管理的数据有问题?
	pItem->SetAttrib_CBR(pEqu);  //设置基本属性 和需求属性
	pItem->SetSeries(nSeriesReq); 

	// 确定装备的魔法属性 一般装备

	if (nPoint == 0)  //白装  蓝装
	{
		if (NULL == pnaryMALevel)  //白装 不需要设置魔法属性
		return TRUE;
		KItemNormalAttrib	sMA[6];	// 道具的魔法属性
		int bECA=0;
		    bECA = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bECA)
			pItem->SetAttrib_MA(sMA);    //传入 魔法属性
           // pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		    pItem->EnChance(nEnChance);  //强化属性

		if (nLucky > 10)                 //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

       	///以下为熔炼属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件	

		if (nIsWhere>0)
            nRongpiont = nIsWhere;

		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongpiont)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0] = 0;
			sMAR[r].nValue[1] = 0;
			sMAR[r].nValue[2] = 0;
			//  bEC=TRUE;
		} 
		
		MagicTabR.Clear();
		pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetIsWhere(nIsWhere);
		pItem->SetRPoint(nRongpiont); 		
//		pItem->EnChanceRong(nEnChance);
     	return bECA;

	}
	else  if  (nPoint ==7) //指定蓝装
	{
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值
		int nType,nMax,nMin;
		for (int j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						nRows = sMA[j].nValue[2]+nIsWhere;
						if (nRows>nMin)
                            nRows=nMin;

						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}
			
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);
		pItem->SetIsWhere(nIsWhere);
		tabMagicAttribe.Clear();
		//pItem->SetBackUpMagicAttrib();//备份原始魔法属性(用于强化)
		pItem->EnChance(nEnChance);
		return bEC;
	}
	else  if  (nPoint ==8) //指定蓝装
	{
		int bECR=0;
       	///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];// 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICATTRIB_PATH);  //熔炼蓝装备魔法属性文件		
		int nTypeR,nMaxR,nMinR;
		for (int r = 0;r < 6;r++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[r] > 0 && r < nRongpiont)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],7,0,&nMinR);
				sMAR[r].nAttribType = nTypeR;  //属性类型
				//sMAR[r].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],9,0,&nMinR);
				//sMAR[r].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[r],11,0,&nMinR);
				//sMAR[r].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[r].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
			}
			sMAR[r].nAttribType = 0;
			sMAR[r].nValue[0] = 0;
			sMAR[r].nValue[1] = 0;
			sMAR[r].nValue[2] = 0;
			//  bEC=TRUE;
		} 
		
		MagicTabR.Clear();
		bECR=pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongpiont); 		
		//		pItem->EnChanceRong(nEnChance);
		return bECR;
	}
	else  //紫装
	{
		int j;
		KItemNormalAttrib sMA[6];
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load(TABFILE_MAGICATTRIB_PATH);
		if (nLucky > 10)  //幸运值
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}
		pItem->m_GeneratorParam.nLuck = nLucky;  //产生 的幸运值
		int nType,nMax,nMin;
		for (j = 0;j < 6;++j)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				//sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[0] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				//sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[1] = GetRandomNumber(nMax,nMin);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				//sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				sMA[j].nValue[2] = GetRandomNumber(nMax,nMin);

				if (sMA[j].nValue[2]>1)
				{
					KTabFile nJnLevel;
					if (nJnLevel.Load(TABFILE_MAGICAGOLD_PATH))
					{  
						int nMinVal=0,nMaxVal=0,nRows;
						nRows = sMA[j].nValue[2]+nIsWhere;
						if (nRows>nMin)
                            nRows=nMin;
						
						nJnLevel.GetInteger(nRows,"参数1最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数1最大值",0,&nMaxVal);
						sMA[j].nValue[0] = GetRandomNumber(nMinVal,nMaxVal); 
						
						nJnLevel.GetInteger(nRows,"参数3最小值",0,&nMinVal);
						nJnLevel.GetInteger(nRows,"参数3最大值",0,&nMaxVal);
						sMA[j].nValue[2] = GetRandomNumber(nMinVal,nMaxVal); 
					}
                    nJnLevel.Clear();
				}

				continue;
			}

			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}

		bEC=pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);		
		tabMagicAttribe.Clear();
		pItem->EnChance(nEnChance);
	   //----------------------纹钢的属性
       	///以下为镶套纹钢属性
		KItemNormalAttrib	sMAR[6];  // 魔法
		ZeroMemory(sMAR,sizeof(sMAR)); //分配内存
		KTabFile MagicTabR;
		MagicTabR.Load(TABFILE_MAGICAGOLD_PATH);  //熔炼蓝装备魔法属性文件		
		int nTypeR,nMaxR,nMinR;

		for (j = 0;j < 6;j++)
		{ 
			if (pItem->m_GeneratorParam.nRGeneratorLevel[j] > 0 && j < nRongpiont)
			{ 
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],5,0,&nTypeR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],6,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],7,0,&nMinR);
				sMAR[j].nAttribType = nTypeR;  //属性类型
				//sMAR[j].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[j].nValue[0] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],8,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],9,0,&nMinR);
				//sMAR[j].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[j].nValue[1] = GetRandomNumber(nMaxR,nMinR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],10,0,&nMaxR);
				MagicTabR.GetInteger(pItem->m_GeneratorParam.nRGeneratorLevel[j],11,0,&nMinR);
				//sMAR[j].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLucky / 10);
				sMAR[j].nValue[2] = GetRandomNumber(nMaxR,nMinR);
				continue;
				/*
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],5,0,&nTypeR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],6,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],7,0,&nMinR);
				sMAR[i].nAttribType = nTypeR;  //属性类型
				sMAR[i].nValue[0] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],8,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],9,0,&nMinR);
				sMAR[i].nValue[1] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],10,0,&nMaxR);
				tabMagic.GetInteger(Item[nLindexID].GetItemParam()->nRGeneratorLevel[i],11,0,&nMinR);
				sMAR[i].nValue[2] = nMaxR + ((nMinR - nMaxR) * nLuck / 10);	
				continue;
				*/
			}
			sMAR[j].nAttribType = 0;
			sMAR[j].nValue[0] = 0;
			sMAR[j].nValue[1] = 0;
			sMAR[j].nValue[2] = 0;
		} 
		
		MagicTabR.Clear();
		bEC=pItem->SetAttrib_RON(sMAR);   //设置熔炼魔法属性
		pItem->SetRPoint(nRongpiont); 		
		//pItem->EnChanceRong(nEnChance);
		return bEC;
	   //----------------------
	}
} 
//生成任务类物品
BOOL KItemGenerator::Gen_Quest(IN int nDetailType, IN OUT KItem* pItem,IN int nStackNum,IN int nLevel)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
	BOOL bEC = FALSE;

	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	

	//if (nDetailType>m_BPTLib.GetQuestRecordNumber()-1);	  //容错 超出范围 现有的 Mine物品的 数量
	//   return bEC;

	const KBASICPROP_QUEST* pQuest = NULL;
	pQuest = m_BPTLib.GetQuestRecord(nDetailType);  //导入模板资料  具体类别（ques）
	if (NULL == pQuest)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetStackNum(nStackNum);
	pItem->SetLevel(nLevel);
	return TRUE;
}
//生成任务类商店物品
BOOL KItemGenerator::Gen_QuestShop(IN int nDetailType,\
								   IN OUT KItem* pItem,\
								   IN int nStackNum,\
								   int nPerXu,\
								   int nPrice,\
								   IN int nLevel,int nIsBang,int nPriceType)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_QUEST* pQuest = NULL;
	pQuest = m_BPTLib.GetQuestRecord(nDetailType);  //导入模板资料 （ques）
	if (NULL == pQuest)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetStackNum(nStackNum);
	//pItem->SetPerXu(nPerXu);
	//pItem->SetPerPrice(nPrice);
	if (nPerXu>0) //如果大于零的 就改变原始数据
		pItem->SetPerXu(nPerXu);
	if (nPrice>0)
		pItem->SetPerPrice(nPrice);

	pItem->SetLevel(nLevel);
	pItem->SetBang(nIsBang);
	pItem->SetModel(nPriceType);
	
	return TRUE;
}

//------------------------------------------产生纹纲
BOOL KItemGenerator::Gen_Fusion(IN int nParticularType,\
								IN OUT KItem* pItem,\
								IN int nStackNum,\
								IN int nLevel,\
								IN int nSeries,\
								IN int nLuck,\
								IN const int* npnaryJBLevel)
{//(nParticularType,pItem,nStackNum,nLevel,nSeries,nLuck,pnJbLevel);
   //	_ASSERT(this != NULL);
//	_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
	
	BOOL bEC = FALSE;
/*
	if (npnaryJBLevel)
        memcpy(pItem->m_GeneratorParam.nJGeneratorLevel, npnaryJBLevel, sizeof(int) * 7); //基本属性（itemlevel数组）
	else
        ZeroMemory(pItem->m_GeneratorParam.nJGeneratorLevel, sizeof(int) * 7);
*/
    ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_FUSION* pQuest = NULL;

    /*#ifdef _SERVER	// 服务器版本
	      pQuest = m_BPTLib.GetFusionRecord(nParticularType+nLevel-1);  //获取这个纹纲的基本信息
    #else*/
	//if (nParticularType>m_BPTLib.GetFusionRecordNumber()-1);	  //容错 超出范围 现有的 Mine物品的 数量
	//   return bEC;


	pQuest = m_BPTLib.GetFusionRecord(nParticularType);
	//#endif

	if (NULL == pQuest)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
     
	*pItem = *pQuest;

	pItem->SetSeries(nSeries);
	pItem->SetStackNum(nStackNum);
	pItem->SetLevel(nLevel);
		
//if (nMagicA && pQuest->m_nParticularType >= 200 && pQuest->m_nParticularType <= 205)  //属性石头的产生
/*	KEQCP_BASIC sBAS[7];
	ZeroMemory(sBAS,sizeof(sBAS));
	
		for (int i=0;i<7;++i)
		{//脚本刷的
			if 	(pItem->m_GeneratorParam.nJGeneratorLevel[i] > 0 )  //0 1
			{  
				sBAS[i].nType = 0;  //类型
				sBAS[i].sRange.nMin = pItem->m_GeneratorParam.nJGeneratorLevel[i];
				sBAS[i].sRange.nMax = pItem->m_GeneratorParam.nJGeneratorLevel[i];
				continue;
			}		
			sBAS[i].nType =0;  //类型
			sBAS[i].sRange.nMin = 0;
			sBAS[i].sRange.nMax = 0;
		}

	pItem->SetAttrib_Base(sBAS);   //基础属性
*/
	return TRUE;
}
//------------------------------------------
BOOL KItemGenerator::Gen_Mine(IN int nDetailType, \
							  IN OUT KItem* pItem,\
							  IN int nStackNum,\
							  IN int nLevel,\
							  IN int nSerises,\
							  IN int nMagicA,\
							  IN const int* npnaryJBLevel)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
	
	BOOL bEC = FALSE;
	if (npnaryJBLevel)
        memcpy(pItem->m_GeneratorParam.nJGeneratorLevel, npnaryJBLevel, sizeof(int) * 7); //基本属性（itemlevel数组）
	else
        ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));

	//if (nDetailType>m_BPTLib.GetMineRecordNumber()-1);	  //容错 超出范围 现有的 Mine物品的 数量
	//   return bEC;

	const KBASICPROP_MINE* pQuest = NULL;
	pQuest = m_BPTLib.GetMine(nDetailType);

	

	if (NULL == pQuest)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetSeries(nSerises);
	pItem->SetStackNum(nStackNum);
	pItem->SetLevel(nLevel);

    KTabFile Mine;
    Mine.Load("\\Settings\\ItemMine.txt");  //是否带有魔法属性的mine物品列表
	int nParticular=0,nMagciType=0;
	int nReg=0,mReg=0, i;
	for (i=0;i<Mine.GetHeight()-1;++i)
	{
       Mine.GetInteger(i+2,"nMineItemID",0,&nParticular);
	   Mine.GetInteger(i+2,"nMagciType",0,&nMagciType);
	   if (nParticular==pQuest->m_nParticularType)
	   {  
		  nReg=1;
		  mReg=nMagciType;
		  break;
	   } 
	}
    Mine.Clear();

//	if (nMagicA && pQuest->m_nParticularType >= 200 && pQuest->m_nParticularType <= 205)  //属性石头的产生
	KEQCP_BASIC sBAS[7];
	      ZeroMemory(sBAS,sizeof(sBAS));
	if (nReg==1 && mReg==1)  //图谱类 龙脉石类
	{//脚本刷的
		
		for (i=0;i<7;++i)
		{
		    if 	(pItem->m_GeneratorParam.nJGeneratorLevel[i] > 0 )  //0 1
			{  
		     sBAS[i].nType = 0;  //类型
		     sBAS[i].sRange.nMin = pItem->m_GeneratorParam.nJGeneratorLevel[i];
		     sBAS[i].sRange.nMax = pItem->m_GeneratorParam.nJGeneratorLevel[i];
			 continue;
			}

		   sBAS[i].nType =0;  //类型
		   sBAS[i].sRange.nMin = 0;
		   sBAS[i].sRange.nMax = 0;
		}
	}
	else if (nReg==1 && mReg==0)   //属性石头 打造紫装
	{
		pItem->m_GeneratorParam.nLuck = nMagicA; //幸运值
		sBAS[0].nType = 0;
		sBAS[0].sRange.nMin = nMagicA;
		sBAS[0].sRange.nMax = nMagicA;	
	}
	else 
	{//什么都不是的清零
		for (i=0;i<7;++i)
		{
			sBAS[i].nType =0;  //类型
			sBAS[i].sRange.nMin = 0;
			sBAS[i].sRange.nMax = 0;
		}
	}
	pItem->SetAttrib_Base(sBAS);   //基础属性

	return TRUE;
}
//生成商店类mine物品
BOOL KItemGenerator::Gen_MineShop(IN int nDetailType, IN OUT KItem* pItem,IN int nStackNum,IN int nLevel,IN int nSerises,IN int nMagicA,int nPerXu,int nPrice,int nIsBang,int nPriceType)
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	//if (nDetailType>m_BPTLib.GetMineRecordNumber()-1);	  //容错 超出范围 现有的 Mine物品的 数量
	//   return bEC;


	const KBASICPROP_MINE* pQuest = NULL;
	pQuest = m_BPTLib.GetMine(nDetailType);
	if (NULL == pQuest)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetSeries(nSerises);
	pItem->SetStackNum(nStackNum);
	pItem->SetLevel(nLevel);
	//pItem->SetPerXu(nPerXu);
	//pItem->SetPerPrice(nPrice);
	if (nPerXu>0) //如果大于零的 就改变原始数据
		pItem->SetPerXu(nPerXu);
	if (nPrice>0)
		pItem->SetPerPrice(nPrice);

	pItem->SetBang(nIsBang);
	pItem->SetModel(nPriceType);

	KTabFile MineShop;
    MineShop.Load("\\Settings\\ItemMine.txt");
	int nParticular=0,nMagciType=0;
	int nReg=0,mReg=0,i;
	for ( i=0;i<MineShop.GetHeight()-1;++i)
	{
		MineShop.GetInteger(i+2,"nMineItemID",0,&nParticular);
		MineShop.GetInteger(i+2,"nMagciType",0,&nMagciType);
		if (nParticular==pQuest->m_nParticularType)
		{  
			nReg=1;
			mReg=nMagciType;
			break;
		} 
	}
    MineShop.Clear();
	KEQCP_BASIC sBAS[7];
	ZeroMemory(sBAS,sizeof(sBAS));
	//	if (nMagicA && pQuest->m_nParticularType >= 200 && pQuest->m_nParticularType <= 205)  //属性石头的产生
    if (nReg==1 && mReg==0)  //属性石头
	{
		pItem->m_GeneratorParam.nLuck = nMagicA;
		sBAS[0].nType = 0;
		sBAS[0].sRange.nMin = nMagicA;
		sBAS[0].sRange.nMax = nMagicA;

	}
	else
	{
		for (i=0;i<7;++i)
		{
			sBAS[i].nType =0;  //类型
			sBAS[i].sRange.nMin = 0;
			sBAS[i].sRange.nMax = 0;
		}	
	}
	pItem->SetAttrib_Base(sBAS);
	return TRUE;
}
BOOL KItemGenerator::Gen_TownPortal(IN OUT KItem* pItem) //生成一个回城卷
{
	//_ASSERT(this != NULL);
	//_ASSERT(pItem != NULL);
	if (this==NULL || pItem==NULL)
			return false;

	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_TOWNPORTAL* pPortal = NULL;
	pPortal = m_BPTLib.GetTownPortalRecord(0);
	if (NULL == pPortal)
	{
		//_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pPortal;
	return TRUE;
}

// BOOL KItemGenerator::GetDIYItem(IN int nLuck,IN int nPoint, IN int nLevel, OUT KItem* pItem,IN int nSeries,IN int nDetail, IN int nParticular,IN int* pnMagicLevel, IN int nEnChance)
// {
// 	_ASSERT(this != NULL);
// 	_ASSERT(pItem != NULL);
// 	
// 	BOOL bEC = FALSE;
// 	
// 	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
// 	if (pnMagicLevel)
// 		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnMagicLevel, sizeof(int) * 6);
// 	int nGameVersion = pItem->m_GeneratorParam.nVersion;
// 	
// 	// 根据入口参数, 确定装备的基本数据
// 	const int i = nParticular * 10 + nLevel - 1;// 本公式由策划定义
// 	// 详见SPE 2002年8月31日7:40
// 	// email: 基础道具数值表
// 	// 等级数据从1开始，所以要减回来
// 	const KBASICPROP_EQUIPMENT* pEqu = NULL;
// 	switch(nDetail)
// 	{
// 	case equip_meleeweapon:
// 		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
// 		break;
// 	case equip_rangeweapon:
// 		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
// 		break;
// 	case equip_armor:
// 		pEqu = m_BPTLib.GetArmorRecord(i);
// 		break;
// 	case equip_helm:
// 		pEqu = m_BPTLib.GetHelmRecord(i);
// 		break;
// 	case equip_boots:
// 		pEqu = m_BPTLib.GetBootRecord(i);
// 		break;
// 	case equip_belt:
// 		pEqu = m_BPTLib.GetBeltRecord(i);
// 		break;
// 	case equip_amulet:
// 		pEqu = m_BPTLib.GetAmuletRecord(i);
// 		break;
// 	case equip_ring:
// 		pEqu = m_BPTLib.GetRingRecord(i);
// 		break;
// 	case equip_cuff:
// 		pEqu = m_BPTLib.GetCuffRecord(i);
// 		break;
// 	case equip_pendant:
// 		pEqu = m_BPTLib.GetPendantRecord(i);
// 		break;
// 	case equip_horse:
// 		pEqu = m_BPTLib.GetHorseRecord(i);
// 		break;
// 	case equip_mask:
// 		pEqu = m_BPTLib.GetMaskRecord(i);
// 		break;
// 	default:
// 		break;
// 	}
// 	if (NULL == pEqu)
// 	{ _ASSERT(FALSE); return bEC; }
// 	// 运行至此的直接原因: 只有n种装备, 而上面 i 的值在[0,n-1]之外
// 	// 检查3点: nParticularType 有误?
// 	//			nLevel 有误?
// 	//			原始的tab file有误导致 m_BPTLib.m_BPTEquipment 所
// 	//			管理的数据有问题?
// 	pItem->SetAttrib_CBR(pEqu);
// 	pItem->SetSeries(nSeries);
// 
// 	pItem->m_GeneratorParam.nLuck = nPoint;
// 	pItem->m_GeneratorParam.nVersion = nPoint;
// 	pItem->SetSet(nPoint);
// 	pItem->SetSetId(nLuck);
// 	KItemNormalAttrib sMA[6];
// 	ZeroMemory(sMA,sizeof(sMA));
// 	KTabFile tabMagicAttribe;
// 	tabMagicAttribe.Load("\\Settings\\item\\magicattrib.txt");
// 
// 	int nType,nMax,nMin;
// 	for (int j = 0;j < nPoint;++j)
// 	{
// 		if (pnMagicLevel[j])
// 		{
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"属性调整类别",0,&nType);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数1最大值",0,&nMax);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数1最小值",0,&nMin);
// 			sMA[j].nAttribType = nType;
// 			sMA[j].nValue[0] = nMin + ((nMax - nMin) * nLuck / 10);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数2最大值",0,&nMax);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数2最小值",0,&nMin);
// 			sMA[j].nValue[1] = nMin + ((nMax - nMin) * nLuck / 10);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数3最大值",0,&nMax);
// 			tabMagicAttribe.GetInteger(pnMagicLevel[j],"参数3最小值",0,&nMin);
// 			sMA[j].nValue[2] = nMin + ((nMax - nMin) * nLuck / 10);
// 		}
// 		else
// 		{
// 			sMA[j].nAttribType = magic_no;
// 			sMA[j].nValue[0] = 0;
// 			sMA[j].nValue[1] = 0;
// 			sMA[j].nValue[2] = 0;
// 		}
// 	}
// 	pItem->SetAttrib_MA(sMA);
// 	pItem->EnChance(nEnChance);
// 
// 	tabMagicAttribe.Clear();
// 	return TRUE;
// }
