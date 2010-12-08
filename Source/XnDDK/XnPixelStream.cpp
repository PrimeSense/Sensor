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
#include "XnPixelStream.h"
#include <XnLog.h>
#include <XnFormats/XnFormats.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// XnPixelStream
//---------------------------------------------------------------------------
XnPixelStream::XnPixelStream(const XnChar* csType, const XnChar* csName, XnBool bAllowCustomResolutions) :
	XnFrameStream(csType, csName),
	m_IsPixelStream(XN_STREAM_PROPERTY_IS_PIXEL_BASED, TRUE),
	m_Resolution(XN_STREAM_PROPERTY_RESOLUTION, XN_RESOLUTION_VGA),
	m_XRes(XN_STREAM_PROPERTY_X_RES, XN_VGA_X_RES),
	m_YRes(XN_STREAM_PROPERTY_Y_RES, XN_VGA_Y_RES),
	m_BytesPerPixel(XN_STREAM_PROPERTY_BYTES_PER_PIXEL),
	m_Cropping(XN_STREAM_PROPERTY_CROPPING, &m_CroppingData, sizeof(XnCropping), ReadCroppingFromFileCallback),
	m_bAllowCustomResolutions(bAllowCustomResolutions)
{
	xnOSMemSet(&m_CroppingData, 0, sizeof(XnCropping));
}

