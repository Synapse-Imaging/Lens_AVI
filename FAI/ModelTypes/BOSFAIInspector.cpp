#include "stdafx.h"
#include "BOSFAIInspector.h"
#include "math.h"
#include "Utils/DebugImageUtils.h"

void BOSFAIInspector::SetAlgorithm(Algorithm *pAlgorithm)
{
	m_pAlgorithm = pAlgorithm;
}

void BOSFAIInspector::Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{
	if (!m_pAlgorithm)
		return;

	if ((m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_2) && (m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_4))
	{
		OutputDebugString(_T("[BOSFAIInspector] 하단 카메라는 건너뜁니다.\n"));
		return;
	}

	CenterlineMeasureStruct *StructFAIMeasure = CFAIDataManager::GetInstance().GetMeasurePtr(
		bIsAutoInsp,
		m_pAlgorithm->m_nMzNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nTrayNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx],
		iFAIParamNo);

	if (!StructFAIMeasure)
	{
		OutputDebugString(_T("[BOSFAIInspector] FAI 측정 구조체가 null입니다.\n"));
		return;
	}

	double *dMultiple = nullptr;
	double *dOffset = nullptr;
	SetCalibrationValues(iInspectionBufferIdx, dMultiple, dOffset);

	const int iCamType = m_pAlgorithm->m_iVisionCamType;
	double dPixelSize = THEAPP.m_pCalDataService_N[iCamType]->GetPixelSize();

	////////////////////////////////////////////////////////////////////////
	// [1] Datum-D : 렌즈 내부 중심좌표
	////////////////////////////////////////////////////////////////////////
	DPOINT dDatumD;
	double dDummyRadius; // 굳이 Radius 가 필요하지 않음 (함수 호출을 위해 선언)

	if (!FitCircle(StructFAIMeasure, enMeasurePos::MEASURE_TOP_LENS_INNER,
				   dDatumD.x, dDatumD.y, dDummyRadius))
	{
		OutputDebugString(_T("[BOSFAIInspector] 렌즈 내부 원 피팅 실패.\n"));
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// [2] Datum-E : X 축 (North, East Bracket Hole 을 이은 선분을 37.09도 회전하여 얻은 선)
	////////////////////////////////////////////////////////////////////////
	CGFunction &gFunc = *THEAPP.m_pGFunction;
	HObject HRgnChaisHoleEast = StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0];
	HObject HRgnChaisHoleNorth = StructFAIMeasure->m_HMeasureRgn[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0];

	// Bracket Hole 영역 유효성 검사 - 하나라도 미검출 시 에러값 저장 후 종료
	if (gFunc.ValidHRegion(HRgnChaisHoleEast) != TRUE || gFunc.ValidHRegion(HRgnChaisHoleNorth) != TRUE)
	{
		StructFAIMeasure->m_dFAIMeasureValue[BOS_FAI::BRACKET_HOLE_CONCENTRICITY][0] = FAI_MEASURE_ERROR;
		StructFAIMeasure->m_dFAIMeasureValue[BOS_FAI::BRACKET_HOLE_CONCENTRICITY][1] = FAI_MEASURE_ERROR;
		OutputDebugString(_T("[BOSFAIInspector] FAI-1: Bracket Hole 미검출.\n"));
		return;
	}

	// [2-1] Point C1
	DPOINT dBracketHoleEastCenter;
	dBracketHoleEastCenter.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].x;
	dBracketHoleEastCenter.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_EAST)][0].y;

	// [2-2] Point C4
	DPOINT dBracketHoleNorthCenter;
	dBracketHoleNorthCenter.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].x;
	dBracketHoleNorthCenter.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_TOP_CHASIS_HOLE_NORTH)][0].y;

	// [2-3] C4(North) 점을 37.09°반시계 회전
	constexpr double dDatumDAngle = -37.09;
	DPOINT dAxisEnd = dBracketHoleNorthCenter;
	DPOINT dDummy;

	// dDummy : X 축 기준 회전 (사용하지 않음)
	m_pAlgorithm->AxisTransAlgorithm(
		TRUE, dDatumDAngle,
		dBracketHoleEastCenter.x, dBracketHoleEastCenter.y,
		dDummy.x, dDummy.y,
		dAxisEnd.x, dAxisEnd.y);

	// [2-4] C1, C4 점을 Datum-D 를 통과하도록 평행이동 -> Datum-E
	// 평행이동 벡터(dTrans) : dBracketHoleEastCenter -> dDatumD
	DPOINT dAxisStart, dTrans;
	dTrans.x = dDatumD.x - dBracketHoleEastCenter.x;
	dTrans.y = dDatumD.y - dBracketHoleEastCenter.y;
	// dAxisStart = dBracketHoleEastCenter + dTrans
	//			  = dDatumD
	dAxisStart.x = dDatumD.x;
	dAxisStart.y = dDatumD.y;
	dAxisEnd.x += dTrans.x;
	dAxisEnd.y += dTrans.y;

	// [3] FAI-1 의 X, Y 측정값 계산
	// [3-1] FAI1_Y : Datum-E 와 East Bracket Hole 간의 거리
	HTuple hFAI1_Y;
	DistancePl(
		dBracketHoleEastCenter.y, dBracketHoleEastCenter.x,
		dAxisStart.y, dAxisStart.x,
		dAxisEnd.y, dAxisEnd.x,
		&hFAI1_Y);
	double dFAI1_Y = hFAI1_Y.D();

	// [3-2] FAI1_X: Datum-E Y축(X축 반시계 90도 회전)까지의 수직거리
	// Datum-E의 Y축 방향 벡터 (dYAxisEnd - dAxisStart)
	const double dXDirRow = dAxisEnd.y - dAxisStart.y;
	const double dXDirCol = dAxisEnd.x - dAxisStart.x;
	// Row = dAxisStart.Row - dXDirCol
	// Col = dAxisStart.Col + dXDirRow
	DPOINT dYAxisEnd;
	dYAxisEnd.y = dAxisStart.y + (-dXDirCol);   
	dYAxisEnd.x = dAxisStart.x + dXDirRow;      

	HTuple hFAI1_X;
	DistancePl(
		dBracketHoleEastCenter.y, dBracketHoleEastCenter.x,
		dAxisStart.y, dAxisStart.x,   // dDatumD (Y축 통과점)
		dYAxisEnd.y,  dYAxisEnd.x,    // Y축 두 번째 점
		&hFAI1_X);
	double dFAI1_X = hFAI1_X.D();

	// [4] 픽셀 → mm 변환
	dFAI1_X *= dPixelSize * UNIT_SCALE_0_001;
	dFAI1_Y *= dPixelSize * UNIT_SCALE_0_001;

	// FAI-1 Spec : (6.144,1.006)(mm)
	constexpr double dFAISpecX = 6.144;
	constexpr double dFAISpecY = 1.006;

	double dFAI1 = sqrt(pow(dFAI1_X - dFAISpecX, 2) + pow(dFAI1_Y - dFAISpecY, 2));

	// [5] 보정값 적용 (Offset, Multiple)
	const int iFAI1 = BOS_FAI::BRACKET_HOLE_CONCENTRICITY;
	dFAI1 = (dFAI1 * dMultiple[iFAI1]) + dOffset[iFAI1];

	// [6] FAI-1 결과 저장
	StructFAIMeasure->m_dFAIMeasureValue[iFAI1][0] = dFAI1;

	// TODO : FAI1-X, FAI1-Y 결과도 로그에 저장할지 검토 필요.

	////////////////////////////////////////////////////////////////////
	// Review 이미지 후처리
	// TODO : 이미지 후처리 및 저장을 별개의 함수로 분리
	////////////////////////////////////////////////////////////////////
	// Datum-D 원점 (렌즈 중심) 십자 표시
	HObject HCrossLensCenter;
	GenCrossContourXld(&HCrossLensCenter, dDatumD.y, dDatumD.x, 30, 0);
	ConcatObj(
		m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::LENS_INNER][iFAIParamNo][0],
		HCrossLensCenter,
		&m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::LENS_INNER][iFAIParamNo][0]);

	// Datum-E 방향선 (X축, 50픽셀 길이)
	HObject HDatumELine;
	GenRegionLine(&HDatumELine, dAxisStart.y, dAxisStart.x, dAxisEnd.y, dAxisEnd.x);
	GenContourRegionXld(HDatumELine, &HDatumELine, "border");
	ConcatObj(
		m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::DATUM_E][iFAIParamNo][0],
		HDatumELine,
		&m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::DATUM_E][iFAIParamNo][0]);

	// East Bracket Hole 중심 십자 표시
	HObject HCrossHoleEast;
	GenCrossContourXld(&HCrossHoleEast, dBracketHoleEastCenter.y, dBracketHoleEastCenter.x, 30, 0);
	ConcatObj(
		m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::BRACKET_HOLE_CONCENTRICITY][iFAIParamNo][0],
		HCrossHoleEast,
		&m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][BOS_FAI::BRACKET_HOLE_CONCENTRICITY][iFAIParamNo][0]);

