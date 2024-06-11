// 场景地图（客户端版）
// Copyright : Kingsoft 2002
// Author    : Wooy (wu yue)
// CreateTime: 2002-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************
#include "gamecore/KCore.h"
#include "engine/KEngine.h"
#include "KScenePlaceC.h"
//#include <crtdbg.h>
#include "KIpotLeaf.h"
#include "SceneMath.h"
#include "gamecore/CoreShell.h"
#include "gamecore/ImgRef.h"
//#include "../../Represent/iRepresent/iRepresentShell.h"
#include "engine/Text.h"
#include "ObstacleDef.h"
#include <math.h>
#include "gamecore/ImgRef.h"
//后来加的
#include "gamecore/KSubWorld.h"
#ifndef WIN32
#include <assert.h>
#endif

//#include "KSubWorldSet.h"
extern KImageStore2 m_ImageStore;
//	bool		g_bShowGameInfo = FALSE;	    //是否显示游戏（场景）信息  调试模式显示坐标
	int			g_nMapIndex = 0;			    //场景索引数值
	//int			g_bShowObstacle = FALSE;		//是否显示障碍
//	bool        g_iShowPaint    = FALSE;
//#endif
KScenePlaceC	g_ScenePlace;
//====是否预绘制地表层====
static bool		l_bPrerenderGround = TRUE;	///TURE

//摆放场景地图文件的目录
#define	ALL_PALCE_ROOT_FOLDER	"\\maps"
//判断区域索引坐标是否落在以焦点区域为中心的一个范围内
#define INSIDE_AREA(h, v, range)	\
	( ((h) - m_FocusRegion.x) * ((h) - m_FocusRegion.x) <= (range * range) &&   \
		((v) - m_FocusRegion.y) * ((v) - m_FocusRegion.y) <= (range * range) )

#define GET_IN_PROCESS_AREA_REGION(h, v)		\
	( m_pInProcessAreaRegions[((v) - m_FocusRegion.y + 1) * SPWP_PROCESS_RANGE + (h) - m_FocusRegion.x + 1])

//***********************************************************************************************
// EnvironmentLight类的实现
DWORD ChaZhiColor(KLColor &cLight1, KLColor &cLight2, float f2)
{
	if(f2 < 0.0f || f2 > 1.0f)
		return 0xff000000;

	float f1 = 1.0f - f2;
	unsigned int r, g, b;
	r = (unsigned int)(cLight2.r * f2 + cLight1.r * f1);
	g = (unsigned int)(cLight2.g * f2 + cLight1.g * f1);
	b = (unsigned int)(cLight2.b * f2 + cLight1.b * f1);
	return 0xff000000 | (r<<16) | (g<<8) | b;
}

EnvironmentLight::EnvironmentLight()
{
	for(int i=0; i<7; i++)
		m_cLight[i].r = m_cLight[i].g = m_cLight[i].b = 0x00000040;

	m_cLight[0].r = 0x18, m_cLight[0].g = 0x18, m_cLight[0].b = 0x18;
	m_cLight[1].r = 0x18, m_cLight[1].g = 0x18, m_cLight[1].b = 0x28;
	m_cLight[2].r = 0x38, m_cLight[2].g = 0x38, m_cLight[2].b = 0x3b;
	m_cLight[3].r = 0x40, m_cLight[3].g = 0x40, m_cLight[3].b = 0x40;
	m_cLight[4].r = 0x58, m_cLight[4].g = 0x58, m_cLight[4].b = 0x58;
	m_cLight[5].r = 0x38, m_cLight[5].g = 0x30, m_cLight[5].b = 0x28;
	m_cLight[6].r = 0x30, m_cLight[6].g = 0x2a, m_cLight[6].b = 0x28;
}

// 设置第nIdx个颜色
void EnvironmentLight::SetLight(const KLColor &cLight, int nIdx)
{
	m_cLight[nIdx] = cLight;
}

// 设置第nIdx个颜色
void EnvironmentLight::SetLight(BYTE r, BYTE g, BYTE b, int nIdx)
{
	m_cLight[nIdx].r = r;
	m_cLight[nIdx].g = g;
	m_cLight[nIdx].b = b;
}

// 设置所有7个颜色
void EnvironmentLight::SetLight(KLColor *pLight)
{
	for(int i=0; i<7; i++)
		m_cLight[i] = pLight[i];
}

// 取得距一天开始nMinutes分钟时的环境光颜色
DWORD EnvironmentLight::GetEnvironmentLight(int nMinutes)
{
	if(nMinutes < 660)
	{
		if(nMinutes < 300)
		{
			if(nMinutes >= 0 && nMinutes < 60)
				return m_cLight[0].GetColor();
			else if(nMinutes >= 60 && nMinutes < 180)
				return ChaZhiColor(m_cLight[0], m_cLight[1], (nMinutes - 60) / 120.0f);
			else if(nMinutes >= 180 && nMinutes < 300)
				return m_cLight[1].GetColor();
		}
		else
		{
			if(nMinutes >= 300 && nMinutes < 420)
				return ChaZhiColor(m_cLight[1], m_cLight[2], (nMinutes - 300) / 120.0f);
			else if(nMinutes >= 420 && nMinutes < 540)
				return ChaZhiColor(m_cLight[2], m_cLight[3], (nMinutes - 420) / 120.0f);
			else if(nMinutes >= 540 && nMinutes < 660)
				return ChaZhiColor(m_cLight[3], m_cLight[4], (nMinutes - 540) / 120.0f);
		}
	}
	else
	{
		if(nMinutes < 1020)
		{
			if(nMinutes >= 660 && nMinutes < 780)
				return m_cLight[4].GetColor();
			else if(nMinutes >= 780 && nMinutes < 900)
				return ChaZhiColor(m_cLight[4], m_cLight[3], (nMinutes - 780) / 120.0f);
			else if(nMinutes >= 900 && nMinutes < 1020)
				return ChaZhiColor(m_cLight[3], m_cLight[5], (nMinutes - 900) / 120.0f);
		}
		else
		{
			if(nMinutes >= 1020 && nMinutes < 1140)
				return ChaZhiColor(m_cLight[5], m_cLight[6], (nMinutes - 1020) / 120.0f);
			else if(nMinutes >= 1140 && nMinutes < 1260)
				return ChaZhiColor(m_cLight[6], m_cLight[0], (nMinutes - 1140) / 120.0f);
			else if(nMinutes >= 1260 && nMinutes < 1440)
				return m_cLight[0].GetColor();
		}
	}  
	return 0xff000000;
}

//*************************************************************************************************

//##ModelId=3DDB39150334
POINT KScenePlaceC::m_RangePosTable[SPWP_MAX_NUM_REGIONS] =
{
	{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0},
	{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1},
	{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2},
	{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3},
	{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4},
	{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5},
	{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6},
};

//##ModelId=3DDB39BA029B
int	KScenePlaceC::m_PRIIdxTable[SPWP_MAX_NUM_REGIONS] =
{
	24, 31, 30, 23, 16, 17, 18, 
	25, 32, 39, 38, 37, 36, 29,
	22, 15, 8,  9,  10, 11, 12,
	19, 26, 33, 40, 47, 46, 45,
	44, 43, 42, 35, 28, 21, 14,
	7,  0,  1,  2,  3,  4,  5,
	6,  13, 20, 27, 34, 41, 48,
};

const KPrevLoadPosItem KScenePlaceC::m_PrevLoadPosOffset[3][3] = 
{
    {
        { 5, {{-2,  0}, {-2, -1}, {-2, -2}, {-1, -2}, { 0, -2}} },  // 左上角(-1, -1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1, -2}, { 0, -2}, { 1, -2}, { 0,  0}, { 0,  0}} },  // 正上方( 0, -1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0, -2}, { 1, -2}, { 2, -2}, { 2, -1}, { 2,  0}} },  // 右上角( 1, -1)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 3, {{-2, -1}, {-2,  0}, {-2,  1}, { 0,  0}, { 0,  0}} },  // 正左方(-1,  0)的对应需要预加载的Region的数目和相对坐标
        { 0, {{ 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}} },  // 原  点( 0,  0)的对应需要预加载的Region的数目和相对坐标
        { 3, {{ 2, -1}, { 2,  0}, { 2,  1}, { 0,  0}, { 0,  0}} },  // 正右方( 1,  0)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 5, {{-2,  0}, {-2,  1}, {-2,  2}, {-1,  2}, { 0,  2}} },  // 左下角(-1,  1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1,  2}, { 0,  2}, { 1,  2}, { 0,  0}, { 0,  0}} },  // 正下方( 0,  1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0,  2}, { 1,  2}, { 2,  2}, { 2,  1}, { 2,  0}} },  // 右下角( 1,  1)的对应需要预加载的Region的数目和相对坐标
    },
};



//##ModelId=3DBE3B53008C
KScenePlaceC::KScenePlaceC()
{
	m_hLoadAndPreprocessThread = NULL;
	m_bInited = false;
	m_bLoading = false;
	m_bEnableWeather = true;
	m_bFollowWithMap = false;
	m_FocusPosition.x = m_FocusPosition.y = 0;//SPWP_FARAWAY_COORD;
	m_FocusRegion.x = m_FocusRegion.y = 0;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	m_bPreprocessEvent = false;
	m_hLoadRegionEvent = NULL;
	m_hSwitchLoadFinishedEvent = NULL;
	m_nFirstToLoadIndex = -1;
	m_szPlaceRootPath[0] = 0;
	ZeroMemory(m_szPlaceRootPath,sizeof(m_szPlaceRootPath));
	ZeroMemory(m_szSceneName,sizeof(m_szSceneName));
	ZeroMemory(m_szMapType,sizeof(m_szMapType));
	//m_szSceneName[0] = 0;
	//m_szMapType[0] = 0;

	m_nSceneId = SPWP_NO_SCENE;
	m_RepresentArea.left = m_RepresentArea.right = m_RepresentArea.top = m_RepresentArea.bottom = 0;
	m_RepresentExactHalfSize.cx = m_RepresentExactHalfSize.cy = 0;
	m_MapFocusOffset.x = m_MapFocusOffset.y = 0;

	for (int i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_pRegions[i] = &m_RegionObjs[i];
	memset(&m_pInProcessAreaRegions, 0, sizeof(m_pInProcessAreaRegions));
	memset(&m_RegionGroundImages, 0, sizeof(m_RegionGroundImages));
	m_nNumGroundImagesAvailable = 0;

	m_bRenderGround = false;
	m_pfunRegionLoadedCallback = NULL;
	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	
	m_pObjsAbove = NULL;
	m_nNumObjsAbove = 0;
	m_IsOneload  = true;
	m_TreesCount = 0;
	m_HouseCount = 0;
	m_AboveCount = 0;
	m_IsClearMap = false;
	m_ClearState = 0;
	m_ispaint    =false;
//	m_pWeather = NULL;
}

//##ModelId=3DD17A770383
KScenePlaceC::~KScenePlaceC()
{
	m_ispaint = false;
	Terminate();
}

#ifndef WIN32

//CEvent *g_hSwitchLoadFinishedEvent = NULL;

CEvent *KScenePlaceC::CreateEvent(bool bManualReset, bool bInitialState)
{
	CEvent *pEvent = new CEvent(bManualReset, bInitialState);
	assert(pEvent);

	bool bRet = pEvent->CreateEvent();
	assert(bRet);

	return pEvent;
}

unsigned int KScenePlaceC::WaitForSingleObject(CEvent *pEvent, int cms)
{
	assert(pEvent);
	if( pEvent->Wait(cms) )
	{
		return 0;
	}

	return 1;
}

bool KScenePlaceC::CloseHandle(CEvent *pEvent)
{
	delete pEvent;
}

bool KScenePlaceC::SetEvent(CEvent *pEvent)
{
	pEvent->Set();
}

bool KScenePlaceC::ResetEvent(CEvent *pEvent)
{
	pEvent->Reset();
}
#endif
//##ModelId=3DCAAE3703A6
void KScenePlaceC::ClosePlace()
{
	m_ispaint = false;

	if (m_szPlaceRootPath[0] == 0)
		return;

	m_nSceneId = SPWP_NO_SCENE;

	m_Map.Free(); //小地图的处理
	SetLoadingStatus(false);	
#ifdef WIN32
	ResetEvent(m_hSwitchLoadFinishedEvent);
	ResetEvent(m_hLoadRegionEvent);	//这要放在修改m_FocusRegion之前，要不子线程就可能在两句代码执行之间退出结束了。
	EnterCriticalSection(&m_RegionListAdjustCritical);
	EnterCriticalSection(&m_LoadCritical);
	m_nFirstToLoadIndex = -1;
	int i;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		m_pInProcessAreaRegions[i] = NULL;//清空数据

	LeaveCriticalSection(&m_LoadCritical);
	LeaveCriticalSection(&m_RegionListAdjustCritical);

	EnterCriticalSection(&m_ProcessCritical);
	ClearPreprocess(true);
	for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].Clear();//清空数据
	LeaveCriticalSection(&m_ProcessCritical);
#else
	ResetEvent(m_hSwitchLoadFinishedEvent);
	ResetEvent(m_hLoadRegionEvent);	//这要放在修改m_FocusRegion之前，要不子线程就可能在两句代码执行之间退出结束了。
	pthread_mutex_lock(&m_RegionListAdjustCritical);
	pthread_mutex_lock(&m_LoadCritical);
	m_nFirstToLoadIndex = -1;
	int i;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		m_pInProcessAreaRegions[i] = NULL;//清空数据

	pthread_mutex_unlock(&m_LoadCritical);
	pthread_mutex_unlock(&m_RegionListAdjustCritical);

	pthread_mutex_lock(&m_ProcessCritical);
	ClearPreprocess(true);
	for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].Clear();//清空数据
	pthread_mutex_unlock(&m_ProcessCritical);
