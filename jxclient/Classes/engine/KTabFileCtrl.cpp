#include "KTabFileCtrl.h"
#include "KStrBase.h"
#include "KMemClass.h"
#include <string.h>

KTabFileCtrl::KTabFileCtrl()
{
	isload = false;
}

KTabFileCtrl::~KTabFileCtrl()
{
	Clear();
}

BOOL	KTabFileCtrl::Load(LPSTR FileName/*,LPSTR mMemKey*/)
{
	
	KPakFile	File;
	DWORD		dwSize;
	PVOID		Buffer;
	KFile		cFile;
	//check file name
	if (FileName[0] == 0)
		return FALSE;
	
	if (!File.Open(FileName))  //打开文件
	{
		if (cFile.Create(FileName))  //创建一个文件
		{
			if (!File.Open(FileName))
			{
		       cFile.Close();
			   return FALSE;
			}
			 cFile.Close();
		}
      	//printf("不能打开TXT文件,创建: (%s) OK...\n", FileName);	
	}

	dwSize = File.Size();
	isload = true;
	/*if 	(dwSize<=0)
		return FALSE; */
	KMemClass Meme;
	Buffer = Meme.Alloc(dwSize);
	File.Read(Buffer, dwSize);
	File.Close();
	DWORD nCurPos = 0;
	while(nCurPos < dwSize)
	{
		long i = 0;
		char szLine[10000];
		//拷贝某一行的数据流
		while(nCurPos <= dwSize)
		{
			if (((char*)Buffer)[nCurPos] == 0x0d)
				break;
			szLine[i++] = ((char*)Buffer)[nCurPos ++];
		}
		
		szLine[i]	= '\0';
		nCurPos		= nCurPos + 2;
		i++;
		
		TTabLineNode  * pLineNode = new TTabLineNode;
		KList * pList = new KList;
		pLineNode->pList = pList;
		m_RowList.AddTail(pLineNode);
		
		long j = 0;
		//
		char *szData = szLine;
		while (1)
		{
			char *szFind = strstr(szData,"\t");
			if (szFind == NULL) 
			{
				
				char * newStr = new char[strlen(szData) + 1];
				g_StrCpyLen(newStr,  szData , strlen(szData) + 1);
				if (newStr[strlen(newStr) - 1] == 0x0d)
					newStr[strlen(newStr) - 1] = '\0';
				
				TTabColNode  * pNode = new TTabColNode;
				pNode->m_Str = newStr;
				pLineNode->pList->AddTail(pNode);	
				break;
			}
			char * newStr = new char[szFind - szData + 1];
			g_StrCpyLen(newStr,  szData , szFind - szData + 1);
				
			TTabColNode  * pNode = new TTabColNode;
			pNode->m_Str = newStr;
			pLineNode->pList->AddTail(pNode);
			szData = szFind + 1;
		}
		
	}
	return TRUE;
}

BOOL	KTabFileCtrl::openfile(LPSTR FileName)
{
	
	KFile	File;
	DWORD		dwSize;
	PVOID		Buffer;
	
	// check file name
	if (FileName[0] == 0)
		return FALSE;
	
	if (!File.Open(FileName))  //打开文件
	{
		//g_DebugLog("Can't open ini file : %s", FileName);
		//printf("不能打开TXT文件: %s\n", FileName);
		//return FALSE;
		if (File.Create(FileName))  //创建一个文件
		{
			if (!File.Open(FileName))
			{
				File.Close();
				return FALSE;
			}
		}
	}
	
	dwSize = File.Size();
	isload = true;
	 /*if (dwSize<=0)
		 return FALSE;*/
	KMemClass Meme;
	Buffer = Meme.Alloc(dwSize);
	File.Read(Buffer, dwSize);
	File.Close();
	DWORD nCurPos = 0;
	while(nCurPos < dwSize)
	{
		long i = 0;
		char szLine[10000];
		//拷贝某一行的数据流
		while(nCurPos <= dwSize)
		{
			if (((char*)Buffer)[nCurPos] == 0x0d)
				break;
			szLine[i++] = ((char*)Buffer)[nCurPos ++];
		}
		
		szLine[i]		= '\0';
		nCurPos		= nCurPos + 2;
		i++;
		
		TTabLineNode  * pLineNode = new TTabLineNode;
		KList * pList = new KList;
		pLineNode->pList = pList;
		m_RowList.AddTail(pLineNode);
		
		long j = 0;
		//
		char *szData = szLine;
		while (1)
		{
			char *szFind = strstr(szData,"\t");
			if (szFind == NULL) 
			{
				
				char * newStr = new char[strlen(szData) + 1];
				g_StrCpyLen(newStr,  szData , strlen(szData) + 1);
				if (newStr[strlen(newStr) - 1] == 0x0d)
					newStr[strlen(newStr) - 1] = '\0';
				
				TTabColNode  * pNode = new TTabColNode;
				pNode->m_Str = newStr;
				pLineNode->pList->AddTail(pNode);	
				break;
			}
			char * newStr = new char[szFind - szData + 1];
			g_StrCpyLen(newStr,  szData , szFind - szData + 1);
			
			TTabColNode  * pNode = new TTabColNode;
			pNode->m_Str = newStr;
			pLineNode->pList->AddTail(pNode);
			szData = szFind + 1;
		}
		
	}
	return TRUE;
}

