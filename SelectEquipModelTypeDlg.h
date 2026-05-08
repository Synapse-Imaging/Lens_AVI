#pragma once


// CSelectEquipModelTypeDlg 대화 상자입니다.

class CSelectEquipModelTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectEquipModelTypeDlg)

public:
	CSelectEquipModelTypeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectEquipModelTypeDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECT_EQUIP_MODEL_TYPE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	int m_iRadioEquipModelType;

	void ChangeLanguage();
};
