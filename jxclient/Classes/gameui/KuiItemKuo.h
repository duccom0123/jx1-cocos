//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiItemKuo__
#define __KuiItemKuo__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度

class KuiItemKuo:public CCLayer{
public: 
	KuiItemKuo();
	~KuiItemKuo();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiItemKuo);

	static KuiItemKuo * create(CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void    UpdateData(int nExboxIndex);
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd,int nExBoxIndex);//物品变化
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
	void buttonCallBackFunc_1(CCObject * pSender);
	void buttonCallBackFunc_2(CCObject * pSender);
	void buttonCallBackFunc_3(CCObject * pSender);
	void qiCallBackFunc(CCObject * pSender);
	void quCallBackFunc(CCObject * pSender);
	void zenliCallBackFunc(CCObject * pSender);
	void closeItemPadCallback(CCNode *pNode);
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
	CCSprite *ParentNode_ItemEx;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;

	typedef std::map<std::string,KUiItemdObject> _ItemData;                 //SkillId,Echance
	_ItemData m_ItemData;

	int  m_nIstrue;
	int  m_nNumObjects;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	std::string nstrCurSelItemKey;
	bool m_descPadIsOpen;
	int AddObject(KUiDraggedObject* pObject, int nCount,int ExBoxIndex);
	CCSprite *btnBackPic;
	unsigned int nBakChickTime;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	int nCurExPadIndex;
};

#endif /* defined(__MyGame__PopLayer__) */