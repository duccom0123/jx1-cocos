#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiViewRoleState.h"
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

enum WAIT_OTHER_WND_OPER_PARAM
{
	UISTOREBOX_IS_AVATAR, 
};
// ---> 建立控件与UIEP_*以及可接纳物品的类型的对应关系
static struct UE_CTRL_MAP_VIEW
{
	int				nPosition;
	const char*		pIniSection;
	int             nCellx;    //宽占用几个格子
	int             nCelly;    //高占用几个格子
	float           nOffsetX;  //在面板中的位置偏移固定的
	float           nOffsetY;  //在面板中的位置偏移固定的
}ViewCtrlItemMap[_ITEM_COUNT] =
{
	{ UIEP_HEAD,		"Cap"		,2,2,119,51},	//0装备-帽子
	{ UIEP_HAND,		"Weapon"	,2,4,217,122},	//1装备-武器
	{ UIEP_NECK,		"Necklace"	,2,1,218,83},	//2装备-项链
	{ UIEP_FINESSE,		"Bangle"	,1,2,46,95},	//3装备-手镯
	{ UIEP_BODY,		"Cloth"		,2,3,119,111},	//4装备-衣服
	{ UIEP_WAIST,		"Sash"		,2,1,119,199},	//5装备-腰带
	{ UIEP_FINGER1,		"Ring1"		,1,1,46,153},	//6装备-戒指
	{ UIEP_FINGER2,		"Ring2"		,1,1,46,183},	//7装备-戒指
	{ UIEP_WAIST_DECOR,	"Pendant"	,1,2,46,216},	//8装备-腰缀
	{ UIEP_FOOT,		"Shoes"		,2,2,217,239},	//9装备-鞋子
	{ UIEP_HORSE,		"Horse"		,2,3,119,240},	//10装备-马
	{ UIEP_MASK,		"Mask"		,1,1,46,51},	//11装备-面具	// mat na
	{ UIEP_PIFENG,	    "Mantle"	,1,1,46,305},	//12披风
	{ UIEP_YINJIAN,		"Signet"	,1,1,213,297},	//13印鉴
	{ UIEP_SHIPING,	    "Shipin"	,1,1,247,297},	//14饰品
};

KuiViewRoleState::KuiViewRoleState()
{
	isOpen = false;
	//memset(Skills,0,sizeof(KUiSkillData) * 50);
	colorLayer        = NULL;
	red               = NULL;
	ItemBoxNode       = NULL;
	m_nMoney = 0;
	m_nXu = 0;
	m_nNumObjects = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	m_descPadIsOpen =false;
	m_CurSelItemIndex = 0;
	for (int i=0;i<_ITEM_COUNT;i++)
	    m_itemIndexBox[i]=0;

	pRoleName=NULL;
	pRoleTitle=NULL;
	//pRoleTitle_1=NULL;
	pPk=NULL;

	pZhuanSheng=NULL;
	pZhuanShengLevel=NULL;

	__RemainPoint = 0;
	ncurDest = NULL;
}

KuiViewRoleState::~KuiViewRoleState()
{
	isOpen = false;
	colorLayer        = NULL;
	if (red)
	   red->release();
	//red               = NULL;
}

//创建确定弹出框
KuiViewRoleState * KuiViewRoleState::create(KUiPlayerItem* pDest,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiViewRoleState * popLayer = KuiViewRoleState::create();
	popLayer->ncurDest = pDest;

	//memcpy(&m_pSelf->m_ZhuangBeiPad.m_UseData, m_UseData, sizeof(KUiPlayerItem));     //装备信息
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;

	popLayer->UpdateData(pDest);
	return popLayer;
}

//初始化
bool KuiViewRoleState::init()
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
	/*char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\组队\\组队2.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return false;*/
	CCSprite * spriteBox = CCSprite::create("ui/state/view_other.png");
	ParentNode_Team= spriteBox;
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

