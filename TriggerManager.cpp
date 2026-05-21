#include "stdafx.h"
#include "uScan.h"
#include "TriggerManager.h"

CTriggerManager* CTriggerManager::m_pInstance = NULL;

CTriggerManager* CTriggerManager::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTriggerManager();
	}
	return m_pInstance;
}

void CTriggerManager::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

CTriggerManager::CTriggerManager(void)
{

}

CTriggerManager::~CTriggerManager(void)
{
	m_AJinAXL.Terminate();
}

BOOL CTriggerManager::Initialize()
{
	m_AJinAXL.Initialize();

	m_iSleepTime = 1;

	return TRUE;
}

void CTriggerManager::GrabSeqChangeTrigger(int iGrabSeqIndex)
{
	// Sequence Selection

	int iVisionOrder = 0;

	BOOL bOnOff[GRAB_SEQ_IO_BIT];

	for (int i = 0; i < GRAB_SEQ_IO_BIT; i++)
	{
		bOnOff[i] = FALSE;
	}

	if (iGrabSeqIndex == 0)
	{
		;
	}
	else if (iGrabSeqIndex == 1)
	{
		bOnOff[3] = TRUE;
	}
	else if (iGrabSeqIndex == 2)
	{
		bOnOff[4] = TRUE;
	}
	else if (iGrabSeqIndex == 3)
	{
		bOnOff[3] = TRUE;
		bOnOff[4] = TRUE;
	}
	else if (iGrabSeqIndex == 4)
	{
		bOnOff[5] = TRUE;
	}
	else if (iGrabSeqIndex == 5)
	{
		bOnOff[3] = TRUE;
		bOnOff[5] = TRUE;
	}
	else if (iGrabSeqIndex == 6)
	{
		bOnOff[4] = TRUE;
		bOnOff[5] = TRUE;
	}
	else if (iGrabSeqIndex == 7)
	{
		bOnOff[3] = TRUE;
		bOnOff[4] = TRUE;
		bOnOff[5] = TRUE;
	}

	m_AJinAXL.Set_GrabSeqAddress(iVisionOrder, bOnOff);

	// Speedup
	Delay(1);

	// Sequence Apply
	m_AJinAXL.Set_Output(iVisionOrder, 2, TRUE);

	// Speedup
	Delay(1);
}

void CTriggerManager::GrabSeqFireTrigger()
{
	int iVisionOrder = 0;

	m_AJinAXL.Set_Output(iVisionOrder, 0, TRUE);

	// Speedup
	Delay(1);
}

void CTriggerManager::Delay(int msec)
{
	m_AJinAXL.Delay(msec);	// Delay
}

void CTriggerManager::GrabSeqFireTriggerByEthernet(int iLightControllerIndex, int iGrabSeqIndex)
{
	CString str, strSTX, strSendData;

	strSendData.Format("@WSU0000/%d\r\n", iGrabSeqIndex);
	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);
	Delay(100);
	strSendData.Format("@WSS0000/1\r\n");
	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);
}

void CTriggerManager::GrabSeqChangeByEthernet(int iLightControllerIndex, int iGrabSeqIndex, BOOL bPreDelay, BOOL bPostDelay)
{
	CString str, strSTX, strSendData;

	strSendData.Format("@WSU0000/%d\r\n", iGrabSeqIndex);

	if (bPreDelay)
		Sleep(10);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	if (bPostDelay)
		Sleep(5);
}

void CTriggerManager::GrabFireTriggerByEthernet(int iLightControllerIndex)
{
	CString str, strSTX, strSendData;

	strSendData.Format("@WSS0000/1\r\n");

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

}

void CTriggerManager::AutoLightSequenceSet(int iLightControllerIndex, int iAddrIndex, int iSeqCount)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WSN%02d00/", iAddrIndex);

	strSendData = _T("");

	//Sequence Data	
	for (int y = 0; y < LIGHTCTRL_PAGE_COUNT; y++)
	{
		str.Format("%d,", y);
		strSendData += str;
	}

	strSendData.Delete(strSendData.GetLength() - 1);
	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);

	strSTX.Format("@WSC%02d00/", iAddrIndex);

	strSendData.Format("%d", iSeqCount);

	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);
}

