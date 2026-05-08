#include "stdafx.h"
#include "uScan.h"
#include "MeasureData.h"


CMeasureData::CMeasureData(void)
{
	Reset();
}


CMeasureData::~CMeasureData(void)
{
}

void CMeasureData::Reset()
{
	m_iSurfaceInspectionArea = 0;
	m_dSurfaceInspectionXLength = 0;
	m_dSurfaceInspectionYLength = 0;

	m_dEdgeLineStartX = -1;
	m_dEdgeLineStartY = -1;
	m_dEdgeLineEndX = -1;
	m_dEdgeLineEndY = -1;

	m_dHeightMm = HEIGHT_INIT;

	m_dLineFittingBurLength = 0;
	m_dLineFittingChippingLength = 0;
	m_iLineFittingBurArea = 0;
	m_iLineFittingChippingArea = 0;

	m_dPartCheckShiftX = 0;
	m_dPartCheckShiftY = 0;

	m_bInspectPrintQuality = FALSE;
	for (int i = 0; i < MAX_PRINT_QUALITY_GRADE; i++)
	{
		m_dDatacodePrintQualityGrade[i] = -1;
		m_sDatacodePrintQualityValue[i] = _T("");
	}

	m_iComponentDefectType = -1;

	GenEmptyObj(&m_HInspectTabDefectRgn);
}

CMeasureData& CMeasureData::operator=(CMeasureData& data)
{
	if (this == &data) return *this;

	m_iSurfaceInspectionArea = data.m_iSurfaceInspectionArea;
	m_dSurfaceInspectionXLength = data.m_dSurfaceInspectionXLength;
	m_dSurfaceInspectionYLength = data.m_dSurfaceInspectionYLength;

	m_dEdgeLineStartX = data.m_dEdgeLineStartX;
	m_dEdgeLineStartY = data.m_dEdgeLineStartY;
	m_dEdgeLineEndX = data.m_dEdgeLineEndX;
	m_dEdgeLineEndY = data.m_dEdgeLineEndY;

	m_dHeightMm = data.m_dHeightMm;

	m_dLineFittingBurLength = data.m_dLineFittingBurLength;
	m_dLineFittingChippingLength = data.m_dLineFittingChippingLength;
	m_iLineFittingBurArea = data.m_iLineFittingBurArea;
	m_iLineFittingChippingArea = data.m_iLineFittingChippingArea;

	m_dPartCheckShiftX = data.m_dPartCheckShiftX;
	m_dPartCheckShiftY = data.m_dPartCheckShiftY;

	m_bInspectPrintQuality = data.m_bInspectPrintQuality;
	for (int i = 0; i < MAX_PRINT_QUALITY_GRADE; i++)
	{
		m_dDatacodePrintQualityGrade[i] = data.m_dDatacodePrintQualityGrade[i];
		m_sDatacodePrintQualityValue[i] = data.m_sDatacodePrintQualityValue[i];
	}

	m_iComponentDefectType = data.m_iComponentDefectType;

	m_HInspectTabDefectRgn = data.m_HInspectTabDefectRgn;

	return *this;
}
