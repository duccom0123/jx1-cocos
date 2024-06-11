#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiChatList.h"
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
#include "engine/Text.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;
#define MSG_MAX_COUNT	10
KuiChatList::KuiChatList()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	//pMoneyLabel       = NULL;
	//pXuLabel          = NULL;
	//colorLayer        = NULL;
	red               = NULL;

	//m_nMoney = 0;
	//m_nXu = 0;
	//m_nNumObjects = 0;
	//m_StartPos.x=0;
	//m_StartPos.y=0;
	m_pUiLayer = NULL;
	//m_pWidget  = NULL;
	//m_array    = NULL;
	listView   = NULL;
	nPlayerFun = NULL;
}

KuiChatList::~KuiChatList()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
	//colorLayer   = NULL;
	//CC_SAFE_RELEASE(m_array);
	if (red)
	    red->release();
}

//创建确定弹出框
KuiChatList * KuiChatList::create(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiChatList * popLayer = KuiChatList::create();
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiChatList::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");
	//添加模态=
   	winSize = CCDirector::sharedDirector()->getWinSize();
	ccColor4B color = ccc4(0,0,0,150);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	//CCSprite * colorLayer = CCSprite::create("ui/background2.png");
	colorLayer->setOpacity(150);
	bgSize  = CCSizeMake(281,100);
	colorLayer->setContentSize(bgSize);
	ParentNode_Team= colorLayer;
	colorLayer->setPosition(ccp(winSize.width/2-bgSize.width/2,40));
	this->addChild(colorLayer);
	//获得背景图片的大小
	CCSize contentSize = colorLayer->getContentSize();
	m_size = contentSize;
	m_origin = colorLayer->getPosition();

	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	//开启触摸响应
	//this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}


void KuiChatList::addmsg(char * sendName,char * contentt,int nKind,int nColor,int nPackage)
{
	if  (!listView) return;

	CCArray *items = listView->getItems();
	int items_count = items->count();
	if (items_count>=MSG_MAX_COUNT/2)
	{
		listView->removeItem(0);//删除第一条
		listView->refreshView();
	}

	CCSize nRenderSize;
	nRenderSize.width  = bgSize.width*1.83;
	nRenderSize.height = 12;
	RichText  * chatRickText = RichText::create();//WithParentSize(nParentSize);
	chatRickText->setSize(nRenderSize);
	chatRickText->ignoreContentAdaptWithSize(false);//是否忽略内容适应高宽度  就一行
	chatRickText->setAnchorPoint(ccp(0,0));
    
	ccColor3B ccNameColor={255,255,255};
	ccColor3B ccContColor={255,255,255};
	ccColor3B ccGREEN_ ={98,255,255};
	ccColor3B ccBlue_ ={100,100,255};
	ccColor3B ccZIse_ ={188,64,255};

	switch(nKind)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		{
			ccNameColor=ccRED;
			ccContColor=ccYELLOW;
		}
		break;
	case 4:
		{//世界
			ccNameColor=ccGREEN;
			ccContColor=ccGREEN;
		}
		break;
	case 5:
		{//城市
			ccNameColor=ccGREEN_;
			ccContColor=ccGREEN_;
		}
		break;
	case 6:
		{//帮会
			ccNameColor=ccYELLOW;
			ccContColor=ccYELLOW;
		}
		break;
	case 7:
		{//队伍
			ccNameColor=ccBlue_;
			ccContColor=ccBlue_;
		}
		break;
	case 8:
		{//门派
			ccNameColor=ccYELLOW;
			ccContColor=ccYELLOW;
		}
		break;
	case 9:
		{//系统
			ccNameColor=ccRED;
			ccContColor=ccYELLOW;
		}
		break;
	case 10:
		{//附近聊天
			ccNameColor=ccWHITE;
			ccContColor=ccWHITE;
		}
		break;
	case 11:
		{//其他的信息
			ccNameColor=ccRED;
			ccContColor=ccRED;
		}
		break;
	case 12:
		{//私聊天
			if (nColor)
			{//己方
				ccNameColor=ccORANGE;
				ccContColor=ccORANGE;
			}
			else
			{//对方
				ccNameColor=ccMAGENTA;
				ccContColor=ccMAGENTA;
			}
		}
		break;
	default:
		{
			ccNameColor=ccRED;
			ccContColor=ccYELLOW;
		}
		break;
	}

	RichElementText* senname = RichElementText::create(1,ccNameColor,255,sendName,UI_GAME_FONT_DEFAULT,12); 
	RichElementText* Msg;//    = RichElementText::create(2,ccContColor,255,UTEXT(contentt,1).c_str(),UI_GAME_FONT_DEFAULT,12); 
	
	//if  (nPackage==100000000)
	//	Msg     = RichElementText::create(2,ccContColor,255,contentt,UI_GAME_FONT_DEFAULT,12); 
	//else
	    Msg     = RichElementText::create(2,ccContColor,255,contentt,UI_GAME_FONT_DEFAULT,12); 
		
	chatRickText->pushBackElement(senname);
	chatRickText->pushBackElement(Msg);

	
	nRenderSize = chatRickText->getRenderSize();
	nRenderSize.width = bgSize.width-20;
	chatRickText->setSize(nRenderSize);

	Layout *custom_item = Layout::create();
	custom_item->setSize(nRenderSize);
	chatRickText->setPosition(ccp(4,nRenderSize.height-12));
	custom_item->addChild(chatRickText);
	listView->pushBackCustomItem(custom_item);

	listView->refreshView();
	listView->jumpToBottom();

}

