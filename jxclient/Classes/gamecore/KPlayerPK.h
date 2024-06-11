//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerPK.h
// Date:	2003.07.15
// Code:	边城浪子
// Desc:	PlayerPK Class
//---------------------------------------------------------------------------

#ifndef KPLAYERPK_H
#define KPLAYERPK_H

#define		PK_ANMITY_TIME					200	
#define		PK_ANMITY_CLOSE_TIME			10800

enum
{
	enumPK_ENMITY_STATE_CLOSE = 0,
	enumPK_ENMITY_STATE_TIME,
	enumPK_ENMITY_STATE_PKING,
	enumPK_ENMITY_STATE_NUM,
};

class KPlayerPK
{
	friend class KPlayer;
private:
	int		m_nNormalPKFlag;				// 正常PK开关 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
	int		m_nEnmityPKState;				// 仇杀PK状态 0 关闭 1 正在计时 2 仇杀正在进行，对应 enumPK_ANMITY_STATE
	int		m_nEnmityPKAim;					// 仇杀PK目标，npc id
	int		m_nEnmityPKTime;				// 仇杀PK状态为1正在计时，倒计时计数
	char	m_szEnmityAimName[32];			// 仇杀目标名字
	int		m_nExercisePKFlag;				// 切磋模式PK开关 TRUE 打开，可以砍人  FALSE 关闭，不可以砍人
	int		m_nExercisePKAim;				// 切磋模式PK目标，npc id
	char	m_szExerciseAimName[32];		// 切磋目标名字
	int		m_nPKValue;						// PK值

public:
	void	Init();							// 初始化
	void	ApplySetNormalPKState(BYTE bFlag);// 向服务器申请打开、关闭正常PK状态
	void	SetNormalPKState(int bFlag, BOOL bShowMsg = TRUE);	// 设定正常PK状态
	int		GetNormalPKState();				// 获得正常PK状态
	void	ApplyEnmityPK(char *lpszName);	// 向服务器申请仇杀某人
	void	ApplyEnmityPK(int nNpcID);		// 向服务器申请仇杀某人
	void	SetEnmityPKState(int nState, int nNpcID = 0, char *lpszName = NULL);	// 设定仇杀PK状态
	int		GetEnmityPKState()	{return m_nEnmityPKState;}		// 获得仇杀PK状态
	int		GetEnmityPKAimNpcID()	{return m_nEnmityPKAim;}	// 获得仇杀PK目标npc id
	int		GetEnmityPKTime()	{return m_nEnmityPKTime;}		// 获得仇杀PK准备时间
	char*	GetEnmityPKAimName()	{return m_szEnmityAimName;}	// 获得仇杀PK目标名字
	void	EnmityPKCountDown();			// 仇杀倒计时
	void	SetExercisePKState(int nState, int nNpcID = 0, char *lpszName = NULL);	// 设定切磋状态
	int		GetExercisePKState()	{return m_nExercisePKFlag;}	// 获得切磋状态
	int		GetExercisePKAim()	{return m_nExercisePKAim;}		// 获得切磋目标npc dwid
	char*	GetExercisePKName()	{return m_szExerciseAimName;}	// 获得切磋目标名字
	void	SetPKValue(int nValue);			// 设定PK值
	int		GetPKValue()	{return m_nPKValue;}	// 获得PK值
	void	Active();
};
#endif
