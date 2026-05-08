#ifndef SR7LINK__H
#define SR7LINK__H

#include <stdio.h>

#ifdef WIN32
#define  SR7_IF_API __declspec(dllexport)
#else
#define  SR7_IF_API extern
#endif

typedef void * SR7IF_Data;
typedef void * SR7IF_UserData;


typedef void (*SR7IF_BatchOneTimeCallBack)(const void *info, const SR7IF_Data *data);
typedef void (*TcpConnectFunc)(int dwDeviceId, int cmd);

typedef struct {
    unsigned char	abyIpAddress[4];
} SR7IF_ETHERNET_CONFIG;

#define SR7IF_ERROR_NOT_FOUND                     (-999)  // Function (camera) does not exist.
#define SR7IF_ERROR_COMMAND                       (-998)  // This command is not supported.
#define SR7IF_ERROR_PARAMETER                     (-997)  // Parameter error.
#define SR7IF_ERROR_UNIMPLEMENTED                 (-996)  // Function not implemented.
#define SR7IF_ERROR_HANDLE                        (-995)  // Invalid handle.
#define SR7IF_ERROR_MEMORY                        (-994)  // Memory (overflow/definition) error.
#define SR7IF_ERROR_TIMEOUT                       (-993)  // Operation timeout.
#define SR7IF_ERROR_DATABUFFER                    (-992)  // Data buffer not sufficient.
#define SR7IF_ERROR_STREAM                        (-991)  // Data stream error.
#define SR7IF_ERROR_CLOSED                        (-990)  // Interface closed and unavailable.
#define SR7IF_ERROR_VERSION                       (-989)  // Current version invalid.
#define SR7IF_ERROR_ABORT                         (-988)  // Operation aborted, such as batch process software termination, connection closed, connection interrupted, etc.
#define SR7IF_ERROR_ALREADY_EXISTS                (-987)  // Operation conflicts with existing settings.
#define SR7IF_ERROR_FRAME_LOSS                    (-986)  // Batch frame loss.
#define SR7IF_ERROR_ROLL_DATA_OVERFLOW            (-985)  // Overflow exception in unending loop batch processing, etc.
#define SR7IF_ERROR_ROLL_BUSY                     (-984)  // Busy reading data in unending loop batch processing.
#define SR7IF_ERROR_MODE                          (-983)  // Current processing function conflicts with the set batch mode.
#define SR7IF_ERROR_CAMERA_NOT_ONLINE             (-982)  // Camera (sensor head) not online.
#define SR7IF_ERROR                               (-1)    // General error, such as link failure, setup failure, data acquisition failure, etc.
#define SR7IF_NORMAL_STOP                         (-100)  // Normal stop, such as stopping batch processing operation by external IO, etc.
#define SR7IF_OK                                  (0)     // Correct operation.

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief SR7IF_EthernetOpen   Communication connection.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pEthernetConfig      Ethernet communication settings.
/// \return
///     <0:                     Failure.
///     =0:                     Success.

SR7_IF_API int SR7IF_EthernetOpen(unsigned int lDeviceId, SR7IF_ETHERNET_CONFIG* pEthernetConfig);

/// \brief SR7IF_EthernetOpenEx Communication connection Ext.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pEthernetConfig      Ethernet communication settings.
/// \param timeOut              Search time (ms), minimum value 100.
/// \param fun                  Disconnection callback function.
/// \return
///     <0:                     Failure.
///     =0:                     Success.

SR7_IF_API int SR7IF_EthernetOpenExt(unsigned int lDeviceId, SR7IF_ETHERNET_CONFIG* pEthernetConfig, int timeOut = 2000, TcpConnectFunc fun = NULL);

///
/// \brief SR7IF_CommClose Disconnects from the camera.
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_CommClose(unsigned int lDeviceId);

///
/// \brief SR7IF_SwitchProgram Switches the camera configuration parameters. Recipe number is not saved after reboot.
/// \param lDeviceId            Device ID, range 0-63.
/// \param No:                  Task parameter list number 0 - 63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_SwitchProgram(unsigned int lDeviceId, int No);

///
/// \brief SR7IF_GetOnlineCameraB Checks if Sensor Head B is online.
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     -982: Sensor Head B is not online
///                             Other: Failure.
///     =0:                     Sensor Head B is online.
/// 
SR7_IF_API int SR7IF_GetOnlineCameraB(unsigned int lDeviceId);


