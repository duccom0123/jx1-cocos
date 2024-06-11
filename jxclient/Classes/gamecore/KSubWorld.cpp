#include "KCore.h"
#include <math.h>
#include "KRegion.h"
#include "KMath.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KMissleSet.h"

// #ifdef WIN32
// #include <ppl.h>
// #include "crtdbg.h"
// #endif

#include "gamescene/ObstacleDef.h"

#include "gamescene/KScenePlaceC.h"
//#include "scene/kpathfinder.h"

#include "KSubWorld.h"

KSubWorld	*SubWorld = NULL;//SubWorld[MAX_SUBWORLD];

#define		defLOGIC_CELL_WIDTH			32
#define		defLOGIC_CELL_HEIGHT		32


KSubWorld::KSubWorld()
{
	nWorldMapInfo.clear();
	m_Region = NULL;
	m_nRegionWidth	= 16;//512 / 32; //16
	m_nRegionHeight	= 32;//1024 / 32;//32
	m_nCellWidth  = defLOGIC_CELL_WIDTH;
	m_nCellHeight = defLOGIC_CELL_HEIGHT;
	m_dwCurrentTime = 0;

	m_SubWorldID = -1;
	nWorldMapInfo[STR_MAP_MASTER] = "空缺";
    nWorldMapInfo[STR_MAP_ZTONGNAME] = "空缺";
	nWorldMapInfo[STR_MAP_TGONGNAME] = "空缺";
	nWorldMapInfo[STR_MAP_TSHOUNAME] = "空缺";
	m_WargKillNum=0;	                   //攻方死亡人数	 蓝
	m_WarsKillNum=0;		               //守方死亡人数	 绿
    m_ShuiShouType = 0;
	m_IswarCity=0;

	m_IsLuKey=1;	          //地图的幸运倍率
	m_IsJinYan=0;		      //地图的经验倍率
	m_IsJinQian=1;		      //地图的银两倍率
	m_WarCityManey=0;
	m_WarCityJB=0;
	m_WarCityGX=0;
	m_nWPKFlag=-1;       //可切换PK模式模式
	m_GuziGameStat=-1;
	m_nWFight=-1;
	m_WarIsWho=0;        //公用模式
	m_IsWhereCity=0;
	m_nWcomp=-1;
	m_nMapTime=0;
	m_SubWhereTeam=0;
	m_nSleepTime=-1;
	m_IsShowLoop=FALSE;

	m_nNpcZongNum=0;  //总波数
    m_nCurNpcNum=0;	  //当前波数
	m_nCurNum=0;      //每波的怪物数
	m_nWeather = WEATHERID_NOTHING;
	m_nWorldRegionWidth = 3;
	m_nWorldRegionHeight = 3;
	memset(m_ClientRegionIdx, 0, sizeof(this->m_ClientRegionIdx));
	memset(this->m_szMapPath, 0, sizeof(this->m_szMapPath));
	m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
	m_nLeft = 0;
	m_nTop  = 0;
}

KSubWorld::~KSubWorld()
{
	if (m_Region)
	{
		delete [] m_Region;
		m_Region = NULL;
	}

/*#ifdef _SERVER
	if(m_pWeatherMgr)
	{
		delete m_pWeatherMgr;
		m_pWeatherMgr = NULL;
	}
#endif*/
}

int KSubWorld::FindRegion(int nRegion)
{
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].m_RegionID == nRegion)
			return i;
	}
	return -1;
}

int KSubWorld::FindFreeRegion(int nX, int nY)
{
	if (nX == 0 && nY == 0)
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
		}
	}
	else
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
			int nRegoinX = LOWORD(m_Region[i].m_RegionID);
			int nRegoinY = HIWORD(m_Region[i].m_RegionID);

			if ((nX - nRegoinX) * (nX - nRegoinX) + (nY - nRegoinY) * (nY - nRegoinY) > 2)	// 不在附近
				return i;
		}
	}
	return -1;
}

//extern int nActiveRegionCount;
//地图的循环
void KSubWorld::Activate()
{
	if (m_SubWorldID < 0)
		return;

	++m_dwCurrentTime;	  //每秒18帧

	if (m_nMapTime>0)
	{
		m_nMapTime--;
	}

	if (m_nSleepTime>0)
	{
		m_nSleepTime--;
	}
	//设置场景的时间
	//g_ScenePlace.SetCurrentTime(m_dwCurrentTime);
	NpcSet.ClearActivateFlagOfAllNpc();           //全部设定为 否 ，执行循环处理
	for (int i = 0; i < m_nTotalRegion;i++)      //地图的总面积
	{
		m_Region[i].NewRegActivate();
	}
}

