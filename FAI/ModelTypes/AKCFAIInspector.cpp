#include "StdAfx.h"
#include "AKCFAIInspector.h"
#include "Algorithm.h"
#include "../Dynamic/FAIDynamicRuleManager.h"
#include "PFunction.h"
#include "uScan.h"

/**
 * @brief 생성자
 * @param pAlgorithm Algorithm 포인터
 */
AKCFAIInspector::AKCFAIInspector(Algorithm *pAlgorithm)
    : m_pAlgorithm(pAlgorithm)
{
}

void AKCFAIInspector::SetAlgorithm(Algorithm *pAlgorithm)
{
    m_pAlgorithm = pAlgorithm;
}

/**
 * @brief 검사 실행
 * @param bIsAutoInsp 자동 검사 여부
 * @param iInspectionBufferIdx 검사 버퍼 인덱스
 * @param iFAIParamNo FAI 파라미터 번호
 */
void AKCFAIInspector::Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{

    CenterlineMeasureStruct *StructFAIMeasure = nullptr;

    StructFAIMeasure = CFAIDataManager::GetInstance().GetMeasurePtr(
        bIsAutoInsp,
        m_pAlgorithm->m_nMzNo[iInspectionBufferIdx],
        m_pAlgorithm->m_nTrayNo[iInspectionBufferIdx],
        m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx],
        iFAIParamNo);

    if (!m_pAlgorithm || !StructFAIMeasure)
    {
        OutputDebugString(_T("[AKCFAIInspector] Algorithm instance or FAI Measure Struct is null. FAI Inspection aborted.\n"));
        return;
    }

    // 기존 코드와 호환 목적
    int m_iVisionCamType = m_pAlgorithm->m_iVisionCamType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Set Blend
    double dBlendOffsetLeft = 0, dBlendOffsetRight = 0;
    DPOINT dRefer1HStartPoint, dRefer1HEndPoint, dRefer2HStartPoint, dRefer2HEndPoint;
    DPOINT dImage1HStartPoint, dImage1HEndPoint, dImage2HStartPoint, dImage2HEndPoint;
    DPOINT dImage1VStartPoint, dImage1VEndPoint, dImage2VStartPoint, dImage2VEndPoint;

    // Blend Image 1 Horizontal
    dImage1HStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ST].x;
    dImage1HStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ST].y;
    dImage1HEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ED].x;
    dImage1HEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][BLEND_POINT_ED].y;

    // Blend Image 1 Vertical
    dImage1VStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_Y][BLEND_POINT_ST].x;
    dImage1VStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_Y][BLEND_POINT_ST].y;
    dImage1VEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_Y][BLEND_POINT_ED].x;
    dImage1VEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_Y][BLEND_POINT_ED].y;

    // Blend Image 2 Horizontal
    dImage2HStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][BLEND_POINT_ST].x;
    dImage2HStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][BLEND_POINT_ST].y;
    dImage2HEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][BLEND_POINT_ED].x;
    dImage2HEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][BLEND_POINT_ED].y;

    // Blend Image 2 Vertical
    dImage2VStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_Y][BLEND_POINT_ST].x;
    dImage2VStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_Y][BLEND_POINT_ST].y;
    dImage2VEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_Y][BLEND_POINT_ED].x;
    dImage2VEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_Y][BLEND_POINT_ED].y;

    // Refer Pos 1 Horizontal
    dRefer1HStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][BLEND_POINT_ST].x;
    dRefer1HStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][BLEND_POINT_ST].y;
    dRefer1HEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][BLEND_POINT_ED].x;
    dRefer1HEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][BLEND_POINT_ED].y;

    // Refer Pos 2 Horizontal
    dRefer2HStartPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][BLEND_POINT_ST].x;
    dRefer2HStartPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][BLEND_POINT_ST].y;
    dRefer2HEndPoint.x = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][BLEND_POINT_ED].x;
    dRefer2HEndPoint.y = StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][BLEND_POINT_ED].y;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Set Axis
    DPOINT dOPoint, dAxisXEnd, dAxisYEnd;
    double dAngle;

    if (m_iVisionCamType == VISION_NUMBER_2) // Top Axis
    {
        dOPoint.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].x;
        dOPoint.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].y;

        dAxisYEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].x;
        dAxisYEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].y;

        dAxisXEnd.Reset();

        m_pAlgorithm->BlendingAlgorithm_Horizontal(
            dRefer1HStartPoint.x, dRefer1HEndPoint.x, dRefer2HStartPoint.x, dRefer2HEndPoint.x,
            dImage1HStartPoint.x, dImage1HEndPoint.x, dImage2HStartPoint.x, dImage2HEndPoint.x,
            dImage1VStartPoint.x, dImage1VStartPoint.y, dImage1VEndPoint.x, dImage1VEndPoint.y,
            dImage2VStartPoint.x, dImage2VStartPoint.y, dImage2VEndPoint.x, dImage2VEndPoint.y,
            dOPoint.x, dOPoint.y, dBlendOffsetLeft, dBlendOffsetRight);

        if (dOPoint.x >= 0 && dOPoint.y >= 0 && dAxisYEnd.x >= 0 && dAxisYEnd.y >= 0)
        {
            m_pAlgorithm->AxisTransAlgorithm(TRUE, 21.364, (dOPoint.x - dBlendOffsetLeft), dOPoint.y, dAxisXEnd.x, dAxisXEnd.y, dAxisYEnd.x, dAxisYEnd.y);
        }
    }
    else if (m_iVisionCamType == VISION_NUMBER_1) // Bottom Axis
    {
        dOPoint.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0].x;
        dOPoint.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0].y;

        dAxisYEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0].x;
        dAxisYEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0].y;

        dAxisXEnd.Reset();

        m_pAlgorithm->BlendingAlgorithm_Horizontal(
            dRefer1HStartPoint.x, dRefer1HEndPoint.x, dRefer2HStartPoint.x, dRefer2HEndPoint.x,
            dImage1HStartPoint.x, dImage1HEndPoint.x, dImage2HStartPoint.x, dImage2HEndPoint.x,
            dImage1VStartPoint.x, dImage1VStartPoint.y, dImage1VEndPoint.x, dImage1VEndPoint.y,
            dImage2VStartPoint.x, dImage2VStartPoint.y, dImage2VEndPoint.x, dImage2VEndPoint.y,
            dOPoint.x, dOPoint.y, dBlendOffsetLeft, dBlendOffsetRight);

        if (dOPoint.x >= 0 && dOPoint.y >= 0 && dAxisYEnd.x >= 0 && dAxisYEnd.y >= 0)
        {
            m_pAlgorithm->AxisTransAlgorithm(TRUE, -21.364, (dOPoint.x - dBlendOffsetLeft), dOPoint.y, dAxisXEnd.x, dAxisXEnd.y, dAxisYEnd.x, dAxisYEnd.y);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Measurement
    int iFAINumber;
    double *dMultiple, *dOffset;

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

    DPOINT dLineStart, dLineEnd;
    DPOINT dPoint1, dPoint2;
    HTuple HDistance;

    if (m_iVisionCamType == VISION_NUMBER_2)
    {
        dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_VCM_BOTTOM)][0].x;
        dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_VCM_BOTTOM)][0].y;

        dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_VCM_BOTTOM)][1].x;
        dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_VCM_BOTTOM)][1].y;

        dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_LENS_INNER)][0].x;
        dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_LENS_INNER)][0].y;

        dPoint2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_LENS_INNER)][1].x;
        dPoint2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SIDE_LENS_INNER)][1].y;

        iFAINumber = 6;
        if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint2.x >= 0)
        {
            DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            DistancePl(dPoint2.y, dPoint2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
        }

        // FAI-43, 44 - psw - 250313
        double dInnerNormalRadius, dInnerCircleRadius;
        double dInnerCircleCenterX, dInnerCircleCenterY;
        HTuple HNoCircleFitEdge;
        int iNoInnerEdge;
        BOOL bInnerCircleFound;
        DPOINT dLineStartTopLeft, dLineEndTopLeft;
        DPOINT dLineStartTopTop, dLineEndTopTop;
        HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
        HObject HLensCPoint;

        // FAI - 14
        iFAINumber = 14;
        for (int iii = 0; iii < MAX_FAI_CIRCLE_FIT_POINT; iii++)
        {
            dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].x;
            dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].y;

            if (dPoint1.x >= 0 && dPoint1.y >= 0)
            {
                TupleConcat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
                TupleConcat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);

                ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
                          StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii],
                          &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            }
        }

        TupleLength(HInnerEdgeConcatX, &HNoCircleFitEdge);
        iNoInnerEdge = HNoCircleFitEdge.L();

        if (iNoInnerEdge == MAX_FAI_CIRCLE_FIT_POINT)
        {
            dInnerNormalRadius = 0;
            for (int iii = 0; iii < (MAX_FAI_CIRCLE_FIT_POINT / 2); iii++)
            {
                dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].x;
                dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii].y;

                dPoint2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
                dPoint2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][iii + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

                DistancePp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
                dInnerNormalRadius += HDistance.D();
            }
            dInnerNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
            dInnerNormalRadius *= 0.5;

            bInnerCircleFound = m_pAlgorithm->LeastSquareCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius,
                                                                       &dInnerCircleCenterX, &dInnerCircleCenterY, &dInnerCircleRadius);

            if (bInnerCircleFound)
            {
                GenCrossContourXld(&HLensCPoint, dInnerCircleCenterY, dInnerCircleCenterX, 30, 0);
                ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HLensCPoint, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

                // FAI-14-X
                iFAINumber = 21;
                DistancePl(dInnerCircleCenterY, dInnerCircleCenterX, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HDistance);
                StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = -HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

                // FAI-14-Y
                iFAINumber = 15;
                DistancePl(dInnerCircleCenterY, dInnerCircleCenterX, dOPoint.y, dOPoint.x, dAxisXEnd.y, dAxisXEnd.x, &HDistance);
                StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

                // FAI-14
                double dFAI14 = 2 * sqrt(pow((abs(StructFAIMeasure->m_dFAIMeasureValue[21][0]) - 8.414), 2) + pow((abs(StructFAIMeasure->m_dFAIMeasureValue[15][0]) - 14.885), 2));

                iFAINumber = 14;
                StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dFAI14 * dMultiple[iFAINumber] + dOffset[iFAINumber];

                // =============================================================================================

                dLineStartTopLeft.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0].x;
                dLineStartTopLeft.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0].y;

                dLineEndTopLeft.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1].x;
                dLineEndTopLeft.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1].y;

                dLineStartTopTop.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0].x;
                dLineStartTopTop.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0].y;

                dLineEndTopTop.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1].x;
                dLineEndTopTop.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1].y;

                // FAI-43 (Lens Center - Enclosure Wall @ East) - 250813. jhkim
                iFAINumber = 43;
                if (dLineStartTopLeft.x >= 0 && dLineEndTopLeft.x >= 0)
                {
                    ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
                    ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

                    DistancePl(dInnerCircleCenterY, dInnerCircleCenterX, dLineStartTopLeft.y, dLineStartTopLeft.x, dLineEndTopLeft.y, dLineEndTopLeft.x, &HDistance);
                    StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
                }

                // FAI-44 (Lens Center - Sidefill @ North) - 250813. jhkim
                iFAINumber = 44;
                if (dLineStartTopTop.x >= 0 && dLineEndTopTop.x >= 0)
                {
                    ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
                    ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

                    DistancePl(dInnerCircleCenterY, dInnerCircleCenterX, dLineStartTopTop.y, dLineStartTopTop.x, dLineEndTopTop.y, dLineEndTopTop.x, &HDistance);
                    StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
                }
            }
        }

        // FAI-126~130
        double dDistanceMin, dPointX_Welding_Spot, dPointY_Welding_Spot, dPointX_Welding_Pocket, dPointY_Welding_Pocket;
        HObject HLineXld, HSpotXld, HPocketXld;
        GenEmptyObj(&HLineXld);

        // FAI-126 (Weld Spot Position @ North-West) - 250813. jhkim
        // FAI-127 (Weld Spot Position @ North-West) - 250813. jhkim
        // FAI-128 (Weld Spot Position @ South-West) - 250813. jhkim
        // FAI-129 (Weld Spot Position @ North-East) - 250813. jhkim
        // FAI-130 (Weld Spot Position @ South-East) - 250813. jhkim
        iFAINumber = 126;
        for (int i = 0; i < 5; i++)
        {
            if ((THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_WELDING_SPOT)][i]) == TRUE) && (THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_WELDING_POCKET)][i]) == TRUE))
            {
                GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_WELDING_SPOT)][i], &HSpotXld, "border");
                GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_WELDING_POCKET)][i], &HPocketXld, "border");

                HLineXld = m_pAlgorithm->DistanceMinPointAlgorithm(HSpotXld, HPocketXld, dPointX_Welding_Spot, dPointY_Welding_Spot, dPointX_Welding_Pocket, dPointY_Welding_Pocket, dDistanceMin);
                StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistanceMin * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

                ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HLineXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            }

            iFAINumber++;
        }
    }
    else if (m_iVisionCamType == VISION_NUMBER_1)
    {
        // FAI-19, 20
        DPOINT dLineStart_VCM_West, dLineEnd_VCM_West;
        DPOINT dLineStart_VCM_North, dLineEnd_VCM_North;

        HTuple HStartPosX_VCM_West, HEndPosX_VCM_West;
        HTuple HStartPosX_VCM_North, HEndPosX_VCM_North;

        dLineStart_VCM_West.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][0].x;
        dLineStart_VCM_West.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][0].y;

        dLineEnd_VCM_West.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][1].x;
        dLineEnd_VCM_West.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][1].y;

        dLineStart_VCM_North.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][0].x;
        dLineStart_VCM_North.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][0].y;

        dLineEnd_VCM_North.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][1].x;
        dLineEnd_VCM_North.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][1].y;

        // FAI-19 (Chassis Outer Wall - Datum @ West) - 250813. jhkim
        iFAINumber = 19;
        if (dLineStart_VCM_North.x >= 0 && dLineStart_VCM_North.y >= 0 && dLineEnd_VCM_North.x >= 0 && dLineEnd_VCM_North.y >= 0)
        {
            DistancePl(dLineStart_VCM_North.y, dLineStart_VCM_North.x, dOPoint.y, dOPoint.x, dAxisXEnd.y, dAxisXEnd.x, &HStartPosX_VCM_North);
            DistancePl(dLineEnd_VCM_North.y, dLineEnd_VCM_North.x, dOPoint.y, dOPoint.x, dAxisXEnd.y, dAxisXEnd.x, &HEndPosX_VCM_North);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = (HStartPosX_VCM_North.D() + HEndPosX_VCM_North.D()) * 0.5 * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_NORTH)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-20 (Enclosure Wall - Datum @ West) - 250813. jhkim
        iFAINumber = 20;
        if (dLineStart_VCM_West.x >= 0 && dLineStart_VCM_West.y >= 0 && dLineEnd_VCM_West.x >= 0 && dLineEnd_VCM_West.y >= 0)
        {
            DistancePl(dLineStart_VCM_West.y, dLineStart_VCM_West.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HStartPosX_VCM_West);
            DistancePl(dLineEnd_VCM_West.y, dLineEnd_VCM_West.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HEndPosX_VCM_West);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = -(HStartPosX_VCM_West.D() + HEndPosX_VCM_West.D()) * 0.5 * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_VCM_WEST)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-71, 113
        HObject HConnCrossXld;
        HTuple HConnectorX, HConnectorY;
        DPOINT dPoint_Conn[8], dCenter_Conn;
        BOOL bFoundAll_Conn = TRUE;

        // FAI-71 (FPCB Connector - Datum Gap Y) - 250813. jhkim
        iFAINumber = 71;
        for (int iii = 0; iii < 8; iii++)
        {
            dPoint_Conn[iii].x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CONNECTOR)][iii].x;
            dPoint_Conn[iii].y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CONNECTOR)][iii].y;

            if (dPoint_Conn[iii].x >= 0 && dPoint_Conn[iii].y >= 0)
                ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_CONNECTOR)][iii], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            else
                bFoundAll_Conn = FALSE;
        }

        if (bFoundAll_Conn == TRUE)
        {
            m_pAlgorithm->SquareCenterAlgorithm(dPoint_Conn, dCenter_Conn.x, dCenter_Conn.y);

            GenCrossContourXld(&HConnCrossXld, dCenter_Conn.y, dCenter_Conn.x, 30.0, 0);
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HConnCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

            // Y ??? : BlendOffset?? ?????.
            // UI ??? ?? ???? Y ???? ???η? ???????? -> Y ?? ????? Blend ?? Reference Point ??? ??? ??.
            DistancePl(dCenter_Conn.y, dCenter_Conn.x, dOPoint.y, dOPoint.x - dBlendOffsetRight, dAxisXEnd.y, dAxisXEnd.x - dBlendOffsetRight, &HConnectorY);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = -HConnectorY.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            // FAI-113 (FPCB Connector - Datum Gap X) - 250813. jhkim
            iFAINumber = 113;
            DistancePl(dCenter_Conn.y, dCenter_Conn.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HConnectorX);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HConnectorX.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
        }

        // FAI-119, 120, 70, 81
        DPOINT dLineStart_FPCB_West, dLineEnd_FPCB_West;
        DPOINT dLineStart_FPCB_East, dLineEnd_FPCB_East;
        DPOINT dLineStart_FPCB_South, dLineEnd_FPCB_South;
        DPOINT dLineStart_FPCB_EastEnd, dLineEnd_FPCB_EastEnd;

        HTuple HStartPosX_FPCB_West, HEndPosX_FPCB_West;
        HTuple HStartPosX_FPCB_East, HEndPosX_FPCB_East;
        HTuple HStartPosX_FPCB_South, HEndPosX_FPCB_South;
        HTuple HStartPosX_FPCB_EastEnd, HEndPosX_FPCB_EastEnd;

        // FAI-119 (FPCB - Datum)
        dLineStart_FPCB_West.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][0].x;
        dLineStart_FPCB_West.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][0].y;

        dLineEnd_FPCB_West.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][1].x;
        dLineEnd_FPCB_West.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][1].y;

        // FAI-120 (FPCB - Datum)
        dLineStart_FPCB_East.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][0].x;
        dLineStart_FPCB_East.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][0].y;

        dLineEnd_FPCB_East.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][1].x;
        dLineEnd_FPCB_East.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][1].y;

        // FAI-70 (FPCB - Datum)
        dLineStart_FPCB_South.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][0].x;
        dLineStart_FPCB_South.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][0].y;

        dLineEnd_FPCB_South.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][1].x;
        dLineEnd_FPCB_South.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][1].y;

        // FAI-81 (FPCB - Datum)
        dLineStart_FPCB_EastEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][0].x;
        dLineStart_FPCB_EastEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][0].y;

        dLineEnd_FPCB_EastEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][1].x;
        dLineEnd_FPCB_EastEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][1].y;

        // FAI-70 (FPCB - Datum, Y?? ??)
        iFAINumber = 70;
        if (dLineStart_FPCB_South.x >= 0 && dLineStart_FPCB_South.y >= 0 && dLineEnd_FPCB_South.x >= 0 && dLineEnd_FPCB_South.y >= 0)
        {
            DistancePl(dLineStart_FPCB_South.y, dLineStart_FPCB_South.x, dOPoint.y, dOPoint.x - dBlendOffsetRight, dAxisXEnd.y, dAxisXEnd.x - dBlendOffsetRight, &HStartPosX_FPCB_South);
            DistancePl(dLineEnd_FPCB_South.y, dLineEnd_FPCB_South.x, dOPoint.y, dOPoint.x - dBlendOffsetRight, dAxisXEnd.y, dAxisXEnd.x - dBlendOffsetRight, &HEndPosX_FPCB_South);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = -((HStartPosX_FPCB_South.D() + HEndPosX_FPCB_South.D()) * 0.5) * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_SOUTH)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-81 (FPCB - Datum, X?? ??)
        iFAINumber = 81;
        if (dLineStart_FPCB_EastEnd.x >= 0 && dLineStart_FPCB_EastEnd.y >= 0 && dLineEnd_FPCB_EastEnd.x >= 0 && dLineEnd_FPCB_EastEnd.y >= 0)
        {
            DistancePl(dLineStart_FPCB_EastEnd.y, dLineStart_FPCB_EastEnd.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HStartPosX_FPCB_EastEnd);
            DistancePl(dLineEnd_FPCB_EastEnd.y, dLineEnd_FPCB_EastEnd.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HEndPosX_FPCB_EastEnd);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = (HStartPosX_FPCB_EastEnd.D() + HEndPosX_FPCB_EastEnd.D()) * 0.5 * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_END)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-119 (FPCB - Datum, X?? ??)
        iFAINumber = 119;
        if (dLineStart_FPCB_West.x >= 0 && dLineStart_FPCB_West.y >= 0 && dLineEnd_FPCB_West.x >= 0 && dLineEnd_FPCB_West.y >= 0)
        {
            DistancePl(dLineStart_FPCB_West.y, dLineStart_FPCB_West.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HStartPosX_FPCB_West);
            DistancePl(dLineEnd_FPCB_West.y, dLineEnd_FPCB_West.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HEndPosX_FPCB_West);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = (HStartPosX_FPCB_West.D() + HEndPosX_FPCB_West.D()) * 0.5 * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_WEST)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-120 (FPCB - Datum, X?? ??)
        iFAINumber = 120;
        if (dLineStart_FPCB_East.x >= 0 && dLineStart_FPCB_East.y >= 0 && dLineEnd_FPCB_East.x >= 0 && dLineEnd_FPCB_East.y >= 0)
        {
            DistancePl(dLineStart_FPCB_East.y, dLineStart_FPCB_East.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HStartPosX_FPCB_East);
            DistancePl(dLineEnd_FPCB_East.y, dLineEnd_FPCB_East.x, dOPoint.y, dOPoint.x, dAxisYEnd.y, dAxisYEnd.x, &HEndPosX_FPCB_East);

            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = (HStartPosX_FPCB_East.D() + HEndPosX_FPCB_East.D()) * 0.5 * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][0], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_BOTTOM_FPCB_EAST)][1], &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }
    }

    HObject HContXld, HCrossXld;
    HTuple HRowA, HColA, HRadA;

    // ??? ??????
    if (m_iVisionCamType == VISION_NUMBER_2)
    {
        // FAI-140 Refer
        iFAINumber = 140;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        // FAI-141 Blend Left
        iFAINumber = 141;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        // FAI-142 Blend Right
        iFAINumber = 142;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].x;
        dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].y;

        dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].x;
        dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].y;

        // FAI-146 DIST TOP CHASIS NORTH-EAST
        iFAINumber = 146;
        if (dLineStart.x >= 0 && dLineStart.y >= 0 && dLineEnd.x >= 0 && dLineEnd.y >= 0)
        {
            m_pAlgorithm->BlendingAlgorithm_Horizontal(
                dRefer1HStartPoint.x, dRefer1HEndPoint.x, dRefer2HStartPoint.x, dRefer2HEndPoint.x,
                dImage1HStartPoint.x, dImage1HEndPoint.x, dImage2HStartPoint.x, dImage2HEndPoint.x,
                dImage1VStartPoint.x, dImage1VStartPoint.y, dImage1VEndPoint.x, dImage1VEndPoint.y,
                dImage2VStartPoint.x, dImage2VStartPoint.y, dImage2VEndPoint.x, dImage2VEndPoint.y,
                dLineEnd.x, dLineEnd.y, dBlendOffsetLeft, dBlendOffsetRight);

            DistancePp(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
        }

        if (THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0]) == TRUE)
        {
            GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0], &HContXld, "border");
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HContXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

            GenCrossContourXld(&HCrossXld, dLineEnd.y, (dLineEnd.x - dBlendOffsetRight), 30.0, 0);
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-147 RAD TOP CHASIS NORTH
        iFAINumber = 147;
        if (THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0]) == TRUE)
        {
            InnerCircle(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0], &HRowA, &HColA, &HRadA);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HRadA.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0], &HContXld, "border");
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HContXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

            GenCrossContourXld(&HCrossXld, dLineStart.y, dLineStart.x, 30.0, 0);
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }
    }
    else if (m_iVisionCamType == VISION_NUMBER_1)
    {
        // FAI-143 Refer
        iFAINumber = 143;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_1_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_REFER_2_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        // FAI-144 Blend Left
        iFAINumber = 144;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_1_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        // FAI-145 Blend Right
        iFAINumber = 145;
        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][0].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][0].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        GenCrossContourXld(&HCrossXld, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][1].y, StructFAIMeasure->m_dBlendCPoint[m_iVisionCamType][POSITION_BLEND_2_X][1].x, 60.0, 0);
        ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

        dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0].x;
        dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0].y;

        dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0].x;
        dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0].y;

        // FAI-148 DIST BOTTOM CHASIS NORTH-WEST
        iFAINumber = 148;
        if (dLineStart.x >= 0 && dLineStart.y >= 0 && dLineEnd.x >= 0 && dLineEnd.y >= 0)
        {
            m_pAlgorithm->BlendingAlgorithm_Horizontal(
                dRefer1HStartPoint.x, dRefer1HEndPoint.x, dRefer2HStartPoint.x, dRefer2HEndPoint.x,
                dImage1HStartPoint.x, dImage1HEndPoint.x, dImage2HStartPoint.x, dImage2HEndPoint.x,
                dImage1VStartPoint.x, dImage1VStartPoint.y, dImage1VEndPoint.x, dImage1VEndPoint.y,
                dImage2VStartPoint.x, dImage2VStartPoint.y, dImage2VEndPoint.x, dImage2VEndPoint.y,
                dLineEnd.x, dLineEnd.y, dBlendOffsetLeft, dBlendOffsetRight);

            DistancePp(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HDistance.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];
        }

        if (THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0]) == TRUE)
        {
            GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_WEST)][0], &HContXld, "border");
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HContXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

            GenCrossContourXld(&HCrossXld, dLineEnd.y, (dLineEnd.x - dBlendOffsetRight), 30.0, 0);
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }

        // FAI-149 RAD BOTTOM CHASIS NORTH
        iFAINumber = 149;
        if (THEAPP.m_pGFunction->ValidHRegion(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0]) == TRUE)
        {
            InnerCircle(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0], &HRowA, &HColA, &HRadA);
            StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = HRadA.D() * THEAPP.m_pCalDataService_N[m_iVisionCamType]->GetPixelSize() * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

            GenContourRegionXld(StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_BOTTOM_CHASIS_HOLE_NORTH)][0], &HContXld, "border");
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HContXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));

            GenCrossContourXld(&HCrossXld, dLineStart.y, dLineStart.x, 30.0, 0);
            ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HCrossXld, &(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]));
        }
    }
}