#endif

	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	m_szPlaceRootPath[0] = 0;
	ZeroMemory(m_szPlaceRootPath,sizeof(m_szPlaceRootPath));
	ZeroMemory(m_szSceneName,sizeof(m_szSceneName));
	ZeroMemory(m_szMapType,sizeof(m_szMapType));
	//m_szSceneName[0] = 0;
	//m_szMapType[0] = 0;
	m_bPreprocessEvent = false;
	m_bRenderGround = false;

	/*if(m_pWeather)
	{
		delete m_pWeather;
		m_pWeather = NULL;
	}*/
	if (g_GameWorld)
	   g_GameWorld->FreeAllSprite();
	//换地图的时候全部删除精灵
}

#ifndef WIN32
void KScenePlaceC::_LoadProcess()
{
	DWORD g_nServiceLoop = 0;
	DWORD dwRetCode = 0;
	while(true)
	{
		//CCMessageBox("线程运行中","线程运行中");
		    /*if (++g_nServiceLoop >=800000000)
		    {
			    g_nServiceLoop = 0;
		    }
		    if (g_nServiceLoop & 0x1)
			   usleep(100000); //微秒 毫秒=1000微妙  sleep 为秒
		*/
		dwRetCode = WaitForSingleObject(m_hLoadRegionEvent,1000);
		if (dwRetCode == 0)
		{
			if (m_FocusRegion.x == SPWP_FARAWAY_COORD)
				break;

			KScenePlaceRegionC*	pRegion = NULL;
			pthread_mutex_lock(&m_RegionListAdjustCritical);
			if (m_nFirstToLoadIndex >= 0)
				pRegion = m_pRegions[m_nFirstToLoadIndex];
			pthread_mutex_unlock(&m_RegionListAdjustCritical);

			if (pRegion)
			{
				pthread_mutex_lock(&m_LoadCritical);
				pRegion->LoadRe(m_szPlaceRootPath);	 //客户端导入障碍
				pthread_mutex_unlock(&m_LoadCritical);
				//if (pRegion->getStatus() == 3) //正在加载中
				ARegionLoaded(pRegion);
				//else
                 //continue;
			}
		}
		else
			m_PreLoadPosItem.m_nNum = 0;
			/*if (m_nSceneId == SPWP_NO_SCENE)
				continue;

			if (m_nFirstToLoadIndex >= 0)
				continue;   // 说明还在加载过程中

			PreLoadProcess();*/
	}
}

void KScenePlaceC::Run()
{
	m_bInited = true;
	_LoadProcess();
}
#endif
//##ModelId=3DCAA6A703DB
bool KScenePlaceC::Initialize()
{
	if (m_bInited)
		return true;
#ifdef WIN32
	m_hSwitchLoadFinishedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hLoadRegionEvent         = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hSwitchLoadFinishedEvent && m_hLoadRegionEvent)
	{
		InitializeCriticalSection(&m_RegionListAdjustCritical);
		InitializeCriticalSection(&m_LoadCritical);
		InitializeCriticalSection(&m_ProcessCritical);
		DWORD	ThreadId;
		//创建线程
		m_hLoadAndPreprocessThread = CreateThread(NULL, 0,LoadThreadEntrance, this, 0, &ThreadId);

		if (m_hLoadAndPreprocessThread)
		{
			m_bInited = true;
			return true;
		}
		DeleteCriticalSection(&m_ProcessCritical);
		DeleteCriticalSection(&m_LoadCritical);
		DeleteCriticalSection(&m_RegionListAdjustCritical);
	}
	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;
	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;

	return false;
#else
	m_hSwitchLoadFinishedEvent = CreateEvent(true, true);
	m_hLoadRegionEvent         = CreateEvent(true, true);

	if (m_hSwitchLoadFinishedEvent && m_hLoadRegionEvent)
	{
	  pthread_mutex_init(&m_RegionListAdjustCritical,NULL);
	  pthread_mutex_init(&m_LoadCritical,NULL);
	  pthread_mutex_init(&m_ProcessCritical,NULL);
	//DWORD	ThreadId;
	//创建线程
	  Start();
	}
    return true;

	/*pthread_mutex_destroy(&m_ProcessCritical);
	pthread_mutex_destroy(&m_LoadCritical);
	pthread_mutex_destroy(&m_RegionListAdjustCritical);*/
#endif

}

//##ModelId=3DCAA64C01DA   每换一次地图 打开一次
bool KScenePlaceC::OpenPlace(int nPlaceIndex)
{
	if (m_bInited == false)
		return false;

	ClosePlace();

	KIniFile	Ini,cIni;
	char		mIndex[32]={0};
	char		Buff[128]={0};
	char        mMaptype[32]={0};
	char        nTempStr[128];
	ZeroMemory(nTempStr,sizeof(nTempStr));

	if (_clientlanguage!=1)
		t_sprintf(nTempStr,"\\settings\\vn\\MapList.ini");
	else
		t_sprintf(nTempStr,"\\settings\\MapList.ini");
//	g_SetFilePath(ALL_PALCE_ROOT_FOLDER);
	if (!cIni.Load(nTempStr))
		return false;

	t_sprintf(mIndex,"%d",nPlaceIndex);
	t_sprintf(mMaptype,"%d",nPlaceIndex);

	if (!cIni.GetString("List",mIndex,"",Buff,sizeof(Buff)))  //地图路径
	{
	    cIni.Clear();
		return false;
	}
	//return false;
	ZeroMemory(m_szPlaceRootPath,sizeof(m_szPlaceRootPath));
	t_sprintf(m_szPlaceRootPath,"%s\\%s",ALL_PALCE_ROOT_FOLDER,Buff);
	strcat(mIndex,"_name");
	strcat(mMaptype,"_MapType");
	//CCMessageBox(UTEXT(m_szPlaceRootPath,1).c_str(),"TESE");// 最后不带斜杠的路径
	//return false;
	if (!cIni.GetString("List",mIndex,"",m_szSceneName,sizeof(m_szSceneName)))
	{//如果没有名字的 就在路径中截取 名称
		char* pName = strstr(Buff, "\\");  //strchr
		if (pName)
		{
			while(strstr(pName, "\\"))
				pName = strstr(pName,"\\") + 1; //去掉斜杠
			strcpy(m_szSceneName, pName);
		}
		else
			strcpy(m_szSceneName,Buff);
	}

	if (!cIni.GetString("List",mMaptype,"", m_szMapType, sizeof(m_szMapType)))
       t_sprintf(m_szMapType, "Others");

    cIni.Clear();
//---------------------------------------------------石鼓镇这里有错误 竹丝洞有错 
//取得场景的环境光信息
	char m_sztempRootPath[256];
	ZeroMemory(m_sztempRootPath,sizeof(m_sztempRootPath));
	int nValue = t_sprintf(m_sztempRootPath,"\\maps\\%s.wor",Buff); //返回	  长度
	try
	{
	  if (!Ini.Load(m_sztempRootPath))
		return false; 
	}
    catch (...)
	{
		return false;
	}
//---------------------------------------------------
	//m_szPlaceRootPath[nValue - 4] = 0;	 //去掉.wor后缀，后面4个字节为空

	//std::string Tempstr = m_szPlaceRootPath;
	//std::string::size_type pos = Tempstr.find(".wor");
	//Tempstr.replace(pos,Tempstr.length(),"");
	/*char m_szTempRootPath[256];
	ZeroMemory(m_szTempRootPath,sizeof(m_szTempRootPath));
	t_sprintf(m_szTempRootPath,m_szPlaceRootPath);
	char *nTemRootPath = strstr(m_szTempRootPath,".wor");
	*/

	//int rootLen = strlen(m_szPlaceRootPath);
	//m_szPlaceRootPath[rootLen - 4] = 0;

	m_nSceneId = nPlaceIndex;
	g_nMapIndex = nPlaceIndex;		//场景索引数值
	//冰河洞初始化小地图 有错误
	m_Map.Load(&Ini, m_nSceneId,m_szPlaceRootPath); //初始换小地图
	int nIsInDoor;
	Ini.GetInteger("MAIN", "IsInDoor", 0, &nIsInDoor);
	m_ObjectsTree.SetIsIndoor(nIsInDoor != 0);
	Ini.Clear(); 

	m_FocusRegion.x = m_FocusRegion.y = -SPWP_LOAD_EXTEND_RANGE;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	int	nImageIndex = 0,i;

	for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].ToLoad(-m_RangePosTable[i].x, -m_RangePosTable[i].y);
	//取消?
	return true;
}

