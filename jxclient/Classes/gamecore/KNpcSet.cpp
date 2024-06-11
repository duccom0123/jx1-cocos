#include "KCore.h"
#include <math.h>
#include "KNpc.h"
#include "KSubWorld.h"
#include "KRegion.h"
#include "GameDataDef.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"  //原来没有的
#include "CoreShell.h"
//#include "Scene/KScenePlaceC.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
//#include "KOption.h"
//#ifndef TOOLVERSION
//#include "../../Headers/IClient.h"
//#endif

#include "gamescene/SceneDataDef.h"
#include "KMath.h"
//#define	ENCHANT_SETTING_PATH	"settings\\npc"
//#define	ENCHANT_NORMAL_FILE		"normalunique.txt"
//#define	ENCHANT_SPECIAL_FILE	"speicalunique.txt"
extern KLuaScript		*g_pNpcLevelScript;

KNpcSet	NpcSet;

KNpcSet::KNpcSet()
{
	m_dwIDCreator = 1; //默认是1000起
	Release();
}

BOOL KNpcSet::Release(void)
{
	/*for (_KMapTemplate::iterator it = m_mapTemplate.begin(); it != m_mapTemplate.end(); ++it)
	{
		//SAFE_DELETE(it->second);
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}*/
	m_mapTemplate.clear();
	return TRUE;
}

// 黄金怪物设定文件的初始化没有放在这里，直接放在 core 的初始化里面
void KNpcSet::Init()
{
    //GenRelationTable();  //初始化 敌友关系
	m_FreeIdxNpcSet.Init(MAX_NPC);  //MAX_NPC 分配内存
	m_UseIdxNpcSet.Init(MAX_NPC);
	int i;
	// 开始时所有的数组元素都为空   初始化内存数组
	for (i = MAX_NPC - 1; i > 0; i--) // 除0外
	{
		m_FreeIdxNpcSet.Insert(i);
		Npc[i].m_Node.m_nIndex = i;     //节点的索引
		Npc[i].m_cGold.Init(i);         //黄金怪物初始化
	}

	//if (!m_cTabFile.Load(NPC_SETTING_FILE))
	//	return;
	
	/*INT nNpcTemplateHeight = g_NpcSetting.GetHeight();
	INT nNpcTemplateId;
	ZeroStruct(m_anTemplateRowId);
	for (INT nRowId	= 2; nRowId <= nNpcTemplateHeight; ++nRowId)
	{
		//m_cTabFile.GetInteger(nRowId, "Id", 0, &nNpcTemplateId);
		nNpcTemplateId =  nRowId-1;
		if (nNpcTemplateId <= 0 || nNpcTemplateId > MAX_NPCSTYLE)
		{
			_ASSERT(FALSE);
			continue;
		} 
		
		if (nNpcTemplateId > 0 && nNpcTemplateId <= MAX_NPCSTYLE)
			m_anTemplateRowId[nNpcTemplateId]	= nRowId; //行号
	} */

	LoadPlayerBaseValue(PLAYER_BASE_VALUE);
	m_nShowPateFlag = PATE_CHAT;
	ZeroMemory(m_RequestNpc, sizeof(m_RequestNpc));
	m_RequestFreeIdx.Init(MAX_NPC_REQUEST);  //最大的NPC数量请求
	m_RequestUseIdx.Init(MAX_NPC_REQUEST);

	for (i = MAX_NPC_REQUEST - 1; i > 0; i--)
	{
		m_RequestFreeIdx.Insert(i);
	}
}

void KNpcSet::LoadPlayerBaseValue(LPSTR szFile)
{
	KIniFile	File;

	File.Load(szFile);

	File.GetInteger("Common", "HurtFrame", 12, &m_cPlayerBaseValue.nHurtFrame);
	File.GetInteger("Common", "RunSpeed", 10, &m_cPlayerBaseValue.nRunSpeed);
	File.GetInteger("Common", "WalkSpeed", 5, &m_cPlayerBaseValue.nWalkSpeed);
	File.GetInteger("Common", "AttackFrame", 20, &m_cPlayerBaseValue.nAttackFrame);
	File.GetInteger("Male", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[0]);
	File.GetInteger("Female", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[1]);
	File.GetInteger("Male", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[0]);
	File.GetInteger("Female", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[1]);
	File.GetInteger("Male", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[0]);
	File.GetInteger("Female", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[1]);
	File.Clear();
}
//是否是这个NPC
BOOL KNpcSet::IsNpcExist(int nIdx, DWORD dwId)
{
	if (Npc[nIdx].m_dwID == dwId)
		return TRUE;
	else
		return FALSE;
}

// 获取双方的敌友关系
NPC_RELATION KNpcSet::GenOneRelation(NPCKIND Kind1, NPCKIND Kind2, NPCCAMP Camp1, NPCCAMP Camp2)
{
	// 路人NPC没有战斗关系
	if (Kind1 == kind_dialoger || Kind2 == kind_dialoger)  //任意一方为 对话NPC 都是 对话关系
		return relation_dialog;
	if ((Kind1 == kind_partner|| Kind1 == kind_player) && Kind2 == kind_normal && Camp1!=Camp2)
		return relation_enemy;
	if (Kind1 == kind_normal && (Kind2 == kind_partner || Kind2 == kind_player) && Camp1!=Camp2)
		return relation_enemy;
    if ((Kind1 == kind_normal &&  Kind2 == kind_partner) || (Kind1 == kind_partner &&  Kind2 == kind_normal))
	    return relation_enemy;
    if ((Kind1 == kind_partner &&  Kind2 == kind_partner) && Camp1!=Camp2)
	    return relation_enemy;

	// 路人阵营没有战斗关系
	if (Camp1 == camp_event || Camp2 == camp_event)
		return relation_none;
	
	// 新手和动物还是战斗关系
	if ((Camp1 == camp_begin && Camp2 == camp_animal)
		||(Camp1 == camp_animal && Camp2 == camp_begin))
		return relation_enemy;

	// 只要有一个新手，就不存在战斗关系(是同盟关系，大家帮新手)
	if (Camp1 == camp_begin || Camp2 == camp_begin)
		return relation_ally;
	
    // 两个都是玩家
	if (Kind1 == kind_player && Kind2 == kind_player)
	{
		if (Camp1 == camp_free || Camp2 == camp_free)  //杀手阵营
		{
			return relation_enemy;
		}
	}

	// 同阵营为伙伴关系
	if (Camp1 == Camp2) //同阵营为伙伴关系
		return relation_ally;

	// 其他情况为战斗关系
	return relation_enemy;
}


