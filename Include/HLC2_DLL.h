// ====================================================================================================
// Summary
//		Definition of HL-C2 user API (dll)
//
// Creator
//	    Panasonic Industrial Devices SUNX Co., Ltd.
//
// History
//		Prepared on April 4, 2006
//		2007		Add HLC2_HeadHMeasureMode()
//		2007		Add HLC2_CmnOffDaley()
//		2008 Unsupported items were deleted.
//					New addition
//		September 2009: Additional HLC2_HeadEmissionAdjustmentAreaA()
//						 HLC2_HeadEmissionAdjustmentAreaB()
//						 HLC2_GetBufferBinaryData()
//		May 2013:		Additional HLC2_HeadMedianFilter()
//						 HLC2_HeadMeasuringRangePointA()
//						 HLC2_HeadMeasuringRangePointB()
//		November 2013:	Additional HLC2_GetLastError()
// ====================================================================================================

#ifndef _HLC2_DLL_H
#define _HLC2_DLL_H

#pragma	pack(push, 1)

#ifdef HLC2_DLL_EXPORTS
#define HLC2_DLL_API __declspec(dllexport)
#else
#define HLC2_DLL_API __declspec(dllimport)
#endif



//---------------------------------------------------------------------------
// Variable type
typedef	unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef char * PCHAR;
typedef DWORD * LPDWORD;
typedef void * HLC2_HANDLE;
typedef DWORD HLC2_STATUS;
typedef double * LPDOUBLE;

// HLC2_STATUS
#define HLC2_OK                 0       // Normal end
#define HLC2_INVALID_HANDLE     1       // Handle is incorrect.
#define HLC2_DEVICE_NOT_FOUND   2       // No device exists.
#define HLC2_DEVICE_NOT_OPENED  3       // The device is not open.
#define HLC2_CONTROLLER_ERROR	4		// Controller abnormal response
#define HLC2_INVALID_PARAMETER  5       // Parameter is incorrect.
#define HLC2_RECEIVE_ERROR		6		// Receive error
#define HLC2_FILE_OPEN_ERROR	7		// File cannot be opened.
#define HLC2_FILE_CREATE_ERROR	8		// No file is created.
#define	HLC2_SHORTAGE_MEMORY	9		// Available memory is low.



#define	HLC2_DEFAULT_RX_TIMEOUT		10000		// Read Timeout : 10sec
#define	HLC2_DEFAULT_TX_TIMEOUT		10000		// Write Timeout : 10sec



// OUT selection (value specified to dwOut)
#define	OUT1		0
#define	OUT2		1

// Specifies input/output (value specified to dwIO)
#define	IO_IN		0		// Loading
#define	IO_OUT		1		// Setting/execution



//---------------------------------------------------------------------------
// Definition of structure

// Setting code (for code setting 00000 to 99999)
typedef struct
{
	BYTE	Num[5];
} HLC2_CONFIG5;

// Setting code (for numerical value setting)
typedef struct
{
	BYTE	Sign;						// Code ("+/-")
	BYTE	Integer[3];					// 3-digit integer (no zero suppression)
	BYTE	Period;						// Decimal point(".")
	BYTE	Decimal[6];					// 6 decimal places
} HLC2_NUMERIC11;

// Setting code (for difference value setting)
typedef struct
{
	BYTE	Sign;						// Code ("+/-")
	BYTE	Integer[4];					// 4-digit integer (no zero suppression)
	BYTE	Period;						// Decimal point(".")
	BYTE	Decimal[6];					// 6 decimal places
} HLC2_NUMERIC12;

// Setting code (for setting of 10-digit value)
typedef struct
{
	BYTE	Sign;						// Code ("+/-")
	BYTE	Integer[10];				// 10-digit integer (no zero suppression)
} HLC2_NUMERIC10;

//////////////////////////////////////
// Command format 1 (for code setting) //
//////////////////////////////////////
// Transmission format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE#")
	BYTE	Command[3];					// Command
	BYTE	Id;							// Identification code (0 to 5)
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_SENDCOMFMT1_READ;	// Common with subsequent R command format during transmission

typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE#")
	BYTE	Command[3];					// Command
	BYTE	Id;							// Identification code (0 to 5)
	HLC2_CONFIG5	Config;				// Setting code (00000 to 99999)
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_SENDCOMFMT1_WRITE;

