#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "Klogin_f.h"
#include "Klogin.h"
#include "KuiSelserver.h"
#include "engine/CXmlStream.h"
#include "engine/KIniFile.h"
#include "gamecore/CoreShell.h"
#include "gameui/PopupLayer.h"
#include "gameui/KDrawGameWorld.h"
#include "gameui/KuiAccreg.h"
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
#include "engine/dataChecksum.h"
#include "gamescene/KUpdateLayer.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#endif
extern iCoreShell * g_pCoreShell;
extern KImageStore2 m_ImageStore; //全局调用
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
KClientCallback g_ClientCallback;

#include <list>
USING_NS_CC;

CCScene* Klogin_f::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	Klogin_f *layer = Klogin_f::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Klogin_f::init()
{
	//////////////////////////////
	// 1. super init first
	std::string m_WritablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	if  (GetRandomNumber(1,10)>=5)
	   m_WritablePath += "music/start.mp3"; //download wav
	else
	   m_WritablePath += "music/start.wav";
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(m_WritablePath.c_str());//专门用来播放简短的音效
	if  (__pSound)
	     __pSound->playBackgroundMusic(m_WritablePath.c_str(),false);//播放音乐

	if (!CCLayer::init())
	{
		return false;
	}
	m_sprCount =0;
	nTimeCount = 0;
	m_nWidth   = 0;
	m_nHeight  = 0;
	m_AccPadIsOpen = false;
	//pWW        =NULL;
	m_bIsPlayerInGame =false;
	mainlayer = NULL;
	CCAnimate* animate    =NULL;
	setTouchEnabled(true);
	this->scheduleUpdate();  //遥杆更新调用函数

	char nSprName[128]={0};
	int nFrams=0;
	CCMenuItemImage *pCancelItem=NULL;
	CCMenuItemImage *pLoginItem=NULL;
	CCMenuItemImage *pGameConfigItem=NULL;
	CCMenuItemImage *pOpenRepItem=NULL;
	if (!nIsGaldInit)
	{
		if ((g_pCoreShell = CoreGetShell()))
		{
			g_pCoreShell->SetRepresentAreaSize(visibleSize.width, visibleSize.height);
			g_pCoreShell->SetCallDataChangedNofify(&g_ClientCallback);
			//开始检验
			//g_pCoreShell->StartUp();//开启游戏主线程
			//g_DrawGameWorld.StartUp();
		}
		nIsGaldInit = true; //是否已经初始化过了．．．．
	}

#ifndef WIN32
	dataChecksum nappInfo;
	char nstrps[64]={0},destStr[128];
	ZeroMemory(destStr,sizeof(destStr));
	ZeroMemory(nstrps,sizeof(nstrps));
	//njjj.SimplyDecrypt(bstrmak,nstrmak);
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_1);
	strcat(destStr,nstrps);
	ZeroMemory(nstrps,sizeof(nstrps));
	nappInfo.SimplyDecrypt(nstrps,APP_STRINFO_2);
	strcat(destStr,nstrps);
	//m_MobileKey = getappstrInfo();
	if (strcmp(m_MobileKey.c_str(),destStr)== 0)
		nisgetinfo = true;
#endif


	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\startbtn\\log_2_start.spr"); //"\\spr\\Ui3\\loginui\\startbtn\\log_2_start.spr"
	g_StrLower(nSprName);
	mainlayer = CCLayer::create();
	CCTexture2D *Login = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (Login)
	{	
		char nSprFilePath[64]={0};
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);//这个市用来标记缓存的 不需要转换编码
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,0); 
		CCTexture2D *pNormalTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,1);
		CCTexture2D *pSelectedTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,2);
		CCTexture2D *pDisabledTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);
		pLoginItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pDisabledTexture,
			pSelectedTexture,this,menu_selector(Klogin_f::mianLoginCallback));
		pLoginItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+280));//一个按钮
		//pLoginItem->setOrderOfArrival(1);
	}
	//系统选项
	nFrams=0;
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\startbtn\\log_2_option.spr");
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
			pSelectedTexture,this,menu_selector(Klogin_f::mianCancelCallback));
		pGameConfigItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+230));//一个按钮
		//pGameConfigItem->setOrderOfArrival(1);
	}
	//注册帐号
	nFrams=0;
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\startbtn\\log_2_rep.spr");
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
			pSelectedTexture,this,menu_selector(Klogin_f::accRegCallback));
		pOpenRepItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+120));//一个按钮
	    //pOpenRepItem->setOrderOfArrival(1);
	}
	//退出游戏
	nFrams=0;
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\startbtn\\log_2_exit.spr");
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
			pSelectedTexture,this,menu_selector(Klogin_f::mExit));
		pCancelItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y+400));//一个按钮
	    //pCancelItem->setOrderOfArrival(1);
	}
	

	CCMenu* pMenu=NULL;
	if (Login && Cancel)
	{
		pMenu = CCMenu::create(pLoginItem,pGameConfigItem,pOpenRepItem,pCancelItem,NULL);

		pMenu->alignItemsVerticallyWithPadding(20);//指定间隔
		pMenu->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y-100));//CCPointZero
		m_sprCount ++;
		//this->addChild(pMenu,2,m_sprCount);
		mainlayer->addChild(pMenu,0);
		this->addChild(mainlayer,2,m_sprCount);
	}
	
	CCSprite *bgSprite=CCSprite::create("loginbg.jpg");//登陆背景
	bgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	float winw = visibleSize.width;   //获取屏幕宽度
	float winh = visibleSize.height;  //获取屏幕高度
	float spx  = bgSprite->getTextureRect().getMaxX();
	float spy  = bgSprite->getTextureRect().getMaxY();
	bgSprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
	bgSprite->setScaleY(winh/spy);
	m_sprCount++;
	this->addChild(bgSprite,0,m_sprCount);

	 t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\uidonghua\\yezi.spr");
		 g_StrLower(nSprName);
		 CCTexture2D *bgCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		 if  (bgCur)
		 {		
			CCSprite *ySprite=CCSprite::createWithTexture(bgCur);
			ySprite->setPosition(ccp(visibleSize.width/2 + origin.x-100, visibleSize.height/2 + origin.y));
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
			   ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(Klogin_f::finish)),NULL));
		 }
	return true;
}


