// LightControl.cpp: implementation of the CLightControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uScan.h"
#include "LightControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightControl::CLightControl()
{
	m_iComPortNumber = 1;

	int i;

	for (i = 0; i < LIGHTCTRL_PAGE_COUNT; i++)
	{
		m_Page[i].uiChannel[i] = LIGHT_BRIGHT_MAX;
	}

	for (i = 0; i < LIGHTCTRL_GRAB_ADDR_SEQ_COUNT; i++)
	{
		m_iAddrCount[i] = LIGHTCTRL_GRAB_ADDR_PAGE_COUNT;
		m_sAddrDesc[i] = _T("");

		for (int j = 0; j < LIGHTCTRL_PAGE_COUNT; j++)
			m_iAddrSeq[i][j] = j;

		m_iWSSDelayTime[i] = 0;
	}
}

CLightControl::~CLightControl()
{

}

void CLightControl::SetGrabSequence(int iLightControllerIndex, int iAddrIndex, int *piPageIndex, int iSeqCount)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WSN%02d00/", iAddrIndex);

	strSendData = _T("");

	//Sequence Data	
	for (int y = 0; y < LIGHTCTRL_PAGE_COUNT; y++)
	{
		str.Format("%d,", piPageIndex[y]);
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

void CLightControl::SetOnTime_Page(int iLightControllerIndex, int iPageIndex)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WLO%02d00/", iPageIndex);

	strSendData = _T("");

	SPage& Page = m_Page[iPageIndex];

	for (int y = 0; y < LIGHT_CH_CNT; y++)
	{
		str.Format("%d,", Page.uiChannel[y]);
		strSendData += str;
	}

	strSendData.Delete(strSendData.GetLength() - 1);
	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);
}

void CLightControl::SetOnTime_Channel(int iLightControllerIndex, int iPageIndex, int iChannelNumber, int iOnTime)
{
	CString str, strSTX, strSendData;

	strSTX.Format("@WLO%02d%02d/", iPageIndex, iChannelNumber);

	strSendData.Format("%d", iOnTime);

	strSendData = strSTX + strSendData;
	strSendData.Format("%s\r\n", strSendData);

	THEAPP.m_pHandlerService->Send_Tcp(iLightControllerIndex, strSendData, TRUE);

	Sleep(30);
}

void CLightControl::SetIllumination_2CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	

	//////////////////////////////////////////////////////////////////////////
	BYTE BufData[11];
	//////////////////////////////////////////////////////////////////////////

	SPage& Page = m_Page[uiPageIndex];

	if (Page.uiChannel[0] < 0)
		Page.uiChannel[0] = 0;
	if (Page.uiChannel[1] < 0)
		Page.uiChannel[1] = 0;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Check Sum
	BufData[8] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7];

	// End
	BufData[9] = 0xEE;
	BufData[10] = 0xEE;

	m_ComPort.WriteComm(BufData, 11);
	Sleep(300);

}

void CLightControl::SetIllumination_4CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	

	//////////////////////////////////////////////////////////////////////////
	BYTE BufData[15];
	//////////////////////////////////////////////////////////////////////////

	SPage& Page = m_Page[uiPageIndex];

	if (Page.uiChannel[0] < 0)
		Page.uiChannel[0] = 0;
	if (Page.uiChannel[1] < 0)
		Page.uiChannel[1] = 0;
	if (Page.uiChannel[2] < 0)
		Page.uiChannel[2] = 0;
	if (Page.uiChannel[3] < 0)
		Page.uiChannel[3] = 0;

	// Start
	BufData[0] = 0xEF;
	BufData[1] = 0xEF;

	// Command
	BufData[2] = 0x00;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Check Sum
	BufData[12] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11];

	// End
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;

	m_ComPort.WriteComm(BufData, 15);
	Sleep(300);

}

