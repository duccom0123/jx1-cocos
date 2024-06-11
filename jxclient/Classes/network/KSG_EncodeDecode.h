//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_EncodeDecode.h
//  Version     :   1.0
//  Creater     :   
//  Date        :   2003-6-3 10:29:43
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KSG_ENCODEDECODE_H
#define _KSG_ENCODEDECODE_H 1
#include "cocos2d.h"
USING_NS_CC;
int KSG_DecodeEncode(size_t uSize, unsigned char *pbyBuf, unsigned int *puKey);
int KSG_DecodeEncode_ASM(size_t uSize, unsigned char *pbyBuf, unsigned *puKey);
int KSG_DecodeEncode_New(unsigned int uSize, unsigned char *pbyBuf, unsigned int *puKey);

inline int KSG_DecodeBuf(size_t uSize, unsigned char *pbyBuf, unsigned *puKey)
{
	if (!uSize) return 1;
	if (!pbyBuf) return 1;
	if (!puKey) return 1;
	
	unsigned uKey = *puKey;
    return KSG_DecodeEncode(uSize,pbyBuf,puKey);
	
    //return KSG_DecodeEncode_ASM(uSize, pbyBuf, &uKey);
	//return KSG_DecodeEncode_New(uSize, pbyBuf, puKey);
	//return 1;
}

inline int KSG_EncodeBuf(size_t uSize, unsigned char *pbyBuf, unsigned *puKey)
{
	if (!uSize) return 1;
	if (!pbyBuf) return 1;
	if (!puKey) return 1;

	unsigned uKey = *puKey;

    return KSG_DecodeEncode(uSize, pbyBuf, puKey);
    //return KSG_DecodeEncode_ASM(uSize, pbyBuf, &uKey);
	//return KSG_DecodeEncode_New(uSize, pbyBuf, puKey);
}



#endif  // _KSG_ENCODEDECODE_H