//
//  PopupLayer.h
//  PopupDemo
//  物品说明层
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiSkilldesc__
#define __KuiSkilldesc__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

class KuiSkilldesc:public CCLayer{
public: 
	KuiSkilldesc();
	~KuiSkilldesc();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiSkilldesc);

	static KuiSkilldesc * create(int nskillId,int nLevel,KUiItemdObject nKillData,CCObject * callbackListener,SEL_CallFuncN callfun,int nKind=0);

	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//增加技能图标
	void addDialogData();
	//设置确定取消按钮,参数：调用层对象，调用层回调函数
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
	CCLayerColor  *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	CCLabelTTF* ptitleLabel;//标题
	CCLabelTTF* potherLabel;//其他不要求颜色设置的
	CCNode* pdescLabel;     //需要配置颜色的
	CCLabelTTF* pbaseLabel; //基本属性
	CCLabelTTF* pScriptLabel; //脚本说名
	CCLabelTTF* pScriptLabel_n;
	CCNode* prqLabel;       //需求属性
	CCNode* pmagicLabel;//魔法属性
	CCLayerColor * colorLayer;
	CCAction* red;

	//int  m_ItemIndex;
	KUiItemdObject skillData;
	int _nKind;
	//int  m_nMoney;
	//int  m_nXu;
	//int  m_nNumObjects;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	
};

#endif /* defined(__MyGame__PopLayer__) */