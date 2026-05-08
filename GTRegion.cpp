#include "stdafx.h"
#include "math.h"
#include "GTRegion.h"
#include "uScan.h"
#include "FAIDataManager.h"

IMPLEMENT_DYNCLASS(GTRegion)

GTRegion::GTRegion()
{
	Reset();
}

GTRegion::~GTRegion()
{
	if (m_HROIAlignModelID >= 0)
	{
		ClearShapeModel(m_HROIAlignModelID);
		m_HROIAlignModelID = -1;
	}

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (m_HVarModelID[i] >= 0)
		{
			ClearVariationModel(m_HVarModelID[i]);
			m_HVarModelID[i] = -1;
		}

		GenEmptyObj(m_HVarMeanImage + i);
		GenEmptyObj(m_HVarStdevImage + i);
	}

	HTuple HNoTemplate;
	Hlong lNoTemplate;

	TupleLength(m_HPartModelID, &HNoTemplate);
	lNoTemplate = HNoTemplate.L();

	for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
	{
		if (m_HPartModelID[iIndex] >= 0)
		{
			ClearNccModel(m_HPartModelID[iIndex]);
			m_HPartModelID[iIndex] = -1;
		}
	}

	GenEmptyObj(&m_HTeachPolygonRgn);
	GenEmptyObj(&m_HADJImage);

	delete mpChildTRegionList;
}

void GTRegion::Reset()
{
	mpChildTRegionList = new PList<GTRegion>(PLNO_CONTENT);

	miInspectionType = INSPECTION_TYPE_UNDEFINED;
	miFaiRoiIndex = FAI_PARAMETER_NUMBER_1;
	miTeachImageIndex = -1;
	m_bRegionROI = FALSE;

	GenEmptyObj(&m_HTeachPolygonRgn);

	int i, j;

	for (i = 0; i < MAX_INSPECTION_TAB; i++)
		m_AlgorithmParam[i].Reset();

	for (i = 0; i < INSPECTION_BUFFER_COUNT_MAX; i++)
	{
		GenEmptyObj(&(m_HVariationInspectImage[i]));

		for (j = 0; j < MAX_INSPECTION_TAB; j++)
			m_MeasureData[i][j].Reset();
	}

	mbVisible = TRUE;
	mbSelectable = TRUE;
	mbMovable = TRUE;
	mbSizable = TRUE;
	mbSelect = FALSE;
	mbShape = FALSE;		// Added by Gojw	2005/11/25

	miInspectionROIID = -1;
	miLocalAlignID = -1;
	miDontCareID = -1;
	miGenerateID = -1;
	miMaskID = -1;
	miColorInfoID = -1;
	miLineStyle = PS_SOLID;
	miLineThickness = 1;
	mLineColor = RGB(0, 255, 0);
	mBackColor = RGB(0, 0, 0);
	miBkMode = TRANSPARENT;

	miRegionStyle = GTR_RS_NULL;
	mRegionColor = RGB(255, 255, 255);

	ZeroMemory(maPoints, sizeof(DPOINT) * 2);

	maPoints[0].x = maPoints[0].y = 0;
	maPoints[1].x = maPoints[1].y = 0;

	mbLastSelected = FALSE;

	m_HROIAlignModelID = -1;
	for (i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		m_HVarModelID[i] = -1;
		m_iVarModelImageSizeX[i] = -1;
		m_iVarModelImageSizeY[i] = -1;
		m_iVarNoTrainSample[i] = 0;
		GenEmptyObj(m_HVarMeanImage + i);
		GenEmptyObj(m_HVarStdevImage + i);
	}
	m_HPartModelID = -1;

	for (i = 0; i < NO_MAX_INSPECT_THREAD; i++)
	{
		ResetLocalAlignResult(i);

		GenEmptyObj(&m_HDontCareResultRgn[i]);
		GenEmptyObj(&m_HGenerateResultRgn[i]);
		GenEmptyObj(&m_HMaskResultRgn[i]);
	}

	mbAiRetrainFlag = FALSE;

	miPositionID = -1;
	miPointID = -1;

	ResetVmOptData();
}

BOOL GTRegion::Load(HANDLE hFile, int iModelVersion, HTuple HFileHandle, CCalDataService *pCalDataService, int iVisionIdx, CString strConfig)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	long lTemp;
	BOOL bTemp;
	int iTemp;

	if (miInspectionType == INSPECTION_TYPE_CROSS_BAR)
		return TRUE;

	DWORD dwBytesRead;

	ReadFile(hFile, &miTeachImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miInspectionType, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion < 2017)
	{
		if (miInspectionType == 1)			// Barrel
			miInspectionType = 2;
		else if (miInspectionType == 2)		// Module
			miInspectionType = 1;
		else if (miInspectionType == 3)		// FPCB
			miInspectionType = 7;
		else if (miInspectionType == 4)		// Barcode
			miInspectionType = 4;
		else if (miInspectionType == 5)		// Stiffener Bottom
			miInspectionType = 6;
		else if (miInspectionType == 6)		// Connector
			miInspectionType = 22;
		else if (miInspectionType == 7)		// Connector Stiffener
			miInspectionType = 23;
		else if (miInspectionType == 8)		// VCM Top
			miInspectionType = 5;
		else if (miInspectionType == 9)		// VCM Side
			miInspectionType = 10;
		else if (miInspectionType == 10)	// Stiffener Side
			miInspectionType = 14;

		else if (miInspectionType == 15)	// AF Align
			miInspectionType = 173;
		else if (miInspectionType == 16)	// AF Zig Rotation Center
			miInspectionType = 174;
		else if (miInspectionType == 17)	// AF North Line
			miInspectionType = 175;
		else if (miInspectionType == 18)	// AF West line
			miInspectionType = 176;
		else if (miInspectionType == 19)	// ROI Delete
			miInspectionType = 161;
		else if (miInspectionType == 20)	// ROI Add
			miInspectionType = 162;
		else if (miInspectionType == 21)	// ROI Mask
			miInspectionType = 163;
		else if (miInspectionType == 22)	// Unsed
			miInspectionType = 164;
		else if (miInspectionType == 23)	// Global align
			miInspectionType = 165;
		else if (miInspectionType == 24)	// Local align
			miInspectionType = 166;
		else if (miInspectionType == 25)	// Local align angle
			miInspectionType = 167;
		else if (miInspectionType == 26)	// Light ROI
			miInspectionType = 168;
		else if (miInspectionType == 27)	// Focus ROI
			miInspectionType = 169;
		else if (miInspectionType == 28)	// Color ROI
			miInspectionType = 170;
		else if (miInspectionType == 29)	// Don't care ROI
			miInspectionType = 171;
		else if (miInspectionType == 30)	// Generate ROI
			miInspectionType = 172;
	}

	if (iModelVersion >= 1131)
		ReadFile(hFile, &miFaiRoiIndex, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bRegionROI, sizeof(BOOL), &dwBytesRead, NULL);
	if (iModelVersion < 2016)
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);

	if (iModelVersion >= 2022)
	{
		ReadFile(hFile, &miPositionID, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miPointID, sizeof(int), &dwBytesRead, NULL);
	}

	BOOL bValidRgn;
	HTuple HRow, HColumnBegin, HColumnEnd;
	int iRegionPointNum;
	HObject HTempRgn;

	ReadFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesRead, NULL);
	if (bValidRgn)
	{
		HRow = HTuple();
		HColumnBegin = HTuple();
		HColumnEnd = HTuple();

		ReadFile(hFile, &iRegionPointNum, sizeof(int), &dwBytesRead, NULL);
		for (int i = 0; i < iRegionPointNum; i++) {
			ReadFile(hFile, &lTemp, sizeof(long), &dwBytesRead, NULL);
			TupleConcat(HRow, lTemp, &HRow);
			ReadFile(hFile, &lTemp, sizeof(long), &dwBytesRead, NULL);
			TupleConcat(HColumnBegin, lTemp, &HColumnBegin);
			ReadFile(hFile, &lTemp, sizeof(long), &dwBytesRead, NULL);
			TupleConcat(HColumnEnd, lTemp, &HColumnEnd);
		}

		if (iRegionPointNum > 0)
			GenRegionRuns(&m_HTeachPolygonRgn, HRow, HColumnBegin, HColumnEnd);
	}

	ReadFile(hFile, &mbVisible, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &mbSelectable, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &mbMovable, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &mbSizable, sizeof(BOOL), &dwBytesRead, NULL);

	if (miInspectionType < INSPECTION_TYPE_FIRST || miInspectionType > INSPECTION_TYPE_LAST)
	{
		int iDump;
		ReadFile(hFile, &iDump, sizeof(int), &dwBytesRead, NULL);
		miInspectionROIID = -1;
	}
	else
		ReadFile(hFile, &miInspectionROIID, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &miLocalAlignID, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miDontCareID, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miGenerateID, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1107)
		ReadFile(hFile, &miMaskID, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &miColorInfoID, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &miLineStyle, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miLineThickness, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &mLineColor, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &mBackColor, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miBkMode, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &miRegionStyle, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &mRegionColor, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &maPoints[0].x, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &maPoints[0].y, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &maPoints[1].x, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &maPoints[1].y, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &mbShape, sizeof(BOOL), &dwBytesRead, NULL);

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (iModelVersion < 2006 && i != 1)
			continue;

		if (iModelVersion < 2020 && i >= 3)
			break;

		ReadFile(hFile, &m_iVarModelImageSizeX[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iVarModelImageSizeY[i], sizeof(int), &dwBytesRead, NULL);
	}

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (iModelVersion < 2020 && i >= 3)
			break;

		if (pCalDataService == NULL)
			m_AlgorithmParam[i].Load(hFile, iModelVersion, FALSE);
		else
			m_AlgorithmParam[i].Load(hFile, iModelVersion, TRUE);

		THEAPP.DoEvents();

		if (i == 0 && (m_AlgorithmParam[i].m_bUseROIAnisoAlign || m_AlgorithmParam[i].m_bUseROIAlign) && pCalDataService != NULL)		// 1st 검사탭
		{
			if (m_HROIAlignModelID >= 0)
			{
				ClearShapeModel(m_HROIAlignModelID);
				m_HROIAlignModelID = -1;
			}

			HObject HInspectAreaRgn;
			HInspectAreaRgn = GetROIHRegion(pCalDataService);

			HObject HInspectAreaXLD;
			GenContourRegionXld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

			if (m_AlgorithmParam[i].m_bUseROIAnisoAlign)
			{
				HTuple dAngleRangeRad;
				TupleRad((double)m_AlgorithmParam[i].m_iROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

				if (m_AlgorithmParam[i].m_bUseAnisoColorImage)
				{
					int iColorMinContrast = (int)(sqrt(3.0) * (double)m_AlgorithmParam[i].m_iROIAnisoAlignMatchingMinContrast);
					CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinY, m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxY, "auto", m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinX, m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxX, "auto", "auto", "ignore_color_polarity", iColorMinContrast, &m_HROIAlignModelID);
				}
				else
					CreateAnisoShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinY, m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxY, "auto", m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinX, m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxX, "auto", "auto", "ignore_local_polarity", m_AlgorithmParam[i].m_iROIAnisoAlignMatchingMinContrast, &m_HROIAlignModelID);
			}
			else if (m_AlgorithmParam[i].m_bUseROIAlign)
			{
				HTuple dAngleRangeRad;
				TupleRad((double)m_AlgorithmParam[i].m_iROIAlignMatchingAngleRange, &dAngleRangeRad);

				if (m_AlgorithmParam[i].m_bUseIsoColorImage)
				{
					int iColorMinContrast = (int)(sqrt(3.0) * (double)m_AlgorithmParam[i].m_iROIAlignMatchingMinContrast);
					CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_AlgorithmParam[i].m_dROIAlignMatchingScaleMin, m_AlgorithmParam[i].m_dROIAlignMatchingScaleMax, "auto", "auto", "ignore_color_polarity", iColorMinContrast, &m_HROIAlignModelID);
				}
				else
					CreateScaledShapeModelXld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_AlgorithmParam[i].m_dROIAlignMatchingScaleMin, m_AlgorithmParam[i].m_dROIAlignMatchingScaleMax, "auto", "auto", "ignore_local_polarity", m_AlgorithmParam[i].m_iROIAlignMatchingMinContrast, &m_HROIAlignModelID);
			}
		}
	}

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (iModelVersion < 2006 && i != 1)
			continue;

		if (iModelVersion < 2020 && i >= 3)
			break;

		BOOL bVarModelExist;
		ReadFile(hFile, &bVarModelExist, sizeof(BOOL), &dwBytesRead, NULL);
		if (bVarModelExist && HFileHandle != -1)
		{
			HTuple HSerializedItem;
			if (iModelVersion < 2019)
			{
				FreadSerializedItem(HFileHandle, &HSerializedItem);
				DeserializeVariationModel(HSerializedItem, &m_HVarModelID[i]);
			}
			else
			{
				CString strVariationFolder, strVariationFile;
				strVariationFolder.Format("%s\\Model_Variation\\%s_%s", THEAPP.GetWorkingDirectory(), THEAPP.g_strModelTypeName[THEAPP.GetModelType()], strConfig);
				strVariationFile.Format("%s\\%s_Img%02d_ROI%d_Tab%d.var", strVariationFolder, sVisionCamType_Short, miTeachImageIndex, miInspectionROIID, i + 1);

				if (GetFileAttributes(strVariationFile) != -1)
					ReadVariationModel((HTuple)strVariationFile, &m_HVarModelID[i]);

			}

			if (iModelVersion >= 2007)
			{
				int iNoTrainImage;
				ReadFile(hFile, &iNoTrainImage, sizeof(int), &dwBytesRead, NULL);
				m_iVarNoTrainSample[i] = iNoTrainImage;

				if (iNoTrainImage > 0)
				{
					HTuple HSerializedItem;
					FreadSerializedItem(HFileHandle, &HSerializedItem);
					DeserializeImage(&(m_HVarMeanImage[i]), HSerializedItem);
					FreadSerializedItem(HFileHandle, &HSerializedItem);
					DeserializeImage(&(m_HVarStdevImage[i]), HSerializedItem);
				}
			}
		}
	}

	BOOL bPartModelExist;
	ReadFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesRead, NULL);
	if (bPartModelExist && HFileHandle != -1)
	{
		int iNoPartModel = 1;

		if (iModelVersion >= 1123)
			ReadFile(hFile, &iNoPartModel, sizeof(int), &dwBytesRead, NULL);

		HTuple HSerializedItem, HTempPartModel;

		TupleGenConst(0, 0, &m_HPartModelID);

		for (int iIndex = 0; iIndex < iNoPartModel; iIndex++)
		{
			if (iModelVersion >= 2019 && miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			{
				CString strTemplateFolder, strTemplateFile;
				strTemplateFolder.Format("%s\\Model_Template\\%s_%s", THEAPP.GetWorkingDirectory(), THEAPP.g_strModelTypeName[THEAPP.GetModelType()], strConfig);
				strTemplateFile.Format("%s\\%s_Img%02d_ROI%d.ncm", strTemplateFolder, sVisionCamType_Short, miTeachImageIndex, miLocalAlignID);

				if (GetFileAttributes(strTemplateFile) != -1)
					ReadNccModel((HTuple)strTemplateFile, &m_HPartModelID);
			}
			else
			{
				FreadSerializedItem(HFileHandle, &HSerializedItem);
				DeserializeNccModel(HSerializedItem, &HTempPartModel);
				TupleConcat(m_HPartModelID, HTempPartModel, &m_HPartModelID);
			}
		}
	}

	if (bPartModelExist && HFileHandle == -1)
	{
		int iTemp;
		if (iModelVersion >= 1123)
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
	}

	int iChildTRegionCount;
	ReadFile(hFile, &iChildTRegionCount, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion < 2016)
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);


	if (iModelVersion >= 1042 && iModelVersion < 2016)
	{
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 1086 && iModelVersion < 2016)
		ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);

	if (iModelVersion >= 1131 && iModelVersion < 2016)
	{
		ReadFile(hFile, &bTemp, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
		for (int i = 0; i < 4; i++)
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
	}

	if (iModelVersion >= 2015 && iModelVersion < 2022)
	{
		int iTemp;

		if (miInspectionType == INSPECTION_TYPE_MEASURE_POINT)
		{
			ReadFile(hFile, &miPositionID, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &miPointID, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
		}
		else if (miInspectionType == INSPECTION_TYPE_BLEND_POINT)
		{
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &miPositionID, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &miPointID, sizeof(int), &dwBytesRead, NULL);
		}
		else
		{
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &iTemp, sizeof(int), &dwBytesRead, NULL);

			miPositionID = 0;
			miPointID = -1;
		}
	}
	

	GTRegion *pTRegion;
	PString ClassName;
	for (int i = 0; i < iChildTRegionCount; i++)
	{
		ClassName.Load(hFile);
		pTRegion = (GTRegion*)PGetDynClass(ClassName.mcpStr)->CreateObject();
		if (!pTRegion->Load(hFile, iModelVersion, HFileHandle, pCalDataService, iVisionIdx, strConfig))
			return FALSE;
		AddChildTRegion(pTRegion);
	}

	return TRUE;
}

