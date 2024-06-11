#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "Kuiskill.h"
#include "KuiItem.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
#include "gamecore/KPlayer.h"
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
#include "gameui/KuiSkilldesc.h"
#include "gameui/KCheckbox.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

Kuiskill::Kuiskill()
{
	isOpen = false;
	memset(Skills,0,sizeof(KUiSkillData) * 50);
	nRemainSkillPoint = 0;
	pSkillPointLabel  = NULL;
	pSkillInfoLabel   = NULL;
	colorLayer        = NULL;
	red               = NULL;
	nSkillCount       = 0;
	nCurSelindex      = 0;
}

Kuiskill::~Kuiskill()
{
	isOpen = false;
	nRemainSkillPoint = 0;
	pSkillPointLabel  = NULL;
	colorLayer        = NULL;
	pSkillInfoLabel   = NULL;
	if (red)
	   red->release();
	//red               = NULL;
}

//创建确定弹出框
Kuiskill * Kuiskill::create(int m_Kind,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	Kuiskill * popLayer = Kuiskill::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool Kuiskill::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCSprite * spriteBox = CCSprite::create("ui/uiSkill.png");//CCSprite::createWithSpriteFrame(f_tips_box);
	ParentNode_Fight = spriteBox;
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2));
	this->addChild(spriteBox);
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	m_pUiLayer = UILayer::create();
	m_pUiLayer->setPosition(ccp(0,0));
	addChild(m_pUiLayer);
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void Kuiskill::addDialogData()
{
    pSkillPointLabel = CCLabelTTF::create("",UI_GAME_FONT_SONG, 12);//Arial
	pSkillPointLabel->setPosition(ccp(180,0));
	ParentNode_Fight->addChild(pSkillPointLabel,1);
	CCSize pageSize;
	pageSize.width = 711;
	pageSize.height = 410;
	int NewscrX =0,NewscrY =0;
	CCMenuItemImage *paddPointItem = CCMenuItemImage::create("ui/btn_common1_1.png","ui/btn_common1_2.png",this,menu_selector(Kuiskill::addskillpoint));
	paddPointItem->setPosition(ccp(pageSize.width/2-100,-240));
	paddPointItem->setAnchorPoint(ccp(0,0));
	CCLabelTTF * paddPointItemLabel = CCLabelTTF::create("Tăng điểm",UI_GAME_FONT_DEFAULT_VN_BTN, 18);
	paddPointItemLabel->setColor(ccWHITE);
	paddPointItemLabel->setPosition(ccp(paddPointItemLabel->getContentSize().width/4-paddPointItemLabel->getContentSize().width/4+10,paddPointItemLabel->getContentSize().height/4));
	paddPointItemLabel->setAnchorPoint(ccp(0,0));
	paddPointItemLabel->enableStroke(ccc3(0,0,0),3.0f);
	paddPointItem->addChild(paddPointItemLabel);

	


	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (unsigned int)Skills, 0); //获取分页技能数据
	
	
	//offsetX_b=215;
	//offsetY_b=98;
	
	// Create the page view
	UIPageView* pageView = UIPageView::create();
	pageView->setTouchEnabled(true);
	pageView->setSize(pageSize);
	//pageView->setAnchorPoint(ccp(0,0));
	pageView->setPosition(ccp(165,170));
	pageView->setTag(2000);
	pageView->setName("pageView");
	UILayout* layout = UILayout::create();
	layout->setSize(pageSize);
	UIImageView* imageView = UIImageView::create();
	imageView->setTouchEnabled(false);
	imageView->setScale9Enabled(true);
	//imageView->loadTexture("ui/bg_page.png");
	imageView->setSize(pageSize);
	imageView->setPosition(ccp(pageSize.width/2,pageSize.height/2));
	layout->addChild(imageView);
	
	UILabel* label = UILabel::create();
	label->setText("Trang 1");
	label->setFontName(UI_GAME_FONT_DEFAULT_VN);
	label->setFontSize(36);
	label->setScale(0.5);
	label->setColor(ccc3(255, 255, 255));
	label->setPosition(ccp(layout->getSize().width / 2, layout->getSize().height / 2-180));
	layout->addChild(label);
	layout->setPosition(ccp(0,0));
	pageView->addPage(layout);

	UILayout* layout1 = UILayout::create();
	layout1->setSize(pageSize);
	
	UIImageView* imageView2 = UIImageView::create();
	imageView2->setTouchEnabled(false);
	imageView2->setScale9Enabled(true);
	//imageView2->loadTexture("ui/bg_page.png");
	imageView2->setSize(pageSize);
	imageView2->setPosition(ccp(pageSize.width/2,pageSize.height/2));
	layout1->addChild(imageView2);
	
	UILabel* label2 = UILabel::create();
	label2->setText("Trang 2");
	label2->setFontName(UI_GAME_FONT_DEFAULT_VN);
	label2->setFontSize(36);
	label2->setScale(0.5);
	label2->setColor(ccc3(255, 255, 255));
	label2->setPosition(ccp(layout1->getSize().width / 2, layout1->getSize().height / 2-180));
	layout1->addChild(label2);
	layout1->setPosition(ccp(0,0));
	pageView->addPage(layout1);  

	UILayout* layout2 = UILayout::create();
	layout2->setSize(pageSize);
	
	UIImageView* imageView3 = UIImageView::create();
	imageView3->setTouchEnabled(false);
	imageView3->setScale9Enabled(true);
	//imageView3->loadTexture("ui/bg_page.png");
	imageView3->setSize(pageSize);
	imageView3->setPosition(ccp(pageSize.width/2,pageSize.height/2));
	layout2->addChild(imageView3);
	
	UILabel* label3 = UILabel::create();
	label3->setText("Trang 3");
	label3->setFontName(UI_GAME_FONT_DEFAULT_VN);
	label3->setFontSize(36);
	label3->setScale(0.5);
	label3->setColor(ccc3(255, 255, 255));
	label3->setPosition(ccp(layout2->getSize().width / 2, layout2->getSize().height / 2-180));
	layout2->addChild(label3);
	layout2->setPosition(ccp(0,0));
	pageView->addPage(layout2);  

	layout->setVisible(false);
	layout1->setVisible(false);
	layout2->setVisible(false);

	//trang truoc
	CCMenuItemImage *prePage = CCMenuItemImage::create("ui/btn_turn_left.png","ui/btn_turn_left.png",this,menu_selector(Kuiskill::toprePage));
	prePage->setPosition(ccp(-pageSize.width/2-25,-20));
	prePage->setAnchorPoint(ccp(0,0));
	prePage->setScaleX(0.9);

	//trang sau
	CCMenuItemImage *nextPage = CCMenuItemImage::create("ui/btn_turn_right.png","ui/btn_turn_right.png",this,menu_selector(Kuiskill::tonextPage));
	nextPage->setPosition(ccp(pageSize.width/2-15,-20));
	nextPage->setAnchorPoint(ccp(0,0));
	nextPage->setScaleX(0.9);


	CCMenu *newmenu=CCMenu::create(paddPointItem,prePage,nextPage,NULL);  // 此步很关键，否则只是菜单项，但不能点击
	newmenu->setAnchorPoint(ccp(0,0));
	newmenu->setPosition(ccp(m_size.width/2,m_size.height/2)); //m_size.width-pSelectedTexture->getContentSize().width*3-30,42
	ParentNode_Fight->addChild(newmenu,1);

	int offsetX=120,offsetY=180,i;
	int offsetX_b=207;
	int offsetY_b=98;
	for (i = 0; i < FIGHT_SKILL_COUNT; ++i)  //FIGHT_SKILL_COUNT_PER_PAGE
	{//tao node icon skill
		
		if (i<15){//page 1
			UIButton* button = UIButton::create();
			button->setTouchEnabled(true);
			button->loadTextures("ui/itembg1.png", "ui/itembg1.png", "");
			button->setSize(CCSizeMake(216,61));
			button->setVisible(false);
			CCRect CCRectZero = CCRectMake(0,0,5,5);
			CCRectZero.size = button->getContentSize();
			int NewscrX =0,NewscrY =0;
			CCTexture2D * bgCur= GetiniData("\\spr\\Ui\\技能图标\\枪法.spr",0,0,0,0,&NewscrX,&NewscrY,0,true);
			if (bgCur){
				UIImageView* nSkillSpr = UIImageView::create();
				nSkillSpr->loadTextureByData(bgCur);
				nSkillSpr->setAnchorPoint(ccp(0,0));
				nSkillSpr->setScale(1.2);
				
				nSkillSpr->setTag(i+1);
				nSkillSpr->setPosition(ccp(-button->getSize().width/2+8,-button->getSize().height/2+8));
				if(i==0){
					offsetX=120;
					offsetY=180;
				}
				else if  (i>=0 && i<3)//item 1-2
				{
					offsetX = offsetX+CCRectZero.size.width+20;//đặt độ lệch tiếp theo
				}
				else if (i==3)
				{//2行
					offsetX = 120;
					offsetY = 190+CCRectZero.size.height;
				}
				else if  (i>3 && i<6)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==6)
				{//3行
					offsetX = 120;
					offsetY = 200+CCRectZero.size.height*2;
				}
				else if  (i>6 && i<9)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==9)
				{//4行
					offsetX = 120;
					offsetY = 210+CCRectZero.size.height*3;
				}
				else if  (i>9 && i<12)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}

				else if (i==12)
				{//5行
					offsetX = 120;
					offsetY = 220+CCRectZero.size.height*4;
				}
				else if  (i>12 &&i<15)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				
				char nKey[32],nKeyPoint[32];
				t_sprintf(nKey,"skillname_%d",i+1);
				std::string m_Key = nKey;

				//Ten skill
				UILabel* nSkillName = UILabel::create();
				nSkillName->setText(CCString::createWithFormat("Tên kỹ năng %d", i)->getCString());
				nSkillName->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillName->setFontSize(36);
				nSkillName->setScale(0.5);
				nSkillName->setColor(ccc3(0, 255, 0));
				nSkillName->setAnchorPoint(ccp(0,0));
				nSkillName->setTagbyKey(m_Key);
				nSkillName->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+33));
				//Ten skill

				//cap hien tai
				t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
				std::string m_KeyPoint = nKeyPoint;
				UILabel* nSkillPoint = UILabel::create();
				nSkillPoint->setText(CCString::createWithFormat("Cấp hiện tại %d", i)->getCString());
				nSkillPoint->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPoint->setFontSize(32);
				nSkillPoint->setScale(0.5);
				nSkillPoint->setColor(ccc3(255, 255, 255));
				nSkillPoint->setAnchorPoint(ccp(0,0));
				nSkillPoint->setTagbyKey(m_KeyPoint);
				nSkillPoint->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+8));
				//cap hien tai

				//cap hien tai
				ZeroMemory(nKeyPoint,sizeof(nKeyPoint));
				t_sprintf(nKeyPoint,"skillpointadd_%d",i+1);
				std::string m_KeyPointadd = nKeyPoint;
				UILabel* nSkillPointadd = UILabel::create();
				nSkillPointadd->setText(CCString::createWithFormat("Tăng %d", i)->getCString());
				nSkillPointadd->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPointadd->setFontSize(32);
				nSkillPointadd->setScale(0.5);
				nSkillPointadd->setColor(ccc3(255, 255, 255));
				nSkillPointadd->setAnchorPoint(ccp(0,0));
				nSkillPointadd->setTagbyKey(m_KeyPointadd);
				nSkillPointadd->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+120, -button->getSize().height / 2+8));
				//cap hien tai

				button->addChild(nSkillName);
				button->addChild(nSkillPoint);
				button->addChild(nSkillPointadd);
				button->addChild(nSkillSpr);
			}
			button->setPosition(ccp(offsetX,m_size.height-offsetY));
			button->setTag(i+1);
			//button->addTouchEventListener(this,toucheventselector(Kuiskill::oktouchEvent));
			//button->setTagbyKey(m_Key);
			
			layout->addChild(button);
		}else if (i>14 && i<30){ //Page 2
			UIButton* button = UIButton::create();
			button->setTouchEnabled(true);
			button->loadTextures("ui/itembg1.png", "ui/itembg1.png", "");
			button->setSize(CCSizeMake(216,61));
			button->setVisible(false);
			CCRect CCRectZero = CCRectMake(0,0,5,5);
			CCRectZero.size = button->getContentSize();
			int NewscrX =0,NewscrY =0;
			CCTexture2D * bgCur= GetiniData("\\spr\\Ui\\技能图标\\枪法.spr",0,0,0,0,&NewscrX,&NewscrY,0,true);
			if (bgCur){
				UIImageView* nSkillSpr = UIImageView::create();
				nSkillSpr->loadTextureByData(bgCur);
				nSkillSpr->setAnchorPoint(ccp(0,0));
				nSkillSpr->setScale(1.2);
				
				nSkillSpr->setTag(i+1);
				nSkillSpr->setPosition(ccp(-button->getSize().width/2+8,-button->getSize().height/2+8));
				if(i==15){
					offsetX=120;
					offsetY=180;
				}
				else if  (i>=15 && i<18)//item 1-2
				{
					offsetX = offsetX+CCRectZero.size.width+20;//đặt độ lệch tiếp theo
				}
				else if (i==18)
				{//2行
					offsetX = 120;
					offsetY = 190+CCRectZero.size.height;
				}
				else if  (i>18 && i<21)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==21)
				{//3行
					offsetX = 120;
					offsetY = 200+CCRectZero.size.height*2;
				}
				else if  (i>21 && i<24)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==24)
				{//4行
					offsetX = 120;
					offsetY = 210+CCRectZero.size.height*3;
				}
				else if  (i>24 && i<27)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}

				else if (i==27)
				{//5行
					offsetX = 120;
					offsetY = 220+CCRectZero.size.height*4;
				}
				else if  (i>27 &&i<30)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				
				char nKey[32],nKeyPoint[32];
				t_sprintf(nKey,"skillname_%d",i+1);
				std::string m_Key = nKey;

				//Ten skill
				UILabel* nSkillName = UILabel::create();
				nSkillName->setText(CCString::createWithFormat("Tên kỹ năng %d", i)->getCString());
				nSkillName->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillName->setFontSize(36);
				nSkillName->setScale(0.5);
				nSkillName->setColor(ccc3(0, 255, 0));
				nSkillName->setAnchorPoint(ccp(0,0));
				nSkillName->setTagbyKey(m_Key);
				nSkillName->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+33));
				//Ten skill

				//cap hien tai
				t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
				std::string m_KeyPoint = nKeyPoint;
				UILabel* nSkillPoint = UILabel::create();
				nSkillPoint->setText(CCString::createWithFormat("Cấp hiện tại %d", i)->getCString());
				nSkillPoint->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPoint->setFontSize(32);
				nSkillPoint->setScale(0.5);
				nSkillPoint->setColor(ccc3(255, 255, 255));
				nSkillPoint->setAnchorPoint(ccp(0,0));
				nSkillPoint->setTagbyKey(m_KeyPoint);
				nSkillPoint->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+8));
				//cap hien tai

				//cap hien tai
				ZeroMemory(nKeyPoint,sizeof(nKeyPoint));
				t_sprintf(nKeyPoint,"skillpointadd_%d",i+1);
				std::string m_KeyPointadd = nKeyPoint;
				UILabel* nSkillPointadd = UILabel::create();
				nSkillPointadd->setText(CCString::createWithFormat("Tăng %d", i)->getCString());
				nSkillPointadd->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPointadd->setFontSize(32);
				nSkillPointadd->setScale(0.5);
				nSkillPointadd->setColor(ccc3(255, 255, 255));
				nSkillPointadd->setAnchorPoint(ccp(0,0));
				nSkillPointadd->setTagbyKey(m_KeyPointadd);
				nSkillPointadd->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+120, -button->getSize().height / 2+8));
				//cap hien tai

				button->addChild(nSkillName);
				button->addChild(nSkillPoint);
				button->addChild(nSkillPointadd);
				button->addChild(nSkillSpr);
			}
			button->setPosition(ccp(offsetX,m_size.height-offsetY));
			button->setTag(i+1);
			//button->addTouchEventListener(this,toucheventselector(Kuiskill::oktouchEvent));
			//button->setTagbyKey(m_Key);
			
			layout1->addChild(button);
		}else if (i>29 && i<45){ //Page 2
			UIButton* button = UIButton::create();
			button->setTouchEnabled(true);
			button->loadTextures("ui/itembg1.png", "ui/itembg1.png", "");
			button->setSize(CCSizeMake(216,61));
			button->setVisible(false);
			CCRect CCRectZero = CCRectMake(0,0,5,5);
			CCRectZero.size = button->getContentSize();
			int NewscrX =0,NewscrY =0;
			CCTexture2D * bgCur= GetiniData("\\spr\\Ui\\技能图标\\枪法.spr",0,0,0,0,&NewscrX,&NewscrY,0,true);
			if (bgCur){
				UIImageView* nSkillSpr = UIImageView::create();
				nSkillSpr->loadTextureByData(bgCur);
				nSkillSpr->setAnchorPoint(ccp(0,0));
				nSkillSpr->setScale(1.2);
				
				nSkillSpr->setTag(i+1);
				nSkillSpr->setPosition(ccp(-button->getSize().width/2+8,-button->getSize().height/2+8));
				if(i==30){
					offsetX=120;
					offsetY=180;
				}
				else if  (i>=30 && i<33)//item 1-2
				{
					offsetX = offsetX+CCRectZero.size.width+20;//đặt độ lệch tiếp theo
				}
				else if (i==33)
				{//2行
					offsetX = 120;
					offsetY = 190+CCRectZero.size.height;
				}
				else if  (i>33 && i<36)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==36)
				{//3行
					offsetX = 120;
					offsetY = 200+CCRectZero.size.height*2;
				}
				else if  (i>36 && i<39)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				else if (i==39)
				{//4行
					offsetX = 120;
					offsetY = 210+CCRectZero.size.height*3;
				}
				else if  (i>39 && i<42)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}

				else if (i==42)
				{//5行
					offsetX = 120;
					offsetY = 220+CCRectZero.size.height*4;
				}
				else if  (i>42 &&i<45)
				{
					offsetX = offsetX+CCRectZero.size.width +20;
				}
				
				char nKey[32],nKeyPoint[32];
				t_sprintf(nKey,"skillname_%d",i+1);
				std::string m_Key = nKey;

				//Ten skill
				UILabel* nSkillName = UILabel::create();
				nSkillName->setText(CCString::createWithFormat("Tên kỹ năng %d", i)->getCString());
				nSkillName->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillName->setFontSize(36);
				nSkillName->setScale(0.5);
				nSkillName->setColor(ccc3(0, 255, 0));
				nSkillName->setAnchorPoint(ccp(0,0));
				nSkillName->setTagbyKey(m_Key);
				nSkillName->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+33));
				//Ten skill

				//cap hien tai
				t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
				std::string m_KeyPoint = nKeyPoint;
				UILabel* nSkillPoint = UILabel::create();
				nSkillPoint->setText(CCString::createWithFormat("Cấp hiện tại %d", i)->getCString());
				nSkillPoint->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPoint->setFontSize(32);
				nSkillPoint->setScale(0.5);
				nSkillPoint->setColor(ccc3(255, 255, 255));
				nSkillPoint->setAnchorPoint(ccp(0,0));
				nSkillPoint->setTagbyKey(m_KeyPoint);
				nSkillPoint->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+15, -button->getSize().height / 2+8));
				//cap hien tai

				//cap hien tai
				ZeroMemory(nKeyPoint,sizeof(nKeyPoint));
				t_sprintf(nKeyPoint,"skillpointadd_%d",i+1);
				std::string m_KeyPointadd = nKeyPoint;
				UILabel* nSkillPointadd = UILabel::create();
				nSkillPointadd->setText(CCString::createWithFormat("Tăng %d", i)->getCString());
				nSkillPointadd->setFontName(UI_GAME_FONT_DEFAULT_VN);
				nSkillPointadd->setFontSize(32);
				nSkillPointadd->setScale(0.5);
				nSkillPointadd->setColor(ccc3(255, 255, 255));
				nSkillPointadd->setAnchorPoint(ccp(0,0));
				nSkillPointadd->setTagbyKey(m_KeyPointadd);
				nSkillPointadd->setPosition(ccp(nSkillSpr->getPosition().x+nSkillSpr->getSize().width+120, -button->getSize().height / 2+8));
				//cap hien tai

				button->addChild(nSkillName);
				button->addChild(nSkillPoint);
				button->addChild(nSkillPointadd);
				button->addChild(nSkillSpr);
			}
			button->setPosition(ccp(offsetX,m_size.height-offsetY));
			button->setTag(i+1);
			//button->addTouchEventListener(this,toucheventselector(Kuiskill::oktouchEvent));
			//button->setTagbyKey(m_Key);
			
			layout2->addChild(button);
		}
	}
	//pageView->addEventListenerPageView(this, pagevieweventselector(Kuiskill::pageViewEvent));
	m_pUiLayer->addWidget(pageView);
	for (i = 0; i < FIGHT_SKILL_COUNT; ++i)
	{//战斗技能 
		//m_FightSkills[i].HoldObject(Skills[i].uGenre, Skills[i].uId, Skills[i].nLevel, 0);//显示鼠标进过的物品描述，设置容纳对象的类型
		ISkill *pSkill = g_SkillManager.GetSkill(Skills[i].uId,1);
		if (pSkill)
		{//绘画技能图标
			if (i<15){
				layout->setVisible(true);
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(pSkill->GetSkillName());
				nSkillCount++;
				int NewscrX =0,NewscrY =0;
				char nIconFile[128];
				char nSkillNameData[128];
				t_printf(nSkillNameData,pSkill->GetSkillName());
				t_sprintf(nIconFile,pSkill->getIconPath().c_str());
				CCTexture2D * bgCur= GetiniData(nIconFile,0,0,0,0,&NewscrX,&NewscrY,0,true);
				UIButton* btn = (UIButton *)layout->getChildByTag(i+1);
				if (btn){
					btn->setVisible(true);
					UIImageView* nSkillSpr = (UIImageView *)btn->getChildByTag(i+1);
					if (nSkillSpr){
						char nKey[32],nKeyPoint[32];
						t_sprintf(nKey,"skillname_%d",i+1);
						std::string m_Key = nKey;
						UILabel  *nSkillNameLabel =(UILabel *)btn->getChildByTagKey(m_Key);
						if (nSkillNameLabel){
							//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nSkillNameData);
							nSkillNameLabel->setText(pSkill->GetSkillName());
						}
						t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
						std::string m_KeyPoint = nKeyPoint;
						UILabel  *nSkillLvLabel =(UILabel *)btn->getChildByTagKey(m_KeyPoint);
						if (nSkillLvLabel){
							char nskillinfo[32];
							ZeroMemory(nskillinfo,sizeof(nskillinfo));
							t_sprintf(nskillinfo,"Cấp %d",Skills[i].nLevel);
							nSkillLvLabel->setText(nskillinfo);
						}
						nSkillSpr->loadTextureByData(bgCur);
					}
				}
			}else if (i>14 && i<30){
				layout1->setVisible(true);
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(pSkill->GetSkillName());
				nSkillCount++;
				int NewscrX =0,NewscrY =0;
				char nIconFile[128];
				char nSkillNameData[128];
				t_printf(nSkillNameData,pSkill->GetSkillName());
				t_sprintf(nIconFile,pSkill->getIconPath().c_str());
				CCTexture2D * bgCur= GetiniData(nIconFile,0,0,0,0,&NewscrX,&NewscrY,0,true);
				UIButton* btn = (UIButton *)layout1->getChildByTag(i+1);
				if (btn){
					btn->setVisible(true);
					UIImageView* nSkillSpr = (UIImageView *)btn->getChildByTag(i+1);
					if (nSkillSpr){
						char nKey[32],nKeyPoint[32];
						t_sprintf(nKey,"skillname_%d",i+1);
						std::string m_Key = nKey;
						UILabel  *nSkillNameLabel =(UILabel *)btn->getChildByTagKey(m_Key);
						if (nSkillNameLabel){
							//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nSkillNameData);
							nSkillNameLabel->setText(pSkill->GetSkillName());
						}
						t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
						std::string m_KeyPoint = nKeyPoint;
						UILabel  *nSkillLvLabel =(UILabel *)btn->getChildByTagKey(m_KeyPoint);
						if (nSkillLvLabel){
							char nskillinfo[32];
							ZeroMemory(nskillinfo,sizeof(nskillinfo));
							t_sprintf(nskillinfo,"Cấp %d",Skills[i].nLevel);
							nSkillLvLabel->setText(nskillinfo);
						}
						nSkillSpr->loadTextureByData(bgCur);
					}
				}
			}else if (i>29 && i<45){
				layout2->setVisible(true);
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(pSkill->GetSkillName());
				nSkillCount++;
				int NewscrX =0,NewscrY =0;
				char nIconFile[128];
				char nSkillNameData[128];
				t_printf(nSkillNameData,pSkill->GetSkillName());
				t_sprintf(nIconFile,pSkill->getIconPath().c_str());
				CCTexture2D * bgCur= GetiniData(nIconFile,0,0,0,0,&NewscrX,&NewscrY,0,true);
				UIButton* btn = (UIButton *)layout2->getChildByTag(i+1);
				if (btn){
					btn->setVisible(true);
					UIImageView* nSkillSpr = (UIImageView *)btn->getChildByTag(i+1);
					if (nSkillSpr){
						char nKey[32],nKeyPoint[32];
						t_sprintf(nKey,"skillname_%d",i+1);
						std::string m_Key = nKey;
						UILabel  *nSkillNameLabel =(UILabel *)btn->getChildByTagKey(m_Key);
						if (nSkillNameLabel){
							//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nSkillNameData);
							nSkillNameLabel->setText(pSkill->GetSkillName());
						}
						t_sprintf(nKeyPoint,"skillpoint_%d",i+1);
						std::string m_KeyPoint = nKeyPoint;
						UILabel  *nSkillLvLabel =(UILabel *)btn->getChildByTagKey(m_KeyPoint);
						if (nSkillLvLabel){
							char nskillinfo[32];
							ZeroMemory(nskillinfo,sizeof(nskillinfo));
							t_sprintf(nskillinfo,"Cấp %d",Skills[i].nLevel);
							nSkillLvLabel->setText(nskillinfo);
						}
						nSkillSpr->loadTextureByData(bgCur);
					}
				}
			}
			
			// CCSprite  *nSkillSpr =(CCSprite *)ParentNode_Fight->getChildByTag(i+1);
			// if (nSkillSpr) //改变纹理
			// {
			// 	char nKey[32];
			// 	t_sprintf(nKey,"point_%d",i+1);
			// 	std::string m_Key = nKey;
			// 	CCLabelTTF  *npontLabel =(CCLabelTTF *)ParentNode_Fight->getChildByTagKey(m_Key);
			// 	//nSkillPoint->setTagbyKey(m_Key);
			// 	if (npontLabel)
			// 	{	
			// 		npontLabel->setVisible(true);
			// 		char nskillinfo[32];
			// 		ZeroMemory(nskillinfo,sizeof(nskillinfo));
			// 		if (pSkill)
			// 		{
			// 			int nAddPoint=g_pCoreShell->GetGameData(GDI_PLAYER_SKILL_ADDPOINT,Skills[i].uId,0);	//额外的技能点
			// 			t_sprintf(nskillinfo,"%d+%d",Skills[i].nLevel,nAddPoint);
			// 		}

			// 		npontLabel->setString(nskillinfo);
			// 	}

			// 	nSkillSpr->setVisible(true);
			// 	nSkillSpr->setTexture(bgCur);
			// }
		}
	}
	if  (nSkillCount<=0)
	{//还没有学会技能 就直接设置空手技能为左键
		//Player[CLIENT_PLAYER_INDEX].SetDefaultImmedSkill();
		//CCMessageBox("技能为空","技能为空");
		/*
		pSkillList->uGenre = CGOG_SKILL_SHORTCUT;
		pSkillList->uId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurActiveWeaponSkill();//武器空手技能ID!
		pSkillList->nData = 0;
		*/
	}

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_Fight->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//CCActionInterval* tintred_back = tintred->reverse();
	//red = CCRepeatForever::create(CCSequence::create(tintred,tintred_back,NULL));
	red->retain();
	/*
	CCActionInterval*  action1 = CCTintTo::create(2, 255, 0, 255);
	CCActionInterval*  action2 = CCTintBy::create(2, -127, -255, -127);
	CCActionInterval*  action2Back = action2->reverse();
	m_tamara->runAction( action1);
	m_kathia->runAction( CCSequence::create( action2, action2Back, NULL));

	CCActionInterval* action = CCFadeIn::create(2);
	CCActionInterval* action_back = action->reverse();
	CCAction* fade = CCRepeatForever::create( CCSequence::create( action, action_back, NULL) );

	CCActionInterval* tintred = CCTintBy::create(2, 0, -255, -255);
	CCActionInterval* tintred_back = tintred->reverse();
	CCAction* red = CCRepeatForever::create( CCSequence::create(tintred, tintred_back, NULL) );

	CCActionInterval* tintgreen = CCTintBy::create(2, -255, 0, -255);
	CCActionInterval* tintgreen_back = tintgreen->reverse();
	CCAction* green = CCRepeatForever::create( CCSequence::create( tintgreen, tintgreen_back, NULL) );

	CCActionInterval* tintblue = CCTintBy::create(2, -255, -255, 0);
	CCActionInterval* tintblue_back = tintblue->reverse();
	CCAction* blue = CCRepeatForever::create( CCSequence::create( tintblue, tintblue_back, NULL) );

	sprite5->runAction(red);
	sprite6->runAction(green);
	sprite7->runAction(blue);
	sprite8->runAction(fade);
	*/
}
//返回数组的上标
int Kuiskill::getfreeSkillNode(int skillidx)
{//nSkillCount已经是最大的数
	for (int i = 0; i < nSkillCount; ++i)
	{//战斗技能
		if  (Skills[i].uId>0 && Skills[i].uId==skillidx) //已经有这个技能了相同的技能
			return i;
	}
	nSkillCount++;
	return nSkillCount;
}

