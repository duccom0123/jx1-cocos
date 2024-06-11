//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcFindPath.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	<math.h>
#include	"KMath.h"
#include	"KNpcFindPath.h"
#include	"KSubWorld.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#define	MAX_FIND_TIMER	30

//-------------------------------------------------------------------------
//	功能：构造函数
//-------------------------------------------------------------------------
KNpcFindPath::KNpcFindPath()
{
	m_nDestX = 0;
	m_nDestY = 0;
	m_nFindTimer = 0;
	m_nMaxTimeLong = MAX_FIND_TIMER;
	m_nFindState = 0;
	m_nPathSide = 0;
	m_nFindTimes = 0;
	m_NpcIdx = 0;
};

//-------------------------------------------------------------------------
//	功能：初始化
//	参数：nNpc :这个寻路是属于哪个 npc 的
//-------------------------------------------------------------------------
void KNpcFindPath::Init(int nNpc)
{
	m_NpcIdx = nNpc;
	m_nDestX = 0;
	m_nDestY = 0;
	m_nFindTimer = 0;
	m_nMaxTimeLong = MAX_FIND_TIMER;
	m_nFindState = 0;
	m_nPathSide = 0;
	m_nFindTimes = 0;
}
//-------------------------------------------------------------------------
//	功能：传入当前坐标、方向、目标点坐标、速度，寻路找到下一步应该走的方向
//	返回值；如果返回0：有障碍，不能走了;1，找到一个方向，方向值放在pnGetDir (按64方向)；
//			-1：到地图外面去了
//-------------------------------------------------------------------------
#define		defFIND_PATH_STOP_DISTANCE		64
#define		defFIND_PATH_DISTANCE		1