int KSubWorld::GetDistance(int nRx1, int nRy1, int nRx2, int nRy2)
{
	return (int)sqrt(double((nRx1 - nRx2)) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2));
}

void KSubWorld::SetFocusPosition(int nX, int nY, int nZ)
{
	LookAt(nX, nY, 0);
}

void KSubWorld::LookAt(int nX, int nY, int nZ)
{//m_Canvas.GetWidth() ==800  m_Canvas.GetHeight() ==480 
	m_nLeft = nX - visibleSize.width/2;
	m_nTop  = nY / 2 - ((nZ * 887) >> 10) - visibleSize.height/2;
}

void KSubWorld::GetMpsByLocalPosition(int& nX,	int& nY, int  nZ)
{//屏幕坐标-->像素坐标
	nX = nX  + m_nLeft;
	nY = (nY + m_nTop + ((nZ * 887) >> 10)) * 2;
	//nX = nX + m_RepresentArea.left + SPWP_REPRESENT_RECT_WINDAGE_X;
	//nY = (nY + ((nZ * 887) >> 10)) * 2 + m_RepresentArea.top  + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
}

void KSubWorld::GetLocalPositionByMps(int& nX, int& nY, int nZ)
{//像素坐标-->屏幕坐标
	nX = nX - m_nLeft;
	nY = nY / 2 - m_nTop - ((nZ * 887) >> 10);	// * sqrt(3) / 
}

void KSubWorld::NewMap2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy)
{
/*
#ifndef _SERVER
//	_ASSERT(nR >= 0 && nR < 9);
#endif
	//_ASSERT(nR >= 0);
*/
	if (nR < 0 || nR >= m_nTotalRegion)
	{
		*nRx = 0;
		*nRy = 0;
		return;
	}

	int x, y;
	
	x = m_Region[nR].m_nRegionX;
	y = m_Region[nR].m_nRegionY;
	
	x += nX * m_nCellWidth;
	y += nY * m_nCellHeight;
	
	x += (nDx >> 10);
	y += (nDy >> 10);
	
	*nRx = x;
	*nRy = y;
	
}
/*/格子中的 坐标转换成 像素坐标(废用)
void KSubWorld::Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY)
{
	*pnX = (nRx * REGION_GRID_WIDTH + nX) * defLOGIC_CELL_WIDTH + (nDx >> 10);
	*pnY = (nRy * REGION_GRID_HEIGHT + nY) * defLOGIC_CELL_HEIGHT + (nDy >> 10);
} */
//像素转格子
void KSubWorld::Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;

	int x = Rx/(m_nRegionWidth * m_nCellWidth);	 //取商	  16*32 ==512
	int	y = Ry/(m_nRegionHeight * m_nCellHeight);//取商	  32*32==1024

	      /*if (Rx%(m_nRegionWidth * m_nCellWidth)>=(m_nRegionWidth * m_nCellWidth)/2)
			  x++;

		  if (Ry%(m_nRegionHeight * m_nCellHeight)>=(m_nRegionHeight * m_nCellHeight)/2)
			  y++; */

	*nX =  0;
	*nY =  0;
	*nDx = 0;
	*nDy = 0;
	// 非法的坐标
	int nRegionID = MAKELONG(x, y);
	    *nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}
//////加个障碍物判断/////////////////

/////////////////////////////////////
	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = x/m_nCellWidth;	 //取商
	*nY = y/m_nCellHeight; //取商

	*nDx = (x - *nX * m_nCellWidth) << 10;	  //1024
	*nDy = (y - *nY * m_nCellHeight) << 10;
}


//像素转格子
void KSubWorld::Mps2MapClient(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;

	int x = Rx/(m_nRegionWidth * m_nCellWidth);	 //取商	  16*32 ==512
	int	y = Ry/(m_nRegionHeight * m_nCellHeight);//取商	  32*32==1024

	      /*if (Rx%(m_nRegionWidth * m_nCellWidth)>=(m_nRegionWidth * m_nCellWidth)/2)
			  x++;

		  if (Ry%(m_nRegionHeight * m_nCellHeight)>=(m_nRegionHeight * m_nCellHeight)/2)
			  y++; */

	*nX =  0;
	*nY =  0;
	*nDx = 0;
	*nDy = 0;

	int nRegionID = MAKELONG(x, y);

	    *nR = FindRegion(nRegionID);

	if (*nR == -1)
		return;

	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	} 
