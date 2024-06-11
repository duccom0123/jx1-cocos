//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __Kuiplayerfun__
#define __Kuiplayerfun__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
enum  SelfChannel   //个人频道
{
	ch_Team = 0,
	ch_Faction,
	ch_Tong,
	ch_Screen,//全部
	ch_Msgr,
	ch_GM
};

enum CHANNELRESOURCE
{
	CH_NEARBY,
	CH_TEAM,
	CH_WORLD,
	CH_FACTION,
	CH_TONG,
	CH_SYSTEM,
	CH_CITY,
	CH_MSGR,
	CH_CHATROOM,
};

enum LISTAG_CHAT
{
	CH_ALL_LIST=1,
	CH_SHIJIE_LIST,
	CH_CHENGSHI_LIST,
	CH_DUIWU_LIST,
	CH_SILIAO_LIST,
	CH_BANGPAI_LIST,
	CH_COUNT_LIST,
};

#define MAX_CHANNELRESOURCE 8  //频道总数量

struct KChannelResourceInfo
{
	char cTitle[32];
	char cShortTitle[MAX_CHANNELRESOURCE][32];
	KRColor uTextColor;
	KRColor uTextBorderColor;
	WORD nMenuPicIndex;
	WORD nMenuPicHeight;
	char cMenuText[32];
	WORD nMenuDeactivatePicIndex;
	WORD nMenuDeactivatePicHeight;
	char cMenuDeactivateText[32];
	KRColor uMenuBkColor;
	WORD nTextPicIndex;
	char cFormatName[32];	        //用于频道订阅
	int nNeverClose;
	char szSoundFileName[80];
	unsigned int uSendMsgInterval;	//用于发送消息的时间间隔(毫秒)
	unsigned int uSendMsgNum;	    //用于缓存待发送消息最大数目,最大10句

};

struct KChannelActivateInfo
{
	DWORD nChannelID;               //频道ID
	BYTE cost;
	bool bSubscribe;
	int ResourceIndex;	            //-1表示无资源
	unsigned int uLastSendMsgTime;	//上次发消息的时间
	unsigned int uLeftSendMsgNum;	//已经缓存的代发消息
	unsigned int uBufferOffset;	    //可用空间的起始位置
	char Buffer[1024 * 10];         //存放消息的
};

struct KMSN_ChannelInfo
{
	KRColor uTextColorSelf;
	KRColor uTextBorderColorSelf;
	WORD nSelfTextPicIndex;
	KRColor uTextBKColorSelf;

	KRColor uTextFriendColor;
	KRColor uTextBorderFriendColor;
	WORD nFriendMenuPicIndex;
	WORD nFriendMenuPicHeight;
	KRColor uFriendMenuBkColor;
	WORD nFriendTextPicIndex;
	char szFriendSoundFileName[80];

	KRColor uTextColorUnknown;
	KRColor uTextBorderColorUnknown;
	WORD nStrangerMenuPicIndex;
	WORD nStrangerMenuPicHeight;
	KRColor uStrangerMenuBkColor;
	WORD nStrangerTextPicIndex;
};
struct KPlayer_Chat_Tab
{
	char	szChatTabName[32];	 //频道名字？
	int		nId;
};

#define MAX_CHAT_TAB 9   //聊天频道数量

class Kuiplayerfun:public CCLayer{
public: 
	Kuiplayerfun();
	~Kuiplayerfun();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(Kuiplayerfun);

