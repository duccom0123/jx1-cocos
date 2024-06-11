
#include "engine/KbugInfo.h"
#include "KgameWorld.h"
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
#include "gamecore/KSubWorldSet.h"
extern iCoreShell*	g_pCoreShell; //全局调用
extern KImageStore2 m_ImageStore; //全局调用
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
#include <list>
USING_NS_CC;

CCScene* KgameWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	KgameWorld *layer = KgameWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool KgameWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}
	
	visibleSize        = CCDirector::sharedDirector()->getVisibleSize();//可见的
	origin             = CCDirector::sharedDirector()->getVisibleOrigin();
	CCSize size        = CCDirector::sharedDirector()->getWinSize();
	m_sprCount         =0;
	nTimeCount = 0;
	m_nWidth   = 0;
	m_nHeight  = 0;
	pWW        =NULL;
	animate    =NULL;
	setTouchEnabled(true);

	if (!m_ImageStore.Init())  //初始化图片管理器
		CCMessageBox(UTEXT("空间不足").c_str(),"gameclient");
	if (g_pCoreShell)
	{
		if (g_pCoreShell->test())
		  CCMessageBox("test is suss","g_pCoreShell");
	}
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"mainskillmix.png",
		"mianskillbig.png",
		this,
		menu_selector(KgameWorld::mianSkillCallback)); //回调函数
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2-20 ,
		origin.y + pCloseItem->getContentSize().height/2+20));
	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	m_sprCount ++;
	this->addChild(pMenu,1,m_sprCount);
	/////////////////////////////
	// 3. add your codes below...
	// add a label shows "Hello World"
	// create and initialize a label
	const char * strTiele = CXmlStream::GetStringByKeyFromFile("string.xml","title"); //Resources 资源放在目录下,如果没有这个文件程序会崩溃
	KIniFile test;
	//KPakList g_pPakList;
	char strtest[32]={0};
	//CCFileUtils::getPathForFilename()
	std::string nPath = CCFileUtils::sharedFileUtils()->getWritablePath();

	//CCMessageBox(nPath.c_str(),"getWritablePath");//可读写路径 /data/data/包名/files/

	//nPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("package.ini");
	//nPath = CCFileUtils::sharedFileUtils()->getWritablePath();//getWriteablePath();
	//nPath += "download";
	int nCount = 0;
	//tolower() //大写转小写
	//toupper() //小写转大写
	//getExtSDCardPath();
	//Environment.getExternalStorageDirectory();

	//Context::getCacheDir();
	//Environment.getExternalStorageDirectory().toString() + "/" + "Tianchaoxiong/useso";
	//AssetManager::InputStream.open();
	//pContext.getAssets();
	//getAssetManager();
	std::string fielDir;
	fielDir = "package.ini";
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	fielDir = "download\\package.ini";
#endif
	//nCount = g_pPakList.Open(fielDir.c_str());  //加载PAK
	//CCFileUtils::getFullPathForDirectoryAndFilename("Resources","test.ini");
	//fullPathForFilename(filename.c_str());
	if (test.Load(fielDir.c_str()))  //"D:\\cocos2dx226\\projects\\jxclient\\Resources\\test.ini"
	{
		test.GetString("Package","Path","world",strtest,sizeof(strtest));
	}
	test.Clear();
	start.Start();
	char nCurStr[128]={0};
	sprintf(nCurStr,"(%s)%d \ntime:%u",strtest,nCount,start.GetElapse());
	//CCLabelTTF* 
	pMainLabel = CCLabelTTF::create(strTiele,"Arial", 24);
	pMainLabel->setString(nCurStr);//设置显示的问题
	//CCMessageBox(nPath.c_str(),"title");
	//CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
	// position the label on the center of the screen
	pMainLabel->setPosition(ccp(origin.x + visibleSize.width/2,
		origin.y + visibleSize.height - pMainLabel->getContentSize().height));

	// add the label as a child to this layer
	//m_sprCount++;
	this->addChild(pMainLabel, 1);

	CCSprite *bgSprite=CCSprite::create("bg.jpg");
	bgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	float winw = visibleSize.width;   //获取屏幕宽度
	float winh = visibleSize.height;  //获取屏幕高度
	float spx  = bgSprite->getTextureRect().getMaxX();
	float spy  = bgSprite->getTextureRect().getMaxY();
	bgSprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
	bgSprite->setScaleY(winh/spy);
	//m_sprCount++;
	//this->addChild(bgSprite,0,m_sprCount);

	CCMenuItemFont* itemReset = CCMenuItemFont::create(UTEXT("退出").c_str(),this,menu_selector(KgameWorld::beiyongb));
	//itemReset->setFontName("fonts/汉鼎简中宋.TTF");
	itemReset->setPosition(ccp(visibleSize.width/2, 50));
	itemReset->setFontSize(16);

	CCMenuItemFont* itemGetClientVersion = CCMenuItemFont::create(UTEXT("备用").c_str(),this,menu_selector(KgameWorld::beiyonga));
	//itemGetClientVersion->setFontName("fonts/汉鼎简中宋.TTF");
	itemGetClientVersion->setPosition(ccp(visibleSize.width/2, 100));
	itemGetClientVersion->setFontSize(16);
	CCMenuItemFont* itemGetServerVersion = CCMenuItemFont::create(UTEXT("增加NPC").c_str(),this,menu_selector(KgameWorld::nextsenc));
	itemGetServerVersion->setPosition(ccp(visibleSize.width/2, 150));
	itemGetServerVersion->setFontSize(16);
	CCMenuItemFont* itemUpdateVersion = CCMenuItemFont::create(UTEXT("删除精灵").c_str(),this,menu_selector(KgameWorld::delspr));
	itemUpdateVersion->setPosition(ccp(visibleSize.width/2, 200));
	itemUpdateVersion->setFontSize(16);
	CCMenuItemFont* itemEnterScene = CCMenuItemFont::create(UTEXT("增加精灵").c_str(),this,menu_selector(KgameWorld::addspr));
	itemEnterScene->setPosition(ccp(visibleSize.width/2, 250));
	itemEnterScene->setFontSize(16);

	CCMenuItemFont* itemaddmaop = CCMenuItemFont::create(UTEXT("加载地图").c_str(),this,menu_selector(KgameWorld::addmap));
	itemaddmaop->setPosition(ccp(visibleSize.width/2, 300));
	itemaddmaop->setFontSize(16);
	itemaddmaop->setTag(1);   //设置标志
	//CCAssert(,"");//断言
	CCMenu *newmenu=CCMenu::create(itemaddmaop,itemEnterScene,itemUpdateVersion,itemGetServerVersion,itemGetClientVersion,itemReset,NULL);  // 此步很关键，否则只是菜单项，但不能点击
	newmenu->setPosition(ccp(origin.x+70, visibleSize.height/2 + origin.y+150));//整个菜单的位置
	newmenu->alignItemsVertically();       //设置为竖排排列
	//m_sprCount++;
	this->addChild(newmenu,2);             //
	CCNode * d = newmenu->getChildByTag(1);//获取父节点下的子节点
	//newmenu->removeChild(d,true);          //删除父节点下的子节点
	//newmenu->removeChildByTag(1);
	//itemaddmaop->removeFromParentAndCleanup(true);  //
	d->removeFromParentAndCleanup(true);
	//d->release();//释放了内存。。。。

	if (itemaddmaop)
	   newmenu->addChild(itemaddmaop,true);  //父节点下增加一个子节点
	//创建一个组件？
	//CCComponent::create();
	//newmenu->setUserData();//为某个节点绑定自定义数据
	//渲染纹理 图片数据
	/*
	List<CCTexture2D> loadTexture = new list_t<CCTexture2D>();
	List<CCSprite> loadsprite     = new list_t<CCSprite>();
	*/
	m_pTexture = NULL;
	
	m_pTexture = new CCTexture2D();

	if (m_pTexture)
	{//在这里开始获取图像数据的指针data
		//unsigned char *data = NULL;
		//unsigned long m_size = 0;
		//memset()
		/*CCImage* image = new CCImage();

		if (!image)
		{
			CCMessageBox("image a","title");
			return false;
		}
		//bool bRet = image->initWithImageFile("bg.jpg",CCImage::kFmtJpg);
		*/
		//std::string bFilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename("laohuji.spr");//("MA_BD_075_ST01.spr");//
		/*FILE *fp=NULL;
		    fp = CCFileUtils::sharedFileUtils()->getFilePtr("laohuji.spr", "rb", &m_size);
		if (!fp)
		{
			CCMessageBox("fp is NULL","title");
			return true;
		}
		*/

		//char nSprInfo[128]={0};
		//sprintf(nSprInfo,"m_nWidth:%d,m_nHeight:%d,m_nFrames:%d %d/%d\n颜色:%d 偏移:%d/%d",m_nWidth,m_nHeight,m_nFrames,pFrame->Width,pFrame->Height,m_nColors,pFrame->OffsetX,pFrame->OffsetY);
		//CCMessageBox(nSprInfo,"title");
	     //RenderToA4R4G4B4Alpha2(pW,(BYTE *)pNode->m_pBitmap,pNode->m_nWidth,pNode->m_nHeight,(BYTE *)pNode->m_pPalette,m_ptDes,m_rtSrc,nPitch,pCanvas->m_nMask32);
		//SprDecode.RenderToSprDecode(pW,(BYTE *)pFrame->Sprite,m_nWidth,m_nHeight,m_Palette);
		
		 CCTexture2D *Cur = SprDecode.RenderToSprDecode_New("\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr",10,&m_nWidth,&m_nHeight);
		//SprDecode.RenderToSprDecode("laohuji.spr",0,pW);\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr
		/*stImageInfo m_info;
		m_info.bytes_per_pixel=2;
		m_info.width          =m_nWidth;
		m_info.height         =m_nHeight;
		m_info.buffer         = (BYTE *)pW;
		SprDecode.SaveBMPFile("D://test.bmp",m_info);*/
		//---------------
		/*需要构造 png 数据格式
		bool bRet = image->initWithImageData(pFrame->Sprite,m_size, CCImage::kFmtJpg);
		if (!bRet)
		{
			CCMessageBox("load bg.jpg er","title");
			return true;
		}
		bool ret = false;
		     ret = m_pTexture->initWithImage(image);
	    */
		//CCSize imageSize =CCSizeMake((float)(m_nWidth), (float)(m_nHeight));

		/*CCRenderTexture* renderTexture=CCRenderTexture::create(m_nWidth, m_nHeight);//指定画布的大小
		renderTexture->beginWithClear(0, 23, 5, 200);                    //调用begin()开始在画布上绘制纹理
	    renderTexture->end();                                            //渲染完成
		renderTexture->getSprite()->getTexture()->initWithData(pWW,(CCTexture2DPixelFormat)kTexture2DPixelFormat_RGBA8888,m_nWidth,m_nHeight,imageSize);
		CCSprite * dynamicTexture=CCSprite::createWithTexture(renderTexture->getSprite()->getTexture());//使用动态纹理创建精灵
		dynamicTexture->setPosition(ccp(m_nWidth + origin.x+400, visibleSize.height/2 + origin.y));
	    this->addChild(dynamicTexture);*/

		//std::string bFilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename("bg.jpg");
		//data = CCFileUtils::sharedFileUtils()->getFileData(bFilePath.c_str(), "rb", &m_size);
		
		//imageSize.width = visibleSize.width;
		//imageSize.height= visibleSize.height;
		//m_pTexture->initWithData(pWW,(CCTexture2DPixelFormat)kTexture2DPixelFormat_RGBA8888,m_nWidth,m_nHeight,imageSize); //CCSizeMake((float)w, (float)h)
		/*CCRect rect;
			 rect.size = CCSizeMake((float)(100), (float)(200));
			 rect.origin.x =0;
			 rect.origin.y =0; 老虎机面板
		*/
		//CCMessageBox(m_pTexture->description(),"title");
		testSprite = NULL;
		if (Cur)
		{ 
			/*char nSprFilePath[256]={0};
			sprintf(nSprFilePath,"%s_%d","kl_150_jiankun_fu.spr",10);
			CCTexture2D* OldText=NULL;
			if (OldText = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))
			   CCMessageBox(Cur->description(),"title");*/
			testSprite=CCSprite::createWithTexture(Cur);
			testSprite->setPosition(ccp(m_nWidth + origin.x+100, visibleSize.height/2 + origin.y));
			m_sprCount++;
			this->addChild(testSprite,2,m_sprCount);

			char nSprFilePath[64]={0};
			CCAnimation*animation = CCAnimation::create();//创建一个动画

            //std:string nFileName ="\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr";
			//g_StrLower(nFileName);
			DWORD nFielpahtdwid = g_FileName2Id("\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr");
			
			for (int i=1;i<21;i++)
			{   
				//CCTexture2D* nCurCount=NULL;
				sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,i);  
				CCMessageBox(nSprFilePath,"animation");
				//nCurCount = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);//如果存在的 就
				//if (nCurCount)
				//testSprite->setTexture(nCurCount);//如果没有 会自动创建一个
				animation->addSpriteFrameWithSprName(nSprFilePath);//往这个动画增加纹理帧
			}
			//设置动画帧的时间间隔
			animation->setDelayPerUnit(0.02f);
			//设置播放循环 一直播放 为-1
		    animation->setLoops(-1);
			//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
			animation->setRestoreOriginalFrame(TRUE);
			// 创建一个永远循环的动画
			animate = CCAnimate::create(animation);
			
			//CCRepeatForever* forever = CCRepeatForever::create(animate);
			if (testSprite)
			   //testSprite->runAction(animate);
			   testSprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KgameWorld::finish)),NULL));

			//if(testSprite->isActionDone(animate))
			 // removeChild(testSprite,true);
			//CCMoveTo* move = CCMoveTo::create(30, ccp(-20, 200));
			//testSprite->runAction(move); 
			//removeChild(testSprite,true);
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

		/*if (pWW)
		{
		   free (pWW);
		}*/

		//delete pW;pW=NULL;
		//delete data;data=NULL;
		CCSprite *lpSprite=CCSprite::create("jin.png");
		lpSprite->setPosition(ccp(visibleSize.width/2 +origin.x, visibleSize.height/2 + origin.y));
		m_sprCount++;
		this->addChild(lpSprite,1,m_sprCount);
		//
		//CCFileUtils::sharedFileUtils()->CloseFilePtr("laohuji.spr",FALSE);

		//fclose(fp);
		/*
		float winw = visibleSize.width;   //获取屏幕宽度
		float winh = visibleSize.height;  //获取屏幕高度
		float spx = testSprite->getTextureRect().getMaxX();
		float spy = testSprite->getTextureRect().getMaxY();
		testSprite->setScaleX(winw/spx);  //设置精灵宽度缩放比例
		testSprite->setScaleY(winh/spy);
		
		*/
	}
	else
	{

	}

	//this->setRotation(CC_RADIANS_TO_DEGREES(angle));
	// add "HelloWorld" splash screen"
	//CCSprite* pSprite = CCSprite::create("HelloWorld.png");
	// position the sprite on the center of the screen
	//pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	// add the sprite as a child to this layer
	//this->addChild(pSprite, 0);
	//会自动覆盖掉前一个场景
	//CCSprite* sprite = CCSprite::create("bg.jpg");
	//ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y)
	//sprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));//(ccp(visibleSize.width/2,visibleSize.height/2));
	//sprite->setScaleX(1.5f);
	//sprite->setScaleY(2.0f);
	//Size mywinsize=Director::getInstance()->getWinSize();
	/*float winw = visibleSize.width; //获取屏幕宽度
	float winh = visibleSize.height;//获取屏幕高度
	float spx = sprite->getTextureRect().getMaxX();
	float spy = sprite->getTextureRect().getMaxY();
	sprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
	sprite->setScaleY(winh/spy);*/
	//this->addChild(sprite,0);
	//摇杆
	CCSprite *controlSprite=CCSprite::create("yaoganx.png");
	//摇杆背景
	CCSprite *ygSprite=CCSprite::create("yaoganz.png");
	joystick=HRocker::HRockerWithCenter(ccp(100.0f,100.0f),60.0f ,controlSprite ,ygSprite,false);
	m_sprCount++;
	this->addChild(joystick,2,m_sprCount);
	nTestCount = 0;
	//----------复制资源文件到可读写文件夹
	/*if (!isFileExist("laohuji.spr"))
	{//不存在就开始复制
		copyData("laohuji.spr");
	}
	//开始测试 是否复制成功....
	if (isFileExist("laohuji.spr"))
	{//不存在就开始复制
		CCMessageBox("suee","isFileExist");
	}*/
	//this->draw();


	this->scheduleUpdate();  //遥杆更新调用函数

	return true;
}

