/*****************************************************************************************
//	外界访问Core接口方法
//	Copyright : Kingsoft 2002
//	Author	:   
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "CoreShell.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTask.h"
#include "KTaskFuns.h"
#include "KItem.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KBuySell.h"
/////////后来加的////////////////
#include "gamecore/KNpcAI.h"
#include "gamescene/KScenePlaceC.h"
#include "gamecore/KViewItem.h"
#include "gamecore/KSellItem.h"
#include "gameui/GameSpaceChangedNotify.h"
#include "engine/Text.h"
#include "engine/KTabFileCtrl.h"
//#endif

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4

IClientCallback* l_pDataChangedNotifyFunc = 0;

class KCoreShell : public iCoreShell,protected CThread
{
public:
	using CThread::Start;
	int  StartUp();
	int  ClearUp();
	int	 GetProtocolSize(BYTE byProtocol);
	void NetMsgCallbackFunc(void* pMsgData);
	void Run();
	int  Breathe();
	unsigned long  GetSubGameTime();
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam,int nIsMianBan=0,char* strVal=NULL);
	void GotoWhere(int x, int y, int mode);
	void setNpcDir(int x, int y);
	void setSand();
	int	 FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind);
	int  FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind);
	int  FindSpecialNPC(char* Name, void* pReturn, int& nKind);
	int  LockSomeoneAction(int nTargetIndex);
	int  LockObjectAction(int nTargetIndex);
	int  LockSomeoneUseSkill(int nTargetIndex, int nSkillID);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam,int inParam=0);
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void SendNewDataToServer(void* pData, int nLength);
	int  GetDataNpcShop(int nSaleId, unsigned int uParam, int nParam);
	int  UseSkill(int x, int y, int nSkillID,int m_Kind);
	void MainAttack(int m_Kind);
	void clientPickItem();
	void AutoChangeItem(int nItemIdx);
	int  TeamOperation(unsigned int uOper, unsigned int uParam, int nParam);
	//与帮会相关的操作, uOper的取值来自 GAME_TONG_OPERATION_INDEX
	int	 TongOperation(unsigned int uOper, unsigned int uParam, int nParam,int vnParam=0,char *strParam=NULL);
	int  GetAutoplayid();//获取挂机状态参数
	void Setpos(int x,int y);
	void SetNpcCurPos();
	void YaBiao();
	void Guaji(int x, int y, int mode);
	int  AutoUseSkill();
	int  GetSkillLiveTime(int nSkillId);
	void __ApplyAddTeam(int nTarIndex);
	int  ClearClientRoom(int nRoomKind);
	void __opensysui(int nKind,int ver);
	void TradeApplyStart(int nIndex);
	bool getTradeState();
	int  GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam);
	void setPadCanMove(int _PadIndex,bool ver);
};

static KCoreShell g_CoreShell;

void g_InitCore();

iCoreShell* CoreGetShell()
{
	g_InitCore();
	return &g_CoreShell;
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
}
//--------------------------------------------------------------------------
//	功能：设置游戏世界数据改变的通知函数
//	参数：fnCoreDataChangedCallback pNotifyFunc --> 通知函数的指针。
//	返回：返回值为非0值表示注册成功，否则表示失败。
//--------------------------------------------------------------------------
int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}


void KCoreShell::setPadCanMove(int _PadIndex,bool ver)
{
	if (g_GameWorld)
		g_GameWorld->setPadCanMove(_PadIndex,ver);
}

int KCoreShell::GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = nSaleId;//Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pSShopRoom)
			return 0;
		BuySell.m_pSShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);

			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pSShopRoom->Clear();
					BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}

				if (strstr(pItem->GetItmeInfo(),".lua"))
				{
					ItemXinXi SynItem;
					ZeroStruct(SynItem);
					//SynItem.nIdx  = nIndex; //索引
					SynItem.nX = nBuyIdx;
					SynItem.nY = i;
					SynItem.nIsRongqi = 1;
					SendClientAddItem(&SynItem);
				}

				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);

			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}


bool KCoreShell::getTradeState()
{
	return Player[CLIENT_PLAYER_INDEX].CheckTrading();
}

void KCoreShell::TradeApplyStart(int nIndex)
{
	if (nIndex >= 0 && nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		if (Npc[nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		   Player[CLIENT_PLAYER_INDEX].TradeApplyStart(nIndex);
		else
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("交易:对方没有开启交易!");
	}
}
void KCoreShell::__opensysui(int nKind,int ver)
{
	SendClientRunScript(nKind,ver,0,0);
}
int  KCoreShell::ClearClientRoom(int nRoomKind)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.ClearRoom(nRoomKind);//room_give
	return TRUE;
}


void KCoreShell::__ApplyAddTeam(int nTarIndex)
{
	if (nTarIndex >= 0 && nTarIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(nTarIndex);
	}
}

int  KCoreShell::GetSkillLiveTime(int nSkillId)
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSkillLeftTime(nSkillId);
}


int KCoreShell::AutoUseSkill()
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

/*	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);*/

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

    if (!Npc[nIndex].m_AutoplayId || Npc[nIndex].m_FightMode==0)  //主人为非战斗模式 
    {
      return 0;
    }
     
	int mcActiveSkillID=0;

	if (Npc[nIndex].IsCanInput()) //人物AI开启的话
	{
		int nLoop=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop++;
		
		if  (nLoop>200000)
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop=0;
		int nCostTimes= 5;//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nCostTime;
		if (nLoop>nCostTimes*18 && nLoop%(nCostTimes*18)==0)
		 for (int i=0;i<4;++i)
		 {
		   int mIsoPen=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nIsOpen;
		   int mSkillId=Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[i].nsKillId;

		   if (mSkillId<=0)
			   continue;

           if (mIsoPen==1)
		   {//如果是开启的 
              int nLevel = Npc[nIndex].m_SkillList.GetLevel(mSkillId);
			  if (nLevel<=0)
				  continue;

			  KSkill * pISkill = (KSkill *) g_SkillManager.GetSkill(mSkillId,nLevel);
		      if (!pISkill) 
                  continue;

		      if (pISkill->IsAura())  
			      continue;

             int nLeftime=Npc[nIndex].GetSkillLeftTime(mSkillId);

			 if (nLeftime>0)
				 continue;

		     if (pISkill->IsTargetSelf())
	         {//如果是对自己有用的 并且 是时间到了的

				if (!Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill)
				{//开始使用技能
//====================客户端检测是否能发技能==============================
					 Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill =TRUE;

					 int nIdx = 0;
		                 nIdx = Npc[nIndex].m_SkillList.FindSame(mSkillId);
                     
					if (nIdx <= 0 || nIdx >= MAX_NPCSKILL)
						continue;
					//----------------------------------------取消  改为服务器脚本后台释放
		            /* Npc[nIndex].SetActiveSkill(nIdx);
			         Npc[nIndex].SendSerCommand(do_skill,mSkillId,-1,nIndex);
				     SendClientCmdSkill(mSkillId,-1,Npc[nIndex].m_dwID);*/
					SendClientRunScript(0,nIdx,mSkillId,Npc[nIndex].m_dwID);
                    //------------------------------------------------------
					Npc[nIndex].m_nPeopleIdx = 0;
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop=0;

					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill =FALSE;
					break;
				}
	         }
		   }
		 }   
	}
	else
	{
		return 0;
	}

	return 1;
}
void KCoreShell::YaBiao()
{ 

	NpcAI.MoveYaBiao(Player[CLIENT_PLAYER_INDEX].m_nIndex);

}


void KCoreShell::Guaji(int x, int y, int mode)
{ 

	if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn)
		return;
	
	NpcAI.FollowCharacter(Player[CLIENT_PLAYER_INDEX].m_nIndex,x,y,mode);

}

void KCoreShell::SetNpcCurPos()
{
	int nDesX,nDesY,nDmap;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nDesX, &nDesY,&nDmap);
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_run, nDesX, nDesY);
	SendClientCmdRun(nDesX, nDesY);
}


void KCoreShell::Setpos(int x,int y)
{
	NpcAI.Setscpos(Player[CLIENT_PLAYER_INDEX].m_nIndex,x,y);
}
//获取挂机状态参数
int KCoreShell::GetAutoplayid()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId;

}
void KCoreShell::AutoChangeItem(int nItemIdx)
{
	Player[CLIENT_PLAYER_INDEX].m_ItemList.AutoChangeItem(nItemIdx);
}

void KCoreShell::clientPickItem()
{
	NpcAI.mainPickItem(Player[CLIENT_PLAYER_INDEX].m_nIndex);
}

