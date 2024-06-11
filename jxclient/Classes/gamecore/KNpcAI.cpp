#include "KCore.h"
#include "KNpcAI.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KMath.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KPlayer.h"
#include "KItem.h"
#include "KItemSet.h"

#include "gamescene/KScenePlaceC.h"
#include "KMagicDesc.h"

// flying add here, to use math lib
#include <math.h>

const char * g_MagicID2String(int nAttrib);

extern int GetRandomNumber(int nMin, int nMax);

#define		MAX_FOLLOW_DISTANCE		48
#define		FOLLOW_WALK_DISTANCE	100

#define		AUTOFINDPATH	"\\settings\\mappos\\"

KNpcAI NpcAI;
KSubWorldSet g_SubWorldSetA;

KNpcAI::KNpcAI()
{//构造函数
	m_nIndex = 0;
	m_bActivate = TRUE;

	nPickState = PICCK_NONE;
}

KNpcAI::~KNpcAI()
{//析构函数

}

void KNpcAI::Activate(int nIndex)
{
	m_nIndex = nIndex;	
    

	if (Npc[m_nIndex].IsPlayer())
	{
		// 新增的Player AI在这里调用实现。更新个人攻城信息
		g_ScenePlace.setIsPaint(true);
		ProcessPlayer();
		return;
	}
	if (Npc[m_nIndex].m_Kind > 3 && Npc[m_nIndex].m_AiMode > 10)  //敌人关系
	{
		if (CanShowNpc())
		{
			// 让NPC随机休息一下，是个好建议。
			if (GetRandomNumber(0, 1))	
			{
				Npc[m_nIndex].m_AiParam[5] = 0;
				Npc[m_nIndex].m_AiParam[4] = 5;
				return;
			}
			if (!KeepActiveRange())
				ProcessShowNpc();
		}
	}
}
//---------------------------------------------------------------------
// flying add these functions
// Run at client.
// 仅仅有画面效果的NPC
int KNpcAI::ProcessShowNpc()
{
    int nResult  = false;
    int nRetCode = false;

	switch (Npc[m_nIndex].m_AiMode)
	{
	// 飞鸟型
	case 11:
		nRetCode = ShowNpcType11();
        if (!nRetCode)
            goto Exit0;
		break;
	// 蜻蜓型
	case 12:
		nRetCode = ShowNpcType12();
        if (!nRetCode)
            goto Exit0;
		break;
	// 鱼类型
	case 13:
		nRetCode = ShowNpcType13();
        if (!nRetCode)
            goto Exit0;
		break;
	// 老鼠型
	case 14:
		nRetCode = ShowNpcType14();
        if (!nRetCode)
            goto Exit0;
		break;
	// 鸡犬型
	case 15:
		nRetCode = ShowNpcType15();
        if (!nRetCode)
            goto Exit0;
		break;
	// 兔子型
	case 16:
		nRetCode = ShowNpcType16();
        if (!nRetCode)
            goto Exit0;
		break;
	// 蝴蝶型
	case 17:
		nRetCode = ShowNpcType17();
        if (!nRetCode)
            goto Exit0;
		break;
	default:
		break;
	}

    nResult = true;
Exit0:
	return nResult;
}

// 飞鸟型
int KNpcAI::ShowNpcType11()
{
    int nResult = false;
    int nRetCode = false;

	KNpc& aNpc = Npc[m_nIndex];
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	
	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	
    if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	
    nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	
    // 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
    if (!nRetCode)
        goto Exit0;

	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);	

    nResult = true;
Exit0:
	return nResult;
}

// 蜻蜓型
// done
int KNpcAI::ShowNpcType12()
{
    int nResult = false;
    int nRetCode = false;

	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);
		
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;

	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鱼类型
// done
int KNpcAI::ShowNpcType13()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 老鼠型
// done
int KNpcAI::ShowNpcType14()
{
	int nResult  = false;
	int nRetCode = false;

	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
    int	nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nRandom = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	nRandom = GetRandomNumber(1, 10);
	// 掉头就跑
	if (nRandom < 4)
		nDistance = -nDistance;
	// 嗷嗷发呆
	else if (nRandom < 7)
	{
		aNpc.SendSerCommand(do_stand);
		goto Exit0;
	}
	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);
	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	aNpc.m_Dir += nOffsetDir;
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 鸡犬型
int KNpcAI::ShowNpcType15()
{
	int nResult  = false;
	int nRetCode = false;
	// Go the distance between P1 to P2	
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
    int nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		DoShowFlee(nIndex);
		goto Exit0;
	}
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);

	nResult = true;
Exit0:
	return nResult;
}

// 兔子型
int KNpcAI::ShowNpcType16()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	register int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
	int nCurY = 0;
int	nCmap=0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	int nIndex = 0;
	KNpc& aNpc = Npc[m_nIndex];

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);

	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);
	// 附近有玩家
	nIndex = IsPlayerCome();
	if (nIndex > 0)
	{
		// do flee
		nRetCode = DoShowFlee(nIndex);
		if (!nRetCode)
			goto Exit0;		
		goto Exit1;
	}

	// 计算距离
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}

	// 计算新角度
	//if (KeepActiveShowRange())
	//	aNpc.m_Dir += 32;
	aNpc.m_Dir += GetRandomNumber(0, 6);
	aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);

Exit1:	
	nResult = true;
Exit0:
	return nResult;
}

// 蝴蝶型
int KNpcAI::ShowNpcType17()
{
	int nResult  = false;
	int nRetCode = false;

	// Go the distance between P1 to P2
	int nDistance = 0;
	int nDesX = 0;
	int nDesY = 0;
	int nCurX = 0;
int	nCmap=0;
	int nCurY = 0;
	int nOffX = 0;
	int nOffY = 0;
	int nOffsetDir = 0;
	KNpc& aNpc = Npc[m_nIndex];

	// 效果加强 随机调整高度
	aNpc.m_Height = GetRandomNumber(aNpc.m_AiParam[6] - 4, aNpc.m_AiParam[6]);

	aNpc.GetMpsPos(&nCurX, &nCurY,&nCmap);
		
	// 计算新角度 和 距离
	if (aNpc.m_AiParam[3] > 0)
		nOffsetDir = GetRandomNumber(aNpc.m_AiParam[3], aNpc.m_AiParam[2]);
	else
		nOffsetDir = aNpc.m_AiParam[2];
	if (GetRandomNumber(0, 1))
		nOffsetDir = -nOffsetDir;
	nDistance = GetRandomNumber(aNpc.m_AiParam[0] - aNpc.m_AiParam[1], aNpc.m_AiParam[0]);	

	// 取得运动所需的时间，保存在参数表中
	if (aNpc.m_CurrentWalkSpeed > 0)
	{
		aNpc.m_AiParam[4] = (int) nDistance / aNpc.m_CurrentWalkSpeed;
		aNpc.m_AiParam[5] = 0;
	}
	else
	{
		aNpc.m_AiParam[4] = 0;
		aNpc.m_AiParam[5] = 0;
	}
	if (KeepActiveRange())
	{
		//aNpc.SendCommand(do_walk, aNpc.m_OriginX, aNpc.m_OriginY);
		goto Exit0;
		//aNpc.m_Dir += 32;
	}
	aNpc.m_Dir += nOffsetDir;
	//aNpc.m_Dir += GetRandomNumber(32, 64);
	if (aNpc.m_Dir < 0)
		aNpc.m_Dir += 64;
	else
		aNpc.m_Dir %= 64;
	// 根据三角函数计算偏移的X、Y数值
	nRetCode = GetNpcMoveOffset(aNpc.m_Dir, nDistance, &nOffX, &nOffY);
	if (!nRetCode)
		goto Exit0;
	// 获取目标坐标
	nDesX = nCurX + nOffX;
	nDesY = nCurY + nOffY;
	aNpc.SendSerCommand(do_walk, nDesX, nDesY);
	
	nResult = true;
Exit0:
	return nResult;
}
//---------------------------------------------------------------------
// Player AI add here.
// flying comment
void KNpcAI::ProcessPlayer()
{
//客户端
	int i = Npc[m_nIndex].m_nPeopleIdx;//锁定人物

	if  (Npc[m_nIndex].m_randmove.nTime>0 && m_nIndex==Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		Npc[m_nIndex].m_nPeopleIdx=0;
		Npc[m_nIndex].m_nObjectIdx=0;
		CommonActionWithPlayer();
	    i=0;
		return;
	}
		    
	if (i > 0)
	{
		FollowPeople(i);  
	}

	i = Npc[m_nIndex].m_nObjectIdx; //锁定物品

	if (Npc[m_nIndex].m_AutoplayId<=0 && i > 0)
	{
		FollowObject(i);
	}
}

void KNpcAI::FollowObject(int nIdx)
{
	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nObjectIdx = 0;
		return;
	}

	int nX1, nY1,nMap1, nX2, nY2;

	Npc[m_nIndex].GetMpsPos(&nX1, &nY1,&nMap1);
	Object[nIdx].GetMpsPos(&nX2, &nY2);

	if ((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2) < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE)
	{
		Player[CLIENT_PLAYER_INDEX].CheckObject(nIdx);
		Npc[m_nIndex].m_nIsOver=TRUE;
	}

}

void KNpcAI::FollowPeople(int nIdx)
{
	if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}

	if (Npc[m_nIndex].m_Doing == do_death || Npc[m_nIndex].m_Doing == do_revive)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}

	if (Npc[m_nIndex].m_AutoplayId)
	{//挂机状态 自动使用技能状态就停止寻怪
		if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill)
		{
			Npc[m_nIndex].m_nPeopleIdx = 0;
			Npc[m_nIndex].m_nObjectIdx = 0;
			return;
		}
	}


   /*
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand())
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>提示<color>:手上有绑定的东西!");
		Npc[m_nIndex].m_nPeopleIdx = 0;
		return;
	}*/ 
//    Player[CLIENT_PLAYER_INDEX].ExecuteScript("\\script\\system\\updatewar.lua","onwar","0","0",nIdx);
/*	
	char nPname[32];
	g_StrCpyLen(nPname,Npc[m_nIndex].Name,sizeof(Npc[m_nIndex].Name));
    g_StrCatLen(nPname,"的[镖车]",sizeof("的[镖车]"));
	int nReg;
    nReg=strcmpi(nPname, Npc[nIdx].Name); //这个对比不分字母的大小

    if ((Npc[nIdx].m_btRankFFId==88 && Npc[m_nIndex].m_nPKFlag==0)|| nReg==0) //或者是自己的镖车
	return;
   *///是镖车

	// 取得到目标的距离
	int distance = NpcSet.GetDistance(nIdx, m_nIndex);
	int	nRelation = NpcSet.GetRelation(m_nIndex, nIdx);  //获取与对方的关系

	// 小于对话半径就开始对话
	  if ((Npc[nIdx].m_Kind == kind_dialoger))
	  { 
		if (distance <= Npc[nIdx].m_DialogRadius)
		{
			int x, y;
			SubWorld[Npc[m_nIndex].m_SubWorldIndex].NewMap2Mps(Npc[m_nIndex].m_RegionIndex, Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, Npc[m_nIndex].m_OffX, Npc[m_nIndex].m_OffY, &x, &y);
			Npc[m_nIndex].SendSerCommand(do_walk, x,y);
			
			// Send Command to Server
			SendClientCmdWalk(x,y);
			Player[CLIENT_PLAYER_INDEX].DialogNpc(nIdx);//打开对话框
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
			Npc[nIdx].TurnTo(Player[CLIENT_PLAYER_INDEX].m_nIndex);
			
			return;
		}
	  } 
		// 摆摊
		if (Npc[nIdx].m_Kind == kind_player && Npc[nIdx].m_BaiTan && distance <= Npc[nIdx].m_DialogRadius)
		{
			int x, y;
			SubWorld[Npc[m_nIndex].m_SubWorldIndex].NewMap2Mps(Npc[m_nIndex].m_RegionIndex, Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY, Npc[m_nIndex].m_OffX, Npc[m_nIndex].m_OffY, &x, &y);
			Npc[m_nIndex].SendSerCommand(do_walk, x,y);
			// Send Command to Server
			SendClientCmdWalk(x, y);
			Player[CLIENT_PLAYER_INDEX].OpenBaiTan(Npc[nIdx].m_dwID);//打开摆摊面板
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
			Npc[nIdx].TurnTo(Player[CLIENT_PLAYER_INDEX].m_nIndex);	
			return;
		}
	///PK关闭 就不攻击了！
//	if (Npc[nEnemyIdx].m_FightMode==0)
//	{
//		Npc[m_nIndex].m_nPeopleIdx = 0;
//		nEnemyIdx=0;
//	}

	// 距离小于攻击范围就开始攻击
	if (nRelation == relation_enemy) //如果是敌人关系 就攻击
	{
       	if (Npc[nIdx].m_Kind == kind_player && Npc[nIdx].m_FightMode==0)
		{
			return;
		}

		if  (Npc[m_nIndex].m_randmove.nTime>0 || Npc[nIdx].m_Doing == do_jump)
		{//混乱状态
			Npc[m_nIndex].m_nPeopleIdx = 0;
		    //CommonAction();
			return;
		}

		//自动调整攻击距离
		if (!Npc[m_nIndex].m_AutoplayId)
		{//非挂机状态
			if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum>0 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum%2==0)
			{//每2秒调整一次攻击距离
			   int nSkillMagic =0;
			   if  (Npc[m_nIndex].m_ActiveSkillID>0)
			       nSkillMagic = Npc[m_nIndex].m_ActiveSkListIndex;//Npc[m_nIndex].m_SkillList.FindSame(Npc[m_nIndex].m_ActiveSkillID);
			   else
				   nSkillMagic = Npc[m_nIndex].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());

			   if (Npc[nIdx].m_CurrentLife == Npc[nIdx].m_CurrentLifeMax)
			   {//卡怪了-直没掉血的
				 //Npc[m_nIndex].m_CurrentAttackRadius -=18;
				   Npc[m_nIndex].ChangeSkillAttackRadius(nSkillMagic,-18);
			   }

			   if (Npc[nIdx].m_CurrentLife == Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife)
			   {//没掉血 -一直没掉血的
				   //Npc[m_nIndex].m_CurrentAttackRadius -=18;
				   Npc[m_nIndex].ChangeSkillAttackRadius(nSkillMagic,-18);
			   }

			   if (Npc[nIdx].m_CurrentLife > Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife)
			   {//回血
				   //Npc[m_nIndex].m_CurrentAttackRadius -=18;
				   Npc[m_nIndex].ChangeSkillAttackRadius(nSkillMagic,-18);
			   }
			   //Npc[m_nIndex].OnDrag();
			}


			/*char nMSG[64];
			t_sprintf(nMSG,"--当前攻击距离:--%d--生命:%d",Npc[m_nIndex].m_CurrentAttackRadius,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife);		 
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMSG);
			*/

		}

		if (distance <= Npc[m_nIndex].m_CurrentAttackRadius)
		{
			Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, nIdx);
			SendClientCmdSkill(Npc[m_nIndex].m_ActiveSkillID, -1, Npc[nIdx].m_dwID);

			if (!Npc[m_nIndex].m_AutoplayId)
			{//非挂机状态,使用技能后  保存NPC的当前血量
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=Npc[nIdx].m_CurrentLife;
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum++;
			}
		}
		// 嗷嗷追
		else
		{
			int nDesX, nDesY,nDmap;
			Npc[nIdx].GetMpsPos(&nDesX, &nDesY,&nDmap);

			// modify by spe 2003/06/13
			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
			{
				Npc[m_nIndex].SendSerCommand(do_run, nDesX, nDesY);			
				SendClientCmdRun(nDesX, nDesY);
			}
			else
			{
				Npc[m_nIndex].SendSerCommand(do_walk, nDesX, nDesY);
				SendClientCmdWalk(nDesX, nDesY);
			}
			if (!Npc[m_nIndex].m_AutoplayId)
			{//非挂机状态,使用技能后  保存NPC的当前血量
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
				Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			}
		}
		return;
	}
	// 跟随
	if (Npc[nIdx].m_Kind == kind_player)
	{
		int nDesX, nDesY,nDmap;
		if (distance < MAX_FOLLOW_DISTANCE)
		{
			Npc[this->m_nIndex].GetMpsPos(&nDesX, &nDesY,&nDmap);
			Npc[m_nIndex].SendSerCommand(do_walk, nDesX, nDesY);
			SendClientCmdWalk(nDesX, nDesY);
		}
		else
		{
			Npc[nIdx].GetMpsPos(&nDesX, &nDesY,&nDmap);
			if (distance < FOLLOW_WALK_DISTANCE ||
				!Player[CLIENT_PLAYER_INDEX].m_RunStatus)
			{
				Npc[m_nIndex].SendSerCommand(do_walk, nDesX, nDesY);
				SendClientCmdWalk(nDesX, nDesY);
			}
			else
			{
				Npc[m_nIndex].SendSerCommand(do_run, nDesX, nDesY);			
				SendClientCmdRun(nDesX, nDesY);
			}
		}
	}
	return;
}


