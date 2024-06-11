//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KCore_H
#define KCore_H
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

#define	UI_GAME_FONT_SONG	 "fonts/common_zh_vet.ttf"
//#define	UI_GAME_FONT_DEFAULT "fonts/common_zh_vet.ttf"  //"fonts/Marker Felt.ttf"
#define	UI_GAME_FONT_DEFAULT "fonts/VNARIAL.ttf"
#define	UI_GAME_FONT_NUMBER	 "fonts/number_16_w.fnt"

#define	UI_GAME_FONT_SONG_VN	 "fonts/syzt-e.ttf"
#define	UI_GAME_FONT_VN	 "fonts/common_zh_vet.ttf"
#define	UI_GAME_FONT_DEFAULT_VN  "fonts/common_zh_vet.ttf"  //"fonts/Marker Felt.ttf"
#define	UI_GAME_FONT_DEFAULT_VN_BTN  "fonts/btn_hydl_vet.ttf"  //"fonts/Marker Felt.ttf"
//#define	UI_GAME_FONT_NUMBER_VN	 "fonts/number_16_w.fnt"
//---------------------------------------------------------------------------
/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif*/

#ifndef ZeroStruct
#define ZeroStruct(buf)					ZeroMemory(&(buf), sizeof(buf))
#endif

#define MAX_MELEEWEAPON_PARTICULARTYPE_NUM 100   //�����������
#define MAX_RANGEWEAPON_PARTICULARTYPE_NUM 100   //�����������
#define APP_STRINFO_1 "vvlmXQy0KkYOvN9gEi0IW4kkp602Lym0" //
#define APP_STRINFO_2 "IzOINKf_Rz1T0Lvdf106klw0HxpZ4pWh"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
//stdio.h ���û�����ͷ�ļ� �����һЩ���� �������͵�xxx ��˼����û��������͵� ����

#include "engine/KbugInfo.h"
#include "engine/KEngine.h"
#include "engine/KTimer.h"
#include "engine/KGbktoUtf8.h"
#include "KProtocol.h"
#include "KProtocolProcess.h"
#include "KMagicDesc.h"
#include <map>
#include "network/ClientStage.h"
#include "gamescene/LoginDef.h"
#include "gamescene/KgameWorld.h"
#ifndef WIN32
#include "unistd.h"
#include <ctype.h>
#include <assert.h>
#endif
USING_NS_CC;
using namespace CocosDenshion;
static unsigned long gs_holdrand = time(NULL);//time(NULL)%102400;  ////��ȡϵͳʱ�䣬��λΪ��

static inline unsigned long _Rand()
{
	gs_holdrand = gs_holdrand * 244213L + 1541021L;	 //time(NULL)%102400;//
	return gs_holdrand;//ȫ�ֵ�
}

inline void RandMemSet(int nSize, unsigned char *pbyBuffer)
{
	if (nSize<=0 || pbyBuffer==NULL)
		return;

	while (nSize--)
	{
		*pbyBuffer++ = (unsigned char)_Rand();
	}
}


inline int GetRandomNumber(int nMin, int nMax) //-5 -10
{
	int nIsThis=0;
	if (nMin<0 && nMax<0)
	{
		nMin=-nMin;
		nMax=-nMax;
		nIsThis=1;
	}
	else if (nMin<0 && nMax>0)
	{
	}
	if  (nMax - nMin + 1 < 0)
		return 0;
	if  (nIsThis)
	{//��ֵ
		int nReg=g_Random(nMax - nMin + 1) + nMin;
		nReg=-nReg;
		return nReg; //���ظ�ֵ
	}
	else
		return g_Random(nMax - nMin + 1) + nMin;	   //������ֵ
}

bool InitSkillSetting();
bool InitMissleSetting();
bool InitNpcSetting();
void g_ReleaseCore();

