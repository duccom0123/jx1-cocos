/*****************************************************************************************
//	文字串处理
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-31
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "KStrBase.h"
//#include <string>
#ifndef __linux
#include <crtdbg.h>
#else
#define HRESULT	long
#define S_OK 0
#endif

//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#define min(a,b)            (((a) < (b)) ? (a) : (b))

#include "Text.h"
#define		MAX_ENCODED_CTRL_LEN	4	//单个控制符（控制标识+控制参数）的最大存储长度


IInlinePicEngineSink* g_pIInlinePicSink = NULL;	//嵌入式图片的处理接口[wxb 2003-6-19]
extern "C"  long 
AdviseEngine(IInlinePicEngineSink* pSink)
{
	//_ASSERT(NULL == g_pIInlinePicSink);	//一般不会挂接两次

	if  (g_pIInlinePicSink)
		return 0;

	g_pIInlinePicSink = pSink;
	return 1;
}

extern "C"  long 
UnAdviseEngine(IInlinePicEngineSink* pSink)
{
	if (pSink == g_pIInlinePicSink)
		g_pIInlinePicSink = NULL;
	return 1;
}

//检测某个字符是否为不许放置行首的字符
#define	NUM_CHARACTER_IN_A1	11
unsigned char	s_NotAllowAtLineHeadA1Characters[NUM_CHARACTER_IN_A1] = 
{
	 //、。’〕〉》」』〗】
	0xa2, 0xa3, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf
};
#define	NUM_CHARACTER_IN_A3	10
unsigned char	s_NotAllowAtLineHeadA3Characters[NUM_CHARACTER_IN_A3] = 
{
	//！   ）    ，   ．     ：    ；   ＞    ？    ］    ｝
	0xa1, 0xa9, 0xac, 0xae, 0xba, 0xbb, 0xbe, 0xbf, 0xdd, 0xfd
};//0x20
#define	NUM_CHARACTER_IN_00	7
unsigned char	s_NotAllowAtLineHead00Characters[NUM_CHARACTER_IN_00] =
{
	//!),.:;>?
	0x21, 0x29, 0x2c, /*0x2e,*/ 0x3a, 0x3b, 0x3e, 0x3f
};

extern "C" 
void TReplaceText(char* pBuffer, const char* pszName1, const char* pszName2)
{
	int nMsgLen=strlen(pBuffer);
	int nMsgLen1=strlen(pszName1);
	int nMsgLen2=strlen(pszName2);
	int i=0,j=0,pst=0;
	while(pBuffer[i])
	{
		pst=i;
		j=0;
		while (pszName1[j] && pBuffer[pst]==pszName1[j]) {pst++;j++;}
		if (j==nMsgLen1)
		{
			memmove(pBuffer+i+nMsgLen2,pBuffer+i+nMsgLen1,nMsgLen-i-nMsgLen1+1);
			if (nMsgLen2<nMsgLen1)
			{
				memcpy(pBuffer+i,pszName2,nMsgLen2);
				nMsgLen-=nMsgLen1-nMsgLen2;
				i+= nMsgLen2;
			}
			else
			{
				memcpy(pBuffer+i,pszName2,nMsgLen2);
				nMsgLen+=nMsgLen2-nMsgLen1;
				i+=nMsgLen2;
			}
		}
		else i++;
	}
}


extern "C" 
int EGetBit(int nIntValue,int nBitNumber)
{
	int nBitValue = 0;
	
	if (nBitNumber >= 32 || nBitNumber <= 0) 
		return nIntValue;

	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;

	return nBitValue;
}

extern "C" 
int ESetBit(int nIntValue,int nBitNumber,int nBitValue)
{

	nBitValue = (nBitValue == 1);
	
	if (nBitNumber > 32 || nBitNumber <= 0) //32位
		return nIntValue;
	
	nIntValue = (nIntValue | (1 << (nBitNumber - 1)));

	return nIntValue;
}

extern "C" 
int FilterTextColor(char* pMsgBuff, unsigned short nMsgLength)
{
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_ENTER);          //禁止回车代码
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR);          //禁止颜色代码
	//nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR_RESTORE);  //禁止颜色复原
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_COLOR);   //禁止描边设置
	//nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_RESTORE); //禁止描边设置
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_INLINE_PIC);     //禁止内图设置
	//nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_LINK);           //禁止连接设置
	//	return TEncodeText(pMsgBuff, nMsgLength);
	return  nMsgLength;
}

extern "C" 
int  ESetByte(int nIntValue,int nByteNumber,int nByteValue)
{
	unsigned char * pByte =	NULL;
	
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;
	
	pByte = (unsigned char*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (unsigned char)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}
extern "C" 
int  EGetByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		return nByteValue;
	
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}

//转换成字符串 颜色
extern "C" 
const char* TGetColorStr(unsigned int nColor)
{
	static char szColor[12];
	KERColor c;
	c.Color_dw = nColor;
	t_sprintf(szColor, "%d,%d,%d", c.Color_b.r, c.Color_b.g, c.Color_b.b);
	szColor[11] = 0;
	
	return szColor;
}