///
/// \brief SR7IF_StartMeasure Starts batch processing, immediately executes batch processing program.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Timeout              Timeout duration (ms) in non-looping mode, -1 for indefinite wait; can be set to -1 in loop mode.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_StartMeasure(unsigned int lDeviceId, int Timeout = 50000);


/// \brief SR7IF_StartIOTriggerMeasure Starts batch processing, hardware IO triggered start, see hardware manual for details.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Timeout              Timeout duration (ms) in non-looping mode, -1 for indefinite wait; can be set to -1 in loop mode.
/// \param restart              Reserved, set to 0.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_StartIOTriggerMeasure(unsigned int lDeviceId, int Timeout = 50000, int restart = 0);

///
/// \brief SR7IF_StopMeasure Stops batch processing.
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_StopMeasure(unsigned int lDeviceId);

/// \brief SR7IF_ReceiveData Blocking mode to retrieve data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Pointer to the returned data.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_ReceiveData(unsigned int lDeviceId, SR7IF_Data DataObj);


/// \brief SR7IF_ProfilePointSetCount Retrieves the set number of rows for current batch processing.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \return                     Returns the set number of batch processing rows.
/// 
SR7_IF_API int SR7IF_ProfilePointSetCount(unsigned int lDeviceId, const SR7IF_Data DataObj);

/// \brief SR7IF_ProfilePointCount Retrieves the actual number of rows obtained in batch processing.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \return                     Returns the actual number of batch processing rows.
/// 
SR7_IF_API int SR7IF_ProfilePointCount(unsigned int lDeviceId, const SR7IF_Data DataObj);

/// \brief SR7IF_ProfileDataWidth Retrieves data width.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \return                     Returns data width (in pixels).
/// 
SR7_IF_API int SR7IF_ProfileDataWidth(unsigned int lDeviceId, const SR7IF_Data DataObj);

///
/// \brief SR7IF_ProfileData_XPitch Retrieves the pitch of data in the x-direction.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \return                     Returns data pitch in the x-direction (mm).
/// 
SR7_IF_API double SR7IF_ProfileData_XPitch(unsigned int lDeviceId, const SR7IF_Data DataObj);

///
/// \brief SR7IF_GetEncoder Retrieves encoder values.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Encoder              Pointer to the returned data, alternates A/B for dual cameras.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
/// 
SR7_IF_API int SR7IF_GetEncoder(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *Encoder);

///
/// \brief SR7IF_GetBatchEncoder64Bit Retrieves 64-bit encoder values.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Encoder              Pointer to the returned data.
/// \return
///     <0:                     Retrieval failure.
///     >=0:                    Returns the number of encoders.
///
SR7_IF_API int SR7IF_GetBatchEncoder64Bit(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned long long *Encoder);


///
/// \brief SR7IF_GetEncoderContiune Non-blocking retrieval of encoder values.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Encoder              Pointer to the returned data, alternates A/B for dual cameras.
/// \param GetCnt               Length of data to retrieve.
/// \return
///     <0:                     Retrieval failure.
///     >=0:                    Actual length of returned data.
///
SR7_IF_API int SR7IF_GetEncoderContiune(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *Encoder, unsigned int GetCnt);


///
/// \brief SR7IF_GetProfileData Blocking retrieval of profile data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Profile              Pointer to the returned data, alternates A/B rows for dual cameras.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetProfileData(unsigned int lDeviceId, const SR7IF_Data DataObj, int *Profile);

///
/// \brief SR7IF_GetLaserWidthData  Retrieves laser width data (only supports SRI devices).
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param widthData            Pointer to the returned data, alternates A/B rows for dual cameras.
/// \param GetCnt               Length of data to retrieve.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetLaserWidthData(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned char *widthData, int GetCnt = 0);

///
/// \brief SR7IF_GetProfileContiuneData Non-blocking retrieval of profile data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Profile              Pointer to the returned data, alternates A/B rows for dual cameras.
/// \param GetCnt               Length of data to retrieve.
/// \return
///     <0:                     Retrieval failure.
///     >=0:                    Actual length of returned data.
///
SR7_IF_API int SR7IF_GetProfileContiuneData(unsigned int lDeviceId, const SR7IF_Data DataObj, int *Profile, unsigned int GetCnt);



