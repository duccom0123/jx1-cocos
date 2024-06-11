//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
// 
// File:	KLuaScript.h
// Date:	2001-9-11 11:11:09
// Code:	Romandou
// Desc:	
//---------------------------------------------------------------------------
#ifndef KLuaScript_H
#define KLuaScript_H
//---------------------------------------------------------------------------
#include "KScript.h"
#include "lualib/LuaLib.h"
#include <string.h>

#define MAINFUNCTIONNAME "main"
#define LUA_CREATE_ERROR 1			 //创建错误
#define LUA_SCRIPT_LEN_ERROR 2		 //大小出错
#define LUA_SCRIPT_COMPILE_ERROR 3	 //编译出错
#define LUA_SCRIPT_EXECUTE_ERROR 4	 //执行出错
#define LUA_SCRIPT_NOT_NUMBER_ERROR 10	//不是数字出错
#define LUA_SCRIPT_NOT_STRING_ERROR 11	//不是字符串
#define LUA_SCRIPT_NOT_TABLE_ERROR 12	//不是表格
#define LUA_SCRIPT_STATES_IS_NULL 20	//脚本为空

typedef struct  {
	const char *name;
	lua_CFunction func;
	
}TLua_Funcs;

//---------------------------------------------------------------------------
class  KLuaScript : public KScript
{
public:
	/*
	KLuaScript();
	virtual ~KLuaScript();
	KLuaScript(int StackSize);*/
	KLuaScript(void);
	KLuaScript(int StackSize);
	virtual ~KLuaScript();
	
	int		Activate(){Execute(); return 1;};

	
	virtual BOOL Init();
	virtual void Exit();
	virtual BOOL Load(char* FileName);
	virtual	BOOL Compile(char* FileName);
	virtual	BOOL Execute();
	virtual	BOOL CallFunction(LPSTR cFuncName, int nResults, LPSTR cFormat, ...);
	virtual BOOL RegisterFunction(LPSTR FuncName, void* Func);
	virtual void SetScriptName(LPSTR scpname){strcpy(m_szScriptName, scpname);};

	int	 GetUserTag() { return m_UserTag; };
	BOOL RegisterFunctions( TLua_Funcs Funcs[], int n = 0 );
	void SetInIt(int nStackSize,TLua_Funcs Funcs[], int n = 0);
	BOOL LoadBuffer(PBYTE pBuffer, DWORD dwLen );
	int  SafeCallBegin();//记录当前堆栈栈顶位置，并保存到pIndex指向的数据中
	void SafeCallEnd (int nIndex);   //恢复至调用之前栈顶位置。
	BOOL GetValuesFromStack(char * cFormat , ...);			
	
	BOOL  Stop();
	BOOL  Resume();
	DWORD CreateTable();
	DWORD ModifyTable(LPSTR szTableName); 
	void  SetGlobalName(LPSTR szName);
	int   GetStackSpace();
	inline void SetTableMember(int nIndex, int Id, const char * szString){Lua_SetTable_StringFromId(m_LuaState,  nIndex, Id, szString);};
	inline void SetTableMember(int nIndex, int Id, double nNumber){Lua_SetTable_DoubleFromId(m_LuaState,  nIndex, Id, nNumber);};
	inline void SetTableMember(int nIndex, int Id, int nNumber){Lua_SetTable_IntFromId(m_LuaState,  nIndex, Id, nNumber);};
	inline void SetTableMember(int nIndex, const char * szMemberName, Lua_CFunction CFun){Lua_SetTable_CFunFromName(m_LuaState,  nIndex, szMemberName, CFun);};
	inline void SetTableMember(int nIndex, int nId, Lua_CFunction CFun){Lua_SetTable_CFunFromId(m_LuaState,  nIndex, nId, CFun);};
	inline void SetTableMember(int nIndex, const char * szMemberName, char * szString){Lua_SetTable_StringFromName(m_LuaState,  nIndex, szMemberName, szString);};
	inline void SetTableMember(int nIndex, const char * szMemberName, int Number){Lua_SetTable_IntFromName(m_LuaState,  nIndex, szMemberName, Number);};
	inline void SetTableMember(int nIndex, const char * szMemberName, double Number){Lua_SetTable_DoubleFromName(m_LuaState,  nIndex, szMemberName, Number);};
	void ScriptError(int Error);
	void ScriptError(int , int);
	BOOL ExecuteCode();

//private:
	void RegisterStandardFunctions();
	BOOL m_IsRuning;//是否该脚本有效
	char m_szScriptName[256]; //脚本路径名
	int	 m_UserTag;
	Lua_State * m_LuaState;
private:
	BOOL CallFunctionS(LPSTR cFuncName, int nResults, LPSTR cFormat, va_list vlist);

};
//---------------------------------------------------------------------------
#endif //KLuaScript_H
