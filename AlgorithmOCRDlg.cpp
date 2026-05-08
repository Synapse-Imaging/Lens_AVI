// AlgorithmOCRDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmOCRDlg.h"
#include "afxdialogex.h"

#include "CreateFontDlg.h"
// CAlgorithmOCRDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAlgorithmOCRDlg, CDialog)

CAlgorithmOCRDlg::CAlgorithmOCRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmOCRDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseOCR = FALSE;
	m_lEditCharPlateWidth = 45;
	m_lEditMinCharWidth = 15;
	m_lEditMaxCharWidth = 285;
	m_lEditMinCharHeight = 15;
	m_lEditMaxCharHeight = 265;
	m_lEditMinCharCandidateHeight = 20;
	m_lEditWholeAreaDivide = 10;
	m_lEditHorizontalDivide = 50;
	m_lEditFirstDiffernt = 70;
	m_lEditSecondDifferent = 50;
	m_lEditdiff = 5;
	m_iEditOCRSegmentDilation = 3;
}

CAlgorithmOCRDlg::~CAlgorithmOCRDlg()
{
}

void CAlgorithmOCRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_OCR, m_bCheckUseOCR);
	DDX_Control(pDX, IDC_STATIC_1, m_HWinPanel1);
	DDX_Control(pDX, IDC_STATIC_2, m_HWinPanel2);
	DDX_Control(pDX, IDC_STATIC_3, m_HWinPanel3);
	DDX_Control(pDX, IDC_STATIC_4, m_HWinPanel4);
	DDX_Control(pDX, IDC_STATIC_5, m_HWinPanel5);
	DDX_Control(pDX, IDC_STATIC_6, m_HWinPanel6);
	DDX_Control(pDX, IDC_STATIC_7, m_HWinPanel7);
	DDX_Control(pDX, IDC_STATIC_8, m_HWinPanel8);
	DDX_Control(pDX, IDC_STATIC_9, m_HWinPanel9);
	DDX_Control(pDX, IDC_STATIC_10, m_HWinPanel10);
	DDX_Control(pDX, IDC_STATIC_11, m_HWinPanel11);
	DDX_Control(pDX, IDC_STATIC_12, m_HWinPanel12);
	DDX_Control(pDX, IDC_STATIC_13, m_HWinPanel13);
	DDX_Control(pDX, IDC_STATIC_14, m_HWinPanel14);
	DDX_Control(pDX, IDC_STATIC_15, m_HWinPanel15);
	DDX_Control(pDX, IDC_STATIC_16, m_HWinPanel16);
	DDX_Control(pDX, IDC_STATIC_17, m_HWinPanel17);
	DDX_Control(pDX, IDC_STATIC_18, m_HWinPanel18);
	DDX_Control(pDX, IDC_STATIC_19, m_HWinPanel19);
	DDX_Control(pDX, IDC_STATIC_20, m_HWinPanel20);
	DDX_Control(pDX, IDC_STATIC_21, m_HWinPanel21);
	DDX_Control(pDX, IDC_STATIC_22, m_HWinPanel22);
	DDX_Control(pDX, IDC_STATIC_23, m_HWinPanel23);
	DDX_Control(pDX, IDC_STATIC_24, m_HWinPanel24);
	DDX_Control(pDX, IDC_STATIC_25, m_HWinPanel25);
	DDX_Control(pDX, IDC_STATIC_26, m_HWinPanel26);
	DDX_Control(pDX, IDC_STATIC_27, m_HWinPanel27);
	DDX_Control(pDX, IDC_STATIC_28, m_HWinPanel28);
	DDX_Control(pDX, IDC_STATIC_29, m_HWinPanel29);
	DDX_Control(pDX, IDC_STATIC_30, m_HWinPanel30);
	DDX_Radio(pDX, IDC_RADIO_A, m_iRadioCharIndex);
	DDX_Control(pDX, IDC_LIST_FONT, m_FontListBox);
	DDX_Control(pDX, IDC_BUTTON_FONT_ADD, m_bnFontAdd);
	DDX_Control(pDX, IDC_BUTTON_FONT_DELETE, m_bnFontDelete);
	DDX_Control(pDX, IDC_BUTTON_FONT_NEW, m_bnFontNew);
	DDX_Control(pDX, IDC_BUTTON_FONT_SAVE, m_bnFontSave);
	DDX_Control(pDX, IDC_BUTTON_FONTIMAGE_ALLDELETE, m_bnFontImageAllDelete);
	DDX_Control(pDX, IDC_BUTTON_FONTIMAGE_DELETE, m_bnFontImageDelete);
	DDX_Control(pDX, IDC_BUTTON_FONTIMAGE_NEXT, m_bnFontImageNext);
	DDX_Control(pDX, IDC_BUTTON_FONTIMAGE_PREV, m_bnFontImagePrev);
	DDX_Control(pDX, IDC_STATIC_FONT_COUNT, m_stFontCount);
	DDX_Control(pDX, IDC_STATIC_FONT, m_stFontList);
	DDX_Text(pDX, IDC_EDIT_CHAR_PLATE_WIDTH, m_lEditCharPlateWidth);
	DDX_Text(pDX, IDC_EDIT_MIN_CHAR_WIDTH, m_lEditMinCharWidth);
	DDX_Text(pDX, IDC_EDIT_MAX_CHAR_WIDTH, m_lEditMaxCharWidth);
	DDX_Text(pDX, IDC_EDIT_MIN_CHAR_HEIGHT, m_lEditMinCharHeight);
	DDX_Text(pDX, IDC_EDIT_MAX_CHAR_HEIGHT, m_lEditMaxCharHeight);
	DDX_Text(pDX, IDC_EDIT_MIN_CHAR_CANDIDATE_HEIGHT, m_lEditMinCharCandidateHeight);
	DDX_Text(pDX, IDC_EDIT_WHOLE_AREA_DIVIDE, m_lEditWholeAreaDivide);
	DDX_Text(pDX, IDC_EDIT_HORIZONTAL_DIVIDE, m_lEditHorizontalDivide);
	DDX_Text(pDX, IDC_EDIT_FIRST_DIFFERENT, m_lEditFirstDiffernt);
	DDX_Text(pDX, IDC_EDIT_SECOND_DIFFERENT, m_lEditSecondDifferent);
	DDX_Text(pDX, IDC_EDIT_MAX_CHAR_DIFF, m_lEditdiff);
	DDX_Text(pDX, IDC_EDIT_DILATION_SEGMENT, m_iEditOCRSegmentDilation);
}


BEGIN_MESSAGE_MAP(CAlgorithmOCRDlg, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_A, IDC_RADIO_9, OnClickPageNumber)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST_FONT, &CAlgorithmOCRDlg::OnSelchangeListFont)
	ON_BN_CLICKED(IDC_BUTTON_FONTIMAGE_PREV, &CAlgorithmOCRDlg::OnBnClickedButtonFontimagePrev)
	ON_BN_CLICKED(IDC_BUTTON_FONTIMAGE_NEXT, &CAlgorithmOCRDlg::OnBnClickedButtonFontimageNext)
	ON_BN_CLICKED(IDC_BUTTON_FONTIMAGE_ALLDELETE, &CAlgorithmOCRDlg::OnBnClickedButtonFontimageAlldelete)
	ON_BN_CLICKED(IDC_BUTTON_FONT_NEW, &CAlgorithmOCRDlg::OnBnClickedButtonFontNew)
	ON_BN_CLICKED(IDC_BUTTON_FONT_ADD, &CAlgorithmOCRDlg::OnBnClickedButtonFontAdd)
	ON_BN_CLICKED(IDC_BUTTON_FONT_DELETE, &CAlgorithmOCRDlg::OnBnClickedButtonFontDelete)
	ON_BN_CLICKED(IDC_BUTTON_FONT_SAVE, &CAlgorithmOCRDlg::OnBnClickedButtonFontSave)
	ON_STN_CLICKED(IDC_STATIC_1, &CAlgorithmOCRDlg::OnStnClickedStatic1)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_2, &CAlgorithmOCRDlg::OnStnClickedStatic2)
	ON_STN_CLICKED(IDC_STATIC_3, &CAlgorithmOCRDlg::OnStnClickedStatic3)
	ON_STN_CLICKED(IDC_STATIC_4, &CAlgorithmOCRDlg::OnStnClickedStatic4)
	ON_STN_CLICKED(IDC_STATIC_5, &CAlgorithmOCRDlg::OnStnClickedStatic5)
	ON_STN_CLICKED(IDC_STATIC_6, &CAlgorithmOCRDlg::OnStnClickedStatic6)
	ON_STN_CLICKED(IDC_STATIC_7, &CAlgorithmOCRDlg::OnStnClickedStatic7)
	ON_STN_CLICKED(IDC_STATIC_8, &CAlgorithmOCRDlg::OnStnClickedStatic8)
	ON_STN_CLICKED(IDC_STATIC_9, &CAlgorithmOCRDlg::OnStnClickedStatic9)
	ON_STN_CLICKED(IDC_STATIC_10, &CAlgorithmOCRDlg::OnStnClickedStatic10)
	ON_STN_CLICKED(IDC_STATIC_11, &CAlgorithmOCRDlg::OnStnClickedStatic11)
	ON_STN_CLICKED(IDC_STATIC_12, &CAlgorithmOCRDlg::OnStnClickedStatic12)
	ON_STN_CLICKED(IDC_STATIC_13, &CAlgorithmOCRDlg::OnStnClickedStatic13)
	ON_STN_CLICKED(IDC_STATIC_14, &CAlgorithmOCRDlg::OnStnClickedStatic14)
	ON_STN_CLICKED(IDC_STATIC_15, &CAlgorithmOCRDlg::OnStnClickedStatic15)
	ON_STN_CLICKED(IDC_STATIC_16, &CAlgorithmOCRDlg::OnStnClickedStatic16)
	ON_STN_CLICKED(IDC_STATIC_17, &CAlgorithmOCRDlg::OnStnClickedStatic17)
	ON_STN_CLICKED(IDC_STATIC_18, &CAlgorithmOCRDlg::OnStnClickedStatic18)
	ON_STN_CLICKED(IDC_STATIC_19, &CAlgorithmOCRDlg::OnStnClickedStatic19)
	ON_STN_CLICKED(IDC_STATIC_20, &CAlgorithmOCRDlg::OnStnClickedStatic20)
	ON_STN_CLICKED(IDC_STATIC_21, &CAlgorithmOCRDlg::OnStnClickedStatic21)
	ON_STN_CLICKED(IDC_STATIC_22, &CAlgorithmOCRDlg::OnStnClickedStatic22)
	ON_STN_CLICKED(IDC_STATIC_23, &CAlgorithmOCRDlg::OnStnClickedStatic23)
	ON_STN_CLICKED(IDC_STATIC_24, &CAlgorithmOCRDlg::OnStnClickedStatic24)
	ON_STN_CLICKED(IDC_STATIC_25, &CAlgorithmOCRDlg::OnStnClickedStatic25)
	ON_STN_CLICKED(IDC_STATIC_26, &CAlgorithmOCRDlg::OnStnClickedStatic26)
	ON_STN_CLICKED(IDC_STATIC_27, &CAlgorithmOCRDlg::OnStnClickedStatic27)
	ON_STN_CLICKED(IDC_STATIC_28, &CAlgorithmOCRDlg::OnStnClickedStatic28)
	ON_STN_CLICKED(IDC_STATIC_29, &CAlgorithmOCRDlg::OnStnClickedStatic29)
	ON_STN_CLICKED(IDC_STATIC_30, &CAlgorithmOCRDlg::OnStnClickedStatic30)
	ON_BN_CLICKED(IDC_BUTTON_FONTIMAGE_DELETE, &CAlgorithmOCRDlg::OnBnClickedButtonFontimageDelete)
	ON_BN_CLICKED(IDC_BUTTON_FONT_NEW2, &CAlgorithmOCRDlg::OnBnClickedButtonFontNew2)