///
/// \brief SR7IF_GetIntensityData  Blocking retrieval of intensity data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Intensity            Pointer to the returned data, alternates A/B rows for dual cameras.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetIntensityData(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned char *Intensity);


///
/// \brief SR7IF_GetIntensityContiuneData Non-blocking retrieval of intensity data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Intensity            Pointer to the returned data, alternates A/B rows for dual cameras.
/// \param GetCnt               Length of data to retrieve.
/// \return
///     <0:                     Retrieval failure.
///     >=0:                    Returns the actual number of retrieved data rows.
///
SR7_IF_API int SR7IF_GetIntensityContiuneData(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned char *Intensity, unsigned int GetCnt);

///
/// \brief SR7IF_GetBatchRollData No-ending loop to get data
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Profile              Pointer to the returned profile data, alternates A/B rows for dual cameras.
/// \param Intensity            Pointer to the returned intensity data, alternates A/B rows for dual cameras.
/// \param Encoder              Pointer to the returned encoder data, alternates A/B for dual cameras.
/// \param FrameId              Pointer to the returned frame ID data.
/// \param FrameLoss            Pointer to the returned frame loss data due to batch processing being too fast, alternates A/B for dual cameras.
/// \param GetCnt               Length of data to retrieve.
/// \return
///     <0:                     Retrieval failure.
///     >=0:                    Actual length of returned data.
///
SR7_IF_API int SR7IF_GetBatchRollData(unsigned int lDeviceId, const SR7IF_Data DataObj,
                                        int *Profile, unsigned char *Intensity, unsigned int *Encoder, long long *FrameId, unsigned int *FrameLoss,
                                        unsigned int GetCnt);


///
/// \brief SR7IF_SetBatchRollProfilePoint Sets the number of rows for unending loops.
///                               Used to set between 50-65535 rows when high data speed causes transmission speed issues leading to overlap.
///                               This function must be initialized at least once and is not saved after power-off.
/// \param lDeviceId              Device ID, range 0-63.
/// \param DataObj                Reserved, set to NULL.
/// \param points                 0: unending loop  >=50: set number of rows  Others: invalid
/// \return
///     <0:                       Failure.
///     >=0:                      Success.
///
SR7_IF_API int SR7IF_SetBatchRollProfilePoint(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int points);



///
/// \brief SR7IF_GetBatchRollError   Retrieves error count for unending loop data acquisition.
/// \param lDeviceId            Device ID, range 0-63.
/// \param EthErrCnt            Number of errors due to network transmission.
/// \param UserErrCnt           Number of errors due to user retrieval.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetBatchRollError(unsigned int lDeviceId, int *EthErrCnt, int *UserErrCnt);


///
/// \brief SR7IF_RollDataCallback 				SR7IF_RollDataCallbackInitalize Number of rows for unending loop callback.
/// \param pProfileBuffer       Pointer to the returned profile data.
/// \param pIntensityBuffer     Pointer to the returned intensity data.
/// \param pEncoder             Pointer to the returned encoder data.
/// \param dwSize               Data width.
/// \param dwCount              Number of batch points.
/// \param dwRet                Return value, for future use.
/// \param dwDeviceId           Corresponding controller (0-63).
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
typedef void (*SR7IF_RollDataCallback)(int* pProfileBuffer, unsigned char* pIntensityBuffer, unsigned int* pEncoder,
                                       unsigned int dwSize, unsigned int dwCount, int dwRet, unsigned int dwDeviceId);

///
/// \brief SR7IF_RollDataCallbackInitalize Callback method for unending loops.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pCallBack            Callback function.
/// \param dwProfileCnt         Number of rows per callback.
/// \param pCallBack            Timeout for each callback row retrieval, in ms   <0: disable timeout.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_RollDataCallbackInitalize(unsigned int lDeviceId, SR7IF_RollDataCallback pCallBack, unsigned int dwProfileCnt, int Timeout);



///
/// \brief SR7IF_SetBatchCtrlByIO Enables resume functionality.
///                                 Sets the camera to pause or continue batch processing via IO control during a batch process.
///                                 IO control uses controller pins 11 and 14, where pin 11 enables level control mode, and pin 14 controls the pause and continue of batch processing.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param Enable               Feature enable 0: disable  1: enable.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetBatchCtrlByIO(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int Enable);

