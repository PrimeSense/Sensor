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
#ifndef __XN_SENSOR_AUDIO_GENERATOR_H__
#define __XN_SENSOR_AUDIO_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorGenerator.h"
#include "XnExportedSensorGenerator.h"
#include <XnListT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
// disable the "inherits via dominance" warning. This is exactly what we want.
#pragma warning (push)
#pragma warning (disable: 4250)

class XnSensorAudioGenerator : 
	public XnSensorGenerator,
	virtual public xn::ModuleAudioGenerator
{
public:
	XnSensorAudioGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);

	XnStatus Init();

	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	const void* GetData() { return XnSensorGenerator::GetData(); }
	XnUChar* GetAudioBuffer();
	XnUInt32 GetSupportedWaveOutputModesCount();
	XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount);
	XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode);
	XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode);
	XnStatus RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback);

protected:
	virtual void FilterProperties(XnActualPropertiesHash* pHash);

private:
	typedef XnListT<XnWaveOutputMode> XnWaveOutputModeList;

	XnWaveOutputModeList m_SupportedModes;
};

class XnExportedSensorAudioGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorAudioGenerator();

protected:
	virtual XnStatus IsSupportedForDevice(xn::Context& context, xn::NodeInfo& sensorInfo, XnBool* pbSupported);
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};

#pragma warning (pop)

#endif // __XN_SENSOR_AUDIO_GENERATOR_H__