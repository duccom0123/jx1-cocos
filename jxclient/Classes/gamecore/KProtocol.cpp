#include "KCore.h"
#include "CoreShell.h"
#include "engine/KEngine.h"
#include "engine/Text.h"
#include "KPlayer.h"
#include "KItemList.h"
////////后来加的///////
#include "GameDataDef.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KLadder.h"

//////////////////////
int	g_nProtocolSize[MAX_PROTOCOL_NUM] = 
{
    // 客户端接收到的服务器到客户端的协议长度
	-1,							// s2c_login,
	-1,							// s2c_logout,
	sizeof(CLIENT_SYNC_END),	// s2c_syncclientend,
	sizeof(CURPLAYER_SYNC),		// s2c_synccurplayer,
	-1,							// s2c_synccurplayerskill
	sizeof(CURPLAYER_NORMAL_SYNC),// s2c_synccurplayernormal
	-1,							// s2c_newplayer,
	-1,							// s2c_removeplayer,
	sizeof(WORLD_SYNC),			// s2c_syncworld,
	sizeof(PLAYER_SYNC),		// s2c_syncplayer,
	sizeof(PLAYER_NORMAL_SYNC),	// s2c_syncplayermin,
	-1,	//sizeof(NPC_SYNC),		// s2c_syncnpc,
	sizeof(NPC_NORMAL_SYNC),	// s2c_syncnpcmin,
	sizeof(NPC_PLAYER_TYPE_NORMAL_SYNC),	// s2c_syncnpcminplayer,
	-1,//sizeof(OBJ_ADD_SYNC),	    // s2c_objadd,-1
	sizeof(OBJ_SYNC_STATE),		// s2c_syncobjstate,
	sizeof(OBJ_SYNC_DIR),		// s2c_syncobjdir,
	sizeof(OBJ_SYNC_REMOVE),	// s2c_objremove,
	sizeof(OBJ_SYNC_TRAP_ACT),	// s2c_objTrapAct,
	sizeof(NPC_REMOVE_SYNC),	// s2c_npcremove,
	sizeof(NPC_WALK_SYNC),		// s2c_npcwalk,
	sizeof(NPC_RUN_SYNC),		// s2c_npcrun,
	-1,							// s2c_npcattack,
	-1,							// s2c_npcmagic,
	sizeof(NPC_JUMP_SYNC),		// s2c_npcjump,
	sizeof(NPC_CHAT_SYNC),		// s2c_npctalk,
	sizeof(NPC_HURT_SYNC),		// s2c_npchurt,
	sizeof(NPC_DEATH_SYNC),		// s2c_npcdeath,
	sizeof(NPC_CHGCURCAMP_SYNC),// s2c_npcchgcurcamp,
	sizeof(NPC_CHGCAMP_SYNC),	// s2c_npcchgcamp,
	sizeof(NPC_SKILL_SYNC),		// s2c_skillcast,
	-1,							// s2c_playertalk,
	sizeof(PLAYER_EXP_SYNC),	// s2c_playerexp,
	sizeof(PLAYER_SEND_TEAM_INFO),			// s2c_teaminfo,
	sizeof(PLAYER_SEND_SELF_TEAM_INFO),		// s2c_teamselfinfo,
	sizeof(PLAYER_APPLY_TEAM_INFO_FALSE),	// s2c_teamapplyinfofalse,
	sizeof(PLAYER_SEND_CREATE_TEAM_SUCCESS),// s2c_teamcreatesuccess,
	sizeof(PLAYER_SEND_CREATE_TEAM_FALSE),	// s2c_teamcreatefalse,
	sizeof(PLAYER_TEAM_OPEN_CLOSE),			// s2c_teamopenclose,
	sizeof(PLAYER_APPLY_ADD_TEAM),			// s2c_teamgetapply,
	sizeof(PLAYER_TEAM_ADD_MEMBER),			// s2c_teamaddmember,
	sizeof(PLAYER_LEAVE_TEAM),				// s2c_teamleave,
	sizeof(PLAYER_TEAM_CHANGE_CAPTAIN),		// s2c_teamchangecaptain,
	sizeof(PLAYER_FACTION_DATA),			// s2c_playerfactiondata,
	sizeof(PLAYER_LEAVE_FACTION),			// s2c_playerleavefaction,
	sizeof(PLAYER_FACTION_SKILL_LEVEL),		// s2c_playerfactionskilllevel,
	-1,//sizeof(PLAYER_SEND_CHAT_SYNC),			// s2c_playersendchat,
	sizeof(PLAYER_LEAD_EXP_SYNC),			// s2c_playersyncleadexp
	sizeof(PLAYER_LEVEL_UP_SYNC),			// s2c_playerlevelup
	sizeof(PLAYER_TEAMMATE_LEVEL_SYNC),		// s2c_teammatelevel
	sizeof(PLAYER_ATTRIBUTE_SYNC),			// s2c_playersyncattribute
	sizeof(PLAYER_SKILL_LEVEL_SYNC),		// s2c_playerskilllevel
	sizeof(ITEM_SYNC),						// s2c_syncitem
	sizeof(ITEM_REMOVE_SYNC),				// s2c_removeitem
	sizeof(PLAYER_FS_MONEY_SYNC),				// s2c_syncmoney
	sizeof(PLAYER_FS_XU_SYNC),					// s2c_syncxu
	sizeof(PLAYER_MOVE_ITEM_SYNC),			// s2c_playermoveitem
	-1,										// s2c_playershowui
	sizeof(CHAT_APPLY_ADD_FRIEND_SYNC),		// s2c_chatapplyaddfriend
	sizeof(CHAT_ADD_FRIEND_SYNC),			// s2c_chataddfriend
	-1,//sizeof(CHAT_REFUSE_FRIEND_SYNC),		// s2c_chatrefusefriend
	sizeof(CHAT_ADD_FRIEND_FAIL_SYNC),		// s2c_chataddfriendfail
	sizeof(CHAT_LOGIN_FRIEND_NONAME_SYNC),	// s2c_chatloginfriendnoname
	-1,//sizeof(CHAT_LOGIN_FRIEND_NAME_SYNC),	// s2c_chatloginfriendname
	sizeof(CHAT_ONE_FRIEND_DATA_SYNC),		// s2c_chatonefrienddata
	sizeof(CHAT_FRIEND_ONLINE_SYNC),		// s2c_chatfriendinline
	sizeof(CHAT_DELETE_FRIEND_SYNC),		// s2c_chatdeletefriend
	sizeof(CHAT_FRIEND_OFFLINE_SYNC),		// s2c_chatfriendoffline
	sizeof(ROLE_LIST_SYNC),					// s2c_syncrolelist
	sizeof(TRADE_CHANGE_STATE_SYNC),		// s2c_tradechangestate
	-1, // NPC_SET_MENU_STATE_SYNC			   s2c_npcsetmenustate
	sizeof(TRADE_MONEY_SYNC),				// s2c_trademoneysync
	sizeof(TRADE_DECISION_SYNC),			// s2c_tradedecision
	-1, // sizeof(CHAT_SCREENSINGLE_ERROR_SYNC)s2c_chatscreensingleerror
	sizeof(NPC_SYNC_STATEINFO),				// s2c_syncnpcstate,
	-1,	// sizeof(TEAM_INVITE_ADD_SYNC)		   s2c_teaminviteadd
	sizeof(TRADE_STATE_SYNC),				// s2c_tradepressoksync
	sizeof(PING_COMMAND),					// s2c_ping
	sizeof(NPC_SIT_SYNC),					// s2c_npcsit
	sizeof(SALE_BOX_SYNC),					// s2c_opensalebox
	sizeof(NPC_SKILL_SYNC),					// s2cDirectlyCastSkill
	-1,										// s2c_msgshow
	-1,										// s2c_syncstateeffect
	sizeof(BYTE),							// s2c_openstorebox
	sizeof(RESET_PASS),						// s2c_openresetpass
	sizeof(PLAYER_STRING),					// s2c_stringboxSTRING_BOX
	sizeof(DATAU_BOX),						// s2c_opendataubox
	sizeof(NPC_REVIVE_SYNC),				// s2c_playerrevive
	sizeof(NPC_REQUEST_FAIL),				// s2c_requestnpcfail
	sizeof(TRADE_APPLY_START_SYNC),			// s2c_tradeapplystart
	sizeof(tagNewDelRoleResponse),			// s2c_rolenewdelresponse
	sizeof(ITEM_AUTO_MOVE_SYNC),			// s2c_ItemAutoMove
	sizeof(BYTE),							// s2c_itemexchangefinish
	sizeof(SYNC_WEATHER),					// s2c_changeweather
	sizeof(PK_NORMAL_FLAG_SYNC),			// s2c_pksyncnormalflag
	-1,//sizeof(PK_ENMITY_STATE_SYNC),		// s2c_pksyncenmitystate
	-1,//sizeof(PK_EXERCISE_STATE_SYNC),	// s2c_pksyncexercisestate
	sizeof(PK_VALUE_SYNC),					// s2c_pksyncpkvalue
	sizeof(REPUTE_VALUE_SYNC),				// s2c_reputesyncreputevalue
	sizeof(FUYUAN_VALUE_SYNC),				// s2c_fuyuansyncfuyuanvalue
	sizeof(REBORN_VALUE_SYNC),				// s2c_rebornsyncrebornvalue
	sizeof(SALE_MARKET_SYNC),				// s2c_openmarketbox
	sizeof(NPC_SLEEP_SYNC),					// s2c_npcsleepmode
	-1,//sizeof(VIEW_EQUIP_SYNC),				// s2c_viewequip  -1,//
	sizeof(LADDER_DATA),					// s2c_ladderresult
	-1,										// s2c_ladderlist
	sizeof(TONG_CREATE_SYNC),				// s2c_tongcreate
	sizeof(PING_COMMAND),					// s2c_replyclientping
	sizeof(NPC_GOLD_CHANGE_SYNC),			// s2c_npcgoldchange
	sizeof(ITEM_DURABILITY_CHANGE),			// s2c_itemdurabilitychange
	sizeof(BYTE),							// s2c_opentremble  //打造紫装 图谱黄金
	sizeof(RANKFF_SYNC),					// s2c_rankname
	sizeof(VIEW_ITEM_SYNC),					// s2c_viewsellitem
	sizeof(VIEW_ITEM_SYNC),					// s2c_viewupdateitem
	sizeof(PLAYER_GET_COUNT),				// s2c_playergetcount
	sizeof(PLAYER_PLAYER_SHOPNAME),			// s2c_shopname  //	sizeof(CURPLAYER_TASKINFO_SYNC),		//s2c_synccurplayerinfo
	sizeof(NPC_SIT_SYNC),
	sizeof(PLAYER_GIVE),				    // s2c_opengive
	sizeof(NPC_JUMP_SYNC),
	sizeof(PKVALUE),						// s2c_pkvalue
	sizeof(VIEW_LIAN_ITEM_SYNC),			// s2c_viewlianitem
	sizeof(BYTE),							// s2c_openwengang   熔炼纹钢
	sizeof(BYTE),                           // s2c_openstone     宝石镶嵌
	sizeof(VIEW_ITEM_SYNC),					// s2c_viewupdateitem
	sizeof(PLAYER_TONGINFO_CALLBACK),	
	sizeof(ITEM_SYNC),	                    //
	sizeof(PLAYER_GETBACK_TASKVAL),         // s2c_taskcallback
	sizeof(PLAYER_OPEN_JINDUTIAO),          // s2c_openjindutiao
	-1,//sizeof(ITEM_SYNC_POSITIONS), 
	sizeof(PLAYER_DUANZHAO),
	sizeof(BYTE),                           // s2c_syncurupdata 同步当前数据	  
	sizeof(PLAYER_JINMAI_GETBACK), 
	sizeof(PLAYER_VIPDAZAO),
	sizeof(NPC_SYN_INFO_GETBACK),
	sizeof(PLAYER_AUTOCALLBACK_EQUIP),
	sizeof(PLAYER_SYNCALLBACK_GUZI),
	sizeof(PLAYER_NORMALMAP_SYNC),
	sizeof(PLAYER_ONESTATE_SYNC),
	sizeof(STATE_NODATA_EFFECT_SYNC),
	sizeof(NPC_PLAYER_TYPE_NORMAL_SYNC),
	sizeof(PLAYER_MISSION_DATA),			// s2c_playermissiondata,
	sizeof(PLAYER_MISSION_RANKDATA),		// s2c_syncrankdata
	sizeof(S2C_SUPERSHOP),
	sizeof(ITEM_SYNC_SHOP),
	sizeof(ITEMDATA_SYNC_END),
	sizeof(KICK_OUT_GAME_COMMAND),
	sizeof(OBJ_ADD_SYNC_VN),
	sizeof(ROLEVIP_VALUE_SYNC),
};

