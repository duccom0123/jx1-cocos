//有按钮的　ｔａｌｋ　对话框
//
#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiPlayerTrade.h"
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
#include "gameui/KuiItemdesc.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

#define	STR_WAIT_TRADING "等待交易执行"
#define	STR_OTHER_OK	 "对方已锁定"
#define	STR_OTHER_NOT_OK "等待对方锁定"
#define	SEND_MONEY_CHANGE_MSG_DELAY		16

KuiPlayerTrade::KuiPlayerTrade()
{
	isOpen = false;
	colorLayer        = NULL;
	//red               = NULL;
	ParentNode_Item   = NULL;
	ParentNode_Give   = NULL; 
	ParentNode_tarGive= NULL;
	ptradeInfo        = NULL;
	m_StartPos.x=0;
	m_StartPos.y=0;
	m_StartPos_g.x = 0;
	m_StartPos_g.y = 0;
	m_StartPos_t.x = 0;
	m_StartPos_t.y = 0;
	//btnBackPic = NULL;
	m_nItemNumObjects = 0;
	m_ItemData.clear();
	m_giveItemData.clear();
	m_targiveItemData.clear();
	m_nIstrue = false;
	nstrCurSelItemKey = "";
	inCurSelItemKey   = -1;
	m_descPadIsOpen = false;
	m_targiveNum = 0;
	m_giveNum = 0;
	m_itemNum = 0;
	m_nNeed   = 0;
	nBakChickTime = 0;
	m_nMoney =0;
	//m_nXu    =0;
	m_bAdjustingMoney= 0;
	m_nTakewithMoney= 0;
	m_nSelfTradeMoney = 0;
	m_OtherMoney = NULL;
	m_SelfMoneyBox = NULL;
	okConfirm = NULL;
	tradeConfirm= NULL;
	__IsLock = false;
}

KuiPlayerTrade::~KuiPlayerTrade()
{
	isOpen = false;
	colorLayer = NULL;
	//if (red)
	//	red->release();

	m_ItemData.clear();
	m_giveItemData.clear();
	m_targiveItemData.clear();
}

//创建确定弹出框
KuiPlayerTrade * KuiPlayerTrade::create(KUiPlayerItem* pOther,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiPlayerTrade * popLayer = KuiPlayerTrade::create();

	if (pOther)
	   popLayer->m_OtherData = *pOther;

	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;

	//popLayer->m_nNeed = nNum;  //是执行脚本类型 kind  还是不执行脚本
	//t_sprintf(popLayer->m_Callback,"%s",pszCallback);
	//t_sprintf(popLayer->m_Error,"%s",pszError);
	return popLayer;
}


//初始化
bool KuiPlayerTrade::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\交易\\玩家交易.spr");
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
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size   = contentSize;
	m_origin = spriteBox->getPosition();
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2+20)); //-m_size.width/2+50
	this->addChild(spriteBox);
	
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne);   //禁止多点触摸
	return true;
}

void KuiPlayerTrade::addDialogData()
{
	if  (!g_pCoreShell) return;
	CCSize nSize;
	nSize.width =150;
	nSize.height=100;

	m_StartPos.x =m_size.width-28*6-10;    //+ITEM_CELL_SIZE/2;
	m_StartPos.y =40;//+ITEM_CELL_SIZE/2;

	m_StartPos_g.x =9;
	m_StartPos_g.y =210;

	m_StartPos_t.x =9;
	m_StartPos_t.y =55;
	
	char nTempStr[64];
	ParentNode_Give =CCNode::create();
	ParentNode_Item =CCNode::create();
	ParentNode_tarGive =CCNode::create();
	ParentNode_ItemEx->addChild(ParentNode_Item);
	ParentNode_ItemEx->addChild(ParentNode_Give);
	ParentNode_ItemEx->addChild(ParentNode_tarGive);
	
	t_sprintf(nTempStr,m_OtherData.Name);
	CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(50,m_size.height-48));
	ParentNode_ItemEx->addChild(ptextLabel);
	/*t_sprintf(nTempStr,pContent);
	CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,12,nSize,kCCTextAlignmentLeft);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setPosition(ccp(128/2+40,m_size.height-143));
	ParentNode_ItemEx->addChild(pstrLabel);

	char ntitleStr[64];
	t_sprintf(ntitleStr,strTitel);
	CCLabelTTF *ptitLabel = CCLabelTTF::create(UTEXT(ntitleStr,1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	ptitLabel->setPosition(ccp(m_size.width/2,m_size.height-80));
	ParentNode_ItemEx->addChild(ptitLabel);
	*/
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));

	pMoneyLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//Arial
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(m_size.width/2+70,43));
	ParentNode_ItemEx->addChild(pMoneyLabel,1);

	char nPointCount[32];
	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	m_nTakewithMoney = m_nMoney;
	//m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	if (pMoneyLabel)
	{
		t_sprintf(nPointCount,"%d",m_nMoney);
		pMoneyLabel->setString(nPointCount);
	}

	KUiObjAtRegion* pObjs = NULL;
	int nCount   = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nItemNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; ++i)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId    = pObjs[i].Obj.uId;
			no.DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
			no.DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
			no.DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
			no.DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
			if (Item && pObjs[i].Obj.uId>0 && pObjs[i].Obj.uId<MAX_ITEM)
			{
				/*m_ItemData[i].uGenre = pObjs[i].Obj.uGenre;
				m_ItemData[i].uId    = pObjs[i].Obj.uId;
				m_ItemData[i].nindex = i;
				m_ItemData[i].DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
				m_ItemData[i].DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
				m_ItemData[i].DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
				m_ItemData[i].DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
				m_ItemData[i].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带
				*/
				char nItemSprPath[256];
				ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
				if  (Item[pObjs[i].Obj.uId].GetImagePath())
					t_sprintf(nItemSprPath,Item[pObjs[i].Obj.uId].GetImagePath());
				else
					t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");

				if  (nItemSprPath[0])
				{
					//t_sprintf(nItemSprPath,Item[pObjs[i].Obj.uId].GetImagePath());
					g_StrLower(nItemSprPath);
					int m_nWidth,m_nHeight,nFrams;
					CCTexture2D *bgCur = NULL;
					SPRFRAMSINFO nSprInfo;
					ZeroMemory(&nSprInfo,sizeof(nSprInfo));
					bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
					if (bgCur==NULL)
						return;
					char nItemKey[32];
					t_sprintf(nItemKey,"item_%u",Item[pObjs[i].Obj.uId].GetID());
					std::string nKey =nItemKey;
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

						if  (Item[pObjs[i].Obj.uId].GetGenre()!=item_equip && Item[pObjs[i].Obj.uId].IsStack())
						{//不是装备 可叠加物品
							char stack[32];
							t_sprintf(stack,"%d",Item[pObjs[i].Obj.uId].GetStackNum());
							CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
							stuckCountlabel->setColor(ccYELLOW);
							stuckCountlabel->setAnchorPoint(ccp(0,0));
							stuckCountlabel->setPosition(ccp(0,0));
							stuckCountlabel->setTagbyKey("stack");
							nItemSpr->addChild(stuckCountlabel,1);
						}
					}
					//开始填充书库
					m_ItemData[nKey].uGenre = pObjs[i].Obj.uGenre;
					m_ItemData[nKey].uId    = pObjs[i].Obj.uId;
					m_ItemData[nKey].nindex = i;
					m_ItemData[nKey].DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
					m_ItemData[nKey].DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
					m_ItemData[nKey].DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
					m_ItemData[nKey].DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150); //112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey = nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
					if (!bgcolorLayer)
					{
						bgcolorLayer = CCLayerColor::create(color);//颜色层
					    //bgcolorLayer->setPosition(CCPointZero);
						bgcolorLayer->setAnchorPoint(ccp(0,0));
						CCSize bgsize;
						bgsize.width  = no.DataW*ITEM_CELL_SIZE-2;
						bgsize.height = no.DataH*ITEM_CELL_SIZE-2;
					    bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
						bgcolorLayer->setTagbyKey(nKey);
						ParentNode_Item->addChild(bgcolorLayer,0);
					}

					int nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
					int nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;
					
					nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
					//颜色层
					nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+1;
					nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
					bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

					KUiObjAtContRegion	Obj;
					Obj.Obj.uGenre = no.uGenre;
					Obj.Obj.uId    = no.uId;
					Obj.Region.h   = Obj.Region.v = 0;
					Obj.Region.Width = Obj.Region.Height = 0;
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

					ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
					    GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
					//if (eProp == IIEP_NORMAL) //可用
					//else 
					if (eProp == IIEP_NOT_USEABLE)  //不可用
					{
						ccColor3B color_ = ccc3(128, 0, 0);
						bgcolorLayer->setColor(color_);
					}
					//else if (eProp == IIEP_SPECIAL) //其他特殊物品
				}
			}
		}
		free(pObjs); //释放内存
		pObjs = NULL; //清空
	}

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_Item->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}