// Reception format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command
	HLC2_CONFIG5	Config;				// Setting code (00000 to 99999)
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOMFMT1_READ;

typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOMFMT1_WRITE;	// Common items when receiving W command format 2

typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE!")
	BYTE	ErrCode[2];					// Error code
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOMFMT1_ERROR;	// Common in all formats in case of abnormal response


//////////////////////////////////
// Command format 2 (for numerical value setting) //
//////////////////////////////////
// Transmission format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE#")
	BYTE	Command[3];					// Command
	BYTE	Id;							// Identification code (0 to 5)
	HLC2_NUMERIC11	Numeric;			// Setting code
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_SENDCOMFMT2_WRITE;

// Reception format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command
	HLC2_NUMERIC11	Numeric;			// Setting code
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOMFMT2_READ;


//////////////////////////////////////
// Command format 3 (for setting of 10-digit value) //
//////////////////////////////////////
// Transmission format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE#")
	BYTE	Command[3];					// Command
	BYTE	Id;							// Identification code (0 to 5)
	HLC2_NUMERIC10	Numeric;			// Setting code
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_SENDCOMFMT3_WRITE;

// Reception format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command
	HLC2_NUMERIC10	Numeric;			// Setting code
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOMFMT3_READ;


//////////////////////////////////////////////
// Special command format 1 (for reading out 2 output measurement values) //
//////////////////////////////////////////////
// The transmission format structure is same as the one for the command format 1.
// Command: "RMA", identification code: "5"

// Reception format structure
typedef struct
{
	HLC2_NUMERIC11	Numeric[2];				// OUT1 and OUT2 measurement values
} HLC2_OUTMEASUREVALUE;

typedef struct
{
	BYTE	Symbol[4];						// Symbol ("%EE$")
	BYTE	Command[3];						// Command ("RMA")
	HLC2_OUTMEASUREVALUE	MeasureValue;	// OUT1 and OUT2 measurement values 
	BYTE	Bcc[2];							// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOM_SPFMT1;


//////////////////////////////////////////
// Special command format 2 (for all output readout) //
//////////////////////////////////////////
// The transmission format structure is same as the one for the command format 1.
// Command: "RMB", identification code: "5"

// Reception format structure
typedef struct
{
	HLC2_NUMERIC11	Numeric;		// OUT1/2 measurement value
	BYTE	StrobeOut;				// Strobe output (0 or 1)
	BYTE	HighOut;				// Judgment output HI (0 or 1)
	BYTE	GoOut;					// Judgment output GO (0 or 1)
	BYTE	LowOut;					// Judgment output LO (0 or 1)
	BYTE	ExtOut;					// Not used
	BYTE	AlarmOut;				// Alarm output (0 or 1)
} HLC2_OUT_DATA;

typedef struct
{
	HLC2_OUT_DATA	OutData[2];			// {OUT1: Measurement value/Strobe output/Judgment output HI/Judgment output GO/Judgment output LO/Not used/Alarm output},
										// {OUT2: Measurement value/Strobe output/Judgment output HI/Judgment output GO/Judgment output LO/Not used/Alarm output}
} HLC2_OUTALL_DATA;

typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command ("RMB")
	HLC2_OUTALL_DATA	OutAllData;		// All output readout data
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_RECVCOM_SPFMT2;


//////////////////////////////////////////////////////////////////
// Special command format 4 (for buffering data normal readout) //
// Special command format 5 (for buffering data difference data readout) //
// Special command format 5 (for buffering binary data readout) //
//////////////////////////////////////////////////////////////////

// Transmission format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE#")
	BYTE	Command[3];					// Command ("RMA" / "RMB" / "RMC")
	BYTE	Id;							// Identification code (3 or 4)
	BYTE	TopPoint[5];				// Top point (00001 to 99999)
	BYTE	EndPoint[5];				// End point (00001 to 99999)
	BYTE	Bcc[2];						// BCC("**")
	BYTE	Cr;
} HLC2_SENDCOM_SPFMT;		// Common with R command format at transmission

// Reception format structure
typedef struct
{
	BYTE	Symbol[4];					// Symbol ("%EE$")
	BYTE	Command[3];					// Command ("RMA" / "RMB" / "RMC"))
	BYTE	BufferingData[1];			// Buffering data (variable length)
} HLC2_RECVCOM_SPFMT;		// Common with R command format at receiving


