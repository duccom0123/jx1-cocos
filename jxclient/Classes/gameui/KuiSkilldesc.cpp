#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiSkilldesc.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
#include "gamecore/KPlayer.h"
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
#include "engine/Text.h"
#include "gamecore/KSkillManager.h"
#include "gamecore/KItem.h"
#include "gameui/KuiItem.h"
#include "gameui/KuiItemEx.h"
#include "gameui/KuiShop.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiSkilldesc::KuiSkilldesc()
{
	isOpen = false;
	memset(&skillData,0,sizeof(KUiItemdObject));
	ptitleLabel       = NULL;
	potherLabel       = NULL;
	pdescLabel        = NULL;
	pScriptLabel      = NULL;
	pScriptLabel_n    = NULL;
	pbaseLabel        = NULL;   //基本属性
	prqLabel          = NULL;   //需求属性
	pmagicLabel       = NULL;   //魔法属性
	colorLayer        = NULL;
	red               = NULL;

	//m_nMoney = 0;
	//m_nXu = 0;
	//m_nNumObjects = 0;
	m_StartPos.x=0;
    _nKind = 0;
	m_StartPos.y=0;
	//m_ItemIndex = 0;
}

KuiSkilldesc::~KuiSkilldesc()
{
	isOpen = false;
	ptitleLabel = NULL;
	//m_ItemIndex = 0;
	colorLayer        = NULL;
	if (red)
	    red->release();
	//red               = NULL;
}

//创建确定弹出框
KuiSkilldesc * KuiSkilldesc::create(int nskillId,int nLevel,KUiItemdObject nKillData,CCObject * callbackListener,SEL_CallFuncN callfun,int nKind)
{
	KuiSkilldesc * popLayer = KuiSkilldesc::create();
	popLayer->skillData       = nKillData;
	popLayer->skillData.DataX = nLevel;
	popLayer->_nKind          = nKind;
	//popLayer->setTitle(title);
	//char msg[64];
	//t_sprintf(msg,"%d",popLayer->m_ItemData.uId);
	//CCMessageBox(msg,"KuiSkilldesc");
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}



