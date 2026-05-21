// AJinAXL.h : 헤더 파일
//
#pragma once

// *** AJin Configuration **********************************
// DO Board        : SIO-DO32P => 2EA
// Library Version : EzSoftwareUC V4.1.1.4033, 64Bit Version
// *********************************************************

void __stdcall OnDIOInterruptCallback(long lActiveNo, DWORD uFlag);

const int DO_COUNT = 64;		// Output Count

// Digital Input

typedef union tag_DX_DATA {
	DWORD nValue;
	struct {
		// Byte 0 (X000 ~ X007)
		DWORD iInput00 : 1;     // X000 - Encoder Trigger TOP1 → Camera 1
		DWORD iInput01 : 1;     // X001
		DWORD iInput02 : 1;
		DWORD iInput03 : 1;
		DWORD iInput04 : 1;
		DWORD iInput05 : 1;
		DWORD iInput06 : 1;
		DWORD iInput07 : 1;
		// Byte 1 (X008 ~ X015) - 미사용
		DWORD iInput08 : 1;
		DWORD iInput09 : 1;
		DWORD iInput10 : 1;
		DWORD iInput11 : 1;
		DWORD iInput12 : 1;
		DWORD iInput13 : 1;
		DWORD iInput14 : 1;
		DWORD iInput15 : 1;
		// Byte 2 (X016 ~ X023)
		DWORD iInput16 : 1;     // X016 - Encoder Trigger BTM1 → Camera 2
		DWORD iInput17 : 1;     // X017
		DWORD iInput18 : 1;
		DWORD iInput19 : 1;
		DWORD iInput20 : 1;
		DWORD iInput21 : 1;
		DWORD iInput22 : 1;
		DWORD iInput23 : 1;
		// Byte 3 (X024 ~ X031) - 미사용
		DWORD iInput24 : 1;
		DWORD iInput25 : 1;
		DWORD iInput26 : 1;
		DWORD iInput27 : 1;
		DWORD iInput28 : 1;
		DWORD iInput29 : 1;
		DWORD iInput30 : 1;
		DWORD iInput31 : 1;
	};
} DX_DATA;

typedef union tag_DY_0 {	// Vision1
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y000
		DWORD oLight1: 1;		// Y001
		DWORD oLight2: 1;		// Y002
		DWORD oLight3: 1;		// Y003
		DWORD oLight4: 1;		// Y004
		DWORD oLight5: 1;		// Y005
		DWORD oLight6: 1;		// Y006
		DWORD oLight7: 1;		// Y007

		DWORD oLight8: 1;		// Y008
		DWORD oLight9: 1;		// Y009
		DWORD oLight10: 1;		// Y010
		DWORD o011: 1;			// Y011
		DWORD o012: 1;			// Y012
		DWORD o013: 1;			// Y013
		DWORD o014: 1;			// Y014
		DWORD o015: 1;			// Y015
	};
} DY_0;

typedef union tag_DY_1 {	// Vision2
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y100
		DWORD oLight1: 1;		// Y101
		DWORD oLight2: 1;		// Y102
		DWORD oLight3: 1;		// Y103
		DWORD oLight4: 1;		// Y104
		DWORD oLight5: 1;		// Y105
		DWORD oLight6: 1;		// Y106
		DWORD oLight7: 1;		// Y107

		DWORD oLight8: 1;		// Y108
		DWORD oLight9: 1;		// Y109
		DWORD oLight10: 1;		// Y110
		DWORD o111: 1;			// Y111
		DWORD o112: 1;			// Y112
		DWORD o113: 1;			// Y113
		DWORD o114: 1;			// Y114
		DWORD o115: 1;			// Y115
	};
} DY_1;

typedef union tag_DY_2 {	// Vision3
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y200
		DWORD oLight1: 1;		// Y201
		DWORD oLight2: 1;		// Y202
		DWORD oLight3: 1;		// Y203
		DWORD oLight4: 1;		// Y204
		DWORD oLight5: 1;		// Y205
		DWORD oLight6: 1;		// Y206
		DWORD oLight7: 1;		// Y207

		DWORD oLight8: 1;		// Y208
		DWORD oLight9: 1;		// Y209
		DWORD oLight10: 1;		// Y210
		DWORD o211: 1;			// Y211
		DWORD o212: 1;			// Y212
		DWORD o213: 1;			// Y213
		DWORD o214: 1;			// Y214
		DWORD o215: 1;			// Y215
	};
} DY_2;

