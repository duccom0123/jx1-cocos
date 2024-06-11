//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItem.h
// Date:	2002.08
// Code:	LiXueWu, DongBo
// Desc:	Header File
//---------------------------------------------------------------------------

#ifndef	KItemH
#define	KItemH

#include	"KBasPropTbl.h"
#include	"KMagicAttrib.h"
#include	"GameDataDef.h"
#include	"engine/KTabFile.h"
#include    "KRUImage.h"

class KPlayer;
class KIniFile;

#define		IN
#define		OUT

#define		MAX_ITEM	1024    //150000    

#define		KItemNormalAttrib KMagicAttrib

#define		DESCMAXLEN	516		// 典型的字符串缓冲区长度

#define	 TABFILE_MAGICATTRIB_PATH	"\\settings\\item\\004\\magicattrib.txt" //蓝装魔法属性
#define	 TABFILE_MAGICAGOLD_PATH	"\\settings\\item\\004\\GoldMagic.txt"   //黄金魔法属性
#define	 TABFILE_MAGICALEVEL_PATH	"\\Settings\\item\\004\\magicattriblevel_index.txt"
#define	 TABFILE_FUSION_PATH	    "\\Settings\\item\\004\\fusion.txt" 
#define	 TABFILE_BIANSHI_PATH	    "\\Settings\\item\\004\\bianshi.txt" 
#define	 JIACHENG_VAL	2	    // 强化的最小值	18
#define	 MAX_STACKNUM	200		// 典型的字符串缓冲区长度  nStackNum

enum ITEMGENRE
{
	item_equip = 0,			// 装备
	item_medicine,			// 药品
	item_mine,				// 矿石
	item_materials,			// 药材
	item_task,				// 任务
	item_townportal,		// 传送门
	item_fusion,            // 纹纲
	item_number,			// 类型数目
};

enum EQUIPDETAILTYPE
{
	equip_meleeweapon = 0,
	equip_rangeweapon,
	equip_armor,
	equip_ring,
	equip_amulet,
	equip_boots,
	equip_belt,
	equip_helm,
	equip_cuff,
	equip_pendant,
	equip_horse,
	equip_mask,	// mat na
    equip_pifeng,
	equip_yinjian,
	equip_shiping,
	equip_detailnum,
};

enum MEDICINEDETAILTYPE
{
	medicine_blood = 0,   //吃药的类型  血
	medicine_mana,        //内
	medicine_both,        //血和内
	medicine_stamina,     //体力
	medicine_antipoison,  //
	medicine_allboth=8,
	medicine_detailnum,
};
//装备的共同属性
typedef struct
{
	int		nItemGenre;				// 道具种类 (武器? 药品? 矿石?)
	int		nDetailType;			// 在该种类中的具体类别
	int		nParticularType;		// 详细类别
	int		nObjIdx;				// 地图上摆放时对应的物件数据编号
	BOOL	bStack;					// 是否可叠放
	int		nWidth;					// 道具栏中所占宽度
	int		nHeight;				// 道具栏中所占高度
	int		nPrice;					// 商店购买价格
	int		nLevel;					// 等级
	int		nSeries;				// 五行属性
	char	szItemName[80];			// 名称
	//LPCSTR	szItemName;
	char	szWonName[32];			// 所有者名称
	//LPCSTR	szWonName;
	int 	nColor;                 // 颜色
	int     nLianjieFlg;            // 链接状态
	int     nCanUse;                // 是否可以使用
	BYTE    nSellModel;             // 摆摊交易的模式
	//客户端
	//char	szImageName[80];		 // 界面中的动画文件名
	LPCSTR  szImageName;
	char	szIntro[DESCMAXLEN];	 // 说明文字  物品的说明文字
	//LPCSTR 	szIntro;
	int		uPrice;                  // 摆摊购买价格
	BYTE    uLianjieFlg;             // 是否正在连接
//	int     uCanUse;                 // 是否可以使用
    BYTE    ncSellModel;             // 摆摊交易的模式

	//char    ItmeInfo[DESCMAXLEN];    // 说明文字
	LPCSTR  ItmeInfo;
	//char	szScript[256];			 // 执行脚本
	LPCSTR  szScript;
	int		nSet;					 // 套装
	int		nSetId;					 // 当前 穿戴套装的Id
	int		nSetNum;				 // 套装数量
	int		nBigSet;				 // 套装扩展
 	int		nGoldId;				 // 黄金Id
	int		nIsPlatina;				 // 是否白金装
	int		nStackNum;               //叠放数量
	int		nEnChance;               //强化
	int		nPoint;					 //紫装用
	KTime	LimitTime;               //限时
	int		nIsSell;                 //是否可以买卖
	int		nPriceXu;                //金币
	int     nJiFen;                  //积分
	int		nIsTrade;                //是否可以交易
	int		nIsDrop;                 //是否可以丢弃
//    int	    nYingNuma;                //隐藏属性1
//    int	    nYingNumb;                //隐藏属性2
	int     nRongNum;                 //可溶属性数量
    int     nWengangPin;              //可溶纹钢品质
	int     nBinfujiazhi;             //兵富甲值
    int	    nRongpiont;               //熔炼用
	int     nIsBang;                  //是否绑定
	int     nIsKuaiJie;               //是否快捷栏
	int     nSkillType;               //技能类型
	int     nISMagic;                 //是否带属性
	int     nMagicID;                 //纹钢的技能ID
	int     nIsUse;                   //是否捡到立即使用
	int     nIsWhere;                 //是第几行
	int     nChiBangRes;              //翅膀的外观序号
	int     nParticularTypea;         //类型2
	int     nUseMap;
	int     nSixSkill;
	int     nTenSkill;
	int     nRes;                     //改变的装备的外观
	int		nUseKind;				  //使用的类型

} KItemCommonAttrib;
//装备魔法属性生成的核心参数
typedef struct
{
	UINT	uRandomSeed;         // 随机种子
	int		nGeneratorLevel[6];  // 魔法
	int		nRGeneratorLevel[6]; // 熔炼
	//int	nBGeneratorLevel[6]; // 宝石
	int		nJGeneratorLevel[7]; // 基础属性
	int		nVersion;            // 版本
	int		nLuck;               // 幸运值

//	UINT   uItemRandomSeed[12];  //储存每个属性的 随机种子 0-5 为 蓝装,5-10,熔炼
} KItemGeneratorParam;

