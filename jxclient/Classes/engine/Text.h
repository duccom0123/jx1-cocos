/*****************************************************************************************
//	文字串处理
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-31
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef TEXT_H
#define TEXT_H
#include "cocos2d.h"
using namespace cocos2d;
//字符串控制码的内部编码
enum KTEXT_CTRL_CODE
{
	KTC_INVALID			=	0,
	KTC_ENTER			=	0x0a,
	KTC_COLOR			=	0x02,	//后面跟随三个BYTE数据分别为RGB三色分量
	KTC_COLOR_RESTORE	=	0x03,	//回复到原来设置的颜色
	KTC_BORDER_COLOR	=	0x04,	//设置边缘色，后面跟随三个BYTE数据分别为RGB三色分量
	KTC_BORDER_RESTORE	=	0x05,	//设置边缘色还原
	KTC_INLINE_PIC		=	0x06,	//嵌入式图片[wxb 2003-6-19]
	KTC_LINK			=	0x07,	//嵌入图形描述控件[ChenWeiMing 2004-11-9]
	//KTC_ITEM			=	0x08,	//嵌入式窗口（带物品信息）
	KTC_TAB				=	0x09,	//tab
	KTC_SPACE			=	0x20,	//空格
	KTC_TAIL			=	0xFF,	//字符串结束
};

struct KTP_CTRL
{
	unsigned char	cCtrl;	//此成员取值为Text.h中梅举KTEXT_CTRL_CODE中的一个值
	union
	{
	    struct
	    {
   			unsigned char cParam0;	//此及以下两个成员的取值与含义依据cCtrl的取值而定
		    unsigned char cParam1;
		    unsigned char cParam2;
	    };
	    unsigned short wParam;
   };
};

//## 表述颜色的结构
union KERColor
{
	struct {unsigned char b, g, r, a; } Color_b;
	unsigned int	Color_dw;
};

#define MAX_SYSTEM_INLINE_PICTURES	500 	//系统预留的嵌入式图片个数  4096

struct IInlinePicEngineSink
{
	//lấy kích thước ảnh
	virtual long GetPicSize(unsigned short wIndex, int& cx, int& cy) = 0;
	//Vẽ ảnh
	virtual long DrawPic(unsigned short wIndex, int x, int y) = 0;
	//Tự động tải hình ảnh và nhận được WORD, tức là chỉ mục của hình ảnh
	virtual long AddCustomInlinePic(unsigned short& wIndex, const char* szSprPathName) = 0;
	//Tự động dỡ hình ảnh
	virtual long RemoveCustomInlinePic(unsigned short wIndex) = 0;
};

//接口 IInlinePicEngineSink 由应用层实现并挂接进 Engine 模块 [wxb 2003-6-19]
//相关挂接函数:
// AdviseEngine(IInlinePicEngineSink*);
// UnAdviseEngine(IInlinePicEngineSink*);
extern "C"
{
	int EGetBit(int nIntValue,int nBitNumber);
	int ESetBit(int nIntValue,int nBitNumber,int nBitValue);
	int EGetByte(int nIntValue,int nByteNumber);
	int ESetByte(int nIntValue,int nByteNumber,int nByteValue);
	int FilterTextColor(char* pMsgBuff, unsigned short nMsgLength);
	unsigned int TGetColor(const char* pColor);
	const char* TGetColorStr(unsigned int nColor);
	void TReplaceText(char* pBuffer, const char* pszKey, const char* pszText);
	//Nhận ký tự hiển thị tiếp theo của dòng này
	const char* TGetSecondVisibleCharacterThisLine(const char* pCharacter, int nPos, int nLen);
	//Phát hiện một ký tự có phải là ký tự không được phép đặt ở đầu dòng hay không, nếu không phải là ký tự bị hạn chế thì trả về 0, ngược lại thì trả về số phần con mà ký tự đó chiếm
	int TIsCharacterNotAlowAtLineHead(const char* pCharacter);
	//Nếu độ dài chuỗi ban đầu (bao gồm cả dấu chấm hết) vượt quá giới hạn, nó sẽ bị cắt bớt và có hậu tố là ..
	const char* TGetLimitLenString(const char* pOrigString, int nOrigLen, char* pLimitLenString, int nLimitLen);
	//Nếu độ dài chuỗi ban đầu (bao gồm cả ký tự điều khiển) (bao gồm cả ký tự kết thúc) vượt quá độ dài giới hạn, nó sẽ bị cắt bớt và có hậu tố là ..
	const char* TGetLimitLenEncodedString(const char* pOrigString, int nOrigLen, int nFontSize,
		int nWrapCharaNum, char* pLimitLenString, int& nShortLen, int nLineLimit, int bPicPackInSingleLine = false);
	//Tìm nơi thích hợp để tách một chuỗi
	int	TSplitString(const char* pString, int nDesirePos, int bLess);
	//Tìm vị trí thích hợp để tách chuỗi trong chuỗi được mã hóa
	int	TSplitEncodedString(const char* pString, int nCount, int nDesirePos, int bLess);
	//获得指定行的开始位置
	int TGetEncodeStringLineHeadPos(const char* pBuffer, int nCount, int nLine, int nWrapCharaNum, int nFontSize, int bPicPackInSingleLine = false);
	//Chuyển đổi các thẻ điều khiển trong chuỗi văn bản, loại bỏ các ký tự không hợp lệ và rút ngắn độ dài lưu trữ của chuỗi văn bản
	int	TEncodeText(char* pBuffer, int nCount);
	int	TEncodeText_(char* pBuffer, int nCount);
	int	_TEncodeText(char* pBuffer, int nCount);
 //   void DelChar(char *inStr,char delchar,char *outStr);
	//int	TEGetTextString(char* pBuffer,char * nYStr,char * nDStr,int nCount,char* nFHBuffer);
	//Chuyển đổi các thẻ điều khiển trong chuỗi văn bản, loại bỏ các ký tự không hợp lệ và rút ngắn độ dài lưu trữ của chuỗi văn bản
	int TFilterEncodedText(char* pBuffer, int nCount);
	//Xóa các ký hiệu điều khiển khỏi văn bản được mã hóa
	int	TRemoveCtrlInEncodedText(char* pBuffer, int nCount);
	//Nhận số dòng và độ rộng dòng tối đa của văn bản được mã hóa
	int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine = 0, int nLineLimit = 0, int bPicSingleLine = false);
	//Đối với văn bản được mã hóa, hãy bắt đầu từ vị trí được chỉ định để tìm vị trí của ký hiệu điều khiển được chỉ định và trả về -1 để cho biết rằng không tìm thấy
	int	TFindSpecialCtrlInEncodedText(const char* pBuffer, int nCount, int nStartPos, char cControl);
	//Loại bỏ loại ký tự điều khiển được chỉ định khỏi văn bản được mã hóa
	int	TClearSpecialCtrlInEncodedText(char* pBuffer, int nCount, char cControl);
	//Đối với văn bản được mã hóa, hãy chỉ định vị trí trong bộ đệm của độ dài đầu ra
	int TGetEncodedTextOutputLenPos(const char* pBuffer, int nCount, int& nLen, bool bLess, int nFontSize);
	//Đối với văn bản được mã hóa, ký tự điều khiển trong bộ đệm phía trước được chỉ định có ảnh hưởng đến kết quả đầu ra tiếp theo
	int TGetEncodedTextEffectCtrls(const char* pBuffer, int nSkipCount, KTP_CTRL& Ctrl0, KTP_CTRL& Ctrl1);

	long AdviseEngine(IInlinePicEngineSink*);
	long UnAdviseEngine(IInlinePicEngineSink*);

}
#endif
