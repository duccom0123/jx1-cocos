#include "KCore.h"
//#include "../../Represent/iRepresent/iRepresentShell.h"
#include "KSubWorldSet.h"

//iRepresentShell*	g_pRepresent = 0;

unsigned int	l_Time = 0;

unsigned int IR_GetCurrentTime()
{
	return l_Time;
}

//--------------------------------------------------------------------------
//	功能：更新图形换帧计算用时钟
//--------------------------------------------------------------------------
void IR_UpdateTime()
{//CCTime::gettimeofdayCocos2d(m_nTimeStop.QuadPart,NULL);
	cc_timeval m_pStartUpdate;
	CCTime::gettimeofdayCocos2d(&m_pStartUpdate,NULL);
	l_Time = m_pStartUpdate.tv_sec*1000+m_pStartUpdate.tv_usec/1000;//毫秒timeGetTime();获得系统运行的时间
}

int	IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer)
{
	if ((l_Time - uLastTimer) >= uInterval)
	{
		uLastTimer += uInterval;
		return 1;
	}
	return 0;
}
//获取剩余时间，如果时间已经到了/过了，返回值都为0
unsigned int IR_GetRemainTime(unsigned int uInterval, unsigned int uLastTimer)
{
	register unsigned int uRemain;
	if ((uRemain = l_Time - uLastTimer) < uInterval)
		return (uInterval - uRemain);
	return 0;
}
//--------------------------------------------------------------------------
//	功能：换帧计算
//--------------------------------------------------------------------------
void IR_NextFrame(int& nFrame, int nTotalFrame, unsigned int uInterval, unsigned int& uFlipTime)
{
	if (nTotalFrame > 1 && uInterval)
	{
		while ((l_Time - uFlipTime) >= uInterval)
		{
			uFlipTime += uInterval;
			if ((++nFrame) >= nTotalFrame)
			{
				uFlipTime = l_Time;
				nFrame = 0;
				break;
			}
		}
	}
}