//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiPlayerTrade::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\交易\\玩家交易－锁定交易.spr");
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
	okConfirm = CCMenuItemSprite::create(sprite_ok_btn_confirm_normal, sprite_ok_btn_confirm_select, this, menu_selector(KuiPlayerTrade::okCallBackFunc));
	okConfirm->setPosition(ccp(-140,0));
	okConfirm->setTagbyKey("box");
	//最终确认
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\交易\\玩家交易－最终确认.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_trade_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_trade_select = CCSprite::createWithTexture(bgCur);
	tradeConfirm = CCMenuItemSprite::create(sprite_trade_normal, sprite_trade_select, this, menu_selector(KuiPlayerTrade::tradeCallBackFunc));
	tradeConfirm->setPosition(ccp(-15,0));
	tradeConfirm->setTagbyKey("box");
	//关闭
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\交易\\玩家交易－取消交易.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_select = CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * closeConfirm = CCMenuItemSprite::create(sprite_close_normal, sprite_close_select, this, menu_selector(KuiPlayerTrade::buttonCallBackFunc));
	//closeConfirm->setTagbyKey("box");
	closeConfirm->setPosition(ccp(120,0));

	//开始增加操作按钮
	CCSprite * sprite_fanzhi_normal =  CCSprite::create("ui/btn/fz01.png");
	CCSprite * sprite_fanzhi_select = CCSprite::create("ui/btn/fz02.png");
	CCMenuItemSprite * fanzhiConfirm = CCMenuItemSprite::create(sprite_fanzhi_normal, sprite_fanzhi_select, this, menu_selector(KuiPlayerTrade::btnCallBackFunc_fangzhi));
	fanzhiConfirm->setPosition(ccp(38,-30));

	CCSprite * sprite_quxia_normal =  CCSprite::create("ui/btn/qx01.png");
	CCSprite * sprite_quxia_select = CCSprite::create("ui/btn/qx02.png");
	CCMenuItemSprite * quxiaConfirm = CCMenuItemSprite::create(sprite_quxia_normal, sprite_quxia_select, this, menu_selector(KuiPlayerTrade::btnCallBackFunc_quxia));
	quxiaConfirm->setPosition(ccp(38+sprite_quxia_normal->getContentSize().width,-30));
	//CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\zenglibtn.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zhengli_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zhengli_select =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * zhengliConfirm = CCMenuItemSprite::create(sprite_zhengli_normal, sprite_zhengli_select, this, menu_selector(KuiPlayerTrade::zhengCallBackFunc));
	zhengliConfirm->setPosition(ccp(35+sprite_quxia_normal->getContentSize().width*2,-30));
	
	tradeConfirm->setEnabled(false);
	CCMenu * menu = CCMenu::create(okConfirm,tradeConfirm,closeConfirm,fanzhiConfirm,quxiaConfirm,zhengliConfirm,NULL);
	menu->setisCheckBox(true);
	menu->setPosition(ccp(m_size.width/2,20)); //菜单位置设置为弹出框左下
	ParentNode_ItemEx->addChild(menu);         //加到父层里

	CCScale9Sprite *pBg = CCScale9Sprite::create(); 
	m_SelfMoneyBox = CCEditBox::create(CCSize(112,24),pBg);
	const ccColor3B nfcolor ={255,255,255};
	if (m_SelfMoneyBox)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		m_SelfMoneyBox->setPlaceHolder("input money");
		m_SelfMoneyBox->setFontColor(nfcolor);
		m_SelfMoneyBox->setFontSize(24);
		m_SelfMoneyBox->setMaxLength(9);
		m_SelfMoneyBox->setPosition(ccp(90,45));
		m_SelfMoneyBox->setInputMode(kEditBoxInputModeNumeric);
		m_SelfMoneyBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		ParentNode_ItemEx->addChild(m_SelfMoneyBox,1,500);
	}

	m_OtherMoney = CCLabelTTF::create("money",UI_GAME_FONT_DEFAULT,12);//Arial
	m_OtherMoney->setColor(ccYELLOW);
	m_OtherMoney->setPosition(ccp(90,m_size.height-170));
	ParentNode_ItemEx->addChild(m_OtherMoney);
}


