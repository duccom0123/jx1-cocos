
#ifndef COREUSENAMEDEF_H
#define COREUSENAMEDEF_H

#define		MAX_PLAYER_IN_ACCOUNT			3   //角色数量

//#define		SETTING_PATH					"\\settings"

#define	PLAYER_MENU_STATE_RES_FILE		"\\settings\\npcres\\界面状态与图形对照表.txt"//Ui_Stateandimgtable.txt"//界面状态与图形对照表.txt"
#define	NPC_RES_KIND_FILE_NAME			"\\settings\\npcres\\人物类型.txt"//PlayerType.txt" //人物类型.txt"

//---------------------------- npc res 相关 ------------------------------
#ifndef _SERVER

#define	RES_INI_FILE_PATH				"\\settings\\npcres"

#define	RES_SOUND_FILE_PATH				"sound"

#define	NPC_NORMAL_RES_FILE				"\\settings\\npcres\\普通npc资源.txt"//Nor_Npcres.txt"//普通npc资源.txt"
#define		NPC_NORMAL_SPRINFO_FILE		"\\settings\\npcres\\普通npc资源信息.txt"//Nor_Npcres_info.txt" //普通npc资源信息
#define		STATE_MAGIC_TABLE_NAME		"\\settings\\npcres\\状态图形对照表.txt"//StateImgtable.txt"//状态图形对照表.txt"
#define		PLAYER_RES_SHADOW_FILE		"\\settings\\npcres\\主角动作阴影对应表.txt"//roleactshadowtable.txt"//主角动作阴影对应表.txt"

#define		PLAYER_SOUND_FILE			"\\settings\\npcres\\主角动作声音表.txt"//roleactvoicetable.txt"//主角动作声音表.txt"
#define		NPC_SOUND_FILE				"\\settings\\npcres\\npc动作声音表.txt"//npcactvoicetable.txt"//npc动作声音表.txt"

#define		NPC_ACTION_NAME				"npc动作表.txt"//"npcacttable.txt"//"npc动作表.txt"
#define		ACTION_FILE_NAME			"动作编号表.txt"//"actnumbertable.txt"//"动作编号表.txt"

#define		PLAYER_INSTANT_SPECIAL_FILE		"\\settings\\npcres\\瞬间特效.txt"//one_effects.txt"//瞬间特效.txt"

#define		SPR_INFO_NAME					"信息"//"info"        //信息
#define		KIND_NAME_SECT					"人物类型"//"roletype"    //"人物类型"
#define		KIND_NAME_SPECIAL				"SpecialNpc"  //特殊npc
#define		KIND_NAME_NORMAL				"NormalNpc"   //普通npc
#define		KIND_FILE_SECT1					"部件说明文件名"//"bujianfilename"//"部件说明文件名"
#define		KIND_FILE_SECT2					"武器行为关联表1"//"wuqiactconnettable1"//"武器行为关联表1"
#define		KIND_FILE_SECT3					"武器行为关联表2"//"wuqiactconnettable2"//"武器行为关联表2"
#define		KIND_FILE_SECT4					"动作贴图顺序表"//"actimgordertable"//"动作贴图顺序表"
#define		KIND_FILE_SECT5					"资源文件路经"//"resfilepath"//"资源文件路经"
//---------------------------- 小地图相关 -------------------------------
#define		defLITTLE_MAP_SET_FILE	       "\\Ui\\Default\\小地图颜色.ini"//minimapcolor.ini" //"小地图颜色"
//----------------------------- 聊天相关 ------------------------------
#define		CHAT_PATH						"\\chat"
#define		CHAT_TEAM_INFO_FILE_NAME		"Team.cht"
#define		CHAT_CHANNEL_INFO_FILE_NAME		"Channel.cht"
//----------------------------- 声音相关 --------------------------------
#define		defINSTANT_SOUND_FILE	"\\settings\\SoundList.txt"
#define		defMUSIC_SET_FILE		"\\settings\\music\\MusicSet.txt"
#define		defMUSIC_FIGHT_SET_FILE	"\\settings\\music\\MusicFightSet.ini"

#define		OBJ_NAME_COLOR_FILE		"\\settings\\obj\\ObjNameColor.ini"  //物品颜色设置

#define STRINGRESOURSE_TABFILE			"\\settings\\StringResource.txt"
#define PLAYER_RANK_SETTING_TABFILE		"\\settings\\RankSetting.txt"

#define		CHAT_CHANNEL_NAME_ALL			"所有玩家"
#define		CHAT_CHANNEL_NAME_SCREEN		"附近玩家"
#define		CHAT_CHANNEL_NAME_SINGLE		"好友"
#define		CHAT_CHANNEL_NAME_TEAM			"队友"
#define		CHAT_CHANNEL_NAME_FACTION		"同门"
#define		CHAT_CHANNEL_NAME_TONG			"帮众"
#define		CHAT_CHANNEL_NAME_SCREENSINGLE	"陌生人"
#define		CHAT_CHANNEL_NAME_SYSTEM		"系统"

