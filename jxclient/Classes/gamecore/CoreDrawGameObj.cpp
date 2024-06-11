#include "KCore.h"
#include "KNpc.h"
#include "KMissle.h"
#include "KItem.h"
#include "KBuySell.h"
#include "KPlayer.h"
//#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KSubWorldSet.h"
#include "gamescene/KScenePlaceC.h"
#include "ImgRef.h"
#include "GameDataDef.h"
#include "KObjSet.h"
//=========================
#include "engine/Text.h"
#include "CoreShell.h"
#include "CoreDrawGameObj.h"
//=========================
#define  PHYSICSSKILLICON "\\spr\\Ui\\技能图标\\icon_sk_ty_ap.spr"
#define  ADDSKILLPIONT    "\\spr\\Ui4\\common\\加点按钮.spr"
#define  SHOW_SPACE_HEIGHT 5
//=========================

BOOL CoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam,int inParam)
{
	return true;

	switch(uObjGenre)
	{
	case CGOG_NPC:
		if (uId > 0)
		{
			int nHeight = Npc[uId].GetNpcPate();
			
			if  (!Npc[uId].IsPlayer() && Npc[uId].m_Kind!=kind_dialoger)
				Npc[uId].PaintBlood(nHeight/2);	  //绘制冒血

			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{
				int nnHeight = nHeight;
				    nHeight = Npc[uId].PaintChat(nHeight);	//头顶聊天信息中有名字
				
			    if (nHeight == nnHeight)	//没有聊天信息时绘制人物信息
				{
					    //if (nnHeight != nHeight)	//有内力显示时
						//{
						//	if(Npc[uId].m_CurrentCamp != camp_event)   //对话者不显示内力条
						//		nHeight = Npc[uId].PaintMana(nHeight+3);
						//nHeight = nnHeight+3;//好看
						//} 
					if (NpcSet.CheckShowLife())
					{
						if (Npc[uId].m_Hide.nTime <= 0 || Player[CLIENT_PLAYER_INDEX].m_nIndex == uId)
						{ 
							if(Npc[uId].m_CurrentCamp != camp_event)                     //路人对话NPC不显示血条
								nHeight = Npc[uId].PaintLife(nnHeight, true);				  
						}
					}

					if (NpcSet.CheckShowName())
					{
						if (Npc[uId].m_Hide.nTime <= 0 || Player[CLIENT_PLAYER_INDEX].m_nIndex == uId)
						{//本人
						    if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc() && Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == uId)
							{  
							    nHeight = Npc[uId].PaintInfo(nHeight,false,14,0XFF000000);	//被选中的人名放大显示	    
							} 
					     	else
							{ 
							    nHeight = Npc[uId].PaintInfo(nHeight, true);
						        nHeight += 0;
							} 
						}
					}
				}
			}
			else if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
			{
				if (Npc[uId].m_Hide.nTime <= 0 ||  Player[CLIENT_PLAYER_INDEX].m_nIndex == uId)
				{ 
					 Npc[uId].Paint();  //绘画 NPC 外观

				     if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
					 { 
					     Npc[uId].DrawBorder();//绘画 NPC 颜色边框外观	 3D加亮
					 } 														 
				}
			}			
		}
		break;
	case CGOG_MISSLE:
	if (uId > 0 && uId < MAX_MISSLE)
	{
	    if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsHideMissle)
		{//绘画 玫瑰花 崩溃(已经修复)
        	Missle[uId].Paint();  //绘制子弹图
		}
	}
	break;
	
	case CGOG_IME_ITEM:        //快捷栏物品图标--技能--叠加数量
		if (uId == 0)
			break;
		if (Width == 0 && Height == 0)
		{
        #define	ITEM_CELL_WIDTH		26
        #define	ITEM_CELL_HEIGHT	26
			Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
			Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
			x -= Width / 2;
			y -= Height / 2;
		}
		else
		{
			x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
			y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
		}

		if (Item[uId].GetGenre()==1)  //药品  就显示包袱里面的总数
		{
			int szStackNum=0;
			    Item[uId].GetDetailType();
			    Item[uId].GetParticular();
                szStackNum = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameEquipmentItemNum(uId);
			    Item[uId].Paint(x, y,TRUE,szStackNum);
		}
        else
		   Item[uId].Paint(x, y);  //描绘可叠放数量,FALSE
