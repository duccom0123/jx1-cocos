//有按钮的　ｔａｌｋ　对话框
//
#include "gamecore/KCore.h"

#ifndef WIN32
#include <sys/select.h>
#include <ctype.h>
//typedef struct fd_set FD_SET;
#endif
#include "network/Socket.h"

#include "engine/KbugInfo.h"
#include "engine/CXmlStream.h"
#include "KuiAccreg.h"
#include "Klogin_f.h"
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
#include "engine/dataChecksum.h"

extern iCoreShell*	g_pCoreShell;
#include <list>
USING_NS_CC;

KuiAccreg::KuiAccreg()
{
	isOpen = false;
	colorLayer        = NULL;
	red               = NULL;
	AccNode = NULL;
	PasNode = NULL;
	ePasNode = NULL;
	m_AnswerCount = 0;
	m_StartPos.x=0;
	m_StartPos.y=0;
	s = INVALID_SOCKET;
	nport=INVALID_SOCKET;
	naccIp="";
}

KuiAccreg::~KuiAccreg()
{
	colseSockets();
	isOpen = false;
	colorLayer        = NULL;
}

//创建确定弹出框
KuiAccreg * KuiAccreg::create(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	KuiAccreg * popLayer = KuiAccreg::create();
	//popLayer->setTitle(title);
	popLayer->addDialogData();
	//设置按钮
	popLayer->setcoloseButton(callbackListener,callfun);
	popLayer->isOpen = true;
	return popLayer;
}

//初始化
bool KuiAccreg::init()
{
	if ( !CCLayer::init() ){
		return false;
	}
	//开启触摸响应
	this->setTouchEnabled(true);
	this->scheduleUpdate();                   //调用函数
	this->setTouchMode(kCCTouchesOneByOne);   //禁止多点触摸
	//精灵帧缓存
	//CCSpriteFrameCache * sfCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//sfCache->addSpriteFramesWithFile("p_tips.plist");

	//添加模态背景
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	/*ccColor4B color = ccc4(112, 128, 144, 150);
	CCLayerColor * colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	//colorLayer->setContentSize(winSize);//设置和窗口的大小
	this->addChild(colorLayer);
	*/
	char nSprName[128]={0};
	ZeroMemory(nSprName,sizeof(nSprName));
	t_sprintf(nSprName,"\\spr\\Ui3\\主界面\\图标\\帐号注册.spr");
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
	spriteBox->setPosition(ccp(winSize.width/2,winSize.height/2-bgCur->getContentSize().height/2));

	ccColor4B color = ccc4(112, 128, 144, 150);
	colorLayer = CCLayerColor::create(color);//颜色层
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(visibleSize);//设置和窗口的大小
	colorLayer->addChild(spriteBox);
	this->addChild(colorLayer);
	
	//获得背景图片的大小
	CCSize contentSize = spriteBox->getContentSize();
	m_size = contentSize;
	m_origin = spriteBox->getPosition();
	//colorLayer->setContentSize(contentSize);//设置和窗口的大小
	
	return true;
}

