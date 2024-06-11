#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "gameui/Uiglobaldata.h"
#include "Klogin.h"
#include "Klogin_f.h"
#include "KSelPlayer.h"
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
#include "gamescene/KUpdateLayer.h"
#ifndef WIN32
#include <ctype.h>
#endif
extern iCoreShell*	g_pCoreShell; //全局调用
extern KImageStore2 m_ImageStore; //全局调用
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
#include <list>
USING_NS_CC;

void CcharToDouble(const char* ch, double& res)
{
	sscanf(ch,"%lf",&res);
}

CCScene* Klogin::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	Klogin *layer = Klogin::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Klogin::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	m_sprCount         =0;
	nTimeCount = 0;
	m_nWidth   = 0;
	m_nHeight  = 0;
	//pWW        =NULL;
	animate    =NULL;
	m_bIsPlayerInGame =false;
	setTouchEnabled(true);
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	//CCMenuItemImage *pCloseItem = CCMenuItemImage::createWithTexture();
	m_Status = LL_S_WAIT_INPUT_ACCOUNT;
	char nSprName[128]={0};
	int nFrams=0;
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\okbtn\\loginui_6_ok.spr");
	g_StrLower(nSprName);
	CCMenuItemImage *pCancelItem=NULL;
	CCMenuItemImage *pLoginItem=NULL;
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
			pSelectedTexture,this,menu_selector(Klogin::mianLoginCallback));
		pLoginItem->setPosition(ccp(origin.x + visibleSize.width - pLoginItem->getContentSize().width/2-200 ,
			origin.y + pLoginItem->getContentSize().height/2+20));//一个按钮
	}

	nFrams=0;
	t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\okbtn\\loginui_6_cancel.spr");
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
			pSelectedTexture,this,menu_selector(Klogin::mianCancelCallback));
		pCancelItem->setPosition(ccp(origin.x + visibleSize.width - pCancelItem->getContentSize().width/2 ,
			origin.y + pCancelItem->getContentSize().height/2+20));//一个按钮
	}
	/*CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"mainskillmix.png",
		"mianskillbig.png",
		this,
		menu_selector(Klogin::mianSkillCallback)); //回调函数
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2-20 ,
		origin.y + pCloseItem->getContentSize().height/2+20));//一个按钮
	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	m_sprCount ++;
	this->addChild(pMenu,1,m_sprCount);*/
	/////////////////////////////
	// 3. add your codes below...
	// add a label shows "Hello World"
	// create and initialize a label
	const char * strTiele = CXmlStream::GetStringByKeyFromFile("string.xml","title"); //Resources 资源放在目录下,如果没有这个文件程序会崩溃
	//KPakList g_pPakList;
	//CCFileUtils::getPathForFilename()
	//std::string nPath = CCFileUtils::sharedFileUtils()->getWritablePath();

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
	//nCount = g_pPakList.Open(fielDir.c_str());  //加载PAK
	//CCFileUtils::getFullPathForDirectoryAndFilename("Resources","test.ini");
	//fullPathForFilename(filename.c_str());
	pMainLabel = CCLabelTTF::create(strTiele,"Arial", 20);
	pMainLabel->setString(UTEXT("",1).c_str());//设置显示的问题
	//CCMessageBox(nPath.c_str(),"title");
	// position the label on the center of the screen
	pMainLabel->setPosition(ccp(origin.x + visibleSize.width/2,
		origin.y + visibleSize.height - pMainLabel->getContentSize().height-10));
	// add the label as a child to this layer
	//m_sprCount++;
	this->addChild(pMainLabel, 1);

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
	//CCNode * d = newmenu->getChildByTag(1);//获取父节点下的子节点
	//newmenu->removeChild(d,true);          //删除父节点下的子节点
	//newmenu->removeChildByTag(1);
	//itemaddmaop->removeFromParentAndCleanup(true);  //
	//d->removeFromParentAndCleanup(true);
	//d->release();//释放了内存。。。。
	//if (itemaddmaop)
	//   newmenu->addChild(itemaddmaop,true);  //父节点下增加一个子节点
	//创建一个组件？
	//CCComponent::create();
	//newmenu->setUserData();//为某个节点绑定自定义数据
	//在这里开始获取图像数据的指针data
		//memset()
		//\\Spr\\Ui3\\登入界面\\登陆界面_6_边框.spr  \Spr\Ui3\登入界面\界面小动画\叶子飘过.spr  \\spr\\Ui3\\loginui\\uidonghua\\yezi.spr
	    
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
			ySprite->setScaleY(winh/spy);*/
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
			   ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(Klogin::finish)),NULL));
		 }
		 //输入帐号　密码 "\\Spr\\Ui3\\登入界面\\登陆界面_6_边框.spr"
		 t_sprintf(nSprName,"\\spr\\Ui3\\loginui\\longi_6_b.spr");//"\\spr\\Ui3\\loginui\\longi_6_b.spr"
		 g_StrLower(nSprName);
		 nFrams=0;
		 CCTexture2D *Cur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
		testSprite = NULL;
		if (Cur)
		{ 
			//登陆背景图
			testSprite=CCSprite::createWithTexture(Cur);

		  if (testSprite)
		  {
			char nTempStr[64];
			ZeroMemory(nTempStr,sizeof(nTempStr));
			KIniFile accinfo;
			accinfo.Load("accinfo.ini",true);
			 
			testSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y-50));
			m_sprCount++;
			this->addChild(testSprite,2,m_sprCount);
			//帐号输入框
			CCScale9Sprite *pAccBg = CCScale9Sprite::create();  //"boxgb.png"
			CCEditBox *pAccEditBox = CCEditBox::create(CCSize(200,24),pAccBg);
			const ccColor3B nfcolor ={255,255,255};
			if (pAccEditBox)
			{
				//pEditBox->setContentSize()
				//pAccEditBox->setText("input your account");
				pAccEditBox->setFontColor(nfcolor);
				pAccEditBox->setFontSize(24);
				pAccEditBox->setMaxLength(8);
				//pEditBox->setPosition(ccp(testSprite->getPositionX(), testSprite->getPositionX()));
				pAccEditBox->setPositionX(testSprite->getPositionX()+20);
				pAccEditBox->setPositionY(testSprite->getPositionY()+38);
				///m_sprCount++;
				accinfo.GetString("info","acc","",nTempStr,sizeof(nTempStr));
				pAccEditBox->setText(nTempStr);
				this->addChild(pAccEditBox,2,1000);
			}
		
			//密码输入框
			CCScale9Sprite *pPassBg = CCScale9Sprite::create(); //"boxgb.png"
			CCEditBox *pPassEditBox = CCEditBox::create(CCSize(200,24),pPassBg);
			if (pPassEditBox)
			{
				//pEditBox->setContentSize()
				//pPassEditBox->setText("input your password");
				pPassEditBox->setFontColor(nfcolor);
				pPassEditBox->setFontSize(24);
				pPassEditBox->setInputFlag(kEditBoxInputFlagPassword);
				pPassEditBox->setMaxLength(16);
				//pEditBox->setPosition(ccp(testSprite->getPositionX(), testSprite->getPositionX()));
				pPassEditBox->setPositionX(testSprite->getPositionX()+20);
				pPassEditBox->setPositionY(testSprite->getPositionY());
				//m_sprCount++;
				accinfo.GetString("info","pas","",nTempStr,sizeof(nTempStr));
				pPassEditBox->setText(nTempStr);
				this->addChild(pPassEditBox,2,999);
			}
			accinfo.Clear();

			/*char nSprFilePath[64]={0};
			CCAnimation*animation = CCAnimation::create();//创建一个动画
            //std:string nFileName ="\\spr\\skill\\1502\\kl\\kl_150_jiankun_fu.spr";
			DWORD nFielpahtdwid = g_FileName2Id(nSprName);
			for (int i=0;i<nFrams;i++)
			{   
				t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,i);  
				//CCMessageBox(nSprFilePath,"animation");
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
			   testSprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(Klogin::finish)),NULL));
			*/
			//if(testSprite->isActionDone(animate))
			 // removeChild(testSprite,true);
			//CCMoveTo* move = CCMoveTo::create(30, ccp(-20, 200));
			//testSprite->runAction(move); 
			//removeChild(testSprite,true);
			// create menu, it's an autorelease object
			CCMenu* pMenu=NULL;
			if (Login && Cancel)
			{
				pLoginItem->setPositionX(testSprite->getPositionX()-75);
				pLoginItem->setPositionY(testSprite->getPositionY()-80); //确认按钮
				pCancelItem->setPositionX(testSprite->getPositionX()+75);//改变按钮
				pCancelItem->setPositionY(testSprite->getPositionY()-80);

				pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);
				pMenu->setPosition(CCPointZero);//
				m_sprCount ++;
				this->addChild(pMenu,2,m_sprCount);
			}
		  }
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
		/*CCSprite *lpSprite=CCSprite::create("jin.png");
		lpSprite->setPosition(ccp(visibleSize.width/2 +origin.x, visibleSize.height/2 + origin.y));
		m_sprCount++;
		this->addChild(lpSprite,1,m_sprCount);*/
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
	//this->setTouchMode(kCCTouchesOneByOne); //禁止多点触摸
	return true;
}

