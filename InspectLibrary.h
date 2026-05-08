#pragma once

#include "GTRegion.h"

class CInspectLibrary
{
public:
	CInspectLibrary(void);
	virtual ~CInspectLibrary(void);

	CString m_sLibraryName;
	CString m_sLibraryComment;
	int		m_iModelVersion;
	GTRegion *m_pROI;

public:
	void Reset();
	void SetLibrary(CString sLibName, GTRegion *pRegion, CString sComment);
};