///
/// \brief SR7IF_GetError       Retrieves system error information.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pbyErrCnt            Pointer to return the number of error codes.
/// \param pwErrCode            Pointer to return the error codes, array size recommended to be 2048.
/// \return
///     <0:                     Retrieval failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetError(unsigned int lDeviceId, int *pbyErrCnt, int *pwErrCode);

///
/// \brief SR7IF_ClearError     Not available yet.
/// \param lDeviceId
/// \param wErrCode
/// \return
///     <0:                     Clear failed.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_ClearError(unsigned int lDeviceId, unsigned short wErrCode);

///
/// \brief SR7IF_GetVersion     Retrieves library version number.
/// \return                     Returns version information.
///
SR7_IF_API const char *SR7IF_GetVersion();

///
/// \brief SR7IF_GetModels      Retrieves camera model.
/// \param lDeviceId            Device ID, range 0-63.
/// \return                     Returns camera model string.
///
SR7_IF_API const char *SR7IF_GetModels(unsigned int lDeviceId);

///
/// \brief SR7IF_GetHeaderSerial   Retrieves camera head serial number.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Head                 0: Camera head A  1: Camera head B
/// \return
///     !=NULL:                 Returns camera serial number string.
///     =NULL:                  Failure, corresponding head does not exist or parameter error.
///
SR7_IF_API const char *SR7IF_GetHeaderSerial(unsigned int lDeviceId, int Head);



///
/// \brief SR7IF_SetOutputPortLevel      Sets the output port level.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Port                 Output port number, range 0-7.
/// \param Level                Output level value.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetOutputPortLevel(unsigned int lDeviceId, unsigned int Port, bool Level);


///
/// \brief SR7IF_SetInputPortLevel      Reads the input port level.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Port                 Input port number, range 0-7.
/// \param Level                Reads the input level.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetInputPortLevel(unsigned int lDeviceId, unsigned int Port, bool *Level);

///
/// \brief SR7IF_GetSingleProfile   /// Retrieves the current profile (non-batch mode, must be set to 2.5D mode in EdgeImaging).
/// \param lDeviceId            Device ID, range 0-63.
/// \param pProfileData         Pointer to the returned profile.
/// \param pEncoder             Pointer to the returned encoder.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetSingleProfile(unsigned int lDeviceId, int *pProfileData, unsigned int *pEncoder);

///
/// \brief SR7IF_SetSetting     Parameter setting.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Depth                Level of the set value.
/// \param Type                 Setting type. -1: Set current recipe parameters  0x10-0x50: Set recipe 0-63 parameters
/// \param Category             Setting category.
/// \param Item                 Setting item.
/// \param Target[4]            Specify as needed based on send/receive settings. Set to 0 if not needed.
/// \param pData                Setting data.
/// \param DataSize             Length of the setting data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetSetting(unsigned int lDeviceId, int Depth, int Type, int Category, int Item, int Target[4], void *pData, int DataSize);


///
/// Retrieves parameter settings. Acquiring parameters for a recipe other than the currently running one may interrupt batch processing.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Type                 Retrieval type. -1: Retrieve current recipe parameters  0x10-0x50: Retrieve recipe 0-63 parameters
/// \param Category             Retrieval category.
/// \param Item                 Retrieval item.
/// \param Target[4]            Specify as needed based on send/receive settings. Set to 0 if not needed.
/// \param pData                Retrieved data.
/// \param DataSize             Length of the retrieved data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetSetting(unsigned int lDeviceId, int Type, int Category, int Item, int Target[4], void *pData, int DataSize);
///
/// \brief SR7IF_ExportParameters   Exports system parameters, note that only current task parameters are exported.
/// \param lDeviceId            Device ID, range 0-63.
/// \param size                 Returns the size of the parameter table.
/// \return
///     NULL:                   Failure.
///     Others:                 Success.
///
SR7_IF_API const char *SR7IF_ExportParameters(unsigned int lDeviceId, unsigned int *size);

///
/// \brief SR7IF_LoadParameters   Imports exported parameters into the system.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pSettingdata         Pointer to the imported parameter table.
/// \param size                 Size of the imported parameter table.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_LoadParameters(unsigned int lDeviceId, const char *pSettingdata, unsigned int size);