void KuiPlayerTrade::zhengCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
	}

	if  (m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}
	if  (m_ItemData[nstrCurSelItemKey].uId<=0 || m_ItemData[nstrCurSelItemKey].uId>=MAX_ITEM)
	{
#ifdef WIN32
		CCMessageBox("该物品不能整理","提示:");
#else
		CCMessageBox(UTEXT("该物品不能整理",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}

	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);

	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数

	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=2)
	{//间隔２秒
		//nBakChickTime = nCurChicktime;
#ifdef WIN32
		CCMessageBox("整理的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("整理的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	nBakChickTime = nCurChicktime;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipment); 
}
//最终确认 开始交易
void KuiPlayerTrade::tradeCallBackFunc(CCObject * pSender)
{
	g_pCoreShell->OperationRequest(GOI_TRADE,0,0);
	UpdateTradeOperData();
}
//锁定交易
void KuiPlayerTrade::okCallBackFunc(CCObject * pSender)
{
	if (pSender)
	{
		CCNode *  nCurSelNode = ((CCMenuItemImage *)pSender)->getSelectedSprite();
		if (nCurSelNode)
		{
			OnAdjustedMoney();
			if  (nCurSelNode->isVisible()) 
			{//已经是选择状态 未选择---选择
				g_pCoreShell->OperationRequest(GOI_TRADE_LOCK,0,true);
				__IsLock = true;
			}
			else
			{//设置为选择状态 :选择---未选择
				g_pCoreShell->OperationRequest(GOI_TRADE_LOCK,0,false);
				__IsLock = false;
			}
			UpdateTradeOperData();
		}
		//((CCMenuItemImage *)pSender)->setEnabled(false);
	}

	/*CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);*/
}

void KuiPlayerTrade::btnCallBackFunc_fangzhi(CCObject * pSender)
{//放置自己的东西
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}

	if  (nstrCurSelItemKey.empty() || m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示",1).c_str());
#endif
		return;
	}
	if  (m_ItemData[nstrCurSelItemKey].uId<=0 || m_ItemData[nstrCurSelItemKey].uId>=MAX_ITEM)
	{
#ifdef WIN32
		CCMessageBox("该物品不能放置","提示");
#else
		CCMessageBox(UTEXT("该物品不能放置",1).c_str(),UTEXT("提示",1).c_str());
#endif	
		return;
	}

	if (__IsLock)
	{
#ifdef WIN32
		CCMessageBox("锁定交易不能放置","提示");
#else
		CCMessageBox(UTEXT("锁定交易不能放置",1).c_str(),UTEXT("提示",1).c_str());
#endif	
		return;
	}
	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);

	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数

	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=2)
	{//间隔２秒
		return;
	}
	nBakChickTime = nCurChicktime;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	nstrCurSelItemKey="";
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_trade); 
	//g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_immediacy); 
}

void KuiPlayerTrade::btnCallBackFunc_quxia(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
	}

	if  (inCurSelItemKey<=-1 || m_giveItemData.count(inCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}
	if  (m_giveItemData[inCurSelItemKey].uId<=0 || m_giveItemData[inCurSelItemKey].uId>=MAX_ITEM)
	{
#ifdef WIN32
		CCMessageBox("该物品不能取下","提示:");
#else
		CCMessageBox(UTEXT("该物品不能取下",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}

	if (__IsLock)
	{
#ifdef WIN32
		CCMessageBox("锁定交易不能取下","提示");
#else
		CCMessageBox(UTEXT("锁定交易不能取下",1).c_str(),UTEXT("提示",1).c_str());
#endif	
		return;
	}

	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);

	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数

	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=2)
	{//间隔２秒
		return;
	}
	nBakChickTime = nCurChicktime;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_giveItemData[inCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_giveItemData[inCurSelItemKey].uId;
	Obj.Region.h      = m_giveItemData[inCurSelItemKey].DataX;
	Obj.Region.v      = m_giveItemData[inCurSelItemKey].DataY;
	Obj.Region.Width  = m_giveItemData[inCurSelItemKey].DataW;
	Obj.Region.Height = m_giveItemData[inCurSelItemKey].DataH;
	Obj.eContainer    = UOC_TO_BE_TRADE;
	inCurSelItemKey=-1;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipment);
}

