//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerFuYuan.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerFuYuan Class
//---------------------------------------------------------------------------

#ifndef KPLAYERFUYUAN_H
#define KPLAYERFUYUAN_H

class KPlayerFuYuan
{
	friend class KPlayer;
private:
	int		m_nFuYuanValue;						// gia tri
public:
	void	SetFuYuanValue(int nValue);
	void	AddFuYuanValue(int nAdd);	
	int		GetFuYuanValue()	{return m_nFuYuanValue;}	

	
};
#endif
