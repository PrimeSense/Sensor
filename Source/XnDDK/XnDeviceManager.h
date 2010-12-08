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