void KNpcAI::TriggerMapTrap()     //人物的脚本trap点
{
	Npc[m_nIndex].CheckTrap();
}

void KNpcAI::TriggerObjectTrap()  //物品的脚本trap点
{
	return;
}

//设置当前坐标为起点坐标
void KNpcAI::Setscpos(int m_nIndex,int x,int y)
{
	int xxa;
	int xyy;
	int xMap;
	Npc[m_nIndex].GetMpsPos(&xxa,&xyy,&xMap);   //像素坐标
 
//	Npc[m_DesY].m_MapY, 
    Npc[m_nIndex].m_OriginX=xxa;	
    Npc[m_nIndex].m_OriginY=xyy;
}

//设置当前坐标为起点坐标
void KNpcAI::SetCurpos(int m_nIndex,int x,int y)
{ 
//	Npc[m_DesY].m_MapY, 
    Npc[m_nIndex].m_OriginX=x;	
    Npc[m_nIndex].m_OriginY=y;
}

void KNpcAI::MoveJiaoBen(int m_Index)
{
    if (Npc[m_Index].m_Doing == do_death || Npc[m_Index].m_Doing == do_revive)
	{
		Npc[m_Index].m_nPeopleIdx = 0;
		Npc[m_Index].m_AutoplayId=0;
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
		return;
	}

    int xx,yy,yMmap,dMap,nDistance,nDistancea,nDesMpsX,nDesMpsY,mDesMpsX,mDesMpsY;

	    Npc[m_Index].GetMpsPos(&xx, &yy,&yMmap);

    /*KTabFile nPos;
	   char nFiledPath[128];
	   t_sprintf(nFiledPath,"\\FsAi\\%s_Pos.txt",Npc[m_Index].Name);

	   if (!nPos.Load(nFiledPath))
	   {
	      Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("脚本出错,停止挂机。");
		  return;
       } */

    int mRows= Player[CLIENT_PLAYER_INDEX].nPos.GetHeight();

 if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo==0)
 {
   for (int i=1;i<mRows;++i)
   {
    if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum==0) 
	{
	    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=1;
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurStaus=0;
    }
     

	 Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,2,0,&nDesMpsX);
     Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,3,0,&nDesMpsY);

	 Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(i,2,0,&mDesMpsX);
     Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(i,3,0,&mDesMpsY);

     nDistance = SubWorld[0].GetDistance(xx,yy,nDesMpsX,nDesMpsY);

     nDistancea = SubWorld[0].GetDistance(xx,yy,mDesMpsX,mDesMpsY);
     
	  if (nDistancea<nDistance)  //如果小于最小值
	     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=i;
	 else
	    continue;
   }
 }
    //得到了距离最小距离的 坐标
    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=1;
    char msg[64]={0};
    Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,2,0,&nDesMpsX);
    Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,3,0,&nDesMpsY);

    
    //nPos.Clear();


    if (nDesMpsX<=0 || nDesMpsY<=0)
	{
	      /*char msg[64];
		  t_sprintf(msg,"脚本(X:%s,Y:%d)出错,(%d)停止挂机。",nDesMpsX,nDesMpsY,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum);
	      Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);*/
		  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=0;
		  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		  Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
		  return;
	}

       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurXpos=nDesMpsX;
       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurYpos=nDesMpsY;


	//t_sprintf(msg,"提示:当前(%d)坐标,X: %d,Y:%d",Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,xx,yy);
    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

    //if (!ai_PathFinder.CheckDistance(xx>>10, yy >>10, nDesMpsX, nDesMpsY, Npc[m_Index].m_CurrentRunSpeed)) //m_CurrentRunSpeed
    //if (((xx-nDesMpsX >=0 && xx-nDesMpsX<=10) && (yy-nDesMpsY >=0 && yy-nDesMpsY<=10)) || ((xx-nDesMpsX <0 && xx-nDesMpsX>=-10) && (yy-nDesMpsY <0 && yy-nDesMpsY>=-10)))  //走到了这个点
   
     //GotoWhere(nDesMpsX,);

	if (AiCheckDistance(xx,yy,nDesMpsX,nDesMpsY,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentRunSpeed))  //16
	{//跑到了某个点
	   //t_sprintf(msg,"提示:走到目标,第 %d 个坐标点",Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum);
       //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop = 0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurXpos=nDesMpsX;
       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurYpos=nDesMpsY;
       //SetCurpos(m_Index,nDesMpsX,nDesMpsY);
	  if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum<mRows && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurStaus==0)
	      Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum +=1; //设置下一个点的坐标
	  else
	  {//走到终点后 往回走
	     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurStaus=1;
	     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum -=1; //设置下一个点的坐标  往回走

		 if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum==0)
		 {
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=1;
            Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurStaus=0;
		 }
	  }
	   
	   return;
    }

	/*Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop ++;
	int nRunTime   = Player[CLIENT_PLAYER_INDEX].m_Autoplay.nRunLoop;
	int nForBitTime= Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime;

	if  (nForBitTime<=0)
		nForBitTime =9;

	if (nRunTime>nForBitTime*18 && nRunTime%(nForBitTime*18)==0)
	{//跑得太久了,还没有跑到.不跑了,就直接飞到哪里了
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=gyellow>助手<color>:人物卡地图,变更直飞成功!");
		return;
	}*/
 //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=gyellow>助手<color>:跑向目标中　!");
 if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
 {   
    if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
	{
          Npc[m_Index].SendSerCommand(do_run,nDesMpsX,nDesMpsY);  //跑向目标
	      SendClientCmdRun(nDesMpsX,nDesMpsY); //跑向目标
	}
	else
	{
		  Npc[m_Index].SendSerCommand(do_walk,nDesMpsX,nDesMpsY);  //跑向目标
	      SendClientCmdWalk(nDesMpsX,nDesMpsY); //跑向目标
	}
   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
   Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;

}


}

void KNpcAI::MoveYaBiao(int m_Index)
{
    if (Npc[m_Index].m_Doing == do_death || Npc[m_Index].m_Doing == do_revive)
	{
		Npc[m_Index].m_nPeopleIdx = 0;
		Npc[m_Index].m_AutoplayId=0;
		return;
	}

    int xx,yy,Mmap,nDistance,nDistancea,nDesMpsX,nDesMpsY,mDesMpsX,mDesMpsY;

	    Npc[m_Index].GetMpsPos(&xx, &yy,&Mmap);

    //KTabFile nPos;
	//   nPos.Load("\\settings\\mappos\\biao_pos.txt");
  int mRows= Player[CLIENT_PLAYER_INDEX].nYaBiao.GetHeight();

  if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo==0)
     for (int i=1;i<mRows;++i)
     { 
        if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum==0) 
	    {
     //nPos.GetInteger(1,2,0,&nDesMpsX);
     //nPos.GetInteger(1,3,0,&nDesMpsY);

	 //nPos.GetInteger(i+1,2,0,&mDesMpsX);
     //nPos.GetInteger(i+1,3,0,&mDesMpsY);

     //nDistance = SubWorld[0].GetDistance(xx,yy,nDesMpsX,nDesMpsY);
     //nDistancea = SubWorld[0].GetDistance(xx,yy,mDesMpsX,mDesMpsY);

     //if (nDistance<nDistancea)
	      Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=1;
	 //else
	 // Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=i+1;

	      continue;
        }
     

	 Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,2,0,&nDesMpsX);
     Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,3,0,&nDesMpsY);

	 Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(i,2,0,&mDesMpsX);
     Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(i,3,0,&mDesMpsY);

     nDistance = SubWorld[0].GetDistance(xx,yy,nDesMpsX,nDesMpsY);

     nDistancea = SubWorld[0].GetDistance(xx,yy,mDesMpsX,mDesMpsY);
     
	  if (nDistancea<nDistance)  //如果小于最小值
	     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=i;
	 else
	    continue;
   }

    //得到了距离最小距离的 坐标
    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=1;
    //char msg[64]={0};
    Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,2,0,&nDesMpsX);
    Player[CLIENT_PLAYER_INDEX].nYaBiao.GetInteger(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,3,0,&nDesMpsY);
    
    //nPos.Clear();

	 //t_sprintf(msg,"提示:当前(%d)坐标,X: %d,Y:%d",Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum,xx,yy);
     //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);

    //if (!ai_PathFinder.CheckDistance(xx>>10, yy >>10, nDesMpsX, nDesMpsY, Npc[m_Index].m_CurrentRunSpeed)) //m_CurrentRunSpeed
    //if (((xx-nDesMpsX >=0 && xx-nDesMpsX<=10) && (yy-nDesMpsY >=0 && yy-nDesMpsY<=10)) || ((xx-nDesMpsX <0 && xx-nDesMpsX>=-10) && (yy-nDesMpsY <0 && yy-nDesMpsY>=-10)))  //走到了这个点
   
     //GotoWhere(nDesMpsX,);

	if  (nDesMpsX<=0 ||	nDesMpsY<=0)
	{
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=0;
		return;
	}

	if (AiCheckDistance(xx,yy,nDesMpsX,nDesMpsY,Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentRunSpeed))//16
	{
	 if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum<mRows)
	     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum +=1;     //设置下一个点的坐标
	   
	   return;
    }
    
    if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
	{
		  Npc[m_Index].SendSerCommand(do_run, nDesMpsX, nDesMpsY);			
		  SendClientCmdRun(nDesMpsX, nDesMpsY);
	}
	else
	{
		   Npc[m_Index].SendSerCommand(do_walk,nDesMpsX,nDesMpsY);  //跑向目标
	       SendClientCmdWalk(nDesMpsX,nDesMpsY); //跑向目标
	}
}

void KNpcAI::GotoWhere(int x, int y, int mode) //鼠标点击的坐标
{
	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) || mode == 2)
			bRun = true;
         
		int nX = x;
		int nY = y;
		int nZ = 0;

		//g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);  //转成三维空间坐标	鼠标坐标 转换成场景中的像素坐标
		SubWorld[0].GetMpsByLocalPosition(nX, nY, nZ); 
		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;


		//需要检测 障碍  焦点的四个方位检

		if (!bRun)
		{
			Npc[nIndex].SendSerCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendSerCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}

		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

void KNpcAI::mainPickItem(int m_Index)  //攻击按钮
{
	if (Npc[m_Index].m_AutoplayId==1)
		return;

	if (Npc[m_Index].m_Doing == do_death || Npc[m_Index].m_Doing == do_revive/* || Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill*/)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	if  (Npc[m_Index].m_SubWorldIndex < 0 || Npc[m_Index].m_RegionIndex < 0 || Npc[m_Index].m_randmove.nTime >0/* Npc[m_Index].m_Hide>0*/)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	/*if 	(Npc[m_Index].m_nObjectIdx>0 && SubWorld[Npc[m_Index].m_SubWorldIndex].m_SubWorldID !=SubWorld[Npc[Npc[m_Index].m_nPeopleIdx].m_SubWorldIndex].m_SubWorldID)
	{//地图不相同的 
		//Npc[Npc[m_Index].m_nPeopleIdx].m_IsbeSel  = 0;
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}
	*/
	gameAutoplaerInfo nAutoinfo;
	nAutoinfo.isopenmoney      = 0;
	nAutoinfo.insopenzhuang    = 1;
	nAutoinfo.ismonyeandzhuang = 0;
	nAutoinfo.isopentuzhi      = 1;
	nAutoinfo.isopendaoju      = 1;

	if  (Npc[m_Index].m_nObjectIdx<=0)
	{
		int CurState = 0;
		int nObjectIdx =  GetNearestObject(m_Index,(BYTE*)&nAutoinfo,&CurState);//获取地上周围物品的索引
		if (nObjectIdx>0)
		{   
			Npc[m_Index].m_nPeopleIdx=0;
			//nPickState = CurState;//PICCK_MONEY;
			/*switch(CurState)
			{
			case 0:
				{
					nPickState = PICCK_NONE;
				}
				break;
			case 1:
				nPickState = PICCK_MONEY;
				break;
			case 2:
				nPickState = PICCK_ITEM;
				break;
			case 3:
				nPickState = PICCK_ITEM_MONEY;
				break;
			case 4:
				nPickState = PICCK_TUPU;
				break;
			case 5:
				nPickState = PICCK_TOOL;
				break;
			default:
				{
					nPickState = PICCK_NONE;
					break;
				}
			}*/

			Npc[m_Index].m_nObjectIdx=nObjectIdx;
		}//end if 
	}
	

	if  (Npc[m_Index].m_nObjectIdx>0)
	{
		//CCMessageBox("找到物品","test");
		int xx,yy,Mmap,nMapIdx;
		Npc[m_Index].GetMpsPos(&xx, &yy,&Mmap);

		int x, y,nObjectIdx;
		nObjectIdx = Npc[m_Index].m_nObjectIdx;

		Object[nObjectIdx].GetMpsPos(&x, &y);
		if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
		{   
			Npc[m_Index].SendSerCommand(do_run,x,y);  //跑向目标
			SendClientCmdRun(x,y); //跑向目标
		}  
		else
		{  
			Npc[m_Index].SendSerCommand(do_walk,x,y);  //跑向目标
			SendClientCmdWalk(x,y); //跑向目标
		} 

		if (((xx - x) * (xx - x) + (yy - y) * (yy - y))<(PLAYER_PICKUP_CLIENT_DISTANCE*PLAYER_PICKUP_CLIENT_DISTANCE))
		{  	

			/*if  (nPickState==PICCK_ITEM_MONEY)
				Player[CLIENT_PLAYER_INDEX].CheckObject(nObjectIdx);
			else if  (nPickState==PICCK_MONEY)
			{
				Player[CLIENT_PLAYER_INDEX].PickUpMoney(nObjectIdx,m_Index);
			}
			else */
		    Player[CLIENT_PLAYER_INDEX].PickUpItem(nObjectIdx,m_Index);

			//nPickState=PICCK_NONE;
			Npc[m_Index].m_nObjectIdx = 0;
		}
	}
}

