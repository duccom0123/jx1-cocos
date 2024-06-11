#ifndef __GAMEWORLD_SCENE_H__
#define __GAMEWORLD_SCENE_H__

#include "cocos2d.h"
#include<list>
#include "cocos-ext.h" //用CCPhysicsSprite类需要导入此头文件
#include "gameui/Uiglobaldata.h"
#include "ImageStore/KImageStore2.h"
#include "engine/Kgetinidata.h"
#include "physics_nodes/CCPhysicsSprite.h" 
#include "engine/KGbktoUtf8.h"
#include "engine/KTimer.h"
#include "engine/KLinkArray.h"
#include "gamecore/GameDataDef.h"
#include "gameui/KuiChatList.h"
#include "gameui/KuiMyMenu.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
#define MAX_NUM_REGIONS	9
#define MAX_SPR_COUNT  165
#define MAX_TREES_COUNT  100
#define MAX_HOUSE_COUNT  100
#define MAX_ABOVE_COUNT  100
#define MAX_FUZHUSKILL_COUNT 5
#define MAX_MSGREV_COUNT 5

#include "gamescene/KHRocker.h"
#include "gamescene/KSkillRocker.h"

struct mapRegData
{
	KLinkArray		m_FreeIdxNode;				//	可用表
	KLinkArray		m_UseIdxNode;				//	已用表

	KLinkArray		m_FreeIdxPartNode;			//	可用表
	KLinkArray		m_UseIdxPartNode;			//	已用表

	int             nDrawKind;
	int             nSprCount;
};

struct msgListData
{
	int             nindex;      //当前的索引
	int             nkind;       //信息的类型
	float           offsetY;     //当前高度的偏移
	char            m_msgContent[256];//内容
};

struct skillstickData
{
	KSkillRocker  *skillstick;
	CCSprite      *controlSprite_s;
	CCSprite      *ygSprite_s;
};

struct auxiliarySkillData
{
	//CCSprite *auxiliaryskillSpr;
	CCMenuItemSprite* auxiliaryskillSpr;
	float OffsetX,OffsetY;
	int m_skillidx;
};
//快捷物品描绘 
class KImmediaItem
{
public:
	int m_nIndex;
	//int m_nIsSKill;
	//int m_mIsFull;
	int m_uid;
	//DWORD m_TextColor;
	KImmediaItem() :m_nIndex(-1){} //, m_TextColor(0xFFFFFF00)
//	int	Init(KIniFile* pIniFile, const char* pSection);
};