void g_InitProtocol()
{//250-65=185 186 187 188
	g_nProtocolSize[s2c_extend - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendchat - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendfriend - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendtong - s2c_clientbegin - 1] = -1;
}

void SendClientCmdRun(int nX, int nY,int nMapID)
{
	NPC_RUN_COMMAND	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)c2s_npcrun;
	NetCommand.nMpsX  = nX;
	NetCommand.nMpsY  = nY;
	NetCommand.nMapID = nMapID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NetCommand));
}

void SendClientCmdSunyi(int nMaps,int nX, int nY,int nNpcID,int nModel) //瞬移
{
	
	NPC_SUNYI_COMMAND	SunYICommand;
	
	SunYICommand.ProtocolType = (BYTE)c2s_npcsunyi;
	SunYICommand.nMapID= nModel;
    SunYICommand.nMap  = nMaps;
	SunYICommand.nMpsX = nX;
	SunYICommand.nMpsY = nY;
	SunYICommand.nDwid = nNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SunYICommand, sizeof(SunYICommand));
		
}

void SendClientCmdWalk(int nX, int nY)
{
	NPC_WALK_COMMAND	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)c2s_npcwalk;
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NetCommand));
}

void SendClientRunScript(int nKind,int nVala, int nValb, int nValc)
{
	CLIENT_RUN_SCRIPT	NetCommand;
	NetCommand.ProtocolType = (BYTE)c2s_runscript;
	NetCommand.nKind = nKind;
	NetCommand.nVala = nVala;
	NetCommand.nValb = nValb;
	NetCommand.nValc = nValc;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(CLIENT_RUN_SCRIPT));
}
//发送给服务器
void SendClientCmdSkill(int nSkillID, int nX, int nY)
{
	NPC_SKILL_COMMAND	NetCommand;
	/*
	int ParamX = pNetCommand->nSkillID;
	int ParamY = pNetCommand->nMpsX;
	int ParamZ = pNetCommand->nMpsY;
	*/
	NetCommand.ProtocolType = (BYTE)c2s_npcskill;
	NetCommand.nSkillID = nSkillID;
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(NPC_SKILL_COMMAND));	
}
//客户端要求同步这个NPC的数据
void SendClientCmdRequestNpc(int nID)
{
	NPC_REQUEST_COMMAND NpcRequest;
	
	NpcRequest.ProtocolType = c2s_requestnpc;
	NpcRequest.ID = nID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&NpcRequest, sizeof(NPC_REQUEST_COMMAND));

}

