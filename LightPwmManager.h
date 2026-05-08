#pragma once

#include "ComThread.h"

class CLightPwmManager : public CWnd
{
public:

	static CLightPwmManager* m_pInstance;
	static CLightPwmManager* GetInstance();
	void   DeleteInstance();
	CLightPwmManager();
	~CLightPwmManager();

	void Initialize();
	void UnInitialize();

	BOOL OpenPort(int iComPort);

	void SetLightValue(int ch, int nValue);
	void LightOn();
	void LightOff();

	void SetCh1LightValue(int iLightValue) { m_iCh1LightValue = iLightValue; }
	void SetCh2LightValue(int iLightValue) { m_iCh2LightValue = iLightValue; }
	void SetCh3LightValue(int iLightValue) { m_iCh3LightValue = iLightValue; }
	void SetCh4LightValue(int iLightValue) { m_iCh4LightValue = iLightValue; }

	CCommThread m_ComPort;

private:
	int m_iCh1LightValue;
	int m_iCh2LightValue;
	int m_iCh3LightValue;
	int m_iCh4LightValue;
	CRect m_ScreenRect;
	BOOL m_bConnected;
};