//关闭按钮回调函数
void KgameWorld::mianSkillCallback(CCObject* pSender)
{//关闭
/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif*/
}

//循环更新虚函数 默认自动调用
void KgameWorld::update(float delta)
{
	if (nTimeCount*1000 <= start.GetElapse()*18) //
	{//让客户端跑起来 每秒运行一次
		if (g_pCoreShell)
		    g_pCoreShell->Breathe();
		nTimeCount++;
	//g_SubWorldSet.m_nLoopRate=10000;
	char nPiontInfo[64*3]={0}; //start.GetElapse()
	sprintf(nPiontInfo,"%u,%u,%u",nTimeCount,g_SubWorldSet.m_nLoopRate,start.GetElapse()); //g_pCoreShell->GetSubGameTime()
	pMainLabel->setString(nPiontInfo);
	}
	if (nTimeCount*1000 >= start.GetElapse()*18)
	{
		//UiPaint(nGameFps);    //绘画游戏世界 
		Sleep(1);               //太快就暂停一秒
	}
	else if ((nTimeCount >> 3))   //每半帧就停
	{//否则停止绘画游戏世界，就卡住
		Sleep(1);
	}

	DWORD nFielpahtdwid = g_FileName2Id("\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr");
	for (int i=1;i<21;i++)
	{   		
		char nSprFilePath[64]={0};
		sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,i);  
		CCTexture2D* nCurTexture = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath);//如果存在的 就
		if (!nCurTexture)
			continue;
		CCAnimation*animation = CCAnimation::create();//创建一个动画
		animation->addSpriteFrameWithSprName(nSprFilePath);//往这个动画增加纹理帧
		animation->setDelayPerUnit(0.02f);
		//设置播放循环 一直播放 为-1
		animation->setLoops(1);
		//设置动画结束后恢复到第一帧 setRestoreOriginalFrame
		animation->setRestoreOriginalFrame(TRUE);
		// 创建一个永远循环的动画
		animate = CCAnimate::create(animation);
		CCSprite * tSprite=CCSprite::createWithTexture(nCurTexture);
		tSprite->setPosition(ccp(m_nWidth + origin.x+300, visibleSize.height/2 + origin.y));
		this->addChild(tSprite,2,100);
		tSprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KgameWorld::endSprite)),NULL));
		
	}

	if  (m_pTexture && testSprite)
	{//如果这个纹理存在的话,刷新这个纹理的数据 能达到动画的显示吗？
			//pWW = SprDecode.RenderToSprDecode_New("kl_150_jiankun_fu.spr",18,&m_nWidth,&m_nHeight);//内存泄露
				/*m_pTexture->DeleteTexture();

				CCSize imageSize =CCSizeMake((float)(m_nWidth), (float)(m_nHeight));
			    //内存泄露
				m_pTexture->initWithData(pWW,(CCTexture2DPixelFormat)kTexture2DPixelFormat_RGBA8888,m_nWidth,m_nHeight,imageSize); //CCSizeMake((float)w, (float)h)
				*/
		/*if (animate)
		{
			if (testSprite->isActionDone(animate))
			{
				testSprite->stopAction(animate);
				removeChild(testSprite,true);
				testSprite->release();
				testSprite=NULL;
			}
		}*/
	}

	int newX = 0;
	int newY = 0;
	//判断是否按下摇杆及其类型
    //CCPoint nCurPoint= joystick->getDirection();
	//float n = joystick->getAngleSigned();
	/*char nPiontInfo[64]={0};
	sprintf(nPiontInfo,"%f",n);
	pMainLabel->setString(nPiontInfo);
    */
	switch(newY)
	{
	case 1:
		break;
	case  2:
		break;
	case 3:
		break;
	case 4:
		break;
	default:
		break;
	}
}