void KNpcAI::mainSkillattack(int m_Index,int m_Kind)  //攻击按钮
{
	if (Npc[m_Index].m_Doing == do_death || Npc[m_Index].m_Doing == do_revive || Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		//Npc[m_Index].m_AutoplayId=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	if  (Npc[m_Index].m_SubWorldIndex < 0 || Npc[m_Index].m_RegionIndex < 0)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	if 	(Npc[m_Index].m_nPeopleIdx>0 && SubWorld[Npc[m_Index].m_SubWorldIndex].m_SubWorldID !=SubWorld[Npc[Npc[m_Index].m_nPeopleIdx].m_SubWorldIndex].m_SubWorldID)
	{//地图不相同的 
		Npc[Npc[m_Index].m_nPeopleIdx].m_IsbeSel  = 0;
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}
//----------------------------------------------   
	KSkill * pCurSkill = (KSkill *)g_SkillManager.GetSkill(Npc[m_Index].m_ActiveSkillID,1);
	if (pCurSkill)
	{
							 int nHorse=0,nIsHorse;
							     nHorse= pCurSkill->GetHorseLimited();
							 nIsHorse=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse;
							 if(nHorse) //1(不能骑马使用) 2(需要骑马使用)
							 {//如果有骑马限制的
								 if (nHorse==1)
								 {
									 if (nIsHorse)
									 {//骑马状态,则下马
										 SendClientCmdRide(); //骑马发送函数
									 }
								 }
								 
								 if (nHorse==2)
								 {
									 if (!nIsHorse)
									 {//非骑马状态,则上马
										 SendClientCmdRide(); //骑马发送函数	
									 }
								 }	
							 }
	}	
//----------------------------------------------

   if(Npc[m_Index].m_FightMode == 0)
   { 

	 if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
	 {//如果是骑马状态,就下马再 打坐
		   SendClientCmdRide(); //上下马发送函数
	 }

     Npc[m_Index].m_nPeopleIdx = 0;
     Npc[m_Index].m_nObjectIdx=0;
	 if (!Npc[m_Index].m_AutoplayId){
     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
	 }
	 Npc[m_Index].SendSerCommand(do_sit);
     SendClientCmdSit(TRUE);
	 return;
   }

	int xx,yy,Mmap,nMapIdx;
	Npc[m_Index].GetMpsPos(&xx, &yy,&Mmap);

   if (Npc[m_Index].m_randmove.nTime>0)                          //混乱状态不能发招
   {
	   Npc[m_Index].m_nPeopleIdx=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
	   //Npc[m_Index].m_nObjectIdx=0;
	   return;
   }

   if (Npc[m_Index].m_nPeopleIdx>0 && Npc[Npc[m_Index].m_nPeopleIdx].m_Doing == do_jump)
   {//使用轻功
	   Npc[m_Index].m_nPeopleIdx=0;
	   if (!Npc[m_Index].m_AutoplayId)
	   {
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
	   }
	   //Npc[m_Index].m_nObjectIdx=0;	Npc[nIdx].m_Doing == do_jump
	   return;
   }
//  锁定到目标物品
//  int nObjectIdx =  GetNearestObject(relation_enemy,m_Index);//获取地上周围物品的索引
//  Npc[m_Index].m_nObjectIdx=nObjectIdx; 
    int nEnemyIdx = GetNearestCharacter(relation_enemy,m_Index,0,180); //获取周围敌对的NPC编号
	if (nEnemyIdx<=0)
	{//如果附近没有就全屏查找
       nEnemyIdx = GetNearestCharacter(relation_enemy,m_Index); //获取周围敌对的NPC编号
	}

	/*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw <=0)
	{
        Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=Npc[nEnemyIdx].m_dwID;
	}
	else
	{
		nEnemyIdx = GetNearestCharacter(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw,m_Index,2,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcDis);

		if  (nEnemyIdx<=0)
		{    
			nEnemyIdx = GetNearestCharacter(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw,m_Index,2); //获取周围敌对的NPC编号
			if (nEnemyIdx<=0)
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
		}

		Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人
	}*/
	Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人

	int nSkillMagic = 0;

	//------------------------
	//if (Npc[m_Index].m_CurrentMana>=10 && Npc[m_Index].m_CurrentLife>=10)
	if (!m_Kind)
	{//左键  
		nSkillMagic = Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx();//Npc[m_Index].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());
		Npc[m_Index].SetActiveSkill(nSkillMagic);
	}  
	else
	{//右键   
		nSkillMagic = Player[CLIENT_PLAYER_INDEX].GetRightSkillListidx();//Npc[m_Index].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetRightSkill());
		Npc[m_Index].SetActiveSkill(nSkillMagic);
	}
	
    int distance;

    if (nEnemyIdx>0)
	{
		//------------------------临时保护
		/*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsFell)
		{
		  	//FleeEnemy(nEnemyIdx);
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:<color=gyellow>开始逃离敌人<color>");
			//Npc[m_Index].m_nPeopleIdx = 0;
			//return;
		}*/

		if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive)
		{
			Npc[m_Index].m_nPeopleIdx = 0;
			Npc[nEnemyIdx].m_IsbeSel  = 0;
			//正在攻击中
			if (!Npc[m_Index].m_AutoplayId)
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;

			return;
		}

	    //distance = NpcSet.GetDistance(nEnemyIdx, m_Index);
	   //检测只能调整攻击距离
         //每秒调整下攻击距离
		/*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife==Npc[nEnemyIdx].m_CurrentLife)
		{
			  // Npc[m_Index].m_CurrentAttackRadius -=18;
			Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		}

	    if (Npc[nEnemyIdx].m_CurrentLife == Npc[nEnemyIdx].m_CurrentLifeMax)
		{//卡怪了
			   //Npc[m_Index].m_CurrentAttackRadius -=18;
		    Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		}

	    if (Npc[nEnemyIdx].m_CurrentLife > Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife)
		{//回血的
			  // Npc[m_Index].m_CurrentAttackRadius -=18;
			Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		}
		*/

	}

	int	nRelation = NpcSet.GetRelation(m_Index, nEnemyIdx);

   if (nEnemyIdx==0)
	{
	     /*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel==1)
         {//脚本
            MoveJiaoBen(m_Index);
			return;
         }*/
		Npc[m_Index].SendSerCommand(do_stand); 
		///SendClientCmdRun(Npc[m_Index].m_OriginX, Npc[m_Index].m_OriginY);
		
		//int xxa,xyy,Xmap;
	    //Npc[m_Index].GetMpsPos(&xxa, &xyy,&Xmap);   //像素坐标

       /* if (AiCheckDistance(xxa,xyy,Npc[m_Index].m_OriginX,Npc[m_Index].m_OriginY,Npc[m_Index].m_CurrentRunSpeed))//16
    	{
			if (Npc[m_Index].m_bRideHorse)
			{//如果是骑马状态,就下马再 打坐
				SendClientCmdRide(); //上下马发送函数
			} 
	        Npc[m_Index].SendSerCommand(do_sit);
            SendClientCmdSit(TRUE);
    	}*/
			
		return;
	}
}

void KNpcAI::FollowCharacter(int m_Index,int fanwei, int juli,int nActiveNum)  //挂机
{
	if (Npc[m_Index].m_Doing == do_death || Npc[m_Index].m_Doing == do_revive || Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		//Npc[m_Index].m_AutoplayId=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	if  (Npc[m_Index].m_SubWorldIndex < 0 || Npc[m_Index].m_RegionIndex < 0)
	{
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}

	if 	(Npc[m_Index].m_nPeopleIdx>0 && SubWorld[Npc[m_Index].m_SubWorldIndex].m_SubWorldID !=SubWorld[Npc[Npc[m_Index].m_nPeopleIdx].m_SubWorldIndex].m_SubWorldID)
	{//地图不相同的 
		Npc[Npc[m_Index].m_nPeopleIdx].m_IsbeSel  = 0;
		Npc[m_Index].m_nPeopleIdx=0;
		Npc[m_Index].m_nObjectIdx=0;
		return;
	}
//----------------------------------------------   
	KSkill * pCurSkill = (KSkill *)g_SkillManager.GetSkill(Npc[m_Index].m_ActiveSkillID,1);
	if (pCurSkill)
	{
							 int nHorse=0,nIsHorse;
							     nHorse= pCurSkill->GetHorseLimited();
							 nIsHorse=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse;
							 if(nHorse) //1(不能骑马使用) 2(需要骑马使用)
							 {//如果有骑马限制的
								 if (nHorse==1)
								 {
									 if (nIsHorse)
									 {//骑马状态,则下马
										 SendClientCmdRide(); //骑马发送函数
									 }
								 }
								 
								 if (nHorse==2)
								 {
									 if (!nIsHorse)
									 {//非骑马状态,则上马
										 SendClientCmdRide(); //骑马发送函数	
									 }
								 }	
							 }
	}	
//----------------------------------------------

   if(Npc[m_Index].m_FightMode == 0)
   { 

	 if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
	 {//如果是骑马状态,就下马再 打坐
		   SendClientCmdRide(); //上下马发送函数
	 }

     Npc[m_Index].m_nPeopleIdx = 0;
     Npc[m_Index].m_nObjectIdx=0;
     Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
	 Npc[m_Index].SendSerCommand(do_sit);
     SendClientCmdSit(TRUE);
	 return;
   }

	int xx,yy,Mmap,nMapIdx;
	Npc[m_Index].GetMpsPos(&xx, &yy,&Mmap);

   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel==1)
   {//脚本
     //if (Npc[m_Index].m_nPeopleIdx = 0)
      //   MoveJiaoBen(m_Index);
	   Player[CLIENT_PLAYER_INDEX].nPos.GetInteger(1,1,0,&nMapIdx);
	   
	   if  (nMapIdx!=SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID)
	   {
		   Npc[m_Index].m_nPeopleIdx=0;
		   Npc[m_Index].m_nObjectIdx=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:当前路线不是该地图,请重新录制路线!");
		   return;
		}

	   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum==0)
	   {
	      MoveJiaoBen(m_Index);
		  return;
	   }  

	     /*char msg[64];
	     int DisJuli = g_GetDistance(xx,yy,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurXpos,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurYpos);
	         t_sprintf(msg,"X:%d,Y:%d,距离:%d,范围:%d",Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurXpos,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurYpos,DisJuli,fanwei);
			 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);*/
			   
      if(KeepActiveJiaoBen(fanwei,m_Index,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurXpos,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurYpos)) //保持活动范围
         return;
   }
   else
   {//系统
	 if(KeepActiveCharacter(fanwei,m_Index,xx,yy)) //保持活动范围
       return;
   }

   if (Npc[m_Index].m_randmove.nTime>0)                          //混乱状态不能发招
   {
	   Npc[m_Index].m_nPeopleIdx=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
	   //Npc[m_Index].m_nObjectIdx=0;
	   return;
   }

   if (Npc[m_Index].m_nPeopleIdx>0 && Npc[Npc[m_Index].m_nPeopleIdx].m_AttackerDwid==Npc[m_Index].m_dwID && Npc[Npc[m_Index].m_nPeopleIdx].m_IsHaveAttack==1)
   {//如果攻击次数
     Npc[m_Index].m_nPeopleIdx=0;
	 Npc[Npc[m_Index].m_nPeopleIdx].m_IsbeSel  = 0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
	 Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
	 //Npc[m_Index].m_nObjectIdx=0;
	 return;
   }

   if (Npc[m_Index].m_nPeopleIdx>0 && Npc[Npc[m_Index].m_nPeopleIdx].m_Doing == do_jump)
   {//使用轻功
	   Npc[m_Index].m_nPeopleIdx=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
	   //Npc[m_Index].m_nObjectIdx=0;	Npc[nIdx].m_Doing == do_jump

	   return;
   }

   if (Npc[m_Index].m_nObjectIdx>0 && Object[Npc[m_Index].m_nObjectIdx].m_AttackerDwid==Npc[m_Index].m_dwID && Object[Npc[m_Index].m_nObjectIdx].m_IsHaveAttack==1)
   {//如果是本人丢的就不拾取的了
     Npc[m_Index].m_nObjectIdx=0;
	 //Npc[m_Index].m_nObjectIdx=0;
	 return;
   }

//  锁定到目标物品
//  int nObjectIdx =  GetNearestObject(relation_enemy,m_Index);//获取地上周围物品的索引
//  Npc[m_Index].m_nObjectIdx=nObjectIdx; 

   int nRetaMoney =  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney;           //钱
   int nRetaEques =  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques;           //装备
   int nRetaMoneyAndEques =  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques;   //钱和装备 
   int nRetaTuPu  =  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu;            //神秘图志
   BYTE nRetaTool =  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool;            //道具
