//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.cpp
// Date:	2002.02.20
// Code:	Huyi(Spe)
// Desc:	Tab File Operation Class
//---------------------------------------------------------------------------
#include "KStrBase.h"
#include "KFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#ifndef _SERVER
#include "KCodec.h"
#endif
#include "KTabFile.h"
#include <string.h>
//---------------------------------------------------------------------------
// 函数:	KTabFile
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::KTabFile()
{
    m_Width		= 0;
	m_Height	= 0;
	dwSize      = 0;
	//dwFileIdx   = 0;

	///ZeroMemory(nMemKey,sizeof(nMemKey));
}
//---------------------------------------------------------------------------
// 函数:	~KTabFile
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::~KTabFile()
{
	Clear();
}

bool  KTabFile::CreatFile(char * FileName)
{
	if  (!FileName) return false;
	KFile cFile;
	if (cFile.Create(FileName))  //创建一个文件
	{
		cFile.Close();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	加载一个Tab文件
// 参数:	FileName	文件名
// 返回:	true		成功
//			false		失败
//---------------------------------------------------------------------------
BOOL KTabFile::Load(char * FileName/*,char * mMemKey*/)
{
    
	KPakFile	File;
	//unsigned long		dwSize;
	void *		Buffer;

	// check file name
	if (FileName[0] == 0)
		return false;

	
	if (!File.Open(FileName))
	{
	//	printf("Can't open tab file : %s", FileName);
		return false;
	}
  
	dwSize = File.Size(); //文件的大小

	/*char nFileName[256]={0};
	sprintf(nFileName,FileName);
	g_StrLower(nFileName);
	dwFileIdx = g_FileName2Id(nFileName);*/

	if (dwSize<=0)
	{
		File.Close();
		return false;
	}

	//if  (mMemKey)
	//     Buffer = m_Memory.Alloc(dwSize,g_FileName2Id(mMemKey));  //分配内存  并设置内存标志
	//else
	Buffer = m_Memory.Alloc(dwSize);

	//sprintf(nMemKey,mMemKey);

	File.Read(Buffer, dwSize);

	CreateTabOffset();

	File.Close();
	return true;
}

//---------------------------------------------------------------------------
// 函数:	CreateTabOffset
// 功能:	建立制作表符分隔文件的偏移表
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::CreateTabOffset()
{
	int		nWidth, nHeight, nOffset, nSize;
	unsigned char	*Buffer;
	TABOFFSET *TabBuffer;

	nWidth	= 1;
	nHeight	= 1;
	nOffset = 0;

	Buffer	= (unsigned char *)m_Memory.GetMemPtr();	//获取内存指针
	nSize	= m_Memory.GetMemLen();

	// 读第一行决定有多少列
	while (*Buffer != 0x0d && *Buffer != 0x0a)
	{
		if (*Buffer == 0x09)
		{
			nWidth++;
		}
		Buffer++;
		nOffset++;
	}
	if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
	{
		Buffer += 2;	// 0x0a跳过		
		nOffset += 2;	// 0x0a跳过
	}
	else
	{
		Buffer += 1;	// 0x0a跳过		
		nOffset += 1;	// 0x0a跳过
	}
	while(nOffset < nSize)
	{
		while (*Buffer != 0x0d && *Buffer != 0x0a)
		{
			Buffer++;
			nOffset++;
			if (nOffset >= nSize)
				break;
		}
		nHeight++;
		if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a跳过		
			nOffset += 2;	// 0x0a跳过
		}
		else
		{
			Buffer += 1;	// 0x0a跳过		
			nOffset += 1;	// 0x0a跳过
		}
	}
	m_Width		= nWidth;
	m_Height	= nHeight;

	TabBuffer = (TABOFFSET *)m_OffsetTable.Alloc(m_Width * m_Height * sizeof (TABOFFSET));
	Buffer = (unsigned char *)m_Memory.GetMemPtr();

	nOffset = 0;
	int nLength;
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			TabBuffer->dwOffset = nOffset;	
			nLength = 0;
			while(*Buffer != 0x09 && *Buffer != 0x0d && *Buffer != 0x0a && nOffset < nSize)
			{
				Buffer++;
				nOffset++;
				nLength++;
			}
			Buffer++;	// 0x09或0x0d或0x0a(linux)跳过
			nOffset++;
			TabBuffer->dwLength = nLength;
			TabBuffer++;
			if (*(Buffer - 1) == 0x0a || *(Buffer - 1) == 0x0d)	//	本行已经结束了，虽然可能没到nWidth //for linux modified [wxb 2003-7-29]
			{
				for (int k = j+1; k < nWidth; ++k)
				{
					TabBuffer->dwOffset = nOffset;
					TabBuffer->dwLength = 0;
					TabBuffer++;					
				}
				break;
			}
		}

		//modified for linux [wxb 2003-7-29]
		if (*(Buffer - 1) == 0x0d && *Buffer == 0x0a)
		{
			Buffer++;				// 0x0a跳过	
			nOffset++;				// 0x0a跳过	
		}
	}
}

