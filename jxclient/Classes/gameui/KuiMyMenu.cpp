#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiMyMenu.h"
#include "KuiItem.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
#include "gamecore/KPlayer.h"
#include "engine/KCodec.h"
#include "engine/KCodecLzo.h"
#include "engine/KFile.h"
#include "engine/KFilePath.h"
#include "engine/KList.h"
#include "engine/KMemBase.h"
#include "engine/KMemClass.h"
#include "engine/KMemStack.h"
#include "engine/KNode.h"
#include "engine/KPakFile.h"
#include "engine/KStrBase.h"
#include "engine/KPakList.h"
#include "engine/XPackFile.h"
#include "gamecore/KSkillManager.h"
#include "gameui/KuiSkilldesc.h"
#include "gameui/KCheckbox.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiMyMenu::KuiMyMenu()
{
	isFollowRole =false;
	active = false;
	isRun  = false;
	radius = 60;
	nRolePoint=ccp(0,0);
	nRoleIndex = 0;

	jsSprite=NULL;
	bgSprite=NULL;
	_BackAttackRadius=0;
	_CurSkillId=0;
	_Beilv=0;
	_useSkillModel=0;
}

KuiMyMenu::~KuiMyMenu()
{
	/*if  (bgSprite)
	    bgSprite->release();
	if (jsSprite)
	    jsSprite->release();*/
}

void KuiMyMenu::updatePos(float dt){
	if  (jsSprite)
	    jsSprite->setPosition(ccpAdd(jsSprite->getPosition(),ccpMult(ccpSub(currentPoint, jsSprite->getPosition()),0.5)));
}
//启动摇杆
void KuiMyMenu::Active()
{
	if (!active)
	{
		active=true;
		this->schedule(schedule_selector(KuiMyMenu::updatePos));                       //添加刷新函数
		//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,0,false);
	}else {
	}
}
//解除摇杆
void   KuiMyMenu::Inactive()
{
	if (active) {
		active=false;
		this->unschedule(schedule_selector(KuiMyMenu::updatePos));                 //删除刷新
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);//删除委托
	}else {
	}
}


//第二部：计算出两个向量之间的夹角
//因为求出的角度是以π为单位，所以这里要转换成以度为单位
float KuiMyMenu::getAngleSigned()
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

float KuiMyMenu::getRad(CCPoint p1,CCPoint p2)
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
CCPoint KuiMyMenu::getDirection()
{
	return ccpNormalize(ccpSub(centerPoint, currentPoint));
}
//摇杆和中心点的距离
float KuiMyMenu::getVelocity()
{
	return ccpDistance(centerPoint, currentPoint);
}

KuiMyMenu * KuiMyMenu::create(CCMenuItem* item, ...)
{
	va_list args;
	va_start(args,item);
	KuiMyMenu *pRet = new KuiMyMenu();
	if (pRet && pRet->createWithItems(item, args))
	{
		pRet->autorelease();
		va_end(args);
		//开始创建一个控制遥杆
		pRet->setTouchPriority(kCCMenuHandlerPriority);
		pRet->setTouchMode(kCCTouchesOneByOne);
		pRet->setTouchEnabled(true);
		pRet->setEnabled(true);
		pRet->ignoreAnchorPointForPosition(true);
		pRet->setAnchorPoint(ccp(0.5f, 0.5f));
		pRet->m_pSelectedItem = NULL;
		pRet->m_eState = kCCMenuStateWaiting;

		pRet->initWithCenter();
		return pRet;
	}
	va_end(args);
	CC_SAFE_DELETE(pRet);
	return NULL;
}


KuiMyMenu* KuiMyMenu::initWithCenter(){
	
	visibleSize = CCDirector::sharedDirector()->getVisibleSize();//可见的
	//pCoreShell = NULL;
	//__GameWorld = NULL;
	moveEndPoint = ccp(0,0);

	if(!isFollowRole){
		centerPoint =ccp(visibleSize.width,-100.0f);
	}else{
		centerPoint =ccp(visibleSize.width,-100.0f);  //中心坐标
	}
	currentPoint = centerPoint; //移动后当前的坐标

	/*ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setContentSize(CCSizeMake(200,200));//设置和窗口的大小
	colorLayer->setPosition(CCPointMake(visibleSize.width-200,10));
	colorLayer->setAnchorPoint(ccp(0,0));
	//colorLayer->addChild(spriteBox);
	this->addChild(colorLayer);*/

	bgSprite = CCSprite::create("useskill_b.png");
	bgSprite->setPosition(centerPoint);   //背景spr
	jsSprite = CCSprite::create("useskill_z.png");
	jsSprite->setPosition(centerPoint);//中心spr
	//bgSprite->setTag(1000);
	//bgSprite->retain();
	//jsSprite->retain();
	this->addChild(bgSprite);
	this->addChild(jsSprite);
	if(isFollowRole){
		//this->setVisible(false);
		setSpriteVisible(false);
	}
	this->Active();//激活摇杆
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return this;
}