void Kuiskill::oktouchEvent(CCObject *pSender, TouchEventType type, CCTouch *pTouch)
{
	if  (!g_pCoreShell || !g_GameWorld) return;


	CCPoint location = pTouch->getLocation();
	switch (type)
	{
	case TOUCH_EVENT_BEGAN://Touch Down
	{
		CCLayer * sDesc = (CCLayer*)this->getChildByTag(1001);
		if (sDesc){
			this->removeChildByTag(1001);
		}
		if (pSender)
		{
			UIButton* nTbtn = (UIButton*)pSender;
			if (nTbtn && nTbtn->isVisible()){
				//nTbtn->setScale(1.1);
				
				bool isThis = false;
				if  (g_GameWorld)
				{
					int i = nTbtn->getTag()-1;
					ISkill *pSkill = g_SkillManager.GetSkill(Skills[i].uId,1);
					char nIconName[128] ,nskillinfo[128];
					ZeroMemory(nIconName,sizeof(nIconName));
					ZeroMemory(nskillinfo,sizeof(nskillinfo));
					if (pSkill)
						t_sprintf(nskillinfo,"%s cấp %d ",pSkill->GetSkillName(),Skills[i].nLevel);

					if (!pSkillInfoLabel)
					{
						pSkillInfoLabel = CCLabelTTF::create("",UI_GAME_FONT_SONG, 12);//Arial
						pSkillInfoLabel->setPosition(ccp(245,43));
						pSkillInfoLabel->setAnchorPoint(ccp(0,0));
						pSkillInfoLabel->setColor(ccYELLOW);
						ParentNode_Fight->addChild(pSkillInfoLabel,1);
					}
					pSkillInfoLabel->setString(nskillinfo);
					KSkill * pOrdinSkill = NULL;
					pOrdinSkill = (KSkill *)pSkill;
					if (pSkill && pOrdinSkill && (pOrdinSkill->GetSkillLRInfo() == BothSkill || pOrdinSkill->GetSkillLRInfo() == leftOnlySkill))
					{//攻击技能（除断魂和轻功）//Skills[i].uId
						t_sprintf(nIconName,pSkill->getIconPath().c_str());
						g_GameWorld->setattackSprInfo(pSkill->GetSkillId(),Skills[i].uGenre,nIconName);
					}
					KUiItemdObject skillData;
					ZeroMemory(&skillData,sizeof(KUiItemdObject));
					skillData.nRect  = getRect(nTbtn,0);
					skillData.uId    = Skills[i].uId;
					skillData.uGenre = Skills[i].uGenre;
					skillData.nindex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Skills[i].uId);
					//int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Skills[i].uId);
					skillData.DataH  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddPoint(skillData.nindex);
					skillData.DataW  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetEnChance(skillData.nindex);

					CCLayer * sDescLayer = KuiSkilldesc::create(Skills[i].uId,Skills[i].nLevel,skillData,this,callfuncN_selector(Kuiskill::closeSkilldescPadCallback));
					this->addChild(sDescLayer,2,1001);
					nCurSelindex = Skills[i].uId;
					isThis = true;
					
				}	
			}else{
				this->removeChildByTag(1001);
			}
		}
	}
		break;
	case TOUCH_EVENT_MOVED://Touch Move
		break;

	case TOUCH_EVENT_ENDED://Touch Up
		{
			
		}
		break;
	case TOUCH_EVENT_CANCELED:
		this->removeChildByTag(1001);
		break;

	default:
		break;
	}
}
void Kuiskill::pageViewEvent(CCObject *pSender, PageViewEventType type,CCTouch *pTouch)
{
	// UIPageView* pageView = dynamic_cast<UIPageView*>(pSender);
	// if (pageView){
	// 	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	// 	CCSprite * SkillNode =NULL;
	// 	bool isThis = false;
	// 	for (int i = 0; i < FIGHT_SKILL_COUNT; ++i)
	// 	{
	// 		if (i<15){//page 1
			
	// 			UILayout *layout = (UILayout *)pageView->getPage(0);
	// 			if (layout){
	// 				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("111");
	// 				UIButton *btn = (UIButton *)layout->getChildByTag(i+1);
	// 				if (btn && btn->isVisible()){
	// 					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("222");
	// 					if (getRect(btn,i+1).containsPoint(location)){
	// 						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("333");
	// 						btn->runAction(red);
	// 						btn->setScale(2);
	// 					}
	// 				}
	// 			}
	// 		}else if (i>14 && i<30){//page 2

	// 		}else if (i>29 && i<45){//page 3

	// 		}
	// 	}
	// }
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("OKKKKKK");
    // switch (type)
    // {
    //     case PAGEVIEW_EVENT_TURNING:
    //     {
            
    //        // m_pDisplayValueLabel->setText(CCString::createWithFormat("page = %d", pageView->getCurPageIndex() + 1)->getCString());
    //     }
    //         break;
            
    //     default:
    //         break;
    // }
}
void Kuiskill::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	if (pSkill)
	{
			//if (pSkill->uGenre == CGOG_SKILL_LIVE)
				//m_pSelf->m_LiveSkillPad.UpdateSkill(pSkill, nIndex);
			//else 
		if (pSkill->uGenre == CGOG_SKILL_FIGHT)
		{
			//CCMessageBox("技能更新","技能更新");
			/*char skillinfo[64];
			sprintf(skillinfo,"%d",nIndex);
			CCMessageBox(skillinfo,"技能更新");
			*/
			//int nPage = nIndex/FIGHT_SKILL_COUNT_PER_PAGE; //第几页 
			//    nIndex = nIndex%FIGHT_SKILL_COUNT_PER_PAGE;//索引

			
			if (pSkill==NULL || nIndex<0 || nIndex >=FIGHT_SKILL_COUNT)
				return;
			//CCMessageBox("技能更新","技能更新");
			//Skills[i].uId
			 CCSprite  *nSkillSpr =(CCSprite *)ParentNode_Fight->getChildByTag(nIndex+1);
			 if (nSkillSpr)
			 {//如果存在就增加
				 nSkillSpr->setVisible(true);
				 Skills[nIndex].uId     = pSkill->uId;
				 Skills[nIndex].nLevel  = pSkill->nLevel;
				 Skills[nIndex].uGenre  = pSkill->uGenre;

				 char nKey[32];
				 t_sprintf(nKey,"point_%d",nIndex+1);
				 std::string m_Key = nKey;
				 CCLabelTTF  *npontLabel =(CCLabelTTF *)ParentNode_Fight->getChildByTagKey(m_Key);
				 if (npontLabel)
				 {	
					 npontLabel->setVisible(true);
					 char nskillinfo[32];
					 ZeroMemory(nskillinfo,sizeof(nskillinfo));
					 if (pSkill)
					 {
						 int nAddPoint=g_pCoreShell->GetGameData(GDI_PLAYER_SKILL_ADDPOINT,Skills[nIndex].uId,0);	//额外的技能点
						 t_sprintf(nskillinfo,"%d+%d",Skills[nIndex].nLevel,nAddPoint);
					 }
					 npontLabel->setString(nskillinfo);
				 }

				 int NewscrX =0,NewscrY =0;
				 char nIconFile[128];
				 ISkill *mSkill = g_SkillManager.GetSkill(Skills[nIndex].uId,1);
				 if  (!mSkill)
					  return;
				 t_sprintf(nIconFile,mSkill->getIconPath().c_str());
				 CCTexture2D * bgCur= GetiniData(nIconFile,0,0,0,0,&NewscrX,&NewscrY,0,true);
				 if (bgCur)
				 {
					 CCRect CCRectZero = CCRectMake(0,0,5,5);
				     CCRectZero.size = bgCur->getContentSize();
					 nSkillSpr->setTextureRect(CCRectZero);
					 nSkillSpr->setTexture(bgCur);
				 }
			 }
			//Skills[nIndex].
		}
				//m_pSelf->m_FightSkillPad.UpdateSkill(pSkill, nIndex); //更新某个位置的技能
	}
}
//设置确定取消按钮,参数：调用层对象，调用层回调函数
void Kuiskill::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	CCSprite * sprite_tips_btn_confirm_normal =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	//CCSpriteFrame * f_tips_btn_confirm_select = sfCache->spriteFrameByName("btn_confirm_select.png");
	CCSprite * sprite_tips_btn_confirm_select =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_select);
	CCMenuItemSprite * itemConfirm = CCMenuItemSprite::create(sprite_tips_btn_confirm_normal, sprite_tips_btn_confirm_select, this, menu_selector(Kuiskill::buttonCallBackFunc));
	itemConfirm->setPosition(ccp(0,0));
	CCPoint bgSpritePoint = ParentNode_Fight->getPosition();
	CCMenu * menu = CCMenu::create(itemConfirm,NULL);
	//menu->setPosition(ccp(bgSpritePoint.x-m_size.width/2,bgSpritePoint.y-m_size.height/2));//菜单位置设置为弹出框左下
	menu->setPosition(ccp(m_size.width/2,20));//菜单位置设置为弹出框左下
	ParentNode_Fight->addChild(menu);         //加到父层里
}

