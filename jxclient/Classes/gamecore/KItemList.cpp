#include	"KCore.h"
//#include	"MyAssert.H"
//#include  "KProtocol.h"
//#include  <map>
#include	"KItem.h"
#include	"KItemSet.h"
#include	"KNpc.h"
#include	"KMath.h"
#include	"KPlayer.h"
#include	"KItemList.h"
#include    "KItemGenerator.h"
#include	"KItemChangeRes.h"
#include	<time.h>
#include    "engine/Text.h"
#include	"CoreShell.h"
//#include	"../../Headers/IClient.h"

extern KTabFile g_EventItemTab;
extern KTabFile g_MineItemTab;
extern KTabFile g_FusionTab;

// PK死亡惩罚掉穿的装备，各装备掉落的权值
#define	defEQUIP_POWER

#ifdef  defEQUIP_POWER
	int	g_nEquipPower[itempart_num] =
	{2, 4, 2, 4, 1, 1, 1, 1, 1, 1, 0,0,1,2,1};

#endif
//激活的装备清单
int KItemList::ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE] = 
{
    { itempart_foot, itempart_ring1,itempart_weapon},	  //itempart_head,    头盔 激活 靴子,上戒,武器
	{ itempart_weapon, itempart_head,itempart_yinjian},	  //itempart_body,    衣服 激活 武器,头盔,印鉴
	{ itempart_body, itempart_amulet,itempart_ring1},	  //itempart_belt,    腰带 激活 衣服,项链,上戒
	{ itempart_ring1, itempart_foot,itempart_cuff},	      //itempart_weapon,  武器 激活 上戒,靴子,护腕
	{ itempart_cuff, itempart_pendant,itempart_body},     //itempart_foot,    靴子 激活 护腕,玉佩,衣服
	{ itempart_belt, itempart_ring2,itempart_head},	      //itempart_cuff,    护腕 激活 腰带,下戒,头盔
	{ itempart_head, itempart_weapon,itempart_ring2},	  //itempart_amulet,  项链 激活 头盔,武器,下戒
	{ itempart_pendant,itempart_cuff,itempart_amulet},   //itempart_ring1,    上戒 激活 玉佩,护腕,项链
	{ itempart_amulet, itempart_body,itempart_belt},      //itempart_ring2,   下戒 激活 项链,衣服,腰带
	{ itempart_ring2,itempart_belt,itempart_pifeng},	  //itempart_pendant, 玉佩 激活 下戒,腰带,披风
	{ itempart_pifeng, itempart_shiping ,itempart_foot}, //itempart_horse,    马匹 激活 披风,饰品,靴子
	{ itempart_horse, itempart_pifeng ,itempart_pendant}, //itempart_mask	  面具 激活 马匹,披风,玉佩 
	{ itempart_shiping, itempart_yinjian,itempart_mask},  //                  披风 激活 饰品,印鉴,面具
	{ itempart_mask, itempart_horse,itempart_shiping},	  //	              印鉴 激活 面具,马匹,饰品
	{ itempart_yinjian, itempart_mask,itempart_horse},	  //	              饰品 激活 印鉴,面具,马匹
	                                                                          //以上均是相生激活
};
//被激活的装备清单
int KItemList::ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE] =
{
	{ itempart_body, itempart_amulet,itempart_cuff},   //	itempart_head,  头盔 需要 衣服,项链,护腕  激活
	{ itempart_ring2, itempart_belt,itempart_foot},	   //	itempart_body,  衣服 需要 下戒,腰带,靴子  激活	    
	{ itempart_pendant, itempart_cuff,itempart_ring2}, //	itempart_belt,  腰带 需要 玉佩,护腕,下戒  激活
	{ itempart_amulet, itempart_body,itempart_head},   //	itempart_weapon,武器 需要 项链,衣服,头盔  激活
	{ itempart_weapon, itempart_head,itempart_horse},//	itempart_foot,      靴子 需要 武器,头盔,马匹  激活
	{ itempart_foot, itempart_ring1,itempart_weapon},  //	itempart_cuff,  护腕 需要 靴子,上戒,武器  激活
	{ itempart_belt, itempart_ring2,itempart_ring1},   //	itempart_amulet,项链 需要 腰带,下戒,上戒  激活
	{ itempart_weapon, itempart_head,itempart_belt},   //	itempart_ring1, 上戒 需要 武器,头盔,腰带  激活
	{ itempart_cuff, itempart_pendant,itempart_amulet},//	itempart_ring2, 下戒 需要 护腕,玉佩,项链  激活
	{ itempart_foot, itempart_ring1,itempart_mask},	 //	itempart_pendant,   玉佩 需要 靴子,上戒,面具  激活
	{ itempart_mask, itempart_yinjian,itempart_shiping},//	                马匹 需要 面具,印鉴,饰品  激活
	{ itempart_yinjian, itempart_shiping,itempart_pifeng},             //   面具 需要 印鉴,饰品,披风  激活
	{ itempart_horse, itempart_mask,itempart_pendant},	               //   披风 需要 马匹,面具,玉佩  激活
	{ itempart_pifeng, itempart_shiping,itempart_body},	               //   印鉴 需要 披风,饰品,衣服  激活
	{ itempart_horse, itempart_pifeng,itempart_yinjian},	           //   饰品 需要 马匹,披风,印鉴  激活
};
                                                                       //   以上均是相生激活
KItemList::KItemList()
{
	m_PlayerIdx = 0;
	m_nListCurIdx = 0;
	m_ItemCount  = 0;
}

KItemList::~KItemList()
{

}


/*!*****************************************************************************
// Function		: KItemList::GetWeaponType
// Purpose		: 取得玩家装备的武器类型
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::GetWeaponType()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetDetailType();
	else
		return -1;
}

void KItemList::GetWeaponDamage(int* nMin, int* nMax)  //武器伤害
{
	int nWeaponIdx = m_EquipItem[itempart_weapon];
	if (nWeaponIdx)
	{//assert
		assert(Item[nWeaponIdx].m_CommonAttrib.nItemGenre == item_equip
			&& (Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_meleeweapon
			|| Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_rangeweapon));
		int nMinDamage, nMaxDamage, nEnhance;
		int nDamageMinBase = Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0]; //武器的基本伤害
		int	nDamageMaxBase = Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
		nMinDamage = 0;
		nMaxDamage = 0;
		nEnhance = 0;
		for (int i = 0; i < 6; ++i)
		{
			switch(Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
			{
			case magic_weapondamagemin_v:
				nMinDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamagemax_v:
				nMaxDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamageenhance_p:
				nEnhance += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			default:
				break;
			}
		}
		*nMin = (nDamageMinBase + nMinDamage) * (100 + nEnhance) / 100;
		*nMax = (nDamageMaxBase + nMaxDamage) * (100 + nEnhance) / 100;
	}
	else
	{// 空手
		/*
		int nDamageBase = Player[m_PlayerIdx].m_nCurStrength * Player[m_PlayerIdx].m_nCurDexterity;
		*nMin = nDamageBase >> 9;
		*nMax = nDamageBase >> 8;
		*/
		// 数值计算方法修改（孙英要求） by Spe 03/06/11
		//_ASSERT(STRENGTH_SET_DAMAGE_VALUE > 0);
		*nMin = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
		*nMax = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
	}
}
/*!*****************************************************************************
// Function		: KItemList::Add
// Purpose		: 玩家得到一个装备
// Return		: int 
// Argumant		: int nIdx		Item数组的idx
// Argumant		: int nPlace
// Argumant		: int nX
// Argumant		: int nY
// Comments		:
// Author		: Spe   玩家增加装备调用的函数！同步装备
*****************************************************************************/
int KItemList::Add(int nIdx, int nPlace, int nX, int nY, BOOL bInit,int nIsAutoDie,int StackNum,int nIsLoign) //目标容器增加物品
{
	if (nIdx <= 0)
	{
		return 0;
	}

	if (Item[nIdx].GetID() == 0)  //获取dwid
	{
		return 0;
	}
    //产生物品 查找未使用表 是否有位置
	int i = FindFree();

	if (i<=0)
	{
		return 0;
	}

	switch(nPlace) //目标容器
	{
	case pos_hand:
		if (m_Hand)
		{
			return 0;
		}
		m_Items[i].nPlace = pos_hand;
		m_Items[i].nX = 0;
		m_Items[i].nY = 0;
		m_Hand = nIdx;
		break;
	case pos_equip:
		if (nX < 0 || nX >= itempart_num)
			return 0;
		if (m_EquipItem[nX])
			return 0;
//	有可能是条件不够，被其他装备凑上的。是否能装备应该放到Equip时检查
//		if (!CanEquip(nIdx, nX))
//			return 0;
		m_Items[i].nPlace = pos_equip;
		m_Items[i].nX = nX;       //设置装备的 部位
		m_Items[i].nY = 0;
		break;
	case pos_equiproom:  //包袱 装备空间  
		if (!m_Room[room_equipment].PlaceItem(nX,nY,nIdx,Item[nIdx].GetWidth(),Item[nIdx].GetHeight()))
		{
//客户端
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:包袱空间不足C!");
	   	return 0;
		} 
		m_Items[i].nPlace = pos_equiproom;  //设置容器物品信息
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			return 0;
		if (!m_Room[room_trade1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_trade1;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_repositoryroom:  //储物箱
		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		} 
		m_Items[i].nPlace = pos_repositoryroom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox1room: // 扩展箱 1
		if (!m_Room[room_exbox1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_exbox1room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox2room: // 扩展箱 2
		if (!m_Room[room_exbox2].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_exbox2room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox3room: // 扩展箱 3
		if (!m_Room[room_exbox3].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_exbox3room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_equiproomex:  // 字母袋
		if (!m_Room[room_equipmentex].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
//客户端
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:子母袋空间不足C!");
	   	return 0;
		}
		m_Items[i].nPlace = pos_equiproomex;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_immediacy:   //快捷栏
		if (!m_Room[room_immediacy].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_immediacy;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;		
	case pos_give:        //给予界面
		if (!m_Room[room_give].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_give;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_dazao:        //打造界面
		m_ItemBox[nX]=nIdx;
		m_Items[i].nPlace = pos_dazao;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_cailiao:        //打造界面
		if (!m_Room[room_cailiao].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
		{
			return 0;
		}
		m_Items[i].nPlace = pos_cailiao;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	default:
		return 0;
	}

	m_Items[i].nIdx = nIdx;
	m_Items[i].nPrice = 0;
	m_Items[i].nLianJieFlg =0;
	m_Items[i].nModel =0;

	m_FreeIdxItemList.Remove(i); // 移除未用表
	m_UseIdxItemList.Insert(i);  // 添加可用表
	m_ItemCount ++;

	if (m_Items[i].nPlace == pos_equip)
	{//装备这个装备
		Equip(m_Items[i].nIdx,nX,nIsLoign);
	}

	if (m_Items[i].nPlace == pos_dazao)
	{//装备这个装备
		if (nX>=53 && nX<=77)
		   TonEquip(m_Items[i].nIdx,nX,nIsLoign);
	}

	KUiObjAtContRegion	pInfo;
	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	// mat na
		UIEP_PIFENG,	//披风
		UIEP_YINJIAN,		//印鉴
	    UIEP_SHIPING,	//饰品
	};

	pInfo.Obj.uGenre    = CGOG_ITEM;	//源装备
	pInfo.Obj.uId       = nIdx;
	pInfo.Region.Width  = Item[nIdx].GetWidth();
	pInfo.Region.Height = Item[nIdx].GetHeight();

	switch(nPlace) //客户端的目标容器
	{
	case pos_immediacy:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_hand:
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_equiproom: //客户端包袱
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: //
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // 子母袋
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_trade1:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		break;
	case pos_dazao:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_DAZAO;
		break;
	case pos_cailiao:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_CAILIAO;
	case pos_give:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_GIVE;
		break;
	}
	if (nPlace != pos_trade1) //通知客户端物品增加
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);
	else
	{// 交易对方增加
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 1);
	}
	return i;
}

/*!*****************************************************************************
// Function		: KItemList::Remove
// Purpose		: 玩家失去一个装备
// Return		: int 
// Argumant		: int nGameIdx为游戏世界中道具数组的编号
// Comments		: 删除容器中的物品。。。。
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Remove(int nGameIdx,int ndX,int ndY,int nModel,BOOL nIsSave)
{
	if (!nGameIdx)
		return FALSE;

	int nFIdx = FindSame(nGameIdx);//查找已用表

	if (!nFIdx)
		return FALSE;
	switch(m_Items[nFIdx].nPlace)
	{
	case pos_hand:
		m_Hand = 0;
		break;
	case pos_equip:
		UnEquip(m_Items[nFIdx].nIdx);
		break;
	case pos_immediacy://清空这个位置
		m_Room[room_immediacy].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_equiproom:
		{
		m_Room[room_equipment].PickUpItem(
				nGameIdx,
	    m_Items[nFIdx].nX,
		m_Items[nFIdx].nY,
				Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		}
		break;
	case pos_repositoryroom:
		m_Room[room_repository].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox1room: // ruong mo rong 1
		m_Room[room_exbox1].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox2room: // ruong mo rong 2
		m_Room[room_exbox2].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_exbox3room: // ruong mo rong 3
		m_Room[room_exbox3].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_equiproomex: // mo rong hanh trang
		m_Room[room_equipmentex].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_traderoom:
		m_Room[room_trade].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_give:
		m_Room[room_give].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_dazao:
		/*m_Room[room_dazao].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());*/
		   if  (m_Items[nFIdx].nX>=53 && m_Items[nFIdx].nX<=77)
			   UnTonEquip(m_Items[nFIdx].nIdx);
			   
			RemoveItemBox(m_Items[nFIdx].nIdx);
			//m_ItemBox[ndX]=0;
		break;
	case pos_cailiao:
		m_Room[room_cailiao].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	case pos_trade1:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			//_ASSERT(0);
			return false;
		}
		m_Room[room_trade1].PickUpItem(
			nGameIdx,
			m_Items[nFIdx].nX,
			m_Items[nFIdx].nY,
			Item[m_Items[nFIdx].nIdx].GetWidth(),
			Item[m_Items[nFIdx].nIdx].GetHeight());
		break;
	default:
		return FALSE;
	}
	 // 客户端从玩家身上去除装备就应该从装备表中去除掉。
	ItemSet.Remove(m_Items[nFIdx].nIdx);    //节点处理
	//界面处理
	KUiObjAtContRegion pInfo;
      memset(&pInfo, 0, sizeof(KUiObjAtContRegion));

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	    // mat na
		UIEP_PIFENG,	//披风
		UIEP_YINJIAN,	//印鉴
	    UIEP_SHIPING,	//饰品
	};

	pInfo.Obj.uGenre = CGOG_ITEM;
	pInfo.Obj.uId       =m_Items[nFIdx].nIdx;
	pInfo.Region.Width  =Item[m_Items[nFIdx].nIdx].GetWidth();
	pInfo.Region.Height =Item[m_Items[nFIdx].nIdx].GetHeight();
//以下结构发向界面更新物品
	switch(m_Items[nFIdx].nPlace)
	{
	case pos_hand:
		pInfo.Obj.uGenre = CGOG_NOTHING;
		pInfo.Region.h = 0;
		pInfo.Region.v = 0;
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equiproom:
		{
          if (nModel==1)
		  {   
		    pInfo.Region.h =ndX;
		    pInfo.Region.v =ndY;
		  }   
		  else
		  { 
			pInfo.Region.h = m_Items[nFIdx].nX;
	 	    pInfo.Region.v = m_Items[nFIdx].nY;
		  } 
		  pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		}
		break;
	case pos_repositoryroom:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: // ruong mo rong 1
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: // ruong mo rong 2
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: // ruong mo rong 3
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // mo rong hanh trang
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_immediacy:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	/*case pos_give:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.eContainer = UOC_ITEM_GIVE;
		break;*/
	case pos_dazao:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		pInfo.Region.IsRongqi=0;
		pInfo.eContainer = UOC_ITEM_DAZAO;
		break;
	case pos_cailiao:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
        pInfo.Region.IsRongqi=1;
		pInfo.eContainer = UOC_ITEM_CAILIAO;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[m_Items[nFIdx].nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_trade1:
		pInfo.Region.h = m_Items[nFIdx].nX;
		pInfo.Region.v = m_Items[nFIdx].nY;
		break;
	}
	if (m_Items[nFIdx].nPlace != pos_trade1)
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);  //通知客户端改变更新
	else
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 0);

   if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId)
   {
	  KSystemMessage sMsg;
	  sMsg.eType = SMT_NORMAL;
	  sMsg.byConfirmType = SMCT_NONE;
	  sMsg.byPriority = 1;
	  sMsg.byParamSize = 0;

      if (Item[m_Items[nFIdx].nIdx].GetTime()->bYear>0)
		    t_sprintf(sMsg.szMessage, "限时:使用时间到,回收(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
	  else
	  {
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_BaiTan)
		    t_sprintf(sMsg.szMessage, "摆摊:卖掉(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
		else
			t_sprintf(sMsg.szMessage, "物品:消耗(%s)OK.", Item[m_Items[nFIdx].nIdx].GetName());
	  }
	  sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	  CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
   }

	m_Items[nFIdx].nIdx = 0;
	m_Items[nFIdx].nPlace = 0;
	m_Items[nFIdx].nX = 0;
	m_Items[nFIdx].nY = 0;
	m_Items[nFIdx].nPrice = 0;
	m_Items[nFIdx].nLianJieFlg =0;
	m_Items[nFIdx].nModel =0;

	m_FreeIdxItemList.Insert(nFIdx);  //增加一个可用节点
	m_UseIdxItemList.Remove(nFIdx);   //删除一个已用节点
	m_ItemCount --;
	if  (m_ItemCount<0)
		m_ItemCount =0;
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::FindFree
// Purpose		: 查找可用空索引
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindFree()
{
	return m_FreeIdxItemList.GetNext(0); //未用表
}

/*!*****************************************************************************
// Function		: KItemList::FindSame
// Purpose		: 
// Return		: int 
// Argumant		: int nGameIdx	  物品的索引
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindSame(int nGameIdx)
{
	int mIdx = 0;
	while(1)
	{
		mIdx = m_UseIdxItemList.GetNext(mIdx); //可用表
		if (!mIdx)
			break;
		
		if (m_Items[mIdx].nIdx == nGameIdx)
			return mIdx;
	}
	return 0;
}
/*!*****************************************************************************
// Function		: KItemList::Init
// Purpose		: 初始化玩家装备列表
// Return		: BOOL
// Comments		:
// Author		: Spe 初始化容器大小
*****************************************************************************/
int KItemList::Init(int nPlayerIdx)
{
	m_PlayerIdx = nPlayerIdx;
	m_Hand = 0;
	m_nBackHand = 0;
	m_nMaskLock = FALSE;	// 面具锁
	m_Room[room_equipment].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_repository].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT); //储物箱
	m_Room[room_trade].Init(TRADE_ROOM_WIDTH,TRADE_ROOM_HEIGHT);
	m_Room[room_exbox1].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_exbox2].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT); 
	m_Room[room_exbox3].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_equipmentex].Init(REPOSITORY_ROOM_WIDTH,REPOSITORY_ROOM_HEIGHT);
	m_Room[room_tradeback].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_immediacy].Init(IMMEDIACY_ROOM_WIDTH,IMMEDIACY_ROOM_HEIGHT);
	m_Room[room_give].Init(EQUIPMENT_ROOM_WIDTH,TRADE_ROOM_HEIGHT);
	m_Room[room_giveback].Init(EQUIPMENT_ROOM_WIDTH,EQUIPMENT_ROOM_HEIGHT);
	//if (!m_Room[room_dazao].Init(DAZAO_ROOM_WIDTH,DAZAO_ROOM_HEIGHT))
	//      printf(" [room_dazao] 内存初始化失败A\n");
	if (!m_Room[room_cailiao].Init(CAILIAO_ROOM_WIDTH, CAILIAO_ROOM_HEIGHT))
          printf(" [room_cailiao] error\n");

	m_Room[room_trade1].Init(TRADE_ROOM_WIDTH,TRADE_ROOM_HEIGHT);		// 这个的大小必须与 room_trade 的大小一样

	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));				// 玩家装备的道具（对应游戏世界中道具数组的索引）
	ZeroMemory(m_Items, sizeof(m_Items));						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	m_nListCurIdx = 0;											// 用于 GetFirstItem 和 GetNextItem
	
	m_FreeIdxItemList.Init(MAX_PLAYER_ITEM);
	m_UseIdxItemList.Init(MAX_PLAYER_ITEM);

	for (int i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdxItemList.Insert(i);
	}
	//CCMessageBox("增加包袱空间成功","增加包袱空间成功");
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::CanEquip
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe----  是否可以装备这个同伴
*****************************************************************************/
BOOL KItemList::CanTonEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex,nTonNpcIdx=0;

	KMagicAttrib* pData = NULL;
	
	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nTonNpcIdx<=0)
	{	
		return FALSE;
	}

  /*
	if (nPlace != -1 && !Fit(nIdx, nPlace))
	{
		return FALSE;
	}*/

	 if (!Item[nIdx].CheckRequirement(magic_item_needtongban))
	 {//如果这个装备没有这个 同伴的需求属性
		 return FALSE;
	 }
	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,Item[nIdx].GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}



