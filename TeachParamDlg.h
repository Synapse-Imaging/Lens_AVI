#pragma once

// CTeachParamDlg 대화 상자입니다.
#include "TeachParamGridCtrl.h"

class CTeachParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachParamDlg)

public:
	CTeachParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachParamDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_PARAM_DLG };

	static CTeachParamDlg	*m_pInstance;

	static	CTeachParamDlg* GetInstance(BOOL bShowFlag = FALSE);
	void	DeleteInstance();
	void Show();

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg LRESULT OnEventROISelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventROIInspectionTabSave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	int	m_iPrevAlgorithmTabIndex;
	int m_iCurrentAlgorithmTabIdx;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedButtonAlgorithmTab1();
	afx_msg void OnBnClickedButtonAlgorithmTab2();
	afx_msg void OnBnClickedButtonAlgorithmTab3();
	afx_msg void OnBnClickedButtonAlgorithmTab4();
	afx_msg void OnBnClickedButtonAlgorithmTab5();
	afx_msg void OnBnClickedButtonAlgorithmTab6();
	afx_msg void OnBnClickedButtonAlgorithmTab7();
	afx_msg void OnBnClickedButtonInspectAreaUpdate();
	afx_msg void OnBnClickedButtonSetAlgorithmParm();
	afx_msg void OnBnClickedButtonAlgorithmTestTab();
	afx_msg void OnBnClickedButtonAlgorithmTestRoi();
	afx_msg void OnBnClickedButtonAlgorithmTestImage();

	CButtonCS m_ButtonSetAlgorithmTab1;
	CButtonCS m_ButtonSetAlgorithmTab2;
	CButtonCS m_ButtonSetAlgorithmTab3;
	CButtonCS m_ButtonSetAlgorithmTab4;
	CButtonCS m_ButtonSetAlgorithmTab5;
	CButtonCS m_ButtonSetAlgorithmTab6;
	CButtonCS m_ButtonSetAlgorithmTab7;
	CStaticCS m_staticInspectApply;
	CStaticCS m_staticDefectType;
	CButtonCS m_ButtonInspectAreaUpdate;
	CButtonCS m_ButtonSetAlgorithmParam;
	CButtonCS m_ButtonAlgorithmTestTab;
	CButtonCS m_ButtonAlgorithmTestRoi;
	CButtonCS m_ButtonAlgorithmTestImage;
	CMFCButton m_ButtonSetAISet1;
	CMFCButton m_ButtonSetAISet2;
	CMFCButton m_ButtonSetAISet3;
	CStaticCS m_staticTabInspectApply;

	BOOL m_bCheckUseInspect;

	BOOL m_bCheckUseFilter1;
	BOOL m_bCheckUseFilter2;
	BOOL m_bCheckUseFilter3;
	BOOL m_bCheckUseFilter4;

	BOOL m_bCheckAIUseConsiderImageNo;
	BOOL m_bCheckAIUseConsiderDefectCode;

	CComboBox m_ComboDefectType;

	CTeachParamGridCtrl m_TeachParamGridCtrl;

	BOOL m_bCheckUseInspectArea1;
	int m_iEditArea1Margin1;
	int m_iEditArea1Margin2;
	BOOL m_bCheckUseInspectArea2;
	int m_iEditArea2Margin1;
	int m_iEditArea2Margin2;
	BOOL m_bCheckUseInspectArea3;
	int m_iEditArea3Margin1;
	int m_iEditArea3Margin2;

	BOOL m_bCheckUseInspectTemp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseFilter1Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseFilter2Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseFilter3Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseFilter4Temp[MAX_INSPECTION_TAB];
	int m_iDefectTypeTemp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseInspectArea1Temp[MAX_INSPECTION_TAB];
	int m_iEditArea1Margin1Temp[MAX_INSPECTION_TAB];
	int m_iEditArea1Margin2Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseInspectArea2Temp[MAX_INSPECTION_TAB];
	int m_iEditArea2Margin1Temp[MAX_INSPECTION_TAB];
	int m_iEditArea2Margin2Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckUseInspectArea3Temp[MAX_INSPECTION_TAB];
	int m_iEditArea3Margin1Temp[MAX_INSPECTION_TAB];
	int m_iEditArea3Margin2Temp[MAX_INSPECTION_TAB];
	BOOL m_bCheckAIUseConsiderImageNoTemp[MAX_INSPECTION_TAB];
	BOOL m_bCheckAIUseConsiderDefectCodeTemp[MAX_INSPECTION_TAB];

	CButtonCS m_ButtonInspectStatus[MAX_INSPECTION_TAB];

	void SetParamData(BOOL bAllTab);
	void GetInspectParam(BOOL bAllTab);

	void UpdateUsedAlgorithmList(int iTabIdx);
	void UpdateInspectCheckStatus();
	void AlgorithmParamUpDate(BOOL bFlag, BOOL bPrevTab, int iTabIdx);
	void UpdateDefectTypeComboBox(int iDefectType);
	afx_msg void OnBnClickedButtonImageCompareTrainTab();
	afx_msg void OnBnClickedButtonImageCompareTrainRoi();
	afx_msg void OnBnClickedButtonImageCompareTrainImage();
	afx_msg void OnBnClickedCheckUseInspect();
	afx_msg void OnBnClickedCheckUseFilter1();
	afx_msg void OnBnClickedCheckUseFilter2();
	afx_msg void OnBnClickedCheckUseFilter3();
	afx_msg void OnBnClickedCheckUseFilter4();
	afx_msg void OnBnClickedCheckAIUseConsiderImageNo();
	afx_msg void OnBnClickedCheckAIUseConsiderDefectCode();

	void ChangeLanguage();
private:
	void AISetUpdate(int iTabIdx);

private:
	const COLORREF BUTTON_BLACK_COLOR = RGB(0, 0, 0);
	const COLORREF BUTTON_YELLOW_GREEN_COLOR = RGB(150, 200, 0);
	const COLORREF BUTTON_YELLOW_COLOR = RGB(255, 255, 0);
	const COLORREF BUTTON_LIGHT_YELLOW_COLOR = RGB(255, 255, 150);
	const COLORREF BUTTON_LIGHT_RED_COLOR = RGB(255, 150, 150);
};