// Add by Freeway Chen in 2003.7.14
/*int KNpcSet::GenRelationTable()
{
    int nKind1 = 0;
    int nKind2 = 0;
    int nCamp1 = 0;
    int nCamp2 = 0;

    for (nKind1 = 0; nKind1 < kind_num; ++nKind1)
    {
        for (nKind2 = 0; nKind2 < kind_num; ++nKind2)
        {
            for (nCamp1 = 0; nCamp1 < camp_num; ++nCamp1)
            {
                for (nCamp2 = 0; nCamp2 < camp_num; ++nCamp2)
                {
                    m_RelationTable[nKind1][nKind2][nCamp1][nCamp2] = GenOneRelation(
                        (NPCKIND)nKind1, 
                        (NPCKIND)nKind2, 
                        (NPCCAMP)nCamp1, 
                        (NPCCAMP)nCamp2
                    );
                }
            }
        }

    }

    return true;

} */


int	KNpcSet::SearchByTongName(LPSTR szName)
{

	return 0;

}


//dwID 名字查找 返回索引
int	KNpcSet::SearchID(DWORD dwID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_dwID == dwID)  //唯一的标示
			return nIdx;
	}
	return 0;
}

//---------------------------------------------------------------------------
//	功能：查找某个ClientID的npc是否存在
//---------------------------------------------------------------------------
int		KNpcSet::SearchClientID(KClientNpcID sClientID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID == sClientID.m_dwRegionID &&
			Npc[nIdx].m_sClientNpcID.m_nNo == sClientID.m_nNo)
			return nIdx;
	}

	return 0;
}

//名字查找 返回索引
int KNpcSet::SearchName(LPSTR szName)
{

	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name,szName))
		  return nIdx;
	}
	return 0;
}

int KNpcSet::SearchGSNameInMap(LPSTR szName,int inMapidx)
{
	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		char nstrName[64]={0};
		Npc[nIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);

		if (Npc[nIdx].m_Kind==kind_normal && g_StrCmp(nstrName,szName) && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID==inMapidx)
			nCount ++;
			//return nIdx;
	}
	return nCount;
}

//名字查找 返回索引
int KNpcSet::SearchNameInMap(LPSTR szName,int inMapidx,int nType)
{

	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name,szName) && SubWorld[Npc[nIdx].m_SubWorldIndex].m_SubWorldID==inMapidx)
		{
		  	nCount ++;
		    if (nType==1)
			   return nIdx;
		}
			//return nIdx;
	}
	return nCount;
}


//获取NPC setings
int KNpcSet::SearchNpcSettingID(LPSTR szName)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name,szName))
			return Npc[nIdx].m_NpcSettingIdx;
	}
	return 0;
}

int KNpcSet::DelNpcInNpcSet(int nNpcSettings)
{
	int nIdx = 0;
	int nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_NpcSettingIdx==nNpcSettings)
		{
             Remove(nIdx);
			 Npc[nIdx].m_Node.Remove();
			 nCount++;
			//return Npc[nIdx].m_NpcSettingIdx;
		}
	}
	return nCount;
}

//查归属名
int KNpcSet::SearchGSName(LPSTR szName)
{
	int nIdx = 0,nCount=0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		char nstrName[64]={0};
		Npc[nIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);
		if (g_StrCmp(nstrName,szName))
			//nCount ++;
			return nIdx;
	}
	return nIdx;
}

//dwID 名字查找
int KNpcSet::SearchNameID(DWORD dwID)
{
	int nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)   //查找玩家ID  不是玩家的就不查找了
			continue;
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if (dwID == dwNameID)
			return nIdx;
	}
	return 0;
}


int KNpcSet::FindFreeNpcSet()
{
	return m_FreeIdxNpcSet.GetNext(0);
}

//---------------------------------------------------------------------------
//	功能：添加一个客户端npc（需要设定ClientNpcID）
//---------------------------------------------------------------------------
int		KNpcSet::AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo)
{
	int		nNpcNo, nNpcSettingIdxInfo, nMapX, nMapY, nOffX, nOffY, nRegion;

	nNpcSettingIdxInfo = MAKELONG(1, nTemplateID);
	SubWorld[0].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;

	nNpcNo = this->AddServerNpcB(nNpcSettingIdxInfo, 0, nRegion, nMapX, nMapY, nOffX, nOffY);

	if (nNpcNo > 0)
	{
		Npc[nNpcNo].m_sClientNpcID.m_dwRegionID = MAKELONG(nRegionX, nRegionY);
		Npc[nNpcNo].m_sClientNpcID.m_nNo = nNo;
		Npc[nNpcNo].m_RegionIndex = nRegion;
		Npc[nNpcNo].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		Npc[nNpcNo].m_bClientOnly = TRUE;
		Npc[nNpcNo].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		SubWorld[0].m_Region[nRegion].DecNpcRef(Npc[nNpcNo].m_MapX, Npc[nNpcNo].m_MapY);
	}

	return nNpcNo;
}