//执行上层对象的回调函数，关闭弹出框
void Kuiskill::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = dynamic_cast<CCNode *>(pSender);
	node->setTag(3000);//设置tag，在调用层可以获取到

	if (m_callfun &&  m_callbackListener)
	{
		//执行调用层回调函数，传递参数CCNode
		(m_callbackListener->*m_callfun)(node);
	}

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void Kuiskill::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


//重写触摸注册函数，重新给定触摸级别
void Kuiskill::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect Kuiskill::getRect(CCNode* pNode,int i)
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
bool Kuiskill::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	UIPageView* pageView = (UIPageView *)m_pUiLayer->getWidgetByName("pageView");
	if(pageView)
	{
		CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
		CCSprite * SkillNode =NULL;
		bool isThis = false;
		for (int i = 0; i < FIGHT_SKILL_COUNT; ++i)
		{
			if (i<15){//page 1
			
				UILayout *layout = (UILayout *)pageView->getPage(1);
				if (layout){
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("1111");
					UIButton *btn = (UIButton *)layout->getChildByTag(i+1);
					if (btn && btn->isVisible()){
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("2222");
						if (getRect(layout,i+1).containsPoint(location)){
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("3333");
							btn->runAction(red);
							btn->setScale(2);
						}
					}
				}
			}else if (i>14 && i<30){//page 2

			}else if (i>29 && i<45){//page 3

			}
		}
	}


	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	// CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	// CCSprite * SkillNode =NULL;
	// bool isThis = false;
	// for (int i = 0; i < FIGHT_SKILL_COUNT; ++i)
	// {
	// 	SkillNode = (CCSprite *)ParentNode_Fight->getChildByTag(i+1);
	// 	if  (SkillNode && SkillNode->isVisible())
	// 	{
	// 		if (getRect(SkillNode,i+1).containsPoint(location))
	// 		{//点中了
	// 			SkillNode->runAction(red);  
	// 			if  (g_GameWorld)
	// 			{
	// 				ISkill *pSkill = g_SkillManager.GetSkill(Skills[i].uId,1);
	// 				char nIconName[128] ,nskillinfo[128];
	// 				ZeroMemory(nIconName,sizeof(nIconName));
	// 				ZeroMemory(nskillinfo,sizeof(nskillinfo));
	// 				if (pSkill)
	// 					t_sprintf(nskillinfo,"%s %d级",pSkill->GetSkillName(),Skills[i].nLevel);
					
	// 				if (!pSkillInfoLabel)
	// 				{
	// 					pSkillInfoLabel = CCLabelTTF::create("",UI_GAME_FONT_SONG, 12);//Arial
	// 					pSkillInfoLabel->setPosition(ccp(245,43));
	// 					pSkillInfoLabel->setAnchorPoint(ccp(0,0));
	// 					pSkillInfoLabel->setColor(ccYELLOW);
	// 					ParentNode_Fight->addChild(pSkillInfoLabel,1);
	// 				}
	// 				pSkillInfoLabel->setString(UTEXT(nskillinfo,1).c_str());
	// 				KSkill * pOrdinSkill = NULL;
	// 				         pOrdinSkill = (KSkill *)pSkill;
	// 				//if (pSkill && Skills[i].nLevel>0 && Skills[i].uId !=15 && Skills[i].uId !=210 && Skills[i].uId !=40 && (pSkill->GetSkillStyle() == SKILL_SS_Missles || pSkill->GetSkillStyle() == SKILL_SS_Melee || Skills[i].uId ==53))
	// 				if (pSkill && pOrdinSkill && (pOrdinSkill->GetSkillLRInfo() == BothSkill || pOrdinSkill->GetSkillLRInfo() == leftOnlySkill))
	// 				{//攻击技能（除断魂和轻功）//Skills[i].uId
	// 					t_sprintf(nIconName,pSkill->getIconPath().c_str());
	// 					g_GameWorld->setattackSprInfo(pSkill->GetSkillId(),Skills[i].uGenre,nIconName);
	// 				}
	// 				/*else if (pSkill && Skills[i].nLevel>0 && (Skills[i].uId ==15 || pSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState || pSkill->GetSkillStyle() == SKILL_SS_Melee || Skills[i].uId ==210))
	// 				{//右边技能
	// 					t_sprintf(nIconName,pSkill->getIconPath().c_str());
	// 					g_GameWorld->setaauxiliaryskillInfo(Skills[i].uId,Skills[i].uGenre,nIconName);
	// 				}*/
	// 				KUiItemdObject skillData;
	// 				ZeroMemory(&skillData,sizeof(KUiItemdObject));
	// 				skillData.nRect  = getRect(SkillNode,0);
	// 				skillData.uId    = Skills[i].uId;
	// 				skillData.uGenre = Skills[i].uGenre;
	// 				skillData.nindex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Skills[i].uId);
	// 				//int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Skills[i].uId);
	// 				skillData.DataH  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddPoint(skillData.nindex);
	// 				skillData.DataW  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetEnChance(skillData.nindex);

	// 				CCLayer * sDescLayer = KuiSkilldesc::create(Skills[i].uId,Skills[i].nLevel,skillData,this,callfuncN_selector(Kuiskill::closeSkilldescPadCallback));
	// 				this->addChild(sDescLayer,2,1001);
	// 				nCurSelindex = Skills[i].uId;
	// 				isThis = true;
	// 			}
	// 			//colorLayer->setContentSize(SkillNode->getContentSize());
	// 			//colorLayer->setPosition(SkillNode->getPosition());
	// 		}
	// 		else
	// 		{
	// 			if (!isThis)
	// 				nCurSelindex = 0;
	// 			SkillNode->setColor(ccWHITE);           //默认颜色
	// 			//colorLayer->setContentSize(CCSizeZero);
	// 		}
	// 	}
	// }
	return true;
}