//////加个障碍物判断/////////////////

/////////////////////////////////////
	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = x/m_nCellWidth;	 //取商
	*nY = y/m_nCellHeight;   //取商

	*nDx = (x - *nX * m_nCellWidth) << 10;	  //1024
	*nDy = (y - *nY * m_nCellHeight) << 10;
}
//////随机刷怪////////////////////////////////////////////////////
void KSubWorld::Mps2MapnRodom(int * nR, int * nX, int * nY, int *nDx, int * nDy,int *Rx,int*Ry,int nOffwminx,int nOffwmaxx,int nOffhminy,int nOffhmaxy)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return;
//	int x = Rx / (m_nRegionWidth * m_nCellWidth);
//	int	y = Ry / (m_nRegionHeight * m_nCellHeight);
	int x,y;
    for (int n=0;n<m_nRegionEndX;++n)
    {
      x =GetRandomNumber(m_nRegionBeginX+nOffwminx,m_nWorldRegionWidth +m_nRegionBeginX -nOffwmaxx);  
	  for (int s=0;s<m_nRegionEndY;++s)
	  {	 
	  y =GetRandomNumber(m_nRegionBeginY+nOffhminy,m_nWorldRegionHeight + m_nRegionBeginY-nOffhmaxy);
      *Rx=x*(m_nRegionWidth * m_nCellWidth); //=x*16*32
      *Ry=y*(m_nRegionHeight * m_nCellHeight);//=y*32*32
	//int Rxx=x*(m_nRegionWidth * m_nCellWidth);
         int nBarrier = GetBarrier(*Rx, *Ry); ///像素坐标障碍物判断	
	     if (nBarrier==Obstacle_NULL)
		 { 
		    break;
		 } 
         continue;
	  }
    }
	*nX = 0;
	*nY = 0;
	*nDx = 0;
	*nDy = 0;
	int nRegionID = MAKELONG(x, y);
	*nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}
	//////加个障碍物判断/////////////////
	
	/////////////////////////////////////
	x = *Rx - m_Region[*nR].m_nRegionX;
	y = *Ry - m_Region[*nR].m_nRegionY;
	
	*nX = x / m_nCellWidth;
	*nY = y / m_nCellHeight;
	
	*nDx = (x - *nX * m_nCellWidth) << 10;
	*nDy = (y - *nY * m_nCellHeight) << 10;
}
//格子内粗略查找
////////////////障碍物判断////////////////////////////////////////////
BYTE	KSubWorld::TestBarrier(int nMpsX, int nMpsY)
{
	if (m_nCellWidth == 0 || m_nCellHeight == 0 || m_nRegionWidth == 0 || m_nRegionHeight == 0)
		return 0xff;
	
	int x = nMpsX / (m_nRegionWidth * m_nCellWidth);
	int	y = nMpsY / (m_nRegionHeight * m_nCellHeight);
	int nRegion = FindRegion(MAKELONG(x, y));
	if (nRegion == -1)
		return 0xff;
	if (nRegion >= m_nTotalRegion)
		return 0xff;
	
	x = nMpsX - m_Region[nRegion].m_nRegionX;
	y = nMpsY - m_Region[nRegion].m_nRegionY;
	
	int nCellX = x / m_nCellWidth;
	int nCellY = y / m_nCellHeight;
	
	int nOffX = x - nCellX * m_nCellWidth;
	int nOffY = y - nCellY * m_nCellHeight;
	
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);  //获取障碍的信息
	if (bRet != Obstacle_NULL)
		return bRet;
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
}
//格子内粗略查找
BYTE KSubWorld::TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= m_nRegionHeight;
	}

	int nXf, nYf;
	nXf = (nDx >> 10);
	nYf = (nDy >> 10);
