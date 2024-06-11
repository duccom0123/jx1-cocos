//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "KProtocolProcess.h"
/*#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#endif*/
#include "KPlayerSet.h"
#include "engine/Text.h"
#include "CoreUseNameDef.h"

#define		PLAYER_FIRST_LUCKY				0   //原始幸运

KPlayerSet PlayerSet;

KPlayerSet::KPlayerSet()
{
}

BOOL	KPlayerSet::Init()
{
	int i;
	
	// 优化查找表
	m_FreeIdxPlayerSet.Init(MAX_PLAYER);
	m_UseIdxPlayerSet.Init(MAX_PLAYER);

	// 开始时所有的数组元素都为空
	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreeIdxPlayerSet.Insert(i);
	}

	if ( !m_cLevelAdd.Init() )
		return FALSE;
	if ( !m_cLeadExp.Init() )
		return FALSE;
	for (i = 0; i < MAX_PLAYER; ++i)
	{
		Player[i].Release();
		Player[i].SetPlayerIndex(i);
//		Player[i].m_cTong.Init(i);     //帮会初始化
		Player[i].m_ItemList.Init(i);  //容器初始化
		Player[i].m_Node.m_nIndex = i; //节点索引
	}

	if (!m_cNewPlayerAttribute.Init())  //玩家的基本信息
		return FALSE;
	// 帮会参数
	KIniFile	cTongFile;
	if (cTongFile.Load(defPLAYER_TONG_PARAM_FILE))
	{
		cTongFile.GetInteger("TongCreate", "Level", 60, &m_sTongParam.m_nLevel);  //等级
		cTongFile.GetInteger("TongCreate", "LeadLevel", 10, &m_sTongParam.m_nLeadLevel);  //统帅
		cTongFile.GetInteger("TongCreate", "Money",500000, &m_sTongParam.m_nMoney);  //金钱
	}
	else
	{
		m_sTongParam.m_nLevel		= 60;
		m_sTongParam.m_nLeadLevel	= 10;
		m_sTongParam.m_nMoney		= 500000;
	}
    //printf(" KPlayerSet::Init(%d) OK!!!..\n",MAX_PLAYER);
	cTongFile.Clear();
	return TRUE;
}

int	KPlayerSet::FindFree()
{
	return m_FreeIdxPlayerSet.GetNext(0);
}

int KPlayerSet::FindSame(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}


int KPlayerSet::CheckLiXian(LPSTR szName)
{
	int nUseIdx = 0;	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx && Player[nUseIdx].m_nNetConnectIdx>=0)
	{
		if (g_StrCmp(Npc[Player[nUseIdx].m_nIndex].Name, szName))
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}


int KPlayerSet::FindByTongName(LPSTR szTongName)
{
	/*int nUseIdx = 0;	
	    nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_cTong.m_nFlag)
		{//已经入帮的
			if (g_StrCmp(Player[nUseIdx].m_cTong.m_szName,szTongName))
			{//如果帮会名相同,就恢复阵营
				Npc[Player[nUseIdx].m_nIndex].RestoreCurrentCamp();
			}
		}
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}*/
	return TRUE;
}

int KPlayerSet::FindNameID(LPSTR szName)
{
	int nUseIdx = 0;	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
	if (g_StrCmp(Npc[Player[nUseIdx].m_nIndex].Name, szName))
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}
//服务器端用的
int KPlayerSet::FindPlayerID(DWORD dwID)
{
	int nUseIdx = 0;
	
	nUseIdx = m_UseIdxPlayerSet.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nIndex == dwID)
	//		if (Npc[Player[nUseIdx].m_nIndex].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
	}
	return 0;
}

int	KPlayerSet::GetFirstPlayer()
{
	m_nListCurIdx = m_UseIdxPlayerSet.GetNext(0);
	return m_nListCurIdx;
}

