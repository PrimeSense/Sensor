/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.0 Alpha                                               *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Common.                                   *
*                                                                            *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  PrimeSense Sensor is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>. *
*                                                                            *
*****************************************************************************/






#ifndef _XN_DEVICESENSOR_H_
#define _XN_DEVICESENSOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#ifndef XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	#undef XN_DEVICE_EXPORT_PREFIX
	#define XN_DEVICE_EXPORT_PREFIX SensorV2_
#endif

#include <XnDevice.h>
#include <XnDDK.h>
#include <XnDDK/XnDeviceBaseProxy.h>
#include "XnDeviceSensorIO.h"
#include <XnFPSCalculator.h>
#include <XnLog.h>
#include <XnScheduler.h>
#include <XnCore/XnBuffer.h>
#include <XnDDK/XnFrameBufferManager.h>
#include "XnSensorFPS.h"
#include "XnFirmwareInfo.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEVICE_MAJORVERSION 1
#define XN_DEVICE_MINORVERSION 0
#define XN_DEVICE_NAME "SensorV2"
#define XN_DEVICE_DESCRIPTION "Xiron I/O Prime Sensor v2/v3/v4 Device"

#define XN_DEVICE_SENSOR_THREAD_KILL_TIMEOUT 5000

#define XN_DEVICE_SENSOR_DEPTH_CMOS_XRES 1280
#define XN_DEVICE_SENSOR_DEPTH_CMOS_YRES 1024

#define XN_DEVICE_SENSOR_QVGA_DEPTH_XRES 320
#define XN_DEVICE_SENSOR_QVGA_DEPTH_YRES 240
#define XN_DEVICE_SENSOR_QVGA_DEPTH_YRES_COMPATIBLE 256
#define XN_DEVICE_SENSOR_QVGA_IMAGE_XRES XN_DEVICE_SENSOR_QVGA_DEPTH_XRES
#define XN_DEVICE_SENSOR_QVGA_IMAGE_YRES XN_DEVICE_SENSOR_QVGA_DEPTH_YRES
#define XN_DEVICE_SENSOR_QVGA_IMAGE_YRES_COMPATIBLE XN_DEVICE_SENSOR_QVGA_DEPTH_YRES_COMPATIBLE

#define XN_DEVICE_SENSOR_VGA_DEPTH_XRES 640
#define XN_DEVICE_SENSOR_VGA_DEPTH_YRES 480
#define XN_DEVICE_SENSOR_VGA_DEPTH_YRES_COMPATIBLE 512
#define XN_DEVICE_SENSOR_VGA_IMAGE_XRES XN_DEVICE_SENSOR_VGA_DEPTH_XRES
#define XN_DEVICE_SENSOR_VGA_IMAGE_YRES XN_DEVICE_SENSOR_VGA_DEPTH_YRES
#define XN_DEVICE_SENSOR_VGA_IMAGE_YRES_COMPATIBLE XN_DEVICE_SENSOR_VGA_DEPTH_YRES_COMPATIBLE

#define XN_DEVICE_SENSOR_SXGA_IMAGE_XRES 1280
#define XN_DEVICE_SENSOR_SXGA_IMAGE_YRES 1024

#define XN_DEVICE_SENSOR_UXGA_IMAGE_XRES 1600
#define XN_DEVICE_SENSOR_UXGA_IMAGE_YRES 1200
/*
#define XN_DEVICE_SENSOR_HD720_IMAGE_XRES 1280
#define XN_DEVICE_SENSOR_HD720_IMAGE_YRES 720

#define XN_DEVICE_SENSOR_2MPIX_IMAGE_XRES 1600
#define XN_DEVICE_SENSOR_2MPIX_IMAGE_YRES 1200
*/
#define XN_DEVICE_SENSOR_MAX_IMAGE_XRES XN_DEVICE_SENSOR_SXGA_IMAGE_XRES
#define XN_DEVICE_SENSOR_MAX_IMAGE_YRES XN_DEVICE_SENSOR_SXGA_IMAGE_YRES

