#pragma once

#include "HWinPanel.h"

// CAlgorithmOCRDlg 대화 상자입니다.
#define FONT_IAMGE_COUNT				30

class CAlgorithmOCRDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmOCRDlg)

public:
	CAlgorithmOCRDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAlgorithmOCRDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	void UpdateFontList();
	void Reset();

	void LoadFontFile(CString FontPathName);
	void SaveFontFile(CString FontPathName);

	bool TrainOCR_SVM(CString FontName, bool bCreate);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_OCR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	GTRegion	*m_pSelectedROI;
	int			m_iSelectedInspectionTabIndex;

	bool mbSelectImageIndex[FONT_IAMGE_COUNT];
	bool mbImageValid[FONT_IAMGE_COUNT];
	int miCurImageCount;

	int miCount_A;
	int miCount_B;	
	int miCount_C;	
	int miCount_D;	
	int miCount_E;	
	int miCount_F;	
	int miCount_G;	
	int miCount_H;	
	int miCount_I;	
	int miCount_J;	
	int miCount_K;	
	int miCount_L;	
	int miCount_M;	
	int miCount_N;	
	int miCount_O;	
	int miCount_P;	
	int miCount_Q;	
	int miCount_R;	
	int miCount_S;	
	int miCount_T;	
	int miCount_U;	
	int miCount_V;	
	int miCount_W;	
	int miCount_X;	
	int miCount_Y;	
	int miCount_Z;	
	int miCount_0;	
	int miCount_1;	
	int miCount_2;	
	int miCount_3;	
	int miCount_4;	
	int miCount_5;	
	int miCount_6;	
	int miCount_7;	
	int miCount_8;	
	int miCount_9;	
	
	HObject* mp_HCharImage_A;
	HObject* mp_HCharImage_B;
	HObject* mp_HCharImage_C;
	HObject* mp_HCharImage_D;
	HObject* mp_HCharImage_E;
	HObject* mp_HCharImage_F;
	HObject* mp_HCharImage_G;
	HObject* mp_HCharImage_H;
	HObject* mp_HCharImage_I;
	HObject* mp_HCharImage_J;
	HObject* mp_HCharImage_K;
	HObject* mp_HCharImage_L;
	HObject* mp_HCharImage_M;
	HObject* mp_HCharImage_N;
	HObject* mp_HCharImage_O;
	HObject* mp_HCharImage_P;
	HObject* mp_HCharImage_Q;
	HObject* mp_HCharImage_R;
	HObject* mp_HCharImage_S;
	HObject* mp_HCharImage_T;
	HObject* mp_HCharImage_U;
	HObject* mp_HCharImage_V;
	HObject* mp_HCharImage_W;
	HObject* mp_HCharImage_X;
	HObject* mp_HCharImage_Y;
	HObject* mp_HCharImage_Z;
	HObject* mp_HCharImage_0;
	HObject* mp_HCharImage_1;
	HObject* mp_HCharImage_2;
	HObject* mp_HCharImage_3;
	HObject* mp_HCharImage_4;
	HObject* mp_HCharImage_5;
	HObject* mp_HCharImage_6;
	HObject* mp_HCharImage_7;
	HObject* mp_HCharImage_8;
	HObject* mp_HCharImage_9;

	BOOL m_bCheckUseOCR;

	CListBox m_FontListBox;
	int m_iRadioCharIndex;

	CButtonCS m_bnFontAdd;
	CButtonCS m_bnFontDelete;
	CButtonCS m_bnFontNew;
	CButtonCS m_bnFontSave;

	CButtonCS m_bnFontImageAllDelete;
	CButtonCS m_bnFontImageDelete;
	CButtonCS m_bnFontImageNext;
	CButtonCS m_bnFontImagePrev;

	CStatic m_stFontCount; 
	CStatic m_stFontList;

	CHWinPanel m_HWinPanel1;
	CHWinPanel m_HWinPanel2;
	CHWinPanel m_HWinPanel3;
	CHWinPanel m_HWinPanel4;
	CHWinPanel m_HWinPanel5;
	CHWinPanel m_HWinPanel6;
	CHWinPanel m_HWinPanel7;
	CHWinPanel m_HWinPanel8;
	CHWinPanel m_HWinPanel9;
	CHWinPanel m_HWinPanel10;
	CHWinPanel m_HWinPanel11;
	CHWinPanel m_HWinPanel12;
	CHWinPanel m_HWinPanel13;
	CHWinPanel m_HWinPanel14;
	CHWinPanel m_HWinPanel15;
	CHWinPanel m_HWinPanel16;
	CHWinPanel m_HWinPanel17;
	CHWinPanel m_HWinPanel18;
	CHWinPanel m_HWinPanel19;
	CHWinPanel m_HWinPanel20;
	CHWinPanel m_HWinPanel21;
	CHWinPanel m_HWinPanel22;
	CHWinPanel m_HWinPanel23;
	CHWinPanel m_HWinPanel24;
	CHWinPanel m_HWinPanel25;
	CHWinPanel m_HWinPanel26;
	CHWinPanel m_HWinPanel27;
	CHWinPanel m_HWinPanel28;
	CHWinPanel m_HWinPanel29;
	CHWinPanel m_HWinPanel30;
