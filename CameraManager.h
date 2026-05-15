#pragma once

#include "MvCamera.h"
#pragma comment(lib,"MvCameraControl.lib")

//#ifdef USE_DIFFUSED
#include "S_Universal_AVI.h"
//#endif

class CCameraManager
{
public:
	CCameraManager(void);
	~CCameraManager(void);

	MIL_ID MilDigitizer;
	MIL_ID MilMonoImageBuf[GRAB_CIRCULAR_MAX][MAX_GRAB_BUFFER];
	MIL_ID MilColorImageBuf[GRAB_CIRCULAR_MAX][MAX_GRAB_BUFFER];
	MIL_ID MilImageBuf_SWTrg;

	MIL_ID m_MilWBCoeff;
	CString m_sBayerType;

	HObject m_hoCallBackImage[GRAB_CIRCULAR_MAX][MAX_GRAB_BUFFER][3];

	BOOL m_bGrabDone;
	BOOL m_bReGrab;

	int iDFMGrabCountIndex;

	BOOL bDFMNormalGrabProcStopFlag;
	BOOL bDFMGrabProcStopFlag;

	int iSeqGrabCountIndex;
	BOOL bCameraGrabProcStopFlag;

	int m_iDFMStartBufferIndex;

	//////////////////////////////////////////////////////////////////////////
	BOOL   m_bDFMGrabComPortOpened;
	HTuple m_HDFMGrabSerialHandle;
	HANDLE m_hEventDFMGrabDone;
	HANDLE m_hEventCameraGrabDone;

	int	m_nProcessCount;
	int	m_nDFMTargetCount;
	int	m_nSEQTargetCount;

	////////////////////////////////////////////////////////////
	// Proc Member Variables
	int  m_iGpuProcMode;
	int m_iMzNo;
	int m_iPcVisionNo;

	// Specular Mode
	HObject m_HDFMConvColorImage[DFM_USED_CONV_IMAGE_NUMBER];
	int m_iVisionCamType;
	BOOL m_bUseGPU;
	BOOL m_bDualProc;
	BOOL m_bLeft;

	// Diffused Mode
	HObject m_HDiffusedConvColorImage;

	HObject m_HGrabColorImage[MAX_IMAGE_TAB];
	HObject m_HAffineTransImage[MAX_IMAGE_TAB];
	BOOL m_bGlobalAlignTransImageFlag[MAX_IMAGE_TAB];

	int m_iNoInspectImage;
	HTuple *m_pHMatchingHomMat;

	BOOL m_bDFMProcRet;
	////////////////////////////////////////////////////////////

	void SetGpuProcMode(int iMode) { m_iGpuProcMode = iMode; }

	// Raw Image Display for Teaching Mode
	HObject m_HDFMRawImage[SPV_RAW_IMAGE_NUMBER];

	void ResetDFMVariable();
	int DFMStartGrab(int iGrabStartBufferIdx, BOOL bSingleDualGrabMode = TRUE);
	BOOL DFMSetLightIntensity(int iLightValue, int iPortOrder);

	int DFMProc(int iPcVisionNo, int iDFMStartGrabBufferIndex, int iVisionCamType);
	int DiffusedProc(int iPcVisionNo, int iDFMStartGrabBufferIndex, int iVisionCamType);

	void SetSpecularProcParameter(int iMzNo, HObject *pHSpecularGrayRawImage, HObject HSpecularConvNormalImage, int iPcVisionNo, int iVisionCamType, BOOL bUseGPU, BOOL bDualProc, BOOL bLeft);
	void SetDiffusedProcParameter(int iMzNo, HObject *pHDiffusedGrayRawImage, int iPcVisionNo, int iVisionCamType, BOOL bUseGPU);
	BOOL GetSpecularProcParameter(HObject *pHConvColorImage, double dZoomFactor, int iPcVisionNo);
	BOOL GetDiffusedProcParameter(HObject *pHConvColorImage, double dZoomFactor, int iPcVisionNo);
	BOOL GetSpecularCropProcParameter(HObject *pHConvColorImage, long LTPointX, long LTPointY, double dZoomFactor, int iPcVisionNo);
	BOOL GetDiffusedCropProcParameter(HObject *pHConvColorImage, long LTPointX, long LTPointY, double dZoomFactor, int iPcVisionNo);

	void SetAffineParameter(HObject *pHGrabColorImage, int iMzNo, int iNoInspectImage, int iPcVisionNo, HTuple *pHMatchingHomMat);
	BOOL GetAffineParameter(HObject *pHAffineTransImage);

	BOOL DFMGlobalProcInspection();

	BOOL DFMCalNormal(HObject *pHGrabColorImage, HObject *pHGrayImage, HObject *pHNormalColorImage, BOOL bColor, int iModuleNo=1, int iGrabOrder=1);
	BOOL DiffusedCalNormal(HObject *pHGrabColorImage, HObject *pHGrayImage, BOOL bColor, int iModuleNo=1, int iGrabOrder=1);

