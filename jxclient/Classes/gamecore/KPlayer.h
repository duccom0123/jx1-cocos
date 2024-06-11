#ifndef KPlayerH
#define	KPlayerH

//#include "KWorldMsg.h"
//#include "KMessage.h"

#include "KInventory.h"
/*
#include "KPlayerChat.h"
*/
#include "KPlayerTong.h"

#include "KPlayerMenuState.h"
#include "KSkillManager.h"
#include "KItemList.h"
#include "KSkills.h"
#include "KNpc.h"
#include "KPlayerDef.h"
#include "KPlayerFaction.h"
#include "KPlayerTeam.h"
#include "KPlayerTask.h"
#include "KPlayerPK.h"

#include "KPlayerTrade.h"
#include "KPlayerRepute.h"
#include "KPlayerFuYuan.h"
#include "KPlayerReBorn.h"
#include "KPlayerVip.h"
#include "engine/KTabFileCtrl.h"
#define		MAX_ANSWERNUM					16 //
#define		PLAYER_LIFE_REPLENISH			0
#define		PLAYER_MANA_REPLENISH			0
#define		PLAYER_STAMINA_GAIN				1
#define		PLAYER_STAMINA_LOSS				4

#define		STRENGTH_SET_DAMAGE_VALUE		5
#define		DEXTERITY_SET_DAMAGE_VALUE		5

//#define		MAX_AVENGE_NUM					4

#define		BASE_FIRE_RESIST_MAX			150//275  //火防最大值
#define		BASE_COLD_RESIST_MAX			150//     //冰防最大值
#define		BASE_POISON_RESIST_MAX			150//     //毒防最大值
#define		BASE_LIGHT_RESIST_MAX			150//     //雷防最大值
#define		BASE_PHYSICS_RESIST_MAX			150//     //普防最大值
#define		BASE_FANGYU_ALL_MAX			    75        //原始五防御
#define		defPLAYER_SAVE_TIMEOUT			18*60*5   // 存档时间


enum UIInfo //脚本通知显示的界面类型
{
	UI_SELECTDIALOG,
	UI_TRADEDIALOG,
	UI_TALKDIALOG,
	UI_NOTEINFO,
	UI_MSGINFO,      //自右向左冒出来的信息
	UI_NEWSINFO,     //滚动新闻
	UI_PLAYMUSIC,
	UI_OPENTONGUI,
};

// 重生点位置信息
typedef struct PLAYER_REVIVAL_POS_DATA
{
	int				m_nSubWorldID;		// 重生点地图
	int				m_ReviveID;			// 重生点索引
	int				m_nMpsX;			// 重生点地图位置 x
	int				m_nMpsY;			// 重生点地图位置 y
} PLAYER_REVIVAL_POS;

typedef struct
{
	int				m_nSubWorldId;		// 传送门世界ID
	int				m_nTime;			// 传送门保持时间
	int				m_nMpsX;
	int				m_nMpsY;
} PLAYER_TOWNPORTAL_POS;

typedef struct 
{
	UINT			m_dwMapID;
	int				m_nX;
	int				m_nY;
} PLAYER_EXCHANGE_POS;


struct KplayerTimeInfo
{
	  int nMapLetTime;
	  int nExpLeftTime;
	  int nLuckLeftTime;
	  int nKangLeftTime;
	  int nYaoLeftTime;
};

//玩家老虎机结构 
struct KplayerGuzi
{
	BOOL nIsOpen;
	int  nTpye;
	int  nYingL;
	int  nJinBi;
	int  nDianShu;
	int  nBeiLv;
};


struct KplayerAuto
{
    struct	Kteam
	{
		BYTE	nIsHuiDa;
		BYTE    nIsDuiZhang;
		BYTE    nIsDuiYuan;
	}; //组队的

	struct	Kskill
	{
		BYTE	nIsOpen;
		int     nsKillId;
		int     nCostTime;
		int     nsKillRows;
		int     nsFactionTaskVal;
		//int     nSkilloop;
	};

