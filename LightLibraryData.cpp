#include "stdafx.h"
#include "uScan.h"
#include "LightLibraryData.h"

CLightLibraryData::CLightLibraryData(void)
{
	SetFilePtr(&m_File);

	m_pLightLibraryList = NULL;
}

CLightLibraryData::~CLightLibraryData(void)
{
	if (m_pLightLibraryList)
		ClearAllLightLibrary();

	SAFE_DELETE(m_pLightLibraryList);
}

void CLightLibraryData::ClearAll()
{
	if (m_pLightLibraryList)
		ClearAllLightLibrary();

	SAFE_DELETE(m_pLightLibraryList);

	m_pLightLibraryList = new PList<CLightLibrary>(PLNO_POINTER);
}

int CLightLibraryData::GetLightLibraryCount()
{
	if (m_pLightLibraryList == NULL)
		return 0;
	else
		return m_pLightLibraryList->miCount;
}

void CLightLibraryData::AddLightLibrary(CLightLibrary *pLightLibrary)
{
	m_pLightLibraryList->Add(pLightLibrary);
}

void CLightLibraryData::DeleteLightLibrary(int iIndex)
{
	if (m_pLightLibraryList == NULL)
		return;

	CLightLibrary *pLightLibrary;

	pLightLibrary = GetLightLibrary(iIndex);
	SAFE_DELETE(pLightLibrary);

	m_pLightLibraryList->Delete(iIndex);
}

void CLightLibraryData::DeleteLightLibraryPtr(CLightLibrary *pLightLibrary)
{
	if (m_pLightLibraryList == NULL)
		return;

	for (int i = 0; i < GetLightLibraryCount(); i++)
	{
		if (GetLightLibrary(i) == pLightLibrary)
		{
			DeleteLightLibrary(i);
			return;
		}
	}
}

void CLightLibraryData::ClearAllLightLibrary()
{
	if (m_pLightLibraryList == NULL)
		return;

	CLightLibrary *pLightLibrary;

	for (int i = 0; i < GetLightLibraryCount(); i++) {
		pLightLibrary = GetLightLibrary(i);
		SAFE_DELETE(pLightLibrary);
	}

	m_pLightLibraryList->ClearAll();
}

CLightLibrary* CLightLibraryData::GetLightLibrary(int iIndex)
{
	if (m_pLightLibraryList == NULL)
		return NULL;
	else
		return m_pLightLibraryList->Get(iIndex);
}

//////////////////////////////////////////////////////////////////////////

void CLightLibraryData::Load()
{
	if (!m_bOpened)
		OpenDataFile();

	if (m_bOpened)
	{
		DWORD dwBytesRead;
		int i;
		int iNoLibrary = 0;

		ReadFile((HANDLE)m_File.m_hFile, &iNoLibrary, sizeof(int), &dwBytesRead, NULL);

		ClearAll();

		CString sLibraryFolder;
		sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";

		CLightLibrary *pLightLibrary;
		char cTempRead[100];
		int iStringLength;

		for (i = 0; i < iNoLibrary; i++)
		{
			pLightLibrary = new CLightLibrary;

			ReadFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesRead, NULL);
			ReadFile((HANDLE)m_File.m_hFile, cTempRead, iStringLength, &dwBytesRead, NULL);
			pLightLibrary->m_sLibraryName.Empty();
			cTempRead[iStringLength] = '\0';
			pLightLibrary->m_sLibraryName.Format("%s", cTempRead);

			ReadFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesRead, NULL);
			ReadFile((HANDLE)m_File.m_hFile, cTempRead, iStringLength, &dwBytesRead, NULL);
			pLightLibrary->m_sLibraryComment.Empty();
			cTempRead[iStringLength] = '\0';
			pLightLibrary->m_sLibraryComment.Format("%s", cTempRead);

			for (int j = 0; j < 32; j++)
				ReadFile((HANDLE)m_File.m_hFile, &(pLightLibrary->m_Page.uiChannel[j]), sizeof(int), &dwBytesRead, NULL);

			AddLightLibrary(pLightLibrary);
		}

		CloseDataFile();
	}
}

void CLightLibraryData::Save()
{
	if (!m_bOpened)
		OpenDataFile(CFile::modeCreate | CFile::modeWrite);

	if (m_bOpened)
	{
		DWORD dwBytesWritten;
		int i;
		int iNoLibrary;

		iNoLibrary = GetLightLibraryCount();

		WriteFile((HANDLE)m_File.m_hFile, &iNoLibrary, sizeof(int), &dwBytesWritten, NULL);

		CString sLibraryFolder;
		sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";

		CLightLibrary *pLightLibrary;
		int iStringLength;

		for (i = 0; i < iNoLibrary; i++)
		{
			pLightLibrary = GetLightLibrary(i);

			if (pLightLibrary != NULL)
			{
				iStringLength = pLightLibrary->m_sLibraryName.GetLength();
				WriteFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesWritten, NULL);
				WriteFile((HANDLE)m_File.m_hFile, (LPCTSTR)pLightLibrary->m_sLibraryName, iStringLength, &dwBytesWritten, NULL);

				iStringLength = pLightLibrary->m_sLibraryComment.GetLength();
				WriteFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesWritten, NULL);
				WriteFile((HANDLE)m_File.m_hFile, (LPCTSTR)pLightLibrary->m_sLibraryComment, iStringLength, &dwBytesWritten, NULL);

				for (int j = 0; j < 32; j++)
					WriteFile((HANDLE)m_File.m_hFile, &(pLightLibrary->m_Page.uiChannel[j]), sizeof(int), &dwBytesWritten, NULL);
			}
		}

		CloseDataFile();
	}
}

BOOL CLightLibraryData::LibraryExist(CString sLibName)
{
	CLightLibrary *pLightLibrary;
	CString str, sName;

	int iNoLibrary = GetLightLibraryCount();

	for (int i = 0; i < iNoLibrary; i++)
	{
		pLightLibrary = GetLightLibrary(i);

		if (pLightLibrary->m_sLibraryName == sLibName)
			return TRUE;
	}

	return FALSE;
}

BOOL CLightLibraryData::DeleteSelectedLibrary(CString sLibName)
{
	CLightLibrary *pLightLibrary;
	CString str, sName;

	int iNoLibrary = GetLightLibraryCount();

	int iSelectedLibIndex = -1;

	for (int i = 0; i < iNoLibrary; i++)
	{
		pLightLibrary = GetLightLibrary(i);

		if (pLightLibrary->m_sLibraryName == sLibName)
		{
			iSelectedLibIndex = i;
			break;
		}
	}

	if (iSelectedLibIndex == -1)
		return FALSE;

	DeleteLightLibrary(iSelectedLibIndex);

	return TRUE;
}

CLightLibrary* CLightLibraryData::GetSelectedLibrary(CString sLibName)
{
	CLightLibrary *pLightLibrary;
	CString str, sName;

	int iNoLibrary = GetLightLibraryCount();

	int iSelectedLibIndex = -1;

	for (int i = 0; i < iNoLibrary; i++)
	{
		pLightLibrary = GetLightLibrary(i);

		if (pLightLibrary->m_sLibraryName == sLibName)
		{
			iSelectedLibIndex = i;
			break;
		}
	}

	if (iSelectedLibIndex == -1)
		return NULL;

	return GetLightLibrary(iSelectedLibIndex);
}