#define XN_DEVICE_SENSOR_DEFAULT_DEPTH_XRES XN_DEVICE_SENSOR_QVGA_DEPTH_XRES
#define XN_DEVICE_SENSOR_DEFAULT_DEPTH_YRES XN_DEVICE_SENSOR_QVGA_DEPTH_YRES
#define XN_DEVICE_SENSOR_DEFAULT_IMAGE_XRES XN_DEVICE_SENSOR_QVGA_IMAGE_XRES
#define XN_DEVICE_SENSOR_DEFAULT_IMAGE_YRES XN_DEVICE_SENSOR_QVGA_IMAGE_YRES

#define XN_DEVICE_SENSOR_INTERNAL_IMAGE_XRES XN_DEVICE_SENSOR_VGA_IMAGE_XRES
#define XN_DEVICE_SENSOR_INTERNAL_IMAGE_YRES XN_DEVICE_SENSOR_VGA_IMAGE_YRES

#define XN_DEVICE_SENSOR_MIN_DEPTH 0
#define XN_DEVICE_SENSOR_MAX_DEPTH 10000
#define XN_DEVICE_SENSOR_NO_DEPTH_VALUE 0
#define XN_DEVICE_SENSOR_MAX_SHIFT_VALUE 2048/*336*/

#define XN_DEVICE_SENSOR_BOARDID_SEP ":"
#define XN_DEVICE_SENSOR_DEFAULT_ID "*"

#define XN_DEVICE_SENSOR_INI_FILE_EXT ".ini"

#define XN_SENSOR_PROTOCOL_SENSOR_ID_LENGTH	16

#define XN_SENSOR_PROFANE_DIST_QVGA 1
#define XN_SENSOR_PROFANE_DIST_VGA 3
#define XN_SENSOR_PROFANE_MDIFF 350

#define XN_SENSOR_GLOBHIST_REGSIZE_QVGA 40
#define XN_SENSOR_GLOBHIST_REGSIZESCORE_QVGA 62
#define XN_SENSOR_GLOBHIST_FRAME_SIZE_QVGA 7

#define XN_SENSOR_GLOBHIST_REGSIZE_VGA 160
#define XN_SENSOR_GLOBHIST_REGSIZESCORE_VGA 250
#define XN_SENSOR_GLOBHIST_FRAME_SIZE_VGA 15

#define XN_SENSOR_PP_FILTER_HEIGHT_SPACE_QVGA 4
#define XN_SENSOR_PP_FILTER_HEIGHT_SPACE_VGA 8

#define XN_SENSOR_PROTOCOL_NUM_FRAME_BUFFERS 3

#define XN_SENSOR_TIMESTAMP_SANITY_DIFF 10 // in ms

#define XN_MASK_DEVICE_SENSOR			"DeviceSensor"
#define XN_MASK_DEVICE_IO				"DeviceIO"
#define XN_MASK_SENSOR_PROTOCOL			"DeviceSensorProtocol"
#define XN_MASK_SENSOR_PROTOCOL_IMAGE	XN_MASK_SENSOR_PROTOCOL "Image"
#define XN_MASK_SENSOR_PROTOCOL_DEPTH	XN_MASK_SENSOR_PROTOCOL "Depth"
#define XN_MASK_SENSOR_PROTOCOL_AUDIO	XN_MASK_SENSOR_PROTOCOL "Audio"
#define XN_MASK_SENSOR_READ				"DeviceSensorRead"
#define XN_MASK_SENSOR_READ_IMAGE		XN_MASK_SENSOR_READ "Image"
#define XN_MASK_SENSOR_READ_DEPTH		XN_MASK_SENSOR_READ "Depth"
#define XN_MASK_SENSOR_READ_AUDIO		XN_MASK_SENSOR_READ "Audio"
#define XN_DUMP_AUDIO_IN				"AudioIn"
#define XN_DUMP_IMAGE_IN				"ImageIn"
#define XN_DUMP_DEPTH_IN				"DepthIn"
#define XN_DUMP_MINI_PACKETS			"MiniPackets"
#define XN_DUMP_TIMESTAMPS				"SensorTimestamps"
#define XN_DUMP_BANDWIDTH				"SensorBandwidth"
#define XN_DUMP_BAD_IMAGE				"BadImage"
#define XN_DUMP_FRAME_SYNC				"FrameSync"


