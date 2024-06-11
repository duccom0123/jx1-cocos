/*******************************************************************************
// FileName			:	ScriptFuns.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-11-19 15:58:20
// FileDescription	:	脚本指令集
// Revision Count	:	
*******************************************************************************/
/*#ifndef WIN32
#include <string>
#endif*/
#include "engine/KEngine.h"
#include "engine/KTabFileCtrl.h"
#include "engine/KStepLuaScript.h"	      
#include "engine/lualib/LuaLib.h"
#include "engine/KScriptList.h"
#include "engine/Text.h"
#include <string>
#include "LuaFuns.h"
#include "KCore.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KItemSet.h"
//#include "KNetClient.h"
#include "KScriptValueSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
//#include "KPlayerTong.h"
#include "KSubWorldSet.h"
//#include "KProtocolProcess.h"
#include "KBuySell.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"
//#include "KForBitGua.h"
////原来没有的//////////////////////
#include "KProtocolDef.h"
//#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"
#include "KInventory.h"
#include "coreshell.h"
#include <vector>
#include <cassert> 
#include "KNpcTemplate.h"
#include <time.h>
/////////////////////////////////////
#include "TaskDef.h"
#include "KSortScript.h"
#ifdef WIN32
#include <direct.h>
#else
#include "unistd.h"
#endif

/*#ifndef __linux
#include "Shlwapi.h"
#include "windows.h"
#include "winbase.h"
#include <direct.h>
#else
#include "unistd.h"
#endif*/

#include "engine/KSG_StringProcess.h"


/*#ifndef WIN32
typedef struct  _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
}	FILETIME;
#endif*/

#ifdef WIN32
inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }
#endif

KTabFile        g_MineItemTab;
KScriptList		g_StoryScriptList;
KStepLuaScript * LuaGetScript(Lua_State * L);
int	       GetPlayerIndex(Lua_State * L);
extern int g_GetPriceToStation(int,int);
extern int g_GetPriceToWayPoint(int,int);
extern int g_GetPriceToDock(int ,int );
//BitValue = GetBit(Value, BitNo)
void UseSkills(int sKillID,int sLevel,int nNpcIdx,KMagicAttrib *pData=NULL);
int  GetQuestSetIdx(int sVal);
static void KSceGetFliePathc(char* nName,char * nPath,int nMaxRows)
{
	KFile  nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	t_sprintf(nTongApplyPath,"%s","\\clientscript.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		t_sprintf(szCol,"脚本log\15\n");
		nFile.Write(szCol, sizeof(szCol));
		//nFile.Save(nTongApplyPath);
		nFile.Close();
	}
	
	if (nScirptFile.Load(nTongApplyPath))
	{
		int nRows=nScirptFile.GetHeight();
		if  (nRows==0)
			nRows=1;
		
		if (nRows>=nMaxRows)
		{
			while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}
		//nScirptFile.Clear();
		//retrn;
		}
		
		t_sprintf(szCol,"--%s:(%s)--",nName,nPath);
		nScirptFile.InsertAfter(nRows);
		nScirptFile.WriteString(nRows,1, szCol);                                    //行号
		//nTong.WriteString(nRows,2,Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
		//nTong.WriteInteger(nRows,3,Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
		nScirptFile.Save(nTongApplyPath);	
		//nFile.Write(szCol, sizeof(szCol));
	}
	nScirptFile.Clear();
}

int GetPlayerIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
	if (lua_isnil(L,Lua_GetTopIndex(L)))
		return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nIndex >= MAX_PLAYER || nIndex <= 0) 
	{
		return -1;
	}	
	if (Player[nIndex].m_nIndex >= MAX_NPC || Player[nIndex].m_nIndex < 0)
	{
		//_ASSERT(0);
		return -1;
	}
	return nIndex;
}

int GetObjIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_OBJINDEX);
	if (lua_isnil(L,Lua_GetTopIndex(L)))
		return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nIndex >= MAX_OBJECT || nIndex <= 0) 
	{
		//	_ASSERT(0);
		return -1;
	}
	if (Object[nIndex].m_nIndex != nIndex)
	{
		//	_ASSERT(0);
		return -1;
	}
	return nIndex;
}

int LuaGetBit(Lua_State * L)
{
	int nBitValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nBitNumber >= 32 || nBitNumber <= 0) 
		goto lab_getbit;
	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;
lab_getbit:
	Lua_PushNumber(L, nBitValue);
	return 1;
}

//NewBit = SetBit(Value, BitNo, BitValue)
int LuaSetBit(Lua_State * L)
{
	int nIntValue = (int)Lua_ValueToNumber(L, 1);   //值
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);  //位置
	int nBitValue = (int)Lua_ValueToNumber(L,3);    

	nBitValue = (nBitValue == 1);
	
	if (nBitNumber > 32 || nBitNumber <= 0) //32位
		goto lab_setbit;
	
	nIntValue = (nIntValue | (1 << (nBitNumber - 1)));
lab_setbit:
	Lua_PushNumber(L, nIntValue);
	return 1;
}
//NewByte = SetByte(Value, ByteNo, ByteValue)
int LuaSetByte(Lua_State * L)
{
	BYTE * pByte =	NULL;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	int nByteValue = (int)Lua_ValueToNumber(L,3);

	nByteValue = (nByteValue & 0xff);	//0xff换成十进制为255
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		goto lab_setByte;
	
	pByte =(BYTE*)&nIntValue;	 //
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;//
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
lab_setByte:
	Lua_PushNumber(L, nIntValue);
	return 1;
}

//ByteValue = GetByte(Value, ByteNo)
int LuaGetByte(Lua_State * L)
{
	int nByteValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);     //需要转换的值
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);   //最大存 4个偏移
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_getByte;

	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	
lab_getByte:
	Lua_PushNumber(L, nByteValue);
	return 1;
}

int GetSubWorldIndex(Lua_State * L)
{
	Lua_GetGlobal(L, SCRIPT_SUBWORLDINDEX);
	if (lua_isnil(L,Lua_GetTopIndex(L)))
        return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
    if (nIndex >= MAX_SUBWORLD || nIndex <0) 
	{
		//_ASSERT(0);
		return -1;
	}
	if (SubWorld[nIndex].m_nIndex >= MAX_SUBWORLD || SubWorld[nIndex].m_nIndex < 0)
	{
		//_ASSERT(0);
		return -1;
	}
    return nIndex;
}

