// LightControl.h: interface for the CLightControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_)
#define AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComThread.h"

struct SPage
{
	CString m_sPageDesc;
	unsigned int uiChannel[LIGHT_CH_CNT];
};

class CLightControl  
{
public:
	CLightControl();
	virtual ~CLightControl();

	// New Controller
	void SetGrabSequence(int iLightControllerIndex, int iAddrIndex, int *piPageIndex, int iSeqCount);
	void SetOnTime_Page(int iLightControllerIndex, int iPageIndex);
	void SetOnTime_Channel(int iLightControllerIndex, int iPageIndex, int iChannelNumber, int iOnTime);

	void SetIllumination_4CH(unsigned int uiPageIndex = 0);
	void SetChannelllumination_4CH(unsigned int uiPageIndex, unsigned int uiChannelIndex, unsigned int uiChannelValue);
	void SetChannelClear_4CH();
	void SetTotalChannelllumination_4CH(unsigned int uiChannelValue1, unsigned int uiChannelValue2, unsigned int uiChannelValue3, unsigned int uiChannelValue4);

	void SetIllumination_10CH(unsigned int uiPageIndex = 0);
	void SetChannelllumination_10CH(unsigned int uiPageIndex, unsigned int uiChannelIndex, unsigned int uiChannelValue);
	void SetChannelClear_10CH();
	void SetTotalChannelllumination_10CH(unsigned int uiChannelValue1, unsigned int uiChannelValue2,  unsigned int uiChannelValue3, unsigned int uiChannelValue4,  unsigned int uiChannelValue5, unsigned int uiChannelValue6, unsigned int uiChannelValue7, unsigned int uiChannelValue8, unsigned int uiChannelValue9, unsigned int uiChannelValue10);

	int m_iComPortNumber;
	CCommThread m_ComPort;

	// Page
	SPage m_Page[LIGHTCTRL_PAGE_COUNT];

	// Grab Sequence
	int m_iAddrSeq[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT][LIGHTCTRL_PAGE_COUNT];
	int m_iAddrCount[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];
	CString m_sAddrDesc[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];
	int m_iWSSDelayTime[LIGHTCTRL_GRAB_ADDR_SEQ_COUNT];
};

#endif // !defined(AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_)
