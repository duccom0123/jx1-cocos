//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KFile.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KFile_H
#define KFile_H
#include "cocos2d.h"

USING_NS_CC;
//---------------------------------------------------------------------------
#define SEEK_ERROR		0xFFFFFFFF
//---------------------------------------------------------------------------
class KFile
{
private:
//#ifndef __linux
//	HANDLE		m_hFile;	// File Handle
//#else
	FILE *		m_hFile;	// File Handle
//#endif
	unsigned long		m_dwLen;	// File Size
	unsigned long		m_dwPos;	// File Pointer
public:
	KFile();
	~KFile();
	bool		Open(char *FileName);
	bool		Create(char *FileName);
	bool		Append(char * FileName);
	void		Close();
	unsigned long Read(void *lpBuffer,unsigned long dwReadBytes);
	unsigned long Write(void * lpBuffer, unsigned long dwWriteBytes);
	unsigned long Seek(long lDistance, unsigned long dwMoveMethod);
	unsigned long Tell();
	unsigned long Size();
	FILE * getFilePtr(){return m_hFile;};
};
//---------------------------------------------------------------------------
#endif
