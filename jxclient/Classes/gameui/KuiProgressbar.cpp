//有按钮的　ｔａｌｋ　对话框
//
#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiProgressbar.h"
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
#include "gamecore/KSkillManager.h"
#include "gamecore/KItem.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiProgressbar::KuiProgressbar()
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
	ZeroMemory(&proData,sizeof(nJinDuClient));
	proSpr = NULL;
	starttime = 0;
}

KuiProgressbar::~KuiProgressbar()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
	colorLayer        = NULL;
	//if (red)
	  //  red->release();
	//red               = NULL;
}

//创建确定弹出框
KuiProgressbar * KuiProgressbar::create(KUiJinDuTiaoInfo* pJinDu,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	if (!pJinDu)
		return NULL;

	KuiProgressbar * popLayer = KuiProgressbar::create();
	popLayer->proData.nMsgidx = pJinDu->nMsgIndex;

	popLayer->proData.nTimeVal= pJinDu->nTimes;//总时间
	if (pJinDu->nTimes<=0)
       popLayer->proData.nTimeVal =10;

	t_sprintf(popLayer->ntitle,pJinDu->nTitle);
	t_sprintf(popLayer->proData.szCallFun,pJinDu->nCallFuc);
	t_sprintf(popLayer->proData.szParma,pJinDu->szParma);
	t_sprintf(popLayer->proData.szParmb,pJinDu->szParmb);
	t_sprintf(popLayer->proData.szParmc,pJinDu->szParmc);
	popLayer->addDialogData();
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}


//初始化
bool KuiProgressbar::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\progressbar\\progressbar_back.spr");
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
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}


void KuiProgressbar::addDialogData()
{
	if  (!g_pCoreShell) return;
	CCSize nSize;
	nSize.width =128;
	nSize.height=24;
	m_StartPos.x =10;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =140-24;//+ITEM_CELL_SIZE/2;
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\progressbar\\progressbar_bit.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	
	if (bgCur==NULL)
		return;

	proSpr = CCSprite::createWithTexture(bgCur);
	proSpr->setAnchorPoint(ccp(0,0));
	proSpr->setPosition(ccp(10,7));
	proSpr->setScaleX(0); //设置缩放为０;
	ParentNode_ItemEx->addChild(proSpr);

	CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT(ntitle,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(m_size.width/2,m_size.height-15));
	ParentNode_ItemEx->addChild(ptextLabel);

	/*cc_timeval m_pStartUpdate;
	CCTime::gettimeofdayCocos2d(&m_pStartUpdate,NULL);
	starttime = m_pStartUpdate.tv_sec*1000+m_pStartUpdate.tv_usec/1000;//毫秒timeGetTime();获得系统运行的时间
	*/
	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);
	
	starttime = pInfo.npTime;  //记录打开窗口的秒数
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
void KuiProgressbar::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
}
//执行上层对象的回调函数，关闭弹出框
void KuiProgressbar::buttonCallBackFunc(CCObject * pSender)
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
void KuiProgressbar::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiProgressbar::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
}

CCRect KuiProgressbar::getRect(CCNode* pNode,int i)
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
bool KuiProgressbar::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	//case SMCT_UI_RENASCENCE:	//选择重生	
	//	if (g_pCoreShell)
	//	   g_pCoreShell->OperationRequest(GOI_PLAYER_RENASCENCE,0,true);//nSelAction=0：原地重生, nSelAction=1：回城

    buttonCallBackFunc(NULL);
	return true;
}

void KuiProgressbar::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell && proSpr)
	{
		KUiPlayerGameTime	pInfo;
		memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
		ZeroMemory(&pInfo,sizeof(pInfo));
		if (g_pCoreShell)
			g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);
	   float ncTime=0;
	      ncTime=pInfo.npTime-starttime;	//时间差

       if (ncTime>0 && ncTime<=proData.nTimeVal)
	   {
		 float sX = ncTime/proData.nTimeVal;
		 proSpr->setScaleX(sX);
	     //m_Progress.Set2IntValue(ncTime,proData.nTimeVal);//设置进度
		 if (ncTime==proData.nTimeVal)
		 {//开始发送回调函数执行脚本
			 if (atoi(proData.szParmc)==1) 
			 {//是否循环执行--钓鱼
				 starttime=pInfo.npTime;        //记录执行的时间
				 g_pCoreShell->OperationRequest(GOI_JINDUTIAO_CALLBACK,(unsigned int)&proData,0);
				 proSpr->setScaleX(0);
				 //m_pSelf->m_Progress.Set2IntValue(0,proData.nTimeVal); //设置进度
			 }
             else
			 {//一次行 采集
			     starttime=pInfo.npTime; 
				 g_pCoreShell->OperationRequest(GOI_JINDUTIAO_CALLBACK,(unsigned int)&proData,0);
			     buttonCallBackFunc(NULL);
			 } 
		 }
	   }
	}
}

void KuiProgressbar::draw()
{
	//CCMessageBox("无限循环","draw");
}


int KuiProgressbar::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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