//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Uiglobaldata_H
#define Uiglobaldata_H
//---------------------------------------------------------------------------
#pragma warning (disable: 4512)
#pragma warning (disable: 4786)
#pragma warning (disable: 4244)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#define INT int
#endif

#define WEATHERID_NOTHING	255
#define	DIR_DOWN		0
#define	DIR_LEFTDOWN	1
#define	DIR_LEFT		2
#define	DIR_LEFTUP		3
#define	DIR_UP			4
#define	DIR_RIGHTUP		5
#define	DIR_RIGHT		6
#define	DIR_RIGHTDOWN	7
//---------------------------------------------------------------------------
/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif*/
#define	CLIENT_IP	"FGSQE8WmCkIEMNXIBSlSt0IbCYIbwIHy"
#ifndef ZeroStruct
#define ZeroStruct(buf)					ZeroMemory(&(buf), sizeof(buf))
#endif

#define MAX_MELEEWEAPON_PARTICULARTYPE_NUM 100   //武器最大限制
#define MAX_RANGEWEAPON_PARTICULARTYPE_NUM 100   //暗器最大限制

#define  SCHEME_INI_AUTO_SETING "jx50ai/autoset/%u_set.ini"
#define  SCHEME_INI_AUTO_ITEM   "jx50ai/equipName.txt"
#define  SCHEME_INI_AUTO_DAOJU  "jx50ai/itemName.txt"
#define  SCHEME_INI_AUTO_USE    "jx50ai/itemUseName.txt"
#define  SCHEME_INI_AUTO_SKILL  "\\settings\\Ai\\skill.txt"
#define  CONTROL_SCALE 0.5
#define	 LIFE_TIME_DISTANCE_DEFAULT		50
#define	 AUTO_TIME_SAY_DEFAULT          18
#define	 LIFE_TIME_USERITEM_DEFAULT		180


#include "cocos2d.h"
//stdio.h 如果没有这个头文件 会出现一些错误 意外类型的xxx 意思就是没有这个类型的 声明
#include "engine/KbugInfo.h"
#include "engine/KEngine.h"
#include "engine/KGbktoUtf8.h"
//#include "KProtocol.h"
//#include "KMagicDesc.h"
#include <map>
#include "network/ClientStage.h"
#include "gamescene/LoginDef.h"
/*struct iKNetMsgTargetObject
{
	virtual void	AcceptNetMsg(void* pMsgData) = 0;
};

struct iKNetMsgTargetObject;
typedef void (*fnNetMsgCallbackFunc)(void* pMsgData);
*/
//#define	MAX_MSG_COUNT	1 << (PROTOCOL_MSG_SIZE * 8)
//iKNetMsgTargetObject*	m_MsgTargetObjs[MAX_MSG_COUNT];

//ui选择进入的状态 哪个界面
#define	MAX_PLAYER_PER_ACCOUNT	3
#define	SERVER_IP_MASK	"wNx0j0IA3_13KLLx2z_wbhAsAjD0e76C" //服务器ip
#define	CLIENT_MASK	    "0h706ENyd00gTA_vbiASkuQ02sDKnEHi"  //客户端标示
#define CLIENT_VER_FILE "\\Settings\\Ver\\Xfsverjx.ini"

#ifndef WIN32
typedef struct  _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
}	FILETIME;
#endif

struct KRoleChiefInfo
{
	char				Name[32];		//姓名
	unsigned	char	Gender;			//性别	be one of the SSC_CHARACTER_GENDER value
	unsigned	char	Attribute;		//五行属性
	//	unsigned   short	NativeRevID;
	union
	{ //联合变量 一次只能赋值一个
		unsigned short	NativePlaceId;	//出生地ID
		short			nLevel;			//等级		
	};
};
enum UI_STEP_STATE
{
	UI_STEP_NONE,  
	UI_STEP_STOP,  
	UI_STEP_START, 
};

enum UI_EQ_KIND
{//游戏ui中的父节点
	UI_EQ_PART_HEAD = 1, //头10
	UI_EQ_PART_BOBY=6,
	UI_EQ_PART_LEFTHD,
	UI_EQ_PART_RIGTHHD,
	UI_EQ_PART_LEFTHD_W,
	UI_EQ_PART_RIGTHHD_W,
	UI_EQ_PART_HORSE_Q=13,
	UI_EQ_PART_HORSE_Z,
	UI_EQ_PART_HORSE_H,
	UI_EQ_PART_PIFENG=17,
	UI_EQ_PART_NPC,
	UI_EQ_PART_NPC_STATE,
	UI_EQ_PART_SHADOW, //Shadow
	UI_NPC_PART_OTHER,
	UI_EQ_PART_COUNT,
};


enum PAD_KIND
{
	PAD_KIND_ITEM, //名称
	PAD_KIND_ROLE,
	PAD_KIND_ITEMEX,
	PAD_KIND_CHUWUXIANG,
	PAD_KIND_TEAM,
};

enum NPC_PART_OTHER_KEY
{
   NPC_NAME, //名称
   NPC_LIFE, //生命条
   NPC_TITLE,//称号
   NPC_TONG, //帮会名
   NPC_TIME, //存活时间
   NPC_SEX,  //五行
   PLAYER_AUTO,  //挂机
   PLAYER_TAN,   //摆摊
   PLAYER_PIFENG,   //PIFENG
   PLAYER_SPRCHENGHAO,   //PIFENG
   PLAYER_TITLE,    //TITLE
   PLAYER_REBORN,
   KEY_COUNT, 
};