//Idx = SubWorldID2Idx(dwID)
int LuaSubWorldIDToIndex(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldID = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldID = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorld(nSubWorldID); // 当前的地图属于服务器地图列表的第几行(索引)
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

int LuaIndexToSubWorldID(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldindex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldindex = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorldRows(nSubWorldindex);
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}


int LuaSetOrAddTrap(Lua_State * L)
{
    int nMapIdx,nXpos,nYpos,nCellNum,nReg=0;
	DWORD nScriptidx;
	      nMapIdx = (int)Lua_ValueToNumber(L,1);
		  nXpos = (int)Lua_ValueToNumber(L,2);
		  nYpos = (int)Lua_ValueToNumber(L,3);
		  nCellNum = (int)Lua_ValueToNumber(L,4);
		  nScriptidx = g_CheckFileExist((char *)Lua_ValueToString(L,5));

          if (nScriptidx==0)
		  {
			  Lua_PushNumber(L,nReg);
	          return 1;
		  }

		  int nRegion, nMapX, nMapY, nOffX, nOffY;

		  SubWorld[nMapIdx].Mps2Map(nXpos, nYpos, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		  if (nRegion == -1)
		  {
			  Lua_PushNumber(L,nReg);
			  return 1;
		  }
		  
	      //return m_Region[nRegion].SetTrap(nMapX, nMapY,nCellNum,uTrapScriptId);
          //nReg = SubWorld[nMapIdx].SetTrap(nXpos,nYpos,nCellNum,nScriptidx);
		  nReg = SubWorld[nMapIdx].m_Region[nRegion].SetTrap(nMapX,nMapY,nCellNum,nScriptidx);
/*
          int nPlayerIndex   = GetPlayerIndex(L);

          if (nPlayerIndex>0)
		  {

#ifdef _SERVER			 
			  char msg[64];
			  t_sprintf(msg,"<color=yellow>当前坐标:R:%d,X:%d,Y:%d,Tr:%d",nRegion,nMapX,nMapY,nScriptidx);
              //Player[nPlayerIndex].m_ItemList.msgshow(msg);
			  //char nMsg[250];
			  //g_StrCpyLen(nMsg,  Lua_ValueToString(L,1), sizeof(nMsg));
			  int nleg=TEncodeText(msg, strlen(msg));
			  const char * szMsg = msg;
			  if (szMsg)
			  {
				  KPlayerChat::SendSystemInfo(0, 0, "测试:", (char *)szMsg, nleg);  //strlen(szMsg) 
			  }

#endif
		  }
*/
          Lua_PushNumber(L,nReg);
	      return 1;
}

int LuaSubWorldName(Lua_State * L)
{
	int nMapId;
	nMapId = (int)Lua_ValueToNumber(L,1);
    Lua_PushString(L,SubWorld[nMapId].nWorldMapInfo[STR_MAP_NAME].c_str()); 
	return 1;
}


/*
Say(sMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln) 
Say(nMainInfo, nSelCount, SelTab)
如果是客户端的则不会向服务器端发送任何操作

  Say(100, 3, 10, 23,43)
  Say("选择什么？", 2, "是/yes", "否/no");
  Say("选什么呀", 2, SelTab);
*/
//**************************************************************************************************************************************************************
//												界面脚本
//**************************************************************************************************************************************************************

int ExtractChars(const char *inStr,char scrchar,char destchar,char *outStr=NULL,char *outStra=NULL,int nMoedel=0)
{
	char *tmp=NULL,*tmpa=NULL;         //定义一个临时数组空间，存放字符；
	tmp=outStr,tmpa=outStra; 
	int nLen=0;
	while(*inStr!='\0')
	{//一直前行,直到有结束符号将停止。
		if(*inStr==scrchar && nMoedel==0) //数组中的第一个元素与开始字符相同
		{
			//*inStr='|';                 //替换掉开始字符
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。     
           	while(*inStr!='\0')
			{//接着前行   
                  if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {		            
					  //*inStr='|';       //替换掉结束字符
                      inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }
				  *tmpa=*inStr;           //提取字符放进tempa中
				  tmpa++;	              //下一个元素			  
	              inStr++;                //下一个元素
			}
			//inStr++;                   //去掉结束符号
			*tmpa='\0';                  //提取完后，加个结束符号
		}
		else if (*inStr==scrchar && nMoedel==1)  //过滤某个字符后面所有字符串
		{
              break;
		}
		if (*inStr=='\0')                //如果没有找到结束符，并且已经到了结尾，就跳出了！！
			break;
		*tmp=*inStr;                     //元数不相同，则存放到tmp中。
		tmp++;                           //进行下一个元数的比较
		inStr++;                    
		nLen++;
	}
	*tmp='\0';                           //源字符串 提取完后 加个结束符 防止乱码
    return nLen;
}

int LuaSayUI(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0) return 0;

//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	if (Lua_IsNumber(L,2))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2);
	}
	else 
	{
	//	_ASSERT(0);
		return 0;
	}
	
	if  (Lua_IsNumber(L,1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))   // 是一个表跟
	{
		bStringTab = TRUE;
	}
	else 
	{if (nOptionNum > 0)  return 0;
	}
	
	if (bStringTab == FALSE)
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2) nOptionNum = nParamNum - 2;
	}
	
	if (nOptionNum > MAX_ANSWERNUM) nOptionNum = MAX_ANSWERNUM;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId        = UI_SELECTDIALOG;
	UiInfo.m_bParam1      = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum   = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select       = 0;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			t_sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}
	
	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;
	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;		
	
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		char  pAnswer[64];
		ZeroMemory(pAnswer,sizeof(pAnswer));

		if (bStringTab)   //如果输进来的是Table
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3);	 //出栈第三个参数
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 64);
			}
		}
		else 
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 64);
		}
		
		char * pFunName = strstr(pAnswer, "/"); //判断 字符窜 首次出现“/” 的位置!
		
		/*if (pFunName)
		{
			char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分
			if (mpFunName) 			
			{ //只是查找存在与否不需要得到字符串，用strpos效率最高。
				char nmpFunName[32];
				char nmCanshu[32];
				//strpos();
				ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i], nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
				t_sprintf(pContent, "%s|%s", pContent, nmpFunName);
				ZeroMemory(nmpFunName,sizeof(nmpFunName));
			    ZeroMemory(nmCanshu,sizeof(nmCanshu));
			}
			else
			{
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				*pFunName = 0;   //用完清空
				t_sprintf(pContent, "%s|%s", pContent, pAnswer);
			}
		}*/
		if (pFunName)
		{
			char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
			if (mpFunName) 
			{ //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
				char nmpFunName[32];
				char nmCanshu[32];
				//nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
				//nmCanshu=strtok(NULL,"-");        //参数
				ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
				//					 //玩家携带的 函数名称 和 参数
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				//nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
				ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
				t_sprintf(pContent, "%s|%s", pContent,nmpFunName);
				ZeroMemory(nmpFunName,sizeof(nmpFunName));
				ZeroMemory(nmCanshu,sizeof(nmCanshu));
			}
			else
			{
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				*pFunName = 0;   //用完清空
				t_sprintf(pContent, "%s|%s", pContent, pAnswer);
			}
		}
		else 
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			t_sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}
	
	if (nDataType == 0)
		UiInfo.m_nBufferLen  = strlen(pContent);
	else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
	
	UiInfo.m_bParam2 = 0;
	
/*	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	} */
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}


//创建带表格 特定显示SPR的对话框
int LuaCreateSprSay(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   
	   int nParamNum = Lua_GetTopIndex(L);
	   
	   if (nParamNum < 1) 
		   return 0;


	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);

	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;


	   if (Lua_IsTable(L, 1))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,1);       //表中有多少个键

	         if (nOptionNum > MAX_ANSWERNUM+2)
		         nOptionNum = MAX_ANSWERNUM+2;

			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum-2;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum-2;

			   for (int i=0;i<nOptionNum+2;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 1);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					   if (i==0)
					   {//NPC spr 路径
						   t_sprintf(UiInfo.m_szSprPath, "%s", pszString);
						   continue;
					   }
					   else if (i==1)
					   {//主要内容
						   t_sprintf(UiInfo.m_pContent, "%s", pszString);
						   pContent = UiInfo.m_pContent;
						   continue;
					   }
					   else
					     g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号

					   char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //					 //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   t_sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-2],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-2], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-2], "main");  //执行Main函数
						   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   


			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);

			   UiInfo.m_bParam2 = 0;  //客户端
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   } */
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;

}



//新对话框
int LuaCreateTaskSay(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;

       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	  
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   
	   int nParamNum = Lua_GetTopIndex(L);
	   
	   if (nParamNum < 1) 
		   return 0;


	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);
	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;


	   if (Lua_IsTable(L, 1))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,1);       //表中有多少个键

	         if (nOptionNum > MAX_ANSWERNUM+1)
		         nOptionNum = MAX_ANSWERNUM+1;


			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum-1;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum-1;

			   for (int i=0;i<nOptionNum+1;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 1);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					   if (i==0)
					   {//标题
						   t_sprintf(UiInfo.m_pContent, "%s", pszString);
						   pContent = UiInfo.m_pContent;
						   continue;
					   }
					   else
					     g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号

					   char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //					 //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   t_sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], "main");  //执行Main函数
						   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   


			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
			   UiInfo.m_bParam2 = 0;  //客户端
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   }*/
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;
}


