//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiRoleState__
#define __KuiRoleState__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define ITEM_CELL_SIZE	74   //每个格子的高度和宽度
#define ITEM_CELL_SIZE_SMALL	55   //每个格子的高度和宽度
#define _ITEM_COUNT 15
class KuiRoleState:public CCLayer{
public: 
	KuiRoleState();
	~KuiRoleState();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiRoleState);

	static KuiRoleState * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);

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

	void buttonEqCallBackFunc(CCObject * pSender);
	void buttonxiuCallBackFunc(CCObject * pSender);
	void addCallBackFunc(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;
	KTabFile nVipInfo;
	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	//CCPoint diff;
	CCSize  m_size;
	CCSize winSize;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	void closedescPadCallback(CCNode *pNode);

	CCLabelTTF* pRoleName;
	CCLabelTTF* pRoleTitle;
	CCLabelTTF* pRoleTitle_1;
	CCLabelTTF* pRoleVip;
	CCLabelTTF* pFuYuan;
	CCLabelTTF* pShenWang;
	CCLabelTTF* proelLevel;
	CCLabelTTF* pPaiMing;

	CCLabelTTF* pLife;
	CCLabelTTF* pMana;
	CCLabelTTF* pTili;

	CCLabelTTF* pSex;
	CCLabelTTF* pExp;
	CCLabelTTF* pLeftAct;
	CCLabelTTF* pRigthAct;

	CCLabelTTF* pMingZhong;
	CCLabelTTF* pShanBi;
	CCLabelTTF* pRunSpeed;
	CCLabelTTF* pActpeed;
	CCLabelTTF* pMicCount;

	CCLabelTTF* pLiLiang;
	CCLabelTTF* pWauGong;
	CCLabelTTF* pNeiGong;
	CCLabelTTF* pShenFa;

	CCLabelTTF* pPuFang;
	CCLabelTTF* pBingFang;
	CCLabelTTF* pLeiFang;
	CCLabelTTF* pHuoFang;
	CCLabelTTF* pDuFang;

	CCLabelTTF* pLucky;
	CCLabelTTF* pPk;

	CCLabelTTF* pZhuanSheng;
	CCLabelTTF* pZhuanShengLevel;

	CCLabelTTF* pTemDescItem;

	CCEditBox * liEditBox;
	CCEditBox * waiEditBox;
	CCEditBox * shenEditBox;
	CCEditBox * neiEditBox;
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
	void UpdateBaseData();
	void BreatheUpdateInfo();
	void BreathAttributeUpdate();
	int  __RemainPoint;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	unsigned int nBakChickTime;
	bool __isCanMove;
	CCSprite * m_ExeProcessSprite;
	int _VipLevel;
	bool nIsOpenTxt;
};

#endif /* defined(__MyGame__PopLayer__) */