END_MESSAGE_MAP()


// CAlgorithmOCRDlg 메시지 처리기입니다.

void CAlgorithmOCRDlg::OnClickPageNumber(UINT nID)
{
	m_iRadioCharIndex = nID - IDC_RADIO_A;

	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
		mbSelectImageIndex[i] = false;

	miCurImageCount = 0;

	Invalidate(TRUE);
}
void CAlgorithmOCRDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex;
}

void CAlgorithmOCRDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseOCR = AlgorithmParam.m_bUseOCR;

	m_lEditCharPlateWidth = AlgorithmParam.m_lCharPlateWidth;
	m_lEditMinCharWidth = AlgorithmParam.m_lMinCharWidth;
	m_lEditMaxCharWidth = AlgorithmParam.m_lMaxCharWidth;
	m_lEditMinCharHeight = AlgorithmParam.m_lMinCharHeight;
	m_lEditMaxCharHeight = AlgorithmParam.m_lMaxCharHeight;
	m_lEditMinCharCandidateHeight = AlgorithmParam.m_lMinCharCandidateHeight;
	m_lEditWholeAreaDivide = AlgorithmParam.m_lWholeAreaDivide;
	m_lEditHorizontalDivide = AlgorithmParam.m_lHorizontalDivide;
	m_lEditFirstDiffernt = AlgorithmParam.m_lFirstDiffernt;
	m_lEditSecondDifferent = AlgorithmParam.m_lSecondDifferent;
	m_lEditdiff = AlgorithmParam.m_ldiff;
	m_iEditOCRSegmentDilation = AlgorithmParam.m_iOCRSegmentDilation;

	UpdateData(FALSE);
}

void CAlgorithmOCRDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseOCR = m_bCheckUseOCR;

	pAlgorithmParam->m_lCharPlateWidth = m_lEditCharPlateWidth;
	pAlgorithmParam->m_lMinCharWidth = m_lEditMinCharWidth;
	pAlgorithmParam->m_lMaxCharWidth = m_lEditMaxCharWidth;
	pAlgorithmParam->m_lMinCharHeight = m_lEditMinCharHeight;
	pAlgorithmParam->m_lMaxCharHeight = m_lEditMaxCharHeight;
	pAlgorithmParam->m_lMinCharCandidateHeight = m_lEditMinCharCandidateHeight;
	pAlgorithmParam->m_lWholeAreaDivide = m_lEditWholeAreaDivide;
	pAlgorithmParam->m_lHorizontalDivide = m_lEditHorizontalDivide;
	pAlgorithmParam->m_lFirstDiffernt = m_lEditFirstDiffernt;
	pAlgorithmParam->m_lSecondDifferent = m_lEditSecondDifferent;
	pAlgorithmParam->m_ldiff = m_lEditdiff;
	pAlgorithmParam->m_iOCRSegmentDilation = m_iEditOCRSegmentDilation;
}

BOOL CAlgorithmOCRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_bnFontAdd.Set_Text(_T("폰트 파일에 추가"));
	m_bnFontAdd.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontDelete.Set_Text(_T("폰트 파일 삭제"));
	m_bnFontDelete.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontNew.Set_Text(_T("폰트 파일 새로 생성"));
	m_bnFontNew.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontSave.Set_Text(_T("저장"));
	m_bnFontSave.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontImagePrev.Set_Text(_T("이전 이미지"));
	m_bnFontImagePrev.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontImageNext.Set_Text(_T("다음 이미지"));
	m_bnFontImageNext.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontImageAllDelete.Set_Text(_T("전체 삭제"));
	m_bnFontImageAllDelete.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	//m_bnFontImageDelete.Set_Text(_T("선택 삭제"));
	m_bnFontImageDelete.Init_Ctrl(_T("Arial"), 9, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);

	RECT Rect;
	m_HWinPanel1.CreateHWindow();
	m_HWinPanel1.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel1.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel2.CreateHWindow();
	m_HWinPanel2.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel2.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel3.CreateHWindow();
	m_HWinPanel3.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel3.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel4.CreateHWindow();
	m_HWinPanel4.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel4.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel5.CreateHWindow();
	m_HWinPanel5.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel5.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel6.CreateHWindow();
	m_HWinPanel6.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel6.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel7.CreateHWindow();
	m_HWinPanel7.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel7.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel8.CreateHWindow();
	m_HWinPanel8.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel8.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel9.CreateHWindow();
	m_HWinPanel9.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel9.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel10.CreateHWindow();
	m_HWinPanel10.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel10.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel11.CreateHWindow();
	m_HWinPanel11.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel11.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel12.CreateHWindow();
	m_HWinPanel12.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel12.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel13.CreateHWindow();
	m_HWinPanel13.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel13.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel14.CreateHWindow();
	m_HWinPanel14.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel14.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel15.CreateHWindow();
	m_HWinPanel15.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel15.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel16.CreateHWindow();
	m_HWinPanel16.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel16.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel17.CreateHWindow();
	m_HWinPanel17.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel17.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel18.CreateHWindow();
	m_HWinPanel18.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel18.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel19.CreateHWindow();
	m_HWinPanel19.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel19.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel20.CreateHWindow();
	m_HWinPanel20.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel20.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel21.CreateHWindow();
	m_HWinPanel21.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel21.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel22.CreateHWindow();
	m_HWinPanel22.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel22.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel23.CreateHWindow();
	m_HWinPanel23.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel23.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel24.CreateHWindow();
	m_HWinPanel24.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel24.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel25.CreateHWindow();
	m_HWinPanel25.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel25.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel26.CreateHWindow();
	m_HWinPanel26.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel26.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel27.CreateHWindow();
	m_HWinPanel27.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel27.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel28.CreateHWindow();
	m_HWinPanel28.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel28.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel29.CreateHWindow();
	m_HWinPanel29.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel29.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_HWinPanel30.CreateHWindow();
	m_HWinPanel30.GetClientRect(&Rect);
	SetWindowExtents(m_HWinPanel30.mlWindowHandle, 0, 0, Rect.right, Rect.bottom);

	m_iRadioCharIndex = 0;
	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
	{
		mbSelectImageIndex[i] = false;
		mbImageValid[i] = false;
	}

	miCurImageCount = 0;

	mp_HCharImage_A = NULL;
	mp_HCharImage_B = NULL;
	mp_HCharImage_C = NULL;
	mp_HCharImage_D = NULL;
	mp_HCharImage_E = NULL;
	mp_HCharImage_F = NULL;
	mp_HCharImage_G = NULL;
	mp_HCharImage_H = NULL;
	mp_HCharImage_I = NULL;
	mp_HCharImage_J = NULL;
	mp_HCharImage_K = NULL;
	mp_HCharImage_L = NULL;
	mp_HCharImage_M = NULL;
	mp_HCharImage_N = NULL;
	mp_HCharImage_O = NULL;
	mp_HCharImage_P = NULL;
	mp_HCharImage_Q = NULL;
	mp_HCharImage_R = NULL;
	mp_HCharImage_S = NULL;
	mp_HCharImage_T = NULL;
	mp_HCharImage_U = NULL;
	mp_HCharImage_V = NULL;
	mp_HCharImage_W = NULL;
	mp_HCharImage_X = NULL;
	mp_HCharImage_Y = NULL;
	mp_HCharImage_Z = NULL;
	mp_HCharImage_0 = NULL;
	mp_HCharImage_1 = NULL;
	mp_HCharImage_2 = NULL;
	mp_HCharImage_3 = NULL;
	mp_HCharImage_4 = NULL;
	mp_HCharImage_5 = NULL;
	mp_HCharImage_6 = NULL;
	mp_HCharImage_7 = NULL;
	mp_HCharImage_8 = NULL;
	mp_HCharImage_9 = NULL;

	Reset();

	UpdateFontList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlgorithmOCRDlg::UpdateFontList()
{
	HANDLE hFile;
	WIN32_FIND_DATA FindData;

	int iCount;
	CString Text;

	CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font";
	SetCurrentDirectory(strFontFolder);

	m_FontListBox.ResetContent();
	hFile = FindFirstFile("*.trf", &FindData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		Text = FindData.cFileName;

		iCount = Text.GetLength();
		Text.Delete(iCount - 4, 4);

		m_FontListBox.AddString(Text);

		while (FindNextFile(hFile, &FindData))
		{
			Text = FindData.cFileName;

			iCount = Text.GetLength();
			Text.Delete(iCount - 4, 4);

			m_FontListBox.AddString(Text);
		}
	}

	FindClose(hFile);
}