void CLightControl::SetChannelllumination_4CH(unsigned int uiPageIndex, unsigned int uiChannelIndex, unsigned int uiChannelValue)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255
	BYTE BufData[15];
	SPage Page;

	Page.uiChannel[0] = 0;
	Page.uiChannel[1] = 0;
	Page.uiChannel[2] = 0;
	Page.uiChannel[3] = 0;
	Page.uiChannel[4] = 0;
	Page.uiChannel[5] = 0;
	Page.uiChannel[6] = 0;
	Page.uiChannel[7] = 0;
	Page.uiChannel[8] = 0;
	Page.uiChannel[9] = 0;

	Page.uiChannel[uiChannelIndex] = uiChannelValue;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Check Sum
	BufData[12] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11];

	// End
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;

	m_ComPort.WriteComm(BufData, 15);
	Sleep(300);
}

void CLightControl::SetChannelClear_4CH()
{
	BYTE BufData[15];
	SPage Page;
	int uiPageIndex = 0;

	Page.uiChannel[0] = 0;
	Page.uiChannel[1] = 0;
	Page.uiChannel[2] = 0;
	Page.uiChannel[3] = 0;
	Page.uiChannel[4] = 0;
	Page.uiChannel[5] = 0;
	Page.uiChannel[6] = 0;
	Page.uiChannel[7] = 0;
	Page.uiChannel[8] = 0;
	Page.uiChannel[9] = 0;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Check Sum
	BufData[12] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11];

	// End
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;

	m_ComPort.WriteComm(BufData, 15);
	Sleep(300);
}

// PageIndex=0 Only
void CLightControl::SetTotalChannelllumination_4CH(unsigned int uiChannelValue1, unsigned int uiChannelValue2, unsigned int uiChannelValue3, unsigned int uiChannelValue4)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255
	BYTE BufData[15];
	SPage Page;
	int uiPageIndex = 0;
	Page.uiChannel[0] = uiChannelValue1;
	Page.uiChannel[1] = uiChannelValue2;
	Page.uiChannel[2] = uiChannelValue3;
	Page.uiChannel[3] = uiChannelValue4;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Check Sum
	BufData[12] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11];

	// End
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;

	m_ComPort.WriteComm(BufData, 15);
	Sleep(300);
}

void CLightControl::SetIllumination_6CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	
	BYTE BufData[19];
	SPage& Page = m_Page[uiPageIndex];

	for (int i = 0; i < 6; i++)
	{
		if (Page.uiChannel[i] < 0)
			Page.uiChannel[i] = 0;
	}

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Check Sum
	BufData[16] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15];

	// End
	BufData[17] = 0xEE;
	BufData[18] = 0xEE;

	m_ComPort.WriteComm(BufData, 19);
	Sleep(300);

}

void CLightControl::SetIllumination_8CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	

	//////////////////////////////////////////////////////////////////////////
	BYTE BufData[23];
	//////////////////////////////////////////////////////////////////////////

	SPage& Page = m_Page[uiPageIndex];

	for (int i = 0; i < 8; i++)
	{
		if (Page.uiChannel[i] < 0)
			Page.uiChannel[i] = 0;
	}

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Channel7
	BufData[16] = (BYTE)((Page.uiChannel[6] & 0xFF00) >> 8);
	BufData[17] = (BYTE)Page.uiChannel[6] & 0xFF;

	// Channel8
	BufData[18] = (BYTE)((Page.uiChannel[7] & 0xFF00) >> 8);
	BufData[19] = (BYTE)Page.uiChannel[7] & 0xFF;

	// Check Sum
	BufData[20] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15] ^ BufData[16] ^ BufData[17] ^ BufData[18] ^ BufData[19];

	// End
	BufData[21] = 0xEE;
	BufData[22] = 0xEE;

	m_ComPort.WriteComm(BufData, 23);
	Sleep(300);

}


