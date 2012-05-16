/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
#ifndef HOST_PROTOCOL_H
#define HOST_PROTOCOL_H

#include <XnStreamParams.h>
#include "XnParams.h"
#include "XnDeviceSensor.h"


#define XN_HOST_MAGIC_25	0x5053	//PS
#define XN_FW_MAGIC_25		0x5350	//SP
#define XN_HOST_MAGIC_26	0x4d47	//MG
#define XN_FW_MAGIC_26		0x4252	//BR

#define XN_FPGA_VER_FPDB_26	0x21
#define XN_FPGA_VER_FPDB_25	0x0
#define XN_FPGA_VER_CDB		0x1
#define XN_FPGA_VER_RD3		0x2
#define XN_FPGA_VER_RD5		0x3
#define XN_FPGA_VER_RD1081	0x4
#define XN_FPGA_VER_RD1082	0x5

#define XN_CHIP_VER_PS1000	0x00101010
#define XN_CHIP_VER_PS1080	0x00202020
#define XN_CHIP_VER_PS1080A6	0x00212020

enum EPsProtocolOpCodes
{
	OPCODE_GET_VERSION = 0,
	OPCODE_KEEP_ALIVE = 1,
	OPCODE_GET_PARAM = 2,
	OPCODE_SET_PARAM = 3,
	OPCODE_GET_FIXED_PARAMS = 4,
	OPCODE_GET_MODE = 5,
	OPCODE_SET_MODE = 6,
	OPCODE_I2C_WRITE = 10,
	OPCODE_I2C_READ = 11,
	OPCODE_READ_AHB = 20,
	OPCODE_WRITE_AHB = 21,
	OPCODE_ALGORITM_PARAMS = 22,
	OPCODE_SET_CMOS_BLANKING = 34,
	OPCODE_GET_CMOS_BLANKING = 35,
	OPCODE_GET_CMOS_PRESETS = 36,
	OPCODE_GET_SERIAL_NUMBER = 37,
	OPCODE_GET_FAST_CONVERGENCE_TEC = 38,
	OPCODE_GET_PLATFORM_STRING = 39,
	OPCODE_GET_USB_CORE_TYPE = 40,
	OPCODE_KILL = 999,
};


enum XnHostProtocolOpcodes_V110
{
	OPCODE_V110_GET_VERSION = 0,
	OPCODE_V110_KEEP_ALIVE = 1,
	OPCODE_V110_GET_PARAM = 2,
	OPCODE_V110_SET_PARAM = 3,
	OPCODE_V110_GET_FIXED_PARAMS = 4,
	OPCODE_V110_GET_MODE = 5,
	OPCODE_V110_SET_MODE = 6,
	OPCODE_V110_GET_CMOS_REGISTER = 8,
	OPCODE_V110_SET_CMOS_REGISTER = 9,
	OPCODE_V110_READ_AHB = 20,
	OPCODE_V110_WRITE_AHB = 21,
	OPCODE_V110_ALGORITHM_PARAMS = 22,
};

enum EPsProtocolOpCodes_V017
{
	OPCODE_V017_GET_VERSION = 0,
	OPCODE_V017_KEEP_ALIVE = 1,
	OPCODE_V017_GET_PARAM = 2,
	OPCODE_V017_SET_PARAM = 3,
	OPCODE_V017_GET_FIXED_PARAMS = 4,
	OPCODE_V017_RESET = 5,
	OPCODE_V017_GET_CMOS_REGISTER = 7,
	OPCODE_V017_SET_CMOS_REGISTER = 8,
	OPCODE_V017_READ_AHB = 19,
	OPCODE_V017_WRITE_AHB = 20,
	OPCODE_V017_ALGORITM_PARAMS = 21,
};

#define OPCODE_INVALID 0xffff

typedef enum
{
	XN_HOST_PROTOCOL_ALGORITHM_DEPTH_INFO	= 0x00,
	XN_HOST_PROTOCOL_ALGORITHM_REGISTRATION	= 0x02,
	XN_HOST_PROTOCOL_ALGORITHM_PADDING		= 0x03,
	XN_HOST_PROTOCOL_ALGORITHM_BLANKING		= 0x06,
	XN_HOST_PROTOCOL_ALGORITHM_DEVICE_INFO	= 0x07,
	XN_HOST_PROTOCOL_ALGORITHM_FREQUENCY	= 0x80
} XnHostProtocolAlgorithmType;

typedef enum
{
	XN_HOST_PROTOCOL_MODE_WEBCAM = 0,
	XN_HOST_PROTOCOL_MODE_PS,
	XN_HOST_PROTOCOL_MODE_MAINTENANCE,
	XN_HOST_PROTOCOL_MODE_SOFT_RESET,
	XN_HOST_PROTOCOL_MODE_REBOOT,
	XN_HOST_PROTOCOL_MODE_SUSPEND,
	XN_HOST_PROTOCOL_MODE_RESUME,
	XN_HOST_PROTOCOL_MODE_INIT,
	XN_HOST_PROTOCOL_MODE_SYSTEM_RESTORE,
	XN_HOST_PROTOCOL_MODE_WAIT_FOR_ENUM,
	XN_HOST_PROTOCOL_MODE_SAFE_MODE
} XnHostProtocolModeType;

enum XnHostProtocolNacks
{
	ACK = 0,
	NACK_UNKNOWN_ERROR = 1,
	NACK_INVALID_COMMAND = 2,
	NACK_BAD_PACKET_CRC = 3,
	NACK_BAD_PACKET_SIZE = 4,
	NACK_BAD_PARAMS = 5,
	NACK_BAD_COMMAND_SIZE = 12,
	NACK_NOT_READY = 13,
	NACK_OVERFLOW = 14
};

