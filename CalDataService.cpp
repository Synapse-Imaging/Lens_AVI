//////////////////////////////////////////////////////////////////////
//
//			CalDataService( for XPi) Implementation
//
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include"uScan.h"
#include"CalDataService.h"

CCalDataService::CCalDataService()
{
	mdOriginXp = 0;
	mdOriginYp = 0;
	mdAngle = 0;

	mdFOVXm = 0;
	mdFOVYm = 0;

	mdPixelSizeXm = 0;
	mdPixelSizeYm = 0;
	mdPixelSizem = 0;
	mdPixelArea = 0;

	mdPitchm = 0;

	m_iCamImageWidth = 1000;
	m_iCamImageHeight = 1000;

	m_bUseImageScale = FALSE;
	m_dImageScaleX = 1.0;
	m_dImageScaleY = 1.0;

	m_iVisionCamType = 0;
}

CCalDataService::~CCalDataService()
{
}

void CCalDataService::SetCalData(int iCamImageWidth, int iCamImageHeight, double dAvgPixelSizeMM, BOOL bUseImageScale, double dImageScaleX, double dImageScaleY)
{
	m_iCamImageWidth = iCamImageWidth;
	m_iCamImageHeight = iCamImageHeight;

	mdPixelSizeXm = dAvgPixelSizeMM;	// um
	mdPixelSizeYm = dAvgPixelSizeMM;	// um
	mdPixelSizem = dAvgPixelSizeMM;		// um
	mdPixelArea = mdPixelSizem * mdPixelSizem;	//um2

	m_bUseImageScale = bUseImageScale;
	m_dImageScaleX = dImageScaleX;
	m_dImageScaleY = dImageScaleY;
}

void CCalDataService::SetCameraFov(int iCamImageWidth, int iCamImageHeight)
{
	m_iCamImageWidth = iCamImageWidth;
	m_iCamImageHeight = iCamImageHeight;
}

void CCalDataService::SetVisionCamName(int iVisionCamName)
{
	m_iVisionCamType = iVisionCamName;
}

double CCalDataService::GetNominalPixelSize()
{
	return THEAPP.m_ModelDefineInfo.m_iPixelResolution[m_iVisionCamType];
}

double CCalDataService::GetNominalPixelArea()
{
	double dArea;
	dArea = THEAPP.m_ModelDefineInfo.m_iPixelResolution[m_iVisionCamType] * THEAPP.m_ModelDefineInfo.m_iPixelResolution[m_iVisionCamType];

	return dArea;
}

void CCalDataService::IPtoRP(double dXp, double dYp, double* dpXm, double* dpYm)
{
	if (!dpXm || !dpYm) return;

	*dpXm = dXp * GetNominalPixelSize();
	*dpYm = dYp * GetNominalPixelSize();
}

void CCalDataService::RPtoIP(double dXm, double dYm, double* dpXp, double* dpYp)
{
	if (!dpXp || !dpYp) return;

	*dpXp = dXm / GetNominalPixelSize();
	*dpYp = dYm / GetNominalPixelSize();
}

void CCalDataService::ILtoRL(double dLp, double* dLm)
{
	*dLm = dLp * GetNominalPixelSize();
}

void CCalDataService::RLtoIL(double dLm, double* dLp)
{
	*dLp = dLm / GetNominalPixelSize();
}

void CCalDataService::IPtoRL(double dXp1, double dYp1, double dXp2, double dYp2, double* dpLm)
{
	double dXm1, dYm1, dXm2, dYm2;
	IPtoRP(dXp1, dYp1, &dXm1, &dYm1);
	IPtoRP(dXp2, dYp2, &dXm2, &dYm2);
	*dpLm = sqrt((dXm1 - dXm2)*(dXm1 - dXm2) + (dYm1 - dYm2)*(dYm1 - dYm2));
}

void CCalDataService::RPtoIL(double dXm1, double dYm1, double dXm2, double dYm2, double* dpLp)
{
	double dXp1, dYp1, dXp2, dYp2;
	RPtoIP(dXm1, dYm1, &dXp1, &dYp1);
	RPtoIP(dXm2, dYm2, &dXp2, &dYp2);
	*dpLp = sqrt((dXp1 - dXp2)*(dXp1 - dXp2) + (dYp1 - dYp2)*(dYp1 - dYp2));
}

void CCalDataService::Reset()
{
	mdOriginXp = 0;
	mdOriginYp = 0;
	mdAngle = 0;

	mdFOVXm = 0;
	mdFOVYm = 0;

	mdPixelSizeXm = 0;
	mdPixelSizeYm = 0;
	mdPixelSizem = 0;
	mdPixelArea = 0;

	mdPitchm = 0;

	m_iCamImageWidth = 1000;
	m_iCamImageHeight = 1000;

	m_bUseImageScale = FALSE;
	m_dImageScaleX = 1.0;
	m_dImageScaleY = 1.0;
}

CCalDataService& CCalDataService::operator=(CCalDataService& data)
{
	if (this == &data) return *this;

	mdVersion = data.mdVersion;

	mdFOVXm = data.mdFOVXm;
	mdFOVYm = data.mdFOVYm;

	mdPixelSizeXm = data.mdPixelSizeXm;
	mdPixelSizeYm = data.mdPixelSizeYm;
	mdPixelSizem = data.mdPixelSizem;
	mdPixelArea = data.mdPixelArea;

	mdPitchm = data.mdPitchm;

	mdOriginXp = data.mdOriginXp;
	mdOriginYp = data.mdOriginYp;
	mdAngle = data.mdAngle;

	m_dA = data.m_dA;
	m_dB = data.m_dB;
	m_dC = data.m_dC;
	m_dD = data.m_dD;
	m_dA_inverse = data.m_dA_inverse;
	m_dB_inverse = data.m_dB_inverse;
	m_dC_inverse = data.m_dC_inverse;
	m_dD_inverse = data.m_dD_inverse;

	m_iCamImageWidth = data.m_iCamImageWidth;
	m_iCamImageHeight = data.m_iCamImageHeight;

	return *this;
}

