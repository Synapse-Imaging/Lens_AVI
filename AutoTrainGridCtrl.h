#pragma once

#include "ugctrl.h"

class CAutoTrainGridCtrl :
	public CUGCtrl
{
public:
	CAutoTrainGridCtrl(void);
	virtual ~CAutoTrainGridCtrl(void);

	virtual void OnSetup();

	virtual BOOL IsItemChecked(long lIndex);
	virtual BOOL SetItemCheck(long lIndex, BOOL bChecked);

protected:
	CFont m_NameFont;
};

