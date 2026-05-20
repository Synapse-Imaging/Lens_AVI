// LightPageControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightPageControlDlg.h"
#include "afxdialogex.h"


// CLightPageControlDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightPageControlDlg, CDialog)

CLightPageControlDlg::CLightPageControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightPageControlDlg::IDD, pParent)
{
	m_iEditComPortNumber = 1;

	m_sEditPageDesc = _T("");

	m_iRadioPageNumber = 0;
	m_iCurPageIndex = 0;

	m_iEditLight1 = 0;
	m_iEditLight2 = 0;
	m_iEditLight3 = 0;
	m_iEditLight4 = 0;
	m_iEditLight5 = 0;
	m_iEditLight6 = 0;
	m_iEditLight7 = 0;
	m_iEditLight8 = 0;
	m_iEditLight9 = 0;
	m_iEditLight10 = 0;
	m_iEditLight11 = 0;
	m_iEditLight12 = 0;
	m_iEditLight13 = 0;
	m_iEditLight14 = 0;
	m_iEditLight15 = 0;
	m_iEditLight16 = 0;
	m_iEditLight17 = 0;
	m_iEditLight18 = 0;
	m_iEditLight19 = 0;
	m_iEditLight20 = 0;
	m_iEditLight21 = 0;
	m_iEditLight22 = 0;
	m_iEditLight23 = 0;
	m_iEditLight24 = 0;
	m_iEditLight25 = 0;
	m_iEditLight26 = 0;
	m_iEditLight27 = 0;
	m_iEditLight28 = 0;
	m_iEditLight29 = 0;
	m_iEditLight30 = 0;
	m_iEditLight31 = 0;
	m_iEditLight32 = 0;

	m_pLightLibraryDlg = NULL;
}

CLightPageControlDlg::~CLightPageControlDlg()
{
}

void CLightPageControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PAGE_DESC, m_sEditPageDesc);
	DDX_Radio(pDX, IDC_RADIO_PAGE_1, m_iRadioPageNumber);
	DDX_Control(pDX, IDC_SLIDER_LIGHT1, m_SliderLight1);
	DDX_Control(pDX, IDC_SLIDER_LIGHT2, m_SliderLight2);
	DDX_Control(pDX, IDC_SLIDER_LIGHT3, m_SliderLight3);
	DDX_Control(pDX, IDC_SLIDER_LIGHT4, m_SliderLight4);
	DDX_Control(pDX, IDC_SLIDER_LIGHT5, m_SliderLight5);
	DDX_Control(pDX, IDC_SLIDER_LIGHT6, m_SliderLight6);
	DDX_Control(pDX, IDC_SLIDER_LIGHT7, m_SliderLight7);
	DDX_Control(pDX, IDC_SLIDER_LIGHT8, m_SliderLight8);
	DDX_Control(pDX, IDC_SLIDER_LIGHT9, m_SliderLight9);
	DDX_Control(pDX, IDC_SLIDER_LIGHT10, m_SliderLight10);
	DDX_Control(pDX, IDC_SLIDER_LIGHT11, m_SliderLight11);
	DDX_Control(pDX, IDC_SLIDER_LIGHT12, m_SliderLight12);
	DDX_Control(pDX, IDC_SLIDER_LIGHT13, m_SliderLight13);
	DDX_Control(pDX, IDC_SLIDER_LIGHT14, m_SliderLight14);
	DDX_Control(pDX, IDC_SLIDER_LIGHT15, m_SliderLight15);
	DDX_Control(pDX, IDC_SLIDER_LIGHT16, m_SliderLight16);
	DDX_Control(pDX, IDC_SLIDER_LIGHT17, m_SliderLight17);
	DDX_Control(pDX, IDC_SLIDER_LIGHT18, m_SliderLight18);
	DDX_Control(pDX, IDC_SLIDER_LIGHT19, m_SliderLight19);
	DDX_Control(pDX, IDC_SLIDER_LIGHT20, m_SliderLight20);
	DDX_Control(pDX, IDC_SLIDER_LIGHT21, m_SliderLight21);
	DDX_Control(pDX, IDC_SLIDER_LIGHT22, m_SliderLight22);
	DDX_Control(pDX, IDC_SLIDER_LIGHT23, m_SliderLight23);
	DDX_Control(pDX, IDC_SLIDER_LIGHT24, m_SliderLight24);
	DDX_Control(pDX, IDC_SLIDER_LIGHT25, m_SliderLight25);
	DDX_Control(pDX, IDC_SLIDER_LIGHT26, m_SliderLight26);
	DDX_Control(pDX, IDC_SLIDER_LIGHT27, m_SliderLight27);
	DDX_Control(pDX, IDC_SLIDER_LIGHT28, m_SliderLight28);
	DDX_Control(pDX, IDC_SLIDER_LIGHT29, m_SliderLight29);
	DDX_Control(pDX, IDC_SLIDER_LIGHT30, m_SliderLight30);
	DDX_Control(pDX, IDC_SLIDER_LIGHT31, m_SliderLight31);
	DDX_Control(pDX, IDC_SLIDER_LIGHT32, m_SliderLight32);

	DDX_Text(pDX, IDC_EDIT_LIGHT1, m_iEditLight1);
	DDX_Text(pDX, IDC_EDIT_LIGHT2, m_iEditLight2);
	DDX_Text(pDX, IDC_EDIT_LIGHT3, m_iEditLight3);
	DDX_Text(pDX, IDC_EDIT_LIGHT4, m_iEditLight4);
	DDX_Text(pDX, IDC_EDIT_LIGHT5, m_iEditLight5);
	DDX_Text(pDX, IDC_EDIT_LIGHT6, m_iEditLight6);
	DDX_Text(pDX, IDC_EDIT_LIGHT7, m_iEditLight7);
	DDX_Text(pDX, IDC_EDIT_LIGHT8, m_iEditLight8);
	DDX_Text(pDX, IDC_EDIT_LIGHT9, m_iEditLight9);
	DDX_Text(pDX, IDC_EDIT_LIGHT10, m_iEditLight10);
	DDX_Text(pDX, IDC_EDIT_LIGHT11, m_iEditLight11);
	DDX_Text(pDX, IDC_EDIT_LIGHT12, m_iEditLight12);
	DDX_Text(pDX, IDC_EDIT_LIGHT13, m_iEditLight13);
	DDX_Text(pDX, IDC_EDIT_LIGHT14, m_iEditLight14);
	DDX_Text(pDX, IDC_EDIT_LIGHT15, m_iEditLight15);
	DDX_Text(pDX, IDC_EDIT_LIGHT16, m_iEditLight16);
	DDX_Text(pDX, IDC_EDIT_LIGHT17, m_iEditLight17);
	DDX_Text(pDX, IDC_EDIT_LIGHT18, m_iEditLight18);
	DDX_Text(pDX, IDC_EDIT_LIGHT19, m_iEditLight19);
	DDX_Text(pDX, IDC_EDIT_LIGHT20, m_iEditLight20);
	DDX_Text(pDX, IDC_EDIT_LIGHT21, m_iEditLight21);
	DDX_Text(pDX, IDC_EDIT_LIGHT22, m_iEditLight22);
	DDX_Text(pDX, IDC_EDIT_LIGHT23, m_iEditLight23);
	DDX_Text(pDX, IDC_EDIT_LIGHT24, m_iEditLight24);
	DDX_Text(pDX, IDC_EDIT_LIGHT25, m_iEditLight25);
	DDX_Text(pDX, IDC_EDIT_LIGHT26, m_iEditLight26);
	DDX_Text(pDX, IDC_EDIT_LIGHT27, m_iEditLight27);
	DDX_Text(pDX, IDC_EDIT_LIGHT28, m_iEditLight28);
	DDX_Text(pDX, IDC_EDIT_LIGHT29, m_iEditLight29);
	DDX_Text(pDX, IDC_EDIT_LIGHT30, m_iEditLight30);
	DDX_Text(pDX, IDC_EDIT_LIGHT31, m_iEditLight31);
	DDX_Text(pDX, IDC_EDIT_LIGHT32, m_iEditLight32);

	DDX_Text(pDX, IDC_EDIT_COM_PORT_NUMBER, m_iEditComPortNumber);

	DDX_Control(pDX, IDC_BUTTON_CHANGE_LIGHTVALUE, m_bnChangeLight);
	DDX_Control(pDX, IDCANCEL, m_bnExit);
	DDX_Control(pDX, IDC_BUTTON_SHOW_LIBRARY, m_bnShowLibrary);
}


