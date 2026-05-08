//////////////////////////////////////////////////////////////////////
//
//			CalDataService( for XPi) Header
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_)
#define AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TsaiCalibrator.h"


#define PCP_CALDATA_FILEHEADER			"Synapseimaging Calibration Data File"
#define PCP_CALWORKSPACE_FILEHEADER		"Synapseimaging Calibration Workspace File"

#define PCP_FILEVERSION_010000			1.0
#define PCP_FILEVERSION_LEGACY			PCP_FILEVERSION_010000
#define PCP_FILEVERSION_RECENT			PCP_FILEVERSION_010000

class CCalDataService  
{
public:
	CCalDataService();
	virtual ~CCalDataService();

	void SetCalData(int iCamImageWidth, int iCamImageHeight, double dAvgPixelSizeMM, BOOL bUseImageScale=FALSE, double dImageScaleX=1.0, double dImageScaleY=1.0);
	void SetCameraFov(int iCamImageWidth, int iCamImageHeight);

	//transform position information
	void IPtoRP(double dXp, double dYp, double* dpXm, double* dpYm);
	void RPtoIP(double dXm, double dYm, double* dpXp, double* dpYp);
	
	void ILtoRL(double dLp, double* dLm);
	void RLtoIL(double dLm, double* dLp);
	
	//position<->distance transformation
	void IPtoRL(double dXp1, double dYp1, double dXp2, double dYp2, double* dpLm);	//image position to real-world distance
	void RPtoIL(double dXm1, double dYm1, double dXm2, double dYm2, double* dpLp);	//real-world position to pixel distance
	
	void Reset();
	
	double mdVersion;

	double mdFOVXm;
	double mdFOVYm;


	double mdPitchm;

	double mdOriginXp;
	double mdOriginYp;
	double mdAngle;

	double m_dA;
	double m_dB;
	double m_dC;
	double m_dD;
	double m_dA_inverse;
	double m_dB_inverse;
	double m_dC_inverse;
	double m_dD_inverse;

	CCalDataService& operator=(CCalDataService& data);

	double GetPixelSize() { return mdPixelSizem; }
	double GetPixelSizeX() { return mdPixelSizeXm; }
	double GetPixelSizeY() { return mdPixelSizeYm; }
	double GetPixelArea() { return mdPixelArea; }
	double GetNominalPixelSize();
	double GetNominalPixelArea();

	BOOL GetUseImageScale() { return m_bUseImageScale; }
	double GetImageScaleX() { return m_dImageScaleX; }
	double GetImageScaleY() { return m_dImageScaleY; }

	void SetVisionCamName(int iVisionCamType);
	
protected:
	double mdPixelSizeXm;
	double mdPixelSizeYm;
	double mdPixelSizem;
	double mdPixelArea;
	int m_iCamImageWidth;
	int m_iCamImageHeight;

	BOOL m_bUseImageScale;
	double m_dImageScaleX;
	double m_dImageScaleY;

	int m_iVisionCamType;

};

#endif // !defined(AFX_CALDATASERVICE_H__D03B2E75_70E7_4EFD_88B7_CD5ACEEF7F67__INCLUDED_)

