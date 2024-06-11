//有按钮的　ｔａｌｋ　对话框
//
#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "Kuigive.h"
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
#include "gameui/KuiItemdesc.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

Kuigive::Kuigive()
{
	isOpen = false;
	colorLayer        = NULL;
	//red               = NULL;
	ParentNode_Item   = NULL;
	ParentNode_Give   = NULL;
	m_StartPos.x=0;
	m_StartPos.y=0;
	m_StartPos_g.x = 0;
	m_StartPos_g.y = 0;
	btnBackPic = NULL;
	m_nItemNumObjects = 0;
	m_ngiveItemNumObjects = 0;
	m_ItemData.clear();
	m_giveItemData.clear();
	m_nIstrue = false;
	nstrCurSelItemKey = "";
	inCurGiveItemKey  = "";
	m_descPadIsOpen = false;
	m_giveNum = 0;
	m_itemNum = 0;
	m_nNeed   = 0;
	nBakChickTime = 0;
	m_nMoney =0;
	m_nXu    =0;
}

Kuigive::~Kuigive()
{
	isOpen = false;
	colorLayer = NULL;
	//if (red)
	//	red->release();

	m_ItemData.clear();
	m_giveItemData.clear();
}

//创建确定弹出框
Kuigive * Kuigive::create(char * strTitel,char *pContent,int nNum,char* pszCallback,char* pszError,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	Kuigive * popLayer = Kuigive::create();
	popLayer->m_nNeed = nNum;  //是执行脚本类型 kind  还是不执行脚本
	t_sprintf(popLayer->m_Callback,"%s",pszCallback);
	t_sprintf(popLayer->m_Error,"%s",pszError);
	popLayer->addDialogData(strTitel,pContent);
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}


//初始化
bool Kuigive::init()
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\给予界面\\给予.spr");
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
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size   = contentSize;
	m_origin = spriteBox->getPosition();
	spriteBox->setPosition(ccp(winSize.width/2-m_size.width/2,winSize.height/2));
	this->addChild(spriteBox);
	
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne);   //禁止多点触摸
	return true;
}

void Kuigive::addDialogData(char * strTitel,char *pContent)
{
	if  (!g_pCoreShell) return;

	g_pCoreShell->ClearClientRoom(room_give);

	CCSize nSize;
	nSize.width =150;
	nSize.height=100;

	m_StartPos.x =m_size.width+25;    //+ITEM_CELL_SIZE/2;
	m_StartPos.y =3;//+ITEM_CELL_SIZE/2;

	m_StartPos_g.x =22;
	m_StartPos_g.y =192;
	
	char nTempStr[128];
	ParentNode_Give =CCNode::create();
	ParentNode_Item =CCNode::create();
	ParentNode_ItemEx->addChild(ParentNode_Item);
	ParentNode_ItemEx->addChild(ParentNode_Give);
	/*t_sprintf(nTempStr,strTitel);
	CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(m_size.width/2,20));
	ParentNode_ItemEx->addChild(ptextLabel);*/
	t_sprintf(nTempStr,pContent);
	CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,12,nSize,kCCTextAlignmentLeft);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setPosition(ccp(128/2+40,m_size.height-143));
	ParentNode_ItemEx->addChild(pstrLabel);

	char ntitleStr[64];
	t_sprintf(ntitleStr,strTitel);
	CCLabelTTF *ptitLabel = CCLabelTTF::create(UTEXT(ntitleStr,1).c_str(),UI_GAME_FONT_DEFAULT,12);//Arial
	ptitLabel->setPosition(ccp(m_size.width/2,m_size.height-80));
	ParentNode_ItemEx->addChild(ptitLabel);
	
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\道具\\道具面板3.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * itemBox = CCSprite::createWithTexture(bgCur);
	//ParentNode_Item = spriteBox;
	itemBox->setPosition(ccp(m_size.width+itemBox->getContentSize().width/2,winSize.height/2-50));
	ParentNode_Item->addChild(itemBox);
	itemBoxSize = itemBox->getContentSize();

	pMoneyLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//Arial
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(m_size.width+60,70));
	ParentNode_ItemEx->addChild(pMoneyLabel,1);

	pXuLabel = CCLabelTTF::create("","fonts/gb_song.ttf", 12);//Arial
	pXuLabel->setAnchorPoint(ccp(0,0));
	pXuLabel->setPosition(ccp(m_size.width+60,48));
	ParentNode_ItemEx->addChild(pXuLabel,1);


	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\给予界面\\给予.spr");
	g_StrLower(nSprName);
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * giveBox = CCSprite::createWithTexture(bgCur);
	giveBox->setPosition(ccp(m_size.width/2,m_size.height/2));
	ParentNode_Give->addChild(giveBox);
	giveBoxSize = giveBox->getContentSize();
	/*btnBackPic = CCSprite::create("ui/btn_3.png");
	btnBackPic->setAnchorPoint(ccp(0,0));
	btnBackPic->setPosition(ccp(m_size.width,m_size.height/2-btnBackPic->getContentSize().height/2));
	ParentNode_ItemEx->addChild(btnBackPic);*/

	KUiObjAtRegion* pObjs = NULL;
	int nCount   = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nItemNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
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
				/*m_ItemData[i].uGenre = pObjs[i].Obj.uGenre;
				m_ItemData[i].uId    = pObjs[i].Obj.uId;
				m_ItemData[i].nindex = i;
				m_ItemData[i].DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
				m_ItemData[i].DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
				m_ItemData[i].DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
				m_ItemData[i].DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
				m_ItemData[i].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带
				*/
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
					t_sprintf(nItemKey,"item_%u",Item[pObjs[i].Obj.uId].GetID());
					std::string nKey =nItemKey;
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

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
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150); //112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey = nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
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
						ParentNode_Item->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

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

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCSizeZero);//设置和窗口的大小
	ParentNode_Item->addChild(colorLayer,1000);
	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
}