//##ModelId=3DBCE7B70358
void KScenePlaceC::SetFocusPosition(int nX, int nY, int nZ,bool isClearAll)	  //设置焦点像素坐标
{
	if (m_bInited == false || m_szPlaceRootPath[0] == 0 ||
		(m_FocusPosition.x == nX &&	m_FocusPosition.y == nY))
		return;

	if (m_bFollowWithMap)
	{
		m_OrigFocusPosition.x = nX;
		m_OrigFocusPosition.y = nY;
		return;
	}

	//if(m_pWeather)
		//m_pWeather->SetFocusPos(nX, nY);

	//忽略z轴坐标
	m_FocusPosition.x = nX;//像素坐标
	m_FocusPosition.y = nY;//像素坐标

	POINT	pos; //主角当前的区域索引
	pos.x = m_FocusPosition.x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	pos.y = m_FocusPosition.y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	SubWorld[0].LookAt(nX, nY, 0);
	//设置整个绘画界面的大小位置
	m_RepresentArea.right  -= m_RepresentArea.left;//减去原来的
	m_RepresentArea.bottom -= m_RepresentArea.top; //减去原来的
	m_RepresentArea.left    = m_FocusPosition.x - m_RepresentExactHalfSize.cx;
	m_RepresentArea.top     = m_FocusPosition.y - m_RepresentExactHalfSize.cy;
	m_RepresentArea.right  += m_RepresentArea.left;
	m_RepresentArea.bottom += m_RepresentArea.top;

	if (pos.x == m_FocusRegion.x && pos.y == m_FocusRegion.y)
	{//小地图 同个区域
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			 m_FocusPosition.y + m_MapFocusOffset.y, false);
		return;
	}

    POINT OffsetPos;
    // 如果根据移动后Region为相对位移进行的预读
    OffsetPos.x = pos.x - m_FocusRegion.x + 1;
    OffsetPos.y = pos.y - m_FocusRegion.y + 1; 

    if (
        (OffsetPos.x >= 0) &&
        (OffsetPos.x <  3) &&
        (OffsetPos.y >= 0) &&
        (OffsetPos.y <  3)
    )
    {
        const KPrevLoadPosItem *pcPosOffsetItem = NULL;
        pcPosOffsetItem = &m_PrevLoadPosOffset[OffsetPos.y][OffsetPos.x];
        m_PreLoadPosItem.m_nNum = 0;

        int i = 0;
#ifdef WIN32
        EnterCriticalSection(&m_RegionListAdjustCritical);
#else
        pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
        for (i = 0; i < (pcPosOffsetItem->m_nNum); i++)
        {
            // 如果根据移动后Region为相对位移进行的预读
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = pos.x + pcPosOffsetItem->m_Pos[i].x; 
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = pos.y + pcPosOffsetItem->m_Pos[i].y;

            // 如果根据移动前Region为相对位移进行的预读
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = 
            //    m_FocusRegion.x + pcPosOffsetItem->m_Pos[i].x;   
            //
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = 
            //    m_FocusRegion.y + pcPosOffsetItem->m_Pos[i].y;
                
            m_PreLoadPosItem.m_nNum++;   
        }
#ifdef WIN32
        LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
        pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
    }
    else
    {
        // 如果是超过预加载的范围，清空，不需要加载
        m_PreLoadPosItem.m_nNum = 0;
    }
	//小地图
	m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
		m_FocusPosition.y + m_MapFocusOffset.y, true);

	m_FocusMoveOffset.cx += pos.x - m_FocusRegion.x;  //偏移了多少个区域
	m_FocusMoveOffset.cy += pos.y - m_FocusRegion.y;  //

	m_FocusRegion.x   = pos.x;  //设置新的所在的区域值
	m_FocusRegion.y   = pos.y;

	m_ObjectsTree.SetLightenAreaLeftTopPos(
		(m_FocusRegion.x  - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH,
		(m_FocusRegion.y  - 1) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT);

	ClearPreprocess(false);

	if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cx <= -SPWP_TRIGGER_RANGE ||
		m_FocusMoveOffset.cy >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cy <= -SPWP_TRIGGER_RANGE)
	{

		if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cx <= -SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cy >= SPWP_TRIGGER_LOADING_RANGE  ||
			m_FocusMoveOffset.cy <= -SPWP_TRIGGER_LOADING_RANGE)
		{//设置加载状态
			SetLoadingStatus(true);
		}
		//改变加载的区域
		ChangeLoadArea();

		m_FocusMoveOffset.cx = 0;
		m_FocusMoveOffset.cy = 0;
	}
	ChangeProcessArea(isClearAll);
}

void KScenePlaceC::SetRepresentAreaSize(int nWidth, int nHeight)
{
	m_RepresentExactHalfSize.cx = nWidth / 2 + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentExactHalfSize.cy = nHeight + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	m_RepresentArea.right = m_RepresentArea.left + nWidth +
		SPWP_REPRESENT_RECT_WINDAGE_X + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentArea.bottom = m_RepresentArea.top + 
		(nHeight + SPWP_REPRESENT_RECT_WINDAGE_T + SPWP_REPRESENT_RECT_WINDAGE_B) * 2;
}

void KScenePlaceC::GetFocusPosition(int& nX, int& nY, int& nZ)
{
	nX = m_FocusPosition.x;
	nY = m_FocusPosition.y;
	nZ = 0;
}

//##ModelId=3DCD58AC00BC
void KScenePlaceC::Terminate()
{
	if (m_bInited == false)
		return ;
	ClosePlace();

	m_Map.Terminate();

	//触发子线程退出执行
	m_FocusRegion.x = SPWP_FARAWAY_COORD;
#ifdef WIN32
	SetEvent(m_hLoadRegionEvent);
	//等待子线程关闭
	DWORD	dwExitCode;
	if (GetExitCodeThread(m_hLoadAndPreprocessThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
		WaitForSingleObject(m_hLoadAndPreprocessThread, INFINITE);
	CloseHandle(m_hLoadAndPreprocessThread);
	m_hLoadAndPreprocessThread = NULL;
	
	DeleteCriticalSection(&m_ProcessCritical);
	DeleteCriticalSection(&m_LoadCritical);
	DeleteCriticalSection(&m_RegionListAdjustCritical);

	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;
#else
	SetEvent(m_hLoadRegionEvent);
	pthread_mutex_destroy(&m_ProcessCritical);
	pthread_mutex_destroy(&m_LoadCritical);
	pthread_mutex_destroy(&m_RegionListAdjustCritical);

	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;
#endif
	m_bPreprocessEvent = false;

	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
	    m_ImageStore.FreeImage(m_RegionGroundImages[i].szImage);
		m_RegionGroundImages[i].szImage[0] = 0;
		m_RegionGroundImages[i].uImage = 0;
	}
	m_nNumGroundImagesAvailable = 0;
	m_bInited = false;
#ifndef WIN32

#endif
}

// 预加载地图上的图素
void KScenePlaceC::PreLoadProcess()
{
    if (m_PreLoadPosItem.m_nNum == 0)
        return;

    int i = 0;
    int j = 0;
    m_nPrevLoadFileCount = 0;
#ifdef WIN32
    EnterCriticalSection(&m_RegionListAdjustCritical);
#else
    pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
    for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
    {
        int nRegionX = 0;
        int nRegionY = 0;
        
        if (!m_pRegions[i])
            continue;

        m_pRegions[i]->GetRegionIndex(nRegionX, nRegionY);

        for (j = 0; j < m_PreLoadPosItem.m_nNum; j++)
        {
            if (
                (m_PreLoadPosItem.m_Pos[j].x == nRegionX) &&
                (m_PreLoadPosItem.m_Pos[j].y == nRegionY)
            )
            {
                break;
            }
        }

        if (j >= m_PreLoadPosItem.m_nNum)
            continue;   // 如果没有找到匹配的项，则跳到下一个

        
        KBuildinObj *pObjsList = NULL;
        unsigned uNumObjs      = 0;

        m_pRegions[i]->GetBIOSBuildinObjs(pObjsList, uNumObjs); 

        for (NULL; uNumObjs > 0; uNumObjs--, pObjsList++)
        {
            memcpy(
                m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].szName, 
                pObjsList->szImage, 
                MAX_RESOURCE_FILE_NAME_LEN
            );   
            m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].nFrame = pObjsList->nFrame;

            m_nPrevLoadFileCount++;
            if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
                break;
        }
        if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
            break;
    }
    m_PreLoadPosItem.m_nNum = 0;
#ifdef WIN32
    LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
    pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
    if (m_nPrevLoadFileCount == 0)
        return;
    
     /*for (i = 0; i < m_nPrevLoadFileCount; i++)
    {
       KRPosition2 Offset;
        KRPosition2 Size;
        m_ImageStore.GetImageFrameParam(
            m_PrevLoadFileNameAndFrames[i].szName,
			            ISI_T_SPR,
            m_PrevLoadFileNameAndFrames[i].nFrame,
            &Offset,
            &Size
        );
    }*/

}

//##ModelId=3DCB6BC90345
void KScenePlaceC::LoadProcess()
{
#ifdef WIN32
    DWORD dwRetCode = 0;

	while(true)
	{
		//CCMessageBox("线程运行中","线程运行中");
        dwRetCode = WaitForSingleObject(m_hLoadRegionEvent, 1000);
        if (dwRetCode == WAIT_OBJECT_0)
        {
			if (m_FocusRegion.x == SPWP_FARAWAY_COORD)
				break;
			KScenePlaceRegionC*	pRegion = NULL;
			EnterCriticalSection(&m_RegionListAdjustCritical);
			if (m_nFirstToLoadIndex >= 0)
				pRegion = m_pRegions[m_nFirstToLoadIndex];
			LeaveCriticalSection(&m_RegionListAdjustCritical);

			if (pRegion)
			{//加载这个区域的 各种数据
				EnterCriticalSection(&m_LoadCritical);
				pRegion->LoadRe(m_szPlaceRootPath);	 //客户端导入障碍
				LeaveCriticalSection(&m_LoadCritical);
				//if (pRegion->getStatus() == 3)         //已将加载完成
				ARegionLoaded(pRegion);
				//else
				//	continue;
				//ARegionLoaded(pRegion);
			}
		}
        else if (dwRetCode == WAIT_TIMEOUT)
        {
            /*if (m_nSceneId == SPWP_NO_SCENE)
                continue;

            if (m_nFirstToLoadIndex >= 0)
                continue;   // 说明还在加载过程中

            PreLoadProcess();*/
			m_PreLoadPosItem.m_nNum = 0;
        }
	}
#endif
}

