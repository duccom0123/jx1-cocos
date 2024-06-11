//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __KuiAutoPlay_vn__
#define __KuiAutoPlay_vn__

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
#define MAX_SKILL_COUNT	4        //ÿ�����ӵĸ߶ȺͿ��
#define MAX_ITEMLIST_COUNT	16   //ÿ�����ӵĸ߶ȺͿ��

class KuiAutoPlay_vn:public CCLayer//,protected CThread
{
public: 
	KuiAutoPlay_vn();
	~KuiAutoPlay_vn();
	//��ʼ��
	virtual bool init();
	//����
	CREATE_FUNC(KuiAutoPlay_vn);
	//using CThread::Start;
	static KuiAutoPlay_vn * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	virtual void update(float delta); 
	virtual void draw();
	void  _removeDelegate(bool isThis);
	bool    isOpen;
	//virtual void Run();
private:
	//���Ӽ���ͼ��
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	void selectedBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	void checkBoxStateEvent(CCObject* pSender, CheckBoxEventType type);
	void selectedItemEvent(CCObject* pSender, ListViewEventType type);
	void valueChanged(CCObject* sender, CCControlEvent controlEvent);
	//��д����ע�ắ�������¸�����������
	void registerWithTouchDispatcher();
	std::string creatAutoPlayDir(std::string nTempPath);
	//��������ccTouchBegan������true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void touchPageBtnEvent(CCObject *pSender, TouchEventType type);
	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//�رյ�����
	void closePopLayer(CCObject * pSender);

	//ִ���ϲ����Ļص��������رյ�����
	void buttonCallBackFunc(CCObject * pSender);
	//�ϲ����
	CCObject * m_callbackListener;

	int CreateFolder(char * nPath);
	//�ص�����
	SEL_CallFuncN m_callfun;

	void setBtnTouchEnble(int index,bool enble);
	void clearTempSelindex();
	//�Ի��򱳾���С
	CCPoint m_origin;
	CCSize  m_size;
	CCSize winSize;
	//�Ի��򱳾�����
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
	//POINT m_StartPos; //���ӿ�ʼ��ƫ�� ���ƫ�� ���Ͻ�(0,0)
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

    UICheckBox* pAuto_TDP_CheckBox_TDPSinhLuc;
    UICheckBox* pAuto_TDP_CheckBox_HetBinhSinhLuc;
    UICheckBox* pAuto_TDP_CheckBox_HetBinhNoiLuc;
    UICheckBox* pAuto_TDP_CheckBox_DoBenTrangBi;
    UICheckBox* checkBox_Auto_MANA;
    UICheckBox* checkBox_Auto_HP;
	UICheckBox* checkBox_baohu;
	UICheckBox* checkBox_zhandou;
	UICheckBox* checkBox_shiqu;
	UICheckBox* checkBox_savem;

	CCControlSwitch *switchControl_Auto_HP;
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

    CCEditBox *pLifeEditBox_2;
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
