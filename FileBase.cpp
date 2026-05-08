// FileBase.cpp : implementation file
//
#include "stdafx.h"
#include "uScan.h"
#include "FileBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileBase
CFileBase::CFileBase()
{
	m_pFile = NULL;
	m_bOpened = FALSE;

	ResetFolderName();
}

CFileBase::~CFileBase()
{
	CloseDataFile();
}

/////////////////////////////////////////////////////////////////////////////
// CFileBase message handlers

void CFileBase::ResetFolderName()
{
	for (int iMzNo = 0; iMzNo < MAX_MAGAZINE_NO; iMzNo++)
	{
		m_strDayResultLogFolder_Local[iMzNo] = _T("");						// CMI_Results\\yyyy\\mm\\dd
		m_strDayResultLogFolder_LAS[iMzNo] = _T("");						// CMI_Results\\yyyy\\mm\\dd
	}

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_strLastRawImageFolder_Local[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_LAS[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastResultImageFolder[i] = _T("");							// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage

		m_strLastRawImageFolder_Temp1[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_Temp2[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_Temp3[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage

		for (int j = 0; j < MAX_MAGAZINE_NO; j++)
		{
			m_strResultLogFolder_Local[i][j] = _T("");					// CMI_Results\\yyyy\\mm\\dd\\LotID
			m_strResultLogFolder_LAS[i][j] = _T("");					// CMI_Results\\yyyy\\mm\\dd\\LotID
			for (int k = 0; k < MAX_TRAY_LOT; k++)
			{
				m_strAIFolder_Local[i][j][k] = _T("");					// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx
				m_strAIFolder_LAS[i][j][k] = _T("");					// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx

				m_strRawImageFolder_Local[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
				m_strResultImageFolder_Local[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
				m_strReviewImageFolder_Local[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
				m_strADJImageFolder_Local[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\AdjImage
				m_strFAIImageFolder_Local[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

				m_strRawImageFolder_LAS[i][j][k] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
				m_strResultImageFolder_LAS[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
				m_strReviewImageFolder_LAS[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
				m_strADJImageFolder_LAS[i][j][k] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ADJImage
				m_strFAIImageFolder_LAS[i][j][k] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

				m_strRawImageFolder_Temp1[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
				m_strRawImageFolder_Temp2[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
				m_strRawImageFolder_Temp3[i][j][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
			}
		}
	}
}

void CFileBase::ResetMzFolderName(int iMzNo)
{
	m_strDayResultLogFolder_Local[iMzNo - 1] = _T("");					// CMI_Results\\yyyy\\mm\\dd
	m_strDayResultLogFolder_LAS[iMzNo - 1] = _T("");					// CMI_Results\\yyyy\\mm\\dd

	for (int i = 0; i < VISION_NUMBER_MAX; i++)
	{
		m_strLastRawImageFolder_Local[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_LAS[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastResultImageFolder[i] = _T("");							// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage

		m_strResultLogFolder_Local[i][iMzNo - 1] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID
		m_strResultLogFolder_LAS[i][iMzNo - 1] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID

		m_strLastRawImageFolder_Temp1[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_Temp2[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		m_strLastRawImageFolder_Temp3[i] = _T("");						// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage

		for (int k = 0; k < MAX_TRAY_LOT; k++)
		{
			m_strAIFolder_Local[i][iMzNo - 1][k] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx
			m_strAIFolder_LAS[i][iMzNo - 1][k] = _T("");				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx

			m_strRawImageFolder_Local[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
			m_strResultImageFolder_Local[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
			m_strReviewImageFolder_Local[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
			m_strADJImageFolder_Local[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ADJImage
			m_strFAIImageFolder_Local[i][iMzNo - 1][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

			m_strRawImageFolder_LAS[i][iMzNo - 1][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
			m_strResultImageFolder_LAS[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
			m_strReviewImageFolder_LAS[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImag
			m_strADJImageFolder_LAS[i][iMzNo - 1][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ADJImage
			m_strFAIImageFolder_LAS[i][iMzNo - 1][k] = _T("");			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

			m_strRawImageFolder_Temp1[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
			m_strRawImageFolder_Temp2[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
			m_strRawImageFolder_Temp3[i][iMzNo - 1][k] = _T("");		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
		}
	}
}

void CFileBase::SetFilePtr(CFile* pFile)
{
	m_pFile = pFile;
}

void CFileBase::CreateFolder(CString szFolder)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFolder, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
		CreateDirectory(szFolder, NULL);

	// SW Down Bug Fix
	FindClose(hFind);
}

void CFileBase::CreatePath(CString csPath, BOOL bFileNameExist)
{
	if (bFileNameExist)
		csPath = csPath.Left(csPath.ReverseFind('\\'));

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(csPath, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		CString csToken(_T(""));
		CString csResultPath(_T(""));

		csResultPath = "";

		int nStart = 0, nEnd;

		BOOL bNetworkIP = FALSE;

		if (csPath.Find("\\\\", 0) == 0)
		{
			bNetworkIP = TRUE;
		}

		if (bNetworkIP)
		{
			csPath.Delete(0, 2);
		}

		while ((nEnd = csPath.Find('\\', nStart)) >= 0)
		{
			AfxExtractSubString(csToken, csPath, nStart, '\\');
			if (!csToken.Compare("")) break;
			if (nStart != 0)
				csResultPath += ('\\' + csToken);
			else
			{
				if (bNetworkIP)
				{
					csResultPath += "\\\\";
				}
				csResultPath += (csToken);
			}

			if (GetFileAttributes(csResultPath) == -1)
				CreateDirectory(csResultPath, NULL);

			nStart += 1;
		}
	}

	// SW Down Bug Fix
	FindClose(hFind);
}

void CFileBase::DeleteFolder(CString szFolder)
{
	RemoveDirectory(szFolder);
}

void CFileBase::SetFolderFile(CString szFolder, CString szFile)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFolder, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
		CreateDirectory(szFolder, NULL);

	FindClose(hFind);

	m_szFilePath = szFolder + "\\" + szFile;
}

BOOL CFileBase::OpenDataFile(UINT nOpenFlags)
{
	m_bOpened = FALSE;
	if (m_pFile)
		if (m_pFile->Open(m_szFilePath, nOpenFlags))
			m_bOpened = TRUE;

	return m_bOpened;
}

void CFileBase::CloseDataFile()
{
	if (m_pFile && m_bOpened)
		m_pFile->Close();
	m_bOpened = FALSE;
}

BOOL CFileBase::ValidateFileWriting(CString sSaveFilePath, int iTimeOut)
{
	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;

	double tStart, tEnd, tElapsed;
	BOOL bPassedFileExistence;
	bPassedFileExistence = FALSE;
	HObject HTempImage;

	BOOL bWriteOK = FALSE;

	tStart = GetTickCount();

	while (1)
	{
		// Physical File Existence Check
		if (bPassedFileExistence == FALSE)
		{
			if ((hFindFile = FindFirstFile(sSaveFilePath, &FindFileData)) == INVALID_HANDLE_VALUE)
			{
				tEnd = GetTickCount();
				tElapsed = tEnd - tStart;

				if (tElapsed > iTimeOut)
					break;

				Sleep(10);
				continue;
			}

			FindClose(hFindFile);

			bPassedFileExistence = TRUE;
		}

		// Reading File Actually
		try
		{
			ReadImage(&HTempImage, (HTuple)sSaveFilePath);

			tEnd = GetTickCount();
			tElapsed = tEnd - tStart;
			strLog.Format("ValidateFileWriting, File: %s, Time(ms): %d", sSaveFilePath, (int)tElapsed);
			THEAPP.m_log_system->debug("{}", LOG_CSTR(strLog));

			bWriteOK = TRUE;
			break;
		}
		catch (HException& e)
		{
			tEnd = GetTickCount();
			tElapsed = tEnd - tStart;

			if (tElapsed > iTimeOut)
				break;

			Sleep(50);
			continue;
		}
	}

	if (bWriteOK == FALSE)
	{
		strLog.Format("ValidateFileWriting timeout, File: %s", sSaveFilePath);
		THEAPP.m_log_system->warn("{}", LOG_CSTR(strLog));
	}

	return bWriteOK;
}