//设置确定取消按钮,参数：调用层对象，调用层回调函数
void Kuigive::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\特殊对话条\\输入文字条－确定.spr");
	g_StrLower(nSprName);
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	CCPoint nStartPos;
	nStartPos.x = m_size.width/2+20;
	nStartPos.y = m_size.height/2;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_ok_btn_confirm_normal =  CCSprite::createWithTexture(bgCur);//CCSprite::create("close.png");//CCSprite::createWithSpriteFrame(f_tips_btn_confirm_normal);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_ok_btn_confirm_select =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * okConfirm = CCMenuItemSprite::create(sprite_ok_btn_confirm_normal, sprite_ok_btn_confirm_select, this, menu_selector(Kuigive::okCallBackFunc));
	okConfirm->setPosition(ccp(-38,0));
	//关闭
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\Spr\\Ui3\\特殊对话条\\输入文字条－取消.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_close_select = CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * closeConfirm = CCMenuItemSprite::create(sprite_close_normal, sprite_close_select, this, menu_selector(Kuigive::buttonCallBackFunc));
	closeConfirm->setPosition(ccp(38,0));

	//开始增加操作按钮
	CCSprite * sprite_fanzhi_normal =  CCSprite::create("ui/btn/fz01.png");
	CCSprite * sprite_fanzhi_select = CCSprite::create("ui/btn/fz02.png");
	CCMenuItemSprite * fanzhiConfirm = CCMenuItemSprite::create(sprite_fanzhi_normal, sprite_fanzhi_select, this, menu_selector(Kuigive::btnCallBackFunc_fangzhi));
	fanzhiConfirm->setPosition(ccp(m_size.width/2+38,-30));

	CCSprite * sprite_quxia_normal =  CCSprite::create("ui/btn/qx01.png");
	CCSprite * sprite_quxia_select = CCSprite::create("ui/btn/qx02.png");
	CCMenuItemSprite * quxiaConfirm = CCMenuItemSprite::create(sprite_quxia_normal, sprite_quxia_select, this, menu_selector(Kuigive::btnCallBackFunc_quxia));
	quxiaConfirm->setPosition(ccp(m_size.width/2+38+sprite_quxia_normal->getContentSize().width,-30));
	//CCPoint bgSpritePoint = ParentNode_ItemEx->getPosition();
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\zenglibtn.spr");
	g_StrLower(nSprName);
	bgCur = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zhengli_normal =  CCSprite::createWithTexture(bgCur);
	bgCur = _getinidata.getinidata_one(nSprName,1,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
	if (bgCur==NULL)
		return;
	CCSprite * sprite_zhengli_select =  CCSprite::createWithTexture(bgCur);
	CCMenuItemSprite * zhengliConfirm = CCMenuItemSprite::create(sprite_zhengli_normal, sprite_zhengli_select, this, menu_selector(Kuigive::zhengCallBackFunc));
	zhengliConfirm->setPosition(ccp(m_size.width/2+35+sprite_quxia_normal->getContentSize().width*2,-30));
	
	
	CCMenu * menu = CCMenu::create(okConfirm,closeConfirm,fanzhiConfirm,quxiaConfirm,zhengliConfirm,NULL);

	menu->setPosition(ccp(m_size.width/2,45)); //菜单位置设置为弹出框左下
	ParentNode_ItemEx->addChild(menu);         //加到父层里
}