//地图刷NPC 服务器端的
int KNpcSet::AddMapNpc(int nSubWorld, void* pNpcInfo)
{
	KSPNpc*	pKSNpcInfo = (KSPNpc *)pNpcInfo;

	int nMpsX = pKSNpcInfo->nPositionX;
	int nMpsY = pKSNpcInfo->nPositionY;
	int	nNpcSettingIdxInfo = MAKELONG(pKSNpcInfo->nLevel, pKSNpcInfo->nTemplateID);
	int nRet = AddServerNpcA(nNpcSettingIdxInfo, nSubWorld, nMpsX, nMpsY,0,0,0,0,0,0);
	if (nRet)
	{
		Npc[nRet].m_TrapScriptID = 0;
		g_StrCpyLen(Npc[nRet].Name, pKSNpcInfo->szName, sizeof(Npc[nRet].Name)); //按照字节数 复制名字
		//修改NPC的为地图默认数据
		/*Npc[nRet].m_Camp        = pKSNpcInfo->cCamp;    //阵营
		  Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;    //阵营
		*/
		Npc[nRet].m_Series      = (BYTE)pKSNpcInfo->cSeries;  //五行
		if (pKSNpcInfo->shKind >= kind_normal && pKSNpcInfo->shKind < kind_num)
		   Npc[nRet].m_Kind     = pKSNpcInfo->shKind;     //种类

		if (pKSNpcInfo->cCamp >= camp_begin && pKSNpcInfo->cCamp < camp_num)
		{
			Npc[nRet].m_Camp        = pKSNpcInfo->cCamp;    //阵营
			Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;    //阵营
		}

		char nMapNpc[32]={0};
		int  nIsCreat=0;
		ZeroMemory(nMapNpc,sizeof(nMapNpc));
		t_sprintf(nMapNpc,"%d_IsScript",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);
		Npc[nRet].IsExeGoldScript = nIsCreat; 
		t_sprintf(nMapNpc,"%d_IsCreat",SubWorld[nSubWorld].m_SubWorldID);
		g_NpcMapDropRate.GetInteger("List",nMapNpc,0,&nIsCreat);//默认执行全局脚本
		Npc[nRet].IsCreatBoss     = nIsCreat; 


		// 如果是黄金怪物，备份数据
		if (pKSNpcInfo->bSpecialNpc) //特殊NPC
		{
			Npc[nRet].m_cGold.SetGoldTypeAndBackData();
		}
		else
		{
			Npc[nRet].m_cGold.SetGoldType(FALSE);
		}

		if (pKSNpcInfo->szScript[0]) //脚本不等空
		{  
		    int zVal=CheckThisNpc(Npc[nRet].m_NpcSettingIdx,"NoScript");//检查是否在禁止列表中
		    if (zVal)
			{//在禁止列表中，就不加载脚本
			  return nRet; 
			}
			char nMsg[256]={0};
			/*
			if (pKSNpcInfo->szScript[0] == '.')
				g_StrCpyLen(Npc[nRet].ActionScript, &pKSNpcInfo->szScript[1], sizeof(Npc[nRet].ActionScript));
			else
				g_StrCpyLen(Npc[nRet].ActionScript, pKSNpcInfo->szScript, sizeof(Npc[nRet].ActionScript));
			*/
			if (pKSNpcInfo->szScript[0] == '.')
				g_StrCpyLen(nMsg, &pKSNpcInfo->szScript[1], sizeof(nMsg));
			else
				g_StrCpyLen(nMsg, pKSNpcInfo->szScript, sizeof(nMsg));
			 // 保持小写，保证脚本对应关系
			g_StrLower(nMsg);
			Npc[nRet].SetstrInfo(STR_ACTION_SCRIPT,nMsg);
			Npc[nRet].m_ActionScriptID = g_CheckFileExist(nMsg);
		}
		else
		{
			Npc[nRet].m_ActionScriptID = 0;
		}
	}
	return nRet;
}
//脚本刷NPC和 玩家的增加（addNPC）
int KNpcSet::AddServerNpcA(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY,int nBoss,int nRodom,int nOffwminx,int nOffwmaxx,int nOffhminy,int nOffhmaxy,int nNpcKind,unsigned int nNpcDwidx)
{
	int nRegion, nmMapX, nmMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >=MAX_SUBWORLD)
		return 0;

	int nBarrier;

	if (nRodom>0)
	{
	    int mMpsPosX,mMpsPosY;
		    SubWorld[nSubWorld].Mps2MapnRodom(&nRegion, &nmMapX, &nmMapY, &nOffX, &nOffY,&mMpsPosX,&mMpsPosY,nOffwminx,nOffwmaxx,nOffhminy,nOffhmaxy);
            nBarrier = SubWorld[nSubWorld].GetBarrier(mMpsPosX, mMpsPosY);  ///需要像素坐标障碍物判断
	    if (Obstacle_NULL!=nBarrier)
            return 0;
	}
	else
	{ 
		//SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nmMapX, &nmMapY, &nOffX, &nOffY);
        	 //障碍物判断
	   POINT	ptLocal;
	   //KMapPos	Pos;
	   //GetMpsPos(&nX, &nY,&nMap);                          // 怪物的坐标=转换后的坐标  
       ptLocal.x = nMpsX;
       ptLocal.y = nMpsY;
	
	   SubWorld[nSubWorld].GetFreeObjPos(ptLocal);           //获取周围没有障碍物的坐标(位置矫正)

	   SubWorld[nSubWorld].Mps2Map(ptLocal.x, ptLocal.y,&nRegion, &nmMapX, &nmMapY,&nOffX, &nOffY);
	  
		//if (nRegion < 0)
		//{
		 // printf("[Map]传送非法坐标 Pos(%d,%d,m:%d,行:%d) 无效坐标!\n", nX, nY,dwSubWorldID,nTargetSubWorld);
		 // return 0;
		//}

	}

	if (nRegion < 0)
		return 0;

	int nIndexID=0;
        nIndexID=AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nmMapX, nmMapY, nOffX, nOffY,nBoss);	
	
	if (nIndexID >0)
	{
	  // 如果是黄金怪物
	  if (nBoss>0 && Npc[nIndexID].m_Kind==kind_normal) //特殊NPC  g_NpcMapDropRate
	  {//if(!IsPlayer() && m_Kind!=kind_dialoger && m_Kind!=kind_partner)
		Npc[nIndexID].m_cGold.SetGoldCurrentType(nBoss,nSubWorld);
		//Npc[nIndexID].m_cGold.RandChangeGold(0,0,nBoss,nSubWorld);  //同步怪物类型 
	  }
//-----------------------------------------------------------------------------------------
//普通NPC 和特殊NPC 通用
	  if (Npc[nIndexID].m_DataRes.Init(Npc[nIndexID].szNpcTypeName,&g_NpcResList,nIndexID))
	  {
		  //if  (nNpcKind >-1 && nNpcKind==kind_player)
			//  CCMessageBox(Npc[nIndexID].szNpcTypeName,"m_DataRes.Init");
	  }//外观初始化

	  /*Npc[nIndexID].m_DataRes.SetAction(Npc[nIndexID].m_ClientDoing);            //设定外观行为对接
	  Npc[nIndexID].m_DataRes.SetRideHorse(Npc[nIndexID].m_bRideHorse);          //设定是否骑马接帧数
	  Npc[nIndexID].m_DataRes.SetArmor(Npc[nIndexID].m_ArmorType);               //衣服
	  Npc[nIndexID].m_DataRes.SetHelm(Npc[nIndexID].m_HelmType);                 //头部
	  Npc[nIndexID].m_DataRes.SetHorse(Npc[nIndexID].m_HorseType);               //马位置
	  Npc[nIndexID].m_DataRes.SetWeapon(Npc[nIndexID].m_WeaponType);             //武器
	 */
	  if (Npc[nIndexID].m_Kind==kind_player)
	  {  
	      //Npc[nIndexID].m_DataRes.SetPifeng(Npc[nIndexID].m_PifengType);             //披风
	      //Npc[nIndexID].m_DataRes.SetChiBang(Npc[nIndexID].m_ChiBangType);
	  }
//-----------------------------------------------------------------------------------------	
	}
	return nIndexID;	
}
//刷押镖NPC
int KNpcSet::Addyabiao(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, IN int nSeries, IN int nComp,IN int nRindexid,int nBoss)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)//MAX_SUBWORLD
		return 0;
	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	int indexid;
	    indexid= AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nMapX, nMapY, nOffX, nOffY,-1);
	if (indexid >0 && nRindexid>0 )
	{   
		Npc[indexid].m_Camp       = Npc[nRindexid].m_Camp;
		Npc[indexid].m_btRankFFId = 88;
		Npc[indexid].m_IsRevive   = g_FileName2Id(Npc[nRindexid].Name);
		Npc[indexid].m_GuiShuDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].m_Kind       = kind_partner;
		Npc[indexid].m_bClientOnly = FALSE;
		char nPCname[32];
		ZeroMemory(nPCname,32);
		ZeroMemory(Npc[indexid].Name,32); // 清零
        g_StrCpyLen(nPCname,Npc[nRindexid].Name,sizeof(Npc[nRindexid].Name));
		strcat(nPCname,"的(镖车)");
        t_sprintf(Npc[indexid].Name,"%s",nPCname);
        //t_sprintf(Npc[indexid].m_GuishuName,"%s",Npc[nRindexid].Name);
		//char nstrName[64]={0};
		Npc[indexid].SetstrInfo(STR_GUISHU_NAME,Npc[nRindexid].Name);
		return indexid;
	}
	else
	{
	    return 0;
	}

}