//--------------------------
int LuaCreateNewSayEx(Lua_State * L)
{
   char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   if (nPlayerIndex < 0) return 0;

//	   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	   
	   int nParamNum = Lua_GetTopIndex(L);
	   
	   if (nParamNum < 2) 
		   return 0;

	   if  (Lua_IsNumber(L,1))                       //数字
	   {
		   nMainInfo = (int)Lua_ValueToNumber(L,1);  //主要信息
		   nDataType = 1 ;
	   }
	   else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号  字符串
	   {
		   strMain = (char *)Lua_ValueToString(L, 1);
		   nDataType = 0 ;
	   }
	   else
		   return 0;


	   PLAYER_SCRIPTACTION_SYNC UiInfo;
	   ZeroStruct(UiInfo);
	   UiInfo.m_bUIId        = UI_SELECTDIALOG;
	   UiInfo.m_bParam1      = nDataType ;//主信息的类型，字符串(0)或数字(1)
	   // UiInfo.m_bOptionNum   = nOptionNum;
	   UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	   UiInfo.m_Select       = 1;

	  	//主信息为字符串
	  if (nDataType == 0)
	  {  
		if (strMain)
			t_sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	  }  
	  else if (nDataType == 1) //主信息为数字标识
	  {  
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	  }  


	   if (Lua_IsTable(L, 2))  //表格数组
	   {
		       nOptionNum = Lua_GetN(L,2);       //有多少个选项

	         if (nOptionNum > MAX_ANSWERNUM)
		         nOptionNum = MAX_ANSWERNUM;


			   Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;  //选项数	 减去第一个 内容说明

			   UiInfo.m_bOptionNum   = nOptionNum;

			   for (int i=0;i<nOptionNum;i++)  //如果是table
			   {
				   char pAnswer[64];
				   ZeroMemory(pAnswer,sizeof(pAnswer));

				   Lua_PushNumber(L,i + 1); //压入一个Key
				   //lua_settable(L, -3);   //设置KEY值,然后出栈
				   Lua_RawGet(L, 2);        //取得整个脚本传进来的第一个参数的这个对应TABLE的KEY值 然后出栈 
				   char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
				   //nCurNum=4;
				   if (pszString)
				   {
					  
					   g_StrCpyLen(pAnswer, pszString, 64);

					   //i=i-1; //恢复标号

					   char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
					   
					   if (pFunName)
					   {
						   char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
						   if (mpFunName) 
						   { //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
							   char nmpFunName[32]={0};
							   char nmCanshu[32]={0};
							   //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
							   //nmCanshu=strtok(NULL,"-");        //参数
							   ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
							   //					 //玩家携带的 函数名称 和 参数
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
							   ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
							   t_sprintf(pContent, "%s|%s", pContent,nmpFunName);
							   ZeroMemory(nmpFunName,sizeof(nmpFunName));
							   ZeroMemory(nmCanshu,sizeof(nmCanshu));
						   }
						   else
						   {
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i-1],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
							   g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
							   *pFunName = 0;   //用完清空
							   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
						   }
					   }
					   else 
					   {
						   strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i-1], "main");  //执行Main函数
						   t_sprintf(pContent, "%s|%s", pContent, pAnswer);
					   } 
				   }

			   }
			   
			   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
				   UiInfo.m_nBufferLen  = strlen(pContent);
			   else 
				   UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
			   UiInfo.m_bParam2 = 0;  //客户端
			   
/*			   if (nOptionNum == 0)
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
			   }
			   else
			   {
				   Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
			   }*/
			   
			   Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	   }

  return 0;

}

//--------------------------
int LuaSayNew(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	int nIsVal=0;
	int nIsTimeClose=0;
	char * pContent = NULL;
	char * pOptionStr=NULL;
	
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0) return 0;
//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 2) 
		return 0;
	
	if (Lua_IsNumber(L,2))  //第二个参数为 数字
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2);  //有几个链接信息
//		Player[nPlayerIndex].m_nCheckWaiGua=0;
	}
	else 
	{//否则为字符窜
	//	
		 pOptionStr=(char *)Lua_ValueToString(L,2);
		 if (strstr(pOptionStr,"|"))
		 {
		    sscanf(pOptionStr,"%d|%d|%d",&nOptionNum,&nIsVal,&nIsTimeClose);
//		    Player[nPlayerIndex].m_nCheckWaiGua=nIsVal;//(BYTE)Lua_ValueToNumber(L,nCurNum);
		 }
		  else
		  {
			  //_ASSERT(0);
//			  Player[nPlayerIndex].m_nCheckWaiGua=0;
			  return 0;
		  }

	}
	
	if  (Lua_IsNumber(L,1))  //数字
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);  //主要信息
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号  字符串
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3)) //第三个参数字符串 返回假
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))  //表格数组
	{
		bStringTab = TRUE;
	}
	else 
	{
		if (nOptionNum > 0)  return 0;
	}
	
	if (bStringTab == FALSE) //非table
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2)   //减去前面两个参数
			nOptionNum = nParamNum - 2;
	}
	
	if (nOptionNum > MAX_ANSWERNUM) 
		nOptionNum = MAX_ANSWERNUM; //选项显示限制
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId        = UI_SELECTDIALOG;
	UiInfo.m_bParam1      = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum   = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select       = 1;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			t_sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;

		//TEncodeText(UiInfo.m_pContent, strlen(UiInfo.m_pContent));

	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}
	
	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;
	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;  //选项数
	
	//int nCurNum=4;

	for (int i  = 0; i < nOptionNum; i ++)
	{	
		char  pAnswer[64];
		ZeroMemory(pAnswer,sizeof(pAnswer));
		//pAnswer[0]=0;
		
		if (bStringTab)  //如果是table
		{
			Lua_PushNumber(L,i + 1); //压入一个Key
			Lua_RawGet(L, 3);        //取得整个脚本传进来的第三个参数的这个对应TABLE的KEY值 然后出栈  
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			    // nCurNum=4;
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 64);
			}
		}
		else  //否则就是单纯的字符串
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3);	//从第三个参数开始
			       //nCurNum=i + 4;
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 64);
		}
		
		char * pFunName = strstr(pAnswer, "/");  // 查找首次出现 斜杠出现的位置 函数  对大小写敏感
		
		if (pFunName)
		{
			char *mpFunName=strstr(pFunName, "#");  //  stristr() 对大小写不敏感  取后面部分（包括符号）
			if (mpFunName) 
			{ //sscanf(nItemID, "%s(%s", &nTextA, &nTextB); 字符串分割
				char nmpFunName[32]={0};
				char nmCanshu[32]={0};
                     //nmpFunName=strtok(mpFunName,"-");  //字符串分割   函数
                     //nmCanshu=strtok(NULL,"-");        //参数
				     ExtractChars(mpFunName,'(',')',nmpFunName,nmCanshu);
//					 //玩家携带的 函数名称 和 参数
					 g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i],nmpFunName+1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
					 g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],nmCanshu, sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
                     //nmpFunName=strtok(pAnswer, "/"); //去掉 斜杠  //取前面的部分
					 ExtractChars(pAnswer,'/',')',nmpFunName,nmCanshu,1);
			         t_sprintf(pContent, "%s|%s", pContent,nmpFunName);
					 ZeroMemory(nmpFunName,sizeof(nmpFunName));
					 ZeroMemory(nmCanshu,sizeof(nmCanshu));
			}
			else
			{
		    	g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerCanshu[i],"", sizeof(Player[nPlayerIndex].m_szTaskAnswerCanshu[0]));
			    g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			    *pFunName = 0;   //用完清空
			    t_sprintf(pContent, "%s|%s", pContent, pAnswer);
				//printf("--%s--\n",pContent);
			}
		}
		else 
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");  //执行Main函数
			t_sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}

	   if (nDataType == 0)//主信息的类型，字符串(0)或数字(1)
		UiInfo.m_nBufferLen  = strlen(pContent);
	   else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
		
	UiInfo.m_bParam2 = 0;  //客户端
	
/*	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}*/
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);//要求显示某个UI界面

	return 0;
}

//PutMsg(szMsg/MsgId)

int LuaSendMessageInfo(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_MSGINFO;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddGlobalNews(Newsstr)
int LuaAddGlobalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 128);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddLocalNews(Newsstr)
int LuaAddLocalNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nPlayerIndex=0;
		nPlayerIndex = CLIENT_PLAYER_INDEX;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;  //普通消息 显示就消亡
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;  //显示UI界面
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 128);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}
//AddLocalNews(Newsstr)  ,显示一次就消亡
int LuaAddSysNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	int nPlayerIndex=0;
	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_SHUIJI;  //普通消息 显示就消亡
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;  //显示UI界面
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), MAX_SCIRPTACTION_BUFFERNUM);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}
	
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) 
		return 0;
	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo); //客户端直接显示
	return 0;
}


//AddGlobalCountNews(strNew/newid, time)
int LuaAddGlobalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	int nPlayerIndex=0;
		
	nPlayerIndex = CLIENT_PLAYER_INDEX;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;  //倒计时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;

	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
