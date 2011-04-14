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
#include "XnSensorDevice.h"

//---------------------------------------------------------------------------
// XnSensorDevice class
//---------------------------------------------------------------------------
XnSensorDevice::XnSensorDevice(xn::Context& context, XnDeviceBase* pSensor, const XnChar* strInstanceName) :
	XnSensorProductionNode(context, strInstanceName, pSensor, XN_MODULE_NAME_DEVICE)
{
}

XnSensorDevice::~XnSensorDevice()
{
}

XnBool XnSensorDevice::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return (
		strcmp(strCapabilityName, XN_CAPABILITY_ERROR_STATE) == 0 ||
		strcmp(strCapabilityName, XN_CAPABILITY_DEVICE_IDENTIFICATION) == 0 ||
		XnSensorProductionNode::IsCapabilitySupported(strCapabilityName));
}

XnStatus XnSensorDevice::GetErrorState()
{
	XnUInt64 nValue = XN_STATUS_OK;

	XnStatus nRetVal = GetIntProperty(XN_MODULE_PROPERTY_ERROR_STATE, nValue);
	XN_ASSERT(nRetVal == XN_STATUS_OK);

	return (XnStatus)nValue;
}

XnStatus XnSensorDevice::RegisterToErrorStateChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_MODULE_PROPERTY_ERROR_STATE,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorDevice::UnregisterFromErrorStateChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

XnStatus XnSensorDevice::GetDeviceName(XnChar* strBuffer, XnUInt32& nBufferSize)
{
	return GetStringProperty(XN_MODULE_PROPERTY_PHYSICAL_DEVICE_NAME, strBuffer, nBufferSize);
}

XnStatus XnSensorDevice::GetVendorSpecificData(XnChar* strBuffer, XnUInt32& nBufferSize)
{
	return GetStringProperty(XN_MODULE_PROPERTY_VENDOR_SPECIFIC_DATA, strBuffer, nBufferSize);
}

XnStatus XnSensorDevice::GetSerialNumber(XnChar* strBuffer, XnUInt32& nBufferSize)
{
	return GetStringProperty(XN_MODULE_PROPERTY_ID, strBuffer, nBufferSize);
}