void KCoreShell::MainAttack(int m_Kind)
{
	  NpcAI.mainSkillattack(Player[CLIENT_PLAYER_INDEX].m_nIndex,m_Kind);
}
//设置大小
void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.RepresentShellReset();
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}
//与帮会相关的操作, uOper的取值来自 GAME_TONG_OPERATION_INDEX
int	KCoreShell::TongOperation(unsigned int uOper, unsigned int uParam, int nParam,int vnParam,char *strParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GTOI_TONG_ACTION:         //对帮内成员做的动作，或自己与帮会的关系的改变
		//uParam = (KTongOperationParam*) pOperParam 动作时的参数
		//nParam = (KTongMemberItem*) pMember 指出了操作（帮会成员）对象，
		{
			KTongOperationParam *Oper = (KTongOperationParam *)uParam;
			KTongMemberItem *TargetInfo = (KTongMemberItem *)nParam;
			KUiPlayerItem TargetPlayer;
			int nKind;

			switch(Oper->eOper)
			{
			case TONG_ACTION_DISMISS:  //踢人
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyKick(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
				break;
			case TONG_ACTION_ASSIGN:   //任命
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInstate(Oper->nData[0], Oper->nData[1], Oper->nData[2], Oper->nData[3], TargetInfo->Name);
				break;
			case TONG_ACTION_DEMISE:   //传位
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeMaster(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
				break;
			case TONG_ACTION_LEAVE:    //离开帮会
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyLeave();
				break;
			case TONG_ACTION_RECRUIT:  //同意招人
				break;
			case TONG_ACTION_APPLY:    //申请加入帮派
				if(!FindSpecialNPC(TargetInfo->Name, &TargetPlayer, nKind))
				{
					break;
				}
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyAddTong(TargetPlayer.uId);
				break;
			}
		}
		break;

		//对申请加入的答复
		//uPAram = (KUiPlayerItem *) pTarget   申请方
		//nParam : != 0  同意   ==0  拒绝
	case GTOI_TONG_JOIN_REPLY:
		if (uParam && strParam)
		{
			///KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			char nTempChar[32];
			t_sprintf(nTempChar,strParam);
			//t_sprintf(nTempChar,U2G(nTempChar).c_str());
			Player[CLIENT_PLAYER_INDEX].m_cTong.AcceptMember(uParam, g_FileName2Id(nTempChar), nParam);
		}
		break;
	case GTOI_TONG_BZName:  //获取帮主名
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;

			sprintf(pItem->BZname,"%s",Player[CLIENT_PLAYER_INDEX].m_cTong.m_BMasterName);

		}
		break;
	case GTOI_TONG_CREATE:		//创建帮会
		//uParam = (const char*) pszTongName 帮会的名字
		//nParam = (NPCCAMP)enFaction 帮会阵营
		if (strParam)
		   Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyCreateTong(nParam,strParam);
		break;
	default:
		break;
	}
	return nRet;
}
//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case TEAM_OI_APPOINT:		//任命队长，只有队长调用才有效果
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);		
		break;
	case TEAM_OI_KICK:			//踢除队里的一个队员，只有队长调用才有效果
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE:			//离开队伍
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;
	case TEAM_OI_CLOSE:			//关闭组队，只有队长调用才有效果
		nRet = Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(nParam);
		break;
	case TEAM_OI_INVITE_RESPONSE://对组队邀请的回复 ((KUiPlayerItem*)uParam)->nIndex
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(uParam, nParam);
		break;
	case TEAM_OI_APPLY_RESPONSE://批准他人加入队伍，只有队长调用才有效果
		if (uParam)
		{
			if (nParam)
			{//同意
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(uParam);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(uParam);
			}
		}
		break;
	case TEAM_OI_INVITE:		//邀请别人加入队伍，只有队长调用才有效果
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);
			KSystemMessage	sMsg;
			t_sprintf(sMsg.szMessage, strCoreInfo[MSG_TEAM_SEND_INVITE].c_str(), ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			sMsg.nMsgLen = TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case TEAM_OI_CREATE:		//新组队伍
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();//(char*)uParam);
		break;
	case TEAM_OI_GD_MEMBER_LIST://获取主角所在队伍成员列表
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
		break;
	case TEAM_OI_GD_INFO:		//主角所在的队伍信息
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
		}
		break;
	default:
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam,int inParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_SCRIPT_VAL_WITH:
		{//客户端执行脚本！
			if (uParam)
			{
				KUiScrptinfo* pInfo = (KUiScrptinfo*)uParam;
				//char *nVal;
				if (nParam==0)
				{
				}   //sprintf(pInfo->szContent,Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\test.lua","GetItemDescription","",0,""));
				else if (nParam==1)  //面板序号
				{
					ZeroMemory(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo,sizeof(Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo));

					if (inParam==0)  
					{
						if (Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\FsNewTaskWindow.lua","newtaskmain",0,0,""))
						{
							for (int i=0;i<20;++i)
							{
								pInfo->nTaskInfo[i].nTaskidx = Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskidx;
								pInfo->nTaskInfo[i].nTaskType = Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskType;
								t_sprintf(pInfo->nTaskInfo[i].szContent,Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskIconPath);
								t_sprintf(pInfo->nTaskInfo[i].nTaskName,Player[CLIENT_PLAYER_INDEX].m_cTask.nTaskInfo[i].nTaskName);

							}
						}
					}
					else
					{//显示脚本内容

						Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\FsNewTaskWindow.lua","newtaskrequestinfo",inParam,0,"") ;
					}

				}
				else if (nParam==2)
				{
					//RoomMain
				}
				else if (nParam==4)
				{
					//RoomMain
					Player[CLIENT_PLAYER_INDEX].ClientExeItemScript("\\Ui\\roomreset.lua","RoomMain",inParam,0,"") ;

				}
			}  
		}
		break;
	case GDI_PLAYER_SETTIME_INFO:
		{
			KUiPlayerSetTimeInfo *Time = (KUiPlayerSetTimeInfo *)uParam;
			KUiPlyerDoscript nDoscript;
			switch(nParam)
			{
			case 0:
				{//经验银两
					nDoscript.inCurTime  = Time->nYinLiangTime - Player[CLIENT_PLAYER_INDEX].m_YinLiangTime;
					Player[CLIENT_PLAYER_INDEX].m_YinLiangTime = Time->nYinLiangTime;
					nDoscript.inKind=0;
				}
				break;
			case 1:
				{//福缘 声望
					nDoscript.inCurTime  = Time->nExpTime - Player[CLIENT_PLAYER_INDEX].m_ExpTime;
					Player[CLIENT_PLAYER_INDEX].m_ExpTime      = Time->nExpTime;
					nDoscript.inKind=1;
				}
				break;
			case 2:
				{//金币
					nDoscript.inCurTime  = Time->nJinBiTime - Player[CLIENT_PLAYER_INDEX].m_JinBiTime;
					Player[CLIENT_PLAYER_INDEX].m_JinBiTime = Time->nJinBiTime;
					nDoscript.inKind=2;

				}
				break;
			case 3:
				{//积分
					nDoscript.inCurTime = Time->nExpPiontTime - Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime;
					Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime = Time->nExpPiontTime;
					nDoscript.inKind=3;
				}
				break;
			default:
				break;
			}

			SendClientDoScipt(&nDoscript);
		}
		break;
	case GDI_PLAYER_TIME_INFO:	
		{
			int nMin=0,nHour=0;
			if  (uParam)
			{ 
				KUiPlayerActivityInfo *Time = (KUiPlayerActivityInfo *)uParam;

				char nTimeInfo[64];

				nMin  = (g_SubWorldSet.m_nLoopRate)/(18*60);//当前的循环时间   - (int)Player[CLIENT_PLAYER_INDEX].m_dwLoginTime
				nHour = 0;

				if (nMin>60)
				{ 
					nHour = nMin/60;  //小时
					nMin -=60*nHour;
				} 

				Time->nHour     = nHour;
				Time->nMin      = nMin;
				Time->nNpcLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;

				Time->nExpTime       = Player[CLIENT_PLAYER_INDEX].m_ExpTime;
				Time->nExpPiontTime  = Player[CLIENT_PLAYER_INDEX].m_ExpPiontTime;
				Time->nYinLiangTime  = Player[CLIENT_PLAYER_INDEX].m_YinLiangTime;
				Time->nJinBiTime     = Player[CLIENT_PLAYER_INDEX].m_JinBiTime;
				t_sprintf(Time->nTimeInfo,"在线时间:%d小时%d分钟",nHour,nMin);
			} 

		}
		break;
	case GDI_TRADE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
		break;
	case GDI_TRADE_PLAYER_ITEM_COUNT:
		nRet = 0;
		if (nParam==0)
			SendClientCmdGetCount(uParam);   //获取摆摊者的物品数量
		else
			SendClientCmdLianCount(uParam);  //获取链接的物品数量
		break;
	case GDI_TRADE_PLAYER_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;

			for (int i = 0; i < 60; ++i)
			{

				if (nCount>nParam)
					break;

				if (g_cSellItem.m_sItem[i].nIdx)
				{
					pInfo->Obj.uGenre    = CGOG_PLAYERSELLITEM;//CGOG_ITEM;
					pInfo->Obj.uId       = g_cSellItem.m_sItem[i].nIdx;  //物品的索引

					pInfo->Region.Width  = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h      = g_cSellItem.m_sItem[i].nX;
					pInfo->Region.v      = g_cSellItem.m_sItem[i].nY;
					pInfo->nContainer    = g_cSellItem.m_sItem[i].nPrice;
					++nCount;
					++pInfo; //下一个
				}		
			}

			nRet = nCount;
		} 
		else
		{
			nRet = g_cSellItem.GetCount();  //摆摊数量
		}
		break;
	case GDI_PARADE_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_PIFENG,    UIEP_YINJIAN, UIEP_SHIPING
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; ++i)
			{
				pInfo->Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;

					pInfo->Region.Width  = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.v      = PartConvert[i];
					pInfo->Region.h      = 0;
				}
				else
				{
					pInfo->Obj.uGenre    = CGOG_NOTHING;
					pInfo->Region.v      = PartConvert[i];
					pInfo->Region.h      = 0;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		//主角统帅能力相关的数据
		//uParam = (KUiPlayerLeaderShip*) -> 主角统帅能力相关的数据结构指针
	case GDI_PLAYER_LEADERSHIP:
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_dwLeadExp ;		//统帅力经验值
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp;//统帅力经验值
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel ;			//统帅力等级
		}
		break;
	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;
		//获取周围孤单可受邀请的玩家的列表
		//参数含义同GDI_NEARBY_PLAYER_LIST
	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam,nParam);
		break;
	case GDI_LAOHUJI_INFO:
	{
		if (uParam>0)
		   SendClientSetTaskVal(uParam); //设置客户端的 门派唯一序号
	}
	break;
	case GDI_CHESSMENT:
		{
			SendClientCPLockCmd();
		}
		break;
	case GDI_LEFT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;
		}
		break;
		//显示右键技能列表
		//uParam = (KUiSkillData*) pSkills -> 包含65个KUiSkillData的数组用于存储各技能的数据。
		//								KUiSkillData::nLevel用来表述技能显示在第几行
		//Return = 返回有效数据的Skills的数目
	case GDI_RIGHT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
		break;
	case GDI_PLAYER_TIMEINFO:
		{
			if  (uParam)
			{ 
				KUiPlayerGameTime *pTime = (KUiPlayerGameTime *)uParam;
				pTime->npTime = (g_SubWorldSet.m_nLoopRate)/18;  //秒
			}
		}
		break;
		//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo
	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment);				//银两
			pInfo->nXu = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment);
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;						//基本属性剩余点数
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								//力量
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;//身法							//身法
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								//活力
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;	//精力
			pInfo->nPKValue = Player[CLIENT_PLAYER_INDEX].m_cPK.GetPKValue();	//tnpk
			pInfo->nRepute = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); //声望
			pInfo->nFuYuan = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); //富源
			pInfo->nReBorn = Player[CLIENT_PLAYER_INDEX].m_cReBorn.GetReBornNum();   //转生次数
			pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat;
            pInfo->nLuky=Player[CLIENT_PLAYER_INDEX].m_nCurLucky*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsLuKey*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_GoldLucky; //当前幸运值
            pInfo->nZhonji=pNpc->m_CurrentzhongjiRate;  //当前重击m_nCurLucky
            pInfo->nDangge=pNpc->m_CurrentdanggeRate;   //当前档格m_nUpExp
            pInfo->nSprRankId  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_btRankFFId; //称号ID

			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin,&pInfo->nRightKillMax,1);
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN,&pInfo->nKillMAX,0);				//最大最小杀伤力
			pInfo->nAttack      = pNpc->m_CurrentSkillMingZhong+(pNpc->m_CurrentAttackRating+pNpc->m_CurrentAttackRatingEnhancev)*(100+pNpc->m_CurrentAttackRatingEnhancep)/100;				//命中力
			pInfo->nDefence     = pNpc->m_CurrentDefend;				//闪避率
			pInfo->nMoveSpeed   = pNpc->m_CurrentRunSpeed;				//移动速度
			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;			//外攻击速度
			pInfo->nCastSpeed   = pNpc->m_CurrentCastSpeed;             //内攻击速度

			pInfo->nForBitChat  = Player[CLIENT_PLAYER_INDEX].m_nForbiddenFlag;
			//物理防御
			if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
				t_sprintf(pInfo->nPhyDef,"%d/%d/%d", pNpc->m_CurrentPhysicsResist,pNpc->m_CurrentPhysicsResistMax,pNpc->m_PhysicsResistMax);
			else
				t_sprintf(pInfo->nPhyDef,"%d/%d/%d", pNpc->m_CurrentPhysicsResistMax,pNpc->m_CurrentPhysicsResistMax,pNpc->m_PhysicsResistMax);
			//冰冻防御
			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
                t_sprintf(pInfo->nCoolDef,"%d/%d/%d", pNpc->m_CurrentColdResist,pNpc->m_CurrentColdResistMax,pNpc->m_ColdResistMax);
			else
				t_sprintf(pInfo->nCoolDef,"%d/%d/%d", pNpc->m_CurrentColdResistMax,pNpc->m_CurrentColdResistMax,pNpc->m_ColdResistMax);
			//闪电防御
			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
				t_sprintf(pInfo->nLightDef,"%d/%d/%d", pNpc->m_CurrentLightResist,pNpc->m_CurrentLightResistMax,pNpc->m_LightResistMax);
			else
				t_sprintf(pInfo->nLightDef,"%d/%d/%d", pNpc->m_CurrentLightResistMax,pNpc->m_CurrentLightResistMax,pNpc->m_LightResistMax);
			//火焰防御
			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
				t_sprintf(pInfo->nFireDef,"%d/%d/%d", pNpc->m_CurrentFireResist,pNpc->m_CurrentFireResistMax,pNpc->m_FireResistMax);
			else
				t_sprintf(pInfo->nFireDef,"%d/%d/%d", pNpc->m_CurrentFireResistMax,pNpc->m_CurrentFireResistMax,pNpc->m_FireResistMax);
			//毒素防御
			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
				t_sprintf(pInfo->nPoisonDef,"%d/%d/%d", pNpc->m_CurrentPoisonResist,pNpc->m_CurrentPoisonResistMax,pNpc->m_PoisonResistMax);
			else
				t_sprintf(pInfo->nPoisonDef,"%d/%d/%d", pNpc->m_CurrentPoisonResistMax,pNpc->m_CurrentPoisonResistMax,pNpc->m_PoisonResistMax);
			pInfo->nLevel = pNpc->m_Level;
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "Hệ Thuỷ");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "Hệ Mộc");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "Hệ Kim");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "Hệ Hoả");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "Hệ Thổ");
				break;
			}
		}
		break;
	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;		//生命满值
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;				//生命
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;		//内力满值
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;				//内力
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;//体力满值
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;		//体力

			pInfo->nAngryFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQiMax;		//怒满值
			//pInfo->nAngry = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi;			//怒

			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;		        //经验满值
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_nExp;					        //经验
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
			pInfo->nIsGetSkill = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_IsgetSkill;        //是否拾取技能状态
			//	pInfo->nIsGetChaqi = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BtnFlag;           //是否插旗状态
			pInfo->byActionDisable = 0;
			//to do	给pInfo->bActionDisable、赋予合适的值
			pInfo->nVipType = Player[CLIENT_PLAYER_INDEX].m_nVipType; //会员类型
			pInfo->byAction = PA_NONE;

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;
			pInfo->wReserved = 0;
			pInfo->nGameLeftTime=Player[CLIENT_PLAYER_INDEX].m_GameLeftTime;
			pInfo->nIsTranse = Player[CLIENT_PLAYER_INDEX].m_IsTranse;
		}
		break;
	case GDI_PLAYER_BASE_INFO:
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0,nPlayerIdx=0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;  //dwid
				pInfo->nCurFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction;
				pInfo->nCurTong = Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID();
				pInfo->nCurPinDao =	Player[CLIENT_PLAYER_INDEX].m_nCurPinDao;
				//pInfo->nCurFaction = -1; //设置没门派
				nPlayerIdx=CLIENT_PLAYER_INDEX;
				pInfo->nMissionGroup  = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nCurFaction = -1;
				pInfo->nMissionGroup = -1;
				pInfo->nCurTong = 0;
				if (nIndex)
					nPlayerIdx= Npc[nIndex].GetNpcPepoleID();
			}
			if (nIndex)
			{
				strcpy(pInfo->Name, Npc[nIndex].Name);
				//to do:no implements in this version
				pInfo->Agname[0] = 0;
				pInfo->Title[0] = 0;
				if (Npc[nIndex].m_btRankId)
				{
					char szRankId[5];
					 t_sprintf(szRankId,"%d",Npc[nIndex].m_btRankId);
					//itoa(Npc[nIndex].m_btRankId, szRankId, 10);
					g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, 32);
				}

				pInfo->nRankInWorld = Player[nPlayerIdx].m_nWorldStat; //个人排名
				pInfo->nRankInWorld = Npc[nIndex].nRankInWorld;		   //世界排名
				pInfo->nRepute = Npc[nIndex].nRepute;                  //声望
				pInfo->nPKValue = Npc[nIndex].nPKValue;                // PK
				pInfo->nReBorn = Npc[nIndex].nReBorn;                  //转生次数

				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum)
				{//已经召唤了宠物的

					int nNpcIdx=0;
					nNpcIdx =NpcSet.SearchID(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TongBanNum);
					if (nNpcIdx>0)
						pInfo->nTonLuky=Npc[nNpcIdx].m_nCurNpcLucky;
					else
						pInfo->nTonLuky=0;
				}
				else
					pInfo->nTonLuky=0;

				pInfo->nLucky  = Player[nPlayerIdx].m_nCurLucky*Npc[nIndex].IsLuKey*Npc[nIndex].m_GoldLucky; //当前幸运值
				pInfo->nReBornLevel	= Player[nPlayerIdx].m_cReBorn.GetReBornCurLevel();
				if (nParam == 0)
					SendClientSetTaskVal(130); //设置客户端的 门派唯一序号
			}
		}
		break;
	case GDI_PK_SETTING:					//获取pk设置
		nRet = Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState();
		break;
	case GDI_IMMEDIATEITEM_NUM:  //获取快捷面板的物品在总叠加数量
		if (uParam >= 0 && uParam < 9)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameEquipmentItemNum(nParam);
			//nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameDetailItemNum(uParam);
		break;
	case GDI_PLAYER_SKILL_ADDPOINT:
		{//获取额外的增加的技能等级
			if (uParam)
			{ 
				int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(uParam);
				nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddPoint(nList);//---额外技能点
			} 
		}
		break;
	case GDI_ITEM_IN_STORE_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox1);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX2:	// mo rong ruong 2
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox2);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX3:	// mo rong ruong 3
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_exbox3);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_EX:	// 同伴包袱
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre     = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipmentex);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// 第一个是钱
			nCount++;
			// 后面是装备
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
					nCount++;
			}
			nRet = nCount;
		}
		break;
		//主角装备物品
		//uParam = (KUiObjAtRegion*)pInfo -> 包含10个元素的KUiObjAtRegion结构数组指针，
		//			KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
		//			KUiObjAtRegion::Region::h 表示属于第几套装备
		//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
		//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
		//nParam =	要获取的是第几套装备信息
		//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。
	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO：暂时没有做第二套装备
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_PIFENG,    UIEP_YINJIAN, UIEP_SHIPING
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; ++i)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);//获取这个BOX的里的索引
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Region.Width  = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.v      = PartConvert[i];
					pInfo->Region.h      = 0;
				}
				else
				{//这个位置没有东西
					pInfo->Obj.uGenre    = CGOG_NOTHING;
					pInfo->Region.v      = PartConvert[i];
					pInfo->Region.h      = 0;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		//获得物品在某个环境位置的属性状态
		//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
		//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
		//nParam = (int)(bool)bJustTry  是否只是尝试放置
		//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	case GDI_ITEM_IN_ENVIRO_PROP:
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM && pObj->uGenre != CGOG_NPCSELLITEM_PAI)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->uId);
					pItem = BuySell.GetItemidx(nIdx);
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM_PAI)
				{
					//int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->uId);
					pItem = BuySell.GetItemDataByidx(pObj->uId);
				}

				//_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

				int PartConvert[itempart_num] = 
				{ 
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse,		itempart_mask,//   面具
					itempart_pifeng,itempart_yinjian,itempart_shiping	//披风				
				};

				//_ASSERT(pObj->eContainer < itempart_num);

				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;
	case GDI_ITEM_TAKEN_WITH: //玩家随身所带的物品信息！
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId  = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else if (nParam==0)
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		else if (nParam==1)
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetSetPrice()>0)  //摆摊价格大于零的
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom && Item[pItem->nIdx].GetSetPrice()>0)
					nCount++;
			}

			nRet = nCount;
		}
		break;
	    case GDI_PLAYER_HOLD_XU:	
		     nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment); //得到金币
		break;
		case GDI_PLAYER_HOLD_MONEY:
		{
			if (nParam==0)
		        nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment);  //得到剑侠币
			else if (nParam==1)
			    nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetxLient(room_equipment); //得到金币
			else if (nParam==2)
			    nRet = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); //得到富源
			else if (nParam==3)
				nRet = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); //得到声望
			else if (nParam==5)
			{
				  PLAYER_GET_TASKVAL_COMMAND GetTaskval;
				  GetTaskval.ProtocolType = c2s_gettaskval;
				  GetTaskval.m_TaskId     = 192;
				  GetTaskval.m_pDwid      = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;	
				  if (g_pClient)
					g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));

			      nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(192);    //得到积分
			}
			break; 
		}
		break;
	case GDI_FIGHT_SKILL_POINT:
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;
	case GDI_FIGHT_SKILLS:
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
		break;
	case GDI_PLAYER_IS_MALE:
		{
		}
		 break;
		 //主角的立即使用物品与武功
		 //uParam = (KUiPlayerImmedItemSkill*)pInfo
	case GDI_PLAYER_IMMED_ITEMSKILL:
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill(); //左键技能
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();//右键技能

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; ++i)
			{//快捷栏物品
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;
	default:
		break;
	}
	return nRet;
}


