#include "gamecore/KCore.h"
#include "gamecore/coreshell.h"
#include "gamecore/GameDataDef.h"
#include "gamecore/CoreObjGenreDef.h"
#include "engine/KEngine.h"
#include "gamecore/KTongProtocol.h"
#include "engine/Text.h"

extern iCoreShell*	g_pCoreShell;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UIITEM_WAIT_GETNAME,
};

//--------------------------------------------------------------------------
//	功能：接受游戏世界数据改变通知的函数   ---收到服务器通知打开响应窗口
//--------------------------------------------------------------------------
void CoreDataChangedCallback(unsigned int uDataId, unsigned int uParam, int nParam,int inVal)
{
	//KUiTrade* pTradeBar = NULL;
	if (!g_GameWorld || !nisgetinfo) return;

	switch(uDataId)
	{
	case GDCNI_OPEN_TONG_CREATE_SHEET: //打开建帮UI
		g_GameWorld->OpenCreatTongSheet();
		break;
	case GDCNI_TASK_INFO:           //任务系统
		{
			if (uParam)
			{ 
				if  (nParam==0)
					g_GameWorld->taskCilentMsgArrival((KNewsMessage*)uParam, nParam,inVal);
					//KUiTask::MessageArrival((KNewsMessage*)uParam, nParam);
				else if (nParam==1)
					g_GameWorld->taskCilentFindPathArrival((KTaskPathInfo*)uParam, nParam,inVal);
					//KUiTask::MsgFindPathArrival((KTaskPathInfo*)uParam, nParam);
			}      
		}
		break;
	case GDCNI_NEWS_MESSAGE:	 //飞滚动新闻消息
		if (uParam)
			g_GameWorld->OpenFlyNewsDialog_a((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNI_SHUIJI_MESSAGE:	 //顶部飞滚信息显示
		if (uParam)
			g_GameWorld->OpenFlyNewsDialog_b((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNI_VIEW_LIAN_LITEM:  //查看装备链接
		g_GameWorld->OpenItenLinkDialog((KUiLianItem*)uParam);
		break;

	case GDCNI_GAME_QUIT:
		{
			g_GameWorld->_quitGame();
		}
	case GDCNI_GAME_START:
		{
			//g_GameWorld->ReleaseActivateChannelAll();
			g_GameWorld->QueryAllChannel();
			//UIMessageBox2("提示:登录成功..!");
		}
		break;
		case GDCNI_PLAYER_BASE_INFO:	//主角的一些不易变的数据  收到服务器发来的请求
		{
/*			UiNewStatus* pBar = UiNewStatus::GetIfVisible(); //如果状态栏打开的话
			if (pBar)
				pBar->UpdateBaseData(); //更新基本信息
*/
			KUiPlayerBaseInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);

			/*if (Info.nCurFaction>= 0 ) //加入门派了
			{
				KUiMsgCentrePad::QueryAllChannel();//查询全部频道？
			}
			else if (Info.nCurTong!=0) //已经加入帮派
			{
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Faction); //关闭门派频道
				KUiMsgCentrePad::QueryAllChannel(); //查询全部频道？
			}
			else
			{
				KUiMsgCentrePad::QueryAllChannel(); //查询全部频道？
				//KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Tong); //关闭帮派频道
			} */
			   // KUiMsgCentrePad::SetMissGroupIdx(Info.nMissionGroup);

			if ((Info.nCurFaction >= 0) || 
				(Info.nCurTong != 0) || 
				(Info.nMissionGroup >= 0)
				)
				g_GameWorld->QueryAllChannel();
			
			
			if (Info.nCurFaction < 0)
				//KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Faction);
			
			if (Info.nCurTong == 0)
				//KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Tong);
			
			if (Info.nMissionGroup < 0)
			{
				//KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Msgr);
			}
			
			//if (Info.nRoomId < 0)
			//	KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Cr);
		}
		break;
	case GDCNI_TRADE_OPER_DATA:		//交易状态变化
		//pTradeBar = KUiTrade::GetIfVisible();
		//if (pTradeBar)
		g_GameWorld->UpdateTradeOperData();
		break;
	case GDCNI_TRADE_END:			//交易结束
		//pTradeBar = KUiTrade::GetIfVisible();
		//if (pTradeBar)
		g_GameWorld->UpdateTradeEnd();
		break;
	case GDCNI_TRADE_DESIRE_ITEM:	//对方增减想交易的物品
		{
			g_GameWorld->OnTarChangedItem((KUiObjAtRegion*)uParam, nParam);
		}
		break;
	case GDCNI_TRADE_START:
		if (uParam)
		{
			g_GameWorld->OpenPlayerTradeDialog((KUiPlayerItem*)uParam);
		}
		break;
	case GDCNI_GIVE:  //打开给予界面？
		{
			PLAYER_GIVEUI* pObject = (PLAYER_GIVEUI*)uParam; //   
			if (pObject->m_szTitle==NULL)
				t_sprintf(pObject->m_szTitle,"%s","给予(怀旧剑侠)界面");

			g_GameWorld->openGive(pObject->m_szTitle,pObject->m_szName,nParam,pObject->m_Callback,pObject->m_Error);
		}
		break;	
	case GDCNI_CLOSE_BAITAN:
		{
			if (nParam==0)
			    g_GameWorld->ClosePlayerShop();
		}
		break;
	case GDCNI_VIEW_PLAYERSELLITEM: 
		{//打开摆摊面板
           g_GameWorld->OpenPlayerShopDialog((KUiPlayerItem*)uParam);
		}
		break;	
	case GDCNI_VIEW_PLAYERUPDATEITEM:
		{//更新摆摊面板信息
			if (nParam==0)
		      g_GameWorld->UpPlayerShopData();
		}
		break;
	case GDCNI_VIEW_PLAYERITEM:  //查看玩家装备
		    g_GameWorld->OpenViewStateDialog((KUiPlayerItem*)uParam);
		break;
	case GDCNI_TEAM:
		{
			//KUiTeamManage* pPad = KUiTeamManage::GetIfVisible();  //打开窗口的时候才更新
			g_GameWorld->UpdateDataTeam((KUiPlayerTeam*)uParam); //更新队伍信息
			if (uParam)
				g_GameWorld->QueryAllChannel();
			//else
			//	KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Team);*/
		}
		break;
	case GDCNI_OPEN_DATAU_BOX: //老头任务奖励
		g_GameWorld->openlaotou();
		break;
	case GDCNI_OPEN_JINDUTIAO:
		{
		    g_GameWorld->openProgressbar((KUiJinDuTiaoInfo*)uParam);	
		}
		break;
	case GDCNI_SKILL_CHANGE:			//新掌握了一个技能
		if (uParam)
		{//有可能有错误 调试
			g_GameWorld->UpdateSkill((KUiSkillData*)uParam, nParam);
		}
		break;
	case GDCNI_NPC_TRADE:           //客户端打开NPC商店
		{
			if (nParam)
				g_GameWorld->openNpcshop(uParam);
		}
		break;
	case GDCNI_OPEN_STRING_BOX: //打开字符串输入界面
		{//脚本打开 对话框
			PLAYER_STRINGUI* pStr = (PLAYER_STRINGUI*)uParam;
			if (nParam<=5)
				nParam=512;
			g_GameWorld->openstringBox(pStr->m_szTitle,1,nParam,pStr->m_SCallback,pStr->m_SError,pStr->m_nNum);
			//KUiGetStringInUI::OpenWindow(pStr->m_szTitle,"",UIITEM_WAIT_GETNAME,1,nParam,pStr->m_SCallback,pStr->m_SError,pStr->m_nNum); //打开字符窜输入界面
		}
		break;
	case GDCNI_OPEN_STORE_BOX://打开储物箱
		g_GameWorld->openStorebox();
		break;
	case GDCNI_SYSTEM_MESSAGE: 
		{//发送系统公告消息！
			if (uParam)
				g_GameWorld->AMessageArrival((KSystemMessage*)uParam, (void*)nParam);
		}
		break;
	case GDCNI_SPEAK_WORDS:			//npc说话内容  talk
		//uParam = (KUiInformationParam*) pWordDataList 指向KUiInformationParam数组
		//nParam = pWordDataList包含KUiInformationParam元素的数目
		if (uParam && nParam)
			g_GameWorld->CreatTalkDialog((KUiInformationParam*)uParam, nParam);
			//g_UiInformation2.SpeakWords((KUiInformationParam*)uParam, nParam);
		break;
	case GDCNI_QUESTION_CHOOSE:
		{
			if (nParam)
				g_GameWorld->CreatNpcDialog((KUiQuestionAndAnswer*)uParam, (KUiNpcSpr*)nParam,inVal);//是否定时关闭
			else
				g_GameWorld->CreatNpcDialog((KUiQuestionAndAnswer*)uParam);//是否定时关闭
		}
		break; 
	case GDCNI_OBJECT_CHANGED:
		{//增加删除物品
			if (!g_GameWorld) break;
			if (uParam)
			{
				KUiObjAtContRegion* pObject = (KUiObjAtContRegion*)uParam;
				switch(pObject->eContainer)
				{
				case UOC_ITEM_TAKE_WITH:
					{
						//if (g_GameWorld->GetuiItemIfVisible())//打开的 就更新
						g_GameWorld->UpdateItem((KUiObjAtRegion*)uParam, nParam);
					}
					break;
				case UOC_ITEM_EX:
					{
						//字母袋
						if (g_GameWorld->GetuiItemExIfVisible())//打开的 就更新
							g_GameWorld->UpdateItemEx((KUiObjAtRegion*)uParam, nParam);
					}
					break;
				case UOC_EX_BOX1:           //扩展箱1
					{
                       if (g_GameWorld->GetuistoreIfVisible())//打开的 就更新
						   g_GameWorld->UpdateExBox((KUiObjAtRegion*)uParam, nParam,UOC_EX_BOX1);
					} 
					break;
				case UOC_EX_BOX2:
					{
						if (g_GameWorld->GetuistoreIfVisible())//打开的 就更新
							g_GameWorld->UpdateExBox((KUiObjAtRegion*)uParam, nParam,UOC_EX_BOX2);
					}
					break;
				case UOC_EX_BOX3:
					{
						if (g_GameWorld->GetuistoreIfVisible())//打开的 就更新
							g_GameWorld->UpdateExBox((KUiObjAtRegion*)uParam, nParam,UOC_EX_BOX3);
					}
					break;
				case UOC_STORE_BOX:
					{//储物箱
						if (g_GameWorld->GetuistoreIfVisible())//打开的 就更新
							g_GameWorld->UpdateStoreBox((KUiObjAtRegion*)uParam, nParam);
					}
					break;
				case UOC_IMMEDIA_ITEM:
					{
						if (g_GameWorld)
						{
							if (nParam) //增加
								g_GameWorld->UpdateImmediaItem(pObject->Region.h,pObject->Obj.uGenre,pObject->Obj.uId);
							else//删除
								g_GameWorld->UpdateImmediaItem(pObject->Region.h,CGOG_NOTHING,0);
						}
					}
					break;
				case UOC_ITEM_GIVE:
					{
						g_GameWorld->UpdateGiveItem((KUiObjAtRegion*)uParam, nParam);
					}
					break;
				case UOC_TO_BE_TRADE:
					{//自己的增加图标
						//CCMessageBox("开始增加物品","TEST");
						g_GameWorld->OnSelfChangedItem(pObject, nParam);
					}
				default:
					break;
				}

				/*if (pObject->eContainer == UOC_ITEM_TAKE_WITH)
				{
					pTradeBar = KUiTrade::GetIfVisible();
					if (pTradeBar)
					{//交易栏
						pTradeBar->OnChangedTakewithItem((KUiObjAtRegion*)uParam, nParam);
					}
					else
					{//包袱
						//KUiItem* pItemsBar = KUiItem::GetIfVisible();
						if (g_GameWorld->GetuiItemIfVisible())//包袱是打开的 就更新
							g_GameWorld->UpdateItem((KUiObjAtRegion*)uParam, nParam);
					}
				}
				if (pObject->eContainer == UOC_ITEM_EX)
				{//字母袋
						if (g_GameWorld->GetuiItemExIfVisible())//包袱是打开的 就更新
							g_GameWorld->UpdateItemEx((KUiObjAtRegion*)uParam, nParam);
				}  
				if (pObject->eContainer == UOC_STORE_BOX)
				{
					if (g_GameWorld->GetuistoreIfVisible())//包袱是打开的 就更新
						g_GameWorld->UpdateStoreBox((KUiObjAtRegion*)uParam, nParam);
				}*/
			}
		}
		break;
	default:
		break;
	}
}