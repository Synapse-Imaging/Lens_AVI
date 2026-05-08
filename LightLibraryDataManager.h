#pragma once

#include "LightLibraryData.h"

class CLightLibraryDataManager : public CWnd
{
public:
	CLightLibraryDataManager();
	virtual ~CLightLibraryDataManager();

	static CLightLibraryDataManager* GetInstance(BOOL bShowFlag = FALSE);
	void   DeleteInstance();

protected:
	DECLARE_MESSAGE_MAP()

	// Attributes
public:
	CLightLibraryData* GetLightLibraryDataPtr() { return &m_LightLibraryData; }

	// Operations
public:
	void	Initialize();
	void	UnInitialize();
	void	ResetAllData();
	void	Load();
	void	Save();

protected:
	CLightLibraryData	m_LightLibraryData;

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CLightLibraryDataManager*	m_pInstance;

};

