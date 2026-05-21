// AJinAXL.cpp : 구현 파일
//
#include "stdafx.h"
#include "AJinAXL.h"

#include "AXL.h"
#include "AXD.h"
#pragma comment (lib, "AXL.lib")

CAJinAXL g_objAJinAXL;

CCriticalSection TriggerCS;

static const ASSY_TRIGGER_MAP s_AssyMap[AVCAM_MAX] =
{
	//  camStart, camCnt, lightStart, lightCnt
	{  0, 1,  1, 9 },   // AVCAM_TOP_INSP_1
	{ 10, 1, 11, 9 },   // AVCAM_BTM_INSP_1
	{ 20, 1, 21, 3 },   // AVCAM_SIDE_INSP_1
	{ 24, 2, 26, 3 },   // AVCAM_BTM_ALIGN
	{ 29, 1, 30, 2 },   // AVCAM_TOP_ALIGN_1
};

CAJinAXL::CAJinAXL(void)
{
	m_DY0.nValue = 0;
	m_DY1.nValue = 0;
	m_DY2.nValue = 0;
	m_DY3.nValue = 0;

	m_dwAssyOutput = 0xFFFFFFFF;

 	LARGE_INTEGER freq;
 	QueryPerformanceFrequency(&freq);
 	m_nFreq = freq.QuadPart;

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
		m_hEventTrigger[i] = NULL;
}

CAJinAXL::~CAJinAXL(void)
{
}

