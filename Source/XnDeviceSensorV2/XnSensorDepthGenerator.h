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
#ifndef __XN_SENSOR_DEPTH_GENERATOR_H__
#define __XN_SENSOR_DEPTH_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorMapGenerator.h"
#include "XnExportedSensorGenerator.h"
#include <XnEventT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
// disable the "inherits via dominance" warning. This is exactly what we want.
#pragma warning (push)
#pragma warning (disable: 4250)

class XnSensorDepthGenerator : 
	public XnSensorMapGenerator, 
	virtual public xn::ModuleDepthGenerator,
	virtual public xn::ModuleUserPositionInterface,
	virtual public xn::ModuleAlternativeViewPointInterface,
	virtual public xn::ModuleFrameSyncInterface
{
public:
	XnSensorDepthGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
	~XnSensorDepthGenerator();

	XnStatus Init();

	XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

	const void* GetData() { return XnSensorMapGenerator::GetData(); }
	XnDepthPixel* GetDepthMap();
	XnDepthPixel GetDeviceMaxDepth();
	void GetFieldOfView(XnFieldOfView& FOV);
	XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback);

	xn::ModuleUserPositionInterface* GetUserPositionInterface() { return this; }
	XnUInt32 GetSupportedUserPositionsCount();
	XnStatus SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position);
	XnStatus GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position);
	XnStatus RegisterToUserPositionChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromUserPositionChange(XnCallbackHandle hCallback);

	xn::ModuleAlternativeViewPointInterface* GetAlternativeViewPointInterface() { return this; }
	XnBool IsViewPointSupported(xn::ProductionNode& OtherNode);
	XnStatus SetViewPoint(xn::ProductionNode& OtherNode);
	XnStatus ResetViewPoint();
	XnBool IsViewPointAs(xn::ProductionNode& OtherNode);
	XnStatus RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromViewPointChange(XnCallbackHandle hCallback);
	XnStatus GetPixelCoordinatesInViewPoint(xn::ProductionNode& other, XnUInt32 x, XnUInt32 y, XnUInt32& altX, XnUInt32& altY);

	xn::ModuleFrameSyncInterface* GetFrameSyncInterface() { return this; }
	XnBool CanFrameSyncWith(xn::ProductionNode& OtherNode);
	XnStatus FrameSyncWith(xn::ProductionNode& OtherNode);
	XnStatus StopFrameSyncWith(xn::ProductionNode& OtherNode);
	XnBool IsFrameSyncedWith(xn::ProductionNode& OtherNode);
	XnStatus RegisterToFrameSyncChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback);

protected:
	virtual void FilterProperties(XnActualPropertiesHash* pHash);
	
private:
	XnStatus UpdateRealWorldTranslationData();
	XnBool IsSensorImageNode(xn::ProductionNode& Other);

	static void XN_CALLBACK_TYPE RealWorldTranslationPropChanged(void* pCookie);

	XnEventNoArgs m_fovChangedEvent;

	XnCallbackHandle m_hRWPropCallback;

	XnFieldOfView m_FOV;
};

class XnExportedSensorDepthGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorDepthGenerator();
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};

#pragma warning (pop)

#endif // __XN_SENSOR_DEPTH_GENERATOR_H__