#define XN_NORMALIZE_DEPTH(x)	XN_MAX(XN_MIN(x, XN_DEVICE_SENSOR_MAX_DEPTH), XN_DEVICE_SENSOR_MIN_DEPTH)

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
class XnSensorFirmware;
struct XnDevicePrivateData;
class XnSensorFixedParams;
class XnSensorFPS;
class XnCmosInfo;

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------

typedef struct XnSensorObjects
{
	XnSensorObjects(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData, XnSensorFixedParams* pFixedParams, XnSensorFPS* pFPS, XnCmosInfo* pCmosInfo) :
		pFirmware(pFirmware),
		pDevicePrivateData(pDevicePrivateData),
		pFixedParams(pFixedParams),
		pFPS(pFPS),
		pCmosInfo(pCmosInfo)
	{}

	XnSensorFirmware* pFirmware;
	XnDevicePrivateData* pDevicePrivateData;
	XnSensorFixedParams* pFixedParams;
	XnSensorFPS* pFPS;
	XnCmosInfo* pCmosInfo;
} XnSensorObjects;

typedef struct XnSensorInfo
{
	XnSensorVer nSensorVer;
//	XnChar     cSensorID[XN_SENSOR_PROTOCOL_SENSOR_ID_LENGTH+1];
} XnSensorInfo;

typedef struct XnHWInfo
{
	XnHWVer	  nHWVer;
} XnHWInfo;

typedef struct XnChipInfo
{
	XnChipVer	nChipVer;
} XnChipInfo;

typedef enum {
	RGBREG_NONE = 0,
	RGBREG_FIX_IMAGE = 1,
	RGBREG_FIX_DEPTH = 2
} XnDeviceSensorRGBRegType;

typedef struct
{
	XN_THREAD_HANDLE	hThread;
	XnBool				bKillThread;
	XnBool				bThreadAlive;
} XnDeviceSensorThreadContext;

typedef struct XnRegistrationFunctionCoefficients
{
	XnDouble dA;
	XnDouble dB;
	XnDouble dC;
	XnDouble dD;
	XnDouble dE;
	XnDouble dF;
} XnRegistrationFunctionCoefficients;

typedef struct
{
	/* Is this the first time timestamp is calculated. */
	XnBool bFirst;
	/* The device TS which we use as reference for calculation. */
	XnUInt32 nReferenceTS;
	/* The time corresponding to the TS in nReferenceTS. */
	XnUInt64 nTotalTicksAtReferenceTS;
	/* The last device TS received. */
	XnUInt32 nLastDeviceTS;
	/* The last result time calculated. */
	XnUInt64 nLastResultTime;
	/* Stream name - for debug purposes. */
	const XnChar* csStreamName;
} XnTimeStampData;


typedef struct XnCmosBlankingCoefficients
{
	XnFloat fA;
	XnFloat fB;
} XnCmosBlankingCoefficients;

typedef struct XnCmosBlankingInformation
{
	XnCmosBlankingCoefficients Coefficients[2];
} XnCmosBlankingInformation;

typedef XnStatus (XN_CALLBACK_TYPE* NewAudioDataCallback)(void* pCookie);

struct XnSpecificUsbDevice; // Forward Declaration
class XnSensor; // Forward Declaration