//##ModelId=3DCCD131018C
#ifdef WIN32
DWORD WINAPI KScenePlaceC::LoadThreadEntrance(void* pParam)
{
	if (pParam)
		((KScenePlaceC*)pParam)->LoadProcess();
	return 0;
}
#endif
//##ModelId=3DBDBC7200B4
void KScenePlaceC::SetRegionsToLoad()
{
	if (m_FocusMoveOffset.cx == 0 && m_FocusMoveOffset.cy == 0)
		return;
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
#else
	pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif

	//可能正在加载中的区域
	KScenePlaceRegionC* pMayLoadingRegion = NULL;
	if (m_nFirstToLoadIndex >= 0)
		pMayLoadingRegion = m_pRegions[m_nFirstToLoadIndex];

	KScenePlaceRegionC* pTempRegions[SPWP_MAX_NUM_REGIONS];
	int nFirst = 0;
	int nLast = SPWP_MAX_NUM_REGIONS - 1;
	int i, nBourn, nX, nY;
	
	if (m_nFirstToLoadIndex < 0)
		nBourn = SPWP_MAX_NUM_REGIONS;
	else
		nBourn	= m_nFirstToLoadIndex;

	for (i = 0; i < nBourn; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	//设置新的下个起始加载区域在区域指针列表中的索引
	m_nFirstToLoadIndex = (nFirst < SPWP_MAX_NUM_REGIONS) ? nFirst : -1;

	for (i = nBourn; i < SPWP_MAX_NUM_REGIONS; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	int nNewLoadIdx[SPWP_MAX_NUM_REGIONS];
	int nCount = 0;

	if (m_FocusMoveOffset.cx * m_FocusMoveOffset.cx > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 ||
		m_FocusMoveOffset.cy * m_FocusMoveOffset.cy > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 )
	{
		memcpy(nNewLoadIdx, m_PRIIdxTable, sizeof(m_PRIIdxTable));
		nCount = SPWP_MAX_NUM_REGIONS;
	}
	else
	{
		int nBeginX, nEndX, nBeginY, nEndY;
		if (m_FocusMoveOffset.cx > 0)
		{
			nBeginX = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cx;
			nEndX   = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginX = 0;
			nEndX = -m_FocusMoveOffset.cx - 1;
		}
		
		if (m_FocusMoveOffset.cy > 0)
		{
			nBeginY = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cy;
			nEndY   = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginY = 0;
			nEndY = -m_FocusMoveOffset.cy - 1;
		}

		for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		{
			if (
				(m_RangePosTable[m_PRIIdxTable[i]].x >= nBeginX 
				&& m_RangePosTable[m_PRIIdxTable[i]].x <= nEndX)
				||
				(m_RangePosTable[m_PRIIdxTable[i]].y >= nBeginY 
				&& m_RangePosTable[m_PRIIdxTable[i]].y <= nEndY)
				)
			{
				nNewLoadIdx[nCount++] = m_PRIIdxTable[i];
			}
		}
	}

	//CCAssert(nFirst + nCount == SPWP_MAX_NUM_REGIONS,"");
	if (nFirst + nCount != SPWP_MAX_NUM_REGIONS)
		return;

	for (i = 0; i < nCount; i++)
	{
		if (pMayLoadingRegion == pTempRegions[nFirst + i])
		{
			//确保没有区域对象正在执行加载，如果有则等待这个区域对象加载结束
#ifdef WIN32
			EnterCriticalSection(&m_LoadCritical);
			LeaveCriticalSection(&m_LoadCritical);
#else
			pthread_mutex_lock(&m_LoadCritical);
			pthread_mutex_unlock(&m_LoadCritical);
#endif
			//运行到此处，此时可能为“一个区域刚加载完毕，但是还未及更新区域
			//指针列表的下个加载区域索引。”的情况
			//下面的处理把它当作还未加载处理。
		}

		pTempRegions[nFirst + i]->ToLoad(m_FocusRegion.x + m_RangePosTable[nNewLoadIdx[i]].x - SPWP_LOAD_EXTEND_RANGE,
			m_FocusRegion.y + m_RangePosTable[nNewLoadIdx[i]].y - SPWP_LOAD_EXTEND_RANGE);
	}
	memcpy(m_pRegions, pTempRegions, sizeof(m_pRegions));
#ifdef WIN32
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	if (nCount)
		SetEvent(m_hLoadRegionEvent);
#else
	pthread_mutex_unlock(&m_RegionListAdjustCritical);
	if (nCount)
		SetEvent(m_hLoadRegionEvent);
#endif
}

//##ModelId=3DCAA6B90196
unsigned int KScenePlaceC::AddObject(unsigned int uGenre, int nId, int x, int y, int z,
									 int eLayerParam)
{
	POINT	ri;
	KIpotRuntimeObj* pLeaf = NULL;
	ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH; //区域索引
	ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;//区域索引

	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, 1))
	{//如果在主角的所在区域的范围内
		pLeaf = (KIpotRuntimeObj*)malloc(sizeof(KIpotRuntimeObj));

		if (pLeaf)
		{
			pLeaf->eLeafType = pLeaf->IPOTL_T_RUNTIME_OBJ;
			pLeaf->uGenre = uGenre;
			pLeaf->nId = nId;
			pLeaf->oPosition.x = x;  //像素坐标
			pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;
			pLeaf->nPositionZ = z;
			pLeaf->pAheadBrother = NULL;
			pLeaf->pBrother = NULL;
			pLeaf->pLChild = NULL;
			pLeaf->pRChild = NULL;
			pLeaf->pParentLeaf = NULL;
			pLeaf->pParentBranch = NULL;
			pLeaf->eLayerParam = eLayerParam;
#ifdef WIN32
			EnterCriticalSection(&m_ProcessCritical);
#else
	        pthread_mutex_lock(&m_ProcessCritical);
#endif
			m_ObjectsTree.AddLeafPoint(pLeaf);
#ifdef WIN32
			LeaveCriticalSection(&m_ProcessCritical);
#else
	        pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
		} 
	}

	return ((unsigned int)pLeaf);
}

//##ModelId=3DCAA7000085
unsigned int KScenePlaceC::MoveObject(unsigned int uGenre, int nId,  int x, int y, int z,
									  unsigned int& uRtoid, int eLayerParam)
{

	if (uRtoid == 0)
	{//绘画子弹
		uRtoid = AddObject(uGenre, nId, x, y, z, eLayerParam);
		return uRtoid;
	}

	KScenePlaceRegionC* pRegion = NULL;
	KIpotRuntimeObj* pLeaf = (KIpotRuntimeObj*)uRtoid;
	POINT	ri;
	        ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	        ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, 1))
	{
		pLeaf->uGenre = uGenre;
		pLeaf->nId = nId;
		pLeaf->nPositionZ = z;
		pLeaf->eLayerParam = eLayerParam;

		if (pLeaf->oPosition.x != x || pLeaf->oPosition.y != y + POINT_LEAF_Y_ADJUST_VALUE)
		{
#ifdef WIN32
			    EnterCriticalSection(&m_ProcessCritical);
#else
			    pthread_mutex_lock(&m_ProcessCritical);
#endif
			//if (TryEnterCriticalSection(&m_ProcessCritical))
				if (pLeaf)
				   m_ObjectsTree.PluckRto(pLeaf);  //先删除

				pLeaf->oPosition.x = x;
				pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;

				if (pLeaf)
				   m_ObjectsTree.AddLeafPoint(pLeaf);
#ifdef WIN32
			    LeaveCriticalSection(&m_ProcessCritical);
#else
				pthread_mutex_unlock(&m_ProcessCritical);
#endif
		}
	}
	else
	{
#ifdef WIN32
		  EnterCriticalSection(&m_ProcessCritical);
#else
		  pthread_mutex_lock(&m_ProcessCritical);
#endif
		//if (TryEnterCriticalSection(&m_ProcessCritical))
		  if (pLeaf)
		      m_ObjectsTree.PluckRto(pLeaf);
#ifdef WIN32
		  LeaveCriticalSection(&m_ProcessCritical);
#else
		  pthread_mutex_unlock(&m_ProcessCritical);
#endif
		  if (pLeaf)
		  {  
			free(pLeaf);
		    pLeaf = NULL;
		  }  
		
		uRtoid = 0;
	}
	return uRtoid;
}

//##ModelId=3DCAA70603E3
void KScenePlaceC::RemoveObject(unsigned int uGenre, int nId, unsigned int& uRtoid)
{
	if (uRtoid)
	{
#ifdef WIN32
		EnterCriticalSection(&m_ProcessCritical);
		m_ObjectsTree.PluckRto((KIpotRuntimeObj*)uRtoid);
		LeaveCriticalSection(&m_ProcessCritical);
		free ((KIpotRuntimeObj*)uRtoid);
		uRtoid = NULL;
#else
		pthread_mutex_lock(&m_ProcessCritical);
		m_ObjectsTree.PluckRto((KIpotRuntimeObj*)uRtoid);
		pthread_mutex_unlock(&m_ProcessCritical);
		free ((KIpotRuntimeObj*)uRtoid);
		uRtoid = NULL;
#endif
	}

}
//场景循环函数
void KScenePlaceC::Breathe()
{

	//LoadProcess_New();
	//DWORD dwLight;
	//获取环境的灯光颜色
	//dwLight = m_EnLight.GetEnvironmentLight(m_nCurrentTime);
/*
	if(m_pWeather)
		m_pWeather->FilterAmbient(dwLight);
*/
	//SetAmbient(dwLight);

	if (m_bLoading)
	{
#ifdef WIN32
		WaitForSingleObject(m_hSwitchLoadFinishedEvent, SPWP_SWITCH_SCENE_TIMEOUT);
#else
		WaitForSingleObject(m_hSwitchLoadFinishedEvent, SPWP_SWITCH_SCENE_TIMEOUT);
#endif
		m_bRenderGround = true;
	}

	if (m_bPreprocessEvent)
	{
		m_bPreprocessEvent = false;
		Preprocess();
	}

	m_ObjectsTree.Breathe();

}

//##ModelId=3DCD7F0A0071
void KScenePlaceC::Paint()
{//场景中 的绘画函数
	IR_UpdateTime();
	if (!g_GameWorld)
		return;

	if (!m_ispaint)
	{
		g_GameWorld->ParentNode_map->removeAllChildren();
		g_GameWorld->ParentNode_Bigmap->removeAllChildren();
		g_GameWorld->ParentNode_objabove->removeAllChildren();
		return;
	}
	//CCMessageBox("绘画中","绘画中");
	if (m_bInited == false || m_szPlaceRootPath[0] == 0)
		return;

	if (m_bRenderGround)
	{
		m_bRenderGround = false;
		//PrerenderGround(false); //绘画路面
	}
#ifdef WIN32
	EnterCriticalSection(&m_ProcessCritical);
#else
	pthread_mutex_lock(&m_ProcessCritical);
#endif
	bool bPrerenderGroundImg = PaintBackGround();	//是否画了背景图

    //if (!bPrerenderGroundImg)    //如果存在	
	  // PaintBackGroundCloud();	 //画背景云

	unsigned int i;

	for (i = 0; i <SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)  //SPWP_NUM_REGIONS_IN_PROCESS_AREA
	{//绘画地图
		if (m_pInProcessAreaRegions[i])
		{//最低层 地面 路 草皮 水等
	    	m_pInProcessAreaRegions[i]->PaintGround(i);
			m_pInProcessAreaRegions[i]->PaintGround(i,0);
			m_pInProcessAreaRegions[i]->PaintBuildinObj(&m_RepresentArea,i,m_IsClearMap);
		}
	}

	//m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);
	//m_ObjectsTree.Paint(&m_RepresentArea,IPOT_RL_OBJECT);	 //人物 怪物 房屋 树木等
	//char msg[64];
	//t_sprintf(msg,"m_nNumObjsAbove:%d",m_nNumObjsAbove);
	//CCMessageBox(msg,"m_nNumObjsAbove");
	if (m_IsClearMap)
	{
		if (m_ClearState==0)
		{
			m_ClearState =1;//设置已经删除
			char nRegKey[32]={0},nKey[32]={0};
			t_sprintf(nRegKey,"above-0");
			std::string nCurRegKey = nRegKey;
			if (g_GameWorld)
			   (g_GameWorld->ParentNode_objabove)->removeChildByTagKey(nCurRegKey);
		}
	}
	else
	{
	  m_ClearState = 0;
	  for (i = 0; i < m_nNumObjsAbove; i++)  //有多少个 人物附近 9个区域内的高空物件
	  {//绘画上层 整个地图有多少个屋顶
		KScenePlaceRegionC::PaintAboveHeadObj(m_pObjsAbove[i],&m_RepresentArea,i);
	  }
	}
	//m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);  //名字 称号等  有BUG

	// 绘制天气对象
	//if(m_pWeather)
	//	m_pWeather->Render(g_pRepresent);

	//PaintOverCloud();	 //绘画 上层云
#ifdef WIN32
	LeaveCriticalSection(&m_ProcessCritical);
#else
	pthread_mutex_unlock(&m_ProcessCritical);
#endif

}

