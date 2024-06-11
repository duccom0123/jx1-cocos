//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerReBorn.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerReBorn Class 转生
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerReBorn.h"
#include    "CoreShell.h"
void	KPlayerReBorn::SetReBornValue(BYTE* pMsg)
{

	REBORN_VALUE_SYNC	*pValue = (REBORN_VALUE_SYNC*)pMsg;

	if (m_nReBornValue == pValue->m_nReBornValue)
	{
		return;
	}

	this->m_nReBornValue = (pValue->m_nReBornValue < 0 ? 0 : pValue->m_nReBornValue);
  /*
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;
	*/
	this->m_nReBornLevel =	pValue->m_nReBornLevel;
	this->m_nReBornKeepQpiont =	pValue->m_nReBornKeepQpiont;
	this->m_nReBornKeepJpiont =	pValue->m_nReBornKeepJpiont;
	this->m_nReBornSetSkillMaxLevel = pValue->m_nReBornSetSkillMaxLevel;
	this->m_nReBornSetFanYuMaxVal =	pValue->m_nReBornSetFanYuMaxVal;
	this->m_nCurReBornNum=pValue->m_nCurReBornNum;
	this->m_nCurPlyaerLvel=pValue->m_nCurPlyaerLvel;
	this->m_nReBornLifeMaxVal  = pValue->m_nReBornLifeMaxVal;
	if (this->m_nReBornValue<=0)
	{
		this->m_nCurPlyaerLvel=0;
		this->m_nCurReBornNum=0;
		this->m_nReBornLevel =	0;
		this->m_nReBornKeepQpiont =	0;
		this->m_nReBornKeepJpiont =	0;
		this->m_nReBornSetSkillMaxLevel = 0;
	    this->m_nReBornSetFanYuMaxVal =	0;
		this->m_nReBornLifeMaxVal  = 0;
	}

	 /*
	  BYTE    m_nReBornLevel;		       //当前转生的等级
	  BYTE    m_nReBornKeepQpiont;       //保留的潜能点
	  BYTE	m_nReBornKeepJpiont;       //保留的技能
	  BYTE    m_nReBornSetSkillMaxLevel; //技能等级上限增加
	  BYTE    m_nReBornSetFanYuMaxVal;   //防御上限增加

	  */
}

/*
int  KPlayerReBorn::SetReBornByte(int nIntValue,int nByteNumber,int nByteValue)
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

int  KPlayerReBorn::GetReBornByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
*/

void	KPlayerReBorn::AddReBornValue(int nAdd)
{
	m_nReBornValue += nAdd;
	if (m_nReBornValue < 0)
		m_nReBornValue = 0;
	/*
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE; */
}
