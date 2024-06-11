#ifndef KLinkArray_H
#define	KLinkArray_H

class KLinkNode
{
public:
	int nPrev;
	int	nNext;
public:
	KLinkNode() { nPrev = nNext = 0; }
};

class  KLinkArray
{
private:
	KLinkNode*	pNodeL;
	int nCount;
public:
	KLinkArray() { pNodeL = NULL; nCount = 0;}
	~KLinkArray();
	int GetCount() { return nCount; }
	KLinkNode * GetNode(int nIdx){return &pNodeL[nIdx];};
	void			Init(int nSize);
	void			Remove(int nIdx);
	void			Insert(int nIdx);
	int				GetNext(int nIdx) const { 
		if(pNodeL) return pNodeL[nIdx].nNext; 
		else return 0;
	}
	int	GetPrev(int nIdx) const { 
		if(pNodeL) return pNodeL[nIdx].nPrev; 
		else return 0;
	}
};
#endif
