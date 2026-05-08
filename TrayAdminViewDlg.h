#pragma once
#include "TrayImagePB.h"

class ITrayAdminViewDlg
{
public:
	virtual CTrayImagePB& GetTrayImagePB() = 0;
};