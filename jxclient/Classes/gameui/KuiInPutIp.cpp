//有按钮的　ｔａｌｋ　对话框
//
#include "gamecore/KCore.h"
#include "gameui/Uiglobaldata.h"
#ifndef WIN32
#include <sys/select.h>
#include <ctype.h>
//typedef struct fd_set FD_SET;
#endif
#include "network/Socket.h"

#include "engine/KbugInfo.h"
#include "engine/CXmlStream.h"
#include "KuiInPutIp.h"
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

KuiInPutIp::KuiInPutIp()
{
	isOpen = false;
	colorLayer        = NULL;
	red               = NULL;
	AccNode = NULL;
	PasNode = NULL;
	m_AnswerCount = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	s = INVALID_SOCKET;
	nport=INVALID_SOCKET;
	naccIp="";
}

KuiInPutIp::~KuiInPutIp()
{
	isOpen = false;
	colorLayer        = NULL;
}

//创建确定弹出框
KuiInPutIp * KuiInPutIp::create(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiInPutIp * popLayer = KuiInPutIp::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiInPutIp::init()
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
	CCSprite * spriteBox = CCSprite::create("temp/inputip.png");
	ParentNode_ItemEx = spriteBox;
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2-spriteBox->getContentSize().height/2));

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(visibleSize);//设置和窗口的大小
	colorLayer->addChild(spriteBox);
	this->addChild(colorLayer);
	
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	
	return true;
}

void KuiInPutIp::addDialogData()
{
	CCSize nSize;
	nSize.width =128;
	nSize.height=24;
	m_StartPos.x =10;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =140-24;//+ITEM_CELL_SIZE/2;
	//char nTempStr[128];
	//t_sprintf(nTempStr,pContent);
	/*CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setPosition(ccp(128/2+30,m_size.height-50));
	ParentNode_ItemEx->addChild(pstrLabel);*/

	/*CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT("点击屏幕回程",1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(m_size.width/2,20));
	ParentNode_ItemEx->addChild(ptextLabel);*/

	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
	CCScale9Sprite *pAccBg = CCScale9Sprite::create("loadingEnd.png");  //"boxgb.png"
	pAccBg->setOpacity(0);
	
	AccNode = CCEditBox::create(CCSize(200,24),pAccBg);
	const ccColor3B nfcolor ={255,255,255};
	if (AccNode)
	{
		//pEditBox->setContentSize()
		//AccNode->setText("input your account");
		AccNode->setPlaceHolder("Input Server Ip");
		AccNode->setFontColor(nfcolor);
		AccNode->setFontSize(24);
		AccNode->setMaxLength(64);
		AccNode->setInputFlag(kEditBoxInputFlagInitialCapsAllCharacters);
		AccNode->setInputMode(kEditBoxInputModeAny);
		AccNode->setReturnType(kKeyboardReturnTypeDone);
		//AccNode->setDelegate((CCEditBoxDelegate*)this);
		AccNode->setPosition(ccp(m_size.width/2+16,135));
		ParentNode_ItemEx->addChild(AccNode,5);
	}

	//密码输入框
	CCScale9Sprite *pPassBg = CCScale9Sprite::create("loadingEnd.png"); //"boxgb.png"
	pPassBg->setOpacity(0);
	PasNode = CCEditBox::create(CCSize(200,24),pPassBg);
	if (PasNode)
	{
		//pEditBox->setContentSize()
		PasNode->setPlaceHolder("Input Server Ip");
		//PasNode->setText("input your password");
		PasNode->setFontColor(nfcolor);
		PasNode->setFontSize(24);
		PasNode->setMaxLength(64);
		AccNode->setInputFlag(kEditBoxInputFlagInitialCapsAllCharacters);
		AccNode->setInputMode(kEditBoxInputModeAny);
		AccNode->setReturnType(kKeyboardReturnTypeDone);
		PasNode->setPosition(ccp(m_size.width/2+16,100));
		ParentNode_ItemEx->addChild(PasNode,5);
	}
	//
	CCMenuItemImage *pCancelItem=NULL;
	CCMenuItemImage *pLoginItem=NULL;
		pLoginItem = CCMenuItemImage::create(
			"temp/ok.png",
			"temp/ok.png",
			"temp/ok.png",this,menu_selector(KuiInPutIp::mianLoginCallback));
		pLoginItem->setPosition(ccp(origin.x + visibleSize.width/2 - pLoginItem->getContentSize().width/2-249,
			origin.y + pLoginItem->getContentSize().height/2+7));//一个按钮
		pLoginItem->setTag(1);
		pCancelItem = CCMenuItemImage::create(
			"temp/c.png",
			"temp/c.png",
			"temp/c.png",this,menu_selector(KuiInPutIp::mianCancelCallback));
		pCancelItem->setPosition(ccp(origin.x + visibleSize.width/2 - pCancelItem->getContentSize().width/2-99,
			origin.y + pCancelItem->getContentSize().height/2+7));//一个按钮
		pCancelItem->setTag(1);
	    CCMenu* pMenu=NULL;
		pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);
		pMenu->setPosition(CCPointZero);//
		ParentNode_ItemEx->addChild(pMenu,2);
	//获取登陆器的服务器的连接数据
	m_WritablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	
}