//需要检查的
	int nMpsX, nMpsY;
	NewMap2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
	if (bRet != Obstacle_NULL)
		return bRet;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
}
//格子内像数精确查找
BYTE KSubWorld::TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (m_nCellWidth << 10);
		nMapX--;
	}
	else if (nDx >= (m_nCellWidth << 10))
	{
		nDx -= (m_nCellWidth << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (m_nCellHeight << 10);
		nMapY--;
	}
	else if (nDy >= (m_nCellHeight << 10))
	{
		nDy -= (m_nCellHeight << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += m_nRegionWidth;
	}
	else if (nMapX >= m_nRegionWidth)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= m_nRegionWidth;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];
		nMapY += m_nRegionHeight;
	}
	else if (nMapY >= m_nRegionHeight)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= m_nRegionHeight;
	}

	int nMpsX, nMpsY;
	NewMap2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = (BYTE)g_ScenePlace.GetObstacleInfoMin(nMpsX, nMpsY, nDx & 0x000003ff, nDy & 0x000003ff);
	if (bRet != Obstacle_NULL)
		return bRet;

	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
		return Obstacle_NULL;

	return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy,TRUE); //精确查找
}

BOOL KSubWorld::SetTrap(int nMpsX, int nMpsY,int nCellNum,DWORD uTrapScriptId)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	    Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return FALSE;

	return m_Region[nRegion].SetTrap(nMapX, nMapY,nCellNum,uTrapScriptId);
}

DWORD KSubWorld::GetTrap(int nMpsX, int nMpsY)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0;
	return m_Region[nRegion].GetTrap(nMapX, nMapY);
}

