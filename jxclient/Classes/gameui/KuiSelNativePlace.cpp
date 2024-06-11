#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "KuiSelNativePlace.h"
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
#include "engine/Text.h"
#include "gamecore/KNpc.h"
#include "gamecore/KNpcSet.h"
#include "gamecore/KPlayer.h"
#include "gamecore/KPlayerSet.h"
#include "gamecore/KSubWorldSet.h"
#include "gamescene/KUpdateLayer.h"
#include "gameui/KuiNewPlayer.h"
extern iCoreShell*	g_pCoreShell; //全局调用
extern KImageStore2 m_ImageStore; //全局调用
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
#include <list>
USING_NS_CC;
KuiSelNativePlace::KuiSelNativePlace()
{
	m_pPlaceList = NULL;
	showSprite   = NULL;
	m_nLastSelPlace = 0;
}

KuiSelNativePlace::~KuiSelNativePlace()
{
	if (m_pPlaceList)
	{
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
}

CCScene* KuiSelNativePlace::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	KuiSelNativePlace *layer = KuiSelNativePlace::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool KuiSelNativePlace::init()
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
	m_selIndex = 0;
	clicked    = false;
	//pWW        =NULL;
	CCAnimate* animate         =NULL;
	CCAnimate* Roleanimatea    =NULL;
	CCAnimate* Roleanimateb    =NULL;
	CCAnimate* Roleanimatec    =NULL;
	setTouchEnabled(true);
	this->scheduleUpdate();  //遥杆更新调用函数
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	m_Status = LL_S_ROLE_LIST_READY; //等待角色进入游戏服务器
	m_Result = LL_R_NOTHING;

	char nSprName[128]={0};
	int nFrams=0;
	m_CurSelIndex = 0;
	CCMenuItemImage *pCancelItem=NULL;
	CCMenuItemImage *pLoginItem=NULL;
	SelbgSprite = NULL;
	//CCLayer *mainlayer = CCLayer::create(); \Spr\Ui3\登入界面\登陆界面_8_选择新手村.spr
	actionManager = CCDirector::sharedDirector()->getActionManager();
	
	pDescLabel = CCLabelTTF::create("", UI_GAME_FONT_DEFAULT, 12);

	//float m_Scalex,m_Scaley; //与背景 屏幕大小的缩放比例
	//登陆选择框 \Spr\Ui3\登入界面\选存档人物\角色
	t_sprintf(m_szPlayerImgPrefix,"\\Spr\\Ui3\\登入界面\\选存档人物\\角色"); //"\\Spr\\Ui3\\登入界面\\选存档人物\\角色"  "\\spr\\Ui3\\loginui\\saverole\\role"
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\登陆界面_8_选择新手村.spr");//"\\Spr\\Ui3\\登入界面\\登陆界面_9_长边框.spr"  "\\spr\\Ui3\\loginui\\longin_9_big_b.spr"
	g_StrLower(nSprName);
	CCTexture2D *SelCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	if (SelCur)
	{
		SelbgSprite=CCSprite::createWithTexture(SelCur);
		SelbgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
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
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_8_确定.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_确定.spr");
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
			pSelectedTexture,this,menu_selector(KuiSelNativePlace::mainEnterCallback));
		//pLoginItem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//一个按钮
		//pLoginItem->setOrderOfArrival(1);
		//pLoginItem->setScaleX(m_Scalex); //设置精灵宽度缩放比例
		//pLoginItem->setScaleY(m_Scaley);
	}
	//取消 退到登陆界面
	nFrams=0;
	t_sprintf(nSprName,"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_8_取消.spr");//"\\spr\\Ui3\\loginui\\saverole\\cancel_9_btn.spr");//"\\Spr\\Ui3\\登入界面\\选存档角色按钮\\登陆界面_9_取消.spr");
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
			pSelectedTexture,this,menu_selector(KuiSelNativePlace::mExit));
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
			   ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KuiSelNativePlace::finish)),NULL));
		 }
		CCMenu* pMenu=NULL;
		CCSize nbgsize = SelCur->getContentSize(); //图片宽度高度
		if (Login && Cancel)
		{
				/*pLoginItem->setPositionX(testSprite->getPositionX()-75);
				pLoginItem->setPositionY(testSprite->getPositionY()-80); //确认按钮
				pCancelItem->setPositionX(testSprite->getPositionX()+75);//改变按钮
				pCancelItem->setPositionY(testSprite->getPositionY()-80);*/
			pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);//
				//pMenu->alignItemsInRows(1,2,3,4,NULL);     //四列一行
				//pMenu->alignItemsVertically();             //竖排排列 alignItemsHorizontally
			pMenu->alignItemsHorizontally();                 //横排排列 默认间隔是kDefaultPadding=5  
			pMenu->alignItemsHorizontallyWithPadding(7);    //指定横间隔  
			//pMenu->alignItemsVertically();             //默认间隔也是kDefaultPadding=5 
			//pMenu->alignItemsInRows(3,2,1,NULL);   
			//pMenu->alignItemsVerticallyWithPadding(20);//指定间隔
			//pMenu->setScaleX(m_Scalex); //设置精灵宽度缩放比例
			//pMenu->setScaleY(m_Scaley);
			m_sprCount ++;
			SelbgSprite->addChild(pMenu,2);
			//pMenu->addChild_(pLoginItem,1);
			//pMenu->addChild_(pCancelItem,2);
			if (SelCur)
				pMenu->setPosition(ccp(nbgsize.width/2+203,92));//CCPointZero
		}
		pListNode =NULL;
		pListNode = CCNode::create();
		if (pListNode)
		{
			pListNode->setAnchorPoint(ccp(0,0));
			pListNode->setPosition(ccp(nbgsize.width/2+130,30));//
			SelbgSprite->addChild(pListNode,2);
			//开始创建选项精灵
			LoadList(nPlaceId);
			CCPoint startPos;
			startPos.x = 5;
			startPos.y = nbgsize.height - 100;
			int offsetY= 0;
			for (int i=0;i<m_itemsCount;i++)//m_itemsCount
			{
				CCSprite * bgspr = (CCSprite *)pListNode->getChildByTag(i+1);
				if (!bgspr)
				{
					bgspr = CCSprite::create("ui/textbg.png");
					bgspr->setAnchorPoint(ccp(0,0));
					if  (offsetY==0)
					     offsetY = bgspr->getTextureRect().size.height;
					bgspr->setPosition(ccp(startPos.x,startPos.y-(offsetY+8)*i));
					pListNode->addChild(bgspr,1,i+1);
					char msg[64];
					t_sprintf(msg,m_pPlaceList[i].szName);
					int nFontSize = 14;
					CCLabelTTF *tempLabel = CCLabelTTF::create(UTEXT(msg,1).c_str(),UI_GAME_FONT_DEFAULT, nFontSize,bgspr->getTextureRect().size,kCCTextAlignmentCenter);//Arial
					tempLabel->setAnchorPoint(ccp(0,0));
					tempLabel->setPositionY(-nFontSize/2);
					tempLabel->setColor(ccYELLOW);
					bgspr->addChild(tempLabel);
				}
			}
            //
			if (m_itemsCount>0)
			{
				CCTexture2D *nPic=NULL;
				nPic = _getinidata.getpicPath(m_pPlaceList[0].szImage);
				if (nPic)
				{
				   showSprite = CCSprite::createWithTexture(nPic);
				   showSprite->setAnchorPoint(ccp(0,0));
				   showSprite->setPosition(ccp(8,113));
				   SelbgSprite->addChild(showSprite,2,1);
				}
			}
		}

		//pListMenu->alignItemsHorizontally();                 //横排排列 默认间隔是kDefaultPadding=5  
		//pListMenu->alignItemsHorizontallyWithPadding(7);     //指定横间隔 
		
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
		}

	//DrawNode* drawNode = ccDrawNode::create();
	return true;
}

