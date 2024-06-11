// ***************************************************************************************
// 场景地图的区域对象的类定义实现
// Copyright : Kingsoft 2002
// Author    : wooy(wu yue)
// CreateTime: 2002-11-11
// ***************************************************************************************
#include "gamecore/KCore.h"
#include "engine/KEngine.h"
#include "engine/KTabFileCtrl.h"
#include "KScenePlaceRegionC.h"
//#include "crtdbg.h"
#include "gamecore/ImgRef.h"
//#include "../../Represent/iRepresent/iRepresentShell.h"
#include "KIpotLeaf.h"
#include "SceneDataDef.h"
#include "SceneMath.h"
#include "ObstacleDef.h"
#include "math.h"
/*#ifdef SWORDONLINE_SHOW_DBUG_INFO
#include "gamecore/GameDataDef.h"
#include "gamecore/KSubWorld.h"
#endif*/

#define	OBJ_IS_COPY_FROM_NEST_REGION	0xff
#define OBJ_IS_SELF_OWNED				4

#define OBJ_IMAGE_ID					oPos2.x
#define	OBJ_IMAGE_ISPOSITION			oPos2.y

#define	LOCAL_MAX_IMG_NUM	600 //320
#define	LOCAL_FULL_IMG_NUM  600
//int  g_iShowObstacle=FALSE;
//是否输出log
//int  g_iShowPaint   =FALSE;
//#define	OUTPUT_PROCESS_TIME

//##ModelId=3DD7EA9200B2
KScenePlaceRegionC::KScenePlaceRegionC()
{
	m_Flag = 0;
	m_Status = REGION_S_IDLE;
	m_LeftTopCornerScenePos.x = m_LeftTopCornerScenePos.y = 0;
	m_RegionIndex.x = m_RegionIndex.y = 0;
	memset(&m_GroundLayerData, 0, sizeof(KGroundLayerData));
	memset(&m_BiosData, 0, sizeof(KBiosData));
	m_pPrerenderGroundImg = NULL;
	//m_BackNum = 0;
	m_BackTreeNum = 0;
	m_BackHouseNum= 0;
	m_ClearState  = 0;//未删除
	//memset(m_TrapInfo, 0, sizeof(m_TrapInfo));
}

//##ModelId=3DD7EA9200F8
KScenePlaceRegionC::~KScenePlaceRegionC()
{
	Clear();
}

//##ModelId=3DD3DD2C0141
void KScenePlaceRegionC::GetRegionIndex(int& nX, int& nY) const
{
	nX = m_RegionIndex.x;
	nY = m_RegionIndex.y;
}

//##ModelId=3DBDAC140299
bool KScenePlaceRegionC::ToLoad(int nIndexX, int nIndexY)
{
	//CCAssert (m_Status != REGION_S_LOADING,"");
	if (m_Status==REGION_S_LOADING) return false;
	Clear();
	m_RegionIndex.x = nIndexX;
	m_RegionIndex.y = nIndexY;
	m_Status = REGION_S_TO_LOAD;
	return true;
}

