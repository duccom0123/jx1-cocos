#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuigetMoney.h"
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
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuigetMoney::KuigetMoney()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	//pMoneyLabel       = NULL;
	//pXuLabel          = NULL;
	colorLayer        = NULL;
	//red               = NULL;
	pMoneyEditBox     = NULL;
	//m_nMoney = 0;
	//m_nXu = 0;
	//m_nNumObjects = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	nKind = 0;
	seveMoney = 0;
}

KuigetMoney::~KuigetMoney()
{
	isOpen = false;
	colorLayer = NULL;
	nKind = 0;
}

//创建确定弹出框
KuigetMoney * KuigetMoney::create(int nMoney,int nMaxMoney,int nKind,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuigetMoney * popLayer = KuigetMoney::create();

	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	popLayer->nKind = nKind;
	return popLayer;
}

//初始化
bool KuigetMoney::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\特殊对话条\\取钱条.spr");
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

void KuigetMoney::addDialogData()
{
	if  (!g_pCoreShell) return;

	//Left=24
	//Top=72
	m_StartPos.x =24;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =72;//+ITEM_CELL_SIZE/2;

    /*pMoneyLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//Arial
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(60,128));
	ParentNode_ItemEx->addChild(pMoneyLabel,1);

	pXuLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//Arial
	pXuLabel->setAnchorPoint(ccp(0,0));
	pXuLabel->setPosition(ccp(60,105));
	ParentNode_ItemEx->addChild(pXuLabel,1);*/

	CCScale9Sprite *pBg = CCScale9Sprite::create(); 
	pMoneyEditBox = CCEditBox::create(CCSize(112,24),pBg);
	const ccColor3B nfcolor ={255,255,255};
	if (pMoneyEditBox)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pMoneyEditBox->setPlaceHolder("input money");
		pMoneyEditBox->setFontColor(nfcolor);
		pMoneyEditBox->setFontSize(24);
		pMoneyEditBox->setMaxLength(9);
		pMoneyEditBox->setPosition(ccp(57,50));
		pMoneyEditBox->setInputMode(kEditBoxInputModeNumeric);
		pMoneyEditBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		ParentNode_ItemEx->addChild(pMoneyEditBox,1,500);
	}

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_ItemEx->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuigetMoney::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

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
	CCMenuItemSprite * okConfirm = CCMenuItemSprite::create(sprite_ok_btn_confirm_normal, sprite_ok_btn_confirm_select, this, menu_selector(KuigetMoney::getmoneyCallBackFunc));
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
	CCMenuItemSprite * closeConfirm = CCMenuItemSprite::create(sprite_close_normal, sprite_close_select, this, menu_selector(KuigetMoney::buttonCallBackFunc));
	closeConfirm->setPosition(ccp(28,11));


	CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	CCMenu * menu = CCMenu::create(okConfirm,closeConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//菜单位置设置为弹出框左下
	menu->setPosition(ccp(m_size.width/2,3));//菜单位置设置为弹出框左下
	ParentNode_ItemEx->addChild(menu);         //加到父层里
}

void KuigetMoney::getmoneyCallBackFunc(CCObject * pSender)
{
	std::string nCurMoney="";
	if (pMoneyEditBox)
	{
		nCurMoney = pMoneyEditBox->getText();
		if (nCurMoney.length()>10 || nCurMoney.length()<= 0)
		{
#ifdef WIN32
			CCMessageBox("不能为空并长度不能大于10","警告");
#else
			CCMessageBox(G2U("不能为空并长度不能大于10").c_str(),G2U("警告").c_str());
#endif
			return;
		}
		//开始检测字符的合法性
		std::string strcheck="1";
		strcheck+=nCurMoney;
		int count = 0,spaceNum=0,chineseNum=0,other=0,letter=0,xiaoxie=0;
		for(int i = 0; i <strcheck.length();i++)
		{ 
			//isalnum
			if(isdigit(strcheck[i]))     //判断是否是数字0-9
				count++;
			else if(isspace(strcheck[i]))//判断是否是空格
				++spaceNum;
			else if(isalpha(strcheck[i])==1)//判断是否是字母	 1 为大写 2 为 小写  0 为不是字母
			{
				++letter;
			}
			else if(isalpha(strcheck[i])==2)//判断是否是字母	 1 为大写 2 为 小写  0 为不是字母
			{
				++xiaoxie;
			}
			else if(strcheck[i]&0x80)//if(strcheck[0]>=0xB0)
				++chineseNum;        //判断是否是中文
			else
			{
				++other;
			}
		} 

		if (spaceNum>0 || chineseNum>0 || xiaoxie >0 || other>0 || letter>0)
		{ 	//system("pause");
#ifdef WIN32
			CCMessageBox("只能填写数字!","警告");
#else
			CCMessageBox(G2U("只能填写数字!").c_str(),G2U("警告").c_str());
#endif
			return;
		} 

	}
	else
	{
#ifdef WIN32
		CCMessageBox("数据有误．．","警告");
#else
		CCMessageBox(G2U("数据有误．．").c_str(),G2U("警告").c_str());
#endif
		return;
	}
    //开始存取钱数据
	unsigned int nMoney=0;
	char nstrInfo[64];
	ZeroMemory(nstrInfo,sizeof(nstrInfo));
	t_sprintf(nstrInfo,nCurMoney.c_str());
	nMoney = g_Atoui(nstrInfo);
	if (nMoney > 0)
	{
		if (nKind)
		{//0 为取钱 获取储存箱的钱
			g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,true,nMoney);
		}
		else
		{// 1 存钱
			g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,false,nMoney);
		}
		buttonCallBackFunc(NULL);
	}
}
//执行上层对象的回调函数，关闭弹出框
void KuigetMoney::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node =NULL;
	if (pSender)
	   node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);
	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuigetMoney::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


//重写触摸注册函数，重新给定触摸级别
void KuigetMoney::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuigetMoney::getRect(CCNode* pNode,int i)
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
bool KuigetMoney::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}

void KuigetMoney::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	    /*m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
		char nPointCount[32];
	   if (pMoneyLabel)
	   {
		   t_sprintf(nPointCount,"%d",m_nMoney);
		  // pSkillPointLabel->setColor(ccWit)
		   pMoneyLabel->setString(nPointCount);
	   }
	   if  (pXuLabel)
	   {
		   t_sprintf(nPointCount,"%d",m_nXu);
		   // pSkillPointLabel->setColor(ccWit)
		   pXuLabel->setString(nPointCount);
	   }*/
	}
}

void KuigetMoney::draw()
{
	//CCMessageBox("无限循环","draw");
}