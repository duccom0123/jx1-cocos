//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiAccreg__
#define __KuiAccreg__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
#define TIME_OUT     5           //定义超时时限
class KuiAccreg:public CCLayer{
public: 
	KuiAccreg();
	~KuiAccreg();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiAccreg);

	static KuiAccreg * create(CCObject * callbackListener,SEL_CallFuncN callfun);
	virtual void update(float delta); 
	virtual void draw();

	virtual void editBoxEditingDidBegin(CCEditBox *editBox); //开始时的函数
	virtual void editBoxEditingDidEnd(CCEditBox *editBox);   //结束时的函数
	virtual void editBoxReturn(CCEditBox *editBox);          //输入完后点击回车时的处理
	virtual void editBoxTextChanged(CCEditBox *editBox, const std::string &text); //文字更改函数

	bool    isOpen;
private:
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

	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);
	void mianLoginCallback(CCObject* pSender);
	void mianCancelCallback(CCObject* pSender);
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

	void colseSockets();
	int  m_AnswerCount;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	CCEditBox * AccNode;
	CCEditBox * PasNode;
	CCEditBox * ePasNode;
	SOCKET s;
	unsigned int nport;
	std::string  naccIp;
	std::string m_WritablePath;
	
};

#endif /* defined(__MyGame__PopLayer__) */