void SendClientCmdSell(int nId)
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:您网络有延时,请稍等1!");
		return;
	}

	PLAYER_SELL_ITEM_COMMAND PlayerSell;
	PlayerSell.ProtocolType = c2s_playersellitem;
	PlayerSell.m_ID = nId;
	PlayerSell.m_ClinetKind = 1;//mobile端
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerSell, sizeof(PLAYER_SELL_ITEM_COMMAND));

	//Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}

void SendClientCmdBuy(int nBuyIdx, int nPlace, int nX, int nY,int nShuiShou,int nIsCityMenber,int nMapID,int nItemNum)
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:您网络有延时,请稍等2!");
		return;
	}

	PLAYER_BUY_ITEM_COMMAND PlayerBuy;
	PlayerBuy.ProtocolType = c2s_playerbuyitem;
	PlayerBuy.m_BuyIdx = (BYTE)nBuyIdx;
	PlayerBuy.m_Place  = (BYTE)nPlace;//买进的容器
	PlayerBuy.m_X      = (BYTE)nX;    //包袱的位置
	PlayerBuy.m_Y      = (BYTE)nY;    //包袱的位置
	PlayerBuy.m_shuishou  =nShuiShou;
	PlayerBuy.m_CityMenber=nIsCityMenber;
    PlayerBuy.m_MapID     =nMapID;
   PlayerBuy.m_ItemNum    =nItemNum;
	if (g_pClient)
    	g_pClient->SendPackToServer((BYTE*)&PlayerBuy, sizeof(PLAYER_BUY_ITEM_COMMAND));

	//Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();

}

//void SendClientCmdPing()
//{
//	PING_COMMAND PingCmd;
//
//	PingCmd.ProtocolType = c2s_ping;
//	PingCmd.m_dwTime = GetTickCount();
//	if (g_pClient && g_bPingReply)
//	{
//		g_pClient->SendPackToServer((BYTE*)&PingCmd, sizeof(PING_COMMAND));
//		g_bPingReply = FALSE;
//	}
//}

void SendClientCPUnlockCmd(int CP_IntPW)
{
	PLAYER_REQUEST_CP_UNLOCK UlockCmd;

	UlockCmd.ProtocolType = c2s_cpunlock;
	UlockCmd.int_PW = CP_IntPW;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&UlockCmd, sizeof(PLAYER_REQUEST_CP_UNLOCK));
}

void SendClientCPLockCmd()
{
	PLAYER_REQUEST_CP_LOCK LockCmd;

	LockCmd.ProtocolType = c2s_cplock;
	LockCmd._isMobile =1; //是否手机端
	LockCmd._clientVer=_clientlanguage;//客户端版本 1 中国 2越南 其他 英语
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&LockCmd, sizeof(PLAYER_REQUEST_CP_LOCK));
}

void SendClientCPResetCmd(int resetPW)
{
	PLAYER_REQUEST_CP_RESET ResetPWCmd;

	ResetPWCmd.ProtocolType = c2s_cpreset;
	ResetPWCmd.int_ResetPW = resetPW;
	if (g_pClient)
    	g_pClient->SendPackToServer((BYTE*)&ResetPWCmd, sizeof(PLAYER_REQUEST_CP_RESET));
}

void SendClientCPChangeCmd(int oldPW, int newPW)
{
	PLAYER_REQUEST_CP_CHANGE ChangePWCmd;

	ChangePWCmd.ProtocolType = c2s_cpchange;
	ChangePWCmd.int_OldPW = oldPW;
	ChangePWCmd.int_NewPW = newPW;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ChangePWCmd, sizeof(PLAYER_REQUEST_CP_CHANGE));
}

