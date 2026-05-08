#pragma once


// CInspectResultDlg 대화 상자입니다.

#define	SLD_MAX_LOG_SIZE     5000

class CInspectResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectResultDlg)

public:
	CInspectResultDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectResultDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_RESULT_DLG };

	static CInspectResultDlg	*m_pInstance;

public:
	static	CInspectResultDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_sLastLotID;
public:
	void InitStaticText();
	CFont* GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight);

	CLabelCS m_LabelVisionModelStatic;
	CLabelCS m_LabelVisionModelName;
	CLabelCS m_LabelOpticalModelStatic;
	CLabelCS m_LabelOpticalModelName;

	CLabelCS m_LabelLotID;
	CLabelCS m_LabelTrayCount;

	CLabelCS m_ConnectAdj1;
	CLabelCS m_ConnectAdj2;
	CLabelCS m_ConnectAdj3;
	CLabelCS m_ConnectAdj4;
	CLabelCS m_ConnectAdj5;

	CLabelCS m_ConnectLas1;
	CLabelCS m_ConnectLas2;
	CLabelCS m_ConnectLas3;
	CLabelCS m_ConnectLas4;
	CLabelCS m_ConnectLas5;

	CLabelCS m_RawImageSavePathLas;
	CLabelCS m_RawImageSavePathLocal;
	CLabelCS m_ResultImageSavePathLas;
	CLabelCS m_ResultImageSavePathLocal;
	CLabelCS m_ReviewImageSavePathLas;
	CLabelCS m_ReviewImageSavePathLocal;
	CLabelCS m_ADJImageSavePathLas;
	CLabelCS m_ADJImageSavePathLocal;
	CLabelCS m_ResultLogSavePathLas;
	CLabelCS m_ResultLogSavePathLocal;


public:
	virtual BOOL OnInitDialog();
	void ChangeLanguage();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnEnChangeEditLotId();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
