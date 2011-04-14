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






#ifndef __XN_SENSOR_DEPTH_GENERATOR_H__
#define __XN_SENSOR_DEPTH_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorMapGenerator.h"
#include "XnExportedSensorGenerator.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
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

	XN_DECLARE_EVENT_0ARG(PropChangeEvent, PropChangeEventInterface);
	PropChangeEvent m_fovChangedEvent;

	XnCallbackHandle m_hRWPropCallback;

	XnFieldOfView m_FOV;
};

class XnExportedSensorDepthGenerator : public XnExportedSensorGenerator
{
public:
	XnExportedSensorDepthGenerator();
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName);
};


#endif // __XN_SENSOR_DEPTH_GENERATOR_H__