void GTRegion::Save(HANDLE hFile, HTuple HFileHandle, BOOL bVariationModelSave, BOOL bTemplateModelSave, int iVisionIdx, CString strConfig)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[iVisionIdx];

	if (miInspectionType == INSPECTION_TYPE_CROSS_BAR)
		return;

	DWORD dwBytesWritten;

	WriteFile(hFile, &miTeachImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miInspectionType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miFaiRoiIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bRegionROI, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &miPositionID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miPointID, sizeof(int), &dwBytesWritten, NULL);

	BOOL bValidRgn;
	HTuple HRow, HColumnBegin, HColumnEnd;
	int iRegionPointNum;
	long lTemp;

	if (THEAPP.m_pGFunction->ValidHRegion(m_HTeachPolygonRgn))
	{
		bValidRgn = TRUE;
		WriteFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesWritten, NULL);

		GetRegionRuns(m_HTeachPolygonRgn, &HRow, &HColumnBegin, &HColumnEnd);
		iRegionPointNum = HRow.Length();
		if (iRegionPointNum <= 0)
			iRegionPointNum = 0;

		WriteFile(hFile, &iRegionPointNum, sizeof(int), &dwBytesWritten, NULL);
		for (int i = 0; i < iRegionPointNum; i++)
		{
			lTemp = HRow[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);

			lTemp = HColumnBegin[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);

			lTemp = HColumnEnd[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);
		}
	}
	else
	{
		bValidRgn = FALSE;
		WriteFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &mbVisible, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbSelectable, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbMovable, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbSizable, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &miInspectionROIID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLocalAlignID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miDontCareID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miGenerateID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miMaskID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miColorInfoID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLineStyle, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLineThickness, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &mLineColor, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &mBackColor, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miBkMode, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miRegionStyle, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &mRegionColor, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &maPoints[0].x, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[0].y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[1].x, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[1].y, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &mbShape, sizeof(BOOL), &dwBytesWritten, NULL);

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		WriteFile(hFile, &m_iVarModelImageSizeX[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iVarModelImageSizeY[i], sizeof(int), &dwBytesWritten, NULL);
	}

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
		m_AlgorithmParam[i].Save(hFile, iVisionIdx, miTeachImageIndex, miInspectionROIID, i);

	BOOL bVarModelExist, bPartModelExist;
	int iNoPartModel = 0;

	// 모델이 있으면 저장
	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (m_HVarModelID[i] >= 0)
		{
			bVarModelExist = TRUE;
			WriteFile(hFile, &bVarModelExist, sizeof(BOOL), &dwBytesWritten, NULL);

			// Variation 모델 저장
			if (m_HVarModelID[i] >= 0)
			{
				if (bVariationModelSave)
				{
					HANDLE hFindFile;
					WIN32_FIND_DATA FindFileData;

					CString strVariationFolder, strVariationFile;
					strVariationFolder.Format("%s\\Model_Variation", THEAPP.GetWorkingDirectory());
					if ((hFindFile = FindFirstFile(strVariationFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
						CreateDirectory(strVariationFolder, NULL);
					strVariationFolder.Format("%s\\%s_%s", strVariationFolder, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], strConfig);
					if ((hFindFile = FindFirstFile(strVariationFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
						CreateDirectory(strVariationFolder, NULL);
					strVariationFile.Format("%s\\%s_Img%02d_ROI%d_Tab%d.var", strVariationFolder, sVisionCamType_Short, miTeachImageIndex, miInspectionROIID, i + 1);
					WriteVariationModel(m_HVarModelID[i], (HTuple)strVariationFile);
				}

				THEAPP.m_StructVariationParameter.bUseImageCompare[iVisionIdx][miTeachImageIndex][miInspectionROIID][i] = TRUE;
				THEAPP.m_StructDefectCondition.bUseImageCompare[iVisionIdx][miTeachImageIndex][miInspectionROIID][i] = TRUE;
			}

			WriteFile(hFile, &(m_iVarNoTrainSample[i]), sizeof(int), &dwBytesWritten, NULL);

			if (m_iVarNoTrainSample[i] > 0)
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_HVarMeanImage[i]))
				{
					HTuple HSerializedItem;
					SerializeImage(m_HVarMeanImage[i], &HSerializedItem);
					FwriteSerializedItem(HFileHandle, HSerializedItem);
				}

				if (THEAPP.m_pGFunction->ValidHImage(m_HVarStdevImage[i]))
				{
					HTuple HSerializedItem;
					SerializeImage(m_HVarStdevImage[i], &HSerializedItem);
					FwriteSerializedItem(HFileHandle, HSerializedItem);
				}
			}
		}
		else
		{
			bVarModelExist = FALSE;
			WriteFile(hFile, &bVarModelExist, sizeof(BOOL), &dwBytesWritten, NULL);
		}
	}

	// 모델이 있으면 저장
	if (m_HPartModelID >= 0)
	{
		bPartModelExist = TRUE;
		WriteFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesWritten, NULL);

		HTuple HNoTemplate;
		Hlong lNoTemplate;
		TupleLength(m_HPartModelID, &HNoTemplate);
		lNoTemplate = HNoTemplate.L();
		iNoPartModel = lNoTemplate;
		WriteFile(hFile, &iNoPartModel, sizeof(int), &dwBytesWritten, NULL);

		HTuple HSerializedItem;

		for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
		{
			if (miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
			{
				SerializeNccModel(m_HPartModelID[iIndex], &HSerializedItem);
				FwriteSerializedItem(HFileHandle, HSerializedItem);
			}
			else
			{
				if (bTemplateModelSave)
				{
					HANDLE hFindFile;
					WIN32_FIND_DATA FindFileData;

					CString strTemplateFolder, strTemplateFile;
					strTemplateFolder.Format("%s\\Model_Template", THEAPP.GetWorkingDirectory());
					if ((hFindFile = FindFirstFile(strTemplateFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
						CreateDirectory(strTemplateFolder, NULL);
					strTemplateFolder.Format("%s\\%s_%s", strTemplateFolder, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], strConfig);
					if ((hFindFile = FindFirstFile(strTemplateFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
						CreateDirectory(strTemplateFolder, NULL);
					strTemplateFile.Format("%s\\%s_Img%02d_ROI%d.ncm", strTemplateFolder, sVisionCamType_Short, miTeachImageIndex, miLocalAlignID);
					WriteNccModel(m_HPartModelID, (HTuple)strTemplateFile);
				}
			}
		}
	}
	else
	{
		bPartModelExist = FALSE;
		WriteFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesWritten, NULL);
	}

	int iChildTRegionCount = GetChildTRegionCount();
	WriteFile(hFile, &iChildTRegionCount, sizeof(int), &dwBytesWritten, NULL);

	GTRegion *pTRegion;
	for (int i = 0; i < iChildTRegionCount; i++) {
		pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->GetDynClass()->mName.Save(hFile);
		pTRegion->Save(hFile, HFileHandle, bVariationModelSave, bTemplateModelSave, iVisionIdx, strConfig);
	}
}

void GTRegion::Duplicate(GTRegion **ppTRegion, BOOL bOnlyParameter)
{
	GTRegion *pTRegion, *pTRegionDup;

	if (bOnlyParameter == FALSE)
	{
		if (GetDynClass()->mName == "GTRegion")
			*ppTRegion = new GTRegion;

		(*ppTRegion)->miTeachImageIndex = miTeachImageIndex;
		(*ppTRegion)->miFaiRoiIndex = miFaiRoiIndex;
		(*ppTRegion)->m_bRegionROI = m_bRegionROI;
		(*ppTRegion)->mbAiRetrainFlag = mbAiRetrainFlag;
		(*ppTRegion)->miPositionID = miPositionID;
		(*ppTRegion)->miPointID = miPointID;
		(*ppTRegion)->m_HTeachPolygonRgn = m_HTeachPolygonRgn;
	}

	(*ppTRegion)->miInspectionType = miInspectionType;

	int i, j;

	for (i = 0; i < MAX_INSPECTION_TAB; i++)
		(*ppTRegion)->m_AlgorithmParam[i] = m_AlgorithmParam[i];

	if (bOnlyParameter == FALSE)
	{
		(*ppTRegion)->mbVisible = mbVisible;
		(*ppTRegion)->mbSelectable = mbSelectable;
		(*ppTRegion)->mbSizable = mbSizable;
		(*ppTRegion)->mbMovable = mbMovable;
		(*ppTRegion)->mbSelect = mbSelect;
		(*ppTRegion)->mbLastSelected = mbLastSelected;
		(*ppTRegion)->mbShape = mbShape;

		(*ppTRegion)->miInspectionROIID = miInspectionROIID;
		(*ppTRegion)->miLocalAlignID = miLocalAlignID;
		(*ppTRegion)->miDontCareID = miDontCareID;
		(*ppTRegion)->miGenerateID = miGenerateID;
		(*ppTRegion)->miMaskID = miMaskID;
		(*ppTRegion)->miColorInfoID = miColorInfoID;
		(*ppTRegion)->miLineStyle = miLineStyle;
		(*ppTRegion)->miLineThickness = miLineThickness;
		(*ppTRegion)->mLineColor = mLineColor;
		(*ppTRegion)->mBackColor = mBackColor;
		(*ppTRegion)->miBkMode = miBkMode;
		(*ppTRegion)->miRegionStyle = miRegionStyle;
		(*ppTRegion)->mRegionColor = mRegionColor;

		(*ppTRegion)->maPoints[0] = maPoints[0];
		(*ppTRegion)->maPoints[1] = maPoints[1];

		for (i = 0; i < MAX_INSPECTION_TAB; i++)
		{
			(*ppTRegion)->m_iVarModelImageSizeX[i] = m_iVarModelImageSizeX[i];
			(*ppTRegion)->m_iVarModelImageSizeY[i] = m_iVarModelImageSizeY[i];
		}
	}

	if (m_HROIAlignModelID >= 0)
	{
		CString sModelPath = THEAPP.GetWorkingDirectory() + "\\CopyModel";
		WriteShapeModel(m_HROIAlignModelID, HTuple(sModelPath));
		ReadShapeModel(HTuple(sModelPath), &((*ppTRegion)->m_HROIAlignModelID));
	}

	if (m_HPartModelID >= 0)
	{
		CString sModelPath = THEAPP.GetWorkingDirectory() + "\\CopyModel";

		HTuple HNoTemplate;
		Hlong lNoTemplate;
		TupleLength(m_HPartModelID, &HNoTemplate);
		lNoTemplate = HNoTemplate.L();

		if (lNoTemplate == 1)
		{
			WriteNccModel(m_HPartModelID, HTuple(sModelPath));
			ReadNccModel(HTuple(sModelPath), &((*ppTRegion)->m_HPartModelID));
		}
		else
		{
			HTuple HTempReadModel;

			TupleGenConst(0, 0, &((*ppTRegion)->m_HPartModelID));

			for (int iIndex = 0; iIndex < lNoTemplate; iIndex++)
			{
				WriteNccModel(m_HPartModelID[iIndex], HTuple(sModelPath));
				ReadNccModel(HTuple(sModelPath), &HTempReadModel);
				TupleConcat((*ppTRegion)->m_HPartModelID, HTempReadModel, &((*ppTRegion)->m_HPartModelID));
			}
		}
	}

	for (i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		if (m_HVarModelID[i] >= 0)
		{
			CString sModelPath = THEAPP.GetWorkingDirectory() + "\\CopyModel";
			WriteVariationModel(m_HVarModelID[i], HTuple(sModelPath));
			ReadVariationModel(HTuple(sModelPath), &((*ppTRegion)->m_HVarModelID[i]));

			(*ppTRegion)->m_iVarNoTrainSample[i] = m_iVarNoTrainSample[i];

			if (THEAPP.m_pGFunction->ValidHImage(m_HVarMeanImage[i]) && THEAPP.m_pGFunction->ValidHImage(m_HVarStdevImage[i]))
			{
				CopyImage(m_HVarMeanImage[i], &((*ppTRegion)->m_HVarMeanImage[i]));
				CopyImage(m_HVarStdevImage[i], &((*ppTRegion)->m_HVarStdevImage[i]));
			}
		}
	}
}

GTRegion& GTRegion::operator=(GTRegion& data)
{
	if (this == &data) return *this;

	miTeachImageIndex = data.miTeachImageIndex;
	miInspectionType = data.miInspectionType;
	miFaiRoiIndex = data.miFaiRoiIndex;
	m_bRegionROI = data.m_bRegionROI;
	mbAiRetrainFlag = data.mbAiRetrainFlag;
	miPositionID = data.miPositionID;
	miPointID = data.miPointID;
	m_HTeachPolygonRgn = data.m_HTeachPolygonRgn;

	int i, j;

	for (j = 0; j < MAX_INSPECTION_TAB; j++)
		m_AlgorithmParam[j] = data.m_AlgorithmParam[j];

	mbVisible = data.mbVisible;
	mbSelectable = data.mbSelectable;
	mbSizable = data.mbSizable;
	mbMovable = data.mbMovable;
	mbSelect = data.mbSelect;
	mbLastSelected = data.mbLastSelected;
	mbShape = data.mbShape;

	miInspectionROIID = data.miInspectionROIID;
	miLocalAlignID = data.miLocalAlignID;
	miDontCareID = data.miDontCareID;
	miGenerateID = data.miGenerateID;
	miMaskID = data.miMaskID;
	miColorInfoID = data.miColorInfoID;
	miLineStyle = data.miLineStyle;
	miLineThickness = data.miLineThickness;
	mLineColor = data.mLineColor;
	mBackColor = data.mBackColor;
	miBkMode = data.miBkMode;
	miRegionStyle = data.miRegionStyle;
	mRegionColor = data.mRegionColor;

	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		m_iVarModelImageSizeX[i] = data.m_iVarModelImageSizeX[i];
		m_iVarModelImageSizeY[i] = data.m_iVarModelImageSizeY[i];
	}

	maPoints[0] = data.maPoints[0];
	maPoints[1] = data.maPoints[1];

	m_HROIAlignModelID = data.m_HROIAlignModelID;
	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		m_HVarModelID[i] = data.m_HVarModelID[i];

		m_iVarNoTrainSample[i] = data.m_iVarNoTrainSample[i];

		if (THEAPP.m_pGFunction->ValidHImage(data.m_HVarMeanImage[i]) && THEAPP.m_pGFunction->ValidHImage(data.m_HVarStdevImage[i]))
		{
			CopyImage(data.m_HVarMeanImage[i], &(m_HVarMeanImage[i]));
			CopyImage(data.m_HVarStdevImage[i], &(m_HVarStdevImage[i]));
		}
	}

	m_HPartModelID = data.m_HPartModelID;

	ClearAllChildTRegion();

	for (int i = 0; i < data.GetChildTRegionCount(); i++) {
		GTRegion *pTRegion = new GTRegion;
		GTRegion *pTRegionDup = data.GetChildTRegion(i);
		*pTRegion = *pTRegionDup;
		AddChildTRegion(pTRegion);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////

void GTRegion::GetLTPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = min(maPoints[0].x, maPoints[1].x);
	double y = min(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (dIPoint.x < 0) ? 0 : (long)(dIPoint.x + 0.5);
	pIPoint->y = (dIPoint.y < 0) ? 0 : (long)(dIPoint.y + 0.5);
}

void GTRegion::GetRBPoint(POINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth, int iImageHeight)
{
	double x = max(maPoints[0].x, maPoints[1].x);
	double y = max(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	if (iImageWidth == -1) iImageWidth = (long)dIPoint.x + 0.5;
	if (iImageHeight == -1) iImageHeight = (long)dIPoint.y + 0.5;

	pIPoint->x = (dIPoint.x > iImageWidth) ? iImageWidth - 1 : (long)(dIPoint.x + 0.5);
	pIPoint->y = (dIPoint.y > iImageHeight) ? iImageHeight - 1 : (long)(dIPoint.y + 0.5);
}

void GTRegion::GetWHPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = fabs(maPoints[1].x - maPoints[0].x);
	double y = fabs(maPoints[1].y - maPoints[0].y);

	DPOINT dWHPoint;
	pCalDataService->RPtoIP(x, y, &dWHPoint.x, &dWHPoint.y);

	pIPoint->x = (long)(dWHPoint.x + 0.5);
	pIPoint->y = (long)(dWHPoint.y + 0.5);
}

void GTRegion::GetCenterPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = (maPoints[0].x + maPoints[1].x) / 2.0;
	double y = (maPoints[0].y + maPoints[1].y) / 2.0;

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (long)(dIPoint.x + 0.5);
	pIPoint->y = (long)(dIPoint.y + 0.5);
}

void GTRegion::SetLTPoint(POINT Point, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP((double)Point.x, (double)Point.y, &dRPoint.x, &dRPoint.y);

	maPoints[0].x = dRPoint.x;
	maPoints[0].y = dRPoint.y;
}

void GTRegion::SetRBPoint(POINT Point, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP((double)Point.x, (double)Point.y, &dRPoint.x, &dRPoint.y);

	maPoints[1].x = dRPoint.x;
	maPoints[1].y = dRPoint.y;
}

void GTRegion::GetLTPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = min(maPoints[0].x, maPoints[1].x);
	double y = min(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (dIPoint.x < 0) ? 0 : dIPoint.x;
	pIPoint->y = (dIPoint.y < 0) ? 0 : dIPoint.y;
}

void GTRegion::GetRBPointD(DPOINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth, int iImageHeight)
{
	double x = max(maPoints[0].x, maPoints[1].x);
	double y = max(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	if (iImageWidth == -1) iImageWidth = (long)dIPoint.x + 0.5;
	if (iImageHeight == -1) iImageHeight = (long)dIPoint.y + 0.5;

	pIPoint->x = (dIPoint.x > iImageWidth) ? (double)(iImageWidth - 1) : dIPoint.x;
	pIPoint->y = (dIPoint.y > iImageHeight) ? (double)(iImageHeight - 1) : dIPoint.y;
}

void GTRegion::GetWHPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = fabs(maPoints[1].x - maPoints[0].x);
	double y = fabs(maPoints[1].y - maPoints[0].y);

	DPOINT dWHPoint;
	pCalDataService->RPtoIP(x, y, &dWHPoint.x, &dWHPoint.y);

	pIPoint->x = dWHPoint.x;
	pIPoint->y = dWHPoint.y;
}

void GTRegion::GetCenterPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = (maPoints[0].x + maPoints[1].x) / 2.0;
	double y = (maPoints[0].y + maPoints[1].y) / 2.0;

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = dIPoint.x;
	pIPoint->y = dIPoint.y;
}

void GTRegion::SetLTPointD(DPOINT DPoint, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP(DPoint.x, DPoint.y, &dRPoint.x, &dRPoint.y);

	maPoints[0].x = dRPoint.x;
	maPoints[0].y = dRPoint.y;
}

void GTRegion::SetRBPointD(DPOINT DPoint, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP(DPoint.x, DPoint.y, &dRPoint.x, &dRPoint.y);

	maPoints[1].x = dRPoint.x;
	maPoints[1].y = dRPoint.y;
}

void GTRegion::GetLTPointM(DPOINT *pMPoint)
{
	pMPoint->x = maPoints[0].x;
	pMPoint->y = maPoints[0].y;
}

void GTRegion::GetRBPointM(DPOINT *pMPoint)
{
	pMPoint->x = maPoints[1].x;
	pMPoint->y = maPoints[1].y;
}

void GTRegion::GetWHPointM(DPOINT *pWHPoint)
{
	pWHPoint->x = fabs(maPoints[1].x - maPoints[0].x);
	pWHPoint->y = fabs(maPoints[1].y - maPoints[0].y);
}

void GTRegion::GetCenterPointM(DPOINT *pMCPoint)
{
	pMCPoint->x = (maPoints[0].x + maPoints[1].x) / 2.0;
	pMCPoint->y = (maPoints[0].y + maPoints[1].y) / 2.0;
}

void GTRegion::SetLTPointM(DPOINT MPoint)
{
	maPoints[0].x = MPoint.x;
	maPoints[0].y = MPoint.y;
}

void GTRegion::SetRBPointM(DPOINT MPoint)
{
	maPoints[1].x = MPoint.x;
	maPoints[1].y = MPoint.y;
}

void GTRegion::SetWHPointM(DPOINT WHPoint)
{
	maPoints[1].x = maPoints[0].x + WHPoint.x;
	maPoints[1].y = maPoints[0].y + WHPoint.y;
}

void GTRegion::SetCenterPointM(DPOINT CenterPoint)
{
	DPOINT dHalfWHPoint;
	dHalfWHPoint.x = fabs(maPoints[1].x - maPoints[0].x) * 0.5;
	dHalfWHPoint.y = fabs(maPoints[1].y - maPoints[0].y) * 0.5;

	maPoints[0].x = CenterPoint.x - dHalfWHPoint.x;
	maPoints[0].y = CenterPoint.y - dHalfWHPoint.y;

	maPoints[1].x = CenterPoint.x + dHalfWHPoint.x;
	maPoints[1].y = CenterPoint.y + dHalfWHPoint.y;
}

void GTRegion::ArrangePoints()
{
	double dTemp;

	if (maPoints[0].x > maPoints[1].x) dTemp = maPoints[0].x, maPoints[0].x = maPoints[1].x, maPoints[1].x = dTemp;
	if (maPoints[0].y > maPoints[1].y) dTemp = maPoints[0].y, maPoints[0].y = maPoints[1].y, maPoints[1].y = dTemp;
}

int GTRegion::GetChildTRegionCount()
{
	return mpChildTRegionList->miCount;
}

void GTRegion::AddChildTRegion(GTRegion *pTRegion)
{
	pTRegion->mpParent = this;
	mpChildTRegionList->Add(pTRegion);
}

void GTRegion::InsertChildTRegion(int iIndex, GTRegion *pTRegion)
{
	pTRegion->mpParent = this;
	mpChildTRegionList->Insert(iIndex, pTRegion);
}

void GTRegion::DeleteChildTRegion(int iIndex)
{
	mpChildTRegionList->Delete(iIndex);
}

void GTRegion::DeleteChildTRegionPtr(GTRegion *pTRegion, BOOL bChildLink)
{
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		if (GetChildTRegion(i) == pTRegion)
		{
			mpChildTRegionList->Delete(i);
			return;
		}

		if (bChildLink)
			GetChildTRegion(i)->DeleteChildTRegionPtr(pTRegion, bChildLink);
	}
}

void GTRegion::ClearAllChildTRegion()
{
	mpChildTRegionList->ClearAll();
}

void GTRegion::DeleteAllSelectChildTRegion()
{
	PList<GTRegion> *pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);
	int i;
	for (i = 0; i < GetChildTRegionCount(); i++) {
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->DeleteAllSelectChildTRegion();
		if (pTRegion->GetSelect()) pDeleteTRegionList->Add(pTRegion);
	}

	for (i = 0; i < pDeleteTRegionList->miCount; i++) {
		GTRegion *pTRegion = pDeleteTRegionList->Get(i);
		DeleteChildTRegionPtr(pTRegion, TRUE);
	}

	delete pDeleteTRegionList;
}

