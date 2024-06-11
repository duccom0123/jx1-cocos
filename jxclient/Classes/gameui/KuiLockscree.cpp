//有按钮的　ｔａｌｋ　对话框
//
#include "gamecore/KCore.h"

#ifndef WIN32
#include <sys/select.h>
#include <ctype.h>
//typedef struct fd_set FD_SET;
#endif
#include "network/Socket.h"

#include "engine/KbugInfo.h"
#include "engine/CXmlStream.h"
#include "KuiLockscree.h"
#include "Klogin_f.h"
#include "engine/Text.h"
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
#include "engine/dataChecksum.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiLockscree::KuiLockscree()
{
	isOpen = false;
	colorLayer        = NULL;
	red               = NULL;
	m_pUiLayer        = NULL;
	m_StartPos.x=0;
	m_StartPos.y=0;
}

KuiLockscree::~KuiLockscree()
{
	isOpen = false;
	colorLayer        = NULL;
}

//创建确定弹出框
KuiLockscree * KuiLockscree::create(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiLockscree * popLayer = KuiLockscree::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiLockscree::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne);   //禁止多点触摸
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");

	//添加模态背景
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	/*ccColor4B color = ccc4(112, 128, 144, 150);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	//colorLayer->setContentSize(winSize);//设置和窗口的大小
	this->addChild(colorLayer);
	*/
	ccColor4B color = ccc4(112, 128, 144, 0);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(visibleSize);//设置和窗口的大小
	this->addChild(colorLayer);

	ParentNode_ItemEx = colorLayer;
	//获得背景图片的大小
	CCSize contentSize = ParentNode_ItemEx->getContentSize();
	m_size = contentSize;
	m_origin = ParentNode_ItemEx->getPosition();
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	
	return true;
}

void KuiLockscree::addDialogData()
{
	CCSize nSize;
	nSize.width =128;
	nSize.height=24;
	m_StartPos.x =10;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =140-24;//+ITEM_CELL_SIZE/2;

	m_pUiLayer = UILayer::create();
	ParentNode_ItemEx->addChild(m_pUiLayer);

	//增加启动　保存按钮
	UIButton* button = UIButton::create();
	button->setTouchEnabled(true);
	button->setScale9Enabled(true);
	button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	button->setTitleFontSize(14);
	button->setTitleText(UTEXT("锁住屏幕中,点击确认解锁",1).c_str());
	button->setPosition(ccp(m_size.width/2,60));
	button->setTag(1);
	button->setTagbyKey("1");
	button->addTouchEventListener(this,toucheventselector(KuiLockscree::oktouchEvent));
	m_pUiLayer->addWidget(button);
	
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\特殊对话条\\输入文字条－确定.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	CCPoint nStartPos;
	nStartPos.x = m_size.width/2+20;
	nStartPos.y = m_size.height/2;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_ok_btn_confirm_normal =  CCSprite::createWithTexture(bgCur);//CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_ok_btn_confirm_select =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * okConfirm =NULL;

	okConfirm = CCMenuItemSprite::create(sprite_ok_btn_confirm_normal, sprite_ok_btn_confirm_select, this, menu_selector(KuiLockscree::buttonCallBackFunc));
	okConfirm->setPosition(ccp(-28,11));
	//关闭
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\特殊对话条\\输入文字条－取消.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_select = CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * closeConfirm = CCMenuItemSprite::create(sprite_close_normal, sprite_close_select, this, menu_selector(KuiLockscree::buttonCallBackFunc));
	closeConfirm->setPosition(ccp(28,11));

	//托管
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\买卖\\摆摊\\托管.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_tuo_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_tuo_select =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * tuoConfirm = CCMenuItemSprite::create(sprite_tuo_normal, sprite_tuo_select, this, menu_selector(KuiLockscree::tuoguanCallBackFunc));
	tuoConfirm->setPosition(ccp(0,34));
	//CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	CCMenu * menu = CCMenu::create(okConfirm,closeConfirm,tuoConfirm,NULL);
	menu->setPosition(ccp(m_size.width/2,3));  //菜单位置设置为弹出框左下	
	ParentNode_ItemEx->addChild(menu);         //加到父层里
}

void KuiLockscree::tuoguanCallBackFunc(CCObject * pSender)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_LIXIAN, 0, 0);  //发送离线请求
}

//点击按钮回调函数
void KuiLockscree::oktouchEvent(CCObject *pSender, TouchEventType type)
{
	if  (!g_pCoreShell || !g_GameWorld) return;

	switch (type)
	{
	case TOUCH_EVENT_BEGAN://Touch Down
		{
			if (pSender)
			{
				UIButton* nTbtn = (UIButton*)pSender;
				nTbtn->setTitleColor(ccYELLOW);
				buttonCallBackFunc(NULL);
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
			}
		}
		break;
	case TOUCH_EVENT_CANCELED:
		break;

	default:
		break;
	}
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiLockscree::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

}
//执行上层对象的回调函数，关闭弹出框
void KuiLockscree::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiLockscree::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiLockscree::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128,true);
}

CCRect KuiLockscree::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiLockscree::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	//case SMCT_UI_RENASCENCE:	             //选择重生	
    //buttonCallBackFunc(NULL);
	return true;
}

void KuiLockscree::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //	m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiLockscree::draw()
{
	//CCMessageBox("无限循环","draw");
}


int KuiLockscree::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
{
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_ENTER);          //禁止回车代码
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR);          //禁止颜色代码
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR_RESTORE);  //禁止颜色复原
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_COLOR);   //禁止描边设置
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_RESTORE); //禁止描边设置
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_INLINE_PIC);     //禁止内图设置
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_LINK);           //禁止连接设置
	//	return TEncodeText(pMsgBuff, nMsgLength);
	return  nMsgLength;
}