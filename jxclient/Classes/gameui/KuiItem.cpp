#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "engine/CXmlStream.h"
#include "KuiItem.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
//#include "engine/KPalette.h"
#include "engine/KCodec.h"
#include "engine/KCodecLzo.h"
#include "../engine/KFile.h"
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
#include "gamecore/KPlayer.h"
#include "gameui/KuiItemdesc.h"
#include "gameui/KuigetMoney.h"
#include "engine/Text.h"
extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiItem::KuiItem()
{
	__isCanMove  = true;
	m_bScrolling = false;
	isOpen = false;
//	memset(m_ItemData,0,sizeof(KUiItemdObject) * 60);
	pMoneyLabel       = NULL;
	pXuLabel          = NULL;
	colorLayer        = NULL;
	//red               = NULL;
	m_ItemData.clear();
	m_nMoney = 0;
	m_nXu = 0;
	m_nNumObjects = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	nBakChickTime=0;
	m_descPadIsOpen = false;
	m_EqdescPadIsOpen = false;
	m_TempPadIsOpen=false;
	savemenu = NULL;
	nstrCurSelItemKey ="";
	m_MovePosX  = 0;
	m_nIstrue   =false;
}

KuiItem::~KuiItem()
{
	isOpen = false;
	pMoneyLabel  = NULL;
	colorLayer        = NULL;
	m_ItemData.clear();
	//if (red)
	//   red->release();
	m_descPadIsOpen = false;
	m_EqdescPadIsOpen = false;
	m_savemoneyPadIsOpen = false;
	m_TempPadIsOpen=false;
}