BYTE KSubWorld::GetBarrier(int nMpsX, int nMpsY)
{
	return (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
}


///却换地图的时候客户端加载地图信息
BOOL KSubWorld::LoadMapC(int nId, int nRegion,bool isClearAll)
{
	static int	nXOff[8] = {0, -1, -1, -1, 0,  1, 1, 1};
	static int	nYOff[8] = {1, 1,  0,  -1, -1, -1, 0, 1};
	KIniFile	IniFile;
	//KIniFile	Ini;

	if (!m_Region)
	{//客户端 是 9大区域
		m_Region = new KRegion[MAX_REGION];
	}

	if (m_Region==NULL)
		return FALSE;
	//bool isClear = false;
	if (nId != m_SubWorldID)
	{//如果地图不同的话，必定崩溃 64位系统

		//Player[CLIENT_PLAYER_INDEX].m_bExchangeServer =true;
		SubWorld[0].Close();      //删除当前地图的所有东西
		g_ScenePlace.ClosePlace();
		//isClear = true;
		memset(m_szMapPath, 0, sizeof(m_szMapPath));
		char szKeyName[32]={0}, szPathName[128]={0};
		memset(szKeyName, 0, sizeof(szKeyName));
		memset(szPathName, 0, sizeof(szPathName));
		g_SetFilePath("/");
        
		if (IniFile.Load("settings\\MapList.ini"))
		{
			t_sprintf(szKeyName,"%d", nId);
			IniFile.GetString("List",szKeyName,"",szPathName, sizeof(szPathName));
			t_sprintf(m_szMapPath,"maps\\%s",szPathName);	 //区域的路径
			IniFile.Clear();
		}
		m_SubWorldID = nId;
		g_ScenePlace.OpenPlace(nId);  //打开场景空间

		/*t_sprintf(szPathName, "\\maps\\%d.ini", nId);
		if (Ini.Load(szPathName))
			g_ScenePlace.LoadIni(&Ini);
		else
		{
			g_ScenePlace.ClearBgInfo();
		}
		Ini.Clear(); 
		*/
		m_nRegionWidth  = 16;//KScenePlaceRegionC::RWPP_AREGION_WIDTH>>5;  //=16
		m_nRegionHeight = 32;//KScenePlaceRegionC::RWPP_AREGION_HEIGHT>>5;//=32
		m_nCellWidth    = defLOGIC_CELL_WIDTH;
		m_nCellHeight   = defLOGIC_CELL_HEIGHT;
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex = -1;
	}//第一次转换地图的时候

	int nX = LOWORD(nRegion);
	int nY = HIWORD(nRegion);

	int nIdx = FindRegion(nRegion);

	if (nIdx < 0)
	{//主人的 当前区域
		nIdx = m_ClientRegionIdx[0];//哪个区域

		if (m_Region[nIdx].Load(nX, nY))
		{
			m_Region[nIdx].m_nIndex = nIdx;//区域的索引
			m_Region[nIdx].Init(m_nRegionWidth, m_nRegionHeight);
			m_Region[nIdx].LoadObjectC(0, nX, nY, m_szMapPath);//加载 obj 
		}
	}
	//设置焦点（人物）位置
	//SetFocusPosition(m_Region[nIdx].m_nRegionX, m_Region[nIdx].m_nRegionY, 0);
	g_ScenePlace.SetFocusPosition(m_Region[nIdx].m_nRegionX, m_Region[nIdx].m_nRegionY,0,isClearAll);
	m_ClientRegionIdx[0] = nIdx; //当前区域

	for (int i = 0; i < 8;i++)
	{//链接相邻近的 几个区域
		int nConIdx;
		    nConIdx = FindRegion(m_Region[nIdx].m_nConRegionID[i]);
		if (nConIdx < 0)
		{
			  nConIdx = FindFreeRegion(nX, nY);
			//_ASSERT(nConIdx >= 0);
			  if (nConIdx<0)
				continue;

			  if (m_Region[nConIdx].Load(nX + nXOff[i], nY + nYOff[i]))
			  { 
				m_Region[nConIdx].m_nIndex = nConIdx;
				m_Region[nConIdx].Init(m_nRegionWidth, m_nRegionHeight);
				m_Region[nConIdx].LoadObjectC(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);
			  }
			  else
			  { 
				m_Region[nConIdx].m_nIndex   = -1;
				m_Region[nConIdx].m_RegionID = -1;
				nConIdx = -1;
			  }
		}
		m_ClientRegionIdx[i + 1] = nConIdx;
		m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
		m_Region[nIdx].m_pConRegion[i] = &m_Region[nConIdx];
	}
	//
	if (m_Region[nIdx].m_nConnectRegion[0] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[4] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[7];

		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[7] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[4] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[3] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[5] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[7];
	}
	m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[1];
	if (m_Region[nIdx].m_nConnectRegion[1] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[5] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[0];

		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[7] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[5] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[0];
	}

	if (m_Region[nIdx].m_nConnectRegion[2] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[6] = nIdx;

		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[7] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[5] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_pConRegion[6] = &m_Region[nIdx];
	}

	if (m_Region[nIdx].m_nConnectRegion[3] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[7] = nIdx;

		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[2] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_pConRegion[7] = &m_Region[nIdx];
	}

	if (m_Region[nIdx].m_nConnectRegion[4] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[0] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[6];

		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[0] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[1] = m_Region[nIdx].m_pConRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[6] = m_Region[nIdx].m_pConRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_pConRegion[7] = m_Region[nIdx].m_pConRegion[6];
	}

	if (m_Region[nIdx].m_nConnectRegion[5] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[1] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[1] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[3] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[4] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_pConRegion[7] = NULL;

	}

	if (m_Region[nIdx].m_nConnectRegion[6] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[2] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[0] = m_Region[nIdx].m_pConRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[1] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[2] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[3] = m_Region[nIdx].m_pConRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_pConRegion[7] = NULL;
	}
	
	if (m_Region[nIdx].m_nConnectRegion[7] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[3] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[7] = -1;

		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[0] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[1] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[2] = m_Region[nIdx].m_pConRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[3] = &m_Region[nIdx];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[4] = m_Region[nIdx].m_pConRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[5] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[6] = NULL;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_pConRegion[7] = NULL;
	}
	//CCMessageBox("增加地图完毕","LoadMapC");
	//g_ScenePlace.setIsPaint(true);
	//Player[CLIENT_PLAYER_INDEX].m_bExchangeServer =false;

	return TRUE;
}

void KSubWorld::LoadTrap()
{

}

void KSubWorld::MessageLoop()
{
	/*KWorldMsgNode	Msg;
	while(m_WorldMessage.Get(&Msg))
	{
		try
		{
		   ProcessMsg(&Msg);
		}
		catch (...)
		{
		}

	}*/
}
/*
void KSubWorld::ProcessMsg(KWorldMsgNode *pMsg)
{
	switch(pMsg->m_dwMsgType)
	{
	case GWM_NPC_DEL:
//#ifndef _SERVER
		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_NPC)
			break;

		if (Npc[pMsg->m_nParam[0]].m_RegionIndex >= 0)
		{
			int nIdx = pMsg->m_nParam[0];

			if (nIdx<=0)
				break;

			int nSubWorld = Npc[nIdx].m_SubWorldIndex;
			int nRegion = Npc[nIdx].m_RegionIndex;

			if (nSubWorld>=0 && nRegion>=0)
			{
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
				SubWorld[nSubWorld].m_Region[nRegion].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
			}
			
		}
		NpcSet.Remove(pMsg->m_nParam[0]);
		break;
	//case GWM_NPC_CHANGE_REGION:
	//	NpcChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
	//	break;
	case GWM_OBJ_DEL:

		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_OBJECT)
			break;
//#ifndef _SERVER
		if (Object[pMsg->m_nParam[0]].m_nRegionIdx >= 0)
			SubWorld[Object[pMsg->m_nParam[0]].m_nSubWorldID].m_Region[Object[pMsg->m_nParam[0]].m_nRegionIdx].RemoveObj(pMsg->m_nParam[0]);
		
		ObjSet.Remove(pMsg->m_nParam[0]);
		break;
	case GWM_OBJ_CHANGE_REGION:
		ObjChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_CHANGE_REGION:
		MissleChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
		break;
	case GWM_MISSLE_DEL:
		if  (pMsg->m_nParam[0]<=0 || pMsg->m_nParam[0]>=MAX_MISSLE)
			break;
//#ifndef _SERVER
		if (Missle[pMsg->m_nParam[0]].m_nRegionId >= 0)
		{
			SubWorld[Missle[pMsg->m_nParam[0]].m_nSubWorldId].m_Region[Missle[pMsg->m_nParam[0]].m_nRegionId].RemoveMissle(pMsg->m_nParam[0]);
	//		g_DebugLog("[Missle]Missle%dDEL", pMsg->m_nParam[0]);
			MissleSet.Remove(pMsg->m_nParam[0]);
		}
		break;
	//case GWM_PLAYER_CHANGE_REGION:
	//	PlayerChangeRegion(pMsg->m_nParam[0], pMsg->m_nParam[1], pMsg->m_nParam[2]);
	//	break;
	default:
		break;
	}
}*/

void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx,bool isClearAll)
{
	int		nSrc, nDest;

	if (nSrcRnidx == -1)
	{
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest < 0)
			return;
		m_Region[nDest].AddNpc(nIdx);
		Npc[nIdx].m_dwRegionID  = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
		g_ScenePlace.setIsPaint(true);
		return;
	}

	nSrc = SubWorld[0].FindRegion(nSrcRnidx);
	if (nSrc >= 0)
		SubWorld[0].m_Region[nSrc].RemoveNpc(nIdx);

	nDest = SubWorld[0].FindRegion(nDesRnIdx);

	KIndexNode *pNode = &Npc[nIdx].m_Node;
	if (nDest >= 0)
	{//如果目标区域 
		m_Region[nDest].AddNpc(nIdx);
		
		if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
		{//如果是客户端
			LoadMapC(m_SubWorldID, m_Region[nDest].m_RegionID,isClearAll);
		}

		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;

	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
	{//如果是客户端人物 
		LoadMapC(m_SubWorldID,nDesRnIdx,isClearAll);

		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest >= 0)
		{
			m_Region[nDest].AddNpc(nIdx);
			Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
			Npc[nIdx].m_RegionIndex = nDest;
		}
	}
	g_ScenePlace.setIsPaint(true);
}
//没有用到
void KSubWorld::ObjChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
//#ifdef _SERVER
//	_ASSERT(nSrcRnidx >= 0);
//#endif

	if (nIdx<=0 || nIdx>=MAX_OBJECT)
	   return;

	if (nDesRnIdx == -1 && nSrcRnidx>=0)
	{
		SubWorld[Object[nIdx].m_nSubWorldID].m_Region[nSrcRnidx].RemoveObj(nIdx);
//		ObjSet.Remove(nIdx);
		return;
	}

	if (nSrcRnidx == -1 && nDesRnIdx>=0)
	{
		m_Region[nDesRnIdx].AddObj(nIdx);
		return;
	}

	/*KRegion::VEC_LIST::iterator it = m_Region[nSrcRnidx].GetObjNode(nIdx);
	if (it != m_Region[nSrcRnidx].m_ObjList.end())
	{
		m_Region[nSrcRnidx].m_ObjList.erase(it);
		m_Region[nDesRnIdx].m_ObjList.push_back(nIdx);
	}*/
	KIndexNode* pNode = (KIndexNode *)m_Region[nSrcRnidx].GetObjNode(nIdx);
	if (pNode)
	{
		pNode->Remove();
		if (nDesRnIdx>=0)
		   m_Region[nDesRnIdx].m_ObjList.AddTail(pNode);
	}
}