//刷同伴NPC
int KNpcSet::AddTongBan(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, IN int nSeries, IN int nComp,IN int nRindexid,int nBoss)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)//MAX_SUBWORLD
		return 0;
	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	int indexid;
	    indexid= AddServerNpcB(nNpcSettingIdxInfo, nSubWorld, nRegion, nMapX, nMapY, nOffX, nOffY,-1);
	if (indexid >0 && nRindexid>0 )
	{   
		Npc[indexid].m_Camp       = Npc[nRindexid].m_Camp;
		Npc[indexid].m_btRankFFId = 99;
		Npc[indexid].m_GuiShuDwid = Npc[nRindexid].m_dwID;
		Npc[indexid].m_IsRevive   = g_FileName2Id(Npc[nRindexid].Name);  // 名字的转换码
		Npc[indexid].m_Kind       = kind_partner;
		Npc[indexid].m_bClientOnly = FALSE;
		//t_sprintf(Npc[indexid].m_GuishuName,"%s",Npc[nRindexid].Name);
		//char nstrName[64]={0};
		Npc[indexid].SetstrInfo(STR_GUISHU_NAME,Npc[nRindexid].Name);

		return indexid;
	}
	else
	{
		return 0;
	}
	
}
//刷怪
int KNpcSet::AddServerNpcB(int nNpcSettingIdxInfo, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX /* = 0 */, int nOffY,int nBoss)
{
	int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //除于128

    if  (nNpcSettingIdx>g_NpcSetting.GetHeight()-2) //NPC.txt 的行数
	{
		//CCMessageBox("NPC增加失败","NPC增加失败");
	    return 0;
	}

	int i = FindFreeNpcSet();
	if (i <= 0)
	{//查找空闲节点
		//CCMessageBox("NPC已经满员(刷怪失败)","NPC已经满员(刷怪失败)");
	    return 0;
	}
	Npc[i].m_sClientNpcID.m_dwRegionID = 0;
	Npc[i].m_sClientNpcID.m_nNo = -1;
	Npc[i].Remove();                       //初始化 删除外观？
	//int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //除于128
	int nLevel         = LOWORD(nNpcSettingIdxInfo);// & 0x7f; 
	Npc[i].m_Index     = i; //节点的索引
	Npc[i].m_SkillList.m_nNpcIndex = i;
	Npc[i].Load(nNpcSettingIdx, nLevel,nSubWorld,nBoss);  //加载NPC信息等  人物类型 等
	Npc[i].m_SubWorldIndex = nSubWorld;
	Npc[i].m_RegionIndex   = nRegion;

	if (nRegion >= 0 && nRegion < 9)
		Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;

	Npc[i].m_MapX = nMapX;
	Npc[i].m_MapY = nMapY;
	Npc[i].m_OffX = nOffX;
	Npc[i].m_OffY = nOffY;	

	SubWorld[nSubWorld].NewMap2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &Npc[i].m_OriginX, &Npc[i].m_OriginY);
	// 修改可用与使用表
	m_FreeIdxNpcSet.Remove(i); //可用表删除一个
	m_UseIdxNpcSet.Insert(i);  //已用表增加一个
	SubWorld[nSubWorld].m_Region[nRegion].AddNpc(i);//增加节点m_WorldMessage.Send(GWM_NPC_ADD, nRegion, i);
	//SubWorld[nSubWorld].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);  //NPC 出现的位置
	SubWorld[nSubWorld].m_Region[nRegion].AddNpcRef(nMapX,nMapY);
	Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
//---------------------------------------------------------------------------
	/*Npc[i].m_DataRes.Init(Npc[i].szNpcTypeName, &g_NpcResList);  //外观初始化
	Npc[i].m_DataRes.SetAction(Npc[i].m_ClientDoing);            //设定外观行为对接Npc[i].m_ClientDoing
	Npc[i].m_DataRes.SetRideHorse(Npc[i].m_bRideHorse);          //设定是否骑马接帧数
	Npc[i].m_DataRes.SetArmor(Npc[i].m_ArmorType);               //衣服
	Npc[i].m_DataRes.SetHelm(Npc[i].m_HelmType);                 //头部
	Npc[i].m_DataRes.SetHorse(Npc[i].m_HorseType);               //马位置
	Npc[i].m_DataRes.SetWeapon(Npc[i].m_WeaponType);             //武器
	*/
  if (Npc[i].m_Kind==kind_player)
  {  
	 // CCMessageBox("玩家增加成功","玩家增加成功");
    //if (Npc[i].m_PifengType>0)
	//   Npc[i].m_DataRes.SetPifeng(Npc[i].m_PifengType);             //披风
	  //if (Npc[i].m_ChiBangType>0)
	    //Npc[i].m_DataRes.SetChiBang(Npc[i].m_ChiBangType);           //翅膀
  }  
//---------------------------------------------------------------------------
	return i;
}

void KNpcSet::Remove(int nIdx,BOOL isNeedSyn)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;
	//CCMessageBox("删除了npc","Remove npc");
	Npc[nIdx].ClearNpcState();
	Npc[nIdx].m_SkillList.Clear();
	Npc[nIdx].Remove(); ////外观删除
	m_FreeIdxNpcSet.Insert(nIdx);
	m_UseIdxNpcSet.Remove(nIdx);
}

void KNpcSet::RemoveAll()
{
	//CCMessageBox("删除了所有npc","RemoveAll npc");
	int nIdx = m_UseIdxNpcSet.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdxNpcSet.GetNext(nIdx);
		Npc[nIdx].ClearNpcState();
		Npc[nIdx].m_SkillList.Clear();
		Npc[nIdx].Remove();
		m_FreeIdxNpcSet.Insert(nIdx);
		m_UseIdxNpcSet.Remove(nIdx);
		nIdx = nIdx1;
	}
}

//---------------------------------------------------------------------------
//	功能：从npc数组中寻找属于某个region的 client npc ，添加进去
//---------------------------------------------------------------------------
void	KNpcSet::InsertNpcToRegion(int nRegionIdx)
{
	if (nRegionIdx < 0 || nRegionIdx >= MAX_REGION)
		return;
	int nIdx = 0;

	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);

		if (nIdx == 0)
		{//NPC 索引
			//CCMessageBox("nIdx is NULL break;","InsertNpcToRegion");
			break;
		}

		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID > 0 && Npc[nIdx].m_dwRegionID == (DWORD)SubWorld[0].m_Region[nRegionIdx].m_RegionID)
		{//如果是客户端NPC
			SubWorld[0].m_Region[nRegionIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegionIdx;
			Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegionIdx].m_RegionID;
			Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			Npc[nIdx].SendSerCommand(do_stand);
			//CCMessageBox("InsertNpcToRegion","InsertNpcToRegion");
		}
	}
}


