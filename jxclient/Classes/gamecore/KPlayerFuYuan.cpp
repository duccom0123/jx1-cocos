//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerFuYuan.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerFuYuan Class
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerFuYuan.h"
#include    "CoreShell.h"

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void	KPlayerFuYuan::SetFuYuanValue(int nValue)
{
	if (m_nFuYuanValue == nValue)
		return;

	this->m_nFuYuanValue = (nValue < 0 ? 0 : nValue);
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;

}

void	KPlayerFuYuan::AddFuYuanValue(int nAdd)
{
	m_nFuYuanValue += nAdd;
	if (m_nFuYuanValue < 0)
		m_nFuYuanValue = 0;
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;
}













