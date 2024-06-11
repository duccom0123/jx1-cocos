#ifndef KPLAYERDEF_H
#define KPLAYERDEF_H

//数据库加载列表数据时每帧加载数量
#define DBLOADPERTIME_ITEM		50
#define DBLOADPERTIME_SKILL		20
#define DBLOADPERTIME_FRIEND	20
#define	DBLOADPERTIME_TASK		20

#define SCRIPT_PLAYERID		"PlayerId"		//脚本中的玩家Id全局变量名      dwid
#define SCRIPT_PLAYERINDEX	"PlayerIndex"	//脚本中的玩家的编号全局变量名	plaueridx
#define SCRIPT_OBJINDEX		"ObjIndex"		//触发脚本的物件的INDEX
#define SCRIPT_SUBWORLDINDEX "SubWorld"		//脚本中当前世界的Index的全局变量名
#define SCRIPT_SUBWORLDID	"SubWorldID"	//脚本中当前世界的ID的全局变量名	

#define	MAX_PLAYER		2    //Index 0 reserved 500
#define	CLIENT_PLAYER_INDEX	1   //客户端标示

#define	MAX_TEAM_APPLY_LIST	7	// 每个队伍的最大当前申请加入人数 32

//---------------------------------------------------------------------------
/*#define	QUEST_BEFORE	0x0000
#define	QUEST_START		0x0001
#define	QUEST_FINISHED	0xffff*/
//---------------------------------------------------------------------------
enum MOUSE_BUTTON
{
	button_left  = 0,
	button_right,
};

enum SCRIPTACTION  
{
	SCRIPTACTION_UISHOW,			//显示某个UI界面
	SCRIPTACTION_EXESCRIPT,			//执行脚本
};

enum
{
	ATTRIBUTE_STRENGTH = 0,
	ATTRIBUTE_DEXTERITY,
	ATTRIBUTE_VITALITY,
	ATTRIBUTE_ENGERGY,
};

enum	// 队伍创建失败原因
{
	Team_Create_Error_InTeam = 0,	// 已经在队伍中
	Team_Create_Error_SameName,		// 同名错误
	Team_Create_Error_Name,			// 队伍名字非法
	Team_Create_Error_TeamFull,		// 队伍数量太多，容纳不下
	Team_Create_Error_CannotCreate,	// 当前处于不能组队状态
};

enum TEAM_FIGURE
{
	TEAM_CAPTAIN,				// 队伍成员身份 队长
	TEAM_MEMBER,				// 队伍成员身份 队员
};

enum SYNC_STEP
{
	STEP_BASE_INFO = 0,
	STEP_FIGHT_SKILL_LIST,
	STEP_LIFE_SKILL_LIST,
	STEP_TASK_LIST,
	STEP_ITEM_LIST,  //装备清单
	STEP_FRIEND_LIST,  //朋友
	STEP_SYNC_END
};

enum SAVE_STATUS
{
	SAVE_IDLE = 0,
	SAVE_REQUEST,	 //立即存档
	SAVE_DOING,	     //到时间存档了
};
#endif
