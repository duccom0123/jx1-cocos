#ifndef KMagicDescH
#define	KMagicDescH

#include "engine/KIniFile.h"

class KMagicDesc
{
private:
	KIniFile	m_MIniFile;
	char		m_szDesc[512];
public:
	KMagicDesc();
	~KMagicDesc();
	BOOL		Init();
	void        scat(char* p,char const* q);
	const char*	GetDesc(void* pData,int nType=0) ;//0为物品 1 为技能
	const char* GetDesc_(void *pData,int nType=0);
	const void 	GetDesc_New(char *strReturn,void* pData,int nType=0); //0为物品 1 为技能
	const char* BaoShiGetDesc(void *pData);
};

extern KMagicDesc	g_MagicDesc;
#endif
