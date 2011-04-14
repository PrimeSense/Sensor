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
#include "XnImageProcessor.h"
#include "XnSensor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnImageProcessor::XnImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper, XnBool bCompressedOutput /* = FALSE */) :
	XnFrameStreamProcessor(pStream, pHelper, XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_START, XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_END),
	m_bCompressedOutput(bCompressedOutput)
{
}

XnImageProcessor::~XnImageProcessor()
{
	// unregister from properties (otherwise, callbacks will be called with deleted pointer...)
	GetStream()->XResProperty().OnChangeEvent().Unregister(m_hXResCallback);
	GetStream()->YResProperty().OnChangeEvent().Unregister(m_hYResCallback);
	GetStream()->m_FirmwareCropSizeX.OnChangeEvent().Unregister(m_hXCropCallback);
	GetStream()->m_FirmwareCropSizeY.OnChangeEvent().Unregister(m_hYCropCallback);
	GetStream()->m_FirmwareCropEnabled.OnChangeEvent().Unregister(m_hCropEnabledCallback);
}

XnStatus XnImageProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnFrameStreamProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = GetStream()->XResProperty().OnChangeEvent().Register(ActualResChangedCallback, this, &m_hXResCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetStream()->YResProperty().OnChangeEvent().Register(ActualResChangedCallback, this, &m_hYResCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetStream()->m_FirmwareCropSizeX.OnChangeEvent().Register(ActualResChangedCallback, this, &m_hXCropCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetStream()->m_FirmwareCropSizeY.OnChangeEvent().Register(ActualResChangedCallback, this, &m_hYCropCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetStream()->m_FirmwareCropEnabled.OnChangeEvent().Register(ActualResChangedCallback, this, &m_hCropEnabledCallback);
	XN_IS_STATUS_OK(nRetVal);

	CalcActualRes();

	return (XN_STATUS_OK);
}

XnUInt32 XnImageProcessor::CalculateExpectedSize()
{
	XnUInt32 nExpectedDepthBufferSize = GetStream()->GetXRes() * GetStream()->GetYRes();

	// when cropping is turned on, actual depth size is smaller
	if (GetStream()->m_FirmwareCropEnabled.GetValue() == TRUE)
	{
		nExpectedDepthBufferSize = (XnUInt32)(GetStream()->m_FirmwareCropSizeX.GetValue() * GetStream()->m_FirmwareCropSizeY.GetValue());
	}

	nExpectedDepthBufferSize *= GetStream()->GetBytesPerPixel();

	return nExpectedDepthBufferSize;
}

void XnImageProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	if (!m_bCompressedOutput)
	{
		// make sure data size is right
		XnUInt32 nExpectedSize = CalculateExpectedSize();
		if (GetWriteBuffer()->GetSize() != nExpectedSize)
		{
			xnLogWarning(XN_MASK_SENSOR_READ, "Read: Image buffer is corrupt. Size is %u (!= %u)", GetWriteBuffer()->GetSize(), nExpectedSize);
			FrameIsCorrupted();
		}
	}

	// call base
	XnFrameStreamProcessor::OnEndOfFrame(pHeader);
}

void XnImageProcessor::OnFrameReady(XnUInt32 nFrameID, XnUInt64 nFrameTS)
{
	XnFrameStreamProcessor::OnFrameReady(nFrameID, nFrameTS);

	m_pDevicePrivateData->pSensor->GetFPSCalculator()->MarkInputImage(nFrameID, nFrameTS);
}

void XnImageProcessor::CalcActualRes()
{
	if (GetStream()->m_FirmwareCropEnabled.GetValue() == TRUE)
	{
		m_nActualXRes = (XnUInt32)GetStream()->m_FirmwareCropSizeX.GetValue();
		m_nActualYRes = (XnUInt32)GetStream()->m_FirmwareCropSizeY.GetValue();
	}
	else
	{
		m_nActualXRes = GetStream()->GetXRes();
		m_nActualYRes = GetStream()->GetYRes();
	}
}

XnStatus XnImageProcessor::ActualResChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnImageProcessor* pThis = (XnImageProcessor*)pCookie;
	pThis->CalcActualRes();
	return XN_STATUS_OK;
}

