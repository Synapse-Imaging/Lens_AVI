#pragma once

class IInspectAdminViewHideDlg
{
public:
	int m_pMzNo;
	int m_pTrayNo;
	int m_pModuleNo;

	HObject m_HBarcodeDetectRgn;
	HObject m_HSpecApplyDefectRgn[MAX_DEFECT_NAME];
	HObject m_HRosReviewSpecApplyDefectRgn[MAX_INSPECTION_TYPE][MAX_DEFECT_NAME];

	int iDefectSortingNumber[MAX_DEFECT_NAME];

	BOOL m_bBarcodeRegResult;
	BOOL m_bBarcodeShiftNG;
	BOOL m_bBarcodeRotationNG;
	CString m_sBarcodeResult;
	double m_dBarcodePosOffsetX;
	double m_dBarcodePosOffsetY;
	int m_iBarcodeRotationResult;

	virtual void OverlaySetViewportManager(
		int iLeft,
		int iTop,
		double dZoomRatio
	) = 0;

	virtual int WriteResultFile(int iInspectionType,
		int iPcVisionNo,
		int iVisionCamType, 
		int iResultImageNo, 
		HObject HDisplayImage,
		HObject HInspectionTypeAlgoDetectionDefectRgn, 
		int *piDefectCodeIndex, 
		CString sBarcodeName
	) = 0;

	virtual int WriteSelectedRosReviewFile(
		int iPcVisionNo,
		int iVisionCamType,
		int iNoInspectionImage,
		HObject *pHDisplayImage,
		HObject *pHDefectRgn_ImageNo,
		CString sBarcodeID,
		int *piNoSaveReviewImage
	) = 0;

	virtual int WriteSelectedRosReviewFile_FAI(
		int iMzNo,
		int iTrayNo,
		int iModuleNo,
		int iPcVisionNo,
		int iVisionCamType,
		int iNoInspectionImage,
		HObject *pHDisplayImage,
		HObject(*ppHReviewRgn_FAI_Item)[10],
		POINT(*ppiViewportCenter_FAI_Item)[10],
		CString sBarcodeID,
		int *piNoSaveReviewImage
	) = 0;
};