//锁定某人并使用技能
int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)  //锁定某人使用技能
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)  //等于自己 就返回
		return 0;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		return 0;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}


int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
//	Player[CLIENT_PLAYER_INDEX].m_AutoplayId;
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;

	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	//取消Lock
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	//if (nRelation != relation_enemy)
	//{
	//	Npc[nIndex].m_nPeopleIdx = nTargetIndex;
	//	return 1;
	//}

	if (nRelation == relation_enemy && Npc[nTargetIndex].m_BaiTan==0)
	{//敌人
		KUiPlayerImmedItemSkill immedItemSkillInfo; //获取快捷的技能
		memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
		GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);	
		if (!LockSomeoneUseSkill(nTargetIndex, immedItemSkillInfo.IMmediaSkill[0].uId))	//锁定失败
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			return 1;
		}
	}
	else if (nRelation == relation_dialog)
	{//对话NPC
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}
	else //( Npc[nTargetIndex].m_Kind==kind_player && Npc[nTargetIndex].m_BaiTan==1)
	{//是玩家 并 是摆摊的
       Npc[nIndex].m_nPeopleIdx = nTargetIndex;
	   return 1;
	}

	return 0;
}

int KCoreShell::LockObjectAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex <= 0)	//取消Lock
		Npc[nIndex].m_nObjectIdx = 0;
	else
		Npc[nIndex].m_nObjectIdx = nTargetIndex;

	return 1;
}
int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT <MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)  //对比名字 是否相同 
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->nIndex = Npc[nT].m_Index;
				p->uId    = Npc[nT].m_dwID;
				p->nData  = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;
		return true;
	}
	return false;
}
int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation);

	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);

	if (nT > 0)
	{
		if (pReturn)
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
		}
		nKind = Npc[nT].m_Kind;
		return true;
	}
	return false;
}

