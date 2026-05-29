#include "StdAfx.h"
#include "FAIDataManager.h"
#include "FAIModelConfig.h"
#include "Define.h"
#include "uScan.h"


CFAIDataManager::CFAIDataManager()
{
    m_structTeachingFaiMeasure.ResetFAI();
    memset(m_bFAIUse, FALSE, sizeof(m_bFAIUse));
    memset(m_bMeasurePosFAIUse, FALSE, sizeof(m_bMeasurePosFAIUse));
}

void CFAIDataManager::Initialize()
{
    m_currentModelType = THEAPP.g_strModelTypeName[THEAPP.GetModelType()];
    
    // FAI 사용 플래그 초기화
    for (int i = 0; i < MAX_FAI_ITEM; i++)
    {
        m_bFAIUse[i] = FALSE;
        for (int j = 0; j < MEASURE_POS_TOTAL; j++)
        {
            m_bMeasurePosFAIUse[i][j] = FALSE;
        }
    }
    
    m_measurePosNames.clear();
    m_lotResultHeaders.clear();
    for (int i = 0; i < MEASURE_POS_TOTAL; i++)
    {
        m_faiNumbersAtPos[i].clear();
    }

    
    /////////////////////////////////////////////////////////////////////////////////
    // Model config selection by string
    if (m_currentModelType == _T("BOI"))      m_ModelConfig = CreateBOIModelConfig();
    else if (m_currentModelType == _T("ACT")) m_ModelConfig = CreateACTModelConfig();
    else if (m_currentModelType == _T("AKC")) m_ModelConfig = CreateAKCModelConfig();
    else if (m_currentModelType == _T("ATW-D")) m_ModelConfig = CreateATWModelConfig();
    else if (m_currentModelType == _T("BOS")) m_ModelConfig = CreateBOSModelConfig();
    else if (m_currentModelType == _T("RENO")) m_ModelConfig = CreateBOSModelConfig();
    else if (m_currentModelType == _T("BOS")) m_ModelConfig = CreateBOSModelConfig();
    ApplyModelConfig(m_ModelConfig);
    /////////////////////////////////////////////////////////////////////////////////
}

// FAI 번호를 받아 해당 FAI가 사용되는지 여부 반환 (기존 코드 호환용. 추후 필요없을 시 제거)
BOOL CFAIDataManager::IsFAIUsed(int faiNumber) const
{
    if (faiNumber < 0 || faiNumber >= MAX_FAI_ITEM)
        return FALSE;
    
    return m_bFAIUse[faiNumber];
}

// FAI 번호와 측정 위치를 받아 해당 FAI가 해당 위치에서 사용되는지 여부 반환 (기존 코드 호환용. 추후 필요없을 시 제거)
BOOL CFAIDataManager::IsFAIUsedAtPos(int faiNumber, enMeasurePos measurePos) const
{
    if (faiNumber < 0 || faiNumber >= MAX_FAI_ITEM)
        return FALSE;

    int measurePosIndex = static_cast<int>(measurePos);
    if (measurePosIndex < 0 || measurePosIndex >= MEASURE_POS_TOTAL)
        return FALSE;

    return m_bMeasurePosFAIUse[faiNumber][measurePosIndex];
}

void CFAIDataManager::SetFAIUse(int faiNumber, BOOL bUse)
{
    if (faiNumber >= 0 && faiNumber < MAX_FAI_ITEM)
    {
        m_bFAIUse[faiNumber] = bUse;
    }
}

void CFAIDataManager::SetFAIUsePos(int faiNumber, enMeasurePos measurePos, BOOL bUse)
{
    if (faiNumber < 0 || faiNumber >= MAX_FAI_ITEM)
        return;

    int measurePosIndex = static_cast<int>(measurePos);
    if (measurePosIndex < 0 || measurePosIndex >= MEASURE_POS_TOTAL)
        return;

    m_bMeasurePosFAIUse[faiNumber][measurePosIndex] = bUse;
}

