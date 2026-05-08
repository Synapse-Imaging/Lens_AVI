#pragma once

#include "PBC.h"
#include "FileBase.h"
#include "InspectLibrary.h"

class CInspectLibraryData : public CFileBase
{
public:
	CInspectLibraryData(void);
	virtual ~CInspectLibraryData(void);

public:
	void ClearAll();
	void Load();
	void Save();
	BOOL LibraryExist(CString sLibName);
	BOOL DeleteSelectedLibrary(CString sLibName);
	CInspectLibrary* GetSelectedLibrary(CString sLibName);

	int GetInspectLibraryCount();
	void AddInspectLibrary(CInspectLibrary *pInspectLibrary);
	void DeleteInspectLibrary(int iIndex);
	void DeleteInspectLibraryPtr(CInspectLibrary *pInspectLibrary);
	void ClearAllInspectLibrary();
	CInspectLibrary *GetInspectLibrary(int iIndex);

protected:
	PList<CInspectLibrary>		*m_pInspectLibraryList;

protected:
	CFile	m_File;

};

