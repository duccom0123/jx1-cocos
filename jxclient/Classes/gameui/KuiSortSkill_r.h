//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiSortSkill_r__
#define __KuiSortSkill_r__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
#define	SKILLTREE_MAX_SKILL_COUNT		65
#define	SKILLTREE_SHORTCUT_SKILL_COUNT	9

class KuiSortSkill_r:public CCLayer{
public: 
	KuiSortSkill_r();
	~KuiSortSkill_r();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiSortSkill_r);

	static KuiSortSkill_r * create(int nKind,CCObject * callbackListener,SEL_CallFuncN callfun);

	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//增加技能图标
	void addDialogData(int nKind);
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

	void UpdateData();
	//回调函数
	SEL_CallFuncN m_callfun;

	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);

	void skillCallback(CCObject* pSender);
	void closeSkilldescPadCallback(CCNode *pNode);
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCLayerColor * ParentNode_ItemEx;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;

	int				m_nNumSkills;
	KUiSkillData	m_Skills[SKILLTREE_MAX_SKILL_COUNT];
	//KUiSkillData::nLevel在此的含义表示位此技能显示在第几列
	KUiSkillData	ms_ShortcutSkills[SKILLTREE_SHORTCUT_SKILL_COUNT];
	//KUiSkillData::nLevel在此的含义0表示左手技能，1表示右手技能
	int   _sortCount_r;
	
};

#endif /* defined(__MyGame__PopLayer__) */