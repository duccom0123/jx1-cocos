#ifndef	KNpcSetH
#define	KNpcSetH
#include "engine/KbugInfo.h"
#include "engine/KLinkArray.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KNpcGold.h"
#include <map>

#define		MAX_NPC_REQUEST	200   //最大的NPC 请求数量

class KNpcTemplate;

#define		MAX_INSTANT_STATE	20
#define		MAX_INSTANT_SOUND	30
class	KInstantSpecial
{
private:
	int		m_nLoadFlag;
	char	m_szSprName[MAX_INSTANT_STATE][FILE_NAME_LENGTH];
	char	m_szSoundName[MAX_INSTANT_SOUND][FILE_NAME_LENGTH];

	KCacheNode	*m_pSoundNode;	// 声效指针
//	KWavSound	*m_pWave;		// 声效wav指针

private:
	void	LoadSprName();
	void	LoadSoundName();

public:
	KInstantSpecial();
	void	GetSprName(int nNo, char *lpszName, int nLength);
	void	PlaySound(int nNo);
};

typedef struct
{
	DWORD	dwRequestId;
	DWORD	dwRequestTime;
} RequestNpc;

typedef struct
{
	int		nStandFrame[2];
	int		nWalkFrame[2];
	int		nRunFrame[2];
	int		nWalkSpeed;
	int		nRunSpeed;
	int		nAttackFrame;
	int		nHurtFrame;
} PlayerBaseValue;

class KNpcSet
{
public:
	KNpcGoldTemplate	m_cGoldTemplate;					// 黄金怪物模板
//	KNpcPartnerTemplate	m_cPartnerTemplate;					// 同伴模板
	PlayerBaseValue		m_cPlayerBaseValue;					// 玩家标准数据

	KInstantSpecial		m_cInstantSpecial;

private:
	DWORD				m_dwIDCreator;						//	游戏世界中的DWID号
	KLinkArray			m_FreeIdxNpcSet;					//	可用表
	KLinkArray			m_UseIdxNpcSet;						//	已用表
	//int                 m_nListCurIdx;
	typedef std::map<DWORD, KNpcTemplate*> _KMapTemplate;
	//INT				m_anTemplateRowId[MAX_NPCSTYLE + 1];
	_KMapTemplate		m_mapTemplate;
	//KTabFile			m_cTabFile;
	enum
	{
		PATE_CHAT = 0x01,
		PATE_NAME = 0x02,
		PATE_LIFE = 0x04,
		PATE_MANA = 0x08,
	};
	int					m_nShowPateFlag;					// 是否全部显示玩家的名字在头顶上 zroc add
	RequestNpc			m_RequestNpc[MAX_NPC_REQUEST];		//	向服务器申请的ID表
	KLinkArray			m_RequestFreeIdx;					//  向服务器申请空闲表
	KLinkArray			m_RequestUseIdx;					//	向服务器申请可用表
public:
	KNpcSet();
	void			Init();
	virtual         BOOL Release(void);
	int				GetNpcCount(int nKind = -1, int nCamp = -1);
	int				SearchName(LPSTR szName);
	int             SearchNameInMap(LPSTR szName,int inMapidx,int nType=0);
	int             SearchGSNameInMap(LPSTR szName,int inMapidx);
	int				SearchNpcSettingID(LPSTR szName);
	int				SearchByTongName(LPSTR szName);  //区域性用的
	int				SearchID(DWORD dwID);            //区域性用的
	int             DelNpcInNpcSet(int nNpcSettings);
	int             SearchGSName(LPSTR szName);
	int				SearchNameID(DWORD dwID);        //查找玩家的
	BOOL			IsNpcExist(int nIdx, DWORD dwId);
	int				AddServerNpcB(int nNpcSetingIdxInfo, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX = 0, int nOffY = 0,int nBoss=0);
	int				AddServerNpcA(int nNpcSetingIdxInfo, int nSubWorld, int nMpsX, int nMpsY,int nBoss=0,int nRodom=0,int nOffwminx=1,int nOffwmaxx=1,int nOffhminy=1,int nOffhmaxy=1,int nNpcKind=-1,unsigned int nNpcDwidx=0);
	int				AddMapNpc(int nSubWorld, void* pNpcInfo);
    int				Addyabiao(int nNpcSetingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, int nSeries, IN int nComp,IN int nRindexid,int nBoss=0);
    int	            AddTongBan(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, IN int nSeries, IN int nComp,IN int nRindexid,int nBoss=0);
	void			Remove(int nIdx,BOOL isNeedSyn = TRUE);
	void			RemoveAll();
	NPC_RELATION	GetRelation(int nIdx1, int nIdx2);
	int             GetNPCBaiTan(int nIdex);
	int				GetNearestNpc(int nMapX, int nMapY, int nId, int nRelation);
	static int		GetDistance(int nIdx1, int nIdx2);
	static int		GetDistanceSquare(int nIdx1, int nIdx2);
	static INT	    GetMapDisX(INT nIdx1, INT nIdx2);
	static INT	    GetMapDisY(INT nIdx1, INT nIdx2);