#define		CHAT_TAKE_CHANNEL_NAME_TEAM			"队伍频道"
#define		CHAT_TAKE_CHANNEL_NAME_FACTION		"门派频道"
#define		CHAT_TAKE_CHANNEL_NAME_TONG			"帮会频道"
#define		CHAT_TAKE_CHANNEL_NAME_SCREENSINGLE	"私聊频道"


#endif
//--------------------------- player 门派相关 ------------------------------
// 注：下面这个设定文件必须按 金 金 木 木 水 水 火 火 土 土 的顺序排列
#define		FACTION_FILE					"\\settings\\faction\\MonPhai.ini"
//--------------------------- player 帮会相关 ------------------------------
#define		defPLAYER_TONG_PARAM_FILE		"\\settings\\tong\\TongSet.ini"

//---------------------------- player 数值相关 ------------------------------
#define		PLAYER_LEVEL_EXP_FILE			"\\settings\\npc\\player\\level_exp.txt"
#define		PLAYER_LEVEL_ADD_FILE			"\\settings\\npc\\player\\level_add.txt"
#define		PLAYER_LEVEL_LEAD_EXP_FILE		"\\settings\\npc\\player\\level_lead_exp.txt"
#define		BASE_ATTRIBUTE_FILE_NAME		"\\settings\\npc\\player\\NewPlayerBaseAttribute.ini"
#define		PLAYER_PK_RATE_FILE				"\\settings\\npc\\PKRate.ini"
#define		PLAYER_BASE_VALUE				"\\settings\\npc\\player\\BaseValue.ini"
#define     NPC_LEVELSCRIPT_FILENAME		"\\script\\npclevelscript\\npclevelscript.lua"


// 00男金 01女金 02男木 03女木 04男水 05女水 06男火 07女火 08男土 09女土
//#define		NEW_PLAYER_INI_FILE_NAME		"\\settings\\player\\NewPlayerIni%02d.ini"

//---------------------------- player 交易相关 ------------------------------
#define		BUYSELL_FILE					"\\settings\\buysella.txt"
#define		GOODS_FILE						"\\settings\\goodsa.txt"

//----------------------------- 物品外观相关 ------------------------------
#define		CHANGERES_MELEE_FILE			"\\settings\\item\\MeleeRes.txt"
#define		CHANGERES_RANGE_FILE			"\\settings\\item\\RangeRes.txt"
#define		CHANGERES_ARMOR_FILE			"\\settings\\item\\ArmorRes.txt"
#define		CHANGERES_HELM_FILE				"\\settings\\item\\HelmRes.txt"
#define		CHANGERES_HORSE_FILE			"\\settings\\item\\HorseRes.txt"
#define		CHANCERES_GOLD_FILE				"\\settings\\item\\GolditemRes.txt"
#define		CHANCERES_PLAT_FILE			    "\\settings\\item\\PlatinaequipRes.txt"

#define		ITEM_ABRADE_FILE				"\\settings\\item\\AbradeRate.ini"
#define		CHANGERES_PIFENG_FILE           "\\settings\\item\\PiFengRes.txt"
#define		CHANGERES_CHIBANG_FILE          "\\settings\\item\\ChiBangRes.txt"
//--------------- npc skill missles 设定文件，用于生成模板 -------------
#define		SKILL_SETTING_FILE				"\\settings\\Skills.txt"
#define		MISSLES_SETTING_FILE			"\\settings\\Missles.txt"
#define		NPC_SETTING_FILE				"\\settings\\NpcS.txt"
#define		NPC_GOLD_TEMPLATE_FILE			"\\settings\\npc\\NpcGoldTemplate.txt"


//---------------------------- object 相关 ------------------------------
#define		OBJ_DATA_FILE_NAME		"\\settings\\obj\\ObjData.txt"
#define		MONEY_OBJ_FILE_NAME		"\\settings\\obj\\MoneyObj.txt"

//------------------------------------------------------------------------

//#define NPC_TEMPLATE_BINFILEPATH		"\\settings"

#define WEAPON_PHYSICSSKILLFILE			"\\settings\\武器物理攻击对照表.txt"//wuqiattacktabel.txt"//武器物理攻击对照表.txt"				
#define WEAPON_PARTICULARTYPE			"DetailType"//"详细类别"
#define	WEAPON_DETAILTYPE				"ParticularType"//具体类别"
#define	WEAPON_SKILLID					"对应物理技能编号"//"physicskillnumber"//"对应物理技能编号"

//---------------------------------任务--------------------------

//#define TASK_CONFIG_FILE                "\\script\\task\\tasklist.ini"
#define MAP_DROPRATE_FILE               "\\Settings\\mapList.ini"
#define GAME_SETTING_FILE               "\\Settings\\gamesetting.ini"

#define GAME_FSJINMAI_FILE              "\\Settings\\meridian\\meridian_level.txt"//"\\Settings\\FsJinMai\\fsjinmai.txt"
#define GAME_FORBITMAP_FILE             "\\Settings\\ForbititMap.txt"
#define GAME_FORBITWAIGUA_FILE          "\\Settings\\ForbititWaiGua.txt"
//#define GAME_ZHUCE_FILE                 "\\注册信息.ini"

