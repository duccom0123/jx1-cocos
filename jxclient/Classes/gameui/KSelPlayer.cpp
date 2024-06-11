#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "KSelPlayer.h"
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

CCScene* KSelPlayer::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	KSelPlayer *layer = KSelPlayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool KSelPlayer::init()
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
	CCAnimate* Roleanimatea    =NULL;
	CCAnimate* Roleanimateb    =NULL;
	CCAnimate* Roleanimatec    =NULL;
	SelbgSprite =NULL;
	setTouchEnabled(true);

	m_Status = LL_S_ROLE_LIST_READY; //等待角色进入游戏服务器
	m_Result = LL_R_NOTHING;
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	//CCMenuItemImage *pCloseItem = CCMenuItemImage::createWithTexture();
	char nSprName[128]={0};
	int nFrams=0;
	m_CurSelIndex = 0;
	CCMenuItemImage *pCancelItem=NULL;
	CCMenuItemImage *pLoginItem=NULL;
	CCMenuItemImage *pGameConfigItem=NULL;
	CCMenuItemImage *pOpenRepItem=NULL;
	//CCLayer *mainlayer = CCLayer::create();
	actionManager = CCDirector::sharedDirector()->getActionManager();
	pRoela = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);
	pRoelb = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);
	pRoelc = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);

	//float m_Scalex,m_Scaley; //与背景 屏幕大小的缩放比例
	//登陆选择框 \Spr\Ui3\登入界面\选存档人物\角色
	t_sprintf(m_szPlayerImgPrefix,"\\Spr\\Ui3\\登入界面\\选存档人物\\角色"); //"\\Spr\\Ui3\\登入界面\\选存档人物\\角色"  "\\spr\\Ui3\\loginui\\saverole\\role"
	//t_sprintf(m_szPlayerImgPrefix,"\\spr\\Ui3\\loginui\\saverole\\role"); 
	//t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\登陆界面_9_长边框.spr");//"\\Spr\\Ui3\\登入界面\\登陆界面_9_长边框.spr"  "\\spr\\Ui3\\loginui\\longin_9_big_b.spr"
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\longin_9_big_b.spr");
	g_StrLower(nSprName);
	CCTexture2D *SelCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (SelCur)
	{
		SelbgSprite=CCSprite::createWithTexture(SelCur);
		SelbgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+50));
		/*float winw = visibleSize.width;   //获取屏幕宽度
		float winh = visibleSize.height;  //获取屏幕高度
		float spx  = SelbgSprite->getTextureRect().getMaxX();
		float spy  = SelbgSprite->getTextureRect().getMaxY();
		m_Scalex   = winw/spx;
		m_Scaley   = winh/spy;
		SelbgSprite->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		SelbgSprite->setScaleY(m_Scaley);*/
		m_sprCount++;
		this->addChild(SelbgSprite,1,m_sprCount);
	}
	//确认按钮
	nFrams=0; //"\\spr\\Ui3\\loginui\\saverole\\ok_9_btn.spr"
	//t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_确定.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_确定.spr");
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\saverole\\ok_9_btn.spr");
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
			pSelectedTexture,this,menu_selector(KSelPlayer::mainEnterCallback));
		//pLoginItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
		//pLoginItem->setOrderOfArrival(1);
		//pLoginItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pLoginItem->setScaleY(m_Scaley);
		//pLoginItem->setEnabled(false);
	}
	//新建角色
	nFrams=0;
	//t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_新建角色.spr");//"\\spr\\Ui3\\loginui\\saverole\\newrole_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_新建角色.spr");
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\saverole\\newrole_9_btn.spr");
	g_StrLower(nSprName);
	CCTexture2D *GameConfig = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (GameConfig)
	{
		char nSprFilePath[64]={0};
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
		CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
		CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,2);
		CCTexture2D *pDisabledTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		pGameConfigItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pDisabledTexture,
			pSelectedTexture,this,menu_selector(KSelPlayer::mainNewCallback));
		//pGameConfigItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
		//pGameConfigItem->setOrderOfArrival(1);
		//pGameConfigItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pGameConfigItem->setScaleY(m_Scaley);
	}
	//删除角色
	nFrams=0;
	//t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_删除角色.spr");//"\\spr\\Ui3\\loginui\\saverole\\delrole_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_删除角色.spr");
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\saverole\\delrole_9_btn.spr");
	g_StrLower(nSprName);
	CCTexture2D *Rep = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (Rep)
	{
		char nSprFilePath[64]={0};
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
		CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
		CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,2);
		CCTexture2D *pDisabledTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		pOpenRepItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pDisabledTexture,
			pSelectedTexture,this,menu_selector(KSelPlayer::mainDelCallback));
		//pOpenRepItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
	    //pOpenRepItem->setOrderOfArrival(1);
		//pOpenRepItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pOpenRepItem->setScaleY(m_Scaley);
	}
	//取消 退到登陆界面
	nFrams=0;
	//t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");
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
			pSelectedTexture,this,menu_selector(KSelPlayer::mExit));
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
			   ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
		 }

			// create menu, it's an autorelease object
		CCMenu* pMenu=NULL;
		if (Login && Cancel)
		{
				/*pLoginItem->setPositionX(testSprite->getPositionX()-75);
				pLoginItem->setPositionY(testSprite->getPositionY()-80); //确认按钮
				pCancelItem->setPositionX(testSprite->getPositionX()+75);//改变按钮
				pCancelItem->setPositionY(testSprite->getPositionY()-80);*/
			pMenu = CCMenu::create(pLoginItem,pGameConfigItem,pOpenRepItem,pCancelItem,NULL);
				//pMenu->alignItemsInRows(1,2,3,4,NULL);     //四列一行
				//pMenu->alignItemsVertically();             //竖排排列 alignItemsHorizontally
			pMenu->alignItemsHorizontally();             //横排排列 默认间隔是kDefaultPadding=5  
			pMenu->alignItemsHorizontallyWithPadding(5);//指定横间隔  
				//pMenu->alignItemsVertically();             //默认间隔也是kDefaultPadding=5 
				//pMenu->alignItemsInRows(3,2,1,NULL);   
				//pMenu->alignItemsVerticallyWithPadding(20);//指定间隔
			pMenu->setPosition(ccp(246,40));//CCPointZero
			m_sprCount ++;
			SelbgSprite->addChild(pMenu,2,m_sprCount);
		}
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

		if (m_nNumRole>0)
		{//如果角色大于零的 就开始创建精灵
			static const char* pszAttribute[series_num] = {"金","木","水","火","土"};
			static const char* pszGender[2] = { "男", "女" };
			for (int i = 0; i < m_nNumRole; ++i)
			{
				char nInfo[64]={0};
				char szFileName[128]={0};
				t_sprintf(nCurRoleList[i].Name,m_RoleList[i].Name);       //姓名
				nCurRoleList[i].nLevel    = m_RoleList[i].nLevel;
				nCurRoleList[i].Attribute = m_RoleList[i].Attribute;
				nCurRoleList[i].Gender    = m_RoleList[i].Gender;
				t_sprintf(nInfo,"%d 级",m_RoleList[i].nLevel);             //等级
				m_Gender[i]               = m_RoleList[i].Gender;        //性别
				m_Attribute[i]            = m_RoleList[i].Attribute;     //五行属性
				GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute,2);//2 灰色动画 0选中 1 为一性过渡动画
				nFrams=0;
				g_StrLower(szFileName);
				//角色1
				if (i == 0)
				{
					CCTexture2D *RoleCura = _getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCSprite *RoleSpritea = NULL;
					if (RoleCura)
					{ 	
						RoleSpritea=CCSprite::createWithTexture(RoleCura);
						RoleSpritea->setPosition(ccp(visibleSize.width/2 + origin.x-210, visibleSize.height/2 + origin.y+40));
						//RoleSpritea->setScaleX(m_Scalex); //设置精灵宽度缩放比例
						//RoleSpritea->setScaleY(m_Scaley); SelbgSprite
						m_sprCount++;
						this->addChild(RoleSpritea,2,1000);
						CCString* msg = CCString::createWithFormat("%s\n %s,%s,%s",UTEXT(nCurRoleList[i].Name,1).c_str(),pszGender[m_Gender[i]],UTEXT(nInfo,1).c_str(),pszAttribute[m_Attribute[i]]);
						pRoela->setPosition(ccp(visibleSize.width/2 + origin.x-210, visibleSize.height/2 + origin.y-175));
						pRoela->setString(msg->getCString());
						ccColor3B nColor={255,255,0};
						pRoela->setColor(nColor);
						this->addChild(pRoela,2);
					}
					char nySprFilePath[64]={0};
					CCAnimation*animation = CCAnimation::create();//创建一个动画
					DWORD nFielpahtdwid = g_FileName2Id(szFileName);
					int j;
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
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
					Roleanimatea = CCAnimate::create(animation);
					//加入动画缓存
					//t_sprintf(nySprFilePath,"%u",nFielpahtdwid);
					CCAnimationCache::sharedAnimationCache()->addAnimation(animation,"0_2");
					//CCRepeatForever* forever = CCRepeatForever::create(animate);

					GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute, 0);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationb = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationb->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationb->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationb->setLoops(-1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationb->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					//t_sprintf(nySprFilePath,"%u",nFielpahtdwid);
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationb,"0_0");

					GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute, 1);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationc = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationc->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationc->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationc->setLoops(1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationc->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationc,"0_1");

					if (RoleSpritea)
						RoleSpritea->runAction(CCSequence::create(Roleanimatea,CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));

					//RoleSpritea->getBoundingBox();
					//CCMenuItemSprite* sprite2 = CCMenuItemSprite::create(RoleSpritea, RoleSpritea, this, menu_selector(KuiSelNativePlace::finish));        
					//CCMenu* dddmenu = CCMenu::create(sprite2, NULL);
					//dddmenu->alignItemsVerticallyWithPadding(100);
					//dddmenu->setPosition(ccp(size.width / 2, size.height / 2));
				}
				//角色2
				if (i == 1)
				{
					CCTexture2D *RoleCurb = _getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCSprite *RoleSpriteb = NULL;
					if (RoleCurb)
					{ 	
						RoleSpriteb=CCSprite::createWithTexture(RoleCurb);
						RoleSpriteb->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+40));
						//RoleSpriteb->setScaleX(m_Scalex); //设置精灵宽度缩放比例
						//RoleSpriteb->setScaleY(m_Scaley);
						m_sprCount++;
						this->addChild(RoleSpriteb,2,999);
						CCString* msg = CCString::createWithFormat("%s\n %s,%s,%s",UTEXT(nCurRoleList[i].Name,1).c_str(),pszGender[m_Gender[i]],UTEXT(nInfo,1).c_str(),pszAttribute[m_Attribute[i]]);
						pRoelb->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y-175));
						pRoelb->setString(msg->getCString());
						ccColor3B nColor={255,255,0};
						pRoelb->setColor(nColor);
						this->addChild(pRoelb,2);
					}
					char nySprFilePath[64]={0};
					CCAnimation*animation = CCAnimation::create();//创建一个动画
					DWORD nFielpahtdwid = g_FileName2Id(szFileName);
					int j;
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
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
					Roleanimateb = CCAnimate::create(animation);
					//加入动画缓存
					CCAnimationCache::sharedAnimationCache()->addAnimation(animation,"1_2");
					//CCRepeatForever* forever = CCRepeatForever::create(animate);

					GetRoleImageName(szFileName,m_szPlayerImgPrefix, m_RoleList[i].Gender,m_RoleList[i].Attribute,0);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationb = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationb->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationb->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationb->setLoops(-1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationb->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					//t_sprintf(nySprFilePath,"%u",nFielpahtdwid);
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationb,"1_0");

					GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute,1);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationc = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationc->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationc->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationc->setLoops(1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationc->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationc,"1_1");

					if (RoleSpriteb)
						RoleSpriteb->runAction(CCSequence::create(Roleanimateb,CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
				}
				//角色3
				if (i == 2)
				{
					CCTexture2D *RoleCurc = _getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCSprite *RoleSpritec = NULL;
					if (RoleCurc)
					{ 	
						RoleSpritec=CCSprite::createWithTexture(RoleCurc);
						RoleSpritec->setPosition(ccp(visibleSize.width/2 + origin.x+190, visibleSize.height/2 + origin.y+40));
						//RoleSpritec->setScaleX(m_Scalex); //设置精灵宽度缩放比例
						//RoleSpritec->setScaleY(m_Scaley);
						m_sprCount++;
						this->addChild(RoleSpritec,2,998);
						CCString* msg = CCString::createWithFormat("%s\n %s,%s,%s",UTEXT(nCurRoleList[i].Name,1).c_str(),pszGender[m_Gender[i]],UTEXT(nInfo,1).c_str(),pszAttribute[m_Attribute[i]]);
						pRoelc->setPosition(ccp(visibleSize.width/2 + origin.x+190, visibleSize.height/2 + origin.y-175));
						pRoelc->setString(msg->getCString());
						ccColor3B nColor={255,255,0};
						pRoelc->setColor(nColor);
						this->addChild(pRoelc,2);
					}
					//CCRect::
					char nySprFilePath[64]={0};
					CCAnimation*animation = CCAnimation::create();//创建一个动画
					DWORD nFielpahtdwid = g_FileName2Id(szFileName);
					int j;
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
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
					Roleanimatec = CCAnimate::create(animation);
					//加入动画缓存
					CCAnimationCache::sharedAnimationCache()->addAnimation(animation,"2_2");
					//CCRepeatForever* forever = CCRepeatForever::create(animate);

					GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute, 0);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationb = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationb->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationb->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationb->setLoops(-1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationb->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					//t_sprintf(nySprFilePath,"%u",nFielpahtdwid);
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationb,"2_0");

					GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_RoleList[i].Gender, m_RoleList[i].Attribute, 1);
					g_StrLower(szFileName);
					_getinidata.getinidata_new(szFileName,0,&m_nWidth,&m_nHeight,&nFrams);
					CCAnimation*animationc = CCAnimation::create();//创建一个动画
					nFielpahtdwid = g_FileName2Id(szFileName);
					for (j=0;j<nFrams;j++)
					{   
						t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,j);  
						//CCMessageBox(nSprFilePath,"animation");
						animationc->addSpriteFrameWithSprName(nySprFilePath);//往这个动画增加纹理帧
					}
					//设置动画帧的时间间隔
					animationc->setDelayPerUnit(0.10f);
					//设置播放循环 一直播放 为-1
					animationc->setLoops(1);
					//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
					animationc->setRestoreOriginalFrame(TRUE);
					// 创建一个永远循环的动画
					//Roleanimatea = CCAnimate::create(animationb);
					//加入动画缓存
					CCAnimationCache::sharedAnimationCache()->addAnimation(animationc,"2_1");

					if (RoleSpritec)
						RoleSpritec->runAction(CCSequence::create(Roleanimatec,CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
				}
			}
		}

	//DrawNode* drawNode = ccDrawNode::create();
	this->scheduleUpdate();  //遥杆更新调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}
