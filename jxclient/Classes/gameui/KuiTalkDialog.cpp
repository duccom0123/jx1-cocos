#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiTalkDialog.h"
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

KuiTalkDialog::KuiTalkDialog()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	pInformationLabel       = NULL;
	ParentNode_ItemEx       = NULL;
	colorLayer        = NULL;
	red               = NULL;

	//m_nMoney = 0;
	//m_nXu = 0;
	m_AnswerCount = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	m_pWordDataList = NULL;
	m_nNumWordData = 0;
	m_nCurrentWord = 0;
}

KuiTalkDialog::~KuiTalkDialog()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
	colorLayer        = NULL;
	red->release();
	//red               = NULL;
}

//����ȷ��������
KuiTalkDialog * KuiTalkDialog::create(KUiInformationParam* pWordDataList, int nCount,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiTalkDialog * popLayer = KuiTalkDialog::create();
	//popLayer->setTitle(title);
	popLayer->addskillspr(pWordDataList,nCount);
	//���ð�ť
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//��ʼ��
bool KuiTalkDialog::init()
{
	if ( !CCLayer::init() ){
		return false;
	}

	//����֡����
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");

	//����ģ̬����
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	/*ccColor4B color = ccc4(112, 128, 144, 150);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//��ɫ��
	colorLayer->setPosition(CCPointZero);
	//colorLayer->setContentSize(winSize);//���úʹ��ڵĴ�С
	this->addChild(colorLayer);
	*/
	//���������ı���ͼƬ������������λ��Ϊ��Ļ�е�
	//CCSpriteFrame * f_tips_box = sfCache->spriteFrameByName("ui/state/skill.png");
	//CCSprite * spriteBox = CCSprite::create("ui/state/skill.png");//CCSprite::createWithSpriteFrame(f_tips_box);
	//\\spr\\Ui3\\������\\���������.spr
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\NPC�Ի���\\�Ի���2.spr");
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
	
	//��ñ���ͼƬ�Ĵ�С
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	//colorLayer->setContentSize(contentSize);//���úʹ��ڵĴ�С
	//����������Ӧ
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //���ú���
	//this->setTouchMode(kCCTouchesOneByOne); //��ֹ��㴥��
	return true;
}

void KuiTalkDialog::addskillspr(KUiInformationParam* pWordDataList, int nCount)
{
	if  (!g_pCoreShell) return;

	if (pWordDataList == NULL || nCount <= 0)
		return;

	CCSize nSize;
	nSize.width =380;
	nSize.height=120;
	m_StartPos.x =25;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =30;//+ITEM_CELL_SIZE/2;

	pInformationLabel = CCLabelTTF::create("",UI_GAME_FONT_DEFAULT,12,nSize,kCCTextAlignmentLeft);//Arial
	pInformationLabel->setAnchorPoint(ccp(0,0));
	//pInformationLabel->setContentSize(nSize);
	pInformationLabel->setPosition(ccp(m_StartPos.x,m_size.height-m_StartPos.y-nSize.height));
	ParentNode_ItemEx->addChild(pInformationLabel,1);

	KUiInformationParam* pNewList = (KUiInformationParam*)malloc(sizeof(KUiInformationParam) *(nCount + m_nNumWordData - m_nCurrentWord));
	if (pNewList == NULL)
		return;
	if (m_nNumWordData > m_nCurrentWord)
	{
		memcpy(pNewList, m_pWordDataList, sizeof(KUiInformationParam) * (m_nNumWordData - m_nCurrentWord));
		m_nCurrentWord = m_nNumWordData - m_nCurrentWord;
	}
	else
		m_nCurrentWord = 0;
	memcpy(&pNewList[m_nCurrentWord], pWordDataList, sizeof(KUiInformationParam) * nCount);
	m_nNumWordData = m_nCurrentWord + nCount;

	if (m_pWordDataList)
		free(m_pWordDataList);

	m_pWordDataList = pNewList;

	if (isOpen == false)
	{//������ǻ��,ֻ��ʾһ��
		//KWndWindow*	pCaller = m_pWordDataList->bNeedConfirmNotify ? ((KWndWindow*)WND_GAMESPACE) : 0;
		Show(m_pWordDataList->sInformation, m_pWordDataList->nInforLen,
			m_pWordDataList->sConfirmText,0);
		m_nCurrentWord ++;
	}
	//ParentNode_ItemEx->addChild(chatRickText,1);
	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//��ɫ��
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//���úʹ��ڵĴ�С
	ParentNode_ItemEx->addChild(colorLayer,1000);
	//����һ������ �ı侫����ɫ
	red = CCTintBy::create(0.2,0,-255,-255);//����ʱ��+��ɫ 
	red->retain();
}

void KuiTalkDialog::Show(char* pInformation, int nInforLen, const char* pBtnLabel, unsigned int uParam)
{//��ʼ��������
	if (pInformation && pInformationLabel)
	{
		int msgLen  = FilterTextColor(pInformation,strlen(pInformation));
		pInformation[msgLen]='\0';
		char nNewInformation[256];
		t_sprintf(nNewInformation,pInformation);
        pInformationLabel->setString(UTEXT(nNewInformation,1).c_str());
	}
}

//����ȷ��ȡ����ť,���������ò���󣬵��ò�ص�����
void KuiTalkDialog::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	return;

	CCSprite * sprite_tips_btn_confirm_normal =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	//CCSpriteFrame * f_tips_btn_confirm_select = sfCache->spriteFrameByName("btn_confirm_select.png");
	CCSprite * sprite_tips_btn_confirm_select =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_select);
	CCMenuItemSprite * itemConfirm = CCMenuItemSprite::create(sprite_tips_btn_confirm_normal, sprite_tips_btn_confirm_select, this, menu_selector(KuiTalkDialog::buttonCallBackFunc));
	itemConfirm->setPosition(ccp(0,0));
	CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	CCMenu * menu = CCMenu::create(itemConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//�˵�λ������Ϊ����������
	menu->setPosition(ccp(50,m_size.height-sprite_tips_btn_confirm_select->getContentSize().height));//�˵�λ������Ϊ����������
	ParentNode_ItemEx->addChild(menu);         //�ӵ�������
}
//ִ���ϲ����Ļص��������رյ�����
void KuiTalkDialog::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = dynamic_cast<CCNode *>(pSender);
	node->setTag(3000);//����tag���ڵ��ò���Ի�ȡ��

	if (m_callfun &&  m_callbackListener)
	{
		//ִ�е��ò�ص����������ݲ���CCNode
		(m_callbackListener->*m_callfun)(node);
	}

	this->removeFromParentAndCleanup(true);
}