void KuiPlayerTrade::__ClosePad()
{
	CCNode * node = NULL;
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}
//关闭面板
void KuiPlayerTrade::buttonCallBackFunc(CCObject * pSender)
{
	//交易取消
	g_pCoreShell->OperationRequest(GOI_TRADE_CANCEL,0,0);
	
	CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiPlayerTrade::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiPlayerTrade::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

void KuiPlayerTrade::closeItemPadCallback(CCNode *pNode)
{//在这个层中标记号，以确定是哪个按钮
	m_descPadIsOpen = false;
}

CCRect KuiPlayerTrade::gettarGiveItem(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition();                      //这个因该是图内偏移
	rc.size       = pNode->getContentSize();                   //这个物品的宽度和高度
	rc.origin.x   = m_origin.x + m_size.width/2-28+rc.origin.x;      //起点最左边 + m_size.width-30
	rc.origin.y   = m_origin.y+rc.origin.y+m_size.height/2-115;    //最上边    //- rc.origin.y
	return rc;
}

CCRect KuiPlayerTrade::getGiveItem(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition();                      //这个因该是图内偏移
	rc.size       = pNode->getContentSize();                   //这个物品的宽度和高度
	rc.origin.x   = m_origin.x + m_size.width/2-28+rc.origin.x;      //起点最左边 + m_size.width-30
	rc.origin.y   = m_origin.y+ rc.origin.y + 70;              //最上边
	return rc;
}


CCRect KuiPlayerTrade::getRectItem(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x+ rc.origin.x+20+28*6; //起点最左边 m_size.width/2-30
	rc.origin.y   = m_origin.y+ rc.origin.y+70; //最上边    //- rc.origin.y
	
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiPlayerTrade::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * SkillNode =NULL;
	CCLayerColor *nCurColorlayer = NULL;
	bool isthis = false;

	_ItemData::iterator it;
	for( it = m_ItemData.begin(); it != m_ItemData.end(); ++it)
	{
		if (it->second.uId>0 && it->second.uId<MAX_ITEM)
		{////重置某技能的加成
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				if (getRectItem(nCurColorlayer,0).containsPoint(location))
				{//点中了
					if (SkillNode)
					{
						inCurSelItemKey   = -1;
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						m_ItemData[nItemKey].nRect  = getRectItem(nCurColorlayer,0);
						m_ItemData[nItemKey].nindex = -1;
						CCLayer * sDescLayer = KuiItemdesc::create(it->second,this,callfuncN_selector(KuiPlayerTrade::closeItemPadCallback),1);
						m_descPadIsOpen = true;
						//m_CurSelItemIndex = -1;//m_ItemData[i].uId;
						nstrCurSelItemKey =nItemKey;

						isthis = true;
						this->addChild(sDescLayer,2,1001);
					}
				}
				else
				{
					if (!isthis) //点中了,就不归零
					{
						nstrCurSelItemKey ="";
						inCurSelItemKey   = -1;
						//m_CurSelItemIndex = -1;
					}
					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
		}
	}
	//give点击
	isthis = false;
	_giveItemData::iterator its;
	for( its = m_giveItemData.begin(); its != m_giveItemData.end(); ++its)
	{
		if (its->second.uId>0 && its->second.uId<MAX_ITEM)
		{////重置某技能的加成
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[its->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_Give->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[its->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Give->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				//CCMessageBox("开始点击","开始点击");
				if (getGiveItem(nCurColorlayer,0).containsPoint(location))
				{//点中了
					//CCMessageBox("点中了","点中了");
					if (SkillNode)
					{
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						its->second.nRect  = getGiveItem(nCurColorlayer,0);
						inCurSelItemKey    = its->second.nindex;
						//its->second.nindex = -1;
						CCLayer * sDescLayer = KuiItemdesc::create(its->second,this,callfuncN_selector(KuiPlayerTrade::closeItemPadCallback),1);
						m_descPadIsOpen = true;
						//m_CurSelItemIndex = -1;//m_ItemData[i].uId;
						//nstrCurSelItemKey =nItemKey;
						isthis = true;
						this->addChild(sDescLayer,2,1001);
					}
				}
				else
				{
					if (!isthis) //点中了,就不归零
					{
						//nstrCurSelItemKey ="";
						inCurSelItemKey   = -1;
						//m_CurSelItemIndex = -1;
					}
					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
		}
	}

	//targive点击
	isthis = false;
	_targiveItemData::iterator itt;
	for( itt = m_targiveItemData.begin(); itt != m_targiveItemData.end(); ++itt)
	{
		if (itt->second.uId>0 && itt->second.uId<MAX_ITEM)
		{////重置某技能的加成
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[itt->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_tarGive->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[itt->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_tarGive->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				//CCMessageBox("开始点击","开始点击");
				if (gettarGiveItem(nCurColorlayer,0).containsPoint(location))
				{//点中了
					//CCMessageBox("点中了","点中了");
					if (SkillNode)
					{
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						itt->second.nRect  = gettarGiveItem(nCurColorlayer,0);
						//inCurSelItemKey    = itt->second.nindex;
						//its->second.nindex = -1;
						CCLayer * sDescLayer = KuiItemdesc::create(itt->second,this,callfuncN_selector(KuiPlayerTrade::closeItemPadCallback),1);
						m_descPadIsOpen = true;
						//m_CurSelItemIndex = -1;//m_ItemData[i].uId;
						//nstrCurSelItemKey =nItemKey;

						isthis = true;
						this->addChild(sDescLayer,2,1001);
					}
				}
				else
				{
					if (!isthis) //点中了,就不归零
					{
						//nstrCurSelItemKey ="";
						//inCurSelItemKey   = -1;
						//m_CurSelItemIndex = -1;
					}
					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
		}
	}
	return true;
}

void KuiPlayerTrade::UpdateTradeOperData()
{
	int bLock = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_LOCKED, 0);
	//m_Ok.CheckButton(bLock);
	if(okConfirm)
	{
		if (bLock)
	       okConfirm->selected();
		else
			okConfirm->unselected();
	}
	if (!ptradeInfo)
	{
		ptradeInfo = CCLabelTTF::create("",UI_GAME_FONT_DEFAULT,12);//Arial
		ptradeInfo->setColor(ccYELLOW);
		ptradeInfo->setPosition(ccp(120,m_size.height-190));
		ParentNode_ItemEx->addChild(ptradeInfo);
	}

	char nTempStr[64];
	ZeroMemory(nTempStr,sizeof(nTempStr));
	if (g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_TRADING, 0))
	{//等待对方锁定
		if (tradeConfirm)
		{
			tradeConfirm->selected();
			tradeConfirm->setEnabled(true);
		}
		//m_Trade.Enable(true);
		//m_Trade.CheckButton(true);
		//m_Info.SetText(STR_WAIT_TRADING, strlen(STR_WAIT_TRADING));
		//m_Info.SetTextColor(m_uWaitTradeMsgColor);
		t_sprintf(nTempStr,STR_WAIT_TRADING);
		if (ptradeInfo)
			ptradeInfo->setString(UTEXT(nTempStr,1).c_str());
	}
	else
	{
		int bOtherLock = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_OTHER_LOCKED, 0);
		//m_Trade.CheckButton(false);
		//m_Trade.Enable(bLock && bOtherLock);
		if (tradeConfirm)
		{
			tradeConfirm->unselected();
			tradeConfirm->setEnabled(bLock && bOtherLock);
		}
		//对方是否锁定
		t_sprintf(nTempStr,bOtherLock ? STR_OTHER_OK:STR_OTHER_NOT_OK);
		if (ptradeInfo)
			ptradeInfo->setString(UTEXT(nTempStr,1).c_str());
		//m_Info.SetText(bOtherLock ? STR_OTHER_OK : STR_OTHER_NOT_OK, -1);
		//m_Info.SetTextColor(bOtherLock ? m_uOtherLockMsgColor : m_uOtherNotLockMsgColor);
	}
	if (m_SelfMoneyBox)
	   m_SelfMoneyBox->setEnabled(!bLock);
	/*m_SelfItemsBox.Enable(!bLock);
	m_SelfMoney.Enable(!bLock);
	m_TakewithItemsBox.Enable(!bLock);
	m_AddMoney.Enable(!bLock);
	m_ReduceMoney.Enable(!bLock);
	m_SelfMoney.Enable(!bLock);*/
}

//对方的东西
void KuiPlayerTrade::UpdateTarGiveItem(KUiObjAtRegion* pObj, int bAdd)
{
	if (!pObj) return;
	if (pObj->Obj.uGenre == CGOG_MONEY)
	{//对方的钱改变了
		if (m_OtherMoney)
		{
		  char nTempStr[64];
		  ZeroMemory(nTempStr,sizeof(nTempStr));
		  t_sprintf(nTempStr,"%u",pObj->Obj.uId);
		  m_OtherMoney->setString(nTempStr);
		}
	}
	else
	{//物品
			KUiDraggedObject Obj;
			Obj.uGenre = pObj->Obj.uGenre;
			Obj.uId    = pObj->Obj.uId;
			Obj.DataX  = pObj->Region.h;
			Obj.DataY  = pObj->Region.v;
			Obj.DataW  = pObj->Region.Width;
			Obj.DataH  = pObj->Region.Height;

			if  (Obj.uId<=0 || Obj.uId>=MAX_ITEM)
			{
				//CCMessageBox("数据有误","物品");
				return;
			}

			if (bAdd) //增加
			{
				//m_ItemBox.AddObject(&Obj,1);
				if (pObj->Obj.uId)
				{
					m_targiveNum++;
				}

				AddtarGiveObject(&Obj,m_targiveNum);

			}
			else      //减少
			{
				if (pObj->Obj.uId)
				{
					m_targiveNum--;
				}
				char nKey[32];
				t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
				std::string nItemKey = nKey;
				ParentNode_tarGive->removeChildByTagKey(nItemKey,true);
				t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
				nItemKey = nKey;
				ParentNode_tarGive->removeChildByTagKey(nItemKey,true);
				_targiveItemData::iterator it;
				for( it = m_targiveItemData.begin(); it != m_targiveItemData.end(); it++)
				{
					if (it->second.uId>0 && it->second.uId<MAX_ITEM && it->second.uId ==Obj.uId)
					{
						m_targiveItemData.erase(it->second.nindex);//
						break;
					}
				}
			}
	}
}

//本人的东西
void KuiPlayerTrade::UpdateGiveItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{

		if (pItem->Obj.uGenre == CGOG_MONEY)
		{//自己的钱
			m_nSelfTradeMoney = pItem->Obj.uId;
			//m_SelfMoney.SetIntText(pObj->Obj.uId);
			if (m_SelfMoneyBox)
			{
				char nTempStr[64];
				ZeroMemory(nTempStr,sizeof(nTempStr));
				t_sprintf(nTempStr,"%u",pItem->Obj.uId);
				m_SelfMoneyBox->setText(nTempStr);
			}
			return;
		}

		KUiDraggedObject Obj;
		Obj.uGenre = pItem->Obj.uGenre;
		Obj.uId    = pItem->Obj.uId;
		Obj.DataX  = pItem->Region.h;
		Obj.DataY  = pItem->Region.v;
		Obj.DataW  = pItem->Region.Width;
		Obj.DataH  = pItem->Region.Height;

		if  (Obj.uId<=0 || Obj.uId>=MAX_ITEM)
		{
			//CCMessageBox("数据有误","物品");
			return;
		}

		if (bAdd) //增加
		{
			//m_ItemBox.AddObject(&Obj,1);
			if (pItem->Obj.uId)
			{
				m_giveNum++;
			}

			AddGiveObject(&Obj,m_giveNum);

		}
		else      //减少
		{
			if (pItem->Obj.uId)
			{
				m_giveNum--;
			}
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
			t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
			nItemKey = nKey;
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
			_giveItemData::iterator it;
			for( it = m_giveItemData.begin(); it != m_giveItemData.end(); it++)
			{
				if (it->second.uId>0 && it->second.uId<MAX_ITEM && it->second.uId ==Obj.uId)
				{
					m_giveItemData.erase(it->second.nindex);
					break;
				}
			}
			//m_giveItemData.erase(nItemKey);
		}
	}
}

void KuiPlayerTrade::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{

	if (pItem)
	{
		KUiDraggedObject Obj;
		Obj.uGenre = pItem->Obj.uGenre;
		Obj.uId    = pItem->Obj.uId;
		Obj.DataX  = pItem->Region.h;
		Obj.DataY  = pItem->Region.v;
		Obj.DataW  = pItem->Region.Width;
		Obj.DataH  = pItem->Region.Height;

		if  (Obj.uId<=0 || Obj.uId>=MAX_ITEM)
		{
			//CCMessageBox("数据有误","物品");
			return;
		}

		if (bAdd) //增加
		{
			//m_ItemBox.AddObject(&Obj,1);
			if (pItem->Obj.uId)
			{
				m_itemNum ++;
			}
			
			AddObject(&Obj,1);

		}
		else      //减少
		{
			if (pItem->Obj.uId)
			{
				m_itemNum--;
			}
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_Item->removeChildByTagKey(nItemKey,true);
			t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
			nItemKey = nKey;
			ParentNode_Item->removeChildByTagKey(nItemKey,true);
			m_ItemData.erase(nItemKey);
		}
		//m_ItemBox.RemoveObject(&Obj);
		//UiSoundPlay(UI_SI_PICKPUT_ITEM);
		//UpdateData();
	}
	else //更新全部的物品
		UpdateItemData();
}

void KuiPlayerTrade::UpdateItemData()
{
	char nTempStr[64];
	ZeroMemory(nTempStr,sizeof(nTempStr));
	m_nTakewithMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	t_sprintf(nTempStr,"&d",m_nTakewithMoney);
	pMoneyLabel->setString(nTempStr);
// m_CurSelItemIndex = -1;
   nstrCurSelItemKey ="";
  // memset(m_ItemData,0,sizeof(KuiSellItemdObject) * 60);
   _ItemData::iterator it;
   for( it = m_ItemData.begin(); it != m_ItemData.end(); it++)
   {
	   if (it->second.uId>0 && it->second.uId<MAX_ITEM)
	   {////重置某技能的加成
		   char nKey[32];
		   t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
		   std::string nItemKey = nKey;
		   ParentNode_Item->removeChildByTagKey(nItemKey,true);
		   //nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
		   t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
		   nItemKey = nKey;
		   //SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
		   ParentNode_Item->removeChildByTagKey(nItemKey,true);
	   }
   }
   m_ItemData.clear();

	KUiObjAtRegion* pObjs = NULL;
	int nCount    = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nItemNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; ++i)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId    = pObjs[i].Obj.uId;
			no.DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
			no.DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
			no.DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
			no.DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
			if (Item && pObjs[i].Obj.uId>0 && pObjs[i].Obj.uId<MAX_ITEM)
			{
				char nItemSprPath[256];
				ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
				if  (Item[pObjs[i].Obj.uId].GetImagePath())
					t_sprintf(nItemSprPath,Item[pObjs[i].Obj.uId].GetImagePath());
				else
					t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");

				if  (nItemSprPath[0])
				{
					//t_sprintf(nItemSprPath,Item[pObjs[i].Obj.uId].GetImagePath());
					g_StrLower(nItemSprPath);
					int m_nWidth,m_nHeight,nFrams;
					CCTexture2D *bgCur = NULL;
					SPRFRAMSINFO nSprInfo;
					ZeroMemory(&nSprInfo,sizeof(nSprInfo));
					bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
					if (bgCur==NULL)
						return;
					char nItemKey[32];
					ZeroMemory(&nItemKey,sizeof(nItemKey));
					t_sprintf(nItemKey,"item_%u",Item[pObjs[i].Obj.uId].GetID());
					std::string nKey =nItemKey;
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

						if  (Item[pObjs[i].Obj.uId].GetGenre()!=item_equip && Item[pObjs[i].Obj.uId].IsStack())
						{//不是装备 可叠加物品
							char stack[32];
							t_sprintf(stack,"%d",Item[pObjs[i].Obj.uId].GetStackNum());
							CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
							stuckCountlabel->setColor(ccYELLOW);
							stuckCountlabel->setAnchorPoint(ccp(0,0));
							stuckCountlabel->setPosition(ccp(0,0));
							stuckCountlabel->setTagbyKey("stack");
							nItemSpr->addChild(stuckCountlabel,1);
						}
					}
					//开始填充书库
					m_ItemData[nKey].uGenre = pObjs[i].Obj.uGenre;
					m_ItemData[nKey].uId    = pObjs[i].Obj.uId;
					m_ItemData[nKey].nindex = i;
					m_ItemData[nKey].DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
					m_ItemData[nKey].DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
					m_ItemData[nKey].DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
					m_ItemData[nKey].DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
					if (!bgcolorLayer)
					{
						bgcolorLayer = CCLayerColor::create(color);//颜色层
					    //bgcolorLayer->setPosition(CCPointZero);
						bgcolorLayer->setAnchorPoint(ccp(0,0));
						CCSize bgsize;
						bgsize.width  = no.DataW*ITEM_CELL_SIZE-2;
						bgsize.height = no.DataH*ITEM_CELL_SIZE-2;
					    bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
						bgcolorLayer->setTagbyKey(nKey);
						ParentNode_Item->addChild(bgcolorLayer,0);
					}

					int nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
					int nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;
					
					nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
					//颜色层
					nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+1;
					nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
					bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

					KUiObjAtContRegion	Obj;
					Obj.Obj.uGenre = no.uGenre;
					Obj.Obj.uId    = no.uId;
					Obj.Region.h   = Obj.Region.v = 0;
					Obj.Region.Width = Obj.Region.Height = 0;
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

					ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
					    GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
					//if (eProp == IIEP_NORMAL) //可用
					//else 
					if (eProp == IIEP_NOT_USEABLE)  //不可用
					{
						ccColor3B color_ = ccc3(128, 0, 0);
						bgcolorLayer->setColor(color_);
					}
					//else if (eProp == IIEP_SPECIAL) //其他特殊物品
				}
			}
		}
		free(pObjs); //释放内存
		pObjs = NULL; //清空
	}
}