#define GAME_ATLAS_FILE                  "\\Settings\\item\\atlas_compound.txt" //图谱文件
#define GAME_COMP_FILE                   "\\Settings\\item\\other_compound.txt" //其他材料
#define GAME_MSG_FILE                    "\\Settings\\EducationMessage.txt"     //教育信息

#define		MESSAGE_SYSTEM_ANNOUCE_HEAD		"MESSAGE_SYSTEM_ANNOUCE_HEAD"//"公告"
#define		MESSAGE_SYSTEM_WAIGUA_HEAD		"MESSAGE_SYSTEM_WAIGUA_HEAD" //"外挂"
#define		MESSAGE_SYSTEM_TONGZHI_HEAD		"MESSAGE_SYSTEM_TONGZHI_HEAD"//"通知"
#define		MESSAGE_SYSTEM_DUCHUAN_HEAD		"MESSAGE_SYSTEM_DUCHUAN_HEAD"//"战场"
#define     MESSAGE_SYSTEM_LIANJIE_HEAD     "MESSAGE_SYSTEM_LIANJIE_HEAD"//"链接"
#define		MESSAGE_SYSTEM_PAIMING_HEAD		"MESSAGE_SYSTEM_PAIMING_HEAD"//"<bclr=blue><color=white>排名<color><bclr>"
#define		MESSAGE_SYSTEM_BANGZHU_HEAD		"MESSAGE_SYSTEM_BANGZHU_HEAD"//"<bclr=yellow><color=red>帮主<color><bclr>"
#define		MESSAGE_SYSTEM_CITYHEAR_HEAD	"MESSAGE_SYSTEM_CITYHEAR_HEAD"//"<bclr=red><color=yellow>城主<color><bclr>"

#define		L_NPC_15                        "L_NPC_15"      //[error]致命错误,无法正确读取%s
#define		L_SortScript_4                  "L_SortScript_4"//-------脚本不存在,执行[%s][%s]失败!-------- \n
#define		L_SortScript_5                  "L_SortScript_5"
// 特别注意：以下字符串长度不能超过32字节，包括 %d %s 等接收具体内容以后的长度
#ifndef _SERVER  //客户端
#define		MSG_GET_EXP						"MSG_GET_EXP"
#define		MSG_DEC_EXP						"MSG_DEC_EXP"//没用
#define		MSG_LEVEL_UP					"MSG_LEVEL_UP"
#define		MSG_LEADER_LEVEL_UP				"MSG_LEADER_LEVEL_UP"
#define		MSG_GET_ATTRIBUTE_POINT			"MSG_GET_ATTRIBUTE_POINT"
#define		MSG_GET_SKILL_POINT				"MSG_GET_SKILL_POINT"
#define		MSG_GET_ATTRIBUTE_SKILL_POINT	"MSG_GET_ATTRIBUTE_SKILL_POINT" //没用

#define		MSG_TEAM_AUTO_REFUSE_INVITE		"MSG_TEAM_AUTO_REFUSE_INVITE"
#define		MSG_TEAM_NOT_AUTO_REFUSE_INVITE	"MSG_TEAM_NOT_AUTO_REFUSE_INVITE"//"提示:显示他人对您的组队邀请。"
#define		MSG_TEAM_SEND_INVITE			"MSG_TEAM_SEND_INVITE"//"提示:您向(%s)发出组队邀请!"
#define		MSG_TEAM_GET_INVITE				"MSG_TEAM_GET_INVITE"//"提示:(%s)邀请您组队!"
#define		MSG_TEAM_REFUSE_INVITE			"MSG_TEAM_REFUSE_INVITE"//"提示:(%s)拒绝了您的组队邀请!"  //没用
#define		MSG_TEAM_CREATE					"MSG_TEAM_CREATE"//"提示:您创建了一支队伍。"
#define		MSG_TEAM_CREATE_FAIL			"MSG_TEAM_CREATE_FAIL"//"提示:队伍创建失败。"
#define		MSG_TEAM_CANNOT_CREATE			"MSG_TEAM_CANNOT_CREATE"//"提示:您现在不能组队!"
#define		MSG_TEAM_TARGET_CANNOT_ADD_TEAM	"MSG_TEAM_TARGET_CANNOT_ADD_TEAM"//"提示:对方现在不能组队!"
#define		MSG_TEAM_OPEN					"MSG_TEAM_OPEN"//"提示:您的队伍现在允许接收新队员。"
#define		MSG_TEAM_CLOSE					"MSG_TEAM_CLOSE"//"提示:您的队伍现在不允许接收新队员。"
#define		MSG_TEAM_ADD_MEMBER				"MSG_TEAM_ADD_MEMBER"//"提示:(%s)成为您的队友了。"
#define		MSG_TEAM_SELF_ADD				"MSG_TEAM_SELF_ADD"//"提示:您加入(%s)的队伍。"
#define		MSG_TEAM_DISMISS_CAPTAIN		"MSG_TEAM_DISMISS_CAPTAIN"//"提示:您解散了自己的队伍!"
#define		MSG_TEAM_DISMISS_MEMBER			"MSG_TEAM_DISMISS_MEMBER"//"提示:(%s)解散了队伍!"
#define		MSG_TEAM_KICK_ONE				"MSG_TEAM_KICK_ONE"//"提示:(%s)被开除出队伍!"
#define		MSG_TEAM_BE_KICKEN				"MSG_TEAM_BE_KICKEN"//"提示:您被开除出队伍!"
#define		MSG_TEAM_APPLY_ADD				"MSG_TEAM_APPLY_ADD"//"提示:(%s)申请加入队伍!"
#define		MSG_TEAM_APPLY_ADD_SELF_MSG		"MSG_TEAM_APPLY_ADD_SELF_MSG"//"提示:您申请加入(%s)的队伍!"
#define		MSG_TEAM_LEAVE					"MSG_TEAM_LEAVE"//"提示:(%s)离开队伍。"
#define		MSG_TEAM_LEAVE_SELF_MSG			"MSG_TEAM_LEAVE_SELF_MSG"//"提示:您离开(%s)的队伍。"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL1	"MSG_TEAM_CHANGE_CAPTAIN_FAIL1"//"提示:队长任命失败!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL2	"MSG_TEAM_CHANGE_CAPTAIN_FAIL2"//"提示:(%s)统帅力不够!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL3	"MSG_TEAM_CHANGE_CAPTAIN_FAIL3"//"提示:您的队伍不能移交给新手!"
#define		MSG_TEAM_CHANGE_CAPTAIN			"MSG_TEAM_CHANGE_CAPTAIN"     //"提示:(%s)被任命为队长!"
#define		MSG_TEAM_CHANGE_CAPTAIN_SELF	"MSG_TEAM_CHANGE_CAPTAIN_SELF"//"提示:您被(%s)任命为队长!"

