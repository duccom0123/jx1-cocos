//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiChatList__
#define __KuiChatList__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
#include "Kuiplayerfun.h"
//#define FIGHT_SKILL_COUNT_PER_PAGE	25

class KuiChatList:public CCLayer{
public: 
	KuiChatList();
	~KuiChatList();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiChatList);

	static KuiChatList * create(CCObject * callbackListener,SEL_CallFuncN callfun);
	//增加技能图标
	void addDialogData();
	void addmsg(char * sendName,char * contentt,int nKind=-1,int nColor=0,int nPackage=-1);
	int	 NewChannelMessageArrival(DWORD nChannelID, char* szSendName,char* pMsgBuff, unsigned short nMsgLength,DWORD nItemDwidx=0,char *scrSendName=NULL,int nPackage=-1);
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);

	virtual void update(float delta); 
	virtual void draw();
	void _removeDelegate(bool isThis);
	bool    isOpen;
	void _setPlayerFun(Kuiplayerfun * ver){nPlayerFun=ver;};
private:
	Kuiplayerfun *nPlayerFun;
	void selectedItemEvent(CCObject* pSender, ListViewEventType type);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

	void buttonCallBackFunc(CCObject * pSender);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize winSize;
	//对话框背景精灵
	//CCSprite * m_bgSprite;
	CCLayerColor  *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	//CCLayerColor * colorLayer;
	CCAction* red;

	//int  m_nMoney;
	//int  m_nXu;
	//int  m_nNumObjects;
	//POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	UILayer *m_pUiLayer;
	//Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* listView;
	CCSize btnSize;
	CCSize bgSize;
	//int    msgCount;
	
};

#endif /* defined(__MyGame__PopLayer__) */