//初始化
bool KuiSkilldesc::init()
{
	if ( !CCLayer::init() ){
		return false;
	}

	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");
	//添加模态背景
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	ccColor4B color = ccc4(0,0,0,150);
	colorLayer = CCLayerColor::create(color);//颜色层
	//colorLayer->setPosition(CCPointZero);
	colorLayer->setAnchorPoint(ccp(0,0));
	CCSize nColorSize;
	nColorSize.width = 200;
	nColorSize.height= 200;
	colorLayer->setContentSize(nColorSize);//设置和窗口的大小
	this->addChild(colorLayer);
	
	ParentNode_Team= colorLayer;
	
	colorLayer->setPosition(ccp(winSize.width/2,winSize.height/2)); //
	//this->addChild(spriteBox);
	
	//获得背景图片的大小
	CCSize contentSize = colorLayer->getContentSize();
	m_size   = contentSize;
	m_origin = colorLayer->getPosition();
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void KuiSkilldesc::addDialogData()
{
	if  (!g_pCoreShell) return;

	if (skillData.uId<=0 || skillData.uId>=MAX_SKILL) return;
	
	KSkill * pTempSkill = NULL;
	KSkill * pCurSkill = NULL;
	KSkill * pNextSkill = NULL;
	ccColor3B __ccBlue = {100,100,255};
	int  ulCurLevel = skillData.DataX;
	if(ulCurLevel == 0)
	{
		pNextSkill = (KSkill *)g_SkillManager.GetSkill(skillData.uId, 1);
		pTempSkill = pNextSkill;
	}
	else
	{
		pCurSkill = (KSkill *) g_SkillManager.GetSkill(skillData.uId, ulCurLevel);
		pNextSkill = (KSkill *) g_SkillManager.GetSkill(skillData.uId, ulCurLevel + 1);//下一等级
		pTempSkill = pCurSkill;
	} 

	if (pTempSkill == NULL)
	{
		return;
	}

	//int  i;
	char pszMsg[128];
	ZeroMemory(&pszMsg,sizeof(pszMsg));
	m_StartPos.x =24;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =72;//+ITEM_CELL_SIZE/2;
	CCSize nDsize;
	float offSet  = 0;
	nDsize.width  = m_size.width*2;
	nDsize.height = 0;
	int nTempOff =0;
//下个技能等级数据
    char nNextLevelData[640];
    ZeroMemory(nNextLevelData,sizeof(nNextLevelData));
    char *strnextlevel = nNextLevelData;
	bool bGetNextLevelDesc = true;
	if (skillData.uGenre != CGOG_SKILL_SHORTCUT && bGetNextLevelDesc)
	{
		if (pNextSkill)
		{
			strcat(strnextlevel, UTEXT("Cấp độ tiếp theo\n",1).c_str());
			if (pNextSkill->m_szDesc[0])
			{
				int newLen = TEncodeText_(pNextSkill->m_szDesc,strlen(pNextSkill->m_szDesc));
				pScriptLabel_n = CCLabelTTF::create(UTEXT(pNextSkill->m_szDesc,1).c_str(),UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);//Arial
				pScriptLabel_n->setColor(ccRED);
				pScriptLabel_n->setAnchorPoint(ccp(0,0));
				pScriptLabel_n->setPosition(ccp(0,offSet));
				ParentNode_Team->addChild(pScriptLabel_n,1);
				offSet +=pScriptLabel_n->getContentSize().height+1;
			}
			pNextSkill->GetDescAboutLevel(strnextlevel);	
			if  (strnextlevel[0])
			{
				CCLabelTTF *pNextLabel =NULL;
#ifdef WIN32
				pNextLabel = CCLabelTTF::create(UTEXT(strnextlevel,1).c_str(),UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);//Arial
#else
				pNextLabel = CCLabelTTF::create(strnextlevel,UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);//Arial
#endif
				pNextLabel->setAnchorPoint(ccp(0,0));
				pNextLabel->setPosition(ccp(0,offSet));
				pNextLabel->setColor(ccRED);
				ParentNode_Team->addChild(pNextLabel,1);
				offSet +=pNextLabel->getContentSize().height+1;
			}
		}
	}
    //技能限制
     char strLimited[128];
     ZeroMemory(strLimited,sizeof(strLimited));
	 strcat(strLimited,"Hạn chế: ");
	 char nSkillDesc[64]={0};
	 char nKey[8]={0};
	 int m_nEquiptLimited = pTempSkill->GetEquiptLimited();
	 if (m_nEquiptLimited==-2)
		 t_sprintf(nKey,"%s","F1");  //无限制
	 else if (m_nEquiptLimited==-1)
		 t_sprintf(nKey,"%s","F2");  //空手
	 else
		 t_sprintf(nKey,"%d",m_nEquiptLimited);

	 g_GameSetTing.GetString("WeaponLimit_m",nKey,"Không có",nSkillDesc,sizeof(nSkillDesc));
	 strcat(strLimited, nSkillDesc);
	 strcat(strLimited, "\n");
	 int  m_nHorseLimited = pTempSkill->GetHorseLimited();
	 if (m_nHorseLimited)  //骑马限制
	 {
		 switch(m_nHorseLimited)
		 {
		 case 1:
			 strcat(strLimited, "Không thể sử dụng kỹ năng này khi đang cưỡi ngựa\n");
			 break;
		 case 2:
			 strcat(strLimited, "Kỹ năng này chỉ có thể được sử dụng khi cưỡi ngựa\n");
			 break;
		 default:
			 break ;
		 }
	 }
	CCLabelTTF * pskilllitLabel = CCLabelTTF::create(strLimited,UI_GAME_FONT_DEFAULT, 14,nDsize,kCCTextAlignmentCenter);
	 
	 pskilllitLabel->setAnchorPoint(ccp(0,0));
	 pskilllitLabel->setPosition(ccp(0,offSet));
	 pskilllitLabel->setColor(__ccBlue);
	 ParentNode_Team->addChild(pskilllitLabel,1);
	 offSet +=pskilllitLabel->getContentSize().height+1;
	//当前等级数据
    char ncurLevelData[640];
	ZeroMemory(ncurLevelData,sizeof(ncurLevelData));
	char *strlevel = ncurLevelData;
    if (pCurSkill)
    {//但前技能
		//脚本的说明
	   if (pCurSkill->m_szDesc[0])
	   {
		   int newLen = TEncodeText_(pCurSkill->m_szDesc,strlen(pCurSkill->m_szDesc));
			pScriptLabel = CCLabelTTF::create(pCurSkill->m_szDesc,UI_GAME_FONT_DEFAULT, 14,nDsize,kCCTextAlignmentCenter);
		    pScriptLabel->setAnchorPoint(ccp(0,0));
		    pScriptLabel->setPosition(ccp(0,offSet));
		    ParentNode_Team->addChild(pScriptLabel,1);
		    offSet +=pScriptLabel->getContentSize().height+1;

	   }
//------但前等级数据开始
	   pCurSkill->GetDescAboutLevel(strlevel);	
	   if  (strlevel[0])
	   {
		   pbaseLabel = CCLabelTTF::create(strlevel,UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);
		   pbaseLabel->setAnchorPoint(ccp(0,0));
		   pbaseLabel->setPosition(ccp(0,offSet));
		   ParentNode_Team->addChild(pbaseLabel,1);
		   offSet +=pbaseLabel->getContentSize().height+1;
	   }
    }	
	//武功流派　 等级
	char szIntro[256],strSeries[128];
	ZeroMemory(&szIntro,sizeof(szIntro));
	ZeroMemory(&strSeries,sizeof(strSeries));
	nTempOff = 0;
	pdescLabel =CCNode::create();
	pdescLabel->setAnchorPoint(ccp(0,0));
	//prqLabel->setPosition(ccp(0,offSet));
	ParentNode_Team->addChild(pdescLabel,1);
	char szTemp[64];
	//if (skillData.uGenre != CGOG_SKILL_SHORTCUT)
	{
	  if (skillData.DataH)//如果有额外的技能点
		t_sprintf(szTemp, "cấp độ hiện tại:%d(%d+%d)",ulCurLevel , ulCurLevel - skillData.DataH,skillData.DataH);
	  else
		t_sprintf(szTemp, "cấp độ hiện tại:%d", ulCurLevel);
	  strcat(strSeries, szTemp);
	  strcat(strSeries, "\n");
	}

	if (pTempSkill->getUseAttackRate())
	{
		strcat(strSeries, "Đặc điểm kỹ năng：tỉ lệ chính xác");
		strcat(strSeries, "\n");
	}

	if (!_nKind && pTempSkill->getIsExpSkill())
	{//如果是 熟练度技能
		int  nCurExpSKill=0;
		//if (Npc[nOwnerIndex].m_SkillList.FindSame(ulSkillId))
		nCurExpSKill=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurSkillExp(skillData.uId);

		int nper =0;
		int m_nSKillExp = pTempSkill->GetSkillExp();
		if (m_nSKillExp)
			nper= nCurExpSKill/(m_nSKillExp/100);

		t_sprintf(szTemp, "Trình độ hiện tại:%d%s", nper,"%");
		strcat(strSeries, szTemp);
		strcat(strSeries, "\n");
	} 
	int  nEnChance = skillData.DataW;
	if (nEnChance)   //技能加成
	{
		t_sprintf(szTemp, "Tăng cường bởi các kỹ năng:+%d%s", nEnChance,"%");
		strcat(strSeries, szTemp);
		strcat(strSeries, "\n");
	}
	CCLabelTTF *tempLabel = CCLabelTTF::create(UTEXT(strSeries,1).c_str(),UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);//Arial
	tempLabel->setAnchorPoint(ccp(0,0));
	nTempOff -=tempLabel->getContentSize().height+1;
	tempLabel->setPosition(ccp(0,nTempOff));
	tempLabel->setColor(ccGREEN);
	pdescLabel->addChild(tempLabel);
	//武功流派
	int  m_eSkillStyle = pTempSkill->GetSkillStyle();
	//t_sprintf(szIntro,Item[m_ItemData.uId].GetItmeInfo());
	if (m_eSkillStyle == SKILL_SS_Melee||m_eSkillStyle ==SKILL_SS_PassivityNpcState||m_eSkillStyle ==SKILL_SS_InitiativeNpcState|| m_eSkillStyle == SKILL_SS_Missles)
	{
		int nAttribId = pTempSkill->IsAttrib();  //武功流派
		if (nAttribId <=1)
		{ 
			//pTempSkill->GetSkillName();
			if (pTempSkill->IsMagic())  //1 为内功系  0为外攻系
				strcat(szIntro, "Đòn đánh thường không trang bị vũ khí");
			else
				strcat(szIntro, "Đòn đánh thường không trang bị vũ khí");
		} 
		else
		{//技能描述 
			char nSkey[6]={0},sDecsInfo[125]={0};
			t_sprintf(nSkey,"%d",nAttribId);
			g_GameSetTing.GetString("SkillAttrib",nSkey,"Không có dữ liệu",sDecsInfo,sizeof(sDecsInfo));
			strcat(szIntro, sDecsInfo);
		} 
		strcat(szIntro,"\n");
	}

	if (szIntro[0])
	{
		
		t_sprintf(szIntro,UTEXT(szIntro,1).c_str());
		int newLen = TEncodeText(szIntro,strlen(szIntro));
		CCLabelTTF *tempLabel = CCLabelTTF::create(szIntro,UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);
		tempLabel->setAnchorPoint(ccp(0,0));
		nTempOff -=tempLabel->getContentSize().height+1;
		tempLabel->setPosition(ccp(0,nTempOff));
		tempLabel->setColor(ccYELLOW);
		pdescLabel->addChild(tempLabel);
	}

	CCSize nTemp;
	nTemp.width  = m_size.width;
	nTemp.height = -nTempOff;
	pdescLabel->setContentSize(nTemp);
	pdescLabel->setPosition(ccp(0,nTemp.height+offSet));
	offSet +=pdescLabel->getContentSize().height+1;
	////技能的描述--------------------------------------------
	char otherzMsg[256];
	ZeroMemory(&otherzMsg,sizeof(otherzMsg));
	strcat(otherzMsg, pTempSkill->m_szSkillDesc);	
	potherLabel = CCLabelTTF::create(UTEXT(otherzMsg,1).c_str(),UI_GAME_FONT_DEFAULT, 12,nDsize,kCCTextAlignmentCenter);//Arial
	potherLabel->setAnchorPoint(ccp(0,0));
	potherLabel->setPosition(ccp(0,offSet));
	ParentNode_Team->addChild(potherLabel,1);
	offSet +=potherLabel->getContentSize().height+1;
	//物品名称　等级
	ZeroMemory(&pszMsg,sizeof(pszMsg));
	t_sprintf(pszMsg,pTempSkill->GetSkillName());
    ptitleLabel = CCLabelTTF::create(UTEXT(pszMsg,1).c_str(),UI_GAME_FONT_DEFAULT,12,nDsize,kCCTextAlignmentCenter);//Arial
	ptitleLabel ->setColor(ccYELLOW);
	ptitleLabel->setAnchorPoint(ccp(0,0));
	ptitleLabel->setPosition(ccp(0,offSet));
	ParentNode_Team->addChild(ptitleLabel,1);

    m_size.height = offSet+ptitleLabel->getContentSize().height+1;
	ParentNode_Team->setContentSize(m_size);
	//移动整层的位置
	if (skillData.nRect.origin.y+m_size.height>visibleSize.height) //顶部
	{//如果超过了可见范围
		skillData.nRect.origin.y = visibleSize.height-m_size.height-10;
	}
	ParentNode_Team->setPosition(ccp(skillData.nRect.origin.x-m_size.width,skillData.nRect.origin.y));
	//创建一个动作 改变精灵颜色
	red = CCTintBy::create(0.2,0,-255,-255); //持续时间+颜色 
	red->retain();
}
//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiSkilldesc::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	return;

	CCSprite * sprite_tips_btn_confirm_normal =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	//CCSpriteFrame * f_tips_btn_confirm_select = sfCache->spriteFrameByName("btn_confirm_select.png");
	CCSprite * sprite_tips_btn_confirm_select =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_select);
	CCMenuItemSprite * itemConfirm = CCMenuItemSprite::create(sprite_tips_btn_confirm_normal, sprite_tips_btn_confirm_select, this, menu_selector(KuiSkilldesc::buttonCallBackFunc));
	itemConfirm->setPosition(ccp(0,0));
	CCPoint bgSpritePoint = ParentNode_Team->getPosition();
	CCMenu * menu = CCMenu::create(itemConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//菜单位置设置为弹出框左下
	menu->setPosition(ccp(m_size.width/2,45));//菜单位置设置为弹出框左下
	ParentNode_Team->addChild(menu);          //加到父层里
}

//执行上层对象的回调函数，关闭弹出框
void KuiSkilldesc::buttonCallBackFunc(CCObject * pSender)
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
void KuiSkilldesc::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


//重写触摸注册函数，重新给定触摸级别
void KuiSkilldesc::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuiSkilldesc::getRect(CCNode* pNode,int i)
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
bool KuiSkilldesc::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(NULL);
	this->removeFromParentAndCleanup(true);

	return true;
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	return true;
}

void KuiSkilldesc::update(float delta)
{
	//CCMessageBox("无限循环","update");
	/*if (isOpen && g_pCoreShell)
	{
		if (ptitleLabel)
		{

		}
	
	    //m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		//m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}*/
}

void KuiSkilldesc::draw()
{
	//CCMessageBox("无限循环","draw");
}
