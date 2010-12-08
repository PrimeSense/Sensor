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
#include "XnDepthStream.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEPTH_STREAM_MAX_DEPTH_VALUE				XN_MAX_UINT16
#define XN_DEPTH_STREAM_DEFAULT_PIXEL_SIZE_FACTOR	1

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDepthStream::XnDepthStream(const XnChar* csName, XnBool bAllowCustomResolutions, XnDepthPixel nDeviceMaxDepth, XnUInt16 nDeviceMaxShift) :
	XnPixelStream(XN_STREAM_TYPE_DEPTH, csName, bAllowCustomResolutions),
	m_MinDepth(XN_STREAM_PROPERTY_MIN_DEPTH),
	m_MaxDepth(XN_STREAM_PROPERTY_MAX_DEPTH, nDeviceMaxDepth),
	m_ConstShift(XN_STREAM_PROPERTY_CONST_SHIFT),
	m_PixelSizeFactor(XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR, 1),
	m_MaxShift(XN_STREAM_PROPERTY_MAX_SHIFT, nDeviceMaxShift),
	m_DeviceMaxDepth(XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, nDeviceMaxDepth),
	m_ParamCoefficient(XN_STREAM_PROPERTY_PARAM_COEFF),
	m_ShiftScale(XN_STREAM_PROPERTY_SHIFT_SCALE),
	m_ZeroPlaneDistance(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE),
	m_ZeroPlanePixelSize(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE),
	m_EmitterDCmosDistance(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE),
	m_NoDepthValue(XN_STREAM_PROPERTY_NO_SAMPLE),
	m_ShadowValue(XN_STREAM_PROPERTY_SHADOW),
	m_S2DHelper(this)
{
	m_MinDepth.UpdateSetCallback(SetMinDepthCallback, this);
	m_MaxDepth.UpdateSetCallback(SetMaxDepthCallback, this);
}

XnStatus XnDepthStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnPixelStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_MinDepth, &m_MaxDepth, &m_ConstShift, &m_PixelSizeFactor,
		&m_MaxShift, &m_ParamCoefficient, &m_ShiftScale, &m_ZeroPlaneDistance, &m_ZeroPlanePixelSize,
		&m_EmitterDCmosDistance, &m_NoDepthValue, &m_ShadowValue, &m_DeviceMaxDepth);

	nRetVal = m_S2DHelper.Init();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDepthStream::Free()
{
	XnPixelStream::Free();
	return (XN_STATUS_OK);
}

XnStatus XnDepthStream::SetMinDepth(XnDepthPixel nMinDepth)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (nMinDepth > GetDeviceMaxDepth())
	{
		return XN_STATUS_DEVICE_BAD_PARAM;
	}

	nRetVal = m_MinDepth.UnsafeUpdateValue(nMinDepth);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDepthStream::SetMaxDepth(XnDepthPixel nMaxDepth)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (nMaxDepth > GetDeviceMaxDepth())
	{
		return XN_STATUS_DEVICE_BAD_PARAM;
	}

	nRetVal = m_MaxDepth.UnsafeUpdateValue(nMaxDepth);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDepthStream::ValidateDepthValue(XnDepthPixel nDepth)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (nDepth < 0 || nDepth > GetDeviceMaxDepth())
	{
		return XN_STATUS_DEVICE_BAD_PARAM;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDepthStream::OnOutputFormatChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// update no-depth and shadow values
	XnDepthPixel nNoValue;
	XnDepthPixel nShadowValue;

	switch (GetOutputFormat())
	{
	case XN_OUTPUT_FORMAT_SHIFT_VALUES:
		nNoValue = 2047;
		nShadowValue = 0;
		break;
	case XN_OUTPUT_FORMAT_DEPTH_VALUES:
		nNoValue = 0;
		nShadowValue = 1;
		break;
	default:
		XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "DepthStream: Unknown output format!")
	}

	nRetVal = m_NoDepthValue.UnsafeUpdateValue(nNoValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_ShadowValue.UnsafeUpdateValue(nShadowValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnDepthStream::SetMinDepthCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDepthStream* pStream = (XnDepthStream*)pCookie;
	return pStream->SetMinDepth((XnDepthPixel)nValue);	
}

XnStatus XN_CALLBACK_TYPE XnDepthStream::SetMaxDepthCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDepthStream* pStream = (XnDepthStream*)pCookie;
	return pStream->SetMaxDepth((XnDepthPixel)nValue);	
}

XnStatus XN_CALLBACK_TYPE XnDepthStream::OutputFormatValueChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnDepthStream* pStream = (XnDepthStream*)pCookie;
	return pStream->OnOutputFormatChanged();
}
