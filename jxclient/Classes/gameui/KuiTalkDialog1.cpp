//有按钮的　ｔａｌｋ　对话框
//
#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiTalkDialog1.h"
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
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiTalkDialog1::KuiTalkDialog1()
{
	isOpen = false;
	colorLayer        = NULL;
	red               = NULL;
	__Kind      =-1;
	__nParam    =0;
	ZeroMemory(__strParam,sizeof(__strParam));
}

KuiTalkDialog1::~KuiTalkDialog1()
{
	isOpen = false;
	colorLayer = NULL;
	__nParam    =0;
}

//创建确定弹出框
KuiTalkDialog1 * KuiTalkDialog1::create(char * pContent, char *OkbtnStr,char *NobtnStr,int nKind,CCObject * callbackListener,SEL_CallFuncN callfun,unsigned int nParam,char *strParam)
{
	KuiTalkDialog1 * popLayer = KuiTalkDialog1::create();
	popLayer->__nParam = nParam;
	if (strParam)
	   t_sprintf(popLayer->__strParam,strParam);
	//popLayer->setTitle(title);
	popLayer->addDialogData(pContent,nKind);
	//设置按钮
	popLayer->setcoloseButton(OkbtnStr,NobtnStr,callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}


//初始化
bool KuiTalkDialog1::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\NPC对话条\\对话条2.spr");
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


void KuiTalkDialog1::addDialogData(char * pContent,int nKind)
{
	if  (!g_pCoreShell) return;
	CCSize nSize;
	nSize.width =256;
	nSize.height=24;
	char nTempStr[128];
	t_sprintf(nTempStr,pContent);
	__Kind = nKind;
	CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentLeft);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setAnchorPoint(ccp(0,0));
	pstrLabel->setPosition(ccp(30,m_size.height-50));
	ParentNode_ItemEx->addChild(pstrLabel);

	switch(nKind)
	{
	case SMCT_UI_TONG_JOIN_APPLY://申请入帮
	case SMCT_UI_RENASCENCE:
		{
			/*CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
			CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT("点击屏幕回程",1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
			ptextLabel->setColor(ccYELLOW);
			ptextLabel->setPosition(ccp(m_size.width/2,20));
			ParentNode_ItemEx->addChild(ptextLabel);*/
		}
	break;
	case SMCT_UI_TEAM_INVITE:
		{//应答入队请求

		}
		break;
	case SMCT_UI_TEAM_APPLY:
		{//同意或拒绝别人的入队申请
		}
		break;
	default:
		break;
	}
	
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
void KuiTalkDialog1::setcoloseButton(char *OkbtnStr,char *NobtnStr,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	m_pUiLayer = UILayer::create();
	ParentNode_ItemEx->addChild(m_pUiLayer);

	switch(__Kind)
	{
	case SMCT_UI_TONG_JOIN_APPLY://申请入帮
	case SMCT_UI_TRADE:
	case SMCT_UI_TEAM_APPLY:
	case SMCT_UI_TEAM_INVITE:
		{
			//同意
			char nOkStr[64],nNoStr[64];
			t_sprintf(nOkStr,OkbtnStr);
			t_sprintf(nNoStr,NobtnStr);
			
			UIButton* button = UIButton::create();
			button->setTouchEnabled(true);
			button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
			button->setTitleText(UTEXT(nOkStr,1).c_str());
			button->setPosition(ccp(m_size.width/2-70,30));
			button->setTag(__Kind);
			button->setTagbyKey("1");
			button->addTouchEventListener(this,toucheventselector(KuiTalkDialog1::touchEvent));
			m_pUiLayer->addWidget(button);
			//取消
			UIButton* _button = UIButton::create();
			_button->setTouchEnabled(true);
			_button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
			_button->setTitleText(UTEXT(nNoStr,1).c_str());
			_button->setPosition(ccp(m_size.width/2+70,30));
			_button->setTag(__Kind);
			_button->setTagbyKey("0");
			_button->addTouchEventListener(this,toucheventselector(KuiTalkDialog1::touchEvent));
			m_pUiLayer->addWidget(_button);
		}
		break;
	case SMCT_UI_RENASCENCE:
		{
			//回城修养
			UIButton* button = UIButton::create();
			button->setTouchEnabled(true);
			button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
			button->setTitleText(UTEXT("回城修养",1).c_str());
			button->setPosition(ccp(m_size.width/2,30));
			button->setTag(SMCT_UI_RENASCENCE);
			button->setTagbyKey("1");
			button->addTouchEventListener(this,toucheventselector(KuiTalkDialog1::touchEvent));
			m_pUiLayer->addWidget(button);
		}
		break;
	default:
		break;
	}

}
//执行上层对象的回调函数，关闭弹出框
void KuiTalkDialog1::buttonCallBackFunc(CCObject * pSender)
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
void KuiTalkDialog1::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiTalkDialog1::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
}

