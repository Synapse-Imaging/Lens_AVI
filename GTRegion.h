#ifndef _GTREGION_H_
#define _GTREGION_H_

#include "PBC.h"
#include "GViewportManager.h"
#include "PList.hpp"
//#include "halconcpp.h"
#include "CalDataService.h"

//Region Style
#define GTR_RS_FILL					1
#define GTR_RS_NULL					2

// Draw Style
#define GTR_DS_NORMAL				0
#define GTR_DS_ACTIVE				1
#define GTR_DS_SELECTPART			2

// Control Point Half Length
#define GTR_CPHL					5

// Control Point Index
#define GTR_CP_NONE					-3
#define GTR_CP_SEGMENT				-2
#define GTR_CP_REGION				-1
#define GTR_CP_LT					0
#define GTR_CP_T					1
#define GTR_CP_RT					2
#define GTR_CP_R					3
#define GTR_CP_RB					4
#define GTR_CP_B					5
#define GTR_CP_LB					6
#define GTR_CP_L					7
#define GTR_CP_KEY_LEFT						8
#define GTR_CP_KEY_RIGHT					9
#define GTR_CP_KEY_UP						10
#define GTR_CP_KEY_DOWN						11
#define GTR_CP_CKEY_LEFT					12
#define GTR_CP_CKEY_RIGHT					13
#define GTR_CP_CKEY_UP						14
#define GTR_CP_CKEY_DOWN					15
#define GTR_CP_SKEY_LEFT					16
#define GTR_CP_SKEY_RIGHT					17
#define GTR_CP_SKEY_UP						18
#define GTR_CP_SKEY_DOWN					19

// Rotation Point Position
#define RPP_LEFT_TOP		0
#define RPP_RIGHT_TOP		1
#define RPP_RIGHT_BOTTOM	2
#define RPP_LEFT_BOTTOM		3

// Rotation Margin Type
#define RMT_NO_MARGIN		0
#define RMT_OUTER_MARGIN	1
#define RMT_INNER_MARGIN	2

class GTRegion : public PObject	// dynclass때문에 PObject에서 파생되었다.
{
	DECLARE_DYNCLASS(GTRegion)

public:
	GTRegion();
	virtual ~GTRegion();
	
protected:
	BOOL mbVisible;
	BOOL mbSelectable;
	BOOL mbMovable;
	BOOL mbSizable;
	BOOL mbSelect;

	DPOINT maPoints[2];	// Unit: um

	void Reset();

	BOOL PtInCPoint(int iVX, int iVY, POINT VPoint);
	BOOL PtInSegment(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);

	GTRegion		*mpParent;
	PList<GTRegion> *mpChildTRegionList;

public:
	int		miTeachImageIndex;	// (영상탭: 1~16)
	int		miInspectionType;	// 1~10: 검사 타입, 나머지: xx
	int		miFaiRoiIndex;
	BOOL	m_bRegionROI;
	HObject m_HTeachPolygonRgn;
	int		miInspectionROIID;
	int		miLocalAlignID;
	int		miDontCareID;
	int		miGenerateID;
	int		miMaskID;
	int		miColorInfoID;
	BOOL	mbShape;	// Added by Gojw	2005/11/25	FALSE:Rectangle, TRUE:Circle

	int			miLineStyle; 
	int			miLineThickness;
	COLORREF	mLineColor;
	COLORREF	mBackColor;
	int			miBkMode;
	int			miRegionStyle;
	COLORREF	mRegionColor;

	BOOL mbLastSelected;

	BOOL mbAiRetrainFlag;

	int miPositionID;
	int miPointID;

	CString GetInspectionTypeName();
	HObject GetROIHRegion(CCalDataService *pCalDataService);

	void ResetLocalAlignResult(int iThreadIdx);
	void ResetDontCareResult(int iThreadIdx);
	void ResetGenerateResult(int iThreadIdx);
	void ResetMaskResult(int iThreadIdx);
	void ClearAiRetrainFlag();
	void ResetVmOptData();