//信息到达 开始分类信息
int KuiChatList::NewChannelMessageArrival(DWORD nChannelID, char* szSendName,char* pMsgBuff, unsigned short nMsgLength,DWORD nItemDwidx,char *scrSendName,int nPackage)
{
	if  (!nPlayerFun) return false;
	int nChannelIndex =nPlayerFun->FindActivateChannelIndex(nChannelID);

	int nID = nPlayerFun->GetChannelResourceIndex(nChannelIndex);//m_pActivateChannel[nChannelIndex].ResourceIndex;
	char nTempSendName[64];
	ZeroMemory(nTempSendName,sizeof(nTempSendName));
	if (nChannelIndex >= CH_NEARBY) //全部  加 附近频道
	{
		/*if  (strstr(szSendName,"公告"))
		    addmsg(szSendName,pMsgBuff,pListView_all,0);
		else */
		if (strstr(szSendName,"Chú ý:"))
		{
			//nMsgLength = FilterTextColor(pMsgBuff,nMsgLength);
			addmsg(szSendName,pMsgBuff,1,0,nPackage);
		}
	    else if (strstr(szSendName,"Chiến trường:"))
		{
			//nMsgLength = FilterTextColor(pMsgBuff,nMsgLength);
		    addmsg(szSendName,pMsgBuff,2,0,nPackage);
		}
		else if (strstr(szSendName,"Khiêu chiến:"))
		{
			//nMsgLength = FilterTextColor(pMsgBuff,nMsgLength);
			addmsg(szSendName,pMsgBuff,3,0,nPackage);
		}
	    else
		{   
			
			if (nID == CH_WORLD) //世界
			{
				t_sprintf(nTempSendName,"[Thế giới]%s:",szSendName);
			    addmsg(nTempSendName,pMsgBuff,4,0,nPackage);
			}
			else if (nID == CH_CITY)//城市
			{	
				t_sprintf(nTempSendName,"[Thành thị]%s:",szSendName);
				addmsg(nTempSendName,pMsgBuff,5,0,nPackage);
			}
			else if (nID == CH_TONG) //帮派
			{
				t_sprintf(nTempSendName,"[Bang hội]%s:",szSendName);
				addmsg(nTempSendName,pMsgBuff,6,0,nPackage);
			}
			else if (nID == CH_TEAM)
			{
				t_sprintf(nTempSendName,"[Nhóm]%s:",szSendName);
				addmsg(nTempSendName,pMsgBuff,7,0,nPackage);
			}
			else if (nID == CH_FACTION)
			{
				t_sprintf(nTempSendName,"[Môn phái]%s:",szSendName);
				addmsg(szSendName,pMsgBuff,8,0,nPackage);
			}
			else if (nID == CH_SYSTEM)
			{
				t_sprintf(nTempSendName,"%s:",szSendName);
				addmsg(nTempSendName,pMsgBuff,9,0,nPackage);
			}
			else if (nID == CH_NEARBY)
			{//附近聊天
				t_sprintf(nTempSendName,"[Cận]%s:",szSendName);
				addmsg(nTempSendName,pMsgBuff,10,0,nPackage);
			}
			else 
			{
				//nMsgLength = FilterTextColor(pMsgBuff,nMsgLength);
				addmsg(szSendName,pMsgBuff,11,0,nPackage);
			}
			
	    }
	}  

	return true;
}