	struct	KLRskill
	{
		BYTE	nIsOpen;
		int     nsLeftKillId;
		int     nsRightKillId;
		int     nCostTime;
		int     nsKillRows;
		int     nsFactionTaskVal;
		int     nSkilloop;
	};
	int   nSkilloop;
	DWORD nLocKnpcDw;
    int nCurXpos;
	int nCurYpos;
	int nOldIndex;   //卡怪的NPC的索引
	int nHideNpc;
	int nHidePlayer;
	int nHideMiss;
	int nAutoMoney;   //钱
	int nAutoEques;   //装备
	int nAutoMoneyAndEques;
	BYTE nShuXingNum; //属性数量
	int nAutoTuPu;    //图谱
	BYTE nAutoTool;   //道具
	BYTE nAutoOther;  //检测装备名字
	BYTE nIsAuToFenJie;
	BOOL nIsYaBiao;
	BYTE nCurNum;
	BYTE nIsHaveDo;
	BYTE nIsJiaoBen;  //是否录制脚本
	BYTE nAutoModel;
	BYTE nCurStaus;  //当前的状态
	BYTE nIsOpenZiMu;//是否自动放进子母袋
	BYTE nIsAutoSay; //是否自动发言
	BYTE nIsAutoNuQi;
	BYTE nIsAutoUseItem;//自动使用物品
	BYTE nIsShowPath;   //是否显示路线
	BYTE nIsNoBlue;     //是否不攻击蓝绿怪
	BYTE nIsNoYellow;   //是否不攻击黄怪
	BYTE nIsNoRed;      //是否不攻击红怪
	BOOL nIsFollow;     //是否跟随队长
	BOOL nIsFell;
	int  nAttackNum;
	int  _checkLoopTime;
    Kteam nCteam;
	int  nNpcCurlife;//备份的NPC生命
	int  nShiQuNum;
	int  nVal;
	int  nAutoTime;  //取消捡取的时间 延时多少秒后 取消捡取 或攻击
	int  ngMoneyLoop;
	int  ngItemLoop;
	int  ngTuzhiLoop;
	int  ngToolLoop;
	int  ngAttackLoop;
	Kskill  nAutoSkill[4];
	KLRskill nAutoLRskill;
	BOOL nIsReTurn;//是否正在回城
	BOOL nIsUseSkill;
	int  nNpcDis;  //寻怪范围
	int  nBackIndex;//备份的 选中NPC的 索引
	int  nRunLoop;
	int  nIsOpenPiick;
};

class KIniFile;

class KPlayer
{
private:
	int				m_nLeftSkillID;
	int             m_nLeftListidx;
	int				m_nLeftSkillLevel;
	int				m_nRightSkillID;
	int             m_nRightListidx;
	int				m_nRightSkillLevel;

	BOOL			m_MouseDown[2];

	UINT			m_nExtPoint;				// 活动点数

	int				m_nPhysicsSkillID;		    //当前玩家的物理攻击技能
	int				m_nPeapleIdx;
	int				m_nObjectIdx;
	int				m_nPickObjectIdx;
	int				m_nPlayerIndex;				// 本实例在 Player 数组中的位置
	//KCacheNode *	m_pLastScriptCacheNode;

public:
	UINT           m_GameLeftTime;
	BOOL            m_Isinvalid;
	int             m_nRoomFull;   //包袱是否满了

	int             m_IsTranse;                 //是否跨服
	int             m_CurGsSerIdx;              //当前服务器
    int             m_nImageNpcID;              // id npc SPR
	int				m_RunStatus;				// 是跑还是走
	DWORD			m_dwNextLevelLeadExp;		// 统率力下一级经验值
	int				m_nSendMoveFrames;			// 用于控制客户端向服务器发送移动(走或跑)协议的频率，使之不能发送大量的移动协议，减小带宽压力
	//char            szClienResult[256];       // 脚本返回值全局变量
	KTabFile    nPos,nYaBiao,nEquipItem,nCheckName;
	KIniFile	m_FileDesc;//,nEquipItem,nToolItem,nCheckName;
	KTabFile    nGuaSkill;
	KTabFileCtrl nMapPos;

	DWORD           m_ItemLinkDwid;            //当前装备链接的物品ID
	TMissionLadderSelfInfo m_MissionData;
	TMissionLadderInfo m_MissionRank[MISSION_STATNUM];

	int             m_ExpTime;                  //  当前领取经验时间
	int             m_YinLiangTime;             //  当前领取银两时间
	int             m_JinBiTime;                //  当前领取金币时间
	int             m_ExpPiontTime;             //  当前领取点卡时间
	KplayerAuto		m_Autoplay;					// 玩家任务系统(变量)
	int				m_DebugMode;			    //调试模式

	int             m_nCurPinDao;  //临时频道
	enum
	{
		FF_CHAT = 0x01,
	};
	BYTE			m_nForbiddenFlag;			// 禁止发言标志
	KIndexNode		m_Node;
	GUID			m_Guid;                     //网络序号
	BOOL			m_bExchangeServer;          //是否跨服状态