	virtual BOOL Load(HANDLE hFile, int iModelVersion, HTuple HFileHandle, CCalDataService *pCalDataService, int iVisionIdx, CString strConfig);
	virtual void Save(HANDLE hFile, HTuple HFileHandle, BOOL bVariationModelSave, BOOL bTemplateModelSave, int iVisionIdx, CString strConfig);

	virtual void Duplicate(GTRegion **ppTRegion, BOOL bOnlyParameter=FALSE);
	virtual GTRegion& operator=(GTRegion& data);
	
	void GetLTPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void GetRBPoint(POINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth = -1, int iImageHeight = -1);	//이미지 사이즈를 고려하려면 iImageWidth, iImageHeight를 입력해준다. iImageWidth, iImageHeight를 벗어나면 최대값으로 대치해준다. 입력하지 않으면 이미지 사이즈를 고려하지 않은 방식(이전방식)대로 돌아간다
	void GetWHPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void GetCenterPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void SetLTPoint(POINT Point, CCalDataService *pCalDataService);
	void SetRBPoint(POINT Point, CCalDataService *pCalDataService);

	void GetLTPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void GetRBPointD(DPOINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth = -1, int iImageHeight = -1);	//이미지 사이즈를 고려하려면 iImageWidth, iImageHeight를 입력해준다. iImageWidth, iImageHeight를 벗어나면 최대값으로 대치해준다. 입력하지 않으면 이미지 사이즈를 고려하지 않은 방식(이전방식)대로 돌아간다
	void GetWHPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void GetCenterPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void SetLTPointD(DPOINT DPoint, CCalDataService *pCalDataService);
	void SetRBPointD(DPOINT DPoint, CCalDataService *pCalDataService);

	void GetLTPointM(DPOINT *pMPoint);
	void GetRBPointM(DPOINT *pMPoint);
	void GetWHPointM(DPOINT *pWHPoint);
	void GetCenterPointM(DPOINT *pMCPoint);
	void SetLTPointM(DPOINT MPoint);
	void SetRBPointM(DPOINT MPoint);
	void SetWHPointM(DPOINT WHPoint);
	void SetCenterPointM(DPOINT CenterPoint);

	void ArrangePoints();

	int GetChildTRegionCount();
	void AddChildTRegion(GTRegion *pTRegion);
	void InsertChildTRegion(int iIndex, GTRegion *pTRegion);
	void DeleteChildTRegion(int iIndex);
	void DeleteChildTRegionPtr(GTRegion *pTRegion, BOOL bChildLink);
	void ClearAllChildTRegion();
	void DeleteAllSelectChildTRegion();
	void SwapChildTRegion(int a, int b);
	GTRegion *GetChildTRegion(int iIndex);

