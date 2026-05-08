#pragma once

#include "LightControl.h"

class CLightLibrary
{
public:
	CLightLibrary(void);
	virtual ~CLightLibrary(void);

	CString m_sLibraryName;
	CString m_sLibraryComment;
	SPage m_Page;

public:
	void Reset();
	void SetLibrary(CString sLibName, SPage *pPage, CString sComment);
};