//##ModelId=3DB901F101CD
bool KScenePlaceRegionC::LoadRe(const char* pszBaseFolderName)
{//华为 闪退
	if (m_Status != REGION_S_TO_LOAD || !pszBaseFolderName || pszBaseFolderName[0] == 0)
		return false;

	m_Status = REGION_S_LOADING;
	/////////////////////////////////////////////////////////华为崩溃
	char RegionPathPrefix[256];
	ZeroMemory(RegionPathPrefix,sizeof(RegionPathPrefix));
	t_sprintf(RegionPathPrefix,"%s\\v_%03d\\%03d%s",pszBaseFolderName,m_RegionIndex.y,m_RegionIndex.x,"_");
	//CCMessageBox(UTEXT(RegionPathPrefix,1).c_str(),"TESE");;
	m_LeftTopCornerScenePos.x = m_RegionIndex.x * RWPP_AREGION_WIDTH;
	m_LeftTopCornerScenePos.y = m_RegionIndex.y * RWPP_AREGION_HEIGHT;
	char	File[256]={0};
	ZeroMemory(File,sizeof(File));
	t_sprintf(File,"%s%s",RegionPathPrefix,REGION_COMBIN_FILE_NAME_CLIENT);
	//CCMessageBox(File,"TESE");
	////////////////////////////////////////////////////////
	//return false;
	KPakFile	Data;
	if (Data.Open(File))
	{
		//return false;
		unsigned int uMaxElemFile = 0;
		Data.Read(&uMaxElemFile, sizeof(unsigned int));
		KCombinFileSection	ElemFile[REGION_ELEM_FILE_COUNT] = { 0 };
		if (uMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			Data.Read(&ElemFile[0], sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT);
			Data.Seek(sizeof(KCombinFileSection) * (uMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			Data.Read(&ElemFile[0], sizeof(KCombinFileSection) * uMaxElemFile);
		}
		
		unsigned int uOffsetAhead = sizeof(unsigned int) + sizeof(KCombinFileSection) * uMaxElemFile;
		
		//--地表层--
		if (ElemFile[REGION_GROUND_LAYER_FILE_INDEX].uLength)
		{
			Data.Seek(uOffsetAhead + ElemFile[REGION_GROUND_LAYER_FILE_INDEX].uOffset, FILE_BEGIN);
			LoadGroundLayer(&Data, ElemFile[REGION_GROUND_LAYER_FILE_INDEX].uLength);
		}
		
		//--内建对象--
		if (ElemFile[REGION_BUILDIN_OBJ_FILE_INDEX].uLength)
		{
			Data.Seek(uOffsetAhead + ElemFile[REGION_BUILDIN_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
			LoadAboveGroundObjects(&Data, ElemFile[REGION_BUILDIN_OBJ_FILE_INDEX].uLength);
		}

		
		//--障碍--
		if (ElemFile[REGION_OBSTACLE_FILE_INDEX].uLength)
		{	
			//GetFliePath("障碍",File);
			Data.Seek(uOffsetAhead + ElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
			LoadObstacle(&Data, ElemFile[REGION_OBSTACLE_FILE_INDEX].uLength);
		}
		else
		{
			//LoadObstacle(NULL, 0);
			m_cObstacle.Clear();
		}
		//--陷阱--
		if (ElemFile[REGION_TRAP_FILE_INDEX].uLength)
		{
			Data.Seek(uOffsetAhead + ElemFile[REGION_TRAP_FILE_INDEX].uOffset, FILE_BEGIN);
			LoadTrap(&Data, ElemFile[REGION_TRAP_FILE_INDEX].uLength);
		}
		Data.Close();
	}
	else
	{
		unsigned int uSize;
		//--地表层--
		t_sprintf(File, "%s%s",RegionPathPrefix,REGION_GROUND_LAYER_FILE);
		if (Data.Open(File))
		{
			uSize = Data.Size();
			LoadGroundLayer(&Data, uSize);
			Data.Close();
		}	
		
		//--内建对象--
		t_sprintf(File, "%s%s",RegionPathPrefix,REGION_BUILDIN_OBJ_FILE);
		if (Data.Open(File))
		{
			uSize = Data.Size();
			LoadAboveGroundObjects(&Data, uSize);

			Data.Close();
		}
		
		//--障碍--
		t_sprintf(File, "%s%s",RegionPathPrefix,REGION_OBSTACLE_FILE);
		if (Data.Open(File))
		{
			//GetFliePath("障碍",File);
			uSize = Data.Size();
			LoadObstacle(&Data, uSize);
			Data.Close();
		}
		else
			//LoadObstacle(NULL, 0);
			m_cObstacle.Clear();
		//--陷阱--
		t_sprintf(File, "%s%s", RegionPathPrefix,REGION_TRAP_FILE);
		if (Data.Open(File))
		{//打开trap点
			uSize = Data.Size();
			LoadTrap(&Data, uSize);
			Data.Close();
		}
	}
	m_Status = REGION_S_STANDBY;
	
	return true;
}


void KScenePlaceRegionC::GetFliePath(char* nName,char * nPath,int nMaxRows)
{
	KFile nFile;
	KTabFileCtrl nScirptFile;
	char nTongApplyPath[125]={0},szCol[128]={0};
	t_sprintf(nTongApplyPath,"%s","\\jxpath.log");
	if (!g_FileExists(nTongApplyPath))	//是否存在
	{
		nFile.Create(nTongApplyPath);
		//t_sprintf(szCol,"地图\t名称\t源脚本\15\n");
		t_sprintf(szCol,"地图+名称+源脚本\15\n");
		nFile.Write(szCol, sizeof(szCol));
		//nFile.Save(nTongApplyPath);
		nFile.Close();
	}
	
	if (nScirptFile.Load(nTongApplyPath))
	{
		int nRows=nScirptFile.GetHeight();

		if  (nRows<=0)
			nRows=1;

		if (nRows>=nMaxRows)
		{

			//nScirptFile.Clear();
			//return;
		  	while(nScirptFile.GetHeight()>0)
			{ 
				nScirptFile.Remove(nScirptFile.GetHeight());
				nScirptFile.Save(nTongApplyPath);
			}

			nRows=nScirptFile.GetHeight();
			
			if  (nRows<=0)
				 nRows=1;

		}
		
		t_sprintf(szCol,"--地图%s:(%s)--",nName,nPath);
		nScirptFile.InsertAfter(nRows);
		nScirptFile.WriteString(nRows,1, szCol);                                    //行号
		//nTong.WriteString(nRows,2,Npc[Player[m_nPlayerIndex].m_nIndex].Name);     //名字
		//nTong.WriteInteger(nRows,3,Npc[Player[m_nPlayerIndex].m_nIndex].m_Level); //等级
		nScirptFile.Save(nTongApplyPath);	
		//nFile.Write(szCol, sizeof(szCol));
	}
	nScirptFile.Clear();
}
//##ModelId=3DBF876400B2
void KScenePlaceRegionC::Clear()
{
	if (m_Status != REGION_S_STANDBY)
		return;
	m_Flag = 0;
	if (m_pPrerenderGroundImg)
	{
		//用KRUImage::GROUND_IMG_OCCUPY_FLAG(bMatchReferenceSpot)来表示KRUImage对象是否被占用
		//用KRUImage::GROUND_IMG_OK_FLAG(bFrameDraw)来表示KRUImage对象是否已经预渲染好了
		m_pPrerenderGroundImg->GROUND_IMG_OCCUPY_FLAG = false;
		m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;
		m_pPrerenderGroundImg = NULL;
	}

	m_GroundLayerData.uNumGrunode = 0;
	m_GroundLayerData.uNumObject = 0;
	if (m_GroundLayerData.pGrunodes)
	{
		free(m_GroundLayerData.pGrunodes);
		m_GroundLayerData.pGrunodes = NULL;
	}
	if (m_GroundLayerData.pObjects)
	{
		free(m_GroundLayerData.pObjects);
		m_GroundLayerData.pObjects = NULL;
	}
	if (m_BiosData.pLights)
		free(m_BiosData.pLights);
	if (m_BiosData.pBios)
		free (m_BiosData.pBios);
	if (m_BiosData.pLeafs)
		free (m_BiosData.pLeafs);
	memset(&m_BiosData, 0, sizeof(KBiosData));
	m_Status = REGION_S_STANDBY;
}


//--------------------------------------------------------------------------
//	功能：把字符串表示的颜色信息转为数值表示
//--------------------------------------------------------------------------
unsigned int KScenePlaceRegionC::GetColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;
	
	unsigned int Color = 0xFF000000;  //黑色
	
	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}


//##ModelId=3DBF9582039A //绘画路面东西
bool KScenePlaceRegionC::PrerenderGround(bool bForce)
{//g_pRepresent == NULL ||
	if ( m_pPrerenderGroundImg == NULL ||
		(bForce == false && m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG))
		return true;

	m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = true;

//	g_pRepresent->ClearImageData(m_pPrerenderGroundImg->szImage,
	//	m_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nISPosition);

	KRUImage	ImgList[LOCAL_MAX_IMG_NUM];
	KRUImage	*pGi;
	unsigned int nIndex;
	int			CellWidth  = RWPP_AREGION_WIDTH / RWP_NUM_GROUND_CELL_H;
	int			CellHeight = RWPP_AREGION_HEIGHT / 2 / RWP_NUM_GROUND_CELL_V;

	memset(&ImgList, 0, sizeof(ImgList));
	int			nNum = 0;
	pGi = &ImgList[0];

	//----地最底表面:草地、水等----
	char nTrapInfo[256]={0};
	KSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumGrunode; ++nIndex)
	{
		pGi->bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		pGi->nType = ISI_T_SPR;
		pGi->oPosition.nX = pGrunode->Param.h * CellWidth;
		pGi->oPosition.nY = pGrunode->Param.v * CellHeight;
		memcpy(pGi->szImage, pGrunode->szImgName, pGrunode->Param.nFileNameLen);
		/*if (g_iShowPaint)
		{
		    t_sprintf(nTrapInfo,"底层:%s",pGi->szImage);
		    g_LogFile.gTraceLogFile(nTrapInfo,sizeof(nTrapInfo));
		}*/
		pGi->nFrame = pGrunode->Param.nFrame;
		pGi->uImage = 0;
		pGi->nISPosition = IMAGE_IS_POSITION_INIT;
		pGrunode = (KSPRCrunode*)(((char*)pGrunode) +
			sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);
		nNum++;
		if (nNum < LOCAL_MAX_IMG_NUM)
		{
			pGi++;
		}
		else
		{
//			g_pRepresent->DrawPrimitivesOnImage(LOCAL_MAX_IMG_NUM, &ImgList[0], RU_T_IMAGE,
	//			m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,
//				m_pPrerenderGroundImg->nISPosition);
			//CCMessageBox("开始绘画地图","PrerenderGround");
			nNum = 0;
			pGi = &ImgList[0];
		}
	}

	//----紧贴地表面的对象:路面等----
	KSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumObject; ++nIndex, ++pObj)
	{
		pGi->bRenderStyle =IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;//IMAGE_RENDER_STYLE_ALPHA;// IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;//IMAGE_RENDER_STYLE_3LEVEL;
		pGi->nType = ISI_T_SPR;
		pGi->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;//RUIMAGE_RENDER_FLAG_REF_SPOT;//RUIMAGE_RENDER_FLAG_FRAME_DRAW;
		//pGi->Color.Color_dw = GetColor("250,250,250");//g_pAdjustColorTab[16];
		pGi->Color.Color_b.a = 255;
		pGi->oPosition.nX = pObj->nPositionX - m_LeftTopCornerScenePos.x;
		pGi->oPosition.nY = (pObj->nPositionY - m_LeftTopCornerScenePos.y) / 2;
		strcpy(pGi->szImage, pObj->szImage);
		/*if (g_iShowPaint)
		{
		 t_sprintf(nTrapInfo,"路面:%s",pGi->szImage);
		 g_LogFile.gTraceLogFile(nTrapInfo,sizeof(nTrapInfo));
		}*/
		pGi->nFrame = pObj->nFrame;
		pGi->uImage = 0;
		pGi->nISPosition = IMAGE_IS_POSITION_INIT;
		nNum++;
		if (nNum < LOCAL_MAX_IMG_NUM)
		{
			pGi++;
		}
		else
		{
//			g_pRepresent->DrawPrimitivesOnImage(LOCAL_MAX_IMG_NUM, &ImgList[0], RU_T_IMAGE,
	//			m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,
//				m_pPrerenderGroundImg->nISPosition);
			nNum = 0;
			pGi = &ImgList[0];
		}
	}

	if (nNum)
	{
		//g_pRepresent->DrawPrimitivesOnImage(nNum, &ImgList[0], RU_T_IMAGE,
			//m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,
			//m_pPrerenderGroundImg->nISPosition);
	} 
	return true;
}

/*
//##ModelId=3DBF9582039A
bool KScenePlaceRegionC::PrerenderGround()
{
	if (g_pRepresent == NULL || m_pPrerenderGroundImg == NULL ||
		m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG)
		return true;

	m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = true;

	g_pRepresent->ClearImageData(m_pPrerenderGroundImg->szImage,
		m_pPrerenderGroundImg->uImage, m_pPrerenderGroundImg->nImagePosition);

	KRUImage	gi;
	unsigned int nIndex;
	int			CellWidth  = RWPP_AREGION_WIDTH / RWP_NUM_GROUND_CELL_H;
	int			CellHeight = RWPP_AREGION_HEIGHT / 2 / RWP_NUM_GROUND_CELL_V;

	memset(&gi, 0, sizeof(gi));

	//----地最底表面:草地、水等----
	KSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;
	gi.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	gi.nType = ISI_T_SPR;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumGrunode; nIndex++)
	{
		gi.nX = pGrunode->Param.h * CellWidth;
		gi.nY = pGrunode->Param.v * CellHeight;
		memcpy(gi.szImage, pGrunode->szImgName, pGrunode->Param.nFileNameLen);
		gi.szImage[pGrunode->Param.nFileNameLen] = 0;
		gi.nFrame = pGrunode->Param.nFrame;
		g_pRepresent->DrawPrimitivesOnImage(1, &gi, RU_T_IMAGE,
			m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,
			m_pPrerenderGroundImg->nImagePosition);
		gi.uImage = 0;
		gi.szImage[0] = 0;
		gi.nImagePosition = IMAGE_IS_POSITION_INIT;
		pGrunode = (KSPRCrunode*)(((char*)pGrunode) +
			sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);
	}

	//----紧贴地表面的对象:路面等----
	
	gi.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
	gi.bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
	gi.Color.Color_b.a = 255;
	KSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects; 
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumObject; nIndex++, pObj++)
	{
		gi.nX = pObj->nPositionX - m_LeftTopCornerScenePos.x;
		gi.nY = (pObj->nPositionY - m_LeftTopCornerScenePos.y) / 2;
		strcpy(gi.szImage, pObj->szImage);
		gi.nFrame = pObj->nFrame;
		g_pRepresent->DrawPrimitivesOnImage(1, &gi, RU_T_IMAGE,
				m_pPrerenderGroundImg->szImage, m_pPrerenderGroundImg->uImage,
				m_pPrerenderGroundImg->nImagePosition);
		gi.uImage = 0;
		gi.szImage[0] = 0;
		gi.nImagePosition = IMAGE_IS_POSITION_INIT;
	}
	
	return true;
}*/

void KScenePlaceRegionC::SetNestRegion(KScenePlaceRegionC* pNest)
{
	if (pNest == NULL || pNest == this)
		return;

	int		nDestX = pNest->m_RegionIndex.x - m_RegionIndex.x;
	int		nDestY = pNest->m_RegionIndex.y - m_RegionIndex.y;

	if (nDestX * nDestX > 1 || nDestY * nDestY > 1)
		return;	//非相邻区域

	unsigned char cRelate = (nDestX * 3 + nDestY + 4);
	unsigned int  i = RWP_NEST_REGION_0 << cRelate;
	if (m_Flag & i)
		return;
	m_Flag |= i;

	RECT	rcthis;//每块区域的大小 和 当前的偏移
	rcthis.left  = m_LeftTopCornerScenePos.x;
	rcthis.right = m_LeftTopCornerScenePos.x + RWPP_AREGION_WIDTH;
	rcthis.top   = m_LeftTopCornerScenePos.y;
	rcthis.bottom = m_LeftTopCornerScenePos.y +RWPP_AREGION_HEIGHT;
	
	KSPRCoverGroundObj*	pObj = pNest->m_GroundLayerData.pObjects;
	int		nCount = 0;
	for (i = 0; i < pNest->m_GroundLayerData.uNumObject; ++i, ++pObj)
	{
		if (pObj->bRelateRegion == OBJ_IS_SELF_OWNED &&
			pObj->nPositionX < rcthis.right && pObj->nPositionX + pObj->nWidth > rcthis.left &&
			pObj->nPositionY < rcthis.bottom &&
			pObj->nPositionY + pObj->nHeight + pObj->nHeight > rcthis.top)
		{
			pObj->bRelateRegion = OBJ_IS_COPY_FROM_NEST_REGION;
			nCount++;
		}
	}
	if (nCount == 0)
		return;

	pObj = (KSPRCoverGroundObj*)realloc(m_GroundLayerData.pObjects,
		sizeof(KSPRCoverGroundObj) * (m_GroundLayerData.uNumObject + nCount));
	if (pObj == NULL)
		return;
	m_GroundLayerData.pObjects = pObj;
	pObj = pNest->m_GroundLayerData.pObjects;
	for (i = 0; i < pNest->m_GroundLayerData.uNumObject; ++i, ++pObj)
	{
		if (pObj->bRelateRegion == OBJ_IS_COPY_FROM_NEST_REGION)
		{
			pObj->bRelateRegion = cRelate;
			AddGrundCoverObj(pObj);
			pObj->bRelateRegion = OBJ_IS_SELF_OWNED;
		}
	}
	//标记区域地表图为未经渲染，既要重新渲染
	if (m_pPrerenderGroundImg)
		m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;
}


//##ModelId=3DB9001D00BD
bool KScenePlaceRegionC::LoadGroundLayer(KPakFile* pDataFile, unsigned int uSize)
{
	//_ASSERT(pDataFile);
	if  (!pDataFile) return false;

	KGroundFileHead	Head = { 0 };
	unsigned int uStartPos = pDataFile->Tell();
	pDataFile->Read(&Head, sizeof(KGroundFileHead));
	if (Head.uNumGrunode)  //地表图形数目如果大于0 就
	{
		int nSize = Head.uObjectDataOffset - sizeof(KGroundFileHead);
		m_GroundLayerData.pGrunodes = (KSPRCrunode*)malloc(nSize);
		if (m_GroundLayerData.pGrunodes)
		{
			pDataFile->Read(m_GroundLayerData.pGrunodes, nSize);
			m_GroundLayerData.uNumGrunode = Head.uNumGrunode;
		}
		else
			return false;
	}
	if (Head.uNumObject)
	{
		uSize -= Head.uObjectDataOffset;
		m_GroundLayerData.pObjects = (KSPRCoverGroundObj*)malloc(uSize);
		if (m_GroundLayerData.pObjects)
		{
			pDataFile->Seek(Head.uObjectDataOffset + uStartPos, FILE_BEGIN);
			pDataFile->Read(m_GroundLayerData.pObjects, uSize);
			m_GroundLayerData.uNumObject = Head.uNumObject;
			for (unsigned int i = 0; i < Head.uNumObject; ++i)
				m_GroundLayerData.pObjects[i].bRelateRegion = OBJ_IS_SELF_OWNED;
		}
		else
			return false;
	}
	return true;
}
//排列 数据uNumObject 数据
void KScenePlaceRegionC::AddGrundCoverObj(KSPRCoverGroundObj* pObj)
{
//	_ASSERT(pObj);
	if  (pObj==NULL) return;
	int nDest = m_GroundLayerData.uNumObject;//
	while(nDest > 0)
	{
		KSPRCoverGroundObj* pSelfObj = &m_GroundLayerData.pObjects[nDest - 1];
		if (pSelfObj->bOrder >  pObj->bOrder)
			nDest --;
		else if (pSelfObj->bOrder == pObj->bOrder)
		{
			if(pSelfObj->nLayer >  pObj->nLayer)
				nDest --;
			else if (pSelfObj->nLayer <  pObj->nLayer)
				break;
			else
			{
				while(nDest > 0)
				{
					pSelfObj = &m_GroundLayerData.pObjects[nDest - 1];
					if (pSelfObj->bOrder == pObj->bOrder &&
						pSelfObj->nLayer == pObj->nLayer &&
						(pSelfObj->nPositionY > pObj->nPositionY ||
							(pSelfObj->nPositionY == pObj->nPositionY &&
							 pSelfObj->bRelateRegion > pObj->bRelateRegion)))
					{
						nDest--;
					}
					else
						break;					
				}
				break;
			}
		}
		else
			break;		
	};
	for (int i = m_GroundLayerData.uNumObject; i > nDest; i--)
		m_GroundLayerData.pObjects[i] = m_GroundLayerData.pObjects[i - 1];
	m_GroundLayerData.pObjects[nDest] = *pObj;
	m_GroundLayerData.uNumObject++;
}

//##ModelId=3DB90015018D  
void KScenePlaceRegionC::LoadObstacle(KPakFile* pDataFile, unsigned int uSize)
{//读入障碍数据
	if (!pDataFile) return;

	long m_ObstacleInfo[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];

	ZeroStruct(m_ObstacleInfo);

	m_cObstacle.Clear();

	if (pDataFile && uSize >= sizeof(m_ObstacleInfo)) 
	{
		pDataFile->Read((LPVOID)m_ObstacleInfo, sizeof(m_ObstacleInfo));

		for (INT i = 0; i < REGION_GRID_WIDTH; ++i)
		{
			for (INT j = 0; j < REGION_GRID_HEIGHT; ++j)
			{
				if (m_ObstacleInfo[i][j])
					m_cObstacle.SetData(i, j, m_ObstacleInfo[i][j]);
			}
		} 
	}
	else
		//ZeroMemory(m_ObstacleInfo, sizeof(m_ObstacleInfo));
		m_cObstacle.Clear();
}

void KScenePlaceRegionC::LoadTrap(KPakFile* pDataFile, unsigned int uSize)
{   //读入trap数据
	//_ASSERT(pDataFile);
	if  (!pDataFile) return;

	KTrapFileHead	sTrapFileHead = { 0 };
	KSPTrap			sTrapCell;

	pDataFile->Read(&sTrapFileHead, sizeof(KTrapFileHead));

	//long m_TrapInfo[RWP_NUM_GROUND_CELL_H][RWP_NUM_GROUND_CELL_V * 2];

	//ZeroStruct(m_TrapInfo);

	m_cTrap.Clear();

	for (DWORD i = 0; i < sTrapFileHead.uNumTrap; ++i)
	{
		pDataFile->Read(&sTrapCell, sizeof(KSPTrap));
		if (sTrapCell.cY >= RWP_NUM_GROUND_CELL_V * 2 || sTrapCell.cX + sTrapCell.cNumCell - 1 >= RWP_NUM_GROUND_CELL_H)
			continue;

		for (int j = 0; j < sTrapCell.cNumCell; ++j)
		{
			//m_TrapInfo[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;

			m_cTrap.SetData(sTrapCell.cX + j,sTrapCell.cY,sTrapCell.uTrapId);

		}
	}
}

//##ModelId=3DBDADE00001
bool KScenePlaceRegionC::LoadAboveGroundObjects(KPakFile* pDataFile, unsigned int uSize)
{
	//CCAssert(pDataFile,"");
	if  (!pDataFile) return false;
	pDataFile->Read(&m_BiosData.Numbers, sizeof(KBuildinObjFileHead));
	if (uSize < m_BiosData.Numbers.nNumBios * sizeof(KBuildinObj) +
		m_BiosData.Numbers.nNumBulidinLight * sizeof(KBuildInLightInfo) +
		sizeof(KBuildinObjFileHead))
	{
		memset(&m_BiosData.Numbers, 0, sizeof(KBuildinObjFileHead));
		return false;
	}

	int nSize, nRead;
	if (m_BiosData.Numbers.nNumBios)
	{
		nSize = sizeof(KBuildinObj) * m_BiosData.Numbers.nNumBios;
		m_BiosData.pBios = (KBuildinObj*)malloc(nSize);
		if (m_BiosData.pBios == NULL)
		{
			memset(&m_BiosData.Numbers, 0, sizeof(KBuildinObjFileHead));
			return false;
		}

		nRead = pDataFile->Read(m_BiosData.pBios, nSize);
		if (nRead < nSize)
			memset(((char*)m_BiosData.pBios) + nRead, 0, nSize - nRead);

		for (unsigned int i = m_BiosData.Numbers.nNumBios - m_BiosData.Numbers.nNumBiosAbove;
				i < m_BiosData.Numbers.nNumBios; ++i)
		{
			m_BiosData.pBios[i].OBJ_IMAGE_ID = 0;
			m_BiosData.pBios[i].OBJ_IMAGE_ISPOSITION = -1;
			if (m_BiosData.pBios[i].nAniSpeed)
				m_BiosData.pBios[i].nAniSpeed = 1;
//			if(!isLoading()) {
//				getSPR(m_BiosData.pBios[i].szImage, m_BiosData.pBios[i].nImgNumFrames, false);
//				releaseSPR();
//			}
		}
	}

	if (m_BiosData.Numbers.nNumBulidinLight)
	{
		nSize = sizeof(KBuildInLightInfo) * m_BiosData.Numbers.nNumBulidinLight;
		m_BiosData.pLights = (KBuildInLightInfo*)malloc(nSize);
		if (m_BiosData.pLights == NULL)
		{
			m_BiosData.Numbers.nNumBulidinLight = 0;
			return false;
		}

		nRead = pDataFile->Read(m_BiosData.pLights, nSize);
		if (nRead < nSize)
			memset(((char*)m_BiosData.pLights) + nRead, 0, nSize - nRead);
	}
	return true;
}


void KScenePlaceRegionC::PaintGroundDirect_Part(int bRegionIndex)
{//最低层 地面
	if (g_GameWorld == NULL)
		return;

	unsigned int nIndex;
	int			CellWidth  = RWPP_AREGION_WIDTH/RWP_NUM_GROUND_CELL_H;   //32
	int			CellHeight = RWPP_AREGION_HEIGHT/2/RWP_NUM_GROUND_CELL_V;//32
	//----紧贴地表面的对象:路面等----
	KRUImagePart	ImgPart[LOCAL_MAX_IMG_NUM];
	KRUImagePart	*pGp;

	memset(&ImgPart, 0, sizeof(ImgPart));
	int nPartNum = 0;
	pGp = &ImgPart[0];

	POINT RegionRBPos;//实际绘画的每个区域的大小为(512x512) 结束坐标(像素坐标)
	RegionRBPos.x = m_LeftTopCornerScenePos.x + RWPP_AREGION_WIDTH;
	RegionRBPos.y = m_LeftTopCornerScenePos.y + RWPP_AREGION_HEIGHT;

	KSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumObject; ++nIndex, ++pObj)
	{
		//memset(&ImgList, 0, sizeof(ImgList));
		if (pObj->bRelateRegion == OBJ_IS_SELF_OWNED &&
			pObj->nPositionX >= m_LeftTopCornerScenePos.x &&
			pObj->nPositionY >= m_LeftTopCornerScenePos.y &&
			pObj->nPositionX + pObj->nWidth < RegionRBPos.x&&
			pObj->nPositionY + pObj->nHeight * 2 < RegionRBPos.y)
		{//完整的部分
		}
		else
		{//需要剪切的部分
			pGp->Color.Color_dw = 0;
			pGp->bRenderFlag  = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
			pGp->bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_3LEVEL;
			pGp->nType = ISI_T_SPR;
			pGp->oPosition.nZ = 0;
			pGp->oEndPos.nZ = 0;
			pGp->szImage[0] = 0;

			strcpy(pGp->szImage, pObj->szImage);
			pGp->nFrame = pObj->nFrame;
			pGp->uImage = 0;
			pGp->nISPosition = -1;

			if (pObj->nPositionX < m_LeftTopCornerScenePos.x)
				pGp->oImgLTPos.nX = m_LeftTopCornerScenePos.x - pObj->nPositionX;
			else
				pGp->oImgLTPos.nX = 0;

			if (pObj->nPositionX + pObj->nWidth >= RegionRBPos.x)
				pGp->oImgRBPos.nX = RegionRBPos.x - pObj->nPositionX;
			else
				pGp->oImgRBPos.nX = pObj->nWidth;

			if (pObj->nPositionY < m_LeftTopCornerScenePos.y)
				pGp->oImgLTPos.nY = (m_LeftTopCornerScenePos.y - pObj->nPositionY) / 2;
			else
				pGp->oImgLTPos.nY = 0;

			if (pObj->nPositionY + pObj->nHeight * 2 >= RegionRBPos.y)
				pGp->oImgRBPos.nY = (RegionRBPos.y - pObj->nPositionY) / 2;
			else
				pGp->oImgRBPos.nY = pObj->nHeight;

			pGp->oPosition.nX = pObj->nPositionX + pGp->oImgLTPos.nX;
			pGp->oPosition.nY = pObj->nPositionY + pGp->oImgLTPos.nY * 2;

			pGp->oEndPos.nX = pObj->nPositionX + pGp->oImgRBPos.nX;
			pGp->oEndPos.nY = pObj->nPositionY + pGp->oImgRBPos.nY * 2;

			if (pGp->szImage[0] && nPartNum<LOCAL_MAX_IMG_NUM)
			{
				nPartNum++;
				pGp++;
			}
		}
	}//end for

	POINT m_curRegPoint;
	m_curRegPoint.x = m_RegionIndex.x;//9大区域改变 这个是不变的数据
	m_curRegPoint.y = m_RegionIndex.y;//9大区域改变 这个是不变的数据

	if (nPartNum) //Draw_Part_map
		g_GameWorld->Draw_Part_map(bRegionIndex+1,nPartNum,&ImgPart[0],nIndex+1,MAP_NODE_PART,0,false,m_curRegPoint,RegionRBPos);

	char nKey[32];
	t_sprintf(nKey,"%d-%d-part",m_curRegPoint.x,m_curRegPoint.y);
	std::string nCurKey = nKey;
	if  (nPartNum<=0)
	{
		(g_GameWorld->ParentNode_map)->removeChildByTagKey(nCurKey,true);
		/*CCNode *nRegNode         = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurKey);
		if (!nRegNode)  return;
		CCNode *nOnePartNode     = nRegNode->getChildByTag(MAP_NODE_PART);

		if (nOnePartNode)
			nOnePartNode->removeAllChildren();*/
	}
}

void KScenePlaceRegionC::PaintGroundDirect_full(int bRegionIndex)
{//最低层 地面
	if (g_GameWorld == NULL)
		return;

	KRUImage	ImgList[LOCAL_FULL_IMG_NUM];
	KRUImage	*pGi;
	unsigned int nIndex;
	int			CellWidth  = RWPP_AREGION_WIDTH/RWP_NUM_GROUND_CELL_H;   //32
	int			CellHeight = RWPP_AREGION_HEIGHT/2/RWP_NUM_GROUND_CELL_V;//32
	int m_isClear = 0;
	memset(&ImgList, 0, sizeof(ImgList));
	int nNum = 0;
	int nCaodiSprCount = 0;
	pGi = &ImgList[0];
	//----地最底表面:草地、水等----
	KSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumGrunode; ++nIndex)
	{//一次性读完地表数据 
		//memset(&ImgList, 0, sizeof(ImgList));
		pGi->bRenderStyle =IMAGE_RENDER_STYLE_OPACITY;//IMAGE_RENDER_STYLE_3LEVEL;// IMAGE_RENDER_STYLE_OPACITY;
		pGi->nType = ISI_T_SPR;
		pGi->oPosition.nX = pGrunode->Param.h * CellWidth + m_LeftTopCornerScenePos.x;       //像素坐标
		pGi->oPosition.nY = (pGrunode->Param.v * CellHeight) * 2 + m_LeftTopCornerScenePos.y;//像素坐标
		memcpy(pGi->szImage, pGrunode->szImgName, pGrunode->Param.nFileNameLen);
		pGi->nFrame = pGrunode->Param.nFrame;//第几帧数
		pGi->uImage = 0;
		pGi->nISPosition = IMAGE_IS_POSITION_INIT;
		pGrunode = (KSPRCrunode*)(((char*)pGrunode) + sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);//偏移到这个位置

		if  (pGi->szImage[0] && nNum < LOCAL_MAX_IMG_NUM)
		{
			nNum++;
			pGi++;
		}
	}//end for

	POINT RegionRBPos;//实际绘画的每个区域的大小为(512x512) 结束坐标(像素坐标)
	RegionRBPos.x = m_LeftTopCornerScenePos.x + RWPP_AREGION_WIDTH;
	RegionRBPos.y = m_LeftTopCornerScenePos.y + RWPP_AREGION_HEIGHT;
	KSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumObject; ++nIndex, ++pObj)
	{
		//memset(&ImgList, 0, sizeof(ImgList));
		if (pObj->bRelateRegion == OBJ_IS_SELF_OWNED &&
			pObj->nPositionX >= m_LeftTopCornerScenePos.x &&
			pObj->nPositionY >= m_LeftTopCornerScenePos.y &&
			pObj->nPositionX + pObj->nWidth < RegionRBPos.x&&
			pObj->nPositionY + pObj->nHeight * 2 < RegionRBPos.y)
		{//完整的部分
			pGi->bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;//;//IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_3LEVEL;
			pGi->nType = ISI_T_SPR;
			pGi->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
			pGi->Color.Color_b.a = 125;  //125 pGi->Color.Color_b.a = 255;
			pGi->oPosition.nX = pObj->nPositionX;
			pGi->oPosition.nY = pObj->nPositionY;
			strcpy(pGi->szImage, pObj->szImage);
			pGi->nFrame = pObj->nFrame;
			pGi->uImage = 0;
			pGi->nISPosition = IMAGE_IS_POSITION_INIT;

			if (pGi->szImage[0] && nNum < LOCAL_MAX_IMG_NUM)
			{
				nNum++;
				pGi++;
			}
		}
	}//end for

	POINT m_curRegPoint;
	m_curRegPoint.x = m_RegionIndex.x;//9大区域改变 这个是不变的数据
	m_curRegPoint.y = m_RegionIndex.y;//9大区域改变 这个是不变的数据

	if (nNum)
		g_GameWorld->_DrawPrimitives_map(bRegionIndex+1,nNum,&ImgList[0],nIndex+1,MAP_NODE_LUMIAN_A,0,false,m_curRegPoint,RegionRBPos);

	char nKey[32];
	t_sprintf(nKey,"%d-%d-dimian",m_curRegPoint.x,m_curRegPoint.y);
	std::string nCurKey = nKey;

	if  (nNum<=0)
	{
		g_GameWorld->ParentNode_map->removeChildByTagKey(nCurKey,true);
		/*CCNode *nRegNode    = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurKey);
		if (!nRegNode)  return;

		CCNode *nPartNode   = nRegNode->getChildByTag(MAP_NODE_LUMIAN_A);
		if (nPartNode)
			nPartNode->removeAllChildren();*/
	}
	//if (nNum)
	//g_GameWorld->DrawPrimitives_map(bRegionIndex,nNum, &ImgList[0], RU_T_IMAGE,MAP_NODE_OTHER,0, false);
}

