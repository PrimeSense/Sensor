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
#include "XnSensorImageGenerator.h"
#include <XnOpenNI.h>

//---------------------------------------------------------------------------
// XnSensorImageGenerator class
//---------------------------------------------------------------------------
XnSensorImageGenerator::XnSensorImageGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorMapGenerator(context, sensor, pSensor, strStreamName)
{}

XnStatus XnSensorImageGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorMapGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	// add supported modes unique for image
	XnMapOutputMode mode;

	switch (m_Version.FWVer)
	{
	case XN_SENSOR_FW_VER_5_1:
	case XN_SENSOR_FW_VER_5_2:
		// UXGA 30
		mode.nXRes = XN_UXGA_X_RES;
		mode.nYRes = XN_UXGA_Y_RES;
		mode.nFPS = 30;
		nRetVal = m_SupportedModes.AddLast(mode);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_SENSOR_FW_VER_5_3:
		// SXGA 30
		mode.nXRes = XN_SXGA_X_RES;
		mode.nYRes = XN_SXGA_Y_RES;
		mode.nFPS = 30;
		nRetVal = m_SupportedModes.AddLast(mode);
		XN_IS_STATUS_OK(nRetVal);
		break;
	}
	
	return (XN_STATUS_OK);
}

XnBool XnSensorImageGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return XnSensorMapGenerator::IsCapabilitySupported(strCapabilityName);
}

XnUInt8* XnSensorImageGenerator::GetImageMap()
{
	return (XnUInt8*)m_pStreamData->pData;
}

XnBool XnSensorImageGenerator::IsPixelFormatSupported(XnPixelFormat Format)
{
	switch (Format)
	{
	case XN_PIXEL_FORMAT_RGB24:
	case XN_PIXEL_FORMAT_YUV422:
	case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
		return TRUE;
	default:
		return FALSE;
	}
}

XnStatus XnSensorImageGenerator::SetPixelFormat(XnPixelFormat Format)
{
	if (GetPixelFormat() == Format)
	{
		return (XN_STATUS_OK);
	}

	XnOutputFormats OutputFormat;

	switch (Format)
	{
	case XN_PIXEL_FORMAT_RGB24:
		OutputFormat = XN_OUTPUT_FORMAT_RGB24;
		break;
	case XN_PIXEL_FORMAT_YUV422:
		OutputFormat = XN_OUTPUT_FORMAT_YUV422;
		break;
	case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
		OutputFormat = XN_OUTPUT_FORMAT_GRAYSCALE8;
		break;
	default:
		return XN_STATUS_INVALID_OPERATION;
	}

	return m_pSensor->SetProperty(m_strModule, XN_STREAM_PROPERTY_OUTPUT_FORMAT, (XnUInt64)OutputFormat);
}

XnPixelFormat XnSensorImageGenerator::GetPixelFormat()
{
	XnUInt64 nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_OUTPUT_FORMAT, &nValue);

	switch (nValue)
	{
	case XN_OUTPUT_FORMAT_RGB24:
		return XN_PIXEL_FORMAT_RGB24;
	case XN_OUTPUT_FORMAT_YUV422:
		return XN_PIXEL_FORMAT_YUV422;
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
		return XN_PIXEL_FORMAT_GRAYSCALE_8_BIT;
	default:
		xnLogError(XN_MASK_DEVICE_SENSOR, "Unknown output format: %d", nValue);
		return (XnPixelFormat)-1;
	}
}

XnStatus XnSensorImageGenerator::RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_OUTPUT_FORMAT,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorImageGenerator::UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorImageGenerator::FilterProperties(XnActualPropertiesHash* pHash)
{
	XnSensorMapGenerator::FilterProperties(pHash);
	pHash->Remove(XN_STREAM_PROPERTY_OUTPUT_FORMAT);
}

//---------------------------------------------------------------------------
// XnExportedSensorImageGenerator class
//---------------------------------------------------------------------------
XnExportedSensorImageGenerator::XnExportedSensorImageGenerator() :
	XnExportedSensorGenerator(XN_NODE_TYPE_IMAGE, XN_STREAM_TYPE_IMAGE)
{}

XnSensorGenerator* XnExportedSensorImageGenerator::CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName)
{
	return XN_NEW(XnSensorImageGenerator, context, sensor, pSensor, strStreamName);
}
