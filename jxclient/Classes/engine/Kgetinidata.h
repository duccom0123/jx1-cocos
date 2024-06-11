//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCodec.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Kgetinidata_H
#define Kgetinidata_H
#include "cocos2d.h"
#include "KbugInfo.h"
#include "ImageStore/KImageStore2.h"
//#include "KPalette.h"
USING_NS_CC;
//---------------------------------------------------------------------------
//#define CODEC_NONE	0
//#define CODEC_LHA		1
//#define CODEC_LZO		2
//---------------------------------------------------------------------------
typedef struct {
	unsigned char *		lpData;			// 待压缩&压缩前 数据指针
	unsigned long		dwDataLen;		// 待压缩&压缩前 数据长度
	unsigned char *	    lpPack;			// 压缩后&压缩后 数据指针
	unsigned long		dwPackLen;		// 压缩后&压缩后 数据长度
}TSprCodeInfo;

struct Palette_Colour
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
};

typedef struct {
	long		x;			// 裁减后的X坐标
	long		y;			// 裁减后的Y坐标
	long		width;		// 裁减后的宽度
	long		height;		// 裁减后的高度
	long		left;		// 左边界裁剪量　
	long		top;		// 上边界裁剪量
	long		right;		// 右边界裁剪量
} KClipper;
//---------------------------------------------------------------------------
class  Kgetinidata
{
public:
	Kgetinidata(void);
	virtual ~Kgetinidata(){};
	 unsigned long	GetLen(unsigned long dwDataLen);
	// bool	SprEncode(TSprCodeInfo* pCodeInfo);
	// bool	SprDecode(TSprCodeInfo* pCodeInfo);
	 CCTexture2D * getinidata_new(char *nFileName,int nFrameIndex,int *mCurwidth, int *mCurheight,int *mFrams);
	 CCTexture2D * getinidata_one(char *nFileName,int nFrameIndex,int *mCurwidth, int *mCurheight,int *mFrams,SPRFRAMSINFO *nSprInfo=NULL);
	 CCTexture2D * getpicPath(char *picPath);
	 //lpData;dwDataLen;lpPack;必须正确设置，dwPackLen不为0即可
	 //bool   SaveBMPFile(const char *filename,  const stImageInfo& m_info);
private:
	
	SPRHEAD     * pHeader;
	BYTE*	 	m_Palette;
	KPAL16*		m_pPal16;
	SPROFFS* 	m_pOffset;
	SPRFRAME*   nFrameData;
	PBYTE		m_pSprite;
	int			m_nWidth;
	int			m_nHeight;
	int	        m_nCenterX;
	int	        m_nCenterY;
	signed  int	m_nOffsetX;
	signed	int	m_nOffsetY;
	unsigned long	m_nFrames;
	int			m_nColors;
	unsigned long	m_nDirections;
	int			m_nInterval;
	unsigned char *data;
	unsigned long m_size;
	int          m_ReadModel;
	long  GetGreaterNear2Fold(long d);
	PBYTE RevertRowRGBA(PBYTE data, long &width, long &height);
	bool ReadFileData(char *nFileName,int nFrameIndex=0);
};
//---------------------------------------------------------------------------
// void	g_InitCodec(KCodec** ppCodec, int nCompressMethod);
// void	g_FreeCodec(KCodec** ppCodec, int nCompressMethod);
//---------------------------------------------------------------------------
#endif