	BOOL ResetDFMConvertVariable();

	BOOL DFM_ReadConvertImage_TeachMode(int iWhichDFMImage, BOOL bImageSave);
	BOOL DFM_ReadRawImage_TeachMode(int iTeachImageIndex, BOOL bImageSave);
	BOOL DFM_ReadPageImage_TeachMode(int iTeachImageIndex, BOOL bImageSave);

	BOOL Diffused_ReadConvertImage_TeachMode(int iWhichDiffusedImage, BOOL bImageSave);

	static MIL_INT MFTYPE DFMGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr);
	static MIL_INT MFTYPE DFMNormalGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr);
	static MIL_INT MFTYPE CameraGrabProcFunc(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr);
	static MIL_INT MFTYPE CameraGrabProcFunc_OneGrabFunction(MIL_INT HookType, MIL_ID HookId, void* HookDataPtr);

	int m_iDFMNormalGrabBufferIdx;
	int DFMStartNormalGrab(int iGrabBufferIdx, int iImageIndex, BOOL bUseSpecularLight=FALSE, BOOL bSingleDualGrabMode = TRUE);

	void OpenPortDFM();

	//////////////////////////////////////////////////////////////////////////
	
	bool InitGrabInterface(MIL_ID MilSystem);
	bool InitGrabInterface_Mono(MIL_ID MilSystem);
	void UnInitGrabInterface();
	void UpdateWhiteBalance(int iVisionNo);
	
	void GrabErrorPostProcess();

	void MdigProcessGrabErrorPostProcess(int iGrabStartBufferIndex, int iNoGrabImage=1);

	int m_iSEQStartBufferIndex;
	BOOL CameraStartGrab(int iImageGrabIndex, int iSeqAddressIndex = LIGHTCTRL_TEACHING_GRAB_ADDR_INDEX, int iNoGrabImage = 1, BOOL bAutoSetting = FALSE);
	BOOL AutoRunCameraStartGrab(int iGrabStartBufferIdx, int iNoGrabImage);
	BOOL CameraStartGrab_NoSeq(int iGrabStartBufferIdx, int iLightPageIdx);

	BOOL AutoRunCameraGrab_OneGrabFunction_Start(int iGrabStartBufferIdx, int iNoGrabImage);
	BOOL AutoRunCameraGrab_OneGrabFunction_Stop(int iGrabStartBufferIdx, int iNoGrabImage);
	BOOL AutoRunCameraGrab_OneGrabFunction(int iGrabStartBufferIdx, int iNoGrabImage);
	BOOL AutoRunCameraGrab_SingleLens(int iGrabStartBufferIdx, int iNoGrabImage, int iVisionGrabPeriodMsec, int iDualModelData, int iPcVisionNo);

	BOOL m_bGrabIndexMismatchDetected;
	int m_iGrabIndexMismatchOffset;

	void SetVisionCamName(int iVisionCamName);
	int  GetVisionCamName() { return m_iVisionCamName; }

	BOOL GetTeachImageSave() { return m_bTeachImageSave; }
	void SetTeachImageSave(BOOL bSave) { m_bTeachImageSave = bSave; }

	int GetGrabCircularIndex() { return m_iGrabCircularIdx; }
	void SetGrabCircularIndex(int iIndex) { m_iGrabCircularIdx = iIndex; }
	void IncGrabCircularIndex();

	BOOL m_bGrabCircularBufferCopyDone[GRAB_CIRCULAR_MAX];
	BOOL CheckCircularBufferCopyStatus();

	void SetCameraFovCropInfo(BOOL bUse, int iLTX, int iLTY, int iSizeX, int iSizeY);
	BOOL GetCropFovUse() { return m_bUseCropFov; }
	int GetCropFovLTX() { return m_iCropFovLeftTopX; }
	int GetCropFovLTY() { return m_iCropFovLeftTopY; }
	int GetCropFovSizeX() { return m_iCropFovSizeX; }
	int GetCropFovSizeY() { return m_iCropFovSizeY; }

	void ResetSpecularTriggerPeriod(int iMzNo);
	void TurnOff_Specular();

	void SetCamImageSizeX(int iSize) { m_iCamImageSizeX = iSize; }
	void SetCamImageSizeY(int iSize) { m_iCamImageSizeY = iSize; }

	int GetCamImageSizeX() { return m_iCamImageSizeX; }
	int GetCamImageSizeY() { return m_iCamImageSizeY; }

public:
	CCriticalSection	m_csProc;

	CEvent* GetImageReadyEvent() { return &m_eventImageReady; }

	CEvent	m_eventImageReady;	//Auto Reset
	CEvent	m_eventDFMProcDone;
	CWinThread*	m_pConvertThread;

	void StopDFMProcThread();

