// ***************************************************************************************
// 场景地图的小地图
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ---------------------------------------------------------------------------------------
// 考虑效率因素，决定障碍小地图显示的范围要包含在场景地图架载的范围之中。场景模块架载地图的范围是
// 7*7=49个区域，障碍小地图的最大显示范围对应为6*6=36个区域。当小地图的缩小比率为16分之一的情况
// 下，则对应的小地图显示图形的大小就限定在不大于192*192像素。
// ***************************************************************************************
#pragma once
//#include	"KLittleMap.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;
#define	MAX_NUM_CHARACTER	40 //40

class KScenePlaceMapC
{
public:
	//对象运行时的一些参数
	enum SCENE_PLACE_MAP_WORK_PARAM_PUB
	{
        MAP_SCALE_H = 16,				//地图在横向上的比例
		MAP_SCALE_V = 32,				//地图在纵向上的比例
	};

public:
	KScenePlaceMapC();
	~KScenePlaceMapC();

	void	Terminate();	//结束对象功能。释放对象的全部数据与动态构造的资源。

	//读取设置
	void Load(KIniFile* pSetting,int nSubWoleID,const char* pszScenePlaceRootPath );//const char* pszScenePlaceRootPath
	//清除
	void Free();

	//设置场景地图的焦点
	void SetFocusPosition(int nX, int nY, bool bChangedRegion);
	//设置场景地图的包含的元素
	void SetShowElemsFlag(unsigned int uShowElemsFlag);

	//设置小地图的大小（单位：像素点）
	void SetSize(int cx, int cy);
	//绘制
	void PaintMap(int nx, int ny);


	//KLittleMap m_LittleMap;

	//后来加的
	//////////////////////////////////////////////////////////////////////////
	/*void PaintSymbol(int nX);
	void LoadSymbol(int nSubWorldID);
	void PaintFindPos(int nX, int nY, int nMpsX, int nMpsY);
	void DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine);
	BOOL OnDirectMap(int nX, int nY);
	void DoDirectMap(int nX, int nY);
	void AutoRunTo(int nX, int nY);	*/
	//////////////////////////////////////////////////////////////////////////


	//获取地图覆盖的范围,返回值表示是否有地图
	int	 GetMapRect(RECT* pRect);
	void SetGreenLine(int nX, int nY, BOOL bSearch);
	//void DrawGreenLine(int nX, int nY, int nX2, int nY2,const char* pString,int nLineNum=2); // 画黄线
	void DelGreenLine();//取消画黄线
	void GetMapSizePos(KSceneMapSizePos* pInfo);
    void PaintPointYellow(int nX, int nY, int nNpcX, int nNpcY,char szName[64],int nType=0);
	void PaintSymbol(int nX ,int nPosX,int nPosY,char *nContent,int Kind);

	void MapGoTo();
    void PaintPointQiZi(int nX, int nY, int nNpcX, int nNpcY,char szName[64]);
	//-----------------
	//INT	*GetBarrierBuf(int nX, int nY);	// 获得某个region的障碍数据的存储地址

	void	SetHaveLoad(int nX, int nY);	// 设定已载入某个region的障碍数据

	void	Draw(int nX, int nY);


	//INT ** GetbtBarrier(return **m_ppbtBarrier;);

    void	GetMapBarrierBuf(INT **lpbtObstacle);


private:
	bool	Initialize();	//初始化
	void	FillCellsPicInfo();
	void	PaintMapPic(int nX, int nY);
	void	PaintCharacters(int nX, int nY);
	void	CalcPicLayout();
	void    SetClientSpr(char *nSprPath,int nxLeft,int nyTop,int nzPos,int i);
	unsigned int GetColor(const char* pString); //将字符串颜色转换成16进制
private:

	//对象运行时的一些参数(仅内部使用)
	enum SCENE_PLACE_MAP_WORK_PARAM
	{
		MAP_CELL_MAP_WIDTH = 128,		//地图块在地图中横向的宽度（单位：像素点）
		MAP_CELL_MAP_HEIGHT = 128,		//地图块在地图中纵向的宽度（单位：像素点）
		MAP_CELL_SCENE_WIDTH = 2048,	//地图块在实际场景中横向的宽度（单位：像素点）= MAP_SCALE_H * MAP_CELL_MAP_WIDTH
		MAP_CELL_SCENE_HEIGHT = 4096,	//地图块在实际场景中纵向的宽度（单位：像素点）= MAP_SCALE_V * MAP_CELL_MAP_HEIGHT
		MAP_CELL_NUM_REGION_H = 4,		//每个地图块在横向包含的场景区域的数目
		MAP_CELL_NUM_REGION_V = 4,		//每个地图块在纵向包含的场景区域的数目