//新建立角色
void KSelPlayer::mainNewCallback(CCObject* pSender)
{
	int nSel;
	if (GetRoleCount(nSel) < MAX_PLAYER_PER_ACCOUNT)
	{//打开新建立角色窗口
		CCDirector::sharedDirector()->replaceScene(KuiSelNativePlace::scene());
	}
}

int KSelPlayer::GetRoleCount(int& nAdviceChoice)
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
//删除角色
void KSelPlayer::mainDelCallback(CCObject* pSender)
{
	/*if (nAcclient)
	{
		nAcclient->Cleanup();
		m_bIsClientConnecting=false;
	}

	CCDirector::sharedDirector()->replaceScene(KUpdateLayer::scene());*/
}
//进入游戏
void KSelPlayer::mainEnterCallback(CCObject* pSender)
{//
	if (nAcclient/* && g_pClient*/)
	{
		if (pSender)
			((CCMenuItemImage *)pSender)->setEnabled(false);

		EnterGame();
	}
}

//循环更新虚函数 默认自动调用
void KSelPlayer::update(float delta)
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
			if  (Msg == s2c_login || Msg == s2c_roleserver_getrolelist_result || Msg == s2c_notifypl_ayerlogin_hfkqotivbhd || Msg == s2c_rolenewdelresponse)
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

