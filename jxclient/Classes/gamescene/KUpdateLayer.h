//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KList.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	KUPDATELAYER_H
#define	KUPDATELAYER_H
#pragma once
//---------------------------------------------------------------------------
#include "cocos2d.h"
#include "engine/KPakList.h"
#include "engine/KDownLoadFile.h"
USING_NS_CC;
#include "cocos-ext.h"
#include "SimpleAudioEngine.h"
USING_NS_CC_EXT;
#include "AssetsManager/AssetsManager.h"
//#include "network/ODSocket.h"
#include "network/ClientStage.h"
#include "network/Thread.h"
#include "LoginDef.h"
using namespace CocosDenshion;
// 热更新实现示例
struct Kpaklistdata
{
	char    pakName[128];
	int     _pakreload;
	bool    _isbreak;
};
class KUpdateLayer:public CCLayer, public AssetsManagerDelegateProtocol,protected CThread
{
public:
	KUpdateLayer();
	~KUpdateLayer();
	static CCScene* scene(){
		CCScene* scene = CCScene::create();
		scene->addChild(KUpdateLayer::create());
		return scene;
	};

	static KUpdateLayer* create(){
		KUpdateLayer* pLayer = new KUpdateLayer;
		if (pLayer && pLayer->init())
		{
			pLayer->autorelease();
			return pLayer;
		}
		delete pLayer;
		return NULL;
	};
	
	// 初始化
	bool init();
	using CThread::Start;
	//CThread UpThread;
	//virtual void onEnter();
	// 菜单回调函数
	void reset(CCObject* pSender);					// 重置版本
	void getClientVersion(CCObject* pSender);	    // 获取当前客户端版本号
	void checkUpdate(CCObject* pSender);		    // 检查是否有版本更新
	void updates(CCObject* pSender);				// 更新版本
	void enterScene(CCObject* pSender);				// 进入场景，如果未更新屏幕中间会显示叹号的图片，更新后会显示另一张图片
	//ODSocket * sock_client;
	virtual void update(float delta);
	virtual void Run();
protected:
	// 初始化下载目录
	void initDownloadDir();
	void creatDownloadDir(std::string nTempPath);
	// 删除目录
	void deleteDir(std::string dir);
private:
	downloadFile *downPtr;
	//读取开始时候的进度条
	CCSprite *m_pLoadBarStart;
	//读取完成时候的进度条
	CCProgressTimer *m_pLoadBarEnd;
	//线程相关的函数
	//总的加载图片数
	int m_nAllLoadResources;
	//当前加载图片数
	int m_nLoadedResources;
	int m_nLoadedFinish;
	//读取进度
	float m_fProgressIndex;
	CCLabelTTF *m_pLabelLoading;
	CCLabelTTF *m_pLabelPercent;
	void    startload();//动作完成后 执行删除精灵

	CCLabelTTF* m_label;
	std::string m_downloadDir;
	std::string m_WritablePath;
	void copyData(const char* pFileName);
	int isFileExist(const char* pFileName);
	void loadSuccess();
	//读取的回调函数
	void loadingCallBack(CCObject *obj);
	void loadingCallBack_new(const char * nFileNane,int nTalSize,int nCurSize);
	//CGameClient *nAcclient;
	//int m_bIsClientConnecting;
	//int m_bIsGameSevConnecting;
	enum UpdataState
	{//升级的状态
		STATE_NONE_UP,//空闲
		STATE_RUN_UP,//正在运行
		STATE_FLS_UP,//完成
		STATE_ON_FIEL_UP,
		STATE_ON_URLFIEL_UP,
		STATE_ON_NET_UP,
		STATE_LOAD_LISTDATA,
		STATE_DWON_LISTDATA,
	};
	size_t writelen;
	std::string nCurpFileName;
	int nPakCount;
    char downUrlmask[64];
	int nDownState;
	Kpaklistdata* pListData;
	int  pakDownCount;
	int  isOpenDown;
	//int  pakreload;
	unsigned int errorLoop;
	bool isHaveClient;
	bool isOver;
	bool m_IsInPutIpPadIsOpen;
	void startInPutCallback(CCNode *pNode);
	//std::vector<std::strging> searchPaths;

};

#endif