void KScenePlaceRegionC::PaintGroundDirect(int bRegionIndex)
{//最低层 地面
	if (g_GameWorld == NULL)
		return;
	
	KRUImage	ImgList[LOCAL_MAX_IMG_NUM];
	KRUImage	*pGi;
	unsigned int nIndex;
	int			CellWidth  = RWPP_AREGION_WIDTH/RWP_NUM_GROUND_CELL_H;   //32
	int			CellHeight = RWPP_AREGION_HEIGHT/2/RWP_NUM_GROUND_CELL_V;//32
	int m_isClear = 0;
	memset(&ImgList, 0, sizeof(ImgList));
	int nNum = 0;
	int nCaodiSprCount = 0;
	pGi = &ImgList[0];
	//----地最底表面:草地、水等----
	KSPRCrunode* pGrunode = m_GroundLayerData.pGrunodes;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumGrunode; ++nIndex)
	{//一次性读完地表数据 
		//memset(&ImgList, 0, sizeof(ImgList));
		pGi->bRenderStyle =IMAGE_RENDER_STYLE_OPACITY;//IMAGE_RENDER_STYLE_3LEVEL;// IMAGE_RENDER_STYLE_OPACITY;
		pGi->nType = ISI_T_SPR;
		pGi->oPosition.nX = pGrunode->Param.h * CellWidth + m_LeftTopCornerScenePos.x;       //像素坐标
		pGi->oPosition.nY = (pGrunode->Param.v * CellHeight) * 2 + m_LeftTopCornerScenePos.y;//像素坐标
		memcpy(pGi->szImage, pGrunode->szImgName, pGrunode->Param.nFileNameLen);
		pGi->nFrame = pGrunode->Param.nFrame;//第几帧数
		pGi->uImage = 0;
		pGi->nISPosition = IMAGE_IS_POSITION_INIT;
		pGrunode = (KSPRCrunode*)(((char*)pGrunode) + sizeof(KSPRCrunode::KSPRCrunodeParam) + pGrunode->Param.nFileNameLen);//偏移到这个位置
		
		if  (pGi->szImage[0] && nNum < LOCAL_MAX_IMG_NUM)
		{
		   nNum++;
		   pGi++;
	    }
	}//end for

	//----紧贴地表面的对象:路面等----
	//memset(&ImgList, 0, sizeof(ImgList));
	KRUImagePart	ImgPart[LOCAL_MAX_IMG_NUM];
	KRUImagePart	*pGp;
	//KRUImagePart	ImgPart;
	memset(&ImgPart, 0, sizeof(ImgPart));
	int nPartNum = 0;
	pGp = &ImgPart[0];

	POINT RegionRBPos;//实际绘画的每个区域的大小为(512x512) 结束坐标(像素坐标)
	RegionRBPos.x = m_LeftTopCornerScenePos.x + RWPP_AREGION_WIDTH;
	RegionRBPos.y = m_LeftTopCornerScenePos.y + RWPP_AREGION_HEIGHT;
	int nSprCount = 0;
	int nPartSprCount=0;
	KSPRCoverGroundObj* pObj = m_GroundLayerData.pObjects;
	for (nIndex = 0; nIndex < m_GroundLayerData.uNumObject; ++nIndex, ++pObj)
	{
		//memset(&ImgList, 0, sizeof(ImgList));
		if (pObj->bRelateRegion == OBJ_IS_SELF_OWNED &&
			pObj->nPositionX >= m_LeftTopCornerScenePos.x &&
			pObj->nPositionY >= m_LeftTopCornerScenePos.y &&
			pObj->nPositionX + pObj->nWidth < RegionRBPos.x&&
			pObj->nPositionY + pObj->nHeight * 2 < RegionRBPos.y)
		{//完整的部分
			pGi->bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;//;//IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_3LEVEL;
			pGi->nType = ISI_T_SPR;
			pGi->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
			pGi->Color.Color_b.a = 125;  //125 pGi->Color.Color_b.a = 255;
			pGi->oPosition.nX = pObj->nPositionX;
			pGi->oPosition.nY = pObj->nPositionY;
			strcpy(pGi->szImage, pObj->szImage);
			pGi->nFrame = pObj->nFrame;
			pGi->uImage = 0;
			pGi->nISPosition = IMAGE_IS_POSITION_INIT;

		    if (pGi->szImage[0] && nNum < LOCAL_MAX_IMG_NUM)
			{
			  nNum++;
			  pGi++;
		    }
		}
		else
		{//需要剪切的部分

			pGp->Color.Color_dw = 0;
			pGp->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
			pGp->bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_3LEVEL;//IMAGE_RENDER_STYLE_ALPHA;//IMAGE_RENDER_STYLE_3LEVEL;
			pGp->nType = ISI_T_SPR;
			pGp->oPosition.nZ = 0;
			pGp->oEndPos.nZ = 0;
			pGp->szImage[0] = 0;


			strcpy(pGp->szImage, pObj->szImage);
			pGp->nFrame = pObj->nFrame;
			pGp->uImage = 0;
			pGp->nISPosition = -1;
		
			if (pObj->nPositionX < m_LeftTopCornerScenePos.x)
				pGp->oImgLTPos.nX = m_LeftTopCornerScenePos.x - pObj->nPositionX;
			else
				pGp->oImgLTPos.nX = 0;

			if (pObj->nPositionX + pObj->nWidth >= RegionRBPos.x)
				pGp->oImgRBPos.nX = RegionRBPos.x - pObj->nPositionX;
			else
				pGp->oImgRBPos.nX = pObj->nWidth;

			if (pObj->nPositionY < m_LeftTopCornerScenePos.y)
				pGp->oImgLTPos.nY = (m_LeftTopCornerScenePos.y - pObj->nPositionY) / 2;
			else
				pGp->oImgLTPos.nY = 0;

			if (pObj->nPositionY + pObj->nHeight * 2 >= RegionRBPos.y)
				pGp->oImgRBPos.nY = (RegionRBPos.y - pObj->nPositionY) / 2;
			else
				pGp->oImgRBPos.nY = pObj->nHeight;

			pGp->oPosition.nX = pObj->nPositionX + pGp->oImgLTPos.nX;
			pGp->oPosition.nY = pObj->nPositionY + pGp->oImgLTPos.nY * 2;

			pGp->oEndPos.nX = pObj->nPositionX + pGp->oImgRBPos.nX;
			pGp->oEndPos.nY = pObj->nPositionY + pGp->oImgRBPos.nY * 2;
           
			if (pGp->szImage[0] && nPartNum < LOCAL_MAX_IMG_NUM)
			{
				nPartNum++;
				pGp++;
			}
	
		}//end if
	}//end for

	POINT m_curRegPoint;
	m_curRegPoint.x = m_RegionIndex.x;//9大区域改变 这个是不变的数据
	m_curRegPoint.y = m_RegionIndex.y;//9大区域改变 这个是不变的数据

	if (nNum)
	   g_GameWorld->_DrawPrimitives_map(bRegionIndex+1,nNum,&ImgList[0],nIndex+1,MAP_NODE_LUMIAN_A,0,false,m_curRegPoint,RegionRBPos);

	if (nPartNum) //Draw_Part_map
		g_GameWorld->Draw_Part_map(bRegionIndex+1,nPartNum,&ImgPart[0],nIndex+1,MAP_NODE_PART,0,false,m_curRegPoint,RegionRBPos);

	   char nKey[32];
	   t_sprintf(nKey,"%d-%d-dimian",m_curRegPoint.x,m_curRegPoint.y);
	   std::string nFullKey = nKey;
	   if  (nNum<=0)
	   {
		   (g_GameWorld->ParentNode_map)->removeChildByTagKey(nFullKey,true);
		   /*CCNode *nRegNode    = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurKey);
		   if (!nRegNode)  return;

		   CCNode *nPartNode   = nRegNode->getChildByTag(MAP_NODE_LUMIAN_A);
			if (nPartNode)
			    nPartNode->removeAllChildren();*/
	   }
	   t_sprintf(nKey,"%d-%d-part",m_curRegPoint.x,m_curRegPoint.y);
	   std::string nPartKey = nKey;
	   if  (nPartNum<=0)
	   {
		    (g_GameWorld->ParentNode_map)->removeChildByTagKey(nPartKey,true);
		   /*CCNode *nRegNode         = (g_GameWorld->ParentNode_map)->getChildByTagKey(nCurKey);
		   if (!nRegNode)  return;
		   CCNode *nOnePartNode     = nRegNode->getChildByTag(MAP_NODE_PART);

		   if (nOnePartNode)
			   nOnePartNode->removeAllChildren();*/
	   }
	//if (nNum)
		//g_GameWorld->DrawPrimitives_map(bRegionIndex,nNum, &ImgList[0], RU_T_IMAGE,MAP_NODE_OTHER,0, false);
}

