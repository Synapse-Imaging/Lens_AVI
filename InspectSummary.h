#pragma once

#include"Algorithm.h"
#include "afxcmn.h"
// CInspectSummary 대화 상자입니다.

class CInspectSummary : public CDialog
{
	DECLARE_DYNAMIC(CInspectSummary)

public:
	CInspectSummary(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectSummary();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_SUMMARY_DLG };
	static CInspectSummary	*m_pInstance;

	CEvent	m_evModelLoadDone;	//AutoReset 

public:
	static CInspectSummary* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	void Show();

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	void DoEvents(DWORD dwSleep);

public:

	afx_msg void OnBnClickedMfcbuttonInspectStart();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedMfcbuttonMacroPath();

	void MakeTrayResultSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo);
	void MakeTrayReviewSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo);
	void MakeTrayAdjSaveFolder(int iPcVisionNo, CString sLotID, int iMzNo, int iTrayNo);

	int CheckLotIDAndChangeModel(int iMzNo, CString sLotID, CString sHandlerModelName, CString *psProgramVersion, CString *psModelVersion);
	
	void ShowDiskCapacity();

	void Initialize_TrayStart_Vision_N1(CString sLotID, int iMzNo, int iTrayNo);
	void Initialize_TrayStart_Vision_N2(CString sLotID, int iMzNo, int iTrayNo);
	void Initialize_TrayStart_Vision_N3(CString sLotID, int iMzNo, int iTrayNo);
	void Initialize_TrayStart_Vision_N4(CString sLotID, int iMzNo, int iTrayNo);

	CProgressCtrl m_ctrlProgressHddC;
	CProgressCtrl m_ctrlProgressHddD;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMfcbuttonSaveParam();
	afx_msg void OnBnClickedMfcbuttonUpdateParam();
	virtual BOOL OnInitDialog();
	void ChangeLanguage();

	BOOL m_bDxRunMacro;
	BOOL m_bUseRunMacro;
	BOOL m_bDxAutoSettingZFocus;
	BOOL m_bUseAutoSettingZFocus;
	BOOL m_bDxAutoSettingLight;
	BOOL m_bUseAutoSettingLight;

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedCheckRunMacro();
	afx_msg void OnBnClickedChkAutosettingZfocus();
	afx_msg void OnBnClickedChkAutosettingLight();

	CString strLog;
};