//创建确定弹出框
KuiItem * KuiItem::create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiItem * popLayer = KuiItem::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiItem::init()
{
	if ( !CCLayer::init() ){
		return false;
	}

	winSize = CCDirector::sharedDirector()->getWinSize();
	
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	CCSprite * spriteBox = CCSprite::create("ui/item.png");
	CCSprite * spriteBoxMove = CCSprite::create("ui/itembg.png");
	spriteBox->setContentSize(CCSize(335,500));
	spriteBoxMove->setContentSize(CCSize(ITEM_CELL_SIZE,ITEM_CELL_SIZE));
	ParentNode_Item = spriteBox;
	ParentNode_ItemMove = spriteBoxMove;
	spriteBox->setPosition(ccp(winSize.width/2+330,winSize.height/2));
	this->addChild(spriteBoxMove);
	this->addChild(spriteBox);
	
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();      //背景的相对整个屏幕的位置
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void KuiItem::addDialogData()
{
	if  (!g_pCoreShell) return;

	//Left=24
	//Top=72
	m_StartPos.x =31;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =53;//+ITEM_CELL_SIZE/2;

    pMoneyLabel = CCLabelTTF::create("",UI_GAME_FONT_VN, 12);//Arial
	pMoneyLabel->setAnchorPoint(ccp(0,0));
	pMoneyLabel->setPosition(ccp(65,70));
	pMoneyLabel->enableStroke(ccc3(0,0,0),5);
	ParentNode_Item->addChild(pMoneyLabel,1);

	pXuLabel = CCLabelTTF::create("",UI_GAME_FONT_VN, 12);//Arial
	pXuLabel->setAnchorPoint(ccp(0,0));
	pXuLabel->setPosition(ccp(65,50));
	ParentNode_Item->addChild(pXuLabel,1);
	

	KUiObjAtRegion* pObjs = NULL;
	int nCount   = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nNumObjects = nCount;

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
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);
						ParentNode_ItemMove->addChild(nItemSpr,1);

						// if (Item[pObjs[i].Obj.uId].GetGenre()==item_equip){
						// 	nItemSpr->setScale(0.8);
						// }
						//Kientm item 1 o vuong nen can giam scale cua spr item
						if (Item[pObjs[i].Obj.uId].GetDetailType()==equip_armor || Item[pObjs[i].Obj.uId].GetDetailType()==equip_amulet){
							nItemSpr->setScale(0.7);
						}
						if (Item[pObjs[i].Obj.uId].GetDetailType()==equip_horse){
							nItemSpr->setScale(0.6);
						}
						if (Item[pObjs[i].Obj.uId].GetGenre()==item_medicine){
							nItemSpr->setScale(1);
						}
						if (Item[pObjs[i].Obj.uId].GetDetailType()==equip_meleeweapon || Item[pObjs[i].Obj.uId].GetDetailType()==equip_rangeweapon){
							nItemSpr->setScale(0.65);
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

					ccColor4B color = ccc4(0, 56, 33, 150); //112, 128, 144
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
						if  (Item[pObjs[i].Obj.uId].GetGenre()!=item_equip && Item[pObjs[i].Obj.uId].IsStack())
						{//item xep chong
							char stack[32];
							t_sprintf(stack,"%d",Item[pObjs[i].Obj.uId].GetStackNum());
							CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_VN,14);
							stuckCountlabel->setColor(ccYELLOW);
							stuckCountlabel->setAnchorPoint(ccp(0,0));
							stuckCountlabel->enableStroke(ccc3(0,0,0),5);
							stuckCountlabel->setPosition(ccp(3,0));
							stuckCountlabel->setTagbyKey("stack");
							bgcolorLayer->addChild(stuckCountlabel,1);
						}
						ParentNode_Item->addChild(bgcolorLayer,0);
					}
					
					int nCurX = m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2;
					int nCurY = m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2;
					
					switch (no.DataX)
					{
					case 0:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+2;

						break;
					case 1:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+8;
						break;	
					case 2:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+14;
						break;
					case 3:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+20;
						break;
					case 4:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+26;
						break;		
					default:
						break;
					}

					switch (no.DataY)
					{
					case 0:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+2;
						break;
					case 1:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+8;
						break;	
					case 2:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+14;
						break;
					case 3:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+20;
						break;
					case 4:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+26;
						break;
					case 5:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+32;
						break;			
					default:
						break;
					}
					nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
					//颜色层
					switch (no.DataX)
					{
					case 0:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+2;
						break;
					case 1:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+8;
						break;	
					case 2:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+14;
						break;
					case 3:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+20;
						break;
					case 4:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+26;
						break;		
					default:
						break;
					}

					switch (no.DataY)
					{
					case 0:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+2;
						break;
					case 1:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+8;
						break;	
					case 2:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+14;
						break;
					case 3:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+20;
						break;
					case 4:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+26;
						break;
					case 5:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+32;
						break;			
					default:
						break;
					}
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
void KuiItem::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

	
	CCPoint bgSpritePoint = ParentNode_Item->getPosition();

	char nSprName[128]={0};
	int m_nWidth,m_nHeight,nFrams;
	CCTexture2D *bgCur = NULL;
	SPRFRAMSINFO nSprInfo;
	ZeroMemory(&nSprInfo,sizeof(nSprInfo));
	CCPoint nStartPos;
	nStartPos.x = m_size.width/2+20;
	nStartPos.y = m_size.height/2;
	//dong
	CCMenuItemImage * closeConfirm = CCMenuItemImage::create("", "", this, menu_selector(KuiItem::buttonCallBackFunc));
	closeConfirm->setContentSize(CCSize(125,30));
	closeConfirm->setPosition(ccp(235,-128));
	//sap xep 
	// CCMenuItemImage * zhengliConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::zhengCallBackFunc));
	// zhengliConfirm->setPosition(ccp(0,25));
	// CCLabelTTF * zhengliConfirmLabel = CCLabelTTF::create("Sắp xếp",UI_GAME_FONT_VN, 11);
	// zhengliConfirmLabel->setColor(ccWHITE);
	// zhengliConfirmLabel->setAnchorPoint(ccp(0,0));
	// zhengliConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// zhengliConfirmLabel->setPosition(ccp(zhengliConfirm->getContentSize().width/2-20,zhengliConfirm->getContentSize().height/2-7));
	// zhengliConfirm->addChild(zhengliConfirmLabel,1);
	//them vao thanh phim tat
	// CCMenuItemImage * kuaiConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::kuaiCallBackFunc));
	// kuaiConfirm->setPosition(ccp(0,50));
	// CCLabelTTF * kuaiConfirmLabel = CCLabelTTF::create("Phím tắt",UI_GAME_FONT_VN, 11);
	// kuaiConfirmLabel->setColor(ccWHITE);
	// kuaiConfirmLabel->setAnchorPoint(ccp(0,0));
	// kuaiConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// kuaiConfirmLabel->setPosition(ccp(kuaiConfirm->getContentSize().width/2-20,kuaiConfirm->getContentSize().height/2-7));
	// kuaiConfirm->addChild(kuaiConfirmLabel,1);

	//用
	// CCMenuItemImage * itemchuanConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::chuanCallBackFunc));
	// itemchuanConfirm->setPosition(ccp(0,75));
	// CCLabelTTF * itemchuanConfirmLabel = CCLabelTTF::create("Dùng",UI_GAME_FONT_VN, 11);
	// itemchuanConfirmLabel->setColor(ccWHITE);
	// itemchuanConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemchuanConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemchuanConfirmLabel->setPosition(ccp(itemchuanConfirm->getContentSize().width/2-20,itemchuanConfirm->getContentSize().height/2-7));
	// itemchuanConfirm->addChild(itemchuanConfirmLabel,1);

	//拆
	// CCMenuItemImage * itemchaiConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::chaiCallBackFunc));
	// itemchaiConfirm->setPosition(ccp(0,100));
	// CCLabelTTF * itemchaiConfirmLabel = CCLabelTTF::create("Tách",UI_GAME_FONT_VN, 11);
	// itemchaiConfirmLabel->setColor(ccWHITE);
	// itemchaiConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemchaiConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemchaiConfirmLabel->setPosition(ccp(itemchaiConfirm->getContentSize().width/2-20,itemchaiConfirm->getContentSize().height/2-7));
	// itemchaiConfirm->addChild(itemchaiConfirmLabel,1);
	//丢
	// CCMenuItemImage * itemdiuConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::diuCallBackFunc));
	// itemdiuConfirm->setPosition(ccp(0,125));
	// CCLabelTTF * itemdiuConfirmLabel = CCLabelTTF::create("Vứt",UI_GAME_FONT_VN, 11);
	// itemdiuConfirmLabel->setColor(ccWHITE);
	// itemdiuConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemdiuConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemdiuConfirmLabel->setPosition(ccp(itemdiuConfirm->getContentSize().width/2-20,itemdiuConfirm->getContentSize().height/2-7));
	// itemdiuConfirm->addChild(itemdiuConfirmLabel,1);
	//修理
	// CCMenuItemImage * itemxiuConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::xiuCallBackFunc));
	// itemxiuConfirm->setPosition(ccp(0,150));
	// CCLabelTTF * itemxiuConfirmLabel = CCLabelTTF::create("Sửa",UI_GAME_FONT_VN, 11);
	// itemxiuConfirmLabel->setColor(ccWHITE);
	// itemxiuConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemxiuConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemxiuConfirmLabel->setPosition(ccp(itemxiuConfirm->getContentSize().width/2-20,itemxiuConfirm->getContentSize().height/2-7));
	// itemxiuConfirm->addChild(itemxiuConfirmLabel,1);
	//卖
	// CCMenuItemImage * itemmaiConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::maiCallBackFunc));
	// itemmaiConfirm->setPosition(ccp(0,175));
	// CCLabelTTF * itemmaiConfirmLabel = CCLabelTTF::create("Bán",UI_GAME_FONT_VN, 11);
	// itemmaiConfirmLabel->setColor(ccWHITE);
	// itemmaiConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemmaiConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemmaiConfirmLabel->setPosition(ccp(itemmaiConfirm->getContentSize().width/2-20,itemmaiConfirm->getContentSize().height/2-7));
	// itemmaiConfirm->addChild(itemmaiConfirmLabel,1);
	//存
	// CCMenuItemSprite * itemcunConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::cunCallBackFunc));
	// itemcunConfirm->setPosition(ccp(0,200));
	// CCLabelTTF * itemcunConfirmLabel = CCLabelTTF::create("Rương",UI_GAME_FONT_VN, 11);
	// itemcunConfirmLabel->setColor(ccWHITE);
	// itemcunConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemcunConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemcunConfirmLabel->setPosition(ccp(itemcunConfirm->getContentSize().width/2-20,itemcunConfirm->getContentSize().height/2-7));
	// itemcunConfirm->addChild(itemcunConfirmLabel,1);

	//链接
	// CCMenuItemImage * itemLianConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::lianCallBackFunc));
	// itemLianConfirm->setPosition(ccp(0,225));
	// CCLabelTTF * itemLianConfirmLabel = CCLabelTTF::create("Rao",UI_GAME_FONT_VN, 11);
	// itemLianConfirmLabel->setColor(ccWHITE);
	// itemLianConfirmLabel->setAnchorPoint(ccp(0,0));
	// itemLianConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// itemLianConfirmLabel->setPosition(ccp(itemLianConfirm->getContentSize().width/2-20,itemLianConfirm->getContentSize().height/2-7));
	// itemLianConfirm->addChild(itemLianConfirmLabel,1);
	//存钱
	// CCMenuItemImage * saveConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::saveCallBackFunc));
	// saveConfirm->setPosition(ccp(0,250));
	// CCLabelTTF * saveConfirmLabel = CCLabelTTF::create("Gửi",UI_GAME_FONT_VN, 11);
	// saveConfirmLabel->setColor(ccWHITE);
	// saveConfirmLabel->setAnchorPoint(ccp(0,0));
	// saveConfirmLabel->enableStroke(ccc3(0,0,0),6);
	// saveConfirmLabel->setPosition(ccp(saveConfirm->getContentSize().width/2-20,saveConfirm->getContentSize().height/2-7));
	// saveConfirm->addChild(saveConfirmLabel,1);
	//CCMenu * menu = CCMenu::create(closeConfirm,itemchuanConfirm,itemdiuConfirm,itemxiuConfirm,itemmaiConfirm,itemchaiConfirm,itemcunConfirm,saveConfirm,kuaiConfirm,zhengliConfirm,itemLianConfirm,NULL);
	CCMenu * menu = CCMenu::create(closeConfirm,NULL);
	menu->setPosition(ccp(-65,150));//V? trí menu
	ParentNode_Item->addChild(menu);         //thêm vào node ParentNode_Item
	menu->setVisible(true);//
	 

	//储
	CCMenuItemImage * itemchuConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::chuCallBackFunc));
	itemchuConfirm->setPosition(ccp(100,0));
	CCLabelTTF * itemchuConfirmLabel = CCLabelTTF::create("Cửa hàng",UI_GAME_FONT_VN, 11);
	itemchuConfirmLabel->setColor(ccWHITE);
	itemchuConfirmLabel->setAnchorPoint(ccp(0,0));
	itemchuConfirmLabel->enableStroke(ccc3(0,0,0),6);
	itemchuConfirmLabel->setPosition(ccp(closeConfirm->getContentSize().width/2-20,closeConfirm->getContentSize().height/2-7));
	itemchuConfirm->addChild(itemchuConfirmLabel,1);
	//子
	CCMenuItemImage * itemziConfirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::ziCallBackFunc));
	itemziConfirm->setPosition(ccp(100,25));
	CCLabelTTF * itemziConfirmLabel = CCLabelTTF::create("Button 2",UI_GAME_FONT_VN, 11);
	itemziConfirmLabel->setColor(ccWHITE);
	itemziConfirmLabel->setAnchorPoint(ccp(0,0));
	itemziConfirmLabel->enableStroke(ccc3(0,0,0),6);
	itemziConfirmLabel->setPosition(ccp(closeConfirm->getContentSize().width/2-20,closeConfirm->getContentSize().height/2-7));
	itemziConfirm->addChild(itemziConfirmLabel,1);
	
	//扩1
	CCMenuItemImage * itemkuo1Confirm =  CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::kuo1CallBackFunc));
	itemkuo1Confirm->setPosition(ccp(100,50));
	CCLabelTTF * itemkuo1ConfirmLabel = CCLabelTTF::create("Rương 1",UI_GAME_FONT_VN, 11);
	itemkuo1ConfirmLabel->setColor(ccWHITE);
	itemkuo1ConfirmLabel->setAnchorPoint(ccp(0,0));
	itemkuo1ConfirmLabel->enableStroke(ccc3(0,0,0),6);
	itemkuo1ConfirmLabel->setPosition(ccp(closeConfirm->getContentSize().width/2-20,closeConfirm->getContentSize().height/2-7));
	itemkuo1Confirm->addChild(itemkuo1ConfirmLabel,1);

	//扩2
	CCMenuItemImage * itemkuo2Confirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::kuo2CallBackFunc));
	itemkuo2Confirm->setPosition(ccp(100,75));
	CCLabelTTF * itemkuo2ConfirmLabel = CCLabelTTF::create("Rương2",UI_GAME_FONT_VN, 11);
	itemkuo2ConfirmLabel->setColor(ccWHITE);
	itemkuo2ConfirmLabel->setAnchorPoint(ccp(0,0));
	itemkuo2ConfirmLabel->enableStroke(ccc3(0,0,0),6);
	itemkuo2ConfirmLabel->setPosition(ccp(closeConfirm->getContentSize().width/2-20,closeConfirm->getContentSize().height/2-7));
	itemkuo2Confirm->addChild(itemkuo2ConfirmLabel,1);

	//扩3
	CCMenuItemImage * itemkuo3Confirm = CCMenuItemImage::create("ui/textbg_1.png", "ui/textbg_2.png", this, menu_selector(KuiItem::kuo3CallBackFunc));
	itemkuo3Confirm->setPosition(ccp(100,75));
	CCLabelTTF * itemkuo3ConfirmLabel = CCLabelTTF::create("Rương 3",UI_GAME_FONT_VN, 11);
	itemkuo3ConfirmLabel->setColor(ccWHITE);
	itemkuo3ConfirmLabel->setAnchorPoint(ccp(0,0));
	itemkuo3ConfirmLabel->enableStroke(ccc3(0,0,0),6);
	itemkuo3ConfirmLabel->setPosition(ccp(closeConfirm->getContentSize().width/2-20,closeConfirm->getContentSize().height/2-7));
	itemkuo3Confirm->addChild(itemkuo3ConfirmLabel,1);

	savemenu = CCMenu::create(itemchuConfirm,itemziConfirm,itemkuo1Confirm,itemkuo2Confirm,itemkuo3Confirm,NULL);
	savemenu->setPosition(ccp(300,150));
	ParentNode_Item->addChild(savemenu);         
	savemenu->setVisible(false);//
}
//存钱
void KuiItem::saveCallBackFunc(CCObject * pSender)
{
	if (m_savemoneyPadIsOpen)
		this->removeChildByTag(1003,true);
	//1为存钱　０　为取钱
	CCLayer * savemonyLayer = KuigetMoney::create(0,m_nMoney,1,this,callfuncN_selector(KuiItem::closesavemonyPadCallback));
	m_savemoneyPadIsOpen = true;
	this->addChild(savemonyLayer,2,1003);
}
//取钱回调
void KuiItem::closesavemonyPadCallback(CCNode *pNode)
{
	//int nMaxMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_repository);
	m_savemoneyPadIsOpen = false;
}
//使用
void KuiItem::chuanCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}

	if  (m_ItemData.count(nstrCurSelItemKey)<=0)
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Vui lòng chọn vật phẩm");
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
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Bạn thao tác quá nhanh");
		return;
	}
	nBakChickTime = nCurChicktime;


	if  (m_ItemData[nstrCurSelItemKey].uId>0 && m_ItemData[nstrCurSelItemKey].uId<MAX_ITEM)
	{
		if (Item[m_ItemData[nstrCurSelItemKey].uId].GetGenre()==item_equip && g_pCoreShell->GetAutoplayid())
		{
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Vui lòng tắt chức năng tự động đánh trước");
			return;
		}
	}
	else
		return;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	if  (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_USE_ITEM,(unsigned int)(&Obj),UOC_ITEM_TAKE_WITH);
}