//   char nsetionKey[32],nTempItemName[80];
   gameAutoplaerInfo nAutoinfo;
   nAutoinfo.isopenmoney      = nRetaMoney;
   nAutoinfo.insopenzhuang    = nRetaEques;
   nAutoinfo.ismonyeandzhuang = nRetaMoneyAndEques;
   nAutoinfo.isopentuzhi      = nRetaTuPu;
   nAutoinfo.isopendaoju      = nRetaTool;

   if ((nRetaMoney||nRetaEques||nRetaMoneyAndEques||nRetaTuPu||nRetaTool)  && nPickState == PICCK_NONE)
   {   
	   int CurState = 0;
	   int nObjectIdx =  GetNearestObject(m_Index,(BYTE*)&nAutoinfo,&CurState);//获取地上周围物品的索引

	   if (nObjectIdx>0)
	   {   
		   Npc[m_Index].m_nPeopleIdx=0;
		   //nPickState = CurState;//PICCK_MONEY;
		   switch(CurState)
		   {
		   case 0:
			   {
			     nPickState = PICCK_NONE;
		       }
			   break;
		   case 1:
			   nPickState = PICCK_MONEY;
			   break;
		   case 2:
			   nPickState = PICCK_ITEM;
			   break;
		   case 3:
			   nPickState = PICCK_ITEM_MONEY;
			   break;
		   case 4:
			   nPickState = PICCK_TUPU;
			   break;
		   case 5:
			   nPickState = PICCK_TOOL;
			   break;
		   default:
			   {
				   nPickState = PICCK_NONE;
				   break;
			   }
		   }

		   Npc[m_Index].m_nObjectIdx=nObjectIdx;
	   }
   }
   
   /*if (nRetaMoney==1  && nPickState == PICCK_NONE)
   { 
	   int nObjectIdx =  GetNearestObject(m_Index,PICCK_MONEY);//获取地上周围物品的索引
       
	   if (nObjectIdx>0)
	   {   
		   Npc[m_Index].m_nPeopleIdx=0;

		   nPickState = PICCK_MONEY;

		   Npc[m_Index].m_nObjectIdx=nObjectIdx;
	   }
   } 
   
   if (nRetaEques==1 && nPickState == PICCK_NONE)
   {   
	   int nObjectIdx =  GetNearestObject(m_Index,PICCK_ITEM);//获取地上周围物品的索引
       
	   if (nObjectIdx>0)
	   {   
		   Npc[m_Index].m_nPeopleIdx=0;

		   nPickState = PICCK_ITEM;
		   Npc[m_Index].m_nObjectIdx=nObjectIdx;
	   }
  }
   
  if (nRetaMoneyAndEques==1 && nPickState == PICCK_NONE)
  { 
	   int nObjectIdx =  GetNearestObject(m_Index,PICCK_ITEM_MONEY);//获取地上周围物品的索引
       
	   if (nObjectIdx>0)
	   {   
           Npc[m_Index].m_nPeopleIdx=0;

		   nPickState = PICCK_ITEM_MONEY;
		   Npc[m_Index].m_nObjectIdx=nObjectIdx;
	   }
  } 
//-----------------------------------------------图谱
  if (nRetaTuPu==1 && nPickState == PICCK_NONE)
  {   
	     t_sprintf(nTempItemName,UTEXT("神秘图志",1).c_str());
	     int nObjectIdx =  GetNearestObject(m_Index,-1,nTempItemName);//获取地上周围物品的索引
		 t_sprintf(nTempItemName,UTEXT("神秘路志",1).c_str());
		 int nObjectIdxa=  GetNearestObject(m_Index,-1,nTempItemName);

		 if (nObjectIdx>0)
		 {   
			 Npc[m_Index].m_nPeopleIdx=0;
			 Npc[m_Index].m_nObjectIdx=nObjectIdx;

			 nPickState = PICCK_TUPU;
		 }
		 else if (nObjectIdxa>0)
		 {
			 Npc[m_Index].m_nPeopleIdx=0;
			 Npc[m_Index].m_nObjectIdx=nObjectIdxa;

			 nPickState = PICCK_TUPU;
		 } 

  }
//----------------------------------------------------------------道具
  if (nRetaTool==1 && nPickState == PICCK_NONE)
  {   
	 char nItemName[64]={0},nTempItemName[64];
	 for(int i=0;i<16;++i)
	 { 	 
		 Player[CLIENT_PLAYER_INDEX].nToolItem.GetString(i+1,1,"无数据",nItemName,sizeof(nItemName)); 
	     int nObjectIdx =  GetNearestObject(m_Index,-1,nItemName);//获取地上周围物品的索引

		 if (nObjectIdx>0)
		 {
			 Npc[m_Index].m_nPeopleIdx=0;
			 Npc[m_Index].m_nObjectIdx=nObjectIdx;

			 nPickState = PICCK_TOOL;
			 break;
		 } 
	}
  }
  */
// 锁定到目标NPC
	if (Npc[m_Index].m_nObjectIdx>0 && nPickState!=PICCK_NONE)
	{//捡完东西再打怪
		AutoMovePickUp(m_Index,Npc[m_Index].m_nObjectIdx,xx,yy,nPickState);
		if (Npc[m_Index].m_nPeopleIdx>0)
		{
			Npc[Npc[m_Index].m_nPeopleIdx].m_IsbeSel  = 0;
		}
		Npc[m_Index].m_nPeopleIdx=0;
		return;
	}

    int nEnemyIdx = GetNearestCharacter(relation_enemy,m_Index,0,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcDis); //获取周围敌对的NPC编号
	if (nEnemyIdx<=0)
	{//如果附近没有就全屏查找
       nEnemyIdx = GetNearestCharacter(relation_enemy,m_Index); //获取周围敌对的NPC编号
	}

	/*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw <=0)
	{
        Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=Npc[nEnemyIdx].m_dwID;
	}
	else
	{
		nEnemyIdx = GetNearestCharacter(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw,m_Index,2,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcDis);

		if  (nEnemyIdx<=0)
		{    
			nEnemyIdx = GetNearestCharacter(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw,m_Index,2); //获取周围敌对的NPC编号
			if (nEnemyIdx<=0)
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
		}

		Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人
	}*/

	Npc[m_Index].m_nPeopleIdx=nEnemyIdx;  //锁定敌人并跑向敌人

	int nSkillMagic = 0;

	//------------------------
	if (Npc[m_Index].m_CurrentMana>=10 && Npc[m_Index].m_CurrentLife>=10)
	{  

		nSkillMagic = Player[CLIENT_PLAYER_INDEX].GetLeftSkillListidx();//Npc[m_Index].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());
		Npc[m_Index].SetActiveSkill(nSkillMagic);
	}  
	else
	{   
		nSkillMagic = Player[CLIENT_PLAYER_INDEX].GetRightSkillListidx();//Npc[m_Index].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetRightSkill());
		Npc[m_Index].SetActiveSkill(nSkillMagic);
	}
	
    int distance;

    if (nEnemyIdx>0)
	{
		//------------------------临时保护
		/*if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsFell)
		{
		  	//FleeEnemy(nEnemyIdx);
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:<color=gyellow>开始逃离敌人<color>");
			//Npc[m_Index].m_nPeopleIdx = 0;
			//return;
		}*/
		int nBackNpcIndex = Player[CLIENT_PLAYER_INDEX].m_Autoplay.nBackIndex;

		if (nBackNpcIndex>0 && nBackNpcIndex!=nEnemyIdx)
		{
			Npc[nBackNpcIndex].m_IsbeSel = 0;
		}

		Npc[nEnemyIdx].m_IsbeSel = 1;
		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nBackIndex = nEnemyIdx;

	    Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop++;

		int nLtime=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime;

		if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive)
		{
			Npc[m_Index].m_nPeopleIdx = 0;
			Npc[nEnemyIdx].m_IsbeSel  = 0;
			//正在攻击中
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			return;
		}

	    distance = NpcSet.GetDistance(nEnemyIdx, m_Index);

	   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop>=10000)
		     Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop=0;
         
       if (nLtime>0 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop>=(nLtime*18) && Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop%(nLtime*18)==0 && Npc[nEnemyIdx].m_IsHaveAttack==0)
	   {
	       if (Npc[nEnemyIdx].m_CurrentLife == Npc[nEnemyIdx].m_CurrentLifeMax)
		   {//卡怪了
		      Npc[nEnemyIdx].m_IsHaveAttack=1;             //是否设置为攻击无效了
              Npc[nEnemyIdx].m_AttackerDwid=Npc[m_Index].m_dwID; //上次攻击着的DWID
			  Npc[nEnemyIdx].m_IsbeSel  = 0;
              Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:原地打转,执行取消攻击!");
			  return;
		   }
	   }
	   //检测只能调整攻击距离
	   //if (Player[CLIENT_PLAYER_INDEX].m_isopenjuli)
	     if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum>0 && !(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum%18))
	     {//每秒调整下攻击距离
		   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife==Npc[nEnemyIdx].m_CurrentLife)
		   {
			  // Npc[m_Index].m_CurrentAttackRadius -=18;
			   Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		   }

	       if (Npc[nEnemyIdx].m_CurrentLife == Npc[nEnemyIdx].m_CurrentLifeMax)
		   {//卡怪了
			   //Npc[m_Index].m_CurrentAttackRadius -=18;
			   Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		   }

		   if (Npc[nEnemyIdx].m_CurrentLife > Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife)
		   {//回血的
			  // Npc[m_Index].m_CurrentAttackRadius -=18;
			   Npc[m_Index].ChangeSkillAttackRadius(nSkillMagic,-18);
		   }
	     }
	}

	int	nRelation = NpcSet.GetRelation(m_Index, nEnemyIdx);

    if (nEnemyIdx>0 && distance <= Npc[m_Index].m_CurrentAttackRadius)
	{//在攻击范围内
	//	Npc[m_Index].m_nPeopleIdx=nEnemyIdx; // 锁定某怪物
	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum++; 

	   Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngAttackLoop=0;

	   if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum>=10000)
		       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
         
       if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum >=(nActiveNum*18) && Npc[nEnemyIdx].m_IsHaveAttack==0)
	   {//攻击了多少秒后 
		   int  nTempLoop = Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime++;
           if (nTempLoop>=90 && nTempLoop%90==0 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife==Npc[nEnemyIdx].m_CurrentLife)
		   {//没回血的怪物 卡怪了
               Npc[nEnemyIdx].m_IsHaveAttack=1;             //是否设置为攻击无效了
			   Npc[nEnemyIdx].m_IsbeSel  = 0;
               Npc[nEnemyIdx].m_AttackerDwid=Npc[m_Index].m_dwID; //上次攻击着的DWID
               Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:攻击超时,执行取消攻击A!");
			   return;
		   }

		   if (nTempLoop>=36 && nTempLoop%36==0 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife>0 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife<Npc[nEnemyIdx].m_CurrentLife)
		   {//回血怪
			   Npc[nEnemyIdx].m_IsHaveAttack=1;                   //是否设置为攻击无效了
			   Npc[nEnemyIdx].m_IsbeSel  = 0;
			   Npc[nEnemyIdx].m_AttackerDwid=Npc[m_Index].m_dwID; //上次攻击着的DWID
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:怪物回血中,攻击超时,取消攻击!");
			   return;
		   }

	       if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum%10==0 && Npc[nEnemyIdx].m_CurrentLife == Npc[nEnemyIdx].m_CurrentLifeMax)
		   {//卡怪了
		      Npc[nEnemyIdx].m_IsHaveAttack=1;                   //是否设置为攻击无效了
              Npc[nEnemyIdx].m_IsbeSel = 0;
              Npc[nEnemyIdx].m_AttackerDwid=Npc[m_Index].m_dwID; //上次攻击着的DWID
              Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:攻击超时,执行取消攻击!");
			  return;
		   }
		   //打中了...
	   }
	   else if (Npc[nEnemyIdx].m_IsHaveAttack==0)//在攻击期间
	   {
            //正在攻击中
		   /*if (!Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum%18)
		   {//每秒检测一次
		   } */

	       Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=Npc[nEnemyIdx].m_CurrentLife;
	   }
	   else
		    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;

		return;
	}
	else if (nEnemyIdx==0)
	{
	     if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel==1)
         {//脚本
            MoveJiaoBen(m_Index);
			return;
         }

		Npc[m_Index].SendSerCommand(do_run, Npc[m_Index].m_OriginX, Npc[m_Index].m_OriginY); 
		SendClientCmdRun(Npc[m_Index].m_OriginX, Npc[m_Index].m_OriginY);
		
		int xxa,xyy,Xmap;

	    Npc[m_Index].GetMpsPos(&xxa, &xyy,&Xmap);   //像素坐标

        if (AiCheckDistance(xxa,xyy,Npc[m_Index].m_OriginX,Npc[m_Index].m_OriginY,Npc[m_Index].m_CurrentRunSpeed))//16
    	{
			if (Npc[m_Index].m_bRideHorse)
			{//如果是骑马状态,就下马再 打坐
				SendClientCmdRide(); //上下马发送函数
			} 
	        Npc[m_Index].SendSerCommand(do_sit);
            SendClientCmdSit(TRUE);
    	}
		return;
	}
}

int  KNpcAI::AutoMovePickUp(int nIndex ,int nObjIdx,int xx,int yy, AutoPickUp iPickState) 
{   
	     int x, y;
		 Object[nObjIdx].GetMpsPos(&x, &y);

		 if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
		 {   
            Npc[nIndex].SendSerCommand(do_run,x,y);  //跑向目标
		    SendClientCmdRun(x,y); //跑向目标
		 }  
		 else
		 {  
			Npc[nIndex].SendSerCommand(do_walk,x,y);  //跑向目标
			SendClientCmdWalk(x,y); //跑向目标
		 } 
		 
		 int nLtime=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime;
		 int nLoop;

		 if (nLtime>0)
		 {  	
			    if (iPickState==PICCK_MONEY)
					nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngMoneyLoop++;
			    else if (iPickState==PICCK_ITEM)
					nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop++;
			    else if  (iPickState==PICCK_ITEM_MONEY)
					nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop++;
				else if  (iPickState==PICCK_TOOL)
			        nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngToolLoop++;
				else if  (iPickState==PICCK_TUPU)
					nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngTuzhiLoop++;

			 
			    if (nLoop>nLtime && (nLoop%(nLtime*18)==0))
				{  
				   Object[nObjIdx].m_AttackerDwid=Npc[nIndex].m_dwID;
				   Object[nObjIdx].m_IsHaveAttack=1;
				   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:原地打转,执行取消捡取!");
				   
				   //if (iPickState==PICCK_ITEM)
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop=0;
				   //else if  (iPickState==PICCK_ITEM_MONEY)
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop=0;
				   //else if  (iPickState==PICCK_TOOL)
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngToolLoop=0;
				   //else if  (iPickState==PICCK_TUPU)
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngTuzhiLoop=0;
				   //else if  (iPickState==PICCK_MONEY)
					   Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngMoneyLoop=0;
				   
				   nPickState=PICCK_NONE;
				}
				
				if (((xx - x) * (xx - x) + (yy - y) * (yy - y))<(PLAYER_PICKUP_CLIENT_DISTANCE*PLAYER_PICKUP_CLIENT_DISTANCE))
				{  	

					//if (iPickState==PICCK_ITEM)
					    Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop=0;
					//else if  (iPickState==PICCK_ITEM_MONEY)
				        Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngItemLoop=0;
					//else if  (iPickState==PICCK_TOOL)
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngToolLoop=0;
					//else if  (iPickState==PICCK_TUPU)
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngTuzhiLoop=0;

						Player[CLIENT_PLAYER_INDEX].m_Autoplay.ngMoneyLoop=0;
					
					if  (iPickState==PICCK_ITEM_MONEY)
						 Player[CLIENT_PLAYER_INDEX].CheckObject(nObjIdx);
					else if  (iPickState==PICCK_MONEY)
					{
						Player[CLIENT_PLAYER_INDEX].PickUpMoney(nObjIdx,nIndex);
					}
					else
					    Player[CLIENT_PLAYER_INDEX].PickUpItem(nObjIdx,nIndex);


					nPickState=PICCK_NONE;

				} 
		 }  
		 
	     return TRUE;
}