void KuiViewRoleState::addDialogData()
{
	if  (!g_pCoreShell) return;

	/*ItemBoxNode = CCNode::create();
	ItemBoxNode->setPosition(ccp(0,0));
	ItemBoxNode->setAnchorPoint(ccp(0,0));
	ParentNode_Team->addChild(ItemBoxNode,2);*/
	//Left=24
	//Top=72
	m_StartPos.x =24;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =72;//+ITEM_CELL_SIZE/2;

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);  //设置和窗口的大小
	ParentNode_Team->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	red->retain();
	float nfontsize =12;
	float nScale = nfontsize/16;
	//开始创建文本信息
	pRoleName = CCLabelTTF::create(UTEXT("测试测试1",1).c_str(),UI_GAME_FONT_DEFAULT, nfontsize);//Arial
	pRoleName->setAnchorPoint(ccp(0,0));
	pRoleName->setPosition(ccp(50,m_size.height/2+142));
	ParentNode_Team->addChild(pRoleName);

	pRoleTitle = CCLabelTTF::create(UTEXT("测试测试2",1).c_str(),UI_GAME_FONT_SONG, nfontsize);   //Arial
	pRoleTitle->setAnchorPoint(ccp(0,0));
	pRoleTitle->setPosition(ccp(50,m_size.height/2+124));
	pRoleTitle->setColor(ccGREEN);
	ParentNode_Team->addChild(pRoleTitle);

	/*pRoleTitle_1 = CCLabelTTF::create(UTEXT("测试测试3",1).c_str(),UI_GAME_FONT_SONG,nfontsize);   //Arial
	pRoleTitle_1->setAnchorPoint(ccp(0,0));
	pRoleTitle_1->setPosition(ccp(360,m_size.height/2+140));
	ParentNode_Team->addChild(pRoleTitle_1);*/

	pPk   = CCLabelBMFont::create("0",UI_GAME_FONT_NUMBER);
	pPk->setAnchorPoint(ccp(0,0));
	pPk->setPosition(ccp(m_size.width-55,m_size.height/2+138));
	ParentNode_Team->addChild(pPk);

	pZhuanSheng = CCLabelTTF::create(UTEXT("转生次数",1).c_str(),UI_GAME_FONT_SONG, 12);   //Arial
	pZhuanSheng->setAnchorPoint(ccp(0,0));
	pZhuanSheng->setPosition(ccp(m_size.width-100,m_size.height/2+124));
	ParentNode_Team->addChild(pZhuanSheng);

	pZhuanShengLevel = CCLabelTTF::create(UTEXT("转生等级",1).c_str(),UI_GAME_FONT_SONG, 12);   //Arial
	pZhuanShengLevel->setAnchorPoint(ccp(0,0));
	pZhuanShengLevel->setPosition(ccp(m_size.width-100,m_size.height/2+108));
	ParentNode_Team->addChild(pZhuanShengLevel);

}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiViewRoleState::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	//CCSprite * sprite_tips_btn_confirm_normal =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	//CCSpriteFrame * f_tips_btn_confirm_select = sfCache->spriteFrameByName("btn_confirm_select.png");
	//CCSprite * sprite_tips_btn_confirm_select =  CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_select);
	//"\\spr\\UI3\\主界面\\关闭.spr"
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\activityguide\\closebutton.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
    CCSprite * sprite_tips_btn_confirm_normal = CCSprite::createWithTexture(bgCur);

	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_tips_btn_confirm_select = CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * itemConfirm   = CCMenuItemSprite::create(sprite_tips_btn_confirm_normal, sprite_tips_btn_confirm_select, this, menu_selector(KuiViewRoleState::buttonCallBackFunc));
	itemConfirm->setPosition(ccp(0,0));
	
	 CCMenu * menu = CCMenu::create(itemConfirm,NULL);
	 menu->setPosition(ccp(m_size.width/2,13));
	 ParentNode_Team->addChild(menu);          //加到父层里
	//CCPoint bgSpritePoint = ParentNode_Team->getPosition();
}


