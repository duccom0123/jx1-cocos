#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "Uiglobaldata.h"
#include "engine/CXmlStream.h"
#include "KuiMoveToMap.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
//#include "engine/KPalette.h"
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
#include "gamescene/KScenePlaceC.h"
#include "gamecore/KSubWorld.h"
//#include "gamecore/KSkillManager.h"
//#include "gamecore/KItem.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;
#define	WORLD_MAP_POS_FILE		"\\ui\\小地图_山洞地图版_透明按钮.ini"
KuiMoveToMap::KuiMoveToMap()
{
	isOpen = false;
	red               = NULL;
	clipper           = NULL;
	content_Map       = NULL;
	m_PosCount        = 0;
	nMapPicSpr = NULL;
	ZeroMemory(m_MoveMapBtn,sizeof(mapListData)*MAX_COUNT_MAPS);
}

KuiMoveToMap::~KuiMoveToMap()
{
	isOpen = false;
	if (red)
	    red->release();
}

//创建确定弹出框
KuiMoveToMap * KuiMoveToMap::create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiMoveToMap * popLayer = KuiMoveToMap::create();
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiMoveToMap::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");
	//添加模态背景
	winSize = CCDirector::sharedDirector()->getWinSize();
	ccColor4B color = ccc4(255,255,255,0);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	bgSize  = winSize;//CCSizeMake(376,326);
	colorLayer->setContentSize(winSize); //整个屏幕
	ParentNode_Team= colorLayer;         //-1+45  +43
	colorLayer->setPosition(ccp(winSize.width/2-bgSize.width/2,winSize.height/2-bgSize.height/2));
	this->addChild(colorLayer);
	//获得背景图片的大小
	CCSize contentSize = colorLayer->getContentSize();
	m_size = contentSize;
	m_origin = colorLayer->getPosition();

	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void KuiMoveToMap::addDialogData()
{
	if  (!g_pCoreShell) return;

	/*int nRoleMpsX,nRoleMpsY,nMapidx;
	nMapidx = g_GameWorld->getMapIdx();
	g_ScenePlace.GetSceneFocus(nMapidx,nRoleMpsX,nRoleMpsY);
	nRoleDisX = nRoleMpsX-maxMapRc.left*512;     //人物距离原点的距离 offx  宽度
	nRoleDisY = nRoleMpsY-maxMapRc.top*1024;     //人物距离原点的距离 offy  高度
	/*char msg[64];
	int  NewscrX= nRoleMpsX,NewscrY= nRoleMpsY;
	SubWorld[0].GetLocalPositionByMps(NewscrX,NewscrY,0);      //像素坐标－－－＞屏幕坐标
	sprintf(msg,"x:%d,y:%d",NewscrX,NewscrY);
	CCMessageBox(msg,"DSTEN");*/
	//\Spr\Ui3\小地图\打怪向导山洞图.spr


	char nSprName[128]={0};
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D* nPicTexture=NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));

	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\小地图\\打怪向导山洞图.spr");
	g_StrLower(nSprName);

	nPicTexture = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (nPicTexture==NULL)
		return ;

	nTextureSize = nPicTexture->getContentSize();
	ccColor4B color = ccc4(0,0,0,150);
	CCLayerColor * bgLayer = CCLayerColor::create(color);//颜色层
	bgLayer->setAnchorPoint(ccp(0.5,0.5));
	bgLayer->setContentSize(CCSizeMake(376,326));  //CCSizeMake(376,326)
	bgLayer->setPosition(ccp(m_size.width/2-bgLayer->getContentSize().width/2+45,m_size.height/2-bgLayer->getContentSize().height/2+43));
	ParentNode_Team->addChild(bgLayer);

	clipper = CCClippingNode::create();//创建一个可显示区域
	clipper->setTag(1);
	clipper->setContentSize(CCSizeMake(370,320)); //CCSizeMake(370,320)
	clipper->setAnchorPoint(ccp(0.5,0.5));
	//clipper->setPosition(ccp(m_size.width/2-clipper->getContentSize().width/2+45,m_size.height/2-clipper->getContentSize().height/2+43));
	clipper->setPosition(ccp(m_size.width/2+45,m_size.height/2+43));//GL POIT
	//clipper->runAction(CCRepeatForever::create(CCRotateBy::create(1, 45)));
	ParentNode_Team->addChild(clipper);

	CCSize nSize;
	nSize.width =376;
	nSize.height=16;
	CCLabelTTF *pdescInfo =  CCLabelTTF::create(UTEXT("点击空白(无文字)处拖动,点击对应地图移动",1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	//pdescInfo->setColor(ccYELLOW);
	pdescInfo->setPosition(ccp(m_size.width/2+40,m_size.height/2-clipper->getContentSize().height/2+35));
	ParentNode_Team->addChild(pdescInfo,10);

	content_Map = CCSprite::createWithTexture(nPicTexture);
	content_Map->setTag(2);
	content_Map->setAnchorPoint(ccp(0,0));
	//content->setPosition(ccp(clipper->getContentSize().width/2,clipper->getContentSize().height/2));
	//content_Map->setPosition(ccp(-nRoleDisX/16+clipper->getContentSize().width/2,-(nPicTexture->getContentSize().height-nRoleDisY/32)+clipper->getContentSize().height/2));
	//nRoleDisX/16，nRoleDisY/32，图内的这个偏移就是主角的位置(相对于图片的大小来说)
	content_Map->setPosition(ccp(0,0));//相对主节点的坐标
	clipper->addChild(content_Map);//设置被裁剪的精灵

	m_pUiLayer = UILayer::create();
	//ParentNode_Team->addChild(m_pUiLayer);
	content_Map->addChild(m_pUiLayer);

	KIniFile	Ini;
	if (Ini.Load(WORLD_MAP_POS_FILE))
	{
		Ini.GetInteger("BUTTON_COUNT","Count",0,&m_PosCount);
		//Width=754
		//Height=578
		if (m_PosCount>0 && m_PosCount<MAX_COUNT_MAPS)
		{
			char nBtn[32]={0};
			int nLeft,nTop,nWidth,nHeight,nSubwordId,nPosX,nPosY;
			for (int i=0;i<m_PosCount;++i)
			{   
				sprintf(nBtn,"BUTTON_%d",i);
				Ini.GetInteger(nBtn,"Left",0,&nLeft);
				Ini.GetInteger(nBtn,"Top",0,&nTop);
				Ini.GetInteger(nBtn,"Width",0,&nWidth);
				Ini.GetInteger(nBtn,"Height",0,&nHeight);
				Ini.GetInteger(nBtn,"SubwordId",0,&nSubwordId);
				Ini.GetInteger(nBtn,"TargetPosX",0,&nPosX);
				Ini.GetInteger(nBtn,"TargetPosY",0,&nPosY);
				m_MoveMapBtn[i].m_SubwordId = nSubwordId;
				m_MoveMapBtn[i].m_PosX      = nPosX;
				m_MoveMapBtn[i].m_PosY      = nPosY;
				m_MoveMapBtn[i].m_MoveMapBtn = UIButton::create();
				m_MoveMapBtn[i].m_MoveMapBtn->setTouchEnabled(true);
				m_MoveMapBtn[i].m_MoveMapBtn->loadTextures("btn_n.png", "loadingStart.png","");
				m_MoveMapBtn[i].m_MoveMapBtn->setScale9Enabled(true);
				m_MoveMapBtn[i].m_MoveMapBtn->setSize(CCSizeMake(nWidth,nHeight));
				//m_MoveMapBtn[i].m_MoveMapBtn->setContentSize(CCSizeMake(nWidth,nHeight));
				//m_MoveMapBtn[i].m_MoveMapBtn->ignoreContentAdaptWithSize(true);
				m_MoveMapBtn[i].m_MoveMapBtn->setPosition(ccp(nLeft+20,nTextureSize.height-nTop-8));
				m_MoveMapBtn[i].m_MoveMapBtn->setTag(i+1);
				m_MoveMapBtn[i].m_MoveMapBtn->setTagbyKey(nBtn);
				m_MoveMapBtn[i].m_MoveMapBtn->addTouchEventListener(this,toucheventselector(KuiMoveToMap::oktouchEvent));
				/*char nTemp[64],mapName[128];
				ZeroMemory(nTemp,sizeof(nTemp));
				ZeroMemory(mapName,sizeof(mapName));
				t_sprintf(nTemp,"%d_name",nSubwordId);
				g_NpcMapDropRate.GetString("List",nTemp,0,mapName,sizeof(mapName));
				m_MoveMapBtn[i].m_MoveMapBtn->setTitleText(UTEXT(mapName,1).c_str());
				*/
				m_pUiLayer->addWidget(m_MoveMapBtn[i].m_MoveMapBtn);
			} 
		}

	}
	Ini.Clear(); 

	CCDrawNode *stencil = CCDrawNode::create();
	CCPoint rectangle[4];
	rectangle[0] = ccp(0,0);
	rectangle[1] = ccp(clipper->getContentSize().width, 0);
	rectangle[2] = ccp(clipper->getContentSize().width, clipper->getContentSize().height);
	rectangle[3] = ccp(0,clipper->getContentSize().height);

	ccColor4F white = {1, 1, 1, 1};
	stencil->drawPolygon(rectangle,4,white,1,white);
	clipper->setStencil(stencil); //设置模板

	m_bScrolling = false;
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiMoveToMap::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	
	return;
}

//执行上层对象的回调函数，关闭弹出框
void KuiMoveToMap::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = NULL;
	if (pSender)
		node= dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);
	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiMoveToMap::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}
