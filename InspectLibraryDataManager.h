#pragma once

#include "InspectLibraryData.h"

class CInspectLibraryDataManager : public CWnd
{
public:
	CInspectLibraryDataManager();
	virtual ~CInspectLibraryDataManager();

	static CInspectLibraryDataManager* GetInstance(BOOL bShowFlag = FALSE);
	void   DeleteInstance();

protected:
	DECLARE_MESSAGE_MAP()

	// Attributes
public:
	CInspectLibraryData* GetInspectLibraryDataPtr() { return &m_InspectLibraryData; }

	// Operations
public:
	void	Initialize();
	void	UnInitialize();
	void	ResetAllData();
	void	Load();
	void	Save();

protected:
	CInspectLibraryData	m_InspectLibraryData;

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CInspectLibraryDataManager*	m_pInstance;

};