#define		MSG_CHAT_APPLY_ADD_FRIEND		"MSG_CHAT_APPLY_ADD_FRIEND"//"提示:您向(%s)申请好友。"
#define		MSG_CHAT_FRIEND_HAD_IN			"MSG_CHAT_FRIEND_HAD_IN"//"提示:(%s)已被加为好友。"
#define		MSG_CHAT_GET_FRIEND_APPLY		"MSG_CHAT_GET_FRIEND_APPLY"//"提示:(%s)申请加为好友!"
#define		MSG_CHAT_REFUSE_FRIEND			"MSG_CHAT_REFUSE_FRIEND"//"提示:(%s)拒绝好友申请!"
#define		MSG_CHAT_ADD_FRIEND_FAIL		"MSG_CHAT_ADD_FRIEND_FAIL"//"提示:添加(%s)为好友失败!"
#define		MSG_CHAT_ADD_FRIEND_SUCCESS		"MSG_CHAT_ADD_FRIEND_SUCCESS"//"提示:您与(%s)成为好友!"
#define		MSG_CHAT_CREATE_TEAM_FAIL1		"MSG_CHAT_CREATE_TEAM_FAIL1"//"提示:好友分组创建失败!"
#define		MSG_CHAT_CREATE_TEAM_FAIL2		"MSG_CHAT_CREATE_TEAM_FAIL2"//"提示:已存在同名组!"
#define		MSG_CHAT_CREATE_TEAM_FAIL3		"MSG_CHAT_CREATE_TEAM_FAIL3"//"提示:已达最大分组数!"
#define		MSG_CHAT_RENAME_TEAM_FAIL		"MSG_CHAT_RENAME_TEAM_FAIL"//"提示:组名修改失败!"
#define		MSG_CHAT_DELETE_TEAM_FAIL1		"MSG_CHAT_DELETE_TEAM_FAIL1"//"提示:组删除失败!"
#define		MSG_CHAT_DELETE_TEAM_FAIL2		"MSG_CHAT_DELETE_TEAM_FAIL2"//"提示:这个组不允许删除!"
#define		MSG_CHAT_FRIEND_ONLINE			"MSG_CHAT_FRIEND_ONLINE"//"提示:(%s)上线了!"
#define		MSG_CHAT_FRIEND_OFFLINE			"MSG_CHAT_FRIEND_OFFLINE"//"提示:(%s)下线了!"
#define		MSG_CHAT_DELETE_FRIEND			"MSG_CHAT_DELETE_FRIEND"//"提示:您与(%s)解除了好友关系!"
#define		MSG_CHAT_DELETED_FRIEND			"MSG_CHAT_DELETED_FRIEND"//"提示:(%s)与您解除了好友关系!"
#define		MSG_CHAT_MSG_FROM_FRIEND		"MSG_CHAT_MSG_FROM_FRIEND"//"提示:(%s)发来消息!"
#define		MSG_CHAT_FRIEND_NOT_NEAR		"MSG_CHAT_FRIEND_NOT_NEAR"//"提示:(%s)不在附近!"
#define		MSG_CHAT_FRIEND_NOT_ONLINE		"MSG_CHAT_FRIEND_NOT_ONLINE"//"提示:(%s)不在线!"
#define		MSG_CHAT_TAR_REFUSE_SINGLE_TALK	"MSG_CHAT_TAR_REFUSE_SINGLE_TALK"//"提示:(%s)没有订阅私聊频道!"