BEGIN_MESSAGE_MAP(CLightPageControlDlg, CDialog)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_LIGHT1, &CLightPageControlDlg::OnChangeEditLight1)
	ON_EN_CHANGE(IDC_EDIT_LIGHT2, &CLightPageControlDlg::OnChangeEditLight2)
	ON_EN_CHANGE(IDC_EDIT_LIGHT3, &CLightPageControlDlg::OnChangeEditLight3)
	ON_EN_CHANGE(IDC_EDIT_LIGHT4, &CLightPageControlDlg::OnChangeEditLight4)
	ON_EN_CHANGE(IDC_EDIT_LIGHT5, &CLightPageControlDlg::OnChangeEditLight5)
	ON_EN_CHANGE(IDC_EDIT_LIGHT6, &CLightPageControlDlg::OnChangeEditLight6)
	ON_EN_CHANGE(IDC_EDIT_LIGHT7, &CLightPageControlDlg::OnChangeEditLight7)
	ON_EN_CHANGE(IDC_EDIT_LIGHT8, &CLightPageControlDlg::OnChangeEditLight8)
	ON_EN_CHANGE(IDC_EDIT_LIGHT9, &CLightPageControlDlg::OnChangeEditLight9)
	ON_EN_CHANGE(IDC_EDIT_LIGHT10, &CLightPageControlDlg::OnChangeEditLight10)
	ON_EN_CHANGE(IDC_EDIT_LIGHT11, &CLightPageControlDlg::OnChangeEditLight11)
	ON_EN_CHANGE(IDC_EDIT_LIGHT12, &CLightPageControlDlg::OnChangeEditLight12)
	ON_EN_CHANGE(IDC_EDIT_LIGHT13, &CLightPageControlDlg::OnChangeEditLight13)
	ON_EN_CHANGE(IDC_EDIT_LIGHT14, &CLightPageControlDlg::OnChangeEditLight14)
	ON_EN_CHANGE(IDC_EDIT_LIGHT15, &CLightPageControlDlg::OnChangeEditLight15)
	ON_EN_CHANGE(IDC_EDIT_LIGHT16, &CLightPageControlDlg::OnChangeEditLight16)
	ON_EN_CHANGE(IDC_EDIT_LIGHT17, &CLightPageControlDlg::OnChangeEditLight17)
	ON_EN_CHANGE(IDC_EDIT_LIGHT18, &CLightPageControlDlg::OnChangeEditLight18)
	ON_EN_CHANGE(IDC_EDIT_LIGHT19, &CLightPageControlDlg::OnChangeEditLight19)
	ON_EN_CHANGE(IDC_EDIT_LIGHT20, &CLightPageControlDlg::OnChangeEditLight20)
	ON_EN_CHANGE(IDC_EDIT_LIGHT21, &CLightPageControlDlg::OnChangeEditLight21)
	ON_EN_CHANGE(IDC_EDIT_LIGHT22, &CLightPageControlDlg::OnChangeEditLight22)
	ON_EN_CHANGE(IDC_EDIT_LIGHT23, &CLightPageControlDlg::OnChangeEditLight23)
	ON_EN_CHANGE(IDC_EDIT_LIGHT24, &CLightPageControlDlg::OnChangeEditLight24)
	ON_EN_CHANGE(IDC_EDIT_LIGHT25, &CLightPageControlDlg::OnChangeEditLight25)
	ON_EN_CHANGE(IDC_EDIT_LIGHT26, &CLightPageControlDlg::OnChangeEditLight26)
	ON_EN_CHANGE(IDC_EDIT_LIGHT27, &CLightPageControlDlg::OnChangeEditLight27)
	ON_EN_CHANGE(IDC_EDIT_LIGHT28, &CLightPageControlDlg::OnChangeEditLight28)
	ON_EN_CHANGE(IDC_EDIT_LIGHT29, &CLightPageControlDlg::OnChangeEditLight29)
	ON_EN_CHANGE(IDC_EDIT_LIGHT30, &CLightPageControlDlg::OnChangeEditLight30)
	ON_EN_CHANGE(IDC_EDIT_LIGHT31, &CLightPageControlDlg::OnChangeEditLight31)
	ON_EN_CHANGE(IDC_EDIT_LIGHT32, &CLightPageControlDlg::OnChangeEditLight32)

	ON_BN_CLICKED(IDCANCEL, &CLightPageControlDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CLightPageControlDlg::OnBnClickedButtonChangeLightvalue)
	ON_BN_CLICKED(IDC_RADIO_PAGE_1, &CLightPageControlDlg::OnBnClickedRadioPage1)
	ON_BN_CLICKED(IDC_RADIO_PAGE_2, &CLightPageControlDlg::OnBnClickedRadioPage2)
	ON_BN_CLICKED(IDC_RADIO_PAGE_3, &CLightPageControlDlg::OnBnClickedRadioPage3)
	ON_BN_CLICKED(IDC_RADIO_PAGE_4, &CLightPageControlDlg::OnBnClickedRadioPage4)
	ON_BN_CLICKED(IDC_RADIO_PAGE_5, &CLightPageControlDlg::OnBnClickedRadioPage5)
	ON_BN_CLICKED(IDC_RADIO_PAGE_6, &CLightPageControlDlg::OnBnClickedRadioPage6)
	ON_BN_CLICKED(IDC_RADIO_PAGE_7, &CLightPageControlDlg::OnBnClickedRadioPage7)
	ON_BN_CLICKED(IDC_RADIO_PAGE_8, &CLightPageControlDlg::OnBnClickedRadioPage8)
	ON_BN_CLICKED(IDC_RADIO_PAGE_9, &CLightPageControlDlg::OnBnClickedRadioPage9)
	ON_BN_CLICKED(IDC_RADIO_PAGE_10, &CLightPageControlDlg::OnBnClickedRadioPage10)
	ON_BN_CLICKED(IDC_RADIO_PAGE_11, &CLightPageControlDlg::OnBnClickedRadioPage11)
	ON_BN_CLICKED(IDC_RADIO_PAGE_12, &CLightPageControlDlg::OnBnClickedRadioPage12)
	ON_BN_CLICKED(IDC_RADIO_PAGE_13, &CLightPageControlDlg::OnBnClickedRadioPage13)
	ON_BN_CLICKED(IDC_RADIO_PAGE_14, &CLightPageControlDlg::OnBnClickedRadioPage14)
	ON_BN_CLICKED(IDC_RADIO_PAGE_15, &CLightPageControlDlg::OnBnClickedRadioPage15)
	ON_BN_CLICKED(IDC_RADIO_PAGE_16, &CLightPageControlDlg::OnBnClickedRadioPage16)
	ON_BN_CLICKED(IDC_RADIO_PAGE_17, &CLightPageControlDlg::OnBnClickedRadioPage17)
	ON_BN_CLICKED(IDC_RADIO_PAGE_18, &CLightPageControlDlg::OnBnClickedRadioPage18)
	ON_BN_CLICKED(IDC_RADIO_PAGE_19, &CLightPageControlDlg::OnBnClickedRadioPage19)
	ON_BN_CLICKED(IDC_RADIO_PAGE_20, &CLightPageControlDlg::OnBnClickedRadioPage20)
	ON_BN_CLICKED(IDC_RADIO_PAGE_21, &CLightPageControlDlg::OnBnClickedRadioPage21)
	ON_BN_CLICKED(IDC_RADIO_PAGE_22, &CLightPageControlDlg::OnBnClickedRadioPage22)
	ON_BN_CLICKED(IDC_RADIO_PAGE_23, &CLightPageControlDlg::OnBnClickedRadioPage23)
	ON_BN_CLICKED(IDC_RADIO_PAGE_24, &CLightPageControlDlg::OnBnClickedRadioPage24)
	ON_BN_CLICKED(IDC_RADIO_PAGE_25, &CLightPageControlDlg::OnBnClickedRadioPage25)
	ON_BN_CLICKED(IDC_RADIO_PAGE_26, &CLightPageControlDlg::OnBnClickedRadioPage26)
	ON_BN_CLICKED(IDC_RADIO_PAGE_27, &CLightPageControlDlg::OnBnClickedRadioPage27)
	ON_BN_CLICKED(IDC_RADIO_PAGE_28, &CLightPageControlDlg::OnBnClickedRadioPage28)
	ON_BN_CLICKED(IDC_RADIO_PAGE_29, &CLightPageControlDlg::OnBnClickedRadioPage29)
	ON_BN_CLICKED(IDC_RADIO_PAGE_30, &CLightPageControlDlg::OnBnClickedRadioPage30)
	ON_BN_CLICKED(IDC_RADIO_PAGE_31, &CLightPageControlDlg::OnBnClickedRadioPage31)
	ON_BN_CLICKED(IDC_RADIO_PAGE_32, &CLightPageControlDlg::OnBnClickedRadioPage32)

	ON_BN_CLICKED(IDC_BUTTON_SHOW_LIBRARY, &CLightPageControlDlg::OnBnClickedButtonShowLibrary)

	ON_MESSAGE(UM_LIGHT_LIST_APPLY, OnEventSelectedListApply)