class KNpc;

class KItem
{
public:
	KItem();
	~KItem();

// 以下是核心成员变量
private:
	KItemCommonAttrib	m_CommonAttrib;			// 各种道具的共同属性
	KItemNormalAttrib	m_aryBaseAttrib[7];		// 道具的基础属性
	KItemNormalAttrib	m_aryRequireAttrib[6];	// 道具的需求属性
	KItemNormalAttrib	m_aryMagicAttrib[6];	// 道具的魔法属性  //存放着属性的字符串
    KItemNormalAttrib	m_ronMagicAttrib[6];	// 道具的熔炼属性
//	KItemNormalAttrib	m_BaoShiMagicAttrib[6];	// 道具的宝石属性
	KItemNormalAttrib	m_yinMagicAttrib[2];	// 道具的隐藏属性
	KItemNormalAttrib	m_TempPlatinaAttrib[6];	// 临时的白金魔法属性值（用于改变属性）
	KItemNormalAttrib	m_TempMagicAttrib[6];	// 临时的魔法属性值（用于强化）
	KItemNormalAttrib	m_TempRMagicAttrib[6];  // 临时的辨识纹钢属性值（用于强化）
//	KItemNormalAttrib	m_TempBMagicAttrib[6];  // 临时的宝石属性值（用于强化）
// 以下是辅助性的成员变量
public:
	KItemGeneratorParam	m_GeneratorParam;		// 道具的生成参数
private:
	int		m_nIndex;							// 自身在道具数组中的索引
	DWORD	m_dwID;								// 独立的ID，用于客户端与服务器端的交流
	int		m_nCurrentDur;						// 当前耐久度
	KRUImage	m_Image;
// 以下是对外接口
public:
	void	ApplyMagicAttribToNPC(IN KNpc*, IN int = 0 ,IN int *nType=NULL) const;
	void	RemoveMagicAttribFromNPC(IN KNpc*, IN int = 0,IN int *nType=NULL) const;
	void	ApplyHiddenMagicAttribToNPC(IN KNpc*, IN int) const;
	void	RemoveHiddenMagicAttribFromNPC(IN KNpc*, IN int) const;
	KItemGeneratorParam * GetItemParam(){return &m_GeneratorParam;};
	void	SetID(DWORD dwID) { m_dwID = dwID; };
	DWORD 	GetID() const { return m_dwID; };// DWORD CHECK O DAY
	int     GetLianjieFlg() const {return m_CommonAttrib.nLianjieFlg;};  //链接标示
	int     GetIsCanUse()const {return m_CommonAttrib.nCanUse;};         //获取是否能使用
	void	SetIsCanUse(int uCanUse) {m_CommonAttrib.nCanUse = uCanUse;};
	int     ExtractChars(const char *inStr,int scrLen,char *outStr);
	int		GetDetailType() const { return m_CommonAttrib.nDetailType;};
	int		GetGenre() const { return m_CommonAttrib.nItemGenre; };
	int		GetUseMapIdx() const { return m_CommonAttrib.nUseMap; };
	void	SetUseMapIdx(int  nUseMap) { m_CommonAttrib.nUseMap=nUseMap;};