#define		MSG_SHOP_NO_ROOM				"MSG_SHOP_NO_ROOM"//"提示:背包空间不足!"
#define		MSG_SHOP_NO_MONEY				"MSG_SHOP_NO_MONEY"//"提示C:另加<color=green>税收(%d%s)<color>,金钱不足!"
#define		MSG_SHOP_YOU_MONEY				"MSG_SHOP_YOU_MONEY"//"提示:占帮成员优惠<color=green>(%d%s)<color>,金钱不足!"
#define		MSG_SHOP_YOUS_MONEY				"MSG_SHOP_YOUS_MONEY"//"提示:占帮成员优惠<color=green>(%d%s)<color>,花费了<color=green>(%d)<color>两!"
#define		MSG_SHOP_YOUF_MONEY				"MSG_SHOP_YOUF_MONEY"//"提示:另加城市<color=green>税收(%d%s)<color>,花费了<color=green>(%d)<color>两!"
#define		MSG_HUANZHUANG_NO_ROOM			"MSG_HUANZHUANG_NO_ROOM"//"提示:<color=yellow>右键换装功能即将推出!<color>"
#define		MSG_SKILL_EXISTS_IN_IMMEDIA     "MSG_SKILL_EXISTS_IN_IMMEDIA"    //快捷栏上已经有该技能了
#define		MSG_SKILL_IS_NOT_IMMEDIA        "MSG_SKILL_IS_NOT_IMMEDIA"       //该技能不是快捷技能，不能放到快捷栏上
#define		MSG_ITEM_CANNOT_IN_IMMEDIATE    "MSG_ITEM_CANNOT_IN_IMMEDIATE"   //该类物品不能放到快捷栏上

#define		MSG_NPC_NO_MANA					"MSG_NPC_NO_MANA"//"提示:内力不足!"
#define		MSG_NPC_NO_STAMINA				"MSG_NPC_NO_STAMINA"//"提示:体力不足!"
#define		MSG_NPC_NO_LIFE					"MSG_NPC_NO_LIFE"//"提示:生命不足!"
#define		MSG_NPC_DEATH					"MSG_NPC_DEATH"//"提示:携带<color=green>复活印鉴<color>即可原地重生!"

#define		MSG_OBJ_CANNOT_PICKUP			"MSG_OBJ_CANNOT_PICKUP"//"提示:您不能拾取别人的物品!"
#define		MSG_MONEY_CANNOT_PICKUP			"MSG_MONEY_CANNOT_PICKUP"//"提示:您不能拾取别人的钱!"
#define		MSG_OBJ_TOO_FAR					"MSG_OBJ_TOO_FAR"//"提示:物品太远，无法拾取!"
#define		MSG_DEC_MONEY					"MSG_DEC_MONEY"//"提示:您损失了(%d)两银子!"
#define		MSG_EARN_MONEY					"MSG_EARN_MONEY"//"提示:您获得了(%d)两银子!"
#define		MSG_DEATH_LOSE_ITEM				"MSG_DEATH_LOSE_ITEM"//"提示:您丢失了物品%s!"
#define		MSG_ADD_ITEM					"MSG_ADD_ITEM"//"提示:您获得了(%s)!"
#define		MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE	"MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE"//"提示:快捷栏已经有同类型的物品了!"
#define		MSG_CAN_NOT_VIEW_ITEM			"MSG_CAN_NOT_VIEW_ITEM"//"提示:现在不能察看他人装备!"//没用
#define		MSG_ITEM_DAMAGED				"MSG_ITEM_DAMAGED"//"提示:您装备的(%s)长期磨损，已经彻底损坏!"
#define		MSG_ITEM_NEARLY_DAMAGED			"MSG_ITEM_NEARLY_DAMAGED"//"提示:您装备已经接近损坏,请及时修理!"

#define		MSG_TRADE_STATE_OPEN			"MSG_TRADE_STATE_OPEN"//"提示:您现在可以交易。"
#define		MSG_TRADE_STATE_CLOSE			"MSG_TRADE_STATE_CLOSE"//"提示:您现在不能交易。"
#define		MSG_TRADE_SELF_LOCK				"MSG_TRADE_SELF_LOCK"//"提示:您交易物品已经锁定。"
#define		MSG_TRADE_SELF_UNLOCK			"MSG_TRADE_SELF_UNLOCK"//"提示:您交易物品解除锁定。"
#define		MSG_TRADE_DEST_LOCK				"MSG_TRADE_DEST_LOCK"//"提示:(%s)交易物品已经锁定。"
#define		MSG_TRADE_DEST_UNLOCK			"MSG_TRADE_DEST_UNLOCK"//"提示:(%s)交易物品解除锁定。"
#define		MSG_TRADE_SUCCESS				"MSG_TRADE_SUCCESS"//"提示:您与(%s)交易成功。"
#define		MSG_TRADE_FAIL					"MSG_TRADE_FAIL"//"提示:您与(%s)交易失败。"
#define		MSG_TRADE_SELF_ROOM_FULL		"MSG_TRADE_SELF_ROOM_FULL"//"提示:您的背包空间不足!"
#define		MSG_TRADE_DEST_ROOM_FULL		"MSG_TRADE_DEST_ROOM_FULL"//"提示:(%s)的背包空间不足!"
#define		MSG_TRADE_SEND_APPLY			"MSG_TRADE_SEND_APPLY"//"提示:您向(%s)申请交易!"
#define		MSG_TRADE_GET_APPLY				"MSG_TRADE_GET_APPLY"//"提示:(%s)申请与您交易!"
#define		MSG_TRADE_REFUSE_APPLY			"MSG_TRADE_REFUSE_APPLY"//"提示:(%s)拒绝与您交易!"
#define		MSG_TRADE_TASK_ITEM				"MSG_TRADE_TASK_ITEM"//"提示:任务限时绑定物品不能交易!"

