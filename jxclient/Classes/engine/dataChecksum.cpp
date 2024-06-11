// MD5Checksum.cpp: implementation of the MD5Checksum class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "dataChecksum.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
//循环左移
char dataChecksum::__rol(char c, unsigned int count)  
{  
	unsigned int bitcount = sizeof(c) * 8;   //总的计算位数 1字节=8位数
	count %= bitcount;                 //取余数
	return (c << count) | (c >> (bitcount - count));  
}

int dataChecksum::pgChar2Int(char b)
{
#ifdef WIN32
	char i = 0;
	__asm{
		mov al, b
		xor al, PG_PKEYMASK
		rol al, PG_PKEYOFF
		mov i, al
		and i, 0x1f
	      }
	return i;
#else
	char i = b^PG_PKEYMASK;//xor al, 151; PG_PKEYMASK151 0x97
	i=__rol(i,PG_PKEYOFF);
	i=i&0x1F;
	return i; 
#endif
}
/*
char dataChecksum::pgInt2Char(char i, DWORD dwTickCount)
{
	char b;
//	ASSERT(i >= 0 && i <= PG_RESULTLENSTD - 2);
  if (i >= 0 && i <= PG_RESULTLENSTD - 2)
  {
	int index,nSize;
	for (index = dwTickCount % PG_CHARCOUNT, nSize = index + PG_CHARCOUNT; index < nSize; index++)
	{
		b = index % PG_CHARCOUNT + PG_MINCHAR;
		if (pgChar2Int(b) == i && PG_VALIDCHAR(b))
			return b;
	}
	for (index = dwTickCount % PG_CHARCOUNT, nSize = index + PG_CHARCOUNT; index < nSize; index++)
	{
		b = index % PG_CHARCOUNT + PG_MINCHAR;
		if (pgChar2Int(b) == i)
			return b;
	}
	//ASSERT(0);
  }
	return (char)0xFF;
}
*/
int dataChecksum::pgStrLen(const char* sz)
{
	int i = 0;
	while (sz[i])
		i++;
	return i;
}

void dataChecksum::pgSwapChars(char* sz)
{
	char c;
#define PG_SWAP(n1, n2)	c = sz[n1]; sz[n1] = sz[n2]; sz[n2] = c

	PG_SWAP(0, 13);
	PG_SWAP(31, 25);
	PG_SWAP(12, 30);
	PG_SWAP(7, 19);
	PG_SWAP(3, 21);
	PG_SWAP(9, 20);
	PG_SWAP(15, 18);
}
/*
int dataChecksum::pgEncrypt(char* szKey, int nKeyLen, char* szBuffer, const char* szPass, int nStrLen)
{
	int i, c, cc;
	for (i = 0; i < nStrLen; i++)
	{
		cc = szKey[i % nKeyLen];
		c = szPass[i];
		c = (((c - PG_MINCHAR ) + (cc - PG_MINCHAR)) % PG_CHARCOUNT) + PG_MINCHAR;
		if (PG_INVALIDCHAR(c))
		{
			int nDead = 0;
			do {
				c++;
				if (c > PG_MAXCHAR)
					c = PG_MINCHAR;
				cc++;
				if (cc > PG_MAXCHAR)
					cc = PG_MINCHAR;
				szKey[i % nKeyLen] = cc;

				//死循环，加密失败
				if (nDead ++ > 255)
					return 0;
			} while (PG_INVALIDCHAR(cc) || PG_INVALIDCHAR(c));
		}
		szBuffer[i] = c;
	}
	return 1;
}
*/
void dataChecksum::pgDecrypt(char* szKey, int nKeyLen, char* szBuffer, const char* szEnc, int nStrLen)
{
	int i, c, cc;
	for (i = 0; i < nStrLen; i++)
	{
		cc = szKey[i % nKeyLen];
		c = szEnc[i];
		szBuffer[i] = ((PG_CHARCOUNT + (c - PG_MINCHAR ) - (cc - PG_MINCHAR)) % PG_CHARCOUNT) + PG_MINCHAR;
	}
	szBuffer[i] = 0;
}
/*
int dataChecksum::pgSameString(const char* s1, const char* s2)
{
	char c1, c2;
	int i;
	for (i = 0; (c1 = s1[i]) && (c2 = s2[i]); i++)
	{
		if (c1 != c2)
			return 0;
	}
	if (s1[i] || s2[i])
		return 0;
	return 1;
}
*/
//解密：成功返回 1, 失败返回 0
int dataChecksum::SimplyDecrypt(char* szPass, const char* szEncrypted)
{
	int nLen = pgStrLen(szEncrypted), nPKLen = 0, i = 0;
	char szBuffer[PG_RESULTLENSTD + 1];
	if (nLen != PG_RESULTLENSTD)
		return 0;

	for (i = 0; i < PG_RESULTLENSTD; i++)
		szBuffer[i] = szEncrypted[i];
	szBuffer[PG_RESULTLENSTD] = 0;

	pgSwapChars(szBuffer);

	nPKLen = pgChar2Int(szBuffer[0]);
	if (nPKLen < PG_MINPKEYLEN || nPKLen > PG_MAXPKEYLEN)
		return 0;
	nLen = pgChar2Int(szBuffer[nPKLen + 1]);
	if (nLen < 0 || nLen > PG_MAXPASSWORDLEN)
		return 0;
	pgDecrypt(szBuffer + 1, nPKLen, szPass, szBuffer + nPKLen + 2, nLen);

	return 1;
}


