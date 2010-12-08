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






#ifndef _XN_DEVICE_H_
#define _XN_DEVICE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnDDK.h>
#include <XnStreamData.h>
#include <XnStreamDataSet.h>
#include <XnGeneralBuffer.h>
#include <XnStreamParams.h>
#include <XnPropertySet.h>
#include <XnTypes.h>

// General Defines
#define XN_DEVICE_READ_FRAME_TIMEOUT 2000

/** The basic definition of a Xiron device */ 
typedef struct XnDeviceDefinition
{
	/** The device unique name. */ 
	const XnChar*	cpName;

	/** A short description for the device. */ 
	const XnChar*	cpDescription;

	/** The device major version. */ 
	XnUInt8		nMajorVersion; 

	/** The device minor version. */ 
	XnUInt8		nMinorVersion;

	/** The compatible Xiron major version. */ 
	XnUInt8		nXironVersion;
} XnDeviceDefinition;

/** The Xiron device mode enumerator type. */ 
typedef enum {
	/** Read only mode. */ 
	XN_DEVICE_MODE_READ = 0,
	/** Write only mode. */ 
	XN_DEVICE_MODE_WRITE
} XnDeviceMode;

/** The Xiron device sharing mode. */
typedef enum {
	/* Device is exclusive for opening process. */
	XN_DEVICE_EXCLUSIVE = 0,
	/* Device is shared and can be opened by other processes. */
	XN_DEVICE_SHARED = 1
} XnDeviceSharingMode;

typedef XnChar XnConnectionString[XN_DEVICE_MAX_STRING_LENGTH];

/** This structure defines the Xiron device configuration (when opening a new device). */ 
typedef struct XnDeviceConfig
{
	/** The device I/O mode (read or write). */ 
	XnDeviceMode DeviceMode;
	/** The connection string (depending on the device this could mean: file name, IP, sensor serial, etc...). */ 
	const XnChar* cpConnectionString;
	/** Optional. A set of initial values to be used. */
	const XnPropertySet* pInitialValues;
	/** The device sharing mode. */
	XnDeviceSharingMode SharingMode;
} XnDeviceConfig;

typedef void* XnDeviceHandle;

typedef enum XnStreamsChangeEventType
{
	XN_DEVICE_STREAM_ADDED,
	XN_DEVICE_STREAM_DELETED,
} XnStreamsChangeEventType;

typedef void (XN_CALLBACK_TYPE* XnDeviceOnStreamsChangedEventHandler)(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnDeviceOnPropertyChangedEventHandler)(XnDeviceHandle pDeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie);
typedef void (XN_CALLBACK_TYPE* XnDeviceOnNewStreamDataEventHandler)(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, void* pCookie);

//---------------------------------------------------------------------------
// Exported Functions Declaration. Every Device Must Implement Them All!
//---------------------------------------------------------------------------

// Function prototypes names
#define XN_DEVICE_PROTO_VAL(x,y) x ## y
#define XN_DEVICE_PROTO_APPEND(x,y) XN_DEVICE_PROTO_VAL(x,y)

#ifndef XN_DEVICE_EXPORT_PREFIX
	#define XN_DEVICE_EXPORT_PREFIX XnDevice
#endif

#define XN_DEVICE_PROTO_NAME(name)	XN_DEVICE_PROTO_APPEND(XN_DEVICE_EXPORT_PREFIX, name)
#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(name) sig;
#include "XnDeviceProto.inl"
#undef XN_DEVICE_INTERFACE_FUNCTION

#endif //_XN_DEVICE_H_