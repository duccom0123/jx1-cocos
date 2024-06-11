//
//  KuiTongCreateSheet.h
//  KuiTongCreateSheet
//
//  Created by 横刀向天笑 QQ:25557432.
//
//
#ifndef __KuiTongCreateSheet__
#define __KuiTongCreateSheet__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
class KuiTongCreateSheet:public CCLayer{
public: 
	KuiTongCreateSheet();
	~KuiTongCreateSheet();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiTongCreateSheet);

	static KuiTongCreateSheet * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void    setCanMove(bool ver){__isCanMove =ver;};
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
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
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	CCLabelTTF* pTongNameLabel;
	CCLabelTTF* pCampTitleLabel;
	CCLabelTTF* pCampInfoLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	UILayer *m_pMainUiLayer;
	CCEditBox *pNameEditBox;
	UIButton* btnBox_zp;
	UIButton* btnBox_zl;
	UIButton* btnBox_xp;
	CCSize btnSize;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool __isCanMove;
    int   __nSelCamp;
};

#endif /* defined(__KuiTongCreateSheet__) */