//客户端	
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddLocalCountNews(strNew/newid, time)
int LuaAddLocalCountNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2) 
		return 0;
	

	int nPlayerIndex=0;
	
	nPlayerIndex = CLIENT_PLAYER_INDEX;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;  //倒计时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	int nTime = (int)Lua_ValueToNumber(L,2);
	
	if (nTime <= 0)
		nTime = 1;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int *)((char *)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 256);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
		*(int *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}
//客户端	
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);

	return 0;
}

//AddGlobalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddGlobalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	int nPlayerIndex=0;
		
	nPlayerIndex = CLIENT_PLAYER_INDEX;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;  //定时消息
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 64);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = (SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;//GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddLocalTimeNews(strNew/newid, year,month,day,hour,mins)
int LuaAddLocalTimeNews(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 6) 
		return 0;
	
	int nPlayerIndex=0;
		
	nPlayerIndex = CLIENT_PLAYER_INDEX;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nMsgId = (int)Lua_ValueToNumber(L,1);
		*((int *)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else 
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), 64);
		UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}
	
	SYSTEMTIME *pSystemTime = 	(SYSTEMTIME *)((char *)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));
	
	SYSTEMTIME LocalTime ;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));
	
	LocalTime.wYear = (WORD)Lua_ValueToNumber(L,2);
	LocalTime.wMonth =(WORD)Lua_ValueToNumber(L,3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L,5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L,6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif
	
	//int nPlayerIndex = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

//AddNote(str/strid)
int LuaAddNote(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	nPlayerIndex   = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex < 0)
		return 0;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;
	
	int nParam2 = 0;
	
	
	if  (Lua_IsNumber(L,1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	if (nParamNum > 1)
	{
		nParam2 = (int)Lua_ValueToNumber(L, 2);
	}
	

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_NOTEINFO;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	UiInfo.m_bParam2 = 0;
	
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			t_sprintf(UiInfo.m_pContent, "%s", strMain);
		int nLen = strlen(strMain);
		*(int*)(UiInfo.m_pContent + nLen) = nParam2;
		UiInfo.m_nBufferLen = nLen + sizeof(int);
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)UiInfo.m_pContent = nMainInfo;
		*(int *)(UiInfo.m_pContent + sizeof(int)) = nParam2;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(int);
	}
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*
**
**格式1:Talk(SentenceNum, CallBack-Fun(结束后的回调函数), sTalk1, sTalk2, sTalk3, sTalk4,...sTalkN);  
Talk(SentenceNum, CallBack-Fun(结束后的回调函数), nTalk1, nTalk2,nTalk3,nTalk4,...nTalkN);  
**格式2:Talk(SentenceNum, CallBack-Fun, SentenceTab);
**例子:Talk(3,"EndTalk", "玩家：请问现在几点钟了？", "雇员：现在5点钟了","太谢谢你了！");
**
*/

int LuaTalkUI(Lua_State * L)
{
	int nPlayerIndex ;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	if (nPlayerIndex <= 0) 
		return 0;
//	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3) 
		return 0;
	
	if (Lua_IsNumber(L,1))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,1);
	}
	else 
	{
	
		return 0;
	}
	
	const char * pCallBackFun = Lua_ValueToString(L,2);
	
	//检查主信息是字符串还是字符串标识号
	
	if  (Lua_IsNumber(L,3))
	{
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 3)) 
	{
		nDataType = 0 ;
	}
	else
		return 0;
	
	
	//获得实际传入的选项的个数
	if (nOptionNum > nParamNum - 2) 
		nOptionNum = nParamNum - 2;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	pContent = UiInfo.m_pContent;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		const char * pString = NULL;
		if (!nDataType)//StringInfo
		{
			pString = Lua_ValueToString(L, i + 3); //第三个起为字符窜类
			if (nContentLen  + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
			{
				nOptionNum = i;
				UiInfo.m_bOptionNum = nOptionNum;
				break;
			}
			nContentLen += strlen(pString);
			t_sprintf(pContent, "%s%s|", pContent, pString);
		}
		else
		{
			int j = (int)Lua_ValueToNumber(L, i + 3);
			t_sprintf(pContent, "%s%d|", pContent, j);
		}
	}
	UiInfo.m_nBufferLen  = strlen(pContent);
	
	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
//		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else   //有回调函数
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 1;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
//		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	
	UiInfo.m_bParam2 = 0;
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);

	return 0;
	
}

int LuaIncludeLib(Lua_State * L)
{

	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	
	if (Lua_IsString(L,1))
	{
       	const char * pFileDir = lua_tostring(L,1);	
		//char lszCurrentDirectory[MAX_PATH];
	}
	return 0;
}


int LuaIncludeFile(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;
	
	if (Lua_IsString(L,1))
	{
		//char * pFileName = lua_tostring(L,1);	
		char pFileName[256];
		ZeroMemory(pFileName,sizeof(pFileName));
		t_sprintf(pFileName,lua_tostring(L,1));
		char lszCurrentDirectory[MAX_PATH];
		int nLen = 0;
		if (pFileName[0] != '\\' && pFileName[0] != '/')
		{
			getcwd(lszCurrentDirectory, MAX_PATH);
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32  //32位用  "\\"
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
#else          
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
			for (int i = 0; lszCurrentDirectory[i]; ++i)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		else
		{
			g_GetRootPath(lszCurrentDirectory); //获取软件运行的 绝对路径
			
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
			for (int i = 0; lszCurrentDirectory[i]; ++i)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}

	    //g_StrLower(lszCurrentDirectory + nLen);  //大写转小写	 strlwr
		g_StrLower(lszCurrentDirectory + nLen);

        //客户端只能包含 包文件
		if (!pFileName)
			return 0;
		/*g_StrLower(pFileName);   //大写转小写  g_StrLower
		DWORD dwScriptId = g_FileName2Id(pFileName);
			 pScript = (KLuaScript* )g_GetScript(dwScriptId);
		*/
		//Lua_CompileFile();       //加载包含文件 客户端
		 // char nMsg[128]={0};
          KPakFile	File;
          DWORD		Size;
	      if (!File.Open(pFileName))
		  {  	  
		    // t_sprintf(nMsg,"打开%s失败",pFileName);
		     //KSceGetFliePathc(nMsg,"--",100);
		     return 0;
		  }  	  
		  Size = File.Size();
		  KMemClass Memory;
		  if (!Memory.Alloc(Size + 4))
		  {
		     File.Close();
			 //t_sprintf(nMsg,"分配内存(%s):%u 错误!!",pFileName,Size);
			 //KSceGetFliePathc(nMsg,"无",100);
		     return 0;
		  }	
		  if (File.Read(Memory.GetMemPtr(),Size) != Size)
		  {
		     File.Close();
			// t_sprintf(nMsg,"读取(%s):%u 错误!!",pFileName,Size);
			 //KSceGetFliePathc(nMsg,"无",100);
			 Memory.Free();
		     return 0;
		  }

		  char * pszMem = (char *)Memory.GetMemPtr();
		  pszMem[Size + 1] = 0;
		  
		  File.Close();
			
		  try
		  {
			  if (pszMem)
			  {
				  int state;
				  if (state = Lua_ExecuteBuffer(L,pszMem,Size,NULL)!=0)
				  {
					  //t_sprintf(nMsg,"----执行包含脚本(%s)错误(%u)----",pFileName,state);
					  //KSceGetFliePathc(nMsg,"无",100);
					  Memory.Free();
					  return 0;
				  }
			  }  
		  }
		  catch(...)
		  {
			  //t_sprintf(nMsg,"出错 %s 出现异常,请检查!!",pFileName);
			  //KSceGetFliePathc(nMsg,"无",100);
		  }

		  //if (pszMem)
		  Memory.Free();

		return 0;
	}
	else
		return 0;
}

//**************************************************************************************************************************************************************
//												任务脚本
//**************************************************************************************************************************************************************


//**************************************************************************************************************************************************************
#define MAX_TEMPVALUENUM_INCLIENT 4000
int g_TempValue[MAX_TEMPVALUENUM_INCLIENT];

int LuaGetTaskValue(Lua_State * L)
{
	int nPlayerIndex=0;

	int nTaskId= (int)Lua_ValueToNumber(L,1);
       nPlayerIndex   = CLIENT_PLAYER_INDEX;

       PLAYER_GET_TASKVAL_COMMAND GetTaskval;
	       GetTaskval.ProtocolType = c2s_gettaskval;
           GetTaskval.m_TaskId=nTaskId;
           GetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
	   if (g_pClient)
		   g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));
	      //Player[nPlayerIndex].m_ItemList.LockOperation();

    int nValue=0;

	if (nPlayerIndex > 0) 
	{
		nValue = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);		
		//Lua_PushNumber(L, nValue);
	}
	//else		
	Lua_PushNumber(L, nValue);	
	return 1;
}