/*!*****************************************************************************
// Function		: KItemList::CanEquip
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe----
*****************************************************************************/
BOOL KItemList::CanEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(nIdx, nPlace))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,Item[nIdx].GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);

//---------------------------锁魂检测
	//Item[nIdx].GetWonName() ; //归属者

//---------------------------
	return TRUE;
}

BOOL KItemList::CanEquip(KItem* pItem, int nPlace /* = -1 */)
{
	if (m_PlayerIdx <= 0 || !pItem)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(pItem, nPlace))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)pItem->GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData,nPlace,pItem->GetID()))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}
//需求属性 是否可以装备
BOOL KItemList::EnoughAttrib(void* pAttrib,int nPlace,DWORD nDwIdx)
{
	//SearchID
	KMagicAttrib*	pData = (KMagicAttrib *)pAttrib;
	//_ASSERT(pData);
	if (!pData) return false;
	int nNpcIdx=0;
	if (nDwIdx>0)
	{
		int nIdx = SearchID(nDwIdx);
		if (nIdx>0)
		{
			if (Item[nIdx].GetUseKind()==1 && Item[nIdx].GetUseMapIdx()>0)
			{//无任何要求
				return TRUE;
			}
		}
	}
	
	switch(pData->nAttribType)
	{
	case magic_requirestr:
		if (Player[m_PlayerIdx].m_nCurStrength < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiredex:
		if (Player[m_PlayerIdx].m_nCurDexterity < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirevit:
		if (Player[m_PlayerIdx].m_nCurVitality < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireeng:
		if (Player[m_PlayerIdx].m_nCurEngergy < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirelevel:
		{
		    if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level < pData->nValue[0])
			{ //需求等级
			   return FALSE;
			}  
		}      
		break;
	case magic_requiremenpai:
		//if (Player[m_PlayerIdx].m_cFaction.m_nFirstAddFaction != pData->nValue[0])
		if (Player[m_PlayerIdx].m_cFaction.m_nCurFaction != pData->nValue[0])
		{//需求门派
			return FALSE;
		}
		break;
	case magic_requireseries:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series != pData->nValue[0])
		{//需求五行
			return FALSE;
		}
		break;
	case magic_requiresex:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_nSex != pData->nValue[0])
		{//需求男女
			return FALSE;
		}
		break;	 
	case magic_item_nouser:	  //不能使用的门派
		if (Player[m_PlayerIdx].m_cFaction.m_nFirstAddFaction == pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_item_noseries: //某五行不能使用
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series == pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_item_needskill:	  //需要学会什么技能才能装备
		if (!Npc[Player[m_PlayerIdx].m_nIndex].m_SkillList.CheckHaveSkill(pData->nValue[0]))
		{	
			return FALSE;
		}
		break;
	case magic_item_needreborn:	  //需要转生次数才能装备
		if (Player[m_PlayerIdx].m_cReBorn.GetReBornNum()<pData->nValue[0])
		{	
			return FALSE;
		}
		break;
	case magic_item_needcity:	  //需要城主专用 ---针对帮战地图
		{
			/*if (!Player[m_PlayerIdx].m_cTong.m_nFlag)
			{//入帮 并是 帮主
				return FALSE;
			}
			if  (Player[m_PlayerIdx].m_cTong.GetFigure()== enumTONG_FIGURE_MASTER)
			{
				return FALSE;
			}*/
			return FALSE;
		}
		break;
	case magic_item_needbangzhu:	  //需要帮主专用 ---针对帮战地图
		{
		   /*if (!Player[m_PlayerIdx].m_cTong.m_nFlag)
		   {//入帮 并是 帮主
			   return FALSE;
		   }
		   if  (Player[m_PlayerIdx].m_cTong.GetFigure()== enumTONG_FIGURE_MASTER)
		   {
			   return FALSE;
		   }*/
		   return FALSE;
		}
		break;
	case magic_item_needtongban:  //需要激活并召唤了同伴才可以装备
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_TongBanNum)
		{//已经召唤了宠物的			
		    nNpcIdx =NpcSet.SearchID(Npc[Player[m_PlayerIdx].m_nIndex].m_TongBanNum);
		}
		if (!nNpcIdx || nPlace!=pData->nValue[0]) //部位不形同
		{//需求同伴 并是 部位合适的
		
		///t_sprintf(mag,"不可以装备:P:%d;D:%d",nPlace,pData->nValue[0]);
	 		return FALSE; 
		}
		break;	
	default:
		break;
	}
//-------------------------------------------锁魂检测 是否可以装备

//-------------------------------------------

	return TRUE;
}
/*!*****************************************************************************
// Function		: KItemList::Equip---同伴换装
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::TonEquip(int nIdx, int nPlace,int nIsLoign)
{
   //人物的换装
	if (m_PlayerIdx <= 0 || nIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex,nTonNpcIdx=0;

	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nTonNpcIdx<=0)
	{//同伴没有召唤的
		return FALSE;
	}

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		return FALSE;
	}

	int nEquipPlace = nPlace;

/*	if (-1 == nEquipPlace)
	{
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	}
	else if (!Fit(nIdx, nEquipPlace))
	{
		return FALSE;
	} */

	int nType = 0;
	// 换装 外观改变

	// 更新装备自身坐标
	m_ItemBox[nEquipPlace]       = nIdx;
	m_Items[nItemListIdx].nPlace = pos_dazao;
	m_Items[nItemListIdx].nX     = nEquipPlace;
	m_Items[nItemListIdx].nY     = 0;

	int nActive =3;// GetEquipEnhance(nEquipPlace); //获取该部位 被激活的

	int nActiveInfo[2];

	   ZeroMemory(nActiveInfo,sizeof(nActiveInfo));

	if (Item[nIdx].IsBlue())
	{//如果是蓝装
	   nActiveInfo[0] = 1;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
       
	} 
    else if  (Item[nIdx].IsPurple())
	{//如果是紫装
	   nActiveInfo[0] = 2;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
	} 
    else if  (Item[nIdx].IsGold())
	{//如果是黄装
		nActiveInfo[0] = 3;
		nActiveInfo[1] = Item[nIdx].GetLevel();//等级
	} 
    else
	{//白装
		nActiveInfo[0] = 0;
		nActiveInfo[1] = 0;
	}

    if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
        Item[nIdx].ApplyMagicAttribToNPC(&Npc[nTonNpcIdx],nActive,nActiveInfo);  // 应用激活的魔法到NPC 身上
        ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    //InfectionNextEquip(nEquipPlace, TRUE);                                //穿上 激活暗属性并应用魔法在NPC身上  
	}
    else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
	{//面具 不受限制
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nTonNpcIdx], nActive,nActiveInfo);
		ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    //InfectionNextEquip(nEquipPlace, TRUE);
	}
 /*
	if (itempart_weapon == nEquipPlace)
	{//换装武器的话，设置相关的基本攻击		
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill(); //更新武器技能参数
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();        //设置物理伤害相关
	}
//	if(Item[nIdx].GetGoldId()>0)
 */
	if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
	   GetIfActive();     //黄金套装激活列表GetIfActive()或全身激活的;
	}
  
//	Player[m_PlayerIdx].UpdataCurData(nIsLoign);                 //更新自身的属性参数

	Npc[nTonNpcIdx].UpdataNpcCurData(m_PlayerIdx,nEquipPlace);
    Player[m_PlayerIdx].UpdataCurData(nIsLoign);

    //ReSkillEnhance(m_PlayerIdx,nNpcIdx);
	return TRUE;

}

/*!*****************************************************************************
// Function		: KItemList::Equip---换装
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Equip(int nIdx, int nPlace,int nIsLoign)
{//人物的换装
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (m_PlayerIdx <= 0 || nIdx <= 0 || nNpcIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;	

//	(strcmp(nNpcName,Npc[nNpcID].Name) == 0)
//	if (strcmp(Item[nIdx].GetWonName(),"系统")!=0)
//		return FALSE;

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		return FALSE;
	}

	int nEquipPlace = nPlace;
	if (-1 == nEquipPlace)
	{
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	}
	else if (!Fit(nIdx, nEquipPlace))
	{
		return FALSE;
	}

	int nType = 0;
	// 换装 外观改变
	switch(nEquipPlace)
	{
	case itempart_head:
		if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_HelmType = Item[nIdx].GetItemResIdx();
			break;
		}

		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
			   nType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
			else
			   nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
		}

		if (nType)
			Npc[nNpcIdx].m_HelmType = nType;
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nIdx].GetParticular(),Item[nIdx].GetLevel());
		break;
	case itempart_body:
		{
		    if (Item[nIdx].GetItemResIdx()>0)
			{ 
			  Npc[nNpcIdx].m_ArmorType = Item[nIdx].GetItemResIdx();
			  break;
			}   

		    if (Item[nIdx].GetGoldId())
			{ 	
		      if (Item[nIdx].GetIsPlatima())
		        nType = 	g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		      else
			    nType =  g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
			}   

		    if (nType)
			  Npc[nNpcIdx].m_ArmorType = nType;
		    else
        	  Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nIdx].GetParticular(),Item[nIdx].GetLevel());
		}	
		break;
	case itempart_weapon:

	    if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_WeaponType = Item[nIdx].GetItemResIdx();
			break;
		}
		
		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
				nType  = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
			else
			    nType  = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());

            /*#ifdef _SERVER 
			char nMsg[64]={0};
			t_sprintf(nMsg,"--是否白金A: %d,武器类型: %d,ID:%d ",Item[nIdx].GetIsPlatima(),nType,Item[nIdx].GetGoldId());
			msgshow(nMsg);
            #endif
			*/
		}

		if (nType)
		{
			Npc[nNpcIdx].m_WeaponType = nType;

            /*#ifdef _SERVER 
			char nMsg[64]={0};
			   t_sprintf(nMsg,"--是否白金B: %d,武器类型: %d ",Item[nIdx].GetIsPlatima(),Npc[nNpcIdx].m_WeaponType);
               msgshow(nMsg);
            #endif*/
		}
		else
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
		
		break;
	case itempart_horse:
		if (Item[nIdx].GetGoldId())
		{
			if (Item[nIdx].GetIsPlatima())
				nType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		   else
			    nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
		}

		if (nType)
			Npc[nNpcIdx].m_HorseType = nType;
		else
			Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetHorseRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());

		if (Item[nIdx].GetItemResIdx()>0)
		{
			Npc[nNpcIdx].m_HorseType = Item[nIdx].GetItemResIdx();
		}

		if (Npc[nNpcIdx].m_HorseType >= 0)
		{
			Npc[nNpcIdx].m_bRideHorse = TRUE;
		}
		else
		{
			Npc[nNpcIdx].m_bRideHorse = FALSE;
		}
		break;
	case itempart_mask:	   // 面具的类型
		if (!m_nMaskLock)
		{	 
			if (Item[nIdx].GetItemResIdx()>0)
			{   
			  Npc[nNpcIdx].m_MaskType = Item[nIdx].GetItemResIdx();
			  break;
			}
			
			Npc[nNpcIdx].m_MaskType = Item[nIdx].GetBaseMagic();
		}
		break;
	case itempart_pifeng:	// 披风
		    if (Item[nIdx].GetItemResIdx()>0)
			{    
			   Npc[nNpcIdx].m_PifengType = Item[nIdx].GetItemResIdx();
			   break;
			}
			
			if (Item[nIdx].GetGoldId())
			{
				//if (Item[nIdx].GetIsPlatima())
				//	Npc[nNpcIdx].m_PifengType = g_ItemChangeRes.GetPlatinaRes(Item[nIdx].GetGoldId());
		        //else

				int nEqLevel=Item[nIdx].GetLevel();
				if  (nEqLevel<=10)
				    Npc[nNpcIdx].m_PifengType = nEqLevel;//g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());//获取外观ID	
				else
					Npc[nNpcIdx].m_PifengType = nEqLevel%10;

			}
			else
			{
				int nEqLevel=Item[nIdx].GetLevel();
				if  (nEqLevel<=10)
					Npc[nNpcIdx].m_PifengType =g_ItemChangeRes.GetPifengRes(Item[nIdx].GetParticular(),nEqLevel);
				else
					Npc[nNpcIdx].m_PifengType = nEqLevel%10;
			  
			}
			   //Npc[nNpcIdx].m_ChiBangType=(BYTE)Item[nIdx].GetChiBangRes();
			break;
	case itempart_shiping:	// 饰品
		 if (Item[nIdx].GetItemResIdx()>0)
		 { 
			Npc[nNpcIdx].m_WeaponType = Item[nIdx].GetItemResIdx();
		 }  
	     break;
	case itempart_yinjian:	//印鉴
		//Npc[nNpcIdx].m_ChiBangType=Item[nIdx].GetChiBangRes();
		if (Item[nIdx].GetItemResIdx()>0)
		{ 
			Npc[nNpcIdx].m_ArmorType = Item[nIdx].GetItemResIdx();
		}  
		break;

	default:
		break;
	}
	// 更新装备自身坐标
	m_EquipItem[nEquipPlace]     = nIdx;
	m_Items[nItemListIdx].nPlace = pos_equip;
	m_Items[nItemListIdx].nX     = nEquipPlace;
	m_Items[nItemListIdx].nY     = 0;

	int nActive = GetEquipEnhance(nEquipPlace); //获取该部位 被激活的

	int nActiveInfo[2];

	   ZeroMemory(nActiveInfo,sizeof(nActiveInfo));

	if (Item[nIdx].IsBlue())
	{//如果是蓝装
	   nActiveInfo[0] = 1;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
       
	} 
    else if  (Item[nIdx].IsPurple())
	{//如果是紫装
	   nActiveInfo[0] = 2;
       nActiveInfo[1] = Item[nIdx].GetIsWhere();
	} 
    else if  (Item[nIdx].IsGold())
	{//如果是黄装 和 白金
		nActiveInfo[0] = 3;
		nActiveInfo[1] = Item[nIdx].GetLevel();//等级
	} 
    else
	{//白装
		nActiveInfo[0] = 0;
		nActiveInfo[1] = 0;
	}

    if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
        Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx],nActive,nActiveInfo);  // 应用激活的魔法到NPC 身上
        ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    InfectionNextEquip(nEquipPlace, TRUE);                                //穿上 激活暗属性并应用魔法在NPC身上  
	}
    else if (Item[nIdx].GetIsCanUse()==0 && Item[nIdx].GetGenre()==item_equip && Item[nIdx].GetDetailType()==equip_mask) 
	{//面具 不受限制
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], nActive,nActiveInfo);
		ZeroMemory(nActiveInfo,sizeof(nActiveInfo));
	    InfectionNextEquip(nEquipPlace, TRUE);
	}

	if (itempart_weapon == nEquipPlace)
	{//换装武器的话，设置相关的基本攻击		
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill(); //更新武器技能参数
		Player[m_PlayerIdx].SetNpcDamageAttrib();        //设置物理伤害相关
	}