// Buffering upper/lower limit data group structure
typedef struct
{
	DWORD	TopPoint;					// Top point (00001 to 99999)
	DWORD	EndPoint;					// End point (00001 to 99999)
	HLC2_NUMERIC11	*pLimitData;		// Setting data stored point
} HLC2_BUFFERLIMIT;


// Buffering normal read data structure
typedef struct
{
	DWORD	TopPoint;					// Top point (00001 to 99999)
	DWORD	EndPoint;					// End point (00001 to 99999)
	DWORD	dwCount;					// Read counter value (not used)
	HLC2_NUMERIC11	*pGetData;			// Read data stored point
} HLC2_BUFFERNORMAL;


// Buffering fast read data structure (Difference area ->Convert to the measurement value calculated from difference, or)
//									   Outputs top point data and difference area data formed into 4-digit integer and 6 decimal places.)
typedef struct
{
	DWORD	TopPoint;					// Top point (00001 to 99999)
	DWORD	EndPoint;					// End point (00001 to 99999)
	DWORD	dwCount;					// Read counter value (not used)
	HLC2_NUMERIC12	*pGetData;			// Top point read data, 2nd and later calculation data stored point
} HLC2_BUFFERRAPID;


// Buffering binary read data structure
typedef struct
{
	DWORD	TopPoint;					// Top point (00001 to 99999)
	DWORD	EndPoint;					// End point (00001 to 99999)
	DWORD	dwBinCount;					// Read counter value (not used)
	DWORD	*pdwBinData;				// Read data (binary format) stored point
} HLC2_BUFFERBINARY;



// Memory copy structure
// The function is limited in this version.
typedef struct
{
	BYTE	SetMemFrom;					// Copy source memory No. (0 to 15)
	BYTE	SetMemTo;					// Copy destination memory No. (0 to 15)
	BYTE	MemCopyCom;					// Copy command (0: No command, 1: Execute 
} HLC2_MEMCOPY;

//---------------------------------------------------------------------------
// Exported function
// USB device control API

// Opens HL-C2 by a device No.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OpenByIndex(DWORD dwIndex, HLC2_HANDLE *hlc2Handle);

// Loads the number of connections for HL-C2
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetCount(LPDWORD lpCount);

// Initializes specified device.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_Init(HLC2_HANDLE hlc2Handle);

// Closes specified device.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_Close(HLC2_HANDLE hlc2Handle);

// Loads string of serial number.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetSerialNumber(DWORD dwIndex, PCHAR szSerialNumberBuffer);

// Opens HL-C2 by a serial No.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_Open(HLC2_HANDLE *hlc2Handle, PCHAR szSerialNumber);

// Returns the value of the last error.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetLastError(DWORD *pdwResponseCode);



//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////
//	API function of controller specific command						//
//////////////////////////////////////////////////////////////

//----------------------------------------
// Head A, B API
//----------------------------------------
// Loads/sets installation mode.
// Installation mode (*lpReflect = 0:Diffuse, 1:Specular)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadSetupMode(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO,
												   LPDWORD lpReflect, BYTE bccFlg);

// Loads/sets emission adjustment.
// Emission adjustment (*lpInfo = 0: Auto 1 to 31: Fixed value)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadFloodLightAjust(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO,
														 LPDWORD lpInfo, BYTE bccFlg);

// Executes emitted light intensity search or loads its status.
// Emitted light intensity search command (*lpStatus = 0: No command, 1: Execute, 2: Searching)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecFloodLight(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpStatus, BYTE bccFlg);

// Loads/sets alarm delay times.
// Alarm delay times (*lpCount = 0: OFF, 1 to 65535:Hold previous normal value)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadAlarmDelayTimes(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpCount, BYTE bccFlg);

