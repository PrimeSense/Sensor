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






#ifndef __XN_SENSOR_GENERATOR_H__
#define __XN_SENSOR_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorProductionNode.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
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

	static void XN_CALLBACK_TYPE OnDeviceNewStreamData(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, void* pCookie);
};

#endif // __XN_SENSOR_GENERATOR_H__