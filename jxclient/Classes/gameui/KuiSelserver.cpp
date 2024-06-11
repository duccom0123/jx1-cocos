#include "engine/KbugInfo.h"
#include "gamecore/KCore.h"
#include "KuiSelserver.h"
#include "Klogin_f.h"
#include "Klogin.h"
#include "KSelPlayer.h"
#include "engine/CXmlStream.h"
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
#include "engine/dataChecksum.h"
#include "gamescene/KUpdateLayer.h"
//#include "network/Socket.h"
extern iCoreShell*	g_pCoreShell; //??????
extern KImageStore2 m_ImageStore; //??????
//extern KPakList     g_pPakList;
//extern KPakList * g_pPakList;
#include <list>
USING_NS_CC;

/*void CcharToDouble(const char* ch, double& res)
{
	sscanf(ch,"%lf",&res);
}
*/
KuiSelserver::KuiSelserver()
{
	m_sprCount = 0;
	m_nWidth   = 0;
	m_nHeight  = 0;
	animate    = NULL;
	nserCount  = 0;
	nCurSelIndex = -1;
	m_bIsPlayerInGame =false;
}

KuiSelserver::~KuiSelserver()
{
	nserlist.Clear();
}

CCScene* KuiSelserver::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	KuiSelserver *layer = KuiSelserver::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool KuiSelserver::init()
{
	if (!CCLayer::init())
		return false;
	
	setTouchEnabled(true);
	this->scheduleUpdate();  //??????®≤???
	this->setTouchMode(kCCTouchesOneByOne); //????????
	CCSprite *bgSprite=CCSprite::create("loginbg.jpg");//???????
	bgSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	float winw = visibleSize.width;   //??????????
	float winh = visibleSize.height;  //?????????
	float spx  = bgSprite->getTextureRect().getMaxX();
	float spy  = bgSprite->getTextureRect().getMaxY();
	bgSprite->setScaleX(winw/spx); //?????????????????
	bgSprite->setScaleY(winh/spy);
	m_sprCount++;
	this->addChild(bgSprite,0,m_sprCount);

	m_Status = LL_S_WAIT_INPUT_ACCOUNT;
	char nSprName[128]={0};
	int nFrams=0;

	// t_sprintf(nSprName,"\\Spr\\Ui3\\???????\\????ß≥????\\??????.spr");//hi?u ?ng l?? bay
	// g_StrLower(nSprName);
	// CCTexture2D *bgCur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	// if  (bgCur)
	// {		
		// CCSprite *ySprite=CCSprite::createWithTexture(bgCur);
		// ySprite->setPosition(ccp(visibleSize.width/2 + origin.x-100, visibleSize.height/2 + origin.y));
		// m_sprCount++;
		// this->addChild(ySprite,2,m_sprCount);
		// char nySprFilePath[64]={0};
		// CCAnimation*animation = CCAnimation::create();//???????????
		// DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		// for (int i=0;i<nFrams;i++)
		// {   
			// t_sprintf(nySprFilePath,"%u-%d",nFielpahtdwid,i);  
			// //CCMessageBox(nSprFilePath,"animation");
			// animation->addSpriteFrameWithSprName(nySprFilePath);//??????????????????
		// }
		// //???????????????
		// animation->setDelayPerUnit(0.10f);
		// //???®∞?????? ?????? ?-1
		// animation->setLoops(-1);
		// //????????????????????? setRestoreOriginalFrame
		// animation->setRestoreOriginalFrame(TRUE);
		// // ??????????????????
		// animate = CCAnimate::create(animation);
		// //CCRepeatForever* forever = CCRepeatForever::create(animate);
		// if (ySprite)
			// ySprite->runAction(CCSequence::create(animate,CCCallFunc::create(this, callfunc_selector(KuiSelserver::finish)),NULL));
	// }

	t_sprintf(nSprName,"\\spr\\Dangnhap\\xacnhan1.spr");
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
		pLoginItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pSelectedTexture,
			pSelectedTexture,this,menu_selector(KuiSelserver::mianLoginCallback));
		pLoginItem->setPosition(ccp(270,origin.y + pLoginItem->getContentSize().height/2+20));//??????
	}

	nFrams=0;
	t_sprintf(nSprName,"\\spr\\Dangnhap\\huybo1.spr");
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
		pCancelItem = CCMenuItemImage::createWithTexture(
			pNormalTexture,
			pSelectedTexture,
			pSelectedTexture,this,menu_selector(KuiSelserver::mianCancelCallback));
		pCancelItem->setPosition(ccp(120,origin.y + pCancelItem->getContentSize().height/2+20));//??????
	}
	t_sprintf(nSprName,"\\spr\\Dangnhap\\chonmaychu.spr");//"\\spr\\Ui3\\loginui\\longi_6_b.spr"
	g_StrLower(nSprName);
	nFrams=0;
	
	
	CCTexture2D *Cur = _getinidata.getinidata_new(nSprName,0,&m_nWidth,&m_nHeight,&nFrams);
	testSprite = NULL;
	if (Cur)
	{ 
		testSprite=CCSprite::createWithTexture(Cur);

	   if (testSprite)
	   {
		  testSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y-40));
		  this->addChild(testSprite,2);
		  CCSprite *nTitelSpr =CCSprite::create("ui/selserver.png");
		  nTitelSpr->setAnchorPoint(ccp(0,0));
		  nTitelSpr->setPosition(ccp(testSprite->getContentSize().width/2-nTitelSpr->getContentSize().width/2+15,\
				                       testSprite->getContentSize().height-nTitelSpr->getContentSize().height-9));
		  testSprite->addChild(nTitelSpr);

		  CCMenu* pMenu=NULL;
		  if (Login && Cancel)
		  {
		    pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);
		    pMenu->setAnchorPoint(ccp(0,0));
		    pMenu->setPosition(CCPointZero);
			testSprite->addChild(pMenu,2);
		   }
	   }
    }
	
	
	
	
	//?????????????ß“?
