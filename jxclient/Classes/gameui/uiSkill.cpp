#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "uiskill.h"
#include "engine/CXmlStream.h"
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
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

uiskill::uiskill():
m__pMenu(NULL)
,m_contentPadding(0)
,m_contentPaddingTop(0)
,m_callbackListener(NULL)
,m_callback(NULL)
,m__sfBackGround(NULL)
,m__s9BackGround(NULL)
,m__ltContentText(NULL)
,m__ltTitle(NULL)
,m_ParentNode_Fight(NULL)
{
    //memset(Skills,0,sizeof(KUiSkillData) * 50);
}

uiskill::~uiskill()
{
    CC_SAFE_RELEASE(m__pMenu);
    CC_SAFE_RELEASE(m__sfBackGround);
    CC_SAFE_RELEASE(m__ltContentText);
    CC_SAFE_RELEASE(m__ltTitle);
    CC_SAFE_RELEASE(m__s9BackGround);
	CC_SAFE_RELEASE(m_ParentNode_Fight);
}

bool uiskill::init()
{
    bool bRef = false;
    do
    {
        CC_BREAK_IF(!CCLayer::init());

        this->setContentSize(CCSizeZero);
        // 初始化需要的 Menu
        CCMenu* menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        setMenuButton(menu);
        setTouchEnabled(true);
        bRef = true;
    } while (0);
    return bRef;
}


void uiskill::registerWithTouchDispatcher()
{
    // 这里的触摸优先级设置为 -128 这保证了，屏蔽下方的触摸
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
}

bool uiskill::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{//点击了这个层
   // CCLog("uiskill touch");
    return true;
}


uiskill* uiskill::create(const char *backgroundImage,int nKind)
{
    uiskill* ml = uiskill::create();
	if  (!nKind)
	{
      ml->setSpriteBackGround(CCSprite::create(backgroundImage));//背景的精灵
      ml->setSprite9BackGround(CCScale9Sprite::create(backgroundImage));
	}
	else
	{//纹理加载，不能缩放
		char nSprName[128]={0};
		ZeroMemory(nSprName,sizeof(nSprName));
		t_sprintf(nSprName,backgroundImage);
		g_StrLower(nSprName);
		int m_nWidth,m_nHeight,nFrams;
		CCTexture2D *bgCur = NULL;
		SPRFRAMSINFO nSprInfo;
		ZeroMemory(&nSprInfo,sizeof(nSprInfo));
		bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
		if (bgCur==NULL)
			return NULL;
		ml->setSpriteBackGround(CCSprite::createWithTexture(bgCur));//背景的精灵
	}

	  CCSprite *nFightNode =  CCSprite::create();
	  nFightNode->setPosition(ccp(0,0));
	  nFightNode->setAnchorPoint(ccp(0,0));
	  

	  KUiSkillData	Skills[FIGHT_SKILL_COUNT];

	  if (g_pCoreShell)
		  g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (unsigned int)Skills, 0); //获取分页技能数据

	  for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; ++i)
	  {//战斗技能
		  //m_FightSkills[i].HoldObject(Skills[i].uGenre, Skills[i].uId, Skills[i].nLevel, 0);//显示鼠标进过的物品描述，设置容纳对象的类型
		  ISkill *pSkill = g_SkillManager.GetSkill(Skills[i].uId,1);
		  if (pSkill)
		  {//绘画技能图标
			  //pSkill->m_szSkillIcon;
			  //pSkill->DrawSkillIcon(x, y, Width, Height,inParam);  //绘制技能图标
			  //pSkill->getIconPath()
			  //char msg[64];
			  //t_sprintf(msg,"idx:%d,Skillid:%d",i,Skills[i].uId);
			  //CCMessageBox(pSkill->getIconPath().c_str(),"绘画技能图标");
			  //CCSprite *nSkillSpr =CCSprite::create();
			  //nSkillSpr->setAnchorPoint(ccp(0,0));
			  //nFightNode->addChild(nSkillSpr,1,i+1);

			  char nSprName[128]={0};
			  ZeroMemory(nSprName,sizeof(nSprName));
			  t_sprintf(nSprName,pSkill->getIconPath().c_str());
			  g_StrLower(nSprName);
			  int m_nWidth,m_nHeight,nFrams;
			  CCTexture2D *bgCur = NULL;
			  SPRFRAMSINFO nSprInfo;
			  ZeroMemory(&nSprInfo,sizeof(nSprInfo));
			  bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
			  if (bgCur==NULL)
				  return ml;

			  CCRect CCRectZero = CCRectMake(0,0,5,5);
			  CCRectZero.size = bgCur->getContentSize();
			  nFightNode->setTextureRect(CCRectZero,false,CCRectZero.size);
			  nFightNode->setTexture(bgCur);
			  nFightNode->setPosition(ccp(0,0));
			  //this->setSpriteBackGround(nSkillSpr);
			  //getSpriteBackGround()->setTexture(bgCur);
			   ml->setParentNode_Fight(nFightNode); 
		  }
	  }
	  

    return ml;
}


void uiskill::setTitle(const char*title,int fontsize)
{
    CCLabelTTF* ltfTitle = CCLabelTTF::create(title,UI_GAME_FONT_DEFAULT,fontsize);
    setLabelTitle(ltfTitle);
}

