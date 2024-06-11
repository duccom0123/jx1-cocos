#include "KCore.h"
//#include "MyAssert.H"
#include "KPlayer.h"
#include "KPlayerTask.h"
#include "KTaskFuns.h"

KTabFile g_StringResourseTabFile;

char* g_GetStringRes(int nStringID, char * szString, int nMaxLen)
{
	char szStringId[10];
	sprintf(szStringId, "%d", nStringID);
	g_StringResourseTabFile.GetString(szStringId, "STRING", "", szString, nMaxLen);
	return szString;
}