void Klogin::mianCancelCallback(CCObject* pSender)
{
	if (nAcclient)
	{
		m_bIsClientConnecting=false;
		m_bIsPlayerInGame =false;
		nAcclient->Cleanup();
		
	}
	CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
}

void Klogin::mianLoginCallback(CCObject* pSender)
{
	CCEditBox * AccNode =  (CCEditBox *)this->getChildByTag(1000);
	CCEditBox * PasNode =  (CCEditBox *)this->getChildByTag(999);
	std::string strAcc;
	std::string strPas;
	//CCEditBox *pPassEditBox
	if (AccNode && PasNode)
	{
		strAcc = AccNode->getText();
		strPas = PasNode->getText();

		if (strAcc.length()>16 || strAcc.length()<=0 || strPas.length()>16 || strPas.length()<=0)
		{
#ifdef WIN32
			CCMessageBox("Sai tài kho?n ho?c m?t kh?u","Chú y");
#else
			CCMessageBox(G2U("Sai tài kho?n ho?c m?t kh?u！").c_str(),G2U("Chú y").c_str());
#endif
			return;
		}
		//开始检测字符的合法性
		std::string strcheck="check";
		strcheck+=strAcc;
		strcheck+=strPas;
		int count = 0,spaceNum=0,chineseNum=0,other=0,letter=0,xiaoxie=0;
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
			else if(strcheck[i]&0x80)//if(strcheck[0]>=0xB0)
				++chineseNum;    //判断是否是中文
			else
			{//开放合区注册 不检测特殊符号
				++other;
			}
		} 
if (_clientlanguage==1)
{//中文版本
		if (spaceNum>0 || chineseNum >0 || other>0 || letter>0)
		{ 	//system("pause");
#ifdef WIN32
			CCMessageBox("含有非法字符,请检查帐号,密码\n不能含有空格,大写字母等特殊符号!","警告");
#else
			CCMessageBox(G2U("含有非法字符,请检查帐号,密码\n不能含有空格,大写字母等特殊符号!").c_str(),G2U("警告").c_str());
#endif
			return;
		} 
}
else
{//不是中文版本
	if (spaceNum>0 || chineseNum >0 || other>0)
	{ 	//system("pause");
#ifdef WIN32
		CCMessageBox("含有非法字符,请检查帐号,密码\n不能含有空格,大写字母等特殊符号!","警告");
#else
		CCMessageBox("Invalid character,please check your name!\n Cannot contain special symbols such as chinese, spaces, etc.!","Warning");
#endif
		return;
	} 
}

	}
	else
	{
#ifdef WIN32
		CCMessageBox("数据有误．．","警告");
#else
		CCMessageBox(G2U("数据有误．．").c_str(),G2U("警告").c_str());
#endif
		return;
	}
	KIniFile accinfo;
	if (accinfo.Load("accinfo.ini",true))
	{
		char nTempStr[64];
		t_sprintf(nTempStr,strAcc.c_str());
		accinfo.WriteString("info","acc",nTempStr);

		t_sprintf(nTempStr,strPas.c_str());
		accinfo.WriteString("info","pas",nTempStr);
		accinfo.Save("accinfo.ini");
		accinfo.Clear();
	}

	if (m_bIsClientConnecting && nAcclient)
	{
		if  (m_ClientChoices.nServerRegionIndex<=0)
		{
#ifdef WIN32
			CCMessageBox("请选择登陆服务器！","警告");
#else
			CCMessageBox(G2U("请选择登陆服务器！").c_str(),G2U("警告").c_str());
#endif
			return;
		}
		//模拟帐号登陆
		m_nNumRole = 0;                          //角色数量设置为0
		memset(&m_RoleList,0,sizeof(m_RoleList));//清空角色清单数据
		ClearAccountPassword(true, true);        //清空备份的帐号密码
		
		BYTE Buff[sizeof(KLoginAccountInfo) + PROTOCOL_MSG_SIZE];
		RandMemSet(sizeof(Buff),(BYTE*)Buff);		// random memory for make a cipher
		char pszAccount[32]={0};
		t_sprintf(pszAccount,strAcc.c_str());

		(*(PROTOCOL_MSG_TYPE*)Buff) = c2s_login_fhgyrinhkmvnsheypo;
		KLoginAccountInfo* pInfo = (KLoginAccountInfo*)&Buff[PROTOCOL_MSG_SIZE];

		pInfo->Size  = sizeof(KLoginAccountInfo);
		pInfo->Param = LOGIN_A_LOGIN | LOGIN_R_REQUEST;
		strncpy(pInfo->Account,pszAccount,sizeof(pInfo->Account));
		
		pInfo->Account[sizeof(pInfo->Account) - 1] = '\0';
		//KSG_PASSWORD pcPassword;
		KSG_StringToMD5String(pInfo->Password.szPassword,strPas.c_str());
		//pInfo->Password      = pcPassword;
		pInfo->nSelSererIdx    = m_ClientChoices.nServerRegionIndex;//KUiSelServer::GetCurServer();
		pInfo->ProtocolVersion = KPROTOCOL_VERSION;       //  传输协议版本，以便校验是否兼容
		m_Status               = LL_S_ACCOUNT_CONFIRMING; //  等待验证帐号密码
		SetAccountPassword(pszAccount, &pInfo->Password);
	    if (!nAcclient->SendMsg(Buff, sizeof(KLoginAccountInfo) + PROTOCOL_MSG_SIZE))
		//if (!nAcclient->SendPackToServer(Buff,sizeof(KLoginAccountInfo) + PROTOCOL_MSG_SIZE))  //sizeof(KLoginAccountInfo) + PROTOCOL_MSG_SIZE
		{
			if (nAcclient)
			{
				nAcclient->Cleanup();
				m_bIsClientConnecting=false;
			}
			CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		}
		//CCDirector::sharedDirector()->replaceScene(KSelPlayer::scene());
		//CCMessageBox("send suss","send suss");
	}
}
//关闭按钮回调函数
void Klogin::mianSkillCallback(CCObject* pSender)
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
void Klogin::update(float delta)
{
	if  (m_Status == LL_S_ROLE_LIST_READY)
	{//如果已经获取到角色数据了 就进入下一个场景
		m_Status = LL_S_IDLE;
		CCDirector::sharedDirector()->replaceScene(KSelPlayer::scene());
		return;
	}
	unsigned int nSize;
	const char* pBuffer = NULL;
	if (m_bIsClientConnecting)
	{ //账号服务器
		while (true)
		{
			if (!nAcclient)
				break;
			//无限循环从服务器获取数据小包
			pBuffer = (const char*)nAcclient->GetPackFromServer(nSize);

			if (!(pBuffer && nSize))
				break;

			PROTOCOL_MSG_TYPE*	pMsg = (PROTOCOL_MSG_TYPE*)pBuffer;
			PROTOCOL_MSG_TYPE	Msg  = *pMsg;  //协议头			

			//CCAssert(Msg > s2c_multiserverbegin || Msg < s2c_end,"");
			if  (Msg == s2c_login || Msg == s2c_roleserver_getrolelist_result ||Msg == s2c_notifypl_ayerlogin_hfkqotivbhd || Msg == s2c_rolenewdelresponse)
			     AcceptNetMsg(pMsg);
//			if (m_MsgTargetObjs[Msg])
	//		   (m_MsgTargetObjs[Msg])->AcceptNetMsg(pMsg);  //处理网络发过来的消息
		}
	}
	int newX = 0;
	int newY = 0;
	//判断是否按下摇杆及其类型
    //CCPoint nCurPoint= joystick->getDirection();
	//float n = joystick->getAngleSigned();
	/*char nPiontInfo[64]={0};
	t_sprintf(nPiontInfo,"%f",n);
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


void Klogin::draw()
{
	/*nTestCount ++;
	char nPiontInfo[64]={0};
	t_sprintf(nPiontInfo,"%d",nTestCount);
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
bool Klogin::isFileExist(const char* pFileName)
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

void Klogin::copyData(const char* pFileName)
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

void Klogin::finish()
{
   if (!nisgetinfo)
	  mExit(NULL);
}
/*
void Klogin::onEnter()
{

}

void Klogin::onExit()
{

}
//进入后 有 过渡完成
void Klogin::onEnterTransitionDidFinish()
{

}*/

bool Klogin::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void Klogin::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void Klogin::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void Klogin::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{

}
void Klogin::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void Klogin::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
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
void Klogin::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}
void Klogin::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void Klogin::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("我被点中了!\n");
	}*/
}