//跟随攻击 
void KNpcAI::FollowAttackCharacter(int i,int r, int l,int nActiveNum) 
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;
	//	Npc[i].m_Hide.nTime=0;
	//	Npc[r].m_Hide.nTime=0;
	int distance = NpcSet.GetDistance(i, r);  //获取场景的距离 人与怪物之间的距离
	
	if ( distance == 0)
		distance = 1;
	//#define	MINI_ATTACK_RANGE	75
	//	if (distance <= Npc[r].m_CurrentAttackRadius) //最小攻击距离
	//	{
	///		KeepAttackRangeA(i,Npc[r].m_CurrentAttackRadius,r); //保持攻击范围
	///		return;
	//	}
	// Attack Enemy
	//	if (nButton == button_right)
	//	{
	//	int	nIdx = Npc[r].m_SkillList.FindSame(SKill->IsCanSkillid());
	//	Npc[r].SetActiveSkill(nIdx);
	//	int	nIdx = SKill->IsCanSkillid();
	//   Npc[r].m_ActiveSkillID=nIdx;

    if (Npc[i].m_IsHaveAttack==1 && Npc[i].m_AttackerDwid==Npc[r].m_dwID)  //已经是卡怪的 就放弃攻击了
	    return;



	if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum>nActiveNum*18 && Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum%nActiveNum==0 && Npc[i].m_IsHaveAttack==0)
	   {
	       if (Npc[i].m_CurrentLife == Npc[i].m_CurrentLifeMax)
		   {
		      Npc[i].m_IsHaveAttack=1;             //是否设置为攻击无效了
              Npc[i].m_AttackerDwid=Npc[r].m_dwID; //上次攻击着的DWID
              Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
			  Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
			  return;
		   }

		   //char mag[64];
		   //  t_sprintf(mag,"NPC:L:%d,MAXl:%d,DWID:%d,num:%d",Npc[i].m_CurrentLife,Npc[i].m_CurrentLifeMax,Npc[i].m_AttackerDwid,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum);
		   //  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(mag);
	   }
	
	if (distance <= (Npc[r].m_CurrentAttackRadius-100) /*&& InEyeshota(i,r)*/)  //小于攻击范围 就使用技能
	{
        Npc[r].SendSerCommand(do_skill, Npc[r].m_ActiveSkillID, -1, i);
		SendClientCmdSkill(Npc[r].m_ActiveSkillID, -1, Npc[i].m_dwID);
        Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum++;
        return;
	}	
	else
	{
		int x, y,nmmap;
		Npc[i].GetMpsPos(&x, &y,&nmmap);	
		Npc[r].SendSerCommand(do_run, x, y);  //跑向敌人
		SendClientCmdRun(x, y); //跑向敌人
	}
}

//挂机
BOOL KNpcAI::KeepActiveJiaoBen(int fanwei,int indexid,int xx,int yy)  // 保持活动范围
{
	int x, y,nmapp;	
	Npc[indexid].GetMpsPos(&x, &y,&nmapp); //NPC所在的 坐标  现在的距离 

	int	nRange = g_GetDistance(xx,yy,x,y); //比较 现在的距离 和原来距离的差距

	if (AiCheckDistance(x,y,xx,yy,Npc[indexid].m_CurrentRunSpeed))//16
	{
	   Npc[indexid].m_CurrentActiveRadius = fanwei;
	   return FALSE;
	}
	
	// 发现超出活动范围，把当前活动范围缩小，避免在活动范围边缘来回晃。
	if (nRange>fanwei)//<1000
	{
		Npc[indexid].m_CurrentActiveRadius = fanwei/10;//500
	}
	
	// 发现超出当前活动范围，往回走
	if (nRange > Npc[indexid].m_CurrentActiveRadius)  //
	{
	    if (Npc[indexid].m_nPeopleIdx>0)
        {
		   Npc[Npc[indexid].m_nPeopleIdx].m_AttackerDwid=Npc[indexid].m_dwID;
		   Npc[Npc[indexid].m_nPeopleIdx].m_IsHaveAttack=1;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:怪物超出活动范围,取消攻击!");
        }

        if (Npc[indexid].m_nObjectIdx>0)
		{
           Object[Npc[indexid].m_nObjectIdx].m_AttackerDwid=Npc[indexid].m_dwID;
		   Object[Npc[indexid].m_nObjectIdx].m_IsHaveAttack=1;
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:物品超出活动范围,执行取消拾取!");
		   nPickState=PICCK_NONE;
		}

        Npc[indexid].m_nPeopleIdx=0;
		Npc[indexid].m_nObjectIdx=0;

     
	if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
	{
            Npc[indexid].SendSerCommand(do_run, xx, yy); 
		    SendClientCmdRun(xx,yy);
	}
	else
	{
		    Npc[indexid].SendSerCommand(do_walk, xx, yy); 
		    SendClientCmdWalk(xx,yy);
	}


	return TRUE;
		
	}
	else	// 在当前活动范围内，恢复当前活动范围大小。
	{
		Npc[indexid].m_CurrentActiveRadius = fanwei;
	}

    return FALSE;
}

BOOL KNpcAI::KeepActiveCharacter(int fanwei,int indexid,int xx,int yy)  // 保持活动范围
{
	int x, y,nmapp;	
	Npc[indexid].GetMpsPos(&x, &y,&nmapp); //NPC所在的 坐标  现在的距离 
	int	nRange = g_GetDistance(Npc[indexid].m_OriginX, Npc[indexid].m_OriginY, x, y); //比较 现在的距离 和原来距离的差距
	
	// 发现超出活动范围，把当前活动范围缩小，避免在活动范围边缘来回晃。
	if (fanwei < nRange)//<1000
	{
		Npc[indexid].m_CurrentActiveRadius = fanwei/10;//500
	}
	
	// 发现超出当前活动范围，往回走
	if (Npc[indexid].m_CurrentActiveRadius < nRange)  //
	{

        if (Npc[indexid].m_nPeopleIdx>0)
        {
		   Npc[Npc[indexid].m_nPeopleIdx].m_AttackerDwid=Npc[indexid].m_dwID;
		   Npc[Npc[indexid].m_nPeopleIdx].m_IsHaveAttack=1;
		   Npc[Npc[indexid].m_nPeopleIdx].m_IsbeSel  = 0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nLocKnpcDw=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcCurlife=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAttackNum=0;
		   Player[CLIENT_PLAYER_INDEX].m_Autoplay._checkLoopTime=0;
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:怪物超出活动范围,取消攻击!");
        }
        if (Npc[indexid].m_nObjectIdx>0)
		{
           Object[Npc[indexid].m_nObjectIdx].m_AttackerDwid=Npc[indexid].m_dwID;
		   Object[Npc[indexid].m_nObjectIdx].m_IsHaveAttack=1;
		   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:物品超出活动范围,执行取消拾取!");
		   nPickState=PICCK_NONE;
		}
	    
        Npc[indexid].m_nPeopleIdx=0;
		Npc[indexid].m_nObjectIdx=0;

	if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
	{
            Npc[indexid].SendSerCommand(do_run, Npc[indexid].m_OriginX, Npc[indexid].m_OriginY); 
		    SendClientCmdRun(Npc[indexid].m_OriginX, Npc[indexid].m_OriginY);
	}
	else
	{
		   Npc[indexid].SendSerCommand(do_walk, Npc[indexid].m_OriginX, Npc[indexid].m_OriginY); 
		   SendClientCmdWalk(Npc[indexid].m_OriginX, Npc[indexid].m_OriginY);
	}
		

        /*
           if (Npc[m_Index].m_nPeopleIdx>0 && Npc[Npc[m_Index].m_nPeopleIdx].m_AttackerDwid==Npc[m_Index].m_dwID && Npc[Npc[m_Index].m_nPeopleIdx].m_IsHaveAttack==1)
   {//如果攻击次数
     Npc[m_Index].m_nPeopleIdx=0;
	 //Npc[m_Index].m_nObjectIdx=0;
	 return;
   }

   if (Npc[m_Index].m_nObjectIdx>0 && Object[Npc[m_Index].m_nObjectIdx].m_AttackerDwid==Npc[m_Index].m_dwID && Object[Npc[m_Index].m_nObjectIdx].m_IsHaveAttack==1)
   {//如果攻击次数
     Npc[m_Index].m_nObjectIdx=0;
	 //Npc[m_Index].m_nObjectIdx=0;
	 return;
   }



		*/



		return TRUE;
		
	}
	else	// 在当前活动范围内，恢复当前活动范围大小。
	{
		Npc[indexid].m_CurrentActiveRadius = fanwei;
		return FALSE;
	}
}

BOOL KNpcAI::AiCheckDistance(int nYMpsX ,int nYMpsY, int nDMpsX,int nDMpsY,int nDisten)
{
  /* int nXcha=nYMpsX-nDMpsX;
   int nYcha=nYMpsY-nDMpsY;

   if (nXcha<0)
      nXcha =-nXcha;
   if (nYcha<0)
      nYcha =-nYcha;


   if (nXcha<=nDisten && nYcha<=nDisten)
	
	  int KSubWorld::GetDistance(int nRx1, int nRy1, int nRx2, int nRy2)
	  {
	  return (int)sqrt((nRx1 - nRx2) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2));
	  }

	
	
	*/
	if  ((int)sqrt(double((nYMpsX - nDMpsX)) * (nYMpsX - nDMpsX) + (nYMpsY - nDMpsY) * (nYMpsY - nDMpsY)) <= nDisten)
      return TRUE;

      return FALSE;
}

int KNpcAI::GetNearestCharacter(int nRelation,int indexid,int nType,int nDis)
{
	int nRangeX;
	if (Npc[indexid].m_Kind==kind_player || Npc[indexid].m_Kind==kind_partner)
	    nRangeX   = Npc[indexid].m_VisionRadius*6;  //视野范围
	else
        nRangeX   = Npc[indexid].m_VisionRadius;

	int	nRangeY   = nRangeX;
	int	nSubWorld = Npc[indexid].m_SubWorldIndex;
	int	nRegion   = Npc[indexid].m_RegionIndex;
	int	nMapX     = Npc[indexid].m_MapX;
	int	nMapY     = Npc[indexid].m_MapY;
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;	
//	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
//	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;	

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; ++i)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; ++j)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定	右下
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC		返回NPC索引	
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpcAuto(nRMx, nRMy, indexid, nRelation,nType,nDis);

			if (Npc[nRet].m_Hide.nTime > 0) //隐身的
				nRet = 0;

			if (nRet > 0)
				return nRet;
			
			// 确定目标格子实际的REGION和坐标确定	 左下
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpcAuto(nRMx, nRMy, indexid, nRelation,nType,nDis);
		
			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
		
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定	   左上
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}

			if (nSearchRegion == -1)
				continue;

			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpcAuto(nRMx, nRMy, indexid, nRelation,nType,nDis);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定	  右上
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpcAuto(nRMx, nRMy, indexid, nRelation,nType,nDis);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
				return nRet;
		}
	}
	return 0;
}
//---------获取周围NPC列表----------------------------------------------------


//----------------------------------------------------------------------------

//查找附近物品
int KNpcAI::GetNearestObject(int indexid,void *pData,int *CurState)
{
	int nRangeX;
	if (Npc[indexid].m_Kind==kind_player)
		nRangeX = Npc[indexid].m_VisionRadius*6;  //视野范围
	else
        nRangeX = Npc[indexid].m_VisionRadius;

	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[indexid].m_SubWorldIndex;
	int	nRegion = Npc[indexid].m_RegionIndex;
	int	nMapX = Npc[indexid].m_MapX;
	int	nMapY = Npc[indexid].m_MapY;
	int	nRet,m_CurState=0;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;	
//	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
//	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;	

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; ++i)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; ++j)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;   //往前下目标坐标
			nRMy = nMapY + j;   //目标坐标
			nSearchRegion = nRegion;

			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;

			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC		返回NPC索引	
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindObjectAuto(nRMx, nRMy,indexid,pData,&m_CurState);

			if (nRet > 0)
			{   
				*CurState=m_CurState;
				return nRet;
			}
			
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;  //往回下的坐标
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindObjectAuto(nRMx, nRMy,indexid,pData,&m_CurState);

			if (nRet > 0)
			{   
				*CurState=m_CurState;
				return nRet;
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;  //网后上的坐标
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindObjectAuto(nRMx, nRMy,indexid,pData,&m_CurState);

			if (nRet > 0)
			{   
				*CurState=m_CurState;
				return nRet;
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;// 往前上的坐标
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindObjectAuto(nRMx, nRMy,indexid,pData,&m_CurState);

			if (nRet > 0)
			{   
				*CurState=m_CurState;
				return nRet;
			}
		}
	}
	return 0;
}

int KNpcAI::GetNearestNpc(int nRelation)
{
	int nRangeX   = Npc[m_nIndex].m_VisionRadius;
	int	nRangeY   = nRangeX;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion   = Npc[m_nIndex].m_RegionIndex;
	int	nMapX     = Npc[m_nIndex].m_MapX;
	int	nMapY     = Npc[m_nIndex].m_MapY;
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;	

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; ++i)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; ++j)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;

			nSearchRegion = nRegion;

			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;

			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;

			if (nRet > 0)
				return nRet;
			
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
		
			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
		
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);

			if (Npc[nRet].m_Hide.nTime > 0)
				nRet = 0;
			
			if (nRet > 0)
				return nRet;
		}
	}
	return 0;
}

// flying add this
// 查找离某个NPC最近的玩家
int KNpcAI::IsPlayerCome()
{
	int nResult = 0;
	int nPlayer = 0;
	int X1 = 0;
	int Y1 = 0;
	int X2 = 0;
	int Y2 = 0;
	int nDistance = 0;

	nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	nDistance = NpcSet.GetDistance(nPlayer, m_nIndex);

	if ( nDistance == 0) nDistance = 1;
	// 鸡看的到的玩家
	if (nDistance < Npc[m_nIndex].m_VisionRadius)
	{
		// 分别处理走和跑
		if (Player[CLIENT_PLAYER_INDEX].m_RunStatus || Npc[m_nIndex].m_CurrentVisionRadius > nDistance * 4)
		{
			nResult = nPlayer;
		}
	}
	return nResult;
}

