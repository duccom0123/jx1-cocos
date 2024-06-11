//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KStrBase.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	String Utility Functions
//---------------------------------------------------------------------------
#include "KMemBase.h"
#include "KStrBase.h"
#include "KPaklist.h"
#ifndef WIN32
#include "KGbktoUtf8.h"
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include <direct.h>
#endif
#include <string.h>
#include <stdarg.h>
//isgraph 检测字符是否为可显示字符（函数）
//ispunct 检测字符是否为标点符号（函数）
//isalnum 检查字符是否是字母或者数字（函数）
//isspace 检测字符是否为空白（函数）
//isprint 判断是否打印字符 isdigit
//isxdigit 检查参数c是否为16 进制数字，只要c为下列其中一个情况就检测成功 0123456789ABCDEF
//isalnum,isalpha,isdigit,isxdigit,iscntrl,isgraph,isprint,ispunct

int GetBig5Count(char *str)
{
	int lnBIG5 = 0;//用於统计可能是繁体字的汉字个数
	int lnGB = 0;//用於统计可能是简体字的汉字个数
	int liTranLen = strlen(str);

	for(int liT = 0; liT<liTranLen-1; liT++)
	{
		//尾字节40-7E是BGI5码特有的,如果扫描到这种编码说明此字元串是繁体(经测试:有例外,可能是汉字的最後一个编码与英文编码组合而成的)
		if((BYTE)(BYTE)str[liT]>=161 && (BYTE)(BYTE)str[liT]<=254 && (BYTE)(BYTE)str[liT+1]>=64 && (BYTE)(BYTE)str[liT+1]<=126)
			lnBIG5 ++;

		if((BYTE)(BYTE)str[liT]>=129 && (BYTE)(BYTE)str[liT]<=160 && (BYTE)(BYTE)str[liT+1]>=64 && (BYTE)(BYTE)str[liT+1]<=126)
			lnBIG5 ++;

		if((BYTE)(BYTE)str[liT]>=129 && (BYTE)(BYTE)str[liT]<=160 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
			lnBIG5 ++;

		//首字节A4-A9在GB中为日文假名,希腊字母,俄文字母和制表符,正常文本中很少出现,而这个范围是BIG5的常用汉字,所以认为这是BIG5码
		if((BYTE)(BYTE)str[liT]>=164 && (BYTE)(BYTE)str[liT]<=169 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
			lnBIG5 ++;

		//GB中首字节AA-AF没有定义,所以首字节位於AA-AF之间,尾字节位於A1-FE的编码几乎100%是BIG5(经测试:没有100%),认为是BIG5码
		/*if((BYTE)(BYTE)str[liT]>=170 && (BYTE)(BYTE)str[liT]<=175 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
			lnBIG5 ++;
		*/
		//首字节C6-D7,尾字节A1-FE在GB中属於一级字库,是常用汉字,而在BIG5中,C6-C7没有明确定义,但通常用来放日文假名和序号,C8-D7属於罕用汉字区,所以可认为是GB码
		//if((BYTE)(BYTE)str[liT]>=196 && (BYTE)(BYTE)str[liT]<=215 && (BYTE)(BYTE)str[liT+1]>=161 && (BYTE)(BYTE)str[liT+1]<=254)
		//	lnGB ++;
	}

	//如果扫描完整个字元串,可能是简体字的数目比可能是繁体字的数目多就认为是简体字不转简(不一定准确)
	//return lnBIG5-lnGB;
	return lnBIG5;
}

UINT g_Atoui(LPSTR str)
{
	UINT result = 0, i = 0;

	char *tmp = NULL;

	for (i = 0; isspace(str[i]) && i < strlen(str); i++)//跳过空白符;  
		;
	tmp = str+i;

	while (*tmp)
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}

	return result;
}

int g_Round2Int(double d){
	return static_cast<int>(d+(d > 0.0 ? 0.5 : -0.5));
}

float g_Int2Round(int d){
	return static_cast<float>(0.0 + d);
}

int g_ExtractChar(const char *inStr,char scrchar,char destchar,char *outStr,char *outStra,int nMoedel)
{
	char *tmp=NULL,*tmpa=NULL;         //定义一个临时数组空间，存放字符；
	tmp=outStr,tmpa=outStra; 
	int nLen=0;
	while(*inStr!='\0')
	{//一直前行,直到有结束符号将停止。
		if(nMoedel==0 && *inStr==scrchar)
		{//提取两个字符之间的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。     
           	while(*inStr!='\0')
			{//接着前行   
                  if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {		            
					  //*inStr='|';       //替换掉结束字符
                      inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }
				  *tmpa=*inStr;           //提取字符放进tempa中
				  tmpa++;	              //下一个元素			  
	              inStr++;                //下一个元素
			}
			*tmpa='\0';                  //提取完后，加个结束符号
		}
		else if (nMoedel==1 && *inStr==scrchar)
		{//提取字符前面的内容 存放在 tmp中
              break;
		}
		else if (nMoedel==2 && *inStr==scrchar)
		{//提取字符后面的内容 存放在 tmpa中
			inStr++;                      //过滤掉开始字符，指向下个数进行比较。     
           	while(*inStr!='\0')
			{//接着前行,继续提取后面的内容   
                  /*if (*inStr==destchar)   //数组中的第二个元素与结束字符相同
				  {		            
					  //*inStr='|';       //替换掉结束字符
                      inStr++;            //过滤掉结束符，指向下个数。
					  break;              //找到结束符号 过滤掉 跳出循环。
				  }*/
				  *tmpa=*inStr;           //提取字符放进tempa中
				  tmpa++;	              //下一个元素			  
	              inStr++;                //下一个元素
			}
			*tmpa='\0';                  //提取完后，加个结束符号
		}

		if (*inStr=='\0')                //如果没有找到结束符，并且已经到了结尾，就跳出了！！
			break;

		*tmp=*inStr;                     //元数不相同，则存放到tmp中。
		tmp++;                           //进行下一个元数的比较
		inStr++;                    
		nLen++;
	}
	*tmp='\0';                           //源字符串 提取完后 加个结束符 防止乱码
    return nLen;
}

/*UINT g_Atoui(LPSTR str)
{
	UINT result = 0, i = 0;

	char *tmp = NULL;

	for (i = 0; isspace(str[i]) && i < strlen(str); i++)//跳过空白符;  
		;
	tmp = str+i;

	while (*tmp)
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}

	return result;
}*/

int g_StrLen(LPCSTR lpStr)
{
/*#ifdef WIN32
	register int nLen;

	__asm
	{
		mov		edi, lpStr
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		not		ecx
		dec		ecx
		mov		nLen, ecx
	}
	return nLen;
#else*/
     return strlen(lpStr);
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrEnd
// 功能:	返回字符串结尾指针
// 参数:	lpStr	:	字符串开头的指针
// 返回:	lpEnd	:	字符串末尾的指针
//---------------------------------------------------------------------------
LPSTR g_StrEnd(LPCSTR lpStr)
{
/*#ifdef WIN32
	register LPSTR lpEnd;

	__asm
	{
		mov		edi, lpStr
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		lea		eax, [edi - 1]
		mov		lpEnd, eax
	}
	return lpEnd;
#else*/
     return (char *)lpStr + strlen(lpStr);
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCpy
// 功能:	字符串拷贝
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
void g_StrCpy(LPSTR lpDest, LPCSTR lpSrc)
{
#ifdef WIN32
	__asm
	{
		mov		edi, lpSrc
		mov		ecx, 0xffffffff
		xor		al, al
		repne	scasb
		not		ecx
		mov		edi, lpDest
		mov		esi, lpSrc
		mov		edx, ecx
		shr		ecx, 2
		rep		movsd
		mov		ecx, edx
		and		ecx, 3
		rep		movsb
	};
#else
	t_sprintf(lpDest,"%s",lpSrc);
#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCpyLen
// 功能:	字符串拷贝,有最大长度限制
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
//			nMaxLen	:	最大长度
// 返回:	void
//---------------------------------------------------------------------------
void g_StrCpyLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
#ifdef WIN32
	__asm
	{
		xor		al, al
		mov		edx, nMaxLen
		dec		edx
		jg		copy_section

		jl		finished
		mov		edi, lpDest
		stosb
		jmp		finished

copy_section:
		mov		edi, lpSrc
		mov		ecx, 0xffffffff
		repne	scasb
		not		ecx
		dec		ecx
		cmp		ecx, edx
		jle		loc_little_equal
		mov		ecx, edx

loc_little_equal:

		mov		edi, lpDest
		mov		esi, lpSrc
		mov		edx, ecx
		shr		ecx, 2
		rep		movsd
		mov		ecx, edx
		and		ecx, 3
		rep		movsb
		stosb
		
finished:
	};
#else
    //strncpy(lpDest, lpSrc, nMaxLen);
	if (nMaxLen>0)
	   t_sprintf(lpDest,"%s",lpSrc);
	else
       t_sprintf(lpDest,"%s",lpSrc);
#endif
}
//---------------------------------------------------------------------------
// 函数:	StrCat
// 功能:	字符串末尾追加另一个字符串
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
// 返回:	void
//---------------------------------------------------------------------------
void g_StrCat(LPSTR lpDest, LPCSTR lpSrc)
{
	register LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpy(lpEnd, lpSrc);
}
//---------------------------------------------------------------------------
// 函数:	StrCatLen
// 功能:	字符串末尾追加另一个字符串,有最大长度限制
// 参数:	lpDest	:	目标字符串
//			lpSrc	:	源字符串
//			nMaxLen	:	最大长度
// 返回:	void 没有调用
//---------------------------------------------------------------------------
void g_StrCatLen(LPSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	register LPSTR lpEnd;

	lpEnd = g_StrEnd(lpDest);
	g_StrCpyLen(lpEnd, lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrCmp
// 功能:	字符串比较
// 参数:	lpDest	:	字符串1	
//			lpSrc	:	字符串2
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
BOOL g_StrCmp(LPCSTR lpDest, LPCSTR lpSrc)
{
	register int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nLen1 != nLen2)
		return FALSE;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nLen1);
}
//---------------------------------------------------------------------------
// 函数:	StrCmpLen
// 功能:	字符串比较,限定长度
// 参数:	lpDest	:	字符串1	
//			lpSrc	:	字符串2
//			nLen	:	长度
// 返回:	TRUE	:	相同
//			FALSE	:	不同
//---------------------------------------------------------------------------
BOOL g_StrCmpLen(LPCSTR lpDest, LPCSTR lpSrc, int nMaxLen)
{
	register int nLen1, nLen2;

	nLen1 = g_StrLen(lpDest);
	nLen2 = g_StrLen(lpSrc);
	if (nMaxLen > nLen1)
		nMaxLen = nLen1;
	if (nMaxLen > nLen2)
		nMaxLen = nLen2;
	return g_MemComp((void*)lpDest, (void*)lpSrc, nMaxLen);
}
//---------------------------------------------------------------------------
// 函数:	StrUpper
// 功能:	小写字母转大写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
void g_StrUpper(LPSTR lpDest)
{
/*#ifdef WIN32
	__asm
	{
		mov		esi, lpDest
loc_lodsb:
		lodsb
		or		al, al
		je		loc_exit
		cmp		al, 'a'
		jb		loc_lodsb
		cmp		al, 'z'
		ja		loc_lodsb
		sub		al, 0x20
		mov		[esi - 1], al
		jmp		loc_lodsb
loc_exit:
	}
#else*/
     char *ptr = lpDest;
     while(*ptr) {
         if(*ptr >= 'a' && *ptr <= 'z') //*ptr += 'A' - 'a';
          *ptr = toupper(*ptr);
          ptr++;
     }
//#endif
}
//---------------------------------------------------------------------------
// 函数:	StrLower
// 功能:	大写字母转小写字母
// 参数:	lpDest	:	字符串
// 返回:	void
//---------------------------------------------------------------------------
void g_StrLower(LPSTR lpDest)
{
/*#ifdef WIN32
	__asm
	{
		mov		esi, lpDest
loc_lodsb:
		lodsb
		or		al, al
		je		loc_exit
		cmp		al, 'A'
		jb		loc_lodsb
		cmp		al, 'Z'
		ja		loc_lodsb
		add		al, 0x20
		mov		[esi - 1], al
		jmp		loc_lodsb
loc_exit:
	}
#else*/

	
	/*std::string lowerCase(lpDest);
	for (unsigned int i = 0; i < lowerCase.length(); ++i)
	{
	  lowerCase[i] = tolower(lowerCase[i]);
	}*/
	
	char *ptr = lpDest;
	while(*ptr) {
		if(*ptr >= 'A' && *ptr <= 'Z')
			//*ptr += 'a' - 'A';
		    *ptr = tolower(*ptr);
		++ptr;
	}
//#endif
}
//---------------------------------------------------------------------------没有调用
void g_StrRep(LPSTR lpDest, LPSTR lpSrc, LPSTR lpRep)
{
	int		nSrcLen = g_StrLen(lpSrc);
	int		nDestLen = g_StrLen(lpDest);
	int		nMaxLen = nDestLen - nSrcLen + g_StrLen(lpRep) + 1;
	char	*pStart = NULL;
        int i;
	for (i = 0; i < nDestLen - nSrcLen; i++)
	{
		if (g_StrCmpLen(&lpDest[i], lpSrc, nSrcLen))
			break;
	}
	if (i == nDestLen - nSrcLen)
		return;

	pStart = new char[nMaxLen];

	if (i != 0)
	{
		g_StrCpyLen(pStart, lpDest, i);
		g_StrCat(pStart, lpRep);
		g_StrCat(pStart, &lpDest[i + nSrcLen]);
	}
	else
	{
		g_StrCpy(pStart, lpRep);
		g_StrCat(pStart, &lpDest[nSrcLen]);
	}
	g_StrCpy(lpDest, pStart);
	if (pStart)
	{
		delete [] pStart;
		pStart = NULL;
	}
}


static void t_printchar(char **str, int c)
{
/*#ifndef WIN32
	extern int putchar(int c);
#endif*/
	if (str) {
		**str = c;
		++(*str);
	}
	//else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int t_prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			t_printchar(out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		t_printchar(out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		t_printchar(out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int t_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return t_prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			t_printchar(out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + t_prints(out, s, width, pad);
}

static int t_print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out_t;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += t_prints(out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += t_printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += t_printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X') {
				pc += t_printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += t_printi(out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += t_prints(out, scr, width, pad);
				continue;
			}
		}
		else {
out_t:
			t_printchar(out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int t_printf(const char *format, ...)
{
	va_list args;

	va_start( args, format );
	return t_print( 0, format, args );
}

int t_sprintf(char *out, const char *format, ...)
{
	va_list args;
	va_start( args, format );
	return t_print( &out, format, args );
}


int t_snprintf(char *buf, unsigned int count, const char *format, ... )
{
	va_list args;

	(void) count;

	va_start( args, format );
	return t_print( &buf, format, args );
}

/*
荣耀7(PLK-CL00)里无法处理中文，中文被截断，处理英文数字正常，怀疑内部实现的是UTF-8
酷派5360里处理中文正常，怀疑内部使用的是ANSIC
修改方法，全部改成std::string的加号操作符，或者std::ostringstream的<<操作符，所有手机都正常了
--------------------- 
*/
static bool _mysprintf(LPTSTR szDst ,size_t nSize, const char* szFmt,...)
{
	assert(NULL != szDst && 0 < nSize || NULL != szFmt);
	va_list args;
	va_start(args, szFmt);
	const bool bRet =(0!= vsnprintf(szDst,nSize,szFmt,args) );//  2013-7-26 上午1:19:29
	va_end(args);
	szDst[nSize-1] = '\0' ;
	return bRet ;
}

//创建文件夹
bool __CreateFolder(std::string nSdcardPath, char * nPath)
{
	if (!nPath || nSdcardPath=="") return false;
	/* 格式化路径并创建目录 */
	/*for (int i = 0; i < (int)strlen(nPath); i++)
	{
		if (nPath[i] == '/') 
		{
			nPath[i] = '\\';
		}
	}
	if(nPath[0]=='\\'||nPath[0]=='/')
	{
		memcpy(oPath,nPath+1,strlen(nPath)-1);
	}
	else 
	{
		memcpy(oPath, nPath, strlen(nPath));
	}*/

	char tPath[255];
	if (nPath[0]=='/'|| nPath[0]=='\\')
	{
		nPath[0]='/';
		nPath++;
	}

	for (size_t i = 1; i < strlen(nPath); i++)
	{
		if (nPath[i] == '\\') nPath[i] = '/';
		if (nPath[i] == '/')
		{
			memcpy(tPath, nPath, i );
			tPath[i] = 0;
			char nTempDir[256];
			ZeroMemory(nTempDir,sizeof(nTempDir));
			t_sprintf(nTempDir,"%s%s",nSdcardPath.c_str(),tPath);
#ifdef WIN32 //完整路径
			_mkdir(nTempDir);//tPath
#else
			//DIR *pDir = NULL;
			//pDir = opendir(nTempDir);
			//if (!pDir)
			mkdir(nTempDir,S_IRWXU|S_IRWXG|S_IRWXO);
		    //else
			   //closedir(pDir);
#endif
		}
	}

	return true;

}

void g_StrCopy(std::string nSdcardPath, char *pMusicName)
{
	if (!pMusicName || nSdcardPath=="" || !g_pPakList) return;
	//开始解压PAK音乐文件
	char m_szEntireMapFile[256];
	ZeroMemory(m_szEntireMapFile,sizeof(m_szEntireMapFile));
	XPackElemFileRef m_PackRef;
	ZeroMemory(&m_PackRef,sizeof(XPackElemFileRef));
	int nIsHave = 0;
	//CCMessageBox(pMusicName,nSdcardPath.c_str());
	nIsHave  = g_pPakList->pGetFilePath(pMusicName,m_PackRef);
	if (nIsHave)
	{//如果存在这个音乐
		//CCMessageBox(pMusicName,"找到文件");
		char * sBuf=NULL;
		sBuf = (char *)malloc(m_PackRef.nSize+1);
		if (sBuf)
		{
			__CreateFolder(nSdcardPath,pMusicName);
			g_pPakList->ElemFileRead(m_PackRef,sBuf,m_PackRef.nSize);
			//CCMessageBox(m_szEntireMapFile,"开始查找");
#ifdef WIN32
			t_sprintf(m_szEntireMapFile,"%s%s",nSdcardPath.c_str(),pMusicName);
#else
			t_sprintf(m_szEntireMapFile,"%s%s",nSdcardPath.c_str(),G2U(pMusicName).c_str());
#endif
			FILE *oFp = NULL;
			if((oFp=fopen(m_szEntireMapFile,"wb+"))==0)
				return;

			fseek(oFp,0,0);
			fwrite(sBuf,m_PackRef.nSize,1,oFp);
			fclose(oFp);
			oFp = NULL;

			free(sBuf);
			sBuf = NULL;
		}
	}
}

unsigned int _ccHash(const char *key)
{
	unsigned int len = strlen(key);
	const char *end=key+len;
	unsigned int hash;

	for (hash = 0; key < end; key++)
	{
		hash *= 16777619;
		hash ^= (unsigned int)(unsigned char)toupper(*key);
	}
	return (hash);
}
