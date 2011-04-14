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






#ifndef _XN_STREAM_PARAMS_H_
#define _XN_STREAM_PARAMS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamFormats.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/** The maximum permitted Xiron device name string length. */ 
#define XN_DEVICE_MAX_STRING_LENGTH 200

//---------------------------------------------------------------------------
// Modules Names
//---------------------------------------------------------------------------
#define XN_MODULE_NAME_DEVICE			"Device"

#define XN_MODULE_NAME_FIXED_PARAMS		"FixedParams"
#define	XN_MODULE_NAME_SHIFTS			"Shifts"

#ifndef XN_USE_DEVICE_3_6
#pragma deprecated ("XN_MODULE_NAME_FIXED_PARAMS", "XN_MODULE_NAME_SHIFTS")
#endif

//---------------------------------------------------------------------------
// Streams Types
//---------------------------------------------------------------------------
#define XN_STREAM_TYPE_DEPTH	"Depth"
#define XN_STREAM_TYPE_IMAGE	"Image"
#define XN_STREAM_TYPE_IR		"IR"
#define XN_STREAM_TYPE_AUDIO	"Audio"

//---------------------------------------------------------------------------
// Streams Names
//---------------------------------------------------------------------------
#define XN_STREAM_NAME_DEPTH	XN_STREAM_TYPE_DEPTH
#define XN_STREAM_NAME_IMAGE	XN_STREAM_TYPE_IMAGE
#define XN_STREAM_NAME_IR		XN_STREAM_TYPE_IR
#define XN_STREAM_NAME_AUDIO	XN_STREAM_TYPE_AUDIO

//---------------------------------------------------------------------------
// General Properties
//---------------------------------------------------------------------------
#define XN_STREAM_PROPERTY_TYPE						"Type"
#define XN_STREAM_PROPERTY_IS_STREAM				"IsStream"
#define XN_STREAM_PROPERTY_IS_FRAME_BASED			"IsFrameBased"
#define XN_STREAM_PROPERTY_IS_PIXEL_BASED			"IsPixelBased"
#define XN_STREAM_PROPERTY_IS_STREAMING				"IsStreaming"

/** Integer */
#define XN_MODULE_PROPERTY_LOCK						"Lock"

//---------------------------------------------------------------------------
// General Stream Properties
//---------------------------------------------------------------------------
/** Integer */
#define XN_STREAM_PROPERTY_STATE					"State"
/** Integer */
#define XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE		"RequiredDataSize"
/** Integer */
#define XN_STREAM_PROPERTY_FRAME_ID					"FrameId"
/** Integer */
#define XN_STREAM_PROPERTY_TIMESTAMP				"Timestamp"
/** Integer */
#define XN_STREAM_PROPERTY_IS_NEW_DATA				"IsNew"
/** Integer (XnOutputFormats)*/
#define XN_STREAM_PROPERTY_OUTPUT_FORMAT			"OutputFormat"
/** General (void*) */ 
#define XN_STREAM_PROPERTY_BUFFER					"Buffer"
/** Integer */ 
#define XN_STREAM_PROPERTY_BUFFER_SIZE				"BufferSize"
/** Integer */
#define XN_STREAM_PROPERTY_COMPRESSION				"Compression"
/** String */
#define XN_STREAM_PROPERTY_SHARED_BUFFER_NAME		"SharedBufferName"
/** Boolean */
#define XN_STREAM_PROPERTY_ACTUAL_READ_DATA			"ActualReadData"

/** Integer */ 
#define XN_STREAM_PROPERTY_BUFFER_ALLOCATED_SIZE	XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE

#ifndef USE_DEVICE_3_6
#pragma deprecated ("XN_STREAM_PROPERTY_BUFFER_ALLOCATED_SIZE")
#endif

//---------------------------------------------------------------------------
// Frame-Based Stream Properties (Depth, Image, IR)
//---------------------------------------------------------------------------
/** Integer */ 
#define XN_STREAM_PROPERTY_FPS						"FPS"
/** Integer */
#define XN_STREAM_PROPERTY_NUMBER_OF_FRAMES			"NumberOfFrames"
/** Integer */ 
#define XN_STREAM_PROPERTY_INPUT_FORMAT				"InputFormat"
/** XnDynamicSizeBuffer */
#define XN_STREAM_PROPERTY_LAST_RAW_FRAME			"LastRawFrame"