void GTRegion::SwapChildTRegion(int a, int b)
{
	mpChildTRegionList->Swap(a, b);
}

GTRegion *GTRegion::GetChildTRegion(int iIndex)
{
	if (iIndex >= mpChildTRegionList->miCount) return NULL;
	return mpChildTRegionList->Get(iIndex);
}

BOOL GTRegion::RgnInRegion(GTRegion *pTRegion)
{
	ArrangePoints();
	pTRegion->ArrangePoints();

	if (maPoints[0].x<pTRegion->maPoints[0].x && maPoints[0].y<pTRegion->maPoints[0].y &&
		maPoints[1].x>pTRegion->maPoints[1].x && maPoints[1].y>pTRegion->maPoints[1].y)
		return TRUE;
	else
		return FALSE;

}

BOOL GTRegion::PtInCPoint(int iVX, int iVY, POINT VPoint)
{
	// GTR_CPHL => Control Point Half Length
	if (VPoint.x >= iVX - GTR_CPHL && VPoint.x <= iVX + GTR_CPHL &&
		VPoint.y >= iVY - GTR_CPHL && VPoint.y <= iVY + GTR_CPHL)
		return TRUE;
	return FALSE;
}

BOOL GTRegion::PtInSegment(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT LTPoint, RBPoint;

	GetLTPoint(&LTPoint, pCalDataService);
	GetRBPoint(&RBPoint, pCalDataService);

	POINT aPoints[2];

	aPoints[0].x = min(LTPoint.x, RBPoint.x);
	aPoints[0].y = min(LTPoint.y, RBPoint.y);
	aPoints[1].x = max(LTPoint.x, RBPoint.x);
	aPoints[1].y = max(LTPoint.y, RBPoint.y);

	pVManager->IPtoVP(aPoints, 2);

	// top
	if (VPoint.x >= aPoints[0].x && VPoint.x <= aPoints[1].x && VPoint.y >= aPoints[0].y - GTR_CPHL && VPoint.y <= aPoints[0].y + GTR_CPHL) return TRUE;
	// bottom
	if (VPoint.x >= aPoints[0].x && VPoint.x <= aPoints[1].x && VPoint.y >= aPoints[1].y - GTR_CPHL && VPoint.y <= aPoints[1].y + GTR_CPHL) return TRUE;
	// left
	if (VPoint.y >= aPoints[0].y && VPoint.y <= aPoints[1].y && VPoint.x >= aPoints[0].x - GTR_CPHL && VPoint.x <= aPoints[0].x + GTR_CPHL) return TRUE;
	// right
	if (VPoint.y >= aPoints[0].y && VPoint.y <= aPoints[1].y && VPoint.x >= aPoints[1].x - GTR_CPHL && VPoint.x <= aPoints[1].x + GTR_CPHL) return TRUE;

	return FALSE;
}