//点击按钮回调函数
void KuiMoveToMap::oktouchEvent(CCObject *pSender, TouchEventType type)
{
	if  (!g_pCoreShell || !g_GameWorld) return;

	switch (type)
	{
	case TOUCH_EVENT_BEGAN://Touch Down
		{
			if (pSender)
			{
				UIButton* nTbtn = (UIButton*)pSender;
				if (nTbtn)
				{
					nTbtn->setTitleColor(ccYELLOW);
					int nSelIndex = nTbtn->getTag()-1;
					g_pCoreShell->OperationRequest(GOI_MAP_SUNYI,m_MoveMapBtn[nSelIndex].m_SubwordId,1); 
				}
				buttonCallBackFunc(NULL);
				break;
/*				
				CCNode *clipper = ParentNode_Team->getChildByTag(1);
				CCNode *content = clipper->getChildByTag(2);
				CCPoint diff = ccpSub(content->getPosition(),ccp(0,0));//偏移了原来的地方的距离坐标差值
				CCPoint curPos = ccpAdd(nTbtn->getPosition(),diff);
				curPos = ccpAdd(curPos,ccp(clipper->getPosition().x-clipper->getContentSize().width/2,clipper->getPosition().y-clipper->getContentSize().height/2));
				//CCPoint point = clipper->convertToNodeSpace(curPos);
				//CCRect rect = CCRectMake(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
				//m_bScrolling = rect.containsPoint(point);
				char nTempPoit[128];
				sprintf(nTempPoit,"%s:%f/%f",nTbtn->getTitleText(),curPos.x,curPos.y);
				CCMessageBox(nTempPoit,"test");
*/
			}
		}
		break;
	case TOUCH_EVENT_MOVED://Touch Move
		break;
	case TOUCH_EVENT_ENDED://Touch Up
		{
			if (pSender)
			{
				UIButton* nTbtn = (UIButton*)pSender;
				nTbtn->setTitleColor(ccWHITE);
				if  (nTbtn->getTag()==1)
				{

				}
			}
		}
		break;
	case TOUCH_EVENT_CANCELED:
		break;

	default:
		break;
	}
}
//重写触摸注册函数，重新给定触摸级别
void KuiMoveToMap::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2,true);
}

