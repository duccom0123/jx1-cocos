#ifndef KMISSLESET_H
#define KMISSLESET_H
#include "KMissle.h"
#include "engine/KLinkArray.h"

class  KMissleSet
{
	KLinkArray		m_FreeIdxMissleSet;				//	可用表
	KLinkArray		m_UseIdxMissleSet;				//	已用表
public:
	void	Init();
	int		Activate();
	int		FindFree();
	int		Add(int SubWorldId, int regionid, int x , int y , int dx = 0 , int dy = 0);
	int		Add(int SubWorldId, int px, int py);
	int		CreateMissile(int nSkillId, int nMissleId, int nLauncher,  int nTargetId ,int nSubWorldId, int nPX, int nPY, int nDir);
	void	Remove(int nIndex);
	void	ClearMissles();
	int		GetCount();
	void	Draw();
};
extern  KMissleSet MissleSet;
#endif