enum LOGIN_LOGIC_STATUS//发送的消息
{
	LL_S_IDLE = 0,					//空闲
	LL_S_WAIT_INPUT_ACCOUNT,		//等待传账号密码
	LL_S_ACCOUNT_CONFIRMING,		//等待账号密码验证
	LL_S_WAIT_ROLE_LIST,			//等待接收角色列表数据
	LL_S_ROLE_LIST_READY,			//角色列表就绪
	LL_S_CREATING_ROLE,				//正在新建角色
	LL_S_WAIT_TO_DEL,				//等待验证删除
	LL_S_DELETING_ROLE,				//正在删除角色
	LL_S_WAIT_TO_LOGIN_GAMESERVER,	//等待登陆游戏服务器
	LL_S_ENTERING_GAME,				//正在进入游戏
	LL_S_IN_GAME,					//游戏运行时
};

enum LOGIN_LOGIC_RESULT_INFO //返回的消息
{
	LL_R_NOTHING,					//无结果信息
	LL_R_CONNECT_FAILED,			//连接失败
	LL_R_CONNECT_SERV_BUSY,			//服务器忙
	LL_R_CONNECT_TIMEOUT,			//连接超时未获得响应
	LL_R_ACCOUNT_PWD_ERROR,			//账号/密码错误
	LL_R_ACCOUNT_FREEZE,			//账号冻结
	LL_R_ACCOUNT_LOCKED,			//账号被锁定
	LL_R_INVALID_ROLENAME,			//(新建)角色的名字不合法
	LL_R_SERVER_SHUTDOWN,			//游戏服务器已满或正在维护中
	LL_R_INVALID_PROTOCOLVERSION,	//版本号较旧，需要升级到新的客户端	
	LL_R_INVALID_PASSWORD,			//（删除角色时）提供的密码错误
	LL_R_ACCOUNT_CONFIRM_SUCCESS,	//账号验证成功
	LL_R_CREATE_ROLE_SUCCESS,		//创建角色成功
	LL_R_LOGIN_TO_GAMESERVER,		//开始了与游戏世界服务器的连接
	LL_R_ACCOUNT_NOT_ENOUGH_POINT,	//账号点数不足
	LL_R_DENGLUQI_XFSJX,	        //专用登录器
	LL_R_WAIGUA_XFSJX,	            //外挂检测
	LL_R_CHANNEL_ROLE,              //却换角色
	LL_R_FORBITOPEN_GAMECLIENT,	    //限制多开
	LL_R_DIANNAO_GUANLIYUAN,        //管理员身份
	LL_R_OPNE_ACCOUNT,              //打开账号

};

enum BTN_PAGE_AUTO
{
	AUTOBTN_PAGE_BASE =1,
	AUTOBTN_PAGE_ART,
	AUTOBTN_PAGE_ITEM_1,
	AUTOBTN_PAGE_ITEM_2,
	AUTOBTN_PAGE_OTHER,
};

enum LIST_VIEW_TAG
{
	LIST_VIEW_ITEM =1,
	LIST_VIEW_TOOL,
	LIST_VIEW_USE,
	LIST_VIEW_OTHER,
};

enum CHECK_BOX_TAG
{
	BOX_TAG_BASE =1,
	BOX_TAG_ART,
	BOX_TAG_ITEM_1,
	BOX_TAG_ITEM_2,
	BOX_TAG_SHIQU,
	BOX_TAG_SAVE_M,
	BOX_TAG_TEAM_1,
	BOX_TAG_TEAM_2,
	BOX_TAG_TEAM_3,
	BOX_TAG_FENJIE,
	BOX_TAG_YANSHI,
	BOX_TAG_YABIAO,
	BOX_TAG_BAOLIU,
	BOX_TAG_USEITEM,
	BOX_TAG_DING,
	BOX_TAG_LUXIAN,
	BOX_TAG_LUZHI,
	BOX_TAG_GENSHUI,
	BOX_TAG_MIAOSHA,
	BOX_TAG_SOUND,
	BOX_TAG_JULI,
	BOX_TAG_SHADOM,
	BOX_TAG_TITLE,
	BOX_TAG_OTHER,
};
struct KLoginServer
{
	char			Title[32];		//服务器文字说明
	unsigned char	Address[4];		//服务器ip地址
};

struct	m_Choices
{
	int						nServerRegionIndex;			//服务器区域索引
	KLoginServer			AccountServer;				//当前使用的服务器
	char					Account[32];				//当前账号
	KSG_PASSWORD    		Password;					//当前账号的密码
	char					szProcessingRoleName[32];	//当前处理的角色的名字
	bool					bRememberAccount;			//是否纪录登陆账号
	bool					bRememberAll;				//是否纪录全部的登陆选择
	bool					bAutoLoginEnable;			//是否允许自动登陆
	bool					bIsRoleNewCreated;			//当前角色是否为新建的角色
	bool					bLoaded;					//是否已加载选择纪录
};

extern m_Choices m_ClientChoices;
extern short m_nNumRole;	        //角色数量
extern int   nGlobaluistep;
extern KRoleChiefInfo	m_RoleList[MAX_PLAYER_PER_ACCOUNT];

extern int  nPlaceId;               //选择的出生地
extern int  nNativePlaceId;         //地图编号
extern RECT maxMapRc;
extern bool isthisVer;
extern int  nCuraMapIdx;            //选择的出生地

#endif