void Kuigive::zhengCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
	}

	if  (m_ItemData.count(nstrCurSelItemKey)<=0)
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

	KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);

	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数

	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=2)
	{//间隔２秒
		//nBakChickTime = nCurChicktime;
#ifdef WIN32
		CCMessageBox("整理的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("整理的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif
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
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipment); 
}


void Kuigive::okCallBackFunc(CCObject * pSender)
{
	if (g_pCoreShell)
	{
		//		m_nNum = m_ItemBox.GetObjectNum();
		KUiDraggedObject pObj;
		PLAYER_GIVEUI pUI;
		//char ngiveKey[32];
		ZeroMemory(&pUI,sizeof(pUI));
		int i=0;
		_giveItemData::iterator it_g;
		for(it_g = m_giveItemData.begin(); it_g != m_giveItemData.end(); ++it_g)
		{
			if (it_g->second.uId>0 && it_g->second.uId<MAX_ITEM)
			{
                pUI.uId[i] = it_g->second.uId;
				i++;
			}
			//m_giveItemData.erase(it_g); //itit->second.nindex
		}
		/*for (int i = 0;i <MAX_GIVE_COUNT;i++)
		{
			t_sprintf(ngiveKey,"key_%d",i+1);
			std::string _giveKey =ngiveKey;
			if (i < m_giveNum && m_giveItemData[_giveKey].uId>0 && m_giveItemData[_giveKey].uId<MAX_ITEM)
			{
				//m_ItemBox.GetObjectById(pObj,i);
				pUI.uId[i] = m_giveItemData[_giveKey].uId;
			}
			else
			{
				pUI.uId[i] = 0;
			}

		}*/
#ifdef WIN32
		snprintf(pUI.m_Callback,sizeof(pUI.m_Callback)-1,"%s",m_Callback);
		//sprintf(pUI.m_Callback,"%s",m_Callback);  //回调函数
		snprintf(pUI.m_Error,sizeof(pUI.m_Error)-1,"%s",m_Error);
		//sprintf(pUI.m_Error,"%s",m_Error);        //错误信息
#else
		snprintf(pUI.m_Callback,sizeof(pUI.m_Callback),"%s",m_Callback);
		snprintf(pUI.m_Error,sizeof(pUI.m_Error),"%s",m_Error);
#endif
		if (m_nNeed)
		{//有数量
			if (strcmp(m_Callback, "sysqianghua") == 0)     //系统强化回调函数
				g_pCoreShell->OperationRequest(GOI_GIVE_ITEM_QIANG,(unsigned int)&pUI,m_giveNum);
			else
				g_pCoreShell->OperationRequest(GOI_GIVE_ITEM_BACK,(unsigned int)&pUI,m_giveNum);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_GIVE_ITEM_NOBACK,(unsigned int)&pUI,m_giveNum);
		}
	}
	CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

void Kuigive::btnCallBackFunc_fangzhi(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}

	if  (m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示",1).c_str());
#endif
		return;
	}
	if  (m_ItemData[nstrCurSelItemKey].uId<=0 || m_ItemData[nstrCurSelItemKey].uId>=MAX_ITEM)
	{
#ifdef WIN32
		CCMessageBox("该物品不能放置","提示");
#else
		CCMessageBox(UTEXT("该物品不能放置",1).c_str(),UTEXT("提示",1).c_str());
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
#ifdef WIN32
		CCMessageBox("放置的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("放置的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif
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
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_give); 
	//g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_immediacy); 
}

