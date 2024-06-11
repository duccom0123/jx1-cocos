//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiViewRoleState__
#define __KuiViewRoleState__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define ITEM_CELL_SIZE	27   //每个格子的高度和宽度
#define _ITEM_COUNT 15
class KuiViewRoleState:public CCLayer{
public: 
	KuiViewRoleState();
	~KuiViewRoleState();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiViewRoleState);

	static KuiViewRoleState * create(KUiPlayerItem* pDest,CCObject * callbackListener,SEL_CallFuncN callfun);
	bool    isOpen;
private:
	virtual void update(float delta); 
	virtual void draw();

	void  UpdateData(KUiPlayerItem* pDest);
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

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
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	void closedescPadCallback(CCNode *pNode);

	CCLabelTTF* pRoleName;
	CCLabelTTF* pRoleTitle;
	//CCLabelTTF* pRoleTitle_1;
	
	CCLabelBMFont* pPk;

	CCLabelTTF* pZhuanSheng;
	CCLabelTTF* pZhuanShengLevel;

	CCLayerColor * colorLayer;
	CCAction* red;
	CCNode * ItemBoxNode;
	int  m_nMoney;
	int  m_nXu;
	int  m_nNumObjects;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	void UpdateAllEquips();
	void UpdateEquip(KUiObjAtRegion* pEquip, int bAdd);
	void addpicBox(int index,int nItemIndx,bool isAdd);
	bool m_descPadIsOpen;
	int  m_itemIndexBox[_ITEM_COUNT];
	int  m_CurSelItemIndex;
	void UpdateBaseData(KUiPlayerItem* pDest);
	void BreatheUpdateInfo();
	void BreathAttributeUpdate();
	int  __RemainPoint;
	KUiPlayerItem* ncurDest;
	
};

#endif /* defined(__MyGame__PopLayer__) */