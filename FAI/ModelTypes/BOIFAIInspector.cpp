#include "StdAfx.h"
#include "uScan.h"
#include "Algorithm.h"
#include "BOIFAIInspector.h"
#include "math.h"

BOIFAIInspector::BOIFAIInspector(Algorithm* pAlgorithm)
	: m_pAlgorithm(pAlgorithm)
{
}

void BOIFAIInspector::SetAlgorithm(Algorithm* pAlgorithm)
{
	m_pAlgorithm = pAlgorithm;
}

void BOIFAIInspector::Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{
	if (!m_pAlgorithm)
		return;

	// BOI FAI는 상단 카메라(VISION_NUMBER_2 또는 VISION_NUMBER_4)에서만 실행됩니다
	// 예 : FAI5, FAI11, FAI38
	if ((m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_2) && (m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_4))
	{
		OutputDebugString(_T("[BOIFAIInspector] 하단 카메라는 건너뜁니다.\n"));
		return;
	}

	CenterlineMeasureStruct* StructFAIMeasure = CFAIDataManager::GetInstance().GetMeasurePtr(
		bIsAutoInsp,
		m_pAlgorithm->m_nMzNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nTrayNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx],
		iFAIParamNo);

	if (!StructFAIMeasure)
	{
		OutputDebugString(_T("[BOIFAIInspector] FAI 측정 구조체가 null입니다.\n"));
		return;
	}

	double* dMultiple = nullptr;
	double* dOffset   = nullptr;
	SetCalibrationValues(iInspectionBufferIdx, dMultiple, dOffset);

	const int iCamType  = m_pAlgorithm->m_iVisionCamType;
	double    dPixelSize = THEAPP.m_pCalDataService_N[iCamType]->GetPixelSize();

	int    iFAINumber = 0;
	HTuple HDistance;
	DPOINT dLineStart, dLineEnd;
	DPOINT dPoint1, dPoint2;

	////////////////////////////////////////////////////////////////////////
	// FAI-11: 렌즈 외부 지름 (MEASURE_TOP_LENS_OUTER에서 지름 측정)
	//////////////////////////////////////////////////////////////////////// 
	double dOuterCenterX = 0.0, dOuterCenterY = 0.0, dOuterRadius = 0.0;

	iFAINumber = 11;
	if (!FitCircle(StructFAIMeasure, enMeasurePos::MEASURE_TOP_LENS_OUTER,
	               iFAINumber, dOuterCenterX, dOuterCenterY, dOuterRadius,
	               iInspectionBufferIdx, iFAIParamNo))
	{
		OutputDebugString(_T("[BOIFAIInspector] FAI-11: 렌즈 외부 지름 측정이 실패했습니다.\n"));
		return;
	}

	StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] =
		dOuterRadius * 2.0 * dPixelSize * UNIT_SCALE_0_001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

	////////////////////////////////////////////////////////////////////////
	// FAI-5 / FAI-5.X / FAI-5.Y: 렌즈 중심 위치 편차
	// 중심점을 얻기 위해 MEASURE_TOP_LENS_INNER에서 지름 측정 필요
	//////////////////////////////////////////////////////////////////////// 
	double dInnerCenterX = 0.0, dInnerCenterY = 0.0, dInnerRadius = 0.0;

	iFAINumber = 5;
	if (!FitCircle(StructFAIMeasure, enMeasurePos::MEASURE_TOP_LENS_INNER,
	               iFAINumber, dInnerCenterX, dInnerCenterY, dInnerRadius,
	               iInspectionBufferIdx, iFAIParamNo))
	{
		OutputDebugString(_T("[BOIFAIInspector] FAI-5: 렌즈 내부 지름 측정이 실패했습니다.\n"));
		return;
	}

	// 맞춘 중심을 후속 계산을 위한 중심 좌표로 저장
	StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].x = dInnerCenterX;
	StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER)][0].y = dInnerCenterY;

	// FAI-5.X: 렌즈 중심에서 WEST 기준선까지의 거리
	constexpr int FAI5X_ID = 6;
	{
		dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0].x;
		dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0].y;
		dLineEnd.x   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1].x;
		dLineEnd.y   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1].y;

		double dFAI5X = 0.0;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0)
		{
			ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0],
			          StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][0],
			          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0]);
			ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0],
			          StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_WEST)][1],
			          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5X_ID][iFAIParamNo][0]);

			DistancePl(dInnerCenterY, dInnerCenterX,
			           dLineStart.y, dLineStart.x,
			           dLineEnd.y,   dLineEnd.x,
			           &HDistance);

			dFAI5X = HDistance.D() * dPixelSize * UNIT_SCALE_0_001 * dMultiple[FAI5X_ID] + dOffset[FAI5X_ID];
		}
		StructFAIMeasure->m_dFAIMeasureValue[FAI5X_ID][0] = dFAI5X;
	}

	// FAI-5.Y: 렌즈 중심에서 NORTH 기준선까지의 거리
	constexpr int FAI5Y_ID = 7;
	{
		dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0].x;
		dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0].y;
		dLineEnd.x   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1].x;
		dLineEnd.y   = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1].y;

		double dFAI5Y = 0.0;
		if (dLineStart.x >= 0 && dLineEnd.x >= 0)
		{
			ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5Y_ID][iFAIParamNo][0],
			          StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][0],
			          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5Y_ID][iFAIParamNo][0]);
			ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5Y_ID][iFAIParamNo][0],
			          StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_DATUM_NORTH)][1],
			          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI5Y_ID][iFAIParamNo][0]);

			DistancePl(dInnerCenterY, dInnerCenterX,
			           dLineStart.y, dLineStart.x,
			           dLineEnd.y,   dLineEnd.x,
			           &HDistance);

			dFAI5Y = HDistance.D() * dPixelSize * UNIT_SCALE_0_001 * dMultiple[FAI5Y_ID] + dOffset[FAI5Y_ID];
		}
		StructFAIMeasure->m_dFAIMeasureValue[FAI5Y_ID][0] = dFAI5Y;
	}

	// FAI-5: 동심도 계산 = 2 * sqrt((|5X| - specX)^2 + (|5Y| - specY)^2)
	constexpr int    FAI5_ID    = 5;
	constexpr double FAI5_X_SPEC = 10.418; // MCO X spec (mm)
	constexpr double FAI5_Y_SPEC = 10.978; // MCO Y spec (mm)
	{
		double dFAI5X = StructFAIMeasure->m_dFAIMeasureValue[FAI5X_ID][0];
		double dFAI5Y = StructFAIMeasure->m_dFAIMeasureValue[FAI5Y_ID][0];
		double dFAI5  = 2.0 * sqrt(pow(abs(dFAI5X) - FAI5_X_SPEC, 2.0) + pow(abs(dFAI5Y) - FAI5_Y_SPEC, 2.0));
		dFAI5 = dFAI5 * dMultiple[FAI5_ID] + dOffset[FAI5_ID];
		StructFAIMeasure->m_dFAIMeasureValue[FAI5_ID][0] = dFAI5;
	}

	////////////////////////////////////////////////////////////////////////
	// FAI-38: Shield Can 라인과 Harrison flex 라인 사이의 각도 (90도 기준)
	//////////////////////////////////////////////////////////////////////// 
	constexpr int FAI38_ID = 38;
	{
		DPOINT dPointShieldCanNorth[2], dPointShieldCanSouth[2];
		DPOINT dPointHarrisonFlexNorth[2], dPointHarrisonFlexSouth[2];

		for (int i = 0; i < 2; i++)
		{
			dPointShieldCanNorth[i].x  = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SHIELD_CAN_NORTH)][i].x;
			dPointShieldCanNorth[i].y  = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SHIELD_CAN_NORTH)][i].y;
			dPointShieldCanSouth[i].x  = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SHIELD_CAN_SOUTH)][i].x;
			dPointShieldCanSouth[i].y  = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_SHIELD_CAN_SOUTH)][i].y;
			dPointHarrisonFlexNorth[i].x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_HARRISON_FLEX_NORTH)][i].x;
			dPointHarrisonFlexNorth[i].y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_HARRISON_FLEX_NORTH)][i].y;
			dPointHarrisonFlexSouth[i].x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_HARRISON_FLEX_SOUTH)][i].x;
			dPointHarrisonFlexSouth[i].y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_HARRISON_FLEX_SOUTH)][i].y;
		}

		// SM1: S1-S3의 중점 (north), SM2: S2-S4의 중점 (south)
		DPOINT dSM1, dSM2;
		dSM1.x = (dPointShieldCanNorth[0].x + dPointShieldCanSouth[0].x) / 2.0;
		dSM1.y = (dPointShieldCanNorth[0].y + dPointShieldCanSouth[0].y) / 2.0;
		dSM2.x = (dPointShieldCanNorth[1].x + dPointShieldCanSouth[1].x) / 2.0;
		dSM2.y = (dPointShieldCanNorth[1].y + dPointShieldCanSouth[1].y) / 2.0;

		// HM1: H1-H2의 중점, HM2: H3-H4의 중점
		DPOINT dHM1, dHM2;
		dHM1.x = (dPointHarrisonFlexNorth[0].x + dPointHarrisonFlexNorth[1].x) / 2.0;
		dHM1.y = (dPointHarrisonFlexNorth[0].y + dPointHarrisonFlexNorth[1].y) / 2.0;
		dHM2.x = (dPointHarrisonFlexSouth[0].x + dPointHarrisonFlexSouth[1].x) / 2.0;
		dHM2.y = (dPointHarrisonFlexSouth[0].y + dPointHarrisonFlexSouth[1].y) / 2.0;

		// Shield Can 라인(X축 기준)과 Harrison Flex 라인(Y축 기준)
		HTuple hFAI38;
		AngleLl(dSM1.y, dSM1.x, dSM2.y, dSM2.x,
		        dHM1.y, dHM1.x, dHM2.y, dHM2.x,
		        &hFAI38);
		TupleDeg(hFAI38, &hFAI38);

		double dFAI38 = abs(hFAI38.D()) * dMultiple[FAI38_ID] + dOffset[FAI38_ID];
		StructFAIMeasure->m_dFAIMeasureValue[FAI38_ID][0] = dFAI38;

		// 리뷰이미지를 위해 XLD 라인 저장
		HObject hLineShield, hLineFlex;
		GenRegionLine(&hLineShield, dSM1.y, dSM1.x, dSM2.y, dSM2.x);
		GenRegionLine(&hLineFlex,   dHM1.y, dHM1.x, dHM2.y, dHM2.x);
		GenContourRegionXld(hLineShield, &hLineShield, "border");
		GenContourRegionXld(hLineFlex,   &hLineFlex,   "border");

		StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER_ANGLE)][0] = hLineShield;
		StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_TOP_LENS_INNER_ANGLE)][1] = hLineFlex;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI38_ID][iFAIParamNo][0],
		          hLineShield,
		          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI38_ID][iFAIParamNo][0]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI38_ID][iFAIParamNo][0],
		          hLineFlex,
		          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][FAI38_ID][iFAIParamNo][0]);
	}
}

