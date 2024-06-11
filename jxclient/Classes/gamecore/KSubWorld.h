#ifndef	KWorldH
#define	KWorldH

#define	MAX_SUBWORLD	1  //客户端的地图限制
#include <map>
#include <vector>

#define	MAP_REGION		-2
//-------------------------------------------------------------
#include "engine/KEngine.h"
#include "KRegion.h"
//#include "KWeatherMgr.h"
#include "GameDataDef.h"
//-------------------------------------------------------------

class  KSubWorld
{
public:	
	int			m_nIndex;
	int			m_SubWorldID;        //地图编号
	int			m_SubWhereTeam;      //当前地图属于那个队伍比赛用的
	BOOL        m_IsShowLoop;        //是否显示地图信息
	
	int         m_WargKillNum;	     //攻方死亡人数	 蓝
	int         m_WarsKillNum;		 //守方死亡人数	 绿

	BYTE        m_IsWhereCity;        //今天是哪个城市被攻城 1,2.....7 用于写入城战文件 
    BYTE        m_WarIsWho;           //属于哪方的 或 帮派地图--共用  1 为攻方 2 为守方
	int         m_ShuiShouType;       //税收的类型  是增加 还是减少

    BYTE        m_IswarCity;          //是否被占领

	int         m_IsLuKey;	          //地图的幸运倍率
	int         m_IsJinYan;		      //地图的经验倍率
	int         m_IsJinQian;		  //地图的银两倍率

	int         m_WarCityManey; //城市建设资金
	int         m_WarCityJB;    //金币
	int         m_WarCityGX;    //贡献
	int         m_GuziGameStat;
    
	int         m_nWPKFlag;  //是否允许PK
	int         m_nWFight;   //是否战斗模式
	int         m_nWcomp;    //该地图的阵营		 Npc[nNpcIndex].SetCurrentCamp(nValue);
    int         m_nMapTime;  //能逗留的时间
    int         m_nSleepTime; //能延时的时间

	int         m_nNpcZongNum;  //总波数
	int         m_nCurNpcNum;	//当前波数
	int         m_nCurNum;      //每波的怪物数

	int         m_nPleyerZongNum;  //总人数
	int         m_nCurPleyerNum;   //当前人数
	KRegion     *m_Region;

	int			m_ClientRegionIdx[MAX_REGION];
	char		m_szMapPath[FILE_NAME_LENGTH];

	int         m_nLeft;
	int         m_nTop;

int			m_nWorldRegionWidth;			//	地图的实际宽度
int			m_nWorldRegionHeight;			//  地图的实际高度	
int			m_nTotalRegion;					//	地图的实际面积大小
int			m_nRegionWidth;					//	地图中每块Region的格子宽度（固定16）
int			m_nRegionHeight;				//	地图中每块Region的格子高度（固定32）
int			m_nCellWidth;					//	Cell的像素宽度（固定32）
int			m_nCellHeight;					//	Cell的像素高度（固定32）
int			m_nRegionBeginX;		        //  地图的开始位置X LEFT		
int			m_nRegionBeginY;                //  地图的开始位置Y TOP
int	        m_nRegionEndX;                  //  地图的结束位置X Rigth
int         m_nRegionEndY;                  //  地图的结束位置Y Buttom
int			m_nWeather;						//	天气变化
DWORD		m_dwCurrentTime;				//	当前帧
//KWorldMsg	m_WorldMessage;					//	消息
KList		m_NoneRegionNpcList;			//	不在地图上的NPC

typedef std::map<INT, std::string> _WorldMapInfo;                 //SkillId,Echance
_WorldMapInfo nWorldMapInfo;

private:
	//	BYTE*		m_pNpcRef;							// 每个格子上的NPC数目
	//  BYTE*		m_pObjRef;							// 每个格子上的OBJ数目	
public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		GetFreeObjPos(POINT& pos);
	void        GetFreePos(POINT& pos);
	void        GetFreeNewObjPos(POINT& pos);
	
	BOOL		CanPutObj(POINT pos,int nModle=0,BOOL nIsCheckNpc=FALSE);
	void		ObjChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close();
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);// 像素级坐标
	void        LookAt(int nX, int nY, int nZ);
	void        GetLocalPositionByMps(int& nX, int& nY, int nZ);
	void        GetMpsByLocalPosition(int& nX, int& nY, int nZ);  //屏幕坐标转成像素坐标
	void		NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy);		// 格子坐标转像素坐标
	//static void Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY);// 格子坐标转像素坐标	
	void		Mps2MapnRodom(int * nR, int * nX, int * nY, int *nDx, int * nDy,int *Rx,int*Ry,int nOffwminx=1,int nOffwmaxx=1,int nOffhminy=1,int nOffhmaxy=1);	// 像素坐标转格子坐标
	void		Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);	    // 像素坐标转格子坐标
	void		GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir = 64);			    // 取得某方向某速度下一点的坐标
	BYTE		TestBarrier(int nMpsX, int nMpsY);
	BYTE		TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 检测下一点是否为障碍
	BYTE		TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 检测下一点是否为障碍
	BYTE		GetBarrier(int nMpsX, int nMpsY);											// 取得某点的障碍信息
	DWORD		GetTrap(int nMpsX, int nMpsY);
	BOOL		SetTrap(int nMpsX, int nMpsY,int nCellNum=1,DWORD uTrapScriptId=0);
	void		MessageLoop();
	int			FindRegion(int RegionID);													// 找到某ID的Region的索引
	int			FindFreeRegion(int nX = 0, int nY = 0);
	void		RemoveNpc(int nIdx);
	void        SetFocusPosition(int nX, int nY, int nZ);	  //设置焦点坐标
	//客户端
	BOOL		LoadMapC(int nIdx, int nRegion,bool isClearAll=false);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx,bool isClearAll=false);
	void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
	void        Mps2MapClient(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);

private:
	void		LoadTrap();
	//void		ProcessMsg(KWorldMsgNode *pMsg);
	void		LoadCell();	
};

extern  KSubWorld	*SubWorld;//SubWorld[MAX_SUBWORLD];

#endif