void CTriggerManager::AutoLightPageSet_Single(int iLightControllerIndex, int iPageIndex, int iChannelIndex, int iChannelValue, BOOL bCoaxial)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WLO%02d00/", iPageIndex);

	strSendData = _T("");

	if (bCoaxial)
	{
		for (int y = 0; y < LIGHT_CH_CNT; y++)
		{
			if (y < 8)
				str.Format("%d,", iChannelValue);
			else
				str.Format("%d,", 0);
			strSendData += str;
		}
	}
	for (int y = 0; y < LIGHT_CH_CNT; y++)
	{
		if (y != iChannelIndex)
			str.Format("%d,", 0);
		else
			str.Format("%d,", iChannelValue);
		strSendData += str;
	}

	strSendData.Delete(strSendData.GetLength() - 1);
	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);
}

void CTriggerManager::AutoLightPageSet_Multi(int iLightControllerIndex, int iPageIndex, const std::vector<std::pair<int, int>>& vLightGroup)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WLO%02d00/", iPageIndex);

	strSendData = _T("");

	for (int iLIGHTCHIdx = 0; iLIGHTCHIdx < LIGHTCTRL_PAGE_COUNT; iLIGHTCHIdx++)
	{
		if (iLIGHTCHIdx > 0 && iLIGHTCHIdx < 8)
			continue;

		BOOL bFindCheck = FALSE;
		for (int i = 0; i < vLightGroup.size(); ++i)
		{
			if (iLIGHTCHIdx == vLightGroup[i].first)
			{
				if (iLIGHTCHIdx == 0)
					str.Format("%d,%d,%d,%d,%d,%d,%d,%d,", vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second, vLightGroup[i].second);
				else
					str.Format("%d,", vLightGroup[i].second);
				bFindCheck = TRUE;
				break;
			}
		}

		if (!bFindCheck)
		{
			if (iLIGHTCHIdx == 0)
				str.Format("%d,%d,%d,%d,%d,%d,%d,%d,", 0, 0, 0, 0, 0, 0, 0, 0);
			else
				str.Format("%d,", 0);
		}

		strSendData += str;
	}

	strSendData.Delete(strSendData.GetLength() - 1);
	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);
}

void CTriggerManager::FireTrigger(int iCamIdx, int iTriggerPageIndex)
{
	BOOL bPageOnOff[LIGHTCTRL_PAGE_COUNT];
	for (int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
		bPageOnOff[i] = FALSE;

	bPageOnOff[iTriggerPageIndex] = TRUE;

	int iVisionOrder = 2;

	if (iCamIdx == VISION_NUMBER_1)
		iVisionOrder = 2;
	else if (iCamIdx == VISION_NUMBER_2)
		iVisionOrder = 3;

	m_AJinAXL.Set_Trigger(iVisionOrder, bPageOnOff, m_iSleepTime);	// FALSE: No Usage
}

void CTriggerManager::FireAssyTrigger(int iCamIdx, int iTriggerPageIndex, BOOL bSimulGrabBottomAlign)
{
	// iTriggerPageIndex = 해당 카메라 기준 0-base Page 번호

	if (iCamIdx>= VISION_NUMBER_1 && iCamIdx <= VISION_NUMBER_3)
		m_AJinAXL.Set_AssyTrigger(iCamIdx, iTriggerPageIndex, m_iSleepTime);
	else
	{
		if (iCamIdx >= VISION_NUMBER_4 && iCamIdx <= VISION_NUMBER_4_2)
		{
			if (bSimulGrabBottomAlign)
				m_AJinAXL.Set_AssyTrigger(iCamIdx, iTriggerPageIndex, m_iSleepTime);
			else
			{
				if (iCamIdx== VISION_NUMBER_4)
					m_AJinAXL.Set_AssyTrigger(iCamIdx, iTriggerPageIndex, m_iSleepTime, ACAM_SEL_1);
				else
					m_AJinAXL.Set_AssyTrigger(iCamIdx-1, iTriggerPageIndex, m_iSleepTime, ACAM_SEL_2);
			}
		}
		else if (iCamIdx <= VISION_NUMBER_4_3)
		{
			m_AJinAXL.Set_AssyTrigger(iCamIdx-1, iTriggerPageIndex, m_iSleepTime);
		}
	}
}