void KuiItem::diuCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}
	/*if (m_CurSelItemIndex<0)
	{
		CCMessageBox("请选择一个物品","提示:");
		return;
	}*/
	if  (nstrCurSelItemKey=="" || m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	if  (Item[m_ItemData[nstrCurSelItemKey].uId].GetTime()->bYear>0 || Item[m_ItemData[nstrCurSelItemKey].uId].GetIsBang())
	{
		nstrCurSelItemKey="";
#ifdef WIN32
		CCMessageBox("限时/邦定物品禁止丢弃","提示:");
#else
		CCMessageBox(UTEXT("限时/邦定物品禁止丢弃",1).c_str(),UTEXT("提示:",1).c_str());
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
		return;
	}
	nBakChickTime = nCurChicktime;
	
	bool isYes = false;
	if (g_pCoreShell && g_pCoreShell->GetAutoplayid())
		isYes = true;
	Player[CLIENT_PLAYER_INDEX].AutoAwayItem(Item[m_ItemData[nstrCurSelItemKey].uId].GetID(),isYes);
	nstrCurSelItemKey="";
	//char msg[64];
	//t_sprintf(msg,"丢弃:%d",m_CurSelItemIndex);
	//CCMessageBox(msg,"提示");
}


void KuiItem::lianCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}
	/*if (m_CurSelItemIndex<0)
	{
		CCMessageBox("请选择一个物品","提示:");
		return;
	}*/
	if  (nstrCurSelItemKey=="" || m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	if  (Item[m_ItemData[nstrCurSelItemKey].uId].GetTime()->bYear>0 || Item[m_ItemData[nstrCurSelItemKey].uId].GetIsBang())
	{
		nstrCurSelItemKey="";
#ifdef WIN32
		CCMessageBox("限时/邦定物品禁止出售","提示:");
#else
		CCMessageBox(UTEXT("限时/邦定物品禁止出售",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	if  (g_GameWorld)
	{//开始输入聊天信息框
		char nTempStrVer[80];
		ZeroMemory(nTempStrVer,sizeof(nTempStrVer));
		t_sprintf(nTempStrVer,"<%s>",Item[m_ItemData[nstrCurSelItemKey].uId].GetName());
		g_GameWorld->setInputMsg(nTempStrVer);
		Player[CLIENT_PLAYER_INDEX].m_ItemLinkDwid = Item[m_ItemData[nstrCurSelItemKey].uId].GetID();
	}

}

void KuiItem::cunCallBackFunc(CCObject * pSender)
{
	if (savemenu)
	{
		if (savemenu->isVisible())
			savemenu->setVisible(false);//隐藏
		else
			savemenu->setVisible(true);//显示
	}
	
}

void KuiItem::xiuCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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

	if  (m_ItemData[nstrCurSelItemKey].uId>0 && m_ItemData[nstrCurSelItemKey].uId<MAX_ITEM)
	{
		if (Item[m_ItemData[nstrCurSelItemKey].uId].GetGenre()!=item_equip)
		{
#ifdef WIN32
			CCMessageBox("该物品不能修理!","提示:");
#else
			CCMessageBox(UTEXT("该物品不能修理!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			return;
		}
	}
	else
		return;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;

	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,(unsigned int)(&Obj),0);
}
void KuiItem::maiCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}


	if  (nstrCurSelItemKey=="" || m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品!","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}
	/*KUiPlayerGameTime	pInfo;
	memset(&pInfo, 0, sizeof(KUiPlayerGameTime));
	ZeroMemory(&pInfo,sizeof(pInfo));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_TIMEINFO, (int)&pInfo, 0);
	unsigned int nCurChicktime = pInfo.npTime;  //记录打开窗口的秒数
	if (nBakChickTime>0 && nCurChicktime-nBakChickTime<=2)
	{//间隔２秒
		return;
	}
	nBakChickTime = nCurChicktime;
	*/
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;
	nstrCurSelItemKey="";
	if (g_pCoreShell)
		//g_pCoreShell->OperationRequest(GOI_TRADE_PLAYER_BUY,(unsigned int)(&Obj), m_uId);
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,(unsigned int)(&Obj),0);
}
void KuiItem::chaiCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}


	if  (m_ItemData.count(nstrCurSelItemKey)<=0)
	{
#ifdef WIN32
		CCMessageBox("请选择一个物品","提示:");
#else
		CCMessageBox(UTEXT("请选择一个物品",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		return;
	}

	if  (m_ItemData[nstrCurSelItemKey].uId>0 && m_ItemData[nstrCurSelItemKey].uId<MAX_ITEM)
	{
		if (Item[m_ItemData[nstrCurSelItemKey].uId].GetGenre()==0)
		{
#ifdef WIN32
		CCMessageBox("该物品不能拆分","提示:");
#else
		CCMessageBox(UTEXT("该物品不能拆分",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		    return;
		}
	}
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre    = m_ItemData[nstrCurSelItemKey].uGenre;
	Obj.Obj.uId       = m_ItemData[nstrCurSelItemKey].uId;
	Obj.Region.h      = m_ItemData[nstrCurSelItemKey].DataX;
	Obj.Region.v      = m_ItemData[nstrCurSelItemKey].DataY;
	Obj.Region.Width  = m_ItemData[nstrCurSelItemKey].DataW;
	Obj.Region.Height = m_ItemData[nstrCurSelItemKey].DataH;
	Obj.eContainer    = UOC_ITEM_TAKE_WITH;

	if (g_pCoreShell)
	{
		if (g_pCoreShell->OperationRequest(GOI_NPC_ITEM_BREAK,(unsigned int)(&Obj), 1))
		{//拆分成功了,设置显示
			char nKey[32];
			t_sprintf(nKey,"item_%u",Item[Obj.Obj.uId].GetID());
			std::string nItemKey = nKey;
			CCSprite *nItemSpr =(CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			if (nItemSpr)
			{
				CCLabelTTF *stacklabel =(CCLabelTTF *)nItemSpr->getChildByTagKey("stack");
				if (stacklabel)
				{
					int nstackCount = Item[Obj.Obj.uId].GetStackNum();
					t_sprintf(nKey,"%d",nstackCount);
					stacklabel->setString(nKey);
				}
			}
		}
	}
}
//储物箱
void KuiItem::chuCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
			CCMessageBox("该物品不能储存","提示:");
#else
			CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		   g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_repository); 
}

void KuiItem::zhengCallBackFunc(CCObject * pSender)
{
	if (!g_pCoreShell) return;

	if (g_pCoreShell->GetAutoplayid())
	{
#ifdef WIN32
		CCMessageBox("挂机中,禁止整理!","提示");
#else
		CCMessageBox(UTEXT("挂机中,禁止整理!",1).c_str(),UTEXT("提示",1).c_str());
#endif
		return;
	}

	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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

//放快捷栏
void KuiItem::kuaiCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
		CCMessageBox("该物品不能移动","提示:");
#else
		CCMessageBox(UTEXT("该物品不能移动",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_immediacy); 
}
//字母袋
void KuiItem::ziCallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
		CCMessageBox("该物品不能储存","提示:");
#else
		CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
#endif	
		return;
	}
	if  (!m_nIstrue)
	{
#ifdef WIN32
		CCMessageBox("子母袋还没有开启","提示:");
#else
		CCMessageBox(UTEXT("子母袋还没有开启",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_equipmentex); 
		//g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_immediacy); 
}

void KuiItem::kuo1CallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
		CCMessageBox("该物品不能储存","提示:");
#else
		CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_exbox1); 
	/*
#ifdef WIN32
	CCMessageBox("扩展箱暂时关闭","提示:");
#else
	CCMessageBox(UTEXT("扩展箱暂时关闭",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/	

}
void KuiItem::kuo2CallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
		CCMessageBox("该物品不能储存","提示:");
#else
		CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_exbox2); 
	/*
#ifdef WIN32
	CCMessageBox("扩展箱暂时关闭","提示:");
#else
	CCMessageBox(UTEXT("扩展箱暂时关闭",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
}
void KuiItem::kuo3CallBackFunc(CCObject * pSender)
{
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
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
		CCMessageBox("该物品不能储存","提示:");
#else
		CCMessageBox(UTEXT("该物品不能储存",1).c_str(),UTEXT("提示:",1).c_str());
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
/*#ifdef WIN32
		CCMessageBox("移动的太频繁了","提示:");
#else
		CCMessageBox(UTEXT("移动的太频繁了",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/
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
		g_pCoreShell->OperationRequest(GOI_ADDITEM_CLIENT,(int)(&Obj),room_exbox3); 
/*#ifdef WIN32
	CCMessageBox("扩展箱暂时关闭","提示:");
#else
	CCMessageBox(UTEXT("扩展箱暂时关闭",1).c_str(),UTEXT("提示:",1).c_str());
#endif*/

}
//执行上层对象的回调函数，关闭弹出框
void KuiItem::buttonCallBackFunc(CCObject * pSender)
{
	if  (!pSender)
	{
		if (m_callfun &&  m_callbackListener)
			(m_callbackListener->*m_callfun)(NULL);
	}
	else
	{
		CCNode * node = dynamic_cast<CCNode *>(pSender);
		//node->setTag(3000);//设置tag，在调用层可以获取到
		if (m_callfun &&  m_callbackListener)
			(m_callbackListener->*m_callfun)(node);
	}
	
	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiItem::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}


//重写触摸注册函数，重新给定触摸级别
void KuiItem::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuiItem::getRect(CCNode* pNode,int i)
{
	//m_origin 背景的相对整个屏幕的开始位置
	CCRect  rc;
	rc.origin     = pNode->getPosition();                      //这个因该是图内偏移
	rc.size       = pNode->getContentSize();                   //这个物品的宽度和高度
	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}
//sap xep item dang drag & drop KienTM
void KuiItem::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling) return;
	m_bScrolling = false;
	
	

	CCPoint point = ParentNode_Item->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, winSize.width/*ParentNode_Team->getContentSize().width*/,winSize.height/* ParentNode_Team->getContentSize().height*/);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;
	CCPoint diff    = ccpSub(point,m_lastPoint);
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * SkillNode =NULL;
	CCLayerColor *nCurColorlayer = NULL;
	_ItemData::iterator it;
	for( it = m_ItemData.begin(); it != m_ItemData.end(); ++it)
	{
		if (it->second.uId>0 && it->second.uId<MAX_ITEM)
		{////����ĳ���ܵļӳ�
			char nKey[32];
			//char msg[50];
			t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			// t_sprintf(msg,"ccTouchEnded:%d,nKey:%d",nKey,nKey);
			// ClientShowMsg(msg);
			if(ParentNode_ItemMove->GetTagbyKey().c_str()==nItemKey){
				if (SkillNode)
				{

					
					int nWidth = 0;
					int nHeight = 0;
					m_StartPos.x =31;
					m_StartPos.y =53;
					//lay vi tri cua o item
					int nCurX = (SkillNode->getPosition().x-m_StartPos.x)/ITEM_CELL_SIZE;
					
					int nCurY =(m_size.height-SkillNode->getPosition().y-m_StartPos.y)/(ITEM_CELL_SIZE);
					if (nCurX>=5){
						nCurX = 4;
					}
					if (nCurY>5){
						nCurY = 5;
					}
					int nIdx = Item[it->second.uId].GetID();
					ItemPos	sSrc, sDest;
					sSrc.nPlace     = pos_equiproom;    //Vi tri goc
					sSrc.nX         = m_ItemData[nstrCurSelItemKey].DataX;
					sSrc.nY         = m_ItemData[nstrCurSelItemKey].DataY;
					//sSrc.nSIsRongqi = m_ItemData[nstrCurSelItemKey].m_SisRongqi;

					sDest.nPlace     = pos_equiproom;  //Vi tri moi
					sDest.nX         = nCurX;
					sDest.nY         = nCurY;
					//sDest.nDIsRongqi = pSync->m_DisRongqi;
					if (!Player[CLIENT_PLAYER_INDEX].MoveItem(sDest, sSrc,nIdx)){
						//Neu vi tri chuyen toi da co item thi khai bao object no la data cua item, va call func AddObject de tra ve vi tri cu
						KUiDraggedObject no;
						no.uGenre = m_ItemData[nstrCurSelItemKey].uGenre;
						no.uId    = m_ItemData[nstrCurSelItemKey].uId;
						no.DataX  = m_ItemData[nstrCurSelItemKey].DataX;     
						no.DataY  = m_ItemData[nstrCurSelItemKey].DataY;     
						no.DataW  = m_ItemData[nstrCurSelItemKey].DataW;
						no.DataH  = m_ItemData[nstrCurSelItemKey].DataH;
						AddObject(&no,1);
					}

					//Hoan thanh chuc nang sap xep item drag & drop mobile 6:25 AM 21/09/2022 - Dev KienTM
				}
			}
			
		}else{
			if (m_descPadIsOpen)
			{  
				m_descPadIsOpen = false;
				this->removeChildByTag(1001,true);
			}
			if (m_EqdescPadIsOpen)
			{  
				m_EqdescPadIsOpen = false;
				this->removeChildByTag(1002,true);
			}
		}
	}
	

	
}
void KuiItem::ClientShowMsg(const char *Msg)
{
	// CCMessageBox(Msg,"ClientShowMsg");
	KSystemMessage	sMsg;
	ZeroMemory(&sMsg,sizeof(sMsg));
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.eType         = SMT_NORMAL;
	sMsg.byParamSize   = 0;
	sMsg.byPriority    = 0;

	if (sizeof(Msg)<125)
	  t_sprintf(sMsg.szMessage,Msg);
	else
	  t_sprintf(sMsg.szMessage,"Tin nhắn quá dài không thể gửi!");

	sMsg.nMsgLen = TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg,0);
}