void KuiPlayerTrade::OnModififyMoney()
{
	m_bAdjustingMoney = 1;
	char nTempStr[64];
	ZeroMemory(nTempStr,sizeof(nTempStr));
	t_sprintf(nTempStr,m_SelfMoneyBox->getText());
	
	int nNewSelfTradeMoney = g_Atoui(nTempStr);//m_SelfMoney.GetIntNumber();
	if (nNewSelfTradeMoney < 0)
		nNewSelfTradeMoney = 0;
	if (nNewSelfTradeMoney > m_nTakewithMoney + m_nSelfTradeMoney)
	{
		nNewSelfTradeMoney = m_nTakewithMoney + m_nSelfTradeMoney;
		//m_SelfMoney.SetIntText(nNewSelfTradeMoney);
		t_sprintf(nTempStr,"%u",nNewSelfTradeMoney);
		m_SelfMoneyBox->setText(nTempStr);
	}

	m_nTakewithMoney -= nNewSelfTradeMoney - m_nSelfTradeMoney;
	m_nSelfTradeMoney = nNewSelfTradeMoney;
	//m_TakewithMoney.SetMoneyText(m_nTakewithMoney);
	t_sprintf(nTempStr,"%u",m_nTakewithMoney);
	pMoneyLabel->setString(nTempStr);
}

