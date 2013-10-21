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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorDepthGenerator.h"
#include <XnOpenNI.h>
#include <XnOS.h>
#include <XnPsVersion.h>
#include <XnCommon.h>
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
	return m_fovChangedEvent.Register(handler, pCookie, hCallback);
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
	bin.nBin = (XnUInt16)nIndex;
	bin.nMin = (XnUInt16)Position.LeftBottomNear.Z;
	bin.nMax = (XnUInt16)Position.RightTopFar.Z;
	return m_pSensor->SetProperty(m_strModule, XN_STREAM_PROPERTY_AGC_BIN, XN_PACK_GENERAL_BUFFER(bin));
}

XnStatus XnSensorDepthGenerator::GetUserPosition(XnUInt32 nIndex, XnBoundingBox3D& Position)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get
	XnDepthAGCBin bin;
	bin.nBin = (XnUInt16)nIndex;
	nRetVal =  m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_AGC_BIN, XN_PACK_GENERAL_BUFFER(bin));
	XN_IS_STATUS_OK(nRetVal);

	XnMapOutputMode MapOutputMode;
	nRetVal = GetMapOutputMode(MapOutputMode);
	XN_IS_STATUS_OK(nRetVal);

	// we only support Z position for now
	Position.LeftBottomNear.Z = bin.nMin;
	Position.RightTopFar.Z = bin.nMax;
	Position.LeftBottomNear.X = 0;
	Position.RightTopFar.X = (XnFloat)(MapOutputMode.nXRes - 1);
	Position.LeftBottomNear.Y = 0;
	Position.RightTopFar.Y = (XnFloat)(MapOutputMode.nYRes - 1);

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

XnStatus XnSensorDepthGenerator::GetPixelCoordinatesInViewPoint(xn::ProductionNode& other, XnUInt32 x, XnUInt32 y, XnUInt32& altX, XnUInt32& altY)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!IsSensorImageNode(other))
	{
		return XN_STATUS_BAD_PARAM;
	}

	xn::MapGenerator image(other);
	XnMapOutputMode imageMode;
	nRetVal = image.GetMapOutputMode(imageMode);
	XN_IS_STATUS_OK(nRetVal);

	XnDepthPixel* pDepthMap = GetDepthMap();
	XnMapOutputMode mode;
	GetMapOutputMode(mode);

	if (x >= mode.nXRes || y >= mode.nYRes)
	{
		return XN_STATUS_BAD_PARAM;
	}

	XnPixelRegistration args;
	args.nDepthX = x;
	args.nDepthY = y;
	args.nDepthValue = pDepthMap[mode.nXRes * y + x];
	args.nImageXRes = imageMode.nXRes;
	args.nImageYRes = imageMode.nYRes;

	nRetVal = GetGeneralProperty(XN_STREAM_PROPERTY_PIXEL_REGISTRATION, sizeof(args), &args);
	XN_IS_STATUS_OK(nRetVal);

	altX = args.nImageX;
	altY = args.nImageY;

	return (XN_STATUS_OK);
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

XnStatus XnSensorDepthGenerator::StopFrameSyncWith(xn::ProductionNode& /*OtherNode*/)
{
	// we assume the other node is the image one (this is the only one we started)
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