extern "C" 
unsigned int TGetColor(const char* pColor)
{
	if (pColor == NULL)
		return false;
	
	unsigned int Color = 0xFF000000;
	
	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pColor[i] != ',')
	{
		if (pColor[i] == 0 || n >= 15)
			return atoi(pColor);
		Buf[n++] = pColor[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pColor[i] != ',')
	{
		
		if (pColor[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pColor[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pColor[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pColor[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}


//检测某个字符是否为不许放置行首的字符，不是限制字符则返回0，否则返回字符占的子节数
extern "C" 
int TIsCharacterNotAlowAtLineHead(const char* pCharacter)
{
	int				i;
	unsigned char	cChar;
	cChar = (unsigned char)(*pCharacter);
	if (cChar == 0xa3)
	{
		cChar = (unsigned char)pCharacter[1];
		if (cChar >= 0xa1 && cChar <= 0xfd)
		{
			for (i = 0; i < NUM_CHARACTER_IN_A3; ++i)
				if (s_NotAllowAtLineHeadA3Characters[i] == cChar)
					return 2;
		}
	}
	else if (cChar == 0xa1)
	{
		cChar = (unsigned char)pCharacter[1];
		if (cChar >= 0xa2 && cChar <= 0xbf)
		{
			for (i = 0; i < NUM_CHARACTER_IN_A1; ++i)
				if (s_NotAllowAtLineHeadA1Characters[i] == cChar)
					return 2;
		}
	}
	else if (cChar >= 0x21 && cChar <= 0x3f)
	{
		for (i = 0; i < NUM_CHARACTER_IN_00; ++i)
			if (s_NotAllowAtLineHead00Characters[i] == cChar)
				return 1;
	}
	return false;
}

//获取本行的下个显示字符
extern "C" 
const char* TGetSecondVisibleCharacterThisLine(const char* pCharacter, int nPos, int nLen)
{
	if (pCharacter && nLen > 0)
	{
		bool bFoundFirst = false;
		while(nPos < nLen)
		{
			unsigned char cChar = (unsigned char)(pCharacter[nPos]);
			if (cChar >= 0x20)
			{
				if (bFoundFirst)
					return (pCharacter + nPos);
				bFoundFirst = true;
				if (cChar > 0x80)
					nPos += 2;
				else
					nPos ++;
				continue;
			}
			if (cChar == KTC_COLOR || cChar == KTC_BORDER_COLOR)
				nPos += 4;
			else if (cChar == KTC_INLINE_PIC)
				nPos += 1 + sizeof(unsigned short);
			else if (cChar == KTC_COLOR_RESTORE || cChar == KTC_BORDER_RESTORE)
				nPos++;
			break;
		};
	}
	return NULL;
}


//--------------------------------------------------------------------------
//	功能：寻找分割字符串的合适位置
//	参数：pString    --> 想要分割的字符串
//		　nDesirePos --> 期望分割的位置（以字节为单位）
//		  bLess      --> 如果期望分割的位置处于一个字符编码的中间时，结果位置
//					为前靠还是后靠，0: 向后靠; 非0: 向前靠。
//	注释：Chinese GBK编码版本，此字符串中字符全部视为显示字符，不包含控制字符
//--------------------------------------------------------------------------
extern "C" 
int TSplitString(const char* pString, int nDesirePos, int bLess)
{
	register int	nPos = 0;
	if (pString)
	{
		nDesirePos -= 2;
		while(nPos < nDesirePos)
		{
			if ((unsigned char)pString[nPos] > 0x80) //中文
				nPos += 2;
			else if (pString[nPos])
				nPos++;
			else
				break;
		};
		nDesirePos += 2;
		while(nPos < nDesirePos)
		{
			if ((unsigned char)pString[nPos] > 0x80)
			{
				if (bLess && (nPos + 2 > nDesirePos))
					break;
				if (pString[nPos + 1] == 0)
				{//防止出现单BYTE的高于0x80的字符
					nPos ++;
					break;
				}				
				nPos += 2;
			}
			else if (pString[nPos])
				nPos ++;
			else
				break;
		}
	}
	return nPos;
}

//--------------------------------------------------------------------------
//	功能：在编码字串寻找分割字符串的合适位置
//	参数：pString    --> 想要分割的字符串
//		  nCount	 -->字符串内容的长度（以字节为单位）
//		　nDesirePos --> 期望分割的位置（以缓冲驱存储字节为单位）
//		  bLess      --> 如果期望分割的位置处于一个中文字符编码的中间时，
//						结果位置为前靠还是后靠，0: 向后靠; 非0: 向前靠。
//	注释：Chinese GBK编码版本，此字符串中可包含已经编码的控制符
//--------------------------------------------------------------------------
extern "C" 
int	TSplitEncodedString(const char* pString, int nCount, int nDesirePos, int bLess)
{
	int	nPos = 0;
	if (pString)
	{
		if (nDesirePos <= nCount)
		{
			register unsigned char cCharacter;
			nDesirePos -= MAX_ENCODED_CTRL_LEN;
			while (nPos < nDesirePos)
			{
				cCharacter = (unsigned char)pString[nPos];
				if (cCharacter > 0x80)
					nPos += 2;
				else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
					nPos += 4;
				else if (cCharacter == KTC_INLINE_PIC)
					nPos += 1 + sizeof(unsigned short);
				else
					nPos ++;
			}
			nPos += MAX_ENCODED_CTRL_LEN;
			while(nPos < nDesirePos)
			{
				cCharacter = (unsigned char)pString[nPos];
				if (cCharacter > 0x80)
				{
					if (bLess && (nPos + 2 > nDesirePos))
						break;
					if (nPos + 1 == nCount)
					{//防止出现单BYTE的高于0x80的字符
						nPos = nCount;
						break;
					}
					nPos += 2;
				}				
				else if (cCharacter == KTC_INLINE_PIC)
				{
					if (bLess && nPos + 2 > nDesirePos)
						break;
					if (nPos + 2 >= nCount)
					{
						nPos = nCount;
						break;
					}
					nPos += 2;//1 + sizeof(unsigned short);
				}
				else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
				{
					if (bLess && (nPos + 4 > nDesirePos))
						break;
					if (nPos + 4 >= nCount)
					{
						nPos = nCount;
						break;
					}
					nPos += 4;
				}
				else
					nPos ++;
			}

		}
		else
		{
			nPos = nCount;
		}
	}
	return nPos;
	
}


//字符换控制码的字符标记表示与内部编码的对应结构
#define	KTC_CTRL_CODE_MAX_LEN	255//7  127
typedef struct _KCtrlTable
{
	char    szCtrl[KTC_CTRL_CODE_MAX_LEN + 1];	//字符换控制码的字符表示
	short	nCtrlLen;							//字符换控制码的字符表示的长度
	short   nCtrl;								//字符换控制码的内部编码
}KCtrlTable;

//控制码列表
static	const KCtrlTable	s_CtrlTable[] =
{	
{ "enter",	5, KTC_ENTER		},
{ "color",	5, KTC_COLOR		},
{ "bclr",	4, KTC_BORDER_COLOR	},
{ "pic",	3, KTC_INLINE_PIC	},
{ "link",	4, KTC_LINK	},

};
/*
//控制码列表
static	const KCtrlTable	s_CtrlTable[] =
{	
{ "enter",	5, KTC_ENTER		},
{ "color",	5, KTC_COLOR		},
{ "bclr",	4, KTC_BORDER_COLOR	},
{ "pic",	3, KTC_INLINE_PIC	},
};
*/
//控制码的数目
static	const int	s_nCtrlCount = sizeof(s_CtrlTable)/sizeof(KCtrlTable);

//颜色结构
typedef struct _KColorTable
{
	char			Token[8];		//颜色的字符表示
	unsigned char	Red;			//颜色的R分量
	unsigned char	Green;			//颜色的G分量
	unsigned char	Blue;			//颜色的B分量
}KColorTable;

//颜色列表
static	const KColorTable	s_ColorTable[] =
{
	{ "Black",	0,		0,		0	},
	{ "White",	255,	255,	255	},
	{ "Red",	255,	0,		0	},
	{ "Green",	0,		255,	0	},
	{ "Blue",	100,	100,	255 },
	{ "Yellow",	234,	189,	11  },
	{ "Pink",	255,	0,		255	},
	{ "Cyan",	0,		255,	255	},
	{ "Metal",	246,	255,	117	},
	{ "Wood",	0,		255,	120	},
	{ "Water",	78,		124,	255	},
	{ "Fire",	255,	90,		0	},
	{ "Earth",	254,	207,	179	},
	{ "DBlue",	120,	120,	120 },
	{ "HBlue",	100,	100,	255 },
	{ "DYellow",110,	110,	0   },     //{ "DYellow",155,	155,	120 },	255,217,78
	{ "HYellow",234,	189,	11  },
	{ "Purple", 170,	 30,	255 },
	{ "DGreen",   0,	 75,	   0},
	{ "HGreen",   0,    175,	   0},
	{ "Orange", 249,     81,       0},
	{ "GYellow", 255,     255,    0},
};

/*    黄金色  :243,194,90		234,	189,	11		 245,205,46
R=234
G=189
B=11

*/

//颜色的数目
static	const int	s_nColorCount = sizeof(s_ColorTable)/sizeof(KColorTable);

//嵌入图片[wxb 2003-6-19]
#define MAXPICTOKENLEN	16


static bool TEncodeCtrl(char* pBuffer, int nCount, int& nReadPos, int& nShortCount);
static int  TEncodeCtrlc(int nCtrl, char* pParamBuffer, int nParamLen, char* pEncodedBuffer);




extern "C"  
	int	_TEncodeText(char* pBuffer, int nCount)
{
	int nShortCount = 0;
	if (pBuffer)
	{
		unsigned char	cCharacter;
		int		nReadPos = 0;		
		while(nReadPos < nCount)
		{
			cCharacter = pBuffer[nReadPos]; //字节
			if (cCharacter > 0x80) //中文
			{
				if (nReadPos + 2 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 2];
					nReadPos += 3;
				}
				else	//大于0x80的单字节西文字符被过滤掉
					break;
			}
			else if (cCharacter == 0x0d)	//换行
			{
				if (nReadPos + 1 < nCount && pBuffer[nReadPos + 1] == 0x0a)
				{
					nReadPos += 2;
				}
				else
					nReadPos ++;
				pBuffer[nShortCount++] = 0x0a;  //换行加回车
			}

			else if (pBuffer[nReadPos] == '<')  // 如这个符号果有
			{
				//CCMessageBox("find < suss","find");
				TEncodeCtrl(pBuffer, nCount, nReadPos, nShortCount);
			}

			else if((cCharacter >= 0x20 && cCharacter < 0x7F) ||
				cCharacter == 0x0a || cCharacter == 0x09)
			{
				pBuffer[nShortCount++] = cCharacter;
				nReadPos++;
			}
			else
				nReadPos++;
		}
		if (nShortCount <nCount)
			pBuffer[nShortCount] = 0;
	}

	return nShortCount;  //返回长度
}

//	功能：对文本串中的控制标记进行转换，去除无效字符，缩短文本串存储长度
//--------------------------------------------------------------------------
extern "C"  
int	TEncodeText_(char* pBuffer, int nCount)
{
	int nShortCount = 0;
	if (pBuffer)
	{
		unsigned char	cCharacter;
		int		nReadPos = 0;		
		while(nReadPos < nCount)
		{
			cCharacter = pBuffer[nReadPos]; //字节
			if (cCharacter > 0x80)          //中文
			{
				if (nReadPos + 1 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					nReadPos += 2;
				}
				else	                   //大于0x80的单字节西文字符被过滤掉
					break;
			}
			else if (cCharacter == 0x0d)   //换行
			{
				if (nReadPos + 1 < nCount && pBuffer[nReadPos + 1] == 0x0a)
				{
					nReadPos += 2;
				}
				else
					nReadPos ++;
				pBuffer[nShortCount++] = 0x0a;  //换行加回车
			}
			
			else if (pBuffer[nReadPos] == '<')  // 如这个符号果有
			{
				//CCMessageBox("find < suss","find");
				TEncodeCtrl(pBuffer, nCount, nReadPos, nShortCount);
			}

			else if((cCharacter >= 0x20 && cCharacter < 0x7F) ||
				cCharacter == 0x0a || cCharacter == 0x09)
			{
				pBuffer[nShortCount++] = cCharacter;
				nReadPos++;
			}
			else
				nReadPos++;
		}
		if (nShortCount <nCount)
			pBuffer[nShortCount] = 0;
	}

	return nShortCount;  //返回长度
}
//--------------------------------------------------------------------------
//	功能：对文本串中的控制标记进行转换，去除无效字符，缩短文本串存储长度
//--------------------------------------------------------------------------
extern "C"  
int	TEncodeText(char* pBuffer, int nCount)
{
	int nShortCount = 0;
#ifdef WIN32
	if (pBuffer)
	{
		unsigned char	cCharacter;
		int		nReadPos = 0;		
		while(nReadPos < nCount)
		{/*
		 if (c<=127) i+=0;
		 else if ((c & 0xE0) == 0xC0) i+=1;
		 else if ((c & 0xF0) == 0xE0) i+=2;
		 else if ((c & 0xF8) == 0xF0) i+=3;
		 */
			cCharacter = pBuffer[nReadPos]; //字节
			if (cCharacter > 0x80) //中文
			{
				if (nReadPos + 1 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					nReadPos += 2;
				}
				else	//大于0x80的单字节西文字符被过滤掉
					break;
			}
			else if (cCharacter == 0x0d)	//换行
			{
				if (nReadPos + 1 < nCount && pBuffer[nReadPos + 1] == 0x0a)
				{
					nReadPos += 2;
				}
				else
					nReadPos ++;
				pBuffer[nShortCount++] = 0x0a;  //换行加回车
			}
			
			else if (pBuffer[nReadPos] == '<')  // 如这个符号果有
			{
				//CCMessageBox("find < suss","find");
				TEncodeCtrl(pBuffer, nCount, nReadPos, nShortCount);
			}

			else if((cCharacter >= 0x20 && cCharacter < 0x7F) ||
				cCharacter == 0x0a || cCharacter == 0x09)
			{
				pBuffer[nShortCount++] = cCharacter;
				nReadPos++;
			}
			else
				nReadPos++;
		}
		if (nShortCount <nCount)
			pBuffer[nShortCount] = 0;
	}
#else
	if (pBuffer)
	{
		unsigned char	cCharacter;
		int		nReadPos = 0;		
		while(nReadPos < nCount)
		{
			cCharacter = pBuffer[nReadPos]; //字节

			if (cCharacter > 0x80) //中文
			{

				/*if (nReadPos + 1 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					nReadPos += 2;
				}
				else	//大于0x80的单字节西文字符被过滤掉
					break;*/

				if (nReadPos + 2 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 2];
					nReadPos += 3;
				}
				else	//大于0x80的单字节西文字符被过滤掉
					break;
			}
			else if (cCharacter == 0x0d)	//换行
			{
				if (nReadPos + 1 < nCount && pBuffer[nReadPos + 1] == 0x0a)
				{
					nReadPos += 2;
				}
				else
					nReadPos ++;

				pBuffer[nShortCount++] = 0x0a;
			}
			else if (pBuffer[nReadPos] == '<')  // 如这个符号果有
			{
				//CCMessageBox("find < suss","find");
				TEncodeCtrl(pBuffer, nCount, nReadPos, nShortCount);
			}
			else if((cCharacter >= 0x20 && cCharacter < 0x7F) ||
				cCharacter == 0x0a || cCharacter == 0x09)
			{
				pBuffer[nShortCount++] = cCharacter;
				nReadPos++;
			}
			else
				nReadPos++;
		}
		if (nShortCount <nCount)
			pBuffer[nShortCount] = 0;
	}
#endif

	return nShortCount;  //返回长度
}

//--------------------------------------------------------------------------
//	功能：对文本串中的控制标记进行转换，去除无效字符，缩短文本串存储长度
//--------------------------------------------------------------------------
extern "C" 
int TFilterEncodedText(char* pBuffer, int nCount)
{
	int nShortCount = 0;
	if (pBuffer)
	{
		unsigned char	cCharacter;
		int nReadPos = 0;
		while(nReadPos < nCount)
		{
			cCharacter = pBuffer[nReadPos];
			if (cCharacter > 0x80)
			{
				if (nReadPos + 1 < nCount)	//是可能是中文文字
				{
					pBuffer[nShortCount++] = cCharacter;
					pBuffer[nShortCount++] = pBuffer[nReadPos+ 1];
					nReadPos += 2;
				}
				else	//大于0x80的单字节西文字符被过滤掉
				{
					nReadPos++;
					break;
				}
			}
			else if ((cCharacter >= 0x20 && cCharacter < 0x7F) ||
				cCharacter == 0x0a || cCharacter == 0x09)
			{
				pBuffer[nShortCount++] = cCharacter;
				nReadPos++;
			}
			else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
			{
				if (nReadPos + 4 < nCount)
				{
					*(int*)(pBuffer + nShortCount) = *(int*)(pBuffer + nReadPos);
					nShortCount += 4;
					nReadPos += 4;
				}
				else
				{
					nReadPos++;
					break;
				}
			}
			else if (cCharacter == KTC_INLINE_PIC)
			{
				if ((int)(nReadPos + 1 + sizeof(unsigned short)) < nCount)   //
				{
					memcpy(pBuffer + nShortCount, pBuffer + nReadPos, 1 + sizeof(unsigned short));;
					nShortCount += 1 + sizeof(unsigned short);
					nReadPos += 1 + sizeof(unsigned short);
				}
				else
				{
					nReadPos++;
					break;
				}
			}
			else
				nReadPos ++;
		}
		if (nShortCount < nCount)
			pBuffer[nShortCount] = 0;
	}

	return nShortCount;
}

//--------------------------------------------------------------------------
//	功能：分析转换控制符
//--------------------------------------------------------------------------
static bool TEncodeCtrl(char* pBuffer, int nCount, int& nReadPos, int& nShortCount)
{

	//_ASSERT(pBuffer != NULL && nReadPos < nCount && nShortCount < nCount && nShortCount <= nReadPos);
	if  (pBuffer==NULL || nReadPos>= nCount || nShortCount>=nCount || nShortCount>nReadPos)
	    return false;
	
	int nCtrlCodeSize, nEndPos, m_Ctrl;

	//寻找结束符号'='的位置或'>'的位置
	int nEqualPos = nReadPos + 1;
	for (; nEqualPos < nCount && nEqualPos <= nReadPos + KTC_CTRL_CODE_MAX_LEN; nEqualPos++)
		if (pBuffer[nEqualPos] == '>' || pBuffer[nEqualPos] == '=')  //找到 结束符号
			break;	//找到就跳出

	if(nEqualPos >= nCount || nEqualPos > nReadPos + KTC_CTRL_CODE_MAX_LEN)
		goto NO_MATCHING_CTRL;	//未找到'='或者'>'结束符号

	nCtrlCodeSize = nEqualPos - nReadPos - 1;	//找到后 两个控制命令符号之间字符的长度

	for (m_Ctrl = 0; m_Ctrl < s_nCtrlCount; m_Ctrl++) //s_nCtrlCount 控制符号的种类数量
	{//和记录的控制符号的长度相同的
		if (nCtrlCodeSize ==  s_CtrlTable[m_Ctrl].nCtrlLen && 
			memcmp(pBuffer + nReadPos + 1, s_CtrlTable[m_Ctrl].szCtrl, nCtrlCodeSize) == 0)
			break;
	}

	if (m_Ctrl >= s_nCtrlCount)		//未找到匹配一致的控制命令
		goto NO_MATCHING_CTRL;

	m_Ctrl = s_CtrlTable[m_Ctrl].nCtrl; //赋值给这个
	nEndPos = nEqualPos;

	if (pBuffer[nEqualPos] != '>')
	{//如果找到的符号 不等于 > 而是 =号 继续找
		for(nEndPos++; nEndPos < nCount; nEndPos++)
			if (pBuffer[nEndPos] == '>')
				break;
		if (nEndPos >= nCount)  //如果找不到
			goto NO_MATCHING_CTRL;
		//找到了 >符号 去掉 两个 <> 中间的内容
		nShortCount += TEncodeCtrlc(m_Ctrl, pBuffer + nEqualPos + 1,
			                        nEndPos - nEqualPos - 1, pBuffer + nShortCount);
	}
	else//如果找的是 > 符号 去掉 两个 <> 中间的内容
		nShortCount += TEncodeCtrlc(m_Ctrl,NULL,0,pBuffer + nShortCount);

	nReadPos = nEndPos + 1;
	return true;

NO_MATCHING_CTRL:
	pBuffer[nShortCount++] = '<'; //替换掉这个符号
	nReadPos++;
	return false;
}

//--------------------------------------------------------------------------
//	功能：转换并存储控制命令及各控制参数
//--------------------------------------------------------------------------
static int TEncodeCtrlc(int nCtrl, char* pParamBuffer, int nParamLen, char* pEncodedBuffer)
{
	//_ASSERT(pEncodedBuffer && (nParamLen == 0 || pParamBuffer != NULL));
	//if (pEncodedBuffer==NULL || (pParamBuffer==NULL && nParamLen!=0))
		//return 0;
	if (pEncodedBuffer==NULL || pParamBuffer==NULL || nParamLen!=0)
		return 0;
	//if (pEncodedBuffer==NULL)
	//	return 0;
	/*if (pEncodedBuffer && (nParamLen == 0 || pParamBuffer!= NULL))
	{
	}
	else
		return 0;*/
	
	int nEncodedSize = 0;
	static char	Color[8];
	static char	szPic[MAXPICTOKENLEN];
	//static char	szPicPath[128];
	switch(nCtrl)
	{
	case KTC_ENTER:
		pEncodedBuffer[nEncodedSize ++] = nCtrl;
		break;
	case KTC_INLINE_PIC:	//[wxb 2003-6-19]
		if (nParamLen == 0 && nParamLen >= MAXPICTOKENLEN)
			break;
		{
			memcpy(szPic, pParamBuffer, nParamLen);
			szPic[nParamLen] = 0;
			pEncodedBuffer[nEncodedSize] = KTC_INLINE_PIC;
			*((unsigned short*)(pEncodedBuffer + nEncodedSize + 1)) = atoi(szPic); //char 转 int
			nEncodedSize += 1 + sizeof(unsigned short);
		}		
		break;
	case KTC_LINK:
		{
			//printf("---检测到 link 标签 ----\n");

			if (nParamLen == 0 && nParamLen >= 128)
				break;
			{
				//memcpy(szPicPath, pParamBuffer, nParamLen);
				//szPicPath[nParamLen] = 0;
				pEncodedBuffer[nEncodedSize] = KTC_LINK;
				//unsigned short nLen = sizeof(szPicPath); 
				*((unsigned short*)(pEncodedBuffer + nEncodedSize + 1)) = nParamLen; //记录这个这个长度
				nEncodedSize += 1 + sizeof(unsigned short);
				memcpy(pEncodedBuffer + nEncodedSize + 1, pParamBuffer, nParamLen);
				nEncodedSize += 1 + nParamLen;
			}		
		    break; 
		}
		break;
	case KTC_COLOR:
		if (nParamLen == 0)
		{
			pEncodedBuffer[nEncodedSize ++] = KTC_COLOR_RESTORE;
		}
		else if (nParamLen < 8)
		{
			memcpy(Color, pParamBuffer, nParamLen);
			Color[nParamLen] = 0;
			for (int i = 0; i < s_nColorCount; ++i)
			{
#ifndef __linux
				if (stricmp(Color,s_ColorTable[i].Token) == 0)
#else
				if(strcasecmp(Color,s_ColorTable[i].Token) == 0)
#endif
				{
					pEncodedBuffer[nEncodedSize] = KTC_COLOR;
					pEncodedBuffer[nEncodedSize + 1]= s_ColorTable[i].Red;
					pEncodedBuffer[nEncodedSize + 2]= s_ColorTable[i].Green;
					pEncodedBuffer[nEncodedSize + 3]= s_ColorTable[i].Blue;
					nEncodedSize += 4;
					break;
				}
			}
		}		
		break;
	case KTC_BORDER_COLOR:
		if (nParamLen == 0)
		{
			pEncodedBuffer[nEncodedSize ++] = KTC_BORDER_RESTORE;
		}
		else if (nParamLen < 8)
		{
			memcpy(Color, pParamBuffer, nParamLen);
			Color[nParamLen] = 0;
			for (int i = 0; i < s_nColorCount; ++i)
			{
#ifndef __linux
				if (stricmp(Color,s_ColorTable[i].Token) == 0)
#else
				if(strcasecmp(Color,s_ColorTable[i].Token) == 0)
#endif
//				if (stricmp(Color,s_ColorTable[i].Token) == 0)
				{
					pEncodedBuffer[nEncodedSize] = KTC_BORDER_COLOR;
					pEncodedBuffer[nEncodedSize + 1]= s_ColorTable[i].Red;
					pEncodedBuffer[nEncodedSize + 2]= s_ColorTable[i].Green;
					pEncodedBuffer[nEncodedSize + 3]= s_ColorTable[i].Blue;
					nEncodedSize += 4;
					break;
				}
			}
		}
		break;
	}
	return nEncodedSize;
}

extern "C" 
int	TRemoveCtrlInEncodedText(char* pBuffer, int nCount)
{
	int nLen = 0;
	nCount = TFilterEncodedText(pBuffer, nCount);
	for (int nPos = 0; nPos < nCount; nPos++)
	{
		char cCharacter = pBuffer[nPos];
		if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
			nPos += 3;
		else if (cCharacter == KTC_INLINE_PIC)
			nPos += sizeof(unsigned short);
		else if (cCharacter != KTC_COLOR_RESTORE && cCharacter != KTC_BORDER_RESTORE)
		{
			pBuffer[nLen] = cCharacter;
			nLen ++;
		}
	}
	return nLen;
}

//获取编码文本的行数与最大行宽
//参数：pBuffer			文本缓冲区
//		nCount			文本数据的长度
//		nWrapCharaNum	限制每行不许超过的字符数目
//		nMaxLineLen		用于获取文本的实际最大行宽（字符数目）
//		nFontSize		采用字体的大小 [wxb 2003-6-19]
//		nSkipLine		跳过前面多少行的数据
//		nNumLineLimit	检测的文本的行数，超过限制行数目之后的内容被忽略。如果此值小于等于0则表示无此限制。
//返回：文本的行数
//extern "C" 
//int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine = 0, int nNumLineLimit = 0)
extern "C" 
int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine, int nNumLineLimit,
							 int bPicSingleLine/* = FALSE*/)
//int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine = 0, int nLineLimit = 0, int bPicSingleLine = false);
{
	//设一个极值免得出错 [wxb 2003-6-20]
	//_ASSERT(nFontSize >= 4 && nFontSize < 64);
	if (nFontSize<=1 || nFontSize>=64)
	    return 0;

	nFontSize = max(4, nFontSize);
	nFontSize = min(64, nFontSize);
	float fMaxLineLen = 0;
	nMaxLineLen = 0;
	if (pBuffer == 0)
		return 0;

	if (nCount < 0)
		nCount = strlen(pBuffer);

	float fNumChars = 0;
	int nNumLine = 0;
	int nPos = 0;
	unsigned char	cCode;

	if (nWrapCharaNum <= 0)
		nWrapCharaNum = 0x7fffffff; //每行字符数无限制
	if (nSkipLine < 0)
		nSkipLine = 0;
	if (nNumLineLimit <= 0)
		nNumLineLimit = 0x7fffffff; //行数无限制

	bool bNextLine = false;
	float fNumNextLineChar = 0;
	int  nExtraLineForInlinePic = 0;
	while(nPos < nCount)
	{
		cCode = pBuffer[nPos];
		if (cCode > 0x80)	//可能是中文字符
		{
			nPos += 2;
			if (fNumChars + 2 < nWrapCharaNum)
				fNumChars += 2;
			else if (fNumChars + 2 == nWrapCharaNum || fNumChars == 0)
			{
				bNextLine = true;
				fNumChars += 2;
			}
			else
			{
				bNextLine = true;
				fNumNextLineChar = 2;
			}
		}
		else if (cCode == KTC_COLOR || cCode == KTC_BORDER_COLOR)//颜色控制
			nPos += 4;
		else if (cCode == KTC_INLINE_PIC)
		{
			//嵌入式图片处理 [wxb 2003-6-19]
			unsigned short wPicIndex = *((unsigned short*)(pBuffer + nPos + 1));
			nPos += 1 + sizeof(unsigned short);
			if (g_pIInlinePicSink)
			{
				int nWidth, nHeight;
				if (g_pIInlinePicSink->GetPicSize(wPicIndex, nWidth, nHeight))
				{
					if (nHeight > nFontSize)
					{
						int nExtraLines = nHeight - nFontSize;
						nExtraLines = nExtraLines / nFontSize + ((nExtraLines % nFontSize) ? 1 : 0);
						if (nExtraLines > nExtraLineForInlinePic && !bPicSingleLine)
							nExtraLineForInlinePic = nExtraLines;
					}
					if (fNumChars + (((float)nWidth) * 2 / nFontSize) < nWrapCharaNum)
						fNumChars += ((float)nWidth) * 2 / nFontSize;
					else if (fNumChars + (((float)nWidth) * 2 / nFontSize) == nWrapCharaNum || fNumChars == 0)
					{
						bNextLine = true;
						fNumChars += ((float)nWidth) * 2 / nFontSize;
					}
					else
					{
						bNextLine = true;
						fNumNextLineChar = ((float)nWidth) * 2 / nFontSize;
					}
				}
			}
		}
		else if (cCode == KTC_ENTER)
		{
			nPos ++;
			bNextLine = true;
		}
		else if (cCode != KTC_COLOR_RESTORE && cCode != KTC_BORDER_RESTORE)
		{
			nPos ++;
			fNumChars += 1;
			if (fNumChars >= nWrapCharaNum)
			{
				bNextLine = true;
			}
		}
		else
		{
			nPos++;
		}

		if (bNextLine == false && fNumChars && fNumChars + 3 >= nWrapCharaNum)
		{
			const char* pNext = TGetSecondVisibleCharacterThisLine(pBuffer, nPos, nCount);
			if (pNext && TIsCharacterNotAlowAtLineHead(pNext))
				bNextLine = true;
		}
		if (bNextLine) //下一行
		{
			if (nSkipLine > 0)
			{
				nSkipLine -= 1 + nExtraLineForInlinePic;

				//处理图片占多行的情况 [wxb 2003-6-19]
				if (nSkipLine < 0)
				{
					if (fMaxLineLen < fNumChars)
						fMaxLineLen = fNumChars;
					nNumLine += (-nSkipLine);
					if (nNumLine >= nNumLineLimit)
						break;
				}
			}
			else
			{
				if (fMaxLineLen < fNumChars)
					fMaxLineLen = fNumChars;
				nNumLine += 1 + nExtraLineForInlinePic;
				if (nNumLine >= nNumLineLimit)
					break;
			}
			nExtraLineForInlinePic = 0;
			fNumChars = (float)fNumNextLineChar;
			fNumNextLineChar = 0;
			bNextLine = false;
		}
	}
	if (nNumLine < nNumLineLimit && fNumChars && nSkipLine == 0)
	{
		if (fMaxLineLen < fNumChars)
			fMaxLineLen = fNumChars;
		nNumLine += 1 + nExtraLineForInlinePic;
	}

	nMaxLineLen = (int)(fMaxLineLen + (float)0.9999);	//进1
	return nNumLine;
}

//获得指定行的开始位置
int TGetEncodeStringLineHeadPos(const char* pBuffer, int nCount, int nLine, int nWrapCharaNum, int nFontSize, int bPicSingleLine)
{
	//设一个极值免得出错 [wxb 2003-6-20]
	//_ASSERT(nFontSize > 1 && nFontSize < 64);
	if (nFontSize<=1 || nFontSize>=64)
		return 0;

	nFontSize = max(1, nFontSize);
	nFontSize = min(64, nFontSize);

	float fMaxLineLen = 0;
	if (pBuffer == 0 || nLine <= 0)
		return 0;

	if (nCount < 0)
		nCount = strlen(pBuffer);

	float fNumChars = 0;
	int  nExtraLineForInlinePic = 0;
	int nPos = 0;
	unsigned char	cCode;

	if (nWrapCharaNum <= 0)
		nWrapCharaNum = 0x7fffffff;

	bool bNextLine = false;
	float fNumNextLineChar = 0;
	while(nPos < nCount)
	{
		cCode = pBuffer[nPos];
		if (cCode > 0x80)	//可能是中文字符
		{
			nPos += 2;
			if (fNumChars + 2 < nWrapCharaNum)
				fNumChars += 2;
			else if (fNumChars + 2 == nWrapCharaNum || fNumChars == 0)
			{
				bNextLine = true;
				fNumChars += 2;
			}
			else
			{
				bNextLine = true;
				fNumNextLineChar = 2;
			}
		}
		else if (cCode == KTC_COLOR || cCode == KTC_BORDER_COLOR)//颜色控制
			nPos += 4;
		else if (cCode == KTC_INLINE_PIC)
		{
			//嵌入式图片处理 [wxb 2003-6-19]
			unsigned short wPicIndex = *((unsigned short*)(pBuffer + nPos + 1));
			nPos += 1 + sizeof(unsigned short);
			if (g_pIInlinePicSink)
			{
				int nWidth, nHeight;
				if (g_pIInlinePicSink->GetPicSize(wPicIndex, nWidth, nHeight))
				{
					if (nHeight > nFontSize)
					{
						int nExtraLines = nHeight - nFontSize;
						nExtraLines = nExtraLines / nFontSize + ((nExtraLines % nFontSize) ? 1 : 0);
						if (nExtraLines > nExtraLineForInlinePic && !bPicSingleLine)
							nExtraLineForInlinePic = nExtraLines;
					}
					if (fNumChars + (((float)nWidth) * 2 / nFontSize) < nWrapCharaNum)
						fNumChars += ((float)nWidth) * 2 / nFontSize;
					else if (fNumChars + (((float)nWidth) * 2 / nFontSize) == nWrapCharaNum || fNumChars == 0)
					{
						bNextLine = true;
						fNumChars += ((float)nWidth) * 2 / nFontSize;
					}
					else
					{
						bNextLine = true;
						fNumNextLineChar = ((float)nWidth) * 2 / nFontSize;
					}
				}
			}
		}
		else if (cCode == KTC_ENTER)
		{
			nPos ++;
			bNextLine = true;
		}
		else if (cCode != KTC_COLOR_RESTORE && cCode != KTC_BORDER_RESTORE)
		{
			nPos ++;
			fNumChars += 1;
			if (fNumChars >= nWrapCharaNum)
			{
				bNextLine = true;
			}
		}
		else
		{
			nPos++;
		}

		if (bNextLine == false && fNumChars && fNumChars + 3 >= nWrapCharaNum)
		{
			const char* pNext = TGetSecondVisibleCharacterThisLine(pBuffer, nPos, nCount);
			if (pNext && TIsCharacterNotAlowAtLineHead(pNext))
				bNextLine = true;
		}
		if (bNextLine)
		{
//			if (nSkipLine > 0)
//			{
//				nSkipLine -= 1 + nExtraLineForInlinePic;
//
//				//处理图片占多行的情况 [wxb 2003-6-19]
//				if (nSkipLine < 0)
//				{
//					if (fMaxLineLen < fNumChars)
//						fMaxLineLen = fNumChars;
//					nNumLine += (-nSkipLine);
//					if (nNumLine >= nNumLineLimit)
//						break;
//				}
//			}
			if ((--nLine) == 0)
				break;
			fNumChars = (float)fNumNextLineChar;
			fNumNextLineChar = 0;
			bNextLine = false;
		}
	}

	return nPos;
}

extern "C" 
//如果原(包含控制符)字符串长度（包括结尾符）超过限定的长度，则截短它并加上..后缀
const char* TGetLimitLenEncodedString(const char* pOrigString, int nOrigLen, int nFontSize,
	int nWrapCharaNum, char* pLimitLenString, int& nShortLen, int nLineLimit, int bPicPackInSingleLine/*=false*/)
{
	if (pOrigString == NULL || pLimitLenString == NULL ||
		nOrigLen <= 0 || nShortLen < 2 || nLineLimit < 1 || nWrapCharaNum < 2)
	{
		nShortLen = 0;
		return NULL;
	}

	int nPreLineEndPos = 0, nFinalLineEndPos;
	if (nLineLimit > 1)	//跳过前面几行
	{
		nPreLineEndPos = TGetEncodeStringLineHeadPos(pOrigString, nOrigLen, nLineLimit - 1, nWrapCharaNum, nFontSize, bPicPackInSingleLine);
		if (nPreLineEndPos > nShortLen)
		{
			nShortLen = TSplitEncodedString(pOrigString, nOrigLen, nShortLen - 2, true);
			memcpy(pLimitLenString, pOrigString, nShortLen);
			pLimitLenString[nShortLen] = '.';
			pLimitLenString[nShortLen + 1] = '.';
			nShortLen += 2;
			return pLimitLenString;
		}
	}

	if (nPreLineEndPos < nOrigLen)
	{
		nFinalLineEndPos = TGetEncodeStringLineHeadPos(pOrigString + nPreLineEndPos,
			nOrigLen - nPreLineEndPos, 1, nWrapCharaNum, nFontSize, bPicPackInSingleLine) + nPreLineEndPos;
	}
	else
		nFinalLineEndPos = nOrigLen;

	if (nFinalLineEndPos >= nOrigLen)
	{
		nShortLen = TSplitEncodedString(pOrigString, nOrigLen, nShortLen, true);
		memcpy(pLimitLenString, pOrigString, nShortLen);
		return pLimitLenString;
	}

	int nDesireLen = (nFinalLineEndPos <= nShortLen) ? nFinalLineEndPos - 2 : nShortLen - 2;

	const char* pFinalLineHead = pOrigString + nPreLineEndPos;
	int nRemainCount = nOrigLen - nPreLineEndPos;
	nDesireLen -= nPreLineEndPos;
	while(true)
	{
		nShortLen = TSplitEncodedString(pFinalLineHead, nRemainCount, nDesireLen, true);
		int nMaxLineLen;
		TGetEncodedTextLineCount(pFinalLineHead, nShortLen, 0, nMaxLineLen, nFontSize, 0, 1, false);
		if (nMaxLineLen <= nWrapCharaNum - 2)
			break;
		nDesireLen --;
	}
	nShortLen += nPreLineEndPos;

   	memcpy(pLimitLenString, pOrigString, nShortLen);
	pLimitLenString[nShortLen] = '.';
	pLimitLenString[nShortLen + 1] = '.';
	nShortLen += 2;
	return pLimitLenString;
}

//--------------------------------------------------------------------------
//	功能：如果原字符串长度（包括结尾符）超过限定的长度，则截短它并加上..后缀
//	参数：pOrigString     --> 原字符串，要求不为空指针
//		　nOrigLen		  --> 原字符串长度（不包括结尾符）
//		  pLimitLenString --> 如果原字符串超出限长，用来存储截短后的字符串的缓冲区，要求不为空指针
//		  nLimitLen		  --> 限定长度，此值要求大于等于3
//	返回：如原字符串不超过限长，则返回原缓冲区指针，否则返回用来存储截短后的字符串的缓冲区的指针
//	注释：Chinese GBK编码版本，此字符串中字符全部视为显示字符，不包含控制字符
//--------------------------------------------------------------------------
extern "C" 
const char* TGetLimitLenString(const char* pOrigString, int nOrigLen, char* pLimitLenString, int nLimitLen)
{
	if (pOrigString && pLimitLenString && nLimitLen > 0)
	{
		if (nOrigLen < 0)
			nOrigLen = strlen(pOrigString);
		if (nOrigLen < nLimitLen)
			return pOrigString;
		if (nLimitLen > 2)
		{
			nOrigLen = TSplitString(pOrigString, nLimitLen - 3, true);
			memcpy(pLimitLenString, pOrigString, nOrigLen);
			pLimitLenString[nOrigLen] = '.';
			pLimitLenString[nOrigLen + 1] = '.';
			pLimitLenString[nOrigLen + 2] = 0;
		}
		else if (nLimitLen == 2)
		{
			pLimitLenString[0] = '.';
			pLimitLenString[1] = 0;
		}
		return ((nLimitLen >= 2) ? pLimitLenString : NULL);
	}
	return NULL;
}


//对已经编码的文本，从指定位置开始查找指定的控制符号的位置，返回-1表示未找到
extern "C" 
int	TFindSpecialCtrlInEncodedText(const char* pBuffer, int nCount, int nStartPos, char cControl)
{
	int nFindPos = -1;
	if (pBuffer)
	{
		while(nStartPos < nCount)
		{
			unsigned char cCharacter = pBuffer[nStartPos];
			if ((unsigned char)cControl == cCharacter)
			{
				nFindPos = nStartPos;
				break;
			}
			if (cCharacter > 0x80)	//可能是中文文字
				nStartPos += 2;
			else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
				nStartPos += 4;
			else if (cCharacter == KTC_INLINE_PIC)
				nStartPos += 3;
			else
				nStartPos ++;
		}
	}
	return nFindPos;
}

//对已经编码的文本，去除指定类型的控制符

/*
{ "enter",	5, KTC_ENTER		},
{ "color",	5, KTC_COLOR		},
{ "bclr",	4, KTC_BORDER_COLOR	},
{ "pic",	3, KTC_INLINE_PIC	},
{ "link",	4, KTC_LINK	},
*/
extern "C" 
int	TClearSpecialCtrlInEncodedText(char* pBuffer, int nCount, char cControl)
{
	int nFinalLen = 0;
	int nReadPos = 0;
#ifdef WIN32
	if (pBuffer)
	{
		if ((unsigned char)(cControl) <= 0x80)
		{
			int nMatchLen = 1;
			if (cControl == KTC_COLOR || cControl == KTC_BORDER_COLOR)
				nMatchLen = 4;
			else if (cControl == KTC_INLINE_PIC)
				nMatchLen = 3;
			//else if (cControl == KTC_ENTER)
			//	nMatchLen = 5;

			while(nReadPos < nCount)
			{
				unsigned char cCharacter = pBuffer[nReadPos];
				if ((unsigned char)cControl == cCharacter)
				{
					nReadPos += nMatchLen;
				}
				else if (cCharacter > 0x80)  //中文
				{
					short sTemp = *(short*)(pBuffer + nReadPos);
					*(short*)(pBuffer + nFinalLen) = sTemp;
					nReadPos += 2;
					nFinalLen += 2;
				}
				
				else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
				{
					int nTemp = *(int*)(pBuffer + nReadPos);
					*(int*)(pBuffer + nFinalLen) = nTemp;
					nFinalLen += 4;
					nReadPos += 4;
				}
				else if (cCharacter == KTC_INLINE_PIC)
				{
					memmove((pBuffer + nFinalLen), (pBuffer + nReadPos), 3);
					nFinalLen += 3;
					nReadPos += 3;
				}
				else
				{
					pBuffer[nFinalLen++] = pBuffer[nReadPos++];
				}
			}
		}
	}
#else
	if (pBuffer)
	{
		if ((unsigned char)(cControl) <= 0x80)
		{//如果控制符不是中文
			int nMatchLen = 1;
			if (cControl == KTC_COLOR || cControl == KTC_BORDER_COLOR)
				nMatchLen = 4;
			else if (cControl == KTC_INLINE_PIC)
				nMatchLen = 3;

			while(nReadPos < nCount)
			{
				unsigned char cCharacter = pBuffer[nReadPos];
				if ((unsigned char)cControl == cCharacter)
				{
					nReadPos += nMatchLen;
				}
				else if (cCharacter > 0x80)  //中文
				{
					short sTemp = *(short*)(pBuffer + nReadPos);
					*(short*)(pBuffer + nFinalLen) = sTemp;
					nReadPos += 2;
					nFinalLen += 2;
				}
				else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
				{
					//CCMessageBox("color char","color char");
					int nTemp = *(int*)(pBuffer + nReadPos);
					*(int*)(pBuffer + nFinalLen) = nTemp;
					nFinalLen += 4;
					nReadPos += 4;
				}
				else if (cCharacter == KTC_INLINE_PIC)
				{
					memmove((pBuffer + nFinalLen), (pBuffer + nReadPos), 3);
					nFinalLen += 3;
					nReadPos += 3;
				}
				else
				{
					pBuffer[nFinalLen++] = pBuffer[nReadPos++];
				}
			}
		}
	}
#endif
	return nFinalLen;
}
//对已经编码的文本，指定输出长度的在缓冲区中位置
extern "C" 
int TGetEncodedTextOutputLenPos(const char* pBuffer, int nCount, int& nLen, bool bLess, int nFontSize)
{
	int nIndex = 0, nLenTemp = 0;

	//_ASSERT(nFontSize >= 4);
	if (nFontSize < 4)
		return nIndex;

	nFontSize = max(4, nFontSize);

    if (pBuffer)
	{
		int nWidth, nHeight;
		int nByteCount = 0, nCurCharLen = 0;
	    unsigned char cCharacter        = 0;

		while(nLenTemp < nLen)
		{
			cCharacter = pBuffer[nIndex];
			//计算出当前元素的所占字节数nByteCount和在显示画面上所占宽度nCurCharLen
			if (cCharacter > 0x80)	//可能是中文文字
			{
                nByteCount  = 2;
				nCurCharLen = 2;
			}
			else if (cCharacter == KTC_COLOR || cCharacter == KTC_BORDER_COLOR)
			{
			    nByteCount  = 4;
				nCurCharLen = 0;
			}
			else if (cCharacter == KTC_COLOR_RESTORE && cCharacter == KTC_BORDER_RESTORE)
			{
				nByteCount  = 1;
				nCurCharLen = 0;
			}
			else if (cCharacter == KTC_INLINE_PIC)
			{
				nByteCount  = 3;
				if(g_pIInlinePicSink->GetPicSize(
					*((unsigned short *)(pBuffer + nIndex + 1)), nWidth, nHeight))
				{
					nCurCharLen = ((nWidth * 2 + nFontSize - 1) /  nFontSize);
				}
				else
					nCurCharLen = 0;
			}
			else
			{
				nByteCount  = 1;
				nCurCharLen = 1;
			}

			//如果超出缓冲区，就停止吧
			if(nIndex + nByteCount > nCount)
				break;
			//如果宽度还没超过要求宽度
			if(nLenTemp + nCurCharLen < nLen)
			{
				nLenTemp += nCurCharLen;
		        nIndex   += nByteCount;
			}
			//如果宽度等于要求宽度了
			else if(nLenTemp + nCurCharLen == nLen)
			{
				nLenTemp += nCurCharLen;
				nIndex   += nByteCount;
				break;
			}
			//这里就是超过了
			else
			{
				nLenTemp = bLess ? nLenTemp : (nLenTemp + nCurCharLen);
				nIndex   = bLess ? nIndex   : (nIndex + nByteCount);
				break;
			}
		}
	}
	nLen = nLenTemp;
	return nIndex;
}

//对已经编码的文本，指定的前段缓冲区中控制符，对后面的输出产生效果影响
extern "C" 
int TGetEncodedTextEffectCtrls(const char* pBuffer, int nSkipCount, KTP_CTRL& Ctrl0, KTP_CTRL& Ctrl1)
{
	int nIndex = 0;
	Ctrl0.cCtrl = Ctrl1.cCtrl = KTC_INVALID;
	if (pBuffer)
	{
		KTP_CTRL PreCtrl0, PreCtrl1;
		PreCtrl0.cCtrl = PreCtrl1.cCtrl = KTC_INVALID;

		while(nIndex < nSkipCount)
		{
			unsigned char cCharacter = pBuffer[nIndex];
			if (cCharacter == KTC_COLOR)
			{
				PreCtrl0  =  Ctrl0;
				*(int*)(&Ctrl0) = *(int*)(pBuffer + nIndex);
				nIndex += 4;				
			}
			else if (cCharacter == KTC_BORDER_COLOR)
			{
				PreCtrl1  =  Ctrl1;
				*(int*)(&Ctrl1) = *(int*)(pBuffer + nIndex);
				nIndex += 4;
			}
			else if(cCharacter == KTC_COLOR_RESTORE)
			{
				Ctrl0 = PreCtrl0;
				nIndex ++;
			}
			else if(cCharacter == KTC_BORDER_RESTORE)
			{
				Ctrl1 = PreCtrl1;
				nIndex ++;
			}
			else
				nIndex ++;
		}
	}
	return nIndex;
}