BOOL	KTabFileCtrl::LoadPack(LPSTR FileName)
{
	return TRUE;
}

bool KTabFileCtrl::CreatFile(LPSTR FileName)//清空文件
{
	KFile	File;
	if (FileName[0] == 0 || !FileName)
		return false;
	// create ini file
	if (!File.Create(FileName))  //创建一个文件
		return false;

	File.Close();
	return true;
}

BOOL	KTabFileCtrl::Save(LPSTR FileName)
{
	KFile		File;
	if (FileName[0] == 0)
		return FALSE;
	
	// create ini file
	if (!File.Create(FileName))  //创建一个文件
	{
		return FALSE;
	}
	
	// write ini file
	char szCol[1000];  //1000列
	TTabLineNode 	* pLineNode = (TTabLineNode*)m_RowList.GetHead();
	while(pLineNode)  //循环写文件 行
	{
		TTabColNode * pNode = (TTabColNode*)pLineNode->pList->GetHead();  //列
		while (pNode)
		{
			TTabColNode * pNextNode = (TTabColNode*)pNode->GetNext();
			if (pNextNode)//如果有下一列
				t_sprintf(szCol, "%s\t", pNode->m_Str); //列字符串
			else 
				t_sprintf(szCol,"%s",pNode->m_Str);

			File.Write(szCol, g_StrLen(szCol));      //写内容
			pNode = (TTabColNode*)pNode->GetNext();  //下一列
			
		}
		File.Write((void *)"\15", 1);  //0x0D0A  15
		File.Write((void *)"\n", 1) ;  //换行符
		
		pLineNode  = (TTabLineNode * )pLineNode->GetNext();
	}

	File.Close();

	return TRUE;
}
//获得某行的名
LPSTR	KTabFileCtrl::GetRowName(int nRow)
{	
	TTabLineNode * pLineNode = (TTabLineNode*) m_RowList.GetHead();

	for (int i = 0 ; i < nRow - 1 ;  i++)
	{
		if (!pLineNode) return NULL;
		pLineNode = (TTabLineNode*) pLineNode->GetNext();
	}
	if (!pLineNode) return NULL;
	return ((TTabColNode *)(pLineNode->pList->GetHead()))->m_Str;
}
//获得某列的名
LPSTR	KTabFileCtrl::GetColName(int nCol)
{
	TTabLineNode * pLineNode  = (TTabLineNode *) m_RowList .GetHead();
	if (!pLineNode) return NULL;
	TTabColNode  * pColNode	= (TTabColNode*)pLineNode->pList->GetHead();
	for (int i = 0 ; i < nCol - 1; i++)
	{
		if (!pColNode) return NULL;
		pColNode = (TTabColNode*) pColNode->GetNext();
	}
	if (!pColNode) return NULL;
	return pColNode->m_Str;
}

int		KTabFileCtrl::FindRow(LPSTR szRow)
{
	TTabLineNode * pLineNode = (TTabLineNode *) m_RowList.GetHead();

	int  nRow = 1;
	while (pLineNode)
	{
		KList  * pList = pLineNode->pList;
		TTabColNode * pCol = (TTabColNode*)pList->GetHead();
		if (!pCol) return -1;
		if (!strcmp(pCol->m_Str, szRow))
		{
			return nRow;
		}
		pLineNode = (TTabLineNode *)pLineNode->GetNext();
		nRow ++;
	}
	return -1;
}
//查找某列 返回某列
int		KTabFileCtrl::FindColumn(LPSTR szColumn)
{
	TTabLineNode * pLine = (TTabLineNode*)m_RowList.GetHead();
	if (!pLine) return -1;

	TTabColNode* pColNode =(TTabColNode * )pLine->pList->GetHead();
		
	int nCol = 1;
	while (pColNode)
	{
		if (g_StrCmp(pColNode->m_Str, szColumn))
		{
			return nCol;
		}
		pColNode = (TTabColNode *)pColNode->GetNext();
		nCol ++;
	}
	return -1;
}