//	if(Item[nIdx].GetGoldId()>0)
	if (Item[nIdx].GetIsCanUse()==1)   //是可以使用的
	{
	   GetIfActive();     //黄金套装激活列表GetIfActive()或全身激活的;
	}

	Player[m_PlayerIdx].UpdataCurData(nIsLoign);         //更新自身的属性参数
    //ReSkillEnhance(m_PlayerIdx,nNpcIdx);
	return TRUE;
}

void KItemList::ReSkillEnhance(int m_PlayerIdx,int nNpcIdx)
{
	int nActiveSkillID;
	nActiveSkillID=Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
	if (Npc[nNpcIdx].m_Currentskillenhance!= 0)
	{
		int nlistIndex = Npc[nNpcIdx].m_SkillList.FindSame(nActiveSkillID);
		if (nlistIndex)                 //查找激活的当前攻击技能ID
		{
			if (Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(nlistIndex) > 0)  //等级大于0的才 增加
			{
				Npc[nNpcIdx].m_SkillList.AddEnChance(nlistIndex,Npc[nNpcIdx].m_Currentskillenhance);
			}			
		}
	}
}

/*!*****************************************************************************
// Function		: KItemList::InfectionNextEquip
// Purpose		: 装备影响其他装备-没有激活的属性 开关
// Return		: void 
// Argumant		: int nEquipPlace
// Argumant		: BOOL bEquipUp 是装上（TRUE）还是卸下（FALSE）
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemList::InfectionNextEquip(int nEquipPlace,BOOL bEquip)
{
	if (m_PlayerIdx <= 0)
		return;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i]; //激活的装备列表
		if (!m_EquipItem[nInfectionEquip])                    //如果没有装备这个装备
			continue;
		if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),Item[m_EquipItem[nInfectionEquip]].GetSeries()))  //装备五行相生
		{
			int nActive = GetEquipEnhance(nInfectionEquip);
			//_ASSERT(nActive > 0);
			if (nActive<=0) return;
			if (bEquip)//穿上激活暗属性
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else//卸下（移除暗属性）
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
	}
}
//全身激活
/*
void KItemList::InAllNextEquip(int nEquipPlace,BOOL bEquip)
{
	if (m_PlayerIdx <= 0)
		return;
	
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	
	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		//int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i]; //激活的装备列表
		if (!m_EquipItem[nEquipPlace])                    //如果没有装备这个装备
			continue;
		//if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),Item[m_EquipItem[nInfectionEquip]].GetSeries()))  //装备五行相生
		{
			int nActive = GetEquipEnhance(nInfectionEquip); //获取被激活的暗属性的数量
			_ASSERT(nActive >0);
			if (bEquip)                                      //穿上激活暗属性
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else//卸下（移除暗属性）
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
	}
}
*/
BOOL KItemList::RemoveItemBox(int nIdx , int nPos)
{
    int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < MAX_ITEM_BOX; ++i)
		{
			if (m_ItemBox[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == MAX_ITEM_BOX)
			return FALSE;

	}
	else
	{
		if (m_ItemBox[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}

	m_ItemBox[i] = 0; 
	return TRUE;
}
/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除同伴装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/

BOOL KItemList::UnTonEquip(int nIdx, int nPos)
{
   	int i = 0,nTonNpcIdx=0;

	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;	 //主人的索引

	if (Npc[nNpcIdx].m_TongBanNum)
	{//已经召唤了宠物的	
		nTonNpcIdx =NpcSet.SearchID(Npc[nNpcIdx].m_TongBanNum);
	}

	if (nIdx <= 0 || nNpcIdx<0 || nTonNpcIdx<=0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 53; i < 78; ++i)
		{
			if (m_ItemBox[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == 78)
			return FALSE;

	}
	else
	{
		if (m_ItemBox[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	int nActive =3;// GetEquipEnhance(i);

	//if (m_bActiveSet) //是黄金装备并且是套装
	//	nActive = 3;  //激活的默认明属性有三个

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nTonNpcIdx],nActive);
	// 移除该装备可能对它激活的装备的影响。
	//InfectionNextEquip(i,FALSE);
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_ItemBox[i] = 0;   //卸载了装备 就设置这个位置为空 ！
	// 换装		
	Npc[nTonNpcIdx].UpdataNpcCurData(m_PlayerIdx,i);

	if (!GetIfActive())                            //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据
     //ReSkillEnhance(m_PlayerIdx,nNpcIdx);  //重置对攻击技能的加成！
	return TRUE;

}


/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/
BOOL KItemList::UnEquip(int nIdx, int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < itempart_num; ++i)
		{
			if (m_EquipItem[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);

	if (m_bActiveSet) //是黄金装备并且是套装
		nActive = 3;  //激活的默认明属性有三个

	int nActiveInfo[2];

	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;

	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	// 移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i,FALSE);
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_EquipItem[i] = 0;   //卸载了装备 就设置这个位置为空 ！
	// 换装
	int nNeWidx=0,nType=0;
	switch(i)
	{//外观的改变
	case itempart_head:
		Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
	case itempart_body:
		Npc[nNpcIdx].m_ChiBangType=0;
		Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		break;
	case itempart_weapon:
		Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
		break;
	case itempart_horse:
		Npc[nNpcIdx].m_HorseType = -1;
		Npc[nNpcIdx].m_bRideHorse = FALSE;
		break;
	case itempart_mask:
		if (!m_nMaskLock)
		{
		   Npc[nNpcIdx].m_MaskType = 0;
		}
		break;
	case itempart_pifeng:
		  Npc[nNpcIdx].m_PifengType=0;//g_ItemChangeRes.GetPifengRes(0, 0);
		break;
    case itempart_shiping:	// 饰品	 武器
		{
			if (nNeWidx=m_EquipItem[itempart_weapon])
			{
				if (Item[nNeWidx].GetItemResIdx()>0)
				{  
			       Npc[nNpcIdx].m_WeaponType = Item[nNeWidx].GetItemResIdx();
			       break;
				}  
		
		       if (Item[nNeWidx].GetGoldId())
			   {   
			     if (Item[nNeWidx].GetIsPlatima())
				    nType  = g_ItemChangeRes.GetPlatinaRes(Item[nNeWidx].GetGoldId());
			     else
			        nType  = g_ItemChangeRes.GetGoldItemRes(Item[nNeWidx].GetGoldId());
			   }

		      if (nType)
			  { 
			     Npc[nNpcIdx].m_WeaponType = nType;
			  }  
		      else
			     Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nNeWidx].GetDetailType(), Item[nNeWidx].GetParticular(), Item[nNeWidx].GetLevel());
 
			}
			else
				Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0); 
	
		} 

		  break;
	case itempart_yinjian:	// 印鉴	 衣服
		{//改变的 衣服
		   
		   if (nNeWidx=m_EquipItem[itempart_body])
		   {
			   
			   if (Item[nNeWidx].GetItemResIdx()>0)
			   {
				   Npc[nNpcIdx].m_ArmorType = Item[nNeWidx].GetItemResIdx();
				   break;
			   }
			   
			   if (Item[nNeWidx].GetGoldId())
			   {	
				   if (Item[nNeWidx].GetIsPlatima())
					   nType = 	g_ItemChangeRes.GetPlatinaRes(Item[nNeWidx].GetGoldId());
				   else
					   nType =  g_ItemChangeRes.GetGoldItemRes(Item[nNeWidx].GetGoldId());
			   }
			   
			   if (nType)
				   Npc[nNpcIdx].m_ArmorType = nType;
			   else
				   Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nNeWidx].GetParticular(),Item[nNeWidx].GetLevel());
		   }
		   else
			   Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);

		   //Npc[nNpcIdx].m_ChiBangType=0;
		}
		break;	  
	default:
		break;
	}

	if (itempart_weapon == i)
	{
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	
   	if (!GetIfActive())                            //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData(FALSE,TRUE);       //更新数据  
     //ReSkillEnhance(m_PlayerIdx,nNpcIdx);  //重置对攻击技能的加成！
	return TRUE;
}


