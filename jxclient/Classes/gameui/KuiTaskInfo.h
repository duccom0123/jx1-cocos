//
//  KuiTaskInfo.h
//  KuiTaskInfo
//
//  Created by 横刀向天笑 QQ:25557432.
//
//
#ifndef __KuiTaskInfo__
#define __KuiTaskInfo__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Engine/LinkStruct.h"
#include <map>
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

#define MAX_TASK_MESSAGE_LENGTH 128
#define MAX_TASK_MESSAGE_MUM 512

struct KTaskIndexNode// : public KListIndex
{
	char szMsg[MAX_TASK_MESSAGE_LENGTH]; //内容
	int  nMsgLen;		   //长度
	char szUnit[16];	   //单位
	int nTaskId;
	int nType;
	int nClass;
	int nTrace;
	int nIndex;
	int nCount;
	KTaskIndexNode *next;
	KTaskIndexNode *previous;
};

struct	nNewTaskFindPath
{
	int nTsakType;	    //内容
	int	nMsgidx;		//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
	int	nMapIdx;
	int nXpos;
	int nYpos;
};

class KuiTaskInfo:public CCLayer{
public: 
	KuiTaskInfo();
	~KuiTaskInfo();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiTaskInfo);

	static KuiTaskInfo * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void    setCanMove(bool ver){__isCanMove =ver;};
	void    addmsgInfo(char *nMsgInfo,int nIsBtn,int nMsgIndex);
	void    addmsgBtnInfo(KTaskPathInfo* pMsg);
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	void selectedMainListItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedMsgListItemEvent(CCObject* pSender, ListViewEventType type);
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);
	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void btnCallBackFunc(CCObject * pSender);
	

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize  winSize;
	//对话框背景精灵
	//CCSprite * m_bgSprite;
	CCSprite *ParentNode_Task;
	//nNewTaskFindPath nFindPath[MAX_TASK_MESSAGE_MUM];
	typedef std::map<int,nNewTaskFindPath> _FindPath;
	_FindPath nFindPath;
	CCRect getRect(CCNode* pNode,int i);
	void removeitems();
	void AddTaskIndex();
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	UILayer *m_pMainUiLayer;
	//Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* ptaskMsglistView;
	UIListView* pmainListView;
	//UICheckBox* checkBox;
	CCSize btnSize;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool __isCanMove;
	CCSize btnNewSize;
	CCSize btnCurSize;
	CCSize btnMsgSize;
	int  __nSelIndex;
	int  __nCount;
	KIniFile           m_IndexIDIni;
	KLinkStruct<KTaskIndexNode> m_IndexData; //所有索引数据的链表
};

#endif /* defined(__KuiTaskInfo__) */