// 以0,0为起点  获取某行某列的值
BOOL	KTabFileCtrl::GetValue(int nRow, int nColumn, LPSTR& lpRString, DWORD dwSize)
{
	if ( nRow < 0 || nColumn < 0) return FALSE;
	TTabLineNode * pLineNode = (TTabLineNode*)m_RowList.GetHead();
	for (int i = 0; i < nRow; i++) 
	{
		if (!pLineNode) return FALSE;
		pLineNode = (TTabLineNode * )pLineNode->GetNext();
	}
	if (!pLineNode) return FALSE;
	TTabColNode * pColNode = (TTabColNode*)pLineNode->pList->GetHead();
	for (int j = 0; j < nColumn; j++)
	{
		if (!pColNode) return FALSE;
		pColNode = (TTabColNode*) pColNode->GetNext();
	}
	if (!pColNode)
		return FALSE;
	lpRString = pColNode->m_Str;
	return TRUE;
}   

//设置某行某列的值
BOOL	KTabFileCtrl::SetValue(int nRow, int nColumn, LPSTR lpString, DWORD dwSize, BOOL bCanCreateNew )
{
	if ( nRow < 0 || nColumn < 0) return FALSE;
	TTabLineNode * pLineNode = (TTabLineNode*)m_RowList.GetHead();
	TTabLineNode * pTempNode = pLineNode;
	for (int i = 0; i < nRow + 1; i++) 
	{
		//如果没有该行结点则自动生成
		pLineNode = pTempNode;	
		if (!pLineNode)
		{
			if (!bCanCreateNew) return FALSE;
			for(int j = 0; j < nRow + 1 - i; j++)
			{
				TTabLineNode * pLine = new TTabLineNode;
				KList * pList = new KList;
				pLine->pList = pList;
				m_RowList.AddTail(pLine);
			}
			pLineNode = (TTabLineNode*) m_RowList.GetTail();
			break;
		}
		
		pTempNode = (TTabLineNode * )pLineNode->GetNext();
	}
	
	KList * pColList =  pLineNode->pList;
	TTabColNode * pColNode = (TTabColNode*)pColList->GetHead();
	TTabColNode * pTempColNode = pColNode;

	for (int j = 0; j < nColumn + 1; j++)
	{
		pColNode = pTempColNode;
		if (!pColNode) 
		{
			if (!bCanCreateNew) return FALSE;
			for (int k = 0; k < nColumn +1 - j -1; k++)
			{
				TTabColNode * pNode = new TTabColNode;
				char * newStr = new char[2];
				strcpy(newStr,"");
				pNode->m_Str = newStr;
				pColList->AddTail(pNode);
			}
			
			TTabColNode * pNode = new TTabColNode;
			char * newStr = new char[dwSize + 1];
			g_StrCpyLen(newStr, lpString, dwSize + 1);
			pNode->m_Str = newStr;
			pColList->AddTail(pNode);
			return TRUE;
		}
		pTempColNode = (TTabColNode*) pColNode->GetNext();
	}

	 if (pColNode->m_Str)
	 {
		 delete pColNode->m_Str;
		 pColNode->m_Str=NULL;
	 }

	char * pNewStr = new char[dwSize + 1];
	g_StrCpyLen(pNewStr, lpString, dwSize + 1);
	pColNode->m_Str = pNewStr;
	return TRUE;
	
}

BOOL	KTabFileCtrl::GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize)
{
	char * pData = NULL;
	if (!GetValue(nRow - 1, nColumn - 1, pData, dwSize))
	{
		g_StrCpy(lpRString , lpDefault);
		return FALSE;
	}
	else
	{
	g_StrCpyLen(lpRString, pData, dwSize);
	}
	
	return TRUE;
}

BOOL		KTabFileCtrl::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	char * pData = NULL;
	if (!GetValue(nRow - 1, nColumn - 1, pData, 100))
	{
		*pnValue = nDefault;
		return FALSE;
	}
	else
		*pnValue = atoi(pData);

	return TRUE;
}

