// TrayImagePB.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayImagePB.h"

// CTrayImagePB

IMPLEMENT_DYNAMIC(CTrayImagePB, CStatic)

CTrayImagePB::CTrayImagePB()
{
	for (int i = 0; i < MAX_MODULE_TRAY; i++)
	{
		OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화
		ClickRegion[i] = FALSE;
	}

	WrongPointClick = FALSE;

	m_iVisionCamType = VISION_NUMBER_1;
	m_iVisionIdx = VISION_NUMBER_1;

	m_iLastClickRegionIdx = -1;

	WindowHandle = -1;

	bOnDrawing = FALSE;
}

CTrayImagePB::~CTrayImagePB()
{
	try
	{
		if (WindowHandle > -1)
		{
			HalconCpp::CloseWindow(WindowHandle);
			WindowHandle = -1;
		}
	}
	catch (HException &except) {}
}

void CTrayImagePB::Reset()
{
	for (int i = 0; i < MAX_MODULE_TRAY; i++)
	{
		OkNg[i] = DEFECT_TYPE_PROCESSING_ERROR;		// 초기화
		ClickRegion[i] = FALSE;
	}

	if (WindowHandle > -1)
	{
		HalconCpp::CloseWindow(WindowHandle);
		WindowHandle = -1;
	}

	WrongPointClick = FALSE;

	m_iLastClickRegionIdx = -1;

}

BEGIN_MESSAGE_MAP(CTrayImagePB, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DISPLAY_TRAY, OnDispTray)
END_MESSAGE_MAP()

// CTrayImagePB 메시지 처리기입니다.
LRESULT CTrayImagePB::OnDispTray(WPARAM wParam, LPARAM lParam)
{
	ReDraw();

	return 0;
}

void CTrayImagePB::ClearLastSelectedModule()
{
	try
	{
		if (m_iLastClickRegionIdx >= 0)
		{
			Union1(RegionErosion, &RegionErosion);
			Connection(RegionErosion, &RegionErosion);

			HObject sel;
			SelectObj(RegionErosion, &sel, m_iLastClickRegionIdx + 1);
			ClickRegion[m_iLastClickRegionIdx] = FALSE;

			HTuple Row1, Row2, Cloumn1, Cloumn2;
			SmallestRectangle1(sel, &Row1, &Cloumn1, &Row2, &Cloumn2);
			SetColor(WindowHandle, "white");
			DispRectangle1(WindowHandle, Row1, Cloumn1, Row2, Cloumn2);

			m_iLastClickRegionIdx = -1;

			//		Invalidate(FALSE);
			//		InvalidateRect(FALSE);
			ReDraw();
		}
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CTrayImagePB::ClearLastSelectedModule] : %s", (CString)(LPCTSTR)except.ErrorMessage());
	}
}