int KCoreShell::GetDataNpcShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = nSaleId;//Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pShopRoom)
			return 0;
		BuySell.m_pShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);

			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;  //页数 加1 
					BuySell.m_pShopRoom->Clear();
					BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);

					if (strstr(pItem->GetItmeInfo(),".lua"))
					{//如果是脚本 则要求服务器同步过来

						ItemXinXi SynItem;
						ZeroStruct(SynItem);
						//SynItem.nIdx  = nIndex; //索引
						SynItem.nX = nBuyIdx;
						SynItem.nY = i;
						SynItem.nIsRongqi = 1;
						SendClientAddItem(&SynItem);

					}

					BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}

				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width  = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItemidx(nIndex);

			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}
//--------------------------------------------------------------------------
//	功能：发出游戏世界数据改变的通知函数
//	返回：如未被注册通知函数，则直接返回0，否则返回通知函数执行结果。
//--------------------------------------------------------------------------
void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam,int inVal)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam,inVal);
}
//--------------------------------------------------------------------------
//	功能：客户端接受处理服务器分派的网络消息
//--------------------------------------------------------------------------
void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];  
}

int KCoreShell::StartUp()
{//开启一个线程循环游戏世界
	Start();
	return 1;
}

int KCoreShell::ClearUp()
{
#ifdef WIN32
		Terminate();
#else
	    Terminate(1);
#endif
		return true;
}

void KCoreShell::Run()
{
/*	int nLoopCount = 0;

	while(true)
	{   
	   //g_NetConnectAgent.Breathe(); //循环处理服务器送来的协议函数
	   if  (nLoopCount>2000000)
	   {//重起 计时器
		   nLoopCount = 0 ;
		   timerStart.Passed(1);
	   }
	   if (nLoopCount*1000 <= timerStart.GetElapse()*18)
	   {
		   Breathe();
		//if (UiHeartBeat())  //接入无限循环
		//{
			++nLoopCount;                           //每秒增加 18帧
		//}
	   }
	   if (nLoopCount*1000 >=timerStart.GetElapse()*18)
	   {
#ifdef WIN32
		 Sleep(1);                 //太快就暂停一秒
#endif
	   }
	   else if ((nLoopCount >> 3))   //每半帧就停
	   {//否则停止绘画游戏世界，就卡住
#ifdef WIN32
		   Sleep(1);               //太快就暂停一秒
#endif
	   }
	}
*/
	return;
}

int  KCoreShell::Breathe()
{
	g_SubWorldSet.MainLoop();    //地图循环
//#ifdef WIN32
	g_ScenePlace.Breathe();      //客户端场景循环
//#endif
	return true;
}

unsigned long  KCoreShell::GetSubGameTime()
{
	return g_SubWorldSet.GetGameTime();
}