enum GAME_OBJ_KIND
{//��Ϸui�еĸ��ڵ�
	OBJ_NODE_UI  = 1,
	OBJ_NODE_MAP,
	OBJ_NODE_MISS,
	OBJ_NODE_NPC,
	OBJ_NODE_ITEM,
	OBJ_NODE_OBJSET,
	OBJ_NODE_SHORTKILL,
	OBJ_NODE_MINMAP,
	OBJ_NODE_BIGMAP,
	OBJ_NODE_ROLESTATE,
	OBJ_NODE_CHAT,
	OBJ_NODE_STATE,
	OBJ_NODE_SKILL,
	OBJ_NODE_ITEMBOX,
	OBJ_NODE_TEAM,
	OBJ_NODE_STORE,
	OBJ_NODE_ITEMBOX_EX,
	OBJ_NODE_ITEMBOX_A,
	OBJ_NODE_ITEMBOX_B,
	OBJ_NODE_ITEMBOX_C,
	OBJ_NODE_NPCDIALOG,
	OBJ_NODE_TALKDIALOG,
	OBJ_NODE_NPCSHOP,
	OBJ_NODE_STRINGBOX,
	OBJ_NODE_SKILL_STATE,
	OBJ_NODE_NPC_OTHER,
	OBJ_NODE_NPC_BLOOD,
	OBJ_NODE_TALKDIALOG_NEW,
	OBJ_NODE_PROGRESS,
	OBJ_NODE_MOVEMAP,
	OBJ_NODE_SHORTKILL_R,
	OBJ_NODE_SHORTKILL_L,
	OBJ_NODE_LAOTOU,
	OBJ_NODE_GIVE,
	OBJ_NODE_CHATLIST,
	OBJ_NODE_AUTOPLAY,
	OBJ_NODE_PLAYFUN,
	OBJ_NODE_VIEW,
	OBJ_NODE_ITEM_SELL,
	OBJ_NODE_PLAYERSHOP,
	OBJ_NODE_TRADE,
	OBJ_NODE_DYSHOP,
	OBJ_NODE_MAXMAP,
	OBJ_NODE_ITEMLINK,
	OBJ_NODE_MOVECAVE,
	OBJ_NODE_TASK,
	OBJ_NODE_TONG,
	OBJ_NODE_ESC,
	OBJ_NODE_MISS_SPECIAL,
	OBJ_NODE_TONG_CREAT,
	OBJ_NODE_COUNT,
};


enum GAME_MAP_KIND
{//��Ϸui�еĸ��ڵ�
	MAP_NODE_CAODI  = 1,
	MAP_NODE_LUMIAN,
	MAP_NODE_LUMIAN_A,
	MAP_NODE_PART,
	MAP_NODE_TREES, //��ľ����
	MAP_NODE_HOUSE, //���ӳ�ǽ
	MAP_NODE_ABOVE, //�Ͽ� �ݶ� ��
	MAP_NODE_OTHER,
};
//Above
enum GAME_BAOVE_KIND
{//��Ϸui�еĸ��ڵ�
	BAOVE_NODE_PART  = 1,
	BAOVE_NODE_FULL,
	BAOVE_NODE_OTHER,
};

enum GAME_MINIMAP_KIND
{//��Ϸui�еĸ��ڵ�
	MINIMAP_NODE_BGSPR  = 1,
	MINIMAP_NODE_NAME,
	MINIMAP_NODE_POINT,
	MINIMAP_NODE_ROLE,
	MINIMAP_NODE_MAXMAP,
	MINIMAP_NODE_COUNT,
};

enum GAME_ROLESTATE_KIND
{//��Ϸui�еĸ��ڵ�
	ROLESTATE_NODE_BGSPR  = 1,
	ROLESTATE_NODE_LIFE,
	ROLESTATE_NODE_LAN,
	ROLESTATE_NODE_NAME,
	ROLESTATE_NODE_SAITAMA,
};

enum GAME_CHAT_KIND
{//��Ϸui�еĸ��ڵ�
	CHAT_NODE_BGSPR  = 1,
	CHAT_NODE_TILI,
	CHAT_NODE_EDIT,
	CHAT_NODE_SEND,
	CHAT_NODE_EXP,
	CHAT_NODE_PINDAO,
	CHAT_NODE_CHATLIST,
};

enum GAME_EQ_PART
{//��Ϸui�еĸ��ڵ�
	EQ_PART_HEAD  = 1,
	EQ_PART_BODY,
	EQ_PART_HAND_W,
	EQ_PART_HORSE,
	EQ_PART_PIFENG,
	EQ_PART_COUNT,
};

enum GAME_STATE_PART
{//��Ϸui�еĸ��ڵ�
	STATE_PART_HEAD  = 1, //ͷ��
	STATE_PART_BODY_Q,    //��ǰ
	STATE_PART_BODY_H,    //���
	STATE_PART_FOOT,      //�ŵ�
};
//��Ϸ�ķֲ�
enum GAME_LAYER_ORDER
{//��Ϸui�еĸ��ڵ�
	ORDER_MAP_UNDER  = 0,
	ORDER_TREE_HOUSE,
	ORDER_OBJ_MIDDLE,         
	ORDER_NPC_PLAYER,       //npc��
	ORDER_OBJ_ABOVE,        //�߿ղ�
	ORDER_UI_DIALOD,        //�Ի����
	ORDER_COUNT,
};