END_MESSAGE_MAP()

// CLightPageControlDlg 메시지 처리기입니다.

BOOL CLightPageControlDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CLightPageControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pLightLibraryDlg = CLightLibraryDlg::GetInstance();
	HideLightLibraryDlg();

	m_pLightLibraryDlg->delegateSelectedListApply += new CEventStub(this, UM_LIGHT_LIST_APPLY);

	//////////////////////////////////////////////////////////////////////////
	/// 조명 채널명 
	CString strChNameFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LightChannel.ini";
	CIniFileCS INI_LIGHTCH(strChNameFileName);
	CString sSection, sKey;
	sSection = THEAPP.m_ModelDefineInfo.m_strVisionName[THEAPP.m_iCurTeachVision];

	CString sLightChannelName;
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "1", "미사용");
	GetDlgItem(IDC_STATIC_CH1)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "2", "미사용");
	GetDlgItem(IDC_STATIC_CH2)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "3", "미사용");
	GetDlgItem(IDC_STATIC_CH3)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "4", "미사용");
	GetDlgItem(IDC_STATIC_CH4)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "5", "미사용");
	GetDlgItem(IDC_STATIC_CH5)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "6", "미사용");
	GetDlgItem(IDC_STATIC_CH6)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "7", "미사용");
	GetDlgItem(IDC_STATIC_CH7)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "8", "미사용");
	GetDlgItem(IDC_STATIC_CH8)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "9", "미사용");
	GetDlgItem(IDC_STATIC_CH9)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "10", "미사용");
	GetDlgItem(IDC_STATIC_CH10)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "11", "미사용");
	GetDlgItem(IDC_STATIC_CH11)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "12", "미사용");
	GetDlgItem(IDC_STATIC_CH12)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "13", "미사용");
	GetDlgItem(IDC_STATIC_CH13)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "14", "미사용");
	GetDlgItem(IDC_STATIC_CH14)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "15", "미사용");
	GetDlgItem(IDC_STATIC_CH15)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "16", "미사용");

	GetDlgItem(IDC_STATIC_CH16)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "17", "미사용");
	GetDlgItem(IDC_STATIC_CH17)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "18", "미사용");
	GetDlgItem(IDC_STATIC_CH18)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "19", "미사용");
	GetDlgItem(IDC_STATIC_CH19)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "20", "미사용");
	GetDlgItem(IDC_STATIC_CH20)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "21", "미사용");
	GetDlgItem(IDC_STATIC_CH21)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "22", "미사용");
	GetDlgItem(IDC_STATIC_CH22)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "23", "미사용");
	GetDlgItem(IDC_STATIC_CH23)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "24", "미사용");
	GetDlgItem(IDC_STATIC_CH24)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "25", "미사용");
	GetDlgItem(IDC_STATIC_CH25)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "26", "미사용");
	GetDlgItem(IDC_STATIC_CH26)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "27", "미사용");
	GetDlgItem(IDC_STATIC_CH27)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "28", "미사용");
	GetDlgItem(IDC_STATIC_CH28)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "29", "미사용");
	GetDlgItem(IDC_STATIC_CH29)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "30", "미사용");
	GetDlgItem(IDC_STATIC_CH30)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "31", "미사용");
	GetDlgItem(IDC_STATIC_CH31)->SetWindowTextA(sLightChannelName);
	sLightChannelName = INI_LIGHTCH.Get_String(sSection, "32", "미사용");
	GetDlgItem(IDC_STATIC_CH32)->SetWindowTextA(sLightChannelName);

	//////////////////////////////////////////////////////////////////////////

	m_bnChangeLight.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0xB5, 0xC4, 0x91), 0, 0);
	m_bnExit.Init_Ctrl(_T("Arial"), 10, FALSE, RGB(0x00, 0x00, 0x00), RGB(0x86, 0xAC, 0xE3), 0, 0);
	m_bnShowLibrary.Init_Ctrl(_T("Malgun Gothic"), 10, TRUE, RGB(0x00, 0x00, 0x00), RGB(226, 226, 226), 0, 0);

#ifdef ASSY_LENS
	if (THEAPP.m_iCurTeachVision == VISION_NUMBER_4 && THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_TOP)
		m_iEditComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iSubComPortNumber;
#else
	m_iEditComPortNumber = THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber;
#endif

	m_sEditPageDesc = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].m_sPageDesc;

	m_SliderLight1.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight2.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight3.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight4.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight5.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight6.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight7.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight8.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight9.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight10.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight11.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight12.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight13.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight14.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight15.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight16.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight17.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight18.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight19.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight20.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight21.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight22.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight23.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight24.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight25.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight26.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight27.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight28.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight29.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight30.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight31.SetRange(0, LIGHT_BRIGHT_MAX);
	m_SliderLight32.SetRange(0, LIGHT_BRIGHT_MAX);

	m_SliderLight1.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0]);
	m_SliderLight2.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1]);
	m_SliderLight3.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2]);
	m_SliderLight4.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3]);
	m_SliderLight5.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4]);
	m_SliderLight6.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5]);
	m_SliderLight7.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6]);
	m_SliderLight8.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7]);
	m_SliderLight9.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8]);
	m_SliderLight10.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9]);
	m_SliderLight11.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10]);
	m_SliderLight12.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11]);
	m_SliderLight13.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12]);
	m_SliderLight14.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13]);
	m_SliderLight15.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14]);
	m_SliderLight16.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15]);
	m_SliderLight17.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16]);
	m_SliderLight18.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17]);
	m_SliderLight19.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18]);
	m_SliderLight20.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19]);
	m_SliderLight21.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20]);
	m_SliderLight22.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21]);
	m_SliderLight23.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22]);
	m_SliderLight24.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23]);
	m_SliderLight25.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24]);
	m_SliderLight26.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25]);
	m_SliderLight27.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26]);
	m_SliderLight28.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27]);
	m_SliderLight29.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28]);
	m_SliderLight30.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29]);
	m_SliderLight31.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30]);
	m_SliderLight32.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31]);

	m_iEditLight1 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0];
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1];
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2];
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3];
	m_iEditLight5 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4];
	m_iEditLight6 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5];
	m_iEditLight7 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6];
	m_iEditLight8 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7];
	m_iEditLight9 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8];
	m_iEditLight10 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9];
	m_iEditLight11 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10];
	m_iEditLight12 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11];
	m_iEditLight13 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12];
	m_iEditLight14 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13];
	m_iEditLight15 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14];
	m_iEditLight16 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15];
	m_iEditLight17 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16];
	m_iEditLight18 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17];
	m_iEditLight19 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18];
	m_iEditLight20 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19];
	m_iEditLight21 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20];
	m_iEditLight22 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21];
	m_iEditLight23 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22];
	m_iEditLight24 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23];
	m_iEditLight25 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24];
	m_iEditLight26 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25];
	m_iEditLight27 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26];
	m_iEditLight28 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27];
	m_iEditLight29 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28];
	m_iEditLight30 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29];
	m_iEditLight31 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30];
	m_iEditLight32 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31];

	ChangeLanguage();

#if defined(SINGLE_LENS) || defined(ASSY_LENS)
	SetDlgStatus();