int LuaSetTaskValue(Lua_State * L)
{
	int nPlayerIndex;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	int nValueIndex = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);	
	if (nPlayerIndex <= 0) return 0;

	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, nValue);  //编号 和 值

	 PLAYER_SET_TASKVAL_COMMAND SetTaskval;
	 SetTaskval.ProtocolType = c2s_settaskval;
	 SetTaskval.m_TaskId  = nValueIndex;
	 SetTaskval.m_TaskVal = nValue;
	 SetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
	 if (g_pClient)
	   g_pClient->SendPackToServer((BYTE*)&SetTaskval, sizeof(PLAYER_SET_TASKVAL_COMMAND));
	//	int nReg=Player[nPlayerIndex].Save();  //
	return 0;
}

int LuaSetDeathSkill(Lua_State * L)
{
	int nPlayerIndex;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   int nValueIndex = (int)Lua_ValueToNumber(L, 1);
	   int nValue = (int)Lua_ValueToNumber(L, 2);	
	   if (nPlayerIndex <= 0) return 0;
	   
	   Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, nValue);  //编号 和 值
	   //	int nReg=Player[nPlayerIndex].Save();  //
	   return 0;
}

int LuaGetDeathSkill(Lua_State * L)
{
	int nPlayerIndex=0;
	
	int nTaskId= (int)Lua_ValueToNumber(L,1);
	
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   
       PLAYER_GET_TASKVAL_COMMAND GetTaskval;
	   GetTaskval.ProtocolType = c2s_gettaskval;
	   GetTaskval.m_TaskId=nTaskId;
	   GetTaskval.m_pDwid=Npc[Player[nPlayerIndex].m_nIndex].m_dwID;	
	   if (g_pClient)
		   g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));
	   
	   int nValue=0;
	   
	   if (nPlayerIndex > 0) 
	   {
		   nValue = Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);		
		   //Lua_PushNumber(L, nValue);
	   }
	   //else		
	   Lua_PushNumber(L, nValue);	
	   return 1;
}

int LuaAddTaskValue(Lua_State * L)
{
	int nPlayerIndex;
       nPlayerIndex   = CLIENT_PLAYER_INDEX;
	int nValueIndex = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L, 2);

	int nValueOld = (int)Player[nPlayerIndex].m_cTask.GetSaveVal(nValueIndex);
	
	if (nPlayerIndex <= 0)
	    return 0;

	    Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex,nValueOld + nValue);
	return 0;
}

int LuaMathematical(Lua_State * L)
{
	return 0;
}

int  LuaMessage(Lua_State * L)
{
	const char * szString;
	szString  = lua_tostring (L,1);
	printf("--%s-- \n",(char *)szString);
	return 0;
}
//***************************************************************************8***
int LuaGetTempTaskValue(Lua_State * L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	int nPlayerIndex;
	nPlayerIndex=CLIENT_PLAYER_INDEX;
	if (nTempIndex >= 0 && nTempIndex < MAX_TEMPVALUENUM_INCLIENT)
		Lua_PushNumber(L, g_TempValue[nTempIndex]);
	else 
		Lua_PushNil(L);
	return 1;
}

int LuaSetTempTaskValue(Lua_State * L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L) - 1);
	int nValue = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	int nPlayerIndex;
	nPlayerIndex=CLIENT_PLAYER_INDEX;
	g_TempValue[nTempIndex] = nValue;
	return 0;
}

//获取列 数和行数
int LuaGetTabRowAndCol(Lua_State * L) 
{
	KTabFile  TableFile;
	char *FiledName=NULL,*LieName=NULL;
	int nReg=0,mLie=0,mRow=0;
	FiledName = (char *)Lua_ValueToString(L, 1);
	nReg=TableFile.Load(FiledName);
    //TableFile.TabFileCtrl->WriteString(1,1,"测试写入");
	if (nReg)
	{
		mLie= TableFile.GetWidth();        //总列数
		mRow= TableFile.GetHeight()-1;     //总行数
	}
	TableFile.Clear();
	Lua_PushNumber(L,mRow);
    Lua_PushNumber(L,mLie);
	return 2;
}


//获取INI某节 某键的值
int LuaGetIniFileInfo(Lua_State * L)
{  
	KIniFile  IniFile;
	char *FiledName=NULL,*JieName=NULL,*KeyName=NULL;
	    FiledName =(char *)Lua_ValueToString(L,1);
		JieName =(char *)Lua_ValueToString(L, 2);
		KeyName =(char *)Lua_ValueToString(L, 3);
     int nIsType=0;
		 nIsType=(int)Lua_ValueToNumber(L, 4);
	 int nInVal=0;
	 char nstrVal[256]={0};
	 if (!IniFile.Load(FiledName))
		 return 0;

	 if  (nIsType==0)
	 {
	      IniFile.GetInteger(JieName,KeyName,0,&nInVal);
		  IniFile.Clear();
		  Lua_PushNumber(L,nInVal); 
          return 1;
	 }
	 else
	 {
		 IniFile.GetString(JieName,KeyName,"",nstrVal,sizeof(nstrVal));
		 IniFile.Clear();
		 Lua_PushString(L,nstrVal); 
         return 1;
	 }

}

//获取某行 某列的值
int LuaGetTabFileInfo(Lua_State * L)
{
	KTabFile  TableFile;
	char *FiledName=NULL,*LieName=NULL;
	char nVal[512]={0};
	ZeroMemory(nVal,sizeof(nVal));
	int mLiem;
	FiledName =(char *)Lua_ValueToString(L, 1);
	//int mRowm=(int)Lua_ValueToNumber(L, 2);
	if(!TableFile.Load(FiledName))
		return 0;
	
    if (Lua_IsNumber(L,2))
	{
         int mRowm=(int)Lua_ValueToNumber(L, 2);

		 if (Lua_IsNumber(L,3))
		 { 
			 mLiem=(int)Lua_ValueToNumber(L, 3);
			 TableFile.GetString(mRowm,mLiem,"",nVal,sizeof(nVal));
		 } 
		 else
		 {
			 LieName = (char *)Lua_ValueToString(L, 3); 
			 TableFile.GetString(mRowm,LieName,"",nVal,sizeof(nVal));
		 } 

	}
	else
	{
        char *mRowm=(char *)Lua_ValueToString(L, 2);
		
		if (Lua_IsNumber(L,3))
		{ 
			mLiem=(int)Lua_ValueToNumber(L, 3);
			TableFile.GetString(mRowm,mLiem,"",nVal,sizeof(nVal));
		} 
		else
		{
			LieName = (char *)Lua_ValueToString(L, 3); 
			TableFile.GetString(mRowm,LieName,"",nVal,sizeof(nVal));
		} 
	}

	TableFile.Clear();
	Lua_PushString(L,nVal);
	
	return 1;
}

int LuaGetTaskAwardMatrix(Lua_State * L)
{
	KTabFile TableFile;
	//  KTabFile    TableFile;     // 读取TXT文件 返回表格
	char *FiledName=NULL,*nKeyName=NULL,*nTxtName=NULL;
	int nRows=0,nReg=0,mLie=0,mRow=0;
	
	FiledName = (char *)Lua_ValueToString(L, 1);
    nKeyName  = (char *)Lua_ValueToString(L, 2);  //列的字段
	nTxtName  = (char *)Lua_ValueToString(L, 3);  //需要寻找的名称
	
    if (!TableFile.Load(FiledName))
		return 0;
    int i ,nStartPos=0,nEndPos=0;
    nRows = TableFile.GetHeight();
    mLie  = TableFile.GetWidth();
	char nTitle[64]={0};

	for (i=2;i<nRows+1;++i)
	{
	
		TableFile.GetString(i,nKeyName,"",nTitle,sizeof(nTitle));
		if (strstr(nTitle,nTxtName))
		{
			nStartPos = i;
			break;
		}
	}
	
	for (i=nStartPos;i<nRows+1;++i)
	{
		TableFile.GetString(i,nKeyName,"",nTitle,sizeof(nTitle));
		if (strstr(nTitle,nTxtName))
		{
			nEndPos = i;
			nReg++;	   //有多少行
		}
		else
			break;     //否则跳出  只能是 连续的行号
	}
	
	TableFile.Clear();
	Lua_PushNumber(L,nReg); 
	Lua_PushNumber(L,mLie); 
	Lua_PushNumber(L,nStartPos); 
	Lua_PushNumber(L,nEndPos);  
	return 4;
}


