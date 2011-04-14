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






#ifndef __XN_SENSOR_DEVICE_H__
#define __XN_SENSOR_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorProductionNode.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
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

#endif // __XN_SENSOR_DEVICE_H__