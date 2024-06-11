#include "gamecore/KCore.h"
#include "gamecore/KObjSet.h"
#include "gamecore/KNpcSet.h"
#include "gamecore/KSubWorld.h"
#include "gamecore/KNpc.h"
#include "engine/KIniFile.h"
#include "KDrawGameWorld.h"
#include "gamecore/KMissleSet.h"
#include "gamecore/KPlayerSet.h"
#include "gamecore/KPlayer.h"
#include "gameui/Uiglobaldata.h"
///////////////////////////////////
KDrawGameWorld g_DrawGameWorld;

KDrawGameWorld::KDrawGameWorld()
{
	/*ParentNode_ui =NULL;
	ParentNode_map=NULL;
	ParentNode_npc=NULL;
	ParentNode_player=NULL;
	ParentNode_miss=NULL;
	ParentNode_item=NULL;*/
}

KDrawGameWorld::~KDrawGameWorld()
{
	/*ParentNode_ui =NULL;
	ParentNode_map=NULL;
	ParentNode_npc=NULL;
	ParentNode_player=NULL;
	ParentNode_miss=NULL;
	ParentNode_item=NULL;*/

#ifdef WIN32
	Terminate();
#else
	Terminate(1);
#endif
}

void KDrawGameWorld::ClearUp()
{
#ifdef WIN32
	Terminate();
#else
	Terminate(1);
#endif
}

void KDrawGameWorld::StartUp()
{//开启一个线程循环游戏世界
	Start();
}
//绘画线程
void KDrawGameWorld::Run()
{
	//CCMessageBox("绘画线程启动","绘画线程启动");
	/*unsigned long g_nServiceLoop=0;
	while(true)
	{
		if(g_GameWorld && m_bIsPlayerInGame) 
		{//如果 有子节点
		  if (ParentNode_npc && ParentNode_npc->getChildrenCount())
		  {
			CCObject *temp;
			CCArray *nodeArray_Npc = ParentNode_npc->getChildren();//获取父节点下的 所有子节点的数组
			CCARRAY_FOREACH(nodeArray_Npc,temp)
			{
				if(temp)
				{
					int nNpcIndex = ((CCNode*)temp)->getTag();
					//char msg[64]={0};
					//t_sprintf(msg,"NPC:%d",nNpcIndex);
					//CCMessageBox(msg,"KDrawGameWorld");
					//if  (nNpcIndex>0)
						//Npc[nNpcIndex].Paint();
				}
			}
		  }//end ParentNode_npc

		}

#ifdef WIN32
		if (++g_nServiceLoop & 0x80000000)
		{
			g_nServiceLoop = 0;
		}
		if (g_nServiceLoop & 0x1)
			Sleep(10);       //挂起1毫秒
#else
		usleep(100000); //微秒 毫秒=1000微妙  sleep 为秒
		//sleep(1);
#endif
	}//end while
	*/
}

void  KDrawGameWorld::SetGameWorlNode(CCNode * ngameNode,int nKind)
{

	switch(nKind)
	{
	case OBJ_NODE_UI:
		ParentNode_ui=ngameNode;
		break;
	case OBJ_NODE_MAP:
		ParentNode_map=ngameNode;
		break;
	case OBJ_NODE_NPC:
		{//NPC 的各个部位的信息
			ParentNode_npc=ngameNode;
		}
		break;
	case OBJ_NODE_MISS:
		{
			ParentNode_miss=ngameNode;
		}
		break;
	case OBJ_NODE_ITEM:
		{
			ParentNode_item=ngameNode;
		}
		break;
	default:
		break;
	}
}