 		MAP_CELL_MAX_RANGE = 5,			//模块中同时绘制的地图块的横/纵向最大跨度（单位：块）
		MAP_CELL_CENTRE_INDEX = 2,		//焦点所在的地图格的横或纵向索引

//		MAP_CELL_FOCUS_INDEX_MIN = 2,	//焦点所在的地图格的横或纵向索引许可最小值
//		MAP_CELL_FOCUS_INDEX_MAX = 2,	//焦点所在的地图格的横或纵向索引许可最大值

		MAP_MAX_SUPPORT_WIDTH  = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_WIDTH,		//小地图的最大支持显示宽（单位：像素点）
		MAP_MAX_SUPPORT_HEIGHT = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_HEIGHT,	//小地图的最大支持显示高（单位：像素点）

		MAP_A_REGION_NUM_MAP_PIXEL_H = 32,	//场景一个区域在地图上的横向跨度（单位：像素点）
		MAP_A_REGION_NUM_MAP_PIXEL_V = 32,	//场景一个区域在地图上的纵向跨度（单位：像素点）
	};

	struct	MAP_CELL
	{
		 char			szImageName[128];
		 unsigned int	uImageId;
		 short			sISPosition;
		 short			sReserved;
	};

private:
	int		m_bHavePicMap;			//此场景是否有所略图形式的小地图
	int     m_bPicMapType;			//此场景是否有所略图形式的小地图
	bool	m_bInited;				//是否执行过初始化
    int     bFlag; 
	BOOL    bSearch;
	int     m_SubWorld;
	RECT    m_MapPos;
	POINT m_GreenLineDes;           //黄线目标坐标
	POINT m_GreenLineYsc;           //黄线当前坐标
	int   m_CurPicWidth;            //当前jpg图片的宽度
	int   m_CurPicHeight;           //当前jpg图片的高度
    int   m_DurFrame[10];
	unsigned int        uFlipTime[10];
	char	m_szEntireMapFile[128];	 //地图图文件名称
	//KSGImageContent*	m_pEntireMap;//完整的缩略地图

	POINT	m_EntireMapLTPosition;	//完整地图的左上角的场景坐标 


	POINT	m_EndMapRBPosition;	    //右下角场景坐标

	POINT	m_FocusPosition;		//焦点坐标
	POINT	m_PicLoadedLTPosition;	//缩略地图加载部分左上点(在整个场景中的对应)坐标
	RECT	m_FocusLimit;			//焦点位置的限定范围

	SIZE			m_Size;			//地图的显示大小
	unsigned int	m_uMapShowElems;//地图中要显示的内容
	//MAP_CELL		m_ElemsList[MAP_CELL_MAX_RANGE][MAP_CELL_MAX_RANGE];

	//--绘制显示时参数预先计算与保留的--
	RECT		m_MapCoverArea;		    //地图绘制部分覆盖的范围（以地图像素点为单位，场景坐标点0,0对应点为原点）
	RECT		m_PaintCell;			//要绘制的地图块索引范围
	SIZE		m_FirstCellSkipWidth;	//绘制的横/纵头一块要跳过的宽/高度
	SIZE		m_LastCellSkipHeight;	//绘制的横/纵最末那块的结束处的宽/高度

	unsigned int	m_uSelfColor;			// 主角颜色
	unsigned int	m_uTeammateColor;		// 队友颜色
	unsigned int	m_uPlayerColor;			// 其他玩家颜色
	unsigned int	m_uFightNpcColor;		// 战斗npc颜色
	unsigned int	m_uNormalNpcColor;		// 普通npc颜色

	//------------------------
	//BYTE		*m_pbyLoadFlag;			    // 某个region的障碍数据载入与否
	//INT 		**m_ppbtBarrier;		    // 所有region的障碍数据
	int			m_nMapWidth;			// 地图长(region, m_sMapRect.right - m_sMapRect.left + 1)
	int			m_nMapHeight;			// 地图宽(region, m_sMapRect.bottom - m_sMapRect.top + 1)
	int			m_nShowWidth;			// 显示宽多少个region的障碍
	int			m_nShowHeight;			// 显示高多少个region的障碍

	RECT        m_MapRect;

	int         m_DestRegIdx_X;		    //  目标所在的区域 索引
    int         m_DestRegIdx_Y;


	int         m_YscRegIdx_X;		    //  人物当前所在的区域 索引
    int         m_YscRegIdx_Y;

	int         m_YscTempRegIdx_X;		//  人物开始寻路的所在的区域 索引
    int         m_YscTempRegIdx_Y;
	int         CreateFolder(char * nPath);
	XPackElemFileRef m_PackRef;
};
