//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KCodec_H
#define KCodec_H
#include "cocos2d.h"

USING_NS_CC;
//---------------------------------------------------------------------------
#define CODEC_NONE		0
#define CODEC_LHA		1
#define CODEC_LZO		2
//---------------------------------------------------------------------------
typedef struct {
	unsigned char *		lpData;			// 待压缩&压缩前 数据指针
	unsigned long		dwDataLen;		// 待压缩&压缩前 数据长度
	unsigned char *	    lpPack;			// 压缩后&压缩后 数据指针
	unsigned long		dwPackLen;		// 压缩后&压缩后 数据长度
} TCodeInfo;
//---------------------------------------------------------------------------
class  KCodec
{
public:
	 unsigned long	GetPackLen(unsigned long dwDataLen);
	 bool	Encode(TCodeInfo* pCodeInfo);
	 bool	Decode(TCodeInfo* pCodeInfo);
	//lpData;dwDataLen;lpPack;必须正确设置，dwPackLen不为0即可	
};
//---------------------------------------------------------------------------
 void	g_InitCodec(KCodec** ppCodec, int nCompressMethod);
 void	g_FreeCodec(KCodec** ppCodec, int nCompressMethod);
//---------------------------------------------------------------------------
#endif
