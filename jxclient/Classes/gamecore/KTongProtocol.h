// -------------------------------------------------------------------------
//	文件名		：	KTongProtocol.h
//	创建者		：	谢茂培 (Hsie)
//	创建时间	：	2003-08-13 15:12:19
//	功能描述	：	
//
// -------------------------------------------------------------------------
#ifndef __KTONGPROTOCOL_H__
#define __KTONGPROTOCOL_H__
#include "KProtocol.h"
#pragma pack(push, 1)

#define		defTONG_PROTOCOL_SERVER_NUM		255
#define		defTONG_PROTOCOL_CLIENT_NUM		255

//---------------------------- tong protocol ----------------------------
// relay server 收到的 game server 的协议
enum 
{
	enumC2S_TONG_CREATE = 0,			// 创建帮会
	enumC2S_TONG_ADD_MEMBER,			// 添加成员
	enumC2S_TONG_GET_HEAD_INFO,			// 获得帮会信息
	enumC2S_TONG_GET_MANAGER_INFO,		// 获得帮会队长信息
	enumC2S_TONG_GET_MEMBER_INFO,		// 获得帮会帮众信息
	enumC2S_TONG_INSTATE,				// 任命
	enumC2S_TONG_KICK,					// 踢人
	enumC2S_TONG_LEAVE,					// 离帮
	enumC2S_TONG_CHANGE_MASTER,			// 传位
	enumC2S_TONG_ACCEPT_MASTER,			// 是否接受传位
	enumC2S_TONG_GET_LOGIN_DATA,		// 玩家登陆时申请帮会数据
	enumC2S_TONG_MONEY_SAVE,
	enumC2S_TONG_MONEY_GET,
	enumC2S_TONG_MONEY_SND,
	enumC2S_TONG_ATTACK,
	enumC2S_TONG_ATTACK_GET_INFO,
	enumC2S_TONG_ATTACK_SEND_INFO,
	enumC2S_TONG_GET_LIST_INFO,		    // 获得帮会列表信息
	enumC2S_TONG_INFO_SET,              // 设置帮会信息
	enumC2S_TONG_NUM,					// 数量
};
//设置帮会信息的子协议
enum 
{
	    enumC2S_TONG_SET_LEVEL = 0,			//设置等级
		enumC2S_TONG_SET_MAP,
		enumC2S_TONG_SET_CREDIT,
		enumC2S_TONG_SET_BANGTITLE,
		enumC2S_TONG_SET_ZHANGTITLE,
		enumC2S_TONG_SET_DUITITLE,
		enumC2S_TONG_SET_ZONGTITLE,
		enumC2S_TONG_SET_DEATHCOUNT,
		enumC2S_TONG_SET_ATTACKWIN,
		enumC2S_TONG_SET_ATTACKLOSE,
		enumC2S_TONG_RANDOM_MAP,  //随机设置帮助地图
		enumC2S_TONG_SET_NUM,
};

// relay server 发给 game server 的协议
enum
{
	enumS2C_TONG_CREATE_SUCCESS = 0,		// 帮会创建成功
	enumS2C_TONG_CREATE_FAIL,				// 帮会创建失败
	enumS2C_TONG_ADD_MEMBER_SUCCESS,		// 帮会添加成员成功
	enumS2C_TONG_ADD_MEMBER_FAIL,			// 帮会添加成员失败
	enumS2C_TONG_HEAD_INFO,					// 帮会信息
	enumS2C_TONG_MANAGER_INFO,				// 帮会队长信息
	enumS2C_TONG_MEMBER_INFO,				// 帮会队长信息
	enumS2C_TONG_BE_INSTATED,				// 被任命
	enumS2C_TONG_INSTATE,					// 任命成功或失败
	enumS2C_TONG_KICK,						// 踢人成功或失败
	enumS2C_TONG_BE_KICKED,					// 被踢出帮会
	enumS2C_TONG_LEAVE,						// 离开帮会成功或失败
	enumS2C_TONG_CHECK_CHANGE_MASTER_POWER,	// 判断是否有当帮主的能力
	enumS2C_TONG_CHANGE_MASTER_FAIL,		// 传位失败
	enumS2C_TONG_CHANGE_AS,					// 传位成功，身份改变
	enumS2C_TONG_CHANGE_MASTER,				// 广播，更换帮主
	enumS2C_TONG_LOGIN_DATA,				// 玩家登陆时候获得帮会数据
	enumS2C_TONG_MONEY_SAVE,	            // 存钱
	enumS2C_TONG_MONEY_GET,	                // 取钱
	enumS2C_TONG_MONEY_SND,                 // 发钱
	enumS2C_TONG_ATTACK_GETBACK,
	enumS2C_TONG_ATTACK_SENDBACK,
	enumS2C_TONG_ATTACK_OVER,
	enumS2C_TONG_ALLLIST_INFO,
	enumS3C_TONG_SYNCITY_INFO,
	enumS3C_TONG_SERVER_TIEM,
	enumS2C_TONG_NUM,					    // 数量
};
//-------------------------- tong protocol end --------------------------

