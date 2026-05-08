#pragma once
#include "afxwin.h"


// CMacroPathList 대화 상자입니다.

class CMacroPathList : public CDialogEx
{
	DECLARE_DYNAMIC(CMacroPathList)

public:
	CMacroPathList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMacroPathList();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_MACRO_PATH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboLotIdMode();
	afx_msg void OnBnClickedButtonMacroPathSave();
	afx_msg void OnBnClickedButtonMacroPathSaveClose();
	afx_msg void OnBnClickedButtonMakeINI();
	afx_msg void OnBnClickedButtonMakeAndInspect();
	afx_msg void OnBnClickedCancel();

	CComboBox m_comboLotIdMode;
	CString m_strMacroLotMode;
	bool bLot_ID;

	CString m_strLotIdLength;
	int iLotIdLength;

	CString m_strMacroPath[PATH_LIST_NUM];

	void GetAdjFileList(CString strFolder);
	void TraySorting();
	CString m_strLotPath[9000];
	CString m_strLotFolderName[9000];
	CString m_strTrayNumber[9000];

	int g_npathnum;
	int cut;

	bool bAutoStart;

	CString strSectionLotMode, strTempLotMode;
	CString strSectionLotId, strTempLotId;
	CString strSectionMacroPath, strTempLotMacroPath;
	CString strSectionMacroLot, strTempMacroLot;

	CString strArrayTemp[PATH_LIST_NUM], strArrayPath[PATH_LIST_NUM];
	CString strArrayTemp_A[PATH_LIST_NUM], strArrayPath_A[PATH_LIST_NUM];
	CString strArrayTemp_M[PATH_LIST_NUM], strArrayPath_M[PATH_LIST_NUM];

	void ChangeLanguage();
};