XnStatus XnPixelStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnFrameStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	// update set callbacks
	m_Resolution.UpdateSetCallback(SetResolutionCallback, this);
	m_XRes.UpdateSetCallback(SetXResCallback, this);
	m_YRes.UpdateSetCallback(SetYResCallback, this);
	m_Cropping.UpdateSetCallback(SetCroppingCallback, this);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_IsPixelStream, &m_Resolution, &m_XRes, &m_YRes, &m_BytesPerPixel, &m_Cropping);

	// register required size properties
	nRetVal = RegisterRequiredSizeProperty(&m_XRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RegisterRequiredSizeProperty(&m_YRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RegisterRequiredSizeProperty(&m_BytesPerPixel);
	XN_IS_STATUS_OK(nRetVal);

	// register for important properties
	XnCallbackHandle hDummyCallback;
	nRetVal = m_Resolution.OnChangeEvent().Register(ResolutionValueChangedCallback, this, &hDummyCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = OutputFormatProperty().OnChangeEvent().Register(OutputFormatValueChangedCallback, this, &hDummyCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_XRes.OnChangeEvent().Register(FixCroppingCallback, this, &hDummyCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_YRes.OnChangeEvent().Register(FixCroppingCallback, this, &hDummyCallback);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::SetResolution(XnResolutions nResolution)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Resolution.UnsafeUpdateValue(nResolution);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::SetXRes(XnUInt32 nXRes)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_bAllowCustomResolutions)
	{
		// just update the value
		nRetVal = m_XRes.UnsafeUpdateValue(nXRes);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// instead of updating XRes, we'll update the resolution, and XRes will be updated
		// via ValueChanged event
		XnResolutions nRes;

		switch (nXRes)
		{
		case XN_QVGA_X_RES:
			nRes = XN_RESOLUTION_QVGA;
			break;
		case XN_VGA_X_RES:
			nRes = XN_RESOLUTION_VGA;
			break;
		case XN_SXGA_X_RES:
			nRes = XN_RESOLUTION_SXGA;
			break;
		case XN_UXGA_X_RES:
			nRes = XN_RESOLUTION_UXGA;
			break;
		default:
			return (XN_STATUS_DEVICE_BAD_PARAM);
		}		

		nRetVal = SetResolution(nRes);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::SetYRes(XnUInt32 nYRes)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_bAllowCustomResolutions)
	{
		// just update the value
		nRetVal = m_YRes.UnsafeUpdateValue(nYRes);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// instead of updating YRes, we'll update the resolution, and YRes will be updated
		// via ValueChanged event
		XnResolutions nRes;

		switch (nYRes)
		{
		case XN_QVGA_Y_RES:
			nRes = XN_RESOLUTION_QVGA;
			break;
		case XN_VGA_Y_RES:
			nRes = XN_RESOLUTION_VGA;
			break;
		case XN_SXGA_Y_RES:
			nRes = XN_RESOLUTION_SXGA;
			break;
		case XN_UXGA_Y_RES:
			nRes = XN_RESOLUTION_UXGA;
			break;
		default:
			return (XN_STATUS_DEVICE_BAD_PARAM);
		}		

		nRetVal = SetResolution(nRes);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::SetCropping(const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateCropping(pCropping);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Cropping.UnsafeUpdateValue(XN_PACK_GENERAL_BUFFER(*(XnCropping*)pCropping));
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::ValidateCropping(const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pCropping->bEnabled)
	{
		if (pCropping->nXOffset > GetXRes() ||
			XnUInt32(pCropping->nXOffset + pCropping->nXSize) > GetXRes() ||
			pCropping->nYOffset > GetYRes() ||
			XnUInt32(pCropping->nYOffset + pCropping->nYSize) > GetYRes())
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DDK, "Cropping values do not match stream resolution!");
		}

		if (pCropping->nXSize == 0 || pCropping->nYSize == 0)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DDK, "Cannot set a cropping window of zero size!");
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::OnResolutionChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Resolution.GetValue() != XN_RESOLUTION_CUSTOM)
	{
		// update XRes and YRes accordingly
		XnUInt32 nXRes;
		XnUInt32 nYRes;

		switch (m_Resolution.GetValue())
		{
		case XN_RESOLUTION_QVGA:
			nXRes = XN_QVGA_X_RES;
			nYRes = XN_QVGA_Y_RES;
			break;
		case XN_RESOLUTION_VGA:
			nXRes = XN_VGA_X_RES;
			nYRes = XN_VGA_Y_RES;
			break;
		case XN_RESOLUTION_UXGA:
			nXRes = XN_UXGA_X_RES;
			nYRes = XN_UXGA_Y_RES;
			break;
		case XN_RESOLUTION_SXGA:
			nXRes = XN_SXGA_X_RES;
			nYRes = XN_SXGA_Y_RES;
			break;
		}

		nRetVal = m_XRes.UnsafeUpdateValue(nXRes);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_YRes.UnsafeUpdateValue(nYRes);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::OnOutputFormatChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// update the bytes-per-pixel value
	XnUInt32 nBytesPerPixel;

	switch (GetOutputFormat())
	{
	case XN_OUTPUT_FORMAT_SHIFT_VALUES:
		nBytesPerPixel = sizeof(XnUInt16);
		break;
	case XN_OUTPUT_FORMAT_DEPTH_VALUES:
		nBytesPerPixel = sizeof(XnDepthPixel);
		break;
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
		nBytesPerPixel = sizeof(XnUInt8);
		break;
	case XN_OUTPUT_FORMAT_GRAYSCALE16:
		nBytesPerPixel = sizeof(XnUInt16);
		break;
	case XN_OUTPUT_FORMAT_YUV422:
		// YUV422 is actually 4 bytes for every 2 pixels
		nBytesPerPixel = sizeof(XnUChar) * 2;
		break;
	case XN_OUTPUT_FORMAT_RGB24:
		nBytesPerPixel = sizeof(XnUChar) * 3;
		break;
	case XN_OUTPUT_FORMAT_JPEG:
		// size is unknown.
		nBytesPerPixel = 1;
		break;
	default:
		return (XN_STATUS_DEVICE_BAD_PARAM);
	}

	nRetVal = m_BytesPerPixel.UnsafeUpdateValue(nBytesPerPixel);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::FixCropping()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnCropping cropping = *GetCropping();
	if (cropping.nXOffset > GetXRes() || 
		cropping.nYOffset > GetYRes() ||
		XnUInt32(cropping.nXOffset + cropping.nXSize) > GetXRes() ||
		XnUInt32(cropping.nYOffset + cropping.nYSize) > GetYRes())
	{
		// disable it
		cropping.bEnabled = FALSE;
		nRetVal = SetCropping(&cropping);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::CalcRequiredSize(XnUInt32* pnRequiredSize) const
{
	*pnRequiredSize = GetXRes() * GetYRes() * GetBytesPerPixel();
	return XN_STATUS_OK;
}

XnStatus XnPixelStream::ReadImpl(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first read
	nRetVal = XnFrameStream::ReadImpl(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	// now crop
	xnOSEnterCriticalSection(GetLock());
	XnCropping cropping = *GetCropping();
	xnOSLeaveCriticalSection(GetLock());

	if (cropping.bEnabled)
	{
		nRetVal = CropImpl(pStreamOutput, &cropping);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnPixelStream::Mirror(XnStreamData* pStreamOutput) const
{
	XnUInt32 nXRes = GetCropping()->bEnabled ? GetCropping()->nXSize : GetXRes();
	return XnFormatsMirrorPixelData(GetOutputFormat(), (XnUChar*)pStreamOutput->pData, pStreamOutput->nDataSize, nXRes);
}

XnStatus XnPixelStream::CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUChar* pPixelData = (XnUChar*)pStreamOutput->pData;
	XnUInt32 nCurDataSize = 0;

	for (XnUInt32 y = pCropping->nYOffset; y < XnUInt32(pCropping->nYOffset + pCropping->nYSize); ++y)
	{
		XnUChar* pOrigLine = &pPixelData[y * GetXRes() * GetBytesPerPixel()];

		// move line
		xnOSMemCopy(pPixelData + nCurDataSize, pOrigLine + pCropping->nXOffset * GetBytesPerPixel(), pCropping->nXSize * GetBytesPerPixel());
		nCurDataSize += pCropping->nXSize * GetBytesPerPixel();
	}

	// update size
	pStreamOutput->nDataSize = nCurDataSize;

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::ResolutionValueChangedCallback(const XnProperty* pSenser, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->OnResolutionChanged();
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::OutputFormatValueChangedCallback(const XnProperty* pSenser, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->OnOutputFormatChanged();
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::FixCroppingCallback(const XnProperty* pSenser, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->FixCropping();
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::SetResolutionCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->SetResolution((XnResolutions)nValue);
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::SetXResCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->SetXRes((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::SetYResCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	return pStream->SetYRes((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::SetCroppingCallback(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XnPixelStream* pStream = (XnPixelStream*)pCookie;
	if (gbValue.nDataSize != sizeof(XnCropping))
	{
		return XN_STATUS_DEVICE_PROPERTY_SIZE_DONT_MATCH;
	}

	return pStream->SetCropping((XnCropping*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnPixelStream::ReadCroppingFromFileCallback(XnGeneralProperty* pSender, const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read section name
	XnChar csCroppingSection[XN_FILE_MAX_PATH];
	sprintf(csCroppingSection, "%s.Cropping", csSection);

	// read cropping values
	XnUInt32 nOffsetX;
	XnUInt32 nOffsetY;
	XnUInt32 nSizeX;
	XnUInt32 nSizeY;
	XnUInt32 bEnabled;

	// only if all values are here
	if (XN_STATUS_OK == xnOSReadIntFromINI(csINIFile, csCroppingSection, "OffsetX", &nOffsetX) &&
		XN_STATUS_OK == xnOSReadIntFromINI(csINIFile, csCroppingSection, "OffsetY", &nOffsetY) &&
		XN_STATUS_OK == xnOSReadIntFromINI(csINIFile, csCroppingSection, "SizeX", &nSizeX) &&
		XN_STATUS_OK == xnOSReadIntFromINI(csINIFile, csCroppingSection, "SizeY", &nSizeY) &&
		XN_STATUS_OK == xnOSReadIntFromINI(csINIFile, csCroppingSection, "Enabled", &bEnabled))
	{
		XnCropping Cropping;
		Cropping.nXOffset = nOffsetX;
		Cropping.nYOffset = nOffsetY;
		Cropping.nXSize = nSizeX;
		Cropping.nYSize = nSizeY;
		Cropping.bEnabled = bEnabled;

		// set value
		nRetVal = pSender->SetValue(XN_PACK_GENERAL_BUFFER(Cropping));
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}