void Kuigive::btnCallBackFunc_quxia(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
	}

	if  (inCurGiveItemKey=="" || m_giveItemData.count(inCurGiveItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}
	if  (m_giveItemData[inCurGiveItemKey].uId<=0 || m_giveItemData[inCurGiveItemKey].uId>=MAX_ITEM)
	{
#ifdef WIN32
		CCMessageBox("该物品不能取下","提示:");
#else
		CCMessageBox(UTEXT("该物品不能取下",1).c_str(),UTEXT("提示:",1).c_str());
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
		//nBakChickTime = nCurChicktime;
#ifdef WIN32
		CCMessageBox("操作的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("操作的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	nBakChickTime = nCurChicktime;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_giveItemData[inCurGiveItemKey].uGenre;
	Obj.Obj.uId       = m_giveItemData[inCurGiveItemKey].uId;
	Obj.Region.h      = m_giveItemData[inCurGiveItemKey].DataX;
	Obj.Region.v      = m_giveItemData[inCurGiveItemKey].DataY;
	Obj.Region.Width  = m_giveItemData[inCurGiveItemKey].DataW;
	Obj.Region.Height = m_giveItemData[inCurGiveItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_GIVE;

	/*char msg[64];
	t_sprintf(msg,"取下:x=%d,y=%d",Obj.Region.h,Obj.Region.v);
	CCMessageBox(msg,"取下");*/
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(unsigned int)(&Obj),room_equipment);
}

//执行上层对象的回调函数，关闭弹出框
void Kuigive::buttonCallBackFunc(CCObject * pSender)
{

	if (g_pCoreShell)
	{
		KUiDraggedObject pObj;
		PLAYER_GIVEUI pUI;
		ZeroMemory(&pUI,sizeof(pUI));
		int i=0;
		_giveItemData::iterator it_q;
		for(it_q = m_giveItemData.begin(); it_q != m_giveItemData.end(); ++it_q)
		{
			if (it_q->second.uId>0 && it_q->second.uId<MAX_ITEM)
			{
				pUI.uId[i] = it_q->second.uId;
				i++;
			}
			//m_giveItemData.erase(it_g); //itit->second.nindex
		}
		//unsigned int uId[32];
		/*char ngiveKey[32];
		ZeroMemory(&ngiveKey,sizeof(ngiveKey));
		for (int i = 0;i < MAX_GIVE_COUNT;i++)
		{
			t_sprintf(ngiveKey,"key_%d",i+1);
			std::string _giveKey =ngiveKey;
			if (i < m_giveNum && m_giveItemData[_giveKey].uId>0 && m_giveItemData[_giveKey].uId<MAX_ITEM)
			{
				//m_ItemBox.GetObjectById(pObj,i);
				pUI.uId[i] = m_giveItemData[_giveKey].uId;
				//uId[i] = pObj.uId;
			}
			else
			{
				//uId[i] = 0;
				pUI.uId[i] = 0;
			}
		}*/

		t_sprintf(pUI.m_Callback,"%s",m_Callback);  //回调函数
		t_sprintf(pUI.m_Error,"%s",m_Error);        //错误信息

		if  (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_GIVE_ITEM_FAIL,(unsigned int)(&pUI),m_giveNum);
	}

	CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void Kuigive::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void Kuigive::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-128, true);
}

void Kuigive::closeItemPadCallback(CCNode *pNode)
{//在这个层中标记号，以确定是哪个按钮
	m_descPadIsOpen = false;
}

CCRect Kuigive::getGiveItem(CCNode* pNode,int i)
{
		/*CCRect  rc;
		rc.origin     = pNode->getPosition(); //这个因该是图内偏移
		rc.size       = pNode->getContentSize();
		rc.origin.x   = m_origin.x+ m_size.width-30+ rc.origin.x; //起点最左边
		rc.origin.y   = m_origin.y+ rc.origin.y + 60; //最上边    //- rc.origin.y
		*/
	CCRect  rc;
	rc.origin     = pNode->getPosition();                      //这个因该是图内偏移
	rc.size       = pNode->getContentSize();                   //这个物品的宽度和高度
	//rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	//rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	rc.origin.x   = m_origin.x+ m_size.width-30+rc.origin.x;      //起点最左边 + m_size.width-30
	rc.origin.y   = m_origin.y+ rc.origin.y + 60; //最上边    //- rc.origin.y
	return rc;
}


CCRect Kuigive::getRectItem(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	rc.size       = pNode->getContentSize();
	rc.origin.x   = m_origin.x+ m_size.width-30+ rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y+ rc.origin.y + 60; //最上边    //- rc.origin.y
	
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool Kuigive::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
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
			nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				if (getRectItem(nCurColorlayer,0).containsPoint(location))
				{//点中了
					if (SkillNode)
					{
						inCurGiveItemKey  ="";
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						m_ItemData[nItemKey].nRect  = getRectItem(nCurColorlayer,0);
						m_ItemData[nItemKey].nindex = -1;
						CCLayer * sDescLayer = KuiItemdesc::create(it->second,this,callfuncN_selector(Kuigive::closeItemPadCallback),1);
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
						inCurGiveItemKey  ="";
						//m_CurSelItemIndex = -1;
					}
					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
		}
	}
	//give点击
	isthis = false;
	_giveItemData::iterator its;
	for( its = m_giveItemData.begin(); its != m_giveItemData.end(); ++its)
	{
		if (its->second.uId>0 && its->second.uId<MAX_ITEM)
		{////重置某技能的加成
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[its->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_Give->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[its->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Give->getChildByTagKey(nItemKey);
			if  (nCurColorlayer)
			{
				//CCMessageBox("开始点击","开始点击");
				if (getGiveItem(nCurColorlayer,0).containsPoint(location))
				{//点中了
					//CCMessageBox("点中了","点中了");
					if (SkillNode)
					{
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED);
						its->second.nRect  = getGiveItem(nCurColorlayer,0);
						/*char ngiveKey[32];
						ZeroMemory(&ngiveKey,sizeof(ngiveKey));
						t_sprintf(ngiveKey,"key_%d",its->second.nindex);
						std::string _giveKey =ngiveKey;*/
						inCurGiveItemKey     =nItemKey;

						CCLayer * sDescLayer = KuiItemdesc::create(its->second,this,callfuncN_selector(Kuigive::closeItemPadCallback),1);
						m_descPadIsOpen = true;
						/*char msg[64];
						t_sprintf(msg,"位置:x=%d,y=%d",its->second.DataX,its->second.DataY);
						CCMessageBox(msg,"test");*/
						//m_CurSelItemIndex = -1;//m_ItemData[i].uId;
						//nstrCurSelItemKey =nItemKey;

						isthis = true;
						this->addChild(sDescLayer,2,1001);
					}
				}
				else
				{
					if (!isthis) //点中了,就不归零
					{
						inCurGiveItemKey ="";
					}
					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
		}
	}
	return true;
}

void Kuigive::UpdateGiveItem(KUiObjAtRegion* pItem, int bAdd)
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
		{
			CCMessageBox("itemdata is error","itemgive");
			return;
		}

		if (bAdd) //增加
		{
			//m_ItemBox.AddObject(&Obj,1);
			if (pItem->Obj.uId)
				m_giveNum++;

			AddGiveObject(&Obj,m_giveNum);

		}
		else      //减少
		{
			//CCMessageBox("itemdata is del","itemgive_u");
			if (pItem->Obj.uId)
				m_giveNum--;
			if  (m_giveNum<0)
				m_giveNum=0;

			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
			t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
			nItemKey = nKey;
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
			m_giveItemData.erase(nItemKey); 
			/*_giveItemData::iterator it_g;
			for(it_g = m_giveItemData.begin(); it_g != m_giveItemData.end(); ++it_g)
			{
				if (it_g->second.uId>0 && it_g->second.uId<MAX_ITEM && it_g->second.uId ==Obj.uId)
				{
					it_g->second.uId = 0;
					it_g->second.nindex=0;
				}
					//m_giveItemData.erase(it_g); //itit->second.nindex
			}*/
		}
	}
	else
      CCMessageBox("itemdata is error","itemgive_u");
	//	UpdateGiveItemData();
}


void Kuigive::UpdateGiveItemData()
{
	inCurGiveItemKey ="";
	// memset(m_ItemData,0,sizeof(KuiSellItemdObject) * 60);
	_giveItemData::iterator it;
	for( it = m_giveItemData.begin(); it != m_giveItemData.end(); it++)
	{
		if (it->second.uId>0 && it->second.uId<MAX_ITEM)
		{
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
			//nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			//SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			ParentNode_Give->removeChildByTagKey(nItemKey,true);
		}
	}
	m_giveItemData.clear();

	KUiObjAtRegion* pObjs = NULL;
	int nCount    = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_ngiveItemNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; i++)
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
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Give->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Give->addChild(nItemSpr,1);

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
					/*char ngiveKey[32];
					ZeroMemory(&ngiveKey,sizeof(ngiveKey));
					t_sprintf(ngiveKey,"key_%d",i+1);
					std::string _giveKey =ngiveKey;*/
					m_giveItemData[nKey].uGenre = pObjs[i].Obj.uGenre;
					m_giveItemData[nKey].uId    = pObjs[i].Obj.uId;
					m_giveItemData[nKey].nindex = i+1;
					m_giveItemData[nKey].DataX  = pObjs[i].Region.h;     //包袱中的起点X位置 第几个格子 25x25
					m_giveItemData[nKey].DataY  = pObjs[i].Region.v;     //包袱中的起点Y位置
					m_giveItemData[nKey].DataW  = pObjs[i].Region.Width; //物品的高度(占用了多少个格子)
					m_giveItemData[nKey].DataH  = pObjs[i].Region.Height;//物品的宽度(占用了多少个格子)
					m_giveItemData[nKey].nGenkind = UOC_ITEM_GIVE;  

					ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Give->getChildByTagKey(nKey);
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
						ParentNode_Give->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_GIVE;

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

void Kuigive::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
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
		{
			//CCMessageBox("数据有误","物品");
			return;
		}

		if (bAdd) //增加
		{
			//m_ItemBox.AddObject(&Obj,1);
			if (pItem->Obj.uId)
				m_itemNum ++;

			AddObject(&Obj,1);

		}
		else      //减少
		{
			if (pItem->Obj.uId)
				m_itemNum--;

			if  (m_itemNum<0)
				m_itemNum=0;

			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[Obj.uId].GetID());
			std::string nItemKey = nKey;
			ParentNode_Item->removeChildByTagKey(nItemKey,true);
			t_sprintf(nKey,"item_%u",Item[Obj.uId].GetID());
			nItemKey = nKey;
			ParentNode_Item->removeChildByTagKey(nItemKey,true);
			m_ItemData.erase(nItemKey);
		}
		//m_ItemBox.RemoveObject(&Obj);
		//UiSoundPlay(UI_SI_PICKPUT_ITEM);
		//UpdateData();
	}
	else //更新全部的物品
		UpdateItemData();
}

