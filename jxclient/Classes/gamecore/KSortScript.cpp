#include "KCore.h"
#include "KSortScript.h"
#include "LuaFuns.h"
#include "engine/KFilePath.h"
#include "engine/KTabFile.h"
#include "engine/KTabFileCtrl.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif

KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET];   //脚本数量限制
//KScriptBinTree g_ScriptBinTree;
unsigned int   nCurrentScriptNum;

void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir);
//重载操作符号
/*int	operator<(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() < ScriptRight.GetScriptID();
};
	
int operator==(KSortScriptNode ScriptLeft, KSortScriptNode ScriptRight)
{
	return ScriptLeft.GetScriptID() == ScriptRight.GetScriptID();
};
 */
//-------------新的----------------------------------------------
#include <algorithm>

class KLuaScriptRateMap : public std::map<DWORD, KLuaScript *>
{
private:
	static VOID _FreeNode(value_type &cValue)
	{
		//SAFE_DELETE(cValue.second);
		if (cValue.second)
		{//KEY值
			delete (cValue.second);
			(cValue.second) = NULL;
		} 
	}
	
public:
	~KLuaScriptRateMap()
	{//删除全部节点
/*#ifdef _SERVER
		std::for_each(begin(), end(), _FreeNode);
#endif*/

	}
};

static KLuaScriptRateMap g_ScriptRateMap;	// 为了保证内存管理的安全，该变量不再对外


const KScript * g_GetScript(DWORD dwScriptId)
{//已经加载脚本 查找栈中的节点
	//KSortScriptNode ScriptNode;
	 //               ScriptNode.SetScriptID(dwScriptId);
	/*if (g_ScriptBinTree.Find(ScriptNode))
	{
		return ScriptNode.GetScript();
	} */

	if (g_ScriptRateMap.count(dwScriptId) > 0)
	{
		return g_ScriptRateMap[dwScriptId];
	} 

	return NULL;
}

const KScript * g_GetScript(const char * szRelativeScriptFile)
{
	DWORD dwScriptId = g_CheckFileExist((LPSTR)szRelativeScriptFile);
	return g_GetScript(dwScriptId);
}
///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
extern int LuaIncludeFile(Lua_State * L);
extern int LuaIncludeLib(Lua_State * L);

static void KSceGetFliePath(char* nName,char * nPath,int nMaxRows)
{
	KFile nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	t_sprintf(nTongApplyPath,"%s","\\clientscript.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		//t_sprintf(szCol,"地图\t名称\t源脚本\15\n");
		t_sprintf(szCol,"脚本Log\15\n");
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
			//nScirptFile.Clear();
			//return;
			while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}
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
//加载脚本，该文件名参数为相对目录
static BOOL LoadScriptToSortListA(char * szRelativeFile)
{
	if (!szRelativeFile || !szRelativeFile[0]) return FALSE;
	//KSortScriptNode ScriptNode ;
	//ScriptNode.SetScriptID(g_FileName2Id(szRelativeFile));  //设置节点索引
	int t  =strlen(szRelativeFile);

	if (t >= 256)
		CCMessageBox("path is to long ","script");
		//printf("[脚本]加载脚本[%s]长度过长,该脚本可能出错!请检查! \n", szRelativeFile);
	
	if (nCurrentScriptNum < MAX_SCRIPT_IN_SET)  //脚本限制？
	{
		g_ScriptSet[nCurrentScriptNum].Init();  //初始化脚本基本库
		g_ScriptSet[nCurrentScriptNum].RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum()); //注册游戏函数
		g_StrCpyLen(g_ScriptSet[nCurrentScriptNum].m_szScriptName, szRelativeFile, 256);
		if (!g_ScriptSet[nCurrentScriptNum].Load(szRelativeFile))  //可以加载PAK文件的
		{
			//printf("-[脚本]加载[%s]出错,请检查!-\n", szRelativeFile);
			g_ScriptSet[nCurrentScriptNum].Exit();
			return FALSE;
		}		
	}
	else
	{
		//printf("-脚本]脚本数量超限制[%d]!请立即解决!-\n", MAX_SCRIPT_IN_SET);
		return FALSE;
	}

	 g_ScriptRateMap[g_CheckFileExist(szRelativeFile)] = &g_ScriptSet[nCurrentScriptNum];
     nCurrentScriptNum ++;
	//ScriptNode.SetScriptIndex(nCurrentScriptNum++);	//设置脚本索引
	//g_ScriptBinTree.Insert(ScriptNode);             //加入节点
	return TRUE;
}