void KNpcSet::SetID(int m_nIndex)
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;
	Npc[m_nIndex].m_dwID = m_dwIDCreator;
	m_dwIDCreator++; //设定一个后自加一次
}

int KNpcSet::GetDistance(int nIdx1, int nIdx2) //获取场景的距离
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += (Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10;

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff += (Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10;

		nRet = (int)sqrt(double(XOff * XOff) + YOff * YOff);
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].NewMap2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].NewMap2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)sqrt(double((X2 - X1)) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	}
	return nRet;
}

int		KNpcSet::GetDistanceSquare(int nIdx1, int nIdx2)
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += (Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10;

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff += (Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10;

		nRet = (int)(XOff * XOff + YOff * YOff);
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].NewMap2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].NewMap2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	}
	return nRet;
}


#define VOID_DIS 0x7FFFFFFF
INT	KNpcSet::GetMapDisX(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex < 0 || Npc[nIdx2].m_RegionIndex < 0)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((LOWORD(region1) - LOWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionWidth + Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX);
}

INT	KNpcSet::GetMapDisY(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex == -1 || Npc[nIdx2].m_RegionIndex == -1)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((HIWORD(region1) - HIWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionHeight + Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY);
}


int		KNpcSet::GetNextIdx(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_NPC)
		return 0;
	return m_UseIdxNpcSet.GetNext(nIdx);
}
//检测禁止物品
int  KNpcSet::CheckForBit(int *nVal,char *mKey,int Model)
{
  int nRows=g_ForbitMap.GetHeight()+1,nReg=0;

  for(int i=2;i<nRows;++i)
  { 
	  if (Model==1)
	  { 
		 int zVal; 
         g_ForbitMap.GetInteger(i,mKey,0,&zVal);
	     if (zVal==nVal[0])
		 { 
           nReg=1;
		   break;
		 } 
	  } 
	  else if(Model==2)
	  {
		  int zVal[2];   
		  g_ForbitMap.GetInt2s(i,mKey,zVal);
		  if ((zVal[0]==nVal[0])&&(zVal[1]==nVal[1]))
		  { 
			  nReg=1;
			  break;
		 } 

	  }
	  else if(Model==3)
	 {
		  int zVal[3];   
		  g_ForbitMap.GetInt3s(i,mKey,zVal);
		  if ((zVal[0]==nVal[0])&&(zVal[1]==nVal[1])&&(zVal[2]==nVal[2]))
		  { 
			  nReg=1;
			  break;
		  } 	  
	  }
  }

 return nReg;
}


BOOL KNpcSet::CheckThisNpc(int nSetings,char *nKey)
{
     
	int nRow = g_ForbitMap.GetHeight()+1,nReg=FALSE;
	
	for (int i=2;i<nRow;++i)
	{
		int nSkilid =0;
		g_ForbitMap.GetInteger(i,nKey,0,&nSkilid);
		if (nSkilid==nSetings)
		{
			nReg =TRUE;
			break;
		}
	}
	return nReg;   
}

void KNpcSet::CheckBalance()
{
	int nIdx;
	nIdx = m_UseIdxNpcSet.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)
		{
			if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				}
				Remove(nIdx);
			}
		}

		if (nIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 100)
		{
			if (g_GameWorld)
				g_GameWorld->setIsPaint(false);
		}

		/*if (Npc[nIdx].m_Kind!=kind_player && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)
		{//如果时间相差 1000毫秒 就删除 NPC 同步延时的怪物
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				 //if (Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				}
				Remove(nIdx);
		}
		else if (Npc[nIdx].m_Kind==kind_player && nIdx!=Player[CLIENT_PLAYER_INDEX].m_nIndex && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 100)
		{//人物的删除 除客户端本人
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
					//if (Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
				}
				Remove(nIdx);

		}
		else if (Npc[nIdx].m_Kind==kind_player && nIdx==Player[CLIENT_PLAYER_INDEX].m_nIndex && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 100)
		{//人物的删除 客户端本人 删除周围的精灵 卡号不同步的
			if (g_GameWorld)
				g_GameWorld->setIsPaint(false);
				//g_GameWorld->ParentNode_map->removeAllChildren();
               //g_GameWorld->FreeAllSprite();
		}
		*/
		nIdx = nTmpIdx;
	}
	nIdx = m_RequestUseIdx.GetNext(0);
	while(nIdx)
	{//删除请求超时里面的NPC
		int nTmpIdx = m_RequestUseIdx.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - m_RequestNpc[nIdx].dwRequestTime > 100)
		{
			DWORD	dwID = m_RequestNpc[nIdx].dwRequestId;
			m_RequestNpc[nIdx].dwRequestId = 0;	
			m_RequestNpc[nIdx].dwRequestTime = 0;
			
			m_RequestUseIdx.Remove(nIdx);
			m_RequestFreeIdx.Insert(nIdx);
		}
		nIdx = nTmpIdx;
	}
}

//-------------------------------------------------------------------------
//	功能：获得周围玩家列表，用于界面，队伍邀请列表
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount)
{
	int nCamp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp;
	int nNum = 0, i;

	if (nCount == 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			for (i = 0; i < MAX_TEAM_MEMBER; ++i)
			{//队员不显示
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
					break;
			}
			if (i < MAX_TEAM_MEMBER)
				continue;
			if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)  //队长不显示
				continue;
			nNum++;
		}	
		return nNum;
	}
	if (!pList)
		return 0;

	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		for (i = 0; i < MAX_TEAM_MEMBER; ++i)
		{
			if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
				break;
		}
		if (i < MAX_TEAM_MEMBER)
			continue;
		if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)
			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		nNum++;
	}
	
	return nNum;
}

//-------------------------------------------------------------------------
//	功能：获得周围玩家列表(用于列表)
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayer(KUiPlayerItem *pList, int nCount)
{
	int nNum = 0;

	if (nCount <= 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player ||
				nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
				Npc[nIdx].m_RegionIndex < 0)
			{
				continue;
			}
//			if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player ||
			nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
			Npc[nIdx].m_RegionIndex < 0)
		{
			continue;
		}
//		if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId    = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		pList[nNum].nData  = Npc[nIdx].GetMenuState();
		nNum++;
	}
	
	return nNum;
}

//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的名字
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowNameFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_NAME;
	else
		m_nShowPateFlag &= ~PATE_NAME;
}

//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的名字  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowName()
{
	return m_nShowPateFlag & PATE_NAME;
}

//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的血
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowLifeFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_LIFE;
	else
		m_nShowPateFlag &= ~PATE_LIFE;
}

//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的血  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowLife()
{
	return m_nShowPateFlag & PATE_LIFE;
}

//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的聊天
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowChatFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_CHAT;
	else
		m_nShowPateFlag &= ~PATE_CHAT;
}

//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的聊天  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowChat()
{
	return m_nShowPateFlag & PATE_CHAT;
}
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的内力
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowManaFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_MANA;
	else
		m_nShowPateFlag &= ~PATE_MANA;
}

//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的内力  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowMana()
{
	return m_nShowPateFlag & PATE_MANA;
}

