#pragma once


// CTabControlDlg 대화 상자입니다.
#include "ThresholdHistogramDlg.h"
#include "LightControlDlg.h"
#include "JogSetDlg.h"
#include "InspectionConditionTabDlg.h"
#include "ExtraDlg.h"

#include "TeachParamDlg.h"

#include "TeachingAlgorithmTabDlg.h"
#include "AutoFocusDlg.h"
#include "MotionControlDlg.h"
#include "InspectLibraryDlg.h"
#include "GrabSequenceViewDlg.h"
#include "CameraLightManagerDlg.h"

class CTabControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CTabControlDlg)

public:
	CTabControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACHING_TAB_DLG };
	static CTabControlDlg	*m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()

public:
	static CTabControlDlg* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSelchangeTeachingTab(NMHDR *pNMHDR, LRESULT *pResult);

	void Show();
	void ShowFirstPage(BOOL bReadTeachingImage);
	void HideAllPage();

	void ReadTeachingImage(int iTeachingImageTabIndex);
	
	void LoadModelConditionParam();

	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }

	CThresholdHistogramDlg* m_pThresholdHistogramDlg;
	CLightControlDlg*  m_pLightControlDlg;
	CJogSetDlg* m_pJogSetDlg;
	CInspectionConditionTabDlg* m_pInspectionConditionTabDlg;
	CExtraDlg* m_pExtraDlg;

	CTeachParamDlg* m_pTeachParamDlg;

	CTeachingAlgorithmTabDlg *m_pTeachingAlgorithmTabDlg;
	void ShowAlgorithmWnd(int iImageType, GTRegion *pROIRegion, int iInspectionTabIndex);
	void UpdateList(int iImageType, int iInspectionTabIndex);
	void ShowAlgorithmDlg();
	void HideAlgorithmDlg();

	CAutoFocusDlg *m_pAutoFocusDlg;
	void ShowAutoFocusDlg();
	void HideAutoFocusDlg();

	CInspectLibraryDlg *m_pInspectLibraryDlg;
	void ShowInspectLibraryDlg();
	void HideInspectLibraryDlg();

	CGrabSequenceViewDlg *m_pGrabSequenceViewDlg;
	void ShowGrabSequenceViewDlg();
	void HideGrabSequenceViewDlg();

	CMotionControlDlg *m_pMotionControlDlg;
	void ShowMotionControlDlg();
	void HideMotionControlDlg();

	CCameraLightManagerDlg* m_pCameraLightManagerDlg;
	void ShowCameraLightManagerDlg();
	void HideCameraLightManagerDlg();

	void GetROIUpdate(int iTabIdx, BOOL bCheckArea1, int iArea1Contour1Margin, int iArea1Contour2Margin, BOOL bCheckArea2, int iArea2Contour1Margin, int iArea2Contour2Margin, BOOL bCheckArea3, int iArea3Contour1Margin, int iArea3Contour2Margin);

	void TeachAlgorithmTest_Tab(int iImageType, int iAlgorithmTab);
	void TeachAlgorithmTest_ROI(int iImageType);
	void TeachAlgorithmTest_Image(int iImageType);
	void AlgorithmPreProcessing();
	void AlgorithmPreMeasurement();

	CTabCtrl m_TabControl;

	int m_iCurrentTab;
	afx_msg void OnDestroy();

	int GetAutoFocusLensRefHeight();
	POINT GetAutoFocusLeftTopCornerPoint(double *pdRotationAngle);
	DPOINT GetAutoFocusBlobCenterPoint(int iRotationAngle, BOOL bUseZigCenterPos, int iZigCenterX, int iZigCenterY);

	void ChangeLanguage();

	CButton m_btnPrev;
	CButton m_btnNext;
	int m_iCurOffset;
	void UpdateTabDisplay();

	std::vector<CString> m_arrAllTabs;
	int m_nScrollPos;

	static const int TABS_PER_LINE = 10;
	static const int VISIBLE_TABS = 20;

	CFont m_fontNormal;
	CFont m_fontBold;
	void InitFonts();
	void ResetTabPosition();
	void TabPositionFitting(int iTabsPerLine, int iTabHeight);

	CString strLog;
	CString strMessageBox;
};