//---------------------------------------------------------------------------
// Pixel-Based Stream Properties (Depth, Image, IR)
//---------------------------------------------------------------------------
/** XnResolutions */
#define XN_STREAM_PROPERTY_RESOLUTION				"Resolution"
/** Integer */ 
#define XN_STREAM_PROPERTY_X_RES					"XRes"
/** Integer */ 
#define XN_STREAM_PROPERTY_Y_RES					"YRes"
/** Integer */ 
#define XN_STREAM_PROPERTY_BYTES_PER_PIXEL			"BytesPerPixel"
/** XnCropping */ 
#define XN_STREAM_PROPERTY_CROPPING					"Cropping"

/** Boolean */
#define XN_STREAM_PROPERTY_WHITE_BALANCE_ENABLED	"WhiteBalancedEnabled"
/** Integer */ 
#define XN_STREAM_PROPERTY_GAIN						"Gain"
/** Integer */
#define XN_STREAM_PROPERTY_SUPPORT_MODES_COUNT		"SupportedModesCount"
/** General (XnCmosPreset array) */
#define XN_STREAM_PROPERTY_SUPPORT_MODES			"SupportedModes"

//---------------------------------------------------------------------------
// Depth Specific Properties
//---------------------------------------------------------------------------
/** Integer */ 
#define XN_STREAM_PROPERTY_HOLE_FILTER				"HoleFilter"
/** Integer */ 
#define XN_STREAM_PROPERTY_MIN_DEPTH				"MinDepthValue"
/** Integer */ 
#define XN_STREAM_PROPERTY_MAX_DEPTH				"MaxDepthValue"
/** Integer */ 
#define XN_STREAM_PROPERTY_SHADOW					"ShadowValue"
/** Integer */ 
#define XN_STREAM_PROPERTY_NO_SAMPLE				"NoSampleValue"
/** Boolean */ 
#define XN_STREAM_PROPERTY_REGISTRATION				"Registration"
/** XnProcessingType */ 
#define XN_STREAM_PROPERTY_REGISTRATION_TYPE		"RegistrationType"
/** XnDepthAGCBin* */
#define XN_STREAM_PROPERTY_AGC_BIN					"AGCBin"
/** Integer */ 
#define XN_STREAM_PROPERTY_CONST_SHIFT				"ConstShift"
/** Integer */ 
#define XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR		"PixelSizeFactor"
/** Integer */ 
#define XN_STREAM_PROPERTY_MAX_SHIFT				"MaxShift"
/** Integer */ 
#define XN_STREAM_PROPERTY_PARAM_COEFF				"ParamCoeff"
/** Integer */ 
#define XN_STREAM_PROPERTY_SHIFT_SCALE				"ShiftScale"
/** XN_DEPTH_TYPE[] */ 
#define XN_STREAM_PROPERTY_S2D_TABLE				"S2D"
/** XnUInt16[] */ 
#define XN_STREAM_PROPERTY_D2S_TABLE				"D2S"
/** Integer */
#define XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH			"DeviceMaxDepth"
/** XN_DEPTH_TYPE */ 
#define XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE		"ZPD"
/** Real */ 
#define XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE	"ZPPS"
/** Real */ 
#define XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE	"LDDIS"
/** Boolean */
#define XN_STREAM_PROPERTY_GMC_MODE					"GmcMode"

//---------------------------------------------------------------------------
// Shifts Properties (up to v3.6)
//---------------------------------------------------------------------------
/** Integer */ 
#define XN_MODULE_PROPERTY_CONST_SHIFT				XN_STREAM_PROPERTY_CONST_SHIFT
/** Integer */ 
#define XN_MODULE_PROPERTY_PIXEL_SIZE_FACTOR		XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR
/** Integer */ 
#define XN_MODULE_PROPERTY_MAX_SHIFT				XN_STREAM_PROPERTY_MAX_SHIFT
/** Integer */ 
#define XN_MODULE_PROPERTY_PARAM_COEFF				XN_STREAM_PROPERTY_PARAM_COEFF
/** Integer */ 
#define XN_MODULE_PROPERTY_SHIFT_SCALE				XN_STREAM_PROPERTY_SHIFT_SCALE
/** XN_DEPTH_TYPE[] */ 
#define XN_MODULE_PROPERTY_S2D_TABLE				XN_STREAM_PROPERTY_S2D_TABLE
/** XnUInt16[] */ 
#define XN_MODULE_PROPERTY_D2S_TABLE				XN_STREAM_PROPERTY_D2S_TABLE

