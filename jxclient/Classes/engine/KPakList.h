//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
// Modify:	Wooy(2003-9-17)
//---------------------------------------------------------------------------
#ifndef KPakList_H
#define KPakList_H
#include "cocos2d.h"

USING_NS_CC;
#include "KbugInfo.h"
#include "XPackFile.h"
//---------------------------------------------------------------------------


class KPakList
{
public:
	KPakList();
	~KPakList();
	int		Open(char* pPakListFile,int nKind=0);
	void		Close();
	//查找包内的子文件
	bool		pFindElemFileA(unsigned long uId, XPackElemFileRef& ElemRef);
	//查找包内的子文件
	bool		pFindElemFile(char* pszFileName, XPackElemFileRef& ElemRef);
	//读取包内的子文件
	int			ElemFileRead(XPackElemFileRef& ElemRef, void* pBuffer, unsigned uSize);
	//读取spr文件头部或整个spr
	SPRHEAD*	GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable);
	//读取按帧压缩的spr的一帧的数据
	SPRFRAME*	GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame,unsigned int &nSingFrameSize);

	bool		pGetFilePath(char* pszFileName, XPackElemFileRef& ElemRef);
	

private:
	unsigned long FileNameToId(const char* pszFileName);
private:
	#define MAX_PAK		50   //加载PAK的数量限制
	XPackFile*			m_PakFilePtrList[MAX_PAK];
	long				m_nPakNumber;
};

extern KPakList* g_pPakList;

#endif
