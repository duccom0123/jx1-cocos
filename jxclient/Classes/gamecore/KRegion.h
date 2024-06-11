#ifndef	KRegionH
#define	KRegionH

//-----------------------------------------------------------------------
#include "engine/KEngine.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KObj.h"
//#include "KWorldMsg.h"
//后来加的
//#include "Scene/SceneDataDef.h"
#include <vector>
#include <list>
#include <map>
//-----------------------------------------------------------------------
#define	MAX_REGION		    9
#define	REGION_GRID_WIDTH	16
#define	REGION_GRID_HEIGHT	32
#define	REGION_WIDTH		16
#define	REGION_HEIGHT		32

/*enum MOVE_OBJ_KIND
{
	obj_npc,
	obj_object,
	//obj_missle,
}; */
class  KRegion
{
	friend class KSubWorld;
public:
	typedef std::vector<INT> VEC_LIST;
	int			m_nIndex;							// 地图索引
	int			m_nSubMapidx;
	int			m_RegionID;
	KList		m_NpcList;							// 人物列表
	KList		m_ObjList;							// 物件列表
	//VEC_LIST	m_ObjList;							// 物件列表
	KList		m_MissleList;						// 子弹列表
	KList		m_PlayerList;						// 玩家列表
	int			m_nConnectRegion[8];				// 相邻的地图索引
	int			m_nConRegionID[8];					// 相邻的地图ID
	KRegion*	m_pConRegion[8];					// 相邻的Region
	int			m_nRegionX;							// 在世界中的位置X（象素点）
	int			m_nRegionY;							// 在世界中的位置Y（象素点）
	int			m_nWidth;
	int			m_nHeight;
private:
//--------------------------------------------------------------------------
	enum _KEACT_TYPE
	{
		emACT_TYPE_NONE	= 0,	// 一切活动终止
		emACT_TYPE_WAIT,		// 等待终止，可能尚有Missile、Obj未消失
		emACT_TYPE_ALL,			// 全部活动，周围有玩家
	};

	class _KGridData
	{
	public:
		_KGridData()
		{
			//ZeroStruct(m_adwDataFlag);
			memset(&m_adwDataFlag,0,sizeof(m_adwDataFlag));
		}
		
		BOOL HasData(INT nX, INT nY) const
		{
			return (m_adwDataFlag[nX] & (1 << nY)) != 0;
		}
		INT GetData(INT nX, INT nY) const
		{
			INT nResult	= 0;
			//KGLOG_PROCESS_ERROR(nX >= 0 && nX < REGION_WIDTH);
			//KGLOG_PROCESS_ERROR(nY >= 0 && nY < REGION_HEIGHT);
			if (nX<0 || nX>=REGION_WIDTH) 
				goto Exit0;
			if (nY<0 || nY>=REGION_HEIGHT) 
				goto Exit0;

			if (m_adwDataFlag[nX] & (1 << nY))
			{
				nResult	= m_mapData.find(MAKELONG(nX, nY))->second;
			}
Exit0:
			return nResult;
		}
		BOOL SetData(INT nX, INT nY, INT nData)
		{
			DWORD dwId	= MAKELONG(nX, nY);
			//KGLOG_PROCESS_ERROR(nX >= 0 && nX < REGION_WIDTH);
			//KGLOG_PROCESS_ERROR(nY >= 0 && nY < REGION_HEIGHT);
			if (nX<0 || nX>=REGION_WIDTH) 
				goto Exit0;
			if (nY<0 || nY>=REGION_HEIGHT) 
				goto Exit0;

			if (nData)
			{
				m_adwDataFlag[nX] |= 1 << nY;
				m_mapData[dwId]	= nData;
			}
			else
			{
				m_adwDataFlag[nX] &= ~(1 << nY);
				m_mapData.erase(dwId);
			}
Exit0:
			return TRUE;
		}
		BOOL IncData(INT nX, INT nY)
		{
			DWORD dwId	= MAKELONG(nX, nY);
			//KGLOG_PROCESS_ERROR(nX >= 0 && nX < REGION_WIDTH);
			//KGLOG_PROCESS_ERROR(nY >= 0 && nY < REGION_HEIGHT);
			if (nX<0 || nX>=REGION_WIDTH) 
			  goto Exit0;
			if (nY<0 || nY>=REGION_HEIGHT) 
				goto Exit0;
			
			if (m_adwDataFlag[nX] & (1 << nY))	// 已经有的，+1
			{
				m_mapData[dwId]++;  
			}
			else								// 新的，直接置1
			{
				m_adwDataFlag[nX] |= 1 << nY;
				m_mapData[dwId]	= 1;
			}
Exit0:
			return TRUE;
		}
		BOOL DecData(INT nX, INT nY)
		{
			BOOL bResult	= FALSE;
			DWORD dwId		= MAKELONG(nX, nY);
			//KGLOG_PROCESS_ERROR(nX >= 0 && nX < REGION_WIDTH);
			//KGLOG_PROCESS_ERROR(nY >= 0 && nY < REGION_HEIGHT);
			if (nX<0 || nX>=REGION_WIDTH) 
				goto Exit0;
			if (nY<0 || nY>=REGION_HEIGHT) 
				goto Exit0;

			//KGLOG_PROCESS_ERROR(m_adwDataFlag[nX] & (1 << nY));
			if (m_adwDataFlag[nX] & (1 << nY))
			{
				if (--m_mapData[dwId] == 0)	// 减为0了
				{
					m_mapData.erase(dwId);
					m_adwDataFlag[nX] &= ~(1 << nY);
				} 
			}		
Exit0:
			return bResult;
		}
		BOOL Clear()
		{
			//ZeroStruct(m_adwDataFlag);
			memset(&m_adwDataFlag,0,sizeof(m_adwDataFlag));
			//m_mapData::iterator it;
			//for(it = m_mapData.begin(); it != m_mapData.end(); ++it)
			//   m_mapData.erase(it);
			m_mapData.clear();
			return TRUE;
		}	
	private:
		DWORD m_adwDataFlag[REGION_WIDTH];	// 用位来标记某点是否有数据（刚好REGION_HEIGHT == 32，用一个DWORD）
		typedef std::map<DWORD, INT> _mapData;
		_mapData m_mapData;
	};	
//---------------------------------------------------------
	int			m_nNpcSyncCounter;					// 同步计数器
	//int		m_nLoopCounter;					// 同步计数器
	int			m_nObjSyncCounter;
	int			m_nActive;							// 是否激活（是否有玩家在附近）
	//BYTE*		m_pNpcRef;							// 每个格子上的NPC数目
	//BYTE*		m_pObjRef;							// 每个格子上的OBJ数目
	//BYTE*		m_pMslRef;							// 每个格子上的MISSLE数目
	_KGridData	m_cNpcRef;						    // 每个格子上的NPC数目
	_KGridData	m_cObjRef;						    // 每个格子上的OBJ数目
public:
	KRegion();
	~KRegion();
	BOOL		Init(int nWidth, int nHeight);
	BOOL		Load(int nX, int nY);