void Kuigive::UpdateItemData()
{
// m_CurSelItemIndex = -1;
   nstrCurSelItemKey ="";
  // memset(m_ItemData,0,sizeof(KuiSellItemdObject) * 60);
   _ItemData::iterator it;
   for( it = m_ItemData.begin(); it != m_ItemData.end(); it++)
   {
	   if (it->second.uId>0 && it->second.uId<MAX_ITEM)
	   {////重置某技能的加成
		   char nKey[32];
		   t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
		   std::string nItemKey = nKey;
		   ParentNode_Item->removeChildByTagKey(nItemKey,true);
		   //nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
		   t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
		   nItemKey = nKey;
		   //SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
		   ParentNode_Item->removeChildByTagKey(nItemKey,true);
	   }
   }
   m_ItemData.clear();

	KUiObjAtRegion* pObjs = NULL;
	int nCount    = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nItemNumObjects = nCount;

	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
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
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

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
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
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
						ParentNode_Item->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

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

void Kuigive::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
		char nPointCount[32];
		m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
		if (pMoneyLabel)
		{
			t_sprintf(nPointCount,"%d",m_nMoney);
			pMoneyLabel->setString(nPointCount);
		}
		if  (pXuLabel)
		{
			t_sprintf(nPointCount,"%d",m_nXu);
			pXuLabel->setString(nPointCount);
		}
		//扩展箱开关检测
	   if (g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, GET_BOXEX_OPEN, 0))
		       m_nIstrue = true;
		   //	m_ItemEX.SetOpenClose(TRUE);
		   else
			   m_nIstrue = false;

	  //更新叠加数量
		   _ItemData::iterator it;
		   for( it = m_ItemData.begin(); it != m_ItemData.end(); ++it)
		   {
			   if (it->second.uId>0 && it->second.uId<MAX_ITEM)
			   {
				   if  (Item[it->second.uId].GetGenre()==0)
					   continue;

				   char nItemKey[32];
				   t_sprintf(nItemKey,"item_%u",Item[it->second.uId].GetID());
				   std::string nKey =nItemKey;
				   CCSprite *nTempSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
				   if  (nTempSpr)
				   {
					   CCLabelTTF *stuckCountlabel = (CCLabelTTF *)nTempSpr->getChildByTagKey("stack");
					   if  (stuckCountlabel)
					   {
						   char nCount[32];
						   t_sprintf(nCount,"%d",Item[it->second.uId].GetStackNum());
						   stuckCountlabel->setString(nCount);
					   }
				   }
			   }
		   }
	}
}