typedef struct XnDevicePrivateData
{
	XnVersions Version;
	XnUInt32 nDepthFramePos;
	XnUInt32 nImageFramePos;

	XnChar				cpSensorID[XN_SENSOR_PROTOCOL_SENSOR_ID_LENGTH+1];
	XnUInt8				nBoardID;
	XN_SENSOR_HANDLE	SensorHandle;
	XnSensorInfo		SensorInfo;
	XnFirmwareInfo		FWInfo;
	XnHWInfo			HWInfo;
	XnChipInfo			ChipInfo;

	XN_CRITICAL_SECTION_HANDLE hAudioBufferCriticalSection;

	XnSpecificUsbDevice* pSpecificDepthUsb;
	XnSpecificUsbDevice* pSpecificImageUsb;
	XnSpecificUsbDevice* pSpecificMiscUsb;

	XnUInt32 nImageBayerDownSampleStep;

	XnCropping IRCropping;

	/** A single (big) buffer for audio. */
	XN_AUDIO_TYPE* pAudioBuffer;
	/** An array of pointers into the audio buffer. */
	XnUInt64* pAudioPacketsTimestamps;
	/** The index of the next packet that should be written. */
	volatile XnUInt32 nAudioWriteIndex;
	/** The index of the next packet that can be read. */
	volatile XnUInt32 nAudioReadIndex;
	/** Size of the audio buffer, in packets. */
	XnUInt32 nAudioBufferNumOfPackets;
	/** Size of the audio buffer, in bytes. */
	XnUInt32 nAudioBufferSize;
	XnUInt32 nAudioPacketSize;
	/** When true, when reading from device, if frames were lost, their audio will also be dropped. */
	XnBool bSyncAudio;
	/** A callback for new data */
	NewAudioDataCallback pAudioCallback;
	void* pAudioCallbackCookie;

	XnFloat fDeviceFrequency;

	/** Keeps the global reference TS (the one marking time-zero). */
	XnUInt32 nGlobalReferenceTS;
	/** Keeps the OS time of global reference TS. */
	XnUInt64 nGlobalReferenceOSTime;

	/** A general critical section used to synch end-points threads. */
	XN_CRITICAL_SECTION_HANDLE hEndPointsCS;

	XnDepthPixel* pTempDepth1;
	XnUInt8* pTempImage1;

	XnDeviceConfig DeviceConfig;

	XnBool bIgnoreDataPackets;

	XnUInt16 nFrameSync;

	/** Used to dump timestamps data. */
	XnDump TimestampsDump;
	/** Used to dump bandwidth data. */
	XnDump BandwidthDump;
	/** Used to dump MiniPackets data. */
	XnDump MiniPacketsDump;

	XnBool bDepthMirror;
	XnBool bImageMirror;
	XnBool bIRMirror;

	XnBool bMirror; // Mirroring is ON

	XnSensor* pSensor;

	XN_MUTEX_HANDLE hExecuteMutex;


} XnDevicePrivateData;

#pragma pack (push, 1)

typedef struct XnFixedParams
{
	// Misc
	XnInt32 nSerialNumber;
	XnInt32 nWatchDogTimeout;

	// Flash
	XnInt32 nFlashType;
	XnInt32 nFlashSize;
	XnInt32 nFlashBurstEnable;
	XnInt32 nFmifReadBurstCycles;
	XnInt32 nFmifReadAccessCycles;
	XnInt32 nFmifReadRecoverCycles;
	XnInt32 nFmifWriteAccessCycles;
	XnInt32 nFmifWriteRecoverCycles;
	XnInt32 nFmifWriteAssertionCycles;
	
	// Audio
	XnInt32 nI2SLogicClockPolarity;

	// Depth
	XnInt32 nDepthCiuHorizontalSyncPolarity;
	XnInt32 nDepthCiuVerticalSyncPolarity;
	XnInt32 nDepthCmosType;
	XnInt32 nDepthCmosI2CAddress;
	XnInt32 nDepthCmosI2CBus;

	// Image
	XnInt32 nImageCiuHorizontalSyncPolarity;
	XnInt32 nImageCiuVerticalSyncPolarity;
	XnInt32 nImageCmosType;
	XnInt32 nImageCmosI2CAddress;
	XnInt32 nImageCmosI2CBus;

	// Geometry
	XnInt32 nIrCmosCloseToProjector;
	XnFloat fDCmosEmitterDistance;
	XnFloat fDCmosRCmosDistance;
	XnFloat fReferenceDistance;
	XnFloat fReferencePixelSize;

	// Clocks
	XnInt32 nPllValue;
	XnInt32 nSystemClockDivider;
	XnInt32 nRCmosClockDivider;
	XnInt32 nDCmosClockDivider;
	XnInt32 nAdcClocDivider;
	XnInt32 nI2CStandardSpeedHCount;
	XnInt32 nI2CStandardSpeedLCount;
	
	XnInt32 nI2CHoldFixDelay;

	XnInt32 nSensorType;
	XnInt32 nDebugMode;
	XnInt32 nUseExtPhy;
	XnInt32 bProjectorProtectionEnabled;
	XnInt32 nProjectorDACOutputVoltage;
	XnInt32 nProjectorDACOutputVoltage2;
	XnInt32 nTecEmitterDelay;
} XnFixedParams;