class KgameWorld : public cocos2d::CCLayer//,public ui::Layout
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	KgameWorld();
	~KgameWorld();
	virtual bool init();  
	/*virtual void onEnter();
    virtual void onExit();
    virtual void onEnterTransitionDidFinish();
   */
	virtual void update(float delta);
	virtual void draw();
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();
	CREATE_FUNC(KgameWorld);
	

	//void registerWithTouchDispatcher();
	KImageStore2 m_ImageStore;
	KTimer start;
	void setIsPaint(bool ver);
	void KickOutGame();
	void MoveObject(int ObjKind,int nIndex,bool nIsClear,int nPosX=0,int nPosY=0,int nPosZ=0);
	void RemoveObject(int ObjKind,int nIndex);
	void ergodicAllNode(CCNode *rootNode);
	void SetHorseState(int nNpcIndex);
	void SetPiFengState(int  nNpcIndex);
	//CCTexture2D * ccgetTxtData(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh);
	//CCTexture2D * ccgetTxtData_g(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh);
	void DrawPrimitives_State(int nNpcIndex,int nstateIdx,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord);
	void DrawPrimitives_MenuState(int nNpcIndex,int nstateIdx,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord);
	void DrawShadow(int nNpcIndex,int nPos, KRUImage* pPrimitives);
	void DrawPrimitives(int nNpcIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord);
	void DrawPrimitives_obj(int nObjIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,ccColor3B nObjColor,char *nObjName,char *_clientObjName);
	void DrawSpecial_miss(int nMissIndex,int eStatus,int nNpcIndex,unsigned int nNpcDwidx,int nPos,KRUImage* pPrimitives,bool isClear ,unsigned int uGenre,int nOrDer,int bSinglePlaneCoord);
	void DrawPrimitives_miss(int nMissIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord);
	void DrawPrimitives_map(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont,POINT nRegRbMpsPoint);
	void _DrawPrimitives_map(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont,POINT nRegRbMpsPoint);
	void Draw_Part_map(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont,POINT nRegRbMpsPoint);
	void DrawPrimitives_BigMap(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont);
	void DrawPrimitives_BigMap_Tree(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont);
	void DrawPrimitives_BigMap_house(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont);
	void _DrawPrimitives_BigMap(int nRegIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord,POINT nRegPiont);
	void DrawPrimitives_RolePoint(int nRegIndex,int nPos, KRepresentUnit* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer,int bSinglePlaneCoord);
	void DrawPrimitives_minemap(int nposX,int nposY, RECT pRc,std::string nJpaPath,CCTexture2D *nMapTxtture=NULL);
	//CCNode *GetMainWorldNode(int nNodeKind);
	void DrawPrimitives_NpcBlood(int nNpcIndex,int bloodindex,char *strBloodNo,int nNpcMpsX,int nNpcMpsY,int nOffsetY,bool isClear);
	void DrawPrimitives_NpcOther(int nNpcIndex,int m_Kind,bool isClear = false);
	void DrawAboveHead(int index,POINT nRegionIndex,int nCount,KRUImage* pPrimitives,int nKind,int bSinglePlaneCoord=false);
	void setBuWeiHide(int nNpcIndex,bool isHide,int isthis);
	void _setBuWeiHide(int nNpcIndex,bool isHide,int isthis);
	void taskCilentMsgArrival(KNewsMessage* pMsg, int pTime,int nIsBtn);
	void taskCilentFindPathArrival(KTaskPathInfo* pMsg,int pTime,int nIsBtn);
	//void  AddIndex(int nRegIndex,int i);
	//void  RemoveIndex(int nRegIndex,int i);
	//int   GetCount(int nRegIndex);
	//void  ReSetUseNode(int nRegIndex);
	//void  ReSetUsePartNode(int nRegIndex);
	//void  ReSetTreeNode();
	//void  ReSetHouseNode();
	//int   GetTreeNodeCount();
	//int   GetHouseNodeCount();
	/*void  SetOtherSprCount(int nRegIndex,int i);
	int   GetOtherSprCount(int nRegIndex);
	void  SetPartSprCount(int nRegIndex,int i);
	int   GetPartSprCount(int nRegIndex);
	int   GetRegDrawKind(int nRegIndex);
	int   GetRegSprCount(int nRegIndex);
	void  AddPartIndex(int nRegIndex,int i);
	*/
	void  setattackSprInfo(int nSkillIdx,int uGenre,char * icoPath);
	void  setaauxiliaryskillInfo(int nSkillIdx,int uGenre,char * icoPath);
	//聊天系统
	 void ChannelMessageArrival(UINT nChannelID, char* szSendName,char* pMsgBuff, unsigned short nMsgLength, bool bSucc,UINT nItemDwidx=0,int nPackage=-1);
	 //私聊显示(显示对方的信息)（密）
	 void MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc,UINT nItemDwidx=0,bool isClent=false);
	 void NotifyChannelID(char* ChannelName, UINT channelid, BYTE cost);
	 bool AMessageArrival(KSystemMessage* pMsg, void* pParam);

	 //聊天
	 void QueryAllChannel();
	 //---------------------------------
	 void CreatNpcDialog(KUiQuestionAndAnswer* pContent, KUiNpcSpr* pImage=NULL,int nIsTimeClose=0);
	 void CreatTalkDialog(KUiInformationParam* pWordDataList, int nCount);
	 void CreatNpcShopDialog();
	 bool GetuiItemIfVisible(){return m_itemsPadIsOpen;};
	 bool GetuiItemExIfVisible(){return m_itemExPadIsOpen;};
	 bool GetuistoreIfVisible(){return m_storeboxPadIsOpen;};

	 void UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
	 void UpdateItemEx(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
	 void UpdateStoreBox(KUiObjAtRegion* pItem, int bAdd);
	 void UpdateExBox(KUiObjAtRegion* pItem, int bAdd,int exBoxIndex);
	 void openstringBox(const char* pszTitle,int nMinLen, int nMaxLen,const char* pszCallback,const char* pszError,int bNum);
	 void openStorebox();
	 void openNpcshop(int nshopidx);
	 void openProgressbar(KUiJinDuTiaoInfo* pJinDu=NULL);
	 void openlaotou();
	 void openGive(char* pszTitle, char* pszInitString,int nNum,char* pszCallback,char* pszError);
	 void OpenViewStateDialog(KUiPlayerItem* pDest);
	 void OpenPlayerShopDialog(KUiPlayerItem* pDest);
	 void OpenPlayerTradeDialog(KUiPlayerItem* pDest);
	 void OpenItenLinkDialog(KUiLianItem*  pDest);
	 void OpenFlyNewsDialog_a(KNewsMessage* pMsg, SYSTEMTIME* pTime);
	 void OpenFlyNewsDialog_b(KNewsMessage* pMsg, SYSTEMTIME* pTime);
	 void OpenCreatTongSheet();
	 void OnSelfChangedItem(KUiObjAtRegion* pObj, int bAdd);
	 void OnTarChangedItem(KUiObjAtRegion* pObj, int bAdd);
	 void UpdateTradeOperData();
	 void UpdateTradeEnd();	//交易结束

	 void UpPlayerShopData();
	 void ClosePlayerShop();

	 void FreeAllSprite();
	 void UpdateSkill(KUiSkillData* pSkill, int nIndex);
	 void removespriteByIdx(int nNpcIndex,int nStateIdx,bool isAll=false);
	 void removeMenuByIdx(int nNpcIndex,int nStateIdx);
	 bool CheckImmediacyBox(int nGenre, int DetailType);
	 void UpdateImmediaItem(int nIndex, unsigned int uGenre, unsigned int uId);	//变更物品摆换
	 void UpdateGiveItem(KUiObjAtRegion* pItem, int bAdd);

	 void UpdateDataTeam(KUiPlayerTeam* pInfo);
	 void setchatDelegate(bool isYes);
	 void setchatCurChannel(char  *name);
	 void setchatInputText(char  *text);
	 int  getMapIdx(){return nMapidx;};
	 void setPingTime(unsigned int ver){__pingTime=ver;};
	 void setPadCanMove(int _PadIndex,bool ver);
	 CCNode   *ParentNode_map;//
	 CCNode   *ParentNode_Bigmap;
	 CCNode   *ParentNode_objabove;
	 void setAttackSprVisible(bool ver);
	 void setAttackSprPosition(CCPoint nPos);
	 int  getUseSkillModel(){return m_useSkillModel;};
	 void customSkillAttack(int nSkillid,float xPos,float yPos);
	 void setInputMsg(char * strInfo);
	 void _quitGame();
private:
	char     _strPing[64];
	CCNode   *ParentNode_rolestate;
	CCNode   *ParentNode_chat;
	CCNode   *ParentNode_npc;
	CCNode   *ParentNode_npc_state;//状态
	CCNode   *ParentNode_npcother;
	CCNode   *ParentNode_npcblood;
	CCNode   *ParentNode_player;
	CCNode   *ParentNode_miss;
	CCNode   *ParentNode_miss_Special;
	CCNode   *ParentNode_item;    //9个快捷栏
	CCNode   *ParentNode_obj;
	CCNode   *ParentNode_ShortSkill;
	//CCNode   *ParentNode_RightSkill;
	CCNode   *ParentNode_minmap;
	//mapRegData *RegData;
	HRocker   *joystick;
	//KSkillRocker  *skillstick;
	//skillstickData skillstick[MAX_FUZHUSKILL_COUNT];
	//CCAnimate* animate;
	CCLabelTTF* pmanaLabel;
	CCLabelTTF* plifeLabel;
	CCLabelTTF* pSaitamaLabel;
	CCLabelBMFont* pExpLabel;
	CCLabelBMFont* pPointLabel;
	CCLabelTTF* pMapNameLabel;
	CCLabelTTF* pRoleNameLabel;
	CCLabelTTF* pNetStateLabel;
	CCLabelBMFont* ptestLabel;
	CCMenuItemSprite *ChannelSpr;

	CCTexture2D * ccgetTxtData(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh);
	CCTexture2D * ccgetTxtData_g(char *sprPath,int nFramIndex,int nMpsX,int nMpsY,int nMpsZ,int *mScreeX,int *mScreeY,int bRenderFlag,int bSinglePlaneCoord,int *mHeadw,int *mHeadh);

	CCNode *PaintPifeng(int m_PifengType,CCNode *nNpcOtherNode,int nMpsX,int nMpsY);
	CCNode *PaintHonor(int nbtRankFFId,CCNode *nNpcOtherNode,int nMpsX,int nMpsY);
	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);

	void qiangItemCallback(CCObject* pSender);
	void playerSellCallback(CCObject* pSender);
	void playerfunCallback(CCObject* pSender);
	void delmapCallback(CCObject* pSender);
	void mianYinCallback(CCObject* pSender);
	void mianQuCallback(CCObject* pSender);
	void mianSortSillCallback(CCObject* pSender);
	void huodongCallback(CCObject* pSender);
	void autoCallback(CCObject* pSender);
	void caveCallback(CCObject* pSender);
	
	int  getIndexBySkillID(int nSkillIdx);
	//交易
	void mianExcCallback(CCObject* pSender);
	void auxiliaryskillCallback(CCObject* pSender);
	void onsendCallback(CCObject* pSender);
	void ImmediaCallback(CCObject* pSender);
	//PK
	void mianPkCallback(CCObject* pSender);
	void mSkillsCallback(CCObject* pSender);
	void mOptionsCallback(CCObject* pSender);
	void mTaskCallback(CCObject* pSender);
	void mPaimaiCallback(CCObject* pSender);
	void mQizCallback(CCObject* pSender);
	void mFactionCallback(CCObject* pSender);
	void mTeamCallback(CCObject* pSender);
	void mItemExCallback(CCObject* pSender);
	void mItemsCallback(CCObject* pSender);
	void mRoleStatusCallback(CCObject* pSender);
	void mianSkillCallback(CCObject* pSender);
	void useModelCallback(CCObject* pSender);
	void mianHorseCallback(CCObject* pSender);
	void mianSitCallback(CCObject* pSender);
	void mianRunCallback(CCObject* pSender);
	void pickCallback(CCObject* pSender);
	void hidechatCallback(CCObject* pSender);
	void maxmapCallBackFunc(CCObject* pSender);
	void  copyData(const char* pFileName);
	bool  isFileExist(const char* pFileName);
	void  buttonCallback(CCNode *pNode);
	void  buttonSkillCallback(CCNode *pNode);
	void  closeSkillPadCallback(CCNode *pNode);
	void  closeItemPadCallback(CCNode *pNode);
	void  closeTeamPadCallback(CCNode *pNode);
	void  closeRolePadCallback(CCNode *pNode);
	void  closeItemExPadCallback(CCNode *pNode);
	void  closeNpcDialogPadCallback(CCNode *pNode);
	void  closeTaikDialogPadCallback(CCNode *pNode);
	void  closestorePadCallback(CCNode *pNode);
	void  closeNewDialogPadCallback(CCNode *pNode);
	void  NpcshopPadCallback(CCNode *pNode);
	void  getStringPadCallback(CCNode *pNode);
	void  progressPadCallback(CCNode *pNode);
	void  closemapPadCallback(CCNode *pNode);
	void  closesortskillPad_r(CCNode *pNode);
	void  closesortskillPad_l(CCNode *pNode);
	void  closeLaoPadCallback(CCNode *pNode);
	void  closegivePadCallback(CCNode *pNode);
	void  closeChatlistPad(CCNode *pNode);
	void  closeplayerfunPadCallback(CCNode *pNode);
	void  autoPadCallback(CCNode *pNode);
	void  cavePadCallback(CCNode *pNode);
	void  closeViewPadCallback(CCNode *pNode);
	void  closesellItemPadCallback(CCNode *pNode);
	void  playershopPadCallback(CCNode *pNode);
	void  closetradePadCallback(CCNode *pNode);
	void  closeDyshopPadCallback(CCNode *pNode);
	void  closeMaxMapPadFunc(CCNode* pNode);
	void  closeLinkPadCallback(CCNode* pNode);
	void  closeShowSpritePad(CCNode* pNode);
	void  closetaskPadCallback(CCNode* pNode);
	void  closeTongPadCallback(CCNode *pNode);
	void  closeescPadCallback(CCNode *pNode);
	void  closeTongSheetPadCallback(CCNode *pNode);
	void  attackFinish();
	//virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	// default implements are used to call script callback if exist
	/*virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	*/
	// default implements are used to call script callback if exist
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
	virtual void didAccelerate(CCAcceleration* pAccelerationValue);
	void   setadjustColor(int nIndex,CCSprite *nResSpr);
	//void   HoldObject(int nType,int nBoxIndex,int nItemidx,bool isAdd);
	void   HoldObject_(int nType,int nBoxIndex,int nItemidx,bool isAdd);
	void   TempUpdataImmediacy();
	int    getImmediacyBoxindex(int nidx);
	void   selchatlistItemEvent(CCObject* pSender,ui::ListViewEventType type);

	int    m_NpcIndex;                    //客户端主角的NPC索引
	//char   nBugInfo[256];
	//KCriticalSection    m_GameProcessLock;//互斥锁
	unsigned int nLoopCount;
	//int   nRegSprCount[MAX_NUM_REGIONS];
	//int   nOtherSprCount[MAX_NUM_REGIONS];
	//int   nPartSprCount[MAX_NUM_REGIONS];
	//int	  FindFree(int nRegIndex);
	//int	  FindPartFree(int nRegIndex);
	//int	  FindTreeFree();
	//int	  FindHouseFree();
	//void  AddTreeIndex(int i);
	//void  AddHouseIndex(int i);
	/*KLinkArray		m_FreeIdxTreeNode;			//	可用表
	KLinkArray		m_UseIdxTreeNode;			//	已用表

	KLinkArray		m_FreeIdxHouseNode;			//	可用表
	KLinkArray		m_UseIdxHouseNode;			//	已用表
	*/
	//KLinkArray		m_FreeIdxAboveNode;			//	可用表
	//KLinkArray		m_UseIdxAboveNode;			//	已用表

	POINT	m_MapPos;
    SIZE	m_MapSize;
	char nMapName[64];
	char nMapKind[32];
	int  nMapStartLeft;
	int  nMapStartTop;
	int  nMapidx,nPosX,nPosY;
	bool m_skilPadIsOpen;
	bool m_itemsPadIsOpen;
	bool m_playerSellPadIsOpen;
	bool m_teamPadIsOpen;
	bool m_RoleStatePadIsOpen;
	bool m_itemExPadIsOpen;
	bool m_NpcDialogPadIsOpen;
	bool m_talkDialogPadIsOpen;
	bool m_shopDialogPadIsOpen;
	bool m_storeboxPadIsOpen;
	bool m_npcshopPadIsOpen;
	bool m_getstringPadIsOpen;
	bool m_talkDialogPadIsOpen_1;
	bool m_ProgressPadIsOpen;
	bool m_movemapPadIsOpen;
	bool m_sortskilPadIsOpen_r;
	bool m_sortskilPadIsOpen_l;
	bool m_laotouPadIsOpen;
	bool m_givePadIsOpen;
	bool m_autoPadIsOpen;
	bool m_cavePadIsOpen;
	bool m_playerfunPadIsOpen;
	bool m_viewPadIsOpen;
	bool m_tanweoPadIsOpen;
	bool m_tradePadIsOpen;
	bool m_dyshopPadIsOpen;
	bool m_maxMapPadIsOpen;
	bool m_itemLinkPadIsOpen;
	bool m_taskPadIsOpen;
	bool m_tongPadIsOpen;
	bool m_escPadIsOpen;
	bool m_tongCreatPadIsOpen;
	//CCRenderTexture *nRegCanvas; //创建一个区域画板
	//主技能按钮
	CCMenuItemImage *pMainSkillItem;
	CCMenuItemImage *psortSkillItem_l;
	CCMenu* sortskillMenu;
	CCMenu* pcontrolMenu;
	CCMenu* ImmediaMenu_1;
	CCMenu* ImmediaMenu_2;
	CCMenu* ImmediaMenu_3;
	CCMenu* pminMapMenu;
	KuiMyMenu* KskillMenu;
	CCSprite *attackSpr_dir;
	auxiliarySkillData auxiliaryskill[MAX_FUZHUSKILL_COUNT];
	//接受信息的最大数量的数据缓存，超过将删除
	msgListData       m_msgChannelData[MAX_MSGREV_COUNT];
	CCPoint           screenPoint;
	int               mainattackSkill;