//将szFilePath目录下的所有脚本文件加载进ScriptBinTree二叉树中
static unsigned long LoadAllScript(char * szFilePath,BOOL nIsSer)
{
	g_SetFilePath("\\");

	char szFileRootPath[256]={0};
	char szOldRootPath[MAX_PATH]={0};
	//	GetCurrentDirectory(MAX_PATH, szOldRootPath);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_getcwd(szOldRootPath, MAXPATH);//获取了运行目录
	//参数说明：getcwd()会将当前工作目录的绝对路径复制到参数buffer所指的内存空间中
	//参数maxlen为buffer的空间大小。
#else
	std::string nPath =  CCFileUtils::sharedFileUtils()->getWritablePath();
	t_sprintf(szOldRootPath,"%s",nPath.c_str());
#endif
	g_GetFullPath(szFileRootPath,szFilePath);

	if (nIsSer)
	   LoadScriptInDirectory(szFileRootPath,"");  //硬盘文件:全路径; 包文件:相对路径
	//-------------------开始加载PAK的脚本----------------
	char szRealDir[128]={0};
	KTabFile nMapScript;
	t_sprintf(szRealDir,"%s\\%s",szFilePath,"script.txt");
	//KSceGetFliePath("加载成功",szRealDir);
	if (nMapScript.Load(szRealDir))	//包文件是相对文件
	{
		int nRows=nMapScript.GetHeight();
		for (int i=0;i<nRows;++i)
		{
			nMapScript.GetString(i+1,1,"",szRealDir,sizeof(szRealDir));
			g_StrUpper(szRealDir); //转成大写 _strupr g_StrUpper
			if (strstr(szRealDir,".LUA"))
			{
				g_StrLower(szRealDir);   //大写转小写 g_StrLower
				LoadScriptToSortListA(szRealDir);
			}
		}
		nMapScript.Clear();
	}
	//-----------------------------------------------------------------
	chdir(szOldRootPath);   
	//chdir 是C语言中的一个系统调用函数（同cd），
	//用于改变当前工作目录，其参数为Path 目标目录，可以是绝对目录或相对目录。
	//SetCurrentDirectory(szOldRootPath);
	return nCurrentScriptNum;
}

unsigned long g_IniScriptEngine(char * nScriptDir,BOOL nIsSer,BOOL nIsClear)
{
	//g_szCurScriptDir[0] = 0;
	if (nIsClear)
	{
		nCurrentScriptNum = 0;
		//g_ScriptBinTree.ClearList();
       ClearAllScript();
	}
	return LoadAllScript(nScriptDir,nIsSer);
	
}

//加载脚本，该文件名参数为实际目录
static BOOL LoadScriptToSortList(char * szFileName)
{
	if (!szFileName || !szFileName[0]) return FALSE;
	if (nCurrentScriptNum>= MAX_SCRIPT_IN_SET)
	{
		//printf("[Script]脚本总容量超过%d,严重错误请检查!\n",MAX_SCRIPT_IN_SET);
		return FALSE;
	}

	//int nFileNameLen = strlen(szFileName);
	
	char szRootPath[MAX_PATH];	
	g_GetRootPath(szRootPath);
//	char szRelativePath[MAX_PATH];
	char *szRelativePath;
	char szCurrentDirectory[MAX_PATH];

//	GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	getcwd(szCurrentDirectory, MAX_PATH);
	szRelativePath = szCurrentDirectory + strlen(szRootPath);
//	PathRelativePathTo(szRelativePath,szRootPath, FILE_ATTRIBUTE_DIRECTORY,szCurrentDirectory , FILE_ATTRIBUTE_NORMAL );
	char szRelativeFile[MAX_PATH];
	if (szRelativePath[0] == '.' && szRelativePath[1] == '\\')
		t_sprintf(szRelativeFile, "%s\\%s", szRelativePath + 1, szFileName);
	else
		t_sprintf(szRelativeFile, "%s\\%s", szRelativePath, szFileName);

	g_StrLower(szRelativeFile);  //转成小写
//	printf("[Script]Loading Script %s %d\n", szRelativeFile, g_FileName2Id(szRelativeFile));
//
	return LoadScriptToSortListA(szRelativeFile);
//	return FALSE;
}


