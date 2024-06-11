#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "KuiNewPlayer.h"
//#include "Klogin.h"
#include "Klogin_f.h"
#include "KTranSence.h"
#include "engine/CXmlStream.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
#include "gameui/PopupLayer.h"
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
#include "gamescene/KUpdateLayer.h"
#include "gameui/KuiSelNativePlace.h"
extern iCoreShell*	g_pCoreShell; //全局调用
extern KImageStore2 m_ImageStore; //全局调用
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
#include <list>
USING_NS_CC;
KuiNewPlayer::KuiNewPlayer()
{
  m_Status = LL_S_ROLE_LIST_READY; //等待角色进入游戏服务器
  m_Result = LL_R_NOTHING;
  memset(&m_Info, 0, sizeof(m_Info));
  m_Info.Attribute = series_metal;
  m_Info.Gender    = OBJ_G_MALE;
  m_Info.NativePlaceId = nNativePlaceId;
  memset(&m_propTypeInfoTable, 0, sizeof(m_propTypeInfoTable));
  pLoginItem=NULL;
}

KuiNewPlayer::~KuiNewPlayer()
{
}
CCScene* KuiNewPlayer::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	KuiNewPlayer *layer = KuiNewPlayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool KuiNewPlayer::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}
	/*visibleSize        = CCDirector::sharedDirector()->getVisibleSize();//可见的
	origin             = CCDirector::sharedDirector()->getVisibleOrigin();
	CCSize size        = CCDirector::sharedDirector()->getWinSize();
	*/
	m_sprCount         =0;
	//nTimeCount = 0;
	m_nWidth   = 0;
	m_nHeight  = 0;
	clicked    = false;
	//pWW        =NULL;
	CCAnimate* animate         =NULL;
	this->setTouchEnabled(true);
	this->scheduleUpdate();  //遥杆更新调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	char nSprName[128]={0};
	int nFrams=0;
	m_CurSelIndex = 0;
	CCMenuItemImage *pCancelItem=NULL;
	SelbgSprite = NULL;
	//CCLayer *mainlayer = CCLayer::create();
	actionManager = CCDirector::sharedDirector()->getActionManager();
	/*pRoela = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);
	pRoelb = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);
	pRoelc = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);
	*/
	//float m_Scalex,m_Scaley; //与背景 屏幕大小的缩放比例
	//登陆选择框 \Spr\Ui3\登入界面\选存档人物\角色
	t_sprintf(m_szPlayerImgPrefix,"\\Spr\\Ui3\\登入界面\\选存档人物\\角色");
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\登陆界面_7_角色名.spr");
	g_StrLower(nSprName);
	CCTexture2D *SelCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (SelCur)
	{
		SelbgSprite=CCSprite::createWithTexture(SelCur);
		SelbgSprite->setPosition(ccp(visibleSize.width/2 + origin.x,50));
		m_sprCount++;
		this->addChild(SelbgSprite,1,m_sprCount);
	}

	//确认按钮
	nFrams=0; //"\\spr\\Ui3\\loginui\\saverole\\ok_9_btn.spr"
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_确定.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_确定.spr");
	g_StrLower(nSprName);
	CCTexture2D *Login = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if  (Login)
	{	
		char nSprFilePath[64]={0};
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
		CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
		CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,2);
		CCTexture2D *pDisabledTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		pLoginItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pDisabledTexture,
			pSelectedTexture,this,menu_selector(KuiNewPlayer::mainEnterCallback));
		//pLoginItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
		//pLoginItem->setOrderOfArrival(1);
		//pLoginItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pLoginItem->setScaleY(m_Scaley);
	}
	//取消 退到登陆界面
	nFrams=0;
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_取消.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
	g_StrLower(nSprName);
	CCTexture2D *Cancel = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (Cancel)
	{
		char nSprFilePath[64]={0};
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
		CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
		CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,2);
		CCTexture2D *pDisabledTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		pCancelItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pDisabledTexture,
			pSelectedTexture,this,menu_selector(KuiNewPlayer::mExit));
		//pCancelItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
		//pCancelItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pCancelItem->setScaleY(m_Scaley);
		//pCancelItem->setOrderOfArrival(1);
	}
	CCSprite *bgSprite=CCSprite::create("loginbg_2-9.jpg");//登陆背景
	bgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	float winw = visibleSize.width;   //获取屏幕宽度
	float winh = visibleSize.height;  //获取屏幕高度
	float spx  = bgSprite->getTextureRect().getMaxX();
	float spy  = bgSprite->getTextureRect().getMaxY();
	bgSprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
	bgSprite->setScaleY(winh/spy);
	m_sprCount++;
	this->addChild(bgSprite,0,m_sprCount);
	//界面动画效果
		 t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\uidonghua\\yezi.spr");
		 g_StrLower(nSprName);
		 CCTexture2D *bgCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		 if  (bgCur)
		 {		
			CCSprite *ySprite=CCSprite::createWithTexture(bgCur);
			ySprite->setPosition(ccp(visibleSize.width/2 + origin.x-100, visibleSize.height/2 + origin.y));
			
			/*float winw = visibleSize.width;   //获取屏幕宽度
			float winh = visibleSize.height;  //获取屏幕高度
			float spx  = bgSprite->getTextureRect().getMaxX();
			float spy  = bgSprite->getTextureRect().getMaxY();
			ySprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
			ySprite->setScaleY(winh/spy);
			*/
			m_sprCount++;
			this->addChild(ySprite,2,m_sprCount);
			char nySprFilePath[64]={0};
			CCAnimation*animation = CCAnimation::create();//创建一个动画
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			for (int i=0;i<nFrams;i++)
			{   
				t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,i);  
				//CCMessageBox(nSprFilePath,"animation");
				animation->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
			}
			//设置动画帧的时间间隔
			animation->setDelayPerUnit(0.10f);
			//设置播放循环 一直播放 为-1
		    animation->setLoops(-1);
			//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
			animation->setRestoreOriginalFrame(TRUE);
			// 创建一个永远循环的动画
			animate = CCAnimate::create(animation);
			//CCRepeatForever* forever = CCRepeatForever::create(animate);
			if (ySprite)
			   ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
		 }
		CCMenu* pMenu=NULL;
		if (Login && Cancel)
		{
			pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);
			//pMenu->alignItemsInRows(1,2,3,4,NULL);     //四列一行
			//pMenu->alignItemsVertically();             //竖排排列 alignItemsHorizontally
			pMenu->alignItemsHorizontally();             //横排排列 默认间隔是kDefaultPadding=5  
			pMenu->alignItemsHorizontallyWithPadding(7);//指定横间隔  
			//pMenu->alignItemsVertically();             //默认间隔也是kDefaultPadding=5 
			//pMenu->alignItemsInRows(3,2,1,NULL);   
			//pMenu->alignItemsVerticallyWithPadding(20);//指定间隔
			pMenu->setPosition(ccp(371,40));//CCPointZero
			SelbgSprite->addChild(pMenu,2);
		}
		 CCMenuItemImage* pjinItem=NULL;
		 CCMenuItemImage* pmuItem=NULL;
		 CCMenuItemImage* pshuiItem=NULL;
         CCMenuItemImage* phuoItem=NULL;
		 CCMenuItemImage* ptuItem=NULL;
		//金
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_金.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		CCTexture2D *jinTexture = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		if (jinTexture)
		{
			char nSprFilePath[64]={0};
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
			CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
			CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			pjinItem = CCMenuItemImage::createWithTexture(
				pNormalTexture,
				pSelectedTexture,
				pSelectedTexture,this,menu_selector(KuiNewPlayer::mSellistCallBack));
			pjinItem->setNewTag(1);
		}
		//木
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_木.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		CCTexture2D *muTexture = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		if (muTexture)
		{
			char nSprFilePath[64]={0};
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
			CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
			CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			pmuItem = CCMenuItemImage::createWithTexture(
				pNormalTexture,
				pSelectedTexture,
				pSelectedTexture,this,menu_selector(KuiNewPlayer::mSellistCallBack));
			pmuItem->setNewTag(2);
		}
		//水
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_水.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		CCTexture2D *shuiTexture = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		if (shuiTexture)
		{
			char nSprFilePath[64]={0};
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
			CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
			CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			pshuiItem = CCMenuItemImage::createWithTexture(
				pNormalTexture,
				pSelectedTexture,
				pSelectedTexture,this,menu_selector(KuiNewPlayer::mSellistCallBack));
			pshuiItem->setNewTag(3);
		}
		//火
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_火.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		CCTexture2D *huoTexture = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		if (huoTexture)
		{
			char nSprFilePath[64]={0};
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
			CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
			CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			phuoItem = CCMenuItemImage::createWithTexture(
				pNormalTexture,
				pSelectedTexture,
				pSelectedTexture,this,menu_selector(KuiNewPlayer::mSellistCallBack));
			phuoItem->setNewTag(4);
		}
		//土
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_土.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		CCTexture2D *tuTexture = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		if (tuTexture)
		{
			char nSprFilePath[64]={0};
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
			CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
			CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
			ptuItem = CCMenuItemImage::createWithTexture(
				pNormalTexture,
				pSelectedTexture,
				pSelectedTexture,this,menu_selector(KuiNewPlayer::mSellistCallBack));
			ptuItem->setNewTag(5);
		}
		//选择人物菜单背景
		CCSprite *nMenuBgSpr = NULL;
		nFrams=0;
		t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\新建角色按钮\\登陆界面_7_选择人物.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
		g_StrLower(nSprName);
		SPRFRAMSINFO nSprInfo;
		ZeroMemory(&nSprInfo,sizeof(nSprInfo));
		CCTexture2D *muBgTexture = _getinidata.getinidata_one(nSprName,0,&m_nWidth,&m_nHeight,&nFrams,&nSprInfo);
		CCSize m_selSize = SelbgSprite->getTextureRect().size;
		float offsetY=0;
		if (muBgTexture)
		{
			offsetY = m_selSize.height/2+muBgTexture->getContentSize().height/2+50;
			nMenuBgSpr = CCSprite::createWithTexture(muBgTexture);
			nMenuBgSpr->setPosition(ccp(m_selSize.width/2,offsetY));
			SelbgSprite->addChild(nMenuBgSpr,1);
		}
		CCMenu *pselMenu = NULL;
		pselMenu = CCMenu::create(pjinItem,pmuItem,pshuiItem,phuoItem,ptuItem,NULL);
		//pMenu->alignItemsInRows(1,2,3,4,NULL);     //四列一行
		//             //竖排排列 alignItemsHorizontally
		//pMenu->alignItemsHorizontally();             //横排排列 默认间隔是kDefaultPadding=5  
		//pMenu->alignItemsHorizontallyWithPadding(7);//指定横间隔  
		//pMenu->alignItemsVertically();               //默认间隔也是kDefaultPadding=5 
		//pMenu->alignItemsInRows(3,2,1,NULL); 
		pselMenu->alignItemsVertically();
		pselMenu->alignItemsVerticallyWithPadding(30); //指定间隔
		pselMenu->setPosition(ccp(m_selSize.width/2,offsetY-10));           //CCPointZero
		SelbgSprite->addChild(pselMenu,2);
		
		//CCSpriteFrameCache
	    //CCTextureCache 纹理缓存
		/*
		
		void addSpriteFramesWithFile(const char *pszPlist)
		：从一个.plist文件添加多个精灵帧。 一个纹理将被自动加载。纹理名称将把.plist后缀名替换为.png来组成。
		void addSpriteFramesWithFile(const char* plist, const char* textureFileName)
		：通过一个.plist文件添加多个精灵帧。纹理将与被创建的精灵帧结合。
		void addSpriteFramesWithFile(const char *pszPlist, CCTexture2D *pobTexture)
		：通过一个.plist文件添加多个精灵帧。纹理将与被创建的精灵帧结合。
		void addSpriteFrame(CCSpriteFrame *pobFrame, const char *pszFrameName)
		：通过给定的名称添加一个精灵帧。 如果名称已经存在，那么原来名称的内容将被新的所替代
		*/
		/*
		float winw = visibleSize.width;   //获取屏幕宽度
		float winh = visibleSize.height;  //获取屏幕高度
		float spx = testSprite->getTextureRect().getMaxX();
		float spy = testSprite->getTextureRect().getMaxY();
		testSprite->setScaleX(winw/spx);  //设置精灵宽度缩放比例
		testSprite->setScaleY(winh/spy);
		*/
	//this->draw();

		//if (m_nNumRole>0)
		{//如果角色大于零的 就开始创建精灵
			    static const char* pszAttribute[series_num] = {"金","木","水","火","土"};
			    static const char* pszGender[2] = { "男", "女" };
			//for (int i = 0; i < m_nNumRole; ++i)
			{
				char nInfoKey[64]={0};
				char szFileName[128]={0};
				GetRoleImageName(szFileName,m_szPlayerImgPrefix,false,m_Info.Attribute,0);//2 灰色动画 0选中 1 为一性过渡动画
				nFrams=0;
				g_StrLower(szFileName);
				//男
				//if (i == 0)
				{
					CCTexture2D *RoleCura = _getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCSprite *RoleSpritea = NULL;
					if (RoleCura)
					{ 	
						RoleSpritea=CCSprite::createWithTexture(RoleCura);
						RoleSpritea->setPosition(ccp(visibleSize.width/2 + origin.x-210, visibleSize.height/2 + origin.y+50));
						m_sprCount++;
						this->addChild(RoleSpritea,2,1000);
					}
					CCAnimation* nTempAni;
				    nTempAni  = creatSpranimation(0);
					if (RoleSpritea)
						//RoleSpritea->runAction(CCSequence::create(Roleanimatea,CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
					    RoleSpritea->runAction(CCSequence::create(CCAnimate::create(nTempAni),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
				}
				//女
				//if (i == 1)
				{
					GetRoleImageName(szFileName, m_szPlayerImgPrefix,true,m_Info.Attribute,2);
					g_StrLower(szFileName);
					CCTexture2D *RoleCurb = _getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCSprite *RoleSpriteb = NULL;
					if (RoleCurb)
					{ 	
						RoleSpriteb=CCSprite::createWithTexture(RoleCurb);
						RoleSpriteb->setPosition(ccp(visibleSize.width/2 + origin.x+210, visibleSize.height/2 + origin.y+50));
						m_sprCount++;
						this->addChild(RoleSpriteb,2,999);
					}
					m_Info.Gender = OBJ_G_FEMALE;
					CCAnimation* nTempAni;
					nTempAni  = creatSpranimation(2);
					if (RoleSpriteb)
						RoleSpriteb->runAction(CCSequence::create(CCAnimate::create(nTempAni),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
						//RoleSpriteb->runAction(CCSequence::create(Roleanimateb,CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
					m_Info.Gender = OBJ_G_MALE;
				}
			}
		}
		//创建一个输入框
		CCScale9Sprite *pBg = CCScale9Sprite::create(); 
		proleNameEditBox = CCEditBox::create(CCSize(112,24),pBg);
		const ccColor3B nfcolor ={255,255,255};
		if (proleNameEditBox)
		{
			proleNameEditBox->setPlaceHolder("role name");
			proleNameEditBox->setFontColor(nfcolor);
			proleNameEditBox->setFontSize(24);
			proleNameEditBox->setMaxLength(32);
			proleNameEditBox->setPosition(ccp(180,40));
			proleNameEditBox->setInputMode(kEditBoxInputModeAny);
			proleNameEditBox->setReturnType(kKeyboardReturnTypeDone);
			//pMoneyEditBox->setDelegate(this);
			//pMoneyEditBox->setDefaultTouchPriority(-128);
			SelbgSprite->addChild(proleNameEditBox,2,500);
		}
	//DrawNode* drawNode = ccDrawNode::create();
	return true;
}

int KuiNewPlayer::GetRoleCount(int& nAdviceChoice)
{
	nAdviceChoice = 0;
	if (m_ClientChoices.szProcessingRoleName[0])
	{
		for (int i = 0; i < m_nNumRole; ++i)
		{
			if (strcmp(m_ClientChoices.szProcessingRoleName, m_RoleList[i].Name) == 0)
			{
				nAdviceChoice = i;
				break;
			}
		}
	}
	return m_nNumRole;
}
//如果此函数返回值大于零,则表明传入的字符串极可能是繁体
long KuiNewPlayer::_GetBig5Count(char *str)
{
	int lnBIG5 = 0;//用於统计可能是繁体字的汉字个数
	int lnGB = 0;//用於统计可能是简体字的汉字个数
	int liTranLen = strlen(str);

	for(int liT = 0; liT<liTranLen-1; liT++)
	{
		//尾字节40-7E是BGI5码特有的,如果扫描到这种编码说明此字元串是繁体(经测试:有例外,可能是汉字的最後一个编码与英文编码组合而成的)
		if((BYTE)(BYTE)str[liT]>=161 && (BYTE)(BYTE)str[liT]<=254 && (BYTE)(BYTE)str[liT+1]>=64 && (BYTE)(BYTE)str[liT+1]<=126)
		{
			//CCMessageBox("1","TEST");
			lnBIG5++;
		}

		if((BYTE)(BYTE)str[liT]>=129 && (BYTE)(BYTE)str[liT]<=160 && (BYTE)(BYTE)str[liT+1]>=64 && (BYTE)(BYTE)str[liT+1]<=126)
		{
			//CCMessageBox("2","TEST");
			lnBIG5++;
		}

		/*if((BYTE)(BYTE)str[liT]>=129 && (BYTE)(BYTE)str[liT]<=160 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
		{
			CCMessageBox("3","TEST");
			lnBIG5++;
		}*/

		//首字节A4-A9在GB中为日文假名,希腊字母,俄文字母和制表符,正常文本中很少出现,而这个范围是BIG5的常用汉字,所以认为这是BIG5码
		if((BYTE)(BYTE)str[liT]>=164 && (BYTE)(BYTE)str[liT]<=169 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
		{
			//CCMessageBox("4","TEST");
			lnBIG5++;
		}

		//GB中首字节AA-AF没有定义,所以首字节位於AA-AF之间,尾字节位於A1-FE的编码几乎100%是BIG5(经测试:没有100%),认为是BIG5码
		/*if((BYTE)(BYTE)str[liT]>=170 && (BYTE)(BYTE)str[liT]<=175 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
			lnBIG5 ++;
		*/
		//首字节C6-D7,尾字节A1-FE在GB中属於一级字库,是常用汉字,而在BIG5中,C6-C7没有明确定义,但通常用来放日文假名和序号,C8-D7属於罕用汉字区,所以可认为是GB码
		//if((BYTE)(BYTE)str[liT]>=196 && (BYTE)(BYTE)str[liT]<=215 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
		//	lnGB ++;
	}

	//如果扫描完整个字元串,可能是简体字的数目比可能是繁体字的数目多就认为是简体字不转简(不一定准确)
	//return lnBIG5-lnGB;
	return lnBIG5;
}

/*void KuiNewPlayer::isSimpleOrComplex(std::string str, std::string encode) 
{

if (str.equals(new std::string(str.getBytes(encode), encode))) {
	//System.out.println(str + "---是简体");

}else{
	//System.out.println(str + "---是繁体");
}
*/
//进入游戏
void KuiNewPlayer::mainEnterCallback(CCObject* pSender)
{//
	if (nAcclient/* && g_pClient*/)
	{
		//开始创建角色
		m_Info.Gender = m_CurSelIndex;
	//开始获取编辑框的内容和 检测字符串
		if  (!proleNameEditBox->getText())
		{
#ifdef WIN32
			CCMessageBox("角色名不能为空!","警告");
#else
			CCMessageBox("the name is Null","Warning");
#endif
		     return;
		}
		//char msg[64];
		//t_sprintf(msg,"地图:%d",m_Info.NativePlaceId);
		//CCMessageBox(msg,"提示:");

		char nRoleName[32];
		t_sprintf(nRoleName,proleNameEditBox->getText());

		int len=0;
		    len = strlen(nRoleName);
			if (len<12 || len>24)
			{
if (_clientlanguage==1)
{
#ifdef WIN32
	CCMessageBox("角色名最小长度12,最大长度24!","警告");
#else
	CCMessageBox(G2U("角色名最小长度12,最大长度24!").c_str(),G2U("警告").c_str());
#endif
}
else
{
#ifdef WIN32
	CCMessageBox("角色名最小长度12,最大长度24!","警告");
#else
	CCMessageBox("the Name length:less than 12 or more than 24","Warning");
#endif
}
				return;
			}

		//开始检测禁止的字符串
		//开始检测字符的合法性
		std::string strcheck="check";
		strcheck+=nRoleName;
		int count = 0,spaceNum=0,chineseNum=0,other=0,letter=0,xiaoxie=0,biaodian=0;
		for(int i = 0; i <strcheck.length();i++)
		{ 
			if(isdigit(strcheck[i])) //判断是否是数字
				count++;
			else if(isspace(strcheck[i]))//判断是否是空格
				++spaceNum;
			else if(isalpha(strcheck[i])==1)//判断是否是字母	 1 为大写 2 为 小写  0 为不是字母
			{
				++letter;
			}
			else if(isalpha(strcheck[i])==2)//判断是否是字母	 1 为大写 2 为 小写  0 为不是字母
			{
				++xiaoxie;
			}
			//else if(ispunct(strcheck[i])) //是否有标点符号
			//	++biaodian;
			else if(strcheck[i]&0x80)//if(strcheck[0]>=0xB0) 0x80
				++chineseNum;        //判断是否是中文(不包括繁体中文)
			else
			{//开放合区注册 不检测特殊符号
				++other;
			}
		} 
if (_clientlanguage==1)
{
		if (spaceNum>0 || count >0 || other>0 || letter>0)
		{ 	//system("pause");
#ifdef WIN32
			CCMessageBox("含有非法字符,请检查姓名\n不能含有空格,数字,大写字母等特殊符号!","警告");
#else
			CCMessageBox(G2U("含有非法字符,请检查姓名\n不能含有空格,数字,大写字母等特殊符号!").c_str(),G2U("警告").c_str());
#endif
			return;
		} 
}
else
{
	if (spaceNum>0 || count >0 || other>0)
	{ 	//system("pause");
#ifdef WIN32
		CCMessageBox("含有非法字符,请检查姓名\n不能含有空格,数字,大写字母等特殊符号!","警告");
#else
		CCMessageBox("Invalid character,please check your name!\n Cannot contain special symbols such as NumBer, spaces, etc.!","Warning");
#endif
		return;
	} 
}
		/*if  (_GetBig5Count(nRoleName))
		{
#ifdef WIN32
			CCMessageBox("含有非法字符,请检查姓名\n不能含有空格,数字,大写字母,繁体等特殊符号!","警告");
#else
			CCMessageBox(G2U("含有非法字符,请检查姓名\n不能含有空格,数字,大写字母,繁体等特殊符号!").c_str(),G2U("警告").c_str());
#endif
			return;
		}*/

		t_sprintf(m_Info.Name,U2G(nRoleName).c_str());//将uf8 专成 中文编码然后发送
		if (CreateRole())
		{
			if (pLoginItem)
			    pLoginItem->setEnabled(false);
		}
	}
}

int  KuiNewPlayer::CreateRole()
{
	if (!m_bIsClientConnecting || !nAcclient || m_ClientChoices.nServerRegionIndex<=0)
		return false;

	int nRet = false;
	m_Result = LL_R_NOTHING;

	if (m_Status == LL_S_ROLE_LIST_READY && m_nNumRole < MAX_PLAYER_PER_ACCOUNT &&
		m_Info.Attribute >= 0 && m_Info.Attribute < series_num)
	{
		int nNameLen = strlen(m_Info.Name);
		if (nNameLen >= 1 && nNameLen < sizeof(m_Info.Name))
		{
			char	Data[sizeof(TProcessData) + sizeof(NEW_PLAYER_COMMAND)];
			TProcessData*	pNetCommand = (TProcessData*)&Data;
			NEW_PLAYER_COMMAND* pInfo = (NEW_PLAYER_COMMAND*)pNetCommand->pDataBuffer;
			pInfo->m_btRoleNo = m_Info.Gender;    //男女
			pInfo->m_btSeries = m_Info.Attribute; //五行
			pInfo->m_NativePlaceId = m_Info.NativePlaceId; //出生地图ID

			memcpy(pInfo->m_szName, m_Info.Name, nNameLen);
			pInfo->m_szName[nNameLen] = '\0';
            //PathFileExists();
			pNetCommand->nProtoId = c2s_newplayer;     //发往bishop
			pNetCommand->nDataLen = sizeof(NEW_PLAYER_COMMAND) - sizeof(pInfo->m_szName) + nNameLen + 1/* sizeof( '\0' ) */;
			pNetCommand->ulIdentity = 0;
			//第几个服务器
			pNetCommand->uSelServer = m_ClientChoices.nServerRegionIndex;//KUiSelServer::GetCurServer();
			//帐号服务器
			nAcclient->SendPackToServer(&Data, sizeof(TProcessData) - sizeof(pNetCommand->pDataBuffer) + pNetCommand->nDataLen);
            //赋值给本地储蓄的角色名
			memcpy(m_ClientChoices.szProcessingRoleName, m_Info.Name, nNameLen);
			m_ClientChoices.szProcessingRoleName[nNameLen] =0;

			m_Status = LL_S_CREATING_ROLE;
			m_Result = LL_R_NOTHING;
			nRet = true;
			//CCMessageBox("发包成功","发包成功");
		}
	}
	return nRet;
}

//循环更新虚函数 默认自动调用
void KuiNewPlayer::update(float delta)
{
	unsigned int nSize = 0;
	const char* pBuffer = NULL;
	if (m_bIsClientConnecting)
	{ //账号服务器
		while (true)
		{
			if (!nAcclient)
				break;
			//无限循环从服务器获取数据小包
			nSize = 0;
			pBuffer = NULL;

			pBuffer = (const char*)nAcclient->GetPackFromServer(nSize);
			if (!(pBuffer && nSize))
				break;
			PROTOCOL_MSG_TYPE*	pMsg = (PROTOCOL_MSG_TYPE*)pBuffer;
			PROTOCOL_MSG_TYPE	Msg  = *pMsg;  //协议头			
			//CCAssert(Msg > s2c_multiserverbegin || Msg < s2c_end,"");
			if  (Msg == s2c_login || Msg == s2c_roleserver_getrolelist_result ||Msg == s2c_notifypl_ayerlogin_hfkqotivbhd || Msg == s2c_rolenewdelresponse)
			     AcceptNetMsg(pMsg);
		}
	}
	//----处理来自游戏服务器的数据----
	if (/*m_bIsGameSevConnecting && */g_pClient)
	{
		while (true)
		{
			if (!g_pClient)	//GS的数据包
				break;
			//无限循环从服务器获取数据小包
			nSize  = 0;
			pBuffer = NULL;
			pBuffer = (const char*)g_pClient->GetPackFromServer(nSize);

			if (!(pBuffer && nSize))
				break;
			PROTOCOL_MSG_TYPE *pMsg = (PROTOCOL_MSG_TYPE*)pBuffer; //当前包
			
			while(pMsg < (PROTOCOL_MSG_TYPE*)(pBuffer + nSize))
			{//帅选数据包
				PROTOCOL_MSG_TYPE Msg = *pMsg;
				// 跨服务器的协议
				if (Msg == s2c_notifyplayerexchange)
				{//转换地图或转服
					//ProcessSwitchGameSvrMsg(pMsg);
					break;
				}
				else if (g_pCoreShell)
				{
					if  (Msg <= s2c_clientbegin ||  Msg >= s2c_end || g_pCoreShell->GetProtocolSize(Msg)==0)
						break;

					g_pCoreShell->NetMsgCallbackFunc(pMsg);          //接受从服务器发来的协议消息

					if (g_pCoreShell->GetProtocolSize(Msg) > 0)
						pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + g_pCoreShell->GetProtocolSize(Msg));
					else //没有限制长度的 -1
						pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + PROTOCOL_MSG_SIZE + (*(unsigned short*)(((char*)pMsg) + PROTOCOL_MSG_SIZE)));			
				}
			}
		}
	}
}

CCRect KuiNewPlayer::getRect(CCNode* pNode)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition();
	rc.size       = pNode->getContentSize();
	rc.origin.x -= 50;//rc.size.width;
	rc.origin.y -= 80;//rc.size.height;
	rc.size.width /=6;
	rc.size.height /=4;
	//CCSize s = pNode->getContentSize();
	//return CCRectMake(-s.width / 2, -s.height / 2, s.width, s.height);
	return rc;
}

void KuiNewPlayer::draw()
{   

	if (m_Result != LL_R_NOTHING)
	{
		if (nAcclient)
		{
			m_bIsPlayerInGame =false;
			m_bIsClientConnecting=false;
			nAcclient->Cleanup();
		}

		if (g_pClient)
		{;
			m_bIsPlayerInGame =false;
			m_bIsGameSevConnecting=false;
			g_pClient->Cleanup();
			delete g_pClient;
			g_pClient=NULL;
		}
		isthisVer = false;
		CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		return;
	}

	if (m_Status == LL_S_ENTERING_GAME)
	{//马上跳转画面
		CCDirector::sharedDirector()->replaceScene(KTranSence::scene());
		//CCDirector::sharedDirector()->replaceScene(KgameWorld::scene());
		return;
	}
}
//
bool KuiNewPlayer::isFileExist(const char* pFileName)
{
	if( !pFileName ) return false;
	//strFilePathName is :/data/data/ + package name
	std::string filePath = CCFileUtils::sharedFileUtils()->getWritablePath();//getWriteablePath();
	
	filePath += pFileName;

	FILE *fp = fopen(filePath.c_str(),"r");
	if(fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}

void KuiNewPlayer::copyData(const char* pFileName)
{
	std::string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pFileName);
	unsigned long len = 0;
	unsigned char * data = NULL;
	data = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(),"r",&len);
	std::string destPath = CCFileUtils::sharedFileUtils()->getWritablePath();//getWriteablePath();
	destPath += pFileName;
	FILE *fp = fopen(destPath.c_str(),"w+");
	fwrite(data,sizeof(unsigned char),len,fp);
	fclose(fp);
	delete [] data;
	data = NULL;
}