/*
#ifndef _SERVER
		//-------------------------------------------------------------------------------
		//	功能：获取包袱的同种物品的 叠加数量
		//-------------------------------------------------------------------------------
		int		KItemList::GetSameEquipmentItemNum(int nIdx)
		{
			//if (nImmediatePos < 0 || nImmediatePos >= EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
			//	return 0;
			//int		nIdx = m_Room[room_equipment].FindItem(nImmediatePos, 0);
			if (nIdx <= 0)
				return 0;
			return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType()) + Item[nIdx].GetStackNum();
		}
#endif
*/
		break;
  case CGOG_COM_ITEM:   //打造栏物品
		if (uId == 0)
			break;
		if (Width == 0 && Height == 0)
		{
#define	ITEM_CELL_WIDTH		26
#define	ITEM_CELL_HEIGHT	26
			Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
			Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
			x -= Width / 2;
			y -= Height / 2;
		}
		else
		{
			x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
			y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
		}
		Item[uId].Paint(x, y);  //不描绘可叠放数量,FALSE
		break;
	case CGOG_ITEM_SET:     //物品图标显示
		if (uId == 0)
			break;
		Item[uId].Paint(x, y,nParam);    //描绘可叠放数量
		break;
	case CGOG_CHESS_ITEM:   //象棋图标显示
		if (uId == 0)
			break;
		//	Item[uId].Paint(x, y,nParam);    //描绘可叠放数量
		break;
	case CGOG_PLAYERSELLITEM:  //绘画摆摊物品图标
	case CGOG_ITEM:            //物品图标显示
		if (uId == 0)
			break;

		if (Width == 0 && Height == 0)
		{
        #define	ITEM_CELL_WIDTH		26
        #define	ITEM_CELL_HEIGHT	26
			Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
			Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
			x -= Width / 2;
			y -= Height / 2;
		}
		else
		{
			x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
			y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
		}
		Item[uId].Paint(x, y);    //描绘可叠放数量
		break;
	case CGOG_MENU_NPC:  //菜单 绘画 NPC外观
		if (nParam)
		{
			((KNpcRes *)nParam)->SetPos(0, x + Width / 2, y + Height / 2 + 28, 0, FALSE, TRUE);  //设定位置
             #define	STAND_TOTAL_FRAME	15
			int nFrame = g_SubWorldSet.m_nLoopRate % STAND_TOTAL_FRAME;   //服务器当前的时间
			((KNpcRes *)nParam)->Draw(0, 0, STAND_TOTAL_FRAME, nFrame, true);  // 2D 绘画外观
		}
		break;
	
	case CGOG_NPC_BLUR_DETAIL(1):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(1); // 绘画残影
		break;
	case CGOG_NPC_BLUR_DETAIL(2):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(2);
		break;
	case CGOG_NPC_BLUR_DETAIL(3):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(3);
		break;
	case CGOG_NPC_BLUR_DETAIL(4):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(4);
		break;
	case CGOG_NPC_BLUR_DETAIL(5):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(5);
		break;
	case CGOG_NPC_BLUR_DETAIL(6):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(6);
		break;
	case CGOG_NPC_BLUR_DETAIL(7):
		//Npc[uId].GetNpcRes()->m_cNpcBlur.Draw(7);
		break;
	case CGOG_SKILL_SHIQU:
	case CGOG_SKILL:
	case CGOG_SKILL_FIGHT:
		{
			int nSkillId = (int)uId;
			if (nSkillId > 0)
			{
				ISkill *pSkill = g_SkillManager.GetSkill(nSkillId,1);
				if (pSkill)
				{
					pSkill->DrawSkillIcon(x, y, Width, Height,inParam);  //绘制技能图标
				}
			}
			else
			{
				if(uId == -1) 
				{
					KRUImage RUIconImage;
					RUIconImage.nType = ISI_T_SPR;
					RUIconImage.Color.Color_b.a = 255;
					RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					RUIconImage.uImage = 0;
					RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
					RUIconImage.bRenderFlag = 0;
					strcpy(RUIconImage.szImage, PHYSICSSKILLICON); //通用图标
					RUIconImage.oPosition.nX = x;
					RUIconImage.oPosition.nY = y;
					RUIconImage.oPosition.nZ = 0;
					RUIconImage.nFrame = 0;
					//g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 1);
				}
			}
		}
		break;
	case CGOG_SKILL_LIVE:
	case CGOG_SKILL_SHORTCUT:  // 快捷技能描绘
		{
		int nSkillId = (int)uId;
		if (nSkillId > 0)
		{
			ISkill *pSkill = g_SkillManager.GetSkill(nSkillId,1);
			if (pSkill)
            {
				pSkill->DrawSkillIcon(x, y, Width, Height);  //绘制技能图标
            }
		}
		else
		{
			if(uId == -1) 
			{
				KRUImage RUIconImage;
				RUIconImage.nType = ISI_T_SPR;
				RUIconImage.Color.Color_b.a = 255;
				RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				RUIconImage.uImage = 0;
				RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
				RUIconImage.bRenderFlag = 0;
				strcpy(RUIconImage.szImage, PHYSICSSKILLICON); //通用图标
				RUIconImage.oPosition.nX = x;
				RUIconImage.oPosition.nY = y;
				RUIconImage.oPosition.nZ = 0;
				RUIconImage.nFrame = 0;
				//g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 1);
			}
		}
		}
		break;
	case CGOG_OBJECT:   //地上物品
		if (uId)
		{
			if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			//if ((nParam & IPOT_RL_COVER_GROUND) == IPOT_RL_COVER_GROUND)
			//if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)
			{
//				if (ObjSet.CheckShowName())
					Object[uId].DrawInfo();	  //绘画名字
				//	Object[uId].DrawBorder();
			}
			else
		    //if ((nParam & IPOT_RL_COVER_GROUND) == IPOT_RL_COVER_GROUND)
			//if ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)
			{  
				    Object[uId].Draw();
				if ((int)uId == Player[CLIENT_PLAYER_INDEX].GetTargetObj())
					Object[uId].DrawBorder();
			}   
				    
/*//////////////////////////////显示可叠数量////////////////////////////
if (Width == 0 && Height == 0)
{
#define	ITEM_CELL_WIDTH		26
#define	ITEM_CELL_HEIGHT	26
Width = Item[uId].GetWidth() * ITEM_CELL_WIDTH;
Height = Item[uId].GetHeight() * ITEM_CELL_HEIGHT;
x -= Width / 2;
y -= Height / 2;
}
else
{
x += (Width - Item[uId].GetWidth() * ITEM_CELL_WIDTH) / 2;
y += (Height - Item[uId].GetHeight() * ITEM_CELL_HEIGHT) / 2;
}

  Item[uId].Paint(x, y); 
	
//////////////////////////////////////////////////////////////////////*/
		}
		break;
	case CGOG_NPCSELLITEM://绘画商城　商店的东西
		if (uId < 0)
			break;
		BuySell.PaintItem(uId, x, y);
		break;
	case CGOG_NPCSELLITEM_PAI://绘画拍卖行的东西
		if (uId < 0)
			break;
		BuySell.PaintItemPai(uId, x, y,CGOG_NPCSELLITEM_PAI);
		break;
	default:
		break;
	}

	return TRUE;
}

