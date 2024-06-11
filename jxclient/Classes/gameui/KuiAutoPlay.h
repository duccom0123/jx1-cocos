//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiAutoPlay__
#define __KuiAutoPlay__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "engine/KIniFile.h"
#include "engine/KTabFile.h"
#include "gameui/Uiglobaldata.h"
//#include "network/Thread.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define MAX_SKILL_COUNT	4        //每个格子的高度和宽度
#define MAX_ITEMLIST_COUNT	16   //每个格子的高度和宽度
/*enum BTN_PAGE_AUTO
{
	AUTOBTN_PAGE_BASE =1,
	AUTOBTN_PAGE_ART,
	AUTOBTN_PAGE_ITEM_1,
	AUTOBTN_PAGE_ITEM_2,
	AUTOBTN_PAGE_OTHER,
};

enum LIST_VIEW_TAG
{
	LIST_VIEW_ITEM =1,
	LIST_VIEW_TOOL,
	LIST_VIEW_USE,
	LIST_VIEW_OTHER,
};

enum CHECK_BOX_TAG
{
	BOX_TAG_BASE =1,
	BOX_TAG_ART,
	BOX_TAG_ITEM_1,
	BOX_TAG_ITEM_2,
	BOX_TAG_SHIQU,
	BOX_TAG_SAVE_M,
	BOX_TAG_TEAM_1,
	BOX_TAG_TEAM_2,
	BOX_TAG_TEAM_3,
	BOX_TAG_FENJIE,
	BOX_TAG_YANSHI,
	BOX_TAG_YABIAO,
	BOX_TAG_BAOLIU,
	BOX_TAG_USEITEM,
	BOX_TAG_DING,
	BOX_TAG_LUXIAN,
	BOX_TAG_LUZHI,
	BOX_TAG_GENSHUI,
	BOX_TAG_MIAOSHA,
	BOX_TAG_SOUND,
	BOX_TAG_JULI,
	BOX_TAG_OTHER,
};
*/
class KuiAutoPlay:public CCLayer//,protected CThread
{
public: 
	KuiAutoPlay();
	~KuiAutoPlay();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(KuiAutoPlay);
	//using CThread::Start;
	static KuiAutoPlay * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	virtual void update(float delta); 
	virtual void draw();
	void  _removeDelegate(bool isThis);
	bool    isOpen;
	//virtual void Run();
private:
	//增加技能图标
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	void selectedBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	void checkBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	void selectedItemEvent(CCObject* pSender, ListViewEventType type);
	void valueChanged(CCObject* sender, CCControlEvent controlEvent);
	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();
	std::string creatAutoPlayDir(std::string nTempPath);
	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void touchPageBtnEvent(CCObject *pSender, TouchEventType type);
	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	//上层对象
	CCObject * m_callbackListener;

	int CreateFolder(char * nPath);
	//回调函数
	SEL_CallFuncN m_callfun;

	void setBtnTouchEnble(int index,bool enble);
	void clearTempSelindex();
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize winSize;
	//对话框背景精灵
	//CCSprite * m_bgSprite;
	CCSprite *ParentNode_Team;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRect(CCNode* pNode,int i);
	void base_Breathe();
	void item_Breathe();
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXuLabel;
	//CCLayerColor * colorLayer;
	CCAction* red;

	//int  m_nMoney;
	//int  m_nXu;
	//POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	UILayer *m_pUiLayer;
	Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* listView_Item;
	UIListView* listView_tool;
	UIListView* listView_use;
	CCSize btnSize;
	UIButton* btnPageBox_1;
	UIButton* btnPageBox_2;
	UIButton* btnPageBox_3;
	UIButton* btnPageBox_4;
	UIButton* btnPageBox_5;

	//UILayer * _PageLayer_1;
	CCNode  * _PageLayer_1;
	UILayer * _PageLayer_2;
	UILayer * _PageLayer_3;
	UILayer * _PageLayer_4;
	UILayer * _PageLayer_5;
	std::string clientSaveDir;

	UICheckBox* checkBox_baohu;
	UICheckBox* checkBox_zhandou;
	UICheckBox* checkBox_shiqu;
	UICheckBox* checkBox_savem;

	CCControlSwitch *switchControl_baohu;
	CCControlSwitch *switchControl_team;
	CCControlSwitch *switchControl_team_g;
	CCControlSwitch *switchControl_team_d;
	CCControlSwitch *switchControl_team_y;

	CCControlSwitch *switchControl_zhandou;
	CCControlSwitch *switchControl_shiqu;
	CCControlSwitch *switchControl_savem;
	CCControlSwitch *switchControl_fenjie;
	CCControlSwitch *switchControl_yanshi;
    CCControlSwitch *switchControl_skill[MAX_SKILL_COUNT];
	CCLabelTTF      *skillnameLabel[MAX_SKILL_COUNT];

	CCControlSwitch *switchControl_item_m;
	CCControlSwitch *switchControl_item_d;
	CCControlSwitch *switchControl_item_z;
	CCControlSwitch *switchControl_item_zm;
	CCControlSwitch *switchControl_item_t;

	CCControlSwitch *switchControl_yabiao;
	CCControlSwitch *switchControl_baoliu;
	CCControlSwitch *switchControl_useitem;

	CCControlSwitch *switchControl_dingdian;
	CCControlSwitch *switchControl_luxian;
	CCControlSwitch *switchControl_luzhi;
	CCControlSwitch *switchControl_miaosha;
	CCControlSwitch *switchControl_other_s;
	CCControlSwitch *switchControl_other_j;
	CCControlSwitch *switchControl_other_y;
	CCControlSwitch *switchControl_other_t;
	CCEditBox *pLifeEditBox;
	CCEditBox *pLifeEditBox_h;
	CCEditBox *pNeiliEditBox;
	CCEditBox *pshuxEditBox;
	CCEditBox *pEditBox_yanshi;

	CCEditBox *pXunguaiEditBox;
	CCEditBox *pGongjiEditBox;
	CCEditBox *pGongjiEditBox_m;
	CCEditBox *pmoneyEditBox;
	CCEditBox *pshiquEditBox_c;

	CCEditBox *pstrEditBox_s;
	CCEditBox *pstrEditBox_sz;
	CCEditBox *pstrEditBox_d;
	CCEditBox *pstrEditBox_u;

	char nMapName[64];
	int  nMapidx;
	int  nPosX;
	int  nPosY;
	char nMapKind[32];
	KIniFile nAutoNeiGua;
	KTabFile nAutoZhuang;
	KTabFile nAutoDaiju;
	KTabFile nAutoUseItem;
	KTabFile nAutoSkill;
	char nFilePath[128];
	int nFactionNo;
	UIButton* custom_button_item[MAX_ITEMLIST_COUNT];
	UIButton* custom_button_tool[MAX_ITEMLIST_COUNT];
	UIButton* custom_button_use[MAX_ITEMLIST_COUNT];

	UIButton* _button_1;
	UIButton* _button_2;
	UIButton* _button;
	int   nTempSelItemIndex;
	int   nTempSeltoolIndex;
	int   nTempSeluseIndex;

	unsigned int m_uLifeTime;
	unsigned int m_uManaTime,m_PortalTime;
	unsigned int m_uMoneyTime;
	unsigned int nUseItemLoopTime;
	unsigned int g_nServiceLoop;
	
};

#endif /* defined(__MyGame__PopLayer__) */