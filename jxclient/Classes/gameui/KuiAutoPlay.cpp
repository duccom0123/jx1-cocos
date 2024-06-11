#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiAutoPlay.h"

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
#include "gamecore/KNpc.h"
#include "gamecore/KNpcSet.h"
#include "gamecore/KPlayer.h"
#include "gamecore/KPlayerSet.h"
#include "gamecore/KSubWorldSet.h"
#include "gamecore/KSkillManager.h"
#include "gamecore/KItem.h"
#include "gamescene/KScenePlaceC.h"

#ifdef WIN32
#include <direct.h>
#else
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

/*#ifndef WIN32
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#endif*/
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

/*#define  SCHEME_INI_AUTO_SETING "jx50ai/autoset/%u_set.ini"
#define  SCHEME_INI_AUTO_ITEM   "jx50ai/equipName.txt"
#define  SCHEME_INI_AUTO_DAOJU  "jx50ai/itemName.txt"
#define  SCHEME_INI_AUTO_USE    "jx50ai/itemUseName.txt"
#define  SCHEME_INI_AUTO_SKILL "\\settings\\Ai\\skill.txt"
#define  CONTROL_SCALE 0.5
#define	 LIFE_TIME_DISTANCE_DEFAULT		50
#define	 AUTO_TIME_SAY_DEFAULT          18
#define	 LIFE_TIME_USERITEM_DEFAULT		180
*/
KuiAutoPlay::KuiAutoPlay()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	//pMoneyLabel       = NULL;
	//pXuLabel          = NULL;
//	colorLayer        = NULL;
	red               = NULL;

	m_uLifeTime = 0;
	m_uManaTime = 0;
	m_PortalTime = 0;
	m_uMoneyTime = 0;
	nUseItemLoopTime = 0;
	g_nServiceLoop   = 0;
	m_pUiLayer = NULL;
	m_pWidget  = NULL;
//	m_array    = NULL;
	listView_Item   = NULL;
	listView_tool   = NULL;
	listView_use    = NULL;
	btnPageBox_1= NULL;
	btnPageBox_2= NULL;
	btnPageBox_3= NULL;
	btnPageBox_4= NULL;
	btnPageBox_5= NULL;
	clientSaveDir = "";
	checkBox_baohu = NULL;
	checkBox_zhandou = NULL;
	checkBox_shiqu   = NULL;

	pshuxEditBox =NULL;
	pstrEditBox_s=NULL;
	pstrEditBox_sz=NULL;
	pstrEditBox_d=NULL;
	pstrEditBox_u=NULL;
	pEditBox_yanshi = NULL;

	switchControl_baohu = NULL;
	switchControl_team  = NULL;
	switchControl_team_g = NULL;
	switchControl_zhandou = NULL;
	switchControl_shiqu = NULL;
	switchControl_savem = NULL;
	switchControl_fenjie = NULL;
	switchControl_yanshi = NULL;
	switchControl_team_y =NULL;
	switchControl_team_d =NULL;
	switchControl_yabiao =NULL;
	switchControl_baoliu =NULL;
	switchControl_useitem=NULL;
	switchControl_luxian =NULL;
	switchControl_dingdian=NULL;
	switchControl_luzhi =NULL;
	switchControl_miaosha = NULL;
	switchControl_other_s = NULL;
	nFactionNo = 0;
	int i;
	for (i = 0; i < MAX_ITEMLIST_COUNT; ++i)
	{
		custom_button_item[i] = NULL;
		custom_button_tool[i] = NULL;
		custom_button_use[i]  = NULL;
	}
	for (i = 0; i < MAX_SKILL_COUNT; ++i)
	{
		switchControl_skill[i] = NULL;
		skillnameLabel[i] = NULL;
	}
	nTempSelItemIndex = -1;
	nTempSeltoolIndex = -1;
	nTempSeluseIndex  = -1;

	_button_1 =NULL;
	_button_2 =NULL;
	_button   =NULL;

	ZeroMemory(nFilePath,sizeof(nFilePath));
}

KuiAutoPlay::~KuiAutoPlay()
{
	isOpen = false;
	//pMoneyLabel  = NULL;
//	colorLayer   = NULL;
//	CC_SAFE_RELEASE(m_array);
	if (red)
	    red->release();

	nAutoNeiGua.Clear();
	nAutoSkill.Clear();
	nAutoZhuang.Clear();
	nAutoDaiju.Clear();
	nAutoUseItem.Clear();
	//Terminate(1);
}

//创建确定弹出框
KuiAutoPlay * KuiAutoPlay::create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	if (!g_GameWorld) return NULL;
	KuiAutoPlay * popLayer = KuiAutoPlay::create();
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiAutoPlay::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne);   //禁止多点触摸
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\任务记事\\任务记事.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return false;
	CCSprite * spriteBox = CCSprite::createWithTexture(bgCur);
	ParentNode_Team= spriteBox;
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2));
	this->addChild(spriteBox);
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	clientSaveDir = creatAutoPlayDir("jx50ai");
	char nTempDir[128];
	t_sprintf(nTempDir,SCHEME_INI_AUTO_SETING);
	CreateFolder(nTempDir);
	//creatAutoPlayDir("jx50ai");
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	
	return true;
}