	UINT			m_dwID;						// 玩家的32位ID
	int				m_nIndex;					// 玩家的Npc编号
	int				m_nNetConnectIdx;			// 第几个网络连接
	KItemList		m_ItemList;					// 玩家的装备列表
	BuySellInfo		m_BuyInfo;					// 进行的交易列表
	//NewBuySellInfo	m_NewBuyInfo;				// 进行的交易列表
	KPlayerMenuState	m_cMenuState;			// 是否处于交易或队伍开放状态
	KTrade			m_cTrade;					// 交易模块
	int				m_nAttributePoint;			// 未分配属性点
	int				m_nSkillPoint;				// 未分配技能点
	int 			m_nChestPW;		            //密码 已经给 城市地图 使用
	int				m_nStrength;				// 玩家的基本力量（决定基本伤害）
	int				m_nDexterity;				// 玩家的基本敏捷（决定命中、体力）
	int				m_nVitality;				// 玩家的基本活力（决定生命、体力）
	int				m_nEngergy;					// 玩家的基本精力（决定内力）
	int				m_nLucky;					// 玩家的基本运气（影响得到装备的好坏）
	BYTE			m_btChatSpecialChannel;		
	int				m_nUpExp;                   // 杀死敌人获得经验增加百分比

	int				m_nCurStrength;				// 玩家的当前力量（决定基本伤害）   力量
	int				m_nCurDexterity;			// 玩家的当前敏捷（决定命中、体力） 身法
	int				m_nCurVitality;				// 玩家的当前活力（决定生命、体力） 外功
	int				m_nCurEngergy;				// 玩家的当前精力（决定内力）       内功
	int				m_nCurLucky;				// 玩家的当前运气（影响得到装备的好坏）
	int				m_nTempLucky_p;				// 记录玩家的临时装备的总幸运百分比
    BYTE	        m_nVipType;                 // 会员的类型
//	int             m_nZhonji;                  // 当前重击
//	int             m_nDangge;                  // 当前档格

	int 			m_nExp;						// 当前经验值(当前等级在npc身上)
	int				m_nNextLevelExp;			// 下一级经验值
	BYTE			m_CUnlocked;                // 客户端是否解屏 屏幕

	UINT			m_dwLeadExp;				// 统率力经验值
	UINT			m_dwLeadLevel;				// 统率力等级
	char			m_PlayerName[32];           // 玩家的角色名字
	//char			m_PeiouName[32];            // 配偶名字

	KPlayerTeam		m_cTeam;					// 玩家的组队信息
	KPlayerFaction	m_cFaction;					// 玩家的门派信息

//	KPlayerChat		m_cChat;                    // 玩家头顶说话显示

	KPlayerTask		m_cTask;					// 玩家任务系统(变量)
    
//	KplayerGuzi		m_GuziGame;	                // 玩家的老虎机系统

	KplayerTimeInfo m_MapTimeInfo;			    // 幸运草 长效药存档

	KPlayerPK		m_cPK;						// PK关系处理
	
	KPlayerRepute	m_cRepute;					//声望

	KPlayerFuYuan	m_cFuYuan;					//福源

	KPlayerReBorn	m_cReBorn;					//转生

	KPlayerVip	    m_cVip;					    //会员

	KPlayerTong		m_cTong;					// 自己的帮会信息
	
	DWORD			m_dwDeathScriptId;			// 死亡脚本

	char			m_szTaskAnswerCanshu[MAX_ANSWERNUM][32];  //玩家 所带的 脚本参数
	char			m_szTaskAnswerFun[MAX_ANSWERNUM][32];
	int				m_nAvailableAnswerNum;                    //当前选择界面下，最大回答数。
	int				m_nWorldStat;    // 排名
	int				m_nSectStat;     // 帮会排名
	bool            m_isopenjuli;

	DWORD			m_nGiveCallBackScript;
	BYTE			m_nGiveCallBackNum;

public:
	KPlayer();
	~KPlayer();

