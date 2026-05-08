#pragma once

#include "AJinAXL.h"

class CTriggerManager
{
public:

	static CTriggerManager* m_pInstance;
	static CTriggerManager* GetInstance(BOOL bShowFlag=FALSE);
	void   DeleteInstance();
	CTriggerManager(void);
	~CTriggerManager(void);

	BOOL Initialize();
	void SetTriggerSleepTime(int iMsec) { m_iSleepTime = iMsec; }

	void GrabSeqChangeTrigger(int iGrabSeqIndex);

	void GrabSeqFireTrigger();

	void Delay(int msec);

	void GrabSeqFireTriggerByEthernet(int iLightControllerIndex, int iGrabSeqIndex=0);

	void GrabSeqChangeByEthernet(int iLightControllerIndex, int iGrabSeqIndex, BOOL bPreDelay=FALSE, BOOL bPostDelay = FALSE);

	void GrabFireTriggerByEthernet(int iLightControllerIndex);

	void AutoLightSequenceSet(int iLightControllerIndex, int iAddrIndex, int iSeqCount);

	void AutoLightPageSet_Single(int iLightControllerIndex, int iPageIndex, int iChannelIndex, int iChannelValue, BOOL bCoaxial);

	void AutoLightPageSet_Multi(int iLightControllerIndex, int iPageIndex, const std::vector<std::pair<int, int>>& vLightGroup);

protected:
	CAJinAXL m_AJinAXL;
	int		m_iSleepTime;
};