void KuiPlayerTrade::OnAdjustedMoney()
{
	OnModififyMoney();
	m_bAdjustingMoney = 0;
	if (g_pCoreShell)
	{
		KUiObjAtRegion	Obj = {0};
		Obj.Obj.uGenre = CGOG_MONEY;
		Obj.Obj.uId = m_nSelfTradeMoney;
		g_pCoreShell->OperationRequest(GOI_TRADE_DESIRE_ITEM,(unsigned int)&Obj, 0);
	}
}

void KuiPlayerTrade::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
		if (m_bAdjustingMoney)
		{
			if ((++m_bAdjustingMoney) == SEND_MONEY_CHANGE_MSG_DELAY)
				OnAdjustedMoney();
		}

	   if (g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, GET_BOXEX_OPEN, 0))
		   m_nIstrue = true;
		   //	m_ItemEX.SetOpenClose(TRUE);
		   else
			   m_nIstrue = false;

	  //更新叠加数量
		   _ItemData::iterator it;
		   for( it = m_ItemData.begin(); it != m_ItemData.end(); ++it)
		   {
			   if (it->second.uId>0 && it->second.uId<MAX_ITEM)
			   {
				   if  (Item[it->second.uId].GetGenre()==0)
					   continue;

				   char nItemKey[32];
				   t_sprintf(nItemKey,"item_%u",Item[it->second.uId].GetID());
				   std::string nKey =nItemKey;
				   CCSprite *nTempSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
				   if  (nTempSpr)
				   {
					   CCLabelTTF *stuckCountlabel = (CCLabelTTF *)nTempSpr->getChildByTagKey("stack");
					   if  (stuckCountlabel)
					   {
						   char nCount[32];
						   t_sprintf(nCount,"%d",Item[it->second.uId].GetStackNum());
						   stuckCountlabel->setString(nCount);
					   }
				   }
			   }
		   }
	}
}