int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam,int nIsMianBan,char* strVal)
{
	int nRet = 1;

	switch(uOper)
	{
	case GOI_MAP_SUNYI:
		{		
			int nMapID,nX,nY;
			char nMapName[64]={0},nType[32]={0};
			if (nParam==0)
			{//BB
				g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到当前场景的格子坐标
				int nNpcID= NpcSet.SearchGSName(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
				if (nNpcID>0)
				{  
					SendClientCmdSunyi(nMapID,nX*32,nY*64,Npc[nNpcID].m_dwID);  //发送瞬移命令	
				} 
			}
			else if (nParam==1)
			{//点击地图瞬移动 或 执行脚本
				if (uParam>0)
				   SendClientCmdSysShop(4,uParam);   //执行脚本
			}
		}
		break;
	case GOI_ITEM_LIANJIE:// 发送链接信息
		{
			KUiItemLian* pInfo = (KUiItemLian*)uParam;  
			SendClientLianJie(pInfo->nPlayerName,pInfo->nItemDWID);  //发送链接信息
		}		
		break;
	case GOI_SYN_SHOPIDX:
		SendClientOpenMarket(nParam,uParam);
		break;
	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;
	case GOI_TRADE_WILLING:
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		}
		break;
		//锁定交易
		//nParam = (int)(book)bLock 是否锁定
	case GOI_TRADE_LOCK:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
			break;
		//if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(nParam);
		//else
			//Player[CLIENT_PLAYER_INDEX].TradeApplyLock(1);
		break;
		//交易
	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1);		// 交易确定
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2);		// 交易确定取消
		}
		break;
		//交易取消
	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		// 交易取消
		break;
	case GOI_TRADE_INVITE_RESPONSE:
		{
			KTrade::ReplyInvite(uParam,nParam);
		}
		break;
	case GOI_GIVE_ITEM_FAIL:
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
			npid=(int)(pcui->uId);
			SendClientCmdGive(npid,nParam,0,pcui->m_Callback,pcui->m_Error);
		}
		break;
	case GOI_GIVE_ITEM_NOBACK:  //错误提示
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
			npid=(int)(pcui->uId);

			SendClientCmdGive(npid,nParam,2,pcui->m_Callback,pcui->m_Error);
		}
		break;
	case GOI_GIVE_ITEM_BACK:
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
			npid=(int)(pcui->uId);
			SendClientCmdGive(npid,nParam,1,pcui->m_Callback,pcui->m_Error);  //nParam 为 nItemNum参数，1 为kind 参数
		}
		break;
	case GOI_GIVE_ITEM_QIANG:  //强化
		{
			PLAYER_GIVEUI* pcui = (PLAYER_GIVEUI*)uParam;
			int npid;
			npid=(int)(pcui->uId);
			SendClientCmdGive(npid,nParam,3,pcui->m_Callback,pcui->m_Error);  //nParam 为 nItemNum参数，1 为kind 参数
		}
		break;
	case GOI_LIXIAN: //离线脚本
		SendClientCmdLiXian();
		break;
	case GOI_TRADE_PLAYER_BUY:  //买东西-摆摊  ---直接购买 没有跳出支付窗口
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_PLAYERSELLITEM != pObject1->Obj.uGenre)
				break;			

			int nWidth, nHeight;
			ItemPos	Pos;
			///////////////////////////////////摆摊税收/////////////////////////////////////////
			int nMapID,nX,nY,nShuiShou,nIsTong=0;
			char nMapName[64]={0},nType[32]={0},nMasterName[32]={0};
			g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
			//			int nIdx = 0;
			//			KItem* pItem = NULL;
			t_sprintf(nMasterName,SubWorld[0].nWorldMapInfo[STR_MAP_MASTER].c_str());
			nIsTong=0;
			nShuiShou=0;//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_WarShuishou;
			///////////////////////////////////////////////////////////////////////////////
			nWidth  = Item[pObject1->Obj.uId].GetWidth();
			nHeight = Item[pObject1->Obj.uId].GetHeight();

			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;

				KSystemMessage	sMsg;			
				strcpy(sMsg.szMessage,strCoreInfo[MSG_SHOP_NO_ROOM].c_str());
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

			BYTE nModelval=0;
			nModelval=Item[pObject1->Obj.uId].GetcModel();

			if ((nModelval==0 || nModelval==1) && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[pObject1->Obj.uId].GetSetPrice())
			{//默认是银两？
				nRet = 0;
				KSystemMessage	sMsg;				
				t_sprintf(sMsg.szMessage,strCoreInfo[MSG_SHOP_NO_MONEY].c_str(),nShuiShou,"%%");
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText_(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;

			}
			else if (nModelval==2 && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() < Item[pObject1->Obj.uId].GetSetPrice())
			{
				KSystemMessage	sMsg;				
				t_sprintf(sMsg.szMessage, "提示:另加税收(%d%s),金币不足!",nShuiShou,"%%");
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

			nRet = 1;
			//g_cSellItem.FindIdx(pObject1->Obj.uId)
			SendClientCmdPlayerBuy(Item[pObject1->Obj.uId].GetID(),nParam, pos_equiproom,Pos.nX,Pos.nY,nIsTong,nShuiShou,nMapID,nModelval);
		}
		break;
	case GDI_PLAYER_TRADE:
		{
			char* sShopName = (char *)uParam;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
			{
				KSystemMessage	sMsg;
				t_sprintf(sMsg.szMessage, "提示:骑马状态中不能进行摆摊！");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				nRet = 0;
				break;
			}

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
			{
				KSystemMessage	sMsg;
				t_sprintf(sMsg.szMessage, "提示:战斗状态不能进行摆摊!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				nRet = 0;
				break;
			}

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
				SendClientCmdSit(TRUE);
			}

			if (nParam && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan == 0)
				SendClientCmdStartTrade(1,sShopName);      //开始摆摊商城名
			else
			{
				SendClientCmdStartTrade(0,"系统商城");     //关闭摆摊
				nRet=0;
			}
		}
		break;
	case GDI_SET_TRADE_ITEM:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();

			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (!Item[nIdx].GetIsBang() && Item[nIdx].GetUseMapIdx()<=0)
				{  
					Item[nIdx].SetPrice(nParam);                                 //设置客户端摆摊标价格
					Item[nIdx].SetModel(nIsMianBan);
					SendClientCmdSetPrice(Item[nIdx].GetID(),nParam,nIsMianBan); //设置服务器端摆摊标价
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("绑定/限时/锁魂物品不能交易!")	;
				}
			}
		}
		break;
	case GOI_VIEW_PLAYERSELLITEM_END:  //结束摆摊
		if (nParam==0)
			g_cSellItem.DeleteAll();
		else
			g_cSellItem.DeleteLAll();
		break;
	case GOI_VIEW_PLAYERSELLITEM:  //看摆摊物品面板
		{
			if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan)
			{
				if (nParam<=0 || nParam>=MAX_NPC) break;
				unsigned int dwid = Npc[nParam].m_dwID;
				g_cSellItem.ApplyViewItem(dwid);
				//CCMessageBox(Npc[nParam].Name,"看摊");
			}
		}
		break;
	case GOI_REVENGE_SOMEONE: //仇杀某人？
		if (nParam)
		{
			if (nParam<=0 || nParam>=MAX_NPC) break;

			unsigned int dwid = Npc[nParam].m_dwID;
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplyEnmityPK(dwid);
		}
		break;
	case GOI_VIEW_PLAYERITEM: //发送玩家查看装备信息协议
		{
			g_cViewItem.ApplyViewEquip(nParam);
		}
		break;

	case GOI_AUTO_COMMAND:   //外挂
		{ 
			switch(uParam) 
			{ 
			case 0:{ //血
				ItemPos    Pos; 
				PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 

				if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
					(Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_blood 
					|| Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
				{ 
					Pos.nPlace = pItem->nPlace; 
					Pos.nX = pItem->nX; 
					Pos.nY = pItem->nY; 
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
					return 1; 
				} 
				while(pItem) 
				{ 
					pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
					if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
						(Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_blood 
						|| Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
					{ 
						Pos.nPlace = pItem->nPlace; 
						Pos.nX = pItem->nX; 
						Pos.nY = pItem->nY; 
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos); 
						return 1; 
					} 
				} 
				   }break; 
			case 1:{//蓝
				ItemPos    Pos; 
				PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 
				if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
					(Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_mana 
					|| Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
				{ 
					Pos.nPlace = pItem->nPlace; 
					Pos.nX = pItem->nX; 
					Pos.nY = pItem->nY; 
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
					return 1; 
				} 
				while(pItem) 
				{ 
					pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
					if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
						(Item[pItem->nIdx].GetGenre() == item_medicine && (Item[pItem->nIdx].GetDetailType() == medicine_mana 
						|| Item[pItem->nIdx].GetDetailType() >= medicine_allboth || Item[pItem->nIdx].GetDetailType() == medicine_both))) 
					{ 
						Pos.nPlace = pItem->nPlace; 
						Pos.nX = pItem->nX; 
						Pos.nY = pItem->nY; 
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos); 
						return 1; 
					} 
				}                
				   }break; 
			case 2:
				{//  使用回城卷？
					if (!nParam)
					{
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
						break;
					}

					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode==0)
						break;	//非战斗模式退出
					ItemPos    Pos; 
					PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 
					if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
						((Item[pItem->nIdx].GetGenre()==item_task && Item[pItem->nIdx].GetDetailType()==35) || Item[pItem->nIdx].GetGenre() == item_townportal))
					{ 
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
						Pos.nPlace = pItem->nPlace; 
						Pos.nX = pItem->nX; 
						Pos.nY = pItem->nY; 
						Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos); 
						return 1; 
					} 

					while(pItem) 
					{ 
						pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem(); 
						if (pItem && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
							((Item[pItem->nIdx].GetGenre()==item_task && Item[pItem->nIdx].GetDetailType()==35) || Item[pItem->nIdx].GetGenre() == item_townportal))
						{ 
							Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =nParam;
							Pos.nPlace = pItem->nPlace; 
							Pos.nX = pItem->nX; 
							Pos.nY = pItem->nY; 
							Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx, Pos);
							return 1; 
						} 
					} 
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsReTurn =FALSE;
				}
				break; 
			case 3:{  //自动捡钱
				//	Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(166, 1);
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney=nParam;
				   }break;
			case 4:{  //自动捡装备
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques=nParam;
				if (nIsMianBan>6)
					nIsMianBan=6;
				if (nIsMianBan<0)
					nIsMianBan=0;

				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum=nIsMianBan;

				   }break;
			case 5:{  //自动钱和装备
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques=nParam;
				if (nIsMianBan>6)
					nIsMianBan=6;
				if (nIsMianBan<0)
					nIsMianBan=0;
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nShuXingNum=nIsMianBan;
				   }break;
			case 6:{  //自动图谱
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu=nParam;
				   }break;
			case 7:{  //自动押送
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCurNum=nParam;
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsHaveDo=nParam;
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen=0;
				   }break;
			case 8:{  //录制脚本
				char nFiledPath[128];
				t_sprintf(nFiledPath,"jx50ai/%u_Pos.txt",g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
				if  (nParam==TRUE)
				{
					//KTabFileCtrl nDel;
					//清空文件
					Player[CLIENT_PLAYER_INDEX].nMapPos.CreatFile(nFiledPath);
					if (Player[CLIENT_PLAYER_INDEX].nMapPos.Load(nFiledPath))
					{
						Player[CLIENT_PLAYER_INDEX].nMapPos.isload = true;
						/*while(Player[CLIENT_PLAYER_INDEX].nMapPos.GetHeight()>0)
							Player[CLIENT_PLAYER_INDEX].nMapPos.Remove(Player[CLIENT_PLAYER_INDEX].nMapPos.GetHeight());//BUG
						Player[CLIENT_PLAYER_INDEX].nMapPos.Save(nFiledPath);*/
						//nDel.Clear();
					}
					//nDel.Clear();
				}
				//停止
				//if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen)
				{//如果是已经加载的就删除
				Player[CLIENT_PLAYER_INDEX].nMapPos.Save(nFiledPath);
				Player[CLIENT_PLAYER_INDEX].nMapPos.Clear();
				Player[CLIENT_PLAYER_INDEX].nMapPos.isload = false;
				}
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen=nParam;

				   }break;
			case 9:{  //挂机模式
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
				   }break;
			case 10:{ //自动应答组队
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsHuiDa=nParam;
					}break;

			case 11:{ //我是队长
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiZhang=nParam;
					}break;
			case 12:{ //我是队员
				Player[CLIENT_PLAYER_INDEX].m_Autoplay.nCteam.nIsDuiYuan=nParam;
					}break;
			case 14:
				{  //道具拾取
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool=nParam; //nAutoTool
				}
				break;
			case 17:
				{  //设定拾取次数
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nVal=nParam;
				}
				break;
			case 20:
				{  //跑到指定地点的时间
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTime=nParam;
				}
				break;
			case 21:
				{  //技能
					if (nParam>0)
					{
						int nRows=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130);
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nIsOpen=nIsMianBan;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nsFactionTaskVal=nRows;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nCostTime=5;
						if  (nRows>0)
						{
							char nKillKey[32];
							t_sprintf(nKillKey,"skill_%d",nParam-1);
							Player[CLIENT_PLAYER_INDEX].nGuaSkill.GetInteger(nRows+1,nKillKey,0,&Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoSkill[nParam-1].nsKillId);
						}					    
					}
				}
				break;
			case 26:
				{  //自动使用物品
				//Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel=nParam;
				    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsAutoUseItem=nParam;
					if (nParam)
					{
						ItemPos    Pos; 
						PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem(); 

				        int mItemInfo[3],mprg=0,nprg=0;
						while(pItem) 
						{ 
							ZeroMemory(mItemInfo,sizeof(mItemInfo));
							mItemInfo[0]=Item[pItem->nIdx].GetGenre();
							mItemInfo[1]=Item[pItem->nIdx].GetDetailType();
							mItemInfo[2]=Item[pItem->nIdx].GetParticular();
			                mprg=NpcSet.CheckForBit(mItemInfo,"IsUseStack",3);//是否可以叠加使用
							nprg=0;	 
							if (pItem && mprg==1 && (pItem->nPlace == pos_equiproom || pItem->nPlace == pos_immediacy) && 
							(mItemInfo[0] == item_mine || mItemInfo[0] == item_task)) 
							{ 
								char nItemName[32],nTempItemName[64];
								for (int i=0;i<16;++i)
								{	
									Player[CLIENT_PLAYER_INDEX].nCheckName.GetString(i+1,1,"封神暂无",nItemName,sizeof(nItemName));
									t_sprintf(nTempItemName,UTEXT(Item[pItem->nIdx].GetName(),1).c_str());
									if (strstr(nTempItemName,nItemName))
									{//名称相同 就使用
										if ((mItemInfo[0] == item_task && (mItemInfo[1]==95 || mItemInfo[1]==96 || mItemInfo[1]==97)) || (mItemInfo[0] == item_mine && (mItemInfo[2]==72 || mItemInfo[2]==73||mItemInfo[2]==74)))
										{//如果是天山玉露 等东西 就检测状态
										  if ((mItemInfo[0] == item_task && mItemInfo[1]==95)  || (mItemInfo[0] == item_mine && mItemInfo[2]==72))
										  {//仙草露
											   if(GetSkillLiveTime(440)<=0)
											   {
												   nprg=1;
												   break;
											   }
										  }
										  else if ((mItemInfo[0] == item_task && mItemInfo[1]==96)  || (mItemInfo[0] == item_mine && mItemInfo[2]==73))
										  {//天山

											  if(GetSkillLiveTime(441)<=0)
											  {
												  nprg=1;
												  break;
											   }

										  }
										  else if ((mItemInfo[0] == item_task && mItemInfo[1]==97)  || (mItemInfo[0] == item_mine && mItemInfo[2]==74))
										  {//白果
											  
											  if(GetSkillLiveTime(442)<=0)
											  {
												  nprg=1;
												  break;
											  }	  
										  }
										}
										else
										{//其他东西,直接使用了
											nprg=1;
										    break;
										}	
									}
								} 

							   if (nprg)
							   {//是可以使用的
								 Pos.nPlace = pItem->nPlace; 
								 Pos.nX = pItem->nX; 
								 Pos.nY = pItem->nY; 
								 Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pItem->nIdx,Pos);
							   }
								//return 1; 
							}
							
						  pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();	//下一个物品
						} 
					}
				}													
				break;
            case 27:
				{  //设置左键技能
					if (nParam>0)
					{
					   if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode==0)
						break;	//非战斗模式退出

					   /*if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level<90)
					   {
						   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("<color=yellow>助手<color>:等级小于90级,启用重连自动挂机失败!");
						   break;	//非战斗模式退出
					   }*/
					   
					    //if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nIsOpen==1)
					    ///	 break;//已经设置过了

						int nRows=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(130);
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsFactionTaskVal=nRows;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nCostTime=5;
						Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsRightKillId=0;
					   KIniFile nAutoConfig;
					   char nConfig[128]={0};
					   sprintf(nConfig,"jx50ai/autoset/%u_set.ini",g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
					   if  (!nAutoConfig.Load(nConfig))
					   { 
						  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:角色配置不存在!");
						  break;
					   }	 
					    nAutoConfig.GetInteger("AAAA","LeftSkill_0",0,&Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId);	
						nAutoConfig.Clear();

						if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId<=0)
						{
							nRet=0;
							Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:请设置左键技能,点击快捷技能栏即可!");
							break;
						}

						if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId!=Player[CLIENT_PLAYER_INDEX].GetLeftSkill())//获取当前的左键技能
						{//左键技能不相同
							if  (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId)<=0)
								break;
							    Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nIsOpen=1;
								Player[CLIENT_PLAYER_INDEX].SetLeftSkill(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoLRskill.nsLeftKillId,TRUE);  //设置左键技能同步到服务器
						}
						else
						 nRet=0;		
					}
                }
			break;
			case 28:
				{  //秒杀蓝绿怪
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsNoBlue=nParam;
                }
				break;
			case 29:
				{  //跟随队长
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsFollow=nParam;
					/*KUiPlayerTeam* pTeam ;
					memset(&pTeam, 0, sizeof(KUiPlayerTeam)); //	功能：初始化结构数据
					nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam); //获取队伍队长的信息
					*/
					if (nParam && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag && Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure ==TEAM_MEMBER)
					{//已经入队 并且是队员
						  if (g_Team[0].m_nCaptain>-1)
						  {
							  int nNpcIdx=NpcSet.SearchID((DWORD)g_Team[0].m_nCaptain);
							  if (nNpcIdx>0)
							  {//队长在线的
								  if (Npc[nNpcIdx].m_bLockNpcDwID>0)
								  {
									   int nSkillMagic = 0;
									       nSkillMagic = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(Player[CLIENT_PLAYER_INDEX].GetLeftSkill());
									   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetActiveSkill(nSkillMagic); 
									   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx   =  NpcSet.SearchID(Npc[nNpcIdx].m_bLockNpcDwID);
								  
								  }
								  else
								       Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx	= nNpcIdx;

								   if (Npc[nNpcIdx].m_Doing == do_sit)
								   {
									   if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
									   {
									      if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
										  { 
									         Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
                                             SendClientCmdSit(TRUE);
										  }   
									   }
								   } 
							  }
						  }
					}
                }
				break;
			case 31:
				{  //寻挂范围
					Player[CLIENT_PLAYER_INDEX].m_Autoplay.nNpcDis=nParam;
				}
				break;
			default:
				break;
			}
		}
		break;
	case GOI_AUTOPALYOPEN:  
		{// 挂机开始
		  NpcAI.setPickStateNone();
		  SendClientAutoPlay(uParam,nParam,nIsMianBan);  //发送命令执行脚本
		}
		break;
		//增强一些属性的值，一次加一点
		//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一
	case GOI_TONE_UP_ATTRIBUTE:
		if (nParam>0 && nParam<=10)
		{
			switch (uParam)
			{ 
			case UIPA_STRENGTH:		//力量
				Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, nParam);
				break;
			case UIPA_DEXTERITY:	//敏捷
				Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, nParam);
				break;		
			case UIPA_VITALITY:		//活力
				Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, nParam);
				break;
			case UIPA_ENERGY:		//精力
				Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, nParam);
				break;		
			} 
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("请设置每次加点数(1--10)之间!");	   
		}
		break;
	case GOI_DATAU:// 老头任务 抽奖
		SendClientDaTau(uParam,-1);
		break;
	case GOI_PLAYER_SYSSHOP:    //通用调用脚本
		{
			if (uParam)
			{//任务自动寻路用的
				KUiTongYong *nTy = (KUiTongYong*)uParam;
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex==g_SubWorldSet.SearchWorld(nTy->inParma))
				{//相同地图
					int nSrcMpsX,nSrcMpsY,nDmap,nDistance;
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nSrcMpsX,&nSrcMpsY,&nDmap);
					nDistance = SubWorld[0].GetDistance(nSrcMpsX,nSrcMpsY,nTy->inParmb*32,nTy->inParmc*32);

					if (nDistance>=2000)
					{//相同区域 就用跑的	
						SendClientCmdSysShop(nTy->inKind,nTy->inParma,nTy->inParmb,nTy->inParmc,nTy->inParmd);
					}
					else
					{//不同区域就飞               
						g_ScenePlace.FindPos(nTy->inParmb/8, nTy->inParmc/16, true);
					}
				}
				else
				{//不同地图，直接飞
					SendClientCmdSysShop(nTy->inKind,nTy->inParma,nTy->inParmb,nTy->inParmc,nTy->inParmd);
				}             
			}
			else
			{
				if (nIsMianBan==0)
				{ //怒气
					SendClientCmdSysShop(0,nParam);
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentNuQi=0;
				}
				else
				{//其他类型
					SendClientCmdSysShop(nIsMianBan,nParam);
				}
			}	
		}
		break;
		case GOI_MONEY_INOUT_STORE_BOX:
		{
			BOOL	bIn = (BOOL)uParam;
			int		nMoney  = nParam;
			int		nSrcRoom, nDesRoom;
			if (nMoney<0)
				break;
			int nCurMoney = 0;
			if (bIn)
			{//存钱
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
				nCurMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_equipment);
			  if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoplayId)
			  {//挂机中
				  Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("助手:执行存钱成功!");
			  } 
			}
			else
			{//取钱
				nCurMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetsMon(room_repository);
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}

			if (nMoney>nCurMoney)
				nMoney = nCurMoney;

			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom,nDesRoom,nMoney);
		}
		break;
	case GOI_JINDUTIAO_CALLBACK:
		{
			if (uParam)
			{
				nJinDuClient *nJd = (nJinDuClient*)uParam;
				SendClientJdCallBack(nJd);
			}
		}
		break;
		//玩家点完对话框
	case GOI_INFORMATION_CONFIRM_NOTIFY:
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = 0;//选择第一条?
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
			break;
		}
	case GOI_PLAYER_RENASCENCE:  //重生选择
		{
			if (nParam>=0)
				SendClientCmdRevive(nParam);
		}
		break;
	case GOI_PK_SETTING: //PK设置
		Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam);
		break;
	case GOI_TONE_UP_SKILL:
		{
			Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
		}
		break;
	case GOI_TRADE_NPC_BUY:  //NPC买东西 商城
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
			{//如果不是NPC商店就跳出
				break;
			}
			int nIdx = 0;
			KItem* pItem = NULL;
			nIdx      = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
			pItem     = BuySell.GetItemidx(nIdx);

			if (pItem==NULL)
			{
#ifdef WIN32
				CCMessageBox("物品数据有误,请联系GM处理!","提示");
				//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:物品数据有误,请联系GM处理!");
#else
				CCMessageBox(UTEXT("物品数据有误,请联系GM处理!",1).c_str(),UTEXT("提示",1).c_str());
#endif 
				break;
			}
			int nWidth, nHeight;
			ItemPos	Pos;
			nWidth = pItem->GetWidth();
			nHeight = pItem->GetHeight();

			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{//查找位置
				nRet = 0;
#ifdef WIN32
				CCMessageBox("包袱空间不足!","提示");;
#else
				CCMessageBox(UTEXT("包袱空间不足!",1).c_str(),UTEXT("提示",1).c_str());
#endif 
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;
#ifdef WIN32
				CCMessageBox("包袱空间不足!","提示");;
#else
				CCMessageBox(UTEXT("包袱空间不足!",1).c_str(),UTEXT("提示",1).c_str());
#endif
				break;
			}
			int nPrice = pItem->GetPrice();
			//int nJB  = pItem->GetPriceXu();
			int  nSellModel = pItem->GetcModel();
			int nBei = 1;
			if (nParam>1)
			{
				if (!pItem->IsStack())
				{
#ifdef WIN32
					CCMessageBox("该物品不能批量购买!","提示");;
#else
					CCMessageBox(UTEXT("该物品不能批量购买!",1).c_str(),UTEXT("提示",1).c_str());
#endif					
					break;	
				}
				nBei =nParam;					
			}

			if (nPrice >0)/// && Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() >= nJXB*nBei /*&& pObject1->eContainer==UOC_NPC_SHOP*/)
			{
				if (nSellModel == moneyunit_money || nSellModel == moneyunit_moneya)
				{//银两不足
					if  (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < nPrice*nBei)
						break;
				}
				else if (nSellModel == moneyunit_xu)
				{//金币不足
					if  (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentXu() < nPrice*nBei)
						break;
				}
				else if (nSellModel == moneyunit_fuyuan)
				{//富源不足
					if  (Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue() < nPrice*nBei)
						break;
				}
				else if (nSellModel == moneyunit_repute)
				{//声望不足
					if  (Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue() < nPrice*nBei)
						break;
				}
				else if (nSellModel == moneyunit_expoint)
				{//点卡不足	Player[nPlayerIdx].GetExtPoint()
					if  (Player[CLIENT_PLAYER_INDEX].GetExtPoint() < nPrice*nBei)
						break;
				}
				else if (nSellModel == moneyunit_jifen)
				{//积分	   Player[nPlayerIdx].GetExtPoint()
					//CCMessageBox("积分购买","test");
					if  (Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(192)< nPrice*nBei)
						break;
				}
				int nMapID,nX,nY;
				char nMapName[64]={0},nType[32]={0},nMasterName[32]={0};
				g_ScenePlace.GetSceneNameAndFocus(nMapName, nMapID,nX, nY,nType); //得到场景的格子坐标
				SendClientCmdBuy(pObject1->Obj.uId,pos_equiproom,Pos.nX, Pos.nY,0,0,nMapID,nParam); //发送买卖的信息给服务器
				break;
			} //检测身上的银两
		}
		break;
	case GOI_STRINGBOX:// 字符串输入界面
		{
			PLAYER_STRINGUI* psui = (PLAYER_STRINGUI*)uParam;
			SendClientString(psui->m_Content,psui->m_SCallback,psui->m_SError,nParam);
		}
		break;
	case GOI_ADDITEM_CLIENT: //包袱增加物品
		if (uParam)
		{	
			if (!g_GameWorld) break;

			if (nParam<room_equipment || nParam>room_num)
				nParam = room_equipment;
			KUiObjAtContRegion *pObj=(KUiObjAtContRegion *)uParam;
			int nIndex=pObj->Obj.uId;
			if (nIndex)
			{
				ItemPos	sItemPos;
				if (nParam==room_immediacy)
				{//如果目标为快捷栏
					if (g_GameWorld->CheckImmediacyBox(Item[nIndex].GetGenre(),Item[nIndex].GetDetailType()))
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("V?t ph?m ?? t?n t?i!");
					    break;
					}

					if (!Item[nIndex].GetIsKuaiJie())
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Kh?ng th? ??t v?t ph?m này trong thanh phím t?t!");
						break;
					}
					int nGenre = Item[nIndex].GetGenre();
					if  (nGenre==item_equip ||  nGenre ==item_materials || nGenre ==item_fusion)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Kh?ng th? ??t v?t ph?m này trong thanh phím t?t!");
						break;
					}

					if  (nGenre<=item_equip && nGenre >=item_number)
					{
						Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Kh?ng th? ??t v?t ph?m này trong thanh phím t?t!");
						break;
					}
				}

				if (FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchItemPosition(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(),&sItemPos,nParam))
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Kh?ng ?? ch? tr?ng!");
					break;
				}  
				switch(pObj->eContainer) //源容器信息
				{
				case UOC_IMMEDIA_ITEM:		//快捷栏
					sItemPos.nPlace  = pos_immediacy;         //源容器
					break;
				case UOC_STORE_BOX:		//储物箱
					sItemPos.nPlace  = pos_repositoryroom;    //源容器
					break;
				case UOC_EX_BOX1:            //扩展箱1
					sItemPos.nPlace  = pos_exbox1room;    //源容器
					break;
				case UOC_EX_BOX2:
					sItemPos.nPlace  = pos_exbox2room;    //源容器
					break;
				case UOC_EX_BOX3:
					sItemPos.nPlace  = pos_exbox3room;    //源容器
					break;
				case UOC_ITEM_EX:          //同伴包袱
					sItemPos.nPlace  = pos_equiproomex;    //源容器
					break;
				case UOC_ITEM_TAKE_WITH:
					sItemPos.nPlace  = pos_equiproom;      //源容器
					break;
				case UOC_EQUIPTMENT:
					sItemPos.nPlace  = pos_equip;          //源容器
					break;
				case UOC_ITEM_DAZAO://单对象容器
					sItemPos.nPlace  = pos_dazao;          //源容器
					break;
				case UOC_ITEM_CAILIAO://多对象容器
					sItemPos.nPlace  = pos_cailiao;        //源容器
					break;
				case UOC_ITEM_GIVE:
					sItemPos.nPlace  = pos_give;           //源容器
					break;
				case UOC_TO_BE_TRADE:
					sItemPos.nPlace  = pos_traderoom;      //源容器
				default:
					break;
				}
				sItemPos.nX      = pObj->Region.h; //源位置
				sItemPos.nY      = pObj->Region.v; //源位置
				//nParam 为目标容器类型ｒｏｏｍ类型
				Player[CLIENT_PLAYER_INDEX].ApplyAutoMoveItem(nIndex,sItemPos,nParam);
			}
		}
		break;
	
	case GOI_TRADE_NPC_SELL:
		{
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("?ang ti?n hành bán v?t ph?m");
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (Item[nIdx].GetGenre() == item_task /*|| Item[nIdx].GetGenre() ==item_equip || Item[nIdx].GetGenre() ==item_mine*/)
				{
					if (Item[nIdx].GetIsSell() == 1)
					{//是可以买卖的	
						SendClientCmdSell(Item[nIdx].GetID());
						return 1;
					}
					else 
					{
#ifdef WIN32
						CCMessageBox("任务/限时/绑定物品不能买卖!","提示:");
#else
						CCMessageBox(UTEXT("任务/限时/绑定物品不能买卖!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
						return 0;
					}
				}

				if (Item[nIdx].GetIsBang())
				{
#ifdef WIN32
					CCMessageBox("任务/限时/绑定物品不能买卖!","提示:");
#else
					CCMessageBox(UTEXT("任务/限时/绑定物品不能买卖!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					return 0;
				}

				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR:
		{//修理
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{		
				if (Item[nIdx].GetGenre() != item_equip)
				{//如果不是装备的 不能修理
#ifdef WIN32
					CCMessageBox("该物品不能修理!","提示:");
#else
					CCMessageBox(UTEXT("该物品不能修理!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					return 0;
				}
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability())
				{//永不磨损 或 持久 是满的 不需要修理
#ifdef WIN32
					CCMessageBox("该物品无需修理!","提示:");
#else
					CCMessageBox(UTEXT("该物品无需修理!",1).c_str(),UTEXT("提示:",1).c_str());
#endif				
					return 0;
				}
				else if (Item[nIdx].GetRepairPrice() <= Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney())
				{//修理的价格 小于包袱的价格 就可以修理了
					SendClientCmdRepair(Item[nIdx].GetID());
				}
				else
				{
#ifdef WIN32
					 CCMessageBox("银两不足,不能修理!","提示:");
#else
					CCMessageBox(UTEXT("银两不足,不能修理!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					return 0;
				}

				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_NPC_ITEM_BREAK:   
		{//拆分可叠物品
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
			{
				nRet = 0;
				break;
			}
			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx < MAX_ITEM && nIdx != 0)
			{		

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand())
				{
#ifdef WIN32
					CCMessageBox("拆分:手上有东西,拆分失败!","提示:");
#else
					CCMessageBox(UTEXT("拆分:手上有东西,拆分失败!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					nRet = 0;
					break;
				}

				if (Item[nIdx].GetStackNum() > nParam)
				{
					int ncount = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFreePositionCount(Item[nIdx].GetWidth(),Item[nIdx].GetHeight(),room_equipment);

					if (ncount<nParam)
					{
#ifdef WIN32
						CCMessageBox("包袱没有足够的空间!","提示:");
#else
						CCMessageBox(UTEXT("包袱没有足够的空间!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
						nRet = 0;
						break;
					}

					ItemPos sItemPos;
					if (FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchItemPosition(Item[nIdx].GetWidth(),Item[nIdx].GetHeight(),&sItemPos,room_equipment))
					{

					}
					Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nParam);
					SendClientCmdBreak(Item[nIdx].GetID(),nParam);
				}
				else
				{
					/*KSystemMessage	sMsg;
					t_sprintf(sMsg.szMessage, "提示:拆分失败或该物品没有叠加!");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					*/
#ifdef WIN32
					CCMessageBox("提示:拆分失败或该物品没有叠加!","提示:");
#else
					CCMessageBox(UTEXT("提示:拆分失败或该物品没有叠加!",1).c_str(),UTEXT("提示:",1).c_str());
#endif
					return 0;
				}

				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	//自动移动到容器
	case GOI_CHANGE_ITEM:
		break;
		//使用物品
		//uParam = (KUiObjAtRegion*)pInfo -> 物品的数据以及物品原来摆放的位置
		//nParam = 物品使用前放置的位置，取值为枚举类型UIOBJECT_CONTAINER。
	case GOI_USE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_EX_BOX1:           //扩展箱1
				Pos.nPlace = pos_exbox1room;
				break;
			case UOC_EX_BOX2:
				Pos.nPlace = pos_exbox2room;
				break;
			case UOC_EX_BOX3:
				Pos.nPlace = pos_exbox3room;
				break;
			case UOC_ITEM_EX:           //同伴包袱
				Pos.nPlace = pos_equiproomex;
				break;
			case UOC_STORE_BOX:        //储物箱
				Pos.nPlace = pos_repositoryroom;
				break;
			case UOC_ITEM_TAKE_WITH:   // 随身物品
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_IMMEDIA_ITEM:     //立即物品快捷
				Pos.nPlace = pos_immediacy;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}
			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;
			if (pInfo->Obj.uGenre == CGOG_ITEM && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
				Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId,Pos);
		}
		break;
		//nParma = nAnswerIndex
	    case GOI_QUESTION_CHOOSE:
			{
			  if (g_bUISelLastSelCount == 0 )
				 break;	
			  PLAYER_SELECTUI_COMMAND command;
			  command.nSelectIndex = nParam;
			  Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command,UI_SELECTDIALOG);
			}
		break;
      case GOI_PLAYER_ACTION:
	  {
		switch(uParam)
		{
		case PA_RUN:
			Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
			break;
		case PA_SIT:
			if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
			{
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
				{
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_sit);
					SendClientCmdSit(TRUE);
				}
				else
				{
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_stand);
					SendClientCmdSit(FALSE);
				}
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("正在骑马中!不能打坐！");
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_CLICK;
				Msg.eType = SMT_PLAYER;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:骑马中!不能打坐！");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case PA_RIDE://骑马的信息？
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
			{
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendSerCommand(do_stand);
				SendClientCmdSit(FALSE);
			}//	坐着--首先要站起来才可以 骑马！！！

			SendClientCmdRide(); //骑马发送函数

			break;
		case PW_NOT_SAME: // 不能相同？
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:密码不能相同");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case PW_NOT_LONG: // mat khau ko du do dai
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:两次密码长度不同！");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case PW_ACCEPTED: // mat khau dc chap nhan
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:密码通过");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case HT_CN:
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:<color=yellow>功能完善中C！<color>");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case KD_VP:
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:材料不足！");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case CN_GH:
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:<color=yellow>锁频情况下，不能使用界面！请解锁！<color>");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;
		case HAVE_SKILL:
			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:<color=yellow>快捷栏已有同类技能!<color>");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}
			break;	
		case MO_AUTO:
			{ 

				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:功能完善中A");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/

			}
			break;
		case TAT_AUTO:

			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:功能完善中B");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}

			break;
		case CESHI_AUTO:

			{
				/*KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "提示:自动挂机测试开始！");
				Msg.nMsgLen=TEncodeText(Msg.szMessage, strlen(Msg.szMessage));
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);*/
			}

			break;
		case ERORR_AUTO:
			{
			}
			break;
		case EX_BOX:
			{	
				int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				if(nExbox == 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:扩展箱功能还没有开启,请到巴陵县(沈九)处开启!");
				}
				//else 
				//CoreDataChanged(GDCNI_OPEN_EX_BOX, nExbox, NULL);


			}
			break;
		case EX_BOX2:
			{		
				int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				if(nExbox == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:扩展箱2还没有开启!");
				}
				//else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
				//CoreDataChanged(GDCNI_OPEN_EX_BOX2, nExbox, NULL);

			}
			break;
		case EX_BOX3:
			{	

				int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				if(nExbox == 2 || nExbox == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:扩展箱3还没有开启!");
				}
				//else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
				//CoreDataChanged(GDCNI_OPEN_EX_BOX3, nExbox, NULL);


			}
			break;
		case ITEMEX:
			{	
				int nItemEX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;

				if(nItemEX == 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:子母袋还未开启,请咨询80沈九!");
				}
				//else if(nItemEX >= 1) 
				//CoreDataChanged(GDCNI_OPEN_ITEMEX, nItemEX, NULL);//打开子母袋功能
			}
			break;
		case GET_BOX_OPEN:
			{
				nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
			}
			break;
		case GET_BOXEX_OPEN:
			{
				nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;
			}
			break;
		case OPEN_MARKET://打开奇珍阁
			if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == 0)
			{
				break;
			}
			else
			{	
				nRet=0;
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("商城需要在安全区才可以打开!");
			}
			break;
		}
	}
	break;
default:
	break;
 }
 return nRet;
}

int KCoreShell::UseSkill(int x, int y, int nSkillID,int m_Kind)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return 0;
	}

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].m_randmove.nTime>0)                          //混乱状态不能发招
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
		return 0;
	}
	if (Npc[nIndex].IsCanInput()) //人物AI开启的话
	{
		int nIdx = 0;
		
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		if  (!Npc[nIndex].SetActiveSkill(nIdx))
			return 0;
		/*int nDesX,nDesY,nDmap;
		Npc[nIndex].GetMpsPos(&nDesX, &nDesY,&nDmap);
		Npc[nIndex].SendSerCommand(do_walk, nDesX, nDesY);
		SendClientCmdWalk(nDesX, nDesY);*/
	}
	else
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		//CCMessageBox("开始攻击","test");
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		//int nAttackRange = pISkill->GetAttackRadius();

		int nTargetIdx = 0;
		//按照Object / Enemy / Ally 的优先级找到需要打的对象id
		
		/*if (pISkill->IsTargetAlly())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_ally);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}*/
		char nInfo[128];
		if (m_Kind!=-1 && pISkill->IsTargetEnemy())
		{//攻击技能 和 
			/*Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_enemy);
			if (Player[CLIENT_PLAYER_I NDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}*/
			//t_sprintf(nInfo,"成功-敌人-%s",pISkill->GetSkillName());
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nInfo);
			MainAttack(m_Kind);
			return true;
		}

		/*if (pISkill->IsTargetObj())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetObj())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
			}
		}

		if (pISkill->IsTargetOther())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_dialog);

			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}
        
		//如果技能必须指定对象，而当前位置无对象的话，直接退出
		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		
		if (nIndex == nTargetIdx)
		{//等于自己
			//CCMessageBox("不成功-自己","skill");
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("不成功-自己");
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}*/
//====================客户端检测是否能发技能==============================
		//if (!pISkill->IsTargetEnemy())
		/*{
		   if ((!Npc[nIndex].m_SkillList.CanCastByIndex(Npc[nIndex].m_ActiveSkListIndex, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			   ||(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE)))
		   {
			//CCMessageBox("不成功-时间","skill");
			  Npc[nIndex].m_nPeopleIdx = 0;
			  Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			  return 0;
		   }
		}*/
//=========================================================================
		Npc[nIndex].m_nPeopleIdx = 0;
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
		//t_sprintf(nInfo,"成功-自己-%s",pISkill->GetSkillName());
		//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nInfo);
		//Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nIndex);
		//SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nIndex].m_dwID);
		if (pISkill->IsTargetSelf())
		{
			Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nIndex);
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nIndex].m_dwID);
		}
		else
		{
			Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);	
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}

		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
		
		//无对象，直接发坐标
		/*if (!nTargetIdx)
		{
			//CCMessageBox("不成功-没对象","skill");
			if (pISkill->IsTargetSelf())
			{
				Npc[nIndex].m_nPeopleIdx = 0;
				Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("成功-自己");
				Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nIndex);
				SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nIndex].m_dwID);
			}
			else
			{
				Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);	
				SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
			}

		//	return 0; //  无对象 直接退出
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					//CCMessageBox("不成功-范围","skill");
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}
			// 自己 并使子弹技能
			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles)
			{
				//CCMessageBox("不成功-自己子弹","skill");
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("不成功-自己-子弹");
				return 0;
			}
			//CCMessageBox("不成功-正常","skill");
			Npc[nIndex].SendSerCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}*/
	}
	//阻断攻击
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
	
}