typedef enum
{
	A2D_SAMPLE_RATE_48KHZ,
	A2D_SAMPLE_RATE_44KHZ,
	A2D_SAMPLE_RATE_32KHZ,
	A2D_SAMPLE_RATE_24KHZ,
	A2D_SAMPLE_RATE_22KHZ,
	A2D_SAMPLE_RATE_16KHZ,
	A2D_SAMPLE_RATE_12KHZ,
	A2D_SAMPLE_RATE_11KHZ,
	A2D_SAMPLE_RATE_8KHZ,
	A2D_NUM_OF_SAMPLE_RATES
} EA2d_SampleRate;

typedef enum XnHostProtocolUsbCore
{
	XN_USB_CORE_JANGO = 0,
	XN_USB_CORE_GADGETFS = 1,
} XnHostProtocolUsbCore;

#pragma pack(push,1)
typedef struct
{
	XnUInt16 nMagic;
	XnUInt16 nSize;
	XnUInt16 nOpcode;
	XnUInt16 nId;
	XnUInt16 nCRC16;
} XnHostProtocolHeaderV25;

typedef struct
{
	XnUInt16 nMagic;
	XnUInt16 nSize;
	XnUInt16 nOpcode;
	XnUInt16 nId;
} XnHostProtocolHeaderV26;

typedef struct
{
	XnUInt16 nErrorCode;
} XnHostProtocolReplyHeader;


#pragma pack(pop)

////////////////////////////////////// Exported h file should be only from here down
// Exported params

// All implemented protocol commands
// Init
XnStatus XnHostProtocolInitFWParams(XnDevicePrivateData* pDevicePrivateData, XnUInt8 nMajor, XnUInt8 nMinor, XnUInt16 nBuild, XnHostProtocolUsbCore usb);

XnStatus XnHostProtocolKeepAlive		(XnDevicePrivateData* pDevicePrivateData);
XnStatus XnHostProtocolGetVersion		(const XnDevicePrivateData* pDevicePrivateData, XnVersions& Version);
XnStatus XnHostProtocolAlgorithmParams	(XnDevicePrivateData* pDevicePrivateData,
										 XnHostProtocolAlgorithmType eAlgorithmType,
										 void* pAlgorithmInformation, XnUInt16 nAlgInfoSize, XnResolutions nResolution, XnUInt16 nFPS);
XnStatus XnHostProtocolSetImageResolution(XnDevicePrivateData* pDevicePrivateData, XnUInt32 nResolutionParamName, XnResolutions nRes);
XnStatus XnHostProtocolSetDepthResolution(XnDevicePrivateData* pDevicePrivateData, XnResolutions nRes);
XnStatus XnHostProtocolGetFixedParams(XnDevicePrivateData* pDevicePrivateData, XnFixedParams& FixedParams);

XnStatus XnHostProtocolSetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate nSampleRate);
XnStatus XnHostProtocolGetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate* pSampleRate);

XnStatus XnHostProtocolSetIRCropping	(XnDevicePrivateData* pDevicePrivateData, XnCropping* pCropping);
XnStatus XnHostProtocolSetMode			(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nMode);
XnStatus XnHostProtocolGetMode			(XnDevicePrivateData* pDevicePrivateData, XnUInt16& nMode);

XnStatus XnHostProtocolSetParam			(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nParam, XnUInt16 nValue);
XnStatus XnHostProtocolSetMultipleParams(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nNumOfParams, XnInnerParamData* anParams);
XnStatus XnHostProtocolReset(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nResetType);

XnStatus XnHostProtocolGetParam			(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nParam, XnUInt16& nValue);

XnStatus XnHostProtocolSetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nBin, XnUInt16 nMinShift, XnUInt16 nMaxShift);
XnStatus XnHostProtocolGetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nBin, XnUInt16* pnMinShift, XnUInt16* pnMaxShift);

XnStatus XnHostProtocolSetCmosBlanking	(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nLines, XnCMOSType nCMOSID, XnUInt16 nNumberOfFrames);
XnStatus XnHostProtocolGetCmosBlanking	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, XnUInt16* pnLines);

XnStatus XnHostProtocolGetCmosPresets	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, XnCmosPreset* aPresets, XnUInt32& nCount);

XnStatus XnHostProtocolGetSerialNumber	(XnDevicePrivateData* pDevicePrivateData, XnChar* cpSerialNumber);
XnStatus XnHostProtocolGetPlatformString(XnDevicePrivateData* pDevicePrivateData, XnChar* cpPlatformString);

XnStatus XnHostProtocolGetCMOSRegister(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, XnUInt16 nAddress, XnUInt16& nValue);
XnStatus XnHostProtocolSetCMOSRegister	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, XnUInt16 nAddress, XnUInt16 nValue);
XnStatus XnHostProtocolGetCMOSRegisterI2C(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, XnUInt16 nAddress, XnUInt16& nValue);
XnStatus XnHostProtocolSetCMOSRegisterI2C (XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, XnUInt16 nAddress, XnUInt16 nValue);
XnStatus XnHostProtocolReadAHB			(XnDevicePrivateData* pDevicePrivateData, XnUInt32 nAddress, XnUInt32 &nValue);
XnStatus XnHostProtocolWriteAHB			(XnDevicePrivateData* pDevicePrivateData, XnUInt32 nAddress, XnUInt32 nValue, XnUInt32 nMask);
XnStatus XnHostProtocolGetUsbCoreType	(XnDevicePrivateData* pDevicePrivateData, XnHostProtocolUsbCore& nValue);


#endif