// Loads/sets measurement mode.
// Measurement mode (*lpSelect = 0: Diffuse Reflection [Standard], 1: Specular Reflection [Standard] , 2: Metal 1, 3: Metal 2, 4:Penetration,
// 5: Glass, 6: Glass pattern)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadHMeasureMode(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets measurement surface reference.
// Measurement surface reference (*lpSelect = 0:Near 1:Far)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadMeasureWorkBasis(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets calibration measurement value A.
// Calibration measurement value A (*lpdValue = -950.000000  to  +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadCalibMeasureValueA(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets calibration correction value a.
// Calibration correction value a (*lpdValue = -950.000000  to  +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadCalibCorrectValueA(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets calibration measurement value B.
// Calibration measurement value B (*lpdValue = -950.000000  to  +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadCalibMeasureValueB(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets calibration correction value b.
// Calibration correction value b (*lpdValue = -950.000000  to  +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadCalibCorrectValueB(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Executes calibration.
// Calibration command (*lpStatus = 0: No command, 1: Execute, 2: Cancel)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecCaliburation(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpStatus, BYTE bccFlg);

// Loads/sets laser control.
// Laser control (*lpOnOff = 0: Laser ON, 1: Laser OFF)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadLaserOff(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpOnOff, BYTE bccFlg);

// Loads received light intensity data.
// Readout received light intensity (*lpValue = 00000 to 01023)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetLightWaveData(HLC2_HANDLE hlc2Handle, DWORD dwHead, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets peak recognition sensitivity.
// Peak recognition sensitivity (*lpValue = 100  to  400)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadPeakSearchMinLevel(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets emission adjustment area a.
// Emission adjustment area a (*lpValue = 1  to  512)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadEmissionAdjustmentAreaA(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets emission adjustment area b.
// Emission adjustment area b (*lpValue = 1  to  512)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadEmissionAdjustmentAreaB(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets median filter.
// Median filter (*lpSelect = 0:OFF 1:5 1:15 1:31)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadMedianFilter(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets measuring range point a.
// Measuring range point a (*lpValue = 3 to 510)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadMeasuringRangePointA(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets measuring range point b.
// Measuring range point b (*lpValue = 3 to 510)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_HeadMeasuringRangePointB(HLC2_HANDLE hlc2Handle, DWORD dwHead, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);



//----------------------------------------
// OUT1, 2 API
//----------------------------------------
// Loads/sets output selection.
// Output selection (*lpPattern = 0:'A',1:'B',2:'-A',3:'-B',4:'A+B',5:'-(A+B)',
//  6: 'A-B'. 7: 'B-A', 8: 'A transparent object', 9: 'B transparent object', 10: '-A transparent object', 11: '-B transparent object')
//  12: 'A1+B1[Transparent object]', 13 :'-(A1+B1)[Transparent object]', 14: 'A1-B1[Transparent object]', 15: 'B1-A1[Transparent object]')
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutPattern(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpPattern, BYTE bccFlg);

// Loads/sets output selection: transparent object.
// Transparent object selection (*lpWork = 0:'1st surface', 1:'2nd surface', 2:'3rd surface', 3:'4th surface', 4:'Maximum surface',
//  5: '1st surface - 2nd surface', 6: '1st surface - 3rd surface', 7: '1st surface - 4th surface', 8: '1st surface - Maximum surface', 9: '2nd surface- 3rd surface', 
//  10: '2nd surface - 4th surface', 11: '2nd surface - Maximum surface', 12: '3rd surface - 4th surface', 13: '3rd surface - Upper limit surface', 14: '4th surface - Maximum surface') 
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutMeasureWork(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpWork, BYTE bccFlg);

// Loads/sets output selection: transparent object refractive index calculation.
// Refractive index calculation (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutReflectionCalc(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets output selection: transparent object refractive index
// Reflective index (*lpdRate = +000.500000 to +002.000000)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutReflectionRate(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdRate, BYTE bccFlg);

// Loads/sets zero set.
// Zero set (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutZeroSet(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets timing.
// Timing (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutTiming(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets reset.
// Reset (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutReset(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets hold.
// Hold (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutHold(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets measurement mode.
// Measurement mode (*lpMode = 0: Normal, 1: Peak, 2: Bottom, 3: P-P)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutMeasureMode(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpMode, BYTE bccFlg);