/*!*****************************************************************************
// Function		: KItemList::UnTimeEquip  //移除时间到的属性
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/

BOOL  KItemList::tempUnEquip(int nIdx,int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;
	i = nPos;
	if  (i<0) return false;
	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);
	if (m_bActiveSet) //是黄金装备并且是套装  或者 是全身激活的
		nActive = 3;  //激活的默认明属性有三个
	int nActiveInfo[2];
	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;

	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	// 移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i, FALSE);

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	

	if (!GetIfActive())  //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据  

#ifdef _SERVER
	for (i = 1;i < MAX_NPCSKILL;++i)  
	{//通知客户端改变全部技能数据
		if (Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=1 &&
			Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=2 && 
			Npc[nNpcIdx].m_SkillList.GetSkillId(i)!=53 &&
			Npc[nNpcIdx].m_SkillList.GetCurrentLevelByIdx(i) > 0)
		{ 
			KSkill * pSkill =(KSkill *)g_SkillManager.GetSkill(Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillId, Npc[nNpcIdx].m_SkillList.m_Skills[i].SkillLevel);
			if (pSkill) 
			{
				int nReg=0;
				nReg = pSkill->NewCanCastSkill(nNpcIdx,-1,nNpcIdx);  //武器限制
				//如果该技能属于被动辅助技能时，则设置Npc状态
				if (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg==1)
				{//有武器在手 无限制的 或 与武器相符的被动技能就设置，无限状态
					pSkill->Cast(nNpcIdx, -1, nNpcIdx,-1,SKILL_SLT_Npc,0,1);
				}
				else if  (pSkill->GetSkillStyle() == SKILL_SS_PassivityNpcState && nReg<=0)
				{//空手 状态 有武器限制的被动技能 被清除！
					pSkill->Cast(nNpcIdx, -1, nNpcIdx,1);
				}
			} 
			pSkill=NULL;
			////-------------------------------------------------------------------------------	
		}
	}
#endif
	return TRUE;
}

BOOL KItemList::TimeUnEquip(int nIdx, int nPos)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{//如果容器位置为空
		for (i = 0; i < itempart_num; ++i)
		{
			if (m_EquipItem[i] == nIdx)
			{//如果装备着的 等于手上的
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	// 东西不对
			return FALSE;

		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	int nActive = GetEquipEnhance(i);
	if (m_bActiveSet) //是黄金装备并且是套装  或者 是全身激活的
		nActive = 3;  //激活的默认明属性有三个
	int nActiveInfo[2];
	if (Item[nIdx].GetGoldId()>0)
	{
		nActiveInfo[0]=3;
		nActiveInfo[1]=Item[nIdx].GetLevel();
	}
	else
	{
		nActiveInfo[0]=0;
		nActiveInfo[1]=0;
		
	}

	Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx],nActive,nActiveInfo);
	// 移除该装备可能对它激活的装备的影响。
	InfectionNextEquip(i, FALSE);
	
	//这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	//m_EquipItem[i] = 0;   //这个位置设置没有装备

	if (itempart_weapon == i)
	{
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
		Player[m_PlayerIdx].SetNpcDamageAttrib();  //伤害属性？
	}	
   	if (!GetIfActive())  //如果不是黄金激活
		Player[m_PlayerIdx].UpdataCurData();       //更新数据  
	return TRUE;
}



int KItemList::_getEquipPlaceByItemIdx(int nidx)
{
	if (m_PlayerIdx <= 0) return false;
	//int nSeries = Item[m_EquipItem[nPlace]].GetSeries();  //装备五行
	for (int i=0;i<itempart_num;i++)
	{
		if (m_EquipItem[i]>0 && m_EquipItem[i]==nidx)
			return i;
	}

	return -1;
}

//获取装备的部位类型
int KItemList::GetEquipPlace(int nType)
{
	int nRet = -1;
	switch(nType)
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		nRet = itempart_weapon;
		break;
	case equip_armor:
		nRet = itempart_body;
		break;
	case equip_helm:
		nRet = itempart_head;
		break;
	case equip_boots:
		nRet = itempart_foot;
		break;
	case equip_ring:
		nRet = itempart_ring1;
		break;
	case equip_amulet:
		nRet = itempart_amulet;
		break;
	case equip_belt:
		nRet = itempart_belt;
		break;
	case equip_cuff:
		nRet = itempart_cuff;
		break;
	case equip_pendant:
		nRet = itempart_pendant;
		break;
	case equip_horse:
		nRet = itempart_horse;
		break;
	case equip_mask:	// mat na
		nRet = itempart_mask;
		break;
	case equip_pifeng:	//披风
		nRet = itempart_pifeng;
		break;
	case equip_yinjian:	// mat na
		nRet = itempart_yinjian;
		break;
	case equip_shiping:	// mat na
		nRet = itempart_shiping;
		break;
	default:
		break;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::Fit
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Fit(int nIdx, int nPlace)
{
	BOOL	bRet = FALSE;
	//_ASSERT(Item[nIdx].GetGenre() == item_equip);
	if (Item[nIdx].GetGenre() != item_equip) return false;

	switch(Item[nIdx].GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	// mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_pifeng:	// mat na
		if (nPlace == itempart_pifeng)
			bRet = TRUE;
		break;
	case equip_yinjian:	// mat na
		if (nPlace == itempart_yinjian)
			bRet = TRUE;
		break;
	case equip_shiping:	// mat na
		if (nPlace == itempart_shiping)
			bRet = TRUE;
		break;
	}
	return bRet;
}

BOOL KItemList::Fit(KItem* pItem, int nPlace)
{
	BOOL	bRet = FALSE;
	//_ASSERT(pItem->GetGenre() == item_equip);
	if (pItem->GetGenre() != item_equip) return false;
	switch(pItem->GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	// mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_pifeng:	//披风
		if (nPlace == itempart_pifeng)
			bRet = TRUE;
		break;
	case equip_yinjian:	// mat na
		if (nPlace == itempart_yinjian)
			bRet = TRUE;
		break;
	case equip_shiping:	// mat na
		if (nPlace == itempart_shiping)
			bRet = TRUE;
		break;
	}
	return bRet;
}

int KItemList::GetEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		//_ASSERT(0);
		return 0;
	}
	int nRet = 0;

	int nSeries = Item[m_EquipItem[nPlace]].GetSeries();  //装备五行
	if (g_IsAccrue(Npc[nNpcIdx].m_Series, nSeries))  //人物五行 和装备五行相生 就激活
		nRet ++;

	for (int i = 0; i < MAX_ITEM_ACTIVE; ++i)
	{
		// 看看激活这个装备的装备中是否相生这个装备
		int nActivedEquip = m_EquipItem[ms_ActivedEquip[nPlace][i]]; //被激活索引
		if (nActivedEquip)
		{
			if (g_IsAccrue(Item[nActivedEquip].GetSeries(), nSeries))  //物品五行相生 就激活
				nRet++;
		}
	}

	if (nRet>3)
		nRet=3;

	return nRet;
}
//服务器端 吃药品 执行脚本  --右键执行脚本
BOOL KItemList::EatMecidineS(int nIdx,int nx,int nY)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	int nGenre = Item[nIdx].GetGenre();

	if  (nGenre==item_equip)
		return false;
	//_ASSERT(nGenre == item_medicine || nGenre == item_townportal || nGenre == item_task || nGenre == item_mine);
	
	if (nGenre == item_medicine)  //药品
	{
		//Npc[nNpcIdx].GetMpsPos(&xx, &yy,&Mmap);
		if (Item[nIdx].GetUseKind() == 0 && Item[nIdx].GetUseMapIdx()>0 && Item[nIdx].GetUseMapIdx()!=SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID)
		{
		   Player[m_PlayerIdx].m_ItemList.ClientShowMsg("药品:该专用药品不能使用!");
	       return FALSE;
		}

		int Val[2];
		ZeroMemory(Val,sizeof(Val));
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx],3,Val);  //增加魔法属性到 NPC身上
	}

	if (nGenre == item_townportal)  //土地符
	{
		if (!Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode)
		{// || Npc[Player[m_PlayerIdx].m_nIndex].m_AutoplayId>0
#ifdef WIN32
			CCMessageBox("挂机/非战斗模式,不能回城!","提示:");
#else
			CCMessageBox(UTEXT("挂机/非战斗模式,不能回城!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			return FALSE;
		}
	}

	if (nGenre == item_task || nGenre == item_mine)
	{
		//客户端
		int mItemInfo[3],mprg;
		ZeroMemory(mItemInfo,sizeof(mItemInfo));
            mItemInfo[0]=Item[nIdx].GetGenre();
            mItemInfo[1]=Item[nIdx].GetDetailType();
            mItemInfo[2]=Item[nIdx].GetParticular();
			mprg=NpcSet.CheckForBit(mItemInfo,"IsUseStack",3);
       if (Item[nIdx].GetStackNum() > 1 && mprg==0)  //是否可以叠加使用
	   {
#ifdef WIN32
		   CCMessageBox("提示:物品叠加,不能使用!","提示:");
#else
		   CCMessageBox(UTEXT("提示:物品叠加,不能使用!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		   return FALSE;
	   }
	}
//药品
	if (Item[nIdx].GetStackNum() > 1)
	{
		Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);  //数量减一
	}

	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)   //坐着的 让站起来 做动作
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
	return TRUE;
}

int KItemList::AutoChangeItem(int nIdx)
{
	//int nSplace=GetEquipPlace(Item[nIdx].GetDetailType()); //获取是那个部位
	//int nEuqIdx=GetEquipment(nSplace); //获取他的索引

	if (nIdx<=0 || nIdx>=MAX_ITEM) return false;
	//开始检测包袱是否可以装下
	int nItemWith=1,nItemHigth=1;
	    nItemWith = Item[nIdx].GetWidth();
	    nItemHigth=	Item[nIdx].GetHeight();

	int x,y;
	if (!CheckCanPlaceInEquipment(nItemWith,nItemHigth, &x, &y))
	{
#ifdef WIN32
		CCMessageBox("包袱空间不足","提示");
#else
		CCMessageBox(UTEXT("包袱空间不足",1).c_str(),UTEXT("提示",1).c_str());
#endif 
		return false;
	}
	//-----------------------
	unsigned long nItemDwId=0;
	nItemDwId=Item[nIdx].GetID();
	PLAYER_AUTO_EQUIP eItem;
	eItem.ProtocolType = (BYTE)c2s_playerautoequip;
	eItem.bKind        = 1;
	eItem.uIiemDwId    = nItemDwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&eItem, sizeof(PLAYER_AUTO_EQUIP));
	return true;
}

//使用物品  客户端 -- 包袱 到 装备栏
int KItemList::UseItem(int nIdx,int cnx,int cny)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (0 == FindSame(nIdx))
	{
		return 0;
	}

	int		nRet = 0;
	switch(Item[nIdx].GetGenre())
	{
	case item_equip:
		{
		    int nSplace=GetEquipPlace(Item[nIdx].GetDetailType());
			int nEuqIdx=GetEquipment(nSplace);
			int nEuqWith=1,nEuqHigth=1;

			if 	(nSplace == itempart_ring1 && nEuqIdx)
			{//原来位置有东西 上芥子

				if (!GetEquipment(itempart_ring2))//如果没有东西
				   nSplace = itempart_ring2;
				 //开始检测包袱
			}
			DWORD nItemDwId=0;
			if (CanEquip(nIdx,nSplace)) //部位是否合适该装备
			{//开始换装 和卸装备
				//ClientShowMsg("<color=yellow>换装<color>:该装备满足换装条件");
				///Equip(nIdx,nSplace)   //右键换物品装备nNpcIdx 
				if (nEuqIdx)
				{//有装备开始卸装，开始检测包袱 是否有空间

				//	UnEquip(nEuqIdx,nSplace);
					
				}

				if ((Item[nIdx].GetDurability()>=0 && Item[nIdx].GetDurability()<=3) || Item[nIdx].GetIsCanUse()==0)
				{
				   ClientShowMsg("换装C:该装备不满足换装条件");
				   nRet = REQUEST_EQUIP_ITEM;
				   break;
				}

				/*if (Item[nIdx].GetUseKind!=0 && Item[nIdx].GetUseMapIdx()>0)
				{//等级限制的
					
					if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level>=Item[nIdx].GetUseMapIdx())
					{
						ClientShowMsg("<color=yellow>换装D<color>:该装备不满足换装条件");
						nRet = REQUEST_EQUIP_ITEM;
						break;
					}
				}*/

				//然后开始上装
				/*	int		nPlace;
				int		nX;     // 0 1 2 3 4
				int		nY;     // 0123456789
				int     nSIsRongqi;
				int     nDIsRongqi;
                 */
				nItemDwId=Item[nIdx].GetID();

				PLAYER_AUTO_EQUIP eItem;
				eItem.ProtocolType = (BYTE)c2s_playerautoequip;
				eItem.bKind        = 0;
				eItem.uIiemDwId    = nItemDwId;
				if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)&eItem, sizeof(PLAYER_AUTO_EQUIP));
			}
			else
			{//不能装备该装备
#ifdef WIN32
				CCMessageBox("该装备不满足换装条件","提示:");
#else
				CCMessageBox(UTEXT("该装备不满足换装条件",1).c_str(),UTEXT("提示",1).c_str());
#endif
			}

		    nRet = REQUEST_EQUIP_ITEM;
		}
		break;
	case item_townportal:   //使用回城
	case item_medicine:
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_task:          //药品
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_mine:
		if (EatMecidineS(nIdx,cnx,cny))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	default:
		break;
	}
	return nRet;
}


int KItemList::GetFreePositionCount(int nWidth, int nHeight,int RoomKind)
{
	if (nWidth < 1 || nHeight < 1 || RoomKind<room_equipment || RoomKind>=room_num)
	{
		return 0;
	}

	return m_Room[RoomKind].GetFreeRoomCount(nWidth, nHeight);
}
//寻找空位置
//#ifndef _SERVER
BOOL KItemList::SearchPosition(int nWidth, int nHeight, ItemPos* pPos)
{
	if (nWidth < 1 || nHeight < 1 || NULL == pPos)
	{
		return FALSE;
	}

	POINT	pPt;
	if (m_Room[room_equipment].FindRoom(nWidth, nHeight, &pPt)==TRUE)
	{

        pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}
	else
	{
	  //if (0 != m_Hand)
//		{
//		return FALSE;
//		}
		pPos->nPlace = pos_hand;
		pPos->nX = 0;
		pPos->nY = 0;
		return FALSE;
	}
	return TRUE;
}

//自动换装
void KItemList::AutoEquip(ItemPos SrcPos,ItemPos DesPos,DWORD nSrcItemDwIdx,int nEuqPlace,int nKind)
{
    if (!nSrcItemDwIdx)
	   	return;
	if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
		return;

	int nSrcItemIdx=ItemSet.SearchID(nSrcItemDwIdx);


	 int nsListIdx = FindSame(nSrcItemIdx);
	//m_Items[nsListIdx].nPlace = pos_equiproom;
	 if (nsListIdx)
	 {
		 SrcPos.nPlace=m_Items[nsListIdx].nPlace;
		 SrcPos.nX=m_Items[nsListIdx].nX;
	     SrcPos.nY=m_Items[nsListIdx].nY;
	 }
	int nEuqWith=0,nEuqHigth=0,nEuqIdx=0;

	switch(SrcPos.nPlace)
	{//源目标
	case pos_equiproom:
		{//包袱移动到 装备栏目
		    if (DesPos.nPlace==pos_equiproom)
			{
				 nEuqIdx=GetEquipment(nEuqPlace);

				if (nEuqIdx)
				{//如果有装备,就换下
					nEuqWith = Item[nEuqIdx].GetWidth();
					nEuqHigth= Item[nEuqIdx].GetHeight();
					UnEquip(nEuqIdx,nEuqPlace);	
				}
				//else
				{//否则直接换上,清空包袱位置信息
					if (!m_Room[room_equipment].PickUpItem(nSrcItemIdx,SrcPos.nX,SrcPos.nY,Item[nSrcItemIdx].GetWidth(),Item[nSrcItemIdx].GetHeight()))
						return;

					Equip(nSrcItemIdx,nEuqPlace); //换到装备栏
					//设置包袱信息
					if (nEuqIdx && m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nEuqIdx,Item[nEuqIdx].GetWidth(),Item[nEuqIdx].GetHeight()))
					{
						int nListIdx = FindSame(nEuqIdx);
						m_Items[nListIdx].nPlace = pos_equiproom;
						m_Items[nListIdx].nX = DesPos.nX;
						m_Items[nListIdx].nY = DesPos.nY;
						//m_Hand = 0;
						//m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;	
					}
				}
			}
		}
		break;
	case pos_equip:
		{//原来是装备栏的
			if (DesPos.nPlace==pos_equiproom)
			{//移动到包袱
				nEuqIdx=GetEquipment(nEuqPlace);
				if (nEuqIdx)
				{//如果有装备,就换下
					nEuqWith = Item[nEuqIdx].GetWidth();
					nEuqHigth= Item[nEuqIdx].GetHeight();
					UnEquip(nEuqIdx,nEuqPlace);
				}
				//if (!m_Room[room_equipment].PickUpItem(nSrcItemIdx,SrcPos.nX,SrcPos.nY,Item[nSrcItemIdx].GetWidth(),Item[nSrcItemIdx].GetHeight()))
				//	return;
				if (nEuqIdx && m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nEuqIdx,Item[nEuqIdx].GetWidth(),Item[nEuqIdx].GetHeight()))
				{
					int nListIdx             = FindSame(nEuqIdx);
					m_Items[nListIdx].nPlace = pos_equiproom;
					m_Items[nListIdx].nX     = DesPos.nX;
					m_Items[nListIdx].nY     = DesPos.nY;
				}
			}
		}
		break;
	default:
		break;
	}
	//客户端界面处理
	//uParam = (KUiObjAtRegion*)pInfo -> 物品数据及其放置区域位置信息
	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品

	KUiObjAtContRegion pInfo1, pInfo2;	//源容器 目标容器

	if (nSrcItemIdx == 0)
	{
		pInfo1.Obj.uGenre = CGOG_NOTHING;
		pInfo1.Obj.uId = 0;
		pInfo1.Region.Width = 0;
		pInfo1.Region.Height = 0;
	}
	else
	{
		pInfo1.Obj.uGenre = CGOG_ITEM;
		pInfo1.Obj.uId = nSrcItemIdx;
		pInfo1.Region.Width = Item[nSrcItemIdx].GetWidth();
		pInfo1.Region.Height = Item[nSrcItemIdx].GetHeight();
	}

	if (nEuqIdx == 0)
	{
		pInfo2.Obj.uGenre = CGOG_NOTHING;
		pInfo2.Obj.uId = 0;
		pInfo2.Region.Width = 0;
		pInfo2.Region.Height = 0;
	}
	else
	{
		pInfo2.Obj.uGenre = CGOG_ITEM;	    //目标装备
		pInfo2.Obj.uId = nEuqIdx;
		pInfo2.Region.Width  = Item[nEuqIdx].GetWidth();
		pInfo2.Region.Height = Item[nEuqIdx].GetHeight();
	}
	
	int PartConvert[itempart_num] = 
	{
		    UIEP_HEAD,
			UIEP_BODY,
			UIEP_WAIST,
			UIEP_HAND,
			UIEP_FOOT,
			UIEP_FINESSE,
			UIEP_NECK,
			UIEP_FINGER1,
			UIEP_FINGER2,
			UIEP_WAIST_DECOR,
			UIEP_HORSE,
			UIEP_MASK,	// mat na
			UIEP_PIFENG,	//披风
			UIEP_YINJIAN,		//印鉴
			UIEP_SHIPING,	//饰品
	};
	    //客户端对象信息赋值
		switch(SrcPos.nPlace)
		{
		case pos_equiproom:
			pInfo1.Region.h = SrcPos.nX;
			pInfo1.Region.v = SrcPos.nY;
			pInfo1.Region.IsRongqi = 1;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.Region.h = DesPos.nX;
			pInfo2.Region.v = DesPos.nY;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.Region.IsRongqi = 1;
			//t_sprintf(msg,"[KR_equiproom] X:%d-Y:%d",DesPos.nX,DesPos.nY);
			//ClientShowMsg(msg);
			break;
		case pos_equip:
				/*pInfo1.Region.h = 0;
			pInfo1.Region.v = PartConvert[SrcPos.nX];
			pInfo1.eContainer = UOC_EQUIPTMENT;
			pInfo1.Region.IsRongqi = 0;
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartConvert[DesPos.nX];
			pInfo2.eContainer = UOC_EQUIPTMENT;
			pInfo2.Region.IsRongqi = 0;*/
			pInfo2.Region.h = DesPos.nX;
			pInfo2.Region.v = DesPos.nY;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.Region.IsRongqi = 1;
			break;
		}
		//开始交换物品
	       //设置减少
		  if (nKind<=0) //换装
		    CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0); //容器减少

		   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1); //容器增加
}


//#ifndef _SERVER

//Check o trong trong hanh trang Kientm
BOOL KItemList::SearchItemPositionKienTM(int nWidth, int nHeight,int posX,int posY,int roomType)
{
	if (nWidth < 1 || nHeight < 1 || posX < 0 || posY < 0)
	{
		return FALSE;
	}
	int checkItem;
	checkItem = m_Room[roomType].FindItem(posX,posY);
	if (checkItem>0){
		return FALSE;
	}

	return TRUE;
}

