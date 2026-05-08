#pragma once

#include "HWinPanel.h"
#include "LibraryGridCtrl.h"
#include "SetLibraryNameDlg.h"
#include "RoiAutoCreateDlg.h"

// CInspectLibraryDlg 대화 상자

class CInspectLibraryDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectLibraryDlg)

public:
	CInspectLibraryDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CInspectLibraryDlg();

	static	CInspectLibraryDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();

	BOOL	GetShowStatus() { return m_bShowDlg; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSPECT_LIBRARY_DLG };
#endif

	CButtonCS m_bnInspectLibraryAdd;
	CButtonCS m_bnInspectLibraryApply;
	CButtonCS m_bnInspectLibraryCreate;
	CButtonCS m_bnInspectLibraryDelete;
	CButtonCS m_bnInspectLibraryRename;
	CButtonCS m_bnClose;

	int m_iCurListIdx;

	CHWinPanel	m_UnitView;
	CRect		m_UnitViewRect;
	HObject		m_HUnitImage;
	int			m_iUnitImageWidth;
	int			m_iUnitImageHeight;

	// Auto Segmenation
	CComboBox m_cbAutoSegModel;
	CRoiAutoCreateDlg*  m_RoiAutoCreateDlg;
	static UINT AutoROINetworkThread(LPVOID lp);
	BOOL m_bAutoRoiCreateTriger;

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

	static CInspectLibraryDlg*	m_pInstance;

	BOOL		m_bShowDlg;

public:
	afx_msg void OnBnClickedButtonInspectLibraryAdd();
	afx_msg void OnBnClickedButtonInspectLibraryApply();
	afx_msg void OnBnClickedButtonInspectLibraryCreate();
	afx_msg void OnBnClickedButtonInspectLibraryDelete();
	afx_msg void OnBnClickedButtonInspectLibraryRename();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonInspectLibraryAutoSegCustom();
	afx_msg void OnBnClickedButtonInspectLibraryAutoSegStart();
	afx_msg void OnBnClickedButtonInspectLibraryAutoSegEnd();
};