//-------------------------------------------------------------------------
//	功能：把所有npc的 bActivateFlag 设为 FALSE
//		(每次游戏循环处理所有npc的activate之前做这个处理)
//-------------------------------------------------------------------------
void	KNpcSet::ClearActivateFlagOfAllNpc()
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		  Npc[nIdx].m_bActivateFlag = FALSE;
	}
}

//-------------------------------------------------------------------------
//	功能：获得周围同阵营的已开放队伍队长列表 不同阵营现在可以组队
//-------------------------------------------------------------------------
void	KNpcSet::GetAroundOpenCaptain(int nCamp)
{
	int		nIdx, nNum, nNo;

	nIdx = 0;
	nNum = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
		if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
			continue;
//		if (Npc[nIdx].m_Camp != nCamp)
//			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			nNum++;
	}
	
	if (nNum > 0)
	{
		KUiTeamItem* const pTeamList = new KUiTeamItem[nNum];
		nIdx = 0;
		nNo = 0;
		while (1)
		{
			nIdx = m_UseIdxNpcSet.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
			if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
				continue;
//			if (Npc[nIdx].m_Camp != nCamp)
//				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			{
				pTeamList[nNo].Leader.nIndex = nIdx;
				pTeamList[nNo].Leader.uId = Npc[nIdx].m_dwID;
				strcpy(pTeamList[nNo].Leader.Name, Npc[nIdx].Name);
				nNo++;
				if (nNo >= nNum)
					break;
			}
		}
		CoreDataChanged(GDCNI_TEAM_NEARBY_LIST, (unsigned int)pTeamList, nNo);
		delete []pTeamList;
	}
}
// 某座标上精确查找Npc，客户端专用
	// 用于客户端
int	KNpcSet::SearchNpcAt(int nX, int nY, int nRelation, int nRange)
{
	int nIdx;
	int	nMin = nRange;
	int nMinIdx = 0;
	int	nLength = 0;
	int nSrcX[2];
	int	nSrcY[2];

	nSrcX[0] = nX;
	nSrcY[0] = nY;
	//g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[0], nSrcY[0], 0);
	SubWorld[0].GetMpsByLocalPosition(nSrcX[0], nSrcY[0], 0);

	nSrcX[1] = nX;
	nSrcY[1] = nY;
	//g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[1], nSrcY[1], 120);
	SubWorld[0].GetMpsByLocalPosition(nSrcX[1], nSrcY[1], 120);

	int nDx = nSrcX[0] - nSrcX[1];
	int nDy = nSrcY[0] - nSrcY[1];

	nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdxNpcSet.GetNext(nIdx);

		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_RegionIndex < 0)
			continue;

		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;

		if (Npc[nIdx].m_bClientOnly)
			continue;

		if (!(GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIdx) & nRelation))
			continue;
		
		int x, y;
		SubWorld[0].NewMap2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY,
			Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &x, &y);

		if (nSrcY[0] > y)
			continue;

		if (nSrcY[0] < y - 120)
			continue;

		if  ((nDy + nSrcX[0] - x)==0)
			continue;

		nLength = abs(nDx * (nSrcY[0] - y) / nDy + nSrcX[0] - x);
		if (nLength < nMin)
		{
			nMin = nLength;
			nMinIdx = nIdx;
		}
	}

	return nMinIdx;
}


BOOL KNpcSet::IsNpcRequestExist(DWORD dwID)
{
	return (GetRequestIndex(dwID) > 0);
}

