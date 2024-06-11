#ifndef KLadderH
#define	KLadderH

#include "KProtocol.h"

#define	MAX_FAC FACTIONS_PRR_SERIES * series_num+1  //11个门派

enum
{
	enumLadderBegin = 0,
	enumTopTenMasterHand,  //等级
	enumTopTenRicher,      //金钱
	enumTopTenKiller,      //杀人
	enumTopTenXu,          //世界金币排行
	enumFacTopTenMasterHand,
	enumFacTopTenRicher = enumFacTopTenMasterHand + MAX_FAC, //16		  
	enumFacTopTenXu     = enumFacTopTenRicher + MAX_FAC,     //16+11 =27
	enumLadderEnd       = enumFacTopTenXu + MAX_FAC,         //16+11+11 =38
};

class KLadder
{
public:
	KLadder();
	~KLadder();
	const TRoleList*	TopTenRich();
	const TRoleList*    TopTenXu();
	const TRoleList*	TopTenMasterHand();
	const TRoleList*	TopTenKiller();
	const TRoleList*	TopTenFacRich(int nFac);
	const TRoleList*    TopTenFacXu(int nFac);
	const TRoleList*	TopTenFacMasterHand(int nFac);
	const TRoleList*	GetTopTen(DWORD	dwLadderID);
	int					GetFacMemberCount(int nFac);
	int					GetFacMasterHandPercent(int nFac);
	int					GetFacMoneyPercent(int nFac);
	BOOL				Init(void* pData, size_t uSize);
private:
	TGAME_STAT_DATA	GameStatData;
};

extern KLadder	Ladder;
#endif