void CAlgorithmOCRDlg::Reset()
{
	miCount_A = 0;
	miCount_B = 0;
	miCount_C = 0;
	miCount_D = 0;
	miCount_E = 0;
	miCount_F = 0;
	miCount_G = 0;
	miCount_H = 0;
	miCount_I = 0;
	miCount_J = 0;
	miCount_K = 0;
	miCount_L = 0;
	miCount_M = 0;
	miCount_N = 0;
	miCount_O = 0;
	miCount_P = 0;
	miCount_Q = 0;
	miCount_R = 0;
	miCount_S = 0;
	miCount_T = 0;
	miCount_U = 0;
	miCount_V = 0;
	miCount_W = 0;
	miCount_X = 0;
	miCount_Y = 0;
	miCount_Z = 0;
	miCount_0 = 0;
	miCount_1 = 0;
	miCount_2 = 0;
	miCount_3 = 0;
	miCount_4 = 0;
	miCount_5 = 0;
	miCount_6 = 0;
	miCount_7 = 0;
	miCount_8 = 0;
	miCount_9 = 0;

	if (mp_HCharImage_A)
	{
		delete[] mp_HCharImage_A;
		mp_HCharImage_A = NULL;
	}

	if (mp_HCharImage_B)
	{
		delete[] mp_HCharImage_B;
		mp_HCharImage_B = NULL;
	}

	if (mp_HCharImage_C)
	{
		delete[] mp_HCharImage_C;
		mp_HCharImage_C = NULL;
	}
	if (mp_HCharImage_D)
	{
		delete[] mp_HCharImage_D;
		mp_HCharImage_D = NULL;
	}
	if (mp_HCharImage_E)
	{
		delete[] mp_HCharImage_E;
		mp_HCharImage_E = NULL;
	}
	if (mp_HCharImage_F)
	{
		delete[] mp_HCharImage_F;
		mp_HCharImage_F = NULL;
	}
	if (mp_HCharImage_G)
	{
		delete[] mp_HCharImage_G;
		mp_HCharImage_G = NULL;
	}
	if (mp_HCharImage_H)
	{
		delete[] mp_HCharImage_H;
		mp_HCharImage_H = NULL;
	}
	if (mp_HCharImage_I)
	{
		delete[] mp_HCharImage_I;
		mp_HCharImage_I = NULL;
	}
	if (mp_HCharImage_J)
	{
		delete[] mp_HCharImage_J;
		mp_HCharImage_J = NULL;
	}
	if (mp_HCharImage_K)
	{
		delete[] mp_HCharImage_K;
		mp_HCharImage_K = NULL;
	}
	if (mp_HCharImage_L)
	{
		delete[] mp_HCharImage_L;
		mp_HCharImage_L = NULL;
	}
	if (mp_HCharImage_M)
	{
		delete[] mp_HCharImage_M;
		mp_HCharImage_M = NULL;
	}
	if (mp_HCharImage_N)
	{
		delete[] mp_HCharImage_N;
		mp_HCharImage_N = NULL;
	}
	if (mp_HCharImage_O)
	{
		delete[] mp_HCharImage_O;
		mp_HCharImage_O = NULL;
	}
	if (mp_HCharImage_P)
	{
		delete[] mp_HCharImage_P;
		mp_HCharImage_P = NULL;
	}
	if (mp_HCharImage_Q)
	{
		delete[] mp_HCharImage_Q;
		mp_HCharImage_Q = NULL;
	}
	if (mp_HCharImage_R)
	{
		delete[] mp_HCharImage_R;
		mp_HCharImage_R = NULL;
	}
	if (mp_HCharImage_S)
	{
		delete[] mp_HCharImage_S;
		mp_HCharImage_S = NULL;
	}
	if (mp_HCharImage_T)
	{
		delete[] mp_HCharImage_T;
		mp_HCharImage_T = NULL;
	}
	if (mp_HCharImage_U)
	{
		delete[] mp_HCharImage_U;
		mp_HCharImage_U = NULL;
	}
	if (mp_HCharImage_V)
	{
		delete[] mp_HCharImage_V;
		mp_HCharImage_V = NULL;
	}
	if (mp_HCharImage_W)
	{
		delete[] mp_HCharImage_W;
		mp_HCharImage_W = NULL;
	}
	if (mp_HCharImage_X)
	{
		delete[] mp_HCharImage_X;
		mp_HCharImage_X = NULL;
	}
	if (mp_HCharImage_Y)
	{
		delete[] mp_HCharImage_Y;
		mp_HCharImage_Y = NULL;
	}
	if (mp_HCharImage_Z)
	{
		delete[] mp_HCharImage_Z;
		mp_HCharImage_Z = NULL;
	}
	if (mp_HCharImage_0)
	{
		delete[] mp_HCharImage_0;
		mp_HCharImage_0 = NULL;
	}
	if (mp_HCharImage_1)
	{
		delete[] mp_HCharImage_1;
		mp_HCharImage_1 = NULL;
	}
	if (mp_HCharImage_2)
	{
		delete[] mp_HCharImage_2;
		mp_HCharImage_2 = NULL;
	}
	if (mp_HCharImage_3)
	{
		delete[] mp_HCharImage_3;
		mp_HCharImage_3 = NULL;
	}
	if (mp_HCharImage_4)
	{
		delete[] mp_HCharImage_4;
		mp_HCharImage_4 = NULL;
	}
	if (mp_HCharImage_5)
	{
		delete[] mp_HCharImage_5;
		mp_HCharImage_5 = NULL;
	}
	if (mp_HCharImage_6)
	{
		delete[] mp_HCharImage_6;
		mp_HCharImage_6 = NULL;
	}
	if (mp_HCharImage_7)
	{
		delete[] mp_HCharImage_7;
		mp_HCharImage_7 = NULL;
	}
	if (mp_HCharImage_8)
	{
		delete[] mp_HCharImage_8;
		mp_HCharImage_8 = NULL;
	}
	if (mp_HCharImage_9)
	{
		delete[] mp_HCharImage_9;
		mp_HCharImage_9 = NULL;
	}

	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
	{
		mbSelectImageIndex[i] = false;
		mbImageValid[i] = false;
	}

	miCurImageCount = 0;

	m_iRadioCharIndex = 0;

	UpdateData(FALSE);
}