void KNpcSet::InsertNpcRequest(DWORD dwID)
{
	if (IsNpcRequestExist(dwID))
	{
		return;
	}

	int nIndex = m_RequestFreeIdx.GetNext(0);
	if (!nIndex)
		return;

	m_RequestNpc[nIndex].dwRequestId = dwID;
	m_RequestNpc[nIndex].dwRequestTime = SubWorld[0].m_dwCurrentTime;
	m_RequestFreeIdx.Remove(nIndex);
	m_RequestUseIdx.Insert(nIndex);
//	g_DebugLog("[Request]Insert %u at %d on %u", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

void KNpcSet::RemoveNpcRequest(DWORD dwID)
{
	if(!IsNpcRequestExist(dwID))
	{
		return;
	}
	int nIndex = GetRequestIndex(dwID);

	// because _ASSERT(IsNpcRequestExist()); so nIndex > 0;
	m_RequestNpc[nIndex].dwRequestId = 0;	
	m_RequestNpc[nIndex].dwRequestTime = 0;

	m_RequestUseIdx.Remove(nIndex);
	m_RequestFreeIdx.Insert(nIndex);
//	g_DebugLog("[Request]Remove %u from %d on %u", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

int KNpcSet::GetRequestIndex(DWORD dwID)
{
	int nIndex = m_RequestUseIdx.GetNext(0);

	while(nIndex)
	{
		if (m_RequestNpc[nIndex].dwRequestId == dwID)
		{
			return nIndex;
		}
		nIndex = m_RequestUseIdx.GetNext(nIndex);
	}
	return 0;
}

int KNpcSet::GetNPCBaiTan(int nIdex)
{
  return Npc[nIdex].m_BaiTan;
}

//敌友关系设置---或取与对方的关系
NPC_RELATION KNpcSet::GetRelation(int nId1, int nId2)
{
	if  (nId1<=0 || nId1>=MAX_NPC || nId2<=0 || nId2>=MAX_NPC) return relation_none;
	// 同一个人
	if (nId1 == nId2)
		return relation_self;

	if (Npc[nId1].m_bClientOnly || Npc[nId2].m_bClientOnly)
		return relation_none;

	if  (Npc[nId1].m_Kind<0 || Npc[nId1].m_Kind>=kind_num ||\
		Npc[nId2].m_Kind<0  || Npc[nId2].m_Kind>=kind_num ||\
		Npc[nId1].m_CurrentCamp<0 || Npc[nId1].m_CurrentCamp>=camp_num||\
		Npc[nId2].m_CurrentCamp<0 || Npc[nId2].m_CurrentCamp>=camp_num)
		return relation_none;

//客户端
	char nstrNamea[64]={0},nstrNameb[64]={0};
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex != nId1 && Player[CLIENT_PLAYER_INDEX].m_nIndex != nId2)  //对方  怪物？
	{//怪物 怪物之间	本人  与  怪物     别人与  怪物
		//char nstrName[64]={0};
		Npc[nId1].GetstrInfo(STR_GUISHU_NAME,nstrNamea);
		Npc[nId2].GetstrInfo(STR_GUISHU_NAME,nstrNameb);

		if (Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_normal)
			return relation_ally;

		if (Npc[nId2].m_Kind == kind_partner && (strcmp(Npc[nId1].Name, nstrNameb) == 0))
	  	    return relation_ally;  // 同伴

        if (Npc[nId2].m_Kind == kind_partner && (strcmp(Npc[nId1].Name,nstrNameb) != 0))
		{//不是自己的宠物
		   int mNpcIdx=NpcSet.SearchName(nstrNameb);   //查找归属者的索引
			 if (mNpcIdx>0 && Npc[mNpcIdx].m_Kind==kind_player && Npc[nId1].m_Kind==kind_player)
			 {
			    if ((Npc[nId1].m_CurrentCamp == Npc[mNpcIdx].m_CurrentCamp) && (Npc[nId1].m_CurrentCamp!=camp_free || Npc[mNpcIdx].m_CurrentCamp!=camp_free))
                   return relation_ally;

                if ((Npc[nId1].m_nPKFlag!=0 || Npc[mNpcIdx].m_nPKFlag!=0)&& Npc[nId1].m_FightMode!=0 && Npc[mNpcIdx].m_FightMode!=0)
				   return relation_enemy;

				  return relation_none;
			 }
		}

		if (Npc[nId1].m_Kind == kind_partner && (strcmp(Npc[nId2].Name, nstrNamea) == 0))
	  	    return relation_ally;  // 同伴


		if (Npc[nId1].m_Kind == kind_partner && (strcmp(Npc[nId2].Name, nstrNamea) != 0))
		{//不是自己的宠物
		   int mNpcIdx=NpcSet.SearchName(nstrNamea);   //查找归属者的索引
			 if (mNpcIdx>0 && Npc[mNpcIdx].m_Kind==kind_player && Npc[nId2].m_Kind==kind_player)
			 {
			    if ((Npc[nId2].m_CurrentCamp == Npc[mNpcIdx].m_CurrentCamp) && (Npc[nId2].m_CurrentCamp!=camp_free || Npc[mNpcIdx].m_CurrentCamp!=camp_free))
                   return relation_ally;

                if ((Npc[nId2].m_nPKFlag!=0 || Npc[mNpcIdx].m_nPKFlag!=0) && Npc[nId2].m_FightMode!=0 && Npc[mNpcIdx].m_FightMode!=0)
				   return relation_enemy;

				  return relation_none;
			 }
		}

		if ((Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_partner) || (Npc[nId1].m_Kind == kind_partner && Npc[nId2].m_Kind == kind_normal)) //ndi1 为怪物
			return relation_enemy;  //同伴与怪物的关系

		if ((Npc[nId2].m_CurrentCamp == Npc[nId1].m_CurrentCamp) && (Npc[nId2].m_CurrentCamp!=camp_free || Npc[nId1].m_CurrentCamp!=camp_free))
			return relation_ally;

		return (NPC_RELATION)GenOneRelation(
			(NPCKIND)Npc[nId1].m_Kind, 
			(NPCKIND)Npc[nId2].m_Kind, 
			(NPCCAMP)Npc[nId1].m_CurrentCamp, 
			(NPCCAMP)Npc[nId2].m_CurrentCamp);
	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nId1) 
	{//本人 和 对方的关系()
		//Npc[nId1].GetstrInfo(STR_GUISHU_NAME,nstrNamea);
		Npc[nId2].GetstrInfo(STR_GUISHU_NAME,nstrNameb);

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetExercisePKAim() == Npc[nId2].m_dwID)  // 切磋状态
			return relation_enemy;
		
		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKAimNpcID() == Npc[nId2].m_dwID)  //仇杀
			return relation_enemy;

        if ((Npc[nId1].m_CurrentCamp == Npc[nId2].m_CurrentCamp) && (Npc[nId1].m_CurrentCamp!=camp_free || Npc[nId2].m_CurrentCamp!=camp_free))
		{ 
			return relation_ally;  //人与人 或 人与怪
		}

		if (Npc[nId2].m_Kind == kind_partner && ((strcmp(Npc[nId1].Name, nstrNameb) == 0) || Npc[nId1].m_CurrentCamp == Npc[nId2].m_CurrentCamp))
			return relation_ally;  // 同伴 同盟

		if (Npc[nId2].m_Kind == kind_partner && (strcmp(Npc[nId1].Name, nstrNameb) != 0))
		{//不是自己的宠物
		   int mNpcIdx=NpcSet.SearchName(nstrNameb);   //查找归属者的索引
			 if (mNpcIdx>0 && Npc[mNpcIdx].m_Kind==kind_player && Npc[nId1].m_Kind==kind_player)
			 {
			    if ((Npc[nId1].m_CurrentCamp == Npc[mNpcIdx].m_CurrentCamp) && (Npc[nId1].m_CurrentCamp!=camp_free || Npc[mNpcIdx].m_CurrentCamp!=camp_free))
                   return relation_ally;

                if ((Npc[nId1].m_nPKFlag!=0 || Npc[mNpcIdx].m_nPKFlag!=0)&& Npc[nId1].m_FightMode!=0 && Npc[mNpcIdx].m_FightMode!=0)
				   return relation_enemy;

				  return relation_none;
			 }
		}
			
        if (Npc[nId2].m_Kind == kind_player && (Npc[nId1].m_FightMode==0 || Npc[nId2].m_FightMode==0))  //任意一方为非战斗模式
            return relation_none;  // 和平

		if (Npc[nId2].m_Kind == kind_player && (Npc[nId2].m_nPKFlag == 2 || Npc[nId1].m_nPKFlag == 2)&&(Npc[nId1].m_CurrentCamp !=camp_begin||Npc[nId2].m_CurrentCamp !=camp_begin)) // 任意一方为屠杀模式
			return relation_enemy;

		if (Npc[nId2].m_Kind == kind_player && (Npc[nId2].m_nPKFlag ==0 || Npc[nId1].m_nPKFlag ==0))  //任意一方为和平模式
			return relation_none;

		return (NPC_RELATION)GenOneRelation(
			(NPCKIND)Npc[nId1].m_Kind, 
			(NPCKIND)Npc[nId2].m_Kind, 
			(NPCCAMP)Npc[nId1].m_CurrentCamp, 
			(NPCCAMP)Npc[nId2].m_CurrentCamp);
	}
	else	// if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nId2)  
	{//玩家之间	或 怪物 与 怪物
		if (Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_normal)
			return relation_ally;

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetExercisePKAim() == Npc[nId1].m_dwID)
			return relation_enemy;
		
		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKAimNpcID() == Npc[nId1].m_dwID)
			return relation_enemy;


		if (Npc[nId1].m_CurrentCamp == Npc[nId2].m_CurrentCamp && (Npc[nId1].m_CurrentCamp!=camp_free || Npc[nId2].m_CurrentCamp!=camp_free))
		{
			if (Npc[nId1].m_Kind == kind_normal && Npc[nId2].m_Kind == kind_normal)
				return relation_ally;
			//else if (Npc[nId1].m_Kind == kind_normal || Npc[nId2].m_Kind == kind_normal)
			//	return relation_enemy;
			else
				return relation_ally;
		}

		if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_player && (Npc[nId1].m_FightMode ==0 || Npc[nId2].m_FightMode ==0))  //玩家 只要有一方是非战斗模式 就是和平模式
           return relation_none;

		if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_player && (Npc[nId1].m_nPKFlag == 2 || Npc[nId2].m_nPKFlag == 2)&&\
			(Npc[nId1].m_CurrentCamp !=camp_begin||Npc[nId2].m_CurrentCamp !=camp_begin)) //任意一方为屠杀模式都是 敌对关系
			return relation_enemy;

		if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_player && (!Npc[nId1].m_nPKFlag || !Npc[nId2].m_nPKFlag))
			return relation_none;
		if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_player && (Npc[nId1].m_nPKFlag == 0 || Npc[nId2].m_nPKFlag == 0))
			return relation_none;

		return (NPC_RELATION)GenOneRelation(
			(NPCKIND)Npc[nId1].m_Kind, 
			(NPCKIND)Npc[nId2].m_Kind, 
			(NPCCAMP)Npc[nId1].m_CurrentCamp, 
			(NPCCAMP)Npc[nId2].m_CurrentCamp);
	}