int		KPlayerSet::GetNextPlayer()
{
	if ( !m_nListCurIdx )
		return 0;
	m_nListCurIdx = m_UseIdxPlayerSet.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

//取商
/*DWORD KPlayerSet::TakeTrader(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,eax
			//mov nYuShu,edx
	}
	return nRet;
}
//取余数
DWORD KPlayerSet::TakeRemainder(DWORD a,DWORD b)
{
	DWORD nRet = 0;
	//	DWORD nYuShu=0;
	__asm
	{
		mov eax,a
			mov ecx,b
			xor edx,edx
			idiv ecx
			mov nRet,edx
			//mov nYuShu,edx
	}
	return nRet;
}
*/
//增加玩家链表信息

//根据连接号 查找 playerid
int KPlayerSet::FindClient(int nClient)
{
	int i;
	i = m_UseIdxPlayerSet.GetNext(0);
	while(i)
	{
		if (Player[i].m_nNetConnectIdx == nClient)
		{
			return i;
		}
		i = m_UseIdxPlayerSet.GetNext(i);
	}
	return 0;
}
//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KLevelAdd::KLevelAdd()
{
	memset(m_nLevelExp, 0, sizeof(int) * MAX_LEVEL);
	memset(m_nLifePerLevel, 0, sizeof(int) * series_num);
	memset(m_nManaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nLifePerVitality, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerVitality, 0, sizeof(int) * series_num);
	memset(m_nManaPerEnergy, 0, sizeof(int) * series_num);
}

//---------------------------------------------------------------------------
//	功能：初始化
//---------------------------------------------------------------------------
BOOL	KLevelAdd::Init()
{
	int			i;
	KTabFile	LevelExp;
	if ( !LevelExp.Load(PLAYER_LEVEL_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEVEL; ++i)
	{
		LevelExp.GetInteger(i + 2, 2, 0, &m_nLevelExp[i]);
	}
	LevelExp.Clear();

	KTabFile	LevelAdd;
	if ( !LevelAdd.Load(PLAYER_LEVEL_ADD_FILE) )
		return FALSE;
	for (i = 0; i < series_num; ++i)
	{
		LevelAdd.GetInteger(i + 2, 2, 0, &m_nLifePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 3, 0, &m_nStaminaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 4, 0, &m_nManaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 5, 0, &m_nLifePerVitality[i]);
		LevelAdd.GetInteger(i + 2, 6, 0, &m_nStaminaPerVitality[i]);
		LevelAdd.GetInteger(i + 2, 7, 0, &m_nManaPerEnergy[i]);
		LevelAdd.GetInteger(i + 2, 8, 0, &m_nLeadExpShare[i]);
		LevelAdd.GetInteger(i + 2, 9, 0, &m_nFireResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 10, 0, &m_nColdResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 11, 0, &m_nPoisonResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 12, 0, &m_nLightResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 13, 0, &m_nPhysicsResistPerLevel[i]);
	}
	LevelAdd.Clear();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：获得某等级的升级经验
//	参数：nLevel  目标等级
//---------------------------------------------------------------------------
int	KLevelAdd::GetLevelExp(int nLevel)
{
	if (nLevel < 1 || nLevel > MAX_LEVEL)
		return 0;
	return m_nLevelExp[nLevel - 1];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加生命点
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加体力点
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：获得每个系升级加内力点
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系活力点增加一点后生命点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系活力点增加一点后体力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系精力点增加一点后内力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerEnergy(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerEnergy[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系精力点增加一点后内力点增长
//---------------------------------------------------------------------------
int		KLevelAdd::GetLeadExpShare(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLeadExpShare[nSeries];
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本火抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetFireResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nFireResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本冰抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetColdResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nColdResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本毒抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetPoisonResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPoisonResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本电抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetLightResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nLightResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：每个系某个等级的基本物理抗性
//---------------------------------------------------------------------------
int		KLevelAdd::GetPhysicsResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPhysicsResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KTeamLeadExp::KTeamLeadExp()
{
	for (int i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		m_sPerLevel[i].m_dwExp = 0;
		m_sPerLevel[i].m_dwMemNum = 1;
	}
}

//---------------------------------------------------------------------------
//	功能：初始化，读取统率力数据
//---------------------------------------------------------------------------
BOOL	KTeamLeadExp::Init()
{
	int			i;
	KTabFile	LevelExp;

	if ( !LevelExp.Load(PLAYER_LEVEL_LEAD_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		LevelExp.GetInteger(i + 2, 2, 0, (int*)&m_sPerLevel[i].m_dwExp);
		LevelExp.GetInteger(i + 2, 3, 1, (int*)&m_sPerLevel[i].m_dwMemNum);  //能最组队的人数
	}
	LevelExp.Clear();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：传入经验获得等级数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevel(DWORD dwExp, int nCurLeadLevel)
{
	if (dwExp <= 0)
		return 1;
	if (nCurLeadLevel > 0)
	{
		if (nCurLeadLevel >= MAX_LEAD_LEVEL)
			return MAX_LEAD_LEVEL;
		if (dwExp < m_sPerLevel[nCurLeadLevel - 1].m_dwExp)
			return nCurLeadLevel;
		if (dwExp < m_sPerLevel[nCurLeadLevel].m_dwExp)
			return nCurLeadLevel + 1;
	}
	for (int i = 0; i < MAX_LEAD_LEVEL; ++i)
	{
		if (dwExp < m_sPerLevel[i].m_dwExp)
		{
			return i + 1;
		}
	}
	return MAX_LEAD_LEVEL;
}

//---------------------------------------------------------------------------
//	功能：传入经验获得可带队员数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromExp(DWORD dwExp)
{
	int nGetLevel;
	nGetLevel = GetLevel(dwExp);
	return m_sPerLevel[nGetLevel - 1].m_dwMemNum;
}

//---------------------------------------------------------------------------
//	功能：传入等级获得可带队员数
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromLevel(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return m_sPerLevel[nLevel - 1].m_dwMemNum;
	return 1;
}

//---------------------------------------------------------------------------
//	功能：传入等级获得升级所需经验值
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevelExp(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return this->m_sPerLevel[nLevel - 1].m_dwExp;
	return 0;
}

KNewPlayerAttribute::KNewPlayerAttribute()
{
	memset(m_nStrength, 0, sizeof(m_nStrength));
	memset(m_nDexterity, 0, sizeof(m_nDexterity));
	memset(m_nVitality, 0, sizeof(m_nVitality));
	memset(m_nEngergy, 0, sizeof(m_nEngergy));
	memset(m_nLucky, 0, sizeof(m_nLucky));
}

BOOL	KNewPlayerAttribute::Init()
{
	char		szSeries[5][16] = {"metal", "wood", "water", "fire", "earth"};
	KIniFile	AttributeFile;

	if ( !AttributeFile.Load(BASE_ATTRIBUTE_FILE_NAME) )
		return FALSE;

	for (int i = 0; i < series_num; ++i)
	{
		AttributeFile.GetInteger(szSeries[i], "Strength", 0, &m_nStrength[i]);
		AttributeFile.GetInteger(szSeries[i], "Dexterity", 0, &m_nDexterity[i]);
		AttributeFile.GetInteger(szSeries[i], "Vitality", 0, &m_nVitality[i]);
		AttributeFile.GetInteger(szSeries[i], "Engergy", 0, &m_nEngergy[i]);
		AttributeFile.GetInteger(szSeries[i], "Lucky", PLAYER_FIRST_LUCKY, &m_nLucky[i]);
	}
	AttributeFile.Clear();
	return TRUE;
}
//游戏中定时存档 玩家自动存档
/*#ifdef _SERVER
void KPlayerSet::AutoSave()
{
	unsigned long uTime = g_SubWorldSet.GetGameTime();	//服务器运行的 循环数

	if (uTime >= m_ulNextSaveTime)//服务器的下次存档时间
	{
		int nUseIdx = m_UseIdxPlayerSet.GetNext(0);

		while(nUseIdx)
		{
			//printf("--循环:%d,登录:%d,间隔:%d,下次:%d--\n",uTime,Player[nUseIdx].m_ulLastSaveTime,m_ulMaxSaveTimePerPlayer,m_ulNextSaveTime); 
			if (Player[nUseIdx].CanSave() && uTime - Player[nUseIdx].m_ulLastSaveTime >= m_ulMaxSaveTimePerPlayer)
			{//个人存档时间
				if (Player[nUseIdx].Save())
				{//生成数据包
					Player[nUseIdx].m_uMustSave = SAVE_REQUEST;
					Player[nUseIdx].m_ulLastSaveTime = uTime;
					m_ulNextSaveTime += m_ulDelayTimePerSave;
					Player[nUseIdx].SavePlayerData(nUseIdx,false);
					const char *nPinfo=NULL;
					if (g_pServer)
					    nPinfo=g_pServer->GetClientInfo(Player[nUseIdx].m_nNetConnectIdx);
					if (nPinfo) 
						sprintf(Player[nUseIdx].m_PlayerIpInfo,nPinfo);

					break;
				}
			}
			nUseIdx = m_UseIdxPlayerSet.GetNext(nUseIdx);
		}
	///	m_ulNextSaveTime += m_ulDelayTimePerSave; //服务器下次存档的时间
	}
}
#endif	*/