#ifndef XN_USE_DEVICE_3_6
#pragma deprecated ("XN_MODULE_PROPERTY_CONST_SHIFT", "XN_MODULE_PROPERTY_PIXEL_SIZE_FACTOR", "XN_MODULE_PROPERTY_MAX_SHIFT")
#pragma deprecated ("XN_MODULE_PROPERTY_PARAM_COEFF", "XN_MODULE_PROPERTY_SHIFT_SCALE")
#pragma deprecated ("XN_MODULE_PROPERTY_S2D_TABLE", "XN_MODULE_PROPERTY_D2S_TABLE")
#endif

//---------------------------------------------------------------------------
// FixedParams Properties (up to v3.6)
//---------------------------------------------------------------------------
/** XN_DEPTH_TYPE */ 
#define XN_MODULE_PROPERTY_ZERO_PLANE_DISTANCE		XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE
/** Real */ 
#define XN_MODULE_PROPERTY_ZERO_PLANE_PIXEL_SIZE	XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE
/** Real */ 
#define XN_MODULE_PROPERTY_EMITTER_DCMOS_DISTANCE	XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE

#ifndef XN_USE_DEVICE_3_6
#pragma deprecated ("XN_MODULE_PROPERTY_ZERO_PLANE_DISTANCE", "XN_MODULE_PROPERTY_ZERO_PLANE_PIXEL_SIZE", "XN_MODULE_PROPERTY_EMITTER_DCMOS_DISTANCE")
#endif

//---------------------------------------------------------------------------
// Image Specific Properties
//---------------------------------------------------------------------------
/** Integer */ 
#define XN_STREAM_PROPERTY_FLICKER					"Flicker"
/** Integer */ 
#define XN_STREAM_PROPERTY_QUALITY					"Quality"
/** Float */
#define XN_STREAM_PROPERTY_BRIGHTNESS				XN_CAPABILITY_BRIGHTNESS
/** Float */
#define XN_STREAM_PROPERTY_CONTRAST					XN_CAPABILITY_CONTRAST
/** Float */
#define XN_STREAM_PROPERTY_SATURATION				XN_CAPABILITY_SATURATION
/** Float */
#define XN_STREAM_PROPERTY_SHARPNESS				XN_CAPABILITY_SHARPNESS
/** Float */
#define XN_STREAM_PROPERTY_COLOR_TEMPERATURE		XN_CAPABILITY_COLOR_TEMPERATURE
/** Float */
#define XN_STREAM_PROPERTY_BACKLIGHT_COMPENSATION	XN_CAPABILITY_BACKLIGHT_COMPENSATION
/** Float */
#define XN_STREAM_PROPERTY_ZOOM						XN_CAPABILITY_ZOOM
/** Integer (in microseconds) */
#define XN_STREAM_PROPERTY_EXPOSURE					XN_CAPABILITY_EXPOSURE
/** Float */
#define XN_STREAM_PROPERTY_PAN						XN_CAPABILITY_PAN
/** Float */
#define XN_STREAM_PROPERTY_TILT						XN_CAPABILITY_TILT
/** Boolean */
#define XN_STREAM_PROPERTY_LOW_LIGHT_COMPENSATION	XN_CAPABILITY_LOW_LIGHT_COMPENSATION

//---------------------------------------------------------------------------
// Audio Specific Properties
//---------------------------------------------------------------------------
/** XnSampleRate */
#define XN_STREAM_PROPERTY_SAMPLE_RATE				"SampleRate"
/** Integer */
#define XN_STREAM_PROPERTY_LEFT_CHANNEL_VOLUME		"LeftChannelVolume"
/** Integer */
#define XN_STREAM_PROPERTY_RIGHT_CHANNEL_VOLUME		"RightChannelVolume"
/** Integer */
#define XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS		"NumOfChannels"
/** Boolean */
#define XN_STREAM_PROPERTY_IS_STEREO				"IsStereo"
/** Integer */
#define XN_STREAM_PROPERTY_READ_MODE				"ReadMode"
/** Integer */
#define XN_STREAM_PROPERTY_READ_CHUNK_SIZE			"ReadChunkSize"
/** Integer */
#define XN_STREAM_PROPERTY_READ_SYNC				"AudioReadSync"