//移动中
void KuiItem::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrolling || !__isCanMove) return;
	if (m_descPadIsOpen)
	{  
		m_descPadIsOpen = false;
		this->removeChildByTag(1001,true);
	}
	if (m_EqdescPadIsOpen)
	{  
		m_EqdescPadIsOpen = false;
		this->removeChildByTag(1002,true);
	}

	CCPoint point = ParentNode_Item->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, winSize.width,winSize.height);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;
	CCPoint diff    = ccpSub(point,m_lastPoint);
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * SkillNode =NULL;
	CCLayerColor *nCurColorlayer = NULL;
	bool isthis = false;


	_ItemData::iterator it;
	for( it = m_ItemData.begin(); it != m_ItemData.end(); ++it)
	{
		if (it->second.uId>0 && it->second.uId<MAX_ITEM)
		{////����ĳ���ܵļӳ�
			char nKey[32];
			t_sprintf(nKey,"item_%u_color",Item[it->second.uId].GetID());
			std::string nItemKey = nKey;
			nCurColorlayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nItemKey);
			t_sprintf(nKey,"item_%u",Item[it->second.uId].GetID());
			nItemKey = nKey;
			SkillNode = (CCSprite *)ParentNode_Item->getChildByTagKey(nItemKey);
			if(ParentNode_ItemMove->GetTagbyKey().c_str()==nItemKey){
				if (SkillNode)
				{
					CCPoint touchLocaltion = pTouch->getLocation();
					SkillNode->setPosition(SkillNode->getPosition() + pTouch->getDelta());
					if (nCurColorlayer)
					{
						nCurColorlayer->setPosition(ccp(SkillNode->getPosition().x-ITEM_CELL_SIZE/2,SkillNode->getPosition().y-ITEM_CELL_SIZE/2) + pTouch->getDelta());	
					}
				}
			}
			
			
		}
	}
}
//触摸函数ccTouchBegan，返回true
bool KuiItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

	CCPoint point = ParentNode_Item->convertToNodeSpace(CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()));
	CCRect rect = CCRectMake(0, 0, winSize.width/*ParentNode_Team->getContentSize().width*/,winSize.height/* ParentNode_Team->getContentSize().height*/);
	m_bScrolling = rect.containsPoint(point); //如果在范围内 就可以拖动
	m_lastPoint = point;

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
				if (getRect(nCurColorlayer,0).containsPoint(location))
				{//点中了
					if (SkillNode)
					{
						char msg[32];
						//SkillNode->runAction(red);
						SkillNode->setColor(ccRED); 
						KUiItemdObject nTempItemData;
						ZeroMemory(&nTempItemData,sizeof(nTempItemData));
						m_ItemData[nItemKey].nRect  = getRect(nCurColorlayer,0);
						nTempItemData.nRect = m_ItemData[nItemKey].nRect;
						//m_ItemData[nItemKey].nindex = -1;
						
						//开始创建一个已装备说明
						if (Item[it->second.uId].GetGenre()==item_equip)
						{//如果是装备
							
							int nItemBoxIdx= Item[it->second.uId].GetDetailType();

							switch(nItemBoxIdx)
							{
							case 0:
							case 1:
								{//武器
									nTempItemData.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_weapon);
								}
								break;
							case 2://衣服
								{
									nTempItemData.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_body);
								}
								break;
							case 3://戒指
								{
									nTempItemData.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_ring1);
									if (!nTempItemData.uId)
										nTempItemData.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_ring2);
								}
								break;
							case 4:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_amulet);
								}
								break;
							case 5:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_foot);
								}
								break;
							case 6:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_belt);
								}
								break;
							case 7://itempart_head
								    nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_head);
								break;
							case 8:
								{
                                    nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_cuff);
								}
								break;
							case 9:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_pendant);
								}
								break;
							case 10:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse);
								}
								break;
							case 11:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_mask);
								}
								break;
							case 12:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_pifeng);
								}
								break;
							case 13:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_yinjian);
								}
								break;
							case 14:
								{
									nTempItemData.uId=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_shiping);
								}
								break;
							default:
								break;
							}
					
							if (nTempItemData.uId>0)
							{
							  float _bak_x =m_ItemData[nItemKey].nRect.origin.x;
							  nTempItemData.nRect.origin.x =m_ItemData[nItemKey].nRect.origin.x-370+Item[it->second.uId].GetWidth()*ITEM_CELL_SIZE;
							  CCLayer * sTempLayer = KuiItemdesc::create(nTempItemData,this,callfuncN_selector(KuiItem::closeItemPadCallback1),10);
							//恢复原来的数据
							  nTempItemData.nRect.origin.x =_bak_x;
							  m_EqdescPadIsOpen = true;
							  
							  this->addChild(sTempLayer,2,1002);
							}
							
						}
						CCLayer * sDescLayer = KuiItemdesc::create(it->second,this,callfuncN_selector(KuiItem::closeItemPadCallback),0);
						ParentNode_ItemMove->setTagbyKey(nItemKey);
						ParentNode_ItemMove->addChild(SkillNode,1);
						m_lastPoint = SkillNode->getPosition();
						m_descPadIsOpen = true;
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
						//m_CurSelItemIndex = -1;
					}

					if (SkillNode)
						SkillNode->setColor(ccWHITE);           //默认颜色
				}
			}
				 		
		}
	}
	
	return true;
}