BOOL GTRegion::PtInRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT LTPoint, RBPoint;

	GetLTPoint(&LTPoint, pCalDataService);
	GetRBPoint(&RBPoint, pCalDataService);

	POINT aPoints[2];

	aPoints[0].x = min(LTPoint.x, RBPoint.x);
	aPoints[0].y = min(LTPoint.y, RBPoint.y);
	aPoints[1].x = max(LTPoint.x, RBPoint.x);
	aPoints[1].y = max(LTPoint.y, RBPoint.y);

	RECT Rect = { min(aPoints[0].x, aPoints[1].x), min(aPoints[0].y, aPoints[1].y), max(aPoints[0].x, aPoints[1].x), max(aPoints[0].y, aPoints[1].y) };

	pVManager->IPtoVP((POINT*)&Rect, 2);

	return ::PtInRect(&Rect, VPoint);
}

BOOL GTRegion::GetVisible()
{
	return mbVisible;
}

BOOL GTRegion::GetSelectable()
{
	return mbSelectable;
}

BOOL GTRegion::GetMovable()
{
	return mbMovable;
}

BOOL GTRegion::GetSizable()
{
	return mbSizable;
}

BOOL GTRegion::GetSelect()
{
	return mbSelect;
}

BOOL GTRegion::GetShape()
{
	return mbShape;
}

void GTRegion::SetLineColor(COLORREF lineColor)
{
	mLineColor = lineColor;
}

void GTRegion::SetVisible(BOOL bVisible, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetVisible(bVisible, bChildLink);
		}
	}
	mbVisible = bVisible;
}

void GTRegion::SetSelectable(BOOL bSelectable, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetSelectable(bSelectable, bChildLink);
		}
	}
	mbSelectable = bSelectable;
}

void GTRegion::SetMovable(BOOL bMovable, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetMovable(bMovable, bChildLink);
		}
	}
	mbMovable = bMovable;
}

void GTRegion::SetSizable(BOOL bSizable, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetSizable(bSizable, bChildLink);
		}
	}
	mbSizable = bSizable;
}

void GTRegion::SetSelect(BOOL bSelect, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetSelect(bSelect, bChildLink);
		}
	}
	mbSelect = bSelect;
	mbLastSelected = bSelect;
}

void GTRegion::SetShape(BOOL bShape, BOOL bChildLink)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetShape(bShape, bChildLink);
		}
	}
	mbShape = bShape;
}

void GTRegion::ClearAllSelect()
{
	for (int i = 0; i < GetChildTRegionCount(); i++) {
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->ClearAllSelect();
	}
	mbSelect = FALSE;
}

void GTRegion::ClearAllLastSelected()
{
	for (int i = 0; i < GetChildTRegionCount(); i++) {
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->ClearAllLastSelected();
	}
	mbLastSelected = FALSE;
}

