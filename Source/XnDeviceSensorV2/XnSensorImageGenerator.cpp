/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorImageGenerator.h"
#include <XnOpenNI.h>
#include "XnSensorImageStream.h"

//---------------------------------------------------------------------------
// Static Data
//---------------------------------------------------------------------------
static const XnUInt32 INVALID_INPUT_FORMAT = 9999;
static XnUInt32 g_anAllowedRGBFormats[] = { XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422, XN_IO_IMAGE_FORMAT_YUV422, XN_IO_IMAGE_FORMAT_BAYER, XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER };
static XnUInt32 g_anAllowedYUVFormats[] = { XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422, XN_IO_IMAGE_FORMAT_YUV422 };
static XnUInt32 g_anAllowedJPEGFormats[] = { XN_IO_IMAGE_FORMAT_JPEG };
static XnUInt32 g_anAllowedGray8Formats[] = { XN_IO_IMAGE_FORMAT_UNCOMPRESSED_GRAY8, XN_IO_IMAGE_FORMAT_BAYER, XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER };

//---------------------------------------------------------------------------
// XnSensorImageGenerator class
//---------------------------------------------------------------------------
XnSensorImageGenerator::XnSensorImageGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorMapGenerator(context, sensor, pSensor, strStreamName)
{}

XnBool XnSensorImageGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return 
		(GetGeneralIntInterface(strCapabilityName) != NULL ||
		strcmp(strCapabilityName, XN_CAPABILITY_ANTI_FLICKER) == 0 ||
		XnSensorMapGenerator::IsCapabilitySupported(strCapabilityName));
}

XnUInt8* XnSensorImageGenerator::GetImageMap()
{
	return (XnUInt8*)m_pStreamData->pData;
}

XnBool XnSensorImageGenerator::IsPixelFormatSupported(XnPixelFormat Format)
{
	for (XnUInt32 i = 0; i < m_nSupportedModesCount; ++i)
	{
		switch (Format)
		{
		case XN_PIXEL_FORMAT_RGB24:
		case XN_PIXEL_FORMAT_YUV422:
			if (m_aSupportedModes[i].nInputFormat == XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422 ||
				m_aSupportedModes[i].nInputFormat == XN_IO_IMAGE_FORMAT_YUV422)
			{
				return TRUE;
			}
			break;
		case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
			if (m_aSupportedModes[i].nInputFormat == XN_IO_IMAGE_FORMAT_BAYER ||
				m_aSupportedModes[i].nInputFormat == XN_IO_IMAGE_FORMAT_UNCOMPRESSED_GRAY8)
			{
				return TRUE;
			}
			break;
		case XN_PIXEL_FORMAT_MJPEG:
			if (m_aSupportedModes[i].nInputFormat == XN_IO_IMAGE_FORMAT_JPEG)
			{
				return TRUE;
			}
			break;
		default:
			return FALSE;
		}
	}

	// not found
	return FALSE;
}

XnUInt32 XnSensorImageGenerator::FindSupportedInputFormat(XnUInt32* anAllowedInputFormats, XnUInt32 nAllowedInputFormats)
{
	// first check if current one is allowed
	XnUInt64 nCurrentInputFormat;
	GetIntProperty(XN_STREAM_PROPERTY_INPUT_FORMAT, nCurrentInputFormat);

	for (XnUInt32 i = 0; i < nAllowedInputFormats; ++i)
	{
		if (anAllowedInputFormats[i] == nCurrentInputFormat)
		{
			return (XnUInt32)nCurrentInputFormat;
		}
	}

	// current one is not allowed. find a matching mode and take its input format
	XnMapOutputMode outputMode;
	GetMapOutputMode(outputMode);

	// the order in the allowed input formats is the preferred one
	for (XnUInt32 iInput = 0; iInput < nAllowedInputFormats; ++iInput)
	{
		// see if such a mode exists
		for (XnUInt32 iMode = 0; iMode < m_nSupportedModesCount; ++iMode)
		{
			if (m_aSupportedModes[iMode].nInputFormat == anAllowedInputFormats[iInput] &&
				m_aSupportedModes[iMode].OutputMode.nXRes == outputMode.nXRes &&
				m_aSupportedModes[iMode].OutputMode.nYRes == outputMode.nYRes &&
				m_aSupportedModes[iMode].OutputMode.nFPS == outputMode.nFPS)
			{
				return anAllowedInputFormats[iInput];
			}
		}
	}

	return INVALID_INPUT_FORMAT;
}

