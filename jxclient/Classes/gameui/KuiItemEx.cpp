#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiItemEx.h"
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
#include "gamecore/KPlayer.h"
#include "gameui/KuiItemdesc.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;
#define MOVE_TIME 2
KuiItemEx::KuiItemEx()
{
	isOpen = false;
	m_bScrolling=false;
	__isCanMove=true;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	m_ItemData.clear();
	colorLayer        = NULL;
	//red               = NULL;
	nstrCurSelItemKey ="";
	m_nIstrue = false;
	m_nNumObjects = 0;
	m_descPadIsOpen =false;
	m_StartPos.x=0;
	m_StartPos.y=0;
	btnBackPic = NULL;
	nBakChickTime = 0;
}

KuiItemEx::~KuiItemEx()
{
	isOpen = false;
	m_ItemData.clear();
	nstrCurSelItemKey ="";
	colorLayer        = NULL;
	//if (red)
	//	red->release();
	//red               = NULL;
}

//创建确定弹出框
KuiItemEx * KuiItemEx::create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiItemEx * popLayer = KuiItemEx::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiItemEx::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\同伴背包\\同伴背包界面.spr");
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
	spriteBox->setPosition(ccp(winSize.width/2-100,winSize.height/2));
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

void KuiItemEx::addDialogData()
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
	ParentNode_ItemEx->addChild(pXuLabel,1);
	*/
	
	UpdateData();
	//CCTextureCache::sharedTextureCache()->addImage("");
	btnBackPic = CCSprite::create("ui/btn_3.png");
	btnBackPic->setAnchorPoint(ccp(0,0));
	btnBackPic->setPosition(ccp(m_size.width,m_size.height/2-btnBackPic->getContentSize().height/2));
	ParentNode_ItemEx->addChild(btnBackPic);

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
void KuiItemEx::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	CCSprite * sprite_colse_normal =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	//CCSpriteFrame * f_tips_btn_confirm_select = sfCache->spriteFrameByName("btn_confirm_select.png");
	CCSprite * sprite_colse_select =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_select);
	CCMenuItemSprite * closeConfirm = CCMenuItemSprite::create(sprite_colse_normal, sprite_colse_select, this, menu_selector(KuiItemEx::buttonCallBackFunc));
	closeConfirm->setPosition(ccp(0,0));
	//取
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\取_3.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_qu_normal =  CCSprite::createWithTexture(bgCur);

	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_qu_select=  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * quConfirm = CCMenuItemSprite::create(sprite_qu_normal, sprite_qu_select, this, menu_selector(KuiItemEx::quCallBackFunc));
	quConfirm->setPosition(ccp(m_size.width/2+20,m_size.height/2-5));

	//弃
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\弃_3.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_qi_normal =  CCSprite::createWithTexture(bgCur);

	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_qi_select=  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * qiConfirm = CCMenuItemSprite::create(sprite_qi_normal, sprite_qi_select, this, menu_selector(KuiItemEx::qiCallBackFunc));
	qiConfirm->setPosition(ccp(m_size.width/2+20,m_size.height/2-sprite_qi_select->getContentSize().height-10));

	//整理
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\整.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zheng_normal =  CCSprite::createWithTexture(bgCur);

	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zheng_select=  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * zenConfirm = CCMenuItemSprite::create(sprite_zheng_normal, sprite_zheng_select, this, menu_selector(KuiItemEx::zenliCallBackFunc));
	zenConfirm->setPosition(ccp(m_size.width/2+20,m_size.height/2-sprite_zheng_select->getContentSize().height*2-10*2));
	
	CCMenu * menu = CCMenu::create(closeConfirm,quConfirm,qiConfirm,zenConfirm,NULL);
	menu->setPosition(ccp(m_size.width/2,45));//菜单位置设置为弹出框左下
	ParentNode_ItemEx->addChild(menu);         //加到父层里
}

