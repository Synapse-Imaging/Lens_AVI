#pragma once

#include "ugctrl.h"

class CTeachParamGridCtrl :
	public CUGCtrl
{
public:
	CTeachParamGridCtrl(void);
	virtual ~CTeachParamGridCtrl(void);

	virtual void OnSetup();

	virtual BOOL IsItemChecked(long lIndex);
	virtual BOOL SetItemCheck(long lIndex, BOOL bChecked);

protected:
	CFont m_NameFont;
};