///
/// \brief SR7IF_GetLicenseKey   Returns the remaining days of the product.
/// \param lDeviceId            Device ID, range 0-63.
/// \param RemainDay            Returns the remaining days.
/// \return
///     < 0:                     Failure, parameter error or product not registered.
///     >=0:                     Success.
///
SR7_IF_API int SR7IF_GetLicenseKey(unsigned int lDeviceId, unsigned short *RemainDay);

///
/// \brief SR7IF_GetCurrentEncoder   Reads the current encoder value.
/// \param value                    Returns the encoder value.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetCurrentEncoder(unsigned int lDeviceId, unsigned int *value);

///
/// \brief SR7IF_GetCurrentEncoder64Bit   Reads the current 64-bit encoder value.
/// \param value                    Returns the encoder value.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetCurrentEncoder64Bit(unsigned int lDeviceId, unsigned long long *value);


///
/// \brief SR7IF_GetCameraTemperature   Reads the camera temperature, unit 0.01 degrees Celsius.
/// \param lDeviceId            Device ID, range 0-63.
/// \param tempA                Camera A temperature value, -1000000: temperature reading not supported.
/// \param tempB                Camera B temperature value, -1000000: temperature reading not supported.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetCameraTemperature(unsigned int lDeviceId, int *tempA, int *tempB);

///
/// \brief SR7IF_GetCameraBoardTemperature   Reads the camera board temperature, unit 0.125 degrees Celsius.
/// \param lDeviceId            Device ID, range 0-63.
/// \param tempA                Camera A temperature value, -1000000: temperature reading not supported.
/// \param tempB                Camera B temperature value, -1000000: temperature reading not supported.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetCameraBoardTemperature(unsigned int lDeviceId, int *tempA, int *tempB);

//
typedef struct {
	int xPoints;                // Number of data points in x direction
	int BatchPoints;            // Number of batches
	unsigned int BatchTimes;    // Number of batch processes
	
	double xPixth;              // Distance between points in the x direction
	unsigned int startEncoder;  // Start encoder value for batch processing
	int HeadNumber;             // Number of camera heads
	int returnStatus;           // SR7IF_OK: Normal batch processing
                                //SR7IF_NORMAL_STOP
                                //SR7IF_ERROR_ABORT
                                //SR7IF_ERROR_CLOSED
} SR7IF_STR_CALLBACK_INFO;
///
/// \brief SR7IF_SetBatchOneTimeDataHandler   Sets the callback function. It is recommended to start a new thread to process data after retrieval (Data acquisition mode: callback after each batch).
/// \param lDeviceId            Device ID, range 0-63.
/// \param CallFunc             Callback function.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetBatchOneTimeDataHandler(unsigned int lDeviceId, SR7IF_BatchOneTimeCallBack CallFunc);

///
/// \brief SR7IF_SetBatchOneTimeDataValidRange   Sets the valid data range (Data acquisition mode: callback after each batch).
/// \param lDeviceId            Device ID, range 0-63.
/// \param Left                 Number of points to crop on the left.
/// \param Right                Number of points to crop on the right.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetBatchOneTimeDataValidRange(unsigned int lDeviceId, unsigned int Left, unsigned int Right);

///
/// \brief SR7IF_StartMeasureWithCallback   Starts batch processing (Data acquisition mode: callback after each batch).
/// \param lDeviceId            Device ID, range 0-63.
/// \param ImmediateBatch       0: Start batch processing immediately, non-zero: wait for external batch processing to start.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_StartMeasureWithCallback(int iDeviceId, int ImmediateBatch);

///
/// \brief SR7IF_TriggerOneBatch   Software triggers batch processing start (Data acquisition mode: callback after each batch).
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_TriggerOneBatch(int iDeviceId);

///
/// \brief SR7IF_GetBatchProfilePoint   Retrieves batch profile (Data acquisition mode: callback after each batch).
/// \param DataIndex            Parameter data transfer.
/// \param Head                 0: Camera head A  1: Camera head B
/// \return
///     !=NULL:                 Returns data pointer.
///     =NULL:                  Failure, no data or corresponding head does not exist.
///
SR7_IF_API const int *SR7IF_GetBatchProfilePoint(const SR7IF_Data *DataIndex, int Head);

