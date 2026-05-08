#include "stdafx.h"
#include "uScan.h"
#include "FAIDynamicRuleManager.h"
#include "../../IniFileCS.h"
#include "../../GFunction.h"
#include "../../Algorithm.h"

// Static empty-rule vectors used when a model has no rules yet
const std::vector<FAIDynamicP5Rule> FAIDynamicRuleManager::s_emptyP5;
const std::vector<FAIDynamicP7Rule> FAIDynamicRuleManager::s_emptyP7;

// ============================================================
//  Singleton accessor
// ============================================================
FAIDynamicRuleManager& FAIDynamicRuleManager::GetInstance()
{
    static FAIDynamicRuleManager instance;
    return instance;
}

// ============================================================
//  Helper: INI file path
//  Path: <exe dir>/Data/FaiDynamicRules_{ModelName}.ini
// ============================================================
CString FAIDynamicRuleManager::GetIniPath(CString strModelType)
{
    CString strPath;
    strPath.Format(_T("Data\\FaiDynamicRules_%s.ini"), strModelType.GetString());
    return strPath;
}

// ============================================================
//  LoadRules: INI -> memory
// ============================================================
void FAIDynamicRuleManager::LoadRules(CString strModelType)
{
    CString strIniPath = GetIniPath(strModelType);
    CIniFileCS ini(strIniPath);

    std::vector<FAIDynamicP5Rule>& p5 = m_p5Rules[strModelType];
    std::vector<FAIDynamicP7Rule>& p7 = m_p7Rules[strModelType];
    p5.clear();
    p7.clear();

    // --- P5 Rules ---
    int nP5Count = ini.Get_Integer(_T("P5_Rules"), _T("Count"), 0);
    p5.reserve(nP5Count);
    for (int i = 0; i < nP5Count; i++)
    {
        CString strKey;
        FAIDynamicP5Rule rule;

        strKey.Format(_T("Rule%d_FaiId"), i);
        rule.faiId = ini.Get_Integer(_T("P5_Rules"), strKey, 0);

        strKey.Format(_T("Rule%d_Type"), i);
        rule.type = static_cast<DynamicP5Type>(ini.Get_Integer(_T("P5_Rules"), strKey, 0));

        strKey.Format(_T("Rule%d_PosA"), i);
        rule.posA = ItoM(ini.Get_Integer(_T("P5_Rules"), strKey, MtoI(enMeasurePos::MEASURE_POSITION_NONE)));

        strKey.Format(_T("Rule%d_IdxA"), i);
        rule.idxA = ini.Get_Integer(_T("P5_Rules"), strKey, 0);

        strKey.Format(_T("Rule%d_PosB"), i);
        rule.posB = ItoM(ini.Get_Integer(_T("P5_Rules"), strKey, MtoI(enMeasurePos::MEASURE_POSITION_NONE)));

        strKey.Format(_T("Rule%d_IdxB"), i);
        rule.idxB = ini.Get_Integer(_T("P5_Rules"), strKey, 0);

        strKey.Format(_T("Rule%d_Multiple"), i);
        rule.multiple = ini.Get_Double(_T("P5_Rules"), strKey, 1.0);

        strKey.Format(_T("Rule%d_Offset"), i);
        rule.offset = ini.Get_Double(_T("P5_Rules"), strKey, 0.0);

        p5.push_back(rule);
    }

    // --- P7 Rules ---
    int nP7Count = ini.Get_Integer(_T("P7_Rules"), _T("Count"), 0);
    p7.reserve(nP7Count);
    for (int i = 0; i < nP7Count; i++)
    {
        CString strKey;
        FAIDynamicP7Rule rule;

        strKey.Format(_T("Rule%d_FaiId"), i);
        rule.faiId = ini.Get_Integer(_T("P7_Rules"), strKey, 0);

        for (int j = 0; j < 2; j++)
        {
            strKey.Format(_T("Rule%d_L1Pos%d"), i, j);
            rule.posL1[j] = ItoM(ini.Get_Integer(_T("P7_Rules"), strKey, MtoI(enMeasurePos::MEASURE_POSITION_NONE)));

            strKey.Format(_T("Rule%d_L1Idx%d"), i, j);
            rule.idxL1[j] = ini.Get_Integer(_T("P7_Rules"), strKey, j);

            strKey.Format(_T("Rule%d_L2Pos%d"), i, j);
            rule.posL2[j] = ItoM(ini.Get_Integer(_T("P7_Rules"), strKey, MtoI(enMeasurePos::MEASURE_POSITION_NONE)));

            strKey.Format(_T("Rule%d_L2Idx%d"), i, j);
            rule.idxL2[j] = ini.Get_Integer(_T("P7_Rules"), strKey, j);
        }

        strKey.Format(_T("Rule%d_AbsAngle"), i);
        rule.bAbsAngle = (ini.Get_Integer(_T("P7_Rules"), strKey, 1) != 0);

        strKey.Format(_T("Rule%d_Multiple"), i);
        rule.multiple = ini.Get_Double(_T("P7_Rules"), strKey, 1.0);

        strKey.Format(_T("Rule%d_Offset"), i);
        rule.offset = ini.Get_Double(_T("P7_Rules"), strKey, 0.0);

        p7.push_back(rule);
    }
}

