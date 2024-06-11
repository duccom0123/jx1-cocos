//
//  KuiTaskInfo.h
//  KuiTaskInfo
//
//  Created by �ᵶ����Ц QQ:25557432.
//
//
#ifndef __KuiTaskInfo__
#define __KuiTaskInfo__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Engine/LinkStruct.h"
#include <map>
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

#define MAX_TASK_MESSAGE_LENGTH 128
#define MAX_TASK_MESSAGE_MUM 512

struct KTaskIndexNode// : public KListIndex
{
	char szMsg[MAX_TASK_MESSAGE_LENGTH]; //����
	int  nMsgLen;		   //����
	char szUnit[16];	   //��λ
	int nTaskId;
	int nType;
	int nClass;
	int nTrace;
	int nIndex;
	int nCount;
	KTaskIndexNode *next;
	KTaskIndexNode *previous;
};

struct	nNewTaskFindPath
{
	int nTsakType;	    //����
	int	nMsgidx;		//������Ǯ����ֵΪ���۸񣬸�ֵ��ʾ����ļ۸�Ϊ(-nPrice)
	int	nMapIdx;
	int nXpos;
	int nYpos;
};

class KuiTaskInfo:public CCLayer{
public: 
	KuiTaskInfo();
	~KuiTaskInfo();
	//��ʼ��
	virtual bool init();
	//����
	CREATE_FUNC(KuiTaskInfo);

	static KuiTaskInfo * create(char * title,CCObject * callbackListener,SEL_CallFuncN callfun);
	
	virtual void update(float delta); 
	virtual void draw();

	bool    isOpen;
	void    setCanMove(bool ver){__isCanMove =ver;};
	void    addmsgInfo(char *nMsgInfo,int nIsBtn,int nMsgIndex);
	void    addmsgBtnInfo(KTaskPathInfo* pMsg);
private:
	//���Ӽ���ͼ��
	void addDialogData();
	void setcoloseButton(CCObject * callbackListener,SEL_CallFuncN callfun);
	//��д����ע�ắ�������¸�����������
	void registerWithTouchDispatcher();

	void selectedMainListItemEvent(CCObject* pSender, ListViewEventType type);
	void selectedMsgListItemEvent(CCObject* pSender, ListViewEventType type);
	//��������ccTouchBegan������true
	bool ccTouchBegan(CCTouch * touch,CCEvent * pevent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//�رյ�����
	void closePopLayer(CCObject * pSender);
	void oktouchEvent(CCObject *pSender, TouchEventType type);
	//ִ���ϲ����Ļص��������رյ�����
	void buttonCallBackFunc(CCObject * pSender);
	void btnCallBackFunc(CCObject * pSender);
	

	//�ϲ����
	CCObject * m_callbackListener;

	//�ص�����
	SEL_CallFuncN m_callfun;
	//�Ի��򱳾���С
	CCPoint m_origin;
	CCSize  m_size;
	CCSize  winSize;
	//�Ի��򱳾�����
	//CCSprite * m_bgSprite;
	CCSprite *ParentNode_Task;
	//nNewTaskFindPath nFindPath[MAX_TASK_MESSAGE_MUM];
	typedef std::map<int,nNewTaskFindPath> _FindPath;
	_FindPath nFindPath;
	CCRect getRect(CCNode* pNode,int i);
	void removeitems();
	void AddTaskIndex();
	//CCLabelTTF* pMoneyLabel;
	//CCLabelTTF* pXLabel;
	CCLayerColor * colorLayer;
	CCAction* red;
	UILayer *m_pMainUiLayer;
	//Layout  *m_pWidget;  
	//CCArray* m_array;
	UIListView* ptaskMsglistView;
	UIListView* pmainListView;
	//UICheckBox* checkBox;
	CCSize btnSize;
	bool m_bScrolling;
	CCPoint m_lastPoint;
	bool __isCanMove;
	CCSize btnNewSize;
	CCSize btnCurSize;
	CCSize btnMsgSize;
	int  __nSelIndex;
	int  __nCount;
	KIniFile           m_IndexIDIni;
	KLinkStruct<KTaskIndexNode> m_IndexData; //�����������ݵ�����
};

#endif /* defined(__KuiTaskInfo__) */