CCRect KSelPlayer::getRect(CCNode* pNode)
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
void KSelPlayer::draw()
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
		if  (m_Result==LL_R_INVALID_PROTOCOLVERSION)
		{//版本不队
#ifdef WIN32
			CCMessageBox("版本不对","提示:");
#else
			CCMessageBox(UTEXT("版本不对",1).c_str(),UTEXT("提示:",1).c_str());
#endif
		}
		CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		return;
	}

	if (m_Status == LL_S_ENTERING_GAME)
	{//马上跳转画面
		//CCDirector::sharedDirector()->replaceScene(KTranSence::scene());
		CCDirector::sharedDirector()->replaceScene(KgameWorld::scene());
		return;
	}
}
//
bool KSelPlayer::isFileExist(const char* pFileName)
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

void KSelPlayer::copyData(const char* pFileName)
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
void KSelPlayer::onEnter()
{

}

void KSelPlayer::onExit()
{

}
//进入后 有 过渡完成
void KSelPlayer::onEnterTransitionDidFinish()
{

}*/

bool KSelPlayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void KSelPlayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void KSelPlayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void KSelPlayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	if (clicked)
	{
		clicked = false;
		CCMessageBox("双击","警告");
	}
	else
	{
		//延时0.25s执行（注意在这0.25s的延时过程中clicked已经为true），  
		//如果在这个过程中再次click，那么就执行上面的双击事件处理了  
		//否则，那么就执行下面的回调函数了，处理单击事件 
		scheduleOnce(schedule_selector(KSelPlayer::CheckClick),0.25f);
		clicked =false;
		//定时器
		//this->schedule(schedule_selector(LoginScenes::movebd)); 
	}
}

