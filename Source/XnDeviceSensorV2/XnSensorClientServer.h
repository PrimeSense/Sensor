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