void	LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir)
{
	int				nFlag;
	char			szRealDir[MAX_PATH];
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	t_sprintf(szRealDir, "%s\\%s", lpszRootDir, lpszSubDir);
#else
	t_sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
        char *ptr = szRealDir;
        while(*ptr) { if(*ptr == '\\') *ptr = '/';  ptr++;  }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if(chdir(szRealDir)) return;
	_finddata_t FindData;
	long dir = _findfirst("*.*", &FindData);
	while(dir != -1)
	 {
		if(strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0)	{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
		if(FindData.attrib == _A_SUBDIR)
		{//属性
			LoadScriptInDirectory(szRealDir, FindData.name);
		}
		else
		{	
			nFlag = 0;
			for (int i = 0; i < (int)strlen(FindData.name);  i++)
			{
				if (FindData.name[i] == '.')
					break;
				if (FindData.name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir, FindData.name);
			}
			else
			{
				char szExt[50];
				if (strlen(FindData.name) >= 4) 
				{
					strcpy(szExt, FindData.name + strlen(FindData.name) - 4);
					g_StrUpper(szExt); //转成大写
					if ((!strcmp(szExt, ".LUA"))/* || (!strcmp(szExt, ".TXT"))*/)
						if (!LoadScriptToSortList(FindData.name))
							printf("-------加载%s文件出错------\n", FindData.name);
				}
			}
		}
		if(_findnext(dir, &FindData)) break;
	} 
	_findclose(dir);
	chdir(lpszRootDir);
#else
     DIR *dp;
     int i;
     struct dirent *ep;
     if(chdir(szRealDir)) return;
     dp = opendir(".");
     if(dp) {
          while(ep = readdir(dp)) {
            if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) continue;
            
            if(ep->d_type ==4) {
                LoadScriptInDirectory(szRealDir, ep->d_name);
            }
            else {
			nFlag = 0;
			for (i = 0; i < (int)strlen(ep->d_name);  i++)
			{
				if (ep->d_name[i] == '.')
					break;
				if (ep->d_name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir,ep->d_name);
			}
			else
			{
				char szExt[50];
				if (strlen(ep->d_name) >= 4)
				{
					strcpy(szExt, ep->d_name + strlen(ep->d_name) - 4);
					g_StrUpper(szExt);  //转成大写g_StrUpper
///					_strupr(szExt);
					if ( (!strcmp(szExt, ".LUA"))/* || (!strcmp(szExt, ".TXT"))*/)
						if (!LoadScriptToSortList(ep->d_name))
							printf("-----加载%s文件出错------\n", ep->d_name);
				}
			}
		}
	          }
          closedir(dp);
     }
	chdir(lpszRootDir);
#endif
}

void UnLoadScript(DWORD dwScriptID)
{
	/*KSortScriptNode ScriptNode;
	ScriptNode.SetScriptID(dwScriptID);
	g_ScriptBinTree.Delete(ScriptNode);
	*/
	if (g_ScriptRateMap.count(dwScriptID) > 0)
	{//如果存在就 删除
		//return g_ScriptRateMap[dwScriptID];
		//printf("-- 脚本存在,清除成功 --\n");
		g_ScriptRateMap.erase(dwScriptID);
	}

}
//重载脚本
int ReLoadScript(const char * szRelativePathScript)
{
	if (!szRelativePathScript || !szRelativePathScript[0])
		return FALSE;
	char script[MAX_PATH];
	strcpy(script, szRelativePathScript);
//	_strlwr(script);
	g_StrLower(script);   //大写转小写
	UnLoadScript(g_CheckFileExist(script));  //先删除 在加载
	LoadScriptToSortListA(script);
	return nCurrentScriptNum;
}

unsigned long  ReLoadAllScript()
{
//	g_ScriptBinTree.ClearList();
	
	ClearAllScript();
	return g_IniScriptEngine("\\script");
}

void  ClearAllScript()
{
	for (KLuaScriptRateMap::iterator it = g_ScriptRateMap.begin(); it != g_ScriptRateMap.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	} 

	g_ScriptRateMap.clear();

}

 