void Kuiskill::closeSkilldescPadCallback(CCNode *pNode)
{

}

void Kuiskill::closeDescCallback(CCNode *pNode)
{
	//CCMessageBox("成功","成功");
}

void Kuiskill::toprePage(CCObject* pSender)
{

	UIPageView* pageView = (UIPageView *)m_pUiLayer->getWidgetByName("pageView");
	if(pageView){
		UILayout* layout = (UILayout *)pageView->getPage(pageView->getCurPageIndex()-1);
		if (layout){
			if (!layout->isVisible()){
				pageView->scrollToPage(0);
			}else{
				pageView->scrollToPage(pageView->getCurPageIndex()-1);
			}
		}
		
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("toprePage");
	}
	return;
}
void Kuiskill::tonextPage(CCObject* pSender)
{
	UIPageView* pageView = (UIPageView *)m_pUiLayer->getWidgetByName("pageView");
	if(pageView){
		//pageView->scrollToPage(pageView->getCurPageIndex()+1);
		UILayout* layout = (UILayout *)pageView->getPage(pageView->getCurPageIndex()+1);
		if (layout){
			if (!layout->isVisible()){
				pageView->scrollToPage(0);
			}else{
				pageView->scrollToPage(pageView->getCurPageIndex()+1);
			}
		}
	}
	
	return;
}
void Kuiskill::addskillpoint(CCObject* pSender)
{//增加技能
	if (nCurSelindex<=0 || nCurSelindex>=MAX_SKILL)
	{
#ifdef WIN32
		CCMessageBox("请选择一个技能","提示");
#else
		CCMessageBox(UTEXT("请选择一个技能",1).c_str(),UTEXT("提示",1).c_str());
#endif
	   return;
	}
	   nRemainSkillPoint--;	 //使用技能点数
	if (pSkillPointLabel)
	{
		char nPointCount[32];
		t_sprintf(nPointCount,"%d",nRemainSkillPoint);
		pSkillPointLabel->setString(nPointCount);
	}
	if (g_pCoreShell)
	  g_pCoreShell->OperationRequest(GOI_TONE_UP_SKILL,CGOG_SKILL_FIGHT,nCurSelindex);
	//CCLayer * sItemsLayer = KuiItem::create("",this,callfuncN_selector(Kuiskill::closeDescCallback));
	//ParentNode_Fight->addChild(sItemsLayer,2,OBJ_NODE_ITEM);
	//CCMessageBox("成功","成功");
}