GTRegion *GTRegion::GetTopTRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex)
{
	if (miTeachImageIndex >= 0 && iCurTeachingTabIndex != miTeachImageIndex)
		return 0;

	int iRet = GetCPointIndex(VPoint, pVManager, pCalDataService);
	if (iRet != GTR_CP_NONE)
		return this;

	GTRegion *pMinAreaRegionClick = NULL;
	long lArea, lMinArea;

	lMinArea = 10000000000000;

	for (int i = GetChildTRegionCount() - 1; i >= 0; i--)					// Used for Inspection ROI
	{
		GTRegion *pTRegion = GetChildTRegion(i);

		if (pTRegion == NULL)
			continue;

		if (pTRegion->miTeachImageIndex != iCurTeachingTabIndex)
			continue;

		GTRegion *pTopTRegion = pTRegion->GetTopTRegionArea(VPoint, pVManager, pCalDataService, iCurTeachingTabIndex, &lArea);

		if (pTopTRegion)
		{
			if (lArea > 0)	// Region Click
			{
				if (lArea < lMinArea)
				{
					lMinArea = lArea;
					pMinAreaRegionClick = pTopTRegion;
				}
			}
			else
				return pTopTRegion;
		}
	}

	if (pMinAreaRegionClick != NULL)
		return pMinAreaRegionClick;

	return 0;
}

GTRegion *GTRegion::GetTopTRegionArea(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex, long *plArea)
{
	*plArea = 0;

	if (miTeachImageIndex >= 0 && iCurTeachingTabIndex != miTeachImageIndex)
		return 0;

	int iRet = GetCPointIndex(VPoint, pVManager, pCalDataService);
	if (iRet != GTR_CP_NONE)
	{
		if (iRet == GTR_CP_REGION)
		{
			HObject HROIRgn;

			if (m_bRegionROI)
			{
				HROIRgn = m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				GetLTPoint(&LTPoint, pCalDataService);
				GetRBPoint(&RBPoint, pCalDataService);

				if (mbShape)
					GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
				else
					GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			POINT IPoint = VPoint;
			pVManager->VPtoIP(&IPoint, 1);

			HTuple IsInside;
			TestRegionPoint(HROIRgn, (HTuple)IPoint.y, (HTuple)IPoint.x, &IsInside);

			if (IsInside != 1)
				return 0;

			HTuple HArea, HCenterX, HCenterY;
			Hlong lArea;
			double dCenterX, dCenterY;

			if (THEAPP.m_pGFunction->ValidHRegion(HROIRgn))
			{
				AreaCenter(HROIRgn, &HArea, &HCenterX, &HCenterY);
				lArea = HArea.L();
				dCenterX = HCenterX.D();
				dCenterY = HCenterY.D();

				*plArea = lArea;
			}
		}

		return this;
	}

	return 0;
}

int GTRegion::GetCPointIndex(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT aPoints[2];

	if (mbSelectable)
	{
		POINT LTPoint, RBPoint;

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		pVManager->IPtoVP(aPoints, 2);

		if (PtInCPoint(aPoints[0].x, aPoints[0].y, VPoint))														return GTR_CP_LT;
		else if (PtInCPoint(aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y, VPoint))				return GTR_CP_T;
		else if (PtInCPoint(aPoints[1].x, aPoints[0].y, VPoint))													return GTR_CP_RT;
		else if (PtInCPoint(aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2, VPoint))				return GTR_CP_R;
		else if (PtInCPoint(aPoints[1].x, aPoints[1].y, VPoint))													return GTR_CP_RB;
		else if (PtInCPoint(aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y, VPoint))				return GTR_CP_B;
		else if (PtInCPoint(aPoints[0].x, aPoints[1].y, VPoint))													return GTR_CP_LB;
		else if (PtInCPoint(aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2, VPoint))				return GTR_CP_L;
		else if (PtInSegment(VPoint, pVManager, pCalDataService))	return GTR_CP_SEGMENT;
		else if (PtInRegion(VPoint, pVManager, pCalDataService))	return GTR_CP_REGION;
	}
	return GTR_CP_NONE;
}

void GTRegion::SetCPoint(int iCPointIndex, POINT *pMIPoints, CCalDataService *pCalDataService)
{
	double dAvgCalData;
	dAvgCalData = pCalDataService->GetNominalPixelSize();

	DPOINT DiffPoint;
	DiffPoint.x = (pMIPoints[1].x - pMIPoints[0].x) * dAvgCalData;
	DiffPoint.y = (pMIPoints[1].y - pMIPoints[0].y) * dAvgCalData;

	switch (iCPointIndex)
	{
	case GTR_CP_SEGMENT:
	case GTR_CP_REGION:
		if (mbMovable) {
			maPoints[0].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
			maPoints[1].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_LT:
		if (mbSizable) {
			maPoints[0].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_T:
		if (mbSizable) {
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_RT:
		if (mbSizable) {
			maPoints[1].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_R:
		if (mbSizable) {
			maPoints[1].x += DiffPoint.x;
		}
		break;
	case GTR_CP_RB:
		if (mbSizable) {
			maPoints[1].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_B:
		if (mbSizable) {
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_LB:
		if (mbSizable) {
			maPoints[0].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_L:
		if (mbSizable) {
			maPoints[0].x += DiffPoint.x;
		}
		break;
	}
}

void GTRegion::Scale(double dScale, BOOL bChildLink, CCalDataService *pCalDataService)
{
	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->Scale(dScale, bChildLink, pCalDataService);
		}
	}

	HTuple HomMat2DIdentity, HomMat2DScale;

	HomMat2dIdentity(&HomMat2DIdentity);
	HomMat2dScale(HomMat2DIdentity, HTuple(dScale), HTuple(dScale), 0, 0, &HomMat2DScale);

	if (m_bRegionROI)
	{
		AffineTransRegion(m_HTeachPolygonRgn, &m_HTeachPolygonRgn, HomMat2DScale, "nearest_neighbor");
	}
}

void GTRegion::MovePixel(int iXMove, int iYMove, BOOL bChildLink, CCalDataService *pCalDataService)
{
	double dAvgCalData;
	dAvgCalData = pCalDataService->GetNominalPixelSize();

	DPOINT dMovePoint;
	dMovePoint.x = (double)iXMove * dAvgCalData;
	dMovePoint.y = (double)iYMove * dAvgCalData;

	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->MovePixel(iXMove, iYMove, bChildLink, pCalDataService);
		}
	}

	if (m_bRegionROI)
	{
		MoveRegion(m_HTeachPolygonRgn, &m_HTeachPolygonRgn, iYMove, iXMove);
	}

	maPoints[0].x += dMovePoint.x;
	maPoints[0].y += dMovePoint.y;
	maPoints[1].x += dMovePoint.x;
	maPoints[1].y += dMovePoint.y;
}

void GTRegion::MoveUm(double dXMove, double dYMove, BOOL bChildLink, CCalDataService *pCalDataService)
{
	double dAvgCalData;
	dAvgCalData = pCalDataService->GetNominalPixelSize();

	POINT iMovePointPxl;
	iMovePointPxl.x = (int)((double)dXMove / dAvgCalData + 0.5);
	iMovePointPxl.y = (int)((double)dYMove / dAvgCalData + 0.5);

	if (bChildLink) {
		for (int i = 0; i < GetChildTRegionCount(); i++) {
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->MoveUm(dXMove, dYMove, bChildLink, pCalDataService);
		}
	}

	if (m_bRegionROI)
	{
		MoveRegion(m_HTeachPolygonRgn, &m_HTeachPolygonRgn, iMovePointPxl.y, iMovePointPxl.x);
	}

	maPoints[0].x += dXMove;
	maPoints[0].y += dYMove;
	maPoints[1].x += dXMove;
	maPoints[1].y += dYMove;
}

void GTRegion::DrawControlPoint(HDC hDestDC, int iVX, int iVY)
{
	Rectangle(hDestDC, iVX - GTR_CPHL, iVY - GTR_CPHL, iVX + GTR_CPHL, iVY + GTR_CPHL);
}

void GTRegion::DrawControlPoint(HTuple hDestDC, int iVX, int iVY)
{
	SetDraw(hDestDC, "fill");
	SetRgb(hDestDC, 0, 0, 0);
	DispRectangle1(hDestDC, iVY - (GTR_CPHL), iVX - (GTR_CPHL), iVY + (GTR_CPHL), iVX + (GTR_CPHL));
	SetDraw(hDestDC, "margin");
	SetRgb(hDestDC, 0, 255, 0);
	DispRectangle1(hDestDC, iVY - (GTR_CPHL), iVX - (GTR_CPHL), iVY + (GTR_CPHL), iVX + (GTR_CPHL));
}

void GTRegion::Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex)
{
	if (miTeachImageIndex >= 0 && iCurTeachingTabIndex != miTeachImageIndex)
		return;

	if (m_bRegionROI)
	{
		if (mbVisible)
		{
			if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
			{
				if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) || (miInspectionType >= INSPECTION_TYPE_AF_ALIGN && miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) || miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE || (miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
				{
					HTuple HArea, HCenterX, HCenterY;
					AreaCenter(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					double dCenterX, dCenterY;
					dCenterX = HCenterX.D();
					dCenterY = HCenterY.D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName = GetInspectionTypeName();

					if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST && mbAiRetrainFlag)
						sInspectionTypeName += "(AI)";

					if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
					{
						BOOL bUseAutoTrain = FALSE;
						for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
						{
							if (m_AlgorithmParam[ii].m_bUseImageCompare && m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain)
							{
								bUseAutoTrain = TRUE;
								break;
							}
						}

						if (bUseAutoTrain)
							sInspectionTypeName += "(TR)";
					}

					SetBkMode(hDestDC, TRANSPARENT);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(255, 0, 0));

					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));

					if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
					{
						HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
						SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
						dCenterX = HlLTPointX[0].L();
						dCenterY = HlLTPointY[0].L();

						CenterVPoint.x = (long)dCenterX;
						CenterVPoint.y = (long)dCenterY;

						pVManager->IPtoVP(&CenterVPoint, 1);
						CenterVPoint.x += 10;
						CenterVPoint.y += 10;

						sInspectionTypeName.Format("%d", miInspectionROIID);

						SetBkMode(hDestDC, TRANSPARENT);

						if (mbSelect)
							SetTextColor(hDestDC, RGB(255, 255, 0));
						else
							SetTextColor(hDestDC, RGB(255, 0, 0));

						TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
					}
				}
				else if (miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
				{
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					double dCenterX, dCenterY;
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miLocalAlignID);

					SetBkMode(hDestDC, TRANSPARENT);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(255, 0, 255));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (miInspectionType == INSPECTION_TYPE_DONTCARE)
				{
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					double dCenterX, dCenterY;
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miDontCareID);

					SetBkMode(hDestDC, TRANSPARENT);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(128, 0, 255));

					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (miInspectionType == INSPECTION_TYPE_GENERATE)
				{
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					double dCenterX, dCenterY;
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miGenerateID);

					SetBkMode(hDestDC, TRANSPARENT);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(255, 0, 128));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (miInspectionType == INSPECTION_TYPE_MASK)
				{
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					double dCenterX, dCenterY;
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miMaskID);

					SetBkMode(hDestDC, TRANSPARENT);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(0, 0, 255));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (miInspectionType == INSPECTION_TYPE_COLOR)
				{
					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					double dCenterX, dCenterY;
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miColorInfoID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 0, 255));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
			}
		}

		return;
	}

	HPEN hOldPen, hPen;
	HBRUSH hOldBrush, hBrush;
	POINT aPoints[2];

	POINT LTPoint, RBPoint;

	if (mbVisible && !(iDrawStyle == GTR_DS_ACTIVE && mbSelect == FALSE))
	{
		if (iDrawStyle == GTR_DS_NORMAL)
		{
			SetROP2(hDestDC, R2_COPYPEN);
			SetBkMode(hDestDC, miBkMode);
			SetBkColor(hDestDC, mBackColor);

			if (mbLastSelected)
			{
				if ((miInspectionType > INSPECTION_TYPE_UNDEFINED && miInspectionType < INSPECTION_TYPE_DELETE) ||
					miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE ||
					(miInspectionType >= INSPECTION_TYPE_AF_ALIGN && miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) ||
					(miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
					hPen = CreatePen(miLineStyle, miLineThickness, RGB(255, 0, 0));
				else
					hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);
			}
			else
				hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);

			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			if (miRegionStyle == GTR_RS_NULL || bDrawControlPoint) hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			else hBrush = CreateSolidBrush(mRegionColor);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_ACTIVE)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_SELECTPART)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		pVManager->IPtoVP(aPoints, 2);

		int iRectWidth, iRectHeight;

		if (mbShape)
		{	// Circle
			iRectWidth = aPoints[1].x - aPoints[0].x + 1;
			iRectHeight = aPoints[1].y - aPoints[0].y + 1;
			if (iRectWidth >= iRectHeight) {
				aPoints[1].x = aPoints[0].x + iRectWidth;
				aPoints[1].y = aPoints[0].y + iRectWidth;
			}
			else {
				aPoints[1].x = aPoints[0].x + iRectHeight;
				aPoints[1].y = aPoints[0].y + iRectHeight;
			}

			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}
		else
		{
			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint) {
			if (mbSelect) {
				SelectObject(hDestDC, hOldPen);
				DeleteObject(hPen);
				SelectObject(hDestDC, hOldBrush);
				DeleteObject(hBrush);

				hPen = CreatePen(PS_SOLID, 1, mLineColor);
				hOldPen = (HPEN)SelectObject(hDestDC, hPen);
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
			}
		}

		SelectObject(hDestDC, hOldPen);
		DeleteObject(hPen);
		SelectObject(hDestDC, hOldBrush);
		DeleteObject(hBrush);

		SetROP2(hDestDC, R2_COPYPEN);

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
		{
			if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) || (miInspectionType >= INSPECTION_TYPE_AF_ALIGN && miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) || miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE || (miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName = GetInspectionTypeName();

				if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST && mbAiRetrainFlag)
					sInspectionTypeName += "(AI)";

				if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
				{
					BOOL bUseAutoTrain = FALSE;
					for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
					{
						if (m_AlgorithmParam[ii].m_bUseImageCompare && m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain)
						{
							bUseAutoTrain = TRUE;
							break;
						}
					}

					if (bUseAutoTrain)
						sInspectionTypeName += "(TR)";
				}

				if (mbSelect)
					SetTextColor(hDestDC, RGB(255, 255, 0));
				else
					SetTextColor(hDestDC, RGB(255, 0, 0));

				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));

				if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
				{
					GetLTPoint(&CenterVPoint, pCalDataService);

					pVManager->IPtoVP(&CenterVPoint, 1);
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miInspectionROIID);

					if (mbSelect)
						SetTextColor(hDestDC, RGB(255, 255, 0));
					else
						SetTextColor(hDestDC, RGB(255, 0, 0));

					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
			}
			else if (miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
				CenterVPoint.x += 10;
				CenterVPoint.y += 10;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miLocalAlignID);

				if (mbSelect)
					SetTextColor(hDestDC, RGB(255, 255, 0));
				else
					SetTextColor(hDestDC, RGB(255, 0, 255));

				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (miInspectionType == INSPECTION_TYPE_DONTCARE)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
				CenterVPoint.x += 10;
				CenterVPoint.y += 10;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miDontCareID);

				if (mbSelect)
					SetTextColor(hDestDC, RGB(255, 255, 0));
				else
					SetTextColor(hDestDC, RGB(255, 0, 128));

				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (miInspectionType == INSPECTION_TYPE_GENERATE)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
				CenterVPoint.x += 10;
				CenterVPoint.y += 10;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miGenerateID);

				if (mbSelect)
					SetTextColor(hDestDC, RGB(255, 255, 0));
				else
					SetTextColor(hDestDC, RGB(128, 0, 255));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (miInspectionType == INSPECTION_TYPE_MASK)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
				CenterVPoint.x += 10;
				CenterVPoint.y += 10;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miMaskID);

				if (mbSelect)
					SetTextColor(hDestDC, RGB(255, 255, 0));
				else
					SetTextColor(hDestDC, RGB(0, 0, 255));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (miInspectionType == INSPECTION_TYPE_COLOR)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
				CenterVPoint.x += 10;
				CenterVPoint.y += 10;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miColorInfoID);

				SetTextColor(hDestDC, RGB(255, 0, 255));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
		}
	}

	// Child Link
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pChildTRegion = GetChildTRegion(i);
		if (pChildTRegion == NULL)
			continue;

		pChildTRegion->Draw(hDestDC, pVManager, iDrawStyle, FALSE, pCalDataService, iCurTeachingTabIndex);
	}
}

