#ifndef __GAME_KTRANSENCE_H__
#define __GAME_KTRANSENCE_H__
/*
//登陆界面　输入帐号密码
*/
#include "cocos2d.h"
#include "cocos-ext.h"
#include <gameui/Uiglobaldata.h>
#include<list>
#include "ImageStore/KImageStore2.h"
#include "engine/Kgetinidata.h"
#include "physics_nodes/CCPhysicsSprite.h" 
#include "network/Thread.h"
#include "engine/KGbktoUtf8.h"
#include "engine/KTimer.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;

class KTranSence : public CCLayer,protected CThread//,public CCEditBoxDelegate
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	KTranSence();
	~KTranSence();
	virtual bool init();  
	/*virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
   */
	using CThread::Start;
	virtual void update(float delta); 
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();
	CREATE_FUNC(KTranSence);
	//重写draw()函数
	virtual void draw();
	virtual void  Run();
	//KImageStore2 m_ImageStore;
private: 
	void loadSuccess();
	//读取的回调函数
	void loadingCallBack(CCObject *obj);
	void updateProgress(float dt);
	// a selector callback
	void mianCancelCallback(CCObject* pSender);
	// implement the "static node()" method manually
	//读取开始时候的进度条
	CCSprite *m_pLoadBarStart;
	//读取完成时候的进度条
	CCProgressTimer *m_pLoadBarEnd;
	//线程相关的函数
	//总的加载图片数
	int m_nAllLoadResources;
	//当前加载图片数
	int m_nLoadedResources;
	//读取进度
	float m_fProgressIndex;
	CCLabelTTF *m_pLabelLoading;
	CCLabelTTF *m_pLabelPercent;
	//	当要测试有m_fWaitTime的效果的时候，将下一行与update()和init()里面初始化的注释取消掉即可
	//	float m_fWaitTime
	//====iKNetMsgTargetObject接口函数====
	void	AcceptNetMsg(void* pMsgData);	//接受网络消息
	LOGIN_LOGIC_STATUS			m_Status;  //发送标识变量
	LOGIN_LOGIC_RESULT_INFO		m_Result; //接受标识变量
	CCLabelTTF* pMainLabel;
	//CCLabelTTF* pBugLabel;
	int       m_nWidth;
	int       m_nHeight;
	int       m_sprCount;
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	//virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	// default implements are used to call script callback if exist
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	// default implements are used to call script callback if exist
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
	virtual void didAccelerate(CCAcceleration* pAccelerationValue);

	void    startload();//动作完成后 执行删除精灵
	void	ClearAccountPassword(bool bAccount, bool bPassword);
	void	ClearCachedata();
	void    mExit(CCObject* pSender);
private:			//角色列表返回
	float m_ScaleX;
	float m_ScaleY;
	int   m_NpcIndex; //客户端主角的NPC索引
	//char nBugInfo[128];
};

#endif // __GAMEWORLD_SCENE_H__