XnStatus XnSensorImageGenerator::SetPixelFormat(XnPixelFormat Format)
{
	if (GetPixelFormat() == Format)
	{
		return (XN_STATUS_OK);
	}

	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	XnStatus nRetVal = XnPropertySetAddModule(&props, m_strModule);
	XN_IS_STATUS_OK(nRetVal);

	XnOutputFormats OutputFormat;
	XnUInt32* anAllowedInputFormats = NULL;
	XnUInt32 nAllowedInputFormats = 0;

	switch (Format)
	{
	case XN_PIXEL_FORMAT_RGB24:
		OutputFormat = XN_OUTPUT_FORMAT_RGB24;
		anAllowedInputFormats = g_anAllowedRGBFormats;
		nAllowedInputFormats = sizeof(g_anAllowedRGBFormats)/sizeof(XnUInt32);
		break;
	case XN_PIXEL_FORMAT_YUV422:
		OutputFormat = XN_OUTPUT_FORMAT_YUV422;
		anAllowedInputFormats = g_anAllowedYUVFormats;
		nAllowedInputFormats = sizeof(g_anAllowedYUVFormats)/sizeof(XnUInt32);
		break;
	case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
		OutputFormat = XN_OUTPUT_FORMAT_GRAYSCALE8;
		anAllowedInputFormats = g_anAllowedGray8Formats;
		nAllowedInputFormats = sizeof(g_anAllowedGray8Formats)/sizeof(XnUInt32);
		break;
	case XN_PIXEL_FORMAT_MJPEG:
		OutputFormat = XN_OUTPUT_FORMAT_JPEG;
		anAllowedInputFormats = g_anAllowedJPEGFormats;
		nAllowedInputFormats = sizeof(g_anAllowedJPEGFormats)/sizeof(XnUInt32);
		break;
	default:
		return XN_STATUS_INVALID_OPERATION;
	}

	XnUInt32 nInputFormat = FindSupportedInputFormat(anAllowedInputFormats, nAllowedInputFormats);
	if (nInputFormat == INVALID_INPUT_FORMAT)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Cannot set pixel format to %s - no matching input format.", xnPixelFormatToString(Format));
	}

	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_INPUT_FORMAT, (XnUInt64)nInputFormat);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_OUTPUT_FORMAT, (XnUInt64)OutputFormat);

	return m_pSensor->BatchConfig(&props);
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
	case XN_OUTPUT_FORMAT_JPEG:
		return XN_PIXEL_FORMAT_MJPEG;
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

xn::ModuleGeneralIntInterface* XnSensorImageGenerator::GetGeneralIntInterface( const XnChar* strCap )
{
	if (m_Version.FWVer < XN_SENSOR_FW_VER_5_4)
	{
		return NULL;
	}

	if (strcmp(strCap, XN_CAPABILITY_BRIGHTNESS) == 0 ||
		strcmp(strCap, XN_CAPABILITY_CONTRAST) == 0 ||
		strcmp(strCap, XN_CAPABILITY_SATURATION) == 0 ||
		strcmp(strCap, XN_CAPABILITY_SHARPNESS) == 0 ||
		strcmp(strCap, XN_CAPABILITY_COLOR_TEMPERATURE) == 0 ||
		strcmp(strCap, XN_CAPABILITY_BACKLIGHT_COMPENSATION) == 0 ||
		strcmp(strCap, XN_CAPABILITY_GAIN) == 0 ||
		strcmp(strCap, XN_CAPABILITY_ZOOM) == 0 ||
		strcmp(strCap, XN_CAPABILITY_EXPOSURE) == 0 ||
		strcmp(strCap, XN_CAPABILITY_PAN) == 0 ||
		strcmp(strCap, XN_CAPABILITY_TILT) == 0 ||
		strcmp(strCap, XN_CAPABILITY_LOW_LIGHT_COMPENSATION) == 0)
	{
		return this;
	}
	else
	{
		return NULL;
	}
}

XnStatus XnSensorImageGenerator::GetRange( const XnChar* strCap, XnInt32& nMin, XnInt32& nMax, XnInt32& nStep, XnInt32& nDefault, XnBool& bIsAutoSupported )
{
	if (strcmp(strCap, XN_CAPABILITY_BRIGHTNESS) == 0)	
	{
		nMin = 0;
		nMax = 255;
		nStep = 1;
		nDefault = 128;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_CONTRAST) == 0)	
	{
		nMin = 0;
		nMax = 255;
		nStep = 1;
		nDefault = 32;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_SATURATION) == 0)	
	{
		nMin = 0;
		nMax = 255;
		nStep = 1;
		nDefault = 128;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_SHARPNESS) == 0)	
	{
		nMin = 0;
		nMax = 255;
		nStep = 1;
		nDefault = 32;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_COLOR_TEMPERATURE) == 0)	
	{
		nMin = 0;
		nMax = 10000;
		nStep = 1;
		nDefault = 5000;
		bIsAutoSupported = TRUE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_BACKLIGHT_COMPENSATION) == 0)	
	{
		nMin = 0;
		nMax = 3;
		nStep = 1;
		nDefault = 1;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_GAIN) == 0)	
	{
		nMin = 0;
		nMax = 255;
		nStep = 1;
		nDefault = 128;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_ZOOM) == 0)	
	{
		nMin = 100;
		nMax = 200;
		nStep = 10;
		nDefault = 100;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_EXPOSURE) == 0)	
	{
		nMin = 5;
		nMax = 2000;
		nStep = 1;
		nDefault = 100;
		bIsAutoSupported = TRUE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_PAN) == 0)	
	{
		nMin = -180;
		nMax = 180;
		nStep = 1;
		nDefault = 0;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_TILT) == 0)	
	{
		nMin = -180;
		nMax = 180;
		nStep = 1;
		nDefault = 0;
		bIsAutoSupported = FALSE;
	}
	else if (strcmp(strCap, XN_CAPABILITY_LOW_LIGHT_COMPENSATION) == 0)	
	{
		nMin = 0;
		nMax = 1;
		nStep = 1;
		nDefault = 1;
		bIsAutoSupported = FALSE;
	}
	else
	{
		return XN_STATUS_NO_SUCH_PROPERTY;
	}

	return XN_STATUS_OK;
}

