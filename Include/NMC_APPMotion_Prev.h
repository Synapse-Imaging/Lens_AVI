#ifndef __NMCAPPMOTION_H__
#define __NMCAPPMOTION_H__


#ifndef __NMCMOTIONSDK_H__
#include "NMC_Motion.h"
#endif


#ifdef FLAMENCOMOTIONAPI_EXPORTS
#define APPMOTIONAPI __declspec(dllexport)
#else
#define APPMOTIONAPI __declspec(dllimport)
#endif

//
typedef enum TRConfigEncDir_tag
{
	TRC_NoInvert = 0,
	TRC_Invert 
}TRConfigEncDir;

typedef enum TRConfigVLevel_tag
{
	Level_24V = 0,
	Level_5V,
	Level_Both,
}TRConfigVLevel;

typedef enum TRConfigMode_tag
{
	Mode_Table = 0,
	Mode_Interval = 1,
	Mode_OneShot = 2,
	Mode_Frequency = 3
}TRConfigMode;

typedef enum TRTableMode_tag
{
	Mode_High = 0,
	Mode_Low,
	Mode_High_Width,
	Mode_Low_Width,
	Mode_Toggle,
}TRTableMode;	//TRConfigMode;


typedef enum GearHomeResult_tag
{
	GEAR_HOME_SEARCHING		 = 1,
	GEAR_USER_HALT_COMMAND	 = 2,
	GEAR_PRAMETER_READ_FAIL	 = 3,
	GEAR_GEAR_SETTING_FAIL	 = 4,
	GEAR_HOME_STOP_ERROR_STATE = 5,
	GEAR_HOME_STOP_RANGE		 = 6,
	GEAR_HOME_FAIL			 = 7,
	GEAR_HOME_STOP_SERVO_OFF   = 8,
	GEAR_HOME_COMPLETE		 = 0,
}GearHomeResult;

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

//디바이스의 통신 Status 변경
//BoardID, DeviceID : Board ID, Status 변경을 진행 할 Device Array
//Size : Device Count
//Mode : 0 Init, 1 Boot, 2 PreOP, 3 SafeOP, 4 OP
//TimeOut : TimeOut 처리 시간 (msec), 해당 설정 시간내에 Status 변경이 완료 되지 않을 경우 Return Error
APPMOTIONAPI MC_STATUS __stdcall MC_StatusChange_OP(UINT16 BoardID, UINT16* DeviceID, UINT16 Size,UINT16 Mode, UINT32 TimmeOut);

// ==================================================== Trigger =============================================================================================================

/*Trigger Configuration 설정, 선택 Trigger Index 의 Trigger 출력을 위한 기본 파라미터를  설정 한다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
EncSourceIndex : Encoder Sorce (0~7 선택 가능), (100 : Axis 0 Command Pos)
InvertEncDir : Trigger Position 설정 시 방향 반전. (false : NoInvert, true : Invert)
EncRatio : Trigger Position 설정 시 Ratio 반영, Encoder Ratio 값 설정
VLevel : (0:24V, 1:5V, 2:Both)  (5V 신호는 각 축 Axis Cable 11,12 Pin, 24V 신호는 DIO Cable 에서 출력 된다)
ActiveLevel : Trigger 출력 Active Level 설정 (true : High, false : Low)
Mode : 0:Interval, 1:Table, 2:OneShot, 3 : Frequency*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetTriggerConfig(UINT16 BoardID, UINT16 DeviceID ,UINT16 TriggerIndex, UINT16 EncSourceIndex, bool InvertEncDir, double EncRatio, UINT32 VLevel, bool ActiveLevel, UINT32 Mode);  

/*
Encoder Counter 를 Clear 한다.*/
APPMOTIONAPI MC_STATUS __stdcall TR_TriggerEncoderCounterClear(UINT16 BoardID, UINT16 DeviceID, UINT16 EncSourceIndex);

/*Table Trigger 설정을 진행 한다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
Trigger 출력 Position
Mode : Signal 출력 Mode (0:High, 1:LOW, 2:High_Width, 3:Low_Width, 4:Toggle)
Width : Trigger 출력 폭 설정 (1당 100usec, Max 16383 설정 가능)
Count : Trigger 개수 (Max 128 개) */
APPMOTIONAPI MC_STATUS __stdcall TR_SetTableTriggerData(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, double* Pos, UINT32* Mode, UINT16 Width, UINT16 Count);