BOOL		KTabFileCtrl::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	char * pData = NULL;
	if (!GetValue(nRow -1 , nColumn -1, pData, 100))
	{
		*pfValue = fDefault;
		return FALSE;
	}
	else
		*pfValue = (float)atof(pData);
	
	return TRUE;
}
//清空
void  KTabFileCtrl::Clear()
{
	while(m_RowList.GetTail())
	{
		TTabLineNode * pLineNode = (TTabLineNode *)m_RowList.GetTail();

		KList * pList = pLineNode->pList;
		
		while(pList->GetHead())		
		{//从这一行 首列 开始释放移除
			TTabColNode * pNode = (TTabColNode*) pList->GetHead();
			if (pNode)
			{
				if (pNode->m_Str)
				{
					delete pNode->m_Str;
					pNode->m_Str=NULL;
				}
				pList->RemoveHead();
			}

		}

		if (pList)
		{
			delete pList;
			pList=NULL;
		}
		pLineNode->Remove();
	}
	isload = false;
	//Meme.Free();
	
}
//某行某列写字符串
BOOL  KTabFileCtrl::WriteString(int nRow, int nColumn,  LPSTR lpString, DWORD dwSize)
{
	if (dwSize <= 0) 
	 dwSize = g_StrLen(lpString);
	return SetValue(nRow -1 ,nColumn -1, lpString, dwSize);
}
//某行某列写整数字
BOOL  KTabFileCtrl::WriteInteger(int nRow, int nColumn, int nValue)
{
	char IntNum[256]={0};
	t_sprintf(IntNum, "%d", nValue);
	return SetValue(nRow -1, nColumn -1, IntNum, g_StrLen(IntNum));
}
//某行某列写小数点字
BOOL  KTabFileCtrl::WriteFloat(int nRow, int nColumn,float fValue)
{
	char FloatNum[256]={0};
	//sprintf(FloatNum,"%f",fValue);
#ifdef WIN32
	snprintf(FloatNum,sizeof(FloatNum)-1,"%f",fValue);
#else
	snprintf(FloatNum,sizeof(FloatNum),"%f",fValue);
#endif
	return SetValue(nRow - 1, nColumn - 1, FloatNum, g_StrLen(FloatNum));
}

int KTabFileCtrl::Str2Col(LPSTR szColumn)
{
	int	nStrLen = g_StrLen(szColumn);
	char	szTemp[4];
	
	g_StrCpy(szTemp, szColumn);
	g_StrUpper(szTemp);
	if (nStrLen == 1)
	{
		return (szTemp[0] - 'A');
	}
	return ((szTemp[0] - 'A' + 1) * 26 + szTemp[1] - 'A');
}

// 删除某行
BOOL KTabFileCtrl::Remove(int nRow)
{
	TTabLineNode * pNode = (TTabLineNode*)m_RowList.GetHead();
		
	if (!pNode) 
		return FALSE;
	
	for(int i  = 0; i < nRow  - 1; i++)
	{
		if (!pNode)
		   return FALSE;
		pNode = (TTabLineNode*) pNode->GetNext();
	}
	
	TTabLineNode *pDelNode = pNode;
	if (!pNode)
		return FALSE;

	pNode->Remove();

	if (pDelNode)
	{
		delete pDelNode;
		pDelNode=NULL;
	}
	return TRUE;
}
//行后加入一行
BOOL KTabFileCtrl::InsertAfter(int nRow)
{
	TTabLineNode * pNode = (TTabLineNode*)m_RowList.GetHead();
	if (!pNode) return FALSE;
	
	for(int i  = 0; i < nRow  - 1; i++)
	{
		if (!pNode) return FALSE;
		pNode = (TTabLineNode*) pNode->GetNext();
	}

	if (!pNode) return FALSE;
	
	TTabLineNode * pNewNode = new TTabLineNode;
	KList * pList = new KList;
	pNewNode->pList = pList;
	pNode->InsertAfter(pNewNode);
	return TRUE;
}
// 行前加入一行
BOOL KTabFileCtrl::InsertBefore(int nRow)
{
	TTabLineNode * pNode = (TTabLineNode*)m_RowList.GetHead();
	if (!pNode) return FALSE;
	
	for(int i  = 0; i < nRow  - 1; i++)
	{
		if (!pNode) return FALSE;
		pNode = (TTabLineNode*) pNode->GetNext();
	}
	if (!pNode) return FALSE;
	
	TTabLineNode * pNewNode = new TTabLineNode;
	KList * pList = new KList;
	pNewNode->pList = pList;
	pNode->InsertBefore(pNewNode);
	return TRUE;
}
//返回新生成Col的Col编号  加入新的一列
int	KTabFileCtrl::InsertNewCol(LPSTR strNewCol)
{
	if (!strNewCol || !strNewCol[0]) return -1;

	int nResult = -1;
	
	//如果已有就不用再加了
	if ((nResult = FindColumn(strNewCol)) > 0) 	return nResult;
	
	nResult = GetWidth() + 1;

	WriteString(1, nResult, strNewCol);
	return nResult;
}

