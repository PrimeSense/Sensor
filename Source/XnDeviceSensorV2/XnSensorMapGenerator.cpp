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
#include "XnSensorMapGenerator.h"

//---------------------------------------------------------------------------
// XnSensorMapGenerator class
//---------------------------------------------------------------------------
XnSensorMapGenerator::XnSensorMapGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorGenerator(context, sensor, pSensor, strStreamName)
{}

XnStatus XnSensorMapGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	XnMapOutputMode mode;

	// QVGA 30
	mode.nXRes = XN_QVGA_X_RES;
	mode.nYRes = XN_QVGA_Y_RES;
	mode.nFPS = 30;
	nRetVal = m_SupportedModes.AddLast(mode);
	XN_IS_STATUS_OK(nRetVal);

	// QVGA 60
	mode.nXRes = XN_QVGA_X_RES;
	mode.nYRes = XN_QVGA_Y_RES;
	mode.nFPS = 60;
	nRetVal = m_SupportedModes.AddLast(mode);
	XN_IS_STATUS_OK(nRetVal);

	// VGA 30
	mode.nXRes = XN_VGA_X_RES;
	mode.nYRes = XN_VGA_Y_RES;
	mode.nFPS = 30;
	nRetVal = m_SupportedModes.AddLast(mode);
	XN_IS_STATUS_OK(nRetVal);

	if (m_Version.FWVer >= XN_SENSOR_FW_VER_5_2)
	{
		// QVGA 25
		mode.nXRes = XN_QVGA_X_RES;
		mode.nYRes = XN_QVGA_Y_RES;
		mode.nFPS = 25;
		nRetVal = m_SupportedModes.AddLast(mode);
		XN_IS_STATUS_OK(nRetVal);

		// VGA 25
		mode.nXRes = XN_VGA_X_RES;
		mode.nYRes = XN_VGA_Y_RES;
		mode.nFPS = 25;
		nRetVal = m_SupportedModes.AddLast(mode);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnBool XnSensorMapGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return (strcmp(strCapabilityName, XN_CAPABILITY_CROPPING) == 0 ||
		XnSensorGenerator::IsCapabilitySupported(strCapabilityName));
}

XnUInt32 XnSensorMapGenerator::GetSupportedMapOutputModesCount()
{
	return m_SupportedModes.Size();
}

XnStatus XnSensorMapGenerator::GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(aModes);

	if (nCount < m_SupportedModes.Size())
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	XnUInt32 i = 0;
	for (XnMapOutputModeList::Iterator it = m_SupportedModes.begin(); it != m_SupportedModes.end(); ++it, ++i)
	{
		aModes[i] = *it;
	}

	nCount = m_SupportedModes.Size();

	return (XN_STATUS_OK);
}

XnStatus XnSensorMapGenerator::SetMapOutputMode(const XnMapOutputMode& Mode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnMapOutputMode current;
	GetMapOutputMode(current);

	if (current.nFPS == Mode.nFPS &&
		current.nXRes == Mode.nXRes &&
		current.nYRes == Mode.nYRes)
	{
		return (XN_STATUS_OK);
	}

	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	XnPropertySetAddModule(&props, m_strModule);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_X_RES, Mode.nXRes);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_Y_RES, Mode.nYRes);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_FPS, Mode.nFPS);

	nRetVal = m_pSensor->BatchConfig(&props);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorMapGenerator::GetMapOutputMode(XnMapOutputMode& OutputMode)
{
	XnUInt64 nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_X_RES, &nValue);
	OutputMode.nXRes = (XnUInt32)nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_Y_RES, &nValue);
	OutputMode.nYRes = (XnUInt32)nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_FPS, &nValue);
	OutputMode.nFPS = (XnUInt32)nValue;

	return XN_STATUS_OK;
}

XnStatus XnSensorMapGenerator::RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	// don't register to xRes and yRes. If we did so, we would get two notifications when
	// resolution changed.
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_RESOLUTION,
		XN_STREAM_PROPERTY_FPS,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorMapGenerator::UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

XnStatus XnSensorMapGenerator::SetCropping(const XnCropping &Cropping)
{
	XnGeneralBuffer gbValue = XnGeneralBufferPack((void*)&Cropping, sizeof(Cropping));
	return m_pSensor->SetProperty(m_strModule, XN_STREAM_PROPERTY_CROPPING, gbValue);
}

XnStatus XnSensorMapGenerator::GetCropping(XnCropping &Cropping)
{
	return m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_CROPPING, XN_PACK_GENERAL_BUFFER(Cropping));
}

XnStatus XnSensorMapGenerator::RegisterToCroppingChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_CROPPING,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorMapGenerator::UnregisterFromCroppingChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorMapGenerator::FilterProperties(XnActualPropertiesHash* pHash)
{
	XnSensorGenerator::FilterProperties(pHash);
	pHash->Remove(XN_STREAM_PROPERTY_X_RES);
	pHash->Remove(XN_STREAM_PROPERTY_Y_RES);
	pHash->Remove(XN_STREAM_PROPERTY_FPS);
	pHash->Remove(XN_STREAM_PROPERTY_CROPPING);
}