/*
Table Trigger Data 를 Clear 한다.
EncSourceIndex : Encoder Sorce (0~7 선택 가능), (100 : Axis 0 Command Pos) */
APPMOTIONAPI MC_STATUS __stdcall TR_TableTriggerDataClear(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, bool TriggerOff);

/*
Interval Trigger 관련 설정을 진행 한다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
StartPos, EndPos : Interval Trigger 시작 위치 종료 위치
Period : Trigger 발생 주기 설정
Widhth : Trigger 발생 폭. (1당 50nsec)*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetIntervalTriggerData(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, double StartPos, double EndPos, double Period, UINT16 Width);

/*
Interval Trigger Enable, Disable
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
Mode : Start 위치에서 Trigger 출력 여부 설정 (0:None Trigger, 1:With Trigger) */
APPMOTIONAPI MC_STATUS __stdcall TR_SetIntervalTriggerEnable(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, bool Enable, UINT32 Mode);


/*
Interval Trigger 관련 설정과 Enable/Disable 동작을 한번에 진행 한다, 해당 함수 호출 시 Trigger Mode 는 자동으로 Interval Mode로 설정 된다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
StartPos, EndPos : Interval Trigger 시작 위치 종료 위치
Period : Trigger 발생 주기 설정
Widhth : Trigger 발생 폭. (1당 50nsec)
Mode : Start 위치에서 Trigger 출력 여부 설정 (0:None Trigger, 1:With Trigger)
*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetIntervalTriggerData_WEN(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, double StartPos, double EndPos, double Period, UINT16 Width, bool Enable,  UINT32 Mode);

/*
OneShot Trigger 를 발생 시킨다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
Width : Trigger 출력 폭 설정 (1당 50nsec, Max 65535 설정 가능)*/
APPMOTIONAPI MC_STATUS __stdcall TR_TriggerOneShot(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, UINT16 Width);

/*
Frequency Trigger 관련 설정을 진행 한다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
Frequency : Trigger 출력 Hz 설정 ( 1 ~ 1000000 설정 가능)
Count : Trigger 출력 개수 설정
Width : Trigger 출력 폭 설정 (1당 50nsec, Max 65535 설정 가능)*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetFrequencyTriggerData(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, UINT32 Frequency, UINT32 Count ,UINT16 Width);

/*
Frequency Trigger Enable, Disable
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능)
Mode : Count, Infinite (0 : Count Mode, 1 : Infinite Mode)*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetFrequencyTriggerEnable(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, bool Enable, UINT32 Mode);

/*
Frequency Trigger 관련 설정과 Enable/Disable 동작을 한번에 진행 한다, 해당 함수 호출시 Mode 는 자동으로 Frequency Mode 로 변경된다.
BoardID, DeviceID : BoardID, FunctionModule DeviceID
TriggerIndex : Trigger Index (1~3, 5~7 선택 가능) 
Frequency : Trigger 출력 Hz 설정 ( 1 ~ 1000000 설정 가능)
Count : Trigger 출력 개수 설정
Width : Trigger 출력 폭 설정 (1당 50nsec, Max 65535 설정 가능)
Mode : Count, Infinite (0 : Count Mode, 1 : Infinite Mode)*/
APPMOTIONAPI MC_STATUS __stdcall TR_SetFrequencyTriggerData_WEN(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, UINT32 Frequency, UINT32 Count ,UINT16 Width, bool Enable, UINT32 Mode);

/*InterVal Trigger 출력 Count를 Read 한다*/
APPMOTIONAPI MC_STATUS __stdcall TR_GetIntervalPulseCount(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, UINT32* PulseCount);

/*Frequency Trigger 출력 Count를 Read 한다*/
APPMOTIONAPI MC_STATUS __stdcall TR_GetFrequencyPulseCount(UINT16 BoardID, UINT16 DeviceID, UINT16 TriggerIndex, UINT32* PulseCount);


// ==================================================== Trigger =============================================================================================================


/*
Position 까지 절대 위치 구동하며 고속, 저속 구간으로 Velocity Override 형태로 구동한다. (고속 -> 저속)
Position : Target Position
EventPosition : 해당 포지션 까지 도달 하였을때 LowVelocity 값으로 속도변경이 완료 된다.
Velocity : 고속 구간 속도 값.
LowVelocity : 저속 구간 속도 값.*/
APPMOTIONAPI MC_STATUS __stdcall AMC_MoveDown(UINT16 BoardID, UINT16 AxisID, double Position, double EventPosition, double Velocity, double LowVelocity, 
	double Accel, double Decel, double Jerk);

