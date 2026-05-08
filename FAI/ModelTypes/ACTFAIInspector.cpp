#include "StdAfx.h"
#include "uScan.h"
#include "Algorithm.h"
#include "FAIMeasureRegistry.h"
#include "ACTFAIInspector.h"
#include "../Dynamic/FAIDynamicRuleManager.h"
#include "math.h"

ACTFAIInspector::ACTFAIInspector(Algorithm *pAlgorithm)
	: m_pAlgorithm(pAlgorithm), m_registry(FAIMeasureRegistry::GetInstance())
{
	InitializeFAIMeasures();

}

void ACTFAIInspector::SetAlgorithm(Algorithm* pAlgorithm)
{
	m_pAlgorithm = pAlgorithm;
}

void ACTFAIInspector::InitializeFAIMeasures()
{
	constexpr int FAI5_ID = 5;
	constexpr int FAI9_ID = 9;
	constexpr int FAI10_ID = 10;
	constexpr int FAI11_ID = 11;
	constexpr int FAI13_ID = 13;
	constexpr int FAI14_ID = 14;
	constexpr int FAI19_ID = 19;
	constexpr int FAI20_ID = 20;
	constexpr int FAI21_ID = 21;
	constexpr int FAI22_ID = 22;
	constexpr int FAI24_ID = 24;
	constexpr int FAI26_ID = 26;
	constexpr int FAI27_ID = 27;
	constexpr int FAI29_ID = 29;
	constexpr int FAI31_ID = 31;
	constexpr int FAI32_ID = 32;
	constexpr int FAI33_ID = 33;
	constexpr int FAI34_ID = 34;
	constexpr int FAI44_ID = 44;
	constexpr int FAI124_ID = 124;

	FAIMeasureSpec specFAI5;
	specFAI5.id = FAI5_ID;
	specFAI5.type = FAIMeasureType::COMPOSITE;
	specFAI5.name = "FAI-5";
	m_registry.RegisterConfig(FAI5_ID, specFAI5);

	FAIMeasureSpec specFAI9;
	specFAI9.id = FAI9_ID;
	specFAI9.type = FAIMeasureType::COMPOSITE;
	specFAI9.name = "FAI-9";
	m_registry.RegisterConfig(FAI9_ID, specFAI9);

	FAIMeasureSpec specFAI10;
	specFAI10.id = FAI10_ID;
	specFAI10.type = FAIMeasureType::COMPOSITE;
	specFAI10.name = "FAI-10";
	m_registry.RegisterConfig(FAI10_ID, specFAI10);

	FAIMeasureSpec specFAI11;
	specFAI11.id = FAI11_ID;
	specFAI11.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI11.name = "FAI-11";
	m_registry.RegisterConfig(FAI11_ID, specFAI11);

	FAIMeasureSpec specFAI13;
	specFAI13.id = FAI13_ID;
	specFAI13.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI13.name = "FAI-13";
	m_registry.RegisterConfig(FAI13_ID, specFAI13);

	FAIMeasureSpec specFAI14;
	specFAI14.id = FAI14_ID;
	specFAI14.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI14.name = "FAI-14";
	m_registry.RegisterConfig(FAI14_ID, specFAI14);

	FAIMeasureSpec specFAI19;
	specFAI19.id = FAI19_ID;
	specFAI19.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI19.name = "FAI-19";
	m_registry.RegisterConfig(FAI19_ID, specFAI19);

	FAIMeasureSpec specFAI20;
	specFAI20.id = FAI20_ID;
	specFAI20.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI20.name = "FAI-20";
	m_registry.RegisterConfig(FAI20_ID, specFAI20);

	FAIMeasureSpec specFAI21;
	specFAI21.id = FAI21_ID;
	specFAI21.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI21.name = "FAI-21";
	m_registry.RegisterConfig(FAI21_ID, specFAI21);

	FAIMeasureSpec specFAI22;
	specFAI22.id = FAI22_ID;
	specFAI22.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI22.name = "FAI-22";
	m_registry.RegisterConfig(FAI22_ID, specFAI22);

	FAIMeasureSpec specFAI24;
	specFAI24.id = FAI24_ID;
	specFAI24.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI24.name = "FAI-24";
	m_registry.RegisterConfig(FAI24_ID, specFAI24);

	FAIMeasureSpec specFAI26;
	specFAI26.id = FAI26_ID;
	specFAI26.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI26.name = "FAI-26";
	m_registry.RegisterConfig(FAI26_ID, specFAI26);

	FAIMeasureSpec specFAI27;
	specFAI27.id = FAI27_ID;
	specFAI27.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI27.name = "FAI-27";
	m_registry.RegisterConfig(FAI27_ID, specFAI27);

	FAIMeasureSpec specFAI29;
	specFAI29.id = FAI29_ID;
	specFAI29.type = FAIMeasureType::COMPOSITE;
	specFAI29.name = "FAI-29";
	m_registry.RegisterConfig(FAI29_ID, specFAI29);

	FAIMeasureSpec specFAI31;
	specFAI31.id = FAI31_ID;
	specFAI31.type = FAIMeasureType::COMPOSITE;
	specFAI31.name = "FAI-31";
	m_registry.RegisterConfig(FAI31_ID, specFAI31);

	FAIMeasureSpec specFAI32;
	specFAI32.id = FAI32_ID;
	specFAI32.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI32.name = "FAI-32";
	m_registry.RegisterConfig(FAI32_ID, specFAI32);

	FAIMeasureSpec specFAI33;
	specFAI33.id = FAI33_ID;
	specFAI33.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI33.name = "FAI-33";
	m_registry.RegisterConfig(FAI33_ID, specFAI33);

	FAIMeasureSpec specFAI34;
	specFAI34.id = FAI34_ID;
	specFAI34.type = FAIMeasureType::COMPOSITE;
	specFAI34.name = "FAI-34";
	m_registry.RegisterConfig(FAI34_ID, specFAI34);

	FAIMeasureSpec specFAI44;
	specFAI44.id = FAI44_ID;
	specFAI44.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI44.name = "FAI-44";
	m_registry.RegisterConfig(FAI44_ID, specFAI44);

	FAIMeasureSpec specFAI124;
	specFAI124.id = FAI124_ID;
	specFAI124.type = FAIMeasureType::DISTANCE_TO_DATUM;
	specFAI124.name = "FAI-124";
	m_registry.RegisterConfig(FAI124_ID, specFAI124);
}

