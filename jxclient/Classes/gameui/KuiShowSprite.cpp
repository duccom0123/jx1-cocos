#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "Uiglobaldata.h"
#include "engine/CXmlStream.h"
#include "KuiShowSprite.h"
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
#include "gamecore/ImgRef.h"
#include "gamecore/KSubWorld.h"
//#include "gamecore/KSkillManager.h"
//#include "gamecore/KItem.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;
#define MAX_LOOP_TIME	180
KuiShowSprite::KuiShowSprite()
{
	isOpen = false;
	red               = NULL;
	sFarmsSprite      = NULL;
	sFarmsTexture     = NULL;
	pPointLabel = NULL;
	__loopTime  = 0;
	_nFarms     = 0;
    _curFarms   = 0;
	_nModel     = 0;
	_nInterval  = 0;		        //帧间隔
	_nFlipTime  = 0;
	ZeroMemory(nSprNamePath,sizeof(nSprNamePath));
	_nXpos = 0;
	_nYpos = 0;
	_bRenderFlag=0;
	ptestLabel =NULL;
	_pHwidth = 0;
	_pHheight = 0;
}

KuiShowSprite::~KuiShowSprite()
{
	isOpen = false;
	if (red)
	    red->release();
}

//创建确定弹出框
KuiShowSprite * KuiShowSprite::create(char * nFilePath,int nXpos,int nYpos,CCObject * callbackListener,SEL_CallFuncN callfun,bool nTouchesModel,int nRenderFlag)
{
	if (!nFilePath) return NULL;

	KuiShowSprite * popLayer = KuiShowSprite::create();

	popLayer->mianDialogSet(nFilePath,nXpos,nYpos,nTouchesModel,nRenderFlag);

	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiShowSprite::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void KuiShowSprite::mianDialogSet(char * nFilePath,int nXpos,int nYpos,bool nTouchesModel,int nRenderFlag)
{
	if  (nTouchesModel)
	  this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	else
      this->setTouchMode(kCCTouchesAllAtOnce);

	int m_nWidth,m_nHeight,nFrams;
	//CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	//ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprNamePath,nFilePath);
	g_StrLower(nSprNamePath);

	sFarmsTexture	=_getinidata.getinidata_one(nSprNamePath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	//bgCur   =_getinidata.getinidata_new(nSprNamePath,0,&m_nWidth,&m_nHeight,&nFrams);
	
	if  (!sFarmsTexture) return;

	_bRenderFlag = nRenderFlag;
	_nFarms    = nFrams;
	_nInterval = nSprInfo.nInterval;
	_nFlipTime = IR_GetCurrentTime();
	_nXpos = nXpos;
	_nYpos = nYpos;
	_pHwidth = nSprInfo.pHwidth;
	_pHheight = nSprInfo.pHheight;
	winSize = CCDirector::sharedDirector()->getWinSize();
	ccColor4B color = ccc4(0,0,0,0);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setAnchorPoint(ccp(0,0));
	bgSize  = CCSizeMake(nSprInfo.pHwidth,nSprInfo.pHheight);;//CCSizeMake(376,326);
	colorLayer->setContentSize(bgSize);
	ParentNode_Team= colorLayer;//-1+45  +43
	colorLayer->setPosition(ccp(nXpos,nYpos));
	this->addChild(colorLayer);
	//获得背景图片的大小
	//CCSize contentSize = colorLayer->getContentSize();
	m_size   = bgSize;
	m_origin = colorLayer->getPosition();
}

void KuiShowSprite::addDialogData()
{
	if  (!g_pCoreShell) return;

	/*int nRoleMpsX,nRoleMpsY,nMapidx;
	nMapidx = g_GameWorld->getMapIdx();
	g_ScenePlace.GetSceneFocus(nMapidx,nRoleMpsX,nRoleMpsY);
	nRoleDisX = nRoleMpsX-maxMapRc.left*512;     //人物距离原点的距离 offx  宽度
	nRoleDisY = nRoleMpsY-maxMapRc.top*1024;     //人物距离原点的距离 offy  高度
	*/
	sFarmsSprite=CCSprite::createWithTexture(sFarmsTexture);
	sFarmsSprite->setAnchorPoint(ccp(0,0));
	sFarmsSprite->setPosition(ccp(0,0));
	ParentNode_Team->addChild(sFarmsSprite);

	/*ptestLabel = CCLabelTTF::create("x:0/y:0",UI_GAME_FONT_DEFAULT,14);//Arial
	ptestLabel->setAnchorPoint(ccp(0,0));
	ptestLabel->setColor(ccYELLOW);
	ptestLabel->setPosition(ccp(0,0));
	ParentNode_Team->addChild(ptestLabel,1);
	*/
	if  (_nFarms>0)
	{//动画图

	}
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiShowSprite::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	
	return;
}

//执行上层对象的回调函数，关闭弹出框
void KuiShowSprite::buttonCallBackFunc(CCObject * pSender)
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
void KuiShowSprite::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

void KuiShowSprite::setRegisterWithTouchDispatcher(int nVer)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,nVer,true);
}
//重写触摸注册函数，重新给定触摸级别
/*void KuiShowSprite::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,1000,true);
}*/