void SendClientOpenMarket(int nShopid,int iShopType)
{
	PLAYER_REQUEST_OPEN_MARKET Market;
    Market.ProtocolType = c2s_market;
	Market.nShopType    = iShopType;
    Market.nShopID      = nShopid;

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&Market, sizeof(PLAYER_REQUEST_OPEN_MARKET));
}
//经脉
void SendClientCmdJinMai(int nType,int nLevel)
{
	CP_JINGMAI JinMaiCmd;
	
	JinMaiCmd.ProtocolType =(BYTE)c2s_fsjingmai;
	JinMaiCmd.ntype = nType;
    JinMaiCmd.nlevel =  nLevel;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&JinMaiCmd, sizeof(CP_JINGMAI));
		
}

void SendClientAutoPlay(int nstt,int cishu,int nIsOpen) //stringbox
{
	CP_AUTOPLAY AutoPlayCmd;

	memset(&AutoPlayCmd,0,sizeof(AutoPlayCmd));

	AutoPlayCmd.ProtocolType = c2s_autoplay;

	if 	(cishu==-1)
	{ //挂机专用
		int nBackNpcIndex = Player[CLIENT_PLAYER_INDEX].m_Autoplay.nBackIndex;

		if  (nBackNpcIndex>0 && nBackNpcIndex<MAX_NPC)
		{
			Npc[nBackNpcIndex].m_IsbeSel = 0;
		}

		Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsOpenPiick = nIsOpen;

		if (nstt==1)
		{//开启挂机
			
			Player[CLIENT_PLAYER_INDEX].ApplyRoomInfo(0);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nIsOver=FALSE;
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nSkilloop=0;
			Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsUseSkill =FALSE;

			Player[CLIENT_PLAYER_INDEX].nYaBiao.Clear();
			Player[CLIENT_PLAYER_INDEX].nPos.Clear();
			Player[CLIENT_PLAYER_INDEX].nEquipItem.Clear();
			nToolItem.Clear();

			Player[CLIENT_PLAYER_INDEX].m_FileDesc.Clear();
			Player[CLIENT_PLAYER_INDEX].nCheckName.Clear();
			Player[CLIENT_PLAYER_INDEX].nGuaSkill.Clear();


			if(!Player[CLIENT_PLAYER_INDEX].nYaBiao.Load("\\settings\\mappos\\biao_pos.txt"))
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Cài đặt mục tiêu không chính xác!");
			   return;
			}

			if(!Player[CLIENT_PLAYER_INDEX].nGuaSkill.Load("\\settings\\Ai\\skill.txt"))
			{
			   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Sử dụng sai kỹ năng");
			   return;
			}

			if (!Player[CLIENT_PLAYER_INDEX].nCheckName.Load("jx50ai/itemUseName.txt"))
			{
			   Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Cặt đặt nhặt đồ không thành công, vui lòng lưu cấu hình sau khi cài đặt!");
			   return;
			}
			
			char nFiledPath[128]={0};
			t_sprintf(nFiledPath,"jx50ai/%u_Pos.txt",g_FileName2Id(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name));

			//KIniFile	m_FileDesc;
			 if (!Player[CLIENT_PLAYER_INDEX].m_FileDesc.Load("\\settings\\MagicDesc.Ini"))
			 {  
				 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Dữ liệu không chính xác, vui lòng liên hệ GM để xử lý!");
				 return;												   
			 } 

			 if(Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoModel==1 && !Player[CLIENT_PLAYER_INDEX].nPos.Load(nFiledPath))
			 {
				 Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Lộ trình bị sai, vui lòng ghi lại lộ trình!");
				 return;
			 }

		     if(!Player[CLIENT_PLAYER_INDEX].nEquipItem.Load("jx50ai/equipName.txt"))  //装备属性列表
			 {
			    Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Cài đặt trang bị không thành công, vui lòng lưu cấu hình sau khi cài đặt!");
			    return;
			 }
				  
             if(!nToolItem.Load("jx50ai/itemName.txt"))  //道具
			 {
		         Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Cài đặt vật phẩm đạo cụ không thành công, vui lòng lưu lại cấu hình sau khi cài đặt!");
			     return;
			 }
		}

		if (nstt==0)
		{//关闭挂机
			Player[CLIENT_PLAYER_INDEX].ApplyRoomInfo(0);
			//Player[CLIENT_PLAYER_INDEX].nPos.Clear();
		}
		//AutoPlayCmd.ByteVal=Player[CLIENT_PLAYER_INDEX].m_cTask.SetTaskByte(AutoPlayCmd.ByteVal,1,nstt);
		AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,1,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nIsNoBlue);
		AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,2,nIsOpen);
		AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoney);//钱
		AutoPlayCmd.ByteVal=ESetByte(AutoPlayCmd.ByteVal,4,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoEques);//装备

		AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,1,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoMoneyAndEques);//钱和装备
		AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,2,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTuPu);//图谱
		AutoPlayCmd.ByteVala=ESetByte(AutoPlayCmd.ByteVala,3,Player[CLIENT_PLAYER_INDEX].m_Autoplay.nAutoTool);//道具
	}
	
	AutoPlayCmd.nbutton = nstt;
    AutoPlayCmd.ncishu  = cishu;
	//AutoPlayCmd.nVal    = nIsOpen;//如果是挂机是否开启了 捡取开关
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&AutoPlayCmd, sizeof(CP_AUTOPLAY));
}
//查看链接信息
void SendClientLianJie(char*nPLname,int nItemDWid) //链接
{
    VIEW_LIANJIE_COMMAND	sLView;
	sLView.ProtocolType = c2s_lianjie;
	t_sprintf(sLView.m_PLname,nPLname);//发送连接的玩家姓名
    sLView.m_ItemDwid=nItemDWid;       //被查看装备的 dwid
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&sLView, sizeof(VIEW_LIANJIE_COMMAND));
	
}

void SendClientGuZi(int nMapID,int dianshu,int nNetIndexid,int ZuoWeiHao)  //股子
{
	CP_GUZI GuZiCmd;
	
	GuZiCmd.ProtocolType   = c2s_guzi;
	GuZiCmd.nmapid         = nMapID;
    GuZiCmd.ndianshu       = dianshu;
	GuZiCmd.nNetConnectIdx = nNetIndexid;
	GuZiCmd.nZuoWeiHao     = ZuoWeiHao;
//	if (g_pClient)
	//	g_pClient->SendPackToServer((BYTE*)&GuZiCmd, sizeof(CP_GUZI));
}