void KuiItem::closeItemPadCallback1(CCNode *pNode)
{
 	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("KuiItem::closeItemPadCallback1");
	m_descPadIsOpen = false;
	m_EqdescPadIsOpen =false;
	this->removeChildByTag(1001,true);
	this->removeChildByTag(1002,true);
}

void KuiItem::closeItemPadCallback(CCNode *pNode)
{
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("KuiItem::closeItemPadCallback");
	m_descPadIsOpen = false;
	m_EqdescPadIsOpen =false;
	this->removeChildByTag(1001,true);
	this->removeChildByTag(1002,true);
}
void KuiItem::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen && g_pCoreShell)
	{
	    m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
		char nPointCount[32];
	   if (m_nMoney)
	   {
			char szMoneyText[32];
			if (m_nMoney < 10000)
			{
				sprintf(nPointCount, "%d lượng", m_nMoney);
			}
			else
			{

				if (m_nMoney%10000 == 0)
				{
					sprintf(nPointCount, "%d vạn lượng", m_nMoney/10000);
				}
				else
				{
					sprintf(nPointCount, "%d vạn %d lượng", m_nMoney/10000, m_nMoney%10000);
				}
			}
		  // t_sprintf(nPointCount,"%d",m_nMoney);
		  // pSkillPointLabel->setColor(ccWit)
		   pMoneyLabel->setString(nPointCount);
		   pMoneyLabel->setColor(ccRED);
	   }
	   if  (pXuLabel)
	   {
		   t_sprintf(nPointCount,"%d",m_nXu);
		   // pSkillPointLabel->setColor(ccWit)
		   pXuLabel->setString(nPointCount);
		   pXuLabel->setColor(ccYELLOW);
	   }
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

void KuiItem::draw()
{
	//CCMessageBox("无限循环","draw");
}

void KuiItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
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
			AddObject(&Obj,1);

		}
		else      //减少
		{
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
		UpdateData();
}

