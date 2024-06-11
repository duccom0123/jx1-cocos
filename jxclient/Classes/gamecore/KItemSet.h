#ifndef KItemSetH
#define	KItemSetH

#include "engine/KLinkArray.h"

#define		IN
#define		OUT
class	KItem;
//磨损种类
enum enumAbrade
{
	    enumAbradeAttack = 0,  //攻击磨损
		enumAbradeDefend,      //给攻击时候磨损
		enumAbradeMove,        //移动磨损
		enumAbradeEquip,       //换装磨损
		enumAbradeNum,
};
// 玩家之间交易进行时，用于判断玩家物品栏能否放下买进的物品
typedef struct
{
	int		m_nIdx;
	int		m_nX;
	int		m_nY;
	int		m_nWidth;
	int		m_nHeight;
} TRADE_ITEM_INFO;

typedef struct
{
	int		nPriceScale;
	int		nMagicScale;
	int		nLevelScale;
} REPAIR_ITEM_PARAM;

class KItemSet
{
private:
	DWORD			m_dwIDCreator;		        //  ID生成器，用于客户端与服务器端的交流
	KLinkArray		m_FreeIdxItemSet;			//	可用表
	KLinkArray		m_UseIdxItemSet;			//	已用表
	
public:
	int				m_nItemAbradeRate[enumAbradeNum][itempart_num];   //磨损概率
	REPAIR_ITEM_PARAM	m_sRepairParam;	
public:
	KItemSet();
	~KItemSet();
	void			Init();
	int				GetItemCount(IN int nItemGenre = -1);
	int				SearchID(IN DWORD dwID);
	int				Add(KItem* pItem);
	int				AddOther(IN int nItemGenre, IN int nSeries, IN int nLevel=1, IN int nLuck=0, IN int nDetail = -1, \
		                               IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0,\
							IN UINT nRandomSeed = 0,IN int nStackNum = 1, IN int nEnChance = 0,IN int nPoint = 0, \
						  IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int nMin = 0,\
						                                        IN int nRongpiont=0,IN int* pnRMagicLevel = NULL,\
						                        IN int* pnJBLevel = NULL,IN int* pnBsLevel=NULL,IN int IsWhere=0,IN int nIsLogin=FALSE); //蓝装 以及 蓝装熔炼
	int				AddCilentItem(IN int nItemGenre, IN int nSeries, IN int nLevel=1, IN int nLuck=0, IN int nDetail = -1,\
		                                    IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0, \
											       IN UINT nRandomSeed = 0,IN int nStackNum = 1, IN int nEnChance = 0,\
				            IN int nPoint = 0, IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,\
									                 IN int nMin = 0,IN int nRongpiont=0,IN int* pnRMagicLevel = NULL, \
									       IN int* pnJBLevel = NULL,IN int* pnMagType = NULL,IN int* pnMagVala = NULL,\
									                      IN int* pnMagValb = NULL,IN int* pnMagValc = NULL,IN int=0);
	
	int				AddGold(IN int nId , IN int* pnMagicLevel = NULL , IN int nSeries = 0, IN int nEnChance = 0,\
		                    IN int nRongpiont=0,IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,\
							IN int nMin = 0,IN int* pnRMagicLevel = NULL,IN int* pnBMagicLevel=NULL, IN int nLevel=1,\
							IN int nLuck=1, IN int nDetailType = -1, IN int nParticular = -1,IN int nStackNum = 1,IN int nVersion = 0,IN int nIsPlatina=0,IN BOOL nIsLogin=FALSE,IN UINT nRandomSeed = 0);//黄金的
	int				AddRondomGold(IN int nId , IN int* pnMagicLevel = NULL , IN int nSeries = 0, IN int nEnChance = 0,\
		                    IN int nRongpiont=0,IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,\
		                    IN int nMin = 0,IN int* pnRMagicLevel = NULL, IN int nLevel=1,\
		                    IN int nLuck=1, IN int nDetailType = -1, IN int nParticular = -1,IN int nStackNum = 1,IN int nVersion = 0,IN int nIsPlatina=0,IN BOOL nIsLogin=FALSE,IN UINT nRandomSeed = 0);//黄金的

    int             AddRong(IN int Goidid, IN int nSeries, IN int nLevel=1, IN int nLuck=0, IN int nDetailType = -1,\
		                      IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int* rnMagicLevel = NULL,\
					 int* BnMagicLevel = NULL,IN int nVersion = 0, IN UINT nRandomSeed = 0,IN int nStackNum = 1, \
							    IN int nEnChance = 0,IN int nRongID = 0, IN int nPoint = 0,IN int mIsDaZhao = 0, \
							IN KMagicAttrib *nItemMag=NULL,IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0,\
							IN int nHour = 0, IN int nMin = 0,IN int IsWhere=0,BOOL nIsLoggin=FALSE);
	void			Remove(IN int nIdx);
	int				GetAbradeRange(IN int nType, IN int nPart);
private:
	void			SetID(IN int nIdx);
	int				FindFreeItemSet();
};

extern KItemSet	ItemSet;
#endif