if (_clientlanguage==1)
{//????∑⁄
	if (nserlist.Load("\\Settings\\Ver\\FsjxClient.ini"))//???????????∑⁄?? GetDouble
	{ 
	   nserlist.GetInteger("List","RegionCount",0,&nserCount);
	   CCMenu* serlistMenu = CCMenu::create(NULL);
	   serlistMenu->setAnchorPoint(ccp(0,0));
	   serlistMenu->setPosition(ccp(0,testSprite->getContentSize().height-100));
	   testSprite->addChild(serlistMenu,4);
	   int offSetX=250,offSetY=0,nfontSize=16;
	   for (int i=0;i<nserCount;++i)
	   {
		   /*CheckBox * nSerVerBox = CheckBox::create();
		   nSerVerBox->loadTextures("ui/btn_sel.png","ui/btn_sel.png","ui/btn_sel.png","ui/btn_sel.png","ui/btn_sel.png");
		   nSerVerBox->setEnabled(true);
		   nSerVerBox->setTouchEnabled(true);
		   nSerVerBox->setPosition(ccp(offSetX,offSetY));
		   nSerVerBox->setAnchorPoint(ccp(0,0));
		   nSerVerBox->setTag(nserCount+1);
		   nSerVerBox->addEventListenerCheckBox(this,checkboxselectedeventselector(KuiSelserver::selServerCallBack));
		   offSetY +=30;
		   testSprite->addChild(nSerVerBox,3);*/
		   //????
		   CCSprite *serBgSor=CCSprite::create("ui/btn_sel.png");
		   serBgSor->setPosition(ccp(offSetX,offSetY+(serBgSor->getContentSize().height/2-nfontSize/2)));
		   serlistMenu->addChild(serBgSor,-1);
		   //??????
		   char strSerName[128],cckey[32];
		   t_sprintf(cckey,"Region_%d",i);
		   nserlist.GetString(cckey,"0_Title","",strSerName,sizeof(strSerName));
		   CCLabelTTF* serLabel = CCLabelTTF::create(UTEXT(strSerName,1).c_str(),UI_GAME_FONT_VN,nfontSize,serBgSor->getContentSize(),kCCTextAlignmentCenter);
		   serLabel->setColor(ccc3(0,255,0));
		   CCMenuItemLabel* mserLabel1=CCMenuItemLabel::create(serLabel,this,menu_selector(KuiSelserver::selServerCallBack));
		   mserLabel1->setTag(i+1);
		   mserLabel1->setPosition(ccp(offSetX,offSetY));
		   int nIsOpen=0;
		   nserlist.GetInteger(cckey,"0_IsOpen",0,&nIsOpen);
		   if  (nIsOpen==0)
			   mserLabel1->setEnabled(false);
		   
		   serlistMenu->addChild_(mserLabel1,i+1);
		   offSetY -=30;
	   }
	}
}
else
{//?????∑⁄
	if (nserlist.Load("\\Settings\\Ver_vn\\FsjxClient.ini"))//???????????∑⁄?? GetDouble
	{ 
	   nserlist.GetInteger("List","RegionCount",0,&nserCount);
	   CCMenu* serlistMenu = CCMenu::create(NULL);
	   serlistMenu->setAnchorPoint(ccp(0,0));
	   serlistMenu->setPosition(ccp(-50,testSprite->getContentSize().height-60));
	   testSprite->addChild(serlistMenu,4);
	   int offSetX=250,offSetY=0,nfontSize=16;
	   for (int i=0;i<nserCount;++i)
	   {
		   //????
		   CCSprite *serBgSor=CCSprite::create("ui/btn_sel.png");
		   serBgSor->setPosition(ccp(offSetX,offSetY+(serBgSor->getContentSize().height/2-nfontSize/2)));
		   serlistMenu->addChild(serBgSor,-1);
		   //??????
		   char strSerName[128],cckey[32];
		   t_sprintf(cckey,"Region_%d",i);
		   nserlist.GetString(cckey,"0_Title","",strSerName,sizeof(strSerName));
		   CCLabelTTF* serLabel = CCLabelTTF::create(UTEXT(strSerName,1).c_str(),UI_GAME_FONT_VN,nfontSize,serBgSor->getContentSize(),kCCTextAlignmentCenter);
		   serLabel->setColor(ccc3(0,255,0));
		   CCMenuItemLabel* mserLabel1=CCMenuItemLabel::create(serLabel,this,menu_selector(KuiSelserver::selServerCallBack));
		   mserLabel1->setTag(i+1);
		   mserLabel1->setPosition(ccp(offSetX,offSetY));
		   int nIsOpen=0;
		   nserlist.GetInteger(cckey,"0_IsOpen",0,&nIsOpen);
		   if  (nIsOpen==0)
			   mserLabel1->setEnabled(false);
		   serlistMenu->addChild_(mserLabel1,i+1);
		   offSetY -=30;
	   }
	   testSprite->setScale(1.2);
	}
}
	return true;
}