BOOL KScenePlaceC::GetMapNextMovePos()
{
	int nReg=FALSE;
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			if (m_pInProcessAreaRegions[i]->GetNextMapPos())
			{
				nReg=TRUE;
				break;
			}
		}
	}
	return nReg;
}




//##ModelId=3DCE68BB0238
void KScenePlaceC::ChangeLoadArea()
{
	SetRegionsToLoad();
}
//点击回程
void KScenePlaceC::ClearProcessArea()
{
	if (g_GameWorld)
		g_GameWorld->FreeAllSprite();
	//换地图的时候全部删除精灵
	
	/*int h,v,i;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{//清空所有区域
		if (m_pInProcessAreaRegions[i])
		{//#define INSIDE_AREA(h, v, range) ( ((h) - m_FocusRegion.x) * ((h) - m_FocusRegion.x) <= (range * range) && ((v) - m_FocusRegion::y) * ((v) - m_FocusRegion::y) <= (range * range) )
			m_pInProcessAreaRegions[i]->GetRegionIndex(h, v);
			char nRegKey[32]={0};
			if (g_GameWorld)
			{
				t_sprintf(nRegKey,"%d-%d-dimian",h,v);
				std::string nCurRegKey = nRegKey;
				CCNode *nRegNode = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurRegKey);//如果区域节点存在

				if (nRegNode)
					(g_GameWorld->ParentNode_map)->removeChild(nRegNode);

				t_sprintf(nRegKey,"%d-%d",h,v);
				nCurRegKey = nRegKey;
				nRegNode = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurRegKey);//如果区域节点存在
				if (nRegNode)
				{//%d-%d-dimian
					CCNode *nPartNode_A= nRegNode->getChildByTag(MAP_NODE_LUMIAN_A);
					if (nPartNode_A)
						nPartNode_A->removeAllChildren();

					CCNode *nPartNode_B= nRegNode->getChildByTag(MAP_NODE_PART);
					if (nPartNode_B)
						nPartNode_B->removeAllChildren();
				}

				CCNode *nRegBigNode = (g_GameWorld->ParentNode_Bigmap)->getChildByTagKey(nCurRegKey);//如果区域节点存在
				if (nRegBigNode)
				{
					CCNode *nPartNode_A= nRegBigNode->getChildByTag(MAP_NODE_TREES);
					if (nPartNode_A)
					{
						//CCMessageBox("全部删除","MAP_NODE_TREES");
						nPartNode_A->removeAllChildren();
					}

					CCNode *nPartNode_B= nRegBigNode->getChildByTag(MAP_NODE_HOUSE);
					if (nPartNode_B)
						nPartNode_B->removeAllChildren();
				}
			}
		}
	}*/
}

//##ModelId=3DBF946D0053
void KScenePlaceC::ChangeProcessArea(bool isClearAll)
{
	KRUImage*	pImage = NULL;
	/*RECT	rc;
	rc.left = (m_FocusRegion.x - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.top  = (m_FocusRegion.y - 1) *  KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	rc.right  = rc.left + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.bottom = rc.top  + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	*/
#ifdef WIN32
	EnterCriticalSection(&m_ProcessCritical);
#else
	pthread_mutex_lock(&m_ProcessCritical);
#endif
	int h,v,i;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{//清空所有区域
		if (m_pInProcessAreaRegions[i])
		{//#define INSIDE_AREA(h, v, range) ( ((h) - m_FocusRegion.x) * ((h) - m_FocusRegion.x) <= (range * range) && ((v) - m_FocusRegion::y) * ((v) - m_FocusRegion::y) <= (range * range) )
			  m_pInProcessAreaRegions[i]->GetRegionIndex(h, v);
			  if (!isClearAll && INSIDE_AREA(h, v, 1) == 0)
			  {//只有一个区域没有清除
				m_pInProcessAreaRegions[i]->LeaveProcessArea(); //离开原来的区域
				char nRegKey[32]={0};
			   if (g_GameWorld)
			   {
					t_sprintf(nRegKey,"%d-%d-dimian",h,v);
					std::string nfullKey = nRegKey;
				    (g_GameWorld->ParentNode_map)->removeChildByTagKey(nfullKey,true);
					
					t_sprintf(nRegKey,"%d-%d-part",h,v);
					std::string  nPartKey = nRegKey;
					(g_GameWorld->ParentNode_map)->removeChildByTagKey(nPartKey,true);
					
					CCNode *nRegNode = NULL; 
					/*= (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurRegKey);//如果区域节点存
				    if (nRegNode)
						(g_GameWorld->ParentNode_map)->removeChild(nRegNode);
					*/
					t_sprintf(nRegKey,"%d-%d",h,v);
					std::string   nBigMapKey = nRegKey;
					/*nRegNode = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurRegKey);//如果区域节点存在
				   if (nRegNode)
				   {//%d-%d-dimian
					   nRegNode->removeAllChildren();
					   //nRegNode->removeChildByTag(MAP_NODE_LUMIAN_A,true);
					   //nRegNode->removeChildByTag(MAP_NODE_PART,true);
				   }*/
				   CCNode *nRegBigNode = (g_GameWorld->ParentNode_Bigmap)->getChildByTagKey(nBigMapKey);//如果区域节点存在
				   if (nRegBigNode)
				   {
					   nRegBigNode->removeAllChildren();
					   //nRegBigNode->removeChildByTag(MAP_NODE_TREES,true);
					   //nRegBigNode->removeChildByTag(MAP_NODE_HOUSE,true);
				   }
			   }
			  }
			   m_pInProcessAreaRegions[i] = NULL;
		}
	}
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
#else
    pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
	int nNum;
	if (m_nFirstToLoadIndex < 0)
		nNum = SPWP_MAX_NUM_REGIONS;
	else
		nNum = m_nFirstToLoadIndex;
	for (i = 0; i < nNum; i++)
	{
		m_pRegions[i]->GetRegionIndex(h, v);

		if (INSIDE_AREA(h, v, 1))
		{
			GET_IN_PROCESS_AREA_REGION(h, v) = m_pRegions[i];
			pImage = m_pRegions[i]->GetPrerenderGroundImage();
			if (pImage == NULL && l_bPrerenderGround)
			{//是否重新渲染
				pImage = GetFreeGroundImage();
				//_ASSERT(pImage);
				//if (!pImage)
				//	continue;
			}
			m_pRegions[i]->EnterProcessArea(pImage); //进入这个区域
		}
	}
#ifdef WIN32
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	LeaveCriticalSection(&m_ProcessCritical);
#else
    pthread_mutex_unlock(&m_RegionListAdjustCritical);
	pthread_mutex_unlock(&m_ProcessCritical);
#endif

	if (m_nFirstToLoadIndex < 0 || m_nFirstToLoadIndex >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		m_bPreprocessEvent = true;
		m_bRenderGround = true;
	}
}

void KScenePlaceC::PrerenderGround(bool bForce)
{
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
			m_pInProcessAreaRegions[i]->PrerenderGround(bForce);
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
pthread_mutex_lock(&m_RegionListAdjustCritical);
for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
{
	if (m_pInProcessAreaRegions[i])
		m_pInProcessAreaRegions[i]->PrerenderGround(bForce);
}
pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
}

