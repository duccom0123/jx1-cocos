//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiSuperShop__
#define __KuiSuperShop__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "gamecore/GameDataDef.h"
USING_NS_CC;
USING_NS_CC_EXT;

#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
#define PAGE_MAX_COUNT_ITEM	60
class KuiSuperShop:public CCLayer{
public: 
	KuiSuperShop();
	~KuiSuperShop();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiSuperShop);

	static KuiSuperShop * create(BuySellInfo* pBSinfo,CCObject * callbackListener,SEL_CallFuncN callfun);

	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	void UpdateData();
	//增加技能图标
	void addDialoaData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buyCallBackFunc(CCObject * pSender);
	//上一页
	void backCallBackFunc(CCObject * pSender);
	//下一页
	void nextCallBackFunc(CCObject * pSender);
	void closeCallBackFunc(CCObject * pSender);
	void closedescPadCallback(CCNode *pNode);
	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_ItemEx;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	void   SetPage(int nIndex);
    int    AddObject(KUiDraggedObject* pObject, int nCount);
	void   Clear();

	void  UpdateShop(BuySellInfo* pBSinfo);
	CCLabelTTF* pMoneyLabel;
	CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	CCNode * ItemNode;//一个层
	//int  m_nMoney;
	//int  m_nXu;
	//int  m_nNumObjects;
	int m_nObjCount;
	int m_CurShopIdx;
	int m_nPageCount;
	int	m_nCurrentPage;  //当前页
	int	m_nCurrentCount;
	int m_ItemIndex[PAGE_MAX_COUNT_ITEM];
	KUiObjAtContRegion* m_pObjsList;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	bool m_descPadIsOpen;
	int m_selItemIndex;
	CCEditBox *pstrEditBox_count;

	BuySellInfo	 m_pBSinfo;
};

#endif /* defined(__MyGame__PopLayer__) */