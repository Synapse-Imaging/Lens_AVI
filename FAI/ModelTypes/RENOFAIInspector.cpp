#include "stdafx.h"
#include "RENOFAIInspector.h"
#include "../Dynamic/FAIDynamicRuleManager.h"

void RENOFAIInspector::SetAlgorithm(Algorithm* pAlgorithm)
{
    m_pAlgorithm = pAlgorithm;
}

/**
 * @brief 검사 실행
 * @param bIsAutoInsp 자동 검사 여부
 * @param iInspectionBufferIdx 검사 버퍼 인덱스
 * @param iFAIParamNo FAI 파라미터 번호
 */
void RENOFAIInspector::Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{
    // TODO : RENO 검사 로직 구현

    CenterlineMeasureStruct* pStructFAIMeasure = nullptr;

    pStructFAIMeasure = CFAIDataManager::GetInstance().GetMeasurePtr(
        bIsAutoInsp,
        m_pAlgorithm->m_nMzNo[iInspectionBufferIdx],
        m_pAlgorithm->m_nTrayNo[iInspectionBufferIdx],
        m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx],
        iFAIParamNo);

    if (!m_pAlgorithm || !pStructFAIMeasure)
    {
        OutputDebugString(_T("[RENOFAIInspector] Algorithm instance or FAI Measure Struct is null. FAI Inspection aborted.\n"));
        return;
    }

    // 기존 코드와 호환 목적
    int m_iVisionCamType = m_pAlgorithm->m_iVisionCamType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Measurement
    double* dMultiple, * dOffset;

#ifdef INLINE_MODE
    if (m_pAlgorithm->m_nStageNo[iInspectionBufferIdx] == STAGE_NUMBER_2)
    {
        dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai;
        dOffset = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai;
    }
    else
    {
        dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai;
        dOffset = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai;
    }
#else
    if ((m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx] % 2) == 0)
    {
        dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai;
        dOffset = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai;
    }
    else
    {
        dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai;
        dOffset = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai;
    }
#endif
    return; // 검사 없음

    DPOINT dPoint1, dPoint2;
    int iFAINumber;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Fit Circle
    // 원 둘레 점들의 좌표값 Concat
    HTuple HEdgeConcatX, HEdgeConcatY;
    for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
    {
        dPoint1.x = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].x;
        dPoint1.y = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].y;

        if (dPoint1.x >= 0 && dPoint1.y >= 0)
        {
            TupleConcat(HEdgeConcatX, HTuple(dPoint1.x), &HEdgeConcatX);
            TupleConcat(HEdgeConcatY, HTuple(dPoint1.y), &HEdgeConcatY);

            // UI 리뷰용 XLD 생성
            // measurePointType : enMeasurePos::MEASURE_TOP_LENS_INNER
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
                      pStructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii],
                      &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }
    }

    HTuple HNoCircleFitEdge;
    HTuple HDistance;
    TupleLength(HEdgeConcatX, &HNoCircleFitEdge);
    int iNoEdge = HNoCircleFitEdge.L();
    double dCircleCenterX;
    double dCircleCenterY;
    double dCircleRadius;

    // 원 피팅 점 없을 시 처리 (잠시 주석처리)
    // if (iNoEdge != MAX_FAI_CIRCLE_FIT_POINT)
    // {
    // 	OutputDebugString(_T("FAI_CIRCLE_FIT_POINT 에러\r\n"));
    // 	return FALSE;
    // }

    // 초기 반지름 계산
    double dNormalRadius = 0;
    for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
    {
        dPoint1.x = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].x;
        dPoint1.y = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].y;

        dPoint2.x = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
        dPoint2.y = pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

        // Distance Point to Point
        DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
        dNormalRadius += HDistance.D();
    }
    dNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
    dNormalRadius *= 0.5;

    // LeastSquareCircleFitting : 해당 점들로 원 피팅
    BOOL bCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HEdgeConcatX, HEdgeConcatY, dNormalRadius,
                                                               &dCircleCenterX, &dCircleCenterY, &dCircleRadius);

    // 원 중심에 윤곽선 그리기
    HObject HCircleCPoint;
    GenCrossContourXld(&HCircleCPoint, dCircleCenterY, dCircleCenterX, 30, 0);
    ConcatObj(
        m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
        HCircleCPoint,
        &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

    // FAI-4 : 렌즈 지름
    pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].x = dCircleCenterX;
    pStructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].y = dCircleCenterY;

    // TODO : 아래 구문 깔끔하게 정리.
    // FAI-4 지름 계산
    double dPixelSize = THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize();
    double dDiameter = dCircleRadius * 2 * dPixelSize * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
    double dFAI4 = dDiameter;

    pStructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dFAI4;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
