#include "stdafx.h"
#include "uScan.h"
#include "LightLibrary.h"

CLightLibrary::CLightLibrary(void)
{
	Reset();
}

CLightLibrary::~CLightLibrary(void)
{
}

void CLightLibrary::Reset()
{
	m_sLibraryName = _T("Lib");
	m_sLibraryComment = _T(".");
}

void CLightLibrary::SetLibrary(CString sLibName, SPage *pPage, CString sComment)
{
	m_sLibraryName = sLibName;
	m_sLibraryComment = sComment;

	m_Page.m_sPageDesc = pPage->m_sPageDesc;

	for (int i = 0; i < 32; i++)
		m_Page.uiChannel[i] = pPage->uiChannel[i];
}