void	CoreGetGameObjLightInfo(unsigned int uObjGenre, unsigned int uId, KLightInfo *pLightInfo)
{
	switch (uObjGenre)
	{
	case CGOG_NPC:   //NPC颜色加亮
		if (uId > 0 && uId < MAX_NPC)
		{
			int uMap=0;
			Npc[uId].GetMpsPos(&pLightInfo->oPosition.nX, &pLightInfo->oPosition.nY,&uMap);
			pLightInfo->oPosition.nZ = Npc[uId].m_Height;
			pLightInfo->dwColor = 0;
			if (Npc[uId].m_RedLum > 255)
			{
				Npc[uId].m_RedLum = 255;
			}
			if (Npc[uId].m_GreenLum > 255)
			{
				Npc[uId].m_GreenLum = 255;
			}
			if (Npc[uId].m_BlueLum > 255)
			{
				Npc[uId].m_BlueLum = 255;
			}
			pLightInfo->dwColor = 0xff000000 | Npc[uId].m_RedLum << 16 | Npc[uId].m_GreenLum << 8 | Npc[uId].m_BlueLum;
			pLightInfo->nRadius = Npc[uId].m_CurrentVisionRadius;
		}
		break;
	case CGOG_OBJECT:
		break;
	case CGOG_MISSLE:
		if (uId > 0 && uId < MAX_MISSLE)
		{
			if (Missle[uId].m_nMissleId > 0)
			{
				Missle[uId].GetLightInfo(pLightInfo);
			}
		} 
		break;
	default:
		break;
	}
}
