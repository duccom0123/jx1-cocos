#include "KCore.h"
#include "KPlayerFaction.h"
#include "KLadder.h"

KLadder	Ladder;

KLadder::KLadder()
{
	ZeroMemory(&GameStatData, sizeof(TGAME_STAT_DATA));
}

KLadder::~KLadder()
{
	ZeroMemory(&GameStatData, sizeof(TGAME_STAT_DATA));
}

BOOL KLadder::Init(void* pData, size_t uSize)
{
	if (uSize != sizeof(TGAME_STAT_DATA))
		return FALSE;
//获取排名数据包
	memcpy(&GameStatData, pData, uSize);
	return TRUE;
}
//门派中前10等级排名
const TRoleList* KLadder::TopTenFacMasterHand(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)
		return NULL;
	return GameStatData.LevelStatBySect[nFac + 1];   //0在野    1-10
}
//门派中前10金钱排名
const TRoleList* KLadder::TopTenFacRich(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)  //10个门派的排名
		return NULL;
	return GameStatData.MoneyStatBySect[nFac + 1];   //0为在野	 1-10
}
//门派中前10金币排名
const TRoleList* KLadder::TopTenFacXu(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)  //10个门派的排名
		return NULL;
	return GameStatData.XuStatBySect[nFac + 1];
}
//世界前10杀人排名
const TRoleList* KLadder::TopTenKiller()
{
	return GameStatData.KillerStat;
}
//世界前10等级排名
const TRoleList* KLadder::TopTenMasterHand()
{
	return GameStatData.LevelStat;
}
//世界前10金钱排名
const TRoleList* KLadder::TopTenRich()
{
	return GameStatData.MoneyStat;
}
//世界前10金币排名
const TRoleList* KLadder::TopTenXu()
{
	return GameStatData.XuStat;
}

//门派中等级排名的占有率
int KLadder::GetFacMasterHandPercent(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)
		return 0;
	return GameStatData.SectLevelMost[nFac + 1];
}
//门派中金钱排名的占有率
int KLadder::GetFacMoneyPercent(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)
		return NULL;
	return GameStatData.SectMoneyMost[nFac + 1];
}
//各个门派的人数
int KLadder::GetFacMemberCount(int nFac)
{
	if (nFac < - 1 || nFac >= series_num * FACTIONS_PRR_SERIES)
		return NULL;
	return GameStatData.SectPlayerNum[nFac + 1];
}

const TRoleList* KLadder::GetTopTen(DWORD dwLadderID)  //原数据 1-26个
{
	if (dwLadderID <= enumLadderBegin || dwLadderID >= enumLadderEnd)  //0----38
	{																			 
		return NULL;
	}

	if (dwLadderID == enumTopTenMasterHand)
	{
		return GameStatData.LevelStat;  //世界等级排名
	}
	else if (dwLadderID == enumTopTenRicher)
	{
		return GameStatData.MoneyStat;  //世界金钱排名
	}
	else if (dwLadderID == enumTopTenKiller)
	{
		return GameStatData.KillerStat;  //世界杀人排名
	}
	else if (dwLadderID == enumTopTenXu)
	{
		return GameStatData.XuStat;       //世界金币排名
	}
	else if (dwLadderID < enumFacTopTenRicher)   //5-15  门派等级排行  5 为在野十大排行
	{
		return TopTenFacMasterHand(dwLadderID - enumFacTopTenMasterHand - 1);  //门派中前10等级排名(0 -- 9)
	}
	else //大于等于15 就是门派中前10金钱排名  15为在野十大富豪	 16-26
	{
		return TopTenFacRich(dwLadderID - enumFacTopTenRicher - 1);  //门派金钱排名	 (0-9)
	}
}