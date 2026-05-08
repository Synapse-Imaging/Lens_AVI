#pragma once

#include "PBC.h"
#include "FileBase.h"
#include "LightLibrary.h"

class CLightLibraryData : public CFileBase
{
public:
	CLightLibraryData(void);
	virtual ~CLightLibraryData(void);

public:
	void ClearAll();
	void Load();
	void Save();
	BOOL LibraryExist(CString sLibName);
	BOOL DeleteSelectedLibrary(CString sLibName);
	CLightLibrary* GetSelectedLibrary(CString sLibName);

	int GetLightLibraryCount();
	void AddLightLibrary(CLightLibrary *pLightLibrary);
	void DeleteLightLibrary(int iIndex);
	void DeleteLightLibraryPtr(CLightLibrary *pLightLibrary);
	void ClearAllLightLibrary();
	CLightLibrary *GetLightLibrary(int iIndex);

protected:
	PList<CLightLibrary>		*m_pLightLibraryList;

protected:
	CFile	m_File;

};