	void			SetPlayerIndex(int nNo);					// 设定 m_nPlayerIndex
	void			GetAboutPos(KMapPos *pMapPos);			    // 获得玩家附近一个空位置
	void			GetNewAboutPos(KMapPos *pMapPos);
	int				GetPlayerIndex();							// 获得本实例在 Player 数组中的位置
	UINT			GetPlayerID(){return m_dwID;};
	void			GetFactionName(char *lpszName, int nSize);	// 获得当前门派名称 not end
	void			ChatFriendOnLine(DWORD dwID, int nFriendIdx);// 获得通知某好友上线了
	BOOL			ExecuteScriptA(char * ScriptFileName, char * szFunName, int nParam = 0,char * szCanshu=NULL,BOOL nIsGive=TRUE); //NPC对话框调用
	BOOL			ExeNoBackScript(char * ScriptFileName, char * szFunName,int nKind = 0,int nParama = 0,int nParamb = 0,int nParamc = 0);
	BOOL			ExecuteScriptC(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=TRUE);
	BOOL			ExecuteScriptD(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inval=0,int invala=0,BOOL nIsGive=FALSE);
	BOOL			ExecuteScriptB(DWORD dwScriptId,  char * szFunName, int nParam=0,char * szCanshu=NULL,BOOL nIsGive=TRUE);
	BOOL			DoScript(char * ScriptCommand);				//执行某个脚本指令
	BOOL			ExeItemScriptA(char * ScriptFileName, char * szFunName, char * szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
	BOOL			ExeItemScript(DWORD dwScriptId, char * szFunName, char *  szParams=NULL,char * snParams=NULL,int inParam = 0,int njb=0,int njxb=0,int inGenre=0,int inDetail=0,int inParticular=0,int inStackNum=0,int inItemX=0,int inItemY=0);
    void            myPrintLuaStackA(lua_State *L);  //调试
	void			ChangeCurStrength(int nData);			// 改变当前力量(当 nData 小于 0 时，减少)
	void			ChangeCurDexterity(int nData);			// 改变当前敏捷(当 nData 小于 0 时，减少)
	void			ChangeCurVitality(int nData);			// 改变当前活力(当 nData 小于 0 时，减少)
	void			ChangeCurEngergy(int nData);			// 改变当前精力(当 nData 小于 0 时，减少)
//	BOOL			ExecuteScriptC(char * ScriptFileName);
	void			Release();
	void			Active();								// 玩家每次游戏循环都需要处理的东西
//	void			ProcessMsg(KWorldMsgNode *lpMsg);		// 处理世界消息，转为NPC命令
    void            ReSkillEnhance();
	LPSTR			GetPlayerName() { return m_PlayerName; };

	BOOL			NewPlayerGetBaseAttribute(int Series);	// 新玩家登陆时根据五行属性产生 力量 敏捷 活力 精力 四项数值
	void			AddBaseLucky(int nData);				// 增加基本运气

	void			DirectAddExp(int nExp);					// 直接增加经验值，不考虑其他因素
	void			LevelUp();								// 升一级
	void			AddLeadExp(int nExp);					// 增加统率力经验
	void			UpdataCurData(int nIsLoign=0,int nUnEquip=0);
	void			ReCalcEquip();		    // 重新计算身上的装备
	void			ReCalcState(int nIsLoign=0);							// 重新计算身上的状态
	BOOL            CheckThisItem(int nSetings,char *nKey);
	void			ChangePlayerCamp(int nCamp);			// 改变玩家阵营
	void 			Revive(int nType);						// 重生	
	void			GetEchoDamage(int* nMin, int* nMax, int nType);// 获取界面需要显示的伤害值
	BOOL			CheckTrading();
	void			SetFirstDamage();   //设置基本伤害
	void			SetBaseAttackRating();
	void			SetBaseDefence();
	void			SetBaseResistData();
	void			SetBaseSpeedAndRadius();
	int				GetPhysicsSkillId(){return m_nPhysicsSkillID;};
	void			SetPhysicsSkillId(int nPhysicsSkillID)
	{
		if (nPhysicsSkillID <= 0) return ;
		ISkill * pISkill =  g_SkillManager.GetSkill(nPhysicsSkillID, 1);
		if (!pISkill) 
            return ;
		
        if (nPhysicsSkillID > 0 && pISkill->IsPhysical())
			m_nPhysicsSkillID = nPhysicsSkillID;
	};
	int				GetLeftSkill(){return m_nLeftSkillID;};
	int			    GetLeftSkillListidx(){return m_nLeftListidx;};
	int				GetRightSkill(){return m_nRightSkillID;};
	int			    GetRightSkillListidx(){return m_nRightListidx;};

	void            SetExtGameTime(DWORD nPoint);

	int				GetTargetNpc() { return m_nPeapleIdx; };
	int				GetTargetObj() { return m_nObjectIdx; };
	void			SetTargetNpc(int n) { m_nPeapleIdx = n; };
	void			SetTargetObj(int n) { m_nObjectIdx = n; };
	void			FindSelectNpc(int x, int y, int nRelation);
	void			FindSelectObject(int x, int y);
	void			Walk(int nDir, int nSpeed);	
	void			TurnLeft();
	void			TurnRight();
	void			TurnBack();
	void			DrawSelectInfo();
	BOOL			ConformIdx(int nIdx);	
//	void			GetEchoDamage(int* nMin, int* nMax, int nType);// 获取界面需要显示的伤害值
	void			ProcessInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);// 处理键盘鼠标消息
	void			RecvSyncData();								// 接收同步数据
    int             ClientExeItemScript(char *nScriptPath,char *nFunName,int nPaparm,int ninVal,char *nstrVala);
	void			ApplyTeamInfo(DWORD dwNpcID);				// 向服务器申请查询某个npc所在队伍的信息
	void	        ApplyRoomInfo(int nIsFull);
	void			ApplySelfTeamInfo();						// 向服务器申请查询玩家自身的队伍情况
	BOOL			ApplyCreateTeam();//char *lpszTeamName);		// 玩家向服务器申请创建队伍
	BOOL			ApplyTeamOpenClose(BOOL bFlag);				// 队长向服务器申请开放、关闭队伍是否允许加入成员状态
	void			ApplyAddTeam(int nNpcIndex);				// 玩家向服务器申请加入某个队伍
	void			AcceptTeamMember(DWORD dwNpcID);			// 玩家通知服务器接受某个npc为队伍成员
	void			TeamDropApplyOne(DWORD dwNpcID);			// 队长删除加入队伍申请列表中的某个npc
	void			LeaveTeam();								// 通知服务器本玩家离开队伍
	void			TeamKickMember(DWORD dwNpcID);				// 队长通知服务器踢除某个队员
	void			ApplyTeamChangeCaptain(DWORD dwNpcID);		// 队长向服务器申请把自己的队长身份交给别的队员
	void			ApplyTeamDismiss();							// 队长向服务器申请解散队伍
	void			ApplySetPK(BOOL bPK);						// 玩家向服务器申请打开、关闭pk开关
	void			ApplyFactionData();							// 玩家向服务器申请门派数据
	void			SendChat(KUiMsgParam *pMsg, char *lpszSentence);// 客户端发送聊天语句给服务器
	void			ApplyAddBaseAttribute(int nAttribute, int nNo);// 队长向服务器申请增加四项属性中某一项的点数(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	BOOL			ApplyAddSkillLevel(int nSkillID, int nAddPoint);// 向服务器申请某个技能升级
	BOOL			ApplyUseItem(int nItemID, ItemPos SrcPos);	// 向服务器申请使用某个物品（鼠标右键点击该物品）
	BOOL	        AutoChangeItem(int nItemID, ItemPos SrcPos,ItemPos DestPos);
	BOOL            ApplyAutoMoveItem(int nItemID, ItemPos SrcPos,int destPlace);
	void	        PickUpItem(int nObjIndex,int rid);
    void	        PickUpMoney(int nObjIndex,int rid);
	void			PickUpObj(int nObjIndex);					// 客户端鼠标点击obj检起某个物品，向服务器发消息
	void			ObjMouseClick(int nObjIndex);				// 客户端鼠标点击obj，向服务器发消息
	BOOL			MoveItem(ItemPos DownPos, ItemPos UpPos,int nIsComp=0);	// DownPos 不能是手，UpPos 必须是手
	void			MoveSkill(ItemPos DownPos, ItemPos UpPos);
	int				ThrowAwayItem();
	int	            AutoAwayItem(DWORD nItmeDwIdx,bool isAttack=true);  //---------------外挂丢东西-------------------
	void			ChatAddFriend(int nPlayerIdx);				// 客户端通过别人的添加聊天好友的申请
	void			ChatRefuseFriend(int nPlayerIdx);			// 客户端拒绝别人的添加聊天好友的申请
	void			TradeApplyOpen(char *lpszSentence, int nLength);
	void			TradeApplyClose();
	void			TradeApplyStart(int nNpcIdx);
	BOOL			TradeMoveMoney(int nMoney);					// 交易时输入自己的钱	
	void			TradeDecision(int nDecision);				// if nDecision == 0 推出交易  if nDecision == 1 确定交易  if nDecision == 2 取消交易确定
	void			TradeApplyLock(int nLockOrNot);				// 交易锁定或取消锁定