void Kuigive::draw()
{
	//CCMessageBox("无限循环","draw");
}

int Kuigive::AddGiveObject(KUiDraggedObject* pObject, int nindex)
{
	if (!pObject) return false;
	for (int i = 0; i < 1; ++i)
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
				CCSprite *nItemSpr = (CCSprite *)ParentNode_Give->getChildByTagKey(nKey);
				if (!nItemSpr)
				{
					nItemSpr = CCSprite::createWithTexture(bgCur);
					//nItemSpr->setAnchorPoint(ccp(0,0));
					nItemSpr->setTagbyKey(nKey);
					ParentNode_Give->addChild(nItemSpr,1);

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
				/*char ngiveKey[32];
				ZeroMemory(&ngiveKey,sizeof(ngiveKey));
				t_sprintf(ngiveKey,"key_%d",nindex);
				std::string _giveKey =ngiveKey;*/
				m_giveItemData[nKey].uGenre = pObject->uGenre;
				m_giveItemData[nKey].uId    = pObject->uId;
				m_giveItemData[nKey].nindex = nindex;
				m_giveItemData[nKey].DataX  = pObject->DataX;     //包袱中的起点X位置 第几个格子 25x25
				m_giveItemData[nKey].DataY  = pObject->DataY;     //包袱中的起点Y位置
				m_giveItemData[nKey].DataW  = pObject->DataW; //物品的高度(占用了多少个格子)
				m_giveItemData[nKey].DataH  = pObject->DataH;//物品的宽度(占用了多少个格子)
				m_giveItemData[nKey].nGenkind = UOC_ITEM_GIVE;  //随身携带

				ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
				//t_sprintf(nItemKey,"color_%d",i+1);
				strcat(nItemKey,"_color");
				nKey =nItemKey;
				CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Give->getChildByTagKey(nKey);
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
					ParentNode_Give->addChild(bgcolorLayer,0);
				}

				int nCurX = m_StartPos_g.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
				int nCurY = m_StartPos_g.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;

				nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
				//颜色层
				nCurX = m_StartPos_g.x+no.DataX*ITEM_CELL_SIZE+1;
				nCurY = m_StartPos_g.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE-1;
				bgcolorLayer->setPosition(ccp(nCurX,m_size.height-nCurY));

				KUiObjAtContRegion	Obj;
				Obj.Obj.uGenre = no.uGenre;
				Obj.Obj.uId    = no.uId;
				Obj.Region.h   = Obj.Region.v = 0;
				Obj.Region.Width = Obj.Region.Height = 0;
				Obj.nContainer = UOC_ITEM_GIVE;

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
//增加物品
int Kuigive::AddObject(KUiDraggedObject* pObject, int nCount)
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
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					if (!nItemSpr)
					{
						nItemSpr = CCSprite::createWithTexture(bgCur);
						//nItemSpr->setAnchorPoint(ccp(0,0));
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);

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
					m_ItemData[nKey].nGenkind = UOC_ITEM_TAKE_WITH;  //随身携带

					ccColor4B color = ccc4(112, 128, 144, 150);//112, 128, 144
					//t_sprintf(nItemKey,"color_%d",i+1);
					strcat(nItemKey,"_color");
					nKey =nItemKey;
					CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
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
						ParentNode_Item->addChild(bgcolorLayer,0);
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
					Obj.nContainer = UOC_ITEM_TAKE_WITH;

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

int Kuigive::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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