int KNpcAI::GetNpcNumber(int nRelation)
{
	int nRangeX = Npc[m_nIndex].m_VisionRadius;
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nMapX = Npc[m_nIndex].m_MapX;
	int	nMapY = Npc[m_nIndex].m_MapY;
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	if  (nSubWorld<0 || nRegion<0)
		return 0;

	nRangeX = nRangeX / SubWorld[nSubWorld].m_nCellWidth;
	nRangeY = nRangeY / SubWorld[nSubWorld].m_nCellHeight;

	// 检查视野范围内的格子里的NPC
	for (int i = -nRangeX; i < nRangeX; ++i)
	{
		for (int j = -nRangeY; j < nRangeY; ++j)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;

			if (nSearchRegion == -1)
				continue;

			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += SubWorld[nSubWorld].m_nRegionWidth;
			}
			else if (nRMx >= SubWorld[nSubWorld].m_nRegionWidth)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= SubWorld[nSubWorld].m_nRegionWidth;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += SubWorld[nSubWorld].m_nRegionHeight;
			}
			else if (nRMy >= SubWorld[nSubWorld].m_nRegionHeight)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= SubWorld[nSubWorld].m_nRegionHeight;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nNpcIdx > 0)
				nRet++;
		}
	}
	return nRet;
}

void KNpcAI::KeepAttackRange(int nEnemy, int nRange)
{
	int nX1, nY1,nMap1, nX2, nY2,nMap2, nDir, nWantX, nWantY;

	Npc[m_nIndex].GetMpsPos(&nX1, &nY1,&nMap1);
	Npc[nEnemy].GetMpsPos(&nX2, &nY2,&nMap2);
	nDir = g_GetDirIndex(nX1, nY1, nX2, nY2);//获取64方向 中是哪个编号

	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;

	nWantX = nX2 - ((nRange * g_DirCos(nDir, MaxMissleDir)) >> 10);
	nWantY = nY2 - ((nRange * g_DirSin(nDir, MaxMissleDir)) >> 10);

	Npc[m_nIndex].SendSerCommand(do_walk, nWantX, nWantY);
}
//押镖跟随
void KNpcAI::FollowChargedart(int i) //
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;
	
	int distance = NpcSet.GetDistance(m_nIndex, i);  //获取场景的距离
	
	int nDesX, nDesY,nDmap;

		if (distance < 80)
		{
			Npc[m_nIndex].SendSerCommand(do_stand);
		}
		else
		{
			Npc[i].GetMpsPos(&nDesX, &nDesY,&nDmap);
			if (distance > 100)
			{
				Npc[m_nIndex].SendSerCommand(do_walk, nDesX, nDesY);
			}
			else
			{
				Npc[m_nIndex].SendSerCommand(do_stand);
			}
		}
}

//同伴跟随
void KNpcAI::FollowCompanion(int i) //
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;
	
	int distance = NpcSet.GetDistance(m_nIndex, i);  //获取场景的距离
	
	int nDesX, nDesY,nDmap;
	
	if (distance < 80)
	{
		Npc[m_nIndex].SendSerCommand(do_stand);
		
	}
	else
	{
		Npc[i].GetMpsPos(&nDesX, &nDesY,&nDmap);
		if (distance > 100)
		{
			Npc[m_nIndex].SendSerCommand(do_run, nDesX, nDesY);
		}
		else
		{
			Npc[m_nIndex].SendSerCommand(do_stand);
		}
	}
}

void KNpcAI::FollowAttack(int i)
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;

	int distance = NpcSet.GetDistance(m_nIndex, i);

	if ( distance == 0) 
		distance = 1;
#define	MINI_ATTACK_RANGE	100

//	if (distance <= MINI_ATTACK_RANGE)
//	{
//		KeepAttackRange(i, MINI_ATTACK_RANGE);
//		return;
//	}

	if (distance <= Npc[m_nIndex].m_CurrentAttackRadius && InEyeshot(i)) //取消视野范围限制
	{
		Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);
		return;
	}
	int x, y,nMap;
	Npc[i].GetMpsPos(&x, &y,&nMap);
	Npc[m_nIndex].SendSerCommand(do_walk, x, y);  //走向敌人
}

void KNpcAI::FollowAttackCompanion(int i,int juli)
{
	if ( Npc[i].m_RegionIndex < 0 )
		return;
	
	int distance = NpcSet.GetDistance(m_nIndex, i);
	
	if ( distance == 0) 
		distance = 1;
	if (distance <= juli && distance<= Npc[m_nIndex].m_CurrentAttackRadius) //取消视野范围限制
	{
		Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, i);
		return;
	}
//	int x, y,nMap;
//	Npc[i].GetMpsPos(&x, &y,&nMap);
//	Npc[m_nIndex].SendCommand(do_run, x, y);  //走向敌人
}
//视野范围
BOOL KNpcAI::InEyeshot(int nIdx)
{
	int distance = NpcSet.GetDistance(nIdx, m_nIndex);

	return (Npc[m_nIndex].m_VisionRadius > distance);
}

//客户端的随意走动 逃离
void KNpcAI::CommonActionWithPlayer()
{
	int nClientIndex =Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if (nClientIndex<=0 || nClientIndex>=MAX_NPC) return;
	if (Npc[nClientIndex].m_Kind != kind_player)
		return;

	int	nOffX=0, nOffY=0;
	if (g_RandPercent(10))
	{
		nOffX = 0;
		nOffY = 0;
	}
	else
	{
		nOffX = g_Random(400);
		nOffY = g_Random(200);
		if (nOffX & 1)
		{
			nOffX = - nOffX;
		}
		if (nOffY & 1)
		{
			nOffY = - nOffY;
		}
	}

	Npc[nClientIndex].SendSerCommand(do_run, Npc[nClientIndex].m_OriginX + nOffX, Npc[nClientIndex].m_OriginY + nOffY);
	// Send to Server
	SendClientCmdRun(Npc[nClientIndex].m_OriginX + nOffX,Npc[nClientIndex].m_OriginY + nOffY);
	//Npc[m_nIndex].SendSerCommand(do_walk,Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
}


