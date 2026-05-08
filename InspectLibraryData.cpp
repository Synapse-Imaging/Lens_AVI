#include "stdafx.h"
#include "uScan.h"
#include "InspectLibraryData.h"

CInspectLibraryData::CInspectLibraryData(void)
{
	SetFilePtr(&m_File);

	m_pInspectLibraryList = NULL;
}

CInspectLibraryData::~CInspectLibraryData(void)
{
	if (m_pInspectLibraryList)
		ClearAllInspectLibrary();

	SAFE_DELETE(m_pInspectLibraryList);
}

void CInspectLibraryData::ClearAll()
{
	if (m_pInspectLibraryList)
		ClearAllInspectLibrary();

	SAFE_DELETE(m_pInspectLibraryList);

	m_pInspectLibraryList = new PList<CInspectLibrary>(PLNO_POINTER);
}

int CInspectLibraryData::GetInspectLibraryCount()
{
	if (m_pInspectLibraryList == NULL)
		return 0;
	else
		return m_pInspectLibraryList->miCount;
}

void CInspectLibraryData::AddInspectLibrary(CInspectLibrary *pInspectLibrary)
{
	m_pInspectLibraryList->Add(pInspectLibrary);
}

void CInspectLibraryData::DeleteInspectLibrary(int iIndex)
{
	if (m_pInspectLibraryList == NULL)
		return;

	CInspectLibrary *pInspectLibrary;

	pInspectLibrary = GetInspectLibrary(iIndex);
	SAFE_DELETE(pInspectLibrary);

	m_pInspectLibraryList->Delete(iIndex);
}

void CInspectLibraryData::DeleteInspectLibraryPtr(CInspectLibrary *pInspectLibrary)
{
	if (m_pInspectLibraryList == NULL)
		return;

	for (int i = 0; i < GetInspectLibraryCount(); i++)
	{
		if (GetInspectLibrary(i) == pInspectLibrary)
		{
			DeleteInspectLibrary(i);
			return;
		}
	}
}

void CInspectLibraryData::ClearAllInspectLibrary()
{
	if (m_pInspectLibraryList == NULL)
		return;

	CInspectLibrary *pInspectLibrary;

	for (int i = 0; i < GetInspectLibraryCount(); i++) {
		pInspectLibrary = GetInspectLibrary(i);
		SAFE_DELETE(pInspectLibrary);
	}

	m_pInspectLibraryList->ClearAll();
}

CInspectLibrary* CInspectLibraryData::GetInspectLibrary(int iIndex)
{
	if (m_pInspectLibraryList == NULL)
		return NULL;
	else
		return m_pInspectLibraryList->Get(iIndex);
}

//////////////////////////////////////////////////////////////////////////

void CInspectLibraryData::Load()
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

		HTuple HFileHandle = -1;
		CString szTemplateModelFilename;
		CString sLibraryFolder;
		sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";

		szTemplateModelFilename = sLibraryFolder + "\\LibraryTemplate.dat";

		HANDLE				hFindFile;
		WIN32_FIND_DATA		FindFileData;

		if ((hFindFile = FindFirstFile(szTemplateModelFilename, &FindFileData)) != INVALID_HANDLE_VALUE)
		{
			OpenFile(HTuple(szTemplateModelFilename), "input_binary", &HFileHandle);
			FindClose(hFindFile);
		}

		CInspectLibrary *pInspectLibrary;
		char cTempRead[100];
		int iStringLength;
		int iModelVersion;

		for (i = 0; i < iNoLibrary; i++)
		{
			pInspectLibrary = new CInspectLibrary;

			ReadFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesRead, NULL);
			ReadFile((HANDLE)m_File.m_hFile, cTempRead, iStringLength, &dwBytesRead, NULL);
			pInspectLibrary->m_sLibraryName.Empty();
			cTempRead[iStringLength] = '\0';
			pInspectLibrary->m_sLibraryName.Format("%s", cTempRead);

			ReadFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesRead, NULL);
			ReadFile((HANDLE)m_File.m_hFile, cTempRead, iStringLength, &dwBytesRead, NULL);
			pInspectLibrary->m_sLibraryComment.Empty();
			cTempRead[iStringLength] = '\0';
			pInspectLibrary->m_sLibraryComment.Format("%s", cTempRead);

			ReadFile((HANDLE)m_File.m_hFile, &(pInspectLibrary->m_iModelVersion), sizeof(int), &dwBytesRead, NULL);

			SAFE_DELETE(pInspectLibrary->m_pROI);
			pInspectLibrary->m_pROI = new GTRegion;
			pInspectLibrary->m_pROI->Load((HANDLE)m_File.m_hFile, pInspectLibrary->m_iModelVersion, HFileHandle, THEAPP.m_pCalDataService, 0, "");

			AddInspectLibrary(pInspectLibrary);
		}

		if (HFileHandle != -1)
			CloseFile(HFileHandle);

		CloseDataFile();
	}
}

