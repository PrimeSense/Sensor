/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef __XN_SENSOR_CLIENT_SERVER_H__
#define __XN_SENSOR_CLIENT_SERVER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDataPacker.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------
#define XN_SENSOR_SERVER_IP_ADDRESS				XN_OS_NETWORK_LOCAL_HOST
#define XN_SENSOR_SERVER_PORT					18180
#define XN_SENSOR_SERVER_MAX_MESSAGE_SIZE		(4 * 1024 * 1024)
#define XN_SENSOR_SERVER_CONFIG_PACKER_SIZE		(100 * 1024)
#define XN_SENSOR_SERVER_MAX_REPLY_SIZE			(40 * 1024)
#define XN_SENSOR_REPLY_TIMEOUT					30000
#define XN_SENSOR_SERVER_RUNNING_MUTEX_TIMEOUT	15000
#define XN_MASK_SENSOR_SERVER					"SensorServer"
#define XN_SENSOR_SERVER_RUNNING_EVENT_NAME		"XnSensorServerRunningEvent"
#define XN_SENSOR_SERVER_RUNNING_MUTEX_NAME		"XnSensorServerRunningMutex"
#define XN_SENSOR_SERVER_CONFIG_FILE_SECTION	"Server"

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
typedef enum XnSensorServerCustomMessages
{
	XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND = XN_PACKED_CUSTOM_MESSAGE + 1,
	XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR,
	XN_SENSOR_SERVER_MESSAGE_INI_FILE,
	XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY,
	XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY,
	XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY,
	XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY,
	XN_SENSOR_SERVER_MESSAGE_NEW_STREAM_DATA,
	XN_SENSOR_SERVER_MESSAGE_READ_STREAM,
	XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM,
	XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM,
	XN_SENSOR_SERVER_MESSAGE_BYE,
} XnSensorServerCustomMessages;

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef struct XnSensorClientServerReply
{
	XnStatus nRetVal;
	XnSensorServerCustomMessages Type;
	XnUInt32 nDataSize;
	XnUChar pData[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
} XnSensorClientServerReply;

typedef struct XnSensorServerMessageIniFile
{
	XnChar strFileName[XN_FILE_MAX_PATH];
	XnChar strSectionName[XN_DEVICE_MAX_STRING_LENGTH];
} XnSensorServerMessageIniFile;

typedef struct XnSensorServerMessageGetPropertyRequest
{
	XnChar strModuleName[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strPropertyName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nSize;
} XnSensorServerMessageGetPropertyRequest;

typedef struct XnSensorServerGetGeneralPropReply
{
	XnUInt32 nSize;
	XnUChar pData[XN_SENSOR_SERVER_MAX_REPLY_SIZE - 50];
} XnSensorServerGetGeneralPropReply;

typedef struct XnSensorServerNewStreamData
{
	XnUInt64 nTimestamp;
	XnUInt32 nFrameID;
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
} XnSensorServerNewStreamData;

typedef struct XnSensorServerReadReply
{
	XnUInt64 nTimestamp;
	XnUInt32 nOffset;
	XnUInt32 nDataSize;
	XnUInt32 nFrameID;
} XnSensorServerReadReply;

#pragma pack (pop)

#endif //__XN_SENSOR_CLIENT_SERVER_H__