void KgameWorld::draw()
{
	/*nTestCount ++;
	char nPiontInfo[64]={0};
	sprintf(nPiontInfo,"%d",nTestCount);
	pMainLabel->setString(nPiontInfo);*/
	//屏幕尺寸大小
	//CCSize mysize      = CCDirector::sharedDirector()->getVisibleSize();
	//CCPoint origin     = CCDirector::sharedDirector()->getVisibleOrigin();
	//点
	//一个点
	/*ccPointSize(1);
	ccDrawColor4B(255, 25, 200, 250);
	ccDrawSolidRect(ccp(origin.x, origin.y), ccp(origin.x+mysize.width, origin.y+mysize.height), ccc4f(0, 0, 0, 0.5f));
	*/
	/*for (int i=0;i<=mysize.width;i++)
		for(int j=0;j<=mysize.height;j++)
		{
			CCPoint nPos;
			nPos.x = i;
			nPos.y = j;
            ccDrawPoint(nPos);
		}*/
/*
	//多个点
	ccPointSize(30);
	ccDrawColor4B(255, 0, 0, 100);
	CCPoint Pointarray[] = {ccp(200, 150), ccp(200, 200), ccp(280, 150), ccp(280, 200)};
	ccDrawPoints(Pointarray, 4);


	//直线
	glLineWidth(3);
	ccDrawColor4B(255, 255, 255, 130);
	ccDrawLine(ccp(10, 300), ccp(200, 300) );


	//圆
	glLineWidth(3);
	ccDrawColor4B(255, 255, 100, 190);
	ccDrawCircle(ccp(50, 250), 40, 3.14/2, 360, true, 1, 0.5);
	*/

	//矩形
	//空心
/*	glLineWidth(5);
	ccDrawColor4B(24, 25, 200, 140);
	//ccDrawRect(ccp(10, 150), ccp(110, 200));
	//实心
	ccDrawSolidRect(ccp(origin.x, origin.y), ccp(origin.x+mysize.width, origin.y+mysize.height), ccc4f(0, 0, 0, 0.5f));
*/
/*
	//多边形
	//空心
	glLineWidth(10);
	ccDrawColor4B(240, 225, 100, 130);
	CCPoint Polyarray[] = {ccp(20, 20), ccp(50, 0), ccp(250, 100), ccp(300, 100), ccp(250, 50)};
	ccDrawPoly(Polyarray, 5, 1); 

	//实心
	CCPoint Polyarray2[] = {ccp(250,30), ccp(280,300), ccp(450,0), ccp(410,130), ccp(420,50)};
	ccDrawSolidPoly(Polyarray2, 5, ccc4f(142, 245, 70, 0.3f)); 

	
	//贝赛尔曲线
	//平面
	glLineWidth(5);
	ccDrawColor4B(100, 100, 100, 255);
	ccDrawQuadBezier(ccp(0,320), ccp(160,100), ccp(480,320), 100); 

	//立体
	glLineWidth(5);
	ccDrawColor4B(200, 200, 200, 255);
	ccDrawCubicBezier(ccp(0,0), ccp(160,300), ccp(320,20), ccp(480,320), 100);*/
}
//
bool KgameWorld::isFileExist(const char* pFileName)
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

