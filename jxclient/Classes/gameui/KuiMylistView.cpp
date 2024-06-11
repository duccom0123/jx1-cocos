#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiMylistView.h"
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

KuiMylistView::KuiMylistView()
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

KuiMylistView::~KuiMylistView()
{
}

//第二部：计算出两个向量之间的夹角
//因为求出的角度是以π为单位，所以这里要转换成以度为单位
float KuiMylistView::getAngleSigned()
{//ccpSub(centerPoint, currentPoint),CCPointMake(0, 1)) 
    float rad = getRad(centerPoint,currentPoint);

	return 180.f/3.141592654*rad;

}

float KuiMylistView::getRad(CCPoint p1,CCPoint p2)
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


KuiMylistView * KuiMylistView::create()
{
	KuiMylistView *widget = new KuiMylistView();
	if (widget && widget->init())
	{
		widget->autorelease();

		//widget->setTouchMode(kCCTouchesOneByOne);
		widget->setTouchEnabled(true);
		widget->setEnabled(true);
		widget->ignoreAnchorPointForPosition(true);
		widget->setAnchorPoint(ccp(0.5f, 0.5f));
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}


KuiMylistView* KuiMylistView::initWithCenter(){
	
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

    jsSprite = CCSprite::create("useskill_z.png");
	jsSprite->setPosition(centerPoint);//中心spr
	bgSprite = CCSprite::create("useskill_b.png");
	bgSprite->setPosition(centerPoint);   //背景spr
	//bgSprite->setTag(1000);
	//bgSprite->retain();
	//jsSprite->retain();
	this->addChild(bgSprite);
	this->addChild(jsSprite);
	if(isFollowRole){
		//this->setVisible(false);
		setSpriteVisible(false);
	}
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return this;
}


/*KuiMylistView* KuiMylistView::menuWithItem(CCMenuItem* item)
{
	return create(item, NULL);
}*/


// 重写registerWithTouchDispatcher
void KuiMylistView::registerWithTouchDispatcher()
{
	//这里优先级设为1，只要比CCScrollView、tableView、CCControlButton等（这些优先级都为0）低就可以
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
	//CCDirector* pDirector = CCDirector::sharedDirector();
	//pDirector->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(),true);//false
}


bool KuiMylistView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	if (g_GameWorld && g_pCoreShell && g_pCoreShell->getTradeState()) return false;

	_useSkillModel=g_GameWorld->getUseSkillModel();//0 为自动 1 为手动

	return true;
}

void KuiMylistView::ccTouchMoved(CCTouch* touch, CCEvent* event)
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
}

void KuiMylistView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{

	if (g_GameWorld && g_pCoreShell && g_pCoreShell->getTradeState()) return;
	currentPoint = centerPoint;
	isRun = true;
}

void KuiMylistView::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{

}


void  KuiMylistView::setSpriteOpacity(int ver)
{
	if (jsSprite)
		jsSprite->setOpacity(ver);
	if (bgSprite)
		bgSprite->setOpacity(ver);
}

void  KuiMylistView::setSpriteVisible(bool ver)
{
	if (jsSprite)
		jsSprite->setVisible(ver);
	if (bgSprite)
		bgSprite->setVisible(ver);

}

void  KuiMylistView::setSpritePiont(CCPoint aPoint)
{
	centerPoint  = aPoint;
	currentPoint = centerPoint;              //移动后当前的坐标
	if (jsSprite)
		jsSprite->setPosition(centerPoint);   //中心spr
	if (bgSprite)
		bgSprite->setPosition(centerPoint);   //背景spr
}