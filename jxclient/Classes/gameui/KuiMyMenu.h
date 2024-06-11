//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiMyMenu__
#define __KuiMyMenu__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

class KuiMyMenu:public CCMenu{
public: 
	KuiMyMenu();
	~KuiMyMenu();
	//初始化
	//virtual bool init();
	//创建
	//CREATE_FUNC(KuiMyMenu);
	/** creates a CCMenu with it's items */
	static KuiMyMenu* create(CCMenuItem* item, ...);
	static KuiMyMenu* menuWithItem(CCMenuItem* item);
	// 重写registerWithTouchDispatcher
	virtual void registerWithTouchDispatcher();

	CCPoint getMoveEndPos(){return moveEndPoint;};
	void    setSpritePiont(CCPoint aPoint);
	void    setSpriteVisible(bool ver);
	void    setSpriteOpacity(int ver);

	bool    getIsRun(){return isRun;};
	void    setIsRun(bool nisRun){isRun=nisRun;};
	void    setRoleIndex(int nIdx){nRoleIndex=nIdx;};
	void    setRolePoint(CCPoint nPoint){nRolePoint=nPoint;};
	void    setSpriteOrder(int ver);
private:
	//启动摇杆
	void Active();
	//解除摇杆
	void Inactive();
	void updatePos(float dt);  //CCTime

	CCPoint getDirection();
	float   getAngleSigned();
	float   getRad(CCPoint p1,CCPoint p2);
	float   getVelocity();
	//重写触摸注册函数，重新给定触摸级别
	//void registerWithTouchDispatcher();
	KuiMyMenu * initWithCenter();
	//触摸函数ccTouchBegan，返回true
	/*bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void ccTouchEnded(CCTouch* touch, CCEvent* pevent);
	void ccTouchCancelled(CCTouch *touch, CCEvent* pevent);
	void ccTouchMoved(CCTouch* touch, CCEvent* pevent);*/
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

#endif /* defined(__MyGame__PopLayer__) */