void KgameWorld::copyData(const char* pFileName)
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


void KgameWorld::endSprite()
{
	//removeChild(tSprite,true);
	CCNode* s = getChildByTag(100);
	if (s)
		removeChild(s,true);
}

void KgameWorld::finish()
{
		if (testSprite)
		{
		   testSprite->stopAction(animate);
		   removeChild(testSprite,true);
		   //testSprite->removeFromParent();
		   testSprite->release();
		   //testSprite=NULL;
		}

		/*if (testSprite)
		{
		 addChild(testSprite,2,1);
		}*/
}
/*
void KgameWorld::onEnter()
{

}

void KgameWorld::onExit()
{

}
//进入后 有 过渡完成
void KgameWorld::onEnterTransitionDidFinish()
{

}*/

bool KgameWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void KgameWorld::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void KgameWorld::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void KgameWorld::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{

}
void KgameWorld::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}
void KgameWorld::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);

	CCPoint location = touch->getLocation();//目的的坐标?屏幕点击处


	/*public void GetMpsByLocalPosition(Vector3 v3LocalPosition, out int nMpsX, out int nMpsY)
	{
		nMpsX = ((int) (location.x * 100f)) + ((this.m_nRegionBeginX * this.m_nRegionWidth) * this.m_nCellWidth);
		nMpsY = ((this.m_nRegionBeginY * this.m_nRegionHeight) * this.m_nCellHeight) - ((int) (location.y * 200f));
	}*/

	/*CCNode* s = getChildByTag(1);           //通过标记号获取精灵的节点
	//s->stopAllActions();                    //停止所有动作
	s->runAction(CCMoveTo::create(10, ccp(location.x, location.y)));//移动到

	float o = location.x - s->getPosition().x;
	float a = location.y - s->getPosition().y;

	float at = (float) CC_RADIANS_TO_DEGREES( atanf( o/a) );

	if( a < 0 ) 
	{
		if(  o < 0 )
			at = 180 + fabs(at);
		else
			at = 180 - fabs(at);    
	}

	s->runAction( CCRotateTo::create(10, at) ); //旋转到......
	*/
}
void KgameWorld::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void KgameWorld::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void KgameWorld::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}

