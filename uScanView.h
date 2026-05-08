// uScanView.h : interface of the CuScanView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_)
#define AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NewModelDlg.h"
#include "PreferenceDlg.h"
#include "afxwin.h"

class CuScanView : public CFormView
{
	DECLARE_DYNCREATE(CuScanView)

protected: // create from serialization only
	CuScanView();
	virtual ~CuScanView();

public:
	//{{AFX_DATA(CuScanView)
	enum { IDD = IDD_uScan_FORM };

	CPreferenceDlg PreferenceDlg;

	CNewModelDlg m_pNewModelDlg;

	CStaticCS m_StaticSPIVersion;

	//}}AFX_DATA

// Attributes
public:
	//CuScanDoc* GetDocument();
	void SetStatusText(int idx, CString szMsg="");

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CuScanView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void TeachingShow();
	void UpdateTeachModule();
	void UpdateCurMode();

	void SetExitProgram();
	void SetTextPCBCount(CString strPCBCount);
	void SetButtonStatus(int nStatus);

	void ShowVersionText();
	void UpdateVersionText();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTime	m_StartTime;
	BOOL	m_bTeachingShow;

	CButtonCS m_bnModeChange;
	CLabelCS m_LabelTeachModule;
	CLabelCS m_LabelCurMode;
	BOOL m_bStatusToggle;

// Generated message map functions
protected:
	//{{AFX_MSG(CuScanView)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LRESULT OnEventChangeMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventChangeStatus(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnAiSetupExited(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedMfcbuttonFormNew();
	afx_msg void OnBnClickedMfcbuttonFormLoad();
	afx_msg void OnBnClickedMfcbuttonFormSave();
	afx_msg void OnBnClickedMfcbuttonFormTeachingVisionN1();
	afx_msg void OnBnClickedMfcbuttonFormTeachingVisionN2();
	afx_msg void OnBnClickedMfcbuttonFormTeachingVisionN3();
	afx_msg void OnBnClickedMfcbuttonFormTeachingVisionN4();
	afx_msg void OnBnClickedMfcbuttonFormPreference();
	afx_msg void OnBnClickedMfcbuttonFormExit();
	afx_msg void OnBnClickedButtonModeChange();
	afx_msg void OnBnClickedMfcbuttonFormRegister();
	afx_msg void OnBnClickedMfcbuttonFormPrevTray();

	void ChangeLanguage();
	CString strLog;
	CString strDialog;
	CString strMessageBox;
	afx_msg void OnBnClickedMfcbuttonFormAisetup();
	void ChangeHeaderImage(UINT nBitmapID);

protected:
	HANDLE ai_setup_process_handle_ = nullptr; // 실행된 프로그램의 프로세스 핸들 저장
	static UINT AFX_CDECL AiSetupWaitThreadProc(LPVOID pParam);
	bool ReinitializeAiInspection();

private:
	CStatic m_pStaticMainTitle;
	HBITMAP m_hHeaderBitmap;
};

#ifndef _DEBUG  // debug version in uScanView.cpp
//inline CuScanDoc* CuScanView::GetDocument() { return (CuScanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_)
