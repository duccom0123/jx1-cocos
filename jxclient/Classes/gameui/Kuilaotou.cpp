//有按钮的　ｔａｌｋ　对话框
//
#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "Kuilaotou.h"
#include "engine/Text.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
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
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

Kuilaotou::Kuilaotou()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	//pMoneyLabel       = NULL;
	//pXuLabel          = NULL;
	colorLayer        = NULL;
	red               = NULL;

	//m_nMoney = 0;
	//m_nXu = 0;
	//m_AnswerCount = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
}

Kuilaotou::~Kuilaotou()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
	colorLayer        = NULL;
	//if (red)
	  //  red->release();
	//red               = NULL;
}

//创建确定弹出框
Kuilaotou * Kuilaotou::create(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	Kuilaotou * popLayer = Kuilaotou::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}


//初始化
bool Kuilaotou::init()
{
	if ( !CCLayer::init() ){
		return false;
	}

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
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\新任务系统\\相关面板\\奖励面板.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return false;
	CCSprite * spriteBox = CCSprite::createWithTexture(bgCur);
	ParentNode_ItemEx = spriteBox;
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2));
	this->addChild(spriteBox);
	
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}


void Kuilaotou::addDialogData()
{
	if  (!g_pCoreShell || !g_GameWorld ||!nisgetinfo) return;

	CCSize nSize;
	nSize.width =350;
	nSize.height=140;
	m_StartPos.x =10;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =140-24;//+ITEM_CELL_SIZE/2;
	char nTempStr[128];
	KIniFile strInfo;

	if  (strInfo.Load("\\ui\\ui3\\奖励界面.ini"))
	{
		strInfo.GetString("StringQuest", "GetGift","", nTempStr, sizeof(nTempStr));
		strInfo.Clear();
	}
	//t_sprintf(nTempStr,pContent);
	CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setPosition(ccp(nSize.width/2+30,m_size.height-100));
	ParentNode_ItemEx->addChild(pstrLabel);


	/*CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT("点击屏幕回程",1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(m_size.width/2,20));
	ParentNode_ItemEx->addChild(ptextLabel);*/

	/*ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_ItemEx->addChild(colorLayer,1000);
	*/
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}


//设置确定取消按钮,参数：调用层对象，调用层回调函数
void Kuilaotou::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	if  (!g_pCoreShell || !g_GameWorld ||!nisgetinfo) return;

	m_callbackListener = callbackListener;
	m_callfun = callfun;

	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Quest\\ExpBtn.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	CCPoint nStartPos;
	nStartPos.x = m_size.width/2+20;
	nStartPos.y = m_size.height/2;
	//经验
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_ExpBtn_normal =  CCSprite::createWithTexture(bgCur);//CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	CCMenuItemSprite * ExpBtnConfirm = CCMenuItemSprite::create(sprite_ExpBtn_normal,sprite_ExpBtn_normal, this, menu_selector(Kuilaotou::btnCallBackFunc));
	ExpBtnConfirm->setPosition(ccp(-125,11));
	ExpBtnConfirm->setTag(1);
	//随机
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Quest\\Random.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_Random_normal =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * RandomConfirm = CCMenuItemSprite::create(sprite_Random_normal, sprite_Random_normal, this, menu_selector(Kuilaotou::btnCallBackFunc));
	RandomConfirm->setPosition(ccp(-4,11));
	RandomConfirm->setTag(2);
	//银两
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Quest\\Money.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_Money_normal =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * MoneyConfirm = CCMenuItemSprite::create(sprite_Money_normal,sprite_Money_normal, this, menu_selector(Kuilaotou::btnCallBackFunc));
	MoneyConfirm->setPosition(ccp(125,11));
	MoneyConfirm->setTag(3);
	CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	CCMenu * menu = CCMenu::create(ExpBtnConfirm,RandomConfirm,MoneyConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//菜单位置设置为弹出框左下
	menu->setPosition(ccp(m_size.width/2,58)); //菜单位置设置为弹出框左下
	ParentNode_ItemEx->addChild(menu);         //加到父层里
}

void Kuilaotou::btnCallBackFunc(CCObject * pSender)
{
	if  (!g_pCoreShell || !g_GameWorld ||!nisgetinfo)
	{
		buttonCallBackFunc(NULL);
		return;
	}

	if (pSender)
	{
		CCNode * node = NULL;
			node = dynamic_cast<CCNode *>(pSender);
		int inselindex = node->getTag();
		if  (inselindex>=1 && inselindex<=3)
		    g_pCoreShell->OperationRequest(GOI_DATAU,inselindex,0);
	}
	buttonCallBackFunc(NULL);
}
//执行上层对象的回调函数，关闭弹出框
void Kuilaotou::buttonCallBackFunc(CCObject * pSender)
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
void Kuilaotou::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void Kuilaotou::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128,true);
}

CCRect Kuilaotou::getRect(CCNode* pNode,int i)
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
bool Kuilaotou::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	//case SMCT_UI_RENASCENCE:	//选择重生	
		//if (g_pCoreShell)
		//   g_pCoreShell->OperationRequest(GOI_PLAYER_RENASCENCE,0,true);//nSelAction=0：原地重生, nSelAction=1：回城	
    buttonCallBackFunc(NULL);
	return true;
}

void Kuilaotou::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	}
}

void Kuilaotou::draw()
{
	//CCMessageBox("无限循环","draw");
}


int Kuilaotou::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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