void KuiSelserver::selServerCallBack(CCObject* menuItem)
{
	if (menuItem)
	{
		CCMenuItemLabel *nCerBox  = (CCMenuItemLabel * )menuItem;
		KIniFile nClentMak;
		if (!nClentMak.Load(CLIENT_VER_FILE))
		{
#ifdef WIN32
			CCMessageBox("?∑⁄????","???:");
#else
			CCMessageBox(UTEXT("?∑⁄????",1).c_str(),UTEXT("???:",1).c_str());
#endif
			return;
		}

		char nstrmak[64]={0},bstrmak[64]={0};
		ZeroMemory(nstrmak,sizeof(nstrmak));
		ZeroMemory(bstrmak,sizeof(bstrmak));
		nClentMak.GetString("CilentVer","mobilemak","0yUDTM0xXm7ZrAjDj7Gf00Mw72fwnrBv",nstrmak,sizeof(nstrmak));
		nClentMak.Clear();

		dataChecksum njjj;
		char nstrps[64]={0};
		ZeroMemory(nstrps,sizeof(nstrps));
		njjj.SimplyDecrypt(bstrmak,nstrmak);
		njjj.SimplyDecrypt(nstrps,CLIENT_MASK);

		if (!strstr(nstrps,bstrmak))
		{
#ifdef WIN32
			CCMessageBox("?∑⁄????","???:");
#else
			CCMessageBox(UTEXT("?∑⁄????",1).c_str(),UTEXT("???:",1).c_str());
#endif
			return;
		}

		if (nAcclient)
		{
			   KIniFile dwmask;
			   std::string fielDir ="";
#ifdef WIN32
			   //fielDir = std::string("paklist.ini");
			   if (_clientlanguage!=1)
				   fielDir = std::string("paklist_vn.ini");
			   else
				   fielDir = std::string("paklist.ini");
#else
			   //fielDir = std::string("data\\paklist.ini");
			   if (_clientlanguage!=1)
				   fielDir = std::string("data\\paklist_vn.ini");
			   else
				   fielDir = std::string("data\\paklist.ini");
#endif
			   char nTempstr[128];
			   t_sprintf(nTempstr,fielDir.c_str());

			   if (!_clientNoNetWork && !dwmask.Load(nTempstr))
			   {
#ifdef WIN32
					CCMessageBox("????????????...","????");
#else
					CCMessageBox(UTEXT("????????????...",1).c_str(),UTEXT("????",1).c_str());
#endif
					return;
			    }
			    dwmask.Clear();

				char downUrlmask[64];
				ZeroMemory(downUrlmask,sizeof(downUrlmask));
				//dwmask.GetString("list","accinfo","",downUrlmask,sizeof(downUrlmask));
				
				char cckey[32];
				t_sprintf(cckey,"Region_%d",nCerBox->getTag()-1);
				nserlist.GetString(cckey,"0_Address","",downUrlmask,sizeof(downUrlmask));

				dataChecksum nurlMask;
				char donwPath[64]={0};
				ZeroMemory(donwPath,sizeof(donwPath));
				nurlMask.SimplyDecrypt(donwPath,downUrlmask);

				/*if  (_clientNoNetWork)
				{//????
					KIniFile nTempIni;
					if (nTempIni.Load("data\\package_temp.ini"))
					{
						nTempIni.GetString("Package","down","",donwPath,sizeof(donwPath));

						dataChecksum nurlMask;
						char nTempPaht[64];
						ZeroMemory(nTempPaht,sizeof(nTempPaht));
						nurlMask.SimplyDecrypt(nTempPaht,CLIENT_IP);
						if  (!strstr(donwPath,nTempPaht))
						{
#ifdef WIN32
							CCMessageBox("????????????...","????");
#else
							CCMessageBox(UTEXT("????????????...",1).c_str(),UTEXT("????",1).c_str());
#endif
							nTempIni.Clear();
							return;
						}

						nTempIni.Clear();
					}
					else
						t_sprintf(donwPath,"192.168.1.128");
				}*/

				if (nAcclient->FsGameServerConnectTo(donwPath,56722))
				{//????????????
					//???????????????
					m_ClientChoices.nServerRegionIndex = nCerBox->getTag();
					nAcclient->Startup();//???????????????
					m_bIsClientConnecting=true;
				}
				else
				{
					nAcclient->Cleanup();
					m_bIsClientConnecting=false;
#ifdef WIN32
					//CCMessageBox("????????????...","????");
					CCMessageBox(donwPath,"Connect IP:");
#else
					//CCMessageBox(UTEXT("????????????...",1).c_str(),UTEXT("????",1).c_str());
					CCMessageBox(donwPath,"Connect IP:");
#endif
					return;
				}
	   }
	  CCDirector::sharedDirector()->replaceScene(Klogin::scene());
	}
}

