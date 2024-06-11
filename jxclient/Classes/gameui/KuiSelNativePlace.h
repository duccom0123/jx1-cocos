#ifndef __KuiSelNativePlace__
#define __KuiSelNativePlace__
/*
//登陆界面 选择人物 创建 删除等
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
//using namespace cocos2d::extension;
using namespace std;

struct KNativePlace
{
	char			szName[32];
	char			szImage[128];
	unsigned short	nId;
	unsigned short	nDescLen;
	char			sDesc[256];
	unsigned short  inRevID;  //出生点
};
#define	PLACE_LIST_FILE	 "\\Settings\\NativePlaceList.ini"
#define	PLACE_LIST_FILE_VN	 "\\Settings\\NativePlaceList_vn.ini"
class KuiSelNativePlace : public cocos2d::CCLayer,public CCRect//,public CCEditBoxDelegate
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	KuiSelNativePlace();
	~KuiSelNativePlace();
	virtual bool init();  
	/*virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
   */
	virtual void update(float delta); 
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();

	//CCLabelTTF* GetTestLabel(){return pMainLabel;};
	// implement the "static node()" method manually
	CREATE_FUNC(KuiSelNativePlace);
	//重写draw()函数
	virtual void draw();
	KImageStore2 m_ImageStore;
	//CCSize visibleSize;
	//CCPoint origin;
private: 
	//CCLabelTTF*pMainLabel;
	//CCLabelTTF* pBugLabel;
	CCLabelTTF* pDescLabel; //说明
	//====iKNetMsgTargetObject接口函数====
	void AcceptNetMsg(void* pMsgData);	//接受网络消息
	bool clicked;  //是否双击
	LOGIN_LOGIC_STATUS			m_Status;  //发送标识变量
	LOGIN_LOGIC_RESULT_INFO		m_Result; //接受标识变量
	//KRoleInfo  nCurRoleList[MAX_PLAYER_PER_ACCOUNT];
	int       m_nWidth;
	int       m_nHeight;
	int       m_sprCount;
	CCRect getRect(CCNode* pNode);
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	void  buttonCallback(CCNode *pNode);
	// a selector callback
	void mainEnterCallback(CCObject* pSender);
	void CheckClick(float tt);

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

	void ProcessToLoginGameServResponse(tagNotifyPlayerLogin* pResponse);//准备登陆游戏服务器的响应

	void finish();
	void mExit(CCObject* pSender);

	void LoadList(int nPlaceId);

	unsigned char	m_Gender[MAX_PLAYER_PER_ACCOUNT];
	unsigned char	m_Attribute[MAX_PLAYER_PER_ACCOUNT];
	char		    m_szLoginBg[32];
	char		    m_szPlayerImgPrefix[128];
	int             m_CurSelIndex;
	int   m_selIndex; //选择的索引
	CCActionManager* actionManager;
	CCNode* pListNode; //选项菜单
	CCSprite *SelbgSprite;
	CCSprite *showSprite;
	KNativePlace*	m_pPlaceList;
	int    m_itemsCount;
	int    m_nLastSelPlace;
};

#endif // __KuiSelNativePlace__
