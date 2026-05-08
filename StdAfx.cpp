// stdafx.cpp : source file that includes just the standard includes
//	uScan.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return FALSE;
		}
	}

	return TRUE;
}

void edSaveLog(char *szFormat, ...)
{
#ifdef EDDY_SEQUENCE_LOG

	char szBuffer[1024]; char *pArguments; int strlen = 0;

	pArguments = (char*)&szFormat + sizeof(szFormat);

	if ((strlen = vsprintf(szBuffer, szFormat, pArguments)) >= 1023)
	{
		strLog("edSaveLog()- MaxStringLength Error..");
		THEAPP.m_log_system->error("{}", LOG_CSTR(strLog));
		return;
	}

	THEAPP.m_log_system->info("{}", LOG_CSTR(strLog));

#endif
}


double similarity(const HObject& XldOrg, const HObject& XldTgt)
{
	//////////////////////////////////////////////////////////////////////////
	// 입력 xld validity 검사
	//////////////////////////////////////////////////////////////////////////

	HTuple CntOrg;

	CountObj(XldOrg, &CntOrg);

	if (CntOrg <= 0)
		return 0.0;
	int i;
	for (i = 0; i < CntOrg; i++)
	{
		HObject SelectedXld;
		SelectObj(XldOrg, &SelectedXld, i + 1);

		HTuple Length;
		LengthXld(SelectedXld, &Length);

		if (Length > 0.0)
			goto PASS1;
	}

	return 0.0;

PASS1:

	HTuple CntTgt;

	CountObj(XldTgt, &CntTgt);

	if (CntTgt <= 0)
		return 0.0;

	for (i = 0; i < CntTgt; i++)
	{
		HObject SelectedXld;
		SelectObj(XldTgt, &SelectedXld, i + 1);

		HTuple Length;
		LengthXld(SelectedXld, &Length);

		if (Length > 0.0)
			goto PASS2;
	}

	return 0.0;

PASS2:

	//////////////////////////////////////////////////////////////////////////
	// similarity estimation
	//////////////////////////////////////////////////////////////////////////

	HTuple ModelID;

	//Herror err =	
	CreateShapeModelXld(XldOrg, "auto", 0.0, 0.0, "auto", "point_reduction_high", "ignore_local_polarity", 5, &ModelID);    // 180228jwj 반환값이 없어야 오류 없어짐
	//Herror err = CreateShapeModelXld(XldOrg, "auto", -0.39, 0.79, "auto", "auto", "ignore_local_polarity", 5, &ModelID);  // 180228jwj comment

	//if(err != H_MSG_TRUE) //180228jwj comment
	//	return 0.0;			//180228jwj comment

	HObject ImageTgt;
	gen_bin_image_xld_crop(XldTgt, &ImageTgt);



	HTuple Score;
	FindShapeModel(ImageTgt, ModelID, 0.0, 0.0, 0.0, 1, 0.5, "least_squares", 0, 0.9, NULL, NULL, NULL, &Score);
	//FindShapeModel(ImageTgt, ModelID, 0.0, 0.0, 0.0, 1, 0.5, "least_squares", 0, 0.9, _,_,_, &Score);  //180228jwj comment

	ClearShapeModel(ModelID);

	//if(Score.TupleNumber() <= 0)  //180228jwj
	if (Score.Length() <= 0)  //180228jwj	


		return 0.0;

	return Score[0].D();
}

void gen_bin_image_xld_crop(const HObject& Xld, HObject* pImage, long lMarginX, long lMarginY)
{
	if (!CGFunction::ValidHXLD(Xld))
		return;

	//////////////////////////////////////////////////////////////////////////
	// Create image
	//////////////////////////////////////////////////////////////////////////

	HTuple Row1, Col1, Row2, Col2;
	SmallestRectangle1Xld(Xld, &Row1, &Col1, &Row2, &Col2);

	TupleMin(Row1, &Row1);
	TupleMin(Col1, &Col1);
	TupleMax(Row2, &Row2);
	TupleMax(Col2, &Col2);

	HTuple HomMat2DIdentity;
	HomMat2dIdentity(&HomMat2DIdentity);

	HTuple MarginX = HTuple(lMarginX);
	HTuple MarginY = HTuple(lMarginY);

	HTuple HomMat2DTranslate;
	HomMat2dTranslate(HomMat2DIdentity, -(Row1 - MarginY), -(Col1 - MarginX), &HomMat2DTranslate);

	HObject XldTrans;
	AffineTransContourXld(Xld, &XldTrans, HomMat2DTranslate);

	DPOINT WHPoint;
	WHPoint.x = Col2[0].D() - Col1[0].D();
	WHPoint.y = Row2[0].D() - Row1[0].D();
	HObject ImageOrg;
	GenImageConst(&ImageOrg, "byte", WHPoint.x + lMarginX * 2, WHPoint.y + lMarginY * 2);
	PaintXld(XldTrans, ImageOrg, pImage, 255);
}

void gen_bin_image_xld(const HObject& Xld, HObject* pImage, long lMarginRight, long lMarginBottom)
{
	if (!CGFunction::ValidHXLD(Xld))
		return;

	//////////////////////////////////////////////////////////////////////////
	// Create image
	//////////////////////////////////////////////////////////////////////////

	HTuple Row1, Col1, Row2, Col2;
	SmallestRectangle1Xld(Xld, &Row1, &Col1, &Row2, &Col2);

	TupleMin(Row1, &Row1);
	TupleMin(Col1, &Col1);
	TupleMax(Row2, &Row2);
	TupleMax(Col2, &Col2);

	HObject Image;
	GenImageConst(&Image, "byte", Col2 + (Hlong)lMarginRight, Row2 + (Hlong)lMarginBottom);
	//GenImageConst(&Image, "byte", Col2 + lMarginRight, Row2 + lMarginBottom); //180228 comment
	PaintXld(Xld, Image, pImage, 255);

}

Herror intersection_contours_lc(const HObject& Region, const HObject& Contours, HObject* pContoursIntersection)
{
	Herror err = H_MSG_TRUE;

	HTuple Number;
	CountObj(Contours, &Number);

	for (int i = 1; i <= Number; i++)
	{
		HObject ObjectSelected;
		SelectObj(Contours, &ObjectSelected, i);

		HTuple Row, Col;
		GetContourXld(ObjectSelected, &Row, &Col);

		HTuple Length;
		TupleLength(Row, &Length);

		HTuple RowSelected = HTuple();
		HTuple ColSelected = HTuple();

		//pContoursIntersection->Reset();
		pContoursIntersection->Clear();
		GenEmptyObj(pContoursIntersection);

		for (int Index = 0; Index < Length; Index++)
		{
			HTuple IsInside;
			TestRegionPoint(Region, (HTuple)Row[Index], (HTuple)Col[Index], &IsInside);

			if (IsInside == 1)
			{
				TupleConcat(RowSelected, Row[Index], &RowSelected);
				TupleConcat(ColSelected, Col[Index], &ColSelected);

				if (Index != Length - 1)
					continue;
			}

			//if(RowSelected.TupleNumber() == 0)
			if (RowSelected.Length() == 0) //180228jwj
				continue;

			HObject ContourNew;

			GenContourPolygonXld(&ContourNew, RowSelected, ColSelected);
			ConcatObj(*pContoursIntersection, ContourNew, pContoursIntersection);

			RowSelected = HTuple();
			ColSelected = HTuple();
		}
	}

	return err;
}