void KSelPlayer::CheckClick(float tt)
{
	if (clicked) 
	{//单击
	   clicked = false;  
	   CCMessageBox("单击","警告");
	}  
}

void KSelPlayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void KSelPlayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处
	RoleNodea = (CCSprite *)this->getChildByTag(1000);
	RoleNodeb = (CCSprite *)this->getChildByTag(999);
	RoleNodec = (CCSprite *)this->getChildByTag(998);
	char nRoleInfo[128]={0};
	if (RoleNodea)
	{
		if (getRect(RoleNodea).containsPoint(location))
		{
			m_CurSelIndex = 0;
			if (m_CurSelIndex == 0)
			{//原来就等于0 灰色的 
				actionManager->removeAllActionsFromTarget(RoleNodea);
				//RoleNodea->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1"))));
				RoleNodea->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1")),CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
				if (RoleNodeb)
				{
					actionManager->removeAllActionsFromTarget(RoleNodeb);
					RoleNodeb->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("1_2")),NULL,NULL));
				}
				if (RoleNodec)
				{
					actionManager->removeAllActionsFromTarget(RoleNodec);
					RoleNodec->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("2_2")),NULL,NULL));
				}
			}  
			//EnterGame();
		}
	}

	
	if (RoleNodeb)
	{
		if (getRect(RoleNodeb).containsPoint(location))
		{
			m_CurSelIndex = 1;
			if (m_CurSelIndex == 1)
			{//原来就等于0 灰色的 
				actionManager->removeAllActionsFromTarget(RoleNodeb);
				//RoleNodea->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1"))));
				RoleNodeb->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("1_1")),CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
				if (RoleNodea)
				{
					actionManager->removeAllActionsFromTarget(RoleNodea);
					RoleNodea->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_2")),NULL,NULL));
				}
				if (RoleNodec)
				{
					actionManager->removeAllActionsFromTarget(RoleNodec);
					RoleNodec->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("2_2")),NULL,NULL));
				}
			} 
			
			//EnterGame();
		}
	}

	
	if (RoleNodec)
	{
		if (getRect(RoleNodec).containsPoint(location))
		{  
			m_CurSelIndex = 2;
			if (m_CurSelIndex == 2)
			{//原来就等于0 灰色的 
				actionManager->removeAllActionsFromTarget(RoleNodec);
				//RoleNodea->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_1"))));
				RoleNodec->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("2_1")),CCCallFunc::create(this, callfunc_selector(KSelPlayer::finish)),NULL));
				if (RoleNodea)
				{
					actionManager->removeAllActionsFromTarget(RoleNodea);
					RoleNodea->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("0_2")),NULL,NULL));
				}
				if (RoleNodeb)
				{
					actionManager->removeAllActionsFromTarget(RoleNodeb);
					RoleNodeb->runAction(CCSequence::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("1_2")),NULL,NULL));
				}
			} 
			//EnterGame();
		}
	}
}