void KuiSelNativePlace::LoadList(int nPlaceId)
{
	if (m_pPlaceList)
	{
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
	
	KIniFile	File;
	if (_clientlanguage==1)
	{
		if (!File.Load(PLACE_LIST_FILE))
			return;
	}
	else
	{
		if (!File.Load(PLACE_LIST_FILE_VN))
			return;
	}

	int nCount;
	File.GetInteger("List", "Count", 0, &nCount);
	if (nCount <= 0)
		return;
	m_pPlaceList = (KNativePlace*)malloc(sizeof(KNativePlace) * nCount);
	if (m_pPlaceList == NULL)
		return;

	char	szSection[4]={0}, sBuffer[320]={0};
	int		nNumValid = 0;
	m_nLastSelPlace = 0;
	for (int i = 0; i < nCount; ++i)
	{
		//itoa(i, szSection, 10);
		t_sprintf(szSection,"%d",i);
		int	nValue;
		if (!File.GetInteger(szSection, "Id", 0, &nValue))
			continue;
		m_pPlaceList[nNumValid].nId = nValue;  // 地图ID

		if (!File.GetInteger(szSection, "RevID", 0, &nValue))
			continue;

		m_pPlaceList[nNumValid].inRevID = nValue; // 出生点

		if (!File.GetString(szSection, "Name", "", m_pPlaceList[nNumValid].szName, sizeof(m_pPlaceList->szName)))
			continue;
		if (!File.GetString(szSection, "Img", "", m_pPlaceList[nNumValid].szImage,sizeof(m_pPlaceList[nNumValid].szImage)))
			continue;		
		m_pPlaceList[nNumValid].nDescLen = 0;
		m_pPlaceList[nNumValid].sDesc[0] = 0;
		if (File.GetString(szSection, "Desc", "", sBuffer, sizeof(sBuffer)))
		{
			m_pPlaceList[nNumValid].nDescLen = strlen(sBuffer);
			m_pPlaceList[nNumValid].nDescLen = TEncodeText(sBuffer, m_pPlaceList[nNumValid].nDescLen);
			if (m_pPlaceList[nNumValid].nDescLen < sizeof(m_pPlaceList[nNumValid].sDesc))
				memcpy(m_pPlaceList[nNumValid].sDesc, sBuffer, m_pPlaceList[nNumValid].nDescLen);
			else
				m_pPlaceList[nNumValid].nDescLen = 0;
		}
		if (m_pPlaceList[nNumValid].nId == nPlaceId)
			m_nLastSelPlace = nNumValid;
		nNumValid ++;
	}
	if (nNumValid)
	{//如果有的
		 m_itemsCount=nNumValid;
		//m_List.SetContent((BYTE*)m_pPlaceList, nNumValid, sizeof(KNativePlace), (int)(&((KNativePlace*)0)->szName));
		//m_List.SetCurSel(m_nLastSelPlace);
	}
	else
	{
		m_itemsCount = 0;
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
	m_nLastSelPlace = 0;
	File.Clear();
}

//进入游戏
void KuiSelNativePlace::mainEnterCallback(CCObject* pSender)
{//
	if (nAcclient/* && g_pClient*/)
	{
		//EnterGame();
		//char nmsg[64]={0};
		if (m_selIndex!=-1)
		{//开始进入创建角色界面
			if (m_pPlaceList && m_selIndex >= 0)
			{//开始创建角色
				nNativePlaceId = m_pPlaceList[m_selIndex].nId;
				CCDirector::sharedDirector()->replaceScene(KuiNewPlayer::scene());
			}
		}
		else
		{
#ifdef WIN32
			CCMessageBox("请选择一项","提示");
#else
			CCMessageBox(UTEXT("请选择一项",1).c_str(),UTEXT("提示",1).c_str());
#endif
		}
		//CCDirector::sharedDirector()->replaceScene(KTranSence::scene());
	}
}

//循环更新虚函数 默认自动调用
void KuiSelNativePlace::update(float delta)
{
	unsigned int nSize = 0;
	const char* pBuffer = NULL;
	if (m_bIsClientConnecting)
	{ //账号服务器

	}
	//----处理来自游戏服务器的数据----
	if (m_bIsGameSevConnecting && g_pClient)
	{
	}
}


void KuiSelNativePlace::draw()
{   

	/*if (m_Result != LL_R_NOTHING)
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
		}

		CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		return;
	}

	if (m_Status == LL_S_ENTERING_GAME)
	{//马上跳转画面
		CCDirector::sharedDirector()->replaceScene(KTranSence::scene());
		//CCDirector::sharedDirector()->replaceScene(KgameWorld::scene());
		return;
	}*/
}
//
bool KuiSelNativePlace::isFileExist(const char* pFileName)
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

void KuiSelNativePlace::copyData(const char* pFileName)
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
void KuiSelNativePlace::onEnter()
{

}

void KuiSelNativePlace::onExit()
{

}
//进入后 有 过渡完成
void KuiSelNativePlace::onEnterTransitionDidFinish()
{

}*/

bool KuiSelNativePlace::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void KuiSelNativePlace::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void KuiSelNativePlace::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
CCRect KuiSelNativePlace::getRect(CCNode* pNode)
{
	CCRect  rc;
	CCPoint nbgPos   = SelbgSprite->getPosition();//屏幕中点
	CCSize nbgsize   = SelbgSprite->getContentSize();
	CCPoint nlistPos = pListNode->getPosition();//表单向对 父节点的位置
//	CCDirector::sharedDirector()->convertToGL();
	rc.origin     = pNode->getPosition();
	rc.size       = pNode->getContentSize();
	rc.origin.x   = nbgPos.x+130+origin.x;
	rc.origin.y   = nbgPos.y-nbgsize.height/2+30+rc.origin.y;
	return rc;

	/*
	nbgsize.width/2+130
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y

	*/
}

void KuiSelNativePlace::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处
	CCSprite * sprNode =NULL;
	bool isthis = false;
	for (int i = 0; i < m_itemsCount; ++i) //
	{
		sprNode = (CCSprite *)pListNode->getChildByTag(i+1);
		if  (sprNode)
		{
			if (getRect(sprNode).containsPoint(location))
			{//点中了
				m_selIndex = i;
				sprNode->setColor(ccBLUE);
				isthis = true;

				CCTexture2D *nPic=NULL;
				nPic = _getinidata.getpicPath(m_pPlaceList[i].szImage);
				if (nPic && showSprite)
					showSprite->setTexture(nPic);
			}
			else
			{
				if (!isthis) //点中了,就不归零
				{
				   m_selIndex = -1;
				}
				sprNode->setColor(ccWHITE);
			}
		}
	}
	/*if (clicked)
	{
		clicked = false;
		CCMessageBox("双击","警告");
	}
	else
	{
		//延时0.25s执行（注意在这0.25s的延时过程中clicked已经为true），  
		//如果在这个过程中再次click，那么就执行上面的双击事件处理了  
		//否则，那么就执行下面的回调函数了，处理单击事件 
		scheduleOnce(schedule_selector(KuiSelNativePlace::CheckClick),0.25f);
		clicked =false;
		//定时器
		//this->schedule(schedule_selector(LoginScenes::movebd)); 
	}*/

}

void KuiSelNativePlace::CheckClick(float tt)
{
	if (clicked) 
	{//单击
	   clicked = false;  
	   CCMessageBox("单击","警告");
	}  
}

void KuiSelNativePlace::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void KuiSelNativePlace::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处
}