void CTrayImagePB::OnLButtonDown(UINT nFlags, CPoint point)
{
	CString sVisionCamType_Short;
	sVisionCamType_Short = THEAPP.m_ModelDefineInfo.m_strVisionName_Short[m_iVisionCamType];

	try
	{
		if (m_iVisionCamType != VISION_NUMBER_1 && m_iVisionCamType != VISION_NUMBER_2 && m_iVisionCamType != VISION_NUMBER_3 && m_iVisionCamType != VISION_NUMBER_4)
		{
			WrongPointClick = TRUE;
			ReDraw();

			CStatic::OnLButtonDown(nFlags, point);
			return;
		}

		HANDLE hFindFile;
		WIN32_FIND_DATA FindFileData;
		CString sReadFileName;

		int i;

		if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)		// 관리자모드에서만
		{
			for (i = 0; i < THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][VISION_NUMBER_1]->m_iTrayModuleMax; i++)
			{
				ClickRegion[i] = FALSE;
			}

			Union1(RegionErosion, &RegionErosion);
			Connection(RegionErosion, &RegionErosion);

			HObject Hrect, Hintersection;
			HTuple  HInterArea, HInterRow, HInterColumn;
			GetMposition(WindowHandle, &Row, &Column, &Button);
			GetRegionIndex(RegionErosion, Row, Column, &Index);	// Index: 1~N

			THEAPP.m_pTrayAdminViewDlg1->m_TrayImagePB.ClearLastSelectedModule();
			THEAPP.m_pTrayAdminViewDlg2->m_TrayImagePB.ClearLastSelectedModule();
			THEAPP.m_pTrayAdminViewDlg3->m_TrayImagePB.ClearLastSelectedModule();
			THEAPP.m_pTrayAdminViewDlg4->m_TrayImagePB.ClearLastSelectedModule();

			THEAPP.m_iTeachingModuleNo = TEACHING_MODULE_NO_UNDEFINED;
			THEAPP.m_iLastSelectVisionCamType = -1;
			THEAPP.m_iLastSelectPcVision = -1;

			HObject sel;
			SelectObj(RegionErosion, &sel, Index);

			HTuple Row1, Row2, Cloumn1, Cloumn2;
			SmallestRectangle1(sel, &Row1, &Cloumn1, &Row2, &Cloumn2);
			SetColor(WindowHandle, "yellow");
			DispRectangle1(WindowHandle, Row1, Cloumn1, Row2, Cloumn2);

			m_iLastClickRegionIdx = Index[0].I() - 1;

			ClickRegion[m_iLastClickRegionIdx] = TRUE;
			//		Invalidate(FALSE);
			//		InvalidateRect(FALSE);
			ReDraw();

			if (THEAPP.m_bShowReviewWindow == TRUE && (Index[0].I() - 1) >= 0)
			{
				CString FileName;
				HObject HImage;

				int iModuleNr = Index[0].I();

				//////////////////////////////////////////////////////////////////////////
				///  RawImage 영상 복사

				CString strRawImageFileFullName, strResultFileFullName;
				CString RawImageFileName, ResultFileName;
				CString strRawImagePath;

				THEAPP.m_pInspectViewOverayImageDlg1->ResetRawImage();

				int iNoValidImage = 0;

				if (THEAPP.Struct_PreferenceStruct.m_bCombineRawImage)
				{
					int iNoInspectImage = 0;
					iNoInspectImage = THEAPP.m_pDualModelDataManager[TEACHING_MZ_NO - 1][m_iVisionIdx]->m_iNoUsedImageGrab;

					int iNoImageYDir, iModular;
					int iCropImageSizeX, iCropImageSizeY;

					iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
					iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
					if (iModular > 0)
						iNoImageYDir += 1;
					RawImageFileName.Format("Module%d_%s_Image%02d*", iModuleNr, sVisionCamType_Short, iNoInspectImage);

					if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
					{
						sReadFileName = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx] + "\\" + RawImageFileName;
						strRawImagePath = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx];

						CString sReadFileNameTemp1, sReadFileNameTemp2, sReadFileNameTemp3;
						CString strRawImagePathTemp1, strRawImagePathTemp2, strRawImagePathTemp3;
						if (THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						{
							if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
							{
								sReadFileNameTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp1[m_iVisionIdx] + "\\" + RawImageFileName;
								strRawImagePathTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp1[m_iVisionIdx];
							}
							if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2)
							{
								sReadFileNameTemp2 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp2[m_iVisionIdx] + "\\" + RawImageFileName;
								strRawImagePathTemp2 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp2[m_iVisionIdx];
							}
							if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3)
							{
								sReadFileNameTemp3 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp3[m_iVisionIdx] + "\\" + RawImageFileName;
								strRawImagePathTemp3 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp3[m_iVisionIdx];
							}
						}

						if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePath, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
						else if ((hFindFile = FindFirstFile(sReadFileNameTemp1, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp1, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
						else if ((hFindFile = FindFirstFile(sReadFileNameTemp2, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp2, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
						else if ((hFindFile = FindFirstFile(sReadFileNameTemp3, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp3, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
					}
					else
					{
						sReadFileName = THEAPP.m_FileBase.m_strLastRawImageFolder_LAS[m_iVisionIdx] + "\\" + RawImageFileName;
						strRawImagePath = THEAPP.m_FileBase.m_strLastRawImageFolder_LAS[m_iVisionIdx];

						CString sReadFileNameTemp1;
						CString strRawImagePathTemp1;
						if (THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
						{
							if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
							{
								sReadFileNameTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx] + "\\" + RawImageFileName;
								strRawImagePathTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx];
							}
						}

						if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePath, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
						else if ((hFindFile = FindFirstFile(sReadFileNameTemp1, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp1, (LPCTSTR)FindFileData.cFileName);

							FindClose(hFindFile);

							ReadImage(&HImage, HTuple(strRawImageFileFullName));
						}
					}

					if (THEAPP.m_pGFunction->ValidHImage(HImage) == TRUE)
					{
						HTuple htImageHeight = 0;
						HTuple htImageWidth = 0;
						HalconCpp::GetImageSize(HImage, &htImageWidth, &htImageHeight);
						int wd = (int)htImageWidth.L();
						int ht = (int)htImageHeight.L();

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;

						for (i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							CropRectangle1(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]), CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]))
								++iNoValidImage;
						}
					}
				}
				else
				{
					for (i = 0; i < MAX_IMAGE_TAB; i++)
					{
						RawImageFileName.Format("Module%d_%s_Image%02d*", iModuleNr, sVisionCamType_Short, i + 1);

						if (THEAPP.Struct_PreferenceStruct.m_bRawImageSaveLocal)
						{
							sReadFileName = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx] + "\\" + RawImageFileName;
							strRawImagePath = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx];

							CString sReadFileNameTemp1, sReadFileNameTemp2, sReadFileNameTemp3;
							CString strRawImagePathTemp1, strRawImagePathTemp2, strRawImagePathTemp3;
							if (THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							{
								if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
								{
									sReadFileNameTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp1[m_iVisionIdx] + "\\" + RawImageFileName;
									strRawImagePathTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp1[m_iVisionIdx];
								}
								if (THEAPP.Struct_PreferenceStruct.m_bUseLAS2)
								{
									sReadFileNameTemp2 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp2[m_iVisionIdx] + "\\" + RawImageFileName;
									strRawImagePathTemp2 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp2[m_iVisionIdx];
								}
								if (THEAPP.Struct_PreferenceStruct.m_bUseLAS3)
								{
									sReadFileNameTemp3 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp3[m_iVisionIdx] + "\\" + RawImageFileName;
									strRawImagePathTemp3 = THEAPP.m_FileBase.m_strLastRawImageFolder_Temp3[m_iVisionIdx];
								}
							}

							if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePath, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
							else if ((hFindFile = FindFirstFile(sReadFileNameTemp1, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp1, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
							else if ((hFindFile = FindFirstFile(sReadFileNameTemp2, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp2, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
							else if ((hFindFile = FindFirstFile(sReadFileNameTemp3, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp3, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
						}
						else
						{
							sReadFileName = THEAPP.m_FileBase.m_strLastRawImageFolder_LAS[m_iVisionIdx] + "\\" + RawImageFileName;
							strRawImagePath = THEAPP.m_FileBase.m_strLastRawImageFolder_LAS[m_iVisionIdx];

							CString sReadFileNameTemp1;
							CString strRawImagePathTemp1;
							if (THEAPP.Struct_PreferenceStruct.m_bUseRawImageTempDrive)
							{
								if (THEAPP.Struct_PreferenceStruct.m_bUseLAS1)
								{
									sReadFileNameTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx] + "\\" + RawImageFileName;
									strRawImagePathTemp1 = THEAPP.m_FileBase.m_strLastRawImageFolder_Local[m_iVisionIdx];
								}
							}

							if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePath, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
							else if ((hFindFile = FindFirstFile(sReadFileNameTemp1, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								strRawImageFileFullName.Format("%s\\%s", strRawImagePathTemp1, (LPCTSTR)FindFileData.cFileName);

								FindClose(hFindFile);

								ReadImage(&HImage, HTuple(strRawImageFileFullName));
								CopyImage(HImage, &(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[i]));

								++iNoValidImage;
							}
						}
					}
				}

				THEAPP.m_pInspectViewOverayImageDlg1->m_iNoValidRawImage = iNoValidImage;

				if (THEAPP.m_pInspectViewOverayImageDlg1->iCurRawImageIdx >= iNoValidImage)
					THEAPP.m_pInspectViewOverayImageDlg1->iCurRawImageIdx = 0;

				THEAPP.m_bReviewImageClick = TRUE;

				if (m_iVisionIdx == VISION_NUMBER_1)
					THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_VISION_N1;
				else if (m_iVisionIdx == VISION_NUMBER_2)
					THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_VISION_N2;
				else if (m_iVisionIdx == VISION_NUMBER_3)
					THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_VISION_N3;
				else if (m_iVisionIdx == VISION_NUMBER_4)
					THEAPP.m_iCurSelectedResultModule = SELECTED_RESULT_VISION_N4;

				BYTE *pImageData;
				char type[30];
				Hlong lImageWidth, lImageHeight;
				HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[0]))
				{
					GetImagePointer1(THEAPP.m_pInspectViewOverayImageDlg1->m_HRawImage[0], &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

					lImageWidth = HlImageWidth.L();
					lImageHeight = HlImageHeight.L();

					if (THEAPP.m_pInspectViewOverayImageDlg1->m_iImageWidth != (int)lImageWidth)
					{
						THEAPP.m_pInspectViewOverayImageDlg1->m_iImageWidth = (int)lImageWidth;
						THEAPP.m_pInspectViewOverayImageDlg1->m_iImageHeight = (int)lImageHeight;
						THEAPP.m_pInspectViewOverayImageDlg1->CallFitImageHeight();
					}

					THEAPP.m_pInspectViewOverayImageDlg1->InvalidateRect(false);
				}

				//////////////////////////////////////////////////////////////////////////
				///  Result Capture Image 로딩 & Display

				GenEmptyObj(&THEAPP.m_pInspectViewOverayImageDlg2->m_pHImage);
				GenEmptyObj(&THEAPP.m_pInspectViewOverayImageDlg3->m_pHImage);
				GenEmptyObj(&THEAPP.m_pInspectViewOverayImageDlg4->m_pHImage);
				THEAPP.m_pInspectViewOverayImageDlg2->UpdateView(TRUE);
				THEAPP.m_pInspectViewOverayImageDlg3->UpdateView(TRUE);
				THEAPP.m_pInspectViewOverayImageDlg4->UpdateView(TRUE);

				ResultFileName.Format("Module%d_%s_Result%d_*", iModuleNr, sVisionCamType_Short, THEAPP.m_pInspectViewOverayImageDlg2->m_iCurResultNo);

				sReadFileName = THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx] + "\\" + ResultFileName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strResultFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx], (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&THEAPP.m_pInspectViewOverayImageDlg2->m_pHImage, HTuple(strResultFileFullName));
					GetImagePointer1(THEAPP.m_pInspectViewOverayImageDlg2->m_pHImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
					lImageWidth = HlImageWidth.L();
					lImageHeight = HlImageHeight.L();
					THEAPP.m_pInspectViewOverayImageDlg2->m_iImageWidth = (int)lImageWidth;
					THEAPP.m_pInspectViewOverayImageDlg2->m_iImageHeight = (int)lImageHeight;

					// Need to modify
					THEAPP.m_pInspectViewOverayImageDlg2->m_sWindowName.Format("Result %d", THEAPP.m_pInspectViewOverayImageDlg2->m_iCurResultNo);
					THEAPP.m_pInspectViewOverayImageDlg2->CallFitImageWidth();
				}

				ResultFileName.Format("Module%d_%s_Result%d_*", iModuleNr, sVisionCamType_Short, THEAPP.m_pInspectViewOverayImageDlg3->m_iCurResultNo);

				sReadFileName = THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx] + "\\" + ResultFileName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strResultFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx], (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&(THEAPP.m_pInspectViewOverayImageDlg3->m_pHImage), HTuple(strResultFileFullName));
					GetImagePointer1(THEAPP.m_pInspectViewOverayImageDlg3->m_pHImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
					lImageWidth = HlImageWidth.L();
					lImageHeight = HlImageHeight.L();
					THEAPP.m_pInspectViewOverayImageDlg3->m_iImageWidth = (int)lImageWidth;
					THEAPP.m_pInspectViewOverayImageDlg3->m_iImageHeight = (int)lImageHeight;

					// Need to modify
					THEAPP.m_pInspectViewOverayImageDlg3->m_sWindowName.Format("Result %d", THEAPP.m_pInspectViewOverayImageDlg3->m_iCurResultNo);
					THEAPP.m_pInspectViewOverayImageDlg3->CallFitImageWidth();
				}

				ResultFileName.Format("Module%d_%s_Result%d_*", iModuleNr, sVisionCamType_Short, THEAPP.m_pInspectViewOverayImageDlg4->m_iCurResultNo);

				sReadFileName = THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx] + "\\" + ResultFileName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strResultFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strLastResultImageFolder[m_iVisionIdx], (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					ReadImage(&(THEAPP.m_pInspectViewOverayImageDlg4->m_pHImage), HTuple(strResultFileFullName));
					GetImagePointer1(THEAPP.m_pInspectViewOverayImageDlg4->m_pHImage, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);
					lImageWidth = HlImageWidth.L();
					lImageHeight = HlImageHeight.L();
					THEAPP.m_pInspectViewOverayImageDlg4->m_iImageWidth = (int)lImageWidth;
					THEAPP.m_pInspectViewOverayImageDlg4->m_iImageHeight = (int)lImageHeight;

					// Need to modify
					THEAPP.m_pInspectViewOverayImageDlg4->m_sWindowName.Format("Result %d", THEAPP.m_pInspectViewOverayImageDlg4->m_iCurResultNo);
					THEAPP.m_pInspectViewOverayImageDlg4->CallFitImageWidth();
				}

				THEAPP.m_iTeachingModuleNo = iModuleNr;
				THEAPP.m_iLastSelectVisionCamType = m_iVisionCamType;
				THEAPP.m_iLastSelectPcVision = m_iVisionIdx;

			}
		}

		CStatic::OnLButtonDown(nFlags, point);
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CTrayImagePB::OnLButtonDown] : %s", (CString)(LPCTSTR)except.ErrorMessage());
		WrongPointClick = TRUE;
		//InvalidateRect(FALSE);
		ReDraw();
	}
}

void CTrayImagePB::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	ReDraw();
}

CRect CTrayImagePB::SetPictureBoxSize(CRect PbRect)
{
	CRect rect;

	rect.top = 0;
	rect.bottom = PbRect.Height() - 20;
	rect.left = 0;
	rect.right = PbRect.Width();

	MoveWindow(0, 0, PbRect.Width(), PbRect.Height() - 20);

	return rect;
}

void CTrayImagePB::ReDraw()
{
	try
	{
		bOnDrawing = TRUE;

		HObject SelObj;
		HTuple HTuArea, HTuRow, HTuCol, HTuCNT;
		HObject HRgn_conn;
		HTuple HTuNumber;

		Connection(RegionErosion, &HRgn_conn);
		HalconCpp::SetFont(WindowHandle, "Courier New-Bold-17");
		CountObj(HRgn_conn, &HTuNumber);

		int iRegionCount = 0;
		if (HTuNumber.Length() > 0)
			iRegionCount = HTuNumber[0].I();

		if (iRegionCount > MAX_MODULE_TRAY)
		{
			THEAPP.m_log_halcon->warn("ReDraw: Too many regions. count={}, limit={}", iRegionCount, MAX_MODULE_TRAY);
			iRegionCount = MAX_MODULE_TRAY;
		}

		for (int i = 0; i < iRegionCount; i++)
		{
			SelectObj(HRgn_conn, &SelObj, i + 1);
			HTuple HTuRow1, HTuRow2, HTuCol1, HTuCol2;
			AreaCenter(SelObj, &HTuArea, &HTuRow, &HTuCol);
			SetTposition(WindowHandle, HTuRow - 12, HTuCol - 8);
			if (!WrongPointClick)
			{
				if (!ClickRegion[i])
				{
					SmallestRectangle1(SelObj, &HTuRow1, &HTuCol1, &HTuRow2, &HTuCol2);

					int idx = i;
					const int iDefectResult = OkNg[i];

					if (iDefectResult == DEFECT_TYPE_PROCESSING_ERROR)
					{
						SetColor(WindowHandle, "white");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
					}
					else if (iDefectResult == DEFECT_TYPE_GOOD)
					{
						SetColor(WindowHandle, "white");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "green");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetTposition(WindowHandle, HTuRow - 12, HTuCol - 4);
						SetColor(WindowHandle, "black");
						WriteString(WindowHandle, "G");
					}
					else if (iDefectResult >= DEFECT_TYPE_FIRST && iDefectResult <= DEFECT_TYPE_LAST)
					{
						SetColor(WindowHandle, "white");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "red");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "yellow");
						if (0 <= iDefectResult && iDefectResult < (int)g_strInspectionTypeName_Short.size())
							WriteString(WindowHandle, HTuple(g_strInspectionTypeName_Short[iDefectResult].c_str()));
						else
							THEAPP.m_log_halcon->warn("ReDraw: Array data mismatching. iDefectResult={}", iDefectResult);
					}
					else if (iDefectResult == DEFECT_TYPE_LIGHT_ERROR)
					{
						SetColor(WindowHandle, "yellow");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "black");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "yellow");
						WriteString(WindowHandle, "LI");
					}
					else if (iDefectResult == DEFECT_TYPE_GRAB_FAIL)
					{
						SetColor(WindowHandle, "medium slate blue");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "white");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "medium slate blue");
						WriteString(WindowHandle, "GF");
					}
					else if (iDefectResult == DEFECT_TYPE_ALIGN_ERROR)
					{
						SetColor(WindowHandle, "magenta");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "white");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "red");
						WriteString(WindowHandle, "MC");
					}
					else if (iDefectResult == DEFECT_TYPE_BARCODE_NOT_SAME)
					{
						SetColor(WindowHandle, "magenta");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "black");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "magenta");
						WriteString(WindowHandle, "BA");
					}
					else if (iDefectResult == DEFECT_TYPE_BARCODE_MIX)
					{
						SetColor(WindowHandle, "magenta");
						DispRectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						SetColor(WindowHandle, "black");
						DispRectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						SetColor(WindowHandle, "magenta");
						WriteString(WindowHandle, "BM");
					}
				}
			}
			else
			{
				WrongPointClick = FALSE;
			}

			if (i + 1 < 10)
				SetTposition(WindowHandle, HTuRow, HTuCol - 4);
			else
				SetTposition(WindowHandle, HTuRow, HTuCol - 8);

			SetColor(WindowHandle, "blue");

			HTuple HHTuCNT;
			TupleInt(i + 1, &HHTuCNT);
			WriteString(WindowHandle, HHTuCNT);
		}

		bOnDrawing = FALSE;

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [TrayImagePB::ReDraw] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		bOnDrawing = FALSE;
	}
}