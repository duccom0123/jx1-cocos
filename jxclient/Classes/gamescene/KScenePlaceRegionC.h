// ***************************************************************************************
// 场景地图的区域对象的类声明
// Copyright : Kingsoft 2002
// Author    : wooy(wu yue)
// CreateTime: 2002-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************

#ifndef KSCENEPLACEREGIONC_H_HEADER_INCLUDED_C22EB91D
#define KSCENEPLACEREGIONC_H_HEADER_INCLUDED_C22EB91D
#include "engine/KbugInfo.h"
#include "SceneDataDef.h"
#include "gamecore/KRegion.h"
#include "KIpotLeaf.h"
#include <map>

struct KIpotRuntimeObj;
struct KIpotBuildinObj;
struct KRUImage;
class KPakFile;

//##ModelId=3DB8F89F02D5
//##Documentation
//## 场景地图中的一个区域。（客户端版）
class KScenePlaceRegionC
{
  public:

	//##ModelId=3DD7E5A80351
	//##Documentation
	//## 区域对象运行时的一些参数(外部亦可引用)
	enum KREGION_WORK_PARAM_PUB { 
		//##Documentation
		//## 单个区域的横向宽度（单位：像点）
		RWPP_AREGION_WIDTH = 512, 
		//##Documentation
		//## 单个区域的纵向宽度（单位：像点）
		RWPP_AREGION_HEIGHT = 1024 };

  public:

	//##ModelId=3DD7EA9200B2
	KScenePlaceRegionC();

	//##ModelId=3DD7EA9200F8
	~KScenePlaceRegionC();
   
	//##ModelId=3DBDAC140299
	//##Documentation
	//## 指示区域对象应该加载的区域数据的区域索引坐标
	bool ToLoad(
		//##Documentation
		//## 区域的横向索引
		int nIndexX, 
		//##Documentation
		//## 区域的纵向索引
		int nIndexY);

	int  getStatus(){return m_Status;};
	//##ModelId=3DB901F101CD
	//##Documentation
	//## 载入区域信息。
	//## 函数过程参看[Use Case View\场景\客户端\单个区域的加载]。
	bool LoadRe(
		//##Documentation
		//## 指向存储区域信息的'区域目录'所在的父目录的名称的缓冲区。
		const char* pszBaseFolderName);


	//bool KScenePlaceRegionC::FindPathInit(const char* pszBaseFolderName,int nRegX,int nRegY);

	//##ModelId=3DD3DD2C0141
	//##Documentation
	//## 获取区域的索引坐标
	void GetRegionIndex(
		//##Documentation
		//## 横向索引
		int& nX, 
		//##Documentation
		//## 纵向索引
		int& nY) const;

	//##ModelId=3DBF876400B2
	//##Documentation
	//## 清空此对象的数据
	void Clear();

    void GetFliePath(char* nName,char * nPath,int nMaxRows=100);
   
	//##ModelId=3DBF9582039A
	//##Documentation
	//## 预渲染地表层图形
	bool PrerenderGround(bool bForce);
	unsigned int GetColor(LPCTSTR pString);


	//##ModelId=3DDBD8C80309
	//##Documentation
	//## 绘制场景区域地表层
	void PaintGround(int bRegionIndex,int fullorpart=1);
	void PaintBuildinObj(RECT* pRepresentArea,int bRegionIndex,bool isClear);
	void ReomeGroundObj(int bRegionIndex);
	void PaintGroundDirect(int bRegionIndex);
	void PaintGroundDirect_Part(int bRegionIndex);
	void PaintGroundDirect_full(int bRegionIndex);

	//##ModelId=3DE29E9102B0
	//##Documentation
	//## 获取高空对象最大的绘图次序编号。
	unsigned int GetAboveHeadLayer(
		//## 指向高空对象的列表
		KBuildinObj*& pObjsAboveHead);

	//##ModelId=3DE29F360221
	//##Documentation
	//## 绘制高空物体
	static void PaintAboveHeadObj(
		KBuildinObj* pObj,
		RECT* pRepresentArea,int nindex);