//�رյ�����
void KuiTalkDialog::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//��д����ע�ắ�������¸�����������
void KuiTalkDialog::registerWithTouchDispatcher()
{
	//����Ĵ������ȼ�����Ϊ��128����CCMenuͬ������֤�������·��Ĵ���
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
}

CCRect KuiTalkDialog::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //��������ͼ��ƫ��
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //��������
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //���ϱ�    //- rc.origin.y
	
	return rc;
}

void KuiTalkDialog::changeMsg()
{//��ʼ�ı���Ϣ����
	if (m_nCurrentWord < m_nNumWordData)
	{
		//KWndWindow*	pCaller = m_pWordDataList[m_nCurrentWord].bNeedConfirmNotify ?
		//	((KWndWindow*)WND_GAMESPACE) : 0;
		Show(m_pWordDataList[m_nCurrentWord].sInformation,
			m_pWordDataList[m_nCurrentWord].nInforLen,
			m_pWordDataList[m_nCurrentWord].sConfirmText, 0);
		m_nCurrentWord ++;
		return;
	}

	if (m_pWordDataList && m_nCurrentWord >=m_nNumWordData)
	{//��ʼ�رմ���
		m_nCurrentWord--;
		if (m_pWordDataList[m_nCurrentWord].bNeedConfirmNotify) //�лص�����
			g_pCoreShell->OperationRequest(GOI_INFORMATION_CONFIRM_NOTIFY, 0, 0);

		m_nNumWordData = 0;
		m_nCurrentWord = 0;
		free(m_pWordDataList);
		m_pWordDataList = NULL;
		//CCNode * node = dynamic_cast<CCNode *>(pInformationLabel);
		if (m_callfun &&  m_callbackListener)
		{
			//ִ�е��ò�ص����������ݲ���CCNode
			(m_callbackListener->*m_callfun)(NULL);
		}
		isOpen = false;
		this->removeFromParentAndCleanup(true);
	}
}
//��������ccTouchBegan������true
bool KuiTalkDialog::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	changeMsg();
	return true;

	CCPoint location = pTouch->getLocation();//Ŀ�ĵ�����?��Ļ�����
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
			{//������
				//restartAction();
				sprNode->runAction(red);

#ifndef WIN32
                 if (i==m_AnswerCount-1)
				 {
					 CCNode * node = dynamic_cast<CCNode *>(sprNode);
					 if (m_callfun &&  m_callbackListener)
					 {
						 //ִ�е��ò�ص����������ݲ���CCNode
						 (m_callbackListener->*m_callfun)(node);
					 }
					 this->removeFromParentAndCleanup(true);
					 return true;
				 }
#endif

				if (g_pCoreShell)
					g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, i);  //ִ�нű�����
				//colorLayer->setVisible(true);
				//colorLayer->setContentSize(sprNode->getContentSize());
				//colorLayer->setPosition(sprNode->getPosition());
				//char msg[64];
				//t_sprintf(msg,"����:%d",i+1);
				//CCMessageBox(msg,"����");
			}
			else
			{
				sprNode->setColor(ccWHITE);           //Ĭ����ɫ
				//colorLayer->setContentSize(CCSizeZero);
			}
		}
	}
	return true;
}

void KuiTalkDialog::update(float delta)
{
	//CCMessageBox("����ѭ��","update");
	if (isOpen && g_pCoreShell)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //	m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiTalkDialog::draw()
{
	//CCMessageBox("����ѭ��","draw");
}


int KuiTalkDialog::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
{
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_ENTER);          //��ֹ�س�����
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR);          //��ֹ��ɫ����
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR_RESTORE);  //��ֹ��ɫ��ԭ
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_COLOR);   //��ֹ�������
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_RESTORE); //��ֹ�������
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_INLINE_PIC);     //��ֹ��ͼ����
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_LINK);           //��ֹ��������
	//	return TEncodeText(pMsgBuff, nMsgLength);
	return  nMsgLength;
}