//##ModelId=3DBFA1460230
void KScenePlaceC::Preprocess()
{
	unsigned int i, j, dx, dy, nTotalLineObj = 0;

	struct
	{
		KIpotBuildinObj*	pObjsPoint;
		KIpotBuildinObj*	pObjsLine;
		KIpotBuildinObj*	pObjsTree;
		KBuildinObj*		pObjsAbove;
		unsigned int		nNumObjsPoint;
		unsigned int		nNumObjsLine;
		unsigned int		nNumObjsTree;
		unsigned int		nNumObjsAbove;
	}RegionRtoData[SPWP_NUM_REGIONS_IN_PROCESS_AREA] = { 0 };
#ifdef WIN32
	EnterCriticalSection(&m_ProcessCritical);
#else
    pthread_mutex_lock(&m_ProcessCritical);
#endif

	ClearPreprocess(false);

	m_ObjectsTree.SetPermanentBranchPos(
		m_FocusPosition.x - KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.x + KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.y - KScenePlaceRegionC::RWPP_AREGION_HEIGHT * 2);

	//--------获取内建对象的列表----------
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->GetBuildinObjs(
				RegionRtoData[i].pObjsPoint, RegionRtoData[i].nNumObjsPoint,
				RegionRtoData[i].pObjsLine, RegionRtoData[i].nNumObjsLine,
				RegionRtoData[i].pObjsTree, RegionRtoData[i].nNumObjsTree);
			nTotalLineObj += RegionRtoData[i].nNumObjsLine;
			//获取这个区域的高空数量
			RegionRtoData[i].nNumObjsAbove = m_pInProcessAreaRegions[i]->
				GetAboveHeadLayer(RegionRtoData[i].pObjsAbove);
			m_nNumObjsAbove += RegionRtoData[i].nNumObjsAbove;
		}
	}

	//--------处理高空对象---------
	if (m_nNumObjsAbove)
	{
		m_pObjsAbove = (KBuildinObj**)malloc(sizeof(KBuildinObj*) * m_nNumObjsAbove);
		m_nNumObjsAbove = 0;
		if (m_pObjsAbove)
		{
			for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			{
				while(RegionRtoData[i].nNumObjsAbove)
				{
					RegionRtoData[i].nNumObjsAbove--;
					KBuildinObj* pBio = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					for (j = 0; j < m_nNumObjsAbove; j++)
					{
						KBuildinObj* pBio2 = m_pObjsAbove[j];
						if ((pBio->oPos1.y < pBio2->oPos1.y) ||
							(pBio->oPos1.y == pBio2->oPos1.y &&
								(pBio->oPos1.z < pBio2->oPos1.z)))
						{
							break;
						}
					}
					for (unsigned int k = m_nNumObjsAbove; k > j; k--)
						m_pObjsAbove[k] = m_pObjsAbove[k - 1];
					m_pObjsAbove[j] = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					m_nNumObjsAbove ++;
				}
			}
		}			
	}

	//--------处理树方式排序的对象---------
	class TreeObjSet : public KNode
	{
	public:
		float	fAngleXY;
		float	fNodicalY;
		int		nLength2;
		POINT	oLP1, oLP2;
		KIpotBuildinObj*	pObjs;
	};

	KList		List;
	TreeObjSet	*pNode1 = NULL, *pNode2 = NULL;
	KIpotBuildinObj* pObj = NULL;

	//---把同在一条直线上的连在一起--
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsTree; j++)
		{
			pObj = &RegionRtoData[i].pObjsTree[j];
			
			pNode1 = (TreeObjSet*)List.GetHead();
			while (pNode1)
			{
				if (SM_IsLineLinkable(pObj->pBio->fAngleXY,
					pObj->pBio->fNodicalY,
					pNode1->fAngleXY, pNode1->fNodicalY))
				{
					break;
				}
				pNode1 = (TreeObjSet*)pNode1->GetNext();
			};

			if (pNode1 == NULL)
			{
				if ((pNode1 = new TreeObjSet) == NULL)
				{
					i = SPWP_NUM_REGIONS_IN_PROCESS_AREA;
					break;
				}
				pNode1->pObjs = pObj;
				pNode1->oLP1 = pObj->oPosition;
				pNode1->oLP2 = pObj->oEndPos;
				pNode1->fAngleXY  = pObj->fAngleXY  = pObj->pBio->fAngleXY;
				pNode1->fNodicalY = pObj->fNodicalY = pObj->pBio->fNodicalY;
				List.AddHead(pNode1);
				continue;
			}
			
			pObj->fAngleXY  = pNode1->fAngleXY;
			pObj->fNodicalY = pNode1->fNodicalY;

			int x = pObj->oPosition.x;

			if (x <  pNode1->pObjs->oPosition.x)
			{
				pObj->pBrother = pNode1->pObjs;
				pNode1->pObjs = pObj;
			}
			else
			{
				KIpotBuildinObj* pSortStop = pNode1->pObjs;
				while(pSortStop->pBrother)
				{
					if (x < ((KIpotBuildinObj*)pSortStop->pBrother)->oPosition.x)
						break;
					pSortStop = (KIpotBuildinObj*)pSortStop->pBrother;
				};
				pObj->pBrother = pSortStop->pBrother;
				pSortStop->pBrother = pObj;
			}
			if (pNode1->oLP1.x > pObj->oPosition.x)
				pNode1->oLP1 = pObj->oPosition;
			if (pNode1->oLP2.x < pObj->oEndPos.x)
				pNode1->oLP2 = pObj->oEndPos;
		}
	}
	
	//----把线条组合按长度排序----
	pNode1 = (TreeObjSet*)List.GetHead();
	while(pNode1)
	{
		dx = pNode1->oLP1.x - pNode1->oLP2.x;
		dy = pNode1->oLP1.y - pNode1->oLP2.y;
		pNode1->nLength2 = dx * dx + dy * dy;
		pNode1 = (TreeObjSet*)pNode1->GetNext();
	};

	KList		List2;
	while(pNode1 = (TreeObjSet*)List.GetHead())
	{
		pNode2 = (TreeObjSet*)pNode1->GetNext();
		while(pNode2)
		{
			if (pNode1->nLength2 < pNode2->nLength2)
				pNode1 = pNode2;
			pNode2 = (TreeObjSet*)pNode2->GetNext();
		};
		pNode1->Remove();
		List2.AddTail(pNode1);
	};
	
	//----把按树方式排序的对象加入对象树-----
	while(pNode1 = (TreeObjSet*)List2.RemoveHead())
	{
		while(pObj = pNode1->pObjs)
		{
			pNode1->pObjs = (KIpotBuildinObj*)pObj->pBrother;
			pObj->pBrother = NULL;
			m_ObjectsTree.AddBranch(pObj);
		}
		delete pNode1;	
		pNode1=NULL;
	};

	//----把线方式排序的对象进行根据线长度排序-----
	if (nTotalLineObj)
	{
		struct LineObjItem//---处理线方式排序的对象-----
		{
			int		nLength2;
			KIpotBuildinObj*	pObj;
		};
		LineObjItem* pNodeList = (LineObjItem*)malloc(sizeof(LineObjItem) * nTotalLineObj);
		nTotalLineObj = 0;
		if (pNodeList)
		{
			for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			{
				for (j = 0; j < RegionRtoData[i].nNumObjsLine; j++)
				{
					KIpotBuildinObj* pObj = &RegionRtoData[i].pObjsLine[j];
					dx = pObj->oEndPos.x - pObj->oPosition.x;
					dy = pObj->oEndPos.y - pObj->oPosition.y;
					int nLength2 = dx * dx + dy * dy;
					unsigned int k;
					for (k = 0; k < nTotalLineObj; k++)
					{
						if (nLength2 > pNodeList[k].nLength2)
						{
							for (unsigned int j = nTotalLineObj; j > k; j--)
								pNodeList[j] = pNodeList[j - 1];
							pNodeList[k].pObj = pObj;
							pNodeList[k].nLength2 = nLength2;
							break;
						}
					}
					if (k == nTotalLineObj)
					{
						pNodeList[nTotalLineObj].pObj = pObj;
						pNodeList[nTotalLineObj].nLength2 = nLength2;
					}
					nTotalLineObj++;
				}
			}
			//----把线方式排序的对象加入对象树-----
			for (i = 0; i < nTotalLineObj; i++)
			{
				m_ObjectsTree.AddLeafLine(pNodeList[i].pObj);
			}
			free(pNodeList);
			pNodeList = NULL;
		}
	}

	//----把点方式排序的对象加入对象树-----
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsPoint; j++)
		{
			m_ObjectsTree.AddLeafPoint(&RegionRtoData[i].pObjsPoint[j]);
		}
	}

	
	//----把场景内建光源加入树-----
	/*KBuildInLightInfo* pLights = NULL;
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{

		if (m_pInProcessAreaRegions[i])
		{
			j = m_pInProcessAreaRegions[i]->GetBuildinLights(pLights);  //内建光源
			if (j)
				m_ObjectsTree.AddBuildinLight(pLights, j);			
		}
	}*/

	RECT	KeepArea;
	KeepArea.left = (m_FocusRegion.x - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH - SPWP_RTO_HALF_RANGE;
	KeepArea.right = KeepArea.left + KScenePlaceRegionC::RWPP_AREGION_WIDTH * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	KeepArea.top = (m_FocusRegion.y - 1) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT - SPWP_RTO_HALF_RANGE;
	KeepArea.bottom = KeepArea.top + KScenePlaceRegionC::RWPP_AREGION_HEIGHT * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	m_ObjectsTree.StrewRtoLeafs(KeepArea);
#ifdef WIN32
	LeaveCriticalSection(&m_ProcessCritical);
#else
    pthread_mutex_unlock(&m_ProcessCritical);
#endif
}

//##ModelId=3DCCBD7B0239
void KScenePlaceC::ClearPreprocess(int bIncludeRto)
{
	if (m_pObjsAbove)
	{
		free(m_pObjsAbove);
		m_pObjsAbove = NULL;

		char nRegKey[32]={0};//整个9大区域的总结点
		t_sprintf(nRegKey,"above-0");
		std::string nCurRegKey = nRegKey;
		if (g_GameWorld)
			g_GameWorld->ParentNode_objabove->removeChildByTagKey(nCurRegKey,true);
			//CCNode *nRegNode = g_GameWorld->ParentNode_Bigmap->getChildByTagKey(nCurRegKey);//如果区域节点存在
	}
	m_nNumObjsAbove = 0;
#ifdef WIN32
	EnterCriticalSection(&m_ProcessCritical);
	if (bIncludeRto == false)
		m_ObjectsTree.Fell();
	else
		m_ObjectsTree.Clear();
	LeaveCriticalSection(&m_ProcessCritical);
#else
//	pthread_mutex_lock(&m_ProcessCritical);
	if (bIncludeRto == false)
		m_ObjectsTree.Fell();
	else
		m_ObjectsTree.Clear();
//	pthread_mutex_unlock(&m_ProcessCritical);
#endif

}

void KScenePlaceC::ProjectDistToSpaceDist(int& nXDistance, int& nYDistance)
{
	nYDistance = nYDistance + nYDistance;
}

void KScenePlaceC::ViewPortCoordToSpaceCoord(int& nX, int& nY, int nZ)
{
	if (g_GameWorld)//屏幕坐标---像素坐标
	    SubWorld[0].GetMpsByLocalPosition(nX, nY, nZ);
	else
	{
		nX = nX + m_RepresentArea.left + SPWP_REPRESENT_RECT_WINDAGE_X;
		nY = (nY + ((nZ * 887) >> 10)) * 2 + m_RepresentArea.top  + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	}
}

void KScenePlaceC::GetRegionLeftTopPos(int nRegionX, int nRegionY, int& nLeft, int& nTop)
{
	nLeft = nRegionX * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	nTop  = nRegionY * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
}

void KScenePlaceC::ARegionLoaded(KScenePlaceRegionC* pRegion)
{//登陆时候所在的区域
	//CCAssert(pRegion,"");
	if (pRegion==NULL)
		return;

	//如果刚加载完毕的区域属于预处理范围则触发预处理信号
	int	h, v;
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
#else
	pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
	int nCount;
	if (m_nFirstToLoadIndex >= 0)		
	{
		nCount = m_nFirstToLoadIndex;
		if (m_pRegions[m_nFirstToLoadIndex] == pRegion)
		{
			m_nFirstToLoadIndex++;
			if (m_nFirstToLoadIndex == SPWP_MAX_NUM_REGIONS)
				m_nFirstToLoadIndex = -1;
		}
	}
	else
	{
		nCount = SPWP_MAX_NUM_REGIONS - 1;
	}
	for (h = 0; h < nCount; h++)
	{
		pRegion->SetNestRegion(m_pRegions[h]);
		m_pRegions[h]->SetNestRegion(pRegion);
	}
#ifdef WIN32
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	if (m_nFirstToLoadIndex < 0)//已经加载完毕 设置无信号
		ResetEvent(m_hLoadRegionEvent);
#else
    pthread_mutex_unlock(&m_RegionListAdjustCritical);
	if (m_nFirstToLoadIndex < 0)//已经加载完毕 设置无信号
		ResetEvent(m_hLoadRegionEvent);
#endif

	if (nCount >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		SetLoadingStatus(false);
		m_bRenderGround = true;
	}

	KRUImage* pImage = NULL;
	pRegion->GetRegionIndex(h, v);
	if (INSIDE_AREA(h, v, 1))
	{
#ifdef WIN32
		EnterCriticalSection(&m_ProcessCritical);
#else
        pthread_mutex_lock(&m_ProcessCritical);
#endif
		if (l_bPrerenderGround)
			pImage = GetFreeGroundImage();
//		_ASSERT(pImage);
		//if (pImage)
		   pRegion->EnterProcessArea(pImage);

		 GET_IN_PROCESS_AREA_REGION(h, v) = pRegion;

		if (nCount >= 8)
			m_bPreprocessEvent = true;
#ifdef WIN32
		LeaveCriticalSection(&m_ProcessCritical);
#else
        pthread_mutex_unlock(&m_ProcessCritical);
#endif
	}

	//处理highlight的special object
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT &&
		h == m_nHLSpecialObjectRegionX && v == m_nHLSpecialObjectRegionY)
	{
#ifdef WIN32
		EnterCriticalSection(&m_ProcessCritical);
		pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
		LeaveCriticalSection(&m_ProcessCritical);
#else
        pthread_mutex_lock(&m_ProcessCritical);
		pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
        pthread_mutex_unlock(&m_ProcessCritical);
#endif
	}

	if (m_pfunRegionLoadedCallback)
		m_pfunRegionLoadedCallback(h, v);
}