void KgameWorld::beiyongb(CCObject* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif

}
//弹出对话框层
void KgameWorld::beiyonga(CCObject* pSender)
{//模态对话框，只能对本曾进行操作
	// 定义一个弹出层，传入一张背景图
	PopupLayer* pl = PopupLayer::create("laohuji.png"); //创建一个层 父节点
	// ContentSize 是可选的设置，可以不设置，如果设置把它当作 9 图缩放
	pl->setContentSize(CCSizeMake(400, 360));
	pl->setTitle(UTEXT("吾名一叶").c_str());
	pl->setContentText(UTEXT("娇兰傲梅世人赏，却少幽芬暗里藏。不看百花共争艳，独爱疏樱一枝香。").c_str(), 20, 50, 150);
	// 设置回调函数，回调传回一个 CCNode 以获取 tag 判断点击的按钮
	// 这只是作为一种封装实现，如果使用 delegate 那就能够更灵活的控制参数了
	pl->setCallbackFunc(this, callfuncN_selector(KgameWorld::buttonCallback));
	// 添加按钮，设置图片，文字，tag 信息
	pl->addButton("ok.png","ok.png","", 0);      //在父节点下创建一个子节点按钮 0
	pl->addButton("close.png","close.png","", 1);//在父节点下创建一个子节点按钮 1
	// 添加到当前层
	pl->setPosition(ccp(0,0));
	this->addChild(pl,2,1000); //主场景中增加父节点
}
//弹出对话框的回调函数
void KgameWorld::buttonCallback(CCNode *pNode)
{//在这个层中标记号，以确定是哪个按钮
	CCLog("button call back. tag: %d", pNode->getTag());
}


void  KgameWorld::nextsenc(CCObject* pSender)
{//增加一个NPC

}

//加载地图
void KgameWorld::addmap(CCObject* pSender)
{	
	//MTNotificationQueue::sharedNotificationQueue()->postNotification("connectok", NULL
}

void KgameWorld::addspr(CCObject* pSender){	
	if (m_sprCount>0)
	{
		for (int i=1;i<=m_sprCount;i++)
		{
			CCNode* s = getChildByTag(i);
			if (s)
			   addChild(s,2,i);
		}
	}

}

void KgameWorld::delspr(CCObject* pSender){

	if (m_sprCount>0)
	{
		for (int i=1;i<=m_sprCount;i++)
		{
			CCNode* s = getChildByTag(i);
			if (s)
				removeChild(s,true);
		}
		
	}	
}