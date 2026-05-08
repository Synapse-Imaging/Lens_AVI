#pragma once

class CMeasureData
{
public:
	CMeasureData(void);
	virtual ~CMeasureData(void);

	// 표면검사
	int m_iSurfaceInspectionArea;
	double m_dSurfaceInspectionXLength;
	double m_dSurfaceInspectionYLength;

	// Line Fitting based on Edge Measure
	double m_dEdgeLineStartX;
	double m_dEdgeLineStartY;
	double m_dEdgeLineEndX;
	double m_dEdgeLineEndY;

	// 3D Height
	double m_dHeightMm;

	// Line Fitting
	double m_dLineFittingBurLength;
	int m_iLineFittingBurArea;
	double m_dLineFittingChippingLength;
	int m_iLineFittingChippingArea;

	// Part Check
	double m_dPartCheckShiftX;
	double m_dPartCheckShiftY;

	// Datacode Print Quality Grade
	BOOL m_bInspectPrintQuality;
	double	m_dDatacodePrintQualityGrade[MAX_PRINT_QUALITY_GRADE];
	CString	m_sDatacodePrintQualityValue[MAX_PRINT_QUALITY_GRADE];

	int m_iComponentDefectType;

	// Defect Region
	HObject m_HInspectTabDefectRgn;

public:
	void Reset();

	CMeasureData& operator=(CMeasureData& data);
};

