#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiTongCreateSheet.h"
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
KuiTongCreateSheet::KuiTongCreateSheet()
{
	isOpen = false;
	__isCanMove=true;
	m_bScrolling=false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	colorLayer        = NULL;
	red               = NULL;
	m_pMainUiLayer    = NULL;
	__nSelCamp        = -1;
	pNameEditBox      = NULL;
	btnBox_zp         = NULL;
	btnBox_zl         = NULL;
	btnBox_xp         = NULL;
	pCampInfoLabel    = NULL;
	pTongNameLabel    = NULL;
	pCampTitleLabel   = NULL;
}

KuiTongCreateSheet::~KuiTongCreateSheet()
{
	isOpen = false;
	colorLayer   = NULL;
	//CC_SAFE_RELEASE(m_array);
	if (red)
	    red->release();

}

//创建确定弹出框
KuiTongCreateSheet * KuiTongCreateSheet::create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiTongCreateSheet * popLayer = KuiTongCreateSheet::create();
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiTongCreateSheet::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");
	//添加模态背景
	winSize = CCDirector::sharedDirector()->getWinSize();
	/*ccColor4B color = ccc4(112, 128, 144, 150);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	//colorLayer->setContentSize(winSize);//设置和窗口的大小
	this->addChild(colorLayer);
	*/
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮会界面\\帮会信息修改底图.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return false;
	CCSprite * spriteBox = CCSprite::createWithTexture(bgCur);
	ParentNode_Task= spriteBox;
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

void KuiTongCreateSheet::addDialogData()
{
	if  (!g_pCoreShell) return;

	/*pMoneyLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//统帅
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(70,35));
	ParentNode_Task->addChild(pMoneyLabel,1);
*/
	m_pMainUiLayer = UILayer::create();
	ParentNode_Task->addChild(m_pMainUiLayer);



	pTongNameLabel =CCLabelTTF::create(UTEXT("帮会名称",1).c_str(),UI_GAME_FONT_SONG, 12);//Arial
	pTongNameLabel->setAnchorPoint(ccp(0,0));
	pTongNameLabel->setPosition(ccp(20,m_size.height-48));
	pTongNameLabel->setColor(ccYELLOW);
	ParentNode_Task->addChild(pTongNameLabel);

	pCampTitleLabel =CCLabelTTF::create(UTEXT("帮会阵营",1).c_str(),UI_GAME_FONT_SONG, 12);//Arial
	pCampTitleLabel->setAnchorPoint(ccp(0,0));
	pCampTitleLabel->setPosition(ccp(20,m_size.height-73));
	pCampTitleLabel->setColor(ccYELLOW);
	ParentNode_Task->addChild(pCampTitleLabel);

	pCampInfoLabel =CCLabelTTF::create(UTEXT("阵营说明",1).c_str(),UI_GAME_FONT_SONG, 12);//Arial
	pCampInfoLabel->setAnchorPoint(ccp(0,0));
	pCampInfoLabel->setPosition(ccp(20,m_size.height-105));
	pCampInfoLabel->setColor(ccYELLOW);
	ParentNode_Task->addChild(pCampInfoLabel);


	// Create the checkbox
	/*char nSprName[128]={0};
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮会界面\\帮会信息修改底图.spr");
	g_StrLower(nSprName);
    
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	UIImageView *titleImg = UIImageView::create();
	titleImg->loadTextureByData(bgCur);
	titleImg->setPosition(ccp(m_size.width/2,m_size.height/2-15));
	m_pMainUiLayer->addWidget(titleImg);
	*/

	CCScale9Sprite *pBg;
	pBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pNameEditBox = CCEditBox::create(CCSize(90,24),pBg);
	const ccColor3B nfcolor ={255,255,255};
	if (pNameEditBox)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pNameEditBox->setPlaceHolder(UTEXT("tong name",1).c_str());
		//pNameEditBox->setFontColor(ccYELLOW);
		pNameEditBox->setFontSize(24);
		pNameEditBox->setMaxLength(24);
		pNameEditBox->setPosition(ccp(pTongNameLabel->getPosition().x+pTongNameLabel->getContentSize().width+50,m_size.height-40));
		pNameEditBox->setInputMode(kEditBoxInputModeAny);
		pNameEditBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		m_pMainUiLayer->addChild(pNameEditBox);
	}

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);  //设置和窗口的大小
	ParentNode_Task->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiTongCreateSheet::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	
	char nSprName[128]={0};
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	//正派
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮派\\帮派创建-正派.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnZp_normal = CCSprite::createWithTexture(bgCur);
	sprite_btnZp_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnZp_select = CCSprite::createWithTexture(bgCur);
	sprite_btnZp_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * btnZpConfirm = CCMenuItemSprite::create(sprite_btnZp_normal, sprite_btnZp_select, this, menu_selector(KuiTongCreateSheet::btnCallBackFunc));
	btnZpConfirm->setPosition(ccp(pCampTitleLabel->getPosition().x+pCampTitleLabel->getContentSize().width+15,m_size.height-65));
	btnZpConfirm->setTag(3);

	//邪派
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮派\\帮派创建-邪派.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnXp_normal = CCSprite::createWithTexture(bgCur);
	sprite_btnXp_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnXp_select = CCSprite::createWithTexture(bgCur);
	sprite_btnXp_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * btnXpConfirm = CCMenuItemSprite::create(sprite_btnXp_normal, sprite_btnXp_select, this, menu_selector(KuiTongCreateSheet::btnCallBackFunc));
	btnXpConfirm->setPosition(ccp(btnZpConfirm->getPosition().x+btnZpConfirm->getContentSize().width+5,m_size.height-65));
	btnXpConfirm->setTag(4);

	//中立
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮派\\帮派创建-中立.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnZl_normal = CCSprite::createWithTexture(bgCur);
	sprite_btnZl_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_btnZl_select = CCSprite::createWithTexture(bgCur);
	sprite_btnZl_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * btnZlConfirm = CCMenuItemSprite::create(sprite_btnZl_normal, sprite_btnZl_select, this, menu_selector(KuiTongCreateSheet::btnCallBackFunc));
	btnZlConfirm->setPosition(ccp(btnXpConfirm->getPosition().x+btnXpConfirm->getContentSize().width+5,m_size.height-65));
	btnZlConfirm->setTag(5);


	//确认
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮派\\帮派创建-确定.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_Ok_normal = CCSprite::createWithTexture(bgCur);
	sprite_Ok_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,2,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_Ok_select = CCSprite::createWithTexture(bgCur);
	sprite_Ok_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * OkConfirm = CCMenuItemSprite::create(sprite_Ok_normal, sprite_Ok_select, this, menu_selector(KuiTongCreateSheet::btnCallBackFunc));
	OkConfirm->setPosition(ccp(48,m_size.height-130));
	OkConfirm->setTag(1);

	//关闭
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\帮派\\帮派创建-取消.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_Leave_normal = CCSprite::createWithTexture(bgCur);
	sprite_Leave_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,2,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_Leave_select = CCSprite::createWithTexture(bgCur);
	sprite_Leave_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * LeaveConfirm = CCMenuItemSprite::create(sprite_Leave_normal, sprite_Leave_select, this, menu_selector(KuiTongCreateSheet::btnCallBackFunc));
	LeaveConfirm->setPosition(ccp(134,m_size.height-130));
	LeaveConfirm->setTag(2);
	//加入菜单
	CCMenu * menu = CCMenu::create(OkConfirm,LeaveConfirm,btnZpConfirm,btnXpConfirm,btnZlConfirm,NULL);
	menu->setAnchorPoint(ccp(0,0));
	menu->setPosition(ccp(0,0));             //菜单位置设置为弹出框左下
	ParentNode_Task->addChild(menu);         //加到父层里


	KUiPlayerTeam mTeam;
	ZeroMemory(&mTeam,sizeof(mTeam));
	if (g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&mTeam, 0)) //没有队伍 就创建一支队伍
	{//有队伍
	}
	else
	{//没有队伍
	}
}