protected:
	int m_iVisionCamName;

	BOOL m_bTeachImageSave;

	int	m_iGrabCircularIdx;
	int m_iMaxCircularGrab;

	int m_iCamImageSizeX;
	int m_iCamImageSizeY;

	BOOL m_bUseCropFov;
	int m_iCropFovLeftTopX;
	int m_iCropFovLeftTopY;
	int m_iCropFovSizeX;
	int m_iCropFovSizeY;

public:
	//////////////////////////////////////////////////////////////////////////
	// Specular iconic variables

	HObject ho_Hor1, ho_Hor2, ho_Hor3, ho_Hor4;
	HObject ho_Vert1, ho_Vert2, ho_Vert3, ho_Vert4;

	UINT8 *pIm1x, *pIm2x, *pIm3x, *pIm4x, *pImFCx;
	UINT8 *pIm1y, *pIm2y, *pIm3y, *pIm4y, *pImFCy;
	float *pImPhx, *pImPhy;
	UINT8 *pImResi_ssss, *pImResi_1, *pImResi_2, *pImSpec;
	HTuple hImResi_ssss, hImResi_1, hImResi_2;

	HObject ho_ImagePhX, ho_ImageFCX, ho_ImagePhY, ho_ImageFCY;
	HObject ho_DerivGaussX, ho_DerivGaussY;

	HObject m_HProcNormalImage, m_HProcSpecularImage, m_HProcShapeImage, m_HProcShapeImage1, m_HProcShapeImage2;
	HObject HMultiChImage;

	HTuple  hv_psx, hv_psy, hv_mult, hv_Message;
	HTuple  hv_add, hv_zf, hv_sec;
	HTuple  hv_Information, hv_BoardList, hv_MyDevice, hv_MyCameraType;
	HTuple  hv_MyPort, hv_AcqHandle, hv_Revision, hv_tgrb, hv_tgre;
	HTuple  hv_DeviceIdentifier, hv_DeviceName0, hv_DeviceName1;
	HTuple  hv_w, hv_h;
	HTuple  hv_MP, hv_Pi, hv_Pi2;
	HTuple  hv_Fi, hv_FCmx, hv_PH;
	HTuple  hv_Type, hv_Width, hv_Height;
	HTuple hIm1x, hIm2x, hIm3x, hIm4x, hImPhx, hImFCx;
	HTuple hIm1y, hIm2y, hIm3y, hIm4y, hImPhy, hImFCy;

	HTuple HInputImageSizeX, HInputImageSizeY;
	Hlong iInputImageSizeX, iInputImageSizeY;
	
	HObject ho_ImagePh, ho_ImageFC, ho_ImagePh1, ho_ImageFC1;
	UINT8 *pImPhxU8, *pImPhyU8;

	//////////////////////////////////////////////////////////////////////////
	// Diffused Vision

	HObject ho_Diffused1, ho_Diffused2, ho_Diffused3, ho_Diffused4;
	HObject ho_DiffusedResulti, ho_DiffusedResulti2;
	HObject ho_ImageC2D;
	UINT8 *pImResi, *pImResi2, *pImC2D;
	HTuple hImResi, hImResi2, hImC2D;

	HTuple hIm1xD, hIm2xD, hIm3xD, hIm4xD;
	UINT8 *pIm1xD, *pIm2xD, *pIm3xD, *pIm4xD;

	HTuple  hv_TypeD, hv_WidthD, hv_HeightD;
	HTuple HInputImageSizeXD, HInputImageSizeYD;
	Hlong iInputImageSizeXD, iInputImageSizeYD;
	HTuple  hv_wD, hv_hD;

	S_Universal_AVI* s_universal_avi;
	void Init4DVision();
	void Get_System_Dir(string& key_file_name);
	void SetInitDiffusedVision() { bCreatePhSObject = TRUE;  }

	void CalcGaussDeriv1D(HTuple hv_sigma, HTuple *hv_Gauss, HTuple *hv_GaussDeriv, HTuple *hv_Gauss2Deriv, HTuple *hv_ksz);

	UINT OfflineSpecularPhaseConv();
	UINT OfflineSpecularPhaseConvForSubFolder();
	UINT OfflineDiffusedVisionConv();
	UINT OfflineDiffusedVisionConvForSubFolder();

	BOOL m_bAlignGrabFail[ALIGN_CAM_GRAB_NUMBER];
	HObject m_MvhoCallBackImage[ALIGN_CAM_GRAB_NUMBER];

	CMvCamera* m_pcMyCamera;
	MV_CC_DEVICE_INFO_LIST  m_stDevList;

	BYTE* m_MvGrabBufAddr[ALIGN_CAM_GRAB_NUMBER];
	BOOL m_bMvGrabDone;
	BOOL m_bMvInit;

	bool InitMvEthernetCam(CString sCameraSerial);
	BOOL GrabEthernetCamSWTrg(int iGrabBufIdx);
	int GetMvCamGrabIndex() { return m_iMvGrabIndex; }
	CString strLog;

protected:
	BOOL bCreatePhSObject;
	int m_iMvGrabIndex;
};