CCRect KuiTalkDialog1::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}

void KuiTalkDialog1::touchEvent(CCObject *pSender, TouchEventType type)
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
				switch (nTbtn->getTag())
				{
				   case SMCT_UI_TONG_JOIN_APPLY://申请入帮
					   {
						   /*
						   g_pCoreShell->TongOperation(GTOI_TONG_JOIN_REPLY,
						   (unsigned int)pPlayer, (nSelAction == 0));	//nSelAction=0：答应, nSelAction=1：拒绝
						   */
						   if (nTbtn->GetTagbyKey()=="1")
						   {//同意
							   g_pCoreShell->TongOperation(GTOI_TONG_JOIN_REPLY,__nParam,true,0,__strParam);	    //nSelAction=0：答应, nSelAction=1：拒绝
						   }else
							   g_pCoreShell->TongOperation(GTOI_TONG_JOIN_REPLY,__nParam,false,0,__strParam);	//nSelAction=0：答应, nSelAction=1：拒绝

						   //char nTemp[64];
						   //t_sprintf(nTemp,"%u:%s",__nParam,__strParam);
						   //CCMessageBox(UTEXT(nTemp,1).c_str(),"test");
					   }
					   break;
				   case SMCT_UI_TRADE:
				    {//申请交易的
						if (nTbtn->GetTagbyKey()=="1")
						{//同意
							g_pCoreShell->OperationRequest(GOI_TRADE_INVITE_RESPONSE,__nParam,true);	//nSelAction=0：答应, nSelAction=1：拒绝
						}else
                            g_pCoreShell->OperationRequest(GOI_TRADE_INVITE_RESPONSE,__nParam,false);	//nSelAction=0：答应, nSelAction=1：拒绝

				    }
					break;
				   case SMCT_UI_TEAM_APPLY:
					{//同意或拒绝别人的入队申请
						if (nTbtn->GetTagbyKey()=="1")
						{
							//CCMessageBox("dlalog","同意入队");
							g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE,__nParam,true);
						}
						else
							g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE,__nParam,false);	//nSelAction=0：答应, nSelAction=1：拒绝

					 }
					 break;
					case SMCT_UI_TEAM_INVITE://Touch Down
						{//自己拒绝或同意邀请入队的申请
							if (__nParam)
							{
								if (nTbtn->GetTagbyKey()=="1")
								    g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE,__nParam,true);
								else
                                    g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE,__nParam,false);	//nSelAction=0：答应, nSelAction=1：拒绝

							}
							//(unsigned int)sPlayer
						}
						break;
					case SMCT_UI_RENASCENCE://Touch Down
						{
							if (g_pCoreShell)
								g_pCoreShell->OperationRequest(GOI_PLAYER_RENASCENCE,0,true);//nSelAction=0：原地重生, nSelAction=1：回城	
							buttonCallBackFunc(NULL);
						}
						break;
					default:
						break;
				}
			}
		}
		//m_pDisplayValueLabel->setText(CCString::createWithFormat("Touch Down")->getCString());
		break;
	case TOUCH_EVENT_MOVED://Touch Move
		break;

	case TOUCH_EVENT_ENDED://Touch Up
		{
			if (pSender)
			{
				UIButton* nTbtn = (UIButton*)pSender;
				nTbtn->setTitleColor(ccWHITE);
				switch (nTbtn->getTag())
				{
				case SMCT_UI_TONG_JOIN_APPLY://申请入帮
				case SMCT_UI_TRADE:
				case SMCT_UI_TEAM_APPLY:
				case SMCT_UI_TEAM_INVITE://Touch Down
					{
						buttonCallBackFunc(NULL);
					}
					break;
				case SMCT_UI_RENASCENCE://重生
					{
					}
					break;
				default:
					break;
				}					
			}
		}
		break;
	case TOUCH_EVENT_CANCELED:
		break;

	default:
		break;
	}
}

//触摸函数ccTouchBegan，返回true
bool KuiTalkDialog1::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处	
	switch(__Kind)
	{
	case SMCT_UI_RENASCENCE:
		{//选择重生
			
		}
		break;
	case SMCT_UI_TEAM_INVITE:
		{
		}
		break;
	default:
		break;
	}
	return true;
}

void KuiTalkDialog1::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //	m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiTalkDialog1::draw()
{
	//CCMessageBox("无限循环","draw");
}

int KuiTalkDialog1::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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