void SendClientGuZiFarms(int nMapID,int nFarmA,int nFarmB)  //股子
{
	CP_GUZI_FARMS GuZiFarmsCmd;
	
	GuZiFarmsCmd.ProtocolType = c2s_gufarm;
	GuZiFarmsCmd.nmapid  = nMapID;
    GuZiFarmsCmd.nFarmsA = nFarmA;
    GuZiFarmsCmd.nFarmsB = nFarmB;
//	if (g_pClient)
	//	g_pClient->SendPackToServer((BYTE*)&GuZiFarmsCmd, sizeof(CP_GUZI_FARMS));
}

void SendClientAddJinBi(int nJinBi)  //股子
{
	CP_ADD_JINBI AddJinBiCmd;
	
	AddJinBiCmd.ProtocolType = c2s_addjinbi;
	AddJinBiCmd.nJinBi  = nJinBi;
//	if (g_pClient)
	//	g_pClient->SendPackToServer((BYTE*)&AddJinBiCmd, sizeof(CP_ADD_JINBI));
}

void SendClientDaTau(int nstt,int nXuHao)
{
	CP_DATAU ButtonCmd;

	ButtonCmd.ProtocolType = c2s_datau;
	ButtonCmd.nbutton = nstt;
	ButtonCmd.nXuHao  = nXuHao;
	if (g_pClient)
    	g_pClient->SendPackToServer((BYTE*)&ButtonCmd, sizeof(CP_DATAU));
}


void SendClientCmdSit(int nSitFlag)
{
	NPC_SIT_COMMAND SitCmd;

	SitCmd.ProtocolType = c2s_npcsit;
	SitCmd.m_btSitFlag = (nSitFlag != 0);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SitCmd, sizeof(NPC_SIT_COMMAND));
}

void SendClientCmdRide()
{
	NPC_RIDE_COMMAND SitCmd;
	
	SitCmd.ProtocolType = c2s_npcride;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SitCmd, sizeof(NPC_RIDE_COMMAND));
}

void SendClientCmdJump(int nMpsX, int nMpsY)
{
	NPC_JUMP_COMMAND JumpCmd;

	JumpCmd.nMpsX = nMpsX;
	JumpCmd.nMpsY = nMpsY;
	JumpCmd.ProtocolType = c2s_npcjump;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&JumpCmd, sizeof(NPC_JUMP_COMMAND));
}

void SendObjMouseClick(int nObjID, UINT dwRegionID)
{
	OBJ_MOUSE_CLICK_SYNC	sObj;
	sObj.ProtocolType = c2s_objmouseclick;
	sObj.m_dwRegionID = dwRegionID;
	sObj.m_nObjID = nObjID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&sObj, sizeof(OBJ_MOUSE_CLICK_SYNC));
}

void SendClientCmdStoreMoney(int nDir, int nMoney)
{
	STORE_MONEY_COMMAND	StoreMoneyCmd;

	StoreMoneyCmd.ProtocolType = c2s_storemoney;
	StoreMoneyCmd.m_byDir = (BYTE)nDir;
	StoreMoneyCmd.m_dwMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&StoreMoneyCmd, sizeof(STORE_MONEY_COMMAND));
}

void SendClientCmdRevive(int nReviveType)
{
	NPC_REVIVE_COMMAND	ReviveCmd;

	ReviveCmd.ProtocolType = c2s_playerrevive;
	ReviveCmd.ReviveType = nReviveType;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE *)&ReviveCmd, sizeof(NPC_REVIVE_COMMAND));
}

//封挂选择
void SendClientCmdGua(int nGuaType)
{
	NPC_GUAFORBIT_COMMAND	GuaCmd;
	GuaCmd.ProtocolType = c2s_guaforbit;
	GuaCmd.GuaType      = nGuaType;
//	if (g_pClient)
	//	g_pClient->SendPackToServer((BYTE *)&GuaCmd, sizeof(NPC_GUAFORBIT_COMMAND));
}

/*
void SendClientCmdRevive()
{
	NPC_REVIVE_COMMAND	ReviveCmd;

	ReviveCmd.ProtocolType = c2s_playerrevive;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE *)&ReviveCmd, sizeof(NPC_REVIVE_COMMAND));
}

*/
void SendClientCmdMoveItem(void* pDownPos, void* pUpPos,int nIsComp)
{
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("Hàm SendClientCmdMoveItem run ok!");
	if (!pDownPos || !pUpPos)
		return;
    //移动物品 如果是锁着的话 就返回
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	{
        Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("M?o: M?ng c?a b?n có ?? tr?, vui lòng ??i!");
		return;
	}
	ItemPos* pos1 = (ItemPos *)pDownPos;  //需要放进物品的目标容器
	ItemPos* pos2 = (ItemPos *)pUpPos;

	PLAYER_MOVE_ITEM_COMMAND	sMove;
	sMove.ProtocolType = c2s_playermoveitem;
	sMove.m_btDownPos  = pos1->nPlace;
	sMove.m_btDownX    = pos1->nX;
	sMove.m_btDownY    = pos1->nY;
    sMove.m_DownRongqi = pos1->nSIsRongqi;
	sMove.m_btUpPos    = pos2->nPlace;
	sMove.m_btUpX      = pos2->nX;
	sMove.m_btUpY      = pos2->nY;
	sMove.m_UpRongqi   = pos2->nDIsRongqi;
	sMove.m_IsComp     = nIsComp;

	if (g_pClient)
		g_pClient->SendPackToServer(&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));
	
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}

void SendClientCmdQueryLadder(UINT	dwLadderID)
{
	if (dwLadderID <= enumLadderBegin || dwLadderID >= enumLadderEnd)  //	0<	dwLadderID <38

		return;

	if (g_pClient)
	{
		LADDER_QUERY	LadderQuery;
		LadderQuery.ProtocolType = c2s_ladderquery;
		LadderQuery.dwLadderID = dwLadderID;
		if (g_pClient)
		   g_pClient->SendPackToServer(&LadderQuery, sizeof(LADDER_QUERY));
	}
	
}

//获取某类型数据
void SendClientCmdTpye(int nType)
{
	PLAYER_SYN_TYPE mType;
	mType.ProtocolType = c2s_playerguzgame;
	mType.nType = nType;
					
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&mType, sizeof(PLAYER_SYN_TYPE));
}

void SendClientSetPos(int mXpos,int mYpos)
{
	PLAYER_SYN_POS mPos;
	mPos.ProtocolType = c2s_playersetpos;
	mPos.nXpos = mXpos;
	mPos.nYpos = mYpos;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&mPos, sizeof(PLAYER_SYN_POS));

}