void CAlgorithmOCRDlg::LoadFontFile(CString FontPathName)
{
	HTuple HFontChar;
	HObject HFontImage;

	ReadOcrTrainf(&HFontImage, HTuple(FontPathName), &HFontChar);

	Hlong FontCharCount = HFontChar.Length();

	Hlong FontImageCount;
	HTuple HFontImageCount;
	CountObj(HFontImage, &HFontImageCount);
	FontImageCount = HFontImageCount.L();

	if (FontCharCount == FontImageCount)
	{
		Reset();

		for (int i = 0; i < FontCharCount; i++)
		{
			CString FontChar = HFontChar[i].S();
			if (FontChar == "A")
				miCount_A++;
			else if (FontChar == "B")
				miCount_B++;
			else if (FontChar == "C")
				miCount_C++;
			else if (FontChar == "D")
				miCount_D++;
			else if (FontChar == "E")
				miCount_E++;
			else if (FontChar == "F")
				miCount_F++;
			else if (FontChar == "G")
				miCount_G++;
			else if (FontChar == "H")
				miCount_H++;
			else if (FontChar == "I")
				miCount_I++;
			else if (FontChar == "J")
				miCount_J++;
			else if (FontChar == "K")
				miCount_K++;
			else if (FontChar == "L")
				miCount_L++;
			else if (FontChar == "M")
				miCount_M++;
			else if (FontChar == "N")
				miCount_N++;
			else if (FontChar == "O")
				miCount_O++;
			else if (FontChar == "P")
				miCount_P++;
			else if (FontChar == "Q")
				miCount_Q++;
			else if (FontChar == "R")
				miCount_R++;
			else if (FontChar == "S")
				miCount_S++;
			else if (FontChar == "T")
				miCount_T++;
			else if (FontChar == "U")
				miCount_U++;
			else if (FontChar == "V")
				miCount_V++;
			else if (FontChar == "W")
				miCount_W++;
			else if (FontChar == "X")
				miCount_X++;
			else if (FontChar == "Y")
				miCount_Y++;
			else if (FontChar == "Z")
				miCount_Z++;
			else if (FontChar == "0")
				miCount_0++;
			else if (FontChar == "1")
				miCount_1++;
			else if (FontChar == "2")
				miCount_2++;
			else if (FontChar == "3")
				miCount_3++;
			else if (FontChar == "4")
				miCount_4++;
			else if (FontChar == "5")
				miCount_5++;
			else if (FontChar == "6")
				miCount_6++;
			else if (FontChar == "7")
				miCount_7++;
			else if (FontChar == "8")
				miCount_8++;
			else if (FontChar == "9")
				miCount_9++;
		}

		if (miCount_A > 0)
			mp_HCharImage_A = new HObject[miCount_A];

		if (miCount_B > 0)
			mp_HCharImage_B = new HObject[miCount_B];

		if (miCount_C > 0)
			mp_HCharImage_C = new HObject[miCount_C];

		if (miCount_D > 0)
			mp_HCharImage_D = new HObject[miCount_D];

		if (miCount_E > 0)
			mp_HCharImage_E = new HObject[miCount_E];

		if (miCount_F > 0)
			mp_HCharImage_F = new HObject[miCount_F];

		if (miCount_G > 0)
			mp_HCharImage_G = new HObject[miCount_G];

		if (miCount_H > 0)
			mp_HCharImage_H = new HObject[miCount_H];

		if (miCount_I > 0)
			mp_HCharImage_I = new HObject[miCount_I];

		if (miCount_J > 0)
			mp_HCharImage_J = new HObject[miCount_J];

		if (miCount_K > 0)
			mp_HCharImage_K = new HObject[miCount_K];

		if (miCount_L > 0)
			mp_HCharImage_L = new HObject[miCount_L];

		if (miCount_M > 0)
			mp_HCharImage_M = new HObject[miCount_M];

		if (miCount_N > 0)
			mp_HCharImage_N = new HObject[miCount_N];

		if (miCount_O > 0)
			mp_HCharImage_O = new HObject[miCount_O];

		if (miCount_P > 0)
			mp_HCharImage_P = new HObject[miCount_P];

		if (miCount_Q > 0)
			mp_HCharImage_Q = new HObject[miCount_Q];

		if (miCount_R > 0)
			mp_HCharImage_R = new HObject[miCount_R];

		if (miCount_S > 0)
			mp_HCharImage_S = new HObject[miCount_S];

		if (miCount_T > 0)
			mp_HCharImage_T = new HObject[miCount_T];

		if (miCount_U > 0)
			mp_HCharImage_U = new HObject[miCount_U];

		if (miCount_V > 0)
			mp_HCharImage_V = new HObject[miCount_V];

		if (miCount_W > 0)
			mp_HCharImage_W = new HObject[miCount_W];

		if (miCount_X > 0)
			mp_HCharImage_X = new HObject[miCount_X];

		if (miCount_Y > 0)
			mp_HCharImage_Y = new HObject[miCount_Y];

		if (miCount_Z > 0)
			mp_HCharImage_Z = new HObject[miCount_Z];

		if (miCount_0 > 0)
			mp_HCharImage_0 = new HObject[miCount_0];

		if (miCount_1 > 0)
			mp_HCharImage_1 = new HObject[miCount_1];

		if (miCount_2 > 0)
			mp_HCharImage_2 = new HObject[miCount_2];

		if (miCount_3 > 0)
			mp_HCharImage_3 = new HObject[miCount_3];

		if (miCount_4 > 0)
			mp_HCharImage_4 = new HObject[miCount_4];

		if (miCount_5 > 0)
			mp_HCharImage_5 = new HObject[miCount_5];

		if (miCount_6 > 0)
			mp_HCharImage_6 = new HObject[miCount_6];

		if (miCount_7 > 0)
			mp_HCharImage_7 = new HObject[miCount_7];

		if (miCount_8 > 0)
			mp_HCharImage_8 = new HObject[miCount_8];

		if (miCount_9 > 0)
			mp_HCharImage_9 = new HObject[miCount_9];

		int iCount_A = 0;
		int iCount_B = 0;
		int iCount_C = 0;
		int iCount_D = 0;
		int iCount_E = 0;
		int iCount_F = 0;
		int iCount_G = 0;
		int iCount_H = 0;
		int iCount_I = 0;
		int iCount_J = 0;
		int iCount_K = 0;
		int iCount_L = 0;
		int iCount_M = 0;
		int iCount_N = 0;
		int iCount_O = 0;
		int iCount_P = 0;
		int iCount_Q = 0;
		int iCount_R = 0;
		int iCount_S = 0;
		int iCount_T = 0;
		int iCount_U = 0;
		int iCount_V = 0;
		int iCount_W = 0;
		int iCount_X = 0;
		int iCount_Y = 0;
		int iCount_Z = 0;
		int iCount_0 = 0;
		int iCount_1 = 0;
		int iCount_2 = 0;
		int iCount_3 = 0;
		int iCount_4 = 0;
		int iCount_5 = 0;
		int iCount_6 = 0;
		int iCount_7 = 0;
		int iCount_8 = 0;
		int iCount_9 = 0;

		HObject HSelImage;
		for (int i = 0; i < FontImageCount; i++)
		{
			SelectObj(HFontImage, &HSelImage, i + 1);

			CString FontChar = HFontChar[i].S();
			if (FontChar == "A")
			{
				mp_HCharImage_A[iCount_A] = HSelImage;
				iCount_A++;
			}
			else if (FontChar == "B")
			{
				mp_HCharImage_B[iCount_B] = HSelImage;
				iCount_B++;
			}
			else if (FontChar == "C")
			{
				mp_HCharImage_C[iCount_C] = HSelImage;
				iCount_C++;
			}
			else if (FontChar == "D")
			{
				mp_HCharImage_D[iCount_D] = HSelImage;
				iCount_D++;
			}
			else if (FontChar == "E")
			{
				mp_HCharImage_E[iCount_E] = HSelImage;
				iCount_E++;
			}
			else if (FontChar == "F")
			{
				mp_HCharImage_F[iCount_F] = HSelImage;
				iCount_F++;
			}
			else if (FontChar == "G")
			{
				mp_HCharImage_G[iCount_G] = HSelImage;
				iCount_G++;
			}
			else if (FontChar == "H")
			{
				mp_HCharImage_H[iCount_H] = HSelImage;
				iCount_H++;
			}
			else if (FontChar == "I")
			{
				mp_HCharImage_I[iCount_I] = HSelImage;
				iCount_I++;
			}
			else if (FontChar == "J")
			{
				mp_HCharImage_J[iCount_J] = HSelImage;
				iCount_J++;
			}
			else if (FontChar == "K")
			{
				mp_HCharImage_K[iCount_K] = HSelImage;
				iCount_K++;
			}
			else if (FontChar == "L")
			{
				mp_HCharImage_L[iCount_L] = HSelImage;
				iCount_L++;
			}
			else if (FontChar == "M")
			{
				mp_HCharImage_M[iCount_M] = HSelImage;
				iCount_M++;
			}
			else if (FontChar == "N")
			{
				mp_HCharImage_N[iCount_N] = HSelImage;
				iCount_N++;
			}
			else if (FontChar == "O")
			{
				mp_HCharImage_O[iCount_O] = HSelImage;
				iCount_O++;
			}
			else if (FontChar == "P")
			{
				mp_HCharImage_P[iCount_P] = HSelImage;
				iCount_P++;
			}
			else if (FontChar == "Q")
			{
				mp_HCharImage_Q[iCount_Q] = HSelImage;
				iCount_Q++;
			}
			else if (FontChar == "R")
			{
				mp_HCharImage_R[iCount_R] = HSelImage;
				iCount_R++;
			}
			else if (FontChar == "S")
			{
				mp_HCharImage_S[iCount_S] = HSelImage;
				iCount_S++;
			}
			else if (FontChar == "T")
			{
				mp_HCharImage_T[iCount_T] = HSelImage;
				iCount_T++;
			}
			else if (FontChar == "U")
			{
				mp_HCharImage_U[iCount_U] = HSelImage;
				iCount_U++;
			}
			else if (FontChar == "V")
			{
				mp_HCharImage_V[iCount_V] = HSelImage;
				iCount_V++;
			}
			else if (FontChar == "W")
			{
				mp_HCharImage_W[iCount_W] = HSelImage;
				iCount_W++;
			}
			else if (FontChar == "X")
			{
				mp_HCharImage_X[iCount_X] = HSelImage;
				iCount_X++;
			}
			else if (FontChar == "Y")
			{
				mp_HCharImage_Y[iCount_Y] = HSelImage;
				iCount_Y++;
			}
			else if (FontChar == "Z")
			{
				mp_HCharImage_Z[iCount_Z] = HSelImage;
				iCount_Z++;
			}
			else if (FontChar == "0")
			{
				mp_HCharImage_0[iCount_0] = HSelImage;
				iCount_0++;
			}
			else if (FontChar == "1")
			{
				mp_HCharImage_1[iCount_1] = HSelImage;
				iCount_1++;
			}
			else if (FontChar == "2")
			{
				mp_HCharImage_2[iCount_2] = HSelImage;
				iCount_2++;
			}
			else if (FontChar == "3")
			{
				mp_HCharImage_3[iCount_3] = HSelImage;
				iCount_3++;
			}
			else if (FontChar == "4")
			{
				mp_HCharImage_4[iCount_4] = HSelImage;
				iCount_4++;
			}
			else if (FontChar == "5")
			{
				mp_HCharImage_5[iCount_5] = HSelImage;
				iCount_5++;
			}
			else if (FontChar == "6")
			{
				mp_HCharImage_6[iCount_6] = HSelImage;
				iCount_6++;
			}
			else if (FontChar == "7")
			{
				mp_HCharImage_7[iCount_7] = HSelImage;
				iCount_7++;
			}
			else if (FontChar == "8")
			{
				mp_HCharImage_8[iCount_8] = HSelImage;
				iCount_8++;
			}
			else if (FontChar == "9")
			{
				mp_HCharImage_9[iCount_9] = HSelImage;
				iCount_9++;
			}
		}

		Invalidate(FALSE);
	}
}

void CAlgorithmOCRDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.


	HObject* pHImage = NULL;
	int iCount = 0;

	switch (m_iRadioCharIndex)
	{
	case 0:
		pHImage = mp_HCharImage_A;
		iCount = miCount_A;
		break;

	case 1:
		pHImage = mp_HCharImage_B;
		iCount = miCount_B;
		break;

	case 2:
		pHImage = mp_HCharImage_C;
		iCount = miCount_C;
		break;

	case 3:
		pHImage = mp_HCharImage_D;
		iCount = miCount_D;
		break;

	case 4:
		pHImage = mp_HCharImage_E;
		iCount = miCount_E;
		break;

	case 5:
		pHImage = mp_HCharImage_F;
		iCount = miCount_F;
		break;

	case 6:
		pHImage = mp_HCharImage_G;
		iCount = miCount_G;
		break;

	case 7:
		pHImage = mp_HCharImage_H;
		iCount = miCount_H;
		break;

	case 8:
		pHImage = mp_HCharImage_I;
		iCount = miCount_I;
		break;

	case 9:
		pHImage = mp_HCharImage_J;
		iCount = miCount_J;
		break;
	case 10:
		pHImage = mp_HCharImage_K;
		iCount = miCount_K;
		break;

	case 11:
		pHImage = mp_HCharImage_L;
		iCount = miCount_L;
		break;
	case 12:
		pHImage = mp_HCharImage_M;
		iCount = miCount_M;
		break;

	case 13:
		pHImage = mp_HCharImage_N;
		iCount = miCount_N;
		break;

	case 14:
		pHImage = mp_HCharImage_O;
		iCount = miCount_O;
		break;
	case 15:
		pHImage = mp_HCharImage_P;
		iCount = miCount_P;
		break;
	case 16:
		pHImage = mp_HCharImage_Q;
		iCount = miCount_Q;
		break;

	case 17:
		pHImage = mp_HCharImage_R;
		iCount = miCount_R;
		break;
	case 18:
		pHImage = mp_HCharImage_S;
		iCount = miCount_S;
		break;

	case 19:
		pHImage = mp_HCharImage_T;
		iCount = miCount_T;
		break;

	case 20:
		pHImage = mp_HCharImage_U;
		iCount = miCount_U;
		break;

	case 21:
		pHImage = mp_HCharImage_V;
		iCount = miCount_V;
		break;

	case 22:
		pHImage = mp_HCharImage_W;
		iCount = miCount_W;
		break;

	case 23:
		pHImage = mp_HCharImage_X;
		iCount = miCount_X;
		break;

	case 24:
		pHImage = mp_HCharImage_Y;
		iCount = miCount_Y;
		break;

	case 25:
		pHImage = mp_HCharImage_Z;
		iCount = miCount_Z;
		break;

	case 26:
		pHImage = mp_HCharImage_0;
		iCount = miCount_0;
		break;
	case 27:
		pHImage = mp_HCharImage_1;
		iCount = miCount_1;
		break;

	case 28:
		pHImage = mp_HCharImage_2;
		iCount = miCount_2;
		break;

	case 29:
		pHImage = mp_HCharImage_3;
		iCount = miCount_3;
		break;

	case 30:
		pHImage = mp_HCharImage_4;
		iCount = miCount_4;
		break;

	case 31:
		pHImage = mp_HCharImage_5;
		iCount = miCount_5;
		break;

	case 32:
		pHImage = mp_HCharImage_6;
		iCount = miCount_6;
		break;

	case 33:
		pHImage = mp_HCharImage_7;
		iCount = miCount_7;
		break;

	case 34:
		pHImage = mp_HCharImage_8;
		iCount = miCount_8;
		break;

	case 35:
		pHImage = mp_HCharImage_9;
		iCount = miCount_9;
		break;
	}

	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
		mbImageValid[i] = false;

	BYTE *pImageData;
	Hlong lWidth, lHeight;
	HTuple HlPointer, HlWidth, HlHeight, HcaType;
	char caType[256];

	int iTotalCount;

	if (iCount > 0)
	{
		if (miCurImageCount <= 0)
		{
			iTotalCount = iCount;
		}
		else
		{
			if (iCount > miCurImageCount + FONT_IAMGE_COUNT)
			{
				iTotalCount = miCurImageCount + FONT_IAMGE_COUNT;
			}
			else
			{
				iTotalCount = iCount;
			}
		}

		HObject HRectRgn;
		for (int i = miCurImageCount; i < iTotalCount; i++)
		{
			if (THEAPP.m_pGFunction->ValidHImage(pHImage[i]))
			{
				GetImagePointer1(pHImage[i], &HlPointer, &HcaType, &HlWidth, &HlHeight);

				pImageData = (BYTE *)HlPointer.L();
				lWidth = HlWidth.L();
				lHeight = HlHeight.L();

				if (i - miCurImageCount == 0)
				{
					SetPart(m_HWinPanel1.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[0])
						SetColor(m_HWinPanel1.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel1.mlWindowHandle, "green");

					SetDraw(m_HWinPanel1.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel1.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel1.mlWindowHandle);

					mbImageValid[0] = true;

				}
				else if (i - miCurImageCount == 1)
				{
					SetPart(m_HWinPanel2.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[1])
						SetColor(m_HWinPanel2.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel2.mlWindowHandle, "green");

					SetDraw(m_HWinPanel2.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel2.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel2.mlWindowHandle);

					mbImageValid[1] = true;
				}
				else if (i - miCurImageCount == 2)
				{
					SetPart(m_HWinPanel3.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[2])
						SetColor(m_HWinPanel3.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel3.mlWindowHandle, "green");

					SetDraw(m_HWinPanel3.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel3.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel3.mlWindowHandle);

					mbImageValid[2] = true;
				}
				else if (i - miCurImageCount == 3)
				{
					SetPart(m_HWinPanel4.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[3])
						SetColor(m_HWinPanel4.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel4.mlWindowHandle, "green");

					SetDraw(m_HWinPanel4.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel4.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel4.mlWindowHandle);

					mbImageValid[3] = true;
				}
				else if (i - miCurImageCount == 4)
				{
					SetPart(m_HWinPanel5.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[4])
						SetColor(m_HWinPanel5.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel5.mlWindowHandle, "green");

					SetDraw(m_HWinPanel5.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel5.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel5.mlWindowHandle);

					mbImageValid[4] = true;
				}
				else if (i - miCurImageCount == 5)
				{
					SetPart(m_HWinPanel6.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[5])
						SetColor(m_HWinPanel6.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel6.mlWindowHandle, "green");

					SetDraw(m_HWinPanel6.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel6.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel6.mlWindowHandle);

					mbImageValid[5] = true;
				}
				else if (i - miCurImageCount == 6)
				{
					SetPart(m_HWinPanel7.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[6])
						SetColor(m_HWinPanel7.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel7.mlWindowHandle, "green");

					SetDraw(m_HWinPanel7.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel7.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel7.mlWindowHandle);

					mbImageValid[6] = true;
				}
				else if (i - miCurImageCount == 7)
				{
					SetPart(m_HWinPanel8.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[7])
						SetColor(m_HWinPanel8.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel8.mlWindowHandle, "green");

					SetDraw(m_HWinPanel8.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel8.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel8.mlWindowHandle);

					mbImageValid[7] = true;
				}
				else if (i - miCurImageCount == 8)
				{
					SetPart(m_HWinPanel9.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[8])
						SetColor(m_HWinPanel9.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel9.mlWindowHandle, "green");

					SetDraw(m_HWinPanel9.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel9.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel9.mlWindowHandle);

					mbImageValid[8] = true;
				}
				else if (i - miCurImageCount == 9)
				{
					SetPart(m_HWinPanel10.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[9])
						SetColor(m_HWinPanel10.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel10.mlWindowHandle, "green");

					SetDraw(m_HWinPanel10.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel10.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel10.mlWindowHandle);

					mbImageValid[9] = true;
				}
				else if (i - miCurImageCount == 10)
				{
					SetPart(m_HWinPanel11.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[10])
						SetColor(m_HWinPanel11.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel11.mlWindowHandle, "green");

					SetDraw(m_HWinPanel11.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel11.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel11.mlWindowHandle);

					mbImageValid[10] = true;
				}
				else if (i - miCurImageCount == 11)
				{
					SetPart(m_HWinPanel12.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[11])
						SetColor(m_HWinPanel12.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel12.mlWindowHandle, "green");
					SetDraw(m_HWinPanel12.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel12.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel12.mlWindowHandle);

					mbImageValid[11] = true;
				}
				else if (i - miCurImageCount == 12)
				{
					SetPart(m_HWinPanel13.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[12])
						SetColor(m_HWinPanel13.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel13.mlWindowHandle, "green");
					SetDraw(m_HWinPanel13.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel13.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel13.mlWindowHandle);

					mbImageValid[12] = true;
				}
				else if (i - miCurImageCount == 13)
				{
					SetPart(m_HWinPanel14.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[13])
						SetColor(m_HWinPanel14.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel14.mlWindowHandle, "green");
					SetDraw(m_HWinPanel14.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel14.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel14.mlWindowHandle);

					mbImageValid[13] = true;
				}
				else if (i - miCurImageCount == 14)
				{
					SetPart(m_HWinPanel15.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[14])
						SetColor(m_HWinPanel15.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel15.mlWindowHandle, "green");
					SetDraw(m_HWinPanel15.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel15.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel15.mlWindowHandle);

					mbImageValid[14] = true;
				}
				else if (i - miCurImageCount == 15)
				{
					SetPart(m_HWinPanel16.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[15])
						SetColor(m_HWinPanel16.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel16.mlWindowHandle, "green");

					SetDraw(m_HWinPanel16.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel16.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel16.mlWindowHandle);

					mbImageValid[15] = true;
				}
				else if (i - miCurImageCount == 16)
				{
					SetPart(m_HWinPanel17.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[16])
						SetColor(m_HWinPanel17.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel17.mlWindowHandle, "green");

					SetDraw(m_HWinPanel17.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel17.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel17.mlWindowHandle);

					mbImageValid[16] = true;
				}
				else if (i - miCurImageCount == 17)
				{
					SetPart(m_HWinPanel18.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);
					if (mbSelectImageIndex[17])
						SetColor(m_HWinPanel18.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel18.mlWindowHandle, "green");

					SetDraw(m_HWinPanel18.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel18.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel18.mlWindowHandle);

					mbImageValid[17] = true;
				}
				else if (i - miCurImageCount == 18)
				{
					SetPart(m_HWinPanel19.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[18])
						SetColor(m_HWinPanel19.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel19.mlWindowHandle, "green");

					SetDraw(m_HWinPanel19.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel19.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel19.mlWindowHandle);

					mbImageValid[18] = true;
				}
				else if (i - miCurImageCount == 19)
				{
					SetPart(m_HWinPanel20.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[19])
						SetColor(m_HWinPanel20.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel20.mlWindowHandle, "green");

					SetDraw(m_HWinPanel20.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel20.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel20.mlWindowHandle);

					mbImageValid[19] = true;
				}
				else if (i - miCurImageCount == 20)
				{
					SetPart(m_HWinPanel21.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[20])
						SetColor(m_HWinPanel21.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel21.mlWindowHandle, "green");

					SetDraw(m_HWinPanel21.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel21.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel21.mlWindowHandle);

					mbImageValid[20] = true;
				}
				else if (i - miCurImageCount == 21)
				{
					SetPart(m_HWinPanel22.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[21])
						SetColor(m_HWinPanel22.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel22.mlWindowHandle, "green");

					SetDraw(m_HWinPanel22.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel22.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel22.mlWindowHandle);

					mbImageValid[21] = true;
				}
				else if (i - miCurImageCount == 22)
				{
					SetPart(m_HWinPanel23.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[22])
						SetColor(m_HWinPanel23.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel23.mlWindowHandle, "green");

					SetDraw(m_HWinPanel23.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel23.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel23.mlWindowHandle);

					mbImageValid[22] = true;
				}
				else if (i - miCurImageCount == 23)
				{
					SetPart(m_HWinPanel24.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[23])
						SetColor(m_HWinPanel24.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel24.mlWindowHandle, "green");

					SetDraw(m_HWinPanel24.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel24.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel24.mlWindowHandle);

					mbImageValid[23] = true;
				}
				else if (i - miCurImageCount == 24)
				{
					SetPart(m_HWinPanel25.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[24])
						SetColor(m_HWinPanel25.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel25.mlWindowHandle, "green");

					SetDraw(m_HWinPanel25.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel25.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel25.mlWindowHandle);

					mbImageValid[24] = true;
				}
				else if (i - miCurImageCount == 25)
				{
					SetPart(m_HWinPanel26.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[25])
						SetColor(m_HWinPanel26.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel26.mlWindowHandle, "green");

					SetDraw(m_HWinPanel26.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel26.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel26.mlWindowHandle);

					mbImageValid[25] = true;
				}
				else if (i - miCurImageCount == 26)
				{
					SetPart(m_HWinPanel27.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[26])
						SetColor(m_HWinPanel27.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel27.mlWindowHandle, "green");

					SetDraw(m_HWinPanel27.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel27.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel27.mlWindowHandle);

					mbImageValid[26] = true;
				}
				else if (i - miCurImageCount == 27)
				{
					SetPart(m_HWinPanel28.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[27])
						SetColor(m_HWinPanel28.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel28.mlWindowHandle, "green");

					SetDraw(m_HWinPanel28.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel28.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel28.mlWindowHandle);

					mbImageValid[27] = true;
				}
				else if (i - miCurImageCount == 28)
				{
					SetPart(m_HWinPanel29.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[28])
						SetColor(m_HWinPanel29.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel29.mlWindowHandle, "green");

					SetDraw(m_HWinPanel29.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel29.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel29.mlWindowHandle);

					mbImageValid[28] = true;
				}
				else if (i - miCurImageCount == 29)
				{
					SetPart(m_HWinPanel30.mlWindowHandle, 0, 0, lHeight, lWidth);

					GenRectangle1(&HRectRgn, 0, 0, lHeight, lWidth);

					if (mbSelectImageIndex[29])
						SetColor(m_HWinPanel30.mlWindowHandle, "red");
					else
						SetColor(m_HWinPanel30.mlWindowHandle, "green");

					SetDraw(m_HWinPanel30.mlWindowHandle, "fill");
					DispObj(HRectRgn, m_HWinPanel30.mlWindowHandle);

					DispObj(pHImage[i], m_HWinPanel30.mlWindowHandle);

					mbImageValid[29] = true;
				}
			}
		}
	}

	CString str;
	if (iCount == 0)
		str.Format("0 / 0");
	else
		str.Format("%d / %d", miCurImageCount, iCount);

	m_stFontCount.SetWindowText((LPCTSTR)str);

}