// 모델 별 FAI 설정 적용
// : CuScan::Initialize()에서 호출됨
// (모델 별 FAI 설정에 더해, 공통부분 설정을 아래 함수에서 관리)
void CFAIDataManager::ApplyModelConfig(const FAIModelConfig& m_ModelConfig)
{
    // 1. m_measurePosNames
    for (const auto& mp : m_ModelConfig.measurePositions)
        m_measurePosNames.push_back(mp.displayName);

    // 2. m_lotResultHeaders (모델 불문 공통 헤더 1-9)
    m_lotResultHeaders.push_back(_T("Date"));            // 1
    m_lotResultHeaders.push_back(_T("Time"));            // 2
    m_lotResultHeaders.push_back(_T("Station"));		 // 3
	m_lotResultHeaders.push_back(_T("Machine_Code"));	 // 4
    m_lotResultHeaders.push_back(_T("LotNum"));          // 5
    m_lotResultHeaders.push_back(_T("Config"));          // 6
#ifdef SINGLE_LENS
	m_lotResultHeaders.push_back(_T("TrayID"));            // 7
	m_lotResultHeaders.push_back(_T("TrayNo"));            // 7
#else
    m_lotResultHeaders.push_back(_T("Tray"));            // 7
#endif
    m_lotResultHeaders.push_back(_T("ModuleNo"));        // 8

#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
	m_lotResultHeaders.push_back(_T("Barcode"));         // 9
	m_lotResultHeaders.push_back(_T("StageNo"));         // 10
    m_lotResultHeaders.push_back(_T("JigNo"));           // 11
#endif
    for (int i = 0; i < VISION_NUMBER_MAX; i++)
    {
        // Vision 헤더 추가
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
        if (i > VISION_NUMBER_2)
            break;
#endif
        if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i])
            m_lotResultHeaders.push_back(_T(THEAPP.m_ModelDefineInfo.m_strVisionName[i]));
    }

    for (int i = 0; i < VISION_NUMBER_MAX; i++)
    {
        // AI Step 헤더 추가 (예: "Top AI Step 1", "Top AI Step 2", ...)
#if defined (UAVI_BOI) || defined (UAVI_BOS) || defined (UAVI_KRIOS)
        if (i > VISION_NUMBER_2)
            break;
#endif
        if (THEAPP.Struct_PreferenceStruct.m_bUseVision[i])
        {
            m_lotResultHeaders.push_back(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" AI Step 1"));
            m_lotResultHeaders.push_back(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" AI Step 2"));
            m_lotResultHeaders.push_back(THEAPP.m_ModelDefineInfo.m_strVisionName[i] + _T(" AI Step 3"));
        }
    }

#if !defined(SINGLE_LENS) && !defined(ASSY_LENS)
    m_lotResultHeaders.push_back(_T("FAI_NG_Item"));     // 18

														 // FAI headers
    // 모델 별 FAI 항목 LOG 헤더 추가
    for (const auto& fi : m_ModelConfig.faiItems)
    {
        for (auto headerName : fi.logHeaderNames)
            m_lotResultHeaders.push_back(headerName);
    }

    // Extra headers
    // 모델 별 추가 LOG 헤더
    for (auto extraHeader : m_ModelConfig.extraLogHeaders)
        m_lotResultHeaders.push_back(extraHeader);

    // 3. FAI Usage
    // FAI 항목별 사용 플래그 설정 (FaiUse, FaiUsePos)
    for (const auto& fi : m_ModelConfig.faiItems)
    {
        SetFAIUse(fi.faiNumber, TRUE);
        if (fi.measurePosition != enMeasurePos::MEASURE_POSITION_ETC)
            SetFAIUsePos(fi.faiNumber, fi.measurePosition, TRUE);
    }

    // 4. position -> FAI numbers
    // position별 FAI 번호 매핑 설정
    for (const auto& fi : m_ModelConfig.faiItems)
    {
        int posIdx = MtoI(fi.measurePosition);
        
        if (posIdx >= 0 && posIdx < MEASURE_POS_TOTAL)
        {
            m_faiNumbersAtPos[posIdx].push_back(fi.faiNumber);
        }
    }
#endif
}

// position ID (int) -> FAI number vector 반환
// 해당 포지션에서 사용하는 FAI 번호를 모두 리턴
const std::vector<int>& CFAIDataManager::GetFAINumbersAtPosition(enMeasurePos enPosName) const
{
    static const std::vector<int> empty;
    int posIdx = MtoI(enPosName);
    if (posIdx < 0 || posIdx >= MEASURE_POS_TOTAL)
        return empty;

    return m_faiNumbersAtPos[posIdx];
}
