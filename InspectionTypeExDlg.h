#pragma once


// CInspectionTypeExDlg 대화 상자입니다.

class CInspectionTypeExDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionTypeExDlg)

public:
	CInspectionTypeExDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionTypeExDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_COSMETIC_INSPECTION_TYPE_EX_DLG };

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

	int m_iPositionID;
	int m_iPointID;

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

	CComboBox m_cbMeasurePos;		// 측정 위치 ex) Chasis A Hole
	CComboBox m_cbMeasurePoint;		// 측정 포인트 ex) P1

	CComboBox m_cbBlendPos;			// 합성 용도 ex) Blend X, Valid X
	CComboBox m_cbBlendPoint;		// 합성 포인트 ex) Start, End

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGenLine();
};
