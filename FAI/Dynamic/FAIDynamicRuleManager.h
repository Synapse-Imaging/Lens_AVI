#pragma once
#include <map>
#include <vector>
#include "FAIDynamicRule.h"
#include "../../Define.h"
#include "../ModelData/FAIMeasureData.h"

// Forward declaration
class Algorithm;

// ============================================================
//  FAIDynamicRuleManager  (Singleton)
//
//  Manages dynamic P5/P7 FAI measurement rules per model type.
//  Rules are persisted in:
//    Data/FaiDynamicRules_{ModelName}.ini
//
//  Usage:
//    auto& mgr = FAIDynamicRuleManager::GetInstance();
//    mgr.LoadRules(_T("BOS"));
//    mgr.ExecuteP5Rules(_T("BOS"), pStruct, bufIdx, paramNo, pAlg);
//    mgr.ExecuteP7Rules(_T("BOS"), pStruct, bufIdx, paramNo, pAlg);
// ============================================================
class FAIDynamicRuleManager
{
public:
    static FAIDynamicRuleManager& GetInstance();

    // ----- Rule I/O ----------------------------------------
    void LoadRules(CString strModelType);
    void SaveRules(CString strModelType);

    // ----- Rule Management ----------------------------------
    void AddP5Rule   (CString strModelType, const FAIDynamicP5Rule& rule);
    void AddP7Rule   (CString strModelType, const FAIDynamicP7Rule& rule);
    void UpdateP5Rule(CString strModelType, int index, const FAIDynamicP5Rule& rule);
    void UpdateP7Rule(CString strModelType, int index, const FAIDynamicP7Rule& rule);
    void RemoveP5Rule(CString strModelType, int index);
    void RemoveP7Rule(CString strModelType, int index);
    void ClearRules  (CString strModelType);

    const std::vector<FAIDynamicP5Rule>& GetP5Rules(CString strModelType) const;
    const std::vector<FAIDynamicP7Rule>& GetP7Rules(CString strModelType) const;

    // ----- Execution ----------------------------------------
    // Called at the end of each Inspector::Inspect() to apply dynamic rules.
    void ExecuteP5Rules(CString strModelType, CenterlineMeasureStruct* pStruct,
                        int iBufferIdx, int iParamNo, Algorithm* pAlgorithm);
    void ExecuteP7Rules(CString strModelType, CenterlineMeasureStruct* pStruct,
                        int iBufferIdx, int iParamNo, Algorithm* pAlgorithm);

    // Returns the INI file path for a given model
    static CString GetIniPath(CString strModelType);

private:
    FAIDynamicRuleManager()  = default;
    ~FAIDynamicRuleManager() = default;
    FAIDynamicRuleManager(const FAIDynamicRuleManager&)            = delete;
    FAIDynamicRuleManager& operator=(const FAIDynamicRuleManager&) = delete;

    std::map<CString, std::vector<FAIDynamicP5Rule>> m_p5Rules;
    std::map<CString, std::vector<FAIDynamicP7Rule>> m_p7Rules;

    // Empty rule vectors returned when a model has no rules loaded
    static const std::vector<FAIDynamicP5Rule> s_emptyP5;
    static const std::vector<FAIDynamicP7Rule> s_emptyP7;
};
