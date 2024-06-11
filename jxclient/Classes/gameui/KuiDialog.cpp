#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiDialog.h"
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

KuiDialog::KuiDialog()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	//pMoneyLabel       = NULL;
	//pXuLabel          = NULL;
	colorLayer        = NULL;
	red               = NULL;

	//m_nMoney = 0;
	//m_nXu = 0;
	m_AnswerCount = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
}

KuiDialog::~KuiDialog()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
	colorLayer        = NULL;
	if (red)
	   red->release();
	//red               = NULL;
}

//创建确定弹出框
KuiDialog * KuiDialog::create(KUiQuestionAndAnswer* pContent, KUiNpcSpr* pImage,int nIsTimeClose,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiDialog * popLayer = KuiDialog::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData(pContent,pImage,nIsTimeClose);
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiDialog::init()
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
	t_sprintf(nSprName,"\\Spr\\new\\bg_task_to.spr");
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

void KuiDialog::addDialogData(KUiQuestionAndAnswer* pContent, KUiNpcSpr* pImage,int nIsTimeClose)
{
	if  (!g_pCoreShell) return;
	CCSize nSize;
	nSize.width =1200;
	nSize.height=250;
	m_StartPos.x =24;    //+ITEM_CELL_SIZE/2;
	m_StartPos.y =195;//+ITEM_CELL_SIZE/2;
	ui::RichText* chatRickText = ui::RichText::createWithParentSize(m_size);
	chatRickText->setSize(nSize);
	chatRickText->ignoreContentAdaptWithSize(false);//是否忽略内容适应高宽度  就一行
	chatRickText->setAnchorPoint(ccp(0,0));
	chatRickText->setPosition(ccp(136,m_size.height-146));//m_size.height-146
	//开始创建按钮
	if (pContent && pContent->AnswerCount >= 0)
	{
		m_AnswerCount = pContent->AnswerCount;

		int nmsglen = FilterTextColor(pContent->Question,strlen(pContent->Question));
		pContent->Question[nmsglen]='\0';
		ui::RichElementText* re1=NULL;
		re1 = ui::RichElementText::create(1,ccYELLOW,255,UTEXT(pContent->Question,1).c_str(),UI_GAME_FONT_DEFAULT,12);
		chatRickText->pushBackElement(re1);
		ParentNode_ItemEx->addChild(chatRickText,1);

		int i = 0;
		if (pContent->AnswerCount <= 0)
		{
			//m_MsgScrollList.GetMessageListBox()->AddOneMessage("返回退出", -1,10); //选项信息
		}
		else
		{
			float m_PosX=0,m_PosY=0;
			for (i = 0; i < pContent->AnswerCount; ++i)
			{//动态创建选项节点
				CCSprite  *nAnswerspr=NULL;
				char mKey[32];
				t_sprintf(mKey,"answer_%d",i);
				std::string nKeyName = mKey;
				nAnswerspr = (CCSprite *)ParentNode_ItemEx->getChildByTagKey(nKeyName);
				if (!nAnswerspr)
				{
					nAnswerspr = CCSprite::create("ui/textbg.png");
					nAnswerspr->setAnchorPoint(ccp(0,0));
					nAnswerspr->setTagbyKey(nKeyName);

					ui::RichText* nTextLabel = ui::RichText::create();
					nTextLabel->setSize(nAnswerspr->getContentSize());
					nTextLabel->ignoreContentAdaptWithSize(false);//是否忽略内容适应高宽度  就一行
					nTextLabel->setAnchorPoint(ccp(0,0));
					ui::RichElementText* nCure1=NULL;
					int nfontsize = 14;
					nCure1 = ui::RichElementText::create(1,ccYELLOW,255,UTEXT(pContent->Answer[i].AnswerText,1).c_str(),UI_GAME_FONT_DEFAULT,nfontsize);
					nTextLabel->pushBackElement(nCure1);
					//相对父层的位置
					int nRows = nTextLabel->getRenderRows();
					int nOffsetX = strlen(pContent->Answer[i].AnswerText)*nfontsize/2;//文字的长度字节
					int nPosX    = nAnswerspr->getContentSize().width/2-nOffsetX/2;
					if (nRows>1)
					{
					    nOffsetX = 0;
						nPosX    = 0;
					}
					nTextLabel->setPosition(ccp(nPosX,-((nAnswerspr->getContentSize().height-nTextLabel->getContentSize().height)/2))); //(nAnswerspr->getContentSize().height-nTextLabel->getContentSize().height)/2
					nAnswerspr->addChild(nTextLabel,0);
					
					if (i==0)
					{//m_size
						m_PosX = m_StartPos.x;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height);
					}
					else if (i>0 &&i<4)
					{//第一行
						m_PosX += nAnswerspr->getContentSize().width+3;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height);
					}
					else if (i==4)
					{//第二行
						m_PosX = m_StartPos.x;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*2+3);
					}
					else if (i>4 &&i<8)
					{//第二行
						m_PosX += nAnswerspr->getContentSize().width+3;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*2+3);
					}
					else  if (i==8)
					{//第三行
						m_PosX = m_StartPos.x;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*3+3);
					}
					else  if (i>8 &&i<12)
					{//第三行
						m_PosX += nAnswerspr->getContentSize().width+3;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*3+3);
					}
					else  if (i==12)
					{//第四行
						m_PosX = m_StartPos.x;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*4+3);
					}
					else  if (i>12)
					{//第四行
						m_PosX += nAnswerspr->getContentSize().width+3;
						m_PosY = m_size.height-(m_StartPos.y+nAnswerspr->getContentSize().height*4+3);
					}

					nAnswerspr->setPosition(ccp(m_PosX,m_PosY));
					ParentNode_ItemEx->addChild(nAnswerspr,1);
				}
			}			
		}
	}
	

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_ItemEx->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	red->retain();
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiDialog::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	CCMenuItemImage * itemConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiDialog::buttonCallBackFunc));
	itemConfirm->setPosition(ccp(260,-415));
	CCLabelTTF * itemConfirmLabel = CCLabelTTF::create("Đóng lại",UI_GAME_FONT_VN, 11);
	itemConfirmLabel->setColor(ccWHITE);
	itemConfirmLabel->setAnchorPoint(ccp(0,0));
	itemConfirmLabel->enableStroke(ccc3(0,0,0),6);//vi?n ch? ?en
	itemConfirmLabel->setPosition(ccp(itemConfirm->getContentSize().width/2-20,itemConfirm->getContentSize().height/2-7));
	itemConfirm->addChild(itemConfirmLabel,1);
	CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	CCMenu * menu = CCMenu::create(itemConfirm,NULL);
	menu->setPosition(ccp(80,m_size.height-itemConfirm->getContentSize().height));
	ParentNode_ItemEx->addChild(menu);
}