//friend protocol
enum 
{
	friend_c2c_askaddfriend,	//请求加为好友
	friend_c2c_repaddfriend,	//同意/拒绝加为好友
	friend_c2s_groupfriend,		//将好友分组
	friend_c2s_erasefriend,		//删除好友

	friend_c2s_asksyncfriendlist,	//请求同步好友列表
	friend_s2c_repsyncfriendlist,	//同步好友列表

	friend_s2c_friendstate,		//好友状态通知

	friend_c2s_associate,		//GS到Relay，自动组合2个人（有方向）
	friend_c2s_associatebevy,	//GS到Relay，自动组合n个人
	friend_s2c_syncassociate,	//Relay到Client，通知组合
};

//extend protocol
enum
{
	extend_s2c_passtosomeone,
	extend_s2c_passtobevy,
};



/////////////////////////////////////////////////////////////////
//friend struct

const int _GROUP_NAME_LEN = _NAME_LEN * 2;


struct ASK_ADDFRIEND_CMD : EXTEND_HEADER
{
	BYTE pckgid;
	char dstrole[_NAME_LEN];
};
struct ASK_ADDFRIEND_SYNC : EXTEND_HEADER
{
	BYTE pckgid;
	char srcrole[_NAME_LEN];
};

enum {answerAgree, answerDisagree, answerUnable};
struct REP_ADDFRIEND_CMD : EXTEND_HEADER
{
	BYTE pckgid;
	char dstrole[_NAME_LEN];
	BYTE answer;	//agree/disagree/unable
};
struct REP_ADDFRIEND_SYNC : EXTEND_HEADER
{
	BYTE pckgid;
	char srcrole[_NAME_LEN];
	BYTE answer;	//agree/disagree/unable
};



//used by GROUP_FRIEND & REP_SYNCFRIENDLIST
enum {specOver = 0x00, specGroup = 0x01, specRole = 0x02};

struct GROUP_FRIEND : EXTEND_HEADER
{
	//format: char seq
	//specGroup标记组，其后接该组好友列表，以\0间隔，specRole标记角色名
	//最后以双\0结束
};


struct ERASE_FRIEND : EXTEND_HEADER
{
	char friendrole[_NAME_LEN];
};


struct ASK_SYNCFRIENDLIST : EXTEND_HEADER
{
	BYTE pckgid;
	BYTE full;
};


struct REP_SYNCFRIENDLIST : EXTEND_HEADER
{
	BYTE pckgid;
	//format: char seq (same as GROUP_FRIEND)
	//specGroup标记组，其后接该组好友列表，以\0间隔，specRole标记角色名
	//最后以双\0结束
};


enum {stateOffline, stateOnline};

struct FRIEND_STATE : EXTEND_HEADER
{
	BYTE state;
	//format: char seq, \0间隔，双\0结束
};

struct FRIEND_ASSOCIATE : EXTEND_HEADER
{
	BYTE bidir;
	//format: string * 3
	//组名
	//角色名 * 2
};

struct FRIEND_ASSOCIATEBEVY : EXTEND_HEADER
{
	//format: char seq, \0间隔，双\0结束
	//组名
	//角色名列表
};

struct FRIEND_SYNCASSOCIATE : EXTEND_HEADER
{
	//format: char seq, \0间隔，双\0结束
	//组名
	//角色名列表
};


/////////////////////////////////////////////////////////////////
//extend struct

struct EXTEND_PASSTOSOMEONE : EXTEND_HEADER
{
	UINT			nameid;
	unsigned long	lnID;
	WORD			datasize;
};

struct EXTEND_PASSTOBEVY : EXTEND_HEADER
{
	WORD	datasize;
	WORD	playercount;
	//data
	//tagPlusSrcInfo vector
};

