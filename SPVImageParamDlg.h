#pragma once


// CSPVImageParamDlg 대화 상자입니다.

class CSPVImageParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CSPVImageParamDlg)

public:
	CSPVImageParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSPVImageParamDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPV_IMAGE_PARAM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	double m_dEditDFMPsx;
	double m_dEditDFMPsy;
	double m_dEditDFMTex;
	double m_dEditDFMTey;

	double m_dEditDFMSpecularImageSF[DFM_MAX_SPECULAR_IMAGE_NUMBER];
	int m_iEditDFMMeanFilterSize;
	double m_dEditDFMHigh[DFM_MAX_SHAPE_IMAGE_NUMBER];
	double m_dEditDFMLow[DFM_MAX_SHAPE_IMAGE_NUMBER];
	double m_dEditDFMDerivativeSmFactor[DFM_MAX_SHAPE_IMAGE_NUMBER];

	double m_dEditDiffusedHigh[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];
	double m_dEditDiffusedLow[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];
	double m_dEditDiffusedSmFactor[DIFFUSED_MAX_SHAPE_IMAGE_NUMBER];

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonOfflineSpecular();
	afx_msg void OnBnClickedButtonOfflineDiffused();

	void ChangeLanguage();
};