void KNpcAI::CommonAction()
{
	// 如果是对话类的NPC，就原地不动
	if (Npc[m_nIndex].m_Kind == kind_dialoger)
	{
		Npc[m_nIndex].SendSerCommand(do_stand,0,0,0);
		return;
	}
	int	nOffX, nOffY;
	if (g_RandPercent(80))
	{
		nOffX = 0;
		nOffY = 0;
	}
	else
	{
		
		nOffX = g_Random(Npc[m_nIndex].m_CurrentActiveRadius / 2);
		nOffY = g_Random(Npc[m_nIndex].m_CurrentActiveRadius / 2);
		if (nOffX & 1)
		{
			nOffX = - nOffX;
		}
		if (nOffY & 1)
		{
			nOffY = - nOffY;
		}
	}
    
	Npc[m_nIndex].SendSerCommand(do_walk,Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
}


void KNpcAI::CommonActionCompanion(int fanwei)
{
	// 如果是对话类的NPC，就原地不动
	if (Npc[m_nIndex].m_Kind == kind_dialoger)
	{
		Npc[m_nIndex].SendSerCommand(do_stand);
		return;
	}

	int	nOffX, nOffY;	
		nOffX = g_Random(Npc[fanwei].m_CurrentActiveRadius / 2);
		nOffY = g_Random(Npc[fanwei].m_CurrentActiveRadius / 2);
		if (nOffX & 1)
		{
			nOffX = - nOffX;
		}
		if (nOffY & 1)
		{
			nOffY = - nOffY;
		}
	Npc[m_nIndex].SendSerCommand(do_run, Npc[m_nIndex].m_OriginX + nOffX, Npc[m_nIndex].m_OriginY + nOffY);
}

void KNpcAI::ReMoveCommonAction(int nNpcIdx)
{
	// 如果是对话类的NPC，就原地不动
	if (Npc[nNpcIdx].m_Kind == kind_dialoger)
	{
		Npc[nNpcIdx].SendSerCommand(do_stand);
		return;
	}
	
	int	nOffX, nOffY;	
	nOffX = g_Random(400/ 2);
	nOffY = g_Random(400/ 2);
	if (nOffX & 1)
	{
		nOffX = - nOffX;
	}
	if (nOffY & 1)
	{
		nOffY = - nOffY;
	}

	Npc[nNpcIdx].SendSerCommand(do_run, Npc[nNpcIdx].m_OriginX + nOffX, Npc[nNpcIdx].m_OriginY + nOffY);

	SendClientCmdRun(Npc[nNpcIdx].m_OriginX + nOffX, Npc[nNpcIdx].m_OriginY + nOffY);


}


BOOL KNpcAI::KeepActiveRange()
{
	int x, y,nMap;
	
	Npc[m_nIndex].GetMpsPos(&x, &y,&nMap);
	int	nRange = g_GetDistance(Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY, x, y);

	// 发现超出活动范围，把当前活动范围缩小，避免在活动范围边缘来回晃。
	if (Npc[m_nIndex].m_ActiveRadius < nRange)
	{
		Npc[m_nIndex].m_CurrentActiveRadius = Npc[m_nIndex].m_ActiveRadius / 2;
	}

	// 发现超出当前活动范围，往回走
	if (Npc[m_nIndex].m_CurrentActiveRadius < nRange)
	{
		Npc[m_nIndex].SendSerCommand(do_walk, Npc[m_nIndex].m_OriginX, Npc[m_nIndex].m_OriginY);
		return TRUE;
	}
	else	// 在当前活动范围内，恢复当前活动范围大小。
	{
		Npc[m_nIndex].m_CurrentActiveRadius = Npc[m_nIndex].m_ActiveRadius;
		return FALSE;
	}
}

// 15/16 AiMode NPC的逃逸动作
int KNpcAI::DoShowFlee(int nIdx)
{
	int nResult  = false;
	int nRetCode = false;
	
	int x1, y1,nmap1 ,x2, y2;
	int nDistance = Npc[m_nIndex].m_AiParam[6];

	Npc[m_nIndex].GetMpsPos(&x1, &y1,&nmap1);
	//Npc[nIdx].GetMpsPos(&x2, &y2);
	Npc[m_nIndex].m_Dir = Npc[nIdx].m_Dir;
	nRetCode = GetNpcMoveOffset(Npc[m_nIndex].m_Dir, nDistance, &x2, &y2);
	if (!nRetCode)
		goto Exit0;
	Npc[m_nIndex].m_AiParam[4] = (int) nDistance / Npc[m_nIndex].m_WalkSpeed;
	Npc[m_nIndex].m_AiParam[5] = 0;
	Npc[m_nIndex].SendSerCommand(do_walk, x1 + x2, y1 + y2);

	nResult = true;
Exit0:
	return nResult;
}

/*
void KNpcAI::FleeEnemy(int nEnemyIdx)
{
   	int x1, y1,nMap1, x2, y2,nMap2;
	
	Npc[m_nIndex].GetMpsPos(&x1, &y1,&nMap1);
	Npc[nEnemyIdx].GetMpsPos(&x2, &y2,&nMap2);
	
	x1 = x1 * 2 - x2;
	y1 = y1 * 2 - y2;


	//Npc[m_nIndex].m_Dir = Npc[nIdx].m_Dir;
	//nRetCode = GetNpcMoveOffset(Npc[m_nIndex].m_Dir, nDistance, &x2, &y2);

	
	Npc[m_nIndex].SendSerCommand(do_run, x1, y1);
	
#ifndef _SERVER
	SendClientCmdRun(x1, y1);
#endif

}  
 */
// 逃离Npc[nIdx]
void KNpcAI::Flee(int nIdx)
{
	int x1, y1,nMap1, x2, y2,nMap2;

	Npc[m_nIndex].GetMpsPos(&x1, &y1,&nMap1);
	Npc[nIdx].GetMpsPos(&x2, &y2,&nMap2);

	x1 = x1 * 2 - x2;
	y1 = y1 * 2 - y2;

	Npc[m_nIndex].SendSerCommand(do_walk, x1, y1);
}

//------------------------------------------------------------------------------
//	功能：押镖主动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessChargedart()
{
    int nSub=0;
	int nReg=0;
	int nRes=0;
	int nNpcID=0;
    int nDesX,nDesY,nMap;

	if (Npc[m_nIndex].m_btRankFFId==88)
	{

	  nSub   = Npc[m_nIndex].m_SubWorldIndex;
	  nReg   = Npc[m_nIndex].m_RegionIndex;		
	  nNpcID = NpcSet.SearchNameID(Npc[m_nIndex].m_IsRevive);

	if (nNpcID>0)
	{
		Npc[m_nIndex].m_GameliveTime = -1;
		FollowChargedart(nNpcID);  //跟随
	}      
    else if (nNpcID<=0) ////主角退出游戏叛变---死亡或下线不叛变
	{    
		    if (Npc[m_nIndex].m_GameliveTime==-1)
		         Npc[m_nIndex].m_GameliveTime = 120;  //给两分钟等待 然后销毁
               return;
		    Npc[m_nIndex].GetMpsPos(&nDesX, &nDesY,&nMap);
		    Npc[m_nIndex].m_OriginX=nDesX;	
            Npc[m_nIndex].m_OriginY=nDesY;   
			Npc[m_nIndex].m_nPeopleIdx = 0;
			Npc[m_nIndex].m_btRankFFId=0;
			Npc[m_nIndex].m_Camp=5;
			Npc[m_nIndex].m_IsRevive=0;
			Npc[m_nIndex].m_Kind=kind_normal;//kind_mouse;
            ZeroMemory(Npc[m_nIndex].Name,32); // 清零
			strcat(Npc[m_nIndex].Name,"叛变的(镖车)");
  	}
	}
/*////////////////////获取玩家的任务号对比////////////////////////
    int nSub=Npc[m_nIndex].m_SubWorldIndex;
    int nReg=Npc[m_nIndex].m_RegionIndex;
	int nRes=0;
    KIndexNode *pNode=NULL;
	 
	pNode=(KIndexNode*)SubWorld[nSub].m_Region[nReg].m_NpcList.GetHead();//刷怪节点
          while (pNode)
		  {
			  if (pNode->m_nIndex==Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(170)&& Npc[pNode->m_nIndex].m_NpcSettingIdx==0)
			  {
                  nRes=1;
				  break;
			  }
			  pNode=(KIndexNode *)pNode->GetNext();
		  }
*//////////////////////////////////////////////////////////////////
//	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	//if (KeepActiveRange())
	//	return;
	

	/*
     char isname[32];
     t_sprintf(isname,"%s的[镖车]",Npc[nEnemyIdx].Name);
	if (Npc[m_nIndex].Name!=isname)
	{
     Npc[m_nIndex].m_nPeopleIdx=0;
     return;
	}

	// 如果原本没有锁定敌人或者这个敌人跑太远，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy); //获取附近NPC的索引编号  
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}
	
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	
	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}
	
	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
*/	
//FollowYabiao(nEnemyIdx);
}
//------------------------------------------------------------------------------
//	功能：同伴主动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessCompanion()
{
    int nSub=0,nReg=0,nRes=0,nNpcID=0,nPepID=0;
    int nDesX,nDesY,nMap;
	char nstrName[64]={0};
	
    if (Npc[m_nIndex].m_btRankFFId==99)
	{
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
		int  nEnemyIdx  = GetNearestCharacter(relation_enemy,m_nIndex);   //获取周围敌对的NPC编号
		gameAutoplaerInfo nAutoinfo;
		nAutoinfo.isopenmoney      = 1;
		nAutoinfo.insopenzhuang    = 0;
		nAutoinfo.ismonyeandzhuang = 0;
		nAutoinfo.isopentuzhi      = 0;
		nAutoinfo.isopendaoju      = 0;
		int  nCurState = 0;
        int  nObjectIdx = GetNearestObject(m_nIndex,(BYTE*)&nAutoinfo,&nCurState);      //获取地上周围物品的索引
		
		nNpcID = NpcSet.SearchNameID(Npc[m_nIndex].m_IsRevive);         //查找名字的转换码  ==主人的 索引
		nPepID = PlayerSet.FindNameID(nstrName);      //Npc[nNpcID].m_nsPlayerIdx;
		//	g_FileName2Id(Npc[m_nIndex].m_GuishuName);
		int nNpcMap,nNpcX,nNpcY,nPlayerMap,nPlayerX,nPlayerY;

		if (nNpcID>0)
		{	
            int mPlayerSubWorldID,nNpcSubWorldID,distance;

            Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;  //设置主人已经有同伴了！
		
			if (Npc[m_nIndex].m_TempliveTime>0)
			{
                 Npc[m_nIndex].m_GameliveTime = Npc[m_nIndex].m_TempliveTime;
				 Npc[m_nIndex].m_TempliveTime = 0;
			}

            Npc[m_nIndex].GetMpsPos(&nNpcX,&nNpcY,&nNpcMap);
			Npc[nNpcID].GetMpsPos(&nPlayerX,&nPlayerY,&nPlayerMap);

		    mPlayerSubWorldID = SubWorld[Npc[nNpcID].m_SubWorldIndex].m_SubWorldID;
            nNpcSubWorldID    = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
            distance          = NpcSet.GetDistance(nNpcID, m_nIndex);                          //与主人的距离

			if (nNpcSubWorldID!=mPlayerSubWorldID)
			{//如果不同地图 就飞吧
				  return;
			}
			else  
			{  
				if  (distance > 800)
				{
				  return;
				}        
			}

			if (nObjectIdx>0 && (Object[nObjectIdx].m_nKind == Obj_Kind_Money/*||(Object[nObjectIdx].m_nKind == Obj_Kind_Item && Object[nObjectIdx].m_nColorID==1 ) */) )
			{   
				Npc[m_nIndex].m_nPeopleIdx=0;
				int x, y;
				Object[nObjectIdx].GetMpsPos(&x, &y);	
				Npc[m_nIndex].SendSerCommand(do_run, x, y);  //跑向钱
			//	SendClientCmdRun(x, y); //跑向钱
	            Player[nPepID].PickUpMoney(nObjectIdx,m_nIndex);
				return;
			}
			else
			{
				Npc[m_nIndex].m_nObjectIdx=0;
			}

			if (Npc[nNpcID].m_FightMode==0)  //主人为非战斗模式 
			{
				Npc[m_nIndex].m_nPeopleIdx=0;
				nEnemyIdx=0;
				FollowCompanion(nNpcID);
				return;
			}
 
			    //int  Zdistance = NpcSet.GetDistance(nNpcID, m_nIndex);  //与主人的距离

			    if (distance>400)
				{ 
				  Npc[m_nIndex].m_nPeopleIdx=0;
				  nEnemyIdx=0;
				  FollowCompanion(nNpcID);
			  	  return;
				}  
				 //NPC死亡也不攻击
				 if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive)
				 {
					 Npc[m_nIndex].m_nPeopleIdx = 0;
					 nEnemyIdx=0;
				 }
				 //检测同伴类的攻击
				 if (Npc[nEnemyIdx].m_Kind==kind_partner) 
				 {//如果敌人是同伴
					 Npc[nEnemyIdx].GetstrInfo(STR_GUISHU_NAME,nstrName);
					 int nEnemyPepID=PlayerSet.FindNameID(nstrName);  //敌人同伴的主人
					 
					 if (Player[nEnemyPepID].m_cPK.GetNormalPKState()==0 &&
						 Npc[nEnemyIdx].m_FightMode==0)
					 {//检测主人的状态 非战斗模式，没开PK  不攻击
						 Npc[m_nIndex].m_nPeopleIdx = 0;
					     nEnemyIdx=0;
					 }
					 else if (Player[nEnemyPepID].m_cPK.GetNormalPKState()==0 &&
						 Npc[nEnemyIdx].m_FightMode==1)
					 {//战斗模式 但没开PK 不攻击
						 Npc[m_nIndex].m_nPeopleIdx = 0;
						 nEnemyIdx=0;
					 }
					 else if (Npc[nEnemyIdx].m_FightMode==0)
					 {//战斗模式关闭 不攻击
						 Npc[m_nIndex].m_nPeopleIdx = 0;
						 nEnemyIdx=0;
					 }
			
				 }		
               else if (Npc[nEnemyIdx].m_Kind==kind_player) 
			   {// 如果敌人是玩家
				 if (Npc[nEnemyIdx].m_FightMode==0)
				 {///战斗模式关闭 就不攻击了！
					 Npc[m_nIndex].m_nPeopleIdx = 0;
					 nEnemyIdx=0;
				 }
				 else if (Player[Npc[nEnemyIdx].GetNpcPepoleID()].m_cPK.GetNormalPKState()==0)
				 {///pk模式关闭 就不攻击了！
					 Npc[m_nIndex].m_nPeopleIdx = 0;
					 nEnemyIdx=0;
				 }
				 else if (Npc[nEnemyIdx].m_CurrentCamp ==Npc[Player[nPepID].m_nIndex].m_CurrentCamp)
				 {//当前阵营相同 就不攻击了
					 Npc[m_nIndex].m_nPeopleIdx = 0;
					 nEnemyIdx=0;
				 }
			   }

			if (nEnemyIdx>0)
			{
			  int  Ddistance = NpcSet.GetDistance(nEnemyIdx, m_nIndex);  //与敌人的距离
			  
			  if (Ddistance>400)
			  {//大于这个距离就不攻击了
			     nEnemyIdx=0;
				 Npc[m_nIndex].m_nPeopleIdx = 0;
				 FollowCompanion(nNpcID);
				 return;
			  }
			  Npc[m_nIndex].m_nPeopleIdx=nEnemyIdx;
			  int   nSkillID;
			  nSkillID=GetRandomNumber(1,4);
		      if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
			  {
				    Npc[nNpcID].GetMpsPos(&nDesX, &nDesY,&nMap);
				    Npc[m_nIndex].m_OriginX=nDesX;	
                    Npc[m_nIndex].m_OriginY=nDesY;  
                    CommonActionCompanion(nNpcID);
				     return;
			  }
                 FollowAttackCompanion(nEnemyIdx,400);   //攻击敌人
			}
            else
			{ // 周围没有敌人，一定概率待机/巡逻
	         nEnemyIdx = GetNearestCharacter(relation_enemy,m_nIndex); //获取周围敌对的NPC编号
	         Npc[nNpcID].GetMpsPos(&nDesX, &nDesY,&nMap);
	         Npc[m_nIndex].m_OriginX=nDesX;	
	         Npc[m_nIndex].m_OriginY=nDesY; 
	         CommonActionCompanion(nNpcID);
			 return;
			} 
		}      
		else if (nNpcID==0) //主角退出游戏叛变---死亡或下线不叛变
		{  
			if (Npc[m_nIndex].m_GameliveTime>120)
			{
              Npc[m_nIndex].m_TempliveTime = Npc[m_nIndex].m_GameliveTime; 
			  Npc[m_nIndex].m_GameliveTime = 120;  //给两分钟等待 然后销毁
              
			}
		    return;
            
			Npc[m_nIndex].GetMpsPos(&nDesX, &nDesY,&nMap);
			Npc[m_nIndex].m_OriginX    = nDesX;	
            Npc[m_nIndex].m_OriginY    = nDesY;  
			Npc[m_nIndex].m_nPeopleIdx = 0;
			Npc[m_nIndex].m_btRankFFId = 0;
			Npc[m_nIndex].m_Camp     = 5;
			Npc[m_nIndex].m_Kind     = kind_normal;//kind_mouse;
			Npc[m_nIndex].m_IsRevive = 0;
			Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\tongyong.lua");  //设定死亡脚本
			Npc[m_nIndex].m_GameliveTime = 60;
			//strcpy(Npc[m_nIndex].ActionScript, "\\script\\deadth\\tongyong.lua");
		    Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\tongyong.lua");
            ZeroMemory(Npc[m_nIndex].Name,sizeof(Npc[m_nIndex].Name)); // 清零
			//ZeroMemory(Npc[m_nIndex].m_GuishuName,sizeof(Npc[m_nIndex].m_GuishuName)); // 归属清零
			Npc[m_nIndex].ClearstrInfo(STR_GUISHU_NAME);
			strcat(Npc[m_nIndex].Name,"叛变的(宠物)");

			return;
		}
	}
}


//------------------------------------------------------------------------------
//	功能：指定路线ai 自动攻击
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//---------/---------------------------------------------------------------------
void	KNpcAI::ProcessLuXianAi()
{
	if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
            g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
			g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
			char nstrName[64]={0};
		    Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
			nPepID = PlayerSet.FindNameID(nstrName); 

	    if (nPepID)
		{
		    Npc[m_nIndex].m_ZhuaVal    = 0;
		} 
	}

	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// 如果原本没有锁定敌人或者这个敌人跑太远，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))  //不在视野范围内
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}
//NPC死亡也不攻击
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
		   CommonAction();
		}
		return;
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1]) //10
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		int   nSkillID;
           nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}

	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通主动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种技能的使用概率，分别对应SkillList里的技能1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//---------/---------------------------------------------------------------------
void	KNpcAI::ProcessAIType01()
{
	if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
            g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
			g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
			char nstrName[64]={0};
		    Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
			nPepID = PlayerSet.FindNameID(nstrName); 

	    if (nPepID)
		{
             if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			 {
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
			 }

			 if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			 {
				    Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
			 }

		     if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			 {  //开始叛变
	
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;

				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
		        //strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
				Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);

				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
			
			 }  
	     return;
		} 
	}

	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// 如果原本没有锁定敌人或者这个敌人跑太远，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx))  //不在视野范围内
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
//NPC死亡也不攻击
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			  Npc[m_nIndex].m_nPeopleIdx = 0;
			  nEnemyIdx=0;
		  }
		  
		}   
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
		   CommonAction();
		}
		return;
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1]) //10
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}

		int   nSkillID;
              nSkillID=GetRandomNumber(1,4);

		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}

	FollowAttack(nEnemyIdx);
}


//------------------------------------------------------------------------------
//	功能：普通主动类2
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用回复技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType02()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
		g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
		g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
		char nstrName[64]={0};
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
		nPepID = PlayerSet.FindNameID(nstrName); 
		if (nPepID)
		{
			if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			}
			if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			 }
			if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			{  //开始叛变
				
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;				
				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
				//strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
				Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);

				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
				
			}  
			return;
		} 
	}


	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// 如果原本没有锁定敌人或者这个敌人跑太远，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}

	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			  Npc[m_nIndex].m_nPeopleIdx = 0;
			  nEnemyIdx=0;
		  }
		  
		}   
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}