	void			SetChatCurChannel(int nChannelNo);			// 设定当前聊天频道
	void			TeamInviteAdd(DWORD dwNpcID);				// 邀请加入队伍

	void			SetLeftSkill(int nSkillID,BOOL nIsNoLogin=FALSE);
	void			SetRightSkill(int nSkillID);
	void	        SetShiquSkill(int nSkillID);
	void			UpdateWeaponSkill();
//	int				GetLeftSkill(){return m_nLeftSkillID;};
//	int				GetRightSkill(){return m_nRightSkillID;};
	void			SetDefaultImmedSkill();
	void			s2cApplyAddTeam(BYTE* pProtocol);			// 收到服务器通知有人申请加入队伍
	void			s2cTradeChangeState(BYTE* pMsg);
	void			s2cTradeMoneySync(BYTE* pMsg);
	void			s2cTradeDecision(BYTE* pMsg);				// 收到服务器通知交易完成或取消
	void			SyncCurPlayer(BYTE* pMsg);
	void			s2cLevelUp(BYTE* pMsg);
	void			s2cGetCurAttribute(BYTE* pMsg);
	void			s2cSetExp(int nExp);
	void			s2cSyncMoney(BYTE* pMsg);
	void			s2cSyncXu(BYTE* pMsg);
	void			s2cTradeApplyStart(BYTE* pMsg);
	void			CheckObject(int nIdx);

