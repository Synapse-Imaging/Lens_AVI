#pragma once


// CPGMLanguageSelectDlg 대화 상자입니다.

class CPGMLanguageSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CPGMLanguageSelectDlg)

public:
	CPGMLanguageSelectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPGMLanguageSelectDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PGM_LANGUAGE_SELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CButtonCS m_buttonPGMLanguageKorean;
	CButtonCS m_buttonPGMLanguageEnglish;
	BOOL m_bCheckPGMLanguageDialogHide;
	afx_msg void OnBnClickedButtonPgmLangugageKorean();
	afx_msg void OnBnClickedButtonPgmLangugageEnglish();
};