void KuiItem::UpdateData()
{
// m_CurSelItemIndex = -1;
   nstrCurSelItemKey ="";
  // memset(m_ItemData,0,sizeof(KUiItemdObject) * 60);
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
  /* for (int i=0;i<m_nNumObjects;++i)
   {
	t_sprintf(nItemKey,"item_%d",i+1);
	std::string nKey =nItemKey;
	ParentNode_Item->removeChildByTagKey(nKey,true);
	t_sprintf(nItemKey,"color_%d",i+1);
	nKey =nItemKey;
	ParentNode_Item->removeChildByTagKey(nKey,true);
	//CCLayerColor *bgcolorLayer = (CCLayerColor *)ParentNode_Item->getChildByTagKey(nKey);
   }*/

	KUiObjAtRegion* pObjs = NULL;
	int nCount    = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0); //得到随身带物品的数量

	m_nNumObjects = nCount;

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

				if (!Item[pObjs[i].Obj.uId].GetImagePath())
				{
					t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
				}
				else
					t_sprintf(nItemSprPath,Item[pObjs[i].Obj.uId].GetImagePath());

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

//增加物品
int KuiItem::AddObject(KUiDraggedObject* pObject, int nCount)
{
	if (!pObject) return false;
	m_StartPos.x =31;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =53;//+ITEM_CELL_SIZE/2;
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

				if (!Item[pObject->uId].GetImagePath())
				{
					t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
				}
				else
					t_sprintf(nItemSprPath,Item[pObject->uId].GetImagePath());

				if  (nItemSprPath[0])
				{
					//char msg[50];
					g_StrLower(nItemSprPath);
					int m_nWidth,m_nHeight,nFrams;
					CCTexture2D *bgCur = NULL;
					SPRFRAMSINFO nSprInfo;
					ZeroMemory(&nSprInfo,sizeof(nSprInfo));
					bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
					if (bgCur==NULL)
					{
						t_sprintf(nItemSprPath,"\\spr\\others\\问号.spr");
						g_StrLower(nItemSprPath);
						bgCur = _getinidata.getinidata_one(nItemSprPath,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
						//return false;
					}
					char nItemKey[32];
					ZeroMemory(&nItemKey,sizeof(nItemKey));
					t_sprintf(nItemKey,"item_%u",Item[pObject->uId].GetID());
					std::string nKey =nItemKey;
					CCSprite *nItemSpr = (CCSprite *)ParentNode_Item->getChildByTagKey(nKey);
					//t_sprintf(msg,"AddObject:%d,nKey:%d",nItemKey,nItemKey);
					//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nItemKey);
					if (!nItemSpr)
					{
						
						nItemSpr = CCSprite::createWithTexture(bgCur);
						nItemSpr->setTagbyKey(nKey);
						ParentNode_Item->addChild(nItemSpr,1);
						if (Item[pObject->uId].GetGenre()==item_equip){
							nItemSpr->setScale(0.8);
						}
						//find here
						if (Item[pObject->uId].GetDetailType()==equip_armor){
							nItemSpr->setScale(0.7);
						}
						if  (Item[pObject->uId].GetGenre()!=item_equip && Item[pObject->uId].IsStack())
						{//不是装备 可叠加物品
							char stack[32];
							t_sprintf(stack,"%d",Item[pObject->uId].GetStackNum());
							CCLabelTTF *stuckCountlabel = CCLabelTTF::create(stack,UI_GAME_FONT_DEFAULT,14);
							stuckCountlabel->setColor(ccWHITE);
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

					ccColor4B color = ccc4(0, 56, 33, 150);//112, 128, 144
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
					switch (no.DataX)
					{
					case 0:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+2;
						break;
					case 1:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+8;
						break;	
					case 2:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+14;
						break;
					case 3:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+20;
						break;
					case 4:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE+no.DataW*ITEM_CELL_SIZE/2)+26;
						break;		
					default:
						break;
					}

					switch (no.DataY)
					{
					case 0:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+2;
						break;
					case 1:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+8;
						break;	
					case 2:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+14;
						break;
					case 3:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+20;
						break;
					case 4:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+26;
						break;
					case 5:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+no.DataH*ITEM_CELL_SIZE/2)+32;
						break;			
					default:
						break;
					}
					nItemSpr->setPosition(ccp(nCurX,m_size.height-nCurY));
			
					switch (no.DataX)
					{
					case 0:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+2;
						break;
					case 1:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+8;
						break;	
					case 2:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+14;
						break;
					case 3:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+20;
						break;
					case 4:
						nCurX = (m_StartPos.x+no.DataX*ITEM_CELL_SIZE)+26;
						break;		
					default:
						break;
					}

					switch (no.DataY)
					{
					case 0:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+2;
						break;
					case 1:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+8;
						break;	
					case 2:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+14;
						break;
					case 3:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+20;
						break;
					case 4:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+26;
						break;
					case 5:
						nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+32;
						break;			
					default:
						break;
					}
					//nCurY = (m_StartPos.y+no.DataY*ITEM_CELL_SIZE+1*ITEM_CELL_SIZE)+32;
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