#pragma once

#include "AlgorithmDontcareDlg.h"
#include "AlgorithmSurfaceDlg.h"
#include "AlgorithmSurfaceDualDlg.h"
#include "AlgorithmDefectConditionDlg.h"
#include "AlgorithmImageProcessDlg.h"
#include "AlgorithmFittingDlg.h"
#include "AlgorithmOCRDlg.h"
#include "AlgorithmBarcodeDlg.h"
#include "AlgorithmConnectorPinDlg.h"
#include "AlgorithmDentDlg.h"
#include "AlgorithmImageCompareDlg.h"
#include "AlgorithmPartCheckDlg.h"
#include "AlgorithmROIAlignDlg.h"
#include "AlgorithmConcenterMeasureDlg.h"
#include "AlgorithmShapeDlg.h"
#include "AlgorithmEdgeMeasureDlg.h"
#include "AlgorithmTiltDlg.h"
#include "AlgorithmEpoxyVoidHoleDlg.h"
#include "AlgorithmPrintQualityDlg.h"
#include "AlgorithmROIAnisoAlignDlg.h"
#include "AlgorithmStiffenerEpoxyDlg.h"
#include "AlgorithmDistanceMeasureDlg.h"
#include "AlgorithmGapMeasureDlg.h"
#include "AlgorithmComponentDlg.h"
#include "AlgorithmNeckEpoxyMeasureDlg.h"
#include "AlgorithmNeckEpoxyCrackDlg.h"
#include "AlgorithmCornerMeasureDlg.h"
#include "AlgorithmHeightMeasureDlg.h"
#include "AlgorithmBoundaryDlg.h"

/**
 * @class CTeachingAlgorithmTabDlg
 * @brief 알고리즘 교육 모드에서 다양한 검사 알고리즘의 파라미터를 설정하고 테스트하는 탭 기반 대화 상자
 *
 * @details 이 클래스는 다양한 검사 알고리즘(이미지 처리, OCR, 바코드, 치수 측정 등)에 대한
 * 파라미터 설정 UI를 제공하며, 싱글톤 패턴으로 구현되어 있습니다.
 * AI 검사 초기화 및 검사 테스트 기능을 지원합니다.
 */
class CTeachingAlgorithmTabDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachingAlgorithmTabDlg)

public:
	/**
	 * @brief 생성자
	 * @param pParent 부모 윈도우 포인터 (기본값: NULL)
	 */
	CTeachingAlgorithmTabDlg(CWnd* pParent = NULL);

	/**
	 * @brief 소멸자
	 */
	virtual ~CTeachingAlgorithmTabDlg();

	/**
	 * @brief 싱글톤 인스턴스를 반환합니다
	 * @param bShowFlag 대화 상자를 표시할지 여부 (기본값: FALSE)
	 * @return CTeachingAlgorithmTabDlg* 싱글톤 인스턴스 포인터
	 */
	static	CTeachingAlgorithmTabDlg* GetInstance(BOOL bShowFlag = FALSE);

	/**
	 * @brief 싱글톤 인스턴스를 삭제합니다
	 */
	void	DeleteInstance();

	/**
	 * @brief 대화 상자를 표시합니다
	 */
	void	Show();

	/**
	 * @brief 대화 상자를 숨깁니다
	 */
	void	Hide();

	/**
	 * @brief 대화 상자 표시 상태를 반환합니다
	 * @return BOOL 표시 상태 (TRUE: 표시 중, FALSE: 숨겨짐)
	 */
	BOOL	GetShowStatus() { return m_bShowDlg; }

	/**
	 * @brief 선택된 검사 유형과 ROI를 설정합니다
	 * @param iImageType 이미지 유형 (검사 종류)
	 * @param pSelectedROI 선택된 ROI(Region of Interest) 포인터
	 * @param iTabIndex 활성화할 탭 인덱스
	 * @see SetParam()
	 */
	void	SetSelectedInspection(int iImageType, GTRegion* pSelectedROI, int iTabIndex);

	/**
	 * @brief 현재 탭의 파라미터를 설정합니다
	 * @see GetParam()
	 */
	void	SetParam();

	/**
	 * @brief 피크(Peak) 값을 반환합니다
	 * @return int 피크 값
	 */
	int GetPeak();

	/**
	 * @brief 도형 이미지 확인 상태를 반환합니다
	 * @return BOOL 확인 상태 (TRUE: 체크됨, FALSE: 체크 안 됨)
	 */
	BOOL GetCheckedShapeImage();

	/**
	 * @brief 현재 활성 탭의 알고리즘 파라미터를 반환합니다
	 * @return CAlgorithmParam 현재 탭의 알고리즘 파라미터
	 */
	CAlgorithmParam GetCurTabAlgorithmParam();

	/** @brief 알고리즘 탭 컨트롤 */
	CTabCtrl m_AlgorithmTabCtrl;

	/** @brief 이미지 처리 알고리즘 대화 상자 포인터 */
	CAlgorithmImageProcessDlg* m_pAlgorithmImageProcessDlg;

	/** @brief Dontcare 알고리즘 대화 상자 포인터 */
	CAlgorithmDontcareDlg* m_pAlgorithmDontcareDlg;

	/** @brief Surface 알고리즘 대화 상자 포인터 */
	CAlgorithmSurfaceDlg* m_pAlgorithmSurfaceDlg;

	/** @brief 결함 조건 알고리즘 대화 상자 포인터 */
	CAlgorithmDefectConditionDlg* m_pAlgorithmDefectConditionDlg;

	/** @brief Fitting 알고리즘 대화 상자 포인터 */
	CAlgorithmFittingDlg* m_pAlgorithmFittingDlg;

	/** @brief OCR 알고리즘 대화 상자 포인터 */
	CAlgorithmOCRDlg* m_pAlgorithmOCRDlg;

	/** @brief 바코드 알고리즘 대화 상자 포인터 */
	CAlgorithmBarcodeDlg* m_pAlgorithmBarcodeDlg;

	/** @brief 커넥터 핀 알고리즘 대화 상자 포인터 */
	CAlgorithmConnectorPinDlg* m_pAlgorithmConnectorPinDlg;

	/** @brief 덴트 알고리즘 대화 상자 포인터 */
	CAlgorithmDentDlg* m_pAlgorithmDentDlg;

	/** @brief 이미지 비교 알고리즘 대화 상자 포인터 */
	CAlgorithmImageCompareDlg* m_pAlgorithmImageCompareDlg;

	/** @brief 부품 확인 알고리즘 대화 상자 포인터 */
	CAlgorithmPartCheckDlg* m_pAlgorithmPartCheckDlg;

	/** @brief ROI 정렬 알고리즘 대화 상자 포인터 */
	CAlgorithmROIAlignDlg* m_pAlgorithmROIAlignDlg;

	/** @brief 동심원 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmConcenterMeasureDlg* m_pAlgorithmConcenterMeasureDlg;

	/** @brief 도형 알고리즘 대화 상자 포인터 */
	CAlgorithmShapeDlg* m_pAlgorithmShapeDlg;

	/** @brief 엣지 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmEdgeMeasureDlg* m_pAlgorithmEdgeMeasureDlg;

	/** @brief Surface Dual 알고리즘 대화 상자 포인터 */
	CAlgorithmSurfaceDualDlg* m_pAlgorithmSurfaceDualDlg;

	/** @brief Tilt 알고리즘 대화 상자 포인터 */
	CAlgorithmTiltDlg* m_pAlgorithmHomerTiltDlg;

	/** @brief Epoxy Void Hole 알고리즘 대화 상자 포인터 */
	CAlgorithmEpoxyVoidHoleDlg* m_pAlgorithmEpoxyVoidHoleDlg;

	/** @brief 인쇄 품질 알고리즘 대화 상자 포인터 */
	CAlgorithmPrintQualityDlg* m_pAlgorithmPrintQualityDlg;

	/** @brief ROI 비등방 정렬 알고리즘 대화 상자 포인터 */
	CAlgorithmROIAnisoAlignDlg* m_pAlgorithmROIAnisoAlignDlg;

	/** @brief Stiffener Epoxy 알고리즘 대화 상자 포인터 */
	CAlgorithmStiffenerEpoxyDlg* m_pAlgorithmStiffenerEpoxyDlg;

	/** @brief 거리 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmDistanceMeasureDlg* m_pAlgorithmDistanceMeasureDlg;

	/** @brief 간격 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmGapMeasureDlg* m_pAlgorithmGapMeasureDlg;

	/** @brief 부품 알고리즘 대화 상자 포인터 */
	CAlgorithmComponentDlg* m_pAlgorithmComponentDlg;

	/** @brief 넥 Epoxy 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmNeckEpoxyMeasureDlg* m_pAlgorithmNeckEpoxyMeasureDlg;

	/** @brief 넥 Epoxy 크랙 알고리즘 대화 상자 포인터 */
	CAlgorithmNeckEpoxyCrackDlg* m_pAlgorithmNeckEpoxyCrackDlg;

	/** @brief 코너 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmCornerMeasureDlg* m_pAlgorithmCornerMeasureDlg;

	/** @brief 높이 측정 알고리즘 대화 상자 포인터 */
	CAlgorithmHeightMeasureDlg* m_pAlgorithmHeightMeasureDlg;

	/** @brief 경계 알고리즘 대화 상자 포인터 */
	CAlgorithmBoundaryDlg* m_pAlgorithmBoundaryDlg;

	/** @brief 대화 상자 데이터 ID */
	enum { IDD = IDD_TEACH_ALGORITHM_TAB_DLG };

	/**
	* @brief 선택된 알고리즘 탭을 업데이트합니다
	* @param iSelectedTab 선택할 탭 인덱스
	*/
	void UpdateAlgorithmTab(int iSelectedTab);