// Loads/sets filter operation.
// Filter operation (* lpSelect = 0: Moving average, 1: low-pass filter, 2: high-pass filter)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutFilterSelect(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets average times.
// Average times (*lpSelect = 0: 1 time, 1: 2 times, 2: 4 times, 3: 8 times, 4:16 times, 5: 32 times,
//  6: 64 times, 7: 128 times, 8: 256 times, 9: 512 times, 10: 1024 times, 11: 2048 times
//  12: 4096 times, 13: 8192 times, 14: 16384 times, 15: 32768 times, 16: 65536 times)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutAverageTimes(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets cutoff frequency.
// Cutoff frequency (*lpSelect = 0:1, 1:2, 2:4, 3:10, 4:20, 5:40, 6:100, 7:200, 8:400,
// 9:1000, 10:2000[Hz])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutCutOffCycle(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets operation coefficient.
// Operation coefficient (*lpdValue = 000.100000  to  009.999999)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutSpan(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets offset.
// Offset (*lpdOffset = -950.000000 to +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutOffsetInput(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdOffset, BYTE bccFlg);

// Loads/sets judgment output: upper limit value.
// Judgment output: upper limit value (*lpdValue = -950.000000 to +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDecisionMax(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets judgment output: lower limit value.
// Judgment output: lower limit value (*lpdValue = -950.000000 to +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDecisionMin(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads judgment output: upper limit hysteresis.
// Judgment output: upper limit hysteresis (*lpdHis = +000.000000 to 950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDecisionHisMax(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdHis, BYTE bccFlg);

// Loads/sets judgment output: lower limit hysteresis.
// Judgment output: lower limit hysteresis (*lpdHis = +000.000000 to 950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDecisionHisMin(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdHis, BYTE bccFlg);

// Loads/sets analog scaling measurement value A.
// Analog scaling measurement value A (*lpdValue = -950.000000 to +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutScalingMeasureValueA(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets analog scaling voltage a.
// Analog scaling voltage a (*lpdVoltage = +/-010.000000[V] The last three digits are zero-fixed.)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutScalingVoltageValueA(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdVoltage, BYTE bccFlg);

// Loads/sets analog scaling measurement value B
// Analog scaling measurement value B (*lpdValue = -950.000000 to +950.000000[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutScalingMeasureValueB(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets analog scaling voltage b.
// Analog scaling voltage b (*lpdVoltage = +/-010.000000[V] The last three digits are zero-fixed.)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutScalingVoltageValueB(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdVoltage, BYTE bccFlg);

// Executes analog scaling
// Analog scaling command (dwStatus = 0: No command, 1: Execute, 2: Cancel)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecAnalogScaling(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, DWORD dwStatus, BYTE bccFlg);

// Loads/sets analog output at alarm.
// Analog output at alarm (*lpSelect = 0: Hold previous value, 1: Fixed value)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutAnalogOutOnAlarm(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets analog output at alarm: fixed value.
// Analog output at alarm: fixed value (*lpdValue = +/-010.800000[V] The last three digits are zero-fixed.)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutFixedValueInput(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets analog output at data unfixed.
// Analog output at data unfixed  (*lpdValue = +/-010.800000[V] The last three digits are zero-fixed.)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutAnalogOutOnUnfixed(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads/sets digital output at alarm.
// Digital output at alarm (*lpSelect = 0: Hold previous value, 1: Fixed value)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDegitalOutOnAlarm(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets alarm output delay.
// Alarm output delay (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutAlarmDelayChange(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets digit number of measurement value.
// Digit number of measurement value (*lpSelect = 0: 6 decimal places, 1: 5 decimal places, 2: 4 decimal places,
//  3: 3 decimal places, 4: 2 decimal places, 5: 1 decimal place)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_OutDispDigit(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Reads out measurement value.
// Measurement value (*lpdvalue = -999.999999 to +999.999999[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetMeasureValue(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDOUBLE lpdValue, BYTE bccFlg);

// Loads alarm output status.
// Alarm output status  (*lpStatus = 0: No alarm output status (OFF), 1: Measurement alarm output status (ON),
//		5: Sensor head unconnected (ON), 6: Sensor head unadapted (ON), 7: Head connection check error (ON))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetAlarmState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);

// Load strobe status
// Strobe output status  (*lpStatus = 0: No strobe status (OFF), 1: Strobe output status (ON))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetStrobeState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);

// Loads judgment output HI status.
// Judgment output HI status (*lpStatus = 0: Judgment output HI no output status (OFF), 1: Judgment output HI output status (ON))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetHighState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);