	int		GetUseKind() const { return m_CommonAttrib.nUseKind; };
	void	SetUseKind(int  nUseKind) { if (nUseKind<0) nUseKind = 0 ; if (nUseKind>1) nUseKind = 1 ;  m_CommonAttrib.nUseKind=nUseKind;}

	int		GetItemResIdx() const { return m_CommonAttrib.nRes; };
	void	SetItemResIdx(int  nResIdx) { m_CommonAttrib.nRes=nResIdx;};

	int		GetIsUse() const { return m_CommonAttrib.nIsUse; };
	int		GetIsSell() const { return m_CommonAttrib.nIsSell; };
	int		GetIsTrade() const { return m_CommonAttrib.nIsTrade; };
	int		GetIsDrop() const { return m_CommonAttrib.nIsDrop; };

	int		GetParticular() { return m_CommonAttrib.nParticularType;};
	int		GetParticular2() { return m_CommonAttrib.nParticularTypea;};
	int		GetChiBangRes() { return m_CommonAttrib.nChiBangRes; };
	void	SetChiBangRes(int nRes) { m_CommonAttrib.nChiBangRes = nRes;};
	//int	GetLParticular() { return m_CommonAttrib.nParticularType; };
	int		GetLevel() { return m_CommonAttrib.nLevel; };
	void	SetSeries(int nSeries) { m_CommonAttrib.nSeries = nSeries; };
	int   	GetSeries() const { return m_CommonAttrib.nSeries; };
	// int		GetWidth() const { return m_CommonAttrib.nWidth; };
	// int		GetHeight() const { return m_CommonAttrib.nHeight; };
	int		GetWidth() const { return 1; };
	int		GetHeight() const { return 1; };
	int		GetPrice() const { return m_CommonAttrib.nPrice; };
	int		GetPriceXu() const { return m_CommonAttrib.nPriceXu;};
	char*	GetName() const { return (char *)m_CommonAttrib.szItemName; };
	char*	GetImagePath() const { return (char *)m_CommonAttrib.szImageName; };
	char*	GetWonName() const { return (char *)m_CommonAttrib.szWonName; };
	void    SetWonName(char *strWonName){t_sprintf(m_CommonAttrib.szWonName,strWonName);};  //设设置归属
	int		GetObjIdx() { return m_CommonAttrib.nObjIdx;};
	void*	GetRequirement(IN int);
	BOOL    CheckRequirement(IN int nReq);
	int		GetMaxDurability();
	int		GetTotalMagicLevel();
	int		GetRepairPrice();                                           //获取修理的总价格
	void	Remove();
	void    SetIndex(int i);
	int     GetIndex(){ return m_nIndex;};
	BOOL	SetBaseAttrib(IN const KItemNormalAttrib*);
	BOOL	SetRequireAttrib(IN const KItemNormalAttrib*);
	BOOL	SetMagicAttrib(IN const KItemNormalAttrib*);
	void	SetDurability(IN const int nDur) {m_nCurrentDur = nDur;};   //设置持久度
	int		GetDurability() { return m_nCurrentDur;};                   //返回当前持久度
	//KItemGeneratorParam*	GetGeneratorParam() { return &m_GeneratorParam; }
	int		Abrade(IN const int nRange);
	BOOL	CanBeRepaired();                                             //检查是否能修复持久度
    int     GetIsBang() const { return m_CommonAttrib.nIsBang; };        //是否绑定
	void    SetBang(int nVal=0)  { m_CommonAttrib.nIsBang = nVal; };     //设置绑定
	int     GetIsKuaiJie() const { return m_CommonAttrib.nIsKuaiJie;};   //是否快捷栏
	void    SetKuaiJie(int nVal=0) { m_CommonAttrib.nIsKuaiJie=nVal;};   //设置快捷栏
	int     GetIsMagic() const { return m_CommonAttrib.nISMagic; };      //是否魔法属性	
	void    SetMagic(int nVal=0) { m_CommonAttrib.nISMagic=nVal;};       //() const { return m_CommonAttrib.nISMagic; };      //是否魔法属性	
	
