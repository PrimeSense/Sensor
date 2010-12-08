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