//执行上层对象的回调函数，关闭弹出框
void KuiViewRoleState::buttonCallBackFunc(CCObject * pSender)
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
void KuiViewRoleState::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiViewRoleState::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128,true);
}


void KuiViewRoleState::UpdateAllEquips()
{
	if  (!g_GameWorld || !g_pCoreShell) return;

	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_PARADE_EQUIPMENT, (unsigned int)&Equips, 0);
	
	int	i;

	//for (i = 0; i < _ITEM_COUNT; ++i)
		//m_EquipBox[i].Celar();

	for (i = 0; i < nCount; ++i)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);  //增加或更换
		else
		    UpdateEquip(&Equips[i], false); //删除这个精灵
	}
}

void KuiViewRoleState::addpicBox(int index,int nItemIndx,bool isAdd)
{
	char nKey[32];
	t_sprintf(nKey,"%s",ViewCtrlItemMap[index].pIniSection);
	std::string m_ItemBoxKey = nKey;
	t_sprintf(nKey,"%s_color",ViewCtrlItemMap[index].pIniSection);
	std::string m_colorbgKey = nKey;//颜色层的key
	if  (isAdd)
	{//增加 
		if (nItemIndx<=0 || nItemIndx>=MAX_ITEM) return;
		if (ParentNode_Team)
		{
			char nItemSprPath[256];
			ZeroMemory(nItemSprPath,sizeof(nItemSprPath));
			CCTexture2D *bgCur = NULL;
			int nItemW = Item[nItemIndx].GetWidth()*ITEM_CELL_SIZE;
			int nItemH = Item[nItemIndx].GetHeight()*ITEM_CELL_SIZE;
			m_itemIndexBox[index]=nItemIndx;
			if  (Item[nItemIndx].GetImagePath())
			{
				t_sprintf(nItemSprPath,Item[nItemIndx].GetImagePath());
			}
			else
				t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");

			if  (nItemSprPath[0])
			{
				//t_sprintf(nItemSprPath,Item[nItemIndx].GetImagePath());
				g_StrLower(nItemSprPath);
				int m_nWidth,m_nHeight,nFrams;
				SPRFRAMSINFO nSprInfo;
				ZeroMemory(&nSprInfo,sizeof(nSprInfo));
				bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
			}

			if (bgCur==NULL)
				return;

		   CCSprite * tempSpr = (CCSprite *)ParentNode_Team->getChildByTagKey(m_ItemBoxKey);
		   CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Team->getChildByTagKey(m_colorbgKey);
		   
		   CCSize bgsize;
		   bgsize.width  = ViewCtrlItemMap[index].nCellx*ITEM_CELL_SIZE;
		   bgsize.height = ViewCtrlItemMap[index].nCelly*ITEM_CELL_SIZE;
		   CCSize nTempSize = bgCur->getContentSize();
		   float tempOffsetY = 55;
		   if (!tempSpr)
		   {   
			   tempSpr =CCSprite::createWithTexture(bgCur);
			   tempSpr->setPosition(ccp(0,0));
			   tempSpr->setAnchorPoint(ccp(0,0));
			   tempSpr->setTagbyKey(m_ItemBoxKey);
			   //tempSpr->setPosition(ccp(m_size.width/2+CtrlItemMap[index].nOffsetX+bgsize.width/2-nItemW/2,m_size.height-CtrlItemMap[index].nOffsetY));
			   ParentNode_Team->addChild(tempSpr,2);
			   
			   if (!bgcolorLayer)
			   {
				   ccColor4B color = ccc4(112, 128, 144, 150);
				   bgcolorLayer = CCLayerColor::create(color);//颜色层
				   //bgcolorLayer->setPosition(CCPointZero);
				   bgcolorLayer->setAnchorPoint(ccp(0,0));
				  
				   bgcolorLayer->setContentSize(bgsize);//设置和窗口的大小
				   bgcolorLayer->setTagbyKey(m_colorbgKey);
				   bgcolorLayer->setPosition(ccp(0+ViewCtrlItemMap[index].nOffsetX,m_size.height-ViewCtrlItemMap[index].nOffsetY-bgsize.height-tempOffsetY));
				   ParentNode_Team->addChild(bgcolorLayer,1);
			   }
			 //  return;
		   }
		   //tempSpr->setContentSize(nTempSize);
		   CCRect nRectZero = CCRectMake(0,0,0,0);
		   nRectZero.size = bgCur->getContentSize();
		   tempSpr->setTexture(bgCur);
		   tempSpr->setTextureRect(nRectZero,false,nRectZero.size);
		   tempSpr->setPosition(ccp(0+ViewCtrlItemMap[index].nOffsetX+bgsize.width/2-nTempSize.width/2,m_size.height-ViewCtrlItemMap[index].nOffsetY-(bgsize.height+nTempSize.height)/2-tempOffsetY));
		  
		   //开始检测设置背景颜色
		   KUiObjAtContRegion	Obj;
		   Obj.Obj.uGenre = CGOG_ITEM;
		   Obj.Obj.uId    = nItemIndx;
		   Obj.Region.h     = Obj.Region.v = 0;
		   Obj.Region.Width = Obj.Region.Height = 0;
		   Obj.nContainer   = UOC_EQUIPTMENT;

		   ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
			   GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
		   //if (eProp == IIEP_NORMAL) //可用
		   //else 
		   if (eProp == IIEP_NOT_USEABLE)  //不可用
		   {
			   ccColor3B color_ = ccc3(128, 0, 0);
			   if (bgcolorLayer)
			      bgcolorLayer->setColor(color_);
		   }
		}
	}
	else
	{//删除
		if (ParentNode_Team)
		{
			m_itemIndexBox[index]=0;
			ParentNode_Team->removeChildByTagKey(m_ItemBoxKey);
			ParentNode_Team->removeChildByTagKey(m_colorbgKey);
		}
	}
}
//--------------------------------------------------------------------------
//	功能：装备变化更新
//--------------------------------------------------------------------------
void KuiViewRoleState::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
{
	if (Item && pEquip)
	{
		for (int i = 0; i < _ITEM_COUNT; ++i)
		{
			if (ViewCtrlItemMap[i].nPosition == pEquip->Region.v)
			{//就是这个精灵
				if (bAdd)//增加
					//m_EquipBox[i].HoldObject(pEquip->Obj.uGenre, pEquip->Obj.uId,
					//pEquip->Region.Width, pEquip->Region.Height);
					//CCMessageBox(Item[pEquip->Obj.uId].GetImagePath(),"增加");
					addpicBox(i,pEquip->Obj.uId,true);
				else//移除
					addpicBox(i,pEquip->Obj.uId,false);
					//CCMessageBox(Item[pEquip->Obj.uId].GetImagePath(),"移除");
					//m_EquipBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				
				break;
			}
		}
	}
}

