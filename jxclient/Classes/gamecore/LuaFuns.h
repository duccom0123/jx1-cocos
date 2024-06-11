#ifndef LUAFUNS_H
#define LUAFUNS_H
#include "KCore.h"
#include "engine/KScriptList.h"
#include "engine/KScriptCache.h"

extern  TLua_Funcs GameScriptFuns[];
extern  int	g_GetGameScriptFunNum();

extern  TLua_Funcs WorldScriptFuns[] ;
extern  int	g_GetWorldScriptFunNum();

//脚本列表实例
//Questions:单机版剧情脚本列表在网络版暂用.
extern  KScriptList		g_StoryScriptList;	//单机版剧情脚本列表
#endif