	// 载入客户端地图上本region 的 object数据（包括npc、box等）
	// 如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
	BOOL		LoadObjectC(int nSubWorld, int nX, int nY, char *lpszPath);
	// 载入客户端地图上本 region 的 clientonlynpc 数据
	BOOL		LoadClientNpc(KPakFile *pFile, DWORD dwDataSize);
	// 载入客户端地图上本 region 的 clientonlyobj 数据
	BOOL		LoadClientObj(KPakFile *pFile, DWORD dwDataSize);
	// 载入障碍数据给小地图
	static void	LoadLittleMapData(int nX, int nY, char *lpszPath, INT *lpbtObstacle);

	static void GetFliePath(char* nName,char * nPath,int nMaxRows);

	void		Close();
	void		Activate(int mSubWorldID);
	void		NewRegActivate();
	void        ActivateObj(int mSubWorldID);
	BYTE		GetBarrier(int MapX, int MapY, int nDx, int nDy);	//	地图高度
	BYTE        GetNewBarrier(int nMapX, int nMapY, int nDx, int nDy,BOOL nIsCheckNpc=FALSE);
	// 按 像素点坐标 * 1024 的精度判断某个位置是否障碍
	// 参数：nGridX nGirdY ：本region格子坐标
	// 参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
	// 参数：bCheckNpc ：是否判断npc形成的障碍
	// 返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
	BYTE		GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);
	BYTE	    GetBarrierNewMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);

	DWORD		GetTrap(int MapX, int MapY);						//	得到Trap编号
    BOOL		SetTrap(int MapX, int MapY,int nCellNum=1,DWORD uTrapScriptId=0);

	inline BOOL	IsActiveAll()
	{
		return TRUE;
	}

	inline BOOL	IsActive() 
	{
		return TRUE;
	};
 
    BOOL	HasNpc(INT nMapX, INT nMapY) const { return m_cNpcRef.HasData(nMapX, nMapY); }
	INT		GetNpcRef(INT nMapX, INT nMapY) const { return m_cNpcRef.HasData(nMapX, nMapY); }
	BOOL	AddNpcRef(INT nMapX, INT nMapY) { return m_cNpcRef.IncData(nMapX, nMapY); }
	BOOL	DecNpcRef(INT nMapX, INT nMapY) { return m_cNpcRef.DecData(nMapX, nMapY); }

	BOOL	HasObj(INT nMapX, INT nMapY) const { return m_cObjRef.HasData(nMapX, nMapY); }
	INT		GetObjRef(INT nMapX, INT nMapY) const { return m_cObjRef.GetData(nMapX, nMapY); }
	BOOL	AddObjRef(INT nMapX, INT nMapY) { return m_cObjRef.IncData(nMapX, nMapY); }
	BOOL	DecObjRef(INT nMapX, INT nMapY) { return m_cObjRef.DecData(nMapX, nMapY); }
 /*
	int			GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL		AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL		DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
   */
	int			FindNpc(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType=0);
	int			CheckNpcInDisten(int nScyNpcIdx, int nTarNpcIdx,int nDis);
	int	        FindNpcAuto(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType=0,int nDis=0);
	//int		FindEquip(int nMapX, int nMapY);
	int			FindObjectAuto(int nMapX, int nMapY,int nNpcIdx=0,void *pData=NULL,int *m_CurState=0);
	int			FindObject(int nObjID);
	void*		GetObjNode(int nIdx);
	//VEC_LIST::iterator	GetObjNode(INT nIdx);
	int			SearchNpc(DWORD dwNpcID);		// 寻找本区域内是否有某个指定 id 的 npc (zroc add)

	void		Paint();
	void		AddNpc(int nIdx);
	void		RemoveNpc(int nIdx);
	//void	    RemoveNpcwor(int nIdx);
	void		AddMissle(int nIdx);
	void		RemoveMissle(int nIdx);
	void		AddObj(int nIdx);
	void		RemoveObj(int nIdx);
	BOOL		AddPlayer(int nIdx);
	BOOL		RemovePlayer(int nIdx);

};