CCRect KuiViewRoleState::getRect(CCNode* pNode,int i)
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
bool KuiViewRoleState::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * itemBoxSpr =NULL;
	bool isthis=false;
	for (int i = 0; i < _ITEM_COUNT; ++i)
	{
		char nKey[32];
		t_sprintf(nKey,"%s",ViewCtrlItemMap[i].pIniSection);
		std::string m_ItemBoxKey = nKey;
		t_sprintf(nKey,"%s_color",ViewCtrlItemMap[i].pIniSection);
		std::string m_colorbgKey = nKey;//颜色层的key
		itemBoxSpr = (CCSprite *)ParentNode_Team->getChildByTagKey(m_ItemBoxKey);
		CCLayerColor *colorBoxbg = (CCLayerColor *)ParentNode_Team->getChildByTagKey(m_colorbgKey);
		//CCMessageBox("点击开始","点击开始");
		if  (itemBoxSpr && colorBoxbg)
		{
			if (getRect(colorBoxbg,i+1).containsPoint(location))
			{//点中了
				//restartAction();
				itemBoxSpr->runAction(red);  
				//ccColor3B color_ = ccc3(128, 0, 0);
				//if (colorBoxbg)
					//colorBoxbg->setColor(color_);
				KUiItemdObject second;
				second.uId    = m_itemIndexBox[i];
				second.uGenre = CGOG_ITEM;
				second.nRect  = getRect(colorBoxbg,0);
				second.nGenkind  =  UOC_EQUIPTMENT;
				CCLayer * sDescLayer = KuiItemdesc::create(second,this,callfuncN_selector(KuiViewRoleState::closedescPadCallback),0);
				m_descPadIsOpen = true;
				m_CurSelItemIndex = m_itemIndexBox[i];//m_ItemData[i].uId;
				//nstrCurSelItemKey =nItemKey;
				isthis = true;
				this->addChild(sDescLayer,2,1001);
			}
			else
			{
				if (!isthis)
				  m_CurSelItemIndex =0;

				itemBoxSpr->setColor(ccWHITE);           //默认颜色
			}
		}
	}
	return true;
}