void SendClientCmdRepair(UINT dwID)
{
	ITEM_REPAIR ItemRepair;
	ItemRepair.ProtocolType = c2s_repairitem;
	ItemRepair.dwItemID = dwID;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ItemRepair, sizeof(ITEM_REPAIR));
}

void SendClientCmdBreak(UINT dwID, int nNum)
{
	ITEM_BREAK ItemBreak;
	ItemBreak.ProtocolType = c2s_breakitem;
	ItemBreak.dwItemID = dwID;
	ItemBreak.nNum     = nNum;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ItemBreak, sizeof(ITEM_BREAK));
}
// 给予界面回调函数 给予界面回调函数
void SendClientCmdGive( unsigned int uId, int nItemNum, int nKind, char* sCallback,char* sError,int nPrice)
{
	unsigned int* pInfo = (unsigned int*) uId;

	PLAYER_GIVE_CALLBACK PGC;
	PGC.ProtocolType = c2s_playergiveback;
	PGC.nKind        = (BYTE)nKind; //执行的种类
	PGC.nPrice       = nPrice;      //寄售价格
	ZeroMemory(PGC.m_uId,sizeof(PGC.m_uId));

	if  (nItemNum>24)
		 nItemNum =24;

	for (int i = 0;i < 24;i++) //nItemNum
	{//物品数量
		int nIdx = *pInfo;

		if (nIdx <= 0 /*&& nIdx >= 512*/)
		{
			PGC.m_uId[i] = 0;
			//return;
		}
		else
		   PGC.m_uId[i] = Item[nIdx].GetID();

		pInfo++;
	}

	ZeroMemory(PGC.m_Callback,sizeof(PGC.m_Callback));
	ZeroMemory(PGC.m_Error,sizeof(PGC.m_Error));

    t_sprintf(PGC.m_Callback,"%s",sCallback);
	t_sprintf(PGC.m_Error,"%s",sError);

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PGC, sizeof(PLAYER_GIVE_CALLBACK));
}
//字符串输入界面
void SendClientString(char* sSContent,char* sSCallback,char* sSError,int biaozhi)
{
	PLAYER_STRING_CALLBACK StringCmd;
	
	StringCmd.ProtocolType = c2s_string;
//	StringCmd.nbutton = nstt;
	ZeroMemory(StringCmd.m_SCallback,sizeof(StringCmd.m_SCallback));
	ZeroMemory(StringCmd.m_SError,sizeof(StringCmd.m_SError));
    ZeroMemory(StringCmd.m_Content,sizeof(StringCmd.m_Content));
#ifdef WIN32
	t_sprintf(StringCmd.m_Content,"%s",U2G(sSContent).c_str());
	t_sprintf(StringCmd.m_SCallback,"%s",U2G(sSCallback).c_str());
#else
	t_sprintf(StringCmd.m_Content,"%s",U2G(sSContent).c_str());
    t_sprintf(StringCmd.m_SCallback,"%s",U2G(sSCallback).c_str());
#endif
	t_sprintf(StringCmd.m_SError,"%s",sSError);
	StringCmd.m_nNum=biaozhi;

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&StringCmd, sizeof(PLAYER_STRING_CALLBACK));
/*测试
	KSystemMessage	sMsg;
	t_sprintf(sMsg.szMessage,"%s*%s*%s*",StringCmd.m_Content,StringCmd.m_SCallback,StringCmd.m_SError);
	sMsg.eType = SMT_SYSTEM;
	sMsg.byConfirmType = SMCT_CLICK;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
    CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
*/
}

void SendClientPKValue(int nstt) //pkvalue
{
	CP_PKVALUE PKValueCmd;

	PKValueCmd.ProtocolType = c2s_pkvalue;
	PKValueCmd.nbutton = nstt;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PKValueCmd, sizeof(CP_PKVALUE));
}
//熔炼纹钢
void SendClientCmdRong(unsigned int uId, int nItemNum, int nKind)
{
	unsigned int* pInfo = (unsigned int*) uId;
	PLAYER_RONGITEM PlayerRong;
	PlayerRong.ProtocolType = (BYTE)c2s_playerrong;
	ZeroMemory(PlayerRong.uId,sizeof(PlayerRong.uId));
	if (nKind < 4)
	{
		nItemNum = 3;
	}
	int temp[3];
	for (int i = 0;i < nItemNum;i++)
	{
		int nIdx = *pInfo;
		
		if (nIdx <= 0 /*&& nIdx >= 512*/)
		{
			return;
		}
		PlayerRong.uId[i] = Item[nIdx].GetID();
		temp[i]=nIdx;
		pInfo++;
	}
	if(temp[0] == temp[1] || temp[0] == temp[2] || temp[1] == temp[2] )
		return;
	PlayerRong.bKind = nKind; 
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerRong, sizeof(PLAYER_RONGITEM));
}

void SendClientDelItem(int nIndex)
{
	PLAYER_DELL_ITEM pItem;
	pItem.ProtocolType  = (BYTE)c2s_playerdelitem;
	//int nItemidx       = pos1->nIdx;
	pItem.uIiemIdx      = Item[nIndex].GetID();
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&pItem, sizeof(PLAYER_DELL_ITEM));
}

//客户端要求增加物品
void SendClientAddItem(void* pInfo)
{
	if (NULL==pInfo)
		return;
	
	ItemXinXi* pos1 = (ItemXinXi *)pInfo;

	PLAYER_ADD_ITEM pItem;
	pItem.ProtocolType = (BYTE)c2s_playeradditem;
	//pItem.uIiemId      = pos1->nIdx;
	pItem.nX		   = pos1->nX;//shopid
	pItem.nY		   = pos1->nY;
	pItem.nKind        = pos1->nIsRongqi; //执行的类型
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&pItem, sizeof(PLAYER_ADD_ITEM));
/*
	char nMsg[64]={0};
	t_sprintf(nMsg,"send suss:(%d,%d) info",pItem.nX,pItem.nY);
	Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nMsg);


	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 1;
	sMsg.byParamSize = 0;
	t_sprintf(sMsg.szMessage,"警告:数据出错(%d)!",pos1->nIdx);
	sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
*/

}
//更新帮会设置
void SendClientGetTongInfo(int nIndex,int nType,int nStartPos)
{
	PLAYER_TONGINFO nTongComp;
	memset(&nTongComp, 0, sizeof(nTongComp));
    nTongComp.ProtocolType  = (BYTE)c2s_playertongset;
    nTongComp.inAcceptLevel = Npc[nIndex].m_dwID;
	nTongComp.inType        = nType;
    if (nType==5)
	   nTongComp.inRefuseLevel = nStartPos;

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&nTongComp, sizeof(PLAYER_TONGINFO));

}
//更新公告
void SendClientGetTonggg(int nIndex)
{
	PLAYER_TONGINFO nTongComp;
	memset(&nTongComp, 0, sizeof(nTongComp));
    nTongComp.ProtocolType  = (BYTE)c2s_playertongset;
	nTongComp.inAcceptLevel = Npc[nIndex].m_dwID;
	nTongComp.inType        = 3;
	if (g_pClient)
    	g_pClient->SendPackToServer((BYTE*)&nTongComp, sizeof(PLAYER_TONGINFO));
}

