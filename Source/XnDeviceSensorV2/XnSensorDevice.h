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
#ifndef __XN_SENSOR_DEVICE_H__
#define __XN_SENSOR_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorProductionNode.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
// disable the "inherits via dominance" warning. This is exactly what we want.
#pragma warning (push)
#pragma warning (disable: 4250)

class XnSensorDevice : 
	public XnSensorProductionNode,
	virtual public xn::ModuleDevice,
	virtual public xn::ModuleErrorStateInterface,
	virtual public xn::ModuleDeviceIdentificationInterface
{
public:
	XnSensorDevice(xn::Context& context, XnDeviceBase* pSensor, const XnChar* strInstanceName);
	virtual ~XnSensorDevice();

	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	virtual ModuleErrorStateInterface* GetErrorStateInterface() { return this; }
	virtual XnStatus GetErrorState();
	virtual XnStatus RegisterToErrorStateChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromErrorStateChange(XnCallbackHandle hCallback);

	virtual ModuleDeviceIdentificationInterface* GetIdentificationInterface() { return this; }
	virtual XnStatus GetDeviceName(XnChar* strBuffer, XnUInt32& nBufferSize);
	virtual XnStatus GetVendorSpecificData(XnChar* strBuffer, XnUInt32& nBufferSize);
	virtual XnStatus GetSerialNumber(XnChar* strBuffer, XnUInt32& nBufferSize);
};

#pragma warning (pop)

#endif // __XN_SENSOR_DEVICE_H__