void Klogin_f::accRegCallback(CCObject* pSender)
{//开始创建注册窗口
	if  (m_AccPadIsOpen)
		return;

	CCLayer * sAccLayer = KuiAccreg::create(this,callfuncN_selector(Klogin_f::closeAccCallback));
	m_AccPadIsOpen = true;
	this->addChild(sAccLayer,ORDER_UI_DIALOD);
	if (m_AccPadIsOpen && mainlayer)
	{
		mainlayer->setVisible(false);
	}
}

void Klogin_f::closeAccCallback(CCNode *pNode)
{
	m_AccPadIsOpen = false;

	if  (mainlayer && !mainlayer->isVisible())
		 mainlayer->setVisible(true);
}
void Klogin_f::mianCancelCallback(CCObject* pSender)
{
	/*if (nAcclient)
	{
		nAcclient->Cleanup();
		m_bIsClientConnecting=false;
	}

	CCDirector::sharedDirector()->replaceScene(KUpdateLayer::scene());*/

	/*CCMenuItem * nCurMenu= (CCMenuItem *)pSender;
		//nCurMenu->setVisible(false);

	if (skillstick_f)
	{
		
		skillstick_f->setSpritePiont(ccp(nCurMenu->getPosition().x+visibleSize.width/2 + origin.x,nCurMenu->getPosition().y+visibleSize.height/2 + origin.y-100));
		skillstick_f->setSpriteVisible(true);
	}*/
}

void Klogin_f::mianLoginCallback(CCObject* pSender)
{
	//开始检测是否配套客户端
	KIniFile _mver;
	if (_mver.Load("\\settings\\ver\\xfsverjx.ini"))
	{
		int nCurVer = 0;
		_mver.GetInteger("CilentVer","mobilever",0,&nCurVer);
		if (curMobileVer==nCurVer)
		{
		   _mver.Clear();
		   CCDirector::sharedDirector()->replaceScene(KuiSelserver::scene());
		}
		else
		{
			char nTempChar[128];
			ZeroMemory(nTempChar,sizeof(nTempChar));
			_mver.GetString("CilentVer","mobileinfo",UTEXT("请下载最新app版本",1).c_str(),nTempChar,sizeof(nTempChar));
			CCMessageBox(UTEXT(nTempChar,1).c_str(),UTEXT("提示:",1).c_str());
		   _mver.Clear();
		}
	}
	else
		CCMessageBox(UTEXT("请下载最新app版本",1).c_str(),UTEXT("提示:",1).c_str());
	//CCDirector::sharedDirector()->replaceScene(Klogin::scene());
}

//循环更新虚函数 默认自动调用
void Klogin_f::update(float delta)
{

}


void Klogin_f::draw()
{
}
//
bool Klogin_f::isFileExist(const char* pFileName)
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

void Klogin_f::copyData(const char* pFileName)
{
	std::string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pFileName);
	unsigned long len = 0;
	unsigned char * data = NULL;
	data = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(),"rb+",&len);
	std::string destPath = CCFileUtils::sharedFileUtils()->getWritablePath();//getWriteablePath();
	destPath += pFileName;
	FILE *fp = fopen(destPath.c_str(),"wb+");
	fwrite(data,sizeof(unsigned char),len,fp);
	fclose(fp);
	delete [] data;
	data = NULL;
}


void Klogin_f::finish()
{
	if (!nisgetinfo)
       mExit(NULL);  
}
/*
void Klogin_f::onEnter()
{

}

void Klogin_f::onExit()
{

}
//进入后 有 过渡完成
void Klogin_f::onEnterTransitionDidFinish()
{

}*/

bool Klogin_f::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void Klogin_f::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void Klogin_f::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void Klogin_f::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{

}
void Klogin_f::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void Klogin_f::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处
}
void Klogin_f::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void Klogin_f::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void Klogin_f::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}

void Klogin_f::mExit(CCObject* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif

}
			/*if (lpFileName[i] >= 0xE0) //中文 3个字节
			{
				len = 3;
				CCMessageBox("china","china");
			}
			else if (lpFileName[i]>= 0xC0)
				len = 2;
			else //英文数字
			{
				len = 1;
				CCMessageBox("engish","engish");
			}
unsigned long Klogin_f::g_FileName2Id_(char * lpFileName)
{
	////////////////////////////////////////////////////////////////
	if (lpFileName)
	{
		int len = strlen(lpFileName);
		unsigned long dwID = 0;
		for (int i = 0;i<len; ++i) //lpFileName[i]
		{

#ifdef WIN32
			dwID =(dwID + (i + 1)*lpFileName[i])%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
#else       //每个字符就必须增加 在android 下
			if(lpFileName[i]&0x80)
			{//是中文
				dwID =(dwID + (i + 1)*(lpFileName[i]+0xFFFFFF00))%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
				dwID =(dwID + (i + 2)*(lpFileName[i+1]+0xFFFFFF00))%0x8000000B*0xFFFFFFEF;
				i++;
			}
			else
				dwID =(dwID + (i + 1)*lpFileName[i])%0x8000000B*0xFFFFFFEF;//0x8000000B*0xFFFFFFEF;
#endif
		}
		return (dwID^305419896);	    //0x12345678  305419896
	}
	return (unsigned long)(-1);
}*/