void KuiChatList::addDialogData()
{
	if  (!g_pCoreShell) return;

	btnSize.width = 70;
	btnSize.height= 14;

	m_pUiLayer = UILayer::create();
	ParentNode_Team->addChild(m_pUiLayer);

	/*m_pWidget = Layout::create();
	m_pWidget->setBackGroundImage("loadingEnd.png");
	m_pWidget->setBackGroundImageScale9Enabled(true);
	m_pWidget->setBackGroundColorOpacity(105);
	m_pWidget->setContentSize(m_size);
	m_pUiLayer->addWidget(m_pWidget);
	*/
	// create list view data
	/*m_array = CCArray::create();
	CC_SAFE_RETAIN(m_array);
	for (int i = 0; i < MSG_MAX_COUNT; ++i)
	{
		CCString* ccstr = CCString::createWithFormat("玩家列表_%d", i);
		m_array->addObject(ccstr);
	}*/
	//CCSize widgetSize = m_pWidget->getSize();
	// Create the list view
	listView = UIListView::create();
	// listView->setBackGroundImage("ui/btn_sel.png");
	// listView->setBackGroundImageScale9Enabled(true);
	// listView->setBackGroundImageOpacity(150);
	// set list view direction
	listView->setDirection(SCROLLVIEW_DIR_VERTICAL); //
	CCSize nTempListSize;
	nTempListSize.width  = bgSize.width;
	nTempListSize.height = bgSize.height-1;
	listView->setSize(nTempListSize);//
	//listView->setInnerContainerSize(CCSizeMake(110, 110));
	listView->setPosition(ccp(5,0));
	listView->setTouchEnabled(true);
	//listView->setDirection(SCROLLVIEW_DIR_HORIZONTAL);
	listView->setBounceEnabled(true);
	listView->addEventListenerListView(this, listvieweventselector(KuiChatList::selectedItemEvent));
	m_pUiLayer->addWidget(listView);

	//int count = m_array->count();
	// add custom item
	CCSize nRenderSize;
	nRenderSize.width  = bgSize.width-5;
	nRenderSize.height = 12;
	for (int i = 0; i < 2; ++i)
	{
		RichText  * chatRickText = RichText::create();//WithParentSize(nParentSize);
		chatRickText->setSize(nRenderSize);
		chatRickText->ignoreContentAdaptWithSize(false);//是否忽略内容适应高宽度  就一行
		chatRickText->setAnchorPoint(ccp(0,0));
		RichElementText* Msg     = RichElementText::create(1,ccGREEN,255,"JX 1 Mobile VN Team",UI_GAME_FONT_DEFAULT_VN,14); 
		chatRickText->pushBackElement(Msg);
		//chatRickText->RendererElement();  
		nRenderSize = chatRickText->getRenderSize();
		chatRickText->setSize(nRenderSize);
		//chatRickText->setTouchEnabled(true);
		//}
		Layout *custom_item = Layout::create();
		custom_item->setSize(nRenderSize);
		custom_item->setAnchorPoint(ccp(0,0));
		chatRickText->setPosition(ccp(-20,nRenderSize.height));  //custom_item->getSize().width / 2, custom_item->getSize().height/2
		custom_item->addChild(chatRickText);
		listView->pushBackCustomItem(custom_item);
	}
	listView->setGravity(LISTVIEW_GRAVITY_LEFT);
	listView->refreshView();
	listView->jumpToBottom();

	/*ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);  //设置和窗口的大小
	ParentNode_Team->addChild(colorLayer,1000);
	*/
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiChatList::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	
	return;
}

//执行上层对象的回调函数，关闭弹出框
void KuiChatList::buttonCallBackFunc(CCObject * pSender)
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
void KuiChatList::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


void KuiChatList::selectedItemEvent(CCObject *pSender, ListViewEventType type)
{
	char msg[64];
	switch (type)
	{
	case LISTVIEW_ONSELECTEDITEM_START:
		{
			UIListView* listViewEx = static_cast<UIListView*>(pSender);
			if (listViewEx)
			{
				t_sprintf(msg,"Bắt đầu:%d",listViewEx->getCurSelectedIndex());
			}
				//CCLOG("select child start index = %d", listViewEx->getCurSelectedIndex());
			break;
		}
	case LISTVIEW_ONSELECTEDITEM_END:
		{
			UIListView* listViewEx = static_cast<UIListView*>(pSender);
			if (listViewEx)
				t_sprintf(msg,"Kết thúc:%d",listViewEx->getCurSelectedIndex());
				//CCLOG("select child end index = %d", listViewEx->getCurSelectedIndex());
			break;
		}
	default:
		break;
	}

	//CCMessageBox(msg,"点击事件");
}


//重写触摸注册函数，重新给定触摸级别
void KuiChatList::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2,true);
}

void KuiChatList::_removeDelegate(bool isThis)
{
	if  (!listView) return;
	listView->setTouchEnabled(isThis);
	listView->setBounceEnabled(isThis);
	this->setTouchEnabled(isThis);
	if  (!isThis)
		ParentNode_Team->setPosition(ccp(winSize.width/2-bgSize.width/2,40));
	else
	    ParentNode_Team->setPosition(ccp(winSize.width/2-bgSize.width/2,40));

	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);//删除委托
}

CCRect KuiChatList::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiChatList::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}

void KuiChatList::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	}
}

void KuiChatList::draw()
{
	//CCMessageBox("无限循环","draw");
}