void CAlgorithmOCRDlg::OnSelchangeListFont()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString CurFontName;

	if (m_FontListBox.GetCurSel() >= 0)
	{
		m_FontListBox.GetText(m_FontListBox.GetCurSel(), CurFontName);

		THEAPP.m_pModelDataManager->m_strFontName = CurFontName;

		CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font\\" + CurFontName + ".trf";

		LoadFontFile(strFontFolder);

		Invalidate(TRUE);
	}
}

void CAlgorithmOCRDlg::OnBnClickedButtonFontimagePrev()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCount = 0;

	switch (m_iRadioCharIndex)
	{
	case 0:
		iCount = miCount_A;
		break;

	case 1:
		iCount = miCount_B;
		break;

	case 2:
		iCount = miCount_C;
		break;

	case 3:
		iCount = miCount_D;
		break;

	case 4:
		iCount = miCount_E;
		break;

	case 5:
		iCount = miCount_F;
		break;

	case 6:
		iCount = miCount_G;
		break;

	case 7:
		iCount = miCount_H;
		break;

	case 8:
		iCount = miCount_I;
		break;

	case 9:
		iCount = miCount_J;
		break;

	case 10:
		iCount = miCount_K;
		break;

	case 11:
		iCount = miCount_L;
		break;

	case 12:
		iCount = miCount_M;
		break;

	case 13:
		iCount = miCount_N;
		break;

	case 14:
		iCount = miCount_O;
		break;

	case 15:
		iCount = miCount_P;
		break;

	case 16:
		iCount = miCount_Q;
		break;

	case 17:
		iCount = miCount_R;
		break;

	case 18:
		iCount = miCount_S;
		break;

	case 19:
		iCount = miCount_T;
		break;

	case 20:
		iCount = miCount_U;
		break;

	case 21:
		iCount = miCount_V;
		break;

	case 22:
		iCount = miCount_W;
		break;

	case 23:
		iCount = miCount_X;
		break;

	case 24:
		iCount = miCount_Y;
		break;

	case 25:
		iCount = miCount_Z;
		break;

	case 26:
		iCount = miCount_0;
		break;

	case 27:
		iCount = miCount_1;
		break;

	case 28:
		iCount = miCount_2;
		break;

	case 29:
		iCount = miCount_3;
		break;

	case 30:
		iCount = miCount_4;
		break;

	case 31:
		iCount = miCount_5;
		break;

	case 32:
		iCount = miCount_6;
		break;

	case 33:
		iCount = miCount_7;
		break;

	case 34:
		iCount = miCount_8;
		break;

	case 35:
		iCount = miCount_9;
		break;
	}

	if (miCurImageCount - FONT_IAMGE_COUNT >= 0)
		miCurImageCount -= FONT_IAMGE_COUNT;

	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
		mbSelectImageIndex[i] = false;

	Invalidate(TRUE);
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontimageNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCount = 0;

	switch (m_iRadioCharIndex)
	{
	case 0:
		iCount = miCount_A;
		break;

	case 1:
		iCount = miCount_B;
		break;

	case 2:
		iCount = miCount_C;
		break;

	case 3:
		iCount = miCount_D;
		break;

	case 4:
		iCount = miCount_E;
		break;

	case 5:
		iCount = miCount_F;
		break;

	case 6:
		iCount = miCount_G;
		break;

	case 7:
		iCount = miCount_H;
		break;

	case 8:
		iCount = miCount_I;
		break;

	case 9:
		iCount = miCount_J;
		break;

	case 10:
		iCount = miCount_K;
		break;

	case 11:
		iCount = miCount_L;
		break;

	case 12:
		iCount = miCount_M;
		break;

	case 13:
		iCount = miCount_N;
		break;

	case 14:
		iCount = miCount_O;
		break;

	case 15:
		iCount = miCount_P;
		break;

	case 16:
		iCount = miCount_Q;
		break;

	case 17:
		iCount = miCount_R;
		break;

	case 18:
		iCount = miCount_S;
		break;

	case 19:
		iCount = miCount_T;
		break;

	case 20:
		iCount = miCount_U;
		break;

	case 21:
		iCount = miCount_V;
		break;

	case 22:
		iCount = miCount_W;
		break;

	case 23:
		iCount = miCount_X;
		break;

	case 24:
		iCount = miCount_Y;
		break;

	case 25:
		iCount = miCount_Z;
		break;

	case 26:
		iCount = miCount_0;
		break;

	case 27:
		iCount = miCount_1;
		break;

	case 28:
		iCount = miCount_2;
		break;

	case 29:
		iCount = miCount_3;
		break;

	case 30:
		iCount = miCount_4;
		break;

	case 31:
		iCount = miCount_5;
		break;

	case 32:
		iCount = miCount_6;
		break;

	case 33:
		iCount = miCount_7;
		break;

	case 34:
		iCount = miCount_8;
		break;

	case 35:
		iCount = miCount_9;
		break;
	}

	if (miCurImageCount + FONT_IAMGE_COUNT < iCount)
		miCurImageCount += FONT_IAMGE_COUNT;

	for (int i = 0; i < FONT_IAMGE_COUNT; i++)
		mbSelectImageIndex[i] = false;

	Invalidate(TRUE);
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontimageAlldelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HObject* pHImage = NULL;
	int iCount = 0;

	switch (m_iRadioCharIndex)
	{
	case 0:
		pHImage = mp_HCharImage_A;
		iCount = miCount_A;
		break;

	case 1:
		pHImage = mp_HCharImage_B;
		iCount = miCount_B;
		break;

	case 2:
		pHImage = mp_HCharImage_C;
		iCount = miCount_C;
		break;

	case 3:
		pHImage = mp_HCharImage_D;
		iCount = miCount_D;
		break;

	case 4:
		pHImage = mp_HCharImage_E;
		iCount = miCount_E;
		break;

	case 5:
		pHImage = mp_HCharImage_F;
		iCount = miCount_F;
		break;

	case 6:
		pHImage = mp_HCharImage_G;
		iCount = miCount_G;
		break;

	case 7:
		pHImage = mp_HCharImage_H;
		iCount = miCount_H;
		break;

	case 8:
		pHImage = mp_HCharImage_I;
		iCount = miCount_I;
		break;

	case 9:
		pHImage = mp_HCharImage_J;
		iCount = miCount_J;
		break;

	case 10:
		pHImage = mp_HCharImage_K;
		iCount = miCount_K;
		break;

	case 11:
		pHImage = mp_HCharImage_L;
		iCount = miCount_L;
		break;

	case 12:
		pHImage = mp_HCharImage_M;
		iCount = miCount_M;
		break;

	case 13:
		pHImage = mp_HCharImage_N;
		iCount = miCount_N;
		break;

	case 14:
		pHImage = mp_HCharImage_O;
		iCount = miCount_O;
		break;

	case 15:
		pHImage = mp_HCharImage_P;
		iCount = miCount_P;
		break;

	case 16:
		pHImage = mp_HCharImage_Q;
		iCount = miCount_Q;
		break;

	case 17:
		pHImage = mp_HCharImage_R;
		iCount = miCount_R;
		break;

	case 18:
		pHImage = mp_HCharImage_S;
		iCount = miCount_S;
		break;

	case 19:
		pHImage = mp_HCharImage_T;
		iCount = miCount_T;
		break;

	case 20:
		pHImage = mp_HCharImage_U;
		iCount = miCount_U;
		break;

	case 21:
		pHImage = mp_HCharImage_V;
		iCount = miCount_V;
		break;

	case 22:
		pHImage = mp_HCharImage_W;
		iCount = miCount_W;
		break;

	case 23:
		pHImage = mp_HCharImage_X;
		iCount = miCount_X;
		break;

	case 24:
		pHImage = mp_HCharImage_Y;
		iCount = miCount_Y;
		break;

	case 25:
		pHImage = mp_HCharImage_Z;
		iCount = miCount_Z;
		break;

	case 26:
		pHImage = mp_HCharImage_0;
		iCount = miCount_0;
		break;

	case 27:
		pHImage = mp_HCharImage_1;
		iCount = miCount_1;
		break;

	case 28:
		pHImage = mp_HCharImage_2;
		iCount = miCount_2;
		break;

	case 29:
		pHImage = mp_HCharImage_3;
		iCount = miCount_3;
		break;

	case 30:
		pHImage = mp_HCharImage_4;
		iCount = miCount_4;
		break;

	case 31:
		pHImage = mp_HCharImage_5;
		iCount = miCount_5;
		break;

	case 32:
		pHImage = mp_HCharImage_6;
		iCount = miCount_6;
		break;

	case 33:
		pHImage = mp_HCharImage_7;
		iCount = miCount_7;
		break;

	case 34:
		pHImage = mp_HCharImage_8;
		iCount = miCount_8;
		break;

	case 35:
		pHImage = mp_HCharImage_9;
		iCount = miCount_9;
		break;
	}

	if (AfxMessageBox("전체 폰트 이미지를 삭제 하시겠습니까 ?", MB_OKCANCEL) == IDOK)
	{
		for (int i = 0; i < iCount; i++)
		{
			GenEmptyObj(&(pHImage[i]));
		}
	}

	Invalidate(TRUE);
}

