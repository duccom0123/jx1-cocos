//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.h
// Date:	2002.02.18
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KTabFile_H
#define KTabFile_H
#include "cocos2d.h"
#include "KMemClass.h"
#include "KITabFile.h"
//#include "KTabFileCtrl.h"
//---------------------------------------------------------------------------
typedef struct tagTabOffset
{
	unsigned long		dwOffset;
	unsigned long		dwLength;
} TABOFFSET;

class  KTabFile:public KITabFile
{
private:
	int			m_Width;
	int			m_Height;
	KMemClass	m_Memory;
	KMemClass	m_OffsetTable;
	unsigned long	dwSize;
	//unsigned long		dwFileIdx;
	//void *		Buffer;
	//char        nMemKey[32];
private:
	void		CreateTabOffset();
	BOOL		GetValue(int nRow, int nColumn, char * lpRString, unsigned long dwSize);
	int			Str2Col(char * szColumn);
public:
	KTabFile();
	~KTabFile();
//	KTabFileCtrl *TabFileCtrl;
	BOOL		Load(char * FileName/*,char * mMemKey=NULL*/);
	bool        CreatFile(char * FileName);
	BOOL		Save(char * FileName){return false;}; //无法保存
	BOOL		LoadPack( char * FileName);
	//unsigned long       GetFileDwIdx(){ return dwFileIdx;}

	unsigned long		GetSize(){ return dwSize;};
	int			FindRow( char * szRow);//返回以1为起点的值
	int			FindColumn( char * szColumn);//返回以1为起点的值
	void		Col2Str(int nCol,  char * szColumn);
	int			GetWidth() { return m_Width;};
	int			GetHeight() { return m_Height;};
	// int			GetWidth() { return 1;};
	// int			GetHeight() { return 1;};
	BOOL		GetString(int nRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize, BOOL bColumnLab = TRUE);
	BOOL		GetString(char * szRow, int szColumn, char * lpDefault, char * lpRString, unsigned long dwSize, BOOL bColumnLab = TRUE);
	BOOL		GetString(int nRow, int nColumn, char * lpDefault, char * lpRString, unsigned long dwSize);
	BOOL		GetString(char * szRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize);
	BOOL		GetInteger(int nRow, char * szColumn, int nDefault, int *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger6(int nRow, char * szColumna,char * szColumnb,char * szColumnc,char * szColumnd,char * szColumne,char * szColumnf,int nDefault, int *pnValue, BOOL bColumnLab= TRUE);
	BOOL		GetInteger3(int nRow, char * szColumna,char * szColumnb,char * szColumnc,int nDefault, int *pnValue, BOOL bColumnLab= TRUE);
	BOOL		GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
	BOOL        GetIntegerDW(int nRow, char * szColumn, int nDefault,unsigned long *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger(char * szRow, char * szColumn, int nDefault, int *pnValue);
	BOOL		GetFloat(int nRow, char * szColumn, float fDefault, float *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetDouble(int nRow, char * szColumn, double fDefault, double *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetDword(int nRow, char * szColumn, unsigned long fDefault, unsigned long *pfValue, BOOL bColumnLab = TRUE);
//	BOOL		GetFloat(char * szRow, char * szColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(char * szRow, char * szColumn, float fDefault, float *pfValue);
	void        GetInt2(int nRow, int szColumn, int *pRect);
	void        GetInt15(int nRow, int szColumn, int *pRect);
	void        GetInt15s(int nRow, char * szColumn, int *pRect);
	void        GetInt3(int nRow, int szColumn, int *pRect);
	void        GetInt2s(int nRow, char * szColumn, int *pRect);
	void        GetInt3s(int nRow, char * szColumn, int *pRect);
	void		Clear();
};
//---------------------------------------------------------------------------
#endif