//---------------------------------------------------------------------------
// 函数:	Str2Column
// 功能:	取得某行某列字符串的值
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
int KTabFile::Str2Col(char * szColumn)
{
	int	nStrLen = g_StrLen(szColumn);
	char	szTemp[4];

	g_StrCpy(szTemp, szColumn);
	g_StrUpper(szTemp);

	//toupper(szTemp);
    // _strupr(szTemp);
	if (nStrLen == 1)
	{
		return (szTemp[0] - 'A');
	}
	return ((szTemp[0] - 'A' + 1) * 26 + szTemp[1] - 'A') + 1;
}

//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize, BOOL bColumnLab)
{
	int nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize); //默认值
	    return false;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(char * szRow, int szColumn, char * lpDefault, char * lpRString, unsigned long dwSize, BOOL bColumnLab)
{
	int nRow, nColumn;
	
	   nRow = FindRow(szRow);

	if (bColumnLab)
		nColumn = szColumn;
	else
		nColumn = szColumn;

	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return false;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行	（关键字）
//			szColomn		列	（关键字）
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(char * szRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize)
{
	int nRow, nColumn;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return false;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, int nColumn, char * lpDefault, char * lpRString, unsigned long dwSize)
{
	if (GetValue(nRow - 1, nColumn - 1,  lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return false;
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, char * szColumn, int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return false;
	}
}

//---------------------------------------------------------------------------
// 函数:	GetInteger6  6字段提取int字符
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger6(int nRow, char * szColumna,char * szColumnb,char * szColumnc,char * szColumnd,char * szColumne,char * szColumnf,  int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumna,nColumnb,nColumnc,nColumnd,nColumne,nColumnf;
	if (bColumnLab)
	{
		nColumna = FindColumn(szColumna);
		nColumnb = FindColumn(szColumnb);
		nColumnc = FindColumn(szColumnc);
		nColumnd = FindColumn(szColumnd);
		nColumne = FindColumn(szColumne);
		nColumnf = FindColumn(szColumnf);
	}
	else
	{
		nColumna = Str2Col(szColumna);
		nColumnb = Str2Col(szColumnb);
		nColumnc = Str2Col(szColumnc);
		nColumnd = Str2Col(szColumnd);
		nColumne = Str2Col(szColumne);
		nColumnf = Str2Col(szColumnf);
	}

	if (GetValue(nRow - 1, nColumna - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
    if(GetValue(nRow - 1, nColumnb - 1, Buffer, sizeof(Buffer)))
	{
        
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
    if(GetValue(nRow - 1, nColumnc - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumnd - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumne - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
     pnValue++;
	if(GetValue(nRow - 1, nColumnf - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}

	if (pnValue)
	{
      return true;
	}

	 return false;
}
//---------------------------------------------------------------------------
// 函数:	GetInteger3  3字段提取int字符
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger3(int nRow, char * szColumna,char * szColumnb,char * szColumnc,int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumna,nColumnb,nColumnc;
	if (bColumnLab)
	{
		nColumna = FindColumn(szColumna);
		nColumnb = FindColumn(szColumnb);
		nColumnc = FindColumn(szColumnc);
	}
	else
	{
		nColumna = Str2Col(szColumna);
		nColumnb = Str2Col(szColumnb);
		nColumnc = Str2Col(szColumnc);
	}
	
	if (GetValue(nRow - 1, nColumna - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
	pnValue++;
    if(GetValue(nRow - 1, nColumnb - 1, Buffer, sizeof(Buffer)))
	{
        
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
	pnValue++;
    if(GetValue(nRow - 1, nColumnc - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		//return true;
	}
	else
	{
		*pnValue = nDefault;
	}
	
	
	if (pnValue)
	{
	   return true;
	}
	   return false;
}




//同个字段分割符号 |
//void KTabFile::GetInt2(LPCSTR lpSection, LPCSTR lpKeyName, int* pRect)

void KTabFile::GetInt2(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];

	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d", &pRect[0], &pRect[1]);
	}
}

void KTabFile::GetInt15(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];
	
	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", &pRect[0],&pRect[1],&pRect[2],&pRect[3],&pRect[4],&pRect[5],&pRect[6],&pRect[7],&pRect[8],&pRect[9],&pRect[10],&pRect[11],&pRect[12],&pRect[13],&pRect[14]);
	}
}

void KTabFile::GetInt15s(int nRow, char * szColumn, int *pRect)
{
	char  Buffer[256];
	
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", &pRect[0],&pRect[1],&pRect[2],&pRect[3],&pRect[4],&pRect[5],&pRect[6],&pRect[7],&pRect[8],&pRect[9],&pRect[10],&pRect[11],&pRect[12],&pRect[13],&pRect[14]);
	}
}

void KTabFile::GetInt3(int nRow, int szColumn, int *pRect)
{
	char  Buffer[256];
	
	if (GetValue(nRow - 1, szColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d", &pRect[0], &pRect[1], &pRect[2]);
	}
}
//------字符串类-----
void KTabFile::GetInt2s(int nRow, char * szColumn, int *pRect)
{
	char  Buffer[256];
 
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d", &pRect[0], &pRect[1]);
	}
}

void KTabFile::GetInt3s(int nRow, char * szColumn, int *pRect)
{
	char  Buffer[256];
	int	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		sscanf(Buffer, "%d|%d|%d", &pRect[0], &pRect[1], &pRect[2]);
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(char * szRow, char * szColumn, int nDefault, int *pnValue)
{
	int		nRow, nColumn;
	char	Buffer[64];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	char	Buffer[64]={0};

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return true;
	}
}

BOOL KTabFile::GetIntegerDW(int nRow, char * szColumn, int nDefault,unsigned long *pnValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return false;
	}
}

BOOL KTabFile::GetDword(int nRow, char * szColumn, unsigned long fDefault, unsigned long *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (unsigned long)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}


//---------------------------------------------------------------------------
// 函数:	GetDouble
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetDouble(int nRow, char * szColumn, double fDefault, double *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (double)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}

//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, char * szColumn, float fDefault, float *pfValue, BOOL bColumnLab)
{
	char	Buffer[64];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(char * szRow, char * szColumn, float fDefault, float *pfValue)
{
	int		nRow, nColumn;
	char	Buffer[64];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	char	Buffer[64];
	
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetValue
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetValue(int nRow, int nColumn, char * lpRString, unsigned long dwSize)
{
	if (nRow >= m_Height || nColumn >= m_Width || nRow < 0 || nColumn < 0)
		return false;

	TABOFFSET	*TempOffset;
	char *		Buffer;

	Buffer = (char *)m_Memory.GetMemPtr();
	TempOffset = (TABOFFSET *)m_OffsetTable.GetMemPtr();
	TempOffset += nRow * m_Width + nColumn;

	//ZeroMemory(lpRString, dwSize);
	memset(lpRString, 0, dwSize);
	Buffer += TempOffset->dwOffset;
	if (TempOffset->dwLength == 0)
	{
		TempOffset=NULL;
		return false;
	}
	if (dwSize > TempOffset->dwLength)
	{
		memcpy(lpRString, Buffer, TempOffset->dwLength);
		lpRString[TempOffset->dwLength] ='\0';
	}
	else
	{
		memcpy(lpRString, Buffer, dwSize);
		lpRString[dwSize] = '\0';
	}
    TempOffset=NULL;
	return true;
}
//---------------------------------------------------------------------------
// 函数:	Clear
// 功能:	清除TAB文件的内容
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::Clear()
{
/*	if (m_Memory)
	{
		g_MemFree(m_Memory);
		m_Memory = NULL;
	}
	*/
	//ZeroMemory(nMemKey,sizeof(nMemKey));
	//dwFileIdx = 0;
	m_OffsetTable.Free();
	m_Memory.Free();
}
//---------------------------------------------------------------------------
// 函数:	FindRow
// 功能:	查找行关键字
// 参数:	szRow（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindRow(char * szRow)
{
	char	szTemp[128];
	for (int i = 0; i < m_Height; ++i)	// 从1开始，跳过第一行的字段行
	{
		GetValue(i, 0, szTemp, g_StrLen(szRow));
		if (g_StrCmp(szTemp, szRow))
			return i + 1; //改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}
//---------------------------------------------------------------------------
// 函数:	FindColumn
// 功能:	查找列关键字
// 参数:	szColumn（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindColumn(char * szColumn)
{
	char	szTemp[128];
	for (int i = 0; i < m_Width; ++i)	// 从1开始，跳过第一列的字段行
	{
		GetValue(0, i, szTemp, g_StrLen(szColumn));
		if (g_StrCmp(szTemp, szColumn))
			return i + 1;//改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}

//---------------------------------------------------------------------------
// 函数:	Col2Str
// 功能:	把整数转成字符串
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
void KTabFile::Col2Str(int nCol, char * szColumn)
{

	if (nCol < 26)
	{
		szColumn[0] = 'A' + nCol;
		szColumn[1]	= 0;
	}
	else
	{
		szColumn[0] = 'A' + (nCol / 26 - 1);
		szColumn[1] = 'A' + nCol % 26;
		szColumn[2] = 0;
	}
}