void CAlgorithmOCRDlg::OnBnClickedButtonFontNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CCreateFontDlg CreateFontDlg;

	if (CreateFontDlg.DoModal() == IDOK)
	{
		CString CurFontName;
		CurFontName = CreateFontDlg.m_sFontName;

		TrainOCR_SVM(CurFontName, true);

		UpdateFontList();

	}
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_FontListBox.GetCurSel() >= 0)
	{
		CString CurFontName;
		m_FontListBox.GetText(m_FontListBox.GetCurSel(), CurFontName);

		TrainOCR_SVM(CurFontName, false);

		UpdateFontList();
	}
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString CurFontName;

	if (m_FontListBox.GetCurSel() >= 0)
	{
		int res = AfxMessageBox("현재 리스트에서 선택된 폰트 파일을 삭제 하시겠습니까?", MB_YESNOCANCEL);

		if (res == IDYES)
		{
			m_FontListBox.GetText(m_FontListBox.GetCurSel(), CurFontName);

			CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font\\" + CurFontName + ".trf";
			DeleteFile(HTuple(strFontFolder));
			strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font\\" + CurFontName + ".svm";
			DeleteFile(HTuple(strFontFolder));

			UpdateFontList();
		}
	}
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontSave()
{
	int res = AfxMessageBox("현재 리스트에서 선택된 폰트 파일에 저장 하시겠습니까?", MB_YESNOCANCEL);

	if (res == IDCANCEL)
		return;
	else if (res == IDNO)
	{
		char caBuf[256];
		char* caExt = ".trf";
		sprintf(caBuf, "Font Data Files (*%s)|*%s|", caExt, caExt);
		CFileDialog FileDialog(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, caBuf, this);

		if (FileDialog.DoModal() == IDOK)
		{
			CString PathName;
			PathName = (LPCTSTR)FileDialog.GetPathName();
			PathName = PathName + caExt;

			SaveFontFile(PathName);
			UpdateFontList();
		}
		else
			return;
	}
	else if (res == IDYES)
	{
		if (m_FontListBox.GetCurSel() >= 0)
		{
			CString CurFontName;
			m_FontListBox.GetText(m_FontListBox.GetCurSel(), CurFontName);
			CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font\\" + CurFontName + ".trf";
			SaveFontFile(strFontFolder);

			UpdateFontList();
		}
	}
}

bool CAlgorithmOCRDlg::TrainOCR_SVM(CString FontName, bool bCreate)
{
	try
	{

		THEAPP.m_pAlgorithm->m_bManualBarcodeForOcrInspect = FALSE;

		return true;
	}
	catch (HException& except)
	{
		THEAPP.m_pAlgorithm->m_bManualBarcodeForOcrInspect = FALSE;

		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmOCRDlg::TrainOCR_SVM] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return FALSE;
	}
}

void CAlgorithmOCRDlg::OnStnClickedStatic1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (mbImageValid[0])
	{
		if (mbSelectImageIndex[0])
			mbSelectImageIndex[0] = false;
		else
			mbSelectImageIndex[0] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[1])
	{
		if (mbSelectImageIndex[1])
			mbSelectImageIndex[1] = false;
		else
			mbSelectImageIndex[1] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[2])
	{
		if (mbSelectImageIndex[2])
			mbSelectImageIndex[2] = false;
		else
			mbSelectImageIndex[2] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[3])
	{
		if (mbSelectImageIndex[3])
			mbSelectImageIndex[3] = false;
		else
			mbSelectImageIndex[3] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[4])
	{
		if (mbSelectImageIndex[4])
			mbSelectImageIndex[4] = false;
		else
			mbSelectImageIndex[4] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[5])
	{
		if (mbSelectImageIndex[5])
			mbSelectImageIndex[5] = false;
		else
			mbSelectImageIndex[5] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[6])
	{
		if (mbSelectImageIndex[6])
			mbSelectImageIndex[6] = false;
		else
			mbSelectImageIndex[6] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[7])
	{
		if (mbSelectImageIndex[7])
			mbSelectImageIndex[7] = false;
		else
			mbSelectImageIndex[7] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[8])
	{
		if (mbSelectImageIndex[8])
			mbSelectImageIndex[8] = false;
		else
			mbSelectImageIndex[8] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[9])
	{
		if (mbSelectImageIndex[9])
			mbSelectImageIndex[9] = false;
		else
			mbSelectImageIndex[9] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[10])
	{
		if (mbSelectImageIndex[10])
			mbSelectImageIndex[10] = false;
		else
			mbSelectImageIndex[10] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[11])
	{
		if (mbSelectImageIndex[11])
			mbSelectImageIndex[11] = false;
		else
			mbSelectImageIndex[11] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[12])
	{
		if (mbSelectImageIndex[12])
			mbSelectImageIndex[12] = false;
		else
			mbSelectImageIndex[12] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[13])
	{
		if (mbSelectImageIndex[13])
			mbSelectImageIndex[13] = false;
		else
			mbSelectImageIndex[13] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[14])
	{
		if (mbSelectImageIndex[14])
			mbSelectImageIndex[14] = false;
		else
			mbSelectImageIndex[14] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[15])
	{
		if (mbSelectImageIndex[15])
			mbSelectImageIndex[15] = false;
		else
			mbSelectImageIndex[15] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic17()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[16])
	{
		if (mbSelectImageIndex[16])
			mbSelectImageIndex[16] = false;
		else
			mbSelectImageIndex[16] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic18()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[17])
	{
		if (mbSelectImageIndex[17])
			mbSelectImageIndex[17] = false;
		else
			mbSelectImageIndex[17] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic19()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[18])
	{
		if (mbSelectImageIndex[18])
			mbSelectImageIndex[18] = false;
		else
			mbSelectImageIndex[18] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic20()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[19])
	{
		if (mbSelectImageIndex[19])
			mbSelectImageIndex[19] = false;
		else
			mbSelectImageIndex[19] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic21()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[20])
	{
		if (mbSelectImageIndex[20])
			mbSelectImageIndex[20] = false;
		else
			mbSelectImageIndex[20] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic22()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[21])
	{
		if (mbSelectImageIndex[21])
			mbSelectImageIndex[21] = false;
		else
			mbSelectImageIndex[21] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic23()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[22])
	{
		if (mbSelectImageIndex[22])
			mbSelectImageIndex[22] = false;
		else
			mbSelectImageIndex[22] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic24()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[23])
	{
		if (mbSelectImageIndex[23])
			mbSelectImageIndex[23] = false;
		else
			mbSelectImageIndex[23] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic25()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[24])
	{
		if (mbSelectImageIndex[24])
			mbSelectImageIndex[24] = false;
		else
			mbSelectImageIndex[24] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic26()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[25])
	{
		if (mbSelectImageIndex[25])
			mbSelectImageIndex[25] = false;
		else
			mbSelectImageIndex[25] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic27()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[26])
	{
		if (mbSelectImageIndex[26])
			mbSelectImageIndex[26] = false;
		else
			mbSelectImageIndex[26] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic28()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[27])
	{
		if (mbSelectImageIndex[27])
			mbSelectImageIndex[27] = false;
		else
			mbSelectImageIndex[27] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic29()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[28])
	{
		if (mbSelectImageIndex[28])
			mbSelectImageIndex[28] = false;
		else
			mbSelectImageIndex[28] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnStnClickedStatic30()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (mbImageValid[29])
	{
		if (mbSelectImageIndex[29])
			mbSelectImageIndex[29] = false;
		else
			mbSelectImageIndex[29] = true;

		Invalidate(TRUE);
	}
}


void CAlgorithmOCRDlg::OnBnClickedButtonFontimageDelete()
{
	HObject* pHImage = NULL;
	int iCount = 0;

	switch (m_iRadioCharIndex)
	{
	case 0:
		pHImage = mp_HCharImage_A;
		iCount = miCount_A;
		break;

	case 1:
		pHImage = mp_HCharImage_B;
		iCount = miCount_B;
		break;

	case 2:
		pHImage = mp_HCharImage_C;
		iCount = miCount_C;
		break;

	case 3:
		pHImage = mp_HCharImage_D;
		iCount = miCount_D;
		break;
	case 4:
		pHImage = mp_HCharImage_E;
		iCount = miCount_E;
		break;

	case 5:
		pHImage = mp_HCharImage_F;
		iCount = miCount_F;
		break;

	case 6:
		pHImage = mp_HCharImage_G;
		iCount = miCount_G;
		break;

	case 7:
		pHImage = mp_HCharImage_H;
		iCount = miCount_H;
		break;

	case 8:
		pHImage = mp_HCharImage_I;
		iCount = miCount_I;
		break;

	case 9:
		pHImage = mp_HCharImage_J;
		iCount = miCount_J;
		break;

	case 10:
		pHImage = mp_HCharImage_K;
		iCount = miCount_K;
		break;

	case 11:
		pHImage = mp_HCharImage_L;
		iCount = miCount_L;
		break;

	case 12:
		pHImage = mp_HCharImage_M;
		iCount = miCount_M;
		break;

	case 13:
		pHImage = mp_HCharImage_N;
		iCount = miCount_N;
		break;

	case 14:
		pHImage = mp_HCharImage_O;
		iCount = miCount_O;
		break;

	case 15:
		pHImage = mp_HCharImage_P;
		iCount = miCount_P;
		break;

	case 16:
		pHImage = mp_HCharImage_Q;
		iCount = miCount_Q;
		break;

	case 17:
		pHImage = mp_HCharImage_R;
		iCount = miCount_R;
		break;

	case 18:
		pHImage = mp_HCharImage_S;
		iCount = miCount_S;
		break;

	case 19:
		pHImage = mp_HCharImage_T;
		iCount = miCount_T;
		break;

	case 20:
		pHImage = mp_HCharImage_U;
		iCount = miCount_U;
		break;

	case 21:
		pHImage = mp_HCharImage_V;
		iCount = miCount_V;
		break;

	case 22:
		pHImage = mp_HCharImage_W;
		iCount = miCount_W;
		break;

	case 23:
		pHImage = mp_HCharImage_X;
		iCount = miCount_X;
		break;

	case 24:
		pHImage = mp_HCharImage_Y;
		iCount = miCount_Y;
		break;

	case 25:
		pHImage = mp_HCharImage_Z;
		iCount = miCount_Z;
		break;

	case 26:
		pHImage = mp_HCharImage_0;
		iCount = miCount_0;
		break;

	case 27:
		pHImage = mp_HCharImage_1;
		iCount = miCount_1;
		break;

	case 28:
		pHImage = mp_HCharImage_2;
		iCount = miCount_2;
		break;

	case 29:
		pHImage = mp_HCharImage_3;
		iCount = miCount_3;
		break;

	case 30:
		pHImage = mp_HCharImage_4;
		iCount = miCount_4;
		break;

	case 31:
		pHImage = mp_HCharImage_5;
		iCount = miCount_5;
		break;

	case 32:
		pHImage = mp_HCharImage_6;
		iCount = miCount_6;
		break;

	case 33:
		pHImage = mp_HCharImage_7;
		iCount = miCount_7;
		break;

	case 34:
		pHImage = mp_HCharImage_8;
		iCount = miCount_8;
		break;

	case 35:
		pHImage = mp_HCharImage_9;
		iCount = miCount_9;
		break;
	}

	if (AfxMessageBox("선택된 폰트 이미지를 삭제 하시겠습니까 ?", MB_OKCANCEL) == IDOK)
	{
		for (int i = 0; i < FONT_IAMGE_COUNT; i++)
		{
			if (mbSelectImageIndex[i])
			{
				if (i + miCurImageCount < iCount)
					GenEmptyObj(&(pHImage[i + miCurImageCount]));
			}
		}

		for (int i = 0; i < FONT_IAMGE_COUNT; i++)
			mbSelectImageIndex[i] = false;
	}

	Invalidate(TRUE);
}

