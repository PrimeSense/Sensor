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
#ifndef __XN_DEVICE_MANAGER_H__
#define __XN_DEVICE_MANAGER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnDevice.h>
#include "XnDeviceFunctionsTypedefs.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES	100
/** The name of the auto scan device */ 
#define XN_DEVICE_MANAGER_AUTO_NAME				"Auto"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* Represents all the functions in the device interface.
*/
typedef struct XnDeviceInterfaceFunctions {
	#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) XN_DEVICE_TYPEDEF_NAME(name) name;
	#include <XnDeviceProto.inl>
	#undef XN_DEVICE_INTERFACE_FUNCTION
} XnDeviceInterfaceFunctions;

/**
* Represents a descriptor of a device.
*/
typedef struct XnDeviceDescriptor
{
	XnDeviceDefinition Definition;
	XnDeviceInterfaceFunctions Interface;
	XN_LIB_HANDLE hLib;
} XnDeviceDescriptor;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
XnStatus XnDeviceManagerInit(const XnChar* strDevicesDir);
XnStatus XnDeviceManagerShutdown();
XnStatus XnDeviceManagerGetDeviceList(XnDeviceDefinition* aDeviceDefinitions, XnUInt32* pnCount);
XnStatus XnDeviceManagerGetDeviceByName(const XnChar* csName, XnDeviceDescriptor** ppDeviceDescriptor);

#endif //__XN_DEVICE_MANAGER_H__