/*
Position 까지 절대 위치 구동하며 저속, 고속 구간으로 Velocity Override 형태로 구동한다. (저속 -> 고속)
Position : Target Position
EventPosition : 해당 포지션 부터 Velocity 값으로 가속을 시작한다.
Velocity : 고속 구간 속도 값.
LowVelocity : 저속 구간 속도 값.*/
APPMOTIONAPI MC_STATUS __stdcall AMC_MoveUp(UINT16 BoardID, UINT16 AxisID, double Position, double EventPosition, double Velocity, double LowVelocity, 
	double Accel, double Decel, double Jerk);

/*
Torque Limit 값을 Write 한다. Master Parameter Maximum Torque/Current Limit Mode 에 설정된 항목에 따라 Write 되는 영역이 변경된다.*/
APPMOTIONAPI MC_STATUS __stdcall AMC_WriteTorqueLimitValue(UINT16 BoardID, UINT16 AxisID, UINT16 Data);

/*
Gear Mode 에서의 Master, Slave 홈서치 동작을 진행 한다.
Offset : Master, Slave Home Sensor의 Offset 값 (Home Mode 2로 Offset 값을 찾은 다음 해당 값 입력)
OffsetRange : Master 홈서치 후 Slave 홈서치시 설정 Range 이상 구동시 정지 (Home Fail), 설정 Range 값을 초과할 경우 Home Fail 이 발생하므로 Offset 값보다 충분히 크게 설정해야 한다.
Home Mode :0 : Master, Slave Both home, 1 : Both home + Retry, 2 : Offset find
Retry Move Range (Mode 1 에서만 사용, Retry 시 구동 Pos.)
Second Vel. (Mode 1 에서만 사용, Retry 시 구동 Vel)
Second Creep Vel. (Mode 1 에서만 사용, Retry 시 구동 CreepVel)
);	//_in Second Servo Amp Off Delay Time (1000ms 이상 설정을 권장)
               
1. Home Mode 2번을 사용하여 Master, Slave Home Sensor 의 Offset 값을 찾는다.
- Offset Find 를 진행 할때는 기구물 평행을 맞춰 놓은 상태에서 진행 해야 한다. (해당 위치가 차후 기준이 된다)
- Offset Find 동작은 Setup 당시 혹은 H/W 적인 변경 및 틀어짐이 (Or HomeSensor 위치 변경) 발생 했을때만 진행 한다.
(Master, Slave 의 틀어짐에 따라 Offset 값은 다르게 측정 되므로 셋업시 찾은 Offset 값을 그대로 사용해야 홈서치시 동일한 위치로 기구물이 위치하게된다)
- Offset 값은 Offset Find 홈동작 완료 후 Master 축의 Command 값에 부호를 반전하여 사용한다.
2. Offset find Mode 로 찾은 Offset 값을 적용하여 1 Or 2 Mode 로 홈서치를 진행한다.
- Master, Slave 축에서 사용되는 홈서치 방법 및 홈속도 값은 기본적으로 각축의 Single Home 설정 파라미터를 그대로 사용한다. 
- 따라서 Master, Slave 양축 모두 홈서치 관련 파라미터 설정은 사전에 진행 되어야 한다. */
APPMOTIONAPI MC_STATUS __stdcall AMC_GearHome(UINT16 BoardID2,
	UINT16 MasterAxisID,
	UINT16 SlaveAxisID,
	double Offset,
	double OffsetRange,
	UINT32 HomeMode,
	double RetryMoveRange,
	double SecondVel,
	double SecondCreepVel,
	UINT16 SecondAmpOffDelay);

/*GearHome 상태를 반환 한다.
Status : false : 홈동작 진행 중이지 않음, true : Home 동작 진행 중. */
APPMOTIONAPI MC_STATUS __stdcall AMC_ReadGearHoming(UINT16 BoardID, UINT16 AxisID,	bool*  Status);			
 
/*GearHome 동작 결과를 반환 한다.
HomeResult : Home 동작 결과 값.
StepStatus : 진행 Home Step 값 */ 
APPMOTIONAPI MC_STATUS __stdcall AMC_ReadGearHomedResult(UINT16 BoardID, UINT16 AxisID, UINT32 *HomeResult, UINT32 *StepStatus);	

#ifdef __cplusplus
}
#endif	

#endif