void KuiAccreg::addDialogData()
{
	CCSize nSize;
	nSize.width =128;
	nSize.height=24;
	m_StartPos.x =10;//+ITEM_CELL_SIZE/2;
	m_StartPos.y =140-24;//+ITEM_CELL_SIZE/2;
	//char nTempStr[128];
	//t_sprintf(nTempStr,pContent);
	/*CCLabelTTF *pstrLabel = CCLabelTTF::create(UTEXT(nTempStr,1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	pstrLabel->setColor(ccYELLOW);
	pstrLabel->setPosition(ccp(128/2+30,m_size.height-50));
	ParentNode_ItemEx->addChild(pstrLabel);*/

	/*CCLabelTTF *ptextLabel = CCLabelTTF::create(UTEXT("点击屏幕回程",1).c_str(),UI_GAME_FONT_DEFAULT,14,nSize,kCCTextAlignmentCenter);//Arial
	ptextLabel->setColor(ccYELLOW);
	ptextLabel->setPosition(ccp(m_size.width/2,20));
	ParentNode_ItemEx->addChild(ptextLabel);*/

	//创建一个动作 改变精灵颜色
	//red = CCTintBy::create(0.2,0,-255,-255);//持续时间+颜色 
	//red->retain();
	CCScale9Sprite *pAccBg = CCScale9Sprite::create("loadingEnd.png");  //"boxgb.png"
	pAccBg->setOpacity(0);
	
	AccNode = CCEditBox::create(CCSize(200,24),pAccBg);
	const ccColor3B nfcolor ={255,255,255};
	if (AccNode)
	{
		//pEditBox->setContentSize()
		//AccNode->setText("input your account");
		AccNode->setPlaceHolder("input your account");
		AccNode->setFontColor(nfcolor);
		AccNode->setFontSize(24);
		AccNode->setMaxLength(8);
		AccNode->setInputFlag(kEditBoxInputFlagInitialCapsAllCharacters);
		AccNode->setInputMode(kEditBoxInputModeAny);
		AccNode->setReturnType(kKeyboardReturnTypeDone);
		//AccNode->setDelegate((CCEditBoxDelegate*)this);
		AccNode->setPosition(ccp(m_size.width/2+16,135));
		ParentNode_ItemEx->addChild(AccNode,5);
	}

	//密码输入框
	CCScale9Sprite *pPassBg = CCScale9Sprite::create("loadingEnd.png"); //"boxgb.png"
	pPassBg->setOpacity(0);
	PasNode = CCEditBox::create(CCSize(200,24),pPassBg);
	if (PasNode)
	{
		//pEditBox->setContentSize()
		PasNode->setPlaceHolder("input your password");
		//PasNode->setText("input your password");
		PasNode->setFontColor(nfcolor);
		PasNode->setFontSize(24);
		PasNode->setInputFlag(kEditBoxInputFlagPassword);
		PasNode->setMaxLength(16);
		PasNode->setReturnType(kKeyboardReturnTypeDone);
		//CCTouchDelegate *pDelegate
		//PasNode->setDelegate(this);
		PasNode->setPosition(ccp(m_size.width/2+16,100));
		ParentNode_ItemEx->addChild(PasNode,5);
	}
	//二级密码
	CCScale9Sprite *pePassBg = CCScale9Sprite::create("loadingEnd.png"); //"boxgb.png"
	pePassBg->setOpacity(0);
	ePasNode = CCEditBox::create(CCSize(200,24),pePassBg);
	if (ePasNode)
	{
		//ePasNode->setText("input your epassword");
		ePasNode->setPlaceHolder("input your epassword");
		ePasNode->setFontColor(nfcolor);
		ePasNode->setFontSize(24);
		ePasNode->setInputFlag(kEditBoxInputFlagPassword);
		ePasNode->setMaxLength(16);
		//ePasNode->setDelegate(this);
		ePasNode->setReturnType(kKeyboardReturnTypeDone);
		ePasNode->setPosition(ccp(m_size.width/2+16, 65));
		ParentNode_ItemEx->addChild(ePasNode,5);
	}

	//
	char nSprName[128]={0};
	int nFrams=0,m_nWidth,m_nHeight;
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
			pSelectedTexture,this,menu_selector(KuiAccreg::mianLoginCallback));
		pLoginItem->setPosition(ccp(origin.x + visibleSize.width/2 - pLoginItem->getContentSize().width/2-249,
			origin.y + pLoginItem->getContentSize().height/2+7));//一个按钮
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
			pSelectedTexture,this,menu_selector(KuiAccreg::mianCancelCallback));
		pCancelItem->setPosition(ccp(origin.x + visibleSize.width/2 - pCancelItem->getContentSize().width/2-99,
			origin.y + pCancelItem->getContentSize().height/2+7));//一个按钮
	}

	CCMenu* pMenu=NULL;
	if (Login && Cancel)
	{
		pMenu = CCMenu::create(pLoginItem,pCancelItem,NULL);
		pMenu->setPosition(CCPointZero);//
		ParentNode_ItemEx->addChild(pMenu,2);
	}
	//获取登陆器的服务器的连接数据
	m_WritablePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	KIniFile pPakList;
	if (pPakList.Load("data/paklist.ini"))
	{
		char nstrName[64];
		char nstrmak[64]={0},bstrmak[64]={0};
		ZeroMemory(nstrmak,sizeof(nstrmak));
		ZeroMemory(bstrmak,sizeof(bstrmak));
		dataChecksum njjj;
		pPakList.GetString("list","accinfo","",nstrName,sizeof(nstrName));
		njjj.SimplyDecrypt(bstrmak,nstrName);
		naccIp = bstrmak;
		pPakList.GetString("list","accpint","",nstrName,sizeof(nstrName));
		njjj.SimplyDecrypt(nstrmak,nstrName);
		nport = g_Atoui(nstrmak);
		
		pPakList.Clear();
	}
}

