#include "stdafx.h"
#include "uScan.h"
#include "LightPwmManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightPwmManager

CLightPwmManager* CLightPwmManager::m_pInstance = NULL;

CLightPwmManager* CLightPwmManager::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CLightPwmManager();
		if (!m_pInstance->m_hWnd) {
			CRect r(0, 0, 0, 0);
			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), (LPCTSTR)"CLightPwmManager", 0, r, NULL, 0, NULL);
		}
	}
	return m_pInstance;
}

void CLightPwmManager::DeleteInstance()
{
	if (m_pInstance->m_hWnd)
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance)
}

CLightPwmManager::CLightPwmManager()
{
	m_bConnected = FALSE;

	m_iCh1LightValue = 0;
	m_iCh2LightValue = 0;
	m_iCh3LightValue = 0;
	m_iCh4LightValue = 0;
}

CLightPwmManager::~CLightPwmManager()
{
}

void CLightPwmManager::Initialize()
{

}

void CLightPwmManager::UnInitialize()
{
#ifdef INLINE_MODE
	LightOff();

	if (m_bConnected)
	{
		m_ComPort.ClosePort();
		m_bConnected = FALSE;
	}
#endif
}

//////////////////////////// Function to use  ///////////////////////////////////////////

BOOL CLightPwmManager::OpenPort(int iComPort)
{
	if (m_bConnected)
		m_ComPort.ClosePort();

	if (m_ComPort.OpenPort(iComPort, 9600))
		m_bConnected = TRUE;
	else
		m_bConnected = FALSE;

	return m_bConnected;
}

//여기부터
void CLightPwmManager::LightOff()
{
	SetLightValue(0, 0);
	SetLightValue(1, 0);
	SetLightValue(2, 0);
	SetLightValue(3, 0);
}

void CLightPwmManager::LightOn()
{
	SetLightValue(0, m_iCh1LightValue);
	SetLightValue(1, m_iCh2LightValue);
	SetLightValue(2, m_iCh3LightValue);
	SetLightValue(3, m_iCh4LightValue);
}

void CLightPwmManager::SetLightValue(int ch, int nValue)
{
	if (!m_bConnected)
		return;

	// 채널 범위 체크 (0~3)
	if (ch < 0 || ch > 3)
		return;

	// PWM 범위 체크 (0~1023)
	if (nValue < 0)      nValue = 0;
	if (nValue > 1023)   nValue = 1023;

	BYTE BufData[8];

	// STX
	BufData[0] = 0x02;

	// CH.No : '0' ~ '3' (ASCII)
	BufData[1] = '0' + ch;

	// Command : 'w' -> PWM set
	BufData[2] = 'w';

	// Data : 4자리 10진수 ASCII
	char szPwm[5] = { 0 };
	sprintf_s(szPwm, "%04d", nValue);   // 예: 25 -> "0025"

	BufData[3] = szPwm[0];
	BufData[4] = szPwm[1];
	BufData[5] = szPwm[2];
	BufData[6] = szPwm[3];

	// ETX
	BufData[7] = 0x03;

	m_ComPort.WriteComm(BufData, 8);

	// 필요하면 약간의 딜레이
	Sleep(10);
}
