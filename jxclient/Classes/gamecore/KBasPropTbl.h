//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.h
// Date:	2002.08.14
// Code:	DongBo
// Desc:    header file. 本文件定义的类用于从tab file中读出道具的初始属性,
//			并生成对应的属性表
//---------------------------------------------------------------------------

#ifndef	KBasPropTblH
#define	KBasPropTblH

#define		SZBUFLEN_0	128		    // 典型的字符串缓冲区长度
#define		SZBUFLEN_1	516		    // 典型的字符串缓冲区长度
#define		SCBUFLEN_1	300		    // 典型的字符串缓冲区长度


//#define		MAX_MAGIC_PREFIX	20
//#define		MAX_MAGIC_SUFFIX	20
// 以下结构用于描述矿石的基本属性. 相关属性由配置文件(tab file)提供
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	                //金币
	int			m_nLevel;					// 等级
	BOOL		m_bStack;					// 是否可叠放	
	char		m_szScript[SCBUFLEN_1];		// 执行脚本
	int			m_Magic[6];
	int			m_nDelet;
	int	        m_IsBang;
    int	        m_IsKuaiJie;
    int	        m_IsMagic;
    int	        m_IsUse;
} KBASICPROP_MINE;

// 以下结构用于描述药品属性的特性：数值与时间
typedef struct
{
	int			nAttrib;
	int			nValue;
	int			nTime;
} KMEDATTRIB;

// 以下结构用于描述药品的基本属性. 相关属性由配置文件(tab file)提供
// 适用于以下药品: 生命补充类,内力补充类,体力补充类,毒药类,解毒类,
//					解燃烧类,解冰冻类
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	int			m_nLevel;					// 等级
	BOOL		m_bStack;					// 是否可叠放
	KMEDATTRIB	m_aryAttrib[6];				// 药品的属性
	int	        m_IsBang;
	int	        m_IsKuaiJie;
	int	        m_IsMagic;
	int         m_UseMap;
} KBASICPROP_MEDICINE;

// 以下结构用于描述药材的基本属性. 相关属性由配置文件(tab file)提供
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	int			m_nLevel;					// 等级
	BOOL		m_bStack;					// 是否可叠放
	int			m_nAttrib1_Type;			// 属性1类型
	int			m_nAttrib1_Para;			// 属性1参数
	int			m_nAttrib2_Type;			// 属性2类型
	int			m_nAttrib2_Para;			// 属性2参数
	int			m_nAttrib3_Type;			// 属性2类型
	int			m_nAttrib3_Para;			// 属性2参数
	int	        m_IsBang;
	int	        m_IsKuaiJie;
	int	        m_IsMagic;
} KBASICPROP_MEDMATERIAL;

// 以下结构用于描述一对最大,最小值
typedef struct
{
	int			nMin;
	int			nMax;
} KMINMAXPAIR;

// 以下结构用于给出装备的核心参数: 基础属性
typedef struct
{
	int			nType;						// 属性类型
	KMINMAXPAIR	sRange;						// 取值范围
} KEQCP_BASIC;	// Equipment_CorePara_Basic

// 以下结构用于给出装备的核心参数: 需求属性
typedef struct
{
	int			nType;						// 属性类型
	int			nPara;						// 数值
} KEQCP_REQ;	// Equipment_CorePara_Requirment

// 以下结构用于给出魔法的核心参数
typedef struct
{
	int			nPropKind;					// 修改的属性类型（对同一个数值加百分比和加点数被认为是两个属性）
	KMINMAXPAIR	aryRange[3];				// 修改属性所需的几个参数
} KMACP;	// MagicAttrib_CorePara

// 以下结构用于描述配置文件中给出的魔法属性. 相关属性由配置文件(tab file)提供
// Add by Freeway Chen in 2003.5.30
#define			MATF_CBDR		    15      // 物品类型 +1 容错 type(现在的值为 equip_detailnum)  装备属性可出现部位数量限制
#define         MATF_PREFIXPOSFIX   2       // 前缀后缀
#define         MATF_SERIES         5       // 五行
#define         MATF_LEVEL          10      // 魔法属性最多有10个级别