void KuiSelserver::mianCancelCallback(CCObject* pSender)
{
	m_ClientChoices.nServerRegionIndex =-1;
	if (nAcclient)
	{
		m_bIsClientConnecting=false;
		m_bIsPlayerInGame =false;
		nAcclient->Cleanup();
		
	}
	CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
}

void KuiSelserver::mianLoginCallback(CCObject* pSender)
{

}

//????????¸D?? ??????????
void KuiSelserver::update(float delta)
{
}


void KuiSelserver::draw()
{
}
//
bool KuiSelserver::isFileExist(const char* pFileName)
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

void KuiSelserver::copyData(const char* pFileName)
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

void KuiSelserver::finish()
{

}

bool KuiSelserver::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void KuiSelserver::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void KuiSelserver::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{

}
// default implements are used to call script callback if exist
void KuiSelserver::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{

}
void KuiSelserver::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void KuiSelserver::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);

	CCPoint location = touch->getLocation();//?????????????????
}

void KuiSelserver::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{

}

void KuiSelserver::didAccelerate(CCAcceleration* pAccelerationValue)
{
	
}
void KuiSelserver::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{//convertTouchToNodeSpace
   /*CCPoint touchLocation = convertTouchToNodeSpace(pTouch);
   if(CCRect::CCRectContainsPoint(getRect(pSprite), touchLocation))
	{
	   printf("?????????!\n");
	}*/
}
/*
//????????????????????????
void KuiSelserver::editBoxEditingDidBegin(extension::CCEditBox *editBox)
{
	CCLog("begin");
}

//?????????????????????????
void KuiSelserver::editBoxEditingDidEnd(extension::CCEditBox *editBox)
{
	CCLog("end");
}

//???????????Å£?????
void KuiSelserver::editBoxTextChanged(extension::CCEditBox *editBox, const std::string &text)
{
	CCLog("change");
}

//???°§???????????????¶À?????????????????????????????
void KuiSelserver::editBoxReturn(extension::CCEditBox *editBox)
{
	CCLog("return");
}*/