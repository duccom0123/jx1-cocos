#ifndef	KBuySellH
#define	KBuySellH

#define	BUY_SELL_SCALE		4

class KItem;
//商店商品产生结构
typedef struct
{
	int		nGenre;
	int		nDetailType;
	int		nParticularType;
	int		nSeriesReq;
	int		nLevel;
	int		nPrice;
	int		nPriceXu;
} ItemGenerate; 

class KInventory;

class KBuySell
{
public:
	KBuySell();
	~KBuySell();
	BOOL			Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	// int				GetWidth() { return 1; }
	// int				GetHeight() { return 1; }
	KItem*			GetItemidx(int nIndex);
	int				GetItemIndex(int nShop, int nIndex);
private:
	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_MaxItem;
public:
	//客户端打开商城
	KInventory*		m_pShopRoom;
	KInventory*		m_pSShopRoom;
	//typedef std::map<int, KItem*> _mItemPtr;
	//_mItemPtr mItemPtr;
	void			OpenCSale(int nShop);
	void 			OpenCMarket(int nShop);
	void			PaintItem(int nIdx, int nX, int nY);
	void            PaintItemPai(int nIdx, int nX, int nY,int nKind=-1);
	void            OpenCSuPerShop(int nSaleType, BuySellInfo *pInfo);
	void            SetShopItemInfo(BYTE* pMsg );
	void            SetShopItemdata(BYTE* pMsg );
	//------------------------------------------
	KItem*          GetItemDataByidx(int nIndex);
private:
	KItem*			m_paiItem;
};

extern KBuySell	BuySell;
#endif