//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if  (Npc[m_nIndex].m_CurrentLifeMax<=0)
		Npc[m_nIndex].m_CurrentLifeMax = 100;

	// 检测剩余生命是否符合条件，生命太少一定概率使用补血技能或逃跑
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用补血技能或逃跑
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// 使用补血技能
			{
				Npc[m_nIndex].SetActiveSkill(1);
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
				Npc[m_nIndex].m_AiAddLifeTime++;
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通主动类3
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用攻击技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType03()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
		g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
		g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
		char nstrName[64]={0};
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
		nPepID = PlayerSet.FindNameID(nstrName); 
		if (nPepID)
		{
			if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			}	

			if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			 }

			if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			{  //开始叛变
				
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;
				
				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
				//strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
				Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);

				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
				
			}  
			return;
		} 
	}


	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// 如果原本没有锁定敌人或者这个敌人跑太远，重新锁定敌人
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);

	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			  Npc[m_nIndex].m_nPeopleIdx = 0;
			  nEnemyIdx=0;
		  }
		  
		}   
	} 
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if  (Npc[m_nIndex].m_CurrentLifeMax<=0)
		Npc[m_nIndex].m_CurrentLifeMax = 100;
	// 检测剩余生命是否符合条件，生命太少一定概率使用攻击技能或逃跑
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用攻击技能或逃跑
		{
			if (g_RandPercent(pAIParam[3]))	// 使用攻击技能
			{
				Npc[m_nIndex].SetActiveSkill(1);
				FollowAttack(nEnemyIdx);
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类1
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1、2、3、4] 四种攻击技能的使用概率，分别对应SkillList里的技能 1 2 3 4
//	m_AiParam[5、6] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType04()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

    if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
		g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
		g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
		char nstrName[64]={0};
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);

		nPepID = PlayerSet.FindNameID(nstrName); 
		if (nPepID)
		{
			if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			}

			if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			 }

			if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			{  //开始叛变
				
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;
				
				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
				//strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
		        Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);
				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
				
			}  
			return;
		} 
	}


	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	
	
	//NPC死亡也不攻击
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		 nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			  Npc[m_nIndex].m_nPeopleIdx = 0;
			  nEnemyIdx=0;
		  }
		  
		}   
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}	
	
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[5])	// 待机
			return;
		if (nRand < pAIParam[5] + pAIParam[6])	// 巡逻
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[1])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[1] + pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类2
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用回复技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType05()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

    if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
		g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
		g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
		char nstrName[64]={0};
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
		nPepID = PlayerSet.FindNameID(nstrName); 
		if (nPepID)
		{
			if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			}
			
			if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			 }

			if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			{  //开始叛变
				
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;
				
				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
				//strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
		        Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);
				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
				
			}  
			return;
		} 
	}

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	//NPC死亡也不攻击
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		 nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			  Npc[m_nIndex].m_nPeopleIdx = 0;
			  nEnemyIdx=0;
		  }
		  
		}   
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}
		
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;
	if  (Npc[m_nIndex].m_CurrentLifeMax<=0)
		Npc[m_nIndex].m_CurrentLifeMax = 100;
	// 检测剩余生命是否符合条件，生命太少一定概率使用补血技能或逃跑
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用补血技能或逃跑
		{
			if (Npc[m_nIndex].m_AiAddLifeTime < pAIParam[9] && g_RandPercent(pAIParam[3]))	// 使用补血技能
			{
				Npc[m_nIndex].m_AiAddLifeTime++;
				Npc[m_nIndex].SetActiveSkill(1);
				Npc[m_nIndex].SendSerCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机  《0
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻  小于10
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

//------------------------------------------------------------------------------
//	功能：普通被动类3
//	m_AiParam[0] 无敌人时候的巡逻概率
//	m_AiParam[1] 剩余生命低于这个百分比的时候执行相应处理
//	m_AiParam[2] 在m_AiParam[1]的情况出现的时候是否执行相应处理的概率
//	m_AiParam[3] 在m_AiParam[1]的情况出现并决定要执行相应处理，使用攻击技能的概率 对应SkillList里面的技能 1
//	m_AiParam[4、5、6] 三种攻击技能的使用概率，分别对应SkillList里的技能 2 3 4
//	m_AiParam[7、8] 看见敌人但比较远时，待机、巡逻的概率
//------------------------------------------------------------------------------
void	KNpcAI::ProcessAIType06()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

    if (Npc[m_nIndex].m_ZhuaVal > 0)
	{//正在被抓捕
		int nZhuaRank=0,nLiveTime=0,nPepID=0;;
		g_GameSetTing.GetInteger("SYSTEM","ZhuaBuRank",0,&nZhuaRank);
		g_GameSetTing.GetInteger("SYSTEM","LifeTime",0,&nLiveTime);
		char nstrName[64]={0};
		Npc[m_nIndex].GetstrInfo(STR_GUISHU_NAME,nstrName);
		nPepID = PlayerSet.FindNameID(nstrName); 
		if (nPepID)
		{
			if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			}
			
			if (Npc[m_nIndex].m_cGold.GetGoldType()>0)
			{
				Npc[m_nIndex].m_ZhuaVal    = 0;
				return;
			 }

			if (Npc[m_nIndex].m_ZhuaVal > Npc[m_nIndex].m_Level*nZhuaRank)
			{  //开始叛变
				
                if (Npc[Player[nPepID].m_nIndex].m_TongBanNum > 0)
				{
					Npc[m_nIndex].m_ZhuaVal    = 0;
					return;
				}
				
                Npc[Player[nPepID].m_nIndex].m_TongBanNum = Npc[m_nIndex].m_dwID;
				
				Npc[m_nIndex].m_Camp       = Npc[Player[nPepID].m_nIndex].m_Camp;
				Npc[m_nIndex].m_btRankFFId = 99;
				Npc[m_nIndex].m_GuiShuDwid = Npc[Player[nPepID].m_nIndex].m_dwID;
				Npc[m_nIndex].m_IsRevive   = g_FileName2Id(Npc[Player[nPepID].m_nIndex].Name);  // 名字的转换码
				Npc[m_nIndex].m_Kind       = kind_partner;
				Npc[m_nIndex].m_ActionScriptID = g_CheckFileExist("\\script\\deadth\\同伴死亡\\tongyong.lua");  //设定死亡脚本
				//strcpy(Npc[m_nIndex].ActionScript,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				Npc[m_nIndex].SetstrInfo(STR_ACTION_SCRIPT,"\\script\\deadth\\同伴死亡\\tongyong.lua");
				//t_sprintf(Npc[m_nIndex].m_GuishuName,"%s",Npc[Player[nPepID].m_nIndex].Name);
				//char nstrName[64]={0};
		        Npc[m_nIndex].SetstrInfo(STR_GUISHU_NAME,Npc[Player[nPepID].m_nIndex].Name);
				t_sprintf(Npc[m_nIndex].Name,"宠物");
				Npc[m_nIndex].m_ZhuaVal    = 0;
				Npc[m_nIndex].m_GameliveTime   = nLiveTime;
				Npc[m_nIndex].m_bClientOnly = FALSE;
				
			}  
			return;
		} 
	}

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	
	//NPC死亡也不攻击
	if (Npc[nEnemyIdx].m_Doing == do_death || Npc[nEnemyIdx].m_Doing == do_revive || Npc[nEnemyIdx].m_Hide.nTime >0) 
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		 nEnemyIdx=0;
	}
	// 周围没有敌人，一定概率待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}
	if (Npc[m_nIndex].m_cGold.GetGoldType()>0 && Npc[m_nIndex].m_cGold.GetGoldType()<5)
	{//自己是黄金怪物

		if (Npc[nEnemyIdx].m_Kind==kind_player &&  Npc[nEnemyIdx].m_AutoplayId == 1)
		{//挂机中的 	
			int nPlaidx=0;
			nPlaidx=CLIENT_PLAYER_INDEX;
		  if (Player[nPlaidx].m_Autoplay.nIsNoBlue==1)
		  {
			Npc[m_nIndex].m_nPeopleIdx = 0;
			nEnemyIdx=0;
		  }
		}   
	}
///战斗模式关闭 就不攻击了！
	if (Npc[nEnemyIdx].m_Kind==kind_player && Npc[nEnemyIdx].m_FightMode==0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
	}
//恐惧状态 不能攻击人了
    if  (Npc[m_nIndex].m_randmove.nTime>0)
	{
		Npc[m_nIndex].m_nPeopleIdx = 0;
		nEnemyIdx=0;
		CommonAction();
		return;
	}
	
	// 是否受到攻击，否，一定概率选择待机/巡逻
	if (nEnemyIdx <= 0)
	{
		// pAIParam[0]:巡逻概率
		if (pAIParam[0] > 0 && g_RandPercent(pAIParam[0]))
		{	// 巡逻
			CommonAction();
		}
		return;
	}

	// 是否已超过活动半径
	if (KeepActiveRange())
		return;
	if  (Npc[m_nIndex].m_CurrentLifeMax<=0)
		Npc[m_nIndex].m_CurrentLifeMax = 100;
	// 检测剩余生命是否符合条件，生命太少一定概率使用攻击技能或逃跑
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[1])
	{
		if (g_RandPercent(pAIParam[2]))	// 是否使用攻击技能或逃跑
		{
			if (g_RandPercent(pAIParam[3]))	// 使用攻击技能
			{
				Npc[m_nIndex].SetActiveSkill(1);
				FollowAttack(nEnemyIdx);	// 向敌人靠近
				return;
			}
			else	// 逃跑
			{
				Flee(nEnemyIdx);
				return;
			}
		}
	}

	// 如果敌人在所有技能攻击范围之外，一定概率选择待机/巡逻/向敌人靠近
	if (KNpcSet::GetDistanceSquare(m_nIndex, nEnemyIdx) > pAIParam[MAX_AI_PARAM - 1])
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[7])	// 待机
			return;
		if (nRand < pAIParam[7] + pAIParam[8])	// 巡逻
		{
			CommonAction();
			return;
		}
		int   nSkillID;
		nSkillID=GetRandomNumber(1,4);
		if (!Npc[m_nIndex].SetActiveSkill(nSkillID))
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);	// 向敌人靠近
		return;
	}

	// 敌人在最大技能攻击范围之内，选择一种技能攻击
	int		nRand;
	nRand = g_Random(100);
	if (nRand < pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[4] + pAIParam[5] + pAIParam[6])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}
	else	// 待机
	{
		return;
	}
	FollowAttack(nEnemyIdx);
}

/*
// 一般主动型
void KNpcAI::ProcessAIType1()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;
	// 是否已超过活动半径
	if (KeepActiveRange())
		return;

	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return;
		}
	}

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	
	if (nEnemyIdx <= 0 || Npc[nEnemyIdx].m_dwID <= 0 || !InEyeshot(nEnemyIdx) )
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}
	
	

	if (nEnemyIdx > 0)
	{
		int		nRand;
		nRand = g_Random(100);
		if (nRand < pAIParam[2])
		{
			if (!Npc[m_nIndex].SetActiveSkill(2))
			{
				CommonAction();
				return;
			}
		}
		else if (nRand < pAIParam[2] + pAIParam[3])
		{
			if (!Npc[m_nIndex].SetActiveSkill(3))
			{
				CommonAction();
				return;
			}
		}
		else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
		{
			if (!Npc[m_nIndex].SetActiveSkill(4))
			{
				CommonAction();
				return;
			}
		}

//		if (g_RandPercent(pAIParam[2]))
//		{
//			Npc[m_nIndex].SetActiveSkill(2);
//		}
//		else if (g_RandPercent(pAIParam[3]))
//		{
//			Npc[m_nIndex].SetActiveSkill(3);
//		}
//		else if (g_RandPercent(pAIParam[4]))
//		{
//			Npc[m_nIndex].SetActiveSkill(4);
//		}
		else
		{
			CommonAction();
			return;
		}
		FollowAttack(nEnemyIdx);
		return;
	}
	CommonAction();
}
*/

/*
// 一般被动型
void KNpcAI::ProcessAIType2()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return;
		}
	}

	int nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;
	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
		return;

	int		nRand;
	nRand = g_Random(100);

	if (nRand < pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(4))
		{
			CommonAction();
			return;
		}
	}

//	if (g_RandPercent(pAIParam[2]))
//	{
//		Npc[m_nIndex].SetActiveSkill(2);
//	}
//	else if (g_RandPercent(pAIParam[3]))
//	{
//		Npc[m_nIndex].SetActiveSkill(3);
//	}
//	else if (g_RandPercent(pAIParam[4]))
//	{
//		Npc[m_nIndex].SetActiveSkill(4);
//	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(nEnemyIdx);

	return;
}
*/

/*
// 一般逃跑型
void KNpcAI::ProcessAIType3()
{
	int* pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int	nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	if (nEnemyIdx <= 0)
	{
		CommonAction();
		return;
	}
	
	if (Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(nEnemyIdx);
			return;
		}
	}

	int		nRand;
	nRand = g_Random(100);

	if (nRand < pAIParam[2])
	{
		if (!Npc[m_nIndex].SetActiveSkill(1))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3])
	{
		if (!Npc[m_nIndex].SetActiveSkill(2))
		{
			CommonAction();
			return;
		}
	}
	else if (nRand < pAIParam[2] + pAIParam[3] + pAIParam[4])
	{
		if (!Npc[m_nIndex].SetActiveSkill(3))
		{
			CommonAction();
			return;
		}
	}

//	if (g_RandPercent(pAIParam[2]))
///	{
//		Npc[m_nIndex].SetActiveSkill(1);
//	}
//	else if (g_RandPercent(pAIParam[3]))
//	{
//		Npc[m_nIndex].SetActiveSkill(2);
//	}
//	else if (g_RandPercent(pAIParam[4]))
//	{
//		Npc[m_nIndex].SetActiveSkill(3);
//	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(nEnemyIdx);
	return;
}
*/

/*
// 逃跑加强型
void KNpcAI::ProcessAIType4()
{
	int*	pAIParam = Npc[m_nIndex].m_AiParam;
	
	if (KeepActiveRange())
		return;

	int	nEnemyIdx = Npc[m_nIndex].m_nPeopleIdx;

	if (nEnemyIdx <= 0 || !InEyeshot(nEnemyIdx))
	{
		nEnemyIdx = 3(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = nEnemyIdx;
	}

	if (nEnemyIdx <= 0)
	{
		CommonAction();
		return;
	}
	
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;
	if (nLifePercent < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(nEnemyIdx);
			return;
		}
	}
	if (nLifePercent < pAIParam[2])
	{
		if (g_RandPercent(pAIParam[3]))
		{
			Npc[m_nIndex].SetActiveSkill(1);
			Npc[m_nIndex].SendCommand(do_skill, Npc[m_nIndex].m_ActiveSkillID, -1, m_nIndex);
			return;
		}
	}

	if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[5]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(nEnemyIdx);
	return;
}
*/

/*
//	人多就跑型
void KNpcAI::ProcessAIType5()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}

	int nEnemyNumber = GetNpcNumber(relation_enemy);
	if (nEnemyNumber > pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(i);
			return;
		}
	}

	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (nEnemyNumber <= pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else
	{
		CommonAction();
		return;
	}

	FollowAttack(i);
	return;
}
*/

/*
//	成群结队型
void KNpcAI::ProcessAIType6()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}

	int nAllyNumber = GetNpcNumber(relation_none);
	if (nAllyNumber <= pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			Flee(i);
			return;
		}
	}
	
	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (nAllyNumber > pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else
	{
		CommonAction();
		return;
	}

	FollowAttack(i);
	return;
}
*/

/*
// 挨打聚堆型
void KNpcAI::ProcessAIType7()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;
	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}

	int j = GetNearestNpc(relation_ally);

	if (j && Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax < pAIParam[0])
	{
		if (g_RandPercent(pAIParam[1]))
		{
			int x, y;
			Npc[j].GetMpsPos(&x, &y);
			Npc[m_nIndex].SendCommand(do_walk, x, y);
			return;
		}
	}

	if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(i);
	return;
}
*/

/*
//	主动送死型
void KNpcAI::ProcessAIType8()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}
	
	if (g_RandPercent(pAIParam[0]))
	{
		int x, y;

		Npc[i].GetMpsPos(&x, &y);
		Npc[m_nIndex].SendCommand(do_walk, x, y);
	}
	else if (g_RandPercent(pAIParam[1]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(i);
	return;
}
*/

/*
//	越战越勇型
void KNpcAI::ProcessAIType9()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}
	
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;

	if (g_RandPercent(pAIParam[0]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (nLifePercent < pAIParam[1] && g_RandPercent(pAIParam[2]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (nLifePercent < pAIParam[3] && g_RandPercent(pAIParam[4]))
	{
		Npc[m_nIndex].SetActiveSkill(3);
	}
	else
	{
		CommonAction();
		return;
	}
	FollowAttack(i);
	return;
}
*/

/*
//	逃跑不掉型
void KNpcAI::ProcessAIType10()
{
	int *pAIParam = Npc[m_nIndex].m_AiParam;

	if (KeepActiveRange())
		return;

	int i = Npc[m_nIndex].m_nPeopleIdx;

	if (!i || !InEyeshot(i))
	{
		i = GetNearestNpc(relation_enemy);
		Npc[m_nIndex].m_nPeopleIdx = i;
	}

	if (!i)
	{
		CommonAction();
		return;
	}
	
	int nLifePercent = Npc[m_nIndex].m_CurrentLife * 100 / Npc[m_nIndex].m_CurrentLifeMax;

	if (nLifePercent < pAIParam[0] && g_RandPercent(pAIParam[1]))
	{
		Npc[m_nIndex].SetActiveSkill(1);
	}
	else if (nLifePercent < pAIParam[2] && g_RandPercent(pAIParam[3]))
	{
		Npc[m_nIndex].SetActiveSkill(2);
	}
	else if (nLifePercent < pAIParam[4] && g_RandPercent(pAIParam[5]))
	{
		Flee(i);
		return;
	}
	else
	{
		CommonAction();
		return;
	}

	FollowAttack(i);
	return;
}
*/