void KuiAccreg::mianCancelCallback(CCObject* pSender)
{
	buttonCallBackFunc(NULL);
	/*if (nAcclient)
	{
		m_bIsClientConnecting=false;
		m_bIsPlayerInGame =false;
		nAcclient->Cleanup();

	}
	CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
	*/
}

void KuiAccreg::mianLoginCallback(CCObject* pSender)
{
	std::string strAcc;
	std::string strPas;
	std::string strePas;

	//CCEditBox *pPassEditBox
	if (AccNode && PasNode && ePasNode)
	{
		strAcc  = AccNode->getText();
		strPas  = PasNode->getText();
		strePas = ePasNode->getText();
		if (strAcc.length()>16 || strAcc.length()<= 0 || strPas.length()>16 || strPas.length()<=0 || strePas.length()>16 || strePas.length()<=0)
		{
#ifdef WIN32
			CCMessageBox("帐号或密码,不能为空并长度不能大于16","警告");
#else
			CCMessageBox(G2U("帐号或密码,不能为空并长度不能大于16").c_str(),G2U("警告").c_str());
#endif
			return;
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
	//开始检测字符的合法性
	std::string strcheck="check";
	strcheck+=strAcc;
	strcheck+=strPas;
	strcheck+=strePas;
	int count = 0,spaceNum=0,chineseNum=0,other=0,letter=0,xiaoxie=0;
	for(int i = 0; i <strcheck.length();i++)
	{ 
		//  if(str[i] < 0 && str[i] > 9);
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
{//中文
	if (spaceNum>0 || chineseNum >0 || other>0 || letter>0)
	{ 	//system("pause");
		//AfxMessageBox("含有非法字符，请检查姓名，密码，不能含有中文，空格，大写字母等特殊符号！");
#ifdef WIN32
		CCMessageBox("含有非法字符,请检查姓名,密码\n不能含有中文,空格,大写字母等特殊符号!","警告");
#else
		CCMessageBox(G2U("含有非法字符,请检查姓名,密码\n不能含有中文,空格,大写字母等特殊符号!").c_str(),G2U("警告").c_str());
#endif
		return;
	} 
}
else
{//其他版本
	if (spaceNum>0 || chineseNum >0 || other>0)
	{ 	//system("pause");
		//AfxMessageBox("含有非法字符，请检查姓名，密码，不能含有中文，空格，大写字母等特殊符号！");
#ifdef WIN32
		CCMessageBox("含有非法字符,请检查姓名,密码\n不能含有中文,空格,大写字母等特殊符号!","警告");
#else
		CCMessageBox("Invalid character,please check your name or password！\n Cannot contain special symbols such as Chinese, spaces, etc.!","Warning");
#endif
		return;
	} 
}
	//开始连接服务器
	//开始创建一个套子节
	//char msg[64];
	//t_sprintf(msg,"%d",nport);
	//CCMessageBox(naccIp.c_str(),msg);
	s = socket(AF_INET, SOCK_STREAM,IPPROTO_IP);

	if  (s==INVALID_SOCKET)
	{
#ifdef WIN32
		CCMessageBox("连接服务器失败","警告");
#else
		CCMessageBox(G2U("连接服务器失败").c_str(),G2U("警告").c_str());
#endif
		CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		return;
	}

/*#ifndef WIN32
	int flags = fcntl(s,F_GETFL,0);                        //获取文件的flags值。
	fcntl(s,F_SETFL,flags|O_NONBLOCK);                     //设置成非阻塞模式；
#endif*/

	KSG_PASSWORD pcPassword;
	ZeroMemory(&pcPassword,sizeof(pcPassword));
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(naccIp.c_str());
	svraddr.sin_port = htons(nport);
	int ret =::connect(s,(struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret == SOCKET_ERROR) 
	{
#ifdef WIN32
		CCMessageBox("连接服务器失败A","警告");
#else
		CCMessageBox(G2U("连接服务器失败A").c_str(),G2U("警告").c_str());
#endif
		return;
	}
	std::string strMsg = "SEND MSG";
	strMsg +=strAcc;
	strMsg +=" ";
	KSG_StringToMD5String(pcPassword.szPassword, strPas.c_str());
	strMsg +=pcPassword.szPassword;   //密码
	strMsg +=" ";
	ZeroMemory(&pcPassword,sizeof(pcPassword));
	KSG_StringToMD5String(pcPassword.szPassword, strePas.c_str());
	strMsg +=pcPassword.szPassword;   //二级密码
	strMsg +=" ";
	strMsg +="ENDMSG";
	strMsg +=" ";

	int nLen = strMsg.length();
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(s,&fd);
	//FD_SET fd = {1,s};
	//memset( &writefds, 0, sizeof( writefds ) );
	//fd.fd_count = 1;
	//fd.fd_array[0] = s;
	TIMEVAL tv = {TIME_OUT,0};
	if(select(s+1,NULL,&fd,NULL,&tv) == 0) 
	{
		colseSockets();
#ifdef WIN32
		CCMessageBox("提交注册申请超时","警告");
#else
		CCMessageBox(G2U("提交注册申请超时").c_str(),G2U("警告").c_str());
#endif
		return;
	}

	int nBytesSent;
	if((nBytesSent = ::send(s,strMsg.c_str(),nLen,0)) == SOCKET_ERROR) 
	{
#ifdef WIN32
		CCMessageBox("提交注册申请失败","警告");
#else
		CCMessageBox(G2U("提交注册申请失败").c_str(),G2U("警告").c_str());
#endif
		colseSockets();
		return;
	}
	if (nBytesSent == nLen)          // 发送成功
	{
#ifdef WIN32
		CCMessageBox("提交注册申请成功","警告");
#else
		CCMessageBox(G2U("提交注册申请成功").c_str(),G2U("警告").c_str());
#endif
		char m_pReadBuf[256]={0};
		//循环等待服务器的相应消息
		while(1)
		{//给接受数据缓冲区清零
			memset(m_pReadBuf,0,sizeof(m_pReadBuf));
			if(select(s+1,&fd,NULL,NULL,&tv) == 0) 
			{
#ifdef WIN32
				CCMessageBox("注册失败A","警告");
#else
				CCMessageBox(G2U("注册失败A").c_str(),G2U("警告").c_str());
#endif
				colseSockets();
				return;
			}
			//接收数据
			int nBytesReceived;
			if((nBytesReceived =recv(s,m_pReadBuf,sizeof(m_pReadBuf),0)) == SOCKET_ERROR)
			{
#ifdef WIN32
				CCMessageBox("注册失败B","警告");
#else
				CCMessageBox(G2U("注册失败B").c_str(),G2U("警告").c_str());
#endif
				colseSockets();
				return;
			}  
			//如果接受到的数据长度大于0，则退出循环，否则循环等待
			if (nBytesReceived > 0)
				break;
		};
		char strCommand[9];
		memset(strCommand,0,9);          //清零
		strncpy(strCommand,m_pReadBuf,7);//复制m_pReadBuf前七个字符窜到strCommand
		if (strcmp(strCommand,"ACK MSG") == 0)   //只处理服务器端对原消息的应答数据  比较大小区别
		{ 
#ifdef WIN32
			CCMessageBox("注册成功","警告");
#else
			CCMessageBox(G2U("注册成功").c_str(),G2U("警告").c_str());
#endif
			CCDirector::sharedDirector()->replaceScene(Klogin_f::scene());
		}
		else if (strcmp(strCommand,"YCK MSG")== 0)
		{
#ifdef WIN32
			CCMessageBox("该用户名已经注册！","警告");
#else
			CCMessageBox(G2U("该用户名已经注册！").c_str(),G2U("警告").c_str());
#endif
		}
		else if (strcmp(strCommand,"IGNORED")== 0)
		{
#ifdef WIN32
			CCMessageBox("注册失败！请联系GM处理！","警告");
#else
			CCMessageBox(G2U("注册失败！请联系GM处理！").c_str(),G2U("警告").c_str());
#endif
		}
	}
	colseSockets();
	//CCMessageBox(naccIp.c_str(),"连接服务器成功");
}


// 关闭Socket
void KuiAccreg::colseSockets()
{
	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	if (SOCKET_ERROR == ::setsockopt(s, 
		SOL_SOCKET, 
		SO_LINGER, 
		(char *)&lingerStruct, 
		sizeof(lingerStruct)))
	{
	}
#ifdef WIN32
	if(closesocket(s) == SOCKET_ERROR)
		s = INVALID_SOCKET;
#else
	close(s);
#endif
	s = INVALID_SOCKET;
}

//设置确定取消按钮,参数：调用层对象，调用层回调函数
void KuiAccreg::setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun)
{
	m_callbackListener = callbackListener;
	m_callfun = callfun;

}
//执行上层对象的回调函数，关闭弹出框
void KuiAccreg::buttonCallBackFunc(CCObject * pSender)
{
	CCNode * node = NULL;
	if (pSender)
		node = dynamic_cast<CCNode *>(pSender);
	//node->setTag(3000);//设置tag，在调用层可以获取到
	if (m_callfun &&  m_callbackListener)
		(m_callbackListener->*m_callfun)(node);

	this->removeFromParentAndCleanup(true);
}

//关闭弹出框
void KuiAccreg::closePopLayer(CCObject * pSender)
{
	this->removeFromParentAndCleanup(true);
}

//重写触摸注册函数，重新给定触摸级别
void KuiAccreg::registerWithTouchDispatcher()
{
	//这里的触摸优先级设置为－128，与CCMenu同级，保证了屏蔽下方的触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,2, true);
}

CCRect KuiAccreg::getRect(CCNode* pNode,int i)
{
	CCRect  rc;
	rc.origin     = pNode->getPosition(); //这个因该是图内偏移
	//visibleSize
	rc.size       = pNode->getContentSize();

	rc.origin.x   = m_origin.x - m_size.width/2 + rc.origin.x; //起点最左边
	rc.origin.y   = m_origin.y - m_size.height/2+ rc.origin.y; //最上边    //- rc.origin.y
	
	return rc;
}
//触摸函数ccTouchBegan，返回true
bool KuiAccreg::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCSetIterator it = pTouch->begin();
	//CCTouch* touch = (CCTouch*)(*it);
	//containsPoint(convertTouchToNodeSpaceAR(touch))
	CCPoint location = pTouch->getLocation();//目的的坐标?屏幕点击处
	//case SMCT_UI_RENASCENCE:	             //选择重生	
    //buttonCallBackFunc(NULL);
	return true;
}

void KuiAccreg::update(float delta)
{
	//CCMessageBox("无限循环","update");
	if (isOpen)
	{
	   // m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	  //	m_nXu    = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_XU, 0, 0);
	}
}

void KuiAccreg::draw()
{
	//CCMessageBox("无限循环","draw");
}


int KuiAccreg::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
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

void KuiAccreg::editBoxEditingDidBegin(CCEditBox *editBox)//开始时的函数
{
	CCMessageBox("点击开始","点击开始");
}
void KuiAccreg::editBoxEditingDidEnd(CCEditBox *editBox) //结束时的函数
{

}
void KuiAccreg::editBoxReturn(CCEditBox *editBox)         //输入完后点击回车时的处理
{

}
void KuiAccreg::editBoxTextChanged(CCEditBox *editBox, const std::string &text)//文字更改函数
{

}