#ifndef XN_USE_DEVICE_3_6
#pragma deprecated ("XN_STREAM_PROPERTY_READ_MODE", "XN_STREAM_PROPERTY_READ_SYNC")
#endif

//---------------------------------------------------------------------------
// DeviceParams Properties
//---------------------------------------------------------------------------
/** XnSDKVersion */
#define XN_MODULE_PROPERTY_SDK_VERSION				"SDKVersion"
/** String */
#define XN_MODULE_PROPERTY_DEVICE_NAME				"DeviceName"
/** String */
#define XN_MODULE_PROPERTY_USB_PATH					"USBPath"
/** Integer (XnSensorUsbInterface) */
#define XN_MODULE_PROPERTY_USB_INTERFACE			"UsbInterface"
/** Integer */
#define XN_MODULE_PROPERTY_NUMBER_OF_BUFFERS		"NumberOfBuffers"
/** Boolean */
#define XN_MODULE_PROPERTY_READ_ENDPOINT_1			"ReadEndpoint1"
/** Boolean */
#define XN_MODULE_PROPERTY_READ_ENDPOINT_2			"ReadEndpoint2"
/** Boolean */
#define XN_MODULE_PROPERTY_READ_ENDPOINT_3			"ReadEndpoint3"
/** Boolean */
#define XN_MODULE_PROPERTY_RESET_SENSOR_ON_STARTUP	"ResetSensorOnStartup"
/** String */
#define XN_MODULE_PROPERTY_ID						"ID"
/** String */ 
#define XN_MODULE_PROPERTY_PRIMARY_STREAM			"PrimaryStream"
/** Boolean */ 
#define XN_MODULE_PROPERTY_MIRROR					"Mirror"
/** Boolean */
#define XN_MODULE_PROPERTY_READ_DATA				"ReadData"
/** Integer */
#define XN_MODULE_PROPERTY_READ_WRITE_MODE			"ReadWriteMode"
/** Integer */
#define XN_MODULE_PROPERTY_SHARE_MODE				"ShareMode"
/** Boolean */
#define XN_MODULE_PROPERTY_FRAME_DELAY				"FrameDelay"
/** Boolean */
#define XN_MODULE_PROPERTY_FRAME_SYNC				"FrameSync"
/** XnCmosBlankingUnits */
#define XN_MODULE_PROPERTY_CMOS_BLANKING_UNITS		"CmosBlankingUnits"
/** XnCmosBlankingTime */
#define XN_MODULE_PROPERTY_CMOS_BLANKING_TIME		"CmosBlankingTime"
/** XnInnerParam */
#define XN_MODULE_PROPERTY_FIRMWARE_PARAM			"FirmwareParam"
/** XnVersions */
#define XN_MODULE_PROPERTY_VERSION					"Version"
/* XnDynamicSizeBuffer */
#define XN_MODULE_PROPERTY_FIXED_PARAMS				"FixedParams"
/** Integer */
#define XN_MODULE_PROPERTY_RESET					"Reset"
/** Integer */
#define XN_MODULE_PROPERTY_FIRMWARE_MODE			"FirmwareMode"
/** Boolean */
#define XN_MODULE_PROPERTY_HIGH_RES_TIMESTAMPS		"HighResTimestamps"
/** Boolean */
#define XN_MODULE_PROPERTY_CLOSE_STREAMS_ON_SHUTDOWN	"CloseStreamsOnShutdown"
/** Integer */
#define XN_MODULE_PROPERTY_SERVER_NO_CLIENTS_TIMEOUT	"ServerNoClientsTimeout"
/** Integer */
#define XN_MODULE_PROPERTY_SERVER_START_NEW_LOG_FILE	"ServerStartNewLogFile"
/** Integer */
#define XN_MODULE_PROPERTY_ERROR_STATE				"ErrorState"
/** Boolean */
#define XN_MODULE_PROPERTY_ENABLE_MULTI_PROCESS		"EnableMultiProcess"
/** String */
#define XN_MODULE_PROPERTY_PHYSICAL_DEVICE_NAME		"PhysicalDeviceName"
/** String */
#define XN_MODULE_PROPERTY_VENDOR_SPECIFIC_DATA		"VendorSpecificData"


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_PRIMARY_STREAM_ANY	"Any"
#define XN_PRIMARY_STREAM_NONE	"None"

