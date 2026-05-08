#pragma once


// CInspectionTypeChsWZDlg 대화 상자입니다.

class CInspectionTypeChsWZDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionTypeChsWZDlg)

public:
	CInspectionTypeChsWZDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionTypeChsWZDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_COSMETIC_INSPECTION_TYPE_CHS_WZ_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_iRadioInspectionType;
	int m_iRadioFaiRoiNumber;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	BOOL m_bFromPolygon;
	BOOL m_bGenLineRegion;

protected:
	CStaticCS m_LabelInspectType;
	CStaticCS m_LabelTeaching;
	CStaticCS m_LabelMask;
	CStaticCS m_LabelAlign;
	CStaticCS m_LabelCal;
	CStaticCS m_LabelMeasure;
	CStaticCS m_LabelDontCare;
	CStaticCS m_LabelGenerate;
	CStaticCS m_LabelAutoFocus;
	CStaticCS m_LabelMeasure2;
	CStaticCS m_LabelMeasure3;

	CStaticCS m_LabelGenerateColor;
	CStaticCS m_LabelAutoFocusColor;
	CStaticCS m_LabelDontCareColor;
	CStaticCS m_LabelLAlignColor;
	CStaticCS m_LabelGAlignColor;
	CStaticCS m_LabelAAlignColor;
	CStaticCS m_LabelMeasureColor;
	CStaticCS m_LabelMeasureColor2;
	CStaticCS m_LabelMaskColor;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGenLine();
};
