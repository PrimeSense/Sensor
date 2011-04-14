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






#ifndef __XN_SENSOR_IR_GENERATOR_H__
#define __XN_SENSOR_IR_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorMapGenerator.h"
#include "XnExportedSensorGenerator.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorIRGenerator:
	public XnSensorMapGenerator,
	virtual public xn::ModuleIRGenerator
{
public:
	XnSensorIRGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);

	virtual XnStatus Init();

	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnUInt32 GetDataSize();

	const void* GetData() { return XnSensorMapGenerator::GetData(); }
	virtual XnIRPixel* GetIRMap();

private:
	void OnResChanged();

	static void XN_CALLBACK_TYPE OnResChangedCallback(void* pCookie);

	XnCallbackHandle m_hMapModeCallback;
	XnCallbackHandle m_hCroppingCallback;
	XnUInt32 m_nBufferSize;
};

class XnExportedSensorIRGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorIRGenerator();
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};

#endif // __XN_SENSOR_IR_GENERATOR_H__