private:
	/**
	* @brief 윈도우 위치를 설정합니다
	* @param left 왼쪽 좌표
	* @param top 상단 좌표
	* @param right 오른쪽 좌표
	* @param bottom 하단 좌표
	*/
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	/** @brief 대화 상자의 화면 영역 */
	CRect	m_ScreenRect;

	/** @brief 싱글톤 인스턴스 포인터 */
	static CTeachingAlgorithmTabDlg* m_pInstance;

protected:
	/**
	* @brief MFC 데이터 교환 함수 (DDX/DDV 지원)
	* @param pDX 데이터 교환 객체 포인터
	*/
	virtual void DoDataExchange(CDataExchange* pDX);

	/**
	* @brief AI 설정 프로세스 종료 메시지 핸들러
	* @param wParam 프로세스 종료 코드
	* @param lParam 추가 파라미터
	* @return LRESULT 메시지 처리 결과
	*/
	afx_msg LRESULT OnAiSetupExited(WPARAM wParam, LPARAM lParam);

	/** @brief MFC 메시지 맵 선언 */
	DECLARE_MESSAGE_MAP()
public:
	/**
	* @brief 알고리즘 탭 선택 변경 이벤트 핸들러
	* @param pNMHDR 알림 메시지 헤더
	* @param pResult 메시지 처리 결과를 저장할 포인터
	*/
	afx_msg void OnSelchangeAlgorithmTab(NMHDR* pNMHDR, LRESULT* pResult);

	/**
	* @brief 윈도우 메시지를 처리 전에 가로채는 함수
	* @param pMsg 윈도우 메시지
	* @return BOOL 메시지 처리 여부 (TRUE: 처리됨, FALSE: 처리 안 됨)
	*/
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/**
	* @brief 대화 상자 초기화 함수
	* @return BOOL 초기화 성공 여부
	*/
	virtual BOOL OnInitDialog();

protected:
	/** @brief 대화 상자 표시 여부 플래그 */
	BOOL		m_bShowDlg;

	/** @brief 테스트 버튼 */
	CButtonCS m_bnTest;

	/** @brief 비전 테스트 버튼 */
	CButtonCS m_bnVisionTest;

	/** @brief AI 테스트 버튼 */
	CButtonCS m_bnAiTest;

	/** @brief AI 셋업 버튼 */
	CButtonCS m_bnAiSetup;

	/** @brief AI 적용 버튼 */
	CButtonCS m_bnAiApply;

	/** @brief 저장 후 닫기 버튼 */
	CButtonCS m_bnSaveClose;

	/** @brief 닫기 버튼 */
	CButtonCS m_bnClose;

	/** @brief 단위 변환 버튼 */
	CButtonCS m_bnConvertUnit;

protected:
	/** @brief 선택된 이미지 유형 */
	int			m_iSelectedImageType;

	/** @brief 선택된 ROI 포인터 */
	GTRegion* m_pSelectedROI;

	/** @brief 선택된 검사 탭 인덱스 */
	int			m_iSelectedInspectionTabIndex;

	/**
	* @brief 현재 탭의 파라미터를 가져옵니다
	*/
	void		GetParam();
public:
	/**
	* @brief 전체 테스트 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonTest();

	/**
	* @brief 비전 테스트 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedMfcbuttonVisionTest();

	/**
	* @brief AI 테스트 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedMfcbuttonAiTest();

	/**
	* @brief 저장 후 닫기 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonSaveAndClose();

	/**
	* @brief 닫기 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonClose();

	/**
	* @brief 단위 변환 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonConvertUnit();

	/** @brief 로그 메시지 저장 문자열 */
	CString strLog;

	/**
	* @brief AI 설정 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonAisetup();

	/**
	* @brief AI 설정 적용 버튼 클릭 이벤트 핸들러
	*/
	afx_msg void OnBnClickedButtonAisetupApply();

protected:
	/** @brief AI 설정 프로그램의 프로세스 핸들 */
	HANDLE ai_setup_process_handle_ = nullptr;

	/**
	* @brief AI 설정 프로세스 종료 대기 스레드 프로시저
	* @param pParam 스레드 파라미터 (CTeachingAlgorithmTabDlg* 포인터)
	* @return UINT 스레드 반환값
	*/
	static UINT AFX_CDECL AiSetupWaitThreadProc(LPVOID pParam);

	/**
	* @brief AI 검사를 재초기화합니다
	* @return bool 재초기화 성공 여부
	*/
	bool ReinitializeAiInspection();

	/**
	* @brief 검사를 테스트합니다
	* @param enableVision 비전 검사 활성화 여부
	* @param enableAi AI 검사 활성화 여부
	* @return bool 테스트 성공 여부
	*/
	bool InspectionTest(bool enableVision, bool enableAi);

};