#define XN_MAX_LOG_SIZE	(6*1024)

#define XN_GAIN_AUTO	0U

#define XN_QVGA_X_RES	320
#define XN_QVGA_Y_RES	240
#define XN_VGA_X_RES	640
#define XN_VGA_Y_RES	480
#define XN_SXGA_X_RES	1280
#define XN_SXGA_Y_RES	1024
#define XN_UXGA_X_RES	1600
#define XN_UXGA_Y_RES	1200


//---------------------------------------------------------------------------
// Enums - values of various properties
//---------------------------------------------------------------------------
typedef enum XnResolutions
{
	XN_RESOLUTION_CUSTOM = -1,
	XN_RESOLUTION_QVGA = 0, // 320x240
	XN_RESOLUTION_VGA = 1, // 640x480
	XN_RESOLUTION_SXGA = 2, // 1280x1024
	XN_RESOLUTION_UXGA = 3, // 1600x1200
	XN_RESOLUTION_QQVGA = 4, // 160x120
	XN_RESOLUTION_QCIF = 5, // 176x144
	XN_RESOLUTION_240P = 6, // 432x240
	XN_RESOLUTION_CIF = 7, // 352x288
	XN_RESOLUTION_WVGA = 8, // 640x360
	XN_RESOLUTION_480P = 9, // 864x480
	XN_RESOLUTION_800_448 = 10, // 800x448
	XN_RESOLUTION_SVGA = 11, // 800x600
	XN_RESOLUTION_576P = 12, // 1024x576
	XN_RESOLUTION_DV = 13, // 960x720
	XN_RESOLUTION_720P = 14, // 1280x720
	XN_RESOLUTION_1280_960 = 15, // 1280x960
} XnResolutions;

typedef enum 
{
	XN_SENSOR_FW_VER_UNKNOWN = 0,
	XN_SENSOR_FW_VER_0_17 = 1,
	XN_SENSOR_FW_VER_1_1 = 2,
	XN_SENSOR_FW_VER_1_2 = 3,
	XN_SENSOR_FW_VER_3_0 = 4,
	XN_SENSOR_FW_VER_4_0 = 5,
	XN_SENSOR_FW_VER_5_0 = 6,
	XN_SENSOR_FW_VER_5_1 = 7,
	XN_SENSOR_FW_VER_5_2 = 8,
	XN_SENSOR_FW_VER_5_3 = 9,
	XN_SENSOR_FW_VER_5_4 = 10,
} XnFWVer;

typedef enum
{
	XN_MODE_PS = 0,
	XN_MODE_MAINTENANCE = 1,
	XN_MODE_SAFE_MODE = 2,
} XnParamCurrentMode;

typedef enum {
	XN_SENSOR_VER_UNKNOWN = 0,
	XN_SENSOR_VER_2_0 = 1,
	XN_SENSOR_VER_3_0 = 2,
	XN_SENSOR_VER_4_0 = 3,
	XN_SENSOR_VER_5_0 = 4
} XnSensorVer;

typedef enum {
	XN_SENSOR_HW_VER_UNKNOWN = 0,
	XN_SENSOR_HW_VER_FPDB_10 = 1,
	XN_SENSOR_HW_VER_CDB_10  = 2,
	XN_SENSOR_HW_VER_RD_3  = 3,
	XN_SENSOR_HW_VER_RD_5  = 4
} XnHWVer;

typedef enum {
	XN_SENSOR_CHIP_VER_UNKNOWN = 0,
	XN_SENSOR_CHIP_VER_PS1000 = 1,
	XN_SENSOR_CHIP_VER_PS1080 = 2
} XnChipVer;

typedef enum
{
	XN_CMOS_TYPE_IMAGE = 0,
	XN_CMOS_TYPE_DEPTH = 1,

	XN_CMOS_COUNT
} XnCMOSType;