void KuiTongCreateSheet::btnCallBackFunc(CCObject * pSender)
{
	if (!g_pCoreShell|| !g_GameWorld) return;

	CCNode * node = NULL;
	if (pSender)
	{
		node= dynamic_cast<CCNode *>(pSender);
		switch(node->getTag())
		{
		case 1://确认 
			{
				char Buff[32],nTempBuff[32];
				ZeroMemory(Buff,sizeof(Buff));
				ZeroMemory(nTempBuff,sizeof(nTempBuff));
				t_sprintf(nTempBuff,pNameEditBox->getText());
				t_sprintf(Buff,U2G(nTempBuff).c_str());
				if(Buff[0] && g_pCoreShell)
				{
					if(__nSelCamp != -1){
						g_pCoreShell->TongOperation(GTOI_TONG_CREATE, 0, (int)__nSelCamp,0,Buff);
						buttonCallBackFunc(NULL);
					}
					else
						pCampInfoLabel->setString(UTEXT("请选择帮派所属阵营",1).c_str());
				}
				else
					pCampInfoLabel->setString(UTEXT("请输入帮派名字",1).c_str());
			}
			break;
		case 2://取消
			buttonCallBackFunc(NULL);
			break;
		case 3:
			if (pCampInfoLabel)
				pCampInfoLabel->setString(UTEXT("正在建立正派",1).c_str());

			__nSelCamp = camp_justice;
			break;
		case 4:
			if (pCampInfoLabel)
				pCampInfoLabel->setString(UTEXT("正在建立邪派",1).c_str());
			__nSelCamp = camp_evil;
			break;
		case 5:
			if (pCampInfoLabel)
				pCampInfoLabel->setString(UTEXT("正在建立中立派",1).c_str());

			__nSelCamp = camp_balance;
			break;
		default:
			break;
		}
	}
}
//执行上层对象的回调函数，关闭弹出框
void KuiTongCreateSheet::buttonCallBackFunc(CCObject * pSender)
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
void KuiTongCreateSheet::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiTongCreateSheet::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,128, true);
}

CCRect KuiTongCreateSheet::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}

void KuiTongCreateSheet::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	m_bScrolling = false;

	//__loopTime = MAX_LOOP_TIME;
}

//移动中
void KuiTongCreateSheet::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling || !__isCanMove) return;
	CCPoint point   = ParentNode_Task->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCPoint diff    = ccpSub(point,m_lastPoint);                  //设置图片的位置
	ParentNode_Task->setPosition(ccpAdd(ParentNode_Task->getPosition(),diff));
	m_origin      = ParentNode_Task->getPosition();
	m_lastPoint = point;
}
//触摸函数ccTouchBegan，返回true
bool KuiTongCreateSheet::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint point = ParentNode_Task->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, winSize.width/*ParentNode_Task->getContentSize().width*/,winSize.height/* ParentNode_Task->getContentSize().height*/);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;
	return true;
}

//--------------------------------------------------------------------------
//	功能：更新队伍信息
//--------------------------------------------------------------------------
void KuiTongCreateSheet::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	}
}

void KuiTongCreateSheet::draw()
{
	//CCMessageBox("无限循环","draw");
}