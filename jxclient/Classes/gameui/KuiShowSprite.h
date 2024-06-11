//
//  KuiShowSprite.h
//  KuiShowSprite
//
//  Created by 横刀向天笑 QQ:25557432
//
//
#ifndef __KuiShowSprite__
#define __KuiShowSprite__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "gamescene/KScenePlaceC.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
//#include "Kuiplayerfun.h"
//#define FIGHT_SKILL_COUNT_PER_PAGE	25

class KuiShowSprite:public CCLayer{
public: 
	KuiShowSprite();
	~KuiShowSprite();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiShowSprite);

	static KuiShowSprite * create(char * nFilePath,int nXpos,int nYpos,CCObject * callbackListener,SEL_CallFuncN callfun,bool nTouchesModel=true,int nRenderFlag=0);
	//增加技能图标
	void addDialogData();
	void mianDialogSet(char * nFilePath,int nXpos,int nYpos,bool nTouchesModel=true,int nRenderFlag=0);
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	virtual void update(float delta); 
	virtual void draw();
	bool    isOpen;
	CCSize getShowSize();
	void   removeShowSprite();
	void   setShowPosition(int nPosX,int nPosY);
	void   setRegisterWithTouchDispatcher(int nVer);
	//void _setPlayerFun(Kuiplayerfun * ver){nPlayerFun=ver;};
private:
	//重写触摸注册函数，重新给定触摸级别
	//void registerWithTouchDispatcher();
	//触摸函数ccTouchBegan，返回true
	CCTexture2D * ccgetTxtData_s(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	
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
	CCSize  winSize;
	//对话框背景精灵
	CCLayerColor *ParentNode_Team;
	CCRect getRect(CCNode* pNode,int i);
	CCLabelTTF* ptestLabel;
	CCAction* red;
	
	//UILayer *m_pUiLayer;
	//CCEditBox *pstrEditBox_d;
	CCSize bgSize;
	CCSize nTextureSize;
	bool m_bScrolling;
    float nRoleDisX;
    float nRoleDisY;
	CCPoint m_lastPoint;
	CCSprite *sFarmsSprite;
	CCTexture2D *sFarmsTexture; 
	unsigned int __loopTime;
	CCLabelBMFont *pPointLabel;
	int    _nFarms;
	int    _curFarms;
	int    _nModel;
	char   nSprNamePath[128];
	int		_nInterval;		        //帧间隔
	unsigned int _nFlipTime;		//当前帧切换出来的时间
	int   _nXpos;
	int   _nYpos;
	int   _pHwidth;
	int   _pHheight;
	int   _bRenderFlag;
	//KUiImageRef m_Image;
	
};

#endif /* defined(__MyGame__PopLayer__) */