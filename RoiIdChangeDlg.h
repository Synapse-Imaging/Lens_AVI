#pragma once


// CRoiIdChangeDlg 대화 상자입니다.

class CRoiIdChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoiIdChangeDlg)

public:
	CRoiIdChangeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRoiIdChangeDlg();

	void SetROIType(int iType) { m_iROIType = iType; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROI_ID_CHANGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iEditId;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

protected:
	int m_iROIType;
};