void KScenePlaceRegionC::ReomeGroundObj(int bRegionIndex)
{
	if (g_GameWorld == NULL)
		return;
	CCNode *nRegNode    = (g_GameWorld->ParentNode_map)->getChildByTag(bRegionIndex+1);
	CCNode *nLuMianNode = NULL;
	int  nLuMianSprCount= 0;

	if (nRegNode)
	{
		nLuMianNode = nRegNode->getChildByTag(MAP_NODE_LUMIAN);
		if (nLuMianNode)
			nLuMianSprCount = nLuMianNode->getChildrenCount();
	}
	int  m_isClear=0;
	if  (m_isClear<=0)
	{
		//刚好到这里 就监测到 已经更换区域的已经添加了节点 导致闪屏
		//if  (nLuMianNode && nLuMianSprCount)
			//nLuMianNode->removeAllChildren();
	}
}
//##ModelId=3DDBD8C80309
void KScenePlaceRegionC::PaintGround(int bRegionIndex,int fullorpart)
{
	//if (m_pPrerenderGroundImg && bPrerenderGroundImg)
		//g_pRepresent->DrawPrimitives(1, m_pPrerenderGroundImg, RU_T_IMAGE, false);
	//else
	    if  (fullorpart)
		    PaintGroundDirect_full(bRegionIndex);
		else
			PaintGroundDirect_Part(bRegionIndex);
}

