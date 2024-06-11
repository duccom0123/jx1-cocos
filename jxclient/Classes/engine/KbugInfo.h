#ifndef KstrFile_H
#define KstrFile_H

#include "cocos2d.h"
//#include "tchar.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#pragma warning (disable: 4005)
USING_NS_CC;

typedef struct _SECURITY_ATTRIBUTES_S {
	unsigned long nLength;
	void * lpSecurityDescriptor;
	bool bInheritHandle;
} SECURITY_ATTRIBUTES_S, *PSECURITY_ATTRIBUTES_S, *LPSECURITY_ATTRIBUTES_S;

typedef unsigned short *PWORD;
//#define PDWORD * DWORD
typedef unsigned long  *PDWORD;
typedef unsigned long  *LPDWORD;

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#define IN
#define OUT
#define BYTE unsigned char
#define MAX_PATH 1200
#define UINT unsigned int
#define INT  int
#define FALSE false
#define TRUE  true
#define DWORD unsigned long
#define WORD  unsigned short

#define BOOL int
#define LPCSTR const char *

#define PVOID void *
#define LPVOID PVOID
#define LPSTR char *
#define LONG  long
#define PBYTE unsigned char *
#define LPBYTE PBYTE
#define LPTSTR LPSTR
//typedef unsigned char BYTE;
typedef long   LONG_PTR;
typedef unsigned long  DWORD_PTR;
typedef unsigned long  DWORD_PTRD;
typedef unsigned int   UINT_PTR;
typedef unsigned long  ULONG_PTR;
//UINT_PTR
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)

#define MAX_CHUNK	250		//最大的栈数   250
#define HANDLE void *

#ifndef WIN32
//#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
enum enumServerConnectInfo
{
	enumServerConnectCreate = 0x100,
	enumServerConnectClose
};
#define CP_UTF8 65001
typedef int* LPINT;
#define FLOAT float
#define LPCTSTR const char *
#define CONST const
#define SOCKET_ERROR -1
#define  VOID void
#define CALLBACK __stdcall
#define  CHAR char
#define  ULONG unsigned long
typedef int SOCKET;
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL;}}
#define RELEASE(a) if(a){a->Release();a=NULL;}
#define SAFE_DELETE(a) if(a){delete a;a=NULL;}
#define SAFE_CLOSEHANDLE(x)		try{ if (x) {close(x); (x) = NULL; } } catch(...) { }
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
#define INVALID_SOCKET -1
#define ZeroMemory(x,y)  memset((x),0,(y))
//typedef struct fd_set FD_SET;
//typedef struct fd_set FD_SET;
//#define FD_ZERO(set) (((fd_set FAR *)(set))->fd_count=0)

typedef struct timeval TIMEVAL;
typedef struct linger LINGER;
typedef struct sockaddr_in SOCKADDR_IN;

typedef struct {
    ULONG len;     /* the length of the buffer */
    CHAR *buf; /* the pointer to the buffer __field_bcount(len) */
}WSABUF, * LPWSABUF;

//typedef void (CALLBACK *CALLBACK_CLIENT_EVENT )(LPVOID lpParam,const unsigned long &ulnEventType);
typedef struct
{
	ULONG_PTR Internal;
	ULONG_PTR InternalHigh;
	union 
	{
		struct 
		{
			unsigned long Offset;
			unsigned long OffsetHigh;
		};
		void * Pointer;
	};
	 void * hEvent;
}OVERLAPPED;