#endif

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightPageControlDlg::SetDlgStatus()
{
	GetDlgItem(IDC_STATIC_PORT_NAME)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_COM_PORT_NUMBER)->ShowWindow(TRUE);

	GetDlgItem(IDC_RADIO_PAGE_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_2)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_3)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_4)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_5)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_6)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_7)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_8)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_9)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_10)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_11)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_12)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_13)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_14)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_15)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_16)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_17)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_18)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_19)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_20)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_21)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_22)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_23)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_24)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_25)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_26)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_27)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_28)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_29)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_30)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_31)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PAGE_32)->EnableWindow(TRUE);

	if (THEAPP.m_iCurTeachVision == VISION_NUMBER_1 || THEAPP.m_iCurTeachVision == VISION_NUMBER_2)
	{
		GetDlgItem(IDC_RADIO_PAGE_13)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_14)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_15)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_16)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_17)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_18)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_19)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_20)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_21)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_22)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_23)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_24)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_25)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_26)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_27)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_28)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_29)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_30)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_31)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_32)->EnableWindow(FALSE);
	}
	else if (THEAPP.m_iCurTeachVision == VISION_NUMBER_3)
	{
		GetDlgItem(IDC_RADIO_PAGE_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_12)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_13)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_14)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_15)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_16)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_17)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_18)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_19)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_20)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_21)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_22)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_23)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_24)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_25)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_26)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_27)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_28)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_29)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_30)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_31)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PAGE_32)->EnableWindow(FALSE);
	}
	else if (THEAPP.m_iCurTeachVision == VISION_NUMBER_4)
	{
		if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_1 || THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_2)
		{
			GetDlgItem(IDC_RADIO_PAGE_4)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_5)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_6)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_7)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_8)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_9)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_10)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_11)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_12)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_13)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_14)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_15)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_16)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_17)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_18)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_19)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_20)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_21)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_22)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_23)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_24)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_25)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_26)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_27)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_28)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_29)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_30)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_31)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_32)->EnableWindow(FALSE);
		}
		else if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_TOP)
		{
			m_iRadioPageNumber = 3;

			GetDlgItem(IDC_RADIO_PAGE_1)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_2)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_3)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_6)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_7)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_8)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_9)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_10)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_11)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_12)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_13)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_14)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_15)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_16)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_17)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_18)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_19)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_20)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_21)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_22)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_23)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_24)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_25)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_26)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_27)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_28)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_29)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_30)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_31)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_PAGE_32)->EnableWindow(FALSE);
		}
	}
}

void CLightPageControlDlg::ChangeLanguage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		this->SetWindowText(_T("조명 페이지 설정"));
		SetDlgItemText(IDC_STATIC_LIGHTPAGECONTROLDLG_1, _T("조명 설명:"));

		int piUsedImageNumber[50], piUsedSequenceNumber[50], iNoUsedImage;
		CString sPageUsedImageInfo;

		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      영상 %d(주소 %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, 영상 %d(주소 %d), ", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      미사용");
		GetDlgItem(IDC_STATIC_PAGE_USED_IMAGE_INFO)->SetWindowTextA(sPageUsedImageInfo);

		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_1, _T("조명 선택"));
		SetDlgItemText(IDC_RADIO_PAGE_1, _T("페이지 1"));
		SetDlgItemText(IDC_RADIO_PAGE_2, _T("페이지 2"));
		SetDlgItemText(IDC_RADIO_PAGE_3, _T("페이지 3"));
		SetDlgItemText(IDC_RADIO_PAGE_4, _T("페이지 4"));
		SetDlgItemText(IDC_RADIO_PAGE_5, _T("페이지 5"));
		SetDlgItemText(IDC_RADIO_PAGE_6, _T("페이지 6"));
		SetDlgItemText(IDC_RADIO_PAGE_7, _T("페이지 7"));
		SetDlgItemText(IDC_RADIO_PAGE_8, _T("페이지 8"));
		SetDlgItemText(IDC_RADIO_PAGE_9, _T("페이지 9"));
		SetDlgItemText(IDC_RADIO_PAGE_10, _T("페이지 10"));
		SetDlgItemText(IDC_RADIO_PAGE_11, _T("페이지 11"));
		SetDlgItemText(IDC_RADIO_PAGE_12, _T("페이지 12"));
		SetDlgItemText(IDC_RADIO_PAGE_13, _T("페이지 13"));
		SetDlgItemText(IDC_RADIO_PAGE_14, _T("페이지 14"));
		SetDlgItemText(IDC_RADIO_PAGE_15, _T("페이지 15"));
		SetDlgItemText(IDC_RADIO_PAGE_16, _T("페이지 16"));
		SetDlgItemText(IDC_RADIO_PAGE_17, _T("페이지 17"));
		SetDlgItemText(IDC_RADIO_PAGE_18, _T("페이지 18"));
		SetDlgItemText(IDC_RADIO_PAGE_19, _T("페이지 19"));
		SetDlgItemText(IDC_RADIO_PAGE_20, _T("페이지 20"));
		SetDlgItemText(IDC_RADIO_PAGE_21, _T("페이지 21"));
		SetDlgItemText(IDC_RADIO_PAGE_22, _T("페이지 22"));
		SetDlgItemText(IDC_RADIO_PAGE_23, _T("페이지 23"));
		SetDlgItemText(IDC_RADIO_PAGE_24, _T("페이지 24"));
		SetDlgItemText(IDC_RADIO_PAGE_25, _T("페이지 25"));
		SetDlgItemText(IDC_RADIO_PAGE_26, _T("페이지 26"));
		SetDlgItemText(IDC_RADIO_PAGE_27, _T("페이지 27"));
		SetDlgItemText(IDC_RADIO_PAGE_28, _T("페이지 28"));
		SetDlgItemText(IDC_RADIO_PAGE_29, _T("페이지 29"));
		SetDlgItemText(IDC_RADIO_PAGE_30, _T("페이지 30"));
		SetDlgItemText(IDC_RADIO_PAGE_31, _T("페이지 31"));
		SetDlgItemText(IDC_RADIO_PAGE_32, _T("페이지 32"));
		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_2, _T("채널값 설정(1 - 16)"));
		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_3, _T("채널값 설정(17 - 32)"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("조명 밝기 변경"));
		SetDlgItemText(IDCANCEL, _T("닫기"));
		SetDlgItemText(IDC_BUTTON_SHOW_LIBRARY, _T("조명 라이브러리"));
	}
	else
	{
		this->SetWindowText(_T("Set the light page"));
		SetDlgItemText(IDC_STATIC_LIGHTPAGECONTROLDLG_1, _T("Page description:"));

		int piUsedImageNumber[50], piUsedSequenceNumber[50], iNoUsedImage;
		CString sPageUsedImageInfo;

		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("Image in used(Sequence):   Image %d(Addr %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, Image %d(Addr %d)", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("Image in used(Sequence):   Unused");
		GetDlgItem(IDC_STATIC_PAGE_USED_IMAGE_INFO)->SetWindowTextA(sPageUsedImageInfo);

		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_1, _T("Select page"));
		SetDlgItemText(IDC_RADIO_PAGE_1, _T("Page 1"));
		SetDlgItemText(IDC_RADIO_PAGE_2, _T("Page 2"));
		SetDlgItemText(IDC_RADIO_PAGE_3, _T("Page 3"));
		SetDlgItemText(IDC_RADIO_PAGE_4, _T("Page 4"));
		SetDlgItemText(IDC_RADIO_PAGE_5, _T("Page 5"));
		SetDlgItemText(IDC_RADIO_PAGE_6, _T("Page 6"));
		SetDlgItemText(IDC_RADIO_PAGE_7, _T("Page 7"));
		SetDlgItemText(IDC_RADIO_PAGE_8, _T("Page 8"));
		SetDlgItemText(IDC_RADIO_PAGE_9, _T("Page 9"));
		SetDlgItemText(IDC_RADIO_PAGE_10, _T("Page 10"));
		SetDlgItemText(IDC_RADIO_PAGE_11, _T("Page 11"));
		SetDlgItemText(IDC_RADIO_PAGE_12, _T("Page 12"));
		SetDlgItemText(IDC_RADIO_PAGE_13, _T("Page 13"));
		SetDlgItemText(IDC_RADIO_PAGE_14, _T("Page 14"));
		SetDlgItemText(IDC_RADIO_PAGE_15, _T("Page 15"));
		SetDlgItemText(IDC_RADIO_PAGE_16, _T("Page 16"));
		SetDlgItemText(IDC_RADIO_PAGE_17, _T("Page 17"));
		SetDlgItemText(IDC_RADIO_PAGE_18, _T("Page 18"));
		SetDlgItemText(IDC_RADIO_PAGE_19, _T("Page 19"));
		SetDlgItemText(IDC_RADIO_PAGE_20, _T("Page 20"));
		SetDlgItemText(IDC_RADIO_PAGE_21, _T("Page 21"));
		SetDlgItemText(IDC_RADIO_PAGE_22, _T("Page 22"));
		SetDlgItemText(IDC_RADIO_PAGE_23, _T("Page 23"));
		SetDlgItemText(IDC_RADIO_PAGE_24, _T("Page 24"));
		SetDlgItemText(IDC_RADIO_PAGE_25, _T("Page 25"));
		SetDlgItemText(IDC_RADIO_PAGE_26, _T("Page 26"));
		SetDlgItemText(IDC_RADIO_PAGE_27, _T("Page 27"));
		SetDlgItemText(IDC_RADIO_PAGE_28, _T("Page 28"));
		SetDlgItemText(IDC_RADIO_PAGE_29, _T("Page 29"));
		SetDlgItemText(IDC_RADIO_PAGE_30, _T("Page 30"));
		SetDlgItemText(IDC_RADIO_PAGE_31, _T("Page 31"));
		SetDlgItemText(IDC_RADIO_PAGE_32, _T("Page 32"));
		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_2, _T("Set the channel value (1 - 16)"));
		SetDlgItemText(IDC_GROUPBOX_LIGHTPAGECONTROLDLG_3, _T("Set the channel value (17 - 32)"));
		SetDlgItemText(IDC_BUTTON_CHANGE_LIGHTVALUE, _T("Change the page"));
		SetDlgItemText(IDCANCEL, _T("Close"));
		SetDlgItemText(IDC_BUTTON_SHOW_LIBRARY, _T("Light library"));
	}
}


void CLightPageControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;

	switch (pSlider->GetDlgCtrlID())
	{
	case IDC_SLIDER_LIGHT1:
		m_iEditLight1 = m_SliderLight1.GetPos();
		break;

	case IDC_SLIDER_LIGHT2:
		m_iEditLight2 = m_SliderLight2.GetPos();
		break;

	case IDC_SLIDER_LIGHT3:
		m_iEditLight3 = m_SliderLight3.GetPos();
		break;

	case IDC_SLIDER_LIGHT4:
		m_iEditLight4 = m_SliderLight4.GetPos();
		break;

	case IDC_SLIDER_LIGHT5:
		m_iEditLight5 = m_SliderLight5.GetPos();
		break;

	case IDC_SLIDER_LIGHT6:
		m_iEditLight6 = m_SliderLight6.GetPos();
		break;

	case IDC_SLIDER_LIGHT7:
		m_iEditLight7 = m_SliderLight7.GetPos();
		break;

	case IDC_SLIDER_LIGHT8:
		m_iEditLight8 = m_SliderLight8.GetPos();
		break;

	case IDC_SLIDER_LIGHT9:
		m_iEditLight9 = m_SliderLight9.GetPos();
		break;

	case IDC_SLIDER_LIGHT10:
		m_iEditLight10 = m_SliderLight10.GetPos();
		break;

	case IDC_SLIDER_LIGHT11:
		m_iEditLight11 = m_SliderLight11.GetPos();
		break;

	case IDC_SLIDER_LIGHT12:
		m_iEditLight12 = m_SliderLight12.GetPos();
		break;

	case IDC_SLIDER_LIGHT13:
		m_iEditLight13 = m_SliderLight13.GetPos();
		break;

	case IDC_SLIDER_LIGHT14:
		m_iEditLight14 = m_SliderLight14.GetPos();
		break;

	case IDC_SLIDER_LIGHT15:
		m_iEditLight15 = m_SliderLight15.GetPos();
		break;

	case IDC_SLIDER_LIGHT16:
		m_iEditLight16 = m_SliderLight16.GetPos();
		break;

	case IDC_SLIDER_LIGHT17:
		m_iEditLight17 = m_SliderLight17.GetPos();
		break;

	case IDC_SLIDER_LIGHT18:
		m_iEditLight18 = m_SliderLight18.GetPos();
		break;

	case IDC_SLIDER_LIGHT19:
		m_iEditLight19 = m_SliderLight19.GetPos();
		break;

	case IDC_SLIDER_LIGHT20:
		m_iEditLight20 = m_SliderLight20.GetPos();
		break;

	case IDC_SLIDER_LIGHT21:
		m_iEditLight21 = m_SliderLight21.GetPos();
		break;

	case IDC_SLIDER_LIGHT22:
		m_iEditLight22 = m_SliderLight22.GetPos();
		break;

	case IDC_SLIDER_LIGHT23:
		m_iEditLight23 = m_SliderLight23.GetPos();
		break;

	case IDC_SLIDER_LIGHT24:
		m_iEditLight24 = m_SliderLight24.GetPos();
		break;

	case IDC_SLIDER_LIGHT25:
		m_iEditLight25 = m_SliderLight25.GetPos();
		break;

	case IDC_SLIDER_LIGHT26:
		m_iEditLight26 = m_SliderLight26.GetPos();
		break;

	case IDC_SLIDER_LIGHT27:
		m_iEditLight27 = m_SliderLight27.GetPos();
		break;

	case IDC_SLIDER_LIGHT28:
		m_iEditLight28 = m_SliderLight28.GetPos();
		break;

	case IDC_SLIDER_LIGHT29:
		m_iEditLight29 = m_SliderLight29.GetPos();
		break;

	case IDC_SLIDER_LIGHT30:
		m_iEditLight30 = m_SliderLight30.GetPos();
		break;

	case IDC_SLIDER_LIGHT31:
		m_iEditLight31 = m_SliderLight31.GetPos();
		break;

	case IDC_SLIDER_LIGHT32:
		m_iEditLight32 = m_SliderLight32.GetPos();
		break;

	default:
		break;

	}

	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLightPageControlDlg::OnChangeEditLight1()
{
	UpdateData(TRUE);
	m_SliderLight1.SetPos(m_iEditLight1);
}


void CLightPageControlDlg::OnChangeEditLight2()
{
	UpdateData(TRUE);
	m_SliderLight2.SetPos(m_iEditLight2);
}


void CLightPageControlDlg::OnChangeEditLight3()
{
	UpdateData(TRUE);
	m_SliderLight3.SetPos(m_iEditLight3);
}


void CLightPageControlDlg::OnChangeEditLight4()
{
	UpdateData(TRUE);
	m_SliderLight4.SetPos(m_iEditLight4);
}

void CLightPageControlDlg::OnChangeEditLight5()
{
	UpdateData(TRUE);
	m_SliderLight5.SetPos(m_iEditLight5);
}

void CLightPageControlDlg::OnChangeEditLight6()
{
	UpdateData(TRUE);
	m_SliderLight6.SetPos(m_iEditLight6);
}

void CLightPageControlDlg::OnChangeEditLight7()
{
	UpdateData(TRUE);
	m_SliderLight7.SetPos(m_iEditLight7);
}

void CLightPageControlDlg::OnChangeEditLight8()
{
	UpdateData(TRUE);
	m_SliderLight8.SetPos(m_iEditLight8);
}

void CLightPageControlDlg::OnChangeEditLight9()
{
	UpdateData(TRUE);
	m_SliderLight9.SetPos(m_iEditLight9);
}

void CLightPageControlDlg::OnChangeEditLight10()
{
	UpdateData(TRUE);
	m_SliderLight10.SetPos(m_iEditLight10);
}

void CLightPageControlDlg::OnChangeEditLight11()
{
	UpdateData(TRUE);
	m_SliderLight11.SetPos(m_iEditLight11);
}

void CLightPageControlDlg::OnChangeEditLight12()
{
	UpdateData(TRUE);
	m_SliderLight12.SetPos(m_iEditLight12);
}

