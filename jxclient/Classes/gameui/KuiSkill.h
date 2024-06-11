//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __Kuiskill__
#define __Kuiskill__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define LIVE_SKILL_COUNT			10
#define	FIGHT_SKILL_COUNT			50
#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define FIGHT_SKILL_PAGE_1	0
#define FIGHT_SKILL_PAGE_2	1
#define FIGHT_SKILL_PAGE_3	2
#define FIGHT_SKILL_PAGE_4	3
#define FIGHT_SKILL_PAGE_5	4
#define	FIGHT_SKILL_SUB_PAGE_COUNT	FIGHT_SKILL_COUNT / FIGHT_SKILL_COUNT_PER_PAGE

class Kuiskill:public CCLayer{
public: 
	Kuiskill();
	~Kuiskill();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(Kuiskill);

	static Kuiskill * create(int m_Kind,CCObject * callbackListener,SEL_CallFuncN callfun);
	void touchEvent(CCObject* pSender, TouchEventType type);
    void scrollEvent(CCObject* pSender, ScrollviewEventType type);
	void pageViewEvent(CCObject* pSender, PageViewEventType type, CCTouch *pTouch);
	void UpdateSkill(KUiSkillData* pSkill, int nIndex);
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);

	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void oktouchEvent(CCObject *pSender, TouchEventType type, CCTouch *pTouch);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	void   addskillpoint(CCObject* pSender);
	void   toprePage(CCObject* pSender);
	void   tonextPage(CCObject* pSender);

	void  closeDescCallback(CCNode *pNode);
	void  closeSkilldescPadCallback(CCNode *pNode);
	
	int   getfreeSkillNode(int skillidx);
	CCTexture2D * GetiniData(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag=0,int bSinglePlaneCoord=0);
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	UILayer *m_pUiLayer;
    Layout *m_pWidget; 
	UIPageView* pageView;
	CCNode * ItemNode;
	UIPageView* pageViewTemp;
	CCSprite *ParentNode_Fight;
	CCNode *_pageLayout1;
	CCNode *_pageLayout2;
	CCNode *_pageLayout3;
	CCNode *_pageLayout4;
	CCNode *_pageLayout5;
	KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	int nRemainSkillPoint;
	CCLabelTTF* pSkillPointLabel;
	CCLabelTTF* pSkillInfoLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	int  nSkillCount;
	int  nCurSelindex;
};

#endif /* defined(__MyGame__PopLayer__) */
