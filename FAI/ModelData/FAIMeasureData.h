#pragma once
#include "StdAfx.h"
#include "FAIDefine.h"

// CenterlineMeasureStruct: FAI Measurement Data
struct CenterlineMeasureStruct
{
    ////////////////////////////////////////////////////////////////
    // CHS

    // Common
    DPOINT m_dDatumBLinePoint[8], m_dDatumBPoint;
    double m_dDatumA_Height[4];
    DPOINT m_dDatumA_CPoint[4];
    double m_dVCMShieldCanTop_Height[8];
    DPOINT m_dVCMShieldCanTop_CPoint[8];

    // Top, Bottom
    DPOINT m_dFAI5_LensBarrelTopEdgePoint[MAX_FAI_CIRCLE_FIT_POINT];
    DPOINT m_dMeasureFAI5_LensBarrelTopEdgePoint;
    double m_dMeasureFAI5_Diameter;
    DPOINT m_dFAI6_LensInnerEdgePoint[MAX_FAI_CIRCLE_FIT_POINT];
    DPOINT m_dMeasureFAI6_LensInnerEdgePoint;

    // m_dMeasureCPoint : (Maybe) 해당 Blob 의 Center 측정하여 저장
    // CPoint : Center Point
    DPOINT m_dMeasureCPoint[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];
    HObject m_HMeasureXLD[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];
    HObject m_HMeasureRgn[MEASURE_POS_TOTAL][MAX_MEASURE_POINT_NUM];
    DPOINT m_dBlendCPoint[VISION_NUMBER_MAX][POSITION_BLEND_END][BLEND_POINT_END];
    double m_dHeight[MEASURE_POS_TOTAL][MEASURE_POS_TOTAL];
    double m_dDiameter[MEASURE_POS_TOTAL];
// TODO : CHS only
#if defined (UAVI_CHS_KS) || defined (UAVI_CHS_TV)
    DPOINT m_dFAI7_WestPoint[2], m_dFAI7_EastPoint[2];
    DPOINT m_dFAI9_WestPoint[5], m_dFAI9_EastPoint[3];
    DPOINT m_dFAI10_NorthPoint[3], m_dFAI10_SouthPoint[3];
    DPOINT m_dFAI18_NeckEpoxyPoint[3];
    DPOINT m_dFAI21_FlexPoint[2];
    DPOINT m_dFAI23_FlexPoint[2];
    DPOINT m_dFAI24_Point[2];
    DPOINT m_dFAI25_NorthPoint[2], m_dFAI25_SouthPoint[2];
    DPOINT m_dFAI16_1_LinePoint[2], m_dFAI16_1_TopPoint[4];
    DPOINT m_dFAI16_2_LinePoint[2], m_dFAI16_2_TopPoint[4];
    DPOINT m_dFAI16_3_LinePoint[2], m_dFAI16_3_TopPoint[4];
    DPOINT m_dFAI19_LinePoint[2], m_dFAI19_TopPoint[4];
#endif
#if defined (UAVI_CHS_WZ)
    DPOINT m_dFAI3_WestPoint[2], m_dFAI3_EastPoint[2];
    DPOINT m_dFAI9_WestPoint[3], m_dFAI9_EastPoint[3];
    DPOINT m_dFAI10_NorthPoint[3], m_dFAI10_SouthPoint[2];
    DPOINT m_dFAI14_NorthPoint[2], m_dFAI14_SouthPoint[2];
    DPOINT m_dFAI18_NeckEpoxyPoint[3];
    DPOINT m_dFAI21_FlexPoint[2];
    DPOINT m_dFAI22_FlexPoint[2];
    DPOINT m_dFAI24_Point[4];
    DPOINT m_dFAI25_Point[4];
    DPOINT m_dFAI17_1_LinePoint[2], m_dFAI17_1_TopPoint[4];
    DPOINT m_dFAI17_2_LinePoint[2], m_dFAI17_2_TopPoint[4];
    DPOINT m_dFAI17_3_LinePoint[2], m_dFAI17_3_TopPoint[4];
    DPOINT m_dFAI19_LinePoint[2], m_dFAI19_TopPoint[4];
#endif
    // Measurement Value/Count/NG
    double m_dFAIMeasureValue[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];
    int m_iFAIMeasureCount[MAX_FAI_ITEM];

    // Inspection Result
    BOOL m_bInspectFail[VISION_NUMBER_MAX];
    BOOL m_bDoROS;
    BOOL m_bFAI_ResultNG[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];
    BOOL m_bDeepLearningResultGood[MAX_FAI_ITEM];
    BOOL m_bParamResultNG[MAX_FAI_ITEM];
    int m_iFAIResultCase[MAX_FAI_ITEM];

    // Fiducial Mark
    double m_dFiducialMark_PointX[MAX_TILT_MEASURE_POINT];
    double m_dFiducialMark_PointY[MAX_TILT_MEASURE_POINT];

    // Constructor
    CenterlineMeasureStruct()
    {
        ResetFAI();
    }