void KSubWorld::MissleChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_MISSLE)
		return;

	if (nSrcRnidx == -1 && nDesRnIdx>=0)
	{
		m_Region[nDesRnIdx].AddMissle(nIdx);
		return;
	}
	if (nSrcRnidx>=0 && Missle[nIdx].m_nSubWorldId>=0)
	   SubWorld[Missle[nIdx].m_nSubWorldId].m_Region[nSrcRnidx].RemoveMissle(nIdx);

	if (nDesRnIdx == -1)
	{
		MissleSet.Remove(nIdx);
		return;
	}

	if (nDesRnIdx>=0)
	   m_Region[nDesRnIdx].AddMissle(nIdx);
}

void KSubWorld::GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir /* = 64 */)
{
	/*if (nDir < 0)
		nDir = MaxMissleDir +nDir;
	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;*/
	int n;
	if (nDir < 0 )
	{
		if (nDir>=-MaxMissleDir)
			nDir = MaxMissleDir + nDir;
		else
		{//-129;
			n=-nDir%MaxMissleDir;//		 -1
			nDir = MaxMissleDir-n-1;
		}
		//nWantDir = MaxMissleDir+ nWantDir;
	}
	
	if (nDir >= MaxMissleDir)
	{
		n=nDir/MaxMissleDir;
		nDir -= MaxMissleDir*n;
	} 

	*nX += (g_DirCos(nDir, nMaxDir) * nSpeed) >> 10;
	*nY += (g_DirSin(nDir, nMaxDir) * nSpeed) >> 10;
}

