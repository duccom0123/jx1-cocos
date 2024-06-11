//#include "gamecore/KCore.h"
#include "KSkillRocker.h"
#include "gamecore/GameDataDef.h"
#include "gameui/Uiglobaldata.h"
using namespace cocos2d;

void KSkillRocker::updatePos(float dt){
	jsSprite->setPosition(ccpAdd(jsSprite->getPosition(),ccpMult(ccpSub(currentPoint, jsSprite->getPosition()),0.5)));
}
//启动摇杆
void KSkillRocker::Active()
{
	if (!active)
	{
		active=true;
		this->schedule(schedule_selector(KSkillRocker::updatePos));                       //添加刷新函数
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,false);
	}else {
	}
}
//解除摇杆
void   KSkillRocker::Inactive()
{
	if (active) {
		active=false;
		this->unschedule(schedule_selector(KSkillRocker::updatePos));                 //删除刷新
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);//删除委托
	}else {
	}
}

//第二部：计算出两个向量之间的夹角
//因为求出的角度是以π为单位，所以这里要转换成以度为单位
float KSkillRocker::getAngleSigned()
{//ccpSub(centerPoint, currentPoint),CCPointMake(0, 1))
	//return CC_RADIANS_TO_DEGREES(ccpAngleSigned(ccpSub(centerPoint, currentPoint),CCPointMake(0, 1)));
	/*  //#define PI 3.141592654    float angle = 180.f / PI * rad; ==等于角度
	float dx = currentPoint.x - centerPoint.x;  
	float dy = currentPoint.y - centerPoint.y;  
	float t = dx/dy;  

	float  at = atan(t);  
	return at/3.1415926*180; 
	
	// 获取角度
	Point p1 = this->getPosition(); //摇杆的中心的位置
	Point p2 = touch->getLocation();//触摸点的位置
	
	*/  
    float rad = getRad(centerPoint,currentPoint);

	return 180.f/3.141592654*rad;

}

float KSkillRocker::getRad(CCPoint p1,CCPoint p2)
{
	float xx = p2.x - p1.x;
	float yy = p2.y - p1.y;
	// 斜边
	float xie = sqrt(pow(xx, 2) + pow(yy, 2));
	// yy >= 0 弧度在于 0 到 π 之间。(0~180°)
	// yy < 0 弧度在于 π 到 2π 之间。(180°~360°)
	float rad = yy >= 0 ? (acos(xx / xie)) : (3.141592654 * 2 - acos(xx / xie));
	return rad;
}
//摇杆方位
CCPoint KSkillRocker::getDirection()
{
	return ccpNormalize(ccpSub(centerPoint, currentPoint));
}
//摇杆和中心点的距离
float KSkillRocker::getVelocity()
{
	return ccpDistance(centerPoint, currentPoint);
}
KSkillRocker* KSkillRocker:: KSkillRockerWithCenter(CCPoint aPoint ,float aRadius ,CCSprite* aJsSprite,CCSprite* aJsBg,bool _isFollowRole){
	KSkillRocker *jstick=KSkillRocker::create();
	jstick->initWithCenter(aPoint,aRadius,aJsSprite,aJsBg,_isFollowRole);
	return jstick;
}