void KSelPlayer::finish()
{
	CCAnimation* nCurAnimat = NULL;
	if  (m_CurSelIndex==0)
	{
		//actionManager->removeAllActionsFromTarget(RoleNodea);
		nCurAnimat= CCAnimationCache::sharedAnimationCache()->animationByName("0_0");
		if (nCurAnimat)
		    RoleNodea->runAction(CCSequence::create(CCAnimate::create(nCurAnimat),NULL,NULL));
	}

	if  (m_CurSelIndex==1)
	{
		//actionManager->removeAllActionsFromTarget(RoleNodeb);
		nCurAnimat= CCAnimationCache::sharedAnimationCache()->animationByName("1_0");
		if (nCurAnimat)
		   RoleNodeb->runAction(CCSequence::create(CCAnimate::create(nCurAnimat),NULL,NULL));
	}

	if  (m_CurSelIndex==2)
	{
		//actionManager->removeAllActionsFromTarget(RoleNodec);
		nCurAnimat= CCAnimationCache::sharedAnimationCache()->animationByName("2_0");
		if (nCurAnimat)
		   RoleNodec->runAction(CCSequence::create(CCAnimate::create(nCurAnimat),NULL,NULL));
	}

}

void KSelPlayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void KSelPlayer::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void KSelPlayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}

