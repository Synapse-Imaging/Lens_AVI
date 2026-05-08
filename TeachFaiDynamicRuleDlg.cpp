// TeachFaiDynamicRuleDlg.cpp
//
// TODO: Resource Editor에서 IDD_TEACH_FAI_DYNAMIC_RULE_DLG (ID=419) 다이얼로그 추가 필요
//       - IDC_LIST_DYNAMIC_P5_RULES (8628) : CListCtrl
//       - IDC_LIST_DYNAMIC_P7_RULES (8629) : CListCtrl
//       - IDC_BTN_DYN_ADD_P5 (8630) / IDC_BTN_DYN_DEL_P5 (8631)
//       - IDC_BTN_DYN_ADD_P7 (8632) / IDC_BTN_DYN_DEL_P7 (8633)
//       - IDC_BTN_DYN_SAVE (8634) / IDC_BTN_DYN_CLOSE (8635)

#include "stdafx.h"
#include "uScan.h"
#include "TeachFaiDynamicRuleDlg.h"

IMPLEMENT_DYNAMIC(CTeachFaiDynamicRuleDlg, CDialog)

CTeachFaiDynamicRuleDlg::CTeachFaiDynamicRuleDlg(CString strModelType, CWnd* pParent)
    : CDialog(IDD_TEACH_FAI_DYNAMIC_RULE_DLG, pParent)
    , m_modelType(strModelType)
{
}

void CTeachFaiDynamicRuleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_DYNAMIC_P5_RULES, m_listP5);
    DDX_Control(pDX, IDC_LIST_DYNAMIC_P7_RULES, m_listP7);
}

BEGIN_MESSAGE_MAP(CTeachFaiDynamicRuleDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_DYN_ADD_P5,  &CTeachFaiDynamicRuleDlg::OnBnClickedBtnAddP5)
    ON_BN_CLICKED(IDC_BTN_DYN_DEL_P5,  &CTeachFaiDynamicRuleDlg::OnBnClickedBtnDelP5)
    ON_BN_CLICKED(IDC_BTN_DYN_ADD_P7,  &CTeachFaiDynamicRuleDlg::OnBnClickedBtnAddP7)
    ON_BN_CLICKED(IDC_BTN_DYN_DEL_P7,  &CTeachFaiDynamicRuleDlg::OnBnClickedBtnDelP7)
    ON_BN_CLICKED(IDC_BTN_DYN_SAVE,    &CTeachFaiDynamicRuleDlg::OnBnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_DYN_CLOSE,   &CTeachFaiDynamicRuleDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()

BOOL CTeachFaiDynamicRuleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: P5 리스트 컬럼 설정
    // m_listP5.InsertColumn(0, _T("FAI#"),  LVCFMT_LEFT, 50);
    // m_listP5.InsertColumn(1, _T("Type"),  LVCFMT_LEFT, 80);
    // m_listP5.InsertColumn(2, _T("PosA"),  LVCFMT_LEFT, 120);
    // m_listP5.InsertColumn(3, _T("IdxA"),  LVCFMT_LEFT, 40);
    // m_listP5.InsertColumn(4, _T("PosB"),  LVCFMT_LEFT, 120);
    // m_listP5.InsertColumn(5, _T("IdxB"),  LVCFMT_LEFT, 40);
    // m_listP5.InsertColumn(6, _T("x"),     LVCFMT_LEFT, 60);
    // m_listP5.InsertColumn(7, _T("offset"),LVCFMT_LEFT, 60);

    // TODO: P7 리스트 컬럼 설정 (유사)

    // 기존 규칙 로드 후 리스트 갱신
    FAIDynamicRuleManager::GetInstance().LoadRules(m_modelType);
    RefreshP5List();
    RefreshP7List();

    return TRUE;
}

void CTeachFaiDynamicRuleDlg::RefreshP5List()
{
    // TODO: m_listP5 내용 갱신
    // const auto& rules = FAIDynamicRuleManager::GetInstance().GetP5Rules(m_modelType);
    // for (int i = 0; i < rules.size(); i++) { ... InsertItem / SetItemText ... }
}

void CTeachFaiDynamicRuleDlg::RefreshP7List()
{
    // TODO: m_listP7 내용 갱신
}

CString CTeachFaiDynamicRuleDlg::PosName(enMeasurePos pos) const
{
    // TODO: FAIModelConfig의 measurePositions에서 displayName 검색
    CString str;
    str.Format(_T("Pos%d"), MtoI(pos));
    return str;
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnAddP5()
{
    // TODO: 입력 다이얼로그 또는 인라인 편집으로 신규 P5 규칙 추가
    // FAIDynamicP5Rule rule;
    // rule.faiId = ...; rule.posA = ...; ...
    // FAIDynamicRuleManager::GetInstance().AddP5Rule(m_modelType, rule);
    // RefreshP5List();
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnDelP5()
{
    // TODO: 선택된 항목 삭제
    // int sel = m_listP5.GetNextItem(-1, LVNI_SELECTED);
    // if (sel >= 0) FAIDynamicRuleManager::GetInstance().RemoveP5Rule(m_modelType, sel);
    // RefreshP5List();
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnAddP7()
{
    // TODO: 신규 P7 규칙 추가
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnDelP7()
{
    // TODO: 선택된 P7 규칙 삭제
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnSave()
{
    FAIDynamicRuleManager::GetInstance().SaveRules(m_modelType);
    MessageBox(_T("Dynamic rules saved."), _T("Save"), MB_OK | MB_ICONINFORMATION);
}

void CTeachFaiDynamicRuleDlg::OnBnClickedBtnClose()
{
    EndDialog(IDCANCEL);
}