void KScenePlaceRegionC::PaintBuildinObj(RECT* pRepresentArea,int bRegionIndex,bool isClear)
{
	    if (!g_GameWorld)
		   return;
	    if (pRepresentArea==NULL || isClear) 
		{//删除节点精灵
			if (m_ClearState==0)
			{
				m_ClearState =1;//设置已经删除
				char nRegKey[32]={0},nKey[32]={0};
				t_sprintf(nRegKey,"%d-%d",m_RegionIndex.x,m_RegionIndex.y);
				std::string nCurRegKey = nRegKey;
				(g_GameWorld->ParentNode_Bigmap)->removeChildByTagKey(nCurRegKey,true);
				//CCNode *nRegNode = (g_GameWorld->ParentNode_Bigmap)->getChildByTagKey(nCurRegKey);
				//if (nRegNode)
					//nRegNode->removeAllChildren();
			}
			
			return;
		}
		m_ClearState = 0;
	   //pLeaf = (KIpotLeaf*)m_BiosData.pLeafs;
	  // (m_BiosData.pLeafs)->PaintABuildinObject(pRepresentArea,bRegionIndex);
	   //这个区域的内建数
		if  (m_BiosData.pBios==NULL || m_BiosData.pLeafs==NULL) return;

		KIpotBuildinObj* pLeaf = NULL;
		//KBuildinObj*	 pObj  = NULL;

		unsigned int nCount = m_BiosData.Numbers.nNumBios - m_BiosData.Numbers.nNumBiosAbove; //减去高空数目

		pLeaf = m_BiosData.pLeafs;
		//pObj  = m_BiosData.pBios;
		if  (nCount<0)
			return;

#define BIG_MAX_IMG_NUM 100

		KRUImage4	ImgListTrees[BIG_MAX_IMG_NUM];
		KRUImage4	*pGtree;
		memset(&ImgListTrees, 0, sizeof(ImgListTrees));
		int nTreeNum = 0;
		pGtree = &ImgListTrees[0];

		KRUImage4	ImgListHouse[BIG_MAX_IMG_NUM];
		KRUImage4	*pGhouse;
		memset(&ImgListHouse, 0, sizeof(ImgListHouse));
		int nHouseNum = 0;
		pGhouse = &ImgListHouse[0];

		int i;
		for (i = 0; i < nCount; ++i, ++pLeaf) //++pLeaf,
		{
			if (!pLeaf) continue;

			/*KRUImage4	Image;
			Image.Color.Color_dw = 0;
			Image.nType          = ISI_T_SPR;
			Image.nFrame         = (pLeaf->pBio)->nFrame;
			Image.nISPosition    = -1;     //(m_BiosData.pLeafs)->nISPosition;
			strcpy(Image.szImage, (pLeaf->pBio)->szImage);
			Image.uImage         = 0;      //(pLeaf)->uImage;
			*/
			pGtree->Color.Color_dw = 0;
			pGtree->nType          = ISI_T_SPR;
			pGtree->nFrame         = (pLeaf->pBio)->nFrame;
			pGtree->nISPosition    = pLeaf->nISPosition;
			strcpy(pGtree->szImage, (pLeaf->pBio)->szImage);
			pGtree->uImage         = pLeaf->uImage;

			pGhouse->Color.Color_dw = 0;
			pGhouse->nType          = ISI_T_SPR;
			pGhouse->nFrame         = (pLeaf->pBio)->nFrame;
			pGhouse->nISPosition    = pLeaf->nISPosition;
			strcpy(pGhouse->szImage, (pLeaf->pBio)->szImage);
			pGhouse->uImage         = pLeaf->uImage;

			
			if (((pLeaf->pBio)->Props & SPBIO_F_HIGHT_LIGHT) == 0)
			{
				if ((pLeaf->pBio)->nImgWidth > 128 && (pLeaf->pBio)->nImgHeight > 128)
				{
					//Image.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
					pGtree->bRenderStyle  = IMAGE_RENDER_STYLE_3LEVEL;
					pGhouse->bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
				}
				else
				{
					//Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
					//Image.Color.Color_b.a = 255;
					pGtree->bRenderStyle     = IMAGE_RENDER_STYLE_ALPHA;
					pGtree->Color.Color_b.a  = 255;
					pGhouse->bRenderStyle    = IMAGE_RENDER_STYLE_ALPHA;
					pGhouse->Color.Color_b.a = 255;
				}
			}
			else
			{
				//Image.bRenderStyle = IMAGE_RENDER_STYLE_BORDER;
				pGtree->bRenderStyle  = IMAGE_RENDER_STYLE_BORDER;
				pGhouse->bRenderStyle = IMAGE_RENDER_STYLE_BORDER;
			}

			if ((pLeaf->pBio)->nAniSpeed == 0)
			{//单帧
				/*Image.bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
				Image.oPosition.nX = (pLeaf->pBio)->ImgPos1.x;
				Image.oPosition.nY = (pLeaf->pBio)->ImgPos1.y;
				Image.oPosition.nZ = (pLeaf->pBio)->ImgPos1.z;

				Image.oEndPos.nX = (pLeaf->pBio)->ImgPos3.x;
				Image.oEndPos.nY = (pLeaf->pBio)->ImgPos3.y;
				Image.oEndPos.nZ = (pLeaf->pBio)->ImgPos3.z;
				*/

				pGtree->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
				pGtree->oPosition.nX = (pLeaf->pBio)->ImgPos1.x;
				pGtree->oPosition.nY = (pLeaf->pBio)->ImgPos1.y;
				pGtree->oPosition.nZ = (pLeaf->pBio)->ImgPos1.z;
				pGtree->oEndPos.nX = (pLeaf->pBio)->ImgPos3.x;
				pGtree->oEndPos.nY = (pLeaf->pBio)->ImgPos3.y;
				pGtree->oEndPos.nZ = (pLeaf->pBio)->ImgPos3.z;

				pGhouse->bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
				pGhouse->oPosition.nX = (pLeaf->pBio)->ImgPos1.x;
				pGhouse->oPosition.nY = (pLeaf->pBio)->ImgPos1.y;
				pGhouse->oPosition.nZ = (pLeaf->pBio)->ImgPos1.z;
				pGhouse->oEndPos.nX = (pLeaf->pBio)->ImgPos3.x;
				pGhouse->oEndPos.nY = (pLeaf->pBio)->ImgPos3.y;
				pGhouse->oEndPos.nZ = (pLeaf->pBio)->ImgPos3.z;

				if (((pLeaf->pBio)->Props & SPBIO_P_SORTMANNER_MASK) == SPBIO_P_SORTMANNER_POINT)
				{//树木花草和一些装饰品
					/*if (Image.oEndPos.nY >= pRepresentArea->top)
					{   
						POINT nCurPoint;
						nCurPoint.x = m_RegionIndex.x;//Image.oPosition.nX/512;
						nCurPoint.y = m_RegionIndex.y;//(Image.oPosition.nY - 6)/1024;
						if (Image.szImage[0])
							g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,1,&Image,0,MAP_NODE_TREES,0,false,nCurPoint);
					}*/
					if (pGtree->oEndPos.nY >= pRepresentArea->top)
					{   
						if (pGtree->szImage[0])
						{
							nTreeNum++;
							pGtree++;
							//g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,1,&Image,0,MAP_NODE_TREES,0,false,nCurPoint);
						}
					}
					//g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE, false)
				}
				else
				{
					if (pGhouse->oEndPos.nY > pRepresentArea->top && pLeaf->pBio->ImgPos3.x >= pRepresentArea->left && pLeaf->pBio->ImgPos4.x < pRepresentArea->right)
					{//房屋 墙壁
						/*Image.oSecondPos.nX = (pLeaf->pBio)->ImgPos2.x;
						Image.oSecondPos.nY = (pLeaf->pBio)->ImgPos2.y;
						Image.oSecondPos.nZ = (pLeaf->pBio)->ImgPos2.z;

						Image.oThirdPos.nX = (pLeaf->pBio)->ImgPos4.x;
						Image.oThirdPos.nY = (pLeaf->pBio)->ImgPos4.y;
						Image.oThirdPos.nZ = (pLeaf->pBio)->ImgPos4.z;

						Image.oImgLTPos.nX = 0;
						Image.oImgLTPos.nY = 0;
						Image.oImgRBPos.nX = (pLeaf->pBio)->nImgWidth;
						Image.oImgRBPos.nY = (pLeaf->pBio)->nImgHeight;
						*/
						pGhouse->oSecondPos.nX = (pLeaf->pBio)->ImgPos2.x;
						pGhouse->oSecondPos.nY = (pLeaf->pBio)->ImgPos2.y;
						pGhouse->oSecondPos.nZ = (pLeaf->pBio)->ImgPos2.z;

						pGhouse->oThirdPos.nX = (pLeaf->pBio)->ImgPos4.x;
						pGhouse->oThirdPos.nY = (pLeaf->pBio)->ImgPos4.y;
						pGhouse->oThirdPos.nZ = (pLeaf->pBio)->ImgPos4.z;

						pGhouse->oImgLTPos.nX = 0;
						pGhouse->oImgLTPos.nY = 0;
						pGhouse->oImgRBPos.nX = (pLeaf->pBio)->nImgWidth;
						pGhouse->oImgRBPos.nY = (pLeaf->pBio)->nImgHeight;

						if (pGhouse->szImage[0])
						{
							nHouseNum++;
							pGhouse++;
							//g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,1,&Image,0,MAP_NODE_HOUSE,0,false,nCurPoint);
						}
						//g_pRepresent->DrawPrimitives(1, &Image, RU_T_IMAGE_4, false);
					}
				}
			}
		}//end for
		POINT nCurPoint;
		nCurPoint.x = m_RegionIndex.x;//Image.oPosition.nX/512;
		nCurPoint.y = m_RegionIndex.y;//(Image.oPosition.nY - 6)/1024;

		//char msg[128];
		//t_sprintf(msg,"Reg:%d,nTNum:%d,nHNum:%d,RegX:%d,RegY:%d",bRegionIndex,nTreeNum,nHouseNum,nCurPoint.x,nCurPoint.y);
		//CCMessageBox(msg,"PaintBuildinObj");

		if  (nTreeNum>0)//DrawPrimitives_BigMap_Tree
			g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,nTreeNum,&ImgListTrees[0],0,MAP_NODE_TREES,0,false,nCurPoint);
		
		if  (nHouseNum>0)//DrawPrimitives_BigMap_house
			g_GameWorld->DrawPrimitives_BigMap(bRegionIndex+1,nHouseNum,&ImgListHouse[0],0,MAP_NODE_HOUSE,0,false,nCurPoint);

		char nRegKey[32]={0},nKey[32]={0};
		t_sprintf(nRegKey,"%d-%d",nCurPoint.x,nCurPoint.y);
		std::string nCurRegKey = nRegKey;

		CCNode *nRegNode = (g_GameWorld->ParentNode_Bigmap)->getChildByTagKey(nCurRegKey);

		if (nRegNode && m_BackTreeNum>0 && nTreeNum>0 && m_BackTreeNum>nTreeNum)
		{

			CCNode *nPartNode = nRegNode->getChildByTag(MAP_NODE_TREES);
			if  (nPartNode)
			{
				for (int i=nTreeNum;i<m_BackTreeNum;i++)
				{
					t_sprintf(nKey,"tree-%d",i+1);
					std::string nCurKey = nKey;
					nPartNode->removeChildByTagKey(nCurKey,true);
				}
			}
		}

		if (nRegNode &&  m_BackHouseNum>0 && nHouseNum>0 && m_BackHouseNum>nHouseNum)
		{
			CCNode *nPartNode = nRegNode->getChildByTag(MAP_NODE_HOUSE);
			if  (nPartNode)
			{
				for (int i=nHouseNum;i<m_BackHouseNum;i++)
				{
					t_sprintf(nKey,"house-%d",i+1);
					std::string nCurKey = nKey;
					nPartNode->removeChildByTagKey(nCurKey,true);
				}
			}
		}
		m_BackTreeNum  = nTreeNum;
		m_BackHouseNum = nHouseNum;
}

