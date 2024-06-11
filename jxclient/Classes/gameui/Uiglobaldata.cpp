//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Data Compress / Decompress Base Class
//---------------------------------------------------------------------------
#include "Uiglobaldata.h"
//#include "engine/KEngine.h"
#include "gamescene/KScenePlaceC.h"
KRoleChiefInfo	m_RoleList[MAX_PLAYER_PER_ACCOUNT];
int   nGlobaluistep = UI_STEP_NONE;
short m_nNumRole  = 0;
m_Choices m_ClientChoices;
int  nPlaceId=-1;
int  nNativePlaceId=-1;
RECT maxMapRc;
bool isthisVer=false;
int  nCuraMapIdx=-1;