void KSelPlayer::mExit(CCObject* pSender){

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

void KSelPlayer::GetRoleImageName(char* pszName, const char* pszPrefix, unsigned char bGender, unsigned char bAttribute, int nIndex)
{
	//CCAssert(pszName && bAttribute >= series_metal && bAttribute < series_num,"");
	if  (!pszName || bAttribute < series_metal || bAttribute >= series_num)
		return;

	static const char* pszAttribute[series_num] = {"金","木","水","火","土"};
	static const char* pszGender[2] = { "男", "女" };
	t_sprintf(pszName,"%s_%s_%s_%d.spr", pszPrefix, pszAttribute[bAttribute],
		(bGender ? pszGender[1] : pszGender[0]), nIndex);  //spr格式 门派 人物spr  角色_系_性别_性别编码
}

void KSelPlayer::AcceptNetMsg(void* pMsgData) //循环接受处理消息
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
		//ProcessCreateRoleResponse((tagNewDelRoleResponse*)pMsgData); //创建角色
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
//	功能：准备登陆游戏服务器的响应---无限线程接包-有问题 空包
//	状态切换：LL_S_WAIT_TO_LOGIN_GAMESERVER -> LL_S_ENTERING_GAME
//--------------------------------------------------------------------------
void KSelPlayer::ProcessToLoginGameServResponse(tagNotifyPlayerLogin* pResponse)
{
	//CCMessageBox("收包成功","提示");
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
			CCMessageBox("正在尝试打开账号","提示:");
			return;
		}
		//开始检查预备建立角色
		/*if (pInfo->ProtocolVersion==0)
			{
#ifdef WIN32
				CCMessageBox("当前服务器处理预建立角色状态,不能登陆","提示:");
#else
				CCMessageBox(UTEXT("当前服务器处理预建立角色状态,不能登陆",1).c_str(),UTEXT("提示:",1).c_str());
#endif
				return;
			}*/
		if (strcmp((const char*)pResponse->szRoleName, m_ClientChoices.szProcessingRoleName) == 0 
			&& strcmp((const char*)pResponse->szAccountName, pzAc) == 0)
		{
			    // g_NetConnectAgent.UpdateClientRequestTime(true); //时间归零
			if (NULL==g_pClient)
			{
				const size_t bufferSize           = 1024;   //Scoket决定发包的大小 分配的内存(m_bufferSize > 0) ? m_bufferSize : (1024*64);
				const size_t bufferSize_Cache     = 1024*512; //分配的内存 读包 接包的缓存大小
				const size_t maxFreeBuffers	      = 2;        //Scoket保留的数量
				const size_t maxFreeBuffers_Cache = 2;        //读包 接包的缓存 保留的数量
				g_pClient     = new CGameClient(maxFreeBuffers,maxFreeBuffers_Cache,bufferSize_Cache,bufferSize,1); //2,2   8
			}
			if (g_pClient)
			{   // (const unsigned char*)&pResponse->nIPAddr
				const unsigned char* pIpAddress = (const unsigned char*)&pResponse->nIPAddr;
				char Address[128]={0}; //IP地址
				t_sprintf(Address, "%d.%d.%d.%d", pIpAddress[0], pIpAddress[1],pIpAddress[2], pIpAddress[3]);
			    //  CCMessageBox(Address,"开始连接服务器..!");
				if (g_pClient->FsGameServerConnectTo(Address,pResponse->nPort))
				{
				//	CCMessageBox(Address,"连接服务器成功!");
					g_pClient->Startup();
					m_bIsGameSevConnecting = true;	             //服务器已经链接
					g_pClient->setSocketStates(true);            //设置已经连接
					tagLogicLogin ll;
					ll.cProtocol = c2s__loginfs_kfjghtueodnchsf; //通知GS服务器开始登陆	  赋值我网络号号给角色
					
					GUID* pGuid = &pResponse->guid;//服务器端唯一的标示
					memcpy(&ll.guid,pGuid,sizeof(GUID));

					if (!g_pClient->SendPackToServer(&ll,sizeof(tagLogicLogin)))
					{
						m_Result = LL_R_CONNECT_SERV_BUSY; //卡号 正在尝试打开账号
						g_pClient->Cleanup();
						CCMessageBox("连接服务器失败","提示:");
						return;
					}
					//开始断开网关的连接
					if (nAcclient)
					{
						m_bIsClientConnecting =false;
						nAcclient->Cleanup(true); //只杀掉线程，不断开连接
						nAcclient->Shutdown();
					}
					m_Status = LL_S_ENTERING_GAME;//正在进入游戏中
					m_Result = LL_R_NOTHING;      //连接成功了
				}
				else
				{//连接失败
					m_Result = LL_R_CONNECT_FAILED;
					CCMessageBox("连接服务器失败","提示:");
				}
			}
			// 断开与网关的连接
			//g_NetConnectAgent.DisconnectClient();
		}
		else
		{
			//ReturnToIdle();
			m_Result = LL_R_SERVER_SHUTDOWN; //提示服务器满员或离线中
			CCMessageBox("连接服务器失败","提示:");
		}
	}
}