void KuiSelNativePlace::finish()
{
}

void KuiSelNativePlace::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void KuiSelNativePlace::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void KuiSelNativePlace::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}

void KuiSelNativePlace::mExit(CCObject* pSender){
	nPlaceId = -1;
	nNativePlaceId = -1;
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
		//delete g_pClient;
		//g_pClient=NULL;
	}
	//退到 开始登陆界面
	CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
}

void KuiSelNativePlace::AcceptNetMsg(void* pMsgData) //循环接受处理消息
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
void KuiSelNativePlace::ProcessToLoginGameServResponse(tagNotifyPlayerLogin* pResponse)
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
			CCMessageBox("正在尝试打开账号","提示:");
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
					g_pClient->setSocketStates(true);            //设置已经连接
					tagLogicLogin ll;
					ll.cProtocol = c2s__loginfs_kfjghtueodnchsf; //通知GS服务器开始登陆	  赋值我网络号号给角色
					
					GUID* pGuid = &pResponse->guid;//服务器端唯一的标示
					memcpy(&ll.guid,pGuid,sizeof(GUID));

					if (!g_pClient->SendPackToServer(&ll,sizeof(tagLogicLogin)))
					{
						m_Result = LL_R_CONNECT_SERV_BUSY; //卡号 正在尝试打开账号
						CCMessageBox("连接服务器失败","提示:");
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