void CInspectLibraryData::Save()
{
	if (!m_bOpened)
		OpenDataFile(CFile::modeCreate | CFile::modeWrite);

	if (m_bOpened)
	{
		DWORD dwBytesWritten;
		int i;
		int iNoLibrary;

		iNoLibrary = GetInspectLibraryCount();

		WriteFile((HANDLE)m_File.m_hFile, &iNoLibrary, sizeof(int), &dwBytesWritten, NULL);

		HTuple HFileHandle = -1;
		CString szTemplateModelFilename;
		CString sLibraryFolder;
		sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";
		szTemplateModelFilename = sLibraryFolder + "\\LibraryTemplate.dat";

		OpenFile(HTuple(szTemplateModelFilename), "output_binary", &HFileHandle);

		CInspectLibrary *pInspectLibrary;
		int iStringLength;

		for (i = 0; i < iNoLibrary; i++)
		{
			pInspectLibrary = GetInspectLibrary(i);

			if (pInspectLibrary != NULL)
			{
				if (pInspectLibrary->m_pROI != NULL)
				{
					iStringLength = pInspectLibrary->m_sLibraryName.GetLength();
					WriteFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesWritten, NULL);
					WriteFile((HANDLE)m_File.m_hFile, (LPCTSTR)pInspectLibrary->m_sLibraryName, iStringLength, &dwBytesWritten, NULL);

					iStringLength = pInspectLibrary->m_sLibraryComment.GetLength();
					WriteFile((HANDLE)m_File.m_hFile, &iStringLength, sizeof(int), &dwBytesWritten, NULL);
					WriteFile((HANDLE)m_File.m_hFile, (LPCTSTR)pInspectLibrary->m_sLibraryComment, iStringLength, &dwBytesWritten, NULL);

					pInspectLibrary->m_iModelVersion = THEAPP.GetProgramVersion();
					WriteFile((HANDLE)m_File.m_hFile, &pInspectLibrary->m_iModelVersion, sizeof(int), &dwBytesWritten, NULL);

					pInspectLibrary->m_pROI->Save((HANDLE)m_File.m_hFile, HFileHandle, FALSE, FALSE, 0, "");
				}
			}
		}

		if (HFileHandle != -1)
			CloseFile(HFileHandle);

		CloseDataFile();
	}
}

BOOL CInspectLibraryData::LibraryExist(CString sLibName)
{
	CInspectLibrary *pInspectLibrary;
	CString str, sName;

	int iNoLibrary = GetInspectLibraryCount();

	for (int i = 0; i < iNoLibrary; i++)
	{
		pInspectLibrary = GetInspectLibrary(i);

		if (pInspectLibrary->m_sLibraryName == sLibName)
			return TRUE;
	}

	return FALSE;
}

BOOL CInspectLibraryData::DeleteSelectedLibrary(CString sLibName)
{
	CInspectLibrary *pInspectLibrary;
	CString str, sName;

	int iNoLibrary = GetInspectLibraryCount();

	int iSelectedLibIndex = -1;

	for (int i = 0; i < iNoLibrary; i++)
	{
		pInspectLibrary = GetInspectLibrary(i);

		if (pInspectLibrary->m_sLibraryName == sLibName)
		{
			iSelectedLibIndex = i;
			break;
		}
	}

	if (iSelectedLibIndex == -1)
		return FALSE;

	DeleteInspectLibrary(iSelectedLibIndex);

	return TRUE;
}

CInspectLibrary* CInspectLibraryData::GetSelectedLibrary(CString sLibName)
{
	CInspectLibrary *pInspectLibrary;
	CString str, sName;

	int iNoLibrary = GetInspectLibraryCount();

	int iSelectedLibIndex = -1;

	for (int i = 0; i < iNoLibrary; i++)
	{
		pInspectLibrary = GetInspectLibrary(i);

		if (pInspectLibrary->m_sLibraryName == sLibName)
		{
			iSelectedLibIndex = i;
			break;
		}
	}

	if (iSelectedLibIndex == -1)
		return NULL;

	return GetInspectLibrary(iSelectedLibIndex);
}