#define		G_ProtocolProcess_20            "G_ProtocolProcess_20"  //你当前状态禁止移动或者交易物品！

#define		MSG_PK_NORMAL_FLAG_OPEN			"MSG_PK_NORMAL_FLAG_1" //"MSG_PK_NORMAL_FLAG_OPEN"//"提示:PK开关打开!"
#define		MSG_PK_NORMAL_FLAG_CLOSE		"MSG_PK_NORMAL_FLAG_0"//"MSG_PK_NORMAL_FLAG_CLOSE"//"提示:PK开关关闭!"
#define		MSG_PK_VALUE					"MSG_PK_VALUE"//"提示:您目前的PK值是%d!"
#define		MSG_PK_ERROR_1					"MSG_PK_ERROR_1"//"提示:您现在处于非战斗状态!"
#define		MSG_PK_ERROR_2					"MSG_PK_ERROR_2"//"提示:新手不能仇杀!"
#define		MSG_PK_ERROR_3					"MSG_PK_ERROR_3"//"提示:您正与他人切磋!"
#define		MSG_PK_ERROR_4					"MSG_PK_ERROR_4"//"提示:您正与他人仇杀!"
#define		MSG_PK_ERROR_5					"MSG_PK_ERROR_5"//"提示:您不能仇杀新手!"
#define		MSG_PK_ERROR_6					"MSG_PK_ERROR_6"//"提示:对方正处于非战斗状态!"
#define		MSG_PK_ERROR_7					"MSG_PK_ERROR_7"//"提示:对方正与他人切磋!"
#define		MSG_PK_ENMITY_SUCCESS_1			"MSG_PK_ENMITY_SUCCESS_1"//"提示:您与(%s)建立仇杀关系!"
#define		MSG_PK_ENMITY_SUCCESS_2			"MSG_PK_ENMITY_SUCCESS_2"//"提示:10秒钟后仇杀开始执行!"
#define		MSG_PK_ENMITY_CLOSE				"MSG_PK_ENMITY_CLOSE"//"提示:您的仇杀关系结束!"
#define		MSG_PK_ENMITY_OPEN				"MSG_PK_ENMITY_OPEN"//"提示:仇杀现在开始执行!"