	int     GetSkillType() const { return m_CommonAttrib.nSkillType; };  //纹钢在可纹钢属性列表的行号m_SkillType
	void    SetSkillType(int nVal=0) {m_CommonAttrib.nSkillType=nVal;};

	int     GetMagicID() const { return m_CommonAttrib.nMagicID; };  	 //物品所带的技能id
	void    SetMagicID(int nVal=0) {m_CommonAttrib.nMagicID=nVal;};

	int		GetSet() const { return m_CommonAttrib.nSet; };              //是否套装装备
	int		GetSetId() const { return m_CommonAttrib.nSetId; };          //套装ID
	int		GetSetNum() const { return m_CommonAttrib.nSetNum; };        //套装数量
	int		GetGoldId() const { return m_CommonAttrib.nGoldId; };
	void	SetGoldId(int nId) {m_CommonAttrib.nGoldId=nId;};

	int		GetIsPlatima() const { return m_CommonAttrib.nIsPlatina; };
	void	SetPlatima(int nId) {m_CommonAttrib.nIsPlatina=nId; };

	//void    SetSItmeInfo(char *SItemInfo) {ZeroMemory(m_CommonAttrib.ItmeInfo,sizeof(m_CommonAttrib.ItmeInfo));_snprintf(m_CommonAttrib.ItmeInfo,sizeof(m_CommonAttrib.ItmeInfo)-1,"%s",SItemInfo);};  //设设置说明
	void    SetSItmeInfo(char *SItemInfo){m_CommonAttrib.ItmeInfo = SItemInfo;};
	char*	GetSItmeInfo() const {return (char *)m_CommonAttrib.ItmeInfo;};
	void    SetItemInfo(int nVale);
    int	    GetColor()  const { return m_CommonAttrib.nColor; };
	int		GetBigSet() const { return m_CommonAttrib.nBigSet; };
	int		GetRongNum() const { return m_CommonAttrib.nRongNum; };       //可熔炼数量
	int		GetWengangPin() const { return m_CommonAttrib.nWengangPin; }; //获取纹纲的品质
	int		GetBinfujiazhi() const { return m_CommonAttrib.nBinfujiazhi;};
	int     GetUsedRong()
	{ 
		int mRmun=0;
	    for (int i=0;i<6;i++)
		{ 
		    if (m_ronMagicAttrib[i].nAttribType>0) 
			     mRmun++;  
		}
		return mRmun;
	};