	BOOL RgnInRegion(GTRegion *pTRegion);
	BOOL PtInRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);
	void DrawControlPoint(HDC hDestDC, int iVX, int iVY);
	void DrawControlPoint(HTuple hDestDC, int iVX, int iVY);

	BOOL GetVisible();
	BOOL GetSelectable();
	BOOL GetMovable();
	BOOL GetSizable();
	BOOL GetSelect();
	GTRegion* GetCurSelectRegion();
	BOOL GetShape();		// Added by Gojw	2005/11/25

	void SetLineColor(COLORREF lineColor);				// GJW (2005/6/20)
	void SetVisible(BOOL bVisible, BOOL bChildLink);
	void SetSelectable(BOOL bSelectable, BOOL bChildLink);
	void SetMovable(BOOL bMovable, BOOL bChildLink);
	void SetSizable(BOOL bSizable, BOOL bChildLink);
	void SetSelect(BOOL bSelect, BOOL bChildLink);
	void SetShape(BOOL bShape, BOOL bChildLink);		// Added by Gojw	2005/11/25
	void ClearAllSelect();
	void ClearAllLastSelected();

	virtual GTRegion *GetTopTRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex=-1);
	virtual GTRegion *GetTopTRegionArea(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex, long *plArea);

	virtual int GetCPointIndex(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);
	virtual void SetCPoint(int iCPointIndex, POINT *pMIPoints, CCalDataService *pCalDataService);
	
	virtual void Scale(double dScale, BOOL bChildLink, CCalDataService *pCalDataService);

	virtual void MovePixel(int iXMove, int iYMove, BOOL bChildLink, CCalDataService *pCalDataService);
	virtual void MoveUm(double dXMove, double dYMove, BOOL bChildLink, CCalDataService *pCalDataService);

	// Drawing Functions
	virtual void Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex=-1);
	virtual void Draw(HTuple hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex=-1);
	virtual void Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int ViewOffsetX, int ViewOffsetY);

	// Algorithm Parameter
	CAlgorithmParam m_AlgorithmParam[MAX_INSPECTION_TAB];
	
	HTuple m_HROIAlignModelID;
	HTuple m_HVarModelID[MAX_INSPECTION_TAB];
	int	m_iVarModelImageSizeX[MAX_INSPECTION_TAB];
	int	m_iVarModelImageSizeY[MAX_INSPECTION_TAB];
	int	m_iVarNoTrainSample[MAX_INSPECTION_TAB];
	HObject m_HVarMeanImage[MAX_INSPECTION_TAB];
	HObject m_HVarStdevImage[MAX_INSPECTION_TAB];

	HTuple m_HPartModelID;

	// Measurement Data
	CMeasureData m_MeasureData[INSPECTION_BUFFER_COUNT_MAX][MAX_INSPECTION_TAB];

	// Variation Inspection Image
	HObject m_HVariationInspectImage[INSPECTION_BUFFER_COUNT_MAX];

	HObject	m_HADJImage;

	// Local Align Result
	HObject m_HLocalAlignShapeRgn[NO_MAX_INSPECT_THREAD];
	double m_dLocalAlignMatchingScore[NO_MAX_INSPECT_THREAD];
	int	m_iLocalAlignDeltaX[NO_MAX_INSPECT_THREAD];
	int	m_iLocalAlignDeltaY[NO_MAX_INSPECT_THREAD];
	double	m_dLocalAlignDeltaAngle[NO_MAX_INSPECT_THREAD];
	double	m_dLocalAlignDeltaAngleFixedPointX[NO_MAX_INSPECT_THREAD];
	double	m_dLocalAlignDeltaAngleFixedPointY[NO_MAX_INSPECT_THREAD];
	int m_iLocalAlignLineFitXPos[NO_MAX_INSPECT_THREAD];
	int m_iLocalAlignLineFitYPos[NO_MAX_INSPECT_THREAD];
	int m_iLocalAlignLineFitXPos2[NO_MAX_INSPECT_THREAD];
	int m_iLocalAlignLineFitYPos2[NO_MAX_INSPECT_THREAD];
	double m_dLocalAlignAngleFitXPos[NO_MAX_INSPECT_THREAD];
	double m_dLocalAlignAngleFitYPos[NO_MAX_INSPECT_THREAD];
	double m_dLocalAlignAngleFitAngle[NO_MAX_INSPECT_THREAD];

	// DontCare Rgn
	HObject m_HDontCareResultRgn[NO_MAX_INSPECT_THREAD];

	// Generate Rgn
	HObject m_HGenerateResultRgn[NO_MAX_INSPECT_THREAD];

	// Mask Rgn
	HObject m_HMaskResultRgn[NO_MAX_INSPECT_THREAD];

	// VM Optimize
	HTuple m_HBrightAbsDiffs[MAX_INSPECTION_TAB];
	HTuple m_HBrightRatioDiffs[MAX_INSPECTION_TAB];
	HTuple m_HDarkAbsDiffs[MAX_INSPECTION_TAB];
	HTuple m_HDarkRatioDiffs[MAX_INSPECTION_TAB];

	CString strLog;
};

#endif
