//
//  PopupLayer.h
//  PopupDemo
//
//  Created by IDEA-MAC03 on 13-10-10.
//
//
#ifndef __Kuigive__
#define __Kuigive__
#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度
#define MAX_ITEM_COUNT  60
#define MAX_GIVE_COUNT  24
#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include <map>
USING_NS_CC;
USING_NS_CC_EXT;

//#define FIGHT_SKILL_COUNT_PER_PAGE	25
//#define ITEM_CELL_SIZE	28   //每个格子的高度和宽度

class Kuigive:public CCLayer{
public: 
	Kuigive();
	~Kuigive();
	//初始化
	virtual bool init();
	//创建
	CREATE_FUNC(Kuigive);

	static Kuigive * create(char * strTitel,char *pContent,int nNum,char* pszCallback,char* pszError,CCObject * callbackListener,SEL_CallFuncN callfun);
	//char* pszTitle, char* pszInitString,const int nNum,const char* pszCallback,const char* pszError

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化
	void	UpdateGiveItem(KUiObjAtRegion* pItem, int bAdd);
	virtual void update(float delta);
	virtual void draw();

	bool    isOpen;
private:
	//增加技能图标
	void addDialogData(char * strTitel,char *pContent);
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);

	//重写触摸注册函数，重新给定触摸级别
	void registerWithTouchDispatcher();

	//触摸函数ccTouchBegan，返回true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);

	void  closeItemPadCallback(CCNode *pNode);
	//关闭弹出框
	void closePopLayer(CCObject * pSender);

	//执行上层对象的回调函数，关闭弹出框
	void buttonCallBackFunc(CCObject * pSender);
	void btnCallBackFunc_fangzhi(CCObject * pSender);
	void btnCallBackFunc_quxia(CCObject * pSender);
	void okCallBackFunc(CCObject * pSender);
	void zhengCallBackFunc(CCObject * pSender);

	//上层对象
	CCObject * m_callbackListener;

	//回调函数
	SEL_CallFuncN m_callfun;

	int  FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);
	typedef std::map<std::string,KUiItemdObject> _ItemData;                 //SkillId,Echance
	_ItemData m_ItemData;

	typedef std::map<std::string,KUiItemdObject> _giveItemData;                 //SkillId,Echance
	_giveItemData m_giveItemData;

	bool  m_nIstrue;
	int AddObject(KUiDraggedObject* pObject, int nCount);
	int AddGiveObject(KUiDraggedObject* pObject, int nindex);
	void  UpdateItemData();
	void  UpdateGiveItemData();
	//对话框背景大小
	CCPoint m_origin;
	CCSize  m_size;
	CCSize  winSize;
	CCSize  itemBoxSize;
	CCSize  giveBoxSize;
	//对话框背景精灵
	CCSprite * m_bgSprite;
	CCSprite *ParentNode_ItemEx;

	CCNode *ParentNode_Item;
	CCNode *ParentNode_Give;
	//KUiSkillData Skills[FIGHT_SKILL_COUNT];
	CCRect getRectItem(CCNode* pNode,int i);
	CCRect getGiveItem(CCNode* pNode,int i);
	CCLabelTTF* pMoneyLabel;
	CCLabelTTF* pXuLabel;
	CCLayerColor * colorLayer;
	//CCAction* red;
	CCSprite *btnBackPic;
	POINT m_StartPos; //格子开始的偏移 相对偏移 左上角(0,0)
	POINT m_StartPos_g; //格子开始的偏移 相对偏移 左上角(0,0)
	int  m_nItemNumObjects;
	int  m_ngiveItemNumObjects;
	std::string nstrCurSelItemKey;
	std::string inCurGiveItemKey;
	bool m_descPadIsOpen;
	int m_itemNum,m_giveNum,m_nNeed;
	char   m_Callback[64];
	char   m_Error[16];
	unsigned int nBakChickTime;
	int  m_nMoney;
	int  m_nXu;
};

#endif /* defined(__MyGame__PopLayer__) */