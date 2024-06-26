//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiTalkDialog1__
#define __KuiTalkDialog1__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度

class KuiTalkDialog1:public CCLayer{
public: 
	KuiTalkDialog1();
	~KuiTalkDialog1();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiTalkDialog1);

	static KuiTalkDialog1 * create(char * pContent, char *OkbtnStr,char *NobtnStr,int nKind,CCObject * callbackListener,SEL_CallFuncN callfun,unsigned int nParam=0,char *strParam=NULL);

	//增加技能图标
	void addDialogData(char * pContent,int nKind);
	void setcoloseButton(char *OkbtnStr,char *NobtnStr,CCObject * callbackListener,SEL_CallFuncN callfun);
	int  getDialogKind(){return __Kind ;};
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void touchEvent(CCObject *pSender, TouchEventType type);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;

	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	//对话框背景精灵
	CCSprite *ParentNode_ItemEx;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	UILayer *m_pUiLayer;
	int  __Kind;
	unsigned int  __nParam;
	char __strParam[32];
	
};

#endif /* defined(__MyGame__PopLayer__) */