///
/// \brief SR7IF_GetBatchIntensityPoint   Retrieves batch intensity (Data acquisition mode: callback after each batch).
/// \param DataIndex            Parameter data transfer.
/// \param Head                 0: Camera head A  1: Camera head B.
/// \return
///     !=NULL:                 Returns data pointer.
///     =NULL:                  Failure, no data or corresponding head does not exist.
///
SR7_IF_API const unsigned char *SR7IF_GetBatchIntensityPoint(const SR7IF_Data *DataIndex, int Head);

///
/// \brief SR7IF_GetBatchIntensityPoint   Retrieves batch intensity (Data acquisition mode: callback after each batch).
/// \param DataIndex            Parameter data transfer.
/// \param Head                 0: Camera head A  1: Camera head B
/// \return
///     !=NULL:                 Returns data pointer.
///     =NULL:                  Failure, no data or corresponding head does not exist.
///
SR7_IF_API const unsigned int *SR7IF_GetBatchEncoderPoint(const SR7IF_Data *DataIndex, int Head);

///
/// \brief SR7LinkSearchOnline   Camera online search. May fail to search when used with EdgeImaging, other programs calling this interface, or being blocked by a firewall.
///
/// \param ReadNum              Number of online cameras found.
/// \param timeOut              Search time (ms), minimum value 500.
/// \return
///     !=NULL:                 Returns pointer to IP.
///     =NULL:                  Failure. Parameter error or no online camera.
///
SR7_IF_API SR7IF_ETHERNET_CONFIG *SR7IF_SearchOnline(int *ReadNum, int timeOut);

///
/// \brief SR7IF_SetMultiEncoderInterval   Sets multiple encoder subdivisions. Subdivisions set in EdgeImaging are the starting batch points.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param enable               0: stop multiple encoder subdivisions, 1: enable multiple encoder subdivisions.
/// \param Point[1-8]           Starting points for batch processing by the encoder, range [0-15000], 0: point not effective. Data must be written in ascending order (e.g., Point1 < Point2...).
/// \param Interval[1-8]        Subdivision of the encoder corresponding to the point, range [1-10000].
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_SetMultiEncoderInterval(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int enable,
                                  unsigned short Point1, unsigned short Interval1,
                                  unsigned short Point2, unsigned short Interval2,
                                  unsigned short Point3, unsigned short Interval3,
                                  unsigned short Point4, unsigned short Interval4,
                                  unsigned short Point5, unsigned short Interval5,
                                  unsigned short Point6, unsigned short Interval6,
                                  unsigned short Point7, unsigned short Interval7,
                                  unsigned short Point8, unsigned short Interval8);


///
/// \brief SR7IF_GetTimeStamp   Gets the controller's runtime since the last power-on.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param TimeStamp            Returns the runtime (in seconds).
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetTimeStamp(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *TimeStamp);

///
/// Camera batch processing IO trigger counter, counting starts from power-on.
/// \param lDeviceId            Device ID, range 0-63.
/// \param DataObj              Reserved, set to NULL.
/// \param TriggerCount         Returns the number of triggers.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetStartIOTriggerCount(unsigned int lDeviceId, const SR7IF_Data DataObj, unsigned int *pTriggerCount);

///
/// \brief SR7IF_GetBatchStatus   Batch processing status query.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Status               Batch processing status 1: Processing 0: Processing ended.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetBatchStatus(unsigned int lDeviceId, int * Status);

///
/// \brief SR7IF_GetActivityProgramNo   Retrieves the current recipe number.
/// \param lDeviceId            Device ID, range 0-63.
/// \param ProgramNo            Recipe number, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetActivityProgramNo(unsigned int lDeviceId, int *ProgramNo);

///
/// \brief SR7IF_SetNetworkParam  Sets the controller network parameters.
/// \param lDeviceId              Device ID, range 0-63.
/// \param ip                     IP address.
/// \param netmask                Subnet mask.
/// \param gateway                Default gateway.
/// \return
///     <0:                       Failure.
///     =0:                       Success.
///
SR7_IF_API int SR7IF_SetNetworkParam(unsigned int lDeviceId, const char *ip, const char *netmask, const char *gateway);


///
/// \brief SR7IF_Get16BitScale Retrieves the physical unit of 16-bit height data.
/// \param lDeviceId            Device ID, range 0-63.
/// \param Scale                Unit (mm).
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_Get16BitScale(unsigned int lDeviceId, float *Scale);

///////////////////////////////////High-speed callback////////////////////////////////