void KSubWorld::LoadCell()
{

}

void KSubWorld::Paint()
{
	int nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nX, nY;
	NewMap2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &nX, &nY);
//	地图的绘画

/*	for (int i = 0; i < m_nTotalRegion; ++i)
	{
		m_Region[i].Paint();
	}*/
}

void KSubWorld::Mps2Screen(int *Rx, int *Ry)
{

}

void KSubWorld::Screen2Mps(int *Rx, int *Ry)
{

}

void KSubWorld::Close()
{
	if (m_SubWorldID == -1)
		return;

	for (int i = 0; i < m_nTotalRegion; i++)
	{
		m_Region[i].Close();
	}
	//m_WorldMessage.Clear();
	m_nIndex = -1;
	m_SubWorldID = -1;
}

void KSubWorld::AddPlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
	{
		//printf("---错误ARegion:(%d) Player(%d) TotalRegion(%d)---\n", nRegion, nIdx,m_nTotalRegion);
		return;
	}

	if (m_Region[nRegion].AddPlayer(nIdx))
	{
		m_Region[nRegion].m_nActive++;

		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive++;  //激活的区域加1
		}
		//g_ScenePlace.setIsPaint(true);
	}
	//else
	//{
		//printf("---错误BRegion:(%d) Player(%d) TotalRegion(%d)---\n", nRegion, nIdx,m_nTotalRegion);
	//}
}

void KSubWorld::RemovePlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
		return;

	if (m_Region[nRegion].RemovePlayer(nIdx))
	{	
		m_Region[nRegion].m_nActive--;		

		if (m_Region[nRegion].m_nActive < 0)
		{
			//_ASSERT(0);
			m_Region[nRegion].m_nActive = 0;
		}
		
		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive--;

			if (m_Region[nConRegion].m_nActive < 0)
			{
				//_ASSERT(0);
				m_Region[nConRegion].m_nActive = 0;
			}
		}
	}
}



void KSubWorld::GetFreePos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int nLayer = 1;

	//if (CanPutObj(posLocal,nModel)) //如果 NPC所在的地方能掉东西 就 返回
	//	return;

    int xPos=1,yPos=1,nCheckNum=512,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;

    if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	   if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		   return;
	}

	while(1)
	{
		for (int i = 0; i <= nLayer; ++i)
		{
         	posTemp.y = posLocal.y;
			posTemp.x = posLocal.x+(i+1)*xPos;      //正右边

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
			}
     
	        posTemp.y = posLocal.y+(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正下边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}

			posTemp.y = posLocal.y;
			posTemp.x = posLocal.x-(i+1)*xPos;       //正左边 

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}

			posTemp.y = posLocal.y-(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正上边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{

			}
			posTemp.y = posLocal.y + (i+1) * yPos;           //像数坐标
			posTemp.x = posLocal.x + (i+1) * xPos;           //往右下方向(相隔的距离) (nLayer - i)

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y + (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;            //往左下方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y - (i+1) * yPos;             //往右上方向
			posTemp.x = posLocal.x + (i+1) * xPos;
			if (CanPutObj(posTemp,nModel))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

			posTemp.y = posLocal.y - (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;             //往左上方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			else
			{
				
			}

		}

		nLayer++;

		if (nLayer >= nCheckNum)
			break;
	}

	return;
}