	//##ModelId=3DE33AB30318
	//##Documentation
	//## 获取区域内内建对象的列表。
	void GetBuildinObjs(
		//##Documentation
		//## 指向按点方式排序对象的列表
		KIpotBuildinObj*& pObjsPointList, 
		//##Documentation
		//## 按点方式排序对象的数目
		unsigned int& nNumObjsPoint, 
		//##Documentation
		//## 指向按线方式排序对象的列表
		KIpotBuildinObj*& pObjsLineList,
		//##Documentation
		//## 按线方式排序对象的数目
		unsigned int& nNumObjsLine, 
		//##Documentation
		//## 指向按树方式排序对象的列表
		KIpotBuildinObj*& pObjsTreeList, 
		//##Documentation
		//## 按树方式排序对象的数目
		unsigned int& nNumObjsTree);

	//## 获取区域内内建未初始化的对象的列表。
	void GetBIOSBuildinObjs(
		KBuildinObj*& pObjsList, 
		unsigned int& nNumObjs 
    );

	//获得内建光源的信息
	unsigned int GetBuildinLights(
		KBuildInLightInfo*& pLights);

	void SetNestRegion(KScenePlaceRegionC* pNest);
	
	void EnterProcessArea(KRUImage* pImage);
	void LeaveProcessArea();
	KRUImage* GetPrerenderGroundImage() { return m_pPrerenderGroundImg; }

	long GetObstacleInfo(int nX, int nY);
	long GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY);
	void SetHightLightSpecialObject(unsigned int uBioIndex);
	void UnsetHightLightSpecialObject(unsigned int uBioIndex);

//#ifdef SWORDONLINE_SHOW_DBUG_INFO
	void PaintObstacle();
	BOOL GetNextMapPos();

//#endif

  private:
	
	//##Documentation
	//## 区域对象运行时的一些参数(仅内部使用)
	//##ModelId=3DD7E4C101E9
	enum KREGION_WORK_PARAM
	{
		//##Documentation
		//## 地表横向划分格子的数目
		RWP_NUM_GROUND_CELL_H = 16, 
		//##Documentation
		//## 地表纵向划分格子的数目
		RWP_NUM_GROUND_CELL_V = 16,

		RWP_NEST_REGION_0 = 1,
		//##Documentation
		//## 默认地图格的横宽。
		//## 地图格指场景地图为作障碍等计算划分的最小格子单元。
		RWP_OBSTACLE_WIDTH = 32,
		//##Documentation
		//## 默认地图格的纵宽。
		//## 地图格指场景地图为作障碍等计算划分的最小格子单元。
		RWP_OBSTACLE_HEIGHT = 32,
	};

	//##ModelId=3DCE75E2003D
	//##Documentation
	//## 区域对象的状态枚举
	enum KREGION_STATUS { 
		//##Documentation
		//## 空闲无数据状态。
		REGION_S_IDLE,
		//##Documentation
		//## 等待执行加载
		REGION_S_TO_LOAD,
		//##Documentation
		//## 加载数据入中
		REGION_S_LOADING, 
		//##Documentation
		//## 加载数据完毕，可以被应用状态。
		REGION_S_STANDBY,
		//##Documentation
		//## 数据处理中
		REGION_S_PROCESSING };

	struct KGroundLayerData
	{
		unsigned int uNumGrunode;	//地表格图形的数目
		unsigned int uNumObject;	//紧贴地面的类似路面之类的对象的数目
		KSPRCrunode* pGrunodes;
		KSPRCoverGroundObj* pObjects;
	};

	//##ModelId=3DE1311803A3
	//##Documentation
	//## 内建对象数据
	struct KBiosData
	{
		KBuildinObjFileHead	Numbers;
		KBuildinObj*		pBios;
		KIpotBuildinObj*	pLeafs;
		KBuildInLightInfo*	pLights;
	};
  private:

	//##ModelId=3DB90015018D
	//##Documentation
	//## 载入障碍信息。
	void LoadObstacle(		 
		//## 数据文件对象的指针
		KPakFile* pDataFile,
		//## 数据文件的大小
		unsigned int uSize); 
	//##ModelId=3DBDADE00001
	//##Documentation
	//## 载入物品信息。
	bool LoadAboveGroundObjects(
		//## 数据文件对象的指针
		KPakFile* pDataFile,
		//## 数据文件的大小
		unsigned int uSize); 
	//##ModelId=3DB9001D00BD
	//##Documentation
	//## 载入地表信息
	bool LoadGroundLayer(
		//## 数据文件对象的指针
		KPakFile* pDataFile,
		//## 数据文件的大小
		unsigned int uSize); 
	// 载入事件点信息
	void LoadTrap(
		//## 数据文件对象的指针
		KPakFile* pDataFile,
		//## 数据文件的大小
		unsigned int uSize); 

	void AddGrundCoverObj(KSPRCoverGroundObj* pObj);
	
  private:

	unsigned int	m_Flag;
	//int m_isClear ;
	//##ModelId=3DBD531C02B2
	//##Documentation
	//## 状态标记
	KREGION_STATUS m_Status;
	
	//##ModelId=3DDC11C400A3
	//##Documentation
	//## Region左上角点在整个场景地图中的坐标（单位：像素点）
	POINT m_LeftTopCornerScenePos;
	//##ModelId=3DBE392600F3
	//##Documentation
	//## 区域的纵向与横向索引坐标
	POINT m_RegionIndex;
	//##ModelId=3DE131AE01A0
	//##Documentation
	//## 内建对象数据
	KBiosData m_BiosData; //这个区域的 树木 房屋等

