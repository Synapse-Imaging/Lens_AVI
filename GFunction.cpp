// GFunction.cpp : implementation file
//

#include "stdafx.h"
#include "uScan.H"
#include "GFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGFunction

CGFunction* CGFunction::m_pInstance = NULL;

CGFunction* CGFunction::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CGFunction();
		if (!m_pInstance->m_hWnd) {
			CRect r = m_pInstance->GetPosition();
			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "CGFunction", 0, r, NULL, 0, NULL);
		}
	}
	return m_pInstance;
}

void CGFunction::DeleteInstance()
{
	if (m_pInstance->m_hWnd)
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

CGFunction::CGFunction()
{
}

CGFunction::~CGFunction()
{
}

BOOL CGFunction::ValidHImage(HObject rHObject)
{
	try
	{
		bool bValid = rHObject.IsInitialized();

		if (bValid)
		{
			HTuple HObjectNum;
			CountObj(rHObject, &HObjectNum);

			if (HObjectNum > 0)
			{
				return TRUE;
			}
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}

}

BOOL CGFunction::ValidHRegion(HObject rHObject)
{
	try
	{
		bool bValid = rHObject.IsInitialized();

		if (bValid)
		{
			HTuple HObjectNum;
			CountObj(rHObject, &HObjectNum);

			if (HObjectNum > 0)
			{
				HTuple Area, Row, Column;

				AreaCenter(rHObject, &Area, &Row, &Column);

				for (int i = 0; i < HObjectNum; i++)
				{
					if (Area[i].L() > 0)
						return TRUE;
				}
			}
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}

}

BOOL CGFunction::ValidHXLD(HObject rHObject)
{
	try
	{
		bool bValid = rHObject.IsInitialized();

		if (bValid)
		{
			HTuple HObjectNum;
			CountObj(rHObject, &HObjectNum);

			if (HObjectNum > 0)
			{
				HTuple ContLength;
				LengthXld(rHObject, &ContLength);

				if (ContLength > 0)
					return TRUE;
			}
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}
}

void CGFunction::ConvertTupleToFloat(HTuple T, float **dp)
{
	double *dptr, d;
	HTuple tl;

	TupleLength(T, &tl);
	int itl = (int)tl.L();

	dptr = T.ToDArr();
	*dp = (float *)malloc(itl * sizeof(float));
	for (int i = 0; i < itl; i++)
	{
		d = *(dptr + i);
		(*dp)[i] = (float)d;
	}
}

void CGFunction::DisplayCross(HTuple lWindowHandle, double dXPos, double dYPos, COLORREF Color, double dRadius)
{
	SetRgb(lWindowHandle, GetRValue(Color), GetGValue(Color), GetBValue(Color));

	DispLine(lWindowHandle, dYPos, dXPos - dRadius, dYPos, dXPos + dRadius);
	DispLine(lWindowHandle, dYPos - dRadius, dXPos, dYPos + dRadius, dXPos);
}

void CGFunction::DisplayCross(HDC hDestDC, GViewportManager *pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl)
{
	POINT CPoint, sPoint;
	POINT Left, Right, Top, Bottom;

	CPoint.x = (long)(dXPos + 0.5);
	CPoint.y = (long)(dYPos + 0.5);

	Left.x = CPoint.x - lRadiusPxl;
	Left.y = CPoint.y;
	Right.x = CPoint.x + lRadiusPxl;
	Right.y = CPoint.y;
	Top.x = CPoint.x;
	Top.y = CPoint.y - lRadiusPxl;
	Bottom.x = CPoint.x;
	Bottom.y = CPoint.y + lRadiusPxl;

	pVManager->IPtoVP(&Left, 1);
	pVManager->IPtoVP(&Right, 1);
	pVManager->IPtoVP(&Top, 1);
	pVManager->IPtoVP(&Bottom, 1);

	HPEN hOldPen, hPen;
	hPen = CreatePen(PS_SOLID, 1, Color);
	hOldPen = (HPEN)SelectObject(hDestDC, hPen);

	MoveToEx(hDestDC, Left.x, Left.y, &sPoint);
	LineTo(hDestDC, Right.x, Right.y);

	MoveToEx(hDestDC, Top.x, Top.y, &sPoint);
	LineTo(hDestDC, Bottom.x, Bottom.y);

	SelectObject(hDestDC, hOldPen);
	DeleteObject(hPen);
}

BOOL CGFunction::GetIntersectionRegion(HObject HInputRgn, HObject HTestRgn, HObject* pHIntersectRgn, long lMinCheckSize)
{
	pHIntersectRgn->Clear();
	GenEmptyObj(pHIntersectRgn);

	HObject HConnectedRgn;
	Connection(HInputRgn, &HConnectedRgn);
	SelectShape(HConnectedRgn, &HConnectedRgn, "area", "and", lMinCheckSize, MAX_DEF);

	Hlong lCount = 0;
	HTuple HlCount;
	CountObj(HConnectedRgn, &HlCount);
	lCount = HlCount.L();
	if (lCount <= 0)
		return FALSE;

	HObject HTempRgn;
	Hlong lArea;
	double dRow, dCol;
	HTuple HlArea, HdRow, HdCol;

	for (int i = 0; i < lCount; i++)
	{
		SelectObj(HConnectedRgn, &HTempRgn, i + 1);
		Intersection(HTempRgn, HInputRgn, &HTempRgn);

		lArea = 0;

		(HTempRgn, &HlArea, &HdRow, &HdCol);
		lArea = HlArea.L();
		dRow = HdRow.D();
		dCol = HdCol.D();

		if (lArea > 0)
		{
			*pHIntersectRgn = HTempRgn;
			return TRUE;
		}
	}

	return FALSE;
}

void CGFunction::ScaleImageRange(HObject Image, HObject *ImageScaled, HTuple Min, HTuple Max)
{
	// Local iconic variables 
	HObject  SelectedChannel, LowerRegion, UpperRegion;


	// Local control variables 
	HTuple  LowerLimit, UpperLimit, Mult, Add, Channels;
	HTuple  Index, MinGray, MaxGray, ExpDefaultCtrlDummyVar;

	//Convenience procedure to scale the gray values of the
	//input image Image from the interval [Min,Max]
	//to the interval [0,255] (default).
	//Grey values < 0 or > 255 (after scaling) are clipped.
	//
	//If the image shall be scaled to an interval different from [0,255],
	//this can be achieved by passing tuples with 2 values [From, To]
	//as Min and Max.
	//Example:
	//ScaleImage_range(Image:ImageScaled:[100,50],[200,250])
	//maps the gray values of Image from the interval [100,200] to [50,250].
	//All other gray values will be clipped.
	//
	//input parameters:
	//Image: the input image
	//Min: the minimum gray value which will be mapped to 0
	//     If a tuple with two values is given, the first value will
	//     be mapped to the second value.
	//Max: The maximum gray value which will be mapped to 255
	//     If a tuple with two values is given, the first value will
	//     be mapped to the second value.
	//
	//output parameter:
	//ImageScale: the resulting scaled image
	//
	if ((Min.Length()) == 2)
	{
		LowerLimit = Min[1];
		Min = Min[0];
	}
	else
	{
		LowerLimit = 0.0;
	}
	if ((Max.Length()) == 2)
	{
		UpperLimit = Max[1];
		Max = Max[0];
	}
	else
	{
		UpperLimit = 255.0;
	}
	//
	//Calculate scaling parameters
	Mult = ((UpperLimit - LowerLimit).TupleReal()) / (Max - Min);
	Add = ((-Mult)*Min) + LowerLimit;
	//
	//Scale image
	ScaleImage(Image, &Image, Mult, Add);
	//
	//Clip gray values if necessary
	//This must be done for each channel separately
	CountChannels(Image, &Channels);
	for (Index = 1; Index <= Channels; Index += 1)
	{
		AccessChannel(Image, &SelectedChannel, Index);
		MinMaxGray(SelectedChannel, SelectedChannel, 0, &MinGray, &MaxGray, &ExpDefaultCtrlDummyVar);
		Threshold(SelectedChannel, &LowerRegion, (MinGray.TupleConcat(LowerLimit)).TupleMin(),
			LowerLimit);
		Threshold(SelectedChannel, &UpperRegion, UpperLimit, (UpperLimit.TupleConcat(MaxGray)).TupleMax());
		PaintRegion(LowerRegion, SelectedChannel, &SelectedChannel, LowerLimit, "fill");
		PaintRegion(UpperRegion, SelectedChannel, &SelectedChannel, UpperLimit, "fill");
		if (Index == 1)
		{
			CopyObj(SelectedChannel, &(*ImageScaled), 1, 1);
		}
		else
		{
			AppendChannel((*ImageScaled), SelectedChannel, &(*ImageScaled));
		}
	}
	return;
}

double CGFunction::Distance(const Point& p1, const Point& p2)
{
	double dblDistance = 0.0;

	// 피타고라스의 정리 
	dblDistance = sqrt(pow(double(p1.x - p2.x), 2) + pow(double(p1.y - p2.y), 2));

	return dblDistance;
}

cv::Mat CGFunction::convert_hobject_to_mat(const HObject& h_img) {
	cv::Mat cv_img;
	HTuple channels, w, h;
	HObject h_image = h_img;

	HalconCpp::ConvertImageType(h_image, &h_image, "byte");
	HalconCpp::CountChannels(h_image, &channels);

	if (channels.I() == 1) {
		HTuple pointer;
		GetImagePointer1(h_image, &pointer, nullptr, &w, &h);
		int width = w.I(), height = h.I();
		cv_img = cv::Mat(height, width, CV_8UC1, (void*)(pointer.L())).clone();
	}
	else if (channels.I() == 3) {
		HTuple pointer_r, pointer_g, pointer_b;
		HalconCpp::GetImagePointer3(h_image, &pointer_r, &pointer_g, &pointer_b, nullptr, &w, &h);
		int width = w.I(), height = h.I();
		cv::Mat r(height, width, CV_8UC1, (void*)(pointer_r.L()));
		cv::Mat g(height, width, CV_8UC1, (void*)(pointer_g.L()));
		cv::Mat b(height, width, CV_8UC1, (void*)(pointer_b.L()));
		std::vector<cv::Mat> channels_vec = { b, g, r };
		cv::merge(channels_vec, cv_img);
	}
	return cv_img;
}

void CGFunction::convert_list_2_region(const std::list<std::list<std::pair<int, int>>>& external_contours, const std::list<std::list<std::pair<int, int>>>& internal_contours, HRegion& output_region)
{
	HRegion external_region, internal_region;

	// 외부 컨투어 처리
	for (const auto& contour : external_contours) {
		HTuple rows, cols;

		// 좌표를 Halcon의 튜플로 변환
		for (const auto& point : contour) {
			rows.Append(point.second); // y 좌표
			cols.Append(point.first);  // x 좌표
		}

		// 외부 컨투어를 리전으로 생성
		HRegion single_region;
		single_region.GenRegionPolygonFilled(rows, cols);
		external_region = (external_region.IsInitialized() ? external_region.Union2(single_region) : single_region);
	}

	// 내부 컨투어 처리
	for (const auto& contour : internal_contours) {
		HTuple rows, cols;

		// 좌표를 Halcon의 튜플로 변환
		for (const auto& point : contour) {
			rows.Append(point.second); // y 좌표
			cols.Append(point.first);  // x 좌표
		}

		// 내부 컨투어를 리전으로 생성
		HRegion single_region;
		single_region.GenRegionPolygonFilled(rows, cols);
		internal_region = (internal_region.IsInitialized() ? internal_region.Union2(single_region) : single_region);
	}

	// 최종 리전 생성: 외부 - 내부
	if (external_region.IsInitialized()) {
		if (internal_region.IsInitialized()) {
			output_region = external_region.Difference(internal_region);
		}
		else {
			output_region = external_region; // 내부 컨투어가 없으면 외부만 반환
		}
	}
	else {
		output_region = internal_region; // 외부 컨투어가 없으면 내부만 반환
	}
}


DRECT::DRECT()
{
}

DRECT::~DRECT()
{
}

BOOL DRECT::PtInRect(DPOINT dPoint)
{
	if (dPoint.x >= left && dPoint.x <= right && dPoint.y >= top && dPoint.y <= bottom)
		return TRUE;
	else
		return FALSE;
}

BEGIN_MESSAGE_MAP(CGFunction, CWnd)
	//{{AFX_MSG_MAP(CGFunction)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGFunction message handlers