typedef struct tagSIZE
{
    long        cx;
    long        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef struct
{
	long  x;
	long  y;
}POINT;

typedef struct tagRECT
{
	long    left;
	long    top;
	long    right;
	long    bottom;
}RECT;
#define MAKELONG(a, b) ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)      ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)      ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)      ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)      ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

#endif //end #ifndef WIN32 #ifndef WIN32 #ifndef WIN32 #ifndef WIN32

typedef struct {
	BYTE		Blue;		// 兰色分量
	BYTE		Green;		// 绿色分量
	BYTE		Red;		// 红色分量
	BYTE		Alpha;		// 透明分量
} KPAL32;					// 32位色盘
//---------------------------------------------------------------------------
typedef struct {
	BYTE		Red;		// 红色分量
	BYTE		Green;		// 绿色分量
	BYTE		Blue;		// 兰色分量
} KPAL24;					// 24位色盘
//---------------------------------------------------------------------------
typedef WORD	KPAL16;		// 16位色盘

enum eImageFormat
{
	IMAGE_FORMAT_NULL,
	IMAGE_8888,
	IMAGE_888,
	IMAGE_565,
	IMAGE_4444,
	IMAGE_256,
};

struct stImageInfo    //图片属性申明
{
	unsigned long			nDirections;
	unsigned long			nFrames;
	int				bytes_per_pixel;
	LONG			size;
	BYTE*			buffer;	
	LONG			width,height;
	int				x,y;
};
typedef struct
{
	WORD bfType; // 位图文件的类型，必须为BM(1-2字节）
	DWORD bfSize; // 位图文件的大小，以字节为单位（3-6字节）
	WORD bfReserved1; // 位图文件保留字，必须为0(7-8字节）
	WORD bfReserved2; // 位图文件保留字，必须为0(9-10字节）
	DWORD bfOffBits; // 位图数据的起始位置，以相对于位图（11-14字节）
	// 文件头的偏移量表示，以字节为单位
} BITMAPFILEHEADERS;

typedef struct 
{
	DWORD biSize; // 本结构所占用字节数（15-18字节）
	LONG biWidth; // 位图的宽度，以像素为单位（19-22字节）
	LONG biHeight; // 位图的高度，以像素为单位（23-26字节）
	WORD biPlanes; // 目标设备的级别，必须为1(27-28字节）
	WORD biBitCount;// 每个像素所需的位数，必须是1（双色），（29-30字节）
	// 4(16色），8(256色）或24（真彩色）之一
	DWORD biCompression;  // 位图压缩类型，必须是 0（不压缩），（31-34字节）
	// 1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
	DWORD biSizeImage;    // 位图的大小，以字节为单位（35-38字节）
	LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数（39-42字节）
	LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数（43-46字节)
	DWORD biClrUsed;      // 位图实际使用的颜色表中的颜色数（47-50字节）
	DWORD biClrImportant; // 位图显示过程中重要的颜色数（51-54字节）
} BITMAPINFOHEADERS;

typedef struct {
	BYTE		Red;		// 红色分量
	BYTE		Green;		// 绿色分量
	BYTE		Blue;		// 兰色分量
} KPAL244;					// 24位色盘
typedef struct 
{
	BYTE rgbBlue;    // 蓝色的亮度（值范围为0-255)
	BYTE rgbGreen;   // 绿色的亮度（值范围为0-255)
	BYTE rgbRed;     // 红色的亮度（值范围为0-255)
	BYTE rgbReserved;// 保留，必须为0
} RGBQUADS;

typedef struct 
{
	BITMAPINFOHEADERS bmiHeader;    // 位图信息头
	KPAL244          bmiColors[1]; // 颜色表
} BITMAPINFOS;

typedef struct
{
	/* //工具
	BYTE	Comment[4];	// 注释文字(SPR\0)
	WORD	Width;		// 图片宽度
	WORD	Height;		// 图片高度
	int 	CenterX;	// 重心的水平位移
	int 	CenterY;	// 重心的垂直位移
	WORD	Frames;		// 总帧数
	WORD	Colors;		// 颜色数
	WORD	Directions;	// 方向数
	WORD	nInterval;	// 每帧间隔（以游戏帧为单位）
	WORD	Reserved[6];// 保留字段（到以后使用）*/
	BYTE	Comment[4];	// 注释文字(SPR\0)
	WORD	Width;		// 图片宽度
	WORD	Height;		// 图片高度
	WORD	CenterX;	// 重心的水平位移
	WORD	CenterY;	// 重心的垂直位移
	WORD	Frames;		// 总帧数
	WORD	Colors;		// 颜色数
	WORD	Directions;	// 方向数
	WORD	nInterval;	// 每帧间隔（以游戏帧为单位）
	WORD	Reserved[6];// 保留字段（到以后使用）
} SPRHEAD;

#define	SPR_COMMENT_FLAG				0x525053	//'SPR'

//---------------------------------------------------------------------------
typedef struct
{
	DWORD	Offset;		// 每一帧的偏移
	DWORD	Length;		// 每一帧的长度
} SPROFFS;

//---------------------------------------------------------------------------
typedef struct
{
	WORD	Width;		// 帧最小宽度
	WORD	Height;		// 帧最小高度
	WORD	OffsetX;	// 水平位移（相对于原图左上角）
	WORD	OffsetY;	// 垂直位移（相对于原图左上角）
	BYTE	Sprite[1];	// RLE压缩图形数据
} SPRFRAME;

enum enumSubDir{sdFont=0,sdMaps,sdScript,sdResource,sdSetting,sdSound,sdSpr,sdUi,sdLastDir};
//一个Pack文件具有的头结构:老PAK头
struct XPackFileHeader
{
	unsigned char cSignature[4];		//四个字节的文件的头标志，固定为字符串'PACK'
	unsigned long uCount;				//文件数据的条目数
	unsigned long uIndexTableOffset;	//索引的偏移量
	unsigned long uDataOffset;			//数据的偏移量
	unsigned long uCrc32;				//校验和
	unsigned int  uPakTime;			    //打包文件制作时的时间，秒为单位time()
	unsigned char cReserved[8];		    //保留的字节
};

