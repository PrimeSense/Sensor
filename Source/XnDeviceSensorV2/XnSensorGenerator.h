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
#ifndef __XN_SENSOR_GENERATOR_H__
#define __XN_SENSOR_GENERATOR_H__

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

class XnSensorGenerator : 
	public XnSensorProductionNode, 
	virtual public xn::ModuleGenerator,
	virtual public xn::ModuleMirrorInterface
{
public:
	XnSensorGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
	~XnSensorGenerator();

	XnStatus Init();

	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	XnStatus StartGenerating();
	XnBool IsGenerating();
	void StopGenerating();
	XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback);
	XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback);
	XnBool IsNewDataAvailable(XnUInt64& pnTimestamp);
	XnStatus UpdateData();
	const void* GetData();
	XnUInt32 GetDataSize();
	XnUInt64 GetTimestamp();
	XnUInt32 GetFrameID();

	xn::ModuleMirrorInterface* GetMirrorInterface() { return this; }
	XnStatus SetMirror(XnBool bMirror);
	XnBool IsMirrored();
	XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromMirrorChange(XnCallbackHandle hCallback);

protected:
	virtual void FilterProperties(XnActualPropertiesHash* pHash);

	XnStreamData* m_pStreamData;
	XnVersions m_Version;
	xn::Device m_device;

private:
	struct NewDataCallback
	{
		NewDataCallback(XnSensorGenerator *pGenerator, XnModuleStateChangedHandler handler, void *pCookie) :
			m_pGenerator(pGenerator), m_handler(handler), m_pCookie(pCookie), m_hCallback(NULL) {}
		
		XnSensorGenerator *m_pGenerator;
		XnModuleStateChangedHandler m_handler;
		void *m_pCookie;
		XnCallbackHandle m_hCallback;
	};

	static void XN_CALLBACK_TYPE OnDeviceNewStreamData(const XnNewStreamDataEventArgs& args, void* pCookie);
};

#pragma warning (pop)

#endif // __XN_SENSOR_GENERATOR_H__