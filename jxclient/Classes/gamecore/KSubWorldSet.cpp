#include "KCore.h"
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KNpc.h"
#include "engine/KIniFile.h"
#include "KSubWorldSet.h"
#include "KMissleSet.h"
#include "LuaFuns.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
/////////////后来加/////////////////
#include  <time.h>
#include "engine/KTimer.h"
#include "KSortScript.h"
//#include "KScriptValueSet.h"
// #ifdef WIN32
// #include <ppl.h>
// #endif
//#include "KNpcTemplate.h"
//extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点
///////////////////////////////////
KSubWorldSet g_SubWorldSet;

KSubWorldSet::KSubWorldSet()
{
	//m_TaskTimer.Start();
	m_nLoopRate = 0;   //循环的帧数 18帧为一秒
	m_dwPing    = 0;
	//CCMessageBox("m_nLoopRate is null","KSubWorldSet");
}

////////////////////////////////////地图执行脚本//////////////////////////////////////

BOOL KSubWorldSet::SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam,char * szCanshu)
{//g_FileName2Id(ScriptFileName)
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	char nNewScriptName[128]={0};
	t_sprintf(nNewScriptName,"%s",ScriptFileName);
	g_StrLower(nNewScriptName);//g_StrLower
	return SubExecuteScriptB(nNewScriptName, szFunName, nParam,szCanshu);	
}

BOOL KSubWorldSet::SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam,char * szCanshu)
{
	KSubWorld nIndex;	
 
		bool bExecuteScriptMistake = true;
		//创建一个新栈
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptIdA); //dwScriptId
		int nTopIndex = 0;  //记录位置 （栈底）

		if (pScript==NULL)
		{
			printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",dwScriptIdA,szFunName);
			return FALSE;
		} 
	try
	{
		if (pScript)
		{	//向栈中压入一个int数字

		    nTopIndex=pScript->SafeCallBegin();
   
		    if (pScript->CallFunction(szFunName,0,"ds",nParam,szCanshu)) 
			{
				bExecuteScriptMistake = false;
			} 
			
           // lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
			nTopIndex=pScript->SafeCallBegin();
			pScript->SafeCallEnd(nTopIndex);
		    //lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		//	lua_close(pScript->m_LuaState);   //释放内存
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈   
		if (bExecuteScriptMistake)
		{
		    pScript=NULL;
			return FALSE;
		}

		pScript=NULL;
		return TRUE;
	}
	catch(...)
	{
		
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);
		}

		printf("执行脚本发生意外E,Script[%s]!\n",szFunName);
		return FALSE;
	}
	return TRUE;
}

BOOL	KSubWorldSet::SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams,char * snParams,int inParams,int njb,int njxb )
{
	KSubWorld nIndex;
	KNpc   m_Index;

		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript==NULL)
		{
			printf("-------脚本不存在,执行[%s]失败!-------- \n",szFunName);
			return FALSE;
		}

		int nTopIndex = 0;
	 try
	 { 	
		if (pScript)
		{
		//	Lua_PushNumber(pScript->m_LuaState, nIndex.m_nIndex);
		//	pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);			
		     nTopIndex=pScript->SafeCallBegin();
			if ((!szParams) || !szParams[0]) 
			{//如果没有参数	 如果第一次为空，就执行空参数
				 if (pScript->CallFunction(szFunName,0,""))
				 {
					 bExecuteScriptMistake = false;
				 }
			}
		    else if (pScript->CallFunction(szFunName,0,"ssdddd",szParams,snParams,inParams,njb,njxb,0))  //有参数
			{
				bExecuteScriptMistake = false;
			}
			//}
		   nTopIndex=pScript->SafeCallBegin();
		   pScript->SafeCallEnd(nTopIndex);
         //#define Lua_Release(L)					lua_close(L)
		//	lua_close(pScript->m_LuaState); //释放内存
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		}
		//lua_pop(pScript->m_LuaState, -1); //从栈顶弹出1个元素 -1清空栈
		if (bExecuteScriptMistake)
		{//执行脚本失败后，就清空
			pScript=NULL;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		if (pScript)
		{
			nTopIndex=pScript->SafeCallBegin();	
			pScript->SafeCallEnd(nTopIndex);		
			//pScript->Exit();
		}
		printf("执行脚本发生意外D,Script[%s],[%d]!\n",szFunName);		
		return FALSE;
	}
	return TRUE;
}