#define		MSG_TONG_CREATE_ERROR01			"MSG_TONG_CREATE_ERROR01"//"提示:帮会名过长!或帮会名问题"
#define		MSG_TONG_CREATE_ERROR06			"MSG_TONG_CREATE_ERROR06"//"提示:帮会阵营错误或统帅等级不够!"
#define		MSG_TONG_CREATE_ERROR05		    "MSG_TONG_CREATE_ERROR05"//"提示:帮会成员不能另外建立帮会!"
#define		MSG_TONG_CREATE_ERROR04			"MSG_TONG_CREATE_ERROR04"//"提示:杀手才能建立帮会!"
#define		MSG_TONG_CREATE_ERROR07			"MSG_TONG_CREATE_ERROR07"//"提示:建立帮会的等级不够!"
#define		MSG_TONG_CREATE_ERROR08			"MSG_TONG_CREATE_ERROR08"//"提示:建立帮会需要(%d)金钱!"
#define		MSG_TONG_CREATE_ERROR09			"MSG_TONG_CREATE_ERROR09"//"提示:组队中，不能建立帮会!"
#define		MSG_TONG_CREATE_ERROR10		    "MSG_TONG_CREATE_ERROR10"//"提示:帮会模块出错!请联系GM!"
#define		MSG_TONG_CREATE_ERROR11			"MSG_TONG_CREATE_ERROR11"//"提示:帮会名不合法或名字字符串出错!"
#define		MSG_TONG_CREATE_ERROR12			"MSG_TONG_CREATE_ERROR12"//"提示:帮会名不合法或名字字符串过长!"
#define		MSG_TONG_CREATE_ERROR13			"MSG_TONG_CREATE_ERROR13"//"提示:帮会名不合法或与别的帮会重名!"
#define		MSG_TONG_APPLY_CREATE			"MSG_TONG_APPLY_CREATE"//"提示:您申请创建帮会!"
#define		MSG_TONG_CREATE_SUCCESS			"MSG_TONG_CREATE_SUCCESS"//"提示:帮会创建成功!"
#define		MSG_TONG_APPLY_ADD				"MSG_TONG_APPLY_ADD"//"提示:您申请加入帮会!"
#define		MSG_TONG_APPLY_ADD_ERROR1		"MSG_TONG_APPLY_ADD_ERROR1"//"提示:帮会成员不能另外加入帮会!"
#define		MSG_TONG_APPLY_ADD_ERROR2		"MSG_TONG_APPLY_ADD_ERROR2"//"提示:杀手才能加入帮会!"
#define		MSG_TONG_APPLY_ADD_ERROR3		"MSG_TONG_APPLY_ADD_ERROR3"//"提示:组队中，不能加入帮会!"
#define		MSG_TONG_REFUSE_ADD				"MSG_TONG_REFUSE_ADD"//"提示:(%s)拒绝您加入帮会!"
#define		MSG_TONG_ADD_SUCCESS			"MSG_TONG_ADD_SUCCESS"//"提示:您加入帮会!"
#define		MSG_TONG_CANNOT_LEAVE1			"MSG_TONG_CANNOT_LEAVE1"//"提示:帮主不能离开帮会!"
#define		MSG_TONG_CANNOT_LEAVE2			"MSG_TONG_CANNOT_LEAVE2"//"提示:长老不能离开帮会!"
#define		MSG_TONG_BE_KICKED				"MSG_TONG_BE_KICKED"//"提示:您被踢出帮会!"
#define		MSG_TONG_LEAVE_SUCCESS			"MSG_TONG_LEAVE_SUCCESS"//"提示:您主动离开帮会成功!"
#define		MSG_TONG_LEAVE_FAIL				"MSG_TONG_LEAVE_FAIL"//"提示:您主动离开帮会失败!"
#define		MSG_TONG_CHANGE_AS_MASTER		"MSG_TONG_CHANGE_AS_MASTER"//"提示:您被任命为帮主!"
#define		MSG_TONG_CHANGE_AS_MEMBER		"MSG_TONG_CHANGE_AS_MEMBER"//"提示:您现在成为普通帮众!"

#define		MSG_EARN_XU		                "MSG_EARN_XU"          //"提示:金币增加(%d)个!"
#define		MSG_PK_NORMAL_FLAG_DS		    "MSG_PK_NORMAL_FLAG_2" //"MSG_PK_NORMAL_FLAG_DS"//"提示:屠杀模式开启!"
#define		MSG_NPC_CANNOT_RIDE		        "MSG_NPC_CANNOT_RIDE"  //"提示:您太累了,不能频繁的上下马!"
#define     MSG_SUNYI_FAILED                "MSG_SUNYI_FAILED"     //"提示:瞬间移动执行失败"  //没有
#define     MSG_SUNYI_SUCCESS               "MSG_SUNYI_SUCCESS"    //"提示:瞬间移动执行成功"

#define     MSG_GUAJI_SUCCESS               "MSG_GUAJI_SUCCESS"//"提示:自动挂机开启!"
#define		MSG_COMP_FAILED					"MSG_COMP_FAILED"//"提示:合成失败!"
#define		MSG_COMP_SUCCESS				"MSG_COMP_SUCCESS"//"提示:合成成功!"
#define		MSG_COMP_ERITEM					"MSG_COMP_ERITEM"//"提示:物品不对!"
#define		MSG_COMP_MONEY					"MSG_COMP_MONEY"//"提示:金钱不足!"
#define		MSG_ID_COMP_NO                  "MSG_ID_COMP_NO"//"提示:该物品禁止铸造!"
#define		MSG_SHOP_NO_XU		            "MSG_SHOP_NO_XU"//"提示:另加<color=green>税收(%d%s)<color>,金币或银两不足!"
#define		MSG_SHOP_NO_FUYUAN	            "MSG_SHOP_NO_FUYUAN"
#define		MSG_SHOP_NO_SHENGWANG	        "MSG_SHOP_NO_SHENGWANG"
#define		MSG_SHOP_NO_DIANKA	            "MSG_SHOP_NO_DIANKA"
#define		MSG_SHOP_NO_JIFEN	            "MSG_SHOP_NO_JIFEN"
#define		MSG_SHOP_YOU_XU		            "MSG_SHOP_YOU_XU"//"提示:占帮成员优惠<color=green>(%d%s)<color>,金币或银两不足!"
#define		MSG_SHOP_YOUS_XU				"MSG_SHOP_YOUS_XU"//"提示:占帮成员优惠<color=green>(%d%s)<color>,花费了<color=green>(%s)<color>金币!" //没有
#define		MSG_SHOP_YOUF_XU				"MSG_SHOP_YOUF_XU"//"提示:另加城市<color=green>税收(%d%s)<color>,花费了<color=green>(%s)<color>金币!"//没有
#define	    MSG_DROP_DEST_FAILED            "MSG_DROP_DEST_FAILED"//"提示:物品不能随便丢掉!" 没有
#define	    MSG_ID_LIANJIE_FAILED           "MSG_ID_LIANJIE_FAILED"//"提示:<color=white>该链接已经失效!<color>"           
#define		MSG_TEAM_CREAT_FRIEND			"MSG_TEAM_CREAT_FRIEND"//"提示:创建组队界面失败!" 没有
#define	    MSG_ID_SHUA_SHUA                "MSG_ID_SHUA_SHUA"//"提示:<color=white>放置失败!<color>"
#define		MSG_CAN_NOT_PK_SUNYI			"MSG_CAN_NOT_PK_SUNYI"//"提示:非战斗模式状态,启动寻路功能!"
#define		MSG_ID_FUHUO_YUANDIAN           "MSG_ID_FUHUO_YUANDIAN"//"<color=yellow>复活:请带上您的<color=green>复活戒子<color>就可以原地复活了!"
#define		MSG_SKILL_CANNOT_PICKUP         "MSG_SKILL_CANNOT_PICKUP" //不能拾取该技能

