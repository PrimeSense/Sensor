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






#ifndef __XN_SENSOR_MAP_GENERATOR_H__
#define __XN_SENSOR_MAP_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorGenerator.h"
#include <XnList.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorMapGenerator : 
	public XnSensorGenerator, 
	virtual public xn::ModuleMapGenerator,
	virtual public xn::ModuleCroppingInterface
{
public:
	XnSensorMapGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);

	XnStatus Init();

	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	XnUInt32 GetSupportedMapOutputModesCount();
	XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount);

	XnStatus SetMapOutputMode(const XnMapOutputMode& Mode);
	XnStatus GetMapOutputMode(XnMapOutputMode& Mode);
	XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback);

	xn::ModuleCroppingInterface* GetCroppingInterface() { return this; }
	XnStatus SetCropping(const XnCropping &Cropping);
	XnStatus GetCropping(XnCropping &Cropping);
	XnStatus RegisterToCroppingChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromCroppingChange(XnCallbackHandle hCallback);

protected:
	virtual void FilterProperties(XnActualPropertiesHash* pHash);

	struct SupportedMode
	{
		XnMapOutputMode OutputMode;
		XnUInt32 nInputFormat;
	};

	SupportedMode* m_aSupportedModes;
	XnUInt32 m_nSupportedModesCount;
};

#endif // __XN_SENSOR_MAP_GENERATOR_H__