KRUImage* KScenePlaceC::GetFreeGroundImage()
{
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		if (m_RegionGroundImages[i].GROUND_IMG_OCCUPY_FLAG == false)
			return (&m_RegionGroundImages[i]);
	}
	return NULL;
}

long KScenePlaceC::GetObstacleInfo(int nX, int nY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;		//像素坐标
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;    //像素坐标
	
	if (INSIDE_AREA(ri.x, ri.y, 1))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfo(nX, nY);
	}
	return Obstacle_Normal;
}

long KScenePlaceC::GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	
	if (INSIDE_AREA(ri.x, ri.y, 1))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfoMin(nX, nY, nOffX, nOffY);
	}
	return Obstacle_Normal;
}

void KScenePlaceC::RepresentShellReset()
{
	m_bRenderGround = true;
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
		m_RegionGroundImages[i].GROUND_IMG_OK_FLAG = false;
}

//设置场景中一个区域被加载完毕后的回调函数
void  KScenePlaceC::SetRegionLoadedCallback(funScenePlaceRegionLoadedCallback pfunCallback)
{
	m_pfunRegionLoadedCallback = pfunCallback;
}

void  KScenePlaceC::SetHightLightSpecialObject(int nRegionX, int nRegionY, int nBioIndex)
{
	KScenePlaceRegionC* pRegion = NULL;
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
#else
	pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
	{
		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
#ifdef WIN32
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
#else
				pthread_mutex_lock(&m_ProcessCritical);
				pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				pthread_mutex_unlock(&m_ProcessCritical);
#endif
			}
		}

		m_nHLSpecialObjectRegionX = nRegionX;
		m_nHLSpecialObjectRegionY = nRegionY;
		m_nHLSpecialObjectBioIndex = nBioIndex;

		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
#ifdef WIN32
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
#else
				pthread_mutex_lock(&m_ProcessCritical);
				pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				pthread_mutex_unlock(&m_ProcessCritical);
#endif
			}
		}
	}
#ifdef WIN32
	LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
    pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
}

void  KScenePlaceC::UnsetHightLightSpecialObject()
{
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
	{
#ifdef WIN32
		EnterCriticalSection(&m_RegionListAdjustCritical);
		KScenePlaceRegionC* pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
		if (pRegion)
		{
			EnterCriticalSection(&m_ProcessCritical);
			pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
			LeaveCriticalSection(&m_ProcessCritical);
		}
		LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
		pthread_mutex_lock(&m_RegionListAdjustCritical);
		KScenePlaceRegionC* pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
		if (pRegion)
		{
			pthread_mutex_lock(&m_ProcessCritical);
			pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
			pthread_mutex_unlock(&m_ProcessCritical);
		}
        pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif


		m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	}
}

KScenePlaceRegionC*	KScenePlaceC::GetLoadedRegion(int h, int v)
{
#ifdef WIN32
	EnterCriticalSection(&m_RegionListAdjustCritical);
#else
     pthread_mutex_lock(&m_RegionListAdjustCritical);
#endif
	int nCount;
	KScenePlaceRegionC* pRegion = NULL;
	if (m_nFirstToLoadIndex < 0)
		nCount = SPWP_MAX_NUM_REGIONS;
	else
		nCount = m_nFirstToLoadIndex;
	for (int i = 0; i < nCount; i++)
	{
		int rh, rv;
		m_pRegions[i]->GetRegionIndex(rh, rv);
		if (rh == h && rv == v)
		{
			pRegion = m_pRegions[i];
			break;
		}
	}
#ifdef WIN32
	LeaveCriticalSection(&m_RegionListAdjustCritical);
#else
    pthread_mutex_unlock(&m_RegionListAdjustCritical);
#endif
	return pRegion;
}
//获取场景的名称和ID 坐标
void KScenePlaceC::GetSceneNameAndFocus(char* pszName,
										int& nId, 
										int& nX, 
										int& nY,
										char* pszType)
{
	//CCAssert(pszName,"");
	if (pszName==NULL) return;
	strcpy(pszName, m_szSceneName);
    strcpy(pszType,m_szMapType);
	nId = m_nSceneId;
	nX = (m_FocusPosition.x) / 32;
	nY = (m_FocusPosition.y) / 64;
}

void KScenePlaceC::GetSceneFocus(int& nId, int& nX, int& nY)
{
	nId = m_nSceneId;
	nX = m_FocusPosition.x;
	nY = m_FocusPosition.y;
}

void KScenePlaceC::SetAmbient(DWORD dwAmbient)
{
	m_ObjectsTree.SetAmbient(dwAmbient);
}

void KScenePlaceC::SetCurrentTime(DWORD dwCurrentTime)
{
	m_nCurrentTime = (dwCurrentTime/100)%1440;
	m_ObjectsTree.SetCurrentTime(m_nCurrentTime);
}

void KScenePlaceC::SetEnvironmentLight(int idx, BYTE r, BYTE g, BYTE b)
{
	m_EnLight.SetLight(r, g, b, idx);
}

void KScenePlaceC::EnableBioLights(bool bEnable)
{
	m_ObjectsTree.EnableBioLights(bEnable);
}

void KScenePlaceC::EnableDynamicLights(bool bEnable)
{
	m_ObjectsTree.EnableDynamicLights(bEnable);
}

void KScenePlaceC::ChangeWeather(int nWeatherID)
{
	/*if (m_bEnableWeather == false)
		return;
	if(m_pWeather)
	{
		if(nWeatherID == WEATHERID_NOTHING)
			m_pWeather->ShutDown();
		else
			return;
	}
	else
	{
		if(nWeatherID == WEATHERID_NOTHING)
			return;
		else
		{
			m_pWeather = new KWeatherRain;
			if(!m_pWeather->ReadInfoFromIniFile(nWeatherID))
			{
				delete m_pWeather;
				m_pWeather = NULL;
			}
		}
	}*/
}


/*void KScenePlaceC::LoadSymbol(int nSubWorldID)
{
	if (nSubWorldID)
		m_Map.LoadSymbol(nSubWorldID);
	else
		m_Map.LoadSymbol(SubWorld[0].m_SubWorldID);
}
 */


void KScenePlaceC::ClearBgInfo()
{
	m_nBGNum = 0;
	m_nOCNum = 0;
	m_nBCNum = 0;
	
	memset(m_pBGImg, 0, sizeof(m_pBGImg));
	m_bBGImg = false;
	memset(&m_pOCImg, 0, sizeof(m_pOCImg));
	m_bOCImg = false;
	memset(&m_pBCImg, 0, sizeof(m_pBCImg));
	m_bBCImg = false;

}

void KScenePlaceC::LoadIni(KIniFile *pIni)
{
	/*m_nBGNum = 0;
	m_nOCNum = 0;
	m_nBCNum = 0;

	memset(m_pBGImg, 0, sizeof(m_pBGImg));
	m_bBGImg = false;
	memset(&m_pOCImg, 0, sizeof(m_pOCImg));
	m_bOCImg = false;
	memset(&m_pBCImg, 0, sizeof(m_pBCImg));
	m_bBCImg = false;

	if (pIni)
	{
		int nCount = 0;
		    pIni->GetInteger("Main", "Count", 0, &nCount);
		
		if (nCount)
		{
			int i, j;
			char szSection[64]={0}, szKeyName[64]={0};
			for (i = 0; i < nCount; i ++)
			{
				int nValue = 0;
				t_sprintf(szSection, "%d", i);
				pIni->GetInteger(szSection, "Type", 0, &nValue);
				if (nValue)
				{
					switch (nValue)
					{
					case BACKGROUND_IMAGE:
						if (m_nBGNum < MAX_BACKGROUND_IMAGE)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							BGArea[m_nBGNum].left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							BGArea[m_nBGNum].top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							BGArea[m_nBGNum].right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							BGArea[m_nBGNum].bottom = nValue;

							m_pBGImg[m_nBGNum].nType = ISI_T_BITMAP16; //ISI_T_SPR;//
							m_pBGImg[m_nBGNum].bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
							m_pBGImg[m_nBGNum].uImage = 0;
							m_pBGImg[m_nBGNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pBGImg[m_nBGNum].bRenderFlag = 0;
							pIni->GetString(szSection, "Image", "", m_pBGImg[m_nBGNum].szImage, sizeof(m_pBGImg[m_nBGNum].szImage));
							pIni->GetInteger(szSection, "PicCenterPointX", 0, &m_pBGImg[m_nBGNum].oPosition.nX);
							pIni->GetInteger(szSection, "PicCenterPointY", 0, &m_pBGImg[m_nBGNum].oPosition.nY);
							m_pBGImg[m_nBGNum].oPosition.nZ = 0;
							m_pBGImg[m_nBGNum].nFrame = -1;
							//m_pBGImg[m_nBGNum].nFlipTime = IR_GetCurrentTime();
							m_pBGImg[m_nBGNum].nNumFrames = 0;
							if (g_pRepresent)
							{
								KImageParam	Param;
								if (g_pRepresent)
								{
								   if (g_pRepresent->GetImageParam(m_pBGImg[m_nBGNum].szImage, &Param, ISI_T_BITMAP16))
									//SetSize(Param.nWidth, Param.nHeight);
								   {
								     m_pBGImg[m_nBGNum].nNumFrames = Param.nNumFrames;
								     m_pBGImg[m_nBGNum].nInterval  = Param.nInterval;		//帧间隔
								     m_pBGImg[m_nBGNum].nDirs      = Param.nNumFramesGroup; //总方向数
								   }
								}
							}

							m_nBGNum++;
						}
						break;
					case OVER_CLOUD_IMAGE:
						for (j = 0; j < MAX_CLOUD_IMAGE; j ++)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							OCArea.left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							OCArea.top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							OCArea.right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							OCArea.bottom = nValue;
							m_pOCImg[m_nOCNum].nType = ISI_T_SPR;
							m_pOCImg[m_nOCNum].Color.Color_b.a = 255;
							m_pOCImg[m_nOCNum].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
							m_pOCImg[m_nOCNum].uImage = 0;
							m_pOCImg[m_nOCNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pOCImg[m_nOCNum].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
							t_sprintf(szKeyName, "Image%d", m_nOCNum);
							pIni->GetString(szSection, szKeyName, "", m_pOCImg[m_nOCNum].szImage, sizeof(m_pOCImg[m_nOCNum].szImage));
							m_pOCImg[m_nOCNum].oPosition.nX = ::GetRandomNumber(0, RESOLUTION_WIDTH_800);
							m_pOCImg[m_nOCNum].oPosition.nY = ::GetRandomNumber(0, RESOLUTION_HEIGHT_800);
							m_pOCImg[m_nOCNum].oPosition.nZ = 0;
							m_pOCImg[m_nOCNum].nFrame = 0;
							m_nOCNum++;
						}
						break;
					case BACKGROUND_CLOUD_IMAGE:
						for (j = 0; j < MAX_CLOUD_IMAGE; j ++)
						{
							pIni->GetInteger(szSection, "AreaLeft", 0, &nValue);
							BCArea.left = nValue;
							pIni->GetInteger(szSection, "AreaTop", 0, &nValue);
							BCArea.top = nValue;
							pIni->GetInteger(szSection, "AreaRight", 0, &nValue);
							BCArea.right = nValue;
							pIni->GetInteger(szSection, "AreaBottom", 0, &nValue);
							BCArea.bottom = nValue;
							m_pBCImg[m_nBCNum].nType = ISI_T_SPR;
							m_pBCImg[m_nBCNum].Color.Color_b.a = 255;
							m_pBCImg[m_nBCNum].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
							m_pBCImg[m_nBCNum].uImage = 0;
							m_pBCImg[m_nBCNum].nISPosition = IMAGE_IS_POSITION_INIT;
							m_pBCImg[m_nBCNum].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
							t_sprintf(szKeyName, "Image%d", m_nBCNum);
							pIni->GetString(szSection, szKeyName, "", m_pBCImg[m_nBCNum].szImage, sizeof(m_pBCImg[m_nBCNum].szImage));
							m_pBCImg[m_nBCNum].oPosition.nX = ::GetRandomNumber(0, RESOLUTION_WIDTH_800);
							m_pBCImg[m_nBCNum].oPosition.nY = ::GetRandomNumber(0, RESOLUTION_HEIGHT_800);
							m_pBCImg[m_nBCNum].oPosition.nZ = 0;
							m_pBCImg[m_nBCNum].nFrame = 0;
							m_nBCNum++;
						}
						break;
					}
				}
			}
			m_bBGImg = m_nBGNum > 0;
			m_bOCImg = m_nOCNum > 0;
			m_bBCImg = m_nBCNum > 0;					
		}
	}

	*/
}