void Kuiskill::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	   nRemainSkillPoint = g_pCoreShell->GetGameData(GDI_FIGHT_SKILL_POINT, 0, 0);
	   if (pSkillPointLabel)
	   {
		   char nPointCount[32];
		   t_sprintf(nPointCount,"%d",nRemainSkillPoint);
		  // pSkillPointLabel->setColor(ccWit)
		   pSkillPointLabel->setString(nPointCount);
	   }
	}
}

void Kuiskill::draw()
{
	//CCMessageBox("无限循环","draw");
}

CCTexture2D * Kuiskill::GetiniData(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord)
{
	    if  (sprPath==NULL)
			return NULL;

	    char nSprName[128]={0};
		ZeroMemory(nSprName,sizeof(nSprName));
		t_sprintf(nSprName,sprPath);
		g_StrLower(nSprName);
		int m_nWidth,m_nHeight,nFrams;
		CCTexture2D *bgCur = NULL;
		SPRFRAMSINFO nSprInfo;
		ZeroMemory(&nSprInfo,sizeof(nSprInfo));
		//if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
		bgCur = _getinidata.getinidata_one(nSprName,nFramIndex,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
		
		if (bgCur==NULL)
			return NULL;

#define CENTERX		160
#define	CENTERY		192

		int nX = nMpsX;  //位置
		int nY = nMpsY;  //位置

		if (bSinglePlaneCoord == false)//转换成屏幕坐标 
			SubWorld[0].GetLocalPositionByMps(nX,nY,nMpsZ);

		if (bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
		{//渲染模式
		//****to be modify****
		  int nCenterX = nSprInfo.pHCenterX; //重心的水平位移
		  int nCenterY = nSprInfo.pHCenterY; //重心的垂直位移

		   if (nCenterX || nCenterY)
		   {
			 nX -= nCenterX;
			 nY -= nCenterY;
		   }
		   else if (nSprInfo.pHwidth > CENTERX)
		   {
			  nX -= CENTERX;
			  nY -= CENTERY;
		   }
		}

		if ((bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
		{//帧内有偏移的
			nX += nSprInfo.pFoffsetX;
			nY += nSprInfo.pFoffsetY;
		}
		//开始裁剪
		
		*mScreeX = nX;
		*mScreeY = nY;
	return bgCur;
}