struct TDBShopItemData
{
	int iid;        //���idx 1��ʼ
	int iequipgen;  //
	int idetailtype;
	int iparticulartype;
	char itemName[64];//��Ʒ������
	int ilevel;     //�ȼ�
	int iseries;    //����
	UINT irandseed; //�������
	int iparam2;    //��װ
	int iparam3;
	int iparam5;
	int iparam4;
	int iparam6;
	int iparam1;
	int iparamr2;   //����
	int iparamr3;
	int iparamr5;
	int iparamr4;
	int iparamr6;
	int iparamr1;
	int ilucky;   //װ��������ֵ
	int igoldid;  //�Ƿ�ƽ�
	int	istacknum;//��������
	int ienchance;//�ӳ�
	BYTE ipoint;  //�Ƿ���װ 7Ϊ����װ 8 Ϊ����ƽ�   1-6 Ϊ��װ
	UINT addtime; //��ʼʱ��
	UINT endtime; //����ʱ��
	BYTE iPriceKind;   //��������
	int  iSellPrice;   //�����۸�
	BYTE iRongpiont;   //�Ƿ�����
	char iWonName[32]; //������
	char iaccName[32]; //�ʺ�
	int iswhere;    //�Ƿ�ϴ��װ
	int isplatina;  //�Ƿ�׽�
	BYTE isSellOut; //�Ƿ��Ѿ�����
	int iparamj1;   //(����ʹ��)
	int iparamj2;
	UINT iparamj3;
	int iparamj5;
	int iparamj4;
	int iparamj6;
	int iparamj7;
};

typedef std::map<int, TDBShopItemData> _ItemDataInfo;
typedef std::map<std::string, std::string> _StrNpcInfo;
typedef std::map<int, std::string> _StrObjInfo;
typedef std::map<std::string, std::string> _StrCoreInfo;

extern KTimer timerStart;
extern char* g_GetStringRes(int nStringID, char * szString, int nMaxLen);
extern CGameClient *nAcclient;
extern CGameClient *g_pClient;
extern int m_bIsClientConnecting;
extern int m_bIsGameSevConnecting;
extern int m_bIsPlayerInGame;
extern int _clientlanguage; 
extern bool _clientNoNetWork; 
extern BOOL  g_bUISelIntelActiveWithServer;//��ǰѡ����Ƿ���������˽���
extern BOOL  g_bUISpeakActiveWithServer;
extern int   g_bUISelLastSelCount;
extern int   nGameVersion;
extern int   nExpSkillIdx;
extern int   nLuckySkillIdx;
extern int   nKangkillIdx;
extern int   nYaoPinSkillIdx;
extern bool  nisgetinfo;
extern KPakList g_pcPakList;
extern Kgetinidata _getinidata;
extern int   nIsGaldInit;
extern bool  nIsShowShadow;
extern bool  nIsShowTitle;
extern KgameWorld  *g_GameWorld;

extern  _ItemDataInfo  iItemDataInfo;
extern  _StrNpcInfo    strNpcInfo;
extern  _StrObjInfo    strObjInfo;
extern  _StrCoreInfo   strCoreInfo;

extern KTabFile	g_OrdinSkillsSetting, g_MisslesSetting,g_PlayerTitle,g_ObjChangeName;
extern KTabFile g_SkillLevelSetting,g_FsJinMai,g_ForbitMap,g_CompEquip;
extern KTabFile g_NpcSetting,g_NpcKindFile,g_RankTabSetting;
extern KIniFile g_GameSetTing,g_NpcMapDropRate,g_MagicBaoshiDesc;
extern KTabFile cTabFile,nToolItem;
extern KIniFile g_MapTraffic;
extern KTabFile g_StringResourseTabFile;
extern KTabFile g_CompAtlas,g_GetMsg;

extern CCSize  visibleSize;
extern CCPoint origin;
extern CCSize  size;
extern std::string m_SDcardDirPath;
extern std::string m_MobileKey;
extern std::string maxMapPicPath;
extern SimpleAudioEngine *__pSound;
extern CCFileUtils* ccFileUtils;
extern bool _openSound;
extern unsigned int curMobileVer;
extern unsigned int curMobileKey;
//extern KSprCodec SprDecode;

//USING_NS_CC;
//---------------------------------------------------------------------------
/*class  KCore
{
public:
	KCore(void);
	virtual ~KCore(){};
private:

};*/
//---------------------------------------------------------------------------
// void	g_InitCodec(KCodec** ppCodec, int nCompressMethod);
// void	g_FreeCodec(KCodec** ppCodec, int nCompressMethod);
//---------------------------------------------------------------------------
#endif