void KDrawGameWorld::DrawPrimitives(int nNpcIndex,int nPos, KRUImage* pPrimitives, unsigned int uGenre, int bpartNo,int nOrDer)
{//只是人物的绘画
	/*char msg[256]={0};//线程内不能运行这个显示
	t_sprintf(msg,"index:%d,nPos:%d,uGenre:%d,partNo:%d\n Path:%s",nNpcIndex,nPos,uGenre,bpartNo,pPrimitives[nPos].szImage);
	//CCMessageBox(msg,"DrawPrimitives");
	DrawInfoLabel->setString(msg);*/
	//return;

	if  (!ParentNode_npc || nNpcIndex<0)
		return;

	if  (pPrimitives[nPos].szImage[0] && nNpcIndex>0 && Npc[nNpcIndex].m_Kind==kind_player)
	{   
		char nSprName[128]={0};
		ZeroMemory(nSprName,sizeof(nSprName));
		t_sprintf(nSprName,pPrimitives[nPos].szImage);
		g_StrLower(nSprName);
		int m_nWidth,m_nHeight,nFrams;
		CCTexture2D *bgCur = NULL;
		CCRect CCRectZero = CCRectMake(0,0,0,0);
		char nSprFilePath[64]={0};
		ZeroMemory(nSprFilePath,sizeof(nSprFilePath));
		DWORD nFielpahtdwid = g_FileName2Id(nSprName);
		t_sprintf(nSprFilePath,"%u-%d",nFielpahtdwid,pPrimitives[nPos].nFrame);

		switch(uGenre)
		{
		case EQ_PART_HEAD:
			{//头部
				switch(bpartNo)
				{
				case 0:
					{//头盔
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_HEAD));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							//nPartSpr->initWithTexture(bgCur);
							nPartSpr->setZOrder(nOrDer);
							nPartSpr->setPosition(ccp(1,33));
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case EQ_PART_BODY:
			{//默认位置在节点的中间
				switch(bpartNo)
				{
				case 5:
					{//躯体
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
						     bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nQutiSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_BOBY));
							nQutiSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nQutiSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							//nPartSpr->initWithTexture(bgCur);
							nQutiSpr->setZOrder(nOrDer);
							//nPartSpr->setScaleX(0.5);
							//nPartSpr->setScaleY(0.5);
						}
					}
					break;
				case 6:
					{//左手
						//CCMessageBox(nSprName,"DrawPrimitives5");
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_LEFTHD));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
							//nPartSpr->setPosition(ccp(1,33));
						}

						
					}
					break;
				case 7:
					{//右手
						//CCMessageBox(nSprName,"DrawPrimitives6");
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_RIGTHHD));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
							//nPartSpr->setPosition(ccp(1,33));
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case EQ_PART_HAND_W: //左右武器
			{//默认位置在节点的中间
				switch(bpartNo)
				{
				case 8:
					{//左手武器
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_LEFTHD_W));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
							//nPartSpr->setPosition(ccp(1,33));
						}
					}
					break;
				case 9:
					{//右手武器
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_RIGTHHD_W));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
							//nPartSpr->setPosition(ccp(1,33));
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case EQ_PART_HORSE:
			{
				switch(bpartNo)
				{
				case 10:
					{
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_HORSE_Q));
							//bgCur->getContentSize();
							if (Npc[nNpcIndex].m_HorseType<0)
								nPartSpr->setVisible(false);//设置不可见
							else
								nPartSpr->setVisible(true); //设置可见

							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);

						}
					}
					break;
				case 11:
					{
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_HORSE_Z));
							//bgCur->getContentSize();
							if (Npc[nNpcIndex].m_HorseType<0)
								nPartSpr->setVisible(false);//设置不可见
							else
								nPartSpr->setVisible(true); //设置可见

							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
						}
					}
					break;
				case 12:
					{
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_HORSE_H));
							//bgCur->getContentSize();
							if (Npc[nNpcIndex].m_HorseType<0)
								nPartSpr->setVisible(false);//设置不可见
							else
								nPartSpr->setVisible(true); //设置可见

							nPartSpr->setTexture(bgCur);
							
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
							
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case EQ_PART_PIFENG:
			{//披风
				switch(bpartNo)
				{
				case 16:
					{
						if ((bgCur = CCTextureCache::sharedTextureCache()->isFileExist(nSprFilePath))==NULL)
							bgCur = _getinidata.getinidata_one(nSprName,pPrimitives[nPos].nFrame,&m_nWidth,&m_nHeight,&nFrams);
						if (bgCur)
						{//修改精灵的纹理
							CCSprite * nPartSpr = (CCSprite *)(ParentNode_npc->getChildByTag(nNpcIndex)->getChildByTag(UI_EQ_PART_PIFENG));
							//bgCur->getContentSize();
							nPartSpr->setTexture(bgCur);
							CCRectZero.size = bgCur->getContentSize();
							nPartSpr->setTextureRect(CCRectZero,false,CCRectZero.size);
							nPartSpr->setZOrder(nOrDer);
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
	//开始统一调整位置
}
/*#ifndef _SERVER
void KDrawGameWorld::Paint()
{
	SubWorld[0].Paint();
}
#endif*/
