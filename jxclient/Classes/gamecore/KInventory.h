#ifndef KInventoryH
#define	KInventoryH

class  KInventory
{
	friend	class	KItemList;

private:
	int*	m_pArray;
	int		m_nMoney;
	int		m_nWidth;  //容器宽度
	int		m_nHeight; //容器高度
	int		m_nBakWidth;  //备份的容器宽度
	int		m_nBakHeight; //备份的容器高度
	int		m_nXu;

public:
	KInventory();
    ~KInventory();

	BOOL	Init(int nWidth, int nHeight);
	void	Release();
	void	Clear();
	BOOL	PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight);//检测idx是否与位置上的标示相同,否则设置为位置标示为0 返回真
	BOOL    SetRoomNull();
	BOOL    SetEmptyPlaceItemIdx(int nIdx, int nWidth, int nHeight,POINT *pPos);
	int		FindItem(int nX, int nY);
	BOOL	PlaceItem(int nXpos, int nYpos, int nIdx, int nWidth, int nHeight);//检测物品放进容器时，位置上是否有物品,否则设置该位置的标示为idx
	void    DirectSetPosition(int nIdx, int nX, int nY,int nWidth,int nHeight);
	
	BOOL	HoldItem(int nIdx, int nWidth, int nHeight);  //剩余容器空间是否能放下该物品
	BOOL	FindRoom(int nWidth, int nHeight, POINT* pPos);
	BOOL	CheckRoom(int nXpos, int nYpos, int nWidth, int nHeight);
	int     GetFreeRoomCount(int nWidth, int nHeight);
	BOOL	AddmClient(int nMoney);
	int		GetmServer() { return m_nMoney; }
	int     GetWidth() { return m_nWidth; }
	int     GetHeight() { return m_nHeight; }
	// int     GetWidth() { return 1; }
	// int     GetHeight() { return 1; }
	BOOL	SetClientMon(int nMon) { if (nMon < 0) return FALSE; m_nMoney = nMon; return TRUE; }
	//Xu
	BOOL	AddxCliet(int ni);
	int		GetxCLient() { return m_nXu; }
	BOOL	SetxClient(int ni) { if (ni < 0) return FALSE; m_nXu = ni; return TRUE; }
	////////////////////
	int		GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY);
	int		CalcSameDetailType(int nGenre, int nDetail);
	int		GetCurIndexItem(int nCurCount);
	BOOL	FindEmptyPlace(int nWidth, int nHeight, POINT *pPos);

	// 输入物品类型和具体类型，察看Inventory里面有没有相同的物品，输出位置和编号
	BOOL	FindSameDetailType(int nGenre, int nDetail, int *pnIdx, int *pnX, int *pnY);
	// 输入物品类型和具体类型，察看Inventory里面有没有相同的物品
	BOOL	CheckSameDetailType(int nGenre, int nDetail,int ParticularType);
};
#endif //KInventoryH