//保存公告
void SendClientSetTonggg(KUiTongInfoWithName *nTong)
{
	PLAYER_TONGINFO nTongComp;
	memset(&nTongComp, 0, sizeof(nTongComp));
    nTongComp.ProtocolType = (BYTE)c2s_playertongset;
	nTongComp.inType=2;
	t_sprintf(nTongComp.szGongGao,nTong->szGongGao);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&nTongComp, sizeof(PLAYER_TONGINFO));
}
//通用执行脚本函数
void SendClientDoScipt(KUiPlyerDoscript *nDoscript)
{ 
	PLAYER_DOSCRIPT nScript;
	memset(&nScript, 0, sizeof(nScript));
    ZeroMemory(&nScript,sizeof(nScript));
    nScript.ProtocolType   = (BYTE)c2s_playerdoScript;
	nScript.inKind         = nDoscript->inKind;
	nScript.inCurTime      = nDoscript->inCurTime;
    nScript.inCurTimeb     = nDoscript->inCurTimeb;
	nScript.inCurTimec     = nDoscript->inCurTimec;
	nScript.inCurTimed     = nDoscript->inCurTimed;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&nScript, sizeof(PLAYER_DOSCRIPT));
	//char msg[64];
	//t_sprintf(msg,"提示:发送%d-%d成功。。。",nDoscript->inKind,nDoscript->inCurTime);
    //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(msg);
}

void SendClientSetTaskVal(int nTaskNo)
{

     PLAYER_GET_TASKVAL_COMMAND GetTaskval;
	    GetTaskval.ProtocolType = c2s_gettaskval;
        GetTaskval.m_TaskId=nTaskNo;
        GetTaskval.m_pDwid=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;	
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&GetTaskval, sizeof(PLAYER_GET_TASKVAL_COMMAND));
	      //Player[nPlayerIndex].m_ItemList.LockOperation();
}

void SendClientVipDoScipt(KUiPlyerVipDoscript *nVipDoscript)
{
   	PLAYER_VIP_DOSCRIPT nVipScript;
	memset(&nVipScript, 0, sizeof(nVipScript));
    ZeroMemory(&nVipScript,sizeof(nVipScript));
    nVipScript.ProtocolType   = (BYTE)c2s_vipdoScript;
      nVipScript.cnKind       = nVipDoscript->inKind;
	  nVipScript.cnCurJiHuo   = nVipDoscript->inCurJiHuo;
	  nVipScript.cnCurExp     = nVipDoscript->inCurExp;
	  nVipScript.cnCurLucky   = nVipDoscript->inCurLucky;
	  nVipScript.cnCurWuHang  = nVipDoscript->inCurWuHang;
	  t_sprintf(nVipScript.snCardAcc,nVipDoscript->snCardAcc);
	  t_sprintf(nVipScript.snCardPass,nVipDoscript->snCardPass);

	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&nVipScript, sizeof(PLAYER_VIP_DOSCRIPT));
}


//保存招募信息
void SendClientSetTongInfo(KUiTongInfoWithName *nTong)
{
   PLAYER_TONGINFO nTongComp;
   memset(&nTongComp, 0, sizeof(nTongComp));
   nTongComp.ProtocolType = (BYTE)c2s_playertongset;
  
   t_sprintf(nTongComp.szQingXiang,nTong->szQingXiang);
   t_sprintf(nTongComp.szHuoDong1,nTong->szHuoDong1);
   t_sprintf(nTongComp.szHuoDong2,nTong->szHuoDong2);
   t_sprintf(nTongComp.szHuoDong3,nTong->szHuoDong3);
   t_sprintf(nTongComp.szHuoDong4,nTong->szHuoDong4);
   t_sprintf(nTongComp.szJiYu,nTong->szJiYu);	
   nTongComp.inType=0;
   nTongComp.inAcceptLevel = nTong->inAcceptLevel;
   nTongComp.inRefuseLevel = nTong->inRefuseLevel;
   if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&nTongComp, sizeof(PLAYER_TONGINFO));
}

///打造物品
void SendClientCmdComp(ItemCounmd *pInfo, int nItemNum, int nKind,int nFuMoVal) //命令 Pitem  种类
{
//	unsigned int* pInfo = (unsigned int*) uId;	
 //   ItemCounmd* pInfo = (ItemCounmd*)uId
	PLAYER_COMPITEM PlayerComp;	

	PlayerComp.ProtocolType = (BYTE)c2s_playercomp;

	ZeroMemory(PlayerComp.uId,sizeof(PlayerComp.uId));

 if (nKind < 4)
 {
    nItemNum = 3;
 }

 int temp[3]={0};

 if (nKind <13)
 { 
	for (int i = 0;i < nItemNum;i++)
	{
	//	int nIdx = *pInfo;  //一个int数组指针:pInfo[i]
	  int nIdx = pInfo->Uid[i];

	  if (nIdx <= 0)   //物品的索引限制？
      {
         if (nKind!=11)
         {
			/*KSystemMessage	sMsg;
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 1;
			sMsg.byParamSize = 0;
			t_sprintf(sMsg.szMessage,"警告:数据出错,请联系GM处理C!");
			sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
            CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);*/
			return;
		}
		else
		{
		    PlayerComp.uId[i]=0;
			continue;
		}		
      }

		PlayerComp.uId[i] = Item[nIdx].GetID();  //dwid

	  if (i<3)
		temp[i]=nIdx;

	}

	if((temp[0] == temp[1] || temp[0] == temp[2] || temp[1] == temp[2]))//如果主要的三个材料相同
	{
		/*KSystemMessage	sMsg;
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 1;
		sMsg.byParamSize = 0;
		t_sprintf(sMsg.szMessage,"警告:数据出错(%d)(%d)(%d)!",temp[0],temp[1],temp[2]);
        sMsg.nMsgLen=TEncodeText(sMsg.szMessage, strlen(sMsg.szMessage));
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);*/
	    return;
	}
 } 
 else if (nKind ==13 || nKind ==15 || nKind ==16)
 {
    int nIdx          = pInfo->Uid[0];
	PlayerComp.uId[0] = Item[nIdx].GetID();  //dwid
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:进阶石发送成功A！！");
 }
 //else if (nKind ==14)
	PlayerComp.bKind    = nKind; 
	PlayerComp.mFuMoVal = nFuMoVal;
 
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerComp, sizeof(PLAYER_COMPITEM));
    
}