void KuiAutoPlay::addDialogData()
{
	if  (!g_pCoreShell) return;
	//CheckBox::
	//Left=24
	//Top=72
	//m_StartPos.x =24;//+ITEM_CELL_SIZE/2;
	//m_StartPos.y =72;//+ITEM_CELL_SIZE/2;

	btnSize.width = 110;
	btnSize.height= 16;
	g_pCoreShell->GetGameData(GDI_LAOHUJI_INFO,130,0);
	g_ScenePlace.GetSceneNameAndFocus(nMapName,nMapidx,nPosX,nPosY,nMapKind);
	t_sprintf(nFilePath,SCHEME_INI_AUTO_SETING,g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
	if (!nAutoNeiGua.Load(nFilePath,TRUE))
	{
#ifdef WIN32
		CCMessageBox("配置文件缺失!","提示");
#else
		CCMessageBox(UTEXT("配置文件缺失!",1).c_str(),UTEXT("提示",1).c_str());
		//CCMessageBox(nFilePath,UTEXT("提示",1).c_str());

#endif
	}
	if (!nAutoSkill.Load(SCHEME_INI_AUTO_SKILL))
	{
#ifdef WIN32
		CCMessageBox("技能配置文件缺失!","提示");
#else
		CCMessageBox(UTEXT("技能配置文件缺失!",1).c_str(),UTEXT("提示",1).c_str());
#endif
	}

	if (!nAutoZhuang.Load(SCHEME_INI_AUTO_ITEM))
	{
		if (nAutoZhuang.CreatFile(SCHEME_INI_AUTO_ITEM))
		{//创建文件
			if (!nAutoZhuang.Load(SCHEME_INI_AUTO_ITEM))
			{
#ifdef WIN32
				CCMessageBox("属性文件缺失!","提示");
#else
				CCMessageBox(UTEXT("属性文件缺失!",1).c_str(),UTEXT("提示",1).c_str());
#endif
			}
		}
	}
	if (!nAutoDaiju.Load(SCHEME_INI_AUTO_DAOJU))
	{
		if (nAutoDaiju.CreatFile(SCHEME_INI_AUTO_DAOJU))
		{//创建文件
			if (!nAutoDaiju.Load(SCHEME_INI_AUTO_DAOJU))
			{
#ifdef WIN32
				CCMessageBox("道具文件缺失!","提示");
#else
				CCMessageBox(UTEXT("道具文件缺失!",1).c_str(),UTEXT("提示",1).c_str());
#endif
			}
		}
	}

	if (!nAutoUseItem.Load(SCHEME_INI_AUTO_USE))
	{
		if (nAutoUseItem.CreatFile(SCHEME_INI_AUTO_USE))
		{//创建文件
			if (!nAutoUseItem.Load(SCHEME_INI_AUTO_USE))
			{
#ifdef WIN32
				CCMessageBox("物品文件缺失!","提示");
#else
				CCMessageBox(UTEXT("物品文件缺失!",1).c_str(),UTEXT("提示",1).c_str());
#endif
			}
		}
	}
	m_pUiLayer = UILayer::create();
	ParentNode_Team->addChild(m_pUiLayer);
	
	UIImageView* titleImg =UIImageView::create();
	titleImg->loadTexture("ui/autoplay.png");
	//titleImg->setAnchorPoint(ccp(0,0));
	titleImg->setPosition(ccp(m_size.width/2,m_size.height-13));
	m_pUiLayer->addWidget(titleImg);

	// Create the checkbox
	char nSprName[128]={0};
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui4\\common\\勾选.spr");
	g_StrLower(nSprName);

	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCTexture2D * check_box_normal = bgCur;

	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCTexture2D * check_box_active = bgCur;
	CCTexture2D * check_box_active_disable = bgCur;

	/*bgCur = _getinidata.getinidata_one(nSprName,2,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;

	CCTexture2D * check_box_active_disable = bgCur;
	*/
	ccColor4B color = ccc4(112, 128, 144,150);
	CCSize nPageSize    = CCSizeMake(m_size.width-35,m_size.height-63);
	CCPoint CCPointPage = CCPointMake(18,22);
	int nisOpen=0,nsaveMonye=0;
	_PageLayer_1 = CCNode::create();//UILayer::create();//基本设置
	//_PageLayer_1->setContentSize(nPageSize);
	_PageLayer_1->setPosition(CCPointPage);
	//_PageLayer_1->setVisible(false);
	ParentNode_Team->addChild(_PageLayer_1);

//------------基本设置面板-------------------------
	CCLabelTTF* pBaohuSetLabel = CCLabelTTF::create(UTEXT("保护设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	pBaohuSetLabel->setAnchorPoint(ccp(0,0));
	pBaohuSetLabel->setColor(ccYELLOW);
	pBaohuSetLabel->setPosition(ccp(10,nPageSize.height-25));
	_PageLayer_1->addChild(pBaohuSetLabel);
	//开启保护
	/*CCLabelTTF* pBaohuOpenLabel = CCLabelTTF::create(UTEXT("开启保护",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pBaohuOpenLabel->setAnchorPoint(ccp(0,0));
	pBaohuOpenLabel->setPosition(ccp(15,nPageSize.height-35));
	_PageLayer_1->addChild(pBaohuOpenLabel);*/
	char nTempStr[32];
	nAutoNeiGua.GetString("AAAA","IsOpenBaohu","0",nTempStr,sizeof(nTempStr));
	int nTempInt = g_Atoui(nTempStr);
	switchControl_baohu = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_baohu->setScale(CONTROL_SCALE);
	switchControl_baohu->setTag(BOX_TAG_BASE);
	//switchControl_baohu->setAnchorPoint(ccp(0,0));
	//switchControl_baohu->setPosition(ccp(pBaohuOpenLabel->getContentSize().width+35,nPageSize.height-35+pBaohuOpenLabel->getContentSize().height/2));
	switchControl_baohu->setPosition(ccp(pBaohuSetLabel->getContentSize().width+27,nPageSize.height-18));
	_PageLayer_1->addChild(switchControl_baohu);
	switchControl_baohu->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	valueChanged(switchControl_baohu, CCControlEventValueChanged);
	if  (nTempInt>0)
	    switchControl_baohu->setOn(true);
	else
		switchControl_baohu->setOn(false);
	/*checkBox_baohu = UICheckBox::create();
	checkBox_baohu->setTouchEnabled(true);
	checkBox_baohu->loadTexturesBydata(check_box_normal,
		check_box_active_disable,
		check_box_active_disable,check_box_normal,check_box_active);
	checkBox_baohu->setScale(1.5);
	checkBox_baohu->setPosition(ccp(pBaohuOpenLabel->getContentSize().width+27,nPageSize.height-35+pBaohuOpenLabel->getContentSize().height/2));
	checkBox_baohu->addEventListenerCheckBox(this, checkboxselectedeventselector(KuiAutoPlay::selectedBoxStateEvent));
	checkBox_baohu->setTag(BOX_TAG_BASE);
	_PageLayer_1->addWidget(checkBox_baohu);
	 if  (nTempInt>0)
		 checkBox_baohu->setSelectedState(true);*/

	/* checkBox_zhandou = UICheckBox::create();
	 checkBox_zhandou->setTouchEnabled(true);
	 CCTexture2D * check_box_normal_z          = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox/001.png");
	 CCTexture2D * check_box_active_disable_z  = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox/002.png");
	 CCTexture2D * check_box_active_z          = check_box_active_disable_z;
	 checkBox_zhandou->loadTexturesBydata(check_box_normal_z,
		 check_box_active_disable_z,
		 check_box_active_disable_z,check_box_normal_z,check_box_active_z);
	 checkBox_zhandou->setScale(1.5);
	 checkBox_zhandou->setPosition(ccp(pBaohuOpenLabel->getContentSize().width+80,nPageSize.height-35+pBaohuOpenLabel->getContentSize().height/2));
	 checkBox_zhandou->addEventListenerCheckBox(this, checkboxselectedeventselector(KuiAutoPlay::checkBoxStateEvent));
	 checkBox_zhandou->setTag(BOX_TAG_ART);
	 _PageLayer_1->addWidget(checkBox_zhandou);
	 if  (nTempInt>0)
		 checkBox_zhandou->setSelectedState(true);
	 */
	CCScale9Sprite *pBg;

	CCLabelTTF* plifeLabel = CCLabelTTF::create(UTEXT("生命 <=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	plifeLabel->setAnchorPoint(ccp(0,0));
	plifeLabel->setPosition(ccp(switchControl_baohu->getPosition().x+30,nPageSize.height-35));
	_PageLayer_1->addChild(plifeLabel);

	pBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pLifeEditBox = CCEditBox::create(CCSize(50,24),pBg);
	const ccColor3B nfcolor ={255,255,255};
	if (pLifeEditBox)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pLifeEditBox->setPlaceHolder(UTEXT("生命",1).c_str());
		pLifeEditBox->setFontColor(ccYELLOW);
		pLifeEditBox->setFontSize(24);
		pLifeEditBox->setMaxLength(6);
		pLifeEditBox->setPosition(ccp(plifeLabel->getPosition().x+plifeLabel->getContentSize().width+30,nPageSize.height-30));
		pLifeEditBox->setInputMode(kEditBoxInputModeNumeric);
		pLifeEditBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_1->addChild(pLifeEditBox);

		nAutoNeiGua.GetString("AAAA","nBaohuInfo1","50",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pLifeEditBox->setText(nTempStr);
	}
	CCLabelTTF* pheyaoLabel = CCLabelTTF::create(UTEXT("喝红",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	pheyaoLabel->setAnchorPoint(ccp(0,0));
	pheyaoLabel->setPosition(ccp(pLifeEditBox->getPosition().x+pLifeEditBox->getContentSize().width-20,nPageSize.height-35));
	_PageLayer_1->addChild(pheyaoLabel);


	CCLabelTTF* pneiliLabel = CCLabelTTF::create(UTEXT("内力 <=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pneiliLabel->setAnchorPoint(ccp(0,0));
	pneiliLabel->setPosition(ccp(switchControl_baohu->getPosition().x+180,nPageSize.height-35));
	_PageLayer_1->addChild(pneiliLabel);

	          pBg = CCScale9Sprite::create("ui/editbg.png");
	pNeiliEditBox = CCEditBox::create(CCSize(50,24),pBg);
	if (pNeiliEditBox)
	{
		pNeiliEditBox->setPlaceHolder(UTEXT("内力",1).c_str());
		pNeiliEditBox->setFontColor(ccYELLOW);
		pNeiliEditBox->setFontSize(24);
		pNeiliEditBox->setMaxLength(6);
		pNeiliEditBox->setPosition(ccp(pneiliLabel->getPosition().x+pneiliLabel->getContentSize().width+30,nPageSize.height-30));
		pNeiliEditBox->setInputMode(kEditBoxInputModeNumeric);
		pNeiliEditBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_1->addChild(pNeiliEditBox);
		nAutoNeiGua.GetString("AAAA","nBaohuInfo2","50",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pNeiliEditBox->setText(nTempStr);
	}
	CCLabelTTF* phelanLabel = CCLabelTTF::create(UTEXT("喝蓝",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	phelanLabel->setAnchorPoint(ccp(0,0));
	phelanLabel->setPosition(ccp(pNeiliEditBox->getPosition().x+pNeiliEditBox->getContentSize().width-20,nPageSize.height-35));
	_PageLayer_1->addChild(phelanLabel);


	CCLabelTTF* plifeLabel_h = CCLabelTTF::create(UTEXT("生命 <=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	plifeLabel_h->setAnchorPoint(ccp(0,0));
	plifeLabel_h->setPosition(ccp(switchControl_baohu->getPosition().x+30,nPageSize.height-65));
	_PageLayer_1->addChild(plifeLabel_h);

	pBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pLifeEditBox_h = CCEditBox::create(CCSize(50,24),pBg);
	if (pLifeEditBox_h)
	{
		pLifeEditBox_h->setPlaceHolder(UTEXT("生命",1).c_str());
		pLifeEditBox_h->setFontColor(ccYELLOW);
		pLifeEditBox_h->setFontSize(24);
		pLifeEditBox_h->setMaxLength(6);
		pLifeEditBox_h->setPosition(ccp(plifeLabel_h->getPosition().x+plifeLabel_h->getContentSize().width+30,nPageSize.height-55));
		pLifeEditBox_h->setInputMode(kEditBoxInputModeNumeric);
		pLifeEditBox_h->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_1->addChild(pLifeEditBox_h);
		nAutoNeiGua.GetString("AAAA","nBaohuInfo3","50",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pLifeEditBox_h->setText(nTempStr);
	}
	CCLabelTTF* phuichengLabel = CCLabelTTF::create(UTEXT("回城",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	phuichengLabel->setAnchorPoint(ccp(0,0));
	phuichengLabel->setPosition(ccp(pLifeEditBox_h->getPosition().x+pLifeEditBox_h->getContentSize().width-20,nPageSize.height-65));
	_PageLayer_1->addChild(phuichengLabel);
	
	CCLabelTTF* pTeamSetLabel = CCLabelTTF::create(UTEXT("组队设置",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pTeamSetLabel->setAnchorPoint(ccp(0,0));
	pTeamSetLabel->setColor(ccYELLOW);
	pTeamSetLabel->setPosition(ccp(10,nPageSize.height-100));
	_PageLayer_1->addChild(pTeamSetLabel);
	//跟随队长
	CCLabelTTF* pgenshuiOpenLabel = CCLabelTTF::create(UTEXT("自动跟随队长",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pgenshuiOpenLabel->setAnchorPoint(ccp(0,0));
	pgenshuiOpenLabel->setPosition(ccp(70,nPageSize.height-100));
	_PageLayer_1->addChild(pgenshuiOpenLabel);
	switchControl_team_g = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_team_g->setTag(BOX_TAG_GENSHUI);
	switchControl_team_g->setPosition(ccp(pgenshuiOpenLabel->getPosition().x+pgenshuiOpenLabel->getContentSize().width+20,nPageSize.height-94));
	_PageLayer_1->addChild(switchControl_team_g);
	switchControl_team_g->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	switchControl_team_g->setOn(false);
	valueChanged(switchControl_team_g, CCControlEventValueChanged);
	//--------
	CCLabelTTF* pteamOpenLabel = CCLabelTTF::create(UTEXT("自动应答",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pteamOpenLabel->setAnchorPoint(ccp(0,0));
	pteamOpenLabel->setPosition(ccp(15,nPageSize.height-120));
	_PageLayer_1->addChild(pteamOpenLabel);
	switchControl_team = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_team->setTag(BOX_TAG_TEAM_1);
	switchControl_team->setPosition(ccp(pteamOpenLabel->getPosition().x+pteamOpenLabel->getContentSize().width+20,nPageSize.height-114));
	_PageLayer_1->addChild(switchControl_team);
	switchControl_team->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	switchControl_team->setOn(false);
	valueChanged(switchControl_team, CCControlEventValueChanged);
	
	CCLabelTTF* pduizhangOpenLabel = CCLabelTTF::create(UTEXT("我是队长",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pduizhangOpenLabel->setAnchorPoint(ccp(0,0));
	pduizhangOpenLabel->setPosition(ccp(120,nPageSize.height-120));
	_PageLayer_1->addChild(pduizhangOpenLabel);
	switchControl_team_d = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("是",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("否",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_team_d->setTag(BOX_TAG_TEAM_2);
	switchControl_team_d->setPosition(ccp(pduizhangOpenLabel->getPosition().x+pduizhangOpenLabel->getContentSize().width+20,nPageSize.height-114));
	_PageLayer_1->addChild(switchControl_team_d);
	switchControl_team_d->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	switchControl_team_d->setOn(false);
	//valueChanged(switchControl_team_d, CCControlEventValueChanged);
	

	CCLabelTTF* pduiyOpenLabel = CCLabelTTF::create(UTEXT("我是队员",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pduiyOpenLabel->setAnchorPoint(ccp(0,0));
	pduiyOpenLabel->setPosition(ccp(220,nPageSize.height-120));
	_PageLayer_1->addChild(pduiyOpenLabel);
	switchControl_team_y = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("是",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("否",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_team_y->setTag(BOX_TAG_TEAM_3);
	switchControl_team_y->setPosition(ccp(pduiyOpenLabel->getPosition().x+pduiyOpenLabel->getContentSize().width+20,nPageSize.height-114));
	_PageLayer_1->addChild(switchControl_team_y);
	switchControl_team_y->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	switchControl_team_y->setOn(false);
	//valueChanged(switchControl_team_y, CCControlEventValueChanged);
	

	CCLabelTTF* pbiaoSetLabel = CCLabelTTF::create(UTEXT("押镖设置",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pbiaoSetLabel->setAnchorPoint(ccp(0,0));
	pbiaoSetLabel->setColor(ccYELLOW);
	pbiaoSetLabel->setPosition(ccp(10,nPageSize.height-140));
	_PageLayer_1->addChild(pbiaoSetLabel);

	CCLabelTTF* pybiaoOpenLabel = CCLabelTTF::create(UTEXT("自动押镖",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pybiaoOpenLabel->setAnchorPoint(ccp(0,0));
	pybiaoOpenLabel->setPosition(ccp(pbiaoSetLabel->getContentSize().width+20,nPageSize.height-140));
	_PageLayer_1->addChild(pybiaoOpenLabel);
	switchControl_yabiao = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_yabiao->setTag(BOX_TAG_YABIAO);
	switchControl_yabiao->setPosition(ccp(pybiaoOpenLabel->getPosition().x+pybiaoOpenLabel->getContentSize().width+20,nPageSize.height-134));
	_PageLayer_1->addChild(switchControl_yabiao);
	switchControl_yabiao->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	switchControl_yabiao->setOn(false);
	valueChanged(switchControl_yabiao, CCControlEventValueChanged);

	CCLabelTTF* pzbSetLabel = CCLabelTTF::create(UTEXT("其他设置(拾取装备,物品)",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pzbSetLabel->setAnchorPoint(ccp(0,0));
	pzbSetLabel->setColor(ccYELLOW);
	pzbSetLabel->setPosition(ccp(10,nPageSize.height-160));
	_PageLayer_1->addChild(pzbSetLabel);

	CCLabelTTF* pshuOpenLabel = CCLabelTTF::create(UTEXT("装备属性数量>=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pshuOpenLabel->setAnchorPoint(ccp(0,0));
	pshuOpenLabel->setPosition(ccp(15,nPageSize.height-180));
	_PageLayer_1->addChild(pshuOpenLabel);

	pBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pshuxEditBox = CCEditBox::create(CCSize(50,24),pBg);
	if (pshuxEditBox)
	{
		pshuxEditBox->setPlaceHolder(UTEXT("数量",1).c_str());
		pshuxEditBox->setFontColor(ccYELLOW);
		pshuxEditBox->setFontSize(24);
		pshuxEditBox->setMaxLength(1);
		pshuxEditBox->setPosition(ccp(pshuOpenLabel->getPosition().x+pshuOpenLabel->getContentSize().width+30,nPageSize.height-174));
		pshuxEditBox->setInputMode(kEditBoxInputModeNumeric);
		pshuxEditBox->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_1->addChild(pshuxEditBox);
		//ShuXingNum=1
		//AllKeep=0 nisOpen
		nAutoNeiGua.GetString("CCCC","ShuXingNum","0",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pshuxEditBox->setText(nTempStr);
	}


	CCLabelTTF* pbaoliuOpenLabel = CCLabelTTF::create(UTEXT("个,保留所有装备",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pbaoliuOpenLabel->setAnchorPoint(ccp(0,0));
	pbaoliuOpenLabel->setPosition(ccp(160,nPageSize.height-180));
	_PageLayer_1->addChild(pbaoliuOpenLabel);
	switchControl_baoliu = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_baoliu->setTag(BOX_TAG_BAOLIU);
	switchControl_baoliu->setPosition(ccp(pbaoliuOpenLabel->getPosition().x+pbaoliuOpenLabel->getContentSize().width+20,nPageSize.height-174));
	_PageLayer_1->addChild(switchControl_baoliu);
	switchControl_baoliu->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("CCCC","AllKeep",0,&nisOpen);
	if (nisOpen>0)
		switchControl_baoliu->setOn(true);
	else
		switchControl_baoliu->setOn(false);
	valueChanged(switchControl_baoliu, CCControlEventValueChanged);

	CCLabelTTF* puseitemOpenLabel = CCLabelTTF::create(UTEXT("使用物品",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	puseitemOpenLabel->setAnchorPoint(ccp(0,0));
	puseitemOpenLabel->setPosition(ccp(15,nPageSize.height-200));
	_PageLayer_1->addChild(puseitemOpenLabel);
	switchControl_useitem = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_useitem->setTag(BOX_TAG_USEITEM);
	switchControl_useitem->setPosition(ccp(puseitemOpenLabel->getPosition().x+puseitemOpenLabel->getContentSize().width+20,nPageSize.height-194));
	_PageLayer_1->addChild(switchControl_useitem);
	switchControl_useitem->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("DDDD","IsOpenUseIetm",0,&nisOpen);
	if (nisOpen>0)
		switchControl_useitem->setOn(true);
	else
		switchControl_useitem->setOn(false);
	valueChanged(switchControl_useitem, CCControlEventValueChanged);

	CCLabelTTF* pmiaoshaOpenLabel = CCLabelTTF::create(UTEXT("过滤蓝绿怪物",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pmiaoshaOpenLabel->setAnchorPoint(ccp(0,0));
	pmiaoshaOpenLabel->setPosition(ccp(110,nPageSize.height-200));
	_PageLayer_1->addChild(pmiaoshaOpenLabel);
	switchControl_miaosha = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_team->setScale(CONTROL_SCALE);
	switchControl_miaosha->setTag(BOX_TAG_MIAOSHA);
	switchControl_miaosha->setPosition(ccp(pmiaoshaOpenLabel->getPosition().x+pmiaoshaOpenLabel->getContentSize().width+20,nPageSize.height-194));
	_PageLayer_1->addChild(switchControl_miaosha);
	switchControl_miaosha->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("DDDD","IsOpenNoBlue",0,&nisOpen);
	if (nisOpen>0)
		switchControl_miaosha->setOn(true);
	else
		switchControl_miaosha->setOn(false);

	valueChanged(switchControl_miaosha, CCControlEventValueChanged);
//--------攻击设置面板--------------------------
	_PageLayer_2 = UILayer::create();//攻击
	_PageLayer_2->setContentSize(nPageSize);
	_PageLayer_2->setPosition(CCPointPage);
	//_PageLayer_2->setVisible(false); //设置隐藏
	ParentNode_Team->addChild(_PageLayer_2);


	CCLabelTTF* pZhandouSetLabel = CCLabelTTF::create(UTEXT("战斗设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	pZhandouSetLabel->setAnchorPoint(ccp(0,0));
	pZhandouSetLabel->setColor(ccYELLOW);
	pZhandouSetLabel->setPosition(ccp(10,nPageSize.height-18));
	_PageLayer_2->addChild(pZhandouSetLabel);

	CCLabelTTF* pZhandouOpenLabel = CCLabelTTF::create(UTEXT("自动战斗",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pZhandouOpenLabel->setAnchorPoint(ccp(0,0));
	pZhandouOpenLabel->setPosition(ccp(15,nPageSize.height-35));
	_PageLayer_2->addChild(pZhandouOpenLabel);
	//char nTempStr[32];
	nAutoNeiGua.GetString("BBBB","IsOpenZhanDou","0",nTempStr,sizeof(nTempStr));
	nTempInt = g_Atoui(nTempStr);
	switchControl_zhandou = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_zhandou->setScale(CONTROL_SCALE);
	switchControl_zhandou->setPosition(ccp(pZhandouOpenLabel->getContentSize().width+35,nPageSize.height-35+pZhandouOpenLabel->getContentSize().height/2));
	switchControl_zhandou->setTag(BOX_TAG_ART);
	_PageLayer_2->addChild(switchControl_zhandou);
	switchControl_zhandou->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	if  (nTempInt>0)
		switchControl_zhandou->setOn(true);
	else
		switchControl_zhandou->setOn(false);
	valueChanged(switchControl_zhandou, CCControlEventValueChanged);
	switchControl_zhandou->setTouchEnabled(false);
	/*checkBox_zhandou = UICheckBox::create();
	checkBox_zhandou->setTouchEnabled(true);
	CCTexture2D * check_box_normal_z          = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox/001.png");
	CCTexture2D * check_box_active_disable_z  = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox/002.png");
	CCTexture2D * check_box_active_z          = check_box_active_disable_z;
	checkBox_zhandou->loadTexturesBydata(check_box_normal_z,
		check_box_active_disable_z,
		check_box_active_disable_z,check_box_normal_z,check_box_active_z);
	checkBox_zhandou->setScale(1.5);
	checkBox_zhandou->setPosition(ccp(pZhandouOpenLabel->getContentSize().width+27,nPageSize.height-35+pZhandouOpenLabel->getContentSize().height/2));
	checkBox_zhandou->addEventListenerCheckBox(this, checkboxselectedeventselector(KuiAutoPlay::checkBoxStateEvent));
	checkBox_zhandou->setTag(BOX_TAG_ART);
	_PageLayer_2->addWidget(checkBox_zhandou);
	if  (nTempInt>0)
		checkBox_zhandou->setSelectedState(true);
	*/

	CCLabelTTF* pxunguaiLabel = CCLabelTTF::create(UTEXT("寻怪范围",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pxunguaiLabel->setAnchorPoint(ccp(0,0));
	pxunguaiLabel->setPosition(ccp(switchControl_zhandou->getPosition().x+30,nPageSize.height-35));
	_PageLayer_2->addChild(pxunguaiLabel);

	CCScale9Sprite *gBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pXunguaiEditBox = CCEditBox::create(CCSize(50,24),gBg);
	if (pXunguaiEditBox)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pXunguaiEditBox->setPlaceHolder(UTEXT("范围",1).c_str());
		pXunguaiEditBox->setFontColor(ccYELLOW);
		pXunguaiEditBox->setFontSize(24);
		pXunguaiEditBox->setMaxLength(6);
		pXunguaiEditBox->setPosition(ccp(pxunguaiLabel->getPosition().x+pxunguaiLabel->getContentSize().width+30,switchControl_zhandou->getPosition().y));
		pXunguaiEditBox->setInputMode(kEditBoxInputModeNumeric);
		pXunguaiEditBox->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_2->addChild(pXunguaiEditBox);

		nAutoNeiGua.GetString("BBBB","FindNpcDis","180",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pXunguaiEditBox->setText(nTempStr);
	}
	CCLabelTTF* phuodongLabel = CCLabelTTF::create(UTEXT("活动范围",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	phuodongLabel->setAnchorPoint(ccp(0,0));
	phuodongLabel->setPosition(ccp(switchControl_zhandou->getPosition().x+180,nPageSize.height-35));
	_PageLayer_2->addChild(phuodongLabel);
	pBg = CCScale9Sprite::create("ui/editbg.png");
	pGongjiEditBox = CCEditBox::create(CCSize(50,24),pBg);
	if (pGongjiEditBox)
	{
		pGongjiEditBox->setPlaceHolder(UTEXT("活动范围",1).c_str());
		pGongjiEditBox->setFontColor(ccYELLOW);
		pGongjiEditBox->setFontSize(24);
		pGongjiEditBox->setMaxLength(6);
		pGongjiEditBox->setPosition(ccp(phuodongLabel->getPosition().x+phuodongLabel->getContentSize().width+30,switchControl_zhandou->getPosition().y));
		pGongjiEditBox->setInputMode(kEditBoxInputModeNumeric);
		pGongjiEditBox->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_2->addChild(pGongjiEditBox);
		nAutoNeiGua.GetString("BBBB","GongJiFanWei","180",nTempStr,sizeof(nTempStr));
		pGongjiEditBox->setText(nTempStr);
	}

	CCLabelTTF* pgmiaoLabel_h = CCLabelTTF::create(UTEXT("攻击秒数",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pgmiaoLabel_h->setAnchorPoint(ccp(0,0));
	pgmiaoLabel_h->setPosition(ccp(switchControl_zhandou->getPosition().x+30,nPageSize.height-62));
	_PageLayer_2->addChild(pgmiaoLabel_h);

	pBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pGongjiEditBox_m = CCEditBox::create(CCSize(50,24),pBg);
	if (pGongjiEditBox_m)
	{
		pGongjiEditBox_m->setPlaceHolder(UTEXT("秒",1).c_str());
		pGongjiEditBox_m->setFontColor(ccYELLOW);
		pGongjiEditBox_m->setFontSize(24);
		pGongjiEditBox_m->setMaxLength(2);
		pGongjiEditBox_m->setPosition(ccp(pgmiaoLabel_h->getPosition().x+pgmiaoLabel_h->getContentSize().width+30,nPageSize.height-55));
		pGongjiEditBox_m->setInputMode(kEditBoxInputModeNumeric);
		pGongjiEditBox_m->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_2->addChild(pGongjiEditBox_m);
		nAutoNeiGua.GetString("BBBB","GongJiNun","10",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		pGongjiEditBox_m->setText(nTempStr);
	}
	CCLabelTTF* ptempLabel = CCLabelTTF::create(UTEXT("秒无效,放弃攻击",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	ptempLabel->setAnchorPoint(ccp(0,0));
	ptempLabel->setPosition(ccp(pGongjiEditBox_m->getPosition().x+pGongjiEditBox_m->getContentSize().width-20,nPageSize.height-62));
	_PageLayer_2->addChild(ptempLabel);
	//------录制脚本
	CCLabelTTF* pautoSetLabel = CCLabelTTF::create(UTEXT("挂机模式",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pautoSetLabel->setAnchorPoint(ccp(0,0));
	pautoSetLabel->setColor(ccYELLOW);
	pautoSetLabel->setPosition(ccp(10,nPageSize.height-80));
	_PageLayer_2->addChild(pautoSetLabel);

	CCLabelTTF* pdingLabel = CCLabelTTF::create(UTEXT("定点",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	pdingLabel->setAnchorPoint(ccp(0,0));
	pdingLabel->setPosition(ccp(80,nPageSize.height-80));
	_PageLayer_2->addChild(pdingLabel);
	switchControl_dingdian = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("是",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("否",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_shiqu->setScale(CONTROL_SCALE);
	switchControl_dingdian->setPosition(ccp(pdingLabel->getPosition().x+pdingLabel->getContentSize().width+20,nPageSize.height-74));
	switchControl_dingdian->setTag(BOX_TAG_DING);
	_PageLayer_2->addChild(switchControl_dingdian);
	switchControl_dingdian->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("BBBB","IsXiTong",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_dingdian->setOn(true);
	else
		switchControl_dingdian->setOn(false);
	valueChanged(switchControl_dingdian, CCControlEventValueChanged);

	switchControl_dingdian->setTouchEnabled(false);

	CCLabelTTF* pluxianLabel = CCLabelTTF::create(UTEXT("路线",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	pluxianLabel->setAnchorPoint(ccp(0,0));
	pluxianLabel->setPosition(ccp(160,nPageSize.height-80));
	_PageLayer_2->addChild(pluxianLabel);
	switchControl_luxian = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("是",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("否",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_shiqu->setScale(CONTROL_SCALE);
	switchControl_luxian->setPosition(ccp(pluxianLabel->getPosition().x+pluxianLabel->getContentSize().width+20,nPageSize.height-74));
	switchControl_luxian->setTag(BOX_TAG_LUXIAN);
	_PageLayer_2->addChild(switchControl_luxian);
	switchControl_luxian->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("BBBB","IsJiaoBen",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_luxian->setOn(true);
	else
		switchControl_luxian->setOn(false);

	if  (switchControl_dingdian && switchControl_dingdian->isOn())
		switchControl_luxian->setOn(false);
	else
		switchControl_luxian->setOn(true);

	valueChanged(switchControl_luxian, CCControlEventValueChanged);
	switchControl_luxian->setTouchEnabled(false);


	switchControl_luzhi = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask-6.png"),
		CCSprite::create("ui/switch/switch-on-6.png"),
		CCSprite::create("ui/switch/switch-off-6.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("录制路线:开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("录制路线:关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//setPosition(ccp(160,nPageSize.height-80));
	//switchControl_luzhi->setScaleX(4);
	switchControl_luzhi->setPosition(ccp(300,nPageSize.height-74));
	switchControl_luzhi->setTag(BOX_TAG_LUZHI);
	_PageLayer_2->addChild(switchControl_luzhi);
	switchControl_luzhi->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	//valueChanged(switchControl_luzhi, CCControlEventValueChanged);
	if  (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen)
		switchControl_luzhi->setOn(true);
	else
	    switchControl_luzhi->setOn(false);

	switchControl_luzhi->setTouchEnabled(false);

	//------
	CCLabelTTF* pPickSetLabel = CCLabelTTF::create(UTEXT("拾取设置",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pPickSetLabel->setAnchorPoint(ccp(0,0));
	pPickSetLabel->setColor(ccYELLOW);
	pPickSetLabel->setPosition(ccp(10,nPageSize.height-100));
	_PageLayer_2->addChild(pPickSetLabel);

	CCLabelTTF* pPickLabel = CCLabelTTF::create(UTEXT("自动拾取",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	pPickLabel->setAnchorPoint(ccp(0,0));
	pPickLabel->setPosition(ccp(80,nPageSize.height-100));
	_PageLayer_2->addChild(pPickLabel);

	nAutoNeiGua.GetString("BBBB","IsOpenShiQu","0",nTempStr,sizeof(nTempStr));
	nTempInt = g_Atoui(nTempStr);
	switchControl_shiqu = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_shiqu->setScale(CONTROL_SCALE);
	switchControl_shiqu->setPosition(ccp(pPickLabel->getPosition().x+pPickLabel->getContentSize().width+20,nPageSize.height-94));
	switchControl_shiqu->setTag(BOX_TAG_SHIQU);
	_PageLayer_2->addChild(switchControl_shiqu);
	switchControl_shiqu->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	if  (nTempInt>0)
		switchControl_shiqu->setOn(true);
	else
		switchControl_shiqu->setOn(false);
	valueChanged(switchControl_shiqu, CCControlEventValueChanged);
	switchControl_shiqu->setTouchEnabled(false);
	CCLabelTTF* pshiquOpenLabel = CCLabelTTF::create(UTEXT("自动存钱",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pshiquOpenLabel->setAnchorPoint(ccp(0,0));
	pshiquOpenLabel->setPosition(ccp(15,nPageSize.height-115));
	_PageLayer_2->addChild(pshiquOpenLabel);
	//char nTempStr[32];
	switchControl_savem = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_savem->setScale(CONTROL_SCALE);
	switchControl_savem->setPosition(ccp(pshiquOpenLabel->getContentSize().width+35,nPageSize.height-115+pshiquOpenLabel->getContentSize().height/2));
	//switchControl_savem->setPosition(ccp(popensaveLabel->getPosition().x+popensaveLabel->getContentSize().width+35,nPageSize.height-80+popensaveLabel->getContentSize().height/2));
	switchControl_savem->setTag(BOX_TAG_SAVE_M);
	_PageLayer_2->addChild(switchControl_savem);
	switchControl_savem->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger2("BBBB","IsOpenAutoSave",&nisOpen,&nsaveMonye);
	if  (nisOpen>0)
		switchControl_savem->setOn(true);
	else
		switchControl_savem->setOn(false);
	valueChanged(switchControl_savem, CCControlEventValueChanged);
	switchControl_savem->setTouchEnabled(false);
	/*checkBox_shiqu = UICheckBox::create();
	checkBox_shiqu->setTouchEnabled(true);
	check_box_normal_z          = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox1/001.png");
	check_box_active_disable_z  = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox1/002.png");
	check_box_active_z          = check_box_active_disable_z;
	checkBox_shiqu->loadTexturesBydata(check_box_normal_z,
		check_box_active_disable_z,
		check_box_active_disable_z,check_box_normal_z,check_box_active_z);
	checkBox_shiqu->setScale(1.5);
	checkBox_shiqu->setPosition(ccp(pshiquOpenLabel->getContentSize().width+27,nPageSize.height-115+pshiquOpenLabel->getContentSize().height/2));
	checkBox_shiqu->addEventListenerCheckBox(this, checkboxselectedeventselector(KuiAutoPlay::checkBoxStateEvent));
	checkBox_shiqu->setTag(BOX_TAG_SHIQU);
	_PageLayer_2->addWidget(checkBox_shiqu);
	if  (nTempInt>0)
		checkBox_shiqu->setSelectedState(true);
	*/
	CCLabelTTF* psaveLabel = CCLabelTTF::create(UTEXT("银两>=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	psaveLabel->setAnchorPoint(ccp(0,0));
	psaveLabel->setPosition(ccp(switchControl_shiqu->getPosition().x+30,nPageSize.height-115));
	_PageLayer_2->addChild(psaveLabel);

	gBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pmoneyEditBox = CCEditBox::create(CCSize(50,24),gBg);
	
	if (pmoneyEditBox)
	{
		pmoneyEditBox->setPlaceHolder(UTEXT("银两",1).c_str());
		pmoneyEditBox->setFontColor(ccYELLOW);
		pmoneyEditBox->setFontSize(24);
		pmoneyEditBox->setMaxLength(6);
		pmoneyEditBox->setPosition(ccp(psaveLabel->getPosition().x+psaveLabel->getContentSize().width+30,switchControl_savem->getPosition().y));
		pmoneyEditBox->setInputMode(kEditBoxInputModeNumeric);
		pmoneyEditBox->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_2->addChild(pmoneyEditBox);
		t_sprintf(nTempStr,"%d",nsaveMonye);
		//nTempInt = g_Atoui(nTempStr);
		pmoneyEditBox->setText(nTempStr);
	}
	CCLabelTTF* popensaveLabel = CCLabelTTF::create(UTEXT("万,自动存钱",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	popensaveLabel->setAnchorPoint(ccp(0,0));
	popensaveLabel->setPosition(ccp(pmoneyEditBox->getPosition().x+30,nPageSize.height-115));
	_PageLayer_2->addChild(popensaveLabel);
	/*checkBox_savem = UICheckBox::create();
	checkBox_savem->setTouchEnabled(true);
	check_box_normal_z          = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox1/001.png");
	check_box_active_disable_z  = CCTextureCache::sharedTextureCache()->addImage("ui/checkbox1/002.png");
	check_box_active_z          = check_box_active_disable_z;
	checkBox_savem->loadTexturesBydata(check_box_normal_z,
		check_box_active_disable_z,
		check_box_active_disable_z,check_box_normal_z,check_box_active_z);
	checkBox_savem->setScale(1.5);
	//checkBox_savem->setPosition(ccp(pshiquOpenLabel->getContentSize().width+47,nPageSize.height-115+pshiquOpenLabel->getContentSize().height/2));
	checkBox_savem->setPosition(ccp(popensaveLabel->getPosition().x+popensaveLabel->getContentSize().width+15,nPageSize.height-115+popensaveLabel->getContentSize().height/2));
	checkBox_savem->addEventListenerCheckBox(this, checkboxselectedeventselector(KuiAutoPlay::checkBoxStateEvent));
	checkBox_savem->setTag(BOX_TAG_SAVE_M);
	_PageLayer_2->addWidget(checkBox_savem);
	if  (nTempInt>0)
		checkBox_savem->setSelectedState(true);
	*/

	CCLabelTTF* pshiquLabel_c = CCLabelTTF::create(UTEXT("拾取>=",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pshiquLabel_c->setAnchorPoint(ccp(0,0));
	pshiquLabel_c->setPosition(ccp(switchControl_shiqu->getPosition().x+30,nPageSize.height-140));
	_PageLayer_2->addChild(pshiquLabel_c);

	gBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pshiquEditBox_c = CCEditBox::create(CCSize(50,24),gBg);
	if (pshiquEditBox_c)
	{
		pshiquEditBox_c->setPlaceHolder(UTEXT("次数",1).c_str());
		pshiquEditBox_c->setFontColor(ccYELLOW);
		pshiquEditBox_c->setFontSize(24);
		pshiquEditBox_c->setMaxLength(6);
		pshiquEditBox_c->setPosition(ccp(pshiquLabel_c->getPosition().x+pshiquLabel_c->getContentSize().width+30,pshiquLabel_c->getPosition().y+6));
		pshiquEditBox_c->setInputMode(kEditBoxInputModeNumeric);
		pshiquEditBox_c->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_2->addChild(pshiquEditBox_c);
		nAutoNeiGua.GetString("BBBB","ShiQuNum","10",nTempStr,sizeof(nTempStr));
		//t_sprintf(nTempStr,"%d",nsaveMonye);
		//nTempInt = g_Atoui(nTempStr);
		pshiquEditBox_c->setText(nTempStr);
	}
	CCLabelTTF* plostLabel = CCLabelTTF::create(UTEXT("次无效,放弃拾取",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	plostLabel->setAnchorPoint(ccp(0,0));
	plostLabel->setPosition(ccp(pshiquEditBox_c->getPosition().x+30,nPageSize.height-140));
	_PageLayer_2->addChild(plostLabel);

	CCLabelTTF* pfenjieOpenLabel = CCLabelTTF::create(UTEXT("自动分解",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenjieOpenLabel->setAnchorPoint(ccp(0,0));
	pfenjieOpenLabel->setPosition(ccp(15,nPageSize.height-135));
	_PageLayer_2->addChild(pfenjieOpenLabel);
	//char nTempStr[32];
	switchControl_fenjie = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_savem->setScale(CONTROL_SCALE);
	switchControl_fenjie->setPosition(ccp(pfenjieOpenLabel->getContentSize().width+35,nPageSize.height-135+pfenjieOpenLabel->getContentSize().height/2));
	//switchControl_savem->setPosition(ccp(popensaveLabel->getPosition().x+popensaveLabel->getContentSize().width+35,nPageSize.height-80+popensaveLabel->getContentSize().height/2));
	switchControl_fenjie->setTag(BOX_TAG_FENJIE);
	_PageLayer_2->addChild(switchControl_fenjie);
	switchControl_fenjie->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("BBBB","IsOpenFenJie",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_fenjie->setOn(true);
	else
		switchControl_fenjie->setOn(false);
	valueChanged(switchControl_fenjie,CCControlEventValueChanged);
	switchControl_fenjie->setTouchEnabled(false);
	CCLabelTTF* pyanshiOpenLabel = CCLabelTTF::create(UTEXT("攻击拾取延时",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pyanshiOpenLabel->setAnchorPoint(ccp(0,0));
	pyanshiOpenLabel->setPosition(ccp(15,nPageSize.height-155));
	_PageLayer_2->addChild(pyanshiOpenLabel);
	//char nTempStr[32];
	switchControl_yanshi = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	//switchControl_savem->setScale(CONTROL_SCALE);
	switchControl_yanshi->setPosition(ccp(pyanshiOpenLabel->getContentSize().width+35,nPageSize.height-155+pyanshiOpenLabel->getContentSize().height/2));
	//switchControl_savem->setPosition(ccp(popensaveLabel->getPosition().x+popensaveLabel->getContentSize().width+35,nPageSize.height-80+popensaveLabel->getContentSize().height/2));
	switchControl_yanshi->setTag(BOX_TAG_YANSHI);
	_PageLayer_2->addChild(switchControl_yanshi);
	switchControl_yanshi->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("BBBB","IsOpenTime",0,&nisOpen);
	//if  (nisOpen>0)
		switchControl_yanshi->setOn(true);
	//else
		//switchControl_yanshi->setOn(false);
	valueChanged(switchControl_yanshi,CCControlEventValueChanged);
	switchControl_yanshi->setTouchEnabled(false);

		gBg = CCScale9Sprite::create("ui/editbg.png");
	//pBg->setColor(ccGREEN);
	pEditBox_yanshi = CCEditBox::create(CCSize(25,24),gBg);
	if (pEditBox_yanshi)
	{
		pEditBox_yanshi->setPlaceHolder(UTEXT("秒数",1).c_str());
		pEditBox_yanshi->setFontColor(ccYELLOW);
		pEditBox_yanshi->setFontSize(24);
		pEditBox_yanshi->setMaxLength(2);
		pEditBox_yanshi->setPosition(ccp(switchControl_yanshi->getPosition().x+switchControl_yanshi->getContentSize().width,pyanshiOpenLabel->getPosition().y+6));
		pEditBox_yanshi->setInputMode(kEditBoxInputModeNumeric);
		pEditBox_yanshi->setReturnType(kKeyboardReturnTypeDone);
		_PageLayer_2->addChild(pEditBox_yanshi);
		nAutoNeiGua.GetString("BBBB","QuXiaoTime","8",nTempStr,sizeof(nTempStr));
		//t_sprintf(nTempStr,"%d",nsaveMonye);
		//nTempInt = g_Atoui(nTempStr);
		pEditBox_yanshi->setText(nTempStr);
	}
	CCLabelTTF* pfangqiLabel = CCLabelTTF::create(UTEXT("秒,放弃攻击拾取",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	pfangqiLabel->setAnchorPoint(ccp(0,0));
	pfangqiLabel->setPosition(ccp(pEditBox_yanshi->getPosition().x+30,nPageSize.height-155));
	_PageLayer_2->addChild(pfangqiLabel);

    //--------------------
	CCLabelTTF* pskillSetLabel = CCLabelTTF::create(UTEXT("技能设置",1).c_str(),UI_GAME_FONT_DEFAULT,14);//Arial
	pskillSetLabel->setAnchorPoint(ccp(0,0));
	pskillSetLabel->setColor(ccYELLOW);
	pskillSetLabel->setPosition(ccp(10,nPageSize.height-175));
	_PageLayer_2->addChild(pskillSetLabel);
	/*CCControlSwitch *switchControl = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,20),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,20)
		);
	//switchControl->setScale(CONTROL_SCALE);
	switchControl->setPosition(ccp(100,nPageSize.height-175));
	_PageLayer_2->addChild(switchControl);
	switchControl->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	valueChanged(switchControl, CCControlEventValueChanged);
	*/
	nFactionNo =  Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130);
	
	float offsetX=50,offsetY=190;
	if (nFactionNo>0)
	{  
		for (int i=0;i<MAX_SKILL_COUNT;++i) 
		{
			if (i==2)
			{
              offsetX=50;
			  offsetY=205;
			}
			switchControl_skill[i] = CCControlSwitch::create
				(
				CCSprite::create("ui/switch/switch-mask.png"),
				CCSprite::create("ui/switch/switch-on.png"),
				CCSprite::create("ui/switch/switch-off.png"),
				CCSprite::create("ui/switch/switch-thumb.png"),
				CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
				CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
				);
			//switchControl->setScale(CONTROL_SCALE);
			switchControl_skill[i]->setPosition(ccp(offsetX-17,nPageSize.height-offsetY+5));
			_PageLayer_2->addChild(switchControl_skill[i]);
			switchControl_skill[i]->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
			char ntempKey[32];
			t_sprintf(ntempKey,"IsOpenSkill_%d",i);
			nAutoNeiGua.GetInteger("BBBB",ntempKey,0,&nisOpen);
			if  (nisOpen>0)
				switchControl_skill[i]->setOn(true);
			else
				switchControl_skill[i]->setOn(false);
			valueChanged(switchControl_skill[i], CCControlEventValueChanged);
			switchControl_skill[i]->setTouchEnabled(false);
			/*IsOpenSkill_0=0
				IsOpenSkill_1=1
				IsOpenSkill_2=1
				IsOpenSkill_3=0
				*/
			skillnameLabel[i] = CCLabelTTF::create(UTEXT("技能",1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
			skillnameLabel[i]->setAnchorPoint(ccp(0,0));
			skillnameLabel[i]->setColor(ccYELLOW);
			skillnameLabel[i]->setPosition(ccp(offsetX,nPageSize.height-offsetY));
			_PageLayer_2->addChild(skillnameLabel[i]);
			offsetX +=100;
			char nSkillKey[32],nNameKey[64],nSkillName[64];
			int  nSkillId=0;

			t_sprintf(nSkillKey,"skill_%d",i);
			t_sprintf(nNameKey,"%s_name",nSkillKey);
			nAutoSkill.GetInteger(nFactionNo+1,nSkillKey,0,&nSkillId);

			if (nSkillId>0)
			{
				nAutoSkill.GetString(nFactionNo+1,nNameKey,"",nSkillName,sizeof(nSkillName));
				//设置技能名称
				skillnameLabel[i]->setString(UTEXT(nSkillName,1).c_str());
			}
		}
	}
	_PageLayer_2->setVisible(false);
//--------物品１设置面板------------------------
	_PageLayer_3 = UILayer::create();//物品１
	_PageLayer_3->setContentSize(nPageSize);
	_PageLayer_3->setPosition(CCPointPage);
	_PageLayer_3->setVisible(false);
	ParentNode_Team->addChild(_PageLayer_3);

	CCLabelTTF* pfenleiSetLabel = CCLabelTTF::create(UTEXT("拾取分类设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	pfenleiSetLabel->setAnchorPoint(ccp(0,0));
	pfenleiSetLabel->setColor(ccYELLOW);
	pfenleiSetLabel->setPosition(ccp(10,nPageSize.height-18));
	_PageLayer_3->addChild(pfenleiSetLabel);
	//--------------
	CCLabelTTF* pfenleiLabel1 = CCLabelTTF::create(UTEXT("银两",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenleiLabel1->setAnchorPoint(ccp(0,0));
	pfenleiLabel1->setPosition(ccp(15,nPageSize.height-35));
	_PageLayer_3->addChild(pfenleiLabel1);

	switchControl_item_m = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_item_m->setPosition(ccp(pfenleiLabel1->getPosition().x+pfenleiLabel1->getContentSize().width*2,pfenleiLabel1->getPosition().y+6));
	_PageLayer_3->addChild(switchControl_item_m);
	switchControl_item_m->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("CCCC","IsQian",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_item_m->setOn(true);
	else
		switchControl_item_m->setOn(false);
	valueChanged(switchControl_item_m, CCControlEventValueChanged);
	switchControl_item_m->setTouchEnabled(false);
	//--------------
	CCLabelTTF* pfenleiLabel2 = CCLabelTTF::create(UTEXT("道具",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenleiLabel2->setAnchorPoint(ccp(0,0));
	pfenleiLabel2->setPosition(ccp(100,nPageSize.height-35));
	_PageLayer_3->addChild(pfenleiLabel2);

	switchControl_item_d = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_item_d->setPosition(ccp(pfenleiLabel2->getPosition().x+pfenleiLabel2->getContentSize().width*2,pfenleiLabel2->getPosition().y+6));
	_PageLayer_3->addChild(switchControl_item_d);
	switchControl_item_d->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	
	nAutoNeiGua.GetInteger("CCCC","IsDaoJu",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_item_d->setOn(true);
	else
		switchControl_item_d->setOn(false);
	valueChanged(switchControl_item_d, CCControlEventValueChanged);
	switchControl_item_d->setTouchEnabled(false);
	//--------------
	CCLabelTTF* pfenleiLabel3 = CCLabelTTF::create(UTEXT("装备",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenleiLabel3->setAnchorPoint(ccp(0,0));
	pfenleiLabel3->setPosition(ccp(185,nPageSize.height-35));
	_PageLayer_3->addChild(pfenleiLabel3);

	switchControl_item_z = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_item_z->setPosition(ccp(pfenleiLabel3->getPosition().x+pfenleiLabel3->getContentSize().width*2,pfenleiLabel3->getPosition().y+6));
	_PageLayer_3->addChild(switchControl_item_z);
	switchControl_item_z->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("CCCC","IsZhuang",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_item_z->setOn(true);
	else
		switchControl_item_z->setOn(false);
	valueChanged(switchControl_item_z, CCControlEventValueChanged);
	switchControl_item_z->setTouchEnabled(false);
	//--------------
	CCLabelTTF* pfenleiLabel5 = CCLabelTTF::create(UTEXT("图志",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenleiLabel5->setAnchorPoint(ccp(0,0));
	pfenleiLabel5->setPosition(ccp(15,nPageSize.height-55));
	_PageLayer_3->addChild(pfenleiLabel5);

	switchControl_item_t = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_item_t->setPosition(ccp(pfenleiLabel5->getPosition().x+pfenleiLabel5->getContentSize().width*2,pfenleiLabel5->getPosition().y+6));
	_PageLayer_3->addChild(switchControl_item_t);
	switchControl_item_t->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("CCCC","IsTuZhi",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_item_t->setOn(true);
	else
		switchControl_item_t->setOn(false);
	valueChanged(switchControl_item_t, CCControlEventValueChanged);
	switchControl_item_t->setTouchEnabled(false);
	//--------------
	CCLabelTTF* pfenleiLabel4 = CCLabelTTF::create(UTEXT("钱与装备",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pfenleiLabel4->setAnchorPoint(ccp(0,0));
	pfenleiLabel4->setPosition(ccp(100,nPageSize.height-55));
	_PageLayer_3->addChild(pfenleiLabel4);

	switchControl_item_zm = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_item_zm->setPosition(ccp(pfenleiLabel4->getPosition().x+pfenleiLabel4->getContentSize().width*1.5,pfenleiLabel4->getPosition().y+6));
	_PageLayer_3->addChild(switchControl_item_zm);
	switchControl_item_zm->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("CCCC","IsQianZhuang",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_item_zm->setOn(true);
	else
		switchControl_item_zm->setOn(false);
	valueChanged(switchControl_item_zm, CCControlEventValueChanged);
	switchControl_item_zm->setTouchEnabled(false);
	CCLabelTTF* pzhuangSetLabel = CCLabelTTF::create(UTEXT("装备设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	pzhuangSetLabel->setAnchorPoint(ccp(0,0));
	pzhuangSetLabel->setColor(ccYELLOW);
	pzhuangSetLabel->setPosition(ccp(10,nPageSize.height-75));
	_PageLayer_3->addChild(pzhuangSetLabel);

	CCLabelTTF* ptoolSetLabel = CCLabelTTF::create(UTEXT("道具设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	ptoolSetLabel->setAnchorPoint(ccp(0,0));
	ptoolSetLabel->setColor(ccYELLOW);
	ptoolSetLabel->setPosition(ccp(150,nPageSize.height-75));
	_PageLayer_3->addChild(ptoolSetLabel);

	CCLabelTTF* puseSetLabel = CCLabelTTF::create(UTEXT("使用设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	puseSetLabel->setAnchorPoint(ccp(0,0));
	puseSetLabel->setColor(ccYELLOW);
	puseSetLabel->setPosition(ccp(290,nPageSize.height-75));
	_PageLayer_3->addChild(puseSetLabel);

	CCLabelTTF* pguanjianLabel_z = CCLabelTTF::create(UTEXT("关键字",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pguanjianLabel_z->setAnchorPoint(ccp(0,0));
	pguanjianLabel_z->setPosition(ccp(10,nPageSize.height-180));
	_PageLayer_3->addChild(pguanjianLabel_z);

	gBg = CCScale9Sprite::create("ui/editbg.png");
	pstrEditBox_s = CCEditBox::create(CCSize(70,24),gBg);
	if (pstrEditBox_s)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pstrEditBox_s->setPlaceHolder(UTEXT("关键字,0",1).c_str());
		pstrEditBox_s->setFontColor(ccYELLOW);
		pstrEditBox_s->setFontSize(24);
		pstrEditBox_s->setMaxLength(12);
		pstrEditBox_s->setPosition(ccp(pguanjianLabel_z->getPosition().x+pguanjianLabel_z->getContentSize().width+40,nPageSize.height-174));
		pstrEditBox_s->setInputMode(kEditBoxInputModeAny);
		pstrEditBox_s->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_3->addChild(pstrEditBox_s);

		//nAutoNeiGua.GetString("BBBB","FindNpcDis","0",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		//pXunguaiEditBox->setText(nTempStr);
	}

	_button = UIButton::create();
	_button->setTouchEnabled(false);
	_button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	_button->setScale9Enabled(true);
	_button->setSize(btnSize);
	_button->setTitleFontSize(14);
	_button->setTitleText(UTEXT("保存修改",1).c_str());
	_button->setPosition(ccp(10+btnSize.width/2,nPageSize.height-195));
	_button->setTag(3);
	_button->setTagbyKey("3");
	_button->addTouchEventListener(this,toucheventselector(KuiAutoPlay::oktouchEvent));
	_PageLayer_3->addWidget(_button);

	CCLabelTTF* pguanjianLabel_d = CCLabelTTF::create(UTEXT("关键字",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pguanjianLabel_d->setAnchorPoint(ccp(0,0));
	pguanjianLabel_d->setPosition(ccp(150,nPageSize.height-180));
	_PageLayer_3->addChild(pguanjianLabel_d);

	gBg = CCScale9Sprite::create("ui/editbg.png");
	pstrEditBox_d = CCEditBox::create(CCSize(70,24),gBg);
	if (pstrEditBox_d)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pstrEditBox_d->setPlaceHolder(UTEXT("关键字",1).c_str());
		pstrEditBox_d->setFontColor(ccYELLOW);
		pstrEditBox_d->setFontSize(24);
		pstrEditBox_d->setMaxLength(12);
		pstrEditBox_d->setPosition(ccp(pguanjianLabel_d->getPosition().x+pguanjianLabel_d->getContentSize().width+40,nPageSize.height-174));
		pstrEditBox_d->setInputMode(kEditBoxInputModeAny);
		pstrEditBox_d->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_3->addChild(pstrEditBox_d);

		//nAutoNeiGua.GetString("BBBB","FindNpcDis","0",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		//pXunguaiEditBox->setText(nTempStr);
	}

	_button_1 = UIButton::create();
	_button_1->setTouchEnabled(false);
	_button_1->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	_button_1->setScale9Enabled(true);
	_button_1->setSize(btnSize);
	_button_1->setTitleFontSize(14);
	_button_1->setTitleText(UTEXT("保存修改",1).c_str());
	_button_1->setPosition(ccp(150+btnSize.width/2,nPageSize.height-195));
	_button_1->setTag(4);
	_button_1->setTagbyKey("4");
	_button_1->addTouchEventListener(this,toucheventselector(KuiAutoPlay::oktouchEvent));
	_PageLayer_3->addWidget(_button_1);

	CCLabelTTF* pguanjianLabel_u = CCLabelTTF::create(UTEXT("关键字",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pguanjianLabel_u->setAnchorPoint(ccp(0,0));
	pguanjianLabel_u->setPosition(ccp(290,nPageSize.height-180));
	_PageLayer_3->addChild(pguanjianLabel_u);

	gBg = CCScale9Sprite::create("ui/editbg.png");
	pstrEditBox_u = CCEditBox::create(CCSize(70,24),gBg);
	if (pstrEditBox_u)
	{
		//pEditBox->setContentSize()
		//pMoneyEditBox->setText("your money");
		pstrEditBox_u->setPlaceHolder(UTEXT("关键字",1).c_str());
		pstrEditBox_u->setFontColor(ccYELLOW);
		pstrEditBox_u->setFontSize(24);
		pstrEditBox_u->setMaxLength(12);
		pstrEditBox_u->setPosition(ccp(pguanjianLabel_u->getPosition().x+pguanjianLabel_u->getContentSize().width+40,nPageSize.height-174));
		pstrEditBox_u->setInputMode(kEditBoxInputModeAny);
		pstrEditBox_u->setReturnType(kKeyboardReturnTypeDone);
		//pMoneyEditBox->setDelegate(this);
		//pMoneyEditBox->setDefaultTouchPriority(-128);
		_PageLayer_3->addChild(pstrEditBox_u);

		//nAutoNeiGua.GetString("BBBB","FindNpcDis","0",nTempStr,sizeof(nTempStr));
		//nTempInt = g_Atoui(nTempStr);
		//pXunguaiEditBox->setText(nTempStr);
	}

	_button_2 = UIButton::create();
	_button_2->setTouchEnabled(false);
	_button_2->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	_button_2->setScale9Enabled(true);
	_button_2->setSize(btnSize);
	_button_2->setTitleFontSize(14);
	_button_2->setTitleText(UTEXT("保存修改",1).c_str());
	_button_2->setPosition(ccp(290+btnSize.width/2,nPageSize.height-195));
	_button_2->setTag(5);
	_button_2->setTagbyKey("5");
	_button_2->addTouchEventListener(this,toucheventselector(KuiAutoPlay::oktouchEvent));
	_PageLayer_3->addWidget(_button_2);

//--------物品2设置面板-------------------------
	_PageLayer_4 = UILayer::create();//物品２
	_PageLayer_4->setContentSize(nPageSize);
	_PageLayer_4->setPosition(CCPointPage);
	_PageLayer_4->setVisible(false);
	ParentNode_Team->addChild(_PageLayer_4);
	CCLabelTTF* pitemSetLabel = CCLabelTTF::create(UTEXT("拾取设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	pitemSetLabel->setAnchorPoint(ccp(0,0));
	pitemSetLabel->setColor(ccYELLOW);
	pitemSetLabel->setPosition(ccp(10,nPageSize.height-18));
	_PageLayer_4->addChild(pitemSetLabel);
//--------其他设置面板-------------------------
	_PageLayer_5 = UILayer::create();//其他
	_PageLayer_5->setContentSize(nPageSize);
	_PageLayer_5->setPosition(CCPointPage);
	_PageLayer_5->setVisible(false);
	ParentNode_Team->addChild(_PageLayer_5);
	CCLabelTTF* potherSetLabel = CCLabelTTF::create(UTEXT("其他设置",1).c_str(),UI_GAME_FONT_DEFAULT, 14);//Arial
	potherSetLabel->setAnchorPoint(ccp(0,0));
	potherSetLabel->setColor(ccYELLOW);
	potherSetLabel->setPosition(ccp(10,nPageSize.height-18));
	_PageLayer_5->addChild(potherSetLabel);

	CCLabelTTF* psoundLabel1 = CCLabelTTF::create(UTEXT("游戏音效",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	psoundLabel1->setAnchorPoint(ccp(0,0));
	psoundLabel1->setPosition(ccp(15,nPageSize.height-35));
	_PageLayer_5->addChild(psoundLabel1);

	switchControl_other_s = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_other_s->setTag(BOX_TAG_SOUND);
	switchControl_other_s->setPosition(ccp(psoundLabel1->getPosition().x+psoundLabel1->getContentSize().width*2,psoundLabel1->getPosition().y+6));
	_PageLayer_5->addChild(switchControl_other_s);
	switchControl_other_s->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("EEEE","IsOenSound",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_other_s->setOn(true);
	else
		switchControl_other_s->setOn(false);
	valueChanged(switchControl_other_s, CCControlEventValueChanged);
	switchControl_other_s->setTouchEnabled(false);

	//开关自动调整距离
	CCLabelTTF* pjuliLabel1 = CCLabelTTF::create(UTEXT("自动调整攻击距离",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pjuliLabel1->setAnchorPoint(ccp(0,0));
	pjuliLabel1->setPosition(ccp(15,nPageSize.height-60));
	_PageLayer_5->addChild(pjuliLabel1);

	switchControl_other_j = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_other_j->setTag(BOX_TAG_JULI);
	switchControl_other_j->setPosition(ccp(pjuliLabel1->getPosition().x+pjuliLabel1->getContentSize().width+20,pjuliLabel1->getPosition().y+6));
	_PageLayer_5->addChild(switchControl_other_j);
	switchControl_other_j->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("EEEE","IsOenjuli",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_other_j->setOn(true);
	else
		switchControl_other_j->setOn(false);
	valueChanged(switchControl_other_j, CCControlEventValueChanged);
	switchControl_other_j->setTouchEnabled(false);

	//开关阴影
	CCLabelTTF* pShadomLabel1 = CCLabelTTF::create(UTEXT("NPC阴影",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pShadomLabel1->setAnchorPoint(ccp(0,0));
	pShadomLabel1->setPosition(ccp(15,nPageSize.height-85));
	_PageLayer_5->addChild(pShadomLabel1);

	switchControl_other_y = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_other_y->setTag(BOX_TAG_SHADOM);
	switchControl_other_y->setPosition(ccp(pShadomLabel1->getPosition().x+pShadomLabel1->getContentSize().width+20,pShadomLabel1->getPosition().y+6));
	_PageLayer_5->addChild(switchControl_other_y);
	switchControl_other_y->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("EEEE","IsOenShadom",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_other_y->setOn(true);
	else
		switchControl_other_y->setOn(false);
	valueChanged(switchControl_other_y, CCControlEventValueChanged);
	switchControl_other_y->setTouchEnabled(false);

	//开关称号
	CCLabelTTF* pTitleLabel1 = CCLabelTTF::create(UTEXT("人物称号",1).c_str(),UI_GAME_FONT_DEFAULT, 12);//Arial
	pTitleLabel1->setAnchorPoint(ccp(0,0));
	pTitleLabel1->setPosition(ccp(15,nPageSize.height-110));
	_PageLayer_5->addChild(pTitleLabel1);

	switchControl_other_t = CCControlSwitch::create
		(
		CCSprite::create("ui/switch/switch-mask.png"),
		CCSprite::create("ui/switch/switch-on.png"),
		CCSprite::create("ui/switch/switch-off.png"),
		CCSprite::create("ui/switch/switch-thumb.png"),
		CCLabelTTF::create(UTEXT("开",1).c_str(),UI_GAME_FONT_DEFAULT,12),
		CCLabelTTF::create(UTEXT("关",1).c_str(),UI_GAME_FONT_DEFAULT,12)
		);
	switchControl_other_t->setTag(BOX_TAG_TITLE);
	switchControl_other_t->setPosition(ccp(pTitleLabel1->getPosition().x+pTitleLabel1->getContentSize().width+20,pTitleLabel1->getPosition().y+6));
	_PageLayer_5->addChild(switchControl_other_t);
	switchControl_other_t->addTargetWithActionForControlEvents(this, cccontrol_selector(KuiAutoPlay::valueChanged), CCControlEventValueChanged);
	nAutoNeiGua.GetInteger("EEEE","IsOenTitle",0,&nisOpen);
	if  (nisOpen>0)
		switchControl_other_t->setOn(true);
	else
		switchControl_other_t->setOn(false);
	valueChanged(switchControl_other_t, CCControlEventValueChanged);
	switchControl_other_t->setTouchEnabled(false);

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
	for (int i = 0; i < 32; ++i)
	{
		CCString* ccstr = CCString::createWithFormat("属性关键字_%d", i);
		m_array->addObject(ccstr);
	}*/
	//CCSize widgetSize = m_pWidget->getSize();
	// Create the list view
	char nsetionKey[32],nDestStr[64];
	float list_w = 110;
	float list_h = 80;
	listView_Item = UIListView::create();
	// set list view direction
	listView_Item->setDirection(SCROLLVIEW_DIR_VERTICAL);
	listView_Item->setTouchEnabled(false);
	listView_Item->setBounceEnabled(true);
	listView_Item->setBackGroundImage("loadingStart.png");
	listView_Item->setBackGroundImageScale9Enabled(true);
	//listView_Item->setBackGroundColorOpacity(0);
	listView_Item->setBackGroundImageOpacity(0);
	listView_Item->setSize(CCSizeMake(list_w, list_h));
	//listView_Item->setInnerContainerSize(CCSizeMake(110, 110));
	//listView_Item->setPosition(ccp(m_size.width/2+40,m_size.height/2-50));
	listView_Item->setPosition(ccp(10,nPageSize.height-80-list_h));
	listView_Item->setTag(LIST_VIEW_ITEM);
	listView_Item->addEventListenerListView(this, listvieweventselector(KuiAutoPlay::selectedItemEvent));
	_PageLayer_3->addWidget(listView_Item);

//	int count = m_array->count();
	// add custom item
	for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
	{
		custom_button_item[i] = UIButton::create();
		custom_button_item[i]->setName("TextButton");
		custom_button_item[i]->setTouchEnabled(false);
		custom_button_item[i]->loadTextures("ui/textbg.png", "ui/state/life_pk.png", "");
		custom_button_item[i]->setScale9Enabled(true);
		custom_button_item[i]->setSize(btnSize);
		custom_button_item[i]->setTitleText(UTEXT("关键字,0",1));
		t_sprintf(nsetionKey,"item_%d",i);
		char nTempStr[32],nTempStr_1[128];
		ZeroMemory(nTempStr,sizeof(nTempStr));
		ZeroMemory(nTempStr_1,sizeof(nTempStr_1));
		nAutoZhuang.GetString(i+1,1,"关键字",nDestStr,sizeof(nDestStr));
		nAutoZhuang.GetInteger(i+1,2,0,&nTempInt);
		t_sprintf(nTempStr_1,"%s,%d",nDestStr,nTempInt);
		if (nTempStr_1[0])
			custom_button_item[i]->setTitleText(UTEXT(nTempStr_1,1));

		Layout *custom_item = Layout::create();
		custom_item->setSize(custom_button_item[i]->getSize());
		custom_button_item[i]->setPosition(ccp(custom_item->getSize().width/2, custom_item->getSize().height/2));
		custom_item->addChild(custom_button_item[i]);

		listView_Item->pushBackCustomItem(custom_item);
	}
	listView_Item->refreshView();
	listView_Item->jumpToBottom();


	listView_tool = UIListView::create();
	// set list view direction
	listView_tool->setDirection(SCROLLVIEW_DIR_VERTICAL);
	listView_tool->setTouchEnabled(false);
	listView_tool->setBounceEnabled(true);
	listView_tool->setBackGroundImage("loadingStart.png");
	listView_tool->setBackGroundImageScale9Enabled(true);
	//listView_Item->setBackGroundColorOpacity(0);
	listView_tool->setBackGroundImageOpacity(0);
	listView_tool->setSize(CCSizeMake(list_w, list_h));
	//listView_Item->setInnerContainerSize(CCSizeMake(110, 110));
	//listView_Item->setPosition(ccp(m_size.width/2+40,m_size.height/2-50));
	listView_tool->setPosition(ccp(150,nPageSize.height-80-list_h));
	listView_tool->setTag(LIST_VIEW_TOOL);
	listView_tool->addEventListenerListView(this, listvieweventselector(KuiAutoPlay::selectedItemEvent));
	_PageLayer_3->addWidget(listView_tool);

	//	int count = m_array->count();
	// add custom item
	for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
	{
		custom_button_tool[i] = UIButton::create();
		custom_button_tool[i]->setName("TextButton");
		custom_button_tool[i]->setTouchEnabled(false);
		custom_button_tool[i]->loadTextures("ui/textbg.png", "ui/state/life_pk.png", "");
		custom_button_tool[i]->setScale9Enabled(true);
		custom_button_tool[i]->setSize(btnSize);
		custom_button_tool[i]->setTitleText(UTEXT("道具关键字",1));
		//t_sprintf(nsetionKey,"tool_%d",i);
		nAutoDaiju.GetString(i+1,1,"道具关键字",nDestStr,sizeof(nDestStr));
		if (nDestStr[0])
			custom_button_tool[i]->setTitleText(UTEXT(nDestStr,1));

		Layout *custom_item = Layout::create();
		custom_item->setSize(custom_button_tool[i]->getSize());
		custom_button_tool[i]->setPosition(ccp(custom_item->getSize().width/2, custom_item->getSize().height/2));
		custom_item->addChild(custom_button_tool[i]);

		listView_tool->pushBackCustomItem(custom_item);
	}
	listView_tool->refreshView();
	listView_tool->jumpToBottom();


	listView_use = UIListView::create();
	// set list view direction
	listView_use->setDirection(SCROLLVIEW_DIR_VERTICAL);
	listView_use->setTouchEnabled(false);
	listView_use->setBounceEnabled(true);
	listView_use->setBackGroundImage("loadingStart.png");
	listView_use->setBackGroundImageScale9Enabled(true);
	//listView_Item->setBackGroundColorOpacity(0);
	listView_use->setBackGroundImageOpacity(0);
	listView_use->setSize(CCSizeMake(list_w, list_h));
	//listView_Item->setInnerContainerSize(CCSizeMake(110, 110));
	//listView_Item->setPosition(ccp(m_size.width/2+40,m_size.height/2-50));
	listView_use->setPosition(ccp(290,nPageSize.height-80-list_h));
	listView_use->setTag(LIST_VIEW_USE);
	listView_use->addEventListenerListView(this, listvieweventselector(KuiAutoPlay::selectedItemEvent));
	_PageLayer_3->addWidget(listView_use);

	//	int count = m_array->count();
	// add custom item
	for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
	{
		custom_button_use[i] = UIButton::create();
		custom_button_use[i]->setName("TextButton");
		custom_button_use[i]->setTouchEnabled(false);
		custom_button_use[i]->loadTextures("ui/textbg.png", "ui/state/life_pk.png", "");
		custom_button_use[i]->setScale9Enabled(true);
		custom_button_use[i]->setSize(btnSize);
		custom_button_use[i]->setTitleText(UTEXT("物品关键字",1));
		//t_sprintf(nsetionKey,"use_%d",i);
		nAutoUseItem.GetString(i+1,1,"物品关键字",nDestStr,sizeof(nDestStr));
		if (nDestStr[0])
			custom_button_use[i]->setTitleText(UTEXT(nDestStr,1));
		Layout *custom_item = Layout::create();
		custom_item->setSize(custom_button_use[i]->getSize());
		custom_button_use[i]->setPosition(ccp(custom_item->getSize().width/2, custom_item->getSize().height/2));
		custom_item->addChild(custom_button_use[i]);

		listView_use->pushBackCustomItem(custom_item);
	}
	listView_use->refreshView();
	listView_use->jumpToBottom();

    /*pMoneyLabel = CCLabelTTF::create("","fonts/gb_song.ttf",12);//Arial
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(60,128));
	ParentNode_Team->addChild(pMoneyLabel,1);

	pXuLabel = CCLabelTTF::create("","fonts/gb_song.ttf",12);//Arial
	pXuLabel->setAnchorPoint(ccp(0,0));
	pXuLabel->setPosition(ccp(60,105));
	ParentNode_Team->addChild(pXuLabel,1);*/
	
	//ccColor4B color = ccc4(112, 128, 144, 150);
	/*colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);  //设置和窗口的大小
	ParentNode_Team->addChild(colorLayer,1000);*/
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色
	//red->retain();
}

void KuiAutoPlay::valueChanged(CCObject* sender, CCControlEvent controlEvent)
{
	if (!sender || !g_pCoreShell) return;

	CCControlSwitch* pSwitch = (CCControlSwitch*)sender;
	int tag = pSwitch->getTag();
	switch(tag)
	{
	case BOX_TAG_TITLE:
		{
			if (pSwitch->isOn())
				nIsShowTitle = true;
			else
				nIsShowTitle = false;
		}
		break;
	case BOX_TAG_SHADOM:
		{
			if (pSwitch->isOn())
				nIsShowShadow = true;
			else
				nIsShowShadow = false;
		}
	    break;
	case BOX_TAG_JULI:
		{
			if (pSwitch->isOn())
			  Player[CLIENT_PLAYER_INDEX].m_isopenjuli = true;
			else
              Player[CLIENT_PLAYER_INDEX].m_isopenjuli = false;
		}
		break;
	case BOX_TAG_SHIQU:
		{
			if (!pSwitch->isOn())
			{
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney = 0;           //钱
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques= 0;           //装备
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques= 0;    //钱和装备
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu= 0;             //神秘图志
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool= 0; 
			}
			            //道具
		}
		break;
	case BOX_TAG_TEAM_3:
		{//我是队员
			if (!switchControl_team || !switchControl_team->isOn())
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:请开启自动应答！");
				if (pSwitch->isOn())
					pSwitch->setOn(false);

				break;
			}

			if (pSwitch->isOn())
			{
				KUiPlayerTeam mTeam;
				ZeroMemory(&mTeam,sizeof(mTeam));
				if (g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&mTeam, 0)) //没有队伍 就创建一支队伍
				{
					if (mTeam.bTeamLeader)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:已经是队长,不能选择队员！");
						pSwitch->setOn(false);
						break;
					}
					else
					{//退出队伍
						g_pCoreShell->TeamOperation(TEAM_OI_LEAVE, 0, 0);
					}
				}
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,12,TRUE);
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,12,FALSE);
			}
			//if  (switchControl_team_d)
			//     switchControl_team_d->setOn(false);
		}
		break;
	case BOX_TAG_TEAM_2:
		{//我是队长
			if (!switchControl_team || !switchControl_team->isOn())
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:请开启自动应答！");
				if (pSwitch->isOn())
					pSwitch->setOn(false);

				break;
			}

			if (pSwitch->isOn())
			{
				KUiPlayerTeam mTeam;
				ZeroMemory(&mTeam,sizeof(mTeam));
				if (!g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&mTeam, 0)) //没有队伍 就创建一支队伍
				{
					g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
				}

				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,11,TRUE);
			}
			else
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,11,FALSE);

			//if (switchControl_team_y)
			 //   switchControl_team_y->setOn(false);
		}
		break;
	case BOX_TAG_TEAM_1:
		{//开启自动应答
			if (pSwitch->isOn())
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,10,TRUE);
			else
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,10,FALSE);
		}
		break;
	case BOX_TAG_GENSHUI:
		{//跟随队长
			if (pSwitch->isOn())
			{
				KUiPlayerTeam mTeam;
				ZeroMemory(&mTeam,sizeof(mTeam));
				if (!g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&mTeam, 0))
				{//没有队伍,不能开启
					//g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
					//UIMessageBox2("提示:没有加入队伍,不能设置！");
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:没有加入队伍,不能设置！");
					pSwitch->setOn(false);
					break;
				}
				else
				{
					if (mTeam.bTeamLeader)
					{//是队长 不能跟随自己
						//UIMessageBox2("提示:已经是队长,不能设置！");
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:已经是队长,不能设置！");
						pSwitch->setOn(false);
						break;
					}
				}
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,29,TRUE);
			}
			else
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,29,FALSE);
		}
		break;
	case BOX_TAG_USEITEM:
		{
			if (pSwitch->isOn())                        //开启使用物品
			{
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,26,TRUE);
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,26,FALSE);
			}
		}
		break;
	case BOX_TAG_MIAOSHA:
		{
			if (pSwitch->isOn())
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,28,TRUE);
			else
			    g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,28,FALSE);
		}
		break;
	case BOX_TAG_SOUND:
		{//声音开关
			if (pSwitch->isOn())
			{//pshuxEditBox->setText(nTempStr);
				_openSound = true;
			    std::string m_WritablePath = m_SDcardDirPath;
				if  (GetRandomNumber(1,10)>=5)
					m_WritablePath += "music/start.mp3"; //download wav
				else
					m_WritablePath += "music/start.wav";
				//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(m_WritablePath.c_str());//专门用来播放简短的音效
				if  (__pSound)
					__pSound->playBackgroundMusic(m_WritablePath.c_str(),false);//播放音乐
				
			}
			else
			{
				_openSound = false;
				if  (__pSound)
					__pSound->stopBackgroundMusic(true);//播放音乐
			}
		}
		break;
	case BOX_TAG_BAOLIU:
		{
			if (pSwitch->isOn())
			{//pshuxEditBox->setText(nTempStr);
				if (pshuxEditBox)
					pshuxEditBox->setText("0");
			}
			else
			{
				if (pshuxEditBox)
					pshuxEditBox->setText("1");
			}
		}
		break;
	case BOX_TAG_LUXIAN:
		{
			if (pSwitch->isOn())
			{
			  if (switchControl_dingdian)
			     switchControl_dingdian->setOn(false);
			  g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,9,true);
			}
			else
			{

			}
		}
		break;
	case BOX_TAG_DING:
		{
			if (pSwitch->isOn())
			{
				if (switchControl_luxian)
				    switchControl_luxian->setOn(false);
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,9,false);
			}
			else
			{

			}
		}
		break;
	case BOX_TAG_YABIAO:
		{	if (pSwitch->isOn())
			    g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,7,false);
		}
		break;
	case BOX_TAG_LUZHI:
		{//录制脚本
			if (g_pCoreShell->GetAutoplayid())	//获取挂机的状态
			{
				//pSwitch->setOn(false);
                Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:请先停止挂机");
				break;
			}
			if (pSwitch->isOn())
			{//录制路线
				//g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,8,FALSE,FALSE);
				if  (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen)
					break;
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,8,TRUE,TRUE);
				//CCMessageBox("开启","提示");
				//UIMessageBox2("提示:\n  1.请走动人物,录制你需要的挂机路线;完毕后,取消勾选<录制路线>=关闭录制!\n  2.挂机时需要选择<路线>模式\n  3.每次勾选<录制路线>都会清除原路线!\n  4.两点之间的距离需小于活动范围");
			} 
			else
			{//停止录制
			   //g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,8,TRUE,TRUE);
				if (g_pCoreShell->GetAutoplayid())	//获取挂机的状态
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:请先停止挂机");
					break;
				}
				if  (!Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen)
					break;
			   g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,8,FALSE,FALSE);
			   //CCMessageBox("关闭","提示");
			}
		}
		break;
	case BOX_TAG_BASE:
		{
			if (pSwitch->isOn())
			{
			} 
			else
			{
			}
		}
		break;
	default:
		break;
	}

}

void KuiAutoPlay::checkBoxStateEvent(CCObject* pSender, CheckBoxEventType type)
{
	if  (!g_pCoreShell || !g_GameWorld) return;
	switch (type)
	{
	case CHECKBOX_STATE_EVENT_UNSELECTED:
		break;
	case CHECKBOX_STATE_EVENT_SELECTED:
		break;
	default:
		break;
	}
}

void KuiAutoPlay::selectedBoxStateEvent(CCObject* pSender, CheckBoxEventType type)
{
	if  (!g_pCoreShell || !g_GameWorld) return;
	switch (type)
	{
	case CHECKBOX_STATE_EVENT_UNSELECTED:
		{
		}
		break;
	case CHECKBOX_STATE_EVENT_SELECTED:
		{
			if (pSender)
			{
				UICheckBox* nTbtn = (UICheckBox*)pSender;
				switch(nTbtn->getTag())
				{
				case BOX_TAG_BASE:
					{
						CCMessageBox("选中","选中");
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}
//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiAutoPlay::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;
	
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	//"\\spr\\UI3\\主界面\\关闭.spr"
	t_sprintf(nSprName,"\\spr\\Ui3\\activityguide\\closebutton.spr"); //"\\spr\\Ui3\\组队\\关闭.spr" "\\Spr\\Ui3\\特殊对话条\\输入文字条－取消.spr"
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));

	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_colse_normal = CCSprite::createWithTexture(bgCur);
	sprite_colse_normal->setAnchorPoint(ccp(0,0));
	bgCur = _getinidata.getinidata_one(nSprName,2,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCSprite * sprite_colse_select = CCSprite::createWithTexture(bgCur);
	sprite_colse_select->setAnchorPoint(ccp(0,0));

	CCMenuItemSprite * colseConfirm = CCMenuItemSprite::create(sprite_colse_normal, sprite_colse_select, this, menu_selector(KuiAutoPlay::buttonCallBackFunc));
	colseConfirm->setPosition(ccp(m_size.width/2,14));
	//加入菜单
	CCMenu * menu = CCMenu::create(colseConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//菜单位置设置为弹出框左下
	menu->setAnchorPoint(ccp(0,0));
	menu->setPosition(ccp(0,0));             //菜单位置设置为弹出框左下
	ParentNode_Team->addChild(menu);         //加到父层里

	// Create the checkbox
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\UI3\\状态与装备\\分页.spr");
	g_StrLower(nSprName);

	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCTexture2D * check_btn_normal = bgCur;
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return ;
	CCTexture2D * check_btn_active = bgCur;

	float nTempFontSize =14;
	float nOffsetY = 34;
	
	btnPageBox_1 = UIButton::create();
	btnPageBox_1->setTouchEnabled(true);
	btnPageBox_1->loadTexturesByData(check_btn_normal,check_btn_active,check_btn_normal);
	btnPageBox_1->setTitleFontSize(nTempFontSize);
	btnPageBox_1->setTitleText(UTEXT("基本设置",1).c_str());
	//btnPageBox_1->setAnchorPoint(ccp(0,0));
	btnPageBox_1->setPosition(ccp(60,m_size.height-nOffsetY));
	btnPageBox_1->setTag(AUTOBTN_PAGE_BASE);
	btnPageBox_1->setTagbyKey("1");
	//btnPageBox_1->ignoreContentAdaptWithSize(true);
	btnPageBox_1->addTouchEventListener(this,toucheventselector(KuiAutoPlay::touchPageBtnEvent));
	m_pUiLayer->addWidget(btnPageBox_1);

	btnPageBox_2 = UIButton::create();
	btnPageBox_2->setTouchEnabled(true);
	btnPageBox_2->loadTexturesByData(check_btn_normal,check_btn_active,check_btn_normal);
	btnPageBox_2->setTitleFontSize(nTempFontSize);
	btnPageBox_2->setTitleText(UTEXT("攻击设置",1).c_str());
	//btnPageBox_1->setAnchorPoint(ccp(0,0));
	btnPageBox_2->setPosition(ccp(135,m_size.height-nOffsetY));
	btnPageBox_2->setTag(AUTOBTN_PAGE_ART);
	btnPageBox_2->setTagbyKey("2");
	//btnPageBox_1->ignoreContentAdaptWithSize(true);
	btnPageBox_2->addTouchEventListener(this,toucheventselector(KuiAutoPlay::touchPageBtnEvent));
	m_pUiLayer->addWidget(btnPageBox_2);

	btnPageBox_3 = UIButton::create();
	btnPageBox_3->setTouchEnabled(true);
	btnPageBox_3->loadTexturesByData(check_btn_normal,check_btn_active,check_btn_normal);
	btnPageBox_3->setTitleFontSize(nTempFontSize);
	btnPageBox_3->setTitleText(UTEXT("物品设置1",1).c_str());
	//btnPageBox_1->setAnchorPoint(ccp(0,0));
	btnPageBox_3->setPosition(ccp(210,m_size.height-nOffsetY));
	btnPageBox_3->setTag(AUTOBTN_PAGE_ITEM_1);
	btnPageBox_3->setTagbyKey("3");
	//btnPageBox_1->ignoreContentAdaptWithSize(true);
	btnPageBox_3->addTouchEventListener(this,toucheventselector(KuiAutoPlay::touchPageBtnEvent));
	m_pUiLayer->addWidget(btnPageBox_3);

	btnPageBox_4 = UIButton::create();
	btnPageBox_4->setTouchEnabled(true);
	btnPageBox_4->loadTexturesByData(check_btn_normal,check_btn_active,check_btn_normal);
	btnPageBox_4->setTitleFontSize(nTempFontSize);
	btnPageBox_4->setTitleText(UTEXT("物品设置2",1).c_str());
	//btnPageBox_1->setAnchorPoint(ccp(0,0));
	btnPageBox_4->setPosition(ccp(285,m_size.height-nOffsetY));
	btnPageBox_4->setTag(AUTOBTN_PAGE_ITEM_2);
	btnPageBox_4->setTagbyKey("4");
	//btnPageBox_1->ignoreContentAdaptWithSize(true);
	btnPageBox_4->addTouchEventListener(this,toucheventselector(KuiAutoPlay::touchPageBtnEvent));
	m_pUiLayer->addWidget(btnPageBox_4);

	btnPageBox_5 = UIButton::create();
	btnPageBox_5->setTouchEnabled(true);
	btnPageBox_5->loadTexturesByData(check_btn_normal,check_btn_active,check_btn_normal);
	btnPageBox_5->setTitleFontSize(nTempFontSize);
	btnPageBox_5->setTitleText(UTEXT("其他设置",1).c_str());
	//btnPageBox_1->setAnchorPoint(ccp(0,0));
	btnPageBox_5->setPosition(ccp(360,m_size.height-nOffsetY));
	btnPageBox_5->setTag(AUTOBTN_PAGE_OTHER);
	btnPageBox_5->setTagbyKey("5");
	//btnPageBox_1->ignoreContentAdaptWithSize(true);
	btnPageBox_5->addTouchEventListener(this,toucheventselector(KuiAutoPlay::touchPageBtnEvent));
	m_pUiLayer->addWidget(btnPageBox_5);

	//增加启动　保存按钮
	UIButton* button = UIButton::create();
	button->setTouchEnabled(true);
	button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	button->setTitleFontSize(14);
	button->setTitleText(UTEXT("启动/取消",1).c_str());
	button->setPosition(ccp(m_size.width/2-70,37));
	button->setTag(1);
	button->setTagbyKey("1");
	button->addTouchEventListener(this,toucheventselector(KuiAutoPlay::oktouchEvent));
	m_pUiLayer->addWidget(button);

	UIButton* _button = UIButton::create();
	_button->setTouchEnabled(true);
	_button->loadTextures("ui/textbg.png", "ui/textbg.png", "");
	_button->setTitleFontSize(14);
	_button->setTitleText(UTEXT("保存配置",1).c_str());
	_button->setPosition(ccp(m_size.width/2+70,37));
	_button->setTag(2);
	_button->setTagbyKey("2");
	_button->addTouchEventListener(this,toucheventselector(KuiAutoPlay::oktouchEvent));
	m_pUiLayer->addWidget(_button);

	//开始创建线程
	//Start();
}

//执行上层对象的回调函数，关闭弹出框
void KuiAutoPlay::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = NULL;
	if (pSender)
		node= dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if(g_pCoreShell->GetAutoplayid())	//获取挂机的状态
	{//如果是激活的 就隐藏
		_removeDelegate(false);
		this->setVisible(false);
		return;
	}

	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);
	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiAutoPlay::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

void KuiAutoPlay::clearTempSelindex()
{
	nTempSelItemIndex = -1;
	nTempSeltoolIndex = -1;
	nTempSeluseIndex  = -1;
}
//列表点击
void KuiAutoPlay::selectedItemEvent(CCObject *pSender, ListViewEventType type)
{
	if (!pSender) return;
	char msg[64];
	switch (type)
	{
	case LISTVIEW_ONSELECTEDITEM_START:
		{
			UIListView* listView_ItemEx = static_cast<UIListView*>(pSender);
			if (listView_ItemEx)
			{
				int tag          = listView_ItemEx->getTag();
				int nCurSelIndex = listView_ItemEx->getCurSelectedIndex();
				/*
				int   nTempSelItemIndex;
				int   nTempSeltoolIndex;
				int   nTempSeluseIndex;

				*/
				switch(tag)
				{
				case LIST_VIEW_ITEM:
					{//装备
						nTempSelItemIndex = nCurSelIndex;
						nTempSeltoolIndex = -1;
						nTempSeluseIndex  = -1;
						if (pstrEditBox_s)
						{
							UIWidget *item = listView_ItemEx->getItem(nCurSelIndex);
							UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
							pstrEditBox_s->setText(button->getTitleText());
						}
						
					}
					break;
				case LIST_VIEW_TOOL:
					{//道具
						nTempSelItemIndex = -1;
						nTempSeltoolIndex = nCurSelIndex;
						nTempSeluseIndex  = -1;
						if (pstrEditBox_d)
						{
							UIWidget *item = listView_ItemEx->getItem(nCurSelIndex);
							UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
							pstrEditBox_d->setText(button->getTitleText());
						}
					}
					break;
				case LIST_VIEW_USE:
					{//使用物品
						nTempSelItemIndex = -1;
						nTempSeltoolIndex = -1;
						nTempSeluseIndex  = nCurSelIndex;
						if (pstrEditBox_u)
						{
							UIWidget *item = listView_ItemEx->getItem(nCurSelIndex);
							UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
							pstrEditBox_u->setText(button->getTitleText());
						}

					}
					break;
				default:
					break;
				}
				t_sprintf(msg,"开始:%d",listView_ItemEx->getCurSelectedIndex());
			}
				//CCLOG("select child start index = %d", listView_ItemEx->getCurSelectedIndex());
			break;
		}
	case LISTVIEW_ONSELECTEDITEM_END:
		{
			UIListView* listView_ItemEx = static_cast<UIListView*>(pSender);
			if (listView_ItemEx)
				t_sprintf(msg,"结束:%d",listView_ItemEx->getCurSelectedIndex());
				//CCLOG("select child end index = %d", listView_ItemEx->getCurSelectedIndex());
			break;
		}
	default:
		break;
	}
}


//重写触摸注册函数，重新给定触摸级别
void KuiAutoPlay::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,128, true);
}

CCRect KuiAutoPlay::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}

//点击按钮回调函数
void KuiAutoPlay::oktouchEvent(CCObject *pSender, TouchEventType type)
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
				if  (nTbtn->getTag()==1)
				{//激活取消
					//检测是不是新手村
					int n_mapindex = g_GameWorld->getMapIdx();
					bool isbreak=false;
					switch(n_mapindex)
					{
					case 1:
					case 11:
					case 20:
					case 37:
					case 53:
					case 54:
					case 78:
					case 99:
					case 100:
					case 101:
					case 121:
					case 153:
					case 174:
					case 175:
					case 176:
						isbreak = true;
						break;
					default:
						break;
					}

					if(g_pCoreShell->GetAutoplayid())	//获取挂机的状态
						g_pCoreShell->OperationRequest(GOI_AUTOPALYOPEN,0,-1,100);  //关闭挂机
					else
					{//m_pSelf->nXpos=pInfo->nScenePos0/8;
						//m_pSelf->nYpos=pInfo->nScenePos1/8;
						if  (isbreak)
						{
#ifdef WIN32
							CCMessageBox("该地图不能挂机","提示");
#else
							CCMessageBox(UTEXT("该地图不能挂机",1).c_str(),UTEXT("提示",1).c_str());
#endif
							break;
						}

						g_pCoreShell->Setpos(nPosX/8,nPosY/8);                                        //设定挂机起点
						g_pCoreShell->OperationRequest(GOI_AUTOPALYOPEN,1,-1,101);
					}
				}
				else if  (nTbtn->getTag()==3)
				{//保存修改装备属性
					if (listView_Item && nTempSelItemIndex!=-1 && pstrEditBox_s)
					{
						UIWidget *item = listView_Item->getItem(nTempSelItemIndex);
						UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
						button->setTitleText(pstrEditBox_s->getText());
#ifdef WIN32
						CCMessageBox("修改成功","提示");
#else
						CCMessageBox(UTEXT("修改成功",1).c_str(),UTEXT("提示",1).c_str());
#endif
					}
				}
				else if  (nTbtn->getTag()==4)
				{//保存修改道具
					if (listView_tool && nTempSeltoolIndex!=-1 && pstrEditBox_d)
					{
						UIWidget *item = listView_tool->getItem(nTempSeltoolIndex);
						UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
						button->setTitleText(pstrEditBox_d->getText());
#ifdef WIN32
						CCMessageBox("修改成功","提示");
#else
						CCMessageBox(UTEXT("修改成功",1).c_str(),UTEXT("提示",1).c_str());
#endif
					}
				}
				else if  (nTbtn->getTag()==5)
				{//保存修改使用物品
					if (listView_use && nTempSeluseIndex!=-1 && pstrEditBox_u)
					{
						UIWidget *item = listView_use->getItem(nTempSeluseIndex);
						UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
						button->setTitleText(pstrEditBox_u->getText());
#ifdef WIN32
						CCMessageBox("修改成功","提示");
#else
						CCMessageBox(UTEXT("修改成功",1).c_str(),UTEXT("提示",1).c_str());
#endif
					}
				}
				else if  (nTbtn->getTag()==2)
				{//保存配置
					if(switchControl_baohu->isOn())
						nAutoNeiGua.WriteInteger("AAAA","IsOpenBaohu",1);
					else
						nAutoNeiGua.WriteInteger("AAAA","IsOpenBaohu",0);

					nAutoNeiGua.WriteString("AAAA","nBaohuInfo1",pLifeEditBox->getText());  //血
					nAutoNeiGua.WriteString("AAAA","nBaohuInfo2",pNeiliEditBox->getText()); //蓝
					nAutoNeiGua.WriteString("AAAA","nBaohuInfo3",pLifeEditBox_h->getText());//回城血量


					if(switchControl_zhandou->isOn())
						nAutoNeiGua.WriteInteger("BBBB","IsOpenZhanDou",1);
					else
						nAutoNeiGua.WriteInteger("BBBB","IsOpenZhanDou",0);

					nAutoNeiGua.WriteString("BBBB","FindNpcDis",pXunguaiEditBox->getText());

					nAutoNeiGua.WriteString("BBBB","GongJiFanWei",pGongjiEditBox->getText());
					
					nAutoNeiGua.WriteString("BBBB","GongJiNun",pGongjiEditBox_m->getText());

					nAutoNeiGua.WriteString("BBBB","QuXiaoTime",pEditBox_yanshi->getText());

					if(switchControl_shiqu->isOn())
						nAutoNeiGua.WriteInteger("BBBB","IsOpenShiQu",1);
					else
						nAutoNeiGua.WriteInteger("BBBB","IsOpenShiQu",0);

					nAutoNeiGua.WriteString("BBBB","ShiQuNum",pshiquEditBox_c->getText());
					int nisOpen=0,nsaveMonye=0;
					if  (switchControl_savem->isOn())
						nisOpen =1;
					char nTempMoney[64];
					t_sprintf(nTempMoney,pmoneyEditBox->getText());
					nsaveMonye = g_Atoui(nTempMoney);
					nAutoNeiGua.WriteInteger2("BBBB","IsOpenAutoSave",nisOpen,nsaveMonye);

					if  (switchControl_fenjie->isOn())
						 nisOpen =1;
					else
						 nisOpen =0;
					nAutoNeiGua.WriteInteger("BBBB","IsOpenFenJie",nisOpen);

					if  (switchControl_yanshi->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("BBBB","IsOpenTime",nisOpen);
					char ntempKey[32];
					for (int i=0;i<MAX_SKILL_COUNT;i++)
					{
						if (switchControl_skill[i])
						{
							if  (switchControl_skill[i]->isOn())
								nisOpen =1;
							else
								nisOpen =0;
							t_sprintf(ntempKey,"IsOpenSkill_%d",i);
							nAutoNeiGua.WriteInteger("BBBB",ntempKey,nisOpen);
						}
					}
					nAutoNeiGua.WriteString("CCCC","ShuXingNum",pshuxEditBox->getText());
					if  (switchControl_baoliu->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","AllKeep",nisOpen);

					if  (switchControl_item_m->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","IsQian",nisOpen);
					if  (switchControl_item_d->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","IsDaoJu",nisOpen);
					if  (switchControl_item_t->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","IsTuZhi",nisOpen);
					if  (switchControl_item_z->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","IsZhuang",nisOpen);
					if  (switchControl_item_zm->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("CCCC","IsQianZhuang",nisOpen);
					
					if  (switchControl_useitem->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("DDDD","IsOpenUseIetm",nisOpen);

					if  (switchControl_miaosha->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("DDDD","IsOpenNoBlue",nisOpen);
					

					if  (switchControl_dingdian->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("BBBB","IsXiTong",nisOpen);

					if  (switchControl_luxian->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("BBBB","IsJiaoBen",nisOpen);

					if  (switchControl_other_s->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("EEEE","IsOenSound",nisOpen);

					if  (switchControl_other_j->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("EEEE","IsOenjuli",nisOpen);

					if  (switchControl_other_t->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("EEEE","IsOenTitle",nisOpen);

					if  (switchControl_other_y->isOn())
						nisOpen =1;
					else
						nisOpen =0;
					nAutoNeiGua.WriteInteger("EEEE","IsOenShadom",nisOpen);


					nAutoNeiGua.Save(nFilePath);
					int i;
					char nsetionKey[32],nTempStr[64];
					KTabFileCtrl nTempTxt;
					nTempTxt.CreatFile(SCHEME_INI_AUTO_ITEM);
					if (listView_Item && nTempTxt.Load(SCHEME_INI_AUTO_ITEM))
					{
					  for (i=0;i<MAX_ITEMLIST_COUNT;i++)
					  {
						//t_sprintf(nsetionKey,"item_%d",i);
						UIWidget *item = listView_Item->getItem(i);
						UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
						std::string _nTempStr = button->getTitleText();
						std::string _nstr_a = _nTempStr.substr(0, _nTempStr.rfind(","));
						std::string _nstr_b = _nTempStr.substr(_nTempStr.rfind(",")+1,_nTempStr.length());
						t_sprintf(nsetionKey,_nstr_b.c_str());
						t_sprintf(nTempStr,_nstr_a.c_str());
						int nRows = 0;
						nRows=nTempTxt.GetHeight();
						nTempTxt.InsertAfter(nRows);
						nTempTxt.WriteString(nRows+1,1,nTempStr);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex);
						nTempTxt.WriteInteger(nRows+1,2,g_Atoui(nsetionKey));
						//nTempTxt.WriteInteger(nRows+1,3,nY);
						//nAutoZhuang.WriteString(nsetionKey,"listStr",_nstr_a.c_str());
						//nAutoZhuang.WriteString(nsetionKey,"listStrVer",_nstr_b.c_str());
					  }
						nTempTxt.Save(SCHEME_INI_AUTO_ITEM);
						nTempTxt.Clear();
					}
					
					nTempTxt.CreatFile(SCHEME_INI_AUTO_DAOJU);
					if (listView_tool && nTempTxt.Load(SCHEME_INI_AUTO_DAOJU))
					{
						for (i=0;i<MAX_ITEMLIST_COUNT;i++)
						{
							//t_sprintf(nsetionKey,"tool_%d",i);
							UIWidget *item = listView_tool->getItem(i);
							UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
							int nRows = 0;
							nRows=nTempTxt.GetHeight();
							nTempTxt.InsertAfter(nRows);
							t_sprintf(nTempStr,button->getTitleText());
							nTempTxt.WriteString(nRows+1,1,nTempStr);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex);
							//nTempTxt.WriteInteger(nRows+1,2,g_Atoui(nsetionKey));
							//nAutoDaiju.WriteString(nsetionKey,"listStr",button->getTitleText());
						}
						//nAutoDaiju.Save(SCHEME_INI_AUTO_DAOJU);
						nTempTxt.Save(SCHEME_INI_AUTO_DAOJU);
						nTempTxt.Clear();
					}

					nTempTxt.CreatFile(SCHEME_INI_AUTO_USE);
					if (listView_use && nTempTxt.Load(SCHEME_INI_AUTO_USE))
					{
						for (i=0;i<MAX_ITEMLIST_COUNT;i++)
						{
							//t_sprintf(nsetionKey,"use_%d",i);
							UIWidget *item = listView_use->getItem(i);
							UIButton *button = static_cast<UIButton*>(item->getChildByName("TextButton"));
							int nRows = 0;
							nRows=nTempTxt.GetHeight();
							nTempTxt.InsertAfter(nRows);
							t_sprintf(nTempStr,button->getTitleText());
							nTempTxt.WriteString(nRows+1,1,nTempStr);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex);
							//nAutoUseItem.WriteString(nsetionKey,"listStr",button->getTitleText());
						}
						//nAutoUseItem.Save(SCHEME_INI_AUTO_USE);
						nTempTxt.Save(SCHEME_INI_AUTO_USE);
						nTempTxt.Clear();
					}

#ifdef WIN32
					CCMessageBox("保存配置成功","提示");
#else
					CCMessageBox(UTEXT("保存配置成功",1).c_str(),UTEXT("提示",1).c_str());
#endif
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
				if  (nTbtn->getTag()==1)
				{

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


void KuiAutoPlay::touchPageBtnEvent(CCObject *pSender, TouchEventType type)
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
				switch(nTbtn->getTag())
				{
				case AUTOBTN_PAGE_BASE:
					{//基本设置
						clearTempSelindex();
						if (listView_Item)
						{
							for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
							{
								if (custom_button_item[i])
									custom_button_item[i]->setTouchEnabled(false);
								if (custom_button_tool)
									custom_button_tool[i]->setTouchEnabled(false);
								if (custom_button_use)
									custom_button_use[i]->setTouchEnabled(false);
							}
							listView_Item->setTouchEnabled(false);
							listView_Item->setBounceEnabled(false);
							listView_tool->setTouchEnabled(false);
							listView_tool->setBounceEnabled(false);
							listView_use->setTouchEnabled(false);
							listView_use->setBounceEnabled(false);
						}
						_PageLayer_1->setVisible(true);
						_PageLayer_2->setVisible(false);
						_PageLayer_3->setVisible(false);
						_PageLayer_4->setVisible(false);
						_PageLayer_5->setVisible(false);
						setBtnTouchEnble(1,true);
						setBtnTouchEnble(2,false);
						setBtnTouchEnble(3,false);
						setBtnTouchEnble(4,false);
						setBtnTouchEnble(5,false);
						
					}
					break;
				case AUTOBTN_PAGE_ART:
					{//攻击设置
						clearTempSelindex();
						if (listView_Item)
						{
							for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
							{
								if (custom_button_item[i])
									custom_button_item[i]->setTouchEnabled(false);
								if (custom_button_tool)
									custom_button_tool[i]->setTouchEnabled(false);
								if (custom_button_use)
									custom_button_use[i]->setTouchEnabled(false);
							}
							listView_Item->setTouchEnabled(false);
							listView_Item->setBounceEnabled(false);
							listView_tool->setTouchEnabled(false);
							listView_tool->setBounceEnabled(false);
							listView_use->setTouchEnabled(false);
							listView_use->setBounceEnabled(false);
						}
						_PageLayer_1->setVisible(false);
						_PageLayer_2->setVisible(true);
						_PageLayer_3->setVisible(false);
						_PageLayer_4->setVisible(false);
						_PageLayer_5->setVisible(false);
						setBtnTouchEnble(1,false);
						setBtnTouchEnble(2,true);
						setBtnTouchEnble(3,false);
						setBtnTouchEnble(4,false);
						setBtnTouchEnble(5,false);
					
					}
					break;
				case AUTOBTN_PAGE_ITEM_1:
					{//物品1设置
						clearTempSelindex();
						_PageLayer_1->setVisible(false);
						_PageLayer_2->setVisible(false);
						_PageLayer_3->setVisible(true);
						_PageLayer_4->setVisible(false);
						_PageLayer_5->setVisible(false);
						if (listView_Item)
						{
							for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
							{
								if (custom_button_item[i])
									custom_button_item[i]->setTouchEnabled(true);
								if (custom_button_tool)
									custom_button_tool[i]->setTouchEnabled(true);
								if (custom_button_use)
									custom_button_use[i]->setTouchEnabled(true);
							}
							listView_Item->setTouchEnabled(true);
							listView_Item->setBounceEnabled(true);
							listView_tool->setTouchEnabled(true);
							listView_tool->setBounceEnabled(true);
							listView_use->setTouchEnabled(true);
							listView_use->setBounceEnabled(true);
						}
						setBtnTouchEnble(1,false);
						setBtnTouchEnble(2,false);
						setBtnTouchEnble(3,true);
						setBtnTouchEnble(4,false);
						setBtnTouchEnble(5,false);
					}
					break;
				case AUTOBTN_PAGE_ITEM_2:
					{//物品2设置
						clearTempSelindex();
						if (listView_Item)
						{
							for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
							{
								if (custom_button_item[i])
									custom_button_item[i]->setTouchEnabled(false);
								if (custom_button_tool)
									custom_button_tool[i]->setTouchEnabled(false);
								if (custom_button_use)
									custom_button_use[i]->setTouchEnabled(false);
							}
							listView_Item->setTouchEnabled(false);
							listView_Item->setBounceEnabled(false);
							listView_tool->setTouchEnabled(false);
							listView_tool->setBounceEnabled(false);
							listView_use->setTouchEnabled(false);
							listView_use->setBounceEnabled(false);
						}
						_PageLayer_1->setVisible(false);
						_PageLayer_2->setVisible(false);
						_PageLayer_3->setVisible(false);
						_PageLayer_4->setVisible(true);
						_PageLayer_5->setVisible(false);
						setBtnTouchEnble(1,false);
						setBtnTouchEnble(2,false);
						setBtnTouchEnble(3,false);
					    setBtnTouchEnble(4,true);
						setBtnTouchEnble(5,false);
					}
					break;
				case AUTOBTN_PAGE_OTHER:
					{//其他设置
						clearTempSelindex();
						if (listView_Item)
						{
							for (int i = 0; i < MAX_ITEMLIST_COUNT; ++i)
							{
								if (custom_button_item[i])
									custom_button_item[i]->setTouchEnabled(false);
								if (custom_button_tool)
									custom_button_tool[i]->setTouchEnabled(false);
								if (custom_button_use)
									custom_button_use[i]->setTouchEnabled(false);
							}
							listView_Item->setTouchEnabled(false);
							listView_Item->setBounceEnabled(false);
							listView_tool->setTouchEnabled(false);
							listView_tool->setBounceEnabled(false);

							listView_tool->setTouchEnabled(false);
							listView_tool->setBounceEnabled(false);

							listView_use->setTouchEnabled(false);
							listView_use->setBounceEnabled(false);
						}
						_PageLayer_1->setVisible(false);
						_PageLayer_2->setVisible(false);
						_PageLayer_3->setVisible(false);
						_PageLayer_4->setVisible(false);
						_PageLayer_5->setVisible(true);

						setBtnTouchEnble(1,false);
						setBtnTouchEnble(2,false);
						setBtnTouchEnble(3,false);
						setBtnTouchEnble(4,false);
						setBtnTouchEnble(5,true);
						
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
				if  (nTbtn->getTag()==AUTOBTN_PAGE_BASE)
				{
					//buttonCallBackFunc(NULL);
					//CCMessageBox(nTbtn->GetTagbyKey().c_str(),"按下按钮");
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
bool KuiAutoPlay::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	/*CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * SkillNode =NULL;
	for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; ++i)
	{
		SkillNode = (CCSprite *)ParentNode_Team->getChildByTag(i+1);
		if  (SkillNode)
		{
			if (getRect(SkillNode,i+1).containsPoint(location))
			{//点中了
				SkillNode->runAction(red);  
			}
			else
			{
				SkillNode->setColor(ccWHITE);           //默认颜色
			}
		}
	}
	return true;*/
}

std::string  KuiAutoPlay::creatAutoPlayDir(std::string nTempPath)
{
	std::string m_TempDir = CCFileUtils::sharedFileUtils()->getWritablePath();
	m_TempDir += nTempPath;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	DIR *pDir = NULL;
	pDir = opendir(m_TempDir.c_str());
	if (!pDir)
	{
		mkdir(m_TempDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
#else
	if ((GetFileAttributesA(m_TempDir.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(m_TempDir.c_str(),0);
	}
#endif
	return m_TempDir;
}

void KuiAutoPlay::setBtnTouchEnble(int index,bool enble)
{
	switch(index)
	{
	case 1:
		{
			if (switchControl_baohu)
				switchControl_baohu->setTouchEnabled(enble);
			if (switchControl_team_d)
				switchControl_team_d->setTouchEnabled(enble);
			if (switchControl_team)
				switchControl_team->setTouchEnabled(enble);
			if (switchControl_team_y)
				switchControl_team_y->setTouchEnabled(enble);
			if (switchControl_team_g)
				switchControl_team_g->setTouchEnabled(enble);
			if (switchControl_yabiao)
				switchControl_yabiao->setTouchEnabled(enble);
			if (switchControl_baoliu)
				switchControl_baoliu->setTouchEnabled(enble);
			if (switchControl_useitem)
				switchControl_useitem->setTouchEnabled(enble);
			if (switchControl_miaosha)
				switchControl_miaosha->setTouchEnabled(enble);
		}
		break;
	case 3:
		{
			if (switchControl_item_m)
			    switchControl_item_m->setTouchEnabled(enble);
			if (switchControl_item_d)
				switchControl_item_d->setTouchEnabled(enble);
			if (switchControl_item_z)
				switchControl_item_z->setTouchEnabled(enble);
			if (switchControl_item_t)
				switchControl_item_t->setTouchEnabled(enble);
			if (switchControl_item_zm)
				switchControl_item_zm->setTouchEnabled(enble);

			if (_button)
				_button->setTouchEnabled(enble);
			if (_button_1)
				_button_1->setTouchEnabled(enble);
			if (_button_2)
				_button_2->setTouchEnabled(enble);
		}
        break;
	case 2:
		{
			if (switchControl_zhandou)
			    switchControl_zhandou->setTouchEnabled(enble);
			if (switchControl_shiqu)
			    switchControl_shiqu->setTouchEnabled(enble);
			if (switchControl_savem)
			    switchControl_savem->setTouchEnabled(enble);
			if (switchControl_fenjie)
				switchControl_fenjie->setTouchEnabled(enble);
			if (switchControl_yanshi)
				switchControl_yanshi->setTouchEnabled(enble);
			if (switchControl_dingdian)
				switchControl_dingdian->setTouchEnabled(enble);
			if (switchControl_luxian)
				switchControl_luxian->setTouchEnabled(enble);
			if (switchControl_luzhi)
				switchControl_luzhi->setTouchEnabled(enble);


			for (int i=0;i<MAX_SKILL_COUNT;++i) 
			{
				if (switchControl_skill[i])
					switchControl_skill[i]->setTouchEnabled(enble);
			}
		}
		break;
	case 5:
		{
			if (switchControl_other_s)
				switchControl_other_s->setTouchEnabled(enble);
			if (switchControl_other_j)
				switchControl_other_j->setTouchEnabled(enble);
		    if (switchControl_other_y)
				switchControl_other_y->setTouchEnabled(enble);
			if (switchControl_other_t)
				switchControl_other_t->setTouchEnabled(enble);
		}
		break;
	default:
		break;
	}
}
//创建文件夹
int KuiAutoPlay::CreateFolder(char * nPath)
{
	/* 格式化路径并创建目录 */
	/*for (int i = 0; i < (int)strlen(nPath); i++)
	{
		if (nPath[i] == '/') 
		{
			nPath[i] = '\\';
		}
	}
	if(nPath[0]=='\\'||nPath[0]=='/')
	{
		memcpy(oPath,nPath+1,strlen(nPath)-1);
	}
	else 
	{
		memcpy(oPath, nPath, strlen(nPath));
	}*/
	char tPath[255];
	if (nPath[0]=='/'|| nPath[0]=='\\')
	{
		nPath[0]='/';
		nPath++;
	}

	for (size_t i = 1; i < strlen(nPath); i++)
	{
		if (nPath[i] == '\\') nPath[i] = '/';
		if (nPath[i] == '/')
		{
			memcpy(tPath, nPath, i );
			tPath[i] = 0;
			char nTempDir[256];
			ZeroMemory(nTempDir,sizeof(nTempDir));
			t_sprintf(nTempDir,"%s%s",m_SDcardDirPath.c_str(),tPath);
#ifdef WIN32 //完整路径
			_mkdir(nTempDir);//tPath
#else
			//DIR *pDir = NULL;
			//pDir = opendir(nTempDir);
			//if (!pDir)
			mkdir(nTempDir,S_IRWXU|S_IRWXG|S_IRWXO);
			//else
			//closedir(pDir);
#endif
		}
	}

	return true;

}

void KuiAutoPlay::_removeDelegate(bool isThis)
{
	this->setTouchEnabled(isThis);
	//this->setBounceEnabled(isThis);
	//this->setTouchEnabled(isThis);
	//ParentNode_Team= spriteBox;
	if (isThis)
	   ParentNode_Team->setPosition(ccp(winSize.width/2,winSize.height/2));
	else
	   ParentNode_Team->setPosition(ccp(winSize.width*2,winSize.height/2));
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);//删除委托
}
//线程
/*void KuiAutoPlay::Run()
{

}
*/
//基本设置循环
void KuiAutoPlay::base_Breathe()
{
    if (switchControl_baohu && switchControl_baohu->isOn())
	{//保护开启的
		if(m_uLifeTime) 
		   m_uLifeTime--;

		if (m_uLifeTime<0)
			m_uLifeTime=0;

		int nLife_1 = 0;
		char nTemPstr[64];
		if (pLifeEditBox)
		{
		  t_sprintf(nTemPstr,pLifeEditBox->getText());
		  nLife_1 = g_Atoui(nTemPstr);
		}

		if(nLife_1 > 0 && !m_uLifeTime)
		{
			int nRet;
			KUiPlayerRuntimeInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

			if(Info.nLife <= nLife_1)
				nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,0,false); //吃红
			else 
				nRet = 0;

			if(nRet)
				m_uLifeTime = LIFE_TIME_DISTANCE_DEFAULT;  //时间 50？
		}
		//吃蓝
		int nMana = 0;
        if (pNeiliEditBox)
		{
			 t_sprintf(nTemPstr,pNeiliEditBox->getText());
			 nMana = g_Atoui(nTemPstr);
		}
		if(m_uManaTime)
			m_uManaTime--;

		if (m_uManaTime<0)
			m_uManaTime=0;

		if(nMana > 0 && !m_uManaTime)
		{
			int nRet;
			KUiPlayerRuntimeInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

			if(Info.nMana <= nMana)
				nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,1,false); //吃内
			else 
				nRet = 0;

			if(nRet)
				m_uManaTime = LIFE_TIME_DISTANCE_DEFAULT;
		}
		//回城
		int nPortal = 0;
		if (pLifeEditBox_h)
		{
			t_sprintf(nTemPstr,pLifeEditBox_h->getText());
			nPortal = g_Atoui(nTemPstr);
		}

		if(m_PortalTime) 
			m_PortalTime--;

		if(m_PortalTime<0) 
			m_PortalTime=0;

		if(nPortal > 0 && !m_PortalTime)
		{
			int nRet;
			KUiPlayerRuntimeInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);
			if(Info.nLife <= nPortal)
				nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,2,TRUE); //回城
			else 
			{
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,2,FALSE);       //设置内挂活动状态
				nRet = 0;
			}
			if(nRet)
				m_PortalTime = LIFE_TIME_DISTANCE_DEFAULT;
		}
		//自动使用物品
		if(switchControl_useitem && switchControl_useitem->isOn())
		{  
			int nRet;
			if (nUseItemLoopTime)
				nUseItemLoopTime--;
			if (nUseItemLoopTime<=0)
			{
				nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,26,true);
				nUseItemLoopTime=LIFE_TIME_USERITEM_DEFAULT;
			}
		}
		else
		{  
			int nReta;
			nReta = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,26,false);
			nUseItemLoopTime=0;
		}  
		//跟随队长
		if (switchControl_team_g && switchControl_team_g->isOn())
		{//如果开启了 自动跟随队长 就循环
			//if (m_FollowCheck.IsVisible())
			//	m_FollowCheck.Hide();
			if (switchControl_zhandou && switchControl_zhandou->isOn())
				switchControl_zhandou->setOn(false);//关闭战斗
			if (switchControl_shiqu && switchControl_shiqu->isOn())
				switchControl_shiqu->setOn(false);  //关闭拾取

			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,29,TRUE);
		}
		else
		{
			//if (m_FollowCheck.IsVisible())
			//	m_FollowCheck.Hide();

			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,29,FALSE);
		}

	}
}
//无限循环
void KuiAutoPlay::update(float delta)
{
	if (isOpen && g_pCoreShell)
	{
/*#ifdef WIN32
		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
			Sleep(10);       //挂起1毫秒
#else
		usleep(10000); //微秒 毫秒=1000微妙  sleep 为秒
		//sleep(1);
#endif*/
		if (g_pCoreShell->GetAutoplayid())
		{//挂机状态
			base_Breathe();
			if(switchControl_luzhi && switchControl_luzhi->isOn())
			{//如果是勾选状态 取消 录制脚本
				switchControl_luzhi->setOn(false);	//关闭录制
				g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,8,FALSE,FALSE);	
			}

			if (switchControl_yabiao && switchControl_yabiao->isOn())
			{//自动押镖
				if (switchControl_zhandou && switchControl_zhandou->isOn())
				   switchControl_zhandou->setOn(false);//关闭战斗
				if (switchControl_shiqu && switchControl_shiqu->isOn())
				   switchControl_shiqu->setOn(false);  //关闭拾取
				if (switchControl_team_g && switchControl_team_g->isOn())
					switchControl_team_g->setOn(false);//关闭跟随队长

				g_pCoreShell->YaBiao();                //开始押镖
				return;
			}
			//如果跟随开启的
			/*if (switchControl_team_g &&switchControl_team_g->isOn())
			{
				if (switchControl_zhandou)
					switchControl_zhandou->setOn(false);//关闭战斗
				if (switchControl_shiqu)
					switchControl_shiqu->setOn(false);  //关闭拾取
			}*/

			if(switchControl_zhandou && switchControl_zhandou->isOn())
			{//自动战斗
				if(switchControl_luxian && switchControl_luxian->isOn())
				{//脚本模式
					if  (switchControl_dingdian && switchControl_dingdian->isOn())
					     switchControl_dingdian->setOn(false);
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,9,TRUE);
				} 
				else//定点模式
			        g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,9,FALSE);
				char nTempStr[32];
				int fanwei=180,juli=75,AtrakTime=0;
				if (pGongjiEditBox)
				{//活动范围
					t_sprintf(nTempStr,pGongjiEditBox->getText());
					fanwei = g_Atoui(nTempStr);
				}
				if (pXunguaiEditBox)
				{//寻怪范围
					int nNpcDis=0;
					t_sprintf(nTempStr,pXunguaiEditBox->getText());
					nNpcDis = g_Atoui(nTempStr);

					if (nNpcDis<90)
					{
						nNpcDis = 90;
						pXunguaiEditBox->setText("90");
					}
					if (nNpcDis>800)
					{
						nNpcDis=800;
						pXunguaiEditBox->setText("800");
					}
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,31,nNpcDis);
				}
				if  (pGongjiEditBox_m)
				{//攻击秒数
					t_sprintf(nTempStr,pGongjiEditBox_m->getText());
					AtrakTime = g_Atoui(nTempStr);
				}

				if (AtrakTime<=0)
					AtrakTime=10;

				g_pCoreShell->Guaji(fanwei,juli,AtrakTime); //开始跑动
				
				if (pshiquEditBox_c)
				{//拾取的次数
                    t_sprintf(nTempStr,pshiquEditBox_c->getText());
					AtrakTime = g_Atoui(nTempStr);
					if (AtrakTime<=0)
						AtrakTime=10;
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,17,AtrakTime);
				}
				//拾取攻击延时
				AtrakTime = 10;
				if (pEditBox_yanshi)
				{
					t_sprintf(nTempStr,pEditBox_yanshi->getText());
					AtrakTime = g_Atoui(nTempStr);
				}

				if(switchControl_yanshi)
				{//开启延时
					if (!switchControl_yanshi->isOn())
					    switchControl_yanshi->setOn(true);

					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,20,AtrakTime);
				}
				else
				{
					if (switchControl_yanshi && !switchControl_yanshi->isOn())
						switchControl_yanshi->setOn(true);
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,20,AtrakTime);
				}

				//自动存钱
				int  nDesMoney = 0;
				if  (pmoneyEditBox)
				{
					t_sprintf(nTempStr,pmoneyEditBox->getText());
					nDesMoney = g_Atoui(nTempStr);
				}
				 nDesMoney=nDesMoney*10000;

				if (m_uMoneyTime)
					m_uMoneyTime--;
				if (m_uMoneyTime<=0)
					m_uMoneyTime=0;

				if(switchControl_savem && switchControl_savem->isOn() && nDesMoney>0 && m_uMoneyTime==0)
				{
					int nMoney=0;	
					nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);                     //获取包袱的钱

					if (nMoney>nDesMoney)
					{
						g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,true,nMoney-nDesMoney);  //存取钱
					}
					m_uMoneyTime=100;
				}

				item_Breathe();

				if(switchControl_skill[0] &&switchControl_skill[0]->isOn())
				{//开启技能1
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,1,TRUE);
				}
				else
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,1,FALSE);


				if(switchControl_skill[1] &&switchControl_skill[1]->isOn())
				{//开启技能2
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,2,TRUE);
				}
				else
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,2,FALSE);

				if(switchControl_skill[2] &&switchControl_skill[2]->isOn())
				{//开启技能3
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,3,TRUE);
				}
				else
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,3,FALSE);

				if(switchControl_skill[3] && switchControl_skill[3]->isOn())
				{//开启技能4
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,4,TRUE);
				}
				else
					g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,21,4,FALSE);

				g_pCoreShell->AutoUseSkill();	 //自动使用技能
			}//end zhandou
			//item_Breathe();
		}
	}
}
//物品循环
void KuiAutoPlay::item_Breathe()
{
	if (switchControl_shiqu && switchControl_shiqu->isOn())
	{//如果拾取 是开启的
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("自动捡取开启中");
		if(switchControl_item_m && switchControl_item_m->isOn())  //自动捡钱
		{
			int nRet;
			nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,3,true); //
			//if (switchControl_item_z)
			//	switchControl_item_z->setOn(false);
			if (switchControl_item_zm && switchControl_item_zm->isOn())
				switchControl_item_zm->setOn(false);
		}
		else
		{
			int nReta;
			nReta = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,3,false); //关闭捡钱
		}

		int shuxingCount = 0;
		char nTempStrVer[32];
		if  (pshuxEditBox)
		{//属性数量
			t_sprintf(nTempStrVer,pshuxEditBox->getText());
			shuxingCount = g_Atoui(nTempStrVer);
		}

		if(switchControl_item_z && switchControl_item_z->isOn())  //自动捡装备
		{
			int nRet;
			nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,4,true,shuxingCount); //
			//if (switchControl_item_m)
			//	switchControl_item_m->setOn(false);
			if (switchControl_item_zm &&switchControl_item_zm->isOn())
				switchControl_item_zm->setOn(false);
		}
		else
		{
			int nReta;
			nReta = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,4,false,shuxingCount); //关闭捡装备
		}

		if(switchControl_item_zm && switchControl_item_zm->isOn())  //自动钱装备
		{
			int nRet;
			nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,5,true,shuxingCount); //	
			if (switchControl_item_z && switchControl_item_z->isOn())
				switchControl_item_z->setOn(false);
			if (switchControl_item_m && switchControl_item_m->isOn())
				switchControl_item_m->setOn(false);
		}
		else
		{
			int nReta;
			nReta = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,5,false,shuxingCount); //关闭捡装备和钱
		}

		if(switchControl_item_t && switchControl_item_t->isOn())  //自动捡图
		{
			int nRet;
			nRet = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,6,true); //	
			//m_AutoTu.CheckButton(false);
			//m_AutoTu.CheckButton(false);
		}
		else
		{
			int nReta;
			nReta = g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,6,false); 
		}

		if(switchControl_item_d && switchControl_item_d->isOn())  //道具拾取
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,14,true,shuxingCount);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_COMMAND,14,false,shuxingCount);
		}


	}
}

void KuiAutoPlay::draw()
{
	if (isOpen && g_pCoreShell)
	{
		if (switchControl_team_d && switchControl_team_d->isOn())
			switchControl_team_y->setOn(false);

		if (switchControl_team_y && switchControl_team_y->isOn())
			switchControl_team_d->setOn(false);
	}
	//if (switchControl_team_y)
	//   switchControl_team_y->setOn(false);
	//CCMessageBox("无限循环","draw");
}