bool KSkillRocker::ccTouchBegan(CCTouch* touch, CCEvent* event)
{//开始移动
	if (!active)
		return false;
	//if (pCoreShell) pCoreShell->setPadCanMove(PAD_KIND_ITEM,false);

	if (pCoreShell && pCoreShell->getTradeState()) return false;

	this->setVisible(true);

	setSpriteOpacity(255);
	CCPoint touchPoint = touch->getLocationInView();     //获取触摸的坐标
	touchPoint = CCDirector:: sharedDirector()->convertToGL(touchPoint); //转换屏幕坐标
	if(!isFollowRole){
		if (ccpDistance(touchPoint, centerPoint) > radius){
			return false;
		}
	}
	currentPoint = touchPoint;
	if(isFollowRole){
		centerPoint=currentPoint;
		jsSprite->setPosition(currentPoint);
		this->getChildByTag(1000)->setPosition(currentPoint);
	}
	//CCMessageBox("开始移动中","开始移动中");
	return true;
}
void  KSkillRocker::ccTouchMoved(CCTouch* touch, CCEvent* event)
{//移动中
	if (pCoreShell && pCoreShell->getTradeState()) return;
	//if (pCoreShell) pCoreShell->setPadCanMove(PAD_KIND_ITEM,false);
	int nBeilv = 3;
	CCPoint touchPoint = touch->getLocationInView();                     //屏幕坐标系 +x 向右 +ｙ向下
	touchPoint = CCDirector:: sharedDirector()->convertToGL(touchPoint); //转换成左下角gl起点坐标(0,0)
	float nDistance = ccpDistance(touchPoint, centerPoint);
	if (nDistance > radius)
	{//超出范围
		currentPoint =ccpAdd(centerPoint,ccpMult(ccpNormalize(ccpSub(touchPoint, centerPoint)), radius));
	}else {
		currentPoint = touchPoint;
	}
	//char nPiontinfo[128]={0};
    /*sprintf(nPiontinfo,"CurrentPointX:%f,CurrentPointY:%f\nCenterPointX:%f,CenterPointY:%f",
		    currentPoint.x,currentPoint.y,centerPoint.x,centerPoint.y);
	CCMessageBox(nPiontinfo,"摇杆移动中");*/
	float nVer = getAngleSigned();  //中心点与触摸点的距离
    float nDestPontX,nDestPontY;
	if (nVer==0)
	{//正右
        nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*nBeilv;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-nRolePoint.y;
	}
	else if(nVer>0 && nVer<90)
	{//右上
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*nBeilv;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-(nRolePoint.y+(currentPoint.y-centerPoint.y)*nBeilv);
	}
	else if (nVer==90)
	{//正上
		nDestPontX = nRolePoint.x;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-(nRolePoint.y+(currentPoint.y-centerPoint.y)*nBeilv);
	}
	else if (nVer>90 && nVer<180)
	{//左上
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*nBeilv;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-(nRolePoint.y+(currentPoint.y-centerPoint.y)*nBeilv);
	}
	else if (nVer==180)
	{//正左
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*nBeilv;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-nRolePoint.y;
	}
	else if (nVer>180 && nVer<270)
	{//左下
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*nBeilv;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-(nRolePoint.y-(centerPoint.y-currentPoint.y)*nBeilv);
	}
	else if (nVer==270)
	{//正下
		nDestPontX = nRolePoint.x;//目标gl坐标x y不变
		nDestPontY = visibleSize.height-(nRolePoint.y-(centerPoint.y-currentPoint.y)*nBeilv);
	}
	else if (nVer>270 && nVer<360)
	{//右下
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*nBeilv;//
		nDestPontY = visibleSize.height-(nRolePoint.y-(centerPoint.y-currentPoint.y)*nBeilv);
	}
	else if (nVer==360)
	{//正右
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*nBeilv;//
		nDestPontY = visibleSize.height-nRolePoint.y;
	}

	if (pCoreShell == NULL)
		return ;
/*
		pCoreShell->LockSomeoneAction(0);

		pCoreShell->LockObjectAction(0);

	if (pCoreShell)
	{
		isRun  = false;
		moveEndPoint.x = nDestPontX;
		moveEndPoint.y = nDestPontY;
		pCoreShell->GotoWhere(nDestPontX,nDestPontY,0); //走向鼠标坐标点
	}*/
	
}

void  KSkillRocker::ccTouchEnded(CCTouch* touch, CCEvent* event)
{//移动结束
	if (pCoreShell && pCoreShell->getTradeState()) return;

	//if (pCoreShell) pCoreShell->setPadCanMove(PAD_KIND_ITEM,true);

	currentPoint = centerPoint;
	//if(isFollowRole){
	//this->setVisible(false);
	//}
	//if (pCoreShell)
	//   pCoreShell->setSand();
	isRun = true;
	//this->setVisible(false);
	//setSpriteOpacity(100);
	setSpritePiont(ccp(visibleSize.width,-100.0f));
	//this->Inactive();
	//CCMessageBox("移动结束","移动结束");
}

KSkillRocker* KSkillRocker::initWithCenter(CCPoint aPoint ,float aRadius ,CCSprite* aJsSprite,CCSprite* aJsBg,bool _isFollowRole){
	isFollowRole =_isFollowRole;
	active = false;
	isRun  = false;
	radius = aRadius;
	visibleSize = CCDirector::sharedDirector()->getVisibleSize();//可见的
	pCoreShell = NULL;
	//__GameWorld = NULL;
	moveEndPoint = ccp(0,0);
	if(!_isFollowRole){
		centerPoint =aPoint;
	}else{
		centerPoint =ccp(0,0);  //中心坐标
	}
	currentPoint = centerPoint; //移动后当前的坐标

	/*ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setContentSize(CCSizeMake(200,200));//设置和窗口的大小
	colorLayer->setPosition(CCPointMake(visibleSize.width-200,10));
	colorLayer->setAnchorPoint(ccp(0,0));
	//colorLayer->addChild(spriteBox);
	this->addChild(colorLayer);
	*/
	jsSprite = aJsSprite;
	jsSprite->setPosition(centerPoint);//中心spr
	bgSprite = aJsBg;
	aJsBg->setPosition(centerPoint);   //背景spr
	aJsBg->setTag(1000);
	this->addChild(aJsBg);
	this->addChild(jsSprite);
	if(isFollowRole){
		this->setVisible(false);
	}
	//this->Active();//激活摇杆
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return this;
}


void  KSkillRocker::setSpriteOpacity(int ver)
{
	if (jsSprite)
	  jsSprite->setOpacity(ver);
	if (bgSprite)
	  bgSprite->setOpacity(ver);
}

void  KSkillRocker::setSpriteVisible(bool ver)
{
	this->setVisible(ver);

}

void  KSkillRocker::setSpritePiont(CCPoint aPoint)
{
	this->Active();
	centerPoint  = aPoint;
	currentPoint = centerPoint;              //移动后当前的坐标
    if (jsSprite)
	   jsSprite->setPosition(centerPoint);   //中心spr
	if (bgSprite)
	   bgSprite->setPosition(centerPoint);   //背景spr
}