typedef struct
{
	int			m_nPos;						// 前缀还是后缀
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nClass;					// 五行要求
	int			m_nLevel;					// 等级要求
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	KMACP		m_MagicAttrib;				// 核心参数
	int			m_DropRate[MATF_CBDR];		// 出现概率
    //add by Freeway Chen in 2003.5.30
    int         m_nUseFlag;                 // 该魔法是否被使用过
} KMAGICATTRIB_TABFILE;

// 以下结构用于描述魔法属性. 相关属性由配置文件(tab file)提供
/*
typedef struct
{
	int			m_nPos;						// 前缀还是后缀
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nClass;					// 五行要求
	int			m_nLevel;					// 等级要求
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	KMACP		m_MagicAttrib;				// 核心参数
	int			m_DropRate;					// 出现概率
} KMAGICATTRIB;
*/
// 以下结构用于描述装备的初始属性. 相关数据由配置文件(tab file)提供====基本装备（白装）
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类 (武器? 药品? 矿石?)
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	int         m_nParticularTypea;
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	int			m_nLevel;					// 等级
	BOOL		m_bStack;					// 是否可叠放
	KEQCP_BASIC	m_aryPropBasic[7];			// 基础属性
	KEQCP_REQ	m_aryPropReq[6];			// 需求属性

	int	        m_nYingNuma;               //隐藏属性1
    int	        m_nYingNumb;               //隐藏属性2
	int         m_nRongNum;                //可溶属性数量
    int         m_nWengangPin;             //可溶纹钢品质
	int         m_nBinfujiazhi;            //兵富甲值
	int	        m_IsBang;
	int         m_IsKuaiJie;
	int	        m_IsMagic;
    int	        m_ChiBangRes;
} KBASICPROP_EQUIPMENT;

// 以下结构用于描述唯一装备的初始属性. 相关数据由配置文件(tab file)提供 ----蓝装
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类 (武器? 药品? 矿石?)
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	int			m_nLevel;					// 等级
	int			m_nRarity;					// 稀有程度
	KEQCP_REQ	m_aryPropReq[6];			// 需求属性
	KMACP		m_aryMagicAttribs[6];		// 魔法属性
	int	        m_IsBang;
	int         m_IsKuaiJie;
	int	        m_IsMagic;
} KBASICPROP_EQUIPMENT_UNIQUE;

// 以下结构用于描述黄金装备的初始属性. 相关数据由配置文件(tab file)提供 =====白金 
typedef struct
 {
	 char		m_szName[SZBUFLEN_0];		// 名称
	 int			m_nItemGenre;				// 道具种类 (武器? 药品? 矿石?)
	 int			m_nDetailType;				// 具体类别
	 int			m_nParticularType;			// 详细类别
	 //	int			m_nRarity;					// 稀有程度
	 char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	 BOOL		m_bStack;					// 是否可叠放
	 int			m_nObjIdx;					// 对应物件索引
	 int			m_nWidth;					// 物品栏宽度
	 int			m_nHeight;					// 物品栏高度
	 char		m_szIntro[SZBUFLEN_1];		// 说明文字
	 int			m_nSeries;					// 五行属性
	 int			m_nPrice;					// 价格
	 int			m_nPriceXu;	
	 int			m_nLevel;					// 等级	
	 KEQCP_BASIC	m_aryPropBasic[7];			// 基础属性
	 KEQCP_REQ	    m_aryPropReq[6];			// 需求属性
	 int			m_aryMagicAttribs[6];		// 魔法属性
	 int            m_yinMagicAttribs[2];
	 int            m_TempPlatinaAttrib[6];     // 临时的魔法属性
	 int			m_nId;						// 黄金Id
	 int			m_nSet;						// 所在套装
	 int			m_nSetId;					// 所属序号
	 int			m_nSetNum;					// 套装数量
	 int			m_nUpSet;					// 扩展套装
//	 int	        m_nYingNuma;               //隐藏属性1
//	 int	        m_nYingNumb;               //隐藏属性2
	 int            m_nRongNum;               //可溶属性数量
	 int            m_nWengangPin;            //可溶纹钢品质
	 int            m_nBinfujiazhi;            //兵富甲值
	 int	        m_IsBang;
	 int            m_IsKuaiJie;  
	 int	        m_IsMagic;
	 int            m_ChiBangRes;              //翅膀的外观类型
	 int            m_nSixSkill;
	 int            m_nTenSkill;
 } KBASICPROP_EQUIPMENT_PLATINA;

