//
//  KuiMylistView.h
//  KuiMylistView
//
//  Created by 横刀向天笑 QQ:25557432
//
//
#ifndef __KuiMylistView__
#define __KuiMylistView__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

using namespace ui;
class KuiMylistView:public ListView{
public: 
	KuiMylistView();
	~KuiMylistView();
	//初始化
	static KuiMylistView* create();
	
	CCPoint getMoveEndPos(){return moveEndPoint;};
	void    setSpritePiont(CCPoint aPoint);
	void    setSpriteVisible(bool ver);
	void    setSpriteOpacity(int ver);

	bool    getIsRun(){return isRun;};
	void    setIsRun(bool nisRun){isRun=nisRun;};
	void    setRoleIndex(int nIdx){nRoleIndex=nIdx;};
	void    setRolePoint(CCPoint nPoint){nRolePoint=nPoint;};
private:
	//重写触摸注册函数，重新给定触摸级别
	virtual void registerWithTouchDispatcher();
	float getAngleSigned();
	float getRad(CCPoint p1,CCPoint p2);
	KuiMylistView * initWithCenter();
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	void ccTouchEnded(CCTouch* touch, CCEvent* event);
	void ccTouchCancelled(CCTouch *touch, CCEvent* event);
	void ccTouchMoved(CCTouch* touch, CCEvent* event);

	CCPoint nRolePoint;
	int     nRoleIndex;

	CCPoint moveEndPoint;
	CCPoint centerPoint; //摇杆中心
	CCPoint currentPoint;//摇杆当前位置
	bool    isRun;
	bool    active;//是否激活摇杆
	float   radius;//摇杆半径
	CCSprite *jsSprite;
	CCSprite *bgSprite;
	bool    isFollowRole;//是否跟随用户点击

	int     _BackAttackRadius;
	int     _CurSkillId;
	float   _Beilv;
	int     _useSkillModel;
};

#endif /* defined(__KuiMylistView__) */