BOOL KItemList::SearchItemPosition(int nWidth, int nHeight, ItemPos* pPos,int roomType)
{
	if (nWidth < 1 || nHeight < 1 || NULL == pPos)
	{
		return FALSE;
	}
	
	POINT	pPt;
	if (!m_Room[roomType].FindRoom(nWidth, nHeight, &pPt))
	{
		
/*		if (0 != m_Hand)
		{
			return FALSE;
		}*/
		pPos->nPlace = pos_hand;
		pPos->nX = 0;
		pPos->nY = 0;
		return FALSE;
	}
	else
	{
		if  (roomType==room_equipment)
		    pPos->nPlace = pos_equiproom;
		else if  (roomType==room_repository)
            pPos->nPlace = pos_repositoryroom;
		else if  (roomType==room_exbox1)
            pPos->nPlace = pos_exbox1room;
		else if  (roomType==room_exbox2)
            pPos->nPlace = pos_exbox2room;
		else if  (roomType==room_exbox3)
            pPos->nPlace = pos_exbox3room;
        else if  (roomType==room_equipmentex)
            pPos->nPlace = pos_equiproomex;
		else if  (roomType==room_trade)
            pPos->nPlace = pos_traderoom;
		else if  (roomType==room_trade1)
            pPos->nPlace = pos_trade1;
		else if  (roomType==room_immediacy)
            pPos->nPlace = pos_immediacy;
		else if  (roomType==room_give)
            pPos->nPlace = pos_give;
		else if  (roomType==room_dazao)
            pPos->nPlace = pos_dazao;
		else if  (roomType==room_cailiao)
            pPos->nPlace = pos_cailiao;
		else
            pPos->nPlace = pos_equiproom;

		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}

	return TRUE;
}
//#endif
//搜索物品的DWID
int	KItemList::SearchID(int nID)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdxItemList.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
			return m_Items[nIdx].nIdx;
	}
	return 0;
}

//搜索物品的DWID
int	KItemList::SearchItemsIdx(int nDwid)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nItemIdx = 0;
	while(1)
	{
		nItemIdx = m_UseIdxItemList.GetNext(nItemIdx);
		if (!nItemIdx)
			break;
		if (Item[m_Items[nItemIdx].nIdx].GetID() == (DWORD)nDwid)
			return nItemIdx;
	}
	return 0;
}

void KItemList::ExchangeMoney(int pos1, int pos2, int nMoney)
{
	if (pos1 < 0 || pos2 < 0 || pos1 > room_trade || pos2 > room_trade)
		return;
/*	if (pos1 == room_repository && !Player[m_PlayerIdx].m_CUnlocked)
	{
		return;
	}*/

	if  (nMoney<0)
		return;

	if (m_Room[pos1].AddmClient(-nMoney))		// 源位置能拿出这么多钱来
	{
		if (!m_Room[pos2].AddmClient(nMoney))	// 目的地能放不下去
		{
			m_Room[pos1].AddmClient(nMoney);	// 还原源位置的钱
		}
	}
	else
	{
		return;
	}
	if (pos1 == room_equipment && pos2 == room_repository)
		SendClientCmdStoreMoney(0, nMoney);
	else if (pos1 == room_repository && pos2 == room_equipment)
		SendClientCmdStoreMoney(1, nMoney);
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的总钱数
//----------------------------------------------------------------------
int KItemList::GetMoneyAmount()
{
	return (m_Room[room_equipment].GetmServer() + m_Room[room_repository].GetmServer());
}

int KItemList::GetXuAmount()
{
	return (m_Room[room_equipment].GetxCLient());
}
//----------------------------------------------------------------------
//	功能：得到物品栏钱数
//----------------------------------------------------------------------
int KItemList::GetEquipmentMoney()
{
	return m_Room[room_equipment].GetmServer();
}

int KItemList::GetEquipmentXu()
{
	return m_Room[room_equipment].GetxCLient();
}


BOOL KItemList::AddmServer(int nRoom, int nMi)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddmClient(nMi) )
		return FALSE;

	return TRUE;
}
//增加金币
BOOL KItemList::AddxCliet(int nRoom, int ni)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddxCliet(ni) )
		return FALSE;
	return TRUE;
}

BOOL KItemList::CostMoney(int nMoney)
{
	if (nMoney > GetEquipmentMoney())
		return FALSE;

	if (!m_Room[room_equipment].AddmClient(-nMoney) )
		return FALSE;
	return TRUE;
}
//减少金币
BOOL KItemList::CostXu(int nXu)
{
	if (nXu > GetEquipmentXu())
		return FALSE;

	if ( !m_Room[room_equipment].AddxCliet(-nXu) )
		return FALSE;

	return TRUE;
}

BOOL KItemList::DecMoney(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (nMoney > m_Room[room_equipment].GetmServer())
	{
		nMoney -= m_Room[room_equipment].GetmServer();
		SetRoomMoney(room_equipment, 0);
		if (nMoney > m_Room[room_repository].GetmServer())
			SetRoomMoney(room_repository, 0);
		else
			AddmServer(room_repository, -nMoney);
	}
	else
	{
		AddmServer(room_equipment, -nMoney);
	}
	return TRUE;
}
/*
BOOL KItemList::DecXu(int nXu)
{
	if (nXu < 0)
		return FALSE;

	if (nXu > m_Room[room_equipment].GetXu())
	{
		nXu -= m_Room[room_equipment].GetXu();
		SetRoomXu(room_equipment, 0);
	}
	else
	{
		AddXu(room_equipment, -nXu);
	}

#ifdef _SERVER
	SendXuSync();
#endif

	return TRUE;
}
*/
void KItemList::SetClient(int nMon1, int nMon2, int nMon3)
{
	m_Room[room_equipment].SetClientMon(nMon1);
	m_Room[room_repository].SetClientMon(nMon2);
	m_Room[room_trade].SetClientMon(nMon3);
	KUiObjAtContRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = nMon2;
	sMoney.eContainer = UOC_STORE_BOX;
	//CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
}

void KItemList::SetServer(int nXu)
{
	m_Room[room_equipment].SetxClient(nXu);
}

void KItemList::SetRoomMoney(int nRoom, int nMoney)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetClientMon(nMoney);
}

void KItemList::SetRoomXu(int nRoom, int nXu)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetxClient(nXu);
}
//===========================蓝装提炼============================================
int KItemList::BlueTiLian(int nLindexID,int nBaoshiID,int nModel,int nWengGangPos)
{
			// 装备熔炼  ---提炼晶石
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0|| Item[nLindexID].IsBlue()))
				{
				}
			    return 0;
}

//=====================装备纹钢提炼 和 重铸==================================
int KItemList::TiLian(int nLindexID,int nBaoshiID,int nModel,int nWengGangPos)
{
			// 蓝装提篮炼  ---提炼晶石
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0|| Item[nLindexID].IsBlue()))
				{
				}
					return 0;

}
//================黄金装备纹钢=====================
int KItemList::WenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
					return 0;
}


//================蓝装装备融合========================
int KItemList::BlueRonHe(int nIdxa,int nIdxb,int nEqType,int nRank,int nCurLuck,int nsLevel)
{
    if (!Player[m_PlayerIdx].ExeItemScriptA("\\script\\system\\sysitemronhe.lua","ItemRonHe",Item[nIdxa].GetName(),Item[nIdxb].GetName(),nIdxa,nIdxb,nEqType,nRank,nCurLuck,nsLevel,0,0,0))
	   return FALSE;

    return TRUE;
}

//================蓝装装备辨识========================
int KItemList::BlueBianShi(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
					return 0;
}

//===============紫装纹钢=============================
 
int KItemList::PurpleWenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
		return 0;
}

//================蓝装装备纹钢=========================
int KItemList::BlueWenGang(int nLindexID,int nBaoshiID)
{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
		return 0;
}
//宝石灵合
int KItemList::LingHe(int nLindexID,int nBaoshiID)
{
/*			// 装备熔炼  ---熔炼晶石
		//	if (Item[nBaoshiID].GetGenre() == item_task && (Item[nBaoshiID].GetDetailType() >= 243 && Item[nBaoshiID].GetDetailType() <=371))
			{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
#ifdef _SERVER
					int nItemClass		= Item[nLindexID].GetGenre();
					int nDetailType		= Item[nLindexID].GetDetailType();
					int nParticularType	= Item[nLindexID].GetParticular();
					int nLevel			= Item[nLindexID].GetLevel();
					int nSeries			= Item[nLindexID].GetSeries();
					int nLuck			= Item[nLindexID].GetItemParam()->nLuck;
					int nEnChance		= Item[nLindexID].GetEnChance();
					int nGoldId			= Item[nLindexID].GetGoldId();	
					int nVersion		= Item[nLindexID].GetItemParam()->nVersion;
					int	nGndrom			= Item[nLindexID].GetItemParam()->uRandomSeed;
					int nPoint			= Item[nLindexID].GetPoint();   //紫装IsPurple
					int nRPoint			= Item[nLindexID].IsRong();
					int nMlevel         = Item[nBaoshiID].GetLevel();  //纹钢的等级
					int nBlue           = Item[nLindexID].IsBlue();
                   	
					int nItemLevel[6];
					int nRItemLevel[6];
					//int nBItemLevel[6];

					ZeroMemory(nItemLevel, sizeof(nItemLevel));
					ZeroMemory(nRItemLevel, sizeof(nRItemLevel));
					//同步魔法属性
					for (int i = 0; i < 6; ++i)
					{
						nItemLevel[i]  = Item[nLindexID].GetItemParam()->nGeneratorLevel[i];
						nRItemLevel[i] = Item[nLindexID].GetItemParam()->nRGeneratorLevel[i];
//						nBItemLevel[i] = Item[nLindexID].GetItemParam()->nBGeneratorLevel[i];
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////
        //确定成功的机率
					int nVal=0,nRate=0;
                    //--------------概率检测--------------------------------------------
					g_GameSetTing.GetInteger2("ServerConfig","GoldLingHeRank",&nVal,&nRate);
					if (g_Random(nVal) < g_Random(nRate))
					{
                        Remove(nBaoshiID);
						ItemSet.Remove(nBaoshiID); //删除材料
						msgshow("提示:装备灵合失败!再接再励!");	
						return 0;
					}
                   //-------------------------------------------------------------------
		//可溶链的 装备位置
		KTabFile tabMagic;
		tabMagic.Load(TABFILE_MAGICALEVEL_PATH);
		int nRow;
        nRow = Item[nBaoshiID].GetSkillType();  //获取改属性的行号
		int nAddLevel =nMlevel;   //为纹钢的等级
		if (nLuck < 0) 
			nLuck = 0;
		if (nLuck > 10) 
			nLuck = 10;
		if (nAddLevel > 10) 
			nAddLevel = 10;

		int nMinRow,nMgIdx,IsChonWenGang=1,nSkilType;
		tabMagic.GetInteger(nRow+1,"MAGATTRLVL_START",0,&nMinRow);//在魔法属性表的行数
		tabMagic.GetInteger(nRow+1,"MAGIC_ID",0,&nMgIdx);         //属性ID的
		tabMagic.GetInteger(nRow+1,"MAGIC_CHONG",1,&IsChonWenGang);  //是否可以重复纹钢
		tabMagic.GetInteger(nRow+1,"MAGIC_TTPE",0,&nSkilType); 
		if (nSkilType!=1)
		{
		   msgshow("提示:该灵合宝石已经失效!");
           return 0; 
		}
		nMinRow +=  nAddLevel;  //产生属性的等级（品数）
		tabMagic.Clear();

		tabMagic.Load(TABFILE_MAGICATTRIB_PATH);
		int nMagic;
	    tabMagic.GetInteger(nMinRow,Item[nLindexID].GetGenre() + 13,0,&nMagic);
		if (nMagic > 1) 
		{//可出现该属性的装备类型
		   msgshow("提示:此灵合宝石与装备类型不符合!");
		   return 0;
		}
		tabMagic.Clear();
		//已经镶套了的相同属性的不能镶套
	if  (IsChonWenGang==0)
		 for (int p = 0;p < 6;++p)
		 { 
		    if(Item[nLindexID].GetRongAttribType(p) == nMgIdx)
			{
			   msgshow("提示:该黄金已经灵合了同类型宝石!");
               return 0; 
			}
		 }  
        //同步熔炼属性值 和 判断 属性石头的位置
		int nMun=0;
		for (int k = 0;k <6;++k)
		{
		    if (nMun == 0 && Item[nLindexID].GetBaoShiAttribType(k) <= 0)  //位置相同 并且没有属性的空位置
			{
				nBItemLevel[k] = nMinRow;   //设定属性行数
				nMun++;
				continue;
			}
//			    nBItemLevel[k] = Item[nLindexID].m_GeneratorParam.nBGeneratorLevel[k];
		}
		//char msg[64];
		//t_sprintf(msg,"灵合:(%d)(%d)(%d)(%d)(%d)(%d)",nBItemLevel[0],nBItemLevel[1],nBItemLevel[2],nBItemLevel[3],nBItemLevel[4],nBItemLevel[5]);
		//msgshow(msg);
		            int nStackNum = 1;
//////////////////////////////////////////////////////////////////////////////////////////////////////
					int	nIndex;			
					if (nGoldId>0)
					{//黄金的灵合
						nIndex = ItemSet.AddGold(nGoldId,nItemLevel,nSeries,nEnChance,nRPoint,0,0,0,0,0,nRItemLevel,nBItemLevel,nLevel);
					} 
					else
					{//其他紫装和蓝装的灵合
						nPoint=7;
						nIndex = ItemSet.AddRong(0,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nRItemLevel,NULL,nVersion,nGndrom,1,nEnChance,nRPoint,nPoint);
					}	
					if (nIndex <= 0)
					{
						return 0;
					}	
					
					Remove(nBaoshiID);
					ItemSet.Remove(nBaoshiID);
					Remove(nLindexID);
					ItemSet.Remove(nLindexID);

					int		x, y;
					if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
					{
						Add(nIndex, pos_equiproom, x, y);   //包袱
					}
					else
					{
                        msgshow("提示:包袱空间不足,前功尽弃啦!");
						Remove(nIndex);
					    ItemSet.Remove(nIndex);
					    return 0;
					}
                        msgshow("提示:宝石灵合成功!恭喜!恭喜!");

					return nIndex;
#endif 
				}
					return 0;
			}
			*/
			return 0;
}
//装备熔炼
int KItemList::RongLian(int nLindexID,int nBaoshiID)
{
			// 装备熔炼  ---熔炼晶石
			if (Item[nBaoshiID].GetGenre() == item_task && (Item[nBaoshiID].GetDetailType() >= 243 && Item[nBaoshiID].GetDetailType() <=371))
			{
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
					return 0;
			}
			return 0;
}
//服务器端产生物品
int KItemList::CilentAddItem(int nItemIdx)
{
			// 包袱强化  ---红狂星宝石
		//   if (Item[nItemIdx].GetGenre() == item_task && Item[nItemIdx].GetDetailType() == 238)
	      if (nItemIdx)
		  { 
          }   
	return 0;
}