//----------------------------- tong struct -----------------------------
//帮会创建时的结构协议 来自客户端
struct STONG_CREATE_COMMAND : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btCamp;
	int	    m_btTongNameLength;
	int	    m_btPlayerNameLength;
	char	m_szBuffer[64];
	char    m_TongName[32];
	char    m_PlayerName[32];
	int     m_blevel; // 帮会等级
};
//帮会创建成功时 发回给客户端的同步信息结构
struct STONG_CREATE_SUCCESS_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btCamp;
	int 	m_btTongNameLength;
	char	m_szTongName[32];
	int     m_blevel; // 帮会等级
//	char    m_BangZhu[32];
};

struct STONG_CREATE_FAIL_SYNC : EXTEND_HEADER
{
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btFailID;
};

struct STONG_ADD_MEMBER_COMMAND : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btTongNameLength;
	BYTE	m_btPlayerNameLength;
	char	m_szBuffer[64];
};

struct STONG_ADD_MEMBER_SUCCESS_SYNC : EXTEND_HEADER
{
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btCamp;
	char	m_szTongName[32];
	char	m_szMasterName[32];
	char	m_szTitleName[32];
};

struct STONG_ADD_MEMBER_FAIL_SYNC : EXTEND_HEADER
{
	UINT	m_dwPlayerNameID;
	UINT	m_dwParam;
	BYTE	m_btFailID;
};

struct STONG_GET_TONG_HEAD_INFO_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwNpcID;
	UINT	m_dwTongNameID;
};

struct STONG_GET_TONG_ATTACK_INFO_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwNpcID;
	UINT	m_dwTongNameID;
};

struct STONG_SET_TONG_INFO_COMMAND : EXTEND_HEADER
{
	BYTE    NextProtocolID;
	UINT	m_dwParam;
	UINT	m_dwNpcID;
	UINT	m_dwTongNameID;
	char    m_title[32];
};


struct STONG_GET_MANAGER_INFO_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
};

struct STONG_GET_MEMBER_INFO_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	int		m_nParam1;  //请求的帮会名的id
	int		m_nParam2;  //请求开始的位置
	int		m_nParam3;  //请求的数量 最大为 7个
};

struct STONG_GET_LIST_INFO_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	int		m_nParam1;  //请求的帮会名的id
	int		m_nParam2;  //请求开始的位置
	int		m_nParam3;  //请求的数量 最大为 7个
};

struct STONG_ONE_LEADER_INFO
{
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];  //称号
	char	m_szName[32];   //姓名
};

struct STONG_WAR_CITY_INFO
{
	char	m_szMapName[32];	//地名称图
	int 	m_idx;			 //索引
	int 	m_mapidx;		 //编号
	char	m_Tongmaster[32];//占领帮会名
	char	m_Mastername[32];//城主
	int     m_levle;		  //城市等级
	int     m_shushou;		  //城市税收
};

struct STONG_ONE_MEMBER_INFO
{
	char	m_szName[32];
};

//帮会宣战信息同步结构
struct STONG_ATTACKSTIME_SENDBACK : EXTEND_HEADER
{
	WORD	    m_wLength;
	char 		m_szTime[64];
};

//帮会宣战信息同步结构
struct STONG_ATTACK_SENDBACK : EXTEND_HEADER
{
	WORD	    m_wLength;
	UINT	    m_dwParam;
	UINT	    m_dwNpcID;
	char		m_szAttackName[defTONG_STR_LENGTH];		    // 宣战帮会名
	char		m_szDAttackName[defTONG_STR_LENGTH];		// 宣战帮会名
};
//帮会宣战信息同步结构
struct STONG_CITY_INFO_SYNC : EXTEND_HEADER
{
	WORD m_wLength;
    int	 m_WarCityCount;
	STONG_WAR_CITY_INFO snWarInfo[7];                   //城战的其他信息
};