void Klogin::AcceptNetMsg(void* pMsgData) //循环接受处理消息
{
	if (pMsgData == NULL)  //数据包为空 则返回
		return;
	//CCMessageBox("回调成功","GetPackFromServer");
	switch(m_Status)       //发送数据包标识变量
	{
	case LL_S_ACCOUNT_CONFIRMING:  //账号验证
		ProcessAccountLoginResponse((KLoginStructHead*) (((char*)pMsgData) + PROTOCOL_MSG_SIZE));
		break;
	case LL_S_WAIT_ROLE_LIST:      //等待角色清单
		ProcessRoleListResponse((TProcessData*)pMsgData);   //接收协议角色信息
		break;
	case LL_S_CREATING_ROLE:       //创建角色
		//ProcessCreateRoleResponse((tagNewDelRoleResponse*)pMsgData); //创建角色
		break;
	case LL_S_DELETING_ROLE:       //删除角色
		//ProcessDeleteRoleResponse((tagNewDelRoleResponse*)pMsgData);
		break;
	case LL_S_WAIT_TO_DEL:         //已经取消
		//ProcessGetRoleResponse((tagNotifyPlayerLogin*)pMsgData);
		break;
	case LL_S_WAIT_TO_LOGIN_GAMESERVER:  //等待链接到服务器
		//ProcessToLoginGameServResponse((tagNotifyPlayerLogin*)pMsgData); //调用链接到服务器的函数
		break;
	}
}

