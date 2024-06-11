#ifndef KTASKFUNCS_H
#define KTASKFUNCS_H
#include "KSubWorldSet.h"
typedef int (* PF_TimerCallBackFun)(void * pOwner, char * szFileName);
#define TASKVALUE_SAVEWAYPOINT_BEGIN 201
#define	TASKVALUE_MAXWAYPOINT_COUNT 3//3

#define TASKVALUE_SAVESTATION_BEGIN  210
#define TASKVALUE_MAXSTATION_COUNT 32

#define TASKVALUE_TIMERTASK_TAST	220
#define TASKVALUE_TIMERTASK_RESTTIME 221
#define TIMERTASK_SETTINGFILE "\\settings\\TimerTask.txt"
#define TASKVALUE_TIME 120
#define TASKVALUE_DIEM 121
#define TASKVALUE_PLAYER 122
#define TASKVALUE_NPC 123
#define TASKVALUE_XEPHANG 124
#define TASKVALUE_LIXIAN 165   //离线
#define TASKVALUE_DEATH  199   //记录死亡触发技能ID

class KPlayer;

class KTaskFun
{
public:
};

class KTimerTaskFun :public KTaskFun
{
public:
	KTimerTaskFun();
	
	KTimerTaskFun(void * pOwner)
	{
		//m_pTimerOwner = pOwner;
	};
};
typedef KTaskFun KTask;

extern KTimerTaskFun g_TimerTask;
#endif