//#ifdef SWORDONLINE_SHOW_DBUG_INFO
void KScenePlaceRegionC::PaintObstacle()
{//绘画障碍
	/*KRULine	Line[4];
	int		nNumLine, i, j, nX, nY;

	for (i = 0; i < 4; ++i)
	{
		Line[i].Color.Color_dw = 0xffffffff;
		Line[i].oPosition.nZ = 0;
		Line[i].oEndPos.nZ = 0;
	}

	nX = m_LeftTopCornerScenePos.x;
	for (i = 0; i < 16; ++i, nX += RWP_OBSTACLE_WIDTH)
	{
		nY = m_LeftTopCornerScenePos.y;
		for (j = 0; j < 32; ++j, nY += RWP_OBSTACLE_HEIGHT)
		{
			long nTrap = m_cTrap.GetData(i,j);//m_TrapInfo[i][j];
			if (nTrap > 0)
			{//TRAP点
				char	szDebugTrap[32]={0};
				t_sprintf(szDebugTrap, "%x", nTrap);
				g_pRepresent->OutputText(14, szDebugTrap, KRF_ZERO_END, nX, nY, 0xffffffff, 0, 0);
			}

			int nIdx = SubWorld[0].FindRegion(MAKELONG(m_RegionIndex.x, m_RegionIndex.y));
			if (nIdx >= 0)
			{//区域内的NPC
				int nRef = SubWorld[0].m_Region[nIdx].GetNpcRef(i,j);//SubWorld[0].m_Region[nIdx].GetRef(i,j,obj_npc);
				char szDebugNpcRef[64]={0};
				if (nRef)
				{
					t_sprintf(szDebugNpcRef, "NPC(%d/%d):%d,X:%d,Y:%d,域:%d,总:%d",nX/32/16,nY/32/32, nRef,i,j,nIdx,SubWorld[0].m_nTotalRegion);
					g_pRepresent->OutputText(14, szDebugNpcRef, KRF_ZERO_END, nX, nY, 0xffffffff, 0, 0);
				}

				nRef = SubWorld[0].m_Region[nIdx].GetObjRef(i,j);//SubWorld[0].m_Region[nIdx].GetRef(i,j,obj_object);
				//char	szDebugNpcRef[32];
				if (nRef)
				{
					t_sprintf(szDebugNpcRef, "object:%d,X:%d,Y:%d", nRef,i,j);
					g_pRepresent->OutputText(14, szDebugNpcRef, KRF_ZERO_END, nX, nY, 0xffffffff, 0, 0);
				
			}
		   if (g_iShowObstacle)
		   {//m_ObstacleInfo[i][j]
			long nType = (m_cObstacle.GetData(i,j) >> 4) & 0x0000000f;  //判断障碍的 16 32 * 16
			if (nType == Obstacle_Empty)
				continue;

			Line[2].oEndPos.nX = Line[1].oEndPos.nX = Line[0].oEndPos.nX =
				Line[2].oPosition.nX = Line[1].oPosition.nX = Line[0].oPosition.nX = nX;
			Line[2].oEndPos.nY = Line[1].oEndPos.nY = Line[0].oEndPos.nY =
				Line[2].oPosition.nY = Line[1].oPosition.nY = Line[0].oPosition.nY = nY;
			nNumLine = 0;
			switch(nType)
			{
			case Obstacle_Full:
				Line[0].oEndPos.nX      += 32;	//上
				Line[1].oEndPos.nY      += 32;	//左
				Line[2].oPosition.nX    += 32;	//右
				Line[2].oEndPos.nX      += 32;
				Line[2].oEndPos.nY      += 32;
				Line[3].oPosition.nX	= Line[0].oPosition.nX;	//下
				Line[3].oPosition.nY    = Line[2].oEndPos.nY;
				Line[3].oEndPos.nX      = Line[2].oEndPos.nX;
				Line[3].oEndPos.nY      = Line[2].oEndPos.nY;
				nNumLine = 4;
				break;
			case Obstacle_LT:
				Line[0].oEndPos.nX      += 32;	//上
				Line[1].oEndPos.nY      += 32;	//左
				Line[2].oPosition.nY    += 32;	//左斜
				Line[2].oEndPos.nX      += 32;
				nNumLine = 3;
				break;
			case Obstacle_RT:
				Line[0].oEndPos.nX      += 32;	//上
				Line[1].oPosition.nX    += 32;	//右
				Line[1].oEndPos.nX      += 32;
				Line[1].oEndPos.nY      += 32;
				Line[2].oEndPos.nX      += 32;	//右斜
				Line[2].oEndPos.nY      += 32;
				nNumLine = 3;
				break;
			case Obstacle_LB:
				Line[0].oEndPos.nY      += 32;	//左
				Line[1].oPosition.nY    += 32;	//下
				Line[1].oEndPos.nX      += 32;
				Line[1].oEndPos.nY      += 32;
				Line[2].oEndPos.nX      += 32;	//右斜
				Line[2].oEndPos.nY      += 32;
				nNumLine = 3;
				break;
			case Obstacle_RB:
				Line[0].oPosition.nX    += 32;	//右
				Line[0].oEndPos.nX      += 32;
				Line[0].oEndPos.nY      += 32;
				Line[1].oPosition.nY	+= 32;	//左斜
				Line[1].oEndPos.nX      += 32;
				Line[2].oPosition.nY    += 32;	//下
				Line[2].oEndPos.nX      += 32;
				Line[2].oEndPos.nY      += 32;
				nNumLine = 3;
				break;
			}
//			g_pRepresent->DrawPrimitives(nNumLine, Line, RU_T_LINE, false);
		   }
		}
	}
		*/
}
//#endif