int LuaCalcFreeItemCellCount(Lua_State * L)
{
int nPlayerIndex;
    nPlayerIndex   = CLIENT_PLAYER_INDEX;
	   int nCount = 0;
	   if (nPlayerIndex > 0)
	   {
		   int x,y;
		   for (x=0;x<6;x++)
			   for (y=0;y<10;y++)
			   {
		           if (!Player[nPlayerIndex].m_ItemList.m_Room[room_equipment].FindItem(x, y))
				   {//没有东西 就增加 不连续的空间
					   nCount ++;
				   }
			   }
	   }
	   Lua_PushNumber(L, nCount);
 return 1;
}

int LuaGetPositionCount(Lua_State * L)
{
	int nPlayerIndex;

       nPlayerIndex   = CLIENT_PLAYER_INDEX;

	int nWpos  = (int)Lua_ValueToNumber(L,1);
	int nHpos  = (int)Lua_ValueToNumber(L,2);
    int nCount = 0;
    if (nPlayerIndex > 0)
    {
		ItemPos Pos;
		ZeroStruct(Pos);
		if (Player[nPlayerIndex].m_ItemList.SearchPosition(nWpos,nHpos,&Pos))
		{//查找位置	连续空间
			nCount=nWpos*nHpos;
		}
		ZeroStruct(Pos);
	}
	Lua_PushNumber(L, nCount);
    return 1;
}

//获取当前时间（按照模式 格式化）
int LuaGetLocalDate(Lua_State * L)
{
    char * nTimeStr = (char *)Lua_ValueToString(L,1);

	time_t rawtime;                  //定义一个long 型存放秒数
	struct tm * timeinfo;	
	time (&rawtime);                 //距离现在的时间（秒）
	timeinfo = localtime(&rawtime);  //把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间

	char strTime[80];
    strftime(strTime,sizeof(strTime),nTimeStr,timeinfo);

    Lua_PushString(L,strTime);
    return 1;
}
//打开进度条
int LuaOpenJinDuTiao(Lua_State * L)
{
	//int nPlayerIndex;

    const	char *mTitle=(char *)Lua_ValueToString(L,1);   //标题
	int  nMsgIndex=(int )Lua_ValueToNumber(L,2);           //模板ID  是那个进度条
	int  nTimes=(int )Lua_ValueToNumber(L,3);              //持续多少后执行回调函数
    const	char *mFun=(char *)Lua_ValueToString(L,4);     //回调函数名称
    const   char *szParma=(char *)Lua_ValueToString(L,5);  //传递参数一
    const	char *szParmb=(char *)Lua_ValueToString(L,6);  //传递参数二
    const	char *szParmc=(char *)Lua_ValueToString(L,7);  //传递参数三    
    return 0;
}


int LuaGetRandomNum(Lua_State * L)
{

	int nMaxNum = (int)Lua_ValueToNumber(L, 1);
	if (nMaxNum <0)
		nMaxNum=0;

	char nsg[64]={0};
	t_sprintf(nsg,"%d",g_Random(nMaxNum));
	Lua_PushString(L,nsg); 
	return 1;
	
}




int LuaSetRandomSeed(Lua_State * L)
{
	int nSeed = (int)Lua_ValueToNumber(L, 1);

	if  (nSeed<=0)
		 nSeed=1;

	g_RandomSeed(nSeed);

	return 0;
	
}

int LuaGetRandomSeed(Lua_State * L)
{
	Lua_PushNumber(L,g_GetRandomSeed());
	return 1;

}

int LuaNewRandom(Lua_State * L)
{

	int nMinNum = (int)Lua_ValueToNumber(L, 1);
	int nMaxNum = (int)Lua_ValueToNumber(L, 2);

	if (nMinNum<0 || nMaxNum<0) return 0;

	if (nMinNum>nMaxNum)
	{//交换
	   int inTemp= nMinNum;
	   nMinNum=nMaxNum;
	   nMaxNum=inTemp;
	}

	srand((unsigned)time(0));

    Lua_PushNumber(L,rand()%(nMaxNum-nMinNum)+nMinNum); 

	return 1;
	
}


int LuaTabFile_UnLoad(Lua_State * L)
{
	return 0;	
}
/*
int LuaTabFile_GetRowCount(Lua_State * L)
{
   return 0;	
}

int LuaTabFile_GetCell(Lua_State * L)
{
  return 0;
}
*/
int LuaTabFile_Load(Lua_State * L)
{
   KTabFile TableFile;

   char *FiledName=NULL,*nKeyName=NULL,*LieName=NULL;
   int nRows=0,nReg=0,mLie=0,mRow=0;

   FiledName = (char *)Lua_ValueToString(L, 1);
   nKeyName = (char *)Lua_ValueToString(L, 2);  //列的字段
   nReg= TableFile.Load(FiledName);
   
  /*
   int mRowm=(int)Lua_ValueToNumber(L, 3);
   if (Lua_IsString(L,4))
   {  

        LieName = (char *)Lua_ValueToString(L, 4); 
        nReg=1;
   }  
   else if (Lua_IsNumber(L,4))
   { 
       mLie=(int)Lua_ValueToNumber(L, 4);
       nReg=1;
   } 
*/
   TableFile.Clear();
   Lua_PushNumber(L,nReg); 
//Lua_PushUserTag(L,,TableFile);
   return 1;

}


int LuaFileName2Id(Lua_State * L)
{
	char *strScript = (char *)Lua_ValueToString(L ,1);

	if (strScript)
      Lua_PushNumber(L, g_FileName2Id(strScript));
	else
	  Lua_PushNumber(L, 0);

	return 1;
}



//设置包袱为空--修复函数
int LuaSetRoomNull(Lua_State * L)
{
   int nPlayerIndex,mreg=0;

       nPlayerIndex   = CLIENT_PLAYER_INDEX;
/*
  	room_equipment = 0,	//0 装备栏-包袱
	room_repository,	//1 贮物箱
	room_exbox1,		//2 扩展箱1
	room_exbox2,		//3 扩展箱2
	room_exbox3,		//4 扩展箱3
	room_equipmentex,	//5 同伴包袱
	room_trade,			//6 交易栏
	room_tradeback,		//7 交易过程中装备栏的备份
	room_trade1,		//8 交易过程中对方的交易栏
	room_immediacy,		//9 快捷物品
	room_give,          //10 给予空间
	room_giveback,      //11 给予回调空间 
	room_dazao,         //12 打造框
	room_cailiao,       //13 材料框
	room_num,			// 空间数量

*/
	   int  nRoomidx=(int )Lua_ValueToNumber(L,1);

            mreg=Player[nPlayerIndex].m_ItemList.m_Room[nRoomidx].SetRoomNull();
  //#include	"KItemList.h"
	Lua_PushNumber(L,mreg);
	return 1;
}

//获取某个动作下的NPC SPR路径
int LuaGetMaskActionSpr(Lua_State * L)
{

	 int  nNpcSetIdx=(int )Lua_ValueToNumber(L,1);
	 int  nAction   =(int )Lua_ValueToNumber(L,2);

	 char nNpcKind[32]={0},nNpcRePath[128],nNpcActSpr[64]={0},nNpcSprFullPath[256]={0};
	 g_NpcSetting.GetString(nNpcSetIdx,"NpcResType","enemy003",nNpcKind,sizeof(nNpcKind));

	 KTabFile nNpcType;

	 if (nNpcType.Load("\\settings\\npcres\\人物类型.txt"))
	 {
		 nNpcType.GetString(nNpcKind,3,"",nNpcRePath,sizeof(nNpcRePath));
	 }
	 nNpcType.Clear();

	 if (nNpcType.Load("\\settings\\npcres\\普通npc资源.txt"))
	 {
		 nNpcType.GetString(nNpcKind,nAction+1,"",nNpcActSpr,sizeof(nNpcActSpr));
	 }
	 nNpcType.Clear();

	 if (nNpcRePath[0] && nNpcActSpr[0])
		t_sprintf(nNpcSprFullPath,"\\%s\\%s",nNpcRePath,nNpcActSpr);
	 else
		return 0;

	 Lua_PushString(L, nNpcSprFullPath);
	 return 1;
}