public:
	long m_lEditCharPlateWidth;
	long m_lEditMinCharWidth;
	long m_lEditMaxCharWidth;
	long m_lEditMinCharHeight;
	long m_lEditMaxCharHeight;
	long m_lEditMinCharCandidateHeight;
	long m_lEditWholeAreaDivide;
	long m_lEditHorizontalDivide;
	long m_lEditFirstDiffernt;
	long m_lEditSecondDifferent;
	long m_lEditdiff;
	int m_iEditOCRSegmentDilation;

	CString strLog;

virtual BOOL OnInitDialog();
	afx_msg void OnClickPageNumber(UINT nID); 

	afx_msg void OnPaint();
	afx_msg void OnSelchangeListFont();

	afx_msg void OnBnClickedButtonFontimagePrev();
	afx_msg void OnBnClickedButtonFontimageNext();
	afx_msg void OnBnClickedButtonFontimageAlldelete();
	afx_msg void OnBnClickedButtonFontNew();
	afx_msg void OnBnClickedButtonFontAdd();
	afx_msg void OnBnClickedButtonFontDelete();
	afx_msg void OnBnClickedButtonFontSave();
	afx_msg void OnStnClickedStatic1();
	afx_msg void OnStnClickedStatic2();
	afx_msg void OnStnClickedStatic3();
	afx_msg void OnStnClickedStatic4();
	afx_msg void OnStnClickedStatic5();
	afx_msg void OnStnClickedStatic6();
	afx_msg void OnStnClickedStatic7();
	afx_msg void OnStnClickedStatic8();
	afx_msg void OnStnClickedStatic9();
	afx_msg void OnStnClickedStatic10();
	afx_msg void OnStnClickedStatic11();
	afx_msg void OnStnClickedStatic12();
	afx_msg void OnStnClickedStatic13();
	afx_msg void OnStnClickedStatic14();
	afx_msg void OnStnClickedStatic15();
	afx_msg void OnStnClickedStatic16();
	afx_msg void OnStnClickedStatic17();
	afx_msg void OnStnClickedStatic18();
	afx_msg void OnStnClickedStatic19();
	afx_msg void OnStnClickedStatic20();
	afx_msg void OnStnClickedStatic21();
	afx_msg void OnStnClickedStatic22();
	afx_msg void OnStnClickedStatic23();
	afx_msg void OnStnClickedStatic24();
	afx_msg void OnStnClickedStatic25();
	afx_msg void OnStnClickedStatic26();
	afx_msg void OnStnClickedStatic27();
	afx_msg void OnStnClickedStatic28();
	afx_msg void OnStnClickedStatic29();
	afx_msg void OnStnClickedStatic30();
	afx_msg void OnBnClickedButtonFontimageDelete();
	HObject CharSegmentAlgoritm(HObject& hOCRImage, HObject HROIHRegion);
	afx_msg void OnBnClickedButtonFontNew2();
	void RegisterCharacter(CString strChar, CString FontName);
};