    void ResetFAI()
    {
        // CM CHS
        m_dDatumBPoint.Reset();

        for (int i = 0; i < 8; i++)
        {
            m_dDatumBLinePoint[i].Reset();
            m_dVCMShieldCanTop_Height[i] = HEIGHT_INIT;
            m_dVCMShieldCanTop_CPoint[i].Reset();
        }

        for (int i = 0; i < 4; i++)
        {
            m_dDatumA_Height[i] = HEIGHT_INIT;
            m_dDatumA_CPoint[i].Reset();
        }

        for (int i = 0; i < MAX_FAI_CIRCLE_FIT_POINT; i++)
        {
            m_dFAI5_LensBarrelTopEdgePoint[i].Reset();
            m_dFAI6_LensInnerEdgePoint[i].Reset();
        }

        m_dMeasureFAI5_LensBarrelTopEdgePoint.Reset();
        m_dMeasureFAI5_Diameter = FAI_MEASURE_ERROR_VALUE;
        m_dMeasureFAI6_LensInnerEdgePoint.Reset();

        for (int i = 0; i < MEASURE_POS_TOTAL; i++)
        {
            for (int ii = 0; ii < MAX_MEASURE_POINT_NUM; ii++)
            {
                GenEmptyObj(&(m_HMeasureRgn[i][ii]));
                GenEmptyObj(&(m_HMeasureXLD[i][ii]));
                m_dMeasureCPoint[i][ii].Reset();
            }

            for (int ii = 0; ii < MEASURE_POS_TOTAL; ii++)
            {
                m_dHeight[i][ii] = HEIGHT_INIT;
            }

            m_dDiameter[i] = HEIGHT_INIT;
        }

        for (int i = 0; i < VISION_NUMBER_MAX; i++)
        {
            for (int ii = 0; ii < POSITION_BLEND_END; ii++)
            {
                for (int iii = 0; iii < BLEND_POINT_END; iii++)
                {
                    m_dBlendCPoint[i][ii][iii].Reset();
                }
            }
        }

        for (int i = 0; i < MAX_FAI_ITEM; i++)
        {
            for (int j = 0; j < MAX_ONE_FAI_MEASURE_VALUE; j++)
            {
                m_dFAIMeasureValue[i][j] = FAI_MEASURE_ERROR_VALUE;
                m_bFAI_ResultNG[i][j] = FALSE;
            }

            m_iFAIMeasureCount[i] = 0;
            m_bDeepLearningResultGood[i] = TRUE;
            m_bParamResultNG[i] = FALSE;
            m_iFAIResultCase[i] = FAI_RESULT_CASE_GOOD;
        }

#if defined (UAVI_CHS_KS) || defined (UAVI_CHS_TV)
        for (int i = 0; i < 2; i++)
        {
            m_dFAI7_WestPoint[i].Reset();
            m_dFAI7_EastPoint[i].Reset();
        }

        for (int i = 0; i < 5; i++)
        {
            m_dFAI9_WestPoint[i].Reset();
            m_dFAI9_EastPoint[i].Reset();
        }

        for (int i = 0; i < 3; i++)
        {
            m_dFAI10_NorthPoint[i].Reset();
            m_dFAI10_SouthPoint[i].Reset();
            m_dFAI18_NeckEpoxyPoint[i].Reset();
        }

        for (int i = 0; i < 2; i++)
        {
            m_dFAI21_FlexPoint[i].Reset();
            m_dFAI23_FlexPoint[i].Reset();
            m_dFAI24_Point[i].Reset();
            m_dFAI25_NorthPoint[i].Reset();
            m_dFAI25_SouthPoint[i].Reset();

            m_dFAI16_1_LinePoint[i].Reset();
            m_dFAI16_2_LinePoint[i].Reset();
            m_dFAI16_3_LinePoint[i].Reset();
            m_dFAI19_LinePoint[i].Reset();
        }

        for (int i = 0; i < 4; i++)
        {
            m_dFAI16_1_TopPoint[i].Reset();
            m_dFAI16_2_TopPoint[i].Reset();
            m_dFAI16_3_TopPoint[i].Reset();
            m_dFAI19_TopPoint[i].Reset();
        }
#endif
#ifdef UAVI_CHS_WZ
        for (int i = 0; i < 2; i++)
        {
            m_dFAI3_WestPoint[i].Reset();
            m_dFAI3_EastPoint[i].Reset();
        }

        for (int i = 0; i < 3; i++)
        {
            m_dFAI9_WestPoint[i].Reset();
            m_dFAI9_EastPoint[i].Reset();

            m_dFAI10_NorthPoint[i].Reset();

            m_dFAI18_NeckEpoxyPoint[i].Reset();
        }

        for (int i = 0; i < 2; i++)
        {
            m_dFAI10_SouthPoint[i].Reset();

            m_dFAI14_NorthPoint[i].Reset();
            m_dFAI14_SouthPoint[i].Reset();

            m_dFAI21_FlexPoint[i].Reset();
            m_dFAI22_FlexPoint[i].Reset();

            m_dFAI17_1_LinePoint[i].Reset();
            m_dFAI17_2_LinePoint[i].Reset();
            m_dFAI17_3_LinePoint[i].Reset();

            m_dFAI19_LinePoint[i].Reset();
        }

        for (int i = 0; i < 4; i++)
        {
            m_dFAI24_Point[i].Reset();
            m_dFAI25_Point[i].Reset();

            m_dFAI17_1_TopPoint[i].Reset();
            m_dFAI17_2_TopPoint[i].Reset();
            m_dFAI17_3_TopPoint[i].Reset();

            m_dFAI19_TopPoint[i].Reset();
        }
#endif
    };
};
