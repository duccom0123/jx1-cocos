
#include "cocos2d.h"
#include "gamecore/CoreShell.h"
//#include "KgameWorld.h"
using namespace cocos2d;

class HRocker :public CCLayer {
public :
	//初始化 aPoint是摇杆中心 aRadius是摇杆半径 aJsSprite是摇杆控制点 aJsBg是摇杆背景
	static HRocker*  HRockerWithCenter(CCPoint aPoint ,float aRadius ,CCSprite* aJsSprite,CCSprite* aJsBg,bool _isFollowRole);
	//启动摇杆
	void Active();
	//解除摇杆
	void Inactive();
	CCPoint getDirection();
	float   getAngleSigned();
	float   getRad(CCPoint p1,CCPoint p2);
	bool    getIsRun(){return isRun;};
	void    setIsRun(bool nisRun){isRun=nisRun;};
	void    setRoleIndex(int nIdx){nRoleIndex=nIdx;};
	void    setRolePoint(CCPoint nPoint){nRolePoint=nPoint;};
	void    setCoreShell(iCoreShell * nCoreShell){pCoreShell=nCoreShell;};
	//void    setGameWorld(KgameWorld * nCoreShell){__GameWorld=nCoreShell;};
	CCPoint getMoveEndPos(){return moveEndPoint;};
private:
	HRocker * initWithCenter(CCPoint aPoint ,float aRadius ,CCSprite* aJsSprite,CCSprite* aJsBg,bool _isFollowRole);
	CCPoint centerPoint; //摇杆中心
	CCPoint currentPoint;//摇杆当前位置
	bool isRun;
	bool active;//是否激活摇杆
	float radius;//摇杆半径
	CCSprite *jsSprite;
	bool isFollowRole;//是否跟随用户点击
	float getVelocity();
	void  updatePos(float dt);  //CCTime
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	CCPoint nRolePoint;
	int     nRoleIndex;
	iCoreShell * pCoreShell;
	//KgameWorld *__GameWorld; 
	CCSize  visibleSize;
	CCPoint moveEndPoint;
	CREATE_FUNC(HRocker);
};