void GTRegion::Draw(HTuple hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex)
{
	if (miTeachImageIndex >= 0 && iCurTeachingTabIndex != miTeachImageIndex)
		return;

	HalconCpp::SetFont(hDestDC, "Arial-Bold-16");

	if (m_bRegionROI)
	{
		if (mbVisible)
		{
			if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
			{
				if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) 
				|| (miInspectionType >= INSPECTION_TYPE_AF_ALIGN && miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) 
				|| (miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE) 
				|| (miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
				{
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					AreaCenter(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					dCenterX = HCenterX.D();
					dCenterY = HCenterY.D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					CString sInspectionTypeName = GetInspectionTypeName();

					if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) && mbAiRetrainFlag)
						sInspectionTypeName += "(AI)";

					if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
					{
						BOOL bUseAutoTrain = FALSE;
						for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
						{
							if (m_AlgorithmParam[ii].m_bUseImageCompare && m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain)
							{
								bUseAutoTrain = TRUE;
								break;
							}
						}

						if (bUseAutoTrain)
							sInspectionTypeName += "(TR)";
					}

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 255, 0, 0);

					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);

					// TODO: INSPECTION_TYPE_MEASURE_FIRST 와 INSPECTION_TYPE_FIRST 의 차이는?
					if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
					{
						HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
						SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
						dCenterX = HlLTPointX[0].L();
						dCenterY = HlLTPointY[0].L();

						CenterVPoint.x = (long)dCenterX;
						CenterVPoint.y = (long)dCenterY;
						CenterVPoint.x += 10;
						CenterVPoint.y += 10;

						sInspectionTypeName.Format("%d", miInspectionROIID);

						if (mbSelect)
							SetRgb(hDestDC, 255, 255, 0);
						else
							SetRgb(hDestDC, 255, 0, 0);

						SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
						WriteString(hDestDC, (HTuple)sInspectionTypeName);
					}
				}
				else if (miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
				{
					double dCenterX, dCenterY;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miLocalAlignID);

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 255, 0, 255);

					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
				else if (miInspectionType == INSPECTION_TYPE_DONTCARE)
				{
					double dCenterX, dCenterY;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miDontCareID);

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 128, 0, 255);

					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
				else if (miInspectionType == INSPECTION_TYPE_GENERATE)
				{
					double dCenterX, dCenterY;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miGenerateID);

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 255, 0, 128);
					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
				else if (miInspectionType == INSPECTION_TYPE_MASK)
				{
					double dCenterX, dCenterY;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miMaskID);

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 0, 0, 255);
					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
				else if (miInspectionType == INSPECTION_TYPE_COLOR)
				{
					double dCenterX, dCenterY;

					HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
					SmallestRectangle1(m_HTeachPolygonRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
					dCenterX = HlLTPointX[0].L();
					dCenterY = HlLTPointY[0].L();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;
					CenterVPoint.x += 10;
					CenterVPoint.y += 10;

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", miColorInfoID);

					SetRgb(hDestDC, 255, 0, 255);
					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
			}
		}

		return;
	}

	HPEN hOldPen, hPen;
	HBRUSH hOldBrush, hBrush;
	POINT aPoints[2];

	POINT LTPoint, RBPoint;

	if (mbVisible && !(iDrawStyle == GTR_DS_ACTIVE && mbSelect == FALSE))
	{
		if (miLineStyle == PS_SOLID)
			SetLineStyle(hDestDC, HTuple());
		else if (miLineStyle == PS_DOT)
			SetLineStyle(hDestDC, (HTuple(10).Append(10)));

		if (miLineThickness >= 1)
			SetLineWidth(hDestDC, HTuple(miLineThickness));
		else
			SetLineWidth(hDestDC, 1);

		SetDraw(hDestDC, "margin");

		if (iDrawStyle == GTR_DS_NORMAL)
		{
			if (mbLastSelected)
			{
				if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) || miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE || (miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
					SetRgb(hDestDC, 255, 0, 0);
				else
					SetRgb(hDestDC, GetRValue(mLineColor), GetGValue(mLineColor), GetBValue(mLineColor));
			}
			else
				SetRgb(hDestDC, GetRValue(mLineColor), GetGValue(mLineColor), GetBValue(mLineColor));
		}
		else if (iDrawStyle == GTR_DS_ACTIVE)
		{
			SetRgb(hDestDC, 0, 0, 0);
		}
		else if (iDrawStyle == GTR_DS_SELECTPART)
		{
			SetRgb(hDestDC, GetRValue(mLineColor), GetBValue(mLineColor), GetBValue(mLineColor));
		}

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		int iRectWidth, iRectHeight;

		if (mbShape)
		{	// Circle
			iRectWidth = aPoints[1].x - aPoints[0].x + 1;
			iRectHeight = aPoints[1].y - aPoints[0].y + 1;
			if (iRectWidth >= iRectHeight) {
				aPoints[1].x = aPoints[0].x + iRectWidth;
				aPoints[1].y = aPoints[0].y + iRectWidth;
			}
			else {
				aPoints[1].x = aPoints[0].x + iRectHeight;
				aPoints[1].y = aPoints[0].y + iRectHeight;
			}

			if (iRectWidth >= iRectHeight)
				DispCircle(hDestDC, (aPoints[0].y + aPoints[1].y) / 2, (aPoints[0].x + aPoints[1].x) / 2, iRectWidth / 2);
			else
				DispCircle(hDestDC, (aPoints[0].y + aPoints[1].y) / 2, (aPoints[0].x + aPoints[1].x) / 2, iRectHeight / 2);
		}
		else
		{
			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				DispRectangle1(hDestDC, aPoints[0].y, aPoints[0].x, aPoints[1].y + 1, aPoints[1].x + 1);
			else
				DispRectangle1(hDestDC, aPoints[0].y, aPoints[0].x, aPoints[1].y, aPoints[1].x);
		}

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint) {
			if (mbSelect)
			{
				SetLineStyle(hDestDC, HTuple());
				SetLineWidth(hDestDC, 1);

				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
			}
		}

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
		{
			if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) || (miInspectionType >= INSPECTION_TYPE_AF_ALIGN && miInspectionType <= INSPECTION_TYPE_AF_WEST_LINE) || miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN_ANGLE || (miInspectionType >= INSPECTION_TYPE_MEASURE_FIRST && miInspectionType <= INSPECTION_TYPE_MEASURE_LAST))
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				CString sInspectionTypeName = GetInspectionTypeName();

				if ((miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST) && mbAiRetrainFlag)
					sInspectionTypeName += "(AI)";

				if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
				{
					BOOL bUseAutoTrain = FALSE;
					for (int ii = 0; ii < MAX_INSPECTION_TAB; ii++)
					{
						if (m_AlgorithmParam[ii].m_bUseImageCompare && m_AlgorithmParam[ii].m_bUseImageCompareAutoTrain)
						{
							bUseAutoTrain = TRUE;
							break;
						}
					}

					if (bUseAutoTrain)
						sInspectionTypeName += "(TR)";
				}

				if (mbSelect)
					SetRgb(hDestDC, 255, 255, 0);
				else
					SetRgb(hDestDC, 255, 0, 0);

				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);

				if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
				{
					GetLTPoint(&CenterVPoint, pCalDataService);
					CenterVPoint.x += 20;
					CenterVPoint.y += 20;

					sInspectionTypeName.Format("%d", miInspectionROIID);

					if (mbSelect)
						SetRgb(hDestDC, 255, 255, 0);
					else
						SetRgb(hDestDC, 255, 0, 0);

					SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
					WriteString(hDestDC, (HTuple)sInspectionTypeName);
				}
			}
			else if (miInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);
				CenterVPoint.x += 20;
				CenterVPoint.y += 20;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miLocalAlignID);

				if (mbSelect)
					SetRgb(hDestDC, 255, 255, 0);
				else
					SetRgb(hDestDC, 255, 0, 255);

				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);
			}
			else if (miInspectionType == INSPECTION_TYPE_DONTCARE)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);
				CenterVPoint.x += 20;
				CenterVPoint.y += 20;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miDontCareID);

				if (mbSelect)
					SetRgb(hDestDC, 255, 255, 0);
				else
					SetRgb(hDestDC, 128, 0, 255);

				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);
			}
			else if (miInspectionType == INSPECTION_TYPE_GENERATE)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);
				CenterVPoint.x += 20;
				CenterVPoint.y += 20;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miGenerateID);

				if (mbSelect)
					SetRgb(hDestDC, 255, 255, 0);
				else
					SetRgb(hDestDC, 255, 0, 128);
				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);
			}
			else if (miInspectionType == INSPECTION_TYPE_MASK)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);
				CenterVPoint.x += 20;
				CenterVPoint.y += 20;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miMaskID);

				if (mbSelect)
					SetRgb(hDestDC, 255, 255, 0);
				else
					SetRgb(hDestDC, 0, 0, 255);
				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);
			}
			else if (miInspectionType == INSPECTION_TYPE_COLOR)
			{
				POINT CenterVPoint;
				GetLTPoint(&CenterVPoint, pCalDataService);
				CenterVPoint.x += 20;
				CenterVPoint.y += 20;

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", miColorInfoID);

				SetRgb(hDestDC, 255, 0, 255);
				SetTposition(hDestDC, CenterVPoint.y, CenterVPoint.x);
				WriteString(hDestDC, (HTuple)sInspectionTypeName);
			}
		}
	}

	// Child Link
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pChildTRegion = GetChildTRegion(i);
		if (pChildTRegion == NULL)
			continue;

		pChildTRegion->Draw(hDestDC, pVManager, iDrawStyle, FALSE, pCalDataService, iCurTeachingTabIndex);
	}
}

