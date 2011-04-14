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
	XnSensorGenerator(context, sensor, pSensor, strStreamName),
	m_nSupportedModesCount(0),
	m_aSupportedModes(NULL)
{}

XnStatus XnSensorMapGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	// get supported modes
	XnUInt64 nCount;
	nRetVal = GetIntProperty(XN_STREAM_PROPERTY_SUPPORT_MODES_COUNT, nCount);
	XN_IS_STATUS_OK(nRetVal);

	m_aSupportedModes = (SupportedMode*)xnOSMalloc(sizeof(SupportedMode) * nCount);
	XN_VALIDATE_ALLOC_PTR(m_aSupportedModes);
	m_nSupportedModesCount = nCount;

	const XnUInt32 nAllocCount = 150;
	XnCmosPreset aPresets[nAllocCount];
	XN_ASSERT(nAllocCount >= m_nSupportedModesCount);
	nRetVal = GetGeneralProperty(XN_STREAM_PROPERTY_SUPPORT_MODES, m_nSupportedModesCount*sizeof(XnCmosPreset), aPresets);
	XN_IS_STATUS_OK(nRetVal);

	// Keep those modes
	XnBool bOK = TRUE;
	for (XnUInt32 i = 0; i < m_nSupportedModesCount; ++i)
	{
		m_aSupportedModes[i].nInputFormat = aPresets[i].nFormat;
		bOK = XnDDKGetXYFromResolution((XnResolutions)aPresets[i].nResolution, &m_aSupportedModes[i].OutputMode.nXRes, &m_aSupportedModes[i].OutputMode.nYRes);
		XN_ASSERT(bOK);
		m_aSupportedModes[i].OutputMode.nFPS = aPresets[i].nFPS;
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
	return m_nSupportedModesCount;
}

XnStatus XnSensorMapGenerator::GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(aModes);

	if (nCount < m_nSupportedModesCount)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	XnUInt32 i = 0;
	for (XnUInt32 i = 0; i < m_nSupportedModesCount; ++i)
	{
		aModes[i] = m_aSupportedModes[i].OutputMode;
	}

	nCount = m_nSupportedModesCount;

	return (XN_STATUS_OK);
}

XnBool Equals(const XnMapOutputMode& mode1, const XnMapOutputMode& mode2)
{
	return (
		mode1.nXRes == mode2.nXRes &&
		mode1.nYRes == mode2.nYRes &&
		mode1.nFPS == mode2.nFPS);
}

XnStatus XnSensorMapGenerator::SetMapOutputMode(const XnMapOutputMode& Mode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnMapOutputMode current;
	GetMapOutputMode(current);

	if (Equals(current, Mode))
	{
		return (XN_STATUS_OK);
	}

	// check if this mode is supported. If it is, make sure current input format is OK
	XnUInt64 nCurrInputFormat;
	nRetVal = GetIntProperty(XN_STREAM_PROPERTY_INPUT_FORMAT, nCurrInputFormat);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt32 nChosenInputFormat = XN_MAX_UINT32;

	for (XnUInt32 i = 0; i < m_nSupportedModesCount; ++i)
	{
		if (Equals(Mode, m_aSupportedModes[i].OutputMode))
		{
			// if current input format is supported, it will always be preferred.
			if (m_aSupportedModes[i].nInputFormat == nCurrInputFormat)
			{
				nChosenInputFormat = nCurrInputFormat;
				break;
			}
			else if (nChosenInputFormat == XN_MAX_UINT32)
			{
				nChosenInputFormat = m_aSupportedModes[i].nInputFormat;
				// don't break yet. we might find our input format
			}
		}
	}

	if (nChosenInputFormat == XN_MAX_UINT32) // not found
	{
		return XN_STATUS_BAD_PARAM;
	}

	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	XnPropertySetAddModule(&props, m_strModule);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_X_RES, Mode.nXRes);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_Y_RES, Mode.nYRes);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_FPS, Mode.nFPS);

	if (nChosenInputFormat != nCurrInputFormat)
	{
		XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_INPUT_FORMAT, nChosenInputFormat);
	}

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
