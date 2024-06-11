#ifndef __GAME_KLOGIN_F_H__
#define __GAME_KLOGIN_F_H__
/*
//登陆界面　开始界面
*/
#include "cocos2d.h"
#include "cocos-ext.h"
//#include <gameui/Uiglobaldata.h>
#include<list>
#include "ImageStore/KImageStore2.h"
#include "engine/Kgetinidata.h"
#include "physics_nodes/CCPhysicsSprite.h" 

#include "engine/KGbktoUtf8.h"
#include "engine/KTimer.h"
//#include "gamescene/KSkillRocker.h"

using namespace cocos2d;
//using namespace cocos2d::extension;
using namespace std;

class Klogin_f : public cocos2d::CCLayer//,public CCEditBoxDelegate
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	
	virtual bool init();  
	/*virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
   */
	virtual void update(float delta); 
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();
	// a selector callback
	//CCLabelTTF* GetTestLabel(CCObject* pSender){return pMainLabel;};
	// implement the "static node()" method manually
	CREATE_FUNC(Klogin_f);
	//重写draw()函数
	virtual void draw();
	KImageStore2 m_ImageStore;
	//CCSize visibleSize;
	//CCPoint origin;
private: 
	//CCTexture2D* m_pTexture;
	//CCLabelTTF* pMainLabel;
	
	CCSprite *testSprite;
	int       nTestCount;
	UINT      nTimeCount;
	int       m_nWidth;
	int       m_nHeight;
	int       m_sprCount;
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	void  buttonCallback(CCNode *pNode);
	//unsigned long g_FileName2Id_(char * lpFileName);
	//virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	void mianLoginCallback(CCObject* pSender);
	void mianCancelCallback(CCObject* pSender);
	void accRegCallback(CCObject* pSender);
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

	void closeAccCallback(CCNode *pNode);

	void finish();
	void mExit(CCObject* pSender);
	bool m_AccPadIsOpen;
	CCLayer *mainlayer;
	//KSkillRocker *skillstick_f;

};

#endif // __GAMEWORLD_SCENE_H__