//执行上层对象的回调函数，关闭弹出框
void KuiDialog::buttonCallBackFunc(CCObject * pSender)
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
void KuiDialog::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiDialog::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128,true);
}

CCRect KuiDialog::getRect(CCNode* pNode,int i)
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
bool KuiDialog::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * sprNode =NULL;
	
	for (int i = 0; i < m_AnswerCount; ++i)
	{
		char mKey[32];
		t_sprintf(mKey,"answer_%d",i);
		std::string nKeyName = mKey;
		sprNode = (CCSprite *)ParentNode_ItemEx->getChildByTagKey(nKeyName);
		if  (sprNode)
		{
			if (getRect(sprNode,i).containsPoint(location))
			{//点中了
				sprNode->runAction(red);
				CCNode * node = dynamic_cast<CCNode *>(sprNode);
				if (m_callfun &&  m_callbackListener)
					(m_callbackListener->*m_callfun)(node);

				this->removeFromParentAndCleanup(true);

				if (g_pCoreShell)
				    g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, i);  //执行脚本函数
			}
			else
			{
				sprNode->setColor(ccWHITE);           //默认颜色
			}
		}
	}
	return true;
}

void KuiDialog::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //	m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiDialog::draw()
{
	//CCMessageBox("无限循环","draw");
}


int KuiDialog::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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