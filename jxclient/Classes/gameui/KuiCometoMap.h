//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiCometoMap__
#define __KuiCometoMap__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "gameui/KuiShowSprite.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
#define _MAPMAX_COUNT 20
class KuiCometoMap:public CCLayer{
public: 
	KuiCometoMap();
	~KuiCometoMap();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiCometoMap);

	static KuiCometoMap * create(CCObject * callbackListener,SEL_CallFuncN callfun);

	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);

	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

	//关闭弹出框
	void closePopLayer(CCObject * pSender);
	void  closeShowPad(CCNode *pNode);
	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void movemapCallBack(CCObject* menuItem);
	//上层对象
	CCObject * m_callbackListener;

	void UpdateData();
	//回调函数
	SEL_CallFuncN m_callfun;

	void touchPageBtnEvent(CCObject *pSender, TouchEventType type);
	void touchAwardBtnEvent(CCObject *pSender, TouchEventType type);
	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);
	void setBtnTouchEnble(int index,bool enble);
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_ItemEx;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	int  m_MaxCount;
	int  m_MapCount;
	CCLabelTTF *m_recommend[_MAPMAX_COUNT];
	CCLabelTTF *m_TimeInfo;;
	UILayer *m_pMainUiLayer;
	Layout  *m_pWidget;

	UIButton* btnPageBox_1;//分页按钮
	UIButton* btnPageBox_2;
	UIButton* btnPageBox_3;
	UIButton* btnPageBox_4;
	UIButton* btnPageBox_5;

	UILayer * _PageLayer_1;//分页面板
	UILayer * _PageLayer_2;
	UILayer * _PageLayer_3; 
	UILayer * _PageLayer_4;
	UILayer * _PageLayer_5;

	UIButton* m_AwardButton_yl;//银两/经验
	UIButton* m_AwardButton_fs;//声望/福缘
	UIButton* m_AwardButton_jb;//金币
	UIButton* m_AwardButton_jf;//积分

	CCSprite * m_timeProcessSprite;
	KuiShowSprite * nShowLayer;

	int _nCurTime; //(单位：分钟)
	int _nMoneyExpTime; //经验和银两 0
	int _nFuYuanTime;   //福缘和声望 1
	int _nJiFenTime;    //积分       3
	int _nJinBiTime;    //金币       2
	
	int _curPage;
};

#endif /* defined(__MyGame__PopLayer__) */