int	KNpcFindPath::GetDir(int nXpos,int nYpos, int nDir, int nDestX, int nDestY, int nMoveSpeed, int *pnGetDir)
{
	if (Npc[m_NpcIdx].m_AutoplayId<=0 && Npc[m_NpcIdx].m_nIsOver)
	{//挂机状态不理睬
		m_nFindTimer = 0;
		m_nFindState = 0;
		m_nFindTimes = 0;
	    Npc[m_NpcIdx].m_nIsOver=FALSE;
		return 0;
	
	}

	// 如果距离接近，认为已经走到了
	if (!CheckDistance(nXpos>>10, nYpos>>10,nDestX,nDestY,nMoveSpeed)) // 15 m_CurrentRunSpeed	  nMoveSpeed
	{
		if (Npc[m_NpcIdx].m_AutoplayId>0 && Npc[m_NpcIdx].IsPlayer())
		{//挂机状态不理睬
		  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop=0;
		}
		m_nFindTimer = 0;
		m_nFindState = 0;
		m_nFindTimes = 0;
		return 0;
	}
	if (Npc[m_NpcIdx].m_AutoplayId>0 && Npc[m_NpcIdx].IsPlayer())
	{//挂机状态
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop ++;
		int nRunTime   = Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop;
		int nForBitTime= Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime;

		//if  (nForBitTime<=0)
		nForBitTime =10;

		if (nRunTime>nForBitTime*18 && nRunTime%(nForBitTime*18)==0)
		{//跑得太久了,还没有跑到.不跑了,就直接飞到哪里了
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:移动超时,模式变更成功!");
			if  (Npc[m_NpcIdx].m_FightMode>0)
			    SendClientCmdSunyi(SubWorld[0].m_SubWorldID,nDestX,nDestY,0); //发送瞬移命令

			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop=0;
			m_nFindTimer = 0;
			m_nFindState = 0;
			m_nFindTimes = 0;
			return 0;
		}
	}
	// 目标点如果有变化，取消原来的找路状态
	if (m_nDestX != nDestX || m_nDestY != nDestY)
	{
		m_nFindTimer = 0;
		m_nFindState = 0;
		m_nFindTimes = 0;
		m_nDestX = nDestX;
		m_nDestY = nDestY;
	}

	int	x, y, nWantDir,n;
	       nWantDir = g_GetDirIndex(nXpos >> 10, nYpos >> 10, nDestX, nDestY);	  //获取要走的方向


    if (nWantDir < 0 )
	{
	  nWantDir = MaxMissleDir + nWantDir;
	}
    if (nWantDir >= MaxMissleDir)
	{
		nWantDir=nWantDir%MaxMissleDir;
	}
	x = g_DirCos(nWantDir,MaxMissleDir) * nMoveSpeed*defFIND_PATH_DISTANCE; //下个要走的点
	y = g_DirSin(nWantDir,MaxMissleDir) * nMoveSpeed*defFIND_PATH_DISTANCE; //下个要走的点
	// 如果有路，直接走
	int nCheckBarrier = CheckBarrierMin(x, y);

	if (nCheckBarrier == Obstacle_NULL)
	{//如果没有障碍
		m_nFindState = 0;
		*pnGetDir = nWantDir;	//就是这个方向了
		return 1;
	}
	// 地图边缘
	else if (nCheckBarrier == 0xff)
	{
		return -1;
	}

	int		i;
	// 从非找路状态进入找路状态
	if (m_nFindState == 0)
	{
		if (Npc[m_NpcIdx].m_RegionIndex >= 0)
			nCheckBarrier = SubWorld[0].TestBarrier(nDestX, nDestY);//粗略判断
		else
			nCheckBarrier = 0xff;

		if (nCheckBarrier != 0 && !CheckDistance(nXpos >> 10, nYpos >> 10, nDestX, nDestY, defFIND_PATH_STOP_DISTANCE))
		{// 如果目标点是障碍而且具体过近，不找了
			m_nFindTimes = 0;
			return 0;
		}
		
	// 如果第二次进入拐弯状态，不找了（只拐一次弯）
	//	m_nFindTimes++;
	//	if (m_nFindTimes > 1)
	//	{
	//		m_nFindTimes = 0;
	//		return 0;
	//	}
		int	nTempDir8, nTempDir64;
		    nTempDir8 = Dir64To8(nWantDir) + 8;		
		// 转换成 8 方向后当前方向是否可行
		    nTempDir64 = Dir8To64(nTempDir8 & 0x07);
		if (nTempDir64 < 0 )
		{
			nWantDir = MaxMissleDir+ nWantDir;
		}
		
		if (nTempDir64 >= MaxMissleDir)
		{
			nTempDir64 = nTempDir64%MaxMissleDir;
		}
		x = g_DirCos( nTempDir64, MaxMissleDir ) * nMoveSpeed;
		y = g_DirSin( nTempDir64, MaxMissleDir ) * nMoveSpeed;

		if (CheckBarrierMin(x, y) == Obstacle_NULL)
		{
			m_nFindState = 1;
			m_nFindTimer = 0;
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				m_nPathSide = 0;
			else
				m_nPathSide = 1;
			*pnGetDir = nTempDir64;
			return 1;
		}
		// 按 8 方向寻找，检查除去正面和背面的另外 6 个方向
		for (i = 1; i < 8; ++i)	   // 32
		{
			nTempDir64 = Dir8To64((nTempDir8 + i) & 0x07);
			if (nTempDir64 < 0 )
			{
				nWantDir = MaxMissleDir+ nWantDir;
			}
			
			if (nTempDir64 >= MaxMissleDir)
			{
				nTempDir64 = nTempDir64%MaxMissleDir;
			} 

			x = g_DirCos( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;
			y = g_DirSin( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;


			if (CheckBarrierMin(x, y) == Obstacle_NULL)
			{
				m_nFindState = 1;
				m_nFindTimer = 0;
				m_nPathSide = 1;
				*pnGetDir = nTempDir64;
				return 1;
			}

			nTempDir64 = Dir8To64((nTempDir8 - i) & 0x07);

			if (nTempDir64 < 0 )
			{
				nWantDir = MaxMissleDir+ nWantDir;
			}
			
			if (nTempDir64 >= MaxMissleDir)
			{
				nTempDir64 = nTempDir64%MaxMissleDir;
			} 

			x = g_DirCos( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;
			y = g_DirSin( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;

			if (CheckBarrierMin(x, y) == Obstacle_NULL )
			{
				m_nFindState = 1;
				m_nFindTimer = 0;
				m_nPathSide = 0;
				*pnGetDir = nTempDir64;
				return 1;
			}
		}
		return 0;
	}
	// 原本是找路状态，继续找路
	else
	{
		// 如果找路时间过长，不找了
	//	if (m_nFindTimer >= m_nMaxTimeLong)
	//	{
	//		m_nFindState = 0;
	//		return 0;
	//	}
		m_nFindTimer++;
		int	nWantDir8, nTempDir64;
		    nWantDir8 = Dir64To8(nWantDir) + 8;
		// 当前方向位于目标方向的右侧
		if (m_nPathSide == 1)
		{
			// 判断是否需要检测当前目标朝向对应的 8 方向上
			nTempDir64 = Dir8To64(nWantDir8 & 0x07);
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				i = 1;
			else
				i = 0;
			// 拐弯过程
			for (; i < 8; ++i)
			{
				nTempDir64 = Dir8To64((nWantDir8 + i) & 0x07);

				if (nTempDir64 < 0 )
				{
					nWantDir = MaxMissleDir+ nWantDir;
				}
				
				if (nTempDir64 >= MaxMissleDir)
				{
					nTempDir64 = nTempDir64%MaxMissleDir;
				} 

				x = g_DirCos( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;
				y = g_DirSin( nTempDir64, MaxMissleDir ) * nMoveSpeed*defFIND_PATH_DISTANCE;
				if (CheckBarrierMin(x, y) == Obstacle_NULL)
				{
					*pnGetDir = nTempDir64;
					return 1;
				}
			}
			m_nFindState = 1;
			m_nFindTimer = 0;
			return 0;
		}
		// 当前方向位于目标方向的左侧
		else
		{
			// 判断是否需要检测当前目标朝向对应的 8 方向上
			nTempDir64 = Dir8To64(nWantDir8 & 0x07);
			if ((nTempDir64 < nWantDir && nWantDir - nTempDir64 <= 4) || (nTempDir64 > nWantDir && nTempDir64 - nWantDir >= 60))
				i = 0;
			else
				i = 1;
			// 拐弯过程
			for (; i < 8; ++i)
			{
				nTempDir64 = Dir8To64((nWantDir8 - i) & 0x07);
				if (nTempDir64 < 0 )
				{
					nWantDir = MaxMissleDir+ nWantDir;
				}
				
				if (nTempDir64 >= MaxMissleDir)
				{
					nTempDir64 = nTempDir64%MaxMissleDir;
				} 

				x = g_DirCos(nTempDir64,MaxMissleDir) * nMoveSpeed*defFIND_PATH_DISTANCE;
				y = g_DirSin(nTempDir64,MaxMissleDir) * nMoveSpeed*defFIND_PATH_DISTANCE;
				if (CheckBarrierMin(x, y) == Obstacle_NULL )
				{
					*pnGetDir = nTempDir64;
					return 1;
				}
			}
			m_nFindState = 1;
			m_nFindTimer = 0;
			return 0;
		}
	}

	m_nFindState = 0;
	m_nFindTimer = 0;
	return 0;
}

//-------------------------------------------------------------------------
//	功能：	64 方向转换为 8 方向
//-------------------------------------------------------------------------
int		KNpcFindPath::Dir64To8(int nDir)
{
	return ((nDir + 4) >> 3) & 0x07;
}

//-------------------------------------------------------------------------
//	功能：	8 方向转换为 64 方向
//-------------------------------------------------------------------------
int		KNpcFindPath::Dir8To64(int nDir)
{
	return nDir << 3;  // *8
}

//-------------------------------------------------------------------------
//	功能：	判断两点间的直线距离是否大于或等于给定距离
//	返回：	距离小于 nDistance 返回 FALSE ，否则返回 TRUE
//-------------------------------------------------------------------------
BOOL	KNpcFindPath::CheckDistance(int x1, int y1, int x2, int y2, int nDistance)
{
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) >= nDistance * nDistance );
}

//-------------------------------------------------------------------------
//	功能：	判断某个点是否是障碍
//-------------------------------------------------------------------------
/*
int	KNpcFindPath::CheckBarrier(int nChangeX, int nChangeY)
{
#ifdef _SERVER
	return SubWorld[Npc[m_NpcIdx].m_SubWorldIndex].TestBarrierMin(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
#else
	if (Npc[m_NpcIdx].m_RegionIndex >= 0)
		return SubWorld[0].TestBarrierMin(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
	else
		return 0xff;
#endif
}
*/
//格子内像数精确查找
int	KNpcFindPath::CheckBarrierMin(int nChangeX, int nChangeY)
{
	if (Npc[m_NpcIdx].m_RegionIndex >= 0)
  //return SubWorld[0].m_Region[Npc[m_NpcIdx].m_RegionIndex].GetBarrierMin(nChangeX, nChangeY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, TRUE);  //有障碍时
	    return SubWorld[0].TestBarrierMin(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
	else
		return 0xff;
}
/*//格子内粗略查找
int	KNpcFindPath::CheckBarrier(int nChangeX, int nChangeY)
{
#ifdef _SERVER
   // return	SubWorld[Npc[m_NpcIdx].m_SubWorldIndex].m_Region[Npc[m_NpcIdx].m_RegionIndex].GetBarrierMin(nChangeX, nChangeY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, TRUE);  //有障碍时
	    return SubWorld[Npc[m_NpcIdx].m_SubWorldIndex].TestBarrier(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
#else
	if (Npc[m_NpcIdx].m_RegionIndex >= 0)
  //return SubWorld[0].m_Region[Npc[m_NpcIdx].m_RegionIndex].GetBarrierMin(nChangeX, nChangeY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, TRUE);  //有障碍时
	    return SubWorld[0].TestBarrier(Npc[m_NpcIdx].m_RegionIndex, Npc[m_NpcIdx].m_MapX, Npc[m_NpcIdx].m_MapY, Npc[m_NpcIdx].m_OffX, Npc[m_NpcIdx].m_OffY, nChangeX, nChangeY);
	else
		return 0xff;
#endif
}
*/