void KuiItemEx::zenliCallBackFunc(CCObject * pSender)
{
	if (!g_pCoreShell) return;

	if (g_pCoreShell->GetAutoplayid())
	{
#ifdef WIN32
		CCMessageBox("挂机中,禁止整理!","提示");
#else
		CCMessageBox(UTEXT("挂机中,禁止整理!",1).c_str(),UTEXT("提示",1).c_str());
#endif
		return;
	}

	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}

	if  (nstrCurSelItemKey.empty() || m_ItemData.count(nstrCurSelItemKey)<=0)
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
	if  (!m_nIstrue)
	{
#ifdef WIN32
		CCMessageBox("子母袋还没有开启","提示:");
#else
		CCMessageBox(UTEXT("子母袋还没有开启",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}

	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);

	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数
	
	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=MOVE_TIME)
	{//间隔２秒
		//nBakChickTime = nCurChicktime;
#ifdef WIN32
		CCMessageBox("整理的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("整理的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	//if (nBakChickTime<=0)
	nBakChickTime = nCurChicktime;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_EX;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipmentex); 
}

void KuiItemEx::qiCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}

	if  (nstrCurSelItemKey.empty()|| m_ItemData.count(nstrCurSelItemKey)<=0)
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
		CCMessageBox("该物品不能丢弃","提示:");
#else
		CCMessageBox(UTEXT("该物品不能丢弃",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}

	if  (!m_nIstrue)
	{
#ifdef WIN32
		CCMessageBox("子母袋还没有开启","提示:");
#else
		CCMessageBox(UTEXT("子母袋还没有开启",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}
	if  (Item[m_ItemData[nstrCurSelItemKey].uId].GetTime()->bYear>0 || Item[m_ItemData[nstrCurSelItemKey].uId].GetIsBang())
	{
		nstrCurSelItemKey="";
#ifdef WIN32
		CCMessageBox("限时/邦定物品禁止丢弃","提示:");
#else
		CCMessageBox(UTEXT("限时/邦定物品禁止丢弃",1).c_str(),UTEXT("提示:",1).c_str());
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

	bool isYes = false;
	if (g_pCoreShell && g_pCoreShell->GetAutoplayid())
		isYes = true;
	Player[CLIENT_PLAYER_INDEX].AutoAwayItem(Item[m_ItemData[nstrCurSelItemKey].uId].GetID(),isYes);
	nstrCurSelItemKey="";
}

void KuiItemEx::quCallBackFunc(CCObject * pSender)
{

	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}

	if  (nstrCurSelItemKey.empty() || m_ItemData.count(nstrCurSelItemKey)<=0)
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
		CCMessageBox("该物品不能储存","提示:");
#else
		CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}
	if  (!m_nIstrue)
	{
#ifdef WIN32
		CCMessageBox("子母袋还没有开启","提示:");
#else
		CCMessageBox(UTEXT("子母袋还没有开启",1).c_str(),UTEXT("提示:",1).c_str());
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
	Obj.eContainer    = UOC_ITEM_EX;
	nstrCurSelItemKey="";
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipment); 

}
//执行上层对象的回调函数，关闭弹出框
void KuiItemEx::buttonCallBackFunc(CCObject * pSender)
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
void KuiItemEx::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


//重写触摸注册函数，重新给定触摸级别
void KuiItemEx::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuiItemEx::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}

void KuiItemEx::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	m_bScrolling = false;

	//__loopTime = MAX_LOOP_TIME;
}