void CLightPageControlDlg::OnChangeEditLight13()
{
	UpdateData(TRUE);
	m_SliderLight13.SetPos(m_iEditLight13);
}

void CLightPageControlDlg::OnChangeEditLight14()
{
	UpdateData(TRUE);
	m_SliderLight14.SetPos(m_iEditLight14);
}

void CLightPageControlDlg::OnChangeEditLight15()
{
	UpdateData(TRUE);
	m_SliderLight15.SetPos(m_iEditLight15);
}

void CLightPageControlDlg::OnChangeEditLight16()
{
	UpdateData(TRUE);
	m_SliderLight16.SetPos(m_iEditLight16);
}

void CLightPageControlDlg::OnChangeEditLight17()
{
	UpdateData(TRUE);
	m_SliderLight17.SetPos(m_iEditLight17);
}

void CLightPageControlDlg::OnChangeEditLight18()
{
	UpdateData(TRUE);
	m_SliderLight18.SetPos(m_iEditLight18);
}

void CLightPageControlDlg::OnChangeEditLight19()
{
	UpdateData(TRUE);
	m_SliderLight19.SetPos(m_iEditLight19);
}

void CLightPageControlDlg::OnChangeEditLight20()
{
	UpdateData(TRUE);
	m_SliderLight20.SetPos(m_iEditLight20);
}

void CLightPageControlDlg::OnChangeEditLight21()
{
	UpdateData(TRUE);
	m_SliderLight21.SetPos(m_iEditLight21);
}

void CLightPageControlDlg::OnChangeEditLight22()
{
	UpdateData(TRUE);
	m_SliderLight22.SetPos(m_iEditLight22);
}

void CLightPageControlDlg::OnChangeEditLight23()
{
	UpdateData(TRUE);
	m_SliderLight23.SetPos(m_iEditLight23);
}

void CLightPageControlDlg::OnChangeEditLight24()
{
	UpdateData(TRUE);
	m_SliderLight24.SetPos(m_iEditLight24);
}

void CLightPageControlDlg::OnChangeEditLight25()
{
	UpdateData(TRUE);
	m_SliderLight25.SetPos(m_iEditLight25);
}

void CLightPageControlDlg::OnChangeEditLight26()
{
	UpdateData(TRUE);
	m_SliderLight26.SetPos(m_iEditLight26);
}

void CLightPageControlDlg::OnChangeEditLight27()
{
	UpdateData(TRUE);
	m_SliderLight27.SetPos(m_iEditLight27);
}

void CLightPageControlDlg::OnChangeEditLight28()
{
	UpdateData(TRUE);
	m_SliderLight28.SetPos(m_iEditLight28);
}

void CLightPageControlDlg::OnChangeEditLight29()
{
	UpdateData(TRUE);
	m_SliderLight29.SetPos(m_iEditLight29);
}

void CLightPageControlDlg::OnChangeEditLight30()
{
	UpdateData(TRUE);
	m_SliderLight30.SetPos(m_iEditLight30);
}

void CLightPageControlDlg::OnChangeEditLight31()
{
	UpdateData(TRUE);
	m_SliderLight31.SetPos(m_iEditLight31);
}

void CLightPageControlDlg::OnChangeEditLight32()
{
	UpdateData(TRUE);
	m_SliderLight32.SetPos(m_iEditLight32);
}

void CLightPageControlDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CLightPageControlDlg::OnBnClickedButtonChangeLightvalue()
{
	UpdateData(TRUE);

	if (m_iEditLight1 < 0) m_iEditLight1 = 0; if (m_iEditLight1 > LIGHT_BRIGHT_MAX) m_iEditLight1 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight2 < 0) m_iEditLight2 = 0; if (m_iEditLight2 > LIGHT_BRIGHT_MAX) m_iEditLight2 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight3 < 0) m_iEditLight3 = 0; if (m_iEditLight3 > LIGHT_BRIGHT_MAX) m_iEditLight3 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight4 < 0) m_iEditLight4 = 0; if (m_iEditLight4 > LIGHT_BRIGHT_MAX) m_iEditLight4 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight5 < 0) m_iEditLight5 = 0; if (m_iEditLight5 > LIGHT_BRIGHT_MAX) m_iEditLight5 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight6 < 0) m_iEditLight6 = 0; if (m_iEditLight6 > LIGHT_BRIGHT_MAX) m_iEditLight6 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight7 < 0) m_iEditLight7 = 0; if (m_iEditLight7 > LIGHT_BRIGHT_MAX) m_iEditLight7 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight8 < 0) m_iEditLight8 = 0; if (m_iEditLight8 > LIGHT_BRIGHT_MAX) m_iEditLight8 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight9 < 0) m_iEditLight9 = 0; if (m_iEditLight9 > LIGHT_BRIGHT_MAX) m_iEditLight9 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight10 < 0) m_iEditLight10 = 0; if (m_iEditLight10 > LIGHT_BRIGHT_MAX) m_iEditLight10 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight11 < 0) m_iEditLight11 = 0; if (m_iEditLight11 > LIGHT_BRIGHT_MAX) m_iEditLight11 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight12 < 0) m_iEditLight12 = 0; if (m_iEditLight12 > LIGHT_BRIGHT_MAX) m_iEditLight12 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight13 < 0) m_iEditLight13 = 0; if (m_iEditLight13 > LIGHT_BRIGHT_MAX) m_iEditLight13 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight14 < 0) m_iEditLight14 = 0; if (m_iEditLight14 > LIGHT_BRIGHT_MAX) m_iEditLight14 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight15 < 0) m_iEditLight15 = 0; if (m_iEditLight15 > LIGHT_BRIGHT_MAX) m_iEditLight15 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight16 < 0) m_iEditLight16 = 0; if (m_iEditLight16 > LIGHT_BRIGHT_MAX) m_iEditLight16 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight17 < 0) m_iEditLight17 = 0; if (m_iEditLight17 > LIGHT_BRIGHT_MAX) m_iEditLight17 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight18 < 0) m_iEditLight18 = 0; if (m_iEditLight18 > LIGHT_BRIGHT_MAX) m_iEditLight18 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight19 < 0) m_iEditLight19 = 0; if (m_iEditLight19 > LIGHT_BRIGHT_MAX) m_iEditLight19 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight20 < 0) m_iEditLight20 = 0; if (m_iEditLight20 > LIGHT_BRIGHT_MAX) m_iEditLight20 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight21 < 0) m_iEditLight21 = 0; if (m_iEditLight21 > LIGHT_BRIGHT_MAX) m_iEditLight21 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight22 < 0) m_iEditLight22 = 0; if (m_iEditLight22 > LIGHT_BRIGHT_MAX) m_iEditLight22 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight23 < 0) m_iEditLight23 = 0; if (m_iEditLight23 > LIGHT_BRIGHT_MAX) m_iEditLight23 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight24 < 0) m_iEditLight24 = 0; if (m_iEditLight24 > LIGHT_BRIGHT_MAX) m_iEditLight24 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight25 < 0) m_iEditLight25 = 0; if (m_iEditLight25 > LIGHT_BRIGHT_MAX) m_iEditLight25 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight26 < 0) m_iEditLight26 = 0; if (m_iEditLight26 > LIGHT_BRIGHT_MAX) m_iEditLight26 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight27 < 0) m_iEditLight27 = 0; if (m_iEditLight27 > LIGHT_BRIGHT_MAX) m_iEditLight27 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight28 < 0) m_iEditLight28 = 0; if (m_iEditLight28 > LIGHT_BRIGHT_MAX) m_iEditLight28 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight29 < 0) m_iEditLight29 = 0; if (m_iEditLight29 > LIGHT_BRIGHT_MAX) m_iEditLight29 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight30 < 0) m_iEditLight30 = 0; if (m_iEditLight30 > LIGHT_BRIGHT_MAX) m_iEditLight30 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight31 < 0) m_iEditLight31 = 0; if (m_iEditLight31 > LIGHT_BRIGHT_MAX) m_iEditLight31 = LIGHT_BRIGHT_MAX;
	if (m_iEditLight32 < 0) m_iEditLight32 = 0; if (m_iEditLight32 > LIGHT_BRIGHT_MAX) m_iEditLight32 = LIGHT_BRIGHT_MAX;

