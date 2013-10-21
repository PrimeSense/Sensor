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