void KSubWorld::GetFreeObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	INT nLayer = 1;
	
	//	if (CanPutObj(posLocal))
	//		return;
	int /*xPos=16,yPos=16,*/nCheckNum=32,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;
	//g_GameSetTing.GetInteger2("Item","ItemDropPos",&xPos,&yPos); // 偏移
	g_GameSetTing.GetInteger2("Item","ItemDropCheckNum",&nCheckNum,&nIsCheckNpcPos); // 查找的范围
	g_GameSetTing.GetInteger2("Item","ItemDropModel",&nModel,&nIsCheckNpc);
       
	if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	    if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		    return;
	}
	
	while(1)
	{
		for (INT i = 0; i <= nLayer; ++i)
		{
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
		}
		nLayer++;
		if (nLayer >= 10)
			break;
	}
	return;
}

//掉下物品距离调节
void KSubWorld::GetFreeNewObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int     nLayer = 1;

	//if (CanPutObj(posLocal,nModel)) //如果 NPC所在的地方能掉东西 就 返回
	//	return;

    int xPos=16,yPos=16,nCheckNum=32,nIsCheckNpcPos=0,nModel=0,nIsCheckNpc=0;
       g_GameSetTing.GetInteger2("Item","ItemDropPos",&xPos,&yPos); // 偏移
	   g_GameSetTing.GetInteger2("Item","ItemDropCheckNum",&nCheckNum,&nIsCheckNpcPos); // 查找的范围
	   g_GameSetTing.GetInteger2("Item","ItemDropModel",&nModel,&nIsCheckNpc);
       
    if (nIsCheckNpcPos==1) //如果 NPC所在的地方能掉东西 就 返回
	{//检测NPC点
	   if (CanPutObj(posLocal,nModel,nIsCheckNpc))
		   return;
	}

	while(1)
	{
		for (int i = 0; i <= nLayer; ++i)
		{
         	posTemp.y = posLocal.y;
			posTemp.x = posLocal.x+(i+1)*xPos;      //正右边

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
     
	        posTemp.y = posLocal.y+(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正下边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y;
			posTemp.x = posLocal.x-(i+1)*xPos;       //正左边 

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y-(i+1)*yPos;
			posTemp.x = posLocal.x;                  //正上边 
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y + (i+1) * yPos;           //像数坐标
			posTemp.x = posLocal.x + (i+1) * xPos;           //往右下方向(相隔的距离) (nLayer - i)

			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y + (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;            //往左下方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - (i+1) * yPos;             //往右上方向
			posTemp.x = posLocal.x + (i+1) * xPos;
			if (CanPutObj(posTemp,nModel))
			{
				pos = posTemp;
				return;
			}

			posTemp.y = posLocal.y - (i+1) * yPos;
			posTemp.x = posLocal.x - (i+1) * xPos;             //往左上方向
			if (CanPutObj(posTemp,nModel,nIsCheckNpc))
			{
				pos = posTemp;
				return;
			}
		}

		nLayer++;

		if (nLayer >= nCheckNum)
			break;
	}

	return;
}

BOOL KSubWorld::CanPutObj(POINT pos,int nModle,BOOL nIsCheckNpc)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY,nIsShowInfo=0,nIsShowHave=0;
	    Mps2Map(pos.x,pos.y,&nRegion,&nMapX,&nMapY,&nOffX,&nOffY);
   
if (nModle==0)
{
	if (nRegion >= 0 \
		&& !m_Region[nRegion].GetNewBarrier(nMapX,nMapY,nOffX,nOffY,nIsCheckNpc)\
		&& !m_Region[nRegion].HasObj(nMapX,nMapY))//m_Region[nRegion].GetRef(nMapX,nMapY,obj_object))
		{
           	return TRUE;
		}
}
else 
{
	if (nRegion >= 0 \
		&& !m_Region[nRegion].GetBarrierNewMin(nMapX,nMapY,nOffX,nOffY,nIsCheckNpc)\
		&& !m_Region[nRegion].HasObj(nMapX,nMapY))//m_Region[nRegion].GetRef(nMapX,nMapY,obj_object))
		{
            return TRUE;
		}
}
	return FALSE;
}
