#pragma once
#include "FAI/Dynamic/FAIDynamicRuleManager.h"
#include "FAI/Dynamic/FAIDynamicRule.h"
#include "FAI/ModelData/FAIModelConfig.h"

// ============================================================
//  CTeachFaiDynamicRuleDlg
//
//  UI dialog for managing dynamic P5/P7 FAI measurement rules.
//  Opened from each TeachFai*Dlg via the "Dynamic Rules" button.
//
//  - Shows P5 (distance/radius) rules in top list control
//  - Shows P7 (angle) rules in bottom list control
//  - Add/Delete per list; Save writes to INI file immediately
// ============================================================
class CTeachFaiDynamicRuleDlg : public CDialog
{
    DECLARE_DYNAMIC(CTeachFaiDynamicRuleDlg)

public:
    explicit CTeachFaiDynamicRuleDlg(CString strModelType, CWnd* pParent = nullptr);
    virtual ~CTeachFaiDynamicRuleDlg() = default;

    enum { IDD = IDD_TEACH_FAI_DYNAMIC_RULE_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    DECLARE_MESSAGE_MAP()

private:
    CString m_modelType;

    // List controls (columns defined in OnInitDialog)
    CListCtrl m_listP5;
    CListCtrl m_listP7;

    // Buttons
    CButton m_btnAddP5;
    CButton m_btnDelP5;
    CButton m_btnAddP7;
    CButton m_btnDelP7;
    CButton m_btnSave;
    CButton m_btnClose;

    // Helpers
    void RefreshP5List();
    void RefreshP7List();
    CString PosName(enMeasurePos pos) const;  // Returns display name for enMeasurePos

    // Populate combo entries for FAI position combo (used in Add dialogs)
    static const std::vector<MeasurePosDescriptor>& GetModelPositions(CString strModelType);

public:
    afx_msg void OnBnClickedBtnAddP5();
    afx_msg void OnBnClickedBtnDelP5();
    afx_msg void OnBnClickedBtnAddP7();
    afx_msg void OnBnClickedBtnDelP7();
    afx_msg void OnBnClickedBtnSave();
    afx_msg void OnBnClickedBtnClose();
};
