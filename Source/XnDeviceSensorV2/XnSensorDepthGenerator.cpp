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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorDepthGenerator.h"
#include <XnOpenNI.h>
#include <XnOS.h>
#include <XnPsVersion.h>
#include <XnCommon/XnCommon.h>
#include <math.h>

//---------------------------------------------------------------------------
// XnSensorDepthGenerator class
//---------------------------------------------------------------------------

XnSensorDepthGenerator::XnSensorDepthGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorMapGenerator(context, sensor, pSensor, strStreamName),
	m_hRWPropCallback(NULL)
{
}

XnSensorDepthGenerator::~XnSensorDepthGenerator()
{
	if (m_hRWPropCallback != NULL)
	{
		UnregisterFromProps(m_hRWPropCallback);
	}
}

XnStatus XnSensorDepthGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorMapGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,
		XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE,
		NULL
	};

	m_hRWPropCallback;
	nRetVal = RegisterToProps(RealWorldTranslationPropChanged, this, m_hRWPropCallback, aProps);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = UpdateRealWorldTranslationData();
	if (nRetVal != XN_STATUS_OK)
	{
		UnregisterFromProps(m_hRWPropCallback);
		m_hRWPropCallback = NULL;
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnBool XnSensorDepthGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return (strcmp(strCapabilityName, XN_CAPABILITY_USER_POSITION) == 0 ||
		strcmp(strCapabilityName, XN_CAPABILITY_ALTERNATIVE_VIEW_POINT) == 0 ||
		strcmp(strCapabilityName, XN_CAPABILITY_FRAME_SYNC) == 0 ||
		XnSensorMapGenerator::IsCapabilitySupported(strCapabilityName));
}

XnDepthPixel* XnSensorDepthGenerator::GetDepthMap()
{
	return (XnDepthPixel*)m_pStreamData->pData;
}

XnDepthPixel XnSensorDepthGenerator::GetDeviceMaxDepth()
{
	XnUInt64 nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, &nValue);
	return (XnDepthPixel)nValue;
}

void XnSensorDepthGenerator::GetFieldOfView(XnFieldOfView& FOV)
{
	FOV = m_FOV;
}

XnStatus XnSensorDepthGenerator::RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_fovChangedEvent.Register(handler, pCookie, &hCallback);
}

void XnSensorDepthGenerator::UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
{
	m_fovChangedEvent.Unregister(hCallback);
}

XnStatus XnSensorDepthGenerator::UpdateRealWorldTranslationData()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt64 nZPD;
	nRetVal = GetIntProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, nZPD);
	XN_IS_STATUS_OK(nRetVal);

	XnDouble fZPPS;
	nRetVal = GetRealProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, fZPPS);
	XN_IS_STATUS_OK(nRetVal);

	m_FOV.fHFOV = 2*atan(fZPPS*XN_SXGA_X_RES/2/nZPD);
	m_FOV.fVFOV = 2*atan(fZPPS*XN_VGA_Y_RES*2/2/nZPD);

	nRetVal = m_fovChangedEvent.Raise();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnSensorDepthGenerator::RealWorldTranslationPropChanged(void* pCookie)
{
	XnSensorDepthGenerator* pThis = (XnSensorDepthGenerator*)pCookie;
	pThis->UpdateRealWorldTranslationData();
}

XnUInt32 XnSensorDepthGenerator::GetSupportedUserPositionsCount()
{
	return 4;
}

XnStatus XnSensorDepthGenerator::SetUserPosition(XnUInt32 nIndex, const XnBoundingBox3D& Position)
{
	// set (we only support Z boxing for now)
	XnDepthAGCBin bin;
	bin.nBin = nIndex;
	bin.nMin = Position.LeftBottomNear.Z;
	bin.nMax = Position.RightTopFar.Z;
	return m_pSensor->SetProperty(m_strModule, XN_STREAM_PROPERTY_AGC_BIN, XN_PACK_GENERAL_BUFFER(bin));
}

XnStatus XnSensorDepthGenerator::GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get
	XnDepthAGCBin bin;
	bin.nBin = nIndex;
	nRetVal =  m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_AGC_BIN, XN_PACK_GENERAL_BUFFER(bin));
	XN_IS_STATUS_OK(nRetVal);

	XnMapOutputMode MapOutputMode;
	nRetVal = GetMapOutputMode(MapOutputMode);
	XN_IS_STATUS_OK(nRetVal);

	// we only support Z position for now
	Position.LeftBottomNear.Z = bin.nMin;
	Position.RightTopFar.Z = bin.nMax;
	Position.LeftBottomNear.X = 0;
	Position.RightTopFar.X = MapOutputMode.nXRes - 1;
	Position.LeftBottomNear.Y = 0;
	Position.RightTopFar.Y = MapOutputMode.nYRes - 1;

	return (XN_STATUS_OK);
}

