#pragma once
#include "afxcmn.h"


// CPreferenceDlg 대화 상자입니다.

class CPreferenceDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreferenceDlg)

public:
	CPreferenceDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPreferenceDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PREFERENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


private:
	virtual BOOL OnInitDialog();

	CButtonCS m_bnOK;
	CButtonCS m_bnCancel;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	

public:
	CString m_strEquipNo;

	int m_iRadioPCType;

	BOOL m_bCheckUseVision[VISION_NUMBER_MAX];

	BOOL m_bCheckUseLAS1;
	BOOL m_bCheckUseLAS2;
	BOOL m_bCheckUseLAS3;

	CString m_strSaveFolderPathLAS1;
	CString m_strSaveFolderPathLAS2;
	CString m_strSaveFolderPathLAS3;

	CString m_EditSaveLASStatus;
	BOOL m_bCheckUseLASConnectionCheck;

	CString m_strRawFolderPath;
	CString m_strResultFolderPath;
	CString m_strReviewFolderPath;
	CString m_strAdjFolderPath;
	CString m_strResultLogFolderPath;
	CString m_strEtcFolderPath;
	CString m_strOfflineTestFolderPath;

	BOOL m_bCheckRawImageSaveLocal;
	BOOL m_bCheckResultImageSaveLocal;
	BOOL m_bCheckReviewImageSaveLocal;
	BOOL m_bCheckADJImageSaveLocal;
	BOOL m_bCheckResultLogSaveLocal;

	BOOL m_bCheckRawImageSaveLAS;
	BOOL m_bCheckResultImageSaveLAS;
	BOOL m_bCheckReviewImageSaveLAS;
	BOOL m_bCheckADJImageSaveLAS;
	BOOL m_bCheckResultLogSaveLAS;

	BOOL m_bCheckUseRawImageThread;
	BOOL m_bCheckUseResultImageThread;
	BOOL m_bCheckUseReviewImageThread;
	BOOL m_bCheckUseADJImageThread;
	BOOL m_bCheckUseResultLogThread;

	BOOL m_bCheckUseRawImageTempDrive;

	BOOL m_bCheckSaveRawImage;
	BOOL m_bRadioRawImageType;
	BOOL m_bCheckReduceRawImage;
	double m_dEditRawImageZoomRatio;
	BOOL m_bCheckCombineRawImage;
	BOOL m_bCheckSaveResultGoodImage;
	BOOL m_bCheckUseResultImageThreadOnlyGood;
	BOOL m_bRadioRawImageSaveLogType;
	BOOL m_bCheckAvoidDuplicateLotLocal;
	BOOL m_bCheckAvoidDuplicateLotLas;

	int m_iRadioRawImageThreadType;

	BOOL m_bCheckUseMultiQueue;
	int m_iEditThreadQueueNumber;

	int m_iRadioResultImageDefectDisplayType;
	BOOL m_bCheckUseResultImageDefectSizeLimit;
	int m_iEditResultImageDefectSizeMax;

	BOOL m_bCheckSaveADJ;
	BOOL m_bCheckUseADJConnectionCheck;
	BOOL m_bCheckUseLocalADJ;
	BOOL m_bCheckUseLocalSEG;
	CString m_strEditAdjCurrentIP;
	BOOL m_bCheckUseADJ[MAX_ADJ_CONNECT_NUM];
	CString m_strADJ_IP[MAX_ADJ_CONNECT_NUM];
	int m_nEditAdjPort[MAX_ADJ_CONNECT_NUM];
	BOOL m_bCheckIsSendADJ;
	BOOL m_bCheckUseADJMultiimage;
	BOOL m_bCheckIsApplyADJ;
	BOOL m_bCheckUseADJTimeOut;
	int m_nADJTimeOut;
	int m_nEditADJMultiNetworkNO;
	int m_iEditADJImageSize;
	int m_iEditADJImageReSize;
	int m_iEditAdjMarkingRad;
	int m_iEditADJCombineDefectDistance;
	BOOL m_bCheckSaveROIImage;
	int m_iRadioSaveROIImageType;
	BOOL m_bCheckAISetupUseVAL;
	BOOL m_bCheckAllImageAi;

	BOOL m_bCheckSaveMDJ;
	int m_iEditMaxReviewImageNumber;
	int m_iEditReviewCombineDefectDistance;
	BOOL m_bCheckUseSaveFaiReviewImage;

	int m_iRadioReviewImageDisplayType;

	int m_iEditVision1LightIP1;
	int m_iEditVision1LightIP2;
	int m_iEditVision1LightIP3;
	int m_iEditVision1LightIP4;
	int m_iEditVision1LightPort;

	int m_iEditVision2LightIP1;
	int m_iEditVision2LightIP2;
	int m_iEditVision2LightIP3;
	int m_iEditVision2LightIP4;
	int m_iEditVision2LightPort;

	int m_iEditVision3LightIP1;
	int m_iEditVision3LightIP2;
	int m_iEditVision3LightIP3;
	int m_iEditVision3LightIP4;
	int m_iEditVision3LightPort;

	int m_iEditVision4LightIP1;
	int m_iEditVision4LightIP2;
	int m_iEditVision4LightIP3;
	int m_iEditVision4LightIP4;
	int m_iEditVision4LightPort;

	int m_iEditGrabErrRetryNo;
	int m_iEditGrabDelayTime;
	int m_iEditTriggerSleepTime;
	int m_iEditGrabDoneWaitTime;
	int m_iEditTeachingLiveGrabPeriod;
	int m_iEditLightErrValue;
	int m_iRadioTeachingCameraLightType;
	BOOL m_bCheckUseGrabHold;
	int m_iEditFocusMoveNoRetry;
	int m_iEditMoveCompleteWaitTime;
	BOOL m_bCheckUse4dSaveRawImage;
	BOOL m_bCheckUseAutoFocus;
	BOOL m_bCheckUseFastGrab;
	BOOL m_bCheckUseBatchInspection;
	BOOL m_bCheckSendGrabFailError;
	int m_iEditSendGrabFailErrorCount;

	int m_iDefectDispDist;
	int m_iEditMatchingSearchMargin;
	int m_iRadioBarcodeNoReadImageType;
	BOOL m_bCheckUseSaveDefectShapeFeature;
	int m_iEditMaxSameDefectShapeFeature;
	BOOL m_bCheckUseSaveGVEdgeMornitoringLog;
	BOOL m_bCheckUseGpuAffineTrans;
	BOOL m_bCheckUseDeleteCrossBar;
	int m_iEditSaveRecentlyCompleteInfoNumber;

	BOOL m_bCheckUseContDefectAlarm;
	int m_iEditContDefectPosTolerance;
	int m_iEditContDefectCount;

	int m_iEditSam2IP_1;
	int m_iEditSam2IP_2;
	int m_iEditSam2IP_3;
	int m_iEditSam2IP_4;
	int m_iEditSam2Port;

	BOOL m_bCheckUseVisionCopy;

	int m_iRadioVmTrain;
	BOOL m_bCheckVmTrainAll;

	BOOL m_bCheckUseVmThresOpt;
	int m_iEditVmSplitThres;
	double m_dEditVmKSigma;

	afx_msg void OnBnClickedButtonSelectInspectionModelType();

	afx_msg void OnBnClickedButtonFindfolderLas1();
	afx_msg void OnBnClickedButtonFindfolderLas2();
	afx_msg void OnBnClickedButtonFindfolderLas3();

	afx_msg void OnBnClickedButtonFindfolderRaw();
	afx_msg void OnBnClickedButtonFindfolderResult();
	afx_msg void OnBnClickedButtonFindfolderReview();
	afx_msg void OnBnClickedButtonFindfolderAdj();
	afx_msg void OnBnClickedButtonFindfolderResultlog();
	afx_msg void OnBnClickedButtonFindfolderEtc();
	afx_msg void OnBnClickedButtonFindfolderDebug();

	afx_msg void OnBnClickedCheckRawImageSaveLocal();
	afx_msg void OnBnClickedCheckResultImageSaveLocal();
	afx_msg void OnBnClickedCheckReviewImageSaveLocal();
	afx_msg void OnBnClickedCheckAdjImageSaveLocal();
	afx_msg void OnBnClickedCheckResultLogSaveLocal();

	afx_msg void OnBnClickedCheckRawImageSaveLas();
	afx_msg void OnBnClickedCheckResultImageSaveLas();
	afx_msg void OnBnClickedCheckReviewImageSaveLas();
	afx_msg void OnBnClickedCheckAdjImageSaveLas();
	afx_msg void OnBnClickedCheckResultLogSaveLas();

	afx_msg void OnBnClickedCheckUseRawImageThread();
	afx_msg void OnBnClickedCheckUseResultImageThread();

	afx_msg void OnBnClickedCheckUseResultImageThreadOnlyGood();

	afx_msg void OnBnClickedButtonSetDefectPriority();
	afx_msg void OnBnClickedButtonSetResultImage();
	afx_msg void OnBnClickedButtonAdjconnect();
	afx_msg void OnBnClickedButtonAdjconnect2();
	afx_msg void OnBnClickedButtonSetDefectAdj();

	void ChangeLanguage();
	afx_msg void OnBnClickedButtonLanguageChange();
	CString strLog;
	CString strDialog;
	CString strMessageBox;

	CString m_strEditEquipModelType;

	afx_msg void OnBnClickedRadioVmTrain1();
	afx_msg void OnBnClickedRadioVmTrain2();
	afx_msg void OnBnClickedRadioVmTrain3();
};