// 以下结构用于描述黄金装备的初始属性. 相关数据由配置文件(tab file)提供 =====黄装
// flying 根据策划需求修改自KBASICPROP_EQUIPMENT_UNIQUE类型
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类 (武器? 药品? 矿石?)
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
//	int			m_nRarity;					// 稀有程度
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	BOOL		m_bStack;					// 是否可叠放
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 物品栏宽度
	int			m_nHeight;					// 物品栏高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int			m_nSeries;					// 五行属性
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	int			m_nLevel;					// 等级	
	KEQCP_BASIC	m_aryPropBasic[7];			// 基础属性
	KEQCP_REQ	m_aryPropReq[6];			// 需求属性
	int			m_aryMagicAttribs[6];		// 魔法属性
	int         m_yinMagicAttribs[2];       // 隐藏属性
	int			m_nId;						// 黄金Id
	int			m_nSet;						// 所在套装
	int			m_nSetId;					// 所属序号
	int			m_nSetNum;					// 套装数量
	int			m_nUpSet;					// 扩展套装
//    int	        m_nYingNuma;            //隐藏属性1
//    int	        m_nYingNumb;            //隐藏属性2
	int         m_nRongNum;               //可溶属性数量
    int         m_nWengangPin;            //可溶纹钢品质
	int         m_nBinfujiazhi;            //兵富甲值
	int	        m_IsBang;
	int         m_IsKuaiJie;  
	int	        m_IsMagic;
	int         m_ChiBangRes;              //翅膀的外观类型
} KBASICPROP_EQUIPMENT_GOLD;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	int			m_nDetailType;				// 具体类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	char		m_szScript[SCBUFLEN_1];		// 执行脚本	
	int			m_nPrice;
	int			m_nPriceXu;	
	int			m_nDelet;
	int			m_nIsSell;
	int			m_nIsTrade;
	int			m_nIsDrop;  //材料？
	int	        m_IsBang;
	int         m_IsKuaiJie;
	int         m_SkillType;                //纹钢的属性魔法ID
	int			m_nSeries;					//五行属性
	int			m_ISMagic;
	int         m_nLevel;
	int         m_bStack;                   //是否叠放
	int         m_MagicID; //技能ID
	int	        m_IsUse;
} KBASICPROP_QUEST;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	int			m_nDetailType;				// 具体类别
	int			m_nParticularType;			// 详细类别
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	char		m_szScript[SCBUFLEN_1];		// 执行脚本	
	int			m_nPrice;
	int			m_nPriceXu;	
	int			m_nDelet;
	int			m_nIsSell;
	int			m_nIsTrade;
	int			m_nIsDrop;  //材料？
	int	        m_IsBang;
	int         m_IsKuaiJie;
	int         m_SkillType;                //纹钢的属性魔法ID
	int			m_nSeries;					//五行属性
	int			m_ISMagic;
	int         m_nLevel;
	int         m_bStack;    //是否叠放
	int         m_MagicID;   //技能ID
	int	        m_IsUse;
    int         m_inPin;     //品质
    int         m_MagIndex;  //魔法索引
    int         m_Magic[6];  //可熔炼部位
}KBASICPROP_FUSION;  //纹纲

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// 名称
	int			m_nItemGenre;				// 道具种类
	char		m_szImageName[SZBUFLEN_0];	// 界面中的动画文件名
	int			m_nObjIdx;					// 对应物件索引
	int			m_nWidth;					// 道具栏中所占宽度
	int			m_nHeight;					// 道具栏中所占高度
	int			m_nPrice;					// 价格
	int			m_nPriceXu;	
	char		m_szIntro[SZBUFLEN_1];		// 说明文字
	int	        m_IsBang;
	int         m_IsKuaiJie;
	int	        m_IsMagic;
} KBASICPROP_TOWNPORTAL;
//=============================================================================

