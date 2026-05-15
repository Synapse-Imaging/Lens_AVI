#pragma once

#include "LightLibraryDlg.h"

// CLightPageControlDlg 대화 상자입니다.

class CLightPageControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightPageControlDlg)

public:
	CLightPageControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightPageControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_PAGE_CONTROL_DLG };

	CString m_sEditPageDesc;

	CSliderCtrl m_SliderLight1;
	CSliderCtrl m_SliderLight2;
	CSliderCtrl m_SliderLight3;
	CSliderCtrl m_SliderLight4;
	CSliderCtrl m_SliderLight5;
	CSliderCtrl m_SliderLight6;
	CSliderCtrl m_SliderLight7;
	CSliderCtrl m_SliderLight8;
	CSliderCtrl m_SliderLight9;
	CSliderCtrl m_SliderLight10;
	CSliderCtrl m_SliderLight11;
	CSliderCtrl m_SliderLight12;
	CSliderCtrl m_SliderLight13;
	CSliderCtrl m_SliderLight14;
	CSliderCtrl m_SliderLight15;
	CSliderCtrl m_SliderLight16;
	CSliderCtrl m_SliderLight17;
	CSliderCtrl m_SliderLight18;
	CSliderCtrl m_SliderLight19;
	CSliderCtrl m_SliderLight20;
	CSliderCtrl m_SliderLight21;
	CSliderCtrl m_SliderLight22;
	CSliderCtrl m_SliderLight23;
	CSliderCtrl m_SliderLight24;
	CSliderCtrl m_SliderLight25;
	CSliderCtrl m_SliderLight26;
	CSliderCtrl m_SliderLight27;
	CSliderCtrl m_SliderLight28;
	CSliderCtrl m_SliderLight29;
	CSliderCtrl m_SliderLight30;
	CSliderCtrl m_SliderLight31;
	CSliderCtrl m_SliderLight32;

	int m_iEditLight1;
	int m_iEditLight2;
	int m_iEditLight3;
	int m_iEditLight4;
	int m_iEditLight5;
	int m_iEditLight6;
	int m_iEditLight7;
	int m_iEditLight8;
	int m_iEditLight9;
	int m_iEditLight10;
	int m_iEditLight11;
	int m_iEditLight12;
	int m_iEditLight13;
	int m_iEditLight14;
	int m_iEditLight15;
	int m_iEditLight16;
	int m_iEditLight17;
	int m_iEditLight18;
	int m_iEditLight19;
	int m_iEditLight20;
	int m_iEditLight21;
	int m_iEditLight22;
	int m_iEditLight23;
	int m_iEditLight24;
	int m_iEditLight25;
	int m_iEditLight26;
	int m_iEditLight27;
	int m_iEditLight28;
	int m_iEditLight29;
	int m_iEditLight30;
	int m_iEditLight31;
	int m_iEditLight32;

public:
	CLightLibraryDlg *m_pLightLibraryDlg;
	void ShowLightLibraryDlg();
	void HideLightLibraryDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	int m_iRadioPageNumber;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();

protected:
	CButtonCS m_bnChangeLight;
	CButtonCS m_bnExit;
	CButtonCS m_bnShowLibrary;

protected:
	int m_iCurPageIndex;
	void UpdateLightChannelValue();
	BOOL GetUsedImageNumber(int *piUsedImageNumber, int *piUsedSequenceNumber, int *piNoUsedImage);
	afx_msg LRESULT OnEventSelectedListApply(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnChangeEditLight1();
	afx_msg void OnChangeEditLight2();
	afx_msg void OnChangeEditLight3();
	afx_msg void OnChangeEditLight4();
	afx_msg void OnChangeEditLight5();
	afx_msg void OnChangeEditLight6();
	afx_msg void OnChangeEditLight7();
	afx_msg void OnChangeEditLight8();
	afx_msg void OnChangeEditLight9();
	afx_msg void OnChangeEditLight10();
	afx_msg void OnChangeEditLight11();
	afx_msg void OnChangeEditLight12();
	afx_msg void OnChangeEditLight13();
	afx_msg void OnChangeEditLight14();
	afx_msg void OnChangeEditLight15();
	afx_msg void OnChangeEditLight16();
	afx_msg void OnChangeEditLight17();
	afx_msg void OnChangeEditLight18();
	afx_msg void OnChangeEditLight19();
	afx_msg void OnChangeEditLight20();
	afx_msg void OnChangeEditLight21();
	afx_msg void OnChangeEditLight22();
	afx_msg void OnChangeEditLight23();
	afx_msg void OnChangeEditLight24();
	afx_msg void OnChangeEditLight25();
	afx_msg void OnChangeEditLight26();
	afx_msg void OnChangeEditLight27();
	afx_msg void OnChangeEditLight28();
	afx_msg void OnChangeEditLight29();
	afx_msg void OnChangeEditLight30();
	afx_msg void OnChangeEditLight31();
	afx_msg void OnChangeEditLight32();

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonChangeLightvalue();
	int m_iEditComPortNumber;
	afx_msg void OnBnClickedRadioPage1();
	afx_msg void OnBnClickedRadioPage2();
	afx_msg void OnBnClickedRadioPage3();
	afx_msg void OnBnClickedRadioPage4();
	afx_msg void OnBnClickedRadioPage5();
	afx_msg void OnBnClickedRadioPage6();
	afx_msg void OnBnClickedRadioPage7();
	afx_msg void OnBnClickedRadioPage8();
	afx_msg void OnBnClickedRadioPage9();
	afx_msg void OnBnClickedRadioPage10();
	afx_msg void OnBnClickedRadioPage11();
	afx_msg void OnBnClickedRadioPage12();
	afx_msg void OnBnClickedRadioPage13();
	afx_msg void OnBnClickedRadioPage14();
	afx_msg void OnBnClickedRadioPage15();
	afx_msg void OnBnClickedRadioPage16();
	afx_msg void OnBnClickedRadioPage17();
	afx_msg void OnBnClickedRadioPage18();
	afx_msg void OnBnClickedRadioPage19();
	afx_msg void OnBnClickedRadioPage20();
	afx_msg void OnBnClickedRadioPage21();
	afx_msg void OnBnClickedRadioPage22();
	afx_msg void OnBnClickedRadioPage23();
	afx_msg void OnBnClickedRadioPage24();
	afx_msg void OnBnClickedRadioPage25();
	afx_msg void OnBnClickedRadioPage26();
	afx_msg void OnBnClickedRadioPage27();
	afx_msg void OnBnClickedRadioPage28();
	afx_msg void OnBnClickedRadioPage29();
	afx_msg void OnBnClickedRadioPage30();
	afx_msg void OnBnClickedRadioPage31();
	afx_msg void OnBnClickedRadioPage32();

	afx_msg void OnBnClickedButtonShowLibrary();

	void ChangeLanguage();
	void SetDlgStatus();
};