void KuiInPutIp::mianCancelCallback(CCObject* pSender)
{
	buttonCallBackFunc(pSender,2);
}

void KuiInPutIp::mianLoginCallback(CCObject* pSender)
{
	KIniFile pPakList;
	if (pPakList.Load("data/package_temp.ini",true))
	{
		std::string strAcc;
		std::string strPas;
		if (AccNode && PasNode)
		{
			strAcc  = AccNode->getText();
			strPas  = PasNode->getText();

			if  (strPas.empty() || strPas.empty())
			{
				CCMessageBox("Ip Is Empty","Error");
				pPakList.Clear();
				return;
			}
			if  (strAcc!=strPas)
			{
				CCMessageBox("Ip Is Error","Error");
				pPakList.Clear();
				return;
			}

			dataChecksum nurlMask;
			char donwPath[64]={0},nTempPaht[128];
			ZeroMemory(donwPath,sizeof(donwPath));
			ZeroMemory(nTempPaht,sizeof(nTempPaht));
			nurlMask.SimplyDecrypt(donwPath,CLIENT_IP);
			t_sprintf(nTempPaht,strAcc.c_str());
			if  (!strstr(nTempPaht,donwPath))
			{
				CCMessageBox("Ip Is Error","Error");
				pPakList.Clear();
				return;
			}

			pPakList.WriteString("Package","down",strAcc.c_str());
			pPakList.Save("data/package_temp.ini");
		}
		pPakList.Clear();
		buttonCallBackFunc(pSender,1);
	}
	else
     CCMessageBox("The Client Is Error","Error");
	
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiInPutIp::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

}
//执行上层对象的回调函数，关闭弹出框
void KuiInPutIp::buttonCallBackFunc(CCObject * pSender,int ver)
{
	CCNode * node = NULL;
	if (pSender)
	{
		node = dynamic_cast<CCNode *>(pSender);
		node->setTag(ver);
	}
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiInPutIp::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiInPutIp::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuiInPutIp::getRect(CCNode* pNode,int i)
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
bool KuiInPutIp::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	//case SMCT_UI_RENASCENCE:	             //选择重生	
    //buttonCallBackFunc(NULL);
	return true;
}

void KuiInPutIp::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //  m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiInPutIp::draw()
{
	//CCMessageBox("无限循环","draw");
}


int KuiInPutIp::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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

void KuiInPutIp::editBoxEditingDidBegin(CCEditBox *editBox)//开始时的函数
{
	CCMessageBox("点击开始","点击开始");
}
void KuiInPutIp::editBoxEditingDidEnd(CCEditBox *editBox) //结束时的函数
{

}
void KuiInPutIp::editBoxReturn(CCEditBox *editBox)         //输入完后点击回车时的处理
{

}
void KuiInPutIp::editBoxTextChanged(CCEditBox *editBox, const std::string &text)//文字更改函数
{

}