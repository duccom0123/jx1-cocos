#ifndef	KItemListH
#define	KItemListH
#include "engine/KLinkArray.h"
#include "KItem.h"

#define	MAX_ITEM_ACTIVE	3

#define	REQUEST_EQUIP_ITEM		1
#define	REQUEST_EAT_MEDICINE	2

typedef struct
{
	int		nPlace;
	int		nX;     // 0 1 2 3 4
	int		nY;     // 0123456789
	int     nSIsRongqi;
	int     nDIsRongqi;
} ItemPos;

typedef struct
{
	int		nPlace;
	int		nX;     // 0 1 2 3 4
	int		nY;     // 0123456789
	int     nIsRongqi;
	int     nIdx;
} ItemXinXi;

class  KItemList
{
private:
	int			m_PlayerIdx;
	int			m_Hand;
	int			m_nBackHand;
	int			m_EquipItem[itempart_num];						// 玩家装备的道具（对应游戏世界中道具数组的索引）
	int			m_ItemBox[MAX_ITEM_BOX];                        // 装备单对象栏
	PlayerItem	m_Items[MAX_PLAYER_ITEM];						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	PlayerItem	m_sBackItems[MAX_PLAYER_ITEM];					// 交易过程中 m_Items 的备份
	static int	ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE];	// 某个部分的装备所激活的装备
	static int	ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE];	// 某个部分的装备被什么装备激活
	KLinkArray	m_FreeIdxItemList;
	KLinkArray	m_UseIdxItemList;
	int			m_nListCurIdx;									// 用于 GetFirstItem 和 GetNextItem
	int         m_ItemCount;                                    // 玩家的所有物品的数量
	BOOL		m_bActiveSet;                   //是否激活设置开关
	BOOL		m_nMaskLock;	                //mat na
	BOOL		m_bLockOperation;               //是否锁着客户端
public:
	KInventory	m_Room[room_num];
private:
	int			FindFree();
	int			FindSame(int nGameIdx);							// nGameIdx指游戏世界中道具数组的编号
	int			GetEquipPlace(int nType);						// 取得某类型装备应该放的位置
	BOOL		Fit(int nIdx, int nPlace);						// 检查是否可以装备上
	BOOL		Fit(KItem* pItem, int nPlace);
	int			GetEquipEnhance(int nPlace);					// 取得装备的激活属性数量
	int			GetActiveEquipPlace(int nPlace, int nCount);	// 取得nPlace的装备激活的第nCount个装备位置
	void		InfectionNextEquip(int nPlace, BOOL bEquip = FALSE);
	// 在room_equipment中查找指定Genre和DetailType的物品，得到ItemIdx和位置
	BOOL		FindSameDetailTypeInEquipment(int nGenre, int nDetail, int *pnIdx, int *pnX, int *pnY);
	friend	class KPlayer;