BOOL KScenePlaceC::PaintBackGround()
{
	BOOL bRet = TRUE;
	/*if (m_bBGImg)
	{
		for (int i = 0; i < m_nBGNum; i++)
		{
			if ((m_FocusRegion.x > BGArea[i].left || BGArea[i].left == 0) && 
				(m_FocusRegion.y < BGArea[i].bottom || BGArea[i].bottom == 0) && 
				(m_FocusRegion.x < BGArea[i].right || BGArea[i].right == 0) && 
				(m_FocusRegion.y > BGArea[i].top || BGArea[i].top == 0))
			{
				g_pRepresent->DrawPrimitives(1, &m_pBGImg[i], RU_T_IMAGE, true);
				bRet = FALSE;
			}
		}
	}
	else
	{
		
		if (g_pRepresent->GetRepMoDel()==3)
			return bRet;

		KUiImageRef nImgBg;	
		nImgBg.nType = ISI_T_BITMAP16;
		nImgBg.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		nImgBg.uImage = 0;
		nImgBg.nISPosition = IMAGE_IS_POSITION_INIT;
		nImgBg.bRenderFlag = 0;
		t_sprintf(nImgBg.szImage,"\\游戏资源\\background\\背景图_黑.jpg");
		KImageParam	Param;
		if (g_pRepresent)
		{
		  if (g_pRepresent->GetImageParam(nImgBg.szImage, &Param, ISI_T_BITMAP16))
		  {
			nImgBg.nNumFrames = Param.nNumFrames;
			nImgBg.nInterval  = Param.nInterval;		//帧间隔
			nImgBg.nDirs      = Param.nNumFramesGroup; //总方向数
		  }
		}
		nImgBg.oPosition.nX=m_FocusRegion.x-Param.nWidth/2+150;
		nImgBg.oPosition.nY=m_FocusRegion.y-Param.nHeight/2;
		nImgBg.oPosition.nZ = 0;
		nImgBg.nFrame = -1;
		nImgBg.nNumFrames = 0;
		g_pRepresent->DrawPrimitives(1, &nImgBg, RU_T_IMAGE, true);
		bRet = FALSE;
	}*/
	return bRet;
}

void KScenePlaceC::PaintOverCloud()
{
	if (m_bOCImg && 
		(m_FocusRegion.x > OCArea.left || OCArea.left == 0) && 
		(m_FocusRegion.y < OCArea.bottom || OCArea.bottom == 0) && 
		(m_FocusRegion.x < OCArea.right || OCArea.right == 0) && 
		(m_FocusRegion.y > OCArea.top || OCArea.top == 0))
	{
		for (int i = 0; i < m_nOCNum; i++)
		{
			/*KImageParam Param;
			if (g_pRepresent->GetImageParam(m_pOCImg[i].szImage, &Param, ISI_T_SPR) == true)
			{
				m_pOCImg[i].oPosition.nX--;		
				if (m_pOCImg[i].oPosition.nX < -Param.nWidth)
					m_pOCImg[i].oPosition.nX = RESOLUTION_WIDTH_800;
				if (m_pOCImg[i].oPosition.nX > RESOLUTION_WIDTH_800)
					m_pOCImg[i].oPosition.nX = -Param.nWidth;
			}*/
			//g_pRepresent->DrawPrimitives(1, &m_pOCImg[i], RU_T_IMAGE, true);
		}
	}
}

void KScenePlaceC::PaintBackGroundCloud()
{
	if (m_bBCImg && 
		(m_FocusRegion.x > BCArea.left || BCArea.left == 0) && 
		(m_FocusRegion.y < BCArea.bottom || BCArea.bottom == 0) && 
		(m_FocusRegion.x < BCArea.right || BCArea.right == 0) && 
		(m_FocusRegion.y > BCArea.top || BCArea.top == 0))
	{
		for (int i = 0; i < m_nBCNum; i++)
		{
			//KImageParam Param;
			/*if (g_pRepresent->GetImageParam(m_pBCImg[i].szImage, &Param, ISI_T_SPR) == true)
			{
				m_pBCImg[i].oPosition.nX--;
				if (m_pBCImg[i].oPosition.nX < -Param.nWidth)
					m_pBCImg[i].oPosition.nX = RESOLUTION_WIDTH_800;
				if (m_pBCImg[i].oPosition.nX > RESOLUTION_WIDTH_800)
					m_pBCImg[i].oPosition.nX = -Param.nWidth;
			}*/
			//g_pRepresent->DrawPrimitives(1, &m_pBCImg[i], RU_T_IMAGE, true);
		}
	}
}


void KScenePlaceC::EnableWeather(int nbEnable)
{
	if (!m_bEnableWeather != !nbEnable)
	{
		if (nbEnable == false)
			ChangeWeather(WEATHERID_NOTHING);
		m_bEnableWeather = (nbEnable != 0);
	}
}

void KScenePlaceC::SetLoadingStatus(bool bLoading)
{
	if ((!m_bLoading) != (!bLoading))
	{
		m_bLoading = bLoading;
		//CoreDataChanged(GDCNI_SWITCHING_SCENEPLACE, 0, m_bLoading);
#ifdef WIN32
		if (m_bLoading == false)
			SetEvent(m_hSwitchLoadFinishedEvent);
#else
		if (m_bLoading == false)
			SetEvent(m_hSwitchLoadFinishedEvent);
#endif
	}
}

void KScenePlaceC::PaintMap(int nX, int nY)
{
	m_Map.PaintMap(nX, nY);
	//绘画障碍小地图
	//m_LittleMap.Draw(nX,nY);			// 绘制

}

void KScenePlaceC::SetMapParam(unsigned int uShowElems, int nSize)
{
	m_Map.SetShowElemsFlag(uShowElems);
	if (uShowElems)
		m_Map.SetSize((nSize & 0xffff), (nSize >> 16));
}

//设置场景的地图的焦点(单位:场景坐标)
void KScenePlaceC::SetMapFocusPositionOffset(int nOffsetX, int nOffsetY)
{
	if (m_bFollowWithMap == false)
	{
		m_MapFocusOffset.x = nOffsetX;
		m_MapFocusOffset.y = nOffsetY;
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			m_FocusPosition.y + m_MapFocusOffset.y, true);
	}
	else
	{
		m_bFollowWithMap = false;
		SetFocusPosition(m_OrigFocusPosition.x + nOffsetX, m_OrigFocusPosition.y + nOffsetY , 0);
		m_bFollowWithMap = true;
	}
}

//获取场景的小地图信息
int KScenePlaceC::GetMapInfo(KSceneMapInfo* pInfo)
{
	RECT	MapRc;
	int nRet = m_Map.GetMapRect(&MapRc);
	if (pInfo)
	{
		int nHalfShowWidth  = m_RepresentExactHalfSize.cx * 2; //- SPWP_REPRESENT_RECT_WINDAGE_X
		int nHalfShowHeight = m_RepresentExactHalfSize.cy * 2; //- SPWP_REPRESENT_RECT_WINDAGE_T
		pInfo->nFocusMinH = MapRc.left + nHalfShowWidth;
		pInfo->nFocusMinV = MapRc.top  + nHalfShowHeight;
		pInfo->nFocusMaxH = MapRc.right  - nHalfShowWidth;
		pInfo->nFocusMaxV = MapRc.bottom - nHalfShowHeight;
		if (pInfo->nFocusMaxH < pInfo->nFocusMinH)
			pInfo->nFocusMaxH = pInfo->nFocusMinH;
		if (pInfo->nFocusMaxV < pInfo->nFocusMinV)
			pInfo->nFocusMaxV = pInfo->nFocusMinV;

		if (m_bFollowWithMap == false)
		{
			pInfo->nOrigFocusH = m_FocusPosition.x;
			pInfo->nOrigFocusV = m_FocusPosition.y;
			pInfo->nFocusOffsetH = m_MapFocusOffset.x;
			pInfo->nFocusOffsetV = m_MapFocusOffset.y;
		}
		else
		{
			pInfo->nOrigFocusH = m_OrigFocusPosition.x;
			pInfo->nOrigFocusV = m_OrigFocusPosition.y;
			pInfo->nFocusOffsetH = m_FocusPosition.x - m_OrigFocusPosition.x;
			pInfo->nFocusOffsetV = m_FocusPosition.y - m_OrigFocusPosition.y;
		}
		pInfo->nScallH = KScenePlaceMapC::MAP_SCALE_H;
		pInfo->nScallV = KScenePlaceMapC::MAP_SCALE_V;
	}
	return nRet;
}

//设置是否跟随地图的移动而移动
void  KScenePlaceC::FollowMapMove(int nbEnable)
{
	if ((!m_bFollowWithMap) != (!nbEnable))
	{
		if (m_bFollowWithMap = (nbEnable != 0))
		{
			m_OrigFocusPosition.x = m_FocusPosition.x;
			m_OrigFocusPosition.y = m_FocusPosition.y;
			int	x = m_MapFocusOffset.x;
			int y = m_MapFocusOffset.y;
			m_MapFocusOffset.x = 0;
			m_MapFocusOffset.y = 0;
			SetMapFocusPositionOffset(x, y);
		}
		else
		{
			m_MapFocusOffset.x = m_FocusPosition.x - m_OrigFocusPosition.x;
			m_MapFocusOffset.y = m_FocusPosition.y - m_OrigFocusPosition.y;
			SetFocusPosition(m_OrigFocusPosition.x, m_OrigFocusPosition.y, 0);
		}
	}
}
void KScenePlaceC::DrawGreenLine(int nX, int nY, BOOL bSearch) // 画黄线
{ 
	m_Map.SetGreenLine(nX, nY, bSearch);
}

void KScenePlaceC::FindPos(int nX, int nY, BOOL bSearch)
{ 
	m_Map.SetGreenLine(nX, nY, bSearch);
}

//删除画黄线
void KScenePlaceC::DelGreenLine()
{ 
	m_Map.DelGreenLine();
}

void KScenePlaceC::GetMapSizePosInfo(int uParam)
{
    m_Map.GetMapSizePos((KSceneMapSizePos*)uParam);
}