/*
void KuiNewPlayer::onEnter()
{

}

void KuiNewPlayer::onExit()
{

}
//进入后 有 过渡完成
void KuiNewPlayer::onEnterTransitionDidFinish()
{

}*/

bool KuiNewPlayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void KuiNewPlayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void KuiNewPlayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void KuiNewPlayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
}

void KuiNewPlayer::CheckClick(float tt)
{
}

void KuiNewPlayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void KuiNewPlayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处
	RoleNodeman = (CCSprite *)this->getChildByTag(1000);
	RoleNodewoman = (CCSprite *)this->getChildByTag(999);

	char nRoleInfo[128]={0};
	if (RoleNodeman)
	{
		if (getRect(RoleNodeman).containsPoint(location))
		{
			m_CurSelIndex = 0;
			if (m_CurSelIndex == 0)
			{//原来就等于0 灰色的 
				m_Info.Attribute;
				m_Info.Gender = OBJ_G_MALE;
				CCAnimation* mananimation = creatSpranimation(1);//过渡动画
				actionManager->removeAllActionsFromTarget(RoleNodeman);
				//RoleNodea->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1"))));
				RoleNodeman->runAction(CCSequence::create(CCAnimate::create(mananimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
				if (RoleNodewoman)
				{  
					m_Info.Gender = OBJ_G_FEMALE;
					CCAnimation*  womenanimation = creatSpranimation(2);//灰色
					actionManager->removeAllActionsFromTarget(RoleNodewoman);
					RoleNodewoman->runAction(CCSequence::create(CCAnimate::create(womenanimation),NULL,NULL));
				}
			}  
			//EnterGame();
		}
	}

	if (RoleNodewoman)
	{
		if (getRect(RoleNodewoman).containsPoint(location))
		{
			m_CurSelIndex = 1;
			if (m_CurSelIndex == 1)
			{//原来就等于0 灰色的 
				m_Info.Attribute;
				m_Info.Gender = OBJ_G_FEMALE;
				CCAnimation* womanimation = creatSpranimation(1);//过渡动画
				actionManager->removeAllActionsFromTarget(RoleNodewoman);
				//RoleNodea->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1"))));
				RoleNodewoman->runAction(CCSequence::create(CCAnimate::create(womanimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
				if (RoleNodeman)
				{
					m_Info.Gender = OBJ_G_MALE;
					CCAnimation* manimation = creatSpranimation(2);//灰色
					actionManager->removeAllActionsFromTarget(RoleNodeman);
					RoleNodeman->runAction(CCSequence::create(CCAnimate::create(manimation),NULL,NULL));
				}
			} 
			
			//EnterGame();
		}
	}
}

void KuiNewPlayer::finish()
{
	if (!nisgetinfo)
		mExit(NULL);  
	/*if  (m_CurSelIndex==0)
	{
		m_Info.Attribute;
		m_Info.Gender = OBJ_G_MALE;
		actionManager->removeAllActionsFromTarget(RoleNodeman);
		RoleNodeman->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_0")),NULL,NULL));
	}

	if  (m_CurSelIndex==1)
	{
		actionManager->removeAllActionsFromTarget(RoleNodewoman);
		RoleNodewoman->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("1_0")),NULL,NULL));
	}*/

}

void KuiNewPlayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void KuiNewPlayer::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void KuiNewPlayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}


void KuiNewPlayer::mSellistCallBack(CCObject* pSender){
	if (!pSender) return;
	CCMenuItemImage* nCurSelItem = (CCMenuItemImage*)pSender;
	int nSelSeries = nCurSelItem->getNewTag();
	m_Info.Attribute = nSelSeries-1;
	CCAnimation * mananimation=NULL;
	RoleNodeman   = (CCSprite *)this->getChildByTag(1000);//
	RoleNodewoman = (CCSprite *)this->getChildByTag(999);
	if (m_CurSelIndex==0) //男精灵
	{
		m_Info.Gender    = OBJ_G_MALE;
		mananimation = creatSpranimation(0);
		if  (mananimation)
		{
			actionManager->removeAllActionsFromTarget(RoleNodeman);
			RoleNodeman->runAction(CCSequence::create(CCAnimate::create(mananimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
		}
		m_Info.Gender    = OBJ_G_FEMALE;
		mananimation = creatSpranimation(2);
		if  (mananimation)
		{
			actionManager->removeAllActionsFromTarget(RoleNodewoman);
			RoleNodewoman->runAction(CCSequence::create(CCAnimate::create(mananimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
		}
	}
	else
	{//==1
		//女
		CCAnimation * womananimation=NULL;
		m_Info.Gender  = OBJ_G_FEMALE;
		womananimation = creatSpranimation(0);
		if  (womananimation)
		{
			actionManager->removeAllActionsFromTarget(RoleNodewoman);
			RoleNodewoman->runAction(CCSequence::create(CCAnimate::create(womananimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
		}

		m_Info.Gender    = OBJ_G_MALE;
		womananimation   = creatSpranimation(2);
		if  (womananimation)
		{
			actionManager->removeAllActionsFromTarget(RoleNodeman);
			RoleNodeman->runAction(CCSequence::create(CCAnimate::create(womananimation),CCCallFunc::create(this, callfunc_selector(KuiNewPlayer::finish)),NULL));
		}
	}
	//char msg[64];
	//t_sprintf(msg,"选择:%d",nSelSeries);
	//CCMessageBox(msg,"选中");
}

void KuiNewPlayer::mExit(CCObject* pSender){
	nPlaceId=-1;
	nNativePlaceId =-1;
	isthisVer = false;
	if (nAcclient)
	{//断开连接
		m_bIsClientConnecting=false;
		m_bIsPlayerInGame  = false;
		nAcclient->Cleanup();
	}

	if (g_pClient)
	{
		m_bIsPlayerInGame  = false;
		m_bIsGameSevConnecting=false;
		g_pClient->Cleanup();
		delete g_pClient;
		g_pClient=NULL;
	}
	//退到 开始登陆界面
	CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
}

void KuiNewPlayer::GetRoleImageName(char* pszName, const char* pszPrefix, unsigned char bGender, unsigned char bAttribute, int nIndex)
{
	//CCAssert(pszName && bAttribute >= series_metal && bAttribute < series_num,"");
	if  (!pszName || bAttribute < series_metal || bAttribute >= series_num)
		return;

	static const char* pszAttribute[series_num] = {"金","木","水","火","土"};
	static const char* pszGender[2] = { "男", "女" };
	t_sprintf(pszName,"%s_%s_%s_%d.spr", pszPrefix, pszAttribute[bAttribute],
		(bGender ? pszGender[1] : pszGender[0]), nIndex);  //spr格式 门派 人物spr  角色_系_性别_性别编码
}

void KuiNewPlayer::AcceptNetMsg(void* pMsgData) //循环接受处理消息
{
	if (pMsgData == NULL)  //数据包为空 则返回
		return;
	//CCMessageBox("回调成功","GetPackFromServer");
	switch(m_Status)       //发送数据包标识变量
	{
	case LL_S_ACCOUNT_CONFIRMING:  //账号验证
		//ProcessAccountLoginResponse((KLoginStructHead*) (((char*)pMsgData) + PROTOCOL_MSG_SIZE));
		break;
	case LL_S_WAIT_ROLE_LIST:      //等待角色清单
		//ProcessRoleListResponse((TProcessData*)pMsgData);   //接收协议角色信息
		break;
	case LL_S_CREATING_ROLE:       //创建角色
		ProcessCreateRoleResponse((tagNewDelRoleResponse*)pMsgData); //创建角色
		break;
	case LL_S_DELETING_ROLE:       //删除角色
		//ProcessDeleteRoleResponse((tagNewDelRoleResponse*)pMsgData);
		break;
	case LL_S_WAIT_TO_DEL:         //已经取消等待 删除角色
		//ProcessGetRoleResponse((tagNotifyPlayerLogin*)pMsgData);
		break;
	case LL_S_WAIT_TO_LOGIN_GAMESERVER:  //等待链接到服务器
		ProcessToLoginGameServResponse((tagNotifyPlayerLogin*)pMsgData); //调用链接到服务器的函数
		break;
	}
}

//--------------------------------------------------------------------------
//	功能：新建角色列表的响应  接BISHOP 返回的结果
//	状态切换：LL_S_CREATING_ROLE -> LL_S_ROLE_LIST_READY
//--------------------------------------------------------------------------
void KuiNewPlayer::ProcessCreateRoleResponse(tagNewDelRoleResponse* pResponse)
{
	//_ASSERT(m_Status == LL_S_DELETING_ROLE && pResponse != NULL);
	if (pResponse->cProtocol == s2c_rolenewdelresponse)
	{
		//	if (strcmp(pResponse->szRoleName, m_Choices.szProcessingRoleName) == 0)	//to be check/* 刘鹏调试版 */
		{//如果和本地的角色不相同 就直接退出
			if (pResponse->bSucceeded)
			{//等待进入游戏
				//g_NetConnectAgent.UpdateClientRequestTime(false);
				m_ClientChoices.bIsRoleNewCreated = true;
				m_Status = LL_S_WAIT_TO_LOGIN_GAMESERVER;
				m_Result = LL_R_NOTHING;
			}
			else
			{//创建失败 返回角色列表 还是直接退出？
				//g_NetConnectAgent.UpdateClientRequestTime(true);	//时间归零
				if (pLoginItem)
					pLoginItem->setEnabled(true);

				m_Status = LL_S_ROLE_LIST_READY;
				m_Result = LL_R_INVALID_ROLENAME;
#ifdef WIN32
				CCMessageBox("创建角色失败","提示:");
#else
				CCMessageBox(UTEXT("创建角色失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：准备登陆游戏服务器的响应---无限线程接包-有问题 空包
//	状态切换：LL_S_WAIT_TO_LOGIN_GAMESERVER -> LL_S_ENTERING_GAME
//--------------------------------------------------------------------------
void KuiNewPlayer::ProcessToLoginGameServResponse(tagNotifyPlayerLogin* pResponse)
{

	if (pResponse->cProtocol == s2c_notifypl_ayerlogin_hfkqotivbhd)  ///bishop 传过来的协议头
	{	
		char pzAc[32]={0};
		for (int k = 0; k < 32;k++)
		{
			pzAc[k] = ~m_ClientChoices.Account[k];
		}	

		if (!pResponse->bPermit &&(pResponse->nIPAddr==0 || pResponse->nPort==0))
		{
			m_Result = LL_R_OPNE_ACCOUNT; //卡号 正在尝试打开账号
#ifdef WIN32
			CCMessageBox("当前服务器处理预建立角色状态,不能登陆","提示:");
#else
			CCMessageBox(UTEXT("当前服务器处理预建立角色状态,不能登陆",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			return;
		}

		if (!pResponse->bPermit)
		{
			//ReturnToIdle();
			m_Result = LL_R_OPNE_ACCOUNT; //卡号 正在尝试打开账号
#ifdef WIN32
			CCMessageBox("正在尝试打开账号","提示:");
#else
			CCMessageBox(UTEXT("正在尝试打开账号",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			return;
		}

		if (!isthisVer)
		{
			m_Result = LL_R_INVALID_PROTOCOLVERSION; //卡号 正在尝试打开账号
#ifdef WIN32
			CCMessageBox("版本不对","提示:");
#else
			CCMessageBox(UTEXT("版本不对",1).c_str(),UTEXT("提示:",1).c_str());
#endif
			return;
		}

		if (strcmp((const char*)pResponse->szRoleName, m_ClientChoices.szProcessingRoleName) == 0 
			&& strcmp((const char*)pResponse->szAccountName, pzAc) == 0)
		{
			//g_NetConnectAgent.UpdateClientRequestTime(true); //时间归零
			if (NULL==g_pClient)
			{
				const size_t bufferSize           = 1024;   //Scoket决定发包的大小 分配的内存(m_bufferSize > 0) ? m_bufferSize : (1024*64);
				const size_t bufferSize_Cache     = 1024*512; //分配的内存 读包 接包的缓存大小
				const size_t maxFreeBuffers	      = 2;        //Scoket保留的数量
				const size_t maxFreeBuffers_Cache = 2;        //读包 接包的缓存 保留的数量
				g_pClient     = new CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize,1); //2,2   8
			}
			
		    if (g_pClient)
			{//(const unsigned char*)&pResponse->nIPAddr
				const unsigned char* pIpAddress = (const unsigned char*)&pResponse->nIPAddr;
				char Address[128]={0}; //IP地址
				t_sprintf(Address, "%d.%d.%d.%d", pIpAddress[0], pIpAddress[1],pIpAddress[2], pIpAddress[3]);
			    if (g_pClient->FsGameServerConnectTo(Address,pResponse->nPort))
				{
					g_pClient->Startup();
					m_bIsGameSevConnecting = true;	             //服务器已经链接
					
					tagLogicLogin ll;
					ll.cProtocol = c2s__loginfs_kfjghtueodnchsf; //通知GS服务器开始登陆	  赋值我网络号号给角色
					
					GUID* pGuid = &pResponse->guid;//服务器端唯一的标示
					memcpy(&ll.guid,pGuid,sizeof(GUID));

					if (!g_pClient->SendPackToServer(&ll,sizeof(tagLogicLogin)))
					{
						m_Result = LL_R_CONNECT_SERV_BUSY; //卡号 正在尝试打开账号
#ifdef WIN32
						CCMessageBox("连接服务器失败","提示:");
#else
                        CCMessageBox(UTEXT("连接服务器失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
						return;
					}
					//开始断开网关的连接
					if (nAcclient)
					{
						m_bIsClientConnecting =false;
						nAcclient->Cleanup(true); //只杀掉线程，不断开连接
						nAcclient->Shutdown();
						//nAcclient->Release();
						//delete nAcclient;
						//nAcclient = NULL;
					}

					/*if (nAcclient)
					{
						nAcclient->Cleanup();
						nAcclient->Release();
						nAcclient = NULL;
					}*/
					m_Status = LL_S_ENTERING_GAME;//正在进入游戏中
					m_Result = LL_R_NOTHING;  //连接成功了
				}
				else
				{//连接失败
					m_Result = LL_R_CONNECT_FAILED;
#ifdef WIN32
					CCMessageBox("连接服务器失败","提示:");
#else
                    CCMessageBox(UTEXT("连接服务器失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
				}
			}
			// 断开与网关的连接
			//g_NetConnectAgent.DisconnectClient();
		}
		else
		{
			//ReturnToIdle();
			m_Result = LL_R_SERVER_SHUTDOWN; //提示服务器满员或离线中
#ifdef WIN32
			CCMessageBox("连接服务器失败","提示:");
#else
            CCMessageBox(UTEXT("连接服务器失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		}
	}
}

CCAnimation* KuiNewPlayer::creatSpranimation(int nindex,int isLoop,bool isforever)
{
	char szFileName[128];
	int nFrams=0;
	char nInfoKey[32];
	t_sprintf(nInfoKey,"%d_%d_%d",m_Info.Attribute,m_Info.Gender,nindex);//五行,男女，那个动画
	CCAnimation*animation=NULL;
	animation = CCAnimationCache::sharedAnimationCache()->animationByName(nInfoKey);
	if (animation)
	{
		//CCMessageBox("存在","存在");
		return animation;
	}

	GetRoleImageName(szFileName, m_szPlayerImgPrefix,m_Info.Gender,m_Info.Attribute,nindex);
	g_StrLower(szFileName);
	_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
	char nySprFilePath[64]={0};
    animation = CCAnimation::create();//创建一个动画
	DWORD nFielpahtdwid = g_FileName2Id(szFileName);
	int j;
	for (j=0;j<nFrams;j++)
	{   
		t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
		animation->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
	}
	//设置动画帧的时间间隔
	animation->setDelayPerUnit(0.10f);
	//设置播放循环 一直播放 为-1
	animation->setLoops(isLoop);
	//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
	animation->setRestoreOriginalFrame(isforever);
	// 创建一个永远循环的动画
	//Roleanimatea = CCAnimate::create(animation);
	//加入动画缓存
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation,nInfoKey); //加入缓存
	return animation;
}