#define	UPB_IMMEDIA_ITEM_COUNT	9    //宏声明 技能框
	KImmediaItem m_ImmediaItem[UPB_IMMEDIA_ITEM_COUNT];
	int m_ImmediaItemSelIndex;
	bool isClose;
	CCEditBox *inputEditBox;
	int  nCurChannelMsgCount;
	//ui::ListView* chatlistView;
	KuiChatList * chatLayer;
	//ui::ListView *chatListView;
	/*CCAction* freeze;
	CCAction* stun;
	CCAction* poison;
	CCAction* red;*/
	int chatoffsetY;
	bool isHave;
	CCLayer * sfunLayer;

	char	m_cPreMsgCounter;
	char	m_cLatestMsgIndex;
#define	MAX_RECENT_MSG_COUNT	8
	char	m_RecentMsg[MAX_RECENT_MSG_COUNT][512];
	int m_nCurChannel;
#define	MAX_RECENTPLAYER_COUNT	10
	char m_RecentPlayerName[MAX_RECENTPLAYER_COUNT][32];
	int m_nRecentPlayerName;

	char m_szSelfName[64];	//自己的名字
	int  AddRecentPlayer(const char* szName);
	void ReplaceSpecialName(char* szDest, size_t nDestSize, char* szSrc);
	int  GetChannelIndex(const char* pTitle);
	void OnSendChannelMessage(DWORD nChannelID, const char* Buffer, int nLen);			//发送频道聊天到服务器
	void OnSendSomeoneMessage(const char* Name, const char* Buffer, int nLen,BOOL nIsItemLink=FALSE);
	void InputCurrentChannel(int nIndex, bool bFocus);
	int  IsHasCost(BYTE cost, int nMoney, int nLevel, int nMana, int nFullMana, int&nUseMoney, int&nUseLevel, int&nUseMana);
	void OnDirectSendChannelMessage(DWORD nChannelID, BYTE cost, const char* Buffer, int nLen,int nItemDwIdx=0);
	bool IsSelfName(char* szName)
	{
		if (szName)
		{
			return (strcmp(m_szSelfName,szName) == 0);   //对比两个字符串 相同 ==0
		}
		return false;
	}
#define SECOND_AUTODELMSG 20000
	unsigned int	m_uAutoDelMsgInterval;	//自动删除消息的间隔时间，单位毫秒
	unsigned int	m_uLastDelMsgTime;		//上次删除消息时的时间
	unsigned int    __pingTime;
	unsigned int    m_uLastSwitchTime;
	unsigned int    m_recontnetTime;
	CCAnimation     *animation_attack;
	CCSprite        *_Maniattack_State;
	CCAnimate       *Attackanimate;
	CCActionManager* actionManager;
	int             m_useSkillModel;
};

#endif // __GAMEWORLD_SCENE_H__