// ============================================================
//  SaveRules: memory -> INI
// ============================================================
void FAIDynamicRuleManager::SaveRules(CString strModelType)
{
    CString strIniPath = GetIniPath(strModelType);
    CIniFileCS ini(strIniPath);

    const auto& p5 = GetP5Rules(strModelType);
    const auto& p7 = GetP7Rules(strModelType);

    // --- P5 Rules ---
    ini.Set_Integer(_T("P5_Rules"), _T("Count"), static_cast<int>(p5.size()));
    for (int i = 0; i < static_cast<int>(p5.size()); i++)
    {
        CString strKey;
        const FAIDynamicP5Rule& rule = p5[i];

        strKey.Format(_T("Rule%d_FaiId"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, rule.faiId);

        strKey.Format(_T("Rule%d_Type"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, static_cast<int>(rule.type));

        strKey.Format(_T("Rule%d_PosA"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, MtoI(rule.posA));

        strKey.Format(_T("Rule%d_IdxA"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, rule.idxA);

        strKey.Format(_T("Rule%d_PosB"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, MtoI(rule.posB));

        strKey.Format(_T("Rule%d_IdxB"), i);
        ini.Set_Integer(_T("P5_Rules"), strKey, rule.idxB);

        strKey.Format(_T("Rule%d_Multiple"), i);
        ini.Set_Double(_T("P5_Rules"), strKey, rule.multiple);

        strKey.Format(_T("Rule%d_Offset"), i);
        ini.Set_Double(_T("P5_Rules"), strKey, rule.offset);
    }

    // --- P7 Rules ---
    ini.Set_Integer(_T("P7_Rules"), _T("Count"), static_cast<int>(p7.size()));
    for (int i = 0; i < static_cast<int>(p7.size()); i++)
    {
        CString strKey;
        const FAIDynamicP7Rule& rule = p7[i];

        strKey.Format(_T("Rule%d_FaiId"), i);
        ini.Set_Integer(_T("P7_Rules"), strKey, rule.faiId);

        for (int j = 0; j < 2; j++)
        {
            strKey.Format(_T("Rule%d_L1Pos%d"), i, j);
            ini.Set_Integer(_T("P7_Rules"), strKey, MtoI(rule.posL1[j]));

            strKey.Format(_T("Rule%d_L1Idx%d"), i, j);
            ini.Set_Integer(_T("P7_Rules"), strKey, rule.idxL1[j]);

            strKey.Format(_T("Rule%d_L2Pos%d"), i, j);
            ini.Set_Integer(_T("P7_Rules"), strKey, MtoI(rule.posL2[j]));

            strKey.Format(_T("Rule%d_L2Idx%d"), i, j);
            ini.Set_Integer(_T("P7_Rules"), strKey, rule.idxL2[j]);
        }

        strKey.Format(_T("Rule%d_AbsAngle"), i);
        ini.Set_Integer(_T("P7_Rules"), strKey, rule.bAbsAngle ? 1 : 0);

        strKey.Format(_T("Rule%d_Multiple"), i);
        ini.Set_Double(_T("P7_Rules"), strKey, rule.multiple);

        strKey.Format(_T("Rule%d_Offset"), i);
        ini.Set_Double(_T("P7_Rules"), strKey, rule.offset);
    }
}

// ============================================================
//  Rule management helpers
// ============================================================
void FAIDynamicRuleManager::AddP5Rule(CString strModelType, const FAIDynamicP5Rule& rule)
{
    m_p5Rules[strModelType].push_back(rule);
}

void FAIDynamicRuleManager::AddP7Rule(CString strModelType, const FAIDynamicP7Rule& rule)
{
    m_p7Rules[strModelType].push_back(rule);
}

void FAIDynamicRuleManager::UpdateP5Rule(CString strModelType, int index, const FAIDynamicP5Rule& rule)
{
    auto& vec = m_p5Rules[strModelType];
    if (index >= 0 && index < static_cast<int>(vec.size()))
        vec[index] = rule;
}

void FAIDynamicRuleManager::UpdateP7Rule(CString strModelType, int index, const FAIDynamicP7Rule& rule)
{
    auto& vec = m_p7Rules[strModelType];
    if (index >= 0 && index < static_cast<int>(vec.size()))
        vec[index] = rule;
}

void FAIDynamicRuleManager::RemoveP5Rule(CString strModelType, int index)
{
    auto& vec = m_p5Rules[strModelType];
    if (index >= 0 && index < static_cast<int>(vec.size()))
        vec.erase(vec.begin() + index);
}

void FAIDynamicRuleManager::RemoveP7Rule(CString strModelType, int index)
{
    auto& vec = m_p7Rules[strModelType];
    if (index >= 0 && index < static_cast<int>(vec.size()))
        vec.erase(vec.begin() + index);
}

void FAIDynamicRuleManager::ClearRules(CString strModelType)
{
    m_p5Rules[strModelType].clear();
    m_p7Rules[strModelType].clear();
}

const std::vector<FAIDynamicP5Rule>& FAIDynamicRuleManager::GetP5Rules(CString strModelType) const
{
    auto it = m_p5Rules.find(strModelType);
    return (it != m_p5Rules.end()) ? it->second : s_emptyP5;
}

const std::vector<FAIDynamicP7Rule>& FAIDynamicRuleManager::GetP7Rules(CString strModelType) const
{
    auto it = m_p7Rules.find(strModelType);
    return (it != m_p7Rules.end()) ? it->second : s_emptyP7;
}

// ============================================================
//  ExecuteP5Rules
//  Computes point-distance or inner-radius for each P5 rule
//  and stores the result in pStruct->m_dFAIMeasureValue[faiId][0].
// ============================================================
void FAIDynamicRuleManager::ExecuteP5Rules(
    CString                  strModelType,
    CenterlineMeasureStruct* pStruct,
    int                      iBufferIdx,
    int                      iParamNo,
    Algorithm*               pAlgorithm)
{
    if (!pStruct || !pAlgorithm)
        return;

    const auto& rules = GetP5Rules(strModelType);
    if (rules.empty())
        return;

    CGFunction& gFunc = *THEAPP.m_pGFunction;

    for (const FAIDynamicP5Rule& rule : rules)
    {
        // Bounds check on faiId
        if (rule.faiId < 0 || rule.faiId >= MAX_FAI_ITEM)
        {
            ATLTRACE(_T("[FAIDynamic] P5 rule skipped: invalid faiId=%d\n"), rule.faiId);
            continue;
        }

        const int iPosA = MtoI(rule.posA);
        const int iPosB = MtoI(rule.posB);

        if (rule.type == DynamicP5Type::POINT_DISTANCE)
        {
            // Validate both center points
            if (iPosA < 0 || iPosA >= MEASURE_POS_TOTAL ||
                iPosB < 0 || iPosB >= MEASURE_POS_TOTAL ||
                rule.idxA < 0 || rule.idxA >= MAX_MEASURE_POINT_NUM ||
                rule.idxB < 0 || rule.idxB >= MAX_MEASURE_POINT_NUM)
                continue;

            const DPOINT& ptA = pStruct->m_dMeasureCPoint[iPosA][rule.idxA];
            const DPOINT& ptB = pStruct->m_dMeasureCPoint[iPosB][rule.idxB];

            if (ptA.x < 0 || ptB.x < 0)
            {
                pStruct->m_dFAIMeasureValue[rule.faiId][0] = FAI_MEASURE_ERROR;
                continue;
            }

            HTuple HDistance;
            // HALCON: Row(Y) first, then Col(X)
            DistancePp(ptA.y, ptA.x, ptB.y, ptB.x, &HDistance);

            double dPixelSize = THEAPP.m_pCalDataService_N[pAlgorithm->m_iVisionCamType]->GetPixelSize();
            pStruct->m_dFAIMeasureValue[rule.faiId][0] =
                HDistance.D() * dPixelSize * UNIT_SCALE_0_001 * rule.multiple + rule.offset;

            // Review XLD: cross markers at both points
            HObject HCrossA, HCrossB;
            GenCrossContourXld(&HCrossA, ptA.y, ptA.x, 20, 0);
            GenCrossContourXld(&HCrossB, ptB.y, ptB.x, 20, 0);
            ConcatObj(pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0],
                      HCrossA,
                      &pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0]);
            ConcatObj(pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0],
                      HCrossB,
                      &pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0]);
        }
        else // INNER_RADIUS
        {
            if (iPosA < 0 || iPosA >= MEASURE_POS_TOTAL ||
                rule.idxA < 0 || rule.idxA >= MAX_MEASURE_POINT_NUM)
                continue;

            const HObject& hRgn = pStruct->m_HMeasureRgn[iPosA][rule.idxA];
            if (gFunc.ValidHRegion(const_cast<HObject&>(hRgn)) != TRUE)
            {
                pStruct->m_dFAIMeasureValue[rule.faiId][0] = FAI_MEASURE_ERROR;
                continue;
            }

            HTuple HRow, HCol, HRadius;
            InnerCircle(hRgn, &HRow, &HCol, &HRadius);

            double dPixelSize = THEAPP.m_pCalDataService_N[pAlgorithm->m_iVisionCamType]->GetPixelSize();
            pStruct->m_dFAIMeasureValue[rule.faiId][0] =
                HRadius.D() * dPixelSize * UNIT_SCALE_0_001 * rule.multiple + rule.offset;

            // Review XLD: region contour
            HObject HContXld;
            GenContourRegionXld(hRgn, &HContXld, "border");
            ConcatObj(pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0],
                      HContXld,
                      &pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0]);
        }
    }
}