//获取下一个坐标点
BOOL KScenePlaceRegionC::GetNextMapPos()
{//绘画障碍
	/*KRULine	Line[4];
	int		nNumLine, i, j, nX, nY,nReg=0;

	for (i = 0; i < 4; ++i)
	{
		Line[i].Color.Color_dw = 0xffffffff;
		Line[i].oPosition.nZ = 0;
		Line[i].oEndPos.nZ = 0;
	}

	nX = m_LeftTopCornerScenePos.x;
	for (i = 0; i < 16; ++i, nX += RWP_OBSTACLE_WIDTH)
	{
		nY = m_LeftTopCornerScenePos.y;
		for (j = 0; j < 32; ++j, nY += RWP_OBSTACLE_HEIGHT)
		{
			int nIdx = SubWorld[0].FindRegion(MAKELONG(m_RegionIndex.x, m_RegionIndex.y));
			if (nIdx >= 0)
			{//区域内的NPC
			

			}
			//m_ObstacleInfo[i][j]
			long nType = (m_cObstacle.GetData(i,j) >> 4) & 0x0000000f;  //判断障碍的 16 32 * 16

			if (nType == Obstacle_Empty)
			{
				nReg=TRUE;
				break;
			}

			Line[2].oEndPos.nX = Line[1].oEndPos.nX = Line[0].oEndPos.nX =
				Line[2].oPosition.nX = Line[1].oPosition.nX = Line[0].oPosition.nX = nX;
			Line[2].oEndPos.nY = Line[1].oEndPos.nY = Line[0].oEndPos.nY =
				Line[2].oPosition.nY = Line[1].oPosition.nY = Line[0].oPosition.nY = nY;
			nNumLine = 0;

			switch(nType)
			{
			case Obstacle_Full:
				Line[0].oEndPos.nX += 32;	//上
				Line[1].oEndPos.nY += 32;	//左
				Line[2].oPosition.nX += 32;	//右
				Line[2].oEndPos.nX += 32;
				Line[2].oEndPos.nY += 32;
				Line[3].oPosition.nX = Line[0].oPosition.nX;	//下
				Line[3].oPosition.nY = Line[2].oEndPos.nY;
				Line[3].oEndPos.nX = Line[2].oEndPos.nX;
				Line[3].oEndPos.nY = Line[2].oEndPos.nY;
				nNumLine = 4;
				break;
			case Obstacle_LT:
				Line[0].oEndPos.nX += 32;	//上
				Line[1].oEndPos.nY += 32;	//左
				Line[2].oPosition.nY += 32;	//左斜
				Line[2].oEndPos.nX += 32;
				nNumLine = 3;
				break;
			case Obstacle_RT:
				Line[0].oEndPos.nX += 32;	//上
				Line[1].oPosition.nX    += 32;	//右
				Line[1].oEndPos.nX += 32;
				Line[1].oEndPos.nY += 32;
				Line[2].oEndPos.nX += 32;	//右斜
				Line[2].oEndPos.nY += 32;
				nNumLine = 3;
				break;
			case Obstacle_LB:
				Line[0].oEndPos.nY += 32;	//左
				Line[1].oPosition.nY += 32;	//下
				Line[1].oEndPos.nX += 32;
				Line[1].oEndPos.nY += 32;
				Line[2].oEndPos.nX += 32;	//右斜
				Line[2].oEndPos.nY += 32;
				nNumLine = 3;
				break;
			case Obstacle_RB:
				Line[0].oPosition.nX += 32;	//右
				Line[0].oEndPos.nX += 32;
				Line[0].oEndPos.nY += 32;
				Line[1].oPosition.nY += 32;	//左斜
				Line[1].oEndPos.nX += 32;
				Line[2].oPosition.nY += 32;	//下
				Line[2].oEndPos.nX += 32;
				Line[2].oEndPos.nY += 32;
				nNumLine = 3;
				break;
			}
			g_pRepresent->DrawPrimitives(nNumLine, Line, RU_T_LINE, false);

		}
	}
	*/
	return  0 ;//nReg;
}


//##ModelId=3DE29F360221
void KScenePlaceRegionC::PaintAboveHeadObj(KBuildinObj* pObj, RECT* pRepresentArea,int nindex)
{
	//_ASSERT(pObj);
	if  (pObj==NULL) return;

	if  (!g_GameWorld) return;

	//char nTrapInfo[256]={0};
	KRUImage4	Img;
	Img.Color.Color_dw = 0;	
	Img.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
	Img.nType = ISI_T_SPR;
	strcpy(Img.szImage, pObj->szImage);

	Img.nFrame = pObj->nFrame;
	Img.uImage = pObj->OBJ_IMAGE_ID;
	Img.nISPosition = pObj->OBJ_IMAGE_ISPOSITION;

	POINT nCurPoint;
	nCurPoint.x = 0;
	nCurPoint.y = 0;

	if (pObj->nAniSpeed == 0)
	{//单帧数
		Img.bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
		if (SM_IsOutsideRepresentArea(pRepresentArea,
			pObj->ImgPos1.x, pObj->ImgPos1.y, pObj->ImgPos1.z,
			pObj->ImgPos2.x, pObj->ImgPos2.y, pObj->ImgPos2.z) == false)
		{//每次也只能画一张 共有最多9张
			Img.oPosition.nX = pObj->ImgPos1.x;
			Img.oPosition.nY = pObj->ImgPos1.y;
			Img.oPosition.nZ = pObj->ImgPos1.z;
			Img.oEndPos.nX = pObj->ImgPos3.x;
			Img.oEndPos.nY = pObj->ImgPos3.y;
			Img.oEndPos.nZ = pObj->ImgPos3.z;
			
			if ((pObj->Props & SPBIO_P_SORTMANNER_MASK) == SPBIO_P_SORTMANNER_POINT)
			{//无偏移绘画 花草 等
//				g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, false);
				g_GameWorld->DrawAboveHead(nindex,nCurPoint,1,&Img,BAOVE_NODE_FULL,false);
			}
			else
			{//有偏移绘画
				Img.oSecondPos.nX = pObj->ImgPos2.x;
				Img.oSecondPos.nY = pObj->ImgPos2.y;
				Img.oSecondPos.nZ = pObj->ImgPos2.z;
				
				Img.oThirdPos.nX = pObj->ImgPos4.x;
				Img.oThirdPos.nY = pObj->ImgPos4.y;
				Img.oThirdPos.nZ = pObj->ImgPos4.z;
				
				Img.oImgLTPos.nX = 0;
				Img.oImgLTPos.nY = 0;
				Img.oImgRBPos.nX = pObj->nImgWidth;
				Img.oImgRBPos.nY = pObj->nImgHeight;	
//				g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_4, false);
				g_GameWorld->DrawAboveHead(nindex,nCurPoint,1,&Img,BAOVE_NODE_PART,false);
			}
		}
	}
	/*else
	{//多帧
		Img.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		Img.oPosition.nX = pObj->oPos1.x;
		Img.oPosition.nY = pObj->oPos1.y;
		Img.oPosition.nZ = pObj->oPos1.z;

		BuildinObjNextFrame(pObj);
	}*/

	pObj->OBJ_IMAGE_ID = Img.uImage;
	pObj->OBJ_IMAGE_ISPOSITION = Img.nISPosition;
}

//## 获取高空对象最大的绘图次序编号。
unsigned int KScenePlaceRegionC::GetAboveHeadLayer(KBuildinObj*& pObjsAboveHead)
{
	pObjsAboveHead = NULL;
	unsigned int uNum = 0;
	if (m_BiosData.Numbers.nNumBiosAbove && m_BiosData.pBios)
	{
		uNum = m_BiosData.Numbers.nNumBiosAbove;
		pObjsAboveHead = &m_BiosData.pBios[m_BiosData.Numbers.nNumBios - uNum];

	}
	return uNum;
}

//获得内建光源的信息
unsigned int KScenePlaceRegionC::GetBuildinLights(KBuildInLightInfo*& pLights)
{
	unsigned int uNum = 0;
	if (m_BiosData.Numbers.nNumBulidinLight && m_BiosData.pLights)
	{
		pLights = m_BiosData.pLights;
		uNum = m_BiosData.Numbers.nNumBulidinLight;
	}
	return uNum;
}