class KBasicPropertyTable			      // 缩写: BPT,用于派生类
{
public:
	KBasicPropertyTable();
	~KBasicPropertyTable();

// 以下是核心成员变量
protected:
	void*		m_pBuf;						// 指向属性表缓冲区的指针
											// 属性表是一个结构数组,
											// 其具体类型由派生类决定
	int			m_nNumOfEntries;			// 属性表含有多少项数据

// 以下是辅助性的成员变量
    int         m_nSizeOfEntry;				// 每项数据的大小(即结构的大小)
	char		m_szTabFile[MAX_PATH];		// tabfile的文件名

// 以下是对外接口
public:
	virtual BOOL Load();					// 从tabfile中读出初始属性值, 填入属性表
	int NumOfEntries() const { return m_nNumOfEntries; }

// 以下是辅助函数
protected:
	BOOL GetMemory();
	void ReleaseMemory();
	void SetCount(int);
	virtual BOOL LoadRecord(int i, KTabFile* pTF) = 0;
};

class KBPT_Mine : public KBasicPropertyTable
{
public:
	KBPT_Mine();
	~KBPT_Mine();

// 以下是对外接口
public:
	const KBASICPROP_MINE* GetRecord(IN int) const;
	const KBASICPROP_MINE* FindRecord(IN int, IN int) const;

// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// =====>药品<=====
class KBPT_Medicine : public KBasicPropertyTable
{
public:
	KBPT_Medicine();
	~KBPT_Medicine();

// 以下是对外接口
public:
	const KBASICPROP_MEDICINE* GetRecord(IN int) const;
	const KBASICPROP_MEDICINE* FindRecord(IN int, IN int) const;

// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// =====>任务物品<=====
class KBPT_Quest : public KBasicPropertyTable
{
public:
	KBPT_Quest();
	~KBPT_Quest();

// 以下是对外接口
public:
	const KBASICPROP_QUEST* GetRecord(IN int) const;
	const KBASICPROP_QUEST* FindRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// =====>纹纲物品<=====
class KBPT_Fusion : public KBasicPropertyTable
{
public:
	KBPT_Fusion();
	~KBPT_Fusion();
	
	// 以下是对外接口
public:
	const KBASICPROP_FUSION* GetRecord(IN int) const;
	const KBASICPROP_FUSION* FindRecord(IN int,IN int) const;
	
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};
// =====>回城物品<=====
class KBPT_TownPortal : public KBasicPropertyTable
{
public:
	KBPT_TownPortal();
	~KBPT_TownPortal();

// 以下是对外接口
public:
	const KBASICPROP_TOWNPORTAL* GetRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// =====>药材<=====
class KBPT_MedMaterial : public KBasicPropertyTable
{
public:
	KBPT_MedMaterial();
	~KBPT_MedMaterial();

// 以下是对外接口
public:
	const KBASICPROP_MEDMATERIAL* GetRecord(IN int) const;

// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};
//白装
class KBPT_Equipment : public KBasicPropertyTable
{
public:
	KBPT_Equipment();
	~KBPT_Equipment();

// 以下是对外接口
public:
	const KBASICPROP_EQUIPMENT* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT* FindRecord(IN int, IN int, IN int) const;
	void Init(IN int);
// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};
//蓝装
class KBPT_Equipment_Unique : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Unique();
	~KBPT_Equipment_Unique();

// 以下是对外接口
public:
	const KBASICPROP_EQUIPMENT_UNIQUE* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_UNIQUE* FindRecord(IN int, IN int, IN int) const;

// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// 白金装备
class KBPT_Equipment_Platina : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Platina();
	virtual ~KBPT_Equipment_Platina();
	