XnStatus XnSensorDepthGenerator::RegisterToUserPositionChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_AGC_BIN,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorDepthGenerator::UnregisterFromUserPositionChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

XnBool XnSensorDepthGenerator::IsSensorImageNode(xn::ProductionNode& OtherNode)
{
	xn::NodeInfo info = OtherNode.GetInfo();

	XnVersion Version;
	Version.nMajor = XN_PS_MAJOR_VERSION;
	Version.nMinor = XN_PS_MINOR_VERSION;
	Version.nMaintenance = XN_PS_MAINTENANCE_VERSION;
	Version.nBuild = XN_PS_BUILD_VERSION;

	// check if this view point is image from this DLL
	if (info.GetDescription().Type != XN_NODE_TYPE_IMAGE ||
		strcmp(info.GetDescription().strName, XN_DEVICE_NAME) != 0 ||
		strcmp(info.GetDescription().strVendor, XN_VENDOR_PRIMESENSE) != 0 ||
		xnVersionCompare(&info.GetDescription().Version, &Version) != 0)
	{
		return FALSE;
	}

	// check if it uses the same device
	xn::NodeInfoList needed = info.GetNeededNodes();
	for (xn::NodeInfoList::Iterator it = needed.Begin(); it != needed.End(); ++it)
	{
		if ((*it).GetDescription().Type == XN_NODE_TYPE_DEVICE &&
			strcmp((*it).GetCreationInfo(), m_device.GetInfo().GetCreationInfo()) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

XnBool XnSensorDepthGenerator::IsViewPointSupported(xn::ProductionNode& OtherNode)
{
	return IsSensorImageNode(OtherNode);
}

XnStatus XnSensorDepthGenerator::SetViewPoint(xn::ProductionNode& OtherNode)
{
	if (IsSensorImageNode(OtherNode))
	{
		return SetIntProperty(XN_STREAM_PROPERTY_REGISTRATION, TRUE);
	}
	else
	{
		return XN_STATUS_BAD_PARAM;
	}
}

XnStatus XnSensorDepthGenerator::ResetViewPoint()
{
	return SetIntProperty(XN_STREAM_PROPERTY_REGISTRATION, FALSE);
}

XnBool XnSensorDepthGenerator::IsViewPointAs(xn::ProductionNode& OtherNode)
{
	XnUInt64 nValue = FALSE;
	GetIntProperty(XN_STREAM_PROPERTY_REGISTRATION, nValue);

	return (nValue == TRUE && IsSensorImageNode(OtherNode));
}

XnStatus XnSensorDepthGenerator::RegisterToViewPointChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_REGISTRATION,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorDepthGenerator::UnregisterFromViewPointChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

XnBool XnSensorDepthGenerator::CanFrameSyncWith(xn::ProductionNode& OtherNode)
{
	return (IsSensorImageNode(OtherNode));
}

XnStatus XnSensorDepthGenerator::FrameSyncWith(xn::ProductionNode& OtherNode)
{
	if (IsSensorImageNode(OtherNode))
	{
		return m_pSensor->SetProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_FRAME_SYNC, (XnUInt64)TRUE);
	}
	else
	{
		return XN_STATUS_BAD_PARAM;
	}
}

XnStatus XnSensorDepthGenerator::StopFrameSyncWith(xn::ProductionNode& OtherNode)
{
	return m_pSensor->SetProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_FRAME_SYNC, (XnUInt64)FALSE);
}

XnBool XnSensorDepthGenerator::IsFrameSyncedWith(xn::ProductionNode& OtherNode)
{
	XnUInt64 nValue = FALSE;
	m_pSensor->GetProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_FRAME_SYNC, &nValue);

	return (nValue == TRUE && IsSensorImageNode(OtherNode));
}

XnStatus XnSensorDepthGenerator::RegisterToFrameSyncChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_MODULE_PROPERTY_FRAME_SYNC,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps, XN_MODULE_NAME_DEVICE);
}

void XnSensorDepthGenerator::UnregisterFromFrameSyncChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorDepthGenerator::FilterProperties(XnActualPropertiesHash* pHash)
{
	XnSensorMapGenerator::FilterProperties(pHash);
	pHash->Remove(XN_STREAM_PROPERTY_REGISTRATION);
	pHash->Remove(XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH);
}

//---------------------------------------------------------------------------
// XnExportedSensorDepthGenerator class
//---------------------------------------------------------------------------
XnExportedSensorDepthGenerator::XnExportedSensorDepthGenerator() :
	XnExportedSensorGenerator(XN_NODE_TYPE_DEPTH, XN_STREAM_TYPE_DEPTH)
{}

XnSensorGenerator* XnExportedSensorDepthGenerator::CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName)
{
	return XN_NEW(XnSensorDepthGenerator, context, sensor, pSensor, strStreamName);
}
