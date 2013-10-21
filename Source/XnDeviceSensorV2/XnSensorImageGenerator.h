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
#ifndef __XN_SENSOR_IMAGE_GENERATOR_H__
#define __XN_SENSOR_IMAGE_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorMapGenerator.h"
#include "XnExportedSensorGenerator.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
// disable the "inherits via dominance" warning. This is exactly what we want.
#pragma warning (push)
#pragma warning (disable: 4250)

class XnSensorImageGenerator:
	public XnSensorMapGenerator,
	virtual public xn::ModuleImageGenerator,
	virtual public xn::ModuleGeneralIntInterface,
	virtual public xn::ModuleAntiFlickerInterface
{
public:
	XnSensorImageGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
	
	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	const void* GetData() { return XnSensorMapGenerator::GetData(); }
	XnUInt8* GetImageMap();
	XnBool IsPixelFormatSupported(XnPixelFormat Format);
	XnStatus SetPixelFormat(XnPixelFormat Format);
	XnPixelFormat GetPixelFormat();
	XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback);

	virtual xn::ModuleGeneralIntInterface* GetGeneralIntInterface(const XnChar* strCap);
	virtual XnStatus GetRange(const XnChar* strCap, XnInt32& nMin, XnInt32& nMax, XnInt32& nStep, XnInt32& nDefault, XnBool& bIsAutoSupported);
	virtual XnStatus Get(const XnChar* strCap, XnInt32& nValue);
	virtual XnInt32 Set(const XnChar* strCap, XnInt32 nValue);
	virtual XnStatus RegisterToValueChange(const XnChar* strCap, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromValueChange(const XnChar* strCap, XnCallbackHandle hCallback);

	virtual xn::ModuleAntiFlickerInterface* GetAntiFlickerInterface() { return this; }
	virtual XnPowerLineFrequency GetPowerLineFrequency();
	virtual XnStatus SetPowerLineFrequency( XnPowerLineFrequency nFrequency );
	virtual XnStatus RegisterToPowerLineFrequencyChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback );
	virtual void UnregisterFromPowerLineFrequencyChange( XnCallbackHandle hCallback );

protected:
	virtual void FilterProperties(XnActualPropertiesHash* pHash);

private:
	XnUInt32 FindSupportedInputFormat(XnUInt32* anAllowedInputFormats, XnUInt32 nAllowedInputFormats);
};

class XnExportedSensorImageGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorImageGenerator();

protected:
	virtual XnStatus IsSupportedForDevice(xn::Context& context, xn::NodeInfo& sensorInfo, XnBool* pbSupported);
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};

#pragma warning (pop)

#endif // __XN_SENSOR_IMAGE_GENERATOR_H__