	int				GetNextIdx(int nIdx);
	// 把所有npc的 bActivateFlag 设为 FALSE (每次游戏循环处理所有npc的activate之前做这个处理)
	void			ClearActivateFlagOfAllNpc();
	void			LoadPlayerBaseValue(LPSTR szFile);
	int				GetPlayerWalkSpeed() { return m_cPlayerBaseValue.nWalkSpeed; };
	int				GetPlayerRunSpeed() { return m_cPlayerBaseValue.nRunSpeed; };
	int				GetPlayerAttackFrame() { return m_cPlayerBaseValue.nAttackFrame; };
	int				GetPlayerHurtFrame() { return m_cPlayerBaseValue.nHurtFrame; };
	int             CheckForBit(int *nVal,char* mVal,int Model=1);
	BOOL            CheckThisNpc(int nSetings,char *nKey);

	KNpcTemplate*	GetTemplate(INT nNpcTemplateId,INT nLevel);

	int				GetPlayerStandFrame(BOOL bMale) 
	{ 
		if (bMale)
			return m_cPlayerBaseValue.nStandFrame[0];
		else
			return m_cPlayerBaseValue.nStandFrame[1];
	};
	int				GetPlayerWalkFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nWalkFrame[0];
		else
			return m_cPlayerBaseValue.nWalkFrame[1];
	};
	int				GetPlayerRunFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nRunFrame[0];
		else
			return m_cPlayerBaseValue.nRunFrame[1];
	};
	BOOL			IsNpcRequestExist(DWORD	dwID);
	void			InsertNpcRequest(DWORD dwID);
	void			RemoveNpcRequest(DWORD dwID);
	int				GetRequestIndex(DWORD dwID);
	// 添加一个客户端npc（需要设定ClientNpcID）
	int				AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo);
	// 从npc数组中寻找属于某个region的 client npc ，添加进去
	void			InsertNpcToRegion(int nRegionIdx);
	// 查找某个ClientID的npc是否存在
	int				SearchClientID(KClientNpcID sClientID);
	// 某座标上精确查找Npc，客户端专用
	int				SearchNpcAt(int nX, int nY, int nRelation, int nRange);
	void			CheckBalance();
	int				GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount);	// 获得周围玩家列表(用于队伍邀请列表)
	void			GetAroundOpenCaptain(int nCamp);		// 获得周围同阵营的已开放队伍队长列表
	int				GetAroundPlayer(KUiPlayerItem *pList, int nCount);	// 获得周围玩家列表(用于列表)

	// 设定是否全部显示玩家的名字  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowNameFlag(BOOL bFlag);
	// 判断是否全部显示玩家的名字  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowName();
	// 设定是否全部显示玩家的聊天  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowChatFlag(BOOL bFlag);
	// 判断是否全部显示玩家的聊天  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowChat();
	// 设定是否全部显示玩家的血  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowLifeFlag(BOOL bFlag);
	// 判断是否全部显示玩家的血  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowLife();
	// 设定是否全部显示玩家的内力  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowManaFlag(BOOL bFlag);
	// 判断是否全部显示玩家的内力  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowMana();
private:
	void			SetID(int m_nIndex);
	int				FindFreeNpcSet();


    // Add By Freeway Chen in 2003.7.14
private:
    // [第一个NPC.m_Kind][第二个NPC.m_Kind][第一个NPC.m_CurrentCamp][第二个个NPC.m_CurrentCamp]
    //unsigned char m_RelationTable[kind_num][kind_num][camp_num][camp_num];
    // 生成关系表
    //int GenRelationTable();
    NPC_RELATION GenOneRelation(NPCKIND Kind1, NPCKIND Kind2, NPCCAMP Camp1, NPCCAMP Camp2);

};

// modify by Freeway Chen in 2003.7.14
// 确定两个NPC之间的战斗关系
extern KNpcSet NpcSet;
#endif