//加密：成功返回 1, 失败返回 0
/*int dataChecksum::pgEncrypt_(char* szResult, const char* szPass, DWORD dwTickCount, int& nLevel)
{
	nLevel++;
	if (nLevel > 32)
		return 0;

	int nPrimeNumberList[PG_RESULTLENSTD] = {//基本的数字列表
		1153,	1789,	2797,	3023,	3491,	3617,	4519,	4547,
		5261,	5939,	6449,	7307,	8053,	9221,	9719,	9851,
		313,	659,	1229,	1847,	2459,	3121,	3793,	4483,
		5179,	6121,	6833,	7333,	7829,	8353,	9323,	9929,
	};

	int nLen = pgStrLen(szPass), nPKeyLen = 0, i = 0;
	int nFlagEncryptOK = 0;
	if (nLen > PG_MAXPASSWORDLEN)
		return 0;
	for (i = 0; i < nLen; i++)
	{
		if (szPass[i] < PG_MINCHAR || szPass[i] > PG_MAXCHAR)
			return 0;
	}

	nPKeyLen = PG_RESULTLENSTD - nLen - 2;
	if (nPKeyLen > PG_MINPKEYLEN)
		nPKeyLen = (dwTickCount + 10237) % (nPKeyLen - PG_MINPKEYLEN) + PG_MINPKEYLEN;

	szResult[0] = pgInt2Char(nPKeyLen, dwTickCount);

	//public key
	for (i = 0; i < nPKeyLen; i++)
	{
		DWORD dwRandom = dwTickCount + nPrimeNumberList[i];
		char c = (char)((dwRandom % PG_CHARCOUNT) + PG_MINCHAR);
		if (PG_INVALIDCHAR(c))
			c = (char)((dwRandom & 1) ? 'a' + (dwRandom % 26) : 'A' + (dwRandom % 26));
		szResult[i + 1] = c;
	}

	szResult[nPKeyLen + 1] = pgInt2Char(nLen, dwTickCount);
	
	nFlagEncryptOK = pgEncrypt(szResult + 1, nPKeyLen, szResult + nPKeyLen + 2, szPass, nLen);

	//fill
	for (i = 0; i < PG_RESULTLENSTD - 2 - nPKeyLen - nLen; i++)
	{
		DWORD dwRandom = (dwTickCount + nPrimeNumberList[PG_RESULTLENSTD - i - 1]);
		char c = (char)((dwRandom % PG_CHARCOUNT) + PG_MINCHAR);
		if (PG_INVALIDCHAR(c))
			c = (char)((dwRandom & 1) ? 'a' + (dwRandom % 26) : 'A' + (dwRandom % 26));
		szResult[PG_RESULTLENSTD - i - 1] = c;
	}
	pgSwapChars(szResult);

	szResult[PG_RESULTLENSTD] = 0;

	char szPassCheck[PG_RESULTLENSTD + 1];
	int nFlagDecryptOK = 0;
	int nDead = 0;

	//recheck password
	if (nFlagEncryptOK)
		nFlagDecryptOK = SimplyDecrypt(szPassCheck, szResult); //解码
	if (!nFlagEncryptOK || !nFlagDecryptOK || !pgSameString(szPassCheck, szPass))
		return pgEncrypt_(szResult, szPass, dwTickCount + 9929, nLevel);//加密

	return 1;
}
*/
//加码
/*int dataChecksum::SimplyEncrypt(char* szResult, const char* szPass)
{
	DWORD dwTickCount =0;
#ifdef WIN32
	dwTickCount = ::GetTickCount();  //获取运行时间
#else
	cc_timeval m_pStartUpdate;
	CCTime::gettimeofdayCocos2d(&m_pStartUpdate,NULL);
	dwTickCount = m_pStartUpdate.tv_sec*1000+m_pStartUpdate.tv_usec/1000;
#endif
	int nLevel = 0;
	return pgEncrypt_(szResult, szPass, dwTickCount, nLevel);
}*/