void GTRegion::Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int ViewOffsetX, int ViewOffsetY)
{
	if (m_bRegionROI)
		return;

	HPEN hOldPen, hPen;
	HBRUSH hOldBrush, hBrush;
	POINT aPoints[2];

	POINT LTPoint, RBPoint;

	if (mbVisible && !(iDrawStyle == GTR_DS_ACTIVE && mbSelect == FALSE))
	{
		if (iDrawStyle == GTR_DS_NORMAL)
		{
			SetROP2(hDestDC, R2_COPYPEN);
			SetBkMode(hDestDC, miBkMode);
			SetBkColor(hDestDC, mBackColor);

			hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);

			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			if (miRegionStyle == GTR_RS_NULL || bDrawControlPoint) hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			else hBrush = CreateSolidBrush(mRegionColor);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_ACTIVE)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_SELECTPART)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		pVManager->IPtoVP(aPoints, 2);

		aPoints[0].x += ViewOffsetX;
		aPoints[0].y += ViewOffsetY;
		aPoints[1].x += ViewOffsetX;
		aPoints[1].y += ViewOffsetY;

		int iRectWidth, iRectHeight;

		if (mbShape)
		{	// Circle
			iRectWidth = aPoints[1].x - aPoints[0].x + 1;
			iRectHeight = aPoints[1].y - aPoints[0].y + 1;
			if (iRectWidth >= iRectHeight) {
				aPoints[1].x = aPoints[0].x + iRectWidth;
				aPoints[1].y = aPoints[0].y + iRectWidth;
			}
			else {
				aPoints[1].x = aPoints[0].x + iRectHeight;
				aPoints[1].y = aPoints[0].y + iRectHeight;
			}

			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}
		else
		{
			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint) {
			if (mbSelect) {
				SelectObject(hDestDC, hOldPen);
				DeleteObject(hPen);
				SelectObject(hDestDC, hOldBrush);
				DeleteObject(hBrush);

				hPen = CreatePen(PS_SOLID, 1, mLineColor);
				hOldPen = (HPEN)SelectObject(hDestDC, hPen);
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
			}
		}

		SelectObject(hDestDC, hOldPen);
		DeleteObject(hPen);
		SelectObject(hDestDC, hOldBrush);
		DeleteObject(hBrush);

		SetROP2(hDestDC, R2_COPYPEN);
	}
}

GTRegion* GTRegion::GetCurSelectRegion()
{
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		if (pTRegion->GetSelect())
		{
			return pTRegion;
		}
	}

	return NULL;
}