//服务器端
}
//------------------------优化的 新函数----------------------------------------
KNpcTemplate * KNpcSet::GetTemplate(INT nNpcTemplateId,INT nLevel)
{
	if (nNpcTemplateId < 0 || nNpcTemplateId > MAX_NPCSTYLE-1 || nLevel<0 || nLevel > MAX_NPC_LEVEL-1)
	{
		return NULL;
	} 

	char nFlag[32]={0};
	t_sprintf(nFlag,"idx_%d_level_%d",nNpcTemplateId,nLevel);	  //这个IDX 这个等级的数据

	DWORD dwKey = g_FileName2Id(nFlag);

	if (nNpcTemplateId >= 0 && nNpcTemplateId <= MAX_NPCSTYLE-1 /*&& eSeries >= series_metal && eSeries < series_num*/ && nLevel >= 0 && nLevel <= MAX_NPC_LEVEL-1)
	{
		// 尝试查找已有数据
		//DWORD dwKey	= nKeyInfo;//((DWORD)nNpcTemplateId << 16) | ((DWORD)eSeries << 8) | (DWORD)nLevel; //((DWORD)nLevel << 8); /*
		_KMapTemplate::iterator it	= m_mapTemplate.find(dwKey);
		if (it != m_mapTemplate.end())
		{
			return it->second;	   //找到了
		}
		
		// 先载入0级基础数据
		t_sprintf(nFlag,"idx_%d_level_0",nNpcTemplateId);
		DWORD dwBaseKey	= g_FileName2Id(nFlag);//dwKey & 0xffffff00;
		it	= m_mapTemplate.find(dwBaseKey);
		if (it == m_mapTemplate.end())
		{
			KNpcTemplate* pTemplate		= new KNpcTemplate;
			if (!pTemplate->InitNpcBaseData(nNpcTemplateId))
			{
				//SAFE_DELETE(pTemplate);
				if (pTemplate)
				{
					delete pTemplate;
				    pTemplate =NULL;
				}
				return NULL;
			}
			//pTemplate->m_Series		= eSeries;//不修改 基本值
			m_mapTemplate[dwBaseKey]	= pTemplate;

		}
		
		// 如果要的就是0级的，直接返回
		KNpcTemplate* pBaseTemplate	= m_mapTemplate[dwBaseKey];

		if (nLevel == 0)
		{
            //#ifdef _SERVER
			//printf("---[要求返回基本数据]找到NPC(%s):%d 等级节点:%d---\n",pBaseTemplate->Name,nNpcTemplateId,nLevel);
            //#endif
			return pBaseTemplate;
		}
		//开始加载脚本数据--------------------------------------

		KLuaScript * pLevelScript = NULL;
		KLuaScript LevelScript;
		if (!pBaseTemplate->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			LevelScript.Init();   //初始化 脚本函数
			if (!LevelScript.Load(pBaseTemplate->m_szLevelSettingScript))
			{
				pLevelScript = g_pNpcLevelScript;
				LevelScript.Exit();
			}
			else
				pLevelScript = &LevelScript;
		}
		// 载入等级相关数据-------------------------------------
		KNpcTemplate* pTemplate	= new KNpcTemplate;
		*pTemplate			    = *pBaseTemplate;
		pTemplate->m_nLevel	    = nLevel;
//#ifdef _SERVER
//		printf("---[创建新等级节点]找到NPC(%s):%d 新等级节点:%d---\n",pBaseTemplate->Name,nNpcTemplateId,nLevel);
//#endif

		if (!pTemplate->InitNpcLevelData(nNpcTemplateId,pLevelScript,nLevel))
		{
			//SAFE_DELETE(pTemplate);
			if (pTemplate)
			{
				delete pTemplate;
				pTemplate =NULL;
			} 
			return NULL;
		}
		m_mapTemplate[dwKey]	= pTemplate; //赋值给这个节点
		
		return pTemplate;
	}else
		return NULL;
}

//------------------------ class KInstantSpecial start -------------------------
KInstantSpecial::KInstantSpecial()
{
	int		i;
	this->m_nLoadFlag = FALSE;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)
		this->m_szSprName[i][0] = 0;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
		this->m_szSoundName[i][0] = 0;

	m_pSoundNode = NULL;
//	m_pWave = NULL;
}
 //瞬间特效
void	KInstantSpecial::LoadSprName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)
		m_szSprName[i][0] = 0;

	KTabFile	cSprName;
//	g_SetFilePath("\\");
	if (!cSprName.Load(PLAYER_INSTANT_SPECIAL_FILE))  //瞬间特效
		return;
	for (i = 0; i < MAX_INSTANT_STATE; ++i)           //特效限制
		cSprName.GetString(i + 2, 3, "", m_szSprName[i], sizeof(m_szSprName[i]));

	cSprName.Clear();
}

void	KInstantSpecial::LoadSoundName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
		m_szSoundName[i][0] = 0;

	KIniFile	cSoundName;
	char		szTemp[32];
//	g_SetFilePath("\\");
	if (!cSoundName.Load(defINSTANT_SOUND_FILE))
		return;
	for (i = 0; i < MAX_INSTANT_SOUND; ++i)
	{
		t_sprintf(szTemp, "%d", i);
		cSoundName.GetString("Game", szTemp, "", this->m_szSoundName[i], sizeof(m_szSoundName[i]));
	}
	cSoundName.Clear();
}

void	KInstantSpecial::GetSprName(int nNo, char *lpszName, int nLength)
{
	if (!lpszName || nLength <= 0)
		return;
	if (nNo < 0 || nNo >= MAX_INSTANT_STATE)
	{
		lpszName[0] = 0;
		return;
	}
	if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}

	if (strlen(this->m_szSprName[nNo]) < (DWORD)nLength)
		strcpy(lpszName, m_szSprName[nNo]);
	else
		lpszName[0] = 0;
}

void	KInstantSpecial::PlaySound(int nNo)
{
	/*if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}
	if (nNo < 0 || nNo >= MAX_INSTANT_SOUND)
		return;
	if ( !m_szSoundName[nNo][0] )
		return;

	m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName[nNo], (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
	}*/
}

//------------------------- class KInstantSpecial end --------------------------
