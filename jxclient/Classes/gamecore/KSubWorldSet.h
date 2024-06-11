#ifndef	KSubWorldSetH
#define	KSubWorldSetH

#include "engine/KTimer.h"
#include "KMapMusic.h"
/*
#ifndef _SERVER
#include "KMapMusic.h"
#else

#include "KSubWorld.h"

#endif
*/
#define  MAX_TASK_COUNT 50

class  KSubWorldSet
{
public:
	DWORD  m_nLoopRate;		// 循环帧数
	//KMapMusic m_cMusic;
private:
	int		m_nGameVersion;
	DWORD	m_dwPing;
public:
	KSubWorldSet();
	//BOOL	LoadSerMaps(LPSTR szFileName);
	int		SearchWorld(DWORD dwID);
	int     SearchWorldRows(int dwID);
	void	MainLoop();
	void	MessageLoop();
	//int  	TaskActivate(); //任务自动触发系统
	int 	TaskMainLoop();
	BOOL	SendMessage(int nSubWorldID, DWORD	dwMsgType, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	DWORD	GetGameTime(){return m_nLoopRate;};
	int		GetGameVersion() { return m_nGameVersion; }
	void	Close();
	BOOL	SubExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL);
	BOOL	SubExecuteScript(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
	BOOL	SubExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0);
	BOOL	SubExecuteScriptB(char* dwScriptIdA,  char * szFunName, int nParam=0,char * szCanshu=NULL);
// Client上才有的几个方法
	void	SetPing(DWORD dwTimer) { m_dwPing = dwTimer; }
	DWORD	GetPing() { return m_dwPing; }
	unsigned long getCilentRunTime();
	//void	Paint();
};
extern  KSubWorldSet g_SubWorldSet;
#endif
