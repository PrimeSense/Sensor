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





#ifndef __XN_DEVICE_PROXY_H__
#define __XN_DEVICE_PROXY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/*****************************/
/* XnDevice interface		 */
/*****************************/
#define XN_DEVICE_PROXY_PROTO_APPEND(prefix, name) prefix ## name
#define XN_DEVICE_PROXY_PROTO(name) XN_DEVICE_PROXY_PROTO_APPEND(XnDeviceProxy, name)
#define XN_DEVICE_INTERFACE_FUNCTION(name, sig)	XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(name)sig;
#include <XnDeviceProto.inl>
#undef XN_DEVICE_INTERFACE_FUNCTION

/****************************/
/* Specific Proxy Functions */
/****************************/

/**
* Gets a list of supported devices, meaning, devices loaded by device manager.
*
* @param	aDeviceDefinitions	[in]		An array of XnDeviceDefinition to be filled with information.
* @param	pnCount				[in/out]	In: the size of the array. Out: the number of elements filled in the array.
*/
XN_DDK_API XnStatus XnDeviceProxyGetDeviceList(XnDeviceDefinition* aDeviceDefinitions, XnUInt32* pnCount);

/**
* Enumerates a specific device, by device name. 
*
* @param	csDeviceName			[in]		The name of the device to enumerate.
* @param	aConnectionStrings		[in]		An array to be filled with connection strings.
* @param	pnCount					[in/out]	In: the size of the array. Out: the number of elements filled in the array.
*/
XN_DDK_API XnStatus XnDeviceProxyEnumerateDeviceByName(const XnChar* csDeviceName, XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

/**
* Creates a device by name.
*
* @param	csDeviceName			[in]		The name of the device to create. The special value "Auto" will create any available device.
* @param	pDeviceHandle			[out]		The opened device handle. If the function fails, NULL is returned.
* @param	pDeviceConfig			[in]		The requested device configuration mode. Contains the mode (read/write) and the target connection string.
*/
XN_DDK_API XnStatus XnDeviceProxyCreateDeviceByName(const XnChar* csDeviceName, XnDeviceHandle* pDeviceHandle, const XnDeviceConfig* pDeviceConfig);

/**
* Creates a device by definitions in INI file.
*
* @param	strIniFileName			[in]		INI file to use for initialization.
* @param	strSectionName			[in]		section name in INI file that describes the device.
* @param	pDeviceHandle			[out]		The opened device handle. If the function fails, NULL is returned.
* @param	pInitialValues			[in]		Optional. A set of initial values to be used.
*/
XN_DDK_API XnStatus XnDeviceProxyCreateDeviceByINIFile(const XnChar* strIniFileName, const XnChar* strSectionName, XnDeviceHandle* pDeviceHandle, const XnPropertySet* pInitialValues);

/**
* Destroys a stream output object that was previously created using CreateStreamOutput.
*
* @param	csDeviceName	[in]	The name of the device that created this object.
* @param	ppStreamOutput	[in]	The stream output object to destroy.
*/
XN_DDK_API XnStatus XnDeviceProxyDestroyStreamOutputByName(const XnChar* csDeviceName, XnStreamData** ppStreamOutput);

/**
* Gets the name of an opened device.
*
* @param	DeviceHandle	[in]		The requested device handle.
* @param	csDeviceName	[in/out]	A string to be filled with its name. The buffer must be at least XN_DEVICE_MAX_STRING_LENGTH long.
*/
XN_DDK_API XnStatus XnDeviceProxyGetDeviceName(XnDeviceHandle DeviceHandle, XnChar* csDeviceName);

#endif //__XN_DEVICE_PROXY_H__