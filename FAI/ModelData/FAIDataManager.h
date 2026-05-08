#pragma once
#include "StdAfx.h"
#include "FAIMeasureData.h"
#include "FAIModelConfig.h"
#include <vector>
#include <string>

/**
 * @brief FAI Data Manager
 */
class CFAIDataManager
{
private:
    static CFAIDataManager* m_pInstance;

private:
    FAIModelConfig m_ModelConfig;

    void ApplyModelConfig(const FAIModelConfig& m_ModelConfig);

private:
    // FAI 사용 플래그 배열
    BOOL m_bFAIUse[MAX_FAI_ITEM];
    BOOL m_bMeasurePosFAIUse[MAX_FAI_ITEM][MEASURE_POS_TOTAL];
    
    // FAI Measurement data
    CenterlineMeasureStruct m_structFaiMeasure[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][MAX_FAI_PARAMETER];
    CenterlineMeasureStruct m_structTeachingFaiMeasure;

    // =측정 위치 정보
    std::vector<CString> m_measurePosNames;
    
    // LOT 결과 헤더
    std::vector<CString> m_lotResultHeaders;

    // position -> FAI numbers
    std::vector<int> m_faiNumbersAtPos[MEASURE_POS_TOTAL];
    
    // 현재 모델 타입
    CString m_currentModelType;
    
    CFAIDataManager();
    ~CFAIDataManager() = default;
    CFAIDataManager(const CFAIDataManager&) = delete;
    CFAIDataManager& operator=(const CFAIDataManager&) = delete;
    
public:
    static CFAIDataManager& GetInstance()
    {
        static CFAIDataManager instance;
        return instance;
    }
    
    // 초기화
    void Initialize();
    void Reset();
    
    // FAI 사용 플래그 관리
    BOOL IsFAIUsed(int iFaiIdx) const;
    BOOL IsFAIUsedAtPos(int iFaiIdx, enMeasurePos iMeasurePos) const;
    void SetFAIUse(int iFaiIdx, BOOL bUse);
    void SetFAIUsePos(int iFaiIdx, enMeasurePos iMeasurePos, BOOL bUse);
    void SetFAIUsed(int iFaiIdx, BOOL bUse) { SetFAIUse(iFaiIdx, bUse); }
    BOOL IsMeasurePosFAIUsed(int iFaiIdx, enMeasurePos iMeasurePos) const { return IsFAIUsedAtPos(iFaiIdx, iMeasurePos); }
    void SetMeasurePosFAIUsed(int iFaiIdx, enMeasurePos iMeasurePos, BOOL bUse) { SetFAIUsePos(iFaiIdx, iMeasurePos, bUse); }

    // Measurement data access (1-based index)
    CenterlineMeasureStruct& GetMeasure(int iMzNo, int iTrayNo, int iModuleNo, int iFAIParam)
    {
        return m_structFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFAIParam];
    }

    // Measurement data access (0-based index)
    CenterlineMeasureStruct& GetMeasureRaw(int iMzIdx, int iTrayIdx, int iModuleIdx, int iFAIParam)
    {
        return m_structFaiMeasure[iMzIdx][iTrayIdx][iModuleIdx][iFAIParam];
    }

    CenterlineMeasureStruct& GetTeachingMeasure()
    {
        return m_structTeachingFaiMeasure;
    }

    // Raw array access for backward compatibility (CuScanApp::m_StructFaiMeasure alias)
    CenterlineMeasureStruct (&GetRawArray())[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][MAX_FAI_PARAMETER]
    {
        return m_structFaiMeasure;
    }

    CenterlineMeasureStruct* GetMeasurePtr(BOOL bIsAutoInsp, int iMzNo, int iTrayNo, int iModuleNo, int iFAIParam)
    {
        if (bIsAutoInsp)
        {
            // 배열 범위 체크
            // TODO : 에러메세지 추가해야함. 넘어가버리니까 왜 FAI 계산안되는지 헤매게 됨.
            if (iMzNo < 1 || iMzNo > MAX_MAGAZINE_NO ||
                iTrayNo < 1 || iTrayNo > MAX_TRAY_LOT ||
                iModuleNo < 1 || iModuleNo > MAX_MODULE_TRAY ||
                iFAIParam < 0 || iFAIParam >= MAX_FAI_PARAMETER)
            {
                // 범위 초과 시 nullptr 반환
                return nullptr;
            }
            return &m_structFaiMeasure[iMzNo - 1][iTrayNo - 1][iModuleNo - 1][iFAIParam];
        }
        else
            return &m_structTeachingFaiMeasure;
    }

    void ResetMeasure(int iMzIdx, int iTrayIdx, int iModuleIdx, int iFAIParam)
    {
        m_structFaiMeasure[iMzIdx][iTrayIdx][iModuleIdx][iFAIParam].ResetFAI();
    }

    void ResetTeachingMeasure()
    {
        m_structTeachingFaiMeasure.ResetFAI();
    }

    // 배열 이름 짧게 줄이기
    using MeasureArray = CenterlineMeasureStruct[MAX_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_TRAY][MAX_FAI_PARAMETER];
    using FAIUseArray = BOOL[MAX_FAI_ITEM];
    using MeasurePosFAIUseArray = BOOL[MAX_FAI_ITEM][MEASURE_POS_TOTAL];
    
    // 배열 전체에 대한 참조(Reference) 반환함수 (Getter)
    MeasureArray& GetMeasureArray() { return m_structFaiMeasure; }
    FAIUseArray& GetFAIUseArray() { return m_bFAIUse; }
    MeasurePosFAIUseArray& GetMeasurePosFAIUseArray() { return m_bMeasurePosFAIUse; }

    const std::vector<CString>& GetMeasurePosNames() const { return m_measurePosNames; }
    
    // 측정 위치 ID 를 받아 측정 위치 이름(string) 반환
    const CString GetMeasurePosName(int iPositionID) const 
    { 
        for (const auto& descriptor : m_ModelConfig.measurePositions)
        {
            // enum -> int 변환 후 비교
            // enumPosNmae 의 String 반환
            if (descriptor.pos == ItoM(iPositionID))
                return descriptor.displayName;
        }
        return CString();
    }

    // LOT 결과 헤더 수
    int GetLotResultHeaderCount() const { return (int)m_lotResultHeaders.size(); }
    
    // LOT 결과 헤더
    const std::vector<CString>& GetLotResultHeaders() const { return m_lotResultHeaders; }
    
    // position ID (int) -> FAI number verctor
    const std::vector<int>& GetFAINumbersAtPosition(enMeasurePos enPosName) const;

    // 현재 모델 config 반환
    const FAIModelConfig& GetModelConfig() const { return m_ModelConfig; }
};