	static Kuiplayerfun * create(CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void	UpdateData(KUiPlayerTeam* pInfo);

	void	QueryAllChannel();
	void	ReplaceChannelName(char* szDest, size_t nDestSize, char* szSrc);
	int	    ReleaseActivateChannelAll();
	void	OpenChannel(char* channelName, DWORD nChannelID, BYTE cost);
	int	    PushChannelData(DWORD dwID, const char* Buffer, int nLen);
	int	    GetChannelData(DWORD& dwID, BYTE& cost, char*& Buffer, int& nLen,int& nIiemDwIdx);
	int   	PopChannelData(DWORD dwID);	
	void    _removeDelegate(bool isThis);
	DWORD   GetChannelCount(); // 取得频道的数量
	int	    GetChannelIndex(DWORD dwID);
	int	    GetChannelIndex(char* channelName);
	bool    IsChannelType(int nChannelIndex, SelfChannel type);
	char*   GetChannelTitle(int nChannelIndex);
	DWORD   GetChannelID(int nChannelIndex);
	int     GetChannelResourceIndex(int nChannelIndex);
	int     CheckChannel(int nChannelIndex, bool b);
	bool    GetChannelSubscribe(int nChannelIndex);
	int		NewChannelMessageArrival(DWORD nChannelID, char* szSendName,char* pMsgBuff, unsigned short nMsgLength,DWORD nItemDwidx=0,char *scrSendName=NULL,int nPackage=-1);
	void    ShowMSNMessage(char* szName, const char* pMsgBuff, unsigned short nMsgLength,DWORD nItemDwidx,bool isClient=false);
	int	    FindActivateChannelIndex(DWORD nChannelID);

	void    setCanMove(bool ver){__isCanMove =ver;};
private:
	//增加技能图标
	void setBtnTouchEnabled(UIListView*listView,bool mEnabled);
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	void selectedItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedPlayersListItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void btnCallBackFunc(CCObject * pSender);
	
	void Clear();
	void OnRefresh();					//刷新
	void touchPageBtnEvent(CCObject *pSender, TouchEventType type);
	//上层对象
	CCObject * m_callbackListener;
	UIButton* btnPageBox_q;//全部
	UIButton* btnPageBox_s;//世界
	UIButton* btnPageBox_c;//城市
	UIButton* btnPageBox_d;//队伍
	UIButton* btnPageBox_sl; //私聊
	UIButton* btnPageBox_m;//门派
	UIButton* btnPageBox_b;//帮派
	//回调函数
	SEL_CallFuncN m_callfun;
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize winSize;
	CCSize  bgSize;
	//对话框背景精灵
	//CCSprite * m_bgSprite;
	CCSprite *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXLabel;
	CCLayerColor * colorLayer;
	CCAction* red;

	//int  m_nMoney;
	//int  m_nXu;
	//int  m_nNumObjects;
	//POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	UILayer *m_pUiLayer;
	//Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* pNearbyPlayerslistView;
	UIListView* pListView_all;
	UIListView* pListView_shijie;
	UIListView* pListView_chengshi;
	UIListView* pListView_duiwu;
	UIListView* pListView_siliao;
    UIListView* pListView_bangpai;

	UICheckBox* checkBox;
	CCSize btnSize;
	KUiPlayerTeam	m_Info;
	KUiPlayerItem*	m_pPlayersList;
	KUiPlayerItem*	m_pNearbyPlayersList;
	int   __nSelIndex;
	int   __playrSelIndex;
	int   __playerListCount;
	int   __NearbyListCount;

	CCEditBox *pfindEditBox;
	//聊天系统
	KChannelResourceInfo m_ChannelsResource[MAX_CHANNELRESOURCE];
	int m_nChannelsResource;
	char m_DefaultChannelSendName[32];
	int m_nDefaultChannelResource;
	KChannelActivateInfo* m_pActivateChannel;
	int m_nActivateChannels;  //已经激活的频道菜单
	unsigned int m_uLastDelMsgTime;
	int	 IsNeverCloseActivateChannel(int nChannelIndex);
	

	bool ReplaceSpecialField(char* szDest, char* szSrc);

	int	FindActivateChannelResourceIndex(char* cTitle);
	int	FindChannelResourceIndex(char* cTitle);
	int	FindActivateChannelIndexByKey(char* cKey);
	BYTE FindActivateChannelCost(DWORD nChannelID);
	void SendQueryChannelID(int nChannelResourceIndex);
	void SendChannelSubscribe(int nChannelIndex, bool b);
	int	 AddActivateChannel(const KChannelActivateInfo& Item);	//返回增加Item的Index
	int	 FindActivateChannelIndexById(DWORD nChannelID);
	int	 FindActivateChannelIndex(char* cTitle);
	
	void addchatmsg(char * typeName,char * sendName,char * contentt,UIListView*listView,int nKind,int nColor=0,unsigned int nItemDwidx=0,char*scrSendName=NULL,int nPackage=0);

	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool __isCanMove;
};

#endif /* defined(__MyGame__PopLayer__) */