void CLightControl::SetIllumination_10CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	

	//////////////////////////////////////////////////////////////////////////
	BYTE BufData[27];
	//////////////////////////////////////////////////////////////////////////

	SPage& Page = m_Page[uiPageIndex];

	if (Page.uiChannel[0] < 0)
		Page.uiChannel[0] = 0;
	if (Page.uiChannel[1] < 0)
		Page.uiChannel[1] = 0;
	if (Page.uiChannel[2] < 0)
		Page.uiChannel[2] = 0;
	if (Page.uiChannel[3] < 0)
		Page.uiChannel[3] = 0;
	if (Page.uiChannel[4] < 0)
		Page.uiChannel[4] = 0;
	if (Page.uiChannel[5] < 0)
		Page.uiChannel[5] = 0;
	if (Page.uiChannel[6] < 0)
		Page.uiChannel[6] = 0;
	if (Page.uiChannel[7] < 0)
		Page.uiChannel[7] = 0;
	if (Page.uiChannel[8] < 0)
		Page.uiChannel[8] = 0;
	if (Page.uiChannel[9] < 0)
		Page.uiChannel[9] = 0;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Channel7
	BufData[16] = (BYTE)((Page.uiChannel[6] & 0xFF00) >> 8);
	BufData[17] = (BYTE)Page.uiChannel[6] & 0xFF;

	// Channel8
	BufData[18] = (BYTE)((Page.uiChannel[7] & 0xFF00) >> 8);
	BufData[19] = (BYTE)Page.uiChannel[7] & 0xFF;

	// Channel9
	BufData[20] = (BYTE)((Page.uiChannel[8] & 0xFF00) >> 8);
	BufData[21] = (BYTE)Page.uiChannel[8] & 0xFF;

	// Channel10
	BufData[22] = (BYTE)((Page.uiChannel[9] & 0xFF00) >> 8);
	BufData[23] = (BYTE)Page.uiChannel[9] & 0xFF;

	// Check Sum
	BufData[24] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15] ^ BufData[16] ^ BufData[17] ^ BufData[18] ^ BufData[19] ^ BufData[20] ^ BufData[21] ^ BufData[22] ^ BufData[23];

	// End
	BufData[25] = 0xEE;
	BufData[26] = 0xEE;

	m_ComPort.WriteComm(BufData, 27);
	Sleep(300);

}

void CLightControl::SetChannelllumination_10CH(unsigned int uiPageIndex, unsigned int uiChannelIndex, unsigned int uiChannelValue)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255
	BYTE BufData[27];
	SPage Page;

	Page.uiChannel[0] = 0;
	Page.uiChannel[1] = 0;
	Page.uiChannel[2] = 0;
	Page.uiChannel[3] = 0;
	Page.uiChannel[4] = 0;
	Page.uiChannel[5] = 0;
	Page.uiChannel[6] = 0;
	Page.uiChannel[7] = 0;
	Page.uiChannel[8] = 0;
	Page.uiChannel[9] = 0;

	Page.uiChannel[uiChannelIndex] = uiChannelValue;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Channel7
	BufData[16] = (BYTE)((Page.uiChannel[6] & 0xFF00) >> 8);
	BufData[17] = (BYTE)Page.uiChannel[6] & 0xFF;

	// Channel8
	BufData[18] = (BYTE)((Page.uiChannel[7] & 0xFF00) >> 8);
	BufData[19] = (BYTE)Page.uiChannel[7] & 0xFF;

	// Channel9
	BufData[20] = (BYTE)((Page.uiChannel[8] & 0xFF00) >> 8);
	BufData[21] = (BYTE)Page.uiChannel[8] & 0xFF;

	// Channel10
	BufData[22] = (BYTE)((Page.uiChannel[9] & 0xFF00) >> 8);
	BufData[23] = (BYTE)Page.uiChannel[9] & 0xFF;

	// Check Sum
	BufData[24] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15] ^ BufData[16] ^ BufData[17] ^ BufData[18] ^ BufData[19] ^ BufData[20] ^ BufData[21] ^ BufData[22] ^ BufData[23];

	// End
	BufData[25] = 0xEE;
	BufData[26] = 0xEE;

	m_ComPort.WriteComm(BufData, 27);
	Sleep(300);
}