//SetPos(X,Y) //跑向
int LuaSetPos(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 2) return 0;
	int nPlayerIndex=0;
	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
	if  (nPlayerIndex<=0) return 0;

	int nX = (int) Lua_ValueToNumber(L,1);
	int nY = (int) Lua_ValueToNumber(L,2);
	int nResult=0;
	
	if (nPlayerIndex > 0)
	{
		PLAYER_SYN_POS mPos;
		mPos.ProtocolType = c2s_playersetpos;
		mPos.nXpos = nX;
		mPos.nYpos = nY;
		if (g_pClient)
			g_pClient->SendPackToServer((BYTE*)&mPos, sizeof(PLAYER_SYN_POS));
		nResult = 1 ;

	}
	
	Lua_PushNumber(L, nResult);
	return 1;
}


int LuaCalcEquiproomItemCount(Lua_State * L)
{
	
int nPlayerIndex=0;
	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
	if  (nPlayerIndex<=0) return 0;
	int nCout=0;
	int nItemGen = (int)Lua_ValueToNumber(L, 1);
    int nDetail  = (int)Lua_ValueToNumber(L, 2);
	int nParticular    = (int)Lua_ValueToNumber(L, 3);
	int nLevel   = (int)Lua_ValueToNumber(L, 4);
	
	
	nCout = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemNum(nItemGen, nDetail, nParticular, nLevel);
	
	
	Lua_PushNumber(L,nCout);
    return 1;
}


//门派
int LuaGetPlayerFaction(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	nPlayerIndex = CLIENT_PLAYER_INDEX;
				
	if (nPlayerIndex > 0)								
	{	
		char FactionName[64]={0};
		Player[nPlayerIndex].GetFactionName(FactionName, 64);
		Lua_PushString(L, FactionName);
	}
	else
	{
		Lua_PushString(L,"");
	}
	return 1;
}


int LuaGetLastFactionNumber(Lua_State * L)
{
	int nPlayerIndex = CLIENT_PLAYER_INDEX;
		if (nPlayerIndex <= 0)
			Lua_PushNumber(L,-1);
		else
			Lua_PushNumber(L, Player[nPlayerIndex].m_cFaction.m_nFirstAddFaction);


	return 1;
}


int LuaGetLevel(Lua_State * L)
{
	
	if (Lua_GetTopIndex(L) >0)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		Lua_PushNumber(L, Npc[nNpcIdx].m_Level);
	}
	else
	{
		int nPlayerIndex =  CLIENT_PLAYER_INDEX;
		if (nPlayerIndex <= 0)		Lua_PushNumber(L,0);	
		if (Player[nPlayerIndex].m_nIndex <= 0) Lua_PushNumber(L,0);
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	}
	return 1;
}


//W,X,Y = GetWorldPos()
int LuaGetNewWorldPos(Lua_State * L)
{
	int nPlayerIndex = CLIENT_PLAYER_INDEX;
		
	if (nPlayerIndex > 0)
	{
		int nPosX = 0,nPosY = 0,nPmap=0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY,&nPmap);
		
		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}
		
		Lua_PushNumber(L, nSubWorldID); 
		Lua_PushNumber(L, ((int)(nPosX / 32)));
		Lua_PushNumber(L, ((int)(nPosY / 32)));
	}
	else
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 3;
	}
	return 3;
}



//==============以上为通用指令===============服务器端脚本函数开始=======================
int LuaPlayMusic(Lua_State * L)//PlayMusic(musicname,loop=1, vol );
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;
	
	int nPlayerIndex=0;
	
#ifdef _SERVER
	nPlayerIndex = GetPlayerIndex(L);
#else	
	nPlayerIndex = CLIENT_PLAYER_INDEX;
#endif

	//int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	ZeroStruct(UiInfo);
	UiInfo.m_bUIId = UI_PLAYMUSIC;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
//	Player[nPlayerIndex].m_nCheckWaiGua =0;
	int nMsgId = 0;
	
	g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L,1), sizeof(UiInfo.m_pContent));
	UiInfo.m_nBufferLen = strlen(((char *)UiInfo.m_pContent));
	UiInfo.m_bParam1 = 0;
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaFadeInMusic(Lua_State * L)
{
	return 0;
}

int LuaFadeOutMusic(Lua_State * L)
{
	return 0;
}

//===========================客户端脚本函数========================
#ifndef _SERVER
int LuaPlaySound(Lua_State * L)
{
	return 0;
}

/*
int Luadostring(Lua_State * L)
{
    char *Fun=(char *)Lua_ValueToString(L,1);
	lua_dostring(L,Fun);
	return 0;
}
*/

int LuaNewTask_AddNpcFindPath(Lua_State * L)
{
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    int nMsgIndex = (int)Lua_ValueToNumber(L,2);
	int nTaskMapId = (int)Lua_ValueToNumber(L,3);
	int nMapXpos = (int)Lua_ValueToNumber(L,4);
	int nMapYpos = (int)Lua_ValueToNumber(L,5);
	// Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
	//t_sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
	KTaskPathInfo nInfo;
        nInfo.nTaskType=nTaskGenre;
        nInfo.nMsgIndex=nMsgIndex;
		nInfo.nMapIdx=nTaskMapId;
		nInfo.nMapXpos=nMapXpos;
		nInfo.nMapYpos=nMapYpos;
	CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&nInfo,1,0);
   return 0;
}

int LuaNewTask_SetFinishedTask(Lua_State * L)
{
	return 0;
}

int LuaNewTask_DetailTextOut(Lua_State * L)
{	
	int nParamCount = Lua_GetTopIndex(L);
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    char *nTaskDesc=(char *)Lua_ValueToString(L,2);
	int nIsBtn=0;
	if  (nParamCount>=3)
		nIsBtn = (int)Lua_ValueToNumber(L,3);
	// Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
	//t_sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
	KNewsMessage News;
	//主信息为字符串 
	//g_StrCpyLen(News.sMsg, nTaskDesc, sizeof(News.sMsg));
	News.nType =	nTaskGenre;
	t_sprintf(News.sMsg,"%s",nTaskDesc);
	// News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
	//SYSTEMTIME systime;
	//memset(&systime, 0, sizeof(SYSTEMTIME));
	CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&News,0,nIsBtn);//;(unsigned int)&News
	return 0;
}

int LuaNewTask_TaskTextOut(Lua_State * L)
{	
	int nParamCount = Lua_GetTopIndex(L);
    int nTaskGenre = (int)Lua_ValueToNumber(L,1);
    char *nTaskDesc=(char *)Lua_ValueToString(L,2);
	int nIsBtn=0;
	if  (nParamCount>=3)
		nIsBtn = (int)Lua_ValueToNumber(L,3);

  // Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskGenre=nTaskGenre;
   //t_sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskDesc,nTaskDesc);
   KNewsMessage News;
   //主信息为字符串 
   //g_StrCpyLen(News.sMsg, nTaskDesc, sizeof(News.sMsg));
   News.nType =	nTaskGenre;
   t_sprintf(News.sMsg,"%s",nTaskDesc);
  // News.nMsgLen = TEncodeText(News.sMsg, strlen(News.sMsg));
   //SYSTEMTIME systime;
   //memset(&systime, 0, sizeof(SYSTEMTIME));
   CoreDataChanged(GDCNI_TASK_INFO,(unsigned int)&News,0,nIsBtn);//;(unsigned int)&News
   return 0;
}