void KuiPlayerTrade::draw()
{
	//CCMessageBox("无限循环","draw");
}
//对方的面板
int KuiPlayerTrade::AddtarGiveObject(KUiDraggedObject* pObject, int nindex)
{
	if (!pObject) return false;
	for (int i = 0; i < 1; ++i)
	{
		KUiDraggedObject no;
		no.uGenre = pObject->uGenre;
		no.uId    = pObject->uId;
		no.DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
		no.DataY  = pObject->DataY;     //包袱中的起点Y位置
		no.DataW  = pObject->DataW; //物品的高度(占用了多少个格子)
		no.DataH  = pObject->DataH;//物品的宽度(占用了多少个格子)
		if (Item && pObject->uId>0 && pObject->uId<MAX_ITEM)
		{
			char nItemSprPath[256];
			ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
			if  (Item[pObject->uId].GetImagePath())
				t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
			else
				t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
			if (nItemSprPath[0])
			{
				//t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
				g_StrLower(nItemSprPath);
				int m_nWidth,m_nHeight,nFrams;
				CCTexture2D *bgCur = NULL;
				SPRFRAMSINFO nSprInfo;
				ZeroMemory(&nSprInfo,sizeof(nSprInfo));
				bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
				if (bgCur==NULL)
					return false;
				char nItemKey[32];
				ZeroMemory(&nItemKey,sizeof(nItemKey));
				t_sprintf(nItemKey,"item_%u",Item[pObject->uId].GetID());
				std::string nKey =nItemKey;
				CCSprite *nItemSpr = (CCSprite *)ParentNode_tarGive->getChildByTagKey(nKey);
				if (!nItemSpr)
				{
					nItemSpr = CCSprite::createWithTexture(bgCur);
					//nItemSpr->setAnchorPoint(ccp(0,0));
					nItemSpr->setTagbyKey(nKey);
					ParentNode_tarGive->addChild(nItemSpr,1);

					if  (Item[pObject->uId].GetGenre()!=item_equip && Item[pObject->uId].IsStack())
					{//不是装备 可叠加物品
						char stack[32];
						t_sprintf(stack,"%d",Item[pObject->uId].GetStackNum());
						CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
						stuckCountlabel->setColor(ccYELLOW);
						stuckCountlabel->setAnchorPoint(ccp(0,0));
						stuckCountlabel->setPosition(ccp(0,0));
						stuckCountlabel->setTagbyKey("stack");
						nItemSpr->addChild(stuckCountlabel,1);
					}
				}
				//开始填充书库
				m_targiveItemData[nindex].uGenre = pObject->uGenre;
				m_targiveItemData[nindex].uId    = pObject->uId;
				m_targiveItemData[nindex].nindex = nindex;
				m_targiveItemData[nindex].DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
				m_targiveItemData[nindex].DataY  = pObject->DataY;     //包袱中的起点Y位置
				m_targiveItemData[nindex].DataW  = pObject->DataW;     //物品的高度(占用了多少个格子)
				m_targiveItemData[nindex].DataH  = pObject->DataH;     //物品的宽度(占用了多少个格子)
				m_targiveItemData[nindex].nGenkind = UOC_OTHER_TO_BE_TRADE;  //自己面板

				ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
				//t_sprintf(nItemKey,"color_%d",i+1);
				strcat(nItemKey,"_color");
				nKey =nItemKey;
				CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_tarGive->getChildByTagKey(nKey);
				if (!bgcolorLayer)
				{
					bgcolorLayer = CCLayerColor::create(color);//颜色层
					//bgcolorLayer->setPosition(CCPointZero);
					bgcolorLayer->setAnchorPoint(ccp(0,0));
					CCSize bgsize;
					bgsize.width  = no.DataW*ITEM_CELL_SIZE-2;
					bgsize.height = no.DataH*ITEM_CELL_SIZE-2;
					bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
					bgcolorLayer->setTagbyKey(nKey);
					ParentNode_tarGive->addChild(bgcolorLayer,0);
				}

				int nCurX = m_StartPos_t.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
				int nCurY = m_StartPos_t.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;

				nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
				//颜色层
				nCurX = m_StartPos_t.x+no.DataX*ITEM_CELL_SIZE+1;
				nCurY = m_StartPos_t.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
				bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

				KUiObjAtContRegion	Obj;
				Obj.Obj.uGenre = no.uGenre;
				Obj.Obj.uId    = no.uId;
				Obj.Region.h   = Obj.Region.v = 0;
				Obj.Region.Width = Obj.Region.Height = 0;
				Obj.nContainer = UOC_ITEM_GIVE;

				ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
					GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
				//if (eProp == IIEP_NORMAL) //可用
				//else 
				if (eProp == IIEP_NOT_USEABLE)  //不可用
				{
					ccColor3B color_ = ccc3(128, 0, 0);
					bgcolorLayer->setColor(color_);
				}
				//else if (eProp == IIEP_SPECIAL) //其他特殊物品
			}
		}
	}
	return true;
}