CCRect KuiMoveToMap::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiMoveToMap::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCNode *clipper = ParentNode_Team->getChildByTag(1);
	CCPoint point = clipper->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;
	/*if (!m_bScrolling)
	    button->setTouchEnabled(false);
	else
		button->setTouchEnabled(true);*/
	return true;
}
//移动中
void KuiMoveToMap::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	//CCTouch *touch = (CCTouch*)pTouches->anyObject();
	//if  (nPartNode)
	//	nPartNode->setVisible(false);
	CCNode *clipper = ParentNode_Team->getChildByTag(1);
	CCPoint point = clipper->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCPoint diff = ccpSub(point,m_lastPoint);
	CCNode *content = clipper->getChildByTag(2);//设置图片的位置
	content->setPosition(ccpAdd(content->getPosition(),diff));	
	m_lastPoint = point;
}
//移动结束
void KuiMoveToMap::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	//if  (nPartNode)
	//	nPartNode->setVisible(true);

	if (!m_bScrolling) return;
	   m_bScrolling = false;

}


void KuiMoveToMap::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCNode *clipper = ParentNode_Team->getChildByTag(1);
	CCPoint point = clipper->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(touch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
	m_bScrolling = rect.containsPoint(point);
	m_lastPoint = point;
}

void KuiMoveToMap::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	CCTouch *touch = (CCTouch*)pTouches->anyObject();
	CCNode *clipper = ParentNode_Team->getChildByTag(1);
	CCPoint point = clipper->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(touch->getLocationInView()));
	CCPoint diff = ccpSub(point, m_lastPoint);
	CCNode *content = clipper->getChildByTag(2);
	content->setPosition(ccpAdd(content->getPosition(),diff));
	m_lastPoint = point;
}

void KuiMoveToMap::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	m_bScrolling = false;
}
void KuiMoveToMap::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (g_GameWorld && isOpen && g_pCoreShell)
	{
		if  (clipper)
		{
			CCNode *content = clipper->getChildByTag(2);//设置图片的位置)
			if (!content) return;
			//CCPoint point = clipper->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
			for (int i=0;i<m_PosCount;++i)
			{
				if (m_MoveMapBtn[i].m_SubwordId>0 && m_MoveMapBtn[i].m_MoveMapBtn)
				{

					CCPoint diff = ccpSub(content->getPosition(),ccp(0,0));//偏移了原来的地方的距离坐标差值
					CCPoint curPos = ccpAdd(m_MoveMapBtn[i].m_MoveMapBtn->getPosition(),diff);
					curPos = ccpAdd(curPos,ccp(clipper->getPosition().x-clipper->getContentSize().width/2,clipper->getPosition().y-clipper->getContentSize().height/2));

					CCPoint newPoint = clipper->convertToNodeSpace(curPos);
					CCRect rect = CCRectMake(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
					bool m_IsIn = rect.containsPoint(newPoint);

					//CCRect rect = CCRectMake(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
					//bool m_IsIn = rect.containsPoint(clipper->convertToNodeSpace(ccpAdd(content->getPosition(),diff)));
					if  (m_IsIn)//在范围内
						m_MoveMapBtn[i].m_MoveMapBtn->setTouchEnabled(true);
					else
						m_MoveMapBtn[i].m_MoveMapBtn->setTouchEnabled(false);
				}
			}
			
		}
		return;
	}
}

void KuiMoveToMap::draw()
{
	//CCMessageBox("无限循环","draw");
}