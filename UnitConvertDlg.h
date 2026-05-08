#pragma once


// CUnitConvertDlg 대화 상자입니다.

class CUnitConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CUnitConvertDlg)

public:
	CUnitConvertDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUnitConvertDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_UNIT_CONVERT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	double m_dEditLengthInputUm;
	int m_iEditLengthOutputPixelUm;
	int m_iEditLengthInputPixel;
	double m_dEditLengthOutputUm;
	double m_dEditAreaInputMm;
	int m_iEditAreaOutputPixel;
	int m_iEditAreaInputPixel;
	double m_dEditAreaOutputMm;
	afx_msg void OnBnClickedButtonConvertLengthUm();
	afx_msg void OnBnClickedButtonConvertLengthPixel();
	afx_msg void OnBnClickedButtonConvertAreaMm();
	afx_msg void OnBnClickedButtonConvertAreaPixel();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

protected:
	CButtonCS m_bnConvertLengthUm;
	CButtonCS m_bnConvertLengthPixel;
	CButtonCS m_bnConvertAreaMm;
	CButtonCS m_bnConvertAreaPixel;
	CButtonCS m_bnOK;
};