	UINT  			GetExtPoint();
private:
	void			CalcCurLucky();							// 计算当前运气
	void			LevelAddBaseLifeMax();					// 等级升一级后增加最大生命点
	void			LevelAddBaseManaMax();					// 等级升一级后增加最大内力点
	void			LevelAddBaseStaminaMax();				// 等级升一级后增加最大体力点

	void			SendFactionData();						// 向客户端发送门派数据

	void			SyncCurrentBaseAttriibute(int type,int attribute,int curAttribute);	// ham moi
	void			AddBaseStrength(int nData);				// 增加基本力量  ---力量
	void			AddBaseDexterity(int nData);			// 增加基本敏捷  ---身法
	void			AddBaseVitality(int nData);				// 增加基本活力---外功
	void			AddBaseEngergy(int nData);				// 增加基本精力---内功
	
	void			ResetBaseStrength(int nData);			// 增加基本力量
	void			ResetBaseDexterity(int nData);			// 增加基本敏捷
	void			ResetBaseVitality(int nData);			// 增加基本活力
	void			ResetBaseEngergy(int nData);			// 增加基本精力
	
	void			SetNpcPhysicsDamage();					// 由当前力量计算对应npc的物理伤害(PhysicsDamage)
	void			SetNpcAttackRating();					// 由当前敏捷计算对应npc的攻击命中率(AttackRating)
	void			SetNpcDefence();						// 由当前敏捷计算对应npc的防御力
//	void			SetNpcWalkSpeed();						// 由当前敏捷计算对应npc的行走速度
//	void			SetNpcRunSpeed();						// 由当前敏捷计算对应npc的跑步速度
	void			ProcessMouse(int x, int y, int Key, MOUSE_BUTTON nButton);
	void			OnButtonUp(int x,int y,MOUSE_BUTTON nButton);				// 处理鼠标键抬起
	void			OnButtonDown(int x,int y,int Key,MOUSE_BUTTON nButton);		// 处理鼠标键按下
	void			OnButtonMove(int x,int y,int Key,MOUSE_BUTTON nButton);		// 处理鼠标键按下后移动
	void			OnMouseMove(int x,int y);									// 处理鼠标移动
	int				NetCommandPlayerTalk(BYTE* pProtocol);

	//Question:为单机测试版使用
	//friend		    int LuaInitStandAloneGame(Lua_State * L);


// 豆豆的东西
private:
	void			S2CExecuteScript(char * ScriptName, char * szParam);
public:
	void			SetNpcDamageAttrib();
	void			DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo); //通知该客户端显示某个UI界面
	void			ProcessPlayerSelectFromUI(BYTE* pProtocol);			// 处理当玩家从选择菜单选择某项时的操作

	void			DialogNpc(int nIndex,BOOL nIsServer=TRUE);
	void            OpenBaiTan(DWORD nIndwId);
	void			OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo);//当玩家从选择框中选择某项后，将向服务器发送			
	void			OnScriptAction(PLAYER_SCRIPTACTION_SYNC * );

};

extern KPlayer	*Player;//Player[MAX_PLAYER];

#endif //KPlayerH