typedef union tag_DY_3 {	// Vision4
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y300
		DWORD oLight1: 1;		// Y301
		DWORD oLight2: 1;		// Y302
		DWORD oLight3: 1;		// Y303
		DWORD oLight4: 1;		// Y304
		DWORD oLight5: 1;		// Y305
		DWORD oLight6: 1;		// Y306
		DWORD oLight7: 1;		// Y307

		DWORD oLight8: 1;		// Y308
		DWORD oLight9: 1;		// Y309
		DWORD oLight10: 1;		// Y310
		DWORD o311: 1;			// Y311
		DWORD o312: 1;			// Y312
		DWORD o313: 1;			// Y313
		DWORD o314: 1;			// Y314
		DWORD o315: 1;			// Y315
	};
} DY_3;

#define MAX_INTERRUPT_NUMBER 2

// ===== [Assy Lens / VPC1] Vision Trigger (Bit 분할, 단일 출력 모듈) =====
const int ASSY_OUTPUT_MODULE = 0;   // Assy Lens: 모듈0 = 출력 (IO 모듈 1개)

enum E_ASSY_VISION_CAM
{
	AVCAM_TOP_INSP_1 = 0,   // Cam Y000, Light Y001~Y009
	AVCAM_BTM_INSP_1,       // Cam Y010, Light Y011~Y019
	AVCAM_SIDE_INSP_1,      // Cam Y020, Light Y021~Y023
	AVCAM_BTM_ALIGN,        // Cam Y024,Y025, Light Y026~Y028
	AVCAM_TOP_ALIGN_1,      // Cam Y029, Light Y030~Y031
	AVCAM_MAX
};

struct ASSY_TRIGGER_MAP
{
	int iCamBitStart;    // 카메라 트리거 시작 비트
	int iCamBitCount;    // 카메라 트리거 비트 수 (Btm Align = 2)
	int iLightBitStart;  // 조명 Page 트리거 시작 비트
	int iLightBitCount;  // 조명 Page 트리거 비트 수
};

// Bottom Align 카메라 선택
enum E_ASSY_CAM_SELECT
{
	ACAM_SEL_ALL = 0,   // 매핑된 카메라 비트 전체 (Btm Align = Y024,Y025 동시)
	ACAM_SEL_1,         // 첫 번째 카메라 비트만 (Btm Align = Y024)
	ACAM_SEL_2,         // 두 번째 카메라 비트만 (Btm Align = Y025)
};

class CAJinAXL
{
public:
	CAJinAXL();
	virtual ~CAJinAXL();

private:
	DY_0	m_DY0;	// Output (Y000 - Y015)
	DY_1	m_DY1;	// Output (Y100 - Y115)
	DY_2	m_DY2;	// Output (Y200 - Y215)
	DY_3	m_DY3;	// Output (Y300 - Y315)

	DX_DATA	m_DX;	// Input  (X00 - X07)

	void Init_Trigger(BOOL bOn);

	HANDLE m_hEventTrigger[MAX_INTERRUPT_NUMBER];

	DWORD m_dwAssyOutput;   // Assy Lens 32비트 출력 캐시 (idle = 0xFFFFFFFF)

public:
	BOOL Initialize();
	void Terminate();

	void Set_Trigger(int nPort, BOOL* bLight, int nS);

	void All_OFF();
	void Set_GrabSeqAddress(int nPort, BOOL* bOn);
	
	// Bit Test
	BOOL Get_Output(int nPort, int nIdx);
	void Set_Output(int nPort, int nIdx, BOOL bOut);

	void Read_Input();		// DI Read
	DX_DATA *Get_pDX() { return &m_DX; }

	// uSleep
 	LONGLONG	m_nFreq;	// Frequence
 	void DoEvents();
 	void uSleep(int msec);
	void Delay(int msec);

	CCriticalSection m_csInput;

	HANDLE GetTriggerEvent(int iVisionCamType);
	void ResetTriggerEvent(int iVisionCamType);

	void Init_AssyTrigger();                                  // Assy Lens idle 초기화
	void Set_AssyTrigger(int iCam, int iPageIndex, int nS, int iCamSelect = ACAM_SEL_ALL);
};

extern CAJinAXL g_objAJinAXL;

///////////////////////////////////////////////////////////////////////////////
