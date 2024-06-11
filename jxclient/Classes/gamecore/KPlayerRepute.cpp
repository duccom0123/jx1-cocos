//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerRepute.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------
#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerRepute.h"
#include "CoreShell.h"
void	KPlayerRepute::SetReputeValue(int nValue)
{
	if (m_nReputeValue == nValue)
		return;

	this->m_nReputeValue = (nValue < 0 ? 0 : nValue);
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;

}

void	KPlayerRepute::AddReputeValue(int nAdd)
{
	m_nReputeValue += nAdd;
	if (m_nReputeValue < 0)
		m_nReputeValue = 0;
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;
}