public:
	KItemList();
	~KItemList();
	int			Init(int nIdx);
	int			GetEquipment(int nIdx) { return m_EquipItem[nIdx]; }
	int         GetEquPlace(int nIdx) { return GetEquipPlace(nIdx); }
	int         _getEquipPlaceByItemIdx(int nidx);
	int			GetItemBox(int nIdx) { return m_ItemBox[nIdx]; }
	void        GetItemsPos(int nIdx,ItemPos* nPos){nPos->nPlace=m_Items[nIdx].nPlace;nPos->nX=m_Items[nIdx].nX;nPos->nY=m_Items[nIdx].nY;};
	int			GetActiveAttribNum(int nIdx);					// nIdx指游戏世界中道具数组的编号
	int			GetWeaponType();								// 取得武器的类型，近程武器(equip_meleeweapon)还是远程武器(equip_rangeweapon)
	int			GetWeaponParticular();							// 取得武器的具体类型，用于计算不同的伤害加强
	void		GetWeaponDamage(int* nMin, int* nMax);			// 取得武器的伤害
	int			Add(int nIdx, int nPlace, int nX, int nY, BOOL bInit = FALSE,int nIsAutoDie=0,int StackNum=1,int nIsLoign=0);		// nIdx指游戏世界中道具数组的编号
	BOOL		Remove(int nIdx,int ndX=0,int ndY=0,int nModel=0,BOOL nIsSave=TRUE);								// nIdx指游戏世界中道具数组的编号
	void		RemoveAll();
	BOOL		CanEquip(int nIdx, int nPlace = -1);			// nIdx指游戏世界中道具数组的编号
	BOOL        CanTonEquip(int nIdx, int nPlace=-1);
	BOOL		CanEquip(KItem* pItem, int nPlace = -1);
	BOOL		EnoughAttrib(void* pData,int nPlace=-1, DWORD nDwIdx=0);
	BOOL		Equip(int nIdx, int nPlace = -1,int nIsLoign=0);				// nIdx指游戏世界中道具数组的编号
	BOOL		TonEquip(int nIdx, int nPlace = -1,int nIsLoign=0);
	BOOL		UnEquip(int nIdx, int nPos = -1);				// nIdx指游戏世界中道具数组的编号
	BOOL        UnTonEquip(int nIdx, int nPos=-1);              //移除同伴的装备
	BOOL        RemoveItemBox(int nIdx , int nPos=-1);
	BOOL        TimeUnEquip(int nIdx,int nPos = -1);            // 移除时间到的某装备属性
	BOOL        tempUnEquip(int nIdx,int nPos=-1);
	void        ReSkillEnhance(int m_PlayerIdx,int nNpcIdx);
	BOOL		EatMecidineS(int nIdx,int nx=0,int nY=0);							// nIdx指游戏世界中道具数组的编号
	PlayerItem*	GetFirstItem();
	PlayerItem*	GetNextItem();
	int			SearchID(int nID);
	int	        SearchItemsIdx(int nDwid);
	void		ExchangeMoney(int nSrcRoom, int DesRoom, int nMoney);
	void		ExchangeItem(ItemPos* SrcPos,ItemPos* DesPos,int nIsMianBam=0);
	int         QiangHua(int nLindexID=0,int nBaoshiID=0,int nRank=0);//强化装备
	int         RongLian(int nLindexID=0,int nBaoshiID=0);//装备熔炼
	int         WenGang(int nLindexID=0,int nBaoshiID=0); //黄金装备纹钢
	int         BlueWenGang(int nLindexID,int nBaoshiID); //蓝装纹钢
	int         PurpleWenGang(int nLindexID,int nBaoshiID); //紫装纹钢

	int         BlueBianShi(int nLindexID,int nBaoshiID); //蓝装辨识
	int         BlueRonHe(int nIdxa,int nIdxb,int nEqType,int nRank,int nCurLuck,int nsLevel);   //蓝装融合
	int         LingHe(int nLindexID,int nBaoshiID);      //宝石灵合
	int         WenGangGenerator(int nRItemLevelRows);    //纹钢产生器
	int         TiLian(int nLindexID=0,int nBaoshiID=0,int nModel=0,int nWenGangPos=0);
	int         BlueTiLian(int nLindexID=0,int nBaoshiID=0,int nModel=0,int nWenGangPos=0);
	int         CilentAddItem(int nItemIdx=0);      // 产生材料
    BOOL        CheckItemType(int ItemClass,int Detail,int nPart,int IsMianBan);
	int			GetMoneyAmount();					// 得到物品栏和储物箱的总钱数
	int			GetEquipmentMoney();				// 得到物品栏和储物箱的钱数
	BOOL		AddmServer(int nRoom, int nMi);
	BOOL		CostMoney(int nMoney);
	BOOL		DecMoney(int nMoney);
	void		SetClient(int nMon1, int nMon2, int nMon3);
	void		SetRoomMoney(int nRoom, int nMoney);
	int			GetsMon(int nRoom) { return m_Room[nRoom].GetmServer(); }	// 取得空间的钱

	int			GetIndexItem(int nRoom,int nIndex) { return m_Room[nRoom].GetCurIndexItem(nIndex); }
	//Xu
	int			GetxLient(int nRoom) { return m_Room[nRoom].GetxCLient(); }
	void		SetServer(int nXu);
	void		SetRoomXu(int nRoom, int nXu);
	BOOL		AddxCliet(int nRoom, int ni);
	BOOL		CostXu(int nXu);
	//BOOL		DecXu(int nXu);
	int			GetXuAmount();					
	int			GetEquipmentXu();
	/////////////////////////////////               // ------------
	void		SetPlayerIdx(int nIdx);				// 设定玩家索引
	int			Hand() { return m_Hand; };
	void		ClearRoom(int nRoom);
	void		BackupTrade();
	void		RecoverTrade();
	void		StartTrade();
	int			GetItemNum(int nGenre, int nDetailType, int nParticular, int nLevel = 1,ITEM_POSITION nRongType=pos_equiproom); // dem so item cung loai
	int         GetItemCount();
	int GetPlayerItemCount()
	{
		/*int nIdx = 0;
		int nCount=0;
		while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
		{
			nCount++;
		}
	    return nCount;*/	
		return m_ItemCount;
	};
	void		RemoveAllInOneRoom(int nRoom);//删除容器所有东西物品
	void		BackupGive();
	void		RecoverGive();
	void		StartGive();

	BOOL		GetIfActive();
	BOOL		GetMaskLock() {return m_nMaskLock;};	// mat na
	void		SetMaskLock(BOOL bFlag);// mat na

    BOOL		CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnX, int *pnY,int nRoomType=0);// 判断一定长宽的物品能否放进物品栏
	BOOL		SearchPosition(int nWidth, int nHeight, ItemPos* pPos);
	int         GetFreePositionCount(int nWidth, int nHeight,int RoomKind);
	BOOL        SearchItemPosition(int nWidth, int nHeight, ItemPos* pPos,int roomType);
	BOOL        SearchItemPositionKienTM(int nWidth, int nHeight, int posX,int posY,int roomType);

    void		AutoEquip(ItemPos SrcPos,ItemPos DesPos,DWORD nSrcItemDwIdx,int nEuqPlace,int nKind=0);
    int         AutoChangeItem(int nIdx); //自动交换容器
	int			UseItem(int nIdx,int cnx=0,int cny=0);					//nIdx指游戏世界中道具数组的编号
	//BOOL		SearchPosition(int nWidth, int nHeight, ItemPos* pPos);
	//BOOL        SearchItemPosition(int nWidth, int nHeight, ItemPos* pPos,int roomType);
	BOOL		AutoMoveItem(ItemPos SrcPos,ItemPos DesPos,unsigned int nItemdwID=0);
	void		MenuSetMouseItem();
//	void		RemoveAllInOneRoom(int nRoom);
	void		LockOperation();											// 锁定客户端对装备的操作
	void		UnlockOperation();
	BOOL		IsLockOperation() { return m_bLockOperation; };
	int			GetSameDetailItemNum(int nImmediatePos);
	int	        GetImmediacyItemIndex(int nImmediatePos);
    int	        GetSameEquipmentItemNum(int nIdx);
	int			GetGoldColor(int nSet,int nId);
	void	    ClientShowMsg(const char *Msg);
	void        CheckBianShiItemTime();
};
#endif