#ifdef ASSY_LENS
	if (THEAPP.m_iCurTeachVision == VISION_NUMBER_4 && THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_TOP)
		THEAPP.m_pModelDataManager->m_PageControlData.m_iSubComPortNumber = m_iEditComPortNumber;
#else
	THEAPP.m_pModelDataManager->m_PageControlData.m_iComPortNumber = m_iEditComPortNumber;
#endif

	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0] = m_iEditLight1;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1] = m_iEditLight2;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2] = m_iEditLight3;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3] = m_iEditLight4;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4] = m_iEditLight5;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5] = m_iEditLight6;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6] = m_iEditLight7;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7] = m_iEditLight8;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8] = m_iEditLight9;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9] = m_iEditLight10;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10] = m_iEditLight11;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11] = m_iEditLight12;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12] = m_iEditLight13;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13] = m_iEditLight14;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14] = m_iEditLight15;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15] = m_iEditLight16;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16] = m_iEditLight17;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17] = m_iEditLight18;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18] = m_iEditLight19;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19] = m_iEditLight20;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20] = m_iEditLight21;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21] = m_iEditLight22;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22] = m_iEditLight23;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23] = m_iEditLight24;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24] = m_iEditLight25;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25] = m_iEditLight26;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26] = m_iEditLight27;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27] = m_iEditLight28;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28] = m_iEditLight29;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29] = m_iEditLight30;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30] = m_iEditLight31;
	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31] = m_iEditLight32;

	THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].m_sPageDesc = m_sEditPageDesc;

#ifdef INLINE_MODE

#ifdef SINGLE_LENS

	int iPageIdx;

	if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(m_iEditComPortNumber, 19200))
	{
		THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_8CH(m_iCurPageIndex);
		THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();
	}
	else
	{
		AfxMessageBox("통신 포트 열기 실패!!!. 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
	}

#elif defined(ASSY_LENS)

	int iPageIdx;

	if (THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.OpenPort(m_iEditComPortNumber, 19200))
	{
		if (THEAPP.m_iCurTeachVision == VISION_NUMBER_1 || THEAPP.m_iCurTeachVision == VISION_NUMBER_2)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_8CH(m_iCurPageIndex);
		}
		else if (THEAPP.m_iCurTeachVision == VISION_NUMBER_3)
		{
			THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_4CH(m_iCurPageIndex);
		}
		else if (THEAPP.m_iCurTeachVision == VISION_NUMBER_4)
		{
			if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_1 || THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_BOTTOM_2)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_6CH(m_iCurPageIndex);
			}
			else if (THEAPP.m_iCurAlignCamIndex == ALIGN_CAM_TOP)
			{
				THEAPP.m_pModelDataManager->m_PageControlData.SetIllumination_2CH(m_iCurPageIndex);
			}
		}

		THEAPP.m_pModelDataManager->m_PageControlData.m_ComPort.ClosePort();
	}
	else
	{
		AfxMessageBox("통신 포트 열기 실패!!!. 조명 컨트롤러 포트번호를 확인해 주십시요.", MB_ICONERROR | MB_SYSTEMMODAL);
	}

#else
	THEAPP.m_pModelDataManager->m_PageControlData.SetOnTime_Page(THEAPP.m_iCurVisionCamType, m_iCurPageIndex);
#endif

#endif

	//**********  조명 값을 파일에 써줌
	CString strOpticModelFolder;
	strOpticModelFolder.Format("%s\\Optical\\%s_%s_PC%d", THEAPP.GetWorkingDirectory(), THEAPP.Struct_PreferenceStruct.m_strEquipNo, THEAPP.g_strModelTypeName[THEAPP.GetModelType()], THEAPP.Struct_PreferenceStruct.m_iPCType + 1);
	if (GetFileAttributes(strOpticModelFolder) == -1)
		return;

	CString strInspectLightInfo;
	strInspectLightInfo.Format("%s\\HW\\Vision_N%d\\InspectLightInfo.ini", strOpticModelFolder, THEAPP.m_pModelDataManager->GetModelIdx() + 1);

	THEAPP.m_pModelDataManager->SaveLightInfo(strInspectLightInfo, m_iCurPageIndex);

	THEAPP.m_pTabControlDlg->m_pCameraLightManagerDlg->LoadViewParam();
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();
}


void CLightPageControlDlg::OnBnClickedRadioPage1()
{
	m_iCurPageIndex = 0;

	UpdateLightChannelValue();
}


void CLightPageControlDlg::OnBnClickedRadioPage2()
{
	m_iCurPageIndex = 1;

	UpdateLightChannelValue();
}


void CLightPageControlDlg::OnBnClickedRadioPage3()
{
	m_iCurPageIndex = 2;

	UpdateLightChannelValue();
}


void CLightPageControlDlg::OnBnClickedRadioPage4()
{
	m_iCurPageIndex = 3;

	UpdateLightChannelValue();
}


void CLightPageControlDlg::OnBnClickedRadioPage5()
{
	m_iCurPageIndex = 4;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage6()
{
	m_iCurPageIndex = 5;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage7()
{
	m_iCurPageIndex = 6;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage8()
{
	m_iCurPageIndex = 7;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage9()
{
	m_iCurPageIndex = 8;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage10()
{
	m_iCurPageIndex = 9;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage11()
{
	m_iCurPageIndex = 10;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage12()
{
	m_iCurPageIndex = 11;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage13()
{
	m_iCurPageIndex = 12;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage14()
{
	m_iCurPageIndex = 13;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage15()
{
	m_iCurPageIndex = 14;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage16()
{
	m_iCurPageIndex = 15;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage17()
{
	m_iCurPageIndex = 16;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage18()
{
	m_iCurPageIndex = 17;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage19()
{
	m_iCurPageIndex = 18;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage20()
{
	m_iCurPageIndex = 19;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage21()
{
	m_iCurPageIndex = 20;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage22()
{
	m_iCurPageIndex = 21;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage23()
{
	m_iCurPageIndex = 22;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage24()
{
	m_iCurPageIndex = 23;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage25()
{
	m_iCurPageIndex = 24;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage26()
{
	m_iCurPageIndex = 25;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage27()
{
	m_iCurPageIndex = 26;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage28()
{
	m_iCurPageIndex = 27;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage29()
{
	m_iCurPageIndex = 28;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage30()
{
	m_iCurPageIndex = 29;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage31()
{
	m_iCurPageIndex = 30;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::OnBnClickedRadioPage32()
{
	m_iCurPageIndex = 31;

	UpdateLightChannelValue();
}

void CLightPageControlDlg::UpdateLightChannelValue()
{
	UpdateData(TRUE);

	m_sEditPageDesc = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].m_sPageDesc;

	m_SliderLight1.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0]);
	m_SliderLight2.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1]);
	m_SliderLight3.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2]);
	m_SliderLight4.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3]);
	m_SliderLight5.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4]);
	m_SliderLight6.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5]);
	m_SliderLight7.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6]);
	m_SliderLight8.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7]);
	m_SliderLight9.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8]);
	m_SliderLight10.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9]);
	m_SliderLight11.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10]);
	m_SliderLight12.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11]);
	m_SliderLight13.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12]);
	m_SliderLight14.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13]);
	m_SliderLight15.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14]);
	m_SliderLight16.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15]);
	m_SliderLight17.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16]);
	m_SliderLight18.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17]);
	m_SliderLight19.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18]);
	m_SliderLight20.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19]);
	m_SliderLight21.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20]);
	m_SliderLight22.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21]);
	m_SliderLight23.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22]);
	m_SliderLight24.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23]);
	m_SliderLight25.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24]);
	m_SliderLight26.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25]);
	m_SliderLight27.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26]);
	m_SliderLight28.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27]);
	m_SliderLight29.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28]);
	m_SliderLight30.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29]);
	m_SliderLight31.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30]);
	m_SliderLight32.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31]);

	m_iEditLight1 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0];
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1];
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2];
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3];
	m_iEditLight5 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4];
	m_iEditLight6 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5];
	m_iEditLight7 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6];
	m_iEditLight8 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7];
	m_iEditLight9 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8];
	m_iEditLight10 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9];
	m_iEditLight11 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10];
	m_iEditLight12 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11];
	m_iEditLight13 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12];
	m_iEditLight14 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13];
	m_iEditLight15 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14];
	m_iEditLight16 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15];
	m_iEditLight17 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16];
	m_iEditLight18 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17];
	m_iEditLight19 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18];
	m_iEditLight20 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19];
	m_iEditLight21 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20];
	m_iEditLight22 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21];
	m_iEditLight23 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22];
	m_iEditLight24 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23];
	m_iEditLight25 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24];
	m_iEditLight26 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25];
	m_iEditLight27 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26];
	m_iEditLight28 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27];
	m_iEditLight29 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28];
	m_iEditLight30 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29];
	m_iEditLight31 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30];
	m_iEditLight32 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31];

	int piUsedImageNumber[50], piUsedSequenceNumber[50], iNoUsedImage;
	CString sPageUsedImageInfo;
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      영상 %d(주소 %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, 영상 %d(주소 %d), ", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      미사용");
	}
	else
	{
		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("Image in used(Sequence):   Image %d(Addr %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, Image %d(Addr %d)", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("Image in used(Sequence):   Unused");
	}
	GetDlgItem(IDC_STATIC_PAGE_USED_IMAGE_INFO)->SetWindowTextA(sPageUsedImageInfo);

	UpdateData(FALSE);

	if (m_pLightLibraryDlg)
		m_pLightLibraryDlg->m_iCurSelectedPageIndex = m_iCurPageIndex;
}