//##ModelId=3DE33AB30318
void KScenePlaceRegionC::GetBuildinObjs(
		KIpotBuildinObj*& pObjsPointList, unsigned int& nNumObjsPoint,
		KIpotBuildinObj*& pObjsLineList, unsigned int& nNumObjsLine,
		KIpotBuildinObj*& pObjsTreeList, unsigned int& nNumObjsTree)
{
	pObjsPointList = NULL;
	pObjsLineList = NULL;
	pObjsTreeList = NULL;
	nNumObjsPoint = 0;
	nNumObjsLine = 0;
	nNumObjsTree = 0;
	if (m_BiosData.pLeafs)
	{
		int	nCount = 0;
		if (nNumObjsPoint = m_BiosData.Numbers.nNumBiosPoint)
		{
			pObjsPointList = m_BiosData.pLeafs;
			nCount += nNumObjsPoint;
		}
		if (nNumObjsLine = m_BiosData.Numbers.nNumBiosLine)
		{
			pObjsLineList = &m_BiosData.pLeafs[nCount];
			nCount += nNumObjsLine;
		}
		if (nNumObjsTree = m_BiosData.Numbers.nNumBiosTree)
			pObjsTreeList = &m_BiosData.pLeafs[nCount];
	}
}

//## 获取区域内内建未初始化的对象的列表。
void KScenePlaceRegionC::GetBIOSBuildinObjs(
	KBuildinObj*& pObjsList, 
	unsigned int& nNumObjs 
)
{
    pObjsList = NULL;

    nNumObjs = m_BiosData.Numbers.nNumBios;
    if (nNumObjs > 0)
        pObjsList = m_BiosData.pBios;
}


void KScenePlaceRegionC::LeaveProcessArea()
{
	//用KRUImage::GROUND_IMG_OCCUPY_FLAG(bMatchReferenceSpot)来表示KRUImage对象是否被占用
	//用KRUImage::GROUND_IMG_OK_FLAG(bFrameDraw)来表示KRUImage对象是否已经预渲染好了
	if (m_pPrerenderGroundImg)
	{
		m_pPrerenderGroundImg->GROUND_IMG_OCCUPY_FLAG = false;
		m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;
	}

	m_pPrerenderGroundImg = NULL;
	if (m_BiosData.pLeafs)
	{//离开了 区域 释放资源树
		free(m_BiosData.pLeafs);
		m_BiosData.pLeafs = NULL;

	}
}

void KScenePlaceRegionC::EnterProcessArea(KRUImage *pImage)
{
	//用KRUImage::GROUND_IMG_OCCUPY_FLAG(bMatchReferenceSpot)来表示KRUImage对象是否被占用
	//用KRUImage::GROUND_IMG_OK_FLAG(bFrameDraw)来表示KRUImage对象是否已经预渲染好了
	if (pImage && m_pPrerenderGroundImg != pImage)
	{//如果不相同
		if (m_pPrerenderGroundImg)
		{
			m_pPrerenderGroundImg->GROUND_IMG_OCCUPY_FLAG = false;
			m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;
		}
		if (m_pPrerenderGroundImg = pImage)
		{//崩溃
		  if (m_pPrerenderGroundImg)
		  {
			m_pPrerenderGroundImg->GROUND_IMG_OCCUPY_FLAG = true;
			m_pPrerenderGroundImg->GROUND_IMG_OK_FLAG = false;
			m_pPrerenderGroundImg->oPosition.nX = m_LeftTopCornerScenePos.x;
			m_pPrerenderGroundImg->oPosition.nY = m_LeftTopCornerScenePos.y;
			m_pPrerenderGroundImg->oEndPos.nX = m_pPrerenderGroundImg->oPosition.nX + RWPP_AREGION_WIDTH;
			m_pPrerenderGroundImg->oEndPos.nY = m_pPrerenderGroundImg->oPosition.nY + RWPP_AREGION_HEIGHT;
			m_pPrerenderGroundImg->oEndPos.nZ = m_pPrerenderGroundImg->oPosition.nZ = 0;
		  }
		}
	}

	unsigned int i;
	unsigned int nCount = m_BiosData.Numbers.nNumBios
							- m_BiosData.Numbers.nNumBiosAbove; //减去高空数目
	KIpotBuildinObj* pLeaf = NULL;
	KBuildinObj*	 pObj  = NULL;

	if (m_BiosData.pLeafs == NULL && m_BiosData.pBios &&
		m_BiosData.Numbers.nNumBios > m_BiosData.Numbers.nNumBiosAbove)
	{//如果不存在		
		m_BiosData.pLeafs = (KIpotBuildinObj*) malloc(
			sizeof(KIpotBuildinObj) * nCount);
		if (m_BiosData.pLeafs)
		{
			pLeaf = m_BiosData.pLeafs;
			pObj  = m_BiosData.pBios;
			for (i = 0; i < nCount; ++i, ++pLeaf, ++pObj)
			{
				pLeaf->eLeafType = KIpotLeaf::IPOTL_T_BUILDIN_OBJ;

				pLeaf->uImage = 0;
				pLeaf->nISPosition = -1;
				pLeaf->pBio = pObj;

				pLeaf->oPosition.x = pObj->oPos1.x;
				pLeaf->oPosition.y = pObj->oPos1.y;
				pLeaf->oEndPos.x = pObj->oPos2.x;
				pLeaf->oEndPos.y = pObj->oPos2.y;

				pLeaf->fAngleXY  = pObj->fAngleXY;
				pLeaf->fNodicalY = pObj->fNodicalY;

				pLeaf->bClone = false;
				pLeaf->bImgPart = false;
				pLeaf->pBrother = NULL;
				pLeaf->pRChild = NULL;
				pLeaf->pLChild = NULL;
			}
			pLeaf  = m_BiosData.pLeafs;
			nCount = m_BiosData.Numbers.nNumBiosPoint;
			for (i = 0; i < nCount; ++i, ++pLeaf)
				pLeaf->oPosition.y += POINT_LEAF_Y_ADJUST_VALUE;			
		}
	}
	else if(m_BiosData.pLeafs)
	{//如果 存在的
		pLeaf = m_BiosData.pLeafs;
		pObj  = m_BiosData.pBios;
		for (i = 0; i < nCount; ++i, ++pLeaf, ++pObj)
		{
			pLeaf->oPosition.x = pObj->oPos1.x;
			pLeaf->oPosition.y = pObj->oPos1.y;
			pLeaf->oEndPos.x = pObj->oPos2.x;
			pLeaf->oEndPos.y = pObj->oPos2.y;
			pLeaf->fAngleXY  = pObj->fAngleXY;
			pLeaf->fNodicalY = pObj->fNodicalY;
			pLeaf->bImgPart = false;
		}
		pLeaf = m_BiosData.pLeafs;
		//紧跟着
		nCount = m_BiosData.Numbers.nNumBiosPoint;
		for (i = 0; i < nCount; ++i, ++pLeaf)
			pLeaf->oPosition.y += POINT_LEAF_Y_ADJUST_VALUE;			
	}
}
//获取障碍的信息
long KScenePlaceRegionC::GetObstacleInfo(int nX, int nY)
{
	int		nMpsX, nMpsY, nMapX, nMapY;
	long	lInfo, lRet, lType;

	nMpsX = nX - m_LeftTopCornerScenePos.x;
	nMpsY = nY - m_LeftTopCornerScenePos.y;

	nMapX = nMpsX / RWP_OBSTACLE_WIDTH;
	nMapY = nMpsY / RWP_OBSTACLE_HEIGHT;

	CCAssert(nMapX >= 0 && nMapX < RWP_NUM_GROUND_CELL_H && nMapY >= 0 && nMapY < RWP_NUM_GROUND_CELL_V * 2,"");
	lInfo = m_cObstacle.GetData(nMapX,nMapY);//m_ObstacleInfo[nMapX][nMapY];
	nMpsX -= nMapX * RWP_OBSTACLE_WIDTH;
	nMpsY -= nMapY * RWP_OBSTACLE_HEIGHT;
	lRet = lInfo & 0x0000000f;

	lType = (lInfo >> 4) & 0x0000000f;
	switch(lType)
	{
	case Obstacle_LT:
		if (nMpsX + nMpsY > RWP_OBSTACLE_WIDTH)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nMpsX < nMpsY)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nMpsX > nMpsY)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nMpsX + nMpsY < RWP_OBSTACLE_WIDTH)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}
	return lRet;
}
//获取像数点的障碍信息
long KScenePlaceRegionC::GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY)
{

	int		nMpsX, nMpsY, nMapX, nMapY;
	long	lRet, lType;

	nMpsX = nX - m_LeftTopCornerScenePos.x;
	nMpsY = nY - m_LeftTopCornerScenePos.y;
	nMapX = nMpsX / RWP_OBSTACLE_WIDTH;
	nMapY = nMpsY / RWP_OBSTACLE_HEIGHT;
	nMpsX = ((nMpsX - nMapX * RWP_OBSTACLE_WIDTH) << 10) + nOffX;
	nMpsY = ((nMpsY - nMapY * RWP_OBSTACLE_HEIGHT) << 10) + nOffY;

	CCAssert(nMapX >= 0 && nMapX < RWP_NUM_GROUND_CELL_H && nMapY >= 0 && nMapY < RWP_NUM_GROUND_CELL_V * 2,"");
	CCAssert(nOffX >= 0 && nOffX < 1024 && nOffY >= 0 && nOffY < 1024,"");

	lRet = /*m_ObstacleInfo[nMapX][nMapY]*/m_cObstacle.GetData(nMapX,nMapY) & 0x0000000f;
	lType = (/*m_ObstacleInfo[nMapX][nMapY]*/m_cObstacle.GetData(nMapX,nMapY) >> 4) & 0x0000000f;

	switch(lType)
	{
	case Obstacle_LT:
		if (nMpsX + nMpsY > RWP_OBSTACLE_WIDTH << 10)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RT:
		if (nMpsX < nMpsY)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_LB:
		if (nMpsX > nMpsY)
			lRet = Obstacle_NULL;
		break;
	case Obstacle_RB:
		if (nMpsX + nMpsY < RWP_OBSTACLE_WIDTH << 10)
			lRet = Obstacle_NULL;
		break;
	default:
		break;
	}
	return lRet;
}

void KScenePlaceRegionC::SetHightLightSpecialObject(unsigned int uBioIndex)
{
	if (m_BiosData.pBios && uBioIndex < m_BiosData.Numbers.nNumBios)
		m_BiosData.pBios[uBioIndex].Props |= SPBIO_F_HIGHT_LIGHT;
}
void KScenePlaceRegionC::UnsetHightLightSpecialObject(unsigned int uBioIndex)
{
	if (m_BiosData.pBios && uBioIndex < m_BiosData.Numbers.nNumBios)
		m_BiosData.pBios[uBioIndex].Props &= ~SPBIO_F_HIGHT_LIGHT;
}