//装备强化
int KItemList::QiangHua(int nLindexID,int nBaoshiID,int nRank)
{
			// 包袱强化  ---红狂星宝石
			//if (Item[nBaoshiID].GetGenre() == item_task && Item[nBaoshiID].GetDetailType() == 238)
				if (nLindexID && Item[nLindexID].GetGenre() == item_equip && (Item[nLindexID].GetGoldId() > 0 || Item[nLindexID].IsPurple()>0 || Item[nLindexID].IsBlue()))
				{
				}
					return 0;
}
//移动物品 装备 快捷栏 包袱  储物箱 扩展箱
void KItemList::ExchangeItem(ItemPos* SrcPos, ItemPos* DesPos,int nIsMianBam)
{
	if (SrcPos->nPlace != DesPos->nPlace)  //不同地方的东西 就返回
		return;

	int nTempHand = m_Hand;
	int	nEquipIdx1 = 0;
	// 源装备从SrcPos到手上，手上的装备到DesPos
	switch(SrcPos->nPlace)
	{
	case pos_hand:  //手里
		//g_DebugLog("%s exchange item error", Npc[Player[m_PlayerIdx].m_nIndex].Name);
		return;
		break;
	case pos_equip:   //身上装备
		if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;

		if (SrcPos->nX < 0 || SrcPos->nX >= itempart_num || DesPos->nX < 0 || DesPos->nX >= itempart_num)
			return;
       
        if (m_Hand)
		{//手里原来有装备
			if 	(Item[m_Hand].GetIsWhere()>0 && Item[m_Hand].GetIsCanUse()<=0)  //是否装备
			{//已经辨识的装备
             /* if (nEquipIdx1)
			  {//否则保留原装备
				Equip(nEquipIdx1, SrcPos->nX);
			  }	  */
			  return;
			}

			if 	(Item[m_Hand].GetGenre() ==item_equip && Item[m_Hand].GetDetailType() !=equip_mask && Item[m_Hand].GetIsCanUse()<=0)  //是否装备
			{//已经辨识的装备			
              /* if (nEquipIdx1)
		       {//否则保留原装备
				 Equip(nEquipIdx1, SrcPos->nX);
			   }*/
			 return;
			}
		}
		nEquipIdx1 = m_EquipItem[SrcPos->nX]; //检测该部位是否有装备

		if (nEquipIdx1)
		{//如果有装备
			UnEquip(nEquipIdx1, SrcPos->nX); //移除源装备
		}

		if (m_Hand)
		{//手里原来有装备
	
			if (CanEquip(m_Hand, DesPos->nX)) //部位是否合适该装备
			{//检测是否可以装备
				Equip(m_Hand, DesPos->nX);  //换装
				m_Hand = nEquipIdx1;        //(拿起了源装备)
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			else if (nEquipIdx1)
			{//否则保留原装备
				Equip(nEquipIdx1, SrcPos->nX);
			}
		}
		else
		{//手里没有东西 则拿起该目标装备
			m_Hand = nEquipIdx1;                              //手里装备的 索引
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;  //赋值给该物品的位置信息 在手里
		}
		break;
	case pos_immediacy:  //单对象容器快捷栏
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		// 判断是否有同类型的物品，有的话不让放下去
		if (m_Hand)
		{
		//	if (Item[m_Hand].GetGenre()==item_equip || Item[m_Hand].GetGenre()==item_task || Item[m_Hand].GetGenre()==item_materials|| Item[m_Hand].GetGenre()==item_mine)		
		    if 	(Item[m_Hand].GetGenre() == item_equip || Item[m_Hand].GetIsKuaiJie()<=0)  //是否可以快捷栏
			{
				return;
			}

			if (m_Room[room_immediacy].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType(),Item[m_Hand].GetParticular()))
			{
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				t_sprintf(sMsg.szMessage, strCoreInfo[MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE].c_str());
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return;
			}
		}
		nEquipIdx1 = m_Room[room_immediacy].FindItem(SrcPos->nX,SrcPos->nY);
		if (nEquipIdx1 < 0) 
			return; //这个位置没有东西
		
		// 先把东西拣起来
		if (nEquipIdx1)
		{//如果有东西
			if (!m_Room[room_immediacy].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			{
				return;
			}
		}

		if (m_Hand)
		{
			if (Item[m_Hand].CanStack(nEquipIdx1))   //是否可以叠加--手里和包袱里的是否相同
			{
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))  //叠加
				{
					Item[m_Hand].Remove();  //删除手里拿的
					Remove(m_Hand);
				}
				else
				{//叠加数大于50的
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM);  //手里
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM);  //实际物品的
				}

				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY,nEquipIdx1,Item[nEquipIdx1].GetWidth(),Item[nEquipIdx1].GetHeight());
				
				return;
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_immediacy].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight())
				&& Item[m_Hand].GetGenre() != item_equip/*(Item[m_Hand].GetGenre() == item_mine || Item[m_Hand].GetGenre() == item_medicine || Item[m_Hand].GetGenre() == item_townportal || Item[m_Hand].GetGenre() == item_task)*/)
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_immediacy;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_repositoryroom:  //多对象容器储物箱
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_repository].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
			if (Item[m_Hand].CanStack(nEquipIdx1))
			{
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM); //手里的减少
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM); //包袱的增加
				}
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;
			}
			// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
			if (m_Hand)
			{
				if (m_Room[room_repository].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
				{ 
				  int nListIdx = FindSame(m_Hand);
				  m_Items[nListIdx].nPlace = pos_repositoryroom;
				  m_Items[nListIdx].nX = DesPos->nX;
				  m_Items[nListIdx].nY = DesPos->nY;
				  m_Hand = nEquipIdx1;
				  m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
				} 
			// 把原来的东西放回去
			    else
				{ 
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				} 
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_exbox1room: // ruong mo rong 1  扩展箱
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox1].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox1].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox1].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox1room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox1].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;

	case pos_exbox2room: // ruong mo rong 2
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox2].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox2].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox2].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox2room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox2].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;

	case pos_exbox3room: // ruong mo rong 3
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_exbox3].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox3].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_exbox3].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox3room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_exbox3].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	// add new
	case pos_equiproomex: // mo rong hanh trang
		nEquipIdx1 = m_Room[room_equipmentex].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_equipmentex].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_equipmentex].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproomex;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_equipmentex].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	//find here
	case pos_equiproom:    //包袱
		//nEquipIdx1 = nIsMianBam;//m_Room[room_equipment].FindItem(SrcPos->nX, SrcPos->nY);
		int nItemIdx;
		int nListIdx ;
		nItemIdx = SearchItemsIdx(nIsMianBam);
		if (nItemIdx <= 0)
		{
			ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
			return;
		}
		nEquipIdx1 = m_Items[nItemIdx].nIdx;//m_Room[nSrcRoom].FindItem(SrcPos.nX, SrcPos.nY); //V? trí ban ??u có th? này kh?ng
		char msg[64];
		t_sprintf(msg,"[SR_equiproom] X:%d-Y:%d-nEquipIdx1:%d",SrcPos->nX,SrcPos->nY,nEquipIdx1);
		ClientShowMsg(msg);
		if (nEquipIdx1 < 0) //tr? l?i n?u kh?ng có gì
		{
			return;
		}
			
		m_Room[room_equipment].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
		m_Room[room_equipment].PlaceItem(DesPos->nX,DesPos->nY,nEquipIdx1,Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
		nListIdx = nEquipIdx1;
		if (nListIdx<0){
			return;
		}
		m_Items[nListIdx].nPlace = pos_equiproom;
		m_Items[nListIdx].nX = DesPos->nX;
		m_Items[nListIdx].nY = DesPos->nY;
		
		break;
	case pos_traderoom:
		if (!Player[m_PlayerIdx].CheckTrading() )	// 不在交易
			return;
		nEquipIdx1 = m_Room[room_trade].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_trade].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_trade].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_traderoom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				if (FindSame(nEquipIdx1))
					m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_trade].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_give:
		nEquipIdx1 = m_Room[room_give].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_give].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
			if (Item[m_Hand].CanStack(nEquipIdx1))
			{
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - MAX_STACKNUM);
					Item[nEquipIdx1].SetStackNum(MAX_STACKNUM);
				}
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;
			}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_give].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_give;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_dazao:  //单对象容器
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		// 判断是否有同类型的物品，有的话不让放下去
		if (m_Hand)
		{
//===================条件检测开始============================
			if  (nIsMianBam==-1)                 //升级玄晶矿石面板  1 3 5
			{
              //  int nLevelA=0,nLevelB=0,nLevelC=0;
			}
			else if  (nIsMianBam==-2)             //升级属性矿石面板 1 3 5
			{
			}
		    else if  (nIsMianBam==1)             //提炼玄晶矿石面板  1 3 5
			{
			}
			else if (nIsMianBam==2)               //提取属性面板
			{ 
		   }
		   else if (nIsMianBam==3)    //打造面板
		   {
		   }
		   else if (nIsMianBam==4)    //镶套面板
		   {
		   }
		   else if (nIsMianBam==5)    //图谱面板
		   {
/*
			  if (m_Room[room_dazao].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType(),Item[m_Hand].GetParticular()))
			  { 
#ifdef _SERVER
					BYTE byFinished = s2c_itemexchangefinish; //解锁
					if (g_pServer)
						g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));                   
					msgshow("提示:图谱栏已经有同类型的物品了！");
#endif
					return;
					
			  }  */
			
		   }
		   else if (nIsMianBam==6)    //提炼纹钢面板
		   {
		   } 
		   else if (nIsMianBam==7)    //纹钢面板
		   {
			/* int nGenre=Item[m_Hand].GetGenre();                //类别
			   int nPart=Item[m_Hand].GetDetailType();            //详细类别
			   int nParticular= Item[m_Hand].GetParticular();     //具体类别
			   int nStackNum  = Item[m_Hand].GetStackNum();    */  
		   }
		   else if (nIsMianBam==8)    //融合面板
		   {
		   }
		   else if (nIsMianBam==9)    //辨识装备面板
		   {	      
		   }
		   else if (nIsMianBam==10)    //灵合宝石面板
		   {
		   }
		   else if (nIsMianBam==11)    //升级称号面板
		   {
		   }
		   else if (nIsMianBam==12)    //神将
		   {
		   }
///////////////////同类检测////////////////////////
/*			if (m_Room[room_dazao].CheckSameDetailType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType()))
			{
#ifdef _SERVER
				BYTE	byFinished = s2c_itemexchangefinish; //解锁
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif
#ifndef _SERVER
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				t_sprintf(sMsg.szMessage,"%s", "提示:打造栏已经有同类型的物品了！");
				sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;		
	    	}*/
		}
//===================单容器条件检测结束============================
        
		//nEquipIdx1 = m_Room[room_dazao].FindItem(SrcPos->nX, SrcPos->nY); 
         if (SrcPos->nX>0)
            nEquipIdx1=m_ItemBox[SrcPos->nX];  //如果需要放进的容器中有物品 
		 
        //m_Hand = nEquipIdx1;                              //手里装备的 索引
		//m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;  //赋值给该物品的位置信息 在手里
		// 先把东西拣起来
		if (nEquipIdx1>0)
		{////如果需要放进的容器中有物品 
			if (SrcPos->nX>=53 &&  SrcPos->nX<=77)
			{//如果是同伴装备栏 攻丹 开始卸载 原来的装备
				UnTonEquip(nEquipIdx1,SrcPos->nX);
				//  UnEquip()
				//  Equip();

			}
			else
			  m_ItemBox[SrcPos->nX]=0;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
		  if (DesPos->nX>=53 && DesPos->nX<=77)
		  {//同伴装备栏
			if (CanTonEquip(m_Hand, DesPos->nX)) //部位是否合适该装备
			{//检测是否可以装备
				TonEquip(m_Hand,DesPos->nX);  //换装
				m_Hand = nEquipIdx1;           //(拿起了源装备)
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			else if (nEquipIdx1)
			{//否则保留原装备
				TonEquip(nEquipIdx1, SrcPos->nX);
			}
		  }
		  else
		  {//其他东西
			    int nListIdx = FindSame(m_Hand);
			    m_ItemBox[SrcPos->nX]=m_Hand;  //设置了这个位置的索引值为物品的索引
				m_Items[nListIdx].nPlace = pos_dazao;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;

				m_Hand = nEquipIdx1;   //手上的东西
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
		  }

		}
		else
		{// 如果手上没有东西，只需要把拣起来的东西放到鼠标上
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
				m_ItemBox[SrcPos->nX]=0;  //设置源位置没有东西
			}
		}
		break;
	case pos_cailiao:  //多对象容器
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
    
		nEquipIdx1 = m_Room[room_cailiao].FindItem(SrcPos->nX,SrcPos->nY);

		if (nEquipIdx1 < 0) //数据出错的 就返回
			return;

        if (m_Hand)
		{
            if (nIsMianBam==1)    //提炼面板
			{

			}
			else if (nIsMianBam==2)    //提取面板
			{
			}
			else if (nIsMianBam==3)    //打造面板
			{
			}
			else if (nIsMianBam==4)    //镶嵌面板
			{
			}
			else if (nIsMianBam==5)    //图谱面板
			{
			}
            else if (nIsMianBam==8)    //融合面板
			{
			}

//==================多容器检测结束======================================
		}

		// 先把东西拣起来
		if (nEquipIdx1>0)
		{//清空容器的物品位置为0
			if (!m_Room[room_cailiao].PickUpItem(nEquipIdx1,SrcPos->nX, SrcPos->nY,Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}	
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{//物品放进容器时 检测并设定各参数
			if (m_Room[room_cailiao].PlaceItem(DesPos->nX,DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_cailiao;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				//m_Items[nListIdx].nIsRongQi=1;  //是多对像容器
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_cailiao].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	}
	// 界面处理
	if (m_Hand != nTempHand)	// 交换成功了
	{
	//uParam = (KUiObjAtRegion*)pInfo -> 物品数据及其放置区域位置信息
	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品
		//char msg[64];

		KUiObjAtContRegion pInfo1, pInfo2;

		if (nTempHand == 0)
		{
			pInfo2.Obj.uGenre = CGOG_NOTHING;
			pInfo2.Obj.uId = 0;
			pInfo2.Region.Width = 0;
			pInfo2.Region.Height = 0;
		}
		else
		{
			pInfo2.Obj.uGenre = CGOG_ITEM;	//源装备
			pInfo2.Obj.uId = nTempHand;
			pInfo2.Region.Width = Item[nTempHand].GetWidth();
			pInfo2.Region.Height = Item[nTempHand].GetHeight();
		}
		
		if (m_Hand == 0)
		{
			pInfo1.Obj.uGenre = CGOG_NOTHING;
			pInfo1.Obj.uId = 0;
			pInfo1.Region.Width = 0;
			pInfo1.Region.Height = 0;
		}
		else
		{
			pInfo1.Obj.uGenre = CGOG_ITEM;
			pInfo1.Obj.uId = m_Hand;
			pInfo1.Region.Width = Item[m_Hand].GetWidth();
			pInfo1.Region.Height = Item[m_Hand].GetHeight();
		}

		int PartConvert[itempart_num] = 
		{
			UIEP_HEAD,
			UIEP_BODY,
			UIEP_WAIST,
			UIEP_HAND,
			UIEP_FOOT,
			UIEP_FINESSE,
			UIEP_NECK,
			UIEP_FINGER1,
			UIEP_FINGER2,
			UIEP_WAIST_DECOR,
			UIEP_HORSE,
			UIEP_MASK,	// mat na
			UIEP_PIFENG,	//披风
			UIEP_YINJIAN,		//印鉴
	        UIEP_SHIPING,	//饰品
		};
        //客户端对象信息赋值
		switch(SrcPos->nPlace)
		{
		case pos_immediacy:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_IMMEDIA_ITEM;
			pInfo2.eContainer = UOC_IMMEDIA_ITEM;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_equiproom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			//t_sprintf(msg,"[KR_equiproom] X:%d-Y:%d",DesPos->nX,DesPos->nY);
			//ClientShowMsg(msg);
			break;
		case pos_equip:
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartConvert[DesPos->nX];
			pInfo1.eContainer = UOC_EQUIPTMENT;
			pInfo2.eContainer = UOC_EQUIPTMENT;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_repositoryroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX;
			pInfo2.eContainer = UOC_STORE_BOX;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox1room: // ruong mo rong 1
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX1;
			pInfo2.eContainer = UOC_EX_BOX1;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox2room: // ruong mo rong 2
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX2;
			pInfo2.eContainer = UOC_EX_BOX2;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_exbox3room: // ruong mo rong 3
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX3;
			pInfo2.eContainer = UOC_EX_BOX3;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_equiproomex: // mo rong hanh trang
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_EX;
			pInfo2.eContainer = UOC_ITEM_EX;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_traderoom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_TO_BE_TRADE;
			pInfo2.eContainer = UOC_TO_BE_TRADE;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_give:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_GIVE;
			pInfo2.eContainer = UOC_ITEM_GIVE;
			pInfo1.Region.IsRongqi = SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = DesPos->nDIsRongqi;
			break;
		case pos_dazao:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_DAZAO;
			pInfo2.eContainer = UOC_ITEM_DAZAO;
			pInfo1.Region.IsRongqi = 0;//SrcPos->nSIsRongqi;
			pInfo2.Region.IsRongqi = 0;//DesPos->nDIsRongqi;
			//t_sprintf(msg,"[KR_dazao] X:%d-Y:%d",DesPos->nX,DesPos->nY);
			//ClientShowMsg(msg);
			break;
		case pos_cailiao:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_CAILIAO;
			pInfo2.eContainer = UOC_ITEM_CAILIAO;
			pInfo1.Region.IsRongqi = 1;
			pInfo2.Region.IsRongqi = 1;
			break;
		}
		//开始交换物品
	       //设置减少
		   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0);//容器减少
		   CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo2, 0);   //手里的东西减少
           //设置增加
		   CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo1, 1);   //手里增加 
		   CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1);//容器增加
	}
	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendSerCommand(do_stand);
	}
}