void KSelPlayer::EnterGame()
{
	if (!isthisVer)
	{
		m_Result = LL_R_INVALID_PROTOCOLVERSION;
		return;
	}
	if (m_bIsClientConnecting && nAcclient)
	{//帐号服务器如果是可以用的
		if (m_Status == LL_S_ROLE_LIST_READY && m_CurSelIndex >= 0 && m_CurSelIndex < m_nNumRole)
		{
			//CCMessageBox("EnterGame","2222");
			tagDBSelPlayer	NetCommand;
			NetCommand.cProtocol = c2s_dbplayerselect;
			strcpy(NetCommand.szRoleName, nCurRoleList[m_CurSelIndex].Name);
			nAcclient->SendPackToServer(&NetCommand, sizeof(tagDBSelPlayer));
			//g_NetConnectAgent.UpdateClientRequestTime(false);
			strcpy(m_ClientChoices.szProcessingRoleName,nCurRoleList[m_CurSelIndex].Name);
			m_Status = LL_S_WAIT_TO_LOGIN_GAMESERVER;	 //链接服务器
			m_Result = LL_R_NOTHING;
		}
		else
		{
			m_Result = LL_R_CONNECT_TIMEOUT;
			//m_Result = LL_R_CONNECT_FAILED; //链接失败！
		}
	}
}