	// 以下是对外接口	 KBASICPROP_EQUIPMENT_PLATINA
public:
	const KBASICPROP_EQUIPMENT_PLATINA* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_PLATINA* FindRecord(IN int, IN int, IN int) const;
	int GetRecordCount() const {return KBasicPropertyTable::NumOfEntries();};
	void Init();
	// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

// flying modify this class
// 黄金装备
class KBPT_Equipment_Gold : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Gold();
	virtual ~KBPT_Equipment_Gold();

// 以下是对外接口
public:
	const KBASICPROP_EQUIPMENT_GOLD* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_GOLD* FindRecord(IN int, IN int, IN int) const;
	int GetRecordCount() const {return KBasicPropertyTable::NumOfEntries();};
	void Init();
// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_MagicAttrib_TF : public KBasicPropertyTable
{
public:
	KBPT_MagicAttrib_TF();
	~KBPT_MagicAttrib_TF();

// 以下是辅助成员变量
protected:
	int m_naryMACount[2][MATF_CBDR];	// 每种装备可适用的魔法数目,分前后缀进行统计
										// 共有MATF_CBDR种装备可以具备魔法
// 以下是对外接口
public:
	void GetMACount(int*) const;
	const KMAGICATTRIB_TABFILE* GetRecord(IN int) const;

// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
	void Init();
};
/*
class KBPT_MagicAttrib : public KBasicPropertyTable
{
public:
	KBPT_MagicAttrib();
	~KBPT_MagicAttrib();

// 以下是辅助函数
protected:
};
*/

//============================================================================
class KBPT_ClassMAIT    // Magic Item Index Table
{
public:
	KBPT_ClassMAIT();
	~KBPT_ClassMAIT();

// 以下是核心成员变量
protected:
	int*	m_pnTable;				// 缓冲区指针, 所存数据为
									// KBPT_MagicAttrib_TF::m_pBuf数组的下标
	int		m_nSize;				// 缓冲区内含多少项数据(并非字节数)

// 以下是辅助成员变量
	int		m_nNumOfValidData;		// 缓冲区中有效数据的个数
									// 初始化工作完成后m_nNumOfValidData < m_nSize
// 以下是对外接口
public:
    BOOL Clear();
	BOOL Insert(int nItemIndex);
	int  Get(int i) const;
    int  GetCount() const { return m_nNumOfValidData; }
};

//============================================================================

class KBPT_ClassifiedMAT
{
public:
	KBPT_ClassifiedMAT();
	~KBPT_ClassifiedMAT();

// 以下是核心成员变量
protected:
	int*	m_pnTable;				// 缓冲区指针, 所存数据为
									// KBPT_MagicAttrib_TF::m_pBuf数组的下标
	int		m_nSize;				// 缓冲区内含多少项数据(并非字节数)

// 以下是辅助成员变量
	int		m_nNumOfValidData;		// 缓冲区中有效数据的个数
									// 初始化工作完成后m_nNumOfValidData==m_nSize
// 以下是对外接口
public:
	BOOL GetMemory(int);
	BOOL Set(int);
	int  Get(int) const;
	BOOL GetAll(int*, int*) const;

// 以下是辅助函数
protected:
	void ReleaseMemory();
};

class KLibOfBPT
{
public:
	KLibOfBPT();
	~KLibOfBPT();

// 以下是核心成员变量
protected:
	KBPT_Medicine			m_BPTMedicine;
	KBPT_TownPortal			m_BPTTownPortal;
	KBPT_Quest				m_BPTQuest;
	KBPT_Fusion				m_BPTFusion;  //纹纲
	KBPT_Mine				m_BPTMine;
	KBPT_Equipment			m_BPTHorse;
	KBPT_Equipment			m_BPTMeleeWeapon;
	KBPT_Equipment			m_BPTRangeWeapon;
	KBPT_Equipment			m_BPTArmor;
	KBPT_Equipment			m_BPTHelm;
	KBPT_Equipment			m_BPTBoot;
	KBPT_Equipment			m_BPTBelt;
	KBPT_Equipment			m_BPTAmulet;
	KBPT_Equipment			m_BPTRing;
	KBPT_Equipment			m_BPTCuff;
	KBPT_Equipment			m_BPTPendant;
	KBPT_Equipment			m_BPTMask;
	KBPT_Equipment			m_BPTPifeng;
	KBPT_Equipment			m_BPTYinjian;
	KBPT_Equipment			m_BPTShipin;
	
