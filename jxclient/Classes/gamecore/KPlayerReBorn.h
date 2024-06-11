//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerReBorn.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------

#ifndef KPLAYERREBORN_H
#define KPLAYERREBORN_H

//客户端
class KPlayerReBorn
{
	friend class KPlayer;
private:
	int	   m_nReBornValue;						//转生值  （记录转生次数,以及转生的等级,保留的技能点,潜能点）
	int    m_nReBornLevel;		       //当前转生的等级
	int    m_nReBornKeepQpiont;       //保留的潜能点
	int	   m_nReBornKeepJpiont;       //保留的技能
	int    m_nReBornSetSkillMaxLevel; //技能等级上限增加
	int    m_nReBornSetFanYuMaxVal;   //防御上限增加
	int    m_nReBornLifeMaxVal;       //生命上限增加
	int    m_nCurPlyaerLvel;
	int    m_nCurReBornNum;



public:
	void	SetReBornValue(BYTE* pMsg);	  //int nValue
	void	AddReBornValue(int nAdd);	
	int		GetReBornValue()	{return m_nReBornValue;};
	int		GetReBornLevel()	{return m_nReBornLevel;};
	int		GetReBornKeepQpiont()	{return m_nReBornKeepQpiont;};
	int		GetReBornKeepJpiont()	{return m_nReBornKeepJpiont;};
	int		GetReBornSkillMaxLevel()	{return m_nReBornSetSkillMaxLevel;};
	int		GetReBornFanYuMaxVal()	{return m_nReBornSetFanYuMaxVal;};
	int		GetReBornLifeMaxVal()	{return m_nReBornLifeMaxVal;};
	int		GetReBornCurLevel()	{return m_nCurPlyaerLvel;};
	int		GetReBornNum()	{return m_nCurReBornNum;};
	//int     SetReBornByte(int nIntValue,int nByteNumber,int nByteValue);
	//int     GetReBornByte(int nIntValue,int nByteNumber);

};
#endif