void uiskill::setContentText(const char *text, int fontsize, int padding, int paddingTop){
    CCLabelTTF* ltf = CCLabelTTF::create(text,UI_GAME_FONT_DEFAULT,fontsize);
    setLabelContentText(ltf);
    m_contentPadding    = padding;
    m_contentPaddingTop = paddingTop;
}

void uiskill::setCallbackFunc(cocos2d::CCObject *target, SEL_CallFuncN callfun)
{
    m_callbackListener = target;
    m_callback         = callfun;
}


bool uiskill::addButton(const char *normalImage, const char *selectedImage, const char *title, int tag,int isSpr){
    
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();//当前窗口的高度和宽度
    CCPoint pCenter = ccp(winSize.width/2, winSize.height/2);
    
    // 创建图片菜单按钮
    CCMenuItemImage* menuImage = CCMenuItemImage::create(normalImage, selectedImage, this, menu_selector(uiskill::buttonCallback));
    menuImage->setTag(tag);
    menuImage->setPosition(pCenter);
    // 添加文字说明并设置位置 文字按钮
    CCSize imenu = menuImage->getContentSize();
    CCLabelTTF* ttf = CCLabelTTF::create(title,"",20);
    ttf->setColor(ccc3(0,0,0));
    ttf->setPosition(ccp(imenu.width/2,imenu.height/2));
    menuImage->addChild(ttf); 
    getMenuButton()->addChild(menuImage);
    return true;
}


void uiskill::buttonCallback(cocos2d::CCObject *pSender){
    CCNode* node = dynamic_cast<CCNode*>(pSender);
    //CCLog("touch tag: %d", node->getTag());
    if (m_callback && m_callbackListener){
        (m_callbackListener->*m_callback)(node);
    }
	//释放了
    this->removeFromParentAndCleanup(true);
}

void uiskill::onEnter()
{
    CCLayer::onEnter();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint pCenter = ccp(winSize.width / 2, winSize.height / 2);
    
    CCSize contentSize;
     // 设定好参数，在运行时加载
    if (getContentSize().equals(CCSizeZero))
    {//如果没有设置大小的
        getSpriteBackGround()->setPosition(ccp(winSize.width / 2, winSize.height / 2));
        this->addChild(getSpriteBackGround(),0,0);
        contentSize = getSpriteBackGround()->getTexture()->getContentSize();
    }else
    {//如果设置了 就按照 9格缩放
        CCScale9Sprite *background = getSprite9BackGround();
        background->setContentSize(getContentSize());
        background->setPosition(ccp(winSize.width / 2, winSize.height / 2));
        this->addChild(background,0);
        contentSize = getContentSize();
    }
    
     // 添加按钮，并设置其位置
    this->addChild(getMenuButton());
    //float btnWidth = contentSize.width/(getMenuButton()->getChildrenCount()+1);
    
    CCArray* array = getMenuButton()->getChildren();
    CCObject* pObj = NULL;
	
	CCMenuItemImage* itemImage = dynamic_cast<CCMenuItemImage*>(array->objectAtIndex(0));
    int count = getMenuButton()->getChildrenCount();
    int btnWidth = itemImage->getContentSize().width;
    int padingWidth = (contentSize.width - btnWidth * count) / (count + 1);
	int i = 0;
    CCARRAY_FOREACH(array, pObj)
    {//设置按钮的位置 
        CCNode* node = dynamic_cast<CCNode*>(pObj);
        node->setAnchorPoint(ccp(0, 0.5f)); //设置描点？
        node->setPosition(ccp(winSize.width * 0.5f - contentSize.width * 0.5f + padingWidth * (i + 1) + btnWidth * i, winSize.height * 0.5f - contentSize.height / 3));
        i++;     
    }
	
    /*int i = 0; 原来的
    CCARRAY_FOREACH(array, pObj)
    {
        CCNode* node = dynamic_cast<CCNode*>(pObj);
        node->setPosition(ccp(winSize.width / 2 - contentSize.width / 2 + btnWidth * (i + 1), winSize.height / 2 - contentSize.height / 3));
        i++;
    }*/
    
    // 显示对话框标题
    if (getLabelTitle())
    {//如果有标题的话
        getLabelTitle()->setPosition(ccpAdd(pCenter, ccp(0, contentSize.height / 2 - 35.0f)));
        this->addChild(getLabelTitle());
    }
    
    // 显示文本内容
    if (getLabelContentText())
    {
        CCLabelTTF* ltf = getLabelContentText();
        ltf->setPosition(ccp(winSize.width / 2, winSize.height / 2));
          ltf->setDimensions(CCSizeMake(contentSize.width - m_contentPadding * 2, contentSize.height - m_contentPaddingTop));
         ltf->setHorizontalAlignment(kCCTextAlignmentLeft);
        this->addChild(ltf);
    }
	this->setPosition(ccp(0,0));   //控制整个层的位置
    
	//ParentNode_Fight = CCNode::create();
	//ParentNode_Fight->setAnchorPoint(ccp(0,0));
	//ParentNode_Fight->setPosition(ccp(0,0));
	this->addChild(getParentNode_Fight(),100);
	getParentNode_Fight()->setPosition(ccp(winSize.width/2, winSize.height/2));
	
	if  (!getParentNode_Fight())
		return;

}


void uiskill::onExit()
{
    //CCLog("popup on exit.");
    CCLayer::onExit();
}

void uiskill::update(float delta)
{
	//CCMessageBox("无限循环","update");
}

void uiskill::draw()
{
	//CCMessageBox("无限循环","draw");
}