#ifdef LOCAL_DUMP_MODE
	HALCON_DUMP_XLD("BOS_FAI1_Debug", HCrossLensCenter, 2048, 2048);
	HALCON_DUMP_XLD("BOS_DatumD_Debug", HDatumELine, 2048, 2048);
	HALCON_DUMP_XLD("BOS_EastHole_Debug", HCrossHoleEast, 2048, 2048);
#endif
}

void BOSFAIInspector::SetCalibrationValues(int iInspectionBufferIdx, double *&dMultiple, double *&dOffset)
{
	if (!m_pAlgorithm)
		return;

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
}

BOOL BOSFAIInspector::FitCircle(
	CenterlineMeasureStruct *pStructFAIMeasure,
	enMeasurePos measurePointType,
	double &dCircleCenterX,
	double &dCircleCenterY,
	double &dCircleRadius)
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

		TRACE("[BOSFAIInspector] FitCircle: Measure Point %d - (%.2f, %.2f)\n", i, dPoint.x, dPoint.y);

		if (dPoint.x >= 0 && dPoint.y >= 0)
		{
			TupleConcat(HEdgeConcatX, HTuple(dPoint.x), &HEdgeConcatX);
			TupleConcat(HEdgeConcatY, HTuple(dPoint.y), &HEdgeConcatY);
		}
	}

	HTuple HEdgeCount;
	TupleLength(HEdgeConcatX, &HEdgeCount);
	int nValidPoints = HEdgeCount.L();
	if (nValidPoints != MAX_FAI_CIRCLE_FIT_POINT)
	{
		OutputDebugString(_T("[BOSFAIInspector] FitCircle: 불충분한 edge 포인트입니다.\n"));
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
		OutputDebugString(_T("[BOSFAIInspector] FitCircle: LeastSquareCircleFitting failed.\n"));
		return FALSE;
	}

	return TRUE;
}
