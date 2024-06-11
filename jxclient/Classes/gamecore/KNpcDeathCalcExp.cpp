//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcDeathCalcExp.cpp
// Date:	2003.07.21
// Code:	边城浪子
// Desc:	KNpcDeathCalcExp Class   经验分配
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayerDef.h"
#include	"KPlayer.h"
#include	"KNpcDeathCalcExp.h"

void	KNpcDeathCalcExp::Init(int nNpcIdx)
{
	m_nNpcIdx = (nNpcIdx > 0 ? nNpcIdx : 0);
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
	ZeroMemory(&m_sCalcInfo,sizeof(m_sCalcInfo));
}

void	KNpcDeathCalcExp::Active()
{
	if (Npc[m_nNpcIdx].m_Kind != kind_normal)
		return;

	for (int i = 0; i < defMAX_CALC_EXP_NUM; ++i)
	{
		if (m_sCalcInfo[i].m_nAttackIdx <= 0 || m_sCalcInfo[i].m_nAttackIdx>=MAX_PLAYER)
			continue;

		int nDistance = KNpcSet::GetDistanceSquare(m_nNpcIdx, Player[m_sCalcInfo[i].m_nAttackIdx].m_nIndex);
		int nSerDistance = 400;
		g_GameSetTing.GetInteger("SkillsIcon","ExpDisDistance",400,&nSerDistance);
		if  (nDistance>nSerDistance)
		{//如果两者的距离范围 大于 800 马上清零
			m_sCalcInfo[i].m_nTime = 0;
			m_sCalcInfo[i].m_nAttackIdx = 0;     //经验所属者
			m_sCalcInfo[i].m_nTotalDamage = 1;   //经验消失
			continue;
		}

		m_sCalcInfo[i].m_nTime--;               //保留的时间

		if (m_sCalcInfo[i].m_nTime <= 0)
		{
			m_sCalcInfo[i].m_nTime = 0;
			m_sCalcInfo[i].m_nAttackIdx = 0;     //经验所属者
			m_sCalcInfo[i].m_nTotalDamage = 1;   //经验消失
		}
	}
}
void KNpcDeathCalcExp::Clear()
{
	memset(m_sCalcInfo, 0, sizeof(m_sCalcInfo));
	ZeroMemory(&m_sCalcInfo,sizeof(m_sCalcInfo));
}



