//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuigetStringInUI__
#define __KuigetStringInUI__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度

class KuigetStringInUI:public CCLayer, public cocos2d::extension::CCEditBoxDelegate
{
public: 
	KuigetStringInUI();
	~KuigetStringInUI();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuigetStringInUI);
	static KuigetStringInUI * create(char* pszTitle,int nMinLen, int nMaxLen,const char* pszCallback,const char* pszError,int bNum,CCObject * callbackListener,SEL_CallFuncN callfun,int outKind=0);

	virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
	virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
	virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
	virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);

	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
private:
	//增加技能图标
	void addDialogData(char* pszTitle);
	//设置确定取消按钮,参数：调用层对象，调用层回调函数
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	/*void editBoxEditingDidBegin(CCEditBox *editBox); //开始时的函数
	void editBoxEditingDidEnd(CCEditBox *editBox); //结束时的函数
	void editBoxReturn(CCEditBox *editBox); //输入完后点击回车时的处理
	void editBoxTextChanged(CCEditBox *editBox, const std::string &text); //文字更改函数
	*/
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void getstringgCallBackFunc(CCObject * pSender);
	void _getstringgCallBackFunc(CCObject * pSender);

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
	CCLabelTTF* ptitleLabel;
	//CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	CCAction* red;

	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	CCEditBox *pMoneyEditBox;
	//char strTitle[64];
	char strCallBackfun[64];
	char strErrorfun[16];
	int  minMsgLen;
	int  maxMsgLen;
	int  inPutKind;
	int  outPutKind;
};

#endif /* defined(__MyGame__PopLayer__) */