//多容器材料检测 
BOOL KItemList::CheckItemType(int ItemClass,int Detail,int nPart,int IsMianBan)
{
	  int nPartTyep[3],nReg=FALSE;
	  ZeroMemory(nPartTyep,sizeof(nPartTyep));
      int nRows=g_CompEquip.GetHeight();
	  for(int i=11;i<nRows;++i)
	  {
		  if (IsMianBan==1)
		  {//提炼
				  break;
		  } 
		  else if (IsMianBan==2)
		  {//提取
			  g_CompEquip.GetInt3s(i,"Distill",nPartTyep); //玄晶矿石
			  if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			  { 
				  nReg=TRUE;
				  break;
			  } 
		  } 
		  else if (IsMianBan==3)
		  {//打造
				  break;	  
		  } 
		  else if (IsMianBan==4)
		  {//镶套
			  g_CompEquip.GetInt3s(i,"Enchase",nPartTyep); //玄晶矿石
			  if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			  { 
				  nReg=TRUE;
				  break;
			  } 
		  } 
		  else if (IsMianBan==5)
		  {//图谱
             g_CompEquip.GetInt3s(i,"Atlas",nPartTyep); //玄晶矿石
		     if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			 { 
			   nReg=TRUE;
			   break;
			 } 
		  } 
		  else if (IsMianBan==8)
		  {//融合材料
             g_CompEquip.GetInt3s(i,"RongHe",nPartTyep);
		     if (ItemClass==nPartTyep[0] && Detail==nPartTyep[1] && nPart==nPartTyep[2])
			 { 
			   nReg=TRUE;
			   break;
			 } 
		  }
		  else
			  break;

	  }
	  return nReg;
}

//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
BOOL	KItemList::AutoMoveItem(ItemPos SrcPos,ItemPos DesPos,unsigned int nItemdwID)
{
	//if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
	//	return FALSE;

	BOOL bMove = FALSE;
	int	 nIdx=0, nListIdx=0,nItemIdx=0;
	int  nSrcRoom=0,nDesRoom=0;
	if (SrcPos.nPlace==pos_repositoryroom)
		nSrcRoom = room_repository;
	else if (SrcPos.nPlace==pos_exbox1room)
		nSrcRoom = room_exbox1;
	else if (SrcPos.nPlace==pos_exbox2room)
		nSrcRoom = room_exbox2;
	else if (SrcPos.nPlace==pos_exbox3room)
		nSrcRoom = room_exbox3;
	else if (SrcPos.nPlace==pos_equiproomex)
		nSrcRoom = room_equipmentex;
	else if (SrcPos.nPlace==pos_equiproom)
		nSrcRoom = room_equipment;
	else if (SrcPos.nPlace==pos_immediacy)
		nSrcRoom = room_immediacy;
	else if (SrcPos.nPlace==pos_give)
		nSrcRoom = room_give;
	else if (SrcPos.nPlace==pos_traderoom)
		nSrcRoom = room_trade;

	if (DesPos.nPlace==pos_repositoryroom)
		nDesRoom = room_repository;
	else if (DesPos.nPlace==pos_exbox1room)
		nDesRoom = room_exbox1;
	else if (DesPos.nPlace==pos_exbox2room)
		nDesRoom = room_exbox2;
	else if (DesPos.nPlace==pos_exbox3room)
		nDesRoom = room_exbox3;
	else if (DesPos.nPlace==pos_equiproomex)
		nDesRoom = room_equipmentex;
	else if (DesPos.nPlace==pos_equiproom)
		nDesRoom = room_equipment;
	else if (DesPos.nPlace==pos_immediacy)
		nDesRoom = room_immediacy;
	else if (DesPos.nPlace==pos_give)
		nDesRoom = room_give;
	else if (DesPos.nPlace==pos_traderoom)
		nDesRoom = room_trade;

	//目前只支持从room_equipment到room_immediacy
	switch (SrcPos.nPlace)      //源容器
	{
	case pos_traderoom:
	case pos_give:
	case pos_immediacy:
	case pos_repositoryroom:	// 贮物箱
	case pos_exbox1room:		// mo rong ruong 1  扩展箱
	case pos_exbox2room:		// mo rong ruong 2
	case pos_exbox3room:		// mo rong ruong 3
	case pos_equiproomex:	    // 同伴包袱
	case pos_equiproom:
		{
			switch (DesPos.nPlace)      //目标容器
			{
			case pos_traderoom:
			case pos_give:
			case pos_repositoryroom:	// 贮物箱
			case pos_exbox1room:		// mo rong ruong 1  扩展箱
			case pos_exbox2room:		// mo rong ruong 2
			case pos_exbox3room:		// mo rong ruong 3
			case pos_equiproomex:	    // 同伴包袱
			case pos_equiproom:
				{//m_Items[nItemIdx].nIdx
					nItemIdx = SearchItemsIdx(nItemdwID);
					if (nItemIdx <= 0)
					{
						ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
						return FALSE;
					}
					nIdx = m_Items[nItemIdx].nIdx;//m_Room[nSrcRoom].FindItem(SrcPos.nX, SrcPos.nY); //原始位置是否有这个东西
					if (nIdx <= 0)
						return FALSE;
					//清空源客户端容器位置为0
					m_Room[nSrcRoom].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					//设置客户端目标容器位置
					m_Room[nDesRoom].PlaceItem(DesPos.nX,DesPos.nY,nIdx,Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx =nItemIdx;// FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = DesPos.nPlace; //设置客户端物品目标容器信息
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			case pos_immediacy:
				{
					nItemIdx = SearchItemsIdx(nItemdwID);
					if (nItemIdx <= 0)
					{
						ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
						return FALSE;
					}
					nIdx = m_Items[nItemIdx].nIdx;// m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					if (!Item[nIdx].GetIsKuaiJie())
					{
						ClientShowMsg("Kh?ng th? ??t v?t ph?m này vào thanh phím t?t");
						return FALSE;
					}
					int nGenre = Item[nIdx].GetGenre();
					if  (nGenre==item_equip ||  nGenre ==item_materials || nGenre ==item_fusion)
					{
						ClientShowMsg("Kh?ng th? ??t v?t ph?m này vào thanh phím t?t");
						return FALSE;
					}

					if  (nGenre<=item_equip && nGenre >=item_number)
					{
						ClientShowMsg("Kh?ng th? ??t v?t ph?m này vào thanh phím t?t");
						return FALSE;
					}
					/*if (Item[nIdx].GetGenre() != item_medicine)
					{
						return FALSE;
					}*/
					m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					m_Room[room_immediacy].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx =nItemIdx;// FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = DesPos.nPlace;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			case pos_dazao:
				{
					nItemIdx = SearchItemsIdx(nItemdwID);
					if (nItemIdx <= 0)
					{
						ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
						return FALSE;
					}
					nIdx = m_Items[nItemIdx].nIdx;//m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					/*if (Item[nIdx].GetGenre() != item_medicine)
					{//物品限制
						_ASSERT(0);
						return FALSE;
					}*/
					//清空客户端容器位置为0
					m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					//设置客户端容器位置为idx
					//m_Room[room_dazao].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = nItemIdx;//FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
                    m_ItemBox[DesPos.nX]=nIdx;
					m_Items[nListIdx].nPlace = DesPos.nPlace;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					
					bMove = TRUE;
				}
				break;
			case pos_cailiao:
				{
					//nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					nItemIdx = SearchItemsIdx(nItemdwID);
					if (nItemIdx <= 0)
					{
						ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
						return FALSE;
					}
					nIdx =m_Items[nItemIdx].nIdx;
					if (nIdx <= 0)
						return FALSE;
					if (Item[nIdx].GetGenre() != item_medicine)
					{
						return FALSE;
					}
					m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					m_Room[room_cailiao].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = nItemIdx;//FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = DesPos.nPlace;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			}
		}
		break;
	case pos_dazao:  //源单对象容器
		{ 
			switch (DesPos.nPlace)  //目标容器
			{
			   case pos_equiproom:
				{
					nIdx = m_ItemBox[SrcPos.nX];//m_Room[room_dazao].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					//清空源容器位置为0
			/*		if (!m_Room[room_dazao].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					//设置目标容器位置为Idx
				*/	
				       m_ItemBox[SrcPos.nX]=0;

					if (!m_Room[room_equipment].PlaceItem(DesPos.nX,DesPos.nY,nIdx,Item[nIdx].GetWidth(),Item[nIdx].GetHeight()))
					{
					  ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
					   return FALSE;
					}
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = DesPos.nPlace;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				} 
				break;
			}	
		} 
		break;
	case pos_cailiao: //源多对象容器
		{ 
			switch (DesPos.nPlace)  //目标容器
			{
			 case pos_equiproom:
				 {
					 nItemIdx = SearchItemsIdx(nItemdwID);
					 if (nItemIdx <= 0)
					 {
						 ClientShowMsg("Kh?ng tìm th?y v?t ph?m");
						 return FALSE;
					 }
					 nIdx =m_Items[nItemIdx].nIdx;//m_Room[room_cailiao].FindItem(SrcPos.nX, SrcPos.nY);
					 if (nIdx <= 0)
						 return FALSE;
					 //清空源客户端容器位置为0
					   m_Room[room_cailiao].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					 //设置客户端目标容器位置
					   m_Room[room_equipment].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());

					 nListIdx = nItemIdx;//FindSame(nIdx);
					 if (nListIdx <= 0)
						 return FALSE;
					 m_Items[nListIdx].nPlace = DesPos.nPlace; //设置客户端物品目标容器信息
					 m_Items[nListIdx].nX = DesPos.nX;
					 m_Items[nListIdx].nY = DesPos.nY;
					 bMove = TRUE;
				 } 
				 break;
			} 
		} 
		break;
	}

	if (!bMove)
		return bMove;

	// 通知界面
	KUiObjAtContRegion sSrcInfo, sDestInfo;

	sSrcInfo.Obj.uGenre		= CGOG_ITEM;
	sSrcInfo.Obj.uId		= nIdx;
	sSrcInfo.Region.Width	= Item[nIdx].GetWidth();
	sSrcInfo.Region.Height	= Item[nIdx].GetHeight();
	sSrcInfo.Region.h		= SrcPos.nX;
	sSrcInfo.Region.v		= SrcPos.nY;

	if (SrcPos.nPlace == pos_immediacy)
		sSrcInfo.eContainer	= UOC_IMMEDIA_ITEM;

	else if (SrcPos.nPlace == pos_dazao)
	{
		sSrcInfo.Region.IsRongqi=0;
		sSrcInfo.eContainer	= UOC_ITEM_DAZAO;
	}
	else if (SrcPos.nPlace == pos_cailiao)
	{
        sSrcInfo.Region.IsRongqi=1;
	    sSrcInfo.eContainer	= UOC_ITEM_CAILIAO;
	}
    else if (SrcPos.nPlace == pos_equiproom)
	    sSrcInfo.eContainer = UOC_ITEM_TAKE_WITH;
	else if (SrcPos.nPlace == pos_repositoryroom)
		sSrcInfo.eContainer = UOC_STORE_BOX;
	else if (SrcPos.nPlace == pos_exbox1room)
		sSrcInfo.eContainer = UOC_EX_BOX1;
	else if (SrcPos.nPlace == pos_exbox2room)
		sSrcInfo.eContainer = UOC_EX_BOX2;
	else if (SrcPos.nPlace == pos_exbox3room)
		sSrcInfo.eContainer = UOC_EX_BOX3;
	else if (SrcPos.nPlace == pos_equiproomex)
		sSrcInfo.eContainer = UOC_ITEM_EX;
	else if (SrcPos.nPlace == pos_give)
		sSrcInfo.eContainer = UOC_ITEM_GIVE;
	else if (SrcPos.nPlace == pos_traderoom)
		sSrcInfo.eContainer = UOC_TO_BE_TRADE;


	sDestInfo.Obj.uGenre	= CGOG_ITEM;
	sDestInfo.Obj.uId		= nIdx;
	sDestInfo.Region.Width	= Item[nIdx].GetWidth();
	sDestInfo.Region.Height	= Item[nIdx].GetHeight();
	sDestInfo.Region.h		= DesPos.nX;
	sDestInfo.Region.v		= DesPos.nY;
	if (DesPos.nPlace == pos_immediacy)
	    sDestInfo.eContainer	= UOC_IMMEDIA_ITEM;

	else if (DesPos.nPlace == pos_dazao)
	{
		sDestInfo.Region.IsRongqi=0;
	    sDestInfo.eContainer	= UOC_ITEM_DAZAO;
	}
	else if (DesPos.nPlace == pos_cailiao)
	{
		sDestInfo.Region.IsRongqi=1;
	    sDestInfo.eContainer	= UOC_ITEM_CAILIAO;
	}
	else if (DesPos.nPlace == pos_equiproom)
	    sDestInfo.eContainer = UOC_ITEM_TAKE_WITH;
	else if (DesPos.nPlace == pos_repositoryroom)
		sDestInfo.eContainer = UOC_STORE_BOX;
	else if (DesPos.nPlace == pos_exbox1room)
		sDestInfo.eContainer = UOC_EX_BOX1;
	else if (DesPos.nPlace == pos_exbox2room)
		sDestInfo.eContainer = UOC_EX_BOX2;
	else if (DesPos.nPlace == pos_exbox3room)
		sDestInfo.eContainer = UOC_EX_BOX3;
	else if (DesPos.nPlace == pos_equiproomex)
		sDestInfo.eContainer = UOC_ITEM_EX;
	else if (DesPos.nPlace == pos_give)
		sDestInfo.eContainer = UOC_ITEM_GIVE;
	else if (DesPos.nPlace == pos_traderoom)
		sDestInfo.eContainer = UOC_TO_BE_TRADE;

	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sSrcInfo,0);  //源容器消除图标
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sDestInfo,1); //目标容器增加图标

	return bMove;
}

//客户端显示公告消息
void	KItemList::ClientShowMsg(const char *Msg)
{
	// CCMessageBox(Msg,"ClientShowMsg");
	KSystemMessage	sMsg;
	ZeroMemory(&sMsg,sizeof(sMsg));
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.eType         = SMT_NORMAL;
	sMsg.byParamSize   = 0;
	sMsg.byPriority    = 0;

	if (sizeof(Msg)<125)
	  t_sprintf(sMsg.szMessage,Msg);
	else
	  t_sprintf(sMsg.szMessage,"警告:信息过长,取消发送");

	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg,0);
}

//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
void	KItemList::MenuSetMouseItem()
{
	KUiObjAtContRegion	sInfo;
	if (!m_Hand)
	{
		CoreDataChanged(GDCNI_HOLD_OBJECT, 0, 0);
	}
	else
	{
		sInfo.Obj.uGenre    = CGOG_ITEM;
		sInfo.Obj.uId       = m_Hand;
		sInfo.Region.Width  = Item[m_Hand].GetWidth();
		sInfo.Region.Height = Item[m_Hand].GetHeight();
		sInfo.Region.h      = 0;
		sInfo.Region.v      = 0;
		sInfo.eContainer    = UOC_IN_HAND;		
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&sInfo, 0);
	}
}

PlayerItem* KItemList::GetFirstItem()
{
	m_nListCurIdx = m_UseIdxItemList.GetNext(0);
	return &m_Items[m_nListCurIdx];
}

PlayerItem* KItemList::GetNextItem()
{
	if ( !m_nListCurIdx )
		return NULL;
	m_nListCurIdx = m_UseIdxItemList.GetNext(m_nListCurIdx);
	return &m_Items[m_nListCurIdx];
}

void	KItemList::ClearRoom(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].Clear();
}


void	KItemList::BackupTrade()
{
	if ( !m_Room[room_tradeback].m_pArray )
		m_Room[room_tradeback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_tradeback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);

	m_nBackHand = m_Hand;
}

void	KItemList::RecoverTrade()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_tradeback].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);
	int nIdx = 0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		if (m_Items[nIdx].nPlace == pos_trade1)   //删除对方的
		{
			Remove(m_Items[nIdx].nIdx);
			//ItemSet.Remove(m_Items[nIdx].nIdx);
		}
	}