typedef struct XnFixedParamsV26
{
	// Misc
	XnInt32 nSerialNumber;
	XnInt32 nWatchDogTimeout;

	// Flash
	XnInt32 nFlashType;
	XnInt32 nFlashSize;
	XnInt32 nFlashBurstEnable;
	XnInt32 nFmifReadBurstCycles;
	XnInt32 nFmifReadAccessCycles;
	XnInt32 nFmifReadRecoverCycles;
	XnInt32 nFmifWriteAccessCycles;
	XnInt32 nFmifWriteRecoverCycles;
	XnInt32 nFmifWriteAssertionCycles;

	// Audio
	XnInt32 nI2SLogicClockPolarity;

	// Depth
	XnInt32 nDepthCiuHorizontalSyncPolarity;
	XnInt32 nDepthCiuVerticalSyncPolarity;
	XnInt32 nDepthCmosType;
	XnInt32 nDepthCmosI2CAddress;
	XnInt32 nDepthCmosI2CBus;

	// Image
	XnInt32 nImageCiuHorizontalSyncPolarity;
	XnInt32 nImageCiuVerticalSyncPolarity;
	XnInt32 nImageCmosType;
	XnInt32 nImageCmosI2CAddress;
	XnInt32 nImageCmosI2CBus;

	// Geometry
	XnInt32 nIrCmosCloseToProjector;
	XnFloat fDCmosEmitterDistance;
	XnFloat fDCmosRCmosDistance;
	XnFloat fReferenceDistance;
	XnFloat fReferencePixelSize;

	// Clocks
	XnInt32 nPllValue;
	XnInt32 nSystemClockDivider;
	XnInt32 nRCmosClockDivider;
	XnInt32 nDCmosClockDivider;
	XnInt32 nAdcClocDivider;
	XnInt32 nI2CStandardSpeedHCount;
	XnInt32 nI2CStandardSpeedLCount;

	XnInt32 nI2CHoldFixDelay;

	XnInt32 nSensorType;
	XnInt32 nDebugMode;
	XnInt32 nTecEmitterDelay;
	XnInt32 nUseExtPhy;
} XnFixedParamsV26;

typedef struct XnFixedParamsV20
{
	// Misc
	XnInt32 nSerialNumber;
	XnInt32 nWatchDogTimeout;

	// Flash
	XnInt32 nFlashType;
	XnInt32 nFlashSize;
	XnInt32 nFlashBurstEnable;
	XnInt32 nFmifReadBurstCycles;
	XnInt32 nFmifReadAccessCycles;
	XnInt32 nFmifReadRecoverCycles;
	XnInt32 nFmifWriteAccessCycles;
	XnInt32 nFmifWriteRecoverCycles;
	XnInt32 nFmifWriteAssertionCycles;

	// Audio
	XnInt32 nI2SLogicClockPolarity;

	// Depth
	XnInt32 nDepthCiuHorizontalSyncPolarity;
	XnInt32 nDepthCiuVerticalSyncPolarity;
	XnInt32 nDepthCmosType;
	XnInt32 nDepthCmosI2CAddress;
	XnInt32 nDepthCmosI2CBus;

	// Image
	XnInt32 nImageCiuHorizontalSyncPolarity;
	XnInt32 nImageCiuVerticalSyncPolarity;
	XnInt32 nImageCmosType;
	XnInt32 nImageCmosI2CAddress;
	XnInt32 nImageCmosI2CBus;

	// Geometry
	XnInt32 nIrCmosCloseToProjector;
	XnFloat fDCmosEmitterDistance;
	XnFloat fDCmosRCmosDistance;
	XnFloat fReferenceDistance;
	XnFloat fReferencePixelSize;

	// Clocks
	XnInt32 nPllValue;
	XnInt32 nSystemClockDivider;
	XnInt32 nRCmosClockDivider;
	XnInt32 nDCmosClockDivider;
	XnInt32 nAdcClocDivider;
	XnInt32 nI2CStandardSpeedHCount;
	XnInt32 nI2CStandardSpeedLCount;

	XnInt32 nI2CHoldFixDelay;

	XnInt32 nSensorType;
	XnInt32 nDebugMode;
	XnInt32 nTecEmitterDelay;
} XnFixedParamsV20;