#ifdef ASSY_LENS
BOOL CAJinAXL::Initialize()
{
#ifdef AJIN_BOARD_USE
	DWORD dwReturn = AxlOpenNoReset(7);
	if (dwReturn != AXT_RT_SUCCESS)
	{
		AfxMessageBox("AJin Open Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;
	}

	long lDIOCount;
	dwReturn = AxdInfoGetModuleCount(&lDIOCount);
	if (dwReturn != AXT_RT_SUCCESS)
	{
		AfxMessageBox("AJin Module Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;
	}
	if (lDIOCount < 1)
	{
		AfxMessageBox("AJin Module Count Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;	// Board 1 or 2장
	}

	for (int i = 0; i < MAX_INTERRUPT_NUMBER; i++)
	{
		m_hEventTrigger[i] = ::CreateEventA(NULL, FALSE, FALSE, NULL);

		if (m_hEventTrigger[i] == NULL)
		{
			AfxMessageBox("AJin Evenet Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
			return FALSE;
		}
	}
#endif

	// 출력 모듈(모듈0) idle 초기화 (Active-Low: 전 비트 High)
	Init_AssyTrigger();

	return TRUE;
}
#else
BOOL CAJinAXL::Initialize()
{
#ifdef AJIN_BOARD_USE
	DWORD dwReturn = AxlOpenNoReset(7);
	if (dwReturn != AXT_RT_SUCCESS)
	{
		AfxMessageBox("AJin Open Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;
	}

	long lDIOCount;
	dwReturn = AxdInfoGetModuleCount(&lDIOCount);
	if (dwReturn != AXT_RT_SUCCESS)
	{
		AfxMessageBox("AJin Module Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;
	}
	if (lDIOCount < 1)
	{
		AfxMessageBox("AJin Module Count Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
		return FALSE;	// Board 1 or 2장
	}

	for (int i = 0; i < MAX_INTERRUPT_NUMBER; i++)
	{
		m_hEventTrigger[i] = ::CreateEventA(NULL, FALSE, FALSE, NULL);

		if (m_hEventTrigger[i] == NULL)
		{
			AfxMessageBox("AJin Evenet Error!!", MB_SYSTEMMODAL | MB_ICONERROR);
			return FALSE;
		}
	}

	Init_Trigger(FALSE);		// 시작시 All On

	AxlInterruptEnable();
	AxdiInterruptSetModuleEnable(0, ENABLE);

	// --- 인터럽트 엣지 설정 변경 ---
// X000 (byte 0, bit 0): 상승엣지만
	AxdiInterruptEdgeSetByte(0, 0, UP_EDGE, 0x01);
	AxdiInterruptEdgeSetByte(0, 0, DOWN_EDGE, 0x00);

	// X008~X015 (byte 1): 미사용
	AxdiInterruptEdgeSetByte(0, 1, UP_EDGE, 0x00);
	AxdiInterruptEdgeSetByte(0, 1, DOWN_EDGE, 0x00);

	// X016 (byte 2, bit 0): 상승엣지만
	AxdiInterruptEdgeSetByte(0, 2, UP_EDGE, 0x01);
	AxdiInterruptEdgeSetByte(0, 2, DOWN_EDGE, 0x00);

	// X024~X031 (byte 3): 미사용
	AxdiInterruptEdgeSetByte(0, 3, UP_EDGE, 0x00);
	AxdiInterruptEdgeSetByte(0, 3, DOWN_EDGE, 0x00);

	AxdiInterruptSetModule(0, NULL, NULL, NULL, NULL);	// 초기화
	AxdiInterruptSetModule(0, NULL, NULL, (AXT_INTERRUPT_PROC)OnDIOInterruptCallback, NULL);

#endif

	return TRUE;
}
#endif

void CAJinAXL::Init_AssyTrigger()
{
	m_dwAssyOutput = 0xFFFFFFFF;   // Active-Low: 전 비트 idle(High)
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportDword(ASSY_OUTPUT_MODULE, 0, m_dwAssyOutput);
#endif
}

#ifdef ASSY_LENS
void CAJinAXL::Terminate()
{
	Init_AssyTrigger();

#ifdef AJIN_BOARD_USE
	if (AxlIsOpened()) 
		AxlClose();
#endif
}
#else
void CAJinAXL::Terminate()
{
	Init_Trigger(FALSE);	// 종료시 All Off

#ifdef AJIN_BOARD_USE
	if (AxlIsOpened())
		AxlClose();
#endif

	for (int i = 0; i < MAX_INTERRUPT_NUMBER; i++)
	{
		if (m_hEventTrigger[i])
		{
			::CloseHandle(m_hEventTrigger[i]);
			m_hEventTrigger[i] = NULL;
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Interrupt Callback
void __stdcall OnDIOInterruptCallback(long lActiveNo, DWORD uFlag)
{
	if (uFlag & (1 << 0))       // X000 → TOP Camera
	{
		::SetEvent(g_objAJinAXL.GetTriggerEvent(VISION_NUMBER_1));
	}
	if (uFlag & (1 << 16))      // X016 → BTM Camera
	{
		::SetEvent(g_objAJinAXL.GetTriggerEvent(VISION_NUMBER_2));
	}

	//g_objAJinAXL.Read_Input();
}
/////////////////////////////////////////////////////////////////////////////

void CAJinAXL::Init_Trigger(BOOL bOn)
{
	DWORD dwValue = (bOn ? 0xFFFF : 0x0000);
	m_DY0.nValue = m_DY1.nValue = m_DY2.nValue = m_DY3.nValue = dwValue;

#ifdef AJIN_BOARD_USE
	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 2; j++) 
			AxdoWriteOutportWord(i, j, dwValue);
#endif
}

///////////////////////////////////////////////////////////////////////////////

void CAJinAXL::Read_Input()
{
	// 32bit 전체를 한 번에 읽음 (X000 ~ X031)
	m_csInput.Lock();
	AxdiReadInportDword(0, 0, &m_DX.nValue);
	m_csInput.Unlock();
}

HANDLE CAJinAXL::GetTriggerEvent(int iVisionCamType)
{
	int idx = iVisionCamType - VISION_NUMBER_1;
	if (idx < 0 || idx >= MAX_INTERRUPT_NUMBER) return NULL;
	return m_hEventTrigger[idx];
}

void CAJinAXL::ResetTriggerEvent(int iVisionCamType)
{
	HANDLE h = GetTriggerEvent(iVisionCamType);
	if (h) ::ResetEvent(h);
}

void CAJinAXL::Set_Trigger(int nPort, BOOL* bLight, int nS)
{
	TriggerCS.Lock();

	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = 0xFFFE;	// Camera Trigger Off
	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
	{
		if (bLight[i]) 
			dwValue &= ~(1 << (i + 1));
	}

	switch (nPort) 
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: m_DY1.nValue = dwValue; break;
	case 2: m_DY2.nValue = dwValue; break;
	case 3: m_DY3.nValue = dwValue; break;
	}

#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif

	Delay(nS);	// Delay

	dwValue = 0xFFFF;		// All On
	switch (nPort) 
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: m_DY1.nValue = dwValue; break;
	case 2: m_DY2.nValue = dwValue; break;
	case 3: m_DY3.nValue = dwValue; break;
	}
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif

	TriggerCS.Unlock();
}

void CAJinAXL::Set_AssyTrigger(int iCam, int iPageIndex, int nS, int iCamSelect /*= ACAM_SEL_ALL*/)
{
	if (iCam < 0 || iCam >= AVCAM_MAX)
		return;

	const ASSY_TRIGGER_MAP& map = s_AssyMap[iCam];

	DWORD dwMask = 0;

	// 카메라 트리거 비트
	//  ACAM_SEL_ALL  : 매핑된 카메라 비트 전체 동시 (Btm Align = Y024+Y025)
	//  ACAM_SEL_1/_2 : 해당 인덱스 카메라 비트 1개만 (Btm Align 개별 촬상용)
	if (iCamSelect == ACAM_SEL_ALL)
	{
		for (int i = 0; i < map.iCamBitCount; i++)
			dwMask |= (1u << (map.iCamBitStart + i));
	}
	else
	{
		int iSelIdx = iCamSelect - ACAM_SEL_1;   // ACAM_SEL_1 -> 0, ACAM_SEL_2 -> 1
		if (iSelIdx >= 0 && iSelIdx < map.iCamBitCount)
			dwMask |= (1u << (map.iCamBitStart + iSelIdx));
		else
			dwMask |= (1u << map.iCamBitStart);  // 범위 밖이면 첫 비트로 폴백
	}

	// 선택 조명 Page 비트 (Page Index는 카메라 기준 0-base)
	if (iPageIndex >= 0 && iPageIndex < map.iLightBitCount)
		dwMask |= (1u << (map.iLightBitStart + iPageIndex));

	TriggerCS.Lock();

	// Active-Low 펄스: 선택 비트만 0, 나머지는 idle(1) 유지, 32비트 동시 출력
	m_dwAssyOutput &= ~dwMask;
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportDword(ASSY_OUTPUT_MODULE, 0, m_dwAssyOutput);
#endif

	Delay(nS);

	m_dwAssyOutput |= dwMask;   // 해제
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportDword(ASSY_OUTPUT_MODULE, 0, m_dwAssyOutput);
#endif

	TriggerCS.Unlock();
}

void CAJinAXL::All_OFF()
{
	DWORD dwValue = 0x0000;
	m_DY0.nValue = dwValue;

#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(0, 0, dwValue);
#endif
}

void CAJinAXL::Set_GrabSeqAddress(int nPort, BOOL* bOn)
{
	TriggerCS.Lock();

	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = 0x0;	// All Off

	for (int i = 0; i < GRAB_SEQ_IO_BIT; i++)
	{
		if (bOn[i])
			dwValue |= (1 << (i));
	}

	switch (nPort)
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: m_DY1.nValue = dwValue; break;
	case 2: m_DY2.nValue = dwValue; break;
	case 3: m_DY3.nValue = dwValue; break;
	}

#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif

	// Speedup
	Delay(1);

	TriggerCS.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Bit Test

BOOL CAJinAXL::Get_Output(int nPort, int nIdx)
{
	switch (nPort) {
	case 0: return ((m_DY0.nValue >> nIdx) & 1);	// Y000 - Y015
	case 1: return ((m_DY1.nValue >> nIdx) & 1);	// Y100 - Y115
	case 2: return ((m_DY2.nValue >> nIdx) & 1);	// Y200 - Y215
	case 3: return ((m_DY3.nValue >> nIdx) & 1);	// Y300 - Y315
	}
	return FALSE;
}

void CAJinAXL::Set_Output(int nPort, int nIdx, BOOL bOn)
{
	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = 0, dwBit = (1 << nIdx);
	switch (nPort) {
	case 0: m_DY0.nValue = dwValue = (bOn ? (m_DY0.nValue | dwBit) : (m_DY0.nValue & ~dwBit)); break;	// Y000 - Y015
	case 1: m_DY1.nValue = dwValue = (bOn ? (m_DY1.nValue | dwBit) : (m_DY1.nValue & ~dwBit)); break;	// Y100 - Y115
	case 2: m_DY2.nValue = dwValue = (bOn ? (m_DY2.nValue | dwBit) : (m_DY2.nValue & ~dwBit)); break;	// Y200 - Y215
	case 3: m_DY3.nValue = dwValue = (bOn ? (m_DY3.nValue | dwBit) : (m_DY3.nValue & ~dwBit)); break;	// Y300 - Y315
	}
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// uSleep
// 
void CAJinAXL::DoEvents()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CAJinAXL::uSleep(int msec)
{
	LARGE_INTEGER tStart, tNow;
	LONGLONG lTerm = 0;
	QueryPerformanceCounter(&tStart);

	while (TRUE) {
		QueryPerformanceCounter(&tNow);
		lTerm = (tNow.QuadPart - tStart.QuadPart) * 1000 / m_nFreq;
		if (lTerm > msec) break;
		DoEvents();
	}
}

void CAJinAXL::Delay(int msec)
{
	LARGE_INTEGER tStart, tNow;
	LONGLONG lTerm = 0;
	QueryPerformanceCounter(&tStart);

	while (TRUE) {
		QueryPerformanceCounter(&tNow);
		lTerm = (tNow.QuadPart - tStart.QuadPart) * 1000 / m_nFreq;
		if (lTerm > msec) 
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