/*	for (int i = 0; i < MAX_PLAYER_ITEM; ++i)
	{
		if (m_Items[i].nIdx && m_Items[i].nPlace == pos_trade1)
			Remove(m_Items[i].nIdx);
	}*/
	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}
//开始交易时 清楚所有交易框的 东西
void	KItemList::StartTrade()
{
	BackupTrade();
	ClearRoom(room_trade);
	ClearRoom(room_trade1);
}

/*!*****************************************************************************
// Function		: KItemList::RemoveAll
// Purpose		: 退出时清除所有的装备
// Return		: void
// Comments		: 会实际地从游戏世界中的道具数组中去掉
// Author		: Spe
*****************************************************************************/
void KItemList::RemoveAll()
{	
   //删除物品前生成一个数据包,用于存档
/*#ifdef _SERVER
	Player[m_PlayerIdx].Save(TRUE);
   //------
#endif*/

	int nIdx = m_UseIdxItemList.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdxItemList.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;
		Remove(m_Items[nIdx].nIdx,0,0,0,FALSE);//只删除，不生成存档包
		nIdx = nIdx1;
	}
}

int KItemList::GetWeaponParticular()
{
	if (m_EquipItem[itempart_weapon])
	{
        if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==0  && Item[m_EquipItem[itempart_weapon]].GetParticular()<=6)
		{//武器
		    return Item[m_EquipItem[itempart_weapon]].GetParticular();
		}
		else if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==0)
		{//武器
            return Item[m_EquipItem[itempart_weapon]].GetParticular2();
		}

		if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==1  && Item[m_EquipItem[itempart_weapon]].GetParticular()<=2)
		{//暗器
			return Item[m_EquipItem[itempart_weapon]].GetParticular();
		}
		else if (Item[m_EquipItem[itempart_weapon]].GetDetailType()==1)
		{//暗器
            return Item[m_EquipItem[itempart_weapon]].GetParticular2();
		}
	}

	return -1;
}

//#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：判断一定长宽的物品能否放进物品栏 (为了服务器效率，本函数里面没有调用其他函数)
//--------------------------------------------------------------------------
BOOL	KItemList::CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnX, int *pnY,int nRoomType)
{
	if (nWidth <= 0 || nHeight <= 0 || !pnX || !pnY)
		return FALSE;

	if (!m_Room[nRoomType].m_pArray)
	   return FALSE;
	POINT pPos;
	if (m_Room[nRoomType].FindRoom(nWidth,nHeight,&pPos))
	{
		*pnX = pPos.x;
		*pnY = pPos.y;
		return true;
	}
	return false;
	/*if (m_Room[nRoomType].FindEmptyPlace(nWidth,nHeight,&pPos)) //查找空位置
	{
		*pnX = pPos.x;
		*pnY = pPos.y;
		return true;
	}
	return false;*/
	/*LPINT	pnTempRoom;
	int		i, j, a, b, nNext;

	pnTempRoom = m_Room[nRoomType].m_pArray; //容器的面积=（长度*高度） 单位：格子

	for (i = 0; i < m_Room[nRoomType].m_nHeight - nHeight + 1; i++)  //容器剩下的高度 （有几个格子）EQUIPMENT_ROOM_HEIGHT
	{
		for (j = 0; j < m_Room[nRoomType].m_nWidth - nWidth + 1; j++) //容器剩下的长度 （有几个格子）EQUIPMENT_ROOM_WIDTH
		{
			nNext = 0;
			for (a = 0; a < nHeight; a++) //物品占用几个格子
			{
				for (b = 0; b < nWidth; b++) 
				{
					if (pnTempRoom[(i + a) * m_Room[nRoomType].m_nWidth + j + b])  //如果有东西  EQUIPMENT_ROOM_WIDTH
					{
						nNext = 1;
						break;
					}
				}
				if (nNext)
					break;
			}

			if (!nNext)
			{
				*pnX = j;
				*pnY = i;
				return TRUE;
			}
		}
	}

	return FALSE;*/
}
//#endif

//------------------------------------------------------------------------------
//	功能：在room_equipment中查找指定Genre和DetailType的物品，得到ItemIdx和位置
//------------------------------------------------------------------------------
BOOL	KItemList::FindSameDetailTypeInEquipment(int nGenre, int nDetail, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_equipment].FindSameDetailType(nGenre, nDetail, pnIdx, pnX, pnY);
}
//删除某个容器的所有东西
//#ifndef	_SERVER
void	KItemList::RemoveAllInOneRoom(int nRoom)
{
	if (nRoom < 0 || nRoom >= room_num)
		return;

	int	nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[nRoom].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;

		//_ASSERT(nPos < m_Room[nRoom].m_nWidth * m_Room[nRoom].m_nHeight);
		if (nPos >= m_Room[nRoom].m_nWidth * m_Room[nRoom].m_nHeight)
			break;

		Remove(nItemIdx);
		//ItemSet.Remove(nItemIdx);
		nPos++;
	}
}
//#endif
//客户端锁着了
void KItemList::LockOperation()
{
	if (IsLockOperation())
	{//如果是锁着的就 返回了
		//_ASSERT(0);
		return;
	}
	m_bLockOperation = TRUE;
}
//客户端解锁
void KItemList::UnlockOperation()
{
	if (!IsLockOperation())
	{
		return;
	}
	m_bLockOperation = FALSE;
}

int KItemList::GetActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; ++i)
	{
		if (nIdx == m_EquipItem[i])
		{
			return GetEquipEnhance(i);
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------
//	功能：
//-------------------------------------------------------------------------------
int		KItemList::GetSameDetailItemNum(int nImmediatePos)
{
	if (nImmediatePos < 0 || nImmediatePos >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return 0;
	int	nIdx = m_Room[room_immediacy].FindItem(nImmediatePos, 0);
	if (nIdx <= 0)
		return 0;
	return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType()) + Item[nIdx].GetStackNum();
}

int	KItemList::GetImmediacyItemIndex(int nImmediatePos)
{
	if (nImmediatePos < 0 || nImmediatePos >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return 0;
	int	nIdx = m_Room[room_immediacy].FindItem(nImmediatePos, 0);

	if (nIdx <= 0 || nIdx>=MAX_ITEM)
		return 0;
	//Item[nIdx].GetGenre();
	return nIdx;
}

//-------------------------------------------------------------------------------
//	功能：获取包袱的同种物品的 叠加数量
//-------------------------------------------------------------------------------
int		KItemList::GetSameEquipmentItemNum(int nIdx)
{
	//if (nImmediatePos < 0 || nImmediatePos >= EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
	//	return 0;
	//int		nIdx = m_Room[room_equipment].FindItem(nImmediatePos, 0);
	if (nIdx <= 0 || nIdx>=MAX_ITEM)
		return 0;
	return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType()) /*+ Item[nIdx].GetStackNum()*/;
}

//黄金套装激活
BOOL KItemList::GetIfActive()
{
	int nSet[9][2],bIfGet,nNum = 0;
	ZeroMemory(nSet,sizeof(nSet));
	int i;
	for (i = 0;i < itempart_num;++i)  
	{
		bIfGet = 0;
		if  (m_EquipItem[i]<= 0 || m_EquipItem[i]>=MAX_ITEM) continue;

	    if (Item[m_EquipItem[i]].GetGoldId() && Item[m_EquipItem[i]].GetIsPlatima() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo<=0)  //如果有部位是白金
		{//白金 并且 没有激活属性
			for (int j = 0;j < 9;++j)  //9件
			{
				if (nSet[j][0] == Item[m_EquipItem[i]].GetSet())  //所在套装
				{
					nSet[j][1]++;  //套装数量增加
					bIfGet = 1;   
					if (nSet[j][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
					{
						m_bActiveSet = TRUE;
						return TRUE;
					}
				}
			}
			
			if (!bIfGet)  // 不是套装的白金  套装数为0或1的 全部激活
			{
				nSet[nNum][0] = Item[m_EquipItem[i]].GetSet(); // 所在套装
				nSet[nNum][1] = 1;
				
				if (nSet[nNum][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
				{
					m_bActiveSet = TRUE;  //全身激活
					return TRUE;
				}
				nNum++;
			} 
		}
		else if (Item[m_EquipItem[i]].GetGoldId() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo<=0)  //如果有部位是黄金
		{
			for (int j = 0;j < 9;++j)  //9件
			{
				if (nSet[j][0] == Item[m_EquipItem[i]].GetSet())  //所在套装
				{
					nSet[j][1]++;  //套装数量增加
					bIfGet = 1;   
					if (nSet[j][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
					{
						m_bActiveSet = TRUE;
						return TRUE;
					}
				}
			}

			if (!bIfGet)  // 不是套装的黄金  套装数为0或1的 全部激活
			{
				nSet[nNum][0] = Item[m_EquipItem[i]].GetSet(); // 所在套装
				nSet[nNum][1] = 1;
				
				if (nSet[nNum][1] >= Item[m_EquipItem[i]].GetSetNum()) //套装数量
				{
					m_bActiveSet = TRUE;  //全身激活
					return TRUE;
				}
				nNum++;
			} 
		}
        else if (Item[m_EquipItem[i]].IsBlue() && /*(Item[m_EquipItem[i]].GetPoint()==7 || */Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo>0)
		{//是蓝装印鉴 或 其他部位的
			 int nIdx=itempart_yinjian;
			     g_GameSetTing.GetInteger("SYSTEM","EquipAllActive",itempart_yinjian,&nIdx);

			 if (i==nIdx)
			 {
				 m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			 }
			 else if (nIdx==-1)
			 {//其他部位的激活 
			     m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			 }
			              
		}
		else if (Item[m_EquipItem[i]].GetGoldId() && Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentAllJiHuo>0)
		{//黄金饰品
			int nIdx=itempart_shiping;
			g_GameSetTing.GetInteger("SYSTEM","GoldAllActive",itempart_shiping,&nIdx);
			
			if (i==nIdx)
			{
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if (nIdx==-1)
			{
			     m_bActiveSet = TRUE;  //全身激活
				 return TRUE;
			}
		}
	}

	/*for(i=53;i<78;++i)
	{     //其他方案
			if (Item[m_ItemBox[53]].IsBlue()&&Item[m_ItemBox[54]].IsBlue()&&Item[m_ItemBox[55]].IsBlue()&&Item[m_ItemBox[56]].IsBlue()&&Item[m_ItemBox[57]].IsBlue())
			{//金系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[58]].IsBlue()&&Item[m_ItemBox[59]].IsBlue()&&Item[m_ItemBox[60]].IsBlue()&&Item[m_ItemBox[61]].IsBlue()&&Item[m_ItemBox[62]].IsBlue())
			{//木系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[63]].IsBlue()&&Item[m_ItemBox[64]].IsBlue()&&Item[m_ItemBox[65]].IsBlue()&&Item[m_ItemBox[66]].IsBlue()&&Item[m_ItemBox[67]].IsBlue())
			{//水系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[68]].IsBlue()&&Item[m_ItemBox[69]].IsBlue()&&Item[m_ItemBox[70]].IsBlue()&&Item[m_ItemBox[71]].IsBlue()&&Item[m_ItemBox[72]].IsBlue())
			{//火系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
			else if	(Item[m_ItemBox[73]].IsBlue()&&Item[m_ItemBox[74]].IsBlue()&&Item[m_ItemBox[75]].IsBlue()&&Item[m_ItemBox[76]].IsBlue()&&Item[m_ItemBox[77]].IsBlue())
			{//土系
				m_bActiveSet = TRUE;  //全身激活
				return TRUE;
			}
	}*/

	m_bActiveSet = FALSE;  //设定不激活
	return FALSE;
}

int KItemList::GetGoldColor(int nSet,int nId )
{
	int i;
	for (i = 0;i < itempart_num;++i)
	{//装备着的
		if (Item[m_EquipItem[i]].GetSet() == nSet && Item[m_EquipItem[i]].GetSetId() == nId)
		{
			return 2;
		}
	}

	for (i = 0;i < MAX_PLAYER_ITEM;++i)
	{//所有东西
		if (Item[m_Items[i].nIdx].GetSet() == nSet && Item[m_Items[i].nIdx].GetSetId() == nId)
		{
			return 1;
		}
	}

	return 0;
}

void KItemList::CheckBianShiItemTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime(&rawtime);   //今天的时间
for (int i = 0;i < MAX_PLAYER_ITEM;++i)
{
 if (Item[m_Items[i].nIdx].GetIsWhere()>0)
 {//是辨识装备
	if (Item[m_Items[i].nIdx].GetTime()->bYear)
		if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		{
			if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
			{
				Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
				//SynPlayerItem(m_Items[i].nIdx);
				if (m_Items[i].nPlace == pos_equip)
				{//卸载这个装备
					TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
				} 
			}
		}
		else if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			if (Item[m_Items[i].nIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			{
				if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
				{
					Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 
					//SynPlayerItem(m_Items[i].nIdx);
					if (m_Items[i].nPlace == pos_equip)
					{//卸载这个装备
						TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
					} 
				}
			}
			else if (Item[m_Items[i].nIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				if (Item[m_Items[i].nIdx].GetTime()->bDay < timeinfo->tm_mday)
				{
					if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
					{
						Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
						//SynPlayerItem(m_Items[i].nIdx);
						if (m_Items[i].nPlace == pos_equip)
						{//卸载这个装备
							TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
						} 
					}
				}
				else if (Item[m_Items[i].nIdx].GetTime()->bDay == timeinfo->tm_mday)
					if (Item[m_Items[i].nIdx].GetTime()->bHour < timeinfo->tm_hour)
					{
						if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
						{
							Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用
							//SynPlayerItem(m_Items[i].nIdx);
							if (m_Items[i].nPlace == pos_equip)
							{//卸载这个装备
								TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
							} 
						}
					}
					else if (Item[m_Items[i].nIdx].GetTime()->bHour == timeinfo->tm_hour) 
						if (Item[m_Items[i].nIdx].GetTime()->bMin <= timeinfo->tm_min)
						{
							if (Item[m_Items[i].nIdx].GetIsCanUse()==1)
							{
								Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用	
								//SynPlayerItem(m_Items[i].nIdx);
								if (m_Items[i].nPlace == pos_equip)
								{//卸载这个装备
									TimeUnEquip(m_Items[i].nIdx,m_Items[i].nX);
								} 
							}
						}							
						//-------------------------------------同步到客户端
	} 
//-------------------------------------所有装备持久为零的
   if (Item[m_Items[i].nIdx].GetIsCanUse()==1 && Item[m_Items[i].nIdx].GetDurability()>0 && Item[m_Items[i].nIdx].GetDurability()<=3)  
   {//
		   Item[m_Items[i].nIdx].SetIsCanUse(0);    //设置不能使用 	
		   //SynPlayerItem(m_Items[i].nIdx);
		   if (m_Items[i].nPlace == pos_equip)
		   {//卸载这个装备
			  // tempUnEquip(m_Items[i].nIdx,m_Items[i].nX);
		   } 	   
   }
//-------------------------------------- 
}
}

// mat na
void KItemList::SetMaskLock( BOOL bFlag )
{
	m_nMaskLock = bFlag;
}
//身上是否有这个相同属性物品
void KItemList::BackupGive()
{//备份包袱的东西
	if (!m_Room[room_giveback].m_pArray)
		m_Room[room_giveback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_giveback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);
	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_nBackHand = m_Hand;
}

void KItemList::RecoverGive()
{//还原备份
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_giveback].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);
	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}

void KItemList::StartGive()
{
	BackupGive();
	ClearRoom(room_give);
}

//-----检查有多少件相同等级类型的装备--------
int	KItemList::GetItemNum(int nGenre, int nDetailType, int nParticular, int nLevel,ITEM_POSITION nRongType)
{
	int	nNo = 0,nIdx = 0;
	while ((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		{	
				if (m_Items[nIdx].nPlace == nRongType && nGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
					nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
					nParticular == Item[m_Items[nIdx].nIdx].GetParticular() &&
					nLevel      == Item[m_Items[nIdx].nIdx].GetLevel())
						++nNo;
		}
	}
	return nNo;
}

//获取所有东西的数量
int KItemList::GetItemCount()
{//检测蓝装
	int nIdx = 0;
	int nCount=0;
	while((nIdx = m_UseIdxItemList.GetNext(nIdx)))
	{
		++nCount;
	}
	return nCount;
}