void BOIFAIInspector::SetCalibrationValues(int iInspectionBufferIdx, double*& dMultiple, double*& dOffset)
{
	if (!m_pAlgorithm)
		return;

#ifdef INLINE_MODE
	if (m_pAlgorithm->m_nStageNo[iInspectionBufferIdx] == STAGE_NUMBER_2)
	{
		dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai;
		dOffset   = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai;
	}
	else
	{
		dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai;
		dOffset   = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai;
	}
#else
	if ((m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx] % 2) == 0)
	{
		dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg2Fai;
		dOffset   = THEAPP.m_pModelDataManager->m_dMOffsetStg2Fai;
	}
	else
	{
		dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai;
		dOffset   = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai;
	}
#endif
}

BOOL BOIFAIInspector::FitCircle(
	CenterlineMeasureStruct* pStructFAIMeasure,
	enMeasurePos measurePointType,
	int iFAINumber,
	double& dCircleCenterX,
	double& dCircleCenterY,
	double& dCircleRadius,
	int iInspectionBufferIdx,
	int iFAIParamNo)
{
	if (!m_pAlgorithm || !pStructFAIMeasure)
		return FALSE;

	const int iMeasureIdx = MtoI(measurePointType);

	HTuple HEdgeConcatX, HEdgeConcatY;
	DPOINT dPoint;

	for (int i = 0; i < MAX_FAI_CIRCLE_FIT_POINT; i++)
	{
		dPoint.x = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i].x;
		dPoint.y = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i].y;

		if (dPoint.x >= 0 && dPoint.y >= 0)
		{
			TupleConcat(HEdgeConcatX, HTuple(dPoint.x), &HEdgeConcatX);
			TupleConcat(HEdgeConcatY, HTuple(dPoint.y), &HEdgeConcatY);

			ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
			          pStructFAIMeasure->m_HMeasureXLD[iMeasureIdx][i],
			          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		}
	}

	HTuple HEdgeCount;
	TupleLength(HEdgeConcatX, &HEdgeCount);
	if (HEdgeCount.L() != MAX_FAI_CIRCLE_FIT_POINT)
	{
		OutputDebugString(_T("[BOIFAIInspector] FitCircle: 불충분한 edge 포인트입니다.\n"));
		return FALSE;
	}

	// 초기 반지름을 평균 지름 / 2로 추정
	double dNormalRadius = 0.0;
	DPOINT dP1, dP2;
	HTuple HDistance;
	for (int i = 0; i < MAX_FAI_CIRCLE_FIT_POINT / 2; i++)
	{
		dP1.x = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i].x;
		dP1.y = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i].y;
		dP2.x = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i + MAX_FAI_CIRCLE_FIT_POINT / 2].x;
		dP2.y = pStructFAIMeasure->m_dMeasureCPoint[iMeasureIdx][i + MAX_FAI_CIRCLE_FIT_POINT / 2].y;

		DistancePp(dP1.y, dP1.x, dP2.y, dP2.x, &HDistance);
		dNormalRadius += HDistance.D();
	}
	dNormalRadius /= (MAX_FAI_CIRCLE_FIT_POINT / 2);
	dNormalRadius *= 0.5;

	BOOL bFound = m_pAlgorithm->LeastSquareCircleFitting(
		HEdgeConcatX, HEdgeConcatY, dNormalRadius,
		&dCircleCenterX, &dCircleCenterY, &dCircleRadius);

	if (!bFound)
	{
		OutputDebugString(_T("[BOIFAIInspector] FitCircle: LeastSquareCircleFitting failed.\n"));
		return FALSE;
	}

	// 검토용 오버레이를 위해 중심 십자선 그리기
	HObject HCross;
	GenCrossContourXld(&HCross, dCircleCenterY, dCircleCenterX, 30, 0);
	ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0],
	          HCross,
	          &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

	return TRUE;
}