// Loads judgment output GO status.
// Judgment output GO status (*lpStatus = 0: Judgment output GO no output status (OFF), 1: Judgment output GO output status (ON))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetGoState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);

// Loads judgment output LO status.
// Judgment output LO status (*lpStatus = 0: Judgment output LO no output status (OFF), 1: Judgment output LO output status (ON))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetLowState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);



//----------------------------------------
// Common API
//----------------------------------------
// Loads/sets sampling cycle.
// Sampling cycle (*lpCycle = 0:10, 1:20, 2:40, 3:100, 4:200, 5:400[us],
//  6:1[ms], 7:2[ms])
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_CmnSamplingCycle(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpCycle, BYTE bccFlg);

// Loads/sets interference prevention.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_CmnPreventInterference(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets terminal input control.
// Terminal input control (*lpSelect = 0: Independent, 1: All)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_CmnTerminalInputCtrl(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets chattering prevention for terminal input.
// Chattering prevention for terminal input (*lpSelect = 0:OFF, 1:ON1, 2:ON2)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_CmnTerminalInputChattering(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads 2 output measurement value.
// OUT1 and OUT 2 measurement values (-999.999999 to +999.999999[mm])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_Get2OutMeasureValue(HLC2_HANDLE hlc2Handle, HLC2_OUTMEASUREVALUE *pOutMeasureValue, BYTE bccFlg);

// Loads all [once] output readout. 
// All output readout (OUT1: Measurement value/Strobe output/Judgment output HI/Judgment output GO/Judgment output LO/Not used/Alarm output
///OUT2: Measurement value/Strobe output/Judgment output HI/ Judgment output GO/Judgment output LO/Not used/Alarm output)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetOutAll(HLC2_HANDLE hlc2Handle, HLC2_OUTALL_DATA *pOutAllData, BYTE bccFlg);

//Loads/sets judgment output off delay.
//Judgment output off delay (OFF/2 ms/10 ms/100 ms/hold)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_CmnOffDaley(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);



//----------------------------------------
// System API
//----------------------------------------
// Copies setting between OUT1 and OUT2
// Copy command (*lpSelect = 0: No command, 1: Copy OUT1 to OUT2, 2: Copy OUT2 to OUT1)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecOutConfigCopy(HLC2_HANDLE hlc2Handle, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets memory change priority setting.
// Sets memory change priority (*lpPriority = 0: Command, 1: Terminal)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysMemChangePriority(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpPriority, BYTE bccFlg);

// Specifies memory No., then executes memory change or loads its status.
// Memory change No. (*lpMemNo = 0 to 15)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecMemChange(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpMemNo, BYTE bccFlg);

// Specify copy source and copy destination, and then execute memory copy.
// Copy source memory No. (pMemCopy->SetMemFrom = 0 to 15)
// Copy destination memory No. (pMemCopy->SetMemTo = 0 to 15)
// Memory copy command (pMemCopy->MemCopyCom = 0: No command, 1: Execute)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecMemCopy(HLC2_HANDLE hlc2Handle, HLC2_MEMCOPY *pMemCopy, BYTE bccFlg);

// Initializes selected memory/initialize all memory.
// Memory initialization command (*lpSelect = 0: No command, 1: Selected memory, 2: All memory)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecMemInitialize(HLC2_HANDLE hlc2Handle, LPDWORD lpSelect, BYTE bccFlg);