//移动中
void KuiItemEx::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling ||!__isCanMove) return;
	CCPoint point   = ParentNode_ItemEx->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCPoint diff    = ccpSub(point,m_lastPoint);                  //设置图片的位置
	ParentNode_ItemEx->setPosition(ccpAdd(ParentNode_ItemEx->getPosition(),diff));
	m_origin      = ParentNode_ItemEx->getPosition();
	m_lastPoint = point;
}
//触摸函数ccTouchBegan，返回true
bool KuiItemEx::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint point = ParentNode_ItemEx->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, winSize.width/*ParentNode_Team->getContentSize().width*/,winSize.height/* ParentNode_Team->getContentSize().height*/);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;
	if  (!m_nIstrue)
	{
		nstrCurSelItemKey ="";
#ifdef WIN32
		CCMessageBox("字母袋没有开启","提示");
#else
		CCMessageBox(UTEXT("字母袋没有开启",1).c_str(),UTEXT("提示",1).c_str());
#endif
		return true;
	}
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
			nCurColorlayer = (CCLayerColor *)ParentNode_ItemEx->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_ItemEx->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				if (getRect(nCurColorlayer,0).containsPoint(location))
				{//点中了
					if (SkillNode)
					{
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						m_ItemData[nItemKey].nRect  = getRect(nCurColorlayer,0);
						m_ItemData[nItemKey].nindex = -1;
						CCLayer * sDescLayer = KuiItemdesc::create(it->second,this,callfuncN_selector(KuiItemEx::closeItemPadCallback),0);
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

void KuiItemEx::closeItemPadCallback(CCNode *pNode)
{//在这个层中标记号，以确定是哪个按钮
	m_descPadIsOpen = false;
}

void KuiItemEx::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
		if (g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, GET_BOXEX_OPEN, 0))
			m_nIstrue = true;
		//	m_ItemEX.SetOpenClose(TRUE);
		else
			m_nIstrue = false;
	}
}

void KuiItemEx::draw()
{
	//CCMessageBox("无限循环","draw");
}

void KuiItemEx::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
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
			return;

		if (bAdd) //增加
		{
			//m_ItemBox.AddObject(&Obj,1);
			AddObject(&Obj,1);

		}
		else      //减少
		{
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_ItemEx->removeChildByTagKey(nItemKey,true);
			t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
			nItemKey = nKey;
			ParentNode_ItemEx->removeChildByTagKey(nItemKey,true);
			m_ItemData.erase(nItemKey);
		}
		//m_ItemBox.RemoveObject(&Obj);
		//UiSoundPlay(UI_SI_PICKPUT_ITEM);
		//UpdateData();
	}
	else //更新全部的物品
		UpdateData();
}
//增加物品
int KuiItemEx::AddObject(KUiDraggedObject* pObject, int nCount)
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

				if  (nItemSprPath[0])
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
					CCSprite *nItemSpr = (CCSprite *)ParentNode_ItemEx->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_ItemEx->addChild(nItemSpr,1);

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
					m_ItemData[nKey].nGenkind = UOC_ITEM_EX;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_ItemEx->getChildByTagKey(nKey);
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
						ParentNode_ItemEx->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_EX;

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
void KuiItemEx::UpdateData()
{
// m_CurSelItemIndex = -1;
   nstrCurSelItemKey ="";
  // memset(m_ItemData,0,sizeof(KUiItemdObject) * 60);
   _ItemData::iterator it;
   for( it = m_ItemData.begin(); it != m_ItemData.end(); it++)
   {
	   if (it->second.uId>0 && it->second.uId<MAX_ITEM)
	   {////重置某技能的加成
		   char nKey[32];
		   t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
		   std::string nItemKey = nKey;
		   ParentNode_ItemEx->removeChildByTagKey(nItemKey,true);
		   //nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
		   t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
		   nItemKey = nKey;
		   //SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
		   ParentNode_ItemEx->removeChildByTagKey(nItemKey,true);
		   m_ItemData.erase(it);
	   }
   }
   m_ItemData.clear();

	KUiObjAtRegion* pObjs = NULL;
	int nCount    = g_pCoreShell->GetGameData(GDI_ITEM_EX, 0, 0); //得到随身带物品的数量

	m_nNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_EX, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
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
					CCSprite *nItemSpr = (CCSprite *)ParentNode_ItemEx->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_ItemEx->addChild(nItemSpr,1);

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
					m_ItemData[nKey].nGenkind = UOC_ITEM_EX;         //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_ItemEx->getChildByTagKey(nKey);
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
						ParentNode_ItemEx->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_EX;

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