	int     GetMagicCount()
	{ 
		int mRmun=0;
		for (int i=0;i<sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]);i++)
		{ 
			if (m_aryMagicAttrib[i].nAttribType>0) 
				mRmun++;  
		}
		return mRmun;
	};


	int     getBaseCount()
	{ 
		int ncount=0;
		for (int i=0;i<sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]);i++)
		{ 
			if (m_aryBaseAttrib[i].nAttribType>0) 
				ncount++;  
		}
		return ncount;
	};
	int     getReqCount()
	{ 
		int ncount=0;
		for (int i=0;i<sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);i++)
		{ 
			if (m_aryRequireAttrib[i].nAttribType!=0) 
				ncount++;  
		}
		return ncount;
	};
	int 	IsPurple();
	int 	GetPoint();
	int     GetIsWhere();
	BOOL    CheckReqability(int n);
	int 	IsRong();
	int     IsBlue();
	int     getItemKind();
	int     IsGold();
	int     IsPlatina();
    int	    IsReceptacle(); // 检查是否容器
    BOOL    HoldItem(int nIdx, int nWidth, int nHeight);
    int		Stack( int nIdx);  // 叠加
    BOOL    CheckItemInfo(int nGen,int nDetail,int nParticular,char *KeyName);
	BOOL	CanStack( int nOldIdx); 
    int     IsStack() const {return m_CommonAttrib.bStack;};//是否可以叠放
	int		GetStackNum() const { return m_CommonAttrib.nStackNum;};
	void	SetStackNum(int nNum) {m_CommonAttrib.nStackNum = nNum;};  //可叠加的数量
	void	SetPerXu(int nPerXu) {m_CommonAttrib.nPriceXu = nPerXu;};
	void	SetPerPrice(int nmPrice) {m_CommonAttrib.nPrice = nmPrice;};

	int		GetEnChance() const { return m_CommonAttrib.nEnChance;};
	int		EnChance(int nEnChance = 0);
    int	    EnChanceRong(int nEnChance = 0);
	int     EnChanceBao(int nEnChance = 0);//宝石加成
	void    SetBackUpMagicAttrib();
	void	SetBackUpBmagicAttrib();
	void    SetBackUpRmagicAttrib();
	int     CheckEnChance(char *nKey,int nAttribType);
	KMagicAttrib getaryRequireAttrib(int i){return m_aryRequireAttrib[i];};
	int		GetItemReqType( int i = 0) const { return m_aryRequireAttrib[i].nAttribType;}; 
	int		GetItemReqMagic(int i)  const { return m_aryRequireAttrib[i].nValue[0]; }; //获取需求属性
	KMagicAttrib getaryBaseAttrib(int i){return m_aryBaseAttrib[i];};
	int		GetItemBaseType( int i = 0) const { return m_aryBaseAttrib[i].nAttribType;}; 
	int		GetItemBaseMagic(int i)  const { return m_aryBaseAttrib[i].nValue[0]; }; //获取基本属性
	int		GetItemBaseMagicA(int i)  const { return m_aryBaseAttrib[i].nValue[1]; }; //获取基本属性
	int		GetItemBaseMagicB(int i)  const { return m_aryBaseAttrib[i].nValue[2]; }; //获取基本属性
	int		GetBaseMagic()  const { return m_aryBaseAttrib[0].nValue[0]; };  //获取基本属性

	char*	GetScript() const { return (char *)m_CommonAttrib.szScript; };  //获取执行的脚本
	void	SetLevel(int i) { m_CommonAttrib.nLevel = i;};
	KMagicAttrib getaryMagicAttrib(int i){return m_aryMagicAttrib[i];};

	int     getmagicAttribCount(){return sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]);};
	int     getbaseAttribCount(){return sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]);};
	int     getrequireAttribCount(){return sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);};

	int		GetAttribType( int i = 0) const { return m_aryMagicAttrib[i].nAttribType;};       //获取魔法属性种类
    int		GetAttribTypeVala( int i = 0) const { return m_aryMagicAttrib[i].nValue[0]; };    //获取魔法属性种类
    int		GetAttribTypeValb( int i = 0) const { return m_aryMagicAttrib[i].nValue[1]; };    //获取魔法属性种类
    int	    GetAttribTypeValc( int i = 0) const { return m_aryMagicAttrib[i].nValue[2]; };    //获取魔法属性种类

	void	SetAttribType( int i ,int nType) { m_aryMagicAttrib[i].nAttribType=nType; };      //获取魔法属性种类
	void	SetAttribTypeAndVal( int i ,int nType,int nVal) { m_aryMagicAttrib[i].nAttribType=nType;m_aryMagicAttrib[i].nValue[0]=nVal;};
	int		GetRongAttribType( int i = 0) const { return m_ronMagicAttrib[i].nAttribType; };  //类型得到熔炼属性的
	int		GetRongAttribTypeVala( int i = 0) const { return m_ronMagicAttrib[i].nValue[0]; };
	int		GetRongAttribTypeValb( int i = 0) const { return m_ronMagicAttrib[i].nValue[1]; };
	int	    GetRongAttribTypeValc( int i = 0) const { return m_ronMagicAttrib[i].nValue[2]; };

	void	SetRongAttribType(int i,int nType){m_ronMagicAttrib[i].nAttribType=nType; };      //类型得到熔炼属性的
	void	SetRongTypeAndVal(int i,int nType,int nVal){m_ronMagicAttrib[i].nAttribType=nType;m_ronMagicAttrib[i].nValue[0]=nVal;};
	int		GetRongTypeVal( int i = 0) const { return m_ronMagicAttrib[i].nValue[0]; }; 