inline int KRegion::CheckNpcInDisten(int nScyNpcIdx, int nTarNpcIdx,int nDis)
{
	if  (nScyNpcIdx<=0 || nTarNpcIdx<=0)
		return FALSE;

	int nScyX,nScyY,nTarX,nTarY,nScymap,nTarmap;
	Npc[nScyNpcIdx].GetMpsPos(&nScyX, &nScyY,&nScymap);
	Npc[nTarNpcIdx].GetMpsPos(&nTarX, &nTarY,&nTarmap);

	if (nScymap!=nTarmap)
		return FALSE;

    int nCurDis=0;
	nCurDis = (int)sqrt(double((nScyX - nTarX)) * (nScyX - nTarX) + (nScyY - nTarY) * (nScyY - nTarY));

	if  (nCurDis<=nDis)
		return TRUE;
	else
        return FALSE;
}
//--------------------------------------------------------------------------
//	Find Npc
//--------------------------------------------------------------------------
inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType)
{
	//if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0) 
	//	return 0;
	if (GetNpcRef(nMapX,nMapY)<=0)
		return 0;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
          if (nType==0)
		  {
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation) //位移对比 是敌人的话
			{
               return pNode->m_nIndex;  
			}
		  }
		  else if (nType==1)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_NpcSettingIdx) 
			  {
                     return pNode->m_nIndex;
			  }
		  }
		  else if (nType==2)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_dwID) 
			  {
                     return pNode->m_nIndex;
			  }
		  }

		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

