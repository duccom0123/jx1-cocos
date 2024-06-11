//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.h
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTASK_H
#define KPLAYERTASK_H

#define		MAX_TASK		4000   //最大任务数
#define		MAX_TEMP_TASK	4000
//-------------新的----------------------------------------------
//#include <algorithm>
#include <map>
struct cTaskInfo
{
	char nTaskIconPath[128];
	int  nTaskidx;
	int  nTaskType;
	char nTaskName[32];
};
class KPlayerTask
{
public:
	//int			nSave[MAX_TASK];					// 用于记录任务是否完成，须保存到数据库
	//int			nClear[MAX_TEMP_TASK];				// 用于记录任务过程中的中间步骤的完成情况，不保存到数据库，玩家下线后次数据清空

	typedef std::map<DWORD, INT> _KTasklate,_KTaskTemplate;
	_KTasklate		m_Tasklate;	
	_KTaskTemplate	m_TaskTemplate;

	cTaskInfo   nTaskInfo[20];                      // 用于客户端任务系统记录
	int         nTaskGenre;
	char        nTaskDesc[512];
public:
	KPlayerTask();									// 构造函数
	void		Release();							// 清空
	void		ClearTempVar();						// 清除临时过程控制变量
	int			GetSaveVal(int nNo);				// 得到任务完成情况
	void		SetSaveVal(int nNo, int bFlag);	    // 设定任务完成情况
	int			GetClearVal(int nNo);				// 得到临时过程控制变量值
	void		SetClearVal(int nNo, int nVal);		// 设定临时过程控制变量值
	//NewByte = SetByte(Value, ByteNo, ByteValue)
	//int         SetTaskByte(int nIntValue,int nByteNumber,int nByteValue);
	//ByteValue = GetByte(Value, ByteNo)
	//int         GetTaskByte(int nIntValue,int nByteNumber);
};

#endif
