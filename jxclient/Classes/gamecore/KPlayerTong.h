//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.h
// Date:	2003.08.12
// Code:	�߳�����
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTONG_H
#define KPLAYERTONG_H

struct AttAckInfo
{
	std::string m_szMapName; //������ͼ
	int 	m_idx;			 //����
	int 	m_mapidx;		 //���
	std::string	m_Tongmaster;//ռ������
	std::string	m_Mastername;//����
	int     m_levle;		 //���еȼ�
	int     m_shushou;		 //����˰��	
};


class KPlayerTong
{
	friend class KPlayer;
private:
	int			m_nPlayerIndex;
	int			m_nFigure;		//ְλ
	DWORD		m_dwTongNameID; //�����
	char		m_szTitle[32];
	int			m_nApplyTo;     //�Ƿ������˼����� �������߹��� ���浵!

public:
	int			m_nFlag;           //�Ƿ������
	int         m_nLevel;          //���ȼ�
	int			m_nCamp;
	int         m_AttackState;
	int         m_AttackCamp;
	int         m_AttackTime;				        
	int         m_nDeathCount;	   //�����Ĵ���
	int         m_nDDeathCount;	   //�����Ĵ���

	int         m_nAttackNum;	                            // ��ս����
	int         m_nWinNum;								// ʤ������
	int         m_nLoseNum;

	char        m_AttackName[32];  //��ս�İ��
	char		m_BMasterName[32]; //������
	//char		m_szMasterName[32];//������
	char		m_szName[32];      //�����
	DWORD		m_nMoney;
	int         ActiveLoop;
	int         m_Mapid;           //����ͼ�ı��
	int         m_CurRenShu;
private:
	BOOL		CheckAcceptAddApplyCondition();	// �ж��Ƿ����ת�����˵ļ���������
	int         CheckAcceptLevel(char *nTongName,int nPlayerLevel);

public:
	// ��ʼ��
	void		Init(int nPlayerIdx);
	void		Clear();
	void		GetTongName(char *lpszGetName);
	DWORD		GetTongNameID();
	void		SetTongNameID(DWORD dwID) { m_dwTongNameID = dwID; };
	int			CheckIn() {return m_nFlag;};      //�Ƿ�������
	int			GetCamp() {return m_nCamp;};
	int			GetFigure() {return m_nFigure;};  //��ȡְλ
	void	    Active();
	//AttAckInfo  AttAck[7];
	typedef std::map<INT, AttAckInfo> _AttAckInfo;                 //SkillId,Echance
	_AttAckInfo nAttAckInfo;

	// �Ƿ���Ȩ����ѯ�ӳ���Ϣ
	BOOL		CanGetManagerInfo(DWORD dwTongNameID);
	// �Ƿ���Ȩ����ѯ������Ϣ
	BOOL		CanGetMemberInfo(DWORD dwTongNameID);

#ifndef _SERVER

	void        SetTongLevel(char *lpszTongName,int nLevel);
	void        SetTongMap(char *lpszTongName,int nMap);
	void		SetTongCredit(char *lpszTongName,int nCredit);        //�����ʽ�
	void		SetTongMasterTitle(char *lpszTongName,char *nTitle);  //�����ƺ�
	void		SetTongDirectorTitle(char *lpszTongName,char *nTitle);//���ϳƺ�
	void		SetTongManagerTitle(char *lpszTongName,char *nTitle); //�ӳ��ƺ�
	void		SetTongNormalTitle(char *lpszTongName,char *nTitle);  //���ڳƺ�

	void        SetTong(char *lpszTongName,int nMap);

	// ��������ս
	void        SendAttack(char *lpszTongName);
	// ���봴�����
	BOOL		ApplyCreateTong(int nCamp, char *lpszTongName);
	// �õ�������֪ͨ�������
	void		Create(TONG_CREATE_SYNC *psCreate);
	// ���������
	BOOL		ApplyAddTong(DWORD dwNpcID);
	// �Ƿ���ܳ�Ա bFlag == TRUE ���� == FALSE ������
	void		AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag);
	// �����ᣬ��Ϊ��ͨ����
	BOOL		AddTong(int nCamp, char *lpszTongName, char *lpszTitle, char *lpszMaster);
	// ��������
	BOOL		ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName);
	// ��������
	BOOL		ApplyKick(int nCurFigure, int nCurPos, char *lpszName);
	// ���봫λ
	BOOL		ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName);
	// �����뿪���
	BOOL		ApplyLeave();

	BOOL		ApplySaveMoney(DWORD nMoney);

	BOOL		ApplyGetMoney(DWORD nMoney);

	BOOL		ApplySndMoney(DWORD nMoney);

	void        SetAttAckInfo(BYTE *Msg);

	


	/* �����ð����Ϣ		 const char * pMsg
	if nInfoID == enumTONG_APPLY_INFO_ID_SELF ���ò���
	if nInfoID == enumTONG_APPLY_INFO_ID_MASTER nParam1 �����id
	if nInfoID == enumTONG_APPLY_INFO_ID_DIRECTOR nParam1 �����id
	if nInfoID == enumTONG_APPLY_INFO_ID_MANAGER nParam1 �����id nParam2 �ӿ�ʼ��Ч�ڼ��� nParam3 ��������
	if nInfoID == enumTONG_APPLY_INFO_ID_MEMBER nParam1 �����id nParam2 �ӿ�ʼ��Ч�ڼ��� nParam3 ��������
	if nInfoID == enumTONG_APPLY_INFO_ID_ONE lpszName ����
	if nInfoID == enumTONG_APPLY_INFO_ID_TONG_HEAD nParam1 NpcIndex
	*/
	BOOL		ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName = NULL);
	// �趨���������Ϣ
	void		SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo);
	
	void        OpenCreateInterface();

	void		ChangeMoney(DWORD dwMoney) {m_nMoney = dwMoney;};

	void        SetAttAckCityInfo(BYTE *Msg);
#endif

};
#endif