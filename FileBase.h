// FileBase.h : header file
//
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CFileBase window

class CFileBase : public CWnd
{
public:
	CFileBase();
	virtual ~CFileBase();

protected:
	CFile*		m_pFile;
	BOOL		m_bOpened;
	CString		m_szFilePath;

public:
	CString m_strDayResultLogFolder_Local[MAX_MAGAZINE_NO];			// CMI_Results\\yyyy\\mm\\dd
	CString m_strDayResultLogFolder_LAS[MAX_MAGAZINE_NO];			// CMI_Results\\yyyy\\mm\\dd

	CString m_strLastRawImageFolder_Local[VISION_NUMBER_MAX];		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strLastRawImageFolder_LAS[VISION_NUMBER_MAX];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strLastResultImageFolder[VISION_NUMBER_MAX];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage

	CString m_strLastRawImageFolder_Temp1[VISION_NUMBER_MAX];		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strLastRawImageFolder_Temp2[VISION_NUMBER_MAX];		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strLastRawImageFolder_Temp3[VISION_NUMBER_MAX];		// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage

	CString m_strResultLogFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO];							// CMI_Results\\yyyy\\mm\\dd\\LotID
	CString m_strResultLogFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO];							// CMI_Results\\yyyy\\mm\\dd\\LotID

	CString m_strAIFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];					// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx
	CString m_strAIFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];					// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx

	CString m_strRawImageFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strResultImageFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
	CString m_strReviewImageFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
	CString m_strADJImageFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\AdjImage
	CString m_strFAIImageFolder_Local[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

	CString m_strRawImageFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strResultImageFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ResultImage
	CString m_strReviewImageFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\ReviewImage
	CString m_strADJImageFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\AdjImage
	CString m_strFAIImageFolder_LAS[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];				// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\FAIImage

	CString m_strRawImageFolder_Temp1[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strRawImageFolder_Temp2[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage
	CString m_strRawImageFolder_Temp3[VISION_NUMBER_MAX][MAX_MAGAZINE_NO][MAX_TRAY_LOT];			// CMI_Results\\yyyy\\mm\\dd\\LotID\\Tray-xx\\RawImage


	CString m_strTestBenchSavePath;
	
	void ResetFolderName();
	void ResetMzFolderName(int iMzNo);
	void SetFilePtr(CFile* pFile);
	void SetFolderFile(CString szFolder, CString szFile);
	void CreateFolder(CString szFolder);
	void CreatePath(CString csPath, BOOL bFileNameExist=FALSE);
	void DeleteFolder(CString szFolder);

	virtual BOOL OpenDataFile(UINT nOpenFlags = CFile::modeRead);
	virtual void CloseDataFile();

	BOOL ValidateFileWriting(CString sSaveFilePath, int iTimeOut = 500);

	CString strLog;
};

/////////////////////////////////////////////////////////////////////////////