XnStatus XnSensorImageGenerator::Get( const XnChar* strCap, XnInt32& nValue )
{
	XnUInt64 nValue64;
	XnStatus nRetVal = GetIntProperty(strCap, nValue64);
	XN_IS_STATUS_OK(nRetVal);

	nValue = (XnInt32)nValue64;

	return XN_STATUS_OK;
}

XnInt32 XnSensorImageGenerator::Set( const XnChar* strCap, XnInt32 nValue )
{
	return SetIntProperty(strCap, nValue);
}

XnStatus XnSensorImageGenerator::RegisterToValueChange( const XnChar* strCap, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	const XnChar* strProps[] = 
	{
		strCap,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, strProps);
}

void XnSensorImageGenerator::UnregisterFromValueChange( const XnChar* /*strCap*/, XnCallbackHandle hCallback )
{
	UnregisterFromProps(hCallback);
}

XnPowerLineFrequency XnSensorImageGenerator::GetPowerLineFrequency()
{
	XnUInt64 nFlicker;
	GetIntProperty(XN_STREAM_PROPERTY_FLICKER, nFlicker);
	return (XnPowerLineFrequency)nFlicker;
}

XnStatus XnSensorImageGenerator::SetPowerLineFrequency( XnPowerLineFrequency nFrequency )
{
	return SetIntProperty(XN_STREAM_PROPERTY_FLICKER, nFrequency);
}

XnStatus XnSensorImageGenerator::RegisterToPowerLineFrequencyChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	const XnChar* strProps[] = 
	{
		XN_STREAM_PROPERTY_FLICKER,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, strProps);
}

void XnSensorImageGenerator::UnregisterFromPowerLineFrequencyChange( XnCallbackHandle hCallback )
{
	return UnregisterFromProps(hCallback);
}

//---------------------------------------------------------------------------
// XnExportedSensorImageGenerator class
//---------------------------------------------------------------------------
XnExportedSensorImageGenerator::XnExportedSensorImageGenerator() :
	XnExportedSensorGenerator(XN_NODE_TYPE_IMAGE, XN_STREAM_TYPE_IMAGE, FALSE)
{}

XnStatus XnExportedSensorImageGenerator::IsSupportedForDevice(xn::Context& context, xn::NodeInfo& sensorInfo, XnBool* pbSupported)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnExportedSensorGenerator::IsSupportedForDevice(context, sensorInfo, pbSupported);
	XN_IS_STATUS_OK(nRetVal);

	if (*pbSupported == FALSE)
	{
		return XN_STATUS_OK;
	}

	xn::Device sensor;
	nRetVal = sensorInfo.GetInstance(sensor);
	XN_IS_STATUS_OK(nRetVal);

	XnBool bShouldBeCreated = (!sensor.IsValid());

	if (bShouldBeCreated)
	{
		nRetVal = context.CreateProductionTree(sensorInfo, sensor);
		XN_IS_STATUS_OK(nRetVal);
	}

	// This is an ugly patch to find out if this sensor has an image CMOS. It will be fixed
	// in future firmwares so we can just ask.
	XnCmosBlankingUnits units;
	units.nCmosID = XN_CMOS_TYPE_IMAGE;
	nRetVal = sensor.GetGeneralProperty(XN_MODULE_PROPERTY_CMOS_BLANKING_UNITS, sizeof(units), &units);
	if (nRetVal != XN_STATUS_OK || units.nUnits == 0)
	{
		// Failed. this means no image CMOS
		*pbSupported = FALSE;
	}

	if (bShouldBeCreated)
	{
		sensor.Release();
	}

	return (XN_STATUS_OK);
}

XnSensorGenerator* XnExportedSensorImageGenerator::CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName)
{
	return XN_NEW(XnSensorImageGenerator, context, sensor, pSensor, strStreamName);
}
