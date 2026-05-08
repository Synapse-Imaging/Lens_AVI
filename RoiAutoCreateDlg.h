#pragma once


// CRoiAutoCreateDlg 대화 상자

class CRoiAutoCreateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRoiAutoCreateDlg)
private:
	int m_iModel_Type_index;
	float m_fPred_iou_threshold;
	float m_fStability_score_thresh;
	float m_fStability_score_offset;
	float m_fMask_threshold;
	float m_fBox_nms_thresh;
	int m_iMin_mask_region_area;
	int m_bUse_m2m;

public:
	CRoiAutoCreateDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CToolTipCtrl m_ToolTip;
	virtual ~CRoiAutoCreateDlg();

public:
	int m_iselectedRoiIndex;
	HRegion TestRegion[5];
	HRegion m_hRegion;
	CComboBox m_ComboSelectManualRoiModel;
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnEnChangeMyEdit();
	// void OnBnClickedCheckRegionList();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOk();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROI_AUTO_CREATE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
