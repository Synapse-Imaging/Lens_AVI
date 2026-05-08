#pragma once

#include "ugctrl.h"

#define LIST_TYPE_INSPECT_LIBRARY	0
#define LIST_TYPE_LIGHT_LIBRARY		1

class CLibraryGridCtrl :
	public CUGCtrl
{
public:
	CLibraryGridCtrl(void);
	virtual ~CLibraryGridCtrl(void);

	void SetListType(int iType) { m_iListType = iType; }
	long GetCurListNumber();

	virtual void OnSetup();
	void SetHwnd(HWND hWnd) { m_HWnd = hWnd; }
	HWND m_HWnd;

	void MoveCurrent(int iCurDefectIdx);
	void MoveFirst();
	void MoveLast();
	void MovePrev();
	void MoveNext();

	virtual BOOL IsItemChecked(long lIndex);
	virtual BOOL SetItemCheck(long lIndex, BOOL bChecked);
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int OnEditFinish(int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, BOOL processed);
	virtual void OnKeyDown(UINT *vcKey, BOOL processed);

protected:
	CFont m_NameFont;
	int m_iListType;
};

