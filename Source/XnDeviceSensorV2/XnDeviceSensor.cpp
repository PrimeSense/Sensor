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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnVersion.h>
#include "XnDeviceSensor.h"
#include "XnDeviceSensorInit.h"
#include "XnDeviceSensorIO.h"
#include "XnDeviceSensorProtocol.h"
#include <XnDevice.h>
#include <XnLog.h>
#include "XnSensor.h"
#include "XnSensorClient.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

// Export XnDeviceSensor implementation for device interface
#define XN_DEVICE_BASE_DERIVATIVE XnDeviceSensor
#include <XnDDK/XnDeviceInterfaceImpl.h>

// The following line is needed to be once in *ALL* of the high level shared library modules. DO NOT REMOVE!!! 
XN_API_EXPORT_INIT()

XnDeviceSensor::XnDeviceSensor() : XnDeviceBaseProxy(NULL)
{

}

XnDeviceSensor::~XnDeviceSensor()
{

}

XnStatus XnDeviceSensor::GetDefinition(XnDeviceDefinition* pDeviceDefinition)
{
	return XnSensor::GetDefinition(pDeviceDefinition);
}

XnStatus XnDeviceSensor::Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	return XnSensor::Enumerate(aConnectionStrings, pnCount);
}

XnStatus XnDeviceSensor::Init(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDeviceConfig);

	XnDeviceBase* pActualDevice = NULL;

	switch (pDeviceConfig->SharingMode)
	{
	case XN_DEVICE_EXCLUSIVE:
		XN_VALIDATE_NEW(pActualDevice, XnSensor);
		break;
	case XN_DEVICE_SHARED:
#if (XN_PLATFORM != XN_PLATFORM_WIN32)
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_DEVICE_INVALID_SHARING, XN_MASK_DEVICE_SENSOR, "Sensor sharing is only supported under win32!");
#endif
		XN_VALIDATE_NEW(pActualDevice, XnSensorClient);
		break;
	default:
		return XN_STATUS_IO_DEVICE_INVALID_SHARING;
	}

	// init actual device
	nRetVal = pActualDevice->Init(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	ReplaceActualDevice(pActualDevice);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensor::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// destroy actual
	nRetVal = XnDeviceBaseProxy::Destroy();
	XN_IS_STATUS_OK(nRetVal);

	IXnDevice* pActual = GetActual();

	ReplaceActualDevice(NULL);

	XN_DELETE(pActual);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensor::DestroyStreamData(XnStreamData** ppStreamData)
{
	return XnSensor::DestroyStreamData(ppStreamData);
}