KuiMyMenu* KuiMyMenu::menuWithItem(CCMenuItem* item)
{
	return create(item, NULL);
}


// 重写registerWithTouchDispatcher
void KuiMyMenu::registerWithTouchDispatcher()
{
	//这里优先级设为1，只要比CCScrollView、tableView、CCControlButton等（这些优先级都为0）低就可以
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(),true);//false
}


bool KuiMyMenu::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	//先执行遥杆
	//CCMessageBox("KuiMyMenu::ccTouchBegan","CCTouch");

	if (!active)
		return false;
	if (g_GameWorld && g_pCoreShell && g_pCoreShell->getTradeState()) return false;

	_useSkillModel=g_GameWorld->getUseSkillModel();//0 为自动 1 为手动

	//-------------------------------------------------
	CC_UNUSED_PARAM(event);
	if (m_eState != kCCMenuStateWaiting || !isEnabled())
	{
		return false;
	}

	for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
	{
		if (c->isVisible() == false)
		{
			return false;
		}
	}

	m_pSelectedItem = this->itemForTouch(touch);
	if (m_pSelectedItem)
	{
		//CCMessageBox("m_pSelectedItem Began","m_pSelectedItem");
		if (_useSkillModel==0) //如果是自动 才执行
		   m_pSelectedItem->activate();//执行回调函数

		m_eState = kCCMenuStateTrackingTouch;
		if (isCheckBox && m_pSelectedItem->GetTagbyKey()=="box")//
		{
			if (m_pSelectedItem->isSelected())
				m_pSelectedItem->unselected();
			else
				m_pSelectedItem->selected();
		}
		else
			m_pSelectedItem->selected();

		setSpriteOrder(0);
		setSpritePiont(m_pSelectedItem->getPosition());
		setSpriteVisible(true);
		setSpriteOpacity(105);
		if (_useSkillModel==1) //如果不是自动 才执行
		{
			g_GameWorld->setAttackSprPosition(ccp(visibleSize.width/2,visibleSize.height/2));//nRolePoint
			g_GameWorld->setAttackSprVisible(true);
		}

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
			bgSprite->setPosition(currentPoint);  //this->getChildByTag(1000)
		}
		//获取技能的范围
		if (_useSkillModel==1) //如果不是自动 才执行
		{
		  _CurSkillId = m_pSelectedItem->getTag();
		  if (_CurSkillId<=0 || _CurSkillId>=MAX_SKILL) return false;
		  ISkill * pOrdinSkill = g_SkillManager.GetSkill(_CurSkillId, 1);
		  if (pOrdinSkill && !pOrdinSkill->IsAura())
		  {
			_BackAttackRadius = pOrdinSkill->getBackAttackRadius();
		  }
		  _Beilv=_BackAttackRadius/radius;
		}
		return true;
	}
	return false;
}

void KuiMyMenu::ccTouchMoved(CCTouch* touch, CCEvent* event)
{

	if (g_GameWorld && g_pCoreShell && g_pCoreShell->getTradeState()) return;
	//if (pCoreShell) pCoreShell->setPadCanMove(PAD_KIND_ITEM,false);
	//int nBeilv = 2;
	CCPoint touchPoint = touch->getLocationInView();                     //屏幕坐标系 +x 向右 +ｙ向下
	touchPoint = CCDirector:: sharedDirector()->convertToGL(touchPoint); //转换成左下角gl起点坐标(0,0)
	float nDistance = ccpDistance(touchPoint, centerPoint);
	if (nDistance > radius)
	{//超出范围
		currentPoint =ccpAdd(centerPoint,ccpMult(ccpNormalize(ccpSub(touchPoint, centerPoint)), radius));
	}else {
		currentPoint = touchPoint;
	}

	CC_UNUSED_PARAM(event);
	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchMoved] -- invalid state");
	CCMenuItem *currentItem = this->itemForTouch(touch);
	if (currentItem != m_pSelectedItem) 
	{
		if (m_pSelectedItem)
		{
			m_pSelectedItem->unselected();
		}
		m_pSelectedItem = currentItem;
		if (m_pSelectedItem)
		{
			m_pSelectedItem->selected();
		}
		//---如果不相同的按钮就返回
		return;
	}
	
	//开始检测技能移动的位置 关联技能数据
	//currentPoint 当前触摸的位置 centerPoint中心圆点的原始位置
	float nVer = getAngleSigned();  //中心点与触摸点的角度
	float nDestPontX,nDestPontY;
	
	if (nVer==0)
	{//正右
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*_Beilv;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/nRolePoint.y;
	}
	else if(nVer>0 && nVer<90)
	{//右上
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*_Beilv;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y+(currentPoint.y-centerPoint.y)*_Beilv);
	}
	else if (nVer==90)
	{//正上
		nDestPontX = nRolePoint.x;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y+(currentPoint.y-centerPoint.y)*_Beilv);
	}
	else if (nVer>90 && nVer<180)
	{//左上
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*_Beilv;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y+(currentPoint.y-centerPoint.y)*_Beilv);
	}
	else if (nVer==180)
	{//正左
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*_Beilv;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/nRolePoint.y;
	}
	else if (nVer>180 && nVer<270)
	{//左下
		nDestPontX = nRolePoint.x-(centerPoint.x-currentPoint.x)*_Beilv;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y-(centerPoint.y-currentPoint.y)*_Beilv);
	}
	else if (nVer==270)
	{//正下
		nDestPontX = nRolePoint.x;//目标gl坐标x y不变
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y-(centerPoint.y-currentPoint.y)*_Beilv);
	}
	else if (nVer>270 && nVer<360)
	{//右下
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*_Beilv;//
		nDestPontY = /*visibleSize.height-*/(nRolePoint.y-(centerPoint.y-currentPoint.y)*_Beilv);
	}
	else if (nVer==360)
	{//正右
		nDestPontX = nRolePoint.x+(currentPoint.x-centerPoint.x)*_Beilv;//
		nDestPontY = /*visibleSize.height-*/nRolePoint.y;
	}
	isRun  = false;
	moveEndPoint.x = nDestPontX;
	moveEndPoint.y = nDestPontY;
	if (_useSkillModel==1) //如果不是自动 才执行
	{
		g_GameWorld->setAttackSprPosition(moveEndPoint);
	}
	
	//int nX = nDestPontX;
	//int nY = nDestPontY;
	//int nZ = 0;

	//SubWorld[0].GetMpsByLocalPosition(nX, nY, nZ);  //将屏幕坐标 换成 转成像素坐标
	//g_pCoreShell->setNpcDir(nX,nY);
	//g_pCoreShell->GotoWhere(nDestPontX,nDestPontY,0); //走向鼠标坐标点
}

