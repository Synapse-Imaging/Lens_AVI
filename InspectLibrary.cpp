#include "stdafx.h"
#include "uScan.h"
#include "InspectLibrary.h"

CInspectLibrary::CInspectLibrary(void)
{
	Reset();
}

CInspectLibrary::~CInspectLibrary(void)
{
	SAFE_DELETE(m_pROI);
}

void CInspectLibrary::Reset()
{
	m_sLibraryName = _T("Lib");
	m_sLibraryComment = _T(".");
	m_iModelVersion = -1;

	m_pROI = NULL;
}

void CInspectLibrary::SetLibrary(CString sLibName, GTRegion *pRegion, CString sComment)
{
	m_sLibraryName = sLibName;
	m_sLibraryComment = sComment;
	m_iModelVersion = THEAPP.GetProgramVersion();

	SAFE_DELETE(m_pROI);
	pRegion->Duplicate(&m_pROI);
}