void KCoreShell::setSand()
{
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Npc[nIndex].SendSerCommand(do_stand); 
}

void KCoreShell::setNpcDir(int x, int y)
{
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].setNpcDir(x,y);
}

void KCoreShell::GotoWhere(int x, int y, int mode) //鼠标点击的坐标
{
//乱状态不能走动
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_randmove.nTime>0)                          //混乱状态不能发招
		return;

	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) || mode == 2)
			bRun = true;
         
		int nX = x;
		int nY = y;
		int nZ = 0;

		SubWorld[0].GetMpsByLocalPosition(nX, nY, nZ);  //将屏幕坐标 换成 转成像素坐标

		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendSerCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			char Debugmsg[200];
			//t_sprintf(Debugmsg,"Toa do 4: nXpos:%d-nYpos:%d-nIndex:%d",nX,nY,nIndex);
			//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(Debugmsg);
			Npc[nIndex].SendSerCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}

		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
//---------------------------------------------------------------
 if (Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsJiaoBen==1)
 {//录制脚本
     //KTabFileCtrl nMapPos;	 
     char nName[32]={0},nFiledPath[128]={0},nTitleName[128]={0};
     int  nRows=0,nCols;
	 int nMapID,ncX,ncY;
	 char nMapName[64],nType[32];
		g_ScenePlace.GetSceneNameAndFocus(nMapName,nMapID,ncX,ncY,nType); //得到当前场景的格子坐标
    // t_sprintf(nFiledPath,"FsAi\\%s_Pos.txt",Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	 t_sprintf(nFiledPath,"jx50ai/%u_Pos.txt",g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
    /* if (!nMapPos.Load(nFiledPath))
	 {
	   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("录制脚本有误,请联系GM处理!");
	   nMapPos.Clear();
	   return;
	 }*/
     nRows=Player[CLIENT_PLAYER_INDEX].nMapPos.GetHeight();
     Player[CLIENT_PLAYER_INDEX].nMapPos.InsertAfter(nRows);
     Player[CLIENT_PLAYER_INDEX].nMapPos.WriteInteger(nRows+1,1,nMapID);//Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex);
     Player[CLIENT_PLAYER_INDEX].nMapPos.WriteInteger(nRows+1,2,nX);
     Player[CLIENT_PLAYER_INDEX].nMapPos.WriteInteger(nRows+1,3,nY);
    // nMapPos.WriteInteger(nRows+1,4,g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));
     //Player[CLIENT_PLAYER_INDEX].nMapPos.Save(nFiledPath);
     //nMapPos.Clear();

     Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("录制路线中,取消选择<录制路线>即可停止..");
   }
   }
}


////////////////////////////////频道消息到达/////////////////////////////////////

void KClientCallback::ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc,DWORD nItemDwidx)
{

}
//私聊
void KClientCallback::MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, bool bSucc,DWORD nItemDwidx)
{
}

void KClientCallback::CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam,int inVal)
{
	CoreDataChangedCallback(uDataId,uParam,nParam,inVal);
}