//挂机的使用的查找NPC
inline int KRegion::FindNpcAuto(int nMapX, int nMapY, int nNpcIdx, DWORD nRelation,int nType,int nDis)
{
	//if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
	//	return 0;
	if (GetNpcRef(nMapX,nMapY)<=0)
		return 0;

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
          if (nType==0)
		  {
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation) //位移对比 是敌人的话
			{
			       if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				   {
					   if  (nDis>0)
					   {
						   if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
							   return pNode->m_nIndex;
					   }
					   else
                           return pNode->m_nIndex;  
				   }
			}
		  }
		  else if (nType==1)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_NpcSettingIdx) 
			  {
                    if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
					{
						if  (nDis>0)
						{
							if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
								return pNode->m_nIndex;
						}
						else
							return pNode->m_nIndex;  
					}

			  }
		  }
		  else if (nType==2)
		  {
			  if (nRelation==Npc[pNode->m_nIndex].m_dwID) 
			  {
                     if (Npc[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
					 {
						 if  (nDis>0)
						 {
							 if (CheckNpcInDisten(nNpcIdx,pNode->m_nIndex,nDis))
								 return pNode->m_nIndex;
						 }
						 else
							 return pNode->m_nIndex;  
					 }
			  }
		  }
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}


inline int KRegion::FindObjectAuto(int nMapX, int nMapY,int nNpcIdx,void *pData,int *m_CurState)
{
	if  (!pData) return 0;
	gameAutoplaerInfo *pAutoInfo = (gameAutoplaerInfo *)pData;
	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_ObjList.GetHead();
	char nTempItemName[128];
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
		{
			if (Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
			{//没有取消拾取的　 
				//t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
				//if (nItemName && strstr(nTempItemName,nItemName))
				//	return pNode->m_nIndex;
				//银两
				if  (pAutoInfo->isopenmoney && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money)
				{//如果开启拾取钱
					*m_CurState = 3;//PICCK_ITEM_MONEY;
					return pNode->m_nIndex;
				}
				//道具
				if  (pAutoInfo->isopendaoju && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
				{//开始循环检测　是否在列表内　如果在返回 Npc[m_Index].m_AutoplayId==1
				   if (Npc[nNpcIdx].m_AutoplayId==1)
				   {//挂机状态
					  char _nItemName[64]={0};
					  for(int i=0;i<16;++i)
					  { 	
						  nToolItem.GetString(i+1,1,"无数据",_nItemName,sizeof(_nItemName)); 
						  t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
						  if (strstr(nTempItemName,_nItemName))
						  {
							*m_CurState = 5;//PICCK_TOOL;
							return pNode->m_nIndex;
						  }
					  }
				   }
				   else
				   {//手动拾取的
					  if (Object[pNode->m_nIndex].m_nColorID==8)
					  {
					    *m_CurState = 5;//PICCK_TOOL;
					    return pNode->m_nIndex;
					  }
				   }
				}
				//装备
				if  (pAutoInfo->insopenzhuang && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
				{//开始循环检测　是否在列表内　如果在返回
					if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
					{
						*m_CurState = 2;//PICCK_ITEM;
						return pNode->m_nIndex;
					}
				}
				//装备和钱
				if  (pAutoInfo->ismonyeandzhuang)
				{//开始循环检测　是否在列表内　如果在返回
					if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item &&(Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4))
					{
						*m_CurState = 2;//PICCK_ITEM;
						return pNode->m_nIndex;
					}

					if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money)
					{//如果开启拾取钱
						*m_CurState = 3;//PICCK_ITEM_MONEY;
						return pNode->m_nIndex;
					}
				}
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	*m_CurState = 0;
	return 0;

//----------------------------------------------------
	/*if (nKind==-1)
	{ 
		while(pNode)
		{//直接检查名称
			if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
			{
				if (Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				{	 
					t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
					if (nItemName && strstr(nTempItemName,nItemName))
						return pNode->m_nIndex;
				}
			}
			pNode = (KIndexNode *)pNode->GetNext();
		}
		return 0;

	}
	else
	{
		while(pNode)
		{  
			if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
			{
				if (nKind==1 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
					return pNode->m_nIndex;
				else if (nKind==2 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				{//有属性的装备
					if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
						return pNode->m_nIndex;
				}
				else  if (nKind==3 && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				{
					if  (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money)
						return pNode->m_nIndex;
					else if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
					{
						if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
							return pNode->m_nIndex;
					}
				}
				else if (nKind==4 && nItemName && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
				{//图谱
					t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
					if (strstr(nTempItemName,nItemName))
						return pNode->m_nIndex;
				}	      
			}
			pNode = (KIndexNode *)pNode->GetNext();
		}
		return 0;

	}
	return 0;
	*/
}

/*
inline int KRegion::FindObjectAuto(int nMapX, int nMapY,int nKind,char *nItemName,int nNpcIdx,void *pData,int *m_CurState)
{
KIndexNode *pNode = NULL;
pNode = (KIndexNode *)m_ObjList.GetHead();
char nTempItemName[128];		
if (nKind==-1)
{ 
while(pNode)
{
if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
{
if (Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
{	 
t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
if (nItemName && strstr(nTempItemName,nItemName))
return pNode->m_nIndex;
}
}
pNode = (KIndexNode *)pNode->GetNext();
}
return 0;

}
else
{
while(pNode)
{  
if (Object[pNode->m_nIndex].m_nMapX == nMapX && Object[pNode->m_nIndex].m_nMapY == nMapY)
{
if (nKind==1 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
return pNode->m_nIndex;
else if (nKind==2 && Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
{
if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
return pNode->m_nIndex;
}
else  if (nKind==3 && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
{
if  (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Money)
return pNode->m_nIndex;
else if (Object[pNode->m_nIndex].m_nKind == Obj_Kind_Item)
{
if (Object[pNode->m_nIndex].m_nColorID==1 || Object[pNode->m_nIndex].m_nColorID==3 || Object[pNode->m_nIndex].m_nColorID==4)
return pNode->m_nIndex;
}
}
else if (nKind==4 && nItemName && Object[pNode->m_nIndex].m_AttackerDwid!=Npc[nNpcIdx].m_dwID)
{//图谱
t_sprintf(nTempItemName,UTEXT(Object[pNode->m_nIndex].m_szName,1).c_str());
if (strstr(nTempItemName,nItemName))
return pNode->m_nIndex;
}	      
}
pNode = (KIndexNode *)pNode->GetNext();
}
return 0;

}
return 0;
}

*/
#endif
