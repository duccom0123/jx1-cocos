#ifndef	KDrawGameWorldH
#define	KDrawGameWorldH
#include "network/Thread.h"
#include "engine/KTimer.h"
/*
#ifndef _SERVER
#include "KMapMusic.h"
#else

#include "KSubWorld.h"

#endif
*/

class  KDrawGameWorld: public cocos2d::CCLayer,protected CThread
{
public:
	using CThread::Start;
	void StartUp();
	void ClearUp();
	virtual void  Run();
	void  SetGameWorlNode(CCNode * ngameNode,int nKind);
	void  DrawPrimitives(int nNpcIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer);

	CCNode   *ParentNode_ui;
	CCNode   *ParentNode_map;//
	CCNode   *ParentNode_npc;
	CCNode   *ParentNode_player;
	CCNode   *ParentNode_miss;
	CCNode   *ParentNode_item;
private:
	
public:
	KDrawGameWorld();
	~KDrawGameWorld();
	//BOOL	LoadSerMaps(LPSTR szFileName);
};
extern  KDrawGameWorld g_DrawGameWorld;
#endif
