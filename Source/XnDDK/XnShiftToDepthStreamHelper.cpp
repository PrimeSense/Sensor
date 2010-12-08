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




#include "XnShiftToDepthStreamHelper.h"

XnShiftToDepthStreamHelper::XnShiftToDepthStreamHelper(XnDeviceModule* pModule) :
	m_ShiftToDepthTable(XN_STREAM_PROPERTY_S2D_TABLE, NULL, 0, NULL),
	m_DepthToShiftTable(XN_STREAM_PROPERTY_D2S_TABLE, NULL, 0, NULL),
	m_pModule(pModule),
	m_bPropertiesAdded(FALSE)
{
	m_ShiftToDepthTable.UpdateGetCallback(GetShiftToDepthTableCallback, this);
	m_DepthToShiftTable.UpdateGetCallback(GetDepthToShiftTableCallback, this);
	xnOSMemSet(&m_ShiftToDepthTables, 0, sizeof(XnShiftToDepthTables));
}

XnShiftToDepthStreamHelper::~XnShiftToDepthStreamHelper()
{
	XnShiftToDepthStreamHelper::Free();
}

XnStatus XnShiftToDepthStreamHelper::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// old depth streams did not have S2D tables as actual properties. Add these properties
	XnBool bDoesExist = FALSE;
	nRetVal = m_pModule->DoesPropertyExist(XN_STREAM_PROPERTY_S2D_TABLE, &bDoesExist);
	XN_IS_STATUS_OK(nRetVal);

	if (!bDoesExist)
	{
		// add properties to the module
		XN_VALIDATE_ADD_PROPERTIES(m_pModule, &m_ShiftToDepthTable, &m_DepthToShiftTable);

		m_bPropertiesAdded = TRUE;

		// now create tables and register to properties
		nRetVal = InitShiftToDepth();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		m_ShiftToDepthTables.pShiftToDepthTable = (XnDepthPixel*)m_ShiftToDepthTable.GetValue().pData;
		m_ShiftToDepthTables.pDepthToShiftTable = (XnUInt16*)m_DepthToShiftTable.GetValue().pData;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::Free()
{
	XnShiftToDepthFree(&m_ShiftToDepthTables);
	return XN_STATUS_OK;
}

XnStatus XnShiftToDepthStreamHelper::InitShiftToDepth()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// register to any shift-to-depth property (so tables can be updated if needed)
	const XnChar* propNames[] = 
	{
		XN_STREAM_PROPERTY_MIN_DEPTH,
		XN_STREAM_PROPERTY_MAX_DEPTH,
		XN_STREAM_PROPERTY_CONST_SHIFT,
		XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR,
		XN_STREAM_PROPERTY_PARAM_COEFF,
		XN_STREAM_PROPERTY_SHIFT_SCALE,
		XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,
		XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,
		XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE
	};

	XnUInt32 nPropCount = sizeof(propNames) / sizeof(const XnChar*);

	XnProperty* pProperty = NULL;
	for (XnUInt32 i = 0; i < nPropCount; ++i)
	{
		nRetVal = m_pModule->GetProperty(propNames[i], &pProperty);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = pProperty->OnChangeEvent().Register(ShiftToDepthPropertyValueChangedCallback, this);
		XN_IS_STATUS_OK(nRetVal);
	}

	// register for tables size properties
	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_MAX_SHIFT, &pProperty);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProperty->OnChangeEvent().Register(DeviceS2DTablesSizeChangedCallback, this);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, &pProperty);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProperty->OnChangeEvent().Register(DeviceS2DTablesSizeChangedCallback, this);
	XN_IS_STATUS_OK(nRetVal);

	// now init the tables
	XnShiftToDepthConfig Config;
	nRetVal = GetShiftToDepthConfig(Config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnShiftToDepthInit(&m_ShiftToDepthTables, &Config);
	XN_IS_STATUS_OK(nRetVal);

	// replace tables buffers
	m_ShiftToDepthTable.ReplaceBuffer(m_ShiftToDepthTables.pShiftToDepthTable, m_ShiftToDepthTables.nShiftsCount * sizeof(XnDepthPixel));
	m_DepthToShiftTable.ReplaceBuffer(m_ShiftToDepthTables.pDepthToShiftTable, m_ShiftToDepthTables.nDepthsCount * sizeof(XnUInt16));

	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::GetShiftToDepthConfig(XnShiftToDepthConfig& Config)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt64 nTemp;
	XnDouble dTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nZeroPlaneDistance = (XnUInt16)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, &dTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.fZeroPlanePixelSize = (XnFloat)dTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, &dTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.fEmitterDCmosDistance = (XnFloat)dTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_MAX_SHIFT, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nDeviceMaxShiftValue = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nDeviceMaxDepthValue = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_CONST_SHIFT, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nConstShift = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nPixelSizeFactor = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_PARAM_COEFF, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nParamCoeff = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_SHIFT_SCALE, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nShiftScale = (XnUInt32)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_MIN_DEPTH, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nDepthMinCutOff = (XnDepthPixel)nTemp;

	nRetVal = m_pModule->GetProperty(XN_STREAM_PROPERTY_MAX_DEPTH, &nTemp);
	XN_IS_STATUS_OK(nRetVal);

	Config.nDepthMaxCutOff = (XnDepthPixel)nTemp;

	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::RaiseChangeEvents()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_ShiftToDepthTable.UnsafeUpdateValue(XnGeneralBufferPack(m_ShiftToDepthTables.pShiftToDepthTable, m_ShiftToDepthTables.nShiftsCount * sizeof(XnDepthPixel)));
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_DepthToShiftTable.UnsafeUpdateValue(XnGeneralBufferPack(m_ShiftToDepthTables.pDepthToShiftTable, m_ShiftToDepthTables.nDepthsCount * sizeof(XnUInt16)));
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::OnShiftToDepthPropertyValueChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnShiftToDepthConfig Config;
	nRetVal = GetShiftToDepthConfig(Config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnShiftToDepthUpdate(&m_ShiftToDepthTables, &Config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RaiseChangeEvents();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::OnDeviceS2DTablesSizeChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// free the tables, and re-init them
	XnShiftToDepthFree(&m_ShiftToDepthTables);

	XnShiftToDepthConfig Config;
	nRetVal = GetShiftToDepthConfig(Config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnShiftToDepthInit(&m_ShiftToDepthTables, &Config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RaiseChangeEvents();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnShiftToDepthStreamHelper::GetShiftToDepthTableImpl(const XnGeneralBuffer& gbValue) const
{
	XnUInt32 nTableSize = m_ShiftToDepthTables.nShiftsCount * sizeof(XnDepthPixel);
	if (gbValue.nDataSize < nTableSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	xnOSMemCopy(gbValue.pData, m_ShiftToDepthTables.pShiftToDepthTable, nTableSize);
	return XN_STATUS_OK;
}

XnStatus XnShiftToDepthStreamHelper::GetDepthToShiftTableImpl(const XnGeneralBuffer& gbValue) const
{
	XnUInt32 nTableSize = m_ShiftToDepthTables.nDepthsCount * sizeof(XnUInt16);
	if (gbValue.nDataSize < nTableSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	xnOSMemCopy(gbValue.pData, m_ShiftToDepthTables.pDepthToShiftTable, nTableSize);
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnShiftToDepthStreamHelper::GetShiftToDepthTableCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XnShiftToDepthStreamHelper* pStream = (XnShiftToDepthStreamHelper*)pCookie;
	return pStream->GetShiftToDepthTableImpl(gbValue);	
}

XnStatus XN_CALLBACK_TYPE XnShiftToDepthStreamHelper::GetDepthToShiftTableCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XnShiftToDepthStreamHelper* pStream = (XnShiftToDepthStreamHelper*)pCookie;
	return pStream->GetDepthToShiftTableImpl(gbValue);	
}

XnStatus XN_CALLBACK_TYPE XnShiftToDepthStreamHelper::ShiftToDepthPropertyValueChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnShiftToDepthStreamHelper* pStream = (XnShiftToDepthStreamHelper*)pCookie;
	return pStream->OnShiftToDepthPropertyValueChanged();
}

XnStatus XN_CALLBACK_TYPE XnShiftToDepthStreamHelper::DeviceS2DTablesSizeChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnShiftToDepthStreamHelper* pStream = (XnShiftToDepthStreamHelper*)pCookie;
	return pStream->OnDeviceS2DTablesSizeChanged();
}