void Klogin::SetAccountPassword(const char* pszAccount, const KSG_PASSWORD* pcPassword)
{
	int i;
	if (pszAccount)
	{
		strncpy(m_ClientChoices.Account, pszAccount, sizeof(m_ClientChoices.Account));
		for (i = 0; i < 32; ++i)
			m_ClientChoices.Account[i] = ~m_ClientChoices.Account[i];
	}

	if (pcPassword)
	{
		m_ClientChoices.Password = *pcPassword;
		for (i = 0; i < KSG_PASSWORD_MAX_SIZE; ++i)
			m_ClientChoices.Password.szPassword[i] = ~m_ClientChoices.Password.szPassword[i];
	}
}

void Klogin::GetAccountPassword(char* pszAccount, KSG_PASSWORD* pPassword)
{
	int i;
	if (pszAccount)
	{
		memcpy(pszAccount, m_ClientChoices.Account, sizeof(m_ClientChoices.Account));
		for (i = 0; i < 32; ++i)
			pszAccount[i] = ~pszAccount[i];
	}
	if (pPassword)
	{
		*pPassword = m_ClientChoices.Password;
		for (i = 0; i < KSG_PASSWORD_MAX_SIZE; ++i)
			pPassword->szPassword[i] = ~pPassword->szPassword[i];
	}
}

