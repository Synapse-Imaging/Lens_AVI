#pragma once

#include "Algorithm.h"

class CAutoCalService
{
public:
	CAutoCalService(void);
	~CAutoCalService(void);

	static CAutoCalService *m_pInstance;
public:
	static CAutoCalService *GetInstance();
	void DeleteInstance();

	//WCS
	////////////////////////////// Auto Cal 관련 데이터  ////////////////////////////////////

public:
	int m_iAutoCalPCType;

	bool m_bAutoCal_Done[MAX_IMAGE_TAB];

	int m_iTeachCH_LV[MAX_IMAGE_TAB][LIGHT_CH_CNT];
	int m_iTeachCH_GV[MAX_IMAGE_TAB][LIGHT_CH_CNT];
	int m_iInspectCH_LV[MAX_IMAGE_TAB][LIGHT_CH_CNT];
	int m_iInspectCH_GV[MAX_IMAGE_TAB][LIGHT_CH_CNT];
	bool m_bJudgeCH[MAX_IMAGE_TAB][LIGHT_CH_CNT];

	int m_iTeachTOTAL_GV[MAX_IMAGE_TAB];
	int m_iInspectTOTAL_GV[MAX_IMAGE_TAB];
	bool m_bJudgeTOTAL[MAX_IMAGE_TAB];

	double m_dTeachZPos[MAX_IMAGE_TAB];
	double m_dTeachEdgeValue[MAX_IMAGE_TAB];
	double m_dInspectZPos[MAX_IMAGE_TAB];
	double m_dInspectEdgeValue[MAX_IMAGE_TAB];
	bool m_bJudge[MAX_IMAGE_TAB];

	int m_iLightAutoCalProgressPercent;
	int m_iFocusAutoCalProgressPercent;

	void InitAutoCalResult();
	BOOL GetMatchingAlignInfo(HObject *pHGrabColorImage, HTuple *pHMatchingHomMat);
	BOOL ApplyMatchingAlignInfo(int iAffineTransImageIndex, HObject *pHGrabColorImage, HTuple *pHMatchingHomMat);

	void AutoCalMasterGvStart(int iPCType);
	void AutoCalLightStart(int iPCType);
	void AutoCalFocusStart(int iPCType);

	CString strLog;
};