typedef struct XnRegistrationInformation1000
{
	XnRegistrationFunctionCoefficients FuncX;
	XnRegistrationFunctionCoefficients FuncY;
	XnDouble dBeta;
} XnRegistrationInformation1000;

typedef struct XnRegistrationInformation1080
{
	XnInt32 nRGS_DX_CENTER;
	XnInt32 nRGS_AX;
	XnInt32 nRGS_BX;
	XnInt32 nRGS_CX;
	XnInt32 nRGS_DX;
	XnInt32 nRGS_DX_START;
	XnInt32 nRGS_AY;
	XnInt32 nRGS_BY;
	XnInt32 nRGS_CY;
	XnInt32 nRGS_DY;
	XnInt32 nRGS_DY_START;
	XnInt32 nRGS_DX_BETA_START;
	XnInt32 nRGS_DY_BETA_START;
	XnInt32 nRGS_ROLLOUT_BLANK;
	XnInt32 nRGS_ROLLOUT_SIZE;
	XnInt32 nRGS_DX_BETA_INC;
	XnInt32 nRGS_DY_BETA_INC;
	XnInt32 nRGS_DXDX_START;
	XnInt32 nRGS_DXDY_START;
	XnInt32 nRGS_DYDX_START;
	XnInt32 nRGS_DYDY_START;
	XnInt32 nRGS_DXDXDX_START;
	XnInt32 nRGS_DYDXDX_START;
	XnInt32 nRGS_DXDXDY_START;
	XnInt32 nRGS_DYDXDY_START;
	XnInt32 nBACK_COMP1;
	XnInt32 nRGS_DYDYDX_START;
	XnInt32 nBACK_COMP2;
	XnInt32 nRGS_DYDYDY_START;
} XnRegistrationInformation1080;

typedef struct XnRegistrationPaddingInformation
{
	XnUInt16 nStartLines;
	XnUInt16 nEndLines;
	XnUInt16 nCroppingLines;
} XnRegistrationPaddingInformation;

typedef struct XnDepthInformation
{
	XnUInt16 nConstShift;
} XnDepthInformation;

typedef struct XnFrequencyInformation
{
	XnFloat fDeviceFrequency;
} XnFrequencyInformation;

typedef struct XnAudioSharedBuffer
{
	XnUInt32 nPacketCount;
	XnUInt32 nPacketSize;
	XnUInt32 nTimestampsListOffset;
	XnUInt32 nBufferOffset;
	XnUInt32 nWritePacketIndex;
} XnAudioSharedBuffer;

#pragma pack (pop)

class XnDeviceSensor : public XnDeviceBaseProxy
{
public:
	XnDeviceSensor();
	~XnDeviceSensor();

	static XnStatus GetDefinition(XnDeviceDefinition* pDeviceDefinition);
	static XnStatus Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();

	static XnStatus DestroyStreamData(XnStreamData** ppStreamData);
};

XnStatus XnDeviceSensorSetParam(XnDevicePrivateData* pDevicePrivateData, const XnChar* cpParamName, const XnInt32 nValue);

#endif 	