void Klogin::ClearAccountPassword(bool bAccount, bool bPassword)
{
	if (bAccount)
		memset(m_ClientChoices.Account, 0xff, sizeof(m_ClientChoices.Account));
	if (bPassword)
		memset(&m_ClientChoices.Password, 0xff, sizeof(m_ClientChoices.Password));
}


void Klogin::ProcessAccountLoginResponse(KLoginStructHead* pResponse)
{
	//_ASSERT(m_Status == LL_S_ACCOUNT_CONFIRMING && pResponse != NULL);

	if (((pResponse->Param & LOGIN_ACTION_FILTER) == LOGIN_A_LOGIN) &&	//操作性为要匹配
		pResponse->Size >= sizeof(KLoginAccountInfo))				//数据内容的大小也要匹配
	{
		KLoginAccountInfo* pInfo = (KLoginAccountInfo*)pResponse;
		char	szAccount[32]={0};
		KSG_PASSWORD Password;
		GetAccountPassword(szAccount, &Password);  //获取数据库的账号和密码
		if (strcmp(pInfo->Account,  szAccount)  == 0 &&
			strcmp(pInfo->Password.szPassword, Password.szPassword) == 0)
		{//返回的结果
			KIniFile nBVer;
			double mBver,mBishopver;
			if (!nBVer.Load("\\Settings\\Ver\\Xfsverjx.ini"))//补丁包里面的版本号 GetDouble
			{ 
				isthisVer = false;
				m_Result = LL_R_INVALID_PROTOCOLVERSION;  //版本问题
				//ReturnToIdle();//断开网络
				nBVer.Clear();
#ifdef WIN32
				CCMessageBox("版本不对","提示:");
#else
                CCMessageBox(UTEXT("版本不对",1).c_str(),UTEXT("提示:",1).c_str());
#endif
				return;
			}
			nBVer.GetDouble("CilentVer","ver",0.1,&mBver);  //补丁包版本号 GetDouble
			CcharToDouble(pInfo->nBishopver,mBishopver)	;  //bishiop发过来的版本
			
			if  (pInfo->nSelSererIdx<=0)
			{//所有服务器客户端都检测
			  if (mBishopver!=mBver)
				isthisVer = false;
			  else
				isthisVer = true;
			}
			else
			{
			  if (m_ClientChoices.nServerRegionIndex==pInfo->nSelSererIdx)
			  {//等于这个服务器才检测
				  if (mBishopver!=mBver)
					  isthisVer = false;
				  else
					  isthisVer = true;
			  }
			  else //否则就不检测 直接通过
				  isthisVer = true;
			}
			//------------多开检测--------------
			if (pInfo->nForbitopennum)
			{
			}
			nBVer.Clear();
			//预建立角色检测
			if (pInfo->ProtocolVersion!=KPROTOCOL_VERSION)
			{
#ifdef WIN32
				CCMessageBox("版本不兼容!","提示:");
#else
				CCMessageBox(UTEXT("版本不兼容!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
				m_Result = LL_R_INVALID_PROTOCOLVERSION;  //版本问题
				return;
			}
			//----------------------------------
			int nResult = ((pResponse->Param) & ~LOGIN_ACTION_FILTER);
			if (nResult == LOGIN_R_SUCCESS)
			{
				//g_NetConnectAgent.UpdateClientRequestTime(false);
				m_Status = LL_S_WAIT_ROLE_LIST;			  //正在获取角色信息
				//m_Result = LL_R_ACCOUNT_CONFIRM_SUCCESS;  //结果
				//m_LeftTime = pInfo->nLeftTime;
				//m_VipType  = pInfo->nVipType;
				//m_ExtPiont = pInfo->nExtPiont;
				//CCMessageBox("登陆成功","ProcessAccountLoginResponse");
			}
			else
			{
				LOGIN_LOGIC_RESULT_INFO eResult = LL_R_NOTHING;
				switch(nResult)
				{
				case LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR:  //密码错误
					//eResult = LL_R_ACCOUNT_PWD_ERROR;
					m_Status = LL_S_WAIT_INPUT_ACCOUNT;
#ifdef WIN32
					CCMessageBox("密码错误","提示:");
#else
					CCMessageBox(UTEXT("密码错误",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					break;
				case LOGIN_R_ACCOUNT_EXIST:   //账号给锁定  --账号正在使用
					//eResult = LL_R_ACCOUNT_LOCKED;
#ifdef WIN32
					CCMessageBox("帐号正在使用","提示:");
#else
                   CCMessageBox(UTEXT("帐号正在使用",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					m_Status = LL_S_WAIT_INPUT_ACCOUNT;
					break;
				case LOGIN_R_FREEZE:  // 账号被冻结
					//eResult = LL_R_ACCOUNT_FREEZE;
#ifdef WIN32
					CCMessageBox("帐号被冻结","提示:");
#else
                    CCMessageBox(UTEXT("帐号被冻结",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					m_Status = LL_S_WAIT_INPUT_ACCOUNT;
					break;
				case LOGIN_R_INVALID_PROTOCOLVERSION: // 版本不兼容
					//eResult = LL_R_INVALID_PROTOCOLVERSION
#ifdef WIN32
					CCMessageBox("版本不兼容","提示:");
#else
                   CCMessageBox(UTEXT("版本不兼容",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					break;
				case LOGIN_R_FAILED:  //链接失败
					//eResult = LL_R_CONNECT_SERV_BUSY; // 提示服务器忙
#ifdef WIN32
					CCMessageBox("连接失败","提示:");
#else
                    CCMessageBox(UTEXT("连接失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					break;
				case LOGIN_R_TIMEOUT: // 账号使用时间到
					//eResult = LL_R_ACCOUNT_NOT_ENOUGH_POINT;
#ifdef WIN32
					CCMessageBox("帐号已经过期","提示:");
#else
                    CCMessageBox(UTEXT("帐号已经过期",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					m_Status = LL_S_WAIT_INPUT_ACCOUNT;
					break;
				default:
#ifdef WIN32
					CCMessageBox("连接失败","提示:");
#else
					CCMessageBox(UTEXT("连接失败",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					break;
				}

				if (m_Status != LL_S_WAIT_INPUT_ACCOUNT)
				{//断开连接
					//ReturnToIdle();
				}
				m_Result = eResult;
			}
		}
		memset(szAccount, 0, sizeof(szAccount));
		memset(&Password, 0, sizeof(Password));
	}
}


//--------------------------------------------------------------------------
//	功能：角色列表返回
//	状态切换：成功 LL_S_WAIT_ROLE_LIST -> LL_S_ROLE_LIST_READY
//			  失败 状态保持
//--------------------------------------------------------------------------
void Klogin::ProcessRoleListResponse(TProcessData* pResponse)
{
	//_ASSERT(m_Status == LL_S_WAIT_ROLE_LIST && pResponse != NULL);
	if (pResponse->nProtoId == s2c_roleserver_getrolelist_result)
	{
		m_nNumRole = pResponse->pDataBuffer[0];
		if (m_nNumRole > MAX_PLAYER_PER_ACCOUNT)
			m_nNumRole = MAX_PLAYER_PER_ACCOUNT;
		else if (m_nNumRole < 0)
			m_nNumRole = 0;

		RoleBaseInfo* pList = (RoleBaseInfo*)&pResponse->pDataBuffer[1];
		for (int i = 0; i < m_nNumRole; ++i)
		{
			if (pList->szName[0])
			{
				strcpy(m_RoleList[i].Name, pList->szName);
				m_RoleList[i].Attribute = pList->Series;
				m_RoleList[i].Gender = pList->Sex;
				m_RoleList[i].nLevel = pList->Level;
				pList ++;
			}
			else
			{
				m_nNumRole = i;
				break;
			}
		}
		//g_NetConnectAgent.UpdateClientRequestTime(true);
		m_Status = LL_S_ROLE_LIST_READY;

		///CCMessageBox("角色数据准备好登陆","ProcessRoleListResponse");
		//m_Result = LL_R_NOTHING;
		/*if (m_bInAutoProgress)
		{//如果是自动登陆
			int nAdviceChoice;
			GetRoleCount(nAdviceChoice);
			SelectRole(nAdviceChoice);
		}*/
	}
}

void Klogin::mExit(CCObject* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif

}
/*
//键盘弹出后输入框获得焦点时触发
void Klogin::editBoxEditingDidBegin(extension::CCEditBox *editBox)
{
	CCLog("begin");
}

//键盘隐藏后输入框失去焦点时触发
void Klogin::editBoxEditingDidEnd(extension::CCEditBox *editBox)
{
	CCLog("end");
}

//输入框内文本变化时触发
void Klogin::editBoxTextChanged(extension::CCEditBox *editBox, const std::string &text)
{
	CCLog("change");
}

//按下返回键或点击键盘外的位置时触发（点击当前输入框时不触发）
void Klogin::editBoxReturn(extension::CCEditBox *editBox)
{
	CCLog("return");
}*/