void SendClientCmdStartTrade(int nStart, char* sName)
{
	PLAYER_START_TRADE PlayerTrade;
	PlayerTrade.ProtocolType =c2s_playerstarttrade;
	PlayerTrade.m_bSet = nStart;
	strcpy(PlayerTrade.m_Name,sName);
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerTrade, sizeof(PLAYER_START_TRADE));
}

void SendClientCmdLianCount(UINT dwId)
{
	/*
	LIAN_NEED_COUNT	LianCount;
	LianCount.ProtocolType = c2s_needLianCount;
	LianCount.dwId = dwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&LianCount, sizeof(LIAN_NEED_COUNT));
*/
}

void SendClientCmdGetCount( UINT dwId )
{
	PLAYER_NEED_COUNT	PlayerCount;
	PlayerCount.ProtocolType = c2s_playerneedcount;
	PlayerCount.dwId = dwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerCount, sizeof(PLAYER_NEED_COUNT));
}
//摆摊卖东西
void SendClientCmdPlayerBuy(int nIdx, UINT nPlayerId, int nPlace, int nX, int nY,int IsCityMenber,int nShuishou,int nMapID,BYTE nModel)
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg("提示:您网络有延时,请稍等4!");
		return;
	}

	PLAYER_TRADE_BUY_ITEM_COMMAND PlayerBuy;
	PlayerBuy.ProtocolType = c2s_playertradebuyitem;
	PlayerBuy.m_Idx = nIdx;
	PlayerBuy.m_PlayerId = nPlayerId;
	PlayerBuy.m_Place = (BYTE)nPlace;
	PlayerBuy.m_X = (BYTE)nX;
	PlayerBuy.m_Y = (BYTE)nY;
    PlayerBuy.m_IsCityMenber=IsCityMenber;
    PlayerBuy.m_Shuihou=nShuishou;
	PlayerBuy.m_MapID=nMapID;
	PlayerBuy.m_Model=nModel;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerBuy, sizeof(PLAYER_TRADE_BUY_ITEM_COMMAND));
	//Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
}
//设置摆摊价格
void SendClientCmdSetPrice(UINT nId, int nPrice,BYTE nModel)
{
	PLAYER_SET_PRICE PlayerPrice;
	PlayerPrice.ProtocolType = c2s_playersetprice;
	PlayerPrice.m_ID = nId;
	PlayerPrice.m_Price = nPrice;
	PlayerPrice.m_Model = nModel;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerPrice, sizeof(PLAYER_SET_PRICE));
}
//设置该物品是否在链接状态
void SendClientCmdSetLianFlg(int nId, BYTE nLianFlg)
{
	PLAYER_SET_LIANJIEFLG PlayerLianFlg;
	PlayerLianFlg.ProtocolType = c2s_playersetLianFlg;
	PlayerLianFlg.m_ID = nId;
	PlayerLianFlg.m_LianFlg = nLianFlg;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&PlayerLianFlg, sizeof(PLAYER_SET_LIANJIEFLG));
}

//摆摊广告
void SendClientCmdShopName(UINT dwId)
{
	PLAYER_NEED_COUNT ShopName;
	ShopName.ProtocolType = c2s_playershopname;
	ShopName.dwId = dwId;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&ShopName, sizeof(PLAYER_NEED_COUNT));
}


void SendClientUpData()//请求更新当前数据
{
	   BYTE	NetCommand = (BYTE)c2s_syncurdata;
    if (g_pClient)
		   g_pClient->SendPackToServer((BYTE*)&NetCommand, sizeof(BYTE));
}

void SendClientJdCallBack(nJinDuClient *nJdscript)
{
   PLAYER_JINDU_CALLBACK pJd;
       pJd.ProtocolType = c2s_jinducallback;
	   pJd.nMsgIndex    = nJdscript->nMsgidx;
#ifdef WIN32
       t_sprintf(pJd.szCallFuc,nJdscript->szCallFun);
	   t_sprintf(pJd.szParma,nJdscript->szParma);
	   t_sprintf(pJd.szParmb,nJdscript->szParmb);
	   t_sprintf(pJd.szParmc,nJdscript->szParmc);
#else
	   t_sprintf(pJd.szCallFuc,U2G(nJdscript->szCallFun).c_str());
	   t_sprintf(pJd.szParma,U2G(nJdscript->szParma).c_str());
	   t_sprintf(pJd.szParmb,U2G(nJdscript->szParmb).c_str());
	   t_sprintf(pJd.szParmc,U2G(nJdscript->szParmc).c_str());
#endif

       if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&pJd, sizeof(PLAYER_JINDU_CALLBACK));
		   
  //Player[CLIENT_PLAYER_INDEX].m_ItemList.ClientShowMsg(nJdscript->szCallFun);
}


void SendClientCmdSysShop(int nKind,int inParma,int inParmb,int inParmc,int inParmd)
{
	TONG_DISMISS_SYNC pDo;
	pDo.ProtocolType = c2s_playersysshop;
	pDo.m_nKind=nKind;
	pDo.m_inParma=inParma;
    pDo.m_inParmb=inParmb;
	pDo.m_inParmc=inParmc;
	pDo.m_inParmd=inParmd;

    if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&pDo, sizeof(TONG_DISMISS_SYNC));
}

void SendClientCmdLiXian()
{
	TONG_DISMISS_SYNC SysShop;
	SysShop.ProtocolType = c2s_playerlixian;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&SysShop, sizeof(TONG_DISMISS_SYNC));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
