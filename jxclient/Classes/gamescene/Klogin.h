#ifndef __GAMEWORLD_SCENE_H__
#define __GAMEWORLD_SCENE_H__

#include "cocos2d.h"
//#include <collections.h>
#include<list>
//#include "ODSocket.h"
//#include "Box2D.h"
#include "cocos-ext.h"                      //用CCPhysicsSprite类需要导入此头文件
#include "ImageStore/KImageStore2.h"
#include "engine/KSprCodec.h"
#include "physics_nodes/CCPhysicsSprite.h" 
#include "cocos-ext.h"
#include "engine/KGbktoUtf8.h"
#include "engine/KTimer.h"

USING_NS_CC_EXT;

#include "gamescene/KHRocker.h"
class KgameWorld : public cocos2d::CCLayer
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
	void mianSkillCallback(CCObject* pSender);
	CCLabelTTF* GetTestLabel(CCObject* pSender){return pMainLabel;};
	// implement the "static node()" method manually
	CREATE_FUNC(KgameWorld);
	//重写draw()函数
	virtual void draw();
	KImageStore2 m_ImageStore;
	KTimer start;
	CCSize visibleSize;
	CCPoint origin;
private: 
	HRocker   *joystick;
	PBYTE     pWW;
	CCAnimate* animate;
	CCTexture2D* m_pTexture;
	CCLabelTTF* pMainLabel;
	KSprCodec SprDecode;
	CCSprite *testSprite;
	int       nTestCount;
	UINT      nTimeCount;
	int       m_nWidth;
	int       m_nHeight;
	int       m_sprCount;
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	void  buttonCallback(CCNode *pNode);
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

	void   beiyongb(CCObject* pSender);
	void   beiyonga(CCObject* pSender);
	void   nextsenc(CCObject* pSender);
	void   delspr(CCObject* pSender);
	void   addspr(CCObject* pSender);
	void   addmap(CCObject* pSender);
	

	void   finish();//动作完成后 执行删除精灵
	void   endSprite();
};

#endif // __GAMEWORLD_SCENE_H__