/// \brief                      High-speed data communication callback function interface.
/// \param data                 Pointer to the buffer storing summary data.
/// \param ProfileCompressType  Compression type 0: 32bit, 1: 16bit
/// \param xPoints              Number of points in one profile line.
/// \param dwCount              Current callback return rows.
/// \param dwNotify             Callback status 0: in callback, 1: callback ended.
/// \param dwDeviceId           Device ID of the called function.
///
typedef void(*SR7IF_ProfileCALLBACK)(const SR7IF_Data data, unsigned char ProfileCompressType, int xPoints, int dwCount, unsigned int dwNotify, unsigned int dwDeviceId);

///
/// \brief SR7IF_HighSpeedDataCallBackInitalize Initializes Ethernet high-speed data communication.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pEthernetConfig      Ethernet communication settings.
/// \param pCallBack            Callback function.
/// \param dwProfileCnt         Frequency of limited loop callback function calls. Range 1-15000
///                             Frequency of unlimited loop callback function calls. Applicable range 1-7500
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_HighSpeedDataCallBackInitalize(unsigned int lDeviceId, const char* pEthernetConfig, SR7IF_ProfileCALLBACK pCallBack, unsigned int dwProfileCnt);

///
/// \brief SR7IF_StartMeasureWithHighSpeedCallback   Starts batch processing with high-speed callback.
/// \param lDeviceId            Device ID, range 0-63.
/// \param ImmediateBatch       0: Start batch processing immediately, non-zero: wait for external batch processing to start.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_StartMeasureWithHighSpeedCallback(unsigned int lDeviceId, unsigned int ImmediateBatch);

///
/// \brief SR7IF_StopMeasureWithHighSpeedCallback   Stops batch processing with high-speed callback.
/// \param lDeviceId            Device ID, range 0-63.
/// \param ImmediateBatch       0: Complete data transmission, 1: Stop immediately, discard remaining untransmitted data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_StopMeasureWithHighSpeedCallback(unsigned int lDeviceId, unsigned int instantStop);

///
/// \brief SR7IF_GetProfilePointData32bit Retrieves 32-bit height data with high-speed callback.
/// \param DataObj              Callback data pointer.
/// \param head                 0: Camera A  1: Camera B
/// \param Profile              Pointer to the received data, single data point size is 4 bytes.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetProfilePointData32bit(const SR7IF_Data DataObj, unsigned int head, int *Profile);

///
/// \brief SR7IF_GetProfilePointData16bit Retrieves 16-bit height data with high-speed callback.
/// \param DataObj              Callback data pointer.
/// \param head                 0: Camera A  1: Camera B
/// \param Profile              Pointer to the received data, single data point size is 2 bytes.
/// \param Scale                Compression ratio.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetProfilePointData16bit(const SR7IF_Data DataObj, unsigned int head, short *Profile, double *Scale);

///
/// \brief SR7IF_GetHighSpeedIntensityData Retrieves intensity data with high-speed callback.
/// \param DataObj              Callback data pointer.
/// \param head                 0: Camera A  1: Camera B.
/// \param Intensity            Pointer to the received data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetHighSpeedIntensityData(const SR7IF_Data DataObj, unsigned int head, unsigned char *Intensity);

///
/// \brief SR7IF_GetGrayData16Bit Retrieves 16-bit grayscale image with high-speed callback.
/// \param DataObj              Callback data pointer.
/// \param head                 0: Camera A  1: Camera B.
/// \param Profile              Pointer to the received data, single data point size is 2 bytes.
/// \param Scale                Compression ratio.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetGrayData16Bit(const SR7IF_Data DataObj, unsigned int head, unsigned short *Profile, double *Scale);

///
/// \brief SR7IF_GetHighSpeedEncoderContiune Retrieves encoder data with high-speed callback.
/// \param DataObj              Callback data pointer.
/// \param pEncoder             Pointer to the received data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_GetHighSpeedEncoderContiune(const SR7IF_Data DataObj, unsigned int *pEncoder);


//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Asynchronous callbacks////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
typedef void(*SR7IF_AsyncErrCallBack)(int dwDeviceId, unsigned int *errCode);