//帮会宣战信息同步结构
struct STONG_ATTACK_INFO_SYNC : EXTEND_HEADER
{
	WORD	    m_wLength;
	UINT	    m_dwParam;
	UINT	    m_dwNpcID;
	int         m_nTempCamp;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	char		m_szAttackName[defTONG_STR_LENGTH];		// 宣战帮会名
	int         m_AttackState;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	int         m_AttackTime;                           // 宣战的时长
//	int         m_WarCityCount;
	int         m_CurMapIdx;                            //当前帮会地图
	int         m_nLevel;
	int         m_YDeathCount;
	int         m_DDeathCount;                          //对战的死亡次数
	int         m_nAttackNum;	                            // 参战场数
	int         m_nWinNum;								// 胜利场数
	int         m_nLoseNum;								// 输掉场数
};
//帮会头信息同步结构
struct STONG_HEAD_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwParam;
	UINT	m_dwNpcID;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	int	    m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	UINT	m_dwMemberNum;
	char	m_szTongName[32];
	STONG_ONE_LEADER_INFO	m_sMember[1 + defTONG_MAX_DIRECTOR]; //0 为帮主 1-7为长老
};
//队长同步信息结构
struct STONG_MANAGER_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwParam;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	int  	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	UINT	m_dwMemberNum;
	BYTE	m_btStartNo;
	BYTE	m_btCurNum;
	char	m_szTongName[32];
	STONG_ONE_LEADER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
};
//帮众同步信息
struct STONG_MEMBER_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwParam;
	UINT	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	int 	m_btLevel;
	BYTE	m_btDirectorNum;  //长老数量
	BYTE	m_btManagerNum;   //队长数量
	UINT	m_dwMemberNum;    //帮众数量
	BYTE	m_btStartNo;      //记录上次请求的开始位置
	BYTE	m_btCurNum;       //当前记录的数量
	char	m_szTitle[32];
	char	m_szTongName[32];
	STONG_ONE_MEMBER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
};


//帮会列表信息
struct STONG_LIST_INFO_SYNC : EXTEND_HEADER
{
	WORD	m_wLength;
	UINT	m_dwParam;
	BYTE	m_btStartNo;      //记录上次请求的开始位置
	BYTE	m_btCurNum;       //当前记录的数量
	int     m_TongCount;
	STONG_ONE_MEMBER_INFO	m_sTongList[defTONG_ONE_PAGE_MAX_NUM];
};

struct STONG_INSTATE_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btCurFigure;
	BYTE	m_btCurPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szName[32];
};

struct STONG_BE_INSTATED_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;			// 网络连接号
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];
	char	m_szName[32];
};

struct STONG_BE_KICKED_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;			// 网络连接号
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_INSTATE_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btOldFigure;
	BYTE	m_btOldPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szTitle[32];
	char	m_szName[32];
};

struct STONG_KICK_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_ATTACK_COMMAND : EXTEND_HEADER
{

};

struct STONG_KICK_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_LEAVE_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_LEAVE_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	BYTE	m_btSuccessFlag;
	char	m_szName[32];
};

struct STONG_CHANGE_MASTER_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[32];
};

struct STONG_CHECK_GET_MASTER_POWER_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	UINT	m_dwTongNameID;
	char	m_szName[32];
};

struct STONG_CHANGE_MASTER_FAIL_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[32];
};

struct STONG_CHANGE_AS_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTitle[32];		// 自己的新头衔
	char	m_szName[32];		// 新帮主的名字
};

struct STONG_ACCEPT_MASTER_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID; //帮会名字ID
	BYTE	m_btFigure;
	BYTE	m_btPos;
	BYTE	m_btAcceptFalg;
	char	m_szName[32];
};

struct STONG_CHANGE_MASTER_SYNC : EXTEND_HEADER
{
	UINT	m_dwTongNameID;
	char	m_szName[32];
};

struct STONG_GET_LOGIN_DATA_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	char	m_szName[32];
};
//登录时 获取帮会信息结构
struct STONG_LOGIN_DATA_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	BYTE	m_btFlag;
	BYTE	m_btCamp;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szTongName[32];
	char	m_szTitle[32];
	char	m_szMaster[32];
	char	m_szName[32];
	UINT	m_nMoney;
	int     m_nTempCamp;                            // 帮会的临时阵营 （用于宣战）(红帮 绿帮)
	char	m_szAttackName[defTONG_STR_LENGTH];		// 被宣战的帮会名
	int     m_AttackState;                          // 宣战状态 0无宣战 1发起宣战放 2被宣战方
	int     m_AttackTime;                           // 宣战的时长
	int     m_nDeathCount;
	int     m_nLevel;
};

struct STONG_MONEY_COMMAND : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	UINT	m_dwMoney;
	char	m_szName[32];
};

struct STONG_MONEY_SYNC : EXTEND_HEADER
{
	UINT	m_dwParam;
	UINT	m_dwTongNameID;
	char	m_szName[32];
	UINT	m_nMoney;
	UINT	m_dwMoney;
};

//--------------------------- tong struct end ---------------------------


#pragma pack(pop)

#endif // __KTONGPROTOCOL_H__
