#include "stdafx.h"
#include "uScan.h"
#include "LightLibraryDataManager.h"

/////////////////////////////////////////////////////////////////////////////
// CLightLibraryDataManager

CLightLibraryDataManager* CLightLibraryDataManager::m_pInstance = NULL;

CLightLibraryDataManager* CLightLibraryDataManager::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CLightLibraryDataManager;

		if (!m_pInstance->m_hWnd)
		{
			CRect r(-20, -20, 20, 20);

			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "LightLibraryDataManager", 0, r, NULL, 0, NULL);
		}
	}

	return m_pInstance;
}

void CLightLibraryDataManager::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

CLightLibraryDataManager::CLightLibraryDataManager()
{
}

CLightLibraryDataManager::~CLightLibraryDataManager()
{
}

BEGIN_MESSAGE_MAP(CLightLibraryDataManager, CWnd)
	//{{AFX_MSG_MAP(CLightLibraryDataManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightLibraryDataManager message handlers

void CLightLibraryDataManager::Initialize()
{
	ResetAllData();

	CString sLibraryFolder;
	sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";

	m_LightLibraryData.SetFolderFile(sLibraryFolder, "LightLibrary.dat");

	Load();
}

void CLightLibraryDataManager::UnInitialize()
{
}

void CLightLibraryDataManager::ResetAllData()
{
	m_LightLibraryData.ClearAll();
}

void CLightLibraryDataManager::Load()
{
	if (m_LightLibraryData.OpenDataFile())
	{
		m_LightLibraryData.Load();
	}
}

void CLightLibraryDataManager::Save()
{
	m_LightLibraryData.Save();
}
