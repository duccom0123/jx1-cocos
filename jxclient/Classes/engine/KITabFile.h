#ifndef __KITABFILE_H__
#define __KITABFILE_H__
#include "KEngine.h"

class  KITabFile//TabFile½Ó¿ÚÀà
{
public:
	KITabFile(){};
	virtual  ~KITabFile() {};

	virtual BOOL		Load(char * FileName/*,char * mMemKey=NULL*/) = 0;
	virtual BOOL		Save(char * FileName) = 0;
	virtual bool        CreatFile(char * FileName)=0;
	virtual int			FindRow(char * szRow) = 0;
	virtual int			FindColumn(char * szColumn)	=0;
	virtual int			GetWidth() 	=0;
	virtual int			GetHeight() =0;
	virtual BOOL		GetString(int nRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize, BOOL bColumnLab = true)	=0;
	virtual BOOL		GetString(int nRow, int nColumn, char * lpDefault, char * lpRString, unsigned long dwSize)	=0;
	virtual BOOL		GetString(char * szRow, char * szColumn, char * lpDefault, char * lpRString, unsigned long dwSize)	=0;
	virtual BOOL		GetInteger(int nRow, char * szColumn, int nDefault, int *pnValue, BOOL bColumnLab = true)	=0;
	virtual BOOL		GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)	=0;
	virtual BOOL		GetInteger(char * szRow, char * szColumn, int nDefault, int *pnValue)	=0;
	virtual BOOL		GetFloat(int nRow, char * szColumn, float fDefault, float *pfValue, BOOL bColumnLab = true)	=0;
	virtual BOOL		GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)	=0;
	virtual BOOL		GetFloat(char * szRow, char * szColumn, float fDefault, float *pfValue)	=0;
//	virtual void        GetInt2(int nRow, int szColumn, int *pRect)=0;
//	virtual	void        GetInt3(int nRow, int szColumn, int *pRect)=0;
//	virtual	void        GetInt2(int nRow, char * szColumn, int *pRect)=0;
//	virtual	void        GetInt3(int nRow, char * szColumn, int *pRect)=0;
	virtual void		Clear()	=0;


};

#endif //__KITABFILE_H__