CString GTRegion::GetInspectionTypeName()
{
	CString sInspectionType;

	if (miInspectionType >= INSPECTION_TYPE_FIRST && miInspectionType <= INSPECTION_TYPE_LAST)
		sInspectionType = g_strInspectionTypeName[miInspectionType - 1].c_str();
	else
	{
		switch (miInspectionType)
		{
		case INSPECTION_TYPE_LOCAL_ALIGN_ANGLE:
			sInspectionType = "Align Angle";
			break;

		case INSPECTION_TYPE_AF_ALIGN:
			sInspectionType = "AF Align";
			break;

		case INSPECTION_TYPE_AF_ZIG_ROTATION_CENTER:
			sInspectionType = "AF Zig Rotation Center";
			break;

		case INSPECTION_TYPE_AF_NORTH_LINE:
			sInspectionType = "AF Horizontal Line";
			break;

		case INSPECTION_TYPE_AF_WEST_LINE:
			sInspectionType = "AF Vertical Line";
			break;
#if defined (UAVI_AKC) || defined (UAVI_BOI) || defined (UAVI_ACT) || defined (UAVI_ATW) || defined (UAVI_BOS) || defined (UAVI_RENO) || defined (UAVI_KRIOS) || defined (SINGLE_LENS) || defined (ASSY_LENS)
		case INSPECTION_TYPE_MEASURE_POINT:
		{
			const CString measurePosName = CFAIDataManager::GetInstance().GetMeasurePosName(miPositionID);
			sInspectionType.Format("%s P%d", measurePosName.GetString(), miPointID + 1);
			break;
		}
		case INSPECTION_TYPE_BLEND_POINT:
			sInspectionType.Format("%s %s", g_strAkcBlendPosName[miPositionID], g_strBlendPointName[miPointID]);
			break;
#endif
#if defined (UAVI_CHS_KS) || defined (UAVI_CHS_TV)
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S1:
			sInspectionType = "DatumA S1 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S2:
			sInspectionType = "DatumA S2 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S3:
			sInspectionType = "DatumA S3 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S4:
			sInspectionType = "DatumA S4 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1:
			sInspectionType = "DatumB Line1 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P2:
			sInspectionType = "DatumB Line1 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P1:
			sInspectionType = "DatumB Line2 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P2:
			sInspectionType = "DatumB Line2 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P1:
			sInspectionType = "DatumB Line3 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P2:
			sInspectionType = "DatumB Line3 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P1:
			sInspectionType = "DatumB Line4 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2:
			sInspectionType = "DatumB Line4 P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_A:
			sInspectionType = "VCM Shield Can Top A (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_B:
			sInspectionType = "VCM Shield Can Top B (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_C:
			sInspectionType = "VCM Shield Can Top C (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_D:
			sInspectionType = "VCM Shield Can Top D (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_E:
			sInspectionType = "VCM Shield Can Top E (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_F:
			sInspectionType = "VCM Shield Can Top F (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_G:
			sInspectionType = "VCM Shield Can Top G (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_TOP_H:
			sInspectionType = "VCM Shield Can Top H (H)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P1:
			sInspectionType = "Lens Barrel Top P1 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P2:
			sInspectionType = "Lens Barrel Top P2 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P3:
			sInspectionType = "Lens Barrel Top P3 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P4:
			sInspectionType = "Lens Barrel Top P4 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P5:
			sInspectionType = "Lens Barrel Top P5 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P6:
			sInspectionType = "Lens Barrel Top P6 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P7:
			sInspectionType = "Lens Barrel Top P7 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_BARREL_TOP_P8:
			sInspectionType = "Lens Barrel Top P8 (P, H)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P1:
			sInspectionType = "Lens Inner P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P2:
			sInspectionType = "Lens Inner P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P3:
			sInspectionType = "Lens Inner P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P4:
			sInspectionType = "Lens Inner P4 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P5:
			sInspectionType = "Lens Inner P5 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P6:
			sInspectionType = "Lens Inner P6 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P7:
			sInspectionType = "Lens Inner P7 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_LENS_INNER_P8:
			sInspectionType = "Lens Inner P8 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_A:
			sInspectionType = "FAI-7 West A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_7_WEST_B:
			sInspectionType = "FAI-7 West B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P1:
			sInspectionType = "FAI-7 East P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_7_EAST_P2:
			sInspectionType = "FAI-7 East P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P1:
			sInspectionType = "FAI-9 West P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P2:
			sInspectionType = "FAI-9 West P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P3:
			sInspectionType = "FAI-9 West P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P4:
			sInspectionType = "FAI-9 West P4 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_WEST_P5:
			sInspectionType = "FAI-9 West P5 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_A:
			sInspectionType = "FAI-9 East A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_B:
			sInspectionType = "FAI-9 East B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_9_EAST_C:
			sInspectionType = "FAI-9 East C (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_A:
			sInspectionType = "FAI-10 North A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_B:
			sInspectionType = "FAI-10 North B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_NORTH_C:
			sInspectionType = "FAI-10 North C (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P1:
			sInspectionType = "FAI-10 South P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P2:
			sInspectionType = "FAI-10 South P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_10_SOUTH_P3:
			sInspectionType = "FAI-10 South P3 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P1:
			sInspectionType = "VCM Step P1 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P2:
			sInspectionType = "VCM Step P2 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P3:
			sInspectionType = "VCM Step P3 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_VCM_SC_STEP_P4:
			sInspectionType = "VCM Step P4 (H)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P1:
			sInspectionType = "FAI-18 Neck Epoxy P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P2:
			sInspectionType = "FAI-18 Neck Epoxy P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_18_NECK_EPOXY_P3:
			sInspectionType = "FAI-18 Neck Epoxy P3 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_A:
			sInspectionType = "FAI-21 Flex A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_21_FLEX_B:
			sInspectionType = "FAI-21 Flex B (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_A:
			sInspectionType = "FAI-23 Flex A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_23_FLEX_B:
			sInspectionType = "FAI-23 Flex B (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_24_P1:
			sInspectionType = "FAI-24 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_24_P2:
			sInspectionType = "FAI-24 P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_25_A:
			sInspectionType = "FAI-25 A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_25_B:
			sInspectionType = "FAI-25 B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_25_P1:
			sInspectionType = "FAI-25 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_25_P2:
			sInspectionType = "FAI-25 P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P1:
			sInspectionType = "D/C_1 P1 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P2:
			sInspectionType = "D/C_1 P2 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P3:
			sInspectionType = "D/C_1 P3 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P4:
			sInspectionType = "D/C_1 P4 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P5:
			sInspectionType = "D/C_1 P5 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P6:
			sInspectionType = "D/C_1 P6 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P7:
			sInspectionType = "D/C_1 P7 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P8:
			sInspectionType = "D/C_1 P8 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P9:
			sInspectionType = "D/C_1 P9 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P10:
			sInspectionType = "D/C_1 P10 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P11:
			sInspectionType = "D/C_1 P11 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P12:
			sInspectionType = "D/C_1 P12 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DC_1_P13:
			sInspectionType = "D/C_1 P13 (H)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_A:
			sInspectionType = "FAI-16-1 A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_B:
			sInspectionType = "FAI-16-1 B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P1:
			sInspectionType = "FAI-16-1 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P2:
			sInspectionType = "FAI-16-1 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P3:
			sInspectionType = "FAI-16-1 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_1_P4:
			sInspectionType = "FAI-16-1 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_A:
			sInspectionType = "FAI-16-2 A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_B:
			sInspectionType = "FAI-16-2 B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P1:
			sInspectionType = "FAI-16-2 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P2:
			sInspectionType = "FAI-16-2 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P3:
			sInspectionType = "FAI-16-2 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_2_P4:
			sInspectionType = "FAI-16-2 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_A:
			sInspectionType = "FAI-16-3 A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_B:
			sInspectionType = "FAI-16-3 B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P1:
			sInspectionType = "FAI-16-3 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P2:
			sInspectionType = "FAI-16-3 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P3:
			sInspectionType = "FAI-16-3 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_16_3_P4:
			sInspectionType = "FAI-16-3 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_A:
			sInspectionType = "FAI-18 A (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_B:
			sInspectionType = "FAI-18 B (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P1:
			sInspectionType = "FAI-18 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P2:
			sInspectionType = "FAI-18 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P3:
			sInspectionType = "FAI-18 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_FAI_19_P4:
			sInspectionType = "FAI-18 P4 (P)";
			break;

#endif
#ifdef UAVI_CHS_WZ
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S1:
			sInspectionType = "DatumA S1 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S2:
			sInspectionType = "DatumA S2 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S3:
			sInspectionType = "DatumA S3 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_A_S4:
			sInspectionType = "DatumA S4 (H)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P1:
			sInspectionType = "DatumB Line1 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE1_P2:
			sInspectionType = "DatumB Line1 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P1:
			sInspectionType = "DatumB Line2 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE2_P2:
			sInspectionType = "DatumB Line2 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P1:
			sInspectionType = "DatumB Line3 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE3_P2:
			sInspectionType = "DatumB Line3 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P1:
			sInspectionType = "DatumB Line4 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_MEASURE_DATUM_B_LINE4_P2:
			sInspectionType = "DatumB Line4 P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_A:
			sInspectionType = "VCM Shield Can Top A (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_B:
			sInspectionType = "VCM Shield Can Top B (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_C:
			sInspectionType = "VCM Shield Can Top C (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_D:
			sInspectionType = "VCM Shield Can Top D (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_E:
			sInspectionType = "VCM Shield Can Top E (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_F:
			sInspectionType = "VCM Shield Can Top F (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_G:
			sInspectionType = "VCM Shield Can Top G (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_TOP_H:
			sInspectionType = "VCM Shield Can Top H (H)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P1:
			sInspectionType = "Lens Barrel Top P1 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P2:
			sInspectionType = "Lens Barrel Top P2 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P3:
			sInspectionType = "Lens Barrel Top P3 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P4:
			sInspectionType = "Lens Barrel Top P4 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P5:
			sInspectionType = "Lens Barrel Top P5 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P6:
			sInspectionType = "Lens Barrel Top P6 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P7:
			sInspectionType = "Lens Barrel Top P7 (P, H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_BARREL_TOP_P8:
			sInspectionType = "Lens Barrel Top P8 (P, H)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P1:
			sInspectionType = "Lens Inner P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P2:
			sInspectionType = "Lens Inner P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P3:
			sInspectionType = "Lens Inner P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P4:
			sInspectionType = "Lens Inner P4 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P5:
			sInspectionType = "Lens Inner P5 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P6:
			sInspectionType = "Lens Inner P6 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P7:
			sInspectionType = "Lens Inner P7 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_LENS_INNER_P8:
			sInspectionType = "Lens Inner P8 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_A:
			sInspectionType = "FAI-3 West A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_WEST_B:
			sInspectionType = "FAI-3 West B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P1:
			sInspectionType = "FAI-3 East P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_3_EAST_P2:
			sInspectionType = "FAI-3 East P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P1:
			sInspectionType = "FAI-9 West P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P2:
			sInspectionType = "FAI-9 West P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_WEST_P3:
			sInspectionType = "FAI-9 West P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_A:
			sInspectionType = "FAI-9 East A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_B:
			sInspectionType = "FAI-9 East B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_9_EAST_C:
			sInspectionType = "FAI-9 East C (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_A:
			sInspectionType = "FAI-10 North A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_B:
			sInspectionType = "FAI-10 North B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_NORTH_C:
			sInspectionType = "FAI-10 North C (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P1:
			sInspectionType = "FAI-10 South P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_10_SOUTH_P2:
			sInspectionType = "FAI-10 South P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P1:
			sInspectionType = "FAI-14 North P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_NORTH_P2:
			sInspectionType = "FAI-14 North P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P3:
			sInspectionType = "FAI-14 South P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_14_SOUTH_P4:
			sInspectionType = "FAI-14 South P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_A:
			sInspectionType = "FAI-18 Neck Epoxy A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_B:
			sInspectionType = "FAI-18 Neck Epoxy B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_18_NECK_EPOXY_C:
			sInspectionType = "FAI-18 Neck Epoxy C (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P1:
			sInspectionType = "VCM Step P1 (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P2:
			sInspectionType = "VCM Step P2 (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P3:
			sInspectionType = "VCM Step P3 (H)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_VCM_SC_STEP_P4:
			sInspectionType = "VCM Step P4 (H)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P1:
			sInspectionType = "FAI-21 Flex P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_21_FLEX_P2:
			sInspectionType = "FAI-21 Flex P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P1:
			sInspectionType = "FAI-22 Flex P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_22_FLEX_P2:
			sInspectionType = "FAI-22 Flex P2 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P1:
			sInspectionType = "FAI-24 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P2:
			sInspectionType = "FAI-24 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P3:
			sInspectionType = "FAI-24 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_24_P4:
			sInspectionType = "FAI-24 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P1:
			sInspectionType = "FAI-25 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P2:
			sInspectionType = "FAI-25 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P3:
			sInspectionType = "FAI-25 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_25_P4:
			sInspectionType = "FAI-25 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_A:
			sInspectionType = "FAI-17-1 A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_B:
			sInspectionType = "FAI-17-1 B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P1:
			sInspectionType = "FAI-17-1 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P2:
			sInspectionType = "FAI-17-1 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P3:
			sInspectionType = "FAI-17-1 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_1_P4:
			sInspectionType = "FAI-17-1 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_A:
			sInspectionType = "FAI-17-2 A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_B:
			sInspectionType = "FAI-17-2 B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P1:
			sInspectionType = "FAI-17-2 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P2:
			sInspectionType = "FAI-17-2 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P3:
			sInspectionType = "FAI-17-2 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_2_P4:
			sInspectionType = "FAI-17-2 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_A:
			sInspectionType = "FAI-17-3 A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_B:
			sInspectionType = "FAI-17-3 B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P1:
			sInspectionType = "FAI-17-3 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P2:
			sInspectionType = "FAI-17-3 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P3:
			sInspectionType = "FAI-17-3 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_17_3_P4:
			sInspectionType = "FAI-17-3 P4 (P)";
			break;

		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_A:
			sInspectionType = "FAI-19 A (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_B:
			sInspectionType = "FAI-19 B (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P1:
			sInspectionType = "FAI-19 P1 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P2:
			sInspectionType = "FAI-19 P2 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P3:
			sInspectionType = "FAI-19 P3 (P)";
			break;
		case INSPECTION_TYPE_CHS_WZ_MEASURE_FAI_19_P4:
			sInspectionType = "FAI-19 P4 (P)";
#endif
		default:
			sInspectionType = "Undefined";
			break;
		}
	}

	return sInspectionType;
}

HObject GTRegion::GetROIHRegion(CCalDataService *pCalDataService)
{
	try
	{
		HObject HROIRgn;
		GenEmptyObj(&HROIRgn);

		if (m_bRegionROI)
		{
			HROIRgn = m_HTeachPolygonRgn;
		}
		else
		{
			POINT LTPoint, RBPoint;

			GetLTPoint(&LTPoint, pCalDataService);
			GetRBPoint(&RBPoint, pCalDataService);

			if (mbShape)
				GenCircle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
			else
				GenRectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
		}

		return HROIRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [GTRegion::GetROIHRegion] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HROIRgn;
		GenEmptyObj(&HROIRgn);
		return HROIRgn;
	}
}

void GTRegion::ResetLocalAlignResult(int iThreadIdx)
{
	GenEmptyObj(&m_HLocalAlignShapeRgn[iThreadIdx]);
	m_dLocalAlignMatchingScore[iThreadIdx] = -1;
	m_iLocalAlignDeltaX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignDeltaY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngle[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitXPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitYPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitXPos2[iThreadIdx] = NOT_ANGLE_LINE;
	m_iLocalAlignLineFitYPos2[iThreadIdx] = NOT_ANGLE_LINE;

	m_dLocalAlignAngleFitXPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignAngleFitYPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignAngleFitAngle[iThreadIdx] = INVALID_ALIGN_RESULT;
}

void GTRegion::ResetDontCareResult(int iThreadIdx)
{
	GenEmptyObj(&m_HDontCareResultRgn[iThreadIdx]);
}

void GTRegion::ResetGenerateResult(int iThreadIdx)
{
	GenEmptyObj(&m_HGenerateResultRgn[iThreadIdx]);
}

void GTRegion::ResetMaskResult(int iThreadIdx)
{
	GenEmptyObj(&m_HMaskResultRgn[iThreadIdx]);
}

void GTRegion::ClearAiRetrainFlag()
{
	for (int i = 0; i < GetChildTRegionCount(); i++) {
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->mbAiRetrainFlag = FALSE;
	}
	mbAiRetrainFlag = FALSE;
}

void GTRegion::ResetVmOptData()
{
	for (int i = 0; i < MAX_INSPECTION_TAB; i++)
	{
		TupleGenConst(0, 0, &m_HBrightAbsDiffs[i]);
		TupleGenConst(0, 0, &m_HBrightRatioDiffs[i]);
		TupleGenConst(0, 0, &m_HDarkAbsDiffs[i]);
		TupleGenConst(0, 0, &m_HDarkRatioDiffs[i]);
	}
}