// Saves all memory.
// All memory save command (*lpSelect = 0: No command, 1: All memory)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecMemSave(HLC2_HANDLE hlc2Handle, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets RS-232C baud rate.
// RS-232C baud rate (*lpSelect = 0:9600, 1:19200, 2:38400, 3:115200[bps])
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysRs232cBaudrate(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets RS-232C data length
// RS-232C data length (*lpLength = 0: 7 bits, 1: 8 bits)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysRs232cDataLen(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpLength, BYTE bccFlg);

// Loads/sets RS-232C parity check.
// RS-232C parity check (*lpSelect = 0: Even number, 1: Odd number, 2: None)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysRs232cParity(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets RS-232C output mode.
// Loads/sets RS-232C output mode. (0: Handshake, 1: Timing, 2: Continuous)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysRs232cOutMode(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets RS-232C output type.
// RS-232C Output Type(0:OUT1&2, 1:OUT1, 2:OUT2)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysRs232cOutType(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets display update cycle of measurement value
// Display update cycle of measurement value (*lpSelect = 0: Fast, 1: Standard, 2: Slow, 3: Very slow)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysMeasureUpdateCycle(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets console startup screen.
// Console start screen (*lpSelect = 0 to 15: Start screen No.)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysConsoleStartNo(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets console panel lock status.
// Console panel lock status (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_SysConsolePanelLock(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);



//------------------------------------------------
// Setting (buffering) API
//------------------------------------------------
// Loads/sets buffering mode.
// Buffering mode (*lpSelect = 0: Continuous, 1: Trigger, 2: Timing, 3: Sample Trigger)
// The function is limited in this version.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferingMode(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets buffering type.
// Buffering type (*lpSelect = 0: OUT 1&2, 1: OUT1, 2: OUT2)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferingType(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets buffering rate.
// Buffering rate (*lpRate = 0:1, 1:1/2, 2:1/4, 3:1/8, 4:1/16, 5:1/32, 6:1/64, 7:/128,
//  8:/256, 9/512, 10:1/1024, 11:1/2048, 12:1/4096, 13:1/8192, 14:1/16384, 15:1/32768)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferingRate(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpRate, BYTE bccFlg);

// Loads/sets accumulated amount.
// Accumulated amount (*lpNum = 1 to Maximum accumulated amount)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferStoreNum(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpNum, BYTE bccFlg);

// Loads/sets sampling trigger accumulation amount.
// Sampling trigger accumulation amount (*lpNum = 1 to accumulated amount (However, accumulated amount/sample trigger accumulation amount shall be an integer.))
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferSampleTriggerStoreNum(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpNum, BYTE bccFlg);

// Loads/sets trigger point.
// Trigger point (*lpNum = 1 to accumulated amount)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferTriggerPoint(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpNum, BYTE bccFlg);

// Loads/sets trigger delay.
// Trigger delay (*lpValue = 0  to  1000000000)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferTriggerDelay(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpValue, BYTE bccFlg);

// Loads/sets trigger conditions.
// Trigger conditions (* lpSelect = 0: At timing input ON, 1: At HI, 2: At LO, 3: At HIorLO, 4: When HI turns to GO, 5: When LO turns to GO, 6: When HIorLO turns to GO, 7: At an alarm occurred, 8: At an alarm released)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferEventCondition(HLC2_HANDLE hlc2Handle, DWORD dwOut, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Loads/sets buffering operation.
// Buffering operation (*lpStatus = 0: Stop, 1: Start)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_ExecBuffering(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpStatus, BYTE bccFlg);

// Loads/sets self stop.
// Self-stop (*lpSelect = 0:OFF, 1:ON)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_BufferSelfStop(HLC2_HANDLE hlc2Handle, DWORD dwIO, LPDWORD lpSelect, BYTE bccFlg);

// Reads out buffering status.
// Readout status (*lpStatus= 0: Non-buffering, 1: Wait for trigger, 2: Accumulating, 3: Accumulation completed)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferState(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpStatus, BYTE bccFlg);

// Loads final data point for buffering.
// Final data point (*lpNum)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferFinalDataPoint(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpNum, BYTE bccFlg);

// Reads out trigger counter.
// Trigger counter (*lpCount)
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferTriggerCount(HLC2_HANDLE hlc2Handle, DWORD dwOut, LPDWORD lpCount, BYTE bccFlg);

// Reads buffering data normally.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferDataNormal(HLC2_HANDLE hlc2Handle, DWORD dwOut, 
														 HLC2_BUFFERNORMAL *BufferNormal, BYTE bccFlg);

// Reads buffering data rapidly.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferDataRapid(HLC2_HANDLE hlc2Handle, DWORD dwOut,
												HLC2_BUFFERRAPID *BufferRapid, LPDWORD lpSelect, BYTE bccFlg);

// Reads buffering binary data.
HLC2_DLL_API HLC2_STATUS WINAPI HLC2_GetBufferBinaryData(HLC2_HANDLE hlc2Handle, DWORD dwOut,
														 HLC2_BUFFERBINARY *BufferBinary, BYTE bccFlg);



#pragma	pack(pop)

#endif	// _HLC2_DLL_H