int LuaNewTask_AddIcon(Lua_State * L)
{ 
	int nTaskType = (int)Lua_ValueToNumber(L, 1);
    char *nTaskIcocPath = (char *)Lua_ValueToString(L, 2);
	int nTaskIdx = (int)Lua_ValueToNumber(L, 3);
	char *nTaskName = (char *)Lua_ValueToString(L, 4);
	if (nTaskIdx>20 || nTaskIdx<0)
		return 0;

	Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskType=nTaskType;
    Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskidx=nTaskIdx;
	t_sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskIconPath,nTaskIcocPath);
	t_sprintf(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[nTaskIdx].nTaskName,nTaskName);

    Lua_PushNumber(L,1);
	return 1;
}
//客户端执行自动封挂脚本
int LuaAutoForbitGua(Lua_State * L)
{
	//int nPlayerIndex = GetPlayerIndex(L);
/*	int nFReg=0;
 //////////////////检测外挂程序/////////////////////////
	int IsForBit[2];  //默认开启封挂
	g_GameSetTing.GetInt2("SYSTEM","IsForBitGua",IsForBit);
	
	int nMun=0;
	int wgRows=g_ForbitWaiGua.GetHeight()+1;

	for (int i=2;i<wgRows;++i)
	{
		char nKey[16],nReg=0;
		g_ForbitWaiGua.GetString(i+2,"GuaNameKey","无",nKey,sizeof(nKey));
		nReg= g_ForBitGua.GetProcessList(nKey,wgRows,i);//CheckWaiGua(nKey);
		if (nReg>=1)
		{//检测到外挂 

			nFReg=1;
			break;
		}
		else if (nReg<0)  
		{//提权失败
			nFReg=-1;
			break;
		}
		else  if (nReg==0)
		{//没有外挂
			nMun++;
//			break;
		}
		g_ForBitGua.Colse();
	}
	if (nMun==wgRows) //行数相等通过外挂检测
	{//同步外挂参数给服务器端
		SKILL_LEFT_SYNC	sLeftSkill;
		sLeftSkill.ProtocolType = c2s_skillsync;
		sLeftSkill.m_nLeftskill =0;
		sLeftSkill.m_Type=1;
		
//		if (g_pClient)
	//	g_pClient->SendPackToServer(&sLeftSkill, sizeof(SKILL_LEFT_SYNC));
		nFReg=0;
	} 
    Lua_PushNumber(L,nFReg);*/
	return 0;
}

//瞬间技能特效（类型0 自己释放 1为指定索引NPC释放）
int LuaCastSkill(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex<0) return 0;
	int nIndexID=0;
	int nStateID=0;
    int nType = (int)Lua_ValueToNumber(L, 1);
	nIndexID = (int)Lua_ValueToNumber(L, 2);
	nStateID = (int)Lua_ValueToNumber(L, 3);
	if (nType==0)
        Npc[Player[nPlayerIndex].m_nIndex].SetInstantSpr(nStateID); 
    else if (nType==1)
		Npc[nIndexID].SetInstantSpr(nStateID);     //释放一个瞬间特效
	
	return 0;    
}

int LuaPlaySprMovie(Lua_State * L)
{
	return 0;
}
#endif
//==============================================================
TLua_Funcs GameScriptFuns[] = 
{
	//通用指令	
	{"Say", LuaSayUI},
	{"SayNew",LuaSayNew},
	{"Describe",LuaSayNew},
	{"CreateTaskSay",LuaCreateTaskSay},	 //支持表格
	{"CreateNewSayEx",LuaCreateNewSayEx},
	{"CreateSprSay",LuaCreateSprSay},

	{"Talk", LuaTalkUI},
	{"GetTaskTemp", LuaGetTempTaskValue},
	{"SetTaskTemp", LuaSetTempTaskValue},
	{"print", LuaMessage},	  //print	 printf
	{"Mathematical", LuaMathematical},
	{"GetBit",	LuaGetBit},
	{"GetByte",	LuaGetByte},
	{"SetBit",	LuaSetBit},
	{"SetByte",	LuaSetByte},
	{"Include",LuaIncludeFile},
	{"IncludeLib",LuaIncludeLib},
	{"IL",LuaIncludeLib},
	{"PutMessage", LuaSendMessageInfo},
	{"AddGlobalNews",LuaAddGlobalNews},
	{"AddGlobalTimeNews",LuaAddGlobalTimeNews},
	{"AddGlobalCountNews",LuaAddGlobalCountNews},
	{"AddLocalNews",LuaAddLocalNews},
	{"AddSysNews"  ,LuaAddSysNews},
	{"AddLocalTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews},
	{"GetTask",			LuaGetTaskValue},	    //GetTask(任务号):获得当前玩家该任务号的值
	{"SetTask",			LuaSetTaskValue},	    //SetTask(任务号,值):设置任务值
	{"SetDeathSkill",			LuaSetDeathSkill},	    //SetTask(任务号,值):设置任务值
	{"GetDeathSkill",			LuaGetDeathSkill},
	{"AddTask",			LuaAddTaskValue},	    //
	{"GetTabFileInfo",  LuaGetTabFileInfo},     //获取TXT文件 某行 某列 的内容 ，返回字符串类型  3个参数
	{"GetIniFileInfo",  LuaGetIniFileInfo},     //获取ini文件 某节 某键 的内容
	
	{"GetTabRowAndCol", LuaGetTabRowAndCol},    //获取文件的 总行数和列数   1个参数

	{"IniFile_GetData",  LuaGetIniFileInfo},
	{"TaskAwardMatrix", LuaGetTaskAwardMatrix}, //获取任务 同一个名称的 有多少行
    {"CalcFreeItemCellCount", LuaCalcFreeItemCellCount},
	{"GetPositionCount",        LuaGetPositionCount},

	{"GetLocalDate", LuaGetLocalDate},
	{"OpenJinDuTiao", LuaOpenJinDuTiao},
	{"SetRoomNull",	LuaSetRoomNull},
	{"FileName2Id",	LuaFileName2Id},

	{"TabFile_Load",             LuaTabFile_Load},         //加载TXT文件
	{"TabFile_GetCell",          LuaGetTabFileInfo},
	{"TabFile_GetRowCount",      LuaGetTabRowAndCol},
	{"TabFile_UnLoad",           LuaTabFile_UnLoad},
	{"GetRandomSeed",           LuaGetRandomSeed},
	{"GetRandomNum",           LuaGetRandomNum},
	{"NewRandom",           LuaNewRandom},
	{"SetRandomSeed",       LuaSetRandomSeed},

	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()返回于NewWorld配陪的坐标
	{"GetLevel",		LuaGetLevel},			        //GetLevel()GetPlayers Level
	{"GetLastFactionNumber", LuaGetLastFactionNumber},
	{"GetFaction",		LuaGetPlayerFaction},    //GetFaction()获得玩家的门派名
	{"GetLastAddFaction",LuaGetPlayerFaction},	
	{"GetMaskActionSpr", LuaGetMaskActionSpr},

	{"CalcEquiproomItemCount", LuaCalcEquiproomItemCount},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)进入某点
	//客户端脚本指令
	{"PlaySound",       LuaPlaySound},      //PlaySound(Sound);
	{"PlaySprMovie",    LuaPlaySprMovie},   //PlaySprMovie(npcindex, Movie, times)
	{"AutoForbitGua",   LuaAutoForbitGua},  //客户端执行自动封挂
	{"CastSkill",       LuaCastSkill},      //瞬间特效
	{"NewTask_AddIcon", LuaNewTask_AddIcon},
	{"NewTask_TaskTextOut", LuaNewTask_TaskTextOut},
	{"NewTask_DetailTextOut", LuaNewTask_DetailTextOut},

	{"NewTask_SetFinishedTask", LuaNewTask_SetFinishedTask},   //设置完成任务
	
	{"NewTask_AddNpcFindPath",  LuaNewTask_AddNpcFindPath},     //设置信息关联寻路
	//{"dostring", Luadostring},
	{"PlayMusic",       LuaPlayMusic}, //PlayMusic(Music,Loop)
	{"FadeInMusic",     LuaFadeInMusic},
	{"FadeOutMusic",    LuaFadeOutMusic},
};


TLua_Funcs WorldScriptFuns[] =// 非指对玩家的脚本指令集
{
	//通用指令
	{"AddLocalNews",     LuaAddLocalNews},
	{"AddSysNews",       LuaAddSysNews},
	{"AddLoaclTimeNews", LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews},
}; 

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(GameScriptFuns[0]);
}

int g_GetWorldScriptFunNum()
{
	return sizeof(WorldScriptFuns)  / sizeof(WorldScriptFuns[0]);
}
