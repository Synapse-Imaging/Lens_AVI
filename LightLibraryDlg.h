#pragma once

#include "LibraryGridCtrl.h"
#include "SetLibraryNameDlg.h"

// CLightLibraryDlg 대화 상자

class CLightLibraryDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightLibraryDlg)

public:
	CLightLibraryDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLightLibraryDlg();

	static	CLightLibraryDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();

	BOOL	GetShowStatus() { return m_bShowDlg; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHT_LIBRARY_DLG };
#endif

	CButtonCS m_bnLightLibraryAdd;
	CButtonCS m_bnLightLibraryApply;
	CButtonCS m_bnLightLibraryDelete;
	CButtonCS m_bnLightLibraryRename;
	CButtonCS m_bnClose;

	int m_iCurListIdx;
	int m_iCurSelectedPageIndex;
	CDelegate delegateSelectedListApply;

	SPage m_UpdatePage;

protected:
	CLibraryGridCtrl m_GridLibraryList;

	void LoadLibrary();
	void UpdateLibrary(int iRowNumber, CString sLibName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	afx_msg LRESULT OnListSelect(WPARAM wParam, LPARAM lParam);

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CLightLibraryDlg*	m_pInstance;

	BOOL		m_bShowDlg;

public:
	afx_msg void OnBnClickedButtonLightLibraryAdd();
	afx_msg void OnBnClickedButtonLightLibraryApply();
	afx_msg void OnBnClickedButtonLightLibraryDelete();
	afx_msg void OnBnClickedButtonLightLibraryRename();
	afx_msg void OnBnClickedButtonClose();
};