void CLightControl::SetChannelClear_10CH()
{
	BYTE BufData[27];
	SPage Page;
	int uiPageIndex = 0;

	Page.uiChannel[0] = 0;
	Page.uiChannel[1] = 0;
	Page.uiChannel[2] = 0;
	Page.uiChannel[3] = 0;
	Page.uiChannel[4] = 0;
	Page.uiChannel[5] = 0;
	Page.uiChannel[6] = 0;
	Page.uiChannel[7] = 0;
	Page.uiChannel[8] = 0;
	Page.uiChannel[9] = 0;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Channel7
	BufData[16] = (BYTE)((Page.uiChannel[6] & 0xFF00) >> 8);
	BufData[17] = (BYTE)Page.uiChannel[6] & 0xFF;

	// Channel8
	BufData[18] = (BYTE)((Page.uiChannel[7] & 0xFF00) >> 8);
	BufData[19] = (BYTE)Page.uiChannel[7] & 0xFF;

	// Channel9
	BufData[20] = (BYTE)((Page.uiChannel[8] & 0xFF00) >> 8);
	BufData[21] = (BYTE)Page.uiChannel[8] & 0xFF;

	// Channel10
	BufData[22] = (BYTE)((Page.uiChannel[9] & 0xFF00) >> 8);
	BufData[23] = (BYTE)Page.uiChannel[9] & 0xFF;

	// Check Sum
	BufData[24] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15] ^ BufData[16] ^ BufData[17] ^ BufData[18] ^ BufData[19] ^ BufData[20] ^ BufData[21] ^ BufData[22] ^ BufData[23];

	// End
	BufData[25] = 0xEE;
	BufData[26] = 0xEE;

	m_ComPort.WriteComm(BufData, 27);
	Sleep(300);
}

// PageIndex=0 Only
void CLightControl::SetTotalChannelllumination_10CH(unsigned int uiChannelValue1, unsigned int uiChannelValue2, unsigned int uiChannelValue3, unsigned int uiChannelValue4, unsigned int uiChannelValue5, unsigned int uiChannelValue6, unsigned int uiChannelValue7, unsigned int uiChannelValue8, unsigned int uiChannelValue9, unsigned int uiChannelValue10)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255
	BYTE BufData[27];
	SPage Page;
	int uiPageIndex = 0;
	Page.uiChannel[0] = uiChannelValue1;
	Page.uiChannel[1] = uiChannelValue2;
	Page.uiChannel[2] = uiChannelValue3;
	Page.uiChannel[3] = uiChannelValue4;
	Page.uiChannel[4] = uiChannelValue5;
	Page.uiChannel[5] = uiChannelValue6;
	Page.uiChannel[6] = uiChannelValue7;
	Page.uiChannel[7] = uiChannelValue8;
	Page.uiChannel[8] = uiChannelValue9;
	Page.uiChannel[9] = uiChannelValue10;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel[0] & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel[0] & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel[1] & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel[1] & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel[2] & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel[2] & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel[3] & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel[3] & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel[4] & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel[4] & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel[5] & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel[5] & 0xFF;

	// Channel7
	BufData[16] = (BYTE)((Page.uiChannel[6] & 0xFF00) >> 8);
	BufData[17] = (BYTE)Page.uiChannel[6] & 0xFF;

	// Channel8
	BufData[18] = (BYTE)((Page.uiChannel[7] & 0xFF00) >> 8);
	BufData[19] = (BYTE)Page.uiChannel[7] & 0xFF;

	// Channel9
	BufData[20] = (BYTE)((Page.uiChannel[8] & 0xFF00) >> 8);
	BufData[21] = (BYTE)Page.uiChannel[8] & 0xFF;

	// Channel10
	BufData[22] = (BYTE)((Page.uiChannel[9] & 0xFF00) >> 8);
	BufData[23] = (BYTE)Page.uiChannel[9] & 0xFF;

	// Check Sum
	BufData[24] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15] ^ BufData[16] ^ BufData[17] ^ BufData[18] ^ BufData[19] ^ BufData[20] ^ BufData[21] ^ BufData[22] ^ BufData[23];

	// End
	BufData[25] = 0xEE;
	BufData[26] = 0xEE;

	m_ComPort.WriteComm(BufData, 27);
	Sleep(300);
}
