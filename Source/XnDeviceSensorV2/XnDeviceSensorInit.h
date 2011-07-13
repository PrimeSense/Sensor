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






#ifndef _XN_DEVICESENSORINIT_H_
#define _XN_DEVICESENSORINIT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"
#include "XnDeviceSensorProtocol.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

#if XN_PLATFORM == XN_PLATFORM_WIN32
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_ISO				8*10
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_BULK				120
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		8*5
	#define XN_SENSOR_USB_IMAGE_BUFFERS									8

	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_ISO				8*10
	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_BULK				120
	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		8*5
	#define XN_SENSOR_USB_DEPTH_BUFFERS									8

	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_ISO				104
	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_BULK				20
	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		52
	#define XN_SENSOR_USB_MISC_BUFFERS									8
#elif XN_PLATFORM == XN_PLATFORM_PS3
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_ISO		0x1E000
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_BULK	0x4000
	#define XN_SENSOR_USB_IMAGE_BUFFERS		1

	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE	0x4000
	#define XN_SENSOR_USB_DEPTH_BUFFERS		2

	#define XN_SENSOR_USB_MISC_BUFFER_SIZE	0x1000
	#define XN_SENSOR_USB_MISC_BUFFERS		1
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_ISO				32
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_BULK				40
	#define XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		16
	#define XN_SENSOR_USB_IMAGE_BUFFERS									16

	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_ISO				32
	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_BULK				40
	#define XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		16
	#define XN_SENSOR_USB_DEPTH_BUFFERS									16

	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_ISO				104
	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_BULK				20
	#define XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO		52
	#define XN_SENSOR_USB_MISC_BUFFERS									5
#endif

#define XN_SENSOR_READ_THREAD_TIMEOUT_ISO	100
#define XN_SENSOR_READ_THREAD_TIMEOUT_BULK	1000

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
XnStatus XnDeviceSensorInit(XnDevicePrivateData* pDevicePrivateData);

XnStatus XnDeviceSensorAllocateBuffers(XnDevicePrivateData* pDevicePrivateData);
XnStatus XnDeviceSensorFreeBuffers(XnDevicePrivateData* pDevicePrivateData);

XnStatus XnDeviceSensorConfigureVersion(XnDevicePrivateData* pDevicePrivateData);

XnStatus XnDeviceSensorOpenInputThreads(XnDevicePrivateData* pDevicePrivateData, XnBool bOpen1, XnBool bOpen2, XnBool bOpen3);

XnStatus XnDeviceSensorConfigure(XnDevicePrivateData* pDevicePrivateData);

XnStatus XnDeviceSensorInitCmosData(XnDevicePrivateData* pDevicePrivateData);

#endif //_XN_DEVICESENSORINIT_H_
