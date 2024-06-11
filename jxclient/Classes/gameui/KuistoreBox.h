//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuistoreBox__
#define __KuistoreBox__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include <map>
USING_NS_CC;
USING_NS_CC_EXT;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
//#define MAX_ITEM_COUNT  60

class KuistoreBox:public CCLayer{
public: 
	KuistoreBox();
	~KuistoreBox();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuistoreBox);

	static KuistoreBox * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化
	void	UpdateExItem(KUiObjAtRegion* pItem, int bAdd,int exBoxIndex);
	bool    isOpen;
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
	void getmoneyCallBackFunc(CCObject * pSender);
	void buttonCallBackFunc(CCObject * pSender);
	void openboxCallBackFunc(CCObject * pSender);

	void closeItemKuoPadCallback(CCNode *pNode);

	void zenliCallBackFunc(CCObject * pSender);
	void diuCallBackFunc(CCObject * pSender);
	void chuCallBackFunc(CCObject * pSender);
	void ziCallBackFunc(CCObject * pSender);
	void kuo1CallBackFunc(CCObject * pSender);
	void kuo2CallBackFunc(CCObject * pSender);
	void kuo3CallBackFunc(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize winSize;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_Item;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	//CCAction* red;
	//CCMenu * savemenu;

	bool m_descPadIsOpen;
	bool m_getmoneyPadIsOpen;
	int  m_nMoney;
	//int  m_nXu;
	int  m_nNumObjects;
	POINT m_StartPos;//格子开始的偏移 相对偏移 左上角(0,0)
	//KuistoreBoxdObject m_ItemData[MAX_ITEM_COUNT];//KuistoreBoxObject
	int AddObject(KUiDraggedObject* pObject, int nCount);
	typedef std::map<std::string,KUiItemdObject> _ItemData;                 //SkillId,Echance
	_ItemData m_ItemData;
	CCSprite *btnBackPic;
	//int   m_CurSelItemIndex;
	void  closeItemPadCallback(CCNode *pNode);
	void  closegetmonyPadCallback(CCNode *pNode);
	void  UpdateData();
	std::string  nstrCurSelItemKey;
	float m_MovePosX;
	unsigned int nBakChickTime;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool _m_itemExPadIsOpen;
	CCLayer * sItemExLayer;
	
};

#endif /* defined(__MyGame__PopLayer__) */