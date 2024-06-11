//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiTeam__
#define __KuiTeam__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度

class KuiTeam:public CCLayer{
public: 
	KuiTeam();
	~KuiTeam();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiTeam);

	static KuiTeam * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void	UpdateData(KUiPlayerTeam* pInfo);
	void    setCanMove(bool ver){__isCanMove =ver;};
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	void selectedItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedPlayersListItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void btnCallBackFunc(CCObject * pSender);
	
	void Clear();
	void OnRefresh();					//刷新
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
	CCSprite *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXLabel;
	CCLayerColor * colorLayer;
	CCAction* red;

	//int  m_nMoney;
	//int  m_nXu;
	//int  m_nNumObjects;
	//POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	UILayer *m_pUiLayer;
	//Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* pNearbyPlayerslistView;
	UIListView* pPlayersListView;
	UICheckBox* checkBox;
	CCSize btnSize;
	KUiPlayerTeam	m_Info;
	KUiPlayerItem*	m_pPlayersList;
	KUiPlayerItem*	m_pNearbyPlayersList;
	int   __nSelIndex;
	int   __playrSelIndex;
	int   __playerListCount;
	int   __NearbyListCount;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool __isCanMove;
};

#endif /* defined(__MyGame__PopLayer__) */