//	int		GetBaoShiAttribType( int i = 0) const { return m_BaoShiMagicAttrib[i].nAttribType; }; //  类型得到熔炼属性的
	void	SetPoint(int i) { m_CommonAttrib.nPoint = i;};
	void	SetIsWhere(int i) { m_CommonAttrib.nIsWhere = i;};
	void	SetRPoint(int i) { m_CommonAttrib.nRongpiont = i;};
	void	SetGoodPrice(int uPrice) {m_CommonAttrib.nPrice = uPrice;};
	void	SetTime(int bYear=0,int bMonth=0,int bDay=0,int bHour=0,int bMin=0);
	void    SetItemMagicLevel(int i,int nRow);  //设置第一个属性的值 主要用于龙脉任务
	int     GetItemMagicLevel(int i);
	KTime*	GetTime() { return & m_CommonAttrib.LimitTime;}; 
	BOOL	HaveMaigc(int nAttribe,int nValue1Min,int nValue1Max,int nValue2Min,int nValue2Max,int nValue3Min,int nValue3Max);
	int     CheckWenPin(int pShuXingID=0,int pVal=0);
	int     CheckBluePin(int pShuXingID=0,int pVal=0);
	int     GeteventID(int pShuXingID=0);
	BOOL    SetAttrib_RON(IN const KItemNormalAttrib*);             //熔炼属性
	BOOL    SetAttrib_Bao(IN const KItemNormalAttrib*);             //宝石属性
	BOOL    SetAttrib_MA(IN const KItemNormalAttrib*);
	BOOL    SetAttrib_Yin(IN const KItemNormalAttrib*);
//客户端
	void	SetPrice(int uPrice) {m_CommonAttrib.uPrice = uPrice;};       //摆摊价格
	void	SetModel(BYTE uPrice) {m_CommonAttrib.ncSellModel = uPrice;}; //购买的类型
	BYTE	GetcModel() {return m_CommonAttrib.ncSellModel;};             //购买的类型
    void	SetLianjieFlg(BYTE uLianjieFlg) {m_CommonAttrib.uLianjieFlg = uLianjieFlg;}; //链接标示
	//void	SetIsCanUse(int uCanUse) {m_CommonAttrib.nCanUse = uCanUse;};               //设置限时到是否能使用
	void    SetItmeInfo(char *ItemInfo) {memset(m_CommonAttrib.szIntro,0,sizeof(m_CommonAttrib.szIntro));t_sprintf(m_CommonAttrib.szIntro,"%s",ItemInfo);};  //设设置说明
//	void    SetItmeInfo(char *ItemInfo) {m_CommonAttrib.szIntro = ItemInfo;};
	char*	GetItmeInfo() const { return (char *)m_CommonAttrib.szIntro; };
	int		GetSetPrice() {return m_CommonAttrib.uPrice;};
	void	Paint(int nX, int nY,BOOL bStack = TRUE,int nzStackNum=0,int nKind=-1);  //默认是描绘
	//获取描述
	void	GetDesc(char* pszMsg, bool bShowPrice = false, int nPriceScale = 1, int nActiveAttrib = 0,int nType=0);


// 以下是辅助接口
friend class	KItemGenerator;
friend class	KPlayer;
friend class	KItemList;
private:
	BOOL SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT*);
	BOOL SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD*);
    BOOL SetAttrib_RCBR(IN const KBASICPROP_EQUIPMENT_GOLD*,BOOL nIsDel=FALSE);
	BOOL SetAttrib_BCBR(IN const KBASICPROP_EQUIPMENT_PLATINA*);
	
//	BOOL SetAttrib_MA(IN const KItemNormalAttrib*);
//	BOOL SetAttrib_RON(IN const KItemNormalAttrib*); //熔炼属性
//	BOOL SetAttrib_Bao(IN const KItemNormalAttrib*); //宝石属性
	BOOL SetAttrib_MAB(IN const KMACP*);
	BOOL SetAttrib_MAC(IN const int*);
	void operator = (const KBASICPROP_EQUIPMENT&); //
	void operator = (const KBASICPROP_MEDMATERIAL&);
	void operator = (const KBASICPROP_MINE&);
	void operator = (const KBASICPROP_MEDICINE&);
	void operator = (const KBASICPROP_QUEST&);
	void operator = (const KBASICPROP_TOWNPORTAL&);
	void operator = (const KBASICPROP_EQUIPMENT_UNIQUE&);

	void operator = (const KBASICPROP_EQUIPMENT_PLATINA&);
	void operator = (const KBASICPROP_EQUIPMENT_GOLD&);
	void operator = (const KBASICPROP_FUSION&);
	BOOL Gen_Equipment_Unique(const KBASICPROP_EQUIPMENT*, const KBASICPROP_EQUIPMENT_UNIQUE*);

// 以下是辅助函数
private:
	BOOL SetAttrib_Base(const KEQCP_BASIC*);
	BOOL SetAttrib_Req(const KEQCP_REQ*,BOOL inDel=FALSE);
};

extern KItem *Item;//Item[MAX_ITEM];

#endif
