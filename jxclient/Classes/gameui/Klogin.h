#ifndef __GAME_KLOGIN_H__
#define __GAME_KLOGIN_H__
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

#include "engine/KGbktoUtf8.h"
#include "engine/KTimer.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;

class Klogin : public CCLayer//,public CCEditBoxDelegate
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	
	virtual bool init();  
	/*virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
   */
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();
	
	//CCLabelTTF* GetTestLabel(CCObject* pSender){return pMainLabel;};
	// implement the "static node()" method manually
	CREATE_FUNC(Klogin);
	//重写draw()函数
	virtual void draw();
	virtual void update(float delta); 
	//KImageStore2 m_ImageStore;
private: 
	//====iKNetMsgTargetObject接口函数====
	void	AcceptNetMsg(void* pMsgData);	//接受网络消息
	LOGIN_LOGIC_STATUS			m_Status;  //发送标识变量
	LOGIN_LOGIC_RESULT_INFO		m_Result; //接受标识变量

	CCAnimate* animate;
	CCLabelTTF* pMainLabel;
	
	CCSprite *testSprite;
	int       nTestCount;
	UINT      nTimeCount;
	int       m_nWidth;
	int       m_nHeight;
	int       m_sprCount;
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	// a selector callback
	void mianSkillCallback(CCObject* pSender);
	void mianLoginCallback(CCObject* pSender);
	void mianCancelCallback(CCObject* pSender);
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

	void   finish();//动作完成后 执行删除精灵
	void   mExit(CCObject* pSender);
	void	GetAccountPassword(char* pszAccount, KSG_PASSWORD* pPassword);
	void	SetAccountPassword(const char* pszAccount, const KSG_PASSWORD* pcPassword);
	void	ClearAccountPassword(bool bAccount, bool bPassword);
private:
	//====各操作的网络响应的返回处理====
	void	ProcessAccountLoginResponse(KLoginStructHead* pResponse);		//处理账号登陆的响应
	void	ProcessRoleListResponse(TProcessData* pResponse);				//角色列表返回
};

#endif // __GAMEWORLD_SCENE_H__