void KuiMyMenu::ccTouchEnded(CCTouch* touch, CCEvent* event)
{

	if (g_GameWorld && g_pCoreShell && g_pCoreShell->getTradeState()) return;

	//if (pCoreShell) pCoreShell->setPadCanMove(PAD_KIND_ITEM,true);
	currentPoint = centerPoint;
	if(isFollowRole){
	//this->setVisible(false);
	   setSpriteVisible(false);
	}
	isRun = true;
	//this->setVisible(false);
	//setSpriteOpacity(100);
	setSpritePiont(ccp(visibleSize.width,-100.0f));

	g_GameWorld->setAttackSprVisible(false);
	//this->Inactive();
	//CCMessageBox("移动结束","移动结束");
	
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(event);
	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchEnded] -- invalid state");
	if (m_pSelectedItem)
	{
		if (isCheckBox)
		{//如果是使box
			if  (m_pSelectedItem->GetTagbyKey()!="box")
				m_pSelectedItem->unselected();
		}
		else
			m_pSelectedItem->unselected();

		//m_pSelectedItem->activate();//执行回调函数
	}
	m_eState = kCCMenuStateWaiting;

	//手动控制开始攻击
	if (_useSkillModel==1) //如果是手动 才执行
	{
		g_GameWorld->customSkillAttack(_CurSkillId,moveEndPoint.x,visibleSize.height-moveEndPoint.y);
		_Beilv=0;
		_BackAttackRadius=0;
		_useSkillModel=0;
		_CurSkillId=0;
	}
}

void KuiMyMenu::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(event);
	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchCancelled] -- invalid state");
	if (m_pSelectedItem)
	{
		m_pSelectedItem->unselected();
	}
	m_eState = kCCMenuStateWaiting;
}


void  KuiMyMenu::setSpriteOpacity(int ver)
{
	if (jsSprite)
		jsSprite->setOpacity(ver);
	if (bgSprite)
		bgSprite->setOpacity(ver);
}

void  KuiMyMenu::setSpriteVisible(bool ver)
{
	if (jsSprite)
		jsSprite->setVisible(ver);
	if (bgSprite)
		bgSprite->setVisible(ver);

}

void  KuiMyMenu::setSpriteOrder(int ver)
{
	if  (ver<=0)
	{
		if (bgSprite)
			bgSprite->setZOrder(zOrder);
		if (jsSprite)
			jsSprite->setZOrder(zOrder);
		
	}
	else
	{
		if (bgSprite)
			bgSprite->setZOrder(ver);
		if (jsSprite)
			jsSprite->setZOrder(ver);
	}
}

void  KuiMyMenu::setSpritePiont(CCPoint aPoint)
{
	this->Active();
	centerPoint  = aPoint;
	currentPoint = centerPoint;              //移动后当前的坐标
	if (jsSprite)
		jsSprite->setPosition(centerPoint);   //中心spr
	if (bgSprite)
		bgSprite->setPosition(centerPoint);   //背景spr
}