BOOL	KSubWorldSet::SubExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams,char *  snParams, int inParams,int njb,int njxb)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	DWORD dwScriptId = g_CheckFileExist(ScriptFileName);

	KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
	if (pScript==NULL)
	{
		printf("-------脚本不存在,执行[%s][%s]失败!-------- \n",ScriptFileName,szFunName);
		return FALSE;
	}
	pScript=NULL ;

	return SubExecuteScript(dwScriptId, szFunName, szParams,snParams,inParams,njb,njxb);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int KSubWorldSet::SearchWorld(DWORD dwID)
{
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		if ((DWORD)SubWorld[i].m_SubWorldID == dwID)
			return i;
	}
	return -1;
}

int KSubWorldSet::SearchWorldRows(int dwID)
{
//	for (int i = 0; i < MAX_SUBWORLD; ++i)
//	{
	if (dwID<MAX_SUBWORLD)
		return (DWORD)SubWorld[dwID].m_SubWorldID;
//	}
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////
//static CLogFile gs_LogFile;
/*
BOOL KSubWorldSet::LoadSerMaps(LPSTR szFileName)
{
   KIniFile IniFile;
   char szKeyName[32];
   int nWorldID,nWorldCount = 0,nSucess = 0,nFail = 0;
   int nNpcmun=0,nZnNpcmun=0,nZnNpcmunA=0;
   IniFile.Load(szFileName);
   IniFile.GetInteger("Init", "Count", 1, &nWorldCount);
   if (nWorldCount > MAX_SUBWORLD)
   {
      printf("----地图加载超限限制:加载数量:%d,限制数量:%d-----\n", nWorldCount,MAX_SUBWORLD);
      IniFile.Clear();
      return FALSE;
   }

    printf("Loading File Map: %s\n", szFileName);

    for (int i = 0; i < nWorldCount; ++i)
	{ 
      t_sprintf((char*)szKeyName, "World%02d", i);
      IniFile.GetInteger("World", szKeyName,5000, &nWorldID);
}
      printf("Map Load Sucessed: %d Map,总NPC数量:%d\n", nSucess,nZnNpcmun);
      printf("Map Load Failed: %d Map \n", nFail);
	  IniFile.Clear();
      return TRUE;
} 
*/
//int nActiveRegionCount;
//客户端和 服务器端 公共调用函数---游戏主循环
void KSubWorldSet::MainLoop()
{
	
	++m_nLoopRate;
	// 游戏音乐的处理
	//this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

	for (int i = 0; i < MAX_SUBWORLD; ++i) //服务器端 地图列表
	{
		SubWorld[i].Activate();
		NpcSet.CheckBalance();             //处理同步不一致的NPC （删除）
	}
}

unsigned long KSubWorldSet::getCilentRunTime()
{
	unsigned long dwTimer=0;
#ifdef WIN32
	dwTimer = GetTickCount(); //获取系统运行的时间
#else
	timeval  m_pStartUpdate;
	gettimeofday(&m_pStartUpdate,NULL);
	dwTimer = m_pStartUpdate.tv_sec*1000+m_pStartUpdate.tv_usec/1000;
#endif
	//dwTimer >>= 1;
	return dwTimer;
}

//地图消息处理循环
void KSubWorldSet::MessageLoop()
{
	
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		SubWorld[i].MessageLoop();
	}
}

BOOL KSubWorldSet::SendMessage(int nSubWorldID, DWORD dwMsgType, int nParam1, int nParam2, int nParam3)
{
	/*KWorldMsgNode *pNode = NULL;
	
	pNode = new KWorldMsgNode;
	if (!pNode)
		return FALSE;
	
	pNode->m_dwMsgType	= dwMsgType;
	pNode->m_nParam[0]	= nParam1;
	pNode->m_nParam[1]	= nParam2;
	pNode->m_nParam[2]	= nParam3;

	return SubWorld[nSubWorldID].m_WorldMessage.Send(pNode);*/
	return false;
}
//离开游戏时调用的函数
void KSubWorldSet::Close()
{
	for (int i = 0; i < MAX_SUBWORLD; ++i)
	{
		SubWorld[i].Close();
	}
	NpcSet.RemoveAll(); //人物删除
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();  // 删除客户端全部物品
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	g_Team[0].Release();
	Player[CLIENT_PLAYER_INDEX].Release();
	//m_cMusic.Stop();
}

/*#ifndef _SERVER
void KSubWorldSet::Paint()
{
	SubWorld[0].Paint();
}
#endif*/
