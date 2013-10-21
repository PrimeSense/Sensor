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
// disable the "inherits via dominance" warning. This is exactly what we want.
#pragma warning (push)
#pragma warning (disable: 4250)

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

#pragma warning (pop)

#endif // __XN_SENSOR_IR_GENERATOR_H__