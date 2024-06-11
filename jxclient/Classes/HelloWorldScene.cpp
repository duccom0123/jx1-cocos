#include "HelloWorldScene.h"
#include "engine/CXmlStream.h"
#include "engine/KIniFile.h"
#include "engine/KbugInfo.h"
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

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin     = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
    /////////////////////////////
    // 3. add your codes below...
    // add a label shows "Hello World"
    // create and initialize a label
	const char * strTiele = CXmlStream::GetStringByKeyFromFile("string.xml","title"); //Resources 资源放在目录下,如果没有这个文件程序会崩溃
	KIniFile test;
	KPakList g_pPakList;
	char strtest[32]={0};
	//CCFileUtils::getPathForFilename()
	std::string nPath = CCFileUtils::sharedFileUtils()->getWritablePath();
	 CCMessageBox(nPath.c_str(),"getWritablePath");
	nPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("package.ini");
    int nCount = 0;
	//getExtSDCardPath();
	//Environment.getExternalStorageDirectory();

    //Context::getCacheDir();
    //Environment.getExternalStorageDirectory().toString() + "/" + "Tianchaoxiong/useso";
    //AssetManager::InputStream.open();
    //pContext.getAssets();
    //getAssetManager();
	nCount = g_pPakList.Open("file:/assets/package.ini");  //nPath.c_str()
	//CCFileUtils::getFullPathForDirectoryAndFilename("Resources","test.ini");
	//fullPathForFilename(filename.c_str());
	if (test.Load("/spr/test/test.ini"))  //"D:\\cocos2dx226\\projects\\jxclient\\Resources\\test.ini"
	{
		test.GetString("test","number","world",strtest,sizeof(strtest));
	   // CCLOG("cocos2d: succ!.");
	}
	//else
	//	CCLOG("cocos2d: error!.");

	test.Clear();

	char nCurStr[64]={0};
	sprintf(nCurStr,"(%s)%d",strtest,nCount);

	CCLabelTTF* pLabel = CCLabelTTF::create(strTiele, "Arial", 24);
	pLabel->setString(nCurStr);//设置显示的问题

	//CCMessageBox(nPath.c_str(),"title");
    //CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");
    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
	//会自动覆盖掉前一个场景
	CCSprite* sprite = CCSprite::create("bg.jpg");
	sprite->setPosition(ccp(visibleSize.width/2,visibleSize.height/2));
	addChild(sprite);
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}