#define	XPACKFILE_SIGNATURE_FLAG		0x4b434150	//'PACK'
//Pack中对应每个子文件的索引信息项
struct XPackIndexInfo
{
	unsigned long	uId;				//子文件id
	unsigned long	uOffset;			//子文件在包中的偏移位置
	long			lSize;				//子文件的原始大小
	long			lCompressSizeFlag;	//子文件压缩后的大小和压缩方法
//	FILETIME        UpdateTime;
	//最高字节表示压缩方法，见XPACK_METHOD
	//低的三个字节表示子文件压缩后的大小
};


//pak包中保存的spr帧信息项
struct XPackSprFrameInfo
{
	long lCompressSize;
	long lSize;
} ;

//分块压缩的文件的文件数据存储区存储的内容与顺序一般为：
// [XPackFileFragmentElemHeader] + [第一块数据] + [第二块数据] + .. + [第n块数据] + [n个XPackFileFragmentInfo的数组]

//分块压缩的文件在该文件数据存储区起始位置的头数据结构
struct XPackFileFragmentElemHeader
{
	int				nNumFragment;		//分块的数目
	int				nFragmentInfoOffest;//分块信息表的偏移位置,相对于XPackFileFragmentElemHeader开始位置的偏移
};

//分块压缩文件的分块信息
struct XPackFileFragmentInfo
{
	unsigned int	uOffset;			//此分块数据的起始处在子文件数据区中的偏移位置,相对于XPackFileFragmentElemHeader开始位置的偏移
	unsigned int	uSize;				//此分块的原始大小
	unsigned int	uCompressSizeFlag;	//此分块压缩后的大小和压缩方法，类似与XPackIndexInfo结构中的uCompressSizeFlag
};

//包文件的压缩方式
enum XPACK_METHOD
{
	    TYPE_NONE	           = 0x00000000,			//没有压缩
		TYPE_UCL	           = 0x10000000,			//子文件分块压缩 UCL压缩
		TYPE_BZIP2	           = 0x20000000,	        //新包, 表示经过了UCL算法压缩
		TYPE_FRAGMENT          = 0x30000000,            //分块压缩
		TYPE_FRAGMENTA         = 0x40000000,            //分块压缩
		TYPE_FRAME	           = 0x10000000,			//使用了独立帧压缩,子文件为spr类型时才可能用到
		TYPE_METHOD_FILTER     = 0xf0000000,	        //新包，压缩标记
		TYPE_FILTER            = 0xf0000000,			//新包，过滤标记
		TYPE_FILTER_SIZE       = 0x07ffffff,            //新包，压缩后大小过滤标记
		TYPE_UCL_OLD	       = 0x01000000,			//老包,表示该文件数据是经过分块压缩 UCL压缩
		TYPE_BZIP2_OLD	       = 0x02000000,	        //老包,表示经过了UCL算法压缩
		TYPE_METHOD_FILTER_OLD = 0x0f000000,            //老包, 压缩标记
		TYPE_FILTER_OLD        = 0xff000000,			//老包, 过滤标记
		TYPE_FRAGMENT_OLD      = 0x03000000,            //老包,分块压缩 有
		TYPE_FRAGMENTA_OLD     = 0x04000000,            //老包,分块压缩
		TYPE_FILTER_SIZE_OLD   = 0x00ffffff,            //老包,压缩后大小过滤标记
		XPACK_COMPRESS_SIZE_BIT	= 27,
};

//--------------------------------------------------
//使用XPackFile对子文件进行访问操作所用到的辅助结构
//--------------------------------------------------
struct	XPackElemFileRef
{
	unsigned long	uId;			//PAK文件id
	int				nPackIndex;		//PAK包索引
	int				nElemIndex;		//包内的子文件索引
	int				nCacheIndex;	//缓冲索引
	int				nOffset;		//子文件的操作便移动
	int				nSize;			//子文件的大小
};



/*typedef struct tagRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
}RECT,*PRECT,*NPRECT,*LPRECT;
*/

/*void g_MessageBox(char* lpMsg, ...)
{
#ifndef __linux
	char szMsg[256]={0};
	va_list va;
	va_start(va, lpMsg);
	vsprintf(szMsg, lpMsg, va);
	va_end(va);
//	g_DebugLog(szMsg);
	//	MessageBox(g_GetMainHWnd(), szMsg, 0, MB_OK);
	MessageBox(NULL, (LPCTSTR)(szMsg), 0, MB_OK);
#endif

}*/

class KstrFile
{
private:

public:
	KstrFile();
	~KstrFile();

	char *_strupr(char *s)
	{

	};

	char *strlwr(char *s)
	{
	 char *str;
	 str = s;
	 while(*str != '\0')
	 {
	  if(*str >= 'A' && *str <= 'Z') {
	     *str += 'a'-'A';
	 }
	 str++;
	 }
	   return s;
	 }
};
//---------------------------------------------------------------------------
#endif