void CAlgorithmOCRDlg::SaveFontFile(CString FontPathName)
{
	try
	{
		HObject HReduceRgn, HConcatImage, HConcatRgn;
		HTuple HChar, HConcatChar;

		int bSuccess;
		int iCount = 0;

		for (int i = 0; i < miCount_A; i++)
		{
			HChar = "A";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_A[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_A[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_A[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_B; i++)
		{
			HChar = "B";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_B[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_B[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_B[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_C; i++)
		{
			HChar = "C";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_C[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_C[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_C[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_D; i++)
		{
			HChar = "D";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_D[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_D[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_D[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_E; i++)
		{
			HChar = "E";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_E[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_E[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_E[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_F; i++)
		{
			HChar = "F";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_F[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_F[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_F[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_G; i++)
		{
			HChar = "G";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_G[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_G[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_G[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_H; i++)
		{
			HChar = "H";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_H[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_H[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_H[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_I; i++)
		{
			HChar = "I";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_I[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_I[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_I[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_J; i++)
		{
			HChar = "J";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_J[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_J[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_J[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_K; i++)
		{
			HChar = "K";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_K[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_K[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_K[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_L; i++)
		{
			HChar = "L";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_L[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_L[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_L[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_M; i++)
		{
			HChar = "M";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_M[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_M[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_M[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_N; i++)
		{
			HChar = "N";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_N[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_N[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_N[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_O; i++)
		{
			HChar = "O";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_O[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_O[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_O[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_P; i++)
		{
			HChar = "P";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_P[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_P[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_P[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_Q; i++)
		{
			HChar = "Q";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_Q[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_Q[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_Q[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_R; i++)
		{
			HChar = "R";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_R[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_R[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_R[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_S; i++)
		{
			HChar = "S";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_S[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_S[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_S[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_T; i++)
		{
			HChar = "T";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_T[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_T[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_T[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_U; i++)
		{
			HChar = "U";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_U[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_U[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_U[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_V; i++)
		{
			HChar = "V";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_V[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_V[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_V[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_W; i++)
		{
			HChar = "W";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_W[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_W[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_W[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_X; i++)
		{
			HChar = "X";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_X[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_X[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_X[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_Y; i++)
		{
			HChar = "Y";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_Y[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_Y[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_Y[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_Z; i++)
		{
			HChar = "Z";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_Z[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_Z[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_Z[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_0; i++)
		{
			HChar = "0";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_0[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_0[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_0[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_1; i++)
		{
			HChar = "1";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_1[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_1[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_1[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_2; i++)
		{
			HChar = "2";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_2[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_2[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_2[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_3; i++)
		{
			HChar = "3";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_3[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_3[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_3[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_4; i++)
		{
			HChar = "4";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_4[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_4[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_4[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_5; i++)
		{
			HChar = "5";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_5[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_5[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_5[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_6; i++)
		{
			HChar = "6";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_6[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_6[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_6[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_7; i++)
		{
			HChar = "7";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_7[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_7[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_7[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_8; i++)
		{
			HChar = "8";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_8[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_8[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_8[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		for (int i = 0; i < miCount_9; i++)
		{
			HChar = "9";
			if (THEAPP.m_pGFunction->ValidHImage(mp_HCharImage_9[i]))
			{
				if (iCount == 0)
					CopyObj(mp_HCharImage_9[i], &HConcatImage, 1, 1);
				else
					ConcatObj(HConcatImage, mp_HCharImage_9[i], &HConcatImage);

				TupleConcat(HConcatChar, HChar, &HConcatChar);
				iCount++;
			}
		}

		bSuccess = TRUE;
		try
		{
			WriteOcrTrainfImage(HConcatImage, HConcatChar, HTuple(FontPathName));
		}
		catch (HException &except)
		{
			bSuccess = FALSE;
		}





		HTuple HOCRHandle;
		if (bSuccess == H_MSG_TRUE)
		{
			HTuple Error, ErrorLog, CharacterNames, CharacterCount, Hfeatures;

			ReadOcrTrainfNames(HTuple(FontPathName), &CharacterNames, &CharacterCount);

			CreateOcrClassSvm(HTuple(8), HTuple(10), HTuple("constant"), HTuple("default"), CharacterNames, HTuple("rbf"), HTuple(0.01), HTuple(0.001), HTuple("one-versus-one"), "none", HTuple(10), &HOCRHandle);
			TrainfOcrClassSvm(HOCRHandle, HTuple(FontPathName), HTuple(0.001), HTuple("default"));

			FontPathName.Delete(FontPathName.GetLength() - 4, 4);
			WriteOcrClassSvm(HOCRHandle, HTuple(FontPathName + ".svm"));
		}
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmOCRDlg::SaveFontFile] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		return;
	}
}

HObject CAlgorithmOCRDlg::CharSegmentAlgoritm(HObject& hOCRImage, HObject HROIHRegion)
{
	try
	{
		HObject HRegionTrans;
		GenEmptyObj(&HRegionTrans);

		return HRegionTrans;

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		strLog.Format("Halcon Exception [AlgorithmOCRDlg::CharSegmentAlgoritm] : <%s>%s", (const char *)HOperatorName.S(), (const char *)HErrMsg.S());
		THEAPP.m_log_halcon->warn("{}", LOG_CSTR(strLog));

		HObject HDefectRgn;
		GenEmptyObj(&HDefectRgn);
		return HDefectRgn;
	}
}

void CAlgorithmOCRDlg::OnBnClickedButtonFontNew2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString FontName;
	m_FontListBox.GetText(m_FontListBox.GetCurSel(), FontName);
	CString strFontFolder = THEAPP.GetWorkingDirectory() + "\\Data\\Font\\" + FontName;

	HTuple HOCRHandle;
	CString strChar;
	CEdit *pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CARACTER);
	pEdit->GetWindowText(strChar);

	HTuple hChar;
	int bSuccess;
	HObject hCharRegionBox2;

	TupleGenConst(0, 0, &hChar);
	for (int i = 0; i < strChar.GetLength(); i++)
	{
		TupleConcat(hChar, HTuple(strChar.Mid(i, 1)), &hChar);
	}

	HObject readImg, HImageMedianProcessed, HMeanImage, HDynThresholeRgn, HConnection;

	for (int i = 1; i < 21; i++)
	{
		CString ImagePath;
		ImagePath.Format("c:\\Users\\synapse\\Desktop\\OCRCREATE\\%c\\%d.jpg", strChar[0], i);

		ReadImage(&readImg, HTuple(ImagePath));

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		HTuple HpImageData, Htype, HlImageWidth, HlImageHeight;

		GetImagePointer1(readImg, &HpImageData, &Htype, &HlImageWidth, &HlImageHeight);

		pImageData = (BYTE *)HpImageData.L();
		lImageWidth = HlImageWidth.L();
		lImageHeight = HlImageHeight.L();

		MedianImage(readImg, &HImageMedianProcessed, "circle", 2, "mirrored");
		Hlong lRow1, lRow2, lCol1, lCol2;

		HObject HCharRgn;

		MeanImage(readImg, &HMeanImage, 100, 100);
		DynThreshold(HImageMedianProcessed, HMeanImage, &HDynThresholeRgn, 5, "light");
		Connection(HDynThresholeRgn, &HConnection);

		HObject HSelectShape, HSelectShape2, HSelectShape3;
		SelectShape(HConnection, &HSelectShape, "width", "and", 5, 99999999);
		SelectShape(HSelectShape, &HSelectShape2, "height", "and", 20, 9999999);
		SelectShape(HSelectShape2, &HSelectShape3, "area", "and", 20, 99999999);
		SelectShapeStd(HSelectShape3, &HSelectShape3, "max_area", 70);
		HObject HUnion;
		Union1(HSelectShape3, &HUnion);

		if (THEAPP.m_pGFunction->ValidHRegion(HUnion) == TRUE)
		{
			CopyObj(HUnion, &hCharRegionBox2, 1, 1);
		}

		Hlong lRgnCnt;
		Connection(hCharRegionBox2, &hCharRegionBox2);
		HTuple HlRgnCnt;
		CountObj(hCharRegionBox2, &HlRgnCnt);
		lRgnCnt = HlRgnCnt.L();

		bSuccess = TRUE;
		try
		{
			AppendOcrTrainf(hCharRegionBox2, readImg, hChar, HTuple(strFontFolder + ".trf"));
		}
		catch (HException &except)
		{
			bSuccess = FALSE;
		}


		THEAPP.m_pAlgorithm->m_strOCRChar = strChar;

		if (bSuccess == H_MSG_TRUE)
		{
			RegisterCharacter(strChar, FontName);
		}
		else
		{
			AfxMessageBox("등록 실패!!", MB_SYSTEMMODAL);
		}
	}

	AfxMessageBox("등록 성공!!", MB_SYSTEMMODAL);

}


void CAlgorithmOCRDlg::RegisterCharacter(CString strChar, CString FontName)
{

}