#endif

enum enumMSG_ID
{
	enumMSG_ID_TEAM_KICK_One,
	enumMSG_ID_TEAM_DISMISS,
	enumMSG_ID_TEAM_LEAVE,
	enumMSG_ID_TEAM_REFUSE_INVITE,
	enumMSG_ID_TEAM_SELF_ADD,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2,
	enumMSG_ID_OBJ_CANNOT_PICKUP,
	enumMSG_ID_OBJ_TOO_FAR,
	enumMSG_ID_DEC_MONEY,
	enumMSG_ID_TRADE_SELF_ROOM_FULL,
	enumMSG_ID_TRADE_DEST_ROOM_FULL,
	enumMSG_ID_TRADE_REFUSE_APPLY,
	enumMSG_ID_TRADE_TASK_ITEM,
	enumMSG_ID_GET_ITEM,
	enumMSG_ID_ITEM_DAMAGED,
	enumMSG_ID_MONEY_CANNOT_PICKUP,
	enumMSG_ID_CANNOT_ADD_TEAM,
	enumMSG_ID_TARGET_CANNOT_ADD_TEAM,
	enumMSG_ID_PK_ERROR_1,
	enumMSG_ID_PK_ERROR_2,
	enumMSG_ID_PK_ERROR_3,
	enumMSG_ID_PK_ERROR_4,
	enumMSG_ID_PK_ERROR_5,
	enumMSG_ID_PK_ERROR_6,
	enumMSG_ID_PK_ERROR_7,
	enumMSG_ID_DEATH_LOSE_ITEM,
	enumMSG_ID_TONG_REFUSE_ADD,
	enumMSG_ID_TONG_BE_KICK,
	enumMSG_ID_TONG_LEAVE_SUCCESS,
	enumMSG_ID_TONG_LEAVE_FAIL,
	enumMSG_ID_TONG_CHANGE_AS_MASTER,
	enumMSG_ID_TONG_CHANGE_AS_MEMBER,
	enumMSG_ID_RIDE_CANNOT, //骑马
	enumMSG_ID_COMP_FAILED,//打造合成
	enumMSG_ID_COMP_SUCCESS,
	enumMSG_ID_COMP_ERITEM,
	enumMSG_ID_SUNYI_FAILED, //瞬移
	enumMSG_ID_GUAJI_SUCCESS,//挂机
	enumMSG_ID_GUAJI_SHIBAIA,
	enumMSG_ID_GUAJI_SHIBAIB,
	enumMSG_ID_GUAJI_SHIBAIC,
	enumMSG_ID_GUAJI_SHIBAID,
	enumMSG_ID_GUAJI_SHIBAIE,
	enumMSG_ID_GUAJI_SHIBAIF,
	enumMSG_ID_GUAJI_SHIBAIG,
	enumMSG_ID_SUNYI_SUCCESS,
	enumMSG_CAN_NOT_PK_SUNYI,
	enumMSG_ID_DROP_DEST_FAILED,
    enumMSG_SHOP_YOUS_MONEY,
	enumMSG_SHOP_YOUF_MONEY,
	enumMSG_SHOP_YOUS_XU,
	enumMSG_SHOP_YOUF_XU,
	enumMSG_ID_LIANJIE_FAILED,//链接失效
	enumMSG_ID_SHUA_SHUA,
	enumMSG_ID_RENOVE_TIME_ITEM, //删除过期物品
    enumMSG_ID_FUHUO_FAILED,
	enumMSG_ID_ERROR_INFO,
	enumTONG_COMMAND_ID_APPLY_SAVE,
	enumTONG_COMMAND_ID_APPLY_GET,
	enumTONG_COMMAND_ID_APPLY_SND,
	enumMSG_ID_COMP_MONEY,
	enumMSG_ID_COMP_NO,
	enumMSG_ID_NUM,
};

//---------------------------- 鼠标指针相关 ------------------------------
#define		MOUSE_CURSOR_NORMAL				0
#define		MOUSE_CURSOR_FIGHT				1
#define		MOUSE_CURSOR_DIALOG				2
#define		MOUSE_CURSOR_PICK				3
#define		MOUSE_CURSOR_USE				8




#endif
