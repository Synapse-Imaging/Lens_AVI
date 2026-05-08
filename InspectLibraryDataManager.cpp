#include "stdafx.h"
#include "uScan.h"
#include "InspectLibraryDataManager.h"

/////////////////////////////////////////////////////////////////////////////
// CInspectLibraryDataManager

CInspectLibraryDataManager* CInspectLibraryDataManager::m_pInstance = NULL;

CInspectLibraryDataManager* CInspectLibraryDataManager::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CInspectLibraryDataManager;

		if (!m_pInstance->m_hWnd)
		{
			CRect r(-20, -20, 20, 20);

			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "InspectLibraryDataManager", 0, r, NULL, 0, NULL);
		}
	}

	return m_pInstance;
}

void CInspectLibraryDataManager::DeleteInstance()
{
	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

CInspectLibraryDataManager::CInspectLibraryDataManager()
{
}

CInspectLibraryDataManager::~CInspectLibraryDataManager()
{
}

BEGIN_MESSAGE_MAP(CInspectLibraryDataManager, CWnd)
	//{{AFX_MSG_MAP(CInspectLibraryDataManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectLibraryDataManager message handlers

void CInspectLibraryDataManager::Initialize()
{
	ResetAllData();

	CString sLibraryFolder;
	sLibraryFolder = THEAPP.GetWorkingDirectory() + "\\Library";

	m_InspectLibraryData.SetFolderFile(sLibraryFolder, "InspectLibrary.dat");

	Load();
}

void CInspectLibraryDataManager::UnInitialize()
{
}

void CInspectLibraryDataManager::ResetAllData()
{
	m_InspectLibraryData.ClearAll();
}

void CInspectLibraryDataManager::Load()
{
	if (m_InspectLibraryData.OpenDataFile())
	{
		m_InspectLibraryData.Load();
	}
}

void CInspectLibraryDataManager::Save()
{
	m_InspectLibraryData.Save();
}
