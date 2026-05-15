// AJinAXL.cpp : 구현 파일
//
#include "stdafx.h"
#include "AJinAXL.h"

#include "AXL.h"
#include "AXD.h"
#pragma comment (lib, "AXL.lib")

CAJinAXL g_objAJinAXL;

CCriticalSection TriggerCS;

CAJinAXL::CAJinAXL(void)
{
	m_DY0.nValue = 0;
	m_DY1.nValue = 0;
	m_DY2.nValue = 0;
	m_DY3.nValue = 0;

 	LARGE_INTEGER freq;
 	QueryPerformanceFrequency(&freq);
 	m_nFreq = freq.QuadPart;

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
		m_hEventTrigger[i] = NULL;
}

CAJinAXL::~CAJinAXL(void)
{
}

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
