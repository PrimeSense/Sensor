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
};

class XnExportedSensorImageGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorImageGenerator();
	virtual XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};

#endif // __XN_SENSOR_IMAGE_GENERATOR_H__