    KBPT_MagicAttrib_TF		m_BPTMagicAttrib;  //魔法属性列表
	KBPT_Equipment_Gold		m_GoldItem;
	KBPT_Equipment_Platina	m_Platina;
	// 四维分别为前后缀、物品类型、五行、级别
    KBPT_ClassMAIT          m_CMAIT[MATF_PREFIXPOSFIX][MATF_CBDR][MATF_SERIES][MATF_LEVEL];

	KBPT_ClassifiedMAT		m_CMAT[2][MATF_CBDR];

// 以下是对外接口
public:
	BOOL Init();

	const KMAGICATTRIB_TABFILE* GetMARecord(IN int) const;
	const int					GetMARecordNumber() const;
    
    // Add by Freeway Chen in 2003.5.30
    const KBPT_ClassMAIT*       GetCMIT(IN int nPrefixPostfix, IN int nType, IN int nSeries, int nLevel) const;

	const KBPT_ClassifiedMAT*	GetCMAT(IN int, int) const;
	// Add by flying on 2003.6.2
	const KBASICPROP_EQUIPMENT_GOLD*	GetGoldItemRecord(IN int nIndex) const;
	const int							GetGoldItemNumber() const;

	const KBASICPROP_EQUIPMENT_PLATINA*	GetPlatinaRecord(IN int nIndex) const;
	const int							GetPlatinaNumber() const;

	const KBASICPROP_EQUIPMENT*	GetMeleeWeaponRecord(IN int) const;
	const int					GetMeleeWeaponRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetRangeWeaponRecord(IN int) const;
	const int					GetRangeWeaponRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetArmorRecord(IN int) const;
	const int					GetArmorRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetHelmRecord(IN int) const;
	const int					GetHelmRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetBootRecord(IN int) const;
	const int					GetBootRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetBeltRecord(IN int) const;
	const int					GetBeltRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetAmuletRecord(IN int) const;
	const int					GetAmuletRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetRingRecord(IN int) const;
	const int					GetRingRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetCuffRecord(IN int) const;
	const int					GetCuffRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetPendantRecord(IN int) const;
	const int					GetPendantRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetHorseRecord(IN int) const;
	const int					GetHorseRecordNumber() const;
	const KBASICPROP_EQUIPMENT_UNIQUE* FindEquipmentUnique(IN int, IN int, IN int) const;
	const KBASICPROP_MEDMATERIAL* GetMedMaterial(IN int) const;
	const KBASICPROP_MEDICINE*	GetMedicineRecord(IN int) const;
	const int					GetMedicineRecordNumber() const;
	const KBASICPROP_MEDICINE*	FindMedicine(IN int, IN int) const;
	const KBASICPROP_QUEST*		GetQuestRecord(IN int) const;
	const int					GetQuestRecordNumber() const;
	const KBASICPROP_TOWNPORTAL*	GetTownPortalRecord(IN int) const;
	const int					GetTownPortalRecordNumber() const;
	const KBASICPROP_MINE*		GetMine(IN int) const;
	const int					GetMineRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetMaskRecord(IN int) const;	// mat na
	const int					GetMaskRecordNumber() const;	// mat na
	const KBASICPROP_EQUIPMENT* GetPifengRecord(IN int) const;	// 披风
	const int					GetPifengRecordNumber() const;	// 披风
	const KBASICPROP_EQUIPMENT* GetYinjianRecord(IN int) const;	// 印鉴
	const int					GetYinjianRecordNumber() const;	// 印鉴
	const KBASICPROP_EQUIPMENT* GetShipinRecord(IN int) const;	// 饰品
	const int					GetShipinRecordNumber() const;	// 饰品
	const KBASICPROP_FUSION*	GetFusionRecord(IN int) const;  //纹纲
	const int					GetFusionRecordNumber() const;  //翁刚

// 	const KBASICPROP_EQUIPMENT_GOLD*	GetGoldRecord(IN int) const;
// 	const int					GetGoldRecordNumber(IN int) const;
// 以下是辅助函数
protected:
	BOOL InitMALib();
    
    // Add by Freeway Chen in 2003.5.30
    BOOL InitMAIT();
};
#endif		// #ifndef KBasPropTblH