/// \brief SR7IF_AsyncEthernetOpen Asynchronous callback connection function.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pEthernetConfig      Ethernet communication settings.
/// \param timeOut              Search time (ms), minimum value 100.
/// \param fun                  Disconnection callback function.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_AsyncEthernetOpen(unsigned int lDeviceId, SR7IF_ETHERNET_CONFIG* pEthernetConfig, int timeOut = 2000,  SR7IF_AsyncErrCallBack errCallBackFunc = NULL);

///
/// \brief SR7IF_AsyncCALLBACK Asynchronous callback function
/// \param info                Reserved.
/// \param SR7IF_Data          Callback related information.
/// \param pUserData           Pointer to user-registered data.
/// \param state               Callback status.
///                            state:(0x01 << 0) : Batch processing ended normally.
///                            state:(0x01 << 1) : Batch processing software overflow.
///                            state:(0x01 << 2) : Batch processing hardware overflow.
///                            state:(0x01 << 3) : Network exception, reception disconnected.
///                            state:(0x01 << 4) : Other errors.
/// \param dwDeviceId          Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
typedef void(*SR7IF_AsyncCALLBACK)(const SR7IF_Data data, SR7IF_UserData pUserData, unsigned int state, unsigned int dwDeviceId);
///
/// \brief SR7IF_AsyncCallBackInitalize   Initializes asynchronous callback data communication.
/// \param lDeviceId            Device ID, range 0-63.
/// \param pCallBack            Callback function.
/// \param pUserData            Pointer to user-registered data.
/// \param ProfileBits          0: Return 32-bit height data. 1: Return 16-bit height data.
/// \param mMaxLine             Specifies the number of lines to return data.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_AsyncCallBackInitalize(unsigned int lDeviceId, SR7IF_AsyncCALLBACK pCallBack, SR7IF_UserData pUserData = NULL, unsigned int ProfileBits = 0, unsigned int mMaxLine = 0xFFFFFFFF);
///
/// \brief SR7IF_AsyncSoftStartBatch   Asynchronous callback soft trigger.
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_AsyncSoftStartBatch(unsigned int lDeviceId);
///
/// \brief SR7IF_AsyncStopStartBatch   Stops batch processing with asynchronous callback.
/// \param lDeviceId            Device ID, range 0-63.
/// \return
///     <0:                     Failure.
///     =0:                     Success.
///
SR7_IF_API int SR7IF_AsyncSoftStopBatch(unsigned int lDeviceId);
///
/// \brief SR7IF_GetAsyncProfilePointData Retrieves 32-bit height data with asynchronous callback.
/// \param DataObj              Returns data pointer.
/// \param head                 0: Camera A  1: Camera B.
/// \return                     Data pointer.
///
SR7_IF_API const int* SR7IF_GetAsyncProfilePointData(const SR7IF_Data DataObj, unsigned int head);
///
/// \brief SR7IF_GetAsyncProfilePointData16Bit Retrieves 16-bit height data with asynchronous callback.
/// \param DataObj              Returns data pointer.
/// \param head                 0: Camera A  1: Camera B.
/// \return                     Data pointer.
///
SR7_IF_API const short* SR7IF_GetAsyncProfilePointData16Bit(const SR7IF_Data DataObj, unsigned int head);
///
/// \brief SR7IF_GetAsyncIntensityContiuneData Retrieves intensity data with asynchronous callback.
/// \param DataObj              Returns data pointer.
/// \param head                 0: Camera A  : Camera B.
/// \return                     Data pointer.
///
SR7_IF_API const unsigned char* SR7IF_GetAsyncIntensityContiuneData(const SR7IF_Data DataObj, unsigned int head);
///
/// \brief SR7IF_GetAsyncEncoderContiune Retrieves encoder data with asynchronous callback.
/// \param DataObj              Returns data pointer.
/// \param head                 0: Camera A  1: Camera B.
/// \return                     Data pointer.
///
SR7_IF_API const unsigned int* SR7IF_GetAsyncEncoderContiune(const SR7IF_Data DataObj, unsigned int head);

///
/// \brief SR7IF_GetMeasuringRangeZ Retrieves the upper and lower limits of the z-direction measurement range.
/// \param lDeviceId            Device ID, range 0-63.
/// \param up                   Upper limit (mm).
/// \param down                 Lower limit (mm).
///
SR7_IF_API int SR7IF_GetMeasuringRangeZ(unsigned int lDeviceId, double *up, double *down);

#ifdef __cplusplus
}
#endif
#endif //SR7LINK__H