typedef enum
{
	XN_IO_IMAGE_FORMAT_BAYER = 0,
	XN_IO_IMAGE_FORMAT_YUV422 = 1,
	XN_IO_IMAGE_FORMAT_JPEG = 2,
	XN_IO_IMAGE_FORMAT_JPEG_420 = 3,
	XN_IO_IMAGE_FORMAT_JPEG_MONO = 4,
	XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422 = 5,
	XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER = 6,
} XnIOImageFormats;

typedef enum
{
	XN_IO_DEPTH_FORMAT_UNCOMPRESSED_16_BIT = 0,
	XN_IO_DEPTH_FORMAT_COMPRESSED_PS = 1,
	XN_IO_DEPTH_FORMAT_UNCOMPRESSED_10_BIT = 2,
	XN_IO_DEPTH_FORMAT_UNCOMPRESSED_11_BIT = 3,
	XN_IO_DEPTH_FORMAT_UNCOMPRESSED_12_BIT = 4,
} XnIODepthFormats;

typedef enum
{
	XN_VIDEO_STREAM_OFF = 0,
	XN_VIDEO_STREAM_COLOR = 1,
	XN_VIDEO_STREAM_DEPTH = 2,
	XN_VIDEO_STREAM_IR = 3,
} XnVideoStreamMode;

typedef enum
{
	XN_AUDIO_STREAM_OFF = 0,
	XN_AUDIO_STREAM_ON = 1,
} XnAudioStreamMode;

typedef enum
{
	XN_RESET_TYPE_POWER = 0,
	XN_RESET_TYPE_SOFT = 1,
	XN_RESET_TYPE_SOFT_FIRST = 2,
} XnParamResetType;

typedef enum XnSensorUsbInterface
{
	XN_SENSOR_USB_INTERFACE_DEFAULT = 0,
	XN_SENSOR_USB_INTERFACE_ISO_ENDPOINTS = 1,
	XN_SENSOR_USB_INTERFACE_BULK_ENDPOINTS = 2,
} XnSensorUsbInterface;

typedef enum XnProcessingType
{
	XN_PROCESSING_DONT_CARE = 0,
	XN_PROCESSING_HARDWARE = 1,
	XN_PROCESSING_SOFTWARE = 2,
} XnProcessingType;


//---------------------------------------------------------------------------
// Data Structures - structures that are arguments to properties
//---------------------------------------------------------------------------

#pragma pack (push, 1)

typedef struct XnSDKVersion
{
	XnUInt8 nMajor;
	XnUInt8 nMinor;
	XnUInt8 nMaintenance;
	XnUInt16 nBuild;
} XnSDKVersion;

typedef struct {
	XnUInt8 nMajor;
	XnUInt8 nMinor;
	XnUInt16 nBuild;
	XnUInt32 nChip;
	XnUInt16 nFPGA;
	XnUInt16 nSystemVersion;

	XnSDKVersion SDK;

	XnHWVer		HWVer;
	XnFWVer		FWVer;
	XnSensorVer SensorVer;
	XnChipVer	ChipVer;
} XnVersions;

typedef struct
{
	XnUInt16 nParam;
	XnUInt16 nValue;
} XnInnerParamData;

typedef struct XnCmosBlankingTime
{
	XnCMOSType nCmosID;
	XnFloat nTimeInMilliseconds;
	XnUInt16 nNumberOfFrames;
} XnCmosBlankingTime;

typedef struct XnCmosBlankingUnits
{
	XnCMOSType nCmosID;
	XnUInt16 nUnits;
	XnUInt16 nNumberOfFrames;
} XnCmosBlankingUnits;

typedef struct XnDepthAGCBin 
{
	XnUInt16 nBin;
	XnUInt16 nMin;
	XnUInt16 nMax;
} XnDepthAGCBin;

typedef struct XnDynamicSizeBuffer
{
	void* pData;
	XnUInt32 nMaxSize;
	XnUInt32 nDataSize;
} XnDynamicSizeBuffer;

typedef struct XnCmosPreset
{
	XnUInt16 nFormat;
	XnUInt16 nResolution;
	XnUInt16 nFPS;
} XnCmosPreset;


#pragma pack (pop)

#endif // _XN_STREAM_PARAMS_H_
