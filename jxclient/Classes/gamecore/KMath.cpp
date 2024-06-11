#include "KCore.h"
#include <math.h>
#include "GameDataDef.h"
#include "KMath.h"
    // 正弦表 (将浮点数 *1024 整型化)
int		g_nSinBuffer[64] = 
    {
	    1024,	1019,	1004,	979,	946,	903,	851,	791,	
        724,	649,	568,	482,	391,	297,	199,	100,	
        0,	   -100,	-199,	-297,	-391,	-482,	-568,	-649,	
        -724,	-791,	-851,	-903,	-946,	-979,	-1004,	-1019,	
        -1024,	-1019,	-1004,	-979,	-946,	-903,	-851,	-791,	
        -724,	-649,	-568,	-482,	-391,	-297,	-199,	-100,	
        0,	     100,	199,	297,	391,	482,	568,	649,	
        724,	791,	851,	903,	946,	979,	1004,	1019
    };

    //---------------------------------------------------------------------------
    // 余弦表 (将浮点数 *1024 整型化)
    int		g_nCosBuffer[64] = 
    {
	    0,	    -100,	-199,	-297,	-391,	-482,	-568,	-649,	
        -724,	-791,	-851,	-903,	-946,	-979,	-1004,	-1019,	
        -1024,	-1019,	-1004,	-979,	-946,	-903,	-851,	-791,	
        -724,	-649,	-568,	-482,	-391,	-297,	-199,	-100,	
        0,	     100,	199,	297,	391,	482,	568,	649,	
        724,	791,	851,	903,	946,	979,	1004,	1019,	
        1024,	1019,	1004,	979,	946,	903,	851,	791,	
        724,	649,	568,	482,	391,	297,	199,	100,
    };

    // 正弦余弦的查表函数代码缓冲区
   /* unsigned char g_InternalDirSinCosCodeBuffer[] =
    {
        0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x7C, 0x16, 
        0x8B, 0x4C, 0x24, 0x0C, 0x3B, 0xC1, 0x7D, 0x0E, 
        0xC1, 0xE0, 0x06, 0x99, 0xF7, 0xF9, 0x8B, 0x4C, 
        0x24, 0x04, 0x8B, 0x04, 0x81, 0xC3, 0x83, 0xC8, 
        0xFF, 0xC3
    }; */

    // 正弦余弦的查表函数代码
    //int	__cdecl g_InternalDirSinCos(int pSinCosTable[], int nDir, int nMaxDir)
    //{
    //	if (nDir < 0 || nDir >= nMaxDir)
    //		return -1;
    //
    //	int	nIndex = (nDir << 6) / nMaxDir;
    //
    //	return pSinCosTable[nIndex];
    //}


    // 五行相生相克函数代码缓冲区
   /* unsigned char g_InternalIsAccrueConquerCodeBuffer[] =
    {
        0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x7C, 0x1E, 
        0x83, 0xF8, 0x05, 0x7D, 0x19, 0x8B, 0x54, 0x24, 
        0x0C, 0x56, 0x8B, 0x74, 0x24, 0x08, 0x57, 0x33, 
        0xC9, 0x8B, 0x3C, 0x86, 0x3B, 0xD7, 0x5F, 0x0F, 
        0x94, 0xC1, 0x8B, 0xC1, 0x5E, 0xC3, 0x33, 0xC0, 
        0xC3
    };	*/

    // 五行相生相克函数代码
    //int __cdecl g_InternalIsAccrueConquer(int pAccrueConquerTable[], int nSrcSeries, int nDesSeries)
    //{
    //	if (nSrcSeries < 0 || nSrcSeries >= series_num)
    //		return false;
    //
    //	return (nDesSeries == pAccrueConquerTable[nSrcSeries]);
    //}

	// 五行相生相克函数代码
	/*BOOL __cdecl g_InternalIsAccrueConquer(INT pAccrueConquerTable[], INT nSrcSeries, INT nDesSeries)
	{
		if (nSrcSeries < series_metal || nSrcSeries >= series_num)
			return FALSE;
		
		return	nDesSeries == pAccrueConquerTable[nSrcSeries];
	} */ 

    //---------------------------------------------------------------------------
    // 正弦表 (将浮点数 *1024 整型化)
    int		*g_nSin = g_nSinBuffer;

    // 余弦表 (将浮点数 *1024 整型化)
    int		*g_nCos = g_nCosBuffer;

    // 正弦余弦的查表函数代码缓冲区
    //unsigned char *g_InternalDirSinCosCode = g_InternalDirSinCosCodeBuffer;


    //---------------------------------------------------------------------------
    // 五行相生相克

    // 五行相生相克函数代码缓冲区
    //unsigned char *g_InternalIsAccrueConquerCode = g_InternalIsAccrueConquerCodeBuffer;
    //---------------------------------------------------------------------------

    int g_InitMath()
    {
        return true;

    }

    int g_UnInitMath()
    {
        return true;
    }

//---------------------------------------------------------------------------
// 五行相生相克
/*int		g_nAccrueSeries[series_num]; //相生
int		g_nConquerSeries[series_num];//相克

void g_InitSeries()
{
	g_nAccrueSeries[series_metal] = series_water; //相生
	g_nConquerSeries[series_metal] = series_wood; //相克
	g_nAccrueSeries[series_wood] = series_fire;
	g_nConquerSeries[series_wood] = series_earth;
	g_nAccrueSeries[series_water] = series_wood;
	g_nConquerSeries[series_water] = series_fire;
	g_nAccrueSeries[series_fire] = series_earth;
	g_nConquerSeries[series_fire] = series_metal;
	g_nAccrueSeries[series_earth] = series_metal;
	g_nConquerSeries[series_earth] = series_water;
}
 */
//---------------------------------------------------------------------------
// 五行相生相克
INT		g_nAccrueSeries[series_num];
INT		g_nConquerSeries[series_num];
INT		g_nAccruedSeries[series_num];
INT		g_nConqueredSeries[series_num];

VOID g_InitSeries()
{
	// 五行生克关系
	g_nAccrueSeries[series_metal] = series_water;  //相生
	g_nConquerSeries[series_metal] = series_wood;
	g_nAccruedSeries[series_metal] = series_earth;
	g_nConqueredSeries[series_metal] = series_fire;

	g_nAccrueSeries[series_wood] = series_fire;
	g_nConquerSeries[series_wood] = series_earth;
	g_nAccruedSeries[series_wood] = series_water;
	g_nConqueredSeries[series_wood] = series_metal;
	g_nAccrueSeries[series_water] = series_wood;
	g_nConquerSeries[series_water] = series_fire;
	g_nAccruedSeries[series_water] = series_metal;
	g_nConqueredSeries[series_water] = series_earth;
	g_nAccrueSeries[series_fire] = series_earth;
	g_nConquerSeries[series_fire] = series_metal;
	g_nAccruedSeries[series_fire] = series_wood;
	g_nConqueredSeries[series_fire] = series_water;
	g_nAccrueSeries[series_earth] = series_metal;
	g_nConquerSeries[series_earth] = series_water;
	g_nAccruedSeries[series_earth] = series_fire;
	g_nConqueredSeries[series_earth] = series_wood;
}

