//---------------------------------------------------------------------------
// File:	KPlayerVip.cpp
// Date:	2019.08.23
// Code:	QQ:25557432
// Desc:	PlayerRoleVip Class 会员
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerVip.h"
#include    "CoreShell.h"
//-------------------------------------------------------------------------
void	KPlayerVip::SetRoleVipValue(BYTE* pMsg)
{

	ROLEVIP_VALUE_SYNC	*pValue = (ROLEVIP_VALUE_SYNC*)pMsg;

	if (m_nRoleVipValue == pValue->m_nRoleVipValue)
	{
		return;
	}

	this->m_nRoleVipValue = (pValue->m_nRoleVipValue < 0 ? 0 : pValue->m_nRoleVipValue);
  /*
	if (m_nRoleVipValue > MAX_RoleVip_VALUE)
		m_nRoleVipValue = MAX_RoleVip_VALUE;
	*/
	this->m_nRoleVipLevel =	pValue->m_nRoleVipLevel;
	this->m_nRoleVipKeepQpiont =	pValue->m_nRoleVipKeepQpiont;
	this->m_nRoleVipKeepJpiont =	pValue->m_nRoleVipKeepJpiont;
	this->m_nRoleVipSetSkillMaxLevel = pValue->m_nRoleVipSetSkillMaxLevel;
	this->m_nRoleVipSetFanYuMaxVal =	pValue->m_nRoleVipSetFanYuMaxVal;
	this->m_nCurRoleVipNum=pValue->m_nCurRoleVipNum;
	this->m_nCurPlyaerLvel=pValue->m_nCurPlyaerLvel;
	this->m_nRoleVipLifeMaxVal  = pValue->m_nRoleVipLifeMaxVal;
	this->m_nRoleVipDamageMaxVal= pValue->m_nRoleVipDamageMaxVal;
	if (this->m_nRoleVipValue<=0)
	{
		this->m_nCurPlyaerLvel=0;
		this->m_nCurRoleVipNum=0;
		this->m_nRoleVipLevel =	0;
		this->m_nRoleVipKeepQpiont =	0;
		this->m_nRoleVipKeepJpiont =	0;
		this->m_nRoleVipSetSkillMaxLevel = 0;
	    this->m_nRoleVipSetFanYuMaxVal =	0;
		this->m_nRoleVipLifeMaxVal  = 0;
		this->m_nRoleVipDamageMaxVal =0;
	}

	 /*
	  BYTE    m_nRoleVipLevel;		       //当前转生的等级
	  BYTE    m_nRoleVipKeepQpiont;       //保留的潜能点
	  BYTE	m_nRoleVipKeepJpiont;       //保留的技能
	  BYTE    m_nRoleVipSetSkillMaxLevel; //技能等级上限增加
	  BYTE    m_nRoleVipSetFanYuMaxVal;   //防御上限增加

	  */
}

/*
int  KPlayerVip::SetRoleVipByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;
	
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerVip::GetRoleVipByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
*/

void	KPlayerVip::AddRoleVipValue(int nAdd)
{
	m_nRoleVipValue += nAdd;
	if (m_nRoleVipValue < 0)
		m_nRoleVipValue = 0;
}