CCRect KuiShowSprite::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiShowSprite::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return false;
}
//移动中
void KuiShowSprite::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	__loopTime = MAX_LOOP_TIME;
}
//移动结束
void KuiShowSprite::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	__loopTime = MAX_LOOP_TIME;
}

void KuiShowSprite::setShowPosition(int nPosX,int nPosY)
{
	if (ParentNode_Team)
	    ParentNode_Team->setPosition(ccp(nPosX,nPosY));
}

void KuiShowSprite::removeShowSprite()
{
	buttonCallBackFunc(NULL);
}

CCSize KuiShowSprite::getShowSize()
{
	return m_size;
}

void KuiShowSprite::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (g_GameWorld && isOpen && g_pCoreShell)
	{	
	}
}


CCTexture2D * KuiShowSprite::ccgetTxtData_s(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh)
{
	    if  (sprPath==NULL)
			return NULL;

	    char nSprName[128]={0};
		ZeroMemory(nSprName,sizeof(nSprName));
		t_sprintf(nSprName,sprPath);
		g_StrLower(nSprName);
		int m_nWidth,m_nHeight,nFrams;
		CCTexture2D *bgCur = NULL;
		SPRFRAMSINFO nSprInfo;
		ZeroMemory(&nSprInfo,sizeof(nSprInfo));
		//if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
		bgCur = _getinidata.getinidata_one(nSprName,nFramIndex,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
		
		if (bgCur==NULL)
			return NULL;

		*mHeadw = nSprInfo.pHwidth;
		*mHeadh = nSprInfo.pHheight;

#define CENTERX		160
#define	CENTERY		192
		//转化成象素坐标
		//SubWorld[0].GetMpsByLocalPosition(nMpsX,nMpsY,0);
		
		int nX = nMpsX;  //位置
		int nY = nMpsY;  //位置

		if (bSinglePlaneCoord == false)//转换成屏幕坐标 
			SubWorld[0].GetLocalPositionByMps(nX,nY,nMpsZ);

		/*if (bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
		{//渲染模式
		//****to be modify****
		  int nCenterX = nSprInfo.pHCenterX; //重心的水平位移
		  int nCenterY = nSprInfo.pHCenterY; //重心的垂直位移

		   if (nCenterX || nCenterY)
		   {
			 nX -= nCenterX;
			 nY -= nCenterY;
		   }
		   else if (nSprInfo.pHwidth > CENTERX)
		   {
			  nX -= CENTERX;
			  nY -= CENTERY;
		   }
		}*/

		if ((bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
		{//帧内有偏移的
			nX += nSprInfo.pFoffsetX;
			nY += nSprInfo.pFoffsetY;
		}
		//开始裁剪
		
		*mScreeX = nX;
		*mScreeY = nY;

	return bgCur;
}

void KuiShowSprite::draw()
{
	if  (isOpen && _nFarms>0){
		int nbackFarms =_curFarms;
	   IR_NextFrame(_curFarms,_nFarms,_nInterval,_nFlipTime);
	   if (_curFarms!=nbackFarms)
	   {//已经换帧了
		   int m_nWidth,m_nHeight,nFrams,mScreeX,mScreeY;
		   //CCTexture2D *bgCur = NULL;
		   SPRFRAMSINFO nSprInfo;
		   ZeroMemory(&nSprInfo,sizeof(nSprInfo));
		   sFarmsTexture = ccgetTxtData_s(nSprNamePath,_curFarms,0,0,0,&mScreeX,&mScreeY,_bRenderFlag,true,&m_nWidth,&m_nHeight);
		   //sFarmsTexture	=_getinidata.getinidata_one(nSprNamePath,_curFarms,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
		   if (sFarmsSprite && sFarmsTexture)
		   {//设置精灵纹理
			   CCRect CCRectSize = CCRectMake(0,0,0,0);
			   CCRectSize.size   = sFarmsTexture->getContentSize();
			   sFarmsSprite->setTexture(sFarmsTexture);
			   sFarmsSprite->setTextureRect(CCRectSize,false,CCRectSize.size);
			   sFarmsSprite->setPosition(ccp(mScreeX,bgSize.height-mScreeY-CCRectSize.size.height));
			   /*if (ptestLabel)
			   {
				   char nTemp[64];
				   t_sprintf(nTemp,"F:%d W:%d H:%d X:%d Y:%d",_nFarms,m_nWidth,m_nHeight,mScreeX,mScreeY);
				   ptestLabel->setString(nTemp);
			   }*/
		   }
	   }
	}
}