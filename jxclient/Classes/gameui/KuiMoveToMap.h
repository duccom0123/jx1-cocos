//  山洞地图传送
//  KuiMoveToMap.h
//  KuiMoveToMap
//
#ifndef __KuiMoveToMap__
#define __KuiMoveToMap__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
#include "Kuiplayerfun.h"
//#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define MAX_COUNT_MAPS	120
struct mapListData
{
	UIButton* m_MoveMapBtn;
	int       m_SubwordId;
	int       m_PosX;
	int       m_PosY;
};

class KuiMoveToMap:public CCLayer{
public: 
	KuiMoveToMap();
	~KuiMoveToMap();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiMoveToMap);
	static KuiMoveToMap * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	virtual void update(float delta); 
	virtual void draw();
	bool    isOpen;
	//void _setPlayerFun(Kuiplayerfun * ver){nPlayerFun=ver;};
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);

	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch *pTouch, CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize  winSize;

	CCLayerColor *ParentNode_Team;
	CCRect getRect(CCNode* pNode,int i);

	CCAction* red;
	UILayer *m_pUiLayer;
	CCSize bgSize;
	CCSize nTextureSize;
	CCSprite *nMapPicSpr;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	CCClippingNode *clipper;
	CCSprite *content_Map;
	//int    msgCount;
	int m_PosCount;
	mapListData m_MoveMapBtn[MAX_COUNT_MAPS];
};

#endif /* defined(__MyGame__PopLayer__) */