//自己面板
int KuiPlayerTrade::AddGiveObject(KUiDraggedObject* pObject, int nindex)
{
	if (!pObject) return false;
	for (int i = 0; i < 1; ++i)
	{
		KUiDraggedObject no;
		no.uGenre = pObject->uGenre;
		no.uId    = pObject->uId;
		no.DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
		no.DataY  = pObject->DataY;     //包袱中的起点Y位置
		no.DataW  = pObject->DataW; //物品的高度(占用了多少个格子)
		no.DataH  = pObject->DataH;//物品的宽度(占用了多少个格子)
		if (Item && pObject->uId>0 && pObject->uId<MAX_ITEM)
		{
			char nItemSprPath[256];
			ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
			if  (Item[pObject->uId].GetImagePath())
				t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
			else
				t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
			if (nItemSprPath[0])
			{
				//t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
				g_StrLower(nItemSprPath);
				int m_nWidth,m_nHeight,nFrams;
				CCTexture2D *bgCur = NULL;
				SPRFRAMSINFO nSprInfo;
				ZeroMemory(&nSprInfo,sizeof(nSprInfo));
				bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
				if (bgCur==NULL)
					return false;
				char nItemKey[32];
				ZeroMemory(&nItemKey,sizeof(nItemKey));
				t_sprintf(nItemKey,"item_%u",Item[pObject->uId].GetID());
				std::string nKey =nItemKey;
				CCSprite *nItemSpr = (CCSprite *)ParentNode_Give->getChildByTagKey(nKey);
				if (!nItemSpr)
				{
					nItemSpr = CCSprite::createWithTexture(bgCur);
					//nItemSpr->setAnchorPoint(ccp(0,0));
					nItemSpr->setTagbyKey(nKey);
					ParentNode_Give->addChild(nItemSpr,1);

					if  (Item[pObject->uId].GetGenre()!=item_equip && Item[pObject->uId].IsStack())
					{//不是装备 可叠加物品
						char stack[32];
						t_sprintf(stack,"%d",Item[pObject->uId].GetStackNum());
						CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
						stuckCountlabel->setColor(ccYELLOW);
						stuckCountlabel->setAnchorPoint(ccp(0,0));
						stuckCountlabel->setPosition(ccp(0,0));
						stuckCountlabel->setTagbyKey("stack");
						nItemSpr->addChild(stuckCountlabel,1);
					}
				}
				//开始填充书库
				m_giveItemData[nindex].uGenre = pObject->uGenre;
				m_giveItemData[nindex].uId    = pObject->uId;
				m_giveItemData[nindex].nindex = nindex;
				m_giveItemData[nindex].DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
				m_giveItemData[nindex].DataY  = pObject->DataY;     //包袱中的起点Y位置
				m_giveItemData[nindex].DataW  = pObject->DataW;     //物品的高度(占用了多少个格子)
				m_giveItemData[nindex].DataH  = pObject->DataH;     //物品的宽度(占用了多少个格子)
				m_giveItemData[nindex].nGenkind = UOC_TO_BE_TRADE;  //自己面板

				ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
				//t_sprintf(nItemKey,"color_%d",i+1);
				strcat(nItemKey,"_color");
				nKey =nItemKey;
				CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Give->getChildByTagKey(nKey);
				if (!bgcolorLayer)
				{
					bgcolorLayer = CCLayerColor::create(color);//颜色层
					//bgcolorLayer->setPosition(CCPointZero);
					bgcolorLayer->setAnchorPoint(ccp(0,0));
					CCSize bgsize;
					bgsize.width  = no.DataW*ITEM_CELL_SIZE-2;
					bgsize.height = no.DataH*ITEM_CELL_SIZE-2;
					bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
					bgcolorLayer->setTagbyKey(nKey);
					ParentNode_Give->addChild(bgcolorLayer,0);
				}

				int nCurX = m_StartPos_g.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
				int nCurY = m_StartPos_g.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;

				nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
				//颜色层
				nCurX = m_StartPos_g.x+no.DataX*ITEM_CELL_SIZE+1;
				nCurY = m_StartPos_g.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
				bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

				KUiObjAtContRegion	Obj;
				Obj.Obj.uGenre = no.uGenre;
				Obj.Obj.uId    = no.uId;
				Obj.Region.h   = Obj.Region.v = 0;
				Obj.Region.Width = Obj.Region.Height = 0;
				Obj.nContainer = UOC_ITEM_GIVE;

				ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
					GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
				//if (eProp == IIEP_NORMAL) //可用
				//else 
				if (eProp == IIEP_NOT_USEABLE)  //不可用
				{
					ccColor3B color_ = ccc3(128, 0, 0);
					bgcolorLayer->setColor(color_);
				}
				//else if (eProp == IIEP_SPECIAL) //其他特殊物品
			}
		}
	}
	return true;
}
//增加物品
int KuiPlayerTrade::AddObject(KUiDraggedObject* pObject, int nCount)
{
	if (!pObject) return false;

	for (int i = 0; i < nCount; ++i)
	{
			KUiDraggedObject no;
			no.uGenre = pObject->uGenre;
			no.uId    = pObject->uId;
			no.DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
			no.DataY  = pObject->DataY;     //包袱中的起点Y位置
			no.DataW  = pObject->DataW; //物品的高度(占用了多少个格子)
			no.DataH  = pObject->DataH;//物品的宽度(占用了多少个格子)
			if (Item && pObject->uId>0 && pObject->uId<MAX_ITEM)
			{
				char nItemSprPath[256];
				ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
				if  (Item[pObject->uId].GetImagePath())
					t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
				else
					t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
				if (nItemSprPath[0])
				{
					//t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());
					g_StrLower(nItemSprPath);
					int m_nWidth,m_nHeight,nFrams;
					CCTexture2D *bgCur = NULL;
					SPRFRAMSINFO nSprInfo;
					ZeroMemory(&nSprInfo,sizeof(nSprInfo));
					bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
					if (bgCur==NULL)
						return false;
					char nItemKey[32];
					ZeroMemory(&nItemKey,sizeof(nItemKey));
					t_sprintf(nItemKey,"item_%u",Item[pObject->uId].GetID());
					std::string nKey =nItemKey;
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

						if  (Item[pObject->uId].GetGenre()!=item_equip && Item[pObject->uId].IsStack())
						{//不是装备 可叠加物品
							char stack[32];
							t_sprintf(stack,"%d",Item[pObject->uId].GetStackNum());
							CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
							stuckCountlabel->setColor(ccYELLOW);
							stuckCountlabel->setAnchorPoint(ccp(0,0));
							stuckCountlabel->setPosition(ccp(0,0));
							stuckCountlabel->setTagbyKey("stack");
							nItemSpr->addChild(stuckCountlabel,1);
						}
					}
					//开始填充书库
					m_ItemData[nKey].uGenre = pObject->uGenre;
					m_ItemData[nKey].uId    = pObject->uId;
					m_ItemData[nKey].nindex = i;
					m_ItemData[nKey].DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
					m_ItemData[nKey].DataY  = pObject->DataY;     //包袱中的起点Y位置
					m_ItemData[nKey].DataW  = pObject->DataW; //物品的高度(占用了多少个格子)
					m_ItemData[nKey].DataH  = pObject->DataH;//物品的宽度(占用了多少个格子)
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
					if (!bgcolorLayer)
					{
						bgcolorLayer = CCLayerColor::create(color);//颜色层
					    //bgcolorLayer->setPosition(CCPointZero);
						bgcolorLayer->setAnchorPoint(ccp(0,0));
						CCSize bgsize;
						bgsize.width  = no.DataW*ITEM_CELL_SIZE-2;
						bgsize.height = no.DataH*ITEM_CELL_SIZE-2;
					    bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
						bgcolorLayer->setTagbyKey(nKey);
						ParentNode_Item->addChild(bgcolorLayer,0);
					}

					int nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
					int nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;
					
					nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
					//颜色层
					nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+1;
					nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
					bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

					KUiObjAtContRegion	Obj;
					Obj.Obj.uGenre = no.uGenre;
					Obj.Obj.uId    = no.uId;
					Obj.Region.h   = Obj.Region.v = 0;
					Obj.Region.Width = Obj.Region.Height = 0;
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

					ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
					    GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
					//if (eProp == IIEP_NORMAL) //可用
					//else 
					if (eProp == IIEP_NOT_USEABLE)  //不可用
					{
						ccColor3B color_ = ccc3(128, 0, 0);
						bgcolorLayer->setColor(color_);
					}
					//else if (eProp == IIEP_SPECIAL) //其他特殊物品
				}
			}
	}
	return true;
}

int KuiPlayerTrade::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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