BOOL CLightPageControlDlg::GetUsedImageNumber(int *piUsedImageNumber, int *piUsedSequenceNumber, int *piNoUsedImage)
{
	BOOL bUsed = FALSE;

	*piNoUsedImage = 0;

	int iNoCurImageGrab = 0;
	int iGrabCount = 0;

	int iSeqAddrIndex, iNoSeqGrabImage, iPageIndex;

	while (TRUE)
	{
		if (iNoCurImageGrab >= THEAPP.m_pModelDataManager->m_iNoUsedImageGrab)
			break;

		//if (iGrabCount >= g_iVisionMaxGrabBuffer[THEAPP.m_iCurVisionCamType])
		//	break;

		iSeqAddrIndex = THEAPP.m_pModelDataManager->m_iSeqAddrIndex[THEAPP.m_iCurStageIndex][iNoCurImageGrab];
		iNoSeqGrabImage = THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrCount[iSeqAddrIndex];

		for (int i = 0; i < iNoSeqGrabImage; i++)
		{
			iPageIndex = THEAPP.m_pModelDataManager->m_PageControlData.m_iAddrSeq[iSeqAddrIndex][i];
			if (iPageIndex == m_iCurPageIndex)
			{
				if (THEAPP.m_pModelDataManager->m_bUseSpecularImage[iNoCurImageGrab] == TRUE || THEAPP.m_pModelDataManager->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
					piUsedImageNumber[*piNoUsedImage] = iNoCurImageGrab + 1;
				else
					piUsedImageNumber[*piNoUsedImage] = iNoCurImageGrab + 1 + i;

				piUsedSequenceNumber[*piNoUsedImage] = iSeqAddrIndex;

				++(*piNoUsedImage);

				bUsed = TRUE;
			}
		}

		if (THEAPP.m_pModelDataManager->m_bUseSpecularImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DFM_USED_CONV_IMAGE_NUMBER;
			iGrabCount += SPV_RAW_IMAGE_NUMBER;
		}
		else if (THEAPP.m_pModelDataManager->m_bUseDiffusedImage[iNoCurImageGrab] == TRUE)
		{
			iNoCurImageGrab += DIFFUSED_USED_CONV_IMAGE_NUMBER;
			iGrabCount += DIFFUSED_RAW_IMAGE_NUMBER;
		}
		else
		{
			iNoCurImageGrab += iNoSeqGrabImage;
			iGrabCount += iNoSeqGrabImage;
		}
	}

	return bUsed;
}

void CLightPageControlDlg::OnBnClickedButtonShowLibrary()
{
	ShowLightLibraryDlg();
}

void CLightPageControlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_pLightLibraryDlg->delegateSelectedListApply -= this;

	m_pLightLibraryDlg->DeleteInstance();
	m_pLightLibraryDlg = NULL;
}

void CLightPageControlDlg::ShowLightLibraryDlg()
{
	m_pLightLibraryDlg->Show();
}

void CLightPageControlDlg::HideLightLibraryDlg()
{
	if (m_pLightLibraryDlg->m_hWnd)
	{
		m_pLightLibraryDlg->Hide();
	}
}

LRESULT CLightPageControlDlg::OnEventSelectedListApply(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < 32; i++)
		THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[i] = m_pLightLibraryDlg->m_UpdatePage.uiChannel[i];

	m_SliderLight1.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0]);
	m_SliderLight2.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1]);
	m_SliderLight3.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2]);
	m_SliderLight4.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3]);
	m_SliderLight5.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4]);
	m_SliderLight6.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5]);
	m_SliderLight7.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6]);
	m_SliderLight8.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7]);
	m_SliderLight9.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8]);
	m_SliderLight10.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9]);
	m_SliderLight11.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10]);
	m_SliderLight12.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11]);
	m_SliderLight13.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12]);
	m_SliderLight14.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13]);
	m_SliderLight15.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14]);
	m_SliderLight16.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15]);
	m_SliderLight17.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16]);
	m_SliderLight18.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17]);
	m_SliderLight19.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18]);
	m_SliderLight20.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19]);
	m_SliderLight21.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20]);
	m_SliderLight22.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21]);
	m_SliderLight23.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22]);
	m_SliderLight24.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23]);
	m_SliderLight25.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24]);
	m_SliderLight26.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25]);
	m_SliderLight27.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26]);
	m_SliderLight28.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27]);
	m_SliderLight29.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28]);
	m_SliderLight30.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29]);
	m_SliderLight31.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30]);
	m_SliderLight32.SetPos(THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31]);

	m_iEditLight1 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[0];
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[1];
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[2];
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[3];
	m_iEditLight5 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[4];
	m_iEditLight6 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[5];
	m_iEditLight7 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[6];
	m_iEditLight8 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[7];
	m_iEditLight9 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[8];
	m_iEditLight10 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[9];
	m_iEditLight11 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[10];
	m_iEditLight12 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[11];
	m_iEditLight13 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[12];
	m_iEditLight14 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[13];
	m_iEditLight15 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[14];
	m_iEditLight16 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[15];
	m_iEditLight17 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[16];
	m_iEditLight18 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[17];
	m_iEditLight19 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[18];
	m_iEditLight20 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[19];
	m_iEditLight21 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[20];
	m_iEditLight22 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[21];
	m_iEditLight23 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[22];
	m_iEditLight24 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[23];
	m_iEditLight25 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[24];
	m_iEditLight26 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[25];
	m_iEditLight27 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[26];
	m_iEditLight28 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[27];
	m_iEditLight29 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[28];
	m_iEditLight30 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[29];
	m_iEditLight31 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[30];
	m_iEditLight32 = THEAPP.m_pModelDataManager->m_PageControlData.m_Page[m_iCurPageIndex].uiChannel[31];

	int piUsedImageNumber[50], piUsedSequenceNumber[50], iNoUsedImage;
	CString sPageUsedImageInfo;
	if (THEAPP.m_iPGMLanguageSelect == 0)
	{
		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      영상 %d(주소 %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, 영상 %d(주소 %d), ", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("사용중인 영상 번호(시퀀스):      미사용");
	}
	else
	{
		if (GetUsedImageNumber(piUsedImageNumber, piUsedSequenceNumber, &iNoUsedImage))
		{
			for (int i = 0; i < iNoUsedImage; i++)
			{
				if (i == 0)
					sPageUsedImageInfo.Format("Image in used(Sequence):   Image %d(Addr %d)", piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
				else
					sPageUsedImageInfo.Format("%s, Image %d(Addr %d)", sPageUsedImageInfo, piUsedImageNumber[i], piUsedSequenceNumber[i] + 1);
			}
		}
		else
			sPageUsedImageInfo.Format("Image in used(Sequence):   Unused");
	}
	GetDlgItem(IDC_STATIC_PAGE_USED_IMAGE_INFO)->SetWindowTextA(sPageUsedImageInfo);

	UpdateData(FALSE);

	return 1;
}