// ============================================================
//  ExecuteP7Rules
//  Computes line-line angle (deg) for each P7 rule.
//  Line points are read from m_dMeasureCPoint[pos][idx].
// ============================================================
void FAIDynamicRuleManager::ExecuteP7Rules(
    CString                  strModelType,
    CenterlineMeasureStruct* pStruct,
    int                      iBufferIdx,
    int                      iParamNo,
    Algorithm*               pAlgorithm)
{
    if (!pStruct || !pAlgorithm)
        return;

    const auto& rules = GetP7Rules(strModelType);
    if (rules.empty())
        return;

    for (const FAIDynamicP7Rule& rule : rules)
    {
        if (rule.faiId < 0 || rule.faiId >= MAX_FAI_ITEM)
        {
            ATLTRACE(_T("[FAIDynamic] P7 rule skipped: invalid faiId=%d\n"), rule.faiId);
            continue;
        }

        // Validate all 4 line endpoint positions
        bool bValid = true;
        for (int j = 0; j < 2 && bValid; j++)
        {
            int posL1j = MtoI(rule.posL1[j]);
            int posL2j = MtoI(rule.posL2[j]);
            if (posL1j < 0 || posL1j >= MEASURE_POS_TOTAL ||
                posL2j < 0 || posL2j >= MEASURE_POS_TOTAL ||
                rule.idxL1[j] < 0 || rule.idxL1[j] >= MAX_MEASURE_POINT_NUM ||
                rule.idxL2[j] < 0 || rule.idxL2[j] >= MAX_MEASURE_POINT_NUM)
            {
                bValid = false;
            }
        }
        if (!bValid)
            continue;

        const DPOINT& ptL1S = pStruct->m_dMeasureCPoint[MtoI(rule.posL1[0])][rule.idxL1[0]];
        const DPOINT& ptL1E = pStruct->m_dMeasureCPoint[MtoI(rule.posL1[1])][rule.idxL1[1]];
        const DPOINT& ptL2S = pStruct->m_dMeasureCPoint[MtoI(rule.posL2[0])][rule.idxL2[0]];
        const DPOINT& ptL2E = pStruct->m_dMeasureCPoint[MtoI(rule.posL2[1])][rule.idxL2[1]];

        if (ptL1S.x < 0 || ptL1E.x < 0 || ptL2S.x < 0 || ptL2E.x < 0)
        {
            pStruct->m_dFAIMeasureValue[rule.faiId][0] = FAI_MEASURE_ERROR;
            continue;
        }

        // AngleLl: Row(Y) first for all endpoints
        HTuple HAngle;
        AngleLl(ptL1S.y, ptL1S.x, ptL1E.y, ptL1E.x,
                ptL2S.y, ptL2S.x, ptL2E.y, ptL2E.x,
                &HAngle);
        TupleDeg(HAngle, &HAngle);

        double dAngle = HAngle[0].D();
        if (rule.bAbsAngle)
            dAngle = abs(dAngle);

        pStruct->m_dFAIMeasureValue[rule.faiId][0] = dAngle * rule.multiple + rule.offset;

        // Review XLD: line segments for both lines
        HObject HLine1, HLine2;
        GenRegionLine(&HLine1, ptL1S.y, ptL1S.x, ptL1E.y, ptL1E.x);
        GenRegionLine(&HLine2, ptL2S.y, ptL2S.x, ptL2E.y, ptL2E.x);
        GenContourRegionXld(HLine1, &HLine1, "border");
        GenContourRegionXld(HLine2, &HLine2, "border");
        ConcatObj(pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0],
                  HLine1,
                  &pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0]);
        ConcatObj(pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0],
                  HLine2,
                  &pAlgorithm->m_HReviewXLD_FAI_Item[iBufferIdx][rule.faiId][iParamNo][0]);
    }
}