void KuiViewRoleState::closedescPadCallback(CCNode *pNode)
{
	m_descPadIsOpen = false;
}

void KuiViewRoleState::UpdateData(KUiPlayerItem* pDest)
{
	if (pDest == NULL)
		return;

	UpdateBaseData(pDest);

	UpdateAllEquips();
}

void KuiViewRoleState::UpdateBaseData(KUiPlayerItem* pDest)
{
	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	if (g_pCoreShell)
	    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info,pDest->uId);

	pRoleName->setString(UTEXT(Info.Name,1).c_str());
	pRoleTitle->setString(UTEXT(Info.Title,1).c_str());
	//pRoleTitle_1->setString(UTEXT(Info.Agname,1).c_str());
	//m_Agname.SetText(Info.Agname);  //绰号
	//m_Name  .SetText(Info.Name);    //名字
	//m_Title .SetText(Info.Title);   //称号
}

void KuiViewRoleState::BreatheUpdateInfo()
{
	/*
	KUiPlayerRuntimeInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
	if (g_pCoreShell)
	    g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

	char nTempS4tr[64];
	ZeroMemory(nTempS4tr,sizeof(nTempS4tr));
	t_sprintf(nTempS4tr,"%d/%d",Info.nLife,Info.nLifeFull);
	pLife->setString(nTempS4tr);
	t_sprintf(nTempS4tr,"%d/%d",Info.nMana,Info.nManaFull);
	pMana->setString(nTempS4tr);
	t_sprintf(nTempS4tr,"%d/%d",Info.nStamina,Info.nStaminaFull);
	pTili->setString(nTempS4tr);
	t_sprintf(nTempS4tr,"%d/%d",Info.nExperience,Info.nExperienceFull);
	pExp->setString(nTempS4tr);
	*/
	//UiNewStatus* pStatus = UiNewStatus::GetIfVisible();
	//if (pStatus)
	//	UpdateRuntimeInfo(&Info);
	//pStatus=NULL;
}


void  KuiViewRoleState::BreathAttributeUpdate()
{
	KUiPlayerAttribute	Info;
	memset(&Info, 0, sizeof(KUiPlayerAttribute));
	if (g_pCoreShell)
	   g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);

	char nTempS4tr[64];
	ZeroMemory(nTempS4tr,sizeof(nTempS4tr));
	t_sprintf(nTempS4tr,"%d",Info.nPKValue);
	pPk->setString(nTempS4tr);

	t_sprintf(nTempS4tr,"转生次数:%d",Info.nReBorn);
	pZhuanSheng->setString(UTEXT(nTempS4tr,1).c_str());

	//t_sprintf(nTempS4tr,"转生次数:%d",Info.);
	//pZhuanSheng->setString(UTEXT(nTempS4tr,1).c_str());

}
void KuiViewRoleState::update(float delta)
{
	if (isOpen && g_pCoreShell)
	{
	   //UpdateAllEquips();
	   //BreatheUpdateInfo();
	   //BreathAttributeUpdate();
	}
}

void KuiViewRoleState::draw()
{
	//CCMessageBox("无限循环","draw");
}