#pragma once


// CDefectPriorityDlg 대화 상자입니다.

class CDefectPriorityDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefectPriorityDlg)

public:
	CDefectPriorityDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDefectPriorityDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PRIORITY_DEFECT_DLG };

	CComboBox m_cbPriority[MAX_DEFECT_NAME];
	int		m_iPriority[MAX_DEFECT_NAME];
	CComboBox m_cbSpecialNGCode[MAX_DEFECT_NAME];

	void SetInspectionType(int iInspectionType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSelchangeComboPriority_1();
	afx_msg void OnSelchangeComboPriority_2();
	afx_msg void OnSelchangeComboPriority_3();
	afx_msg void OnSelchangeComboPriority_4();
	afx_msg void OnSelchangeComboPriority_5();
	afx_msg void OnSelchangeComboPriority_6();
	afx_msg void OnSelchangeComboPriority_7();
	afx_msg void OnSelchangeComboPriority_8();
	afx_msg void OnSelchangeComboPriority_9();
	afx_msg void OnSelchangeComboPriority_10();
	afx_msg void OnSelchangeComboPriority_11();
	afx_msg void OnSelchangeComboPriority_12();
	afx_msg void OnSelchangeComboPriority_13();
	afx_msg void OnSelchangeComboPriority_14();
	afx_msg void OnSelchangeComboPriority_15();
	afx_msg void OnSelchangeComboPriority_16();
	afx_msg void OnSelchangeComboPriority_17();
	afx_msg void OnSelchangeComboPriority_18();
	afx_msg void OnSelchangeComboPriority_19();
	afx_msg void OnSelchangeComboPriority_20();
	afx_msg void OnSelchangeComboPriority_21();
	afx_msg void OnSelchangeComboPriority_22();
	afx_msg void OnSelchangeComboPriority_23();
	afx_msg void OnSelchangeComboPriority_24();
	afx_msg void OnSelchangeComboPriority_25();
	afx_msg void OnSelchangeComboPriority_26();
	afx_msg void OnSelchangeComboPriority_27();
	afx_msg void OnSelchangeComboPriority_28();
	afx_msg void OnSelchangeComboPriority_29();
	afx_msg void OnSelchangeComboPriority_30();
	afx_msg void OnSelchangeComboPriority_31();
	afx_msg void OnSelchangeComboPriority_32();
	afx_msg void OnSelchangeComboPriority_33();
	afx_msg void OnSelchangeComboPriority_34();
	afx_msg void OnSelchangeComboPriority_35();
	afx_msg void OnSelchangeComboPriority_36();
	afx_msg void OnSelchangeComboPriority_37();
	afx_msg void OnSelchangeComboPriority_38();
	afx_msg void OnSelchangeComboPriority_39();
	afx_msg void OnSelchangeComboPriority_40();
	afx_msg void OnSelchangeComboPriority_41();
	afx_msg void OnSelchangeComboPriority_42();
	afx_msg void OnSelchangeComboPriority_43();
	afx_msg void OnSelchangeComboPriority_44();
	afx_msg void OnSelchangeComboPriority_45();
	afx_msg void OnSelchangeComboPriority_46();
	afx_msg void OnSelchangeComboPriority_47();
	afx_msg void OnSelchangeComboPriority_48();
	afx_msg void OnSelchangeComboPriority_49();
	afx_msg void OnSelchangeComboPriority_50();
	afx_msg void OnSelchangeComboPriority_51();
	afx_msg void OnSelchangeComboPriority_52();
	afx_msg void OnSelchangeComboPriority_53();
	afx_msg void OnSelchangeComboPriority_54();
	afx_msg void OnSelchangeComboPriority_55();
	afx_msg void OnSelchangeComboPriority_56();
	afx_msg void OnSelchangeComboPriority_57();
	afx_msg void OnSelchangeComboPriority_58();
	afx_msg void OnSelchangeComboPriority_59();
	afx_msg void OnSelchangeComboPriority_60();
	afx_msg void OnSelchangeComboPriority_61();
	afx_msg void OnSelchangeComboPriority_62();
	afx_msg void OnSelchangeComboPriority_63();
	afx_msg void OnSelchangeComboPriority_64();
	afx_msg void OnSelchangeComboPriority_65();
	afx_msg void OnSelchangeComboPriority_66();
	afx_msg void OnSelchangeComboPriority_67();
	afx_msg void OnSelchangeComboPriority_68();
	afx_msg void OnSelchangeComboPriority_69();
	afx_msg void OnSelchangeComboPriority_70();
	afx_msg void OnSelchangeComboPriority_71();
	afx_msg void OnSelchangeComboPriority_72();
	afx_msg void OnSelchangeComboPriority_73();
	afx_msg void OnSelchangeComboPriority_74();
	afx_msg void OnSelchangeComboPriority_75();
	afx_msg void OnSelchangeComboPriority_76();
	afx_msg void OnSelchangeComboPriority_77();
	afx_msg void OnSelchangeComboPriority_78();
	afx_msg void OnSelchangeComboPriority_79();
	afx_msg void OnSelchangeComboPriority_80();
	afx_msg void OnSelchangeComboPriority_81();
	afx_msg void OnSelchangeComboPriority_82();
	afx_msg void OnSelchangeComboPriority_83();
	afx_msg void OnSelchangeComboPriority_84();
	afx_msg void OnSelchangeComboPriority_85();
	afx_msg void OnSelchangeComboPriority_86();
	afx_msg void OnSelchangeComboPriority_87();
	afx_msg void OnSelchangeComboPriority_88();
	afx_msg void OnSelchangeComboPriority_89();
	afx_msg void OnSelchangeComboPriority_90();
	afx_msg void OnSelchangeComboPriority_91();
	afx_msg void OnSelchangeComboPriority_92();
	afx_msg void OnSelchangeComboPriority_93();
	afx_msg void OnSelchangeComboPriority_94();
	afx_msg void OnSelchangeComboPriority_95();
	afx_msg void OnSelchangeComboPriority_96();
	afx_msg void OnSelchangeComboPriority_97();
	afx_msg void OnSelchangeComboPriority_98();
	afx_msg void OnSelchangeComboPriority_99();
	afx_msg void OnSelchangeComboPriority_100();
	afx_msg void OnSelchangeComboPriority_101();
	afx_msg void OnSelchangeComboPriority_102();
	afx_msg void OnSelchangeComboPriority_103();
	afx_msg void OnSelchangeComboPriority_104();
	afx_msg void OnSelchangeComboPriority_105();
	afx_msg void OnSelchangeComboPriority_106();
	afx_msg void OnSelchangeComboPriority_107();
	afx_msg void OnSelchangeComboPriority_108();
	afx_msg void OnSelchangeComboPriority_109();
	afx_msg void OnSelchangeComboPriority_110();

	void ChangeLanguage();

protected:
	int		m_iCurInspectionType;

	void ChangePriority(int iSelectedIndex);
	void SetDefectName();
};