//-----地表层数据-----
	KGroundLayerData	m_GroundLayerData;

	//##ModelId=3DD7F09402D4
	//##Documentation
	//## 预先渲染的地表层图形。
	KRUImage* m_pPrerenderGroundImg;

	int      m_BackTreeNum;
	int      m_BackHouseNum;
	int      m_ClearState;
//--------------------------------------------------------------------------
	class _KGridRegData
	{
	public:
		_KGridRegData()
		{
#ifdef WIN32
			//ZeroStruct(m_adwDataFlag);
			memset(m_adwDataFlag, 0, sizeof(m_adwDataFlag));
#else
			//memset(m_adwDataFlag);
			memset(m_adwDataFlag, 0, sizeof(m_adwDataFlag));
#endif
		}
		
		BOOL HasData(INT nX, INT nY) CONST
		{
			return (m_adwDataFlag[nX] & (1 << nY)) != 0;
		}
		INT GetData(INT nX, INT nY) CONST
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
				nResult	= m_mapRegData.find(MAKELONG(nX, nY))->second;
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
				m_mapRegData[dwId]	= nData;
			}
			else
			{
				m_adwDataFlag[nX] &= ~(1 << nY);
				m_mapRegData.erase(dwId);
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
				m_mapRegData[dwId]++;
			}
			else								// 新的，直接置1
			{
				m_adwDataFlag[nX] |= 1 << nY;
				m_mapRegData[dwId]	= 1;
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
				if (--m_mapRegData[dwId] == 0)	// 减为0了
				{
					m_mapRegData.erase(dwId);
					m_adwDataFlag[nX] &= ~(1 << nY);
				} 
			}		
Exit0:
			return bResult;
		}
		
		BOOL Clear()
		{
			//ZeroStruct(m_adwDataFlag);
#ifdef WIN32
			//ZeroStruct(m_adwDataFlag);
			memset(m_adwDataFlag, 0, sizeof(m_adwDataFlag));
#else
			//memset(m_adwDataFlag);
			memset(m_adwDataFlag, 0, sizeof(m_adwDataFlag));
#endif
			//_mapRegData::iterator it;
			//for( it = m_mapRegData.begin(); it != m_mapRegData.end(); ++it)
			//  m_mapRegData.erase(it);
			m_mapRegData.clear();
			return TRUE;
		}
	private:
		DWORD m_adwDataFlag[REGION_WIDTH];	// 用位来标记某点是否有数据（刚好REGION_HEIGHT == 32，用一个DWORD）
		typedef std::map<DWORD, INT> _mapRegData;
		_mapRegData m_mapRegData;
	};	
//---------------------------------------------------------


	//long		m_ObstacleInfo[RWP_NUM_GROUND_CELL_H][RWP_NUM_GROUND_CELL_V * 2];
	//long		m_TrapInfo[RWP_NUM_GROUND_CELL_H][RWP_NUM_GROUND_CELL_V * 2];

	_KGridRegData	m_cObstacle;						                // 每个格子上的障碍信息
//	_KGridData	m_cFindPath;										// 寻路表
	_KGridRegData	m_cTrap;							                // 每个格子上的Trap信息

};

#define	GROUND_IMG_OCCUPY_FLAG			Color.Color_b.b
#define	GROUND_IMG_OK_FLAG				Color.Color_b.g

#endif /* KSCENEPLACEREGIONC_H_HEADER_INCLUDED_C22EB91D */