void ACTFAIInspector::Inspect(BOOL bIsAutoInsp, int iInspectionBufferIdx, int iFAIParamNo)
{
	if (!m_pAlgorithm)
		return;

	int iPcVisionNo = m_pAlgorithm->m_iVisionCamType;
	
	// FAI TOP ???? ???? ?? ??????.
	// ???? FAI ???? ?? ?????? ??. - 251222 jhkim
	if ((m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_2) && (m_pAlgorithm->m_iVisionCamType != VISION_NUMBER_4))
	{
		OutputDebugString(_T("FAI Bottom Axis Skipped!!\r\n"));
		return;
	}

	if (THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][iPcVisionNo]->m_bMUseFai == FALSE)
		return;

	CenterlineMeasureStruct *StructFAIMeasure;

	StructFAIMeasure = CFAIDataManager::GetInstance().GetMeasurePtr(
		bIsAutoInsp,
		m_pAlgorithm->m_nMzNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nTrayNo[iInspectionBufferIdx],
		m_pAlgorithm->m_nModuleNo[iInspectionBufferIdx],
		iFAIParamNo);

	// ?????? ????
	double *dMultiple;	
	double *dOffset;
	SetCalibrationValues(iInspectionBufferIdx, dMultiple, dOffset);

	// Measurement

	HTuple HIntersectPointX, HIntersectPointY, HIsOverlap;
	double dDistance, dAngle;
	double dDistanceX, dDistanceY;
	HTuple HDistance, HAngle;
	DPOINT dPoint1, dPoint2;
	DPOINT dLineStart, dLineEnd;
	DPOINT dLineStart2, dLineEnd2;

	// Fiducial Corner Points

	DPOINT dFiducialPismoSidePointA, dFiducialPismoSidePointB, dFiducialPismoSidePointC;
	DPOINT dFiducialSide3PointA, dFiducialSide3PointB;
	HObject HFiducialPismoSideLineA, HFiducialPismoSideLineB, HFiducialPismoSideLineC, HFiducialPismoSideLineD, HFiducialPismoSideLineE, HFiducialPismoSideLineF;
	HObject HFiducialSide3LineA, HFiducialSide3LineB, HFiducialSide3LineC, HFiducialSide3LineD;
	
	dFiducialPismoSidePointA.Reset();
	dFiducialPismoSidePointB.Reset();
	dFiducialPismoSidePointC.Reset();
	dFiducialSide3PointA.Reset();
	dFiducialSide3PointB.Reset();

	GenEmptyObj(&HFiducialPismoSideLineA);
	GenEmptyObj(&HFiducialPismoSideLineB);
	GenEmptyObj(&HFiducialPismoSideLineC);
	GenEmptyObj(&HFiducialPismoSideLineD);
	GenEmptyObj(&HFiducialPismoSideLineE);
	GenEmptyObj(&HFiducialPismoSideLineF);
	GenEmptyObj(&HFiducialSide3LineA);
	GenEmptyObj(&HFiducialSide3LineB);
	GenEmptyObj(&HFiducialSide3LineC);
	GenEmptyObj(&HFiducialSide3LineD);

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_A)][1].y;

	dLineStart2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B)][0].x;
	dLineStart2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B)][0].y;
	dLineEnd2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B)][1].x;
	dLineEnd2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_B)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineEnd2.x >= 0)
	{
		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		dFiducialPismoSidePointA.x = HIntersectPointX[0].D();
		dFiducialPismoSidePointA.y = HIntersectPointY[0].D();

		GenContourPolygonXld(&HFiducialPismoSideLineA, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		GenContourPolygonXld(&HFiducialPismoSideLineB, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
	}

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_C)][1].y;

	dLineStart2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D)][0].x;
	dLineStart2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D)][0].y;
	dLineEnd2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D)][1].x;
	dLineEnd2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_D)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineEnd2.x >= 0)
	{
		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		dFiducialPismoSidePointB.x = HIntersectPointX[0].D();
		dFiducialPismoSidePointB.y = HIntersectPointY[0].D();

		GenContourPolygonXld(&HFiducialPismoSideLineC, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		GenContourPolygonXld(&HFiducialPismoSideLineD, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
	}

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_E)][1].y;

	dLineStart2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F)][0].x;
	dLineStart2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F)][0].y;
	dLineEnd2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F)][1].x;
	dLineEnd2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_PISMO_SIDE_LINE_F)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineEnd2.x >= 0)
	{
		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		dFiducialPismoSidePointC.x = HIntersectPointX[0].D();
		dFiducialPismoSidePointC.y = HIntersectPointY[0].D();

		GenContourPolygonXld(&HFiducialPismoSideLineE, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		GenContourPolygonXld(&HFiducialPismoSideLineF, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
	}

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_A)][1].y;

	dLineStart2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_B)][0].x;
	dLineStart2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_B)][0].y;
	dLineEnd2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_B)][1].x;
	dLineEnd2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_B)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineEnd2.x >= 0)
	{
		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		dFiducialSide3PointA.x = HIntersectPointX[0].D();
		dFiducialSide3PointA.y = HIntersectPointY[0].D();

		GenContourPolygonXld(&HFiducialSide3LineA, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		GenContourPolygonXld(&HFiducialSide3LineB, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
	}

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_C)][1].y;

	dLineStart2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_D)][0].x;
	dLineStart2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_D)][0].y;
	dLineEnd2.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_D)][1].x;
	dLineEnd2.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FIDUCIAL_SIDE3_LINE_D)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineEnd2.x >= 0)
	{
		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		dFiducialSide3PointB.x = HIntersectPointX[0].D();
		dFiducialSide3PointB.y = HIntersectPointY[0].D();

		GenContourPolygonXld(&HFiducialSide3LineC, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		GenContourPolygonXld(&HFiducialSide3LineD, HTuple(dLineStart2.y).TupleConcat(HTuple(dLineEnd2.y)), HTuple(dLineStart2.x).TupleConcat(HTuple(dLineEnd2.x)));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Measurement

	double dMeasureLineStartX, dMeasureLineStartY, dMeasureLineEndX, dMeasureLineEndY;
	HTuple HlNoFoundNumber;
	Hlong lNoFoundNumber;
	double dIntersectionPosY;
	int iFAINumber;
	HObject HReviewXLD;

	// FAI-5
	iFAINumber = 5;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].y;

	dPoint1.x = dFiducialPismoSidePointA.x;
	dPoint1.y = dFiducialPismoSidePointA.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}
	
	// FAI-9
	iFAINumber = 9;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_UPPER)][1].y;

	dPoint1.x = dFiducialPismoSidePointA.x;
	dPoint1.y = dFiducialPismoSidePointA.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-10
	iFAINumber = 10;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].y;

	dLineStart2.x = dFiducialPismoSidePointA.x;
	dLineStart2.y = dFiducialPismoSidePointA.y;
	dLineEnd2.x = dFiducialPismoSidePointB.x;
	dLineEnd2.y = dFiducialPismoSidePointB.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
	{
		AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

		TupleDeg(HAngle, &HAngle);
		dAngle = HAngle[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dAngle * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineC, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialPismoSideLineD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-24
	iFAINumber = 24;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_24)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_24)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_24)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-29
	iFAINumber = 29;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].y;

	dLineStart2.x = dFiducialSide3PointB.x;
	dLineStart2.y = dFiducialSide3PointB.y;
	dLineEnd2.x = dFiducialSide3PointA.x;
	dLineEnd2.y = dFiducialSide3PointA.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dLineStart2.x >= 0 && dLineStart2.y >= 0)
	{
		AngleLl(dLineStart2.y, dLineStart2.x, dLineEnd2.y, dLineEnd2.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HAngle);

		TupleDeg(HAngle, &HAngle);
		dAngle = HAngle[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dAngle * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;
		
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineC, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-31
	iFAINumber = 31;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_SIDE_LOWER)][1].y;

	dPoint1.x = dFiducialSide3PointA.x;
	dPoint1.y = dFiducialSide3PointA.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialSide3LineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HFiducialSide3LineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-34
	iFAINumber = 34;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].y;

	dPoint1.x = dFiducialSide3PointA.x;
	dPoint1.y = dFiducialSide3PointA.y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineA, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], HFiducialSide3LineB, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-11A
	iFAINumber = 11;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_11)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_11)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_11)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-11B
	iFAINumber = 11;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_11)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_11)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_11)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-13A
	iFAINumber = 13;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_13)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_13)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_13)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-13B
	iFAINumber = 13;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_13)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_13)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_13)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-14A
	iFAINumber = 14;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_14)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_14)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_14)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-14B
	iFAINumber = 14;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_FAI_14)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_14)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_14)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_14)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-26A
	iFAINumber = 26;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_26)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_26)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_26)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-26B
	iFAINumber = 26;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_26)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_26)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_26)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-27A
	iFAINumber = 27;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_27)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_27)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_27)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-27B
	iFAINumber = 27;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_UPPER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_27)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_27)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_27)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-32A
	iFAINumber = 32;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_32)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_32)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_32)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-32B
	iFAINumber = 32;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_A_SIDE_LOWER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_32)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_32)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y > dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_32)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-33A
	iFAINumber = 33;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_33)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_33)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_33)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-33B
	iFAINumber = 33;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_D_SIDE_LOWER)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_33)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_33)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		dMeasureLineStartX = dPoint1.x;
		dMeasureLineStartY = dPoint1.y - 1000;
		dMeasureLineEndX = dPoint1.x;
		dMeasureLineEndY = dPoint1.y + 1000;

		IntersectionLines(dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, dMeasureLineStartY, dMeasureLineStartX, dMeasureLineEndY, dMeasureLineEndX, &HIntersectPointY, &HIntersectPointX, &HIsOverlap);

		TupleLength(HIntersectPointX, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber.L();
		if (lNoFoundNumber > 0)
		{
			dIntersectionPosY = HIntersectPointY[0].D();

			if (dPoint1.y < dIntersectionPosY)
				dDistance = -dDistance;
		}

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_33)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// FAI-19
	iFAINumber = 19;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_19)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_19)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_19)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-20
	iFAINumber = 20;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_20)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_20)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_20)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-21
	iFAINumber = 21;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_21)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_21)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_21)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-22
	iFAINumber = 22;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_C_TOP)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_22)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_22)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_22)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
	}

	// FAI-44
	iFAINumber = 44;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_TOP)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_44)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_44)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_44)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-124A
	iFAINumber = 124;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_124)][0].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_124)][0].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][0] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		GenContourPolygonXld(&HReviewXLD, HTuple(dLineStart.y).TupleConcat(HTuple(dLineEnd.y)), HTuple(dLineStart.x).TupleConcat(HTuple(dLineEnd.x)));

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].x = (long)dLineStart.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][0].y = (long)dLineStart.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1], HReviewXLD, &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1]);
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].x = (long)dLineEnd.x;
		m_pAlgorithm->m_iViewportCenter_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][1].y = (long)dLineEnd.y;

		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_124)][0], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][2]);
	}

	// FAI-124B
	iFAINumber = 124;

	dLineStart.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][0].x;
	dLineStart.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][0].y;
	dLineEnd.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][1].x;
	dLineEnd.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_DATUM_B_BOTTOM)][1].y;

	dPoint1.x = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_124)][1].x;
	dPoint1.y = StructFAIMeasure->m_dMeasureCPoint[MtoI(enMeasurePos::MEASURE_FAI_124)][1].y;

	if (dLineStart.x >= 0 && dLineEnd.x >= 0 && dPoint1.x >= 0 && dPoint1.y >= 0)
	{
		DistancePl(dPoint1.y, dPoint1.x, dLineStart.y, dLineStart.x, dLineEnd.y, dLineEnd.x, &HDistance);
		dDistance = HDistance[0].D();

		StructFAIMeasure->m_dFAIMeasureValue[iFAINumber][1] = dDistance * THEAPP.m_pCalDataService_N[m_pAlgorithm->m_iVisionCamType]->GetPixelSize() * 0.001 * dMultiple[iFAINumber] + dOffset[iFAINumber];

		// Review XLD
		ConcatObj(m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3], StructFAIMeasure->m_HMeasureXLD[MtoI(enMeasurePos::MEASURE_FAI_124)][1], &m_pAlgorithm->m_HReviewXLD_FAI_Item[iInspectionBufferIdx][iFAINumber][iFAIParamNo][3]);
	}

	// Dynamic P5/P7 rules (runtime-configurable, no recompile needed)
	auto& dynMgr = FAIDynamicRuleManager::GetInstance();
	dynMgr.ExecuteP5Rules(_T("ACT"), StructFAIMeasure, iInspectionBufferIdx, iFAIParamNo, m_pAlgorithm);
	dynMgr.ExecuteP7Rules(_T("ACT"), StructFAIMeasure, iInspectionBufferIdx, iFAIParamNo, m_pAlgorithm);
}

void ACTFAIInspector::SetCalibrationValues(int iInspectionBufferIdx, double *&dMultiple, double *&dOffset)
{
	if (!m_pAlgorithm)
		return;

#ifdef POC_TEST

	dMultiple = THEAPP.m_pModelDataManager->m_dMMultipleStg1Fai;
	dOffset = THEAPP.m_pModelDataManager->m_dMOffsetStg1Fai;

#else
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
#endif
}
