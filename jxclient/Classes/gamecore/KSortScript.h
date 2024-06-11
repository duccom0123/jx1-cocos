#ifndef KSORTSCRIPT_H
#define KSORTSCRIPT_H

#include "KCore.h"
#include "engine/KLuaScript.h"
//排序二叉树，保存经过文件名转数字的脚本的id,并实现快速查找对应脚本

//#include "KBinTreeNode.h"
//#include "KBinsTree.h"
//extern char g_szCurScriptDir[MAX_PATH];

#define MAX_SCRIPT_IN_SET 5000   //脚本数量限制

extern KLuaScript g_ScriptSet[MAX_SCRIPT_IN_SET];
/*
class KSortScriptNode
{
	DWORD m_dwScriptIndex;
	DWORD m_dwScriptID;
public:



	DWORD GetScriptIndex(){return m_dwScriptIndex;};
	DWORD GetScriptID(){return m_dwScriptID;};

	void SetScriptIndex(DWORD dwScriptIndex){m_dwScriptIndex = dwScriptIndex;};
	void SetScriptID(DWORD dwScriptID){m_dwScriptID = dwScriptID;};
	const KScript * GetScript()
	{
		if (m_dwScriptIndex >= 0 && m_dwScriptIndex <MAX_SCRIPT_IN_SET) 
			return &g_ScriptSet[m_dwScriptIndex];
		else
			return NULL;
	};
};

typedef	BinSTree<KSortScriptNode> KScriptBinTree;
extern KScriptBinTree g_ScriptBinTree;
*/
extern unsigned long g_IniScriptEngine(char * nScriptDir,BOOL nIsSer=TRUE,BOOL nIsClear=TRUE);
extern const KScript * g_GetScript(DWORD dwScriptId);// 
extern const KScript * g_GetScript(const char * szRelativeScriptFile);
extern int ReLoadScript(const char * szRelativePathScript);
extern unsigned long  ReLoadAllScript();
extern void  ClearAllScript();

#endif
